/**
 * @file		jpgepc.h
 * @brief		JPEG fixed-length calc and Encode Ctrl
 * @note		None
 * @attention	None
 */

#ifndef _JPG_EPC__
#define _JPG_EPC__

#include "im_jpeg.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_JPGEPC_RETRY_MAX_COUNT		(30)
#define D_JPGEPC_MACRO_WAIT_TIME		(1000)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_BF_JPGEPC_MODE_CAPTURE = 0,
	E_BF_JPGEPC_MODE_PLAY,
	E_BF_JPGEPC_MODE_MOVIE,
	E_BF_JPGEPC_MODE_MAX,
} E_BF_JPGEPC_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	T_IM_JPEG_ENC_MNG			jpg_enc_mng;		// Parameter of Im_JPEG_Ctrl_Enc().
	T_IM_JPEG_ENC_FRAME_MNG		jpg_enc_frame_mng;	// Parameter of Im_JPEG_Ctrl_Enc_Frame().
	T_IM_JPEG_QUAT_TBL_PACK		jpg_enc_quant_tbl;	// Parameter of Im_JPEG_Set_QTbl()
	E_BF_JPGEPC_MODE			mode;				// Encode mode (parameter of Jpgepc_Wrap_Set_Tuning_Param()).
	ULONG						limit_size;			// Maximum bytes of JPEG code.
	UINT32						wait_time;			// Wait time for JPEG macro.
	USHORT						quality;			// Quality value of JPEG code when encode is successful. (In/Out)
	USHORT						cr;					// Target CR.
} T_BF_JPGENC_PARAM;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function													  	*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function													  	*/
/*----------------------------------------------------------------------*/

/**
Retry Status in the JPEG Encode on the image.
@retval		D_IM_B2Y_UNLOCK_OK			: @ref D_IM_B2Y_UNLOCK_OK
@retval		D_IM_B2Y_UNLOCK_ERR 		: @ref D_IM_B2Y_UNLOCK_ERR
*/
extern UCHAR Jpgepc_Get_Retry_Status( VOID );

/**
JPEG Encode process of the image at the time of shooting(jpeg encode (thumb/main)).
@param[in]	pjpeg_enc_tbl	:Jpeg Endode Maneger Table.
@retval		ULONG	: Jpeg code size(Byte).
*/
extern ULONG Jpgepc_Encode_Jpeg( T_BF_JPGENC_PARAM* const jpgenc_param );

#endif
