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

#define LCM_ID 0x1408
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


#define SET_LSDA_INPUT  SET_GPIO_INPUT(LSDA_GPIO_PIN)
#define SET_LSDA_OUTPUT  SET_GPIO_OUTPUT(LSDA_GPIO_PIN)

#define GET_LSDA_BIT mt_get_gpio_in(LSDA_GPIO_PIN)

#define CTRL_ID  (0 << 8)
#define DATA_ID  (1 << 8)

static __inline void spi_send_data(unsigned int data) 
{
}

static __inline unsigned char spi_read_data(void) 
{
	int k;
	unsigned char getbyte=0;

	SET_LSCE_LOW;
	UDELAY(1);
	SET_LSCK_HIGH;
	SET_LSDA_HIGH;
	UDELAY(1);

	SET_LSDA_INPUT;
	UDELAY(2);    

	for (k = 7; k >= 0; k--)
	{
		SET_LSCK_LOW;       
		UDELAY(5);
		SET_LSCK_HIGH;    
		UDELAY(5);
		if (GET_LSDA_BIT) {
			getbyte |= (1 << k);
		}
	}    

	UDELAY(2);    

	SET_LSDA_OUTPUT;

	SET_LSDA_HIGH;
	SET_LSCE_HIGH;

	return getbyte;
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

    UDELAY(2);        
    SET_LSDA_INPUT;
    UDELAY(2);    
    SET_LSCK_LOW;
    UDELAY(2);      
    SET_LSCK_HIGH;        
    UDELAY(2);
    
    for (k = 39; k >= 0; k--)
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
	
    return (getbyte & 0xffff);
}




static __inline void send_ctrl_cmd(unsigned int cmd)
{
    unsigned char i,tempdata;
   
    SET_LSCE_HIGH;
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
 
	SET_LSCE_LOW;
	UDELAY(1);
	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_LOW;//A0=0
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);
	
    tempdata =cmd;
    for (i = 0; i < 8; i++)
    {
        SET_LSCK_LOW;
        UDELAY(1); 
        if (tempdata&0x80) 
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
        tempdata <<= 1;
    }
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
    UDELAY(1);
}

static __inline void send_data_cmd(unsigned int data)
{
    unsigned char i,tempdata;
    SET_LSCE_HIGH;
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;

    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_LOW;
    UDELAY(1);
    SET_LSDA_HIGH;//A0=1
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    tempdata =data;
    for (i = 0; i < 8; i++)
    {
        SET_LSCK_LOW;
        UDELAY(1);
        if (tempdata&0x80) 
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
     	tempdata <<= 1;
    }
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
    UDELAY(1);
}


static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}

static void init_lcm_registers(void)
{
	send_ctrl_cmd(0xB0); //B0h
	send_data_cmd(0x04);

	send_ctrl_cmd(0xB3); //B3h
	send_data_cmd(0x12);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xBD); //BDh
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC1); //C1h
	send_data_cmd(0x23);
	send_data_cmd(0x31);
	send_data_cmd(0x99);
	send_data_cmd(0x21);
	send_data_cmd(0x20);
	send_data_cmd(0x00);
	send_data_cmd(0x10);
	send_data_cmd(0x28);
	send_data_cmd(0x0b);
	send_data_cmd(0x09);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x21);
	send_data_cmd(0x01);

	send_ctrl_cmd(0xC2); //C2h
	send_data_cmd(0x08);  //two line inversion  //0x08
	send_data_cmd(0x06);
	send_data_cmd(0x06);
	send_data_cmd(0x01);
	send_data_cmd(0x03);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC8); //c8h
	send_data_cmd(0x04); //V255
	send_data_cmd(0x10); //V251
	send_data_cmd(0x18); //V247
	send_data_cmd(0x20); //V240
	send_data_cmd(0x2e);//V224
	send_data_cmd(0x46); //V176
	send_data_cmd(0x3c); //V79
	send_data_cmd(0x28); //V31
	send_data_cmd(0x1f);  //V15
	send_data_cmd(0x18); //V8
	send_data_cmd(0x10); //V4
	send_data_cmd(0x04);

	send_data_cmd(0x04); //V255
	send_data_cmd(0x10); //V251
	send_data_cmd(0x18); //V247
	send_data_cmd(0x20); //V240
	send_data_cmd(0x2e);//V224
	send_data_cmd(0x46); //V176
	send_data_cmd(0x3c); //V79
	send_data_cmd(0x28); //V31
	send_data_cmd(0x1f);  //V15
	send_data_cmd(0x18); //V8
	send_data_cmd(0x10); //V4
	send_data_cmd(0x04);

	send_ctrl_cmd(0xC9); //c9h
	send_data_cmd(0x04); //V255
	send_data_cmd(0x10); //V251
	send_data_cmd(0x18); //V247
	send_data_cmd(0x20); //V240
	send_data_cmd(0x2e);//V224
	send_data_cmd(0x46); //V176
	send_data_cmd(0x3c); //V79
	send_data_cmd(0x28); //V31
	send_data_cmd(0x1f);  //V15
	send_data_cmd(0x18); //V8
	send_data_cmd(0x10); //V4
	send_data_cmd(0x04);

	send_data_cmd(0x04); //V255
	send_data_cmd(0x10); //V251
	send_data_cmd(0x18); //V247
	send_data_cmd(0x20); //V240
	send_data_cmd(0x2e);//V224
	send_data_cmd(0x46); //V176
	send_data_cmd(0x3c); //V79
	send_data_cmd(0x28); //V31
	send_data_cmd(0x1f);  //V15
	send_data_cmd(0x18); //V8
	send_data_cmd(0x10); //V4
	send_data_cmd(0x04);

	send_ctrl_cmd(0xCA); //CAh
	send_data_cmd(0x04); //V255
	send_data_cmd(0x10); //V251
	send_data_cmd(0x18); //V247
	send_data_cmd(0x20); //V240
	send_data_cmd(0x2e);//V224
	send_data_cmd(0x46); //V176
	send_data_cmd(0x3c); //V79
	send_data_cmd(0x28); //V31
	send_data_cmd(0x1f);  //V15
	send_data_cmd(0x18); //V8
	send_data_cmd(0x10); //V4
	send_data_cmd(0x04);

	send_data_cmd(0x04); //V255
	send_data_cmd(0x10); //V251
	send_data_cmd(0x18); //V247
	send_data_cmd(0x20); //V240
	send_data_cmd(0x2e);//V224
	send_data_cmd(0x46); //V176
	send_data_cmd(0x3c); //V79
	send_data_cmd(0x28); //V31
	send_data_cmd(0x1f);  //V15
	send_data_cmd(0x18); //V8
	send_data_cmd(0x10); //V4
	send_data_cmd(0x04);

	send_ctrl_cmd(0xD0); //D0h
	send_data_cmd(0x29);
	send_data_cmd(0x03);
	send_data_cmd(0xCE);
	send_data_cmd(0xA6);
	send_data_cmd(0x0C);
	send_data_cmd(0x43);
	send_data_cmd(0x20);
	send_data_cmd(0x10);
	send_data_cmd(0x01);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x01);
	send_data_cmd(0x00);
	send_data_cmd(0x03);
	send_data_cmd(0x01);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xD1); //D1h
	send_data_cmd(0x18);
	send_data_cmd(0x0C);
	send_data_cmd(0x23);
	send_data_cmd(0x03);
	send_data_cmd(0x75);
	send_data_cmd(0x02);
	send_data_cmd(0x50);

	send_ctrl_cmd(0xD3); //D3h
	send_data_cmd(0x33);

	send_ctrl_cmd(0xD5); //D5h
	send_data_cmd(0x28);
	send_data_cmd(0x28);

	send_ctrl_cmd(0xDE); //DEh
	send_data_cmd(0x01);
	send_data_cmd(0x41);

	send_ctrl_cmd(0xE6); //E6h
	send_data_cmd(0x51);

	send_ctrl_cmd(0xFA); //FAh
	send_data_cmd(0x03);

	send_ctrl_cmd(0xD6); //D6h
	send_data_cmd(0x28);
	MDELAY(100); 


	send_ctrl_cmd(0x2A); //2A
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0xDF);

	send_ctrl_cmd(0x2B); //2B
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x03);
	send_data_cmd(0x1F);

	send_ctrl_cmd(0x36); //36
	send_data_cmd(0x00);


	send_ctrl_cmd(0x3A); //3A
	send_data_cmd(0x77);

	send_ctrl_cmd(0x11); //11
	MDELAY(200); 
	send_ctrl_cmd(0x29); //29
	MDELAY(20); 

	send_ctrl_cmd(0x2C); //2c
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

    params->dpi.hsync_pulse_width = 3; //12;
    params->dpi.hsync_back_porch  = 10; //8;
    params->dpi.hsync_front_porch = 110;  //8;
    params->dpi.vsync_pulse_width = 2;  // 4;
    params->dpi.vsync_back_porch  = 18;  //8;
    params->dpi.vsync_front_porch = 8;
    
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

    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(50);

    init_lcm_registers();
}


static void lcm_suspend(void)
{

	send_ctrl_cmd(0x11); //EXIT_SLEEP_MODE 
	MDELAY(100); 
	send_ctrl_cmd(0x29);    
	MDELAY(100); 
	send_ctrl_cmd(0x11); //EXIT_SLEEP_MODE 
	MDELAY(100); 
	send_ctrl_cmd(0x2c);   
       MDELAY(120); 
	send_ctrl_cmd(0x28);    
	MDELAY(20); 
	send_ctrl_cmd(0x10); //ENTER_SLEEP_MODE 
	MDELAY(100); 
	send_ctrl_cmd(0xb1);    
	send_data_cmd(0x01);
	MDELAY(200); 
	SET_LSDA_LOW;
	SET_LSCK_LOW;
	

}


static void lcm_resume(void)
{
/*	send_ctrl_cmd(0x11); //EXIT_SLEEP_MODE 
	MDELAY(120); 
	send_ctrl_cmd(0x29);    
	MDELAY(100); 
	send_ctrl_cmd(0x2c);   */
      lcm_init();
}

static unsigned int lcm_compare_id(void)
{
	unsigned int    id = 0;

	config_gpio();
	SET_RESET_PIN(1);
	MDELAY(50);
	SET_RESET_PIN(0);
	MDELAY(200);
	SET_RESET_PIN(1);
	MDELAY(300);

	send_ctrl_cmd(0xB0); //B0h
	send_data_cmd(0x04);	
	MDELAY(5);
	send_ctrl_cmd(0xf5);
	
	id = spi_get_byte(0xbf);

#ifdef BUILD_UBOOT
	printf("lcd_id=0x%x  \r\n",id);
#else
	printk("lcd_id=0x%x  \r\n",id);
#endif

	return (LCM_ID == id)?1:0;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER blg9009wv0_r61408_lcm_drv = 
{
	.name		= "blg9009wv0_r61408",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};



