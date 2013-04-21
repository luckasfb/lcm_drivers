#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif


#include "lcm_drv.h"

// ---------------------------------------------------------------------------
// Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH (480)
#define FRAME_HEIGHT (800)

#define REGFLAG_DELAY 0XFD
#define REGFLAG_END_OF_TABLE 0xFE // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE 1

#ifdef BUILD_LK
#define LCD_PRINT	printf
#else
#define LCD_PRINT	printk
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif


#define LCM_ID_OTM8009A       (0x8009)

// ---------------------------------------------------------------------------
// Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

//static kal_bool IsFirstBoot = KAL_TRUE;

// ---------------------------------------------------------------------------
// Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)



static unsigned int lcm_compare_id(void);
static unsigned int lcm_esd_check(void);
static unsigned int lcm_esd_recover(void);
static void lcm_resume(void);


int  DSI_SetMode(unsigned int mode);
//void DSI_clk_HS_mode(bool enter);

static struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[120];
};


static struct LCM_setting_table lcm_initialization_setting[] = {

	/*
	Note :

	Data ID will depends on the following rule.

	count of parameters > 1 => Data ID = 0x39
	count of parameters = 1 => Data ID = 0x15
	count of parameters = 0 => Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag

	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/


	{0x00,1,{0x00}},  		
	{0xFF,3,{0x80,0x09,0x01}}, 

	{0x00,1,{0x80}},   		
	{0xFF,2,{0x80,0x09}},   

	{0x00,1,{0x80}},   	
	{0xF5,12,{0x01,0x18,0x02,0x18,0x10,0x18,0x02,0x18,0x0e,0x18,0x0f,0x20}},

	{0x00,1,{0x90}},
	{0xF5,10,{0x02,0x18,0x08,0x18,0x06,0x18,0x0d,0x18,0x0b,0x18}},

	{0x00,1,{0xA0}},   	
	{0xF5,8,{0x10,0x18,0x01,0x18,0x14,0x18,0x14,0x18}},

	{0x00,1,{0xB0}},   	
	{0xF5,12,{0x14,0x18,0x12,0x18,0x13,0x18,0x11,0x18,0x13,0x18,0x00,0x00}},

	{0x00,1,{0x80}},   	
	{0xC0,5,{0x00,0x5F,0x00,0x10,0x10}},   	

	{0x00,1,{0xB4}},   		
	{0xC0,1,{0x50}},//1+2 dot update on 7/12

	{0x00,1,{0x82}},   		
	{0xC5,1,{0xA3}},   

	{0x00,1,{0x90}},   		
	{0xC5,2,{0x96,0x76}},

	{0x00,1,{0x00}},   		
	{0xD8,2,{0xE0,0xE0}},  

	{0x00,1,{0x00}},   		
	{0xD8,2,{0xA7,0xA7}},  // 6F GVDD/NGVDD = +/-4.5V

	{0x00,1,{0x00}},   		
	{0xD9,1,{0x7A}},   //85		

	{0x00,1,{0x81}},   	
	{0xC1,1,{0x66}},   	

	{0x00,1,{0xA1}},   		
	{0xC1,1,{0x08}},  

	{0x00,1,{0xA3}},   		
	{0xC0,1,{0x1B}}, 


	{0x00,1,{0x81}},   		
	{0xC4,1,{0x83}},   


	{0x00,1,{0x92}},   		
	{0xC5,1,{0x01}},  


	{0x00,1,{0xB1}},   		
	{0xC5,1,{0xA9}},   

	{0x00,1,{0x90}},   		
	{0xB3,1,{0x00}},


	{0x00,1,{0x92}},   		
	{0xB3,1,{0x05}},


	{0x00,1,{0x80}},   	
	{0xCE,6,{0x84,0x03,0x00,0x83,0x03,0x00}},  

	{0x00,1,{0x90}},   	 		
	{0xCE,6,{0x33,0x27,0x00,0x33,0x28,0x00}}, //update on 7/12

	{0x00,1,{0xA0}},   	
	{0xCE,14,{0x38,0x02,0x03,0x21,0x00,0x00,0x00,0x38,0x01,0x03,0x22,0x00,0x00,0x00}}, //update on 7/12


	{0x00,1,{0xB0}},   	
	{0xCE,14,{0x38,0x00,0x03,0x23,0x00,0x00,0x00,0x30,0x00,0x03,0x24,0x00,0x00,0x00}}, //update on 7/12

	{0x00,1,{0xC0}},  
	{0xCE,14,{0x30,0x01,0x03,0x25,0x00,0x00,0x00,0x30,0x02,0x03,0x26,0x00,0x00,0x00}}, //update on 7/12	

	{0x00,1,{0xD0}},   	
	{0xCE,14,{0x30,0x03,0x03,0x27,0x00,0x00,0x00,0x30,0x04,0x03,0x28,0x00,0x00,0x00}}, 	//update on 7/12

	{0x00,1,{0xC7}},   		
	{0xCF,1,{0x00}},  //00h GOA falling precharge  80h GOA rise and falling precharge

	{0x00,1,{0xC0}},   		
	{0xCB,15,{0x00,0x00,0x00,0x00,0x54,0x54,0x54,0x54,0x00,0x54,0x00,0x54,0x00,0x00,0x00}}, 			

	{0x00,1,{0xD0}},   		
	{0xCB,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0x54,0x54,0x54,0x00,0x54}}, 

	{0x00,1,{0xE0}}, 
	{0xCB,9,{0x00,0x54,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 

	{0x00,1,{0x80}},  		
	{0xCC,10,{0x00,0x00,0x00,0x00,0x0C,0x0a,0x10,0x0E,0x00,0x02}}, 

	{0x00,1,{0x90}},   		
	{0xCC,15,{0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0B}}, 

	{0x00,1,{0xA0}},   		
	{0xCC,15,{0x09,0x0F,0x0D,0x00,0x01,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 

	{0x00,1,{0xB0}},   		
	{0xCC,10,{0x00,0x00,0x00,0x00,0x0D,0x0F,0x09,0x0B,0x00,0x05}}, 

	{0x00,1,{0xC0}},   		
	{0xCC,15,{0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E}}, 

	{0x00,1,{0xD0}},   		
	{0xCC,15,{0x10,0x0A,0x0C,0x00,0x06,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 

	{0x00,1,{0x00}},   		

	{0xE1,16,{0x00,0x0a,0x11,0x0e,0x07,0x0f,0x0b,0x0b,0x04,0x07,0x0d,0x09,0x10,0x0f,0x08,0x03}}, 

	{0x00,1,{0x00}},   		

	{0xE2,16,{0x00,0x0a,0x11,0x0e,0x07,0x0f,0x0b,0x0b,0x04,0x07,0x0d,0x09,0x10,0x0f,0x08,0x03}},
 
	{0x35,	1,	{0x00}},//TE enable
	{0x3A,	1,	{0x77}},
	{0x11,	1,	{0x00}},
	{REGFLAG_DELAY, 120, {}},
	{0x29,	1,	{0x00}},
	{REGFLAG_DELAY, 20, {}},
	{0x2C,	0,	{}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};



static struct LCM_setting_table lcm_sleep_out_setting[] = {
	// Sleep Out
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	//{0x2c, 0, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	// Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};



static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
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
			//MDELAY(10);//soso add or it will fail to send register
		}
	}

	LCD_PRINT("%s, -1.2, i = %d, delay cmd=%d, end cmd=%d", __FUNCTION__, i,REGFLAG_DELAY,REGFLAG_END_OF_TABLE);	
}


// ---------------------------------------------------------------------------
// LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode 			= LCM_DBI_TE_MODE_VSYNC_ONLY;  //LCM_DBI_TE_MODE_DISABLED;//
	params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_TWO_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 2;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

	// Bit rate calculation
        //26->351M,have tearing;34->455M;30=>403M,no tearing
	params->dsi.pll_div1=30;//34		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
	params->dsi.pll_div2=1;			// div2=0~15: fout=fvo/(2*div2)
#if 0
	params->dsi.HS_TRAIL    = 10;//10;//min max(n*8*UI, 60ns+n*4UI)
	params->dsi.HS_ZERO    = 8;//8;//min 105ns+6*UI 
	params->dsi.HS_PRPR    = 4;//min 40ns+4*UI; max 85ns+6UI 
	params->dsi.LPX    = 12;//min 50ns 

	params->dsi.TA_SACK=1; 
	params->dsi.TA_GET=60;
	params->dsi.TA_SURE=18;  
	params->dsi.TA_GO    = 12;//12;//4*LPX
	//
	params->dsi.CLK_TRAIL   = 5;//5;//min 60ns
	params->dsi.CLK_ZERO    = 18;//18;//min 300ns-38ns
	params->dsi.LPX_WAIT    = 10;
	params->dsi.CONT_DET    = 0; 

	params->dsi.CLK_HS_PRPR = 4;//min 38ns; max 95ns
#endif
}

static unsigned int lcm_compare_id(void)
{

	int   array[4];
	char  buffer[5]={0};
	char  id_high=0;
	char  id_low=0;
	int   id=0;
	
	SET_RESET_PIN(1);  //NOTE:should reset LCM firstly
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	array[0] = 0x00001500;
	dsi_set_cmdq(array, 1, 1);
	array[0] = 0x00043902;
	array[1] = 0x010980FF;
	dsi_set_cmdq(array, 2, 1);

	array[0] = 0x80001500;
	dsi_set_cmdq(array, 1, 1);
	array[0] = 0x00033902;
	array[1] = 0x000980FF;
	dsi_set_cmdq(array, 2, 1);

	array[0] = 0x03001500;
	dsi_set_cmdq(array, 1, 1);
	array[0] = 0x01FF1500;
	dsi_set_cmdq(array, 1, 1);

	//array[0] = 0x50001500;
	//dsi_set_cmdq(array, 1, 1);
	array[0] = 0x00053700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	//read_reg_v2(0xF8,buffer, 5);
	read_reg_v2(0xA1, buffer, 5);//0x018b8009ff or 0x09ff8009ff?

	id_high = buffer[2];
	id_low = buffer[3];
	id = (id_high << 8) | id_low;
	
	#ifdef BUILD_LK
		printf("LK@otm8009a id = %x,%x,%x,%x,%x,%x \n",id,buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
    #else
		printk("LK@otm8009a id = %x,%x,%x,%x,%x,%x \n",id,buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
    #endif

	return (LCM_ID_OTM8009A == id)?1:0;
}

static void lcm_init(void)
{
   #ifdef BUILD_LK
    printf("lcm ~~~~~~~~~~~~init!!!!!!!!\n"); 
   #else
    printk("lcm ~~~~~~~~~~~~init!!!!!!!!\n"); 
   #endif


    SET_RESET_PIN(1);
    MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}


static void lcm_suspend(void)
{
    LCD_PRINT("%s\r\n", __FUNCTION__);	

    //should do reset??
    //SET_RESET_PIN(1);
    //MDELAY(5);
    //SET_RESET_PIN(0);
    //MDELAY(10);
    //SET_RESET_PIN(1);
    //MDELAY(120);

    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

}


static void lcm_resume(void)
{
	LCD_PRINT("%s\r\n", __FUNCTION__);	
	lcm_init();

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

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}




LCM_DRIVER s90446_wvga_dsi_cmd_otm8009a_lcm_drv = 
{
    .name			= "s90446_dsi_otm8009a",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	//.set_backlight	= lcm_setbacklight,
	//.esd_recover   = lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
	.compare_id = lcm_compare_id,
};
