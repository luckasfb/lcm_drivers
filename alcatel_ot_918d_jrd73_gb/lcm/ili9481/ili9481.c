

#include <linux/string.h>

#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (480)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define LCM_ID       (0x5466)
#define LCM_ID_MASK       (0xFF)

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

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
    return lcm_util.read_data();
}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}


static void init_lcm_registers(void)
{
#if 0  //modify by JRD request
	send_ctrl_cmd(0xC0);    
send_data_cmd(0x08); 
send_data_cmd(0x07); 
send_ctrl_cmd(0xC1);    
send_data_cmd(0x04); 
send_data_cmd(0x04); 
send_ctrl_cmd(0xC2);    
send_data_cmd(0x33); 

send_ctrl_cmd(0xB1);    
send_data_cmd(0xB0); 
send_data_cmd(0x11); 
send_ctrl_cmd(0xB4);    
send_data_cmd(0x02); 

send_ctrl_cmd(0x36);    
send_data_cmd(0x48); 

#if 1//just for temple rework lcd

send_ctrl_cmd(0x3A);    
send_data_cmd(0x66); //0x55 ->0x66 when SMT
#endif
send_ctrl_cmd(0xC5);    
send_data_cmd(0x00); 
send_data_cmd(0x37); 
send_data_cmd(0x80); 
 
send_ctrl_cmd(0xE0);    
send_data_cmd(0x0F); 
send_data_cmd(0x1B); 
send_data_cmd(0x18); 
send_data_cmd(0x0B); 
send_data_cmd(0x0E); 
send_data_cmd(0x09); 
send_data_cmd(0x47); 
send_data_cmd(0x94); 
send_data_cmd(0x35); 
send_data_cmd(0x0A); 
send_data_cmd(0x13); 
send_data_cmd(0x05); 
send_data_cmd(0x08); 
send_data_cmd(0x03); 
send_data_cmd(0x00); 
 
send_ctrl_cmd(0XE1);    
send_data_cmd(0x0F); 
send_data_cmd(0x3A); 
send_data_cmd(0x37); 
send_data_cmd(0x0B); 
send_data_cmd(0x0C); 
send_data_cmd(0x05); 
send_data_cmd(0x4A); 
send_data_cmd(0x24); 
send_data_cmd(0x39); 
send_data_cmd(0x07); 
send_data_cmd(0x10); 
send_data_cmd(0x04); 
send_data_cmd(0x27); 
send_data_cmd(0x25); 
send_data_cmd(0x00); 
 
send_ctrl_cmd(0XF8);    
send_data_cmd(0x21); 
send_data_cmd(0x04); 
 
send_ctrl_cmd(0x11); 
send_ctrl_cmd(0X35);    //libin temp change for te
send_data_cmd(0x00); 
MDELAY(120); 
send_ctrl_cmd(0x29); 
 send_ctrl_cmd(0x2c); 
 #endif
	send_ctrl_cmd(0x36);//set address mode
	send_data_cmd(0x48);

	send_ctrl_cmd(0x3A); //pixel format
	send_data_cmd(0x66);//0x55-16bits,0x66-18bits
	//send_data_cmd(0x55);//0x55-16bits,0x66-18bits

	//gamma 2.2 setting 
	send_ctrl_cmd(0xE0);    //Set Gamma 
	send_data_cmd(0x00); 
	send_data_cmd(0x08); 
	send_data_cmd(0x0F); 
	send_data_cmd(0x06); 
	send_data_cmd(0x14); 
	send_data_cmd(0x0A); 
	send_data_cmd(0x3A); 
	send_data_cmd(0x9A); 
	send_data_cmd(0x49); 
	send_data_cmd(0x0A); 
	send_data_cmd(0x07); 
	send_data_cmd(0x0D); 
	send_data_cmd(0x1B); 
	send_data_cmd(0x1E); 
	send_data_cmd(0x0F); 

	send_ctrl_cmd(0xE1);    //Set Gamma 
	send_data_cmd(0x00); 
	send_data_cmd(0x21); 
	send_data_cmd(0x24); 
	send_data_cmd(0x02); 
	send_data_cmd(0x08); 
	send_data_cmd(0x05); 
	send_data_cmd(0x36); 
	send_data_cmd(0x56); 
	send_data_cmd(0x45); 
	send_data_cmd(0x05); 
	send_data_cmd(0x0B); 
	send_data_cmd(0x09); 
	send_data_cmd(0x30); 
	send_data_cmd(0x37); 
	send_data_cmd(0x0F); 


	send_ctrl_cmd(0xf2);  
	send_data_cmd(0x18); 
	send_data_cmd(0xa3); 
	send_data_cmd(0x12); 
	send_data_cmd(0x02); 
	send_data_cmd(0xB2); 
	send_data_cmd(0x52); //0x12-->0x92-->0x52
	send_data_cmd(0xff); 
	send_data_cmd(0x10); 


	send_ctrl_cmd(0xf7);  
	send_data_cmd(0xa9); 
	send_data_cmd(0x91); 
	send_data_cmd(0x2d); 
	send_data_cmd(0x0a); 
	send_data_cmd(0x4f); 

	send_ctrl_cmd(0xf4);  
	send_data_cmd(0x00); 
	send_data_cmd(0x00); 
	send_data_cmd(0x08); 
	send_data_cmd(0x91); 
	send_data_cmd(0x04); 

	send_ctrl_cmd(0xf8);  
	send_data_cmd(0x21); 
	send_data_cmd(0x04); 

	send_ctrl_cmd(0xf9);  
	send_data_cmd(0x00); 
	send_data_cmd(0x08); 

	send_ctrl_cmd(0xB4);    //Set 2Dot Invertion
	send_data_cmd(0x02); 

	send_ctrl_cmd(0xB1);    //Set Frame Rate
	send_data_cmd(0xB0); 
	send_data_cmd(0x11); 

	send_ctrl_cmd(0x20);  	//Display Inversion OFF

	send_ctrl_cmd(0xc0);  
	send_data_cmd(0x0D); 
	send_data_cmd(0x0D); 


	send_ctrl_cmd(0xc1);  
	send_data_cmd(0x44); 

	send_ctrl_cmd(0xB7);  
	send_data_cmd(0xC6); 

	send_ctrl_cmd(0xc5);  
	send_data_cmd(0x00); 
	send_data_cmd(0x3F); 

	send_ctrl_cmd(0x2A); //display area column setting
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x01);    
	send_data_cmd(0x3F);

	send_ctrl_cmd(0x2B); //display area page setting
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x01);    
	send_data_cmd(0xE0);



	send_ctrl_cmd(0x11);//exit sleep mode

	send_ctrl_cmd(0X35);    //libin temp change for te
	send_data_cmd(0x00); 

	MDELAY(150);

	send_ctrl_cmd(0x29); //Display on

	MDELAY(20);

	send_ctrl_cmd(0x2C); //memary write
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

    params->type   = LCM_TYPE_DBI;
    params->ctrl   = LCM_CTRL_PARALLEL_DBI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    params->io_select_mode = 3;

    params->dbi.port                    = 0;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_52M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_LSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB666;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_16_BITS;
    params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_8MA;
    params->dbi.te_mode= 1;//libin changed
    params->dbi.te_edge_polarity= 1;
    params->dbi.parallel.write_setup    = 1;
    params->dbi.parallel.write_hold     = 0;
    params->dbi.parallel.write_wait     = 3;
    params->dbi.parallel.read_setup     = 1;
    params->dbi.parallel.read_latency   = 31;
    params->dbi.parallel.wait_period    = 3;
}


static void lcm_init(void)
{
	    SET_RESET_PIN(1);
	        MDELAY(100);
    SET_RESET_PIN(0);
    MDELAY(200);
    SET_RESET_PIN(1);
    MDELAY(400);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
	send_ctrl_cmd(0x28);
	MDELAY(50);
	send_ctrl_cmd(0x10);
		MDELAY(120);
}


static void lcm_resume(void)
{
		send_ctrl_cmd(0x11);
			MDELAY(150);
	send_ctrl_cmd(0x29);
				MDELAY(50);
	send_ctrl_cmd(0x2c);
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

	send_ctrl_cmd(0x2A);
	send_data_cmd(HIGH_BYTE(x0));
	send_data_cmd(LOW_BYTE(x0));
	send_data_cmd(HIGH_BYTE(x1));
	send_data_cmd(LOW_BYTE(x1));

	send_ctrl_cmd(0x2B);
	send_data_cmd(HIGH_BYTE(y0));
	send_data_cmd(LOW_BYTE(y0));
	send_data_cmd(HIGH_BYTE(y1));
	send_data_cmd(LOW_BYTE(y1));

	// Write To GRAM
	send_ctrl_cmd(0x2C);
}

static unsigned int lcm_compare_id(void)
{

	unsigned int id=0;

	
	send_ctrl_cmd(0x04);
	
		
	read_data_cmd();	//dummy code:0
		
	unsigned int param1 = 0;
	param1 =read_data_cmd(); //manufacturer id
	
	read_data_cmd();
	
	unsigned int param2 =0;
	param2 = read_data_cmd(); //module/driver  id
	
    id = ((param1 & LCM_ID_MASK)<<8)|(param2);
	

    return (LCM_ID == id)?1:0;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9481_lcm_drv = 
{
    .name			= "ili9481",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.compare_id     = lcm_compare_id
};
