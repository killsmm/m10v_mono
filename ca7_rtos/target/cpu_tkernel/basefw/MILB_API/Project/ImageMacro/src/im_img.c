/**
 * @file		im_img.c
 * @brief		JDSIMG macro wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdsimg.h"
#include "im_img.h"
#include "dd_top.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// wait usec
#define im_img_wait_usec( usec )			Dd_ARM_Wait_ns( (usec) * 1000 )

#define D_IM_IMG_JPEG_ACCESS_WAIT_USEC		(1)

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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

/**
 * @brief	IMG initialize.
 * @param	None.
 * @return	None.
 */
VOID Im_IMG_Init( VOID )
{
	// IMGPIPEPSW
	IO_IMG.IMGPIPEPSW.word		= 0;
	// MXICMON
	IO_IMG.MXICMON.word			= 0;
	// SRAMPD
	IO_IMG.SRAMPD.bit.JPEGPD	= 0x0F;

	return;
}

/**
 * @brief		The setting of IMGPIPEPSW register is acquired.
 * @param [in]	imgpipesw_typ : IMGPIPEPSW type.
 * @return		IMGPIPEPSW register value.
 */
USHORT Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_TYP imgpipesw_typ )
{
	USHORT rtn_val = 0;

#ifdef CO_PARAM_CHECK
	if ( imgpipesw_typ > E_IM_IMG_IMGPIPEPSW_B2RCNC ) {	// imgpipesw_typ parameter range check
		Ddim_Assertion( ("E:Im_IMG_Get_IMGPIPEPSW imgpipesw_typ parameter error=%d\n", imgpipesw_typ) );
		return 0;
	}
#endif // CO_PARAM_CHECK

	switch ( imgpipesw_typ ) {						// IMGPIPEPSW type
		case E_IM_IMG_IMGPIPEPSW_R2YSEL :				// R2Y input select
			rtn_val = IO_IMG.IMGPIPEPSW.bit.R2YSEL;
			break;
		case E_IM_IMG_IMGPIPEPSW_B2RCNC :				// B2R connection select
			rtn_val = IO_IMG.IMGPIPEPSW.bit.B2RCNC;
			break;
		default :
			/* Nothing special */
			break;
	}

	return rtn_val ;
}

/**
 * @brief		The IMGPIPEPSW register is set.
 * @param [in]	imgpipesw_typ : IMGPIPEPSW type.
 * @param [in]	imgpipesw_sel : R2Y input select / B2R connection select.
 * @return		None.
 * @attention	If you select imgpipesw_typ = "E_IM_IMG_IMGPIPEPSW_R2YSEL" and imgpipesw_sel = "E_IM_IMG_IMGPIPEPSW_B2R",<br>
				you must not select imgpipesw_typ = "E_IM_IMG_IMGPIPEPSW_B2RCNC" and imgpipesw_sel = "E_IM_IMG_IMGPIPEPSW_LTM".
 */
VOID Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_TYP imgpipesw_typ, E_IM_IMG_IMGPIPEPSW_SEL imgpipesw_sel )
{
#ifdef CO_PARAM_CHECK
	if ( imgpipesw_typ > E_IM_IMG_IMGPIPEPSW_B2RCNC ) {	// imgpipesw_typ parameter range check
		Ddim_Assertion( ("E:Im_IMG_Set_IMGPIPEPSW imgpipesw_typ parameter error=%d\n", imgpipesw_typ) );
		return;
	}

	if ( imgpipesw_sel > E_IM_IMG_IMGPIPEPSW_NONE ) {	// imgpipesw_sel parameter range check
		Ddim_Assertion( ("E:Im_IMG_Set_IMGPIPEPSW imgpipesw_sel parameter error=%d\n", imgpipesw_sel) );
		return;
	}
#endif // CO_PARAM_CHECK

	switch ( imgpipesw_typ ) {						// IMGPIPEPSW type
		case E_IM_IMG_IMGPIPEPSW_R2YSEL :				// R2Y input select
			IO_IMG.IMGPIPEPSW.bit.R2YSEL	= (USHORT)imgpipesw_sel;
			break;
		case E_IM_IMG_IMGPIPEPSW_B2RCNC :				// B2R connection select
			IO_IMG.IMGPIPEPSW.bit.B2RCNC	= (USHORT)imgpipesw_sel;
			break;
		default :
			/* Nothing special */
			break;
	}
	return ;
}

/**
 * @brief		The setting of MXICMON register is acquired.
 * @param [in]	mxicmon_typ : MXICMON type.
 * @return		MXICMON register value.
*/
USHORT Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_TYP mxicmon_typ )
{
	USHORT mxicmon_sts = 0;

#ifdef CO_PARAM_CHECK
	if ( mxicmon_typ > E_IM_IMG_MXICMON_DEN ) {	// mxicmon_typ parameter range check
		Ddim_Assertion( ("E:Im_IMG_Get_MXICMON mxicmon_typ parameter error=%d\n", mxicmon_typ) );
		return 0;
	}
#endif // CO_PARAM_CHECK

	switch ( mxicmon_typ ) {					// MXICMON Type
		case E_IM_IMG_MXICMON_VDEN :				// VD enable
			mxicmon_sts = IO_IMG.MXICMON.bit.VDEN;
			break;
		case E_IM_IMG_MXICMON_HDEN :				// HD enable
			mxicmon_sts = IO_IMG.MXICMON.bit.HDEN;
			break;
		case E_IM_IMG_MXICMON_SENEN :				// SEN ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.SENEN;
			break;
		case E_IM_IMG_MXICMON_SRO1EN :				// SRO1 ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.SRO1EN;
			break;
		case E_IM_IMG_MXICMON_SRO2EN :				// SRO2 ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.SRO2EN;
			break;
		case E_IM_IMG_MXICMON_B2B1EN :				// B2B1 ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.B2B1EN;
			break;
		case E_IM_IMG_MXICMON_B2B2EN :				// B2B2 ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.B2B2EN;
			break;
		case E_IM_IMG_MXICMON_YO1EN :				// R2Y1 ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.YO1EN;
			break;
		case E_IM_IMG_MXICMON_YO2EN :				// R2Y2 ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.YO2EN;
			break;
		case E_IM_IMG_MXICMON_STATEN :				// STAT ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.STATEN;
			break;
		case E_IM_IMG_MXICMON_PASEN :				// PAS ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.PASEN;
			break;
		case E_IM_IMG_MXICMON_DEN :					// DISP ERROR enable
			mxicmon_sts = IO_IMG.MXICMON.bit.DEN;
			break;
		default :
			/* Nothing special */
			break;
	}
	return mxicmon_sts ;
}

/**
 * @brief		The MXICMON register is set.
 * @param [in]	mxicmon_typ : MXICMON type.
 * @param [in]	mxicmon_sts : Enable/Disable.
 * @return		None.
*/
VOID Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_TYP mxicmon_typ, E_IM_IMG_MXICMON_STS  mxicmon_sts )
{
#ifdef CO_PARAM_CHECK
	if ( mxicmon_typ > E_IM_IMG_MXICMON_DEN ) {		// mxicmon_typ parameter range check
		Ddim_Assertion( ("E:Im_IMG_Set_MXICMON mxicmon_typ parameter error=%d\n", mxicmon_typ) );
		return;
	}
	if ( mxicmon_sts > E_IM_IMG_MXICMON_ENABLE ) {	// mxicmon_sts parameter range check
		Ddim_Assertion( ("E:Im_IMG_Set_MXICMON mxicmon_sts parameter error=%d\n", mxicmon_sts) );
		return;
	}
#endif // CO_PARAM_CHECK

	switch ( mxicmon_typ ) {					// MXICMON type
		case E_IM_IMG_MXICMON_VDEN :				// VD enable
			IO_IMG.MXICMON.bit.VDEN		= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_HDEN :				// HD enable
			IO_IMG.MXICMON.bit.HDEN		= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_SENEN :				// SEN ERROR enable
			IO_IMG.MXICMON.bit.SENEN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_SRO1EN :				// SRO1 ERROR enable
			IO_IMG.MXICMON.bit.SRO1EN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_SRO2EN :				// SRO2 ERROR enable
			IO_IMG.MXICMON.bit.SRO2EN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_B2B1EN :				// B2B1 ERROR enable
			IO_IMG.MXICMON.bit.B2B1EN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_B2B2EN :				// B2B2 ERROR enable
			IO_IMG.MXICMON.bit.B2B2EN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_YO1EN :				// R2Y1 ERROR enable
			IO_IMG.MXICMON.bit.YO1EN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_YO2EN :				// R2Y2 ERROR enable
			IO_IMG.MXICMON.bit.YO2EN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_STATEN :				// STAT ERROR enable
			IO_IMG.MXICMON.bit.STATEN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_PASEN :				// PAS ERROR enable
			IO_IMG.MXICMON.bit.PASEN	= (USHORT)mxicmon_sts;
			break;
		case E_IM_IMG_MXICMON_DEN :					// DISP ERROR enable
			IO_IMG.MXICMON.bit.DEN		= (USHORT)mxicmon_sts;
			break;
		default :
			/* Nothing special */
			break;
	}
	return ;
}

/**
 * @brief		The setting of SRAMPD register is acquired.
 * @param [in]	srampd_typ : SRAMPD type.
 * @return		TRUE:Power down ON / FALSE:Power down OFF.
*/
BOOL Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_TYP srampd_typ )
{
	UCHAR	jpegpd		= 0x00;
	BOOL	power_down	= FALSE;

#ifdef CO_PARAM_CHECK
	if ( srampd_typ > E_IM_IMG_SRAMPD_C ) {	// srampd_typ parameter range check
		Ddim_Assertion( ("E:Im_IMG_Get_SRAMPD srampd_typ parameter error=%d\n", srampd_typ) );
		return power_down;
	}
#endif // CO_PARAM_CHECK

	jpegpd = IO_IMG.SRAMPD.bit.JPEGPD;

	switch ( srampd_typ ) {					// SRAMPD type
		case E_IM_IMG_SRAMPD_Y01 :				// Data placement conversion PBUF for Y0-1
			if ( ( jpegpd & 0x08 ) != 0x00 ) {
				power_down = TRUE;
			}
			break;
		case E_IM_IMG_SRAMPD_Y23 :				// Data placement conversion PBUF for Y2-3
			if ( ( jpegpd & 0x04 ) != 0x00 ) {
				power_down = TRUE;
			}
			break;
		case E_IM_IMG_SRAMPD_C :				// Data placement conversion PBUF for C
			if ( ( jpegpd & 0x02 ) != 0x00 ) {
				power_down = TRUE;
			}
			break;
		default :
			/* Nothing special */
			break;
	}

	return power_down;
}

/**
 * @brief		The SRAMPD register is set.
 * @param [in]	srampd_typ : SRAMPD type.
 * @param [in]	power_down : TRUE:Power down ON / FALSE:Power down OFF.
 * @return		None.
 * @attention	If you select power_down = "FALSE:Power down OFF", wait 1us until jpeg access.
*/
VOID Im_IMG_Set_SRAMPD( E_IM_IMG_SRAMPD_TYP srampd_typ, BOOL power_down )
{
	UCHAR jpegpd = 0x00;

#ifdef CO_PARAM_CHECK
	if ( srampd_typ > E_IM_IMG_SRAMPD_C ) {	// srampd_typ parameter range check
		Ddim_Assertion( ("E:Im_IMG_Set_SRAMPD srampd_typ parameter error=%d\n", srampd_typ) );
		return;
	}

	if ( power_down > TRUE ) {	// power_down parameter range check
		Ddim_Assertion( ("E:Im_IMG_Set_SRAMPD power_down parameter error=%d\n", power_down) );
		return;
	}
#endif // CO_PARAM_CHECK

	jpegpd = IO_IMG.SRAMPD.bit.JPEGPD;

	switch ( srampd_typ ) {					// SRAMPD type
		case E_IM_IMG_SRAMPD_Y01 :				// Data placement conversion PBUF for Y0-1
			if ( power_down ) {
				jpegpd |= 0x08;
			}
			else {
				jpegpd &= 0x07;
			}
			break;
		case E_IM_IMG_SRAMPD_Y23 :				// Data placement conversion PBUF for Y2-3
			if ( power_down ) {
				jpegpd |= 0x04;
			}
			else {
				jpegpd &= 0x0B;
			}
			break;
		case E_IM_IMG_SRAMPD_C :				// Data placement conversion PBUF for C
			if ( power_down ) {
				jpegpd |= 0x02;
			}
			else {
				jpegpd &= 0x0D;
			}
			break;
		default :
			/* Nothing special */
			break;
	}

	IO_IMG.SRAMPD.bit.JPEGPD = jpegpd;

	if ( !power_down ) {
		im_img_wait_usec( D_IM_IMG_JPEG_ACCESS_WAIT_USEC );
	}

	return;
}
