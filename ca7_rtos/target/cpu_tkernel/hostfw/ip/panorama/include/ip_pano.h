/**
 * @file		ip_pano.h
 * @brief		Panorama wrapper
 * @author		yamauchi
 * @since		2014/04/18
 * @date		
 * @note		Panorama wrapper
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#ifndef _IP_PANO_H_
#define _IP_PANO_H_

#include "ddim_typedef.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/

/**
   @name Returned value
   @{
*/
#define D_IP_PANO_OK				(0)					/**< OK */
#define D_IP_PANO_NG				(0x80190000)		/**< NG */
#define D_IP_PANO_NG_ARG			(D_IP_PANO_NG|0x00000001)	/**< Argument  Error		*/
#define D_IP_PANO_NG_OPEN			(D_IP_PANO_NG|0x00000002)	/**< Open      Error		*/
#define D_IP_PANO_NG_SEQUENCE		(D_IP_PANO_NG|0x00000004)	/**< Sequence  Error		*/
#define D_IP_PANO_NG_FORMAT			(D_IP_PANO_NG|0x00000008)	/**< Format    Error		*/
#define D_IP_PANO_NG_SIZE			(D_IP_PANO_NG|0x00000010)	/**< Size      Error		*/
#define D_IP_PANO_NG_PARAM			(D_IP_PANO_NG|0x00000020)	/**< Parameter Error		*/
#define D_IP_PANO_NG_MEMORY			(D_IP_PANO_NG|0x00000040)	/**< Memory    Error		*/
#define D_IP_PANO_NG_HW				(D_IP_PANO_NG|0x00000080)	/**< HW        Error		*/
#define D_IP_PANO_NG_UNKNOWN		(D_IP_PANO_NG|0x00008000)	/**< Unknown   Error		*/
#define D_IP_PANO_NG_LOW_CONTRAST    		(D_IP_PANO_NG|0x00001101)	/**< Not enough "features" in the scene. */
#define D_IP_PANO_NG_OUT_OF_SCENE    		(D_IP_PANO_NG|0x00001102)	/**< Image is out of scene and not good for stitching. */
#define D_IP_PANO_NG_HIGH_SPEED      		(D_IP_PANO_NG|0x00001103)	/**< The speed of panning is too high. */
#define D_IP_PANO_NG_WRONG_DIRECTION 		(D_IP_PANO_NG|0x00001104)	/**< Direction of panning is wrong. */
#define D_IP_PANO_NG_BIG_DISPLACEMENT		(D_IP_PANO_NG|0x00001105)	/**< Orthogonal offset exceeds specified in PAN_CFG::maxDisplacement. */

#define  D_IP_PANO_NG_CANCELLED             (D_IP_PANO_NG|0x00001112)  /**< Operation was cancelled. */
#define  D_IP_PANO_NG_NO_SYSTEM_RESOURCES   (D_IP_PANO_NG|0x00001113)  /**< Not enough system resources. */
#define  D_IP_PANO_NG_INVALID_ARG           (D_IP_PANO_NG|0x00001114)  /**< One of the arguments passed is invalid. */
#define  D_IP_PANO_NG_NOT_SUPPORTED         (D_IP_PANO_NG|0x00001116)  /**< Feature is not supported. */
#define  D_IP_PANO_NG_FMT_NOT_SUPPORTED     (D_IP_PANO_NG|0x00001117)  /**< Image format not supported. */
#define  D_IP_PANO_NG_NOT_IMPLEMENTED       (D_IP_PANO_NG|0x00001118)  /**< Feature is not implemented. */
#define  D_IP_PANO_NG_BAD_STRUCT_SIZE       (D_IP_PANO_NG|0x00001119)  /**< Invalid structure size. */
#define  D_IP_PANO_NG_BAD_IMAGE_SIZE        (D_IP_PANO_NG|0x0000111A)  /**< The size of the input image is not valid. */

#define D_IP_PANO_FRAME_ADDED      (D_IP_PANO_OK|0x101) /**< Frame was added. */
#define D_IP_PANO_DONE             (D_IP_PANO_OK|0x102) /**< Reached end of panorama. */


/** @} */	// name

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/
enum	E_IP_PANO_FORMAT {
	E_IP_PANO_FMT_GRAY = 0x40000001,

	E_IP_PANO_FMT_YUV_422_P2 = 0x20100001,
	E_IP_PANO_FMT_YUV_422_P3 = 0x20200001,

	E_IP_PANO_FMT_YUV_420_P2 = 0x30100001,
	E_IP_PANO_FMT_YUV_420_P3 = 0x30200001,
};


enum	E_IP_PANO_STATE {
	E_IP_PANO_SEQ_IDLE = 0,
	E_IP_PANO_SEQ_OPEN = 1,
	E_IP_PANO_SEQ_INIT = 2,
	E_IP_PANO_SEQ_CTRL = 3,
	E_IP_PANO_SEQ_PROCESS = 4,
	E_IP_PANO_SEQ_STICH = 5,
};


enum E_IP_PANO_FLAG
{
    E_IP_PANO_FLAG_LEAVE_BLACK_AREA   = (1 << 0),
    E_IP_PANO_FLAG_REALTIME_STITCH    = (1 << 1),
    E_IP_PANO_FLAG_USE_HRE            = (1 << 2),
    E_IP_PANO_FLAG_USE_GYRO           = (1 << 3)
};


enum E_IP_PANO_DIR
{
    E_IP_PANO_DIR_AUTO = 0,	//	Panning direction is determined automatically.
    E_IP_PANO_DIR_BACKWARD = 1,
    E_IP_PANO_DIR_FORWARD = 2
};


/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

typedef struct {
	INT32	width;
	INT32	height;
} T_IP_PANO_SIZE;


typedef struct {
	INT32	left;
	INT32	top;
	INT32	width;
	INT32	height;
} T_IP_PANO_RECT;


typedef struct
{
    INT32	stride;
    VOID*	data;
} T_IP_PANO_PLANE;


typedef struct 
{
    UINT32   format;    /**< Image @ref gtAPI_Fmt "color format". */
    UINT32   width;     /**< Image width in pixels. */
    UINT32   height;    /**< Image height in pixels. */
    T_IP_PANO_PLANE	planes[4]; /**< Image data planes. */
} T_IP_PANO_IMAGE;

typedef struct {
	INT32	x;
	INT32	y;
} T_IP_PANO_POINT;

typedef struct {
	INT32	dx;
	INT32	qx;
	INT32	dy;
	INT32	qy;
} T_IP_PANO_VECTOR;

typedef struct
{
    float x; /**< X-axis value */
    float y; /**< Y-axis value */
    float z; /**< Z-axis value */
} T_IP_PANO_GYRO_SAMPLE;

typedef struct
{
	UINT32			reserve0;
	UINT32			flags;
	UINT32			frameFormat;
	T_IP_PANO_SIZE	frameSize;
	UINT32			frameOrientation;
	UINT32			focalLength;
	T_IP_PANO_POINT	opticalCenter;
	UINT32			direction;
	T_IP_PANO_RECT	workCrop;
	T_IP_PANO_SIZE	workSize;
	UINT32			maxDisplacement;
	UINT32			panoramaLength;
	VOID			*panoramaBuf;
	UINT32			panoramaBufSize;
	VOID*			keyFramesBuf;
	UINT32			keyFramesBufSize;
	VOID			*transformStart;
    VOID            *transformComplete;
	VOID			*reserve1;
	UINT32			heapSize;
	VOID			*hwBuffer;
} T_IP_PANO_CONFIG;


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
extern	INT32	IP_PANO_Open(VOID *heapBuffer, T_IP_PANO_CONFIG *cfg);
extern	INT32	IP_PANO_ProcessGyro(T_IP_PANO_IMAGE *frame, UINT32 timeStamp, T_IP_PANO_GYRO_SAMPLE *sample, UINT32 isLast);
//extern	INT32	IP_PANO_ProcessImage(T_IP_PANO_IMAGE *frame, T_IP_PANO_IMAGE *workImage, T_IP_PANO_VECTOR *motion, UINT32 isLast);
extern	INT32	IP_PANO_ProcessImageStart(T_IP_PANO_IMAGE *frame, T_IP_PANO_IMAGE *workImage );
extern	INT32	IP_PANO_ProcessImage(T_IP_PANO_IMAGE *frame, T_IP_PANO_IMAGE *workImage, UINT32 isLast);
extern	INT32	IP_PANO_Stitch(T_IP_PANO_IMAGE* results);
extern	INT32	IP_PANO_Close(VOID);
extern	INT32	IP_PANO_GetBufferSize(T_IP_PANO_CONFIG *cfg);
extern	INT32	IP_PANO_GetDirection(VOID);
extern	INT32	IP_PANO_GetOffset(T_IP_PANO_SIZE *offset);
extern	VOID	IP_PANO_Get_Version(CHAR** str);

/*--------------------------------------------------------------------------------------*/
/* Version  																			*/
/*--------------------------------------------------------------------------------------*/
#define V_IP_PANO_Version		"fn_pano_version:V00L00;\r\n"

#endif	// _IP_PANO_H_

/* V00L01 2014.05.23 IP_PANO ver0.0 */

