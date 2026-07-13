/**
 * @file		ipc_ge.h
 * @brief		SGDE wrapper
 * @author		yamauchi
 * @since		2016/05/18
 * @date		2016/06/02
 * @note		SGDE wrapper
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#ifndef _IP_GE_H_
#define _IP_GE_H_

#include "ddim_typedef.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/

/**
   @name Returned value
   @{
*/
#define D_IP_GE_OK					(0)							/**< OK */
#define D_IP_GE_NG					(0x80190000)				/**< NG */
#define D_IP_GE_NG_ARG				(D_IP_GE_NG|0x00000001)	/**< Argument  Error		*/
#define D_IP_GE_NG_OPEN				(D_IP_GE_NG|0x00000002)	/**< Open      Error		*/
#define D_IP_GE_NG_SEQ				(D_IP_GE_NG|0x00000003)	/**< Sequence  Error		*/
#define D_IP_GE_NG_FORMAT			(D_IP_GE_NG|0x00000004)	/**< Format    Error		*/
#define D_IP_GE_NG_SIZE				(D_IP_GE_NG|0x00000005)	/**< Size      Error		*/
#define D_IP_GE_NG_PARAM			(D_IP_GE_NG|0x00000006)	/**< Parameter Error		*/
#define D_IP_GE_NG_MEMORY			(D_IP_GE_NG|0x00000007)	/**< Memory    Error		*/
#define D_IP_GE_NG_HW				(D_IP_GE_NG|0x00000008)	/**< HW        Error		*/
#define D_IP_GE_NG_INVALID_MODE		(D_IP_GE_NG|0x00000009)
#define D_IP_GE_NG_BUFFER_TOO_SMALL	(D_IP_GE_NG|0x0000000A)
#define D_IP_GE_NG_BUSY            	(D_IP_GE_NG|0x0000000B)
#define D_IP_GE_NG_STOPPED         	(D_IP_GE_NG|0x0000000C)
#define D_IP_GE_NG_WAIT            	(D_IP_GE_NG|0x0000000D)
#define D_IP_GE_NG_UNKNOWN			(D_IP_GE_NG|0x000000FF)	/**< Unknown   Error		*/

#define  D_IP_GE_NG_FAIL                  (D_IP_GE_NG|0x00002001)  /**< Operation was cancelled. */
#define  D_IP_GE_NG_CANCELLED             (D_IP_GE_NG|0x00002002)  /**< Operation was cancelled. */
#define  D_IP_GE_NG_NO_SYSTEM_RESOURCES   (D_IP_GE_NG|0x00002003)  /**< Not enough system resources. */
#define  D_IP_GE_NG_INVALID_ARG           (D_IP_GE_NG|0x00002004)  /**< One of the arguments passed is invalid. */
#define  D_IP_GE_NG_NOT_SUPPORTED         (D_IP_GE_NG|0x00002006)  /**< Feature is not supported. */
#define  D_IP_GE_NG_FMT_NOT_SUPPORTED     (D_IP_GE_NG|0x00002007)  /**< Image format not supported. */
#define  D_IP_GE_NG_NOT_IMPLEMENTED       (D_IP_GE_NG|0x00002008)  /**< Feature is not implemented. */
#define  D_IP_GE_NG_BAD_STRUCT_SIZE       (D_IP_GE_NG|0x00002009)  /**< Invalid structure size. */
#define  D_IP_GE_NG_BAD_IMAGE_SIZE        (D_IP_GE_NG|0x0000200A)  /**< The size of the input image is not valid. */

#define  D_IP_GE_NG_SEQUENCE              (D_IP_GE_NG|0x00000010)  /**< The size of the input image is not valid. */

#define D_IP_GE_IN_PROGRESS      (D_IP_OK|0x00190000|0x100)				/**< More processing required. */
#define D_IP_GE_GRID_NOT_READY   (D_IP_GE_NG|0x00001201)	/**< The correction grid is not ready. */


/** @} */	// name

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/
enum	E_IP_GE_STATE {
	E_IP_GE_SEQ_IDLE = 0,
	E_IP_GE_SEQ_OPEN = 1,
	E_IP_GE_SEQ_INIT = 2,
	E_IP_GE_SEQ_CTRL = 3,
	E_IP_GE_SEQ_PROCESS = 4,
	E_IP_GE_SEQ_STICH = 5,
};

enum E_IP_GE_FLAGS
{
    IP_GE_LOW_LATENCY  = (1 << 0),
    IP_GE_DYN_GGRID    = (1 << 1),
};
/*
typedef	enum	{
	E_IP_GE_FMT_YUV_444    = 0x10000001,
	E_IP_GE_FMT_YUV_444_P2 = 0x10100001,
	E_IP_GE_FMT_YUV_444_P3 = 0x10200001,
	E_IP_GE_FMT_YUV_422_P2 = 0x20100001,
	E_IP_GE_FMT_YUV_422_P3 = 0x20200001,
	E_IP_GE_FMT_YUV_420_P2 = 0x30100001,
	E_IP_GE_FMT_YUV_420_P3 = 0x30200001
} E_IP_GE_FORMAT;
*/
typedef	enum	{
	E_IP_GE_FMT_YUV_420_P3 = 0,
	E_IP_GE_FMT_YUV_422_P3,
	E_IP_GE_FMT_YUV_444_P3,
	E_IP_GE_FMT_YUV_420_P2,
	E_IP_GE_FMT_YUV_422_P2,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_IP_GE_FMT_YUV_420_B
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
} E_IP_GE_FORMAT;



/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

typedef struct
{
    INT32	stride;
    VOID*	data;
} T_IP_GE_PLANE;


typedef struct 
{
    UINT32			format;
    UINT32			width; 
    UINT32			height;
    T_IP_GE_PLANE	planes[4];
} T_IP_GE_IMAGE;


typedef struct
{
    UINT32 flags;
    BYTE  fill[3];
    UINT32 split_y;
} T_IP_GE_CONFIG;


typedef struct
{
    FLOAT  u;
    FLOAT  v;
} T_IP_GE_LNODE;


typedef struct
{
    FLOAT   a[9];
    UINT32	next;
} T_IP_GE_XFORM;


typedef struct
{
    UINT32  width;
    UINT32  height;
    BYTE pitchX;
    BYTE pitchY;
    INT32   outputX;
    INT32   outputY;
    T_IP_GE_LNODE *nodes;
    T_IP_GE_XFORM *xform;
} T_IP_GE_LGRID;


typedef struct
{
	FLOAT	u;
	FLOAT	v;
} T_IP_GE_GNODE;

typedef struct
{
    UINT32  width;
    UINT32  height;
    UINT32  pitchX;
    UINT32  pitchY;
    INT32   offsetX;
    INT32   offsetY;
    T_IP_GE_GNODE *nodes;
} T_IP_GE_GGRID;


/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function  																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API		  																			*/
/*--------------------------------------------------------------------------------------*/

extern	INT32	IP_GE_Open(T_IP_GE_CONFIG *config, VOID *buffer, INT32 size);
extern	INT32	IP_GE_Close( VOID );
extern	INT32	IP_GE_SetCorrectParameter( T_IP_GE_GGRID *global, UINT32 numLocal, T_IP_GE_LGRID *local, FLOAT xform[9]);
extern	INT32	IP_GE_SetGridData( UINT32 num, T_IP_GE_GNODE *nodes );
extern	INT32	IP_GE_Process( T_IP_GE_IMAGE *in, T_IP_GE_IMAGE *out1, T_IP_GE_IMAGE *out2, T_IP_GE_IMAGE *out3, INT32 wait );
extern  INT32   IP_GE_Wait( VOID );
extern	INT32	IP_GE_MIOProcess( T_IP_GE_IMAGE *in, T_IP_GE_IMAGE *out1, T_IP_GE_IMAGE *out2, T_IP_GE_IMAGE *out3 );
extern	VOID	IP_GE_L1l2cache_Clean_Flush_Addr( UINT32 address, UINT32 size );

extern	VOID	IP_GE_Get_Version(BYTE** str);

/*--------------------------------------------------------------------------------------*/
/* Version  																			*/
/*--------------------------------------------------------------------------------------*/

#endif	// _IP_GE_H_

/* V00L01 2014.05.23 IP_EIS ver0.0 */

