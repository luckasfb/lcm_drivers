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

#define GET_LSA0_BIT mt_get_gpio_in(LSA0_GPIO_PIN)

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
	send_ctrl_cmd(0xBE01);send_data_cmd(0x5d);//===0x65

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
	send_ctrl_cmd(0xB900);send_data_cmd(0x34);///===34
	send_ctrl_cmd(0xB901);send_data_cmd(0x34);
	send_ctrl_cmd(0xB902);send_data_cmd(0x34);
	//VCL POWERCONTROL
	send_ctrl_cmd(0xB800);send_data_cmd(0x24);///===
	send_ctrl_cmd(0xB801);send_data_cmd(0x24);
	send_ctrl_cmd(0xB802);send_data_cmd(0x24);
	
	send_ctrl_cmd(0xB700);send_data_cmd(0x34);///===
	send_ctrl_cmd(0xB701);send_data_cmd(0x34);
	send_ctrl_cmd(0xB702);send_data_cmd(0x34);
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
	send_ctrl_cmd(0xBC00);send_data_cmd(0x00);//===00
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
	MDELAY(10);

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
    lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);                   // GPIO in
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
    params->dpi.mipi_pll_clk_div1 = 38;//40?
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

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_8MA;
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
	MDELAY(200);
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
