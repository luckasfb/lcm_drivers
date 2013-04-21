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
#include <asm/arch/mt6575_gpio.h>
#else
#include <mach/mt6575_gpio.h>
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

//chenhaojun
#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCM_ID 0x5582
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)  (lcm_util.set_gpio_out((n), (v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define SET_LSCE_LOW   SET_GPIO_OUT(LSCE_GPIO_PIN, 0)
#define SET_LSCE_HIGH  SET_GPIO_OUT(LSCE_GPIO_PIN, 1)
#define SET_LSCK_LOW   SET_GPIO_OUT(LSCK_GPIO_PIN, 0)
#define SET_LSCK_HIGH  SET_GPIO_OUT(LSCK_GPIO_PIN, 1)
#define SET_LSDA_LOW   SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH  SET_GPIO_OUT(LSDA_GPIO_PIN, 1)

#define SET_LA0_INPUT  SET_GPIO_INPUT(LSA0_GPIO_PIN)
#define SET_LA0_OUTPUT  SET_GPIO_OUTPUT(LSA0_GPIO_PIN)

#define GET_LSA0_BIT mt_get_gpio_in(LSA0_GPIO_PIN)

#define CTRL_ID  (0 << 8)
#define DATA_ID  (1 << 8)
static __inline unsigned char spi_read_data(void) 
{
    unsigned int i;
    int k=0;
    unsigned int getbyte1=0;
    unsigned int data=0;
	
#ifdef BUILD_UBOOT
	printf("spi_read_data   \r\n");
#endif

	data = 0xc0;		
	SET_LSCE_HIGH;
	UDELAY(1);
	SET_LSCK_HIGH;
	UDELAY(1);
	SET_LSDA_HIGH;
	UDELAY(1);
	SET_LA0_INPUT;
	
	SET_LSCE_LOW;
	UDELAY(1);

	for (i = 0; i < 8; ++ i)
	{
		SET_LSCK_LOW;
		if (data & (1 << 7)) 
		{
			SET_LSDA_HIGH;
		} 
		else 
		{
			SET_LSDA_LOW;
		}
		UDELAY(1);
		SET_LSCK_HIGH;
		UDELAY(1);
		data <<= 1;
	}
#if 0
	for(i=8;i>0;i--)
	{
		SET_LSCK_LOW;
		UDELAY(1);
		SET_LSCK_HIGH;
		UDELAY(1);
	}
#endif
	UDELAY(5);
    for (k = 7; k >=0; k--)
    {
        SET_LSCK_LOW;
	if (GET_LSA0_BIT) 
	{
		getbyte1 |= (1 << k);
	}
	
	UDELAY(1);
       SET_LSCK_HIGH;        
       UDELAY(1);
    }    
#ifdef BUILD_UBOOT
	printf("getbyte1=0x%x   \r\n",getbyte1);
#endif
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
    return getbyte1;
}


void Write_com(unsigned char SSD2123_index,unsigned char NUMoFparameter)
{
    unsigned char HX_WR_COM=0x00;
    unsigned char i;
    
    SET_LSCE_HIGH  ;//SET_HX_CS;
    SET_LSDA_HIGH ;//SET_HX_SDI;
    SET_LSCK_HIGH;//SET_HX_CLK;
    SET_LSCE_LOW;
    
    HX_WR_COM |=(NUMoFparameter<<5);
    UDELAY(1);
    
    for(i=0;i<8;i++) // 8 Data
    {
        SET_LSCK_LOW;
		
        if ( HX_WR_COM& 0x80)
            SET_LSDA_HIGH;
        else
            SET_LSDA_LOW;
		
        HX_WR_COM<<= 1;
        UDELAY(1); 
        SET_LSCK_HIGH;
        UDELAY(1);  
    } 
	
    for(i=0;i<8;i++) // 8 Data
    {    
       SET_LSCK_LOW;   
	   
       if ( SSD2123_index& 0x80)    
           SET_LSDA_HIGH;    
       else   
           SET_LSDA_LOW;
	   
       SSD2123_index<<= 1;
       UDELAY(1); 
       SET_LSCK_HIGH;
       UDELAY(1); 
    }
	
    SET_LSCK_HIGH;
    UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1); 
 
    if( NUMoFparameter==0)   
        SET_LSCE_HIGH;
    else
        SET_LSCE_LOW;        
}

void Write_register(unsigned char SSD2123_data,unsigned char NUMoFparameter1)
{
    unsigned char HX_WR_COM=0x40;//76
    unsigned char i;
    
    SET_LSCE_HIGH  ;//SET_HX_CS;
    SET_LSDA_HIGH ;//SET_HX_SDI;
    SET_LSCK_HIGH;//SET_HX_CLK;
    SET_LSCE_LOW;
    
    UDELAY(1);
    
    for(i=0;i<8;i++) // 8 Data    
    {   
        SET_LSCK_LOW; 
		
        if ( HX_WR_COM& 0x80)
            SET_LSDA_HIGH;        
        else
            SET_LSDA_LOW;
        
        HX_WR_COM<<= 1;
        UDELAY(1); 
        SET_LSCK_HIGH;
        UDELAY(1);
    } 
   
    for(i=0;i<8;i++) // 8 Data 
    {   
        SET_LSCK_LOW;
    
        if ( SSD2123_data& 0x80)  
            SET_LSDA_HIGH;
        else
            SET_LSDA_LOW;
    
        SSD2123_data<<= 1;
		
        UDELAY(1); 
        SET_LSCK_HIGH;
        UDELAY(1);
    }
        
    SET_LSCK_HIGH;    
    UDELAY(1);     
    SET_LSDA_HIGH ;  
    UDELAY(1);  
    
    if( NUMoFparameter1==0)    
        SET_LSCE_HIGH;    
    else   
        SET_LSCE_LOW; 
}

void set_lcm_register(unsigned int cmd,unsigned char  dat)
{
    Write_com(cmd>>8,0X01);
    Write_com(cmd,0X00);
    Write_register(dat,0Xff);
}


static void init_lcm_registers(void)
{
    Write_com(0X11,0X01);    
    Write_com(0X00,0X00);    
    set_lcm_register(0xC000, 0x8a);           
    set_lcm_register(0xC002, 0x8A);
    set_lcm_register(0xC100, 0x40);
    set_lcm_register(0xC700, 0x8B);
    //set_lcm_register(0x3600, 0x01);     
    MDELAY(200);      
    //***********Display on*********************    
    Write_com(0X29,0X01);    
    Write_com(0X00,0X00);   
    MDELAY(20);     
    Write_com(0X2c,0X01);    
    Write_com(0X00,0X00);	    
}


static void config_gpio(void)
{
    const unsigned int USED_GPIOS[] = 
    {
        LSCE_GPIO_PIN,
        LSCK_GPIO_PIN,
        LSDA_GPIO_PIN
    };

    unsigned int i;

    //lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_DISP_LSA0_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_DISP_LSCE_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCK_GPIO_PIN, GPIO_DISP_LSCK_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSDA_GPIO_PIN, GPIO_DISP_LSDA_PIN_M_GPIO);

    for (i = 0; i < ARY_SIZE(USED_GPIOS); ++ i)
    {
        lcm_util.set_gpio_dir(USED_GPIOS[i], 1);               // GPIO out
        lcm_util.set_gpio_pull_enable(USED_GPIOS[i], 0);
    }

    // Swithc LSA0 pin to GPIO mode to avoid data contention,
    // since A0 is connected to LCM's SPI SDO pin
    //
    //lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);                   // GPIO in
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type   = LCM_TYPE_DPI;
    params->ctrl   = LCM_CTRL_GPIO;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    
    params->dpi.mipi_pll_clk_ref  = 0;
    params->dpi.mipi_pll_clk_div1 = 42;//40?
    params->dpi.mipi_pll_clk_div2 = 10;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING; 
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 22;
    params->dpi.hsync_back_porch  = 12;
    params->dpi.hsync_front_porch = 16;
    params->dpi.vsync_pulse_width = 10;
    params->dpi.vsync_back_porch  = 33;
    params->dpi.vsync_front_porch = 12;
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;
}


static void lcm_init(void)
{
    config_gpio();
	
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
    Write_com(0X28,0X01);    
    Write_com(0X00,0X00);    
    MDELAY(20);     
    Write_com(0X10,0X01);    
    Write_com(0X00,0X00);    
    MDELAY(120); 
}


static void lcm_resume(void)
{
    Write_com(0X11,0X01);
    Write_com(0X00,0X00);
    MDELAY(120); 
    Write_com(0X29,0X01);
    Write_com(0X00,0X00);
    MDELAY(20); 
}

static unsigned int lcm_compare_id(void)
{
	unsigned int    id1 = 0;
	unsigned int    id2 = 0;
	unsigned int    id = 0;


    config_gpio();

	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);
#if 0
	set_lcm_register(0xF000,0x55);             
	set_lcm_register(0xF001,0xAA);             
	set_lcm_register(0xF002,0x52);             
	set_lcm_register(0xF003,0x08);             
	set_lcm_register(0xF004,0x01);
#endif	
	//send_ctrl_cmd(0x1080);
    Write_com(0X10,0X01);    
    Write_com(0X80,0X00);    
	id1 = spi_read_data();  //spi_read_data();
	MDELAY(20);
	//send_ctrl_cmd(0x1180);
	Write_com(0X11,0X01);    
    Write_com(0X80,0X00);  
	id2 = spi_read_data();  //spi_read_data();

	id = (id1<<8)|id2;


	return (LCM_ID == id)?1:0;
}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER lt050anyb01d_wvga_nt35582_lcm_drv = 
{

     .name			= "lt050anyb01d_wvga_nt35582",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};
