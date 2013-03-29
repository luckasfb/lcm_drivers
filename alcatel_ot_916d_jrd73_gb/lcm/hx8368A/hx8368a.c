

#include <linux/string.h>

#include "lcm_drv.h"
#ifndef BUILD_UBOOT
#include <linux/kernel.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (240)
#define LCM_ID       (0x68)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


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

	send_ctrl_cmd(0x11); //Sleep Out
	MDELAY(100);

	send_ctrl_cmd(0xB9); // Set EXTC
	send_data_cmd(0xFF);
	send_data_cmd(0x83);
	send_data_cmd(0x68);
	MDELAY(5);

	send_ctrl_cmd(0xBB); // Set OTP
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x80); //OTP load disable
	MDELAY(5);

	send_ctrl_cmd(0xC2); // For Himax internel use
	send_data_cmd(0x00);
	send_data_cmd(0x30);
	MDELAY(5);

	send_ctrl_cmd(0xC0); // For Himax internel use
	send_data_cmd(0x1B);
	send_data_cmd(0x05);
	send_data_cmd(0x08);
	send_data_cmd(0xEC);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	MDELAY(5);

	send_ctrl_cmd(0xE3); // For Himax internel use
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x40);
	send_data_cmd(0x00);
	MDELAY(5);

	send_ctrl_cmd(0xBD); // For Himax internel use
	send_data_cmd(0x00);
	send_data_cmd(0x20);
	send_data_cmd(0x52);
	MDELAY(5);

	send_ctrl_cmd(0xBF); // For Himax internel use
	send_data_cmd(0x01);
	send_data_cmd(0x04);
	send_data_cmd(0x2C);
	MDELAY(5);

	send_ctrl_cmd(0xB1); // Set Power
	send_data_cmd(0x00);
	send_data_cmd(0x02);
	send_data_cmd(0x1e);   //30
	send_data_cmd(0x04);
	send_data_cmd(0x22);
	send_data_cmd(0x11);
	send_data_cmd(0xD4);
	MDELAY(5);

	send_ctrl_cmd(0xB6); // Set VCOM
	send_data_cmd(0xC1); // VMF//A6 dithering stripe--->ACh is OK!
	send_data_cmd(0x37); // VMH
	send_data_cmd(0x10); // VML
	MDELAY(5);

	send_ctrl_cmd(0xE0); // Set Gamma
	send_data_cmd(0x04);
	send_data_cmd(0x21);
	send_data_cmd(0x20);
	send_data_cmd(0x1B);
	send_data_cmd(0x1E);
	send_data_cmd(0x30);
	send_data_cmd(0x0F);
	send_data_cmd(0x55);
	send_data_cmd(0x04);
	send_data_cmd(0x04);
	send_data_cmd(0x05);
	send_data_cmd(0x0A);
	send_data_cmd(0x0F);
	send_data_cmd(0x0F);
	send_data_cmd(0x21);
	send_data_cmd(0x24);
	send_data_cmd(0x1F);
	send_data_cmd(0x1E);
	send_data_cmd(0x3B);
	send_data_cmd(0x2A);
	send_data_cmd(0x70);
	send_data_cmd(0x08);
	send_data_cmd(0x15);
	send_data_cmd(0x1A);
	send_data_cmd(0x1B);
	send_data_cmd(0x1B);
	send_data_cmd(0xFF);
	MDELAY(5);

	send_ctrl_cmd(0xB0); // Set OSC
	send_data_cmd(0x0b);   //0d
	send_data_cmd(0x01);
	MDELAY(5);

	send_ctrl_cmd(0xcc);
	send_data_cmd(0x04);

	send_ctrl_cmd(0x36); // Set BGR
	send_data_cmd(0x49); //0X08

	send_ctrl_cmd(0x35);
	send_data_cmd(0x00);

	//send_ctrl_cmd(0x44);
	//send_data_cmd(0x00);   //7f
	//send_data_cmd(0x0a);

	send_ctrl_cmd(0x11); //Sleep Out
	MDELAY(150);

	send_ctrl_cmd(0x29); //Display On
	MDELAY(5);

	send_ctrl_cmd(0x2C); //Write data to SRAM
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
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_52M;//52
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_LSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB666;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_16_BITS;
    params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_8MA;
    params->dbi.te_mode= 1;//libin changed 1
    params->dbi.te_edge_polarity= 1;//  1
    /////////////////ILI9481
///////////changed by max
    params->dbi.parallel.write_setup    = 4;
    params->dbi.parallel.write_hold     = 4;
    params->dbi.parallel.write_wait     = 12;
    params->dbi.parallel.read_setup     = 4;
    params->dbi.parallel.read_latency   = 40;
    params->dbi.parallel.wait_period    = 1;//0
}


static void lcm_init(void)
{
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(10);
        SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(100);
        init_lcm_registers();
}


static void lcm_suspend(void)
{
	send_ctrl_cmd(0x28); // display off
	MDELAY(50);// delay for 100ms
	send_ctrl_cmd(0x10); // Enter Sleep mode
	MDELAY(150);
}

static void lcm_resume(void)
{
	  send_ctrl_cmd(0x11);//Exit Sleep
	  MDELAY(100);
	///////////////////////////////////////////
	////////GRAM data write ////////////////////
	//////////////////////////////////////////
	// Display ON Setting//
	  send_ctrl_cmd(0x29);//Display On
	  MDELAY(10);
	  send_ctrl_cmd(0x2C); //Memory Write
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
       send_ctrl_cmd(0xB9);  // SET password
	send_data_cmd(0xFF);
	send_data_cmd(0x83);
	send_data_cmd(0x68);
	send_ctrl_cmd(0xD0);
	read_data_cmd();
	read_data_cmd();
    	return (LCM_ID == read_data_cmd())?1:0;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hx8368a_lcm_drv =
{
    	.name			= "hx8368a",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.compare_id     = lcm_compare_id,
};

#ifndef BUILD_UBOOT

#ifdef JRD_LCD_ESD_RECOVERY

#include <linux/kernel.h>

extern void jrd_lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
extern void jrd_lcd_waitfornotbusy(void);

static int get_power_setting_status(void)
{
	unsigned int id1,id2;
	jrd_lcd_waitfornotbusy();

         send_ctrl_cmd(0x09);
	read_data_cmd();  //dummy read
	id1 =  read_data_cmd();	// 0xa4
	id1 = (id1 << 8)  | read_data_cmd();	// 0x63
	id1 = (id1 << 8)  | read_data_cmd();	// 0x06
	id1 = (id1 << 8)  | read_data_cmd();	// 0x00

	send_ctrl_cmd(0xb6);
	read_data_cmd(); //dummy read
	id2 = read_data_cmd();  //c1
	id2= (id2<< 8)  | read_data_cmd();  //37
	id2= (id2<< 8)  | read_data_cmd();  //10

	if( (id1 != 0xa4630600) || (id2 != 0xc13710))
	{
		printk("---------------------------lcm_init ---------------------------\n");
		lcm_init();
		jrd_lcm_update(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
	}

	return 0;

}

JRD_LCM_ESD hx8368a_lcm_esd =
{
	.name = "hx8368a",
	.get_register_value = get_power_setting_status,
};

#endif

#endif


