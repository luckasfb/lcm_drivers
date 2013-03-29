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

#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

//chenhaojun
#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCM_ID 0x5510
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

#define SET_LA0_INPUT  SET_GPIO_INPUT(LSA0_GPIO_PIN)
#define SET_LA0_OUTPUT  SET_GPIO_OUTPUT(LSA0_GPIO_PIN)

#define GET_LSA0_BIT mt_get_gpio_in(LSA0_GPIO_PIN)

#define CTRL_ID  (0 << 8)
#define DATA_ID  (1 << 8)

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

static __inline unsigned char spi_read_data(void) 
{
    unsigned int i;
    int k=0;
    unsigned int getbyte1=0;
    unsigned int data=0;
	
#ifdef BUILD_UBOOT
	printf("spi_read_data   \r\n");
#endif

	data = 0xc0;		
	SET_LSCE_HIGH;
	UDELAY(1);
	SET_LSCK_HIGH;
	UDELAY(1);
	SET_LSDA_HIGH;
	UDELAY(1);
	SET_LA0_INPUT;
	
	SET_LSCE_LOW;
	UDELAY(1);

	for (i = 0; i < 8; ++ i)
	{
		SET_LSCK_LOW;
		if (data & (1 << 7)) 
		{
			SET_LSDA_HIGH;
		} 
		else 
		{
			SET_LSDA_LOW;
		}
		UDELAY(1);
		SET_LSCK_HIGH;
		UDELAY(1);
		data <<= 1;
	}
#if 0
	for(i=8;i>0;i--)
	{
		SET_LSCK_LOW;
		UDELAY(1);
		SET_LSCK_HIGH;
		UDELAY(1);
	}
#endif
	UDELAY(5);
    for (k = 7; k >=0; k--)
    {
        SET_LSCK_LOW;
	if (GET_LSA0_BIT) 
	{
		getbyte1 |= (1 << k);
	}
	
	UDELAY(1);
       SET_LSCK_HIGH;        
       UDELAY(1);
    }    
#ifdef BUILD_UBOOT
	printf("getbyte1=0x%x   \r\n",getbyte1);
#endif
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
    return getbyte1;
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
    unsigned char high_8bit = (data>>8)&0x00FF;
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
#ifdef BUILD_UBOOT
	printf("init_lcm_registers\r\n");
#else
	printk("init_lcm_registers\r\n");
#endif
#ifdef AOKESI_CHAOSHENG_LCM
/**********************************************
module factory:chaosheng
module id:GWMTF10160A(R)
resolution:480*800(WVGA)-4.0inch
**********************************************/
 //********Sleep  out****************
 send_ctrl_cmd(0XFF00);  //TEST Commands
 send_data_cmd(0XAA); 
 send_ctrl_cmd(0XFF01);
 send_data_cmd(0X55); 
 send_ctrl_cmd(0XFF02);
 send_data_cmd(0X25);
 send_ctrl_cmd(0XFF03);
 send_data_cmd(0X01);
 
 send_ctrl_cmd(0XF300);//TEST Commands
 send_data_cmd(0X00);    
 send_ctrl_cmd(0XF301);
 send_data_cmd(0X32);    
 send_ctrl_cmd(0XF302);
 send_data_cmd(0X00);    
 send_ctrl_cmd(0XF303);
 send_data_cmd(0X38);
 send_ctrl_cmd(0XF304);
 send_data_cmd(0X31);    
 send_ctrl_cmd(0XF305);
 send_data_cmd(0X08);    
 send_ctrl_cmd(0XF306);
 send_data_cmd(0X11);    
 send_ctrl_cmd(0XF307);
 send_data_cmd(0X00);
 
 //set page 0
 
 send_ctrl_cmd(0XF000);//Select Command Page “0”
 send_data_cmd(0X55);
 send_ctrl_cmd(0XF001);
 send_data_cmd(0XAA);
 send_ctrl_cmd(0XF002);
 send_data_cmd(0X52);
 send_ctrl_cmd(0XF003);
 send_data_cmd(0X08);
 send_ctrl_cmd(0XF004);
 send_data_cmd(0X00);
 
 send_ctrl_cmd(0XB000);//RGB Interface Signals Control?
 send_data_cmd(0X00);
 send_ctrl_cmd(0XB001);
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB002);
 send_data_cmd(0X02);
 send_ctrl_cmd(0XB003);
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB004);
 send_data_cmd(0X02);
 
 send_ctrl_cmd(0XB100);//Display Option Control
 send_data_cmd(0XCC);
 send_ctrl_cmd(0XB101);
 send_data_cmd(0X04);    
 
 send_ctrl_cmd(0X3600);//Memory data access control
 send_data_cmd(0X12);
 
 send_ctrl_cmd(0XB300);//Display Clock in RGB Interface
 send_data_cmd(0X00);
 
 send_ctrl_cmd(0XB600);//Source Output Data Hold Time Control
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XB700);//EQ Control Function for Gate Signals
 send_data_cmd(0X70); 
 send_ctrl_cmd(0XB701);//EQ Control Function for Gate Signals
 send_data_cmd(0X70); 
 
 send_ctrl_cmd(0XB800);//EQ Control Function for Source Driver
 send_data_cmd(0X00);
 send_ctrl_cmd(0XB801);//
 send_data_cmd(0X06);
 send_ctrl_cmd(0XB802);//
 send_data_cmd(0X06);
 send_ctrl_cmd(0XB805);//
 send_data_cmd(0X06);
 
 send_ctrl_cmd(0XBC00);//Inversion Driving Control
 send_data_cmd(0X00);
 send_ctrl_cmd(0XBC01);
 send_data_cmd(0X00);//0Xc8
 send_ctrl_cmd(0XBC02);
 send_data_cmd(0X00);
 
 send_ctrl_cmd(0XBD00);//Display Timing Control
 send_data_cmd(0X01);  
 send_ctrl_cmd(0XBD01);
 send_data_cmd(0X84);   
 send_ctrl_cmd(0XBD02);
 send_data_cmd(0X06);  
 send_ctrl_cmd(0XBD03);
 send_data_cmd(0X50);   
 send_ctrl_cmd(0XBD04);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0Xcc00);//Display Timing Control (Gateless)
 send_data_cmd(0X03);  
 send_ctrl_cmd(0Xcc01);
 send_data_cmd(0X01);   
 send_ctrl_cmd(0Xcc02);
 send_data_cmd(0X06);  
 
 //set page1
 
 send_ctrl_cmd(0XF000);//Select Command Page “1”
 send_data_cmd(0X55);
 send_ctrl_cmd(0XF001);
 send_data_cmd(0XAA);
 send_ctrl_cmd(0XF002);
 send_data_cmd(0X52);
 send_ctrl_cmd(0XF003);
 send_data_cmd(0X08);
 send_ctrl_cmd(0XF004);
 send_data_cmd(0X01);
 
 send_ctrl_cmd(0XB000);//Setting AVDD Voltage
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB001);
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB002);
 send_data_cmd(0X05);
 
 send_ctrl_cmd(0XB100);//Setting AVee Voltage
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB101);
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB102);
 send_data_cmd(0X05);
 
 send_ctrl_cmd(0XB200);//Setting lvgl Voltage
 send_data_cmd(0X03);
 send_ctrl_cmd(0XB201);
 send_data_cmd(0X03);
 send_ctrl_cmd(0XB202);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XB800);//Setting vcl Voltage
 send_data_cmd(0X25);
 send_ctrl_cmd(0XB801);
 send_data_cmd(0X25);
 send_ctrl_cmd(0XB802);
 send_data_cmd(0X25);
 
 send_ctrl_cmd(0XB300);//Setting vgh Voltage
 send_data_cmd(0X0b);
 send_ctrl_cmd(0XB301);
 send_data_cmd(0X0b);
 send_ctrl_cmd(0XB302);
 send_data_cmd(0X0b);
 
 send_ctrl_cmd(0XB900);//Setting vgh Voltage
 send_data_cmd(0X34);
 send_ctrl_cmd(0XB901);
 send_data_cmd(0X34);
 send_ctrl_cmd(0XB902);
 send_data_cmd(0X34);
 
 send_ctrl_cmd(0XBF00);//VGH Output Voltage
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XB500);//Setting VGL_REG Voltage
 send_data_cmd(0X08);
 send_ctrl_cmd(0XB501);
 send_data_cmd(0X08);
 send_ctrl_cmd(0XB502);
 send_data_cmd(0X08);
 
 send_ctrl_cmd(0XBA00);//Power Control for VGLX
 send_data_cmd(0X24);
 send_ctrl_cmd(0XBA01);
 send_data_cmd(0X24);
 send_ctrl_cmd(0XBA02);
 send_data_cmd(0X24);
 
 send_ctrl_cmd(0XB400);//Setting VRGH Voltage
 send_data_cmd(0X2D);
 send_ctrl_cmd(0XB401);
 send_data_cmd(0X2D);
 send_ctrl_cmd(0XB402);
 send_data_cmd(0X2D);
 
 send_ctrl_cmd(0XBC00);//Setting VGMP and VGSP Voltage
 send_data_cmd(0X00);
 send_ctrl_cmd(0XBC01);
 send_data_cmd(0X68);
 send_ctrl_cmd(0XBC02);
 send_data_cmd(0X00);    
 
 send_ctrl_cmd(0XBD00);//Setting VGMN and VGSN Voltage
 send_data_cmd(0X00);
 send_ctrl_cmd(0XBD01);
 send_data_cmd(0X7C);
 send_ctrl_cmd(0XBD02);
 send_data_cmd(0X00); 
 
 send_ctrl_cmd(0XBE00);//Setting VCOM Offset Voltage
 send_data_cmd(0X00);
 send_ctrl_cmd(0XBE01);
 send_data_cmd(0X40);
 
 //set page1
 
 send_ctrl_cmd(0XF000);//Select Command Page “1”
 send_data_cmd(0X55);
 send_ctrl_cmd(0XF001);
 send_data_cmd(0XAA);
 send_ctrl_cmd(0XF002);
 send_data_cmd(0X52);
 send_ctrl_cmd(0XF003);
 send_data_cmd(0X01);
 //send_ctrl_cmd(0XF004);
 //send_data_cmd(0X01);
 
 send_ctrl_cmd(0XD000);//Gradient Control for Gamma Voltage
 send_data_cmd(0X0B);
 send_ctrl_cmd(0XD001);
 send_data_cmd(0X14);
 send_ctrl_cmd(0XD002);
 send_data_cmd(0X0C);
 send_ctrl_cmd(0XD003);
 send_data_cmd(0X0E);
 
 //GAMMA RED
 send_ctrl_cmd(0XD100);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD101);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD102);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD103);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD104);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD105);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD106);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD107);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD108);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD109);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD10a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD10b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD10c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD10d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD10e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD10f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD110);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD111);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD112);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD113);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD114);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD115);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD116);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD117);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD118);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD119);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD11a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD11b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD11c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD11d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD11e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD11f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD120);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD121);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD122);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD123);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD124);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD125);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD126);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD127);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD128);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD129);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD12a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD12b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD12c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD12d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD12e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD12f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD130);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD131);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD132);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD133);
 send_data_cmd(0XF1);
 
 //Positive Gamma for GREEN
 send_ctrl_cmd(0XD200);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD201);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD202);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD203);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD204);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD205);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD206);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD207);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD208);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD209);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD20a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD20b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD20c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD20d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD20e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD20f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD210);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD211);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD212);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD213);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD214);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD215);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD216);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD217);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD218);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD219);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD21a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD21b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD21c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD21d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD21e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD21f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD220);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD221);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD222);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD223);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD224);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD225);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD226);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD227);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD228);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD229);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD22a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD22b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD22c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD22d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD22e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD22f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD230);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD231);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD232);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD233);
 send_data_cmd(0XF1);      
 
 //Positive Gamma for BLUE
 send_ctrl_cmd(0XD300);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD301);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD302);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD303);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD304);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD305);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD306);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD307);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD308);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD309);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD30a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD30b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD30c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD30d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD30e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD30f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD310);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD311);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD312);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD313);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD314);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD315);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD316);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD317);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD318);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD319);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD31a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD31b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD31c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD31d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD31e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD31f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD320);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD321);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD322);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD323);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD324);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD325);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD326);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD327);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD328);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD329);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD32a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD32b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD32c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD32d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD32e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD32f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD330);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD331);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD332);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD333);
 send_data_cmd(0XF1);   
 
 //Negative Gamma for RED
 send_ctrl_cmd(0XD400);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD401);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD402);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD403);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD404);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD405);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD406);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD407);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD408);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD409);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD40a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD40b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD40c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD40d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD40e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD40f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD410);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD411);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD412);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD413);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD414);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD415);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD416);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD417);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD418);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD419);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD41a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD41b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD41c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD41d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD41e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD41f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD420);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD421);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD422);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD423);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD424);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD425);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD426);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD427);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD428);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD429);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD42a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD42b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD42c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD42d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD42e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD42f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD430);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD431);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD432);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD433);
 send_data_cmd(0XF1); 
 
 //Negative Gamma for GREEN
 send_ctrl_cmd(0XD500);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD501);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD502);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD503);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD504);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD505);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD506);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD507);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD508);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD509);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD50a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD50b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD50c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD50d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD50e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD50f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD510);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD511);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD512);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD513);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD514);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD515);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD516);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD517);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD518);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD519);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD51a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD51b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD51c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD51d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD51e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD51f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD520);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD521);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD522);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD523);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD524);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD525);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD526);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD527);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD528);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD529);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD52a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD52b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD52c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD52d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD52e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD52f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD530);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD531);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD532);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD533);
 send_data_cmd(0XF1);  
 //Negative Gamma for BLUE
 send_ctrl_cmd(0XD600);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD601);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD602);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD603);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD604);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD605);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD606);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD607);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD608);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD609);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD60a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD60b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD60c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD60d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD60e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD60f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD610);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD611);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD612);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD613);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD614);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD615);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD616);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD617);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD618);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD619);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD61a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD61b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD61c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD61d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD61e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD61f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD620);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD621);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD622);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD623);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD624);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD625);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD626);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD627);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD628);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD629);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD62a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD62b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD62c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD62d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD62e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD62f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD630);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD631);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD632);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD633);
 send_data_cmd(0XF1);
 
 send_ctrl_cmd(0X3a00);
 send_data_cmd(0X77);// 0X55 //0x77->24bit
 
 // enable tering free
 send_ctrl_cmd(0X3500);
 send_data_cmd(0X00);
 
 //send_ctrl_cmd(0X3600);
 //send_data_cmd(0X54); 
#else
/**********************************************
module factory:sunrise
module id:SRL-WVGA-4009DN (V5.0)
resolution:480*800(WVGA)-4.0inch
**********************************************/
 //********Sleep  out****************
 send_ctrl_cmd(0XFF00);  //TEST Commands
 send_data_cmd(0XAA); 
 send_ctrl_cmd(0XFF01);
 send_data_cmd(0X55); 
 send_ctrl_cmd(0XFF02);
 send_data_cmd(0X25);
 send_ctrl_cmd(0XFF03);
 send_data_cmd(0X01);
 
 send_ctrl_cmd(0XF300);//TEST Commands
 send_data_cmd(0X00);    
 send_ctrl_cmd(0XF301);
 send_data_cmd(0X32);    
 send_ctrl_cmd(0XF302);
 send_data_cmd(0X00);    
 send_ctrl_cmd(0XF303);
 send_data_cmd(0X38);
 send_ctrl_cmd(0XF304);
 send_data_cmd(0X31);    
 send_ctrl_cmd(0XF305);
 send_data_cmd(0X08);    
 send_ctrl_cmd(0XF306);
 send_data_cmd(0X11);    
 send_ctrl_cmd(0XF307);
 send_data_cmd(0X00);
 
 //set page 0
 
 send_ctrl_cmd(0XF000);//Select Command Page “0”
 send_data_cmd(0X55);
 send_ctrl_cmd(0XF001);
 send_data_cmd(0XAA);
 send_ctrl_cmd(0XF002);
 send_data_cmd(0X52);
 send_ctrl_cmd(0XF003);
 send_data_cmd(0X08);
 send_ctrl_cmd(0XF004);
 send_data_cmd(0X00);
 
 send_ctrl_cmd(0XB000);//RGB Interface Signals Control?
 send_data_cmd(0X00);
 send_ctrl_cmd(0XB001);
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB002);
 send_data_cmd(0X02);
 send_ctrl_cmd(0XB003);
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB004);
 send_data_cmd(0X02);
 
 send_ctrl_cmd(0XB100);//Display Option Control
 send_data_cmd(0XCC);
 send_ctrl_cmd(0XB101);
 send_data_cmd(0X04);    
 
 send_ctrl_cmd(0X3600);//Memory data access control
 send_data_cmd(0X11);
 
 send_ctrl_cmd(0XB300);//Display Clock in RGB Interface
 send_data_cmd(0X00);
 
 send_ctrl_cmd(0XB600);//Source Output Data Hold Time Control
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XB700);//EQ Control Function for Gate Signals
 send_data_cmd(0X70); 
 send_ctrl_cmd(0XB701);//EQ Control Function for Gate Signals
 send_data_cmd(0X70); 
 
 send_ctrl_cmd(0XB800);//EQ Control Function for Source Driver
 send_data_cmd(0X00);
 send_ctrl_cmd(0XB801);//
 send_data_cmd(0X06);
 send_ctrl_cmd(0XB802);//
 send_data_cmd(0X06);
 send_ctrl_cmd(0XB805);//
 send_data_cmd(0X06);
 
 send_ctrl_cmd(0XBC00);//Inversion Driving Control
 send_data_cmd(0X00);
 send_ctrl_cmd(0XBC01);
 send_data_cmd(0X00);//0Xc8
 send_ctrl_cmd(0XBC02);
 send_data_cmd(0X00);
 
 send_ctrl_cmd(0XBD00);//Display Timing Control
 send_data_cmd(0X01);  
 send_ctrl_cmd(0XBD01);
 send_data_cmd(0X84);   
 send_ctrl_cmd(0XBD02);
 send_data_cmd(0X06);  
 send_ctrl_cmd(0XBD03);
 send_data_cmd(0X50);   
 send_ctrl_cmd(0XBD04);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0Xcc00);//Display Timing Control (Gateless)
 send_data_cmd(0X03);  
 send_ctrl_cmd(0Xcc01);
 send_data_cmd(0X01);   
 send_ctrl_cmd(0Xcc02);
 send_data_cmd(0X06);  
 
 //set page1
 
 send_ctrl_cmd(0XF000);//Select Command Page “1”
 send_data_cmd(0X55);
 send_ctrl_cmd(0XF001);
 send_data_cmd(0XAA);
 send_ctrl_cmd(0XF002);
 send_data_cmd(0X52);
 send_ctrl_cmd(0XF003);
 send_data_cmd(0X08);
 send_ctrl_cmd(0XF004);
 send_data_cmd(0X01);
 
 send_ctrl_cmd(0XB000);//Setting AVDD Voltage
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB001);
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB002);
 send_data_cmd(0X05);
 
 send_ctrl_cmd(0XB100);//Setting AVee Voltage
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB101);
 send_data_cmd(0X05);
 send_ctrl_cmd(0XB102);
 send_data_cmd(0X05);
 
 send_ctrl_cmd(0XB200);//Setting lvgl Voltage
 send_data_cmd(0X03);
 send_ctrl_cmd(0XB201);
 send_data_cmd(0X03);
 send_ctrl_cmd(0XB202);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XB800);//Setting vcl Voltage
 send_data_cmd(0X25);
 send_ctrl_cmd(0XB801);
 send_data_cmd(0X25);
 send_ctrl_cmd(0XB802);
 send_data_cmd(0X25);
 
 send_ctrl_cmd(0XB300);//Setting vgh Voltage
 send_data_cmd(0X0b);
 send_ctrl_cmd(0XB301);
 send_data_cmd(0X0b);
 send_ctrl_cmd(0XB302);
 send_data_cmd(0X0b);
 
 send_ctrl_cmd(0XB900);//Setting vgh Voltage
 send_data_cmd(0X34);
 send_ctrl_cmd(0XB901);
 send_data_cmd(0X34);
 send_ctrl_cmd(0XB902);
 send_data_cmd(0X34);
 
 send_ctrl_cmd(0XBF00);//VGH Output Voltage
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XB500);//Setting VGL_REG Voltage
 send_data_cmd(0X08);
 send_ctrl_cmd(0XB501);
 send_data_cmd(0X08);
 send_ctrl_cmd(0XB502);
 send_data_cmd(0X08);
 
 send_ctrl_cmd(0XBA00);//Power Control for VGLX
 send_data_cmd(0X24);
 send_ctrl_cmd(0XBA01);
 send_data_cmd(0X24);
 send_ctrl_cmd(0XBA02);
 send_data_cmd(0X24);
 
 send_ctrl_cmd(0XB400);//Setting VRGH Voltage
 send_data_cmd(0X2D);
 send_ctrl_cmd(0XB401);
 send_data_cmd(0X2D);
 send_ctrl_cmd(0XB402);
 send_data_cmd(0X2D);
 
 send_ctrl_cmd(0XBC00);//Setting VGMP and VGSP Voltage
 send_data_cmd(0X00);
 send_ctrl_cmd(0XBC01);
 send_data_cmd(0X68);
 send_ctrl_cmd(0XBC02);
 send_data_cmd(0X00);    
 
 send_ctrl_cmd(0XBD00);//Setting VGMN and VGSN Voltage
 send_data_cmd(0X00);
 send_ctrl_cmd(0XBD01);
 send_data_cmd(0X7C);
 send_ctrl_cmd(0XBD02);
 send_data_cmd(0X00); 
 
 send_ctrl_cmd(0XBE00);//Setting VCOM Offset Voltage
 send_data_cmd(0X00);
 send_ctrl_cmd(0XBE01);
 send_data_cmd(0X40);
 
 //set page1
 
 send_ctrl_cmd(0XF000);//Select Command Page “1”
 send_data_cmd(0X55);
 send_ctrl_cmd(0XF001);
 send_data_cmd(0XAA);
 send_ctrl_cmd(0XF002);
 send_data_cmd(0X52);
 send_ctrl_cmd(0XF003);
 send_data_cmd(0X01);
 //send_ctrl_cmd(0XF004);
 //send_data_cmd(0X01);
 
 send_ctrl_cmd(0XD000);//Gradient Control for Gamma Voltage
 send_data_cmd(0X0B);
 send_ctrl_cmd(0XD001);
 send_data_cmd(0X14);
 send_ctrl_cmd(0XD002);
 send_data_cmd(0X0C);
 send_ctrl_cmd(0XD003);
 send_data_cmd(0X0E);
 
 //GAMMA RED
 send_ctrl_cmd(0XD100);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD101);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD102);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD103);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD104);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD105);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD106);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD107);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD108);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD109);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD10a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD10b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD10c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD10d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD10e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD10f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD110);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD111);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD112);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD113);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD114);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD115);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD116);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD117);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD118);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD119);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD11a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD11b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD11c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD11d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD11e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD11f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD120);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD121);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD122);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD123);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD124);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD125);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD126);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD127);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD128);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD129);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD12a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD12b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD12c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD12d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD12e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD12f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD130);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD131);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD132);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD133);
 send_data_cmd(0XF1);
 
 //Positive Gamma for GREEN
 send_ctrl_cmd(0XD200);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD201);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD202);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD203);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD204);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD205);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD206);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD207);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD208);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD209);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD20a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD20b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD20c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD20d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD20e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD20f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD210);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD211);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD212);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD213);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD214);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD215);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD216);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD217);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD218);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD219);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD21a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD21b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD21c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD21d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD21e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD21f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD220);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD221);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD222);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD223);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD224);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD225);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD226);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD227);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD228);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD229);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD22a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD22b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD22c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD22d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD22e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD22f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD230);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD231);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD232);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD233);
 send_data_cmd(0XF1);      
 
 //Positive Gamma for BLUE
 send_ctrl_cmd(0XD300);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD301);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD302);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD303);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD304);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD305);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD306);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD307);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD308);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD309);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD30a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD30b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD30c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD30d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD30e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD30f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD310);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD311);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD312);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD313);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD314);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD315);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD316);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD317);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD318);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD319);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD31a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD31b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD31c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD31d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD31e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD31f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD320);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD321);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD322);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD323);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD324);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD325);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD326);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD327);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD328);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD329);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD32a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD32b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD32c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD32d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD32e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD32f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD330);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD331);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD332);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD333);
 send_data_cmd(0XF1);   
 
 //Negative Gamma for RED
 send_ctrl_cmd(0XD400);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD401);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD402);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD403);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD404);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD405);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD406);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD407);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD408);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD409);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD40a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD40b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD40c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD40d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD40e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD40f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD410);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD411);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD412);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD413);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD414);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD415);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD416);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD417);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD418);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD419);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD41a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD41b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD41c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD41d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD41e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD41f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD420);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD421);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD422);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD423);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD424);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD425);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD426);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD427);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD428);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD429);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD42a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD42b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD42c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD42d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD42e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD42f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD430);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD431);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD432);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD433);
 send_data_cmd(0XF1); 
 
 //Negative Gamma for GREEN
 send_ctrl_cmd(0XD500);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD501);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD502);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD503);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD504);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD505);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD506);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD507);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD508);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD509);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD50a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD50b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD50c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD50d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD50e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD50f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD510);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD511);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD512);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD513);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD514);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD515);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD516);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD517);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD518);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD519);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD51a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD51b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD51c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD51d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD51e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD51f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD520);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD521);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD522);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD523);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD524);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD525);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD526);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD527);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD528);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD529);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD52a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD52b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD52c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD52d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD52e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD52f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD530);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD531);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD532);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD533);
 send_data_cmd(0XF1);  
 //Negative Gamma for BLUE
 send_ctrl_cmd(0XD600);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD601);
 send_data_cmd(0X37);    
 
 send_ctrl_cmd(0XD602);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD603);
 send_data_cmd(0X61);  
 
 send_ctrl_cmd(0XD604);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD605);
 send_data_cmd(0X92);  
 
 send_ctrl_cmd(0XD606);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD607);
 send_data_cmd(0XB4);  
 
 send_ctrl_cmd(0XD608);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD609);
 send_data_cmd(0XCF);  
 
 send_ctrl_cmd(0XD60a);
 send_data_cmd(0X00);  
 
 send_ctrl_cmd(0XD60b);
 send_data_cmd(0XF6);  
 
 send_ctrl_cmd(0XD60c);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD60d);
 send_data_cmd(0X2F);  
 
 send_ctrl_cmd(0XD60e);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD60f);
 send_data_cmd(0X7F);
 
 send_ctrl_cmd(0XD610);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD611);
 send_data_cmd(0X97);  
 
 send_ctrl_cmd(0XD612);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD613);
 send_data_cmd(0XC0);  
 
 send_ctrl_cmd(0XD614);
 send_data_cmd(0X01);  
 
 send_ctrl_cmd(0XD615);
 send_data_cmd(0XE5); 
 
 send_ctrl_cmd(0XD616);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD617);
 send_data_cmd(0X1F);     
 
 send_ctrl_cmd(0XD618);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD619);
 send_data_cmd(0X50);  
 
 send_ctrl_cmd(0XD61a);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD61b);
 send_data_cmd(0X52);  
 
 send_ctrl_cmd(0XD61c);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD61d);
 send_data_cmd(0X87);  
 
 send_ctrl_cmd(0XD61e);
 send_data_cmd(0X02); 
 

 send_ctrl_cmd(0XD61f);
 send_data_cmd(0XBE);  
 
 send_ctrl_cmd(0XD620);
 send_data_cmd(0X02);  
 
 send_ctrl_cmd(0XD621);
 send_data_cmd(0XE2);  
 
 send_ctrl_cmd(0XD622);
 send_data_cmd(0X03);  
 
 send_ctrl_cmd(0XD623);
 send_data_cmd(0X0F);
 
 send_ctrl_cmd(0XD624);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD625);
 send_data_cmd(0X30);
 
 send_ctrl_cmd(0XD626);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD627);
 send_data_cmd(0X5C);
 
 send_ctrl_cmd(0XD628);
 send_data_cmd(0X03);
 

 send_ctrl_cmd(0XD629);
 send_data_cmd(0X77);
 
 send_ctrl_cmd(0XD62a);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD62b);
 send_data_cmd(0X94);
 
 send_ctrl_cmd(0XD62c);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD62d);
 send_data_cmd(0X9F);
 
 
 
 send_ctrl_cmd(0XD62e);
 send_data_cmd(0X03);    
 
 send_ctrl_cmd(0XD62f);
 send_data_cmd(0XAC);
 
 send_ctrl_cmd(0XD630);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD631);
 send_data_cmd(0XBA);
 
 send_ctrl_cmd(0XD632);
 send_data_cmd(0X03);
 
 send_ctrl_cmd(0XD633);
 send_data_cmd(0XF1);
 
 send_ctrl_cmd(0X3a00);
 send_data_cmd(0X77);// 0X55 //0x77->24bit
 
 // enable tering free
 send_ctrl_cmd(0X3500);
 send_data_cmd(0X00);
 
 //send_ctrl_cmd(0X3600);
 //send_data_cmd(0X54); 
#endif 
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

    lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_DISP_LSA0_PIN_M_GPIO);
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
    lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);                   // GPIO in
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
    params->dpi.mipi_pll_clk_div2 = 10;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 12;
    params->dpi.hsync_back_porch  = 8;
    params->dpi.hsync_front_porch = 8;
    params->dpi.vsync_pulse_width = 4;
    params->dpi.vsync_back_porch  = 8;
    params->dpi.vsync_front_porch = 8;
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;
}


static void lcm_init(void)
{

#ifdef BUILD_UBOOT
	printf("lcm_init\r\n");
#else
	printk("lcm_init\r\n");
#endif
	
    config_gpio();

    SET_RESET_PIN(1);//LK@I3000
    MDELAY(20);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);//LK@0314,at least 120ms

    init_lcm_registers();

}


static void lcm_suspend(void)
{


    send_ctrl_cmd(0X2800); //
    MDELAY(150); 
    send_ctrl_cmd(0x1000);
    MDELAY(150);

}


static void lcm_resume(void)
{
    send_ctrl_cmd(0X1100); //
    MDELAY(150);  
    send_ctrl_cmd(0X2900); //
    MDELAY(150); 



}

static unsigned int lcm_compare_id(void)
{
	unsigned int    id1 = 0;
	unsigned int    id2 = 0;
	unsigned int    id = 0;
	#if 1
	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);//LK@0314,at least 120ms
	#endif
	set_lcm_register(0xF000,0x55);             
	set_lcm_register(0xF001,0xAA);             
	set_lcm_register(0xF002,0x52);             
	set_lcm_register(0xF003,0x08);             
	set_lcm_register(0xF004,0x01);
	
	send_ctrl_cmd(0xc500);
	id1 = spi_read_data();
	MDELAY(20);
	send_ctrl_cmd(0xc501);
	id2 = spi_read_data();

	id = (id1<<8)|id2;

#ifdef BUILD_UBOOT
	printf("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#else
	printk("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#endif
	
    return (LCM_ID == id)?1:0;

}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER tft3p2380_nt35510_lcm_drv = 
{
    .name			= "tft3p2380_nt35510",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};



