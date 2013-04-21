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


#define SET_LSDA_INPUT  SET_GPIO_INPUT(LSDA_GPIO_PIN)
#define SET_LSDA_OUTPUT  SET_GPIO_OUTPUT(LSDA_GPIO_PIN)

#define GET_LSDA_BIT mt_get_gpio_in(LSDA_GPIO_PIN)


#ifdef BUILD_UBOOT
#define dbg_LCM	printf
#else
#define dbg_LCM printk
#endif

static __inline void send_ctrl_cmd(unsigned char cmd)
{
    unsigned char i;

    SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_LOW;//A0=0
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (cmd & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        cmd <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
}

static __inline void send_data_cmd(unsigned char data)
{
    unsigned char i;

    SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_HIGH;//A0=1
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (data & (1 << 7)) {
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


static __inline unsigned int  spi_get_byte(unsigned int reg)
{
    unsigned int i;
    int k;
    unsigned int getbyte=0;

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

    UDELAY(2);        
    SET_LA0_INPUT;
    UDELAY(2);    
    SET_LSCK_LOW;
    UDELAY(2);      
    SET_LSCK_HIGH;        
    UDELAY(2);

    for (k = 39; k >= 0; k--)
    {
        if (GET_LSA0_BIT) {
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
	
    return (getbyte & 0xffff);
}





static __inline void set_lcm_register(unsigned char regIndex,
                                      unsigned char regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}

static  __inline void backlight_ctrl(unsigned char cmd)
{
	/*N/A*/
    return;
}

static void init_lcm_registers(void)
{
//------LCD Init Start------
send_ctrl_cmd(0x00B0); //B0h
send_data_cmd(0x0004);

send_ctrl_cmd(0x00B3); //B3h
send_data_cmd(0x0010);

send_ctrl_cmd(0x00D6); //D6h
send_data_cmd(0x00A8);
send_data_cmd(0x0000);

send_ctrl_cmd(0x00BD); //BDh
send_data_cmd(0x0000);


send_ctrl_cmd(0x00C0);//
send_data_cmd(0x001B);//VSPL[4]; HSPL[3]; 0; EPL[1]; DPL[0]; 
send_data_cmd(0x0066);

send_ctrl_cmd(0x00C1);
send_data_cmd(0x0023); //SEQ_SEL WCVDC2 EN_6Fh xx REV xx BGR SS
send_data_cmd(0x0031); //NL[5:0);
send_data_cmd(0x0099); //BLREV[1:0] PTREV GIPPAT[2:0] GIPMODE[1:0]
send_data_cmd(0x0021); //GSPF[5:0]
send_data_cmd(0x0020); //GSPS[5:0]
send_data_cmd(0x0000); //STVG[1:0] STVGA[1:0] xx T_GALH GLOL[1:0]
send_data_cmd(0x0010); //DVI[3:0] xx xx FL1 xx
send_data_cmd(0x0028); //RTN
send_data_cmd(0x000c); //BPX16 VBPE xx BP[4:0]
send_data_cmd(0x000C); //EFX16 xx xx FP[4:0]
send_data_cmd(0x0000); //ACBF1[1:0] ACF[1:0] ACBR[1:0] ACR[1:0]
send_data_cmd(0x0000); //ACBF2[1:0] ACF2[1:0] ACBR2[1:0] ACR2[1:0]
send_data_cmd(0x0000); //ACCYC[1:0] VGSET ACFIX[1:0]
send_data_cmd(0x0021);
send_data_cmd(0x0001);

send_ctrl_cmd(0x00C2); //C2h inversion
send_data_cmd(0x0010);//0x00 1-line;  0x28 Column inversion
send_data_cmd(0x0006);
send_data_cmd(0x0006);
send_data_cmd(0x0001);
send_data_cmd(0x0003);
send_data_cmd(0x0000);
 
send_ctrl_cmd(0x00C8); //c8h
send_data_cmd(0x0000);
send_data_cmd(0x000E);
send_data_cmd(0x0017);
send_data_cmd(0x0020);
send_data_cmd(0x002E);
send_data_cmd(0x004B);
send_data_cmd(0x003B);
send_data_cmd(0x0028);
send_data_cmd(0x0019);
send_data_cmd(0x0011);
send_data_cmd(0x000A);
send_data_cmd(0x0002);
send_data_cmd(0x0000);
send_data_cmd(0x000E);
send_data_cmd(0x0015);
send_data_cmd(0x0020);
send_data_cmd(0x002E);
send_data_cmd(0x0047);
send_data_cmd(0x003B);
send_data_cmd(0x0028);
send_data_cmd(0x0019);
send_data_cmd(0x0011);
send_data_cmd(0x000A);
send_data_cmd(0x0002);

send_ctrl_cmd(0x00C9); //c9h
send_data_cmd(0x0000);
send_data_cmd(0x000E);
send_data_cmd(0x0017);
send_data_cmd(0x0020);
send_data_cmd(0x002E);
send_data_cmd(0x004B);
send_data_cmd(0x003B);
send_data_cmd(0x0028);
send_data_cmd(0x0019);
send_data_cmd(0x0011);
send_data_cmd(0x000A);
send_data_cmd(0x0002);
send_data_cmd(0x0000);
send_data_cmd(0x000E);
send_data_cmd(0x0015);
send_data_cmd(0x0020);
send_data_cmd(0x002E);
send_data_cmd(0x0047);
send_data_cmd(0x003B);
send_data_cmd(0x0028);
send_data_cmd(0x0019);
send_data_cmd(0x0011);
send_data_cmd(0x000A);
send_data_cmd(0x0002);

send_ctrl_cmd(0x00CA); //CAh
send_data_cmd(0x0000);
send_data_cmd(0x000E);
send_data_cmd(0x0017);
send_data_cmd(0x0020);
send_data_cmd(0x002E);
send_data_cmd(0x004B);
send_data_cmd(0x003B);
send_data_cmd(0x0028);
send_data_cmd(0x0019);
send_data_cmd(0x0011);
send_data_cmd(0x000A);
send_data_cmd(0x0002);
send_data_cmd(0x0000);
send_data_cmd(0x000E);
send_data_cmd(0x0015);
send_data_cmd(0x0020);
send_data_cmd(0x002E);
send_data_cmd(0x0047);
send_data_cmd(0x003B);
send_data_cmd(0x0028);
send_data_cmd(0x0019);
send_data_cmd(0x0011);
send_data_cmd(0x000A);
send_data_cmd(0x0002);

send_ctrl_cmd(0x00D0); //D0h
send_data_cmd(0x0029);
send_data_cmd(0x0003);
send_data_cmd(0x00CE);
send_data_cmd(0x00A6);
send_data_cmd(0x000C);
send_data_cmd(0x0043);
send_data_cmd(0x0020);
send_data_cmd(0x0010);
send_data_cmd(0x0001);
send_data_cmd(0x0000);
send_data_cmd(0x0001);
send_data_cmd(0x0001);
send_data_cmd(0x0000);
send_data_cmd(0x0003);
send_data_cmd(0x0001);
send_data_cmd(0x0000);

send_ctrl_cmd(0x00D1); //D1h
send_data_cmd(0x0018);
send_data_cmd(0x000C);
send_data_cmd(0x0023);
send_data_cmd(0x0003);
send_data_cmd(0x0075);
send_data_cmd(0x0002);
send_data_cmd(0x0050);

send_ctrl_cmd(0x00D3); //D3h
send_data_cmd(0x0033);

send_ctrl_cmd(0x00D5); //D5h
send_data_cmd(0x002A);
send_data_cmd(0x002A);

send_ctrl_cmd(0x00DE); //DEh
send_data_cmd(0x0001);
send_data_cmd(0x0051);

send_ctrl_cmd(0x00E6); //E6h
send_data_cmd(0x0051);

send_ctrl_cmd(0x00FA); //FAh
send_data_cmd(0x0003);

send_ctrl_cmd(0x00D6); //D6h
send_data_cmd(0x0028);
MDELAY(100);

send_ctrl_cmd(0x002A);
send_data_cmd(0x0000);
send_data_cmd(0x0000);
send_data_cmd(0x0001);
send_data_cmd(0x00DF);

send_ctrl_cmd(0x002B);
send_data_cmd(0x0000);
send_data_cmd(0x0000);
send_data_cmd(0x0003);
send_data_cmd(0x001F);

send_ctrl_cmd(0x00E1);
send_data_cmd(0x0000);
send_data_cmd(0x0000);
send_data_cmd(0x0003);
send_data_cmd(0x001F);
send_data_cmd(0x0003);
send_data_cmd(0x001F);

send_ctrl_cmd(0x0036);
send_data_cmd(0x0000);
 
send_ctrl_cmd(0x003A);
send_data_cmd(0x0077);
 
send_ctrl_cmd(0x0011);
MDELAY(200);

send_ctrl_cmd(0x0029);
MDELAY(20);
 
send_ctrl_cmd(0x002C);
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

    params->dpi.clk_pol           = LCM_POLARITY_RISING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 8;
    params->dpi.hsync_back_porch  = 10;//5;
    params->dpi.hsync_front_porch = 120;//5;
    params->dpi.vsync_pulse_width =2;// 4;
    params->dpi.vsync_back_porch  = 15;//5;
    params->dpi.vsync_front_porch = 12;//8;
   
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_4MA;
}


static void lcm_init(void)
{
	config_gpio();

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(100);
	SET_RESET_PIN(1);
	MDELAY(200);

	init_lcm_registers();
	//backlight
	backlight_ctrl(1);
}


static void lcm_suspend(void)
{
	//backlight
	backlight_ctrl(0);

	send_ctrl_cmd(0x28);    
	MDELAY(100); 
	send_ctrl_cmd(0x10); //ENTER_SLEEP_MODE 
	MDELAY(120); 
}


static void lcm_resume(void)
{
	send_ctrl_cmd(0x11); //EXIT_SLEEP_MODE 
	MDELAY(120); 
	send_ctrl_cmd(0x29);    
	MDELAY(100); 
	//backlight
	backlight_ctrl(1);
}

static void lcm_setbacklight(unsigned int level)
{

    backlight_ctrl(1);

}

#define LCM_ID  0x1408
static unsigned int lcm_compare_id(void)
{
	unsigned int ID; 			
  
	config_gpio();

	SET_RESET_PIN(1);
	MDELAY(50);
	SET_RESET_PIN(0);
	MDELAY(200);
	SET_RESET_PIN(1);
	MDELAY(300);

	send_ctrl_cmd(0xB0); //B0h
	send_data_cmd(0x04);	
	MDELAY(5);
	send_ctrl_cmd(0xf5);
   

	ID=spi_get_byte(0xbf);
	
	dbg_LCM("------------------------------> r61408_ID=%x\n",ID);
	
   return (LCM_ID == ID)?1:0;	

  //   return 1;
}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER p2617_wvga_r61408_lcm_drv = 
{
    .name			= "p2617_r61408",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};
