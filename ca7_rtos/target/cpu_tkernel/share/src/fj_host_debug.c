/**
 * @file		fj_host_debug.c
 * @brief		Host debug data.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_host_debug.h"
#include "TKernel_id.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#if defined(CO_DEBUG_MBALOG_ON) || defined(CO_BF_MBALOG_ON)
// MBA Log ID Name List
const FJ_MBALOG_ID_LIST gMBALog_Id_List = {
	// Function ID Name
	{
		[FJ_MBALOG_FUNC_ID_HOST] = "Host",
		[FJ_MBALOG_FUNC_ID_SYSTEM] = "System",
		[FJ_MBALOG_FUNC_ID_LOAD] = "Load",
		[FJ_MBALOG_FUNC_ID_LCD] = "LCD",
		[FJ_MBALOG_FUNC_ID_LENS] = "Lens",
		[FJ_MBALOG_FUNC_ID_AFE] = "AFE",
		[FJ_MBALOG_FUNC_ID_CCD] = "CCD",
		[FJ_MBALOG_FUNC_ID_AUDIO] = "Audio",
		[FJ_MBALOG_FUNC_ID_VIEW] = "View",
		[FJ_MBALOG_FUNC_ID_CAPTURE] = "Capture",
		[FJ_MBALOG_FUNC_ID_IQ] = "IQ",
		[FJ_MBALOG_FUNC_ID_DCF] = "DCF",
		[FJ_MBALOG_FUNC_ID_DPOF] = "DPOF",
		[FJ_MBALOG_FUNC_ID_EXIF] = "EXIF",
		[FJ_MBALOG_FUNC_ID_FS] = "FS",
		[FJ_MBALOG_FUNC_ID_MEDIA] = "Media",
		[FJ_MBALOG_FUNC_ID_MOVIE] = "Movie",
		[FJ_MBALOG_FUNC_ID_OSD] = "OSD",
		[FJ_MBALOG_FUNC_ID_PB] = "PB",
		[FJ_MBALOG_FUNC_ID_AE] = "AE",
		[FJ_MBALOG_FUNC_ID_AF] = "AF",
		[FJ_MBALOG_FUNC_ID_AWB] = "AWB",
		[FJ_MBALOG_FUNC_ID_CALIB] = "Calib",
		[FJ_MBALOG_FUNC_ID_BAYER] = "Bayer",
		[FJ_MBALOG_FUNC_ID_SEN] = "SEN",
		[FJ_MBALOG_FUNC_ID_NCH] = "Nch",
		[FJ_MBALOG_FUNC_ID_SG] = "SG",
		[FJ_MBALOG_FUNC_ID_SHADE] = "SHADE",
		[FJ_MBALOG_FUNC_ID_R2Y] = "R2Y",
		[FJ_MBALOG_FUNC_ID_FRECT] = "FRECT",
		[FJ_MBALOG_FUNC_ID_XCH] = "Xch",
		[FJ_MBALOG_FUNC_ID_BLEND] = "BLEND",
		[FJ_MBALOG_FUNC_ID_AFN] = "AFN",
		[FJ_MBALOG_FUNC_ID_SUPPRE] = "SUPPRE",
		[FJ_MBALOG_FUNC_ID_JPEG] = "JPEG",
		[FJ_MBALOG_FUNC_ID_MXIC] = "MXIC",
		[FJ_MBALOG_FUNC_ID_VLD] = "VLD",
		[FJ_MBALOG_FUNC_ID_WDR] = "WDR",
		[FJ_MBALOG_FUNC_ID_FD] = "FD",
		[FJ_MBALOG_FUNC_ID_IIPFLT] = "IIPFLT",
		[FJ_MBALOG_FUNC_ID_SCENE] = "SCN",
		[FJ_MBALOG_FUNC_ID_DRAM] = "DRAM",
		[FJ_MBALOG_FUNC_ID_YUV] = "YUV",
		[FJ_MBALOG_FUNC_ID_SHUTTER] = "Shutter",
		[FJ_MBALOG_FUNC_ID_HEAP] = "Heap",
		[FJ_MBALOG_FUNC_ID_COMMON] = "Common",
		[FJ_MBALOG_FUNC_ID_ELA] = "ELA",
		[FJ_MBALOG_FUNC_ID_OBJTRACK] = "ObjTrack",
		[FJ_MBALOG_FUNC_ID_SGDE] = "SGDE",
		[FJ_MBALOG_FUNC_ID_MD] = "MD",
		[FJ_MBALOG_FUNC_ID_SFB] = "SFB",
		[FJ_MBALOG_FUNC_ID_SYNC_CTRL] = "SYNC_CTRL",
		[FJ_MBALOG_FUNC_ID_SEN_INT] = "SEN_INT",
		[FJ_MBALOG_FUNC_ID_B2R_INT] = "B2R_INT",
		[FJ_MBALOG_FUNC_ID_LTM_RBK_INT] = "LTM_RBK_INT",
		[FJ_MBALOG_FUNC_ID_LTM_MAP_INT] = "LTM_MAP_INT",
		[FJ_MBALOG_FUNC_ID_LTM_LTM_INT] = "LTM_LTM_INT",
		[FJ_MBALOG_FUNC_ID_R2Y_INT] = "R2Y_INT",
		[FJ_MBALOG_FUNC_ID_CNR_OTF_INT] = "CNR_OTF_INT",
		[FJ_MBALOG_FUNC_ID_FUNC_START] = "Func_Start",
		[FJ_MBALOG_FUNC_ID_VSYNC_INT] = "VSYNC_INT",
		[FJ_MBALOG_FUNC_ID_3A_0] = "3A_0",
		[FJ_MBALOG_FUNC_ID_IP_CHECK] = "IP_CHECK",
		[FJ_MBALOG_FUNC_ID_HDR] = "HDR",
		[FJ_MBALOG_FUNC_ID_AUDIO_AAC] = "AudioAAC",
		[FJ_MBALOG_FUNC_ID_AUDIO_ULAW] = "AudioULAW",
		[FJ_MBALOG_FUNC_ID_IIP_SL0END] = "IIP_SL0END",
		[FJ_MBALOG_FUNC_ID_IIP_SL1END] = "IIP_SL1END",
		[FJ_MBALOG_FUNC_ID_IIP_SL2END] = "IIP_SL2END",
		[FJ_MBALOG_FUNC_ID_IIP_SL3END] = "IIP_SL3END",
		[FJ_MBALOG_FUNC_ID_IIP_SL4END] = "IIP_SL4END",
		[FJ_MBALOG_FUNC_ID_IIP_SL5END] = "IIP_SL5END",
		[FJ_MBALOG_FUNC_ID_IIP_SL6END] = "IIP_SL6END",
		[FJ_MBALOG_FUNC_ID_IIP_SL7END] = "IIP_SL7END",
		[FJ_MBALOG_FUNC_ID_IIP_SL8END] = "IIP_SL8END",
		[FJ_MBALOG_FUNC_ID_IMGPROC_VD] = "IMGPROC_VD",
		[FJ_MBALOG_FUNC_ID_HOSTFUNC1] = "HostFunc1",	// Host Customizable
// --- REMOVE_3DNR BEGIN ---
		[FJ_MBALOG_FUNC_ID_3DNR			] = "3DNR",
// --- REMOVE_3DNR END ---
		[FJ_MBALOG_FUNC_ID_SRO1			] = "SRO1",
		[FJ_MBALOG_FUNC_ID_SRO2			] = "SRO2",
		[FJ_MBALOG_FUNC_ID_B2B1			] = "B2B1",
		[FJ_MBALOG_FUNC_ID_B2B2			] = "B2B2",
		[FJ_MBALOG_FUNC_ID_STAT			] = "STAT",
		[FJ_MBALOG_FUNC_ID_PAS			] = "PAS",
		[FJ_MBALOG_FUNC_ID_SRO1_INT		] = "SRO1_INT",
		[FJ_MBALOG_FUNC_ID_SRO2_INT		] = "SRO2_INT",
		[FJ_MBALOG_FUNC_ID_B2B1_INT		] = "B2B1_INT",
		[FJ_MBALOG_FUNC_ID_B2B2_INT		] = "B2B2_INT",
		[FJ_MBALOG_FUNC_ID_STAT_INT		] = "STAT_INT",
		[FJ_MBALOG_FUNC_ID_PAS_INT		] = "PAS_INT",
		[FJ_MBALOG_FUNC_ID_SEN_IF0_INT	] = "SEN_IF0_INT",
		[FJ_MBALOG_FUNC_ID_SEN_IF1_INT	] = "SEN_IF1_INT",
		[FJ_MBALOG_FUNC_ID_SEN_IF2_INT	] = "SEN_IF2_INT",
		[FJ_MBALOG_FUNC_ID_SEN_IF3_INT	] = "SEN_IF3_INT",
		[FJ_MBALOG_FUNC_ID_3A_1			] = "3A_1",
		[FJ_MBALOG_FUNC_ID_3A_2			] = "3A_2",
		[FJ_MBALOG_FUNC_ID_3A_3			] = "3A_3",
		[FJ_MBALOG_FUNC_ID_GYRO			] = "GYRO",
		[FJ_MBALOG_FUNC_ID_HEVC			] = "HEVC",
// --- REMOVE_3DNR BEGIN ---
		[FJ_MBALOG_FUNC_ID_GMD			] = "GMD",
		[FJ_MBALOG_FUNC_ID_ME			] = "ME",
// --- REMOVE_3DNR END ---
// --- REMOVE_EIS BEGIN ---
		[FJ_MBALOG_FUNC_ID_EIS			] = "EIS",
// --- REMOVE_EIS END ---
		[FJ_MBALOG_FUNC_ID_PIV			] = "PIV",
		
		[FJ_MBALOG_FUNC_ID_LINUX_CMD		] = "CMD",
		[FJ_MBALOG_FUNC_ID_LINUX_STR_264_0	] = "STR 264 0",
		[FJ_MBALOG_FUNC_ID_LINUX_REL_264_0	] = "REL 264 0",
		[FJ_MBALOG_FUNC_ID_LINUX_STR_264_1	] = "STR 264 1",
		[FJ_MBALOG_FUNC_ID_LINUX_REL_264_1	] = "REL 264 1",
		[FJ_MBALOG_FUNC_ID_LINUX_STR_MJPG	] = "STR MJPG",
		[FJ_MBALOG_FUNC_ID_LINUX_REL_MJPG	] = "REL MJPG",
		[FJ_MBALOG_FUNC_ID_LINUX_STR_YUV	] = "STR YUV",
		[FJ_MBALOG_FUNC_ID_LINUX_REL_YUV	] = "REL YUV",
		[FJ_MBALOG_FUNC_ID_LINUX_STR_RAW	] = "STR RAW",
		[FJ_MBALOG_FUNC_ID_LINUX_REL_RAW	] = "REL RAW",
		[FJ_MBALOG_FUNC_ID_LINUX_STR_265	] = "STR 265",
		[FJ_MBALOG_FUNC_ID_LINUX_REL_265	] = "REL 265",
	}
};
#endif	/* CO_DEBUG_MBALOG_ON or CO_BF_MBALOG_ON */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
