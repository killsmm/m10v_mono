/**
 * @file		fj_host_debug.h
 * @brief		HOST Debug Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_HOST_DEBUG_H_
#define _FJ_HOST_DEBUG_H_

#include "fj_debug_mba.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/** @weakgroup fj_debug_mba_definition
@{*/
/** MBA Log Function ID */
typedef enum {
	FJ_MBALOG_FUNC_ID_HOST = 0,		/**< FUNC ID = 0 (for Host) */
	FJ_MBALOG_FUNC_ID_SYSTEM,		/**< FUNC ID = 1 (for System) */
	FJ_MBALOG_FUNC_ID_LOAD,			/**< FUNC ID = 2 (for Load) */
	FJ_MBALOG_FUNC_ID_LCD,			/**< FUNC ID = 3 (for Lcd) */
	FJ_MBALOG_FUNC_ID_LENS,			/**< FUNC ID = 4 (for Lens) */
	FJ_MBALOG_FUNC_ID_AFE,			/**< FUNC ID = 5 (for Afe) */
	FJ_MBALOG_FUNC_ID_CCD,			/**< FUNC ID = 6 (for Ccd) */
	FJ_MBALOG_FUNC_ID_AUDIO,		/**< FUNC ID = 7 (for Audio) */
	FJ_MBALOG_FUNC_ID_VIEW,			/**< FUNC ID = 8 (for View) */
	FJ_MBALOG_FUNC_ID_CAPTURE,		/**< FUNC ID = 9 (for Capture) */
	FJ_MBALOG_FUNC_ID_IQ,			/**< FUNC ID = 10 (for Iq) */
	FJ_MBALOG_FUNC_ID_DCF,			/**< FUNC ID = 11 (for Dcf) */
	FJ_MBALOG_FUNC_ID_DPOF,			/**< FUNC ID = 12 (for Dpof) */
	FJ_MBALOG_FUNC_ID_EXIF,			/**< FUNC ID = 13 (for Exif) */
	FJ_MBALOG_FUNC_ID_FS,			/**< FUNC ID = 14 (for Fs) */
	FJ_MBALOG_FUNC_ID_MEDIA,		/**< FUNC ID = 15 (for Media) */
	FJ_MBALOG_FUNC_ID_MOVIE,		/**< FUNC ID = 16 (for Movie) */
	FJ_MBALOG_FUNC_ID_OSD,			/**< FUNC ID = 17 (for Osd) */
	FJ_MBALOG_FUNC_ID_PB,			/**< FUNC ID = 18 (for Pb) */
	FJ_MBALOG_FUNC_ID_AE,			/**< FUNC ID = 19 (for Ae) */
	FJ_MBALOG_FUNC_ID_AF,			/**< FUNC ID = 20 (for Af) */
	FJ_MBALOG_FUNC_ID_AWB,			/**< FUNC ID = 21 (for Awb) */
	FJ_MBALOG_FUNC_ID_CALIB,		/**< FUNC ID = 22 (for Calib) */
	FJ_MBALOG_FUNC_ID_BAYER,		/**< FUNC ID = 23 (for Bayer) */
	FJ_MBALOG_FUNC_ID_SEN,			/**< FUNC ID = 24 (for SEN) */
	FJ_MBALOG_FUNC_ID_NCH,			/**< FUNC ID = 25 (for Nch) */
	FJ_MBALOG_FUNC_ID_SG,			/**< FUNC ID = 26 (for Sg) */
	FJ_MBALOG_FUNC_ID_SHADE,		/**< FUNC ID = 27 (for Shade) */
	FJ_MBALOG_FUNC_ID_B2R,			/**< FUNC ID = 28 (for B2r) */
	FJ_MBALOG_FUNC_ID_R2Y,			/**< FUNC ID = 29 (for R2y) */
	FJ_MBALOG_FUNC_ID_FRECT,		/**< FUNC ID = 30 (for IIP-FRECT) */
	FJ_MBALOG_FUNC_ID_XCH,			/**< FUNC ID = 31 (for Xch) */
	FJ_MBALOG_FUNC_ID_BLEND,		/**< FUNC ID = 32 (for IIP-BLEND) */
	FJ_MBALOG_FUNC_ID_AFN,			/**< FUNC ID = 33 (for IIP-AFN) */
	FJ_MBALOG_FUNC_ID_SUPPRE,		/**< FUNC ID = 34 (for Suppre) */
	FJ_MBALOG_FUNC_ID_JPEG,			/**< FUNC ID = 35 (for Jpeg) */
	FJ_MBALOG_FUNC_ID_MXIC,			/**< FUNC ID = 36 (for Mxic) */
	FJ_MBALOG_FUNC_ID_VLD,			/**< FUNC ID = 37 (for Vld) */
	FJ_MBALOG_FUNC_ID_WDR,			/**< FUNC ID = 38 (for Wdr) */
	FJ_MBALOG_FUNC_ID_FD,			/**< FUNC ID = 39 (for Fd) */
	FJ_MBALOG_FUNC_ID_IIPFLT,		/**< FUNC ID = 40 (for IIP-Filter Unit) */
	FJ_MBALOG_FUNC_ID_SCENE,		/**< FUNC ID = 41 (for Scene) */
	FJ_MBALOG_FUNC_ID_DRAM,			/**< FUNC ID = 42 (for Dram) */
	FJ_MBALOG_FUNC_ID_YUV,			/**< FUNC ID = 43 (for Yuv) */
	FJ_MBALOG_FUNC_ID_SHUTTER,		/**< FUNC ID = 44 (for Shutter) */
	FJ_MBALOG_FUNC_ID_HEAP,			/**< FUNC ID = 45 (for Heap) */
	FJ_MBALOG_FUNC_ID_COMMON,		/**< FUNC ID = 46 (for Common) */
	FJ_MBALOG_FUNC_ID_ELA,			/**< FUNC ID = 47 (for Ela) */
	FJ_MBALOG_FUNC_ID_OBJTRACK,		/**< FUNC ID = 48 (for Object Tracking) */
	FJ_MBALOG_FUNC_ID_SGDE,			/**< FUNC ID = 49 (for SGDE) */
	FJ_MBALOG_FUNC_ID_MD,			/**< FUNC ID = 50 (for MD) */
	FJ_MBALOG_FUNC_ID_SFB,			/**< FUNC ID = 51 (for SFB) */
	FJ_MBALOG_FUNC_ID_SYNC_CTRL,	/**< FUNC ID = 52 (for Sync Controller) */
	FJ_MBALOG_FUNC_ID_SEN_INT,		/**< FUNC ID = 53 (for PRO IntTask */
	FJ_MBALOG_FUNC_ID_B2R_INT,		/**< FUNC ID = 54 (for B2R IntTask */
	FJ_MBALOG_FUNC_ID_LTM_RBK_INT,	/**< FUNC ID = 55 (for LTM RBK IntTask */
	FJ_MBALOG_FUNC_ID_LTM_MAP_INT,	/**< FUNC ID = 56 (for LTM MAP IntTask */
	FJ_MBALOG_FUNC_ID_LTM_LTM_INT,	/**< FUNC ID = 57 (for LTM CORE IntTask */
	FJ_MBALOG_FUNC_ID_R2Y_INT,		/**< FUNC ID = 58 (for R2Y IntTask */
	FJ_MBALOG_FUNC_ID_CNR_OTF_INT,	/**< FUNC ID = 59 (for CNR OTFF IntTask */
	FJ_MBALOG_FUNC_ID_FUNC_START,	/**< FUNC ID = 60  */
	FJ_MBALOG_FUNC_ID_VSYNC_INT,	/**< FUNC ID = 61 (for VSYNC IntTask) */
	FJ_MBALOG_FUNC_ID_3A_0,			/**< FUNC ID = 62 (for 3A 0) */
	FJ_MBALOG_FUNC_ID_IP_CHECK,		/**< FUNC ID = 63 (for IP check) */
	FJ_MBALOG_FUNC_ID_HDR,			/**< FUNC ID = 64 (for HDR) */
	FJ_MBALOG_FUNC_ID_MFC,			/**< FUNC ID = 65 (for MFC) */
	FJ_MBALOG_FUNC_ID_AUDIO_AAC,	/**< FUNC ID = 66 (for AAC Encode/Decode) */
	FJ_MBALOG_FUNC_ID_AUDIO_ULAW,	/**< FUNC ID = 67 (for ULAW Encode/Decode) */
	FJ_MBALOG_FUNC_ID_IIP_SL0END,	/**< FUNC ID = 68 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IIP_SL1END,	/**< FUNC ID = 69 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IIP_SL2END,	/**< FUNC ID = 70 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IIP_SL3END,	/**< FUNC ID = 71 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IIP_SL4END,	/**< FUNC ID = 72 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IIP_SL5END,	/**< FUNC ID = 73 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IIP_SL6END,	/**< FUNC ID = 74 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IIP_SL7END,	/**< FUNC ID = 75 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IIP_SL8END,	/**< FUNC ID = 76 (for IIP SL Unit end) */
	FJ_MBALOG_FUNC_ID_IMGPROC_VD,	/**< FUNC ID = 77 (for Image Procedure VD) */
	FJ_MBALOG_FUNC_ID_HOSTFUNC1,	/**< FUNC ID = 78 (Host Customizable) */
// --- REMOVE_3DNR BEGIN ---
	FJ_MBALOG_FUNC_ID_3DNR,			/**< FUNC ID = 79 (for 3DNR) */
// --- REMOVE_3DNR END ---
	FJ_MBALOG_FUNC_ID_API_CHECK,	/**< FUNC ID = 80 (for API call check) */
	FJ_MBALOG_FUNC_ID_CB_CHECK,		/**< FUNC ID = 81 (for CB API call check) */
	FJ_MBALOG_FUNC_ID_SRO1,			/**< FUNC ID = 82 (for SRO1) */
	FJ_MBALOG_FUNC_ID_SRO2,			/**< FUNC ID = 83 (for SRO2) */
	FJ_MBALOG_FUNC_ID_B2B1,			/**< FUNC ID = 84 (for B2B1) */
	FJ_MBALOG_FUNC_ID_B2B2,			/**< FUNC ID = 85 (for B2B2) */
	FJ_MBALOG_FUNC_ID_STAT,			/**< FUNC ID = 86 (for STAT) */
	FJ_MBALOG_FUNC_ID_PAS,			/**< FUNC ID = 87 (for PAS) */
	FJ_MBALOG_FUNC_ID_SRO1_INT,		/**< FUNC ID = 88 (for SRO1 IntTask */
	FJ_MBALOG_FUNC_ID_SRO2_INT,		/**< FUNC ID = 89 (for SRO2 IntTask */
	FJ_MBALOG_FUNC_ID_B2B1_INT,		/**< FUNC ID = 90 (for B2B1 IntTask */
	FJ_MBALOG_FUNC_ID_B2B2_INT,		/**< FUNC ID = 91 (for B2B2 IntTask */
	FJ_MBALOG_FUNC_ID_STAT_INT,		/**< FUNC ID = 92 (for STAT IntTask */
	FJ_MBALOG_FUNC_ID_PAS_INT,		/**< FUNC ID = 93 (for PAS IntTask */
	FJ_MBALOG_FUNC_ID_SEN_IF0_INT,	/**< FUNC ID = 94(for Sensor I/F0 IntTask */
	FJ_MBALOG_FUNC_ID_SEN_IF1_INT,	/**< FUNC ID = 95 (for Sensor I/F1 IntTask */
	FJ_MBALOG_FUNC_ID_SEN_IF2_INT,	/**< FUNC ID = 96 (for Sensor I/F2 IntTask */
	FJ_MBALOG_FUNC_ID_SEN_IF3_INT,	/**< FUNC ID = 97 (for Sensor I/F3 IntTask */
	FJ_MBALOG_FUNC_ID_3A_1,			/**< FUNC ID = 98 (for 3A 1) */
	FJ_MBALOG_FUNC_ID_3A_2,			/**< FUNC ID = 99 (for 3A 2) */
	FJ_MBALOG_FUNC_ID_3A_3,			/**< FUNC ID = 100 (for 3A 3) */
	FJ_MBALOG_FUNC_ID_GYRO,			/**< FUNC ID = 101 (for Gyro) */
	FJ_MBALOG_FUNC_ID_HEVC,			/**< FUNC ID = 102 (for HEVC) */
// --- REMOVE_3DNR BEGIN ---
	FJ_MBALOG_FUNC_ID_GMD,			/**< FUNC ID = 103 (for GMD) */
	FJ_MBALOG_FUNC_ID_ME,			/**< FUNC ID = 104 (for ME) */
// --- REMOVE_3DNR END ---
// --- REMOVE_EIS BEGIN ---
	FJ_MBALOG_FUNC_ID_EIS,			/**< FUNC ID = 105 (for EIS) */
// --- REMOVE_EIS END ---
	FJ_MBALOG_FUNC_ID_PIV,			/**< FUNC ID = 106 (for Piv) */
	
	FJ_MBALOG_FUNC_ID_LINUX_CMD,			/**< FUNC ID = 107 */
	FJ_MBALOG_FUNC_ID_LINUX_STR_264_0,		/**< FUNC ID = 108 */
	FJ_MBALOG_FUNC_ID_LINUX_REL_264_0,		/**< FUNC ID = 109 */
	FJ_MBALOG_FUNC_ID_LINUX_STR_264_1,		/**< FUNC ID = 110 */
	FJ_MBALOG_FUNC_ID_LINUX_REL_264_1,		/**< FUNC ID = 111 */
	FJ_MBALOG_FUNC_ID_LINUX_STR_MJPG,		/**< FUNC ID = 112 */
	FJ_MBALOG_FUNC_ID_LINUX_REL_MJPG,		/**< FUNC ID = 113 */
	FJ_MBALOG_FUNC_ID_LINUX_STR_YUV,		/**< FUNC ID = 114 */
	FJ_MBALOG_FUNC_ID_LINUX_REL_YUV,		/**< FUNC ID = 115 */
	FJ_MBALOG_FUNC_ID_LINUX_STR_RAW,		/**< FUNC ID = 116 */
	FJ_MBALOG_FUNC_ID_LINUX_REL_RAW,		/**< FUNC ID = 117 */
	FJ_MBALOG_FUNC_ID_LINUX_STR_265,		/**< FUNC ID = 118 */
	FJ_MBALOG_FUNC_ID_LINUX_REL_265,		/**< FUNC ID = 119 */
	FJ_MBALOG_FUNC_ID_LINUX_STR_AUDIO,		/**< FUNC ID = 120 */
	FJ_MBALOG_FUNC_ID_LINUX_REL_AUDIO,		/**< FUNC ID = 121 */
	FJ_MBALOG_FUNC_ID_LINUX_EVENT_NOTIFY,	/**< FUNC ID = 122 */
	
} FJ_MBALOG_FUNC_ID;
/*@} --- end of fj_debug_mba_definition */

typedef enum {
	FJ_MBALOG_LINE_ID_00 = 0,
	FJ_MBALOG_LINE_ID_01,
	FJ_MBALOG_LINE_ID_02,
	FJ_MBALOG_LINE_ID_03,
	FJ_MBALOG_LINE_ID_04,
	FJ_MBALOG_LINE_ID_05,
	FJ_MBALOG_LINE_ID_06,
	FJ_MBALOG_LINE_ID_07,
	FJ_MBALOG_LINE_ID_08,
	FJ_MBALOG_LINE_ID_09,
	FJ_MBALOG_LINE_ID_10,
	FJ_MBALOG_LINE_ID_11,
	FJ_MBALOG_LINE_ID_12,
	FJ_MBALOG_LINE_ID_13,
	FJ_MBALOG_LINE_ID_14,
	FJ_MBALOG_LINE_ID_15,
	FJ_MBALOG_LINE_ID_16,
	FJ_MBALOG_LINE_ID_17,
	FJ_MBALOG_LINE_ID_18,
	FJ_MBALOG_LINE_ID_19,
	FJ_MBALOG_LINE_ID_20,
	FJ_MBALOG_LINE_ID_21,
	FJ_MBALOG_LINE_ID_22,
	FJ_MBALOG_LINE_ID_23,
	FJ_MBALOG_LINE_ID_24,
	FJ_MBALOG_LINE_ID_25,
	FJ_MBALOG_LINE_ID_26,
	FJ_MBALOG_LINE_ID_27,
	FJ_MBALOG_LINE_ID_28,
	FJ_MBALOG_LINE_ID_29,
	FJ_MBALOG_LINE_ID_30,
	FJ_MBALOG_LINE_ID_31,
	FJ_MBALOG_LINE_ID_32,
	FJ_MBALOG_LINE_ID_33,
	FJ_MBALOG_LINE_ID_34,
	FJ_MBALOG_LINE_ID_35,
	FJ_MBALOG_LINE_ID_36,
	FJ_MBALOG_LINE_ID_37,
	FJ_MBALOG_LINE_ID_38,
	FJ_MBALOG_LINE_ID_39,
	FJ_MBALOG_LINE_ID_40,
	FJ_MBALOG_LINE_ID_41,
	FJ_MBALOG_LINE_ID_42,
	FJ_MBALOG_LINE_ID_43,
	FJ_MBALOG_LINE_ID_44,
	FJ_MBALOG_LINE_ID_45,
	FJ_MBALOG_LINE_ID_46,
	FJ_MBALOG_LINE_ID_47,
	FJ_MBALOG_LINE_ID_48,
	FJ_MBALOG_LINE_ID_49,
	FJ_MBALOG_LINE_ID_50,
	FJ_MBALOG_LINE_ID_51,
	FJ_MBALOG_LINE_ID_52,
	FJ_MBALOG_LINE_ID_53,
	FJ_MBALOG_LINE_ID_54,
	FJ_MBALOG_LINE_ID_55,
	FJ_MBALOG_LINE_ID_56,
	FJ_MBALOG_LINE_ID_57,
	FJ_MBALOG_LINE_ID_58,
	FJ_MBALOG_LINE_ID_59,
	FJ_MBALOG_LINE_ID_60,
	FJ_MBALOG_LINE_ID_61,
	FJ_MBALOG_LINE_ID_62,
	FJ_MBALOG_LINE_ID_63,

	FJ_MBALOG_LINE_ID_CMD_REQ,
	FJ_MBALOG_LINE_ID_CMD_REL,
	FJ_MBALOG_LINE_ID_STR_REQ,
	FJ_MBALOG_LINE_ID_STR_REL,
	FJ_MBALOG_LINE_ID_NOTIFY,

	FJ_MBALOG_LINE_ID_SYSTEM,
	
	FJ_MBALOG_LINE_ID_SHUTTER,
	FJ_MBALOG_LINE_ID_SENSOR,
	FJ_MBALOG_LINE_ID_GYRO,
	FJ_MBALOG_LINE_ID_LENS,

	FJ_MBALOG_LINE_ID_VD,
	FJ_MBALOG_LINE_ID_LDIV_PWCH,
	FJ_MBALOG_LINE_ID_IQ,
	FJ_MBALOG_LINE_ID_3A,
	FJ_MBALOG_LINE_ID_ADAPAE,
	FJ_MBALOG_LINE_ID_HIST,
	FJ_MBALOG_LINE_ID_PRCH,
	FJ_MBALOG_LINE_ID_B2R,
	FJ_MBALOG_LINE_ID_SRO_R2Y,
	FJ_MBALOG_LINE_ID_CNR_OFL,
	FJ_MBALOG_LINE_ID_LTM_MAP,
	FJ_MBALOG_LINE_ID_LTM_RGB,
	FJ_MBALOG_LINE_ID_VHDR,
	FJ_MBALOG_LINE_ID_LTM_R2Y,
	FJ_MBALOG_LINE_ID_IPU_EIS,
	FJ_MBALOG_LINE_ID_IPU_VIS,
	FJ_MBALOG_LINE_ID_IPU_SGDE,
	FJ_MBALOG_LINE_ID_STITCH,
	FJ_MBALOG_LINE_ID_FPT,
	FJ_MBALOG_LINE_ID_ME,
	FJ_MBALOG_LINE_ID_SHDR,
	FJ_MBALOG_LINE_ID_XCH,
	FJ_MBALOG_LINE_ID_INT,

	FJ_MBALOG_LINE_ID_USER_AE,
	FJ_MBALOG_LINE_ID_USER_AF,
	FJ_MBALOG_LINE_ID_USER_AWB,
	FJ_MBALOG_LINE_ID_USER_FD,
	FJ_MBALOG_LINE_ID_USER_DISP,
	FJ_MBALOG_LINE_ID_USER_JPEG_CTRL,
	FJ_MBALOG_LINE_ID_USER_JPEG_THUMB,
	FJ_MBALOG_LINE_ID_USER_JPEG_ENC,
	FJ_MBALOG_LINE_ID_USER_JPEG_ENC_ACTUAL,
	FJ_MBALOG_LINE_ID_USER_JPEG_SAVE,
	FJ_MBALOG_LINE_ID_H265,
	FJ_MBALOG_LINE_ID_H264_0,
	FJ_MBALOG_LINE_ID_H264_1,
	FJ_MBALOG_LINE_ID_MAX = 128, // should be equal to MBALOG_TASK_ID_LINE_MAX
} FJ_MBALOG_LINE_ID;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/

#endif
