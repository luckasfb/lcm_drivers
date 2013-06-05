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
#ifdef BUILD_LK
#else
#include <linux/string.h>
#endif

#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (240)
#define FRAME_HEIGHT (320)
#define LCM_ID       (0x69)

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

static __inline void send_ctrl_cmd(unsigned int cmd)
{
	lcm_util.send_cmd(cmd);
}

static __inline void send_data_cmd(unsigned int data)
{
	lcm_util.send_data(data&0xff);
}

static __inline unsigned int read_data_cmd(void)
{
    return 0xFF&lcm_util.read_data();
}

static __inline void set_lcm_register(unsigned int regIndex,
		unsigned int regData)
{
	send_ctrl_cmd(regIndex);
	send_data_cmd(regData);
}

static void RGB_test_func()
{
	unsigned short x0, y0, x1, y1, x, y;
	send_ctrl_cmd(0x2c);

	unsigned short red;

	red = 0x1;
	y = 0;
	
#if 1 // R and B
	while( y < 16) {
		for (y0 = 0; y0 < 20; y0++) {
			for (x = 0; x < 240; x++) {
				lcm_util.send_data(red << 11);
				//lcm_util.send_data(red);
			}
		}
		red++;
		y++;;
	};
#else // G
	while( y < 63) {
		for (y0 = 0; y0 < 5; y0++) {
			for (x = 0; x < 240; x++) {
				//lcm_util.send_data(red << 11);
				lcm_util.send_data(red<<5);
			}
		}
		red++;
		y++;;
	};
#endif

}

static void sw_clear_panel(unsigned int color)
{
	unsigned short x0, y0, x1, y1, x, y;
	unsigned short h_X_start,l_X_start,h_X_end,l_X_end,h_Y_start,l_Y_start,h_Y_end,l_Y_end;

	x0 = (unsigned short)0;
	y0 = (unsigned short)0;
	x1 = (unsigned short)FRAME_WIDTH-1;
	y1 = (unsigned short)FRAME_HEIGHT-1;

	h_X_start=((x0&0xFF00)>>8);
	l_X_start=(x0&0x00FF);
	h_X_end=((x1&0xFF00)>>8);
	l_X_end=(x1&0x00FF);

	h_Y_start=((y0&0xFF00)>>8);
	l_Y_start=(y0&0x00FF);
	h_Y_end=((y1&0xFF00)>>8);
	l_Y_end=(y1&0x00FF);

	send_ctrl_cmd(0x2A);
	send_data_cmd(h_X_start); 
	send_data_cmd(l_X_start); 
	send_data_cmd(h_X_end); 
	send_data_cmd(l_X_end); 

	send_ctrl_cmd(0x2B);
	send_data_cmd(h_Y_start); 
	send_data_cmd(l_Y_start); 
	send_data_cmd(h_Y_end); 
	send_data_cmd(l_Y_end); 

	send_ctrl_cmd(0x29); 

	send_ctrl_cmd(0x2C);
    for (y = y0; y <= y1; ++ y) {
        for (x = x0; x <= x1; ++ x) {
            lcm_util.send_data(color);
        }
    }
}

static void init_lcm_registers(void)
{
	int x,y;
//E0 AUO
	//		send_ctrl_cmd(0x11); // Sleep Out
	//		MDELAY(120);   // 120

	send_ctrl_cmd(0xB0);
	send_data_cmd(0x3F);
	send_data_cmd(0x3F);

	send_ctrl_cmd(0xB3);
	send_data_cmd(0x02);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x31);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xB4);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC0);
	send_data_cmd(0x33);
	send_data_cmd(0x4F);   
	send_data_cmd(0x00);
	send_data_cmd(0x10);   
	send_data_cmd(0xA0);
	send_data_cmd(0x00);   
	send_data_cmd(0x01);
	send_data_cmd(0x00);   

	send_ctrl_cmd(0xC1);
	send_data_cmd(0x01);
	send_data_cmd(0x02);   
	send_data_cmd(0x1A);
	send_data_cmd(0x60);   
	send_data_cmd(0x60);

	MDELAY(25); // 25

	send_ctrl_cmd(0xC3);
	send_data_cmd(0x01);
	send_data_cmd(0x00);
	send_data_cmd(0x28);
	send_data_cmd(0x08);
	send_data_cmd(0x08);

	MDELAY(25); // 25

	send_ctrl_cmd(0xC4);
	send_data_cmd(0x11);
	send_data_cmd(0x01);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x00);

#if 1
	send_ctrl_cmd(0xC8); //Gamma B
	send_data_cmd(0x0b); // 1
	send_data_cmd(0x18); // 2
	send_data_cmd(0x15); // 3
	send_data_cmd(0x11); // 4
	send_data_cmd(0x0e); // 5
	send_data_cmd(0x06); // 6
	send_data_cmd(0x03); // 7
	send_data_cmd(0x0a); // 9
	send_data_cmd(0x09); // 10
	send_data_cmd(0x0c); // 11
	send_data_cmd(0x00); // 12
	send_data_cmd(0x09); // 13
	send_data_cmd(0x0a); // 14
	send_data_cmd(0x03); // 15
	send_data_cmd(0x06); // 16
	send_data_cmd(0x0e); // 17
	send_data_cmd(0x11); // 18
	send_data_cmd(0x15); // 19
	send_data_cmd(0x18); // 20
	send_data_cmd(0x0b); // 21
	send_data_cmd(0x00); // 22
	send_data_cmd(0x00); // 22

	send_ctrl_cmd(0xC9); //Gamma G
	send_data_cmd(0x0B); // 1
	send_data_cmd(0x19); // 2
	send_data_cmd(0x14); // 3
	send_data_cmd(0x12); // 4
	send_data_cmd(0x0e); // 5
	send_data_cmd(0x08); // 6
	send_data_cmd(0x05); // 7
	send_data_cmd(0x05); // 8
	send_data_cmd(0x09); // 9
	send_data_cmd(0x0C); // 10
	send_data_cmd(0x00); // 11
	send_data_cmd(0x09); // 12
	send_data_cmd(0x05); // 13
	send_data_cmd(0x05); // 14
	send_data_cmd(0x08); // 15
	send_data_cmd(0x0e); // 16
	send_data_cmd(0x12); // 17
	send_data_cmd(0x14); // 18
	send_data_cmd(0x19); // 19
	send_data_cmd(0x0B); // 20
	send_data_cmd(0x00); // 21
	send_data_cmd(0x00); // 22

	send_ctrl_cmd(0xCA); //Gamma R
	send_data_cmd(0x0B); // 1
	send_data_cmd(0x1A); // 2
	send_data_cmd(0x14); // 3
	send_data_cmd(0x12); // 4
	send_data_cmd(0x0e); // 5
	send_data_cmd(0x08); // 6
	send_data_cmd(0x04); // 7
	send_data_cmd(0x05); // 8
	send_data_cmd(0x09); // 9
	send_data_cmd(0x0C); // 10
	send_data_cmd(0x00); // 11
	send_data_cmd(0x09); // 12
	send_data_cmd(0x05); // 13
	send_data_cmd(0x04); // 14
	send_data_cmd(0x08); // 15
	send_data_cmd(0x0e); // 16
	send_data_cmd(0x12); // 17
	send_data_cmd(0x14); // 18
	send_data_cmd(0x1A); // 19
	send_data_cmd(0x0B); // 20
	send_data_cmd(0x00); // 21
	send_data_cmd(0x00); // 22
#else
	send_ctrl_cmd(0xC8); //Gamma B
	send_data_cmd(0x07); // 1
	send_data_cmd(0x19); // 2
	send_data_cmd(0x18); // 3
	send_data_cmd(0x14); // 4
	send_data_cmd(0x0C); // 5
	send_data_cmd(0x0B); // 6
	send_data_cmd(0x04); // 7
	send_data_cmd(0x03); // 8
	send_data_cmd(0x09); // 9
	send_data_cmd(0x0C); // 10
	send_data_cmd(0x00); // 11
	send_data_cmd(0x09); // 12
	send_data_cmd(0x03); // 13
	send_data_cmd(0x04); // 14
	send_data_cmd(0x0B); // 15
	send_data_cmd(0x0C); // 16
	send_data_cmd(0x14); // 17
	send_data_cmd(0x18); // 18
	send_data_cmd(0x19); // 19
	send_data_cmd(0x07); // 20
	send_data_cmd(0x00); // 21
	send_data_cmd(0x00); // 22

	send_ctrl_cmd(0xC9); //Gamma G
	send_data_cmd(0x0B); // 1
	send_data_cmd(0x1A); // 2
	send_data_cmd(0x14); // 3
	send_data_cmd(0x14); // 4
	send_data_cmd(0x0C); // 5
	send_data_cmd(0x0A); // 6
	send_data_cmd(0x04); // 7
	send_data_cmd(0x03); // 8
	send_data_cmd(0x09); // 9
	send_data_cmd(0x0C); // 10
	send_data_cmd(0x00); // 11
	send_data_cmd(0x09); // 12
	send_data_cmd(0x03); // 13
	send_data_cmd(0x04); // 14
	send_data_cmd(0x0A); // 15
	send_data_cmd(0x0C); // 16
	send_data_cmd(0x14); // 17
	send_data_cmd(0x14); // 18
	send_data_cmd(0x1A); // 19
	send_data_cmd(0x0B); // 20
	send_data_cmd(0x00); // 21
	send_data_cmd(0x00); // 22

	send_ctrl_cmd(0xCA); //Gamma R
	send_data_cmd(0x0B); // 1
	send_data_cmd(0x1A); // 2
	send_data_cmd(0x14); // 3
	send_data_cmd(0x14); // 4
	send_data_cmd(0x0C); // 5
	send_data_cmd(0x0A); // 6
	send_data_cmd(0x04); // 7
	send_data_cmd(0x03); // 8
	send_data_cmd(0x09); // 9
	send_data_cmd(0x0C); // 10
	send_data_cmd(0x00); // 11
	send_data_cmd(0x09); // 12
	send_data_cmd(0x03); // 13
	send_data_cmd(0x04); // 14
	send_data_cmd(0x0A); // 15
	send_data_cmd(0x0C); // 16
	send_data_cmd(0x14); // 17
	send_data_cmd(0x14); // 18
	send_data_cmd(0x1A); // 19
	send_data_cmd(0x0B); // 20
	send_data_cmd(0x00); // 21
	send_data_cmd(0x00); // 22
#endif
	send_ctrl_cmd(0xD0);
	send_data_cmd(0x23);
	send_data_cmd(0x53);
	send_data_cmd(0x02);
	send_data_cmd(0x38);
	send_data_cmd(0x30);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xD2);
	send_data_cmd(0x01);
	send_data_cmd(0x22);


	send_ctrl_cmd(0x35);
	send_data_cmd(0x00);


	/* Tearing effect Control Parameter */
	send_ctrl_cmd(0x44);
	send_data_cmd(0x00);
	send_data_cmd(0xef); 


	send_ctrl_cmd(0x36);
	send_data_cmd(0x00);

	send_ctrl_cmd(0x3A);
	send_data_cmd(0x55);

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

	MDELAY(120);   // 120
#if 1
	send_ctrl_cmd(0x2c); // Write memory start
	for(y = 0; y < 320; y++) { //clear Gram
		int pixel = 0x0;
		for(x= 0; x < 240; x++) {
			send_data_cmd(pixel);
		}
	}
	MDELAY(30);
#endif
	send_ctrl_cmd(0x29); //Display On
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
	params->dbi.io_driving_current      = 0;

	params->dbi.parallel.write_setup    = 2;
	params->dbi.parallel.write_hold     = 2;
	params->dbi.parallel.write_wait     = 6; // 4 -> 6
	params->dbi.parallel.read_setup     = 2;
	params->dbi.parallel.read_latency   = 31;
	params->dbi.parallel.wait_period    = 9;

    // enable tearing-free
    params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;
	
}



static void lcm_init(void)
{ 
	SET_RESET_PIN(0);
	MDELAY(1);// 500
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(20); // 20
	SET_RESET_PIN(1);
	MDELAY(50); // 50
	send_ctrl_cmd(0x11); // Sleep Out
	MDELAY(120);   // 120

	init_lcm_registers();

	//	RGB_test_func();
}


static void lcm_suspend(void)
{    
    sw_clear_panel(0);
	send_ctrl_cmd(0x10);
	MDELAY(5);
}


static void lcm_resume(void)
{
	lcm_init();

//	send_ctrl_cmd(0x11);
//	MDELAY(120);
}

static void lcm_update(unsigned int x, unsigned int y,
		unsigned int width, unsigned int height)
{
	unsigned short x0, y0, x1, y1;
	unsigned short h_X_start,l_X_start,h_X_end,l_X_end,h_Y_start,l_Y_start,h_Y_end,l_Y_end;

	x0 = (unsigned short)x;
	y0 = (unsigned short)y;
	x1 = (unsigned short)x+width-1;
	y1 = (unsigned short)y+height-1;


    h_X_start=((x0&0xFF00)>>8);
    l_X_start=(x0&0x00FF);
    h_X_end=((x1&0xFF00)>>8);
    l_X_end=(x1&0x00FF);
    
    h_Y_start=((y0&0xFF00)>>8);
    l_Y_start=(y0&0x00FF);
    h_Y_end=((y1&0xFF00)>>8);
    l_Y_end=(y1&0x00FF);

	send_ctrl_cmd(0x2A);
	send_data_cmd(h_X_start); 
	send_data_cmd(l_X_start); 
	send_data_cmd(h_X_end); 
	send_data_cmd(l_X_end); 

	send_ctrl_cmd(0x2B);
	send_data_cmd(h_Y_start); 
	send_data_cmd(l_Y_start); 
	send_data_cmd(h_Y_end); 
	send_data_cmd(l_Y_end); 

	send_ctrl_cmd(0x29); 

	send_ctrl_cmd(0x2C);
}

static void lcm_setbacklight(unsigned int level)
{
	if(level > 255) level = 255;
#if 0	
	send_ctrl_cmd(0x51);
	send_data_cmd(level);	
#else
    send_ctrl_cmd(0xBE);
    send_data_cmd(0x0F);
#endif
}
static unsigned int lcm_compare_id(void)
{	
#if 0
    send_ctrl_cmd(0xB9);  // SET password
	send_data_cmd(0xFF);  
	send_data_cmd(0x83);  
	send_data_cmd(0x69);
    send_ctrl_cmd(0xC3);
	send_data_cmd(0xFF);

	send_ctrl_cmd(0xF4);
	read_data_cmd();
    return (LCM_ID == read_data_cmd())?1:0;
#else
    return 1; 
#endif
}

static void lcm_set_pwm(unsigned int divider)
{
#if 0
 send_ctrl_cmd(0xBE);
 send_data_cmd(0xFF);

 send_ctrl_cmd(0xBF);
 send_data_cmd(0x07); 
#endif 
}


LCM_DRIVER auo_v3_lcm_drv = 
{
    .name			= "auo_v3",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
//	.set_backlight	= lcm_setbacklight,
	.set_pwm        = lcm_set_pwm,
	.compare_id     = lcm_compare_id,
};



