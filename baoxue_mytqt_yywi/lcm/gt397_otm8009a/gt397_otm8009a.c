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

/*
 * Notes: As for otm8009a, register is 16 bits, but data is 8 bits.
 *        While writing, by non-nokia data format.
 *        While reading, by nokia data format.
 * Author: chu, zewei
 * Date:   2012/09/26
 */

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCM_ID        (0x8009) //  reg a1h , the 3rd and 4th parameters.
#define LCM_ID_REG	   (0xA100)

#define LCM_NAME		"[GT397_OTM8009A]"

//#define	TE_CONTROL

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
	unsigned int id = 0;
	id = lcm_util.read_data();
	//id = (id << 8) | (lcm_util.read_data() & 0xff);
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

	set_lcm_register(0x2A00, HIGH_BYTE(x0),  0);
	set_lcm_register(0x2A01, LOW_BYTE(x0),  0);
	set_lcm_register(0x2A02, HIGH_BYTE(x1),  0);
	set_lcm_register(0x2A03, LOW_BYTE(x1),  0);
	set_lcm_register(0x2B00, HIGH_BYTE(y0),  0);
	set_lcm_register(0x2B01, LOW_BYTE(y0),  0);
	set_lcm_register(0x2B02, HIGH_BYTE(y1),  0);  
	set_lcm_register(0x2B03, LOW_BYTE(y1),  0);

	send_ctrl_cmd(0x2C00);
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
    // gt397_otm8009a  
    
	// HSD panel
	set_lcm_register(0xff00, 0x80, 0);
	set_lcm_register(0xff01, 0x09, 0); //enable EXTC
	set_lcm_register(0xff02, 0x01, 0);
	set_lcm_register(0xff80, 0x80, 0); //enable Orise mode
	set_lcm_register(0xff81, 0x09, 0);
	set_lcm_register(0xff03, 0x01, 0); //enable SPI+I2C cmd2 read

	//gamma DC
	set_lcm_register(0xc0b4, 0x00, 0); //column inversion 	
	set_lcm_register(0xC489, 0x08, 0); //reg off	
	set_lcm_register(0xC0a3, 0x00, 0); //pre-charge //V02             
	set_lcm_register(0xC582, 0xA3, 0); //REG-pump23
	set_lcm_register(0xC590, 0x96, 0); //Pump setting (3x=D6)-->(2x=96)//v02 01/11
	set_lcm_register(0xC591, 0x87, 0); //Pump setting(VGH/VGL)   
	set_lcm_register(0xD800, 0x45, 0); //GVDD=4.5V  73   
	set_lcm_register(0xD801, 0x45, 0); //NGVDD=4.5V 71  

	//VCOMDC                                                                 
	set_lcm_register(0xD900, 0x73, 0); // 0x73

	set_lcm_register(0xE100, 0x00, 0);
	set_lcm_register(0xE101, 0x00, 0);
	set_lcm_register(0xE102, 0x03, 0);
	set_lcm_register(0xE103, 0x08, 0);
	set_lcm_register(0xE104, 0x09, 0);
	set_lcm_register(0xE105, 0x14, 0);
	set_lcm_register(0xE106, 0x0e, 0);
	set_lcm_register(0xE107, 0x0f, 0);
	set_lcm_register(0xE108, 0x00, 0);
	set_lcm_register(0xE109, 0x02, 0);
	set_lcm_register(0xE10A, 0x05, 0);
	set_lcm_register(0xE10B, 0x07, 0);
	set_lcm_register(0xE10C, 0x0e, 0);
	set_lcm_register(0xE10D, 0x27, 0);
	set_lcm_register(0xE10E, 0x26, 0);
	set_lcm_register(0xE10F, 0x0c, 0);
	                
	set_lcm_register(0xE200, 0x00, 0);
	set_lcm_register(0xE201, 0x00, 0);
	set_lcm_register(0xE202, 0x03, 0);
	set_lcm_register(0xE203, 0x08, 0);
	set_lcm_register(0xE204, 0x09, 0);
	set_lcm_register(0xE205, 0x14, 0);
	set_lcm_register(0xE206, 0x0e, 0);
	set_lcm_register(0xE207, 0x0f, 0);
	set_lcm_register(0xE208, 0x00, 0);
	set_lcm_register(0xE209, 0x02, 0);
	set_lcm_register(0xE20A, 0x05, 0);
	set_lcm_register(0xE20B, 0x07, 0);
	set_lcm_register(0xE20C, 0x0e, 0);
	set_lcm_register(0xE20D, 0x27, 0);
	set_lcm_register(0xE20E, 0x26, 0);
	set_lcm_register(0xE20F, 0x0c, 0);




	set_lcm_register(0xC181, 0x66, 0);     //Frame rate 65Hz//V02   

	// RGB I/F setting VSYNC for OTM8018 0x0e
	set_lcm_register(0xC1a1, 0x08, 0);     //external Vsync,Hsync,DE
	set_lcm_register(0xC0a3, 0x1b, 0);     //pre-charge //V02
	set_lcm_register(0xC481, 0x83, 0);     //source bias //V02
	set_lcm_register(0xC592, 0x01, 0);     //Pump45
	set_lcm_register(0xC5B1, 0xA9, 0);     //DC voltage setting ;[0]GVDD output, default: 0xa8

	// CE8x : vst1, vst2, vst3, vst4
	set_lcm_register(0xCE80, 0x85, 0);	// ce81[7:0] : vst1_shift[7:0]
	set_lcm_register(0xCE81, 0x03, 0);	// ce82[7:0] : 0000,	vst1_width[3:0]
	set_lcm_register(0xCE82, 0x00, 0);	// ce83[7:0] : vst1_tchop[7:0]
	set_lcm_register(0xCE83, 0x84, 0);	// ce84[7:0] : vst2_shift[7:0]
	set_lcm_register(0xCE84, 0x03, 0);	// ce85[7:0] : 0000,	vst2_width[3:0]
	set_lcm_register(0xCE85, 0x00, 0);	// ce86[7:0] : vst2_tchop[7:0]	                                                       
	set_lcm_register(0xCE86, 0x83, 0);	// ce87[7:0] : vst3_shift[7:0]	                                                       
	set_lcm_register(0xCE87, 0x03, 0);	// ce88[7:0] : 0000,	vst3_width[3:0]
	set_lcm_register(0xCE88, 0x00, 0);	// ce89[7:0] : vst3_tchop[7:0]                                                     
	set_lcm_register(0xCE89, 0x82, 0);	// ce8a[7:0] : vst4_shift[7:0]
	set_lcm_register(0xCE8a, 0x03, 0);	// ce8b[7:0] : 0000,	vst4_width[3:0]
	set_lcm_register(0xCE8b, 0x00, 0);	// ce8c[7:0] : vst4_tchop[7:0]

	//CEAx : clka1, clka2
	set_lcm_register(0xCEa0, 0x38, 0);	// cea1[7:0] : clka1_width[3:0], clka1_shift[11:8]
	set_lcm_register(0xCEa1, 0x02, 0);	// cea2[7:0] : clka1_shift[7:0]
	set_lcm_register(0xCEa2, 0x03, 0);	// cea3[7:0] : clka1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]               
	set_lcm_register(0xCEa3, 0x21, 0);	// cea4[7:0] : clka1_switch[7:0]                                                       
	set_lcm_register(0xCEa4, 0x00, 0);	// cea5[7:0] : clka1_extend[7:0]                                                       
	set_lcm_register(0xCEa5, 0x00, 0);	// cea6[7:0] : clka1_tchop[7:0]                                                        	                                 
	set_lcm_register(0xCEa6, 0x00, 0);	// cea7[7:0] : clka1_tglue[7:0]                                                                           
	set_lcm_register(0xCEa7, 0x38, 0);	// cea8[7:0] : clka2_width[3:0], clka2_shift[11:8]                                     
	set_lcm_register(0xCEa8, 0x01, 0);	// cea9[7:0] : clka2_shift[7:0]                                                        
	set_lcm_register(0xCEa9, 0x03, 0);	// ceaa[7:0] : clka2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                              
	set_lcm_register(0xCEaa, 0x22, 0);	// ceab[7:0] : clka2_switch[7:0]                                                       
	set_lcm_register(0xCEab, 0x00, 0);	// ceac[7:0] : clka2_extend                                                            
	set_lcm_register(0xCEac, 0x00, 0);	// cead[7:0] : clka2_tchop                                                             
	set_lcm_register(0xCEad, 0x00, 0);	// ceae[7:0] : clka2_tglue                                                             
	                             
	//CEBx : clka3, clka4                                                                                                          
	set_lcm_register(0xCEb0, 0x38, 0);	// ceb1[7:0] : clka3_width[3:0], clka3_shift[11:8]                                     
	set_lcm_register(0xCEb1, 0x00, 0);	// ceb2[7:0] : clka3_shift[7:0]                                                        
	set_lcm_register(0xCEb2, 0x03, 0);	// ceb3[7:0] : clka3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]               
	set_lcm_register(0xCEb3, 0x23, 0);	// ceb4[7:0] : clka3_switch[7:0]                                                       
	set_lcm_register(0xCEb4, 0x00, 0);	// ceb5[7:0] : clka3_extend[7:0]                                                       
	set_lcm_register(0xCEb5, 0x00, 0);	// ceb6[7:0] : clka3_tchop[7:0]                                                        
	set_lcm_register(0xCEb6, 0x00, 0);	// ceb7[7:0] : clka3_tglue[7:0]                                                        
	set_lcm_register(0xCEb7, 0x30, 0);	// ceb8[7:0] : clka4_width[3:0], clka2_shift[11:8]                                     
	set_lcm_register(0xCEb8, 0x00, 0);	// ceb9[7:0] : clka4_shift[7:0]                                                        
	set_lcm_register(0xCEb9, 0x03, 0);	// ceba[7:0] : clka4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]               
	set_lcm_register(0xCEba, 0x24, 0);	// cebb[7:0] : clka4_switch[7:0]                                                       
	set_lcm_register(0xCEbb, 0x00, 0);	// cebc[7:0] : clka4_extend                                                            
	set_lcm_register(0xCEbc, 0x00, 0);	// cebd[7:0] : clka4_tchop                                                             
	set_lcm_register(0xCEbd, 0x00, 0);	// cebe[7:0] : clka4_tglue                                                             
	                             
	//CECx : clkb1, clkb2                                                                                                          
	set_lcm_register(0xCEc0, 0x30, 0);	// cec1[7:0] : clkb1_width[3:0], clkb1_shift[11:8]                                     
	set_lcm_register(0xCEc1, 0x01, 0);	// cec2[7:0] : clkb1_shift[7:0]                                                        
	set_lcm_register(0xCEc2, 0x03, 0);	// cec3[7:0] : clkb1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]               
	set_lcm_register(0xCEc3, 0x25, 0);	// cec4[7:0] : clkb1_switch[7:0]                                                       
	set_lcm_register(0xCEc4, 0x00, 0);	// cec5[7:0] : clkb1_extend[7:0]                                                       
	set_lcm_register(0xCEc5, 0x00, 0);	// cec6[7:0] : clkb1_tchop[7:0]                                                        
	set_lcm_register(0xCEc6, 0x00, 0);	// cec7[7:0] : clkb1_tglue[7:0]                                                        
	set_lcm_register(0xCEc7, 0x30, 0);	// cec8[7:0] : clkb2_width[3:0], clkb2_shift[11:8]                                     
	set_lcm_register(0xCEc8, 0x02, 0);	// cec9[7:0] : clkb2_shift[7:0]                                                        
	set_lcm_register(0xCEc9, 0x03, 0);	// ceca[7:0] : clkb2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]               
	set_lcm_register(0xCEca, 0x26, 0);	// cecb[7:0] : clkb2_switch[7:0]                                                       
	set_lcm_register(0xCEcb, 0x00, 0);	// cecc[7:0] : clkb2_extend                                                            
	set_lcm_register(0xCEcc, 0x00, 0);	// cecd[7:0] : clkb2_tchop                                                             
	set_lcm_register(0xCEcd, 0x00, 0);	// cece[7:0] : clkb2_tglue                                                             
	                                 
	//CEDx : clkb3, clkb4                                                                                                          
	set_lcm_register(0xCEd0, 0x30, 0);	// ced1[7:0] : clkb3_width[3:0], clkb3_shift[11:8]                                     
	set_lcm_register(0xCEd1, 0x03, 0);	// ced2[7:0] : clkb3_shift[7:0]                                                        
	set_lcm_register(0xCEd2, 0x03, 0);	// ced3[7:0] : clkb3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]               
	set_lcm_register(0xCEd3, 0x27, 0);	// ced4[7:0] : clkb3_switch[7:0]                                                       
	set_lcm_register(0xCEd4, 0x00, 0);	// ced5[7:0] : clkb3_extend[7:0]                                                       
	set_lcm_register(0xCEd5, 0x00, 0);	// ced6[7:0] : clkb3_tchop[7:0]                                                        
	set_lcm_register(0xCEd6, 0x00, 0);	// ced7[7:0] : clkb3_tglue[7:0]                                                        
	set_lcm_register(0xCEd7, 0x30, 0);	// ced8[7:0] : clkb4_width[3:0], clkb4_shift[11:8]                                     
	set_lcm_register(0xCEd8, 0x04, 0);	// ced9[7:0] : clkb4_shift[7:0]                                                        
	set_lcm_register(0xCEd9, 0x03, 0);	// ceda[7:0] : clkb4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]               
	set_lcm_register(0xCEda, 0x28, 0);	// cedb[7:0] : clkb4_switch[7:0]                                                       
	set_lcm_register(0xCEdb, 0x00, 0);	// cedc[7:0] : clkb4_extend                                                            
	set_lcm_register(0xCEdc, 0x00, 0);	// cedd[7:0] : clkb4_tchop                                                             
	set_lcm_register(0xCEdd, 0x00, 0);	// cede[7:0] : clkb4_tglue                                                             
	                             
	//CFCx :                                                                                                                       
	set_lcm_register(0xCFc0, 0x00, 0);	// cfc1[7:0] : eclk_normal_width[7:0]                                                  
	set_lcm_register(0xCFc1, 0x00, 0);	// cfc2[7:0] : eclk_partial_width[7:0]                                                 
	set_lcm_register(0xCFc2, 0x00, 0);	// cfc3[7:0] : all_normal_tchop[7:0]                                                   
	set_lcm_register(0xCFc3, 0x00, 0);	// cfc4[7:0] : all_partial_tchop[7:0]                                                  
	set_lcm_register(0xCFc4, 0x00, 0);	// cfc5[7:0] : eclk1_follow[3:0], eclk2_follow[3:0]                                    
	set_lcm_register(0xCFc5, 0x00, 0);	// cfc6[7:0] : eclk3_follow[3:0], eclk4_follow[3:0]                                    
	set_lcm_register(0xCFc6, 0x00, 0);	// cfc7[7:0] : 00, vstmask, vendmask, 00, dir1, dir2 (0=VGL, 1=VGH)                    
	set_lcm_register(0xCFc7, 0x00, 0);	// cfc8[7:0] : reg_goa_gnd_opt, reg_goa_dpgm_tail_set, reg_goa_f_gating_en, 	reg_goa_f_odd_gating, toggle_mod1, 2, 3, 4  
	set_lcm_register(0xCFc8, 0x00, 0);	// cfc9[7:0] : duty_block[3:0], DGPM[3:0]                                              
	set_lcm_register(0xCFc9, 0x00, 0);	// cfca[7:0] : reg_goa_gnd_period[7:0]                                                 

	//CFDx :
	set_lcm_register(0xCFd0, 0x00, 0);// cfd1[7:0] : 0000000, reg_goa_frame_odd_high

	//--------------------------------------------------------------------------------
	//		initial setting 3 < Panel setting >
	//--------------------------------------------------------------------------------
	// cbcx
	set_lcm_register(0xCBc0, 0x00, 0);	//cbc1[7:0] : enmode H-byte of sig1  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc1, 0x00, 0);	//cbc2[7:0] : enmode H-byte of sig2  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc2, 0x00, 0);	//cbc3[7:0] : enmode H-byte of sig3  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc3, 0x00, 0);	//cbc4[7:0] : enmode H-byte of sig4  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc4, 0x04, 0);	//cbc5[7:0] : enmode H-byte of sig5  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc5, 0x04, 0);	//cbc6[7:0] : enmode H-byte of sig6  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc6, 0x04, 0);	//cbc7[7:0] : enmode H-byte of sig7  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc7, 0x04, 0);	//cbc8[7:0] : enmode H-byte of sig8  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc8, 0x04, 0);	//cbc9[7:0] : enmode H-byte of sig9  (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBc9, 0x04, 0);	//cbca[7:0] : enmode H-byte of sig10 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBca, 0x00, 0);	//cbcb[7:0] : enmode H-byte of sig11 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBcb, 0x00, 0);	//cbcc[7:0] : enmode H-byte of sig12 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBcc, 0x00, 0);	//cbcd[7:0] : enmode H-byte of sig13 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBcd, 0x00, 0);	//cbce[7:0] : enmode H-byte of sig14 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBce, 0x00, 0);	//cbcf[7:0] : enmode H-byte of sig15 (pwrof_0, pwrof_1, norm, pwron_4 )                
	      
	// cbdx                                                                                                                        
	set_lcm_register(0xCBd0, 0x00, 0);	//cbd1[7:0] : enmode H-byte of sig16 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd1, 0x00, 0);	//cbd2[7:0] : enmode H-byte of sig17 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd2, 0x00, 0);	//cbd3[7:0] : enmode H-byte of sig18 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd3, 0x00, 0);	//cbd4[7:0] : enmode H-byte of sig19 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd4, 0x00, 0);	//cbd5[7:0] : enmode H-byte of sig20 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd5, 0x00, 0);	//cbd6[7:0] : enmode H-byte of sig21 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd6, 0x00, 0);	//cbd7[7:0] : enmode H-byte of sig22 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd7, 0x00, 0);	//cbd8[7:0] : enmode H-byte of sig23 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd8, 0x00, 0);	//cbd9[7:0] : enmode H-byte of sig24 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBd9, 0x04, 0);	//cbda[7:0] : enmode H-byte of sig25 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBda, 0x04, 0);	//cbdb[7:0] : enmode H-byte of sig26 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBdb, 0x04, 0);	//cbdc[7:0] : enmode H-byte of sig27 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBdc, 0x04, 0);	//cbdd[7:0] : enmode H-byte of sig28 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBdd, 0x04, 0);	//cbde[7:0] : enmode H-byte of sig29 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBde, 0x04, 0);	//cbdf[7:0] : enmode H-byte of sig30 (pwrof_0, pwrof_1, norm, pwron_4 )                
	                                                             
	// cbex                                                      
	set_lcm_register(0xCBe0, 0x00, 0);	//cbe1[7:0] : enmode H-byte of sig31 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe1, 0x00, 0);	//cbe2[7:0] : enmode H-byte of sig32 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe2, 0x00, 0);	//cbe3[7:0] : enmode H-byte of sig33 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe3, 0x00, 0);	//cbe4[7:0] : enmode H-byte of sig34 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe4, 0x00, 0);	//cbe5[7:0] : enmode H-byte of sig35 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe5, 0x00, 0);	//cbe6[7:0] : enmode H-byte of sig36 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe6, 0x00, 0);	//cbe7[7:0] : enmode H-byte of sig37 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe7, 0x00, 0);	//cbe8[7:0] : enmode H-byte of sig38 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe8, 0x00, 0);	//cbe9[7:0] : enmode H-byte of sig39 (pwrof_0, pwrof_1, norm, pwron_4 )                
	set_lcm_register(0xCBe9, 0x00, 0);	//cbea[7:0] : enmode H-byte of sig40 (pwrof_0, pwrof_1, norm, pwron_4 )                
	                                                  
	// cc8x                                           
	set_lcm_register(0xCC80, 0x00, 0);	//cc81[7:0] : reg setting for signal01 selection with u2d mode                         
	set_lcm_register(0xCC81, 0x00, 0);	//cc82[7:0] : reg setting for signal02 selection with u2d mode                         
	set_lcm_register(0xCC82, 0x00, 0);	//cc83[7:0] : reg setting for signal03 selection with u2d mode                         
	set_lcm_register(0xCC83, 0x00, 0);	//cc84[7:0] : reg setting for signal04 selection with u2d mode                         
	set_lcm_register(0xCC84, 0x0C, 0);	//cc85[7:0] : reg setting for signal05 selection with u2d mode                         
	set_lcm_register(0xCC85, 0x0A, 0);	//cc86[7:0] : reg setting for signal06 selection with u2d mode                         
	set_lcm_register(0xCC86, 0x10, 0);	//cc87[7:0] : reg setting for signal07 selection with u2d mode                         
	set_lcm_register(0xCC87, 0x0E, 0);	//cc88[7:0] : reg setting for signal08 selection with u2d mode                         
	set_lcm_register(0xCC88, 0x03, 0);	//cc89[7:0] : reg setting for signal09 selection with u2d mode                         
	set_lcm_register(0xCC89, 0x04, 0);	//cc8a[7:0] : reg setting for signal10 selection with u2d mode                         
	      
	// cc9x                                                                                                                        
	set_lcm_register(0xCC90, 0x00, 0);	//cc91[7:0] : reg setting for signal11 selection with u2d mode                         
	set_lcm_register(0xCC91, 0x00, 0);	//cc92[7:0] : reg setting for signal12 selection with u2d mode                         
	set_lcm_register(0xCC92, 0x00, 0);	//cc93[7:0] : reg setting for signal13 selection with u2d mode                         
	set_lcm_register(0xCC93, 0x00, 0);	//cc94[7:0] : reg setting for signal14 selection with u2d mode                         
	set_lcm_register(0xCC94, 0x00, 0);	//cc95[7:0] : reg setting for signal15 selection with u2d mode                         
	set_lcm_register(0xCC95, 0x00, 0);	//cc96[7:0] : reg setting for signal16 selection with u2d mode                         
	set_lcm_register(0xCC96, 0x00, 0);	//cc97[7:0] : reg setting for signal17 selection with u2d mode                         
	set_lcm_register(0xCC97, 0x00, 0);	//cc98[7:0] : reg setting for signal18 selection with u2d mode                         
	set_lcm_register(0xCC98, 0x00, 0);	//cc99[7:0] : reg setting for signal19 selection with u2d mode                         
	set_lcm_register(0xCC99, 0x00, 0);	//cc9a[7:0] : reg setting for signal20 selection with u2d mode                         
	set_lcm_register(0xCC9a, 0x00, 0);	//cc9b[7:0] : reg setting for signal21 selection with u2d mode                         
	set_lcm_register(0xCC9b, 0x00, 0);	//cc9c[7:0] : reg setting for signal22 selection with u2d mode                         
	set_lcm_register(0xCC9c, 0x00, 0);	//cc9d[7:0] : reg setting for signal23 selection with u2d mode                         
	set_lcm_register(0xCC9d, 0x00, 0);	//cc9e[7:0] : reg setting for signal24 selection with u2d mode                         
	set_lcm_register(0xCC9e, 0x0B, 0);	//cc9f[7:0] : reg setting for signal25 selection with u2d mode                         
	      
	// ccax                                                                                                                        
	set_lcm_register(0xCCa0, 0x09, 0);	//cca1[7:0] : reg setting for signal26 selection with u2d mode                         
	set_lcm_register(0xCCa1, 0x0F, 0);	//cca2[7:0] : reg setting for signal27 selection with u2d mode                         
	set_lcm_register(0xCCa2, 0x0D, 0);	//cca3[7:0] : reg setting for signal28 selection with u2d mode                         
	set_lcm_register(0xCCa3, 0x01, 0);	//cca4[7:0] : reg setting for signal29 selection with u2d mode                         
	set_lcm_register(0xCCa4, 0x02, 0);	//cca5[7:0] : reg setting for signal20 selection with u2d mode                         
	set_lcm_register(0xCCa5, 0x00, 0);	//cca6[7:0] : reg setting for signal31 selection with u2d mode                         
	set_lcm_register(0xCCa6, 0x00, 0);	//cca7[7:0] : reg setting for signal32 selection with u2d mode                         
	set_lcm_register(0xCCa7, 0x00, 0);	//cca8[7:0] : reg setting for signal33 selection with u2d mode                         
	set_lcm_register(0xCCa8, 0x00, 0);	//cca9[7:0] : reg setting for signal34 selection with u2d mode                         
	set_lcm_register(0xCCa9, 0x00, 0);	//ccaa[7:0] : reg setting for signal35 selection with u2d mode                         
	set_lcm_register(0xCCaa, 0x00, 0);	//ccab[7:0] : reg setting for signal36 selection with u2d mode                         
	set_lcm_register(0xCCab, 0x00, 0);	//ccac[7:0] : reg setting for signal37 selection with u2d mode                         
	set_lcm_register(0xCCac, 0x00, 0);	//ccad[7:0] : reg setting for signal38 selection with u2d mode                         
	set_lcm_register(0xCCad, 0x00, 0);	//ccae[7:0] : reg setting for signal39 selection with u2d mode                         
	set_lcm_register(0xCCae, 0x00, 0);	//ccaf[7:0] : reg setting for signal40 selection with u2d mode                         
	      
	// ccbx                                                                                                                        
	set_lcm_register(0xCCb0, 0x00, 0);	//ccb1[7:0] : reg setting for signal01 selection with d2u mode                         
	set_lcm_register(0xCCb1, 0x00, 0);	//ccb2[7:0] : reg setting for signal02 selection with d2u mode                         
	set_lcm_register(0xCCb2, 0x00, 0);	//ccb3[7:0] : reg setting for signal03 selection with d2u mode                         
	set_lcm_register(0xCCb3, 0x00, 0);	//ccb4[7:0] : reg setting for signal04 selection with d2u mode                         
	set_lcm_register(0xCCb4, 0x0D, 0);	//ccb5[7:0] : reg setting for signal05 selection with d2u mode                         
	set_lcm_register(0xCCb5, 0x0F, 0);	//ccb6[7:0] : reg setting for signal06 selection with d2u mode                         
	set_lcm_register(0xCCb6, 0x09, 0);	//ccb7[7:0] : reg setting for signal07 selection with d2u mode                         
	set_lcm_register(0xCCb7, 0x0B, 0);	//ccb8[7:0] : reg setting for signal08 selection with d2u mode                         
	set_lcm_register(0xCCb8, 0x02, 0);	//ccb9[7:0] : reg setting for signal09 selection with d2u mode                         
	set_lcm_register(0xCCb9, 0x01, 0);	//ccba[7:0] : reg setting for signal10 selection with d2u mode                         
	      
	// cccx                                                                                                                        
	set_lcm_register(0xCCc0, 0x00, 0);	//ccc1[7:0] : reg setting for signal11 selection with d2u mode                         
	set_lcm_register(0xCCc1, 0x00, 0);	//ccc2[7:0] : reg setting for signal12 selection with d2u mode                         
	set_lcm_register(0xCCc2, 0x00, 0);	//ccc3[7:0] : reg setting for signal13 selection with d2u mode                         
	set_lcm_register(0xCCc3, 0x00, 0);	//ccc4[7:0] : reg setting for signal14 selection with d2u mode                         
	set_lcm_register(0xCCc4, 0x00, 0);	//ccc5[7:0] : reg setting for signal15 selection with d2u mode                         
	set_lcm_register(0xCCc5, 0x00, 0);	//ccc6[7:0] : reg setting for signal16 selection with d2u mode                         
	set_lcm_register(0xCCc6, 0x00, 0);	//ccc7[7:0] : reg setting for signal17 selection with d2u mode                         
	set_lcm_register(0xCCc7, 0x00, 0);	//ccc8[7:0] : reg setting for signal18 selection with d2u mode                         
	set_lcm_register(0xCCc8, 0x00, 0);	//ccc9[7:0] : reg setting for signal19 selection with d2u mode                         
	set_lcm_register(0xCCc9, 0x00, 0);	//ccca[7:0] : reg setting for signal20 selection with d2u mode                         
	set_lcm_register(0xCCca, 0x00, 0);	//cccb[7:0] : reg setting for signal21 selection with d2u mode                         
	set_lcm_register(0xCCcb, 0x00, 0);	//cccc[7:0] : reg setting for signal22 selection with d2u mode                         
	set_lcm_register(0xCCcc, 0x00, 0);	//cccd[7:0] : reg setting for signal23 selection with d2u mode                         
	set_lcm_register(0xCCcd, 0x00, 0);	//ccce[7:0] : reg setting for signal24 selection with d2u mode                         
	set_lcm_register(0xCCce, 0x0E, 0);	//cccf[7:0] : reg setting for signal25 selection with d2u mode                         
	      
	// ccdx                                                                                                                        
	set_lcm_register(0xCCd0, 0x10, 0);	//ccd1[7:0] : reg setting for signal26 selection with d2u mode                         
	set_lcm_register(0xCCd1, 0x0A, 0);	//ccd2[7:0] : reg setting for signal27 selection with d2u mode                         
	set_lcm_register(0xCCd2, 0x0C, 0);	//ccd3[7:0] : reg setting for signal28 selection with d2u mode                         
	set_lcm_register(0xCCd3, 0x04, 0);	//ccd4[7:0] : reg setting for signal29 selection with d2u mode                         
	set_lcm_register(0xCCd4, 0x03, 0);	//ccd5[7:0] : reg setting for signal30 selection with d2u mode                         
	set_lcm_register(0xCCd5, 0x00, 0);	//ccd6[7:0] : reg setting for signal31 selection with d2u mode                         
	set_lcm_register(0xCCd6, 0x00, 0);	//ccd7[7:0] : reg setting for signal32 selection with d2u mode                         
	set_lcm_register(0xCCd7, 0x00, 0);	//ccd8[7:0] : reg setting for signal33 selection with d2u mode                         
	set_lcm_register(0xCCd8, 0x00, 0);	//ccd9[7:0] : reg setting for signal34 selection with d2u mode                         
	set_lcm_register(0xCCd9, 0x00, 0);	//ccda[7:0] : reg setting for signal35 selection with d2u mode                         
	set_lcm_register(0xCCda, 0x00, 0);	//ccdb[7:0] : reg setting for signal36 selection with d2u mode                         
	set_lcm_register(0xCCdb, 0x00, 0);	//ccdc[7:0] : reg setting for signal37 selection with d2u mode                         
	set_lcm_register(0xCCdc, 0x00, 0);	//ccdd[7:0] : reg setting for signal38 selection with d2u mode                         
	set_lcm_register(0xCCdd, 0x00, 0);	//ccde[7:0] : reg setting for signal39 selection with d2u mode                    
	set_lcm_register(0xCCde, 0x00, 0);	//ccdf[7:0] : reg setting for signal40 selection with d2u mode  

	#if 1 //def DISPLAY_DIRECTION_0_MODE
	set_lcm_register(0x3600, 0x00, 0);// Display Direction 0

	#ifdef TE_CONTROL
	set_lcm_register(0x3500, 0x00, 0);// TE( Fmark ) Signal On
	set_lcm_register(0x4400, 0x01, 0);
	set_lcm_register(0x4401, 0x22, 0);// TE( Fmark ) Signal Output Position: 0x22
	#endif
	#else	//def DISPLAY_DIRECTION_180_MODE
	set_lcm_register(0x3600, 0xD0, 0);// Display Direction 180

	#ifdef TE_CONTROL
	set_lcm_register(0x3500, 0x00, 0);// TE( Fmark ) Signal On
	set_lcm_register(0x4400, 0x01, 0);
	set_lcm_register(0x4401, 0xFF, 0);// TE( Fmark ) Signal Output Position
	#endif
	#endif

#if 0//def LCD_BACKLIGHT_CONTROL_MODE
	set_lcm_register(0x5100, 0xFF, 0);// Backlight Level Control
	set_lcm_register(0x5300, 0x2C, 0);// Backlight On
	set_lcm_register(0x5500, 0x00, 0);// CABC Function Off
#endif

	///=============================
	set_lcm_register(0x3A00, 0x55, 0);     //MCU 16bits D[17:0]
	set_lcm_register(0x2A00, 0x00, 0);
	set_lcm_register(0x2A01, 0x00, 0);
	set_lcm_register(0x2A02, HIGH_BYTE(FRAME_WIDTH), 0); 	// 0x01
	set_lcm_register(0x2A03, LOW_BYTE(FRAME_WIDTH), 0);	// 0xDF
	set_lcm_register(0x2B00, 0x00, 0);
	set_lcm_register(0x2B01, 0x00, 0);
	set_lcm_register(0x2B02, HIGH_BYTE(FRAME_HEIGHT), 0);  	// 0x03
	set_lcm_register(0x2B03, LOW_BYTE(FRAME_HEIGHT), 0);	// 0x1F

	send_ctrl_cmd(0x2C00); 	
	send_ctrl_cmd(0x1100);
	MDELAY(120);	
	send_ctrl_cmd(0x2900);
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
	params->type   = LCM_TYPE_DBI;
    params->ctrl   = LCM_CTRL_PARALLEL_DBI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    
    params->dbi.port                    = 0;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_52M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_LSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB666;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_32_BITS;
    params->dbi.io_driving_current      = 0;

    params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_OR_HSYNC;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;
    params->dbi.te_hs_delay_cnt         = 50;
    params->dbi.te_vs_width_cnt         = 277;
    params->dbi.te_vs_width_cnt_div     = LCM_DBI_TE_VS_WIDTH_CNT_DIV_16;

    params->dbi.parallel.write_setup    = 2;
    params->dbi.parallel.write_hold     = 2;
    params->dbi.parallel.write_wait     = 4;
    params->dbi.parallel.read_setup     = 2;
    params->dbi.parallel.read_latency   = 9;
    params->dbi.parallel.wait_period    = 0;
 */

/*
 * ATTENTIONS: There are a few important differences under the MT6575/15 compared with the MT6573/13
 * 
 *  type: if set LCM_TYPE_DBI, cpu interface.
 *  io_select_mode: if set 0, selects bus composition: LPA0, LWRB, LRDB control bus and NLD[0:15] data bus
 *                  if set 1, selects bus composition: DPIVSYNC, DPIDE, DPIHSYNC control bus and DPIRGB data bus.
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
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_104M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_16BITS; //LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST; // LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB565;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_16BITS; // LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_16_BITS; // LCM_DBI_CPU_WRITE_16_BITS;
	params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_6575_8MA; //LCM_DRIVING_CURRENT_8MA;

	params->dbi.parallel.write_setup    = 2;
	params->dbi.parallel.write_hold     = 3; //, 4
	params->dbi.parallel.write_wait     = 4; //6;
	
	params->dbi.parallel.read_setup     = 2; // 4; // 3;
	params->dbi.parallel.read_hold      = 2;
    params->dbi.parallel.read_latency   = 18;
	
	params->dbi.parallel.wait_period    = 1;  // 2;
	params->dbi.parallel.cs_high_width  = 0;  // 1; //cycles of cs high level between each transfer
	//params->dbi.parallel.read_latency   = 20;  //40
	//params->dbi.parallel.wait_period    = 10; // 0

	#ifdef	TE_CONTROL
	// enable tearing-free
	/*
	if (LCM_DBI_TE_MODE_DISABLED == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_Enable(FALSE));
        return;
    }

    if (LCM_DBI_TE_MODE_VSYNC_ONLY == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_SetMode(LCD_TE_MODE_VSYNC_ONLY));
    } else if (LCM_DBI_TE_MODE_VSYNC_OR_HSYNC == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_SetMode(LCD_TE_MODE_VSYNC_OR_HSYNC));
        LCD_CHECK_RET(LCD_TE_ConfigVHSyncMode(dbi->te_hs_delay_cnt,
                                              dbi->te_vs_width_cnt,
                     (LCD_TE_VS_WIDTH_CNT_DIV)dbi->te_vs_width_cnt_div));
    } else ASSERT(0);

    LCD_CHECK_RET(LCD_TE_SetEdgePolarity(dbi->te_edge_polarity));
    LCD_CHECK_RET(LCD_TE_Enable(TRUE));
	*/
    params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_OR_HSYNC; //LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING; //LCM_POLARITY_FALLING;

	/*
	 * Notes: The body of the function LCD_TE_ConfigVHSyncMode() is void!
	 */
	params->dbi.te_hs_delay_cnt         = 50;
    params->dbi.te_vs_width_cnt         = 277;
    params->dbi.te_vs_width_cnt_div     = LCM_DBI_TE_VS_WIDTH_CNT_DIV_16;
	#else
	params->dbi.te_mode                 = LCM_DBI_TE_MODE_DISABLED;
	#endif
}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);
    MDELAY(120);

    init_lcm_registers();
	lcm_update_black(0, 0, FRAME_WIDTH, FRAME_HEIGHT, 0x00);
}

static void lcm_suspend(void)
{
	send_ctrl_cmd(0x2800);
	MDELAY(50);
	send_ctrl_cmd(0x1000);
	MDELAY(120);	
	
}


static void lcm_resume(void)
{
	#if 1
	send_ctrl_cmd(0x1100);
	MDELAY(120);	
	send_ctrl_cmd(0x2900);
	MDELAY(50);
	#else
	unsigned int lcd_id = 0;
	
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(20);  
    SET_RESET_PIN(1);
    MDELAY(10);  // 400

	set_lcm_register(0xff00, 0x80,  0);
	set_lcm_register(0xff01, 0x09,  0);
	set_lcm_register(0xff02, 0x01,  0);
	set_lcm_register(0xff80, 0x80,  0);
	set_lcm_register(0xff81, 0x09,  0);
	UDELAY(10);
	
	lcd_id = get_lcd_id_n(LCM_ID_REG, 4) & 0xFF;
	lcd_id = (lcd_id << 8) | (get_lcd_id_n(LCM_ID_REG, 5) & 0xFF);

	LCD_PRINT(LCM_NAME"lcm_resume: lcd_id = 0x%x\n", lcd_id);

    init_lcm_registers();
	#endif
}


static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

	set_lcm_register(0x2A00, HIGH_BYTE(x0),  0);
	set_lcm_register(0x2A01, LOW_BYTE(x0),  0);
	set_lcm_register(0x2A02, HIGH_BYTE(x1),  0);
	set_lcm_register(0x2A03, LOW_BYTE(x1),  0);
	set_lcm_register(0x2B00, HIGH_BYTE(y0),  0);
	set_lcm_register(0x2B01, LOW_BYTE(y0),  0);
	set_lcm_register(0x2B02, HIGH_BYTE(y1),  0);  
	set_lcm_register(0x2B03, LOW_BYTE(y1),  0);

	send_ctrl_cmd(0x2c00);
}

static unsigned int get_lcd_id_r(unsigned int addr)
{
	unsigned short id = 0;
	
	send_ctrl_cmd(addr);
    id = read_data_cmd();

	LCD_PRINT(LCM_NAME"get_lcd_id_r: id = 0x%x\n", id);
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
	LCD_PRINT(LCM_NAME"get_lcd_id_n: id = 0x%x\n", id);
	
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
	unsigned int lcd_id = 0;

	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(50);  
    SET_RESET_PIN(1);
    MDELAY(120);

	set_lcm_register(0xff00, 0x80,  0);
	set_lcm_register(0xff01, 0x09,  0);
	set_lcm_register(0xff02, 0x01,  0);
	set_lcm_register(0xff80, 0x80,  0);
	set_lcm_register(0xff81, 0x09,  0);
	UDELAY(10);

	lcd_id = get_lcd_id_n(LCM_ID_REG, 4) & 0xFF;
	lcd_id = (lcd_id << 8) | (get_lcd_id_n(LCM_ID_REG, 5) & 0xFF);

	LCD_PRINT(LCM_NAME"lcm_compare_id: lcd_id = 0x%x\n", lcd_id);

		
    return (LCM_ID == lcd_id)?1:0;
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER gt397_otm8009a_lcm_drv = 
{
    .name			= "gt397_otm8009a",
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

