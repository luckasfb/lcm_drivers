
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

#define LCM_ID 0x2081
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
//5.0NEW AUO+RM68120 TIGONG 20120417
set_lcm_register(0XF000,0X55);
set_lcm_register(0XF001,0XAA);
set_lcm_register(0XF002,0X52);
set_lcm_register(0XF003,0X08);
set_lcm_register(0XF004,0X01);

//GAMMA RED
set_lcm_register(0XD100,0X00);  
set_lcm_register(0XD101,0X00);	  
set_lcm_register(0XD102,0X48);  
set_lcm_register(0XD103,0X8B);  
set_lcm_register(0XD104,0XB5);  
set_lcm_register(0XD105,0X54);  
set_lcm_register(0XD106,0XD5);  
set_lcm_register(0XD107,0X04);  
set_lcm_register(0XD108,0X28);  
set_lcm_register(0XD109,0X5E);  
set_lcm_register(0XD10A,0X55);  
set_lcm_register(0XD10B,0X86);  
set_lcm_register(0XD10C,0XA7);  
set_lcm_register(0XD10D,0XC2);  
set_lcm_register(0XD10E,0XDC);  
set_lcm_register(0XD10F,0XA9);
set_lcm_register(0XD110,0XF2);  
set_lcm_register(0XD111,0X05);  
set_lcm_register(0XD112,0X17);  
set_lcm_register(0XD113,0X28);  
set_lcm_register(0XD114,0XAA);  
set_lcm_register(0XD115,0X38);  
set_lcm_register(0XD116,0X46);  
set_lcm_register(0XD117,0X55);  
set_lcm_register(0XD118,0X63);  
set_lcm_register(0XD119,0XAA);  
set_lcm_register(0XD11A,0X70);  
set_lcm_register(0XD11B,0X71);  
set_lcm_register(0XD11C,0X7E); 
set_lcm_register(0XD11D,0X8B);  
set_lcm_register(0XD11E,0XAA);  
set_lcm_register(0XD11F,0X97);  
set_lcm_register(0XD120,0XA3);  
set_lcm_register(0XD121,0XB0);
set_lcm_register(0XD122,0XBC);
set_lcm_register(0XD123,0XAA);
set_lcm_register(0XD124,0XC8);
set_lcm_register(0XD125,0XD4);
set_lcm_register(0XD126,0XE0);
set_lcm_register(0XD127,0XEE);
set_lcm_register(0XD128,0XFE);
set_lcm_register(0XD129,0XFD);
set_lcm_register(0XD12A,0X0E);
set_lcm_register(0XD12B,0X25);
set_lcm_register(0XD12C,0X4B);
set_lcm_register(0XD12D,0XFF);
set_lcm_register(0XD12E,0X69);
set_lcm_register(0XD12F,0X97);
set_lcm_register(0XD130,0XB7);
set_lcm_register(0XD131,0XDD);
set_lcm_register(0XD132,0X0F);
set_lcm_register(0XD133,0XFB);  
set_lcm_register(0XD134,0XFF);  
//GAMMA GREEN
set_lcm_register(0XD200,0X00);  
set_lcm_register(0XD201,0X00);	  
set_lcm_register(0XD202,0X48);  
set_lcm_register(0XD203,0X8B);  
set_lcm_register(0XD204,0XB5);  
set_lcm_register(0XD205,0X54);  
set_lcm_register(0XD206,0XD5);  
set_lcm_register(0XD207,0X04);  
set_lcm_register(0XD208,0X28);  
set_lcm_register(0XD209,0X5E);  
set_lcm_register(0XD20A,0X55);  
set_lcm_register(0XD20B,0X86);  
set_lcm_register(0XD20C,0XA7);  
set_lcm_register(0XD20D,0XC2);  
set_lcm_register(0XD20E,0XDC);  
set_lcm_register(0XD20F,0XA9);
set_lcm_register(0XD210,0XF2);  
set_lcm_register(0XD211,0X05);  
set_lcm_register(0XD212,0X17);  
set_lcm_register(0XD213,0X28);  
set_lcm_register(0XD214,0XAA);  
set_lcm_register(0XD215,0X38);  
set_lcm_register(0XD216,0X46);  
set_lcm_register(0XD217,0X55);  
set_lcm_register(0XD218,0X63);  
set_lcm_register(0XD219,0XAA);  
set_lcm_register(0XD21A,0X70);  
set_lcm_register(0XD21B,0X71);  
set_lcm_register(0XD21C,0X7E); 
set_lcm_register(0XD21D,0X8B);  
set_lcm_register(0XD21E,0XAA);  
set_lcm_register(0XD21F,0X97);  
set_lcm_register(0XD220,0XA3);  
set_lcm_register(0XD221,0XB0);
set_lcm_register(0XD222,0XBC);
set_lcm_register(0XD223,0XAA);
set_lcm_register(0XD224,0XC8);
set_lcm_register(0XD225,0XD4);
set_lcm_register(0XD226,0XE0);
set_lcm_register(0XD227,0XEE);
set_lcm_register(0XD228,0XFE);
set_lcm_register(0XD229,0XFD);
set_lcm_register(0XD22A,0X0E);
set_lcm_register(0XD22B,0X25);
set_lcm_register(0XD22C,0X4B);
set_lcm_register(0XD22D,0XFF);
set_lcm_register(0XD22E,0X69);
set_lcm_register(0XD22F,0X97);
set_lcm_register(0XD230,0XB7);
set_lcm_register(0XD231,0XDD);
set_lcm_register(0XD232,0X0F);
set_lcm_register(0XD233,0XFB);  
set_lcm_register(0XD234,0XFF); 
//GAMMA SETT BLUE
set_lcm_register(0XD300,0X00);  
set_lcm_register(0XD301,0X00);	  
set_lcm_register(0XD302,0X48);  
set_lcm_register(0XD303,0X8B);  
set_lcm_register(0XD304,0XB5);  
set_lcm_register(0XD305,0X54);  
set_lcm_register(0XD306,0XD5);  
set_lcm_register(0XD307,0X04);  
set_lcm_register(0XD308,0X28);  
set_lcm_register(0XD309,0X5E);  
set_lcm_register(0XD30A,0X55);  
set_lcm_register(0XD30B,0X86);  
set_lcm_register(0XD30C,0XA7);  
set_lcm_register(0XD30D,0XC2);  
set_lcm_register(0XD30E,0XDC);  
set_lcm_register(0XD30F,0XA9);
set_lcm_register(0XD310,0XF2);  
set_lcm_register(0XD311,0X05);  
set_lcm_register(0XD312,0X17);  
set_lcm_register(0XD313,0X28);  
set_lcm_register(0XD314,0XAA);  
set_lcm_register(0XD315,0X38);  
set_lcm_register(0XD316,0X46);  
set_lcm_register(0XD317,0X55);  
set_lcm_register(0XD318,0X63);  
set_lcm_register(0XD319,0XAA);  
set_lcm_register(0XD31A,0X70);  
set_lcm_register(0XD31B,0X71);  
set_lcm_register(0XD31C,0X7E); 
set_lcm_register(0XD31D,0X8B);  
set_lcm_register(0XD31E,0XAA);  
set_lcm_register(0XD31F,0X97);  
set_lcm_register(0XD320,0XA3);  
set_lcm_register(0XD321,0XB0);
set_lcm_register(0XD322,0XBC);
set_lcm_register(0XD323,0XAA);
set_lcm_register(0XD324,0XC8);
set_lcm_register(0XD325,0XD4);
set_lcm_register(0XD326,0XE0);
set_lcm_register(0XD327,0XEE);
set_lcm_register(0XD328,0XFE);
set_lcm_register(0XD329,0XFD);
set_lcm_register(0XD32A,0X0E);
set_lcm_register(0XD32B,0X25);
set_lcm_register(0XD32C,0X4B);
set_lcm_register(0XD32D,0XFF);
set_lcm_register(0XD32E,0X69);
set_lcm_register(0XD32F,0X97);
set_lcm_register(0XD330,0XB7);
set_lcm_register(0XD331,0XDD);
set_lcm_register(0XD332,0X0F);
set_lcm_register(0XD333,0XFB);  
set_lcm_register(0XD334,0XFF); 

//GAMMA SET RED
set_lcm_register(0XD400,0X00);  
set_lcm_register(0XD401,0X00);	  
set_lcm_register(0XD402,0X48);  
set_lcm_register(0XD403,0X8B);  
set_lcm_register(0XD404,0XB5);  
set_lcm_register(0XD405,0X54);  
set_lcm_register(0XD406,0XD5);  
set_lcm_register(0XD407,0X04);  
set_lcm_register(0XD408,0X28);  
set_lcm_register(0XD409,0X5E);  
set_lcm_register(0XD40A,0X55);  
set_lcm_register(0XD40B,0X86);  
set_lcm_register(0XD40C,0XA7);  
set_lcm_register(0XD40D,0XC2);  
set_lcm_register(0XD40E,0XDC);  
set_lcm_register(0XD40F,0XA9);
set_lcm_register(0XD410,0XF2);  
set_lcm_register(0XD411,0X05);  
set_lcm_register(0XD412,0X17);  
set_lcm_register(0XD413,0X28);  
set_lcm_register(0XD414,0XAA);  
set_lcm_register(0XD415,0X38);  
set_lcm_register(0XD416,0X46);  
set_lcm_register(0XD417,0X55);  
set_lcm_register(0XD418,0X63);  
set_lcm_register(0XD419,0XAA);  
set_lcm_register(0XD41A,0X70);  
set_lcm_register(0XD41B,0X71);  
set_lcm_register(0XD41C,0X7E); 
set_lcm_register(0XD41D,0X8B);  
set_lcm_register(0XD41E,0XAA);  
set_lcm_register(0XD41F,0X97);  
set_lcm_register(0XD420,0XA3);  
set_lcm_register(0XD421,0XB0);
set_lcm_register(0XD422,0XBC);
set_lcm_register(0XD423,0XAA);
set_lcm_register(0XD424,0XC8);
set_lcm_register(0XD425,0XD4);
set_lcm_register(0XD426,0XE0);
set_lcm_register(0XD427,0XEE);
set_lcm_register(0XD428,0XFE);
set_lcm_register(0XD429,0XFD);
set_lcm_register(0XD42A,0X0E);
set_lcm_register(0XD42B,0X25);
set_lcm_register(0XD42C,0X4B);
set_lcm_register(0XD42D,0XFF);
set_lcm_register(0XD42E,0X69);
set_lcm_register(0XD42F,0X97);
set_lcm_register(0XD430,0XB7);
set_lcm_register(0XD431,0XDD);
set_lcm_register(0XD432,0X0F);
set_lcm_register(0XD433,0XFB);  
set_lcm_register(0XD434,0XFF); 
//GAMMA SET GREEN
set_lcm_register(0XD500,0X00);  
set_lcm_register(0XD501,0X00);	  
set_lcm_register(0XD502,0X48);  
set_lcm_register(0XD503,0X8B);  
set_lcm_register(0XD504,0XB5);  
set_lcm_register(0XD505,0X54);  
set_lcm_register(0XD506,0XD5);  
set_lcm_register(0XD507,0X04);  
set_lcm_register(0XD508,0X28);  
set_lcm_register(0XD509,0X5E);  
set_lcm_register(0XD50A,0X55);  
set_lcm_register(0XD50B,0X86);  
set_lcm_register(0XD50C,0XA7);  
set_lcm_register(0XD50D,0XC2);  
set_lcm_register(0XD50E,0XDC);  
set_lcm_register(0XD50F,0XA9);
set_lcm_register(0XD510,0XF2);  
set_lcm_register(0XD511,0X05);  
set_lcm_register(0XD512,0X17);  
set_lcm_register(0XD513,0X28);  
set_lcm_register(0XD514,0XAA);  
set_lcm_register(0XD515,0X38);  
set_lcm_register(0XD516,0X46);  
set_lcm_register(0XD517,0X55);  
set_lcm_register(0XD518,0X63);  
set_lcm_register(0XD519,0XAA);  
set_lcm_register(0XD51A,0X70);  
set_lcm_register(0XD51B,0X71);  
set_lcm_register(0XD51C,0X7E); 
set_lcm_register(0XD51D,0X8B);  
set_lcm_register(0XD51E,0XAA);  
set_lcm_register(0XD51F,0X97);  
set_lcm_register(0XD520,0XA3);  
set_lcm_register(0XD521,0XB0);
set_lcm_register(0XD522,0XBC);
set_lcm_register(0XD523,0XAA);
set_lcm_register(0XD524,0XC8);
set_lcm_register(0XD525,0XD4);
set_lcm_register(0XD526,0XE0);
set_lcm_register(0XD527,0XEE);
set_lcm_register(0XD528,0XFE);
set_lcm_register(0XD529,0XFD);
set_lcm_register(0XD52A,0X0E);
set_lcm_register(0XD52B,0X25);
set_lcm_register(0XD52C,0X4B);
set_lcm_register(0XD52D,0XFF);
set_lcm_register(0XD52E,0X69);
set_lcm_register(0XD52F,0X97);
set_lcm_register(0XD530,0XB7);
set_lcm_register(0XD531,0XDD);
set_lcm_register(0XD532,0X0F);
set_lcm_register(0XD533,0XFB);  
set_lcm_register(0XD534,0XFF); 

//GAMMA SET BLUE
set_lcm_register(0XD600,0X00);  
set_lcm_register(0XD601,0X00);	  
set_lcm_register(0XD602,0X48);  
set_lcm_register(0XD603,0X8B);  
set_lcm_register(0XD604,0XB5);  
set_lcm_register(0XD605,0X54);  
set_lcm_register(0XD606,0XD5);  
set_lcm_register(0XD607,0X04);  
set_lcm_register(0XD608,0X28);  
set_lcm_register(0XD609,0X5E);  
set_lcm_register(0XD60A,0X55);  
set_lcm_register(0XD60B,0X86);  
set_lcm_register(0XD60C,0XA7);  
set_lcm_register(0XD60D,0XC2);  
set_lcm_register(0XD60E,0XDC);  
set_lcm_register(0XD60F,0XA9);
set_lcm_register(0XD610,0XF2);  
set_lcm_register(0XD611,0X05);  
set_lcm_register(0XD612,0X17);  
set_lcm_register(0XD613,0X28);  
set_lcm_register(0XD614,0XAA);  
set_lcm_register(0XD615,0X38);  
set_lcm_register(0XD616,0X46);  
set_lcm_register(0XD617,0X55);  
set_lcm_register(0XD618,0X63);  
set_lcm_register(0XD619,0XAA);  
set_lcm_register(0XD61A,0X70);  
set_lcm_register(0XD61B,0X71);  
set_lcm_register(0XD61C,0X7E); 
set_lcm_register(0XD61D,0X8B);  
set_lcm_register(0XD61E,0XAA);  
set_lcm_register(0XD61F,0X97);  
set_lcm_register(0XD620,0XA3);  
set_lcm_register(0XD621,0XB0);
set_lcm_register(0XD622,0XBC);
set_lcm_register(0XD623,0XAA);
set_lcm_register(0XD624,0XC8);
set_lcm_register(0XD625,0XD4);
set_lcm_register(0XD626,0XE0);
set_lcm_register(0XD627,0XEE);
set_lcm_register(0XD628,0XFE);
set_lcm_register(0XD629,0XFD);
set_lcm_register(0XD62A,0X0E);
set_lcm_register(0XD62B,0X25);
set_lcm_register(0XD62C,0X4B);
set_lcm_register(0XD62D,0XFF);
set_lcm_register(0XD62E,0X69);
set_lcm_register(0XD62F,0X97);
set_lcm_register(0XD630,0XB7);
set_lcm_register(0XD631,0XDD);
set_lcm_register(0XD632,0X0F);
set_lcm_register(0XD633,0XFB);  
set_lcm_register(0XD634,0XFF); 
//GAMMA SET END


set_lcm_register(0XB000,0X00);  
set_lcm_register(0XB001,0X00);  
set_lcm_register(0XB002,0X00);  
set_lcm_register(0XB100,0X05);  
set_lcm_register(0XB101,0X05);  
set_lcm_register(0XB102,0X05);  
set_lcm_register(0XB600,0X44);  
set_lcm_register(0XB601,0X44);  
set_lcm_register(0XB602,0X44);  
set_lcm_register(0XB700,0X34);  
set_lcm_register(0XB701,0X34);  
set_lcm_register(0XB702,0X34);  
set_lcm_register(0XB800,0X24);
set_lcm_register(0XB801,0X24);
set_lcm_register(0XB802,0X24);
set_lcm_register(0XB900,0X34);
set_lcm_register(0XB901,0X34);
set_lcm_register(0XB902,0X34);
set_lcm_register(0XBA00,0X14);  
set_lcm_register(0XBA01,0X14);
set_lcm_register(0XBA02,0X14);
set_lcm_register(0XBF00,0X01); 
set_lcm_register(0XB300,0X07);  
set_lcm_register(0XB301,0X07);
set_lcm_register(0XB302,0X07);
set_lcm_register(0XB900,0X25);
set_lcm_register(0XB901,0X25);
set_lcm_register(0XB902,0X25);
set_lcm_register(0XBC00,0X00);
set_lcm_register(0XBC01,0X90);//A0  90  A8
set_lcm_register(0XBC02,0X61);//3A  61  00
set_lcm_register(0XBD00,0X00);
set_lcm_register(0XBD01,0X90);//A0
set_lcm_register(0XBD02,0X61);//3A
set_lcm_register(0XBE00,0X00);
set_lcm_register(0XBE01,0X8F);//8e
set_lcm_register(0XF000,0X55);
set_lcm_register(0XF001,0XAA);
set_lcm_register(0XF002,0X52);
set_lcm_register(0XF003,0X08);
set_lcm_register(0XF004,0X00);
set_lcm_register(0XB400,0X10);
set_lcm_register(0XB600,0X02);
set_lcm_register(0XB100,0XCC);
set_lcm_register(0XB700,0X22);
set_lcm_register(0XB701,0X22);
set_lcm_register(0XC80B,0X2A);
set_lcm_register(0XC80C,0X2A);
set_lcm_register(0XC80F,0X2A);
set_lcm_register(0XC810,0X2A);
set_lcm_register(0XB800,0X01);
set_lcm_register(0XB801,0X03);
set_lcm_register(0XB802,0X03);
set_lcm_register(0XB803,0X03);
set_lcm_register(0XBC00,0X05);//05
set_lcm_register(0XBC01,0X05);
set_lcm_register(0XBC02,0X05);
set_lcm_register(0XD000,0X01);
set_lcm_register(0XBA00,0X01);
set_lcm_register(0XBD02,0X07);
set_lcm_register(0XBD03,0X31);
set_lcm_register(0XBE02,0X07);
set_lcm_register(0XBE03,0X31);
set_lcm_register(0XBF02,0X07);
set_lcm_register(0XBF03,0X31);
set_lcm_register(0XB300,0X00);
set_lcm_register(0XBD00,0X07);
set_lcm_register(0XBE00,0X07);
set_lcm_register(0XBF00,0X07);
set_lcm_register(0XF000,0X55);
set_lcm_register(0XF001,0XAA);
set_lcm_register(0XF002,0X52);
set_lcm_register(0XF003,0X08);
set_lcm_register(0XF004,0X02);
set_lcm_register(0XC301,0XA9);
set_lcm_register(0XFE01,0X94);
set_lcm_register(0XF600,0X60);
set_lcm_register(0X3500,0X00);
set_lcm_register(0XF000,0X55);
set_lcm_register(0XF001,0XAA);
set_lcm_register(0XF002,0X52);
set_lcm_register(0XF003,0X08);
set_lcm_register(0XF004,0X01);//01
set_lcm_register(0X3600,0X00);
set_lcm_register(0X3a00,0X77);	//	  



send_ctrl_cmd(0X1100); //
MDELAY(120);  
send_ctrl_cmd(0X2900); //
MDELAY(10);  
send_ctrl_cmd(0X2C00); //
MDELAY(20); 

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
     send_ctrl_cmd(0x2800);
    MDELAY(10);
	send_ctrl_cmd(0x1000);
	MDELAY(150);
}


static void lcm_resume(void)
{
    send_ctrl_cmd(0x1100);
    MDELAY(150);
	send_ctrl_cmd(0x2900);
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

	set_lcm_register(0xF000,0x55);             
	set_lcm_register(0xF001,0xAA);             
	set_lcm_register(0xF002,0x52);             
	set_lcm_register(0xF003,0x08);             
	set_lcm_register(0xF004,0x01);
	
	send_ctrl_cmd(0xc500);
	id1 = spi_read_data();  //spi_read_data();
	MDELAY(20);
	send_ctrl_cmd(0xc501);
	id2 = spi_read_data();  //spi_read_data();

	id = (id1<<8)|id2;


	return (LCM_ID == id)?1:0;
}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------




LCM_DRIVER lm050amybo1j_wvga_rm68120_lcm_drv = 
{

     .name			= "lm050amybo1j_wvga_rm68120",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};

