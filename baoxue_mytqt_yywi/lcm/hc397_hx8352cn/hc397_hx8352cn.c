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

#define READ_CC
#ifdef READ_CC
#define LCM_ID			(0x0F)
#define LCMID_REG		(0xCC)
#else
#define LCM_ID		    (0x9327) // 0x9327
#define LCMID_REG		(0xEF)
#endif

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
    #if 1 // 

	send_ctrl_cmd(0x11);
    MDELAY(120);    // SLPOUT
	//MDELAY(100);

	send_ctrl_cmd(0xF3 ); //Set EQ
	send_data_cmd(0x08);
	send_data_cmd(0x20);
	send_data_cmd(0x20);
	send_data_cmd(0x08);

	send_ctrl_cmd(0xE7 );
	send_data_cmd(0x60);

	send_ctrl_cmd(0xD1 );
	send_data_cmd(0x00);
	send_data_cmd(0x7B);
	send_data_cmd(0x17);

	send_ctrl_cmd(0xD0 );
	send_data_cmd(0x07);
	send_data_cmd(0x02);
	send_data_cmd(0x85);

	send_ctrl_cmd(0x36 );
	send_data_cmd(0x48);

	send_ctrl_cmd(0xC1 );
	send_data_cmd(0x10);
	send_data_cmd(0x1A);
	send_data_cmd(0x02);
	send_data_cmd(0x02);

	send_ctrl_cmd(0xC0 );
    send_data_cmd(0x05); // 0x00
	send_data_cmd(0x31);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x02);


	send_ctrl_cmd(0xC4 );
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC5 );
	send_data_cmd(0x0c);

	send_ctrl_cmd(0xD2 );
	send_data_cmd(0x01);
	send_data_cmd(0x22);

	//GAMMA2.2
	send_ctrl_cmd(0xC8);  //Set Gamma
	send_data_cmd(0x00);
	send_data_cmd(0x57);
	send_data_cmd(0x34);
	send_data_cmd(0x00);
	send_data_cmd(0x0F);
	send_data_cmd(0x02);
	send_data_cmd(0x34);
	send_data_cmd(0x02);
	send_data_cmd(0x77);
	send_data_cmd(0x00);
	send_data_cmd(0x02);
	send_data_cmd(0x0F);
	send_data_cmd(0x08);
	send_data_cmd(0x80);
	send_data_cmd(0x00);

	send_ctrl_cmd(0x3A);
	send_data_cmd(0x55);

    send_ctrl_cmd(0x2A);   // set cloumn adrdress
    send_data_cmd(0x00); 
    send_data_cmd(0x00);  
    send_data_cmd(0x00);  
    send_data_cmd(0xEF); 
    
    send_ctrl_cmd(0x2B);   // set page adrdress
    send_data_cmd(0x00); 
    send_data_cmd(0x00);  
    send_data_cmd(0x01);  
    send_data_cmd(0x8F); 

	send_ctrl_cmd(0x29);    // display on
	MDELAY(120);	 
    //send_ctrl_cmd(0x2C); 
	#endif
}





// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

/*
 * ATTENTIONS: There are a few important differences under the MT6575/15 compared with the MT6573/13
 * 
 *  type: if set LCM_TYPE_DBI, cpu interface.
 *  io_select_mode: if set 0, selects buss composition: LPA0, LWRB, LRDB control bus and NLD[0:15] data bus
 *                  if set 1, selects buss composition: DPIVSYNC, DPIDE, DPIHSYNC control bus and DPIRGB data bus.
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
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_52M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_8BITS; //LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST; // LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB565;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_8BITS; // LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_8_BITS; // LCM_DBI_CPU_WRITE_16_BITS;
	params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_6575_8MA; //LCM_DRIVING_CURRENT_8MA;

/*
ASSERT(id >= LCD_IF_PARALLEL_0 && id <= LCD_IF_PARALLEL_2);
    ASSERT(writeSetup <= 15U);
    ASSERT(writeHold <= 4U);
    ASSERT(writeWait <= 64U);
    ASSERT(readSetup <= 15U);
    ASSERT(readLatency <= 64U);
    ASSERT(waitPeriod <= 1023U);
*/    
	params->dbi.parallel.write_setup    = 1; // 2;
	params->dbi.parallel.write_hold     = 1; // 3; // 2, 4
	params->dbi.parallel.write_wait     = 6; //6;
	
	params->dbi.parallel.read_setup     = 3; // 3;
	params->dbi.parallel.read_hold      = 0;
    params->dbi.parallel.read_latency   = 18;
	
	params->dbi.parallel.wait_period    = 1;
	params->dbi.parallel.cs_high_width  = 0; //cycles of cs high level between each transfer
	//params->dbi.parallel.read_latency   = 20;  //40
	//params->dbi.parallel.wait_period    = 10; // 0

	// enable tearing-free
    //params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_ONLY;
    //params->dbi.te_edge_polarity        = LCM_POLARITY_FALLING;
}

static void lcm_init(void)
{
	//unsigned int lcd_id = 0;
	
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);

    // Advises that should hold high level for about 120 ms as for HX... series lcd ic
    MDELAY(120);
	

    init_lcm_registers();
	lcm_update_black(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0x00);
}

static void lcm_suspend(void)
{
	send_ctrl_cmd(0x28);
	MDELAY(50);
	send_ctrl_cmd(0x10);
	MDELAY(120);
}


static void lcm_resume(void)
{
	#if 0
	send_ctrl_cmd(0x11);
	MDELAY(120);
	send_ctrl_cmd(0x29);
	MDELAY(50);
	#else
	unsigned int lcd_id = 0;
	
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(20);  
    SET_RESET_PIN(1);
    MDELAY(120);  // 400

	#ifdef READ_CC
	lcd_id = get_lcd_id_n(LCMID_REG, 2);
	#else
	lcd_id = get_lcd_id_n(LCMID_REG, 4);
	lcd_id = (lcd_id << 8) | (get_lcd_id_n(0xEF, 5) & 0xff);
	#endif

	LCD_PRINT("[HC397_HX8352CN]lcm_resume: lcd_id = 0x%x\n", lcd_id);

	init_lcm_registers();	
	#endif
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

	LCD_PRINT("[HC397_HX8352CN]get_lcd_id_r: id = 0x%x\n", id);
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
	LCD_PRINT("[HC397_HX8352CN]get_lcd_id_n: id = 0x%x\n", id);
	
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
	unsigned int lcd_id = 0x00;

	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);
    MDELAY(120);
	
	#ifdef READ_CC
	lcd_id = get_lcd_id_n(LCMID_REG, 2);
	#else
	lcd_id = get_lcd_id_n(LCMID_REG, 4);
	lcd_id = (lcd_id << 8) | (get_lcd_id_n(0xEF, 5) & 0xff);
	#endif


	LCD_PRINT("[HC397_HX8352CN]lcm_compare_id: lcd_id = 0x%x\n", lcd_id);
    return (LCM_ID == lcd_id)?1:0;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hc397_hx8352cn_lcm_drv = 
{
    .name			= "hc397_hx8352cn",
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
