

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

#define LCM_ID       (0x9341)
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

	send_ctrl_cmd(0xCF);
	send_data_cmd(0x00);
	send_data_cmd(0x99);
	send_data_cmd(0x30);

	send_ctrl_cmd(0xED);
	send_data_cmd(0x67);
	send_data_cmd(0x03);
	send_data_cmd(0x12);
	send_data_cmd(0x81);

	send_ctrl_cmd(0xE8);
	send_data_cmd(0x8A);
	send_data_cmd(0x0A);
	send_data_cmd(0x78);

	send_ctrl_cmd(0xCB);
	send_data_cmd(0x39);
	send_data_cmd(0x2C);
	send_data_cmd(0x00);
	send_data_cmd(0x34);
	send_data_cmd(0x02);

	send_ctrl_cmd(0xF7);
	send_data_cmd(0x20);

	send_ctrl_cmd(0xEA);
	send_data_cmd(0x00);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC0);	//Power Control 1 , GVDD
	send_data_cmd(0x26);	//GVDD=4.75V

	send_ctrl_cmd(0xC1);
	send_data_cmd(0x11);        //set DDVDH,VGL,VGH

	send_ctrl_cmd(0xC5);       //VCOM Control 1, VMH,VML
	send_data_cmd(0x2B);
	send_data_cmd(0x2C);

	send_ctrl_cmd(0xC7);	//VCOM Control 2, VMF
	send_data_cmd(0xD4);

	send_ctrl_cmd(0xF6);         //Interface Control
	send_data_cmd(0x01);
	send_data_cmd(0x30);
	send_data_cmd(0x00);         //RGB interface:0x06

              /*
	send_ctrl_cmd(0xB5);        //Blanking Porch Control
	send_data_cmd(0x04);
	send_data_cmd(0x04);
	send_data_cmd(0x02);
	send_data_cmd(0x02);
              */

	send_ctrl_cmd(0xB6);
	send_data_cmd(0x0A);
	send_data_cmd(0xA2);

	send_ctrl_cmd(0x36);         //Memory Access Control
	send_data_cmd(0x08);

	send_ctrl_cmd(0x3A);         //Pixel Format Set
	send_data_cmd(0x66);         //18bit:0x66;16bit:0x55

	send_ctrl_cmd(0xB1);
	send_data_cmd(0x00);
	send_data_cmd(0x19);

	send_ctrl_cmd(0xF2);
	send_data_cmd(0x00);

	send_ctrl_cmd(0x26);
	send_data_cmd(0x01);

	send_ctrl_cmd(0xE0);        //Set Gamma
	send_data_cmd(0x0F); 	//VP63[3:0]
	send_data_cmd(0x22); 	//VP62[5:0]
	send_data_cmd(0x20); 	//VP61[5:0]
	send_data_cmd(0x09); 	//VP59[3:0]
	send_data_cmd(0x0D); 	//VP57[4:0]
	send_data_cmd(0x08); 	//VP50[3:0]
	send_data_cmd(0x4E); 	//VP43[6:0]
	send_data_cmd(0xA7); 	//VP27[3:0];VP36[3:0]
	send_data_cmd(0x3F); 	//VP20[6:0]
	send_data_cmd(0x05); 	//VP13[3:0]
	send_data_cmd(0x0B); 	//VP6[4:0]
	send_data_cmd(0x03); 	//VP4[3:0]
	send_data_cmd(0x20); 	//VP2[5:0]
	send_data_cmd(0x23); 	//VP1[5:0]
	send_data_cmd(0x00); 	//VP0[3:0]

	send_ctrl_cmd(0XE1);        //Set Gamma
	send_data_cmd(0x00); 	//VN63[3:0]
	send_data_cmd(0x1D); 	//VN62[5:0]
	send_data_cmd(0x1F); 	//VN61[5:0]
	send_data_cmd(0x06); 	//VN59[3:0]
	send_data_cmd(0x12); 	//VN57[4:0]
	send_data_cmd(0x07); 	//VN50[3:0]
	send_data_cmd(0x31); 	//VN43[6:0]
	send_data_cmd(0x58); 	//VN27[3:0];VN36[3:0]
	send_data_cmd(0x40); 	//VN20[6:0]
	send_data_cmd(0x0A); 	//VN13[3:0]
	send_data_cmd(0x14); 	//VN6[4:0]
	send_data_cmd(0x0C); 	//VN4[3:0]
	send_data_cmd(0x1F); 	//VN2[5:0]
	send_data_cmd(0x1C); 	//VN1[5:0]
	send_data_cmd(0x0F); 	//VN0[3:0]

	send_ctrl_cmd(0x2A);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0xEF);

	send_ctrl_cmd(0x2B);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x3F);

	send_ctrl_cmd(0x11); 		//Exit Sleep
	 MDELAY(120);

	send_ctrl_cmd(0x29); 		//Display ON
	 MDELAY(10);

	send_ctrl_cmd(0x2C); 		//Memory Write

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

	send_ctrl_cmd(0xD3);

	read_data_cmd();	//dummy code:0

	read_data_cmd();  // 0x00

	unsigned int param1 = 0;
	param1 =read_data_cmd(); //0x93

	unsigned int param2 =0;
	param2 = read_data_cmd(); //0x41

    	id = ((param1 & LCM_ID_MASK)<<8)|(param2);

    	return (LCM_ID == id)?1:0;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9341_lcm_drv =
{
    .name			= "ili9341",
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

#define A_NORMAL			0x84
#define B_NORMAL			0x63
#define C_NORMAL			0x04
#define D_NORMAL			0x00

extern void jrd_lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
extern void jrd_lcd_waitfornotbusy(void);

static int get_power_setting_status(void)
{
	static unsigned int temp0,temp1,temp2,temp3;
	jrd_lcd_waitfornotbusy();

         send_ctrl_cmd(0x09);
	read_data_cmd();  //dummy read
	temp0 =  read_data_cmd();	// 0x84
	temp1 =  read_data_cmd();	// 0x63
	temp2 =  read_data_cmd();	// 0x04
	temp3 =  read_data_cmd();	// 0x00

	if( (temp0 != A_NORMAL) || (temp1 != B_NORMAL) || (temp2 != C_NORMAL) || (temp3 != D_NORMAL))
	{
		printk("---------------------------lcm_init ---------------------------\n");
		lcm_init();
		jrd_lcm_update(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
	}

	return 0;

}

JRD_LCM_ESD ili9341_lcm_esd =
{
	.name = "ili9341",
	.get_register_value = get_power_setting_status,
};

#endif

#endif
