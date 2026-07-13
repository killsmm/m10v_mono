/**
 * @file		iip_resmng_secret.h
 * @brief		Header file of IIP resource management library
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _IIP_RESMNG_SECRET_H_
#define _IIP_RESMNG_SECRET_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** PIXID number */
#define D_IIP_RES_PIXID_MAXNUM		(12)

/** Status */
#define D_IIP_RES_CONFIG_STS_FREE	(0)			/**< resources unused ( Acquisition is possible ) */
#define D_IIP_RES_CONFIG_STS_USING	(1)			/**< resources using ( Release is promising ) */
#define D_IIP_RES_CONFIG_STS_LOCK1	(2)			/**< resources occupancy ( With no hope of release ) */
#define D_IIP_RES_CONFIG_STS_LOCK2	(3)			/**< not used ( Use freely ) */

/** Layer Kind */
#define D_IIP_RES_LAYER_HOST_IP		(1)
#define D_IIP_RES_LAYER_BFW			(2)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** User information */
typedef struct {
	INT32	tskid;			/** task id */
	UCHAR	layer;			/** 1 : Calls from Host or IP (D_IIP_RES_CALL_HOST)<br>
								2 : Calls from BaseFW (D_IIP_RES_CALL_BFW)
							 */
} T_IIP_RES_USER;

/** Unit information */
typedef struct {
	UCHAR			stat;	/** (D_IIP_RES_STS_xxxx) */
	T_IIP_RES_USER	user;
} T_IIP_RES_INFO;

/** Unit management table */
typedef struct {
	T_IIP_RES_INFO		oned0;
	T_IIP_RES_INFO		oned1;
	T_IIP_RES_INFO		oned2;
	T_IIP_RES_INFO		oned3;
	T_IIP_RES_INFO		flt;
	T_IIP_RES_INFO		afn0;
	T_IIP_RES_INFO		afn1;
	T_IIP_RES_INFO		afn2;
	T_IIP_RES_INFO		frect0;
	T_IIP_RES_INFO		mon;
	T_IIP_RES_INFO		csc0;
	T_IIP_RES_INFO		csc1;
	T_IIP_RES_INFO		lut;
	T_IIP_RES_INFO		gpc;
	T_IIP_RES_INFO		blend0;
	T_IIP_RES_INFO		blend1;
	T_IIP_RES_INFO		frect1;
	T_IIP_RES_INFO		cfl0;
	T_IIP_RES_INFO		csc2;
	T_IIP_RES_INFO		csc3;
	T_IIP_RES_INFO		oned4;
	T_IIP_RES_INFO		oned5;
	T_IIP_RES_INFO		oned6;
	T_IIP_RES_INFO		oned7;
	T_IIP_RES_INFO		afn3;
	T_IIP_RES_INFO		mft;
	T_IIP_RES_INFO		sl[9];	// sl0~8
} T_IIP_RES_UNIT_TBL;

/** Pixel format table management table */
typedef struct {
	T_IIP_RES_INFO		pixid[D_IIP_RES_PIXID_MAXNUM];
} T_IIP_RES_PIXID_TBL;

/** Other resource management table */
typedef struct {
	T_IIP_RES_INFO		lut_a;
	T_IIP_RES_INFO		lut_b;
	T_IIP_RES_INFO		lut_c;
	T_IIP_RES_INFO		lut_d;
	T_IIP_RES_INFO		lut_e;
	T_IIP_RES_INFO		lut_f;
	T_IIP_RES_INFO		hist_0;
	T_IIP_RES_INFO		hist_1;
	T_IIP_RES_INFO		hist_2;
	T_IIP_RES_INFO		hist_3;
	T_IIP_RES_INFO		bi_cubic_0;
	T_IIP_RES_INFO		bi_cubic_1;
} T_IIP_RES_OTHER_TBL;

/** Resource management table */
typedef struct {
	T_IIP_RES_UNIT_TBL	unit;
	T_IIP_RES_PIXID_TBL	pixid;
	T_IIP_RES_OTHER_TBL	other;
} T_IIP_RES_CONFIG_TBL;

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
#endif	// __cplusplus

/** Callback function pointer	*/
typedef VOID (*T_IIP_RES_CALLBACK)( VOID* callback_arg_pt, T_IIP_RES_CONFIG_TBL* config_arg_pt );

/**
 Setting the IIP resource management table. (Rewrite table version)
@param[out]	callback_func	: Callback funtion.
@param[in]	callback_arg_pt	: Arguments of the Callback funtion.
@param[in]	wait_time		: wait time (for semaphore).
@retval		D_IIP_ERR_OK			: Normal End
@retval		D_IIP_ERR_NG			: Abnormal End
@retval		D_IIP_ERR_TMOUT		: Timeout End
@retval		D_IIP_ERR_PARAM_ERR	: Parameter Error
@remarks	
*/
extern INT32 IIP_Res_Config( const T_IIP_RES_CALLBACK callback_func, VOID* callback_arg_pt, const INT32 wait_time );


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IIP_RESMNG_SECRET_H_

