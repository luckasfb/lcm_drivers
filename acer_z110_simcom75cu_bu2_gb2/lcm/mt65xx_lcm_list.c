#include <lcm_drv.h>
extern LCM_DRIVER hx8369_lcm_drv;
extern LCM_DRIVER hx8369_6575_lcm_drv;
extern LCM_DRIVER hx8363_6575_dsi_lcm_drv;
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
extern LCM_DRIVER zte6516_lcm_drv;
extern LCM_DRIVER ha5266_lcm_drv;
extern LCM_DRIVER hsd070idw1_lcm_drv;
extern LCM_DRIVER lg4571_lcm_drv;
extern LCM_DRIVER lvds_wsvga_lcm_drv;
extern LCM_DRIVER lvds_wsvga_ti_lcm_drv;
extern LCM_DRIVER nt35565_3d_lcm_drv;
extern LCM_DRIVER sr4009gg_lg4573_lcm_drv;
extern LCM_DRIVER sr4009tn_nt35510_lcm_drv;
extern LCM_DRIVER sr4009dn_nt35510_lcm_drv;
extern LCM_DRIVER n616byd_nt35510_lcm_drv;
extern LCM_DRIVER t040kwv048at_hx8369_lcm_drv;//amy0418 
extern LCM_DRIVER ut40022a0_hx8369_lcm_drv;//amy0604 
extern LCM_DRIVER ll4011_wvga_otm8009a_lcm_drv;//amy0524  
extern LCM_DRIVER yl24001nt_hx8369a_lcm_drv;
extern LCM_DRIVER tft3p2380_nt35510_lcm_drv;   //amy0320 
extern LCM_DRIVER blg9009wv0_r61408_lcm_drv;
extern LCM_DRIVER srl4009dn_nt35510_lcm_drv; //amy0502
extern LCM_DRIVER h40d07_nt35510_lcm_drv;   //amy0503

extern LCM_DRIVER z50002n50g3h_wvga_hx8363a_lcm_drv; 
extern LCM_DRIVER qtb4d0058_wvga_nt35510_lcm_drv; 
extern LCM_DRIVER kd50m4_wvga_otm8009a_lcm_drv; 
extern LCM_DRIVER yt50f23c0_wvga_ili7800_lcm_drv;
extern LCM_DRIVER n618_wvga_nt35510_lcm_drv;
extern LCM_DRIVER nt35516_lcm_drv; //punk
extern LCM_DRIVER p2617_wvga_r61408_lcm_drv;
extern LCM_DRIVER dj3065a_hx8369a_lcm_drv;//LK@AUX_V900
extern LCM_DRIVER lt050anyb01d_wvga_nt35582_lcm_drv;
extern LCM_DRIVER tft050a001_wvga_nt35510_lcm_drv;
extern LCM_DRIVER n3p4527_wvga_nt35510_lcm_drv;
extern LCM_DRIVER sr3545ci_hvga_ili9486_lcm_drv;   //yanfang add
extern LCM_DRIVER sr3579ti_hvga_ili9486_lcm_drv;   //yanfang add
extern LCM_DRIVER kd50m14_wvga_nt35510_lcm_drv;   
extern LCM_DRIVER srl5002hn_wvga_nt35510_lcm_drv;  
extern LCM_DRIVER p2498_wvga_nt35510_lcm_drv; 
extern LCM_DRIVER ad397lcm_wvga_hx8369a_lcm_drv;
extern LCM_DRIVER lm050amybo1j_wvga_rm68120_lcm_drv;
extern LCM_DRIVER bm347hv_hvga_r61581b_lcm_drv;//lifei
extern LCM_DRIVER dp0622_wvga_otm8009a_lcm_drv;//lifei

extern LCM_DRIVER sr3579ti_hvga_ili9486_z1_lcm_drv;
extern LCM_DRIVER by35hp_hvga_hx8357_lcm_drv;
extern LCM_DRIVER jmo4861_hvga_hx8357_lcm_drv;

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

#if defined(ZTE6516)
	&zte6516_lcm_drv,
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

#if defined(SR4009TN_WVGA_NT35510)
	&sr4009tn_nt35510_lcm_drv,
#endif

#if defined(SR4009GG_WVGA_LG4573)
	&sr4009gg_lg4573_lcm_drv,
#endif


#if defined(SR4009DN_WVGA_NT35510)
	&sr4009dn_nt35510_lcm_drv,
#endif

#if defined(N616BYD_WVGA_NT35510)
	&n616byd_nt35510_lcm_drv,
#endif

#if defined(T040KWV048AT_WVGA_HX8369)  //amy0418   t040kwv048at_wvga_HX8369
       &t040kwv048at_hx8369_lcm_drv,
#endif

#if defined(UT40022A0_WVGA_HX8369)  //amy0604    t040kwv048at_wvga_HX8369
       &ut40022a0_hx8369_lcm_drv,
#endif


#if defined(LL4011_WVGA_OTM8009A)  //amy0524    
       &ll4011_wvga_otm8009a_lcm_drv,
#endif

#if defined(YL24001NT_WVGA_HX8369A)
	&yl24001nt_hx8369a_lcm_drv,
#endif

#if defined(BLG9009WV0_WVGA_R61408)
	&blg9009wv0_r61408_lcm_drv,
#endif



#if defined(Z50002N50G3H_WVGA_HX8363A) //chailu
	&z50002n50g3h_wvga_hx8363a_lcm_drv,
#endif

#if defined(QTB4D0058_WVGA_NT35510) //chailu
	&qtb4d0058_wvga_nt35510_lcm_drv,
#endif

#if defined(KD50M4_WVGA_OTM8009A) //chailu
	&kd50m4_wvga_otm8009a_lcm_drv,
#endif
#if defined(N3P4527_WVGA_NT35510)
	&n3p4527_wvga_nt35510_lcm_drv,
#endif
#if defined(YT50F23C0_WVGA_ILI7800) 
	&yt50f23c0_wvga_ili7800_lcm_drv,
#endif

#if defined(N618_WVGA_NT35510) 
	&n618_wvga_nt35510_lcm_drv,
#endif

#if defined(TFT3P2380_WVGA_NT35510)  //amy0320 
	&tft3p2380_nt35510_lcm_drv,
#endif

#if defined(SRL4009DN_WVGA_NT35510)  //amy0502 
	&srl4009dn_nt35510_lcm_drv,
#endif

#if defined(H40D07_WVGA_NT35510)  //amy0503
	&h40d07_nt35510_lcm_drv,
#endif

#if defined(NT35516) //punk
	&nt35516_lcm_drv,
#endif
#if defined(P2617_WVGA_R61408)
	&p2617_wvga_r61408_lcm_drv,
#endif

#if defined(DJ3065A_WVGA_HX8369A)
	&dj3065a_hx8369a_lcm_drv,
#endif

#if defined(LT050ANYB01D_WVGA_NT35582) 
	&lt050anyb01d_wvga_nt35582_lcm_drv,
#endif

#if defined(TFT050A001_WVGA_NT35510) 
	&tft050a001_wvga_nt35510_lcm_drv,
#endif

#if defined(SR3579TI_HVGA_ILI9486)
	&sr3579ti_hvga_ili9486_lcm_drv,
#endif

#if defined(SR3545CI_HVGA_ILI9486)
	&sr3545ci_hvga_ili9486_lcm_drv,
#endif

#if defined(KD50M14_WVGA_NT35510)
	&kd50m14_wvga_nt35510_lcm_drv,
#endif

#if defined(SRL5002HN_WVGA_NT35510)
	&srl5002hn_wvga_nt35510_lcm_drv,
#endif

#if defined(P2498_WVGA_NT35510)
	&p2498_wvga_nt35510_lcm_drv,
#endif

#if defined(AD397LCM_WVGA_HX8369A)
	&ad397lcm_wvga_hx8369a_lcm_drv,
#endif

#if defined(LM050AMYBO1J_WVGA_RM68120)
	&lm050amybo1j_wvga_rm68120_lcm_drv,
#endif
#if defined(DP0622_WVGA_OTM8009A)
	&dp0622_wvga_otm8009a_lcm_drv,
#endif
	
#if defined(BM347HV_HVGA_R61581B)
	&bm347hv_hvga_r61581b_lcm_drv,
#endif	

#if defined(SINQ3545P002TC_ILI9486)
	&sinq3545p002tc_ili9486_lcm_drv,
#endif	

#if defined(SR3579TI_HVGA_ILI9486_Z1)
	&sr3579ti_hvga_ili9486_z1_lcm_drv,
#endif
	
#if defined(BY35HP35T2_HVGA_HX8357)
	&by35hp_hvga_hx8357_lcm_drv,
#endif	

#if defined(JMO4861_HVGA_HX8357)
	&jmo4861_hvga_hx8357_lcm_drv,
#endif	
};

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition)?1:-1]

unsigned int lcm_count = sizeof(lcm_driver_list)/sizeof(LCM_DRIVER*);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list)/sizeof(LCM_DRIVER*));
