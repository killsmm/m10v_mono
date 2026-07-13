/**
 * @file		sdram_map_play.h
 * @brief		SocioNext SDRAM Play Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _SDRAM_PLAY_MAP_H_
#define _SDRAM_PLAY_MAP_H_

#include "ddim_typedef.h"
#include "sdram_map_common.h"
#include "sdram_map_play_define.h"
#include "fj_custom.h"

/***********************************************
 *  Size Definition
 ***********************************************/
// Default size (Maximum size)
#define PIX_SIZ_PLAY_DEFAULT_WIDTH				(WIDTH_36M)
#define PIX_SIZ_PLAY_DEFAULT_LINES				(LINES_36M)
#define PIX_SIZ_PLAY_DEFAULT_SIZE				(PIX_SIZ_PLAY_DEFAULT_WIDTH * PIX_SIZ_PLAY_DEFAULT_LINES)

// YCC image size (Maximum size)
#define SDRAM_SIZ_PLAY_IMAGE_YCC_Y				(PIX_SIZ_PLAY_DEFAULT_SIZE)
#define SDRAM_SIZ_PLAY_IMAGE_YCC_C				(PIX_SIZ_PLAY_DEFAULT_SIZE / 2)

// Middle size for two step REC
#define PIX_SIZ_PLAY_REC_MIDDLE_WIDTH			(WIDTH_1_2M)
#define PIX_SIZ_PLAY_REC_MIDDLE_LINES			(LINES_1_2M)
#define PIX_SIZ_PLAY_REC_MIDDLE_SIZE			(PIX_SIZ_PLAY_REC_MIDDLE_WIDTH * PIX_SIZ_PLAY_REC_MIDDLE_LINES)

// RedEye edit size
#define PIX_SIZ_PLAY_3RDIP_EDIT					(PIX_SIZ_1M)

// DrawFile size
#define PIX_SIZ_PLAY_DRAW_FILE					(PIX_SIZ_4M)

/***********************************************
 *  Number of Bank
 ***********************************************/
#define SDRAM_BNK_PLAY_THUMB_CACHE				(20)
#define D_SDRAM_MAP_PLAY_SDRAM_BNK_VDE			(SDRAM_BNK_PLAY_HDMI_MAIN)

/***********************************************
 *  Size of Area
 ***********************************************/
// PTP ramarea buffer size
#define SDRAM_SIZ_PTP_BUFFER					(232320)

// USB mass storage ramarea buffer size
#define SDRAM_SIZ_USB_MASS_STORAGE_BUFFER		(262144)

// Affine Rotate Table Area Size
//#define SDRAM_SIZ_PLAY_AFN_ROTATE_AREA			(0x20 * 8)								// Affine Table (32Byte * 8sets)

// Display Window Size
#define SDRAM_SIZ_PLAY_DISP_HALF_REC			(PIX_SIZ_VGA / 16)						// (0x4B00)		640 x 480 / 16

// Image Load Area Size
//#define SDRAM_SIZ_PLAY_IMAGE_LOAD				(PIX_SIZ_PLAY_DEFAULT_SIZE / 2)			// [20M] (5184 x 3888) / 50%

// VGA OUT Area Size
#define SDRAM_SIZ_PLAY_VGAOUT_YCC420			(PIX_SIZ_VGA * 3/2)						// (0x70800)	640 x 480 * 1.5
#define SDRAM_SIZ_PLAY_VGAOUT_YCC420_Y			(PIX_SIZ_VGA)							// (0x4B000)	640 x 480
#define SDRAM_SIZ_PLAY_VGAOUT_YCC420_C			(SDRAM_SIZ_PLAY_VGAOUT_YCC420_Y / 4)	// (0x12C00)	640 x 480 / 4 : (YCC420)
#define SDRAM_SIZ_PLAY_VGAOUT_YCC422			(PIX_SIZ_VGA * 2)						// (0x96000)	640 x 480 * 2
#define SDRAM_SIZ_PLAY_VGAOUT_YCC422_Y			(PIX_SIZ_VGA)							// (0x4B000)	640 x 480
#define SDRAM_SIZ_PLAY_VGAOUT_YCC422_C			(SDRAM_SIZ_PLAY_VGAOUT_YCC422_Y / 2)	// (0x25800)	640 x 480 / 2 : (YCC422)

// HD OUT Area Size
#define SDRAM_SIZ_PLAY_HDOUT_YCC420				(PIX_SIZ_FHD * 3/2)						// (0x2F7600)	1920 x 1080 * 1.5
#define SDRAM_SIZ_PLAY_HDOUT_YCC422				(PIX_SIZ_FHD * 2)						// (0x3F4800)	1920 x 1080 * 2

// Image Rotate Area Size
#define SDRAM_SIZ_PLAY_ROTATE_VGA_WORK			(PIX_SIZ_VGA * 3/2)						// (0x70800)	640 x 480 * 1.5
#define SDRAM_SIZ_PLAY_ROTATE_HD_WORK			(PIX_SIZ_FHD * 3/2)						// (0x2F7600)	1920 x 1080 * 1.5

// DrawFile Area Size
#define SDRAM_SIZ_PLAY_DRAW_WORK_AREA			(PIX_SIZ_PLAY_DRAW_FILE * 2)			// (0x798000)	2304 x 1728 * 2
//#define SDRAM_SIZ_PLAY_DRAW_REC_AREA			(PIX_SIZ_PLAY_DRAW_FILE / 16)			// (0x3CC00)	2304 x 1728 / 16

// Image Work Area Size
//#define SDRAM_SIZ_PLAY_IMAGE_WORK				(PIX_SIZ_PLAY_DEFAULT_SIZE * 2)			// (0x269A000)	[14M+] (5184 x 3904) * 2

// Thumbnail Work Area Size
#define SDRAM_SIZ_PLAY_THUMB_WORK				(PIX_SIZ_THUMB * 2)						// (0x9600)		160 x 120 * 2  (suppose YCC422)
#define SDRAM_SIZ_PLAY_THUMB_YCC_Y				(SDRAM_SIZ_PLAY_THUMB_WORK)				// (0x4B00)		160 x 120
#define SDRAM_SIZ_PLAY_THUMB_YCC_C				(SDRAM_SIZ_PLAY_THUMB_WORK)				// (0x4B00)		160 x 120
// Thumbnail Work Area(hukasi) Size
#define SDRAM_SIZ_PLAY_THUMB_H_WORK				((WIDTH_THUMB+32) * (LINES_THUMB+2) * 2) // (0xB700)	192 x 122 * 2

// Thumbnail Load Area Size
#define SDRAM_SIZ_PLAY_THUMB_LOAD				(PIX_SIZ_THUMB * 3)						// (0xE100)		160 x 120 * 3

// 3rdIP Removal Working Memory (1.5*1024*1024) Size
//#define SDRAM_SIZ_PLAY_3RDIP_WORK_AREA			(FJ_CUSTOM_3RDIP_SIZE)					// (0x180000)	1024 x 1024 * 1.5

// 3rdIP Area Size
#define SDRAM_SIZ_PLAY_3RDIP_EDIT_AREA			(PIX_SIZ_PLAY_3RDIP_EDIT * 2)			// (0x180000)	1024 x 768 * 2
#define SDRAM_SIZ_PLAY_NSM_PROC_WIDTH			(((WIDTH_36M+255)>>8)<<8)
#define SDRAM_SIZ_PLAY_NSM_PROC_LINES			(((LINES_36M+255)>>8)<<8)
#define SDRAM_SIZ_PLAY_NSM_WORK_AREA			((SDRAM_SIZ_PLAY_NSM_PROC_WIDTH*SDRAM_SIZ_PLAY_NSM_PROC_LINES) + (WIDTH_36M*LINES_36M) + ((WIDTH_36M>>1)*(LINES_36M>>1)))

// Main Encode Area Size
#define SDRAM_SIZ_PLAY_MAIN_ENC_AREA			(PIX_SIZ_PLAY_DEFAULT_SIZE / 2)			// (0x180000)	4320 x 3264 / 2

// Startup Image Area Size
#define SDRAM_SIZ_PLAY_STARTUP_IMAGE_AREA		(PIX_SIZ_QVGA * 2)						// (0x25800)	320 x 240 * 2


/*--------------------------*/
/* Voice Memo Record & Play */
/*--------------------------*/
// Size
#define SDRAM_SIZ_PLAY_PCM			(0x2EE00)
#define SDRAM_SIZ_PLAY_AUDIO		(0x2EE00)

// bank definitions
#define SDRAM_BNK_PLAY_WAV			(3)
#define SDRAM_BNK_PLAY_PCM			(4)
#define SDRAM_BNK_PLAY_AUDIO		(4)

/*-------------------*/
/* Slide Show Sound */
/*-------------------*/
// Size
#define SDRAM_SIZ_PLAY_AUDIO_WAV_SIZE	(0x13000)
#define SDRAM_SIZ_PLAY_SLIDE_PCM		(0x3E80)
#define SDRAM_SIZ_PLAY_SLIDE_AUDIO		(0x3E80)

// bank definitions
#define SDRAM_BNK_PLAY_SLIDE_WAV		(3)
#define SDRAM_BNK_PLAY_SLIDE_PCM		(3)
#define SDRAM_BNK_PLAY_SLIDE_AUDIO		(3)


/***********************************************
 *  Function List
 ***********************************************/
ULONG	BF_Sdram_Map_Play_Get_Image_Work_Addr( VOID );
ULONG	BF_Sdram_Map_Play_Get_Image_Load_Addr( VOID );
ULONG	BF_Sdram_Map_Play_Get_Vga_Out_Addr( VOID );
ULONG	BF_Sdram_Map_Play_Get_Hd_Out_Addr( VOID );
ULONG	BF_Sdram_Map_Play_Get_Half_Rec_Addr( VOID );

ULONG	BF_Sdram_Map_Play_Get_Ahead_Load_Addr(VOID);

UINT32 BF_Sdram_Map_Play_Get_Decoded_YUV_Main_Addr( VOID );
UINT32 BF_Sdram_Map_Play_Get_Decoded_YUV_Main_Size( VOID );
UINT32 BF_Sdram_Map_Play_Get_Decoded_YUV_Thumb_Addr( VOID );
UINT32 BF_Sdram_Map_Play_Get_Decoded_YUV_Thumb_Size( VOID );
UINT32 BF_Sdram_Map_Play_Get_Resize_YUV_Addr( VOID );
UINT32 BF_Sdram_Map_Play_Get_Resize_YUV_Size( VOID );
UINT32 BF_Sdram_Map_Play_Get_Screen_RGB_Addr( VOID );
UINT32 BF_Sdram_Map_Play_Get_Screen_RGB_Size( VOID );
UINT32 BF_Sdram_Map_Play_Get_Thumb_RGB_Addr( VOID );
UINT32 BF_Sdram_Map_Play_Get_Thumb_RGB_Size( VOID );
UINT32 BF_Sdram_Map_Play_Get_Disp_YUV_LCD_Addr( VOID );
UINT32 BF_Sdram_Map_Play_Get_Disp_YUV_LCD_Size( VOID );
UINT32 BF_Sdram_Map_Play_Get_Disp_YUV_HDMI_Addr( VOID );
UINT32 BF_Sdram_Map_Play_Get_Disp_YUV_HDMI_Size( VOID );

UINT32 Sdram_Map_Play_Change_HDMI_HD(BOOL update);
UINT32 Sdram_Map_Play_Get_HDMI_HD(VOID);
UINT32 Sdram_Map_Play_Get_HDMI_HD_Y(VOID);
UINT32 Sdram_Map_Play_Get_HDMI_HD_Cb(VOID);
VOID   Sdram_Map_Play_Free_HDMI_LockIdx(VOID);

#endif

