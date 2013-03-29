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
//#include <linux/kernel.h>

#include "lcm_drv.h"

#if BUILD_UBOOT
//#include <linux/kernel.h>
#define	LCD_PRINT	printf
#else
#include <linux/kernel.h>

#define	LCD_PRINT	printk
#endif

/*
 * Notes: As for nt35512, register is 16 bits, but data is 8 bits.
 *        While writing, by non-nokia data format.
 *        While reading, by nokia data format.
 * Author: chu, zewei
 * Date:   2012/09/26
 */

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (854)

#define LCM_ID        (0x5510) 	//  reg 04h , the 2nd and 3rd parameters.
#define LCM_ID_REG	   (0xC500)		// 0x0400

#define LCM_NAME		"[CHANGXINGDA463_NT35510]"

#define	TE_CONTROL

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
static unsigned int get_lcd_id_r(unsigned int addr);
static unsigned int get_lcd_id_n(unsigned int addr, unsigned char count);

static __inline unsigned int HIGH_BYTE(unsigned int val)
{
    return (val >> 8) & 0xFF;
}

static __inline unsigned int LOW_BYTE(unsigned int val)
{
    return (val & 0xFF);
}

static __inline void send_ctrl_cmd(unsigned int cmd)
{
	lcm_util.send_cmd(cmd);
}

static __inline void send_data_cmd(unsigned int data)
{
	
	lcm_util.send_data(data);

}

static __inline unsigned int read_data_cmd()
{
	unsigned int id = 0;
	id = lcm_util.read_data();
	//id = (id << 8) | (lcm_util.read_data() & 0xff);
    return id;
}

//static __inline void set_lcm_register(unsigned int regIndex, unsigned int regData, unsigned int uiDelay)
static __inline void set_lcm_register(unsigned int regIndex, unsigned int regData)
{
	send_ctrl_cmd(regIndex);
    send_data_cmd(regData);

	//if (uiDelay > 0)
	//	MDELAY(uiDelay);
}

u8 vcom = 0xA3;
static void  lcm_update_black(unsigned int x, unsigned int y,unsigned int width, unsigned int height, unsigned short data)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width;
    unsigned int y1 = y0 + height + 2;
    unsigned int k, i;

	set_lcm_register(0x2A00, HIGH_BYTE(x0));
	set_lcm_register(0x2A01, LOW_BYTE(x0));
	set_lcm_register(0x2A02, HIGH_BYTE(x1));
	set_lcm_register(0x2A03, LOW_BYTE(x1));
	set_lcm_register(0x2B00, HIGH_BYTE(y0));
	set_lcm_register(0x2B01, LOW_BYTE(y0));
	set_lcm_register(0x2B02, HIGH_BYTE(y1));  
	set_lcm_register(0x2B03, LOW_BYTE(y1));

	send_ctrl_cmd(0x2C00);

	for (i = x0; i < x1; i++)
	{
		for (k = y0; k < y1; k++)
		{
			send_data_cmd(data);
			send_data_cmd(data);
		}
	}
	
}

//static unsigned int vddhh = 0x69;

static void init_lcm_registers(void)
{ 
		// LV2 Page 1 enable
	set_lcm_register(0xF000,0x55);
	set_lcm_register(0xF001,0xAA);
	set_lcm_register(0xF002,0x52);
	set_lcm_register(0xF003,0x08);
	set_lcm_register(0xF004,0x01);

	// AVDD Set AVDD 5.2V
	set_lcm_register(0xB000,0x0D);
	set_lcm_register(0xB001,0x0D);
	set_lcm_register(0xB002,0x0D);

	// AVDD ratio
	set_lcm_register(0xB600,0x34);
	set_lcm_register(0xB601,0x34);
	set_lcm_register(0xB602,0x34);
	 
	// AVEE  -5.2V
	set_lcm_register(0xB100,0x0D);
	set_lcm_register(0xB101,0x0D);
	set_lcm_register(0xB102,0x0D);

	// AVEE ratio
	set_lcm_register(0xB700,0x34);
	set_lcm_register(0xB701,0x34);
	set_lcm_register(0xB702,0x34);

	// VCL  -2.5V
	set_lcm_register(0xB200,0x00);
	set_lcm_register(0xB201,0x00);
	set_lcm_register(0xB202,0x00);

	// VCL ratio
	set_lcm_register(0xB800,0x24);
	set_lcm_register(0xB801,0x24);
	set_lcm_register(0xB802,0x24);


	// VGH 15V  (Free pump)
	set_lcm_register(0xBF00,0x01);
	set_lcm_register(0xB300,0x0F);
	set_lcm_register(0xB301,0x0F);
	set_lcm_register(0xB302,0x0F);

	// VGH ratio
	set_lcm_register(0xB900,0x34);
	set_lcm_register(0xB901,0x34);
	set_lcm_register(0xB902,0x34);

	// VGL_REG -10V
	set_lcm_register(0xB500,0x08);
	set_lcm_register(0xB501,0x08);
	set_lcm_register(0xB502,0x08);

	set_lcm_register(0xC200,0x03);

	// VGLX ratio
	set_lcm_register(0xBA00,0x24);
	set_lcm_register(0xBA01,0x24);
	set_lcm_register(0xBA02,0x24);

	// VGMP/VGSP 4.5V/0V
	set_lcm_register(0xBC00,0x00);
	set_lcm_register(0xBC01,0x78);
	set_lcm_register(0xBC02,0x00);

	// VGMN/VGSN -4.5V/0V
	set_lcm_register(0xBD00,0x00);
	set_lcm_register(0xBD01,0x78);
	set_lcm_register(0xBD02,0x00);

	// VCOM  
	set_lcm_register(0xBE00,0x00);
	set_lcm_register(0xBE01,0x64);	// 0x64   0xA3
//	vcom -= 2;

	// Gamma Setting
	set_lcm_register(0xD100,0x00);
	set_lcm_register(0xD101,0x33);
	set_lcm_register(0xD102,0x00);
	set_lcm_register(0xD103,0x34);
	set_lcm_register(0xD104,0x00);
	set_lcm_register(0xD105,0x3A);
	set_lcm_register(0xD106,0x00);
	set_lcm_register(0xD107,0x4A);
	set_lcm_register(0xD108,0x00);
	set_lcm_register(0xD109,0x5C);
	set_lcm_register(0xD10A,0x00);
	set_lcm_register(0xD10B,0x81);
	set_lcm_register(0xD10C,0x00);
	set_lcm_register(0xD10D,0xA6);
	set_lcm_register(0xD10E,0x00);
	set_lcm_register(0xD10F,0xE5);
	set_lcm_register(0xD110,0x01);
	set_lcm_register(0xD111,0x13);
	set_lcm_register(0xD112,0x01);
	set_lcm_register(0xD113,0x54);
	set_lcm_register(0xD114,0x01);
	set_lcm_register(0xD115,0x82);
	set_lcm_register(0xD116,0x01);
	set_lcm_register(0xD117,0xCA);
	set_lcm_register(0xD118,0x02);
	set_lcm_register(0xD119,0x00);
	set_lcm_register(0xD11A,0x02);
	set_lcm_register(0xD11B,0x01);
	set_lcm_register(0xD11C,0x02);
	set_lcm_register(0xD11D,0x34);
	set_lcm_register(0xD11E,0x02);
	set_lcm_register(0xD11F,0x67);
	set_lcm_register(0xD120,0x02);
	set_lcm_register(0xD121,0x84);
	set_lcm_register(0xD122,0x02);
	set_lcm_register(0xD123,0xA4);
	set_lcm_register(0xD124,0x02);
	set_lcm_register(0xD125,0xB7);
	set_lcm_register(0xD126,0x02);
	set_lcm_register(0xD127,0xCF);
	set_lcm_register(0xD128,0x02);
	set_lcm_register(0xD129,0xDE);
	set_lcm_register(0xD12A,0x02);
	set_lcm_register(0xD12B,0xF2);
	set_lcm_register(0xD12C,0x02);
	set_lcm_register(0xD12D,0xFE);
	set_lcm_register(0xD12E,0x03);
	set_lcm_register(0xD12F,0x10);
	set_lcm_register(0xD130,0x03);
	set_lcm_register(0xD131,0x33);
	set_lcm_register(0xD132,0x03);
	set_lcm_register(0xD133,0x6D);

	set_lcm_register(0xD200,0x00);
	set_lcm_register(0xD201,0x33);
	set_lcm_register(0xD202,0x00);
	set_lcm_register(0xD203,0x34);
	set_lcm_register(0xD204,0x00);
	set_lcm_register(0xD205,0x3A);
	set_lcm_register(0xD206,0x00);
	set_lcm_register(0xD207,0x4A);
	set_lcm_register(0xD208,0x00);
	set_lcm_register(0xD209,0x5C);
	set_lcm_register(0xD20A,0x00);
	set_lcm_register(0xD20B,0x81);
	set_lcm_register(0xD20C,0x00);
	set_lcm_register(0xD20D,0xA6);
	set_lcm_register(0xD20E,0x00);
	set_lcm_register(0xD20F,0xE5);
	set_lcm_register(0xD210,0x01);
	set_lcm_register(0xD211,0x13);
	set_lcm_register(0xD212,0x01);
	set_lcm_register(0xD213,0x54);
	set_lcm_register(0xD214,0x01);
	set_lcm_register(0xD215,0x82);
	set_lcm_register(0xD216,0x01);
	set_lcm_register(0xD217,0xCA);
	set_lcm_register(0xD218,0x02);
	set_lcm_register(0xD219,0x00);
	set_lcm_register(0xD21A,0x02);
	set_lcm_register(0xD21B,0x01);
	set_lcm_register(0xD21C,0x02);
	set_lcm_register(0xD21D,0x34);
	set_lcm_register(0xD21E,0x02);
	set_lcm_register(0xD21F,0x67);
	set_lcm_register(0xD220,0x02);
	set_lcm_register(0xD221,0x84);
	set_lcm_register(0xD222,0x02);
	set_lcm_register(0xD223,0xA4);
	set_lcm_register(0xD224,0x02);
	set_lcm_register(0xD225,0xB7);
	set_lcm_register(0xD226,0x02);
	set_lcm_register(0xD227,0xCF);
	set_lcm_register(0xD228,0x02);
	set_lcm_register(0xD229,0xDE);
	set_lcm_register(0xD22A,0x02);
	set_lcm_register(0xD22B,0xF2);
	set_lcm_register(0xD22C,0x02);
	set_lcm_register(0xD22D,0xFE);
	set_lcm_register(0xD22E,0x03);
	set_lcm_register(0xD22F,0x10);
	set_lcm_register(0xD230,0x03);
	set_lcm_register(0xD231,0x33);
	set_lcm_register(0xD232,0x03);
	set_lcm_register(0xD233,0x6D);

	set_lcm_register(0xD300,0x00);
	set_lcm_register(0xD301,0x33);
	set_lcm_register(0xD302,0x00);
	set_lcm_register(0xD303,0x34);
	set_lcm_register(0xD304,0x00);
	set_lcm_register(0xD305,0x3A);
	set_lcm_register(0xD306,0x00);
	set_lcm_register(0xD307,0x4A);
	set_lcm_register(0xD308,0x00);
	set_lcm_register(0xD309,0x5C);
	set_lcm_register(0xD30A,0x00);
	set_lcm_register(0xD30B,0x81);
	set_lcm_register(0xD30C,0x00);
	set_lcm_register(0xD30D,0xA6);
	set_lcm_register(0xD30E,0x00);
	set_lcm_register(0xD30F,0xE5);
	set_lcm_register(0xD310,0x01);
	set_lcm_register(0xD311,0x13);
	set_lcm_register(0xD312,0x01);
	set_lcm_register(0xD313,0x54);
	set_lcm_register(0xD314,0x01);
	set_lcm_register(0xD315,0x82);
	set_lcm_register(0xD316,0x01);
	set_lcm_register(0xD317,0xCA);
	set_lcm_register(0xD318,0x02);
	set_lcm_register(0xD319,0x00);
	set_lcm_register(0xD31A,0x02);
	set_lcm_register(0xD31B,0x01);
	set_lcm_register(0xD31C,0x02);
	set_lcm_register(0xD31D,0x34);
	set_lcm_register(0xD31E,0x02);
	set_lcm_register(0xD31F,0x67);
	set_lcm_register(0xD320,0x02);
	set_lcm_register(0xD321,0x84);
	set_lcm_register(0xD322,0x02);
	set_lcm_register(0xD323,0xA4);
	set_lcm_register(0xD324,0x02);
	set_lcm_register(0xD325,0xB7);
	set_lcm_register(0xD326,0x02);
	set_lcm_register(0xD327,0xCF);
	set_lcm_register(0xD328,0x02);
	set_lcm_register(0xD329,0xDE);
	set_lcm_register(0xD32A,0x02);
	set_lcm_register(0xD32B,0xF2);
	set_lcm_register(0xD32C,0x02);
	set_lcm_register(0xD32D,0xFE);
	set_lcm_register(0xD32E,0x03);
	set_lcm_register(0xD32F,0x10);
	set_lcm_register(0xD330,0x03);
	set_lcm_register(0xD331,0x33);
	set_lcm_register(0xD332,0x03);
	set_lcm_register(0xD333,0x6D);

	set_lcm_register(0xD400,0x00);
	set_lcm_register(0xD401,0x33);
	set_lcm_register(0xD402,0x00);
	set_lcm_register(0xD403,0x34);
	set_lcm_register(0xD404,0x00);
	set_lcm_register(0xD405,0x3A);
	set_lcm_register(0xD406,0x00);
	set_lcm_register(0xD407,0x4A);
	set_lcm_register(0xD408,0x00);
	set_lcm_register(0xD409,0x5C);
	set_lcm_register(0xD40A,0x00);
	set_lcm_register(0xD40B,0x81);
	set_lcm_register(0xD40C,0x00);
	set_lcm_register(0xD40D,0xA6);
	set_lcm_register(0xD40E,0x00);
	set_lcm_register(0xD40F,0xE5);
	set_lcm_register(0xD410,0x01);
	set_lcm_register(0xD411,0x13);
	set_lcm_register(0xD412,0x01);
	set_lcm_register(0xD413,0x54);
	set_lcm_register(0xD414,0x01);
	set_lcm_register(0xD415,0x82);
	set_lcm_register(0xD416,0x01);
	set_lcm_register(0xD417,0xCA);
	set_lcm_register(0xD418,0x02);
	set_lcm_register(0xD419,0x00);
	set_lcm_register(0xD41A,0x02);
	set_lcm_register(0xD41B,0x01);
	set_lcm_register(0xD41C,0x02);
	set_lcm_register(0xD41D,0x34);
	set_lcm_register(0xD41E,0x02);
	set_lcm_register(0xD41F,0x67);
	set_lcm_register(0xD420,0x02);
	set_lcm_register(0xD421,0x84);
	set_lcm_register(0xD422,0x02);
	set_lcm_register(0xD423,0xA4);
	set_lcm_register(0xD424,0x02);
	set_lcm_register(0xD425,0xB7);
	set_lcm_register(0xD426,0x02);
	set_lcm_register(0xD427,0xCF);
	set_lcm_register(0xD428,0x02);
	set_lcm_register(0xD429,0xDE);
	set_lcm_register(0xD42A,0x02);
	set_lcm_register(0xD42B,0xF2);
	set_lcm_register(0xD42C,0x02);
	set_lcm_register(0xD42D,0xFE);
	set_lcm_register(0xD42E,0x03);
	set_lcm_register(0xD42F,0x10);
	set_lcm_register(0xD430,0x03);
	set_lcm_register(0xD431,0x33);
	set_lcm_register(0xD432,0x03);
	set_lcm_register(0xD433,0x6D);

	set_lcm_register(0xD500,0x00);
	set_lcm_register(0xD501,0x33);
	set_lcm_register(0xD502,0x00);
	set_lcm_register(0xD503,0x34);
	set_lcm_register(0xD504,0x00);
	set_lcm_register(0xD505,0x3A);
	set_lcm_register(0xD506,0x00);
	set_lcm_register(0xD507,0x4A);
	set_lcm_register(0xD508,0x00);
	set_lcm_register(0xD509,0x5C);
	set_lcm_register(0xD50A,0x00);
	set_lcm_register(0xD50B,0x81);
	set_lcm_register(0xD50C,0x00);
	set_lcm_register(0xD50D,0xA6);
	set_lcm_register(0xD50E,0x00);
	set_lcm_register(0xD50F,0xE5);
	set_lcm_register(0xD510,0x01);
	set_lcm_register(0xD511,0x13);
	set_lcm_register(0xD512,0x01);
	set_lcm_register(0xD513,0x54);
	set_lcm_register(0xD514,0x01);
	set_lcm_register(0xD515,0x82);
	set_lcm_register(0xD516,0x01);
	set_lcm_register(0xD517,0xCA);
	set_lcm_register(0xD518,0x02);
	set_lcm_register(0xD519,0x00);
	set_lcm_register(0xD51A,0x02);
	set_lcm_register(0xD51B,0x01);
	set_lcm_register(0xD51C,0x02);
	set_lcm_register(0xD51D,0x34);
	set_lcm_register(0xD51E,0x02);
	set_lcm_register(0xD51F,0x67);
	set_lcm_register(0xD520,0x02);
	set_lcm_register(0xD521,0x84);
	set_lcm_register(0xD522,0x02);
	set_lcm_register(0xD523,0xA4);
	set_lcm_register(0xD524,0x02);
	set_lcm_register(0xD525,0xB7);
	set_lcm_register(0xD526,0x02);
	set_lcm_register(0xD527,0xCF);
	set_lcm_register(0xD528,0x02);
	set_lcm_register(0xD529,0xDE);
	set_lcm_register(0xD52A,0x02);
	set_lcm_register(0xD52B,0xF2);
	set_lcm_register(0xD52C,0x02);
	set_lcm_register(0xD52D,0xFE);
	set_lcm_register(0xD52E,0x03);
	set_lcm_register(0xD52F,0x10);
	set_lcm_register(0xD530,0x03);
	set_lcm_register(0xD531,0x33);
	set_lcm_register(0xD532,0x03);
	set_lcm_register(0xD533,0x6D);

	set_lcm_register(0xD600,0x00);
	set_lcm_register(0xD601,0x33);
	set_lcm_register(0xD602,0x00);
	set_lcm_register(0xD603,0x34);
	set_lcm_register(0xD604,0x00);
	set_lcm_register(0xD605,0x3A);
	set_lcm_register(0xD606,0x00);
	set_lcm_register(0xD607,0x4A);
	set_lcm_register(0xD608,0x00);
	set_lcm_register(0xD609,0x5C);
	set_lcm_register(0xD60A,0x00);
	set_lcm_register(0xD60B,0x81);
	set_lcm_register(0xD60C,0x00);
	set_lcm_register(0xD60D,0xA6);
	set_lcm_register(0xD60E,0x00);
	set_lcm_register(0xD60F,0xE5);
	set_lcm_register(0xD610,0x01);
	set_lcm_register(0xD611,0x13);
	set_lcm_register(0xD612,0x01);
	set_lcm_register(0xD613,0x54);
	set_lcm_register(0xD614,0x01);
	set_lcm_register(0xD615,0x82);
	set_lcm_register(0xD616,0x01);
	set_lcm_register(0xD617,0xCA);
	set_lcm_register(0xD618,0x02);
	set_lcm_register(0xD619,0x00);
	set_lcm_register(0xD61A,0x02);
	set_lcm_register(0xD61B,0x01);
	set_lcm_register(0xD61C,0x02);
	set_lcm_register(0xD61D,0x34);
	set_lcm_register(0xD61E,0x02);
	set_lcm_register(0xD61F,0x67);
	set_lcm_register(0xD620,0x02);
	set_lcm_register(0xD621,0x84);
	set_lcm_register(0xD622,0x02);
	set_lcm_register(0xD623,0xA4);
	set_lcm_register(0xD624,0x02);
	set_lcm_register(0xD625,0xB7);
	set_lcm_register(0xD626,0x02);
	set_lcm_register(0xD627,0xCF);
	set_lcm_register(0xD628,0x02);
	set_lcm_register(0xD629,0xDE);
	set_lcm_register(0xD62A,0x02);
	set_lcm_register(0xD62B,0xF2);
	set_lcm_register(0xD62C,0x02);
	set_lcm_register(0xD62D,0xFE);
	set_lcm_register(0xD62E,0x03);
	set_lcm_register(0xD62F,0x10);
	set_lcm_register(0xD630,0x03);
	set_lcm_register(0xD631,0x33);
	set_lcm_register(0xD632,0x03);
	set_lcm_register(0xD633,0x6D);



	// LV2 Page 0 enable
	set_lcm_register(0xF000,0x55);
	set_lcm_register(0xF001,0xAA);
	set_lcm_register(0xF002,0x52);
	set_lcm_register(0xF003,0x08);
	set_lcm_register(0xF004,0x00);

	// Display control
	set_lcm_register(0xB100, 0xCC);
	set_lcm_register(0xB101, 0x00);

	//480x854
	send_ctrl_cmd(0xB500); send_data_cmd(0x6B);

	// Source hold time
	set_lcm_register(0xB600,0x05);

	// Gate EQ control
	set_lcm_register(0xB700,0x70);
	set_lcm_register(0xB701,0x70);

	// Source EQ control (Mode 2)
	set_lcm_register(0xB800,0x01);
	set_lcm_register(0xB801,0x03);
	set_lcm_register(0xB802,0x03);
	set_lcm_register(0xB803,0x03);


	// Inversion mode  (2-dot)
	set_lcm_register(0xBC00,0x02);	// 0x02
	set_lcm_register(0xBC01,0x00);
	set_lcm_register(0xBC02,0x00);

	// Timing control 4H w/ 4-delay 
	set_lcm_register(0xC900,0xD0);
	set_lcm_register(0xC901,0x02);
	set_lcm_register(0xC902,0x50);
	set_lcm_register(0xC903,0x50);
	set_lcm_register(0xC904,0x50);

#ifdef TE_CONTROL
	set_lcm_register(0x3500,0x00);
#endif

	set_lcm_register(0x3600,0x00);
	set_lcm_register(0x3A00,0x55);	//0x77

	set_lcm_register(0x2A00,0x00);
	set_lcm_register(0x2A01,0x00);
	set_lcm_register(0x2A02,HIGH_BYTE(FRAME_WIDTH));
	set_lcm_register(0x2A03,LOW_BYTE(FRAME_WIDTH));
	set_lcm_register(0x2B00,0x00);
	set_lcm_register(0x2B01,0x00);
	set_lcm_register(0x2B02,HIGH_BYTE(FRAME_HEIGHT));  
	set_lcm_register(0x2B03,LOW_BYTE(FRAME_HEIGHT));
	send_ctrl_cmd(0x2C00);
	MDELAY(50);
	send_ctrl_cmd(0x1100);
	MDELAY(120);
	//lcm_update_black(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0x00);
	send_ctrl_cmd(0x2900);
	MDELAY(50);
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

/*
	params->type   = LCM_TYPE_DBI;
    params->ctrl   = LCM_CTRL_PARALLEL_DBI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    
    params->dbi.port                    = 0;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_52M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_LSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB666;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_32_BITS;
    params->dbi.io_driving_current      = 0;

    params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_OR_HSYNC;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;
    params->dbi.te_hs_delay_cnt         = 50;
    params->dbi.te_vs_width_cnt         = 277;
    params->dbi.te_vs_width_cnt_div     = LCM_DBI_TE_VS_WIDTH_CNT_DIV_16;

    params->dbi.parallel.write_setup    = 2;
    params->dbi.parallel.write_hold     = 2;
    params->dbi.parallel.write_wait     = 4;
    params->dbi.parallel.read_setup     = 2;
    params->dbi.parallel.read_latency   = 9;
    params->dbi.parallel.wait_period    = 0;
 */

/*
 * ATTENTIONS: There are a few important differences under the MT6575/15 compared with the MT6573/13
 * 
 *  type: if set LCM_TYPE_DBI, cpu interface.
 *  io_select_mode: if set 0, selects bus composition: LPA0, LWRB, LRDB control bus and NLD[0:15] data bus
 *                  if set 1, selects bus composition: DPIVSYNC, DPIDE, DPIHSYNC control bus and DPIRGB data bus.
 *  dbi.port:  0 -- select parallel port 0; 1 -- select parallel port 1
 *
 * Author: chu, zewei 
 * Date:  2012/09/05
 */
static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type   = LCM_TYPE_DBI;
    params->ctrl   = LCM_CTRL_PARALLEL_DBI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
	params->io_select_mode = 1;	

	params->dbi.port                    = 0;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_104M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_16BITS; //LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST; // LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB565;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_16BITS; // LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_16_BITS; // LCM_DBI_CPU_WRITE_16_BITS;
	params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_6575_8MA; //LCM_DRIVING_CURRENT_8MA;
	
	params->dbi.parallel.write_setup    = 2;  // 2;
	params->dbi.parallel.write_hold     = 3;  // 3; // 3; // 2, 4
	params->dbi.parallel.write_wait     = 6;  // 6; //6;
	
	params->dbi.parallel.read_setup     = 2;  // 2; // 4; // 3;
	params->dbi.parallel.read_hold      = 3;  // 3;
    params->dbi.parallel.read_latency   = 18; // 18;
	
	params->dbi.parallel.wait_period    = 1;  // 2; // 2;
	params->dbi.parallel.cs_high_width  = 0;  // 1; //cycles of cs high level between each transfer
	//params->dbi.parallel.read_latency   = 20;  //40
	//params->dbi.parallel.wait_period    = 10; // 0

	#ifdef	TE_CONTROL
	// enable tearing-free
	/*
	if (LCM_DBI_TE_MODE_DISABLED == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_Enable(FALSE));
        return;
    }

    if (LCM_DBI_TE_MODE_VSYNC_ONLY == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_SetMode(LCD_TE_MODE_VSYNC_ONLY));
    } else if (LCM_DBI_TE_MODE_VSYNC_OR_HSYNC == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_SetMode(LCD_TE_MODE_VSYNC_OR_HSYNC));
        LCD_CHECK_RET(LCD_TE_ConfigVHSyncMode(dbi->te_hs_delay_cnt,
                                              dbi->te_vs_width_cnt,
                     (LCD_TE_VS_WIDTH_CNT_DIV)dbi->te_vs_width_cnt_div));
    } else ASSERT(0);

    LCD_CHECK_RET(LCD_TE_SetEdgePolarity(dbi->te_edge_polarity));
    LCD_CHECK_RET(LCD_TE_Enable(TRUE));
	*/
    params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_OR_HSYNC; //LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING; //LCM_POLARITY_FALLING;

	/*
	 * Notes: The body of the function LCD_TE_ConfigVHSyncMode() is void!
	 */
	params->dbi.te_hs_delay_cnt         = 50;
    params->dbi.te_vs_width_cnt         = 277;
    params->dbi.te_vs_width_cnt_div     = LCM_DBI_TE_VS_WIDTH_CNT_DIV_16;
	#else
	params->dbi.te_mode                 = LCM_DBI_TE_MODE_DISABLED;
	#endif
}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);
    MDELAY(120);

    init_lcm_registers();
	lcm_update_black(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0x00);
}

static void lcm_suspend(void)
{	
	send_ctrl_cmd(0x2800);
	MDELAY(50);
	send_ctrl_cmd(0x1000);
	MDELAY(120);
	  
}


static void lcm_resume(void)
{
	#if 1
	send_ctrl_cmd(0x1100);
	MDELAY(120);	
	send_ctrl_cmd(0x2900);
	MDELAY(50);
	#else
	unsigned int lcd_id = 0;
	
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);
    MDELAY(100);  // 400

	set_lcm_register(0xF000,0x55);
	set_lcm_register(0xF001,0xAA);
	set_lcm_register(0xF002,0x52);
	set_lcm_register(0xF003,0x08);
	set_lcm_register(0xF004,0x01);
	UDELAY(10);
	
	lcd_id = get_lcd_id_r(LCM_ID_REG) & 0xFF;
	lcd_id = (lcd_id << 8) | (get_lcd_id_r(LCM_ID_REG + 1) & 0xFF);

	LCD_PRINT(LCM_NAME"lcm_resume: lcd_id = 0x%x\n", lcd_id);

    init_lcm_registers();
	#endif
}


static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

	set_lcm_register(0x2A00, HIGH_BYTE(x0));
	set_lcm_register(0x2A01, LOW_BYTE(x0));
	set_lcm_register(0x2A02, HIGH_BYTE(x1));
	set_lcm_register(0x2A03, LOW_BYTE(x1));
	set_lcm_register(0x2B00, HIGH_BYTE(y0));
	set_lcm_register(0x2B01, LOW_BYTE(y0));
	set_lcm_register(0x2B02, HIGH_BYTE(y1));  
	set_lcm_register(0x2B03, LOW_BYTE(y1));

	send_ctrl_cmd(0x2c00);

	//lcm_update_black(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0x00);
}

static unsigned int get_lcd_id_r(unsigned int addr)
{
	unsigned short id = 0;
	
	send_ctrl_cmd(addr);
    id = read_data_cmd();

	LCD_PRINT(LCM_NAME"get_lcd_id_r: id = 0x%x\n", id);
	return id;
}

static unsigned int get_lcd_id_n(unsigned int addr, unsigned char count)
{
	volatile unsigned int id = 0;
	unsigned char k = 0;
	

	send_ctrl_cmd(addr);

	while (k < count)
	{
		id = read_data_cmd();
		k++;
	}
	LCD_PRINT(LCM_NAME"get_lcd_id_n: id = 0x%x\n", id);
	
    return id;
}


static void lcm_setbacklight(unsigned int level)
{
	/*
	unsigned int default_level = 52;
	unsigned int mapped_level = 0;
	if(level > 255) 
		mapped_level = 255;

	if(level >= 102) 
		mapped_level = default_level+(level-102)*(255-default_level)/(255-102);
	else
		mapped_level = default_level-(102-level)*default_level/102;

	send_ctrl_cmd(0x51);
	send_data_cmd(mapped_level);	
	*/
}

static void lcm_setpwm(unsigned int divider)
{
	/*
    send_ctrl_cmd(0xC9);

    send_data_cmd(0x3E);
	send_data_cmd(0x00);
    send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x0F | (divider<<4));   
//	send_data_cmd(0x2F); 
	send_data_cmd(0x02);
	send_data_cmd(0x1E);   
	send_data_cmd(0x1E);
	send_data_cmd(0x00);
	*/
}

static unsigned int lcm_getpwm(unsigned int divider)
{
//	ref freq = 15MHz, B0h setting 0x80, so 80.6% * freq is pwm_clk;
//  pwm_clk / 255 / 2(lcm_setpwm() 6th params) = pwm_duration = 23706
	unsigned int pwm_clk = 23706 / (1<<divider);	
	return pwm_clk;
}
static unsigned int lcm_compare_id(void)
{	
	unsigned int lcd_id = 0;

	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);
    MDELAY(120);

	set_lcm_register(0xF000,0x55);
	set_lcm_register(0xF001,0xAA);
	set_lcm_register(0xF002,0x52);
	set_lcm_register(0xF003,0x08);
	set_lcm_register(0xF004,0x01);
	UDELAY(10);

	lcd_id = get_lcd_id_r(LCM_ID_REG) & 0xFF;
	lcd_id = (lcd_id << 8) | (get_lcd_id_r(LCM_ID_REG + 1) & 0xFF);

	LCD_PRINT(LCM_NAME"lcm_compare_id: lcd_id = 0x%x\n", lcd_id);

		
    return (LCM_ID == lcd_id)?1:0;
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER changxingda463_nt35510_lcm_drv = 
{
    .name			= "changxingda463_nt35510",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	
	.compare_id 	= lcm_compare_id,
	.set_backlight 	= lcm_setbacklight,
	.set_pwm		= lcm_setpwm,
	.get_pwm		= lcm_getpwm,
};

