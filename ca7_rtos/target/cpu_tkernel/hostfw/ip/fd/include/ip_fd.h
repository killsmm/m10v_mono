/**
 * @file		ip_fd.h
 * @brief		API for Fotonation FaceDetect
 * @author		Hattori
 * @since		2016/01/05
 * @date		2016/10/31
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2016</I></B>
 */

#ifndef _IP_FD_H_
#define _IP_FD_H_

// ***************************************************************************
//                          Include
// ***************************************************************************
#include "ddim_typedef.h"
#include "ip_fnc.h"

// ***************************************************************************
//                          Define       			                          
// ***************************************************************************
#define D_IP_FD_OK						(D_IP_OK)
#define D_IP_FD_NG						(D_IP_NG	| 0x00040000)
#define D_IP_FD_NG_SEQUENCE				(D_IP_FD_NG	| 0x0000)
#define D_IP_FD_NG_PARAM				(D_IP_FD_NG	| 0x0100)

#define D_IP_FD_NG_CREATE_HEAP			(D_IP_FD_NG	| 0x1000)
#define D_IP_FD_NG_CREATE_DETECTOR		(D_IP_FD_NG	| 0x1200)
#define D_IP_FD_NG_GET_HEAPSIZE			(D_IP_FD_NG	| 0x1300)
#define D_IP_FD_NG_DESTROY				(D_IP_FD_NG	| 0x1400)
#define D_IP_FD_NG_DETECT_FACES			(D_IP_FD_NG	| 0x1500)
#define D_IP_FD_NG_ENUM_FACES			(D_IP_FD_NG	| 0x1600)
#define D_IP_FD_NG_CLEAR_FACES			(D_IP_FD_NG	| 0x1700)
#define D_IP_FD_NG_UPDATE_IMAGE_ORI		(D_IP_FD_NG	| 0x1800)
#define D_IP_FD_NG_GET_MAIN_FACE		(D_IP_FD_NG	| 0x1900)
#define D_IP_FD_NG_GET_RAW_FACE			(D_IP_FD_NG	| 0x2000)
#define D_IP_FD_NG_PROCESS_FRAME		(D_IP_FD_NG	| 0x2100)
#define D_IP_FD_NG_AHIP_FDIO			(D_IP_FD_NG	| 0x2200)
#define D_IP_FD_NG_AHIP_FD_START		(D_IP_FD_NG	| 0x2300)
#define D_IP_FD_NG_AHIP_INIT			(D_IP_FD_NG	| 0x2400)
#define D_IP_FD_NG_AHIP_OPEN			(D_IP_FD_NG	| 0x2500)
#define D_IP_FD_NG_AHIP_FD_STOP			(D_IP_FD_NG	| 0x2600)

#define	D_IP_FD_FN_OK					0x00
#define	D_IP_FD_FN_FAIL					0xFF
#define	D_IP_FD_FN_CANCELED				0xFE
#define	D_IP_FD_FN_NO_SYSTEM_RESOURCES	0xFD
#define	D_IP_FD_FN_INVALID_ARG			0xFC
#define	D_IP_FD_FN_NOT_SUPPORTED		0xFA
#define	D_IP_FD_FN_FMT_NOT_SUPPORTED	0xF9
#define	D_IP_FD_FN_NOT_IMPLEMENTED		0xF8
#define	D_IP_FD_FN_BAD_STRUCT_SIZE		0xF7
#define	D_IP_FD_FN_BAD_IMAGE_SIZE		0xF6

#define	D_IP_FD_FN_INVALID_MODE			0xEC
#define	D_IP_FD_FN_HW_ERROR				0xEB
#define	D_IP_FD_FN_BUFFER_TOO_SMALL		0xFA
#define	D_IP_FD_FN_BUSY					0xE9
#define	D_IP_FD_FN_STOPPED				0xE8
#define	D_IP_FD_FN_WAIT					0xE7

#define D_IP_FD_FACE_MAX				(16)	/**< Max face number					*/
#define D_IP_FD_INSTANCE_NUM_MAX		(10)	/**< instance max number				*/

#define D_IP_FD_MEM_HEAP_BUF_SIZE	(0x600000)		// T.B.D
#define D_IP_FD_MEM_PRE_BUF_SIZE	(0xF00000)		// T.B.D
#define D_IP_FD_MEM_BUF_SIZE		(0x320000)		// T.B.D

enum E_IP_FD_LOCK
{
	E_IP_FD_LOCK_END,			///< Note: The angle array should be finished with AHFD_LOCK_END.
	E_IP_FD_LOCK_0,				///< rollAngle = [- 45;  45],                   yawAngle = 0			pitchAngle = 0
	E_IP_FD_LOCK_90,			///< rollAngle = [  45; 135],                   yawAngle = 0			pitchAngle = 0
	E_IP_FD_LOCK_180,			///< rollAngle = [ 135; 180] and [ -135; -179], yawAngle = 0			pitchAngle = 0
	E_IP_FD_LOCK_270,			///< rollAngle = [- 45;-135],                   yawAngle = 0			pitchAngle = 0
	E_IP_FD_LOCK_SP_0,			///< rollAngle = 0,                             yawAngle = 45 and -45	pitchAngle = 0
	E_IP_FD_LOCK_P_0,			///< rollAngle = 0,                             yawAngle = 90 and -90	pitchAngle = 0
	E_IP_FD_LOCK_SP_90,			///< rollAngle = 90,                            yawAngle = 45 and -45	pitchAngle = 0
	E_IP_FD_LOCK_P_90,			///< rollAngle = 90,                            yawAngle = 90 and -90	pitchAngle = 0
	E_IP_FD_LOCK_SP_180,		///< rollAngle = 180,                           yawAngle = 45 and -45	pitchAngle = 0
	E_IP_FD_LOCK_P_180,			///< rollAngle = 180,                           yawAngle = 90 and -90	pitchAngle = 0
	E_IP_FD_LOCK_SP_270,		///< rollAngle = -90,                           yawAngle = 45 and -45	pitchAngle = 0
	E_IP_FD_LOCK_P_270,			///< rollAngle = -90,                           yawAngle = 90 and -90	pitchAngle = 0
	E_IP_FD_LOCK_SP_UP_0,		///< rollAngle = 0								yawAngle = 45 and -45	pitchAngle = 30
	E_IP_FD_LOCK_SP_DOWN_0,		///< rollAngle = 0								yawAngle = 45 and -45	pitchAngle = -30
	E_IP_FD_LOCK_P_UP_0,		///< rollAngle = 0        	                    yawAngle = 90 and -90	pitchAngle = 30
	E_IP_FD_LOCK_P_DOWN_0,		///< rollAngle = 0          	                yawAngle = 90 and -90	pitchAngle = -30
	E_IP_FD_LOCK_SP_UP_90,		///< rollAngle = 90								yawAngle = 45 and -45	pitchAngle = 30
	E_IP_FD_LOCK_SP_DOWN_90,	///< rollAngle = 90								yawAngle = 45 and -45	pitchAngle = -30
	E_IP_FD_LOCK_P_UP_90,		///< rollAngle = 90        	                    yawAngle = 90 and -90	pitchAngle = 30
	E_IP_FD_LOCK_P_DOWN_90,		///< rollAngle = 90          	                yawAngle = 90 and -90	pitchAngle = -30
	E_IP_FD_LOCK_SP_UP_180,		///< rollAngle = 180							yawAngle = 45 and -45	pitchAngle = 30
	E_IP_FD_LOCK_SP_DOWN_180,	///< rollAngle = 180							yawAngle = 45 and -45	pitchAngle = -30
	E_IP_FD_LOCK_P_UP_180,		///< rollAngle = 180        	                yawAngle = 90 and -90	pitchAngle = 30
	E_IP_FD_LOCK_P_DOWN_180,	///< rollAngle = 180          	                yawAngle = 90 and -90	pitchAngle = -30
	E_IP_FD_LOCK_SP_UP_270,		///< rollAngle = -90							yawAngle = 45 and -45	pitchAngle = 30
	E_IP_FD_LOCK_SP_DOWN_270,	///< rollAngle = -90							yawAngle = 45 and -45	pitchAngle = -30
	E_IP_FD_LOCK_P_UP_270,		///< rollAngle = -90        	                yawAngle = 90 and -90	pitchAngle = 30
	E_IP_FD_LOCK_P_DOWN_270		///< rollAngle = -90          	                yawAngle = 90 and -90	pitchAngle = -30
};

enum E_IP_FD_CONFIG_FLAGS
{
	E_IP_FD_TRACKING_MODE			= (1 << 0),
	E_IP_FD_LUMINANCE_ONLY			= (1 << 1),
	E_IP_FD_DETECT_EYES				= (1 << 3),
	E_IP_FD_DETECT_BLINK			= (1 << 4),
	E_IP_FD_DETECT_SMILE			= (1 << 5),
	E_IP_FD_DETECT_MOUTH			= (1 << 6),
	E_IP_FD_DETECT_CHILD			= (1 << 7)
};

enum E_IP_FD_MODE
{
	E_IP_FD_PREVIEW,
	E_IP_FD_PLAYBACK
};

enum E_IP_FD_FMT
{
	E_IP_FD_FMT_YUV_420P	= 0,			/**< YUV420 planar				*/
	E_IP_FD_FMT_YUV_422P,					/**< YUV422 planar				*/
	E_IP_FD_FMT_YUV_444P,					/**< YUV444 planar				*/
	E_IP_FD_FMT_YUV_420SP,					/**< YUV420 semi-planar			*/
	E_IP_FD_FMT_YUV_422SP,					/**< YUV422 semi-planar			*/
	E_IP_FD_FMT_YUV_420B,					/**< YUV420 block semi-planar	*/
};

// ***************************************************************************
//                        Structure
// ***************************************************************************
typedef ULONG (*T_IP_FD_FUNC)( ULONG prm );

typedef struct	_T_IP_FD_GT_RECT	{
	INT32			left;	/**< The x-coordinate of the upper-left corner of the rectangle. */
	INT32			top;	/**< The y-coordinate of the upper-left corner of the rectangle. */
	INT32			width;	/**< The horizontal width of the rectangle. */
	INT32			height;	/**< The vertical height of the rectangle. */
} T_IP_FD_GT_RECT;

typedef struct	_T_IP_FD_RECT	{
	INT32			left;	/**< The x-coordinate of the upper-left corner of the rectangle. */
	INT32			top;	/**< The y-coordinate of the upper-left corner of the rectangle. */
	INT32			width;	/**< The horizontal width of the rectangle. */
	INT32			height;	/**< The vertical height of the rectangle. */
	INT32			x;		/**< The  x-coordinate of the center */
	INT32			y;		/**< The  y-coordinate of the center */
} T_IP_FD_RECT;

typedef struct	_T_IP_FD_GT_SIZE	{
	INT32			width;
	INT32			height;
} T_IP_FD_GT_SIZE;

typedef struct	_T_IP_FD_PLANE	{
	UINT32			stride;
	VOID*			data;
} T_IP_FD_PLANE;

typedef struct	T_IP_FD_PLANE_DATA	{
	T_IP_FD_PLANE		plane[3];
} T_IP_FD_PLANE_DATA;

typedef struct	_T_IP_FD_GT_FRAME	{
	UINT32				format;
	UINT32				width;
	UINT32				height;
} T_IP_FD_GT_FRAME;

typedef	struct	_T_IP_FD_ANGLE_	{
	INT32				rollAngle;
	INT32				yawAngle;
	INT32				pitchAngle;
} T_IP_FD_ANGLE;

typedef	struct	_T_IP_FD_OPTION_	{
	INT32				blinkLevelL;
	INT32				blinkLevelR;
	INT32				smileLevel;
	INT32				childLevel;
} T_IP_FD_OPTION;

typedef struct	_T_IP_FD_DETECTOR_CFG	{
			VOID				*FdHeapBuffer;
			UINT32				FdHeapBufferSize;
			VOID				*PreBuffer;
			UINT32				PreBufferSize;
			VOID				*FdBuffer;
			UINT32				FdBufferSize;
			T_IP_FD_GT_FRAME	frame;
			UINT32				flags;
			T_IP_FD_GT_SIZE		workRes;
			UINT32				minFaceSize;
	const	INT32				*lockAngles;
			UINT32				smoothing;
			INT32				keepFacesOverMoreFrames;
			UINT32				numStaticSearches;
			UINT32				maxFaceCountForFeatures;
			UINT32				maxFaceCount;
			INT32				boostFDvsFP;
} T_IP_FD_DETECTOR_CFG;

typedef struct	_T_IP_FD_DATA	{
			T_IP_FD_RECT		rect;
			UINT32				trackedFaceId;
			UINT32				confidence;
			T_IP_FD_GT_RECT		eyeL;
			T_IP_FD_GT_RECT		eyeR;
			T_IP_FD_GT_RECT		mouth;
			T_IP_FD_OPTION		opt;
			T_IP_FD_ANGLE		angle;
} T_IP_FD_DATA;

typedef struct	_T_IP_FD_PARAM	{
			INT32			boostDR;
			FLOAT			extendRateUpper;
			FLOAT			extendRateLeft;
			FLOAT			extendRateWidth;
			FLOAT			extendRateHeight;
} T_IP_FD_PARAM;

// ***************************************************************************
//                        Global Function
// ***************************************************************************
extern	INT32	IP_FD_Open( UCHAR demosaic_id );
extern	INT32	IP_FD_Close( UCHAR demosaic_id );
extern	INT32	IP_FD_Init( UCHAR demosaic_id, T_IP_FD_DETECTOR_CFG	*cfg,  UINT32 mode );
extern	INT32	IP_FD_Start( UCHAR demosaic_id );
extern	INT32	IP_FD_Stop( UCHAR demosaic_id );
extern	INT32	IP_FD_Detect( UCHAR demosaic_id, T_IP_FD_PLANE_DATA plane_data );
extern	INT32	IP_FD_Get_Face( UCHAR demosaic_id, T_IP_FD_DATA *face, UINT32 *fn );
extern	INT32	IP_FD_Get_Time( UINT32 *time );
extern	INT32	IP_FD_Reset_Time( VOID );
extern	INT32	IP_FD_Get_Version( char **str );
extern	INT32	IP_FD_Set_Param( UCHAR demosaic_id, T_IP_FD_PARAM *param );

extern	VOID IP_FD_Get_Face_List( UCHAR demosaic_id, USHORT* const width, USHORT* const height, UCHAR* const num_of_faces, T_IP_FD_DATA* const fd_data );
extern	VOID IP_FD_Task_Detect_Start( UCHAR demosaic_id, T_IP_FD_PLANE_DATA plane_data );
extern	VOID IP_FD_Task_Start( UCHAR demosaic_id );
extern	VOID IP_FD_Task_End( UCHAR demosaic_id, T_IP_FD_FUNC func );

/*----------------------------------------------------------------------*/
/* Version  															*/
/*----------------------------------------------------------------------*/
#define V_Ip_Fd_Version		"flib_version:V01L06\r\n"

#endif	// _IP_FD_H_
