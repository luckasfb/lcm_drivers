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
#include <cust_gpio_usage.h>
#include "lcm_drv.h"

static void init_lcm_registers(void);

#ifndef GN_MTK_BSP_LCD_SW_SPI
 //#define GN_MTK_BSP_LCD_SW_SPI
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};
#define SET_RESET_PIN(v)        (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)      (lcm_util.set_gpio_out((n), (v)))
#define UDELAY(n)               (lcm_util.udelay(n))
#define MDELAY(n)               (lcm_util.mdelay(n))


#define LSA0_GPIO_PIN           (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN           (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN           (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN           (GPIO_DISP_LSDA_PIN)

#define SET_LSCE_LOW             SET_GPIO_OUT(LSCE_GPIO_PIN, 0)
#define SET_LSCE_HIGH            SET_GPIO_OUT(LSCE_GPIO_PIN, 1)
#define SET_LSCK_LOW             SET_GPIO_OUT(LSCK_GPIO_PIN, 0)
#define SET_LSCK_HIGH            SET_GPIO_OUT(LSCK_GPIO_PIN, 1)
#define SET_LSDA_LOW             SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH            SET_GPIO_OUT(LSDA_GPIO_PIN, 1)

#if defined(GN_MTK_BSP_LCD_SW_SPI)  //added by cpeng 20120118
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
static __inline void SPI_3W_SET_CMD(unsigned char data)
{
    unsigned int i;

    SET_LSCK_HIGH;
    UDELAY(10);
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_LOW;
    SET_LSCK_LOW;
    SET_LSDA_LOW;
    SET_LSCK_LOW;
    SET_LSCK_LOW;
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; i++ )
    {
        SET_LSCK_LOW;
        if (data & 0x80) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        data <<= 1;
    }
    SET_LSCE_HIGH;
}

static __inline void SPI_3W_SET_PAs(unsigned char data)
{
    unsigned int i;

    SET_LSCK_HIGH;
    UDELAY(1);
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_LOW;
    SET_LSCK_LOW;
    SET_LSDA_HIGH;
    SET_LSCK_LOW;
    SET_LSCK_LOW;
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; i++ )
    {
        SET_LSCK_LOW;
        if (data & 0x80) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        data <<= 1;
    }
    SET_LSCE_HIGH;
}

static __inline void send_ctrl_cmd(unsigned int cmd)
{
    SPI_3W_SET_CMD(cmd);
}

static __inline void send_data_cmd(unsigned int data)
{
    SPI_3W_SET_PAs(data);
}


static void config_gpio(void)
{
    LCM_DEBUG("[LCM_gionee: config_gpio. \n");
    const unsigned int USED_GPIOS[] = 
    {
        LSCE_GPIO_PIN,
        LSCK_GPIO_PIN,
        LSDA_GPIO_PIN,
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


#else  //GN_DISP_SW_SPI  
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
static __inline void send_ctrl_cmd(unsigned int cmd)
{
    unsigned char temp = (unsigned char)(cmd & 0xFF);
    lcm_util.send_cmd(temp);
}

static __inline void send_data_cmd(unsigned int data)
{
    unsigned char temp = (unsigned char)(data & 0xFF);
    lcm_util.send_data(temp);
}
#endif
static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}
static void inti_lcm_folow(void)
{
    LCM_DEBUG("[LCM************]: init_lcm_registers. \n");
    send_ctrl_cmd(0x36);   //SET_ADDRESS_MODE 
    send_data_cmd(0x0A); 
  
    send_ctrl_cmd(0xB0);   //MANUFACTURER_COMMAND
    send_data_cmd(0x00); 

    send_ctrl_cmd(0xC0);    //PANEL_DRIVING
    send_data_cmd(0x28); 
    send_data_cmd(0x08);

    send_ctrl_cmd(0xC1);   //SOURCE_CONTROL
    send_data_cmd(0x01);   // 0xC1   add by chenqiang 20111223
    send_data_cmd(0x30);
    send_data_cmd(0x15); 
    send_data_cmd(0x05);
    send_data_cmd(0x22);

    send_ctrl_cmd(0xC4);    //GATE_INTERFACE
    send_data_cmd(0x10); 
    send_data_cmd(0x01);
    send_data_cmd(0x00); 
                            
    send_ctrl_cmd(0xC5);   //DISPLAY H-TIMMING
    send_data_cmd(0x06); 
    send_data_cmd(0x55);
    send_data_cmd(0x03); 
    send_data_cmd(0x07); 
    send_data_cmd(0x0B);
    send_data_cmd(0x33);  
    send_data_cmd(0x00); 
    send_data_cmd(0x01);
    send_data_cmd(0x03);   

    send_ctrl_cmd(0xC6);   //RGB_SYNC_OPTION  (USER)    NA IMPORTANT SETTING
    send_data_cmd(0x02); 

    send_ctrl_cmd(0xC8);   //GAMMEA_SET_RED
    send_data_cmd(0x00); 
    send_data_cmd(0x42); 
    send_data_cmd(0x10);
    send_data_cmd(0x1D);  
    send_data_cmd(0x31); 
    send_data_cmd(0x3A);
    send_data_cmd(0x3A);
    send_data_cmd(0x3D); 
    send_data_cmd(0x3B); 
    send_data_cmd(0x42);
    send_data_cmd(0x49);  
    send_data_cmd(0x48); 
    send_data_cmd(0x48);
    send_data_cmd(0x44); 
    send_data_cmd(0x48); 
    send_data_cmd(0x59);
    send_data_cmd(0x55);  
    send_data_cmd(0x57); 
    send_data_cmd(0x14);
    
    send_data_cmd(0x00);  
    send_data_cmd(0x42); 
    send_data_cmd(0x10);
    send_data_cmd(0x1D);  
    send_data_cmd(0x31); 
    send_data_cmd(0x3A);
    send_data_cmd(0x3A);
    send_data_cmd(0x3D); 
    send_data_cmd(0x3B); 
    send_data_cmd(0x42);
    send_data_cmd(0x49);  
    send_data_cmd(0x48); 
    send_data_cmd(0x48);
    send_data_cmd(0x44); 
    send_data_cmd(0x48); 
    send_data_cmd(0x59);
    send_data_cmd(0x55);  
    send_data_cmd(0x57); 
    send_data_cmd(0x14);

    send_ctrl_cmd(0xC9);   //GAMMEA_SET_RED
    send_data_cmd(0x00);   //GAMEME_SET GREEN
    send_data_cmd(0x38); 
    send_data_cmd(0x14);
    send_data_cmd(0x20);  
    send_data_cmd(0x35); 
    send_data_cmd(0x3E);
    send_data_cmd(0x40);
    send_data_cmd(0x44); 
    send_data_cmd(0x42); 
    send_data_cmd(0x48);
    send_data_cmd(0x4F);  
    send_data_cmd(0x4E); 
    send_data_cmd(0x4E);
    send_data_cmd(0x4A); 
    send_data_cmd(0x4D); 
    send_data_cmd(0x68);
    send_data_cmd(0x5C);  
    send_data_cmd(0x5C); 
    send_data_cmd(0x16);

    send_data_cmd(0x00);   
    send_data_cmd(0x38); 
    send_data_cmd(0x14);
    send_data_cmd(0x20);  
    send_data_cmd(0x35); 
    send_data_cmd(0x3E);
    send_data_cmd(0x40);
    send_data_cmd(0x44); 
    send_data_cmd(0x42); 
    send_data_cmd(0x48);
    send_data_cmd(0x4F);  
    send_data_cmd(0x4E); 
    send_data_cmd(0x4E);
    send_data_cmd(0x4A); 
    send_data_cmd(0x4D); 
    send_data_cmd(0x68);
    send_data_cmd(0x5C);  
    send_data_cmd(0x5C); 
    send_data_cmd(0x16);

    send_ctrl_cmd(0xCA);   //GAMMEA_SET_RED
    send_data_cmd(0x00);   //GAMEME_SET BLUE
    send_data_cmd(0x6B); 
    send_data_cmd(0x0B);
    send_data_cmd(0x12);  
    send_data_cmd(0x22); 
    send_data_cmd(0x28);
    send_data_cmd(0x28);
    send_data_cmd(0x2B); 
    send_data_cmd(0x2A); 
    send_data_cmd(0x33);
    send_data_cmd(0x3B);  
    send_data_cmd(0x3B); 
    send_data_cmd(0x3D);
    send_data_cmd(0x3B); 
    send_data_cmd(0x44); 
    send_data_cmd(0x5D);
    send_data_cmd(0x51);  
    send_data_cmd(0x51); 
    send_data_cmd(0x18);

    send_data_cmd(0x00);   
    send_data_cmd(0x6B); 
    send_data_cmd(0x0B);
    send_data_cmd(0x12);  
    send_data_cmd(0x22); 
    send_data_cmd(0x28);
    send_data_cmd(0x28);
    send_data_cmd(0x2B); 
    send_data_cmd(0x2A); 
    send_data_cmd(0x33);
    send_data_cmd(0x3B);  
    send_data_cmd(0x3B); 
    send_data_cmd(0x3D);
    send_data_cmd(0x3B); 
    send_data_cmd(0x44); 
    send_data_cmd(0x5D);
    send_data_cmd(0x51);  
    send_data_cmd(0x51); 
    send_data_cmd(0x18);

    send_ctrl_cmd(0xD1);   //BIAS CURRENT CONTROL
    send_data_cmd(0x33); 
    send_data_cmd(0x13); 

    send_ctrl_cmd(0xD2);   //DDV_CONTROL
    send_data_cmd(0x11); 
    send_data_cmd(0x00); 
    send_data_cmd(0x00); 

    send_ctrl_cmd(0xD3);   //GAMMA_CONTROL_RET
    send_data_cmd(0x50); 
    send_data_cmd(0x50); 

    send_ctrl_cmd(0xD5);   //DCDC_CONTROL
    send_data_cmd(0x2F); 
    send_data_cmd(0x11);
    send_data_cmd(0x1E); 
    send_data_cmd(0x46);

    send_ctrl_cmd(0xD6);   //VCL CONTROL
    send_data_cmd(0x11); 
    send_data_cmd(0x0A);

//********--BACK_LIGHT******************
    send_ctrl_cmd(0xB4);   
    send_data_cmd(0x0F); 
    send_data_cmd(0x00);
    send_data_cmd(0x50);

    send_ctrl_cmd(0xB5);   
    send_data_cmd(0x00); 

    send_ctrl_cmd(0xB7);   
    send_data_cmd(0x00); 

    send_ctrl_cmd(0xB8);   
    send_data_cmd(0x01); 
//*************************************
}
static void inti_lcm_power_on(void)
{
    send_ctrl_cmd(0x11);   //SLEEP_IN
    MDELAY(120);
    send_ctrl_cmd(0x29);  //DISPLAY_ON        
}
static void init_lcm_registers(void)
{
    inti_lcm_folow();
    inti_lcm_power_on();
}

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
    LCM_DEBUG("[LCM************]: lcm_set_util_funcs. \n");
}

static void lcm_get_params(LCM_PARAMS *params)
{
    LCM_DEBUG("[LCM_gionee*]: lcm_get_params. \n");
    memset(params, 0, sizeof(LCM_PARAMS));
    params->type   = LCM_TYPE_DPI;
#if defined(GN_MTK_BSP_LCD_SW_SPI) 	
    params->ctrl   = LCM_CTRL_GPIO;
#else
	params->ctrl   = LCM_CTRL_SERIAL_DBI;
#endif
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
	
    /* serial host interface configurations */
#if 0
    params->dbi.port                    = 0;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_LSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB565;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_32_BITS;
    params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_6575_4MA;
#endif


    /* RGB interface configurations */
    
    params->dpi.mipi_pll_clk_div1 = 0x15;
    params->dpi.mipi_pll_clk_div2 = 5;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_RISING;   
    params->dpi.de_pol            = LCM_POLARITY_FALLING;   //LCM_POLARITY_FALLING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 4;     //5;     
    params->dpi.hsync_back_porch  = 40;    //10;
    params->dpi.hsync_front_porch = 10;    //18;
    params->dpi.vsync_pulse_width = 1;     //4;    //modified by chenqiang 
    params->dpi.vsync_back_porch  = 7;     //3;              
    params->dpi.vsync_front_porch = 6;     //3;              
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

#ifndef GN_MTK_BSP_LCD_SW_SPI
	params->dbi.port	= 0;
    params->dbi.data_width		   = LCM_DBI_DATA_WIDTH_8BITS; 
    params->dbi.cpu_write_bits		   = LCM_DBI_CPU_WRITE_8_BITS;
    params->dbi.io_driving_current	   = LCM_DRIVING_CURRENT_6575_4MA;
    
 
    params->dbi.serial.css = 2;
    params->dbi.serial.csh = 2;	
    params->dbi.serial.rd_1st = 2;
    params->dbi.serial.rd_2nd = 2;
    params->dbi.serial.wr_1st = 2;
    params->dbi.serial.wr_2nd = 2;
 
    
    params->dbi.serial.sif_3wire = 1;
    params->dbi.serial.sif_sdi = 0;

    params->dbi.serial.sif_1st_pol = 0;
    params->dbi.serial.sif_sck_def = 1;
    params->dbi.serial.sif_div2 = 0;
    params->dbi.serial.sif_hw_cs = 1;
#endif	

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;
	params->dpi.i2x_en = 0;
	params->dpi.i2x_edge = 0;
}

static void lcm_init(void)
{
    LCM_DEBUG("[LCM************]: lcm_init. \n");
//gionee pengchao  20120118 added for sw spi  start
#if defined(GN_MTK_BSP_LCD_SW_SPI) 
    config_gpio();
    SET_LSCK_HIGH;
#endif	
//gionee pengchao  20120118 added for sw spi end
    SET_RESET_PIN(1);
    MDELAY(50);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);
    init_lcm_registers();
}

static void lcm_suspend(void)
{
    LCM_DEBUG("[LCM************]: lcm_suspend. \n");
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);
   // send_ctrl_cmd(0x10);
}

static void lcm_setbacklight(unsigned int level)   //back_light setting 
{
	unsigned int default_level = 52;
	unsigned int mapped_level = 0;
	if(level > 255) 
		mapped_level = 255;

	if(level >= 102) 
		mapped_level = default_level+(level-102)*(255-default_level)/(255-102);
	else
		mapped_level = default_level-(102-level)*default_level/102;
    if(mapped_level ==0)
    {
       send_ctrl_cmd(0xB7);   
       send_data_cmd(0x00); 
    }
    else
    {
       send_ctrl_cmd(0xB7);   
       send_data_cmd(0x24); 
	   send_ctrl_cmd(0xB5);
	   send_data_cmd(mapped_level);	
    }
}

static void lcm_resume(void)
{
    LCM_DEBUG("[LCM************]: lcm_resume. \n");
    inti_lcm_folow();
    send_ctrl_cmd(0x29);   //display on
    send_ctrl_cmd(0x11);
    MDELAY(120);

}
#if 1
extern int IMM_GetOneChannelValue(int dwChannel, int deCount);
//gionee chenqiang 20110804 add for lcm compatible CR CR00423199
static unsigned int lcm_compare_id(void)
{    
    unsigned int xx_temp=0;
    LCM_DEBUG("LCM_lms397kf04_37: lcm_compare_id. \n");
    xx_temp= IMM_GetOneChannelValue(COMPARE_ADC_CHANNEL,500);
    LCM_DEBUG(" xx_temp =%d \n",xx_temp);    
    return (xx_temp > COMPARE_ID_V_LEVEL_2 )?1:0;
}
#endif
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER gn_smd_lms397kf04_lcm_drv = 
{
    .name		= "samsung_lms397",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	#ifndef GN_MTK_BSP_LCD_BACKLIGHT_USE_PWM1
   	.set_backlight	= lcm_setbacklight,
	#endif
   .compare_id     = lcm_compare_id
};


