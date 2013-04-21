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

#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)   //USED AS SDA INPUT
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

//chenhaojun
#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCM_ID 0x69 //0x5510

#define LCM_8090 //amy0418 
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


#define SET_LSDA_INPUT  SET_GPIO_INPUT(LSA0_GPIO_PIN)
#define SET_LSDA_OUTPUT  SET_GPIO_OUTPUT(LSDA_GPIO_PIN)

#define GET_LSDA_BIT mt_get_gpio_in(LSA0_GPIO_PIN)

#define CTRL_ID  (0 << 8)
#define DATA_ID  (1 << 8)

#if defined(LCM_8090)//amy0418 add 
static __inline void spi_send_data(unsigned int data)
{
    unsigned int i;

    SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
    UDELAY(1);
    SET_LSCE_LOW;
    for (i = 0; i < 9; ++ i)
    {
        SET_LSCK_LOW;
        if (data & (1 << 8)) {
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

    reg =  (CTRL_ID | (reg & 0xFF));
    
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
    UDELAY(1);

    for (i = 0; i < 9; ++ i)
    {
        SET_LSCK_LOW;
        if (reg & (1 << 8)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        reg <<= 1;
    }
    
    SET_LSDA_INPUT;
    UDELAY(2);    
    SET_LSCK_LOW;
    UDELAY(2);      
    
    for (k = 7; k >= 0; k--)
    {
        if (GET_LSDA_BIT) {
            getbyte |= (1 << k);
        }
        SET_LSCK_HIGH;        
        UDELAY(2);
        SET_LSCK_LOW;
        UDELAY(2);
    }    

    UDELAY(2);    
    
    SET_LSDA_OUTPUT;
    
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

    return getbyte;
}



static __inline void send_ctrl_cmd(unsigned int cmd)
{
    unsigned int out = (CTRL_ID | (cmd & 0xFF));
    spi_send_data(out);
}

static __inline void send_data_cmd(unsigned int data)
{
    unsigned int out = (DATA_ID  | (data & 0xFF));
    spi_send_data(out);
}
#else
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
    
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
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
    
    SET_LSDA_INPUT;
    UDELAY(2);    
  //  SET_LSCK_LOW;
   // UDELAY(2);      
       SET_LSCK_LOW;
    UDELAY(2);      
    SET_LSCK_HIGH;        
    UDELAY(2);
    
    for (k = 7; k >= 0; k--)
    {
        if (GET_LSDA_BIT) {
            getbyte |= (1 << k);
        }
       SET_LSCK_LOW;       
        UDELAY(5);
       SET_LSCK_HIGH;    
        UDELAY(5);
    }    

    UDELAY(2);    
    
    SET_LSDA_OUTPUT;
    
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

#endif
static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}

static void init_lcm_registers(void)
{
	//************* Start Initial Sequence **********// 

	send_ctrl_cmd(0xB9);  // SET password 
	send_data_cmd(0xFF);   
	send_data_cmd(0x83);   
	send_data_cmd(0x69);   


	send_ctrl_cmd(0xB2);  // SET Display  480x800 
	send_data_cmd(0x00);   
	send_data_cmd(0x2B); //res[2:0] rn dfr dm[1:0]   2B
	send_data_cmd(0x03);     //bp0-7  0a
	send_data_cmd(0x03);      //fp 0-7  0a
	send_data_cmd(0x70);     //  驱动能羂u0160
	send_data_cmd(0x00);  //gamma    on 
	send_data_cmd(0xFF);     // gamma off
	send_data_cmd(0x00);     
	send_data_cmd(0x00);    // TE 1 FRAME OUTPUT
	send_data_cmd(0x00);   //te 输出位置 8-9
	send_data_cmd(0x00);    //te 输出位置 0-1,设置第一行
	send_data_cmd(0x03);   //idel bp
	send_data_cmd(0x03);   //idel fp
	send_data_cmd(0x00);  //idel fp
	send_data_cmd(0x01); 

	send_ctrl_cmd(0xB3);  // SET VCOM 
	send_data_cmd(0x03);   //0x03


	send_ctrl_cmd(0xB4);  // SET Display  480x800 
	send_data_cmd(0x00); //穃u017d转穃u0153蔦u0153   02-dot inversion  00 OK 
	send_data_cmd(0x1D); //  soure on
	send_data_cmd(0x80); //   soure off
	send_data_cmd(0x06); //eq on  
	send_data_cmd(0x02);  //eqs 


	send_ctrl_cmd(0xB6);  // SET VCOM 
	send_data_cmd(0x3F);//3A   
	send_data_cmd(0x3F); //3A  



	send_ctrl_cmd(0xD5);  // SETGIP   
	send_data_cmd(0x00);  //SHR 8-11 
	send_data_cmd(0x01);  //SHR 0-7    6

	send_data_cmd(0x03);    //SHR1 8-11 
	send_data_cmd(0x25);  //SHR1 0-7    //reset 808

	send_data_cmd(0x01); //SPD    stv delay
	send_data_cmd(0x02); //CHR         8

	send_data_cmd(0x28); //CON     ck delay
	send_data_cmd(0x70); //COFF 

	send_data_cmd(0x11); //SHP  SCP  stv high 1 hsync  stv 周期
	send_data_cmd(0x13); //CHP  CCP  CK HIGH 1 HSYNC CK周期3

	send_data_cmd(0x00); //CGOUT10_L CGOUT9_L  ML=0 
	send_data_cmd(0x00); //CGOUT10_R   
	send_data_cmd(0x40); //CGOUT6_L CGOUT5_L    //40 
	send_data_cmd(0x26); //CGOUT8_L CGOUT7_L   //26 
	send_data_cmd(0x51); //CGOUT6_R CGOUT5_R   //51 
	send_data_cmd(0x37); //CGOUT8_R CGOUT7_R   //37 

	send_data_cmd(0x00); //CGOUT10_L CGOUT9_L ML=1   
	send_data_cmd(0x00); //CGOUT10_R CGOUT9_R   
	send_data_cmd(0x71); //CGOUT6_L CGOUT5_L   
	send_data_cmd(0x35); //CGOUT8_L CGOUT7_L   
	send_data_cmd(0x60); //CGOUT6_R CGOUT5_R   
	send_data_cmd(0x24); //CGOUT8_R CGOUT7_R 

	send_data_cmd(0x07);   // GTO
	send_data_cmd(0x0F);    // GNO
	send_data_cmd(0x04);    // EQ DELAY
	send_data_cmd(0x04);  // GIP 
	///////////////////////////////////////////////////////////////////////////////////   
	// MDELAY(10); 

	//________________________________________stat ___ok_______   
	send_ctrl_cmd(0xB1);  //Set Power 
	send_data_cmd(0x01); 
	send_data_cmd(0x00); 
	send_data_cmd(0x34); 
	send_data_cmd(0x03); //03
	send_data_cmd(0x00); 
	send_data_cmd(0x11);  //VSP voltage setting[4:0]   0F=>10
	send_data_cmd(0x11);   //VSN voltage setting=-VSP[4:0]  0F=>10
	send_data_cmd(0x32); // 32
	send_data_cmd(0x2F); // 2F
	send_data_cmd(0x3F); //3f
	send_data_cmd(0x3F); //3f
	send_data_cmd(0x01); //01
	send_data_cmd(0x3A); // 3a 
	send_data_cmd(0x01); 
	send_data_cmd(0xE6); 
	send_data_cmd(0xE6); 
	send_data_cmd(0xE6); 
	send_data_cmd(0xE6); 
	send_data_cmd(0xE6); 


	send_ctrl_cmd(0x11);     
	MDELAY(120);
	//////////////////////////////////////////////////////

	send_ctrl_cmd(0xE0); // Set Gamma 2.2 
	send_data_cmd(0x00);  //G1_VRP0[5:0]
	send_data_cmd(0x02);  //G1_VRP1[5:0]    
	send_data_cmd(0x0B);  //G1_VRP2[5:0]    
	send_data_cmd(0x13);  //G1_VRP3[5:0]
	send_data_cmd(0x0B);  //G1_VRP4[5:0]
	send_data_cmd(0x18);  //G1_VRP5[5:0]
	send_data_cmd(0x24);  //G1_PRP0[6:0]    12
	send_data_cmd(0x30);  //G1_PRP1[6:0] 2E  
	send_data_cmd(0x09); //G1_CGMP0[1:0]    G1_PKP0[4:0]
	send_data_cmd(0x13); //G1_CGMP1[1:0]    G1_PKP1[4:0]
	send_data_cmd(0x0F); //G1_CGMP2[1:0]    G1_PKP2[4:0]
	send_data_cmd(0x15); //G1_CGMP3[1:0]    G1_PKP3[4:0]
	send_data_cmd(0x15); //G1_CGMP4,G1_CGMP5    G1_PKP4[4:0]
	send_data_cmd(0x14);  //G1_PKP5[4:0]
	send_data_cmd(0x15);  //G1_PKP6[4:0]
	send_data_cmd(0x0C);  //G1_PKP7[4:0]    8
	send_data_cmd(0x0C);  //G1_PKP8[4:0]
	//*******************************************
	send_data_cmd(0x00);  //G1_VRP0[5:0]
	send_data_cmd(0x02);  //G1_VRP1[5:0]    
	send_data_cmd(0x0B);  //G1_VRP2[5:0]    
	send_data_cmd(0x13);  //G1_VRP3[5:0]
	send_data_cmd(0x0B);  //G1_VRP4[5:0]
	send_data_cmd(0x18);  //G1_VRP5[5:0]
	send_data_cmd(0x24);  //G1_PRP0[6:0]    12
	send_data_cmd(0x30);  //G1_PRP1[6:0]     2E  
	send_data_cmd(0x09); //G1_CGMP0[1:0]    G1_PKP0[4:0]
	send_data_cmd(0x13); //G1_CGMP1[1:0]    G1_PKP1[4:0]
	send_data_cmd(0x0F); //G1_CGMP2[1:0]    G1_PKP2[4:0]
	send_data_cmd(0x15); //G1_CGMP3[1:0]    G1_PKP3[4:0]
	send_data_cmd(0x15); //G1_CGMP4,G1_CGMP5    G1_PKP4[4:0]
	send_data_cmd(0x14);  //G1_PKP5[4:0]
	send_data_cmd(0x15);  //G1_PKP6[4:0]
	send_data_cmd(0x0C);  //G1_PKP7[4:0]    8
	send_data_cmd(0x0C);  //G1_PKP8[4:0]


	send_ctrl_cmd(0x3A); 
	send_data_cmd(0x77);   

//	send_ctrl_cmd(0xcc);   //amy0419 add
//	send_data_cmd(0x00);    //amy0419 add
//


	send_ctrl_cmd(0x29);  
	MDELAY(100); 
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

        lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);               // GPIO in
        lcm_util.set_gpio_pull_enable(LSA0_GPIO_PIN, 0);

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
    params->dpi.mipi_pll_clk_div1 = 42;//40?
    params->dpi.mipi_pll_clk_div2 = 10;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_RISING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 14;
    params->dpi.hsync_back_porch  =4;
    params->dpi.hsync_front_porch = 15;
    params->dpi.vsync_pulse_width = 2;
    params->dpi.vsync_back_porch  = 3;
    params->dpi.vsync_front_porch = 3;
    
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

    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(50);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
   
    send_ctrl_cmd(0x10);
    MDELAY(100);

}


static void lcm_resume(void)
{

    send_ctrl_cmd(0x11);
    MDELAY(100);

 
}

static unsigned int lcm_compare_id(void)
{
	unsigned int    id1 = 0;
	unsigned int    id2 = 0;
	unsigned int    id = 0;
//return 0 ; //amy0522  add 
    config_gpio();

	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(20);

	set_lcm_register(0xF000,0x55);             
	set_lcm_register(0xF001,0xAA);             
	set_lcm_register(0xF002,0x52);             
	set_lcm_register(0xF003,0x08);             
	set_lcm_register(0xF004,0x01);
	
	send_ctrl_cmd(0xc500);
	id1 = spi_get_byte(0xc0);  //spi_read_data();
	MDELAY(20);
	send_ctrl_cmd(0xc501);
	id2 = spi_get_byte(0xc0);  //spi_read_data();

	id = (id1<<8)|id2;

#ifdef BUILD_UBOOT
	printf("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#else
	printk("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#endif

	return (LCM_ID == id)?1:0;
}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks  t040kwv048at_wvga_HX8369
// ---------------------------------------------------------------------------
LCM_DRIVER t040kwv048at_hx8369_lcm_drv = 
{
	.name		= "t040kwv048at_hx8369",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id   = lcm_compare_id,
};

//	.compare_id     = lcm_compare_id


