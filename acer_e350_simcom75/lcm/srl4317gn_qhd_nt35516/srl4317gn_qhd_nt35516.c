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


/*Extern func*/
#if defined(ACER_C8)
/*punk add for tp*/
extern void fts_chk_chr(void);
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------


#ifdef BUILD_UBOOT
#define LCD_PRINT	printf
#else
#define LCD_PRINT	printk
#endif


//#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

#if 0
#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)
#else
#define FRAME_WIDTH  (540)
#define FRAME_HEIGHT (960)
#endif
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


#define PARA_MAX	40
struct init_para_st
{
	unsigned short reg_start;
	unsigned short para_cnt;
	unsigned char parameters[PARA_MAX];
};

static struct init_para_st init_setting[] = 
{
	{
		0xff00,5,
		{
			0xaa,0x55,0x25,0x01,0x01,
		}
	},
	{
		0xF200,35,
		{
			0x00,	0x00,	0x4A,	0x0A,	0xA8,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
			0x00,	0x0B,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
			0x00,	0x00,	0x00,	0x00,	0x40,	0x01,	0x51,	0x00,	
			0x01,	0x00,	0x01,
		}
	},
	{
		0xF300,7,
		{
				0x02,	0x03,	0x07,	0x45,	0x88,	0xD1,	0x0D,
		}
	},
	{
		0xF000,5,
		{
			0x55,	0xAA,	0x52,	0x08,	0x00
		}
	},
	{
		0xB100,3,
		{
			0xCC,	0x00,	0x00,
		}
	},
	{
		0xB800,4,
		{
			0x01,	0x02,	0x02,	0x02,
		}
	},
	{
		0xC900,6,
		{
			0x63,	0x06,	0x0D,	0x1A,	0x17,	0x00,
		}
	},
	{
		0xF000,5,
		{
			0x55,	0xAA,	0x52,	0x08,	0x01,
		}
	},
	{
		0xB000,3,
		{
			0x05,	0x05,	0x05,
		}
	},
	{
		0xB100,3,
		{
			0x05,	0x05,	0x05,
		}
	},
	{
		0xB200,3,
		{
			0x01,	0x01,	0x01,
		}
	},
	{
		0xB300,3,
		{
			0x0E,	0x0E,	0x0E,													
		}
	},
	{
		0xB400,3,
		{
			0x08,	0x08,	0x08,													
		}
	},
	{
		0xB600,3,
		{
			0x44,	0x44,	0x44,													
		}
	},
	{
		0xB700,3,
		{
			0x34,	0x34,	0x34,													
		}
	},
	{
		0xB800,3,
		{
			0x10,	0x10,	0x10,													
		}
	},
	{
		0xB900,3,
		{
			0x26,	0x26,	0x26,													
		}
	},
	{
		0xBA00,3,
		{
			0x34,	0x34,	0x34,													
		}
	},
	{
		0xBC00,3,
		{
			0x00,	0xC8,	0x00,													
		}
	},
	{
		0xBD00,3,
		{
			0x00,	0xC8,	0x00,													
		}
	},
	{
		0xBE00,1,
		{
			0x79,/*punk, 0x92->0x79*/
		}
	},
	{
		0xC000,2,
		{
			0x04,	0x00,														
		}
	},
	{
		0xCA00,1,
		{
			0x00,															
		}
	},
	{
		0xD000,4,
		{
			0x0A,	0x10,	0x0D,	0x0F,												
		}
	},
	{
		0xD100,16, 
		{
			0x00,	0x70,	0x00,	0xCE,	0x00,	0xF7,	0x01,	0x10,
			0x01,	0x21,	0x01,	0x44,	0x01,	0x62,	0x01,	0x8D,
		}
	},
	{
		0xD200,16,
		{
			0x01,	0xAF,	0x01,	0xE4,	0x02,	0x0C,	0x02,	0x4D,
			0x02,	0x82,	0x02,	0x84,	0x02,	0xB8,	0x02,	0xF0,
		}
	},
	{
		0xD300,16,
		{
			0x03,	0x14,	0x03,	0x42,	0x03,	0x5E,	0x03,	0x80,
			0x03,	0x97,	0x03,	0xB0,	0x03,	0xC0,	0x03,	0xDF,
		}
	},
	{
		0xD400,4,
		{
			0x03,	0xFD,	0x03,	0xFF,												
		}
	},
	{
		0xD500,16,
		{
			0x00,	0x70,	0x00,	0xCE,	0x00,	0xF7,	0x01,	0x10,
			0x01,	0x21,	0x01,	0x44,	0x01,	0x62,	0x01,	0x8D,
		}
	},
	{
		0xD600,16,
		{
			0x01,	0xAF,	0x01,	0xE4,	0x02,	0x0C,	0x02,	0x4D,
			0x02,	0x82,	0x02,	0x84,	0x02,	0xB8,	0x02,	0xF0,
		}
	},
	{
		0xD700,16,
		{
			0x03,	0x14,	0x03,	0x42,	0x03,	0x5E,	0x03,	0x80,
			0x03,	0x97,	0x03,	0xB0,	0x03,	0xC0,	0x03,	0xDF,
		}
	},
	{
		0xD800,4,
		{
			0x03,	0xFD,	0x03,	0xFF,												
		}
	},
	{
		0xD900,16,
		{
			0x00,	0x70,	0x00,	0xCE,	0x00,	0xF7,	0x01,	0x10,
			0x01,	0x21,	0x01,	0x44,	0x01,	0x62,	0x01,	0x8D,
		}
	},
	{
		0xDD00,16,
		{
			0x01,	0xAF,	0x01,	0xE4,	0x02,	0x0C,	0x02,	0x4D,	
			0x02,	0x82,	0x02,	0x84,	0x02,	0xB8,	0x02,	0xF0,
		}
	},
	{
		0xDE00,16,
		{
			0x03,	0x14,	0x03,	0x42,	0x03,	0x5E,	0x03,	0x80,
			0x03,	0x97,	0x03,	0xB0,	0x03,	0xC0,	0x03,	0xDF,
		}
	},
	{
		0xDF00,4,
		{
			0x03,	0xFD,	0x03,	0xFF,												
		}
	},
	{
		0xE000,16,
		{
			0x00,	0x70,	0x00,	0xCE,	0x00,	0xF7,	0x01,	0x10,
			0x01,	0x21,	0x01,	0x44,	0x01,	0x62,	0x01,	0x8D,
		}
	},
	{
		0xE100,16,
		{
			0x01,	0xAF,	0x01,	0xE4,	0x02,	0x0C,	0x02,	0x4D,	
			0x02,	0x82,	0x02,	0x84,	0x02,	0xB8,	0x02,	0xF0,
		}
	},
	{
		0xE200,16,
		{
			0x03,	0x14,	0x03,	0x42,	0x03,	0x5E,	0x03,	0x80,
			0x03,	0x97,	0x03,	0xB0,	0x03,	0xC0,	0x03,	0xDF,
		}
	},
	{
		0xE300,4,
		{
			0x03,	0xFD,	0x03,	0xFF,												
		}
	},
	{
		0xE400,16,
		{
			0x00,	0x70,	0x00,	0xCE,	0x00,	0xF7,	0x01,	0x10,
			0x01,	0x21,	0x01,	0x44,	0x01,	0x62,	0x01,	0x8D,
		}
	},
	{
		0xE500,16,
		{
			0x01,	0xAF,	0x01,	0xE4,	0x02,	0x0C,	0x02,	0x4D,
			0x02,	0x82,	0x02,	0x84,	0x02,	0xB8,	0x02,	0xF0,
		}
	},
	{
		0xE600,16,
		{
			0x03,	0x14,	0x03,	0x42,	0x03,	0x5E,	0x03,	0x80,	
			0x03,	0x97,	0x03,	0xB0,	0x03,	0xC0,	0x03,	0xDF,
		}
	},
	{
		0xE700,4,
		{
			0x03,	0xFD,	0x03,	0xFF,												
		}
	},
	{
		0xE800,16,
		{
			0x00,	0x70,	0x00,	0xCE,	0x00,	0xF7,	0x01,	0x10,
			0x01,	0x21,	0x01,	0x44,	0x01,	0x62,	0x01,	0x8D,
		}
	},
	{
		0xE900,16,
		{
			0x01,	0xAF,	0x01,	0xE4,	0x02,	0x0C,	0x02,	0x4D,	
			0x02,	0x82,	0x02,	0x84,	0x02,	0xB8,	0x02,	0xF0,
		}
	},
	{
		0xEA00,16,
		{
			0x03,	0x14,	0x03,	0x42,	0x03,	0x5E,	0x03,	0x80,
			0x03,	0x97,	0x03,	0xB0,	0x03,	0xC0,	0x03,	0xDF,
		}
	},
	{
		0xEB00,4,
		{
			0x03,	0xFD,	0x03,	0xFF,												
		}
	}
};


static int init_setting_num = sizeof(init_setting)/sizeof(init_setting[0]);

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

static __inline void send_ctrl_cmd(unsigned int cmd)
{
    unsigned char high_8bit = (cmd>>8)&0x00FF;
	unsigned char low_8bit = cmd&0x00FF;
    spi_send_data(0x2000|high_8bit);
	spi_send_data(0x0000|low_8bit);
}

static __inline void send_data_cmd(unsigned int data)
{
    //unsigned char high_8bit = (data>>8)&0x00FF;
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
	int i,k;

#if 0
		LCD_PRINT("%s,%s, -0 \n",__FILE__,__FUNCTION__);
	
		for(i=0; i<init_setting_num; i++)
		{
			for(k=0; k<init_setting[i].para_cnt; k++)
			{
				send_ctrl_cmd(init_setting[i].reg_start + k);
				send_data_cmd(init_setting[i].parameters[k]);
				if(init_setting[i].para_cnt<16)
				{
					LCD_PRINT("punk, lcd init, reg=%x, data=%x\n",init_setting[i].reg_start + k,
							init_setting[i].parameters[k]);
				}
			}
		}



	#ifdef DEBUG_FLICKER
		send_ctrl_cmd(0xBE00);
		send_data_cmd(regbe00);
	#endif

		send_ctrl_cmd(0x1100);
		MDELAY(150);
		send_ctrl_cmd(0x2c00);
		send_ctrl_cmd(0x1300);
		send_ctrl_cmd(0x2900);
		xx
  
#else
    LCD_PRINT("punk,init_lcm_registers  -1.3 sunrise code\n");
	//*************************************
	// Select CMD3               
	//*************************************
	set_lcm_register(0xFF00, 0xAA);
	set_lcm_register(0xFF01, 0x55);
	set_lcm_register(0xFF02, 0x25);
	set_lcm_register(0xFF03, 0x01);
	 
	set_lcm_register(0xF200,0x00);
	set_lcm_register(0xF201,0x00);
	set_lcm_register(0xF202,0x4A);
	set_lcm_register(0xF203,0x0A);
	set_lcm_register(0xF204,0xA8);
	set_lcm_register(0xF205,0x00);
	set_lcm_register(0xF206,0x00);
	set_lcm_register(0xF207,0x00);
	set_lcm_register(0xF208,0x00);
	set_lcm_register(0xF209,0x00);
	set_lcm_register(0xF20A,0x00);
	set_lcm_register(0xF20B,0x00);
	set_lcm_register(0xF20C,0x00);
	set_lcm_register(0xF20D,0x00);
	set_lcm_register(0xF20E,0x00);
	set_lcm_register(0xF20F,0x00);
	set_lcm_register(0xF210,0x00);
	set_lcm_register(0xF211,0x0B);
	set_lcm_register(0xF212,0x00);
	set_lcm_register(0xF213,0x00);
	set_lcm_register(0xF214,0x00);
	set_lcm_register(0xF215,0x00);
	set_lcm_register(0xF216,0x00);
	set_lcm_register(0xF217,0x00);
	set_lcm_register(0xF218,0x00);
	set_lcm_register(0xF219,0x00);
	set_lcm_register(0xF21A,0x00);
	set_lcm_register(0xF21B,0x00);
	set_lcm_register(0xF21C,0x40);
	set_lcm_register(0xF21D,0x01);
	set_lcm_register(0xF21E,0x51);
	set_lcm_register(0xF21F,0x00);
	set_lcm_register(0xF220,0x01);
	set_lcm_register(0xF221,0x00);
	set_lcm_register(0xF222,0x01);
	 
	set_lcm_register(0xF300,0x02);
	set_lcm_register(0xF301,0x03);
	set_lcm_register(0xF302,0x07);
	set_lcm_register(0xF303,0x45);
	set_lcm_register(0xF304,0x88);
	set_lcm_register(0xF305,0xD1);
	set_lcm_register(0xF306,0x0D);
	 
	set_lcm_register(0xF000, 0x55);
	set_lcm_register(0xF001, 0xAA);
	set_lcm_register(0xF002, 0x52);
	set_lcm_register(0xF003, 0x08);
	set_lcm_register(0xF004, 0x00);
	 
	set_lcm_register(0xB100, 0xCC);
	set_lcm_register(0xB101, 0x00);
	set_lcm_register(0xB102, 0x00);
	 
	set_lcm_register(0xBB00, 0x8A);
	set_lcm_register(0xBB01, 0x0A);
	set_lcm_register(0xBB02, 0x8A);
	 
	set_lcm_register(0xB800, 0x01);
	set_lcm_register(0xB801, 0x02);
	set_lcm_register(0xB802, 0x02);
	set_lcm_register(0xB803, 0x02);
	 
	//punk set_lcm_register(0xB600, 0x02);
	// Set GOA characteristics
	set_lcm_register(0xC900, 0x63);//0X43
	set_lcm_register(0xC901, 0x06);
	set_lcm_register(0xC902, 0x0D);
	set_lcm_register(0xC903, 0x1A);
	set_lcm_register(0xC904, 0x17);
	set_lcm_register(0xC905, 0x00);
	 
	//**********************************
	// Select CMD2, Page 1
	//**********************************
	set_lcm_register(0xF000, 0x55);
	set_lcm_register(0xF001, 0xAA);
	set_lcm_register(0xF002, 0x52);
	set_lcm_register(0xF003, 0x08);
	set_lcm_register(0xF004, 0x01);
	 
	set_lcm_register(0xB000, 0x05);
	set_lcm_register(0xB001, 0x05);
	set_lcm_register(0xB002, 0x05);
	 
	set_lcm_register(0xB100, 0x05);
	set_lcm_register(0xB101, 0x05);
	set_lcm_register(0xB102, 0x05);
	 
	set_lcm_register(0xB200, 0x01);
	set_lcm_register(0xB201, 0x01);
	set_lcm_register(0xB202, 0x01);
	 
	set_lcm_register(0xB300, 0x0E);
	set_lcm_register(0xB301, 0x0E);
	set_lcm_register(0xB302, 0x0E);
	 
	set_lcm_register(0xB400, 0x0A);
	set_lcm_register(0xB401, 0x0A);
	set_lcm_register(0xB402, 0x0A);
	 
	set_lcm_register(0xB600, 0x44);
	set_lcm_register(0xB601, 0x44);
	set_lcm_register(0xB602, 0x44);
	 
	set_lcm_register(0xB700, 0x34);
	set_lcm_register(0xB701, 0x34);
	set_lcm_register(0xB702, 0x34);
	 
	set_lcm_register(0xB800, 0x20);
	set_lcm_register(0xB801, 0x20);
	set_lcm_register(0xB802, 0x20);
	 
	set_lcm_register(0xB900, 0x24);
	set_lcm_register(0xB901, 0x24);
	set_lcm_register(0xB902, 0x24);
	 
	set_lcm_register(0xBA00, 0x14);
	set_lcm_register(0xBA01, 0x14);
	set_lcm_register(0xBA02, 0x14);
	 
	set_lcm_register(0xBC00, 0x00);
	set_lcm_register(0xBC01, 0xC8);
	set_lcm_register(0xBC02, 0x00);
	 
	set_lcm_register(0xBD00, 0x00);
	set_lcm_register(0xBD01, 0xC8);
	set_lcm_register(0xBD02, 0x00);
	 
#ifdef DEBUG_FLICKER
	set_lcm_register(0xBE00, regbe00);//0X92
	LCD_PRINT("punk,init_lcm_registers  -1.4, regbe00=%x\n ", regbe00);
#else
	set_lcm_register(0xBE00, 0x71);
#endif
	 
	set_lcm_register(0xC000, 0x04);
	set_lcm_register(0xC001, 0x00);
	 
	set_lcm_register(0xCA00, 0x00);
	 
	set_lcm_register(0xD000, 0x0A);
	set_lcm_register(0xD001, 0x10);
	set_lcm_register(0xD002, 0x0D);
	set_lcm_register(0xD003, 0x0F);
	 
	// Gamma for R+
	set_lcm_register(0xD100, 0x00);
	set_lcm_register(0xD101, 0x70);
	set_lcm_register(0xD102, 0x00);
	set_lcm_register(0xD103, 0xCE);
	set_lcm_register(0xD104, 0x00);
	set_lcm_register(0xD105, 0xF7);
	set_lcm_register(0xD106, 0x01);
	set_lcm_register(0xD107, 0x10);
	set_lcm_register(0xD108, 0x01);
	set_lcm_register(0xD109, 0x21);
	set_lcm_register(0xD10A, 0x01);
	set_lcm_register(0xD10B, 0x44);
	set_lcm_register(0xD10C, 0x01);
	set_lcm_register(0xD10D, 0x62);
	set_lcm_register(0xD10E, 0x01);
	set_lcm_register(0xD10F, 0x8D);
	 
	set_lcm_register(0xD200, 0x01);
	set_lcm_register(0xD201, 0xAF);
	set_lcm_register(0xD202, 0x01);
	set_lcm_register(0xD203, 0xE4);
	set_lcm_register(0xD204, 0x02);
	set_lcm_register(0xD205, 0x0C);
	set_lcm_register(0xD206, 0x02);
	set_lcm_register(0xD207, 0x4D);
	set_lcm_register(0xD208, 0x02);
	set_lcm_register(0xD209, 0x82);
	set_lcm_register(0xD20A, 0x02);
	set_lcm_register(0xD20B, 0x84);
	set_lcm_register(0xD20C, 0x02);
	set_lcm_register(0xD20D, 0xB8);
	set_lcm_register(0xD20E, 0x02);
	set_lcm_register(0xD20F, 0xF0);
	 
	set_lcm_register(0xD300, 0x03);
	set_lcm_register(0xD301, 0x14);
	set_lcm_register(0xD302, 0x03);
	set_lcm_register(0xD303, 0x42);
	set_lcm_register(0xD304, 0x03);
	set_lcm_register(0xD305, 0x5E);
	set_lcm_register(0xD306, 0x03);
	set_lcm_register(0xD307, 0x80);
	set_lcm_register(0xD308, 0x03);
	set_lcm_register(0xD309, 0x97);
	set_lcm_register(0xD30A, 0x03);
	set_lcm_register(0xD30B, 0xB0);
	set_lcm_register(0xD30C, 0x03);
	set_lcm_register(0xD30D, 0xC0);
	set_lcm_register(0xD30E, 0x03);
	set_lcm_register(0xD30F, 0xDF);
	 
	set_lcm_register(0xD400, 0x03);
	set_lcm_register(0xD401, 0xFD);
	set_lcm_register(0xD402, 0x03);
	set_lcm_register(0xD403, 0xFF);
	 
	// Gamma for G+
	set_lcm_register(0xD500, 0x00);
	set_lcm_register(0xD501, 0x70);
	set_lcm_register(0xD502, 0x00);
	set_lcm_register(0xD503, 0xCE);
	set_lcm_register(0xD504, 0x00);
	set_lcm_register(0xD505, 0xF7);
	set_lcm_register(0xD506, 0x01);
	set_lcm_register(0xD507, 0x10);
	set_lcm_register(0xD508, 0x01);
	set_lcm_register(0xD509, 0x21);
	set_lcm_register(0xD50A, 0x01);
	set_lcm_register(0xD50B, 0x44);
	set_lcm_register(0xD50C, 0x01);
	set_lcm_register(0xD50D, 0x62);
	set_lcm_register(0xD50E, 0x01);
	set_lcm_register(0xD50F, 0x8D);
	 
	set_lcm_register(0xD600, 0x01);
	set_lcm_register(0xD601, 0xAF);
	set_lcm_register(0xD602, 0x01);
	set_lcm_register(0xD603, 0xE4);
	set_lcm_register(0xD604, 0x02);
	set_lcm_register(0xD605, 0x0C);
	set_lcm_register(0xD606, 0x02);
	set_lcm_register(0xD607, 0x4D);
	set_lcm_register(0xD608, 0x02);
	set_lcm_register(0xD609, 0x82);
	set_lcm_register(0xD60A, 0x02);
	set_lcm_register(0xD60B, 0x84);
	set_lcm_register(0xD60C, 0x02);
	set_lcm_register(0xD60D, 0xB8);
	set_lcm_register(0xD60E, 0x02);
	set_lcm_register(0xD60F, 0xF0);
	 
	set_lcm_register(0xD700, 0x03);
	set_lcm_register(0xD701, 0x14);
	set_lcm_register(0xD702, 0x03);
	set_lcm_register(0xD703, 0x42);
	set_lcm_register(0xD704, 0x03);
	set_lcm_register(0xD705, 0x5E);
	set_lcm_register(0xD706, 0x03);
	set_lcm_register(0xD707, 0x80);
	set_lcm_register(0xD708, 0x03);
	set_lcm_register(0xD709, 0x97);
	set_lcm_register(0xD70A, 0x03);
	set_lcm_register(0xD70B, 0xB0);
	set_lcm_register(0xD70C, 0x03);
	set_lcm_register(0xD70D, 0xC0);
	set_lcm_register(0xD70E, 0x03);
	set_lcm_register(0xD70F, 0xDF);
	 
	set_lcm_register(0xD800, 0x03);
	set_lcm_register(0xD801, 0xFD);
	set_lcm_register(0xD802, 0x03);
	set_lcm_register(0xD803, 0xFF);
	 
	// Gamma for B+
	set_lcm_register(0xD900, 0x00);
	set_lcm_register(0xD901, 0x70);
	set_lcm_register(0xD902, 0x00);
	set_lcm_register(0xD903, 0xCE);
	set_lcm_register(0xD904, 0x00);
	set_lcm_register(0xD905, 0xF7);
	set_lcm_register(0xD906, 0x01);
	set_lcm_register(0xD907, 0x10);
	set_lcm_register(0xD908, 0x01);
	set_lcm_register(0xD909, 0x21);
	set_lcm_register(0xD90A, 0x01);
	set_lcm_register(0xD90B, 0x44);
	set_lcm_register(0xD90C, 0x01);
	set_lcm_register(0xD90D, 0x62);
	set_lcm_register(0xD90E, 0x01);
	set_lcm_register(0xD90F, 0x8D);
	 
	set_lcm_register(0xDD00, 0x01);
	set_lcm_register(0xDD01, 0xAF);
	set_lcm_register(0xDD02, 0x01);
	set_lcm_register(0xDD03, 0xE4);
	set_lcm_register(0xDD04, 0x02);
	set_lcm_register(0xDD05, 0x0C);
	set_lcm_register(0xDD06, 0x02);
	set_lcm_register(0xDD07, 0x4D);
	set_lcm_register(0xDD08, 0x02);
	set_lcm_register(0xDD09, 0x82);
	set_lcm_register(0xDD0A, 0x02);
	set_lcm_register(0xDD0B, 0x84);
	set_lcm_register(0xDD0C, 0x02);
	set_lcm_register(0xDD0D, 0xB8);
	set_lcm_register(0xDD0E, 0x02);
	set_lcm_register(0xDD0F, 0xF0);
	 
	set_lcm_register(0xDE00, 0x03);
	set_lcm_register(0xDE01, 0x14);
	set_lcm_register(0xDE02, 0x03);
	set_lcm_register(0xDE03, 0x42);
	set_lcm_register(0xDE04, 0x03);
	set_lcm_register(0xDE05, 0x5E);
	set_lcm_register(0xDE06, 0x03);
	set_lcm_register(0xDE07, 0x80);
	set_lcm_register(0xDE08, 0x03);
	set_lcm_register(0xDE09, 0x97);
	set_lcm_register(0xDE0A, 0x03);
	set_lcm_register(0xDE0B, 0xB0);
	set_lcm_register(0xDE0C, 0x03);
	set_lcm_register(0xDE0D, 0xC0);
	set_lcm_register(0xDE0E, 0x03);
	set_lcm_register(0xDE0F, 0xDF);
	 
	set_lcm_register(0xDF00, 0x03);
	set_lcm_register(0xDF01, 0xFD);
	set_lcm_register(0xDF02, 0x03);
	set_lcm_register(0xDF03, 0xFF);
	 
	// Gamma for R-
	set_lcm_register(0xE000, 0x00);
	set_lcm_register(0xE001, 0x70);
	set_lcm_register(0xE002, 0x00);
	set_lcm_register(0xE003, 0xCE);
	set_lcm_register(0xE004, 0x00);
	set_lcm_register(0xE005, 0xF7);
	set_lcm_register(0xE006, 0x01);
	set_lcm_register(0xE007, 0x10);
	set_lcm_register(0xE008, 0x01);
	set_lcm_register(0xE009, 0x21);
	set_lcm_register(0xE00A, 0x01);
	set_lcm_register(0xE00B, 0x44);
	set_lcm_register(0xE00C, 0x01);
	set_lcm_register(0xE00D, 0x62);
	set_lcm_register(0xE00E, 0x01);
	set_lcm_register(0xE00F, 0x8D);
	 
	set_lcm_register(0xE100, 0x01);
	set_lcm_register(0xE101, 0xAF);
	set_lcm_register(0xE102, 0x01);
	set_lcm_register(0xE103, 0xE4);
	set_lcm_register(0xE104, 0x02);
	set_lcm_register(0xE105, 0x0C);
	set_lcm_register(0xE106, 0x02);
	set_lcm_register(0xE107, 0x4D);
	set_lcm_register(0xE108, 0x02);
	set_lcm_register(0xE109, 0x82);
	set_lcm_register(0xE10A, 0x02);
	set_lcm_register(0xE10B, 0x84);
	set_lcm_register(0xE10C, 0x02);
	set_lcm_register(0xE10D, 0xB8);
	set_lcm_register(0xE10E, 0x02);
	set_lcm_register(0xE10F, 0xF0);
	 
	set_lcm_register(0xE200, 0x03);
	set_lcm_register(0xE201, 0x14);
	set_lcm_register(0xE202, 0x03);
	set_lcm_register(0xE203, 0x42);
	set_lcm_register(0xE204, 0x03);
	set_lcm_register(0xE205, 0x5E);
	set_lcm_register(0xE206, 0x03);
	set_lcm_register(0xE207, 0x80);
	set_lcm_register(0xE208, 0x03);
	set_lcm_register(0xE209, 0x97);
	set_lcm_register(0xE20A, 0x03);
	set_lcm_register(0xE20B, 0xB0);
	set_lcm_register(0xE20C, 0x03);
	set_lcm_register(0xE20D, 0xC0);
	set_lcm_register(0xE20E, 0x03);
	set_lcm_register(0xE20F, 0xDF);
	 
	set_lcm_register(0xE300, 0x03);
	set_lcm_register(0xE301, 0xFD);
	set_lcm_register(0xE302, 0x03);
	set_lcm_register(0xE303, 0xFF);
	 
	// Gamma for G-
	set_lcm_register(0xE400, 0x00);
	set_lcm_register(0xE401, 0x70);
	set_lcm_register(0xE402, 0x00);
	set_lcm_register(0xE403, 0xCE);
	set_lcm_register(0xE404, 0x00);
	set_lcm_register(0xE405, 0xF7);
	set_lcm_register(0xE406, 0x01);
	set_lcm_register(0xE407, 0x10);
	set_lcm_register(0xE408, 0x01);
	set_lcm_register(0xE409, 0x21);
	set_lcm_register(0xE40A, 0x01);
	set_lcm_register(0xE40B, 0x44);
	set_lcm_register(0xE40C, 0x01);
	set_lcm_register(0xE40D, 0x62);
	set_lcm_register(0xE40E, 0x01);
	set_lcm_register(0xE40F, 0x8D);
	 
	set_lcm_register(0xE500, 0x01);
	set_lcm_register(0xE501, 0xAF);
	set_lcm_register(0xE502, 0x01);
	set_lcm_register(0xE503, 0xE4);
	set_lcm_register(0xE504, 0x02);
	set_lcm_register(0xE505, 0x0C);
	set_lcm_register(0xE506, 0x02);
	set_lcm_register(0xE507, 0x4D);
	set_lcm_register(0xE508, 0x02);
	set_lcm_register(0xE509, 0x82);
	set_lcm_register(0xE50A, 0x02);
	set_lcm_register(0xE50B, 0x84);
	set_lcm_register(0xE50C, 0x02);
	set_lcm_register(0xE50D, 0xB8);
	set_lcm_register(0xE50E, 0x02);
	set_lcm_register(0xE50F, 0xF0);
	 
	set_lcm_register(0xE600, 0x03);
	set_lcm_register(0xE601, 0x14);
	set_lcm_register(0xE602, 0x03);
	set_lcm_register(0xE603, 0x42);
	set_lcm_register(0xE604, 0x03);
	set_lcm_register(0xE605, 0x5E);
	set_lcm_register(0xE606, 0x03);
	set_lcm_register(0xE607, 0x80);
	set_lcm_register(0xE608, 0x03);
	set_lcm_register(0xE609, 0x97);
	set_lcm_register(0xE60A, 0x03);
	set_lcm_register(0xE60B, 0xB0);
	set_lcm_register(0xE60C, 0x03);
	set_lcm_register(0xE60D, 0xC0);
	set_lcm_register(0xE60E, 0x03);
	set_lcm_register(0xE60F, 0xDF);
	 
	set_lcm_register(0xE700, 0x03);
	set_lcm_register(0xE701, 0xFD);
	set_lcm_register(0xE702, 0x03);
	set_lcm_register(0xE703, 0xFF);
	 
	// Gamma for B-
	set_lcm_register(0xE800, 0x00);
	set_lcm_register(0xE801, 0x70);
	set_lcm_register(0xE802, 0x00);
	set_lcm_register(0xE803, 0xCE);
	set_lcm_register(0xE804, 0x00);
	set_lcm_register(0xE805, 0xF7);
	set_lcm_register(0xE806, 0x01);
	set_lcm_register(0xE807, 0x10);
	set_lcm_register(0xE808, 0x01);
	set_lcm_register(0xE809, 0x21);
	set_lcm_register(0xE80A, 0x01);
	set_lcm_register(0xE80B, 0x44);
	set_lcm_register(0xE80C, 0x01);
	set_lcm_register(0xE80D, 0x62);
	set_lcm_register(0xE80E, 0x01);
	set_lcm_register(0xE80F, 0x8D);
	 
	set_lcm_register(0xE900, 0x01);
	set_lcm_register(0xE901, 0xAF);
	set_lcm_register(0xE902, 0x01);
	set_lcm_register(0xE903, 0xE4);
	set_lcm_register(0xE904, 0x02);
	set_lcm_register(0xE905, 0x0C);
	set_lcm_register(0xE906, 0x02);
	set_lcm_register(0xE907, 0x4D);
	set_lcm_register(0xE908, 0x02);
	set_lcm_register(0xE909, 0x82);
	set_lcm_register(0xE90A, 0x02);
	set_lcm_register(0xE90B, 0x84);
	set_lcm_register(0xE90C, 0x02);
	set_lcm_register(0xE90D, 0xB8);
	set_lcm_register(0xE90E, 0x02);
	set_lcm_register(0xE90F, 0xF0);
	 
	set_lcm_register(0xEA00, 0x03);
	set_lcm_register(0xEA01, 0x14);
	set_lcm_register(0xEA02, 0x03);
	set_lcm_register(0xEA03, 0x42);
	set_lcm_register(0xEA04, 0x03);
	set_lcm_register(0xEA05, 0x5E);
	set_lcm_register(0xEA06, 0x03);
	set_lcm_register(0xEA07, 0x80);
	set_lcm_register(0xEA08, 0x03);
	set_lcm_register(0xEA09, 0x97);
	set_lcm_register(0xEA0A, 0x03);
	set_lcm_register(0xEA0B, 0xB0);
	set_lcm_register(0xEA0C, 0x03);
	set_lcm_register(0xEA0D, 0xC0);
	set_lcm_register(0xEA0E, 0x03);
	set_lcm_register(0xEA0F, 0xDF);
	 
	set_lcm_register(0xEB00, 0x03);
	set_lcm_register(0xEB01, 0xFD);
	set_lcm_register(0xEB02, 0x03);
	set_lcm_register(0xEB03, 0xFF);
	 
	set_lcm_register(0x3600, 0x00);
	//**********************************
	// Sleep Out
	//**********************************
	set_lcm_register(0x1100, 0x00);
	MDELAY(150);
	//**********************************
	// Display On
	//**********************************
	set_lcm_register(0x2900, 0x00); 

#endif
}

static void init_lcm_registers_nt35510(void)
{
	LCD_PRINT("init_lcm_registers_nt35510 -1\n");
//ENABLE PAGE 1
send_ctrl_cmd(0xF000);send_data_cmd(0x55);
send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
send_ctrl_cmd(0xF002);send_data_cmd(0x52);
send_ctrl_cmd(0xF003);send_data_cmd(0x08);
send_ctrl_cmd(0xF004);send_data_cmd(0x00);

send_ctrl_cmd(0xB100);send_data_cmd(0x0C);
send_ctrl_cmd(0xB101);send_data_cmd(0x00);

//VGMP/VGMN/VCOM SETING
send_ctrl_cmd(0xBC00);send_data_cmd(0x05);
send_ctrl_cmd(0xBC01);send_data_cmd(0x05);
send_ctrl_cmd(0xBC02);send_data_cmd(0x05);

send_ctrl_cmd(0xF000);send_data_cmd(0x55);
send_ctrl_cmd(0xF001);send_data_cmd(0xAA);
send_ctrl_cmd(0xF002);send_data_cmd(0x52);
send_ctrl_cmd(0xF003);send_data_cmd(0x08);
send_ctrl_cmd(0xF004);send_data_cmd(0x01);

//BGH VOLTAGE SETTING
send_ctrl_cmd(0xB900);send_data_cmd(0x24);
send_ctrl_cmd(0xB901);send_data_cmd(0x24);
send_ctrl_cmd(0xB902);send_data_cmd(0x24);

//VGLX VOLTAGE SETTING
send_ctrl_cmd(0xBA00);send_data_cmd(0x24);
send_ctrl_cmd(0xBA01);send_data_cmd(0x24);
send_ctrl_cmd(0xBA02);send_data_cmd(0x24);

send_ctrl_cmd(0xBE00);send_data_cmd(0x00);//40
send_ctrl_cmd(0xBE01);send_data_cmd(0x78);//78

//Z-INVERSION
send_ctrl_cmd(0xBC00);send_data_cmd(0x00);
send_ctrl_cmd(0xBC01);send_data_cmd(0x88);
send_ctrl_cmd(0xBC02);send_data_cmd(0x01);

send_ctrl_cmd(0xBD00);send_data_cmd(0x00);
send_ctrl_cmd(0xBD01);send_data_cmd(0x88);
send_ctrl_cmd(0xBD02);send_data_cmd(0x01);

//GAMMA SETING  RED
send_ctrl_cmd(0xD100);send_data_cmd(0x00);
send_ctrl_cmd(0xD101);send_data_cmd(0x00);
send_ctrl_cmd(0xD102);send_data_cmd(0x00);
send_ctrl_cmd(0xD103);send_data_cmd(0x16);
send_ctrl_cmd(0xD104);send_data_cmd(0x00);
send_ctrl_cmd(0xD105);send_data_cmd(0x42);
send_ctrl_cmd(0xD106);send_data_cmd(0x00);
send_ctrl_cmd(0xD107);send_data_cmd(0x61);
send_ctrl_cmd(0xD108);send_data_cmd(0x00);
send_ctrl_cmd(0xD109);send_data_cmd(0x74);
send_ctrl_cmd(0xD10A);send_data_cmd(0x00);
send_ctrl_cmd(0xD10B);send_data_cmd(0x97);
send_ctrl_cmd(0xD10C);send_data_cmd(0x00);
send_ctrl_cmd(0xD10D);send_data_cmd(0xAD);
send_ctrl_cmd(0xD10E);send_data_cmd(0x00);
send_ctrl_cmd(0xD10F);send_data_cmd(0xDE);
send_ctrl_cmd(0xD110);send_data_cmd(0x01);
send_ctrl_cmd(0xD111);send_data_cmd(0x00);
send_ctrl_cmd(0xD112);send_data_cmd(0x01);
send_ctrl_cmd(0xD113);send_data_cmd(0x26);
send_ctrl_cmd(0xD114);send_data_cmd(0x01);
send_ctrl_cmd(0xD115);send_data_cmd(0x50);
send_ctrl_cmd(0xD116);send_data_cmd(0x01);
send_ctrl_cmd(0xD117);send_data_cmd(0x87);
send_ctrl_cmd(0xD118);send_data_cmd(0x01);
send_ctrl_cmd(0xD119);send_data_cmd(0xB3);
send_ctrl_cmd(0xD11A);send_data_cmd(0x01);
send_ctrl_cmd(0xD11B);send_data_cmd(0xB6);
send_ctrl_cmd(0xD11C);send_data_cmd(0x01);
send_ctrl_cmd(0xD11D);send_data_cmd(0xDC);
send_ctrl_cmd(0xD11E);send_data_cmd(0x02);
send_ctrl_cmd(0xD11F);send_data_cmd(0x04);
send_ctrl_cmd(0xD120);send_data_cmd(0x02);
send_ctrl_cmd(0xD121);send_data_cmd(0x1C);
send_ctrl_cmd(0xD122);send_data_cmd(0x02);
send_ctrl_cmd(0xD123);send_data_cmd(0x34);
send_ctrl_cmd(0xD124);send_data_cmd(0x02);
send_ctrl_cmd(0xD125);send_data_cmd(0x4E);
send_ctrl_cmd(0xD126);send_data_cmd(0x02);
send_ctrl_cmd(0xD127);send_data_cmd(0x8A);
send_ctrl_cmd(0xD128);send_data_cmd(0x02);
send_ctrl_cmd(0xD129);send_data_cmd(0xC2);
send_ctrl_cmd(0xD12A);send_data_cmd(0x03);
send_ctrl_cmd(0xD12B);send_data_cmd(0x04);
send_ctrl_cmd(0xD12C);send_data_cmd(0x03);
send_ctrl_cmd(0xD12D);send_data_cmd(0x2E);
send_ctrl_cmd(0xD12E);send_data_cmd(0x03);
send_ctrl_cmd(0xD12F);send_data_cmd(0x74);
send_ctrl_cmd(0xD130);send_data_cmd(0x03);
send_ctrl_cmd(0xD131);send_data_cmd(0xEB);
send_ctrl_cmd(0xD132);send_data_cmd(0x03);
send_ctrl_cmd(0xD133);send_data_cmd(0xFF);

//GAMMA SETING  RED
send_ctrl_cmd(0xD200);send_data_cmd(0x00);
send_ctrl_cmd(0xD201);send_data_cmd(0x00);
send_ctrl_cmd(0xD202);send_data_cmd(0x00);
send_ctrl_cmd(0xD203);send_data_cmd(0x16);
send_ctrl_cmd(0xD204);send_data_cmd(0x00);
send_ctrl_cmd(0xD205);send_data_cmd(0x42);
send_ctrl_cmd(0xD206);send_data_cmd(0x00);
send_ctrl_cmd(0xD207);send_data_cmd(0x61);
send_ctrl_cmd(0xD208);send_data_cmd(0x00);
send_ctrl_cmd(0xD209);send_data_cmd(0x74);
send_ctrl_cmd(0xD20A);send_data_cmd(0x00);
send_ctrl_cmd(0xD20B);send_data_cmd(0x97);
send_ctrl_cmd(0xD20C);send_data_cmd(0x00);
send_ctrl_cmd(0xD20D);send_data_cmd(0xAD);
send_ctrl_cmd(0xD20E);send_data_cmd(0x00);
send_ctrl_cmd(0xD20F);send_data_cmd(0xDE);
send_ctrl_cmd(0xD210);send_data_cmd(0x01);
send_ctrl_cmd(0xD211);send_data_cmd(0x00);
send_ctrl_cmd(0xD212);send_data_cmd(0x01);
send_ctrl_cmd(0xD213);send_data_cmd(0x26);
send_ctrl_cmd(0xD214);send_data_cmd(0x01);
send_ctrl_cmd(0xD215);send_data_cmd(0x50);
send_ctrl_cmd(0xD216);send_data_cmd(0x01);
send_ctrl_cmd(0xD217);send_data_cmd(0x87);
send_ctrl_cmd(0xD218);send_data_cmd(0x01);
send_ctrl_cmd(0xD219);send_data_cmd(0xB3);
send_ctrl_cmd(0xD21A);send_data_cmd(0x01);
send_ctrl_cmd(0xD21B);send_data_cmd(0xB6);
send_ctrl_cmd(0xD21C);send_data_cmd(0x01);
send_ctrl_cmd(0xD21D);send_data_cmd(0xDC);
send_ctrl_cmd(0xD21E);send_data_cmd(0x02);
send_ctrl_cmd(0xD21F);send_data_cmd(0x04);
send_ctrl_cmd(0xD220);send_data_cmd(0x02);
send_ctrl_cmd(0xD221);send_data_cmd(0x1C);
send_ctrl_cmd(0xD222);send_data_cmd(0x02);
send_ctrl_cmd(0xD223);send_data_cmd(0x34);
send_ctrl_cmd(0xD224);send_data_cmd(0x02);
send_ctrl_cmd(0xD225);send_data_cmd(0x4E);
send_ctrl_cmd(0xD226);send_data_cmd(0x02);
send_ctrl_cmd(0xD227);send_data_cmd(0x8A);
send_ctrl_cmd(0xD228);send_data_cmd(0x02);
send_ctrl_cmd(0xD229);send_data_cmd(0xC2);
send_ctrl_cmd(0xD22A);send_data_cmd(0x03);
send_ctrl_cmd(0xD22B);send_data_cmd(0x04);
send_ctrl_cmd(0xD22C);send_data_cmd(0x03);
send_ctrl_cmd(0xD22D);send_data_cmd(0x2E);
send_ctrl_cmd(0xD22E);send_data_cmd(0x03);
send_ctrl_cmd(0xD22F);send_data_cmd(0x74);
send_ctrl_cmd(0xD230);send_data_cmd(0x03);
send_ctrl_cmd(0xD231);send_data_cmd(0xEB);
send_ctrl_cmd(0xD232);send_data_cmd(0x03);
send_ctrl_cmd(0xD233);send_data_cmd(0xFF);


//GAMMA SETING  RED
send_ctrl_cmd(0xD300);send_data_cmd(0x00);
send_ctrl_cmd(0xD301);send_data_cmd(0x00);
send_ctrl_cmd(0xD302);send_data_cmd(0x00);
send_ctrl_cmd(0xD303);send_data_cmd(0x16);
send_ctrl_cmd(0xD304);send_data_cmd(0x00);
send_ctrl_cmd(0xD305);send_data_cmd(0x42);
send_ctrl_cmd(0xD306);send_data_cmd(0x00);
send_ctrl_cmd(0xD307);send_data_cmd(0x61);
send_ctrl_cmd(0xD308);send_data_cmd(0x00);
send_ctrl_cmd(0xD309);send_data_cmd(0x74);
send_ctrl_cmd(0xD30A);send_data_cmd(0x00);
send_ctrl_cmd(0xD30B);send_data_cmd(0x97);
send_ctrl_cmd(0xD30C);send_data_cmd(0x00);
send_ctrl_cmd(0xD30D);send_data_cmd(0xAD);
send_ctrl_cmd(0xD30E);send_data_cmd(0x00);
send_ctrl_cmd(0xD30F);send_data_cmd(0xDE);
send_ctrl_cmd(0xD310);send_data_cmd(0x01);
send_ctrl_cmd(0xD311);send_data_cmd(0x00);
send_ctrl_cmd(0xD312);send_data_cmd(0x01);
send_ctrl_cmd(0xD313);send_data_cmd(0x26);
send_ctrl_cmd(0xD314);send_data_cmd(0x01);
send_ctrl_cmd(0xD315);send_data_cmd(0x50);
send_ctrl_cmd(0xD316);send_data_cmd(0x01);
send_ctrl_cmd(0xD317);send_data_cmd(0x87);
send_ctrl_cmd(0xD318);send_data_cmd(0x01);
send_ctrl_cmd(0xD319);send_data_cmd(0xB3);
send_ctrl_cmd(0xD31A);send_data_cmd(0x01);
send_ctrl_cmd(0xD31B);send_data_cmd(0xB6);
send_ctrl_cmd(0xD31C);send_data_cmd(0x01);
send_ctrl_cmd(0xD31D);send_data_cmd(0xDC);
send_ctrl_cmd(0xD31E);send_data_cmd(0x02);
send_ctrl_cmd(0xD31F);send_data_cmd(0x04);
send_ctrl_cmd(0xD320);send_data_cmd(0x02);
send_ctrl_cmd(0xD321);send_data_cmd(0x1C);
send_ctrl_cmd(0xD322);send_data_cmd(0x02);
send_ctrl_cmd(0xD323);send_data_cmd(0x34);
send_ctrl_cmd(0xD324);send_data_cmd(0x02);
send_ctrl_cmd(0xD325);send_data_cmd(0x4E);
send_ctrl_cmd(0xD326);send_data_cmd(0x02);
send_ctrl_cmd(0xD327);send_data_cmd(0x8A);
send_ctrl_cmd(0xD328);send_data_cmd(0x02);
send_ctrl_cmd(0xD329);send_data_cmd(0xC2);
send_ctrl_cmd(0xD32A);send_data_cmd(0x03);
send_ctrl_cmd(0xD32B);send_data_cmd(0x04);
send_ctrl_cmd(0xD32C);send_data_cmd(0x03);
send_ctrl_cmd(0xD32D);send_data_cmd(0x2E);
send_ctrl_cmd(0xD32E);send_data_cmd(0x03);
send_ctrl_cmd(0xD32F);send_data_cmd(0x74);
send_ctrl_cmd(0xD330);send_data_cmd(0x03);
send_ctrl_cmd(0xD331);send_data_cmd(0xEB);
send_ctrl_cmd(0xD332);send_data_cmd(0x03);
send_ctrl_cmd(0xD333);send_data_cmd(0xFF);


//GAMMA SETING  RED
send_ctrl_cmd(0xD400);send_data_cmd(0x00);
send_ctrl_cmd(0xD401);send_data_cmd(0x00);
send_ctrl_cmd(0xD402);send_data_cmd(0x00);
send_ctrl_cmd(0xD403);send_data_cmd(0x16);
send_ctrl_cmd(0xD404);send_data_cmd(0x00);
send_ctrl_cmd(0xD405);send_data_cmd(0x42);
send_ctrl_cmd(0xD406);send_data_cmd(0x00);
send_ctrl_cmd(0xD407);send_data_cmd(0x61);
send_ctrl_cmd(0xD408);send_data_cmd(0x00);
send_ctrl_cmd(0xD409);send_data_cmd(0x74);
send_ctrl_cmd(0xD40A);send_data_cmd(0x00);
send_ctrl_cmd(0xD40B);send_data_cmd(0x97);
send_ctrl_cmd(0xD40C);send_data_cmd(0x00);
send_ctrl_cmd(0xD40D);send_data_cmd(0xAD);
send_ctrl_cmd(0xD40E);send_data_cmd(0x00);
send_ctrl_cmd(0xD40F);send_data_cmd(0xDE);
send_ctrl_cmd(0xD410);send_data_cmd(0x01);
send_ctrl_cmd(0xD411);send_data_cmd(0x00);
send_ctrl_cmd(0xD412);send_data_cmd(0x01);
send_ctrl_cmd(0xD413);send_data_cmd(0x26);
send_ctrl_cmd(0xD414);send_data_cmd(0x01);
send_ctrl_cmd(0xD415);send_data_cmd(0x50);
send_ctrl_cmd(0xD416);send_data_cmd(0x01);
send_ctrl_cmd(0xD417);send_data_cmd(0x87);
send_ctrl_cmd(0xD418);send_data_cmd(0x01);
send_ctrl_cmd(0xD419);send_data_cmd(0xB3);
send_ctrl_cmd(0xD41A);send_data_cmd(0x01);
send_ctrl_cmd(0xD41B);send_data_cmd(0xB6);
send_ctrl_cmd(0xD41C);send_data_cmd(0x01);
send_ctrl_cmd(0xD41D);send_data_cmd(0xDC);
send_ctrl_cmd(0xD41E);send_data_cmd(0x02);
send_ctrl_cmd(0xD41F);send_data_cmd(0x04);
send_ctrl_cmd(0xD420);send_data_cmd(0x02);
send_ctrl_cmd(0xD421);send_data_cmd(0x1C);
send_ctrl_cmd(0xD422);send_data_cmd(0x02);
send_ctrl_cmd(0xD423);send_data_cmd(0x34);
send_ctrl_cmd(0xD424);send_data_cmd(0x02);
send_ctrl_cmd(0xD425);send_data_cmd(0x4E);
send_ctrl_cmd(0xD426);send_data_cmd(0x02);
send_ctrl_cmd(0xD427);send_data_cmd(0x8A);
send_ctrl_cmd(0xD428);send_data_cmd(0x02);
send_ctrl_cmd(0xD429);send_data_cmd(0xC2);
send_ctrl_cmd(0xD42A);send_data_cmd(0x03);
send_ctrl_cmd(0xD42B);send_data_cmd(0x04);
send_ctrl_cmd(0xD42C);send_data_cmd(0x03);
send_ctrl_cmd(0xD42D);send_data_cmd(0x2E);
send_ctrl_cmd(0xD42E);send_data_cmd(0x03);
send_ctrl_cmd(0xD42F);send_data_cmd(0x74);
send_ctrl_cmd(0xD430);send_data_cmd(0x03);
send_ctrl_cmd(0xD431);send_data_cmd(0xEB);
send_ctrl_cmd(0xD432);send_data_cmd(0x03);
send_ctrl_cmd(0xD433);send_data_cmd(0xFF);


//GAMMA SETING  RED
send_ctrl_cmd(0xD500);send_data_cmd(0x00);
send_ctrl_cmd(0xD501);send_data_cmd(0x00);
send_ctrl_cmd(0xD502);send_data_cmd(0x00);
send_ctrl_cmd(0xD503);send_data_cmd(0x16);
send_ctrl_cmd(0xD504);send_data_cmd(0x00);
send_ctrl_cmd(0xD505);send_data_cmd(0x42);
send_ctrl_cmd(0xD506);send_data_cmd(0x00);
send_ctrl_cmd(0xD507);send_data_cmd(0x61);
send_ctrl_cmd(0xD508);send_data_cmd(0x00);
send_ctrl_cmd(0xD509);send_data_cmd(0x74);
send_ctrl_cmd(0xD50A);send_data_cmd(0x00);
send_ctrl_cmd(0xD50B);send_data_cmd(0x97);
send_ctrl_cmd(0xD50C);send_data_cmd(0x00);
send_ctrl_cmd(0xD50D);send_data_cmd(0xAD);
send_ctrl_cmd(0xD50E);send_data_cmd(0x00);
send_ctrl_cmd(0xD50F);send_data_cmd(0xDE);
send_ctrl_cmd(0xD510);send_data_cmd(0x01);
send_ctrl_cmd(0xD511);send_data_cmd(0x00);
send_ctrl_cmd(0xD512);send_data_cmd(0x01);
send_ctrl_cmd(0xD513);send_data_cmd(0x26);
send_ctrl_cmd(0xD514);send_data_cmd(0x01);
send_ctrl_cmd(0xD515);send_data_cmd(0x50);
send_ctrl_cmd(0xD516);send_data_cmd(0x01);
send_ctrl_cmd(0xD517);send_data_cmd(0x87);
send_ctrl_cmd(0xD518);send_data_cmd(0x01);
send_ctrl_cmd(0xD519);send_data_cmd(0xB3);
send_ctrl_cmd(0xD51A);send_data_cmd(0x01);
send_ctrl_cmd(0xD51B);send_data_cmd(0xB6);
send_ctrl_cmd(0xD51C);send_data_cmd(0x01);
send_ctrl_cmd(0xD51D);send_data_cmd(0xDC);
send_ctrl_cmd(0xD51E);send_data_cmd(0x02);
send_ctrl_cmd(0xD51F);send_data_cmd(0x04);
send_ctrl_cmd(0xD520);send_data_cmd(0x02);
send_ctrl_cmd(0xD521);send_data_cmd(0x1C);
send_ctrl_cmd(0xD522);send_data_cmd(0x02);
send_ctrl_cmd(0xD523);send_data_cmd(0x34);
send_ctrl_cmd(0xD524);send_data_cmd(0x02);
send_ctrl_cmd(0xD525);send_data_cmd(0x4E);
send_ctrl_cmd(0xD526);send_data_cmd(0x02);
send_ctrl_cmd(0xD527);send_data_cmd(0x8A);
send_ctrl_cmd(0xD528);send_data_cmd(0x02);
send_ctrl_cmd(0xD529);send_data_cmd(0xC2);
send_ctrl_cmd(0xD52A);send_data_cmd(0x03);
send_ctrl_cmd(0xD52B);send_data_cmd(0x04);
send_ctrl_cmd(0xD52C);send_data_cmd(0x03);
send_ctrl_cmd(0xD52D);send_data_cmd(0x2E);
send_ctrl_cmd(0xD52E);send_data_cmd(0x03);
send_ctrl_cmd(0xD52F);send_data_cmd(0x74);
send_ctrl_cmd(0xD530);send_data_cmd(0x03);
send_ctrl_cmd(0xD531);send_data_cmd(0xEB);
send_ctrl_cmd(0xD532);send_data_cmd(0x03);
send_ctrl_cmd(0xD533);send_data_cmd(0xFF);



//GAMMA SETING  RED
send_ctrl_cmd(0xD600);send_data_cmd(0x00);
send_ctrl_cmd(0xD601);send_data_cmd(0x00);
send_ctrl_cmd(0xD602);send_data_cmd(0x00);
send_ctrl_cmd(0xD603);send_data_cmd(0x16);
send_ctrl_cmd(0xD604);send_data_cmd(0x00);
send_ctrl_cmd(0xD605);send_data_cmd(0x42);
send_ctrl_cmd(0xD606);send_data_cmd(0x00);
send_ctrl_cmd(0xD607);send_data_cmd(0x61);
send_ctrl_cmd(0xD608);send_data_cmd(0x00);
send_ctrl_cmd(0xD609);send_data_cmd(0x74);
send_ctrl_cmd(0xD60A);send_data_cmd(0x00);
send_ctrl_cmd(0xD60B);send_data_cmd(0x97);
send_ctrl_cmd(0xD60C);send_data_cmd(0x00);
send_ctrl_cmd(0xD60D);send_data_cmd(0xAD);
send_ctrl_cmd(0xD60E);send_data_cmd(0x00);
send_ctrl_cmd(0xD60F);send_data_cmd(0xDE);
send_ctrl_cmd(0xD610);send_data_cmd(0x01);
send_ctrl_cmd(0xD611);send_data_cmd(0x00);
send_ctrl_cmd(0xD612);send_data_cmd(0x01);
send_ctrl_cmd(0xD613);send_data_cmd(0x26);
send_ctrl_cmd(0xD614);send_data_cmd(0x01);
send_ctrl_cmd(0xD615);send_data_cmd(0x50);
send_ctrl_cmd(0xD616);send_data_cmd(0x01);
send_ctrl_cmd(0xD617);send_data_cmd(0x87);
send_ctrl_cmd(0xD618);send_data_cmd(0x01);
send_ctrl_cmd(0xD619);send_data_cmd(0xB3);
send_ctrl_cmd(0xD61A);send_data_cmd(0x01);
send_ctrl_cmd(0xD61B);send_data_cmd(0xB6);
send_ctrl_cmd(0xD61C);send_data_cmd(0x01);
send_ctrl_cmd(0xD61D);send_data_cmd(0xDC);
send_ctrl_cmd(0xD61E);send_data_cmd(0x02);
send_ctrl_cmd(0xD61F);send_data_cmd(0x04);
send_ctrl_cmd(0xD620);send_data_cmd(0x02);
send_ctrl_cmd(0xD621);send_data_cmd(0x1C);
send_ctrl_cmd(0xD622);send_data_cmd(0x02);
send_ctrl_cmd(0xD623);send_data_cmd(0x34);
send_ctrl_cmd(0xD624);send_data_cmd(0x02);
send_ctrl_cmd(0xD625);send_data_cmd(0x4E);
send_ctrl_cmd(0xD626);send_data_cmd(0x02);
send_ctrl_cmd(0xD627);send_data_cmd(0x8A);
send_ctrl_cmd(0xD628);send_data_cmd(0x02);
send_ctrl_cmd(0xD629);send_data_cmd(0xC2);
send_ctrl_cmd(0xD62A);send_data_cmd(0x03);
send_ctrl_cmd(0xD62B);send_data_cmd(0x04);
send_ctrl_cmd(0xD62C);send_data_cmd(0x03);
send_ctrl_cmd(0xD62D);send_data_cmd(0x2E);
send_ctrl_cmd(0xD62E);send_data_cmd(0x03);
send_ctrl_cmd(0xD62F);send_data_cmd(0x74);
send_ctrl_cmd(0xD630);send_data_cmd(0x03);
send_ctrl_cmd(0xD631);send_data_cmd(0xEB);
send_ctrl_cmd(0xD632);send_data_cmd(0x03);
send_ctrl_cmd(0xD633);send_data_cmd(0xFF);
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
    params->dpi.mipi_pll_clk_div1 = 54;//40?, punk 42->54 57(38M)
    params->dpi.mipi_pll_clk_div2 = 10;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING; //LCM_POLARITY_FALLING -> LCM_POLARITY_RISING
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

#if 1
	/*from sunrise zhaowu, for c8*/
    params->dpi.hsync_pulse_width = 2;
    params->dpi.hsync_back_porch  = 30;
    params->dpi.hsync_front_porch = 20;
    params->dpi.vsync_pulse_width = 2;
    params->dpi.vsync_back_porch  = 30;
    params->dpi.vsync_front_porch = 20;
#elif 1
	xx
    params->dpi.hsync_pulse_width = 22;
    params->dpi.hsync_back_porch  = 12;
    params->dpi.hsync_front_porch = 16;
    params->dpi.vsync_pulse_width = 10;
    params->dpi.vsync_back_porch  = 33;
    params->dpi.vsync_front_porch = 12;
#else
	xx
    params->dpi.hsync_pulse_width = 12;
    params->dpi.hsync_back_porch  = 8;
    params->dpi.hsync_front_porch = 8;
    params->dpi.vsync_pulse_width = 4;
    params->dpi.vsync_back_porch  = 8;
    params->dpi.vsync_front_porch = 8;
#endif
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current =LCM_DRIVING_CURRENT_6575_4MA;// LCM_DRIVING_CURRENT_6575_4MA; //punk LCM_DRIVING_CURRENT_4MA;
}


static void lcm_hw_reset(void)
{
	
    SET_RESET_PIN(1);
    MDELAY(20);
    SET_RESET_PIN(0);
	LCD_PRINT("punk,lcm_hw_reset -1\n ");
    MDELAY(60);
	LCD_PRINT("punk,lcm_hw_reset -2\n ");
    SET_RESET_PIN(1);
    MDELAY(80);
}

static void lcm_init(void)
{
    config_gpio();

    SET_RESET_PIN(1);
    MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(150);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
#if 0
	xx
    send_ctrl_cmd(0x2800);
    send_ctrl_cmd(0x1000);
    MDELAY(150); 
    send_ctrl_cmd(0x4f00);
    send_data_cmd(0x01); //DSTB

#else
	LCD_PRINT("punk,lcm_suspend -1.11\n ");
	/*from truly sandy*/
	send_ctrl_cmd(0xF000); send_data_cmd(0x55);
	send_ctrl_cmd(0xF001); send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002); send_data_cmd(0x52);
	send_ctrl_cmd(0xF003); send_data_cmd(0x08);
	send_ctrl_cmd(0xF004); send_data_cmd(0x00);
	send_ctrl_cmd(0xB100); send_data_cmd(0x4C);
	send_ctrl_cmd(0x2800); 
 
    MDELAY(100); 
	send_ctrl_cmd(0x1000); 
    MDELAY(50); 
#endif
}


static void lcm_resume(void)
{
#if 0
    lcm_hw_reset();
    init_lcm_registers();
#else
	LCD_PRINT("punk,lcm_resume -1.3\n ");
	/*from truly sandy*/
	send_ctrl_cmd(0xF000); send_data_cmd(0x55);
	send_ctrl_cmd(0xF001); send_data_cmd(0xAA);
	send_ctrl_cmd(0xF002); send_data_cmd(0x52);
	send_ctrl_cmd(0xF003); send_data_cmd(0x08);
	send_ctrl_cmd(0xF004); send_data_cmd(0x00);
	send_ctrl_cmd(0xB100); send_data_cmd(0xCC);
	send_ctrl_cmd(0x1100); 
    MDELAY(150);
	send_ctrl_cmd(0x2900); 
    MDELAY(50);
#endif

/*punk, for tp issue*/
#ifndef BUILD_UBOOT
#if defined(ACER_C8)
	fts_chk_chr();
#endif
#endif

}


/*for read id adc value*/
#if 1  //lk

#ifdef BUILD_UBOOT

#include "cust_adc.h"
#define ADC_CHECK_COUNT	5
extern int IMM_GetOneChannelValue(int dwChannel);

static unsigned int lcm_check_idpin_adc(unsigned char count)
{
    int idpin_auxadc = 0;
    int idpin_auxadc_check = 0;
    int i = 0;

    for(i = 0;i < count;i++)
    {
        idpin_auxadc_check = IMM_GetOneChannelValue(AUXADC_LCM_IDPIN_CHANNEL);
        idpin_auxadc += idpin_auxadc_check; 
        printf("LK+++:LCM Check ID pin ADC value[%d]=%d\n",i,idpin_auxadc_check);
    }
    idpin_auxadc = (idpin_auxadc / count);
    printf("LK+++:LCM Check ID pin ADC average value=%d\n",idpin_auxadc);
    return idpin_auxadc;
}
#endif

static unsigned int lcm_compare_id(void)
{
	unsigned int    id1 = 0;
	unsigned int    id2 = 0;
	unsigned int    id = 0;

#ifdef BUILD_UBOOT
    unsigned int    lcm_idpin_value = 0;

	lcm_idpin_value = lcm_check_idpin_adc(ADC_CHECK_COUNT);

	if(/*(LCM == id)&&*/ lcm_idpin_value < 100)/*0v, 100=1.0v*/
	{
		return 1;
	}
	else
	{
		return 0;
	}
#else
		return 0;
#endif
}

#endif





// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER srl4317gn_qhd_nt35516_lcm_drv= 
{

     .name			= "srl4317gn",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};
