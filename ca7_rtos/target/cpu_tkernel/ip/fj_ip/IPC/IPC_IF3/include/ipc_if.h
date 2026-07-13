/**
 * @file		ipc_if.h
 * @brief		IPC common I/F
 * @author		hata
 * @since		2014/04/20
 * date			2016/06/10
 * @note		None
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2015-2016</I></B>
 */

#ifndef _IPC_IF_H_
#define _IPC_IF_H_

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
typedef void				IPC_VOID;
typedef signed char			IPC_CHAR;
typedef unsigned char		IPC_UCHAR;
typedef signed short		IPC_SHORT;
typedef unsigned short		IPC_USHORT;
typedef signed int			IPC_INT;
typedef unsigned int		IPC_UINT;
typedef signed long			IPC_LONG;
typedef unsigned long		IPC_ULONG;
typedef float				IPC_FLOAT;
typedef double				IPC_DOUBLE;
typedef unsigned int		IPC_BOOL;
typedef long long			IPC_LLONG;

#define D_IPC_OK			0x00000000
#define D_IPC_NG			0x80000000

#define D_IPC_TRUE			0x1
#define D_IPC_FALSE			0x0

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/
typedef enum {
	E_IPC_IMAGE_FORM_420P	= 0,					/* YcbCr420面順次					*/
	E_IPC_IMAGE_FORM_422P	= 1,					/*      422面順次					*/
	E_IPC_IMAGE_FORM_444P	= 2,					/*      444面順次					*/
	E_IPC_IMAGE_FORM_420SP	= 3,					/* YCbCr420面点順次					*/
	E_IPC_IMAGE_FORM_422SP	= 4,					/*      422面点順次					*/
	E_IPC_IMAGE_FORM_444SP	= 5,					/*      444面点順次					*/
	E_IPC_IMAGE_FORM_GRAY	= 6,					/* Y(1面)							*/
	E_IPC_IMAGE_FORM_RGB	= 7,					/* RGB(面/点,並び順は各IP毎)		*/
	E_IPC_IMAGE_FORM_BAY	= 8,					/* Bayer配列						*/
	E_IPC_IMAGE_FORM_CBCR_P	= 9,					/* CbCr面順次						*/
	E_IPC_IMAGE_FORM_CBCR_C = 10,					/*     点順次						*/
	E_IPC_IMAGE_FORM_RGB_P	= 11,					/* RGB面順次(並び順は各IP毎)		*/
	E_IPC_IMAGE_FORM_RGB_C	= 12					/* RGB点順次(並び順は各IP毎)		*/
} E_IPC_IMAGE_FORM;

typedef enum {
	E_IPC_IMAGE_DATA_FORM_PACK		= 0,
	E_IPC_IMAGE_DATA_FORM_UNPACK	= 1
} E_IPC_IMAGE_DATA_FORM;

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/
typedef struct {
	IPC_VOID*				pImgAdrs;
	IPC_UINT				gImgSize;
	IPC_UINT				wImgSize;
	IPC_UINT				hImgSize;
} T_IPC_IMAGE_INFO;

typedef struct {
	E_IPC_IMAGE_FORM		imgForm;
	T_IPC_IMAGE_INFO		imgInfo[3];
	IPC_UINT				bitDepth;
	E_IPC_IMAGE_DATA_FORM	dataForm;
} T_IPC_IMAGE;

typedef struct {
	IPC_VOID*				pImgAdrs;
	IPC_UINT				gImgSize;
	IPC_UINT				wImgSize;
	IPC_UINT				hImgSize;
	IPC_UINT				bitDepth;
	E_IPC_IMAGE_DATA_FORM	dataForm;
} T_IPC_IMAGE_PLANE;

typedef struct {
	IPC_INT					left;
	IPC_INT					top;
	IPC_UINT				width;
	IPC_UINT				height;
} T_IPC_RECT_INFO;

typedef struct {
	IPC_ULONG				en;
	IPC_ULONG				data[4];
	T_IPC_RECT_INFO			rect[32];
} T_IPC_RECT;

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API		  																			*/
/*--------------------------------------------------------------------------------------*/

#endif

/* V00L01 2014.04.25 IPC_IF2 ver0.1 */
/* V01L00 2014.05.30 IPC_IF2 ver1.0 */
/* V01L01 2014.08.11 IPC_IF2 ver1.1 */
/* Socionext Inc. */
/* V01L01 2015.03.17 IPC_IF2 ver1.1 */
/* V01L01 2016.06.10 IPC_IF3 ver3.0 */

