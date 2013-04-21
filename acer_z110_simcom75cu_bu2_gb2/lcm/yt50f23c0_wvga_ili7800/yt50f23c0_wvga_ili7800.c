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

#define ILI7800_SPI_8BIT
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

//#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

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

//punk
#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))
#define SET_LSDA_INPUT  SET_GPIO_INPUT(LSDA_GPIO_PIN)
#define SET_LSDA_OUTPUT  SET_GPIO_OUTPUT(LSDA_GPIO_PIN)
#define GET_LSDA_BIT	 mt_get_gpio_in(LSDA_GPIO_PIN)

static __inline void spi_send_data(unsigned int data) 
{
    unsigned int i;

    SET_LSCE_HIGH;
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

//punk, start
//#define RISING_TRIGGER
#define CTRL_ID  (0 << 8)
#define DATA_ID  (1 << 8)
static __inline void spi_send_data_9bit(unsigned int data)
{
    unsigned int i;
#ifdef  RISING_TRIGGER
	xx
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
    UDELAY(1);

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
#else
    SET_LSCE_HIGH;
    UDELAY(4);
	SET_LSCK_LOW;
    UDELAY(4);
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);

    for (i = 0; i < 9; ++ i)
    {
        SET_LSCK_HIGH;
        if (data & (1 << 8)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
		SET_LSCK_LOW;
        UDELAY(1);
        data <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
#endif
    
}


static __inline void spi_send_data_9bit_internal(unsigned int data)
{
    unsigned int i;
#ifdef  RISING_TRIGGER
    UDELAY(1);

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

#else
    UDELAY(1);

    for (i = 0; i < 9; ++ i)
    {
        SET_LSCK_HIGH;
        if (data & (1 << 8)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
		SET_LSCK_LOW;
        UDELAY(1);
        data <<= 1;
    }

#endif
    
}


static __inline unsigned char  spi_get_bytes_internal(unsigned int count,unsigned int buf[])
{
	int i,k;

	for(i=0;i<count;i++)
	{
		buf[i] = 0;
	}

#ifdef  RISING_TRIGGER
	for(i=0;i<count;i++)
	{
		    SET_LSCK_LOW;       
			UDELAY(5);
		   SET_LSCK_HIGH;    
			for (k = 7; k >= 0; k--)
			{
				if (GET_LSDA_BIT) {
					buf[i] |= (1 << k);
			}
			UDELAY(5);
		}    
	}

#else
	for(i=0;i<count;i++)
	{
		   SET_LSCK_HIGH;    
			UDELAY(5);
		    SET_LSCK_LOW;       
			for (k = 7; k >= 0; k--)
			{
				if (GET_LSDA_BIT) {
					buf[i] |= (1 << k);
			}
			UDELAY(5);
		}    
	}
#endif

    UDELAY(2);    
    
}

static __inline void send_ctrl_cmd_8bit(unsigned int cmd)
{
    unsigned int out = (CTRL_ID | (cmd & 0xFF));
    spi_send_data_9bit(out);
}
static __inline void send_ctrl_cmd_8bit_internal(unsigned int cmd)
{
    unsigned int out = (CTRL_ID | (cmd & 0xFF));
    spi_send_data_9bit_internal(out);
}

static __inline void send_data_cmd_8bit(unsigned int data)
{
    unsigned int out = (DATA_ID  | (data & 0xFF));
    spi_send_data_9bit(out);
}

static __inline void set_lcm_register_8bit(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd_8bit(regIndex);
    send_data_cmd_8bit(regData);
}


static __inline unsigned char  spi_read_reg(unsigned int reg,unsigned int count,unsigned int buf[])
{
    SET_LSCE_LOW;
	UDELAY(10);
	
	/*send reg addr*/
	send_ctrl_cmd_8bit_internal(reg);

	UDELAY(1);
	SET_LSDA_INPUT;  

	/*dumy clock*/
#ifdef  RISING_TRIGGER
	UDELAY(1);
	SET_LSCK_LOW;       
	UDELAY(5);
    SET_LSCK_HIGH;    
	UDELAY(5);
#else
	UDELAY(1);
    SET_LSCK_HIGH;    
	UDELAY(5);
	SET_LSCK_LOW;       
	UDELAY(5);
#endif

	/*start read*/
	spi_get_bytes_internal(count,buf);
	UDELAY(5);

	/*end read*/
    SET_LSCE_HIGH;
	UDELAY(1);
	SET_LSDA_OUTPUT;
	UDELAY(5);
} 


static  __inline void backlight_ctrl(unsigned char cmd)
{

    if(cmd)
    {
        //set backlight level
        send_ctrl_cmd_8bit(0x51);send_data_cmd_8bit(cmd);//0x00~0xff
        //set backlight on
        send_ctrl_cmd_8bit(0x53);send_data_cmd_8bit(0x2c);
        //set CABC on
        send_ctrl_cmd_8bit(0x55);send_data_cmd_8bit(0x02);//still image     
    }
    else
     {
         //set CABC off
        send_ctrl_cmd_8bit(0x55);send_data_cmd_8bit(0x00); 
        //set backlight off
        send_ctrl_cmd_8bit(0x53);send_data_cmd_8bit(0x00);
        //set backlight level
        send_ctrl_cmd_8bit(0x51);send_data_cmd_8bit(0x00);//0x00~0xff       
    }
}

static void init_lcm_registers_8bit(void)
{

#ifdef BUILD_UBOOT
    printf("punk,UBOOT init_lcm_registers_8bit\r\n");
#else
    printk("punk,kennel init_lcm_registers_8bit\r\n");
#endif
send_ctrl_cmd_8bit(0xF3);send_data_cmd_8bit(0xAA);
send_ctrl_cmd_8bit(0x00);send_data_cmd_8bit(0x01);

send_ctrl_cmd_8bit(0X2D);send_data_cmd_8bit(0X84);
send_ctrl_cmd_8bit(0XC3);send_data_cmd_8bit(0X4B);      //res_sel  
send_ctrl_cmd_8bit(0XC4);send_data_cmd_8bit(0X50);      //osc adjustment
send_ctrl_cmd_8bit(0X15);send_data_cmd_8bit(0X20);      //CKH level in the blanking region

MDELAY(200);

//CMD2 PAGE0
send_ctrl_cmd_8bit(0x00);send_data_cmd_8bit(0x00);

//power setting
send_ctrl_cmd_8bit(0X01);send_data_cmd_8bit(0X64);     
send_ctrl_cmd_8bit(0X02);send_data_cmd_8bit(0X22);     //AVDDP1 & AVDDP2 voltage adjust
send_ctrl_cmd_8bit(0X03);send_data_cmd_8bit(0X44);     //AVDDP1 & AVDDP2 ratio adjust
send_ctrl_cmd_8bit(0X04);send_data_cmd_8bit(0X2B);     //Frame rate setting
send_ctrl_cmd_8bit(0X05);send_data_cmd_8bit(0X2B);     //Frame rate setting
send_ctrl_cmd_8bit(0X06);send_data_cmd_8bit(0X2B);     //Frame rate setting

send_ctrl_cmd_8bit(0X08);send_data_cmd_8bit(0X33);     //AVDDP1 & AVDDP2 pump clock frequence   

 
send_ctrl_cmd_8bit(0X09);send_data_cmd_8bit(0X12);     //VGH & VGL & VCL pump clock frequence 


send_ctrl_cmd_8bit(0X0A);send_data_cmd_8bit(0X33); 
send_ctrl_cmd_8bit(0X0C);send_data_cmd_8bit(0X12);     //VGH & VGL & VCL pump clock frequence 


send_ctrl_cmd_8bit(0X0F);send_data_cmd_8bit(0X12);     //VGH & VGL & VCL pump clock frequence 



send_ctrl_cmd_8bit(0X12);send_data_cmd_8bit(0X07);     //ckh prech on/off=> 07h/00h
send_ctrl_cmd_8bit(0X13);send_data_cmd_8bit(0X11);
send_ctrl_cmd_8bit(0X14);send_data_cmd_8bit(0X40);     //VR voltage set
send_ctrl_cmd_8bit(0X15);send_data_cmd_8bit(0X4A);     //GVDDP voltage set
send_ctrl_cmd_8bit(0X16);send_data_cmd_8bit(0X6E);     //GVDDN voltage set
send_ctrl_cmd_8bit(0X1C);send_data_cmd_8bit(0XFD);
send_ctrl_cmd_8bit(0X1D);send_data_cmd_8bit(0X01);
send_ctrl_cmd_8bit(0X1E);send_data_cmd_8bit(0X0B);

send_ctrl_cmd_8bit(0XA3);send_data_cmd_8bit(0XFA);     
send_ctrl_cmd_8bit(0XA4);send_data_cmd_8bit(0X3C);
send_ctrl_cmd_8bit(0XA5);send_data_cmd_8bit(0XC4);
send_ctrl_cmd_8bit(0XA6);send_data_cmd_8bit(0X05);

send_ctrl_cmd_8bit(0XD3);send_data_cmd_8bit(0XFA);
send_ctrl_cmd_8bit(0XD4);send_data_cmd_8bit(0X3C);
send_ctrl_cmd_8bit(0XD5);send_data_cmd_8bit(0XC4);

send_ctrl_cmd_8bit(0XE7);send_data_cmd_8bit(0X15);     // 1/2/4/8 dot inversion => 


send_ctrl_cmd_8bit(0XFB);send_data_cmd_8bit(0X3F);     // VGH clamp voltage adjust

//GAMMA R Setting            
send_ctrl_cmd_8bit(0X24);send_data_cmd_8bit(0X00);
send_ctrl_cmd_8bit(0X25);send_data_cmd_8bit(0X11);
send_ctrl_cmd_8bit(0X26);send_data_cmd_8bit(0X2C);
send_ctrl_cmd_8bit(0X27);send_data_cmd_8bit(0X44);
send_ctrl_cmd_8bit(0X28);send_data_cmd_8bit(0X1B);
send_ctrl_cmd_8bit(0X29);send_data_cmd_8bit(0X30);
send_ctrl_cmd_8bit(0X2A);send_data_cmd_8bit(0X5F);
send_ctrl_cmd_8bit(0X2B);send_data_cmd_8bit(0X66);
send_ctrl_cmd_8bit(0X2D);send_data_cmd_8bit(0X1E);
send_ctrl_cmd_8bit(0X2F);send_data_cmd_8bit(0X24);
send_ctrl_cmd_8bit(0X30);send_data_cmd_8bit(0XB4);
send_ctrl_cmd_8bit(0X31);send_data_cmd_8bit(0X20);
send_ctrl_cmd_8bit(0X32);send_data_cmd_8bit(0X57);
send_ctrl_cmd_8bit(0X33);send_data_cmd_8bit(0X65);
send_ctrl_cmd_8bit(0X34);send_data_cmd_8bit(0XC3);
send_ctrl_cmd_8bit(0X35);send_data_cmd_8bit(0XD4);
send_ctrl_cmd_8bit(0X36);send_data_cmd_8bit(0XF0);
send_ctrl_cmd_8bit(0X37);send_data_cmd_8bit(0X7F);
                             
send_ctrl_cmd_8bit(0X38);send_data_cmd_8bit(0X00);           
send_ctrl_cmd_8bit(0X39);send_data_cmd_8bit(0X11);          
send_ctrl_cmd_8bit(0X3A);send_data_cmd_8bit(0X2C);          
send_ctrl_cmd_8bit(0X3B);send_data_cmd_8bit(0X44);            
send_ctrl_cmd_8bit(0X3D);send_data_cmd_8bit(0X1B);           
send_ctrl_cmd_8bit(0X3F);send_data_cmd_8bit(0X30);            
send_ctrl_cmd_8bit(0X40);send_data_cmd_8bit(0X5F);            
send_ctrl_cmd_8bit(0X41);send_data_cmd_8bit(0X66);            
send_ctrl_cmd_8bit(0X42);send_data_cmd_8bit(0X1E);            
send_ctrl_cmd_8bit(0X43);send_data_cmd_8bit(0X24);            
send_ctrl_cmd_8bit(0X44);send_data_cmd_8bit(0XB4);            
send_ctrl_cmd_8bit(0X45);send_data_cmd_8bit(0X20);            
send_ctrl_cmd_8bit(0X46);send_data_cmd_8bit(0X57);            
send_ctrl_cmd_8bit(0X47);send_data_cmd_8bit(0X65);            
send_ctrl_cmd_8bit(0X48);send_data_cmd_8bit(0XC3);            
send_ctrl_cmd_8bit(0X49);send_data_cmd_8bit(0XD4);            
send_ctrl_cmd_8bit(0X4A);send_data_cmd_8bit(0XF0);            
send_ctrl_cmd_8bit(0X4B);send_data_cmd_8bit(0X7F);            
                             
//GAMMA G Setting            
send_ctrl_cmd_8bit(0X4C);send_data_cmd_8bit(0X00);            
send_ctrl_cmd_8bit(0X4D);send_data_cmd_8bit(0X11);            
send_ctrl_cmd_8bit(0X4E);send_data_cmd_8bit(0X2C);            
send_ctrl_cmd_8bit(0X4F);send_data_cmd_8bit(0X44);            
send_ctrl_cmd_8bit(0X50);send_data_cmd_8bit(0X1B);            
send_ctrl_cmd_8bit(0X51);send_data_cmd_8bit(0X30);            
send_ctrl_cmd_8bit(0X52);send_data_cmd_8bit(0X5F);            
send_ctrl_cmd_8bit(0X53);send_data_cmd_8bit(0X66);            
send_ctrl_cmd_8bit(0X54);send_data_cmd_8bit(0X1E);            
send_ctrl_cmd_8bit(0X55);send_data_cmd_8bit(0X24);            
send_ctrl_cmd_8bit(0X56);send_data_cmd_8bit(0XB4);            
send_ctrl_cmd_8bit(0X57);send_data_cmd_8bit(0X20);           
send_ctrl_cmd_8bit(0X58);send_data_cmd_8bit(0X57);           
send_ctrl_cmd_8bit(0X59);send_data_cmd_8bit(0X65);            
send_ctrl_cmd_8bit(0X5A);send_data_cmd_8bit(0XC3);            
send_ctrl_cmd_8bit(0X5B);send_data_cmd_8bit(0XD4);            
send_ctrl_cmd_8bit(0X5C);send_data_cmd_8bit(0XF0);            
send_ctrl_cmd_8bit(0X5D);send_data_cmd_8bit(0X7F);            
                             
send_ctrl_cmd_8bit(0X5E);send_data_cmd_8bit(0X00);            
send_ctrl_cmd_8bit(0X5F);send_data_cmd_8bit(0X11);            
send_ctrl_cmd_8bit(0X60);send_data_cmd_8bit(0X2C);            
send_ctrl_cmd_8bit(0X61);send_data_cmd_8bit(0X44);            
send_ctrl_cmd_8bit(0X62);send_data_cmd_8bit(0X1B);            
send_ctrl_cmd_8bit(0X63);send_data_cmd_8bit(0X30);            
send_ctrl_cmd_8bit(0X64);send_data_cmd_8bit(0X5F);            
send_ctrl_cmd_8bit(0X65);send_data_cmd_8bit(0X66);            
send_ctrl_cmd_8bit(0X66);send_data_cmd_8bit(0X1E);            
send_ctrl_cmd_8bit(0X67);send_data_cmd_8bit(0X24);            
send_ctrl_cmd_8bit(0X68);send_data_cmd_8bit(0XB4);            
send_ctrl_cmd_8bit(0X69);send_data_cmd_8bit(0X20);           
send_ctrl_cmd_8bit(0X6A);send_data_cmd_8bit(0X57);            
send_ctrl_cmd_8bit(0X6B);send_data_cmd_8bit(0X65);            
send_ctrl_cmd_8bit(0X6C);send_data_cmd_8bit(0XC3);            
send_ctrl_cmd_8bit(0X6D);send_data_cmd_8bit(0XD4);            
send_ctrl_cmd_8bit(0X6E);send_data_cmd_8bit(0XF0);            
send_ctrl_cmd_8bit(0X6F);send_data_cmd_8bit(0X7F);            
                             
//GAMMA B Setting            
send_ctrl_cmd_8bit(0X70);send_data_cmd_8bit(0X00);            
send_ctrl_cmd_8bit(0X71);send_data_cmd_8bit(0X11);            
send_ctrl_cmd_8bit(0X72);send_data_cmd_8bit(0X2C);            
send_ctrl_cmd_8bit(0X73);send_data_cmd_8bit(0X44);            
send_ctrl_cmd_8bit(0X74);send_data_cmd_8bit(0X1B);            
send_ctrl_cmd_8bit(0X75);send_data_cmd_8bit(0X30);            
send_ctrl_cmd_8bit(0X76);send_data_cmd_8bit(0X5F);            
send_ctrl_cmd_8bit(0X77);send_data_cmd_8bit(0X66);            
send_ctrl_cmd_8bit(0X78);send_data_cmd_8bit(0X1E);            
send_ctrl_cmd_8bit(0X79);send_data_cmd_8bit(0X24);            
send_ctrl_cmd_8bit(0X7A);send_data_cmd_8bit(0XB4);            
send_ctrl_cmd_8bit(0X7B);send_data_cmd_8bit(0X20);            
send_ctrl_cmd_8bit(0X7C);send_data_cmd_8bit(0X57);            
send_ctrl_cmd_8bit(0X7D);send_data_cmd_8bit(0X65);            
send_ctrl_cmd_8bit(0X7E);send_data_cmd_8bit(0XC3);            
send_ctrl_cmd_8bit(0X7F);send_data_cmd_8bit(0XD4);            
send_ctrl_cmd_8bit(0X80);send_data_cmd_8bit(0XF0);           
send_ctrl_cmd_8bit(0X81);send_data_cmd_8bit(0X7F);            
                             
send_ctrl_cmd_8bit(0X82);send_data_cmd_8bit(0X00);            
send_ctrl_cmd_8bit(0X83);send_data_cmd_8bit(0X11);            
send_ctrl_cmd_8bit(0X84);send_data_cmd_8bit(0X2C);            
send_ctrl_cmd_8bit(0X85);send_data_cmd_8bit(0X44);            
send_ctrl_cmd_8bit(0X86);send_data_cmd_8bit(0X1B);            
send_ctrl_cmd_8bit(0X87);send_data_cmd_8bit(0X30);            
send_ctrl_cmd_8bit(0X88);send_data_cmd_8bit(0X5F);            
send_ctrl_cmd_8bit(0X89);send_data_cmd_8bit(0X66);            
send_ctrl_cmd_8bit(0X8A);send_data_cmd_8bit(0X1E);            
send_ctrl_cmd_8bit(0X8B);send_data_cmd_8bit(0X24);            
send_ctrl_cmd_8bit(0X8C);send_data_cmd_8bit(0XB4);            
send_ctrl_cmd_8bit(0X8D);send_data_cmd_8bit(0X20);            
send_ctrl_cmd_8bit(0X8E);send_data_cmd_8bit(0X57);            
send_ctrl_cmd_8bit(0X8F);send_data_cmd_8bit(0X65);            
send_ctrl_cmd_8bit(0X90);send_data_cmd_8bit(0XC3);            
send_ctrl_cmd_8bit(0X91);send_data_cmd_8bit(0XD4);            
send_ctrl_cmd_8bit(0X92);send_data_cmd_8bit(0XF0);            
send_ctrl_cmd_8bit(0X93);send_data_cmd_8bit(0X7F);            

//=== Timing ===
send_ctrl_cmd_8bit(0X94);send_data_cmd_8bit(0X1C); 
send_ctrl_cmd_8bit(0X95);send_data_cmd_8bit(0X1C); 
send_ctrl_cmd_8bit(0X96);send_data_cmd_8bit(0X03); 
send_ctrl_cmd_8bit(0X97);send_data_cmd_8bit(0X01); 
send_ctrl_cmd_8bit(0X98);send_data_cmd_8bit(0X2A); 
send_ctrl_cmd_8bit(0X99);send_data_cmd_8bit(0X25); 
send_ctrl_cmd_8bit(0X9A);send_data_cmd_8bit(0X49); 
send_ctrl_cmd_8bit(0X9B);send_data_cmd_8bit(0X03); 
send_ctrl_cmd_8bit(0X9C);send_data_cmd_8bit(0X17); 
send_ctrl_cmd_8bit(0X9D);send_data_cmd_8bit(0X1C); 
send_ctrl_cmd_8bit(0X9E);send_data_cmd_8bit(0X9B); 
send_ctrl_cmd_8bit(0X9F);send_data_cmd_8bit(0X00); 
send_ctrl_cmd_8bit(0XA0);send_data_cmd_8bit(0X02); 


//CMD1
send_ctrl_cmd_8bit(0XFF);send_data_cmd_8bit(0XAA);
send_ctrl_cmd_8bit(0X11);
MDELAY(300);
send_ctrl_cmd_8bit(0XF3);send_data_cmd_8bit(0XAA);
send_ctrl_cmd_8bit(0X00);send_data_cmd_8bit(0X01);
send_ctrl_cmd_8bit(0X2D);send_data_cmd_8bit(0X84);
send_ctrl_cmd_8bit(0X7F);send_data_cmd_8bit(0XAA);
send_ctrl_cmd_8bit(0X29);
}


//punk, end


typedef enum
{
  USER_CMD_SET=0x00,
  MANUFACTURE_CMD_SET_PAGE0=0x80,
  MANUFACTURE_CMD_SET_PAGE1=0xC0,
}ILI7800_CMD_TYPE;

static __inline void send_ctrl_cmd(ILI7800_CMD_TYPE type,unsigned int cmd)
{
    unsigned char high_8bit = cmd&0x00FF;
	unsigned char low_8bit = type;
    spi_send_data(0x2000|high_8bit);
	spi_send_data(0x0000|low_8bit);
}

static __inline void send_data_cmd(unsigned int data)
{
	unsigned char low_8bit = data&0x00FF;
    spi_send_data(0x4000|low_8bit);
}
static __inline void set_lcm_register(ILI7800_CMD_TYPE type,unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(type,regIndex);
    send_data_cmd(regData);
}

//static  __inline void backlight_ctrl(unsigned char cmd)
//{
//    return;
//}

static void init_lcm_registers(void)
{
/**********************************************
module factory:huashi
resolution:480*800(WVGA)
customer:xinya
**********************************************/
//IM[4-0]=10100b
  set_lcm_register(USER_CMD_SET,0xF3,0xAA);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x00,0x01);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE1,0x2D,0x84);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE1,0xC4,0x50);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE1,0x15,0x20);
  MDELAY(200);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE1,0x00,0x00);

  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x01,0x64);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x02,0x22);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x03,0x44);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x04,0x2B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x05,0x2B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x06,0x2B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x08,0x33);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x09,0x12);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x0A,0x33);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x0C,0x12);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x0F,0x12);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x12,0x07);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x13,0x11);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x14,0x40);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x15,0x4A);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x16,0x6E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x1C,0xFD);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x1D,0x01);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x1E,0x0B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xA3,0xFA);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xA4,0x3C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xA5,0xC4);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xA6,0x05);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xD3,0xFA);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xD4,0x3C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xD5,0xC4);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xE7,0x05);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xFB,0x3F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x24,0x00);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x25,0x10);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x26,0x32);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x27,0x45);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x28,0x1B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x29,0x2E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x2A,0x5E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x2B,0x5C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x2D,0x20);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x2F,0x25);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x30,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x31,0x1A);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x32,0x49);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x33,0x5F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x34,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x35,0xB8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x36,0xC8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x37,0x7F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x38,0x00);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x39,0x10);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x3A,0x32);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x3B,0x45);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x3D,0x1B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x3F,0x2E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x40,0x5E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x41,0x5C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x42,0x20);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x43,0x25);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x44,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x45,0x1A);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x46,0x49);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x47,0x5F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x48,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x49,0xB8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x4A,0xC8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x4B,0x7F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x4C,0x00);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x4D,0x10);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x4E,0x32);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x4F,0x45);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x50,0x1B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x51,0x2E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x52,0x5E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x53,0x5C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x54,0x20);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x55,0x25);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x56,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x57,0x1A);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x58,0x49);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x59,0x5F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x5A,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x5B,0xB8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x5C,0xC8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x5D,0x7F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x5E,0x00);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x5F,0x10);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x60,0x32);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x61,0x45);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x62,0x1B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x63,0x2E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x64,0x5E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x65,0x5C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x66,0x20);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x67,0x25);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x68,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x69,0x1A);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x6A,0x49);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x6B,0x5F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x6C,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x6D,0xB8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x6E,0xC8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x6F,0x7F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x70,0x00);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x71,0x10);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x72,0x32);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x73,0x45);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x74,0x1B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x75,0x2E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x76,0x5E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x77,0x5C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x78,0x20);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x79,0x25);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x7A,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x7B,0x1A);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x7C,0x49);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x7D,0x5F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x7E,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x7F,0xB8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x80,0xC8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x81,0x7F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x82,0x00);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x83,0x10);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x84,0x32);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x85,0x45);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x86,0x1B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x87,0x2E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x88,0x5E);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x89,0x5C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x8A,0x20);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x8B,0x25);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x8C,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x8D,0x1A);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x8E,0x49);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x8F,0x5F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x90,0xA3);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x91,0xB8);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x92,0xCA);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x93,0x7F);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x94,0x1C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x95,0x1C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x96,0x03);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x97,0x01);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x98,0x2A);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x99,0x25);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x9A,0x49);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x9B,0x03);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x9C,0x17);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x9D,0x1C);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x9E,0x9B);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0x9F,0x00);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xA0,0x02);
  set_lcm_register(MANUFACTURE_CMD_SET_PAGE0,0xFF,0xAA);
  send_ctrl_cmd(USER_CMD_SET,0x11);
  MDELAY(120);
  send_ctrl_cmd(USER_CMD_SET,0x29); 
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
#ifdef BUILD_UBOOT
    printf("punk,UBOOT lcm_set_util_funcs\r\n");
#else
    printk("punk,kennel lcm_get_params\r\n");
#endif
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
#ifdef BUILD_UBOOT
    printf("punk,UBOOT lcm_get_params\r\n");
#else
    printk("punk,kennel lcm_get_params\r\n");
#endif
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

    params->dpi.hsync_pulse_width = 6;
    params->dpi.hsync_back_porch  = 27;
    params->dpi.hsync_front_porch = 27;
    params->dpi.vsync_pulse_width = 6;
    params->dpi.vsync_back_porch  = 27;
    params->dpi.vsync_front_porch = 27;
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;
}

static void lcm_init(void)
{
#ifdef BUILD_UBOOT
    printf("punk,UBOOT lcm_init\r\n");
#else
    printk("punk,kennel lcm_init\r\n");
#endif
    config_gpio();
    
    SET_RESET_PIN(1);
    MDELAY(20);
    SET_RESET_PIN(0);
    MDELAY(100);
    SET_RESET_PIN(1);
    MDELAY(150);
#ifdef ILI7800_SPI_8BIT
    init_lcm_registers_8bit();
#else
    init_lcm_registers();
	xx
#endif
    //backlight
}


#ifdef ILI7800_SPI_8BIT

static void lcm_suspend_8bit(void)
{

	send_ctrl_cmd_8bit(0x28);
	send_ctrl_cmd_8bit(0x10);
	MDELAY(120);
}

#else
xx
static void lcm_suspend(void)
{
	send_ctrl_cmd_8bit(USER_CMD_SET,0x28);
	send_ctrl_cmd_8bit(USER_CMD_SET,0x10);
	MDELAY(120);
}
#endif

 
#ifdef ILI7800_SPI_8BIT

static void lcm_resume_8bit(void)
{

   send_ctrl_cmd_8bit(0x11);
   MDELAY(120);
   send_ctrl_cmd_8bit(0x29);
}

#else
xx
static void lcm_resume(void)
{
   send_ctrl_cmd(USER_CMD_SET,0x11);
   MDELAY(120);
   send_ctrl_cmd(USER_CMD_SET,0x29);
   //backlight_ctrl(1);
}
#endif


static void lcm_setbacklight(unsigned int level)
{

    backlight_ctrl(level);
}


static unsigned int lcm_compare_id(void)
{	
	unsigned int id[3];
	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(50);
	spi_read_reg(0x04,3,id);
#ifdef BUILD_UBOOT
    printf("punk,uboot lcm_compare_id, id[0]=%x,id[1]=%x,id[2]=%x\r\n",id[0],id[1],id[2]);
#else
    printk("punk,kernel lcm_compare_id, id[0]=%x,id[1]=%x,id[2]=%x\r\n",id[0],id[1],id[2]);
#endif
    //return (LCM_ID == read_data_cmd())?1:0;
	return 1;
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER yt50f23c0_wvga_ili7800_lcm_drv = 
{
    .name			= "ili7800",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
#ifdef ILI7800_SPI_8BIT
        .suspend        = lcm_suspend_8bit,
        .resume         = lcm_resume_8bit,
#else
		xx
        .suspend        = lcm_suspend,
        .resume         = lcm_resume,
#endif

	.set_backlight = lcm_setbacklight,
	.compare_id     = lcm_compare_id,
};



