/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#include <linux/string.h>
#define LCD_DEBUG

#ifdef BUILD_UBOOT
#include <asm/arch/mt6577_gpio.h>
#include <asm/arch/mt6577_pwm.h>
#ifdef LCD_DEBUG
#define LCM_DEBUG(format, ...)   printf("uboot r69329" format "\n", ## __VA_ARGS__)
#else
#define LCM_DEBUG printf
#endif
#else
#include <mach/mt6577_gpio.h>
#include <mach/mt6577_pwm.h>
#ifdef LCD_DEBUG
#define LCM_DEBUG(format, ...)   printk("kernel r69329" format "\n", ## __VA_ARGS__)
#else
#define LCM_DEBUG printk
#endif
#endif

#include "lcm_drv.h"

#ifndef GN_MTK_BSP_LCD_SW_SPI
//	#define GN_MTK_BSP_LCD_SW_SPI
#endif


unsigned int  last_backlight_level=0;
static void config_gpio(void);
static unsigned int lcm_compare_id(void);

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)


static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)  (lcm_util.set_gpio_out((n), (v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------


#define LSDI_GPIO_PIN (GPIO48)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)


#define SET_LSCE_LOW   SET_GPIO_OUT(LSCE_GPIO_PIN, 0)
#define SET_LSCE_HIGH  SET_GPIO_OUT(LSCE_GPIO_PIN, 1)
#define SET_LSCK_LOW   SET_GPIO_OUT(LSCK_GPIO_PIN, 0)
#define SET_LSCK_HIGH  SET_GPIO_OUT(LSCK_GPIO_PIN, 1)
#define SET_LSDA_LOW   SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH  SET_GPIO_OUT(LSDA_GPIO_PIN, 1)
#define GET_HX_SDI	   mt_get_gpio_in(LSDI_GPIO_PIN)


#ifdef GN_MTK_BSP_LCD_SW_SPI

#define LCDSPI_GPIO_CMD LCDSPI_InitCMD
#define LCDSPI_GPIO_DAT LCDSPI_InitDAT

static void LCDSPI_GPIO_CMD(unsigned char cmd)
{
	unsigned int i;

	SET_LSCE_LOW;
	UDELAY(10);     

	SET_LSCK_LOW;
	SET_LSDA_LOW;
	UDELAY(10); 
 
	SET_LSCK_HIGH;
	UDELAY(100);

	for (i = 0; i < 8; i++ ) 
	{
		SET_LSCK_LOW;

		if (cmd & 0x80) 
		{
			SET_LSDA_HIGH;
		} 
		else 
		{
			SET_LSDA_LOW;
		}

		UDELAY(100);  
		SET_LSCK_HIGH;
		UDELAY(100);  

		cmd <<= 1;
	}
	SET_LSCE_HIGH;
	SET_LSCK_HIGH;
}

static void LCDSPI_GPIO_DAT(unsigned char data)
{
	unsigned int i;

	SET_LSCE_LOW;
	UDELAY(10);     

	SET_LSCK_LOW;
	SET_LSDA_HIGH;
	UDELAY(10);  
 
	SET_LSCK_HIGH;
	UDELAY(100);

	for (i = 0; i < 8; i++ ) 
	{
		SET_LSCK_LOW;
		if (data & 0x80) 
		{
			SET_LSDA_HIGH;
		}
		else 
		{
			SET_LSDA_LOW;
		}
		UDELAY(100);   
		SET_LSCK_HIGH;
		UDELAY(100);   
		data <<= 1;
	}

	SET_LSCE_HIGH;
	SET_LSCK_HIGH;
}

#else
static __inline void LCDSPI_InitCMD(unsigned char cmd)
{
    lcm_util.send_cmd(cmd & 0xFF);
    
}
static __inline void LCDSPI_InitDAT(unsigned char reg_Data)
{
    lcm_util.send_data(reg_Data & 0xFF);
}
#endif


static unsigned char LCDSPI_Read(char cmd)
{
	int value = 0; 
	unsigned int i;
	
	SET_LSCK_HIGH;
	SET_LSCE_LOW;
	UDELAY(10);     

	SET_LSCK_LOW;
	SET_LSDA_LOW;
	UDELAY(10); 

	SET_LSCK_HIGH;
	UDELAY(100);


	for (i = 0; i < 8; i++ ) 
	{
		SET_LSCK_LOW;
	
		if (cmd & 0x80) 
		{
		SET_LSDA_HIGH;
		} 
		else 
		{
		SET_LSDA_LOW;
		}

		UDELAY(100);  
		SET_LSCK_HIGH;
		UDELAY(100);  
	
		cmd <<= 1;
	}	

	UDELAY(20); 		       
	
	for(i=0;i<8;i++)
	{ 
		SET_LSCK_LOW;              
	  UDELAY(100);
	  SET_LSCK_HIGH;
   
		value<<= 1;
		value |= GET_HX_SDI;

		UDELAY(10); 
	}        

	SET_LSCE_HIGH;     
	SET_LSCK_HIGH;

	return value;	
}

static void config_gpio_for_read(void)
{
	unsigned int i;
    LCM_DEBUG("[LCM_gionee: config_gpio_for read. \n");

	const unsigned int USED_GPIOS[] = 
    {
        LSDI_GPIO_PIN,
        LSCE_GPIO_PIN,
        LSCK_GPIO_PIN,
        LSDA_GPIO_PIN,
    };


    for (i = 0; i < ARY_SIZE(USED_GPIOS); ++i)
    {   	
        lcm_util.set_gpio_mode(USED_GPIOS[i],GPIO_MODE_00);
        lcm_util.set_gpio_dir(USED_GPIOS[i], 1);
    }
   	
    lcm_util.set_gpio_dir(LSDI_GPIO_PIN, 0); 
    lcm_util.set_gpio_pull_enable(USED_GPIOS[i], 0);  	
}

static int compare_ic_id(void)
{	
	int i;

	LCDSPI_InitCMD(0xB0);
	LCDSPI_InitDAT(0x04);

	LCDSPI_InitCMD(0xF5);
	i=LCDSPI_Read(0xDA);

	LCM_DEBUG("DA IS %02x\n",i);
	return 1; 	
}

static void init_lcm_registers(void)
{    
	//------LCD Init Start------
	LCDSPI_InitCMD(0x11);	//SLEEP OUT
	MDELAY(150);

	LCDSPI_InitCMD(0xB0);	//B0h Manufacturer Command
	LCDSPI_InitDAT(0x04);

	LCDSPI_InitCMD(0x36);	//36h Set address mode
	LCDSPI_InitDAT(0x88); 	//80

	LCDSPI_InitCMD(0xCE);	//pwm FREQUENCE SET  BACKLIGHT CONTROL
	LCDSPI_InitDAT(0x01);   //05  yangqb CR00716620 2012.10.20
	LCDSPI_InitDAT(0x84);
	LCDSPI_InitDAT(0x00);
	LCDSPI_InitDAT(0xFF);
	LCDSPI_InitDAT(0x0A);
	LCDSPI_InitDAT(0x01);

	LCDSPI_InitCMD(0xBD);	//RBDh  TSP Sync Signal Control
	LCDSPI_InitDAT(0xC1);   
	LCDSPI_InitDAT(0x05);
	LCDSPI_InitDAT(0x00); 
	LCDSPI_InitDAT(0x05);
	LCDSPI_InitDAT(0x00);
	LCDSPI_InitDAT(0x02);
	LCDSPI_InitDAT(0x00);
	LCDSPI_InitDAT(0x05);
	LCDSPI_InitDAT(0x03);

	LCDSPI_InitCMD(0xBE);	//RBEh Tx Control
	LCDSPI_InitDAT(0xF0); 
	LCDSPI_InitDAT(0x04);
	LCDSPI_InitDAT(0x14); 

	LCDSPI_InitCMD(0xC0);	//RC0h Panel Driving Setting 1
	LCDSPI_InitDAT(0x00); 
	LCDSPI_InitDAT(0x1F);
	LCDSPI_InitDAT(0x03); 
	LCDSPI_InitDAT(0x6E);	//0x06 74
	LCDSPI_InitDAT(0x01);
	LCDSPI_InitDAT(0x02);  
	LCDSPI_InitDAT(0x0A); 
	LCDSPI_InitDAT(0x0A);
	LCDSPI_InitDAT(0x00);

	LCDSPI_InitCMD(0xC2);	//RC2h  Panel Driving Setting 3 
	LCDSPI_InitDAT(0x0F); 
	LCDSPI_InitDAT(0x04);
	LCDSPI_InitDAT(0x0A);
	LCDSPI_InitDAT(0x0A);

	LCDSPI_InitCMD(0xC1);	//RC1h  Panel Driving Setting 2 
	LCDSPI_InitDAT(0x00);
	LCDSPI_InitDAT(0x83);	//03 
	LCDSPI_InitDAT(0x22);
	LCDSPI_InitDAT(0x11);
	LCDSPI_InitDAT(0x03);
	LCDSPI_InitCMD(0xC8);   //GAMMEA_SETTING_SEQUENCE //GAMMEA_SET_RED
	LCDSPI_InitDAT(0x11);  
	LCDSPI_InitDAT(0x15); 
	LCDSPI_InitDAT(0x1D);
	LCDSPI_InitDAT(0x26);  
	LCDSPI_InitDAT(0x34); 
	LCDSPI_InitDAT(0x4A);
	LCDSPI_InitDAT(0x5A);
	LCDSPI_InitDAT(0x3A); 
	LCDSPI_InitDAT(0x24); 
	LCDSPI_InitDAT(0x18);	//01
	LCDSPI_InitDAT(0x0F);	//15  
	LCDSPI_InitDAT(0x06);	//17 
	LCDSPI_InitDAT(0x02);	//00
	LCDSPI_InitDAT(0x11); 
	LCDSPI_InitDAT(0x15); 
	LCDSPI_InitDAT(0x1D);
	LCDSPI_InitDAT(0x26);  
	LCDSPI_InitDAT(0x34); 
	LCDSPI_InitDAT(0x4A);
	LCDSPI_InitDAT(0x5A);   
	LCDSPI_InitDAT(0x3A); 
	LCDSPI_InitDAT(0x24);
	LCDSPI_InitDAT(0x18);	//01  
	LCDSPI_InitDAT(0x0F); 	//15
	LCDSPI_InitDAT(0x06);	//17
	LCDSPI_InitDAT(0x02);	//00

	LCDSPI_InitCMD(0xC9);	//GAMMEA_SET_GREEN
	LCDSPI_InitDAT(0x00);
	LCDSPI_InitDAT(0x15); 
	LCDSPI_InitDAT(0x1D); 
	LCDSPI_InitDAT(0x27);
	LCDSPI_InitDAT(0x34);  
	LCDSPI_InitDAT(0x4B); 
	LCDSPI_InitDAT(0x5B);
	LCDSPI_InitDAT(0x3A); 
	LCDSPI_InitDAT(0x23); 
	LCDSPI_InitDAT(0x18);	//12
	LCDSPI_InitDAT(0x0F);	//15  
	LCDSPI_InitDAT(0x06); 	//15
	LCDSPI_InitDAT(0x02);
	LCDSPI_InitDAT(0x00);
	LCDSPI_InitDAT(0x15);
	LCDSPI_InitDAT(0x1D);
	LCDSPI_InitDAT(0x27);
	LCDSPI_InitDAT(0x34);
	LCDSPI_InitDAT(0x4B);
	LCDSPI_InitDAT(0x5B);
	LCDSPI_InitDAT(0x3A);
	LCDSPI_InitDAT(0x23);
	LCDSPI_InitDAT(0x18);	//12
	LCDSPI_InitDAT(0x0F);	//15
	LCDSPI_InitDAT(0x06);	//15
	LCDSPI_InitDAT(0x02);

	LCDSPI_InitCMD(0xCA);   //GAMMEA_SET_BLUE
	LCDSPI_InitDAT(0x00);   
	LCDSPI_InitDAT(0x19); 
	LCDSPI_InitDAT(0x1F);
	LCDSPI_InitDAT(0x27);  
	LCDSPI_InitDAT(0x34); 
	LCDSPI_InitDAT(0x4A);
	LCDSPI_InitDAT(0x5A);
	LCDSPI_InitDAT(0x3A); 
	LCDSPI_InitDAT(0x22); 
	LCDSPI_InitDAT(0x18);	//0A
	LCDSPI_InitDAT(0x0f);	//15
	LCDSPI_InitDAT(0x06); 	//18
	LCDSPI_InitDAT(0x02);	//04
	LCDSPI_InitDAT(0x00); 
	LCDSPI_InitDAT(0x19); 
	LCDSPI_InitDAT(0x1F);
	LCDSPI_InitDAT(0x27);  
	LCDSPI_InitDAT(0x34); 
	LCDSPI_InitDAT(0x4A);
	LCDSPI_InitDAT(0x5A);   
	LCDSPI_InitDAT(0x3A); 
	LCDSPI_InitDAT(0x22);
	LCDSPI_InitDAT(0x18);	//0A  
	LCDSPI_InitDAT(0x0F);	//15 
	LCDSPI_InitDAT(0x06);	//18
	LCDSPI_InitDAT(0x02);	//04			

	LCDSPI_InitCMD(0xD0);	//RD0h  Power Setting (Charge Pump Setting)
	LCDSPI_InitDAT(0x54);
	LCDSPI_InitDAT(0x19); 
	LCDSPI_InitDAT(0xAA);
	LCDSPI_InitDAT(0xC7);
	LCDSPI_InitDAT(0x8B);
	LCDSPI_InitDAT(0x8E);

	LCDSPI_InitCMD(0xD1);	//RD1h Power Setting (Switching Regulator Setting)
	LCDSPI_InitDAT(0x0D);
	LCDSPI_InitDAT(0x11); 
	LCDSPI_InitDAT(0x02); 
	LCDSPI_InitDAT(0x22);
	LCDSPI_InitDAT(0x32);  
	LCDSPI_InitDAT(0x03); 
	LCDSPI_InitDAT(0x22);
	LCDSPI_InitDAT(0x43); 
	LCDSPI_InitDAT(0x06); 
	LCDSPI_InitDAT(0x77);
	LCDSPI_InitDAT(0x87);  
	LCDSPI_InitDAT(0x05); 
	LCDSPI_InitDAT(0x77);
	LCDSPI_InitDAT(0x97);

	LCDSPI_InitCMD(0xD4);	//RD4h VH/VL Setting
	LCDSPI_InitDAT(0x0F);  
	LCDSPI_InitDAT(0x0E);

	LCDSPI_InitCMD(0xD5);	//RD5h VPLVL/VNLVL Setting 
	LCDSPI_InitDAT(0x34);	//34
	LCDSPI_InitDAT(0x34);	//34

	LCDSPI_InitCMD(0xB8);	//RB8h BACKLIGHT 
	LCDSPI_InitDAT(0x00);	//01
	LCDSPI_InitCMD(0x53);
	LCDSPI_InitDAT(0x2C);

	LCDSPI_InitCMD(0x29);	//DISPLAY ON 
}

static void config_gpio(void)
{
	unsigned int i;
	LCM_DEBUG("[LCM_gionee: config_gpio. \n");
	
	const unsigned int USED_GPIOS[] = 
	{
		LSCE_GPIO_PIN,
		LSCK_GPIO_PIN,
		LSDA_GPIO_PIN,
	};

    for (i = 0; i < ARY_SIZE(USED_GPIOS); ++i)
    {
#ifdef GN_MTK_BSP_LCD_SW_SPI    	
        lcm_util.set_gpio_mode(USED_GPIOS[i],GPIO_MODE_00);
#else
        lcm_util.set_gpio_mode(USED_GPIOS[i],GPIO_MODE_01);
#endif
        lcm_util.set_gpio_dir(USED_GPIOS[i], 1);              
        lcm_util.set_gpio_pull_enable(USED_GPIOS[i], 0);
    }
#ifdef GN_MTK_BSP_LCD_SW_SPI     	
	lcm_util.set_gpio_dir(LSDI_GPIO_PIN, 0);
#endif 	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
    LCM_DEBUG("[LCM************]: lcm_set_util_funcs. \n");
}


static void lcm_get_params(LCM_PARAMS *params)
{
    LCM_DEBUG("[LCM************]: lcm_get_params. \n");

    memset(params, 0, sizeof(LCM_PARAMS));
    params->type   = LCM_TYPE_DPI;
#ifdef GN_MTK_BSP_LCD_SW_SPI
    params->ctrl   = LCM_CTRL_GPIO;
#else
    params->ctrl   = LCM_CTRL_SERIAL_DBI;
#endif
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    params->io_select_mode = 0;    


    params->dbi.port	= 0;
    params->dbi.data_width		= LCM_DBI_DATA_WIDTH_8BITS; 
    params->dbi.cpu_write_bits	= LCM_DBI_CPU_WRITE_8_BITS;
    params->dbi.io_driving_current	= LCM_DRIVING_CURRENT_2MA;
   

    params->dbi.serial.css = 2;
    params->dbi.serial.csh = 2;	
    params->dbi.serial.rd_1st = 2;
    params->dbi.serial.rd_2nd = 2;
    params->dbi.serial.wr_1st = 2;
    params->dbi.serial.wr_2nd = 2;

   
    params->dbi.serial.sif_3wire 	= 1;
    params->dbi.serial.sif_sdi 	    = 0;
    params->dbi.serial.sif_sck_def 	= 1;
    params->dbi.serial.sif_1st_pol	= 0;
    params->dbi.serial.sif_div2 	= 0;
#ifdef GN_MTK_BSP_LCD_SW_SPI
    params->dbi.serial.sif_hw_cs 	= 0;
#else
    params->dbi.serial.sif_hw_cs 	= 1;	
#endif

    /* RGB interface configurations */
    
    params->dpi.mipi_pll_clk_ref  = 0;
    params->dpi.mipi_pll_clk_div1 = 42;      
    params->dpi.mipi_pll_clk_div2 = 10;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 4;
    params->dpi.hsync_back_porch  = 20;
    params->dpi.hsync_front_porch = 100;
    params->dpi.vsync_pulse_width = 250; 
    params->dpi.vsync_back_porch  = 116; 
    params->dpi.vsync_front_porch = 250;
   
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_4MA;
}


static void lcm_init(void)
{
    LCM_DEBUG("[LCM************]: lcm_init. \n");
#if defined(GN_MTK_BSP_LCD_SW_SPI) 
    config_gpio();
    SET_LSCK_HIGH;
#endif	
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);

    init_lcm_registers();
	
}

static void lcm_suspend(void)
{
	LCM_DEBUG("[LCM************]: lcm_suspend. \n"); 
	
	LCDSPI_InitCMD(0x28);
	MDELAY(20);
	LCDSPI_InitCMD(0x10);
	MDELAY(120);
	
	LCDSPI_InitCMD(0xB0);
	LCDSPI_InitDAT(0x04);
	LCDSPI_InitCMD(0xB1);
	LCDSPI_InitDAT(0x01);
}


static void lcm_resume(void)
{
	LCM_DEBUG("[LCM************]: lcm_resume. \n"); 
	
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	init_lcm_registers();	
}
static void lcm_setbacklight(unsigned int level)
{
	unsigned int mapped_level = 0;

	if(last_backlight_level==level)
	{
		return;
	}
	last_backlight_level=level;
	mapped_level = level*4095/255;

	LCDSPI_InitCMD(0x51);
	LCDSPI_InitDAT((mapped_level>>8)&0x0F);
	LCDSPI_InitDAT(mapped_level&0xFF);

	LCM_DEBUG("[LCM_set_back_light]: =%d. \n",mapped_level);
}

extern int IMM_GetOneChannelValue(int dwChannel, int deCount);

static unsigned int lcm_compare_id(void)
{ 
	unsigned int xx_temp = 0;
	LCM_DEBUG("[gn_smd_r69329: lcm_compare_id. \n");
	xx_temp= IMM_GetOneChannelValue(COMPARE_ADC_CHANNEL,500);
	LCM_DEBUG("xx_temp=%d \n",xx_temp);    

	xx_temp =  compare_ic_id(); 
	return xx_temp;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER gn_smd_r69329_lcm_drv = 
{
	.name			= "samsung_r69329",
	.set_util_funcs	= lcm_set_util_funcs,
	.get_params		= lcm_get_params,
	.init			= lcm_init,
	.suspend		= lcm_suspend,
	.resume			= lcm_resume,
	.set_backlight	= lcm_setbacklight, 
	.compare_id		= lcm_compare_id
};
