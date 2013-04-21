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
#if defined(BUILD_UBOOT)
#include <asm/arch/mt6577_gpio.h>
#else
#include <mach/mt6577_gpio.h>
#endif
#endif
#include "lcm_drv.h"
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(800)
#define REGFLAG_DELAY             							0x00 //0XFE
#define REGFLAG_END_OF_TABLE      							0x01//0xFF   // END OF REGISTERS MARKER


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

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   		lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};


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
	{0xFF,	3,	{0xFF, 0x98, 0x05}},

	{0xFD,	4,	{0x03, 0x13, 0x44, 0x00}},

	{0xF8,	15,	{0x18, 0x02, 0x02, 0x18,
			 0x02, 0x02, 0x30, 0x01,
			 0x01, 0x30, 0x01, 0x01,
			 0x30, 0x01, 0x01}},

	{0xB8, 	1,	{0x74}},

	{0xF1, 	1,	{0x00}},

	{0xF2, 	3,	{0x00, 0x58, 0x40}},

	{0xFC,	10, 	{0x04, 0x0F, 0x01}},

	{0xEB,	2,	{0x08, 0x0F}},

	{0xE0,	16,	{0x00, 0x11, 0x22, 0x11,
			 0x10, 0x1D, 0x0C, 0x0A,
			 0x00, 0x06, 0x00, 0x11,
			 0x0A, 0x35, 0x31, 0x00}},

	{0xE1,	16,	{0x00, 0x0C, 0x14, 0x13,
			 0x14, 0x16, 0x0B, 0x09,
			 0x01, 0x04, 0x05, 0x08,
			 0x09, 0x20, 0x1C, 0x00}},

	{0xC1,	4,	{0x13, 0x26, 0x06, 0x26}},

	{0xC7,	1,	{0x9C}},

	{0xB1,	3,	{0x00, 0x12, 0x14}},

	{0xB4,	1,	{0x02}}, 

	{0xBB,	2,	{0x14, 0x55}},   

	{0x36,	1,	{0x0A}},
	{0x3A,	1,	{0x77}},
	{0x35,	1,	{0x00}},

	{0x21,	1,	{0x00}},
	{0xB0,	1,	{0x00}},
	{0xB6,	1,	{0x01}},
	{0xC2,	1,	{0x11}},

	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.
	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
   	{REGFLAG_DELAY, 120, {}},

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
		params->dbi.te_mode 			= LCM_DBI_TE_MODE_VSYNC_ONLY;  //LCM_DBI_TE_MODE_DISABLED;//
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
xx
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

		// DSI

		//params->dsi.DSI_WMEM_CONTI=0x3C;
		//params->dsi.DSI_RMEM_CONTI=0x3E;

		//params->dsi.VC_NUM=0x0;

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
		params->dsi.pll_div1=30;//38;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		params->dsi.pll_div2=1;			// div2=0~15: fout=fvo/(2*div2)

}


static void lcm_update(unsigned int x, unsigned int y,unsigned int width, unsigned int height);
static void lcm_setbacklight(unsigned int level);
static void lcm_resume(void);
static void lcm_clear(unsigned int color)
{
   unsigned int x, y;

    lcm_update(0, 0, FRAME_WIDTH, FRAME_HEIGHT);


    for (y = 0; y < FRAME_HEIGHT; ++ y) {

        for (x = 0; x < FRAME_WIDTH; ++ x) {

            lcm_util.send_data(color);

        }

    }

}

static void lcm_init(void)
{
	unsigned int array[16];
	unsigned int id = 0;
	unsigned char buffer[4];
	unsigned int data_array[16];

	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(10);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

/*

	data_array[0]=0x00043902;

	data_array[1]=0x0598FFFF;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);

	//set 0xfd
	data_array[0]=0x00053902;

	data_array[1]=0x441303FD;//0x441F0FFD;

	data_array[2]=0x00000000;

	dsi_set_cmdq(&data_array, 3, 1);

	MDELAY(1);


	//SET 0XF8
	data_array[0] = 0x00103902; 

	data_array[1] = 0x020218F8;//0x030330F8; 

	data_array[2] = 0x30020218; //0x30020230; 

	data_array[3] = 0x01300101; 

	data_array[4] = 0x01013001; 

	dsi_set_cmdq(&data_array, 5, 1);

	MDELAY(1);

	

	data_array[0] = 0x74B81500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x00F11500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0]=0x00043902;

	data_array[1]=0x415800F2;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);


  //SET 0XFC
	data_array[0]=0x000b3902;

	data_array[1]=0x000f04FC;

	data_array[2] = 0x1ed87f3c;  

	data_array[3] = 0x0037b100;

	dsi_set_cmdq(&data_array, 4, 1);

	MDELAY(1);



	//data_array[0]=0x00023902;

	//data_array[1]=0x000019FE;

	//dsi_set_cmdq(&data_array, 2, 1);

	//MDELAY(400);


	data_array[0]=0x00033902;

	data_array[1]=0x000F08EB;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);


//
	data_array[0] = 0x00113902; 

	data_array[1] = 0x120800E0; //0x201B00E0;

	data_array[2] = 0x0c1c1314; //0x7B1C100D;

	data_array[3] = 0x0205000b; //0x0303000C;

	data_array[4] = 0x292e0a0c; //0x3E3F0B05; 

	data_array[5] = 0x00000000; //0x00000000;

	dsi_set_cmdq(&data_array, 6, 1);

	MDELAY(1);

//
	data_array[0] = 0x00113902; 

	data_array[1] = 0x110800E1; //0x312500E1;

	data_array[2] = 0x0c1c1313; //0x7C181314; 

	data_array[3] = 0x0104000B; //0x0103000A; 

	data_array[4] = 0x2a2d0a0c; //0x0F130610;

	data_array[5] = 0x00000000; //0x00000000;

	dsi_set_cmdq(&data_array, 6, 1);

	MDELAY(1);



	data_array[0]=0x00053902;

	data_array[1]=0x062613C1;//0x103013C1;

	data_array[2]=0x00000026;

	dsi_set_cmdq(&data_array, 3, 1);

	MDELAY(1);	



	data_array[0] = 0xb8C71500;//0xDCC71500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x00043902; // 

	data_array[1] = 0x141200B1; //

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);


	data_array[0] = 0x02B41500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0]=0x00033902;

	data_array[1]=0x005514bb;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);	


	data_array[0] = 0x0A361500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x773A1500; // 24bit

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x00351500;	// TE

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x00210500; 

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x00B01500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x01B61500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);




	data_array[0]=0x00023902;

	data_array[1]=0x000011C2;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);

	
*/

		
}


static void lcm_suspend(void)
{
	unsigned int data_array[16];
push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

/*
	data_array[0] = 0x00100500; // Sleep In

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(120);

	data_array[0] = 0x00280500;	// Display On

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(20);
*/

}


static void lcm_resume(void)
{
	unsigned int data_array[16];

	lcm_init();
	
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);

/*
	data_array[0] = 0x00110500;	// Sleep Out

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(120);
		

	data_array[0] = 0x00290500;	// Display On

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(20);
*/

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






static unsigned int lcm_compare_id(void)
{
	unsigned int id = 0;
	unsigned char buffer[4];
	unsigned int array[16];

#if defined(BUILD_LK)
	printf("%s, \n", __func__);
#elif defined(BUILD_UBOOT)
    printf("%s\n", __func__);
#else
    printk("lcm_compare_id \n");
#endif

return 1;

}


#if 0
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
#endif
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER yt40f51x6_wvga_ili9805_dsi_lcm_drv = 
{
    .name			= "ili9805_dsi",
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
//	.esd_check   = lcm_esd_check,
//    .esd_recover   = lcm_esd_recover,
//	.compare_id    = lcm_compare_id,
#endif
};
