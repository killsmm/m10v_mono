/**
 * @file		ip_eis.c
 * @brief		EIS wrapper
 * @author		yamauchi
 * @since		2016/04/15
 * @date		2017/03/23
 * @note		EIS wrapper
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#include <string.h>
#include "gtypes.h"
#include "ddim_typedef.h"
#include "LibDeVIS.h"
#include "ahip.h"
#include "ip_eis.h"
#include "ip_ahip.h"

extern INT32 IP_EIS_LL_Init( const T_IP_EIS_PARAM *param );
extern void IP_EIS_LL_Close( VOID );

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
#define V_Ipc_EIS_Version		"fn_eis_version:V01L02;\r\n"

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/
enum	E_IP_EIS_STATE {
	E_IP_EIS_SEQ_CLOSE = 0,
	E_IP_EIS_SEQ_OPEN = 1,
	E_IP_EIS_SEQ_INIT = 2,
	E_IP_EIS_SEQ_CTRL = 3,
	E_IP_EIS_SEQ_PROCESS = 4,
	E_IP_EIS_SEQ_STICH = 5,
};


/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/
static  GT_HEAP     *g_ip_eis_heap  ;

static  UINT32      g_ip_eis_state      = E_IP_EIS_SEQ_CLOSE;
static  DVS_LIB     *g_ip_eis_lib       = NULL;
static  DVS_FILTER  *g_ip_eis_filter    = NULL;
static  UINT32      g_ip_eis_useResource=0; //  1:Gyro / 2:HRE
static  BOOL        g_ip_eis_enabled = FALSE;
static  UINT32      g_ip_eis_fullWidth;
static  BOOL        g_ip_eis_low_latency_mode = FALSE;
static  UINT32      g_ip_eis_init_flag = 0;
static  UINT32      g_ip_eis_gyro_prevTS = 0;
static  UINT32      g_ip_eis_gyro_upperTS = -1;
static  UINT32      g_ip_eis_frame_prevTS = 0;
static  UINT32      g_ip_eis_frame_upperTS = -1;
static	UCHAR		g_ip_eis_pipe_use_num = 1;
/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Local Method Definition																*/
/*--------------------------------------------------------------------------------------*/
INT32 ip_eis_convert_retcode(INT32 status)
{
	INT32		retcode=0;

	switch( status )
	{
		case	GTS_OK:
			retcode = D_IP_EIS_OK;
			break;

		case	GTS_FAIL:
			retcode = D_IP_EIS_NG;
			break;

		case	GTS_CANCELLED:
			retcode = D_IP_EIS_NG_CANCELLED;
			break;

		case	GTS_NO_SYSTEM_RESOURCES:
			retcode = D_IP_EIS_NG_NO_SYSTEM_RESOURCES;
			break;

		case	GTS_INVALID_ARG:
			retcode = D_IP_EIS_NG_INVALID_ARG;
			break;

		case	GTS_NOT_SUPPORTED:
			retcode = D_IP_EIS_NG_NOT_SUPPORTED;
			break;

		case	GTS_FMT_NOT_SUPPORTED:
			retcode = D_IP_EIS_NG_FMT_NOT_SUPPORTED;
			break;

		case	GTS_NOT_IMPLEMENTED:
			retcode = D_IP_EIS_NG_NOT_IMPLEMENTED;
			break;

		case	GTS_BAD_STRUCT_SIZE:
			retcode = D_IP_EIS_NG_BAD_STRUCT_SIZE;
			break;

		case	GTS_BAD_IMAGE_SIZE:
			retcode = D_IP_EIS_NG_BAD_IMAGE_SIZE;
			break;

		case	GTS_DVS_IN_PROGRESS:
			retcode = D_IP_EIS_IN_PROGRESS;
			break;

		case	GTS_DVS_GRID_NOT_READY:
			retcode = D_IP_EIS_GRID_NOT_READY;
			break;

		default:
			retcode = D_IP_EIS_NG_UNKNOWN;
			break;
	}

	return	retcode;
}


INT32 ip_eis_convert_format(INT32 format)
{
	INT32		retcode;

	switch( format )
	{
		case	E_IP_EIS_FMT_YUV_420_P3:
			retcode = AHIP_FMT_YUV_420P;
			break;

		case	E_IP_EIS_FMT_YUV_422_P3:
			retcode = AHIP_FMT_YUV_422P;
			break;

		case	E_IP_EIS_FMT_YUV_420_P2:
			retcode = AHIP_FMT_YUV_420SP;
			break;

		case	E_IP_EIS_FMT_YUV_422_P2:
			retcode = AHIP_FMT_YUV_422SP;
			break;

		default:
			retcode = D_IP_EIS_NG_FORMAT;
			break;
	}

	return	retcode;
}

static VOID ip_eis_GetHREWorkingSize(const AHIP_RECT* inputCrop, AHIP_SIZE* workingSize)
{
    // maintain aspect ratio as the filtering may be sensitive to non-square pixels
    if (inputCrop->width > inputCrop->height)
    {
        workingSize->width = inputCrop->width / 2 < 640 ? inputCrop->width / 2 : 640;
        workingSize->width &= ~3; // align to 4
        workingSize->height = inputCrop->height * workingSize->width / inputCrop->width;
        workingSize->height &= ~1; // align to 2
    }
    else
    {
        workingSize->height = inputCrop->height < 640 ? inputCrop->height : 640;
        workingSize->height &= ~1; // align to 2
        workingSize->width = inputCrop->width * workingSize->height / inputCrop->height;
        workingSize->width &= ~3; // align to 4
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
	Gets the EIS library version.
*/
VOID IP_EIS_Get_Version(CHAR** str)
{
	*str = (CHAR*)strchr( V_Ipc_EIS_Version, ':' ) + 1;
	return;
}

// There seems to be 1 frame delay with respect to motion vectors
// So we use the ones for the previous frame
static DVS_VECTOR s_visVector[256] = {{0}};

INT32 IP_EIS_Open(INT32 id, VOID *heapBuffer, T_IP_EIS_PARAM *param, BOOL gyroEnable)
{
#ifndef D_IP_EIS_STUB
	INT32				retcode;
	UINT32				heapsize;
	UINT32				bufferSize;
	AHIP_VIS_CONFIG		ahipVisCfg;


	if ( g_ip_eis_state != E_IP_EIS_SEQ_CLOSE ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	param->reserve = sizeof(T_IP_EIS_PARAM);
	param->cameraInfo.reserve = sizeof(T_IP_EIS_CAMERA_INFO);
	g_ip_eis_fullWidth = param->originalSize.width;

	if(param->flags & E_IP_EIS_FLAG_USE_GYRO)	{
		g_ip_eis_useResource = 1 ;
	}
	else	{
		g_ip_eis_useResource = 2 ;
	}

	g_ip_eis_state = E_IP_EIS_SEQ_OPEN;
	g_ip_eis_heap = NULL;
	g_ip_eis_filter = NULL;

#if 1
	g_ip_eis_lib = DvsLibGet();
#endif

	heapsize = g_ip_eis_lib->GetRequiredHeapSize((DVS_CFG *)param);
	if( heapsize == 0 )	{
		return	D_IP_EIS_NG_INVALID_ARG;
	}

	if( heapBuffer != NULL )	{
		retcode = g_ip_eis_lib->CreateHeap( (void *)heapBuffer,		// heap object
										heapsize,					// pointer to configuration structure
										(GT_HEAP **)&g_ip_eis_heap	// to receive the pointer to stitcher object
										);
		if( retcode != GTS_OK )	{
			return	ip_eis_convert_retcode( retcode );
		}
	}

//	g_ip_eis_heap = (void *)heapBuffer;
	retcode = g_ip_eis_lib->CreateFilter(g_ip_eis_heap, (DVS_CFG *)param, &g_ip_eis_filter);
	if( retcode != GTS_OK )	{
		return	ip_eis_convert_retcode( retcode );
	}

	if ( g_ip_eis_low_latency_mode ) {
//		retcode = IP_EIS_LL_Init(param);
	}
	else {
		if( g_ip_eis_useResource == 2 )	{
			if( IP_AHIP_Get_Open_Status( id ) == FALSE ){
				T_IP_AHIP_PREVIEW_CFG	cfg;

				// AHIP open (Preview mode).
				if ( gyroEnable ) {
					cfg.flags       = E_IP_AHIP_FLAG_PRE;
				}
				else {
					cfg.flags       = E_IP_AHIP_FLAG_PRE | E_IP_AHIP_FLAG_VIS;
				}
				cfg.fmt             = E_IP_AHIP_FMT_YUV420P; 

				if(g_ip_eis_pipe_use_num >= 2)
				{
					cfg.mode            = E_IP_AHIP_PRV_MODE_SENSOR_AB_MERGED;
					cfg.size_a.width    = param->inputCrop.width / 2;
					cfg.size_a.lines    = param->inputCrop.height;
					cfg.size_b.width    = param->inputCrop.width / 2;
					cfg.size_b.lines    = param->inputCrop.height;
				}
				else
				{
					cfg.mode            = E_IP_AHIP_PRV_MODE_SENSOR_A;
					cfg.size_a.width    = param->inputCrop.width;
					cfg.size_a.lines    = param->inputCrop.height;
					cfg.size_b.width    = param->inputCrop.width;
					cfg.size_b.lines    = param->inputCrop.height;
				}
				
				retcode = IP_AHIP_OpenPreview( id, &cfg );
				if ( retcode != D_IP_AHIP_OK)
					return	ip_eis_convert_retcode( retcode );	
			}

			if(g_ip_eis_pipe_use_num >= 2)
			{
				ahipVisCfg.crop.left	= (param->stabLimits.left / 2) & ~1; // Merge mode
				ahipVisCfg.crop.top		= param->stabLimits.top;
				ahipVisCfg.crop.width	= (param->stabLimits.width / 2) & ~1; // Merge mode
				ahipVisCfg.crop.height	= param->stabLimits.height;
			}
			else
			{
				ahipVisCfg.crop.left	= param->stabLimits.left & ~1;
				ahipVisCfg.crop.top		= param->stabLimits.top;
				ahipVisCfg.crop.width	= param->stabLimits.width & ~1;
				ahipVisCfg.crop.height	= param->stabLimits.height;
			}
			
			ip_eis_GetHREWorkingSize((AHIP_RECT *)&param->stabLimits, &ahipVisCfg.size);
			ahipVisCfg.maxDispH		= 2;
			ahipVisCfg.maxDispL		= 4;
			ahipVisCfg.subPrec		= 8;
			ahipVisCfg.minOverlap	= 10;
			bufferSize = ahipVisCfg.size.width * ahipVisCfg.size.height * 12 + 8192;
			retcode = AhipVisStart( &ahipVisCfg, param->ipuBuffer, bufferSize );

			memset(s_visVector, 0, sizeof(s_visVector));
		}
	}

	g_ip_eis_init_flag = 0;
	g_ip_eis_gyro_upperTS = 0;
	g_ip_eis_frame_upperTS = 0;
	g_ip_eis_gyro_prevTS = 0;
	g_ip_eis_frame_prevTS = 0;

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}


INT32 IP_EIS_Close( INT32 id )
{
#ifndef D_IP_EIS_STUB
//	if ( g_ip_eis_low_latency_mode )
//		IP_EIS_LL_Close();

	if (NULL != g_ip_eis_filter)		g_ip_eis_filter->vft->Destroy(g_ip_eis_filter);
	if (NULL != g_ip_eis_heap)			g_ip_eis_heap->vft->Destroy(g_ip_eis_heap);
	g_ip_eis_lib	= NULL;
	g_ip_eis_filter = NULL;
	g_ip_eis_heap   = NULL;

	if( g_ip_eis_useResource == 2 )	{
		    AhipVisStop();
	}

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN &&
		 g_ip_eis_state != E_IP_EIS_SEQ_CTRL ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	g_ip_eis_init_flag = 0;
	g_ip_eis_state = E_IP_EIS_SEQ_CLOSE;

	return D_IP_EIS_OK;
#else
	return	D_IP_EIS_OK;
#endif
}


UINT32	IP_EIS_GetBufferSize(T_IP_EIS_PARAM *param)
{
#ifndef D_IP_EIS_STUB
	INT32		retcode;

	param->reserve = sizeof(T_IP_EIS_PARAM);
	param->cameraInfo.reserve = sizeof(T_IP_EIS_CAMERA_INFO);

	retcode = g_ip_eis_lib->GetRequiredHeapSize((DVS_CFG *)param);

	return	retcode ;
#else
	return	D_IP_EIS_OK;
#endif
}


INT32	IP_EIS_StartFrame(INT32 id, T_IP_EIS_IMAGE *image, T_IP_EIS_FRAMEINFO *frameinfo)
{
#ifndef D_IP_EIS_STUB
	INT32			retcode = D_IP_EIS_OK;
	AHIP_VIS_DATA	visData;
	DVS_FRAME_INFO	dvsfi;


	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	if( (g_ip_eis_init_flag & 2)==0 )	{
		g_ip_eis_frame_upperTS = 0;
		g_ip_eis_init_flag |= 2;
	}
	else if( g_ip_eis_frame_prevTS > frameinfo->timeStamp )	{
		g_ip_eis_frame_upperTS++;
	}
	g_ip_eis_frame_prevTS  = frameinfo->timeStamp;

	dvsfi.structSize   = sizeof(dvsfi);
	dvsfi.timeStamp    = frameinfo->timeStamp | ((UINT64)g_ip_eis_frame_upperTS<<32);
	dvsfi.readout      = frameinfo->readoutTime;
	dvsfi.exposureTime = frameinfo->exposureTime;

	if( g_ip_eis_useResource == 2 )	{		//	using HRE

		retcode = AhipVisIO( &visData );
		if (retcode)
			return D_IP_EIS_NG_UNKNOWN;

		// There seems to be 1 frame delay with respect to motion vectors
		// So we use the ones for the previous frame
		retcode = g_ip_eis_filter->vft->StartFrameProcessing(
								g_ip_eis_filter,
								(GT_IMAGE *)image,
								(DVS_VECTOR *)s_visVector,
								&dvsfi );

		memcpy(s_visVector, visData.vector, 256 * sizeof(DVS_VECTOR));
	}
	else if( g_ip_eis_useResource == 1 )	{		//	using gyro
		retcode = g_ip_eis_filter->vft->StartFrameProcessing(
								g_ip_eis_filter,
								(GT_IMAGE *)image,
								(DVS_VECTOR *)NULL,
								&dvsfi );
	}

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}


INT32	IP_EIS_AddGyroData(INT32 id, T_IP_EIS_GYRO_DATA *gyroData)
{
#ifndef D_IP_EIS_STUB
	INT32		retcode;
static	UINT64		time_stamp;

	if (g_ip_eis_useResource == 2)
		return D_IP_EIS_OK;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	if( (g_ip_eis_init_flag & 1)==0 )	{
		g_ip_eis_gyro_upperTS = 0;
		g_ip_eis_init_flag |= 1;
	}
	else if( g_ip_eis_gyro_prevTS > gyroData->timeStamp )	{
		g_ip_eis_gyro_upperTS++;
	}
	g_ip_eis_gyro_prevTS  = gyroData->timeStamp;

	time_stamp = ((UINT64)g_ip_eis_gyro_upperTS<<32) | gyroData->timeStamp;

	retcode = g_ip_eis_filter->vft->AddIMUSample( g_ip_eis_filter,
							(UINT64)time_stamp,
							(UINT32)gyroData->exposureTime,
							(DVS_IMU_SAMPLE*)(gyroData->gyro),
							(DVS_IMU_SAMPLE*)(gyroData->accel),
							(DVS_IMU_SAMPLE*)(gyroData->magn)
							);

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}


INT32	IP_EIS_Process( INT32 id )
{
#ifndef D_IP_EIS_STUB
	INT32		retcode;


	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->ProcessFrame(g_ip_eis_filter);

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}


INT32	IP_EIS_GetCorrectionGrid( INT32 id, UINT32 lineBegin, UINT32 lineEnd, T_IP_EIS_NODE *grid)
{
#ifndef D_IP_EIS_STUB
	INT32		retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->GetCorrectionGrid( g_ip_eis_filter,
															lineBegin,
															lineEnd,
															(DVS_NODE *)grid);

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}


INT32	IP_EIS_GetCorrectionMatrix( INT32 id, FLOAT matrix[3][3] )
{
#ifndef D_IP_EIS_STUB
	INT32		retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->GetCorrectionMatrix( g_ip_eis_filter, matrix);

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}


INT32	IP_EIS_GetDisplacementMatrix( INT32 id, FLOAT matrix[3][3] )
{
#ifndef D_IP_EIS_STUB
	INT32		retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->GetDisplacementMatrix( g_ip_eis_filter, matrix );

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}


UINT32	IP_EIS_GetImageLines(INT32 id)
{
#ifndef D_IP_EIS_STUB
	INT32	retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->GetImageLines( g_ip_eis_filter );

	return	retcode;
#else
	return	D_IP_EIS_OK;
#endif
}


UINT32	IP_EIS_GetGridLines(INT32 id)
{
#ifndef D_IP_EIS_STUB
	INT32	retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->GetGridLines( g_ip_eis_filter );

	return	retcode;
#else
	return	D_IP_EIS_OK;
#endif
}


INT32	IP_EIS_GetDisplacementOffset(INT32 id, T_IP_EIS_MOTION *motion)
{
#ifndef D_IP_EIS_STUB
	INT32	retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->GetDisplacementOffset( g_ip_eis_filter, (float *)&(motion->dx), (float *)&(motion->dy) );

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}


INT32	IP_EIS_GetStabilizedOffset(INT32 id, T_IP_EIS_MOTION *motion)
{
#ifndef D_IP_EIS_STUB
	INT32	retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->GetStabilizedOffset( g_ip_eis_filter, (float *)&(motion->dx), (float *)&(motion->dy) );

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}

INT32   IP_EIS_StabilizeRectangle( INT32 id, const T_IP_EIS_RECT *in, T_IP_EIS_RECT *out, BOOL split_enabled)
{
	GT_POINT points_in[] = {
		{in->left, in->top},
		{in->left + in->width, in->top},
		{in->left, in->top + in->height},
		{in->left + in->width, in->top + in->height}
	};

	GT_POINT points_out[4] = {{0}};

	*out = *in;

	if (!g_ip_eis_filter)
		return 0;

	if (g_ip_eis_useResource == 2)
		return 0;

	for (int i = 0; i < 4; i++)
	{
		if (!split_enabled || points_in[i].x < g_ip_eis_fullWidth / 2)
		{
			GTSTATUS status = g_ip_eis_filter->vft->GetStabilizedPoint(g_ip_eis_filter, &points_in[i], &points_out[i]);
			if (status)
				return ip_eis_convert_retcode(status);
		}
		else
		{
			points_out[i] = points_in[i];
		}
	}

	INT32 minX = points_out[0].x;
	INT32 maxX = points_out[0].x;
	INT32 minY = points_out[0].y;
	INT32 maxY = points_out[0].y;
	for (int i = 1; i < 4; i++)
	{
		if (minX > points_out[i].x)
			minX = points_out[i].x;
		if (maxX < points_out[i].x)
			maxX = points_out[i].x;
		if (minY > points_out[i].y)
			minY = points_out[i].y;
		if (maxY < points_out[i].y)
			maxY = points_out[i].y;
	}

	out->left = minX;
	out->top = minY;
	out->width = maxX - minX + 1;
	out->height = maxY - minY + 1;

	return D_IP_EIS_OK;
}

INT32	IP_EIS_SetEnabled(INT32 id, BOOL enabled)
{
	g_ip_eis_enabled = enabled;

	return	D_IP_EIS_OK;
}

BOOL	IP_EIS_GetEnabled(INT32 id)
{
	return	g_ip_eis_enabled;
}

BOOL	IP_EIS_GetLowLatencyModeEnabled(INT32 id)
{
	return	g_ip_eis_low_latency_mode;
}

VOID IP_EIS_SetLowLatencyModeEnabled(INT32 id, BOOL onoff)
{
	g_ip_eis_low_latency_mode = onoff;
}

INT32 IP_EIS_SetFlag(INT32 id, UINT32 inFlag, UINT32 inValue)
{
#ifndef D_IP_EIS_STUB
	INT32	retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->SetFlag( g_ip_eis_filter, inFlag, inValue );

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}

INT32 IP_EIS_SetStrength(INT32 id, FLOAT minStrength, FLOAT maxStrength)
{
#ifndef D_IP_EIS_STUB
	INT32	retcode;

	if ( g_ip_eis_state != E_IP_EIS_SEQ_OPEN ){
		return D_IP_EIS_NG_SEQUENCE;
	}

	retcode = g_ip_eis_filter->vft->SetStrength( g_ip_eis_filter, (float)minStrength, (float)maxStrength );

	return	ip_eis_convert_retcode( retcode );
#else
	return	D_IP_EIS_OK;
#endif
}

VOID IP_EIS_SetPipeNum(INT32 id, UCHAR pipe_use_num)
{
	g_ip_eis_pipe_use_num = pipe_use_num;
}


