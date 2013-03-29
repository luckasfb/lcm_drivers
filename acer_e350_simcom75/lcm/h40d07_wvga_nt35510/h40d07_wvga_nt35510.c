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
	//************* Start Initial Sequence **********//
	send_ctrl_cmd(0xBC00);	send_data_cmd(0x00);  //VGMP/VGMN/VOOM Setting, VGMP=5.8V   #VGSP=0V  
	send_ctrl_cmd(0xBC01);	send_data_cmd(0xA0);
	send_ctrl_cmd(0xBC02);	send_data_cmd(0x00);

	send_ctrl_cmd(0xBD00);   send_data_cmd(0x00); //VGMN=-5.8V  #VGSN=0V   
	send_ctrl_cmd(0xBD01);   send_data_cmd(0xA0);
	send_ctrl_cmd(0xBD02);   send_data_cmd(0x00);

	send_ctrl_cmd(0xBE00);   send_data_cmd(0x00); //VCOM= -1V 
	send_ctrl_cmd(0xBE01);   send_data_cmd(0x40);

	send_ctrl_cmd(0xB000);   send_data_cmd(0x03);  //AVDD, Set AVDD 6.2V
 	send_ctrl_cmd(0xB001);   send_data_cmd(0x03);
 	send_ctrl_cmd(0xB002);   send_data_cmd(0x03);

	send_ctrl_cmd(0xB100);   send_data_cmd(0x00); //AVEE voltage, Set AVEE -6.5V  
 	send_ctrl_cmd(0xB101);   send_data_cmd(0x00);
	send_ctrl_cmd(0xB102);   send_data_cmd(0x00);

	send_ctrl_cmd(0xB900);   send_data_cmd(0x34);   //VGH, Set VGH 18V  
	send_ctrl_cmd(0xB901);   send_data_cmd(0x34);
 	send_ctrl_cmd(0xB902);   send_data_cmd(0x34);

	send_ctrl_cmd(0xBA00);   send_data_cmd(0x04); //VGLX, Set VGL -12V   
	send_ctrl_cmd(0xBA01);   send_data_cmd(0x04);
 	send_ctrl_cmd(0xBA02);   send_data_cmd(0x04);

	// ********************  EABLE PAGE 0 **************//
	send_ctrl_cmd(0xF000);	send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);	send_data_cmd(0xAA);     
	send_ctrl_cmd(0xF002);	send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);	send_data_cmd(0x08);
	send_ctrl_cmd(0xF004);	send_data_cmd(0x00);

	send_ctrl_cmd(0xB400);   send_data_cmd(0x10);//Vivid Color

	send_ctrl_cmd(0xB600);   send_data_cmd(0x02);//SDT  

	send_ctrl_cmd(0xB700);   send_data_cmd(0x70);//Set Gate EQ   
	send_ctrl_cmd(0xB701);   send_data_cmd(0x70);

	send_ctrl_cmd(0xB800);   send_data_cmd(0x01); //Set Source EQ   
	send_ctrl_cmd(0xB801);   send_data_cmd(0x07);
	send_ctrl_cmd(0xB802);   send_data_cmd(0x07);
	send_ctrl_cmd(0xB803);   send_data_cmd(0x07);

	send_ctrl_cmd(0xBC00);   send_data_cmd(0x05);//Inversion Control     
	send_ctrl_cmd(0xBC01);   send_data_cmd(0x05);
 	send_ctrl_cmd(0xBC02);   send_data_cmd(0x05);

	send_ctrl_cmd(0xBD00);   send_data_cmd(0x01); //Porch Adjust  
 	send_ctrl_cmd(0xBD01);   send_data_cmd(0x84);
	send_ctrl_cmd(0xBD02);   send_data_cmd(0x07);
	send_ctrl_cmd(0xBD03);   send_data_cmd(0x31);
 	send_ctrl_cmd(0xBD04);   send_data_cmd(0x00);

	send_ctrl_cmd(0xBE00);   send_data_cmd(0x01);   
	send_ctrl_cmd(0xBE01);   send_data_cmd(0x84);
	send_ctrl_cmd(0xBE02);   send_data_cmd(0x07);
	send_ctrl_cmd(0xBE03);   send_data_cmd(0x31);
	send_ctrl_cmd(0xBE04);   send_data_cmd(0x00);
 
	send_ctrl_cmd(0xBF00);   send_data_cmd(0x01);  
	send_ctrl_cmd(0xBF01);   send_data_cmd(0x84);
	send_ctrl_cmd(0xBF02);   send_data_cmd(0x07);
 	send_ctrl_cmd(0xBF03);   send_data_cmd(0x31);
 	send_ctrl_cmd(0xBF04);   send_data_cmd(0x00);

	//******************* ENABLE CMD2 PAGE0 **************//  

	send_ctrl_cmd(0xF000);	send_data_cmd(0x55);
	send_ctrl_cmd(0xF001);	send_data_cmd(0xAA);     
 	send_ctrl_cmd(0xF002);	send_data_cmd(0x52);
	send_ctrl_cmd(0xF003);	send_data_cmd(0x08);
 	send_ctrl_cmd(0xF004);	send_data_cmd(0x00);

	send_ctrl_cmd(0xC700);   send_data_cmd(0x02);   
 
	send_ctrl_cmd(0xC900);   send_data_cmd(0x11);   
	send_ctrl_cmd(0xC901);   send_data_cmd(0x00);
	send_ctrl_cmd(0xC902);   send_data_cmd(0x00);
 	send_ctrl_cmd(0xC903);   send_data_cmd(0x00);
 
	send_ctrl_cmd(0x3A00);   send_data_cmd(0x77); //Data format 16-Bits
 
	send_ctrl_cmd(0x3500);   send_data_cmd(0x00); //TE ON 

	send_ctrl_cmd(0x2100);   send_data_cmd(0x00); 

	send_ctrl_cmd(0x1100);   send_data_cmd(0x00);//StartUp  
 	MDELAY(120);

	send_ctrl_cmd(0x2900);   send_data_cmd(0x00); //Display On   	
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
//return 1 ;//amy0503 add 
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
LCM_DRIVER h40d07_nt35510_lcm_drv = 
{
    .name			= "h40d07_nt35510",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};



