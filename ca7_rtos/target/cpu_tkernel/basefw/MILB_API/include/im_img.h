/**
 * @file		im_img.h
 * @brief		JDSIMG macro wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup image
@{
	@addtogroup im_img
	@{
*/

#ifndef __IM_IMG_H__
#define __IM_IMG_H__

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** IMGPIPEPSW type */
typedef enum {
	E_IM_IMG_IMGPIPEPSW_R2YSEL = 0,		/**< IMGPIPEPSW.R2YSEL : R2Y input select */
	E_IM_IMG_IMGPIPEPSW_B2RCNC			/**< IMGPIPEPSW.B2RCNC : B2R connection select */
} E_IM_IMG_IMGPIPEPSW_TYP;

/** IMGPIPEPSW select */
typedef enum {
	E_IM_IMG_IMGPIPEPSW_B2R = 0,		/**< [R2Y input]B2R  / [B2R connection]NONE */
	E_IM_IMG_IMGPIPEPSW_LTM,			/**< [R2Y input]LTM  / [B2R connection]LTM  */
	E_IM_IMG_IMGPIPEPSW_NONE			/**< [R2Y input]NONE / [B2R connection]NONE */
} E_IM_IMG_IMGPIPEPSW_SEL;

/** MXICMON type */
typedef enum {
	E_IM_IMG_MXICMON_VDEN = 0,			/**< MXICMON.VDEN   : MXIC VD enable */
	E_IM_IMG_MXICMON_HDEN,				/**< MXICMON.HDEN   : MXIC HD enable */
	E_IM_IMG_MXICMON_SENEN,				/**< MXICMON.SENEN  : MXIC SEN  ERROR enable */
	E_IM_IMG_MXICMON_SRO1EN,			/**< MXICMON.SRO1EN : MXIC SRO1 ERROR enable */
	E_IM_IMG_MXICMON_SRO2EN,			/**< MXICMON.SRO2EN : MXIC SRO2 ERROR enable */
	E_IM_IMG_MXICMON_B2B1EN,			/**< MXICMON.B2B1EN : MXIC B2B1 ERROR enable */
	E_IM_IMG_MXICMON_B2B2EN,			/**< MXICMON.B2B2EN : MXIC B2B2 ERROR enable */
	E_IM_IMG_MXICMON_YO1EN,				/**< MXICMON.YO1EN  : MXIC R2Y1 ERROR enable */
	E_IM_IMG_MXICMON_YO2EN,				/**< MXICMON.YO2EN  : MXIC R2Y2 ERROR enable */
	E_IM_IMG_MXICMON_STATEN,			/**< MXICMON.STATEN : MXIC STAT ERROR enable */
	E_IM_IMG_MXICMON_PASEN,				/**< MXICMON.PASEN  : MXIC PAS  ERROR enable */
	E_IM_IMG_MXICMON_DEN				/**< MXICMON.DEN    : MXIC DISP ERROR enable */
} E_IM_IMG_MXICMON_TYP;

/** MXICMON status */
typedef enum {
	E_IM_IMG_MXICMON_DISABLE = 0,		/**< Disable */
	E_IM_IMG_MXICMON_ENABLE				/**< Enable  */
} E_IM_IMG_MXICMON_STS;

/** SRAMPD type */
typedef enum {
	E_IM_IMG_SRAMPD_Y01 = 0,			/**< Data placement conversion PBUF for Y0-1 */
	E_IM_IMG_SRAMPD_Y23,				/**< Data placement conversion PBUF for Y2-3 */
	E_IM_IMG_SRAMPD_C					/**< Data placement conversion PBUF for C    */
} E_IM_IMG_SRAMPD_TYP;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	IMG initialize.
 * @return	None.
 */
extern	VOID	Im_IMG_Init( VOID );

/**
 * @brief		The setting of IMGPIPEPSW register is acquired.
 * @param [in]	imgpipesw_typ : IMGPIPEPSW type.
 * @return		IMGPIPEPSW register value.
*/
extern	USHORT	Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_TYP imgpipesw_typ );

/**
 * @brief		The IMGPIPEPSW register is set.
 * @param [in]	imgpipesw_typ : IMGPIPEPSW type.
 * @param [in]	imgpipesw_sel : R2Y input select / B2R connection select.
 * @return		None.
 * @attention	If you select imgpipesw_typ = "E_IM_IMG_IMGPIPEPSW_R2YSEL" and imgpipesw_sel = "E_IM_IMG_IMGPIPEPSW_B2R",<br>
				you must not select imgpipesw_typ = "E_IM_IMG_IMGPIPEPSW_B2RCNC" and imgpipesw_sel = "E_IM_IMG_IMGPIPEPSW_LTM".
 */
extern	VOID	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_TYP imgpipesw_typ, E_IM_IMG_IMGPIPEPSW_SEL imgpipesw_sel );

/**
 * @brief		The setting of MXICMON register is acquired.
 * @param [in]	mxicmon_typ : MXICMON type.
 * @return		MXICMON register value.
*/
extern	USHORT	Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_TYP mxicmon_typ );

/**
 * @brief		The MXICMON register is set.
 * @param [in]	mxicmon_typ : MXICMON type.
 * @param [in]	mxicmon_sts : Enable/Disable.
 * @return		None.
*/
extern	VOID	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_TYP mxicmon_typ, E_IM_IMG_MXICMON_STS mxicmon_sts );

/**
 * @brief		The setting of SRAMPD register is acquired.
 * @param [in]	srampd_typ : SRAMPD type.
 * @return		TRUE:Power down ON / FALSE:Power down OFF.
*/
extern BOOL Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_TYP srampd_typ );

/**
 * @brief		The SRAMPD register is set.
 * @param [in]	srampd_typ : SRAMPD type.
 * @param [in]	power_down : TRUE:Power down ON / FALSE:Power down OFF.
 * @return		None.
 * @attention	If you select power_down = "FALSE:Power down OFF", wait 1us until jpeg access.
*/
extern VOID Im_IMG_Set_SRAMPD( E_IM_IMG_SRAMPD_TYP srampd_typ, BOOL power_down );

#ifdef __cplusplus
}
#endif

#endif	// __IM_IMG_H__

/*	@}*/
/*@}*/
