#include <lcm_drv.h>
extern LCM_DRIVER hx8369_lcm_drv;
extern LCM_DRIVER hx8369_6575_lcm_drv;
extern LCM_DRIVER hx8363_6575_dsi_lcm_drv;
extern LCM_DRIVER hx8363_6575_dsi_hvga_lcm_drv;
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
extern LCM_DRIVER nt35565_3d_lcm_drv;
extern LCM_DRIVER tm070ddh03_lcm_drv;
extern LCM_DRIVER r61408_lcm_drv;
extern LCM_DRIVER qtb4d0058_wvga_nt35510_lcm_drv; 
extern LCM_DRIVER kd50m4_wvga_otm8009a_lcm_drv;  //amy0504 
extern LCM_DRIVER srl5002hn_wvga_nt35510_lcm_drv;  //amy0504 
extern LCM_DRIVER yt50f23c0_wvga_ili7800_lcm_drv;
#if defined(N618_WVGA_NT35510)//tengdeqiang 
extern LCM_DRIVER n618_wvga_nt35510_lcm_drv;
#endif
extern LCM_DRIVER nt35516_lcm_drv;
extern LCM_DRIVER tr3p2497_fwvga_hx8363b_lcm_drv;
#if defined(IPS3P2620_QHD_NT35516)
extern LCM_DRIVER ips3p2620_qhd_nt35516_lcm_drv;
#endif
#if defined(SRL4317GN_QHD_NT35516)
extern LCM_DRIVER srl4317gn_qhd_nt35516_lcm_drv;
#endif
#if defined(P2498_WVGA_NT35510)
extern LCM_DRIVER p2498_wvga_nt35510_lcm_drv; 
#endif

extern LCM_DRIVER kd50m14_wvga_nt35510_lcm_drv;   
extern LCM_DRIVER n3p4527_wvga_nt35510_lcm_drv;

#if defined(BLG9056B_WVGA_OTM8018B)//tengdeqiang 
extern LCM_DRIVER blg9056b_wvga_otm8018b_lcm_drv;
#endif
extern LCM_DRIVER srl5002h_wvga_otm8009a_lcm_drv;  //amy0518 
extern LCM_DRIVER sr4009gg_lg4573_lcm_drv;
extern LCM_DRIVER sr4009tn_nt35510_lcm_drv;
extern LCM_DRIVER yt40f51x6_wvga_ili9805_dsi_lcm_drv;
extern LCM_DRIVER dj3065a_hx8369a_lcm_drv;//LK@AUX_V900
extern LCM_DRIVER tft3p2380_nt35510_lcm_drv;   //amy0604
extern LCM_DRIVER h40d07_nt35510_lcm_drv;   //amy0604
extern LCM_DRIVER lm050amybo1j_wvga_rm68120_lcm_drv;
extern LCM_DRIVER tft3p2710_fwvga_otm8018b_lcm_drv;


LCM_DRIVER* lcm_driver_list[] = 
{ 
#if defined(HX8369)
	&hx8369_lcm_drv,
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

#if defined(HX8363_6575_DSI_HVGA)
	&hx8363_6575_dsi_hvga_lcm_drv,
#endif

#if defined(HX8363_6575_DSI)
	&hx8363_6575_dsi_lcm_drv,
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

#if defined(NT35565_3D)
	&nt35565_3d_lcm_drv,
#endif

#if defined(TM070DDH03)
	&tm070ddh03_lcm_drv,
#endif

#if defined(N618_WVGA_NT35510)//tengdeqiang 
	&n618_wvga_nt35510_lcm_drv,
#endif

#if defined(BLG9056B_WVGA_OTM8018B)//tengdeqiang 
	&blg9056b_wvga_otm8018b_lcm_drv,
#endif

#if defined(QTB4D0058_WVGA_NT35510) //chailu
	&qtb4d0058_wvga_nt35510_lcm_drv,
#endif
#if defined(KD50M4_WVGA_OTM8009A) //chailu
	&kd50m4_wvga_otm8009a_lcm_drv,
#endif

#if defined(YT50F23C0_WVGA_ILI7800) 
	&yt50f23c0_wvga_ili7800_lcm_drv,
#endif
#if defined(NT35516)
	&nt35516_lcm_drv,
#endif
#if defined(IPS3P2620_QHD_NT35516)
	&ips3p2620_qhd_nt35516_lcm_drv,
#endif
#if defined(SRL4317GN_QHD_NT35516)
	&srl4317gn_qhd_nt35516_lcm_drv,
#endif
#if defined(KD50M14_WVGA_NT35510)
	&kd50m14_wvga_nt35510_lcm_drv,
#endif
#if defined(SRL5002HN_WVGA_NT35510)
	&srl5002hn_wvga_nt35510_lcm_drv,
#endif
#if defined(TR3P2497_FWVGA_HX8363B)
	&tr3p2497_fwvga_hx8363b_lcm_drv,
#endif
#if defined(P2498_WVGA_NT35510)
	&p2498_wvga_nt35510_lcm_drv,
#endif
#if defined(N3P4527_WVGA_NT35510)
	&n3p4527_wvga_nt35510_lcm_drv,
#endif
#if defined(SRL5002H_WVGA_OTM8009A) //amy0518 
	&srl5002h_wvga_otm8009a_lcm_drv,
#endif
#if defined(YT40F51X6_WVGA_ILI9805_DSI)
	&yt40f51x6_wvga_ili9805_dsi_lcm_drv
#endif
#if defined(SR4009TN_WVGA_NT35510)
	&sr4009tn_nt35510_lcm_drv,
#endif
#if defined(SR4009GG_WVGA_LG4573)
	&sr4009gg_lg4573_lcm_drv,
#endif
#if defined(DJ3065A_WVGA_HX8369A)
	&dj3065a_hx8369a_lcm_drv,
#endif
#if defined(TFT3P2380_WVGA_NT35510)  //amy0604 
	&tft3p2380_nt35510_lcm_drv,
#endif
#if defined(H40D07_WVGA_NT35510)  //amy0604
	&h40d07_nt35510_lcm_drv,
#endif
#if defined(LM050AMYBO1J_WVGA_RM68120)
	&lm050amybo1j_wvga_rm68120_lcm_drv,
#endif
#if defined(TFT3P2710_FWVGA_OTM8018B)
	&tft3p2710_fwvga_otm8018b_lcm_drv,
#endif
};

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition)?1:-1]

unsigned int lcm_count = sizeof(lcm_driver_list)/sizeof(LCM_DRIVER*);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list)/sizeof(LCM_DRIVER*));
