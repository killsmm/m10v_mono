/**
 * @file		iq_common.h
 * @brief		Common definition/macro/function for Image Quality functions.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_COMMON_H__
#define __IQ_COMMON_H__

// Driver Header
#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

//#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__LITTLE_ENDIAN__)
#define CO_BF_IQ_LITTLE_ENDIAN
//#endif //(__CYGWIN__ || __MINGW32__ || __LITTLE_ENDIAN__)

#define M_IQ_ROUNDUP_2(val)				((((val) + 1) / 2) * 2)
#define M_IQ_ROUNDUP_4(val)				((((val) + 3)/4) *4)
#define M_IQ_ROUNDUP_8(val)				((((val) + 7) / 8) * 8)
#define M_IQ_ROUNDUP_16(val)			((((val) + 15) / 16) * 16)
#define M_IQ_ROUNDUP_32(val)			((((val) + 31) / 32) * 32)
#define M_IQ_ROUNDUP_64(val)			((((val) + 63) / 64) * 64)
#define M_IQ_ROUNDUP_512(val)			((((val) + 511) / 512) * 512)
#define M_IQ_ROUNDUP_N(val, n)			((((val) + (n) - 1) / (n)) * (n))

#define M_IQ_ROUNDDOWN_2(a)			(((a) >> 1) << 1)				// 2 byte alignment
#define M_IQ_ROUNDDOWN_8(a)			(((a) >> 3) << 3)				// 8 byte alignment
#define M_IQ_ROUNDDOWN_16(a)		(((a) >> 4) << 4)				// 16 byte alignment
#define M_IQ_ROUNDDOWN_64(a)		(((a) >> 6) << 6)				// 64 byte alignment
#define M_IQ_ROUNDDOWN_N(val, n)	(((val) / (n)) * (n))

#define M_IQ_MIN( a, b )    ( ((a) > (b))?(b):(a) )
#define M_IQ_MAX( a, b )    ( ((a) > (b))?(a):(b) )

#define ABS(val)			( (((INT32)(val)) >= 0)?(val):(-(val)) )

/** endian convert macro.(for on PC debugging) */
#ifdef CO_BF_IQ_LITTLE_ENDIAN
/* on PC */
#  define IQ_HTONS(us_val)  ( (((us_val)&0xFF00) >>8) | (((us_val)&0x00FF) <<8) )
#  define IQ_HTONL(ui_val)  ( (((ui_val)&0xFF000000) >>24) | (((ui_val)&0x00FF0000) >>8) | (((ui_val)&0x0000FF00) <<8) | (((ui_val)&0x000000FF) <<24) )
#  define IQ_NTOHS(us_val)  (IQ_HTONS(us_val))
#  define IQ_NTOHL(ui_val)  (IQ_HTONL(ui_val))

#  define M_IQBIN_HTONS(us_val)		(us_val)
#  define M_IQBIN_HTONL(ui_val)		(ui_val)
#  define M_IQBIN_HTONLL(ull_val)	(ull_val)
#  define M_IQBIN_NTOHS(us_val)		(us_val)
#  define M_IQBIN_NTOHL(ui_val)		(ui_val)
#  define M_IQBIN_NTOHLL(ull_val)	(ull_val)
#else //!CO_BF_IQ_LITTLE_ENDIAN
/* on DSC */
#  define IQ_HTONS(us_val)  (us_val)
#  define IQ_HTONL(ui_val)  (ui_val)
#  define IQ_NTOHS(us_val)  (us_val)
#  define IQ_NTOHL(ui_val)  (ui_val)

#  define M_IQBIN_HTONS(us_val)		(us_val)
#  define M_IQBIN_HTONL(ui_val)		(ui_val)
#  define M_IQBIN_HTONLL(ull_val)	(ull_val)
#  define M_IQBIN_NTOHS(us_val)		(us_val)
#  define M_IQBIN_NTOHL(ui_val)		(ui_val)
#  define M_IQBIN_NTOHLL(ull_val)	(ull_val)
#endif //!CO_BF_IQ_LITTLE_ENDIAN


// PRch burst-read align.
#define D_IQ_PRCH_BURST_BYTES_MIN					(64)
#define M_IQ_ROUNDUP_PRCH_BURST_BYTES_MIN( a )		M_IQ_ROUNDUP_64( (a) )
#define M_IQ_ROUNDUP_PRCH_BURST_BYTES_NORM( a )		M_IQ_ROUNDUP_512( (a) )

#ifdef CO_DEBUG_ON_PC
#define BF_IQ_DebugPrint( args )			printf args
#endif
#define BF_IQ_TermPrint( args )				printf args


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

typedef enum {
	E_BF_IQ_FIELD_EVEN =0,
	E_BF_IQ_FIELD_ODD,
	E_BF_IQ_FIELD_MAX,
} E_BF_IQ_FIELD;


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


extern USHORT BF_IQ_Dpc_Calc_Sta_y_Progr2Intr( const E_BF_IQ_FIELD odd_even, const ULONG progressive_sta_y );
extern USHORT BF_IQ_Dpc_Calc_Lines_Progr2Intr( const E_BF_IQ_FIELD odd_even, const ULONG progressive_sta_y, ULONG progressive_lines );


#ifdef __cplusplus
}
#endif

#endif /* __FJ_COMMON_H__ */

