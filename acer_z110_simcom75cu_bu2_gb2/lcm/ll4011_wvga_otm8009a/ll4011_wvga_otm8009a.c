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

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)
#define LCM_ID 0x40

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

#define GET_LSA0_BIT     mt_get_gpio_in(LSA0_GPIO_PIN)

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


static __inline unsigned char  spi_get_byte(unsigned int reg)
{
	unsigned int i;
	int k;
	unsigned char getbyte=0;

	// reg =  (CTRL_ID | (reg & 0xFF));
	SET_LSCK_HIGH;
	SET_LSDA_HIGH;
	UDELAY(1);
	SET_LSCE_LOW;
	UDELAY(1);


    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (reg & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        reg <<= 1;
    }
    
    UDELAY(2);    
    for (k = 7; k >= 0; k--)
    {
	SET_LSCK_LOW;       
	UDELAY(5);
	SET_LSCK_HIGH;    
	UDELAY(5);
        if (GET_LSA0_BIT) {
            getbyte |= (1 << k);
        }

    }    

    UDELAY(2);    
     
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

    return getbyte;
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

#if 0 //amy0517 



  	set_lcm_register(0xff00,0x80);  // 
	set_lcm_register(0xff01,0x09);  // enable EXTC
	set_lcm_register(0xff02,0x01);  // 
	set_lcm_register(0xff80,0x80); // enable Orise mode
	set_lcm_register(0xff81,0x09); // 
	set_lcm_register(0xff03,0x01);  // enable SPI+I2C cmd2 read
//gamma DC                                                                       
	set_lcm_register(0xC582,0xA3);  //REG-pump23                                
	set_lcm_register(0xC590,0xd6);  //Pump setting (3x=D6)-->(2x=96)//v02 01/11
	set_lcm_register(0xC591,0x87);  //Pump setting(VGH/VGL)                    
       set_lcm_register(0xD800,0x73); //GVDD=4.5V                                     
	set_lcm_register(0xD801,0x71);  //NGVDD=4.5V                               
//VCOMDC                                                                 
	set_lcm_register(0xd900,0x50);  // VCOMDC=                                 
//GAMMA 2.2
	set_lcm_register(0xE100,0x09);//2.2+
	set_lcm_register(0xE101,0x10);
	set_lcm_register(0xE102,0x14);
	set_lcm_register(0xE103,0x14);
	set_lcm_register(0xE104,0x08);
	set_lcm_register(0xE105,0x25);
	set_lcm_register(0xE106,0x14);
	set_lcm_register(0xE107,0x14);
	set_lcm_register(0xE108,0x01);
	set_lcm_register(0xE109,0x05);
	set_lcm_register(0xE10A,0x04);
	set_lcm_register(0xE10B,0x07);
	set_lcm_register(0xE10C,0x14);
	set_lcm_register(0xE10D,0x39);
	set_lcm_register(0xE10E,0x36);
	set_lcm_register(0xE10F,0x05);

	set_lcm_register(0xE100,0x09);//2.2-
	set_lcm_register(0xE101,0x10);
	set_lcm_register(0xE102,0x14);
	set_lcm_register(0xE103,0x14);
	set_lcm_register(0xE104,0x08);
	set_lcm_register(0xE105,0x25);
	set_lcm_register(0xE106,0x14);
	set_lcm_register(0xE107,0x14);
	set_lcm_register(0xE108,0x01);
	set_lcm_register(0xE109,0x05);
	set_lcm_register(0xE10A,0x04);
	set_lcm_register(0xE10B,0x07);
	set_lcm_register(0xE10C,0x14);
	set_lcm_register(0xE10D,0x39);
	set_lcm_register(0xE10E,0x36);
	set_lcm_register(0xE10F,0x05);				
                      				
	set_lcm_register(0xC181,0x66); //Frame rate 65Hz//V02
// RGB I/F setting VSYNC for OTM8018 0x0e
	set_lcm_register(0xC1a1,0x08); //external Vsync(08)  /Vsync,Hsync(0c) /Vsync,Hsync,DE(0e) //V02(0e)  / all  included clk(0f)
	set_lcm_register(0xC489,0x08); //pre-charge OFF
	set_lcm_register(0xC0a3,0x04); //pre-charge //V02
	set_lcm_register(0xC481,0x83); //source bias //V02
	set_lcm_register(0xC592,0x01); //Pump45
	set_lcm_register(0xC5B1,0xA9);  //DC voltage setting ;[0]GVDD output,default: 0xa8
//--------------------------------------------------------------------------------
//		initial setting 2 < tcon_goa_wave >
//--------------------------------------------------------------------------------
// CE8x : vst1,vst2,vst3,vst4
	set_lcm_register(0xCE80,0x85);	// ce81[7:0] : vst1_shift[7:0]
	set_lcm_register(0xCE81,0x03);	// ce82[7:0] : 0000,	vst1_width[3:0]
	set_lcm_register(0xCE82,0x00);	// ce83[7:0] : vst1_tchop[7:0]
	set_lcm_register(0xCE83,0x84);	// ce84[7:0] : vst2_shift[7:0]
	set_lcm_register(0xCE84,0x03);	// ce85[7:0] : 0000,	vst2_width[3:0]
	set_lcm_register(0xCE85,0x00);	// ce86[7:0] : vst2_tchop[7:0]
	set_lcm_register(0xCE86,0x83);	// ce87[7:0] : vst3_shift[7:0]                                                                                        
	set_lcm_register(0xCE87,0x03);	// ce88[7:0] : 0000,	vst3_width[3:0]                                                                               
	set_lcm_register(0xCE88,0x00);	// ce89[7:0] : vst3_tchop[7:0]                                                                                        
	set_lcm_register(0xCE89,0x82);	// ce8a[7:0] : vst4_shift[7:0]                                                                                        
	set_lcm_register(0xCE8a,0x03);	// ce8b[7:0] : 0000,	vst4_width[3:0]
	set_lcm_register(0xCE8b,0x00);	// ce8c[7:0] : vst4_tchop[7:0]
//CEAx : clka1,clka2
	set_lcm_register(0xCEa0,0x38);	// cea1[7:0] : clka1_width[3:0],clka1_shift[11:8]
	set_lcm_register(0xCEa1,0x02);	// cea2[7:0] : clka1_shift[7:0]
	set_lcm_register(0xCEa2,0x03);	// cea3[7:0] : clka1_sw_tg,odd_high,flat_head,flat_tail,switch[11:8]
	set_lcm_register(0xCEa3,0x21);	// cea4[7:0] : clka1_switch[7:0]
	set_lcm_register(0xCEa4,0x00);	// cea5[7:0] : clka1_extend[7:0]
	set_lcm_register(0xCEa5,0x00);	// cea6[7:0] : clka1_tchop[7:0]
	set_lcm_register(0xCEa6,0x00);	// cea7[7:0] : clka1_tglue[7:0]
	set_lcm_register(0xCEa7,0x38);	// cea8[7:0] : clka2_width[3:0],clka2_shift[11:8]
	set_lcm_register(0xCEa8,0x01);	// cea9[7:0] : clka2_shift[7:0]
	set_lcm_register(0xCEa9,0x03);	// ceaa[7:0] : clka2_sw_tg,odd_high,flat_head,flat_tail,switch[11:8]
	set_lcm_register(0xCEaa,0x22);	// ceab[7:0] : clka2_switch[7:0]
	set_lcm_register(0xCEab,0x00);	// ceac[7:0] : clka2_extend
	set_lcm_register(0xCEac,0x00);	// cead[7:0] : clka2_tchop
	set_lcm_register(0xCEad,0x00);	// ceae[7:0] : clka2_tglue
//CEBx : clka3,clka4
	set_lcm_register(0xCEb0,0x38);	// ceb1[7:0] : clka3_width[3:0],clka3_shift[11:8]
	set_lcm_register(0xCEb1,0x00);	// ceb2[7:0] : clka3_shift[7:0]
	set_lcm_register(0xCEb2,0x03);	// ceb3[7:0] : clka3_sw_tg,odd_high,flat_head,flat_tail,switch[11:8]
	set_lcm_register(0xCEb3,0x23);	// ceb4[7:0] : clka3_switch[7:0]
	set_lcm_register(0xCEb4,0x00);	// ceb5[7:0] : clka3_extend[7:0]
	set_lcm_register(0xCEb5,0x00);	// ceb6[7:0] : clka3_tchop[7:0]
	set_lcm_register(0xCEb6,0x00);	// ceb7[7:0] : clka3_tglue[7:0]
	set_lcm_register(0xCEb7,0x30);	// ceb8[7:0] : clka4_width[3:0],clka2_shift[11:8]
	set_lcm_register(0xCEb8,0x00);	// ceb9[7:0] : clka4_shift[7:0]
	set_lcm_register(0xCEb9,0x03);	// ceba[7:0] : clka4_sw_tg,odd_high,flat_head,flat_tail,switch[11:8]
	set_lcm_register(0xCEba,0x24);	// cebb[7:0] : clka4_switch[7:0]
	set_lcm_register(0xCEbb,0x00);	// cebc[7:0] : clka4_extend
	set_lcm_register(0xCEbc,0x00);	// cebd[7:0] : clka4_tchop
	set_lcm_register(0xCEbd,0x00);	// cebe[7:0] : clka4_tglue
//CECx : clkb1,clkb2
	set_lcm_register(0xCEc0,0x30);	// cec1[7:0] : clkb1_width[3:0],clkb1_shift[11:8]
	set_lcm_register(0xCEc1,0x01);	// cec2[7:0] : clkb1_shift[7:0]
	set_lcm_register(0xCEc2,0x03);	// cec3[7:0] : clkb1_sw_tg,odd_high,flat_head,flat_tail,switch[11:8]
	set_lcm_register(0xCEc3,0x25);	// cec4[7:0] : clkb1_switch[7:0]
	set_lcm_register(0xCEc4,0x00);	// cec5[7:0] : clkb1_extend[7:0]
	set_lcm_register(0xCEc5,0x00);	// cec6[7:0] : clkb1_tchop[7:0]
	set_lcm_register(0xCEc6,0x00);	// cec7[7:0] : clkb1_tglue[7:0]
	set_lcm_register(0xCEc7,0x30);	// cec8[7:0] : clkb2_width[3:0],clkb2_shift[11:8]
	set_lcm_register(0xCEc8,0x02);	// cec9[7:0] : clkb2_shift[7:0]
	set_lcm_register(0xCEc9,0x03);	// ceca[7:0] : clkb2_sw_tg,odd_high,flat_head,flat_tail,switch[11:8]
	set_lcm_register(0xCEca,0x26);	// cecb[7:0] : clkb2_switch[7:0]
	set_lcm_register(0xCEcb,0x00);	// cecc[7:0] : clkb2_extend
	set_lcm_register(0xCEcc,0x00);	// cecd[7:0] : clkb2_tchop
	set_lcm_register(0xCEcd,0x00);	// cece[7:0] : clkb2_tglue
//CEDx : clkb3,clkb4
	set_lcm_register(0xCEd0,0x30);	// ced1[7:0] : clkb3_width[3:0],clkb3_shift[11:8]
	set_lcm_register(0xCEd1,0x03);	// ced2[7:0] : clkb3_shift[7:0]
	set_lcm_register(0xCEd2,0x03);	// ced3[7:0] : clkb3_sw_tg,odd_high,flat_head,flat_tail,switch[11:8]
	set_lcm_register(0xCEd3,0x27);	// ced4[7:0] : clkb3_switch[7:0]
	set_lcm_register(0xCEd4,0x00);	// ced5[7:0] : clkb3_extend[7:0]
	set_lcm_register(0xCEd5,0x00);	// ced6[7:0] : clkb3_tchop[7:0]
	set_lcm_register(0xCEd6,0x00);	// ced7[7:0] : clkb3_tglue[7:0]
	set_lcm_register(0xCEd7,0x30);	// ced8[7:0] : clkb4_width[3:0],clkb4_shift[11:8]
	set_lcm_register(0xCEd8,0x04);	// ced9[7:0] : clkb4_shift[7:0]
	set_lcm_register(0xCEd9,0x03);	// ceda[7:0] : clkb4_sw_tg,odd_high,flat_head,flat_tail,switch[11:8]
	set_lcm_register(0xCEda,0x28);	// cedb[7:0] : clkb4_switch[7:0]
	set_lcm_register(0xCEdb,0x00);	// cedc[7:0] : clkb4_extend
	set_lcm_register(0xCEdc,0x00);	// cedd[7:0] : clkb4_tchop
	set_lcm_register(0xCEdd,0x00);	// cede[7:0] : clkb4_tglue
//CFCx :
	set_lcm_register(0xCFc0,0x00);	// cfc1[7:0] : eclk_normal_width[7:0]
	set_lcm_register(0xCFc1,0x00);	// cfc2[7:0] : eclk_partial_width[7:0]
	set_lcm_register(0xCFc2,0x00);	// cfc3[7:0] : all_normal_tchop[7:0]
	set_lcm_register(0xCFc3,0x00);	// cfc4[7:0] : all_partial_tchop[7:0]
	set_lcm_register(0xCFc4,0x00);	// cfc5[7:0] : eclk1_follow[3:0],eclk2_follow[3:0]
	set_lcm_register(0xCFc5,0x00);	// cfc6[7:0] : eclk3_follow[3:0],eclk4_follow[3:0]
	set_lcm_register(0xCFc6,0x00);	// cfc7[7:0] : 00,vstmask,vendmask,00,dir1,dir2 (0=VGL,1=VGH)
	set_lcm_register(0xCFc7,0x00);	// cfc8[7:0] : reg_goa_gnd_opt,reg_goa_dpgm_tail_set,reg_goa_f_gating_en,reg_goa_f_odd_gating,toggle_mod1,2,3,4
	set_lcm_register(0xCFc8,0x00);	// cfc9[7:0] : duty_block[3:0],DGPM[3:0]
	set_lcm_register(0xCFc9,0x00);	// cfca[7:0] : reg_goa_gnd_period[7:0]
//CFDx :
	set_lcm_register(0xCFd0,0x00);	// cfd1[7:0] : 0000000,reg_goa_frame_odd_high

//--------------------------------------------------------------------------------
//		initial setting 3 < Panel setting >
//--------------------------------------------------------------------------------
// cbcx
	set_lcm_register(0xCBc0,0x00);	//cbc1[7:0] : enmode H-byte of sig1  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc1,0x00);	//cbc2[7:0] : enmode H-byte of sig2  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc2,0x00);	//cbc3[7:0] : enmode H-byte of sig3  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc3,0x00);	//cbc4[7:0] : enmode H-byte of sig4  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc4,0x04);	//cbc5[7:0] : enmode H-byte of sig5  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc5,0x04);	//cbc6[7:0] : enmode H-byte of sig6  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc6,0x04);	//cbc7[7:0] : enmode H-byte of sig7  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc7,0x04);	//cbc8[7:0] : enmode H-byte of sig8  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc8,0x04);	//cbc9[7:0] : enmode H-byte of sig9  (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBc9,0x04);	//cbca[7:0] : enmode H-byte of sig10 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBca,0x00);	//cbcb[7:0] : enmode H-byte of sig11 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBcb,0x00);	//cbcc[7:0] : enmode H-byte of sig12 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBcc,0x00);	//cbcd[7:0] : enmode H-byte of sig13 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBcd,0x00);	//cbce[7:0] : enmode H-byte of sig14 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBce,0x00);	//cbcf[7:0] : enmode H-byte of sig15 (pwrof_0,pwrof_1,norm,pwron_4 )
// cbdx
	set_lcm_register(0xCBd0,0x00);	//cbd1[7:0] : enmode H-byte of sig16 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd1,0x00);	//cbd2[7:0] : enmode H-byte of sig17 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd2,0x00);	//cbd3[7:0] : enmode H-byte of sig18 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd3,0x00);	//cbd4[7:0] : enmode H-byte of sig19 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd4,0x00);	//cbd5[7:0] : enmode H-byte of sig20 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd5,0x00);	//cbd6[7:0] : enmode H-byte of sig21 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd6,0x00);	//cbd7[7:0] : enmode H-byte of sig22 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd7,0x00);	//cbd8[7:0] : enmode H-byte of sig23 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd8,0x00);	//cbd9[7:0] : enmode H-byte of sig24 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBd9,0x04);	//cbda[7:0] : enmode H-byte of sig25 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBda,0x04);	//cbdb[7:0] : enmode H-byte of sig26 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBdb,0x04);	//cbdc[7:0] : enmode H-byte of sig27 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBdc,0x04);	//cbdd[7:0] : enmode H-byte of sig28 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBdd,0x04);	//cbde[7:0] : enmode H-byte of sig29 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBde,0x04);	//cbdf[7:0] : enmode H-byte of sig30 (pwrof_0,pwrof_1,norm,pwron_4 )
// cbex
	set_lcm_register(0xCBe0,0x00);	//cbe1[7:0] : enmode H-byte of sig31 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe1,0x00);	//cbe2[7:0] : enmode H-byte of sig32 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe2,0x00);	//cbe3[7:0] : enmode H-byte of sig33 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe3,0x00);	//cbe4[7:0] : enmode H-byte of sig34 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe4,0x00);	//cbe5[7:0] : enmode H-byte of sig35 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe5,0x00);	//cbe6[7:0] : enmode H-byte of sig36 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe6,0x00);	//cbe7[7:0] : enmode H-byte of sig37 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe7,0x00);	//cbe8[7:0] : enmode H-byte of sig38 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe8,0x00);	//cbe9[7:0] : enmode H-byte of sig39 (pwrof_0,pwrof_1,norm,pwron_4 )
	set_lcm_register(0xCBe9,0x00);	//cbea[7:0] : enmode H-byte of sig40 (pwrof_0,pwrof_1,norm,pwron_4 )
// cc8x
	set_lcm_register(0xCC80,0x00);	//cc81[7:0] : reg setting for signal01 selection with u2d mode
	set_lcm_register(0xCC81,0x00);	//cc82[7:0] : reg setting for signal02 selection with u2d mode
	set_lcm_register(0xCC82,0x00);	//cc83[7:0] : reg setting for signal03 selection with u2d mode
	set_lcm_register(0xCC83,0x00);	//cc84[7:0] : reg setting for signal04 selection with u2d mode
	set_lcm_register(0xCC84,0x0C);	//cc85[7:0] : reg setting for signal05 selection with u2d mode
	set_lcm_register(0xCC85,0x0A);	//cc86[7:0] : reg setting for signal06 selection with u2d mode
	set_lcm_register(0xCC86,0x10);	//cc87[7:0] : reg setting for signal07 selection with u2d mode
	set_lcm_register(0xCC87,0x0E);	//cc88[7:0] : reg setting for signal08 selection with u2d mode
	set_lcm_register(0xCC88,0x03);	//cc89[7:0] : reg setting for signal09 selection with u2d mode
	set_lcm_register(0xCC89,0x04);	//cc8a[7:0] : reg setting for signal10 selection with u2d mode
// cc9x
	set_lcm_register(0xCC90,0x00);	//cc91[7:0] : reg setting for signal11 selection with u2d mode
	set_lcm_register(0xCC91,0x00);	//cc92[7:0] : reg setting for signal12 selection with u2d mode
	set_lcm_register(0xCC92,0x00);	//cc93[7:0] : reg setting for signal13 selection with u2d mode
	set_lcm_register(0xCC93,0x00);	//cc94[7:0] : reg setting for signal14 selection with u2d mode
	set_lcm_register(0xCC94,0x00);	//cc95[7:0] : reg setting for signal15 selection with u2d mode
	set_lcm_register(0xCC95,0x00);	//cc96[7:0] : reg setting for signal16 selection with u2d mode
	set_lcm_register(0xCC96,0x00);	//cc97[7:0] : reg setting for signal17 selection with u2d mode
	set_lcm_register(0xCC97,0x00);	//cc98[7:0] : reg setting for signal18 selection with u2d mode
	set_lcm_register(0xCC98,0x00);	//cc99[7:0] : reg setting for signal19 selection with u2d mode
	set_lcm_register(0xCC99,0x00);	//cc9a[7:0] : reg setting for signal20 selection with u2d mode
	set_lcm_register(0xCC9a,0x00);	//cc9b[7:0] : reg setting for signal21 selection with u2d mode
	set_lcm_register(0xCC9b,0x00);	//cc9c[7:0] : reg setting for signal22 selection with u2d mode
	set_lcm_register(0xCC9c,0x00);	//cc9d[7:0] : reg setting for signal23 selection with u2d mode
	set_lcm_register(0xCC9d,0x00);	//cc9e[7:0] : reg setting for signal24 selection with u2d mode
	set_lcm_register(0xCC9e,0x0B);	//cc9f[7:0] : reg setting for signal25 selection with u2d mode
// ccax
       set_lcm_register(0xCCa0,0x09);	//cca1[7:0] : reg setting for signal26 selection with u2d mode
	set_lcm_register(0xCCa1,0x0F);	//cca2[7:0] : reg setting for signal27 selection with u2d mode
	set_lcm_register(0xCCa2,0x0D);	//cca3[7:0] : reg setting for signal28 selection with u2d mode
	set_lcm_register(0xCCa3,0x01);	//cca4[7:0] : reg setting for signal29 selection with u2d mode
	set_lcm_register(0xCCa4,0x02);	//cca5[7:0] : reg setting for signal20 selection with u2d mode
	set_lcm_register(0xCCa5,0x00);	//cca6[7:0] : reg setting for signal31 selection with u2d mode
	set_lcm_register(0xCCa6,0x00);	//cca7[7:0] : reg setting for signal32 selection with u2d mode
	set_lcm_register(0xCCa7,0x00);	//cca8[7:0] : reg setting for signal33 selection with u2d mode
	set_lcm_register(0xCCa8,0x00);	//cca9[7:0] : reg setting for signal34 selection with u2d mode
	set_lcm_register(0xCCa9,0x00);	//ccaa[7:0] : reg setting for signal35 selection with u2d mode
	set_lcm_register(0xCCaa,0x00);	//ccab[7:0] : reg setting for signal36 selection with u2d mode
	set_lcm_register(0xCCab,0x00);	//ccac[7:0] : reg setting for signal37 selection with u2d mode
	set_lcm_register(0xCCac,0x00);	//ccad[7:0] : reg setting for signal38 selection with u2d mode
	set_lcm_register(0xCCad,0x00);	//ccae[7:0] : reg setting for signal39 selection with u2d mode
	set_lcm_register(0xCCae,0x00);	//ccaf[7:0] : reg setting for signal40 selection with u2d mode
// ccbx
	set_lcm_register(0xCCb0,0x00);	//ccb1[7:0] : reg setting for signal01 selection with d2u mode
	set_lcm_register(0xCCb1,0x00);	//ccb2[7:0] : reg setting for signal02 selection with d2u mode
	set_lcm_register(0xCCb2,0x00);	//ccb3[7:0] : reg setting for signal03 selection with d2u mode
	set_lcm_register(0xCCb3,0x00);	//ccb4[7:0] : reg setting for signal04 selection with d2u mode
	set_lcm_register(0xCCb4,0x0D);	//ccb5[7:0] : reg setting for signal05 selection with d2u mode
	set_lcm_register(0xCCb5,0x0F);	//ccb6[7:0] : reg setting for signal06 selection with d2u mode
	set_lcm_register(0xCCb6,0x09);	//ccb7[7:0] : reg setting for signal07 selection with d2u mode
	set_lcm_register(0xCCb7,0x0B);	//ccb8[7:0] : reg setting for signal08 selection with d2u mode
	set_lcm_register(0xCCb8,0x02);	//ccb9[7:0] : reg setting for signal09 selection with d2u mode
	set_lcm_register(0xCCb9,0x01);	//ccba[7:0] : reg setting for signal10 selection with d2u mode
// cccx
	set_lcm_register(0xCCc0,0x00);	//ccc1[7:0] : reg setting for signal11 selection with d2u mode
	set_lcm_register(0xCCc1,0x00);	//ccc2[7:0] : reg setting for signal12 selection with d2u mode
	set_lcm_register(0xCCc2,0x00);	//ccc3[7:0] : reg setting for signal13 selection with d2u mode
	set_lcm_register(0xCCc3,0x00);	//ccc4[7:0] : reg setting for signal14 selection with d2u mode
	set_lcm_register(0xCCc4,0x00);	//ccc5[7:0] : reg setting for signal15 selection with d2u mode
	set_lcm_register(0xCCc5,0x00);	//ccc6[7:0] : reg setting for signal16 selection with d2u mode
	set_lcm_register(0xCCc6,0x00);	//ccc7[7:0] : reg setting for signal17 selection with d2u mode
	set_lcm_register(0xCCc7,0x00);	//ccc8[7:0] : reg setting for signal18 selection with d2u mode
	set_lcm_register(0xCCc8,0x00);	//ccc9[7:0] : reg setting for signal19 selection with d2u mode
	set_lcm_register(0xCCc9,0x00);	//ccca[7:0] : reg setting for signal20 selection with d2u mode
	set_lcm_register(0xCCca,0x00);	//cccb[7:0] : reg setting for signal21 selection with d2u mode
	set_lcm_register(0xCCcb,0x00);	//cccc[7:0] : reg setting for signal22 selection with d2u mode
	set_lcm_register(0xCCcc,0x00);	//cccd[7:0] : reg setting for signal23 selection with d2u mode
	set_lcm_register(0xCCcd,0x00);	//ccce[7:0] : reg setting for signal24 selection with d2u mode
	set_lcm_register(0xCCce,0x0E);	//cccf[7:0] : reg setting for signal25 selection with d2u mode
// ccdx
	set_lcm_register(0xCCd0,0x10);	//ccd1[7:0] : reg setting for signal26 selection with d2u mode
	set_lcm_register(0xCCd1,0x0A);	//ccd2[7:0] : reg setting for signal27 selection with d2u mode
	set_lcm_register(0xCCd2,0x0C);	//ccd3[7:0] : reg setting for signal28 selection with d2u mode
	set_lcm_register(0xCCd3,0x04);	//ccd4[7:0] : reg setting for signal29 selection with d2u mode
	set_lcm_register(0xCCd4,0x03);	//ccd5[7:0] : reg setting for signal30 selection with d2u mode
	set_lcm_register(0xCCd5,0x00);	//ccd6[7:0] : reg setting for signal31 selection with d2u mode
	set_lcm_register(0xCCd6,0x00);	//ccd7[7:0] : reg setting for signal32 selection with d2u mode
	set_lcm_register(0xCCd7,0x00);	//ccd8[7:0] : reg setting for signal33 selection with d2u mode
	set_lcm_register(0xCCd8,0x00);	//ccd9[7:0] : reg setting for signal34 selection with d2u mode
	set_lcm_register(0xCCd9,0x00);	//ccda[7:0] : reg setting for signal35 selection with d2u mode
	set_lcm_register(0xCCda,0x00);	//ccdb[7:0] : reg setting for signal36 selection with d2u mode
	set_lcm_register(0xCCdb,0x00);	//ccdc[7:0] : reg setting for signal37 selection with d2u mode
	set_lcm_register(0xCCdc,0x00);	//ccdd[7:0] : reg setting for signal38 selection with d2u mode
	set_lcm_register(0xCCdd,0x00);	//ccde[7:0] : reg setting for signal39 selection with d2u mode
	set_lcm_register(0xCCde,0x00);	//ccdf[7:0] : reg setting for signal40 selection with d2u mode

	set_lcm_register(0x3A00,0x77); //  RGB 18bits D[17:0]
	set_lcm_register(0x1100,0x00);
	MDELAY(150);


	send_ctrl_cmd(0x2900);
	//printf(" Display on \n");
	MDELAY(50);
#else

set_lcm_register(0xFF00,0x80);
set_lcm_register(0xFF01,0x09);
set_lcm_register(0xFF02,0x01);
set_lcm_register(0xFF80,0x80);
set_lcm_register(0xFF81,0x09);
set_lcm_register(0xFF03,0x01);

set_lcm_register(0x2100,0x00);
set_lcm_register(0xD800,0x6F);
set_lcm_register(0xD801,0x6F);
set_lcm_register(0xC582,0xA3);
set_lcm_register(0xC181,0x66);
set_lcm_register(0xC1A1,0x08);
set_lcm_register(0xC0A3,0x1B);
set_lcm_register(0xC481,0x83);
set_lcm_register(0xC590,0x96);
set_lcm_register(0xC591,0xA7);
set_lcm_register(0xC592,0x01);
set_lcm_register(0xC5B1,0xA9);

set_lcm_register(0xE100,0x05);
set_lcm_register(0xE101,0x0B);
set_lcm_register(0xE102,0x11);
set_lcm_register(0xE103,0x12);
set_lcm_register(0xE104,0x0A);
set_lcm_register(0xE105,0x1B);
set_lcm_register(0xE106,0x0E);
set_lcm_register(0xE107,0x0C);
set_lcm_register(0xE108,0x01);
set_lcm_register(0xE109,0x05);
set_lcm_register(0xE10A,0x04);
set_lcm_register(0xE10B,0x08);
set_lcm_register(0xE10C,0x10);
set_lcm_register(0xE10D,0x27);
set_lcm_register(0xE10E,0x25);
set_lcm_register(0xE10F,0x03);

set_lcm_register(0xE200,0x05);
set_lcm_register(0xE201,0x0B);
set_lcm_register(0xE202,0x11);
set_lcm_register(0xE203,0x12);
set_lcm_register(0xE204,0x0A);
set_lcm_register(0xE205,0x1B);
set_lcm_register(0xE206,0x0E);
set_lcm_register(0xE207,0x0C);
set_lcm_register(0xE208,0x01);
set_lcm_register(0xE209,0x05);
set_lcm_register(0xE20A,0x04);
set_lcm_register(0xE20B,0x08);
set_lcm_register(0xE20C,0x10);
set_lcm_register(0xE20D,0x27);
set_lcm_register(0xE20E,0x25);
set_lcm_register(0xE20F,0x03);

set_lcm_register(0xB3A1,0x10);
set_lcm_register(0xB3A6,0x2B);
set_lcm_register(0xB3A7,0x11);

set_lcm_register(0xCE80,0x82);
set_lcm_register(0xCE81,0x01);
set_lcm_register(0xCE82,0x00);
set_lcm_register(0xCE83,0x81);
set_lcm_register(0xCE84,0x01);
set_lcm_register(0xCE85,0x00);

set_lcm_register(0xCEA0,0x18);
set_lcm_register(0xCEA1,0x00);
set_lcm_register(0xCEA2,0x03);
set_lcm_register(0xCEA3,0x23);
set_lcm_register(0xCEA4,0x00);
set_lcm_register(0xCEA5,0x00);
set_lcm_register(0xCEA6,0x00);
set_lcm_register(0xCEA7,0x10);
set_lcm_register(0xCEA8,0x00);
set_lcm_register(0xCEA9,0x03);
set_lcm_register(0xCEAA,0x24);
set_lcm_register(0xCEAB,0x00);
set_lcm_register(0xCEAC,0x00);
set_lcm_register(0xCEAD,0x00);

set_lcm_register(0xCEB0,0x10);
set_lcm_register(0xCEB1,0x01);
set_lcm_register(0xCEB2,0x03);
set_lcm_register(0xCEB3,0x23);
set_lcm_register(0xCEB4,0x00);
set_lcm_register(0xCEB5,0x00);
set_lcm_register(0xCEB6,0x00);
set_lcm_register(0xCEB7,0x10);
set_lcm_register(0xCEB8,0x02);
set_lcm_register(0xCEB9,0x03);
set_lcm_register(0xCEBA,0x24);
set_lcm_register(0xCEBB,0x00);
set_lcm_register(0xCEBC,0x00);
set_lcm_register(0xCEBD,0x00);

set_lcm_register(0xCFC7,0x80);
set_lcm_register(0xCFC9,0x06);

set_lcm_register(0xCBC6,0x04);
set_lcm_register(0xCBC7,0x04);
set_lcm_register(0xCBC9,0x04);
set_lcm_register(0xCBDB,0x04);
set_lcm_register(0xCBDC,0x04);
set_lcm_register(0xCBDE,0x04);

set_lcm_register(0xCC86,0x0C);
set_lcm_register(0xCC87,0x0A);
set_lcm_register(0xCC89,0x02);
set_lcm_register(0xCCA1,0x0B);
set_lcm_register(0xCCA2,0x09);
set_lcm_register(0xCCA4,0x01);
set_lcm_register(0xCCB6,0x09);
set_lcm_register(0xCCB7,0x0B);
set_lcm_register(0xCCB9,0x01);
set_lcm_register(0xCCD1,0x0A);
set_lcm_register(0xCCD2,0x0C);
set_lcm_register(0xCCD4,0x02);


set_lcm_register(0x3A,0x77);


set_lcm_register(0x1100,0x00);
MDELAY(150);


send_ctrl_cmd(0x2900);
MDELAY(200);
	
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
  //  lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);                   // GPIO in
      lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_DISP_LSA0_PIN_M_GPIO);
	lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);//input
	lcm_util.set_gpio_pull_enable(LSA0_GPIO_PIN, 0);
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
    params->dpi.mipi_pll_clk_div1 = 42;//40?
    params->dpi.mipi_pll_clk_div2 = 10;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING; //LCM_POLARITY_FALLING
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;  //LCM_POLARITY_FALLING amy0518 
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 10; //22
    params->dpi.hsync_back_porch  = 26 ; //12;
    params->dpi.hsync_front_porch = 40 ; //16;
    params->dpi.vsync_pulse_width = 2;
    params->dpi.vsync_back_porch  = 4; //33;
    params->dpi.vsync_front_porch = 4; //12;
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_8MA;
}


static void lcm_init(void)
{
    config_gpio();

    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(50);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
	send_ctrl_cmd(0x2800);
	MDELAY(100);
	send_ctrl_cmd(0x1000);
}

static unsigned int lcm_compare_id(void)
{
	unsigned int    id = 0;

//return 1; //amy0524
	config_gpio();

	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(200);

	send_ctrl_cmd(0xda00);
	id = spi_get_byte(0xc0);  //spi_read_data();
	MDELAY(20);

#ifdef BUILD_UBOOT
	printf("lcd_id=%x \r\n",id);
#else
	printk("lcd_id=%x \r\n",id);
#endif

	return (LCM_ID == id)?1:0;
}

static void lcm_resume(void)
{
	send_ctrl_cmd(0x1100);
	MDELAY(120);
	send_ctrl_cmd(0x2900);
	MDELAY(100);
	
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER ll4011_wvga_otm8009a_lcm_drv = 
{

	.name			= "ll4011_wvga_otm8009a",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id   = lcm_compare_id,
};

