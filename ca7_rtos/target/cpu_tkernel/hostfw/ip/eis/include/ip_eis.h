/**
 * @file		ip_eis.h
 * @brief		EIS wrapper
 * @author		yamauchi
 * @since		2016/04/15
 * @date		2017/03/23
 * @note		EIS wrapper
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#ifndef _IP_EIS_H_
#define _IP_EIS_H_

#include "ddim_typedef.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/

/**
   @name Returned value
   @{
*/
#define D_IP_EIS_OK				(0)					/**< OK */
#define D_IP_EIS_NG				(0x801A0000)		/**< NG */
#define D_IP_EIS_NG_ARG			(D_IP_EIS_NG|0x00000001)	/**< Argument  Error		*/
#define D_IP_EIS_NG_OPEN		(D_IP_EIS_NG|0x00000002)	/**< Open      Error		*/
#define D_IP_EIS_NG_SEQUENCE	(D_IP_EIS_NG|0x00000004)	/**< Sequence  Error		*/
#define D_IP_EIS_NG_FORMAT		(D_IP_EIS_NG|0x00000008)	/**< Format    Error		*/
#define D_IP_EIS_NG_SIZE		(D_IP_EIS_NG|0x00000010)	/**< Size      Error		*/
#define D_IP_EIS_NG_PARAM		(D_IP_EIS_NG|0x00000020)	/**< Parameter Error		*/
#define D_IP_EIS_NG_MEMORY		(D_IP_EIS_NG|0x00000040)	/**< Memory    Error		*/
#define D_IP_EIS_NG_HW			(D_IP_EIS_NG|0x00000080)	/**< HW        Error		*/
#define D_IP_EIS_NG_UNKNOWN		(D_IP_EIS_NG|0x00008000)	/**< Unknown   Error		*/

#define  D_IP_EIS_NG_FAIL                  (D_IP_EIS_NG|0x00002001)  /**< Operation was cancelled. */
#define  D_IP_EIS_NG_CANCELLED             (D_IP_EIS_NG|0x00002002)  /**< Operation was cancelled. */
#define  D_IP_EIS_NG_NO_SYSTEM_RESOURCES   (D_IP_EIS_NG|0x00002003)  /**< Not enough system resources. */
#define  D_IP_EIS_NG_INVALID_ARG           (D_IP_EIS_NG|0x00002004)  /**< One of the arguments passed is invalid. */
#define  D_IP_EIS_NG_NOT_SUPPORTED         (D_IP_EIS_NG|0x00002006)  /**< Feature is not supported. */
#define  D_IP_EIS_NG_FMT_NOT_SUPPORTED     (D_IP_EIS_NG|0x00002007)  /**< Image format not supported. */
#define  D_IP_EIS_NG_NOT_IMPLEMENTED       (D_IP_EIS_NG|0x00002008)  /**< Feature is not implemented. */
#define  D_IP_EIS_NG_BAD_STRUCT_SIZE       (D_IP_EIS_NG|0x00002009)  /**< Invalid structure size. */
#define  D_IP_EIS_NG_BAD_IMAGE_SIZE        (D_IP_EIS_NG|0x0000200A)  /**< The size of the input image is not valid. */

#define D_IP_EIS_IN_PROGRESS      (D_IP_EIS_OK|0x001A0000|0x100)				/**< More processing required. */
#define D_IP_EIS_GRID_NOT_READY   (D_IP_EIS_NG|0x00001201)	/**< The correction grid is not ready. */


/** @} */	// name

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/
enum E_IP_EIS_FLAGS
{
    E_IP_EIS_FLAG_USE_GYRO          = (1 << 0),
    E_IP_EIS_FLAG_USE_ACCELERATION  = (1 << 1),
    E_IP_EIS_FLAG_USE_MAGNET        = (1 << 2),
    E_IP_EIS_FLAG_USE_MOTION        = (1 << 3),
    E_IP_EIS_FLAG_USE_IMAGE         = (1 << 4),
    E_IP_EIS_FLAG_LOW_LATENCY_MODE  = (1 << 5),
//    E_IP_EIS_FLAG_FORWARD_GRID      = (1 << 6),
    E_IP_EIS_FLAG_HORIZON_LOCK      = (1 << 7),
    E_IP_EIS_FLAG_DISABLE_LIMIT     = (1 << 8),
    E_IP_EIS_FLAG_FREEZE_IMAGE      = (1 << 9),
};

enum E_IP_EIS_MODE_FLAGS
{
    E_IP_EIS_MODE_USE_IMAGE           = E_IP_EIS_FLAG_USE_IMAGE,
    E_IP_EIS_MODE_USE_MOTION          = E_IP_EIS_FLAG_USE_MOTION,
    E_IP_EIS_MODE_USE_GYRO            = E_IP_EIS_FLAG_USE_GYRO,
    E_IP_EIS_MODE_USE_ACCELERATION    = E_IP_EIS_FLAG_USE_ACCELERATION,
    E_IP_EIS_MODE_USE_MAGNET          = E_IP_EIS_FLAG_USE_MAGNET,
    E_IP_EIS_MODE_USE_GYRO_AND_IMAGE  = (E_IP_EIS_FLAG_USE_GYRO | E_IP_EIS_FLAG_USE_IMAGE),
    E_IP_EIS_MODE_USE_GYRO_AND_MOTION = (E_IP_EIS_FLAG_USE_GYRO | E_IP_EIS_FLAG_USE_MOTION),
    E_IP_EIS_MODE_LOW_LATENCY_MODE    = E_IP_EIS_FLAG_LOW_LATENCY_MODE,
    E_IP_EIS_MODE_HORIZON_LOCK        = E_IP_EIS_FLAG_HORIZON_LOCK,
    E_IP_EIS_MODE_DISABLE_LIMIT       = E_IP_EIS_FLAG_DISABLE_LIMIT,
    E_IP_EIS_MODE_FREEZE_IMAGE        = E_IP_EIS_FLAG_FREEZE_IMAGE
};

enum	E_IP_EIS_FORMAT {
	E_IP_EIS_FMT_YUV_422_P2 = 0x20100001,
	E_IP_EIS_FMT_YUV_422_P3 = 0x20200001,
	E_IP_EIS_FMT_YUV_420_P2 = 0x30100001,
	E_IP_EIS_FMT_YUV_420_P3 = 0x30200001
};


/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

typedef struct
{
	INT32		width;
	INT32		height;
} T_IP_EIS_SIZE;


typedef struct
{
    INT32 left; 
    INT32 top;  
    INT32 width;
    INT32 height;
} T_IP_EIS_RECT;


typedef struct
{
    INT32	stride;
    VOID*	data;
} T_IP_EIS_PLANE;


typedef struct 
{
    UINT32   format;
    UINT32   width;
    UINT32   height;
    T_IP_EIS_PLANE	planes[4];
} T_IP_EIS_IMAGE;


typedef struct
{
    UINT32	reserve;
    FLOAT	opticalCenterX;
    FLOAT	opticalCenterY;
    FLOAT	focalLengthX;
    FLOAT	focalLengthY;
    UINT32	forwardProjLength;
    FLOAT	*forwardProjLUT;
    UINT32	backwardProjLength;
    FLOAT	*backwardProjLUT;
} T_IP_EIS_CAMERA_INFO;


typedef struct
{
    UINT32	reserve;
    UINT32	flags;
    T_IP_EIS_SIZE	originalSize;
    T_IP_EIS_RECT	inputCrop;
    T_IP_EIS_SIZE	workingSize;
    T_IP_EIS_SIZE	outputSize;
    T_IP_EIS_RECT	stabLimits;
    T_IP_EIS_SIZE	gridSize;
    FLOAT	cellWidth;
    FLOAT	cellHeight;
    FLOAT	gridLeft;
    FLOAT	gridTop;
    T_IP_EIS_CAMERA_INFO	cameraInfo;
    FLOAT	strength;
    FLOAT	minStrength;
    FLOAT	maxStrength;
    UINT32	imuClockJitter;
    UINT32	frameInterval;
    UINT32	gyroInterval;
    UINT32	accelInterval;
    UINT32	magnetInterval;
    INT32	format;
    VOID	*ipuBuffer;
} T_IP_EIS_PARAM;


typedef struct {
	FLOAT	x;
	FLOAT	y;
} T_IP_EIS_NODE;


typedef struct {
	FLOAT	dx;
	FLOAT	dy;
} T_IP_EIS_MOTION;


typedef struct
{
    FLOAT x;
    FLOAT y;
    FLOAT z;
} T_IP_EIS_GYRO_SAMPLE;


typedef struct {
	UINT32			timeStamp;
	UINT32			exposureTime;
	T_IP_EIS_GYRO_SAMPLE	*gyro;
	T_IP_EIS_GYRO_SAMPLE	*accel;
	T_IP_EIS_GYRO_SAMPLE	*magn;
} T_IP_EIS_GYRO_DATA;


typedef struct {
    UINT32 reserve;
    UINT32 timeStamp;		//	(usec)
    UINT32 reserve2;
    INT32  readoutTime;		//	(usec)
    UINT32 exposureTime;	//	(usec)
} T_IP_EIS_FRAMEINFO;


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

extern	INT32	IP_EIS_Open(INT32 id, VOID *heapBuffer, T_IP_EIS_PARAM *param, BOOL gyroEnable);
extern	INT32	IP_EIS_StartFrame(INT32 id, T_IP_EIS_IMAGE *image, T_IP_EIS_FRAMEINFO *frameinfo);
extern	INT32	IP_EIS_AddGyroData(INT32 id, T_IP_EIS_GYRO_DATA *gyroData);
extern	INT32	IP_EIS_Process( INT32 id );
extern	INT32	IP_EIS_Close( INT32 id );
extern	UINT32	IP_EIS_GetBufferSize(T_IP_EIS_PARAM *param);
extern	INT32	IP_EIS_GetCorrectionGrid(INT32 id, UINT32 lineBegin, UINT32 lineEnd, T_IP_EIS_NODE *grid);
extern	UINT32	IP_EIS_GetImageLines(INT32 id);
extern	UINT32	IP_EIS_GetGridLines(INT32 id);
extern	INT32	IP_EIS_GetDisplacementOffset(INT32 id, T_IP_EIS_MOTION *motion);
extern	INT32	IP_EIS_GetStabilizedOffset(INT32 id, T_IP_EIS_MOTION *motion);
extern	INT32	IP_EIS_GetDisplacementMatrix(INT32 id, FLOAT matrix[3][3]);
extern	INT32	IP_EIS_GetStabilizedMatrix(INT32 id, FLOAT matrix[3][3]);
extern	INT32	IP_EIS_StabilizeRectangle( INT32 id, const T_IP_EIS_RECT *in, T_IP_EIS_RECT *out, BOOL split_enabled);
extern	INT32	IP_EIS_SetEnabled(INT32 id, BOOL enabled);
extern	INT32	IP_EIS_GetEnabled(INT32 id);
extern	BOOL	IP_EIS_GetLowLatencyModeEnabled(INT32 id);
extern	VOID	IP_EIS_SetLowLatencyModeEnabled(INT32 id, BOOL onoff);
extern	INT32	IP_EIS_SetFlag(INT32 id, UINT32 inFlag, UINT32 inValue);
extern	INT32	IP_EIS_SetStrength(INT32 id, FLOAT minStrength, FLOAT maxStrength);
extern	VOID	IP_EIS_SetPipeNum(INT32 id, UCHAR pipe_use_num);

extern	VOID IP_EIS_Get_Version(CHAR** str);

/*--------------------------------------------------------------------------------------*/
/* Version  																			*/
/*--------------------------------------------------------------------------------------*/

#endif	// _IP_EIS_H_

/* V00L01 2016.04.15 IP_EIS ver0.0 */
/* V00L01 2016.06.02 IP_EIS ver1.0 */
/* V00L01 2017.03.23 IP_EIS ver1.2 */
