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
#ifdef BUILD_UBOOT
#include <asm/arch/mt6577_gpio.h>
#define LCM_DEBUG printf
#else
#include <mach/mt6577_gpio.h>
#define LCM_DEBUG printk
#endif

#include "lcm_drv.h"

#ifndef GN_MTK_BSP_LCD_SW_SPI
//#define GN_MTK_BSP_LCD_SW_SPI
#endif

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

static char lcm_id = 0x01;

#if defined(GN_MTK_BSP_LCD_SW_SPI)

#define LCDSPI_GPIO_CMD LCDSPI_InitCMD
#define LCDSPI_GPIO_DAT LCDSPI_InitDAT

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
}

static void LCDSPI_GPIO_DAT(unsigned char data)
{
	unsigned int i;

	SET_LSCE_LOW;
	UDELAY(10);	

	SET_LSCK_LOW;
	SET_LSDA_HIGH;
	UDELAY(100);  

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
}

static unsigned char LCDSPI_Read(char cmd)
{
    char MB;
	unsigned int i,j;

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
 	
 	for(j=0;j<2;j++)
 	{	
 	    MB = 0x00;           
        for(i=0;i<8;i++)
        { 
	        SET_LSCK_LOW;              
            UDELAY(100);
            SET_LSCK_HIGH;
              
            if (1 == (mt_get_gpio_in(LSDI_GPIO_PIN))&0x1) 
                MB |= 1 << (7 - i);  
                            
	        UDELAY(100);
        }        
    }
    SET_LSCE_HIGH;
	SET_LSCK_HIGH;
	SET_LSDA_HIGH;     

    return MB;	
}

static void config_gpio_for_read(void)
{
    LCM_DEBUG("[LCM_gionee: config_gpio_for read. \n");

	const unsigned int USED_GPIOS[] = 
    {
        LSDI_GPIO_PIN,
        LSCE_GPIO_PIN,
        LSCK_GPIO_PIN,
        LSDA_GPIO_PIN,
    };

    unsigned int i;

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
    char ret,id;        
 
    config_gpio_for_read();
  
#ifdef BUILD_UBOOT     	
    SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(100);	
#endif

    LCDSPI_GPIO_CMD(0xB0);
    LCDSPI_GPIO_DAT(0x04);
	LCDSPI_GPIO_CMD(0xF5);
	
	id = LCDSPI_Read(0XBF);	

    LCM_DEBUG("LCD id is %d !!!!!!!!!!!!!!!!!!!!!!!!\n",id);
	ret = 0;
	
	if( lcm_id == id )
	    ret = 1;
    
    config_gpio();

	return ret; 	
}

static void init_lcm_registers(void)
{
    //------LCD Init Start------
    LCDSPI_InitCMD(0xB0); //B0h
    LCDSPI_InitDAT(0x04);

    LCDSPI_InitCMD(0xB3); //B3h
    LCDSPI_InitDAT(0x12); 
    LCDSPI_InitDAT(0x00);


     //gionee pengxw added for CABC  from Truly FAE 2012.2.10 start
    LCDSPI_InitCMD(0xB8); //B8h
    LCDSPI_InitDAT(0x00); 
    LCDSPI_InitDAT(0x09);
    LCDSPI_InitDAT(0x09);
    LCDSPI_InitDAT(0xFF);
    LCDSPI_InitDAT(0xFF);
    LCDSPI_InitDAT(0xD8);
    LCDSPI_InitDAT(0xD8);
    LCDSPI_InitDAT(0x02);
    LCDSPI_InitDAT(0x18);
    LCDSPI_InitDAT(0x10);
    LCDSPI_InitDAT(0x10);
    LCDSPI_InitDAT(0x37);
    LCDSPI_InitDAT(0x5A);
    LCDSPI_InitDAT(0x87);
    LCDSPI_InitDAT(0xBE);
    LCDSPI_InitDAT(0xFF);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x00);

    LCDSPI_InitCMD(0xB9); //B9h
    LCDSPI_InitDAT(0x00); 
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x02);
    LCDSPI_InitDAT(0x08);  
    //gionee pengxw added for CABC  from Truly FAE 2012.2.10 end

    LCDSPI_InitCMD(0xBD); //BDh
    LCDSPI_InitDAT(0x00);

    LCDSPI_InitCMD(0xC0);// 0xc0 
    LCDSPI_InitDAT(0x19);//VSPL[4]; HSPL[3]; 0; EPL[1]; DPL[0];  
    LCDSPI_InitDAT(0x66);

    LCDSPI_InitCMD(0xC1); 
    LCDSPI_InitDAT(0x23); //SEQ_SEL WCVDC2 EN_6Fh xx REV xx BGR SS /*03*/
    LCDSPI_InitDAT(0x31); //NL[5:0);
    LCDSPI_InitDAT(0x99); //BLREV[1:0] PTREV GIPPAT[2:0] GIPMODE[1:0]  /*3d*/
    LCDSPI_InitDAT(0x21); //GSPF[5:0]
    LCDSPI_InitDAT(0x20); //GSPS[5:0]
    LCDSPI_InitDAT(0x00); //STVG[1:0] STVGA[1:0] xx T_GALH GLOL[1:0]
    LCDSPI_InitDAT(0x10); //DVI[3:0] xx xx FL1 xx
    LCDSPI_InitDAT(0x28); //RTN
    LCDSPI_InitDAT(0x0b); //BPX16 VBPE xx BP[4:0]   
    LCDSPI_InitDAT(0x0b); //EFX16 xx xx FP[4:0]     
    LCDSPI_InitDAT(0x00); //ACBF1[1:0] ACF[1:0] ACBR[1:0] ACR[1:0]
    LCDSPI_InitDAT(0x00); //ACBF2[1:0] ACF2[1:0] ACBR2[1:0] ACR2[1:0]
    LCDSPI_InitDAT(0x00); //ACCYC[1:0] VGSET ACFIX[1:0]
    LCDSPI_InitDAT(0x21);
    LCDSPI_InitDAT(0x01);

    LCDSPI_InitCMD(0xC2); //C2h inversion
    LCDSPI_InitDAT(0x28);//0x 1-line;  0x28 Column inversion 
    LCDSPI_InitDAT(0x06);   
    LCDSPI_InitDAT(0x06);   
    LCDSPI_InitDAT(0x01);
    LCDSPI_InitDAT(0x03);
    LCDSPI_InitDAT(0x00);
 
    LCDSPI_InitCMD(0xC8); //c8h
		LCDSPI_InitDAT(0x01);
		LCDSPI_InitDAT(0x0A);
		LCDSPI_InitDAT(0x14);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x2c);
		LCDSPI_InitDAT(0x45);
		LCDSPI_InitDAT(0x3e);
		LCDSPI_InitDAT(0x2a);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x13);
		LCDSPI_InitDAT(0x0F);
		LCDSPI_InitDAT(0x04);
		LCDSPI_InitDAT(0x01);
		LCDSPI_InitDAT(0x0A);
		LCDSPI_InitDAT(0x14);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x2c);
		LCDSPI_InitDAT(0x44);
		LCDSPI_InitDAT(0x3e);
		LCDSPI_InitDAT(0x2a);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x13);
		LCDSPI_InitDAT(0x0F);
		LCDSPI_InitDAT(0x04);

 
    LCDSPI_InitCMD(0xC9); //c9h
		LCDSPI_InitDAT(0x01);
		LCDSPI_InitDAT(0x0A);
		LCDSPI_InitDAT(0x14);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x2c);
		LCDSPI_InitDAT(0x45);
		LCDSPI_InitDAT(0x3e);
		LCDSPI_InitDAT(0x2a);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x13);
		LCDSPI_InitDAT(0x0F);
		LCDSPI_InitDAT(0x04);
		LCDSPI_InitDAT(0x01);
		LCDSPI_InitDAT(0x0A);
		LCDSPI_InitDAT(0x14);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x2c);
		LCDSPI_InitDAT(0x44);
		LCDSPI_InitDAT(0x3e);
		LCDSPI_InitDAT(0x2a);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x13);
		LCDSPI_InitDAT(0x0F);
		LCDSPI_InitDAT(0x04);
		
 
    LCDSPI_InitCMD(0xCA); //cah
		LCDSPI_InitDAT(0x01);
		LCDSPI_InitDAT(0x0A);
		LCDSPI_InitDAT(0x14);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x2c);
		LCDSPI_InitDAT(0x45);
		LCDSPI_InitDAT(0x3e);
		LCDSPI_InitDAT(0x2a);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x13);
		LCDSPI_InitDAT(0x0F);
		LCDSPI_InitDAT(0x04);
		LCDSPI_InitDAT(0x01);
		LCDSPI_InitDAT(0x0A);
		LCDSPI_InitDAT(0x14);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x2c);
		LCDSPI_InitDAT(0x44);
		LCDSPI_InitDAT(0x3e);
		LCDSPI_InitDAT(0x2a);
		LCDSPI_InitDAT(0x1e);
		LCDSPI_InitDAT(0x13);
		LCDSPI_InitDAT(0x0F);
		LCDSPI_InitDAT(0x04);

    LCDSPI_InitCMD(0xD0); //D0h
    LCDSPI_InitDAT(0x29); 
    LCDSPI_InitDAT(0x03);
    LCDSPI_InitDAT(0xCE);
    LCDSPI_InitDAT(0xA6);
    LCDSPI_InitDAT(0x0C);
    LCDSPI_InitDAT(0x43);
    LCDSPI_InitDAT(0x20);
    LCDSPI_InitDAT(0x10);
    LCDSPI_InitDAT(0x01);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x01);
    LCDSPI_InitDAT(0x01);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x03);
    LCDSPI_InitDAT(0x01);
    LCDSPI_InitDAT(0x00);

    LCDSPI_InitCMD(0xD1); //D1h
    LCDSPI_InitDAT(0x18);
    LCDSPI_InitDAT(0x0C);
    LCDSPI_InitDAT(0x23);
    LCDSPI_InitDAT(0x03);
    LCDSPI_InitDAT(0x75);
    LCDSPI_InitDAT(0x02);
    LCDSPI_InitDAT(0x50);

    LCDSPI_InitCMD(0xD3); //D3h
    LCDSPI_InitDAT(0x33);

    LCDSPI_InitCMD(0xD5); //D5h
    LCDSPI_InitDAT(0x28); 
    LCDSPI_InitDAT(0x28);

    LCDSPI_InitCMD(0xDE); //DEh
    LCDSPI_InitDAT(0x01);
    LCDSPI_InitDAT(0x51); //0x41

    LCDSPI_InitCMD(0xE6); //E6h
    LCDSPI_InitDAT(0x51);

    LCDSPI_InitCMD(0xFA); //FAh
    LCDSPI_InitDAT(0x03);
    MDELAY(100);

    LCDSPI_InitCMD(0x2A);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x01);
    LCDSPI_InitDAT(0xDF);

    LCDSPI_InitCMD(0x2B);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x00);
    LCDSPI_InitDAT(0x03);
    LCDSPI_InitDAT(0x1F);


    LCDSPI_InitCMD(0x36);
    LCDSPI_InitDAT(0x00);
 
    LCDSPI_InitCMD(0x3A);
    LCDSPI_InitDAT(0x77);
    
    LCDSPI_InitCMD(0xD6); //D6h
    LCDSPI_InitDAT(0x28);
 
    LCDSPI_InitCMD(0x11);
    MDELAY(210);

    LCDSPI_InitCMD(0x29);
    MDELAY(120);    
     
// LCDSPI_InitCMD(0x2C);
}



static void config_gpio(void)
{
    LCM_DEBUG("[LCM_gionee: config_gpio. \n");

	const unsigned int USED_GPIOS[] = 
    {
        LSDI_GPIO_PIN,
        LSCE_GPIO_PIN,
        LSCK_GPIO_PIN,
        LSDA_GPIO_PIN,
    };

    unsigned int i;

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
    params->dbi.data_width		   = LCM_DBI_DATA_WIDTH_8BITS; 
    params->dbi.cpu_write_bits		   = LCM_DBI_CPU_WRITE_8_BITS;
    params->dbi.io_driving_current	   = LCM_DRIVING_CURRENT_2MA;
   

    params->dbi.serial.css = 15;
    params->dbi.serial.csh = 15;	
    params->dbi.serial.rd_1st = 2;
    params->dbi.serial.rd_2nd = 2;
    params->dbi.serial.wr_1st = 15;
    params->dbi.serial.wr_2nd = 15;

   
    params->dbi.serial.sif_3wire 	= 1;
    params->dbi.serial.sif_sdi 	        = 1; //0
    params->dbi.serial.sif_sck_def 	= 1;
    params->dbi.serial.sif_1st_pol	= 0;
    params->dbi.serial.sif_div2 	= 1;//0;
#ifdef GN_MTK_BSP_LCD_SW_SPI
    params->dbi.serial.sif_hw_cs 	= 0;
#else
    params->dbi.serial.sif_hw_cs 	= 1;	
#endif

    /* RGB interface configurations */
    
    params->dpi.mipi_pll_clk_ref  = 0;
    params->dpi.mipi_pll_clk_div1 = 53;//from 42 to 50
    params->dpi.mipi_pll_clk_div2 = 10;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_RISING;
    params->dpi.de_pol            = LCM_POLARITY_FALLING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 15;
    params->dpi.hsync_back_porch  = 60;
    params->dpi.hsync_front_porch = 120;
    params->dpi.vsync_pulse_width =  4; 
    params->dpi.vsync_back_porch  = 14; 
    params->dpi.vsync_front_porch = 12;
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_4MA;
}


static void lcm_init(void)
{
    LCM_DEBUG("[LCM************]: lcm_init. \n");

    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(100);
    config_gpio();
    init_lcm_registers();
}

static void lcm_suspend(void)
{
    LCM_DEBUG("[LCM************]: lcm_suspend. \n");
    LCDSPI_InitCMD(0x28);
    MDELAY(20);
    LCDSPI_InitCMD(0x10);//Sleep in
    MDELAY(120);
}


static void lcm_resume(void)
{
    LCM_DEBUG("[LCM************]: lcm_resume. \n");
    LCDSPI_InitCMD(0x11);
    MDELAY(20);
    LCDSPI_InitCMD(0x29);//Sleep in
    MDELAY(120);
}
static void lcm_setbacklight(unsigned int level)
{
    unsigned int default_level = 40; //gionee lisheng modify for CR00699798
    unsigned int mapped_level = 0;
	
    if(level > 255) 
        mapped_level = 255;

    if(level >= 102) 
        mapped_level = default_level+(level-102)*(255-default_level)/(255-102);
    else
        mapped_level = default_level-(102-level)*default_level/102;

    LCDSPI_InitCMD(0xB9); //B9h
    LCDSPI_InitDAT(0x01);
    LCDSPI_InitDAT(mapped_level);
    LCDSPI_InitDAT(0x03);
    LCDSPI_InitDAT(0x08);
     
    //LCM_DEBUG("[LCM_set_back_light]: =%d. \n",mapped_level);
}

extern int IMM_GetOneChannelValue(int dwChannel, int deCount);

static unsigned int lcm_compare_id(void)
{ 
    unsigned int xx_temp=0;
    LCM_DEBUG("[LCM_byd_r61408: lcm_compare_id. \n");
    xx_temp= IMM_GetOneChannelValue(COMPARE_ADC_CHANNEL,500);
    LCM_DEBUG("xx_temp=%d \n",xx_temp);    
#ifndef GN_MTK_BSP_LCD_ID_CHECK     
    if((xx_temp <= COMPARE_ID_V_LEVEL_1) || (xx_temp >= COMPARE_ID_V_LEVEL_2))
        return 0;

    xx_temp =  compare_ic_id();      

    return xx_temp;
#else
    return ( (xx_temp >= COMPARE_ID_V_LEVEL_1 )&&(xx_temp <= COMPARE_ID_V_LEVEL_2) )?1:0;
#endif
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER gn_byd_r61408_lcm_drv = 
{
    .name		    = "byd_r61408",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
   	.set_backlight	= lcm_setbacklight, 
	.compare_id     = lcm_compare_id
};
