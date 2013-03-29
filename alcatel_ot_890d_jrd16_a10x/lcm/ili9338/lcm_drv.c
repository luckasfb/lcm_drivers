


#include <linux/string.h>
#include "lcm_drv.h"

#define FRAME_WIDTH  (240)
#define FRAME_HEIGHT (320)

static LCM_UTIL_FUNCS lcm_util = { 0 };

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

static int lcm_id = 1;


static __inline void send_ctrl_cmd(unsigned int cmd)
{
	lcm_util.send_cmd(cmd);
}

static __inline void send_data_cmd(unsigned int data)
{
	lcm_util.send_data(data);
}

static __inline void set_lcm_register(unsigned int regIndex, unsigned int regData)
{
	send_ctrl_cmd(regIndex);
	send_data_cmd(regData);
}

const unsigned int lcm_id_detect(void)
{
	int id;

	send_ctrl_cmd(0xDA);
	lcm_util.read_data();
	id = lcm_util.read_data();

	if (id == 84)
		return 0;
	else {			//the driver can't probe ili tft lcd.
		lcm_id = 0;
		return 1;
	}
}


static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS * params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DBI;
	params->ctrl = LCM_CTRL_PARALLEL_DBI;
	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dbi.port = 0;
	params->dbi.clock_freq = LCM_DBI_CLOCK_FREQ_52M;
	params->dbi.data_width = LCM_DBI_DATA_WIDTH_18BITS;
	params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;	/// LCM_COLOR_ORDER_BGR;
	params->dbi.data_format.trans_seq = LCM_DBI_TRANS_SEQ_LSB_FIRST;
	params->dbi.data_format.padding = LCM_DBI_PADDING_ON_LSB;
	params->dbi.data_format.format = LCM_DBI_FORMAT_RGB666;
	params->dbi.data_format.width = LCM_DBI_DATA_WIDTH_18BITS;
	params->dbi.cpu_write_bits = LCM_DBI_CPU_WRITE_32_BITS;
	params->dbi.io_driving_current = LCM_DRIVING_CURRENT_8MA;

	params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;	/// LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

#if 1
	/*
	params->dbi.parallel.write_setup = 0;
	params->dbi.parallel.write_hold = 2;
	params->dbi.parallel.write_wait = 4;
	params->dbi.parallel.read_setup = 0;
	params->dbi.parallel.read_latency = 11;
	params->dbi.parallel.wait_period = 0;
	*/
	params->dbi.parallel.write_setup = 2;
	params->dbi.parallel.write_hold = 2;
	params->dbi.parallel.write_wait = 5;
	params->dbi.parallel.read_setup = 2;
	params->dbi.parallel.read_latency = 20;
	params->dbi.parallel.wait_period = 10;
#else
	params->dbi.parallel.write_setup = 10;
	params->dbi.parallel.write_hold = 13;
	params->dbi.parallel.write_wait = 13;
	params->dbi.parallel.read_setup = 12;
	params->dbi.parallel.read_latency = 29;
	params->dbi.parallel.wait_period = 10;
#endif
}

static void init_lcm_ili_registers(void)
{
	send_ctrl_cmd(0x11);
	MDELAY(100);

	send_ctrl_cmd(0x3A);
	send_data_cmd(0x06);

	send_ctrl_cmd(0xCB);
	send_data_cmd(0x01);

	send_ctrl_cmd(0xC0);
	send_data_cmd(0x26);
	send_data_cmd(0x01);

	send_ctrl_cmd(0xC1);
	send_data_cmd(0x44);

	send_ctrl_cmd(0xC5);
	send_data_cmd(0x2d);
	send_data_cmd(0x38);

	send_ctrl_cmd(0xC7);
	send_data_cmd(0xC4);

	send_ctrl_cmd(0x35);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xB1);
	send_data_cmd(0x00);
	send_data_cmd(0x11);

	send_ctrl_cmd(0x36);
	send_data_cmd(0x08);

	send_ctrl_cmd(0xB6);
	send_data_cmd(0x0A);
	send_data_cmd(0x22);
	send_data_cmd(0x27);

	send_ctrl_cmd(0xE0);
	send_data_cmd(0x1F);
	send_data_cmd(0x18);
	send_data_cmd(0x16);
	send_data_cmd(0x08);
	send_data_cmd(0x10);
	send_data_cmd(0x08);
	send_data_cmd(0x45);
	send_data_cmd(0x9B);
	send_data_cmd(0x34);
	send_data_cmd(0x2A);
	send_data_cmd(0x0F);
	send_data_cmd(0x07);
	send_data_cmd(0x07);
	send_data_cmd(0x07);
	send_data_cmd(0x00);

	send_ctrl_cmd(0XE1);
	send_data_cmd(0x00);
	send_data_cmd(0x27);
	send_data_cmd(0x29);
	send_data_cmd(0x08);
	send_data_cmd(0x0F);
	send_data_cmd(0x07);
	send_data_cmd(0x3A);
	send_data_cmd(0x46);
	send_data_cmd(0x4B);
	send_data_cmd(0x15);
	send_data_cmd(0x10);
	send_data_cmd(0x08);
	send_data_cmd(0x38);
	send_data_cmd(0x38);
	send_data_cmd(0x1F);

	send_ctrl_cmd(0x29);
	MDELAY(120);
}

static void init_lcm_himax_registers(void)
{
	//driving ability setting   
	set_lcm_register(0xEA, 0x00);	//PTBA[15:8]
	set_lcm_register(0xEB, 0x20);	//PTBA[7:0]
	set_lcm_register(0xEC, 0x0C);	//STBA[15:8]
	set_lcm_register(0xED, 0xC4);	//STBA[7:0]
	set_lcm_register(0xE8, 0x40);	//OPON[7:0]
	set_lcm_register(0xE9, 0x38);	//OPON1[7:0]
	set_lcm_register(0xF1, 0x01);	//OTPS1B
	set_lcm_register(0xF2, 0x10);	//GEN
	set_lcm_register(0x27, 0xA3);

	//gamma 2.2 setting
	set_lcm_register(0x40, 0x00);
	set_lcm_register(0x41, 0x00);
	set_lcm_register(0x42, 0x01);
	set_lcm_register(0x43, 0x13);
	set_lcm_register(0x44, 0x10);
	set_lcm_register(0x45, 0x26);
	set_lcm_register(0x46, 0x08);
	set_lcm_register(0x47, 0x51);
	set_lcm_register(0x48, 0x02);
	set_lcm_register(0x49, 0x12);
	set_lcm_register(0x4A, 0x18);
	set_lcm_register(0x4B, 0x19);
	set_lcm_register(0x4C, 0x14);

	set_lcm_register(0x50, 0x19);
	set_lcm_register(0x51, 0x2F);
	set_lcm_register(0x52, 0x2C);
	set_lcm_register(0x53, 0x3E);
	set_lcm_register(0x54, 0x3F);
	set_lcm_register(0x55, 0x3F);
	set_lcm_register(0x56, 0x2E);
	set_lcm_register(0x57, 0x77);
	set_lcm_register(0x58, 0x0B);
	set_lcm_register(0x59, 0x06);
	set_lcm_register(0x5A, 0x07);
	set_lcm_register(0x5B, 0x0D);
	set_lcm_register(0x5C, 0x1D);
	set_lcm_register(0x5D, 0xCC);

	//power voltage setting
	set_lcm_register(0x1B, 0x1B);	//VRH=4.65V/VREG1 Voltage
	set_lcm_register(0x1A, 0x01);	//BT (VGH~15V,VGL~-10V,DDVDH~5V)/0X05(VGH~15v,VGL~-5V,DDVDH~5)
	set_lcm_register(0x24, 0x37);	//VMH(VCOM High voltage ~3.2V)/VCOMH\u8983\u8ab9
	set_lcm_register(0x25, 0x59);	//VML(VCOM Low voltage -1.2V)\ufe5cVCOML\u8983\u8ab9

	//VCOM offset
	set_lcm_register(0x23, 0x86);	//for flicker adjust //can reload from OTP

	//power on setting
	set_lcm_register(0x18, 0x39);	//I/P_RADJ,N/P_RADJ, Normal mode 150Hz
	set_lcm_register(0x19, 0x01);	//OSC_EN='1', start Osc/osc internal
	set_lcm_register(0x01, 0x00);	//DP_STB='0', out deep sleep
	set_lcm_register(0x1F, 0x88);	// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
	MDELAY(5);
	set_lcm_register(0x1F, 0x80);	// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
	MDELAY(5);
	set_lcm_register(0x1F, 0x90);	// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
	MDELAY(5);
	set_lcm_register(0x1F, 0xD0);	// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
	MDELAY(5);

	//18bBit/pixel 16-bits data bus
	set_lcm_register(0x17, 0x07);	//default 0x06 262k color // 0x05 65k color/0x07\u5c8618bit/pixel 16-bits data bus

	//set panel
	set_lcm_register(0x36, 0x00);	//SS_P, GS_P,REV_P,BGR_P

	//display on setting
	set_lcm_register(0x28, 0x38);	//GON=1, DTE=1, D=1000/source=PT
	MDELAY(40);
	set_lcm_register(0x28, 0x3C);	//GON=1, DTE=1, D=1100/source=display

	//set GRAM area
	set_lcm_register(0x02, 0x00);
	set_lcm_register(0x03, 0x00);	//column start
	set_lcm_register(0x04, 0x00);
	set_lcm_register(0x05, 0xEF);	//column end
	set_lcm_register(0x06, 0x00);
	set_lcm_register(0x07, 0x00);	//row start
	set_lcm_register(0x08, 0x01);
	set_lcm_register(0x09, 0x3F);	//row end

	send_ctrl_cmd(0x22);	//start GRAM write     
}

static void lcm_ili_init(void)
{
	SET_RESET_PIN(0);
	MDELAY(100);
	SET_RESET_PIN(1);
	MDELAY(500);

	init_lcm_ili_registers();
}

static void lcm_himax_init(void)
{
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(150);

	init_lcm_himax_registers();
}

static void lcm_ili_suspend(void)
{
	send_ctrl_cmd(0x28);
	send_ctrl_cmd(0x10);
	MDELAY(120);
}

static void lcm_himax_suspend(void)
{
	set_lcm_register(0x28, 0xB8);	//GON=\uff0a1\uff0a DTE=\uff0a1\uff0a D[1:0]=\uff0a10\uff0a 
	MDELAY(40);
	set_lcm_register(0x1F, 0x89);	// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=1 
	MDELAY(40);
	set_lcm_register(0x28, 0x04);	//GON=\uff0a0\uff0a DTE=\uff0a0\uff0a D[1:0]=\uff0a01\uff0a 
	MDELAY(40);
	set_lcm_register(0x28, 0x24);	//GON=\uff0a1 DTE=\uff0a0\uff0a D[1:0]=\uff0a01\uff0a 
	MDELAY(40);
	set_lcm_register(0x19, 0x00);	//OSC_EN=\uff0a0\uff0a 
	MDELAY(5);
}

static void lcm_ili_resume(void)
{
	/* FIXME: above wakup sequence does NOT work,
	   workaround by reinit LCM
	 */
	SET_RESET_PIN(0);
	MDELAY(100);
	SET_RESET_PIN(1);
	MDELAY(500);
	init_lcm_ili_registers();
}

static void lcm_himax_resume(void)
{
	set_lcm_register(0x18, 0x39);	//I/P_RADJ,N/P_RADJ, Normal mode 75Hz 
	set_lcm_register(0x19, 0x01);	//OSC_EN='1', start Osc 
	set_lcm_register(0x1F, 0x88);	// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0 
	MDELAY(5);
	set_lcm_register(0x1F, 0x80);	// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0 
	MDELAY(5);
	set_lcm_register(0x1F, 0x90);	// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0 
	MDELAY(5);
	set_lcm_register(0x1F, 0xD0);	// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0 
	MDELAY(5);
	set_lcm_register(0x28, 0x38);	//GON=1, DTE=1, D=1000 
	MDELAY(40);
	set_lcm_register(0x28, 0x3C);	//GON=1, DTE=1, D=1100  
}

static void lcm_ili_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	send_ctrl_cmd(0x2A);
	send_data_cmd((x0 >> 8) & 0xFF);
	send_data_cmd(x0 & 0xFF);
	send_data_cmd((x1 >> 8) & 0xFF);
	send_data_cmd(x1 & 0xFF);

	send_ctrl_cmd(0x2B);
	send_data_cmd((y0 >> 8) & 0xFF);
	send_data_cmd(y0 & 0xFF);
	send_data_cmd((y1 >> 8) & 0xFF);
	send_data_cmd(y1 & 0xFF);

	send_ctrl_cmd(0x2C);
}

static void lcm_himax_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	send_ctrl_cmd(0x02);
	send_data_cmd((x0 >> 8) & 0xFF);
	send_ctrl_cmd(0x03);
	send_data_cmd(x0 & 0xFF);
	send_ctrl_cmd(0x04);
	send_data_cmd((x1 >> 8) & 0xFF);
	send_ctrl_cmd(0x05);
	send_data_cmd(x1 & 0xFF);

	send_ctrl_cmd(0x06);
	send_data_cmd((y0 >> 8) & 0xFF);
	send_ctrl_cmd(0x07);
	send_data_cmd(y0 & 0xFF);
	send_ctrl_cmd(0x08);
	send_data_cmd((y1 >> 8) & 0xFF);
	send_ctrl_cmd(0x09);
	send_data_cmd(y1 & 0xFF);

	send_ctrl_cmd(0x22);
}

const LCM_DRIVER *LCM_GetDriver()
{
	static const LCM_DRIVER LCM_ILI_DRV = {
		.set_util_funcs = lcm_set_util_funcs,
		.get_params = lcm_get_params,		
		.init = lcm_ili_init,		
		.suspend = lcm_ili_suspend,
		.resume = lcm_ili_resume,
		.update = lcm_ili_update
	};	
	return &LCM_ILI_DRV;
}

const LCM_DRIVER *LCM_GetJrdDriver()
{
	static const LCM_DRIVER LCM_HIMAX_DRV = {
		.set_util_funcs = lcm_set_util_funcs,
		.get_params = lcm_get_params,		
		.init = lcm_himax_init,
		.suspend = lcm_himax_suspend,
		.resume = lcm_himax_resume,
		.update = lcm_himax_update
	};
	return &LCM_HIMAX_DRV;
}