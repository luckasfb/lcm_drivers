

#include <linux/string.h>
#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (480)

#define LCM_ID       (0x8357)
#define LCM_ID_MASK       (0xFF)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

//static unsigned int lcm_compare_id(void);


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
    lcm_util.send_cmd(cmd & 0xFF);
}

static __inline void send_data_cmd(unsigned int data)
{
    lcm_util.send_data(data & 0xFF);
}

static __inline unsigned int read_data_cmd(void)
{
    return lcm_util.read_data();
}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}

static void sw_clear_panel(unsigned int color)
{
    unsigned int x0 = 0;
    unsigned int y0 = 0;
    unsigned int x1 = x0 + FRAME_WIDTH - 1;
    unsigned int y1 = y0 + FRAME_HEIGHT - 1;

    unsigned int x, y;

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

	send_ctrl_cmd(0x2C);    // send DDRAM set

    // 18-bit mode (256K color) coding
    for (y = y0; y <= y1; ++ y) {
        for (x = x0; x <= x1; ++ x) {
            lcm_util.send_data(color);
        }
    }
}


static void init_lcm_registers(void)
{
	MDELAY(10);
	send_ctrl_cmd(0X0011); //SLEEP OUT
    MDELAY(120);
    
    send_ctrl_cmd(0X00EE);  //SET EQ
    send_data_cmd(0X0002);
    send_data_cmd(0X0001);
    send_data_cmd(0X0002);
    send_data_cmd(0X0001);

	send_ctrl_cmd(0xED);//Set DIR TIM 
	send_data_cmd(0x00); 
	send_data_cmd(0x00); 
	send_data_cmd(0x9A); 
	send_data_cmd(0x9A); 
	send_data_cmd(0x9B); 
	send_data_cmd(0x9B); 
	send_data_cmd(0x00); 
	send_data_cmd(0x00); 
	send_data_cmd(0x00); 
	send_data_cmd(0x00); 
	send_data_cmd(0xAE); 
	send_data_cmd(0xAE); 
	send_data_cmd(0x01); 
	send_data_cmd(0xA2); 
	send_data_cmd(0x00); 

	send_ctrl_cmd(0xB4);//Set RM, DM 
    send_data_cmd(0x00);

	send_ctrl_cmd(0xC0);  //Set Panel Driving
	send_data_cmd(0x10); //REV SM GS
	send_data_cmd(0x3B); // NL[5:0] 
	send_data_cmd(0x00); //SCN[6:0] 
	send_data_cmd(0x02); //NDL 0 PTS[2:0]
	send_data_cmd(0x11); //PTG ISC[3:0] 

	send_ctrl_cmd(0xC8);//Set Gamma 
	send_data_cmd(0x00);  //KP1,KP0
	send_data_cmd(0x46);  //KP3,KP2
	send_data_cmd(0x12);  //KP5,KP4
	send_data_cmd(0x20);  //RP1,RP0
	send_data_cmd(0x0c);  //VRP0  01
	send_data_cmd(0x00);  //VRP1
	send_data_cmd(0x56);  //KN1,KN0
	send_data_cmd(0x12);  //KN3,KN2
	send_data_cmd(0x67);  //KN5,KN4
	send_data_cmd(0x02);  //RN1,RN0
	send_data_cmd(0x00);  //VRN0
	send_data_cmd(0x0c);  //VRN1  01

	
	send_ctrl_cmd(0xD0);//Set Power 
	send_data_cmd(0x44);//DDVDH :5.28
	send_data_cmd(0x44); // BT VGH:15.84	VGL:-7.92
	send_data_cmd(0x05);//VREG1  4.625V

	
	send_ctrl_cmd(0xD1);//Set VCOM 
	send_data_cmd(0x4b); //VCOMH  3.367 //3B
	send_data_cmd(0x14);//VCOM	5.2725

	send_ctrl_cmd(0xD2); 
	send_data_cmd(0x04); 
	send_data_cmd(0x22); //12

	send_ctrl_cmd(0xD3); 
	send_data_cmd(0x04); 
	send_data_cmd(0x12); 

	send_ctrl_cmd(0xD4); 
	send_data_cmd(0x07); 
	send_data_cmd(0x12); 

	send_ctrl_cmd(0xE9); //Set Panel
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC5); //Set Frame rate
	send_data_cmd(0x08);  //61.51Hz


	send_ctrl_cmd(0X0036);
	send_data_cmd(0X000a);

	send_ctrl_cmd(0X003A);
	send_data_cmd(0X0006);

	send_ctrl_cmd(0X002A);
	send_data_cmd(0X0000);
	send_data_cmd(0X0000);
	send_data_cmd(0X0001);
	send_data_cmd(0X003F);

	send_ctrl_cmd(0X002B);
	send_data_cmd(0X0000);
	send_data_cmd(0X0000);
	send_data_cmd(0X0001);
	send_data_cmd(0X00E0);
	MDELAY(120);

	send_ctrl_cmd(0x29); //Display On
	MDELAY(5);
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
	params->io_select_mode = 3;	//?

    params->dbi.port                    = 0;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_104M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_18BITS; // im0 im1:00
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST; 
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB; 
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB666; 
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_18BITS; 
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_32_BITS;
    params->dbi.io_driving_current      = 0; 


    params->dbi.parallel.write_setup    = 2;//t(ast)=10ns
    params->dbi.parallel.write_hold     = 3; //t(wrh)/8-1=30ns
    params->dbi.parallel.write_wait     = 4; //(25+10)/8-1
    params->dbi.parallel.read_setup     = 2;
    params->dbi.parallel.read_latency   = 22; //(10+170)/8-2
    params->dbi.parallel.wait_period    = 0;//t(rdh)
	params->dbi.te_mode= LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity= LCM_POLARITY_FALLING;
}


static void lcm_init(void)
{
    SET_RESET_PIN(0);
    MDELAY(40);
    SET_RESET_PIN(1);
    MDELAY(100);

    init_lcm_registers();
	//Set TE register
	send_ctrl_cmd(0x35);
	send_data_cmd(0x00);


	send_ctrl_cmd(0X0044);  // Set TE signal delay scanline
    send_data_cmd(0X0001);  // Set as 1-th scanline, line lsb first
    send_data_cmd(0X0000);

    sw_clear_panel(0x0);    // Clean panel as black
}


static void lcm_suspend(void)
{
	send_ctrl_cmd(0x0028); 
    send_ctrl_cmd(0x0010); 
    MDELAY(10);
}


static void lcm_resume(void)
{
	send_ctrl_cmd(0x0011);
	MDELAY(120);
	send_ctrl_cmd(0x0029);

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

	send_ctrl_cmd(0x2C);    // send DDRAM set
}


static unsigned int lcm_compare_id(void)
{

	unsigned int id = 0;
	
	send_ctrl_cmd(0xBF);

	
	read_data_cmd(); //dummy code:0
	
	read_data_cmd(); //MIPI:0x1

	read_data_cmd();//MIPI: 0x62

	unsigned int param1 =0;
	param1 = read_data_cmd(); //should 0x83

	unsigned int param2 = 0;
	param2 =read_data_cmd();//should 0x57

	read_data_cmd(); //should 0xff

	id = ((param1&LCM_ID_MASK)<< 8)| (param2&LCM_ID_MASK);

  
    return (LCM_ID == id)?1:0;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER tft3p4039_lcm_drv = 
{
    .name			= "tft3p4039",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.compare_id     = lcm_compare_id
};
