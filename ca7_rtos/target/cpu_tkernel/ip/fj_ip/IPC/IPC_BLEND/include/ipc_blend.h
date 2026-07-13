/**
 * @file		ipc_blend.h
 * @brief		Blending.
 * @author		hattori
 * @since		2016/12/02
 * @date		2016/12/05
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2015</I></B>
 */

#ifndef _IPC_BLEND_H_
#define _IPC_BLEND_H_

#include "ipc_if.h"

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IPC_BLEND_OK				(D_IPC_OK)					/**< OK */
#define D_IPC_BLEND_NG				(D_IPC_NG    |0x000D0000)	/**< NG */

#define D_IPC_BLEND_NG_OPEN			(D_IPC_BLEND_NG|0x00000001)	/**< NG : Cannot open. */
#define D_IPC_BLEND_NG_CLOSE		(D_IPC_BLEND_NG|0x00000002)	/**< NG : Cannot close. */
#define D_IPC_BLEND_NG_ARG			(D_IPC_BLEND_NG|0x00000003)	/**< NG : Invalid arguments. */
#define D_IPC_BLEND_NG_HW			(D_IPC_BLEND_NG|0x00000004)	/**< NG : HW Error */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* API																	*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/**
   Get version info.
   
   @param[out] str : It indicates the pointer of version info.

   @retval		D_IPC_BLEND_OK			: @ref D_IPC_BLEND_OK
   @retval		D_IPC_BLEND_NG_ARG		: @ref D_IPC_BLEND_NG_ARG
   @retval		D_IPC_BLEND_NG_UNKNOWN	: @ref D_IPC_BLEND_NG_UNKNOWN
   
   @remarks		Generally, it is not used.
*/
extern IPC_INT IPC_BLEND_Get_Version( IPC_CHAR** str );

/**
   Open library API.<br>
   
   @retval		D_IPC_BLEND_OK		: @ref D_BLEND_BLEND_OK
   @retval		D_IPC_BLEND_NG_OPEN	: @ref D_BLEND_BLEND_NG_OPEN
*/
//extern IPC_INT IPC_BLEND_Open( IPC_VOID );

/**
   Initialize library.<br>
   
   @param[in] ptr : Reserved. Should be NULL.<br>
   
   @retval		D_IPC_BLEND_OK		: @ref D_IPC_BLEND_OK
*/
//extern IPC_INT IPC_BLEND_Init( IPC_VOID* ptr );

/**
   Blend rectangle.

   @endcode
   
   @param[in]		out : Image Info for Output image.<br>
   @param[in]		in0 : Image Info for Input 0 imahe.<br>
   @param[in]		in1 : Image Info for Input 1 imahe.<br>

   @retval		none
*/
extern IPC_VOID IPC_BLEND_Blend(
							T_IPC_IMAGE* 	out,
							T_IPC_IMAGE* 	in0,
							T_IPC_IMAGE* 	in1,
							T_IPC_IMAGE* 	alp
);

/**
   Copy rectangle.

   @endcode
   
   @param[in]		out : Image Info for Output image.<br>
   @param[in]		in  : Image Info for Input  imahe.<br>

   @retval		none
*/
extern IPC_VOID IPC_BLEND_Copy(
							T_IPC_IMAGE* 	out,
							T_IPC_IMAGE* 	in0
);

/**
   Close library API.<br>
   
   @retval		D_IPC_BLEND_OK		: @ref D_IPC_BLEND_OK
   @retval		D_IPC_BLEND_NG_CLOSE	: @ref D_IPC_BLEND_NG_CLOSE
*/
//extern IPC_INT IPC_BLEND_Close( IPC_VOID );


#endif	// _IPC_BLEND_H_
