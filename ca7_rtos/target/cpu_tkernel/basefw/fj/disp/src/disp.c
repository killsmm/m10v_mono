/**
 * @file		fj_disp.c
 * @brief		LCD, HDMI, Image and OSD control
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "disp.h"
#include "dd_arm.h"
#include "dd_hdmac0.h"
#include "debug.h"
#include "fj_host_debug.h"
#include "fj_still.h"
#include "hdmi.h"
#include "im_disp.h"
#include "im_hdmi.h"
#include "im_mipi.h"
#include "instance.h"
#include "l1l2cache.h"
#include "osd_attr.h"
#include "osd_config.h"
#include "osd_draw_comm.h"
#include "sdram_map_cache_define.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_DISP_OSD_COM_GRISIZE_IHSIZE_MIN (8) /* GRISIZE.IHSIZE min */
#define D_DISP_OSD_COM_GRISIZE_IVSIZE_MIN (8) /* GRISIZE.IVSIZE min */
#define D_DISP_OSD_FMT_RGBA8888 (0) /* RGBA8888(000b) for GRIDT.IFMT */
#define D_DISP_OSD_FMT_RGBA4444 (2) /* RGBA4444(010b) for GRIDT.IFMT */
#define D_DISP_OSD_PIX_RGBA8888 (4) /* RGBA8888 pixel size */
#define D_DISP_OSD_PIX_RGBA4444 (2) /* RGBA4444 pixel size */
#define D_DISP_OSD_DMA_CH (7)       /* DMA ch for OSD copy */

#define D_DISP_OSD_LAYER_MAX (2) /* OSD Layer number max */

#define D_DISP_LCD_CLK_PLL04_OUT_MIN (500000000)  /* PLL04 output Min. */
#define D_DISP_LCD_CLK_PLL04_OUT_MAX (1200000000) /* PLL04 output Max. */
#define D_DISP_LCD_CLK_REG_B_MIN (2)              /* b value(P04POSTDIV) Min. */
#define D_DISP_LCD_CLK_REG_B_MAX (256)            /* b value(P04POSTDIV) Max. */
#define D_DISP_LCD_CLK_PLL04_MATCH_DIFF (1.0f)    /* error range */
#define D_DISP_LCD_CLK_PLL04_TBL_MAX (201)        /* PLL04 serch table Num. */

#define D_DISP_HDMI_STAT_HPD (0x02)
#define D_DISP_HDMI_STAT_RX_SENSE (0xF0)
#define D_DISP_HDMI_STAT_HPD_RX_SENSE                                          \
  (D_DISP_HDMI_STAT_HPD | D_DISP_HDMI_STAT_RX_SENSE)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
/** Interrupt register */
typedef enum {
  E_DISP_HDMI_INT_HPD_ON = 0,
  E_DISP_HDMI_INT_HPD_OFF,
  E_DISP_HDMI_INT_RX_SENSE_0_ON,
  E_DISP_HDMI_INT_RX_SENSE_0_OFF
} E_DISP_HDMI_INT_REG;

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
/* Serch table for setting register value(LCDCLK) */
typedef struct {
  //	FLOAT	pll04_out;						/* PLL04 output frequency
  //*/
  DOUBLE pll04_out; /* PLL04 output frequency */
  UCHAR a;          /* a value(P04PREDIV) */
  UCHAR n;          /* N value(P04PLLDIV) */
} T_DISP_PLL04_TBL;

/* Interface for lcd_clk_reg_search() */
typedef struct {
  ULONG a; /* a value(P04PREDIV) */
  ULONG n; /* N value(P04PLLDIV) */
  ULONG b; /* b value(P04POSTDIV) */
} T_DISP_LCD_CLK_REG;

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
// Display instance.
static T_BF_INSTANCE_DISPLAY
    gBF_Instance_Display[D_BF_INSTANCE_ID_MAX_DISPLAY] = {
        {E_BF_INSTANCE_FLAG_UNUSE,
         FJ_DISP_TYPE_LCD,
         E_BF_INSTANCE_STATE_NORMAL,
         E_BF_INSTANCE_CONNECT_STATE_OFF,
         FJ_DISP_ROTATE_DEGREE_0,
         {0}},
        {E_BF_INSTANCE_FLAG_UNUSE,
         FJ_DISP_TYPE_HDMI,
         E_BF_INSTANCE_STATE_NORMAL,
         E_BF_INSTANCE_CONNECT_STATE_OFF,
         FJ_DISP_ROTATE_DEGREE_0,
         {0}}};

// Video enable/disable state
static volatile BOOL gBF_Video_enable_stat[D_BF_INSTANCE_ID_MAX_DISPLAY] = {
    FALSE,
    FALSE,
};

// HDMI callback function pointer
static volatile DISP_HDMI_CALLBACK gDisp_HDMI_Callback = NULL;

static volatile FJ_DISP_HDMI_DETECT_SETTING gDisp_HDMI_Detect_Setting;

static volatile BOOL gDisp_HDMI_Detect_State = FALSE;

static volatile FJ_DISP_HDMI_SINK_INFO *gDisp_HDMI_Sink_Info;

static volatile FJ_HDMI_DTD_MAN *gDisp_HDMI_DTD_Man;

static volatile BOOL
    gDisp_Image_Layer_Enable_Wait[D_BF_INSTANCE_ID_MAX_DISPLAY] = {FALSE,
                                                                   FALSE};

static volatile UCHAR gDisp_OSD_Degree = 0;

static ULONG gBF_Disp_Main_Spin_Lock
    __attribute__((section(".LOCK_SECTION"), aligned(64)));

static ULONG gBF_Disp_Spin_Lock
    __attribute__((section(".LOCK_SECTION"), aligned(64)));

static const T_DISP_PLL04_TBL
    g_lcd_clk_pll04_tbl[D_DISP_LCD_CLK_PLL04_TBL_MAX] = {
        // pll04_out				a	n
        {D_DD_TOP_UCLK40I / 2 * 25, 1, 24},   // 500.00MHz
        {D_DD_TOP_UCLK40I / 5 * 63, 4, 62},   // 504.00MHz
        {D_DD_TOP_UCLK40I / 3 * 38, 2, 37},   // 506.67MHz
        {D_DD_TOP_UCLK40I / 4 * 51, 3, 50},   // 510.00MHz
        {D_DD_TOP_UCLK40I / 5 * 64, 4, 63},   // 512.00MHz
        {D_DD_TOP_UCLK40I / 6 * 77, 5, 76},   // 513.33MHz
        {D_DD_TOP_UCLK40I / 1 * 13, 0, 12},   // 520.00MHz
        {D_DD_TOP_UCLK40I / 6 * 79, 5, 78},   // 526.67MHz
        {D_DD_TOP_UCLK40I / 5 * 66, 4, 65},   // 528.00MHz
        {D_DD_TOP_UCLK40I / 4 * 53, 3, 52},   // 530.00MHz
        {D_DD_TOP_UCLK40I / 3 * 40, 2, 39},   // 533.33MHz
        {D_DD_TOP_UCLK40I / 5 * 67, 4, 66},   // 536.00MHz
        {D_DD_TOP_UCLK40I / 2 * 27, 1, 26},   // 540.00MHz
        {D_DD_TOP_UCLK40I / 5 * 68, 4, 67},   // 544.00MHz
        {D_DD_TOP_UCLK40I / 3 * 41, 2, 40},   // 546.67MHz
        {D_DD_TOP_UCLK40I / 4 * 55, 3, 54},   // 550.00MHz
        {D_DD_TOP_UCLK40I / 5 * 69, 4, 68},   // 552.00MHz
        {D_DD_TOP_UCLK40I / 6 * 83, 5, 82},   // 553.33MHz
        {D_DD_TOP_UCLK40I / 1 * 14, 0, 13},   // 560.00MHz
        {D_DD_TOP_UCLK40I / 6 * 85, 5, 84},   // 566.67MHz
        {D_DD_TOP_UCLK40I / 5 * 71, 4, 70},   // 568.00MHz
        {D_DD_TOP_UCLK40I / 4 * 57, 3, 56},   // 570.00MHz
        {D_DD_TOP_UCLK40I / 3 * 43, 2, 42},   // 573.33MHz
        {D_DD_TOP_UCLK40I / 5 * 72, 4, 71},   // 576.00MHz
        {D_DD_TOP_UCLK40I / 2 * 29, 1, 28},   // 580.00MHz
        {D_DD_TOP_UCLK40I / 5 * 73, 4, 72},   // 584.00MHz
        {D_DD_TOP_UCLK40I / 3 * 44, 2, 43},   // 586.67MHz
        {D_DD_TOP_UCLK40I / 4 * 59, 3, 58},   // 590.00MHz
        {D_DD_TOP_UCLK40I / 5 * 74, 4, 73},   // 592.00MHz
        {D_DD_TOP_UCLK40I / 6 * 89, 5, 88},   // 593.33MHz
        {D_DD_TOP_UCLK40I / 1 * 15, 0, 14},   // 600.00MHz
        {D_DD_TOP_UCLK40I / 6 * 91, 5, 90},   // 606.67MHz
        {D_DD_TOP_UCLK40I / 5 * 76, 4, 75},   // 608.00MHz
        {D_DD_TOP_UCLK40I / 4 * 61, 3, 60},   // 610.00MHz
        {D_DD_TOP_UCLK40I / 3 * 46, 2, 45},   // 613.33MHz
        {D_DD_TOP_UCLK40I / 5 * 77, 4, 76},   // 616.00MHz
        {D_DD_TOP_UCLK40I / 2 * 31, 1, 30},   // 620.00MHz
        {D_DD_TOP_UCLK40I / 5 * 78, 4, 77},   // 624.00MHz
        {D_DD_TOP_UCLK40I / 3 * 47, 2, 46},   // 626.67MHz
        {D_DD_TOP_UCLK40I / 4 * 63, 3, 62},   // 630.00MHz
        {D_DD_TOP_UCLK40I / 5 * 79, 4, 78},   // 632.00MHz
        {D_DD_TOP_UCLK40I / 6 * 95, 5, 94},   // 633.33MHz
        {D_DD_TOP_UCLK40I / 1 * 16, 0, 15},   // 640.00MHz
        {D_DD_TOP_UCLK40I / 6 * 97, 5, 96},   // 646.67MHz
        {D_DD_TOP_UCLK40I / 5 * 81, 4, 80},   // 648.00MHz
        {D_DD_TOP_UCLK40I / 4 * 65, 3, 64},   // 650.00MHz
        {D_DD_TOP_UCLK40I / 3 * 49, 2, 48},   // 653.33MHz
        {D_DD_TOP_UCLK40I / 5 * 82, 4, 81},   // 656.00MHz
        {D_DD_TOP_UCLK40I / 2 * 33, 1, 32},   // 660.00MHz
        {D_DD_TOP_UCLK40I / 5 * 83, 4, 82},   // 664.00MHz
        {D_DD_TOP_UCLK40I / 3 * 50, 2, 49},   // 666.67MHz
        {D_DD_TOP_UCLK40I / 4 * 67, 3, 66},   // 670.00MHz
        {D_DD_TOP_UCLK40I / 5 * 84, 4, 83},   // 672.00MHz
        {D_DD_TOP_UCLK40I / 6 * 101, 5, 100}, // 673.33MHz
        {D_DD_TOP_UCLK40I / 1 * 17, 0, 16},   // 680.00MHz
        {D_DD_TOP_UCLK40I / 6 * 103, 5, 102}, // 686.67MHz
        {D_DD_TOP_UCLK40I / 5 * 86, 4, 85},   // 688.00MHz
        {D_DD_TOP_UCLK40I / 4 * 69, 3, 68},   // 690.00MHz
        {D_DD_TOP_UCLK40I / 3 * 52, 2, 51},   // 693.33MHz
        {D_DD_TOP_UCLK40I / 5 * 87, 4, 86},   // 696.00MHz
        {D_DD_TOP_UCLK40I / 2 * 35, 1, 34},   // 700.00MHz
        {D_DD_TOP_UCLK40I / 5 * 88, 4, 87},   // 704.00MHz
        {D_DD_TOP_UCLK40I / 3 * 53, 2, 52},   // 706.67MHz
        {D_DD_TOP_UCLK40I / 4 * 71, 3, 70},   // 710.00MHz
        {D_DD_TOP_UCLK40I / 5 * 89, 4, 88},   // 712.00MHz
        {D_DD_TOP_UCLK40I / 6 * 107, 5, 106}, // 713.33MHz
        {D_DD_TOP_UCLK40I / 1 * 18, 0, 17},   // 720.00MHz
        {D_DD_TOP_UCLK40I / 6 * 109, 5, 108}, // 726.67MHz
        {D_DD_TOP_UCLK40I / 5 * 91, 4, 90},   // 728.00MHz
        {D_DD_TOP_UCLK40I / 4 * 73, 3, 72},   // 730.00MHz
        {D_DD_TOP_UCLK40I / 3 * 55, 2, 54},   // 733.33MHz
        {D_DD_TOP_UCLK40I / 5 * 92, 4, 91},   // 736.00MHz
        {D_DD_TOP_UCLK40I / 2 * 37, 1, 36},   // 740.00MHz
        {D_DD_TOP_UCLK40I / 5 * 93, 4, 92},   // 744.00MHz
        {D_DD_TOP_UCLK40I / 3 * 56, 2, 55},   // 746.67MHz
        {D_DD_TOP_UCLK40I / 4 * 75, 3, 74},   // 750.00MHz
        {D_DD_TOP_UCLK40I / 5 * 94, 4, 93},   // 752.00MHz
        {D_DD_TOP_UCLK40I / 6 * 113, 5, 112}, // 753.33MHz
        {D_DD_TOP_UCLK40I / 1 * 19, 0, 18},   // 760.00MHz
        {D_DD_TOP_UCLK40I / 6 * 115, 5, 114}, // 766.67MHz
        {D_DD_TOP_UCLK40I / 5 * 96, 4, 95},   // 768.00MHz
        {D_DD_TOP_UCLK40I / 4 * 77, 3, 76},   // 770.00MHz
        {D_DD_TOP_UCLK40I / 3 * 58, 2, 57},   // 773.33MHz
        {D_DD_TOP_UCLK40I / 5 * 97, 4, 96},   // 776.00MHz
        {D_DD_TOP_UCLK40I / 2 * 39, 1, 38},   // 780.00MHz
        {D_DD_TOP_UCLK40I / 5 * 98, 4, 97},   // 784.00MHz
        {D_DD_TOP_UCLK40I / 3 * 59, 2, 58},   // 786.67MHz
        {D_DD_TOP_UCLK40I / 4 * 79, 3, 78},   // 790.00MHz
        {D_DD_TOP_UCLK40I / 5 * 99, 4, 98},   // 792.00MHz
        {D_DD_TOP_UCLK40I / 6 * 119, 5, 118}, // 793.33MHz
        {D_DD_TOP_UCLK40I / 1 * 20, 0, 19},   // 800.00MHz
        {D_DD_TOP_UCLK40I / 6 * 121, 5, 120}, // 806.67MHz
        {D_DD_TOP_UCLK40I / 5 * 101, 4, 100}, // 808.00MHz
        {D_DD_TOP_UCLK40I / 4 * 81, 3, 80},   // 810.00MHz
        {D_DD_TOP_UCLK40I / 3 * 61, 2, 60},   // 813.33MHz
        {D_DD_TOP_UCLK40I / 5 * 102, 4, 101}, // 816.00MHz
        {D_DD_TOP_UCLK40I / 2 * 41, 1, 40},   // 820.00MHz
        {D_DD_TOP_UCLK40I / 5 * 103, 4, 102}, // 824.00MHz
        {D_DD_TOP_UCLK40I / 3 * 62, 2, 61},   // 826.67MHz
        {D_DD_TOP_UCLK40I / 4 * 83, 3, 82},   // 830.00MHz
        {D_DD_TOP_UCLK40I / 5 * 104, 4, 103}, // 832.00MHz
        {D_DD_TOP_UCLK40I / 6 * 125, 5, 124}, // 833.33MHz
        {D_DD_TOP_UCLK40I / 1 * 21, 0, 20},   // 840.00MHz
        {D_DD_TOP_UCLK40I / 6 * 127, 5, 126}, // 846.67MHz
        {D_DD_TOP_UCLK40I / 5 * 106, 4, 105}, // 848.00MHz
        {D_DD_TOP_UCLK40I / 4 * 85, 3, 84},   // 850.00MHz
        {D_DD_TOP_UCLK40I / 3 * 64, 2, 63},   // 853.33MHz
        {D_DD_TOP_UCLK40I / 5 * 107, 4, 106}, // 856.00MHz
        {D_DD_TOP_UCLK40I / 2 * 43, 1, 42},   // 860.00MHz
        {D_DD_TOP_UCLK40I / 5 * 108, 4, 107}, // 864.00MHz
        {D_DD_TOP_UCLK40I / 3 * 65, 2, 64},   // 866.67MHz
        {D_DD_TOP_UCLK40I / 4 * 87, 3, 86},   // 870.00MHz
        {D_DD_TOP_UCLK40I / 5 * 109, 4, 108}, // 872.00MHz
        {D_DD_TOP_UCLK40I / 6 * 131, 5, 130}, // 873.33MHz
        {D_DD_TOP_UCLK40I / 1 * 22, 0, 21},   // 880.00MHz
        {D_DD_TOP_UCLK40I / 6 * 133, 5, 132}, // 886.67MHz
        {D_DD_TOP_UCLK40I / 5 * 111, 4, 110}, // 888.00MHz
        {D_DD_TOP_UCLK40I / 4 * 89, 3, 88},   // 890.00MHz
        {D_DD_TOP_UCLK40I / 3 * 67, 2, 66},   // 893.33MHz
        {D_DD_TOP_UCLK40I / 5 * 112, 4, 111}, // 896.00MHz
        {D_DD_TOP_UCLK40I / 2 * 45, 1, 44},   // 900.00MHz
        {D_DD_TOP_UCLK40I / 5 * 113, 4, 112}, // 904.00MHz
        {D_DD_TOP_UCLK40I / 3 * 68, 2, 67},   // 906.67MHz
        {D_DD_TOP_UCLK40I / 4 * 91, 3, 90},   // 910.00MHz
        {D_DD_TOP_UCLK40I / 5 * 114, 4, 113}, // 912.00MHz
        {D_DD_TOP_UCLK40I / 6 * 137, 5, 136}, // 913.33MHz
        {D_DD_TOP_UCLK40I / 1 * 23, 0, 22},   // 920.00MHz
        {D_DD_TOP_UCLK40I / 6 * 139, 5, 138}, // 926.67MHz
        {D_DD_TOP_UCLK40I / 5 * 116, 4, 115}, // 928.00MHz
        {D_DD_TOP_UCLK40I / 4 * 93, 3, 92},   // 930.00MHz
        {D_DD_TOP_UCLK40I / 3 * 70, 2, 69},   // 933.33MHz
        {D_DD_TOP_UCLK40I / 5 * 117, 4, 116}, // 936.00MHz
        {D_DD_TOP_UCLK40I / 2 * 47, 1, 46},   // 940.00MHz
        {D_DD_TOP_UCLK40I / 5 * 118, 4, 117}, // 944.00MHz
        {D_DD_TOP_UCLK40I / 3 * 71, 2, 70},   // 946.67MHz
        {D_DD_TOP_UCLK40I / 4 * 95, 3, 94},   // 950.00MHz
        {D_DD_TOP_UCLK40I / 5 * 119, 4, 118}, // 952.00MHz
        {D_DD_TOP_UCLK40I / 6 * 143, 5, 142}, // 953.33MHz
        {D_DD_TOP_UCLK40I / 1 * 24, 0, 23},   // 960.00MHz
        {D_DD_TOP_UCLK40I / 6 * 145, 5, 144}, // 966.67MHz
        {D_DD_TOP_UCLK40I / 5 * 121, 4, 120}, // 968.00MHz
        {D_DD_TOP_UCLK40I / 4 * 97, 3, 96},   // 970.00MHz
        {D_DD_TOP_UCLK40I / 3 * 73, 2, 72},   // 973.33MHz
        {D_DD_TOP_UCLK40I / 5 * 122, 4, 121}, // 976.00MHz
        {D_DD_TOP_UCLK40I / 3 * 74, 2, 73},   // 986.67MHz
        {D_DD_TOP_UCLK40I / 2 * 49, 1, 48},   // 980.00MHz
        {D_DD_TOP_UCLK40I / 5 * 123, 4, 122}, // 984.00MHz
        {D_DD_TOP_UCLK40I / 4 * 99, 3, 98},   // 990.00MHz
        {D_DD_TOP_UCLK40I / 5 * 124, 4, 123}, // 992.00MHz
        {D_DD_TOP_UCLK40I / 6 * 149, 5, 148}, // 993.330MHz
        {D_DD_TOP_UCLK40I / 1 * 25, 0, 24},   // 1000.00MHz
        {D_DD_TOP_UCLK40I / 5 * 126, 4, 125}, // 1008.00MHz
        {D_DD_TOP_UCLK40I / 4 * 101, 3, 100}, // 1010.00MHz
        {D_DD_TOP_UCLK40I / 3 * 76, 2, 75},   // 1013.33MHz
        {D_DD_TOP_UCLK40I / 5 * 127, 4, 126}, // 1016.00MHz
        {D_DD_TOP_UCLK40I / 2 * 51, 1, 50},   // 1020.00MHz
        {D_DD_TOP_UCLK40I / 5 * 128, 4, 127}, // 1024.00MHz
        {D_DD_TOP_UCLK40I / 3 * 77, 2, 76},   // 1026.67MHz
        {D_DD_TOP_UCLK40I / 4 * 103, 3, 102}, // 1030.00MHz
        {D_DD_TOP_UCLK40I / 5 * 129, 4, 128}, // 1032.00MHz
        {D_DD_TOP_UCLK40I / 1 * 26, 0, 25},   // 1040.00MHz
        {D_DD_TOP_UCLK40I / 5 * 131, 4, 130}, // 1048.00MHz
        {D_DD_TOP_UCLK40I / 4 * 105, 3, 104}, // 1050.00MHz
        {D_DD_TOP_UCLK40I / 3 * 79, 2, 78},   // 1053.33MHz
        {D_DD_TOP_UCLK40I / 5 * 132, 4, 131}, // 1056.00MHz
        {D_DD_TOP_UCLK40I / 2 * 53, 1, 52},   // 1060.00MHz
        {D_DD_TOP_UCLK40I / 5 * 133, 4, 132}, // 1064.00MHz
        {D_DD_TOP_UCLK40I / 3 * 80, 2, 79},   // 1066.67MHz
        {D_DD_TOP_UCLK40I / 4 * 107, 3, 106}, // 1070.00MHz
        {D_DD_TOP_UCLK40I / 5 * 134, 4, 133}, // 1072.00MHz
        {D_DD_TOP_UCLK40I / 1 * 27, 0, 26},   // 1080.00MHz
        {D_DD_TOP_UCLK40I / 5 * 136, 4, 135}, // 1088.00MHz
        {D_DD_TOP_UCLK40I / 4 * 109, 3, 108}, // 1090.00MHz
        {D_DD_TOP_UCLK40I / 3 * 82, 2, 81},   // 1093.33MHz
        {D_DD_TOP_UCLK40I / 5 * 137, 4, 136}, // 1096.00MHz
        {D_DD_TOP_UCLK40I / 2 * 55, 1, 54},   // 1100.00MHz
        {D_DD_TOP_UCLK40I / 5 * 138, 4, 137}, // 1104.00MHz
        {D_DD_TOP_UCLK40I / 3 * 83, 2, 82},   // 1106.67MHz
        {D_DD_TOP_UCLK40I / 4 * 111, 3, 110}, // 1110.00MHz
        {D_DD_TOP_UCLK40I / 5 * 139, 4, 138}, // 1112.00MHz
        {D_DD_TOP_UCLK40I / 1 * 28, 0, 27},   // 1120.00MHz
        {D_DD_TOP_UCLK40I / 5 * 141, 4, 140}, // 1128.00MHz
        {D_DD_TOP_UCLK40I / 4 * 113, 3, 112}, // 1130.00MHz
        {D_DD_TOP_UCLK40I / 3 * 85, 2, 84},   // 1133.33MHz
        {D_DD_TOP_UCLK40I / 5 * 142, 4, 141}, // 1136.00MHz
        {D_DD_TOP_UCLK40I / 2 * 57, 1, 56},   // 1140.00MHz
        {D_DD_TOP_UCLK40I / 5 * 143, 4, 142}, // 1144.00MHz
        {D_DD_TOP_UCLK40I / 3 * 86, 2, 85},   // 1146.67MHz
        {D_DD_TOP_UCLK40I / 4 * 115, 3, 114}, // 1150.00MHz
        {D_DD_TOP_UCLK40I / 5 * 144, 4, 143}, // 1152.00MHz
        {D_DD_TOP_UCLK40I / 1 * 29, 0, 28},   // 1160.00MHz
        {D_DD_TOP_UCLK40I / 5 * 146, 4, 145}, // 1168.00MHz
        {D_DD_TOP_UCLK40I / 4 * 117, 3, 116}, // 1170.00MHz
        {D_DD_TOP_UCLK40I / 3 * 88, 2, 87},   // 1173.33MHz
        {D_DD_TOP_UCLK40I / 5 * 147, 4, 146}, // 1176.00MHz
        {D_DD_TOP_UCLK40I / 2 * 59, 1, 58},   // 1180.00MHz
        {D_DD_TOP_UCLK40I / 5 * 148, 4, 147}, // 1184.00MHz
        {D_DD_TOP_UCLK40I / 3 * 89, 2, 88},   // 1186.67MHz
        {D_DD_TOP_UCLK40I / 4 * 119, 3, 118}, // 1190.00MHz
        {D_DD_TOP_UCLK40I / 5 * 149, 4, 148}, // 1192.00MHz
        {D_DD_TOP_UCLK40I / 1 * 30, 0, 29},   // 1200.00MHz
};

extern const FJ_HDMI_N_VAL gHDMI_N_Config[D_HDMI_VIDEO_FORMAT_MAX];
extern UCHAR gPhy_Index;
extern UCHAR gVideo_fmt;

#ifdef CO_HDMI_COMPLIANCE_TEST
UINT32 gRS_System_Disp_Target_RGB;
#endif

static FJ_HDMI_EDID_DTD gDisp_HDMI_Vic_Info = {0};

static BOOL gBF_Disp_DWCH_Config_Flg = FALSE;
static UCHAR gBF_Disp_DWCH_Vsync_Count = 0;
static FJ_DWCH_CONFIG gBF_Disp_DWCH_Config = {
    .hifclk = FJ_DISP_HIFCLK_SEL_594, // HIFCLK
    .H_Active = 3840,                 // H active
    .H_Blanking = 420,                // H blank
    .H_SyncOffset = 88,               // H sync offset
    .H_SyncPulseWidth = 44,           // H sync pulse width
    .V_Active = 2160,                 // V active
    .V_Blanking = 45,                 // V blank
    .V_SyncOffset = 4,                // V sync offset
    .V_SyncPulseWidth = 2,            // V sync pulse width

    .yuv_range[E_DWCH_CLIP_Y].gain = 0x00010000,
    .yuv_range[E_DWCH_CLIP_Y].ofs = 0x00000000,
    .yuv_range[E_DWCH_CLIP_Y].cph = 0xFFFF,
    .yuv_range[E_DWCH_CLIP_Y].cpl = 0x0000,
    .yuv_range[E_DWCH_CLIP_CB].gain = 0x8000,
    .yuv_range[E_DWCH_CLIP_CB].ofs = 0x0000,
    .yuv_range[E_DWCH_CLIP_CB].cph = 0xFFFF,
    .yuv_range[E_DWCH_CLIP_CB].cpl = 0x0000,
    .yuv_range[E_DWCH_CLIP_CR].gain = 0x8000,
    .yuv_range[E_DWCH_CLIP_CR].ofs = 0x0000,
    .yuv_range[E_DWCH_CLIP_CR].cph = 0xFFFF,
    .yuv_range[E_DWCH_CLIP_CR].cpl = 0x0000,
};

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
/**
Check the area to draw for size.
@param[in]	territory_size	OSD territory size.
@param[in]	drawArea	draw Area position.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE osd_comm_draw_area_size_check(U_IM_DISP_SIZE territory_size,
                                                 const T_Rect *drawArea) {
  FJ_ERR_CODE ret;

  ret = FJ_ERR_OK;

  // drawArea width check.(MIN,MAX)
  if (drawArea->Width < D_DISP_OSD_COM_GRISIZE_IHSIZE_MIN) {
    ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
  }
  // drawArea width check.(4-unit)
  else if ((drawArea->Width & 0x0003) != 0) {
    ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
  }
  // drawArea width check.(territory_size)
  else if (drawArea->Postion.StartX + drawArea->Width >
           territory_size.size.width) {
    ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
  } else {
    // drawArea height check.(MIN,MAX)
    if (drawArea->Height < D_DISP_OSD_COM_GRISIZE_IVSIZE_MIN) {
      ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
    }
    // drawArea height check.(2-unit)
    else if ((drawArea->Height & 0x0001) != 0) {
      ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
    }
    // drawArea height check.(territory_size)
    //		else if(drawArea->Postion.StartY + drawArea->Height >
    //territory_size.size.lines){ 			ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
    //		}
    else {
      // DO NOTHING.
    }
  }

  return ret;
}

/**
Sets the display state of the drawing area.
@param[in] layer_id	layer ID.<br>
                        @ref E_OSDLayer_LCD_OSD0		: LCD OSD-0
layer<br>
                        @ref E_OSDLayer_LCD_OSD1		: LCD OSD-1
layer<br>
                        @ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0
layer<br>
                        @ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1
layer<br>
@param[in]	enable_area	Each bit indicates the status display of the
drawing area.<br> See @ref E_FJ_OSD_AREA for bit position.
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
static FJ_ERR_CODE fj_osd_set_osd_area_enable(E_OSDLayer layer_id,
                                              UINT32 enable_area) {
  T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
  FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
  T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
  draw_area_inf =
      &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no]
                                         [disp_no_data.gr_no];
  E_IM_DISP_SEL block = E_IM_DISP_LCD_MIPI;

  UINT32 draw_area = enable_area & 0x3ff;

  if (ret == FJ_ERR_OK) {
    if (draw_area != 0) {
      UINT32 area_id[10];
      UINT32 area_num;
      area_num = BF_Osd_General_Get_Bit_Position(enable_area, area_id,
                                                 OSD_DRAW_AREA_NUM);

      if (area_id[area_num - 1] > draw_area_inf->vaild_area_num) {
        ret = FJ_ERR_OSD_SHOWING_AREA_ERROR;
      }
    }
  }

  if (ret == FJ_ERR_OK) {
    if ((layer_id == E_OSDLayer_LCD_OSD0) ||
        (layer_id == E_OSDLayer_LCD_OSD1)) {
      block = E_IM_DISP_LCD_MIPI;
    } else if ((layer_id == E_OSDLayer_HDMI_OSD0) ||
               (layer_id == E_OSDLayer_HDMI_OSD1)) {
      block = E_IM_DISP_HDMI;
    } else {
      ret = FJ_ERR_NG_INPUT_PARAM;
    }
  }

  if (ret == FJ_ERR_OK) {
    //		BF_Debug_Print_Information(( "fj_osd_set_osd_area_enable():
    //Im_DISP_Set_OSD_Area_Enable() block value: block = %d\n", block));
    Im_DISP_Set_OSD_Area_Enable(block, disp_no_data.layer, (ULONG)draw_area);
    draw_area_inf->enable_area = (USHORT)draw_area;
  }

  return ret;
}

/**
Set Enable/Disable each Display Layer.
@param[in] layer_id layer ID.<br>
                        @ref E_OSDLayer_LCD_MAIN		: LCD Main
Layer<br>
                        @ref E_OSDLayer_LCD_OSD0		: LCD OSD-0
Layer<br>
                        @ref E_OSDLayer_LCD_OSD1		: LCD OSD-1
Layer<br>
                        @ref E_OSDLayer_LCD_OSD			: LCD OSD Layer
(OSD-0 and OSD-1)<br>
                        @ref E_OSDLayer_LCD_IMAGE		: LCD Image
layer (except Color bar)<br>
                        @ref E_OSDLayer_LCD_ALL			: LCD All
Layer<br>
                        @ref E_OSDLayer_HDMI_MAIN		: HDMI Main
Layer<br>
                        @ref E_OSDLayer_HDMI_OSD0		: HDMI OSD-0
Layer<br>
                        @ref E_OSDLayer_HDMI_OSD1		: HDMI OSD-1
Layer<br>
                        @ref E_OSDLayer_HDMI_OSD		: HDMI OSD Layer
(OSD-0 and OSD-1)<br>
                        @ref E_OSDLayer_HDMI_IMAGE		: HDMI Image
layer (except Color bar)<br>
                        @ref E_OSDLayer_HDMI_ALL		: HDMI All
Layer<br>
@param[in] enable	TRUE mean enable to show and FALSE mean disable show.
@retval FJ_ERR_OK			Normal End.
@retval FJ_ERR_OSD_LAYER	layer_id error
@retval FJ_ERR_NG			Abnormal End.
*/
static FJ_ERR_CODE fj_show_enable(E_OSDLayer layer_id, BOOL enable) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  UINT32 layer = E_IM_DISP_SEL_LAYER_NONE;
  BYTE ifs;
  E_IM_DISP_SEL block;

  // Set Block Number
  if ((layer_id & E_OSDLayer_LCD) == E_OSDLayer_LCD) {
    block = E_IM_DISP_LCD_MIPI;
  } else {
    block = E_IM_DISP_HDMI;
  }

  // Get display interface selection
  if (Im_DISP_Get_Display_Interface(block, &ifs) == D_DDIM_OK) {
    if ((ifs == D_IM_DISP_IFS_LCD) || (ifs == D_IM_DISP_IFS_MIPI)) {
      // Set Layer
      if ((layer_id & E_OSDLayer_LCD_MAIN) == E_OSDLayer_LCD_MAIN) {
        layer = E_IM_DISP_SEL_LAYER_MAIN;
      }
      if ((layer_id & E_OSDLayer_LCD_OSD0) == E_OSDLayer_LCD_OSD0) {
        layer |= E_IM_DISP_SEL_LAYER_OSD_0;
      }
      if ((layer_id & E_OSDLayer_LCD_OSD1) == E_OSDLayer_LCD_OSD1) {
        layer |= E_IM_DISP_SEL_LAYER_OSD_1;
      }
      if ((layer_id & E_OSDLayer_LCD_COLOR_BAR) == E_OSDLayer_LCD_COLOR_BAR) {
        layer = E_IM_DISP_SEL_LAYER_DCORE;
      }
    } else {
      if ((layer_id & E_OSDLayer_HDMI_MAIN) == E_OSDLayer_HDMI_MAIN) {
        layer |= E_IM_DISP_SEL_LAYER_MAIN;
      }
      if ((layer_id & E_OSDLayer_HDMI_OSD0) == E_OSDLayer_HDMI_OSD0) {
        layer |= E_IM_DISP_SEL_LAYER_OSD_0;
      }
      if ((layer_id & E_OSDLayer_HDMI_OSD1) == E_OSDLayer_HDMI_OSD1) {
        layer |= E_IM_DISP_SEL_LAYER_OSD_1;
      }
      if ((layer_id & E_OSDLayer_HDMI_COLOR_BAR) == E_OSDLayer_HDMI_COLOR_BAR) {
        layer |= E_IM_DISP_SEL_LAYER_DCORE;
      }
    }

    if (layer == E_IM_DISP_SEL_LAYER_NONE) {
      ret = FJ_ERR_OSD_LAYER;
    } else {
      // Enable/Disable
      if (enable) {
        // Start Disp
        if (block == E_IM_DISP_LCD_MIPI) {
          if (Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, layer, 0) != D_DDIM_OK) {
            ret = FJ_ERR_NG;
          }
        } else {
          if (Im_DISP_Start(layer, E_IM_DISP_SEL_LAYER_NONE, 0) != D_DDIM_OK) {
            ret = FJ_ERR_NG;
          }
        }
      } else {
        // Stop Disp
        if (block == E_IM_DISP_LCD_MIPI) {
          if (Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, layer, 0,
                           E_IM_DISP_STOP_TYPE_WITH_WAIT) != D_DDIM_OK) {
            ret = FJ_ERR_NG;
          }
        } else {
          if (Im_DISP_Stop(layer, E_IM_DISP_SEL_LAYER_NONE, 0,
                           E_IM_DISP_STOP_TYPE_WITH_WAIT) != D_DDIM_OK) {
            ret = FJ_ERR_NG;
          }
        }
      }
    }
  } else {
    ret = FJ_ERR_NG;
  }
  return ret;
}

/**
Get resize rate for LCD
@param[in]	layer		OSD layer selection.<br>
                                                <ul><li>@ref
E_IM_DISP_SEL_LAYER_MAIN <li>@ref E_IM_DISP_SEL_LAYER_OSD_0 <li>@ref
E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in/out]	input_size	LCD's input size.
@param[in]		output_size	LCD's output size.
@param[out]		resize		Resize rate.
*/
static VOID osd_config_get_resize_rate(E_IM_DISP_SEL block,
                                       E_IM_DISP_SEL_LAYER layer,
                                       U_IM_DISP_SIZE *input_size,
                                       U_IM_DISP_SIZE *output_size,
                                       T_IM_DISP_RESIZE *resize) {
  UINT32 hrszm = WIDTH_VGA;
  UINT32 hrszn = WIDTH_VGA;
  UINT32 vrszm = LINES_VGA;
  UINT32 vrszn = LINES_VGA;
  U_IM_DISP_DSTA position = {0};

  if (output_size->size.width == 0 || output_size->size.lines == 0) {
    // Output size error
    resize->rsz1.bit.HRSZM = 1;
    resize->rsz1.bit.HRSZN = 1;
    resize->rsz1.bit.HRSZOF = 0;
    resize->rsz2.bit.VRSZM = 1;
    resize->rsz2.bit.VRSZN = 1;
    resize->rsz2.bit.VRSZOF = 0;
    resize->rsz3.bit.HCSTA = 0;
    resize->rsz3.bit.VCSTA = 0;
    return;
  }

  // Get effective output size
  switch (layer) {
  case E_IM_DISP_SEL_LAYER_MAIN:
    Im_DISP_Get_Display_Position(block, &position);
    break;

  case E_IM_DISP_SEL_LAYER_OSD_0:
  case E_IM_DISP_SEL_LAYER_OSD_1:
    Im_DISP_Get_OSD_Territory_Position(block, layer, &position);
    break;

  default:
    position.bit.DSH = 0;
    position.bit.DSV = 0;
    break;
  }

  // Get Width M/N = output_size.size.width/input_size.size.width
  hrszm = output_size->size.width;
  hrszn = input_size->size.width;
  //	BF_Debug_Print_Error(( "osd_config_get_resize_rate() in width = %d, out
  //width = %d\n", hrszn, hrszm));

  // Get Lines M/N = output_size.size.lines/input_size.size.lines
  vrszm = output_size->size.lines;
  vrszn = input_size->size.lines;
  //	BF_Debug_Print_Error(( "osd_config_get_resize_rate() in height = %d, out
  //height = %d\n", vrszn, vrszm));

#if 1 // Replaced by alternative process.
  if (hrszm > (8 * hrszn)) {
    hrszm = 8;
    hrszn = 1;
    BF_Debug_Print_Error(
        ("osd_config_get_resize_rate() Error!!: The size of width is too "
         "different. in width = %d, out width = %d\n",
         hrszn, hrszm));
  }
  if ((hrszm * 2) < hrszn) {
    hrszm = 1;
    hrszn = 2;
    BF_Debug_Print_Error(("osd_config_get_resize_rate() Error!!: The size of "
                          "width is too close. in width = %d, out width = %d\n",
                          hrszn, hrszm));
  }
  if (vrszm > (8 * vrszn)) {
    vrszm = 8;
    vrszn = 1;
    BF_Debug_Print_Error(
        ("osd_config_get_resize_rate() Error!!: The size of height is too "
         "different. in height = %d, out height = %d\n",
         vrszn, vrszm));
  }
  if ((vrszm * 2) < vrszn) {
    vrszm = 1;
    vrszn = 2;
    BF_Debug_Print_Error(
        ("osd_config_get_resize_rate() Error!!: The size of height is too "
         "close. in height = %d, out height = %d\n",
         hrszn, hrszm));
  }
#else
  if ((double)hrszm / hrszn > 8) {
    hrszm = 8;
    hrszn = 1;
    BF_Debug_Print_Error(
        ("osd_config_get_resize_rate() Error!!: The size of width is too "
         "different. in width = %d, out width = %d\n",
         hrszn, hrszm));
  }
  if ((double)hrszm / hrszn < 0.5) {
    hrszm = 1;
    hrszn = 2;
    BF_Debug_Print_Error(("osd_config_get_resize_rate() Error!!: The size of "
                          "width is too close. in width = %d, out width = %d\n",
                          hrszn, hrszm));
  }
  if ((double)vrszm / vrszn > 8) {
    vrszm = 8;
    vrszn = 1;
    BF_Debug_Print_Error(
        ("osd_config_get_resize_rate() Error!!: The size of height is too "
         "different. in height = %d, out height = %d\n",
         vrszn, vrszm));
  }
  if ((double)vrszm / vrszn < 0.5) {
    vrszm = 1;
    vrszn = 2;
    BF_Debug_Print_Error(
        ("osd_config_get_resize_rate() Error!!: The size of height is too "
         "close. in height = %d, out height = %d\n",
         hrszn, hrszm));
  }
#endif

  BF_Osd_General_Fraction(&hrszm, &hrszn);
  if ((hrszm > 0x1F) || (hrszn > 0x1F)) {
    BF_Debug_Print_Error(("osd_config_get_resize_rate() Error!!: Resize "
                          "value(max:31) over. hrszm(%d)/hrszn(%d)\n",
                          hrszm, hrszn));
  }
  resize->rsz1.bit.HRSZM = hrszm;
  resize->rsz1.bit.HRSZN = hrszn;
  resize->rsz1.bit.HRSZOF = 0;

  BF_Osd_General_Fraction(&vrszm, &vrszn);
  if ((vrszm > 0x1F) || (vrszn > 0x1F)) {
    BF_Debug_Print_Error(("osd_config_get_resize_rate() Error!!: Resize "
                          "value(max:31) over. vrszm(%d)/vrszn(%d)\n",
                          vrszm, vrszn));
  }
  resize->rsz2.bit.VRSZN = vrszn;
  resize->rsz2.bit.VRSZM = vrszm;
  resize->rsz2.bit.VRSZOF = 0;

  // display upper left of input image.
  if (output_size->size.width < input_size->size.width * hrszm / hrszn) {
    input_size->size.width = output_size->size.width * hrszn / hrszm;
  }
  if (output_size->size.lines < input_size->size.lines * vrszm / vrszn) {
    input_size->size.lines = output_size->size.lines * vrszn / vrszm;
  }

  resize->rsz3.bit.HCSTA = 0;
  resize->rsz3.bit.VCSTA = 0;
}

static FJ_ERR_CODE disp_check_addr_set(E_IM_DISP_SEL block) {
  INT32 im_ret;
  E_IM_DISP_BANK bank_no;
  T_IM_DISP_IMAGE_ADDR im_disp_addr;

  // Get current bank No.
  im_ret = Im_DISP_Get_Main_Bank(block, &bank_no);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("disp_check_addr_set() Error!!:Im_DISP_Get_Main_Bank "
                          "error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  im_ret = Im_DISP_Get_Addr(block, bank_no, &im_disp_addr);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("disp_check_addr_set() Error!!:Im_DISP_Get_Addr error. im_ret=%x\n",
         im_ret));
    return FJ_ERR_NG;
  }

  if ((im_disp_addr.y_addr == 0) || (im_disp_addr.c_addr == 0)) {
    //		BF_Debug_Print_Error(( "disp_check_addr_set()  Error!!:DISP current
    //bank adderss is 0. y_addr=%x, c_addr=%x\n", im_disp_addr.y_addr,
    //im_disp_addr.c_addr ));
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

static VOID disp_clear_addr(E_IM_DISP_SEL block) {
  INT32 im_ret;
  INT32 bank_no;
  T_IM_DISP_IMAGE_ADDR im_disp_addr = {0};

  for (bank_no = 0; bank_no < D_IM_DISP_MAIN_ADDR_BANK_SIZE; bank_no++) {
    im_ret = Im_DISP_Set_Addr(block, (E_IM_DISP_BANK)bank_no, &im_disp_addr);
    if (im_ret != D_DDIM_OK) {
      BF_Debug_Print_Error(
          ("disp_clear_addr() Error!!:Im_DISP_Set_Addr error. im_ret=%x\n",
           im_ret));
    }
  }
}

static VOID disp_set_main_size(UCHAR disp_id) {
  INT32 im_ret;
  U_IM_DISP_DSTA position = {0};
  U_IM_DISP_SIZE input_size = {0};
  U_IM_DISP_SIZE output_size = {0};
  T_IM_DISP_RESIZE resize = {.rsz0 = E_IM_DISP_RSZSL_BILINEAR,
                             .rsz1.word = 0x00000101,
                             .rsz2.word = 0x00000101,
                             .rsz3.word = 0};
  E_IM_DISP_SEL block;

  // Set Block Number
  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  // Set ouput position
  position.bit.DSH = gBF_Instance_Display[disp_id].window.out_x_pos;
  position.bit.DSV = gBF_Instance_Display[disp_id].window.out_y_pos;
  im_ret = Im_DISP_Set_Display_Position(block, position);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("disp_set_main_size() Error!!:Im_DISP_Set_Display_Position error. "
         "im_ret=%x. DSH=%d, DSV=%d\n",
         im_ret, position.bit.DSH, position.bit.DSV));
  }

  // Set main layer input size
  input_size.size.width = gBF_Instance_Display[disp_id].window.input.width;
  input_size.size.lines = gBF_Instance_Display[disp_id].window.input.lines;

  // Set resize
  output_size.size.width = gBF_Instance_Display[disp_id].window.output.width;
  output_size.size.lines = gBF_Instance_Display[disp_id].window.output.lines;
  osd_config_get_resize_rate(block, E_IM_DISP_SEL_LAYER_MAIN, &input_size,
                             &output_size, &resize);
  im_ret = Im_DISP_Set_Resize(block, &resize);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("disp_set_main_size() Error!!:Im_DISP_Set_Resize error. im_ret=%x. "
         "rszsl=%d, rsz1=%lu, rsz2=%lu, rsz3=%lu\n",
         im_ret, resize.rsz0, resize.rsz1.word, resize.rsz2.word,
         resize.rsz3.word));
  }

  // Set main layer input size
  im_ret = Im_DISP_Set_Input_Size(block, input_size);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("disp_set_main_size() Error!!:Im_DISP_Set_Input_Size "
                          "error. im_ret=%x. width=%d, lines=%d\n",
                          im_ret, input_size.size.width,
                          input_size.size.lines));
  }
}

/* ------------------- */
/* HDMI				   */
/* ------------------- */
/* HDMI interrupt callback  */
static VOID disp_hdmi_interrupt_callback(UCHAR interrupt) {
  U_IM_HDMI_PHY_STAT0 status;

  Dd_ARM_Critical_Section_Start(gBF_Disp_Spin_Lock);

  Im_HDMI_Get_Phy_Status(&status);

  //	printf( "HDMI INT:int=%d, phy_stat0=0x%x.\n", interrupt, status.byte );

  // Monitor Hot Plug/Unplug Detect.
  if (interrupt == E_IM_HDMI_INT_TYPE_PHY_HDP) {
    if ((status.bit.HPD == 1) && (!gDisp_HDMI_Detect_State)) { // HPD ON
      OS_User_Sta_Tsk(TID_HDMI_DETECT, E_DISP_HDMI_INT_HPD_ON);
    } else if ((status.bit.HPD == 0) && (gDisp_HDMI_Detect_State)) { // HPD OFF
      OS_User_Sta_Tsk(TID_HDMI_DETECT, E_DISP_HDMI_INT_HPD_OFF);
    }
  } else if (interrupt == E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_0) {
    if (gDisp_HDMI_Detect_Setting.check_signal ==
        FJ_DISP_HDMI_CHECK_SIGNAL_HPD_RX_SENSE) {
      if ((status.bit.RX_SENSE_0 == 1) &&
          (!gDisp_HDMI_Detect_State)) { // RX_SENSE_0 ON
        OS_User_Sta_Tsk(TID_HDMI_DETECT, E_DISP_HDMI_INT_RX_SENSE_0_ON);
      } else if ((status.bit.RX_SENSE_0 == 0) &&
                 (gDisp_HDMI_Detect_State)) { // RX_SENSE_0 OFF
        OS_User_Sta_Tsk(TID_HDMI_DETECT, E_DISP_HDMI_INT_RX_SENSE_0_OFF);
      }
    }
  }

  Dd_ARM_Critical_Section_End(gBF_Disp_Spin_Lock);
}

VOID BF_Disp_HDMI_Detect_Task(INT32 stacd) {
  U_IM_HDMI_PHY_STAT0 status;
  UINT32 interval_ms;
  UINT32 detect_count;
  UINT32 check_count;
  UINT32 conn_count = 0;
  UINT32 disc_count = 0;
  UINT32 total_count = 0;
  UCHAR mute;
  UCHAR callback = 0;
  UINT32 check_signal;
  FJ_DISP_HDMI_DETECT detect = FJ_DISP_HDMI_DETECT_DISCONNECTED;

  if (gDisp_HDMI_Detect_Setting.check_signal ==
      FJ_DISP_HDMI_CHECK_SIGNAL_HPD_RX_SENSE) {
    check_signal = D_DISP_HDMI_STAT_HPD_RX_SENSE;
  } else {
    check_signal = D_DISP_HDMI_STAT_HPD;
  }

  interval_ms = gDisp_HDMI_Detect_Setting.interval_ms;
  detect_count = gDisp_HDMI_Detect_Setting.detect_count;
  check_count = gDisp_HDMI_Detect_Setting.check_count;

  BF_Debug_Print_Information(("HDMI check start.(%d)\n", stacd));

  // bit[0]:HDP Mute.
  Im_HDMI_Get_Int_Mute(E_IM_HDMI_INT_REG_PHY_STAT0, &mute);
  mute |= 0x3D;
  Im_HDMI_Set_Int_Mute(E_IM_HDMI_INT_REG_PHY_STAT0, mute);

  if (stacd == E_DISP_HDMI_INT_HPD_ON) {
    if (gDisp_HDMI_Detect_Setting.check_signal ==
        FJ_DISP_HDMI_CHECK_SIGNAL_HPD_RX_SENSE) {
      Im_HDMI_Get_Phy_Status(&status);
      if (status.bit.RX_SENSE_0 == 0) { // RX_SENSE_0 OFF
        BF_HDMI_Configure_VGA_DVI_Video_Mode();
      }
    }
  }

  while (1) {
    Im_HDMI_Get_Phy_Status(&status);

    if ((status.byte & check_signal) == check_signal) {
      conn_count++;
      disc_count = 0;
    } else {
      conn_count = 0;
      disc_count++;
    }

    total_count++;

    if (conn_count >= detect_count) {
      Dd_ARM_Critical_Section_Start(gBF_Disp_Spin_Lock);
      if (!gDisp_HDMI_Detect_State) {
        gDisp_HDMI_Detect_State = TRUE;
        callback = 1;
      }
      Dd_ARM_Critical_Section_End(gBF_Disp_Spin_Lock);

      if (callback) {
        BF_Debug_Print_Information(
            ("HDMI check result. total=%d, connect=%d, status=0x%x\n",
             total_count, conn_count, status.byte));
        detect = FJ_DISP_HDMI_DETECT_CONNECTED;
      }
      break;
    }

    if (disc_count >= detect_count) {
      Dd_ARM_Critical_Section_Start(gBF_Disp_Spin_Lock);
      if (gDisp_HDMI_Detect_State) {
        gDisp_HDMI_Detect_State = FALSE;
        callback = 1;
      }
      Dd_ARM_Critical_Section_End(gBF_Disp_Spin_Lock);

      if (callback) {
        BF_Debug_Print_Information(
            ("HDMI check result. total=%d, disconnect=%d, status=0x%x\n",
             total_count, disc_count, status.byte));
        detect = FJ_DISP_HDMI_DETECT_DISCONNECTED;
      }

      // HDMI PHY Power Off.
      Im_HDMI_Power_On(D_IM_HDMI_ENABLE_OFF);
      break;
    }

    if (total_count >= check_count) {
      BF_Debug_Print_Information(("HDMI check result. total=%d, connect=%d, "
                                  "disconnect=%d, status=0x%x\n",
                                  total_count, conn_count, disc_count,
                                  status.byte));
      callback = 0;

      // HDMI PHY Power Off.
      Im_HDMI_Power_On(D_IM_HDMI_ENABLE_OFF);
      break;
    }

    OS_User_Dly_Tsk(interval_ms);
  }

  // bit[0]:HDP Unmute.
  Im_HDMI_Get_Int_Mute(E_IM_HDMI_INT_REG_PHY_STAT0, &mute);
  mute &= 0xC2;
  Im_HDMI_Set_Int_Mute(E_IM_HDMI_INT_REG_PHY_STAT0, mute);

  BF_Debug_Print_Information(("HDMI check end.\n"));

  if ((callback) && (gDisp_HDMI_Callback)) {
    (*gDisp_HDMI_Callback)(detect);
  }

  OS_User_Ext_Tsk();
}

// convert form FJ_DISP_VIDEO_ID to FJ_HDMI_XXX
static FJ_ERR_CODE
disp_hdmi_convert_video_id_2_video_format_id(FJ_DISP_VIDEO_ID video_id,
                                             UINT32 *video_format_id,
                                             E_HD_RESOLUTION *hd_resolution) {
  if (video_format_id == NULL) {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  switch (video_id) {
  case FJ_DISP_VIDEO_ID_720p: // Code:4
    *video_format_id = FJ_HDMI_1280x720p60Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_720P_60HZ;
    break;
  case FJ_DISP_VIDEO_ID_720p50: // Code:19
    *video_format_id = FJ_HDMI_1280x720p50Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_720P_50HZ;
    break;
  case FJ_DISP_VIDEO_ID_1080i: // Code:5
    *video_format_id = FJ_HDMI_1920x1080i60Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_1080I_60H;
    break;
  case FJ_DISP_VIDEO_ID_1080i25: // Code:20;
    *video_format_id = FJ_HDMI_1920x1080i50Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_1080I_50H;
    break;
  case FJ_DISP_VIDEO_ID_480p: // Code:2
    *video_format_id = FJ_HDMI_720x480p60Hz_4_3;
    *hd_resolution = E_HD_RESOLUTION_480P_60HZ;
    break;
  case FJ_DISP_VIDEO_ID_576p: // Code:17
    *video_format_id = FJ_HDMI_720x576p50Hz_4_3;
    *hd_resolution = E_HD_RESOLUTION_576P_50HZ;
    break;
  case FJ_DISP_VIDEO_ID_1080p30: // Code:34
    *video_format_id = FJ_HDMI_1920x1080p30Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_1080P_30H;
    break;
  case FJ_DISP_VIDEO_ID_1080p25: // Code:33
    *video_format_id = FJ_HDMI_1920x1080p25Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_1080P_25H;
    break;
  case FJ_DISP_VIDEO_ID_1080p: // Code:16
    *video_format_id = FJ_HDMI_1920x1080p60Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_1080P_60H;
    break;
  case FJ_DISP_VIDEO_ID_1080p50: // Code:31
    *video_format_id = FJ_HDMI_1920x1080p50Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_1080P_50H;
    break;
  case FJ_DISP_VIDEO_ID_720p25: // Code:61
    *video_format_id = FJ_HDMI_1280x720p25Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_720P_25HZ;
    break;
  case FJ_DISP_VIDEO_ID_720p30: // Code:62
    *video_format_id = FJ_HDMI_1280x720p30Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_720P_30HZ;
    break;
  case FJ_DISP_VIDEO_ID_2160p25_3840: // Code:94
    *video_format_id = FJ_HDMI_3840x2160p25Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_2160P_25H;
    break;
  case FJ_DISP_VIDEO_ID_2160p30_3840: // Code:95
    *video_format_id = FJ_HDMI_3840x2160p30Hz_16_9;
    *hd_resolution = E_HD_RESOLUTION_2160P_30H;
    break;
  case FJ_DISP_VIDEO_ID_2160p50_3840: // Code:96
    *video_format_id = FJ_HDMI_3840x2160p50Hz_16_9;
    if (gVideo_fmt == D_IM_HDMI_OUT_YCC420) {
      // for YCbCr420 Change HIF clock 594MHz -> 297MHz)
      *hd_resolution = E_HD_RESOLUTION_2160P_25H;
    } else {
      *hd_resolution = E_HD_RESOLUTION_2160P_50H;
    }
    break;
  case FJ_DISP_VIDEO_ID_2160p_3840: // Code:97
    *video_format_id = FJ_HDMI_3840x2160p60Hz_16_9;
    if (gVideo_fmt == D_IM_HDMI_OUT_YCC420) {
      // for YCbCr420 Change HIF clock 594MHz -> 297MHz)
      *hd_resolution = E_HD_RESOLUTION_2160P_30H;
    } else {
      *hd_resolution = E_HD_RESOLUTION_2160P_60H;
    }
    break;
  default:
    return FJ_ERR_NO_SUPPORT;
  }

  return FJ_ERR_OK;
}

static INT32 disp_set_y2r_matrix(E_IM_DISP_SEL block,
                                 E_BF_DISP_YCC_COLOR_SPACE color_space) {
  INT32 im_ret;

  const U_IM_DISP_YR_MATRIX_COEFFICIENT ycc601_to_rgb[D_IM_DISP_MATRIX_SIZE] = {
      {0x000059C000004000}, // 0x4000,0x0000,0x59C0 (1.0,  0.0,       1.40625)
      {0x0000D250EA004000}, // 0x4000,0xEA00,0xD250 (1.0, -0.34375,  -0.703125)
      {0x0000FFF071704000}  // 0x4000,0x7170,0xFFF0 (1.0,  1.765625,  0.015625)
  };
  const U_IM_DISP_YR_MATRIX_COEFFICIENT ycc709_to_rgb[D_IM_DISP_MATRIX_SIZE] = {
      {0x000064C900004000}, // 0x4000,0x0000,0x64C9 (1.0,  0.0,      1.5748)
      {0x0000E20AF4024000}, // 0x4000,0xF402,0xE20A (1.0, -0.1873,  -0.4681)
      {0x0000000076C24000}  // 0x4000,0x76C2,0x0000 (1.0,  1.85565,  0.0)
  };

  if (color_space == E_BF_DISP_YCC_COLOR_SPACE_601) {
    im_ret = Im_DISP_Set_Matrix(block, ycc601_to_rgb);
  } else {
    im_ret = Im_DISP_Set_Matrix(block, ycc709_to_rgb);
  }

  return im_ret;
}

VOID Disp_OSD_Copy_By_Src_Offset(LONG src, LONG dst, LONG width,
                                 LONG src_offset) {
  long src_cnt = 0;
  long width_cnt = 0;
  long *src_temp = (long *)src;
  long *dst_temp = (long *)dst;

  for (width_cnt = 0; width_cnt < width; width_cnt += 4) {
    for (src_cnt = 0; src_cnt < 4; src_cnt++) {
      *(dst_temp + src_cnt) = *(src_temp + ((src_offset * src_cnt) / 4));
    }
    src_temp += ((src_offset * 4) / 4);
    dst_temp += 4;
  }
  return;
}

void Disp_OSD_Copy_Invert_By_4pixel(long src, long dst, long width) {
  long src_cnt = 0;
  long width_cnt = 0;
  long *src_temp = (long *)src;
  long *dst_temp = (long *)dst;

  for (width_cnt = 0; width_cnt < width; width_cnt += 4) {
    for (src_cnt = 0; src_cnt < 4; src_cnt++) {
      *(dst_temp + src_cnt) = *(src_temp - src_cnt);
    }
    src_temp -= 4;
    dst_temp += 4;
  }
  return;
}

/* FUNCTION osd_rotate90()
 * ARGUMENT
 *   src         : left top start address
 *   dst         : left top start address
 *   src_width   : (unit : pixel, minimum : 1, step : 1)
 *   src_height  : (unit : pixel, minimum : 8, step : 8)
 *   src_g_width : (unit : pixel)
 *   dst_g_width : (unit : pixel)
 * COMMENT
 *   It writes sequentialiy pixel data on destination.
 */
void disp_osd_rotate90(long src, long dst, long src_width, long src_height,
                       long src_g_width, long dst_g_width) {
  long src_tmp;
  long dst_tmp;
  long width_tmp;
  long src_offset_tmp;
  long src_g_width_4;
  long dst_g_width_4;
  int cnt;
  dst_tmp = dst;
  src_tmp = src + 4 * (src_g_width * (src_height - 1));
  width_tmp = src_height;
  src_g_width_4 = 4 * src_g_width;
  dst_g_width_4 = 4 * dst_g_width;
  src_offset_tmp = -src_g_width_4;
  for (cnt = 0; cnt < src_width; cnt++) {
    Disp_OSD_Copy_By_Src_Offset(src_tmp, dst_tmp, width_tmp, src_offset_tmp);
    dst_tmp += dst_g_width_4;
    src_tmp += 4;
  }
}

/* FUNCTION osd_rotate270()
 *   src        : left top start address
 *   dst        : left top start address
 *   src_width   : (unit : pixel, minimum : 1, step : 1)
 *   src_height  : (unit : pixel, minimum : 8, step : 8)
 *   src_g_width : (unit : pixel)
 *   dst_g_width : (unit : pixel)
 * COMMENT
 *   It writes sequentialiy pixel data on destination.
 */
void disp_osd_rotate270(long src, long dst, long src_width, long src_height,
                        long src_g_width, long dst_g_width) {
  long src_tmp;
  long dst_tmp;
  long width_tmp;
  long src_offset_tmp;
  long src_g_width_4;
  long dst_g_width_4;
  int cnt;
  dst_tmp = dst;
  src_tmp = src + 4 * (src_width - 1);
  width_tmp = src_height;
  src_g_width_4 = 4 * src_g_width;
  dst_g_width_4 = 4 * dst_g_width;
  src_offset_tmp = src_g_width_4;
  for (cnt = 0; cnt < src_width; cnt++) {
    Disp_OSD_Copy_By_Src_Offset(src_tmp, dst_tmp, width_tmp, src_offset_tmp);
    dst_tmp += dst_g_width_4;
    src_tmp -= 4;
  }
}

/* FUNCTION osd_rotate180()
 *   src        : left top start address
 *   dst        : left top start address
 *   src_width   : (unit : pixel, minimum : 8, step : 8)
 *   src_height  : (unit : pixel, minimum : 1, step : 1)
 *   src_g_width : (unit : pixel)
 *   dst_g_width : (unit : pixel)
 * COMMENT
 *   It writes sequentialiy pixel data on destination.
 */
void disp_osd_rotate180(long src, long dst, long src_width, long src_height,
                        long src_g_width, long dst_g_width) {
  long src_tmp;
  long dst_tmp;
  long width_tmp;
  long src_g_width_4;
  long dst_g_width_4;
  int cnt;
  dst_tmp = dst;
  src_tmp = src + 4 * (src_g_width * (src_height - 1) + (src_width - 1));
  width_tmp = src_width;
  src_g_width_4 = 4 * src_g_width;
  dst_g_width_4 = 4 * dst_g_width;
  for (cnt = 0; cnt < src_height; cnt++) {
    Disp_OSD_Copy_Invert_By_4pixel(src_tmp, dst_tmp, width_tmp);
    src_tmp -= src_g_width_4;
    dst_tmp += dst_g_width_4;
  }
}

/* FUNCTION disp_osd_copy_by_thin_mode()
 * @brief		The processing of Xch copy by thin mode(Downsampling
 high-speed mode)
 * @param[in]   E_IM_XCH_CH_SEL	xch			:Channel selection of
 Xch
 * @param[in]	UINT32			src_addr
 * @param[in]	USHORT			src_gl_width
 * @param[in]	USHORT			dst_gl_width
 * @param[in]	UINT32			dst_addr
 * @param[in]	USHORT			width
 * @param[in]	USHORT			lines
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
                                D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR,
 D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
static INT32 disp_osd_copy_by_thin_mode(E_IM_XCH_CH_SEL xch, UINT32 src_addr,
                                        USHORT src_gl_width,
                                        USHORT dst_gl_width, UINT32 dst_addr,
                                        USHORT width, USHORT lines) {
  INT32 ret;

  T_IM_XCH_CTRL_CMN xch_ctrl_cmn;
  T_IM_XCH_CTRL_THIN xch_ctrl_thin;

#ifdef CO_PARAM_CHECK
  if ((xch != E_IM_XCH_CH_SEL_0) && (xch != E_IM_XCH_CH_SEL_1)) {
    // Channel number error
    Ddim_Assertion(("I:Im_Xch_Copy: Unknown channel error. Xch = %d\n", xch));
    return D_IM_XCH_INPUT_PARAM_ERR;
  }
#endif // CO_PARAM_CHECK

  memset(&xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN));
  memset(&xch_ctrl_thin, 0, sizeof(T_IM_XCH_CTRL_THIN));

  xch_ctrl_cmn.mode = D_IM_XCH_MD_DOWN_HI_SPEED;
  xch_ctrl_cmn.g_hsize = dst_gl_width;
  xch_ctrl_cmn.d_hsize = width;
  xch_ctrl_cmn.d_vsize = lines;
  xch_ctrl_cmn.dst_addr = dst_addr;
  xch_ctrl_cmn.pCallBack = NULL;

  xch_ctrl_thin.xdtype = D_IM_XCH_XDTYPE_8;
  xch_ctrl_thin.hcyc = 0x00;
  xch_ctrl_thin.vcyc = 0x00;
  xch_ctrl_thin.hen = 0xFFFFFFFF;
  xch_ctrl_thin.ven = 0xFFFFFFFF;
  xch_ctrl_thin.s_hsize = src_gl_width;
  xch_ctrl_thin.src_addr = src_addr;

  ret = Im_Xch_Open(xch, D_DDIM_WAIT_END_TIME);
  if (ret != D_IM_XCH_OK) {
    // Im_Xch_Open error
    Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Open error. ret = 0x%X\n", ret));
    return ret;
  }

  ret = Im_Xch_Ctrl_Common(xch, &xch_ctrl_cmn);
  if (ret != D_IM_XCH_OK) {
    // Im_Xch_Ctrl_Common error
    Im_Xch_Close(xch);
    Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
    return ret;
  }

  ret = Im_Xch_Ctrl_Thin(xch, &xch_ctrl_thin);
  if (ret != D_IM_XCH_OK) {
    // Im_Xch_Ctrl_Copy error
    Im_Xch_Close(xch);
    Ddim_Print(
        ("I:Im_Xch_Ctrl_Thin: Im_Xch_Ctrl_Copy error. ret = 0x%X\n", ret));
    return ret;
  }

  ret = Im_Xch_Start_Sync(xch);
  if (ret != D_IM_XCH_OK) {
    // Im_Xch_Close error
    Im_Xch_Close(xch);
    Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Start_Sync error. ret = 0x%X\n", ret));
    return ret;
  }

  ret = Im_Xch_Close(xch);
  if (ret != D_IM_XCH_OK) {
    // Im_Xch_Close error
    Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Close error. ret = 0x%X\n", ret));
    return ret;
  }

  return D_IM_XCH_OK;
}

static FJ_ERR_CODE lcd_clk_reg_search(T_DISP_LCD_CLK_REG *const lcd_clk_reg,
                                      FLOAT lcdclk) {
  ULONG reg_b;
  FLOAT pll04_out;
  ULONG tbl_idx;
  FLOAT pll04_out_diff;

  for (reg_b = D_DISP_LCD_CLK_REG_B_MIN; reg_b <= D_DISP_LCD_CLK_REG_B_MAX;
       reg_b++) {
    pll04_out = lcdclk * reg_b;

    // PLL04 output check
    if ((pll04_out < D_DISP_LCD_CLK_PLL04_OUT_MIN) ||
        (pll04_out > D_DISP_LCD_CLK_PLL04_OUT_MAX)) {
      continue;
    }

    // serch Register A, N
    for (tbl_idx = 0; tbl_idx < D_DISP_LCD_CLK_PLL04_TBL_MAX; tbl_idx++) {
      pll04_out_diff = g_lcd_clk_pll04_tbl[tbl_idx].pll04_out - pll04_out;
      if (((-D_DISP_LCD_CLK_PLL04_MATCH_DIFF) < pll04_out_diff) &&
          (pll04_out_diff < D_DISP_LCD_CLK_PLL04_MATCH_DIFF)) {
        lcd_clk_reg->a = g_lcd_clk_pll04_tbl[tbl_idx].a;
        lcd_clk_reg->n = g_lcd_clk_pll04_tbl[tbl_idx].n;
        lcd_clk_reg->b = reg_b - 1;
        return FJ_ERR_OK;
      }
    }
  }

  return FJ_ERR_NG;
}

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/* ------------- */
/* LCD/HDMI      */
/* ------------- */
FJ_ERR_CODE BF_Disp_Set_Main_Addr(UCHAR disp_id, FJ_DISP_YCC_ADDR *ycc_addr,
                                  E_BF_DISP_IMAGE_FORMAT image_format,
                                  E_BF_DISP_YCC_COLOR_SPACE color_space,
                                  ULONG global_width) {
  INT32 im_ret;
  E_IM_DISP_BANK bank_no;
  T_IM_DISP_IMAGE_ADDR im_disp_addr;
  U_IM_DISP_LIDT lidt = {.word[0] = 0, .word[1] = 0};
  E_IM_DISP_SEL block;
  U_IM_DISP_LALP lalp;

  if (gBF_Instance_Display[disp_id].connect_state ==
      E_BF_INSTANCE_CONNECT_STATE_OFF) {
    // Guard when suspended before the mode change.
    return FJ_ERR_NG;
  }

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    // Guard when suspended before the mode change.
    BF_Debug_Print_Error(
        ("BF_Disp_Set_Main_Addr() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Set Block Number
  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  // Set image format
  im_ret = Im_DISP_Get_Input_Data_Transfer(block, &lidt);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("BF_Disp_Set_Main_Addr() Error!!:Im_DISP_Get_Input_Data_Transfer "
         "error. im_ret=%x\n",
         im_ret));
    return FJ_ERR_NG;
  }

  lidt.bit.IFMT = image_format;

  if (image_format == E_BF_DISP_IMAGE_FORMAT_VIDEO) {
    lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;
    lidt.bit.NBT = D_IM_DISP_NBT_512BYTE;
    lidt.bit.PKGDV = 0;
    lidt.bit.LVFM = 1;
  } else {
    lidt.bit.NBT = D_IM_DISP_NBT_512BYTE;
    lidt.bit.PKGDV = 0;
    lidt.bit.LVFM = 0;
  }

  // Set parameter reflect disable.
  Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_OFF);
  Dd_ARM_Dsb_Pou();

  im_ret = Im_DISP_Set_Input_Data_Transfer(block, lidt);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("BF_Disp_Set_Main_Addr() Error!!:Im_DISP_Set_Input_Data_Transfer "
         "error. im_ret=%x\n",
         im_ret));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  // Set YCC->RGB matrix
  im_ret = disp_set_y2r_matrix(block, color_space);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("BF_Disp_Set_Main_Addr() Error!!:disp_set_y2r_matrix "
                          "error. im_ret=%x\n",
                          im_ret));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  // Set global width
  im_ret = Im_DISP_Set_Lfd(block, global_width, global_width);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("BF_Disp_Set_Main_Addr() Error!!:Im_DISP_Set_Lfd "
                          "error. im_ret=%x, global_width=%lu\n",
                          im_ret, global_width));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  // Set main size
  disp_set_main_size(disp_id);

  // Get current bank No.
  im_ret = Im_DISP_Get_Main_Bank_Monitor(block, &bank_no);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("BF_Disp_Set_Main_Addr() Error!!:Im_DISP_Get_Main_Bank_Monitor error. "
         "im_ret=%x\n",
         im_ret));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  // Update current bank No.
  if (bank_no == E_IM_DISP_BANK_11) {
    bank_no = E_IM_DISP_BANK_00;
  } else {
    bank_no++;
  }

  // Get image format
  Im_DISP_Get_Input_Data_Transfer(block, &lidt);

  // Set bank address
  if ((lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT) ||
      (lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_YCC422_RP_12BIT_PACK) ||
      (lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_YCC422_RP_16BIT) ||
      (lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_YCC422_RP_10BIT_PACK)) {
    im_disp_addr.y_addr =
        ycc_addr->y_addr +
        (global_width * gBF_Instance_Display[disp_id].window.in_y_pos) +
        gBF_Instance_Display[disp_id].window.in_x_pos;
    im_disp_addr.c_addr =
        ycc_addr->cbcr_addr +
        (global_width * (gBF_Instance_Display[disp_id].window.in_y_pos)) +
        gBF_Instance_Display[disp_id].window.in_x_pos;
  } else if (lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT) {
    if (lidt.bit.LVFM == 1) { // Video format
      if ((gBF_Instance_Display[disp_id].window.in_x_pos != 0) ||
          (gBF_Instance_Display[disp_id].window.in_y_pos != 0)) {
        BF_Debug_Print_Error(("BF_Disp_Set_Main_Addr() Error!!:in position is "
                              "illegal. in_x_pos=%d, in_y_pos=%d\n",
                              gBF_Instance_Display[disp_id].window.in_x_pos,
                              gBF_Instance_Display[disp_id].window.in_y_pos));
        // Set parameter reflect enable.
        Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
        return FJ_ERR_NG;
      } else {
        im_disp_addr.y_addr = ycc_addr->y_addr;
        im_disp_addr.c_addr = ycc_addr->cbcr_addr;
      }
    } else {
      im_disp_addr.y_addr =
          ycc_addr->y_addr +
          (global_width * gBF_Instance_Display[disp_id].window.in_y_pos) +
          gBF_Instance_Display[disp_id].window.in_x_pos;
      im_disp_addr.c_addr =
          ycc_addr->cbcr_addr +
          (global_width * (gBF_Instance_Display[disp_id].window.in_y_pos / 2)) +
          gBF_Instance_Display[disp_id].window.in_x_pos;
    }
  } else if (lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_YCC420_RP_10BIT_PACK) {
    im_disp_addr.y_addr =
        ycc_addr->y_addr +
        (global_width * gBF_Instance_Display[disp_id].window.in_y_pos) +
        gBF_Instance_Display[disp_id].window.in_x_pos;
    im_disp_addr.c_addr =
        ycc_addr->cbcr_addr +
        (global_width * (gBF_Instance_Display[disp_id].window.in_y_pos / 2)) +
        gBF_Instance_Display[disp_id].window.in_x_pos;
  } else if ((lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_GENERAL_8BIT) ||
             (lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_GENERAL_16BIT) ||
             (lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_GENERAL_10BIT) ||
             (lidt.bit.IFMT == D_IM_DISP_LIDT_IFMT_GENERAL_10BITx2)) {
    im_disp_addr.y_addr =
        ycc_addr->y_addr +
        (global_width * gBF_Instance_Display[disp_id].window.in_y_pos) +
        gBF_Instance_Display[disp_id].window.in_x_pos;
    im_disp_addr.c_addr = ycc_addr->cbcr_addr;
  } else {
    BF_Debug_Print_Error(
        ("BF_Disp_Set_Main_Addr() Error!!:Im_DISP_Set_Addr error. im_ret=%x\n",
         im_ret));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  // check bank address is 0. (to prevent DECERR)
  if ((im_disp_addr.y_addr == 0) || (im_disp_addr.c_addr == 0)) {
    BF_Debug_Print_Error(("BF_Disp_Set_Main_Addr() Error!!:DISP bank adderss "
                          "is 0. y_addr=%lx, c_addr=%lx\n",
                          im_disp_addr.y_addr, im_disp_addr.c_addr));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  im_ret = Im_DISP_Set_Addr(block, bank_no, &im_disp_addr);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("BF_Disp_Set_Main_Addr() Error!!:Im_DISP_Set_Addr error. im_ret=%x\n",
         im_ret));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }
  Dd_ARM_Dsb_Pou();

  // Set new bank No.
  Im_DISP_Set_Main_Bank(block, bank_no);

  Dd_ARM_Critical_Section_Start(gBF_Disp_Main_Spin_Lock);

  if (gDisp_Image_Layer_Enable_Wait[disp_id]) {
    lalp.bit.ALP = 0xFF;
    Im_DISP_Set_Alpha_Value(block, lalp);

    gDisp_Image_Layer_Enable_Wait[disp_id] = FALSE;

    BF_Debug_Print_Information(
        ("BF_Disp_Set_Main_Addr() ::released enable waiting.\n"));
  }

  Dd_ARM_Critical_Section_End(gBF_Disp_Main_Spin_Lock);

  // Set parameter reflect enable.
  Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);

  Dd_ARM_Dsb_Pou();

  return FJ_ERR_OK;
}

VOID BF_Disp_Wait_Vsync(FJ_DISP_TYPE disp_type) {
  INT32 im_ret;
  E_IM_DISP_SEL block;

  // Set Block Number
  if (disp_type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  im_ret = Im_DISP_Wait_Vsync(block, E_IM_DISP_INTERRUPTION_SELECT_VE,
                              E_IM_DISP_WAIT_1_TIME);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("BF_Disp_Wait_Vsync() Error!!:Im_DISP_Wait_Vsync error. im_ret=%x\n",
         im_ret));
  }
}

#if 0
FJ_ERR_CODE BF_Disp_Get_Output_Fps( UCHAR disp_id, ULONG* output_fps, FJ_DISP_LCD_SCAN_MODE *scan_mode )
{
	T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_output_trglmt;
	T_IM_DISP_CTRL_OUTPUT			ctrl_output;
	ULONG	hcyc, vcyc;
	UCHAR	tsl;

	Im_DISP_Get_Ctrl_Output( &ctrl_output_trglmt, &ctrl_output );

	hcyc = ctrl_output_trglmt.hcyc;

	if( ctrl_output_trglmt.tsl == D_IM_DISP_TSL_INTERLACE ){
		vcyc = ctrl_output_trglmt.vcyc.bit.VCYC1 + ctrl_output_trglmt.vcyc.bit.VCYC2;
		tsl = 2;
	}
	else {
		vcyc = ctrl_output_trglmt.vcyc.bit.VCYC1;
		tsl = 1;
	}

	*output_fps	= 10000000 / ( (float)( hcyc * vcyc ) / ( Dd_Top_Get_LCDCLK() / 10000 ) ) * tsl;

	*scan_mode	= (FJ_DISP_LCD_SCAN_MODE)ctrl_output_trglmt.tsl;

	return FJ_ERR_OK;
}
#endif

FJ_ERR_CODE BF_Disp_Set_Audio_I2S(ULONG samp_freq,
                                  E_BF_DISP_HDMI_AUDIO_I2S_JUST i2s_just,
                                  E_BF_DISP_HDMI_AUDIO_I2S_LEN i2s_len) {
  T_IM_HDMI_AUDIO audio;
  UCHAR p05chg;

  if (gBF_Instance_Display[1].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("BF_Disp_Set_Audio_I2S() Error!!:HDMI is not used.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if ((samp_freq != FJ_DISP_HDMI_AUDIO_32000HZ) &&
      (samp_freq != FJ_DISP_HDMI_AUDIO_48000HZ) &&
      (samp_freq != FJ_DISP_HDMI_AUDIO_96000HZ) &&
      (samp_freq != FJ_DISP_HDMI_AUDIO_44100HZ)) {
    BF_Debug_Print_Error(
        ("BF_Disp_Set_Audio_I2S() Error!!:samp_freq is illegal.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Start HIF clock (guard for Dd_HDMI_Set_Audio())
  Dd_Top_Set_CLKSTOP3_HIFCK(0);

  memset(&audio, 0, sizeof(T_IM_HDMI_AUDIO));

  audio.aud_conf0.byte = 0x21; // i2s_select=1(I2S), I2S_in_en=1(1-2 ch enable).

  if (i2s_just == E_BF_DISP_HDMI_AUDIO_I2S_JUST_RIGHT) {
    // right-justified.
    audio.aud_conf1.bit.I2S_mode = E_IM_HDMI_I2S_MODE_RIGHT_JUSTIFIED;
  } else if (i2s_just == E_BF_DISP_HDMI_AUDIO_I2S_JUST_LEFT) {
    // right-justified.
    audio.aud_conf1.bit.I2S_mode = E_IM_HDMI_I2S_MODE_LEFT_JUSTIFIED;
  } else {
    // Standard-justified.
    audio.aud_conf1.bit.I2S_mode = E_IM_HDMI_I2S_MODE_STANDARD;
  }
  if (i2s_len == E_BF_DISP_HDMI_AUDIO_I2S_LEN_24) {
    // 24bit.
    audio.aud_conf1.bit.I2S_width = 0x18; // 24 bit data samples(0x18)
    audio.oiec_wordlength =
        0x0b; // Word length configuration(MAX=24bit/LEN=24bit)
  } else {
    // 16bit.
    audio.aud_conf1.bit.I2S_width = 0x10; // 16 bit data samples(0x10)
    audio.oiec_wordlength =
        0x0a; // Word length configuration(MAX=20bit/LEN=20bit)
  }
  audio.aud_conf2.byte = 0x00; // NLPCM=0, HBR=0.
  audio.aud_spdif1.byte = 0;   // No use.
  audio.fc_audsconf.byte = 0;  // aud_packet_layout=0(layout0).

  // AudioPacketizer Registers
  audio.aud_inputclkfs = E_IM_HDMI_AUD_IFSFACTOR_64XFS; // 64xFs.
  audio.aud_ncts_atomic_write = 0x00;                   // atomic_write

  // FrameComposer Registers
  audio.fc_audiconf0.bit.CC = 1; // Channel count 2ch(1)
  audio.fc_audiconf0.bit.CT = 0; // Coding Type Refer to Stream Header(0)
  audio.fc_audiconf1.bit.SS = 0; // Sampling size Refer to Stream header(0)
  audio.fc_audiconf1.bit.SF = 0; // Sampling frequency Refer to Stream Header(0)
  audio.fc_audiconf2 = 0;
  audio.fc_audiconf3.bit.LFEPBL = 0;
  audio.fc_audiconf3.bit.DM_INH = 0;
  audio.fc_audiconf3.bit.LSV = 1;

  // Sampling frequency and N value setting
  p05chg = Dd_Top_Get_PLLCNT9_P05CHG();
  switch (samp_freq) {
  case FJ_DISP_HDMI_AUDIO_32000HZ:
    audio.sampfreq = 3; // (32kHz).
    if (p05chg == 1) {
      audio.aud_n1 = (gHDMI_N_Config[gPhy_Index].sample32_chg & 0x00FF);
      audio.aud_n2 = (gHDMI_N_Config[gPhy_Index].sample32_chg >> 8);
      audio.aud_n3_audn = 0x00;
    } else {
      audio.aud_n1 = (gHDMI_N_Config[gPhy_Index].sample32 & 0x00FF);
      audio.aud_n2 = (gHDMI_N_Config[gPhy_Index].sample32 >> 8);
      audio.aud_n3_audn = 0x00;
    }
    break;
  case FJ_DISP_HDMI_AUDIO_44100HZ:
    audio.sampfreq = 0; // (44.1kHz).
    if (p05chg == 1) {
      audio.aud_n1 = (gHDMI_N_Config[gPhy_Index].sample441_chg & 0x00FF);
      audio.aud_n2 = (gHDMI_N_Config[gPhy_Index].sample441_chg >> 8);
      audio.aud_n3_audn = 0x00;
    } else {
      audio.aud_n1 = (gHDMI_N_Config[gPhy_Index].sample441 & 0x00FF);
      audio.aud_n2 = (gHDMI_N_Config[gPhy_Index].sample441 >> 8);
      audio.aud_n3_audn = 0x00;
    }
    break;
  case FJ_DISP_HDMI_AUDIO_48000HZ:
    audio.sampfreq = 2; // (48kHz).
    if (p05chg == 1) {
      audio.aud_n1 = (gHDMI_N_Config[gPhy_Index].sample48_chg & 0x00FF);
      audio.aud_n2 = (gHDMI_N_Config[gPhy_Index].sample48_chg >> 8);
      audio.aud_n3_audn = 0x00;
    } else {
      audio.aud_n1 = (gHDMI_N_Config[gPhy_Index].sample48 & 0x00FF);
      audio.aud_n2 = (gHDMI_N_Config[gPhy_Index].sample48 >> 8);
      audio.aud_n3_audn = 0x00;
    }
    break;
  case FJ_DISP_HDMI_AUDIO_96000HZ:
    audio.sampfreq = 10; // (96kHz).
    if (p05chg == 1) {
      audio.aud_n1 = (gHDMI_N_Config[gPhy_Index].sample96_chg & 0x00FF);
      audio.aud_n2 = (gHDMI_N_Config[gPhy_Index].sample96_chg >> 8);
      audio.aud_n3_audn = 0x00;
    } else {
      audio.aud_n1 = (gHDMI_N_Config[gPhy_Index].sample96 & 0x00FF);
      audio.aud_n2 = (gHDMI_N_Config[gPhy_Index].sample96 >> 8);
      audio.aud_n3_audn = 0x00;
    }
    break;
  }

  audio.cts_manual = E_IM_HDMI_CTS_MANUAL_AUTO; //(0)

  // Set Audio Info.
  Im_HDMI_Set_Audio(&audio);

  return FJ_ERR_OK;
}

VOID BF_Disp_Clear_Addr(VOID) {
  UINT32 disp_id;
  U_IM_DISP_LALP lalp;
  UCHAR skip_flg = 0;

  Dd_ARM_Critical_Section_Start(gBF_Disp_Main_Spin_Lock);

  for (disp_id = 0; disp_id < D_BF_INSTANCE_ID_MAX_DISPLAY; disp_id++) {
    if (gBF_Video_enable_stat[disp_id] != FALSE) {
      // Skip address clear when Video is enable.
      skip_flg = 1;
    }
  }

  if (skip_flg == 0) {
    Im_DISP_Get_Alpha_Value(E_IM_DISP_HDMI, &lalp);
    if (lalp.bit.ALP == 0) {
      Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_HDMI,
                                                  D_IM_DISP_ENABLE_OFF);
      Dd_ARM_Dsb_Pou();

      disp_clear_addr(E_IM_DISP_HDMI);

      Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_HDMI,
                                                  D_IM_DISP_ENABLE_ON);
      Dd_ARM_Dsb_Pou();
    }

    Im_DISP_Get_Alpha_Value(E_IM_DISP_LCD_MIPI, &lalp);
    if (lalp.bit.ALP == 0) {
      Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_LCD_MIPI,
                                                  D_IM_DISP_ENABLE_OFF);
      Dd_ARM_Dsb_Pou();

      disp_clear_addr(E_IM_DISP_LCD_MIPI);

      Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_LCD_MIPI,
                                                  D_IM_DISP_ENABLE_ON);
      Dd_ARM_Dsb_Pou();
    }
  }

  Dd_ARM_Critical_Section_End(gBF_Disp_Main_Spin_Lock);
}

VOID BF_Disp_Instance_Print_Info(VOID) {
  UCHAR disp_id;
  E_IM_DISP_SEL block;
  UINT32 disp_status;
  UINT32 hdmi_status;
  UINT32 lcd_status;
  T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_out_trg_limit;
  T_IM_DISP_CTRL_OUTPUT ctrl_out;
  T_IM_DISP_CTRL_MAIN_LAYER_TRG_LIMIT ctrl_main_trg_limit;
  T_IM_DISP_CTRL_MAIN_LAYER ctrl_main;

  printf("<< Disp Instance Information >>\n");

  Im_DISP_Get_Status(&hdmi_status, &lcd_status);

  for (disp_id = 0; disp_id < D_BF_INSTANCE_ID_MAX_DISPLAY; disp_id++) {
    printf(" [Disp id : %d]", disp_id);
    if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_USE) {
      printf(" Active\n");
      if (gBF_Instance_Display[disp_id].connect_state ==
          E_BF_INSTANCE_CONNECT_STATE_ON) {
        if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
          block = E_IM_DISP_HDMI;
          disp_status = hdmi_status;
        } else {
          block = E_IM_DISP_LCD_MIPI;
          disp_status = lcd_status;
        }
        Im_DISP_Get_Ctrl_Output(block, &ctrl_out_trg_limit, &ctrl_out);
        Im_DISP_Get_Ctrl_Main_Layer(block, &ctrl_main_trg_limit, &ctrl_main);
        printf("  [Disp Window Info]\n");
        printf("    Disp type                  : %4d\n",
               gBF_Instance_Display[disp_id].type);
        printf("    Disp state                 : %4d\n",
               gBF_Instance_Display[disp_id].state);
        printf("    Disp rotate_degree         : %4d\n",
               gBF_Instance_Display[disp_id].rotate_degree);
        printf("    DispIn x_pos               : %4d\n",
               gBF_Instance_Display[disp_id].window.in_x_pos);
        printf("    DispIn x_pos               : %4d\n",
               gBF_Instance_Display[disp_id].window.in_y_pos);
        printf("    DispIn width               : %4d\n",
               gBF_Instance_Display[disp_id].window.input.width);
        printf("    DispIn lines               : %4d\n",
               gBF_Instance_Display[disp_id].window.input.lines);
        printf("    DispOut x_pos              : %4d\n",
               gBF_Instance_Display[disp_id].window.out_x_pos);
        printf("    DispOut y_pos              : %4d\n",
               gBF_Instance_Display[disp_id].window.out_y_pos);
        printf("    DispOut width              : %4d\n",
               gBF_Instance_Display[disp_id].window.output.width);
        printf("    DispOut lines              : %4d\n",
               gBF_Instance_Display[disp_id].window.output.lines);
        printf("  [Disp Config Info]\n");
        printf("   Disp Status                                 : 0x%08X\n",
               disp_status);
        printf("   Vsync polarity                              : %lu\n",
               (ULONG)ctrl_out_trg_limit.polsel.bit.VDPS);
        printf("   Hsync polarity                              : %lu\n",
               (ULONG)ctrl_out_trg_limit.polsel.bit.HDPS);
        printf("   Vertical enable signal polarity             : %lu\n",
               (ULONG)ctrl_out_trg_limit.polsel.bit.VEPS);
        printf("   Horizontal enable signal polarity           : %lu\n",
               (ULONG)ctrl_out_trg_limit.polsel.bit.HEPS);
        printf("   Output clock edge                           : %lu\n",
               (ULONG)ctrl_out_trg_limit.polsel.bit.CKEG);
        printf("   0:Progressive, 1:Interlace                  : %lu\n",
               ctrl_out_trg_limit.tsl);
        printf("   Vsync output 0.5H shift                     : %lu\n",
               (ULONG)ctrl_out_trg_limit.vcyc.bit.VDSFT);
        printf("   Vertical data size                          : %lu\n",
               ctrl_out_trg_limit.ovsize);
        printf("   Vertical cycle (odd)                        : %lu\n",
               (ULONG)ctrl_out_trg_limit.vcyc.bit.VCYC1);
        printf("   Vertical cycle (even) + 0.5H shift          : %lu\n",
               (ULONG)ctrl_out_trg_limit.vcyc.bit.VCYC2);
        printf("   Horizontal cycle                            : %lu\n",
               ctrl_out_trg_limit.hcyc);
        printf("   Vsync pulse width                           : %lu\n",
               (ULONG)ctrl_out_trg_limit.ovpw.bit.OVPW);
        printf("   Vsync pluse width unit [Line]               : %lu\n",
               (ULONG)ctrl_out_trg_limit.ovpw.bit.OVPWU);
        printf("   Horizontal signal pulse width               : %lu\n",
               ctrl_out_trg_limit.hpw);
        printf("   Vertical Blank(odd)                         : %lu\n",
               (ULONG)ctrl_out_trg_limit.vblk.bit.VBLK1);
        printf("   Vertical Blank(even)                        : %lu\n",
               (ULONG)ctrl_out_trg_limit.vblk.bit.VBLK2);
        printf("   Vsync output delay                          : %lu\n",
               ctrl_out_trg_limit.vdly);
        printf("   Horizontal Blank                            : %lu\n",
               ctrl_out_trg_limit.hblk);
        printf("   Hsync output delay                          : %lu\n",
               ctrl_out_trg_limit.hdly);
        printf("   Horizontal data size                        : %lu\n",
               ctrl_out_trg_limit.ohsize);
        printf("   Vertical enable signal assert timing        : %lu\n",
               ctrl_out_trg_limit.vrfctl);
        printf("   Asserted for the period of a vertical blank : %lu\n",
               ctrl_out_trg_limit.hrfctl);
        printf("   LCD Output mode                             : %lu\n",
               (ULONG)ctrl_out_trg_limit.domd.bit.MODE);
        printf("   Outputs by LSB stuff                        : %lu\n",
               (ULONG)ctrl_out_trg_limit.domd.bit.OBPSL);
        printf("   Outputs by 8 bit par 1 element              : %lu\n",
               (ULONG)ctrl_out_trg_limit.domd.bit.OBUSL);
        printf("   0.5pix shift output (Odd line)              : %lu\n",
               (ULONG)ctrl_out.doctl1.bit.SOO);
        printf("  [Disp Main Info]\n");
        printf("   Display enable                              : 0x%lu\n",
               (ULONG)ctrl_main.lalp.bit.ALP);
        printf("   Input address[0] of Y data                  : 0x%08lX\n",
               ctrl_main.laddr[0].y_addr);
        printf("   Input address[0] of CbCr data               : 0x%08lX\n",
               ctrl_main.laddr[0].c_addr);
        printf("   Input address[1] of Y data                  : 0x%08lX\n",
               ctrl_main.laddr[1].y_addr);
        printf("   Input address[1] of CbCr data               : 0x%08lX\n",
               ctrl_main.laddr[1].c_addr);
        printf("   Input address[2] of Y data                  : 0x%08lX\n",
               ctrl_main.laddr[2].y_addr);
        printf("   Input address[2] of CbCr data               : 0x%08lX\n",
               ctrl_main.laddr[2].c_addr);
        printf("   Input address[3] of Y data                  : 0x%08lX\n",
               ctrl_main.laddr[3].y_addr);
        printf("   Input address[3] of CbCr data               : 0x%08lX\n",
               ctrl_main.laddr[3].c_addr);
        printf("   Input bank specification                    : %u\n",
               ctrl_main.libctl_ibset);
        printf("   Horizontal global area of Y data            : %lu\n",
               ctrl_main.y_hga);
        printf("   Horizontal global area of CbCr data         : %lu\n",
               ctrl_main.c_hga);
        printf("   Horizontal display beginning position       : %lu\n",
               (ULONG)ctrl_main.ldsta.bit.DSH);
        printf("   Vertical display beginning position         : %lu\n",
               (ULONG)ctrl_main.ldsta.bit.DSV);
        printf("   Input data size width                       : %lu\n",
               (ULONG)ctrl_main.lisize.size.width);
        printf("   Input data size lines                       : %lu\n",
               (ULONG)ctrl_main.lisize.size.lines);
        printf("   Horizontal resize magnification(M)          : %lu\n",
               (ULONG)ctrl_main.lrsz1.bit.HRSZM);
        printf("   Horizontal resize magnification(N)          : %lu\n",
               (ULONG)ctrl_main.lrsz1.bit.HRSZN);
        printf("   Horizontal resize magnification(OF)         : %lu\n",
               (ULONG)ctrl_main.lrsz1.bit.HRSZOF);
        printf("   Vertical resize magnification(M)            : %lu\n",
               (ULONG)ctrl_main.lrsz2.bit.VRSZM);
        printf("   Vertical resize magnification(N)            : %lu\n",
               (ULONG)ctrl_main.lrsz2.bit.VRSZN);
        printf("   Vertical resize magnification(OF)           : %lu\n",
               (ULONG)ctrl_main.lrsz2.bit.VRSZOF);
        printf("   Input format selection                      : %lu\n",
               (ULONG)ctrl_main.lidt.bit.IFMT);
        printf("   gVideo_fmt                                  : %d\n",
               (UCHAR)gVideo_fmt);
        printf("\n");

        BF_Disp_OSD_Print_Info(disp_id);
      } else {
        printf(" Non Connect\n");
      }
    } else {
      printf(" Non Active\n");
    }
  }
}

/* ------------- */
/* LCD/HDMI      */
/* ------------- */
/* Open Display Instance. */
FJ_ERR_CODE fj_disp_open(FJ_DISP_TYPE disp_type, UCHAR *disp_id) {
  FJ_ERR_CODE fj_ret = FJ_ERR_NG;
  UCHAR inst_id;

  if (disp_id == NULL) {
    BF_Debug_Print_Error(("fj_disp_open() Error!!:disp_id is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  for (inst_id = 0; inst_id < D_BF_INSTANCE_ID_MAX_DISPLAY; inst_id++) {
    if (gBF_Instance_Display[inst_id].type == disp_type) {
      if (gBF_Instance_Display[inst_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
        gBF_Instance_Display[inst_id].flag = E_BF_INSTANCE_FLAG_USE;
        gBF_Instance_Display[inst_id].state = E_BF_INSTANCE_STATE_NORMAL;
        *disp_id = inst_id;
        fj_ret = FJ_ERR_OK;
        break;
      } else {
        BF_Debug_Print_Error(
            ("fj_disp_open() Error!!:Instance is opened. disp_type=%d\n",
             disp_type));
        break;
      }
    }
  }

  return fj_ret;
}

/* Close Display Instance. */
FJ_ERR_CODE fj_disp_close(UCHAR disp_id) {
  FJ_ERR_CODE fj_ret = FJ_ERR_OK;

  if (disp_id < D_BF_INSTANCE_ID_MAX_DISPLAY) {
    if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_USE) {
      gBF_Instance_Display[disp_id].flag = E_BF_INSTANCE_FLAG_UNUSE;
      gBF_Instance_Display[disp_id].state = E_BF_INSTANCE_STATE_NORMAL;
      memset(&gBF_Instance_Display[disp_id].window, 0,
             sizeof(gBF_Instance_Display[disp_id].window));
    }
  } else {
    BF_Debug_Print_Error(
        ("fj_disp_close() Error!!:disp_id(%d) is illegal.\n", disp_id));
    fj_ret = FJ_ERR_NG_INPUT_PARAM;
  }

  return fj_ret;
}

/* ------------- */
/* LCD           */
/* ------------- */
/* Set LCD timing generator to DISP macro. */
FJ_ERR_CODE fj_disp_lcd_config(UCHAR disp_id,
                               FJ_DISP_LCD_CONFIG *lcd_config_info) {
  FJ_ERR_CODE fj_ret = FJ_ERR_OK;
  INT32 im_ret;
  INT32 ret;
  T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_output_trglmt;
  T_IM_DISP_CTRL_OUTPUT ctrl_output;
  T_DISP_LCD_CLK_REG lcd_clk_reg;

  memset(&lcd_clk_reg, 0, sizeof(T_DISP_LCD_CLK_REG));

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_config() Error!!:disp_id(%d) is not opened.\n", disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gBF_Instance_Display[disp_id].type != FJ_DISP_TYPE_LCD) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_config() Error!!:disp_id(%d) is illegal.\n", disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (lcd_config_info == NULL) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_config() Error!!:lcd_config_info is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if ((lcd_config_info->lcd_clk < 6750) || (lcd_config_info->lcd_clk > 65000)) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_config() Error!!:lcd_clk(%d) is illegal.\n",
         lcd_config_info->lcd_clk));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (lcd_config_info->lcd_timing.hblk < 8) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_config() Error!!:hblk(%lu) is illegal.\n",
         lcd_config_info->lcd_timing.hblk));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Set LCD clock
  Dd_Top_Set_CLKSTOP3_LCDCK(1);
  fj_ret = lcd_clk_reg_search(&lcd_clk_reg,
                              (FLOAT)(lcd_config_info->lcd_clk * 1000));
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_config() Error!!:lcd_clk_reg_search() error. fj_ret=%x\n",
         fj_ret));
    return FJ_ERR_NG;
  }
  Dd_Top_Stop_Pll04();
  Dd_Top_Set_PLLCNT8_P04PREDIV(lcd_clk_reg.a);
  Dd_Top_Set_PLLCNT8_P04PLLDIV(lcd_clk_reg.n);
  Dd_Top_Set_PLLCNT8_P04POSTDIV(lcd_clk_reg.b);
  ret = Dd_Top_Start_Pll04();
  if (ret != D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  // Start LCD clock (for reset clear)
  Dd_Top_Set_CLKSTOP3_LCDCK(0);

  // DISP macro soft-reset (for display switch)
  Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE,
                 D_IM_DISP_SR_RESET);

  // Switch display to LCD
  fj_ret =
      BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_LCD, E_FJ_DISP_IFS_LCD);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_config() Error!!:BF_Osd_Attr_Set_Display_Interface() "
         "error. fj_ret=%x\n",
         fj_ret));
    return FJ_ERR_NG;
  }

  // DISP macro to clear soft-reset bit
  Im_DISP_Init();

  // Get control data of data output block
  Im_DISP_Get_Ctrl_Output(E_IM_DISP_LCD_MIPI, &ctrl_output_trglmt,
                          &ctrl_output);

  // Set control data of data output block
  ctrl_output_trglmt.tgkmd = E_IM_DISP_TGKMD_NORMAL; // External start-up mode
  ctrl_output_trglmt.tgkst = 0;                      // Operation start setting
  ctrl_output_trglmt.toctl.bit.CON =
      D_IM_DISP_CON_EXTERNAL; // Connects relay terminal of the chip
  ctrl_output_trglmt.toctl.bit.CKOEN =
      D_IM_DISP_ENABLE_ON; // External output clock permission
  ctrl_output_trglmt.toctl.bit.CKICEN =
      D_IM_DISP_ENABLE_OFF; // Internal output clock permission
  ctrl_output_trglmt.toctl.bit.WBEN =
      D_IM_DISP_ENABLE_OFF; // Write back permission
  ctrl_output_trglmt.rpgctl =
      E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP; // Register parameter taking control
  ctrl_output_trglmt.polsel.bit.VDPS =
      lcd_config_info->lcd_polarity.vsync; // Vsync polarity
  ctrl_output_trglmt.polsel.bit.HDPS =
      lcd_config_info->lcd_polarity.hsync; // Hsync polarity
  ctrl_output_trglmt.polsel.bit.VEPS =
      lcd_config_info->lcd_polarity.venable; // Vertical enable signal polarity
  ctrl_output_trglmt.polsel.bit.HEPS =
      lcd_config_info->lcd_polarity
          .henable; // Horizontal enable signal polarity
  ctrl_output_trglmt.polsel.bit.CKEG =
      lcd_config_info->lcd_polarity.clock; // Output clock edge
  ctrl_output_trglmt.tsl =
      lcd_config_info->lcd_timing.scan_mode; // 0:Progressive, 1:Interlace
  ctrl_output_trglmt.vcyc.bit.VCYC1 =
      lcd_config_info->lcd_timing.vcyc; // Vertical cycle (odd)
  if (lcd_config_info->lcd_timing.vcyc_shift) {
    ctrl_output_trglmt.vcyc.bit.VCYC2 = lcd_config_info->lcd_timing.vcyc +
                                        1; // Vertical cycle (even) + 0.5H shift
  } else {
    ctrl_output_trglmt.vcyc.bit.VCYC2 =
        lcd_config_info->lcd_timing.vcyc; // Vertical cycle (even)
  }
  ctrl_output_trglmt.vcyc.bit.VDSFT =
      lcd_config_info->lcd_timing.vcyc_shift; // Vsync output 0.5H shift
  ctrl_output_trglmt.hcyc =
      lcd_config_info->lcd_timing.hcyc; // Horizontal cycle
  ctrl_output_trglmt.ovpw.bit.OVPW =
      lcd_config_info->lcd_timing.vpw; // Vsync pulse width
  ctrl_output_trglmt.ovpw.bit.OVPWU =
      D_IM_DISP_OVPWU_LINE_UNIT; // Vsync pluse width unit [Line]
  ctrl_output_trglmt.hpw =
      lcd_config_info->lcd_timing.hpw; // Horizontal signal pulse width
  ctrl_output_trglmt.vblk.bit.VBLK1 =
      lcd_config_info->lcd_timing.vblk; // Vertical Blank(odd)
  if (lcd_config_info->lcd_timing.vcyc_shift) {
    ctrl_output_trglmt.vblk.bit.VBLK2 = lcd_config_info->lcd_timing.vblk +
                                        1; // Vertical Blank(even) + 0.5H shift
  } else {
    ctrl_output_trglmt.vblk.bit.VBLK2 =
        lcd_config_info->lcd_timing.vblk; // Vertical Blank(even)
  }
  ctrl_output_trglmt.vdly = 0; // Vsync output delay
  if (lcd_config_info->lcd_timing.hblk < 16) {
    ctrl_output_trglmt.hblk = 16; // Horizontal Blank
    ctrl_output_trglmt.hdly =
        16 - lcd_config_info->lcd_timing.hblk; // Hsync output delay
  } else {
    ctrl_output_trglmt.hblk =
        lcd_config_info->lcd_timing.hblk; // Horizontal Blank
    ctrl_output_trglmt.hdly = 0;          // Hsync output delay
  }
  ctrl_output_trglmt.ovsize =
      lcd_config_info->lcd_timing.vsize; // Vertical data size
  ctrl_output_trglmt.ohsize =
      lcd_config_info->lcd_timing.hsize; // Horizontal data size
  ctrl_output_trglmt.vrfctl =
      D_IM_DISP_VENS_HSYNC; // Vertical enable signal assert timing
  ctrl_output_trglmt.hrfctl =
      D_IM_DISP_HENS_ASSERT_OFF; // Asserted for the period of a vertical blank
  ctrl_output_trglmt.domd.bit.MODE =
      lcd_config_info->lcd_format.output_mode; // LCD Output mode
  ctrl_output_trglmt.domd.bit.OBPSL =
      D_IM_DISP_OBPSL_LSB;                    // Outputs by LSB stuff
  switch (ctrl_output_trglmt.domd.bit.MODE) { // Output bit unit selection
  case FJ_DISP_LCD_OUTPUT_MODE_RGB888_24BIT:
    ctrl_output_trglmt.domd.bit.OBUSL = D_IM_DISP_OBUSL_8BIT;
    break;
  case FJ_DISP_LCD_OUTPUT_MODE_YCC422_16BIT:
    ctrl_output_trglmt.domd.bit.OBUSL = D_IM_DISP_OBUSL_8BIT;
    break;
  default:
    ctrl_output_trglmt.domd.bit.OBUSL = D_IM_DISP_OBUSL_8BIT;
    break;
  }
  ctrl_output_trglmt.tblaset_gmtaen =
      E_IM_DISP_TABLE_ACCESS_DISABLE; // Gamma correct table access enable
  ctrl_output_trglmt.tblaset_ysatta =
      E_IM_DISP_TABLE_ACCESS_DISABLE; // Luminance and chroma correct table
                                      // access enable

  if (lcd_config_info->lcd_timing.scan_mode == 1) {
    ctrl_output.intc = D_IM_DISP_VFTMG_1ST; // Interruption control
  } else {
    ctrl_output.intc = D_IM_DISP_VFTMG_1ST_2ND; // Interruption control
  }
  ctrl_output.inte.bit.LEE =
      D_IM_DISP_ENABLE_ON; // Main data input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GR0EE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GR1EE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GA0EE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block A component forwarding
                           // error interruption
  ctrl_output.inte.bit.GA1EE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block A component forwarding
                           // error interruption
  ctrl_output.inte.bit.LREE =
      D_IM_DISP_ENABLE_ON; // Main data input block AXI error interruption
  ctrl_output.inte.bit.GR0REE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block AXI error interruption
  ctrl_output.inte.bit.GR1REE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block AXI error interruption
  ctrl_output.inte.bit.GA0REE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block A component AXI error
                           // interruption
  ctrl_output.inte.bit.GA1REE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block A component AXI error
                           // interruption
  ctrl_output.inte.bit.VE =
      D_IM_DISP_ENABLE_ON; // Vsync1 detection interruption
  ctrl_output.inte.bit.VE2 =
      D_IM_DISP_ENABLE_OFF; // Vsync2 detection interruption
  ctrl_output.hablk = 0;    // Horizontal blank adjustment setting
  ctrl_output.fdoen = D_IM_DISP_ENABLE_OFF; // Fource data output enable
  ctrl_output.fodata.word = 0;              // Fource output data
  ctrl_output.blankdt.word[0] = 0;          // Blank data setting register0
  ctrl_output.blankdt.word[1] = 0;          // Blank data setting register1
  ctrl_output.clbhsize =
      lcd_config_info->lcd_timing.hsize; // Color bar horizontal size
  ctrl_output.clbdt[0].word =
      0; // Color bar display color setting (Set to black)
  ctrl_output.bldctl.bit.BLD1 = 0; // Blend order 1 ayer Grid
  ctrl_output.bldctl.bit.BLD2 = 1; // Blend order 2 ayer Face
  ctrl_output.bldctl.bit.BLD3 = 2; // Blend order 3 ayer OSD0
  ctrl_output.bldctl.bit.BLD4 = 3; // Blend order 4 ayer OSD1
  ctrl_output.r2y[0].dword =
      0x0000075025901320; // RGB->YCbCr matrix (RGB->YCC601)
  ctrl_output.r2y[1].dword =
      0x00002000EAD0F530; // RGB->YCbCr matrix (RGB->YCC601)
  ctrl_output.r2y[2].dword =
      0x0000FAD0E5302000; // RGB->YCbCr matrix (RGB->YCC601)
  ctrl_output.clip_cal.y_cal.ygain = 0x00010000; // Y Gain value
  ctrl_output.clip_cal.y_cal.yofs = 0x00000000;  // Y Offset value
  ctrl_output.clip_cal.y_clip.cph = 0xFFFF;      // Y Clip upper bound value
  ctrl_output.clip_cal.y_clip.cpl = 0x0000;      // Y Clip lower bound value
  ctrl_output.clip_cal.cb_cal.cgain = 0x8000;    // Cb Gain value
  ctrl_output.clip_cal.cb_cal.cofs = 0x0000;     // Cb Offset value
  ctrl_output.clip_cal.cb_clip.cph = 0xFFFF;     // Cb Clip upper bound value
  ctrl_output.clip_cal.cb_clip.cpl = 0x0000;     // Cb Clip lower bound value
  ctrl_output.clip_cal.cr_cal.cgain = 0x8000;    // Cr Gain value
  ctrl_output.clip_cal.cr_cal.cofs = 0x0000;     // Cr Offset value
  ctrl_output.clip_cal.cr_clip.cph = 0xFFFF;     // Cr Clip upper bound value
  ctrl_output.clip_cal.cr_clip.cpl = 0x0000;     // Cr Clip lower bound value
  ctrl_output.doctl0.bit.DTO1 =
      lcd_config_info->lcd_format.output_order[0]; // LCD data output order 1
  ctrl_output.doctl0.bit.DTO2 =
      lcd_config_info->lcd_format.output_order[1]; // LCD data output order 2
  ctrl_output.doctl0.bit.DTO3 =
      lcd_config_info->lcd_format.output_order[2]; // LCD data output order 3
  ctrl_output.doctl0.bit.DTO4 =
      lcd_config_info->lcd_format.output_order[3]; // LCD data output order 4
  ctrl_output.doctl0.bit.DTO5 =
      lcd_config_info->lcd_format.output_order[4]; // LCD data output order 5
  ctrl_output.doctl0.bit.DTO6 =
      lcd_config_info->lcd_format.output_order[5]; // LCD data output order 6
  if (lcd_config_info->lcd_format.pix_fmt == FJ_DISP_LCD_PIX_FMT_DELTA) {
    ctrl_output.doctl1.bit.SOO =
        D_IM_DISP_ENABLE_ON; // 0.5pix shift output (Odd line)
  } else {
    ctrl_output.doctl1.bit.SOO =
        D_IM_DISP_ENABLE_OFF; // 0.5pix shift output (Odd line)
  }
  ctrl_output.doctl1.bit.SOE =
      D_IM_DISP_ENABLE_OFF; // 0.5pix shift output (Even line)
  ctrl_output.doctl1.bit.ODO0 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 0 (Odd line)
  ctrl_output.doctl1.bit.ODO1 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 1 (Odd line)
  ctrl_output.doctl1.bit.ODO2 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 2 (Odd line)
  ctrl_output.doctl1.bit.ODE0 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 0 (Even line)
  ctrl_output.doctl1.bit.ODE1 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 1 (Even line)
  ctrl_output.doctl1.bit.ODE2 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 2 (Even line)
  ctrl_output.doctl2.bit.CLVS = D_IM_DISP_CLVS_NORMAL; // CbCr data output level
  ctrl_output.doctl2.bit.CTOH =
      D_IM_DISP_CTOH_LEFT; // CbCr off (444->442 thin out)
  ctrl_output.doctl2.bit.CTOV =
      D_IM_DISP_CTOV_TOP; // CbCr off (444->442 thin out)
  ctrl_output.doctl2.bit.TRSOS = D_IM_DISP_TRSOS_NONE; // SAV/EAV code output
  //	ctrl_output.trscode[0]........Set the get value, because
  //D_IM_DISP_TRSOS_NONE.
  ctrl_output.y2r[0].dword =
      0x000059C000004000; // YCbCr->RGB matrix (YCC601->RGB)
  ctrl_output.y2r[1].dword =
      0x0000D250EA004000; // YCbCr->RGB matrix (YCC601->RGB)
  ctrl_output.y2r[2].dword =
      0x0000FFF071704000; // YCbCr->RGB matrix (YCC601->RGB)
  ctrl_output.tblaset_gmen =
      E_IM_DISP_CORRECT_PROC_DISABLE; // Gamma correction enable
  ctrl_output.tblaset_gmtsl =
      D_IM_DISP_TABLE_SURFACE_A; // Gamma correction table surface
  ctrl_output.tblaset_ysaten =
      E_IM_DISP_CORRECT_PROC_DISABLE; // Luminance and chroma correction enable
  ctrl_output.tblaset_ysatsl =
      D_IM_DISP_TABLE_SURFACE_A; // Luminance and chroma correction table
                                 // surface
  //	ctrl_output.face..............Set the get value, because it will be set
  //in OSD API. 	ctrl_output.int_callback[0]...Set the get value, because it will
  //be set in each BaseFW function.

  im_ret = Im_DISP_Ctrl_Output(E_IM_DISP_LCD_MIPI, &ctrl_output_trglmt,
                               &ctrl_output);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error((
        "fj_disp_lcd_config() Error!!:Im_DISP_Ctrl_Output() error. im_ret=%x\n",
        im_ret));
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

/* Start LCD clock(LCDCK), VSYNC output, image layer trigger. */
FJ_ERR_CODE fj_disp_lcd_start_sync(UCHAR disp_id) {
  INT32 im_ret;
  U_IM_DISP_LALP lalp;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_start_sync() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gBF_Instance_Display[disp_id].type != FJ_DISP_TYPE_LCD) {
    BF_Debug_Print_Error((
        "fj_disp_lcd_start_sync() Error!!:disp_id(%d) is illegal.\n", disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Start color bar layer
  im_ret =
      Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE, 0);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_lcd_start_sync() "
                          "Error!!:Im_DISP_Start(DCORE) error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  lalp.bit.ALP = 0x00;
  Im_DISP_Set_Alpha_Value(E_IM_DISP_LCD_MIPI, lalp);

  // Start image layer
  im_ret = Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_MAIN, 0);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_lcd_start_sync() "
                          "Error!!:Im_DISP_Start(MAIN) error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  gBF_Instance_Display[disp_id].connect_state = E_BF_INSTANCE_CONNECT_STATE_ON;

  return FJ_ERR_OK;
}

/* Stop LCD clock(LCDCK), VSYNC output, image layer trigger. */
FJ_ERR_CODE fj_disp_lcd_stop_sync(UCHAR disp_id) {
  FJ_ERR_CODE fj_ret;
  INT32 im_ret;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_stop_sync() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gBF_Instance_Display[disp_id].type != FJ_DISP_TYPE_LCD) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_stop_sync() Error!!:disp_id(%d) is illegal.\n", disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Stop DISP trigger
  im_ret = Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_ALL, 0,
                        E_IM_DISP_STOP_TYPE_WITH_WAIT);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_stop_sync() Error!!:Im_DISP_Stop(all) error. im_ret=%x\n",
         im_ret));
    return FJ_ERR_NG;
  }
  Im_DISP_Wait_Stop(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE);

  // Stop LCD clock
  Dd_Top_Set_CLKSTOP3_LCDCK(1);

  // DISP macro soft-reset (for display switch)
  Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE,
                 D_IM_DISP_SR_RESET);

  // Switch display to None
  fj_ret =
      BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_LCD, E_FJ_DISP_IFS_NONE);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_stop_sync() Error!!:BF_Osd_Attr_Set_Display_Interface() "
         "error. fj_ret=%x\n",
         fj_ret));
    return FJ_ERR_NG;
  }

  gBF_Instance_Display[disp_id].connect_state = E_BF_INSTANCE_CONNECT_STATE_OFF;

  return FJ_ERR_OK;
}

FJ_DISP_LCD_CONNECT_STATE fj_disp_lcd_get_status(VOID) {
  FJ_DISP_LCD_CONNECT_STATE ret = FJ_DISP_LCD_CONNECT_STATE_OFF;

  if (gBF_Instance_Display[FJ_DISP_TYPE_LCD].connect_state ==
      E_BF_INSTANCE_CONNECT_STATE_ON) {
    ret = FJ_DISP_LCD_CONNECT_STATE_ON;
  }

  return ret;
}

/* ------------------- */
/* HDMI				   */
/* ------------------- */
/* Regist callback function pointer for hotplug detection. */
FJ_ERR_CODE fj_disp_hdmi_set_detect_callback(
    DISP_HDMI_CALLBACK detect_callback,
    FJ_DISP_HDMI_DETECT_SETTING const *const detect_setting) {
  DISP_HDMI_CALLBACK current_callback = *gDisp_HDMI_Callback;

  if (detect_callback == NULL) {
    BF_Debug_Print_Error(("fj_disp_hdmi_set_detect_callback() "
                          "Error!!:interrupt_callback is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (detect_setting == NULL) {
    BF_Debug_Print_Error(("fj_disp_hdmi_set_detect_callback() "
                          "Error!!:detect_setting is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (detect_setting->detect_count > detect_setting->check_count) {
    BF_Debug_Print_Error(("fj_disp_hdmi_set_detect_callback() "
                          "Error!!:detect_setting count value is illegal.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  gDisp_HDMI_Callback = detect_callback;

  gDisp_HDMI_Detect_Setting = *detect_setting;

  if (current_callback == NULL) {
    // Stop HIF clock (for set CLKSEL)
    Dd_Top_Set_CLKSTOP3_HIFCK(1);
    // HIF clock = 594MHZ.
    Dd_Top_Set_CLKSEL2_HIFCLK(E_BF_OSD_GENERAL_CLK_SEL_594);
    // Start HIF clock (for reset clear)
    Dd_Top_Set_CLKSTOP3_HIFCK(0);

    // reason for setting the 2 times becomes abnormal to
    // IO_HDMI.HDMI_PAGE0.PB_CTRL2 in Dd_HDMI_Set_Tx().
    FJ_HDMI_Init(disp_hdmi_interrupt_callback);
  }

  return FJ_ERR_OK;
}

FJ_DISP_HDMI_DETECT fj_disp_hdmi_get_detect_status(VOID) {
  U_IM_HDMI_PHY_STAT0 status;
  FJ_DISP_HDMI_DETECT ret;
  UINT32 check_signal;

  if (gDisp_HDMI_Detect_Setting.check_signal ==
      FJ_DISP_HDMI_CHECK_SIGNAL_HPD_RX_SENSE) {
    check_signal = D_DISP_HDMI_STAT_HPD_RX_SENSE;
  } else {
    check_signal = D_DISP_HDMI_STAT_HPD;
  }

  Im_HDMI_Get_Phy_Status(&status);

  if ((status.byte & check_signal) == check_signal) {
    ret = FJ_DISP_HDMI_DETECT_CONNECTED;
  } else {
    ret = FJ_DISP_HDMI_DETECT_DISCONNECTED;
  }

  return ret;
}

/* Get HDMI Sinc information. */
FJ_ERR_CODE fj_disp_hdmi_get_sink(UCHAR disp_id,
                                  FJ_DISP_HDMI_SINK_INFO *sink_info) {
  FJ_HDMI_DTD_MAN *dtd_man;
  FJ_HDMI_EDID_DTD *edid_dtd = NULL;
  FJ_HDMI_HPD get_edid = FJ_HDMI_HPD_DETACH;
  UINT32 dtd_num;
  ULONG short_video_type_only = FJ_HDMI_FORMAT_NO;
  ULONG short_video_type_id;
  USHORT bit_num;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_get_sink() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (sink_info == NULL) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_get_sink() Error!!:sink_info is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  get_edid = FJ_HDMI_Get_EDID(&dtd_man, FJ_HDMI_EDID_TAG_FLAG_ALL);
  if (get_edid != FJ_HDMI_HPD_ATTACH) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_get_sink()  Error!!:get_edid is not attach.\n"));
    return FJ_ERR_NG;
  }

  for (dtd_num = 0; dtd_num < dtd_man->dtd_num; dtd_num++) {
    edid_dtd = (FJ_HDMI_EDID_DTD *)(dtd_man)->dtd_addr[dtd_num];

    sink_info->vic_info[dtd_num].video_id =
        (FJ_DISP_VIDEO_ID)edid_dtd->VideoIDCode;
    sink_info->vic_info[dtd_num].h_active = edid_dtd->H_Active;
    sink_info->vic_info[dtd_num].v_active = edid_dtd->V_Active;
    sink_info->vic_info[dtd_num].h_total =
        edid_dtd->H_Active + edid_dtd->H_Blanking;
    sink_info->vic_info[dtd_num].h_blank = edid_dtd->H_Blanking;
    sink_info->vic_info[dtd_num].v_total =
        edid_dtd->V_Active + edid_dtd->V_Blanking;
    sink_info->vic_info[dtd_num].v_blank = edid_dtd->V_Blanking;
    sink_info->vic_info[dtd_num].h_sync_offset = edid_dtd->H_SyncOffset;
    sink_info->vic_info[dtd_num].v_sync_offset = edid_dtd->V_SyncOffset;
    sink_info->vic_info[dtd_num].h_pulse_width = edid_dtd->H_SyncPulseWidth;
    sink_info->vic_info[dtd_num].v_pulse_width = edid_dtd->V_SyncPulseWidth;
    sink_info->vic_info[dtd_num].h_image_size = edid_dtd->H_ImageSize;
    sink_info->vic_info[dtd_num].v_image_size = edid_dtd->V_ImageSize;
    sink_info->vic_info[dtd_num].pixel_freq = edid_dtd->PixelClock;
    sink_info->vic_info[dtd_num].format_flags = edid_dtd->Flags;
    sink_info->vic_info[dtd_num].phy_index = edid_dtd->Phy_Index;
  }

  short_video_type_only = BF_HDMI_Get_Short_Video_Type();
  if (short_video_type_only != FJ_HDMI_FORMAT_NO) {
    for (bit_num = 0; bit_num < D_HDMI_VIDEO_FORMAT_MAX; bit_num++) {
      short_video_type_id =
          short_video_type_only & (FJ_HDMI_FORMAT_NO | (1 << bit_num));
      if (short_video_type_id != FJ_HDMI_FORMAT_NO) {
        if (fj_hdmi_get_format_tbl_addr(short_video_type_id, &edid_dtd) ==
            FJ_ERR_OK) {
          sink_info->vic_info[dtd_num].video_id =
              (FJ_DISP_VIDEO_ID)edid_dtd->VideoIDCode;
          sink_info->vic_info[dtd_num].h_active = edid_dtd->H_Active;
          sink_info->vic_info[dtd_num].v_active = edid_dtd->V_Active;
          sink_info->vic_info[dtd_num].h_total =
              edid_dtd->H_Active + edid_dtd->H_Blanking;
          sink_info->vic_info[dtd_num].h_blank = edid_dtd->H_Blanking;
          sink_info->vic_info[dtd_num].v_total =
              edid_dtd->V_Active + edid_dtd->V_Blanking;
          sink_info->vic_info[dtd_num].v_blank = edid_dtd->V_Blanking;
          sink_info->vic_info[dtd_num].h_sync_offset = edid_dtd->H_SyncOffset;
          sink_info->vic_info[dtd_num].v_sync_offset = edid_dtd->V_SyncOffset;
          sink_info->vic_info[dtd_num].h_pulse_width =
              edid_dtd->H_SyncPulseWidth;
          sink_info->vic_info[dtd_num].v_pulse_width =
              edid_dtd->V_SyncPulseWidth;
          sink_info->vic_info[dtd_num].h_image_size = edid_dtd->H_ImageSize;
          sink_info->vic_info[dtd_num].v_image_size = edid_dtd->V_ImageSize;
          sink_info->vic_info[dtd_num].pixel_freq = edid_dtd->PixelClock;
          sink_info->vic_info[dtd_num].format_flags = edid_dtd->Flags;
          sink_info->vic_info[dtd_num].phy_index = edid_dtd->Phy_Index;
          dtd_num++;
        }
      }
    }
  }

  sink_info->vic_info_num = dtd_num;

  gDisp_HDMI_Sink_Info = sink_info;
  gDisp_HDMI_DTD_Man = dtd_man;

  BF_Debug_Print_Information(("fj_disp_hdmi_get_sink() vic_info_num=%d\n",
                              gDisp_HDMI_Sink_Info->vic_info_num));

  return FJ_ERR_OK;
}

/* Get HDMI Sinc Vendor ID and Product Code. */
FJ_ERR_CODE fj_disp_hdmi_get_devicevendorproductid(
    UCHAR disp_id, FJ_DISP_HDMI_DEV_VENDOR_PRODUCT_ID *vendor_product_id) {
  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(("fj_disp_hdmi_get_devicevendorproductid() "
                          "Error!!:disp_id(%d) is not opened.\n",
                          disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (vendor_product_id == NULL) {
    BF_Debug_Print_Error(("fj_disp_hdmi_get_devicevendorproductid() "
                          "Error!!:vendor_product_id is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  BF_HDMI_Get_Vnder_ID(vendor_product_id);
  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_disp_hdmi_set_vic_info(UCHAR disp_id,
                                      FJ_DISP_HDMI_VIC_INFO *vic_info) {

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_set_vic_info() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (vic_info == NULL) {
    memset(&gDisp_HDMI_Vic_Info, 0x00, sizeof(gDisp_HDMI_Vic_Info));
  } else {
    gDisp_HDMI_Vic_Info.VideoIDCode = vic_info->video_id;
    gDisp_HDMI_Vic_Info.PixelClock = vic_info->pixel_freq;
    gDisp_HDMI_Vic_Info.H_Active = vic_info->h_active;
    gDisp_HDMI_Vic_Info.V_Active = vic_info->v_active;
    gDisp_HDMI_Vic_Info.H_Blanking = vic_info->h_blank;
    gDisp_HDMI_Vic_Info.V_Blanking = vic_info->v_blank;
    gDisp_HDMI_Vic_Info.H_SyncOffset = vic_info->h_sync_offset;
    gDisp_HDMI_Vic_Info.V_SyncOffset = vic_info->v_sync_offset;
    gDisp_HDMI_Vic_Info.H_SyncPulseWidth = vic_info->h_pulse_width;
    gDisp_HDMI_Vic_Info.V_SyncPulseWidth = vic_info->v_pulse_width;
    gDisp_HDMI_Vic_Info.H_ImageSize = vic_info->h_image_size;
    gDisp_HDMI_Vic_Info.V_ImageSize = vic_info->v_image_size;
    gDisp_HDMI_Vic_Info.Flags = vic_info->format_flags;
    gDisp_HDMI_Vic_Info.Phy_Index = vic_info->phy_index;

#if 0
		printf("  VIC:VideoIDCode	   = %d\n", gDisp_HDMI_Vic_Info.VideoIDCode );
		printf("  VIC:PixelClock	   = %d\n", gDisp_HDMI_Vic_Info.PixelClock );
		printf("  VIC:H_Active		   = %d\n", gDisp_HDMI_Vic_Info.H_Active );
		printf("  VIC:V_Active		   = %d\n", gDisp_HDMI_Vic_Info.V_Active );
		printf("  VIC:H_Blanking	   = %d\n", gDisp_HDMI_Vic_Info.H_Blanking );
		printf("  VIC:V_Blanking	   = %d\n", gDisp_HDMI_Vic_Info.V_Blanking );
		printf("  VIC:H_SyncOffset	   = %d\n", gDisp_HDMI_Vic_Info.H_SyncOffset );
		printf("  VIC:V_SyncOffset	   = %d\n", gDisp_HDMI_Vic_Info.V_SyncOffset );
		printf("  VIC:H_SyncPulseWidth = %d\n", gDisp_HDMI_Vic_Info.H_SyncPulseWidth );
		printf("  VIC:V_SyncPulseWidth = %d\n", gDisp_HDMI_Vic_Info.V_SyncPulseWidth );
		printf("  VIC:H_ImageSize	   = %d\n", gDisp_HDMI_Vic_Info.H_ImageSize );
		printf("  VIC:V_ImageSize	   = %d\n", gDisp_HDMI_Vic_Info.V_ImageSize );
		printf("  VIC:Flags 		   = %x\n", gDisp_HDMI_Vic_Info.Flags );
		printf("  VIC:Phy_Index 	   = %d\n", gDisp_HDMI_Vic_Info.Phy_Index );
#endif
  }

  return FJ_ERR_OK;
}
/* Set HDMI Source Product Description (SPD) InfoFrame. */
FJ_ERR_CODE fj_disp_hdmi_set_spd(UCHAR disp_id,
                                 FJ_DISP_HDMI_SPD_INFO *spd_info) {
  T_IM_HDMI_FC_SPD spd_data;
  INT32 im_ret;
  INT32 loop;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_set_spd() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (spd_info == NULL) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_set_spd() Error!!:spd_info is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  memset(&spd_data, 0x00, sizeof(spd_data));

  for (loop = 0; loop < 8; ++loop) { // Vendor Name
    spd_data.fc_spdvendorname[loop] = spd_info->vendor_id[loop];
  }

  for (loop = 0; loop < 16; ++loop) { // Product Name
    spd_data.fc_spdproductname[loop] = spd_info->product_code[loop];
  }

  // Source Product Descriptor
  spd_data.fc_spddeviceinf = spd_info->device_type;

  im_ret = Im_HDMI_Set_Frame_Composer_Spd(&spd_data);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_set_spd() Error!!:Im_HDMI_Set_Frame_Composer_Spd() "
         "error. im_ret=%x\n",
         im_ret));
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

/* Set HDMI Vendor Specific InfoFrame. */
FJ_ERR_CODE fj_disp_hdmi_set_vendor_specific(
    UCHAR disp_id, FJ_DISP_HDMI_VENDOR_SPEC_INFO *vendor_spec_info) {
  INT32 im_ret;
  T_IM_HDMI_FC_VSD vsd_data;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(("fj_disp_hdmi_set_vendor_specific() "
                          "Error!!:disp_id(%d) is not opened.\n",
                          disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (vendor_spec_info == NULL) {
    BF_Debug_Print_Error(("fj_disp_hdmi_set_vendor_specific() "
                          "Error!!:vendor_spec_info is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  memset(&vsd_data, 0x00, sizeof(T_IM_HDMI_FC_VSD));

  // IEEE ID (from HDMI Specification1.4)
  vsd_data.fc_vsdieeeid[0] = 0x00;
  vsd_data.fc_vsdieeeid[1] = 0x0C;
  vsd_data.fc_vsdieeeid[2] = 0x03;

  // VSI Packet Data Payload.
  vsd_data.fc_vsdpayload[0] = vendor_spec_info->video_format; // Video Format.
  if (vendor_spec_info->video_format ==
      FJ_DISP_HDMI_VENDOR_SPEC_VIDEO_FORMAT_EX) {
    vsd_data.fc_vsdpayload[1] =
        vendor_spec_info
            ->vic; // HDMI proprietary Video Format Identification Code
  } else if (vendor_spec_info->video_format ==
             FJ_DISP_HDMI_VENDOR_SPEC_VIDEO_FORMAT_3D) {
    vsd_data.fc_vsdpayload[1] = vendor_spec_info->structure_3d; // 3D Structure
  } else {
    vsd_data.fc_vsdpayload[1] = 0x00;
  }
  vsd_data.fc_vsdpayload[2] = 0x00; // Extension Data

  // Data size (IEEE + Payload = 6).
  vsd_data.fc_vsdsize = 0x06;

  im_ret = Im_HDMI_Set_Frame_Composer_Vsd(&vsd_data);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_set_vendor_specific() "
         "Error!!:Im_HDMI_Set_Frame_Composer_Vsd() error. im_ret=%x\n",
         im_ret));
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

/* Configure Video format. */
FJ_ERR_CODE fj_disp_hdmi_config_video(UCHAR disp_id, FJ_DISP_VIDEO_ID video_id,
                                      UCHAR freq_ctrl) {
  FJ_ERR_CODE fj_ret = FJ_ERR_NG;
  INT32 im_ret;
  UINT32 video_format_id;
  BOOL found = FALSE;
  E_HD_RESOLUTION hd_resolution;
  FJ_HDMI_EDID_DTD *edid_dtd = NULL;
  USHORT v_blanking_2nd;
  ULONG tmp_hblk;
  ULONG index;
  T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_output_trglmt;
  T_IM_DISP_CTRL_OUTPUT ctrl_output;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_config_video() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (video_id >= FJ_DISP_VIDEO_ID_MAX) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_config_video() Error!!:video_id is illegal.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (freq_ctrl > FJ_DISP_HDMI_FREQ_CTRL_ACTIVE) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_config_video() Error!!:freq_ctrl is illegal.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gDisp_HDMI_Vic_Info.VideoIDCode == 0) {
    // search video ID
    BF_HDMI_Get_Format_Tbl_Top_Addr(&edid_dtd);
    for (index = 0; index < D_HDMI_VIDEO_FORMAT_MAX; index++) {
      if (video_id == edid_dtd->VideoIDCode) {
        found = TRUE;
        break;
      }
      edid_dtd++;
    }

    if (!found) {
      BF_Debug_Print_Error(
          ("fj_disp_hdmi_config_video() Error!!:video_id(%d) is not support.\n",
           video_id));
      return FJ_ERR_NO_SUPPORT;
    }
  } else {
    // set video information from EDID
    edid_dtd = (FJ_HDMI_EDID_DTD *)&gDisp_HDMI_Vic_Info;
  }

  tmp_hblk = edid_dtd->H_Blanking - edid_dtd->H_SyncOffset; /* pgr0060 */
  if (tmp_hblk < 8) {
    BF_Debug_Print_Error(
        ("fj_disp_lcd_config() Error!!:hblk(%lu) is illegal.\n", tmp_hblk));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // convert form FJ_DISP_VIDEO_ID to FJ_HDMI_XXX
  fj_ret = disp_hdmi_convert_video_id_2_video_format_id(
      video_id, &video_format_id, &hd_resolution);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(("fj_disp_hdmi_config_video() "
                          "Error!!:disp_hdmi_convert_video_id_2_video_format_"
                          "id() error. fj_ret=%x\n",
                          fj_ret));
    return fj_ret;
  }

  // Stop HIF clock (for set CLKSEL)
  Dd_Top_Set_CLKSTOP3_HIFCK(1);

  // Set P05CHG.
  if (Dd_Top_Get_PLLCNT9_P05CHG() != freq_ctrl) {
    Dd_Top_Stop_Pll05A();
    Dd_Top_Set_PLLCNT9_P05CHG(freq_ctrl);
    fj_ret = Dd_Top_Start_Pll05A();
    if (fj_ret != D_DDIM_OK) {
      return FJ_ERR_NG;
    }
  }

  // Change HIF clock
  BF_Osd_Config_Comm_Set_HDMI_Clock(hd_resolution);

  // Start HIF clock (guard for FJ_HDMI_Connect_HDMI())
  Dd_Top_Set_CLKSTOP3_HIFCK(0);

  // DISP macro soft-reset (for display switch)
  Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE,
                 D_IM_DISP_SR_RESET);

  // Switch display to HDMI
  fj_ret =
      BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_HDMI, E_FJ_DISP_IFS_HDMI);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_config_video() "
         "Error!!:BF_Osd_Attr_Set_Display_Interface() error. fj_ret=%x\n",
         fj_ret));
    return FJ_ERR_NG;
  }

  // Initialize DISP macro to clear soft-reset bit
  Im_DISP_Init();

  // Get control data of data output block
  Im_DISP_Get_Ctrl_Output(E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output);

  // Set control data of data output block
  ctrl_output_trglmt.tgkmd = E_IM_DISP_TGKMD_NORMAL; // External start-up mode
  ctrl_output_trglmt.tgkst = 0;                      // Operation start setting
  ctrl_output_trglmt.toctl.bit.CON =
      D_IM_DISP_CON_INTERNAL; // Connects relay terminal of the chip
  ctrl_output_trglmt.toctl.bit.CKOEN =
      D_IM_DISP_ENABLE_OFF; // External output clock permission
  ctrl_output_trglmt.toctl.bit.CKICEN =
      D_IM_DISP_ENABLE_ON; // Internal output clock permission
  ctrl_output_trglmt.toctl.bit.WBEN =
      D_IM_DISP_ENABLE_OFF; // Write back permission
  ctrl_output_trglmt.rpgctl =
      E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP; // Register parameter taking control
  if (edid_dtd->Flags & FJ_DISP_HDMI_FLAG_BIT_MAP_VSYNC) {
    ctrl_output_trglmt.polsel.bit.VDPS =
        D_IM_DISP_POLARITY_POSITIVE; // Vsync polarity
  } else {
    ctrl_output_trglmt.polsel.bit.VDPS =
        D_IM_DISP_POLARITY_NEGATIVE; // Vsync polarity
  }
  if (edid_dtd->Flags & FJ_DISP_HDMI_FLAG_BIT_MAP_HSYNC) {
    ctrl_output_trglmt.polsel.bit.HDPS =
        D_IM_DISP_POLARITY_POSITIVE; // Hsync polarity
  } else {
    ctrl_output_trglmt.polsel.bit.HDPS =
        D_IM_DISP_POLARITY_NEGATIVE; // Hsync polarity
  }
  ctrl_output_trglmt.polsel.bit.VEPS = 0; // Vertical enable signal polarity
  ctrl_output_trglmt.polsel.bit.HEPS = 0; // Horizontal enable signal polarity
  ctrl_output_trglmt.polsel.bit.CKEG = 0; // Output clock edge

  if (edid_dtd->Flags & FJ_DISP_HDMI_FLAG_BIT_MAP_INTERLACE) {
    ctrl_output_trglmt.tsl =
        D_IM_DISP_TSL_INTERLACE; // 0:Progressive, 1:Interlace
    ctrl_output_trglmt.vcyc.bit.VDSFT =
        D_IM_DISP_VDSFT_SHIFT; // Vsync output 0.5H shift
    ctrl_output_trglmt.ovsize = edid_dtd->V_Active * 2; // Vertical data size
    v_blanking_2nd = edid_dtd->V_Blanking + 1;
  } else {
    ctrl_output_trglmt.tsl =
        D_IM_DISP_TSL_PROGRESSIVE; // 0:Progressive, 1:Interlace
    ctrl_output_trglmt.vcyc.bit.VDSFT =
        D_IM_DISP_VDSFT_NORMAL;                     // Vsync output 0.5H shift
    ctrl_output_trglmt.ovsize = edid_dtd->V_Active; // Vertical data size
    v_blanking_2nd = edid_dtd->V_Blanking;
  }
  ctrl_output_trglmt.vcyc.bit.VCYC1 =
      edid_dtd->V_Active + edid_dtd->V_Blanking; // Vertical cycle (odd)
  ctrl_output_trglmt.vcyc.bit.VCYC2 =
      edid_dtd->V_Active + v_blanking_2nd; // Vertical cycle (even) + 0.5H shift
  ctrl_output_trglmt.ovpw.bit.OVPW =
      edid_dtd->V_SyncPulseWidth; // Vsync pulse width
  ctrl_output_trglmt.ovpw.bit.OVPWU =
      D_IM_DISP_OVPWU_LINE_UNIT; // Vsync pluse width unit [Line]
  ctrl_output_trglmt.vblk.bit.VBLK1 =
      edid_dtd->V_Blanking - edid_dtd->V_SyncOffset; // Vertical Blank(odd)
  ctrl_output_trglmt.vblk.bit.VBLK2 =
      v_blanking_2nd - edid_dtd->V_SyncOffset; // Vertical Blank(even)
  ctrl_output_trglmt.vdly = 0;                 // Vsync output delay

  if (((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (video_id == FJ_DISP_VIDEO_ID_2160p_3840)) ||
      ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (video_id == FJ_DISP_VIDEO_ID_2160p50_3840))) {
    ctrl_output_trglmt.hcyc =
        (edid_dtd->H_Active + edid_dtd->H_Blanking) / 2; // Horizontal cycle
    ctrl_output_trglmt.hpw =
        edid_dtd->H_SyncPulseWidth / 2;     // Horizontal signal pulse width
    ctrl_output_trglmt.hblk = tmp_hblk / 2; // Horizontal Blank
    ctrl_output_trglmt.hdly = 0;            // Hsync output delay
  } else {
    ctrl_output_trglmt.hcyc =
        edid_dtd->H_Active + edid_dtd->H_Blanking; // Horizontal cycle
    ctrl_output_trglmt.hpw =
        edid_dtd->H_SyncPulseWidth; // Horizontal signal pulse width
    if (tmp_hblk < 16) {
      ctrl_output_trglmt.hblk = 16;            // Horizontal Blank
      ctrl_output_trglmt.hdly = 16 - tmp_hblk; // Hsync output delay
    } else {
      ctrl_output_trglmt.hblk = tmp_hblk; // Horizontal Blank
      ctrl_output_trglmt.hdly = 0;        // Hsync output delay
    }
  }
  ctrl_output_trglmt.ohsize = edid_dtd->H_Active; // Horizontal data size

  ctrl_output_trglmt.vrfctl =
      D_IM_DISP_VENS_H_ENABLE; // Vertical enable signal assert timing
  ctrl_output_trglmt.hrfctl =
      D_IM_DISP_HENS_ASSERT_OFF; // Asserted for the period of a vertical blank

  if (((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (video_id == FJ_DISP_VIDEO_ID_2160p_3840)) ||
      ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (video_id == FJ_DISP_VIDEO_ID_2160p50_3840))) {
    ctrl_output_trglmt.domd.bit.MODE =
        D_IM_DISP_MODE_YCC420_24BIT; // HDMI Output mode
  } else if (gDisp_HDMI_DTD_Man->dev_inf == FJ_HDMI_DISPLAY_INTERFACE_DVI) {
    ctrl_output_trglmt.domd.bit.MODE =
        D_IM_DISP_MODE_RGB888_24BIT; // HDMI Output mode
  }
#ifdef CO_HDMI_COMPLIANCE_TEST
  else if (gRS_System_Disp_Target_RGB) {
    ctrl_output_trglmt.domd.bit.MODE =
        D_IM_DISP_MODE_RGB888_24BIT; // HDMI Output mode
  }
#endif
  else {
    ctrl_output_trglmt.domd.bit.MODE =
        D_IM_DISP_MODE_YCC422_16BIT; // HDMI Output mode
  }
  ctrl_output_trglmt.domd.bit.OBPSL =
      D_IM_DISP_OBPSL_MSB; // Outputs by MSB stuff
  ctrl_output_trglmt.domd.bit.OBUSL =
      D_IM_DISP_OBUSL_16BIT; // Outputs by 16bit unit

  if (edid_dtd->Flags & FJ_DISP_HDMI_FLAG_BIT_MAP_INTERLACE) {
    ctrl_output.intc = D_IM_DISP_VFTMG_1ST; // Interruption control
  } else {
    ctrl_output.intc = D_IM_DISP_VFTMG_1ST_2ND; // Interruption control
  }
  ctrl_output.inte.bit.LEE =
      D_IM_DISP_ENABLE_ON; // Main data input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GR0EE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GR1EE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GA0EE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block A component forwarding
                           // error interruption
  ctrl_output.inte.bit.GA1EE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block A component forwarding
                           // error interruption
  ctrl_output.inte.bit.LREE =
      D_IM_DISP_ENABLE_ON; // Main data input block AXI error interruption
  ctrl_output.inte.bit.GR0REE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block AXI error interruption
  ctrl_output.inte.bit.GR1REE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block AXI error interruption
  ctrl_output.inte.bit.GA0REE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block A component AXI error
                           // interruption
  ctrl_output.inte.bit.GA1REE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block A component AXI error
                           // interruption
  ctrl_output.inte.bit.VE =
      D_IM_DISP_ENABLE_ON; // Vsync1 detection interruption
  ctrl_output.inte.bit.VE2 =
      D_IM_DISP_ENABLE_OFF;                 // Vsync2 detection interruption
  ctrl_output.fdoen = D_IM_DISP_ENABLE_OFF; // Fource data output enable
  ctrl_output.fodata.word = 0;              // Fource output data
  ctrl_output.blankdt.word[0] = 0;          // Blank data setting register0
  ctrl_output.blankdt.word[1] = 0;          // Blank data setting register1
#ifdef CO_HDMI_COMPLIANCE_TEST
  ctrl_output.clbhsize = edid_dtd->H_Active / 16; // Color bar horizontal size
  ctrl_output.clbdt[0].word =
      0x000000FF; // Color bar display color setting (Set to black)
  ctrl_output.clbdt[1].word = 0x0000FF00;
  ctrl_output.clbdt[2].word = 0x00FF0000;
  ctrl_output.clbdt[3].word = 0x0000FFFF;
  ctrl_output.clbdt[4].word = 0x00FFFF00;
  ctrl_output.clbdt[5].word = 0x00FF00FF;
  ctrl_output.clbdt[6].word = 0x00000000;
  ctrl_output.clbdt[7].word = 0x00808080;
  ctrl_output.clbdt[8].word = 0x00FFFFFF;
  ctrl_output.clbdt[9].word = 0x00800000;
  ctrl_output.clbdt[10].word = 0x00008000;
  ctrl_output.clbdt[11].word = 0x00000080;
  ctrl_output.clbdt[12].word = 0x00008080;
  ctrl_output.clbdt[13].word = 0x00808000;
  ctrl_output.clbdt[14].word = 0x00800080;
  ctrl_output.clbdt[15].word = 0x00FF80FF;
#else
  ctrl_output.clbhsize = edid_dtd->H_Active; // Color bar horizontal size
  ctrl_output.clbdt[0].word =
      0; // Color bar display color setting (Set to black)
#endif
  ctrl_output.bldctl.bit.BLD1 = 0; // Blend order 1 ayer Grid
  ctrl_output.bldctl.bit.BLD2 = 1; // Blend order 2 ayer Face
  ctrl_output.bldctl.bit.BLD3 = 2; // Blend order 3 ayer OSD0
  ctrl_output.bldctl.bit.BLD4 = 3; // Blend order 4 ayer OSD1
  if (ctrl_output_trglmt.ovsize < 720) {
    ctrl_output.r2y[0].dword =
        0x0000075025901320; // RGB->YCbCr matrix (RGB->YCC601)
    ctrl_output.r2y[1].dword =
        0x00002000EAD0F530; // RGB->YCbCr matrix (RGB->YCC601)
    ctrl_output.r2y[2].dword =
        0x0000FAD0E5302000; // RGB->YCbCr matrix (RGB->YCC601)
  } else {
    ctrl_output.r2y[0].dword =
        0X0000049E2DC50D9B; // RGB->YCbCr matrix (RGB->YCC709)
    ctrl_output.r2y[1].dword =
        0X00002000E755F8AA; // RGB->YCbCr matrix (RGB->YCC709)
    ctrl_output.r2y[2].dword =
        0X0000FD10E2EF2000; // RGB->YCbCr matrix (RGB->YCC709)
  }
  ctrl_output.clip_cal.y_cal.ygain = 0x00010000; // Y Gain value
  ctrl_output.clip_cal.y_cal.yofs = 0x00000000;  // Y Offset value
  ctrl_output.clip_cal.y_clip.cph = 0xFFFF;      // Y Clip upper bound value
  ctrl_output.clip_cal.y_clip.cpl = 0x0000;      // Y Clip lower bound value
  ctrl_output.clip_cal.cb_cal.cgain = 0x8000;    // Cb Gain value
  ctrl_output.clip_cal.cb_cal.cofs = 0x0000;     // Cb Offset value
  ctrl_output.clip_cal.cb_clip.cph = 0xFFFF;     // Cb Clip upper bound value
  ctrl_output.clip_cal.cb_clip.cpl = 0x0000;     // Cb Clip lower bound value
  ctrl_output.clip_cal.cr_cal.cgain = 0x8000;    // Cr Gain value
  ctrl_output.clip_cal.cr_cal.cofs = 0x0000;     // Cr Offset value
  ctrl_output.clip_cal.cr_clip.cph = 0xFFFF;     // Cr Clip upper bound value
  ctrl_output.clip_cal.cr_clip.cpl = 0x0000;     // Cr Clip lower bound value
  if (((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (video_id == FJ_DISP_VIDEO_ID_2160p_3840)) ||
      ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (video_id == FJ_DISP_VIDEO_ID_2160p50_3840))) {
    ctrl_output.doctl0.bit.DTO1 = D_IM_DISP_DTO_G_CB; // LCD data output order 1
    ctrl_output.doctl0.bit.DTO2 = D_IM_DISP_DTO_R_Y0; // LCD data output order 2
    ctrl_output.doctl0.bit.DTO3 = D_IM_DISP_DTO_0_Y1; // LCD data output order 3
    ctrl_output.doctl0.bit.DTO4 = D_IM_DISP_DTO_G_CB; // LCD data output order 4
    ctrl_output.doctl0.bit.DTO5 = D_IM_DISP_DTO_R_Y0; // LCD data output order 5
    ctrl_output.doctl0.bit.DTO6 = D_IM_DISP_DTO_0_Y1; // LCD data output order 6
  } else if (gDisp_HDMI_DTD_Man->dev_inf == FJ_HDMI_DISPLAY_INTERFACE_DVI) {
    ctrl_output.doctl0.bit.DTO1 = D_IM_DISP_DTO_R_Y0; // LCD data output order 1
    ctrl_output.doctl0.bit.DTO2 = D_IM_DISP_DTO_G_CB; // LCD data output order 2
    ctrl_output.doctl0.bit.DTO3 = D_IM_DISP_DTO_B_CR; // LCD data output order 3
    ctrl_output.doctl0.bit.DTO4 = D_IM_DISP_DTO_R_Y0; // LCD data output order 4
    ctrl_output.doctl0.bit.DTO5 = D_IM_DISP_DTO_G_CB; // LCD data output order 5
    ctrl_output.doctl0.bit.DTO6 = D_IM_DISP_DTO_B_CR; // LCD data output order 6
  }
#ifdef CO_HDMI_COMPLIANCE_TEST
  else if (gRS_System_Disp_Target_RGB) {
    ctrl_output.doctl0.bit.DTO1 = D_IM_DISP_DTO_R_Y0; // LCD data output order 1
    ctrl_output.doctl0.bit.DTO2 = D_IM_DISP_DTO_G_CB; // LCD data output order 2
    ctrl_output.doctl0.bit.DTO3 = D_IM_DISP_DTO_B_CR; // LCD data output order 3
    ctrl_output.doctl0.bit.DTO4 = D_IM_DISP_DTO_R_Y0; // LCD data output order 4
    ctrl_output.doctl0.bit.DTO5 = D_IM_DISP_DTO_G_CB; // LCD data output order 5
    ctrl_output.doctl0.bit.DTO6 = D_IM_DISP_DTO_B_CR; // LCD data output order 6
  }
#endif
  else {
    ctrl_output.doctl0.bit.DTO1 = D_IM_DISP_DTO_G_CB; // LCD data output order 1
    ctrl_output.doctl0.bit.DTO2 = D_IM_DISP_DTO_R_Y0; // LCD data output order 2
    ctrl_output.doctl0.bit.DTO3 = D_IM_DISP_DTO_B_CR; // LCD data output order 3
    ctrl_output.doctl0.bit.DTO4 = D_IM_DISP_DTO_0_Y1; // LCD data output order 4
    ctrl_output.doctl0.bit.DTO5 = 0;                  // LCD data output order 5
    ctrl_output.doctl0.bit.DTO6 = 0;                  // LCD data output order 6
  }
  ctrl_output.doctl1.bit.SOO =
      D_IM_DISP_ENABLE_OFF; // 0.5pix shift output (Odd line)
  ctrl_output.doctl1.bit.SOE =
      D_IM_DISP_ENABLE_OFF; // 0.5pix shift output (Even line)
  ctrl_output.doctl1.bit.ODO0 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 0 (Odd line)
  ctrl_output.doctl1.bit.ODO1 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 1 (Odd line)
  ctrl_output.doctl1.bit.ODO2 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 2 (Odd line)
  ctrl_output.doctl1.bit.ODE0 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 0 (Even line)
  ctrl_output.doctl1.bit.ODE1 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 1 (Even line)
  ctrl_output.doctl1.bit.ODE2 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 2 (Even line)
  ctrl_output.doctl2.bit.CLVS = D_IM_DISP_CLVS_NORMAL; // CbCr data output level
  if (gDisp_HDMI_DTD_Man->dev_inf == FJ_HDMI_DISPLAY_INTERFACE_DVI) {
    ctrl_output.doctl2.bit.CTOH = D_IM_DISP_CTOH_LEFT; // Left CbCr data
  }
#ifdef CO_HDMI_COMPLIANCE_TEST
  else if (gRS_System_Disp_Target_RGB) {
    ctrl_output.doctl2.bit.CTOH = D_IM_DISP_CTOH_LEFT; // Left CbCr data
  }
#endif
  else {
    ctrl_output.doctl2.bit.CTOH = D_IM_DISP_CTOH_AVERAGE; // Left CbCr data
  }
  ctrl_output.doctl2.bit.CTOV = D_IM_DISP_CTOV_TOP;    // Top CbCr data
  ctrl_output.doctl2.bit.TRSOS = D_IM_DISP_TRSOS_NONE; // SAV/EAV code output
  //	ctrl_output.trscode[0]........Set the get value, because
  //D_IM_DISP_TRSOS_NONE. 	ctrl_output.face..............Set the get value,
  //because it will be set in OSD API. 	ctrl_output.int_callback[0]...Set the get
  //value, because it will be set in each BaseFW function.

  im_ret =
      Im_DISP_Ctrl_Output(E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_hdmi_config_video() "
                          "Error!!:Im_DISP_Ctrl_Output() error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  // Connect HDMI
  fj_ret = FJ_HDMI_Connect_HDMI(video_format_id);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(("fj_disp_hdmi_config_video() "
                          "Error!!:FJ_HDMI_Connect_HDMI() error. fj_ret=%x\n",
                          fj_ret));
    return fj_ret;
  }

  return FJ_ERR_OK;
}

/* Configure Video format. */
FJ_ERR_CODE fj_disp_hdmi_config_audio(UCHAR disp_id, ULONG samp_freq) {
  INT32 im_ret;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_config_audio() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if ((samp_freq != FJ_DISP_HDMI_AUDIO_32000HZ) &&
      (samp_freq != FJ_DISP_HDMI_AUDIO_48000HZ) &&
      (samp_freq != FJ_DISP_HDMI_AUDIO_96000HZ) &&
      (samp_freq != FJ_DISP_HDMI_AUDIO_44100HZ)) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_config_audio() Error!!:samp_freq is illegal.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Start HIF clock (guard for Dd_HDMI_Set_Audio())
  Dd_Top_Set_CLKSTOP3_HIFCK(0);

  // Set Audio.
  im_ret = fj_hdmi_audio_ctrl(samp_freq);

  return im_ret;
}

/* HDMI output start. */
FJ_ERR_CODE fj_disp_hdmi_start_sync(UCHAR disp_id) {
  INT32 im_ret;
  U_IM_DISP_LALP lalp;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_start_sync() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gBF_Instance_Display[disp_id].type != FJ_DISP_TYPE_HDMI) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_start_sync() Error!!:disp_id(%d) is illegal.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Start color bar layer
  im_ret =
      Im_DISP_Start(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE, 0);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_hdmi_start_sync() "
                          "Error!!:Im_DISP_Start(DOCRE) error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  lalp.bit.ALP = 0x00;
  Im_DISP_Set_Alpha_Value(E_IM_DISP_HDMI, lalp);

  // Start image layer
  im_ret = Im_DISP_Start(E_IM_DISP_SEL_LAYER_MAIN, E_IM_DISP_SEL_LAYER_NONE, 0);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_hdmi_start_sync() "
                          "Error!!:Im_DISP_Start(MAIN) error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  // Start HIF clock (guard for FJ_HDMI_Start_Sync())
  Dd_Top_Set_CLKSTOP3_HIFCK(0);

  // Start HDMI output
  FJ_HDMI_Start_Sync();

  // Start HDMI Audio output
  FJ_HDMI_Audio_Start();

  gBF_Instance_Display[disp_id].connect_state = E_BF_INSTANCE_CONNECT_STATE_ON;

  return FJ_ERR_OK;
}

/* HDMI output stop. */
FJ_ERR_CODE fj_disp_hdmi_stop_sync(UCHAR disp_id) {
  FJ_ERR_CODE fj_ret;
  INT32 im_ret;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_stop_sync() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gBF_Instance_Display[disp_id].type != FJ_DISP_TYPE_HDMI) {
    BF_Debug_Print_Error((
        "fj_disp_hdmi_stop_sync() Error!!:disp_id(%d) is illegal.\n", disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Start HIF clock (guard for FJ_HDMI_Stop_Sync())
  Dd_Top_Set_CLKSTOP3_HIFCK(0);

  // Stop HDMI output
  FJ_HDMI_Stop_Sync();

  // Stop DISP trigger
  im_ret = Im_DISP_Stop(E_IM_DISP_SEL_LAYER_ALL, E_IM_DISP_SEL_LAYER_NONE, 0,
                        E_IM_DISP_STOP_TYPE_WITH_WAIT);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error((
        "fj_disp_hdmi_stop_sync() Error!!:Im_DISP_Stop(all) error. im_ret=%x\n",
        im_ret));
    return FJ_ERR_NG;
  }
  Im_DISP_Wait_Stop(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE);

  // Stop HIF clock
  Dd_Top_Set_CLKSTOP3_HIFCK(1);

  // DISP macro soft-reset (for display switch)
  Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE,
                 D_IM_DISP_SR_RESET);

  // Switch display to None
  fj_ret =
      BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_HDMI, E_FJ_DISP_IFS_NONE);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_stop_sync() Error!!:BF_Osd_Attr_Set_Display_Interface() "
         "error. fj_ret=%x\n",
         fj_ret));
    return FJ_ERR_NG;
  }

  // Disconnect HDMI
  fj_ret = FJ_HDMI_Disconnect_HDMI();
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_hdmi_stop_sync() Error!!:FJ_HDMI_Disconnect_HDMI() error. "
         "fj_ret=%x\n",
         fj_ret));
    return fj_ret;
  }

  gBF_Instance_Display[disp_id].connect_state = E_BF_INSTANCE_CONNECT_STATE_OFF;

  return FJ_ERR_OK;
}

/* ------------------- */
/* MIPI				   */
/* ------------------- */
/* Set MIPI timing generator to DISP macro. */
FJ_ERR_CODE fj_disp_mipi_config_video(UCHAR disp_id,
                                      FJ_DISP_LCD_CONFIG *lcd_config_info) {
  INT32 im_ret;
  T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_output_trglmt;
  T_IM_DISP_CTRL_OUTPUT ctrl_output;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_config_video() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gBF_Instance_Display[disp_id].type != FJ_DISP_TYPE_LCD) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_config_video() Error!!:disp_id(%d) is illegal.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (lcd_config_info == NULL) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_config_video() Error!!:lcd_config_info is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (lcd_config_info->lcd_timing.hblk < 8) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_config_video() Error!!:hblk(%lu) is illegal.\n",
         lcd_config_info->lcd_timing.hblk));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  memset(&ctrl_output_trglmt, 0, sizeof(T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT));
  memset(&ctrl_output, 0, sizeof(T_IM_DISP_CTRL_OUTPUT));

  // Get control data of data output block
  im_ret = Im_DISP_Get_Ctrl_Output(E_IM_DISP_LCD_MIPI, &ctrl_output_trglmt,
                                   &ctrl_output);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_config_video() Error!!:Im_DISP_Get_Ctrl_Output() error. "
         "im_ret=%x\n",
         im_ret));
    return FJ_ERR_NG;
  }

  // Set control data of data output block
  ctrl_output_trglmt.tgkmd = E_IM_DISP_TGKMD_NORMAL; // External start-up mode
  ctrl_output_trglmt.tgkst = 0;                      // Operation start setting
  ctrl_output_trglmt.toctl.bit.CON =
      D_IM_DISP_CON_INTERNAL; // Connects relay terminal of the chip
  ctrl_output_trglmt.toctl.bit.CKOEN =
      D_IM_DISP_ENABLE_OFF; // External output clock permission
  ctrl_output_trglmt.toctl.bit.CKICEN =
      D_IM_DISP_ENABLE_ON; // Internal output clock permission
  ctrl_output_trglmt.toctl.bit.WBEN =
      D_IM_DISP_ENABLE_OFF; // Write back permission
  ctrl_output_trglmt.rpgctl =
      E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP; // Register parameter taking control
  ctrl_output_trglmt.polsel.bit.VDPS =
      lcd_config_info->lcd_polarity.vsync; // Vsync polarity
  ctrl_output_trglmt.polsel.bit.HDPS =
      lcd_config_info->lcd_polarity.hsync; // Hsync polarity
  ctrl_output_trglmt.polsel.bit.VEPS =
      lcd_config_info->lcd_polarity.venable; // Vertical enable signal polarity
  ctrl_output_trglmt.polsel.bit.HEPS =
      lcd_config_info->lcd_polarity
          .henable; // Horizontal enable signal polarity
  ctrl_output_trglmt.polsel.bit.CKEG =
      lcd_config_info->lcd_polarity.clock; // Output clock edge
  ctrl_output_trglmt.tsl =
      lcd_config_info->lcd_timing.scan_mode; // 0:Progressive, 1:Interlace
  ctrl_output_trglmt.vcyc.bit.VCYC1 =
      lcd_config_info->lcd_timing.vcyc; // Vertical cycle (odd)
  if (lcd_config_info->lcd_timing.vcyc_shift) {
    ctrl_output_trglmt.vcyc.bit.VCYC2 = lcd_config_info->lcd_timing.vcyc +
                                        1; // Vertical cycle (even) + 0.5H shift
  } else {
    ctrl_output_trglmt.vcyc.bit.VCYC2 =
        lcd_config_info->lcd_timing.vcyc; // Vertical cycle (even)
  }
  ctrl_output_trglmt.vcyc.bit.VDSFT =
      lcd_config_info->lcd_timing.vcyc_shift; // Vsync output 0.5H shift
  ctrl_output_trglmt.hcyc =
      lcd_config_info->lcd_timing.hcyc; // Horizontal cycle
  ctrl_output_trglmt.ovpw.bit.OVPW =
      lcd_config_info->lcd_timing.vpw; // Vsync pulse width
  ctrl_output_trglmt.ovpw.bit.OVPWU =
      D_IM_DISP_OVPWU_LINE_UNIT; // Vsync pluse width unit [Line]
  ctrl_output_trglmt.hpw =
      lcd_config_info->lcd_timing.hpw; // Horizontal signal pulse width
  ctrl_output_trglmt.vblk.bit.VBLK1 =
      lcd_config_info->lcd_timing.vblk; // Vertical Blank(odd)
  if (lcd_config_info->lcd_timing.vcyc_shift) {
    ctrl_output_trglmt.vblk.bit.VBLK2 = lcd_config_info->lcd_timing.vblk +
                                        1; // Vertical Blank(even) + 0.5H shift
  } else {
    ctrl_output_trglmt.vblk.bit.VBLK2 =
        lcd_config_info->lcd_timing.vblk; // Vertical Blank(even)
  }
  ctrl_output_trglmt.vdly = 0; // Vsync output delay
  if (lcd_config_info->lcd_timing.hblk < 16) {
    ctrl_output_trglmt.hblk = 16; // Horizontal Blank
    ctrl_output_trglmt.hdly =
        16 - lcd_config_info->lcd_timing.hblk; // Hsync output delay
  } else {
    ctrl_output_trglmt.hblk =
        lcd_config_info->lcd_timing.hblk; // Horizontal Blank
    ctrl_output_trglmt.hdly = 0;          // Hsync output delay
  }
  ctrl_output_trglmt.ovsize =
      lcd_config_info->lcd_timing.vsize; // Vertical data size
  ctrl_output_trglmt.ohsize =
      lcd_config_info->lcd_timing.hsize; // Horizontal data size
  ctrl_output_trglmt.vrfctl =
      D_IM_DISP_VENS_HSYNC; // Vertical enable signal assert timing
  ctrl_output_trglmt.hrfctl =
      D_IM_DISP_HENS_ASSERT_OFF; // Asserted for the period of a vertical blank
  ctrl_output_trglmt.domd.bit.MODE =
      lcd_config_info->lcd_format.output_mode; // LCD Output mode
  ctrl_output_trglmt.domd.bit.OBPSL =
      D_IM_DISP_OBPSL_LSB;                    // Outputs by LSB stuff
  switch (ctrl_output_trglmt.domd.bit.MODE) { // Output bit unit selection
  case FJ_DISP_LCD_OUTPUT_MODE_RGB888_24BIT:
    ctrl_output_trglmt.domd.bit.OBUSL = D_IM_DISP_OBUSL_8BIT;
    break;
  case FJ_DISP_LCD_OUTPUT_MODE_RGB565_16BIT:
    ctrl_output_trglmt.domd.bit.OBUSL = D_IM_DISP_OBUSL_8BIT;
    break;
  case FJ_DISP_LCD_OUTPUT_MODE_RGB666_18BIT:
    ctrl_output_trglmt.domd.bit.OBUSL = D_IM_DISP_OBUSL_8BIT;
    break;
  default:
    BF_Debug_Print_Error(
        ("fj_disp_mipi_config_video() Error!!:MODE(%d) is illegal.\n",
         ctrl_output_trglmt.domd.bit.MODE));
    return FJ_ERR_NG_INPUT_PARAM;
  }
  ctrl_output_trglmt.tblaset_gmtaen =
      E_IM_DISP_TABLE_ACCESS_DISABLE; // Gamma correct table access enable
  ctrl_output_trglmt.tblaset_ysatta =
      E_IM_DISP_TABLE_ACCESS_DISABLE; // Luminance and chroma correct table
                                      // access enable

  if (lcd_config_info->lcd_timing.scan_mode == 1) {
    ctrl_output.intc = D_IM_DISP_VFTMG_1ST; // Interruption control
  } else {
    ctrl_output.intc = D_IM_DISP_VFTMG_1ST_2ND; // Interruption control
  }
  ctrl_output.inte.bit.LEE =
      D_IM_DISP_ENABLE_ON; // Main data input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GR0EE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GR1EE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GA0EE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block A component forwarding
                           // error interruption
  ctrl_output.inte.bit.GA1EE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block A component forwarding
                           // error interruption
  ctrl_output.inte.bit.LREE =
      D_IM_DISP_ENABLE_ON; // Main data input block AXI error interruption
  ctrl_output.inte.bit.GR0REE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block AXI error interruption
  ctrl_output.inte.bit.GR1REE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block AXI error interruption
  ctrl_output.inte.bit.GA0REE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block A component AXI error
                           // interruption
  ctrl_output.inte.bit.GA1REE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block A component AXI error
                           // interruption
  ctrl_output.inte.bit.VE =
      D_IM_DISP_ENABLE_ON; // Vsync1 detection interruption
  ctrl_output.inte.bit.VE2 =
      D_IM_DISP_ENABLE_OFF; // Vsync2 detection interruption
  ctrl_output.hablk = 0;    // Horizontal blank adjustment setting
  ctrl_output.fdoen = D_IM_DISP_ENABLE_OFF; // Fource data output enable
  ctrl_output.fodata.word = 0;              // Fource output data
  ctrl_output.blankdt.word[0] = 0;          // Blank data setting register0
  ctrl_output.blankdt.word[1] = 0;          // Blank data setting register1
  ctrl_output.clbhsize =
      lcd_config_info->lcd_timing.hsize; // Color bar horizontal size
  ctrl_output.clbdt[0].word =
      0; // Color bar display color setting (Set to black)
  ctrl_output.bldctl.bit.BLD1 = 0; // Blend order 1 ayer Grid
  ctrl_output.bldctl.bit.BLD2 = 1; // Blend order 2 ayer Face
  ctrl_output.bldctl.bit.BLD3 = 2; // Blend order 3 ayer OSD0
  ctrl_output.bldctl.bit.BLD4 = 3; // Blend order 4 ayer OSD1
  ctrl_output.r2y[0].dword =
      0x0000075025901320; // RGB->YCbCr matrix (RGB->YCC601)
  ctrl_output.r2y[1].dword =
      0x00002000EAD0F530; // RGB->YCbCr matrix (RGB->YCC601)
  ctrl_output.r2y[2].dword =
      0x0000FAD0E5302000; // RGB->YCbCr matrix (RGB->YCC601)
  ctrl_output.clip_cal.y_cal.ygain = 0x00010000; // Y Gain value
  ctrl_output.clip_cal.y_cal.yofs = 0x00000000;  // Y Offset value
  ctrl_output.clip_cal.y_clip.cph = 0xFFFF;      // Y Clip upper bound value
  ctrl_output.clip_cal.y_clip.cpl = 0x0000;      // Y Clip lower bound value
  ctrl_output.clip_cal.cb_cal.cgain = 0x8000;    // Cb Gain value
  ctrl_output.clip_cal.cb_cal.cofs = 0x0000;     // Cb Offset value
  ctrl_output.clip_cal.cb_clip.cph = 0xFFFF;     // Cb Clip upper bound value
  ctrl_output.clip_cal.cb_clip.cpl = 0x0000;     // Cb Clip lower bound value
  ctrl_output.clip_cal.cr_cal.cgain = 0x8000;    // Cr Gain value
  ctrl_output.clip_cal.cr_cal.cofs = 0x0000;     // Cr Offset value
  ctrl_output.clip_cal.cr_clip.cph = 0xFFFF;     // Cr Clip upper bound value
  ctrl_output.clip_cal.cr_clip.cpl = 0x0000;     // Cr Clip lower bound value
  ctrl_output.doctl0.bit.DTO1 =
      lcd_config_info->lcd_format.output_order[0]; // LCD data output order 1
  ctrl_output.doctl0.bit.DTO2 =
      lcd_config_info->lcd_format.output_order[1]; // LCD data output order 2
  ctrl_output.doctl0.bit.DTO3 =
      lcd_config_info->lcd_format.output_order[2]; // LCD data output order 3
  ctrl_output.doctl0.bit.DTO4 =
      lcd_config_info->lcd_format.output_order[3]; // LCD data output order 4
  ctrl_output.doctl0.bit.DTO5 =
      lcd_config_info->lcd_format.output_order[4]; // LCD data output order 5
  ctrl_output.doctl0.bit.DTO6 =
      lcd_config_info->lcd_format.output_order[5]; // LCD data output order 6
  if (lcd_config_info->lcd_format.pix_fmt == FJ_DISP_LCD_PIX_FMT_DELTA) {
    ctrl_output.doctl1.bit.SOO =
        D_IM_DISP_ENABLE_ON; // 0.5pix shift output (Odd line)
  } else {
    ctrl_output.doctl1.bit.SOO =
        D_IM_DISP_ENABLE_OFF; // 0.5pix shift output (Odd line)
  }
  ctrl_output.doctl1.bit.SOE =
      D_IM_DISP_ENABLE_OFF; // 0.5pix shift output (Even line)
  ctrl_output.doctl1.bit.ODO0 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 0 (Odd line)
  ctrl_output.doctl1.bit.ODO1 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 1 (Odd line)
  ctrl_output.doctl1.bit.ODO2 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 2 (Odd line)
  ctrl_output.doctl1.bit.ODE0 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 0 (Even line)
  ctrl_output.doctl1.bit.ODE1 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 1 (Even line)
  ctrl_output.doctl1.bit.ODE2 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 2 (Even line)
  ctrl_output.doctl2.bit.CLVS = D_IM_DISP_CLVS_NORMAL; // CbCr data output level
  ctrl_output.doctl2.bit.CTOH =
      D_IM_DISP_CTOH_LEFT; // CbCr off (444->442 thin out)
  ctrl_output.doctl2.bit.CTOV =
      D_IM_DISP_CTOV_TOP; // CbCr off (444->442 thin out)
  ctrl_output.doctl2.bit.TRSOS = D_IM_DISP_TRSOS_NONE; // SAV/EAV code output
  //	ctrl_output.trscode[0]........Set the get value, because
  //D_IM_DISP_TRSOS_NONE.
  ctrl_output.y2r[0].dword =
      0x000059C000004000; // YCbCr->RGB matrix (YCC601->RGB)
  ctrl_output.y2r[1].dword =
      0x0000D250EA004000; // YCbCr->RGB matrix (YCC601->RGB)
  ctrl_output.y2r[2].dword =
      0x0000FFF071704000; // YCbCr->RGB matrix (YCC601->RGB)
  ctrl_output.tblaset_gmen =
      E_IM_DISP_CORRECT_PROC_DISABLE; // Gamma correction enable
  ctrl_output.tblaset_gmtsl =
      D_IM_DISP_TABLE_SURFACE_A; // Gamma correction table surface
  ctrl_output.tblaset_ysaten =
      E_IM_DISP_CORRECT_PROC_DISABLE; // Luminance and chroma correction enable
  ctrl_output.tblaset_ysatsl =
      D_IM_DISP_TABLE_SURFACE_A; // Luminance and chroma correction table
                                 // surface
  //	ctrl_output.face..............Set the get value, because it will be set
  //in OSD API. 	ctrl_output.int_callback[0]...Set the get value, because it will
  //be set in each BaseFW function.

  im_ret = Im_DISP_Ctrl_Output(E_IM_DISP_LCD_MIPI, &ctrl_output_trglmt,
                               &ctrl_output);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_config_video() "
                          "Error!!:Im_DISP_Ctrl_Output() error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

/* Start MIPI clock(MIFCK), VSYNC output, image layer trigger. */
FJ_ERR_CODE fj_disp_mipi_start_sync(UCHAR disp_id) {
  INT32 im_ret;
  FJ_ERR_CODE fj_ret;
  U_IM_DISP_LALP lalp;
  FJ_MIPI_CMD_INFO mipi_command_info;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_start_sync() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gBF_Instance_Display[disp_id].type != FJ_DISP_TYPE_LCD) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_start_sync() Error!!:disp_id(%d) is illegal.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  fj_ret = FJ_MIPI_Set_Mode(FJ_DISP_MIPI_MODE_CMD);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_start_sync() "
                          "Error!!:FJ_MIPI_Set_Mode() error. fj_ret=%x\n",
                          fj_ret));
    return FJ_ERR_NG;
  }
  // Command Sending(Display On)
  memset(&mipi_command_info, 0, sizeof(FJ_MIPI_CMD_INFO));
  mipi_command_info.cmd_num = 1;
  mipi_command_info.cmd_val[0].cmd_id = 0x29;
  mipi_command_info.cmd_val[0].param_size = 0;
  fj_ret = FJ_MIPI_Command_Ctrl(&mipi_command_info);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_start_sync() "
                          "Error!!:FJ_MIPI_Command_Ctrl() error. fj_ret=%x\n",
                          fj_ret));
    return FJ_ERR_NG;
  }
  fj_ret = FJ_MIPI_Set_Mode(FJ_DISP_MIPI_MODE_VIDEO);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_start_sync() "
                          "Error!!:FJ_MIPI_Set_Mode() error. fj_ret=%x\n",
                          fj_ret));
    return FJ_ERR_NG;
  }

  // MIPI-DSI Shutdown OFF
  im_ret = Im_DISP_Set_Mipi_Dsi_Ctrl0(0, 0);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_start_sync() Error!!:Im_DISP_Set_Mipi_Dsi_Ctrl0() "
         "error. im_ret=%x\n",
         im_ret));
    return FJ_ERR_NG;
  }

  // Start color bar layer
  im_ret =
      Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE, 0);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_start_sync() "
                          "Error!!:Im_DISP_Start(DCORE) error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  lalp.bit.ALP = 0x00;
  Im_DISP_Set_Alpha_Value(E_IM_DISP_LCD_MIPI, lalp);

  // Start image layer
  im_ret = Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_MAIN, 0);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_start_sync() "
                          "Error!!:Im_DISP_Start(MAIN) error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  gBF_Instance_Display[disp_id].connect_state = E_BF_INSTANCE_CONNECT_STATE_ON;

  return FJ_ERR_OK;
}

/* Stop MIPI clock(MIFCK), VSYNC output, image layer trigger. */
FJ_ERR_CODE fj_disp_mipi_stop_sync(UCHAR disp_id) {
  FJ_ERR_CODE fj_ret;
  INT32 im_ret;
  FJ_MIPI_CMD_INFO mipi_command_info;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_stop_sync() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (gBF_Instance_Display[disp_id].type != FJ_DISP_TYPE_LCD) {
    BF_Debug_Print_Error((
        "fj_disp_mipi_stop_sync() Error!!:disp_id(%d) is illegal.\n", disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  fj_ret = FJ_MIPI_Set_Mode(FJ_DISP_MIPI_MODE_CMD);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_stop_sync() Error!!:FJ_MIPI_Set_Mode() "
                          "error. fj_ret=%x\n",
                          fj_ret));
    return FJ_ERR_NG;
  }

  // Command Sending(Software Reset)
  memset(&mipi_command_info, 0, sizeof(FJ_MIPI_CMD_INFO));
  mipi_command_info.cmd_num = 1;
  mipi_command_info.cmd_val[0].cmd_id = 0x01;
  mipi_command_info.cmd_val[0].param_size = 0;
  fj_ret = FJ_MIPI_Command_Ctrl(&mipi_command_info);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_stop_sync() "
                          "Error!!:FJ_MIPI_Command_Ctrl() error. fj_ret=%x\n",
                          fj_ret));
    return FJ_ERR_NG;
  }

  // Stop DISP trigger
  im_ret = Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_ALL, 0,
                        E_IM_DISP_STOP_TYPE_WITH_WAIT);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error((
        "fj_disp_mipi_stop_sync() Error!!:Im_DISP_Stop(all) error. im_ret=%x\n",
        im_ret));
    return FJ_ERR_NG;
  }
  Im_DISP_Wait_Stop(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE);

  // Stop MIF clock (for set CLKSEL)
  Dd_Top_Set_CLKSTOP3_MIFCK(1);

  // DISP macro soft-reset (for display switch)
  Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE,
                 D_IM_DISP_SR_RESET);

  // MIPI-DSI IP Reset Cansel.
  im_ret = Im_DISP_Set_SR_IP(D_IM_DISP_IP_MIPI, D_IM_DISP_SR_RESET);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_mipi_stop_sync() "
                          "Error!!:Im_DISP_Set_SR_IP(MIPI) error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  // Switch display to None
  fj_ret =
      BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_LCD, E_FJ_DISP_IFS_NONE);
  if (fj_ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("fj_disp_mipi_stop_sync() Error!!:BF_Osd_Attr_Set_Display_Interface() "
         "error. fj_ret=%x\n",
         fj_ret));
    return FJ_ERR_NG;
  }

  gBF_Instance_Display[disp_id].connect_state = E_BF_INSTANCE_CONNECT_STATE_OFF;

  return FJ_ERR_OK;
}

/* ------------- */
/* Image Layer   */
/* ------------- */
/* Set Disp instance window setting. */
FJ_ERR_CODE fj_set_display_window(UCHAR disp_id,
                                  FJ_DISP_WINDOW_CONFIG *window_config) {
  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_set_display_window() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (window_config == NULL) {
    BF_Debug_Print_Error(
        ("fj_set_display_window() Error!!:window_config is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  gBF_Instance_Display[disp_id].window.in_x_pos = window_config->in_x_pos;
  gBF_Instance_Display[disp_id].window.in_y_pos = window_config->in_y_pos;
  gBF_Instance_Display[disp_id].window.out_x_pos = window_config->out_x_pos;
  gBF_Instance_Display[disp_id].window.out_y_pos = window_config->out_y_pos;
  gBF_Instance_Display[disp_id].window.input.width = window_config->in_width;
  gBF_Instance_Display[disp_id].window.input.lines = window_config->in_lines;
  gBF_Instance_Display[disp_id].window.output.width = window_config->out_width;
  gBF_Instance_Display[disp_id].window.output.lines = window_config->out_lines;

  // Set main size
  //	disp_set_main_size( disp_id );

  return FJ_ERR_OK;
}

/* Get Disp instance window setting. */
FJ_ERR_CODE fj_get_display_window(UCHAR disp_id,
                                  FJ_DISP_WINDOW_CONFIG *window_config) {
  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    //		BF_Debug_Print_Error(("fj_get_display_window() Error!!:disp_id(%d) is
    //not opened.\n", disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (window_config == NULL) {
    BF_Debug_Print_Error(
        ("fj_get_display_window() Error!!:window_config is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  window_config->in_x_pos = gBF_Instance_Display[disp_id].window.in_x_pos;
  window_config->in_y_pos = gBF_Instance_Display[disp_id].window.in_y_pos;
  window_config->out_x_pos = gBF_Instance_Display[disp_id].window.out_x_pos;
  window_config->out_y_pos = gBF_Instance_Display[disp_id].window.out_y_pos;
  window_config->in_width = gBF_Instance_Display[disp_id].window.input.width;
  window_config->in_lines = gBF_Instance_Display[disp_id].window.input.lines;
  window_config->out_width = gBF_Instance_Display[disp_id].window.output.width;
  window_config->out_lines = gBF_Instance_Display[disp_id].window.output.lines;

  return FJ_ERR_OK;
}

/* Render image layer. */
FJ_ERR_CODE fj_disp_video_render(UCHAR disp_id, FJ_DISP_YCC_ADDR *ycc_addr,
                                 UINT32 global_width) {
  FJ_ERR_CODE fj_ret;
  INT32 im_ret;
  FJ_DISP_YCC_ADDR phy_ycc_addr;
#if 0
	UINT32 				y_addr, c_addr, y_size, c_size;
#endif
  E_IM_DISP_SEL block;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_video_render() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (ycc_addr == NULL) {
    BF_Debug_Print_Error(
        ("fj_disp_video_render() Error!!:ycc_addr is NULL.\n"));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Set Block Number
  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  // Convert address to physical from logical
  phy_ycc_addr.y_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(ycc_addr->y_addr);
  phy_ycc_addr.cbcr_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(ycc_addr->cbcr_addr);

#if 0
	y_addr = phy_ycc_addr.y_addr + (global_width * gBF_Instance_Display[disp_id].window.in_y_pos) + gBF_Instance_Display[disp_id].window.in_x_pos;
	c_addr = ycc_addr->cbcr_addr + (global_width * (gBF_Instance_Display[disp_id].window.in_y_pos / 2)) + gBF_Instance_Display[disp_id].window.in_x_pos;
	y_size = global_width * gBF_Instance_Display[disp_id].window.input.lines;
	c_size = y_size / 2;
	BF_L1l2cache_Clean_Flush_Addr( y_addr, y_size );
	BF_L1l2cache_Clean_Flush_Addr( c_addr, c_size );
#endif

  fj_ret =
      BF_Disp_Set_Main_Addr(disp_id, &phy_ycc_addr, E_BF_DISP_IMAGE_FORMAT_420,
                            E_BF_DISP_YCC_COLOR_SPACE_709, global_width);
  if (fj_ret != FJ_ERR_OK) {
    return FJ_ERR_NG;
  } else {
    im_ret = Im_DISP_Wait_Vsync(block, E_IM_DISP_INTERRUPTION_SELECT_VE,
                                E_IM_DISP_WAIT_1_TIME);
    if (im_ret != D_DDIM_OK) {
      BF_Debug_Print_Error(("fj_disp_video_render() Error!!:Im_DISP_Wait_Vsync "
                            "error. im_ret=%x\n",
                            im_ret));
      return FJ_ERR_NG;
    }
  }

  return FJ_ERR_OK;
}

/* Set display image layer rotation. */
FJ_ERR_CODE fj_disp_video_set_rotate(UCHAR disp_id,
                                     FJ_DISP_ROTATE_DEGREE rotate_degree) {
  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_video_set_rotate() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

#if 0
	if( rotate_degree != FJ_DISP_ROTATE_DEGREE_0 ){
		BF_Debug_Print_Error(( "fj_disp_video_set_rotate() Error!!:rotate_degree(%d) is not support.\n", rotate_degree ));
		return FJ_ERR_NO_SUPPORT;
	}
#endif

  gBF_Instance_Display[disp_id].rotate_degree = rotate_degree;

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_disp_video_get_rotate(UCHAR disp_id,
                                     FJ_DISP_ROTATE_DEGREE *rotate_degree) {
  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    //		BF_Debug_Print_Error(("fj_disp_video_get_rotate() Error!!:disp_id(%d)
    //is not opened.\n", disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  *rotate_degree = gBF_Instance_Display[disp_id].rotate_degree;

  return FJ_ERR_OK;
}

/* Enable image layer. */
FJ_ERR_CODE fj_disp_video_enable(UCHAR disp_id) {
  FJ_ERR_CODE fj_ret;
  E_IM_DISP_SEL block;
  U_IM_DISP_LALP lalp;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_video_enable() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Set Block Number
  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  Dd_ARM_Critical_Section_Start(gBF_Disp_Main_Spin_Lock);

  // check disp crrent bank address is 0. (to prevent DECERR)
  fj_ret = disp_check_addr_set(block);
  if (fj_ret != FJ_ERR_OK) {
    // Wait enable until update address
    gDisp_Image_Layer_Enable_Wait[disp_id] = TRUE;

    BF_Debug_Print_Information(
        ("fj_disp_video_enable() ::Wait enable until update address.\n"));
  } else {
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_OFF);
    Dd_ARM_Dsb_Pou();

    lalp.bit.ALP = 0xFF;
    Im_DISP_Set_Alpha_Value(block, lalp);

    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    Dd_ARM_Dsb_Pou();

    gDisp_Image_Layer_Enable_Wait[disp_id] = FALSE;
  }

  gBF_Video_enable_stat[disp_id] = TRUE;

  Dd_ARM_Critical_Section_End(gBF_Disp_Main_Spin_Lock);

  return FJ_ERR_OK;
}

/* Disable image layer. */
FJ_ERR_CODE fj_disp_video_disable(UCHAR disp_id) {
  E_IM_DISP_SEL block;
  U_IM_DISP_LALP lalp;

  if (gBF_Instance_Display[disp_id].flag == E_BF_INSTANCE_FLAG_UNUSE) {
    BF_Debug_Print_Error(
        ("fj_disp_video_disable() Error!!:disp_id(%d) is not opened.\n",
         disp_id));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Set Block Number
  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  Dd_ARM_Critical_Section_Start(gBF_Disp_Main_Spin_Lock);

  Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_OFF);
  Dd_ARM_Dsb_Pou();

  lalp.bit.ALP = 0x00;
  Im_DISP_Set_Alpha_Value(block, lalp);

  // Clear Display buffer (To hold the enable at FJ_Disp_Video_Enable())
  disp_clear_addr(block);

  Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
  Dd_ARM_Dsb_Pou();

  gDisp_Image_Layer_Enable_Wait[disp_id] = FALSE;

  gBF_Video_enable_stat[disp_id] = FALSE;

  Dd_ARM_Critical_Section_End(gBF_Disp_Main_Spin_Lock);

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_disp_show_boot_image(const FJ_DISP_IMAGE_CONFIG *config_info) {
  FJ_ERR_CODE ercd = FJ_ERR_OK;
  E_IM_DISP_SEL block;
  U_IM_DISP_LIDT lidt = {{0, 0}};
  U_IM_DISP_SIZE input_size = {0};
  U_IM_DISP_SIZE output_size = {0};
  U_IM_DISP_LALP lalp = {0};
  T_IM_DISP_RESIZE resize;
  U_IM_DISP_DSTA position = {0};
  T_IM_DISP_IMAGE_ADDR addr;

  // Set Block Number
  if (config_info->block == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  memset(&resize, 0, sizeof(T_IM_DISP_RESIZE));
  memset(&addr, 0, sizeof(T_IM_DISP_IMAGE_ADDR));

  lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;

  lalp.bit.ALP = 255;

  position.bit.DSH = config_info->window.out_x_pos;
  position.bit.DSV = config_info->window.out_y_pos;

  addr.y_addr = config_info->addr +
                (config_info->g_width * config_info->window.in_y_pos) +
                config_info->window.in_x_pos;
  addr.c_addr =
      config_info->addr + config_info->g_width * config_info->g_height;
  addr.c_addr = addr.c_addr +
                (config_info->g_width * (config_info->window.in_y_pos / 2)) +
                config_info->window.in_x_pos;

  input_size.size.width = config_info->window.in_width;
  input_size.size.lines = config_info->window.in_lines;

  if (config_info->window.out_width == 0 ||
      config_info->window.out_lines == 0) {
    ULONG out_width;
    ULONG out_lines;
    Im_DISP_Get_Output_Size(block, &out_lines, &out_width);
    output_size.size.width = out_width;
    output_size.size.lines = out_lines;
  } else {
    output_size.size.width = config_info->window.out_width;
    output_size.size.lines = config_info->window.out_lines;
  }
  osd_config_get_resize_rate(block, E_IM_DISP_SEL_LAYER_MAIN, &input_size,
                             &output_size, &resize);
  resize.rsz0 = E_IM_DISP_RSZSL_BILINEAR;

  if (Im_DISP_Set_Input_Data_Transfer(block, lidt) != D_DDIM_OK) { // LIDT0,LIDT1
    ercd = FJ_ERR_NG;
  } else if (Im_DISP_Set_Input_Size(block, input_size) != D_DDIM_OK) { // LISIZE
    ercd = FJ_ERR_NG;
  } else if (Im_DISP_Set_Lfd(block, config_info->g_width,
                             config_info->g_width) !=
             D_DDIM_OK) { // LCHGA,LYHGA
    ercd = FJ_ERR_NG;
  } else if (Im_DISP_Set_Addr(block, E_IM_DISP_BANK_00, &addr) !=
             D_DDIM_OK) { // LYSA,LCSA
    ercd = FJ_ERR_NG;
  } else {
    Im_DISP_Set_Main_Bank(block, E_IM_DISP_BANK_00); // LIBCTL
    Im_DISP_Set_Alpha_Value(block, lalp);            // LALP
  }

  if (Im_DISP_Set_Resize(block, &resize) !=
      D_DDIM_OK) { // LRSZ0,LRSZ1,LRSZ2,LRSZ3
    ercd = FJ_ERR_NG;
  } else if (Im_DISP_Set_Display_Position(block, position) !=
             D_DDIM_OK) { // LDSTA
    ercd = FJ_ERR_NG;
  } else if (Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_MAIN,
                           0) != D_DDIM_OK) { // LTRG
    ercd = FJ_ERR_NG;
  }
  return ercd;
}

/* ------------- */
/* OSD Layer   */
/* ------------- */
/* Set the configuration of drawing OSD. */
FJ_ERR_CODE fj_disp_osd_set_config(UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id,
                                   FJ_DISP_OSD_CONFIG *config_info) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  T_IM_DISP_RESIZE resize;
  T_Rect set_imput_info;
  U_IM_DISP_GRIDT gridt = {0};
  U_IM_DISP_DSTA position;
  U_IM_DISP_SIZE input_size, output_size;
  E_IM_DISP_SEL_LAYER osd_sel_layer;
  E_OSDLayer osd_layer_id;
  ULONG pix_size;
  E_IM_DISP_SEL block;
  U_IM_DISP_GRIPO ipo, ipo8888 = {0x03020100}, ipo4444 = {0x03020100};

  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_LCD) { // LCD
    switch (layer_id) {
    case FJ_DISP_LAYERID_TYPE_OSD0:
      osd_sel_layer = E_IM_DISP_SEL_LAYER_OSD_0;
      osd_layer_id = E_OSDLayer_LCD_OSD0;
      break;
    case FJ_DISP_LAYERID_TYPE_OSD1:
      osd_sel_layer = E_IM_DISP_SEL_LAYER_OSD_1;
      osd_layer_id = E_OSDLayer_LCD_OSD1;
      break;
    default:
      return FJ_ERR_OSD_LAYER;
    }
    block = E_IM_DISP_LCD_MIPI;
  } else if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) { // HDMI
    switch (layer_id) {
    case FJ_DISP_LAYERID_TYPE_OSD0:
      osd_sel_layer = E_IM_DISP_SEL_LAYER_OSD_0;
      osd_layer_id = E_OSDLayer_HDMI_OSD0;
      break;
    case FJ_DISP_LAYERID_TYPE_OSD1:
      osd_sel_layer = E_IM_DISP_SEL_LAYER_OSD_1;
      osd_layer_id = E_OSDLayer_HDMI_OSD1;
      break;
    default:
      return FJ_ERR_OSD_LAYER;
    }
    block = E_IM_DISP_HDMI;
  } else {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  if (Im_DISP_Get_OSD_Input_Data_Transfer(block, osd_sel_layer, &gridt) !=
      D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  /* Set OSD format (GRIDT.IFMT, IPO) */
  switch ((ULONG)config_info->in_fmt) {
  case FJ_DISP_OSD_FORMAT_RGBA8888:
    gridt.bit.IFMT = D_DISP_OSD_FMT_RGBA8888;
    pix_size = D_DISP_OSD_PIX_RGBA8888;
    ipo = ipo8888;
    break;
  case FJ_DISP_OSD_FORMAT_RGBA4444:
    gridt.bit.IFMT = D_DISP_OSD_FMT_RGBA4444;
    pix_size = D_DISP_OSD_PIX_RGBA4444;
    ipo = ipo4444;
    break;
  default:
    return FJ_ERR_NG_INPUT_PARAM;
  }
  if (block == E_IM_DISP_HDMI) {
    /* Burst size Setting */
    gridt.bit.NBT = D_IM_DISP_NBT_512BYTE;
  }
  if (Im_DISP_Set_OSD_Input_Data_Transfer(block, osd_sel_layer, gridt) !=
      D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  if (Im_DISP_Set_OSD_IPO(block, osd_sel_layer, ipo) != D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  /* Set input area size (GRTISIZE) */
  input_size.size.width = (ULONG)config_info->in_pitch;
  input_size.size.lines = (ULONG)config_info->in_osd.Height;
  set_imput_info.Width = (SHORT)config_info->in_osd.Width;
  set_imput_info.Height = (SHORT)config_info->in_osd.Height;
  set_imput_info.Postion.StartX = (SHORT)config_info->in_osd.Position.StartX;
  set_imput_info.Postion.StartY = (SHORT)config_info->in_osd.Position.StartY;

  ret = osd_comm_draw_area_size_check(input_size, &set_imput_info);
  if (ret == FJ_ERR_OK) {
    if (Im_DISP_Set_OSD_Territory_Size(block, osd_sel_layer, input_size) !=
        D_DDIM_OK) {
      return FJ_ERR_NG;
    }
  } else {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  /* Set input area display start position (GRTDSTA) */
  position.word = 0;
  position.bit.DSH = (ULONG)config_info->out_osd.Position.StartX;
  position.bit.DSV = (ULONG)config_info->out_osd.Position.StartY;

  if (Im_DISP_Set_OSD_Territory_Position(block, osd_sel_layer, position) !=
      D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  /* OSD Layer display off */
  ret = fj_osd_set_osd_area_enable(osd_layer_id, 0);
  if (ret != FJ_ERR_OK) {
    return FJ_ERR_NG;
  }

  /* Set OSD area 0 (GRDSTA, GRHGA, GRISIZE) */
  position.word = 0;
  input_size.size.width = (ULONG)config_info->in_osd.Width;
  input_size.size.lines = (ULONG)config_info->in_osd.Height;

  position.bit.DSH = (ULONG)config_info->in_osd.Position.StartX;
  position.bit.DSV = (ULONG)config_info->in_osd.Position.StartY;
  if (Im_DISP_Set_OSD_Display_Position(block, osd_sel_layer, 0, position) !=
      D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  if (Im_DISP_Set_OSD_Lfd(block, osd_sel_layer, 0,
                          (ULONG)config_info->in_pitch * pix_size) !=
      D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  if (Im_DISP_Set_OSD_Area_Size(block, osd_sel_layer, &input_size, 1) !=
      D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  /* Set resize configuration (GRHRSZ1, GRVRSZ) */
  output_size.size.width = config_info->out_osd.Width;
  output_size.size.lines = (ULONG)config_info->out_osd.Height;
  resize.rsz0 = E_IM_DISP_RSZSL_BILINEAR;
  osd_config_get_resize_rate(block, osd_sel_layer, &input_size, &output_size,
                             &resize);

  if (Im_DISP_Set_OSD_Resize(block, osd_sel_layer, &resize) != D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

/* BaseFW will enable the OSD of specified layer. */
FJ_ERR_CODE fj_disp_osd_enable(UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  E_OSDLayer osd_layer;
  UINT32 area_enable = 1; /* use only area 0 */

  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_LCD) { // LCD
    switch (layer_id) {
    case FJ_DISP_LAYERID_TYPE_OSD0:
      osd_layer = E_OSDLayer_LCD_OSD0;
      break;
    case FJ_DISP_LAYERID_TYPE_OSD1:
      osd_layer = E_OSDLayer_LCD_OSD1;
      break;
    default:
      return FJ_ERR_OSD_LAYER;
    }
  } else if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) { // HDMI
    switch (layer_id) {
    case FJ_DISP_LAYERID_TYPE_OSD0:
      osd_layer = E_OSDLayer_HDMI_OSD0;
      break;
    case FJ_DISP_LAYERID_TYPE_OSD1:
      osd_layer = E_OSDLayer_HDMI_OSD1;
      break;
    default:
      return FJ_ERR_OSD_LAYER;
    }
  } else {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  /* Set OSD area enable */
  ret = fj_osd_set_osd_area_enable(osd_layer, area_enable);
  if (ret != FJ_ERR_OK) {
    return FJ_ERR_NG;
  }

  /* Display OSD data */
  ret = fj_show_enable(osd_layer, TRUE);
  if (ret != FJ_ERR_OK) {
    return FJ_ERR_NG;
  }
  return ret;
}

/* BaseFW will disconnect the OSD of specified layer. */
FJ_ERR_CODE fj_disp_osd_disable(UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  E_OSDLayer osd_layer;
  UINT32 area_enable = 0;

  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_LCD) { // LCD
    switch (layer_id) {
    case FJ_DISP_LAYERID_TYPE_OSD0:
      osd_layer = E_OSDLayer_LCD_OSD0;
      break;
    case FJ_DISP_LAYERID_TYPE_OSD1:
      osd_layer = E_OSDLayer_LCD_OSD1;
      break;
    default:
      return FJ_ERR_OSD_LAYER;
    }
  } else if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) { // HDMI
    switch (layer_id) {
    case FJ_DISP_LAYERID_TYPE_OSD0:
      osd_layer = E_OSDLayer_HDMI_OSD0;
      break;
    case FJ_DISP_LAYERID_TYPE_OSD1:
      osd_layer = E_OSDLayer_HDMI_OSD1;
      break;
    default:
      return FJ_ERR_OSD_LAYER;
    }
  } else {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  /* Set OSD area disable */
  ret = fj_osd_set_osd_area_enable(osd_layer, area_enable);
  if (ret != FJ_ERR_OK) {
    return FJ_ERR_NG;
  }

  ret = fj_show_enable(osd_layer, FALSE);
  if (ret != FJ_ERR_OK) {
    return FJ_ERR_NG;
  }

  return ret;
}

/* Rotate and Copy the OSD data to display in the OSD area. */
FJ_ERR_CODE fj_disp_osd_rotate_copy(UCHAR disp_id,
                                    FJ_DISP_LAYERID_TYPE layer_id,
                                    FJ_DISP_OSD_RECT_INFO *rect_info,
                                    FJ_DISP_OSD_BUFF_ADDR *buff_addr) {
  E_IM_DISP_SEL_LAYER osd_sel_layer;
  USHORT src_g_width, lines;
  U_IM_DISP_GRIDT gridt;
  UINT32 src_addr, dst_addr;
  USHORT width, dst_g_width, pix_size;
  E_IM_DISP_SEL block;

  BF_Measure_Time_Start(E_BF_MEASURE_TIME_ID_OSD);

  memset(&gridt, 0, sizeof(U_IM_DISP_GRIDT));

  switch (layer_id) {
  case FJ_DISP_LAYERID_TYPE_OSD0:
    osd_sel_layer = E_IM_DISP_SEL_LAYER_OSD_0;
    break;
  case FJ_DISP_LAYERID_TYPE_OSD1:
    osd_sel_layer = E_IM_DISP_SEL_LAYER_OSD_1;
    break;
  default:
    return FJ_ERR_OSD_LAYER;
  }

  // Set Block Number
  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  if (Im_DISP_Get_OSD_Input_Data_Transfer(block, osd_sel_layer, &gridt) !=
      D_DDIM_OK) {
    return FJ_ERR_NG;
  }

  switch (gridt.bit.IFMT) {
  case D_DISP_OSD_FMT_RGBA8888:
    pix_size = D_DISP_OSD_PIX_RGBA8888;
    break;
  case D_DISP_OSD_FMT_RGBA4444:
    pix_size = D_DISP_OSD_PIX_RGBA4444;
    break;
  default:
    return FJ_ERR_NG_INPUT_PARAM;
  }

  src_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(buff_addr->in_buff_addr) +
             ((rect_info->in_pitch * rect_info->in_osd.Position.StartY) +
              rect_info->in_osd.Position.StartX) *
                 pix_size;
  dst_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(buff_addr->out_buff_addr) +
             ((rect_info->out_pitch * rect_info->out_osd.Position.StartY) +
              rect_info->out_osd.Position.StartX) *
                 pix_size;

  gDisp_OSD_Degree = (UCHAR)rect_info->rotate_degree;

  if (rect_info->rotate_degree == FJ_DISP_ROTATE_DEGREE_0) { /* only copy */
    if (((rect_info->in_pitch * pix_size) & 0x0F) ||
        ((rect_info->out_pitch * pix_size) & 0x0F)) {
      return FJ_ERR_NG_INPUT_PARAM;
    }

    width = rect_info->out_osd.Width * pix_size;
    lines = rect_info->out_osd.Height;
    src_g_width = rect_info->in_pitch * pix_size;
    dst_g_width = rect_info->out_pitch * pix_size;

    BF_L1l2cache_Clean_Flush_Addr(src_addr, (src_g_width * lines));

    if ((width & 0x0F) || (src_addr & 0x0F) || (dst_addr & 0x0F)) {
      if (disp_osd_copy_by_thin_mode(E_IM_XCH_CH_SEL_0, src_addr, src_g_width,
                                     dst_g_width, dst_addr, width,
                                     lines) != D_DDIM_OK) {
        return FJ_ERR_NG;
      }
    } else {
      if (Im_Xch_Copy(E_IM_XCH_CH_SEL_0, src_addr, src_g_width, dst_g_width,
                      dst_addr, width, lines) != D_DDIM_OK) {
        return FJ_ERR_NG;
      }
    }
  } else { /* copy & rotate */
    width = rect_info->in_osd.Width;
    lines = rect_info->in_osd.Height;
    src_g_width = rect_info->in_pitch;
    dst_g_width = rect_info->out_pitch;

    if (rect_info->rotate_degree == FJ_DISP_ROTATE_DEGREE_90) {
      disp_osd_rotate90(src_addr, dst_addr, width, lines, src_g_width,
                        dst_g_width);
    } else if (rect_info->rotate_degree == FJ_DISP_ROTATE_DEGREE_270) {
      disp_osd_rotate270(src_addr, dst_addr, width, lines, src_g_width,
                         dst_g_width);
    } else { // FJ_DISP_ROTATE_DEGREE_180
      disp_osd_rotate180(src_addr, dst_addr, width, lines, src_g_width,
                         dst_g_width);
    }
  }

  BF_Measure_Time_Print(E_BF_MEASURE_TIME_ID_OSD, "FJ_Disp_OSD_Rotate_Copy\n");

  return FJ_ERR_OK;
}

/* Render OSD frame. */
FJ_ERR_CODE fj_disp_osd_render(UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id,
                               UINT32 disp_buff_addr) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  E_IM_DISP_SEL_LAYER osd_sel_layer;
  E_IM_DISP_OSD_SA_BANK_NO bank_no = E_IM_DISP_OSD_SA_BANK_NO_0_0;
  UINT32 disp_addr;
  U_IM_DISP_SIZE input_size[D_IM_DISP_OSD_DISPLAY_AREA_COUNT];
  ULONG global_width;
  E_IM_DISP_SEL block;

  switch (layer_id) {
  case FJ_DISP_LAYERID_TYPE_OSD0:
    osd_sel_layer = E_IM_DISP_SEL_LAYER_OSD_0;
    break;
  case FJ_DISP_LAYERID_TYPE_OSD1:
    osd_sel_layer = E_IM_DISP_SEL_LAYER_OSD_1;
    break;
  default:
    return FJ_ERR_OSD_LAYER;
  }

  // Set Block Number
  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  /* Set OSD display data address (GRSA) */
  disp_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(disp_buff_addr);

  Im_DISP_Get_OSD_Lfd(block, osd_sel_layer, bank_no, &global_width);
  Im_DISP_Get_OSD_Area_Size(block, osd_sel_layer, input_size);
  BF_L1l2cache_Clean_Flush_Addr(
      disp_addr, (global_width * input_size[bank_no].size.lines));

  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_LCD) { // LCD
    if (Im_DISP_Set_OSD_Area_Addr(block, osd_sel_layer, bank_no, disp_addr) !=
        D_DDIM_OK) {
      ret = FJ_ERR_NG;
    }

    if (Im_DISP_Wait_Vsync(block, E_IM_DISP_INTERRUPTION_SELECT_VE,
                           E_IM_DISP_WAIT_1_TIME)) {
      ret = FJ_ERR_NG;
    }
  } else if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) { // HDMI
    if (Im_DISP_Set_OSD_Area_Addr(block, osd_sel_layer, bank_no, disp_addr) !=
        D_DDIM_OK) {
      ret = FJ_ERR_NG;
    }

    if (Im_DISP_Wait_Vsync(block, E_IM_DISP_INTERRUPTION_SELECT_VE,
                           E_IM_DISP_WAIT_1_TIME)) {
      ret = FJ_ERR_NG;
    }
  } else {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  return ret;
}

/**
 * Get Display instance.
 */
T_BF_INSTANCE_DISPLAY *BF_Instance_Get_Display(UCHAR disp_id) {
  return &gBF_Instance_Display[disp_id];
}

/**
 * Get OSD instance.
 */
VOID BF_Disp_OSD_Print_Info(UCHAR disp_id) {
  static U_IM_DISP_GRIDT gridt;
  static U_IM_DISP_DSTA in_position;
  static U_IM_DISP_DSTA out_position;
  static U_IM_DISP_SIZE pitch_size;
  static U_IM_DISP_SIZE input_size[D_IM_DISP_OSD_DISPLAY_AREA_COUNT];
  static T_IM_DISP_RESIZE resize;
  static E_IM_DISP_OSD_SA_BANK_NO bank_no = E_IM_DISP_OSD_SA_BANK_NO_0_0;
  ULONG disp_addr = 0;
  UCHAR layer_cnt;
  UINT32 act_layer;
  UINT32 hdmi_layer;
  UINT32 lcd_mipi_layer;
  E_IM_DISP_SEL block;

  // Set Block Number
  if (gBF_Instance_Display[disp_id].type == FJ_DISP_TYPE_HDMI) {
    block = E_IM_DISP_HDMI;
  } else {
    block = E_IM_DISP_LCD_MIPI;
  }

  Im_DISP_Get_Status(&hdmi_layer, &lcd_mipi_layer);
  if (block == E_IM_DISP_HDMI) {
    act_layer = hdmi_layer;
  } else {
    act_layer = lcd_mipi_layer;
  }

  printf("  [Disp OSD Info]\n");

  for (layer_cnt = 0; layer_cnt < D_DISP_OSD_LAYER_MAX; layer_cnt++) {
    if (act_layer & (E_IM_DISP_SEL_LAYER_MAIN << (layer_cnt + 2))) {
      printf("   [OSD Layer id : %d] Active\n", layer_cnt);

      /* Get Input configuration */
      Im_DISP_Get_OSD_Input_Data_Transfer(
          block, (E_IM_DISP_SEL_LAYER_MAIN << (layer_cnt + 2)), &gridt);
      Im_DISP_Get_OSD_Display_Position(
          block, (E_IM_DISP_SEL_LAYER_MAIN << (layer_cnt + 2)), bank_no,
          &in_position);
      Im_DISP_Get_OSD_Area_Size(block,
                                (E_IM_DISP_SEL_LAYER_MAIN << (layer_cnt + 2)),
                                &input_size[bank_no]);
      Im_DISP_Get_OSD_Territory_Size(
          block, (E_IM_DISP_SEL_LAYER_MAIN << (layer_cnt + 2)), &pitch_size);

      /* Get Output configuration */
      Im_DISP_Get_OSD_Territory_Position(
          block, (E_IM_DISP_SEL_LAYER_MAIN << (layer_cnt + 2)), &out_position);
      Im_DISP_Get_OSD_Resize(
          block, (E_IM_DISP_SEL_LAYER_MAIN << (layer_cnt + 2)), &resize);

      /* Get Display address */
      Im_DISP_Get_OSD_Area_Addr(block,
                                (E_IM_DISP_SEL_LAYER_MAIN << (layer_cnt + 2)),
                                bank_no, &disp_addr);

      printf("    OSD In width                               : %lu\n",
             (ULONG)input_size[bank_no].size.width);
      printf("    OSD In height                              : %lu\n",
             (ULONG)input_size[bank_no].size.lines);
      printf("    OSD In position X                          : %lu\n",
             (ULONG)in_position.bit.DSH);
      printf("    OSD In position Y                          : %lu\n",
             (ULONG)in_position.bit.DSV);
      printf("    OSD In pitch                               : %lu\n",
             (ULONG)pitch_size.size.width);
      printf("    OSD In format                              : %lu\n",
             (ULONG)gridt.bit.IFMT);
      printf("    OSD Out width                              : %lu\n",
             (ULONG)(pitch_size.size.width * resize.rsz1.bit.HRSZM /
                     resize.rsz1.bit.HRSZN));
      printf("    OSD Out height                             : %lu\n",
             (ULONG)(pitch_size.size.lines * resize.rsz2.bit.VRSZM /
                     resize.rsz2.bit.VRSZN));
      printf("    OSD Out position X                         : %lu\n",
             (ULONG)out_position.bit.DSH);
      printf("    OSD Out position Y                         : %lu\n",
             (ULONG)out_position.bit.DSV);
      printf("    OSD Buffer address                         : 0x%08lx\n",
             disp_addr);
      printf("    OSD Rotate_degree                          : %d\n",
             gDisp_OSD_Degree);
    } else {
      printf("   [OSD Layer id : %d] Non Active\n", layer_cnt);
    }
  }
}

/* ------------- */
/* DWCH(PWCH)    */
/* ------------- */
static VOID disp_write_channel_vsync_callback(VOID) {
  BYTE ifs;
  INT32 im_ret;

  FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_IMGPROC_VD, FJ_MBALOG_TYPE_POINT, "VSYNC:%d",
                 gBF_Disp_DWCH_Vsync_Count));

  // Get display interface selection
  if (Im_DISP_Get_Display_Interface(E_IM_DISP_HDMI, &ifs) == D_DDIM_OK) {
    gBF_Disp_DWCH_Vsync_Count++;
    switch (gBF_Disp_DWCH_Vsync_Count) {
    case 0: // strat DWCH
    case 1: // 1st VE interrupt end
      break;
    case 2: // 2nd VE interrupt end
      im_ret = Im_DISP_PWCH_Detect(E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE);
      if (im_ret != D_DDIM_OK) {
        BF_Debug_Print_Error(("disp_write_channel_vsync_callback() "
                              "Error!!:Im_DISP_PWCH_Detect error. im_ret=%x\n",
                              im_ret));
      }
      break;
    default: // 2 more VE interrupt end
      break;
    }
  }
}

static FJ_ERR_CODE disp_write_channel_wait_dwch_stop(VOID) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  INT32 im_ret;
  E_IM_DISP_WAIT wait_time = E_IM_DISP_WAIT_1_TIME;

  // Set Block Number
  im_ret = Im_DISP_Wait_PWCH(E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE, wait_time);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("disp_write_channel_wait_dwch_stop() "
                          "Error!!:Im_DISP_Wait_PWCH error. im_ret=%x\n",
                          im_ret));
    ret = FJ_ERR_NG;
  }

  return ret;
}

static FJ_ERR_CODE
disp_write_channel_set_dwch(const FJ_DWCH_IMG_INFO *const img_info) {
  T_IM_DISP_CTRL_P2M p2m_ctrl;
  T_IM_DISP_CTRL_PWCH pwch_ctrl;
  ULONG line_feed = 0;
  T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_output_trglmt;
  T_IM_DISP_CTRL_OUTPUT ctrl_output;

  // Get control data of data output block
  Im_DISP_Get_Ctrl_Output(E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output);

  if (ctrl_output_trglmt.ovsize == 0) {
    BF_Debug_Print_Error(("disp_write_channel_set_dwch() Error!!: ovsize=%lu\n",
                          ctrl_output_trglmt.ovsize));
    return FJ_ERR_NG;
  }
  if (ctrl_output_trglmt.ohsize == 0) {
    BF_Debug_Print_Error(("disp_write_channel_set_dwch() Error!!: ohsize=%lu\n",
                          ctrl_output_trglmt.ohsize));
    return FJ_ERR_NG;
  }

  // P2M
  p2m_ctrl.p2mctl_plpf = E_IM_DISP_PLPF_INVALID;
  p2m_ctrl.ptrmv = ctrl_output_trglmt.vblk.bit.VBLK1;
  p2m_ctrl.ptrmvw = ctrl_output_trglmt.ovsize; // Main Image lines;
  p2m_ctrl.p2mctl_p2mtyp = E_IM_DISP_P2MTYP_8; // 8 bit recording mode.

  line_feed = (ULONG)img_info->img[E_DWCH_IMG_DWCH].g_y_width;
  p2m_ctrl.ptrmh = ctrl_output_trglmt.hblk;
  p2m_ctrl.ptrmhw = ctrl_output_trglmt.ohsize;

  p2m_ctrl.p2mofs = 0;
  p2m_ctrl.pbsft = 0;

  p2m_ctrl.pclph = 0x00FF;
  p2m_ctrl.ptrmxvcyc = 1;
  p2m_ctrl.ptrmxhcyc = 1;
  p2m_ctrl.ptrmxven = 0x8000000000000000;
  p2m_ctrl.ptrmxhen = 0x8000000000000000;
  p2m_ctrl.p2mmir = D_IM_DISP_ENABLE_OFF;
  p2m_ctrl.p2m2pmd = E_IM_DISP_P2M2PMD_2;

  // DWCH
  pwch_ctrl.pwaxctl.word = 1; // AWCACHE[3:0] =000b, AWPROT[2:0] =001b.
  pwch_ctrl.pwmw = D_IM_DISP_ENABLE_ON;
  pwch_ctrl.pwvfm = D_IM_DISP_FORMAT_NORMAL;

  pwch_ctrl.pwlsize = line_feed;
  pwch_ctrl.pwsa = (ULONG)img_info->img[E_DWCH_IMG_DWCH].AddressY;
  //	pwch_ctrl.pwsa			= (ULONG)0xB6439400;
  ////SDRAM_ADR_VIEW_MODE_3_VHDR_RGB_0

  pwch_ctrl.pwchintenb.word =
      (D_IM_DISP_DWCH_FLG_PWF | D_IM_DISP_DWCH_FLG_PWEF |
       D_IM_DISP_DWCH_FLG_PWXF);

  Im_DISP_Set_Write_Channel_Ctrl(
      E_IM_DISP_WC_NUM_0, &p2m_ctrl,
      &pwch_ctrl); /**< Write channel 0 (Luminance data)	*/

  // PWCH1 for Cb/Cr (YUV420)
  p2m_ctrl.ptrmxvcyc = 2; // for YUV 422->YUV 420
  pwch_ctrl.pwsa = (ULONG)img_info->img[E_DWCH_IMG_DWCH].AddressCb;
  //	pwch_ctrl.pwsa			= (ULONG)0xB6C44000;
  ////SDRAM_ADR_VIEW_MODE_3_VHDR_RGB_0;

  Im_DISP_Set_Write_Channel_Ctrl(
      E_IM_DISP_WC_NUM_1, &p2m_ctrl,
      &pwch_ctrl); /**< Write channel 1 (Chrominance data)	*/

  return FJ_ERR_OK;
}

static FJ_ERR_CODE
disp_write_channel_set_main_image(const FJ_DWCH_IMG_INFO *const img_info) {
  INT32 im_ret;
  E_IM_DISP_BANK bank_no;
  T_IM_DISP_IMAGE_ADDR im_disp_addr;
  U_IM_DISP_LIDT lidt = {.word[0] = 0, .word[1] = 0};
  E_IM_DISP_SEL block;

  U_IM_DISP_DSTA position = {0};
  U_IM_DISP_SIZE input_size = {0};
  U_IM_DISP_SIZE output_size = {0};
  T_IM_DISP_RESIZE resize = {.rsz0 = E_IM_DISP_RSZSL_BILINEAR,
                             .rsz1.word = 0x00000101,
                             .rsz2.word = 0x00000101,
                             .rsz3.word = 0};
  UINT32 in_x_pos;
  UINT32 in_y_pos;
  UINT32 out_x_pos;
  UINT32 out_y_pos;

  // Set Block Number
  block = E_IM_DISP_HDMI;

  // initialize in/out position
  in_x_pos = 0;
  in_y_pos = 0;
  out_x_pos = 0;
  out_y_pos = 0;

  // Set image format
  im_ret = Im_DISP_Get_Input_Data_Transfer(block, &lidt);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("%s Error!!:Im_DISP_Get_Input_Data_Transfer error. im_ret=%x\n",
         __func__, im_ret));
    return FJ_ERR_NG;
  }

  // Set YCC->RGB matrix
  disp_set_y2r_matrix(block, E_BF_DISP_YCC_COLOR_SPACE_709);

  lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;
  lidt.bit.NBT = D_IM_DISP_NBT_512BYTE;
  lidt.bit.PKGDV = 0; // Value : 0 (fix).
  lidt.bit.LVFM = D_IM_DISP_FORMAT_NORMAL;
  lidt.bit.YSLVSL = D_IM_DISP_SLVSL_MASTER_INTERFACE_1;
  lidt.bit.CSLVSL = D_IM_DISP_SLVSL_MASTER_INTERFACE_1;

  // Set parameter reflect disable.
  Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_OFF);
  Dd_ARM_Dsb_Pou();

  im_ret = Im_DISP_Set_Input_Data_Transfer(block, lidt);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("%s Error!!:Im_DISP_Set_Input_Data_Transfer error. im_ret=%x\n",
         __func__, im_ret));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  // Set global width
  im_ret = Im_DISP_Set_Lfd(block, img_info->img[E_DWCH_IMG_MAIN].g_y_width,
                           img_info->img[E_DWCH_IMG_MAIN].g_c_width);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("%s Error!!:Im_DISP_Set_Lfd error. im_ret=%x, "
                          "img_info->g_y_width=%u img_info->g_c_width=%u\n",
                          __func__, im_ret,
                          img_info->img[E_DWCH_IMG_MAIN].g_y_width,
                          img_info->img[E_DWCH_IMG_MAIN].g_c_width));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  // Set output position for DISP macro data output block
  position.bit.DSH =
      out_x_pos; // gBF_Instance_Display[disp_id].window.out_x_pos;
  position.bit.DSV =
      out_y_pos; // gBF_Instance_Display[disp_id].window.out_y_pos;
  im_ret = Im_DISP_Set_Display_Position(block, position);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("%s Error!!:Im_DISP_Set_Display_Position error. "
                          "im_ret=%x. DSH=%d, DSV=%d\n",
                          __func__, im_ret, position.bit.DSH,
                          position.bit.DSV));
  }

  // Set main layer input size
  input_size.size.width = img_info->img[E_DWCH_IMG_MAIN].width;
  input_size.size.lines = img_info->img[E_DWCH_IMG_MAIN].lines;

  // Set resize
  output_size.size.width = img_info->img[E_DWCH_IMG_MAIN].width;
  output_size.size.lines = img_info->img[E_DWCH_IMG_MAIN].lines;
  ;
  osd_config_get_resize_rate(block, E_IM_DISP_SEL_LAYER_MAIN, &input_size,
                             &output_size, &resize);

  im_ret = Im_DISP_Set_Resize(block, &resize);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("%s Error!!:Im_DISP_Set_Resize error. im_ret=%x. "
                          "rszsl=%d, rsz1=%lu, rsz2=%lu, rsz3=%lu\n",
                          __func__, im_ret, resize.rsz0, resize.rsz1.word,
                          resize.rsz2.word, resize.rsz3.word));
  }

  // Set main layer input size
  im_ret = Im_DISP_Set_Input_Size(block, input_size);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("%s Error!!:Im_DISP_Set_Input_Size error. im_ret=%x. "
                          "width=%d, lines=%d\n",
                          __func__, im_ret, input_size.size.width,
                          input_size.size.lines));
  }

  // Get current bank No.
  im_ret = Im_DISP_Get_Main_Bank_Monitor(block, &bank_no);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("%s Error!!:Im_DISP_Get_Main_Bank_Monitor error. im_ret=%x\n",
         __func__, im_ret));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  // Update current bank No.
  if (bank_no == E_IM_DISP_BANK_11) {
    bank_no = E_IM_DISP_BANK_00;
  } else {
    bank_no++;
  }

  // Set bank address
  im_disp_addr.y_addr = img_info->img[E_DWCH_IMG_MAIN].AddressY +
                        (img_info->img[E_DWCH_IMG_MAIN].g_y_width * in_y_pos) +
                        in_x_pos;
  im_disp_addr.c_addr =
      img_info->img[E_DWCH_IMG_MAIN].AddressCb +
      (img_info->img[E_DWCH_IMG_MAIN].g_c_width * (in_y_pos / 2)) + in_x_pos;

  // check bank address is 0. (to prevent DECERR)
  if ((im_disp_addr.y_addr == 0) || (im_disp_addr.c_addr == 0)) {
    BF_Debug_Print_Error(
        ("%s Error!!:DISP bank adderss is 0. y_addr=%lx, c_addr=%lx\n",
         __func__, im_disp_addr.y_addr, im_disp_addr.c_addr));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }

  im_ret = Im_DISP_Set_Addr(block, bank_no, &im_disp_addr);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("%s Error!!:Im_DISP_Set_Addr error. im_ret=%x\n", __func__, im_ret));
    // Set parameter reflect enable.
    Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);
    return FJ_ERR_NG;
  }
  Dd_ARM_Dsb_Pou();

  // Set new bank No.
  Im_DISP_Set_Main_Bank(block, bank_no);

#if 0
{
	U_IM_DISP_LALP			lalp;
	Dd_ARM_Critical_Section_Start( gBF_Disp_Main_Spin_Lock );
	if( gDisp_Image_Layer_Enable_Wait[disp_id] ){
		lalp.bit.ALP = 0xFF;
		Im_DISP_Set_Alpha_Value( block, lalp );

		gDisp_Image_Layer_Enable_Wait[disp_id] = FALSE;

		BF_Debug_Print_Information(( "%s ::released enable waiting.\n", __func__ ));
	}

	Dd_ARM_Critical_Section_End( gBF_Disp_Main_Spin_Lock );
}
#endif

  // Set parameter reflect enable.
  Im_DISP_Set_Output_Parameter_Reflect_Enable(block, D_IM_DISP_ENABLE_ON);

  Dd_ARM_Dsb_Pou();

  return FJ_ERR_OK;
}

static FJ_ERR_CODE disp_write_channel_start(E_OSDLayer layer_id) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  BYTE ifs;
  UINT32 hdmi_layer;
  UINT32 lcd_mipi_layer;
  BYTE write_channel;

  hdmi_layer = E_IM_DISP_SEL_LAYER_NONE;
  lcd_mipi_layer = E_IM_DISP_SEL_LAYER_NONE;
  write_channel = 3; // Write channel 0 & 1

  // Get display interface selection
  if (Im_DISP_Get_Display_Interface(E_IM_DISP_HDMI, &ifs) == D_DDIM_OK) {
    if ((layer_id & E_OSDLayer_HDMI_MAIN) == E_OSDLayer_HDMI_MAIN) {
      hdmi_layer |= E_IM_DISP_SEL_LAYER_MAIN;
    }
    if ((layer_id & E_OSDLayer_HDMI_OSD0) == E_OSDLayer_HDMI_OSD0) {
      hdmi_layer |= E_IM_DISP_SEL_LAYER_OSD_0;
    }
    if ((layer_id & E_OSDLayer_HDMI_OSD1) == E_OSDLayer_HDMI_OSD1) {
      hdmi_layer |= E_IM_DISP_SEL_LAYER_OSD_1;
    }

    if (hdmi_layer == E_IM_DISP_SEL_LAYER_NONE) {
      ret = FJ_ERR_OSD_LAYER;
    } else {
      // Start image layer
      FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_START,
                     "Mask TRG start1 %u", write_channel));
      if (Im_DISP_Start(hdmi_layer, lcd_mipi_layer, write_channel) ==
          D_DDIM_OK) {
        FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_END,
                       "Mask TRG start1 %u", write_channel));

        // Start DCORE
        write_channel = 0;                      // Write channel 0 & 1 off
        hdmi_layer = E_IM_DISP_SEL_LAYER_DCORE; // start only DCORE layer
        FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_START,
                       "Mask TRG start2 %u", write_channel));
        if (Im_DISP_Start(hdmi_layer, lcd_mipi_layer, write_channel) !=
            D_DDIM_OK) {
          BF_Debug_Print_Error((
              "disp_write_channel_start() Error!!:Im_DISP_Start() 1 error.\n"));
          ret = FJ_ERR_NG;
        }
        FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_END,
                       "Mask TRG start2 %u", write_channel));
      } else {
        BF_Debug_Print_Error(
            ("disp_write_channel_start() Error!!:Im_DISP_Start() 0 error.\n"));
        ret = FJ_ERR_NG;
      }
    }
  } else {
    BF_Debug_Print_Error(("disp_write_channel_start() "
                          "Error!!:Im_DISP_Get_Display_Interface() error.\n"));
    ret = FJ_ERR_NG;
  }

  return ret;
}

static FJ_ERR_CODE disp_write_channel_stop(E_OSDLayer layer_id) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  BYTE ifs;
  UINT32 hdmi_layer;
  UINT32 lcd_mipi_layer;
  BYTE write_channel;
  E_IM_DISP_STOP_TYPE stop_type;
  INT32 im_ret;

  hdmi_layer = E_IM_DISP_SEL_LAYER_NONE;
  lcd_mipi_layer = E_IM_DISP_SEL_LAYER_NONE;
  write_channel = 3;                         // Write channel 0 & 1
  stop_type = E_IM_DISP_STOP_TYPE_WITH_WAIT; // frame stop

  // Get display interface selection
  if (Im_DISP_Get_Display_Interface(E_IM_DISP_HDMI, &ifs) == D_DDIM_OK) {

    if ((layer_id & E_OSDLayer_HDMI_MAIN) == E_OSDLayer_HDMI_MAIN) {
      hdmi_layer |= E_IM_DISP_SEL_LAYER_MAIN;
    }
    if ((layer_id & E_OSDLayer_HDMI_OSD0) == E_OSDLayer_HDMI_OSD0) {
      hdmi_layer |= E_IM_DISP_SEL_LAYER_OSD_0;
    }
    if ((layer_id & E_OSDLayer_HDMI_OSD1) == E_OSDLayer_HDMI_OSD1) {
      hdmi_layer |= E_IM_DISP_SEL_LAYER_OSD_1;
    }

    if (hdmi_layer == E_IM_DISP_SEL_LAYER_NONE) {
      ret = FJ_ERR_OSD_LAYER;
    } else {
      // Stop Image layer with write channel
      FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_START,
                     "Mask TRG stop1 %u", write_channel));
      im_ret =
          Im_DISP_Stop(hdmi_layer, lcd_mipi_layer, write_channel, stop_type);
      FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_END,
                     "Mask TRG stop1 %u", write_channel));
      if (im_ret == D_DDIM_OK) {

        // Stop DCORE
        write_channel = 0; // Write channel 0 & 1 off
        hdmi_layer = E_IM_DISP_SEL_LAYER_DCORE;
        FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_START,
                       "Mask TRG stop2 %u", write_channel));
        if (Im_DISP_Stop(hdmi_layer, lcd_mipi_layer, write_channel,
                         stop_type) != D_DDIM_OK) {
          BF_Debug_Print_Error(
              ("disp_write_channel_stop() Error!!:Im_DISP_Stop() 1 error.\n"));
          ret = FJ_ERR_NG;
        }
        FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_END,
                       "Mask TRG stop2 %u", write_channel));
      } else {
        BF_Debug_Print_Error(
            ("disp_write_channel_stop() Error!!:Im_DISP_Stop() 0 error.\n"));
        ret = FJ_ERR_NG;
      }
    }
  } else {
    BF_Debug_Print_Error(("disp_write_channel_stop() "
                          "Error!!:Im_DISP_Get_Display_Interface() error.\n"));
    ret = FJ_ERR_NG;
  }

  return ret;
}

FJ_ERR_CODE
disp_write_channel_set_timing(const FJ_DWCH_IMG_INFO *const img_info) {
  INT32 im_ret;
  T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_output_trglmt;
  T_IM_DISP_CTRL_OUTPUT ctrl_output;

  gBF_Disp_DWCH_Config.H_Active = img_info->img[E_DWCH_IMG_MAIN].width;
  gBF_Disp_DWCH_Config.V_Active = img_info->img[E_DWCH_IMG_MAIN].lines;

  // Get control data of data output block
  Im_DISP_Get_Ctrl_Output(E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output);

  // Set control data of data output block
  ctrl_output_trglmt.ovsize =
      gBF_Disp_DWCH_Config.V_Active; // Vertical data size
  ctrl_output_trglmt.vcyc.bit.VCYC1 =
      gBF_Disp_DWCH_Config.V_Active + gBF_Disp_DWCH_Config.V_Blanking;
  // Vertical cycle (odd)
  ctrl_output_trglmt.vcyc.bit.VCYC2 =
      ctrl_output_trglmt.vcyc.bit
          .VCYC1; // Vertical cycle (odd) same value is vcyc1
  ctrl_output_trglmt.hcyc =
      gBF_Disp_DWCH_Config.H_Blanking + (gBF_Disp_DWCH_Config.H_Active * 1) + 8;
  ctrl_output_trglmt.ohsize =
      gBF_Disp_DWCH_Config
          .H_Active; // Horizontal data size YU42216bit = OHSZIE*1
  ctrl_output.clbhsize =
      gBF_Disp_DWCH_Config.H_Active; // Color bar horizontal size

  im_ret =
      Im_DISP_Ctrl_Output(E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_write_channel_set_config() "
                          "Error!!:Im_DISP_Ctrl_Output() error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

/* Configure DISP(HDMI) Video format. */
FJ_ERR_CODE
fj_disp_write_channel_set_config(const FJ_DWCH_CONFIG *const dwch_config) {
  FJ_ERR_CODE fj_ret = FJ_ERR_NG;
  INT32 im_ret;
  ULONG tmp_hblk;
  T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_output_trglmt;
  T_IM_DISP_CTRL_OUTPUT ctrl_output;

  tmp_hblk = dwch_config->H_Blanking - dwch_config->H_SyncOffset; /* pgr0060 */
  if (tmp_hblk < 8) {
    BF_Debug_Print_Error(
        ("fj_disp_write_channel_set_config() Error!!:hblk(%lu) is illegal.\n",
         tmp_hblk));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Stop HIF clock (for set CLKSEL)
  Dd_Top_Set_CLKSTOP3_HIFCK(1);

  // Set HIFCLK
  Dd_Top_Set_CLKSEL2_HIFCLK(
      dwch_config->hifclk); // CLKSEL2.HIFCLK=0x8 ==> 594MHz (=1188MHz/2)

  // Start HIF clock (guard for FJ_HDMI_Connect_HDMI())
  Dd_Top_Set_CLKSTOP3_HIFCK(0);

  // DISP macro soft-reset (for display switch)
  Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE,
                 D_IM_DISP_SR_RESET);

  // Switch display to HDMI
  if (gBF_Disp_DWCH_Config_Flg == FALSE) {
    fj_ret = BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_HDMI,
                                               E_FJ_DISP_IFS_HDMI);
    if (fj_ret != FJ_ERR_OK) {
      BF_Debug_Print_Error(
          ("fj_disp_write_channel_set_config() "
           "Error!!:BF_Osd_Attr_Set_Display_Interface() error. fj_ret=%x\n",
           fj_ret));
      return FJ_ERR_NG;
    }
    gBF_Disp_DWCH_Config_Flg = TRUE;
  }

  gBF_Disp_DWCH_Config = *dwch_config;

  // Initialize DISP macro to clear soft-reset bit
  Im_DISP_Init();

  // Get control data of data output block
  Im_DISP_Get_Ctrl_Output(E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output);

  // Set control data of data output block
  ctrl_output_trglmt.tgkmd = E_IM_DISP_TGKMD_NORMAL; // External start-up mode
  ctrl_output_trglmt.tgkst = 0;                      // Operation start setting
  ctrl_output_trglmt.toctl.bit.CON =
      D_IM_DISP_CON_INTERNAL; // Connects relay terminal of the chip
  ctrl_output_trglmt.toctl.bit.CKOEN =
      D_IM_DISP_ENABLE_OFF; // External output clock permission
  ctrl_output_trglmt.toctl.bit.CKICEN =
      D_IM_DISP_ENABLE_ON; // Internal output clock permission
  ctrl_output_trglmt.toctl.bit.WBEN =
      D_IM_DISP_ENABLE_ON; // Write back permission
  ctrl_output_trglmt.rpgctl =
      E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP; // Register parameter taking control
  ctrl_output_trglmt.polsel.bit.VDPS =
      D_IM_DISP_POLARITY_POSITIVE; // Vsync polarity
  ctrl_output_trglmt.polsel.bit.HDPS =
      D_IM_DISP_POLARITY_POSITIVE;        // Hsync polarity
  ctrl_output_trglmt.polsel.bit.VEPS = 0; // Vertical enable signal polarity
  ctrl_output_trglmt.polsel.bit.HEPS = 0; // Horizontal enable signal polarity
  ctrl_output_trglmt.polsel.bit.CKEG = 0; // Output clock edge

  ctrl_output_trglmt.tsl =
      D_IM_DISP_TSL_PROGRESSIVE; // 0:Progressive, 1:Interlace
  ctrl_output_trglmt.vcyc.bit.VDSFT =
      D_IM_DISP_VDSFT_NORMAL; // Vsync output = 0: Normal output.
  ctrl_output_trglmt.ovsize =
      gBF_Disp_DWCH_Config.V_Active; // Vertical data size

  ctrl_output_trglmt.vcyc.bit.VCYC1 =
      gBF_Disp_DWCH_Config.V_Active + gBF_Disp_DWCH_Config.V_Blanking;
  // Vertical cycle (odd)
  ctrl_output_trglmt.vcyc.bit.VCYC2 =
      ctrl_output_trglmt.vcyc.bit
          .VCYC1; // Vertical cycle (odd) same value is vcyc1
  ctrl_output_trglmt.ovpw.bit.OVPW =
      gBF_Disp_DWCH_Config.V_SyncPulseWidth; // Vsync pulse width
  ctrl_output_trglmt.ovpw.bit.OVPWU =
      D_IM_DISP_OVPWU_LINE_UNIT; // Vsync pluse width unit [Line]
  ctrl_output_trglmt.vblk.bit.VBLK1 =
      gBF_Disp_DWCH_Config.V_Blanking -
      gBF_Disp_DWCH_Config.V_SyncOffset; // Vertical Blank(odd)
  ctrl_output_trglmt.vblk.bit.VBLK2 =
      ctrl_output_trglmt.vblk.bit
          .VBLK1;              // Vertical Blank(even) same value is vblanking1
  ctrl_output_trglmt.vdly = 0; // Vsync output delay

  ctrl_output_trglmt.hcyc =
      gBF_Disp_DWCH_Config.H_Blanking + (gBF_Disp_DWCH_Config.H_Active * 1) + 8;
  // Horizontal cycle (MODE=41h=YCbCr422 16bit)
  ctrl_output_trglmt.hpw = gBF_Disp_DWCH_Config.H_SyncPulseWidth /
                           2;             // Horizontal signal pulse width
  ctrl_output_trglmt.hblk = tmp_hblk / 2; // Horizontal Blank
  ctrl_output_trglmt.hdly = 0;            // Hsync output delay
  ctrl_output_trglmt.ohsize =
      gBF_Disp_DWCH_Config
          .H_Active; // Horizontal data size YU42216bit = OHSZIE*1

  ctrl_output_trglmt.vrfctl =
      D_IM_DISP_VENS_H_ENABLE; // Vertical enable signal assert timing
  ctrl_output_trglmt.hrfctl =
      D_IM_DISP_HENS_ASSERT_OFF; // Asserted for the period of a vertical blank

  ctrl_output_trglmt.domd.bit.MODE =
      D_IM_DISP_MODE_YCC422_16BIT; // HDMI Output mode (YUV422 16bit Fix)
  ctrl_output_trglmt.domd.bit.OBPSL =
      D_IM_DISP_OBPSL_LSB; // Outputs by LSB stuff
  ctrl_output_trglmt.domd.bit.OBUSL =
      D_IM_DISP_OBUSL_8BIT; // Outputs by 8bit unit

  ctrl_output.intc = D_IM_DISP_VFTMG_1ST_2ND; // Interruption control
  ctrl_output.inte.bit.LEE =
      D_IM_DISP_ENABLE_ON; // Main data input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GR0EE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GR1EE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block forwarding error
                           // interruption
  ctrl_output.inte.bit.GA0EE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block A component forwarding
                           // error interruption
  ctrl_output.inte.bit.GA1EE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block A component forwarding
                           // error interruption
  ctrl_output.inte.bit.LREE =
      D_IM_DISP_ENABLE_ON; // Main data input block AXI error interruption
  ctrl_output.inte.bit.GR0REE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block AXI error interruption
  ctrl_output.inte.bit.GR1REE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block AXI error interruption
  ctrl_output.inte.bit.GA0REE =
      D_IM_DISP_ENABLE_ON; // OSD data 0 input block A component AXI error
                           // interruption
  ctrl_output.inte.bit.GA1REE =
      D_IM_DISP_ENABLE_ON; // OSD data 1 input block A component AXI error
                           // interruption
  ctrl_output.inte.bit.VE =
      D_IM_DISP_ENABLE_ON; // Vsync1 detection interruption
  ctrl_output.inte.bit.VE2 =
      D_IM_DISP_ENABLE_OFF;                 // Vsync2 detection interruption
  ctrl_output.fdoen = D_IM_DISP_ENABLE_OFF; // Fource data output enable
  ctrl_output.fodata.word = 0;              // Fource output data
  ctrl_output.blankdt.word[0] = 0;          // Blank data setting register0
  ctrl_output.blankdt.word[1] = 0;          // Blank data setting register1
  ctrl_output.clbhsize =
      gBF_Disp_DWCH_Config.H_Active; // Color bar horizontal size
  ctrl_output.clbdt[0].word =
      0; // Color bar display color setting (Set to black)
  ctrl_output.bldctl.bit.BLD1 = 0; // Blend order 1 ayer Grid
  ctrl_output.bldctl.bit.BLD2 = 1; // Blend order 2 ayer Face
  ctrl_output.bldctl.bit.BLD3 = 2; // Blend order 3 ayer OSD0
  ctrl_output.bldctl.bit.BLD4 = 3; // Blend order 4 ayer OSD1
  ctrl_output.r2y[0].dword =
      0x000004A02DC00DA0; // RGB->YCbCr matrix (RGB->YCC709) Effective
                          // bits[15:4] by DISP ch0.
  ctrl_output.r2y[1].dword =
      0x00002000E750F8B0; // RGB->YCbCr matrix (RGB->YCC709) Effective
                          // bits[15:4] by DISP ch0.
  ctrl_output.r2y[2].dword =
      0x0000FD10E2F02000; // RGB->YCbCr matrix (RGB->YCC709) Effective
                          // bits[15:4] by DISP ch0.
  ctrl_output.clip_cal.y_cal.ygain =
      gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_Y].gain; // Y Gain value
  ctrl_output.clip_cal.y_cal.yofs =
      gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_Y].ofs; // Y Offset value
  ctrl_output.clip_cal.y_clip.cph =
      gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_Y]
          .cph; // Y Clip upper bound value
  ctrl_output.clip_cal.y_clip.cpl =
      gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_Y]
          .cpl; // Y Clip lower bound value
  ctrl_output.clip_cal.cb_cal.cgain =
      (USHORT)gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_CB]
          .gain; // Cb Gain value
  ctrl_output.clip_cal.cb_cal.cofs =
      (SHORT)gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_CB]
          .ofs; // Cb Offset value
  ctrl_output.clip_cal.cb_clip.cph =
      gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_CB]
          .cph; // Cb Clip upper bound value
  ctrl_output.clip_cal.cb_clip.cpl =
      gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_CB]
          .cpl; // Cb Clip lower bound value
  ctrl_output.clip_cal.cr_cal.cgain =
      (USHORT)gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_CR]
          .gain; // Cr Gain value
  ctrl_output.clip_cal.cr_cal.cofs =
      (SHORT)gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_CR]
          .ofs; // Cr Offset value
  ctrl_output.clip_cal.cr_clip.cph =
      gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_CR]
          .cph; // Cr Clip upper bound value
  ctrl_output.clip_cal.cr_clip.cpl =
      gBF_Disp_DWCH_Config.yuv_range[E_DWCH_CLIP_CR]
          .cpl;                                     // Cr Clip lower bound value
  ctrl_output.doctl0.bit.DTO1 = D_IM_DISP_DTO_G_CB; // LCD data output order 1
  ctrl_output.doctl0.bit.DTO2 = D_IM_DISP_DTO_R_Y0; // LCD data output order 2
  ctrl_output.doctl0.bit.DTO3 = D_IM_DISP_DTO_B_CR; // LCD data output order 3
  ctrl_output.doctl0.bit.DTO4 = D_IM_DISP_DTO_0_Y1; // LCD data output order 4
  ctrl_output.doctl0.bit.DTO5 = 0;                  // LCD data output order 5
  ctrl_output.doctl0.bit.DTO6 = 0;                  // LCD data output order 6
  ctrl_output.doctl1.bit.SOO =
      D_IM_DISP_ENABLE_OFF; // 0.5pix shift output (Odd line)
  ctrl_output.doctl1.bit.SOE =
      D_IM_DISP_ENABLE_OFF; // 0.5pix shift output (Even line)
  ctrl_output.doctl1.bit.ODO0 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 0 (Odd line)
  ctrl_output.doctl1.bit.ODO1 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 1 (Odd line)
  ctrl_output.doctl1.bit.ODO2 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 2 (Odd line)
  ctrl_output.doctl1.bit.ODE0 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 0 (Even line)
  ctrl_output.doctl1.bit.ODE1 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 1 (Even line)
  ctrl_output.doctl1.bit.ODE2 =
      D_IM_DISP_ENABLE_OFF; // Data output delay 2 (Even line)
  ctrl_output.doctl2.bit.CLVS = D_IM_DISP_CLVS_NORMAL; // CbCr data output level
  ctrl_output.doctl2.bit.CTOH = D_IM_DISP_CTOH_AVERAGE; // Left CbCr data
  ctrl_output.doctl2.bit.CTOV = D_IM_DISP_CTOV_TOP;     // Top CbCr data
  ctrl_output.doctl2.bit.TRSOS = D_IM_DISP_TRSOS_NONE;  // SAV/EAV code output

  im_ret =
      Im_DISP_Ctrl_Output(E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_write_channel_set_config() "
                          "Error!!:Im_DISP_Ctrl_Output() error. im_ret=%x\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  // Set callback function
  im_ret =
      Im_DISP_Set_Int_Callback(E_IM_DISP_HDMI, E_IM_DISP_INTERRUPTION_SELECT_VE,
                               disp_write_channel_vsync_callback);
  if (im_ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("fj_disp_write_channel_set_config() "
                          "Error!!:Im_DISP_Set_Int_Callback error=%08X\n",
                          im_ret));
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

// Draw mask.
FJ_ERR_CODE fj_disp_write_channel_draw(const FJ_DWCH_IMG_INFO *const img_info,
                                       E_OSDLayer layer_id) {
  FJ_ERR_CODE ret = FJ_ERR_OK;

  FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_START, "Mask set"));

  // DISP DCORE and callback function set
  ret = disp_write_channel_set_timing(img_info);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("disp_write_channel_set_timing() ercd=0x%08x.\n", ret));
    return ret;
  }

  // DISP DWCH set
  ret = disp_write_channel_set_dwch(img_info);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(("disp_write_channel_set_dwch() ercd=0x%08x.\n", ret));
    return ret;
  }

  // DISP Main Image set
  ret = disp_write_channel_set_main_image(img_info);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("disp_write_channel_set_main_image() ercd=0x%08x.\n", ret));
    return ret;
  }

  FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_END, "Mask set"));

  // clear DWCH Vsycn counter
  gBF_Disp_DWCH_Vsync_Count = 0;
  Im_DISP_PWCH_Detect_Clear(E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE);

  // Start OSD/PM for DISP macro
  disp_write_channel_start(layer_id);

  ret = disp_write_channel_stop(layer_id);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("disp_write_channel_start() error ercd=0x%08x.\n", ret));
    return ret;
  }

  // Wait DWCH end
  FJ_MBA_PRINTF(
      (FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_START, "Mask DWCH wait"));
  ret = disp_write_channel_wait_dwch_stop();
  FJ_MBA_PRINTF((FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_END, "Mask DWCH wait"));
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Error(
        ("disp_write_channel_wait_dwch_stop() error ercd=0x%08x.\n", ret));
    return ret;
  }

  return ret;
}
