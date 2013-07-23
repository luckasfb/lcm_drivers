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

#ifndef GN_MTK_BSP_LCD_SW_SPI
#define GN_MTK_BSP_LCD_SW_SPI
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

static void config_gpio(void);

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};
#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)      (lcm_util.set_gpio_out((n), (v)))
#define UDELAY(n)               (lcm_util.udelay(n))
#define MDELAY(n)               (lcm_util.mdelay(n))



#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

#define SET_LSCE_LOW             SET_GPIO_OUT(LSCE_GPIO_PIN, 0)
#define SET_LSCE_HIGH            SET_GPIO_OUT(LSCE_GPIO_PIN, 1)
#define SET_LSCK_LOW             SET_GPIO_OUT(LSCK_GPIO_PIN, 0)
#define SET_LSCK_HIGH            SET_GPIO_OUT(LSCK_GPIO_PIN, 1)
#define SET_LSDA_LOW             SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH            SET_GPIO_OUT(LSDA_GPIO_PIN, 1)

#if defined(GN_MTK_BSP_LCD_SW_SPI)

static void   LCDSPI_InitCMD(int data)   
{
	unsigned short i,MB=0x20;
		
	SET_LSCE_HIGH;
	
	SET_LSCK_HIGH;
	SET_LSDA_LOW;
	UDELAY(100);
	SET_LSCE_LOW;
	UDELAY(100);

	for(i=0;i<8;i++)
	{
		
		
	   	if(MB&0x80)
			SET_LSDA_HIGH;    
	   	else
			SET_LSDA_LOW;
	  
		//UDELAY(100);
		SET_LSCK_LOW;

		UDELAY(100);
		SET_LSCK_HIGH;	
		UDELAY(100);
	   	MB<<=1;
	  }

    	MB=data;
	
	for(i=0;i<8;i++)
	  {

	   	if(MB&0x8000)
			SET_LSDA_HIGH;    
	   	else
			SET_LSDA_LOW;
	  
		//UDELAY(100);
		SET_LSCK_LOW;
		UDELAY(100);	
		SET_LSCK_HIGH;	  
		UDELAY(100);
	   	MB<<=1;
	  }
	SET_LSCE_HIGH;
	SET_LSCK_HIGH;
	UDELAY(100);

//the 2nd byte

	SET_LSCE_LOW;
	UDELAY(100);
	
	MB=0x00;
		
	for(i=0;i<8;i++)
	{

		if(MB&0x80)
			SET_LSDA_HIGH;    
	   	else
			SET_LSDA_LOW;
	  
		//UDELAY(100);
		SET_LSCK_LOW;
		UDELAY(100);
		SET_LSCK_HIGH;	  
		UDELAY(100);
	   	MB<<=1;
	  }

    	MB=data<<8;
	
	for(i=0;i<8;i++)
	{

		if(MB&0x8000)
			SET_LSDA_HIGH;    
	   	else
			SET_LSDA_LOW;
	  
		//UDELAY(100);
		SET_LSCK_LOW;
		UDELAY(100);
		SET_LSCK_HIGH;	  
		UDELAY(100);
	   	MB<<=1;
	 }

	SET_LSCE_HIGH;
	SET_LSCK_HIGH;
	UDELAY(100);
}


static void LCDSPI_InitDAT(int data)
{

	unsigned short i,MB=0x40;
	
	SET_LSCE_HIGH;
	SET_LSCK_HIGH;
	SET_LSDA_LOW;
	UDELAY(100);
	SET_LSCE_LOW;
	UDELAY(100);

	for(i=0;i<8;i++)
	{

	   	if(MB&0x80)
			SET_LSDA_HIGH;    
	  	else
			SET_LSDA_LOW;
	  
		//UDELAY(100);
		SET_LSCK_LOW;
		UDELAY(100);	
		SET_LSCK_HIGH;
		UDELAY(100);	
	    	MB<<=1;
	  }

	MB=data;
	
	for(i=0;i<8;i++)
	{

	   	if(MB&0x80)
			SET_LSDA_HIGH;    
	   	else
			SET_LSDA_LOW;
	  
		//UDELAY(100);
		SET_LSCK_LOW;
		UDELAY(100);	
		SET_LSCK_HIGH;	  
		UDELAY(100);
	   	MB<<=1;
	 }
	
	SET_LSCE_HIGH;
	SET_LSCK_HIGH;
	UDELAY(100);
}

#else

static __inline void LCDSPI_InitCMD(unsigned int cmd)
{
	lcm_util.send_data(((cmd >> 8)& 0xFF)|0x2000);
	lcm_util.send_data((cmd & 0xFF)|0x0000);
}
static __inline void LCDSPI_InitDAT(unsigned char reg_Data)
{
	lcm_util.send_data(0x4000|(reg_Data & 0xFF));
}

#endif

static void init_lcm_registers(void)
{
    	LCM_DEBUG("wanght==>[LCM************]: init_lcm_registers. \n");

	     //ENABLE PAGE 1
        LCDSPI_InitCMD(0xF000);LCDSPI_InitDAT(0x55);
        LCDSPI_InitCMD(0xF001);LCDSPI_InitDAT(0xAA);
        LCDSPI_InitCMD(0xF002);LCDSPI_InitDAT(0x52);
        LCDSPI_InitCMD(0xF003);LCDSPI_InitDAT(0x08);
        LCDSPI_InitCMD(0xF004);LCDSPI_InitDAT(0x01);
        
        //AVDD
        LCDSPI_InitCMD(0xB000);LCDSPI_InitDAT(0x0A);
        LCDSPI_InitCMD(0xB001);LCDSPI_InitDAT(0x0A);
        LCDSPI_InitCMD(0xB002);LCDSPI_InitDAT(0x0A);

        LCDSPI_InitCMD(0xB600);LCDSPI_InitDAT(0x43);
        LCDSPI_InitCMD(0xB601);LCDSPI_InitDAT(0x43);
        LCDSPI_InitCMD(0xB602);LCDSPI_InitDAT(0x43);

        //AVEE
        LCDSPI_InitCMD(0xB100);LCDSPI_InitDAT(0x0A);
        LCDSPI_InitCMD(0xB101);LCDSPI_InitDAT(0x0A);
        LCDSPI_InitCMD(0xB102);LCDSPI_InitDAT(0x0A);

        LCDSPI_InitCMD(0xB700);LCDSPI_InitDAT(0x33);
        LCDSPI_InitCMD(0xB701);LCDSPI_InitDAT(0x33);
        LCDSPI_InitCMD(0xB702);LCDSPI_InitDAT(0x33);

        //VCL
        LCDSPI_InitCMD(0xB200);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xB201);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xB202);LCDSPI_InitDAT(0x01);

        LCDSPI_InitCMD(0xB800);LCDSPI_InitDAT(0x22);
        LCDSPI_InitCMD(0xB801);LCDSPI_InitDAT(0x22);
        LCDSPI_InitCMD(0xB802);LCDSPI_InitDAT(0x22);

        //VGH
        LCDSPI_InitCMD(0xB300);LCDSPI_InitDAT(0x08);
        LCDSPI_InitCMD(0xB301);LCDSPI_InitDAT(0x08);
        LCDSPI_InitCMD(0xB302);LCDSPI_InitDAT(0x08);

        LCDSPI_InitCMD(0xB900);LCDSPI_InitDAT(0x22);
        LCDSPI_InitCMD(0xB901);LCDSPI_InitDAT(0x22);
        LCDSPI_InitCMD(0xB902);LCDSPI_InitDAT(0x22);

        LCDSPI_InitCMD(0xBF00);LCDSPI_InitDAT(0x00);

        //VGL
        LCDSPI_InitCMD(0xBA00);LCDSPI_InitDAT(0x22);
        LCDSPI_InitCMD(0xBA01);LCDSPI_InitDAT(0x22);
        LCDSPI_InitCMD(0xBA02);LCDSPI_InitDAT(0x22);
        LCDSPI_InitCMD(0xC200);LCDSPI_InitDAT(0x02);

        //VGMP
        LCDSPI_InitCMD(0xBc00);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xBc01);LCDSPI_InitDAT(0xa0);
        LCDSPI_InitCMD(0xBc02);LCDSPI_InitDAT(0x00);

        //VGMN
        LCDSPI_InitCMD(0xBD00);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xBD01);LCDSPI_InitDAT(0xa0);
        LCDSPI_InitCMD(0xBD02);LCDSPI_InitDAT(0x00);
        
        //VGMN
        LCDSPI_InitCMD(0xBE00);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xBE01);LCDSPI_InitDAT(0x7C);
        
        LCDSPI_InitCMD(0xD100);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD101);LCDSPI_InitDAT(0x5D);
        LCDSPI_InitCMD(0xD102);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD103);LCDSPI_InitDAT(0x79);
        LCDSPI_InitCMD(0xD104);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD105);LCDSPI_InitDAT(0xA0);
        LCDSPI_InitCMD(0xD106);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD107);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD108);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD109);LCDSPI_InitDAT(0xD1);
        LCDSPI_InitCMD(0xD10A);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD10B);LCDSPI_InitDAT(0xF1);
        LCDSPI_InitCMD(0xD10C);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD10D);LCDSPI_InitDAT(0x13);
        LCDSPI_InitCMD(0xD10E);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD10F);LCDSPI_InitDAT(0x41);
        LCDSPI_InitCMD(0xD110);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD111);LCDSPI_InitDAT(0x64);
        LCDSPI_InitCMD(0xD112);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD113);LCDSPI_InitDAT(0x9E);
        LCDSPI_InitCMD(0xD114);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD115);LCDSPI_InitDAT(0xCC);
        LCDSPI_InitCMD(0xD116);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD117);LCDSPI_InitDAT(0x11);
        LCDSPI_InitCMD(0xD118);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD119);LCDSPI_InitDAT(0x4E);
        LCDSPI_InitCMD(0xD11A);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD11B);LCDSPI_InitDAT(0x4F);
        LCDSPI_InitCMD(0xD11C);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD11D);LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xD11E);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD11F);LCDSPI_InitDAT(0xD4);
        LCDSPI_InitCMD(0xD120);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD121);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD122);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD123);LCDSPI_InitDAT(0x37);
        LCDSPI_InitCMD(0xD124);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD125);LCDSPI_InitDAT(0x57);
        LCDSPI_InitCMD(0xD126);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD127);LCDSPI_InitDAT(0x81);
        LCDSPI_InitCMD(0xD128);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD129);LCDSPI_InitDAT(0x9D);
        LCDSPI_InitCMD(0xD12A);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD12B);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD12C);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD12D);LCDSPI_InitDAT(0xDE);
        LCDSPI_InitCMD(0xD12E);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD12F);LCDSPI_InitDAT(0xE7);
        LCDSPI_InitCMD(0xD130);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD131);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD132);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD133);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD200);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD201);LCDSPI_InitDAT(0x5D);
        LCDSPI_InitCMD(0xD202);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD203);LCDSPI_InitDAT(0x79);
        LCDSPI_InitCMD(0xD204);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD205);LCDSPI_InitDAT(0xA0);
        LCDSPI_InitCMD(0xD206);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD207);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD208);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD209);LCDSPI_InitDAT(0xD1);
        LCDSPI_InitCMD(0xD20A);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD20B);LCDSPI_InitDAT(0xF1);
        LCDSPI_InitCMD(0xD20C);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD20D);LCDSPI_InitDAT(0x13);
        LCDSPI_InitCMD(0xD20E);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD20F);LCDSPI_InitDAT(0x41);
        LCDSPI_InitCMD(0xD210);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD211);LCDSPI_InitDAT(0x64);
        LCDSPI_InitCMD(0xD212);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD213);LCDSPI_InitDAT(0x9E);
        LCDSPI_InitCMD(0xD214);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD215);LCDSPI_InitDAT(0xCC);
        LCDSPI_InitCMD(0xD216);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD217);LCDSPI_InitDAT(0x11);
        LCDSPI_InitCMD(0xD218);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD219);LCDSPI_InitDAT(0x4E);
        LCDSPI_InitCMD(0xD21A);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD21B);LCDSPI_InitDAT(0x4F);
        LCDSPI_InitCMD(0xD21C);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD21D);LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xD21E);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD21F);LCDSPI_InitDAT(0xD4);
        LCDSPI_InitCMD(0xD220);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD221);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD222);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD223);LCDSPI_InitDAT(0x37);
        LCDSPI_InitCMD(0xD224);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD225);LCDSPI_InitDAT(0x57);
        LCDSPI_InitCMD(0xD226);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD227);LCDSPI_InitDAT(0x81);
        LCDSPI_InitCMD(0xD228);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD229);LCDSPI_InitDAT(0x9D);
        LCDSPI_InitCMD(0xD22A);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD22B);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD22C);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD22D);LCDSPI_InitDAT(0xDE);
        LCDSPI_InitCMD(0xD22E);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD22F);LCDSPI_InitDAT(0xE7);
        LCDSPI_InitCMD(0xD230);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD231);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD232);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD233);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD300);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD301);LCDSPI_InitDAT(0x5D);
        LCDSPI_InitCMD(0xD302);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD303);LCDSPI_InitDAT(0x79);
        LCDSPI_InitCMD(0xD304);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD305);LCDSPI_InitDAT(0xA0);
        LCDSPI_InitCMD(0xD306);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD307);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD308);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD309);LCDSPI_InitDAT(0xD1);
        LCDSPI_InitCMD(0xD30A);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD30B);LCDSPI_InitDAT(0xF1);
        LCDSPI_InitCMD(0xD30C);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD30D);LCDSPI_InitDAT(0x13);
        LCDSPI_InitCMD(0xD30E);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD30F);LCDSPI_InitDAT(0x41);
        LCDSPI_InitCMD(0xD310);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD311);LCDSPI_InitDAT(0x64);
        LCDSPI_InitCMD(0xD312);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD313);LCDSPI_InitDAT(0x9E);
        LCDSPI_InitCMD(0xD314);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD315);LCDSPI_InitDAT(0xCC);
        LCDSPI_InitCMD(0xD316);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD317);LCDSPI_InitDAT(0x11);
        LCDSPI_InitCMD(0xD318);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD319);LCDSPI_InitDAT(0x4E);
        LCDSPI_InitCMD(0xD31A);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD31B);LCDSPI_InitDAT(0x4F);
        LCDSPI_InitCMD(0xD31C);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD31D);LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xD31E);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD31F);LCDSPI_InitDAT(0xD4);
        LCDSPI_InitCMD(0xD320);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD321);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD322);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD323);LCDSPI_InitDAT(0x37);
        LCDSPI_InitCMD(0xD324);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD325);LCDSPI_InitDAT(0x57);
        LCDSPI_InitCMD(0xD326);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD327);LCDSPI_InitDAT(0x81);
        LCDSPI_InitCMD(0xD328);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD329);LCDSPI_InitDAT(0x9D);
        LCDSPI_InitCMD(0xD32A);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD32B);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD32C);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD32D);LCDSPI_InitDAT(0xDE);
        LCDSPI_InitCMD(0xD32E);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD32F);LCDSPI_InitDAT(0xE7);
        LCDSPI_InitCMD(0xD330);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD331);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD332);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD333);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD400);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD401);LCDSPI_InitDAT(0x5D);
        LCDSPI_InitCMD(0xD402);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD403);LCDSPI_InitDAT(0x79);
        LCDSPI_InitCMD(0xD404);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD405);LCDSPI_InitDAT(0xA0);
        LCDSPI_InitCMD(0xD406);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD407);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD408);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD409);LCDSPI_InitDAT(0xD1);
        LCDSPI_InitCMD(0xD40A);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD40B);LCDSPI_InitDAT(0xF1);
        LCDSPI_InitCMD(0xD40C);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD40D);LCDSPI_InitDAT(0x13);
        LCDSPI_InitCMD(0xD40E);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD40F);LCDSPI_InitDAT(0x41);
        LCDSPI_InitCMD(0xD410);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD411);LCDSPI_InitDAT(0x64);
        LCDSPI_InitCMD(0xD412);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD413);LCDSPI_InitDAT(0x9E);
        LCDSPI_InitCMD(0xD414);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD415);LCDSPI_InitDAT(0xCC);
        LCDSPI_InitCMD(0xD416);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD417);LCDSPI_InitDAT(0x11);
        LCDSPI_InitCMD(0xD418);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD419);LCDSPI_InitDAT(0x4E);
        LCDSPI_InitCMD(0xD41A);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD41B);LCDSPI_InitDAT(0x4F);
        LCDSPI_InitCMD(0xD41C);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD41D);LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xD41E);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD41F);LCDSPI_InitDAT(0xD4);
        LCDSPI_InitCMD(0xD420);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD421);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD422);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD423);LCDSPI_InitDAT(0x37);
        LCDSPI_InitCMD(0xD424);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD425);LCDSPI_InitDAT(0x57);
        LCDSPI_InitCMD(0xD426);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD427);LCDSPI_InitDAT(0x81);
        LCDSPI_InitCMD(0xD428);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD429);LCDSPI_InitDAT(0x9D);
        LCDSPI_InitCMD(0xD42A);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD42B);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD42C);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD42D);LCDSPI_InitDAT(0xDE);
        LCDSPI_InitCMD(0xD42E);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD42F);LCDSPI_InitDAT(0xE7);
        LCDSPI_InitCMD(0xD430);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD431);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD432);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD433);LCDSPI_InitDAT(0xEA);
        
        LCDSPI_InitCMD(0xD500);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD501);LCDSPI_InitDAT(0x5D);
        LCDSPI_InitCMD(0xD502);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD503);LCDSPI_InitDAT(0x79);
        LCDSPI_InitCMD(0xD504);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD505);LCDSPI_InitDAT(0xA0);
        LCDSPI_InitCMD(0xD506);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD507);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD508);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD509);LCDSPI_InitDAT(0xD1);
        LCDSPI_InitCMD(0xD50A);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD50B);LCDSPI_InitDAT(0xF1);
        LCDSPI_InitCMD(0xD50C);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD50D);LCDSPI_InitDAT(0x13);
        LCDSPI_InitCMD(0xD50E);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD50F);LCDSPI_InitDAT(0x41);
        LCDSPI_InitCMD(0xD510);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD511);LCDSPI_InitDAT(0x64);
        LCDSPI_InitCMD(0xD512);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD513);LCDSPI_InitDAT(0x9E);
        LCDSPI_InitCMD(0xD514);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD515);LCDSPI_InitDAT(0xCC);
        LCDSPI_InitCMD(0xD516);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD517);LCDSPI_InitDAT(0x11);
        LCDSPI_InitCMD(0xD518);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD519);LCDSPI_InitDAT(0x4E);
        LCDSPI_InitCMD(0xD51A);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD51B);LCDSPI_InitDAT(0x4F);
        LCDSPI_InitCMD(0xD51C);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD51D);LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xD51E);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD51F);LCDSPI_InitDAT(0xD4);
        LCDSPI_InitCMD(0xD520);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD521);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD522);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD523);LCDSPI_InitDAT(0x37);
        LCDSPI_InitCMD(0xD524);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD525);LCDSPI_InitDAT(0x57);
        LCDSPI_InitCMD(0xD526);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD527);LCDSPI_InitDAT(0x81);
        LCDSPI_InitCMD(0xD528);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD529);LCDSPI_InitDAT(0x9D);
        LCDSPI_InitCMD(0xD52A);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD52B);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD52C);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD52D);LCDSPI_InitDAT(0xDE);
        LCDSPI_InitCMD(0xD52E);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD52F);LCDSPI_InitDAT(0xE7);
        LCDSPI_InitCMD(0xD530);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD531);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD532);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD533);LCDSPI_InitDAT(0xEA);
        
        LCDSPI_InitCMD(0xD600);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD601);LCDSPI_InitDAT(0x5D);
        LCDSPI_InitCMD(0xD602);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD603);LCDSPI_InitDAT(0x79);
        LCDSPI_InitCMD(0xD604);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD605);LCDSPI_InitDAT(0xA0);
        LCDSPI_InitCMD(0xD606);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD607);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD608);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD609);LCDSPI_InitDAT(0xD1);
        LCDSPI_InitCMD(0xD60A);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD60B);LCDSPI_InitDAT(0xF1);
        LCDSPI_InitCMD(0xD60C);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD60D);LCDSPI_InitDAT(0x13);
        LCDSPI_InitCMD(0xD60E);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD60F);LCDSPI_InitDAT(0x41);
        LCDSPI_InitCMD(0xD610);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD611);LCDSPI_InitDAT(0x64);
        LCDSPI_InitCMD(0xD612);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD613);LCDSPI_InitDAT(0x9E);
        LCDSPI_InitCMD(0xD614);LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xD615);LCDSPI_InitDAT(0xCC);
        LCDSPI_InitCMD(0xD616);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD617);LCDSPI_InitDAT(0x11);
        LCDSPI_InitCMD(0xD618);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD619);LCDSPI_InitDAT(0x4E);
        LCDSPI_InitCMD(0xD61A);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD61B);LCDSPI_InitDAT(0x4F);
        LCDSPI_InitCMD(0xD61C);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD61D);LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xD61E);LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xD61F);LCDSPI_InitDAT(0xD4);
        LCDSPI_InitCMD(0xD620);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD621);LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xD622);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD623);LCDSPI_InitDAT(0x37);
        LCDSPI_InitCMD(0xD624);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD625);LCDSPI_InitDAT(0x57);
        LCDSPI_InitCMD(0xD626);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD627);LCDSPI_InitDAT(0x81);
        LCDSPI_InitCMD(0xD628);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD629);LCDSPI_InitDAT(0x9D);
        LCDSPI_InitCMD(0xD62A);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD62B);LCDSPI_InitDAT(0xBA);
        LCDSPI_InitCMD(0xD62C);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD62D);LCDSPI_InitDAT(0xDE);
        LCDSPI_InitCMD(0xD62E);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD62F);LCDSPI_InitDAT(0xE7);
        LCDSPI_InitCMD(0xD630);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD631);LCDSPI_InitDAT(0xEA);
        LCDSPI_InitCMD(0xD632);LCDSPI_InitDAT(0x03);
        LCDSPI_InitCMD(0xD633);LCDSPI_InitDAT(0xEA);
        
        
       //ENABLE PAGE 0
        LCDSPI_InitCMD(0xF000);LCDSPI_InitDAT(0x55);
        LCDSPI_InitCMD(0xF001);LCDSPI_InitDAT(0xAA);
        LCDSPI_InitCMD(0xF002);LCDSPI_InitDAT(0x52);
        LCDSPI_InitCMD(0xF003);LCDSPI_InitDAT(0x08);
        LCDSPI_InitCMD(0xF004);LCDSPI_InitDAT(0x00);
        
        LCDSPI_InitCMD(0xB000); LCDSPI_InitDAT(0x0C); 
        LCDSPI_InitCMD(0xB001); LCDSPI_InitDAT(0x0A);
        LCDSPI_InitCMD(0xB002); LCDSPI_InitDAT(0x0A);
        LCDSPI_InitCMD(0xB003); LCDSPI_InitDAT(0x10);
        LCDSPI_InitCMD(0xB004); LCDSPI_InitDAT(0x10);

        LCDSPI_InitCMD(0xB100); LCDSPI_InitDAT(0xC8); // RGB Interface Signals Control
        LCDSPI_InitCMD(0xB101); LCDSPI_InitDAT(0x00);
  
        //GEQ
        LCDSPI_InitCMD(0xB700); LCDSPI_InitDAT(0x71); // RGB Interface Signals Control
        LCDSPI_InitCMD(0xB701); LCDSPI_InitDAT(0x71); 
        LCDSPI_InitCMD(0xB900); LCDSPI_InitDAT(0x00); // RGB Interface Signals Control
        LCDSPI_InitCMD(0xB901); LCDSPI_InitDAT(0x40);
        LCDSPI_InitCMD(0xBA00); LCDSPI_InitDAT(0x05);

        //Display timing Control 
        LCDSPI_InitCMD(0xBD00); LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xBD01); LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xBD02); LCDSPI_InitDAT(0x14);
        LCDSPI_InitCMD(0xB003); LCDSPI_InitDAT(0x14);
        LCDSPI_InitCMD(0xB004); LCDSPI_InitDAT(0x00);

        LCDSPI_InitCMD(0xBE00); LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xBE01); LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xBE02); LCDSPI_InitDAT(0x14);
        LCDSPI_InitCMD(0xBE03); LCDSPI_InitDAT(0x14);
        LCDSPI_InitCMD(0xBE04); LCDSPI_InitDAT(0x00);

        LCDSPI_InitCMD(0xBF00); LCDSPI_InitDAT(0x01);
        LCDSPI_InitCMD(0xBF01); LCDSPI_InitDAT(0x8C);
        LCDSPI_InitCMD(0xBF02); LCDSPI_InitDAT(0x14);
        LCDSPI_InitCMD(0xBF03); LCDSPI_InitDAT(0x14);
        LCDSPI_InitCMD(0xBF04); LCDSPI_InitDAT(0x00);
  
        //GOT timing setting 
        LCDSPI_InitCMD(0xC900); LCDSPI_InitDAT(0xC2);
        LCDSPI_InitCMD(0xC901); LCDSPI_InitDAT(0x02);
        LCDSPI_InitCMD(0xC902); LCDSPI_InitDAT(0x50);
        LCDSPI_InitCMD(0xC903); LCDSPI_InitDAT(0x50);
        LCDSPI_InitCMD(0xC904); LCDSPI_InitDAT(0x50);

        LCDSPI_InitCMD(0x3A00);LCDSPI_InitDAT(0x77);

        //*****CABC*******
        LCDSPI_InitCMD(0xE000); LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0xE100); LCDSPI_InitDAT(0x41);
        LCDSPI_InitCMD(0x5100); LCDSPI_InitDAT(0x00);
        LCDSPI_InitCMD(0x5300); LCDSPI_InitDAT(0x27);
        //*************************

        //SLEEP OUT 
        LCDSPI_InitCMD(0x1100);
        LCDSPI_InitDAT(0x00);
        MDELAY(180);

        //DISPLY ON
        LCDSPI_InitCMD(0x2900);
        MDELAY(100);
}

static void config_gpio(void)
{
	LCM_DEBUG("wanght==>[LCM************]: config_gpio. \n");
	
#if defined(GN_MTK_BSP_LCD_SW_SPI)
	//lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_MODE_00);
	lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_MODE_00);
	lcm_util.set_gpio_mode(LSCK_GPIO_PIN, GPIO_MODE_00);
	lcm_util.set_gpio_mode(LSDA_GPIO_PIN, GPIO_MODE_00);
#else	
	//lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_MODE_01);
	lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_MODE_01);
	lcm_util.set_gpio_mode(LSCK_GPIO_PIN, GPIO_MODE_01);
	lcm_util.set_gpio_mode(LSDA_GPIO_PIN, GPIO_MODE_01);
#endif
	//lcm_util.set_gpio_dir(LSA0_GPIO_PIN, GPIO_DIR_OUT);
	lcm_util.set_gpio_dir(LSCE_GPIO_PIN, GPIO_DIR_OUT);
	lcm_util.set_gpio_dir(LSCK_GPIO_PIN, GPIO_DIR_OUT);
	lcm_util.set_gpio_dir(LSDA_GPIO_PIN, GPIO_DIR_OUT);

	//lcm_util.set_gpio_pull_enable(LSA0_GPIO_PIN, GPIO_PULL_DISABLE);
	lcm_util.set_gpio_pull_enable(LSCE_GPIO_PIN, GPIO_PULL_DISABLE);
	lcm_util.set_gpio_pull_enable(LSCK_GPIO_PIN, GPIO_PULL_DISABLE);
	lcm_util.set_gpio_pull_enable(LSDA_GPIO_PIN, GPIO_PULL_DISABLE);

	lcm_util.set_gpio_mode(GPIO66, GPIO_MODE_00);
	lcm_util.set_gpio_dir(GPIO66, GPIO_DIR_OUT);
	lcm_util.set_gpio_pull_enable(GPIO66, GPIO_PULL_DISABLE);

}
// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
	LCM_DEBUG("wanght==>[LCM********byd****]: lcm_set_util_funcs. \n");
}


static void lcm_get_params(LCM_PARAMS *params)
{
	LCM_DEBUG("wanght==>[LCM*****byd*******]: lcm_get_params. \n");

	memset(params, 0, sizeof(LCM_PARAMS));
	params->type   = LCM_TYPE_DPI;
#if defined(GN_MTK_BSP_LCD_SW_SPI)
	params->ctrl   = LCM_CTRL_GPIO; 
#else
    params->ctrl   = LCM_CTRL_SERIAL_DBI;
#endif	   
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	params->io_select_mode = 0;
    	
	/* serial host interface configurations */
    params->dbi.port                    = 0;
	params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_7M;
	params->dbi.data_width              = LCM_DBI_DATA_WIDTH_8BITS;
	params->dbi.cpu_write_bits	    = LCM_DBI_CPU_WRITE_16_BITS;
	params->dbi.io_driving_current	 = LCM_DRIVING_CURRENT_2MA;

	params->dbi.serial.cs_polarity      = LCM_POLARITY_RISING;
	params->dbi.serial.clk_polarity     = LCM_POLARITY_FALLING;
	params->dbi.serial.clk_phase        = LCM_CLOCK_PHASE_0;
	params->dbi.serial.is_non_dbi_mode  = 1;
	params->dbi.serial.clock_base = LCM_DBI_CLOCK_FREQ_52M;
	params->dbi.serial.clock_div = LCM_SERIAL_CLOCK_DIV_16;
 
   	/* RGB interface configurations */
   	params->dpi.mipi_pll_clk_ref  = 0;
	params->dpi.mipi_pll_clk_div1 = 42;
	params->dpi.mipi_pll_clk_div2 = 10;
	params->dpi.dpi_clk_div       = 2;
	params->dpi.dpi_clk_duty      = 1;

	params->dpi.clk_pol           = LCM_POLARITY_RISING;
	params->dpi.de_pol            = LCM_POLARITY_FALLING;
	params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
	params->dpi.hsync_pol         = LCM_POLARITY_FALLING;
	
	params->dpi.hsync_pulse_width = 9;
	params->dpi.hsync_back_porch  = 20;
	params->dpi.hsync_front_porch = 19;
	params->dpi.vsync_pulse_width =  2;
	params->dpi.vsync_back_porch  = 10; 
	params->dpi.vsync_front_porch =  10; 
    
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
	MDELAY(100);
	SET_RESET_PIN(1);
	MDELAY(100);
	config_gpio();
#if 0
  while(0)
    {
LCDSPI_InitCMD(0xD604);
	MDELAY(1);
    }
#endif
	init_lcm_registers();
}

static void lcm_suspend(void)
{
	LCM_DEBUG("[LCM************]: lcm_suspend. \n");
	LCDSPI_InitCMD(0x2800);
	LCDSPI_InitCMD(0x1000);//Sleep in
	MDELAY(120);
}

static void lcm_resume(void)
{
	LCDSPI_InitCMD(0x1100);
   	MDELAY(120);	//modify  20 to 120
	LCDSPI_InitCMD(0x2900);//Sleep in
	MDELAY(20);		//modify  120 to 20
}

static void lcm_setbacklight(unsigned int level)
{
	unsigned int default_level = 52;
	unsigned int mapped_level = 0;
	if(level > 0) 
		//mapped_level = 255;
		SET_GPIO_OUT(GPIO66, 1);      
	if(level ==0) 
		SET_GPIO_OUT(GPIO66, 0);  

		//mapped_level = default_level+(level-102)*(255-default_level)/(255-102);
	//else
		//mapped_level = default_level-(102-level)*default_level/102;
		

   //LCDSPI_InitCMD(0x5100); LCDSPI_InitDAT(mapped_level);	
        //LCM_DEBUG("[LCM_set_back_light]: =%d. \n",mapped_level);    
}

extern int IMM_GetOneChannelValue(int dwChannel, int deCount);
//gionee chenqiang 20110804 add for lcm compatible CR CR00423199
static unsigned int lcm_compare_id(void)
{ 
    unsigned int xx_temp=0;
    LCM_DEBUG("[LCM_truly_nt35510: lcm_compare_id. \n");
    xx_temp= IMM_GetOneChannelValue(COMPARE_ADC_CHANNEL,500);
    LCM_DEBUG("wanght==>xx_temp=%d \n",xx_temp);    
    return 0;
   //return (xx_temp < COMPARE_ID_V_LEVEL_1)?1:0;       
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER gn_byd_nt35510_lcm_drv = 
{
	.name		= "byd_nt35510",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
   	.set_backlight  = lcm_setbacklight, 
	.compare_id     = lcm_compare_id
};

