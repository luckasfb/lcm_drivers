
#include <linux/string.h>
#ifdef BUILD_UBOOT
#include <asm/arch/mt6575_gpio.h>
#else
#include <linux/delay.h>
#include <mach/mt6575_pm_ldo.h>
#include <mach/mt6575_gpio.h>
#endif
#include "lcm_drv.h"

static int lcdc_debug_mask = 1;

#ifdef BUILD_UBOOT
#define lcdc_debug(format,...) \
	do { \
		if (lcdc_debug_mask) \
			printf(format,__VA_ARGS__); \
	} while (0)
#else
#define lcdc_debug(format,...) \
	do { \
		if (lcdc_debug_mask) \
			printk(format,__VA_ARGS__); \
	} while (0)
#endif


static LCM_UTIL_FUNCS lcm_util = {0};

#define FRAME_WIDTH  (800)
#define FRAME_HEIGHT (480)

#define BACKLIGHT_ENABLE_PIN GPIO68
#define LCM_RESET_PIN GPIO18

static int power_on = 0;

static int lcm_config_rgb()
{
	//red
	mt_set_gpio_mode(GPIO26, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO32, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO22, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO27, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO40, GPIO_MODE_01);

	//green
	mt_set_gpio_mode(GPIO34, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO38, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO46, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO39, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO28, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO33, GPIO_MODE_01);

	//blue
	mt_set_gpio_mode(GPIO42, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO20, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO24, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO29, GPIO_MODE_01);
	mt_set_gpio_mode(GPIO31, GPIO_MODE_01);

	//DPIDE
	mt_set_gpio_mode(GPIO30, GPIO_MODE_01);

	//DPICK
	mt_set_gpio_mode(GPIO41, GPIO_MODE_01);

	//DPIVSYNC
	mt_set_gpio_mode(GPIO21, GPIO_MODE_01);

	//DPIHSYNC
	mt_set_gpio_mode(GPIO19, GPIO_MODE_01);

}

static int lcm_backlight_power(int on)
{
	if(on) {
		mt_set_gpio_mode(BACKLIGHT_ENABLE_PIN, GPIO_MODE_GPIO);
		mt_set_gpio_dir(BACKLIGHT_ENABLE_PIN, GPIO_DIR_OUT);
	#ifdef BUILD_UBOOT
		mt_set_gpio_out(BACKLIGHT_ENABLE_PIN, 1);
	#endif
	}
	else {
		mt_set_gpio_mode(BACKLIGHT_ENABLE_PIN, GPIO_MODE_GPIO);
		mt_set_gpio_dir(BACKLIGHT_ENABLE_PIN, GPIO_DIR_OUT);
	#ifdef BUILD_UBOOT
		mt_set_gpio_out(BACKLIGHT_ENABLE_PIN, 0);
	#endif
	}
	return 0;
}

static int lcm_power(int on)
{
	if(on) {
	#ifdef BUILD_UBOOT
		pmic_config_interface(0xAF, 0x2, 0x7, 0x4);
		pmic_config_interface(0xAF, 0x1, 0x1, 0);
	#else
		hwPowerOn(MT65XX_POWER_LDO_VGP, VOL_1800, "lcm");	
	#endif
	}
	else {
	#ifdef BUILD_UBOOT
		pmic_config_interface(0xAF, 0, 0x1, 0);
	#else
		hwPowerDown(MT65XX_POWER_LDO_VGP, "lcm");
	#endif
	}
	return 0;
}

static int lcm_reset(void)
{
	mt_set_gpio_mode(LCM_RESET_PIN, GPIO_MODE_GPIO);
	mt_set_gpio_dir(LCM_RESET_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(LCM_RESET_PIN, 1);
	mdelay(1);
	mt_set_gpio_out(LCM_RESET_PIN, 0);
	mdelay(1);
	mt_set_gpio_out(LCM_RESET_PIN, 1);
	return 0;
}

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	lcdc_debug("%s: --Liu\n", __func__);	
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
	lcdc_debug("%s: --Liu\n", __func__);
	params->type   = LCM_TYPE_DPI;
	params->ctrl   = LCM_CTRL_SERIAL_DBI; //?
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	params->io_select_mode = 0;	
	/* RGB interface configurations */
	//the most important parameters: set pll clk to 66Mhz and dpi clk to 33Mhz
	params->dpi.mipi_pll_clk_ref  = 0;      
	params->dpi.mipi_pll_clk_div1 = 31;
	params->dpi.mipi_pll_clk_div2 = 4;
	params->dpi.dpi_clk_div       = 2;
	params->dpi.dpi_clk_duty      = 1;
	params->dpi.clk_pol           = LCM_POLARITY_RISING;
	params->dpi.de_pol            = LCM_POLARITY_RISING;
	params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
	params->dpi.hsync_pol         = LCM_POLARITY_FALLING;
	params->dpi.hsync_pulse_width = 48;
	params->dpi.hsync_back_porch  = 40;
	params->dpi.hsync_front_porch = 40;
	params->dpi.vsync_pulse_width = 1;
	params->dpi.vsync_back_porch  = 31;
	params->dpi.vsync_front_porch = 13;
	params->dpi.format            = LCM_DPI_FORMAT_RGB565; 
	params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
	params->dpi.is_serial_output  = 0;
	params->dpi.intermediat_buffer_num = 2;
	//LCM_DRIVING_CURRENT_8MA | LCM_DRIVING_CURRENT_4MA | LCM_DRIVING_CURRENT_2MA;
	params->dpi.io_driving_current = LCM_DRIVING_CURRENT_2MA;
}

static void lcm_init(void)
{
	lcdc_debug("%s: --Liu\n", __func__);
	lcm_config_rgb();
#ifdef BUILD_UBOOT
	lcm_power(1);
#else	
	//hwPowerOn(MT65XX_POWER_LDO_VGP, VOL_1800, "bl");
#endif	
}

static void lcm_suspend(void)
{
	lcdc_debug("%s: --Liu\n", __func__);
	lcm_backlight_power(0);
	lcm_power(0);
	
}

static void lcm_resume(void)
{
	lcdc_debug("%s: --Liu\n", __func__);	
	lcm_config_rgb();
	lcm_power(1);
	lcm_reset();
	lcm_backlight_power(1);

}

int lcm_power_test(int on)
{
	lcm_config_rgb();
	if(on){
		lcm_power(1);
		lcm_reset();
		lcm_backlight_power(1);
	}
	else{
		lcm_backlight_power(0);
		lcm_power(0);
	}
}

static void lcm_update(unsigned int x, unsigned int y,
		unsigned int width, unsigned int height)
{
	//lcdc_debug("%s: --Liu\n", __func__);
	if(!power_on) {
		lcm_power(1);
		lcm_reset();
		lcm_backlight_power(1);	
		power_on = 1;
	}
}

static void lcm_setbacklight(unsigned int level)
{
	lcdc_debug("%s: --Liu\n", __func__);
}

static unsigned int lcm_compare_id(void)
{	
	lcdc_debug("%s: --Liu\n", __func__);
	return 0;
}

LCM_DRIVER lcdc_s9050_lcm_drv = 
{
	.name			= "lcdc_s9050",
	.set_util_funcs	= lcm_set_util_funcs,
	.get_params		= lcm_get_params,
	.init				= lcm_init,
	.suspend			= lcm_suspend,
	.resume			= lcm_resume,
	.update			= lcm_update,
	.compare_id		= lcm_compare_id,
	.set_backlight		= lcm_setbacklight,
	.set_pwm		= NULL,
	.get_pwm		= NULL,
	.esd_check		= NULL,
	.esd_recover		= NULL
};

