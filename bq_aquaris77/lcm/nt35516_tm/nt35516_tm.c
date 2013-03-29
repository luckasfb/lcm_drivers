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

//zhaoshaopeng add fn00410 lcd_id det
#ifdef BUILD_UBOOT
#include <asm/arch/mt6577_gpio.h>
#else
#include <mach/mt6577_gpio.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(540)
#define FRAME_HEIGHT 										(960)
#define LCM_ID       (0x69)
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xfd  // END OF REGISTERS MARKER


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
#define LCM_DSI_CMD_MODE									1

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

#if 0
static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/
	/*
	{0xff,	5,	{0xaa, 0x55, 0x25, 0x01, 0x01}},
	{0xf2, 35, {0x00,
0x00,
0x4A,
0x0A,
0xA8,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x0B,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x40,
0x01,
0x51,
0x00,
0x01,
0x00,
0x01
}},
	{0xf3,	7,	{0x02,
0x03,
0x07,
0x45,
0x88,
0xD1,
0x0D
}},
*/
//*************************************
// Select CMD2,Page 0
//*************************************
	{0xf0,	5,	{0x55, 0xaa, 0x52, 0x08, 0x00}},
       {0xb0,	5,	{0x00, 0x0c, 0x40, 0x3c, 0x3c}},	

       {0x4c,	1,	{0x10}},
       {0xb1,	3,	{0x71, 0x06, 0x00}},	    
       {0xb6,	1,	{0x00}},
       {0xb7,	2,	{0xff,0x55}},

	
	{0xbc,	2,	{0x00, 0x00}},
	
       //{0xb8,	4,	{0x01, 0x02, 0x02, 0x02}},
       {0xc9,	6,	{0x41, 0x06, 0x0d, 0x3a, 0x17, 0x00}},
	{REGFLAG_DELAY, 10, {}},
//*************************************
// Select CMD2,Page 1
//*************************************
       {0xf0,	5,	{0x55, 0xaa, 0x52, 0x08, 0x01}},
	{REGFLAG_DELAY, 10, {}}, 
// AVDD: 6.0V	
       //{0xb0,	3,	{0x05, 0x05, 0x05}},
// AVEE: -6.0V
       //{0xb1,	3,	{0x05, 0x05, 0x05}},	
       //{0xb2,	3,	{0x01, 0x01, 0x01}},	
       //{0xb3,	3,	{0x0e, 0x0e, 0x0e}},	       
       //{0xb4,	3,	{0x08, 0x08, 0x08}},	       

	//{0xb6,	3,	{0x44, 0x44, 0x44}},	

	//{0xb7,	3,	{0x34, 0x34, 0x34}},
	//{0xb8,	3,	{0x10, 0x10, 0x10}},	
	//{0xb9,	3,	{0x26, 0x26, 0x26}},	
	
	//{0xba,	3,	{0x34, 0x34, 0x34}},

	//{0xbc,	3,	{0x00, 0xc8, 0x00}},

	//{0xbd,	3,	{0x00, 0xc8, 0x00}},
// VCOM
       //{0xbe,	1,	{0x73}},
       //{0xc0,	2,	{0x04, 0x00}},
       //{0xca,	1,	{0x00}},
	//{0xd0,	4,	{0x0a, 0x10, 0x0d, 0x0f}},	
       
//*************************************
// Gamma Code
//*************************************
// Positive Red Gamma
       {0xd1,	16,	{0x00,
0x00,
0x00,
0x29,
0x00,
0x62,
0x00,
0x86,
0x00,
0xA7,
0x00,
0xD3,
0x00,
0xF3,
0x01,
0x27
}},
       {0xd2,	16,	{0x01,
0x52,
0x01,
0x91,
0x01,
0xC1,
0x02,
0x12,
0x02,
0x4D,
0x02,
0x4F,
0x02,
0x86,
0x02,
0xC5
}},
       {0xd3,	16,	{0x02,
0xE4,
0x03,
0x14,
0x03,
0x2E,
0x03,
0x58,
0x03,
0x6E,
0x03,
0x93,
0x03,
0xA7,
0x03,
0xC3
}},
	{0xd4,	4,	{0x03, 0xf8, 0x03, 0xf8}},
// Positive Green Gamma
       {0xd5,	16,	{0x00,
0x00,
0x00,
0x29,
0x00,
0x62,
0x00,
0x86,
0x00,
0xA7,
0x00,
0xD3,
0x00,
0xF3,
0x01,
0x27
}},
       {0xd6,	16,	{0x01,
0x52,
0x01,
0x91,
0x01,
0xC1,
0x02,
0x12,
0x02,
0x4D,
0x02,
0x4F,
0x02,
0x86,
0x02,
0xC5
}},
       {0xd7,	16,	{0x02,
0xE4,
0x03,
0x14,
0x03,
0x2E,
0x03,
0x58,
0x03,
0x6E,
0x03,
0x93,
0x03,
0xA7,
0x03,
0xC3
}},
	{0xd8,	4,	{0x03, 0xf8, 0x03, 0xf8}},
// Positive Blue Gamma
       {0xd9,	16,	{0x00,
0x00,
0x00,
0x29,
0x00,
0x62,
0x00,
0x86,
0x00,
0xA7,
0x00,
0xD3,
0x00,
0xF3,
0x01,
0x27
}},
       {0xdd,	16,	{0x01,
0x52,
0x01,
0x91,
0x01,
0xC1,
0x02,
0x12,
0x02,
0x4D,
0x02,
0x4F,
0x02,
0x86,
0x02,
0xC5
}},
       {0xde,	16,	{0x02,
0xE4,
0x03,
0x14,
0x03,
0x2E,
0x03,
0x58,
0x03,
0x6E,
0x03,
0x93,
0x03,
0xA7,
0x03,
0xC3
}},
	{0xdf,	4,	{0x03,0xf8,0x03,0xf8}},
// Negative Red Gamma	
       {0xe0,	16,	{0x00,
0x00,
0x00,
0x29,
0x00,
0x62,
0x00,
0x86,
0x00,
0xA7,
0x00,
0xD3,
0x00,
0xF3,
0x01,
0x27
}},
       {0xe1,	16,	{0x01,
0x52,
0x01,
0x91,
0x01,
0xC1,
0x02,
0x12,
0x02,
0x4D,
0x02,
0x4F,
0x02,
0x86,
0x02,
0xC5
}},
       {0xe2,	16,	{0x02,
0xE4,
0x03,
0x14,
0x03,
0x2E,
0x03,
0x58,
0x03,
0x6E,
0x03,
0x93,
0x03,
0xA7,
0x03,
0xC3
}},
	{0xe3,	4,	{0x03,0xf8,0x03,0xf8}},
// Negative Green Gamma	
       {0xe4,	16,	{0x00,
0x00,
0x00,
0x29,
0x00,
0x62,
0x00,
0x86,
0x00,
0xA7,
0x00,
0xD3,
0x00,
0xF3,
0x01,
0x27
}},
       {0xe5,	16,	{0x01,
0x52,
0x01,
0x91,
0x01,
0xC1,
0x02,
0x12,
0x02,
0x4D,
0x02,
0x4F,
0x02,
0x86,
0x02,
0xC5
}},
       {0xe6,	16,	{0x02,
0xE4,
0x03,
0x14,
0x03,
0x2E,
0x03,
0x58,
0x03,
0x6E,
0x03,
0x93,
0x03,
0xA7,
0x03,
0xC3
}},
	{0xe7,	4,	{0x03,0xf8,0x03,0xf8}},	
// Negative Blue Gamma	
       {0xe8,	16,	{0x00,
0x00,
0x00,
0x29,
0x00,
0x62,
0x00,
0x86,
0x00,
0xA7,
0x00,
0xD3,
0x00,
0xF3,
0x01,
0x27
}},
       {0xe9,	16,	{0x01,
0x52,
0x01,
0x91,
0x01,
0xC1,
0x02,
0x12,
0x02,
0x4D,
0x02,
0x4F,
0x02,
0x86,
0x02,
0xC5
}},
       {0xea,	16,	{0x02,
0xE4,
0x03,
0x14,
0x03,
0x2E,
0x03,
0x58,
0x03,
0x6E,
0x03,
0x93,
0x03,
0xA7,
0x03,
0xC3
}},

	{0xeb,	4,	{0x03,0xf8,0x03,0xf8}},
       {0xb0,	1,	{0x0a}},
       {0xb6,	1,	{0x34}},
       {0xb1,	1,	{0x0a}},
       {0xb7,	1,	{0x24}},
       {0xb2,	1,	{0x00}},
       {0xb8,	1,	{0x30}},
       {0xb3,	1,	{0x0a}},

       {0xb9,	1,	{0x24}},
       {0xb4,	1,	{0x0a}},
       {0xba,	1,	{0x34}},
       


       {0xbc,	3,	{0x00,0x78,0x00}},
       {0xbd,	3,	{0x00,0x78,0x00}},
       {0xbe,	1,	{0x44}},
       {0xc2,	1,	{0x00}},
	{0x35,	1,	{0x00}},
       {0x36,	1,	{0xc0}},//zhaoshaopeng add for tm
//*************************************
    // Sleep Out
//*************************************
	{0x11, 0, {}},
       {REGFLAG_DELAY, 200, {}},
//*************************************
// Display On
//*************************************
	{0x29, 0, {}},
	{REGFLAG_DELAY, 200, {}},

	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
       {REGFLAG_DELAY, 200, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 200, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {

	// Display off sequence
	{0x28, 1, {0x00}},
       {REGFLAG_DELAY, 200, {}},
    // Sleep Mode On
	{0x10, 1, {0x00}},
       {REGFLAG_DELAY, 200, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {
	// Display off sequence
	{0xB9,	3,	{0xFF, 0x83, 0x69}},
	{REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
//	{0xC3, 1, {0xFF}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#else
static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/
	/*
	{0xff,	5,	{0xaa, 0x55, 0x25, 0x01, 0x01}},
	{0xf2, 35, {0x00,
0x00,
0x4A,
0x0A,
0xA8,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x0B,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x40,
0x01,
0x51,
0x00,
0x01,
0x00,
0x01
}},
	{0xf3,	7,	{0x02,
0x03,
0x07,
0x45,
0x88,
0xD1,
0x0D
}},
*/
//*************************************
// Select CMD2,Page 0
//*************************************
       {0xf0,	5,	{0x55, 0xaa, 0x52, 0x08, 0x00}},

       {0x4c,	1,	{0x10}},
       {0xb1,	3,	{0x71, 0x00, 0x00}},	    
       {0xb6,	1,	{0x00}},
       {0xb7,	2,	{0xff,0x55}},

	
	{0xbc,	3,	{0x00,0x00,0x00}},
	
       //{0xb8,	4,	{0x01, 0x02, 0x02, 0x02}},
       {0xc9,	6,	{0x41, 0x06, 0x0d, 0x3a, 0x17, 0x00}},
	{REGFLAG_DELAY, 10, {}},
//*************************************
// Select CMD2,Page 1
//*************************************
       {0xf0,	5,	{0x55, 0xaa, 0x52, 0x08, 0x01}},
	{REGFLAG_DELAY, 10, {}}, 
//add for different gamma	
       {0xd0,	4,	{0x0a,0x10,0x0d, 0x0f}},
       
//*************************************
// Gamma Code
//*************************************
// Positive Red Gamma
       {0xd1,	16,	{0x00,
0x00,
0x00,
0x60,
0x00,
0xbb,
0x00,
0xf8,
0x01,
0x1a,
0x01,
0x43,
0x01,
0x61,
0x01,
0x91
}},
       {0xd2,	16,	{0x01,
0xb5,
0x01,
0xee,
0x02,
0x1a,
0x02,
0x5e,
0x02,
0x94,
0x02,
0x95,
0x02,
0xc5,
0x02,
0xf9
}},
       {0xd3,	16,	{0x03,
0x19,
0x03,
0x44,
0x03,
0x5f,
0x03,
0x81,
0x03,
0x95,
0x03,
0xAf,
0x03,
0xba,
0x03,
0xc5
}},
	{0xd4,	4,	{0x03,0xd0,0x03,0xd8}},
// Positive Green Gamma
       {0xd5,	16,	{0x00,
0x00,
0x00,
0x60,
0x00,
0xbb,
0x00,
0xf8,
0x01,
0x1a,
0x01,
0x43,
0x01,
0x61,
0x01,
0x91
}},
       {0xd6,	16,	{0x01,
0xb5,
0x01,
0xee,
0x02,
0x1a,
0x02,
0x5e,
0x02,
0x94,
0x02,
0x95,
0x02,
0xc5,
0x02,
0xf9
}},
       {0xd7,	16,	{0x03,
0x19,
0x03,
0x44,
0x03,
0x5f,
0x03,
0x81,
0x03,
0x95,
0x03,
0xAf,
0x03,
0xba,
0x03,
0xc5
}},
	{0xd8,	4,	{0x03,0xd0,0x03,0xd8}},
// Positive Blue Gamma
       {0xd9,	16,	{0x00,
0x00,
0x00,
0x60,
0x00,
0xbb,
0x00,
0xf8,
0x01,
0x1a,
0x01,
0x43,
0x01,
0x61,
0x01,
0x91
}},
       {0xdd,	16,	{0x01,
0xb5,
0x01,
0xee,
0x02,
0x1a,
0x02,
0x5e,
0x02,
0x94,
0x02,
0x95,
0x02,
0xc5,
0x02,
0xf9
}},
       {0xde,	16,	{0x03,
0x19,
0x03,
0x44,
0x03,
0x5f,
0x03,
0x81,
0x03,
0x95,
0x03,
0xAf,
0x03,
0xba,
0x03,
0xc5
}},
	{0xdf,	4,	{0x03,0xd0,0x03,0xd8}},
// Negative Red Gamma	
       {0xe0,	16,	{0x00,
0x00,
0x00,
0x60,
0x00,
0xbb,
0x00,
0xf8,
0x01,
0x1a,
0x01,
0x43,
0x01,
0x61,
0x01,
0x91
}},
       {0xe1,	16,	{0x01,
0xb5,
0x01,
0xee,
0x02,
0x1a,
0x02,
0x5e,
0x02,
0x94,
0x02,
0x95,
0x02,
0xc5,
0x02,
0xf9
}},
       {0xe2,	16,	{0x03,
0x19,
0x03,
0x44,
0x03,
0x5f,
0x03,
0x81,
0x03,
0x95,
0x03,
0xAf,
0x03,
0xba,
0x03,
0xc5
}},
	{0xe3,	4,	{0x03,0xd0,0x03,0xd8}},
// Negative Green Gamma	
       {0xe4,	16,	{0x00,
0x00,
0x00,
0x60,
0x00,
0xbb,
0x00,
0xf8,
0x01,
0x1a,
0x01,
0x43,
0x01,
0x61,
0x01,
0x91
}},
       {0xe5,	16,	{0x01,
0xb5,
0x01,
0xee,
0x02,
0x1a,
0x02,
0x5e,
0x02,
0x94,
0x02,
0x95,
0x02,
0xc5,
0x02,
0xf9
}},
       {0xe6,	16,	{0x03,
0x19,
0x03,
0x44,
0x03,
0x5f,
0x03,
0x81,
0x03,
0x95,
0x03,
0xAf,
0x03,
0xba,
0x03,
0xc5
}},
	{0xe7,	4,	{0x03,0xd0,0x03,0xd8}},	
// Negative Blue Gamma	
       {0xe8,	16,	{0x00,
0x00,
0x00,
0x60,
0x00,
0xbb,
0x00,
0xf8,
0x01,
0x1a,
0x01,
0x43,
0x01,
0x61,
0x01,
0x91
}},
       {0xe9,	16,	{0x01,
0xb5,
0x01,
0xee,
0x02,
0x1a,
0x02,
0x5e,
0x02,
0x94,
0x02,
0x95,
0x02,
0xc5,
0x02,
0xf9
}},
       {0xea,	16,	{0x03,
0x19,
0x03,
0x44,
0x03,
0x5f,
0x03,
0x81,
0x03,
0x95,
0x03,
0xAf,
0x03,
0xba,
0x03,
0xc5
}},

       {0xeb,	4,	{0x03,0xd0,0x03,0xd8}},
//power setting
       {0xb0,	1,	{0x0a}},
       {0xb6,	1,	{0x34}},
       {0xb1,	1,	{0x0a}},
       {0xb7,	1,	{0x24}},
       {0xb2,	1,	{0x00}},
       {0xb8,	1,	{0x30}},
       {0xb3,	1,	{0x0a}},

       {0xb9,	1,	{0x24}},
       {0xb4,	1,	{0x0a}},
       {0xba,	1,	{0x34}},
       


       {0xbc,	3,	{0x00,0x78,0x00}},
       {0xbd,	3,	{0x00,0x78,0x00}},
       {0xbe,	1,	{0x38}}, //4C
// set PFM for charge bump
       {0xc2,	1,	{0x00}},
       {0x3a,	1,	{0x77}},       
	// ENABLE FMARK
	{0x44,	2,	{((FRAME_HEIGHT/2)>>8), ((FRAME_HEIGHT/2)&0xFF)}},
	{0x35,	1,	{0x00}},
	
//*************************************
    // Sleep Out
//*************************************
	{0x11, 0, {}},
       {REGFLAG_DELAY, 200, {}},
//*************************************
// Display On
//*************************************
	{0x29, 0, {}},
	{REGFLAG_DELAY, 200, {}},

	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {0x00}},
       {REGFLAG_DELAY, 125, {}},

    // Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {

	// Display off sequence
	{0x28, 0, {0x00}},
       {REGFLAG_DELAY, 200, {}},
    // Sleep Mode On
	{0x10, 0, {0x00}},
       {REGFLAG_DELAY, 200, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {
{0xf0,	5,	{0x55, 0xaa, 0x52, 0x08, 0x01}},
	{REGFLAG_DELAY, 10, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
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
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=480*3;	

		params->dsi.vertical_sync_active=3;
		params->dsi.vertical_backporch=12;
		params->dsi.vertical_frontporch=2;
		params->dsi.vertical_active_line=800;
	
		params->dsi.line_byte=2048;		// 2256 = 752*3
		params->dsi.horizontal_sync_active_byte=26;
		params->dsi.horizontal_backporch_byte=146;
		params->dsi.horizontal_frontporch_byte=146;	
		params->dsi.rgb_byte=(480*3+6);	
	
		params->dsi.horizontal_sync_active_word_count=20;	
		params->dsi.horizontal_backporch_word_count=140;
		params->dsi.horizontal_frontporch_word_count=140;

		// Bit rate calculation
		params->dsi.pll_div1=38;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		params->dsi.pll_div2=1;			// div2=0~15: fout=fvo/(2*div2)

}


static void lcm_init(void)
{

    unsigned char lcd_id =0;
    SET_RESET_PIN(1);
    MDELAY(10);	
    SET_RESET_PIN(0);
    MDELAY(150);
    SET_RESET_PIN(1);
    MDELAY(10);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_resume(void)
{
	//lcm_init();
	
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}


static void lcm_setbacklight(unsigned int level)
{
	unsigned int default_level = 145;
	unsigned int mapped_level = 0;

	//for LGE backlight IC mapping table
	if(level > 255) 
			level = 255;

	if(level >0) 
			mapped_level = default_level+(level)*(255-default_level)/(255);
	else
			mapped_level=0;

	// Refresh value of backlight level.
	lcm_backlight_level_setting[0].para_list[0] = mapped_level;

	push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);
}

static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_UBOOT
        if(lcm_esd_test)
        {
            lcm_esd_test = FALSE;
            return TRUE;
        }

        /// please notice: the max return packet size is 1
        /// if you want to change it, you can refer to the following marked code
        /// but read_reg currently only support read no more than 4 bytes....
        /// if you need to read more, please let BinHan knows.
        /*
                unsigned int data_array[16];
                unsigned int max_return_size = 1;
                
                data_array[0]= 0x00003700 | (max_return_size << 16);    
                
                dsi_set_cmdq(&data_array, 1, 1);
        */

        if(read_reg(0xB6) == 0x42)
        {
            return FALSE;
        }
        else
        {            
            return TRUE;
        }
#endif
}

static unsigned int lcm_esd_recover(void)
{
    unsigned char para = 0;

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);
    SET_RESET_PIN(1);
    MDELAY(120);
	  push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
	  push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
    dsi_set_cmdq_V2(0x35, 1, &para, 1);     ///enable TE
    MDELAY(10);

    return TRUE;
}

static unsigned int lcm_compare_id(void)
{
	volatile unsigned int id = 0;
	volatile unsigned char buffer[3];
	unsigned int array[16];
	//zhaoshaopeng for tianma/baolongda lcd_id_det  hx8369 
	volatile unsigned char lcd_id = 0;
        SET_RESET_PIN(1);  //NOTE:should reset LCM firstly
       MDELAY(10);
    	SET_RESET_PIN(0);
    	MDELAY(150);
    	SET_RESET_PIN(1);
    	MDELAY(10);
       //lcm_init();
	push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);

	array[0] = 0x00023700;
	dsi_set_cmdq(array, 1, 1);
//	id = read_reg(0xF4);
	//read_reg_v2(0xF4, buffer, 2);
	//id = buffer[0]; //we only need ID
	//zhaoshaopeng for tianma/baolongda lcd_id_det  hx8369 
	lcd_id =  mt_get_gpio_in(GPIO_LCD_ID);
	//id1 = read_reg(0xc5);
	//id2 = read_reg(0xc5);
	//id3 = read_reg(0xc5);
	read_reg_v2(0xc5, buffer, 3);	  
#if defined(BUILD_UBOOT)
       printf("%s, zhaoshaopeng buffer[0] = 0x%08x, buffer[1] = 0x%08x, id= 0x%08x, lcd_id=%d\n", __func__, buffer[0], buffer[1], id, lcd_id);
#endif
        if((buffer[0] ==0x55)&&(lcd_id==1)&&(buffer[1] ==0x16))
        return 1;
        else
        return 0;
}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER nt35516_tm_lcm_drv = 
{
    .name			= "nt35516_tm",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
	//.set_backlight	= lcm_setbacklight,
//	.set_pwm        = lcm_setpwm,
//	.get_pwm        = lcm_getpwm,
	//.esd_check   = lcm_esd_check,
        //.esd_recover   = lcm_esd_recover,
	.compare_id    = lcm_compare_id,
#endif
};
