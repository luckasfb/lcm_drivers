

#include <linux/string.h>

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (240)
#define FRAME_HEIGHT (320)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define LCM_ID       (0x1526)
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
	send_ctrl_cmd(0x00B0);//Manufacturer Command Access Protect
	send_data_cmd(0x003F);
	send_data_cmd(0x003F);
	MDELAY(100);

	send_ctrl_cmd(0x00FE);//?
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0021);
	send_data_cmd(0x00B4);

	send_ctrl_cmd(0x00B3);//Frame Memory Access and Interface Setting
	send_data_cmd(0x0000);
	send_data_cmd(0x0010);

	send_ctrl_cmd(0x00E0);//NVM Access Control
	send_data_cmd(0x0000);//NVAE: NVM access enable register. NVM access is enabled when NVAE=1.
	send_data_cmd(0x0040);//FTT: NVM control bit.
	MDELAY(50);

	send_ctrl_cmd(0x00B3);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);

	send_ctrl_cmd(0x00FE);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0021);
	send_data_cmd(0x0030);

	send_ctrl_cmd(0x00B0);
	send_data_cmd(0x003F);
	send_data_cmd(0x003F);

	send_ctrl_cmd(0x00B3);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);

	send_ctrl_cmd(0x00B4);
	send_data_cmd(0x0000);

	send_ctrl_cmd(0x00C0);
	send_data_cmd(0x0013);//GIP REV  SM GS BGR SS
	send_data_cmd(0x004F);
	send_data_cmd(0x0000);
	send_data_cmd(0x0010);
	send_data_cmd(0x00A0);//BLV=0 LINE
	send_data_cmd(0x0000);
	send_data_cmd(0x0001);
	send_data_cmd(0x0000);
	//Frame frequency (fFRM)={fosc/(Clock per line×division ratio×(NL + FP + BP))} [Hz]
	send_ctrl_cmd(0x00C1);
	send_data_cmd(0x0001);//BC0   Frame inversion(0)/Line inversion(1)
	send_data_cmd(0x0002);//DIV0[1:0]  800kHz
	send_data_cmd(0x0020);//RTN0[5:0]   clocks    21
	send_data_cmd(0x0008);//BP0[7:0]   4 lines
	send_data_cmd(0x0008);//FP0[7:0]   4 lines
	MDELAY(50);

	send_ctrl_cmd(0x00C3);  //PRTIAL MODE
	send_data_cmd(0x0001);//BC2
	send_data_cmd(0x0000);//DIV2[1:0]
	send_data_cmd(0x0021);//RTN2[5:0]
	send_data_cmd(0x0008);//BP2[7:0]
	send_data_cmd(0x0008);//FP2[7:0]
	MDELAY(50);

	send_ctrl_cmd(0x00C4);
	send_data_cmd(0x0011);
	send_data_cmd(0x0001);
	send_data_cmd(0x0033);
	send_data_cmd(0x0003);

	send_ctrl_cmd(0x00C8);
	send_data_cmd(0x0003);
	send_data_cmd(0x000a);
	send_data_cmd(0x0006);
	send_data_cmd(0x000e);
	send_data_cmd(0x000e);
	send_data_cmd(0x0007);
	send_data_cmd(0x0005);
	send_data_cmd(0x0005);
	send_data_cmd(0x0021);
	send_data_cmd(0x0004);
	send_data_cmd(0x0000);
	send_data_cmd(0x0020);
	send_data_cmd(0x0006);
	send_data_cmd(0x0006);
	send_data_cmd(0x0009);
	send_data_cmd(0x0010);
	send_data_cmd(0x0011);
	send_data_cmd(0x0009);
	send_data_cmd(0x000e);
	send_data_cmd(0x0005);
	send_data_cmd(0x0006);
	send_data_cmd(0x0001);


	send_ctrl_cmd(0x00C9);
	//send_ctrl_cmd(0x00C8);
	send_data_cmd(0x0003);
	send_data_cmd(0x000a);
	send_data_cmd(0x0006);
	send_data_cmd(0x000e);
	send_data_cmd(0x000e);
	send_data_cmd(0x0007);
	send_data_cmd(0x0005);
	send_data_cmd(0x0005);
	send_data_cmd(0x0021);
	send_data_cmd(0x0004);
	send_data_cmd(0x0000);
	send_data_cmd(0x0020);
	send_data_cmd(0x0006);
	send_data_cmd(0x0006);
	send_data_cmd(0x0009);
	send_data_cmd(0x0010);
	send_data_cmd(0x0011);
	send_data_cmd(0x0009);
	send_data_cmd(0x000e);
	send_data_cmd(0x0005);
	send_data_cmd(0x0006);
	send_data_cmd(0x0001);

	send_ctrl_cmd(0x00CA);
	//send_ctrl_cmd(0x00C8);
	send_data_cmd(0x0003);
	send_data_cmd(0x000a);
	send_data_cmd(0x0006);
	send_data_cmd(0x000e);
	send_data_cmd(0x000e);
	send_data_cmd(0x0007);
	send_data_cmd(0x0005);
	send_data_cmd(0x0005);
	send_data_cmd(0x0021);
	send_data_cmd(0x0004);
	send_data_cmd(0x0000);
	send_data_cmd(0x0020);
	send_data_cmd(0x0006);
	send_data_cmd(0x0006);
	send_data_cmd(0x0009);
	send_data_cmd(0x0010);
	send_data_cmd(0x0011);
	send_data_cmd(0x0009);
	send_data_cmd(0x000e);
	send_data_cmd(0x0005);
	send_data_cmd(0x0006);
	send_data_cmd(0x0001);



	send_ctrl_cmd(0x00D0);
	send_data_cmd(0x0033);
	send_data_cmd(0x0053);
	send_data_cmd(0x0081);
	send_data_cmd(0x0031);

	send_ctrl_cmd(0x00D1);
	send_data_cmd(0x002f);//40
	send_data_cmd(0x0057);
	//send_data_cmd(0x0010);

	send_ctrl_cmd(0x00D2);
	send_data_cmd(0x0003);
	send_data_cmd(0x0024);

	send_ctrl_cmd(0x00D4);
	send_data_cmd(0x0003);
	send_data_cmd(0x0024);

	send_ctrl_cmd(0x00E2);
	send_data_cmd(0x003F);

	send_ctrl_cmd(0x0035);//set_tear_on
	send_data_cmd(0x0000);

	send_ctrl_cmd(0x0036);
	send_data_cmd(0x0000);

	send_ctrl_cmd(0x003A);//set_pixel_format
	send_data_cmd(0x0066);//66

	send_ctrl_cmd(0x002A);//set_column_address
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x00EF);

	send_ctrl_cmd(0x002B);//set_page_address:
	send_data_cmd(0x0000);
	send_data_cmd(0x0000);
	send_data_cmd(0x0001);
	send_data_cmd(0x003F);

	send_ctrl_cmd(0x002C);//write_memory_start:
	send_ctrl_cmd(0x0011);//exit_sleep_mode
	MDELAY(100);
	send_ctrl_cmd(0x0029);//set_display_on
	MDELAY(200);
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
    params->dbi.parallel.write_setup    = 4;
    params->dbi.parallel.write_hold     = 4;
    params->dbi.parallel.write_wait     = 12;
    params->dbi.parallel.read_setup     = 4;
    params->dbi.parallel.read_latency   = 40;
    params->dbi.parallel.wait_period    = 1;
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

	send_ctrl_cmd(0xBF);

	read_data_cmd();	//0x01

	read_data_cmd();  // 0x01

	read_data_cmd();	//0x022

	unsigned int param1 = 0;
	param1 =read_data_cmd(); //0x15

	unsigned int param2 =0;
	param2 = read_data_cmd(); //0x26

    	id = ((param1 & LCM_ID_MASK)<<8)|(param2);

    	return (LCM_ID == id)?1:0;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER r61526_lcm_drv =
{
    	.name			= "r61526",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.compare_id     = lcm_compare_id
};

#ifndef BUILD_UBOOT

#ifdef JRD_LCD_ESD_RECOVERY

#include <linux/kernel.h>

#define A_NORMAL			0x2f
#define B_NORMAL			0x57
#define C_NORMAL			0x10

extern void jrd_lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
extern void jrd_lcd_waitfornotbusy(void);

static int get_power_setting_status(void)
{
	static unsigned int temp0,temp1,temp2;
	jrd_lcd_waitfornotbusy();

         send_ctrl_cmd(0xD1);
	read_data_cmd();  //dummy read
	temp0 =  read_data_cmd();	// 0x2f
	temp1 =  read_data_cmd();	// 0x57
	temp2 =  read_data_cmd();	// 0x10

	if( (temp0 != A_NORMAL) || (temp1 != B_NORMAL) || (temp2 != C_NORMAL) )
	{
		printk("---------------------------lcm_init ---------------------------\n");
		lcm_init();
		jrd_lcm_update(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
	}

	return 0;

}

JRD_LCM_ESD r61526_lcm_esd =
{
	.name = "r61526",
	.get_register_value = get_power_setting_status,
};

#endif

#endif

