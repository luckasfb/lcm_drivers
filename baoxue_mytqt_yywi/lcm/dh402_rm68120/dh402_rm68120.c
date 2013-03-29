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

#define LCM_ID        (0x8120) 	//  = [0xc501] << 8| [0xc500]
#define LCM_ID_REG	   (0xC500)		// 

#define LCM_NAME		"[DH402_RM68120]"

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

static __inline void set_lcm_register(unsigned int regIndex, unsigned int regData, unsigned int uiDelay)
{
	send_ctrl_cmd(regIndex);
    send_data_cmd(regData);

	if (uiDelay > 0)
		MDELAY(uiDelay);
}

static void  lcm_update_black(unsigned int x, unsigned int y,unsigned int width, unsigned int height, unsigned short data)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width;
    unsigned int y1 = y0 + height + 2;
    unsigned int k, i;

	set_lcm_register(0x2A00, HIGH_BYTE(x0),  0);
	set_lcm_register(0x2A01, LOW_BYTE(x0),  0);
	set_lcm_register(0x2A02, HIGH_BYTE(x1),  0);
	set_lcm_register(0x2A03, LOW_BYTE(x1),  0);
	set_lcm_register(0x2B00, HIGH_BYTE(y0),  0);
	set_lcm_register(0x2B01, LOW_BYTE(y0),  0);
	set_lcm_register(0x2B02, HIGH_BYTE(y1),  0);  
	set_lcm_register(0x2B03, LOW_BYTE(y1),  0);

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
	send_ctrl_cmd(0xF000);send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002);send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);send_data_cmd(0x08);
	send_ctrl_cmd(0xF004);send_data_cmd(0x01);

	send_ctrl_cmd(0xD100);send_data_cmd(0x00);
	send_ctrl_cmd(0xD101);send_data_cmd(0x00);
	send_ctrl_cmd(0xD102);send_data_cmd(0x11);
	send_ctrl_cmd(0xD103);send_data_cmd(0x2F);
	send_ctrl_cmd(0xD104);send_data_cmd(0x49);
	send_ctrl_cmd(0xD105);send_data_cmd(0x00);
	send_ctrl_cmd(0xD106);send_data_cmd(0x5E);
	send_ctrl_cmd(0xD107);send_data_cmd(0x80);
	send_ctrl_cmd(0xD108);send_data_cmd(0x9D);
	send_ctrl_cmd(0xD109);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD10a);send_data_cmd(0x54);
	send_ctrl_cmd(0xD10b);send_data_cmd(0xEE);
	send_ctrl_cmd(0xD10c);send_data_cmd(0x0B);
	send_ctrl_cmd(0xD10d);send_data_cmd(0x24);
	send_ctrl_cmd(0xD10e);send_data_cmd(0x3B);
	send_ctrl_cmd(0xD10f);send_data_cmd(0x55);
	send_ctrl_cmd(0xD110);send_data_cmd(0x4F);
	send_ctrl_cmd(0xD111);send_data_cmd(0x61);
	send_ctrl_cmd(0xD112);send_data_cmd(0x72);
	send_ctrl_cmd(0xD113);send_data_cmd(0x82);
	send_ctrl_cmd(0xD114);send_data_cmd(0x55);
	send_ctrl_cmd(0xD115);send_data_cmd(0x91);
	send_ctrl_cmd(0xD116);send_data_cmd(0x9F);
	send_ctrl_cmd(0xD117);send_data_cmd(0xAD);
	send_ctrl_cmd(0xD118);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD119);send_data_cmd(0x55);
	send_ctrl_cmd(0xD11a);send_data_cmd(0xC6);
	send_ctrl_cmd(0xD11b);send_data_cmd(0xC8);
	send_ctrl_cmd(0xD11c);send_data_cmd(0xD4);
	send_ctrl_cmd(0xD11d);send_data_cmd(0xE0);
	send_ctrl_cmd(0xD11e);send_data_cmd(0xA5);
	send_ctrl_cmd(0xD11f);send_data_cmd(0xEB);
	send_ctrl_cmd(0xD120);send_data_cmd(0xF6);
	send_ctrl_cmd(0xD121);send_data_cmd(0x01);
	send_ctrl_cmd(0xD122);send_data_cmd(0x0D);
	send_ctrl_cmd(0xD123);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD124);send_data_cmd(0x19);
	send_ctrl_cmd(0xD125);send_data_cmd(0x25);
	send_ctrl_cmd(0xD126);send_data_cmd(0x31);
	send_ctrl_cmd(0xD127);send_data_cmd(0x40);
	send_ctrl_cmd(0xD128);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD129);send_data_cmd(0x51);
	send_ctrl_cmd(0xD12a);send_data_cmd(0x66);
	send_ctrl_cmd(0xD12b);send_data_cmd(0x85);
	send_ctrl_cmd(0xD12c);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD12d);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD12e);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD12f);send_data_cmd(0x2E);
	send_ctrl_cmd(0xD130);send_data_cmd(0x63);
	send_ctrl_cmd(0xD131);send_data_cmd(0xA8);
	send_ctrl_cmd(0xD132);send_data_cmd(0x0F);
	send_ctrl_cmd(0xD133);send_data_cmd(0xED);
	send_ctrl_cmd(0xD134);send_data_cmd(0xFF);

	send_ctrl_cmd(0xD200);send_data_cmd(0x00);
	send_ctrl_cmd(0xD201);send_data_cmd(0x00);
	send_ctrl_cmd(0xD202);send_data_cmd(0x11);
	send_ctrl_cmd(0xD203);send_data_cmd(0x2F);
	send_ctrl_cmd(0xD204);send_data_cmd(0x49);
	send_ctrl_cmd(0xD205);send_data_cmd(0x00);
	send_ctrl_cmd(0xD206);send_data_cmd(0x5E);
	send_ctrl_cmd(0xD207);send_data_cmd(0x80);
	send_ctrl_cmd(0xD208);send_data_cmd(0x9D);
	send_ctrl_cmd(0xD209);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD20a);send_data_cmd(0x54);
	send_ctrl_cmd(0xD20b);send_data_cmd(0xEE);
	send_ctrl_cmd(0xD20c);send_data_cmd(0x0B);
	send_ctrl_cmd(0xD20d);send_data_cmd(0x24);
	send_ctrl_cmd(0xD20e);send_data_cmd(0x3B);
	send_ctrl_cmd(0xD20f);send_data_cmd(0x55);
	send_ctrl_cmd(0xD210);send_data_cmd(0x4F);
	send_ctrl_cmd(0xD211);send_data_cmd(0x61);
	send_ctrl_cmd(0xD212);send_data_cmd(0x72);
	send_ctrl_cmd(0xD213);send_data_cmd(0x82);
	send_ctrl_cmd(0xD214);send_data_cmd(0x55);
	send_ctrl_cmd(0xD215);send_data_cmd(0x91);
	send_ctrl_cmd(0xD216);send_data_cmd(0x9F);
	send_ctrl_cmd(0xD217);send_data_cmd(0xAD);
	send_ctrl_cmd(0xD218);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD219);send_data_cmd(0x55);
	send_ctrl_cmd(0xD21a);send_data_cmd(0xC6);
	send_ctrl_cmd(0xD21b);send_data_cmd(0xC8);
	send_ctrl_cmd(0xD21c);send_data_cmd(0xD4);
	send_ctrl_cmd(0xD21d);send_data_cmd(0xE0);
	send_ctrl_cmd(0xD21e);send_data_cmd(0xA5);
	send_ctrl_cmd(0xD21f);send_data_cmd(0xEB);
	send_ctrl_cmd(0xD220);send_data_cmd(0xF6);
	send_ctrl_cmd(0xD221);send_data_cmd(0x01);
	send_ctrl_cmd(0xD222);send_data_cmd(0x0D);
	send_ctrl_cmd(0xD223);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD224);send_data_cmd(0x19);
	send_ctrl_cmd(0xD225);send_data_cmd(0x25);
	send_ctrl_cmd(0xD226);send_data_cmd(0x31);
	send_ctrl_cmd(0xD227);send_data_cmd(0x40);
	send_ctrl_cmd(0xD228);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD229);send_data_cmd(0x51);
	send_ctrl_cmd(0xD22a);send_data_cmd(0x66);
	send_ctrl_cmd(0xD22b);send_data_cmd(0x85);
	send_ctrl_cmd(0xD22c);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD22d);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD22e);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD22f);send_data_cmd(0x2E);
	send_ctrl_cmd(0xD230);send_data_cmd(0x63);
	send_ctrl_cmd(0xD231);send_data_cmd(0xA8);
	send_ctrl_cmd(0xD232);send_data_cmd(0x0F);
	send_ctrl_cmd(0xD233);send_data_cmd(0xED);
	send_ctrl_cmd(0xD234);send_data_cmd(0xFF);

	send_ctrl_cmd(0xD300);send_data_cmd(0x00);
	send_ctrl_cmd(0xD301);send_data_cmd(0x00);
	send_ctrl_cmd(0xD302);send_data_cmd(0x11);
	send_ctrl_cmd(0xD303);send_data_cmd(0x2F);
	send_ctrl_cmd(0xD304);send_data_cmd(0x49);
	send_ctrl_cmd(0xD305);send_data_cmd(0x00);
	send_ctrl_cmd(0xD306);send_data_cmd(0x5E);
	send_ctrl_cmd(0xD307);send_data_cmd(0x80);
	send_ctrl_cmd(0xD308);send_data_cmd(0x9D);
	send_ctrl_cmd(0xD309);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD30a);send_data_cmd(0x54);
	send_ctrl_cmd(0xD30b);send_data_cmd(0xEE);
	send_ctrl_cmd(0xD30c);send_data_cmd(0x0B);
	send_ctrl_cmd(0xD30d);send_data_cmd(0x24);
	send_ctrl_cmd(0xD30e);send_data_cmd(0x3B);
	send_ctrl_cmd(0xD30f);send_data_cmd(0x55);
	send_ctrl_cmd(0xD310);send_data_cmd(0x4F);
	send_ctrl_cmd(0xD311);send_data_cmd(0x61);
	send_ctrl_cmd(0xD312);send_data_cmd(0x72);
	send_ctrl_cmd(0xD313);send_data_cmd(0x82);
	send_ctrl_cmd(0xD314);send_data_cmd(0x55);
	send_ctrl_cmd(0xD315);send_data_cmd(0x91);
	send_ctrl_cmd(0xD316);send_data_cmd(0x9F);
	send_ctrl_cmd(0xD317);send_data_cmd(0xAD);
	send_ctrl_cmd(0xD318);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD319);send_data_cmd(0x55);
	send_ctrl_cmd(0xD31a);send_data_cmd(0xC6);
	send_ctrl_cmd(0xD31b);send_data_cmd(0xC8);
	send_ctrl_cmd(0xD31c);send_data_cmd(0xD4);
	send_ctrl_cmd(0xD31d);send_data_cmd(0xE0);
	send_ctrl_cmd(0xD31e);send_data_cmd(0xA5);
	send_ctrl_cmd(0xD31f);send_data_cmd(0xEB);
	send_ctrl_cmd(0xD320);send_data_cmd(0xF6);
	send_ctrl_cmd(0xD321);send_data_cmd(0x01);
	send_ctrl_cmd(0xD322);send_data_cmd(0x0D);
	send_ctrl_cmd(0xD323);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD324);send_data_cmd(0x19);
	send_ctrl_cmd(0xD325);send_data_cmd(0x25);
	send_ctrl_cmd(0xD326);send_data_cmd(0x31);
	send_ctrl_cmd(0xD327);send_data_cmd(0x40);
	send_ctrl_cmd(0xD328);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD329);send_data_cmd(0x51);
	send_ctrl_cmd(0xD32a);send_data_cmd(0x66);
	send_ctrl_cmd(0xD32b);send_data_cmd(0x85);
	send_ctrl_cmd(0xD32c);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD32d);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD32e);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD32f);send_data_cmd(0x2E);
	send_ctrl_cmd(0xD330);send_data_cmd(0x63);
	send_ctrl_cmd(0xD331);send_data_cmd(0xA8);
	send_ctrl_cmd(0xD332);send_data_cmd(0x0F);
	send_ctrl_cmd(0xD333);send_data_cmd(0xED);
	send_ctrl_cmd(0xD334);send_data_cmd(0xFF);

	send_ctrl_cmd(0xD400);send_data_cmd(0x00);
	send_ctrl_cmd(0xD401);send_data_cmd(0x00);
	send_ctrl_cmd(0xD402);send_data_cmd(0x11);
	send_ctrl_cmd(0xD403);send_data_cmd(0x2F);
	send_ctrl_cmd(0xD404);send_data_cmd(0x49);
	send_ctrl_cmd(0xD405);send_data_cmd(0x00);
	send_ctrl_cmd(0xD406);send_data_cmd(0x5E);
	send_ctrl_cmd(0xD407);send_data_cmd(0x80);
	send_ctrl_cmd(0xD408);send_data_cmd(0x9D);
	send_ctrl_cmd(0xD409);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD40a);send_data_cmd(0x54);
	send_ctrl_cmd(0xD40b);send_data_cmd(0xEE);
	send_ctrl_cmd(0xD40c);send_data_cmd(0x0B);
	send_ctrl_cmd(0xD40d);send_data_cmd(0x24);
	send_ctrl_cmd(0xD40e);send_data_cmd(0x3B);
	send_ctrl_cmd(0xD40f);send_data_cmd(0x55);
	send_ctrl_cmd(0xD410);send_data_cmd(0x4F);
	send_ctrl_cmd(0xD411);send_data_cmd(0x61);
	send_ctrl_cmd(0xD412);send_data_cmd(0x72);
	send_ctrl_cmd(0xD413);send_data_cmd(0x82);
	send_ctrl_cmd(0xD414);send_data_cmd(0x55);
	send_ctrl_cmd(0xD415);send_data_cmd(0x91);
	send_ctrl_cmd(0xD416);send_data_cmd(0x9F);
	send_ctrl_cmd(0xD417);send_data_cmd(0xAD);
	send_ctrl_cmd(0xD418);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD419);send_data_cmd(0x55);
	send_ctrl_cmd(0xD41a);send_data_cmd(0xC6);
	send_ctrl_cmd(0xD41b);send_data_cmd(0xC8);
	send_ctrl_cmd(0xD41c);send_data_cmd(0xD4);
	send_ctrl_cmd(0xD41d);send_data_cmd(0xE0);
	send_ctrl_cmd(0xD41e);send_data_cmd(0xA5);
	send_ctrl_cmd(0xD41f);send_data_cmd(0xEB);
	send_ctrl_cmd(0xD420);send_data_cmd(0xF6);
	send_ctrl_cmd(0xD421);send_data_cmd(0x01);
	send_ctrl_cmd(0xD422);send_data_cmd(0x0D);
	send_ctrl_cmd(0xD423);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD424);send_data_cmd(0x19);
	send_ctrl_cmd(0xD425);send_data_cmd(0x25);
	send_ctrl_cmd(0xD426);send_data_cmd(0x31);
	send_ctrl_cmd(0xD427);send_data_cmd(0x40);
	send_ctrl_cmd(0xD428);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD429);send_data_cmd(0x51);
	send_ctrl_cmd(0xD42a);send_data_cmd(0x66);
	send_ctrl_cmd(0xD42b);send_data_cmd(0x85);
	send_ctrl_cmd(0xD42c);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD42d);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD42e);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD42f);send_data_cmd(0x2E);
	send_ctrl_cmd(0xD430);send_data_cmd(0x63);
	send_ctrl_cmd(0xD431);send_data_cmd(0xA8);
	send_ctrl_cmd(0xD432);send_data_cmd(0x0F);
	send_ctrl_cmd(0xD433);send_data_cmd(0xED);
	send_ctrl_cmd(0xD434);send_data_cmd(0xFF);

	send_ctrl_cmd(0xD500);send_data_cmd(0x00);
	send_ctrl_cmd(0xD501);send_data_cmd(0x00);
	send_ctrl_cmd(0xD502);send_data_cmd(0x11);
	send_ctrl_cmd(0xD503);send_data_cmd(0x2F);
	send_ctrl_cmd(0xD504);send_data_cmd(0x49);
	send_ctrl_cmd(0xD505);send_data_cmd(0x00);
	send_ctrl_cmd(0xD506);send_data_cmd(0x5E);
	send_ctrl_cmd(0xD507);send_data_cmd(0x80);
	send_ctrl_cmd(0xD508);send_data_cmd(0x9D);
	send_ctrl_cmd(0xD509);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD50a);send_data_cmd(0x54);
	send_ctrl_cmd(0xD50b);send_data_cmd(0xEE);
	send_ctrl_cmd(0xD50c);send_data_cmd(0x0B);
	send_ctrl_cmd(0xD50d);send_data_cmd(0x24);
	send_ctrl_cmd(0xD50e);send_data_cmd(0x3B);
	send_ctrl_cmd(0xD50f);send_data_cmd(0x55);
	send_ctrl_cmd(0xD510);send_data_cmd(0x4F);
	send_ctrl_cmd(0xD511);send_data_cmd(0x61);
	send_ctrl_cmd(0xD512);send_data_cmd(0x72);
	send_ctrl_cmd(0xD513);send_data_cmd(0x82);
	send_ctrl_cmd(0xD514);send_data_cmd(0x55);
	send_ctrl_cmd(0xD515);send_data_cmd(0x91);
	send_ctrl_cmd(0xD516);send_data_cmd(0x9F);
	send_ctrl_cmd(0xD517);send_data_cmd(0xAD);
	send_ctrl_cmd(0xD518);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD519);send_data_cmd(0x55);
	send_ctrl_cmd(0xD51a);send_data_cmd(0xC6);
	send_ctrl_cmd(0xD51b);send_data_cmd(0xC8);
	send_ctrl_cmd(0xD51c);send_data_cmd(0xD4);
	send_ctrl_cmd(0xD51d);send_data_cmd(0xE0);
	send_ctrl_cmd(0xD51e);send_data_cmd(0xA5);
	send_ctrl_cmd(0xD51f);send_data_cmd(0xEB);
	send_ctrl_cmd(0xD520);send_data_cmd(0xF6);
	send_ctrl_cmd(0xD521);send_data_cmd(0x01);
	send_ctrl_cmd(0xD522);send_data_cmd(0x0D);
	send_ctrl_cmd(0xD523);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD524);send_data_cmd(0x19);
	send_ctrl_cmd(0xD525);send_data_cmd(0x25);
	send_ctrl_cmd(0xD526);send_data_cmd(0x31);
	send_ctrl_cmd(0xD527);send_data_cmd(0x40);
	send_ctrl_cmd(0xD528);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD529);send_data_cmd(0x51);
	send_ctrl_cmd(0xD52a);send_data_cmd(0x66);
	send_ctrl_cmd(0xD52b);send_data_cmd(0x85);
	send_ctrl_cmd(0xD52c);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD52d);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD52e);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD52f);send_data_cmd(0x2E);
	send_ctrl_cmd(0xD530);send_data_cmd(0x63);
	send_ctrl_cmd(0xD531);send_data_cmd(0xA8);
	send_ctrl_cmd(0xD532);send_data_cmd(0x0F);
	send_ctrl_cmd(0xD533);send_data_cmd(0xED);
	send_ctrl_cmd(0xD534);send_data_cmd(0xFF);

	send_ctrl_cmd(0xD600);send_data_cmd(0x00);
	send_ctrl_cmd(0xD601);send_data_cmd(0x00);
	send_ctrl_cmd(0xD602);send_data_cmd(0x11);
	send_ctrl_cmd(0xD603);send_data_cmd(0x2F);
	send_ctrl_cmd(0xD604);send_data_cmd(0x49);
	send_ctrl_cmd(0xD605);send_data_cmd(0x00);
	send_ctrl_cmd(0xD606);send_data_cmd(0x5E);
	send_ctrl_cmd(0xD607);send_data_cmd(0x80);
	send_ctrl_cmd(0xD608);send_data_cmd(0x9D);
	send_ctrl_cmd(0xD609);send_data_cmd(0xCA);
	send_ctrl_cmd(0xD60A);send_data_cmd(0x54);
	send_ctrl_cmd(0xD60B);send_data_cmd(0xEE);
	send_ctrl_cmd(0xD60C);send_data_cmd(0x0B);
	send_ctrl_cmd(0xD60D);send_data_cmd(0x24);
	send_ctrl_cmd(0xD60E);send_data_cmd(0x3B);
	send_ctrl_cmd(0xD60F);send_data_cmd(0x55);
	send_ctrl_cmd(0xD610);send_data_cmd(0x4F);
	send_ctrl_cmd(0xD611);send_data_cmd(0x61);
	send_ctrl_cmd(0xD612);send_data_cmd(0x72);
	send_ctrl_cmd(0xD613);send_data_cmd(0x82);
	send_ctrl_cmd(0xD614);send_data_cmd(0x55);
	send_ctrl_cmd(0xD615);send_data_cmd(0x91);
	send_ctrl_cmd(0xD616);send_data_cmd(0x9F);
	send_ctrl_cmd(0xD617);send_data_cmd(0xAD);
	send_ctrl_cmd(0xD618);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD619);send_data_cmd(0x55);
	send_ctrl_cmd(0xD61a);send_data_cmd(0xC6);
	send_ctrl_cmd(0xD61b);send_data_cmd(0xC8);
	send_ctrl_cmd(0xD61c);send_data_cmd(0xD4);
	send_ctrl_cmd(0xD61d);send_data_cmd(0xE0);
	send_ctrl_cmd(0xD61e);send_data_cmd(0xA5);
	send_ctrl_cmd(0xD61f);send_data_cmd(0xEB);
	send_ctrl_cmd(0xD620);send_data_cmd(0xF6);
	send_ctrl_cmd(0xD621);send_data_cmd(0x01);
	send_ctrl_cmd(0xD622);send_data_cmd(0x0D);
	send_ctrl_cmd(0xD623);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD624);send_data_cmd(0x19);
	send_ctrl_cmd(0xD625);send_data_cmd(0x25);
	send_ctrl_cmd(0xD626);send_data_cmd(0x31);
	send_ctrl_cmd(0xD627);send_data_cmd(0x40);
	send_ctrl_cmd(0xD628);send_data_cmd(0xAA);
	send_ctrl_cmd(0xD629);send_data_cmd(0x51);
	send_ctrl_cmd(0xD62a);send_data_cmd(0x66);
	send_ctrl_cmd(0xD62b);send_data_cmd(0x85);
	send_ctrl_cmd(0xD62c);send_data_cmd(0xBA);
	send_ctrl_cmd(0xD62d);send_data_cmd(0xFE);
	send_ctrl_cmd(0xD62e);send_data_cmd(0xE5);
	send_ctrl_cmd(0xD62f);send_data_cmd(0x2E);
	send_ctrl_cmd(0xD630);send_data_cmd(0x63);
	send_ctrl_cmd(0xD631);send_data_cmd(0xA8);
	send_ctrl_cmd(0xD632);send_data_cmd(0x0F);
	send_ctrl_cmd(0xD633);send_data_cmd(0xED);
	send_ctrl_cmd(0xD634);send_data_cmd(0xFF);

	send_ctrl_cmd(0xB000);send_data_cmd(0x0C);
	send_ctrl_cmd(0xB001);send_data_cmd(0x0C);
	send_ctrl_cmd(0xB002);send_data_cmd(0x0C);
	
	send_ctrl_cmd(0xB100);send_data_cmd(0x0C);
	send_ctrl_cmd(0xB101);send_data_cmd(0x0C);
	send_ctrl_cmd(0xB102);send_data_cmd(0x0C);

	send_ctrl_cmd(0xB600);send_data_cmd(0x34);
	send_ctrl_cmd(0xB601);send_data_cmd(0x34);
	send_ctrl_cmd(0xB602);send_data_cmd(0x34);

	send_ctrl_cmd(0xB700);send_data_cmd(0x24);
	send_ctrl_cmd(0xB701);send_data_cmd(0x24);
	send_ctrl_cmd(0xB702);send_data_cmd(0x24);

	send_ctrl_cmd(0xB800);send_data_cmd(0x24);
	send_ctrl_cmd(0xB801);send_data_cmd(0x24);
	send_ctrl_cmd(0xB802);send_data_cmd(0x24);

	send_ctrl_cmd(0xB900);send_data_cmd(0x34);
	send_ctrl_cmd(0xB901);send_data_cmd(0x34);
	send_ctrl_cmd(0xB902);send_data_cmd(0x34);

	send_ctrl_cmd(0xBA00);send_data_cmd(0x24);
	send_ctrl_cmd(0xBA01);send_data_cmd(0x24);
	send_ctrl_cmd(0xBA02);send_data_cmd(0x24);

	send_ctrl_cmd(0xBC00);send_data_cmd(0x00);
	send_ctrl_cmd(0xBC01);send_data_cmd(0x80);
	send_ctrl_cmd(0xBC02);send_data_cmd(0x20);

	send_ctrl_cmd(0xBD00);send_data_cmd(0x00);
	send_ctrl_cmd(0xBD01);send_data_cmd(0x80);
	send_ctrl_cmd(0xBD02);send_data_cmd(0x20);

	send_ctrl_cmd(0xBE00);send_data_cmd(0x00);
	send_ctrl_cmd(0xBE01);send_data_cmd(0x60);

	send_ctrl_cmd(0xF000);send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002);send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);send_data_cmd(0x08);
	send_ctrl_cmd(0xF004);send_data_cmd(0x00);

	send_ctrl_cmd(0xB400);send_data_cmd(0x10);

	send_ctrl_cmd(0xB600);send_data_cmd(0x02);

	send_ctrl_cmd(0xB100);send_data_cmd(0xF8);

	MDELAY(5);

	send_ctrl_cmd(0xB500);send_data_cmd(0x6B);

	send_ctrl_cmd(0xB700);send_data_cmd(0x22);
	send_ctrl_cmd(0xB701);send_data_cmd(0x22);

	send_ctrl_cmd(0xC800);send_data_cmd(0x01);
	send_ctrl_cmd(0xC801);send_data_cmd(0x00);
	send_ctrl_cmd(0xC802);send_data_cmd(0x2C);
	send_ctrl_cmd(0xC803);send_data_cmd(0x13);
	send_ctrl_cmd(0xC804);send_data_cmd(0x2C);
	send_ctrl_cmd(0xC805);send_data_cmd(0x13);
	send_ctrl_cmd(0xC806);send_data_cmd(0x2C);
	send_ctrl_cmd(0xC807);send_data_cmd(0x13);
	send_ctrl_cmd(0xC808);send_data_cmd(0x2C);
	send_ctrl_cmd(0xC809);send_data_cmd(0x13);
	send_ctrl_cmd(0xC80a);send_data_cmd(0x3F);
	send_ctrl_cmd(0xC80b);send_data_cmd(0x3F);
	send_ctrl_cmd(0xC80c);send_data_cmd(0x3F);
	send_ctrl_cmd(0xC80d);send_data_cmd(0x3F);
	send_ctrl_cmd(0xC80e);send_data_cmd(0x3F);
	send_ctrl_cmd(0xC80f);send_data_cmd(0x3F);
	send_ctrl_cmd(0xC810);send_data_cmd(0x3F);
	send_ctrl_cmd(0xC811);send_data_cmd(0x3F);

	send_ctrl_cmd(0xB800);send_data_cmd(0x01);
	send_ctrl_cmd(0xB801);send_data_cmd(0x03);
	send_ctrl_cmd(0xB802);send_data_cmd(0x03);
	send_ctrl_cmd(0xB803);send_data_cmd(0x03);

	send_ctrl_cmd(0xBC00);send_data_cmd(0x05);
	send_ctrl_cmd(0xBC01);send_data_cmd(0x05);
	send_ctrl_cmd(0xBC02);send_data_cmd(0x05);

	send_ctrl_cmd(0xD000);send_data_cmd(0x01);

	send_ctrl_cmd(0xBA00);send_data_cmd(0x01);

	send_ctrl_cmd(0xBD00);send_data_cmd(0x01);
	send_ctrl_cmd(0xBD01);send_data_cmd(0x10);
	send_ctrl_cmd(0xBD02);send_data_cmd(0x07);
	send_ctrl_cmd(0xBD03);send_data_cmd(0x07);

	send_ctrl_cmd(0xBE00);send_data_cmd(0x01);
	send_ctrl_cmd(0xBE01);send_data_cmd(0x10);
	send_ctrl_cmd(0xBE02);send_data_cmd(0x07);
	send_ctrl_cmd(0xBE03);send_data_cmd(0x07);

	send_ctrl_cmd(0xBF00);send_data_cmd(0x01);
	send_ctrl_cmd(0xBF01);send_data_cmd(0x10);
	send_ctrl_cmd(0xBF02);send_data_cmd(0x07);
	send_ctrl_cmd(0xBF03);send_data_cmd(0x07);

	send_ctrl_cmd(0xF000);send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002);send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);send_data_cmd(0x08);
	send_ctrl_cmd(0xF004);send_data_cmd(0x02);

	send_ctrl_cmd(0xF600);send_data_cmd(0x60);

	send_ctrl_cmd(0xEB00);send_data_cmd(0x01);
	send_ctrl_cmd(0xEB01);send_data_cmd(0x26);
	send_ctrl_cmd(0xEB02);send_data_cmd(0x01);
	send_ctrl_cmd(0xEB03);send_data_cmd(0xC8);
	send_ctrl_cmd(0xEB04);send_data_cmd(0x89);

	#ifdef TE_CONTROL
	send_ctrl_cmd(0x3500);send_data_cmd(0x00);
	#endif

	send_ctrl_cmd(0x3600);send_data_cmd(0xC0);


	send_ctrl_cmd(0x2A00);send_data_cmd(0x00);
	send_ctrl_cmd(0x2A01);send_data_cmd(0x00);
	send_ctrl_cmd(0x2A02);send_data_cmd(HIGH_BYTE(FRAME_WIDTH));
	send_ctrl_cmd(0x2A03);send_data_cmd(LOW_BYTE(FRAME_WIDTH));

	send_ctrl_cmd(0x2B00);send_data_cmd(0x00);
	send_ctrl_cmd(0x2B01);send_data_cmd(0x00);
	send_ctrl_cmd(0x2B02);send_data_cmd(HIGH_BYTE(FRAME_HEIGHT));
	send_ctrl_cmd(0x2B03);send_data_cmd(LOW_BYTE(FRAME_HEIGHT));

	send_ctrl_cmd(0x3A00);send_data_cmd(0x55);

	send_ctrl_cmd(0x1100);
	MDELAY(120);
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

	send_ctrl_cmd(0xF000);send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002);send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);send_data_cmd(0x08);
	send_ctrl_cmd(0xF004);send_data_cmd(0x01);
	UDELAY(10);

	lcd_id = get_lcd_id_r(LCM_ID_REG+1) & 0xFF;
	lcd_id = (lcd_id << 8) | (get_lcd_id_r(LCM_ID_REG) & 0xFF);

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

	set_lcm_register(0x2A00, HIGH_BYTE(x0),  0);
	set_lcm_register(0x2A01, LOW_BYTE(x0),  0);
	set_lcm_register(0x2A02, HIGH_BYTE(x1),  0);
	set_lcm_register(0x2A03, LOW_BYTE(x1),  0);
	set_lcm_register(0x2B00, HIGH_BYTE(y0),  0);
	set_lcm_register(0x2B01, LOW_BYTE(y0),  0);
	set_lcm_register(0x2B02, HIGH_BYTE(y1),  0);  
	set_lcm_register(0x2B03, LOW_BYTE(y1),  0);

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

	send_ctrl_cmd(0xF000);send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002);send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);send_data_cmd(0x08);
	send_ctrl_cmd(0xF004);send_data_cmd(0x01);
	UDELAY(10);

	lcd_id = get_lcd_id_r(LCM_ID_REG+1) & 0xFF;
	lcd_id = (lcd_id << 8) | (get_lcd_id_r(LCM_ID_REG) & 0xFF);

	LCD_PRINT(LCM_NAME"lcm_compare_id: lcd_id = 0x%x\n", lcd_id);

		
    return (LCM_ID == lcd_id)?1:0;
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER dh402_rm68120_lcm_drv = 
{
    .name			= "dh402_rm68120",
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

