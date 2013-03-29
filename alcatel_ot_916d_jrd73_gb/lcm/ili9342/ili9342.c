

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
#define LCM_ID       (0x42)

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

	//VCI=2.8V
	//	MDELAY(50); 	//Wait for Power stabilization
	//----------- LCM Reset before initialized ---------//
	//	LcmReset=1;
	//	MDELAY(1);
	//	LcmReset=0; 		//LCM Hardware Reset.
	//	MDELAY(10);
	//	LcmReset=1;
	//	MDELAY(120);		//delay
	//----------------------------------------------------------//
	//
	  //************* Start Initial Sequence **********//
	  send_ctrl_cmd(0xB9);		 //Set EXTC
	  send_data_cmd(0xFF);
	  send_data_cmd(0x93);
	  send_data_cmd(0x42);

	  send_ctrl_cmd(0xCB);
	  send_data_cmd(0x19);
	  send_data_cmd(0xE4);//A4

	  send_ctrl_cmd(0xB6);		 //Display Function Control
	  send_data_cmd(0x0A);//PTG[1:0],PT[1:0]
	  send_data_cmd(0xE2);//REV,GS,SS,SM,ISC[3:0]
	  send_data_cmd(0x1D);//NL[5:0]

	  send_ctrl_cmd(0x36);		 //Memory Access Control
	  send_data_cmd(0x08);//MY,MX,MV,ML,BGR,MH

	  send_ctrl_cmd(0x3A);		 //Pixel Format Set
	  send_data_cmd(0x66);//18bit,262k
	  //send_data_cmd(0x55);//16bit,56k,DPI [2:0],DBI [2:0]
	/////////////////////////////////////
	///////gamma 2.2 setting/////////////

		send_ctrl_cmd(0xC0);
		send_data_cmd(0x1D);
		send_data_cmd(0x0A);
		send_ctrl_cmd(0xC1);
		send_data_cmd(0x01);
		send_ctrl_cmd(0xC2);
		send_data_cmd(0x22);
		send_ctrl_cmd(0xC5);
		send_data_cmd(0x20);//2f
		send_data_cmd(0x20);
		send_ctrl_cmd(0xC7);
		send_data_cmd(0xDb);//c3
		send_ctrl_cmd(0xB8);
		send_data_cmd(0x0b);//08
		send_ctrl_cmd(0xE0);
		send_data_cmd(0x0F);
		send_data_cmd(0x27);
		send_data_cmd(0x23);
		send_data_cmd(0x0b);
		send_data_cmd(0x0F);
		send_data_cmd(0x09);
		send_data_cmd(0x52);
		send_data_cmd(0x99);
		send_data_cmd(0x3f);
		send_data_cmd(0x0a);
		send_data_cmd(0x11);
		send_data_cmd(0x03);
		send_data_cmd(0x08);
		send_data_cmd(0x05);
		send_data_cmd(0x00);
		send_ctrl_cmd(0xE1);
		 send_data_cmd(0x00);
		 send_data_cmd(0x18);
		 send_data_cmd(0x1c);
		 send_data_cmd(0x04);
		 send_data_cmd(0x10);
		 send_data_cmd(0x06);
		 send_data_cmd(0x2d);
		 send_data_cmd(0x66);
		 send_data_cmd(0x40);
		 send_data_cmd(0x05);
		 send_data_cmd(0x0e);
		 send_data_cmd(0x0c);
		 send_data_cmd(0x37);
		 send_data_cmd(0x34);
		 send_data_cmd(0x0F);

	/////////////////////////////////////
	/////////////////////////////////////
	///////gamma 2.5 setting/////////////
	/*
	  send_ctrl_cmd(0xC0);		 //Power Control 1
	  send_data_cmd(0x1D);//VRH[5:0]
	  send_data_cmd(0x0A);//VC[3:0]
	  send_ctrl_cmd(0xC1);		 //Power Control 2
	  send_data_cmd(0x01);//SAP[2:0],BT[3:0]
	  send_ctrl_cmd(0xC2);		 //Power Control 2
	  send_data_cmd(0x22);//SAP[2:0],BT[3:0]
	  send_ctrl_cmd(0xC5);		 //VCOM Control 1
	  send_data_cmd(0x25);//0x20   VMH[6:0]
	  send_data_cmd(0x21);//VML[6:0]
	  send_ctrl_cmd(0xC7);		 //VCOM Control 2
	  send_data_cmd(0xD4);//nVM,VMF[6:0]
	  send_ctrl_cmd(0xB8);		 //Oscillator Control
	  send_data_cmd(0x0B);//FOSC[3:0]
	  send_ctrl_cmd(0xE0);
	  send_data_cmd(0x1F);//P01-VP63
	  send_data_cmd(0x26);//P02-VP62
	  send_data_cmd(0x23);//P03-VP61
	  send_data_cmd(0x0A);//P04-VP59
	  send_data_cmd(0x10);//P05-VP57
	  send_data_cmd(0x09);//P06-VP50
	  send_data_cmd(0x51);//P07-VP43
	  send_data_cmd(0xA9);//P08-VP27,36
	  send_data_cmd(0x3C);//P09-VP20
	  send_data_cmd(0x06);//P10-VP13
	  send_data_cmd(0x0D);//P11-VP6
	  send_data_cmd(0x05);//P12-VP4
	  send_data_cmd(0x10);//P13-VP2
	  send_data_cmd(0x09);//P14-VP1
	  send_data_cmd(0x00);//P15-VP0
	  send_ctrl_cmd(0xE1);
	  send_data_cmd(0x00);//P01
	  send_data_cmd(0x19);//P02
	  send_data_cmd(0x1C);//P03
	  send_data_cmd(0x05);//P04
	  send_data_cmd(0x0F);//P05
	  send_data_cmd(0x06);//P06
	  send_data_cmd(0x2E);//P07
	  send_data_cmd(0x36);//P08
	  send_data_cmd(0x43);//P09
	  send_data_cmd(0x09);//P10
	  send_data_cmd(0x12);//P11
	  send_data_cmd(0x0A);//P12
	  send_data_cmd(0x2F);//P13
	  send_data_cmd(0x36);//P14
	  send_data_cmd(0x1F);//P15
	*/
	/////////////////////////////////////
	  send_ctrl_cmd(0x2A);
	  send_data_cmd(0x00);
	  send_data_cmd(0x00);
	  send_data_cmd(0x01);
	  send_data_cmd(0x3F);

	  send_ctrl_cmd(0x2B);
	  send_data_cmd(0x00);
	  send_data_cmd(0x00);
	  send_data_cmd(0x00);
	  send_data_cmd(0xEF);

	  send_ctrl_cmd(0xF2);
	  send_data_cmd(0x00);

	  send_ctrl_cmd(0x11);//Exit Sleep
	  MDELAY(80);
	  send_ctrl_cmd(0x11);//Exit Sleep
	  MDELAY(80);

	  send_ctrl_cmd(0x29);//Display On
	  MDELAY(20);

	  send_ctrl_cmd(0x2C);//Display On

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
       send_ctrl_cmd(0xD3);  // SET password
	read_data_cmd();  //dummy read
	read_data_cmd();	//dummy read
	read_data_cmd();	//dummy read
	read_data_cmd();
    	return (LCM_ID ==	read_data_cmd())?1:0; //0x42
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9342_lcm_drv =
{
    	.name			= "ili9342",
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
	id1 =  read_data_cmd();	// 0x84
	id1 =  (id1 << 8) |read_data_cmd();	// 0x63
	id1 =  (id1 << 8) |read_data_cmd();	// 0x04
	id1 =  (id1 << 8) |read_data_cmd();	// 0x00

	send_ctrl_cmd(0xc5);
	read_data_cmd(); //dummy read
	id2 = read_data_cmd();  //20
	id2 =  (id2 << 8) | read_data_cmd(); //20

	send_ctrl_cmd(0xc7);
	read_data_cmd(); //dummy read
	id2 =  (id2<< 8) | read_data_cmd();  //Db

	if( (id1 != 0x84630400) || (id2 != 0x2020db))
	{
		printk("---------------------------lcm_init ---------------------------\n");
		lcm_init();
		jrd_lcm_update(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
	}
	return 0;
}

JRD_LCM_ESD ili9342_lcm_esd =
{
	.name = "ili9342",
	.get_register_value = get_power_setting_status,
};

#endif

#endif



