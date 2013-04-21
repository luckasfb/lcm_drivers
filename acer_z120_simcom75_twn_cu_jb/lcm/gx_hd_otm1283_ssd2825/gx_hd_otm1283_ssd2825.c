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

#if 0
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
   Write_register(0x0004);
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
   
    Write_com(0x00BC);
    Write_register(0x0004);
 
    Write_com(0x00bf);
    Write_register(0x12ff);
    Write_register(0x0183);
    //SSD2825_Gen_write_1A_1P(0x00,0x80);
	Write_com(0x00B7);
    Write_register(0x0302);
   
    Write_com(0x00BD);
    Write_register(0x0000);
 
    Write_com(0x00BC);
    Write_register(0x0002);

    Write_com(0x00bf);
    Write_register(0x8000);
    
   // SSD2825_Gen_write_1A_2P(0xFF,0x12,0x83);
   Write_com(0x00B7);
    Write_register(0x0302);
   
    Write_com(0x00BD);
    Write_register(0x0000);
  
    Write_com(0x00BC);
    Write_register(0x0003);
  
    Write_com(0x00bf);
    Write_register(0x12FF); 
    Write_register(0x0083); 
    
    //SSD2825_Gen_write_1A_1P(0x00,0x92);
    
    //SSD2825_Gen_write_1A_2P(0xFF,0x30,0x02);//4 lane
    
   // SSD2825_Gen_write_1A_1P(0x00,0xA6);
		
    Write_com(0x00B7);
    Write_register(0x0302);
    
    Write_com(0x00BD);
    Write_register(0x0000);
 
    Write_com(0x00BC);
    Write_register(0x0002);

    Write_com(0x00bf);
    Write_register(0xA600);    

 
    
    //SSD2825_Gen_write_1A_1P(0xB3,0x0B);

    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0bb3);    
   
  
  //  SSD2825_Gen_write_1A_1P(0x00,0x80);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8000);  

    
 //   SSD2825_Gen_write_1A_9P(0xC0,0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10);
	
    Write_com(0x00BC);		
	Write_register(0x000a);	
	Write_com(0x00BF);		
	Write_register(0x00c0);	
	Write_register(0x0064);
	Write_register(0x1010);	
	Write_register(0x6400);
	Write_register(0x1010);	
   // SSD2825_Gen_write_1A_1P(0x00,0x90);
   	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9000);  
    
   // SSD2825_Gen_write_1A_6P(0xC0,0x00,0x5C,0x00,0x01,0x00,0x04);
   
    Write_com(0x00BC);		
	Write_register(0x0007);	
	Write_com(0x00BF);		
	Write_register(0x00c0);	
	Write_register(0x005c);
	Write_register(0x0001);	/***********0x0001*************/
	Write_register(0x0004); /***********0x0004************/
    
   // SSD2825_Gen_write_1A_1P(0x00,0xB3);
   	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB300);
    
    //SSD2825_Gen_write_1A_2P(0xC0,0x00,0x50);
    Write_com(0x00BC);
    Write_register(0x0003);
	
    Write_com(0x00bf);
    Write_register(0x00C0);
    Write_register(0x0050);   
    
    
    //SSD2825_Gen_write_1A_1P(0x00,0x81);
	
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8100);

   // SSD2825_Gen_write_1A_1P(0xC1,0x66);
   	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x66C1);
   
   // SSD2825_Gen_write_1A_1P(0x00,0xA0);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA000);
 	
    
  //  SSD2825_Gen_write_1A_14P(0xC4,0x05,0x10,0x06,0x02,0x05,0x15,0x12,0x05,0x10,0x07,0x02,0x05,0x15,0x12);
	Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x05C4);
	Write_register(0x0610);
	Write_register(0x0502);
    Write_register(0x1215);
    Write_register(0x1005);
    Write_register(0x0207);
    Write_register(0x1505);
    Write_register(0x0012);  
    
    //SSD2825_Gen_write_1A_1P(0x00,0xB0);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB000);
    
    // SSD2825_Gen_write_1A_2P(0xC4,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0003);
	
    Write_com(0x00bf);
    Write_register(0x00C4);
	Write_register(0x0000);
    
   // SSD2825_Gen_write_1A_1P(0x00,0xBB);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xBB00);
    
   // SSD2825_Gen_write_1A_1P(0xC5,0x80);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x80C5);
    
    //SSD2825_Gen_write_1A_1P(0x00,0x91);
	
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9100);
  
   // SSD2825_Gen_write_1A_2P(0xC5,0x49,0x50);
	Write_com(0x00BC);
    Write_register(0x0003);
	
    Write_com(0x00bf);
    Write_register(0x49C5);
	Write_register(0x0050);
    
    //SSD2825_Gen_write_1A_1P(0x00,0x00);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0000);
  
    
  //  SSD2825_Gen_write_1A_2P(0xD8,0xB6,0xB6);
    Write_com(0x00BC);
    Write_register(0x0003);
	
    Write_com(0x00bf);
    Write_register(0xB6D8);
	Write_register(0x00B6);
    
   // SSD2825_Gen_write_1A_1P(0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0000);
    
  //  SSD2825_Gen_write_1A_1P(0xD9,0x61);
  
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x61D9);
    
  //  SSD2825_Gen_write_1A_1P(0x00,0xB0);
  
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB000);
	
	
    //SSD2825_Gen_write_1A_2P(0xC5,0x04,0x38);
	Write_com(0x00BC);
    Write_register(0x0003);
	
    Write_com(0x00bf);
    Write_register(0x04C5);
	Write_register(0x0038);
	
    
   // SSD2825_Gen_write_1A_1P(0x00,0x82);
   
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8200);
	
   // SSD2825_Gen_write_1A_2P(0xF5,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0003);
	
    Write_com(0x00bf);
    Write_register(0x00F5);
	Write_register(0x0000);
    
  //  SSD2825_Gen_write_1A_1P(0x00,0x82);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8200);
    
   // SSD2825_Gen_write_1A_1P(0xF4,0x00);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x00f4);
    
  //  SSD2825_Gen_write_1A_1P(0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0000);
    
   // SSD2825_Gen_write_1A_1P(0xD0,0x40);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x40D0);
    
   // SSD2825_Gen_write_1A_1P(0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0000);
    
   // SSD2825_Gen_write_1A_2P(0xD1,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0003);
	
    Write_com(0x00bf);
    Write_register(0x00D1);
	Write_register(0x0000);
  /*  
   // SSD2825_Gen_write_1A_1P(0x00,0x9B);
      Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9b00);
    
  //  SSD2825_Gen_write_1A_1P(0xCB,0xFF);
     Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xFFCB);
    
  // SSD2825_Gen_write_1A_1P(0x00,0x9D);
      Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9D00);
    
   // SSD2825_Gen_write_1A_1P(0xCB,0xFF);
      Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xFFCB);
	
    //SSD2825_Gen_write_1A_1P(0x00,0xA0);
        Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA000);
 //   SSD2825_Gen_write_1A_1P(0xCB,0xFF);
	    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xFFCB);
    
  //  SSD2825_Gen_write_1A_1P(0x00,0xA2);
           Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA200);
  //  SSD2825_Gen_write_1A_1P(0xCB,0xFF);
  	    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xFFCB);
    
  //  SSD2825_Gen_write_1A_1P(0x00,0xB3);
    	    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB300);
    //SSD2825_Gen_write_1A_1P(0xCB,0xFF);
		    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xFFCB);
    
    //SSD2825_Gen_write_1A_1P(0x00,0xB5);
		    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB500);
    
   // SSD2825_Gen_write_1A_1P(0xCB,0xFF);
    	    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xFFCB);
  //  SSD2825_Gen_write_1A_1P(0x00,0xB7);
        Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB700);
   // SSD2825_Gen_write_1A_1P(0xCB,0xFF);
        	    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xFFCB);
   // SSD2825_Gen_write_1A_1P(0x00,0xB9);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB900);
  //  SSD2825_Gen_write_1A_1P(0xCB,0xFF);
         	    Write_com(0x00BC);
    Write_register(0x0002);
*/	
    Write_com(0x00bf);
    Write_register(0xFFCB);
   // SSD2825_Gen_write_1A_1P(0x00,0x80);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8000);
  //  SSD2825_Gen_write_1A_1P(0xC4,0x00);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x00C4);
   // SSD2825_Gen_write_1A_1P(0x00,0x98);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9800);
   // SSD2825_Gen_write_1A_1P(0xC5,0x10);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x10C5); 
    //SSD2825_Gen_write_1A_1P(0x00,0x81);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8100);
   // SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
    
   // SSD2825_Gen_write_1A_1P(0x00,0x83);
    Write_com(0x00BC);
    Write_register(0x0002);
	Write_com(0x00bf);
    Write_register(0x8300);
    
    //SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
    //SSD2825_Gen_write_1A_1P(0x00,0x85);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8500);
    
   // SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
    
    //SSD2825_Gen_write_1A_1P(0x00,0x87);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8700);
   // SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
    
    //SSD2825_Gen_write_1A_1P(0x00,0x89);
	
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8900);
   // SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
    
    //SSD2825_Gen_write_1A_1P(0x00,0x8B);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8b00);
   // SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
    
    
   // SSD2825_Gen_write_1A_1P(0x00,0x95);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9500);
    
   // SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
   // SSD2825_Gen_write_1A_1P(0x00,0x97);
          Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9700);
    
  //  SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
    //SSD2825_Gen_write_1A_1P(0x00,0x99);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9900);
		
  //  SSD2825_Gen_write_1A_1P(0xF5,0x15);
  
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
	
   // SSD2825_Gen_write_1A_1P(0x00,0xA1);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA100);
    
   // SSD2825_Gen_write_1A_1P(0xF5,0x15);
            Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
   // SSD2825_Gen_write_1A_1P(0x00,0xA3);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA300);
    
    //SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
   // SSD2825_Gen_write_1A_1P(0x00,0xA5);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA500);
    
   //SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
   // SSD2825_Gen_write_1A_1P(0x00,0xA7);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA700);
    
   // SSD2825_Gen_write_1A_1P(0xF5,0x15);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
	
   // SSD2825_Gen_write_1A_1P(0x00,0xAB);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xAB00);
    
   // SSD2825_Gen_write_1A_1P(0xF5,0x18);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x18F5);
    
  //  SSD2825_Gen_write_1A_1P(0x00,0xB1);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB100);
    
    //SSD2825_Gen_write_1A_13P(0xF5,0x15,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x15,0x08,0x15);
    Write_com(0x00BC);
    Write_register(0x000E);
	
    Write_com(0x00bf);
    Write_register(0x15F5);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x1502);
	Write_register(0x1508);
	
	
   // SSD2825_Gen_write_1A_1P(0x00,0xB4);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB400);
    
  //  SSD2825_Gen_write_1A_1P(0xC5,0xC0);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xC0C5);
	
	 //   SSD2825_Gen_write_1A_1P(0x00,0x90);             //Mode-3 power ic
 

	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9000);
    
  //  SSD2825_Gen_write_1A_4P(0xf5,0x02,0x11,0x02,0x11); 
  

	Write_com(0x00BC);
    Write_register(0x0005);
	
    Write_com(0x00bf);
    Write_register(0x02f5);
	Write_register(0x0211);
    Write_register(0x0011);    
    
//    SSD2825_Gen_write_1A_1P(0x00,0x90);             //3xVPNL 


	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9000);  
	
   // SSD2825_Gen_write_1A_1P(0xc5,0x50);

	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x50c5);
    
  //  SSD2825_Gen_write_1A_1P(0x00,0x94);             //2xVPNL 

	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9400);
    
    //SSD2825_Gen_write_1A_1P(0xc5,0x77); 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x77c5);
    
    /*/---------------------------------------------------------------
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //Mode-3 power ic
    
    SSD2825_Gen_write_1A_4P(0xf5,0x02,0x11,0x02,0x11); 
    
    
    
    SSD2825_Gen_write_1A_1P(0x00,0x90);             //3xVPNL 
    
    SSD2825_Gen_write_1A_1P(0xc5,0x50); 
    
    
    
    SSD2825_Gen_write_1A_1P(0x00,0x94);             //2xVPNL 
    
    SSD2825_Gen_write_1A_1P(0xc5,0x77); 
    
    */
    
    //------------------------------------------------------------------
    
   // SSD2825_Gen_write_1A_1P(0x00,0x80);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8000);
    
   // SSD2825_Gen_write_1A_11P(0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x000C);
	
    Write_com(0x00bf);
    Write_register(0x00CB);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	
    //SSD2825_Gen_write_1A_1P(0x00,0x90);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9000);
   // SSD2825_Gen_write_1A_15P(0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x00CB);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x00ff);
	Write_register(0x00ff);
   // SSD2825_Gen_write_1A_1P(0x00,0xA0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA000);
  //  SSD2825_Gen_write_1A_15P(0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
	Write_register(0xffcb);
	Write_register(0xff00);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
    //SSD2825_Gen_write_1A_1P(0x00,0xB0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB000);
   // SSD2825_Gen_write_1A_15P(0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x00CB);
	Write_register(0x0000);
	Write_register(0xff00);
	Write_register(0xff00);
	Write_register(0xff00);
	Write_register(0xff00);
	Write_register(0x0000);
	Write_register(0x0000);
   // SSD2825_Gen_write_1A_1P(0x00,0xC0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xC000);
   // SSD2825_Gen_write_1A_15P(0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x00,0x05,0x05,0x05,0x05,0x05);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x00CB);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0505);
	Write_register(0x0500);
	Write_register(0x0505);
	Write_register(0x0505); 
   // SSD2825_Gen_write_1A_1P(0x00,0xD0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xD000);
    //SSD2825_Gen_write_1A_15P(0xCB,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x05CB);
	Write_register(0x0505);
	Write_register(0x0505);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0500); 
   // SSD2825_Gen_write_1A_1P(0x00,0xE0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xE000);
  //  SSD2825_Gen_write_1A_15P(0xCB,0x05,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x05CB);
	Write_register(0x0500);
	Write_register(0x0505);
	Write_register(0x0505);
	Write_register(0x0505);
	Write_register(0x0505);
	Write_register(0x0505);
	Write_register(0x0000);  
  //  SSD2825_Gen_write_1A_1P(0x00,0xF0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xf000);
    
  //  SSD2825_Gen_write_1A_11P(0xCB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
    Write_com(0x00BC);
    Write_register(0x000c);
	
    Write_com(0x00bf);
    Write_register(0x0ffcB);/***********0xffcb********/
	Write_register(0xffff);
	Write_register(0xffff);
	Write_register(0xffff);
	Write_register(0xffff);
	Write_register(0xffff);
	
  //  SSD2825_Gen_write_1A_1P(0x00,0x80);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8000);
    
   // SSD2825_Gen_write_1A_15P(0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x07,0x00,0x0D,0x09,0x0F,0x0B,0x11);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x00CC);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0705);
	Write_register(0x0D00);
	Write_register(0x0F09);
	Write_register(0x110B); 
   // SSD2825_Gen_write_1A_1P(0x00,0x90);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9000);
    
   // SSD2825_Gen_write_1A_15P(0xCC,0x15,0x13,0x17,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x15CC);
	Write_register(0x1713);
	Write_register(0x0301);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0600); 
  //  SSD2825_Gen_write_1A_1P(0x00,0xA0);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA000);
    
   // SSD2825_Gen_write_1A_14P(0xCC,0x08,0x00,0x0E,0x0A,0x10,0x0C,0x12,0x16,0x14,0x18,0x02,0x04,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x08CC);
	Write_register(0x0E00);
	Write_register(0x100A);
	Write_register(0x120C);
	Write_register(0x1416);
	Write_register(0x0218);
	Write_register(0x0004);
	Write_register(0x0000); 
 //   SSD2825_Gen_write_1A_1P(0x00,0xB0);
	
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB000);
  //  SSD2825_Gen_write_1A_15P(0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x02,0x00,0x0C,0x10,0x0A,0x0E,0x18);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x00CC);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0204);
	Write_register(0x0C00);
	Write_register(0x0A10);
	Write_register(0x180E); 
  //  SSD2825_Gen_write_1A_1P(0x00,0xC0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xC000);
    
   // SSD2825_Gen_write_1A_15P(0xCC,0x14,0x16,0x12,0x08,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x14CC);
	Write_register(0x1216);
	Write_register(0x0608);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0000);
	Write_register(0x0300); 
   // SSD2825_Gen_write_1A_1P(0x00,0xD0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xD000);
    
    //SSD2825_Gen_write_1A_15P(0xCC,0x01,0x00,0x0B,0x0F,0x09,0x0D,0x17,0x13,0x15,0x11,0x07,0x05,0x00,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0010);
	
    Write_com(0x00bf);
    Write_register(0x01CC);
	Write_register(0x0B00);
	Write_register(0x090F);
	Write_register(0x170D);
	Write_register(0x1513);
	Write_register(0x0711);
	Write_register(0x0005);
	Write_register(0x0000); 
   // SSD2825_Gen_write_1A_1P(0x00,0x80);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8000);
    
   // SSD2825_Gen_write_1A_12P(0xCE,0x87,0x03,0x28,0x86,0x03,0x28,0x85,0x03,0x28,0x84,0x03,0x28);
    Write_com(0x00BC);
    Write_register(0x000D);
	
    Write_com(0x00bf);
    Write_register(0x87CE);
	Write_register(0x2803);
	Write_register(0x0386);
	Write_register(0x8528);
	Write_register(0x2803);
	Write_register(0x0384);
	Write_register(0x0028);
	
    //SSD2825_Gen_write_1A_1P(0x00,0x90);
	Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9000);
    
//    SSD2825_Gen_write_1A_14P(0xCE,0x34,0xFC,0x28,0x34,0xFD,0x28,0x34,0xFE,0x28,0x34,0xFF,0x28,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x34CE);
	Write_register(0x28FC);
	Write_register(0xFD34);
	Write_register(0x3428);
	Write_register(0x28FE);
	Write_register(0x0FF34);
	Write_register(0x0028);
	Write_register(0x0000); 
  //  SSD2825_Gen_write_1A_1P(0x00,0xA0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA000);
    
  //  SSD2825_Gen_write_1A_14P(0xCE,0x38,0x07,0x05,0x00,0x00,0x28,0x00,0x38,0x06,0x05,0x01,0x00,0x28,0x00);
    Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x38CE);
	Write_register(0x0507);
	Write_register(0x0000);
	Write_register(0x0028);
	Write_register(0x0638);
	Write_register(0x0105);
	Write_register(0x2800);
	Write_register(0x0000); 
  //  SSD2825_Gen_write_1A_1P(0x00,0xB0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB000);
  //  SSD2825_Gen_write_1A_14P(0xCE,0x38,0x05,0x05,0x02,0x00,0x28,0x00,0x38,0x04,0x05,0x03,0x00,0x28,0x00);
    Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x38CE);
	Write_register(0x0505);
	Write_register(0x0002);
	Write_register(0x0028);
	Write_register(0x0438);
	Write_register(0x0305);/**********0x0305**********/
	Write_register(0x2800);
	Write_register(0x0000); 
   // SSD2825_Gen_write_1A_1P(0x00,0xC0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xC000);
  //  SSD2825_Gen_write_1A_14P(0xCE,0x38,0x03,0x05,0x04,0x00,0x28,0x00,0x38,0x02,0x05,0x05,0x00,0x28,0x00);
    Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x38CE);
	Write_register(0x0503);
	Write_register(0x0004);
	Write_register(0x0028);
	Write_register(0x0238);
	Write_register(0x00505);
	Write_register(0x2800);
	Write_register(0x0000); 
  //  SSD2825_Gen_write_1A_1P(0x00,0xD0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xD000);
   // SSD2825_Gen_write_1A_14P(0xCE,0x38,0x01,0x05,0x06,0x00,0x28,0x00,0x38,0x00,0x05,0x07,0x00,0x28,0x00);
       Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x38CE);
	Write_register(0x0501);
	Write_register(0x0006);
	Write_register(0x0028);
	Write_register(0x0038);
	Write_register(0x0705);/**********0x0705********/
	Write_register(0x2800);
	Write_register(0x0000); 
 //   SSD2825_Gen_write_1A_1P(0x00,0x80);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8000);
 //   SSD2825_Gen_write_1A_14P(0xCF,0x78,0x07,0x04,0xFF,0x00,0x18,0x10,0x78,0x06,0x05,0x00,0x00,0x18,0x10);
    Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x78CF);
	Write_register(0x0407);
	Write_register(0x00FF);
	Write_register(0x1018); /**********0x1018******************/
	Write_register(0x0678);/**********0x0678******************/
	Write_register(0x0005); /**********0x0005******************/
	Write_register(0x1800); /**********0x1800******************/
	Write_register(0x0010); /**********0x0010******************/
  //  SSD2825_Gen_write_1A_1P(0x00,0x90);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9000);
 //   SSD2825_Gen_write_1A_14P(0xCF,0x78,0x05,0x05,0x01,0x00,0x18,0x10,0x78,0x04,0x05,0x02,0x00,0x18,0x10);
    Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x78CF);
	Write_register(0x0505);
	Write_register(0x0001);
	Write_register(0x1018);
	Write_register(0x0478);
	Write_register(0x0205);
	Write_register(0x1800);
	Write_register(0x0010); 
   // SSD2825_Gen_write_1A_1P(0x00,0xA0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xA000);
  //  SSD2825_Gen_write_1A_14P(0xCF,0x70,0x00,0x05,0x00,0x00,0x18,0x10,0x70,0x01,0x05,0x01,0x00,0x18,0x10);
        Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x70CF);
	Write_register(0x0500);
	Write_register(0x0000);
	Write_register(0x1018);
	Write_register(0x0170);
	Write_register(0x0105);/**********0x0105******************/
	Write_register(0x1800);/**********0x1800******************/
	Write_register(0x0010);/**********0x0010******************/
   // SSD2825_Gen_write_1A_1P(0x00,0xB0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xB000);
    //SSD2825_Gen_write_1A_14P(0xCF,0x70,0x02,0x05,0x02,0x00,0x18,0x10,0x70,0x03,0x05,0x03,0x00,0x18,0x10);
    Write_com(0x00BC);
    Write_register(0x000F);
	
    Write_com(0x00bf);
    Write_register(0x70CF);
	Write_register(0x0502);
	Write_register(0x0002);
	Write_register(0x1018);
	Write_register(0x0370);
	Write_register(0x0305);
	Write_register(0x1800);
	Write_register(0x0010);  
   // SSD2825_Gen_write_1A_1P(0x00,0xC0);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xC000);
   // SSD2825_Gen_write_1A_10P(0xCF,0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x04,0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x000B);
	
    Write_com(0x00bf);
    Write_register(0x01CF);
	Write_register(0x2001);
	Write_register(0x0020);
	Write_register(0x0100);
	Write_register(0x0004);
	Write_register(0x0000);
	
   // SSD2825_Gen_write_1A_1P(0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0000);
   // SSD2825_Gen_write_1A_16P(0xE1,0x00,0x07,0x0C,0x0E,0x06,0x11,0x0D,0x0D,0x01,0x05,0x0F,0x0F,0x17,0x30,0x25,0x06);
    Write_com(0x00BC);
    Write_register(0x0011);
	
    Write_com(0x00bf);
    Write_register(0x00e1);
	Write_register(0x0c07);
	Write_register(0x060e);
	Write_register(0x0d11);
	Write_register(0x010d);
	Write_register(0x0f05);
	Write_register(0x170f);
	Write_register(0x2530);
	Write_register(0x0006);
  //  SSD2825_Gen_write_1A_1P(0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0000);
   // SSD2825_Gen_write_1A_16P(0xE2,0x00,0x06,0x0C,0x0E,0x06,0x11,0x0C,0x0E,0x00,0x05,0x0F,0x0F,0x17,0x30,0x25,0x06);
    Write_com(0x00BC);
    Write_register(0x0011);
	
    Write_com(0x00bf);
    Write_register(0x00e2);
	Write_register(0x0c06);
	Write_register(0x060e);
	Write_register(0x0c11);
	Write_register(0x000e);
	Write_register(0x0f05);
    Write_register(0x170f);
	Write_register(0x2530);
	Write_register(0x0006);

// ***********************CELEVER COLOR SETTING FOR CE ****************************************// 

        //SSD2825_Gen_write_1A_1P(0x00,0xa0);             // 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xa000);
     //   SSD2825_Gen_write_1A_12P(0xd6,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD); 
    Write_com(0x00BC);
    Write_register(0x000d);
	
    Write_com(0x00bf);
    Write_register(0x01d6);
	Write_register(0x01cd);
	Write_register(0x01cd);
	Write_register(0x01cd);
	Write_register(0x01cd);
	Write_register(0x01cd);
    Write_register(0x00cd);
	
       // SSD2825_Gen_write_1A_1P(0x00,0xB0);             // 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xb000);
     //   SSD2825_Gen_write_1A_12P(0xd6,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD); 

    Write_com(0x00BC);
    Write_register(0x000d);
	
    Write_com(0x00bf);
    Write_register(0x01d6);
	Write_register(0x01cd);
	Write_register(0x01cd);
	Write_register(0x01cd);
	Write_register(0x01cd);
	Write_register(0x01cd);
    Write_register(0x00cd);
	
      //  SSD2825_Gen_write_1A_1P(0x00,0xC0);             // 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xc000);
       // SSD2825_Gen_write_1A_12P(0xD6,0x89,0x11,0x89,0x89,0x11,0x89,0x89,0x11,0x89,0x89,0x11,0x89); 
    Write_com(0x00BC);
    Write_register(0x000d);
	
    Write_com(0x00bf);
    Write_register(0x89d6);
	Write_register(0x8911);
	Write_register(0x1189);
	Write_register(0x8989);
	Write_register(0x8911);
	Write_register(0x1189);
    Write_register(0x0089);
	
       // SSD2825_Gen_write_1A_1P(0x00,0xD0);             // 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xd000);
     //   SSD2825_Gen_write_1A_6P(0xD6,0x89,0x11,0x89,0x89,0x11,0x89); 

    Write_com(0x00BC);
    Write_register(0x0007);
	
    Write_com(0x00bf);
    Write_register(0x89d6);
	Write_register(0x8911);
	Write_register(0x1189);
	Write_register(0x0089);


      //  SSD2825_Gen_write_1A_1P(0x00,0xE0);             // 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xe000);
      //  SSD2825_Gen_write_1A_12P(0xD6,0x44,0x11,0x44,0x44,0x11,0x44,0x44,0x11,0x44,0x44,0x11,0x44); 

    Write_com(0x00BC);
    Write_register(0x000d);
	
    Write_com(0x00bf);
    Write_register(0x44d6);
	Write_register(0x4411);
	Write_register(0x1144);
	Write_register(0x4444);
	Write_register(0x4411);
	Write_register(0x1144);
    Write_register(0x0044);

      //  SSD2825_Gen_write_1A_1P(0x00,0xF0);             // 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0xf000);
     //   SSD2825_Gen_write_1A_6P(0xD6,0x44,0x11,0x44,0x44,0x11,0x44); 

    Write_com(0x00BC);
    Write_register(0x0007);
	
    Write_com(0x00bf);
    Write_register(0x44d6);
	Write_register(0x4411);
	Write_register(0x1144);
	Write_register(0x0044);

//**********************************************************************************************// 

    //    SSD2825_Gen_write_1A_1P(0x00,0x90);             //Clever CMD1 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x9000);
     //   SSD2825_Gen_write_1A_1P(0xd6,0x00); 

    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x00d6);

    //    SSD2825_Gen_write_1A_1P(0x00,0x00);             //CE - Low 
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0000);
      //  SSD2825_Gen_write_1A_1P(0x55,0x80); 

    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x8055);
       // SSD2825_Gen_write_1A_1P(0x00,0x00);
    Write_com(0x00BC);
    Write_register(0x0002);
	
    Write_com(0x00bf);
    Write_register(0x0000);
       // SSD2825_Gen_write_1A_3P(0xFF,0xFF,0xFF,0xFF);	
	Write_com(0x00BC);
    Write_register(0x0004);
	
    Write_com(0x00bf);
    Write_register(0xffff);
	Write_register(0xffff);

#if 1//fae 
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
#endif		
/*	Write_com(0x00B7);	

	Write_register(0x0342);

	Write_com(0x00B8);	

	Write_register(0x0000);

    Write_com(0x00Bc);	

	Write_register(0x0000);

 	Write_com(0x0011);

	Write_com(0x0029);
	Write_com(0x00B7);	

	Write_register(0x034b);	//SSL modified

	

	Write_com(0x00B9);	

	Write_register(0x0001);*/


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

    MDELAY(100);

    SET_RESET_PIN(0);
    MDELAY(30);
    SET_RESET_PIN(1);

    MDELAY(120);
	
    init_lcm_registers();
}

static void lcm_suspend(void)

{
    
    SET_GPIO_OUT(LSSHUT_GPIO_PIN, 1);
    
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
      
    Write_com(0x00b9);    
    Write_register(0x0000);
	
    return;

}

static void lcm_resume(void)
{
#if 1
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
    MDELAY(100);
    SET_RESET_PIN(0);
    MDELAY(30);
    SET_RESET_PIN(1);
    MDELAY(120);
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
     return 0;
     else
     return 1;
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

LCM_DRIVER gx_hd_otm1283_ssd2825_lcm_drv =
{
    .name         = "gx_hd_otm1283_ssd2825",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .compare_id = lcm_compare_id,
};

