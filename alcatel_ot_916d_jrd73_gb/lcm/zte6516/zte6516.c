

#include <linux/string.h>

#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (240)
#define LCM_ID       (0x57)
// ---------------------------------------------------------------------------
//  Local Variables:
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define PANEL_CONTROL_DELAY (10)
#define POWER_ON_SEQ_DELAY  (200)


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

static __inline unsigned int LOW_BYTE(unsigned int x)
{
    return (x & 0xFF);
}

static __inline unsigned int HIGH_BYTE(unsigned int x)
{
    return ((x >> 8) & 0xFF);
}


static __inline void send_ctrl_cmd(unsigned int cmd)
{
    lcm_util.send_cmd(cmd);
}

static __inline void send_data_cmd(unsigned int data)
{
    lcm_util.send_data(data);
}

static __inline unsigned int read_data_cmd(void)
{
    return 0x00FF & (lcm_util.read_data() >> 1);
}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd((regIndex & 0x00FF) << 1);
    send_data_cmd((regData & 0x00FF) << 1);
}


static void init_lcm_registers(void)
{


    //=======wenbo add==============
 set_lcm_register(0x00FF, 0x0000);
 set_lcm_register(0x0072, 0x00F6);
 set_lcm_register(0x00FF, 0x0000);
 set_lcm_register(0x00F2, 0x0000);
 set_lcm_register(0x00E4, 0x0000);
 set_lcm_register(0x00E5, 0x001C);
 set_lcm_register(0x00E6, 0x0000);
 set_lcm_register(0x00E7, 0x001C);
 set_lcm_register(0x00EE, 0x0042);
 set_lcm_register(0x00EF, 0x00DB);
 set_lcm_register(0x002E, 0x0098);
 set_lcm_register(0x0040, 0x0000);
 set_lcm_register(0x0041, 0x002E);
 set_lcm_register(0x0042, 0x002D);
 set_lcm_register(0x0043, 0x003F);
 set_lcm_register(0x0044, 0x003F);
 set_lcm_register(0x0045, 0x003F);
 set_lcm_register(0x0046, 0x002C);
 set_lcm_register(0x0047, 0x007F);
 set_lcm_register(0x0048, 0x0007);
 set_lcm_register(0x0049, 0x0005);
 set_lcm_register(0x004A, 0x0008);
 set_lcm_register(0x004B, 0x0013);
 set_lcm_register(0x004C, 0x001E);
 set_lcm_register(0x0050, 0x0000);
 set_lcm_register(0x0051, 0x0000);
 set_lcm_register(0x0052, 0x0000);
 set_lcm_register(0x0053, 0x0012);
 set_lcm_register(0x0054, 0x0011);
 set_lcm_register(0x0055, 0x003F);
 set_lcm_register(0x0056, 0x0000);
 set_lcm_register(0x0057, 0x0053);
 set_lcm_register(0x0058, 0x0001);
 set_lcm_register(0x0059, 0x000C);
 set_lcm_register(0x005A, 0x0017);
 set_lcm_register(0x005B, 0x001A);
 set_lcm_register(0x005C, 0x0018);
 set_lcm_register(0x005D, 0x00C3);
 set_lcm_register(0x0002, 0x0000);
 set_lcm_register(0x0003, 0x0000);
 set_lcm_register(0x0004, 0x0001);
 set_lcm_register(0x0005, 0x003F);
 set_lcm_register(0x0006, 0x0000);
 set_lcm_register(0x0007, 0x0000);
 set_lcm_register(0x0008, 0x0000);
 set_lcm_register(0x0009, 0x00EF);   //
 set_lcm_register(0x0024, 0x0022);
 set_lcm_register(0x0025, 0x0064);
 set_lcm_register(0x0023, 0x0090);
 set_lcm_register(0x001B, 0x000E);
 set_lcm_register(0x001D, 0x0011);
 set_lcm_register(0x0019, 0x0001);
 set_lcm_register(0x001C, 0x0003);
 set_lcm_register(0x0001, 0x0000);
 set_lcm_register(0x001F, 0x0080);
 MDELAY(5); //DELAY 5MS
 set_lcm_register(0x001F, 0x0090);
 MDELAY(5); //DELAY 5MS
 set_lcm_register(0x001F, 0x00D4);
 MDELAY(5); //DELAY 5MS
 set_lcm_register(0x0028, 0x0008);
 MDELAY(40); 
 set_lcm_register(0x0028, 0x0038);
 MDELAY(40);  //DELAY 40MS
 set_lcm_register(0x0028, 0x003C);
 //wr_com(0x0016, 0x0020);  //0020
 set_lcm_register(0x0017, 0x0006); //18bit
 send_ctrl_cmd(0x0022 << 1);
    //===========wenbo add end=========

   
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
    
    params->dbi.port                    = 0;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_52M;        //LCM_DBI_CLOCK_FREQ_52M
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;    
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB666;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_32_BITS;
    params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_8MA;

    params->dbi.parallel.write_setup    = 2;//1;       //0
    params->dbi.parallel.write_hold     = 1;       ////3 33
    params->dbi.parallel.write_wait     = 3;       //3//
    params->dbi.parallel.read_setup     = 0;       ////2
    params->dbi.parallel.read_latency   = 10;      ////19
    params->dbi.parallel.wait_period    = 0;        ////0
}


static void lcm_init(void)
{
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(50);
	
    init_lcm_registers();
}


      static void lcm_suspend(void)
	{
		set_lcm_register(0x00FF, 0x0000);
	
		set_lcm_register(0x0028, 0x0038);
		
		MDELAY(40);
	
		set_lcm_register(0x0028, 0x0004);
	
		set_lcm_register(0x001F, 0x0090);
	
		MDELAY(5);
	
		set_lcm_register(0x001F, 0x0088);
	
		set_lcm_register(0x001C, 0x0000);
	
		set_lcm_register(0x001F, 0x0089);
	
		set_lcm_register(0x0019, 0x0000);
			
	
	/*	  set_lcm_register(0x07, 0x0101);
		MDELAY(10);
		set_lcm_register(0x07, 0x0000);   // display off
		MDELAY(10);
		set_lcm_register(0x10, 0x10B2);
		MDELAY(50);
		*/
	}



static void lcm_resume(void)
	{
	
	set_lcm_register(0x00FF, 0x0000);
	
	set_lcm_register(0x0019, 0x0001);
	
	MDELAY(5);
	
	set_lcm_register(0x001F, 0x0088);
	
	set_lcm_register(0x001C, 0x0003);
	set_lcm_register(0x001F, 0x0080);
	
	MDELAY(5);
	
	set_lcm_register(0x001F, 0x0090);
	
	MDELAY(5);
	
	set_lcm_register(0x001F, 0x00D4);
	
	MDELAY(5);
	
	set_lcm_register(0x0028, 0x0008);
	
	MDELAY(40);
	
	set_lcm_register(0x0028, 0x0038);
	
	MDELAY(40);
	
	set_lcm_register(0x0028, 0x003C);
	
	/*	  set_lcm_register(0x10, 0x10B0);
		MDELAY(25);
		set_lcm_register(0x07, 0x0173);
		MDELAY(175);
		*/
	}



static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
	{
				unsigned int x0 = x;
				unsigned int y0 = y;
				unsigned int x1 = x0 + width - 1;
				unsigned int y1 = y0 + height - 1;
				
				set_lcm_register(0x02, HIGH_BYTE(x0));
				set_lcm_register(0x03, LOW_BYTE(x0));
				set_lcm_register(0x04, HIGH_BYTE(x1));
				set_lcm_register(0x05, LOW_BYTE(x1));
				set_lcm_register(0x06, HIGH_BYTE(y0));
				set_lcm_register(0x07, LOW_BYTE(y0));
				set_lcm_register(0x08, HIGH_BYTE(y1));
				set_lcm_register(0x09, LOW_BYTE(y1));
				
				send_ctrl_cmd(0x0022 << 1);
	}

static unsigned int lcm_compare_id(void)
{
	send_ctrl_cmd(0x00);
    return (LCM_ID == read_data_cmd())?1:0;
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER zte6516_lcm_drv = 
{
    .name			= "zte6516",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.compare_id     = lcm_compare_id
};
