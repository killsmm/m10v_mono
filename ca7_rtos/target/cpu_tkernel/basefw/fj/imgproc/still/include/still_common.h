/**
 * @file		still_common.h
 * @brief		common define for still
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_COMMON_H_
#define _STILL_COMMON_H_

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_BF_STILL_VIDEOFMT_LINES			32
#define D_BF_STILL_VIDEOFMT_YC_LINES		64
#define D_BF_STILL_VIDEOFMT_WIDTH			256
#define D_BF_STILL_VIDEOFMT_YC_WIDTH		256


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/

#define M_BF_STILL_ROUNDUP_2( val )	(((UINT32)(val)  +1) & 0xFFFFFFFE)
#define M_BF_STILL_ROUNDUP_4( val )	(((UINT32)(val)  +3) & 0xFFFFFFFC)
#define M_BF_STILL_ROUNDUP_8( val )	(((UINT32)(val)  +7) & 0xFFFFFFF8)
#define M_BF_STILL_ROUNDUP_16( val )	(((UINT32)(val) +15) & 0xFFFFFFF0)
#define M_BF_STILL_ROUNDUP_32( val )	(((UINT32)(val) +31) & 0xFFFFFFE0)
#define M_BF_STILL_ROUNDUP_64( val )	(((UINT32)(val) +63) & 0xFFFFFFC0)
#define M_BF_STILL_ROUNDUP_128( val )	(((UINT32)(val) +127) & 0xFFFFFF80)
#define M_BF_STILL_ROUNDUP_256( val )	(((UINT32)(val) +255) & 0xFFFFFF00)
#define M_BF_STILL_ROUNDUP_512( val )	(((UINT32)(val) +511) & 0xFFFFFE00)
#define M_BF_STILL_ROUNDUP_16384( val )	(((UINT32)(val) +(16384 -1)) & (~(16384 -1)))
#define M_BF_STILL_ROUNDUP_N( val, n )	((((val) + (n) - 1) / (n)) * (n))
#define M_BF_STILL_ROUNDDOWN_2( val )	(((UINT32)(val)) & 0xFFFFFFFE)
#define M_BF_STILL_ROUNDDOWN_4( val )	(((UINT32)(val)) & 0xFFFFFFFC)
#define M_BF_STILL_ROUNDDOWN_8( val )	(((UINT32)(val)) & 0xFFFFFFF8)
#define M_BF_STILL_ROUNDDOWN_32( val )	(((UINT32)(val)) & 0xFFFFFFE0)
#define M_BF_STILL_ROUNDDOWN_64( val )	(((UINT32)(val)) & 0xFFFFFFC0)
#define M_BF_STILL_ROUNDDOWN_128( val )	(((UINT32)(val)) & 0xFFFFFF80)

#define M_BF_STILL_ROUNDUP_VIDEOFMT_YC_WIDTH( val )	M_BF_STILL_ROUNDUP_256( (val) )
#define M_BF_STILL_ROUNDUP_VIDEOFMT_LINES( val )	M_BF_STILL_ROUNDUP_32( (val) )
#define M_BF_STILL_ROUNDUP_VIDEOFMT_YC_LINES( val )	M_BF_STILL_ROUNDUP_64( (val) )
#define M_BF_STILL_ROUNDUP_VIDEOFMT_YC_BYTES( val )	M_BF_STILL_ROUNDUP_16384( (val) )

#define M_BF_STILL_MAX( a, b )			( ( (a) > (b) )?(a):(b) )
#define M_BF_STILL_MIN( a, b )			( ( (a) > (b) )?(b):(a) )

#define IS_FLGPTN_EQUAL(flgptn, FLG) ((((flgptn) & (FLG)) == (FLG)) ? TRUE : FALSE)


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

typedef enum {
	E_BF_STILL_ONOFF_OFF = 0,
	E_BF_STILL_ONOFF_ON,
} E_BF_STILL_ONOFF;

// E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS
typedef enum {
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_DIRECT_SRO2CNR = 0,
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_DIRECT_SRO2R2Y,
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_DIRECT_SRO2LTM,
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_DIRECT_SRO2B2R,
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_DIRECT_SRO2B2B,
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_DIRECT_SRO,
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_INDIRECT,
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_MIX,					// direct & indirect(for 4sensor)
	E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS_MAX
} E_BF_STILL_COMMON_IMAGEPIPE_CONNECT_STATUS;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

typedef struct {
	UCHAR	sen_id;
	UCHAR	demosaic_id;
	UINT32	bayer_resize_rate;		// 1 / 65536 (0x00010000 = 1.0)
	UINT32	r2y_resize_rate;		// 1 / 65536 (0x00010000 = 1.0)  please set 0x00010000
	UCHAR	r2y_rdc_rate;			// 2^(r2y_rdc_rate+1) 0~3  please set 0
} T_BF_STILL_COMMON_MARGIN_INPUT_PARAM;

typedef struct {
	UCHAR	sro_margin;
	UCHAR	b2b_margin;
	UCHAR	b2r_margin;
	UCHAR	r2y_margin;
	UCHAR	cnr_margin;
} T_BF_STILL_COMMON_MARGIN_INFO;

/*==============================================================================*/
/* Global variable declaration													*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif	// _STILL_COMMON_H_

