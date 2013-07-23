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
#include "lcm_drv.h"
//#define LCD_DEBUG
#ifdef BUILD_UBOOT
#include <asm/arch/mt6577_gpio.h>
#include <asm/arch/mt6577_pwm.h>
#ifdef LCD_DEBUG
#define LCM_DEBUG(format, ...)   printf("uboot ssd2825" format "\n", ## __VA_ARGS__)
#else
#define LCM_DEBUG(format, ...)
#endif
#else
#include <mach/mt6577_gpio.h>
#include <mach/mt6577_pwm.h>
#ifdef LCD_DEBUG
#define LCM_DEBUG(format, ...)   printk("kernel ssd2825" format "\n", ## __VA_ARGS__)
#else
#define LCM_DEBUG(format, ...)
#endif
#endif

#define LCD_RESET_SUSPEND_LOW

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define LSCE_GPIO_PIN                 (GPIO_DISP_LSCE_PIN) //(GPIO47)
#define LSCK_GPIO_PIN                 (GPIO_DISP_LSCK_PIN) //(GPIO51)
#define LSDA_GPIO_PIN                 (GPIO_DISP_LSDA_PIN) //(GPIO52)
#define LSDI_GPIO_PIN                 (GPIO_DISP_LSA0_PIN) //(GPIO48)

#define SSD2825_SHUT_GPIO_PIN         (GPIO_SSD2825_SHUT_PIN)   //(GPIO83) 
#define SSD2825_POWER_GPIO_PIN        (GPIO_SSD2825_POWER_PIN)  //(GPIO106)
#define SSD2825_MIPI_CLK_GPIO_PIN     (GPIO_SSD2825_CLK_PIN)      //(GPIO70)
#define LCD_POWER_GPIO_PIN            (GPIO_LCD_POWER_PIN)         //  (GPIO86)

#ifdef LCD_RESET_SUSPEND_LOW
#define LCD_RESET_GPIO_PIN   (GPIO18)
#define SET_LCD_RESET_lOW                 SET_GPIO_OUT(LCD_RESET_GPIO_PIN, 0)
#define SET_LCD_RESET_HIGH                SET_GPIO_OUT(LCD_RESET_GPIO_PIN, 1)
#endif

#define FRAME_WIDTH                   (720)
#define FRAME_HEIGHT                  (1280)

#define SSD2825_ID                    (0x2825) 

#define GAMMABACKLIGHT_NUM             25
#define GAMMA_TABLE_MAX_INDEX          ARRAY_OF(gamma_table)-1

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {0};

#define UDELAY(n)	                (lcm_util.udelay(n))
#define MDELAY(n)	                (lcm_util.mdelay(n))

#define SET_RESET_PIN(v)	        (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)	        (lcm_util.set_gpio_out((n), (v)))


#define SET_LSCE_LOW                 SET_GPIO_OUT(LSCE_GPIO_PIN, 0)
#define SET_LSCE_HIGH                SET_GPIO_OUT(LSCE_GPIO_PIN, 1)
#define SET_LSCK_LOW   				 SET_GPIO_OUT(LSCK_GPIO_PIN, 0)
#define SET_LSCK_HIGH  				 SET_GPIO_OUT(LSCK_GPIO_PIN, 1)
#define SET_LSDA_LOW                 SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH                SET_GPIO_OUT(LSDA_GPIO_PIN, 1)
#define GET_HX_SDI                   mt_get_gpio_in(LSDI_GPIO_PIN)


#define HX_WR_COM                   (0x70)
#define HX_WR_REGISTER              (0x72)
#define HX_RD_REGISTER              (0x73)

static void setGammaBacklight(unsigned int level);
static void setDynamicElvss(unsigned int level);
extern int lcd_set_pwm(int pwm_num);
static void lcm_setbacklight(unsigned int level); 

static int   g_last_Backlight_level =  -1;   
#define    ARRAY_OF(x)      ((int)(sizeof(x)/sizeof(x[0])))

#ifndef BUILD_UBOOT    
static int acl_on = -1;      // add by lisheng for CR00727852 
#endif

#ifdef BUILD_UBOOT
static int g_resume_flag = 0;
#endif

#ifndef BUILD_UBOOT
struct timeval start,stop,diff;

static int read_flag=0;

static DEFINE_MUTEX(setbacklight_flag);

extern struct timer_list esd_timer;

extern int esd_thread_timeout;

#endif


struct __gamma_backlight {
    unsigned int backlight_level;                       //backlight level
    unsigned char gammaValue[GAMMABACKLIGHT_NUM]; //gamma value for backlight
};

static struct __gamma_backlight gamma_table[] = {
{0,  {0x01,0x52,0x24,0x5D,0x00,0x00,0x00,0x74,0x78,0x5F,0xA8,0xB6,0xAE,0x8A,0xA5,0x93,0xB8,0xBF,0xB3,0x00,0x58,0x00,0x39,0x00,0x62}},
{10, {0x01,0x52,0x24,0x5D,0x00,0x00,0x00,0x74,0x78,0x5F,0xA8,0xB6,0xAE,0x8A,0xA5,0x93,0xB8,0xBF,0xB3,0x00,0x58,0x00,0x39,0x00,0x62}},
{20, {0x01,0x52,0x24,0x5D,0x00,0x00,0x00,0x74,0x78,0x5F,0xA8,0xB6,0xAE,0x8A,0xA5,0x93,0xB8,0xBF,0xB3,0x00,0x58,0x00,0x39,0x00,0x62}},
{30, {0x01,0x52,0x24,0x5D,0x00,0x00,0x00,0x74,0x78,0x5F,0xA8,0xB6,0xAE,0x8A,0xA5,0x93,0xB8,0xBF,0xB3,0x00,0x58,0x00,0x39,0x00,0x62}},
{40, {0x01,0x52,0x24,0x5D,0x00,0x00,0x08,0x7E,0x84,0x6D,0xAD,0xBE,0xB7,0x8C,0xA3,0x91,0xB7,0xC0,0xB3,0x00,0x61,0x00,0x41,0x00,0x6B}},
{50, {0x01,0x52,0x24,0x5D,0x22,0x09,0x1C,0x83,0x8D,0x77,0xAF,0xC4,0xBC,0x8F,0xA3,0x91,0xB7,0xBC,0xB1,0x00,0x68,0x00,0x49,0x00,0x73}},	
{60, {0x01,0x52,0x24,0x5D,0x37,0x23,0x28,0x87,0x95,0x83,0xB2,0xC5,0xBD,0x90,0xA0,0x8E,0xB8,0xBE,0xB7,0x00,0x6E,0x00,0x50,0x00,0x78}},
{70, {0x01,0x52,0x24,0x5D,0x47,0x38,0x34,0x8B,0x9B,0x8C,0xB3,0xC7,0xBD,0x93,0x9F,0x8E,0xB6,0xBD,0xB4,0x00,0x74,0x00,0x57,0x00,0x80}},
{80, {0x01,0x52,0x24,0x5D,0x51,0x45,0x3B,0x8E,0x9F,0x94,0xB5,0xC8,0xBC,0x91,0x9D,0x8E,0xB8,0xBD,0xB2,0x00,0x79,0x00,0x5C,0x00,0x86}},
{90, {0x01,0x52,0x24,0x5D,0x58,0x4E,0x40,0x90,0xA4,0x97,0xB6,0xC8,0xBC,0x90,0x9D,0x8F,0xB9,0xBB,0xB1,0x00,0x7D,0x00,0x61,0x00,0x8A}},
{100,{0x01,0x52,0x24,0x5D,0x5F,0x58,0x47,0x92,0xA8,0x9C,0xB7,0xC8,0xBC,0x91,0x9B,0x8D,0xB7,0xBC,0xB2,0x00,0x83,0x00,0x66,0x00,0x90}},
{110,{0x01,0x52,0x24,0x5D,0x65,0x5F,0x4B,0x94,0xA9,0x9F,0xB6,0xC6,0xBB,0x91,0x9C,0x8C,0xB7,0xBB,0xB3,0x00,0x87,0x00,0x6A,0x00,0x94}},
{120,{0x01,0x52,0x24,0x5D,0x68,0x66,0x50,0x95,0xAC,0xA2,0xB8,0xC7,0xB9,0x93,0x9B,0x8D,0xB5,0xBA,0xB2,0x00,0x88,0x00,0x6F,0x00,0x99}},
{130,{0x01,0x52,0x24,0x5D,0x6B,0x69,0x54,0x97,0xAE,0xA3,0xB7,0xC7,0xBB,0x92,0x9A,0x8B,0xB8,0xB9,0xB1,0x00,0x8D,0x00,0x73,0x00,0x9D}},
{140,{0x01,0x52,0x24,0x5D,0x6E,0x6F,0x57,0x97,0xB1,0xA4,0xB8,0xC5,0xBA,0x92,0x9A,0x8C,0xB6,0xB9,0xB1,0x00,0x92,0x00,0x77,0x00,0xA1}},
{150,{0x01,0x52,0x24,0x5D,0x71,0x73,0x5B,0x98,0xB1,0xA6,0xBA,0xC4,0xB7,0x90,0x9B,0x8D,0xB6,0xB8,0xB0,0x00,0x96,0x00,0x7A,0x00,0xA5}},
{160,{0x01,0x52,0x24,0x5D,0x74,0x77,0x5F,0x98,0xB3,0xA6,0xBA,0xC4,0xB9,0x92,0x9B,0x8B,0xB4,0xB6,0xB2,0x00,0x99,0x00,0x7E,0x00,0xA7}},
{170,{0x01,0x52,0x24,0x5D,0x76,0x7A,0x63,0x9B,0xB4,0xA7,0xB7,0xC3,0xB7,0x93,0x9A,0x8D,0xB4,0xB6,0xB0,0x00,0x9D,0x00,0x82,0x00,0xAC}},
{180,{0x01,0x52,0x24,0x5D,0x77,0x7E,0x66,0x9B,0xB5,0xA6,0xB9,0xC3,0xB8,0x92,0x99,0x8C,0xB3,0xB7,0xAE,0x00,0xA0,0x00,0x85,0x00,0xB0}},
{190,{0x01,0x52,0x24,0x5D,0x7A,0x81,0x69,0x9C,0xB5,0xA6,0xB9,0xC3,0xB9,0x92,0x99,0x8B,0xB3,0xB6,0xAF,0x00,0xA3,0x00,0x89,0x00,0xB3}},
{200,{0x01,0x52,0x24,0x5D,0x7B,0x83,0x6B,0x9C,0xB5,0xA7,0xBA,0xC3,0xB9,0x91,0x98,0x8B,0xB3,0xB6,0xAD,0x00,0xA6,0x00,0x8C,0x00,0xB7}},
{210,{0x01,0x52,0x24,0x5D,0x7D,0x87,0x6E,0x9D,0xB6,0xA7,0xB7,0xC2,0xB9,0x93,0x99,0x8B,0xB1,0xB5,0xAD,0x00,0xAA,0x00,0x8F,0x00,0xBA}},
{220,{0x01,0x52,0x24,0x5D,0x80,0x84,0x70,0x9E,0xB6,0xA8,0xB9,0xC2,0xB7,0x92,0x98,0x8B,0xB0,0xB4,0xAD,0x00,0xAC,0x00,0x92,0x00,0xBD}},
{230,{0x01,0x52,0x24,0x5D,0x7E,0x8A,0x72,0x9E,0xB6,0xA8,0xBA,0xC3,0xB8,0x90,0x97,0x8A,0xB1,0xB5,0xAD,0x00,0xAF,0x00,0x95,0x00,0xC0}},
//{240,{0x01,0x52,0x24,0x5D,0x81,0x8D,0x75,0x9D,0xB5,0xA8,0xBA,0xCC,0xB7,0x92,0x97,0x8A,0xAE,0xB4,0xAD,0x00,0xB3,0x00,0x98,0x00,0xC3}},
{250,{0x01,0x52,0x24,0x5D,0x82,0x8A,0x75,0x9E,0xB6,0xA7,0xBA,0xC2,0xB9,0x92,0x97,0x8A,0xAE,0xB3,0xAE,0x00,0xB5,0x00,0x9A,0x00,0xC4}},
{260,{0x01,0x52,0x24,0x5D,0x84,0x91,0x79,0x9F,0xB5,0xA9,0xBA,0xC2,0xB6,0x91,0x97,0x8A,0xAF,0xB2,0xAB,0x00,0xB7,0x00,0x9E,0x00,0xC9}},
{270,{0x01,0x52,0x24,0x5D,0x84,0x92,0x7A,0x9F,0xB6,0xA8,0xB8,0xC2,0xB8,0x8F,0x97,0x89,0xB0,0xB2,0xAB,0x00,0xBA,0x00,0xA1,0x00,0xCC}},
{280,{0x01,0x52,0x24,0x5D,0x85,0x94,0x7C,0xA0,0xB6,0xA7,0xB9,0xC1,0xB5,0x91,0x97,0x8A,0xAE,0xB1,0xAB,0x00,0xBC,0x00,0xA3,0x00,0xCF}},
//{290,{0x01,0x52,0x24,0x5D,0x86,0x97,0x7E,0xA1,0xB5,0xA7,0xBA,0xC3,0xB7,0x90,0x96,0x8A,0xAE,0xB0,0xAB,0x00,0xBF,0x00,0xA7,0x00,0xD1}}, //del by lisheng for CR00721796
//{300,{0x01,0x52,0x24,0x5D,0xBA,0xCD,0xB3,0xAD,0xC0,0xB1,0xBF,0xC7,0xBC,0x90,0x97,0x8A,0xAA,0xAE,0xA5,0x00,0xC2,0x00,0xA8,0x00,0xD7}},//del by lisheng for CR00721796
};

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
static __inline void spi_send_data(unsigned int data)
{
	unsigned int i;
	
	SET_LSCE_HIGH;
	SET_LSCK_HIGH;
	SET_LSDA_HIGH;
	UDELAY(10);
	
	SET_LSCE_LOW;
	UDELAY(10);
	
	
	for (i = 0; i < 24; ++i)
	{
		if (data & (1 << 23)){
			SET_LSDA_HIGH;
		}
		else {
			SET_LSDA_LOW;
		}
		data <<= 1;
		UDELAY(10);
		SET_LSCK_LOW;
		UDELAY(10);
		SET_LSCK_HIGH;
		UDELAY(10);
	}
	
	SET_LSDA_HIGH;
	SET_LSCE_HIGH;
}

static __inline void Write_com(unsigned int cmd)
{
    unsigned int out = ((HX_WR_COM<<16) | (cmd & 0xFFFF));
    spi_send_data(out);
}

static __inline void Write_register(unsigned int data)
{
    unsigned int out = ((HX_WR_REGISTER<<16) |(data & 0xFFFF));
    spi_send_data(out);
}
  
static __inline unsigned short Read_register(void)
{
	unsigned char i,j,front_data;
	unsigned short value = 0;

	front_data=HX_RD_REGISTER;

	SET_LSCE_HIGH;
	SET_LSCK_HIGH;
	SET_LSDA_HIGH;
	UDELAY(10);
	SET_LSCE_LOW;
	UDELAY(10); 

	for(i=0;i<8;i++) // 8  Data
	{
		if ( front_data& 0x80)
			SET_LSDA_HIGH;
		else
			SET_LSDA_LOW;
		front_data<<= 1;
		UDELAY(10);
		SET_LSCK_LOW;
		UDELAY(10); 
		SET_LSCK_HIGH;
		UDELAY(10);		  
	}
	MDELAY(1);

	for(j=0;j<16;j++) // 16 Data
	{
		SET_LSCK_HIGH;
		UDELAY(10);
		SET_LSCK_LOW;
		value<<= 1;
		value |= GET_HX_SDI;

		UDELAY(10); 
	}

	SET_LSCE_HIGH;
	return value;

}

static void init_lcm_registers(void)
{
	LCM_DEBUG("[LCM************]: init_lcm_registers. \n");
	
	Write_com(0x00B1);
	Write_register(0x0102);
	Write_com(0x00B2);
	Write_register(0x030E);	
	Write_com(0x00B3);
	Write_register(0x0D50);
	Write_com(0x00B4);
	Write_register(0x02D0);
	Write_com(0x00B5);
	Write_register(0x0500);
	Write_com(0x00B6);
	Write_register(0x000B);	//0x0007 yangqb ,from Non-burst  to burst mode
	//MDELAY(2);

	Write_com(0x00DE);
	Write_register(0x0003);
	Write_com(0x00D6);
	Write_register(0x0004);
	Write_com(0x00B9);
	Write_register(0x0000);

	Write_com(0x00BA);
	Write_register(0x801D);

	Write_com(0x00BB);
	Write_register(0x0009);
	Write_com(0x00B9);
	Write_register(0x0001);
	Write_com(0x00B8);
	Write_register(0x0000);

	//F0
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0003);
	Write_com(0x00BF);
	Write_register(0x5AF0);
	Write_register(0x005A);
	//F1
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0003);
	Write_com(0x00BF);
	Write_register(0x5AF1);
	Write_register(0x005A);

	//0x11
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0001);
	Write_com(0x00BF);
	Write_register(0x0011);
	MDELAY(5);	//100

	//F8
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0027);
	Write_com(0x00BF);
	Write_register(0x2DF8);
	Write_register(0x0035);
	Write_register(0x0000);
	Write_register(0x0093);
	Write_register(0x7D3C);
	Write_register(0x2708);
	Write_register(0x3f7d);
	Write_register(0x0000);
	Write_register(0x2000);
	Write_register(0x0804);
	Write_register(0x006E);
	Write_register(0x0000);
	Write_register(0x0802);
	Write_register(0x2308);
	Write_register(0xC023);
	Write_register(0x08C8);
	Write_register(0xC148);
	Write_register(0xC100);
	Write_register(0xFFFF);
	Write_register(0x00C8);

	//F2
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0004);
	Write_com(0x00BF);
	Write_register(0x80F2);
	Write_register(0x0D03); 
	
	//F6
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0003);
	Write_com(0x00BF);
	Write_register(0x00F6);
	Write_register(0x0002);

	
	//B6
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x000A);
	Write_com(0x00BF);
	Write_register(0x0CB6);
	Write_register(0x0302);
	Write_register(0xFF32);
	Write_register(0x4444);
	Write_register(0x00C0);
	 
	//D9
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x000F);
	Write_com(0x00BF);
	Write_register(0x14D9);
	Write_register(0x0C40);
	Write_register(0xCECB);
	Write_register(0xC46E);
	Write_register(0x4007);
	Write_register(0xCB41);
	Write_register(0x6000);
	Write_register(0x0019);

	//E1
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0006);
	Write_com(0x00BF);
	Write_register(0x10E1);
	Write_register(0x171C);
	Write_register(0x1D08);
	
	//E2
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0007);
	Write_com(0x00BF);
	Write_register(0xEDE2);
	Write_register(0xC307);
	Write_register(0x0D13);
	Write_register(0x0003);

	//E3
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0002);
	Write_com(0x00BF);
	Write_register(0x40E3);
	
	//E4
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0008);
	Write_com(0x00BF);
	Write_register(0x00E4);
	Write_register(0x1400);
	Write_register(0x0080);
	Write_register(0x0000);
	
	//F4
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0008);
	Write_com(0x00BF);
	Write_register(0xCFF4);
	Write_register(0x120A);
	Write_register(0x1E10);
	Write_register(0x0233);

	//C1
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x001D);
	Write_com(0x00BF);
	Write_register(0x47C1);
	Write_register(0x1353);
	Write_register(0x0053);
	Write_register(0x0200);
	Write_register(0x00CF);
	Write_register(0x0400);
	Write_register(0x00FF);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0901);
	Write_register(0x1810);
	Write_register(0x271F);
	Write_register(0x362E);
	Write_register(0x453D);
	Write_register(0x004C);


#ifndef  BUILD_UBOOT
        if(acl_on == 1)              //lisheng add for CR00727852
        {
	        	//C0
		Write_com(0x00B7);
		Write_register(0x034B);
		Write_com(0x00B8);
		Write_register(0x0000);
		Write_com(0x00BC);
		Write_register(0x0002);
		Write_com(0x00BF);
		Write_register(0x01C0);
        }
	else
        {
	        	//C0
		Write_com(0x00B7);
		Write_register(0x034B);
		Write_com(0x00B8);
		Write_register(0x0000);
		Write_com(0x00BC);
		Write_register(0x0002);
		Write_com(0x00BF);
		Write_register(0x00C0);
        }

#else
	//C0
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0002);
	Write_com(0x00BF);
	Write_register(0x00C0);
#endif

	
}

void S6e88a_Lcm_ACL_Enable(unsigned int on_off)
{	
	#ifndef BUILD_UBOOT	
	mutex_lock(&setbacklight_flag);
	#endif

	if(on_off)
	{
		#ifndef BUILD_UBOOT	
	                acl_on = 1;        //lisheng add for CR00727852
		#endif
		
		//C0
		Write_com(0x00B7);
		Write_register(0x034B);
		Write_com(0x00B8);
		Write_register(0x0000);
		Write_com(0x00BC);
		Write_register(0x0002);
		Write_com(0x00BF);
		Write_register(0x01C0);
	}
	else
	{
		#ifndef BUILD_UBOOT	
                	acl_on = 0;     //lisheng add for CR00727852
		#endif
		//C0
		Write_com(0x00B7);
		Write_register(0x034B);
		Write_com(0x00B8);
		Write_register(0x0000);
		Write_com(0x00BC);
		Write_register(0x0002);
		Write_com(0x00BF);
		Write_register(0x00C0);
	}
	#ifndef BUILD_UBOOT	
	mutex_unlock(&setbacklight_flag);
	#endif

}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void config_gpio(void)
{
	lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_DISP_LSCE_PIN_M_GPIO);
	lcm_util.set_gpio_mode(LSCK_GPIO_PIN, GPIO_DISP_LSCK_PIN_M_GPIO);
	lcm_util.set_gpio_mode(LSDA_GPIO_PIN, GPIO_DISP_LSDA_PIN_M_GPIO);
	lcm_util.set_gpio_mode(LSDI_GPIO_PIN, GPIO_MODE_00);

	lcm_util.set_gpio_dir(LSCE_GPIO_PIN, GPIO_DIR_OUT);
	lcm_util.set_gpio_dir(LSCK_GPIO_PIN, GPIO_DIR_OUT);
	lcm_util.set_gpio_dir(LSDA_GPIO_PIN, GPIO_DIR_OUT);
	lcm_util.set_gpio_dir(LSDI_GPIO_PIN, GPIO_DIR_IN);

	
	lcm_util.set_gpio_pull_enable(LSCE_GPIO_PIN, GPIO_PULL_DISABLE);
	lcm_util.set_gpio_pull_enable(LSCK_GPIO_PIN, GPIO_PULL_DISABLE);
	lcm_util.set_gpio_pull_enable(LSDA_GPIO_PIN, GPIO_PULL_DISABLE);
	lcm_util.set_gpio_pull_enable(LSDI_GPIO_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(LSDI_GPIO_PIN, GPIO_PULL_UP);

	//set pwm output clk
	lcm_util.set_gpio_mode(SSD2825_MIPI_CLK_GPIO_PIN, GPIO_MODE_02);
	lcm_util.set_gpio_dir(SSD2825_MIPI_CLK_GPIO_PIN, GPIO_DIR_OUT); 
	lcm_util.set_gpio_pull_enable(SSD2825_MIPI_CLK_GPIO_PIN, GPIO_PULL_DISABLE); 
	lcd_set_pwm(PWM0);
	MDELAY(10);
	
	//set ssd2825 shut ping high
	lcm_util.set_gpio_mode(SSD2825_SHUT_GPIO_PIN, GPIO_MODE_00);
	lcm_util.set_gpio_dir(SSD2825_SHUT_GPIO_PIN, GPIO_DIR_OUT); 
	lcm_util.set_gpio_pull_enable(SSD2825_SHUT_GPIO_PIN, GPIO_PULL_DISABLE); 
	SET_GPIO_OUT(SSD2825_SHUT_GPIO_PIN , 1);
	MDELAY(1);
	
	//set ssd2825 poweron
	lcm_util.set_gpio_mode(SSD2825_POWER_GPIO_PIN, GPIO_MODE_00);
	lcm_util.set_gpio_dir(SSD2825_POWER_GPIO_PIN, GPIO_DIR_OUT); 
	lcm_util.set_gpio_pull_enable(SSD2825_POWER_GPIO_PIN, GPIO_PULL_DISABLE); 
	SET_GPIO_OUT(SSD2825_POWER_GPIO_PIN , 1);
	MDELAY(1);

	//set s6e8aa poweron
	lcm_util.set_gpio_mode(LCD_POWER_GPIO_PIN, GPIO_MODE_00);
	lcm_util.set_gpio_dir(LCD_POWER_GPIO_PIN, GPIO_DIR_OUT); 
	lcm_util.set_gpio_pull_enable(LCD_POWER_GPIO_PIN, GPIO_PULL_DISABLE); 
	SET_GPIO_OUT(LCD_POWER_GPIO_PIN , 1);
	MDELAY(50);
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
	params->dpi.mipi_pll_clk_div2 = 5;
	params->dpi.dpi_clk_div       = 2;
	params->dpi.dpi_clk_duty      = 1;

	params->dpi.clk_pol           = LCM_POLARITY_FALLING;
	params->dpi.de_pol            = LCM_POLARITY_RISING;
	params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
	params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

	params->dpi.hsync_pulse_width = 0x02;
	params->dpi.hsync_back_porch  = 0x0E;
	params->dpi.hsync_front_porch = 0x50;
	params->dpi.vsync_pulse_width = 0x01;
	params->dpi.vsync_back_porch  = 0x02;
	params->dpi.vsync_front_porch = 0x0D;

	params->dpi.format            = LCM_DPI_FORMAT_RGB888;
	params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
	params->dpi.is_serial_output  = 0;

	params->dpi.intermediat_buffer_num = 2;
	params->dpi.io_driving_current = LCM_DRIVING_CURRENT_4MA;

	params->dpi.i2x_en = 0;
	params->dpi.i2x_edge = 1;
}

static void lcm_init(void)
{ 

#ifdef LCD_RESET_SUSPEND_LOW
	 SET_LCD_RESET_lOW;
	 MDELAY(25);
#endif

	 config_gpio();

#ifdef LCD_RESET_SUSPEND_LOW

	 SET_LCD_RESET_HIGH;
	 MDELAY(120);
#else
	 SET_RESET_PIN(0);
	 MDELAY(25);
	 SET_RESET_PIN(1);
	 MDELAY(120);
#endif

	 init_lcm_registers();
}

static void lcm_suspend(void)
{

#ifdef BUILD_UBOOT
	g_resume_flag=1;
#endif
#ifndef BUILD_UBOOT	
	del_timer(&esd_timer);
	mutex_lock(&setbacklight_flag);
#endif

	Write_com(0x00B7); 
	Write_register(0x034B);  
	Write_com(0x00B8); 
	Write_register(0x0000);  
	Write_com(0x00BC); 
	Write_register(0x0001);
	Write_com(0xBF);
	Write_register(0x0010);  
	MDELAY(120);
	Write_com(0x00B7); 
	Write_register(0x0344);  
	Write_com(0x00B9);
	Write_register(0x0000); 
	LCM_DEBUG("lcm_suspend\n");

#ifdef LCD_RESET_SUSPEND_LOW
	//set s6e8aa poweroff
	lcm_util.set_gpio_mode(LCD_POWER_GPIO_PIN, GPIO_MODE_00);
	lcm_util.set_gpio_dir(LCD_POWER_GPIO_PIN, GPIO_DIR_OUT); 
	lcm_util.set_gpio_pull_enable(LCD_POWER_GPIO_PIN, GPIO_PULL_DISABLE); 
	SET_GPIO_OUT(LCD_POWER_GPIO_PIN , 0);
	MDELAY(50);

	//set ssd2825 poweroff
	lcm_util.set_gpio_mode(SSD2825_POWER_GPIO_PIN, GPIO_MODE_00);
	lcm_util.set_gpio_dir(SSD2825_POWER_GPIO_PIN, GPIO_DIR_OUT); 
	lcm_util.set_gpio_pull_enable(SSD2825_POWER_GPIO_PIN, GPIO_PULL_DISABLE); 
	SET_GPIO_OUT(SSD2825_POWER_GPIO_PIN , 0);
	MDELAY(1);
	
	SET_LCD_RESET_HIGH;
	MDELAY(20);
	SET_LCD_RESET_lOW;
	MDELAY(25);
	SET_LCD_RESET_HIGH;
	MDELAY(20);
	SET_LCD_RESET_lOW;
	MDELAY(25);
#endif
	
#ifndef BUILD_UBOOT	
	mutex_unlock(&setbacklight_flag);
#endif
}

static void lcm_resume(void)
{
#ifdef BUILD_UBOOT
	if(g_resume_flag==0)
	{
		g_resume_flag=1;
		return;
	}
#endif

	
	config_gpio();

#ifdef LCD_RESET_SUSPEND_LOW
	SET_LCD_RESET_HIGH;
	MDELAY(25);
	SET_LCD_RESET_lOW;
	MDELAY(30);
	SET_LCD_RESET_HIGH;
	MDELAY(120);
#else
	SET_RESET_PIN(0);
	MDELAY(25);
	SET_RESET_PIN(1);
	MDELAY(120);
#endif
	init_lcm_registers();
}

static unsigned int lcm_compare_id(void)
{
	unsigned short id;
	Write_com(0x00b0);	  
	id=Read_register();
	LCM_DEBUG("lcm_compare_id id is: %x\n",id);
 
	return (SSD2825_ID == id)?1:0;

}
static void setDynamicElvss(unsigned int index)
{

	if(gamma_table[index].backlight_level>200)
	{
		Write_com(0x00B7);
		Write_register(0x034B);
		Write_com(0x00B8);
		Write_register(0x0000);
		Write_com(0x00BC);
		Write_register(0x0003);
		Write_com(0x00BF);
		Write_register(0x04B1);
		Write_register(0x008B);
	}
	else
	{
		Write_com(0x00B7);
		Write_register(0x034B);
		Write_com(0x00B8);
		Write_register(0x0000);
		Write_com(0x00BC);
		Write_register(0x0003);
		Write_com(0x00BF);
		Write_register(0x04B1);
		Write_register(0x0095);
	}
    
}

static void setGammaBacklight(unsigned int index)
{
	LCM_DEBUG("setGammaBacklight  index=%d ARRAY_OF(gamma_table)=%d\n", index,ARRAY_OF(gamma_table));
    //FA
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x001A);
	Write_com(0x00BF);
	Write_register((gamma_table[index].gammaValue[0]<<8)|0xFA);
	Write_register((gamma_table[index].gammaValue[2]<<8)|(gamma_table[index].gammaValue[1]));
	Write_register((gamma_table[index].gammaValue[4]<<8)|(gamma_table[index].gammaValue[3]));
	Write_register((gamma_table[index].gammaValue[6]<<8)|(gamma_table[index].gammaValue[5]));
	Write_register((gamma_table[index].gammaValue[8]<<8)|(gamma_table[index].gammaValue[7]));
	Write_register((gamma_table[index].gammaValue[10]<<8)|(gamma_table[index].gammaValue[9]));
	Write_register((gamma_table[index].gammaValue[12]<<8)|(gamma_table[index].gammaValue[11]));
	Write_register((gamma_table[index].gammaValue[14]<<8)|(gamma_table[index].gammaValue[13]));
	Write_register((gamma_table[index].gammaValue[16]<<8)|(gamma_table[index].gammaValue[15]));
	Write_register((gamma_table[index].gammaValue[18]<<8)|(gamma_table[index].gammaValue[17]));
	Write_register((gamma_table[index].gammaValue[20]<<8)|(gamma_table[index].gammaValue[19]));
	Write_register((gamma_table[index].gammaValue[22]<<8)|(gamma_table[index].gammaValue[21]));
	Write_register((gamma_table[index].gammaValue[24]<<8)|(gamma_table[index].gammaValue[23]));
}

static void lcm_setbacklight(unsigned int level)   //back_light setting 
{

	int index;	
	
	index = level*(ARRAY_OF(gamma_table)-1)/255;  //yangqingbing modify for LCM backlight 20120725,CR:CR00657389

	if(g_last_Backlight_level == index)
	{
		return;
	}
	
	#ifndef BUILD_UBOOT	
	esd_thread_timeout = 0;
	mutex_lock(&setbacklight_flag);
	#endif
	if(level == 0)
	{
		//display off
		Write_com(0x00B7);
		Write_register(0x034B);
		Write_com(0x00B8);
		Write_register(0x0000);
		Write_com(0x00BC);
		Write_register(0x0001);
		Write_com(0x00BF);
		Write_register(0x0028);
	}
	else
	{
		Write_com(0x00B6);
		Write_register(0x000B);
		setGammaBacklight(index);
		setDynamicElvss(index);

		Write_com(0x00B7);
		Write_register(0x034B);
		Write_com(0x00B8);
		Write_register(0x0000);
		Write_com(0x00BC);
		Write_register(0x0002);
		Write_com(0x00BF);
		Write_register(0x03F7);

	    if(!g_last_Backlight_level)	
	    {
			MDELAY(120);	//gionee yangb add for CR00651546,2012.8.2
			 //0x29  display on
			Write_com(0x00B7);
			Write_register(0x034B);
			Write_com(0x00B8);
			Write_register(0x0000);
			Write_com(0x00BC);
			Write_register(0x0001);
			Write_com(0x00BF);
			Write_register(0x0029);
			LCM_DEBUG("lcd-backlight  display on\n");
	    }
		#ifndef BUILD_UBOOT	
		mod_timer(&esd_timer,jiffies + msecs_to_jiffies(2000));
		#endif

    }
	#ifndef BUILD_UBOOT	
	mutex_unlock(&setbacklight_flag);
	#endif
	g_last_Backlight_level = index;
    LCM_DEBUG("lcd-backlight  level=%d, index=%d\n", level,index);
}


#ifndef BUILD_UBOOT	

static __inline void spi_read_register(unsigned int data)
{
	unsigned int out = ((HX_RD_REGISTER<<16) | (data & 0xFFFF));
    spi_send_data(out);	
}


static unsigned int read_lcm_register(unsigned char reg)
{
	int read_count=0,temp=0;
#define   readbridge_retry_count   3
#define   readbridge_timeout       0xFFFF
	unsigned short int reg_write=reg;
	mutex_lock(&setbacklight_flag);
	if(read_flag)
	{
		MDELAY(30);
		Write_com(0x00B6);
		Write_register(0x008B);
	}
	else
	{
		Write_com(0x00B6);
		Write_register(0x000B);
		read_flag=1;
	}
	Write_com(0x00B7);
	Write_register(0x03CB);

	Write_com(0x00C4);
	Write_register(0x0001);
	
	Write_com(0x00C1);
	Write_register(0x0001);

	Write_com(0x00BC);
	Write_register(0x0001);

	Write_com(0x00BF);
	Write_register(reg_write);

	do_gettimeofday(&start);
	Write_com(0x00C6);
	spi_read_register(0x0000);
	temp=Read_register();
	while(!(temp&0x1))
	{
		MDELAY(50);
		Write_com(0x00C6);
		temp=Read_register();
		read_count++;
		if(read_count>readbridge_retry_count)
		{
			mutex_unlock(&setbacklight_flag);
			return readbridge_timeout;
		}
	}
	do_gettimeofday(&stop);
	diff.tv_usec=stop.tv_usec-start.tv_usec;
	LCM_DEBUG("LCM read %02x waste %d\n",reg_write,diff.tv_usec);
	
	Write_com(0x00FF);
	spi_read_register(0x0000);
	temp=Read_register();
	LCM_DEBUG("Reg %02x is %02x\n",reg_write,temp);
	mutex_unlock(&setbacklight_flag);
	return temp;
}

static unsigned int lcm_esd_check(void)
{
	int temp=0;
	LCM_DEBUG("start :%s 1\n",__func__);

	temp = read_lcm_register(0x05);
	LCM_DEBUG("lcm_esd_check :temp =%x \n",temp);
	if(temp)
	{
		read_flag =0;
		return TRUE;
	}
	return FALSE;
}
static unsigned int lcm_esd_recover(void)
{

	LCM_DEBUG("start :%s\n",__func__);

	lcm_init();
	
	setGammaBacklight(g_last_Backlight_level);
	setDynamicElvss(g_last_Backlight_level);

	//0xF7
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0002);
	Write_com(0x00BF);
	Write_register(0x03F7);
	MDELAY(120);
	
	//0x29  display on
	Write_com(0x00B7);
	Write_register(0x034b);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0001);
	Write_com(0x00BF);
	Write_register(0x0029);
	return TRUE;

}
#endif

#ifndef BUILD_UBOOT

#ifdef GN_MTK_BSP_LCM_DEBUG   //add by chenqiang for lcd_debug
int lcd_f9_register_write(const char * buf)
{
    int n_idx;
    int max_nu=0;
    int max_np=0;
    int max_no=0;
    int num_n=0;
    int num_q=0;
    const char *p = buf ; 
    char qq[2];
    char num[25];
    while( (*p++ != '{' )&&( max_nu++ <= 200));    
    {
        if(max_nu>200)       
			return -1;
    }
    for(num_n=0;num_n<=24;num_n++)
	{       
		for(num_q=0;num_q<=1;num_q++)
		{
			max_np=0;
			while( (*(p) == 32)&&(max_np++ <= 20) )
			{
				p++;
				if(max_np>20)      
					return -1;
			}
			if( (*p>='0') && (*p <='9'))
				qq[num_q]=(*p++ -'0');
			else if( (*p>='A') && (*p <='F'))
				qq[num_q]=(*p++ -'A' + 10);
			else if( (*p>='a') && (*p <='f'))
				qq[num_q]=(*p++ -'a' + 10);
			else
			 	return -1;
		}
		num[num_n]=qq[0]*16 + qq[1];
	}
    while( (*p++!= '}' )&&( max_no++ <= 20));    
    {
        if(max_no>20)     
			return -1;
    }
	for(num_n =0;num_n<=24;num_n++)
	{
		LCM_DEBUG("register lcd_f9_register_write num[%d]=%d\n", num_n,num[num_n]);
	}
   //0xF7
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0002);
	Write_com(0x00BF);
	Write_register(0x00F7);

    //FA
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x001A);
	Write_com(0x00BF);
	Write_register((num[0]<<8)|0xFA);
	Write_register((num[2]<<8)|(num[1]));
	Write_register((num[4]<<8)|(num[3]));
	Write_register((num[6]<<8)|(num[5]));
	Write_register((num[8]<<8)|(num[7]));
	Write_register((num[10]<<8)|(num[9]));
	Write_register((num[12]<<8)|(num[11]));
	Write_register((num[14]<<8)|(num[13]));
	Write_register((num[16]<<8)|(num[15]));
	Write_register((num[18]<<8)|(num[17]));
	Write_register((num[20]<<8)|(num[19]));
	Write_register((num[22]<<8)|(num[21]));
	Write_register((num[24]<<8)|(num[23]));
	
    //0xF7
	Write_com(0x00B7);
	Write_register(0x034B);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0002);
	Write_com(0x00BF);
	Write_register(0x03F7);
		
	//0x29  display on
	Write_com(0x00B7);
	Write_register(0x034b);
	Write_com(0x00B8);
	Write_register(0x0000);
	Write_com(0x00BC);
	Write_register(0x0001);
	Write_com(0x00BF);
	Write_register(0x0029);
    return 0;
}
#endif
#endif

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER gn_ssd2825_smd_s6e8aa = 
{
	.name			= "gn_ssd2825_smd_s6e8aa",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	.set_backlight  = lcm_setbacklight,
#ifndef BUILD_UBOOT	
	.esd_check      = lcm_esd_check,
	.esd_recover    = lcm_esd_recover,
#endif	
};

