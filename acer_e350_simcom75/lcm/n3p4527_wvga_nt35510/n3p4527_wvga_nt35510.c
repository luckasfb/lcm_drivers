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


#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCM_ID 0x5510

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

static __inline void spi_send_data(unsigned int data)
{
    unsigned int i;

	SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

    for (i = 0; i < 16; ++ i)
    {
        SET_LSCK_LOW;
        if (data & (1 << 15)) {
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

static __inline unsigned char  spi_get_byte(unsigned int reg)
{
    unsigned int i;
    int k;
    unsigned char getbyte=0;

   // reg =  (CTRL_ID | (reg & 0xFF));
    
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (reg & (1 << 7)) {
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
  //  SET_LSCK_LOW;
   // UDELAY(2);      
       SET_LSCK_LOW;
    UDELAY(2);      
    SET_LSCK_HIGH;        
    UDELAY(2);
    
    for (k = 7; k >= 0; k--)
    {
        if (GET_LSA0_BIT) {
            getbyte |= (1 << k);
        }
       SET_LSCK_LOW;       
        UDELAY(5);
       SET_LSCK_HIGH;    
        UDELAY(5);
    }    

    UDELAY(2);    
     
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

    return getbyte;
}

static __inline void send_ctrl_cmd(unsigned int cmd)
{
    unsigned char high_8bit = (cmd>>8)&0x00FF;
	unsigned char low_8bit = cmd&0x00FF;
    spi_send_data(0x2000|high_8bit);
	spi_send_data(0x0000|low_8bit);
}

static __inline void send_data_cmd(unsigned int data)
{
    unsigned char high_8bit = (data>>8)&0x00FF;
	unsigned char low_8bit = data&0x00FF;
    spi_send_data(0x4000|low_8bit);
}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}

static void init_lcm_registers(void)
{
#if 0
	send_ctrl_cmd(0xff00);  // 
	send_data_cmd(0x80);
	send_ctrl_cmd(0xff01);  // enable EXTC
	send_data_cmd(0x09);
	send_ctrl_cmd(0xff02);  // 
	send_data_cmd(0x01);

	send_ctrl_cmd(0xff80);  // enable Orise mode
	send_data_cmd(0x80);
	send_ctrl_cmd(0xff81); // 
	send_data_cmd(0x09);
	//**************************************************************************
	send_ctrl_cmd(0xff03);  // enable SPI+I2C cmd2 read
	send_data_cmd(0x01);

	send_ctrl_cmd(0xC0B4); // dot Inversion 
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC580);  //  Disable auto voltage detecting
	send_data_cmd(0x03);

	send_ctrl_cmd(0xC582);  //  Disable auto voltage detecting
	send_data_cmd(0xA3);

	send_ctrl_cmd(0xc590); // pump1 X2 ratio
	send_data_cmd(0x96);

	send_ctrl_cmd(0xc591); // pump1 X2 ratio
	send_data_cmd(0x0F);//edit by orise FAE£¬2011.12.15

	send_ctrl_cmd(0xc592); // pump45
	send_data_cmd(0x01);

	send_ctrl_cmd(0xc593); //  VRGH/VRGL Setting
	send_data_cmd(0x00);

	send_ctrl_cmd(0xc594); // pump12 clk freq(Normal)
	send_data_cmd(0x33);

	send_ctrl_cmd(0xC0A3); 
	send_data_cmd(0x1B);

	send_ctrl_cmd(0xC481); 
	send_data_cmd(0x04);

	send_ctrl_cmd(0xD800); // GVDD setting
	send_data_cmd(0x70);

	send_ctrl_cmd(0xD801); // NGVDD setting
	send_data_cmd(0x70);

	send_ctrl_cmd(0xc5B1); 
	send_data_cmd(0x29);

	send_ctrl_cmd(0xD900); // Vcom setting
	send_data_cmd(0x36);


	//*********************************CE***********************************

	send_ctrl_cmd(0xD400); 
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);

	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	send_data_cmd(0x40);


	//D5 setting


	send_ctrl_cmd(0xD500); 
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);
	send_data_cmd(0x00);
	send_data_cmd(0x60);

	send_ctrl_cmd(0xD680); 
	send_data_cmd(0x08);
	//*********************************Gamma***********************************
	// Positive
	send_ctrl_cmd(0xE100); 
	send_data_cmd(0x05);

	send_ctrl_cmd(0xE101); 
	send_data_cmd(0x16);

	send_ctrl_cmd(0xE102); 
	send_data_cmd(0x17);

	send_ctrl_cmd(0xE103); 
	send_data_cmd(0x12);

	send_ctrl_cmd(0xE104); 
	send_data_cmd(0x0B);

	send_ctrl_cmd(0xE105); 
	send_data_cmd(0x1D); 

	send_ctrl_cmd(0xE106); 
	send_data_cmd(0x0F);

	send_ctrl_cmd(0xE107); 
	send_data_cmd(0x0E);

	send_ctrl_cmd(0xE108); 
	send_data_cmd(0x00);

	send_ctrl_cmd(0xE109); 
	send_data_cmd(0x05);

	send_ctrl_cmd(0xE10A); 
	send_data_cmd(0x02);

	send_ctrl_cmd(0xE10B); 
	send_data_cmd(0x06); 

	send_ctrl_cmd(0xE10C); 
	send_data_cmd(0x0E);

	send_ctrl_cmd(0xE10D); 
	send_data_cmd(0x20);

	send_ctrl_cmd(0xE10E); 
	send_data_cmd(0x1D);

	send_ctrl_cmd(0xE10F); 
	send_data_cmd(0x16);


	//NEGATIVE

	send_ctrl_cmd(0xE200); 
	send_data_cmd(0x05);

	send_ctrl_cmd(0xE201); 
	send_data_cmd(0x16);

	send_ctrl_cmd(0xE202); 
	send_data_cmd(0x17);

	send_ctrl_cmd(0xE203); 
	send_data_cmd(0x12);

	send_ctrl_cmd(0xE204); 
	send_data_cmd(0x0B);

	send_ctrl_cmd(0xE205); 
	send_data_cmd(0x1D); 

	send_ctrl_cmd(0xE206); 
	send_data_cmd(0x0F);

	send_ctrl_cmd(0xE207); 
	send_data_cmd(0x0E);

	send_ctrl_cmd(0xE208); 
	send_data_cmd(0x00);

	send_ctrl_cmd(0xE209); 
	send_data_cmd(0x05);

	send_ctrl_cmd(0xE20A); 
	send_data_cmd(0x02);

	send_ctrl_cmd(0xE20B); 
	send_data_cmd(0x07); 

	send_ctrl_cmd(0xE20C); 
	send_data_cmd(0x0E);

	send_ctrl_cmd(0xE20D); 
	send_data_cmd(0x20);

	send_ctrl_cmd(0xE20E); 
	send_data_cmd(0x1D);

	send_ctrl_cmd(0xE20F); 
	send_data_cmd(0x16);

	/////////////////////////////////////////////////////////////////////

	send_ctrl_cmd(0xb3a6); // reg_panel_zinv, reg_panel_zinv_pixel, reg_panel_zinv_odd, reg_panel_zigzag, reg_panel_zigzag_blue, reg_panel_zigzag_shift_r, reg_panel_zigzag_odd
	send_data_cmd(0x20);

	send_ctrl_cmd(0xb3a7); //  panel_set[0] = 1
	send_data_cmd(0x01);

	send_ctrl_cmd(0xce80);// ce81[7:0] : vst1_shift[7:0]       
	send_data_cmd(0x85); 
	send_ctrl_cmd(0xce81);// ce82[7:0] : 0000,	vst1_width[3:0] 
	send_data_cmd(0x01); 
	send_ctrl_cmd(0xce82);// ce83[7:0] : vst1_tchop[7:0]       
	send_data_cmd(0x00); 

	send_ctrl_cmd(0xce83);// ce84[7:0] : vst2_shift[7:0]       
	send_data_cmd(0x84); 
	send_ctrl_cmd(0xce84);// ce85[7:0] : 0000,	vst2_width[3:0] 
	send_data_cmd(0x01); 
	send_ctrl_cmd(0xce85);// ce86[7:0] : vst2_tchop[7:0]       
	send_data_cmd(0x00);

	//CEAx : clka1, clka2

	send_ctrl_cmd(0xcea0);// cea1[7:0] : clka1_width[3:0], clka1_shift[11:8]                         
	send_data_cmd(0x18); 
	send_ctrl_cmd(0xcea1);// cea2[7:0] : clka1_shift[7:0]                                            
	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcea2);// cea3[7:0] : clka1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]   
	send_data_cmd(0x03); 
	send_ctrl_cmd(0xcea3);// cea4[7:0] : clka1_switch[7:0]                                               
	send_data_cmd(0x21);
	send_ctrl_cmd(0xcea4);// cea5[7:0] : clka1_extend[7:0]                                           
	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcea5);// cea6[7:0] : clka1_tchop[7:0]                                            
	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcea6);// cea7[7:0] : clka1_tglue[7:0]                                            
	send_data_cmd(0x00); 

	send_ctrl_cmd(0xcea7);// cea8[7:0] : clka2_width[3:0], clka2_shift[11:8]                         
	send_data_cmd(0x18); 
	send_ctrl_cmd(0xcea8);// cea9[7:0] : clka2_shift[7:0]                                            
	send_data_cmd(0x03); 
	send_ctrl_cmd(0xcea9);// ceaa[7:0] : clka2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]   
	send_data_cmd(0x03); 
	send_ctrl_cmd(0xceaa);// ceab[7:0] : clka2_switch[7:0]                                                
	send_data_cmd(0x22); 
	send_ctrl_cmd(0xceab);// ceac[7:0] : clka2_extend                                                
	send_data_cmd(0x00); 
	send_ctrl_cmd(0xceac);// cead[7:0] : clka2_tchop                                                 
	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcead);// ceae[7:0] : clka2_tglue                                                 
	send_data_cmd(0x00); 

//CEBx : clka3, clka4
        send_ctrl_cmd(0xceb0);// ceb1[7:0] : clka3_width[3:0], clka3_shift[11:8]                          
	send_data_cmd(0x18); 
	send_ctrl_cmd(0xceb1);// ceb2[7:0] : clka3_shift[7:0]                                             
	send_data_cmd(0x02); 
	send_ctrl_cmd(0xceb2);// ceb3[7:0] : clka3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]    
	send_data_cmd(0x03); 
	send_ctrl_cmd(0xceb3);// ceb4[7:0] : clka3_switch[7:0]                                               
	send_data_cmd(0x23); 
	send_ctrl_cmd(0xceb4);// ceb5[7:0] : clka3_extend[7:0]                                            
	send_data_cmd(0x00); 
	send_ctrl_cmd(0xceb5);// ceb6[7:0] : clka3_tchop[7:0]                                             
	send_data_cmd(0x00); 
	send_ctrl_cmd(0xceb6);// ceb7[7:0] : clka3_tglue[7:0]                                             
	send_data_cmd(0x00); 

	send_ctrl_cmd(0xceb7);// ceb8[7:0] : clka4_width[3:0], clka2_shift[11:8]                          
	send_data_cmd(0x18); 
	send_ctrl_cmd(0xceb8);// ceb9[7:0] : clka4_shift[7:0]                                             
	send_data_cmd(0x01); 
	send_ctrl_cmd(0xceb9);// ceba[7:0] : clka4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]    
	send_data_cmd(0x03); 
	send_ctrl_cmd(0xceba);// cebb[7:0] : clka4_switch[7:0]                                                
	send_data_cmd(0x24); 
	send_ctrl_cmd(0xcebb);// cebc[7:0] : clka4_extend                                                 
	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcebc);// cebd[7:0] : clka4_tchop                                                  
	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcebd);// cebe[7:0] : clka4_tglue                                                  
	send_data_cmd(0x00); 


//CFCx : 
	send_ctrl_cmd(0xcfc0);// cfc1[7:0] : eclk_normal_width[7:0]   
	send_data_cmd(0x01); 

	send_ctrl_cmd(0xcfc1);// cfc2[7:0] : eclk_partial_width[7:0]                                                   	send_data_cmd(0x01); 

	send_ctrl_cmd(0xcfc2);// cfc3[7:0] : all_normal_tchop[7:0]                                                    	send_data_cmd(0x20); 

	send_ctrl_cmd(0xcfc3);// cfc4[7:0] : all_partial_tchop[7:0]                                                   	send_data_cmd(0x20); 

	send_ctrl_cmd(0xcfc4);// cfc5[7:0] : eclk1_follow[3:0], eclk2_follow[3:0]                                      	send_data_cmd(0x00); 

	send_ctrl_cmd(0xcfc5);// cfc6[7:0] : eclk3_follow[3:0], eclk4_follow[3:0]                                       send_data_cmd(0x00); 

	send_ctrl_cmd(0xcfc6);// cfc7[7:0] : 00, vstmask, vendmask, 00, dir1, dir2 (0=VGL, 1=VGH)                     	send_data_cmd(0x01); 

	send_ctrl_cmd(0xcfc7);// cfc8[7:0] : reg_goa_gnd_opt, reg_goa_dpgm_tail_set, reg_goa_f_gating_en, reg_goa_f_odd_gating, toggle_mod1, 2, 3, 4  
	send_data_cmd(0x00);    // GND OPT1 (00-->80  2011/10/28)

	send_ctrl_cmd(0xcfc8);// cfc9[7:0] : duty_block[3:0], DGPM[3:0]                                                	send_data_cmd(0x00); 

	send_ctrl_cmd(0xcfc9);// cfca[7:0] : reg_goa_gnd_period[7:0]                                                   	send_data_cmd(0x00);    // Gate PCH (CLK base) (00-->0a  2011/10/28)

//CFDx : 
	send_ctrl_cmd(0xcfd0);// cfd1[7:0] : 0000000, reg_goa_frame_odd_high
	send_data_cmd(0x00); 


//--------------------------------------------------------------------------------
//		initial setting 3 < Panel setting >
//--------------------------------------------------------------------------------

// CB8x
	send_ctrl_cmd(0xcb80);//cb81[7:0] : signal mode setting of  sig4,  sig3,  sig2,  sig1 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb81);//cb82[7:0] : signal mode setting of  sig8,  sig7,  sig6,  sig5 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb82);//cb83[7:0] : signal mode setting of sig12, sig11, sig10,  sig9 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb83);//cb84[7:0] : signal mode setting of sig16, sig15, sig14, sig13 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb84);//cb85[7:0] : signal mode setting of sig20, sig19, sig18, sig17 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb85);//cb86[7:0] : signal mode setting of sig24, sig23, sig22, sig21 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb86);//cb87[7:0] : signal mode setting of sig28, sig27, sig26, sig25 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb87);//cb88[7:0] : signal mode setting of sig32, sig31, sig30, sig29 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb88);//cb89[7:0] : signal mode setting of sig36, sig35, sig34, sig33 ( sleep in )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcb89);//cb8a[7:0] : signal mode setting of sig40, sig39, sig38, sig37 ( sleep in )
  	send_data_cmd(0x00); 

// cb9x	 
	send_ctrl_cmd(0xcb90);//cb91[7:0] : enmode L-byte of sig1  (pwron_3, pwron_2, pwron_1, pwron_0 )	 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb91);//cb92[7:0] : enmode L-byte of sig2  (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb92);//cb93[7:0] : enmode L-byte of sig3  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb93);//cb94[7:0] : enmode L-byte of sig4  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb94);//cb95[7:0] : enmode L-byte of sig5  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb95);//cb96[7:0] : enmode L-byte of sig6  (pwron_3, pwron_2, pwron_1, pwron_0 )  
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb96);//cb97[7:0] : enmode L-byte of sig7  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb97);//cb98[7:0] : enmode L-byte of sig8  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb98);//cb99[7:0] : enmode L-byte of sig9  (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb99);//cb9a[7:0] : enmode L-byte of sig10 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb9a);//cb9b[7:0] : enmode L-byte of sig11 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb9b);//cb9c[7:0] : enmode L-byte of sig12 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb9c);//cb9d[7:0] : enmode L-byte of sig13 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb9d);//cb9e[7:0] : enmode L-byte of sig14 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00);
	send_ctrl_cmd(0xcb9e);//cb9f[7:0] : enmode L-byte of sig15 (pwron_3, pwron_2, pwron_1, pwron_0 ) 
  	send_data_cmd(0x00); 

// cbax
	send_ctrl_cmd(0xcba0);//cba1[7:0] : enmode L-byte of sig16 (pwron_3, pwron_2, pwron_1, pwron_0 )	  
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba1);//cba2[7:0] : enmode L-byte of sig17 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba2);//cba3[7:0] : enmode L-byte of sig18 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba3);//cba4[7:0] : enmode L-byte of sig19 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba4);//cba5[7:0] : enmode L-byte of sig20 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba5);//cba6[7:0] : enmode L-byte of sig21 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba6);//cba7[7:0] : enmode L-byte of sig22 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba7);//cba8[7:0] : enmode L-byte of sig23 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba8);//cba9[7:0] : enmode L-byte of sig24 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcba9);//cbaa[7:0] : enmode L-byte of sig25 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbaa);//cbab[7:0] : enmode L-byte of sig26 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbab);//cbac[7:0] : enmode L-byte of sig27 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbac);//cbad[7:0] : enmode L-byte of sig28 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbad);//cbae[7:0] : enmode L-byte of sig29 (pwron_3, pwron_2, pwron_1, pwron_0 )  
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbae);//cbaf[7:0] : enmode L-byte of sig30 (pwron_3, pwron_2, pwron_1, pwron_0 )  
  	send_data_cmd(0x00);   

// cbbx	 
	send_ctrl_cmd(0xcbb0);//cbb1[7:0] : enmode L-byte of sig31 (pwron_3, pwron_2, pwron_1, pwron_0 )	 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb1);//cbb2[7:0] : enmode L-byte of sig32 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb2);//cbb3[7:0] : enmode L-byte of sig33 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb3);//cbb4[7:0] : enmode L-byte of sig34 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb4);//cbb5[7:0] : enmode L-byte of sig35 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb5);//cbb6[7:0] : enmode L-byte of sig36 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb6);//cbb7[7:0] : enmode L-byte of sig37 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb7);//cbb8[7:0] : enmode L-byte of sig38 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb8);//cbb9[7:0] : enmode L-byte of sig39 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbb9);//cbba[7:0] : enmode L-byte of sig40 (pwron_3, pwron_2, pwron_1, pwron_0 )
  	send_data_cmd(0x00); 
  
// cbcx	 
	send_ctrl_cmd(0xcbc0);//cbc1[7:0] : enmode H-byte of sig1  (pwrof_0, pwrof_1, norm, pwron_4 )	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbc1);//cbc2[7:0] : enmode H-byte of sig2  (pwrof_0, pwrof_1, norm, pwron_4 )          
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbc2);//cbc3[7:0] : enmode H-byte of sig3  (pwrof_0, pwrof_1, norm, pwron_4 )	
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbc3);//cbc4[7:0] : enmode H-byte of sig4  (pwrof_0, pwrof_1, norm, pwron_4 )        
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbc4);//cbc5[7:0] : enmode H-byte of sig5  (pwrof_0, pwrof_1, norm, pwron_4 )	  
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbc5);//cbc6[7:0] : enmode H-byte of sig6  (pwrof_0, pwrof_1, norm, pwron_4 )	
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbc6);//cbc7[7:0] : enmode H-byte of sig7  (pwrof_0, pwrof_1, norm, pwron_4 )	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbc7);//cbc8[7:0] : enmode H-byte of sig8  (pwrof_0, pwrof_1, norm, pwron_4 )	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbc8);//cbc9[7:0] : enmode H-byte of sig9  (pwrof_0, pwrof_1, norm, pwron_4 )	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbc9);//cbca[7:0] : enmode H-byte of sig10 (pwrof_0, pwrof_1, norm, pwron_4 )        
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbca);//cbcb[7:0] : enmode H-byte of sig11 (pwrof_0, pwrof_1, norm, pwron_4 )        
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbcb);//cbcc[7:0] : enmode H-byte of sig12 (pwrof_0, pwrof_1, norm, pwron_4 )        
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbcc);//cbcd[7:0] : enmode H-byte of sig13 (pwrof_0, pwrof_1, norm, pwron_4 )        
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbcd);//cbce[7:0] : enmode H-byte of sig14 (pwrof_0, pwrof_1, norm, pwron_4 ) 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbce);//cbcf[7:0] : enmode H-byte of sig15 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 

// cbdx  	 
	send_ctrl_cmd(0xcbd0);//cbd1[7:0] : enmode H-byte of sig16 (pwrof_0, pwrof_1, norm, pwron_4 )	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbd1);//cbd2[7:0] : enmode H-byte of sig17 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbd2);//cbd3[7:0] : enmode H-byte of sig18 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbd3);//cbd4[7:0] : enmode H-byte of sig19 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbd4);//cbd5[7:0] : enmode H-byte of sig20 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbd5);//cbd6[7:0] : enmode H-byte of sig21 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbd6);//cbd7[7:0] : enmode H-byte of sig22 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbd7);//cbd8[7:0] : enmode H-byte of sig23 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbd8);//cbd9[7:0] : enmode H-byte of sig24 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbd9);//cbda[7:0] : enmode H-byte of sig25 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbda);//cbdb[7:0] : enmode H-byte of sig26 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x04); 
	send_ctrl_cmd(0xcbdb);//cbdc[7:0] : enmode H-byte of sig27 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbdc);//cbdd[7:0] : enmode H-byte of sig28 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbdd);//cbde[7:0] : enmode H-byte of sig29 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbde);//cbdf[7:0] : enmode H-byte of sig30 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 

// cbex	 
	send_ctrl_cmd(0xcbe0);//cbe1[7:0] : enmode H-byte of sig31 (pwrof_0, pwrof_1, norm, pwron_4 )	  
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe1);//cbe2[7:0] : enmode H-byte of sig32 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe2);//cbe3[7:0] : enmode H-byte of sig33 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe3);//cbe4[7:0] : enmode H-byte of sig34 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe4);//cbe5[7:0] : enmode H-byte of sig35 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe5);//cbe6[7:0] : enmode H-byte of sig36 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe6);//cbe7[7:0] : enmode H-byte of sig37 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe7);//cbe8[7:0] : enmode H-byte of sig38 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe8);//cbe9[7:0] : enmode H-byte of sig39 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcbe9);//cbea[7:0] : enmode H-byte of sig40 (pwrof_0, pwrof_1, norm, pwron_4 )
  	send_data_cmd(0x00); 
  	
  
// cbfx	 
	send_ctrl_cmd(0xcbf0);//cbf1[7:0] : lvd enmode of ( sig4,  sig3,  sig2,  sig1)	  
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf1);//cbf2[7:0] : lvd enmode of ( sig8,  sig7,  sig6,  sig5)
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf2);//cbf3[7:0] : lvd enmode of (sig12, sig11, sig10,  sig9)
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf3);//cbf4[7:0] : lvd enmode of (sig16, sig15, sig14, sig13)
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf4);//cbf5[7:0] : lvd enmode of (sig20, sig19, sig18, sig17)
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf5);//cbf6[7:0] : lvd enmode of (sig24, sig23, sig22, sig21)
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf6);//cbf7[7:0] : lvd enmode of (sig28, sig27, sig26, sig25)
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf7);//cbf8[7:0] : lvd enmode of (sig32, sig31, sig30, sig29)
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf8);//cbf9[7:0] : lvd enmode of (sig36, sig35, sig34, sig33)
  	send_data_cmd(0xFF); 
	send_ctrl_cmd(0xcbf9);//cbfa[7:0] : lvd enmode of (sig40, sig39, sig38, sig37)
  	send_data_cmd(0xFF); 

// cc8x	 
	send_ctrl_cmd(0xcc80);//cc81[7:0] : reg setting for signal01 selection with u2d mode	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc81);//cc82[7:0] : reg setting for signal02 selection with u2d mode 
  	send_data_cmd(0x26); 
	send_ctrl_cmd(0xcc82);//cc83[7:0] : reg setting for signal03 selection with u2d mode 
  	send_data_cmd(0x09); 
	send_ctrl_cmd(0xcc83);//cc84[7:0] : reg setting for signal04 selection with u2d mode 
  	send_data_cmd(0x0B); 
	send_ctrl_cmd(0xcc84);//cc85[7:0] : reg setting for signal05 selection with u2d mode 
  	send_data_cmd(0x01); 
	send_ctrl_cmd(0xcc85);//cc86[7:0] : reg setting for signal06 selection with u2d mode 
  	send_data_cmd(0x25); 
	send_ctrl_cmd(0xcc86);//cc87[7:0] : reg setting for signal07 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc87);//cc88[7:0] : reg setting for signal08 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc88);//cc89[7:0] : reg setting for signal09 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc89);//cc8a[7:0] : reg setting for signal10 selection with u2d mode 
  	send_data_cmd(0x00);  

// cc9x	 
	send_ctrl_cmd(0xcc90);//cc91[7:0] : reg setting for signal11 selection with u2d mode	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc91);//cc92[7:0] : reg setting for signal12 selection with u2d mode
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc92);//cc93[7:0] : reg setting for signal13 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc93);//cc94[7:0] : reg setting for signal14 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc94);//cc95[7:0] : reg setting for signal15 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc95);//cc96[7:0] : reg setting for signal16 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc96);//cc97[7:0] : reg setting for signal17 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc97);//cc98[7:0] : reg setting for signal18 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc98);//cc99[7:0] : reg setting for signal19 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc99);//cc9a[7:0] : reg setting for signal20 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc9a);//cc9b[7:0] : reg setting for signal21 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcc9b);//cc9c[7:0] : reg setting for signal22 selection with u2d mode 
  	send_data_cmd(0x26); 
	send_ctrl_cmd(0xcc9c);//cc9d[7:0] : reg setting for signal23 selection with u2d mode 
  	send_data_cmd(0x0A); 
	send_ctrl_cmd(0xcc9d);//cc9e[7:0] : reg setting for signal24 selection with u2d mode 
  	send_data_cmd(0x0C); 
	send_ctrl_cmd(0xcc9e);//cc9f[7:0] : reg setting for signal25 selection with u2d mode 
  	send_data_cmd(0x02);  

// ccax	 
	send_ctrl_cmd(0xcca0);//cca1[7:0] : reg setting for signal26 selection with u2d mode	
  	send_data_cmd(0x25); 
	send_ctrl_cmd(0xcca1);//cca2[7:0] : reg setting for signal27 selection with u2d mode
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcca2);//cca3[7:0] : reg setting for signal28 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcca3);//cca4[7:0] : reg setting for signal29 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcca4);//cca5[7:0] : reg setting for signal20 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcca5);//cca6[7:0] : reg setting for signal31 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcca6);//cca7[7:0] : reg setting for signal32 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcca7);//cca8[7:0] : reg setting for signal33 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcca8);//cca9[7:0] : reg setting for signal34 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcca9);//ccaa[7:0] : reg setting for signal35 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccaa);//ccab[7:0] : reg setting for signal36 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccab);//ccac[7:0] : reg setting for signal37 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccac);//ccad[7:0] : reg setting for signal38 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccad);//ccae[7:0] : reg setting for signal39 selection with u2d mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccae);//ccaf[7:0] : reg setting for signal40 selection with u2d mode 
  	send_data_cmd(0x00);  

// ccbx
	send_ctrl_cmd(0xccb0);//ccb1[7:0] : reg setting for signal01 selection with d2u mode	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccb1);//ccb2[7:0] : reg setting for signal02 selection with d2u mode
  	send_data_cmd(0x25); 
	send_ctrl_cmd(0xccb2);//ccb3[7:0] : reg setting for signal03 selection with d2u mode 
  	send_data_cmd(0x0C); 
	send_ctrl_cmd(0xccb3);//ccb4[7:0] : reg setting for signal04 selection with d2u mode 
  	send_data_cmd(0x0A); 
	send_ctrl_cmd(0xccb4);//ccb5[7:0] : reg setting for signal05 selection with d2u mode 
  	send_data_cmd(0x02); 
	send_ctrl_cmd(0xccb5);//ccb6[7:0] : reg setting for signal06 selection with d2u mode 
  	send_data_cmd(0x26); 
	send_ctrl_cmd(0xccb6);//ccb7[7:0] : reg setting for signal07 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccb7);//ccb8[7:0] : reg setting for signal08 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccb8);//ccb9[7:0] : reg setting for signal09 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccb9);//ccba[7:0] : reg setting for signal10 selection with d2u mode 
  	send_data_cmd(0x00);  

// cccx
	send_ctrl_cmd(0xccc0);//ccc1[7:0] : reg setting for signal11 selection with d2u mode	
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc1);//ccc2[7:0] : reg setting for signal12 selection with d2u mode
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc2);//ccc3[7:0] : reg setting for signal13 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc3);//ccc4[7:0] : reg setting for signal14 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc4);//ccc5[7:0] : reg setting for signal15 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc5);//ccc6[7:0] : reg setting for signal16 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc6);//ccc7[7:0] : reg setting for signal17 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc7);//ccc8[7:0] : reg setting for signal18 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc8);//ccc9[7:0] : reg setting for signal19 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccc9);//ccca[7:0] : reg setting for signal20 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccca);//cccb[7:0] : reg setting for signal21 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xcccb);//cccc[7:0] : reg setting for signal22 selection with d2u mode 
  	send_data_cmd(0x25); 
	send_ctrl_cmd(0xcccc);//cccd[7:0] : reg setting for signal23 selection with d2u mode 
  	send_data_cmd(0x0B); 
	send_ctrl_cmd(0xcccd);//ccce[7:0] : reg setting for signal24 selection with d2u mode 
  	send_data_cmd(0x09); 
	send_ctrl_cmd(0xccce);//cccf[7:0] : reg setting for signal25 selection with d2u mode 
  	send_data_cmd(0x01);  

// ccdx
	send_ctrl_cmd(0xccd0);//ccd1[7:0] : reg setting for signal26 selection with d2u mode	
	send_data_cmd(0x26); 
	send_ctrl_cmd(0xccd1);//ccd2[7:0] : reg setting for signal27 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccd2);//ccd3[7:0] : reg setting for signal28 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccd3);//ccd4[7:0] : reg setting for signal29 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccd4);//ccd5[7:0] : reg setting for signal30 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccd5);//ccd6[7:0] : reg setting for signal31 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccd6);//ccd7[7:0] : reg setting for signal32 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccd7);//ccd8[7:0] : reg setting for signal33 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccd8);//ccd9[7:0] : reg setting for signal34 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccd9);//ccda[7:0] : reg setting for signal35 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccda);//ccdb[7:0] : reg setting for signal36 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccdb);//ccdc[7:0] : reg setting for signal37 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccdc);//ccdd[7:0] : reg setting for signal38 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccdd);//ccde[7:0] : reg setting for signal39 selection with d2u mode 
  	send_data_cmd(0x00); 
	send_ctrl_cmd(0xccde);//ccdf[7:0] : reg setting for signal40 selection with d2u mode 
  	send_data_cmd(0x00); 
	MDELAY(12);
       
	send_ctrl_cmd(0x3600); 
	send_data_cmd(0x00);

	send_ctrl_cmd(0xc181); 
	send_data_cmd(0x66);

	send_ctrl_cmd(0xc5B1); 
	send_data_cmd(0x29);

	send_ctrl_cmd(0xb282); 
	send_data_cmd(0x20);

	send_ctrl_cmd(0xc082); 
	send_data_cmd(0x00);

	send_ctrl_cmd(0xc083); 
	send_data_cmd(0x16);

	send_ctrl_cmd(0xc084); 
	send_data_cmd(0x08);

	send_ctrl_cmd(0xc1A1); 
	send_data_cmd(0x08);

	send_ctrl_cmd(0xCFC7); 
	send_data_cmd(0x02);

	send_ctrl_cmd(0x1100);
	//	printf(" Sleep out \n");
	MDELAY(150);

	send_ctrl_cmd(0x2900);
	//printf(" Display on \n");
	MDELAY(50);
#else
	//ENABLE PAGE 1
	send_ctrl_cmd(0xF000);send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002);send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);send_data_cmd(0x08);
	send_ctrl_cmd(0xF004);send_data_cmd(0x01);

	//VGMP/VGSP 4.5V/0V
	send_ctrl_cmd(0xBC00);send_data_cmd(0x00);
	send_ctrl_cmd(0xBC01);send_data_cmd(0xa8);	//78//===68
	send_ctrl_cmd(0xBC02);send_data_cmd(0x00);//00

	//VGMN/VGSN -4.5V/0V
	send_ctrl_cmd(0xBD00);send_data_cmd(0x00);
	send_ctrl_cmd(0xBD01);send_data_cmd(0x7c);	//78
	send_ctrl_cmd(0xBD02);send_data_cmd(0x00);//00

	//VCOM  
	send_ctrl_cmd(0xBE00);send_data_cmd(0x00);
	send_ctrl_cmd(0xBE01);send_data_cmd(0x65);

	//AVDD Set AVDD 5.2V
	send_ctrl_cmd(0xB000);send_data_cmd(0x0D);
	send_ctrl_cmd(0xB001);send_data_cmd(0x0D);
	send_ctrl_cmd(0xB002);send_data_cmd(0x0D);
	//AVEE  -5.2V
	send_ctrl_cmd(0xB100);send_data_cmd(0x0D);
	send_ctrl_cmd(0xB101);send_data_cmd(0x0D);
	send_ctrl_cmd(0xB102);send_data_cmd(0x0D);
	//VGLX ratio
	send_ctrl_cmd(0xBA00);send_data_cmd(0x14);    //14:  AVEE-AVDD
	send_ctrl_cmd(0xBA01);send_data_cmd(0x14);
	send_ctrl_cmd(0xBA02);send_data_cmd(0x14);
	//VGH 15V  (Free pump)
	send_ctrl_cmd(0xBF00);send_data_cmd(0x01);
	send_ctrl_cmd(0xB301);send_data_cmd(0x0F);
	send_ctrl_cmd(0xB301);send_data_cmd(0x0F);
	send_ctrl_cmd(0xB302);send_data_cmd(0x0F);

	//VGH VOLTAGE SETTING
	send_ctrl_cmd(0xB900);send_data_cmd(0x34);
	send_ctrl_cmd(0xB901);send_data_cmd(0x34);
	send_ctrl_cmd(0xB902);send_data_cmd(0x34);
	//VCL POWERCONTROL
	send_ctrl_cmd(0xB800);send_data_cmd(0x34);
	send_ctrl_cmd(0xB801);send_data_cmd(0x34);
	send_ctrl_cmd(0xB802);send_data_cmd(0x34);

	///*
	//GAMMA SETING  RED
	send_ctrl_cmd(0xD100);send_data_cmd(0x00);
	send_ctrl_cmd(0xD101);send_data_cmd(0x33);
	send_ctrl_cmd(0xD102);send_data_cmd(0x00);
	send_ctrl_cmd(0xD103);send_data_cmd(0x34);
	send_ctrl_cmd(0xD104);send_data_cmd(0x00);
	send_ctrl_cmd(0xD105);send_data_cmd(0x3A);
	send_ctrl_cmd(0xD106);send_data_cmd(0x00);
	send_ctrl_cmd(0xD107);send_data_cmd(0x4A);
	send_ctrl_cmd(0xD108);send_data_cmd(0x00);
	send_ctrl_cmd(0xD109);send_data_cmd(0x5C);
	send_ctrl_cmd(0xD10A);send_data_cmd(0x00);
	send_ctrl_cmd(0xD10B);send_data_cmd(0x81);
	send_ctrl_cmd(0xD10C);send_data_cmd(0x00);
	send_ctrl_cmd(0xD10D);send_data_cmd(0xA6);
	send_ctrl_cmd(0xD10E);send_data_cmd(0x00);
	send_ctrl_cmd(0xD10F);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD110);send_data_cmd(0x01);
	send_ctrl_cmd(0xD111);send_data_cmd(0x13);
	send_ctrl_cmd(0xD112);send_data_cmd(0x01);
	send_ctrl_cmd(0xD113);send_data_cmd(0x54);
	send_ctrl_cmd(0xD114);send_data_cmd(0x01);
	send_ctrl_cmd(0xD115);send_data_cmd(0x82);
	send_ctrl_cmd(0xD116);send_data_cmd(0x01);
	send_ctrl_cmd(0xD117);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD118);send_data_cmd(0x02);
	send_ctrl_cmd(0xD119);send_data_cmd(0x00);
	send_ctrl_cmd(0xD11A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD11B);send_data_cmd(0x01);
	send_ctrl_cmd(0xD11C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD11D);send_data_cmd(0x34);
	send_ctrl_cmd(0xD11E);send_data_cmd(0x02);
	send_ctrl_cmd(0xD11F);send_data_cmd(0x67);
	send_ctrl_cmd(0xD120);send_data_cmd(0x02);
	send_ctrl_cmd(0xD121);send_data_cmd(0x84);
	send_ctrl_cmd(0xD122);send_data_cmd(0x02);
	send_ctrl_cmd(0xD123);send_data_cmd(0xA4);
	send_ctrl_cmd(0xD124);send_data_cmd(0x02);
	send_ctrl_cmd(0xD125);send_data_cmd(0xB7);
	send_ctrl_cmd(0xD126);send_data_cmd(0x02);
	send_ctrl_cmd(0xD127);send_data_cmd(0xCF);
	send_ctrl_cmd(0xD128);send_data_cmd(0x02);
	send_ctrl_cmd(0xD129);send_data_cmd(0xDE);
	send_ctrl_cmd(0xD12A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD12B);send_data_cmd(0xF2);
	send_ctrl_cmd(0xD12C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD12D);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD12E);send_data_cmd(0x03);
	send_ctrl_cmd(0xD12F);send_data_cmd(0x10);
	send_ctrl_cmd(0xD130);send_data_cmd(0x03);
	send_ctrl_cmd(0xD131);send_data_cmd(0x33);
	send_ctrl_cmd(0xD132);send_data_cmd(0x03);
	send_ctrl_cmd(0xD133);send_data_cmd(0x6D);

	send_ctrl_cmd(0xD200);send_data_cmd(0x00);
	send_ctrl_cmd(0xD201);send_data_cmd(0x33);
	send_ctrl_cmd(0xD202);send_data_cmd(0x00);
	send_ctrl_cmd(0xD203);send_data_cmd(0x34);
	send_ctrl_cmd(0xD204);send_data_cmd(0x00);
	send_ctrl_cmd(0xD205);send_data_cmd(0x3A);
	send_ctrl_cmd(0xD206);send_data_cmd(0x00);
	send_ctrl_cmd(0xD207);send_data_cmd(0x4A);
	send_ctrl_cmd(0xD208);send_data_cmd(0x00);
	send_ctrl_cmd(0xD209);send_data_cmd(0x5C);
	send_ctrl_cmd(0xD20A);send_data_cmd(0x00);
	send_ctrl_cmd(0xD20B);send_data_cmd(0x81);
	send_ctrl_cmd(0xD20C);send_data_cmd(0x00);
	send_ctrl_cmd(0xD20D);send_data_cmd(0xA6);
	send_ctrl_cmd(0xD20E);send_data_cmd(0x00);
	send_ctrl_cmd(0xD20F);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD210);send_data_cmd(0x01);
	send_ctrl_cmd(0xD211);send_data_cmd(0x13);
	send_ctrl_cmd(0xD212);send_data_cmd(0x01);
	send_ctrl_cmd(0xD213);send_data_cmd(0x54);
	send_ctrl_cmd(0xD214);send_data_cmd(0x01);
	send_ctrl_cmd(0xD215);send_data_cmd(0x82);
	send_ctrl_cmd(0xD216);send_data_cmd(0x01);
	send_ctrl_cmd(0xD217);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD218);send_data_cmd(0x02);
	send_ctrl_cmd(0xD219);send_data_cmd(0x00);
	send_ctrl_cmd(0xD21A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD21B);send_data_cmd(0x01);
	send_ctrl_cmd(0xD21C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD21D);send_data_cmd(0x34);
	send_ctrl_cmd(0xD21E);send_data_cmd(0x02);
	send_ctrl_cmd(0xD21F);send_data_cmd(0x67);
	send_ctrl_cmd(0xD220);send_data_cmd(0x02);
	send_ctrl_cmd(0xD221);send_data_cmd(0x84);
	send_ctrl_cmd(0xD222);send_data_cmd(0x02);
	send_ctrl_cmd(0xD223);send_data_cmd(0xA4);
	send_ctrl_cmd(0xD224);send_data_cmd(0x02);
	send_ctrl_cmd(0xD225);send_data_cmd(0xB7);
	send_ctrl_cmd(0xD226);send_data_cmd(0x02);
	send_ctrl_cmd(0xD227);send_data_cmd(0xCF);
	send_ctrl_cmd(0xD228);send_data_cmd(0x02);
	send_ctrl_cmd(0xD229);send_data_cmd(0xDE);
	send_ctrl_cmd(0xD22A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD22B);send_data_cmd(0xF2);
	send_ctrl_cmd(0xD22C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD22D);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD22E);send_data_cmd(0x03);
	send_ctrl_cmd(0xD22F);send_data_cmd(0x10);
	send_ctrl_cmd(0xD230);send_data_cmd(0x03);
	send_ctrl_cmd(0xD231);send_data_cmd(0x33);
	send_ctrl_cmd(0xD232);send_data_cmd(0x03);
	send_ctrl_cmd(0xD233);send_data_cmd(0x6D);

	send_ctrl_cmd(0xD300);send_data_cmd(0x00);
	send_ctrl_cmd(0xD301);send_data_cmd(0x33);
	send_ctrl_cmd(0xD302);send_data_cmd(0x00);
	send_ctrl_cmd(0xD303);send_data_cmd(0x34);
	send_ctrl_cmd(0xD304);send_data_cmd(0x00);
	send_ctrl_cmd(0xD305);send_data_cmd(0x3A);
	send_ctrl_cmd(0xD306);send_data_cmd(0x00);
	send_ctrl_cmd(0xD307);send_data_cmd(0x4A);
	send_ctrl_cmd(0xD308);send_data_cmd(0x00);
	send_ctrl_cmd(0xD309);send_data_cmd(0x5C);
	send_ctrl_cmd(0xD30A);send_data_cmd(0x00);
	send_ctrl_cmd(0xD30B);send_data_cmd(0x81);
	send_ctrl_cmd(0xD30C);send_data_cmd(0x00);
	send_ctrl_cmd(0xD30D);send_data_cmd(0xA6);
	send_ctrl_cmd(0xD30E);send_data_cmd(0x00);
	send_ctrl_cmd(0xD30F);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD310);send_data_cmd(0x01);
	send_ctrl_cmd(0xD311);send_data_cmd(0x13);
	send_ctrl_cmd(0xD312);send_data_cmd(0x01);
	send_ctrl_cmd(0xD313);send_data_cmd(0x54);
	send_ctrl_cmd(0xD314);send_data_cmd(0x01);
	send_ctrl_cmd(0xD315);send_data_cmd(0x82);
	send_ctrl_cmd(0xD316);send_data_cmd(0x01);
	send_ctrl_cmd(0xD317);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD318);send_data_cmd(0x02);
	send_ctrl_cmd(0xD319);send_data_cmd(0x00);
	send_ctrl_cmd(0xD31A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD31B);send_data_cmd(0x01);
	send_ctrl_cmd(0xD31C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD31D);send_data_cmd(0x34);
	send_ctrl_cmd(0xD31E);send_data_cmd(0x02);
	send_ctrl_cmd(0xD31F);send_data_cmd(0x67);
	send_ctrl_cmd(0xD320);send_data_cmd(0x02);
	send_ctrl_cmd(0xD321);send_data_cmd(0x84);
	send_ctrl_cmd(0xD322);send_data_cmd(0x02);
	send_ctrl_cmd(0xD323);send_data_cmd(0xA4);
	send_ctrl_cmd(0xD324);send_data_cmd(0x02);
	send_ctrl_cmd(0xD325);send_data_cmd(0xB7);
	send_ctrl_cmd(0xD326);send_data_cmd(0x02);
	send_ctrl_cmd(0xD327);send_data_cmd(0xCF);
	send_ctrl_cmd(0xD328);send_data_cmd(0x02);
	send_ctrl_cmd(0xD329);send_data_cmd(0xDE);
	send_ctrl_cmd(0xD32A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD32B);send_data_cmd(0xF2);
	send_ctrl_cmd(0xD32C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD32D);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD32E);send_data_cmd(0x03);
	send_ctrl_cmd(0xD32F);send_data_cmd(0x10);
	send_ctrl_cmd(0xD330);send_data_cmd(0x03);
	send_ctrl_cmd(0xD331);send_data_cmd(0x33);
	send_ctrl_cmd(0xD332);send_data_cmd(0x03);
	send_ctrl_cmd(0xD333);send_data_cmd(0x6D);

	send_ctrl_cmd(0xD400);send_data_cmd(0x00);
	send_ctrl_cmd(0xD401);send_data_cmd(0x33);
	send_ctrl_cmd(0xD402);send_data_cmd(0x00);
	send_ctrl_cmd(0xD403);send_data_cmd(0x34);
	send_ctrl_cmd(0xD404);send_data_cmd(0x00);
	send_ctrl_cmd(0xD405);send_data_cmd(0x3A);
	send_ctrl_cmd(0xD406);send_data_cmd(0x00);
	send_ctrl_cmd(0xD407);send_data_cmd(0x4A);
	send_ctrl_cmd(0xD408);send_data_cmd(0x00);
	send_ctrl_cmd(0xD409);send_data_cmd(0x5C);
	send_ctrl_cmd(0xD40A);send_data_cmd(0x00);
	send_ctrl_cmd(0xD40B);send_data_cmd(0x81);
	send_ctrl_cmd(0xD40C);send_data_cmd(0x00);
	send_ctrl_cmd(0xD40D);send_data_cmd(0xA6);
	send_ctrl_cmd(0xD40E);send_data_cmd(0x00);
	send_ctrl_cmd(0xD40F);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD410);send_data_cmd(0x01);
	send_ctrl_cmd(0xD411);send_data_cmd(0x13);
	send_ctrl_cmd(0xD412);send_data_cmd(0x01);
	send_ctrl_cmd(0xD413);send_data_cmd(0x54);
	send_ctrl_cmd(0xD414);send_data_cmd(0x01);
	send_ctrl_cmd(0xD415);send_data_cmd(0x82);
	send_ctrl_cmd(0xD416);send_data_cmd(0x01);
	send_ctrl_cmd(0xD417);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD418);send_data_cmd(0x02);
	send_ctrl_cmd(0xD419);send_data_cmd(0x00);
	send_ctrl_cmd(0xD41A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD41B);send_data_cmd(0x01);
	send_ctrl_cmd(0xD41C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD41D);send_data_cmd(0x34);
	send_ctrl_cmd(0xD41E);send_data_cmd(0x02);
	send_ctrl_cmd(0xD41F);send_data_cmd(0x67);
	send_ctrl_cmd(0xD420);send_data_cmd(0x02);
	send_ctrl_cmd(0xD421);send_data_cmd(0x84);
	send_ctrl_cmd(0xD422);send_data_cmd(0x02);
	send_ctrl_cmd(0xD423);send_data_cmd(0xA4);
	send_ctrl_cmd(0xD424);send_data_cmd(0x02);
	send_ctrl_cmd(0xD425);send_data_cmd(0xB7);
	send_ctrl_cmd(0xD426);send_data_cmd(0x02);
	send_ctrl_cmd(0xD427);send_data_cmd(0xCF);
	send_ctrl_cmd(0xD428);send_data_cmd(0x02);
	send_ctrl_cmd(0xD429);send_data_cmd(0xDE);
	send_ctrl_cmd(0xD42A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD42B);send_data_cmd(0xF2);
	send_ctrl_cmd(0xD42C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD42D);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD42E);send_data_cmd(0x03);
	send_ctrl_cmd(0xD42F);send_data_cmd(0x10);
	send_ctrl_cmd(0xD430);send_data_cmd(0x03);
	send_ctrl_cmd(0xD431);send_data_cmd(0x33);
	send_ctrl_cmd(0xD432);send_data_cmd(0x03);
	send_ctrl_cmd(0xD433);send_data_cmd(0x6D);

	send_ctrl_cmd(0xD500);send_data_cmd(0x00);
	send_ctrl_cmd(0xD501);send_data_cmd(0x33);
	send_ctrl_cmd(0xD502);send_data_cmd(0x00);
	send_ctrl_cmd(0xD503);send_data_cmd(0x34);
	send_ctrl_cmd(0xD504);send_data_cmd(0x00);
	send_ctrl_cmd(0xD505);send_data_cmd(0x3A);
	send_ctrl_cmd(0xD506);send_data_cmd(0x00);
	send_ctrl_cmd(0xD507);send_data_cmd(0x4A);
	send_ctrl_cmd(0xD508);send_data_cmd(0x00);
	send_ctrl_cmd(0xD509);send_data_cmd(0x5C);
	send_ctrl_cmd(0xD50A);send_data_cmd(0x00);
	send_ctrl_cmd(0xD50B);send_data_cmd(0x81);
	send_ctrl_cmd(0xD50C);send_data_cmd(0x00);
	send_ctrl_cmd(0xD50D);send_data_cmd(0xA6);
	send_ctrl_cmd(0xD50E);send_data_cmd(0x00);
	send_ctrl_cmd(0xD50F);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD510);send_data_cmd(0x01);
	send_ctrl_cmd(0xD511);send_data_cmd(0x13);
	send_ctrl_cmd(0xD512);send_data_cmd(0x01);
	send_ctrl_cmd(0xD513);send_data_cmd(0x54);
	send_ctrl_cmd(0xD514);send_data_cmd(0x01);
	send_ctrl_cmd(0xD515);send_data_cmd(0x82);
	send_ctrl_cmd(0xD516);send_data_cmd(0x01);
	send_ctrl_cmd(0xD517);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD518);send_data_cmd(0x02);
	send_ctrl_cmd(0xD519);send_data_cmd(0x00);
	send_ctrl_cmd(0xD51A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD51B);send_data_cmd(0x01);
	send_ctrl_cmd(0xD51C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD51D);send_data_cmd(0x34);
	send_ctrl_cmd(0xD51E);send_data_cmd(0x02);
	send_ctrl_cmd(0xD51F);send_data_cmd(0x67);
	send_ctrl_cmd(0xD520);send_data_cmd(0x02);
	send_ctrl_cmd(0xD521);send_data_cmd(0x84);
	send_ctrl_cmd(0xD522);send_data_cmd(0x02);
	send_ctrl_cmd(0xD523);send_data_cmd(0xA4);
	send_ctrl_cmd(0xD524);send_data_cmd(0x02);
	send_ctrl_cmd(0xD525);send_data_cmd(0xB7);
	send_ctrl_cmd(0xD526);send_data_cmd(0x02);
	send_ctrl_cmd(0xD527);send_data_cmd(0xCF);
	send_ctrl_cmd(0xD528);send_data_cmd(0x02);
	send_ctrl_cmd(0xD529);send_data_cmd(0xDE);
	send_ctrl_cmd(0xD52A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD52B);send_data_cmd(0xF2);
	send_ctrl_cmd(0xD52C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD52D);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD52E);send_data_cmd(0x03);
	send_ctrl_cmd(0xD52F);send_data_cmd(0x10);
	send_ctrl_cmd(0xD530);send_data_cmd(0x03);
	send_ctrl_cmd(0xD531);send_data_cmd(0x33);
	send_ctrl_cmd(0xD532);send_data_cmd(0x03);
	send_ctrl_cmd(0xD533);send_data_cmd(0x6D);

	send_ctrl_cmd(0xD600);send_data_cmd(0x00);
	send_ctrl_cmd(0xD601);send_data_cmd(0x33);
	send_ctrl_cmd(0xD602);send_data_cmd(0x00);
	send_ctrl_cmd(0xD603);send_data_cmd(0x34);
	send_ctrl_cmd(0xD604);send_data_cmd(0x00);
	send_ctrl_cmd(0xD605);send_data_cmd(0x3A);
	send_ctrl_cmd(0xD606);send_data_cmd(0x00);
	send_ctrl_cmd(0xD607);send_data_cmd(0x4A);
	send_ctrl_cmd(0xD608);send_data_cmd(0x00);
	send_ctrl_cmd(0xD609);send_data_cmd(0x5C);
	send_ctrl_cmd(0xD60A);send_data_cmd(0x00);
	send_ctrl_cmd(0xD60B);send_data_cmd(0x81);
	send_ctrl_cmd(0xD60C);send_data_cmd(0x00);
	send_ctrl_cmd(0xD60D);send_data_cmd(0xA6);
	send_ctrl_cmd(0xD60E);send_data_cmd(0x00);
	send_ctrl_cmd(0xD60F);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD610);send_data_cmd(0x01);
	send_ctrl_cmd(0xD611);send_data_cmd(0x13);
	send_ctrl_cmd(0xD612);send_data_cmd(0x01);
	send_ctrl_cmd(0xD613);send_data_cmd(0x54);
	send_ctrl_cmd(0xD614);send_data_cmd(0x01);
	send_ctrl_cmd(0xD615);send_data_cmd(0x82);
	send_ctrl_cmd(0xD616);send_data_cmd(0x01);
	send_ctrl_cmd(0xD617);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD618);send_data_cmd(0x02);
	send_ctrl_cmd(0xD619);send_data_cmd(0x00);
	send_ctrl_cmd(0xD61A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD61B);send_data_cmd(0x01);
	send_ctrl_cmd(0xD61C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD61D);send_data_cmd(0x34);
	send_ctrl_cmd(0xD61E);send_data_cmd(0x02);
	send_ctrl_cmd(0xD61F);send_data_cmd(0x67);
	send_ctrl_cmd(0xD620);send_data_cmd(0x02);
	send_ctrl_cmd(0xD621);send_data_cmd(0x84);
	send_ctrl_cmd(0xD622);send_data_cmd(0x02);
	send_ctrl_cmd(0xD623);send_data_cmd(0xA4);
	send_ctrl_cmd(0xD624);send_data_cmd(0x02);
	send_ctrl_cmd(0xD625);send_data_cmd(0xB7);
	send_ctrl_cmd(0xD626);send_data_cmd(0x02);
	send_ctrl_cmd(0xD627);send_data_cmd(0xCF);
	send_ctrl_cmd(0xD628);send_data_cmd(0x02);
	send_ctrl_cmd(0xD629);send_data_cmd(0xDE);
	send_ctrl_cmd(0xD62A);send_data_cmd(0x02);
	send_ctrl_cmd(0xD62B);send_data_cmd(0xF2);
	send_ctrl_cmd(0xD62C);send_data_cmd(0x02);
	send_ctrl_cmd(0xD62D);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD62E);send_data_cmd(0x03);
	send_ctrl_cmd(0xD62F);send_data_cmd(0x10);
	send_ctrl_cmd(0xD630);send_data_cmd(0x03);
	send_ctrl_cmd(0xD631);send_data_cmd(0x33);
	send_ctrl_cmd(0xD632);send_data_cmd(0x03);
	send_ctrl_cmd(0xD633);send_data_cmd(0x6D);
	//*/


	//ENABLE PAGE 0
	send_ctrl_cmd(0xF000);send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002);send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);send_data_cmd(0x08);
	send_ctrl_cmd(0xF004);send_data_cmd(0x00);

	//SOURCE OUTPUT DATA HOLD TIME
	send_ctrl_cmd(0xB600);send_data_cmd(0x05);

	send_ctrl_cmd(0xB700);send_data_cmd(0x70);
	send_ctrl_cmd(0xB701);send_data_cmd(0x70);

	//RAM KEEP
	send_ctrl_cmd(0xB100);send_data_cmd(0xCC);
	send_ctrl_cmd(0xB101);send_data_cmd(0x00);
	//INVERSION
	send_ctrl_cmd(0xBC00);send_data_cmd(0x02);
	send_ctrl_cmd(0xBC01);send_data_cmd(0x00);
	send_ctrl_cmd(0xBC02);send_data_cmd(0x00);
	//SOURCE EQ
	send_ctrl_cmd(0xB800);send_data_cmd(0x01);
	send_ctrl_cmd(0xB801);send_data_cmd(0x03);
	send_ctrl_cmd(0xB802);send_data_cmd(0x03);
	send_ctrl_cmd(0xB803);send_data_cmd(0x03);


	//PORCH LINES
	send_ctrl_cmd(0xBD00);send_data_cmd(0x00);
	send_ctrl_cmd(0xBD01);send_data_cmd(0x78);   
	send_ctrl_cmd(0xBD02);send_data_cmd(0x07);   

	send_ctrl_cmd(0xBE02);send_data_cmd(0x07);
	send_ctrl_cmd(0xBE03);send_data_cmd(0x31);
	send_ctrl_cmd(0xBF02);send_data_cmd(0x07);
	send_ctrl_cmd(0xBF03);send_data_cmd(0x31);

	send_ctrl_cmd(0xFF00);send_data_cmd(0xAA);
	send_ctrl_cmd(0xFF01);send_data_cmd(0x55);
	send_ctrl_cmd(0xFF02);send_data_cmd(0x25);
	send_ctrl_cmd(0xFF03);send_data_cmd(0x01);

	//send_ctrl_cmd(0xF304);send_data_cmd(0x11);
	send_ctrl_cmd(0xF306);send_data_cmd(0x10);
	send_ctrl_cmd(0xF408);send_data_cmd(0x00);

	//TE ON
	send_ctrl_cmd(0x3500);send_data_cmd(0x00);
	//OTHER SET
	send_ctrl_cmd(0x3600);send_data_cmd(0x00);
	send_ctrl_cmd(0x3A00);send_data_cmd(0x77);
	//send_ctrl_cmd(0x2100);
	//SLEEP OUT 
	send_ctrl_cmd(0x1100);
	MDELAY(500);
	//DISPLY ON
	send_ctrl_cmd(0x2900);
	MDELAY(100);
#endif
}


static void config_gpio(void)
{
    const unsigned int USED_GPIOS[] = 
    {
        LSCE_GPIO_PIN,
        LSCK_GPIO_PIN,
        LSDA_GPIO_PIN,
        LSA0_GPIO_PIN
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

    // Swithc LSA0 pin to GPIO mode to avoid data contention,
    // since A0 is connected to LCM's SPI SDO pin
    //
    lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_DISP_LSA0_PIN_M_GPIO);
	lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);//input
	lcm_util.set_gpio_pull_enable(LSA0_GPIO_PIN, 0);
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

    params->dpi.clk_pol           = LCM_POLARITY_FALLING; //LCM_POLARITY_FALLING
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

    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(50);

    init_lcm_registers();
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
	MDELAY(20);

	set_lcm_register(0xF000,0x55);             
	set_lcm_register(0xF001,0xAA);             
	set_lcm_register(0xF002,0x52);             
	set_lcm_register(0xF003,0x08);             
	set_lcm_register(0xF004,0x01);
	
	send_ctrl_cmd(0xc500);
	id1 = spi_get_byte(0xc0);  //spi_read_data();
	MDELAY(20);
	send_ctrl_cmd(0xc501);
	id2 = spi_get_byte(0xc0);  //spi_read_data();

	id = (id1<<8)|id2;

#ifdef BUILD_UBOOT
	printf("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#else
	printk("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#endif

	return (LCM_ID == id)?1:0;
}


static void lcm_suspend(void)
{
     send_ctrl_cmd(0x2800);
    MDELAY(100);
	send_ctrl_cmd(0x1000);
}


static void lcm_resume(void)
{
	send_ctrl_cmd(0x1100);
	MDELAY(350);
	send_ctrl_cmd(0x2900);
	MDELAY(100); 
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER n3p4527_wvga_nt35510_lcm_drv = 
{

     .name			= "n3p4527_wvga_nt35510",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id = lcm_compare_id,
};
