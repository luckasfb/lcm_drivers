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

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (240) // (480)
#define FRAME_HEIGHT (320) // (800)
#define LCM_ID       (0x1526)

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
    return lcm_util.read_data();
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
	
	send_ctrl_cmd(0x2A);
	send_data_cmd(HIGH_BYTE(x0));
	send_data_cmd(LOW_BYTE(x0));
	send_data_cmd(HIGH_BYTE(x1));
	send_data_cmd(LOW_BYTE(x1));

	send_ctrl_cmd(0x2B);
	send_data_cmd(HIGH_BYTE(y0));
	send_data_cmd(LOW_BYTE(y0));
	send_data_cmd(HIGH_BYTE(y1));
	send_data_cmd(LOW_BYTE(y1));
	send_ctrl_cmd(0x2C);
	
	for (i = x0; i < x1; i++)
	{
		for (k = y0; k < y1; k++)
		{
			send_data_cmd(HIGH_BYTE(data));
			send_data_cmd(LOW_BYTE(data));
		}
	}
}

static void init_lcm_registers(void)
{
    // r61526  
    #if 1 // codes given by GuangTai Co. 20120528

	send_ctrl_cmd(0xB0);
	send_data_cmd(0x3F);
	send_data_cmd(0x3F);
	MDELAY(5);

	send_ctrl_cmd(0xB3);
	send_data_cmd(0x02);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xB4);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC0);
	send_data_cmd(0x33);
	send_data_cmd(0x4F);
	send_data_cmd(0x00);
	send_data_cmd(0x10);
	send_data_cmd(0xA2);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC1);
	send_data_cmd(0x01);
	send_data_cmd(0x02);
	send_data_cmd(0x16);
	send_data_cmd(0x04);
	send_data_cmd(0x04);

	MDELAY(25);

	send_ctrl_cmd(0xC3);
	send_data_cmd(0x01);
	send_data_cmd(0x02);
	send_data_cmd(0x16);
	send_data_cmd(0x04);
	send_data_cmd(0x04);

	MDELAY(25);

	send_ctrl_cmd(0xC4);
	send_data_cmd(0x11);
	send_data_cmd(0x01);
	send_data_cmd(0x43);
	send_data_cmd(0x04);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC8);//GAMMA
	send_data_cmd(0x02);
	send_data_cmd(0x08);
	send_data_cmd(0x0B);
	send_data_cmd(0x12);
	send_data_cmd(0x10);
	send_data_cmd(0x09);
	send_data_cmd(0x04);
	send_data_cmd(0x07);
	send_data_cmd(0x0E);
	send_data_cmd(0x12);
	send_data_cmd(0x00);

	send_data_cmd(0x0E);
	send_data_cmd(0x07);
	send_data_cmd(0x04);
	send_data_cmd(0x09);
	send_data_cmd(0x10);
	send_data_cmd(0x12);
	send_data_cmd(0x0B);
	send_data_cmd(0x08);
	send_data_cmd(0x02);
	send_data_cmd(0x14);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC9);//GAMMA
	send_data_cmd(0x02);
	send_data_cmd(0x08);
	send_data_cmd(0x0B);
	send_data_cmd(0x12);
	send_data_cmd(0x10);
	send_data_cmd(0x09);
	send_data_cmd(0x04);
	send_data_cmd(0x07);
	send_data_cmd(0x0E);
	send_data_cmd(0x12);
	send_data_cmd(0x00);

	send_data_cmd(0x0E);
	send_data_cmd(0x07);
	send_data_cmd(0x04);
	send_data_cmd(0x09);
	send_data_cmd(0x10);
	send_data_cmd(0x12);
	send_data_cmd(0x0B);
	send_data_cmd(0x08);
	send_data_cmd(0x02);
	send_data_cmd(0x14);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xCA);//GAMMA
	send_data_cmd(0x02);
	send_data_cmd(0x08);
	send_data_cmd(0x0B);
	send_data_cmd(0x12);
	send_data_cmd(0x10);
	send_data_cmd(0x09);
	send_data_cmd(0x04);
	send_data_cmd(0x07);
	send_data_cmd(0x0E);
	send_data_cmd(0x12);
	send_data_cmd(0x00);

	send_data_cmd(0x0E);
	send_data_cmd(0x07);
	send_data_cmd(0x04);
	send_data_cmd(0x09);
	send_data_cmd(0x10);
	send_data_cmd(0x12);
	send_data_cmd(0x0B);
	send_data_cmd(0x08);
	send_data_cmd(0x02);
	send_data_cmd(0x14);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xD0);
	send_data_cmd(0x63);//BT
	send_data_cmd(0x53);
	send_data_cmd(0x82);//VC2
	send_data_cmd(0x37);//VRH
	send_data_cmd(0x30);//DCT
	send_data_cmd(0x00);

	send_ctrl_cmd(0xD1);
	send_data_cmd(0x48);
	send_data_cmd(0x5F);
	send_data_cmd(0x10);

	send_ctrl_cmd(0xD2);
	send_data_cmd(0x03);
	send_data_cmd(0x24);

	send_ctrl_cmd(0xD4);
	send_data_cmd(0x03);
	send_data_cmd(0x24);

	send_ctrl_cmd(0xE2);
	send_data_cmd(0x3F);

	send_ctrl_cmd(0x35);
	send_data_cmd(0x00);

	send_ctrl_cmd(0x36);
	send_data_cmd(0xC0);

	send_ctrl_cmd(0x3A); //16/18 bit
	send_data_cmd(0x55);//66->18 bit, 55->16bit.

	send_ctrl_cmd(0x2A);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0xEF);

	send_ctrl_cmd(0x2B);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x3F);

	send_ctrl_cmd(0x11);
	MDELAY(150);

	send_ctrl_cmd(0x29);
	MDELAY(30);

	send_ctrl_cmd(0x2C);
	#else
	
	send_ctrl_cmd(0x00B0); //Manufacturer Command Access Protect
	send_data_cmd(0x003F);
	send_data_cmd(0x003F);
	MDELAY(5);

	send_ctrl_cmd(0x00B3); //Frame Memory Access and Interface Setting
	send_data_cmd(0x0002);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);

	send_ctrl_cmd(0x00B4);//Display Mode and Frame Memory Write Mode Setting
	send_data_cmd(0x0000);

	send_ctrl_cmd(0x00C0); //Panel Driving Setting
	send_data_cmd(0x0033); //0 0 1 REV, SM GS BGR SS
	send_data_cmd(0x004F); //0 NL[6:0]
	send_data_cmd(0x0000); //0 SCN[6:0]
	send_data_cmd(0x0010); //
	send_data_cmd(0x00A0); ///  //1 0 1 0, 0 0 BLV PTV	  //0:LINE.1:FRAME
	send_data_cmd(0x0000); //0 0 BLS NDL, PTDC PTS 0 0
	send_data_cmd(0x0001); //0 0 0 PTG,ISC[3:0]
	send_data_cmd(0x0000); //

	send_ctrl_cmd(0x00C1); //Display Timing Setting for Normal / Partial Mode
	send_data_cmd(0x0001); //0 0 0 0, 0 0 0 BC0
	send_data_cmd(0x0002); //0 0 0 0, 0 0	DIV0[1:0]
	send_data_cmd(0x0018); //0 0 RTN0[5:0] //These bits set 1 line period//15-3F
	send_data_cmd(0x0004); //BP0[7:0]
	send_data_cmd(0x0004); //FP0[7:0]
	MDELAY(25);

	send_ctrl_cmd(0x00C3); //Display Timing Setting for Idle Mode
	send_data_cmd(0x0001); //0 0 0 0, 0 0 0 BC2
	send_data_cmd(0x0002); //0 0 0 0, 0 0	DIV2[1:0]
	send_data_cmd(0x0018); //0 0 RTN2[5:0]
	send_data_cmd(0x0004); //BP2[7:0]
	send_data_cmd(0x0004); //FP2[7:0]
	MDELAY(25);

	send_ctrl_cmd(0x00C4); //Source/VCOM/Gate Driving Timing Setting
	send_data_cmd(0x0033); //0 SDT[2:0],0 NOW[2:0]  //source:0-7,gate:0-7
	send_data_cmd(0x0003); //0 0 0 0, 0 MCP[2:0]	  //vcom:0-7
	send_data_cmd(0x0023); //VEQW[3:0],0 0 VEM[1:0] //VCOM equalize period:0-15
	send_data_cmd(0x0004); //0 0 0 0,SPCW[3:0]   //
	send_data_cmd(0x0000); //0 0 0 0 0 0 0 0

	//Gamma setting start
	send_ctrl_cmd(0x00C8);//Gamma
	send_data_cmd(0x0007);
	send_data_cmd(0x0009);
	send_data_cmd(0x000A);
	send_data_cmd(0x000C);
	send_data_cmd(0x0018);
	send_data_cmd(0x000D);
	send_data_cmd(0x0005);
	send_data_cmd(0x0004);
	send_data_cmd(0x0007);
	send_data_cmd(0x0003);
	send_data_cmd(0x0000);
	send_data_cmd(0x0003);
	send_data_cmd(0x0007);
	send_data_cmd(0x0004);
	send_data_cmd(0x0005);
	send_data_cmd(0x000D);
	send_data_cmd(0x0018);
	send_data_cmd(0x000C);
	send_data_cmd(0x000A);
	send_data_cmd(0x0009);
	send_data_cmd(0x0007);
	send_data_cmd(0x0000);


	send_ctrl_cmd(0x00C9);//Gamma
	send_data_cmd(0x0007);
	send_data_cmd(0x0009);
	send_data_cmd(0x000A);
	send_data_cmd(0x000C);
	send_data_cmd(0x0018);
	send_data_cmd(0x000D);
	send_data_cmd(0x0005);
	send_data_cmd(0x0004);
	send_data_cmd(0x0007);
	send_data_cmd(0x0003);
	send_data_cmd(0x0000);
	send_data_cmd(0x0003);
	send_data_cmd(0x0007);
	send_data_cmd(0x0004);
	send_data_cmd(0x0005);
	send_data_cmd(0x000D);
	send_data_cmd(0x0018);
	send_data_cmd(0x000C);
	send_data_cmd(0x000A);
	send_data_cmd(0x0009);
	send_data_cmd(0x0007);
	send_data_cmd(0x0000);

	send_ctrl_cmd(0x00CA);//Gamma
	send_data_cmd(0x0007);
	send_data_cmd(0x0009);
	send_data_cmd(0x000A);
	send_data_cmd(0x000C);
	send_data_cmd(0x0018);
	send_data_cmd(0x000D);
	send_data_cmd(0x0005);
	send_data_cmd(0x0004);
	send_data_cmd(0x0007);
	send_data_cmd(0x0003);
	send_data_cmd(0x0000);
	send_data_cmd(0x0003);
	send_data_cmd(0x0007);
	send_data_cmd(0x0004);
	send_data_cmd(0x0005);
	send_data_cmd(0x000D);
	send_data_cmd(0x0018);
	send_data_cmd(0x000C);
	send_data_cmd(0x000A);
	send_data_cmd(0x0009);
	send_data_cmd(0x0007);
	//================================================= 
	//====Power setting start==========================
	//================================================= 
	send_ctrl_cmd(0x00D0);  //Power Setting (Common Setting)
	send_data_cmd(0x0033);  //0 BT[2:0],0 0 1 1	 //DDVDH VCL VGH VGL
	send_data_cmd(0x0053);  //0 1 0 1, 0 0 1 1
	send_data_cmd(0x0085); ///  //1 0 0 0, 0 VC2[2:0]	 //VCI2
	send_data_cmd(0x003D);  //0 0 VRH[5:0]		 //VREG voltage
	send_data_cmd(0x0030);  //0 0 1 1, 0 DCT[2:0] 
	send_data_cmd(0x0000);  //0 0 0 0, 0 0 0 0

	send_ctrl_cmd(0x00D1); //VCOM Setting
	send_data_cmd(0x003E); //0 VCM[6:0]  //VCOMH voltage:00-7F
	send_data_cmd(0x006A); //0 VDV[6:0]  //VCOMa mplitude:00-7f
	send_data_cmd(0x0010); //0 0 0 1, 0 0 0 0

	send_ctrl_cmd(0x00D2); //Power Setting for Normal/Partial Mode
	send_data_cmd(0x0001);
	send_data_cmd(0x0024);

	send_ctrl_cmd(0x00D4);//Power Setting for Idle Mode
	send_data_cmd(0x0001);
	send_data_cmd(0x0024);

	send_ctrl_cmd(0x00E2); //NVM Load Control
	send_data_cmd(0x003F);
	//=========================================
	//======Power setting End==================
	//=========================================
	send_ctrl_cmd(0x0034); //set_tear_on
	//send_ctrl_cmd(0x0035); //set_tear_on
	//send_data_cmd(0x0000);

	send_ctrl_cmd(0x0036); //set_address_mode
	send_data_cmd(0x00C0);

	send_ctrl_cmd(0x003A);//set_pixel_format
	send_data_cmd(0x0055); 

	send_ctrl_cmd(0x002A); //set_column_address
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x00EF);

	send_ctrl_cmd(0x002B); //set_page_address
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0001);
	send_data_cmd(0x003F);

	send_ctrl_cmd(0x0011); //exit_sleep_mode
	MDELAY(100);
	send_ctrl_cmd(0x0029); //set_display_on
	MDELAY(50);
	send_ctrl_cmd(0x002C); //write_memory_start
	//MDELAY(200);
	#endif

	// test
	//lcm_update_black(0, 0, 240, 320, 0x00);
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

    params->type   = LCM_TYPE_DBI;
    params->ctrl   = LCM_CTRL_PARALLEL_DBI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    params->io_select_mode = 3;

	params->dbi.port                    = 0;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_52M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_8BITS; //LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST; // LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB565;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_8BITS; // LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_8_BITS; // LCM_DBI_CPU_WRITE_16_BITS;
    params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_8MA;

/*
ASSERT(id >= LCD_IF_PARALLEL_0 && id <= LCD_IF_PARALLEL_2);
    ASSERT(writeSetup <= 15U);
    ASSERT(writeHold <= 4U);
    ASSERT(writeWait <= 64U);
    ASSERT(readSetup <= 15U);
    ASSERT(readLatency <= 64U);
    ASSERT(waitPeriod <= 1023U);
*/    
    params->dbi.parallel.write_setup    = 1; // 15;
    params->dbi.parallel.write_hold     = 2; //4;
    params->dbi.parallel.write_wait     = 3; //64;
    params->dbi.parallel.read_setup     = 3;
    params->dbi.parallel.read_latency   = 10;
    params->dbi.parallel.wait_period    = 5;
}

static void lcm_init(void)
{
	unsigned int lcd_id = 0;
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);
    MDELAY(120);

	send_ctrl_cmd(0x00b0);
	send_data_cmd(0x003F);
	send_data_cmd(0x003F);
	MDELAY(5); 

	lcd_id = get_lcd_id_n(0xBF, 4);
	lcd_id = (lcd_id << 8) | (get_lcd_id_n(0xBF, 5) & 0xff);
	LCD_PRINT("[HT_R61526]lcm_init: lcd_id = 0x%x\n", lcd_id);

    init_lcm_registers();

	lcm_update_black(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0x00);
}

static void lcm_suspend(void)
{
	send_ctrl_cmd(0x10);
	MDELAY(120);
}


static void lcm_resume(void)
{
	unsigned int lcd_id = 0;
	
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(20);  
    SET_RESET_PIN(1);
    MDELAY(10);

	send_ctrl_cmd(0x00b0);
	send_data_cmd(0x003F);
	send_data_cmd(0x003F);
	MDELAY(5); 

	lcd_id = get_lcd_id_n(0xBF, 4);
	lcd_id = (lcd_id << 8) | (get_lcd_id_n(0xBF, 5) & 0xff);
	LCD_PRINT("[HT_R61526]lcm_resume: lcd_id = 0x%x\n", lcd_id);

	init_lcm_registers();
}


static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

	send_ctrl_cmd(0x2A);
	send_data_cmd(HIGH_BYTE(x0));
	send_data_cmd(LOW_BYTE(x0));
	send_data_cmd(HIGH_BYTE(x1));
	send_data_cmd(LOW_BYTE(x1));

	send_ctrl_cmd(0x2B);
	send_data_cmd(HIGH_BYTE(y0));
	send_data_cmd(LOW_BYTE(y0));
	send_data_cmd(HIGH_BYTE(y1));
	send_data_cmd(LOW_BYTE(y1));

	// Write To GRAM
	send_ctrl_cmd(0x2C);

}

static unsigned int get_lcd_id_r(unsigned int addr)
{
	unsigned short id = 0;
	
	send_ctrl_cmd(addr);
    id = read_data_cmd();
	id = (id << 8) | (read_data_cmd() & 0xFF);

	LCD_PRINT("[HT_R61526]get_lcd_id_r: id = 0x%x\n", id);
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
	LCD_PRINT("[HT_R61526]get_lcd_id_n: id = 0x%x\n", id);
	
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
	unsigned int id = 0x00;

	send_ctrl_cmd(0x00b0);
	send_data_cmd(0x003F);
	send_data_cmd(0x003F);
	MDELAY(5); 
	id = get_lcd_id_n(0xBF, 4);
	id = (id << 8) | (get_lcd_id_n(0xBF, 5) & 0xff);

	LCD_PRINT("[HT_R61526]lcm_compare_id: id = 0x%x\n", id);
    return (LCM_ID == id)?1:0;
}

static unsigned int lcm_check_status(void)
{
	/*
	send_ctrl_cmd(0xB9);
	if(read_data_cmd() != 0x00)	return 0;
	if(read_data_cmd() != 0xff)	return 0;
	if(read_data_cmd() != 0x83)	return 0;
	if(read_data_cmd() != 0x69)	return 0;
	*/

	return 1;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER r61526_lcm_drv = 
{
    .name			= "r61526",
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
	.check_status	= lcm_check_status
};
