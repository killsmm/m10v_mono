/**
 * @file		ip_shdr.h
 * @brief		Header file of wrapper layer (SHDR Library)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _IP_SHDR_H
#define	_IP_SHDR_H

#include "fj_still.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_shdr_overview
@{*/

/*@} --- end of ip_shdr_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_shdr_sequence
@{*/

/*@} --- end of ip_shdr_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_shdr_definition
@{*/
#define D_IP_SHDR_OK			(0)				/**< Success.			*/
#define D_IP_SHDR_NG			(-1)			/**< Error. 			*/

///////////////////////////////////////////////////////////////////////////////
// structure
///////////////////////////////////////////////////////////////////////////////
/** SHDR initialize parameter. */
typedef struct {
	UINT32						heapAddr;				/**< Heap address.			*/
	ULONG						heapSize;				/**< Heap size.				*/
	FJ_STILL_IP_IMG_INFO_PRM	imgInfoPrm;				/**< Image information.		*/
} IP_SHDR_INIT_PRM;

/** SHDR setting parameter. */
typedef struct {
	VOID*						rsv0;					/**< reserved parameter.	*/
} IP_SHDR_SET_PRM;

/** SHDR start parameter. */
typedef struct {
	FJ_STILL_IP_IMAGE			imgSrcL[ 2 ];			/**< [In]Source image (YUV8 Long exposure).<br>
															<ul>
																<li>[0]:YUV8   from SGDE/R2Y.
																<li>[1]:Y8 1/4 from SGDE/R2Y.
															</ul>
														*/
	FJ_STILL_IP_IMAGE			imgSrcM[ 2 ];			/**< [In]Source image (YUV8 Middle exposure).<br>
															<ul>
																<li>[0]:YUV8   from SGDE/R2Y.
																<li>[1]:Y8 1/4 from SGDE/R2Y.
															</ul>
														*/
	FJ_STILL_IP_IMAGE			imgSrcS[ 2 ];			/**< [In]Source image (YUV8 Short exposure).<br>
															<ul>
																<li>[0]:YUV8   from SGDE/R2Y.
																<li>[1]:Y8 1/4 from SGDE/R2Y.
															</ul>
														*/
	FJ_STILL_IP_IMAGE			imgDst;					/**< [Out]Destination image(RGB16).			*/
} IP_SHDR_START_PRM;
/*@} --- end of ip_shdr_definition */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/** @weakgroup ip_shdr_api
@{*/

/**
Open SHDR.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
extern INT32 IP_SHDR_Open( VOID );

/**
Initialize SHDR.
@param[in]	iniPrm	: SHDR initialize parameter.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
extern INT32 IP_SHDR_Init( IP_SHDR_INIT_PRM* iniPrm );

/**
Set SHDR.
@param[in]	setPrm	: SHDR setting parameter.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
extern INT32 IP_SHDR_Set( IP_SHDR_SET_PRM* setPrm );

/**
Start SHDR.
@param[in]	startPrm	: SHDR start parameter.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
extern INT32 IP_SHDR_Start( IP_SHDR_START_PRM* startPrm );

/**
Close SHDR.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
extern INT32 IP_SHDR_Close( VOID );

/**
Get SHDR version.
@param[out]	str	: version.
*/
extern VOID IP_SHDR_Get_Version( CHAR* str );

/*@} --- end of ip_shdr_api */

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IP_SHDR_H
