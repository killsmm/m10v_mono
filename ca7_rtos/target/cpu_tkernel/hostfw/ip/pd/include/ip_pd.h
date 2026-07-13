/**
 * @file		ip_pd.h
 * @brief		API for Fotonation FaceDetect
 * @author		Hattori
 * @since		2016/01/07
 * @date		2016/09/15
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2016</I></B>
 */

#ifndef _IP_PD_H_
#define _IP_PD_H_
// ***************************************************************************
//                          Include
// ***************************************************************************
#include "ip_fnc.h"

// ***************************************************************************
//                          Define       			                          
// ***************************************************************************
#define	D_IP_PD_FN_OK					0x00		//   0
#define	D_IP_PD_FN_FAIL					0xFF		//  -1
#define	D_IP_PD_FN_CANCELLED			0xFE		//  -2
#define	D_IP_PD_FN_NO_SYSTEM_RESOURCES	0xFD		//  -3
#define	D_IP_PD_FN_INVALID_ARG			0xFC		//  -4
#define	D_IP_PD_FN_NOT_SUPPORTED		0xFA		//  -6
#define	D_IP_PD_FN_FMT_NOT_SUPPORTED	0xF9		//  -7
#define	D_IP_PD_FN_NOT_IMPLEMENTED		0xF8		//  -8
#define	D_IP_PD_FN_BAD_STRUCT_SIZE		0xF7		//  -9
#define	D_IP_PD_FN_BAD_IMAGE_SIZE		0xF6		// -10

#define D_IP_PD_OK						(D_IP_OK)						/**< OK */
#define D_IP_PD_NG						(D_IP_NG	| 0x001E0000)	/**< NG */
#define D_IP_PD_NG_SEQUENCE				(D_IP_PD_NG	| 0x0100)
#define D_IP_PD_NG_PARAM				(D_IP_PD_NG	| 0x0200)

#define	D_IP_PD_NG_CREATE_HEAP			(D_IP_PD_NG	| 0x1000)
#define D_IP_PD_NG_CREATE_MANAGER		(D_IP_PD_NG	| 0x1100)
#define D_IP_PD_NG_CREATE_DETECTOR		(D_IP_PD_NG	| 0x1200)
#define D_IP_PD_NG_RESISTER_DETECTOR	(D_IP_PD_NG	| 0x1300)
#define D_IP_PD_NG_START_PREVIEW		(D_IP_PD_NG	| 0x1400)
#define D_IP_PD_NG_START_PLAYBACK		(D_IP_PD_NG	| 0x1500)
#define	D_IP_PD_NG_PREVIEW_RESULT		(D_IP_PD_NG	| 0x1600)
#define	D_IP_PD_NG_PLAYBACK_RESULT		(D_IP_PD_NG	| 0x1700)
#define D_IP_PD_NG_FREE_RESULT			(D_IP_PD_NG	| 0x1800)
#define D_IP_PD_NG_STOP_PREVIEW			(D_IP_PD_NG	| 0x1900)

#define	D_IP_PD_FULLBODY				0
#define	D_IP_PD_HALFBODY				1

#define D_IP_PD_ALREADY_OPEN			0x00
#define D_IP_PD_NOT_YET_OPEN			0x01
#define D_IP_PD_ALREADY_START			0x02
#define D_IP_PD_NOT_YET_START			0x03

#define D_IP_PD_MEM_BUF_SIZE			(0x1900000)		// T.B.D

// ***************************************************************************
//                        Enum
// ***************************************************************************
//////////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_FLAGS
//////////////////////////////////////////////////////////////////////////////
enum E_IP_AHOF_PERSON_FLAGS
{
	E_IP_PD_AHOF_PERSON_FLAGS_TRACKING_MODE			= (1 << 0),
	E_IP_PD_AHOF_PERSON_FLAGS_SHOW_HW_PD_DETECTIONS	= (1 << 1),
	E_IP_PD_AHOF_PERSON_FLAGS_SHOW_HW_HB_DETECTIONS	= (1 << 2)
};

//////////////////////////////////////////////////////////////////////////////
// AHOF_PERSON_FEATURE_*
//////////////////////////////////////////////////////////////////////////////
enum E_IP_AHOF_PERSON_FEATURES_FLAGS
{
	E_IP_PD_AHOF_PERSON_FEATURE_INTERNAL_HEAP		= (1 << 0),
	E_IP_PD_AHOF_PERSON_FEATURE_FULLBPDY			= (1 << 1),
	E_IP_PD_AHOF_PERSON_FEATURE_HALFBPDY			= (1 << 2)
};

enum E_IP_PD_MODE
{
	E_IP_PD_MODE_PREVIEW							= (1 << 0),
	E_IP_PD_MODE_PLAYBACK							= (1 << 1),
};


// ***************************************************************************
//                        Structure
// ***************************************************************************
typedef struct	_T_IP_PD_GT_RECT	{
	INT32			left;	/**< The x-coordinate of the upper-left corner of the rectangle. */
	INT32			top;	/**< The y-coordinate of the upper-left corner of the rectangle. */
	INT32			width;	/**< The horizontal width of the rectangle. */
	INT32			height;	/**< The vertical height of the rectangle. */
} T_IP_PD_GT_RECT;

typedef struct	_T_IP_PD_RECT	{
	INT32			left;	/**< The x-coordinate of the upper-left corner of the rectangle. */
	INT32			top;	/**< The y-coordinate of the upper-left corner of the rectangle. */
	INT32			width;	/**< The horizontal width of the rectangle. */
	INT32			height;	/**< The vertical height of the rectangle. */
	INT32			x;		/**< The  x-coordinate of the center */
	INT32			y;		/**< The  y-coordinate of the center */
}T_IP_PD_RECT;

typedef struct	_T_IP_PD_PLANE	{
	UINT32		stride;
	VOID*		data;
} T_IP_PD_PLANE;

typedef struct	T_IP_PD_PLANE_DATA	{
	T_IP_PD_PLANE		plane[3];
} T_IP_PD_PLANE_DATA;

//////////////////////////////////////////////////////////////////////////////
// AHFD_DETECTOR_CFG
//////////////////////////////////////////////////////////////////////////////
typedef struct	_T_IP_PD_DETECTOR_CFG	{
	VOID	*heapBuffer   ;
	VOID	*preMapBuf;			// PRE map buffer (wxhx3/2x3)

	UINT32	flags;				// Configuration flags
	UINT32	minPersonSize;		// Minimum person size in pixels the library should try to detect.
	UINT32	maxPersonSize;		// Maximum person size in pixels the library should try to detect.
	UINT32	maxPersonCount;		// Maximum number of persons to be detected and tracked.
	INT32	boostDRvsFP;		// Trade-off between detection rate and false positives.
	INT32	boostDRvsSPEED;		// Trade-off between detection rate and execution speed.

	UINT32	width;				// image width
	UINT32	width2;				// image crop width
	UINT32	height;				// image height
	T_IP_PD_PLANE	planes[4];	// plane info

} T_IP_PD_DETECTOR_CFG;

typedef struct	_T_IP_PD_PERSON	{
	T_IP_PD_RECT		rect;			// The detected-data of the person.
	INT32				rollAngle;		// The clockwise roll angle of the person, in degrees. 
	INT32				yawAngle;		// The yaw angle of the person, in degrees.
	INT32				pitchAngle;		// The pitch angle of the person, in degrees.
	INT32				type;			// The type of person. 0 - FullBody 1 - HalfBody 
	UINT32				trackid;		// The tracking ID of the person. 
	INT32				confidence;		// The confidence that the detected person is correct. 
} T_IP_PD_PERSON;

#define D_IP_PD_MAX_PERSONS 		16
#define D_IP_PD_MIN_PERSON_SIZE 	64
#define D_IP_PD_DET_W				640
#define D_IP_PD_DET_H				360

// ***************************************************************************
//                        Global Function
// ***************************************************************************
extern	INT32	IP_PD_Open( VOID );
extern	INT32	IP_PD_Close( VOID );
extern	UINT32	IP_PD_GetLibHeapSize( T_IP_PD_DETECTOR_CFG *cfg );
extern	INT32	IP_PD_Init( T_IP_PD_DETECTOR_CFG	*cfg );
extern	INT32	IP_PD_Start( INT32 mode );
extern	INT32	IP_PD_Stop( VOID );
extern	INT32	IP_PD_Detect( VOID );
extern	INT32	IP_PD_Get_Person( T_IP_PD_PERSON *person,UINT32 *np );
extern	INT32	IP_PD_Get_Person_List( T_IP_PD_PERSON *person,UINT32 *np );
extern	INT32	IP_PD_Get_Time( UINT32 *time );
extern	INT32	IP_PD_Reset_Time( VOID );
extern	INT32	IP_PD_Get_Version( CHAR **str );

extern  INT32	IP_PD_Task_Detect_Start(T_IP_PD_PLANE_DATA plane_data);
extern	INT32	IP_PD_Task_Start(VOID);
extern	VOID 	IP_PD_Task_End();

/*----------------------------------------------------------------------*/
/* Version  															*/
/*----------------------------------------------------------------------*/
#define V_Ip_Pd_Version		"flib_version:V02L02\r\n"

#endif	// _IP_PD_H_

// V00L01 2016.01.05 IP_PD ver0.1
// V00L02 2016.04.01 IP_PD ver0.2
// V00L02 2016.04.14 IP_PD ver0.3
// V01L01 2016.07.01 IP_PD ver1.0
// V02L01 2016.09.15 IP_PD ver2.0
// V02L01 2016.11.09 IP_PD ver2.2


