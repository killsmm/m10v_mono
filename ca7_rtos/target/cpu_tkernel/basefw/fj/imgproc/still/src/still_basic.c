/**
 * @file		still_basic.c
 * @brief		Basic APIs for Still.
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "instance.h"

#include "still_basic.h"
#include "still_attr.h"
#include "still_debug.h"
// delete basefw\fj\imgproc\still
//#include "still_sync_ctrl.h"
//#include "still_r2y_ctrl.h"
//#include "still_connectivity_manager.h"
//#include "still_memory_ctrl.h"
//#include "still_jpeg_capture.h"
#include "sdram_map_manager.h"
#include "fj_host.h"
// delete basefw\fj\imgproc\still
//#include "still_piv_manager.h"
//#include "still_dzoom.h"
#include "debug.h"
#include "movie_video_attr.h"
// delete basefw\fj\imgproc\still
#include "im_pro.h"

#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_STILL_BASIC_JPEG_BUFFER_MAX			(16)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_VIDEO = 0,
	T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_STILL,
	T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_DISP,
	T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_STITCH,
	T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_MAX
} T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE;

// delete basefw\fj\imgproc\still
typedef enum {
	E_BF_R2Y_CTRL_BTC_HIST_TBL_R = 0,		/**< R component data table. */
	E_BF_R2Y_CTRL_BTC_HIST_TBL_G,			/**< G component data table. */
	E_BF_R2Y_CTRL_BTC_HIST_TBL_B,			/**< B component data table. */
	E_BF_R2Y_CTRL_BTC_HIST_TBL_Y,			/**< Y component data table. */
	E_BF_R2Y_CTRL_BTC_HIST_TBL_MAX			/**< Maximum value. */
} E_BF_R2Y_CTRL_BTC_HIST_TBL;

typedef enum {
	E_BF_R2Y_CTRL_TCHS_HIST_BIT_WIDTH_7_0 = 0,		/**< Detection bit range[7:0]	Detection maximum value:0xFF	*/
	E_BF_R2Y_CTRL_TCHS_HIST_BIT_WIDTH_8_1,			/**< Detection bit range[8:1]	Detection maximum value:0x1FF	*/
	E_BF_R2Y_CTRL_TCHS_HIST_BIT_WIDTH_9_2,			/**< Detection bit range[9:2]	Detection maximum value:0x3FF	*/
	E_BF_R2Y_CTRL_TCHS_HIST_BIT_WIDTH_10_3,			/**< Detection bit range[10:3]	Detection maximum value:0x7FF	*/
	E_BF_R2Y_CTRL_TCHS_HIST_BIT_WIDTH_11_4,			/**< Detection bit range[11:4]	Detection maximum value:0xFFF	*/
	E_BF_R2Y_CTRL_TCHS_HIST_BIT_WIDTH_MAX			/**< Maximum value.	*/
} E_BF_R2Y_CTRL_TCHS_HIST_BIT_WIDTH;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	ULONG		frame_no;							// Frame no.
	ULONG		addr;								// Address.
	UINT32		sampling_count;						// sampling count.
} T_BF_STILL_BASIC_JPEG_BUFFER;

typedef struct {
	FJ_ERR_CODE		err_code;
	CHAR*			message;
} T_BF_STILL_BASIC_PARAM_CHECK;

// delete\basefw\fj\imgproc\still
typedef struct {
	SHORT			offset_r;							/**< TCOFR : Offset R correction value before TC(10bits signed)		*/
	SHORT			offset_g;							/**< TCOFG : Offset G correction value before TC(10bits signed)		*/
	SHORT			offset_b;							/**< TCOFB : Offset B correction value before TC(10bits signed)		*/
	SHORT			offset_yb;							/**< TCOFYB : Offset Yb correction value before TC(10bits signed)	*/
} T_BF_R2Y_TCOF_PARAM;								/* Offset before TC data	*/

typedef struct {
	USHORT			start_x;					/**< TCTSTA.TCTHSTA : start X position (12bits, multiple of 2)				*/
	USHORT			start_y;					/**< TCTSTA.TCTVSTA : start Y position (14bits)								*/
	UCHAR			block_hsiz;					/**< TCTB.TCTBHSIZ : Horizontal size (8bits, multiple of 2, 8~128)			*/
	UCHAR			block_vsiz;					/**< TCTB.TCTBVSIZ : Vertical size (8bits, 4~128)							*/
	UCHAR			block_hnum;					/**< TCTB.TCTBHNUM : Block count(Horizontal) (6bits, 2~40)					*/
	UCHAR			block_vnum;					/**< TCTB.TCTBVNUM : Block count(Vertical) (5bits, 2~30)					*/
} T_BF_R2Y_TCT_CTRL_PARAM;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Sensor instance.
static volatile T_BF_INSTANCE_SENSOR		gBF_Instance_Sensor[D_BF_INSTANCE_NUM_MAX_SENSOR];

// SEN instance.
static volatile T_BF_INSTANCE_SEN			gBF_Instance_Sen[D_BF_INSTANCE_ID_MAX_SEN];

// Demosaic instance.
static volatile T_BF_INSTANCE_DEMOSAIC		gBF_Instance_Demosaic[D_BF_INSTANCE_ID_MAX_DEMOSAIC];

// Still encode instance.
static volatile T_BF_INSTANCE_STILL_ENCODE	gBF_Instance_Still_Encode[D_BF_INSTANCE_ID_MAX_STILL_ENCODE];

// Stitch instance.
static volatile T_BF_INSTANCE_STITCH		gBF_Instance_Stitch[D_BF_INSTANCE_ID_MAX_STITCH];

//Stitch mode
static volatile FJ_STILL_STITCH_TYPE		gBF_Instance_Stitch_Type = FJ_STILL_STITCH_TYPE_VIDEO;

// Jpeg buffer.
static volatile T_BF_STILL_BASIC_JPEG_BUFFER	gBF_Still_Basic_Jpeg_buffer[D_BF_STILL_BASIC_JPEG_BUFFER_MAX];


static volatile T_BF_STILL_BASIC_PARAM_CHECK	gBF_Still_Basic_Param_Check[ T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_MAX ][ FJ_DEMOSAIC_OUTPUT_IMG_PFMT_MAX ] = {
	// T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_VIDEO
	{
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_VIDEO
		{
			FJ_ERR_OK,				NULL,
		},
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC422
		{
			FJ_ERR_NG_INPUT_PARAM,	"Video instance is not support to connect YCC422 format of DemosaicOutput.[ImageFormat:%d DemosaicID:%d OutputID:%d]\n",
		},
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC420
		{
			FJ_ERR_NG_INPUT_PARAM,	"Video instance is not support to connect YCC420 format of DemosaicOutput.[ImageFormat:%d DemosaicID:%d OutputID:%d]\n",
		},
	},

	// T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_STILL
	{
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_VIDEO
		{
			FJ_ERR_NG_INPUT_PARAM,	"Still instance is not support to connect VIDEO format of DemosaicOutput.[ImageFormat:%d DemosaicID:%d OutputID:%d]\n",
		},
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC422
		{
			FJ_ERR_OK,				NULL,
		},
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC420
		{
			FJ_ERR_OK,				"Still instance is not recommend to connect YCC420 format of DemosaicOutput.[ImageFormat:%d DemosaicID:%d OutputID:%d]\n",
		},
	},

	// T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_DISP
	{
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_VIDEO
		{
			FJ_ERR_OK,				"Disp instance is not recommend to connect VIDEO format of DemosaicOutput.[ImageFormat:%d DemosaicID:%d OutputID:%d]\n",
		},
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC422
		{
			FJ_ERR_OK,				"Disp instance is not recommend to connect YCC422 format of DemosaicOutput.[ImageFormat:%d DemosaicID:%d OutputID:%d]\n",
		},
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC420
		{
			FJ_ERR_OK,				NULL,
		},
	},

	// T_BF_STILL_BASIC_DEMOSAIC_CONN_TYPE_STITCH
	{
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_VIDEO
		{
			FJ_ERR_NG_INPUT_PARAM,	"Stitch instance is not support to connect VIDEO format of DemosaicOutput.[ImageFormat:%d DemosaicID:%d OutputID:%d]\n",
		},
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC422
		{
			FJ_ERR_OK,				NULL,
		},
		// FJ_DEMOSAIC_OUTPUT_IMG_PFMT_YCC420
		{
			FJ_ERR_OK,				NULL,
		},
	},
};

// delete basefw\fj\imgproc\still
static volatile T_IM_PRO_SG_CTRL g_pro_sen_ctrl_sg_reg[D_BF_INSTANCE_ID_MAX_SEN] = {{ 0 }};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// delete basefw\fj\imgproc\still
VOID BF_Still_PRO_Ctrl_Sg_Set( const UCHAR sen_id, const FJ_SG_CTRL* const sg_ctrl )
{
	g_pro_sen_ctrl_sg_reg[sen_id].vhdo		= sg_ctrl->vhdo;
	g_pro_sen_ctrl_sg_reg[sen_id].vdot		= sg_ctrl->vdot;
	g_pro_sen_ctrl_sg_reg[sen_id].vdinv		= sg_ctrl->vdinv;
	g_pro_sen_ctrl_sg_reg[sen_id].hdinv		= sg_ctrl->hdinv;
	g_pro_sen_ctrl_sg_reg[sen_id].hdchg		= sg_ctrl->hdchg;
	g_pro_sen_ctrl_sg_reg[sen_id].vddly		= sg_ctrl->vddly;
	g_pro_sen_ctrl_sg_reg[sen_id].vdocyc	= sg_ctrl->vdocyc;
	g_pro_sen_ctrl_sg_reg[sen_id].vdow		= sg_ctrl->vdow;
	g_pro_sen_ctrl_sg_reg[sen_id].hdocyc	= sg_ctrl->hdocyc;
	g_pro_sen_ctrl_sg_reg[sen_id].hdow		= sg_ctrl->hdow;
	g_pro_sen_ctrl_sg_reg[sen_id].hdocyc2	= sg_ctrl->hdocyc2;
	g_pro_sen_ctrl_sg_reg[sen_id].hdow2		= sg_ctrl->hdow2;
	Im_PRO_SG_Ctrl( sen_id, (T_IM_PRO_SG_CTRL*)&g_pro_sen_ctrl_sg_reg[sen_id] );
}

VOID BF_Still_PRO_Ctrl_SG_Start( const UCHAR sen_id )
{
	T_IM_PRO_CALLBACK_CFG	callback_cfg;

	// delete\basefw\fj\imgproc\still
	//callback_cfg.inthandler	= BF_Still_PRO_Ctrl_SEN_Sg_VD_CB;
	callback_cfg.user_param	= 0;
	Im_PRO_SEN_Set_SG_VD_Int_Handler( &callback_cfg );

	// delete\basefw\fj\imgproc\still
	//callback_cfg.inthandler	= BF_Still_PRO_Ctrl_SEN_Sg_HD_CB;
	callback_cfg.user_param	= 0;
	Im_PRO_SEN_Set_SG_HD_Int_Handler( &callback_cfg );

#if 0
#ifndef D_SENSOR_LOW_POWER_CTRL_TIMER
	if( ( sen_id == 0 ) &&
		( g_is_sensor_low_power_control[0] == FJ_STILL_SEN_SENSOR_LP_EN ) ) {

		FJ_ERR_CODE	ercd;
		Dd_Top_Set_CLKSTOP4_APCK1( 1 );
		ercd = fj_top_set_clock_select_ap1( FJ_TOP_SNAPCLKO_CLKSEL_APCLK );
		if( FJ_ERR_OK == ercd ) {
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_APCK1() ) {
			Dd_Top_Set_CLKSTOP4_APCK1( 0 );
		}
		Im_PRO_SG_Start( 1 );
#if 0
		OS_User_Dly_Tsk( 1 );
#else
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_SG, FJ_MBALOG_TYPE_START, "SG1_delay" ));
		for( UCHAR i = 0; i < 8; i++ ) {
			Dd_ARM_Wait_ns( 100 * 1000 );
		}
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_SG, FJ_MBALOG_TYPE_END, "SG1_delay" ));
#endif
	}
#endif	// D_SENSOR_LOW_POWER_CTRL_TIMER
#endif
	Im_PRO_SG_Start( sen_id );
}

VOID BF_Still_PRO_Ctrl_SG_Stop( const UCHAR sen_id, const UCHAR stop_mode )
{
	if( stop_mode == 0 ) {
		Im_PRO_SG_Stop( sen_id, 0 );	// not force
	}
	else {
		Im_PRO_SG_Stop( sen_id, 1 );	// force
	}
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

//=========================================================================================

FJ_ERR_CODE fj_set_sg( UCHAR sen_id, FJ_SG_CTRL* sg_ctrl )
{
	BF_Still_PRO_Ctrl_Sg_Set( sen_id, sg_ctrl );

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_sg_start( UCHAR sen_id )
{
	BF_Still_PRO_Ctrl_SG_Start( sen_id );

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_sg_stop( UCHAR sen_id, UCHAR stop_mode )
{
	BF_Still_PRO_Ctrl_SG_Stop( sen_id, stop_mode );

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_sen_control_sensorclk( UCHAR on_off )
{
	
	if(on_off == 1){
		Im_PRO_SENTOP_Control_Clock( E_IM_PRO_SENTOP_CLK_TYPE_SENSORCLK, 0, 0 );
	}else if(on_off == 0){
		Im_PRO_SENTOP_Control_Clock( E_IM_PRO_SENTOP_CLK_TYPE_SENSORCLK, 1, 0 );
	}else{
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_getdemosaicflipmirrorenabled( UCHAR demosaic_id, BOOL* flip_enabled, BOOL* mirror_enabled )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;

	if ( demosaic_id < D_BF_INSTANCE_ID_MAX_DEMOSAIC ) {

		if ( gBF_Instance_Demosaic[demosaic_id].flag == E_BF_INSTANCE_FLAG_USE ) {

			*mirror_enabled = BF_Still_Attr_Get_R2Y_Mirror_Enable( demosaic_id );
			*flip_enabled = BF_Still_Attr_Get_R2Y_Flip_Enable( demosaic_id );
		}
		else {
			err_code = FJ_ERR_NG;			// not use.
		}
	}
	else {
		err_code = FJ_ERR_NG_INPUT_PARAM;	// param err.
	}

	return err_code;
}

/**
 * Get Demosaic instance.
 */
T_BF_INSTANCE_DEMOSAIC* BF_Instance_Get_Demosaic( UCHAR demosaic_id )
{
	if ( demosaic_id < D_BF_INSTANCE_ID_MAX_DEMOSAIC ) {
		return (T_BF_INSTANCE_DEMOSAIC*)&gBF_Instance_Demosaic[demosaic_id];
	}
	else {
		return NULL;
	}
}

/**
 * Get Still encode instance.
 */
T_BF_INSTANCE_STILL_ENCODE* BF_Instance_Get_StillEncode( UCHAR still_encode_id )
{
	if ( still_encode_id >= D_BF_INSTANCE_ID_MAX_VIDEO_ENCODE ) {

		still_encode_id = still_encode_id - D_BF_INSTANCE_ID_MAX_VIDEO_ENCODE;

		if ( still_encode_id < D_BF_INSTANCE_ID_MAX_STILL_ENCODE ) {
			return (T_BF_INSTANCE_STILL_ENCODE*)&gBF_Instance_Still_Encode[still_encode_id];
		}
		else {
			return NULL;
		}
	}
	else {
		return NULL;
	}
}

/**
 * Get Stitch instance.
 */
T_BF_INSTANCE_STITCH* BF_Instance_Get_Stitch( UCHAR stitch_id )
{
	if( stitch_id < D_BF_INSTANCE_ID_MAX_STITCH )
		return (T_BF_INSTANCE_STITCH*)&gBF_Instance_Stitch[stitch_id];
	else {
		return NULL;
	}
}



VOID BF_Instance_Get_Sen_Id_Info( UCHAR* sen_id_cnt, UCHAR* sen_id_array_pointer )
{
	UCHAR		inst_id;
	UCHAR		inst_id_cnt = 0;
	for ( inst_id = 0; inst_id < D_BF_INSTANCE_ID_MAX_SEN; inst_id++ ) {

		if ( gBF_Instance_Sen[inst_id].flag == E_BF_INSTANCE_FLAG_USE ) {
			sen_id_array_pointer[inst_id_cnt] = inst_id;
 			inst_id_cnt++;
		}
	}
	*sen_id_cnt = inst_id_cnt;
}

UCHAR BF_Instance_Get_Sensor_Id_From_Sen( UCHAR sen_id )
{
	UCHAR sensor_id = 0xFF;

	if( gBF_Instance_Sen[sen_id].connect.before.sensor.instance_p != NULL ) {
		sensor_id = gBF_Instance_Sen[sen_id].connect.before.sensor.instance_id;
	}
	return sensor_id;
}

FJ_ERR_CODE BF_Still_Basic_Unlock_JpegBuffer( ULONG frame_no, ULONG bunk_num )
{
	FJ_ERR_CODE err_code		= FJ_ERR_NG;
	ULONG		i;
	UINT32		still_count_max = 0;
	UINT32		sampling_count	= 0;
	UINT32		last_frame_no	= 0;

	OS_User_Wai_Sem( SID_JPEG_BUFFER );

	for ( i = 0; i < bunk_num; i++ ) {

		if ( ( gBF_Still_Basic_Jpeg_buffer[ i ].frame_no == frame_no ) &&
			 ( gBF_Still_Basic_Jpeg_buffer[ i ].addr     != 0        ) ) {

			sampling_count = gBF_Still_Basic_Jpeg_buffer[ i ].sampling_count;

			gBF_Still_Basic_Jpeg_buffer[ i ].frame_no		= 0;
			gBF_Still_Basic_Jpeg_buffer[ i ].addr			= 0;
			gBF_Still_Basic_Jpeg_buffer[ i ].sampling_count	= 0;

			BF_Debug_Print_Information(( "Unlock_JpegBuffer(idx=%lu)\n", frame_no ));

			still_count_max = BF_Still_Attr_Get_Still_Count();

			if ( still_count_max != FJ_CONFIG_CAPTURE_COUNT_STILL_TIME_LAPSE ) {

				if ( still_count_max > sampling_count ) {
					last_frame_no = still_count_max - sampling_count;
				}
				else {
					last_frame_no = 0;
				}

				// delete\basefw\fj\imgproc\still
				//if ( ( frame_no >= last_frame_no ) ||
				//	 ( frame_no >= BF_Still_Jpeg_Capture_Get_Sequence_Terminate_FrameNo() ) ) {
				if ( frame_no >= last_frame_no ) {
					// send Message.
					BF_Debug_Print_Information(( "<<<<< FJ_HM_VIDEO_MODE_READY \n" ));
					FJ_HostPostMessage( FJ_HM_VIDEO_MODE_READY, 0 );
				}
			}
			else {
				// delete\basefw\fj\imgproc\still
				//if ( frame_no >= BF_Still_Jpeg_Capture_Get_Sequence_Terminate_FrameNo() ) {
					// send Message.
				//	BF_Debug_Print_Information(( "<<<<< FJ_HM_VIDEO_MODE_READY \n" ));
				//	FJ_HostPostMessage( FJ_HM_VIDEO_MODE_READY, 0 );
				//}
			}

			err_code = FJ_ERR_OK;

			break;
		}
	}

	if ( err_code != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( "Unlock_JpegBuffer error(idx=%lu)\n", frame_no ));
	}

	OS_User_Sig_Sem( SID_JPEG_BUFFER );

	return err_code;
}

FJ_ERR_CODE fj_get_capture_stitch_status( FJ_STILL_STITCH_TYPE * stitch_status ) 
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;
	*stitch_status = gBF_Instance_Stitch_Type;
	return err_code;
}

