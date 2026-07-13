/**
 * @file		ipc_pfc.h
 * @brief		Purple Fringing Correction.
 * @author		maeda
 * @since		2014/12/25
 * @date		
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2015</I></B>
 */

#ifndef _IPC_PFC_H_
#define _IPC_PFC_H_

#include "ipc_if.h"

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IPC_PFC_NG				(D_IPC_NG    |0x000D0000)	/**< NG */

/**
   @weakgroup pfc_api PFC API
   @{
*/
/**
   @name Returned value
   @{
*/
#define D_IPC_PFC_OK				(D_IPC_OK)					/**< OK */
#define D_IPC_PFC_NG_UNKNOWN		(D_IPC_PFC_NG|0x00000001)	/**< NG : Unknown */
#define D_IPC_PFC_NG_ARG			(D_IPC_PFC_NG|0x00000002)	/**< NG : Invalid arguments. */
#define D_IPC_PFC_NG_HEAP			(D_IPC_PFC_NG|0x00000003)	/**< NG : The lack of heap memory. */
#define D_IPC_PFC_NG_OPEN			(D_IPC_PFC_NG|0x00000004)	/**< NG : Cannot open. */
#define D_IPC_PFC_NG_CLOSE			(D_IPC_PFC_NG|0x00000005)	/**< NG : Cannot close. */
#define D_IPC_PFC_NG_IQ				(D_IPC_PFC_NG|0x00000006)	/**< NG : IQ param is not detected. */
#define D_IPC_PFC_NG_HW				(D_IPC_PFC_NG|0x00000007)	/**< NG : HW Error */
/** @} */	// name
/** @} */	// end of pfc_api

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Parameter of IPC_PFC_Ctrl */
typedef struct {
	IPC_ULONG	size;						/**< Set to "sizeof(T_IPC_PFC_CTRL)". */
	IPC_VOID*	pIqAdrs;					/**< The beginning address of Image Quality parameter. Should be aligned on 8. */
	IPC_VOID  	(*pCallback)( IPC_VOID );	/**< PFC processing has 4 step. It will be called when 1st step has finished. If it is set to NULL, it is NOT called in the library. */
} T_IPC_PFC_CTRL;

/*----------------------------------------------------------------------*/
/* API																	*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/**
   @weakgroup pfc_api
   @{
*/

/**
   Get version info.<br>
   @code
   	IPC_CHAR *str;
   	IPC_PFC_Get_Version( &str );
   	printf("%s\n", str);
   @endcode
   
   @param[out] str : It indicates the pointer of version info.

   @retval		D_IPC_PFC_OK			: @ref D_IPC_PFC_OK
   @retval		D_IPC_PFC_NG_ARG		: @ref D_IPC_PFC_NG_ARG
   @retval		D_IPC_PFC_NG_UNKNOWN	: @ref D_IPC_PFC_NG_UNKNOWN
   
   @remarks		Generally, it is not used.
*/
extern IPC_INT IPC_PFC_Get_Version( IPC_CHAR** str );

/**
   Open library API.<br>
   
   @retval		D_IPC_PFC_OK		: @ref D_IPC_PFC_OK
   @retval		D_IPC_PFC_NG_OPEN	: @ref D_IPC_PFC_NG_OPEN
*/
extern IPC_INT IPC_PFC_Open( IPC_VOID );

/**
   Initialize library.<br>
   
   @param[in] ptr : Reserved. Should be NULL.<br>
   
   @retval		D_IPC_PFC_OK		: @ref D_IPC_PFC_OK
*/
extern IPC_INT IPC_PFC_Init( IPC_VOID* ptr );

/**
   Configure library.<br>
   
   @param[in] ptr : Pointer of T_IPC_PFC_CTRL.<br>
   	@code
   		T_IPC_PFC_CTRL param;
   		param.size = sizeof( T_IPC_PFC_CTRL );
   		param.pIqAdrs = (IPC_VOID*)0x43000000;
   		param.pCallback = NULL;
   		IPC_PFC_Ctrl( (IPC_VOID*)&param );
   	@endcode
   
   @retval		D_IPC_PFC_OK		: @ref D_IPC_PFC_OK
   @retval		D_IPC_PFC_NG_ARG	: @ref D_IPC_PFC_NG_ARG
   @retval		D_IPC_PFC_NG_IQ		: @ref D_IPC_PFC_NG_IQ
*/
extern IPC_INT IPC_PFC_Ctrl( IPC_VOID* ptr );

/**
   Purple Fringing Correction.<br>
   For data flow, please refer to @ref pfc_overview.
   
   @param[in] 		dstImage : Destination image info. Only following members are used.<br>
   
   @code
   
   #include "ipc_if.h"
   
   T_IPC_IMAGE dstImage;
   
   // The number of overall horizontal pixel.
   // Should be aligned on 8.
   dstImage.imgInfo[1].gImgSize;
   
   // The beginning address of corrected chroma image.
   // Should be aligned on 8.
   dstImage.imgInfo[1].pImgAdrs;
   
   @endcode
   
   @param[in] 		srcImage : Source image info. Only following members are used.<br>
   
   @code
   
   #include "ipc_if.h"
   
   T_IPC_IMAGE srcImage;
   
   // Image format. Setting value is following.
   //    - E_IPC_IMAGE_FORM_422SP : 4:2:2, semi-planar.
   //    - E_IPC_IMAGE_FORM_420SP : 4:2:0, semi-planar.
   srcImage.imgForm;
   
   // Luminance info.
   //    The beginning address of image.
   //    Should be aligned on 8.
   srcImage.imgInfo[0].pImgAdrs;
   
   // Luminance info.
   //    The number of overall horizontal pixel.
   //    Should be aligned on 8.
   srcImage.imgInfo[0].gImgSize;
   
   // Luminance info.
   //    The number of horizontal pixel to be processed.
   //    Should be aligned on 8.
   srcImage.imgInfo[0].wImgSize;
   
   // Luminance info.
   //    The number of vertical pixel to be processed.
   //    Should be aligned on 4 when "srcImage.imgForm" is set to E_IPC_IMAGE_FORM_422SP.
   //    Should be aligned on 8 when "srcImage.imgForm" is set to E_IPC_IMAGE_FORM_420SP.
   srcImage.imgInfo[0].hImgSize;
   
   // Chroma info.
   //    The beginning address of image.
   //    Should be aligned on 8.
   srcImage.imgInfo[1].pImgAdrs;
   
   // Chroma info.
   //    The number of overall horizontal pixel.
   //    Should be aligned on 8.
   srcImage.imgInfo[1].gImgSize;
   
   @endcode
   
   @param[in]		heapAddr : The beginning address of heap. Should be aligned on 64.<br>
   @param[in]		heapSize : The size of heap.<br>
                               If return value of API is equal to @ref D_IPC_PFC_NG_HEAP, library returns proper value in it.<br>
   
   @retval		D_IPC_PFC_OK		: @ref D_IPC_PFC_OK
   @retval		D_IPC_PFC_NG_ARG	: @ref D_IPC_PFC_NG_ARG
   @retval		D_IPC_PFC_NG_HEAP	: @ref D_IPC_PFC_NG_HEAP
   @retval		D_IPC_PFC_NG_HW		: @ref D_IPC_PFC_NG_HW
*/
extern IPC_INT IPC_PFC_Main(
							T_IPC_IMAGE* 	dstImage,
							T_IPC_IMAGE* 	srcImage,
							IPC_ULONG		heapAddr,
							IPC_ULONG*		heapSize
);

/**
   Close library API.<br>
   
   @retval		D_IPC_PFC_OK		: @ref D_IPC_PFC_OK
   @retval		D_IPC_PFC_NG_CLOSE	: @ref D_IPC_PFC_NG_CLOSE
*/
extern IPC_INT IPC_PFC_Close( IPC_VOID );

/** @} */	// end of pfc_api

#ifdef __cplusplus
}
#endif	// __cplusplus





/**
   @weakgroup pfc_usage Usage
   @{
   
   @image html pfc_main.png
   
@code

#include "ipc_pfc.h"

void PFC_Sample( void )
{
   IPC_ULONG	  	iqAdrs = 0x41000000;
   T_IPC_PFC_CTRL 	param;
   T_IPC_IMAGE		dstImage;
   T_IPC_IMAGE		srcImage;
   IPC_ULONG		heapAddr;
   IPC_ULONG		heapSize;
   static IPC_INT	flag = 1;
   
   
   // Download Image Quality parameter to RAM.
   Download_IQ_Param( iqAdrs );
   
   
   IPC_PFC_Open();
   
   if ( flag ) {
   	IPC_PFC_Init( NULL );	// Call it at least once.
   	flag = 0;
   }
   
   param.size    = sizeof( T_IPC_PFC_CTRL );
   param.pIqAdrs = (IPC_VOID*)iqAdrs;
   param.pCallback = NULL;
   IPC_PFC_Ctrl( (IPC_VOID*)&param );
   
   srcImage.imgForm = E_IPC_IMAGE_FORM_422SP;
   srcImage.imgInfo[0].pImgAdrs = (IPC_VOID*)0x42000000;
   srcImage.imgInfo[0].gImgSize = 4608;
   srcImage.imgInfo[0].wImgSize = 4608;
   srcImage.imgInfo[0].hImgSize = 3456;
   srcImage.imgInfo[1].pImgAdrs = (IPC_VOID*)0x42000000 + (4608*3456);
   srcImage.imgInfo[1].gImgSize = 4608;
   dstImage.imgInfo[1].pImgAdrs = srcImage.imgInfo[1].pImgAdrs;	// Library can overwrite corrected image.
   dstImage.imgInfo[1].gImgSize = 4608;
   heapAddr = 0x52000000;
#if 1
   heapSize = 7*1024*1024;
#else
   // note: User can get the size of required heap memory as followings:
   //       Its size depends on the input image size.
   heapSize = 0;
   IPC_PFC_Main( &dstImage, 
                 &srcImage, 
                 heapAddr, 
                 &heapSize );	// Library sets the value of the required heap memory.
#endif
   IPC_PFC_Main( &dstImage, 
                 &srcImage, 
                 heapAddr, 
                 &heapSize );
   
   IPC_PFC_Close();
}
@endcode
   
*/
/** @} */	// end of pfc_usage

/**
   @weakgroup pfc_overview Overview
   @{
   This function reduces the purple fringing, caused by the axial chromatic aberration.<br>
   
   The following figure shows data flow.<br>
   @image html pfc_overview.png
   Input data:<br>
   <ul>
    <li> Input image : Only 8 bits/pixel. Only chroma image is corrected.
    <li> Heap buffer : It is used in PFC process.
    <li> IQ Param (Image Quality Parameters) : Image quality depends on this parameters. It is created by the tool.
   </ul>
*/
/** @} */	// end of pfc_overview

/**
   @weakgroup pfc_lib Library Dependency
   @{
   PFC needs following libraries.<br>
   <ul>
    <li> ipc_pfc.a
    <li> ipc_iip.a
    <li> MILB_API.a
    <li> MILB.a
   </ul>
   
   @image html pfc_library.png
*/
/** @} */	// end of pfc_lib

#endif	// _IPC_PFC_H_
