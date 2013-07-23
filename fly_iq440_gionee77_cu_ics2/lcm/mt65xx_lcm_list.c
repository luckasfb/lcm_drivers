#include <lcm_drv.h>
extern LCM_DRIVER hx8369_lcm_drv;
extern LCM_DRIVER hx8369_6575_lcm_drv;
extern LCM_DRIVER hx8363_6575_dsi_lcm_drv;
extern LCM_DRIVER hx8363_6575_dsi_hvga_lcm_drv;
//add by chendong 2012-05-13
extern LCM_DRIVER gn_hx8369_dsi_lcm_drv;
//add by chendong 2012-05-13 end
extern LCM_DRIVER bm8578_lcm_drv;
extern LCM_DRIVER nt35582_mcu_lcm_drv;
extern LCM_DRIVER nt35582_mcu_6575_lcm_drv;
extern LCM_DRIVER nt35582_rgb_6575_lcm_drv;
extern LCM_DRIVER hx8357b_lcm_drv;
extern LCM_DRIVER hx8369_dsi_lcm_drv;
extern LCM_DRIVER hx8369_dsi_6575_lcm_drv;
extern LCM_DRIVER hx8369_dsi_6575_hvga_lcm_drv;
extern LCM_DRIVER hx8369_dsi_6575_qvga_lcm_drv;
extern LCM_DRIVER hx8369_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8369b_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8369_hvga_lcm_drv;
extern LCM_DRIVER ili9481_lcm_drv;
extern LCM_DRIVER nt35582_lcm_drv;
extern LCM_DRIVER s6d0170_lcm_drv;
extern LCM_DRIVER spfd5461a_lcm_drv;
extern LCM_DRIVER ta7601_lcm_drv;
extern LCM_DRIVER tft1p3037_lcm_drv;
extern LCM_DRIVER ha5266_lcm_drv;
extern LCM_DRIVER hsd070idw1_lcm_drv;
extern LCM_DRIVER lg4571_lcm_drv;
extern LCM_DRIVER lvds_wsvga_lcm_drv;
extern LCM_DRIVER lvds_wsvga_ti_lcm_drv;
extern LCM_DRIVER lvds_wsvga_ti_n_lcm_drv;
extern LCM_DRIVER nt35565_3d_lcm_drv;
extern LCM_DRIVER tm070ddh03_lcm_drv;
extern LCM_DRIVER r61408_lcm_drv;
extern LCM_DRIVER nt35510_lcm_drv;
extern LCM_DRIVER nt35510_hvga_lcm_drv;
extern LCM_DRIVER nt35510_qvga_lcm_drv;
extern LCM_DRIVER nt35510_6517_lcm_drv;
extern LCM_DRIVER r63303_idisplay_lcm_drv;
//gionee chenqiang add for lcm_add 20120113 begin
#if defined (GN_MTK_BSP)
extern LCM_DRIVER gn_truly_nt35510_lcm_drv;
extern LCM_DRIVER gn_truly_r61408_lcm_drv;
extern LCM_DRIVER gn_byd_r61408_lcm_drv;
extern LCM_DRIVER gn_samsung_ld9042_lcm_drv;
//Gionee changph 20120417 add for GBW126A begin 
extern LCM_DRIVER gn_smd_lms397kf04_lcm_drv;
//Gionee changph 20120417 add for GBW126A end
#endif
//gionee chenqiang add for lcm_add 20120113 end

//gionee pengchao 20120428 add for GBW131 LCM start
#if defined(GN_SSD2825_SMD_S6E8AA)
extern LCM_DRIVER gn_ssd2825_smd_s6e8aa;
#endif
//gionee pengchao 20120428 add for GBW131 LCM end

//Gionee Qux add for wbw6803, 20120521
extern LCM_DRIVER gn_byd_hx8369_lcm_drv;
extern LCM_DRIVER gn_truly_hx8369_lcm_drv;
extern LCM_DRIVER gn_tianma_hx8369_lcm_drv;
//Gionee Qux. END

extern LCM_DRIVER gn_truly_nt35510_lcm_drv;

extern LCM_DRIVER gn_byd_nt35510_lcm_drv;

//Gionee Yangqb start for GBW138A, 20120620
extern LCM_DRIVER gn_smd_r69329_lcm_drv;
//Gionee Yangqb end for GBW138A, 20120620 

//Gionee changph 20120802 add
extern LCM_DRIVER gn_byd_otm9608a_lcm_drv;

//Gionee xiangzhong 2012-09-19 add for GBW136 LCM begin
extern LCM_DRIVER gn_truly_nt35516_lcm_drv;
//Gionee xiangzhong 2012-09-19 add for GBW136 LCM end

//Gionee wanght 2012-7-17 add for GBW13AA LCM begin
extern LCM_DRIVER gn_smd_db7430_lcm_drv;
//Gionee wanght 2012-7-17 add for GBW13AA LCM end


LCM_DRIVER* lcm_driver_list[] = 
{ 
#if defined(HX8369)
	&hx8369_lcm_drv,
#endif

#if defined(GN_TRULY_NT35510)
	&gn_truly_nt35510_lcm_drv,
#endif

#if defined(HX8369_6575)
	&hx8369_6575_lcm_drv,
#endif

#if defined(BM8578)
	&bm8578_lcm_drv,
#endif

#if defined(NT35582_MCU)
	&nt35582_mcu_lcm_drv,
#endif

#if defined(NT35582_MCU_6575)
	&nt35582_mcu_6575_lcm_drv,
#endif

#if defined(NT35582_RGB_6575)
	&nt35582_rgb_6575_lcm_drv,
#endif

#if defined(HX8357B)
	&hx8357b_lcm_drv,
#endif

#if defined(R61408)
	&r61408_lcm_drv,
#endif

#if defined(HX8369_DSI_VDO)
	&hx8369_dsi_vdo_lcm_drv,
#endif

#if defined(HX8369_DSI)
	&hx8369_dsi_lcm_drv,
#endif

#if defined(HX8369_6575_DSI)
	&hx8369_dsi_6575_lcm_drv,
#endif

#if defined(HX8369_6575_DSI_HVGA)
	&hx8369_dsi_6575_hvga_lcm_drv,
#endif

#if defined(HX8369_6575_DSI_QVGA)
	&hx8369_dsi_6575_qvga_lcm_drv,
#endif

#if defined(HX8369_HVGA)
	&hx8369_hvga_lcm_drv,
#endif

#if defined(NT35510)
	&nt35510_lcm_drv,
#endif

#if defined(NT35510_HVGA)
	&nt35510_hvga_lcm_drv,
#endif

#if defined(NT35510_QVGA)
	&nt35510_qvga_lcm_drv,
#endif

#if defined(NT35510_6517)
	&nt35510_6517_lcm_drv,
#endif

#if defined(ILI9481)
	&ili9481_lcm_drv,
#endif

#if defined(NT35582)
	&nt35582_lcm_drv,
#endif

#if defined(S6D0170)
	&s6d0170_lcm_drv,
#endif

#if defined(SPFD5461A)
	&spfd5461a_lcm_drv,
#endif

#if defined(TA7601)
	&ta7601_lcm_drv,
#endif

#if defined(TFT1P3037)
	&tft1p3037_lcm_drv,
#endif

#if defined(HA5266)
	&ha5266_lcm_drv,
#endif

#if defined(HSD070IDW1)
	&hsd070idw1_lcm_drv,
#endif

#if defined(HX8363_6575_DSI)
	&hx8363_6575_dsi_lcm_drv,
#endif

//add by chendong 2012-05-13
#if defined(GN_TCL_HX8369) //Qux
	&gn_hx8369_dsi_lcm_drv,
#endif
//add by chendong 2012-05-13 end
#if defined(HX8363_6575_DSI_HVGA)
	&hx8363_6575_dsi_hvga_lcm_drv,
#endif

#if defined(LG4571)
	&lg4571_lcm_drv,
#endif

#if defined(LVDS_WSVGA)
	&lvds_wsvga_lcm_drv,
#endif

#if defined(LVDS_WSVGA_TI)
	&lvds_wsvga_ti_lcm_drv,
#endif

#if defined(LVDS_WSVGA_TI_N)
	&lvds_wsvga_ti_n_lcm_drv,
#endif

#if defined(NT35565_3D)
	&nt35565_3d_lcm_drv,
#endif

#if defined(TM070DDH03)
	&tm070ddh03_lcm_drv,
#endif
//gionee chenqiang add for lcm_add 20120113 begin
#if defined (GN_MTK_BSP)
#if defined(GN_SAMSUNG_LD9042)||defined(GN_SAMSUNG_LD9042_BL_GPIO)
       &gn_samsung_ld9042_lcm_drv,
#endif 
#endif
//gionee chenqiang add for lcm_add 20120113 end

//Gionee changph 20120417 add for GBW126A begin 
#if defined(GN_SMD_LMS397KF04)
	&gn_smd_lms397kf04_lcm_drv,
#endif
//Gionee changph 20120417 add for GBW126A end

//Gionee pengxw 20120418 add for GBW126 start
#if defined(GN_TRULY_R61408)
    &gn_truly_r61408_lcm_drv,
#endif
#if defined(R63303_IDISPLAY)
	&r63303_idisplay_lcm_drv,
#endif

#if defined(HX8369B_DSI_VDO)
	&hx8369b_dsi_vdo_lcm_drv,
#endif
#if defined(GN_BYD_R61408)
    &gn_byd_r61408_lcm_drv,
#endif
//Gionee pengxw 20120418 add for GBW126 end

//gionee pengchao 20120428 add for GBW131 LCM start
#if defined(GN_SSD2825_SMD_S6E8AA)
	&gn_ssd2825_smd_s6e8aa,
#endif
//gionee pengchao 20120428 add for GBW131 LCM end

//Gionee Qux add for wbw6803, 20120521
#if defined(GN_BYD_HX8369)
    &gn_byd_hx8369_lcm_drv,
#endif
#if defined(GN_TIANMA_HX8369)
    &gn_tianma_hx8369_lcm_drv,
#endif
#if defined(GN_TRULY_HX8369)
    &gn_truly_hx8369_lcm_drv,
#endif
//Gionee Qux. END

#if defined(GN_TRULY_NT35510)
	&gn_truly_nt35510_lcm_drv,
#endif

#if defined(GN_BYD_NT35510)
	&gn_byd_nt35510_lcm_drv,
#endif

#if defined(GN_SMD_R69329)
	&gn_smd_r69329_lcm_drv,
#endif	

//Gionee changph 20120802 add
#if defined(GN_BYD_OTM9608A)
	&gn_byd_otm9608a_lcm_drv,
#endif
//Gionee wanght 2012-7-17 add for GBW13AA LCM begin
#if defined(GN_SMD_DB7430)
	&gn_smd_db7430_lcm_drv,
#endif
//Gionee wanght  2012-7-17 add for GBW13AA LCM end
//Gionee xiangzhong 2012-09-19 add for GBW136 LCD begin
#if defined(GN_TRULY_NT35516)
	&gn_truly_nt35516_lcm_drv,
#endif
//Gionee xiangzhong 2012-09-19 add for GBW136 LCD end

};

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition)?1:-1]

unsigned int lcm_count = sizeof(lcm_driver_list)/sizeof(LCM_DRIVER*);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list)/sizeof(LCM_DRIVER*));
