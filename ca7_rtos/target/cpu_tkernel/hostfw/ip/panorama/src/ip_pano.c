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

#include <stdio.h>
#include <string.h>
#include "gtypes.h"
#include "LibPan.h"
#include "ip_pano.h"
#include "ahip.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
extern	AHIP_GDC_CONTEXT *gIp_ge_ctx;

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/
typedef struct {
	UINT32		ipc;
	UINT32		im;
} T_IPC_IIP_USER_IPC2IM_TBL;

typedef struct {
	LLONG		ipc;
	LLONG		im;
} T_IPC_IIP_USER_IPC2IM_TBL8;

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/
static	UINT32			g_ip_pano_state=0;
static	PAN_STITCHER	*g_ip_pano_stitcher = NULL;
static	GT_HEAP			*g_ip_pano_heap = NULL;
static	UINT32			g_ip_pano_useResource=0;	//	1:Gyro / 2:HRE
const static	PAN_LIB	*g_ip_pano_lib = NULL;

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Local Method Definition																*/
/*--------------------------------------------------------------------------------------*/
static INT32 ip_pano_convert_retcode(INT32 status)
{
	INT32		retcode;

	switch( status )
	{
		case	GTS_OK:
			retcode = D_IP_PANO_OK;
			break;

		case	GTS_FAIL:
			retcode = D_IP_PANO_NG;
			break;

		case	GTS_CANCELLED:
			retcode = D_IP_PANO_NG_CANCELLED;
			break;

		case	GTS_NO_SYSTEM_RESOURCES:
			retcode = D_IP_PANO_NG_NO_SYSTEM_RESOURCES;
			break;

		case	GTS_INVALID_ARG:
			retcode = D_IP_PANO_NG_INVALID_ARG;
			break;

		case	GTS_NOT_SUPPORTED:
			retcode = D_IP_PANO_NG_NOT_SUPPORTED;
			break;

		case	GTS_FMT_NOT_SUPPORTED:
			retcode = D_IP_PANO_NG_FMT_NOT_SUPPORTED;
			break;

		case	GTS_NOT_IMPLEMENTED:
			retcode = D_IP_PANO_NG_NOT_IMPLEMENTED;
			break;

		case	GTS_BAD_STRUCT_SIZE:
			retcode = D_IP_PANO_NG_BAD_STRUCT_SIZE;
			break;

		case	GTS_BAD_IMAGE_SIZE:
			retcode = D_IP_PANO_NG_BAD_IMAGE_SIZE;
			break;

		case	GTS_PAN_FRAME_ADDED:
			retcode = D_IP_PANO_FRAME_ADDED;
			break;

		case	GTS_PAN_DONE:
			retcode = D_IP_PANO_DONE;
			break;

		case	GTS_PAN_LOW_CONTRAST:
			retcode = D_IP_PANO_NG_LOW_CONTRAST;
			break;

		case	GTS_PAN_OUT_OF_SCENE:
			retcode = D_IP_PANO_NG_OUT_OF_SCENE;
			break;

		case	GTS_PAN_HIGH_SPEED:
			retcode = D_IP_PANO_NG_HIGH_SPEED;
			break;

		case	GTS_PAN_WRONG_DIRECTION:
			retcode = D_IP_PANO_NG_WRONG_DIRECTION;
			break;

		case	GTS_PAN_BIG_DISPLACEMENT:
			retcode = D_IP_PANO_NG_BIG_DISPLACEMENT;
			break;

		default:
			retcode = D_IP_PANO_NG_UNKNOWN;
			break;
	}

	return	retcode;
}


static UINT32 ip_pano_fmt_gt2ahip(UINT32 fmt_gt)
{
    switch (fmt_gt)
    {
    case GT_FMT_YUV_444_P3:
        return AHIP_FMT_YUV_444P;
    case GT_FMT_YUV_420_P2:
        return AHIP_FMT_YUV_420SP;
    case GT_FMT_YUV_420_P3:
        return AHIP_FMT_YUV_420P;
    case GT_FMT_YUV_422_P2:
        return AHIP_FMT_YUV_422SP;
    case GT_FMT_YUV_422_P3:
        return AHIP_FMT_YUV_422P;
    case GT_FMT_GRAY:
        return GT_FMT_GRAY;
    default:
//        printf("Error: Unsupported Format: %d\n", fmt_gt);
        return 0;
    }
}


/*--------------------------------------------------------------------------------------*/
/* Table																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function  																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API		  																			*/
/*--------------------------------------------------------------------------------------*/
/**
	Gets the Panorama library version.
*/
VOID IP_PANO_Get_Version(CHAR** str)
{
	*str = (CHAR*)strchr( V_IP_PANO_Version, ':' ) + 1;
	return;
}



/**
	Opens the Panorama library.
*/
INT32	IP_PANO_Open(VOID *heapBuffer, T_IP_PANO_CONFIG *config)
{
#ifndef D_IP_PANO_STUB
	INT32		retcode;
	PAN_PARAMS	params;
	PAN_CFG		cfg;
	AHIP_VIS_CONFIG	visCfg;
	UINT32		visHWSize;


	if ( g_ip_pano_state != E_IP_PANO_SEQ_IDLE ){
		return D_IP_PANO_NG_SEQUENCE;
	}

	g_ip_pano_state = E_IP_PANO_SEQ_OPEN;
	g_ip_pano_heap = NULL;
	g_ip_pano_stitcher = NULL;

	if(config->flags & PAN_FLAG_USE_GYRO)	{
		g_ip_pano_useResource = 1 ;
	}
	else if(config->flags & PAN_FLAG_USE_HRE)	{
//	メインパス向けに既にAhipOpen()されているのでコメントアウト
//		if( AhipOpen(&ahipCfg) ) {
//			return	D_IP_PANO_NG;
//		}
		g_ip_pano_useResource = 2 ;
	}
	else	{
		g_ip_pano_useResource = 0 ;
	} 

	g_ip_pano_lib = PanLibGet();

	// set here all cfg fields that are marked with (*)
	memset(&params, 0, sizeof(PAN_PARAMS));
	params.structSize = sizeof(PAN_PARAMS);

	//
	// Set the rest of cfg fields:
	//
    cfg.structSize = sizeof(cfg); 
    cfg.flags             = config->flags;
    cfg.frameFormat       = config->frameFormat;
    cfg.frameSize.width   = config->frameSize.width;
    cfg.frameSize.height  = config->frameSize.height;
    cfg.frameOrientation  = config->frameOrientation;
    cfg.focalLength       = config->focalLength;
    cfg.opticalCenter.x   = config->frameSize.width / 2;
    cfg.opticalCenter.y   = config->frameSize.height / 2;
    cfg.direction         = config->direction;
    cfg.workCrop.left     = config->workCrop.left;
    cfg.workCrop.top      = config->workCrop.top;
    cfg.workCrop.width    = config->workCrop.width;
    cfg.workCrop.height   = config->workCrop.height;
    cfg.workSize.width    = config->workSize.width;
    cfg.workSize.height   = config->workSize.height;
    cfg.maxDisplacement   = config->maxDisplacement;
    cfg.panoramaLength    = config->panoramaLength;
    cfg.panoramaBufSize   = config->panoramaBufSize;
    cfg.keyFramesBufSize  = config->keyFramesBufSize;
    cfg.transformStart    = (PAN_TRANSFORM_START_ROUTINE)config->transformStart;
    cfg.transformComplete = (PAN_TRANSFORM_COMPLETE_ROUTINE)config->transformComplete;
    cfg.transformContext  = (void*)gIp_ge_ctx;

	//
	// Call GetParams() with an incomplete cfg:
	//
	retcode = g_ip_pano_lib->GetParams(&cfg, &params);
	if( retcode != GTS_OK ){
		return	ip_pano_convert_retcode( retcode );
	}
    
    cfg.keyFramesBuf     = config->keyFramesBuf;
    cfg.keyFramesBufSize = params.keyFramesBufSize;
    cfg.panoramaBuf      = config->panoramaBuf;
    cfg.panoramaBufSize  = params.panoramaBufSize;

    config->keyFramesBufSize = cfg.keyFramesBufSize;
    config->panoramaBufSize = cfg.panoramaBufSize;
    
	retcode = g_ip_pano_lib->CreateHeap(heapBuffer, params.heapSize, &g_ip_pano_heap);
	if( retcode != GTS_OK ){
		return	ip_pano_convert_retcode( retcode );
	}

	retcode = g_ip_pano_lib->CreateStitcher( g_ip_pano_heap, // heap object
									&cfg,						// pointer to configuration structure
									&g_ip_pano_stitcher		// to receive the pointer to stitcher object
									);
	if( retcode != GTS_OK ){
		return	ip_pano_convert_retcode( retcode );
	}


	if( g_ip_pano_useResource == 2 )
	{
	    memset(&visCfg, 0, sizeof(AHIP_VIS_CONFIG));
	    visCfg.size.width  = config->workSize.width;
	    visCfg.size.height = config->workSize.height;
	    visCfg.crop.width  = config->workCrop.width;
	    visCfg.crop.height = config->workCrop.height;
	    visCfg.crop.left   = config->workCrop.left;
	    visCfg.crop.top    = config->workCrop.top;

	    visCfg.maxDispH    = 2;
	    visCfg.maxDispL    = 4;
	    visCfg.subPrec     = 8;
	    visCfg.minOverlap  = 10;

		visHWSize = visCfg.size.width * visCfg.size.height * 12 + 8192;
		retcode = AhipVisStart(&visCfg, config->hwBuffer, visHWSize);
	}


	return	ip_pano_convert_retcode( retcode );
#else
	return	D_IP_PANO_OK;
#endif
}


INT32 IP_PANO_Close( VOID )
{
#ifndef D_IP_PANO_STUB
	if (g_ip_pano_stitcher != NULL)		g_ip_pano_stitcher->vft->Destroy(g_ip_pano_stitcher);
	if (g_ip_pano_heap != NULL)			g_ip_pano_heap->vft->Destroy(g_ip_pano_heap);


	if ( g_ip_pano_state != E_IP_PANO_SEQ_OPEN &&
		 g_ip_pano_state != E_IP_PANO_SEQ_CTRL ){
		return D_IP_PANO_NG_SEQUENCE;
	}
	g_ip_pano_state = E_IP_PANO_SEQ_IDLE;

	if( g_ip_pano_useResource == 2 )	{
		AhipVisStop();
	}

	return D_IP_PANO_OK;
#else
	return	D_IP_PANO_OK;
#endif
}


INT32	IP_PANO_GetBufferSize(T_IP_PANO_CONFIG *config)
/*
呼び出す前に、T_IP_PANO_PARAMの以下のメンバの値をセットしておく必要がある。
    UINT32 flags;
    UINT32 frameFormat;
    GT_SIZE frameSize;
    UINT32 frameOrientation;
    UINT32 focalLength;
    UINT32 direction;
    GT_RECT workCrop;
    GT_SIZE workSize;
*/
{
#ifndef D_IP_PANO_STUB
	INT32		retcode;
	PAN_CFG		cfg;
	PAN_PARAMS	params;


    cfg.structSize = sizeof(cfg);
    cfg.flags             = config->flags;
    cfg.frameFormat       = config->frameFormat;
    cfg.frameSize.width   = config->frameSize.width;
    cfg.frameSize.height  = config->frameSize.height;
    cfg.frameOrientation  = config->frameOrientation;
    cfg.focalLength       = config->focalLength;
    cfg.opticalCenter.x   = config->frameSize.width / 2;
    cfg.opticalCenter.y   = config->frameSize.height / 2;
    cfg.direction         = config->direction;
    cfg.workCrop.left     = config->workCrop.left;
    cfg.workCrop.top      = config->workCrop.top;
    cfg.workCrop.width    = config->workCrop.width;
    cfg.workCrop.height   = config->workCrop.height;
    cfg.workSize.width    = config->workSize.width;
    cfg.workSize.height   = config->workSize.height;
    cfg.maxDisplacement   = config->maxDisplacement;
    cfg.panoramaLength    = config->panoramaLength;
    cfg.panoramaBufSize   = config->panoramaBufSize;
    cfg.keyFramesBufSize  = config->keyFramesBufSize;
    cfg.transformStart    = (PAN_TRANSFORM_START_ROUTINE)config->transformStart;
    cfg.transformComplete = (PAN_TRANSFORM_COMPLETE_ROUTINE)config->transformComplete;
    cfg.transformContext  = (void*)gIp_ge_ctx;

	params.structSize = sizeof params;

	g_ip_pano_lib = PanLibGet();

	retcode = g_ip_pano_lib->GetParams(&cfg, &params);

	config->heapSize         = params.heapSize;
	config->keyFramesBufSize = params.keyFramesBufSize;
	config->panoramaBufSize  = params.panoramaBufSize;

	if( retcode < 0 )	{
		return	ip_pano_convert_retcode( retcode );
	}

	return	retcode;
#else
	return	D_IP_PANO_OK;
#endif
}


INT32	IP_PANO_ProcessGyro(T_IP_PANO_IMAGE *frame, UINT32 timeStamp, T_IP_PANO_GYRO_SAMPLE *sample, UINT32 isLast)
{
#ifndef D_IP_PANO_STUB
	INT32		retcode;


	if( g_ip_pano_useResource!=1 ) {
		return	D_IP_PANO_NG;
	}

	g_ip_pano_stitcher->vft->AddGyroSample(g_ip_pano_stitcher, timeStamp, (PAN_IMU_SAMPLE *)sample);

	retcode = g_ip_pano_stitcher->vft->ProcessFrame(g_ip_pano_stitcher, (GT_IMAGE *)frame, NULL, NULL, timeStamp, isLast);

	return	ip_pano_convert_retcode( retcode );
#else
	return	D_IP_PANO_OK;
#endif
}


#if 0
INT32	IP_PANO_ProcessImage(T_IP_PANO_IMAGE *frame, T_IP_PANO_IMAGE *workImage, T_IP_PANO_VECTOR *motion, UINT32 isLast)
{
#ifndef D_IP_PANO_STUB
	INT32		retcode;


	if( g_ip_pano_useResource!=2 ) {
		return	D_IP_PANO_NG;
	}

	retcode = g_ip_pano_stitcher->vft->ProcessFrame(g_ip_pano_stitcher, (GT_IMAGE *)frame, (GT_IMAGE *)workImage, (PAN_VECTOR *)motion, 0, isLast);

	return	retcode;
#else
	return	D_IP_PANO_OK;
#endif
}
#else
INT32	IP_PANO_ProcessImageStart(T_IP_PANO_IMAGE *frame, T_IP_PANO_IMAGE *workImage )
{
#ifndef D_IP_PANO_STUB
	GT_IMAGE		inFrame;
	AHIP_VIS_DATA	ahipData;
	INT32			retcode;
	static const AHIP_VIS_VECTOR motionNull[256] = {{0}};


	if( g_ip_pano_useResource!=2 ) {
		return	D_IP_PANO_NG;
	}

	memcpy( &inFrame, frame, sizeof(inFrame) );
	inFrame.format = ip_pano_fmt_gt2ahip( frame->format );

	retcode = AhipProcessFrame( &inFrame );
	if( retcode != GTS_OK )	{
		return	ip_pano_convert_retcode( retcode );
	}

	ahipData.vector = motionNull;

	retcode = g_ip_pano_stitcher->vft->ProcessFrame(g_ip_pano_stitcher, (GT_IMAGE *)frame, NULL, (PAN_VECTOR *)ahipData.vector, 0, 0);

	return	ip_pano_convert_retcode( retcode );
#else
	return	D_IP_PANO_OK;
#endif
}


INT32	IP_PANO_ProcessImage(T_IP_PANO_IMAGE *frame, T_IP_PANO_IMAGE *workImage, UINT32 isLast)
{
#ifndef D_IP_PANO_STUB
	GT_IMAGE		inFrame;
	AHIP_VIS_DATA	ahipData;
	INT32			retcode;


	if( g_ip_pano_useResource!=2 ) {
		return	D_IP_PANO_NG;
	}

	memcpy( &inFrame, frame, sizeof(inFrame) );
	inFrame.format = ip_pano_fmt_gt2ahip( frame->format );

	retcode = AhipProcessFrame( &inFrame );
	if( retcode != GTS_OK )	{
		return	ip_pano_convert_retcode( retcode );
	}

	retcode = AhipVisIO( &ahipData );
	if( retcode != GTS_OK )	{
		return	ip_pano_convert_retcode( retcode );
	}

	retcode = g_ip_pano_stitcher->vft->ProcessFrame(g_ip_pano_stitcher, (GT_IMAGE *)frame, NULL, (PAN_VECTOR *)ahipData.vector, 0, isLast);

	return	ip_pano_convert_retcode( retcode );
#else
	return	D_IP_PANO_OK;
#endif
}
#endif


INT32	IP_PANO_Stitch(T_IP_PANO_IMAGE* results)
{
#ifndef D_IP_PANO_STUB
	INT32	retcode;

	retcode = g_ip_pano_stitcher->vft->Stitch(g_ip_pano_stitcher, (GT_IMAGE *)results);
	
	return	ip_pano_convert_retcode( retcode );
#else
	return	D_IP_PANO_OK;
#endif
}


INT32	IP_PANO_GetDirection(VOID)
{
#ifndef D_IP_PANO_STUB
	INT32	retcode;

	retcode = g_ip_pano_stitcher->vft->GetDirection(g_ip_pano_stitcher);

	return	retcode;
#else
	return	D_IP_PANO_OK;
#endif
}


INT32	IP_PANO_GetOffset(T_IP_PANO_SIZE *offset)
{
#ifndef D_IP_PANO_STUB
	INT32	retcode;

	retcode = g_ip_pano_stitcher->vft->GetOffset( g_ip_pano_stitcher, (GT_SIZE *)offset );

	return	retcode;
#else
	return	D_IP_PANO_OK;
#endif
}
