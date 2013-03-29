

#include <linux/string.h>

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

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

#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
       

#define LCM_DSI_CMD_MODE

static void init_lcm_registers(void)
{
	unsigned int data_array[16];

#if defined(LCM_DSI_CMD_MODE)
	{
		data_array[0]=0x00043902;
		data_array[1]=0x6983FFB9;
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);

		data_array[0]=0x00103902;
		data_array[1]=0x032000B2;
		data_array[2]=0xFF007003;
		data_array[3]=0x00000000;
		data_array[4]=0x01000303;
		dsi_set_cmdq(&data_array, 5, 1);
		MDELAY(10);

		data_array[0]=0x00063902;
		data_array[1]=0x700800B4;
		data_array[2]=0x0000060E;
		dsi_set_cmdq(&data_array, 3, 1);
		MDELAY(10);

		data_array[0] = 0x001B3902;
		data_array[1] = 0x030100D5;
		data_array[2] = 0x08020100;
		data_array[3] = 0x00131180;
		data_array[4] = 0x51064000;
		data_array[5] = 0x71000007;
		data_array[6] = 0x07046005;
		data_array[7] = 0x0000060F;
		dsi_set_cmdq(&data_array, 8, 1);
		MDELAY(10);

		data_array[0] = 0x00143902;
		data_array[1] = 0x340085B1;
		data_array[2] = 0x0E0E0006;
		data_array[3] = 0x1A1A2C24;
		data_array[4] = 0xE6013A07;
		data_array[5] = 0xE6E6E6E6;
		dsi_set_cmdq(&data_array, 6, 1);
		MDELAY(10);

		// color format
		{
			// RGB565
			//data_array[0] = 0x053A1500;
			// RGB666
			//data_array[0] = 0x063A1500;
			// RGB888
			data_array[0] = 0x073A1500;
			dsi_set_cmdq(&data_array, 1, 1);
			MDELAY(10);
		}

		data_array[0] = 0x02CC1500;
		dsi_set_cmdq(&data_array, 1, 1);
		MDELAY(10);

		data_array[0] = 0x00033902;
		data_array[1] = 0x006C6CB6	;
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);

		// ENABLE FMARK
		//data_array[0] = 0x00023902;
		//data_array[1] = 0x00000035	;
		//dsi_set_cmdq(&data_array, 2, 1);
		//MDELAY(10);

		//data_array[0] = 0x00351500;
		//dsi_set_cmdq(&data_array, 1, 1);
		//MDELAY(10);

		// SET GAMMA
		data_array[0] = 0x00233902;
		data_array[1] = 0x140C00E0;
		data_array[2] = 0x293F3F3F;
		data_array[3] = 0x0F0C0654;
		data_array[4] = 0x15131513;
		data_array[5] = 0x0C001F14;
		data_array[6] = 0x3F3F3F14;
		data_array[7] = 0x0C065429;
		data_array[8] = 0x1315130F;
		data_array[9] = 0x001F1415;
		dsi_set_cmdq(&data_array, 10, 1);
		MDELAY(10);

		data_array[0] = 0x000E3902; //// SET MIPI (1 or 2 Lane)
		data_array[1] = 0xC6A000BA	;
		data_array[2] = 0x10000A00	;
		if (0)//(g_LaneNumber==1)
		{
			data_array[3] = 0x10026F30;
		}
		else
		{
			data_array[3] = 0x11026F30;
		}
		data_array[4] = 0x00004018	;
		dsi_set_cmdq(&data_array, 5, 1);
		MDELAY(10);

		data_array[0] = 0x00110500;	// Sleep Out
		dsi_set_cmdq(&data_array, 1, 1);
		MDELAY(100);

		data_array[0] = 0x00290500;	// Display On
		dsi_set_cmdq(&data_array, 1, 1);
		MDELAY(10);

	}
#else
	{
		data_array[0] = 0x00043902; // SET password
		data_array[1] = 0x6983FFB9; //
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);

		data_array[0] = 0x00143902; //// SET Power
		data_array[1] = 0x340085B1; //
		data_array[2] = 0x0F0F0007; //
		data_array[3] = 0x3F3F322A; //
		data_array[4] = 0xE6013A01; //
		data_array[5] = 0xE6E6E6E6;
		dsi_set_cmdq(&data_array, 6, 1);
		MDELAY(10);

		data_array[0] = 0x00103902; //// SET Display 480x800
		data_array[1] = 0x032300B2; //
		data_array[2] = 0xFF007003; //
		data_array[3] = 0x00000000; //
		data_array[4] = 0x01000303; //
		dsi_set_cmdq(&data_array, 5, 1);
		MDELAY(10);

		data_array[0] = 0x00063902; // SET Display
		data_array[1] = 0x801800B4;
		data_array[2] = 0x00000206;
		dsi_set_cmdq(&data_array, 3, 1);
		MDELAY(10);

		data_array[0] = 0x00033902; //// SET VCOM
		data_array[1] = 0x004242B6; 
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);

		data_array[0] = 0x001B3902; //// SET GIP
		data_array[1] = 0x030400D5; 
		data_array[2] = 0x28050100; 
		data_array[3] = 0x00030170; 
		data_array[4] = 0x51064000; 
		data_array[5] = 0x41000007;
		data_array[6] = 0x07075006;
		data_array[7] = 0x0000040F;
		dsi_set_cmdq(&data_array, 8, 1);
		MDELAY(10);

		data_array[0] = 0x00233902; //// SET GAMMA
		data_array[1] = 0x191300E0; //
		data_array[2] = 0x283F3D38; //
		data_array[3] = 0x0E0D0746; //
		data_array[4] = 0x14121512; //
		data_array[5] = 0x1300170F;
		data_array[6] = 0x3F3D3819;
		data_array[7] = 0x0D074628;
		data_array[8] = 0x1215120E;
		data_array[9] = 0x00170F14;
		dsi_set_cmdq(&data_array, 10, 1);
		MDELAY(10);

#if 0
		switch(g_ColorFormat)
		{
			case PACKED_RGB565:
				data_array[0] = 0x553A1500; // SET pixel format
			break;
			
			case LOOSED_RGB666:
				data_array[0] = 0x663A1500; // SET pixel format
			break; 
			
			case PACKED_RGB888:
				data_array[0] = 0x773A1500; // SET pixel format
			break;
			
			case PACKED_RGB666:
				data_array[0] = 0x663A1500; // SET pixel format
			break;   		

			default:
				//dbg_print("Format setting error \n\r");
				while(1);
			break;
		}

		dsi_set_cmdq(&data_array, 1, 1);
		MDELAY(100);
#endif
		data_array[0] = 0x000E3902; // SET MIPI (1 or 2 Lane)
		data_array[1] = 0xC6A000BA; //
		data_array[2] = 0x10000A00; //
		if (0)//(g_LaneNumber==1)
		{
			data_array[3] = 0x10026F30;
		}
		else
		{
			data_array[3] = 0x11026F30;
		}
		data_array[4] = 0x00004018;
		dsi_set_cmdq(&data_array, 5, 1);
		MDELAY(10);

		data_array[0] = 0x00110500;		// Sleep Out
		dsi_set_cmdq(&data_array, 1, 1);
		MDELAY(10);

		data_array[0] = 0x00290500;		// Display On
		dsi_set_cmdq(&data_array, 1, 1);
		MDELAY(10);
	}
#endif
	//------------------------------------------------------------------------
	// LUT for HX8369
	//------------------------------------------------------------------------
	if (1)//(g_ColorFormat==0)
	{
		data_array[0] = 0x00C13902; //Set Color LUT 1
		data_array[1] = 0x1008002D;
		data_array[2] = 0x30282018;               
		data_array[3] = 0x50484038;              
		data_array[4] = 0x70686058;                
		data_array[5] = 0x90888078;                
		data_array[6] = 0xB0A8A098;                
		data_array[7] = 0xD0C8C0B8;                
		data_array[8] = 0xF0E8E0D8;                
		data_array[9] = 0x000000FF;                
		data_array[10] = 0x00000000;              
		data_array[11] = 0x00000000;                
		data_array[12] = 0x00000000;               
		data_array[13] = 0x00000000;                
		data_array[14] = 0x00000000;               
		data_array[15] = 0x00000000;             
		dsi_set_cmdq(&data_array, 16, 1);
		MDELAY(10);

		data_array[0] = 0x00000000; //Set Color LUT 2
		data_array[1] = 0x08040000;
		data_array[2] = 0x1814100C;                
		data_array[3] = 0x2824201C;                
		data_array[4] = 0x3834302C;                
		data_array[5] = 0x4844403C;                
		data_array[6] = 0x5854504C;                
		data_array[7] = 0x6864605C;                
		data_array[8] = 0x7874706C;                
		data_array[9] = 0x8884807C;                
		data_array[10] = 0x9894908C;              
		data_array[11] = 0xA8A4A09C;                
		data_array[12] = 0xB8B4B0AC;               
		data_array[13] = 0xC8C4C0BC;                
		data_array[14] = 0xD8D4D0CC;               
		data_array[15] = 0xE8E4E0DC;               
		dsi_set_cmdq(&data_array, 16, 1);

		MDELAY(10);

		data_array[0] = 0xF8F4F0EC; //Set Color LUT 3
		data_array[1] = 0x100800FC;
		data_array[2] = 0x30282018;               
		data_array[3] = 0x50484038;               
		data_array[4] = 0x70686058;               
		data_array[5] = 0x90888078;               
		data_array[6] = 0xB0A8A098;               
		data_array[7] = 0xD0C8C0B8;               
		data_array[8] = 0xF0E8E0D8;               
		data_array[9] = 0x000000F8;               
		data_array[10] = 0x00000000;             
		data_array[11] = 0x00000000;               
		data_array[12] = 0x00000000;              
		data_array[13] = 0x00000000;               
		data_array[14] = 0x00000000;              
		data_array[15] = 0x00000000;             
		dsi_set_cmdq(&data_array, 16, 1);
		MDELAY(10);

		data_array[0] = 0x00000000; //Set Color LUT
		data_array[1] = 0x00000000;                                                                              
		dsi_set_cmdq(&data_array, 2, 1);                                                    
		MDELAY(10);
	}

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
#if defined(LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_EVENT_VDO_MODE;
#endif	
		params->ctrl   = LCM_CTRL_PARALLEL_DBI;
		//params->ctrl   = LCM_CTRL_NONE;
		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;
	
		// DBI
		//params->dbi.port                    = 0;
		params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_104M;
		params->dbi.data_width              = LCM_DBI_DATA_WIDTH_16BITS;
		params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
		params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_LSB;
		params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB888;
		params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_24BITS;
	  params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_16_BITS;
	  params->dbi.io_driving_current      = 0;

		// enable tearing-free
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
	
		// DPI
		params->dpi.format			  = LCM_DPI_FORMAT_RGB888;
		params->dpi.rgb_order		  = LCM_COLOR_ORDER_RGB;
		params->dpi.intermediat_buffer_num = 2;
	
		// DSI
		params->dsi.DSI_WMEM_CONTI=0x3C;
		params->dsi.DSI_RMEM_CONTI=0x3E;
		//params->dsi.LANE_NUM=LCM_ONE_LANE;
		params->dsi.LANE_NUM=LCM_TWO_LANE;
		params->dsi.VC_NUM=0x0;
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=480*3;
	
		params->dsi.vertical_sync_active=2;
		params->dsi.vertical_backporch=2;
		params->dsi.vertical_frontporch=2;
		params->dsi.vertical_active_line=800;
	
		params->dsi.line_byte=2180;		// 2256 = 752*3
		params->dsi.horizontal_sync_active_byte=26;
		params->dsi.horizontal_backporch_byte=206;
		params->dsi.horizontal_frontporch_byte=206;	
		params->dsi.rgb_byte=(480*3+6);	
	
		params->dsi.horizontal_sync_active_word_count=20;	
		params->dsi.horizontal_backporch_word_count=200;
		params->dsi.horizontal_frontporch_word_count=200;
	
		params->dsi.HS_TRAIL=0x14;
		params->dsi.HS_ZERO=0x14;
		params->dsi.HS_PRPR=0x0A;
		params->dsi.LPX=0x05;
	
		params->dsi.TA_SACK=0x01;
		params->dsi.TA_GET=0x37;
		params->dsi.TA_SURE=0x16;	
		params->dsi.TA_GO=0x10;
	
		params->dsi.CLK_TRAIL=0x14;
		params->dsi.CLK_ZERO=0x14;	
		params->dsi.LPX_WAIT=0x0A;
		params->dsi.CONT_DET=0x00;
	
		params->dsi.CLK_HS_PRPR=0x0A;
	
		params->dsi.pll_div1=37;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
	
		//params->dsi.pll_div2=4;			// div2=0~15: fout=fvo/(2*div2)
		params->dsi.pll_div2=1;			// div2=0~15: fout=fvo/(2*div2)

}


static void lcm_init(void)
{
    SET_RESET_PIN(0);
    MDELAY(25);
    SET_RESET_PIN(1);
    MDELAY(50);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
	unsigned int data_array[16];

	data_array[0]=0x00280500; // Display Off
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(10); 
	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(100);
}


static void lcm_resume(void)
{
	unsigned int data_array[16];

	data_array[0] = 0x00110500; // Sleep Out
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(100);
	data_array[0] = 0x00290500; // Display On
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(10);
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
	//data_array[6]= 0x002c3901;

	dsi_set_cmdq(&data_array, 7, 0);

}

LCM_DRIVER hx8369_dsi_lcm_drv = 
{
    .name			= "hx8369_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if defined(LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
