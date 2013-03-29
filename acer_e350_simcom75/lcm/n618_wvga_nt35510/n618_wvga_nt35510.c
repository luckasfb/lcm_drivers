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

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)
#define NT35510_LCM_ID 0x5510

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

#define GET_LSA0_BIT     mt_get_gpio_in(LSA0_GPIO_PIN)

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

#if 1
static __inline unsigned char  spi_get_byte(unsigned int reg)
{
	unsigned int i;
	int k;
	unsigned char getbyte=0;

	// reg =  (CTRL_ID | (reg & 0xFF));
	SET_LSCK_HIGH;
	SET_LSDA_HIGH;
	UDELAY(1);
	SET_LSCE_LOW;
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
    for (k = 7; k >= 0; k--)
    {
	SET_LSCK_LOW;       
	UDELAY(5);
	SET_LSCK_HIGH;    
	UDELAY(5);
        if (GET_LSA0_BIT) {
            getbyte |= (1 << k);
        }

    }    

    UDELAY(2);    
     
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

    return getbyte;
}
#else
static __inline unsigned char spi_get_data(void) 
{
    unsigned int i;
	int k;
	unsigned char package=0xC0,reg_value=0;

    SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
		UDELAY(1);
        if (package & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        package <<= 1;
    }

	for(k = 7; k >= 0; k--)
	{
	   SET_LSCK_LOW;
       UDELAY(2);
	   
	   if(GET_LSA0_BIT)
	   {
	       reg_value |= (1 << k);
	   }
       SET_LSCK_HIGH;
	   UDELAY(2);
	}

    SET_LSCE_HIGH;
    SET_LSDA_HIGH;

	return reg_value;
}
#endif

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
//ENABLE PAGE 1
set_lcm_register(0xF000,0x55);
set_lcm_register(0xF001,0xAA);
set_lcm_register(0xF002,0x52);
set_lcm_register(0xF003,0x08);
set_lcm_register(0xF004,0x00);
set_lcm_register(0xB100,0x0C);
set_lcm_register(0xB101,0x00);

//VGMP/VGMN/VCOM SETING
set_lcm_register(0xBC00,0x05);
set_lcm_register(0xBC01,0x05);
set_lcm_register(0xBC02,0x05);
    set_lcm_register(0xBD02,0x07);
    set_lcm_register(0xBD03,0x1c);  //31
set_lcm_register(0xF000,0x55);
set_lcm_register(0xF001,0xAA);
set_lcm_register(0xF002,0x52);
set_lcm_register(0xF003,0x08);
set_lcm_register(0xF004,0x01);

//BGH VOLTAGE SETTING
set_lcm_register(0xB900,0x24);
set_lcm_register(0xB901,0x24);
set_lcm_register(0xB902,0x24);

//VGLX VOLTAGE SETTING
set_lcm_register(0xBA00,0x24);
set_lcm_register(0xBA01,0x24);
set_lcm_register(0xBA02,0x24);
set_lcm_register(0xBE00,0x00);//40
set_lcm_register(0xBE01,0x78);//78

//Z-INVERSION
set_lcm_register(0xBC00,0x00);
set_lcm_register(0xBC01,0x88);
set_lcm_register(0xBC02,0x01);
set_lcm_register(0xBD00,0x00);
set_lcm_register(0xBD01,0x88);
set_lcm_register(0xBD02,0x01);

//GAMMA SETING  RED
set_lcm_register(0xD100,0x00);
set_lcm_register(0xD101,0x00);
set_lcm_register(0xD102,0x00);
set_lcm_register(0xD103,0x16);
set_lcm_register(0xD104,0x00);
set_lcm_register(0xD105,0x42);
set_lcm_register(0xD106,0x00);
set_lcm_register(0xD107,0x61);
set_lcm_register(0xD108,0x00);
set_lcm_register(0xD109,0x74);
set_lcm_register(0xD10A,0x00);
set_lcm_register(0xD10B,0x97);
set_lcm_register(0xD10C,0x00);
set_lcm_register(0xD10D,0xAD);
set_lcm_register(0xD10E,0x00);
set_lcm_register(0xD10F,0xDE);
set_lcm_register(0xD110,0x01);
set_lcm_register(0xD111,0x00);
set_lcm_register(0xD112,0x01);
set_lcm_register(0xD113,0x26);
set_lcm_register(0xD114,0x01);
set_lcm_register(0xD115,0x50);
set_lcm_register(0xD116,0x01);
set_lcm_register(0xD117,0x87);
set_lcm_register(0xD118,0x01);
set_lcm_register(0xD119,0xB3);
set_lcm_register(0xD11A,0x01);
set_lcm_register(0xD11B,0xB6);
set_lcm_register(0xD11C,0x01);
set_lcm_register(0xD11D,0xDC);
set_lcm_register(0xD11E,0x02);
set_lcm_register(0xD11F,0x04);
set_lcm_register(0xD120,0x02);
set_lcm_register(0xD121,0x1C);
set_lcm_register(0xD122,0x02);
set_lcm_register(0xD123,0x34);
set_lcm_register(0xD124,0x02);
set_lcm_register(0xD125,0x4E);
set_lcm_register(0xD126,0x02);
set_lcm_register(0xD127,0x8A);
set_lcm_register(0xD128,0x02);
set_lcm_register(0xD129,0xC2);
set_lcm_register(0xD12A,0x03);
set_lcm_register(0xD12B,0x04);
set_lcm_register(0xD12C,0x03);
set_lcm_register(0xD12D,0x2E);
set_lcm_register(0xD12E,0x03);
set_lcm_register(0xD12F,0x74);
set_lcm_register(0xD130,0x03);
set_lcm_register(0xD131,0xEB);
set_lcm_register(0xD132,0x03);
set_lcm_register(0xD133,0xFF);

//GAMMA SETING  RED
set_lcm_register(0xD200,0x00);
set_lcm_register(0xD201,0x00);
set_lcm_register(0xD202,0x00);
set_lcm_register(0xD203,0x16);
set_lcm_register(0xD204,0x00);
set_lcm_register(0xD205,0x42);
set_lcm_register(0xD206,0x00);
set_lcm_register(0xD207,0x61);
set_lcm_register(0xD208,0x00);
set_lcm_register(0xD209,0x74);
set_lcm_register(0xD20A,0x00);
set_lcm_register(0xD20B,0x97);
set_lcm_register(0xD20C,0x00);
set_lcm_register(0xD20D,0xAD);
set_lcm_register(0xD20E,0x00);
set_lcm_register(0xD20F,0xDE);
set_lcm_register(0xD210,0x01);
set_lcm_register(0xD211,0x00);
set_lcm_register(0xD212,0x01);
set_lcm_register(0xD213,0x26);
set_lcm_register(0xD214,0x01);
set_lcm_register(0xD215,0x50);
set_lcm_register(0xD216,0x01);
set_lcm_register(0xD217,0x87);
set_lcm_register(0xD218,0x01);
set_lcm_register(0xD219,0xB3);
set_lcm_register(0xD21A,0x01);
set_lcm_register(0xD21B,0xB6);
set_lcm_register(0xD21C,0x01);
set_lcm_register(0xD21D,0xDC);
set_lcm_register(0xD21E,0x02);
set_lcm_register(0xD21F,0x04);
set_lcm_register(0xD220,0x02);
set_lcm_register(0xD221,0x1C);
set_lcm_register(0xD222,0x02);
set_lcm_register(0xD223,0x34);
set_lcm_register(0xD224,0x02);
set_lcm_register(0xD225,0x4E);
set_lcm_register(0xD226,0x02);
set_lcm_register(0xD227,0x8A);
set_lcm_register(0xD228,0x02);
set_lcm_register(0xD229,0xC2);
set_lcm_register(0xD22A,0x03);
set_lcm_register(0xD22B,0x04);
set_lcm_register(0xD22C,0x03);
set_lcm_register(0xD22D,0x2E);
set_lcm_register(0xD22E,0x03);
set_lcm_register(0xD22F,0x74);
set_lcm_register(0xD230,0x03);
set_lcm_register(0xD231,0xEB);
set_lcm_register(0xD232,0x03);
set_lcm_register(0xD233,0xFF);


//GAMMA SETING  RED
set_lcm_register(0xD300,0x00);
set_lcm_register(0xD301,0x00);
set_lcm_register(0xD302,0x00);
set_lcm_register(0xD303,0x16);
set_lcm_register(0xD304,0x00);
set_lcm_register(0xD305,0x42);
set_lcm_register(0xD306,0x00);
set_lcm_register(0xD307,0x61);
set_lcm_register(0xD308,0x00);
set_lcm_register(0xD309,0x74);
set_lcm_register(0xD30A,0x00);
set_lcm_register(0xD30B,0x97);
set_lcm_register(0xD30C,0x00);
set_lcm_register(0xD30D,0xAD);
set_lcm_register(0xD30E,0x00);
set_lcm_register(0xD30F,0xDE);
set_lcm_register(0xD310,0x01);
set_lcm_register(0xD311,0x00);
set_lcm_register(0xD312,0x01);
set_lcm_register(0xD313,0x26);
set_lcm_register(0xD314,0x01);
set_lcm_register(0xD315,0x50);
set_lcm_register(0xD316,0x01);
set_lcm_register(0xD317,0x87);
set_lcm_register(0xD318,0x01);
set_lcm_register(0xD319,0xB3);
set_lcm_register(0xD31A,0x01);
set_lcm_register(0xD31B,0xB6);
set_lcm_register(0xD31C,0x01);
set_lcm_register(0xD31D,0xDC);
set_lcm_register(0xD31E,0x02);
set_lcm_register(0xD31F,0x04);
set_lcm_register(0xD320,0x02);
set_lcm_register(0xD321,0x1C);
set_lcm_register(0xD322,0x02);
set_lcm_register(0xD323,0x34);
set_lcm_register(0xD324,0x02);
set_lcm_register(0xD325,0x4E);
set_lcm_register(0xD326,0x02);
set_lcm_register(0xD327,0x8A);
set_lcm_register(0xD328,0x02);
set_lcm_register(0xD329,0xC2);
set_lcm_register(0xD32A,0x03);
set_lcm_register(0xD32B,0x04);
set_lcm_register(0xD32C,0x03);
set_lcm_register(0xD32D,0x2E);
set_lcm_register(0xD32E,0x03);
set_lcm_register(0xD32F,0x74);
set_lcm_register(0xD330,0x03);
set_lcm_register(0xD331,0xEB);
set_lcm_register(0xD332,0x03);
set_lcm_register(0xD333,0xFF);


//GAMMA SETING  RED
set_lcm_register(0xD400,0x00);
set_lcm_register(0xD401,0x00);
set_lcm_register(0xD402,0x00);
set_lcm_register(0xD403,0x16);
set_lcm_register(0xD404,0x00);
set_lcm_register(0xD405,0x42);
set_lcm_register(0xD406,0x00);
set_lcm_register(0xD407,0x61);
set_lcm_register(0xD408,0x00);
set_lcm_register(0xD409,0x74);
set_lcm_register(0xD40A,0x00);
set_lcm_register(0xD40B,0x97);
set_lcm_register(0xD40C,0x00);
set_lcm_register(0xD40D,0xAD);
set_lcm_register(0xD40E,0x00);
set_lcm_register(0xD40F,0xDE);
set_lcm_register(0xD410,0x01);
set_lcm_register(0xD411,0x00);
set_lcm_register(0xD412,0x01);
set_lcm_register(0xD413,0x26);
set_lcm_register(0xD414,0x01);
set_lcm_register(0xD415,0x50);
set_lcm_register(0xD416,0x01);
set_lcm_register(0xD417,0x87);
set_lcm_register(0xD418,0x01);
set_lcm_register(0xD419,0xB3);
set_lcm_register(0xD41A,0x01);
set_lcm_register(0xD41B,0xB6);
set_lcm_register(0xD41C,0x01);
set_lcm_register(0xD41D,0xDC);
set_lcm_register(0xD41E,0x02);
set_lcm_register(0xD41F,0x04);
set_lcm_register(0xD420,0x02);
set_lcm_register(0xD421,0x1C);
set_lcm_register(0xD422,0x02);
set_lcm_register(0xD423,0x34);
set_lcm_register(0xD424,0x02);
set_lcm_register(0xD425,0x4E);
set_lcm_register(0xD426,0x02);
set_lcm_register(0xD427,0x8A);
set_lcm_register(0xD428,0x02);
set_lcm_register(0xD429,0xC2);
set_lcm_register(0xD42A,0x03);
set_lcm_register(0xD42B,0x04);
set_lcm_register(0xD42C,0x03);
set_lcm_register(0xD42D,0x2E);
set_lcm_register(0xD42E,0x03);
set_lcm_register(0xD42F,0x74);
set_lcm_register(0xD430,0x03);
set_lcm_register(0xD431,0xEB);
set_lcm_register(0xD432,0x03);
set_lcm_register(0xD433,0xFF);


//GAMMA SETING  RED
set_lcm_register(0xD500,0x00);
set_lcm_register(0xD501,0x00);
set_lcm_register(0xD502,0x00);
set_lcm_register(0xD503,0x16);
set_lcm_register(0xD504,0x00);
set_lcm_register(0xD505,0x42);
set_lcm_register(0xD506,0x00);
set_lcm_register(0xD507,0x61);
set_lcm_register(0xD508,0x00);
set_lcm_register(0xD509,0x74);
set_lcm_register(0xD50A,0x00);
set_lcm_register(0xD50B,0x97);
set_lcm_register(0xD50C,0x00);
set_lcm_register(0xD50D,0xAD);
set_lcm_register(0xD50E,0x00);
set_lcm_register(0xD50F,0xDE);
set_lcm_register(0xD510,0x01);
set_lcm_register(0xD511,0x00);
set_lcm_register(0xD512,0x01);
set_lcm_register(0xD513,0x26);
set_lcm_register(0xD514,0x01);
set_lcm_register(0xD515,0x50);
set_lcm_register(0xD516,0x01);
set_lcm_register(0xD517,0x87);
set_lcm_register(0xD518,0x01);
set_lcm_register(0xD519,0xB3);
set_lcm_register(0xD51A,0x01);
set_lcm_register(0xD51B,0xB6);
set_lcm_register(0xD51C,0x01);
set_lcm_register(0xD51D,0xDC);
set_lcm_register(0xD51E,0x02);
set_lcm_register(0xD51F,0x04);
set_lcm_register(0xD520,0x02);
set_lcm_register(0xD521,0x1C);
set_lcm_register(0xD522,0x02);
set_lcm_register(0xD523,0x34);
set_lcm_register(0xD524,0x02);
set_lcm_register(0xD525,0x4E);
set_lcm_register(0xD526,0x02);
set_lcm_register(0xD527,0x8A);
set_lcm_register(0xD528,0x02);
set_lcm_register(0xD529,0xC2);
set_lcm_register(0xD52A,0x03);
set_lcm_register(0xD52B,0x04);
set_lcm_register(0xD52C,0x03);
set_lcm_register(0xD52D,0x2E);
set_lcm_register(0xD52E,0x03);
set_lcm_register(0xD52F,0x74);
set_lcm_register(0xD530,0x03);
set_lcm_register(0xD531,0xEB);
set_lcm_register(0xD532,0x03);
set_lcm_register(0xD533,0xFF);



//GAMMA SETING  RED
set_lcm_register(0xD600,0x00);
set_lcm_register(0xD601,0x00);
set_lcm_register(0xD602,0x00);
set_lcm_register(0xD603,0x16);
set_lcm_register(0xD604,0x00);
set_lcm_register(0xD605,0x42);
set_lcm_register(0xD606,0x00);
set_lcm_register(0xD607,0x61);
set_lcm_register(0xD608,0x00);
set_lcm_register(0xD609,0x74);
set_lcm_register(0xD60A,0x00);
set_lcm_register(0xD60B,0x97);
set_lcm_register(0xD60C,0x00);
set_lcm_register(0xD60D,0xAD);
set_lcm_register(0xD60E,0x00);
set_lcm_register(0xD60F,0xDE);
set_lcm_register(0xD610,0x01);
set_lcm_register(0xD611,0x00);
set_lcm_register(0xD612,0x01);
set_lcm_register(0xD613,0x26);
set_lcm_register(0xD614,0x01);
set_lcm_register(0xD615,0x50);
set_lcm_register(0xD616,0x01);
set_lcm_register(0xD617,0x87);
set_lcm_register(0xD618,0x01);
set_lcm_register(0xD619,0xB3);
set_lcm_register(0xD61A,0x01);
set_lcm_register(0xD61B,0xB6);
set_lcm_register(0xD61C,0x01);
set_lcm_register(0xD61D,0xDC);
set_lcm_register(0xD61E,0x02);
set_lcm_register(0xD61F,0x04);
set_lcm_register(0xD620,0x02);
set_lcm_register(0xD621,0x1C);
set_lcm_register(0xD622,0x02);
set_lcm_register(0xD623,0x34);
set_lcm_register(0xD624,0x02);
set_lcm_register(0xD625,0x4E);
set_lcm_register(0xD626,0x02);
set_lcm_register(0xD627,0x8A);
set_lcm_register(0xD628,0x02);
set_lcm_register(0xD629,0xC2);
set_lcm_register(0xD62A,0x03);
set_lcm_register(0xD62B,0x04);
set_lcm_register(0xD62C,0x03);
set_lcm_register(0xD62D,0x2E);
set_lcm_register(0xD62E,0x03);
set_lcm_register(0xD62F,0x74);
set_lcm_register(0xD630,0x03);
set_lcm_register(0xD631,0xEB);
set_lcm_register(0xD632,0x03);
set_lcm_register(0xD633,0xFF);
    send_ctrl_cmd(0x1100);//Sleep out
    MDELAY(300);
    
    send_ctrl_cmd(0x2900);//Display on
    MDELAY(200);

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

   // lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_DISP_LSA0_PIN_M_GPIO);
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

    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(50);

    init_lcm_registers();
}

static void lcm_suspend(void)
{
#ifdef BUILD_UBOOT
	//printf("1111111111lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#else
	//printk("11111111111lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#endif

     send_ctrl_cmd(0x2800);
    MDELAY(100);
	send_ctrl_cmd(0x1000);
	MDELAY(150);
}


static void lcm_resume(void)
{
    send_ctrl_cmd(0x1100);
    MDELAY(150);
	send_ctrl_cmd(0x2900);
	 MDELAY(100);
}

static unsigned int lcm_compare_id(void)
{
	unsigned int    id1 = 0;
	unsigned int    id2 = 0;
	unsigned int    id = 0;

#if 1
      config_gpio();

	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);

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
#else
	config_gpio();

	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(150);
	SET_RESET_PIN(1);
	MDELAY(50);

	set_lcm_register(0xF000,0x55);             
	set_lcm_register(0xF001,0xAA);             
	set_lcm_register(0xF002,0x52);             
	set_lcm_register(0xF003,0x08);             
	set_lcm_register(0xF004,0x01);
	
	send_ctrl_cmd(0xc500);
	id1 = spi_get_data();  //spi_read_data();
	MDELAY(20);
	send_ctrl_cmd(0xc501);
	id2 = spi_get_data();  //spi_read_data();

	id = (id1<<8)|id2;

#ifdef BUILD_UBOOT
	printf("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#else
	printk("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#endif

#endif
	return (NT35510_LCM_ID == id)?1:0;
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER n618_wvga_nt35510_lcm_drv = 
{

     .name			= "n618_wvga_nt35510",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id   = lcm_compare_id,
};
