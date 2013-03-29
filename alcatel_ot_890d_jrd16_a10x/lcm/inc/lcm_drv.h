

#ifndef __LCM_DRV_H__
#define __LCM_DRV_H__


// ---------------------------------------------------------------------------

#ifndef ARY_SIZE
#define ARY_SIZE(x) (sizeof((x)) / sizeof((x[0])))
#endif

// ---------------------------------------------------------------------------

/* common enumerations */

typedef enum
{
    LCM_TYPE_DBI = 0,
    LCM_TYPE_DPI,
    LCM_TYPE_DSI
} LCM_TYPE;


typedef enum
{
    LCM_CTRL_NONE = 0,
    LCM_CTRL_SERIAL_DBI,
    LCM_CTRL_PARALLEL_DBI,
    LCM_CTRL_GPIO
} LCM_CTRL;


typedef enum
{
    LCM_POLARITY_RISING  = 0,
    LCM_POLARITY_FALLING = 1
} LCM_POLARITY;


typedef enum
{
    LCM_CLOCK_PHASE_0  = 0,
    LCM_CLOCK_PHASE_90 = 1
} LCM_CLOCK_PHASE;


typedef enum
{
    LCM_COLOR_ORDER_RGB = 0,
    LCM_COLOR_ORDER_BGR = 1
} LCM_COLOR_ORDER;


typedef enum
{    
   LCM_DRIVING_CURRENT_8MA = 0,
   LCM_DRIVING_CURRENT_4MA,
   LCM_DRIVING_CURRENT_2MA,
   LCM_DRIVING_CURRENT_SLEW_CNTL
} LCM_DRIVING_CURRENT;


/* DBI related enumerations */

typedef enum
{
    LCM_DBI_CLOCK_FREQ_104M = 0,
    LCM_DBI_CLOCK_FREQ_52M,
    LCM_DBI_CLOCK_FREQ_26M,
    LCM_DBI_CLOCK_FREQ_13M,
    LCM_DBI_CLOCK_FREQ_7M
} LCM_DBI_CLOCK_FREQ;


typedef enum
{
    LCM_DBI_DATA_WIDTH_8BITS  = 0,
    LCM_DBI_DATA_WIDTH_9BITS  = 1,
    LCM_DBI_DATA_WIDTH_16BITS = 2,
    LCM_DBI_DATA_WIDTH_18BITS = 3,
    LCM_DBI_DATA_WIDTH_24BITS = 4
} LCM_DBI_DATA_WIDTH;


typedef enum
{
    LCM_DBI_CPU_WRITE_8_BITS  = 8,
    LCM_DBI_CPU_WRITE_16_BITS = 16,
    LCM_DBI_CPU_WRITE_32_BITS = 32,
} LCM_DBI_CPU_WRITE_BITS;


typedef enum
{
    LCM_DBI_FORMAT_RGB332 = 0,
    LCM_DBI_FORMAT_RGB444 = 1,
    LCM_DBI_FORMAT_RGB565 = 2,
    LCM_DBI_FORMAT_RGB666 = 3,
    LCM_DBI_FORMAT_RGB888 = 4
} LCM_DBI_FORMAT;


typedef enum
{
    LCM_DBI_TRANS_SEQ_MSB_FIRST = 0,
    LCM_DBI_TRANS_SEQ_LSB_FIRST = 1
} LCM_DBI_TRANS_SEQ;


typedef enum
{
    LCM_DBI_PADDING_ON_LSB = 0,
    LCM_DBI_PADDING_ON_MSB = 1
} LCM_DBI_PADDING;


typedef enum
{
    LCM_DBI_TE_MODE_DISABLED       = 0,
    LCM_DBI_TE_MODE_VSYNC_ONLY     = 1,
    LCM_DBI_TE_MODE_VSYNC_OR_HSYNC = 2,
} LCM_DBI_TE_MODE;


typedef enum
{
    LCM_DBI_TE_VS_WIDTH_CNT_DIV_8  = 0,
    LCM_DBI_TE_VS_WIDTH_CNT_DIV_16 = 1,
    LCM_DBI_TE_VS_WIDTH_CNT_DIV_32 = 2,
    LCM_DBI_TE_VS_WIDTH_CNT_DIV_64 = 3,
} LCM_DBI_TE_VS_WIDTH_CNT_DIV;


/* DPI related enumerations */

typedef enum
{
    LCM_DPI_FORMAT_RGB565 = 0,
    LCM_DPI_FORMAT_RGB666 = 1,
    LCM_DPI_FORMAT_RGB888 = 2
} LCM_DPI_FORMAT;


// ---------------------------------------------------------------------------

typedef struct 
{
    LCM_COLOR_ORDER color_order;
    LCM_DBI_TRANS_SEQ trans_seq;
    LCM_DBI_PADDING padding;
    LCM_DBI_FORMAT format;
    LCM_DBI_DATA_WIDTH width;
} LCM_DBI_DATA_FORMAT;


typedef struct
{
    LCM_POLARITY cs_polarity;
    LCM_POLARITY clk_polarity;
    LCM_CLOCK_PHASE clk_phase;
    unsigned int is_non_dbi_mode;
} LCM_DBI_SERIAL_PARAMS;


typedef struct
{
    /* timing parameters */
    unsigned int write_setup;
    unsigned int write_hold;
    unsigned int write_wait;
    unsigned int read_setup;
    unsigned int read_latency;
    unsigned int wait_period;
} LCM_DBI_PARALLEL_PARAMS;


// ---------------------------------------------------------------------------

typedef struct
{
    /* common parameters for serial & parallel interface */
    unsigned int port;
    LCM_DBI_CLOCK_FREQ     clock_freq;
    LCM_DBI_DATA_WIDTH     data_width; 
    LCM_DBI_DATA_FORMAT    data_format;
    LCM_DBI_CPU_WRITE_BITS cpu_write_bits;
    LCM_DRIVING_CURRENT    io_driving_current;
    
    /* tearing control */
    LCM_DBI_TE_MODE              te_mode;
    LCM_POLARITY                 te_edge_polarity;
    unsigned int                 te_hs_delay_cnt;
    unsigned int                 te_vs_width_cnt;
    LCM_DBI_TE_VS_WIDTH_CNT_DIV  te_vs_width_cnt_div;
    
    /* particular parameters for serial & parallel interface */
    union {
        LCM_DBI_SERIAL_PARAMS serial;
        LCM_DBI_PARALLEL_PARAMS parallel;
    };
} LCM_DBI_PARAMS;


typedef struct
{
    /* 
        Pixel Clock Frequency = 26MHz * mipi_pll_clk_div1
                                      / (mipi_pll_clk_ref + 1) 
                                      / (2 * mipi_pll_clk_div2)
                                      / dpi_clk_div
    */
    unsigned int mipi_pll_clk_ref;   // 0..1
    unsigned int mipi_pll_clk_div1;  // 0..63
    unsigned int mipi_pll_clk_div2;  // 0..15
    unsigned int dpi_clk_div;        // 2..32

    unsigned int dpi_clk_duty;       // (dpi_clk_div - 1) .. 31

    /* polarity parameters */
    LCM_POLARITY clk_pol;
    LCM_POLARITY de_pol;
    LCM_POLARITY vsync_pol;
    LCM_POLARITY hsync_pol;

    /* timing parameters */
    unsigned int hsync_pulse_width;
    unsigned int hsync_back_porch;
    unsigned int hsync_front_porch;
    unsigned int vsync_pulse_width;
    unsigned int vsync_back_porch;
    unsigned int vsync_front_porch;
    
    /* output format parameters */
    LCM_DPI_FORMAT format;
    LCM_COLOR_ORDER rgb_order;
    unsigned int is_serial_output;

    /* intermediate buffers parameters */
    unsigned int intermediat_buffer_num; // 2..3

    /* iopad parameters */
    LCM_DRIVING_CURRENT io_driving_current;
    
} LCM_DPI_PARAMS;


// ---------------------------------------------------------------------------

typedef struct
{
    LCM_TYPE type;
    LCM_CTRL ctrl;  //! how to control LCM registers

    /* common parameters */
    unsigned int width;
    unsigned int height;

    /* particular parameters */
    LCM_DBI_PARAMS dbi;
    LCM_DPI_PARAMS dpi;
} LCM_PARAMS;


// ---------------------------------------------------------------------------

typedef struct
{
    void (*set_reset_pin)(unsigned int value);
    int  (*set_gpio_out)(unsigned int gpio, unsigned int value);

    void (*udelay)(unsigned int us);
    void (*mdelay)(unsigned int ms);

    void (*send_cmd)(unsigned int cmd);
    void (*send_data)(unsigned int data);
    unsigned int (*read_data)(void);

    void (*wait_transfer_done)(void);

    /** FIXME: GPIO mode should not be configured in lcm driver
               REMOVE ME after GPIO customization is done    
    */
    int (*set_gpio_mode)(unsigned int pin, unsigned int mode);
    int (*set_gpio_dir)(unsigned int pin, unsigned int dir);
    int (*set_gpio_pull_enable)(unsigned int pin, unsigned char pull_en);
    
} LCM_UTIL_FUNCS;


typedef struct
{
    void (*set_util_funcs)(const LCM_UTIL_FUNCS *util);
    void (*get_params)(LCM_PARAMS *params);

    void (*init)(void);    
    void (*suspend)(void);
    void (*resume)(void);

    void (*update)(unsigned int x, unsigned int y,
                   unsigned int width, unsigned int height);
} LCM_DRIVER;


// ---------------------------------------------------------------------------
//  LCM Driver Functions
// ---------------------------------------------------------------------------
const unsigned int lcm_id_detect(void);
int lcm_module_init(void);
const LCM_DRIVER* LCM_GetDriver(void);
const LCM_DRIVER* LCM_GetJrdDriver(void);

#endif // __LCM_DRV_H__
