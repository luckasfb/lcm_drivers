#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#else
#include <linux/string.h>
#if defined(BUILD_UBOOT)
#include <asm/arch/mt6577_gpio.h>
#else
#include <mach/mt6577_gpio.h>
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

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)  (lcm_util.set_gpio_out((n), (v)))
 
#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

//#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)

#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)
#define LSDIN_GPIO_PIN (0xFF)//GPIO72
#define LSSHUT_GPIO_PIN (GPIO_DISP_SHUT_PIN) 

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

static void config_gpio(void)
{
    lcm_util.set_gpio_mode(LSSHUT_GPIO_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_DISP_LSCE_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCK_GPIO_PIN, GPIO_DISP_LSCK_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSDA_GPIO_PIN, GPIO_DISP_LSDA_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSDIN_GPIO_PIN, GPIO_MODE_00);

    lcm_util.set_gpio_dir(LSSHUT_GPIO_PIN, GPIO_DIR_OUT);

    lcm_util.set_gpio_dir(LSCE_GPIO_PIN, GPIO_DIR_OUT);

    lcm_util.set_gpio_dir(LSCK_GPIO_PIN, GPIO_DIR_OUT);

    lcm_util.set_gpio_dir(LSDA_GPIO_PIN, GPIO_DIR_OUT);

    lcm_util.set_gpio_dir(LSDIN_GPIO_PIN, GPIO_DIR_IN);

 
    //lcm_util.set_gpio_pull_enable(LSSHUT_GPIO_PIN, GPIO_PULL_DISABLE);

    lcm_util.set_gpio_pull_enable(LSCE_GPIO_PIN, GPIO_PULL_DISABLE);

    lcm_util.set_gpio_pull_enable(LSCK_GPIO_PIN, GPIO_PULL_DISABLE);

    lcm_util.set_gpio_pull_enable(LSDA_GPIO_PIN, GPIO_PULL_DISABLE);

    lcm_util.set_gpio_pull_enable(LSDIN_GPIO_PIN, GPIO_PULL_ENABLE);

    mt_set_gpio_pull_select(LSDIN_GPIO_PIN, GPIO_PULL_UP);

}

// ---------------------------------------------------------------------------

//  Local Functions

// ---------------------------------------------------------------------------

 

//*******************************************************************

//************** write parameter

//*******************************************************************


#define SET_LSCE_HIGH SET_GPIO_OUT(LSCE_GPIO_PIN, 1)
#define SET_LSCE_LOW SET_GPIO_OUT(LSCE_GPIO_PIN, 0)
#define SET_LSDA_HIGH  SET_GPIO_OUT(LSDA_GPIO_PIN, 1)
#define SET_LSDA_LOW  SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSCK_HIGH  SET_GPIO_OUT(LSCK_GPIO_PIN, 0)
#define SET_LSCK_LOW  SET_GPIO_OUT(LSCK_GPIO_PIN, 1)
#define GET_HX_SDI   mt_get_gpio_in(LSDIN_GPIO_PIN)


#define LCD_DSI_CLCK (0x800E) // 0x8013
#define LCD_DSI_CLCK_UBOOT (0x8012)   //0x800A
#define LCD_PCLCK (23)
#define LCD_PCLCK_UBOOT (44)   //0x39

#define HX_WR_COM (0x70) 
#define HX_WR_REGISTER (0x72) 
#define HX_RD_REGISTER (0x73) 

static __inline void spi_send_data(unsigned int data) 
{ 
	unsigned int i; 

	SET_LSCE_HIGH; 
	SET_LSCK_HIGH; 
	SET_LSDA_HIGH; 
	UDELAY(10); 

	SET_LSCE_LOW; 
	UDELAY(10); 
	for (i = 0; i < 24; ++i) 
	{ 
		if (data & (1 << 23))
		{ 
			SET_LSDA_HIGH; 
		}
		else 
		{ 
			SET_LSDA_LOW; 
		} 
		data <<= 1; 
		UDELAY(10); 
		SET_LSCK_LOW; 
		UDELAY(10); 
		SET_LSCK_HIGH; 
		UDELAY(10); 
	} 

	SET_LSDA_HIGH; 
	SET_LSCE_HIGH; 
} 

static __inline void Write_com(unsigned int cmd) 
{ 
	unsigned int out = ((HX_WR_COM<<16) | (cmd & 0xFFFF)); 
	spi_send_data(out); 
} 

static __inline void Write_register(unsigned int data) 
{ 
	unsigned int out = ((HX_WR_REGISTER<<16) |(data & 0xFFFF)); 
	spi_send_data(out); 
} 

static __inline unsigned short Read_register(void) 
{ 
	unsigned char i,j,front_data; 
	unsigned short value = 0; 

	front_data=HX_RD_REGISTER; 

	SET_LSCE_HIGH; 
	SET_LSCK_HIGH; 
	SET_LSDA_HIGH; 
	UDELAY(10); 
	SET_LSCE_LOW; 
	UDELAY(10); 

	for(i=0;i<8;i++) // 8 Data 
	{ 

		if ( front_data& 0x80) 
			SET_LSDA_HIGH; 
		else 
			SET_LSDA_LOW; 
		front_data<<= 1; 
		UDELAY(10); 
		SET_LSCK_LOW; 
		UDELAY(10); 
		SET_LSCK_HIGH; 
		UDELAY(10); 
	} 
	MDELAY(1); 

	for(j=0;j<16;j++) // 16 Data 
	{ 

		SET_LSCK_HIGH; 
		UDELAY(10); 
		SET_LSCK_LOW; 
		value<<= 1; 
		value |= GET_HX_SDI; 

		UDELAY(10); 
	} 

	SET_LSCE_HIGH; 
	return value; 

} 

#if 1
//*******************************************************************
void SSD2825_Gen_write_1A_1P(unsigned short x1, unsigned short x2)
{
   Write_com(0x00BC); 
   Write_register(0x0002);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
}

void SSD2825_Gen_write_1A_2P(unsigned short x1, unsigned short x2, unsigned short x3)
{
   Write_com(0x00BC); 
   Write_register(0x0003);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register(x3);
}

void SSD2825_Gen_write_1A_3P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4)
{
   Write_com(0x00BC); 
   Write_register(0x0004);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
}
void SSD2825_Gen_write_1A_4P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5)
{
   Write_com(0x00BC); 
   Write_register(0x0005);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register(x5);
}
void SSD2825_Gen_write_1A_6P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5,
	                         unsigned short x6, unsigned short x7)
{
   Write_com(0x00BC); 
   Write_register(0x0007);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register(x7);
}

void SSD2825_Gen_write_1A_9P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5, 
	                         unsigned short x6, unsigned short x7, unsigned short x8, unsigned short x9, unsigned short x10)
{
   Write_com(0x00BC); 
   Write_register(0x000A);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register((x8<<8)|x7);
   Write_register((x10<<8)|x9);
}   

void SSD2825_Gen_write_1A_10P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5, 
	                         unsigned short x6, unsigned short x7, unsigned short x8, unsigned short x9, unsigned short x10,
	                         unsigned short x11)
{
   Write_com(0x00BC); 
   Write_register(0x000B);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register((x8<<8)|x7);
   Write_register((x10<<8)|x9);
   Write_register(x11);
}   

void SSD2825_Gen_write_1A_11P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5, 
	                         unsigned short x6, unsigned short x7, unsigned short x8, unsigned short x9, unsigned short x10,
	                         unsigned short x11, unsigned short x12 )
{
   Write_com(0x00BC); 
   Write_register(0x000C);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register((x8<<8)|x7);
   Write_register((x10<<8)|x9);
   Write_register((x12<<8)|x11);
}   

void SSD2825_Gen_write_1A_12P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5, 
	                         unsigned short x6, unsigned short x7, unsigned short x8, unsigned short x9, unsigned short x10,
	                         unsigned short x11, unsigned short x12, unsigned short x13 )
{
   Write_com(0x00BC); 
   Write_register(0x000D);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register((x8<<8)|x7);
   Write_register((x10<<8)|x9);
   Write_register((x12<<8)|x11);
   Write_register(x13);
}   

void SSD2825_Gen_write_1A_13P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5, 
	                         unsigned short x6, unsigned short x7, unsigned short x8, unsigned short x9, unsigned short x10,
	                         unsigned short x11, unsigned short x12, unsigned short x13, unsigned short x14 )
{
   Write_com(0x00BC); 
   Write_register(0x000E);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register((x8<<8)|x7);
   Write_register((x10<<8)|x9);
   Write_register((x12<<8)|x11);
   Write_register((x14<<8)|x13);
}   

void SSD2825_Gen_write_1A_14P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5, 
	                         unsigned short x6, unsigned short x7, unsigned short x8, unsigned short x9, unsigned short x10,
	                         unsigned short x11, unsigned short x12, unsigned short x13, unsigned short x14, unsigned short x15 )
{
   Write_com(0x00BC); 
   Write_register(0x000F);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register((x8<<8)|x7);
   Write_register((x10<<8)|x9);
   Write_register((x12<<8)|x11);
   Write_register((x14<<8)|x13);
   Write_register(x15);

}   
void SSD2825_Gen_write_1A_15P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5, 
	                         unsigned short x6, unsigned short x7, unsigned short x8, unsigned short x9, unsigned short x10,
	                         unsigned short x11, unsigned short x12, unsigned short x13, unsigned short x14, unsigned short x15,
	                         unsigned short x16)
{
   Write_com(0x00BC); 
   Write_register(0x0010);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register((x8<<8)|x7);
   Write_register((x10<<8)|x9);
   Write_register((x12<<8)|x11);
   Write_register((x14<<8)|x13);
   Write_register((x16<<8)|x15);

}   

void SSD2825_Gen_write_1A_16P(unsigned short x1, unsigned short x2, unsigned short x3, unsigned short x4, unsigned short x5, 
	                          unsigned short x6, unsigned short x7, unsigned short x8, unsigned short x9, unsigned short x10,
	                          unsigned short x11, unsigned short x12, unsigned short x13, unsigned short x14, unsigned short x15,
	                          unsigned short x16, unsigned short x17)
{
   Write_com(0x00BC); 
   Write_register(0x0011);
   Write_com(0x00BF); 
   Write_register((x2<<8)|x1);
   Write_register((x4<<8)|x3);
   Write_register((x6<<8)|x5);
   Write_register((x8<<8)|x7);
   Write_register((x10<<8)|x9);
   Write_register((x12<<8)|x11);
   Write_register((x14<<8)|x13);
   Write_register((x16<<8)|x15);
   Write_register(x17);
}   
#endif
//*******************************************************************    
static void otm1283a_auo572_ce_init(void) 
{

//SSD2825_Gen_write_1A_3P(0xFF,0x12,0x83,0x01);
    Write_com(0x00B7);
    Write_register(0x0302);
    
    Write_com(0x00BD);
    Write_register(0x0000);

    SSD2825_Gen_write_1A_1P(0x00,0x00);	
    SSD2825_Gen_write_1A_3P(0xff,0x12,0x83,0x01);	//EXTC=1
    SSD2825_Gen_write_1A_1P(0x00,0x80);	            //Orise mode enable
    SSD2825_Gen_write_1A_2P(0xff,0x12,0x83);
    
    //-------------------- panel setting --------------------//
    
    SSD2825_Gen_write_1A_1P(0x00,0x80);             //TCON Setting
    SSD2825_Gen_write_1A_9P(0xc0,0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10);
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //Panel Timing Setting
    SSD2825_Gen_write_1A_6P(0xc0,0x00,0x5c,0x00,0x01,0x00,0x04);
    
    SSD2825_Gen_write_1A_1P(0x00,0xa4);             //source pre. 
    SSD2825_Gen_write_1A_1P(0xc0,0x22);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb3);             //Interval Scan Frame: 0 frame, column inversion
    SSD2825_Gen_write_1A_2P(0xc0,0x00,0x50);
    
    SSD2825_Gen_write_1A_1P(0x00,0x81);             //frame rate:60Hz
    SSD2825_Gen_write_1A_1P(0xc1,0x55);
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //clock delay for data latch 
    SSD2825_Gen_write_1A_1P(0xc4,0x49);
    //-------------------- power setting --------------------//
    
    SSD2825_Gen_write_1A_1P(0x00,0xa0);             //dcdc setting
    SSD2825_Gen_write_1A_14P(0xc4,0x05,0x10,0x06,0x02,0x05,0x15,0x10,0x05,0x10,0x07,0x02,0x05,0x15,0x10);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb0);             //clamp voltage setting
    SSD2825_Gen_write_1A_2P(0xc4,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0x91);             //VGH=15V, VGL=-10V, pump ratio:VGH=6x, VGL=-5x
    SSD2825_Gen_write_1A_2P(0xc5,0x46,0x40);
    
    SSD2825_Gen_write_1A_1P(0x00,0x00);             //GVDD=4.87V, NGVDD=-4.87V
    SSD2825_Gen_write_1A_2P(0xd8,0xbc,0xbc);
    
    SSD2825_Gen_write_1A_1P(0x00,0x00);             //VCOMDC=-0.9
    SSD2825_Gen_write_1A_1P(0xd9,0x44);
    
    SSD2825_Gen_write_1A_1P(0x00,0x81);             //source bias 0.75uA
    SSD2825_Gen_write_1A_1P(0xc4,0x82);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb0);             //VDD_18V=1.6V, LVDSVDD=1.55V
    SSD2825_Gen_write_1A_2P(0xc5,0x04,0xb8);
    
    SSD2825_Gen_write_1A_1P(0x00,0xbb);             //LVD voltage level setting
    SSD2825_Gen_write_1A_1P(0xc5,0x80);
    
    //-------------------- control setting --------------------//
    
    SSD2825_Gen_write_1A_1P(0x00,0x00);             //ID1
    SSD2825_Gen_write_1A_1P(0xd0,0x40);
    
    SSD2825_Gen_write_1A_1P(0x00,0x00);             //ID2, ID3
    SSD2825_Gen_write_1A_2P(0xd1,0x00,0x00);
    
    //-------------------- panel timing state control --------------------//
    
    SSD2825_Gen_write_1A_1P(0x00,0x80);             //panel timing state control
    SSD2825_Gen_write_1A_11P(0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //panel timing state control
    SSD2825_Gen_write_1A_15P(0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xa0);             //panel timing state control
    SSD2825_Gen_write_1A_15P(0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb0);             //panel timing state control
    SSD2825_Gen_write_1A_15P(0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xc0);             //panel timing state control
    SSD2825_Gen_write_1A_15P(0xcb,0x05,0x00,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xd0);             //panel timing state control
    SSD2825_Gen_write_1A_15P(0xcb,0x00,0x00,0x00,0x05,0x00,0x05,0x05,0x05,0x00,0x05,0x05,0x05,0x05,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xe0);             //panel timing state control
    SSD2825_Gen_write_1A_14P(0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x05,0x05);
    
    SSD2825_Gen_write_1A_1P(0x00,0xf0);             //panel timing state control
    SSD2825_Gen_write_1A_11P(0xcb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff);
    
    //-------------------- panel pad mapping control --------------------//
    
    SSD2825_Gen_write_1A_1P(0x00,0x80);             //panel pad mapping control
    SSD2825_Gen_write_1A_15P(0xcc,0x02,0x00,0x0a,0x0e,0x0c,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //panel pad mapping control
    SSD2825_Gen_write_1A_15P(0xcc,0x00,0x00,0x00,0x06,0x00,0x2e,0x2d,0x01,0x00,0x09,0x0d,0x0b,0x0f,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xa0);             //panel pad mapping control
    SSD2825_Gen_write_1A_14P(0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x2e,0x2d);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb0);             //panel pad mapping control
    SSD2825_Gen_write_1A_15P(0xcc,0x05,0x00,0x0f,0x0b,0x0d,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xc0);             //panel pad mapping control
    SSD2825_Gen_write_1A_15P(0xcc,0x00,0x00,0x00,0x01,0x00,0x2d,0x2e,0x06,0x00,0x10,0x0c,0x0e,0x0a,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xd0);             //panel pad mapping control
    SSD2825_Gen_write_1A_14P(0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x2d,0x2e);
    
    //-------------------- panel timing setting --------------------//
    
    SSD2825_Gen_write_1A_1P(0x00,0x80);             //panel VST setting
    SSD2825_Gen_write_1A_12P(0xce,0x87,0x03,0x18,0x86,0x03,0x18,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //panel VEND setting
    SSD2825_Gen_write_1A_14P(0xce,0x34,0xfe,0x18,0x34,0xff,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xa0);             //panel CLKA1/2 setting
    SSD2825_Gen_write_1A_14P(0xce,0x38,0x03,0x05,0x00,0x00,0x18,0x00,0x38,0x02,0x05,0x01,0x00,0x18,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb0);             //panel CLKA3/4 setting
    SSD2825_Gen_write_1A_14P(0xce,0x38,0x01,0x05,0x02,0x00,0x18,0x00,0x38,0x00,0x05,0x03,0x00,0x18,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xc0);             //panel CLKb1/2 setting
    SSD2825_Gen_write_1A_14P(0xce,0x30,0x00,0x05,0x04,0x00,0x18,0x00,0x30,0x01,0x05,0x05,0x00,0x18,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xd0);             //panel CLKb3/4 setting
    SSD2825_Gen_write_1A_14P(0xce,0x30,0x02,0x05,0x06,0x00,0x18,0x00,0x30,0x03,0x05,0x07,0x00,0x18,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0x80);             //panel CLKc1/2 setting
    SSD2825_Gen_write_1A_14P(0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //panel CLKc3/4 setting
    SSD2825_Gen_write_1A_14P(0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xa0);             //panel CLKd1/2 setting
    SSD2825_Gen_write_1A_14P(0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb0);             //panel CLKd3/4 setting
    SSD2825_Gen_write_1A_14P(0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xc0);             //panel ECLK setting
    SSD2825_Gen_write_1A_11P(0xcf,0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x81,0x00,0x03,0x08); //gate pre. ena.
    
    SSD2825_Gen_write_1A_1P(0x00,0xb5);             //TCON_GOA_OUT Setting
    SSD2825_Gen_write_1A_6P(0xc5,0x37,0xf1,0xfd,0x37,0xf1,0xfd);
    //-------------------- for Power IC --------------------//
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //Mode-3
    SSD2825_Gen_write_1A_4P(0xf5,0x02,0x11,0x02,0x11);
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //3xVPNL
    SSD2825_Gen_write_1A_1P(0xc5,0x50);
    
    SSD2825_Gen_write_1A_1P(0x00,0x94);             //2xVPNL
    SSD2825_Gen_write_1A_1P(0xc5,0x66);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb2);             //VGLO1
    SSD2825_Gen_write_1A_2P(0xf5,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb4);             //VGLO1_S
    SSD2825_Gen_write_1A_2P(0xf5,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb6);             //VGLO2
    SSD2825_Gen_write_1A_2P(0xf5,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb8);             //VGLO2_S
    SSD2825_Gen_write_1A_2P(0xf5,0x00,0x00);
    
    SSD2825_Gen_write_1A_1P(0x00,0xb4);             //VGLO1/2 Pull low setting
    SSD2825_Gen_write_1A_1P(0xc5,0xc0);		//d[7] vglo1 d[6] vglo2 => 0: pull vss, 1: pull vgl
    
    SSD2825_Gen_write_1A_1P(0x00,0xb2);             //C31 cap. not remove
    SSD2825_Gen_write_1A_1P(0xc5,0x40);
    
    SSD2825_Gen_write_1A_1P(0x00,0x00);             //Orise mode disable
    SSD2825_Gen_write_1A_3P(0xff,0xff,0xff,0xff);
    
    SSD2825_Gen_write_1A_1P(0x11,0x00);
    MDELAY(200);
    SSD2825_Gen_write_1A_1P(0x36,0x00);
    SSD2825_Gen_write_1A_1P(0x3A,0x77);
    SSD2825_Gen_write_1A_1P(0x29,0x00);
    MDELAY(20);
    SSD2825_Gen_write_1A_1P(0x2C,0x00);

	Write_com(0x00B7);///////display on
	Write_register(0x034b); 
    Write_com(0x00B8);//////sleep out
	Write_register(0x0000); 
	Write_com(0x00BC);
	Write_register(0x0001); 
	Write_com(0x00BF); 
	Write_register(0x0011); 
	MDELAY(200); 

	Write_com(0x00B7);///////display on
	Write_register(0x034b); 
	MDELAY(100); 
	Write_com(0x00B8); 
	Write_register(0x0000); 
	Write_com(0x00BC); 
	Write_register(0x0001); 
	Write_com(0x00BF); 
	Write_register(0x0029); 
	MDELAY(50);
	
	Write_com(0x00B7);///////display on
	Write_register(0x030b); 
		
  //printf(" huxh_ssd2825_video mode\n");
}

//*******************************************************************

 

static void init_lcm_registers(void)

{

    //SSD2825  initial  part

    //Pulse width

    Write_com(0x00B1); 
    Write_register(0x0505);

    //Back Porch
    Write_com(0x00B2); 

    Write_register(0x1327); //0x0414

    //Front Porch

    Write_com(0x00B3); 
    Write_register(0x1018);

    //H size = 720

    Write_com(0x00B4); 
    Write_register(0x02d0);

   //V Sixe =1280
    Write_com(0x00B5); 
    Write_register(0x0500);  

    
   //MIPI 24bit Burst Mode
    Write_com(0x00B6); 
    Write_register(0x0007);//0X002B

    //4-lane
    Write_com(0x00DE); 
    Write_register(0x0003);

    //RGB
    Write_com(0x00D6); 
    Write_register(0x0004);    

    //MIPI Lane Configuration
    Write_com(0x00B9); 
    Write_register(0x0000);
	
	

    Write_com(0x00BA); 
    Write_register(LCD_DSI_CLCK);  

    Write_com(0x00BB); 
    Write_register(0x0002);

    Write_com(0x00B9); 
    Write_register(0x0001);    

    Write_com(0x00B8); 
    Write_register(0x0000);    

    Write_com(0x00B7); 
    Write_register(0x034b);    //0x0302
	
	//Write_com(0x00B9);	
	//Write_register(0x0001);
	

    MDELAY(100);
	
    //OTM1283 initial part
    otm1283a_auo572_ce_init();
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
    params->dpi.mipi_pll_clk_div1 = LCD_PCLCK; //43
    params->dpi.mipi_pll_clk_div2 = 3;
    params->dpi.dpi_clk_div       = 2;
    params->dpi.dpi_clk_duty      = 1;
    params->dpi.hsync_pulse_width = 5;
    params->dpi.hsync_back_porch  = 34;
    params->dpi.hsync_front_porch = 24;
    params->dpi.vsync_pulse_width = 5;
    params->dpi.vsync_back_porch  = 14;
    params->dpi.vsync_front_porch = 16;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.format            = LCM_DPI_FORMAT_RGB666;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;
    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;

    params->dpi.i2x_en = 0;
    params->dpi.i2x_edge = 1;

}

static unsigned int lcm_compare_id(void); 

static void lcm_init(void)
{
	int i;
    config_gpio();

    SET_RESET_PIN(1);
    SET_GPIO_OUT(LSSHUT_GPIO_PIN, 1);
    MDELAY(10);
    SET_GPIO_OUT(LSSHUT_GPIO_PIN, 0);

    MDELAY(200);//100

    SET_RESET_PIN(0);
    MDELAY(100);//30
    SET_RESET_PIN(1);

    MDELAY(300);//120
	
    init_lcm_registers();
}

static void lcm_suspend(void)

{

    Write_com(0x00b7);
    Write_register(0x0309); 
    
    Write_com(0x00b8);
    Write_register(0x0000); 
    
    Write_com(0x00bc);
    Write_register(0x0001); 
    
    Write_com(0x00bf);
    Write_register(0x0028);
    
    MDELAY(10);//MDELAY(20); 
    
    Write_com(0x00bf);
    Write_register(0x0010);
    
    MDELAY(200);//MDELAY(200); 
    
    Write_com(0x00b7);
    Write_register(0x0300); 
    
    Write_com(0x00b7);
    Write_register(0x0304); 
        
    Write_com(0x00b9);
    Write_register(0x0000);
    
    return;
}

static void lcm_resume(void)

{
#if 0
    Write_com(0x00b9);
    Write_register(0x0001);
    
    Write_com(0x00b7);
    Write_register(0x030b);
    
    Write_com(0x00b8);
    Write_register(0x0000);
    Write_com(0x00bc);
    Write_register(0x0001); 
    Write_com(0x00bf);
    Write_register(0x0011);
    MDELAY(200);
    Write_com(0x00bf);
    Write_register(0x0029);
    
    Write_com(0x00b7);
    Write_register(0x0309); 
    
    return;
    
#else
    config_gpio();
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_GPIO_OUT(LSSHUT_GPIO_PIN, 0);
    MDELAY(200);//100
    SET_RESET_PIN(0);
    MDELAY(100);//30
    SET_RESET_PIN(1);
    MDELAY(300);//120
    init_lcm_registers();   
    return;
#endif	
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
   unsigned short id;
    
#ifdef BUILD_LK
    unsigned int    lcm_idpin_value = 0;
    lcm_idpin_value = lcm_check_idpin_adc(ADC_CHECK_COUNT);
     if(lcm_idpin_value < 90)// < 0.9V
     return 1;
     else
     return 0;
#endif
#if 0
     Write_com(0x00b0); 
     id=Read_register();
     return (0x2825 == id)?1:0;
#endif

}

// ---------------------------------------------------------------------------

//  Get LCM Driver Hooks

// --------------------------------------------------------------------------- 

LCM_DRIVER zes_hd_otm1283_ssd2825_lcm_drv =
{
    .name         = "zes_hd_otm1283_ssd2825",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .compare_id = lcm_compare_id,
};

