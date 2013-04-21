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

#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)   //USED AS SDA INPUT
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

//chenhaojun
#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCM_ID 0x69
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


#define SET_LSDA_INPUT  SET_GPIO_INPUT(LSA0_GPIO_PIN)
#define SET_LSDA_OUTPUT  SET_GPIO_OUTPUT(LSDA_GPIO_PIN)

#define GET_LSDA_BIT mt_get_gpio_in(LSA0_GPIO_PIN)

#define CTRL_ID  (0 << 8)
#define DATA_ID  (1 << 8)


static __inline void send_ctrl_cmd(unsigned char cmd)
{
    unsigned char i;

    SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_LOW;//A0=0
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (cmd & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        cmd <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
}


static __inline void send_data_cmd(unsigned char data)
{
    unsigned char i;

    SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_HIGH;//A0=1
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (data & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        data <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
}


static __inline unsigned int  spi_get_byte(unsigned int reg)
{
    unsigned int i;
    int k;
    unsigned int getbyte=0;

    reg =  (CTRL_ID | (reg & 0xFF));
    
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
    UDELAY(1);

    for (i = 0; i < 9; ++ i)
    {
        SET_LSCK_LOW;
        if (reg & (1 << 8)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        reg <<= 1;
    }
    
    SET_LSDA_INPUT;
    UDELAY(2);    
  //  SET_LSCK_LOW;
   // UDELAY(2);      
       SET_LSCK_LOW;
    UDELAY(2);      
    SET_LSCK_HIGH;        
    UDELAY(2);
    
    for (k = 7; k >= 0; k--)
    {
        if (GET_LSDA_BIT) {
            getbyte |= (1 << k);
        }
       SET_LSCK_LOW;       
        UDELAY(5);
       SET_LSCK_HIGH;    
        UDELAY(5);
    }    

    UDELAY(2);    
    
    SET_LSDA_OUTPUT;
    
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

    return getbyte;
}


static void init_lcm_registers(void)
{
	send_ctrl_cmd(0xB9); //Set_EXTC
	send_data_cmd(0xFF);
	send_data_cmd(0x83);
	send_data_cmd(0x69);
	send_ctrl_cmd(0xB1); //Set Power
	send_data_cmd(0x01);
	send_data_cmd(0x00);
	send_data_cmd(0x34);
	send_data_cmd(0x07);
	send_data_cmd(0x00);
	send_data_cmd(0x0F);
	send_data_cmd(0x0F);
	send_data_cmd(0x21);
	send_data_cmd(0x28);
	send_data_cmd(0x3F);
	send_data_cmd(0x3F);
	send_data_cmd(0x07);
	send_data_cmd(0x23);
	send_data_cmd(0x01);
	send_data_cmd(0xE6);
	send_data_cmd(0xE6);
	send_data_cmd(0xE6);
	send_data_cmd(0xE6);
	send_data_cmd(0xE6);
	send_ctrl_cmd(0xB2); // SET Display 480x800
	send_data_cmd(0x00);
	send_data_cmd(0x2B);
	send_data_cmd(0x0A);
	send_data_cmd(0x0A);
	send_data_cmd(0x70);
	send_data_cmd(0x00);
	send_data_cmd(0xFF);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x03);
	send_data_cmd(0x03);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_ctrl_cmd(0xB4); // SET Display 480x800
	send_data_cmd(0x00);
	send_data_cmd(0x18);
	send_data_cmd(0x80);
	send_data_cmd(0x10);
	send_data_cmd(0x01);
	send_ctrl_cmd(0xB6); // SET VCOM
	send_data_cmd(0x2C);
	send_data_cmd(0x2C);
	send_ctrl_cmd(0xD5); //SET GIP
	send_data_cmd(0x00);
	send_data_cmd(0x05);
	send_data_cmd(0x03);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x09);
	send_data_cmd(0x10);
	send_data_cmd(0x80);
	send_data_cmd(0x37);
	send_data_cmd(0x37);
	send_data_cmd(0x20);
	send_data_cmd(0x31);
	send_data_cmd(0x46);
	send_data_cmd(0x8A);
	send_data_cmd(0x57);
	send_data_cmd(0x9B);
	send_data_cmd(0x20);
	send_data_cmd(0x31);
	send_data_cmd(0x46);
	send_data_cmd(0x8A);
	send_data_cmd(0x57);
	send_data_cmd(0x9B);
	send_data_cmd(0x07);
	send_data_cmd(0x0F);
	send_data_cmd(0x02);
	send_data_cmd(0x00);
	send_ctrl_cmd(0xE0); //SET GAMMA
	send_data_cmd(0x00);
	send_data_cmd(0x06);
	send_data_cmd(0x06);
	send_data_cmd(0x29);
	send_data_cmd(0x2D);
	send_data_cmd(0x3F);
	send_data_cmd(0x13);
	send_data_cmd(0x32);
	send_data_cmd(0x08);
	send_data_cmd(0x0C);
	send_data_cmd(0x0D);
	send_data_cmd(0x11);
	send_data_cmd(0x14);
	send_data_cmd(0x11);
	send_data_cmd(0x14);
	send_data_cmd(0x0E);
	send_data_cmd(0x15);
	send_data_cmd(0x00);
	send_data_cmd(0x06);
	send_data_cmd(0x06);
	send_data_cmd(0x29);
	send_data_cmd(0x2D);
	send_data_cmd(0x3F);
	send_data_cmd(0x13);
	send_data_cmd(0x32);
	send_data_cmd(0x08);
	send_data_cmd(0x0C);
	send_data_cmd(0x0D);
	send_data_cmd(0x11);
	send_data_cmd(0x14);
	send_data_cmd(0x11);
	send_data_cmd(0x14);
	send_data_cmd(0x11);
	send_data_cmd(0x18);
	send_ctrl_cmd(0x3A); //Set COLMOD
	send_data_cmd(0x77);
	send_ctrl_cmd(0x11); //Sleep Out
	MDELAY(120);
	send_ctrl_cmd(0x29); //Display On
	MDELAY(20);

	send_ctrl_cmd(0x2C); 
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

    lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_DISP_LSA0_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_DISP_LSCE_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCK_GPIO_PIN, GPIO_DISP_LSCK_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSDA_GPIO_PIN, GPIO_DISP_LSDA_PIN_M_GPIO);

    for (i = 0; i < ARY_SIZE(USED_GPIOS); ++ i)
    {
        lcm_util.set_gpio_dir(USED_GPIOS[i], 1);               // GPIO out
        lcm_util.set_gpio_pull_enable(USED_GPIOS[i], 0);
    }

        lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);               // GPIO in
        lcm_util.set_gpio_pull_enable(LSA0_GPIO_PIN, 0);

    // Swithc LSA0 pin to GPIO mode to avoid data contention,
    // since A0 is connected to LCM's SPI SDO pin
    //
   // lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);                   // GPIO in
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
    params->dpi.mipi_pll_clk_div1 = 40;
    params->dpi.mipi_pll_clk_div2 = 10;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 44;
    params->dpi.hsync_back_porch  = 22;
    params->dpi.hsync_front_porch = 33;
    params->dpi.vsync_pulse_width = 5;
    params->dpi.vsync_back_porch  = 5;
    params->dpi.vsync_front_porch = 4;
   
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;
	
}



static void lcm_init(void)
{
#ifdef BUILD_UBOOT
	printf("lcm_init\r\n");
#else
	printk("lcm_init\r\n");
#endif
	config_gpio();

	SET_RESET_PIN(1);
    MDELAY(50);
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(150);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
	send_ctrl_cmd(0x28);
	MDELAY(10);
	send_ctrl_cmd(0x10);
	MDELAY(120);
}


static void lcm_resume(void)
{
    send_ctrl_cmd(0x11);
    MDELAY(120);
    send_ctrl_cmd(0x29);
    send_ctrl_cmd(0x2c);
    MDELAY(20);
}


static unsigned int lcm_compare_id(void)
{	

	unsigned int    id = 0;
	
	SET_RESET_PIN(1);
	MDELAY(50);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(120);


	send_ctrl_cmd(0xB9); //Set_EXTC
	send_data_cmd(0xFF);
	send_data_cmd(0x83);
	send_data_cmd(0x69);

	send_ctrl_cmd(0xfe); 
	send_data_cmd(0xF4);
	MDELAY(20);
	id=spi_get_byte(0xFF);
	
#ifdef BUILD_UBOOT
	printf("lcd_id=%x\r\n",id);
#else
	printk("lcd_id=%x\r\n",id);
#endif

	return (LCM_ID == id)?1:0;
}

LCM_DRIVER yl24001nt_hx8369a_lcm_drv = 
{
    .name			= "yl24001nt_hx8369a",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};


