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

#if BUILD_UBOOT
#define	LCD_PRINT	printf
#else
#include <linux/kernel.h>

#define	LCD_PRINT	printk
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  	(320)
#define FRAME_HEIGHT 	(480)

#define LCMID_REG		(0xd3)
#define LCM_ID			(0x9486)

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
			send_data_cmd(data);
		}
	}
}

//unsigned int vcomadj = 0x00;
static void init_lcm_registers(void)
{
	// xingxunda 3.5 ili9486l on 2012/09/24
	send_ctrl_cmd(0x01);
	MDELAY(30);

	send_ctrl_cmd(0xF2);       
	send_data_cmd(0x18); 
	send_data_cmd(0xA3); 
	send_data_cmd(0x12); 
	send_data_cmd(0x02); 
	send_data_cmd(0xB2); 
	send_data_cmd(0x12); 
	send_data_cmd(0xFF); 
	send_data_cmd(0x10); 
	send_data_cmd(0x00); 
	 
	send_ctrl_cmd(0xF8);       
	send_data_cmd(0x21); 
	send_data_cmd(0x04); 
	 
	/*send_ctrl_cmd(0xF9);       
	send_data_cmd(0x00); 
	send_data_cmd(0x08); */
	 
	send_ctrl_cmd(0x36); 
	send_data_cmd(0x08); //  0x08

	send_ctrl_cmd(0x3A); 
	send_data_cmd(0x55); 
	 
	send_ctrl_cmd(0xB4); 
	send_data_cmd(0x02); 
	 
	send_ctrl_cmd(0xB6); 
	send_data_cmd(0x02);
	send_data_cmd(0x22); 
	
	 send_ctrl_cmd(0xC1); 
	send_data_cmd(0x41); 
	 
	 
	send_ctrl_cmd(0xC5); 	//  VCOM
	send_data_cmd(0x00); 
	send_data_cmd(0x08); 
	//vcomadj+=1;
	 
	send_ctrl_cmd(0xE0); 
	send_data_cmd(0x0F); 
	send_data_cmd(0x1F); 	
	send_data_cmd(0x1C); 
	send_data_cmd(0x0C); 
	send_data_cmd(0x0F); 
	send_data_cmd(0x08); 
	send_data_cmd(0x48); 
	send_data_cmd(0x98); 
	send_data_cmd(0x37); 
	send_data_cmd(0x0A); 
	send_data_cmd(0x13); 
	send_data_cmd(0x04); 
	send_data_cmd(0x11); 
	send_data_cmd(0x0D); 
	send_data_cmd(0x00);   
	 
	send_ctrl_cmd(0xE1); 
	send_data_cmd(0x0F); 
	send_data_cmd(0x32); 
	send_data_cmd(0x2E); 
	send_data_cmd(0x0B); 
	send_data_cmd(0x0D); 
	send_data_cmd(0x05); 
	send_data_cmd(0x47); 
	send_data_cmd(0x75); 
	send_data_cmd(0x37); 
	send_data_cmd(0x06); 
	send_data_cmd(0x10); 
	send_data_cmd(0x03); 
	send_data_cmd(0x24); 
	send_data_cmd(0x20); 
	send_data_cmd(0x00); 

	send_ctrl_cmd(0x2A); //Set_column_address
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x3F);
	//Set_page_address
	send_ctrl_cmd(0x2B); //Set_page_address
	send_data_cmd(0x00);
	send_data_cmd(0x00);//00
	send_data_cmd(0x01);
	send_data_cmd(0xDF);//8f
		 
	send_ctrl_cmd(0x11); 
	MDELAY(120); 
	send_ctrl_cmd(0x29); 
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
	params->dbi.data_width              = LCM_DBI_DATA_WIDTH_16BITS;
	params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
	params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
	params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB565;
	params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_16BITS;
	params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_16_BITS;
	params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_6575_8MA; //LCM_DRIVING_CURRENT_8MA;

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
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(100);
	SET_RESET_PIN(1);

    // Advises that should hold high level for about 120 ms as for HX... series lcd ic
    MDELAY(120);
	//MDELAY(50);

	//lcd_id = get_lcd_id_n(LCMID_REG, 2);
	//LCD_PRINT("[XXD35_ILI9486L]lcm_init: lcd_id = 0x%x\n", lcd_id);

	init_lcm_registers();
	lcm_update_black(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0x00);

    #if 0
    //Set TE register
	send_ctrl_cmd(0x35);
	send_data_cmd(0x00);

    send_ctrl_cmd(0X0044);  // Set TE signal MDELAY scanline
    send_data_cmd(0X0000);  // Set as 0-th scanline
    send_data_cmd(0X0000);
	//sw_clear_panel(0);
    #endif
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
    MDELAY(20); 

	lcd_id = get_lcd_id_n(LCMID_REG, 3) & 0xFF;
	lcd_id = (lcd_id << 8) | (get_lcd_id_n(LCMID_REG, 4) & 0xFF);
	LCD_PRINT("[XXD35_ILI9486L]lcm_resume: lcd_id = 0x%x\n", lcd_id);

	init_lcm_registers();	
	#endif
}

static void lcm_update(unsigned int x, unsigned int y,	unsigned int width, unsigned int height)
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
	//id = (id << 8) | (read_data_cmd() & 0xFF);

	LCD_PRINT("[XXD35_ILI9486L]get_lcd_id_r: id = 0x%x\n", id);
	
	return id;
}

static unsigned int get_lcd_id_n(unsigned int addr, unsigned char count)
{
	volatile unsigned int id = 0;
	unsigned char k = 0;
	

	send_ctrl_cmd(addr);
    UDELAY(10);

	while (k < count)
	{
		id = read_data_cmd();
		k++;
	}
	LCD_PRINT("[XXD35_ILI9486L]get_lcd_id_n: id = 0x%x\n", id);
	
    return id;
}


static void lcm_setbacklight(unsigned int level)
{
	/*
    // Tearing effect
	if(level > 255) level = 255;
	send_ctrl_cmd(0x51);
	send_data_cmd(level);	
	*/
}

static void lcm_setpwm(unsigned int divider)
{
}

static unsigned int lcm_getpwm(unsigned int divider)
{
	unsigned int pwm_clk = 23706 / (1<<divider);	
	return pwm_clk;
}

static unsigned int lcm_compare_id(void)
{	
	unsigned int lcd_id = 0x0;

	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);
    MDELAY(120);

	lcd_id = get_lcd_id_n(LCMID_REG, 3) & 0xFF;
	lcd_id = (lcd_id << 8) | (get_lcd_id_n(LCMID_REG, 4) & 0xFF);
	LCD_PRINT("[XXD35_ILI9486L]lcm_compare_id: lcd_id = 0x%x\n", lcd_id);
	
    return (LCM_ID == lcd_id)?1:0;
}

LCM_DRIVER xxd35_ili9486l_lcm_drv = 
{
    .name			= "xxd35_ili9486l",
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

