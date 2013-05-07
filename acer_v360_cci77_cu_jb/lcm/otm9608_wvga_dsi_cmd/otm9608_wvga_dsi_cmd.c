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

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (540)
#define FRAME_HEIGHT (960)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
//#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
       

#define LCM_DSI_CMD_MODE

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/
	{0x00,	1,	{0x00}},
	{0xFF,	3,	{0x96, 0x08, 0X01}},	
    {0x00,	1, {0x80}},
	{0xFF,	2, {0x96,0x08}},


	{0x00,	1, {0x00}},

	{0xA0,	1, {0x00}},


	{0x00,	1, {0x80}},
	{0xB3,	5, {0x00,0x00,0x20,0x00,0x00}},

	{0x00,	1, {0xC0}},
	{0xB3,	1, {0x09}},

	{0x00,	1, {0x80}},
	{0xC0,	9, {0x00,0x48,0x00,0x10,0x10,0x00,0x47,0x10,0x10}},

	{0x00,	1, {0x92}},
   	{0xC0,	4, {0x00,0x10,0x00,0x13}},

   	{0x00,	1, {0xA2}},
   	{0xC0,	3, {0x0c,0x05,0x02}},

   	{0x00,	1, {0xB3}},
   	{0xC0,	2, {0x00,0x50}},

   	{0x00,	1, {0x81}},
	{0xC1,	1, {0x55}},

   	{0x00,	1, {0x80}},
	{0xC4,	3, {0x00,0x84,0xfc}},  

   	{0x00,	1, {0xA0}},
	{0xB3,	2, {0x10,0x00}},

   	{0x00,	1, {0xA0}},
   	{0xC0,	1, {0x00}},

   	{0x00,	1, {0xA0}},
   	{0xC4,	8, {0x33,0x09,0x90,0x3A,0x33,0x09,0x90,0x45}},
    
   	{0x00,	1, {0x80}},
   	{0xC5,	4, {0x08,0x00,0xb0,0x11}},
    
	{0x00,  1, {0x90}},
    {0xc5,  7, {0xd6,0x57,0x00,0x57,0x33,0x33,0x34}},
		
	{0x00,  1, {0xa0}},
	{0xc5,  7, {0xd6,0x57,0x00,0x57,0x33,0x33,0x34}},
		
	{0x00,  1, {0xb0}},
	{0xc5,  7, {0x04,0xac,0x01,0x00,0x71,0xb1,0x83}},
		
	{0x00,  1, {0x00}},
	{0xd9,  1, {0x61}},
		
	{0x00,  1, {0x80}},
	{0xc6,  1, {0x64}},

	{0x00,  1, {0xb0}},
	{0xc6,  5, {0x03,0x10,0x00,0x1f,0x12}}, 
		
	{0x00,  1, {0xe1}},
	{0xc0,  1, {0x9f}},
		
	{0x00,  1, {0xb7}},
	{0xb0,  1, {0x10}},
		
	{0x00,  1, {0xc0}},
	{0xb0,  1, {0x55}},
		
	{0x00,  1, {0xb1}},
	{0xb0,  1, {0x03}},
		
	{0x00,  1, {0x81}},
	{0xd6,  1, {0x00}},
		
	{0x00,  1, {0x00}},
	{0xe1,  16, {0x01,0x0d,0x13,0x0f,0x07,0x11,0x0b,0x0a,0x03,0x06,0x0b,0x08,0x0d,0x0e,0x09,0x01}},
	{0xe1,  16, {0x00,0x0d,0x13,0x0f,0x07,0x11,0x0b,0x0a,0x03,0x06,0x0b,0x08,0x0d,0x0e,0x09,0x01}},
		
	{0x00,  1, {0x00}},
    {0xe2,  16, {0x02,0x0f,0x15,0x0e,0x08,0x10,0x0b,0x0c,0x02,0x04,0x0b,0x04,0x0e,0x0d,0x08,0x00}},
    {0xe2,  16, {0x00,0x0f,0x15,0x0e,0x08,0x10,0x0b,0x0c,0x02,0x04,0x0b,0x04,0x0e,0x0d,0x08,0x00}},
		
	{0x00,  1, {0x80}},
	{0xcb,  10, {0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0x90}},
	{0xcb,  15, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xa0}},
	{0xcb,  15, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xb0}},
	{0xcb,  10, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xc0}},
	{0xcb,  15, {0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x00,0x00}},
		
	{0x00,  1, {0xd0}},
	{0xcb,  15, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x00,0x00,0x04,0x04}},  
		
	{0x00,  1, {0xe0}},
    {0xcb,  10, {0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xf0}},
	{0xcb,  10, {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}},
		
	{0x00,  1, {0x80}},
	{0xcc,  10, {0x00,0x00,0x00,0x02,0x00,0x00,0x0a,0x0e,0x00,0x00}},
		
	{0x00,  1, {0x90}},
	{0xcc,  15, {0x0c,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x09}},
		
	{0x00,  1, {0xa0}},
	{0xcc,  15, {0x0d,0x00,0x00,0x0b,0x0f,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xb0}},
	{0xcc,  10, {0x00,0x00,0x00,0x02,0x00,0x00,0x0a,0x0e,0x00,0x00}}, 
		
	{0x00,  1, {0xc0}},
	{0xcc,  15, {0x0c,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xd0}},
	{0xcc,  15, {0x05,0x00,0x00,0x00,0x00,0x0f,0x0b,0x00,0x00,0x0d,0x09,0x01,0x00,0x00,0x00}},
		
	{0x00,  1, {0x80}},
    {0xce,  12, {0x84,0x03,0x18,0x83,0x03,0x18,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0x90}},
	{0xce,  14, {0x33,0xbf,0x18,0x33,0xc0,0x18,0x10,0x0f,0x18,0x10,0x10,0x18,0x00,0x00}},
		
	{0x00,  1, {0xa0}},
	{0xce,  14, {0x38,0x02,0x03,0xc1,0x00,0x18,0x00,0x38,0x01,0x03,0xc2,0x00,0x18,0x00}},
		
	{0x00,  1, {0xb0}},
	{0xce,  14, {0x38,0x00,0x03,0xc3,0x00,0x18,0x00,0x30,0x00,0x03,0xc4,0x00,0x18,0x00}},
		
	{0x00,  1, {0xc0}},
	{0xce,  14, {0x30,0x01,0x03,0xc5,0x00,0x18,0x00,0x30,0x02,0x03,0xc6,0x00,0x18,0x00}}, 
		
	{0x00,  1, {0xd0}},
	{0xce,  14, {0x30,0x03,0x03,0xc7,0x00,0x18,0x00,0x30,0x04,0x03,0xc8,0x00,0x18,0x00}}, 
		
	{0x00,  1, {0x80}},
	{0xcf,  14, {0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0x90}},
	{0xcf,  14, {0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xa0}},
	{0xcf,  14, {0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xb0}},
	{0xcf,  14, {0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00}},
		
	{0x00,  1, {0xc0}},
	{0xcf,  10, {0x01,0x01,0x20,0x20,0x00,0x00,0x02,0x04,0x00,0x00}},
		
	{0x00,  1, {0x00}},
	{0xd8,  2, {0xa7,0xa7}},
		
	{0x00,  1, {0x00}},
	{0xFF,  3, {0xFF,0xFF,0xFF}},
	{0x35,  1, 0x00},               // TE enable
    {REGFLAG_DELAY, 10, {}},
	{0x11,  0, {}},
    {REGFLAG_DELAY, 120, {}},
	{0x29,  0, {}},
    {REGFLAG_DELAY, 200, {}},

	// Setting ending by predefined flag
	//{REGFLAG_END_OF_TABLE, 0x00, {}}
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 200, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_compare_id_setting[] = {
	// Display off sequence
	{0xF0,	5,	{0x55, 0xaa, 0x52,0x08,0x00}},
	{REGFLAG_DELAY, 10, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
				//MDELAY(2);
       	}
    }
	
}


static void init_lcm_registers(void)
{
	unsigned int data_array[16];
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
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if defined(LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573

			//params->dsi.DSI_WMEM_CONTI=0x3C; 
			//params->dsi.DSI_RMEM_CONTI=0x3E; 

		params->dbi.te_mode=LCM_DBI_TE_MODE_DISABLED;
        
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 3;
		params->dsi.vertical_backporch					= 12;
		params->dsi.vertical_frontporch					= 2;
		params->dsi.vertical_active_line				= FRAME_HEIGHT;

		params->dsi.horizontal_sync_active				= 10;
		params->dsi.horizontal_backporch				= 50;
		params->dsi.horizontal_frontporch				= 50;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		// Bit rate calculation
		params->dsi.pll_div1=38;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		params->dsi.pll_div2=1;			// div2=0~15: fout=fvo/(2*div2)

}

static unsigned int lcm_compare_id(void)
{

		int   array[4];
		char  buffer[3];
        char  buffer1[2]= {0};
		char  id0=0;
		char  id1=0;
		char  id2=0;

#if 0
		SET_RESET_PIN(0);
		MDELAY(200);
		SET_RESET_PIN(1);
		MDELAY(200);
#endif		
	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xDA,buffer, 1);

	
	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xDB,buffer+1, 1);

	
	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xDC,buffer+2, 1);
	
	id0 = buffer[0]; //should be 0x00
	id1 = buffer[1];//should be 0xaa
	id2 = buffer[2];//should be 0x55

#if defined(BUILD_LK) 	
		printf("zhibin uboot %s\n", __func__);
		printf("%s, id0 = 0x%02x\n", __func__, id0);//should be 0x00
		printf("%s, id1 = 0x%02x\n", __func__, id1);//should be 0xaa
		printf("%s, id2 = 0x%02x\n", __func__, id2);//should be 0x55
#elif defined(BUILD_UBOOT)
		printf("zhibin uboot %s\n", __func__);
		printf("%s, id0 = 0x%02x\n", __func__, id0);//should be 0x00
		printf("%s, id1 = 0x%02x\n", __func__, id1);//should be 0xaa
		printf("%s, id2 = 0x%02x\n", __func__, id2);//should be 0x55
#else
		printk("zhibin kernel %s\n", __func__);	
        printk("%s, id0 = 0x%02x\n", __func__, id0);//should be 0x00
		printk("%s, id1 = 0x%02x\n", __func__, id1);//should be 0xaa
		printk("%s, id2 = 0x%02x\n", __func__, id2);//should be 0x55
#endif
	
	return 1;


}

static void lcm_init(void)
{
    
    lcm_util.set_gpio_mode(GPIO18, GPIO_MODE_01);

    SET_RESET_PIN(1);

    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(20);

#if defined(BUILD_LK) 	
		printf("zhibin uboot %s\n", __func__);
#elif defined(BUILD_UBOOT)
		printf("zhibin uboot %s\n", __func__);
#else
		printk("zhibin kernel %s\n", __func__);	
#endif

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	
}



static void lcm_suspend(void)
{
		push_table(lcm_sleep_mode_in_setting, sizeof(lcm_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
		//SET_RESET_PIN(0);
		//MDELAY(1);
		//SET_RESET_PIN(1);
	    //unsigned int data_array[2];
#if defined(BUILD_LK) 	
		printf("zhibin uboot %s\n", __func__);
#elif defined(BUILD_UBOOT)
		printf("zhibin uboot %s\n", __func__);
#else
		printk("zhibin kernel %s\n", __func__);
#endif

}


static void lcm_resume(void)
{
		//lcm_init();	
#if defined(BUILD_LK) 		
		printf("zhibin uboot %s\n", __func__);
#elif defined(BUILD_UBOOT)
		printf("zhibin uboot %s\n", __func__);
#else
		printk("zhibin kernel %s\n", __func__);
	
#endif
		push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);

}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

#if defined(BUILD_LK) 		
		printf("zhibin uboot %s\n", __func__);
#elif defined(BUILD_UBOOT)
		printf("zhibin uboot %s\n", __func__);
#else
		printk("zhibin kernel %s\n", __func__);	
#endif

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(&data_array, 7, 0);

}

static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
        if(lcm_esd_test)
        {
            lcm_esd_test = FALSE;
            return TRUE;
        }
        /// please notice: the max return packet size is 1
        /// if you want to change it, you can refer to the following marked code
        /// but read_reg currently only support read no more than 4 bytes....
        /// if you need to read more, please let BinHan knows.
        /*
                unsigned int data_array[16];
                unsigned int max_return_size = 1;
                
                data_array[0]= 0x00003700 | (max_return_size << 16);    
                
                dsi_set_cmdq(&data_array, 1, 1);
        */

        if(read_reg(0x0A) == 0x9C)
        {
            return FALSE;
        }
       else
        {            
            printk("justin test esd cover start!!!!!!!!!!!!!!!!!!!!\n");
            return TRUE;
        }
#endif
}

static unsigned int lcm_esd_recover(void)
{
#ifndef BUILD_LK
    unsigned char para = 0;

    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(20);

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

    return TRUE;
#endif
}


LCM_DRIVER otm9608_wvga_dsi_cmd_drv = 
{
    .name			= "otm9608a_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.compare_id     = lcm_compare_id,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#ifndef BUILD_LK
	.esd_check   = lcm_esd_check,
       .esd_recover   = lcm_esd_recover,
#endif

#if defined(LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
