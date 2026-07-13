/**
 * @file		play_jpeg.h
 * @brief		play jpeg process
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _PLAY_JPEG_H_
#define _PLAY_JPEG_H_

#include "play_common.h"


/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_BF_PLAY_JPEG_LOAD_STOP	(2)

// Upper bound of size of file of possible LOAD of PlayBack.
#define D_BF_PLAY_JPEG_LOAD_MAX_FILE_SIZE	(PIX_SIZ_PLAY_DEFAULT_SIZE)


/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
typedef enum {
	E_BF_PLAY_JPEG_FILL_REMAKE_FLG_ON,
	E_BF_PLAY_JPEG_FILL_REMAKE_FLG_OFF
} E_BF_PLAY_JPEG_FILL_REMAKE_FLG;


/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/
INT32		BF_Play_Jpeg_Single( VOID );
// --- REMOVE_DCF_EXIF BEGIN ---
INT32		BF_Play_Jpeg_Main( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param, E_BF_DCF_IF_ROTATE rotate );
INT32		BF_Play_Jpeg_Thumbnail( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param, E_BF_DCF_IF_ROTATE rotate );
// --- REMOVE_DCF_EXIF END ---
FJ_ERR_CODE BF_Play_Jpeg_Set_Main_Image(VOID);

#endif	// _PLAY_JPEG_H_
