/**
 * @file		hdmi.c
 * @brief		HDMI I/F API
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "hdmi.h"
#include "dd_audio.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "fj_host.h"
#include "im_disp.h"
#include "im_hdmi.h"
#include "os_user_custom.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

/*----------------------------------------------------------------------*/
/* Definition
 */
/*----------------------------------------------------------------------*/
#define D_HDMI_CEC_SIMULATE                                                    \
  (0) /**< CEC simulate switch.<br>                                            \
                                      1--On:Don't need to connect to a cec     \
         device,<br> simulate received a message by CT command.<br>                                \
                                      0--Off:Need to connect to cec device for \
         for real debug*/

#define D_HDMI_CEA_VSDB_IEEE_REGISTRATION_ID (0x030C00)
#define D_HDMI_GET_CEA_DATA_BLOCK_TAG_CODE(byte) ((byte) >> 5)
#define D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(byte) ((byte)&0x1F)
#define D_HDMI_GET_CEA_VSDB_REGISTRATION_ID(byte, index)                       \
  (((byte[(index)]) << 16) | ((byte[(index) + 1]) << 8) | (byte[(index) + 2]))

// DCC
#define D_HDMI_EDID_PROM (0x50)
#define D_HDMI_EDID_NUM (0x80)
#define D_HDMI_EDID_SEG (0x00) //(0x60)
#define D_HDMI_EDID_OFFSET (0x00)
#define D_HDMI_CEAE_NUM (0x80)
#define D_HDMI_CEAE_TAG (0x02)
#define D_HDMI_CEAE_OFFSET (0x80)

#define D_HDMI_EDID_DETAIL_1_ADDR (0x36)
#define D_HDMI_EDID_DETAIL_2_ADDR (0x48)
#define D_HDMI_EDID_DETAIL_3_ADDR (0x5A)
#define D_HDMI_EDID_DETAIL_4_ADDR (0x6C)
#define D_HDMI_EDID_EXT_FLAG_ADDR (0x7E)
#define D_HDMI_CEAE_VER_OFFSET_ADDR (0x01)
#define D_HDMI_CEAE_DTD_OFFSET_ADDR (0x02)
#define D_HDMI_CEAE_VFORM_OFFSET_ADDR (0x03)
#define D_HDMI_CEAE_DBC_OFFSET_ADDR (0x04)
#define D_HDMI_CEAE_END_OFFSET_ADDR (126)
#define D_HDMI_CEAE_AREA_SIZE (128)
#define D_HDMI_DTD_DATA_SIZE (18)
#define D_HDMI_DTD_MAX_NUM (8)

#define D_HDMI_CEAE_DBC_VENDOR_TAG (0x60)

#define D_HDMI_DDC_CMD_SEQ_READ (0x02)
#define D_HDMI_DDC_CMD_ENH_READ (0x04)
#define D_HDMI_DDC_CMD_CLEAR_FIFO (0x09)
#define D_HDMI_DDC_CMD_ABORT_TRANS (0x0F)
#define D_HDMI_DDC_CMD_CLOCK_SCL (0x0A)
#define D_HDMI_DDC_TIMEOUT_COUNT (1100)
#define D_HDMI_DDC_IN_PROG_BIT (0x10)
#define D_HDMI_DDC_ERROR_CHECK_BIT (0x04)

#define D_HDMI_720P_HSIZE (1280)
#define D_HDMI_720P_VSIZE (720)
#define D_HDMI_720P_HBLANK_60Hz (370)
#define D_HDMI_720P_HBLANK_50Hz (700)
#define D_HDMI_720P_HBLANK_30Hz (700)
#define D_HDMI_720P_HBLANK_25Hz (700)
#define D_HDMI_1080I_HSIZE (1920)
#define D_HDMI_1080I_VSIZE (1080 / 2)
#define D_HDMI_1080I_HBLANK_60Hz (280)
#define D_HDMI_1080I_HBLANK_50Hz (720)
#define D_HDMI_480P_HSIZE (720)
#define D_HDMI_480P_VSIZE (480)
#define D_HDMI_480P_HBLANK_60Hz (138)
#define D_HDMI_576P_HSIZE (720)
#define D_HDMI_576P_VSIZE (576)
#define D_HDMI_576P_HBLANK_50Hz (144)
#define D_HDMI_1080P_HSIZE (1920)
#define D_HDMI_1080P_VSIZE (1080)
#define D_HDMI_1080P_HBLANK_60Hz (280)
#define D_HDMI_1080P_HBLANK_50Hz (720)
#define D_HDMI_1080P_HBLANK_30Hz (280)
#define D_HDMI_1080P_HBLANK_24Hz (830)
#define D_HDMI_1080P_HBLANK_25Hz (720)
#define D_HDMI_VGA_HSIZE (640)
#define D_HDMI_VGA_VSIZE (480)
#define D_HDMI_VGA_HBLANK_60Hz (160)
#define D_HDMI_ASPECT_4_3_HSIZE (531)
#define D_HDMI_ASPECT_16_9_HSIZE (708)
#define D_HDMI_2160P_HSIZE (3840)
#define D_HDMI_2160P_VSIZE (2160)
#define D_HDMI_2160P_HBLANK_25Hz (1440)
#define D_HDMI_2160P_HBLANK_30Hz (560)
#define D_HDMI_2160P_HBLANK_50Hz (1440)
#define D_HDMI_2160P_HBLANK_60Hz (560)

#define D_HDMI_CLK25 (2518)
#define D_HDMI_CLK27 (2700)
#define D_HDMI_CLK74 (7425)
#define D_HDMI_CLK148 (14850)
#define D_HDMI_CLK297 (29700)
#define D_HDMI_CLK594 (59400)

#define D_HDMI_FREQ24 (24)
#define D_HDMI_FREQ25 (25)
#define D_HDMI_FREQ30 (30)
#define D_HDMI_FREQ50 (50)
#define D_HDMI_FREQ60 (60)

// AVI Info
// DBYTE1
#define D_HDMI_FORM_ACTIVE (0x10)

// DBYTE2
#define D_HDMI_ASPECT_SAMEAS (0x08)
#define D_HDMI_COL_ITU601 (0x40)
#define D_HDMI_COL_ITU709 (0x80)
#define D_HDMI_ASPECT_4_3 (0x10)
#define D_HDMI_ASPECT_16_9 (0x20)

#define D_HDMI_CHECKSUM (3)
#define D_HDMI_DBYTE1 (4)
#define D_HDMI_DBYTE2 (5)
#define D_HDMI_DBYTE4 (7)

#define D_HDMI_VID_CODE_2 (2)   // 720x 480P@59.94Hz Aspect(4:3)
#define D_HDMI_VID_CODE_4 (4)   // 1280x 720P@60Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_5 (5)   // 1920x1080I@60Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_16 (16) // 1920x1080P@60Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_17 (17) // 720x 576P@50Hz 	 Aspect(4:3)
#define D_HDMI_VID_CODE_19 (19) // 1280x 720P@50Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_20 (20) // 1920x1080I@50Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_31 (31) // 1920x1080P@50Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_33 (33) // 1920x1080P@25Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_34 (34) // 1920x1080P@30Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_61 (61) // 1280x 720P@25Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_62 (62) // 1280x 720P@30Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_93                                                     \
  (93)                          // 3840x2160P@24Hz 	 Aspect(16:9)(Not support)
#define D_HDMI_VID_CODE_94 (94) // 3840x2160P@25Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_95 (95) // 3840x2160P@30Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_96 (96) // 3840x2160P@50Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_97 (97) // 3840x2160P@60Hz 	 Aspect(16:9)
#define D_HDMI_VID_CODE_98                                                     \
  (98) // 4096x2160P@60Hz 	 Aspect(256:135)(Not support)

#define D_HDMI_VSYNC_NEG (0x00) /**< negative V sync	*/
#define D_HDMI_VSYNC_POS (0x04) /**< positive V sinc	*/

#define D_HDMI_HSYNC_NEG (0x00) /**< negative V sync	*/
#define D_HDMI_HSYNC_POS (0x02) /**< positive V sinc	*/

#define D_HDMI_INTERLACE (0x80)     /**< interlace			*/
#define D_HDMI_NON_INTERLACE (0x00) /**< non interlace		*/

// flag bit
#define D_HDMI_VSYNC_BIT (0x04)     /**< V sync bit			*/
#define D_HDMI_HSYNC_BIT (0x02)     /**< H sync bit			*/
#define D_HDMI_INTERLACE_BIT (0x80) /**< interlace bit		*/

#define D_HDMI_MAKE_SRCDEST(src, dest) (((src) << 4) | (dest))
#define D_HDMI_CPI_COMMAND_TIMING (200)  /**< CEC message timing*/
#define D_HDMI_CPI_MAX_OPERAND_SIZE (15) /**< CEC operands max size	*/

#define D_HDMI_WAIT_SEM_FOR_CEC_SEND (1000) // Send CEC messgae semaphore

#define D_HDMI_DTD_SAVE_ADDR_SZIE (6136)

#define D_HDMI_VSYNC_WAIT_END_TIME (1000)

#ifdef CO_HDMI_COMPLIANCE_TEST
//#define CO_HDMI_EDID_PRINT_ON
//#define CO_HDMI_CTRL_PRINT_ON
#else
//#define CO_HDMI_EDID_PRINT_ON
//#define CO_HDMI_CTRL_PRINT_ON
#endif

/*----------------------------------------------------------------------*/
/* Enumeration
 */
/*----------------------------------------------------------------------*/
typedef enum {
  E_HDMI_ENABLE_INERRUPT_NO = 0, // Enable interrupt
  E_HDMI_ENABLE_INERRUPT_YES     // Disable interrupt
} E_HDMI_ENABLE_INERRUPT;

typedef enum {
  E_HDMI_PROCESS_STATUS_INIT,
  E_HDMI_PROCESS_STATUS_CONNECTED,
  E_HDMI_PROCESS_STATUS_START_PROCESSING,
  E_HDMI_PROCESS_STATUS_START_END_OK,
  E_HDMI_PROCESS_STATUS_START_END_ERR,
  E_HDMI_PROCESS_STATUS_STOP_PROCESSING,
  E_HDMI_PROCESS_STATUS_STOP_END,
  E_HDMI_PROCESS_STATUS_DISCONNECT,
} E_HDMI_PROCESS_STATUS;
/*----------------------------------------------------------------------*/
/* Structure
 */
/*----------------------------------------------------------------------*/
/** DTD data malloc block union */
typedef union {
  UINT32 word[7]; /**< for word access			*/
  struct {
    FJ_HDMI_EDID_DTD dtdData; /**< dtd data */
    UCHAR reserve[3];
  } dtd; /**< for dtd access				*/
} U_HDMI_EDID_DTD_BLOCK;

/** PHY Register configuration */
typedef struct {
  USHORT pwrctrl_adr;       /**< PWRCTRL Register Address		*/
  USHORT pwrctrl_val;       /**< PWRCTRL val					*/
  USHORT opmode_pllcfg_adr; /**< OPMODE_PLLCFG Register Address	*/
  USHORT opmode_pllcfg_val; /**< OPMODE_PLLCFG val */
  USHORT cksymtxctrl_adr;   /**< CKSYMTXCTRL Register Address	*/
  USHORT cksymtxctrl_val;   /**< CKSYMTXCTRL val				*/
  USHORT vlevctrl_adr;      /**< VLEVCTRL Register Address		*/
  USHORT vlevctrl_val;      /**< VLEVCTRL val      */
  USHORT pllcurrctrl_adr;   /**< PLLCURRCTRL Register Address	*/
  USHORT pllcurrctrl_val;   /**< PLLCURRCTRL val				*/
  USHORT pllctrl_adr;       /**< PLLCTRL Register Address		*/
  USHORT pllctrl_val;       /**< PLLCTRL vals					*/
  USHORT pllgmpctrl_adr;    /**< PLLGMPCTRL Register Address	*/
  USHORT pllgmpctrl_val;    /**< PLLGMPCTRL vals				*/
  USHORT txterm_adr;        /**< TXTERM Register Address		*/
  USHORT txterm_val;        /**< TXTERM val        */
} FJ_HDMI_PHY_REG;

/*----------------------------------------------------------------------*/
/* Global Data
 */
/*----------------------------------------------------------------------*/
static volatile FJ_HDMI_EDID_DTD const
    gHDMI_Video_Format[D_HDMI_VIDEO_FORMAT_MAX] = {
        //    VideoIDCode           Active     Blank   Offset   Pulse Imagesize
        //          Pixel Freq      H     V    H   V   H    V   H  V    H    V
        //          Output format flags
        {4, D_HDMI_CLK74, 1280, 720, 370, 30, 110, 5, 40, 5, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         0}, // 1280x 720P@60Hz   16:9  FJ_DISP_VIDEO_ID_720p
        {19, D_HDMI_CLK74, 1280, 720, 700, 30, 440, 5, 40, 5, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         1}, // 1280x 720P@50Hz   16:9  FJ_DISP_VIDEO_ID_720p50
        {5, D_HDMI_CLK74, 1920, 540, 280, 22, 88, 2, 44, 5, 708, 398,
         D_HDMI_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         2}, // 1920x1080I@60Hz 	 16:9  FJ_DISP_VIDEO_ID_1080i
        {20, D_HDMI_CLK74, 1920, 540, 720, 22, 528, 2, 44, 5, 708, 398,
         D_HDMI_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         3}, // 1920x1080I@50Hz 	 16:9  FJ_DISP_VIDEO_ID_1080i25
        {2, D_HDMI_CLK27, 720, 480, 138, 45, 16, 9, 62, 6, 531, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_NEG + D_HDMI_VSYNC_NEG,
         4}, // 720x 480P@59.94Hz  4:3  FJ_DISP_VIDEO_ID_480p
        {17, D_HDMI_CLK27, 720, 576, 144, 49, 12, 5, 64, 5, 531, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_NEG + D_HDMI_VSYNC_NEG,
         5}, // 720x 576P@50Hz     4:3  FJ_DISP_VIDEO_ID_576p
        {34, D_HDMI_CLK74, 1920, 1080, 280, 45, 88, 4, 44, 5, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         6}, // 1920x1080P@30Hz   16:9  FJ_DISP_VIDEO_ID_1080p30
        {33, D_HDMI_CLK74, 1920, 1080, 720, 45, 528, 4, 44, 5, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         7}, // 1920x1080P@25Hz   16:9  FJ_DISP_VIDEO_ID_1080p25
        {16, D_HDMI_CLK148, 1920, 1080, 280, 45, 88, 4, 44, 5, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         8}, // 1920x1080P@60Hz   16:9  FJ_DISP_VIDEO_ID_1080p60
        {31, D_HDMI_CLK148, 1920, 1080, 720, 45, 528, 4, 44, 5, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         9}, // 1920x1080P@50Hz   16:9  FJ_DISP_VIDEO_ID_1080p50
        {61, D_HDMI_CLK74, 1280, 720, 700, 30, 440, 5, 40, 5, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         10}, // 1280x 720P@25Hz   16:9  FJ_DISP_VIDEO_ID_720p25
        {62, D_HDMI_CLK74, 1280, 720, 700, 30, 440, 5, 40, 5, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         11}, // 1280x 720P@30Hz   16:9  FJ_DISP_VIDEO_ID_720p30
        {94, D_HDMI_CLK297, 3840, 2160, 1440, 90, 1056, 8, 88, 10, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         12}, // 3840x2160P@25Hz   16:9  FJ_DISP_VIDEO_ID_2160p25_3840
        {95, D_HDMI_CLK297, 3840, 2160, 560, 90, 176, 8, 88, 10, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         13}, // 3840x2160P@30Hz   16:9  FJ_DISP_VIDEO_ID_2160p30_3840
        {97, D_HDMI_CLK594, 3840, 2160, 560, 90, 176, 8, 88, 10, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         14}, // 3840x2160P@60Hz   16:9  FJ_DISP_VIDEO_ID_2160p60_3840
        {96, D_HDMI_CLK594, 3840, 2160, 1440, 90, 1056, 8, 88, 10, 708, 398,
         D_HDMI_NON_INTERLACE + D_HDMI_HSYNC_POS + D_HDMI_VSYNC_POS,
         15}, // 3840x2160P@50Hz   16:9  FJ_DISP_VIDEO_ID_2160p50_3840
};

static volatile FJ_HDMI_PHY_REG const
    gHDMI_PHY_Config[D_HDMI_VIDEO_FORMAT_MAX] = {
        //    PWRCTRL,        OPMODE_PLLCFG,  CKSYMTXCTRL,    VLEVCTRL,
        //    PLLCURRCTRL,    PLLCTRL,        PLLGMPCTRL,     TXTERM
        //    adr     val     adr     val     adr     val     adr     val adr
        //    val     adr     val     adr     val     adr     val
        {0x0000, 0x0000, 0x0006, 0x0072, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0004, 0x0014, 0x0000, 0x0015, 0x0001, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0072, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0004, 0x0014, 0x0000, 0x0015, 0x0001, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0072, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0004, 0x0014, 0x0000, 0x0015, 0x0001, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0072, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0004, 0x0014, 0x0000, 0x0015, 0x0001, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0113, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0000, 0x0014, 0x0000, 0x0015, 0x0000, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0113, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0000, 0x0014, 0x0000, 0x0015, 0x0000, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0072, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0004, 0x0014, 0x0000, 0x0015, 0x0001, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0072, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0004, 0x0014, 0x0000, 0x0015, 0x0001, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0051, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x001B, 0x0014, 0x0000, 0x0015, 0x0002, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0051, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x001B, 0x0014, 0x0000, 0x0015, 0x0002, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0072, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0004, 0x0014, 0x0000, 0x0015, 0x0001, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0072, 0x0009, 0x8009, 0x000E, 0x026E, 0x0010,
         0x0004, 0x0014, 0x0000, 0x0015, 0x0001, 0x0019, 0x0007},
        {0x0000, 0x0000, 0x0006, 0x0040, 0x0009, 0x8029, 0x000E, 0x026E, 0x0010,
         0x0036, 0x0014, 0x0000, 0x0015, 0x0003, 0x0019, 0x0004},
        {0x0000, 0x0000, 0x0006, 0x0040, 0x0009, 0x8029, 0x000E, 0x026E, 0x0010,
         0x0036, 0x0014, 0x0000, 0x0015, 0x0003, 0x0019, 0x0004},
        {0x0000, 0x0000, 0x0006, 0x1a40, 0x0009, 0x8039, 0x000E, 0x014a, 0x0010,
         0x003f, 0x0014, 0x0000, 0x0015, 0x0003, 0x0019, 0x0000},
        {0x0000, 0x0000, 0x0006, 0x1a40, 0x0009, 0x8039, 0x000E, 0x014a, 0x0010,
         0x003f, 0x0014, 0x0000, 0x0015, 0x0003, 0x0019, 0x0000},
};

const FJ_HDMI_N_VAL gHDMI_N_Config[D_HDMI_VIDEO_FORMAT_MAX] = {
    //     sample32,    sample48,    sample96,      sample44.1
    {4096, 11648, 6144, 11648, 12288, 23296, 6272, 17836},
    {4096, 11648, 6144, 11648, 12288, 23296, 6272, 17836},
    {4096, 11648, 6144, 11648, 12288, 23296, 6272, 17836},
    {4096, 11648, 6144, 11648, 12288, 23296, 6272, 17836},
    {4096, 4096, 6144, 6144, 12288, 12288, 6272, 6272},
    {4096, 4096, 6144, 6144, 12288, 12288, 6272, 6272},
    {4096, 11648, 6144, 11648, 12288, 23296, 6272, 17836},
    {4096, 11648, 6144, 11648, 12288, 23296, 6272, 17836},
    {4096, 11648, 6144, 5824, 12288, 11648, 6272, 8918},
    {4096, 11648, 6144, 5824, 12288, 11648, 6272, 8918},
    {4096, 11648, 6144, 11648, 12288, 23296, 6272, 17836},
    {4096, 11648, 6144, 11648, 12288, 23296, 6272, 17836},
    {3072, 5824, 5120, 5824, 10240, 11648, 4704, 4459},
    {3072, 5824, 5120, 5824, 10240, 11648, 4704, 4459},
    {3072, 5824, 6144, 5824, 12288, 11648, 9408, 8918},
    {3072, 5824, 6144, 5824, 12288, 11648, 9408, 8918},
};

static ULONG gHDMI_Short_Video_Type_Only = FJ_HDMI_FORMAT_NO;

static U_HDMI_EDID_DTD_BLOCK gHDMI_DTD_Data_Block[D_HDMI_DTD_MAX_NUM];
static FJ_HDMI_DTD_MAN gHDMI_DTD_MAN_Ctrl = {
    .dtd_num = 0,
    .dtd_addr = NULL,
};

static FJ_HDMI_DTD_MAN *gHDMI_DTD_MAN = NULL; // Detailed Timing Descriptor
static volatile HDMI_CALLBACK gHDMI_INTERRUPT_Callback = NULL;
static volatile FJ_HDMI_HPD gHDMI_HPD = FJ_HDMI_HPD_DETACH;
static volatile UINT32 gHDMI_Audio_Frequency = FJ_HDMI_AUDIO_44100HZ;
static BOOL gHDMI_EDID_Is_Update = FALSE; /**< flag: EDID is got */
static CHAR gHDMI_DTD_Save_Addr[D_HDMI_DTD_SAVE_ADDR_SZIE] = {0};
static E_HDMI_PROCESS_STATUS gHDMI_Process_Status = E_HDMI_PROCESS_STATUS_INIT;

static FJ_DISP_HDMI_DEV_VENDOR_PRODUCT_ID gHDMI_EDID_Vendor_ID;
UCHAR gPhy_Index;
UCHAR gVideo_fmt = D_IM_HDMI_OUT_YCC422;

#ifdef CO_HDMI_COMPLIANCE_TEST
extern UINT32 gRS_System_Disp_Target_RGB;
#endif

static USHORT gHDMI_Phy_TXCTRL_Value = 0xFFFF;
static USHORT gHDMI_Phy_VLEVCTRL_Value = 0xFFFF;
/*----------------------------------------------------------------------*/
/* Macro
 */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Method Definition
 */
/*----------------------------------------------------------------------*/
static VOID hdmi_interrupt_callback(UCHAR interrupt);
#if 0
static FJ_HDMI_HPD hdmi_plug_detect( VOID );
#endif

static USHORT hdmi_active_size_cal(UCHAR d1, UCHAR d2);
static USHORT hdmi_blank_size_cal(UCHAR d1, UCHAR d2);
static USHORT hdmi_h_sync_size_cal(UCHAR d1, UCHAR d2);
static USHORT hdmi_v_sync_size_cal(UCHAR d1, UCHAR d2);
static USHORT hdmi_h_puls_size_cal(UCHAR d1, UCHAR d2);
static USHORT hdmi_v_puls_size_cal(UCHAR d1, UCHAR d2);
static USHORT hdmi_h_image_size_cal(UCHAR d1, UCHAR d2);
static USHORT hdmi_v_image_size_cal(UCHAR d1, UCHAR d2);
static INT32 hdmi_ddc_checksum(UCHAR *addr, USHORT size);
static VOID hdmi_convert_video_timing(UCHAR *seach_addr,
                                      FJ_HDMI_EDID_DTD *save_addr);
static INT32 hdmi_get_edid_proc(VOID);
static VOID hdmi_convert_short_audio_type(UCHAR *tag_addr);
static ULONG hdmi_convert_short_video_type(UCHAR short_video_descriptor);
static FJ_HDMI_HPD hdmi_get_edid(FJ_HDMI_DTD_MAN **dtd_addr,
                                 UCHAR cea_data_block_flags);
static INT32 hdmi_get_dtd_no(UINT32 video_format, UINT32 *dtd_no);
static INT32 hdmi_get_format_tbl_addr(UINT32 video_format,
                                      FJ_HDMI_EDID_DTD **edid_addr);
static VOID hdmi_get_aspect_type(USHORT width, USHORT lines, BYTE *asp_type);
static VOID hdmi_set_process_status(E_HDMI_PROCESS_STATUS status);
static E_HDMI_PROCESS_STATUS hdmi_check_process_status(VOID);

/*----------------------------------------------------------------*/
/* Local Function
 */
/*----------------------------------------------------------------*/

static VOID hdmi_interrupt_callback(UCHAR interrupt) {
  if (gHDMI_INTERRUPT_Callback != NULL) {
    (*gHDMI_INTERRUPT_Callback)(interrupt);
  }
}

#if 0
static FJ_HDMI_HPD hdmi_plug_detect( VOID )
{
	FJ_HDMI_SYS_STAT status;
	
	Im_HDMI_Get_Phy_Status((U_IM_HDMI_PHY_STAT0*) &status );

	if ( status.bit.HPD == 1 ) {
		return FJ_HDMI_HPD_ATTACH;
	}
	else {
		return FJ_HDMI_HPD_DETACH;
	}
}
#endif

/**
 * @brief		DTD format data change
 * @param[in]	UCHAR d1	: data1
 * @param[in]	UCHAR d2	: data2
 * @param[out]	None
 * @return		None
 */
static USHORT hdmi_active_size_cal(UCHAR d1, UCHAR d2) {
  return (((USHORT)d2 & 0xF0) << 4 | (USHORT)d1);
}

static USHORT hdmi_blank_size_cal(UCHAR d1, UCHAR d2) {
  return (((USHORT)d2 & 0x0F) << 8 | (USHORT)d1);
}

static USHORT hdmi_h_sync_size_cal(UCHAR d1, UCHAR d2) {
  return (((USHORT)d2 & 0xC0) << 2 | (USHORT)d1);
}

static USHORT hdmi_v_sync_size_cal(UCHAR d1, UCHAR d2) {
  return (USHORT)(((USHORT)d2 & 0x0C) << 2 | ((USHORT)d1 & 0xF0) >> 4);
}

static USHORT hdmi_h_puls_size_cal(UCHAR d1, UCHAR d2) {
  return ((((USHORT)d2 & 0x30) << 4) | (USHORT)d1);
}

static USHORT hdmi_v_puls_size_cal(UCHAR d1, UCHAR d2) {
  return ((((USHORT)d2 & 0x03) << 4) | ((USHORT)d1 & 0x0F));
}

static USHORT hdmi_h_image_size_cal(UCHAR d1, UCHAR d2) {
  return ((((USHORT)d2 & 0xF0) << 4) | (USHORT)d1);
}

static USHORT hdmi_v_image_size_cal(UCHAR d1, UCHAR d2) {
  return ((((USHORT)d2 & 0x0F) << 8) | (USHORT)d1);
}

/**
 * @brief		EDID,CEA checksum value calculate
 * @param[in]	UCHAR* addr	: data address
 * @param[in]	USHORT size	: data size
 * @param[out]	None
 * @return		FJ_ERR_OK / FJ_ERR_NG
 */
static INT32 hdmi_ddc_checksum(UCHAR *addr, USHORT size) {

  USHORT cnt;
  UCHAR sum = 0;

  for (cnt = 0, sum = 0; cnt < size; cnt++) {
    sum += addr[cnt];
  }
  if (sum != 0) {
    return FJ_ERR_NG;
  } else {
    return FJ_ERR_OK;
  }
}

/**
 * @brief		Convert DTD format and Save HDMI output timing
 * @param[in]	UCHAR* seach_addr	: DTD format data address
 * @param[in]	FJ_HDMI_EDID_DTD* save_addr	: save address
 * @param[out]	None
 * @return		None
 */
static VOID hdmi_convert_video_timing(UCHAR *seach_addr,
                                      FJ_HDMI_EDID_DTD *save_addr) {
  USHORT conv_data;

  if (*seach_addr != 0) {

#if HDMI_ENDIAN
    save_addr->PixelClock = (((*seach_addr) << 8) + (*(seach_addr + 1)));
#else
    save_addr->PixelClock = (((*(seach_addr + 1)) << 8) | (*(seach_addr)));
#endif

    seach_addr += 2;
    conv_data = hdmi_active_size_cal(*seach_addr, *(seach_addr + 2));
#if HDMI_ENDIAN
    save_addr->H_Active =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->H_Active = conv_data;
#endif

    conv_data = hdmi_active_size_cal(*(seach_addr + 3), *(seach_addr + 5));
#if HDMI_ENDIAN
    save_addr->V_Active =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->V_Active = conv_data;
#endif

    conv_data = hdmi_blank_size_cal(*(seach_addr + 1), *(seach_addr + 2));
#if HDMI_ENDIAN
    save_addr->H_Blanking =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->H_Blanking = conv_data;
#endif

    conv_data = hdmi_blank_size_cal(*(seach_addr + 4), *(seach_addr + 5));
#if HDMI_ENDIAN
    save_addr->V_Blanking =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->V_Blanking = conv_data;
#endif

    conv_data = hdmi_h_sync_size_cal(*(seach_addr + 6), *(seach_addr + 9));
#if HDMI_ENDIAN
    save_addr->H_SyncOffset =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->H_SyncOffset = conv_data;
#endif

    conv_data = hdmi_v_sync_size_cal(*(seach_addr + 8), *(seach_addr + 9));
#if HDMI_ENDIAN
    save_addr->V_SyncOffset =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->V_SyncOffset = conv_data;
#endif

    conv_data = hdmi_h_puls_size_cal(*(seach_addr + 7), *(seach_addr + 9));
#if HDMI_ENDIAN
    save_addr->H_SyncPulseWidth =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->H_SyncPulseWidth = conv_data;
#endif

    conv_data = hdmi_v_puls_size_cal(*(seach_addr + 8), *(seach_addr + 9));
#if HDMI_ENDIAN
    save_addr->V_SyncPulseWidth =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->V_SyncPulseWidth = conv_data;
#endif

    conv_data = hdmi_h_image_size_cal(*(seach_addr + 10), *(seach_addr + 12));
#if HDMI_ENDIAN
    save_addr->H_ImageSize =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->H_ImageSize = conv_data;
#endif

    conv_data = hdmi_v_image_size_cal(*(seach_addr + 11), *(seach_addr + 12));
#if HDMI_ENDIAN
    save_addr->V_ImageSize =
        (((conv_data & 0x00FF) << 8) + ((conv_data & 0xFF00) >> 8));
#else
    save_addr->V_ImageSize = conv_data;
#endif

    save_addr->Flags = *(seach_addr + 15);
  }
}

/**
 * @brief		Save HDMI Audio Data Block
 * @param[in]	UCHAR* tag_addr	: Short Audio Descriptors address
 * @param[out]	None
 * @return		None
 */
static VOID hdmi_convert_short_audio_type(UCHAR *tag_addr) {
  USHORT short_audio_cnt = 0;

  while (short_audio_cnt < gHDMI_DTD_MAN_Ctrl.short_audio_count) {
    gHDMI_DTD_MAN_Ctrl.short_audio_desc[short_audio_cnt].audio_type =
        (FJ_HDMI_EDID_AUDIO_FORMAT)(*(tag_addr + 1 + (short_audio_cnt * 3)) >>
                                    3);
    gHDMI_DTD_MAN_Ctrl.short_audio_desc[short_audio_cnt].audio_max_channels =
        (FJ_HDMI_EDID_AUDIO_CHANNEL)(*(tag_addr + 1 + (short_audio_cnt * 3)) &
                                     0x07);
    gHDMI_DTD_MAN_Ctrl.short_audio_desc[short_audio_cnt].audio_sample_rate =
        *(tag_addr + 1 + (short_audio_cnt * 3 + 1));
    gHDMI_DTD_MAN_Ctrl.short_audio_desc[short_audio_cnt].audio_dbyte =
        *(tag_addr + 1 + (short_audio_cnt * 3 + 2));
    gHDMI_DTD_MAN_Ctrl.short_audio_desc[short_audio_cnt].audio_sample_size =
        ((*(tag_addr + 1 + (short_audio_cnt * 3 + 2)) & 0x07) << 1);

    short_audio_cnt++;
  }
}

/**
 * @brief		Convert Video format
 * @param[in]	UCHAR short_video_descriptor : Short Video Descriptor
 * @param[out]	None
 * @return		Video format
 */
static ULONG hdmi_convert_short_video_type(UCHAR short_video_descriptor) {
  ULONG short_video_type = FJ_HDMI_FORMAT_NO;

  switch (short_video_descriptor & 0x7f) {
  case D_HDMI_VID_CODE_4:
    short_video_type = FJ_HDMI_1280x720p60Hz_16_9;
    break;
  case D_HDMI_VID_CODE_19:
    short_video_type = FJ_HDMI_1280x720p50Hz_16_9;
    break;
  case D_HDMI_VID_CODE_5:
    short_video_type = FJ_HDMI_1920x1080i60Hz_16_9;
    break;
  case D_HDMI_VID_CODE_20:
    short_video_type = FJ_HDMI_1920x1080i50Hz_16_9;
    break;
  case D_HDMI_VID_CODE_2:
    short_video_type = FJ_HDMI_720x480p60Hz_4_3;
    break;
  case D_HDMI_VID_CODE_17:
    short_video_type = FJ_HDMI_720x576p50Hz_4_3;
    break;
  case D_HDMI_VID_CODE_34:
    short_video_type = FJ_HDMI_1920x1080p30Hz_16_9;
    break;
  case D_HDMI_VID_CODE_33:
    short_video_type = FJ_HDMI_1920x1080p25Hz_16_9;
    break;
  case D_HDMI_VID_CODE_16:
    short_video_type = FJ_HDMI_1920x1080p60Hz_16_9;
    break;
  case D_HDMI_VID_CODE_31:
    short_video_type = FJ_HDMI_1920x1080p50Hz_16_9;
    break;
  case D_HDMI_VID_CODE_61:
    short_video_type = FJ_HDMI_1280x720p25Hz_16_9;
    break;
  case D_HDMI_VID_CODE_62:
    short_video_type = FJ_HDMI_1280x720p30Hz_16_9;
    break;
  case D_HDMI_VID_CODE_94:
    short_video_type = FJ_HDMI_3840x2160p25Hz_16_9;
    break;
  case D_HDMI_VID_CODE_95:
    short_video_type = FJ_HDMI_3840x2160p30Hz_16_9;
    break;
  case D_HDMI_VID_CODE_96:
    short_video_type = FJ_HDMI_3840x2160p50Hz_16_9;
    break;
  case D_HDMI_VID_CODE_97:
    short_video_type = FJ_HDMI_3840x2160p60Hz_16_9;
    break;
  default:
    break;
  }

  return short_video_type;
}

/**
 * @brief		Get EDID process
 * @param[in]	None
 * @param[out]	None
 * @return		FJ_ERR_OK / FJ_ERR_NG
 */
static INT32 hdmi_get_edid_proc(VOID) {
  UCHAR EDID[128];
  UCHAR CEAE[128];
  UCHAR dtd_addr;
  INT32 ret;
  USHORT dtd_num;
  FJ_HDMI_EDID_DTD **save_addr;
  FJ_HDMI_EDID_DTD *edid_dtd;
  INT32 cea_num;
  INT32 i, j;
  UCHAR EDID_HEADER[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
  UCHAR MONITER_RANGE_HEADER[] = {0x00, 0x00, 0x00, 0xFD, 0x00};
  UCHAR monitor_freq24;
  UCHAR monitor_freq25;
  UCHAR monitor_freq30;
  UCHAR monitor_freq50;
  UCHAR monitor_freq60;
  ULONG monitor_type;
  FJ_HDMI_SYS_STAT status;
  T_IM_HDMI_I2CM_CONFIG i2cm_config;
  UCHAR read_data[8];

  ULONG short_video_type = FJ_HDMI_FORMAT_NO;
  ULONG short_video_type_tmp = FJ_HDMI_FORMAT_NO;
  UCHAR dtd_offset;
  UCHAR short_video_cnt = 0;
  UCHAR yuv420_cnt = 0;

  UCHAR latency_fields_present = 0;
  UCHAR i_latency_fields_present = 0;
  UCHAR hdmi_video_present = 0;
  UCHAR hdmi_vic_len = 0;
  UCHAR index = 0;

  gHDMI_DTD_MAN_Ctrl.dev_inf = FJ_HDMI_DISPLAY_INTERFACE_UNKNOWN;
  gHDMI_DTD_MAN_Ctrl.short_audio_count = 0;
  gHDMI_DTD_MAN_Ctrl.speaker_data_block = 0;
  gHDMI_DTD_MAN_Ctrl.vesa_dtc_size = 0;
  gHDMI_DTD_MAN_Ctrl.use_extended_size = 0;

  // HPD check
  Im_HDMI_Get_Phy_Status((U_IM_HDMI_PHY_STAT0 *)&status);

  if (status.bit.HPD == 0) {
    BF_Debug_Print_Error(("hdmi_get_edid_proc: HPD = 0.\n"));
    return FJ_ERR_NG;
  }

  if (gHDMI_DTD_MAN_Ctrl.dtd_num != 0) {
    memset(gHDMI_DTD_Save_Addr, 0, sizeof(gHDMI_DTD_Save_Addr));
    gHDMI_DTD_MAN_Ctrl.dtd_num = 0;
  }
  memset(EDID, 0, sizeof(EDID));
  memset(CEAE, 0, sizeof(EDID));

  i2cm_config.i2cm_slaveaddr = D_HDMI_EDID_PROM;
  i2cm_config.i2cm_operation = E_IM_HDMI_I2CM_OPERATION_RD8;
  for (i = 0; i < 16; i++) {
    i2cm_config.i2cm_address = i * 8;
    // Read data.
    ret = Im_HDMI_Read_Sinks_E_EDID(&i2cm_config, &read_data[0]);
    if (ret != D_DDIM_OK) {
      return FJ_ERR_NG;
    }

    memcpy(&EDID[i * 8], &read_data[0], 8);
  }

#ifdef CO_HDMI_EDID_PRINT_ON
#ifdef FJ_DEBUG_PRINT_INFO_ON
  // debug
  {
    USHORT i, j;

    BF_Debug_Print_Information(("----- EDID\n"));
    for (i = 0; i < 8; i++) {
      CHAR str_buf[3 * 16 + 1] = "";
      CHAR *str_buf_p = str_buf;
      for (j = 0; j < 16; j++) {
        sprintf(str_buf_p, "%02x ", EDID[(i * 16) + j]);
        str_buf_p += strlen(str_buf_p);
      }
      BF_Debug_Print_Information(("%s\n", str_buf));
    }
  }
#endif
#endif

  if (memcmp(&EDID[0], &EDID_HEADER[0], sizeof(EDID_HEADER)) != 0) {
    BF_Debug_Print_Error(
        ("hdmi_get_edid_proc: EDID header error. Try to repair!\n"));
    memcpy(&EDID[0], &EDID_HEADER[0], sizeof(EDID_HEADER));
  }

  if (hdmi_ddc_checksum(&EDID[0], D_HDMI_EDID_NUM) != 0) {
    BF_Debug_Print_Error(("hdmi_get_edid_proc: hdmi_ddc_checksum error.\n"));
    return FJ_ERR_NG;
  }

  if (memcmp(&EDID[0], &EDID_HEADER[0], sizeof(EDID_HEADER)) != 0) {
    BF_Debug_Print_Error(("hdmi_get_edid_proc: EDID check error.\n"));
    return FJ_ERR_NG;
  }

  cea_num = EDID[D_HDMI_EDID_EXT_FLAG_ADDR];

  if (cea_num == 0) {
    gHDMI_DTD_MAN_Ctrl.dev_inf = FJ_HDMI_DISPLAY_INTERFACE_DVI;
  }

  // Allocate memory -> CEA num x 1block maxnum(6) +
  // 4(Primary,Secondary,Monitor,Second Monitor)
  save_addr = (FJ_HDMI_EDID_DTD **)gHDMI_DTD_Save_Addr;
  if (save_addr == NULL) {
    BF_Debug_Print_Error(
        ("hdmi_get_edid_proc: gHDMI_DTD_Save_Addr NULL check error.\n"));
    return FJ_ERR_NG;
  }
  gHDMI_DTD_MAN_Ctrl.dtd_addr = (ULONG *)save_addr;

  gHDMI_DTD_MAN_Ctrl.phy_addr = 0xFFFF;
  gHDMI_DTD_MAN_Ctrl.out_fmat = FJ_HDMI_OUT_RGB;
  gHDMI_DTD_MAN_Ctrl.scn_mode = FJ_HDMI_SCAN_OVER;
  dtd_num = 0;

  if ((EDID[D_HDMI_EDID_DETAIL_1_ADDR] != 0) ||
      (EDID[D_HDMI_EDID_DETAIL_1_ADDR + 1] != 0)) {
    save_addr[dtd_num] = (FJ_HDMI_EDID_DTD *)&gHDMI_DTD_Data_Block[dtd_num];
    hdmi_convert_video_timing(&EDID[D_HDMI_EDID_DETAIL_1_ADDR],
                              save_addr[dtd_num]);
    dtd_num++; /* pgr0524 */
  }
  if ((EDID[D_HDMI_EDID_DETAIL_2_ADDR] != 0) ||
      (EDID[D_HDMI_EDID_DETAIL_2_ADDR + 1] != 0)) {
    save_addr[dtd_num] = (FJ_HDMI_EDID_DTD *)&gHDMI_DTD_Data_Block[dtd_num];
    hdmi_convert_video_timing(&EDID[D_HDMI_EDID_DETAIL_2_ADDR],
                              save_addr[dtd_num]);
    dtd_num++; /* pgr0524 */
  }
  if ((EDID[D_HDMI_EDID_DETAIL_3_ADDR] != 0) ||
      (EDID[D_HDMI_EDID_DETAIL_3_ADDR + 1] != 0)) {
    save_addr[dtd_num] = (FJ_HDMI_EDID_DTD *)&gHDMI_DTD_Data_Block[dtd_num];
    hdmi_convert_video_timing(&EDID[D_HDMI_EDID_DETAIL_3_ADDR],
                              save_addr[dtd_num]);
    dtd_num++; /* pgr0524 */
  }
  if ((EDID[D_HDMI_EDID_DETAIL_4_ADDR] != 0) ||
      (EDID[D_HDMI_EDID_DETAIL_4_ADDR + 1] != 0)) {
    save_addr[dtd_num] = (FJ_HDMI_EDID_DTD *)&gHDMI_DTD_Data_Block[dtd_num];
    hdmi_convert_video_timing(&EDID[D_HDMI_EDID_DETAIL_4_ADDR],
                              save_addr[dtd_num]);
    dtd_num++; /* pgr0524 */
  }
  gHDMI_DTD_MAN_Ctrl.dtd_num = dtd_num;

  for (i = 1; i <= cea_num; i++) {
    i2cm_config.i2cm_slaveaddr = D_HDMI_EDID_PROM;
    i2cm_config.i2cm_operation = E_IM_HDMI_I2CM_OPERATION_RD8_EXT;
    i2cm_config.i2cm_segaddr = 0x30;
    i2cm_config.i2cm_segptr = (i / 2);
    for (j = 0; j < 16; j++) {
      i2cm_config.i2cm_address = (D_HDMI_CEAE_OFFSET * (i & 1)) + (j * 8);
      // Read data.
      ret = Im_HDMI_Read_Sinks_E_EDID(&i2cm_config, &read_data[0]);
      if (ret != D_DDIM_OK) {
        BF_Debug_Print_Error(("hdmi_get_edid_proc: Im_HDMI_Read_Sinks_E_EDID() "
                              "error. ret = %x.\n",
                              ret));
        return FJ_ERR_NG;
      }

      memcpy(&CEAE[j * 8], &read_data[0], 8);
    }

#ifdef CO_HDMI_EDID_PRINT_ON
#ifdef FJ_DEBUG_PRINT_INFO_ON
    // debug
    {
      USHORT i, j;

      BF_Debug_Print_Information(("----- CEAE\n"));
      for (i = 0; i < 8; i++) {
        CHAR str_buf[3 * 16 + 1] = "";
        CHAR *str_buf_p = str_buf;
        for (j = 0; j < 16; j++) {
          sprintf(str_buf_p, "%02x ", CEAE[(i * 16) + j]);
          str_buf_p += strlen(str_buf_p);
        }
        BF_Debug_Print_Information(("%s\n", str_buf));
      }
    }
#endif
#endif

    //#ifndef CO_HDMI_COMPLIANCE_TEST
    if (hdmi_ddc_checksum(&CEAE[0], D_HDMI_CEAE_NUM) != 0) {
      BF_Debug_Print_Error(
          ("hdmi_get_edid_proc: hdmi_ddc_checksum() error.\n"));
      return FJ_ERR_NG; /* pgr0524 */
    }
    //#endif

    if (CEAE[0] != D_HDMI_CEAE_TAG)
      continue;

#if 0  // _DD_HDMI_CEA_
		if(gHDMI_DTD_MAN_Ctrl.dev_inf == FJ_HDMI_DISPLAY_INTERFACE_UNKNOWN) {
			if( CEAE[ D_DD_HDMI_CEAE_VER_OFFSET_ADDR ] == 3 ) {
				gHDMI_DTD_MAN_Ctrl.dev_inf = FJ_HDMI_DISPLAY_INTERFACE_DVI;
			}
		}
#endif // _DD_HDMI_CEA_

    if (CEAE[D_HDMI_CEAE_VER_OFFSET_ADDR] == 3) {
      if ((CEAE[D_HDMI_CEAE_VFORM_OFFSET_ADDR] & 0x30) == 0x00) {
        gHDMI_DTD_MAN_Ctrl.out_fmat = FJ_HDMI_OUT_RGB;
        gVideo_fmt = D_IM_HDMI_OUT_RGB;
      } else if ((CEAE[D_HDMI_CEAE_VFORM_OFFSET_ADDR] & 0x30) == 0x20) {
        gHDMI_DTD_MAN_Ctrl.out_fmat = FJ_HDMI_OUT_YCC444;
        gVideo_fmt = D_IM_HDMI_OUT_YCC444;
      } else {
        gHDMI_DTD_MAN_Ctrl.out_fmat = FJ_HDMI_OUT_YCC422;
        gVideo_fmt = D_IM_HDMI_OUT_YCC422;
      }

      if ((CEAE[D_HDMI_CEAE_VFORM_OFFSET_ADDR] & 0x80) == 0x80) {
        gHDMI_DTD_MAN_Ctrl.scn_mode = FJ_HDMI_SCAN_UNDER;
      } else {
        gHDMI_DTD_MAN_Ctrl.scn_mode = FJ_HDMI_SCAN_OVER;
      }
    }

    dtd_offset = CEAE[D_HDMI_CEAE_DTD_OFFSET_ADDR];
    dtd_addr = 4;
    while (dtd_addr < dtd_offset) {
      switch (D_HDMI_GET_CEA_DATA_BLOCK_TAG_CODE(CEAE[dtd_addr])) {
      case FJ_HDMI_EDID_DBC_TAG_AUDIO:
        if (gHDMI_DTD_MAN_Ctrl.data_block_flags & FJ_HDMI_EDID_TAG_FLAG_AUDIO) {
          gHDMI_DTD_MAN_Ctrl.short_audio_count =
              (D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(CEAE[dtd_addr]) / 3);
          if (gHDMI_DTD_MAN_Ctrl.short_audio_count > 0) {
            hdmi_convert_short_audio_type(&CEAE[dtd_addr]);
          }
        }
        break;
      case FJ_HDMI_EDID_DBC_TAG_VIDEO:
        if (gHDMI_DTD_MAN_Ctrl.data_block_flags & FJ_HDMI_EDID_TAG_FLAG_VIDEO) {
          for (short_video_cnt = 0;
               short_video_cnt <
               (UCHAR)D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(CEAE[dtd_addr]);
               short_video_cnt++) {
            short_video_type |= hdmi_convert_short_video_type(
                CEAE[dtd_addr + 1 + short_video_cnt]);
          }
        }
        break;
      case FJ_HDMI_EDID_DBC_TAG_VENDOR:
        if (gHDMI_DTD_MAN_Ctrl.data_block_flags &
            FJ_HDMI_EDID_TAG_FLAG_VENDOR) {
          if (gHDMI_DTD_MAN_Ctrl.dev_inf == FJ_HDMI_DISPLAY_INTERFACE_UNKNOWN) {
            if (D_HDMI_GET_CEA_VSDB_REGISTRATION_ID(CEAE, dtd_addr + 1) !=
                D_HDMI_CEA_VSDB_IEEE_REGISTRATION_ID) {
              gHDMI_DTD_MAN_Ctrl.dev_inf = FJ_HDMI_DISPLAY_INTERFACE_DVI;
            } else {
              gHDMI_DTD_MAN_Ctrl.dev_inf = FJ_HDMI_DISPLAY_INTERFACE_HDMI;
            }
          }
          gHDMI_DTD_MAN_Ctrl.phy_addr =
              CEAE[dtd_addr + 4] << 8 | CEAE[dtd_addr + 5];

          if (D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(CEAE[dtd_addr]) >= 8) {
            hdmi_video_present = ((CEAE[dtd_addr + 8] >> 5) & 0x01);
            i_latency_fields_present = ((CEAE[dtd_addr + 8] >> 6) & 0x01);
            latency_fields_present = ((CEAE[dtd_addr + 8] >> 7) & 0x01);
            index = 0;
            if (latency_fields_present == 1) {
              index += 2;
            }
            if (i_latency_fields_present == 1) {
              index += 2;
            }
            if (hdmi_video_present == 1) {
              hdmi_vic_len = (CEAE[dtd_addr + 8 + index + 2] >> 5);
              for (int cnt = 0; cnt < hdmi_vic_len; cnt++) {
                switch (CEAE[dtd_addr + 8 + index + 3 + cnt]) {
                case 0x01:
                  // 3840x2160p30(VIC=95)
                  short_video_type |=
                      hdmi_convert_short_video_type(D_HDMI_VID_CODE_95);
                  break;
                case 0x02:
                  // 3840x2160p25(VIC=94)
                  short_video_type |=
                      hdmi_convert_short_video_type(D_HDMI_VID_CODE_94);
                  break;
                case 0x03:
                  // 3840x2160p24(VIC=93)
                  short_video_type |=
                      hdmi_convert_short_video_type(D_HDMI_VID_CODE_93);
                  break;
                case 0x04:
                  // 4096x2160p24(VIC=98)
                  short_video_type |=
                      hdmi_convert_short_video_type(D_HDMI_VID_CODE_98);
                  break;
                default:
                  break;
                }
              }
            }
          }
        }
        break;
      case FJ_HDMI_EDID_DBC_TAG_SPEAKER:
        if (gHDMI_DTD_MAN_Ctrl.data_block_flags &
            FJ_HDMI_EDID_TAG_FLAG_SPEAKER) {
          gHDMI_DTD_MAN_Ctrl.speaker_data_block = (CEAE[dtd_addr + 1] & 0x7F);
        }
        break;
      case FJ_HDMI_EDID_DBC_TAG_EXTENDED:
        // Use Extended Tag(0x7)
        if (gHDMI_DTD_MAN_Ctrl.data_block_flags &
            FJ_HDMI_EDID_TAG_FLAG_EXTENDED) {
          if (D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(CEAE[dtd_addr]) < 31) {
            gHDMI_DTD_MAN_Ctrl.use_extended_size =
                D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(CEAE[dtd_addr]);
            memcpy(gHDMI_DTD_MAN_Ctrl.use_extended_dbyte, &CEAE[dtd_addr + 1],
                   gHDMI_DTD_MAN_Ctrl.use_extended_size);

            if (CEAE[dtd_addr + 1] == 0xF) {
#if 0
							// Extended Tag Codes(YCBCR 4:2:0 Capability Bit Map)
							if ( gHDMI_DTD_MAN_Ctrl.use_extended_size == 0x1 ){
								gVideo_fmt = D_IM_HDMI_OUT_YCC420;
							}
							else {
								gVideo_fmt = D_IM_HDMI_OUT_YCC420;
								for( yuv420_cnt=0; yuv420_cnt < (UCHAR)D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH( CEAE[dtd_addr] )-1; yuv420_cnt++ ) {
									if ( CEAE[dtd_addr+2+yuv420_cnt] != 0xFF ) {
										gVideo_fmt = D_IM_HDMI_OUT_YCC422;
										break;
									}
								}
							}
#endif
            } else if (CEAE[dtd_addr + 1] == 0xE) {
              // Extended Tag Codes(YCBCR 4:2:0 Video Data Block)
              for (yuv420_cnt = 0;
                   yuv420_cnt < (UCHAR)(D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(
                                            CEAE[dtd_addr]) -
                                        1);
                   yuv420_cnt++) {
                short_video_type_tmp |= hdmi_convert_short_video_type(
                    CEAE[dtd_addr + 2 + yuv420_cnt]);
              }

              if ((short_video_type_tmp & FJ_HDMI_3840x2160p60Hz_16_9) ==
                  FJ_HDMI_3840x2160p60Hz_16_9) {
                gVideo_fmt = D_IM_HDMI_OUT_YCC420;
              }

              if ((short_video_type_tmp & FJ_HDMI_3840x2160p50Hz_16_9) ==
                  FJ_HDMI_3840x2160p50Hz_16_9) {
                gVideo_fmt = D_IM_HDMI_OUT_YCC420;
              }

              short_video_type |= short_video_type_tmp;
            }
          }
        }
        break;
      case FJ_HDMI_EDID_DBC_TAG_VESA:
        if (gHDMI_DTD_MAN_Ctrl.data_block_flags & FJ_HDMI_EDID_TAG_FLAG_VESA) {
          if (D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(CEAE[dtd_addr]) < 31) {
            gHDMI_DTD_MAN_Ctrl.vesa_dtc_size =
                D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(CEAE[dtd_addr]);
            memcpy(gHDMI_DTD_MAN_Ctrl.vesa_dtc_dbyte, &CEAE[dtd_addr + 1],
                   gHDMI_DTD_MAN_Ctrl.vesa_dtc_size);
          }
        }
        break;
      case FJ_HDMI_EDID_DBC_TAG_RESERVED1:
      case FJ_HDMI_EDID_DBC_TAG_RESERVED2:
      default:
        break;
      }

      dtd_addr += (D_HDMI_GET_CEA_DATA_BLOCK_TAG_LENGTH(CEAE[dtd_addr]) + 1);
      if (dtd_addr >= 128) {
        break;
      }
    }

    dtd_addr = CEAE[D_HDMI_CEAE_DTD_OFFSET_ADDR];

    while (1) {
      if ((CEAE[dtd_addr] != 0) || (CEAE[dtd_addr + 1] != 0)) {
        save_addr[dtd_num] = (FJ_HDMI_EDID_DTD *)&gHDMI_DTD_Data_Block[dtd_num];
        hdmi_convert_video_timing(&CEAE[dtd_addr], save_addr[dtd_num]);
        dtd_num++; /* pgr0524 */
        dtd_addr += D_HDMI_DTD_DATA_SIZE;
        if (dtd_addr >= D_HDMI_CEAE_END_OFFSET_ADDR) {
          break;
        }
      } else {
        break;
      }
    }
    gHDMI_DTD_MAN_Ctrl.dtd_num = dtd_num;
  }

  dtd_addr = D_HDMI_EDID_DETAIL_1_ADDR;
  monitor_freq24 = 0;
  monitor_freq25 = 0;
  monitor_freq30 = 0;
  monitor_freq50 = 0;
  monitor_freq60 = 0;
  monitor_type = 0;

  while (1) {
    if (memcmp(&EDID[dtd_addr], &MONITER_RANGE_HEADER[0],
               sizeof(MONITER_RANGE_HEADER)) == 0) {
      dtd_addr += 5;
      if ((EDID[dtd_addr] - D_HDMI_FREQ24) <= 0) {
        if ((EDID[dtd_addr + 1] - D_HDMI_FREQ24) >= 0) {
          monitor_freq24 = 1;
        } else {
          BF_Debug_Print_Warning(
              ("hdmi_get_edid_proc: freq24 check abnormal.\n"));
          //					return FJ_ERR_NG;	/* pgr0524
          //*/
        }
      }
      if ((EDID[dtd_addr] - D_HDMI_FREQ25) <= 0) {
        if ((EDID[dtd_addr + 1] - D_HDMI_FREQ25) >= 0) {
          monitor_freq25 = 1;
        } else {
          BF_Debug_Print_Warning(
              ("hdmi_get_edid_proc: freq25 check abnormal.\n"));
          //					return FJ_ERR_NG;	/* pgr0524
          //*/
        }
      }
      if ((EDID[dtd_addr] - D_HDMI_FREQ30) <= 0) {
        if ((EDID[dtd_addr + 1] - D_HDMI_FREQ30) >= 0) {
          monitor_freq30 = 1;
        } else {
          BF_Debug_Print_Warning(
              ("hdmi_get_edid_proc: freq30 check abnormal.\n"));
          //					return FJ_ERR_NG;	/* pgr0524
          //*/
        }
      }
      if ((EDID[dtd_addr] - D_HDMI_FREQ50) <= 0) {
        if ((EDID[dtd_addr + 1] - D_HDMI_FREQ50) >= 0) {
          monitor_freq50 = 1;
        } else {
          BF_Debug_Print_Warning(
              ("hdmi_get_edid_proc: freq50 check abnormal.\n"));
          //					return FJ_ERR_NG;	/* pgr0524
          //*/
        }
      }
      if ((EDID[dtd_addr] - D_HDMI_FREQ60) <= 0) {
        if ((EDID[dtd_addr + 1] - D_HDMI_FREQ60) >= 0) {
          monitor_freq60 = 1;
        } else {
          BF_Debug_Print_Warning(
              ("hdmi_get_edid_proc: freq60 check abnormal.\n"));
          //					return FJ_ERR_NG;	/* pgr0524
          //*/
        }
      }
    }
    dtd_addr += D_HDMI_DTD_DATA_SIZE;
    if (dtd_addr > D_HDMI_EDID_NUM) {
      break;
    }
  }

  if ((monitor_freq24 == 0) && (monitor_freq25 == 0) && (monitor_freq30 == 0) &&
      (monitor_freq50 == 0) && (monitor_freq60 == 0)) {
    BF_Debug_Print_Warning(("hdmi_get_edid_proc: monitor freq nothing.\n"));
    //		return FJ_ERR_NG;	/* pgr0524 */
  }

  for (i = 0; i < dtd_num; i++) {
    edid_dtd = (FJ_HDMI_EDID_DTD *)gHDMI_DTD_MAN_Ctrl.dtd_addr[i];
    edid_dtd->VideoIDCode = 0;
    edid_dtd->Phy_Index = 0;
    if ((edid_dtd->H_Active == D_HDMI_720P_HSIZE) &&
        (edid_dtd->V_Active == D_HDMI_720P_VSIZE)) {
      if (edid_dtd->PixelClock == D_HDMI_CLK74) {
        if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) == D_HDMI_NON_INTERLACE) {
          if (monitor_freq60 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_720P_HBLANK_60Hz) {
              monitor_type |= FJ_HDMI_1280x720p60Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[0].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[0].Phy_Index;
            }
          }
          if (monitor_freq50 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_720P_HBLANK_50Hz) {
              monitor_type |= FJ_HDMI_1280x720p50Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[1].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[1].Phy_Index;
            }
          }
          if (monitor_freq25 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_720P_HBLANK_25Hz) {
              monitor_type |= FJ_HDMI_1280x720p25Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[10].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[10].Phy_Index;
            }
          }
          if (monitor_freq30 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_720P_HBLANK_30Hz) {
              monitor_type |= FJ_HDMI_1280x720p30Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[11].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[11].Phy_Index;
            }
          }
        }
      }
    } else if ((edid_dtd->H_Active == D_HDMI_1080I_HSIZE) &&
               (edid_dtd->V_Active == D_HDMI_1080I_VSIZE)) {
      if (edid_dtd->PixelClock == D_HDMI_CLK74) {
        if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) == D_HDMI_INTERLACE) {
          if (monitor_freq60 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_1080I_HBLANK_60Hz) {
              monitor_type |= FJ_HDMI_1920x1080i60Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[2].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[2].Phy_Index;
            }
          }
          if (monitor_freq50 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_1080I_HBLANK_50Hz) {
              monitor_type |= FJ_HDMI_1920x1080i50Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[3].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[3].Phy_Index;
            }
          }
        }
      }
    } else if ((edid_dtd->H_Active == D_HDMI_480P_HSIZE) &&
               (edid_dtd->V_Active == D_HDMI_480P_VSIZE)) {
      if (edid_dtd->PixelClock == D_HDMI_CLK27) {
        if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) == D_HDMI_NON_INTERLACE) {
          if (monitor_freq60 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_480P_HBLANK_60Hz) {
              if (((edid_dtd->H_ImageSize * 3) / 4) == edid_dtd->V_ImageSize) {
                monitor_type |= FJ_HDMI_720x480p60Hz_4_3;
                edid_dtd->VideoIDCode = gHDMI_Video_Format[4].VideoIDCode;
                edid_dtd->Phy_Index = gHDMI_Video_Format[4].Phy_Index;
              }
            }
          }
        }
      }
    } else if ((edid_dtd->H_Active == D_HDMI_576P_HSIZE) &&
               (edid_dtd->V_Active == D_HDMI_576P_VSIZE)) {
      if (edid_dtd->PixelClock == D_HDMI_CLK27) {
        if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) == D_HDMI_NON_INTERLACE) {
          if (monitor_freq50 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_576P_HBLANK_50Hz) {
              if (((edid_dtd->H_ImageSize * 3) / 4) == edid_dtd->V_ImageSize) {
                monitor_type |= FJ_HDMI_720x576p50Hz_4_3;
                edid_dtd->VideoIDCode = gHDMI_Video_Format[5].VideoIDCode;
                edid_dtd->Phy_Index = gHDMI_Video_Format[5].Phy_Index;
              }
            }
          }
        }
      }
    } else if ((edid_dtd->H_Active == D_HDMI_1080P_HSIZE) &&
               (edid_dtd->V_Active == D_HDMI_1080P_VSIZE)) {
      if (edid_dtd->PixelClock == D_HDMI_CLK74) {
        if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) == D_HDMI_NON_INTERLACE) {
          if (monitor_freq30 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_1080P_HBLANK_30Hz) {
              monitor_type |= FJ_HDMI_1920x1080p30Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[6].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[6].Phy_Index;
            }
          }
          if (monitor_freq25 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_1080P_HBLANK_25Hz) {
              monitor_type |= FJ_HDMI_1920x1080p25Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[7].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[7].Phy_Index;
            }
          }
        }
      }
      if (edid_dtd->PixelClock == D_HDMI_CLK148) {
        if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) == D_HDMI_NON_INTERLACE) {
          if (monitor_freq60 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_1080P_HBLANK_60Hz) {
              monitor_type |= FJ_HDMI_1920x1080p60Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[8].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[8].Phy_Index;
            }
          }
          if (monitor_freq50 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_1080P_HBLANK_50Hz) {
              monitor_type |= FJ_HDMI_1920x1080p50Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[9].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[9].Phy_Index;
            }
          }
        }
      }
    } else if ((edid_dtd->H_Active == D_HDMI_2160P_HSIZE) &&
               (edid_dtd->V_Active == D_HDMI_2160P_VSIZE)) {
      if (edid_dtd->PixelClock == D_HDMI_CLK297) {
        if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) == D_HDMI_NON_INTERLACE) {
          if (monitor_freq25 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_2160P_HBLANK_25Hz) {
              monitor_type |= FJ_HDMI_3840x2160p25Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[12].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[12].Phy_Index;
            }
          }
          if (monitor_freq30 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_2160P_HBLANK_30Hz) {
              monitor_type |= FJ_HDMI_3840x2160p30Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[13].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[13].Phy_Index;
            }
          }
        }
      }
      if (edid_dtd->PixelClock == D_HDMI_CLK594) {
        if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) == D_HDMI_NON_INTERLACE) {
          if (monitor_freq60 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_2160P_HBLANK_60Hz) {
              monitor_type |= FJ_HDMI_3840x2160p60Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[14].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[14].Phy_Index;
            }
          }
          if (monitor_freq50 == 1) {
            if (edid_dtd->H_Blanking == D_HDMI_2160P_HBLANK_50Hz) {
              monitor_type |= FJ_HDMI_3840x2160p50Hz_16_9;
              edid_dtd->VideoIDCode = gHDMI_Video_Format[15].VideoIDCode;
              edid_dtd->Phy_Index = gHDMI_Video_Format[15].Phy_Index;
            }
          }
        }
      }
    } else {
      edid_dtd->VideoIDCode = 0;
      edid_dtd->Phy_Index = 0;
    }
  }

  if (monitor_type != short_video_type) {
    BF_Debug_Print_Information(
        (" hdmi_get_edid_info short_video_type = 0x%08lx\n", short_video_type));
    BF_Debug_Print_Information(
        (" hdmi_get_edid_info monitor_type = 0x%08lx\n", monitor_type));
    gHDMI_Short_Video_Type_Only =
        FJ_HDMI_FORMAT_NO | (~monitor_type & short_video_type);
    monitor_type = monitor_type | short_video_type;
  } else {
    gHDMI_Short_Video_Type_Only = FJ_HDMI_FORMAT_NO;
  }

  switch (gHDMI_DTD_MAN_Ctrl.dev_inf) {
  case FJ_HDMI_DISPLAY_INTERFACE_DVI:
    gHDMI_DTD_MAN_Ctrl.sup_fmat = FJ_HDMI_DVI_MONITOR;
    gHDMI_DTD_MAN_Ctrl.scn_mode = FJ_HDMI_SCAN_UNDER;
    break;
  case FJ_HDMI_DISPLAY_INTERFACE_HDMI:
  default:
    gHDMI_DTD_MAN_Ctrl.sup_fmat = monitor_type;
    break;
  }

  BF_Debug_Print_Information(("\n"));
  BF_Debug_Print_Information(
      (" dtd_num=%d, short_video_cnt=%d\n", dtd_num, short_video_cnt));

  gHDMI_EDID_Vendor_ID.vendor_id[0] = ((EDID[8] >> 2) & 0x1F) + 0x40;
  gHDMI_EDID_Vendor_ID.vendor_id[1] =
      (((EDID[8] & 0x03) << 3) | (EDID[9] >> 5)) + 0x40;
  gHDMI_EDID_Vendor_ID.vendor_id[2] = ((EDID[9] & 0x1F)) + 0x40;
  gHDMI_EDID_Vendor_ID.product_code[0] = EDID[10];
  gHDMI_EDID_Vendor_ID.product_code[1] = EDID[11];
  gHDMI_EDID_Vendor_ID.serial_id[0] = EDID[12];
  gHDMI_EDID_Vendor_ID.serial_id[1] = EDID[13];
  gHDMI_EDID_Vendor_ID.serial_id[2] = EDID[14];
  gHDMI_EDID_Vendor_ID.serial_id[3] = EDID[15];
  gHDMI_EDID_Vendor_ID.manufacture_model[0] = EDID[16];
  gHDMI_EDID_Vendor_ID.manufacture_model[1] = EDID[17];

  return FJ_ERR_OK; /* pgr0524 */
}

static FJ_HDMI_HPD hdmi_get_edid(FJ_HDMI_DTD_MAN **dtd_addr,
                                 UCHAR cea_data_block_flags) {
  INT32 ret;

  if (dtd_addr == NULL) {
    BF_Debug_Print_Error((" hdmi_get_edid: dtd_addr is NULL.\n"));
    return FJ_HDMI_HPD_UNKNOWN;
  }

  gHDMI_DTD_MAN_Ctrl.data_block_flags = cea_data_block_flags;

  ret = hdmi_get_edid_proc();
  if (ret != 0) {
    BF_Debug_Print_Error((" hdmi_get_edid: hdmi_get_edid_proc() error.\n"));
    return FJ_HDMI_HPD_DETACH;
  }

  if (gHDMI_DTD_MAN_Ctrl.dtd_num == 0) {
    BF_Debug_Print_Error((" hdmi_get_edid: HPD unknown.\n"));
    return FJ_HDMI_HPD_UNKNOWN;
  }

  *dtd_addr = &gHDMI_DTD_MAN_Ctrl;

  return FJ_HDMI_HPD_ATTACH;
}
/**
 * @brief	Get DTD number corresponding to video_format of the argument.
 * @param [in] video_format	Video format timing
 * @param [in] dtd_no			DTD number
 * @retval FJ_ERR_OK	success.
 * @retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error
 * @retval FJ_ERR_NG		Not receiving the DTD corresponding to the size
 * of the argument
 */
static INT32 hdmi_get_dtd_no(UINT32 video_format, UINT32 *dtd_no) {
  FJ_HDMI_DTD_MAN *dtd_man;
  FJ_HDMI_EDID_DTD *edid_dtd;
  USHORT pclock;
  USHORT hsize;
  USHORT vsize;
  USHORT hblank;
  USHORT inter;
  //	USHORT himgsize;
  USHORT i;

  if (dtd_no == NULL) {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  switch (video_format) {
  case FJ_HDMI_1280x720p60Hz_16_9:
    pclock = D_HDMI_CLK74;
    hsize = D_HDMI_720P_HSIZE;
    vsize = D_HDMI_720P_VSIZE;
    hblank = D_HDMI_720P_HBLANK_60Hz;
    inter = D_HDMI_NON_INTERLACE;
    //		himgsize = D_DD_HDMI_ASPECT_16_9_HSIZE;
    break;
  case FJ_HDMI_1280x720p50Hz_16_9:
    pclock = D_HDMI_CLK74;
    hsize = D_HDMI_720P_HSIZE;
    vsize = D_HDMI_720P_VSIZE;
    hblank = D_HDMI_720P_HBLANK_50Hz;
    inter = D_HDMI_NON_INTERLACE;
    //	himgsize = D_DD_HDMI_ASPECT_16_9_HSIZE;
    break;
  case FJ_HDMI_1920x1080i60Hz_16_9:
    pclock = D_HDMI_CLK74;
    hsize = D_HDMI_1080I_HSIZE;
    vsize = D_HDMI_1080I_VSIZE;
    hblank = D_HDMI_1080I_HBLANK_60Hz;
    inter = D_HDMI_INTERLACE;
    // himgsize = D_DD_HDMI_ASPECT_16_9_HSIZE;
    break;
  case FJ_HDMI_1920x1080i50Hz_16_9:
    pclock = D_HDMI_CLK74;
    hsize = D_HDMI_1080I_HSIZE;
    vsize = D_HDMI_1080I_VSIZE;
    hblank = D_HDMI_1080I_HBLANK_50Hz;
    inter = D_HDMI_INTERLACE;
    // himgsize = D_DD_HDMI_ASPECT_16_9_HSIZE;
    break;
  case FJ_HDMI_720x480p60Hz_4_3:
    pclock = D_HDMI_CLK27;
    hsize = D_HDMI_480P_HSIZE;
    vsize = D_HDMI_480P_VSIZE;
    hblank = D_HDMI_480P_HBLANK_60Hz;
    inter = D_HDMI_NON_INTERLACE;
    // himgsize = D_DD_HDMI_ASPECT_4_3_HSIZE;
    break;
  case FJ_HDMI_720x576p50Hz_4_3:
    pclock = D_HDMI_CLK27;
    hsize = D_HDMI_576P_HSIZE;
    vsize = D_HDMI_576P_VSIZE;
    hblank = D_HDMI_576P_HBLANK_50Hz;
    inter = D_HDMI_NON_INTERLACE;
    // himgsize = D_DD_HDMI_ASPECT_4_3_HSIZE;
    break;
  case FJ_HDMI_1920x1080p30Hz_16_9:
    pclock = D_HDMI_CLK74;
    hsize = D_HDMI_1080P_HSIZE;
    vsize = D_HDMI_1080P_VSIZE;
    hblank = D_HDMI_1080P_HBLANK_30Hz;
    inter = D_HDMI_NON_INTERLACE;
    // himgsize = D_DD_HDMI_ASPECT_16_9_HSIZE;
    break;
  case FJ_HDMI_1920x1080p25Hz_16_9:
    pclock = D_HDMI_CLK74;
    hsize = D_HDMI_1080P_HSIZE;
    vsize = D_HDMI_1080P_VSIZE;
    hblank = D_HDMI_1080P_HBLANK_25Hz;
    inter = D_HDMI_NON_INTERLACE;
    // himgsize = D_DD_HDMI_ASPECT_16_9_HSIZE;
    break;
  case FJ_HDMI_1920x1080p60Hz_16_9:
    pclock = D_HDMI_CLK148;
    hsize = D_HDMI_1080P_HSIZE;
    vsize = D_HDMI_1080P_VSIZE;
    hblank = D_HDMI_1080P_HBLANK_60Hz;
    inter = D_HDMI_NON_INTERLACE;
    break;
  case FJ_HDMI_1920x1080p50Hz_16_9:
    pclock = D_HDMI_CLK148;
    hsize = D_HDMI_1080P_HSIZE;
    vsize = D_HDMI_1080P_VSIZE;
    hblank = D_HDMI_1080P_HBLANK_50Hz;
    inter = D_HDMI_NON_INTERLACE;
    break;
  case FJ_HDMI_1280x720p25Hz_16_9:
    pclock = D_HDMI_CLK74;
    hsize = D_HDMI_720P_HSIZE;
    vsize = D_HDMI_720P_VSIZE;
    hblank = D_HDMI_720P_HBLANK_25Hz;
    inter = D_HDMI_NON_INTERLACE;
    break;
  case FJ_HDMI_1280x720p30Hz_16_9:
    pclock = D_HDMI_CLK74;
    hsize = D_HDMI_720P_HSIZE;
    vsize = D_HDMI_720P_VSIZE;
    hblank = D_HDMI_720P_HBLANK_30Hz;
    inter = D_HDMI_NON_INTERLACE;
    break;
  case FJ_HDMI_3840x2160p25Hz_16_9:
    pclock = D_HDMI_CLK297;
    hsize = D_HDMI_2160P_HSIZE;
    vsize = D_HDMI_2160P_VSIZE;
    hblank = D_HDMI_2160P_HBLANK_25Hz;
    inter = D_HDMI_NON_INTERLACE;
    break;
  case FJ_HDMI_3840x2160p30Hz_16_9:
    pclock = D_HDMI_CLK297;
    hsize = D_HDMI_2160P_HSIZE;
    vsize = D_HDMI_2160P_VSIZE;
    hblank = D_HDMI_2160P_HBLANK_30Hz;
    inter = D_HDMI_NON_INTERLACE;
    break;
  case FJ_HDMI_3840x2160p50Hz_16_9:
    pclock = D_HDMI_CLK594;
    hsize = D_HDMI_2160P_HSIZE;
    vsize = D_HDMI_2160P_VSIZE;
    hblank = D_HDMI_2160P_HBLANK_50Hz;
    inter = D_HDMI_NON_INTERLACE;
    break;
  case FJ_HDMI_3840x2160p60Hz_16_9:
    pclock = D_HDMI_CLK594;
    hsize = D_HDMI_2160P_HSIZE;
    vsize = D_HDMI_2160P_VSIZE;
    hblank = D_HDMI_2160P_HBLANK_60Hz;
    inter = D_HDMI_NON_INTERLACE;
    break;
  default:
    return FJ_ERR_NG_INPUT_PARAM;
  }

  dtd_man = &gHDMI_DTD_MAN_Ctrl;
  for (i = 0; i < dtd_man->dtd_num; i++) {
    edid_dtd = (FJ_HDMI_EDID_DTD *)dtd_man->dtd_addr[i];
    if (edid_dtd->PixelClock != pclock)
      continue;
    if (edid_dtd->H_Active != hsize)
      continue;
    if (edid_dtd->V_Active != vsize)
      continue;
    if (edid_dtd->H_Blanking != hblank)
      continue;
    //		if( edid_dtd->H_ImageSize!= himgsize ) continue;
    if ((edid_dtd->Flags & D_HDMI_INTERLACE_BIT) != inter)
      continue;
    *dtd_no = i;
    return FJ_ERR_OK;
  }

  return FJ_ERR_NG;
}

/**
 * @brief	Get Video format tbl corresponding to video_format of the
 * argument.
 * @param [in] video_format	Video format timing
 * @param [out] edid_addr		Format_tbl_address.
 * @retval FJ_ERR_OK	success.
 * @retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error
 */
static INT32 hdmi_get_format_tbl_addr(UINT32 video_format,
                                      FJ_HDMI_EDID_DTD **edid_addr) {
  if (edid_addr == NULL) {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  switch (video_format) {
  case FJ_HDMI_1280x720p60Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[0];
    break;
  case FJ_HDMI_1280x720p50Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[1];
    break;
  case FJ_HDMI_1920x1080i60Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[2];
    break;
  case FJ_HDMI_1920x1080i50Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[3];
    break;
  case FJ_HDMI_720x480p60Hz_4_3:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[4];
    break;
  case FJ_HDMI_720x576p50Hz_4_3:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[5];
    break;
  case FJ_HDMI_1920x1080p30Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[6];
    break;
  case FJ_HDMI_1920x1080p25Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[7];
    break;
  case FJ_HDMI_1920x1080p60Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[8];
    break;
  case FJ_HDMI_1920x1080p50Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[9];
    break;
  case FJ_HDMI_1280x720p25Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[10];
    break;
  case FJ_HDMI_1280x720p30Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[11];
    break;
  case FJ_HDMI_3840x2160p25Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[12];
    break;
  case FJ_HDMI_3840x2160p30Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[13];
    break;
  case FJ_HDMI_3840x2160p60Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[14];
    break;
  case FJ_HDMI_3840x2160p50Hz_16_9:
    *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[15];
    break;
  default:
    return FJ_ERR_NG_INPUT_PARAM;
  }
  return FJ_ERR_OK;
}

static VOID hdmi_get_aspect_type(USHORT width, USHORT lines, BYTE *asp_type) {
  USHORT asp_ratio = 0;

  asp_ratio = (USHORT)(fabs(((double)width / lines) - ((double)16 / 9)) * 100);
  if (asp_ratio > 1) {
    *asp_type = FJ_HDMI_ASPECT_4_3;
  } else {
    *asp_type = FJ_HDMI_ASPECT_16_9;
  }
}

static VOID hdmi_set_process_status(E_HDMI_PROCESS_STATUS status) {
  gHDMI_Process_Status = status;
}

static E_HDMI_PROCESS_STATUS hdmi_check_process_status(VOID) {
  E_HDMI_PROCESS_STATUS status;
  static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
  Dd_ARM_Critical_Section_Start(critseq);
  status = gHDMI_Process_Status;
  Dd_ARM_Critical_Section_End(critseq);
  return status;
}

#ifdef CO_HDMI_CTRL_PRINT_ON
static void hdmi_get_ctrl_print(VOID) {
  T_IM_HDMI_CTRL get_ctrl;

  // HDMI Ctrl Get value
  memset(&get_ctrl, 0, sizeof(T_IM_HDMI_CTRL));
  Im_HDMI_Get_Ctrl(&get_ctrl);
  // Get Video Mapping input mode.
  BF_Debug_Print_Information(
      ("get_ctrl.video_input_mapping=0x%x\n", get_ctrl.video_input_mapping));
  BF_Debug_Print_Information(
      ("get_ctrl.vsync_in_polarity=0x%x\n", get_ctrl.vsync_in_polarity));
  BF_Debug_Print_Information(
      ("get_ctrl.hsync_in_polarity=0x%x\n", get_ctrl.hsync_in_polarity));
  BF_Debug_Print_Information(
      ("get_ctrl.de_in_polarity=0x%x\n", get_ctrl.de_in_polarity));
  BF_Debug_Print_Information(
      ("get_ctrl.r_v_blank_in_osc=0x%x\n", get_ctrl.r_v_blank_in_osc));
  BF_Debug_Print_Information(("get_ctrl.in_i_p=0x%x\n", get_ctrl.in_i_p));
  BF_Debug_Print_Information(
      ("get_ctrl.h_in_activ=0x%x\n", get_ctrl.h_in_activ));
  BF_Debug_Print_Information(
      ("get_ctrl.v_in_activ=0x%x\n", get_ctrl.v_in_activ));
  BF_Debug_Print_Information(
      ("get_ctrl.fc_invact_2d=0x%x\n", get_ctrl.fc_invact_2d));
  BF_Debug_Print_Information(("get_ctrl.fc_actspc_hdlr_cfg.byte=0x%x\n",
                              get_ctrl.fc_actspc_hdlr_cfg.byte));
  BF_Debug_Print_Information(
      ("get_ctrl.fc_inhblank=0x%x\n", get_ctrl.fc_inhblank));
  BF_Debug_Print_Information(
      ("get_ctrl.fc_invblank=0x%x\n", get_ctrl.fc_invblank));
  BF_Debug_Print_Information(
      ("get_ctrl.fc_hsyncindelay=0x%x\n", get_ctrl.fc_hsyncindelay));
  BF_Debug_Print_Information(
      ("get_ctrl.fc_vsyncindelay=0x%x\n", get_ctrl.fc_vsyncindelay));
  BF_Debug_Print_Information(
      ("get_ctrl.fc_hsyncinwidth=0x%x\n", get_ctrl.fc_hsyncinwidth));
  BF_Debug_Print_Information(
      ("get_ctrl.fc_vsyncinwidth=0x%x\n", get_ctrl.fc_vsyncinwidth));
  BF_Debug_Print_Information(("get_ctrl.DVI_modez=0x%x\n", get_ctrl.DVI_modez));
  BF_Debug_Print_Information(("get_ctrl.pll_config.vp_pr_cd.byte=0x%x\n",
                              get_ctrl.pll_config.vp_pr_cd.byte));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_num=0x%x\n",
                              get_ctrl.pll_config.phy_reg_num));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_addr[0]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_addr[0]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_addr[1]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_addr[1]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_addr[2]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_addr[2]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_addr[3]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_addr[3]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_addr[4]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_addr[4]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_addr[5]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_addr[5]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_addr[6]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_addr[6]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_addr[7]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_addr[7]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_data[0]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_data[0]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_data[1]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_data[1]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_data[2]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_data[2]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_data[3]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_data[3]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_data[4]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_data[4]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_data[5]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_data[5]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_data[6]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_data[6]));
  BF_Debug_Print_Information(("get_ctrl.pll_config.phy_reg_data[7]=0x%x\n",
                              get_ctrl.pll_config.phy_reg_data[7]));
  BF_Debug_Print_Information(("get_ctrl.audio_config.i2s_select=0x%x\n",
                              get_ctrl.audio_config.i2s_select));
  BF_Debug_Print_Information(("get_ctrl.audio_config.aud_conf1.byte=0x%x\n",
                              get_ctrl.audio_config.aud_conf1.byte));
  BF_Debug_Print_Information(("get_ctrl.audio_config.aud_conf2.byte=0x%x\n",
                              get_ctrl.audio_config.aud_conf2.byte));
  BF_Debug_Print_Information(("get_ctrl.audio_config.aud_spdif1.byte=0x%x\n",
                              get_ctrl.audio_config.aud_spdif1.byte));
  BF_Debug_Print_Information(("get_ctrl.audio_config.aud_inputclkfs=0x%x\n",
                              get_ctrl.audio_config.aud_inputclkfs));
  BF_Debug_Print_Information(
      ("get_ctrl.audio_config.aud_n1=0x%x\n", get_ctrl.audio_config.aud_n1));
  BF_Debug_Print_Information(
      ("get_ctrl.audio_config.aud_n2=0x%x\n", get_ctrl.audio_config.aud_n2));
  BF_Debug_Print_Information(("get_ctrl.audio_config.aud_n3_audn=0x%x\n",
                              get_ctrl.audio_config.aud_n3_audn));
  BF_Debug_Print_Information(("get_ctrl.audio_config.cts_manual=0x%x\n",
                              get_ctrl.audio_config.cts_manual));
  BF_Debug_Print_Information(("get_ctrl.audio_config.aud_cts1=0x%x\n",
                              get_ctrl.audio_config.aud_cts1));
  BF_Debug_Print_Information(("get_ctrl.audio_config.aud_cts2;=0x%x\n",
                              get_ctrl.audio_config.aud_cts2));
  BF_Debug_Print_Information(("get_ctrl.audio_config.aud_cts3_audcts=0x%x\n",
                              get_ctrl.audio_config.aud_cts3_audcts));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_aviconf0.byte=0x%x\n",
                              get_ctrl.info_frames.fc_aviconf0.byte));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_aviconf1.byte=0x%x\n",
                              get_ctrl.info_frames.fc_aviconf1.byte));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_aviconf2.byte=0x%x\n",
                              get_ctrl.info_frames.fc_aviconf2.byte));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_aviconf3.byt=0x%x\n",
                              get_ctrl.info_frames.fc_aviconf3.byte));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_avivid=0x%x\n",
                              get_ctrl.info_frames.fc_avivid));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_prconf.byte=0x%x\n",
                              get_ctrl.info_frames.fc_prconf.byte));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_audiconf0.byte=0x%x\n",
                              get_ctrl.info_frames.fc_audiconf0.byte));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_audiconf1.byte=0x%x\n",
                              get_ctrl.info_frames.fc_audiconf1.byte));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_audiconf2=0x%x\n",
                              get_ctrl.info_frames.fc_audiconf2));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_audiconf3.byte=0x%x\n",
                              get_ctrl.info_frames.fc_audiconf3.byte));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_vsdieeeid[0]=0x%x\n",
                              get_ctrl.info_frames.fc_vsdieeeid[0]));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_vsdieeeid[1]=0x%x\n",
                              get_ctrl.info_frames.fc_vsdieeeid[1]));
  BF_Debug_Print_Information(("get_ctrl.info_frames.fc_vsdieeeid[2]=0x%x\n",
                              get_ctrl.info_frames.fc_vsdieeeid[2]));
  BF_Debug_Print_Information(("get_ctrl.info_frames.video_mode=0x%x\n",
                              get_ctrl.info_frames.video_mode));
  BF_Debug_Print_Information(("get_ctrl.info_frames.video_format=0x%x\n",
                              get_ctrl.info_frames.video_format));
  BF_Debug_Print_Information(("get_ctrl.info_frames.scrambler_on=0x%x\n",
                              get_ctrl.info_frames.scrambler_on));
  BF_Debug_Print_Information(
      ("get_ctrl.hdcp_keepout=0x%x\n", get_ctrl.hdcp_keepout));
  BF_Debug_Print_Information(
      ("IO_CHIPTOP.CLKSEL2.bit.HIFCLK=0x%x\n", IO_CHIPTOP.CLKSEL2.bit.HIFCLK));
  BF_Debug_Print_Information(
      ("IO_CHIPTOP.PLLCNT9.bit.P05CHG=0x%x\n", IO_CHIPTOP.PLLCNT9.bit.P05CHG));
}
#endif

VOID BF_HDMI_Get_Vnder_ID(
    FJ_DISP_HDMI_DEV_VENDOR_PRODUCT_ID *vendor_product_id) {
  memcpy(vendor_product_id, &gHDMI_EDID_Vendor_ID,
         sizeof(gHDMI_EDID_Vendor_ID));
}

ULONG BF_HDMI_Get_Short_Video_Type(VOID) { return gHDMI_Short_Video_Type_Only; }

FJ_ERR_CODE BF_HDMI_Get_Format_Tbl_Top_Addr(FJ_HDMI_EDID_DTD **edid_addr) {
  if (edid_addr == NULL) {
    return FJ_ERR_NG_INPUT_PARAM;
  }

  *edid_addr = (FJ_HDMI_EDID_DTD *)&gHDMI_Video_Format[0];

  return FJ_ERR_OK;
}

VOID BF_HDMI_Set_Phy_TXCTRL(USHORT regi_data) {
  printf("Old : gHDMI_Phy_TXCTRL_Value = 0x%04X\n", gHDMI_Phy_TXCTRL_Value);
  gHDMI_Phy_TXCTRL_Value = regi_data;
  printf("New : gHDMI_Phy_TXCTRL_Value = 0x%04X\n", gHDMI_Phy_TXCTRL_Value);
  printf("Set Phy TXCTRL OK.\n");
  printf("Please call FJ_Disp_HDMI_Config_Video() to reflect setting value.\n");
}

VOID BF_HDMI_Set_Phy_VLEVCTRL(USHORT regi_data) {
  printf("Old : gHDMI_Phy_VLEVCTRL_Value = 0x%04X\n", gHDMI_Phy_VLEVCTRL_Value);
  gHDMI_Phy_VLEVCTRL_Value = regi_data;
  printf("New : gHDMI_Phy_VLEVCTRL_Value = 0x%04X\n", gHDMI_Phy_VLEVCTRL_Value);
  printf("Set Phy VLEVCTRL OK.\n");
  printf("Please call FJ_Disp_HDMI_Config_Video() to reflect setting value.\n");
}

INT32 BF_HDMI_Configure_VGA_DVI_Video_Mode(VOID) {
  INT32 ret;
  T_IM_HDMI_PLL_CONFIG pll_config;

  BF_Debug_Print_Information(("HDMI set VGA.\n"));

  memset(&pll_config, 0, sizeof(T_IM_HDMI_PLL_CONFIG));
  // Configure VGA DVI Video Mode
  pll_config.vp_pr_cd.bit.color_depth =
      0x0; // 24 bits per pixel video (8 bits per component).
  pll_config.vp_pr_cd.bit.desired_pr_factor =
      0x0; // No pixel repetition (pixel sent only once).
  pll_config.phy_reg_num = 1;
  pll_config.phy_reg_addr[0] = 0x00;
  pll_config.phy_reg_data[0] = 0x00;

  ret = Im_HDMI_Configure_VGA_DVI_Video_Mode(&pll_config);

  return ret;
}

#if 0
FJ_HDMI_HPD fj_hdmi_detect( VOID )
{
	if ( hdmi_plug_detect() == E_DD_HDMI_HPD_ATTACH ) {
		return FJ_HDMI_HPD_ATTACH;
	}
	else {
		return FJ_HDMI_HPD_DETACH;
	}
}
#else
FJ_ERR_CODE fj_hdmi_detect(VOID) {
  UCHAR mute;
  FJ_HDMI_SYS_STAT status;

  Im_HDMI_Get_Phy_Status((U_IM_HDMI_PHY_STAT0 *)&status);
  if (!status.bit.HPD) {
    return FJ_ERR_NG;
  }

  OS_User_DisableInt((OS_USER_INTID)E_DD_GIC_INTID_HDMI_MASTER_INT);

  Im_HDMI_Get_Int_Mute(E_IM_HDMI_INT_REG_PHY_STAT0, &mute);
  // bit[0]:HDP Unmute.
  mute &= 0xFE;
  Im_HDMI_Set_Int_Mute(E_IM_HDMI_INT_REG_PHY_STAT0, mute);

  OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_HDMI_MASTER_INT,
                    (OS_USER_INTLV)D_DD_GIC_PRI14);

  return FJ_ERR_OK;
}
#endif

VOID fj_hdmi_init(HDMI_CALLBACK interrupt_callback) {
  BYTE sr;

  // CRRRSO register - Clear Software Reset bit
  OS_User_DisableInt((OS_USER_INTID)E_DD_GIC_INTID_HDMI_MASTER_INT);

  // Software reset status check.
  Im_DISP_Get_SR_IP(&sr);
  if ((sr & D_IM_DISP_IP_HDMI) == D_IM_DISP_IP_HDMI) {
    // Software reset cancel.
    Im_DISP_Set_SR_IP(D_IM_DISP_IP_HDMI, D_IM_DISP_SR_CANCEL);
  }

  // Start clock
  Dd_Top_Set_CLKSTOP3_HIFCK(0);

  if (gHDMI_DTD_MAN_Ctrl.dtd_num != 0) {
    for (UINT32 dtd_num = 0; dtd_num < gHDMI_DTD_MAN_Ctrl.dtd_num; dtd_num++) {
      free((VOID *)gHDMI_DTD_MAN_Ctrl.dtd_addr[dtd_num]);
    }
    free((VOID *)gHDMI_DTD_MAN_Ctrl.dtd_addr);
  }

  gHDMI_DTD_MAN_Ctrl.dtd_num = 0;
  gHDMI_DTD_MAN_Ctrl.dtd_addr = 0;

  gHDMI_INTERRUPT_Callback = interrupt_callback;

  Im_HDMI_Set_Int_Callback((VP_HDMI_CALLBACK)hdmi_interrupt_callback);

  // ICDISER4 Register(GIC) - enable corresponding interrupt
  OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_HDMI_MASTER_INT,
                    (OS_USER_INTLV)D_DD_GIC_PRI14);

#if 0
	// init HDMI Transmitter macro
	Im_HDMI_Init(&pll_config);
#else
  // init HDMI Transmitter macro
  Im_HDMI_Init();
#endif

  hdmi_set_process_status(E_HDMI_PROCESS_STATUS_INIT);
}

#if 0
VOID fj_hdmi_uninit( VOID )
{	
	T_DD_HDMI_CTRL_COMMON ctrl;
		
	//ICDISER4 Register(GIC) - disable corresponding interrupt
	OS_User_DisableInt((OS_USER_INTID)E_DD_GIC_INTID_HDMI_INT);

	// SYS_CTRL1 register, set PD
	Dd_HDMI_Get_Ctrl_Common( &ctrl );
	ctrl.sys_ctrl1.bit.PD = D_DD_HDMI_PD_NORMAL;	
	Dd_HDMI_Ctrl_Common( &ctrl );
	
	//Stop clock
	Dd_Top_Set_CLKSTOP_HDCK(1);
	Dd_Top_Set_CLKSTOP_OCLK(1);
	Dd_Top_Set_CLKSTOP_CECCLK(1);

	//CRRRSO register - Reset
	Dd_Top_Set_CRRRSO_HDSR_RES();
}
#endif

VOID fj_hdmi_start_sync(VOID) {
  // ICDISER4 Register(GIC) - enable corresponding interrupt

  // Start Audio (Mute cancel)
  fj_hdmi_audio_mute(0);

  // HDMI PHY Power On.
  Im_HDMI_Power_On(D_IM_HDMI_ENABLE_ON);
}

VOID fj_hdmi_stop_sync(VOID) {
  // Stop Audio
  fj_hdmi_audio_mute(1);

  // HDMI PHY Power Off.
  Im_HDMI_Power_On(D_IM_HDMI_ENABLE_OFF);
}

FJ_ERR_CODE fj_hdmi_connect_hdmi(UINT32 video_format) {
  FJ_HDMI_HPD hpd;
  FJ_HDMI_DTD_MAN *dtd_man;
  FJ_HDMI_EDID_DTD *edid_addr;
  UINT32 dtd_no;
  BYTE avi_aspect_type = FJ_HDMI_ASPECT_16_9;
  T_IM_HDMI_VIDEO_PACKETIZER video_packetizer;
  T_IM_HDMI_CTRL ctrl;
  UCHAR mc_flowctrl = 0x00;
  INT32 ret;
  BOOL scramble;

  E_HDMI_PROCESS_STATUS status = hdmi_check_process_status();
  if (E_HDMI_PROCESS_STATUS_DISCONNECT != status &&
      E_HDMI_PROCESS_STATUS_INIT != status) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_connect_hdmi: Iterate connect, current status is: %d\n",
         status));
    return FJ_ERR_NG;
  }

  BF_HDMI_Configure_VGA_DVI_Video_Mode();

  if (gHDMI_EDID_Is_Update == FALSE) { // haven't got edid
    hpd = hdmi_get_edid(&dtd_man, FJ_HDMI_EDID_TAG_FLAG_ALL);
    if (hpd != FJ_HDMI_HPD_ATTACH) {
      BF_Debug_Print_Error(
          ("fj_hdmi_connect_hdmi: HPD not attach.(hpd = %d).\n", hpd));
      return FJ_ERR_NG;
    }

    gHDMI_DTD_MAN = dtd_man;
  } else { // have got edid
    dtd_man = gHDMI_DTD_MAN;
    gHDMI_EDID_Is_Update = FALSE;
  }

  // Get DTD information
  if (hdmi_get_dtd_no(video_format, &dtd_no) == FJ_ERR_OK) {
    edid_addr = (FJ_HDMI_EDID_DTD *)gHDMI_DTD_MAN->dtd_addr[dtd_no];
  } else {
    if (hdmi_get_format_tbl_addr(
            video_format, (FJ_HDMI_EDID_DTD **)&edid_addr) != FJ_ERR_OK) {
      BF_Debug_Print_Warning(
          ("hdmi_get_video_format_tbl_addr error L=%d\n", __LINE__));
      return FJ_ERR_NG;
    }
  }

  // HDMI Link setting
  hdmi_get_aspect_type(edid_addr->H_ImageSize, edid_addr->V_ImageSize,
                       &avi_aspect_type);

  memset(&ctrl, 0, sizeof(T_IM_HDMI_CTRL));

  // Video Packetizer Register Setting
  if (((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p_3840)) ||
      ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p50_3840)) ||
      (dtd_man->dev_inf == FJ_HDMI_DISPLAY_INTERFACE_DVI)) {
    video_packetizer.vp_pr_cd.byte = 0x00;
    video_packetizer.vp_remap = 0x00;
    video_packetizer.vp_conf.byte = 0x47;
  }
#ifdef CO_HDMI_COMPLIANCE_TEST
  else if (gRS_System_Disp_Target_RGB) {
    video_packetizer.vp_pr_cd.byte = 0x00;
    video_packetizer.vp_remap = 0x00;
    video_packetizer.vp_conf.byte = 0x47;
  }
#endif
  else {
    video_packetizer.vp_pr_cd.byte = 0x00;
    video_packetizer.vp_remap = 0x00;
    video_packetizer.vp_conf.byte = 0x0d;
  }
  Im_HDMI_Set_Video_Packetizer(&video_packetizer);
  // Main Controller Registers Setting
  Im_HDMI_Set_Main_Controller(mc_flowctrl);

  // Video Sampler Registers  Setting
  if (((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p_3840)) ||
      ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p50_3840))) {
    ctrl.video_input_mapping = E_IM_HDMI_VIDEO_MAPPING_YCC444_420_8BIT;
  } else if (gHDMI_DTD_MAN_Ctrl.dev_inf == FJ_HDMI_DISPLAY_INTERFACE_DVI) {
    ctrl.video_input_mapping = E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT;
  }
#ifdef CO_HDMI_COMPLIANCE_TEST
  else if (gRS_System_Disp_Target_RGB) {
    ctrl.video_input_mapping = E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT;
  }
#endif
  else {
    ctrl.video_input_mapping = E_IM_HDMI_VIDEO_MAPPING_YCC422_8BIT;
  }
  // Frame Composer Registers  Setting
  if (((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p_3840)) ||
      ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p50_3840))) {
    ctrl.h_in_activ = edid_addr->H_Active / 2;
    ctrl.v_in_activ = edid_addr->V_Active;
    ctrl.fc_inhblank = edid_addr->H_Blanking / 2;
    ctrl.fc_invblank = edid_addr->V_Blanking;
    ctrl.fc_hsyncindelay = edid_addr->H_SyncOffset / 2;
    ctrl.fc_vsyncindelay = edid_addr->V_SyncOffset;
    ctrl.fc_hsyncinwidth = edid_addr->H_SyncPulseWidth / 2;
    ctrl.fc_vsyncinwidth = edid_addr->V_SyncPulseWidth;
    ctrl.fc_invact_2d = edid_addr->V_Active;
  } else {
    ctrl.h_in_activ = edid_addr->H_Active;
    ctrl.v_in_activ = edid_addr->V_Active;
    ctrl.fc_inhblank = edid_addr->H_Blanking;
    ctrl.fc_invblank = edid_addr->V_Blanking;
    ctrl.fc_hsyncindelay = edid_addr->H_SyncOffset;
    ctrl.fc_vsyncindelay = edid_addr->V_SyncOffset;
    ctrl.fc_hsyncinwidth = edid_addr->H_SyncPulseWidth;
    ctrl.fc_vsyncinwidth = edid_addr->V_SyncPulseWidth;
    ctrl.fc_invact_2d = edid_addr->V_Active;
  }
  // check flag.
  // fc_invidconf
  if ((edid_addr->Flags & FJ_HDMI_INTERLACE) == FJ_HDMI_INTERLACE) {
    // Interlace
    ctrl.in_i_p = D_IM_HDMI_IN_I_P_INTERLACED;
    ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_tgl = D_IM_HDMI_ENABLE_OFF;
    ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_en = D_IM_HDMI_ENABLE_OFF;
    ctrl.r_v_blank_in_osc = D_IM_HDMI_ACTIVE_HIGH;
  } else {
    ctrl.in_i_p = D_IM_HDMI_IN_I_P_PROGRESSIVE;
    ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_tgl = D_IM_HDMI_ENABLE_OFF;
    ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_en = D_IM_HDMI_ENABLE_OFF;
    ctrl.r_v_blank_in_osc = D_IM_HDMI_ACTIVE_LOW;
  }
  // Positive HSYNC
  if ((edid_addr->Flags & FJ_HDMI_HSYNC_POS) == FJ_HDMI_HSYNC_POS) {
    ctrl.hsync_in_polarity = D_IM_HDMI_ACTIVE_HIGH;
  } else {
    ctrl.hsync_in_polarity = D_IM_HDMI_ACTIVE_LOW;
  }
  // Positive VSYNC
  if ((edid_addr->Flags & FJ_HDMI_VSYNC_POS) == FJ_HDMI_VSYNC_POS) {
    ctrl.vsync_in_polarity = D_IM_HDMI_ACTIVE_HIGH;
  } else {
    ctrl.vsync_in_polarity = D_IM_HDMI_ACTIVE_LOW;
  }
  ctrl.de_in_polarity = D_IM_HDMI_ACTIVE_HIGH;

  // Device interface
  if (dtd_man->dev_inf == FJ_HDMI_DISPLAY_INTERFACE_HDMI) {
    // HDMI mode.
    ctrl.DVI_modez = D_IM_HDMI_DVI_MODEZ_HDMI;
  } else {
    // DVI mode.
    ctrl.DVI_modez = D_IM_HDMI_DVI_MODEZ_DVI;
  }

  if (((gVideo_fmt != D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p_3840)) ||
      ((gVideo_fmt != D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p50_3840))) {
    ctrl.hdcp_keepout = 1;
  } else {
    ctrl.hdcp_keepout = 0;
  }

  // Set Phy Index
  if ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
      (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p_3840)) {
    gPhy_Index =
        gHDMI_Video_Format[13]
            .Phy_Index; // 3840x2160P@30Hz   16:9  FJ_DISP_VIDEO_ID_2160p30_3840
  } else if ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
             (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p50_3840)) {
    gPhy_Index =
        gHDMI_Video_Format[12]
            .Phy_Index; // 3840x2160P@25Hz   16:9  FJ_DISP_VIDEO_ID_2160p25_3840
  } else {
    gPhy_Index = edid_addr->Phy_Index;
  }

  // Phy PLL configuration.
  ctrl.pll_config.vp_pr_cd.bit.desired_pr_factor = 0;
  ctrl.pll_config.vp_pr_cd.bit.color_depth = 0;
  ctrl.pll_config.phy_reg_num = 6;
  ctrl.pll_config.phy_reg_addr[0] =
      gHDMI_PHY_Config[gPhy_Index].opmode_pllcfg_adr;
  ctrl.pll_config.phy_reg_data[0] =
      gHDMI_PHY_Config[gPhy_Index].opmode_pllcfg_val;
  ctrl.pll_config.phy_reg_addr[1] =
      gHDMI_PHY_Config[gPhy_Index].pllcurrctrl_adr;
  ctrl.pll_config.phy_reg_data[1] =
      gHDMI_PHY_Config[gPhy_Index].pllcurrctrl_val;
  ctrl.pll_config.phy_reg_addr[2] = gHDMI_PHY_Config[gPhy_Index].pllgmpctrl_adr;
  ctrl.pll_config.phy_reg_data[2] = gHDMI_PHY_Config[gPhy_Index].pllgmpctrl_val;
  ctrl.pll_config.phy_reg_addr[3] = gHDMI_PHY_Config[gPhy_Index].vlevctrl_adr;
  ctrl.pll_config.phy_reg_data[3] = gHDMI_PHY_Config[gPhy_Index].vlevctrl_val;
  ctrl.pll_config.phy_reg_addr[4] =
      gHDMI_PHY_Config[gPhy_Index].cksymtxctrl_adr;
  ctrl.pll_config.phy_reg_data[4] =
      gHDMI_PHY_Config[gPhy_Index].cksymtxctrl_val;
  ctrl.pll_config.phy_reg_addr[5] = gHDMI_PHY_Config[gPhy_Index].txterm_adr;
  ctrl.pll_config.phy_reg_data[5] = gHDMI_PHY_Config[gPhy_Index].txterm_val;

  if (gHDMI_Phy_VLEVCTRL_Value != 0xFFFF) {
    ctrl.pll_config.phy_reg_data[3] = gHDMI_Phy_VLEVCTRL_Value;
  }

  if (gHDMI_Phy_TXCTRL_Value != 0xFFFF) {
    ctrl.pll_config.phy_reg_data[4] = gHDMI_Phy_TXCTRL_Value;
  }

  // Infoframes configuration
  // Appendix F - F.1 Standard Color 8-Bit, YCC422
  if ((edid_addr->VideoIDCode != FJ_DISP_VIDEO_ID_2160p30_3840) &&
      (edid_addr->VideoIDCode != FJ_DISP_VIDEO_ID_2160p25_3840)) {
    if (((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
         (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p_3840)) ||
        ((gVideo_fmt == D_IM_HDMI_OUT_YCC420) &&
         (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p50_3840))) {
      ctrl.info_frames.fc_aviconf0.byte = 0x6F;
    } else {
      ctrl.info_frames.fc_aviconf0.byte = 0x6D;
    }
    ctrl.info_frames.fc_aviconf1.byte = 0x9B; // ITU-709
    ctrl.info_frames.fc_aviconf2.byte = 0x04;
    ctrl.info_frames.fc_aviconf3.byte = 0x00;
    ctrl.info_frames.fc_avivid = edid_addr->VideoIDCode;
    ctrl.info_frames.fc_prconf.byte = 0x10;
  } else {
    ctrl.info_frames.fc_aviconf0.byte = 0x6D;
    if (ctrl.v_in_activ < 720) {
      ctrl.info_frames.fc_aviconf1.byte = 0x5B; // ITU-601
    } else {
      ctrl.info_frames.fc_aviconf1.byte = 0x9B; // ITU-709
    }
    ctrl.info_frames.fc_aviconf2.byte = 0x04;
    ctrl.info_frames.fc_aviconf3.byte = 0x00;
    ctrl.info_frames.fc_avivid = 0x00;
    ctrl.info_frames.fc_prconf.byte = 0x10;
  }

#ifdef CO_HDMI_COMPLIANCE_TEST
  if (gRS_System_Disp_Target_RGB) {
    ctrl.info_frames.fc_aviconf0.bit.rgc_ycc_indication = 0x00;
  }
#endif

  // Aspect ratio.
  ctrl.info_frames.fc_aviconf1.bit.picture_aspect_ratio = avi_aspect_type;

  // FrameComposer Registers
  // IEEE ID (from HDMI Specification1.4)
  ctrl.info_frames.fc_vsdieeeid[0] = 0x00;
  ctrl.info_frames.fc_vsdieeeid[1] = 0x0C;
  ctrl.info_frames.fc_vsdieeeid[2] = 0x03;

  // Video Format & VIC (from HDMI Specification1.4)
  if (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p30_3840) {
    ctrl.info_frames.video_mode = D_IM_HDMI_VIDEO_MODE_HDMI;
    ctrl.info_frames.video_format = 1;
    ctrl.info_frames.scrambler_on = 0;
  } else if (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p25_3840) {
    ctrl.info_frames.video_mode = D_IM_HDMI_VIDEO_MODE_HDMI;
    ctrl.info_frames.video_format = 2;
    ctrl.info_frames.scrambler_on = 0;
  } else if (((gVideo_fmt != D_IM_HDMI_OUT_YCC420) &&
              (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p_3840)) ||
             ((gVideo_fmt != D_IM_HDMI_OUT_YCC420) &&
              (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p50_3840))) {
    ctrl.info_frames.video_mode = 0;
    ctrl.info_frames.video_format = 0;
    ctrl.info_frames.scrambler_on = 1;
  } else {
    ctrl.info_frames.video_mode = 0;
    ctrl.info_frames.video_format = 0;
    ctrl.info_frames.scrambler_on = 0;
  }

  // 3D Setting paramter(No use)
  ctrl.info_frames.meta_present = 0;
  ctrl.info_frames.additional_video_format = 0;
  ctrl.info_frames.meta_type = 0;
  ctrl.info_frames.meta_length = 0;
  for (int i = 0; i < 8; i++) {
    ctrl.info_frames.meta_data[i] = 0;
  }

  Im_HDMI_Ctrl(&ctrl);

  if (((gVideo_fmt != D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p_3840)) ||
      ((gVideo_fmt != D_IM_HDMI_OUT_YCC420) &&
       (edid_addr->VideoIDCode == FJ_DISP_VIDEO_ID_2160p50_3840))) {
    scramble = TRUE;
  } else {
    scramble = FALSE;
  }
  ret = Im_HDMI_Set_TMDS_Scramble(scramble);
  if (ret != D_DDIM_OK) {
    BF_Debug_Print_Error(
        ("fj_hdmi_connect_hdmi: Im_HDMI_Set_TMDS_Scramble() error. ret = %x.\n",
         ret));
    return FJ_ERR_NG;
  }

  // HDMI PHY Power On.
  Im_HDMI_Power_On(D_IM_HDMI_ENABLE_ON);

  hdmi_set_process_status(E_HDMI_PROCESS_STATUS_CONNECTED);

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_disconnect_hdmi(VOID) {
  if (E_HDMI_PROCESS_STATUS_DISCONNECT == hdmi_check_process_status()) {
    return FJ_ERR_NG;
  }

  // HDMI PHY Power Off.
  Im_HDMI_Power_On(D_IM_HDMI_ENABLE_OFF);

  hdmi_set_process_status(E_HDMI_PROCESS_STATUS_DISCONNECT);

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_set_outformat(UCHAR format) {
  switch (format) {
  case FJ_HDMI_FORCE_CLEAR:
  case FJ_HDMI_FORCE_YCC422:
  case FJ_HDMI_FORCE_YCC444:
    break;
  default:
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_outformat Input_Param_Err L=%d, format=%d\n", __LINE__,
         format));
    return FJ_ERR_NG_INPUT_PARAM;
  }
  gHDMI_DTD_MAN_Ctrl.out_fmat = format;

  return FJ_ERR_OK;
}

FJ_HDMI_HPD fj_hdmi_get_edid(FJ_HDMI_DTD_MAN **dtd_addr,
                             UCHAR edid_data_block_tag_flags) {
  FJ_HDMI_HPD hpd;

  hpd = hdmi_get_edid((FJ_HDMI_DTD_MAN **)dtd_addr, edid_data_block_tag_flags);
  if (hpd == FJ_HDMI_HPD_ATTACH) {
    gHDMI_DTD_MAN = (FJ_HDMI_DTD_MAN *)*dtd_addr;
    gHDMI_EDID_Is_Update = TRUE;
  }

  return hpd;
}

FJ_ERR_CODE fj_hdmi_get_dtd_no(UINT32 video_format, UINT32 *dtd_no) {
  INT32 ret;

  ret = hdmi_get_dtd_no(video_format, dtd_no);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_get_dtd_no Input_Param_Err L=%d, ret=0x%x\n", __LINE__, ret));
    return FJ_ERR_NG_INPUT_PARAM;
  }
  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_get_format_tbl_addr(UINT32 video_format,
                                        FJ_HDMI_EDID_DTD **edid_addr) {
  INT32 ret;

  ret = hdmi_get_format_tbl_addr(video_format, (FJ_HDMI_EDID_DTD **)edid_addr);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_get_format_tbl_addr Input_Param_Err L=%d, ret=0x%x\n",
         __LINE__, ret));
    return FJ_ERR_NG_INPUT_PARAM;
  }
  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_get_system_status(FJ_HDMI_SYS_STAT *status) {
  INT32 ret;

  ret = Im_HDMI_Get_Phy_Status((U_IM_HDMI_PHY_STAT0 *)status);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_get_system_status Input_Param_Err L=%d, ret=0x%x\n", __LINE__,
         ret));
    return FJ_ERR_NG_INPUT_PARAM;
  }
  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_set_ctrl_common(FJ_HDMI_CTRL_COMMON const *const ctrl) {
  INT32 ret = D_DDIM_OK;
  T_IM_HDMI_CTRL value;

  // Parameter check
  if (ctrl == NULL) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_ctrl_common Input_Param_Err L=%d\n", __LINE__));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // paramter data set.
  memcpy(&value, ctrl, sizeof(T_IM_HDMI_CTRL));

  // Set Control Info.
  ret = Im_HDMI_Ctrl(&value);
  if (ret == D_IM_HDMI_INPUT_PARAM_ERROR) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_ctrl_common Input_Param_Err L=%d, ret=0x%x\n", __LINE__,
         ret));
    return FJ_ERR_NG_INPUT_PARAM;
  } else if (ret != D_DDIM_OK) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_ctrl_common NG L=%d, ret=0x%x\n", __LINE__, ret));
    return FJ_ERR_NG;
  }

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_get_ctrl_common(FJ_HDMI_CTRL_COMMON *const ctrl) {
  INT32 ret = D_DDIM_OK;
  T_IM_HDMI_CTRL value;

  // Parameter check
  if (ctrl == NULL) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_get_ctrl_common Input_Param_Err L=%d\n", __LINE__));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Get Control Info.
  ret = Im_HDMI_Get_Ctrl(&value);
  if (ret != D_DDIM_OK) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_get_ctrl_common Input_Param_Err L=%d, ret=0x%x\n", __LINE__,
         ret));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // paramter data set.
  memcpy(ctrl, &value, sizeof(T_IM_HDMI_CTRL));

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_set_ctrl_video(FJ_HDMI_CTRL_VIDEO const *const ctrl) {
  U_IM_HDMI_TX_INVID0 tx_invid0;
  T_IM_HDMI_VIDEO_PACKETIZER video_packetizer;
  T_IM_HDMI_FRAME_COMPOSER frame_composer;

  // Parameter check
  if (ctrl == NULL) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_ctrl_video Input_Param_Err L=%d\n", __LINE__));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Video Sampler.
  tx_invid0.byte = ctrl->tx_invid0.byte;
  Im_HDMI_Set_Video_Sampler(tx_invid0);

  // Video Packetizer.
  video_packetizer.vp_pr_cd.byte = ctrl->vp_pr_cd.byte;
  video_packetizer.vp_remap = ctrl->vp_remap;
  video_packetizer.vp_conf.byte = ctrl->vp_conf.byte;
  Im_HDMI_Set_Video_Packetizer(&video_packetizer);

  // Frame Composer.
  frame_composer.fc_invidconf.byte = ctrl->fc_invidconf.byte;
  frame_composer.fc_inhactiv = ctrl->fc_inhactiv;
  frame_composer.fc_inhblank = ctrl->fc_inhblank;
  frame_composer.fc_invactiv = ctrl->fc_invactiv;
  frame_composer.fc_invblank = ctrl->fc_invblank;
  frame_composer.fc_hsyncindelay = ctrl->fc_hsyncindelay;
  frame_composer.fc_hsyncinwidth = ctrl->fc_hsyncinwidth;
  frame_composer.fc_vsyncindelay = ctrl->fc_vsyncindelay;
  frame_composer.fc_vsyncinwidth = ctrl->fc_vsyncinwidth;
  frame_composer.fc_aviconf3.byte = ctrl->fc_aviconf3.byte;
  frame_composer.fc_aviconf0.byte = ctrl->fc_aviconf0.byte;
  frame_composer.fc_aviconf1.byte = ctrl->fc_aviconf1.byte;
  frame_composer.fc_aviconf2.byte = ctrl->fc_aviconf2.byte;
  frame_composer.fc_avivid = ctrl->fc_avivid;
  frame_composer.fc_prconf.byte = ctrl->fc_prconf.byte;
  Im_HDMI_Set_Frame_Composer(&frame_composer);

  // Main Controller.
  Im_HDMI_Set_Main_Controller(ctrl->mc_flowctrl);

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_get_ctrl_video(FJ_HDMI_CTRL_VIDEO *const ctrl) {
  U_IM_HDMI_TX_INVID0 tx_invid0;
  T_IM_HDMI_VIDEO_PACKETIZER video_packetizer;
  T_IM_HDMI_FRAME_COMPOSER frame_composer;

  // Parameter check
  if (ctrl == NULL) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_get_ctrl_video Input_Param_Err L=%d\n", __LINE__));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Video Sampler.
  Im_HDMI_Get_Video_Sampler(&tx_invid0);
  ctrl->tx_invid0.byte = tx_invid0.byte;

  // Video Packetizer.
  Im_HDMI_Get_Video_Packetizer(&video_packetizer);
  ctrl->vp_pr_cd.byte = video_packetizer.vp_pr_cd.byte;
  ctrl->vp_remap = video_packetizer.vp_remap;
  ctrl->vp_conf.byte = video_packetizer.vp_conf.byte;

  // Frame Composer.
  Im_HDMI_Get_Frame_Composer(&frame_composer);
  ctrl->fc_invidconf.byte = frame_composer.fc_invidconf.byte;
  ctrl->fc_inhactiv = frame_composer.fc_inhactiv;
  ctrl->fc_inhblank = frame_composer.fc_inhblank;
  ctrl->fc_invactiv = frame_composer.fc_invactiv;
  ctrl->fc_invblank = frame_composer.fc_invblank;
  ctrl->fc_hsyncindelay = frame_composer.fc_hsyncindelay;
  ctrl->fc_hsyncinwidth = frame_composer.fc_hsyncinwidth;
  ctrl->fc_vsyncindelay = frame_composer.fc_vsyncindelay;
  ctrl->fc_vsyncinwidth = frame_composer.fc_vsyncinwidth;
  ctrl->fc_aviconf3.byte = frame_composer.fc_aviconf3.byte;
  ctrl->fc_aviconf0.byte = frame_composer.fc_aviconf0.byte;
  ctrl->fc_aviconf1.byte = frame_composer.fc_aviconf1.byte;
  ctrl->fc_aviconf2.byte = frame_composer.fc_aviconf2.byte;
  ctrl->fc_avivid = frame_composer.fc_avivid;
  ctrl->fc_prconf.byte = frame_composer.fc_prconf.byte;

  // Main Controller.
  Im_HDMI_Get_Main_Controller(&(ctrl->mc_flowctrl));

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_set_ctrl_audio(FJ_HDMI_CTRL_AUDIO const *const ctrl) {
  INT32 ret = D_DDIM_OK;
  T_IM_HDMI_AUDIO audio;

  // Parameter check
  if (ctrl == NULL) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_ctrl_audio Input_Param_Err L=%d\n", __LINE__));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // paramter data set.
  memcpy(&audio, ctrl, sizeof(T_IM_HDMI_AUDIO));

  // Set Audio Info.
  ret = Im_HDMI_Set_Audio(&audio);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_ctrl_audio Input_Param_Err L=%d, ret=0x%x\n", __LINE__,
         ret));
    return FJ_ERR_NG_INPUT_PARAM;
  }
  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_get_ctrl_audio(FJ_HDMI_CTRL_AUDIO *const ctrl) {
  INT32 ret = D_DDIM_OK;
  T_IM_HDMI_AUDIO audio;

  // Parameter check
  if (ctrl == NULL) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_ctrl_audio Input_Param_Err L=%d\n", __LINE__));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // Set Audio Info.
  ret = Im_HDMI_Get_Audio(&audio);
  if (ret != FJ_ERR_OK) {
    BF_Debug_Print_Warning(
        ("fj_hdmi_set_ctrl_audio Input_Param_Err L=%d, ret=0x%x\n", __LINE__,
         ret));
    return FJ_ERR_NG_INPUT_PARAM;
  }

  // paramter data set.
  memcpy(ctrl, &audio, sizeof(FJ_HDMI_CTRL_AUDIO));

  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_set_ctrl_i2s(FJ_HDMI_CTRL_AUDIO_I2S const *const ctrl) {
#if 0  // Unused functions
	INT32 ret;
	T_DD_HDMI_CTRL_AUDIO_I2S value;

	value.i2s_chst1 = ctrl->i2s_chst1;
	value.i2s_chst2 = ctrl->i2s_chst2;
	value.i2s_chst3.byte = ctrl->i2s_chst3.byte;
	value.i2s_chst4.byte = ctrl->i2s_chst4.byte;
	value.i2s_chst5.byte = ctrl->i2s_chst5.byte;
	value.i2s_control.byte = ctrl->i2s_control.byte;
	value.i2s_in_len.byte = ctrl->i2s_in_len.byte;
	value.i2s_in_map.byte = ctrl->i2s_in_map.byte;
	value.i2s_swap_i2s.byte = ctrl->i2s_swap_i2s.byte;
	
	ret = Dd_HDMI_Ctrl_I2s( &value);
	if (ret != FJ_ERR_OK) {	
		BF_Debug_Print_Warning(("fj_hdmi_set_ctrl_i2s Input_Param_Err L=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}
#endif // Unused functions
  return FJ_ERR_OK;
}

FJ_ERR_CODE fj_hdmi_get_ctrl_i2s(FJ_HDMI_CTRL_AUDIO_I2S *const ctrl) {
#if 0  // Unused functions
	INT32 ret;
	T_DD_HDMI_CTRL_AUDIO_I2S value;

	ret = Dd_HDMI_Get_Ctrl_I2s( &value );
	if (ret != FJ_ERR_OK) {	
		BF_Debug_Print_Warning(("fj_hdmi_get_ctrl_i2s Input_Param_Err L=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ctrl->i2s_chst1 = value.i2s_chst1;
	ctrl->i2s_chst2 = value.i2s_chst2;
	ctrl->i2s_chst3.byte = value.i2s_chst3.byte;
	ctrl->i2s_chst4.byte = value.i2s_chst4.byte;
	ctrl->i2s_chst5.byte = value.i2s_chst5.byte;
	ctrl->i2s_control.byte = value.i2s_control.byte;
	ctrl->i2s_in_len.byte = value.i2s_in_len.byte;
	ctrl->i2s_in_map.byte = value.i2s_in_map.byte;
	ctrl->i2s_swap_i2s.byte= value.i2s_swap_i2s.byte;
#endif // Unused functions

  return FJ_ERR_OK;
}

#if 0
FJ_ERR_CODE fj_hdmi_avi_ctrl( USHORT clock, USHORT hsize, USHORT vsize, USHORT hblank, USHORT himgsize, UCHAR form )
{
	INT32 ret;

	ret = Dd_HDMI_Set_Avi( clock, hsize, vsize, hblank, himgsize, form );
	if (ret != FJ_ERR_OK) {	
		BF_Debug_Print_Warning(("fj_hdmi_avi_ctrl() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
#endif

FJ_ERR_CODE fj_hdmi_audio_ctrl(unsigned long samp_freq) {
  T_IM_HDMI_AUDIO audio;
  UCHAR p05chg;

  memset(&audio, 0, sizeof(T_IM_HDMI_AUDIO));

  // Audio configuration
  audio.aud_conf0.byte = 0x21; // i2s_select=1(I2S), I2S_in_en=1(1-2 ch enable).
  audio.aud_conf1.bit.I2S_width = 0x10; // 16 bit data samples at input
  audio.aud_conf1.bit.I2S_mode = E_IM_HDMI_I2S_MODE_LEFT_JUSTIFIED;
  audio.aud_conf2.bit.HBR = D_IM_HDMI_ENABLE_OFF;
  audio.aud_conf2.bit.NLPCM = D_IM_HDMI_ENABLE_OFF;
  audio.aud_spdif1.byte = 0; // No use.
  audio.fc_audsconf.bit.aud_packet_layout = 0;
  audio.fc_audsconf.bit.aud_packet_sampflt = 0;

  // AudioPacketizer Registers
  audio.aud_inputclkfs = E_IM_HDMI_AUD_IFSFACTOR_64XFS; // 64xFS
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
  audio.oiec_wordlength = 0xA; // Word length configuration
                               // (MAX=20bit/LEN=20bit)

  // Sampling frequency and N value setting
  p05chg = Dd_Top_Get_PLLCNT9_P05CHG();
  switch (samp_freq) {
  case FJ_DISP_HDMI_AUDIO_32000HZ:
    audio.sampfreq = 3; // Sampling frequency 32kHz(3)
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
    audio.sampfreq = 0; // Sampling frequency 44.1kHz(0)
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
    audio.sampfreq = 2; // Sampling frequency 48kHz(2)
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
    audio.sampfreq = 10; // Sampling frequency 96kHz(10)
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

  audio.cts_manual = E_IM_HDMI_CTS_MANUAL_AUTO; // Automatic(0)

  // Device interface
  if (gHDMI_DTD_MAN->dev_inf == FJ_HDMI_DISPLAY_INTERFACE_HDMI) {
    Im_HDMI_Set_Audio(&audio);
  }

  // HDMI PHY Power On.
  Im_HDMI_Power_On(D_IM_HDMI_ENABLE_ON);

  gHDMI_Audio_Frequency = samp_freq;

  return FJ_ERR_OK;
}

VOID fj_hdmi_audio_start(void) {
  // set
  fj_hdmi_audio_set_start(gHDMI_Audio_Frequency);

#ifdef CO_HDMI_CTRL_PRINT_ON
  hdmi_get_ctrl_print();
#endif
}

VOID fj_hdmi_audio_stop(void) {
  // mute
  fj_hdmi_audio_mute(1);
}

VOID fj_hdmi_audio_set_start(ULONG sample_freq) {
  FJ_ERR_CODE err_code = FJ_ERR_NG;
  U_IM_HDMI_PHY_STAT0 state;
  INT32 ret;

  // Check HDMI connected.
  ret = Im_HDMI_Get_Phy_Status(&state);
  if (ret != D_DDIM_OK) {
    BF_Debug_Print_Error(("Im_HDMI_Get_Phy_Status error.\n"));
    return;
  }
  if (state.bit.HPD != 1) {
    BF_Debug_Print_Error(("HDMI not connected.\n"));
    return;
  }

  // Set audio sample  frequence.
  err_code = fj_hdmi_audio_ctrl(sample_freq);
  if (err_code != FJ_ERR_OK) {
    BF_Debug_Print_Error(("fj_hdmi_audio_ctrl error.\n"));
  }
}

FJ_ERR_CODE fj_hdmi_audio_mute(BOOL enable) {
  FJ_ERR_CODE ret = FJ_ERR_NG;
  T_IM_HDMI_AUDIO value;

  if (FJ_ERR_OK == Im_HDMI_Get_Audio(&value)) {
    if (enable == TRUE) {
      value.aud_conf0.bit.I2S_in_en = 0;
    } else {
      value.aud_conf0.bit.I2S_in_en = 0x1;
    }
    if (FJ_ERR_OK != Im_HDMI_Set_Audio(&value)) {
      BF_Debug_Print_Warning(
          ("fj_hdmi_audio_mute Input_Param_Err L=%d, ret=0x%x\n", __LINE__,
           ret));
      return FJ_ERR_NG;
    }
    ret = FJ_ERR_OK;
  } else {
    BF_Debug_Print_Warning(
        ("fj_hdmi_audio_mute Input_Param_Err L=%d, ret=0x%x\n", __LINE__, ret));
    ret = FJ_ERR_NG;
  }
  return ret;
}
