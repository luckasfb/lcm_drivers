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
#include <asm/arch/mt6573_gpio.h>
#else
#include <mach/mt6573_gpio.h>
#endif
#include "lcm_drv.h"

#define dbg_print //printf

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

/* #define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN) */
/* #define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN) */
/* #define LSDA_GPIO_PIN (GPIO_DISP_LSA0_PIN) */
/* #define LSA0_GPIO_PIN (GPIO_DISP_LSDA_PIN) */

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (480)

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

#define ICID_MODE 0
#define SET_ICID_HIGH SET_GPIO_OUT(GPIO36, 1)
#define SET_ICID_LOW SET_GPIO_OUT(GPIO36, 0)

#define GET_LSA0_IN mt_get_gpio_in(LSA0_GPIO_PIN)
static __inline void spi_send_data(unsigned int data)
{
  unsigned int i;

  int clk_num;
  int clk_mask;

  UDELAY(100);
  SET_LSCK_HIGH;
  SET_LSDA_HIGH;
  UDELAY(100);

  clk_num = 9;
  clk_mask = 0x100;
  dbg_print("\n^");
  UDELAY(50);
  for (i = 0; i < clk_num; ++ i)
    {
	  SET_LSCK_LOW;
	  if (data & (clk_mask)) {  // 0x80= (1 << (9-1))
		SET_LSDA_HIGH;
		dbg_print("~1");
	  } else {
		SET_LSDA_LOW;
		dbg_print("~0");
	  }
	  UDELAY(100);
	  SET_LSCK_HIGH;
	  UDELAY(100);
	  data <<= 1;
    }
  dbg_print("\n");

  SET_ICID_HIGH;

  SET_LSDA_HIGH;
}

static __inline void spi_send_data_ICID(unsigned int data)
{
  unsigned int i;

  UDELAY(100);
  SET_LSCK_HIGH;
  SET_LSDA_HIGH;
  UDELAY(100);

  for (i = 0; i < 9; ++ i)
    {
	  SET_LSCK_LOW;
	  if (data & (0x100)) {  // 0x80= (1 << (9-1))
		SET_LSDA_HIGH;
		dbg_print("~1");
	  } else {
		SET_LSDA_LOW;
		dbg_print("~0");
	  }
	  UDELAY(100);
	  SET_LSCK_HIGH;
	  UDELAY(100);
	  data <<= 1;
    }

  SET_LSDA_HIGH;
}

static __inline unsigned int spi_read_data(void)
{
  unsigned int i;
  s32 inA0 = -1;

  UDELAY(100);
  SET_LSCK_HIGH;
  SET_LSDA_HIGH;
  UDELAY(100);

  for (i = 0; i < 9; ++ i)
    {
	  SET_LSCK_LOW;
	  /* if (data & (0x100)) {  // 0x80= (1 << (9-1)) */
	  /* 	SET_LSDA_HIGH; */
	  /* 	dbg_print("~1"); */
	  /* } else { */
	  /* 	SET_LSDA_LOW; */
	  /* 	dbg_print("~0"); */
	  /* } */
	  UDELAY(100);
	  inA0 = GET_LSA0_IN;
	  //printf("`%d",inA0);
	  SET_LSCK_HIGH;
	  UDELAY(100);
    }
  //printf("\n");


  SET_LSDA_HIGH;
}

static __inline void send_ctrl_cmd(unsigned int cmd)
{
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): cmd:0x%x \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__,cmd); 

  unsigned int out = (cmd & 0xFF); 
  spi_send_data(out);
}

static __inline void send_data_cmd(unsigned int data)
{
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): : data:0x%x \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__, data); 

  unsigned int out = (data & 0xFF)| 0x100;
  spi_send_data(out);
}

#define LCD_WRITE_CMD(cmd) SET_LSCE_LOW;send_ctrl_cmd(cmd);SET_LSCE_HIGH
#define LCD_WRITE_DATA(data) SET_LSCE_LOW;send_data_cmd(data);SET_LSCE_HIGH


static void InitLCD_ILI9481_HVGA_CPT32()
{
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): :  \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__); 
  //************* Start Initial Sequence **********//
  MDELAY(100); // Delay 100 ms
  LCD_WRITE_CMD(0x11); //Exit Sleep
  MDELAY(20);

  LCD_WRITE_CMD(0xD0); //Power Setting
  LCD_WRITE_DATA(0x07);
  LCD_WRITE_DATA(0x42);
  LCD_WRITE_DATA(0x18);

  LCD_WRITE_CMD(0xD1); //Vcom control
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x16);//07
  LCD_WRITE_DATA(0x0E);

  LCD_WRITE_CMD(0xD2); //Power setting for Normal mode
  LCD_WRITE_DATA(0x01);
  LCD_WRITE_DATA(0x02);

  LCD_WRITE_CMD(0xC0); //Panel Driver setting.
  LCD_WRITE_DATA(0x10);
  LCD_WRITE_DATA(0x3B);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x02);
  LCD_WRITE_DATA(0x11);

  LCD_WRITE_CMD(0xC5); //Frame rate and inversion control
  LCD_WRITE_DATA(0x03);

  LCD_WRITE_CMD(0xC8); //Gamma setting
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x32);
  LCD_WRITE_DATA(0x36);
  LCD_WRITE_DATA(0x45);
  LCD_WRITE_DATA(0x06);
  LCD_WRITE_DATA(0x16);
  LCD_WRITE_DATA(0x37);
  LCD_WRITE_DATA(0x75);
  LCD_WRITE_DATA(0x77);
  LCD_WRITE_DATA(0x54);
  LCD_WRITE_DATA(0x0C);
  LCD_WRITE_DATA(0x00);

  LCD_WRITE_CMD(0x36); //Set address mode
  //LCD_WRITE_DATA(0x0A);
  LCD_WRITE_DATA(0x09); //Flip Vertically and Horizontally
  
  LCD_WRITE_CMD(0x3A);// set pixel format
  LCD_WRITE_DATA(0x66);

  //  LCD_WRITE_CMD(0x0C); //get pixel format
  //  LCD_WRITE_DATA(0x66);

  LCD_WRITE_CMD(0x2A); //set colum address
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x01);
  LCD_WRITE_DATA(0x40);
  
  LCD_WRITE_CMD(0x2B);//set page address
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x01);
  LCD_WRITE_DATA(0xE0);
  MDELAY(120);

  LCD_WRITE_CMD(0xB4); //Display mode and Frame memory write mode setting.
  LCD_WRITE_DATA(0x10);

  LCD_WRITE_CMD(0x29);//set display on 

  LCD_WRITE_CMD(0x2C);// write memory start
}


static void config_gpio(void)
{
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): :  \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__); 

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
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): :  \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__); 

  memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): :  \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__); 

  memset(params, 0, sizeof(LCM_PARAMS));

  params->type   = LCM_TYPE_DPI;
  params->ctrl   = LCM_CTRL_GPIO;
  params->width  = FRAME_WIDTH;
  params->height = FRAME_HEIGHT;

  // CLK
  params->dpi.mipi_pll_clk_ref  = 1;
  params->dpi.mipi_pll_clk_div1 = 8;
  params->dpi.mipi_pll_clk_div2 = 3;
  params->dpi.dpi_clk_div       = 4;
  params->dpi.dpi_clk_duty      = 2;

  params->dpi.clk_pol           = LCM_POLARITY_FALLING;
  //params->dpi.de_pol            = LCM_POLARITY_RISING;
  params->dpi.de_pol            = LCM_POLARITY_RISING;
  params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
  params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

  // Hsync Vsync
  params->dpi.hsync_pulse_width = 1;
  params->dpi.hsync_back_porch  = 0;
  params->dpi.hsync_front_porch = 0;
  params->dpi.vsync_pulse_width = 1;
  params->dpi.vsync_back_porch  = 0;
  params->dpi.vsync_front_porch = 0;

  params->dpi.format            = LCM_DPI_FORMAT_RGB666;
  params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
  params->dpi.is_serial_output  = 0;

  params->dpi.intermediat_buffer_num = 1;

  params->dpi.io_driving_current = LCM_DRIVING_CURRENT_4MA;
}


static void lcm_init(void)
{
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): :  \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__); 

  config_gpio();

  SET_RESET_PIN(0);
  SET_LSCE_HIGH;
  SET_LSCK_HIGH;

  MDELAY(300);
  SET_RESET_PIN(1);
  MDELAY(2);

  InitLCD_ILI9481_HVGA_CPT32();
}


static void lcm_suspend(void)
{
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): :  \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__); 

  LCD_WRITE_CMD(0x10);
}


static void lcm_resume(void)
{
  dbg_print("===> [ILI9481_DPI] <%s %s> %s:%d %s(): :  \n", __DATE__, __TIME__, __FILE__, __LINE__, __func__); 

  LCD_WRITE_CMD(0x11);
  LCD_WRITE_CMD(0x29);
  MDELAY(120);
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------


LCM_DRIVER ili9481_dpi_lcm_drv =
  {
	.name = "ili9481_dpi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume
  };



const LCM_DRIVER* LCM_GetDriver()
{
    static const LCM_DRIVER LCM_DRV =
    {
        .set_util_funcs = lcm_set_util_funcs,
        .get_params     = lcm_get_params,
        .init           = lcm_init,
        .suspend        = lcm_suspend,
        .resume         = lcm_resume
    };

    return &LCM_DRV;
}

