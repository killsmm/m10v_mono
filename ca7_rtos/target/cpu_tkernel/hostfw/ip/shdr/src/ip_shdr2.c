/**
 * @file		ip_eis.c
 * @brief		EIS wrapper
 * @author		yamauchi
 * @since		2016/04/15
 * @date		2016/06/02
 * @note		EIS wrapper
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#include <stdio.h>
#include <string.h>
#include "NF_driver_wrapper.h"

#include "gtypes.h"
#include "hdr.h"
#include "ip_shdr2.h"
#include "os_user_custom.h"
#include "measure_time.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
#define V_Ipc_SHDR_Version		"shdr_version:V01L02;\r\n"

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/
static	UINT32		g_ip_shdr_state=0;
static	GT_HEAP		*g_ip_shdr_heap = NULL;
static	HDR_LIB		*g_ip_shdr_lib;
static	HDR_ENGINE	*g_ip_shdr_engine;

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Local Method Definition																*/
/*--------------------------------------------------------------------------------------*/
INT32 ip_shdr_convert_retcode(INT32 status)
{
	INT32		retcode=0;

	switch( status )
	{
		case	GTS_OK:
			retcode = D_IP_SHDR_OK;
			break;

		case	GTS_FAIL:
			retcode = D_IP_SHDR_NG;
			break;

		case	GTS_CANCELLED:
			retcode = D_IP_SHDR_NG_CANCELLED;
			break;

		case	GTS_NO_SYSTEM_RESOURCES:
			retcode = D_IP_SHDR_NG_NO_SYSTEM_RESOURCES;
			break;

		case	GTS_INVALID_ARG:
			retcode = D_IP_SHDR_NG_INVALID_ARG;
			break;

		case	GTS_NOT_SUPPORTED:
			retcode = D_IP_SHDR_NG_NOT_SUPPORTED;
			break;

		case	GTS_FMT_NOT_SUPPORTED:
			retcode = D_IP_SHDR_NG_FMT_NOT_SUPPORTED;
			break;

		case	GTS_NOT_IMPLEMENTED:
			retcode = D_IP_SHDR_NG_NOT_IMPLEMENTED;
			break;

		case	GTS_BAD_STRUCT_SIZE:
			retcode = D_IP_SHDR_NG_BAD_STRUCT_SIZE;
			break;

		case	GTS_BAD_IMAGE_SIZE:
			retcode = D_IP_SHDR_NG_BAD_IMAGE_SIZE;
			break;

		default:
			retcode = D_IP_SHDR_NG_UNKNOWN;
			break;
	}

	return	retcode;
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
VOID IP_SHDR_Get_Version(CHAR** str)
{
	*str = (CHAR*)strchr( V_Ipc_SHDR_Version, ':' ) + 1;
	return;
}


INT32	IP_SHDR_Open(VOID *heapBuffer, T_IP_SHDR_CONFIG *config)
{
#ifndef D_IP_SHDR_STUB
	INT32				retcode;
	UINT32				heapsize;


	if ( g_ip_shdr_state != E_IP_SHDR_SEQ_IDLE ){
		return D_IP_SHDR_NG_SEQUENCE;
	}

	g_ip_shdr_state = E_IP_SHDR_SEQ_OPEN;
	g_ip_shdr_heap = NULL;

	g_ip_shdr_lib = HdrLibGet();
	config->reserve = sizeof(HDR_CONFIG);

	heapsize = g_ip_shdr_lib->vft->GetRequiredHeapSize(g_ip_shdr_lib, (HDR_CONFIG *)config);

	if( heapsize == -1 )	{
		return	D_IP_SHDR_NG_PARAM;
	}

	if( heapBuffer != NULL )	{
		retcode = g_ip_shdr_lib->vft->CreateHeap( g_ip_shdr_lib,
										(void *)heapBuffer,			// heap object
										heapsize,					// pointer to configuration structure
										(GT_HEAP **)&g_ip_shdr_heap	// to receive the pointer to stitcher object
										);
		if( retcode != GTS_OK )	{
			return	ip_shdr_convert_retcode( retcode );
		}
	}

	retcode = g_ip_shdr_lib->vft->CreateHdrEngine( g_ip_shdr_lib, g_ip_shdr_heap, (HDR_CONFIG *)config, &g_ip_shdr_engine);

	if( retcode != GTS_OK )	{
		return	ip_shdr_convert_retcode( retcode );
	}

	retcode = g_ip_shdr_engine->vft->SetExtResizeFunctions(g_ip_shdr_engine,
															(INIT_EXT_RESIZE) config->openResize,
															(EXT_RESIZE)      config->runResize,
															(CLEAN_EXT_RESIZE)config->closeResize );

	return	ip_shdr_convert_retcode( retcode );
#else
	return	D_IP_SHDR_OK;
#endif
}


INT32 IP_SHDR_Close( VOID )
{
#ifndef D_IP_SHDR_STUB
	if (NULL != g_ip_shdr_engine)		g_ip_shdr_engine->vft->Destroy(g_ip_shdr_engine);

	if ( g_ip_shdr_state != E_IP_SHDR_SEQ_OPEN &&
		 g_ip_shdr_state != E_IP_SHDR_SEQ_CTRL ){
		return D_IP_SHDR_NG_SEQUENCE;
	}
	g_ip_shdr_state = E_IP_SHDR_SEQ_IDLE;

	return D_IP_SHDR_OK;
#else
	return	D_IP_SHDR_OK;
#endif
}


UINT32	IP_SHDR_GetBufferSize(T_IP_SHDR_CONFIG *config)
{
#ifndef D_IP_SHDR_STUB
	INT32		retcode;


	g_ip_shdr_lib = HdrLibGet();

	config->reserve = sizeof( HDR_CONFIG );
	retcode = g_ip_shdr_lib->vft->GetRequiredHeapSize(g_ip_shdr_lib, (HDR_CONFIG *)config );

	return	retcode ;
#else
	return	D_IP_SHDR_OK;
#endif
}


INT32	IP_SHDR_AnalyzeImage( T_IP_SHDR_IMAGE *image, UINT32 *results )
{
#ifndef D_IP_SHDR_STUB
	INT32	retcode = D_IP_SHDR_OK;


	if ( g_ip_shdr_state != E_IP_SHDR_SEQ_OPEN )	{
		return D_IP_SHDR_NG_SEQUENCE;
	}

	retcode = g_ip_shdr_engine->vft->AnalyzeImage( g_ip_shdr_engine, (GT_IMAGE*)image, results, NULL );

	return	ip_shdr_convert_retcode( retcode );
#else
	return	D_IP_SHDR_OK;
#endif
}

INT32	IP_SHDR_Process( T_IP_SHDR_IMAGE *imageDark, T_IP_SHDR_IMAGE *imageLight, T_IP_SHDR_OPTIONS *opt, T_IP_SHDR_IMAGE **imageResult )
{
#ifndef D_IP_SHDR_STUB
	INT32		retcode;


	if ( g_ip_shdr_state != E_IP_SHDR_SEQ_OPEN )	{
		return D_IP_SHDR_NG_SEQUENCE;
	}


	retcode = g_ip_shdr_engine->vft->RegisterImage( g_ip_shdr_engine, (GT_IMAGE*)imageDark, NULL );
	if( retcode != GTS_OK )	{
		return	ip_shdr_convert_retcode( retcode );
	}

	retcode = g_ip_shdr_engine->vft->RegisterImage( g_ip_shdr_engine, (GT_IMAGE*)imageLight, NULL );
	if( retcode != GTS_OK )	{
		return	ip_shdr_convert_retcode( retcode );
	}

	retcode = g_ip_shdr_engine->vft->GetResult( g_ip_shdr_engine, HDRE_EFF_NATURAL, 2, (HDR_OPTIONS*)opt, (GT_IMAGE**)imageResult );

	return	ip_shdr_convert_retcode( retcode );
#else
	return	D_IP_SHDR_OK;
#endif
}

