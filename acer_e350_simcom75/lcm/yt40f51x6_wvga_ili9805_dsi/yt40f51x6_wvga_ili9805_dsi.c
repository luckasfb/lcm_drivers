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




// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
#ifdef BUILD_UBOOT
    printf("[LONG]lcm_set_util_funcs\r\n");
#else
   // printf("[LONG]lcm_set_util_funcs\r\n");
#endif
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{

#ifdef BUILD_UBOOT
    printf("[LONG]lcm_get_params\r\n");
#else
    printk("[LONG]lcm_get_params\r\n");
#endif
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
    MDELAY(120);
#ifdef BUILD_UBOOT
    printf("[LONG],lcm_init\r\n");
#else
    printk("[LONG],lcm_init\r\n");
#endif



	data_array[0]=0x00043902;

	data_array[1]=0x0598FFFF;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);

	//set 0xfd
	data_array[0]=0x00053902;

	data_array[1]=0x441F0FFD;

	data_array[2]=0x00000000;

	dsi_set_cmdq(&data_array, 3, 1);

	MDELAY(1);


	//SET 0XF8
	data_array[0] = 0x00103902; 

	data_array[1] = 0x030330F8; 

	data_array[2] = 0x30020230; 

	data_array[3] = 0x01300101; 

	data_array[4] = 0x01013001; 

	dsi_set_cmdq(&data_array, 5, 1);

	MDELAY(1);


	data_array[0]=0x00033902;

	data_array[1]=0x000F08EB;

	dsi_set_cmdq(&data_array, 2, 1);

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

	data_array[0]=0x00043902;

	data_array[1]=0x010F04FC;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);



	data_array[0]=0x00023902;

	data_array[1]=0x000019FE;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(400);

//
	data_array[0] = 0x00113902; 

	data_array[1] = 0x1D0B00E0; //0x201B00E0;

	data_array[2] = 0x0E1F1515; //0x7B1C100D;

	data_array[3] = 0x0104000E; //0x0303000C;

	data_array[4] = 0x36370902; //0x3E3F0B05; 

	data_array[5] = 0x00000000; //0x00000000;

	dsi_set_cmdq(&data_array, 6, 1);

	MDELAY(1);

//
	data_array[0] = 0x00113902; 

	data_array[1] = 0x140B00E1; //0x312500E1;

	data_array[2] = 0x0D1F1313; //0x7C181314; 

	data_array[3] = 0x0004000B; //0x0103000A; 

	data_array[4] = 0x171C0C11; //0x0F130610;

	data_array[5] = 0x00000000; //0x00000000;

	dsi_set_cmdq(&data_array, 6, 1);

	MDELAY(1);



	data_array[0]=0x00053902;

	data_array[1]=0x0e3017C1;//0x103013C1;

	data_array[2]=0x00000026;

	dsi_set_cmdq(&data_array, 3, 1);

	MDELAY(1);	



	data_array[0] = 0xb5C71500;//0xDCC71500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x00043902; // 

	data_array[1] = 0x141200B1; //

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(5);


	data_array[0] = 0x02B41500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(5);


	data_array[0] = 0x0A361500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(5);


	data_array[0] = 0x773A1500; // 24bit

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(5);


	data_array[0] = 0x00B01500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(5);


	data_array[0] = 0x01B61500;

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(5);


	data_array[0] = 0x00210500;	

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x00351500;	// TE

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);

/*  //del
	data_array[0] = 0x000D3902;	// CE function
	
data_array[1] = 0x505050C5;

	data_array[2] = 0x50505058;

	data_array[3] = 0x70788058;

	data_array[4] = 0x00000068;

	dsi_set_cmdq(&data_array, 5, 1);

	MDELAY(1);


	data_array[0] = 0x000D3902;	// CE function

	data_array[1] = 0x808080C6;

	data_array[2] = 0x80808080;

	data_array[3] = 0x68707880;

	data_array[4] = 0x00000060;

	dsi_set_cmdq(&data_array, 5, 1);

	MDELAY(1);
*/

	data_array[0]=0x00023902;

	data_array[1]=0x000011C2;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);

/*  //del
	data_array[0]=0x00043902;

	data_array[1]=0xFFFF00F9;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);
*/
		

	data_array[0] = 0x00110500;	// Sleep Out

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(120);
		

	data_array[0] = 0x00290500;	// Display On

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(20);
		

/* //del
	data_array[0]=0x00033902;

	data_array[1]=0x00FFA6F9;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);
		

	data_array[0]=0x00023902;

	data_array[1]=0x000000F9;

	dsi_set_cmdq(&data_array, 2, 1);

	MDELAY(1);
*/

	data_array[0] = 0x00511500;	// DBV[7:0] =0xFF

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x00531500;//0x2C531500;	// BCTRL=1,BL=1

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);



	data_array[0] = 0x00551500;//0x01551500;	// SET CABC UI MODE

	//data_array[0] = 0x00551500;	// SET CABC UI MODE

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


	data_array[0] = 0x005e1500;//0x705e1500;	// SET CABC UI MODE 5E

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(1);


}


static void lcm_suspend(void)
{
	unsigned int data_array[16];
#ifdef BUILD_UBOOT
    printf("[LONG],lcm_suspend\r\n");
#else
    printk("[LONG],lcm_suspend\r\n");
#endif

	data_array[0] = 0x00100500; // Sleep In

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(120);

}


static void lcm_resume(void)
{
	unsigned int data_array[16];
#ifdef BUILD_UBOOT
    printf("[LONG],lcm_resume\r\n");
#else
    printk("[LONG],lcm_resume\r\n");
#endif

	data_array[0]=0x00053902;

	data_array[1]=0x0e3017C1;//0x103013C1;

	data_array[2]=0x00000026;

	dsi_set_cmdq(&data_array, 3, 1);

	MDELAY(1);

	data_array[0] = 0x00110500; // Sleep Out

	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(120);	

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

#ifdef BUILD_UBOOT
    printf("[LONG],lcm_update,%d,%d,%d,%d\r\n",x,y,width,height);
#else
    printk("[LONG],lcm_update,%d,%d,%d,%d\r\n",x,y,width,height);
#endif


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

#ifdef BUILD_UBOOT
    printf("[LONG],lcm_compare_id\r\n");
#else
    printk("[LONG],lcm_compare_id\r\n");
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
