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


#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#else
#include <linux/string.h>
#ifdef BUILD_UBOOT
#include <asm/arch/mt6575_gpio.h>
#else
#include <mach/mt6575_gpio.h>
#endif
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include "cust_adc.h"
#define ADC_CHECK_COUNT	5
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
//extern int IMM_GetOneChannelValue(int dwChannel);
#endif


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)//read pin sdo
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (480)

#define LCM_ID (0x90)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)  (lcm_util.set_gpio_out((n), (v)))

#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))

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

#define SET_LSDA_INPUT  SET_GPIO_INPUT(LSDA_GPIO_PIN)
#define SET_LSDA_OUTPUT  SET_GPIO_OUTPUT(LSDA_GPIO_PIN)
#define SET_LSA0_INPUT  SET_GPIO_INPUT(LSA0_GPIO_PIN)
#define SET_LSA0_OUTPUT  SET_GPIO_OUTPUT(LSA0_GPIO_PIN)

#define GET_LSDA_BIT mt_get_gpio_in(LSDA_GPIO_PIN)
#define GET_LSA0_BIT mt_get_gpio_in(LSA0_GPIO_PIN)




#define CTRL_ID  (0 << 8)
#define DATA_ID  (1 << 8)

 
static __inline void spi_send_data(unsigned int data)
{
    unsigned int i;

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
    
}


static __inline unsigned char  spi_get_data(unsigned int reg)
{
    unsigned int i;
    int k;
    unsigned char getbyte=0;

    reg =  (CTRL_ID | (reg & 0xFF));
    
    SET_LSCE_HIGH;
    SET_LSCK_LOW;
    SET_LSDA_HIGH;
    SET_LSCE_LOW;

    UDELAY(1);

    for (i = 0; i < 9;  i++)
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
    
    SET_LSA0_INPUT;
    UDELAY(2);    
  //  SET_LSCK_LOW;
   // UDELAY(2);      
    //   SET_LSCK_LOW;
   // UDELAY(2);      
   // SET_LSCK_HIGH;        
   // UDELAY(2);
    
    for (k = 7; k >= 0; k--)
    {
        SET_LSCK_LOW;       
        UDELAY(1);
        if (GET_LSA0_BIT) {
            getbyte |= (1 << k);
        }

       SET_LSCK_HIGH;    
        UDELAY(2);
    }    

   
    SET_LSDA_OUTPUT;
    
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

    return getbyte;
}


static __inline unsigned char spi_get_bytes(unsigned int reg,unsigned char data[])
{
    unsigned int i;
    int k;
    unsigned char getbyte1=0;
    unsigned char getbyte2=0;	
    unsigned char getbyte3=0;	
    unsigned char getbyte4=0;	


    data[0]=0;
    data[1]=0;
    data[2]=0;
    data[3]=0;


    reg =  (CTRL_ID | (reg & 0xFF));
    
    SET_LSCE_HIGH;
    SET_LSCK_LOW;
    SET_LSDA_HIGH;
    UDELAY(1);
    SET_LSCE_LOW;
    UDELAY(1);

    for (i = 0; i < 9; i++ )
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
    
    SET_LSA0_INPUT;
    UDELAY(2);    
	
    //SET_LSCK_LOW;
   // UDELAY(2);      
    //SET_LSCK_HIGH;        
    //UDELAY(2);
   // SET_LSCK_LOW;
    //UDELAY(2);
	
    
    for (k = 7; k >= 0; k--)
    {
    	SET_LSCK_LOW;
	    UDELAY(1);
        if (GET_LSA0_BIT) {
            getbyte1 |= (1 << k);
        }

        SET_LSCK_HIGH;        
        UDELAY(2);

    }    

    for (k = 7; k >= 0; k--)
    {
    	SET_LSCK_LOW;
	    UDELAY(1);
        if (GET_LSA0_BIT) {
            getbyte2 |= (1 << k);
        }

        SET_LSCK_HIGH;        
        UDELAY(2);

    }    

    for (k = 7; k >= 0; k--)
    {
    	SET_LSCK_LOW;
	    UDELAY(1);
        if (GET_LSA0_BIT) {
            getbyte3 |= (1 << k);
        }

        SET_LSCK_HIGH;        
        UDELAY(2);

    }    

    for (k = 7; k >= 0; k--)
    {
    	SET_LSCK_LOW;
	    UDELAY(1);
        if (GET_LSA0_BIT) {
            getbyte4 |= (1 << k);
        }

        SET_LSCK_HIGH;        
        UDELAY(2);

    }      
    
    SET_LSDA_OUTPUT;
    
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

    data[0]=getbyte1;
    data[1]=getbyte2;	
    data[2]=getbyte3;	
    data[3]=getbyte4;

    return getbyte1;
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

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}



static __inline unsigned char get_lcm_register_byte(unsigned int regIndex)
{
    unsigned char tmp;
    tmp = spi_get_data(regIndex);
    return tmp;
}


static void init_lcm_registers(void)
{
#ifdef BUILD_LK
	printf("init_lcm_registers -----jmo4861 \r\n");
#else
	printk("init_lcm_registers-----jmo4861  \r\n");
#endif
    //LK@Z1
	send_ctrl_cmd(0xB9); //EXTC
	send_data_cmd(0xFF); //EXTC
	send_data_cmd(0x83); //EXTC
	send_data_cmd(0x57); //EXTC
	MDELAY(15);

	send_ctrl_cmd(0xB6); //
	send_data_cmd(0x25); //6f VCOMDC//56

	send_ctrl_cmd(0x11); // SLPOUT
	MDELAY(130);
	//send_ctrl_cmd(0x35); // TE ON //LK@
        

/*
        send_ctrl_cmd(0xe9); 
	send_data_cmd(0x20); 
*/
        /*
	send_ctrl_cmd(0xB0);
	send_data_cmd(0x68); //80Hz
	send_data_cmd(0x01); 
        */
	send_ctrl_cmd(0xCC); // Set Panel
	send_data_cmd(0x09); //09

	send_ctrl_cmd(0xB1); //
	send_data_cmd(0x00); //
	send_data_cmd(0x15); //BT
	send_data_cmd(0x1e); //1c VSPR//LK@0x3B
	send_data_cmd(0x1e); //1c VSNR//LK@0x3B
	send_data_cmd(0x83); //83 AP  83
	send_data_cmd(0xAA); //FS

	send_ctrl_cmd(0xB4); //
	send_data_cmd(0x02); //NW
	send_data_cmd(0x40); //RTN
	send_data_cmd(0x00); //DIV
	send_data_cmd(0x2A); //DUM
	send_data_cmd(0x2A); //DUM
	send_data_cmd(0x0d); //0D GDON
	send_data_cmd(0x4f); //GDOFF
/*
	send_ctrl_cmd(0xB5); 
	send_data_cmd(0x01); 
	send_data_cmd(0x01); 
	send_data_cmd(0x67); 
*/
	send_ctrl_cmd(0xB3);		//COLOR FORMAT
	send_data_cmd(0xC3);		//SDO_EN,BYPASS,EPF[1:0],0,0,RM,DM//43->C3,SDO_EN=1
	send_data_cmd(0x08);//0x00		//DPL,HSPL,VSPL,EPL
	send_data_cmd(0x06);		//RCM, HPL[5:0]
	send_data_cmd(0x06);		//VPL[5:0]
/*
	send_ctrl_cmd(0xE3); 
	send_data_cmd(0x37); 
	send_data_cmd(0x27);
*/
	send_ctrl_cmd(0xC0); //STBA
	send_data_cmd(0x24); //OPON////LK@0x50
	send_data_cmd(0x24); //OPON///LK@0x50
	send_data_cmd(0x01); //
	send_data_cmd(0x3C); //
	send_data_cmd(0x1E); //
	send_data_cmd(0x08); //GEN

	send_ctrl_cmd(0xE0); //
	send_data_cmd(0x02); //02 1//
	send_data_cmd(0x08); //06 2//
	send_data_cmd(0x11); //3
	send_data_cmd(0x23); //4
	send_data_cmd(0x2C); //5
	send_data_cmd(0x40); //6
	send_data_cmd(0x4A); //7
	send_data_cmd(0x52); //8
	send_data_cmd(0x48); //9
	send_data_cmd(0x41); //10
	send_data_cmd(0x3C); //11
	send_data_cmd(0x33); //12
	send_data_cmd(0x2E); //13
	send_data_cmd(0x28); //14
	send_data_cmd(0x27); //15
	send_data_cmd(0x1B); //16

	send_data_cmd(0x02); //02 17 v1
	send_data_cmd(0x08); //06 18
	send_data_cmd(0x11); //19
	send_data_cmd(0x23); //20
	send_data_cmd(0x2C); //21
	send_data_cmd(0x40); //22
	send_data_cmd(0x4A); //23
	send_data_cmd(0x52); //24
	send_data_cmd(0x48); //25
	send_data_cmd(0x41); //26
	send_data_cmd(0x3C); //27
	send_data_cmd(0x33); //28
	send_data_cmd(0x2E); //29
	send_data_cmd(0x28); //30
	send_data_cmd(0x27); //28 31
	send_data_cmd(0x1B); //03 32
	send_data_cmd(0x00); //33
	send_data_cmd(0x01); //34

    send_ctrl_cmd(0x3A); 
	send_data_cmd(0x66);  //LK@ 0x77:24bit;0x66:18bit;0x55:16bit

        send_ctrl_cmd(0x36); 
	send_data_cmd(0x00);  

	send_ctrl_cmd(0x29); // Display On
	MDELAY(25);
	//send_ctrl_cmd(0x2C); //memary write


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

static void lcm_init(void)
{
#ifdef BUILD_LK
	printf("UBOOT lcm_init\r\n");
#else
	printk("kennel lcm_init\r\n");
#endif

	config_gpio();
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(120);

	init_lcm_registers();

}
static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DPI;
	params->ctrl   = LCM_CTRL_GPIO;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	/* 
        Pixel Clock Frequency = 26MHz * mipi_pll_clk_div1
                                      / (mipi_pll_clk_ref + 1) 
                                      / (2 * mipi_pll_clk_div2)
                                      / dpi_clk_div
    */
    //12.57M pclk, HX8357 require pclk > 9M
	params->dpi.mipi_pll_clk_ref  = 0;//0~1
	params->dpi.mipi_pll_clk_div1 = 29;//0~63
	params->dpi.mipi_pll_clk_div2 = 15;//0~15
	params->dpi.dpi_clk_div       = 2;// 2~32	
	params->dpi.dpi_clk_duty      = 1;//(dpi_clk_div-1)~31

	params->dpi.clk_pol           = LCM_POLARITY_RISING;	
	params->dpi.de_pol            = LCM_POLARITY_RISING;	
	params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
	params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    //12.57/(320+20+20+20)/(480+20+20+20)=61hz
	params->dpi.hsync_pulse_width = 20;
	params->dpi.hsync_back_porch  = 20;	
	params->dpi.hsync_front_porch = 20;		
	params->dpi.vsync_pulse_width = 20;
	params->dpi.vsync_back_porch  = 20;
	params->dpi.vsync_front_porch = 20;

	params->dpi.format            = LCM_DPI_FORMAT_RGB666;
	params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
	params->dpi.is_serial_output  = 0;

	params->dpi.intermediat_buffer_num = 2;

	params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;

}


#ifdef BUILD_LK
static unsigned int lcm_check_idpin_adc(unsigned char count)
{
    int idpin_auxadc = 0;
    //int idpin_auxadc_check = 0;
    int i = 0;
    int realcount = 0;
    int ret = 0, data[4] = {0,0,0,0};

    for(i = 0;i < count;i++)
    {
        ret = IMM_GetOneChannelValue(AUXADC_LCM_IDPIN_CHANNEL,data,NULL);
        if(ret < 0)
        { 
	     printf("[lcm_check_idpin_adc]: get data error\n");
			   
        }
        else
        {
            idpin_auxadc += (data[0]*100+data[1]); 
            realcount ++;
            printf("LK+++:LCM Check ID pin ADC value[%d]=%d,realcount=%d\n",i,data[0]*100+data[1],realcount);
        }

    }
    idpin_auxadc = (idpin_auxadc / realcount);
    printf("LK+++:LCM Check ID pin ADC average value=%d\n",idpin_auxadc);
    return idpin_auxadc;
}
#endif

static unsigned int lcm_compare_id(void)
{

    unsigned int    id = 0;

    //unsigned char data[4];
    
#ifdef BUILD_LK
    unsigned int    lcm_idpin_value = 0;
    lcm_idpin_value = lcm_check_idpin_adc(ADC_CHECK_COUNT);
#endif
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(120);

    send_ctrl_cmd(0xB9); //EXTC
    send_data_cmd(0xFF); //EXTC
    send_data_cmd(0x83); //EXTC
    send_data_cmd(0x57); //EXTC
    MDELAY(15);

    send_ctrl_cmd(0xB3);
    send_data_cmd(0xC3);	
    //for himax8357C
    send_ctrl_cmd(0xFE);
    send_data_cmd(0xD0);
    MDELAY(20);
    id=spi_get_data(0xff);
    //id=spi_get_bytes(0xff,data);
    
#ifdef BUILD_LK
    printf("jmo4861_lcd_id=0x%x \r\n",id);
    //printf("jmo4861_lcd_id data=0x%x,0x%x,0x%x,0x%x \r\n",data[0],data[1],data[2],data[3]);
#else
    printk("jmo4861_lcd_id=0x%x\r\n",id);
#endif

#ifdef BUILD_LK
    if(/*(LCM_ID == id) || */( lcm_idpin_value > 140) && ( lcm_idpin_value <190))//id pin voltage is about 1.5v
#else       
    if(LCM_ID == id)
#endif
    {
        return 1;
    }
    else
    {
        return 0;
    }
    //return (LCM_ID == id)?1:0;

}

static void lcm_suspend(void)
{
    send_ctrl_cmd(0x28);
    MDELAY(25);
    send_ctrl_cmd(0x10);
    MDELAY(150);
}


static void lcm_resume(void)
{
    send_ctrl_cmd(0x11);
    MDELAY(150);
    send_ctrl_cmd(0x29);
    //send_ctrl_cmd(0x2c);
    MDELAY(25);

} 


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER jmo4861_hvga_hx8357_lcm_drv = 
{
    .name			= "jmo4861_hx8357",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id
};



