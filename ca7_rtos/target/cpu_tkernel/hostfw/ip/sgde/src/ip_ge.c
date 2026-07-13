/**
 * @file		ip_ge.c
 * @brief		SGDE wrapper
 * @author		yamauchi
 * @since		2016/05/18
 * @date		2016/06/02
 * @note		SGDE wrapper
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "gtypes.h"
#include "ahip.h"
#include "ip_ge.h"
#include "ip_ge_user_custom.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
#define V_Ip_GE_Version		"ip_ge_version:V01L00;\r\n"

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/
AHIP_GDC_CONTEXT *gIp_ge_ctx;

static	UINT32			gIp_ge_state;
static	AHIP_GDC_GNODE	gIp_ge_gnodes[1024];

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Local Method Definition																*/
/*--------------------------------------------------------------------------------------*/
INT32 ip_ge_convert_retcode(INT32 status)
{
	INT32		retcode=0;


	switch( status )
	{
		case	GTS_OK:
			retcode = D_IP_GE_OK;
			break;

		case	    GTS_INVALID_MODE:
			retcode = D_IP_GE_NG_INVALID_MODE;
			break;

		case	    GTS_HW_ERROR:
			retcode = D_IP_GE_NG_HW;
			break;

		case	    GTS_BUFFER_TOO_SMALL:
			retcode = D_IP_GE_NG_BUFFER_TOO_SMALL;
			break;

		case	    GTS_BUSY:
			retcode = D_IP_GE_NG_BUSY;
			break;

		case	    GTS_STOPPED:
			retcode = D_IP_GE_NG_STOPPED;
			break;

		case	    GTS_WAIT:
			retcode = D_IP_GE_NG_WAIT;
			break;

		case	GTS_FAIL:
			retcode = D_IP_GE_NG_FAIL;
			break;

		case	GTS_CANCELLED:
			retcode = D_IP_GE_NG_CANCELLED;
			break;

		case	GTS_NO_SYSTEM_RESOURCES:
			retcode = D_IP_GE_NG_NO_SYSTEM_RESOURCES;
			break;

		case	GTS_INVALID_ARG:
			retcode = D_IP_GE_NG_INVALID_ARG;
			break;

		case	GTS_NOT_SUPPORTED:
			retcode = D_IP_GE_NG_NOT_SUPPORTED;
			break;

		case	GTS_FMT_NOT_SUPPORTED:
			retcode = D_IP_GE_NG_FMT_NOT_SUPPORTED;
			break;

		case	GTS_NOT_IMPLEMENTED:
			retcode = D_IP_GE_NG_NOT_IMPLEMENTED;
			break;

		case	GTS_BAD_STRUCT_SIZE:
			retcode = D_IP_GE_NG_BAD_STRUCT_SIZE;
			break;

		case	GTS_BAD_IMAGE_SIZE:
			retcode = D_IP_GE_NG_BAD_IMAGE_SIZE;
			break;

		default:
			retcode = D_IP_GE_NG_UNKNOWN;
			break;
	}

	return	retcode;
}

/*
INT32 ip_ge_convert_format(INT32 format)
{
	INT32		retcode;


	switch( format )
	{
		case	E_IP_GE_FMT_YUV_420_P3:
			retcode = AHIP_FMT_YUV_420P;
			break;

		case	E_IP_GE_FMT_YUV_422_P3:
			retcode = AHIP_FMT_YUV_422P;
			break;

//		case	:
//			retcode = AHIP_FMT_YUV_444P;
//			break;

		case	E_IP_GE_FMT_YUV_420_P2:
			retcode = AHIP_FMT_YUV_420SP;
			break;

		case	E_IP_GE_FMT_YUV_422_P2:
			retcode = AHIP_FMT_YUV_422SP;
			break;

//		case	GTS_NOT_SUPPORTED:
//			retcode = AHIP_FMT_YUV_420B;
			break;
		default:
			retcode = D_IP_GE_NG_FORMAT;
			break;
	}

	return	retcode;
}
*/

VOID	ip_ge_conv_float2fix( T_IP_GE_GNODE *ge_nodes, INT32 num, AHIP_GDC_GNODE *ge_nodes_fix )
{
	int	lp;


    for (lp = 0; lp < num; lp++)
    {
        ge_nodes_fix->u = ((INT32)((ge_nodes->u)*(1<<6)) +1 ) >> 1;
        ge_nodes_fix->v = ((INT32)((ge_nodes->v)*(1<<6)) +1 ) >> 1;

        ge_nodes++;
        ge_nodes_fix++;
    }
}

#if 0
VOID	ip_ge_flush_image_plane( T_IP_GE_IMAGE *img )
{
	UINT32	size;


	size = img->planes[0].stride*img->height;
	IP_GE_User_CleanFlush_Dcache( (UINT32)img->planes[0].data, size );

	if( img->format==E_IP_GE_FMT_YUV_420_P2 || img->format==E_IP_GE_FMT_YUV_420_P3)	{
		size = img->planes[1].stride*img->height/2;
	}
	else	{
		size = img->planes[1].stride*img->height;
	}
	IP_GE_User_CleanFlush_Dcache( (UINT32)img->planes[1].data, size );

	if( img->format==E_IP_GE_FMT_YUV_422_P3 || img->format==E_IP_GE_FMT_YUV_420_P3)	{
		IP_GE_User_CleanFlush_Dcache( (UINT32)img->planes[2].data, size );
	}
}
#endif

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
VOID IP_GE_Get_Version(BYTE** str)
{
	*str = (BYTE*)strchr( V_Ip_GE_Version, ':' ) + 1;
	return;
}


INT32	IP_GE_Open(T_IP_GE_CONFIG *config, VOID *buffer, INT32 size)
{
#ifndef D_IP_GE_STUB
	INT32		retcode;


	if ( gIp_ge_state != E_IP_GE_SEQ_IDLE ){
		return D_IP_GE_NG_SEQUENCE;
	}

	gIp_ge_state = E_IP_GE_SEQ_OPEN;
	gIp_ge_ctx = NULL ;

	retcode = AhipGdcStart();
	if( retcode != GTS_OK )	{
		return	ip_ge_convert_retcode( retcode );
	}

	retcode = AhipGdcCreateContext( (AHIP_GDC_CONFIG*)config, buffer, size, (AHIP_GDC_CONTEXT**)&gIp_ge_ctx );
	if( retcode != GTS_OK )	{
        if( gIp_ge_ctx != NULL )	{
        	AhipGdcFreeContext( (AHIP_GDC_CONTEXT*)gIp_ge_ctx );
        }
        if( buffer != NULL )	{
//	        free( buffer );
	    }

		return	ip_ge_convert_retcode( retcode );
	}

	return	D_IP_GE_OK;
#else
	return	D_IP_GE_OK;
#endif
}


INT32 IP_GE_Close( VOID )
{
#ifndef D_IP_GE_STUB
    if( gIp_ge_ctx != NULL )	{
    	AhipGdcFreeContext( gIp_ge_ctx );
    }

    AhipGdcStop();

	gIp_ge_state = E_IP_GE_SEQ_IDLE;

	return	D_IP_GE_OK;
#else
	return	D_IP_GE_OK;
#endif
}


INT32	IP_GE_SetCorrectParameter( T_IP_GE_GGRID *global, UINT32 numLocal, T_IP_GE_LGRID *local, FLOAT xform[9])
//	for low latency mode
{
#ifndef D_IP_GE_STUB
	INT32			retcode;
	AHIP_GDC_GGRID	ggrid;


	if( local==NULL )	{
		return	D_IP_GE_NG_ARG;
	}

	if( global != NULL )	{
		ggrid.width   = global->width;
		ggrid.height  = global->height;
		ggrid.pitchX  = global->pitchX;
		ggrid.pitchY  = global->pitchY;
		ggrid.offsetX = global->offsetX;
		ggrid.offsetY = global->offsetY;
		ggrid.nodes   = NULL;
		
		if (global->nodes)
		{
			ggrid.nodes = (AHIP_GDC_GNODE*)gIp_ge_gnodes;
			ip_ge_conv_float2fix( global->nodes, global->width*global->height, (AHIP_GDC_GNODE*)ggrid.nodes );

			IP_GE_User_CleanFlush_Dcache( (UINT32)global->nodes, global->width*global->height*8 );
		}
		retcode = AhipGdcSetGlobal( (AHIP_GDC_CONTEXT*)gIp_ge_ctx, &ggrid );
	}
	else	{
		retcode = AhipGdcSetGlobal( (AHIP_GDC_CONTEXT*)gIp_ge_ctx, NULL );
	}
	if( retcode != GTS_OK )	{
		return	ip_ge_convert_retcode( retcode );
	}

	for(UINT32 i=0; i < numLocal; i++){
		if( local->nodes != NULL )	{
			IP_GE_User_CleanFlush_Dcache( (UINT32)local->nodes, numLocal*8 );
		}
		if( local->xform != NULL )	{
			IP_GE_User_CleanFlush_Dcache( (UINT32)local->xform, numLocal*40 );
		}
	}

	retcode = AhipGdcSetLocal( (AHIP_GDC_CONTEXT*)gIp_ge_ctx, numLocal, (AHIP_GDC_LGRID*)local );
	if( retcode != GTS_OK )	{
		return	ip_ge_convert_retcode( retcode );
	}

	if( xform != NULL )	{
		IP_GE_User_CleanFlush_Dcache( (UINT32)xform, 3*3*4 );
	}
	retcode = AhipGdcSetTransform( (AHIP_GDC_CONTEXT*)gIp_ge_ctx, xform );

	return	ip_ge_convert_retcode( retcode );
#else
	return	D_IP_GE_OK;
#endif
}


INT32	IP_GE_SetGridData( UINT32 num, T_IP_GE_GNODE *nodes )
//	for dynamic global grid mode
{
#ifndef D_IP_GE_STUB
	INT32		retcode;

	ip_ge_conv_float2fix( nodes, num, gIp_ge_gnodes );

	IP_GE_User_CleanFlush_Dcache( (UINT32)gIp_ge_gnodes, num*8 );
	retcode = AhipGdcWriteNodes( (AHIP_GDC_CONTEXT*)gIp_ge_ctx, num, gIp_ge_gnodes );
		return	ip_ge_convert_retcode( retcode );
#else
	return	D_IP_GE_OK;
#endif
}


INT32	IP_GE_Process( T_IP_GE_IMAGE *in, T_IP_GE_IMAGE *out1, T_IP_GE_IMAGE *out2, T_IP_GE_IMAGE *out3, INT32 wait )
{
#ifndef D_IP_GE_STUB
	INT32		retcode;

	//ip_ge_flush_image_plane( in );

	retcode = AhipGdcProcess( (AHIP_GDC_CONTEXT*)gIp_ge_ctx, (GT_IMAGE*)in, (GT_IMAGE*)out1, (GT_IMAGE*)out2, (GT_IMAGE*)out3, wait );

	return	ip_ge_convert_retcode( retcode );
#else
	return	D_IP_GE_OK;
#endif
}

INT32   IP_GE_Wait( VOID )
{
#ifndef D_IP_GE_STUB
	INT32 retcode = AhipGdcWait( (AHIP_GDC_CONTEXT*)gIp_ge_ctx);

	return	ip_ge_convert_retcode( retcode );	
#else
	return	D_IP_GE_OK;
#endif
}


INT32	IP_GE_MIOProcess( T_IP_GE_IMAGE *in, T_IP_GE_IMAGE *out1, T_IP_GE_IMAGE *out2, T_IP_GE_IMAGE *out3 )
{
#ifndef D_IP_GE_STUB
	INT32		retcode;


//	ip_ge_flush_image_plane( in );

	retcode = AhipMioProcess( (GT_IMAGE*)in, (GT_IMAGE*)out1, (GT_IMAGE*)out2, (GT_IMAGE*)out3 );

	return	ip_ge_convert_retcode( retcode);
#else
	return	D_IP_GE_OK;
#endif
}
