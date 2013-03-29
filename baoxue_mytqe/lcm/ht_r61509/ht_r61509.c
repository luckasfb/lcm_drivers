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
#define FRAME_HEIGHT (400) // (800)
#define LCM_ID        (0xB509) //(0xb509)

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
    lcm_util.send_cmd(cmd >> 8);
    lcm_util.send_cmd(cmd & 0xff);
}

static __inline void send_data_cmd(unsigned int data)
{
    lcm_util.send_data(data >> 8);
    lcm_util.send_data(data & 0xff);
}

static __inline unsigned int read_data_cmd()
{
	unsigned int id = 0;
	id = lcm_util.read_data();
	id = (id << 8) | (lcm_util.read_data() & 0xff);
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
	set_lcm_register(0x0210, x0, 0);
	set_lcm_register(0x0211, x1, 0);
	set_lcm_register(0x0212, y0, 0);
	set_lcm_register(0x0213, y1, 0);
	
	set_lcm_register(0x0200, x0, 0);
	set_lcm_register(0x0201, y0, 0);

	send_ctrl_cmd(0x0202);
	
	for (i = x0; i < x1; i++)
	{
		for (k = y0; k < y1; k++)
		{
			send_data_cmd(data);
		}
	}
}
//unsigned int vddhh = 0x00;
static void init_lcm_registers(void)
{
    // ht_r61509  
    #if 1
	// uses XXD's codes  on 2012/08/06
	// Mr. Wan modifies codes on 2012/08/16
	set_lcm_register(0x0000, 0x0000, 0);
	set_lcm_register(0x0000, 0x0000, 10);

	set_lcm_register(0x0000, 0x0000, 0);
	set_lcm_register(0x0000, 0x0000, 0);
	set_lcm_register(0x0000, 0x0000, 0);
	set_lcm_register(0x0000, 0x0000, 10);

	set_lcm_register(0x0400, 0xE200, 0);  //  0x6200
	set_lcm_register(0x0008, 0x0808, 0);

	set_lcm_register(0x0300, 0x0803, 0);//gamma
	set_lcm_register(0x0301, 0x4714, 0);
	set_lcm_register(0x0302, 0x0703, 0);
	set_lcm_register(0x0303, 0x0D0A, 0);

	set_lcm_register(0x0304, 0x3233, 0);

	set_lcm_register(0x0305, 0x0A0D, 0);
	set_lcm_register(0x0306, 0x4307, 0);
	set_lcm_register(0x0307, 0x1407, 0);
	set_lcm_register(0x0308, 0x0308, 0);

	set_lcm_register(0x0309, 0x3332, 0);

	set_lcm_register(0x0010, 0x0010, 0);
	set_lcm_register(0x0011, 0x0101, 0);
	set_lcm_register(0x0012, 0x0303, 0);
	set_lcm_register(0x0013, 0x0001, 0);

	set_lcm_register(0x0100, 0x0330, 0);//BT,AP
	set_lcm_register(0x0101, 0x0247, 0);//DC0,DC1,VC
	set_lcm_register(0x0103, 0x1000, 0);//VDV	0x1300
	
	set_lcm_register(0x0280, 0x5400, 0);//VCM 0x6200
	//vddhh += 2;
	set_lcm_register(0x0102, 0xF9B0, 100);//VRH,VCMR,PSON,PON

	set_lcm_register(0x0001, 0x0000, 0);  //  0x0100
	set_lcm_register(0x0002, 0x0100, 0);
	set_lcm_register(0x0003, 0x1030, 0);
	set_lcm_register(0x0009, 0x0001, 0);
	set_lcm_register(0x000C, 0x0000, 0);
	set_lcm_register(0x0090, 0x8000, 0);
	set_lcm_register(0x000F, 0x0000, 0);

	set_lcm_register(0x0210, 0x0000, 0);
	set_lcm_register(0x0211, 0x00EF, 0);
	set_lcm_register(0x0212, 0x0000, 0);
	set_lcm_register(0x0213, 0x018F, 0);//432=1AF, 400=18F
	set_lcm_register(0x0200, 0x0000, 0);
	set_lcm_register(0x0201, 0x0000, 0);

	set_lcm_register(0x0500, 0x0000, 0);
	set_lcm_register(0x0501, 0x0000, 0);
	set_lcm_register(0x0502, 0x005F, 0);
	set_lcm_register(0x0401, 0x0001, 0);
	set_lcm_register(0x0404, 0x0000, 100);

	set_lcm_register(0x0007, 0x0100, 100);//BASEE
	
	send_ctrl_cmd(0x0202);
	#else
	// codes given by Hongtao Co. 20120608

	set_lcm_register(0x0000, 0x0000, 0);
	set_lcm_register(0x0000, 0x0000, 10);
	//set_lcm_register(time, 0x10 ,ms, 0);
	set_lcm_register(0x0000, 0x0000, 0);
	set_lcm_register(0x0000, 0x0000, 0);
	set_lcm_register(0x0000, 0x0000, 0);
	set_lcm_register(0x0000, 0x0000, 10);
	//set_lcm_register(time, 0x10 ,ms, 0);
	set_lcm_register(0x0400, 0xE200, 0);	// 0x6200
	set_lcm_register(0x0008, 0x0808, 0);

	set_lcm_register(0x0300, 0x0109, 0);
	set_lcm_register(0x0301, 0x7E0A, 0);
	set_lcm_register(0x0302, 0x0704, 0);
	set_lcm_register(0x0303, 0x0911, 0);
	set_lcm_register(0x0304, 0x2100, 0);

	set_lcm_register(0x0305, 0x1109, 0);
	set_lcm_register(0x0306, 0x7407, 0);
	set_lcm_register(0x0307, 0x0A0E, 0);
	set_lcm_register(0x0308, 0x0901, 0);
	set_lcm_register(0x0309, 0x0021, 0);

	set_lcm_register(0x0010, 0x0010, 0);
	set_lcm_register(0x0011, 0x0202, 0);
	set_lcm_register(0x0012, 0x0300, 0);
	set_lcm_register(0x0013, 0x0007, 10);
  
    set_lcm_register(0x0100, 0x0000, 0);//BT, AP
	set_lcm_register(0x0101, 0x0007, 0);//DC0, DC1, VC
	set_lcm_register(0x0102, 0x0000, 0);
	set_lcm_register(0x0103, 0x0000, 0);//VDV
	set_lcm_register(0x0280, 0x0000, 100);//VCM
	//set_lcm_register(0x0102, 0x0000, 100);//VR, VCMR, PSON, PON 


	set_lcm_register(0x0100, 0x0330, 0);//BT, AP
	set_lcm_register(0x0101, 0x0247, 50);//DC0, DC1, VC
	set_lcm_register(0x0102, 0xD1B0, 50);//VR, VCMR, PSON, PON
	set_lcm_register(0x0103, 0x1A00, 0);//VDV
	set_lcm_register(0x0280, 0x6E00, 100);//VCM

	//set_lcm_register(time, 0x100 ,ms, 0);

	set_lcm_register(0x0001, 0x0000, 0);   //  0x0100
	set_lcm_register(0x0002, 0x0100, 0);
	set_lcm_register(0x0003, 0x1030, 0);
	set_lcm_register(0x0009, 0x0001, 0);
	set_lcm_register(0x000C, 0x0000, 0);
	set_lcm_register(0x0090, 0x8000, 0);
	set_lcm_register(0x000F, 0x0000, 0);

    set_lcm_register(0x0200, 0x0000, 0);
	set_lcm_register(0x0210, 0x0000, 0);
	set_lcm_register(0x0211, 0x00EF, 0);
	set_lcm_register(0x0212, 0x0000, 0);
	set_lcm_register(0x0213, 0x018F, 0);
	set_lcm_register(0x0500, 0x0000, 0);
	set_lcm_register(0x0501, 0x0000, 0);
	set_lcm_register(0x0502, 0x005F, 0);
	set_lcm_register(0x0401, 0x0001, 0);
	set_lcm_register(0x0404, 0x0000, 50);

	set_lcm_register(0x0007, 0x0100, 110);

	set_lcm_register(0x0200, 0x0000, 0);
	set_lcm_register(0x0201, 0x0000, 0);
	
	send_ctrl_cmd(0x0202);
	#endif
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
    params->dbi.parallel.write_setup    = 1;
    params->dbi.parallel.write_hold     = 2;
    params->dbi.parallel.write_wait     = 3; // 2; 
    params->dbi.parallel.read_setup     = 3; // 1;
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
	
	lcd_id = get_lcd_id_r(0x0000);
	LCD_PRINT("[HT_R61509]lcm_init: lcd_id = 0x%x\n", lcd_id);

    init_lcm_registers();
	lcm_update_black(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0x00);
}

static void lcm_suspend(void)
{
	set_lcm_register(0x0007, 0x0000, 100);
	set_lcm_register(0x0102, 0xd180, 100);
}


static void lcm_resume(void)
{
	unsigned int lcd_id = 0;
	
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(20);  
    SET_RESET_PIN(1);
    MDELAY(10);  // 400
	
	lcd_id = get_lcd_id_r(0x0000);
	LCD_PRINT("[HT_R61509]lcm_resume: lcd_id = 0x%x\n", lcd_id);

    init_lcm_registers();
}


static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

	set_lcm_register(0x0210, x0, 0);
	set_lcm_register(0x0211, x1, 0);
	set_lcm_register(0x0212, y0, 0);
	set_lcm_register(0x0213, y1, 0);
	
	set_lcm_register(0x0200, x0, 0);
	set_lcm_register(0x0201, y0, 0);

	send_ctrl_cmd(0x0202);
}

static unsigned int get_lcd_id_r(unsigned int addr)
{
	unsigned short id = 0;
	
	send_ctrl_cmd(addr);
    id = read_data_cmd();

	LCD_PRINT("[HT_R61509]get_lcd_id_r: id = 0x%x\n", id);
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
	LCD_PRINT("[HT_R61509]get_lcd_id_n: id = 0x%x\n", id);
	
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
	unsigned int lcd_id = 0, k = 3;

	while (k-- > 0)
	{
		SET_RESET_PIN(1);
		MDELAY(5);
	    SET_RESET_PIN(0);
	    MDELAY(50);  
	    SET_RESET_PIN(1);
	    MDELAY(120);

		lcd_id = get_lcd_id_r(0x0000);

		LCD_PRINT("[HT_R61509]lcm_compare_id: lcd_id = 0x%x\n", lcd_id);

		if (LCM_ID == lcd_id)
			return 1;
	}
    return (LCM_ID == lcd_id)?1:0;
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
LCM_DRIVER ht_r61509_lcm_drv = 
{
    .name			= "ht_r61509",
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
