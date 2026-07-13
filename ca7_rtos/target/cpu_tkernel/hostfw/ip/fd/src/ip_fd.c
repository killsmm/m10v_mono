/**
 * @file		ipc_fd.c
 * @brief		API for Fotonation FaceDetect
 * @author		Hattori
 * @since		2016/01/05
 * @date		2016/07/08
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT 2016</I></B>
 */
//#pragma GCC optimize ("O0")
//#define	EVAL		// for DEBUG
//#define	D_DETECT_INFO_PRINT		// for DEBUG
//#define	D_IP_FNC_ENABLE			// for FNC
// ****************************************************************************
//                          Include
// ****************************************************************************
#define GT_CUSTOM_INT
#include <string.h>
#include "ip_fd.h"
#include "ip_fnc.h"
#include "ip_ahip.h"
#include "ddim_typedef.h"
#include "LibAHFD.h"
#include "ahip.h"

#ifdef EVAL
#include "measure_time.h"
#endif
#include "os_user_custom.h"

#include "mba_if.h"

// ****************************************************************************
//                          Define
// ****************************************************************************
#define	D_IP_FD_STATUS_NONE			0x0000
#define	D_IP_FD_STATUS_HEAP			0x0001
#define	D_IP_FD_STATUS_DETECTOR		0x0004
#define D_IP_FD_STATUS_PRE_START    0x0008
#define	D_IP_FD_STATUS_START		0x0010

#define D_IP_FD_ALREADY_OPEN		0x01
#define D_IP_FD_NOT_OPEN			0x00
#define D_IP_FD_ALREADY_START		0x02
#define D_IP_FD_NOT_START			0x00

#define	D_IP_FD_CLOSE_NG			0
#define	D_IP_FD_CLOSE_OK			1

#define PRE_BUF_SIZE				(15 * 1024 * 1024)
#define FD_BUF_SIZE					(20 * 160 * 1024)

#define FD_CONV_POS( x, real_size, map_size)	((INT32)((x * real_size / map_size + 128) >> 8))

#define FLG_AHIP_FD_START			(0x00000001)
#define	FLG_AHIP_FD_TERMINATE		(0x10000000)
#define FLG_AHIP_FD_WAITPTN			( FLG_AHIP_FD_START | FLG_AHIP_FD_TERMINATE )

// ****************************************************************************
//                           Structure
// ****************************************************************************
typedef struct	_T_IP_FD_FACE	{
			UINT32				structSize;
			T_IP_FD_GT_RECT		boundRect;
			INT32				rollAngle;
			INT32				yawAngle;
			UINT32				confidence;
			UINT32				trackedFaceId;
			T_IP_FD_GT_RECT		eyeL;
			T_IP_FD_GT_RECT		eyeR;
			T_IP_FD_GT_RECT		mouth;
			INT32				blinkLevelL;
			INT32				blinkLevelR;
			INT32				smileLevel;
			INT32				childLevel;
			INT32				pitchAngle;
} T_IP_FD_FACE;

typedef struct	T_IP_FD_FACE_DATA	{
	T_IP_FD_DATA		fd_data[D_IP_FD_FACE_MAX];
	UINT32				fd_count;
} T_IP_FD_FACE_DATA;

// ****************************************************************************
//                           Global Data
// ****************************************************************************
static	AHFD_DETECTOR	*gIpFDdet ;
static	GT_HEAP			*gIpFDheapPtr ;
static	VOID			*gIpFDahfdIO ;
static	GT_IMAGE 		gIpFdAhip_frame;
static	VOID			*gIpPreBuffer;
static	UINT32			gIpPreBufferSize;
static	VOID			*gIpFdBuffer;
static	UINT32			gIpFdBufferSize;
static	INT32			gIpFdMode;
static	UINT32			gIpFdMapwidth;
static	UINT32			gIpFdMapheight;
static	VOID			*gIpFdHeapBuffer   ;
#ifdef D_IP_FNC_ENABLE
static	UINT32			gIpClosePermission = 0;
#endif	// D_IP_FNC_ENABLE
static 	USHORT			gIpFD_Status = D_IP_FD_STATUS_NONE;
static T_IP_FD_FACE_DATA	gIpFdDetectData[D_IP_FD_INSTANCE_NUM_MAX];
static T_IP_FD_PLANE_DATA	gIpFdPlane[D_IP_FD_INSTANCE_NUM_MAX];
static UCHAR				gIpFdDemosaicID = 0;
static T_IP_FD_PARAM		gIpFdParam[D_IP_FD_INSTANCE_NUM_MAX];

#ifdef EVAL
static	ULONG			gIpFdExTime = 0;
#endif

static T_IP_FD_FUNC		gIpFdTaskEndCallBack = NULL;

// ****************************************************************************
//                        External Method Definition
// ****************************************************************************

// ****************************************************************************
//                        Local Method Definition
// ****************************************************************************

// ****************************************************************************
//                          	Method
// ****************************************************************************

/******************************************************************************
 * @brief	Library Open
 * @param	None
 * @retval	D_IP_FD_OK
 * @retval	D_IP_FD_NG_SEQUENCE
 */
INT32 IP_FD_Open( UCHAR demosaic_id )
{
	INT32	ret;

	ret = IP_FNC_SetFlag( D_IP_FNC_STATUS_FD );
	if( ret ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_ALREADY_OPEN);		// already OPEN
	}

	return D_IP_FD_OK;
}

/******************************************************************************
 * @brief	Library Close
 * @param	None
 * @retval	D_IP_FD_OK
 * @retval	D_IP_FD_NG_SEQUENCE
 */
INT32 IP_FD_Close( UCHAR demosaic_id )
{
	SHORT	flag;
	INT32	ret = 0;

	flag = IP_FNC_GetFlag();
	if ( ~flag & D_IP_FNC_STATUS_FD ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_OPEN);		// not OPEN
	}

	//======= AHIP FD Stop  ============
	AhipFdStop();
	if (gIpFD_Status & D_IP_FD_STATUS_PRE_START){
		AhipPreStop();
		gIpFD_Status &= ~(D_IP_FD_STATUS_PRE_START);
	}
	//======= Destroy  ============
	if(gIpFD_Status & D_IP_FD_STATUS_DETECTOR){
		gIpFDdet->vft->Destroy( gIpFDdet );
		gIpFD_Status &= ~(D_IP_FD_STATUS_DETECTOR);
	}
	if(gIpFD_Status & D_IP_FD_STATUS_HEAP){
		gIpFDheapPtr->vft->Destroy( gIpFDheapPtr );
		gIpFD_Status &= ~(D_IP_FD_STATUS_HEAP);
	}

#ifdef D_IP_FNC_ENABLE
	if( gIpClosePermission == D_IP_FD_CLOSE_OK ){
		//======= AHIP Close  ============
		IP_FNC_Ahip_Close();
	}
#endif	// D_IP_FNC_ENABLE

	ret = IP_FNC_ResetFlag( D_IP_FNC_STATUS_FD );
	if( ret ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_OPEN);		// not OPEN
	}

	return D_IP_FD_OK;
}

/******************************************************************************
 * @brief		Ahip Initialize
 * @param		*cfg, mode
 * @retval		D_IP_FD_OK
 * @retval		D_IP_FD_NG_SEQUENCE
 * @retval		D_IP_FD_NG_AHIP_INIT
 * @retval		D_IP_FD_NG_PARAM
 * @retval		D_IP_FD_NG_AHIP_OPEN
 * @retval		D_IP_FD_NG_CREATE_HEAP
 * @retval		D_IP_FD_NG_CREATE_DETECTOR
 * @note		None
 * @attention	None
 */
INT32 IP_FD_Init( UCHAR demosaic_id, T_IP_FD_DETECTOR_CFG	*cfg, UINT32 mode )
{
	AHFD_LIB				*ahfdlib;
	AHFD_DETECTOR_CFG		detector_cfg;
	UINT32					heapSize ;
	INT32					ret;
#ifdef D_IP_FNC_ENABLE
	T_IP_AHIP_PREVIEW_CONF	prev_cfg;
#endif	// D_IP_FNC_ENABLE
	USHORT			flag;

	//--------------------------------------------
	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_FD) ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_OPEN);		// not OPEN
	}

	gIpFdMode = mode;

	detector_cfg.structSize					= sizeof detector_cfg ;
	detector_cfg.flags						= cfg->flags ;				// AHFD_TRACKING_MODE
	detector_cfg.workRes.width				= cfg->workRes.width ;		// 640
	detector_cfg.workRes.height				= cfg->workRes.height ;		// 480
	detector_cfg.lockAngles					= cfg->lockAngles ;
	detector_cfg.smoothing					= cfg->smoothing ;
	detector_cfg.keepFacesOverMoreFrames	= cfg->keepFacesOverMoreFrames ;
	detector_cfg.numStaticSearches			= cfg->numStaticSearches ;
	detector_cfg.maxFaceCountForFeatures	= cfg->maxFaceCountForFeatures ;
	detector_cfg.maxFaceCount				= cfg->maxFaceCount ;
	detector_cfg.boostFDvsFP				= cfg->boostFDvsFP ;
	detector_cfg.minFaceSize				= cfg->minFaceSize ;

	gIpFdHeapBuffer							= cfg->FdHeapBuffer;
	heapSize								= cfg->FdHeapBufferSize;
	gIpPreBuffer							= cfg->PreBuffer;
	gIpPreBufferSize						= cfg->PreBufferSize;
	gIpFdBuffer								= cfg->FdBuffer;
	gIpFdBufferSize							= cfg->FdBufferSize;

#ifndef D_IP_FNC_ENABLE
	//======= AHIP initialize  ============
	if( IP_AHIP_Get_Open_Status( demosaic_id ) == FALSE ){
		T_IP_AHIP_PREVIEW_CFG	cfg_prev;

		if( gIpFdMode == E_IP_FD_PREVIEW ){
			// AHIP open (Preview mode).
			cfg_prev.flags			= E_IP_AHIP_FLAG_PRE;
			cfg_prev.fmt			= E_IP_AHIP_FMT_YUV420P;
			cfg_prev.mode 			= E_IP_AHIP_PRV_MODE_SENSOR_AB_MERGED;
			cfg_prev.size_a.width	= cfg->frame.width / 2;
			cfg_prev.size_a.lines	= cfg->frame.height / 2;
			cfg_prev.size_b.width	= cfg->frame.width / 2;
			cfg_prev.size_b.lines	= cfg->frame.height / 2;
			ret = IP_AHIP_OpenPreview(0, &cfg_prev);
		}
		else{
			// AHIP open (Playback mode).
			ret = IP_AHIP_OpenPlayback( demosaic_id );
		}
		if ( ret != D_IP_AHIP_OK) {
			printf( "IP_AHIP_OpenPreview or Playback(): error%u. ercd=%d\n", 1, ret );
			return (D_IP_FD_NG_AHIP_INIT | (ret & 0xFF));
		}
	}
#else
	//======= AHIP initialize  ============
	ret = AhipInit();
	if( ret != GTS_OK ){
		return (D_IP_FD_NG_AHIP_INIT | (ret & 0xFF));
	}

	//======= AHIP open  ============
	prev_cfg.flags			= AHIP_PRF_PRE | AHIP_PRF_VIS;
	prev_cfg.format			= cfg->frame.format;
	prev_cfg.width			= detector_cfg.workRes.width;
	prev_cfg.height			= detector_cfg.workRes.height;

	if( gIpFdMode == E_IP_FD_PREVIEW ){
		ret = IP_FNC_Ahip_Open( &prev_cfg );
	} else 
	if( gIpFdMode == E_IP_FD_PLAYBACK ){
		ret = IP_FNC_Ahip_Open( NULL );
	}else{
		return D_IP_FD_NG_PARAM;
	}
	if( ret == D_IP_FNC_OK ){
		gIpClosePermission = D_IP_FD_CLOSE_OK;
	}else{
		gIpClosePermission = D_IP_FD_CLOSE_NG;
		return	(D_IP_FD_NG_CREATE_DETECTOR | (ret & 0xFF));
	}
	gIpClosePermission = D_IP_FD_CLOSE_OK;
#endif	// D_IP_FNC_ENABLE

	//--------------------------------------------
	ahfdlib = AhfdLibGet();

	//======= Create Heap  ============
	ret = ahfdlib->vft->CreateHeap( ahfdlib, gIpFdHeapBuffer, heapSize, &gIpFDheapPtr );
	if( ret != GTS_OK ){
		return	(D_IP_FD_NG_CREATE_HEAP | (ret & 0xFF));
	}
	gIpFD_Status |= D_IP_FD_STATUS_HEAP;

	//======= Create Detector  ============
	ret = ahfdlib->vft->CreateDetector( ahfdlib, gIpFDheapPtr, &detector_cfg, &gIpFDahfdIO, &gIpFDdet );
	if( ret != GTS_OK ){
		return	(D_IP_FD_NG_CREATE_DETECTOR | (ret & 0xFF));
	}
	gIpFD_Status |= D_IP_FD_STATUS_DETECTOR;

	gIpFdAhip_frame.format				= cfg->frame.format;
	gIpFdAhip_frame.width				= cfg->frame.width;
	gIpFdAhip_frame.height				= cfg->frame.height;
	gIpFdMapwidth	= detector_cfg.workRes.width ;
	gIpFdMapheight	= detector_cfg.workRes.height ;

	// temporary to avoid fd start fail
	if((gIpFdAhip_frame.width == 2704) && (gIpFdAhip_frame.height == 1520)){
		gIpFdAhip_frame.width = 2704;
		gIpFdAhip_frame.height = 1440;
	}

	gIpFdParam[demosaic_id].extendRateUpper  = 1.0;
	gIpFdParam[demosaic_id].extendRateLeft   = 1.0;
	gIpFdParam[demosaic_id].extendRateWidth  = 1.0;
	gIpFdParam[demosaic_id].extendRateHeight = 1.0;

	return D_IP_FD_OK;
}

/******************************************************************************
 *	@brief		FD Start
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 IP_FD_Start( UCHAR demosaic_id )
{
	UINT32					ret;
    static AHIP_PRE_CONFIG	pre_cfg;
	USHORT					flag;

	//--------------------------------------------
	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_FD) ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_OPEN);
	}

	if( gIpFD_Status & D_IP_FD_STATUS_START ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_ALREADY_START);
	}
	gIpFD_Status |= D_IP_FD_STATUS_START;

	pre_cfg.crop.left			= 0;
	pre_cfg.crop.top			= 0;
	// In merge mode, PRE input is downsampled 2x horizontally
	if(gIpFdMode == E_IP_FD_PREVIEW){
		pre_cfg.crop.width		= gIpFdAhip_frame.width / 2;
	}else{
		pre_cfg.crop.width		= gIpFdAhip_frame.width;
	}
	pre_cfg.crop.height			= gIpFdAhip_frame.height;
	pre_cfg.size_hi.width		= 0;
	pre_cfg.size_hi.height		= 0;
	pre_cfg.size_lo.width		= gIpFdMapwidth;
	pre_cfg.size_lo.height		= gIpFdMapheight;
	pre_cfg.frame_rate			= 0;

	ret = AhipPreStart( &pre_cfg, gIpPreBuffer, gIpPreBufferSize );
	if( ret != GTS_OK && ret != GTS_BUSY ){
		printf("AhipPreStart() : NG =0x%x\n", ret);
		return	(D_IP_FD_NG_AHIP_FD_START | (ret & 0xFF));
	}

	if (ret != GTS_BUSY)
		gIpFD_Status |= D_IP_FD_STATUS_PRE_START;

	//======= AHIP Start  ============
	if(gIpFdMode == E_IP_FD_PREVIEW){
		ret = AhipFdStart( (const void*)gIpFDahfdIO, gIpFdBuffer, gIpFdBufferSize );
	} else
	if(gIpFdMode == E_IP_FD_PLAYBACK){
		ret = AhipFdStart( (const void*)gIpFDahfdIO, gIpFdBuffer, gIpFdBufferSize );
	}
	if( ret != GTS_OK ){
		return	(D_IP_FD_NG_AHIP_FD_START | (ret & 0xFF));
	}

	return D_IP_FD_OK ;
}

/******************************************************************************
 *	@brief		FD Stop
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32	IP_FD_Stop( UCHAR demosaic_id )
{
	USHORT			flag;

	//--------------------------------------------
	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_FD) ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_OPEN);
	}
	if( ~gIpFD_Status & D_IP_FD_STATUS_START ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_START);
	}
	gIpFD_Status &= ~D_IP_FD_STATUS_START;

	return D_IP_FD_OK;
}

/******************************************************************************
 *	@brief		FD Main
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 IP_FD_Detect( UCHAR demosaic_id, T_IP_FD_PLANE_DATA plane_data )
{
	INT32				ret;
	static	UINT32		fremeNo=0;
#ifdef EVAL
	OS_USER_SYSTIM		t_end;
#endif
	USHORT			flag;

	//--------------------------------------------
	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_FD) ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_OPEN);
	}
	if( ~gIpFD_Status & D_IP_FD_STATUS_START ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_START);
	}

#ifdef EVAL
	BF_Measure_Time_Start(E_BF_MEASURE_TIME_ID_DEBUG_3);
#endif	// EVAL

	if(gIpFdMode == E_IP_FD_PLAYBACK){
		// update address
		gIpFdAhip_frame.planes[0].data		= plane_data.plane[0].data;
		gIpFdAhip_frame.planes[1].data		= plane_data.plane[1].data;
		gIpFdAhip_frame.planes[2].data		= plane_data.plane[2].data;
		gIpFdAhip_frame.planes[0].stride	= plane_data.plane[0].stride;
		gIpFdAhip_frame.planes[1].stride	= plane_data.plane[1].stride;
		gIpFdAhip_frame.planes[2].stride	= plane_data.plane[2].stride;

		ret = AhipProcessFrame( &gIpFdAhip_frame );
		if( ret != GTS_OK ){
			return D_IP_FD_NG_AHIP_FDIO ;
		}
		OS_User_Dly_Tsk(10);
	}
	//======= AHIP FD IO  ============
	ret = AhipFdIO( &fremeNo );

	if( ret != GTS_OK ){
		return D_IP_FD_NG_AHIP_FDIO ;
	} else if( ret == GTS_STOPPED){
		return D_IP_FD_NG_AHIP_FD_STOP ;
	}


#ifdef EVAL
	BF_Measure_Time_Print(E_BF_MEASURE_TIME_ID_DEBUG_3, "FD PRE time\n");
#endif	// EVAL


#ifdef EVAL
//	chg_pri ( TSK_SELF, 1 ) ;
	BF_Measure_Time_Start(E_BF_MEASURE_TIME_ID_DEBUG_3);
#endif
	ret = gIpFDdet->vft->DetectFaces( gIpFDdet );
#ifdef EVAL
	BF_Measure_Time_Get(E_BF_MEASURE_TIME_ID_DEBUG_3, &t_end);
//	chg_pri ( TSK_SELF, TPRI_INI ) ;
	gIpFdExTime += t_end;
#endif
	if( ret != GTS_OK ){
		return (D_IP_FD_NG_DETECT_FACES | (ret & 0xFF));
	}

	return D_IP_FD_OK;
}

/******************************************************************************
 *	@brief		IP_FD_Get_Face
 *	@param		face1
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 IP_FD_Get_Face( UCHAR demosaic_id, T_IP_FD_DATA *facedata, UINT32 *fn )
{
	const AHFD_FACE		*face = NULL;
	USHORT			flag;
	INT32			cnt;
	INT32			work_param = 0;
	T_IP_FD_RECT	rect;

	//--------------------------------------------
	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_FD) ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_OPEN);
	}
	if( ~gIpFD_Status & D_IP_FD_STATUS_START ){
		return (D_IP_FD_NG_SEQUENCE | D_IP_FD_NOT_START);
	}

	cnt = 0;
	while((face = gIpFDdet->vft->EnumFaces(gIpFDdet, face ))!=NULL){
		facedata[cnt].rect.left			= FD_CONV_POS( face->boundRect.left, gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].rect.top			= FD_CONV_POS( face->boundRect.top, gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].rect.width		= FD_CONV_POS( face->boundRect.width, gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].rect.height		= FD_CONV_POS( face->boundRect.height, gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].rect.x			= FD_CONV_POS( face->boundRect.left + (face->boundRect.width  / 2), gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].rect.y			= FD_CONV_POS( face->boundRect.top  + (face->boundRect.height / 2), gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].angle.yawAngle	= face->yawAngle;
		facedata[cnt].angle.pitchAngle	= face->pitchAngle;
		facedata[cnt].confidence		= face->confidence;
		facedata[cnt].trackedFaceId		= face->trackedFaceId;
		facedata[cnt].eyeL.left			= FD_CONV_POS( face->eyeL.left, gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].eyeL.top			= FD_CONV_POS( face->eyeL.top, gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].eyeL.width		= FD_CONV_POS( face->eyeL.width, gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].eyeL.height		= FD_CONV_POS( face->eyeL.height, gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].eyeR.left			= FD_CONV_POS( face->eyeR.left, gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].eyeR.top			= FD_CONV_POS( face->eyeR.top, gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].eyeR.width		= FD_CONV_POS( face->eyeR.width, gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].eyeR.height		= FD_CONV_POS( face->eyeR.height, gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].mouth.left		= FD_CONV_POS( face->mouth.left, gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].mouth.top			= FD_CONV_POS( face->mouth.top, gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].mouth.width		= FD_CONV_POS( face->mouth.width, gIpFdAhip_frame.width, gIpFdMapwidth);
		facedata[cnt].mouth.height		= FD_CONV_POS( face->mouth.height, gIpFdAhip_frame.height, gIpFdMapheight);
		facedata[cnt].opt.blinkLevelL	= face->blinkLevelL;
		facedata[cnt].opt.blinkLevelR	= face->blinkLevelR;
		facedata[cnt].opt.smileLevel	= face->smileLevel;
		facedata[cnt].opt.childLevel	= face->childLevel;

		// Extend calcualtion
		rect = facedata[cnt].rect;
		//Upper
		if(gIpFdParam[demosaic_id].extendRateUpper > 1.0){
			work_param = (INT32)((FLOAT)rect.height * (gIpFdParam[demosaic_id].extendRateUpper - 1.0) + 0.5);
			if(work_param > rect.top){
				work_param = rect.top;
			}
			facedata[cnt].rect.top = rect.top - work_param;
			facedata[cnt].rect.height = rect.height + work_param;
		}
		//Left
		if(gIpFdParam[demosaic_id].extendRateLeft > 1.0){
			work_param = (INT32)((FLOAT)rect.width * (gIpFdParam[demosaic_id].extendRateLeft - 1.0) + 0.5);
			if(work_param > rect.left){
				work_param = rect.left;
			}
			facedata[cnt].rect.left = rect.left - work_param;
			facedata[cnt].rect.width = rect.width + work_param;
		}
		//Width
		if(gIpFdParam[demosaic_id].extendRateWidth > 1.0){
			work_param = (INT32)((FLOAT)rect.width * (gIpFdParam[demosaic_id].extendRateWidth - 1.0) + 0.5);
			if((work_param + facedata[cnt].rect.width) > gIpFdAhip_frame.width){
				work_param = gIpFdAhip_frame.width - facedata[cnt].rect.width;
			}
			facedata[cnt].rect.width = facedata[cnt].rect.width + work_param;
		}
		//Height
		if(gIpFdParam[demosaic_id].extendRateHeight > 1.0){
			work_param = (INT32)((FLOAT)rect.height * (gIpFdParam[demosaic_id].extendRateHeight - 1.0) + 0.5);
			if((work_param + facedata[cnt].rect.height) > gIpFdAhip_frame.height){
				work_param = gIpFdAhip_frame.height - facedata[cnt].rect.height;
			}
			facedata[cnt].rect.height = facedata[cnt].rect.height + work_param;
		}
		
		cnt++;
		if( cnt >= D_IP_FD_FACE_MAX) {
			break;
		}
	}
	*fn = cnt;

	return D_IP_FD_OK;
}

/******************************************************************************
 *	@brief		IP_FD_Get_Face_List
 *	@param		demosaic_id
 *	@param		*width
 *	@param		*height
 *	@param		*num_of_faces
 *	@param		*fd_data
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID IP_FD_Get_Face_List( UCHAR demosaic_id, USHORT* const width, USHORT* const height, UCHAR* const num_of_faces, T_IP_FD_DATA* const fd_data )
{
	// set original image size
	*width			= gIpFdAhip_frame.width;
	*height			= gIpFdAhip_frame.height;
	// get total face number
	*num_of_faces	= gIpFdDetectData[demosaic_id].fd_count;
	if( *num_of_faces > D_IP_FD_FACE_MAX) {
		*num_of_faces = D_IP_FD_FACE_MAX;
	}
	// get face data
	if( *num_of_faces != 0 ){
		memcpy( (VOID*)fd_data, (VOID*)&gIpFdDetectData[demosaic_id].fd_data[0], (*num_of_faces * sizeof(T_IP_FD_DATA)));
	}
}

/******************************************************************************
 *	@brief		IP_FD_Get_Time
 *	@param		*time
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 IP_FD_Get_Time( UINT32 *time )
{
#ifdef EVAL
	*time = gIpFdExTime;
#endif
	return D_IP_FD_OK;
}

/******************************************************************************
 *	@brief		IP_FD_Reset_Time
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 IP_FD_Reset_Time( VOID )
{
#ifdef EVAL
	gIpFdExTime = 0;
#endif
	return D_IP_FD_OK;
}

/******************************************************************************
 *	@brief		Get Version
 *	@param		o:**str (version strings)
 *	@return		None
 *	@note		None
 *	@attention	None
 */

INT32 IP_FD_Get_Version( char **str )
{
	if ( str == NULL ) {
		return D_IP_FD_NG_PARAM ;
	}

	*str = strchr( V_Ip_Fd_Version, ':' ) + 1;

	return D_IP_FD_OK;
}

INT32 IP_FD_Set_Param( UCHAR demosaic_id, T_IP_FD_PARAM *param )
{
	if( demosaic_id >= D_IP_FD_INSTANCE_NUM_MAX ){
		return D_IP_FD_NG_PARAM;
	}

	gIpFdParam[demosaic_id].extendRateUpper  = param->extendRateUpper;
	gIpFdParam[demosaic_id].extendRateLeft   = param->extendRateLeft;
	gIpFdParam[demosaic_id].extendRateWidth  = param->extendRateWidth;
	gIpFdParam[demosaic_id].extendRateHeight = param->extendRateHeight;
	
	return D_IP_FD_OK;
}

// FD detect.
static INT32 ip_fd_start_fd_detect( VOID )
{
	INT32				ret;
	T_IP_FD_PLANE_DATA	plane_data;
	UCHAR				demosaic_id;
#ifdef D_DETECT_INFO_PRINT
	static UINT32		print_counter = 0;
	BOOL				print_write_flag = FALSE;
#endif	// D_DETECT_INFO_PRINT

	// get frame information
	demosaic_id	= gIpFdDemosaicID;
	plane_data	= gIpFdPlane[demosaic_id];

#ifdef D_DETECT_INFO_PRINT
	print_counter++;
	if( print_counter > 30 ){
		print_write_flag = TRUE;
		print_counter = 0;		// reset
	}
	else{
		print_write_flag = FALSE;
	}
#endif	// D_DETECT_INFO_PRINT

	// FD detect execute
	ret = IP_FD_Detect( demosaic_id, plane_data );
	if( (ret & D_IP_FD_NG_DETECT_FACES) == D_IP_FD_NG_DETECT_FACES ){
		// Not detect
		gIpFdDetectData[demosaic_id].fd_count = 0;
#ifdef D_DETECT_INFO_PRINT
		if( print_write_flag == TRUE ){
			printf( "FD OK (Not detect) \n" );
		}
#endif	// D_DETECT_INFO_PRINT
		return D_IP_FD_OK;
	}
	else if( ret != D_IP_FD_OK ){
		gIpFdDetectData[demosaic_id].fd_count = 0;
		printf( "IP_FD_Detect Error: ret=0x%x\n", ret );
		return D_IP_FD_NG;
	}

	// get FD data
	ret = IP_FD_Get_Face( demosaic_id, gIpFdDetectData[demosaic_id].fd_data, &gIpFdDetectData[demosaic_id].fd_count );
	if( ret != D_IP_FD_OK ){
		printf( "IP_FD_Get_Face Error: ret=0x%x\n", ret );
		return D_IP_FD_NG;
	}

#ifdef D_DETECT_INFO_PRINT
	if( print_write_flag == TRUE ){
		if( gIpFdDetectData[demosaic_id].fd_count != 0 ){
			for(UINT32 print_count=0; print_count<gIpFdDetectData[demosaic_id].fd_count; print_count++){
				printf("gIpFdDetectData[%d].fd_data[%d].rect = %dx%d(%d, %d)\n",
						demosaic_id, print_count,
						gIpFdDetectData[demosaic_id].fd_data[print_count].rect.width, gIpFdDetectData[demosaic_id].fd_data[print_count].rect.height,
						gIpFdDetectData[demosaic_id].fd_data[print_count].rect.left, gIpFdDetectData[demosaic_id].fd_data[print_count].rect.top);
			}
		}
	}
#endif	// D_DETECT_INFO_PRINT

	return D_IP_FD_OK;
}

void IP_FD_Task_Detect_Start( UCHAR demosaic_id, T_IP_FD_PLANE_DATA plane_data )
{
	if( demosaic_id >= D_IP_FD_INSTANCE_NUM_MAX ){
		return;
	}

	gIpFdPlane[demosaic_id]	= plane_data;
	gIpFdDemosaicID			= demosaic_id;

	OS_User_Set_Flg( FID_AHIP_FD, FLG_AHIP_FD_START );
}

void IP_FD_Task_Start( UCHAR demosaic_id )
{
	if( demosaic_id >= D_IP_FD_INSTANCE_NUM_MAX ){
		return;
	}

	OS_User_Clr_Flg( FID_AHIP_FD, ~FLG_AHIP_FD_WAITPTN );
	OS_User_Sta_Tsk( TID_AHIP_FD, 0 );
}

void IP_FD_Task_End( UCHAR demosaic_id, T_IP_FD_FUNC func )
{
	T_OS_USER_RTSK	pk_rtsk;

	gIpFdTaskEndCallBack = func;
	
	if( demosaic_id >= D_IP_FD_INSTANCE_NUM_MAX ){
		if(gIpFdTaskEndCallBack){
			gIpFdTaskEndCallBack(0);
		}
		return;
	}

	OS_User_Ref_Tsk( TID_AHIP_FD, &pk_rtsk );

	if( pk_rtsk.tskstat == D_OS_USER_TTS_DMT ) {
		if(gIpFdTaskEndCallBack){
			gIpFdTaskEndCallBack(0);
		}
		return;
	}

	OS_User_Set_Flg( FID_AHIP_FD, FLG_AHIP_FD_TERMINATE );

	if(gIpFdTaskEndCallBack == NULL){
		// wait for terminate
		OS_User_Ref_Tsk( TID_AHIP_FD, &pk_rtsk );

		while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ) {
			OS_User_Dly_Tsk( 1 );
			OS_User_Ref_Tsk( TID_AHIP_FD, &pk_rtsk );
		}
	}
}

void IP_FD_Main( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn;
	INT32			ret;
	UCHAR			demosaic_id = 0;

	ret = IP_FD_Start( demosaic_id );
	if( ret == D_IP_FD_OK ){
		while ( 1 ) {
			OS_User_Wai_Flg( FID_AHIP_FD, FLG_AHIP_FD_WAITPTN, D_OS_USER_TWF_ORW, &flgptn );

			// terminate
			if( flgptn & FLG_AHIP_FD_TERMINATE ){
				OS_User_Clr_Flg( FID_AHIP_FD, ~FLG_AHIP_FD_TERMINATE );
				break;
			}

			// FD detect
			if( flgptn & FLG_AHIP_FD_START ){
				OS_User_Clr_Flg( FID_AHIP_FD, ~FLG_AHIP_FD_START );

				FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_FD, FJ_MBALOG_TYPE_START, "FD start" ));
				ret = ip_fd_start_fd_detect();
				FJ_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_FD, FJ_MBALOG_TYPE_END, "FD start" ));
				if( ret != D_IP_FD_OK ){
					printf( "ip_fd_start_fd_detect Error: ret=0x%x\n", ret );
				}
			}
		}
	}
	else{
		printf( "IP_FD_Start Error: ret=0x%x\n", ret );
	}

	// FD stop
	ret = IP_FD_Stop( demosaic_id );
	if( ret != D_IP_FD_OK ){
		printf( "IP_FD_Stop Error: ret=0x%x\n", ret );
	}

	if(gIpFdTaskEndCallBack){
		gIpFdTaskEndCallBack(0);
	}
	
	OS_User_Ext_Tsk();
}
