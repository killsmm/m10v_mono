/**
 * @file		ahip_dev.c
 * @brief		
 * @version		
 * @note		
 * @attention	
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#if 0
#include <string.h>
#include <math.h>
#include "ddim_typedef.h"
#include "ahip_dev.h"
#include "still_img_info.h"
#include "still_utility.h"
#include "evp_if.h"
#include "rs_system_factor.h"

#ifdef CO_AHIP_DEVELOPMENT
/*---------------------------------------------------------------*/
/* Definition                                                    */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/* Enumeration                                                   */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/* Structure                                                     */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/* Global Data                                                   */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/* Macro                                                         */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/* Function                                                      */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/* Local Function                                                */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/
/* Global Function                                               */
/*---------------------------------------------------------------*/
/**
 * Get AHIP input information for DOC
 */
VOID Ahip_Dev_Get_Config( T_AHIP_DEV_FRAME_SIZES* frm_info )
{
	T_BF_R2Y_LATESTADDR_INFO	addr_info;
	FJ_IMG_DZOOM_INFO			crop_info;

	BF_Still_Get_R2yLatestOutAddr( 0/*pipe_no*/, 0, 0, &addr_info );
	RS_System_Factor_Draft_Get_VIS_Crop_Info( &crop_info );

	frm_info->ahip_in.width 	= addr_info.ycc.width;
	frm_info->ahip_in.lines		= addr_info.ycc.lines;
	frm_info->crop_out.start_x	= crop_info.src_rect.left;
	frm_info->crop_out.start_y	= crop_info.src_rect.top;
	frm_info->crop_out.width	= crop_info.src_rect.width;
	frm_info->crop_out.lines	= crop_info.src_rect.lines;
	return;
}

VOID Ahip_Dev_Set_Vis_Shift( INT32 shift_x, INT32 shift_y )
{
	RS_System_Factor_Draft_Set_VIS_Shift(shift_x, shift_y);
}

/**
 * JPEG Encode
 */
ULONG Ahip_Dev_Jpeg_Util_Enc( T_AHIP_DEV_JPEG_ENC* param )
{
	FJ_IMG_INFO	jpeg_info;
	ULONG		enc_size = 0;

	switch( param->ycbcr_smpl ) {
		case E_IM_JPEG_ENC_SMPL_TYPE_YCC444:
			jpeg_info.pfmt = FJ_IMG_PFMT_YCC444;
			break;

		case E_IM_JPEG_ENC_SMPL_TYPE_YCC420:
			jpeg_info.pfmt = FJ_IMG_PFMT_YCC420;
			break;

		case E_IM_JPEG_ENC_SMPL_TYPE_YCC422:
			jpeg_info.pfmt = FJ_IMG_PFMT_YCC422;
			break;

		case E_IM_JPEG_ENC_SMPL_TYPE_YCC400:
		default:
			jpeg_info.pfmt = FJ_IMG_PFMT_YCC400;
			break;
	}

	jpeg_info.addr.AddressY	= param->ycbcr_addr[0];
	jpeg_info.width			= param->width;
	jpeg_info.lines			= param->lines;
	jpeg_info.g_y_width		= param->global_y_width;

	BF_Still_FjImgInfo_Calc_C_Param( &jpeg_info );
	enc_size = BF_Still_Encode_Jpeg_Debug(&jpeg_info, param->code_addr);

	return enc_size;
}

DOUBLE Ahip_Dev_Get_Sensor_Readout_Time( VOID )
{
}

DOUBLE Ahip_Dev_Get_Sensor_Exp_UCHAR sro_idTime( UCHAR sro_id )
{
	return 1/pow( 2, ((DOUBLE)EVP_If_Getprm_TV( sro_id )/65536) );
}

#endif // CO_AHIP_DEVELOPMENT

#else	// if 0
#ifdef CO_AHIP_DEVELOPMENT
#include <math.h>
#include "ddim_typedef.h"
//#include "3a_if.h"
#include "measure_time.h"
// delete basefw\fj\imgproc\still
//#include "still_pro_ctrl.h"

DOUBLE Ahip_Dev_Get_Sensor_Exp_Time( UCHAR sro_id )
{
	INT32 tv_val = 0;
//Åögoodby_3a_if	IP_Dsc3A_If_Get_Tv(sro_id, &tv_val);
	return 1/pow( 2, ((DOUBLE)tv_val/65536) );
}

ULONG Ahip_Dev_Get_Sensor_Readout_Time_us( UCHAR sro_id, ULLONG sensor_vd_no )
{
	return BF_Still_PRO_Ctrl_Get_Readout_Time( sro_id, sensor_vd_no );
}

ULONG Ahip_Dev_Get_Sensor_Sof_Timestamp( UCHAR sro_id, ULLONG sensor_vd_no )
{
	return BF_Still_PRO_Ctrl_Get_Sof_Timestamp( sro_id, sensor_vd_no );
}

ULONG Ahip_Dev_Get_Time_us( VOID )
{
	return BF_Get_Time_us();
}

// dummy code for local development on SNI FW team
#ifdef CO_AHIP_DEVELOPMENT_DUMMY

/**
 * @file		ahip_dev_img.c
 * @brief		
 * @author	 	Fujitsu
 * @since		2013/05/20
 * @note		None
 * @attention	None
 */

#include <string.h>
#include <stdlib.h>
#include "ddim_typedef.h"
#include "ahip_dev.h"
#include "ip_ahip.h"
#include "ipc_fd.h"
#include "debug.h"
#include "fj_still.h"
#include "fj_memory.h"
#include "measure_time.h"
#include "sdram_map_manager.h"
#include "image_info.h"

#define Ahip_Roundup_8( val )		(((ULONG)(val) +7) & 0xFFFFFFF8)
#define Ahip_Rounddown_8( val )		(((ULONG)(val)) & 0xFFFFFFF8)

static volatile UCHAR g_ahip_dev_vis_enabled[ D_IP_FD_INSTANCE_NUM_MAX ]	= { TRUE };
static volatile UCHAR g_ahip_dev_fd_enabled[ D_IP_FD_INSTANCE_NUM_MAX ]		= { TRUE };
static USHORT gip_fd_width	= 0;
static USHORT gip_fd_height	= 0;
static BOOL gip_fd_auto_log_flag[ D_IP_FD_INSTANCE_NUM_MAX ] = { FALSE, FALSE, FALSE, FALSE, FALSE,
																 FALSE, FALSE, FALSE, FALSE, FALSE };

static ULONG gip_fd_thin_count[ D_IP_FD_INSTANCE_NUM_MAX ][ E_IP_FD_INDEX_MAX ]	= { { 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 },
																					{ 0, 0, 0, 0, 0 } };

static ULONG gip_fd_output_count[ D_IP_FD_INSTANCE_NUM_MAX ] = { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 } ;

static VOID ahip_dev_ip_vis_cb(UINT32* timeStamp, INT32* readout, UINT32* exposure, UINT32* iso)
{
	*timeStamp = BF_Get_Time_us();
	*readout = 0;
	*exposure = 0;

//Åögoodby_3a_if	*iso = IP_Dsc3A_If_Get_ISO( 0 );	// TODO : get sro_id..
	//BF_Debug_Print_Information(( "ahip_dev_ip_vis_cb():timeStamp:%u readout:%u exposure:%u iso:%u.\n", *timeStamp, *readout, *exposure, *iso ));
}

static VOID ahip_dev_ip_fd_complete_cb( const UCHAR demosaic_id, const UINT32 fd_index, const UINT32 ahip_frame_id, const UCHAR face_count, const ULLONG frame_no )
{
	USHORT			rect_width		= 0;
	USHORT			rect_height		= 0;
	UCHAR			num_of_faces	= 0;
	UINT32			start_x			= 0;
	UINT32			start_y			= 0;
	UINT32			width			= 0;
	UINT32			height			= 0;
	T_IP_FD_INFO	fd_info[ D_IP_FD_FACE_MAX ];
	BOOL			flip_enabled, mirror_enabled;

	BF_Debug_Print_Information(( "ahip_dev_ip_fd_complete_cb(): face_count%u.\n", face_count ));

	// Dump FD Information.
	if ( gip_fd_auto_log_flag[ demosaic_id ] == TRUE ) {

		if ( gip_fd_thin_count[ demosaic_id ][ fd_index ] >= gip_fd_output_count[ demosaic_id ] ) {

			(VOID)IP_FD_Get_Rect( demosaic_id, fd_index, &rect_width, &rect_height );
			(VOID)IP_FD_Get_Face_List_With_AHIP_Frame_ID( demosaic_id, fd_index, ahip_frame_id, &num_of_faces, &fd_info[ 0 ] );
			(VOID)FJ_GetDemosaicFlipMirrorEnabled( demosaic_id, &flip_enabled, &mirror_enabled );

			BF_Debug_Print_Information(("== DUMP FD [Ahip FrameID = %d, Flip = %d] ==\n", ahip_frame_id, flip_enabled));
			BF_Debug_Print_Information(("fd_index         = %d\n",fd_index)); 
			BF_Debug_Print_Information(("face_count       = %d\n",num_of_faces)); 

			for( int i = 0; i < num_of_faces; i++ ) {
				width	= ( fd_info[ i ].rectPos.width * gip_fd_width  / rect_width  + 1 ) & 0xFFFE;
				height	= ( fd_info[ i ].rectPos.lines * gip_fd_height / rect_height + 1 ) & 0xFFFE;
				start_x	= ( fd_info[ i ].rectPos.posX  * gip_fd_width  / rect_width  + 1 ) & 0xFFFE;
				if ( flip_enabled ) {
					start_y	= ( gip_fd_height - ( ( fd_info[ i ].rectPos.posY  * gip_fd_height / rect_height + 1 ) & 0xFFFE ) ) - height;
				}
				else {
					start_y	= ( fd_info[ i ].rectPos.posY  * gip_fd_height / rect_height + 1 ) & 0xFFFE;
				}
				BF_Debug_Print_Information(("[%d] x=%d, y=%d, width=%d, height=%d\n", i, start_x, start_y, width, height)); 
			}

			BF_Debug_Print_Information(("== DUMP FD result END ==\n"));

			gip_fd_thin_count[demosaic_id][fd_index] = 0;
		}
		else {
			gip_fd_thin_count[demosaic_id][fd_index]++;
		}
	}
}

VOID ahip_dev_init_vis( UCHAR demosaic_id, T_AHIP_DEV_YUV_INFO* yuv_info )
{
#if 0	// ### MILB_Porting.
	(VOID)IP_VIS_Init( demosaic_id, ahip_dev_ip_vis_cb );
	(VOID)IP_VIS_Open( demosaic_id, yuv_info->yuv_size.width, yuv_info->yuv_size.lines );
	(VOID)IP_GE_Open( demosaic_id );
#endif
}

VOID ahip_dev_init_fd( UCHAR demosaic_id, T_AHIP_DEV_YUV_INFO* yuv_info )
{
	(VOID)IP_FD_Init( demosaic_id, ahip_dev_ip_fd_complete_cb );

	gip_fd_width	= yuv_info->yuv_size.width;
	gip_fd_height	= yuv_info->yuv_size.lines;
	(VOID)IP_FD_Open( demosaic_id, E_IP_FD_INDEX_0, yuv_info->yuv_size.width, yuv_info->yuv_size.lines );
}

VOID Ahip_Dev_Init_CB( UCHAR demosaic_id, const ULONG heap_addr, T_AHIP_DEV_YUV_INFO* yuv_info )
{
	INT32					ret;
	T_IP_AHIP_PREVIEW_CFG	cfg;

	BF_Debug_Print_Information(( "Ahip_Dev_Init_CB(): demosaic_id:%u heap_addr:%lx width:%u lines:%u\n", demosaic_id, heap_addr, yuv_info->yuv_size.width, yuv_info->yuv_size.lines ));
	if( ( g_ahip_dev_vis_enabled[demosaic_id] == TRUE ) || ( g_ahip_dev_fd_enabled[demosaic_id] == TRUE ) ) {
		// AHIP open (Preview mode).
		cfg.fmt			= E_IP_AHIP_FMT_YUV420P;
		cfg.size.width	= yuv_info->yuv_size.width;
		cfg.size.lines	= yuv_info->yuv_size.lines;
		cfg.fdFrameRate	= 0x80000000;	// 1/2 of input rate.
		ret = IP_AHIP_OpenPreview( demosaic_id, &cfg );
		if ( ret != D_IP_AHIP_OK) {
			BF_Debug_Print_Information(( "IP_AHIP_OpenPreview(): error%u. ercd=%d\n", 1, ret ));
		}

		if( g_ahip_dev_vis_enabled[demosaic_id] == TRUE ){
			// initialize VIS
			ahip_dev_init_vis( demosaic_id, yuv_info );
		}

		if ( g_ahip_dev_fd_enabled[demosaic_id] == TRUE ) {
			// initialize FD.
			ahip_dev_init_fd( demosaic_id, yuv_info );
		}
	}
}

VOID Ahip_Dev_Yuv_Output_CB( UCHAR demosaic_id, T_AHIP_DEV_YUV_INFO* yuv_info )
{
#if 0	// ### MILB_Porting.
	T_IP_VIS_FRAME		srcImage;
	T_IP_VIS_FRAME		dstImage;
	T_IMAGE_ADDR_YCC	addr_ycc;

	if( g_ahip_dev_vis_enabled[demosaic_id] == TRUE ) {
		// src image parameter
#if 0
		BF_Debug_Print_Information(( "%llu,%4.2f,%lu,%lu,%lu\n"	, yuv_info->frame_no
																, Ahip_Dev_Get_Sensor_Exp_Time( 0 )*1000
																, Ahip_Dev_Get_Sensor_Readout_Time_us( 0, yuv_info->frame_no )
																, Ahip_Dev_Get_Sensor_Sof_Timestamp( 0, yuv_info->frame_no )
																, Ahip_Dev_Get_Time_us()	));
#endif
		srcImage.format				= D_IP_VIS_FMT_YUV420SP;
		srcImage.width				= yuv_info->yuv_size.width;
		srcImage.height				= yuv_info->yuv_size.lines;
		srcImage.plane[ 0 ].data	= (VOID*)yuv_info->addr.AddressY;
		srcImage.plane[ 1 ].data	= (VOID*)yuv_info->addr.AddressCb;
		srcImage.plane[ 2 ].data	= NULL;
		srcImage.plane[ 0 ].stride	= yuv_info->g_y_width;
		srcImage.plane[ 1 ].stride	= yuv_info->g_c_width;
		srcImage.plane[ 2 ].stride	= 0;

		// dst image parameter
		addr_ycc = Sdram_Map_Mgr_Change_AHIP_VIS();

		dstImage.format				= D_IP_VIS_FMT_YUV420SP;
		dstImage.width				= Ahip_Roundup_8(yuv_info->yuv_size.width / 1.2);
		dstImage.height				= Ahip_Roundup_8(yuv_info->yuv_size.lines / 1.2);
		dstImage.plane[ 0 ].data	= (VOID*)addr_ycc.y;
		dstImage.plane[ 1 ].data	= (VOID*)addr_ycc.cb;
		dstImage.plane[ 2 ].data	= (VOID*)addr_ycc.cr;
		dstImage.plane[ 0 ].stride	= yuv_info->g_y_width;
		dstImage.plane[ 1 ].stride	= yuv_info->g_c_width;
		dstImage.plane[ 2 ].stride	= 0;

		IP_GE_Process( demosaic_id, &srcImage, &dstImage, NULL, NULL );

		yuv_info->yuv_size.width	= dstImage.width;
		yuv_info->yuv_size.lines	= dstImage.height;
		yuv_info->addr.AddressY		= (ULONG)dstImage.plane[ 0 ].data;
		yuv_info->addr.AddressCb	= (ULONG)dstImage.plane[ 1 ].data;
		yuv_info->addr.AddressCr	= NULL;
		yuv_info->g_y_width			= dstImage.plane[ 0 ].stride;
		yuv_info->g_c_width			= dstImage.plane[ 1 ].stride;

#if 0
		BF_Debug_Print_Information(( "Output_CB inwidth=%u inlines=%u outwidth=%u outlines=%u Yaddr:%lx Caddr:%lx\n"	, srcImage.width
																														, srcImage.height
																														, yuv_info->yuv_size.width
																														, yuv_info->yuv_size.lines
																														, yuv_info->addr.AddressY
																														, yuv_info->addr.AddressCb ));
#endif
	}
#endif
}

VOID Ahip_Dev_Vis_Enabled( UCHAR demosaic_id, UCHAR enabled )
{
	g_ahip_dev_vis_enabled[demosaic_id] = enabled;
}

VOID Ahip_Dev_Fd_Enabled( UCHAR demosaic_id, UCHAR enabled )
{
	g_ahip_dev_fd_enabled[demosaic_id] = enabled;
}

VOID Ahip_Dev_Fd_AutoLog_Enabled( UCHAR demosaic_id, UCHAR enabled )
{
	gip_fd_auto_log_flag[demosaic_id] = enabled;
}

VOID IP_AHIP_Int_Handler( UCHAR demosaic_id )
{
#ifndef DEBUG_ON_PC
	AhipIntHandler();
#endif

	return;
}

void cmd_ahip_help( void )
{
	BF_Debug_Print_Information(("ahip COMMAND\n"));
	BF_Debug_Print_Information(("ahip init                                                                         : AHIP Task create\n"));
	BF_Debug_Print_Information(("ahip vis [on|off]                                                                 : VIS on/off\n"));
	BF_Debug_Print_Information(("ahip fd [on|off]                                                                  : FD on/off\n"));
}

VOID cmd_ahip_init( VOID )
{
}

void cmd_ahip( int argc, char** argv )
{
	int cmd_err_flag = 0;
	
	FJ_MEMORY_Dcache_All( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH );

	if ( strcmp(argv[1], "init") == 0 ) {
		cmd_ahip_init();
	}
	else if ( strcmp(argv[1], "vis") == 0 ) {
		if ( strcmp(argv[2], "on") == 0 ) {
			Ahip_Dev_Vis_Enabled( atoi(argv[3]), TRUE );
			BF_Debug_Print_Information(("Ahip_Dev_Vis_Enabled( %ld, TRUE )\n", atoi(argv[3])));
		}
		else if ( strcmp(argv[2], "off") == 0 ) {
			Ahip_Dev_Vis_Enabled( atoi(argv[3]), FALSE );
			BF_Debug_Print_Information(("Ahip_Dev_Vis_Enabled( %ld, FALSE )\n", atoi(argv[3])));
		}
	}
	else if ( strcmp(argv[1], "fd") == 0 ) {
		if ( strcmp(argv[2], "on") == 0 ) {
			Ahip_Dev_Fd_Enabled( atoi(argv[3]), TRUE );
		}
		else if ( strcmp(argv[2], "off") == 0 ) {
			Ahip_Dev_Fd_Enabled( atoi(argv[3]), FALSE );
		}
		else if ( strcmp(argv[2], "autolog") == 0 ) {
			if ( strcmp(argv[3], "on") == 0 ) {
				Ahip_Dev_Fd_AutoLog_Enabled( atoi(argv[4]), TRUE );
				BF_Debug_Print_Information(("Ahip_Dev_Fd_AutoLog_Enabled( TRUE )\n"));
			}
			else if ( strcmp(argv[3], "off") == 0 ) {
				Ahip_Dev_Fd_AutoLog_Enabled( atoi(argv[4]), FALSE );
				BF_Debug_Print_Information(("Ahip_Dev_Fd_AutoLog_Enabled( FALSE )\n"));
			}
		}
	}
	else {
		cmd_err_flag = 1;
	}

	// AHIP COMMAND ERROR
	if ( cmd_err_flag ) {
		BF_Debug_Print_Information(("Arguments Error\n"));
		cmd_ahip_help();
	}
}


#endif	// CO_AHIP_DEVELOPMENT_DUMMY
#endif	// CO_AHIP_DEVELOPMENT
#endif	// if 0
