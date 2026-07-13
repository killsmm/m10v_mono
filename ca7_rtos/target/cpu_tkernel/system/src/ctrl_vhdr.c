/**
 * @file		ctrl_vhdr.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "ctrl_shdr.h"
#include "ctrl_ltm.h"

#include "im_r2y.h"
#include "im_shdr.h"
#include "im_me.h"

#include "system.h"
#include "fj_still.h"
#include "still_utility.h"

#include "ipc_if.h"
#include "ipc_gmd.h"
#include "ipc_vhdr.h"
#include "ipc_hdrprm.h"

#include "ip_ahip.h"
#include "ip_ge.h"
#include "ip_eis.h"

#include "iqpara_api.h"

#include "share_data.h"

#include <string.h>


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define DEBUG_SAVE_IMG

// #define D_IPU_VHDR_BLD_TH_Y					(240) // default
// #define D_IPU_VHDR_BLD_MIN_BLD_RATIO		(128)
// #define D_IPU_VHDR_BLD_TH_Y				(240) // more bright parameter 1 
// #define D_IPU_VHDR_BLD_MIN_BLD_RATIO		(64)
// #define D_IPU_VHDR_BLD_TH_Y				(255) // more bright parameter 2 
// #define D_IPU_VHDR_BLD_MIN_BLD_RATIO		(128)
// #define D_IPU_VHDR_BLD_TH_Y				(255) // more bright parameter 3 
// #define D_IPU_VHDR_BLD_MIN_BLD_RATIO		(64)
#define D_IPU_VHDR_BLD_TH_Y					(255) // more bright parameter 4 
#define D_IPU_VHDR_BLD_MIN_BLD_RATIO		(16)

#define D_IPU_VHDR_BLD_CALC_MODE			(1)
#define D_IPU_VHDR_EVC_FLAG					(0)
#define D_IPU_VHDR_EVC_TYPE					(0)
#define D_IPU_VHDR_EVC_MODE					(0)
#define D_IPU_VHDR_EVC_TH_Y					(240)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define ipu_vhdr_calc_pit( src, dst )	((UINT32)((UINT64)0x4000 * ((src) -1) / ((dst) -1)))
#define RN_FIX_VAL(n) ( (float)( (n) / 65536.0 ) )
/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

typedef enum {
	E_IPU_VHDR_LOWPASS_FILTER_FRAME_OFF = 1,
	E_IPU_VHDR_LOWPASS_FILTER_FRAME_15 = 15,	// 
	E_IPU_VHDR_LOWPASS_FILTER_FRAME_30 = 30,	// Bypass the Video HDR processing with normal exposure frame.
	E_IPU_VHDR_LOWPASS_FILTER_FRAME_60 = 60,	// Bypass the Video HDR processing with short exposure frame1.
} E_IPU_VHDR_LOWPASS_FILTER_FRAME;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

/** VHDR Lib parameter. */
typedef struct {
	UCHAR mode; 									/**<  param mode 0 : default param 1 : custom from user*/
	INT32 bld_th_y; 								/**<  The effective brightness threshold 0-255 default value 240*/
	INT32 bld_calc_mode; 							/**<   Calculation mode of the blend table 0 : normal   1 : custom*/
	INT32 bld_min_bld_ratio; 						/**<  The effective brightness threshold 0-255 default value 240*/
	UCHAR evc_flag; 								/**<  The setting of the EVC table 0: Disable  1: Enable*/
	INT32 evc_type; 								/**<  EVC table type setting to be formed 0: Shade serious consideration 1: Balance 2: Light part serious consideration*/
	INT32 evc_calc_mode;							/**<  The effective brightness threshold 0-255 default value 240*/
	INT32 evc_th_Y;									/**<  Calculation mode setting of EVC table 0 : mild  1 : aggressive*/
	E_IPU_VHDR_LOWPASS_FILTER_FRAME e_lowpass_frame;	/**<  The effective brightness threshold 0-255 for evc table. Recommended value is 240*/
} T_IPU_VHDR_SET_PRM;

/** VHDR Lib Table parameter. */
typedef struct {
	UINT32 bldofs[7];
	UINT32 bldsl[7];
	UINT32 bldb[7];
	SHORT evc[30];
} T_IPU_VHDR_TABLE_DATA;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static volatile USHORT gIpu_vhdr_gamma_tbl[ 4096 ] = { 0 };
static volatile INT32 gIpu_vhdr_gamma_tbl_type = 0;

static const USHORT gIpu_vhdr_iq_default_gamma_full_tbl[D_IM_R2Y_TABLE_MAX_GAMMA] = {
	0x0000, 0x0004, 0x0008, 0x000c, 0x0010, 0x0014, 0x0018, 0x001c,
	0x0020, 0x0024, 0x0028, 0x002c, 0x0030, 0x0034, 0x0038, 0x003c,
	0x0040, 0x0044, 0x0048, 0x004c, 0x0050, 0x0054, 0x0058, 0x005c,
	0x0060, 0x0064, 0x0068, 0x006c, 0x0070, 0x0074, 0x0078, 0x007c,
	0x0080, 0x0084, 0x0088, 0x008c, 0x0090, 0x0094, 0x0098, 0x009c,
	0x00a0, 0x00a4, 0x00a8, 0x00ac, 0x00b0, 0x00b4, 0x00b8, 0x00bc,
	0x00c0, 0x00c4, 0x00c8, 0x00cc, 0x00d0, 0x00d4, 0x00d8, 0x00dc,
	0x00e0, 0x00e4, 0x00e8, 0x00ec, 0x00f0, 0x00f4, 0x00f8, 0x00fc,
	0x0100, 0x0104, 0x0108, 0x010c, 0x0110, 0x0114, 0x0118, 0x011c,
	0x0120, 0x0124, 0x0128, 0x012c, 0x0130, 0x0134, 0x0138, 0x013c,
	0x0140, 0x0144, 0x0148, 0x014c, 0x0150, 0x0154, 0x0158, 0x015c,
	0x0160, 0x0164, 0x0168, 0x016c, 0x0170, 0x0174, 0x0178, 0x017c,
	0x0180, 0x0184, 0x0188, 0x018c, 0x0190, 0x0194, 0x0198, 0x019c,
	0x01a0, 0x01a4, 0x01a8, 0x01ac, 0x01b0, 0x01b4, 0x01b8, 0x01bc,
	0x01c0, 0x01c4, 0x01c8, 0x01cc, 0x01d0, 0x01d4, 0x01d8, 0x01dc,
	0x01e0, 0x01e4, 0x01e8, 0x01ec, 0x01f0, 0x01f4, 0x01f8, 0x01fc,
	0x0200, 0x0204, 0x0208, 0x020c, 0x0210, 0x0214, 0x0218, 0x021c,
	0x0220, 0x0224, 0x0228, 0x022c, 0x0230, 0x0234, 0x0238, 0x023c,
	0x0240, 0x0244, 0x0248, 0x024c, 0x0250, 0x0254, 0x0258, 0x025c,
	0x0260, 0x0264, 0x0268, 0x026c, 0x0270, 0x0274, 0x0278, 0x027c,
	0x0280, 0x0284, 0x0288, 0x028c, 0x0290, 0x0294, 0x0298, 0x029c,
	0x02a0, 0x02a4, 0x02a8, 0x02ab, 0x02af, 0x02b3, 0x02b7, 0x02bb,
	0x02bf, 0x02c3, 0x02c7, 0x02cb, 0x02cf, 0x02d3, 0x02d7, 0x02db,
	0x02df, 0x02e3, 0x02e7, 0x02eb, 0x02ef, 0x02f3, 0x02f7, 0x02fb,
	0x02ff, 0x0303, 0x0307, 0x030b, 0x030f, 0x0313, 0x0317, 0x031b,
	0x031f, 0x0323, 0x0327, 0x032b, 0x032f, 0x0333, 0x0337, 0x033b,
	0x033f, 0x0343, 0x0347, 0x034b, 0x034f, 0x0353, 0x0357, 0x035b,
	0x035f, 0x0363, 0x0367, 0x036b, 0x036f, 0x0373, 0x0377, 0x037b,
	0x037f, 0x0383, 0x0387, 0x038b, 0x038f, 0x0393, 0x0397, 0x039b,
	0x039f, 0x03a3, 0x03a7, 0x03ab, 0x03af, 0x03b3, 0x03b7, 0x03bb,
	0x03bf, 0x03c3, 0x03c7, 0x03cb, 0x03cf, 0x03d3, 0x03d7, 0x03db,
	0x03df, 0x03e3, 0x03e7, 0x03eb, 0x03ef, 0x03f3, 0x03f7, 0x03fb,
};

static const ULLONG gIpu_vhdr_iq_default_gamma_diff_tbl[D_IM_R2Y_TABLE_MAX_GAMMA] = {
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull,
	0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x1000100010001000ull, 0x0001000100001000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull, 0x0001000100010000ull,
	0x0001000100010000ull, 0x0001000100010000ull, 0x0010000100010000ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
	0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull, 0x0010001000100010ull,
};

static USHORT gIpu_vhdr_hist[2][D_SHARE_R2Y_HIST_NUM];
static T_IPU_VHDR_TABLE_DATA vhdrTableData[60] = {{{0},{0},{0},{0}}};

static ULLONG gFrame_No_vhdr = 0;

static ULONG   start_time[10],end_time[10];

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void ipu_iq_default_set_vhdr_gamma_tbl( T_IM_R2Y_CTRL_GAMMA* gamma_ctrl, T_FJ_IQ_GAMMA* gamma_info );
static VOID ipu_vhdr_iq_get_gamma_tbl( T_FJ_IQ_GAMMA* gamma_info, T_FJ_IQ_SEL_INFO* sel_info );
static VOID ipu_vhdr_get_gamma_tbl( INT32* const gamma_tbl_type, VOID** gamma_tbl );
static VOID ipu_vhdr_set_gamma_tbl( T_FJ_IQ_SEL_INFO* sel_info );
static INT32 ipu_vhdr_get_evc_table( IPC_SHORT *evc, const T_SHARE_APEX* apex);
static VOID ipu_vhdr_int_handler_shdr( ULONG interrupt_flag_mask );
/*==============================================================================*/
/* Debug Function																*/
/*==============================================================================*/
#ifdef DEBUG_SAVE_IMG
#include "fj_filesystem.h"
static void debug_save_file( CHAR* fname, ULONG addr, ULONG size )
{
	ULONG fid;
	ULONG write_size = 0;

	fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)addr, size, &write_size );
	FJ_Fs_Close( fid );
}

#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void Init_VHDR_Proc(ULONG heap_addr, ULONG heap_size, USHORT yuv_h_size, USHORT yuv_v_size )
{
	IPC_INT				gamma_tbl_type;
	IPC_USHORT*			gamma_tbl;
	IPC_UINT			img_w;
	IPC_UINT			img_h;
	INT32 ercd;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_VHDR, FJ_MBALOG_FUNC_ID_HDR, FJ_MBALOG_TYPE_POINT, "vHDR Init"));
	
	ipu_vhdr_set_gamma_tbl( NULL );
	
	ercd = IPC_VHDR_Open();
	if( ercd != D_IPC_VHDR_OK ){
		UPRINTF_ERR((INT32)ercd);
		return;
	}

	ipu_vhdr_get_gamma_tbl( &gamma_tbl_type, (VOID**)&gamma_tbl );

	img_w = yuv_h_size;
	img_h = yuv_v_size;
	ercd = IPC_VHDR_Init((IPC_UINT)heap_addr, (IPC_UINT *)&heap_size,
						img_w, img_h,
						gamma_tbl_type, gamma_tbl);
	if( ercd != D_IPC_VHDR_OK ){
		UPRINTF_ERR((INT32)ercd);
		return;
	}
	
	start_time[5] = 0;
	end_time[5] = 0;
}

void Start_VHDR_Proc( T_IPU2SHDR_MNG* in, T_SHARE_R2Y_DOL_HIST* hist, ULLONG frame_no )
{
	T_SHARE_APEX apex;
	T_IPC_VHDR_START_PRM startPrm;
	IPC_SHORT evc[30] = {0,336,688,1216,3328,3968,4000,4032,4064,4095,
						750,750,750,646,561,512,512,512,512,512,
						0,0,-25,-5,-10,0,0,0,0,0 };
	INT32	ercd;
	ULONG	diff_time;
	INT32	data_cnt, cnt;
	ULONG   data0, data1;
	
	end_time[5]= BF_Get_Time_us();
	if(start_time[5] == 0) diff_time = 0;
	else diff_time = end_time[5] - start_time[5];
	start_time[5]= BF_Get_Time_us();

	gFrame_No_vhdr = frame_no;

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_VHDR, FJ_MBALOG_FUNC_ID_HDR, FJ_MBALOG_TYPE_START, "vHDR Start %d %ld", (INT32)frame_no, diff_time));

	memset( &startPrm, 0, sizeof( startPrm ) );

	// update histogram
	for( data_cnt = 0; data_cnt < D_SHARE_R2Y_HIST_NUM; data_cnt++ ) {
		data0 = 0;
		data1 = 0;
		for(cnt = 0; cnt < hist->rect_num; cnt++) {
			data0 += hist->long_hist[cnt].data[data_cnt];
			data1 += hist->short_hist[cnt].data[data_cnt];
		}
		
		gIpu_vhdr_hist[0][data_cnt] = data0; // for long exposure
		gIpu_vhdr_hist[1][data_cnt] = data1; // for short exposure
	}

	Get_Frame_Share_APEX( &apex, frame_no );

	ercd = ipu_vhdr_get_evc_table((IPC_SHORT *)evc, &apex);
	if( ercd != D_IPC_VHDR_OK ){
		UPRINTF_ERR((INT32)ercd);
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_VHDR, FJ_MBALOG_FUNC_ID_HDR, FJ_MBALOG_TYPE_END, "vHDR Start"));
		Snd_Event_to_System( E_SYSTEM_EVENT_DOL_COMBINE, gFrame_No_vhdr );
		return;
	}
	
	// set image information
	startPrm.bypassMode	= FALSE;
	startPrm.compMode	= FALSE;
	startPrm.edgeCpy	= FALSE;
	startPrm.edgeCut	= FALSE;
	startPrm.brcEnable	= FALSE;
	startPrm.brcMode	= FALSE;
	startPrm.filMargin	= 0;
	startPrm.reserve0	= 0;
	startPrm.reserve1	= 0;
	
	if(in->vhdr_in[1].yuv_format == E_SHARE_YUV_FORMAT_422_SEMI){
		startPrm.imgLong[0].imgForm 			= E_IPC_IMAGE_FORM_422SP;
	}else{
		startPrm.imgLong[0].imgForm 			= E_IPC_IMAGE_FORM_420SP;
	}		
	startPrm.imgLong[0].bitDepth				= 8;
	startPrm.imgLong[0].dataForm				= E_IPC_IMAGE_DATA_FORM_PACK;
	startPrm.imgLong[0].imgInfo[ 0 ].pImgAdrs	= (VOID*)in->vhdr_in[1].mem_addr_y;
	startPrm.imgLong[0].imgInfo[ 0 ].gImgSize	= in->vhdr_in[1].yuv_h_byte;
	startPrm.imgLong[0].imgInfo[ 0 ].wImgSize	= in->vhdr_in[1].yuv_h_size;
	startPrm.imgLong[0].imgInfo[ 0 ].hImgSize	= in->vhdr_in[1].yuv_v_size;
	startPrm.imgLong[0].imgInfo[ 1 ].pImgAdrs	= (VOID*)in->vhdr_in[1].mem_addr_c;
	startPrm.imgLong[0].imgInfo[ 1 ].gImgSize	= in->vhdr_in[1].yuv_h_byte;
	startPrm.imgLong[0].imgInfo[ 1 ].wImgSize	= in->vhdr_in[1].yuv_h_size;
	startPrm.imgLong[0].imgInfo[ 1 ].hImgSize	= in->vhdr_in[1].yuv_v_size;
	startPrm.imgLong[0].imgInfo[ 2 ].pImgAdrs	= 0;
	startPrm.imgLong[0].imgInfo[ 2 ].gImgSize	= 0;
	startPrm.imgLong[0].imgInfo[ 2 ].wImgSize	= 0;
	startPrm.imgLong[0].imgInfo[ 2 ].hImgSize	= 0;
	
	
	if(in->vhdr_in[0].yuv_format == E_SHARE_YUV_FORMAT_422_SEMI){
		startPrm.imgShort[0].imgForm 			= E_IPC_IMAGE_FORM_422SP;
	}else{
		startPrm.imgShort[0].imgForm 			= E_IPC_IMAGE_FORM_420SP;
	}		
	startPrm.imgShort[0].bitDepth				= 8;
	startPrm.imgShort[0].dataForm				= E_IPC_IMAGE_DATA_FORM_PACK;
	startPrm.imgShort[0].imgInfo[ 0 ].pImgAdrs	= (VOID*)in->vhdr_in[0].mem_addr_y;
	startPrm.imgShort[0].imgInfo[ 0 ].gImgSize	= in->vhdr_in[0].yuv_h_byte;
	startPrm.imgShort[0].imgInfo[ 0 ].wImgSize	= in->vhdr_in[0].yuv_h_size;
	startPrm.imgShort[0].imgInfo[ 0 ].hImgSize	= in->vhdr_in[0].yuv_v_size;
	startPrm.imgShort[0].imgInfo[ 1 ].pImgAdrs	= (VOID*)in->vhdr_in[0].mem_addr_c;
	startPrm.imgShort[0].imgInfo[ 1 ].gImgSize	= in->vhdr_in[0].yuv_h_byte;
	startPrm.imgShort[0].imgInfo[ 1 ].wImgSize	= in->vhdr_in[0].yuv_h_size;
	startPrm.imgShort[0].imgInfo[ 1 ].hImgSize	= in->vhdr_in[0].yuv_v_size;
	startPrm.imgShort[0].imgInfo[ 2 ].pImgAdrs	= 0;
	startPrm.imgShort[0].imgInfo[ 2 ].gImgSize	= 0;
	startPrm.imgShort[0].imgInfo[ 2 ].wImgSize	= 0;
	startPrm.imgShort[0].imgInfo[ 2 ].hImgSize	= 0;
	
	startPrm.imgDst[0].imgForm					= E_IPC_IMAGE_FORM_RGB_P;
	startPrm.imgDst[0].bitDepth 				= 16;
	startPrm.imgDst[0].dataForm 				= E_IPC_IMAGE_DATA_FORM_PACK;
	startPrm.imgDst[0].imgInfo[ 0 ].pImgAdrs	= (VOID*)in->vhdr_out.mem_addr_r;
	startPrm.imgDst[0].imgInfo[ 0 ].gImgSize	= in->vhdr_out.rgb_h_byte;
	startPrm.imgDst[0].imgInfo[ 0 ].wImgSize	= in->vhdr_out.rgb_h_size;
	startPrm.imgDst[0].imgInfo[ 0 ].hImgSize	= in->vhdr_out.rgb_v_size;
	startPrm.imgDst[0].imgInfo[ 1 ].pImgAdrs	= (VOID*)in->vhdr_out.mem_addr_g;
	startPrm.imgDst[0].imgInfo[ 1 ].gImgSize	= in->vhdr_out.rgb_h_byte;
	startPrm.imgDst[0].imgInfo[ 1 ].wImgSize	= in->vhdr_out.rgb_h_size;
	startPrm.imgDst[0].imgInfo[ 1 ].hImgSize	= in->vhdr_out.rgb_v_size;
	startPrm.imgDst[0].imgInfo[ 2 ].pImgAdrs	= (VOID*)in->vhdr_out.mem_addr_b;
	startPrm.imgDst[0].imgInfo[ 2 ].gImgSize	= in->vhdr_out.rgb_h_byte;
	startPrm.imgDst[0].imgInfo[ 2 ].wImgSize	= in->vhdr_out.rgb_h_size;
	startPrm.imgDst[0].imgInfo[ 2 ].hImgSize	= in->vhdr_out.rgb_v_size;
	
	ercd = IPC_VHDR_Start(
						&startPrm,
						1,
						evc,
						(T_IM_SHDR_CALLBACK)ipu_vhdr_int_handler_shdr);
	if( ercd != D_IPC_VHDR_OK ){
		UPRINTF_ERR((INT32)ercd);
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_VHDR, FJ_MBALOG_FUNC_ID_HDR, FJ_MBALOG_TYPE_END, "vHDR Start"));
		Snd_Event_to_System( E_SYSTEM_EVENT_DOL_COMBINE, gFrame_No_vhdr );
		return;
	}

	#ifdef DEBUG_SAVE_IMG
	{
		static UCHAR d_vhdr_frame_cnt = 0;
		ULONG size = 0;
			
		if(d_vhdr_frame_cnt == 30)
		{
			size = in->vhdr_out.rgb_h_byte * in->vhdr_out.rgb_v_size;
			debug_save_file((CHAR *)"I:\\VHDR_O.RGB", in->vhdr_out.mem_addr_r, size*3);
			UPRINTF("R Component (0x%08lx) \n", in->vhdr_out.mem_addr_r);
			UPRINTF("G Component (0x%08lx) \n", in->vhdr_out.mem_addr_g);
			UPRINTF("B Component (0x%08lx) \n", in->vhdr_out.mem_addr_b);

			UPRINTF("Size: %d x %d, RGB 16bit \n", in->vhdr_out.rgb_h_size, in->vhdr_out.rgb_v_size);
		}
		else if(d_vhdr_frame_cnt >= 120)
		{
			d_vhdr_frame_cnt = 120;
		}
	
		d_vhdr_frame_cnt++;
	}
	#endif

}

void Close_VHDR_Proc( void )
{
	INT32 ercd;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_VHDR, FJ_MBALOG_FUNC_ID_HDR, FJ_MBALOG_TYPE_POINT, "vHDR Close"));
	
	ercd = IPC_VHDR_Close();
	if( ercd != D_IPC_VHDR_OK ){
		UPRINTF_ERR((INT32)ercd);
		return;
	}
	
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void ipu_iq_default_set_vhdr_gamma_tbl( T_IM_R2Y_CTRL_GAMMA* gamma_ctrl, T_FJ_IQ_GAMMA* gamma_info )
{
	gamma_ctrl->gamma_enable = E_IQ_ENABLE_ON;

	gamma_info->gamma_mode = 0;
	gamma_ctrl->gamma_mode	= gamma_info->gamma_mode;

	gamma_info->rgb_tbl.full = gIpu_vhdr_iq_default_gamma_full_tbl;
	gamma_info->rgb_tbl.diff = gIpu_vhdr_iq_default_gamma_diff_tbl;
	gamma_info->r_tbl.full   = gIpu_vhdr_iq_default_gamma_full_tbl;
	gamma_info->r_tbl.diff   = gIpu_vhdr_iq_default_gamma_diff_tbl;
	gamma_info->g_tbl.full   = gIpu_vhdr_iq_default_gamma_full_tbl;
	gamma_info->g_tbl.diff   = gIpu_vhdr_iq_default_gamma_diff_tbl;
	gamma_info->b_tbl.full   = gIpu_vhdr_iq_default_gamma_full_tbl;
	gamma_info->b_tbl.diff   = gIpu_vhdr_iq_default_gamma_diff_tbl;
	gamma_info->yb_tbl.full  = gIpu_vhdr_iq_default_gamma_full_tbl;
	gamma_info->yb_tbl.diff  = gIpu_vhdr_iq_default_gamma_diff_tbl;
}

static VOID ipu_vhdr_iq_get_gamma_tbl( T_FJ_IQ_GAMMA* gamma_info, T_FJ_IQ_SEL_INFO* sel_info )
{
	T_IM_R2Y_CTRL_GAMMA	gamma_ctrl;

	//	FJ_IQ_Get_GammaTbl(&gamma_info,sel_info); // get gamma table from IQ
	ipu_iq_default_set_vhdr_gamma_tbl(&gamma_ctrl,gamma_info);
	
}

static VOID ipu_vhdr_get_gamma_tbl( INT32* const gamma_tbl_type, VOID** gamma_tbl )
{
	*gamma_tbl_type = gIpu_vhdr_gamma_tbl_type;
	*gamma_tbl = (VOID*)gIpu_vhdr_gamma_tbl;
}

static VOID ipu_vhdr_set_gamma_tbl( T_FJ_IQ_SEL_INFO* sel_info )
{
	T_FJ_IQ_GAMMA gamma_info;
	
	ipu_vhdr_iq_get_gamma_tbl(&gamma_info,sel_info);
	
	gIpu_vhdr_gamma_tbl_type = gamma_info.gamma_mode;

	if(gIpu_vhdr_gamma_tbl_type == 1){		//10bit Full Input
		for( USHORT i = 0; i < 256; i++ ) {
			gIpu_vhdr_gamma_tbl[4*i+0] = ((gamma_info.rgb_tbl.diff[i] & 0x000000000000ffff ) >> 0  ) >> 6 ;
			gIpu_vhdr_gamma_tbl[4*i+1] = ((gamma_info.rgb_tbl.diff[i] & 0x00000000ffff0000 ) >> 16 ) >> 6 ;
			gIpu_vhdr_gamma_tbl[4*i+2] = ((gamma_info.rgb_tbl.diff[i] & 0x0000ffff00000000 ) >> 32 ) >> 6 ;
			gIpu_vhdr_gamma_tbl[4*i+3] = ((gamma_info.rgb_tbl.diff[i] & 0xffff000000000000 ) >> 48 ) >> 6 ;
		}
	}else if(gIpu_vhdr_gamma_tbl_type == 0){		//12bit Difference Input
		for( USHORT i = 0; i < 256; i++ ) {
			gIpu_vhdr_gamma_tbl[16*i+0] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  );
		
			gIpu_vhdr_gamma_tbl[16*i+1] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  );
		
			gIpu_vhdr_gamma_tbl[16*i+2] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  );
		
			gIpu_vhdr_gamma_tbl[16*i+3] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  );
		
			gIpu_vhdr_gamma_tbl[16*i+4] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  );
		
			gIpu_vhdr_gamma_tbl[16*i+5] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  );
		
			gIpu_vhdr_gamma_tbl[16*i+6] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  );
		
			gIpu_vhdr_gamma_tbl[16*i+7] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  );
		
			gIpu_vhdr_gamma_tbl[16*i+8] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000f00000000 ) >> 32  );
		
			gIpu_vhdr_gamma_tbl[16*i+9] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000f00000000 ) >> 32  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000f000000000 ) >> 36  );
		
			gIpu_vhdr_gamma_tbl[16*i+10] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000f00000000 ) >> 32  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000f000000000 ) >> 36  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000f0000000000 ) >> 40  );
		
			gIpu_vhdr_gamma_tbl[16*i+11] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000f00000000 ) >> 32  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000f000000000 ) >> 36  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000f0000000000 ) >> 40  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000f00000000000 ) >> 44  );
		
			gIpu_vhdr_gamma_tbl[16*i+12] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000f00000000 ) >> 32  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000f000000000 ) >> 36  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000f0000000000 ) >> 40  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000f00000000000 ) >> 44  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000f000000000000 ) >> 48  );
		
			gIpu_vhdr_gamma_tbl[16*i+13] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000f00000000 ) >> 32  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000f000000000 ) >> 36  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000f0000000000 ) >> 40  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000f00000000000 ) >> 44  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000f000000000000 ) >> 48  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00f0000000000000 ) >> 52  );
		
			gIpu_vhdr_gamma_tbl[16*i+14] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000f00000000 ) >> 32  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000f000000000 ) >> 36  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000f0000000000 ) >> 40  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000f00000000000 ) >> 44  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000f000000000000 ) >> 48  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00f0000000000000 ) >> 52  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0f00000000000000 ) >> 56  );
		
			gIpu_vhdr_gamma_tbl[16*i+15] = gamma_info.rgb_tbl.full[i] + 
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000000f ) >> 0  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000000f0 ) >> 4  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000000f00 ) >> 8  ) +
								((gamma_info.rgb_tbl.diff[i] & 0x000000000000f000 ) >> 12  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000000f0000 ) >> 16  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000000f00000 ) >> 20  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000000f000000 ) >> 24  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000000f0000000 ) >> 28  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000000f00000000 ) >> 32  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000000f000000000 ) >> 36  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00000f0000000000 ) >> 40  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0000f00000000000 ) >> 44  )+
								((gamma_info.rgb_tbl.diff[i] & 0x000f000000000000 ) >> 48  )+
								((gamma_info.rgb_tbl.diff[i] & 0x00f0000000000000 ) >> 52  )+
								((gamma_info.rgb_tbl.diff[i] & 0x0f00000000000000 ) >> 56  )+
								((gamma_info.rgb_tbl.diff[i] & 0xf000000000000000 ) >> 60  );
		
		}
	}

}

static INT32 ipu_vhdr_get_evc_table( IPC_SHORT *evc, const T_SHARE_APEX* apex )
{
	T_IPC_VHDR_HIST		hist_long;
	T_IPC_VHDR_HIST		hist_short;
	T_IPC_VHDR_BLD_TBL 	bld_tbl;

	IPC_UCHAR lut_lo2sh[256] = {0};
	INT32 hist_long_tmp[256];
	INT32 hist_short_tmp[256];

	T_IPU_VHDR_SET_PRM vhdr_set_prm = {0,240,1,128,0,0,0,240,E_IPU_VHDR_LOWPASS_FILTER_FRAME_OFF};
	T_IPU_VHDR_TABLE_DATA vhdr_table_data = {{0},{0},{0},{0}};

	T_IPC_HDRPRM_EVC_TBL hdr_evc_tbl = {0};
	T_IPC_HDRPRM_BLD_TBL hdr_bld_tbl;
	T_IPC_HDRPRM_HIST hdr_hist_long = {0};
	T_IPC_HDRPRM_HIST hdr_hist_short = {0};
	INT32 	i, j;
	INT32	ercd;
	FLOAT bias_EV = 0.0;
	UCHAR th_y_ofs = 0;
	UCHAR min_bld_ofs = 0;

	memset( &lut_lo2sh, 0, sizeof( lut_lo2sh ) );
	memset( &bld_tbl, 0, sizeof( bld_tbl ) );

	memset( &hist_long_tmp, 0, sizeof( hist_long_tmp ) );
	memset( &hist_short_tmp, 0, sizeof( hist_short_tmp ) );

	// calc blend table
	for( i = 0; i < 128; i++ ) {
		hist_long_tmp[i*2] = gIpu_vhdr_hist[0][i];
		hist_long_tmp[i*2+1] = gIpu_vhdr_hist[0][i];
	}
	
	hist_long.bin_num =256;
	hist_long.freq_bit_depth = 24;
	hist_long.addr = hist_long_tmp;
	hdr_hist_long.bin_num =hist_long.bin_num;
	hdr_hist_long.freq_bit_depth = hist_long.freq_bit_depth;
	hdr_hist_long.addr = hist_long_tmp;
	
	for( i = 0; i < 128; i++ ) {
		hist_short_tmp[i*2] = gIpu_vhdr_hist[1][i];
		hist_short_tmp[i*2+1] = gIpu_vhdr_hist[1][i];
	}
	
	hist_short.bin_num = 256;
	hist_short.freq_bit_depth = 24;
	hist_short.addr = hist_short_tmp;
	hdr_hist_short.bin_num = hist_short.bin_num;
	hdr_hist_short.freq_bit_depth = hist_short.freq_bit_depth;
	hdr_hist_short.addr = hist_short_tmp;

	// adjust blending ratio based on EV bias of long frame
	bias_EV = RN_FIX_VAL(apex->sensor[0].bias_EV);
	if( bias_EV > 0.0) {
		th_y_ofs = (UCHAR)(bias_EV * 16.0 - (256 - D_IPU_VHDR_BLD_TH_Y));
		min_bld_ofs = (UCHAR)(bias_EV * 128.0 - D_IPU_VHDR_BLD_MIN_BLD_RATIO);
//		th_y_ofs = (UCHAR)(bias_EV * 32.0 - (256 - D_IPU_VHDR_BLD_TH_Y));
//		min_bld_ofs = (UCHAR)(bias_EV * 64.0 - D_IPU_VHDR_BLD_MIN_BLD_RATIO);
	}
	// Set vhdr parameter
	vhdr_set_prm.mode = 0;
	vhdr_set_prm.bld_th_y 			= D_IPU_VHDR_BLD_TH_Y - th_y_ofs;
	vhdr_set_prm.bld_calc_mode 		= D_IPU_VHDR_BLD_CALC_MODE;
	vhdr_set_prm.bld_min_bld_ratio 	= D_IPU_VHDR_BLD_MIN_BLD_RATIO + min_bld_ofs;
	vhdr_set_prm.evc_flag		 	= D_IPU_VHDR_EVC_FLAG;
	vhdr_set_prm.evc_type 			= D_IPU_VHDR_EVC_TYPE;
	vhdr_set_prm.evc_calc_mode 		= D_IPU_VHDR_EVC_MODE;
	vhdr_set_prm.evc_th_Y 			= D_IPU_VHDR_EVC_TH_Y;
	vhdr_set_prm.e_lowpass_frame	= E_IPU_VHDR_LOWPASS_FILTER_FRAME_OFF;
	
	ercd = IPC_VHDR_Calc_Blend_Table(
						vhdr_set_prm.bld_th_y,
						lut_lo2sh,
						&hist_long,
						&hist_short,
						vhdr_set_prm.bld_calc_mode,
						vhdr_set_prm.bld_min_bld_ratio,
						&bld_tbl);
	if( ercd != D_IPC_VHDR_OK ){
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}
	
	for(i = 0; i < 7 ; i++){
		hdr_bld_tbl.bldofs[i]	= bld_tbl.bldofs[i];
		hdr_bld_tbl.bldsl[i]	= bld_tbl.bldsl[i];
		hdr_bld_tbl.bldb[i] 	= bld_tbl.bldb[i];
	}
	ercd = IPC_HDRPRM_Get_EVC_Table(
						vhdr_set_prm.evc_th_Y, 
						lut_lo2sh,
						&hdr_hist_long,
						&hdr_hist_short,
						vhdr_set_prm.evc_calc_mode,
						vhdr_set_prm.evc_type,
						&hdr_bld_tbl,
						&hdr_evc_tbl);
	if( ercd != D_IPC_HDRPRM_OK ){
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}
	
	// set vhdr table
	for(i = 0; i < 10 ; i++){
		vhdr_table_data.evc[i]		= hdr_evc_tbl.evknb[i] & 0xFFF;
		vhdr_table_data.evc[i + 10] = hdr_evc_tbl.evknofs[i] & 0x3FFF;
		vhdr_table_data.evc[i + 20] = hdr_evc_tbl.evknsl[i];
	}
	for(i = 0; i < 7 ; i++){
		vhdr_table_data.bldofs[i]	= bld_tbl.bldofs[i];
		vhdr_table_data.bldsl[i]	= bld_tbl.bldsl[i];
		vhdr_table_data.bldb[i] 	= bld_tbl.bldb[i];
	}
	
	for(i = 59; i > 0 ; i--){
		for(j = 0; j < 7; j++){
			vhdrTableData[i].bldofs[j] = vhdrTableData[i - 1].bldofs[j];
			vhdrTableData[i].bldsl[j] = vhdrTableData[i - 1].bldsl[j];
			vhdrTableData[i].bldb[j] = vhdrTableData[i - 1].bldb[j];
		}
		for(j = 0; j < 30; j++){
			vhdrTableData[i].evc[j] = vhdrTableData[i - 1].evc[j];
		}
	}
	for(j = 0; j < 7; j++){
		vhdrTableData[0].bldofs[j] = vhdr_table_data.bldofs[j];
		vhdrTableData[0].bldsl[j] = vhdr_table_data.bldsl[j];
		vhdrTableData[0].bldb[j] = vhdr_table_data.bldb[j];
	}
	for(j = 0; j < 30; j++){
		vhdrTableData[0].evc[j] = vhdr_table_data.evc[j];
	}

	{
		UINT32 bldofs_buf[7] = {0};
		UINT32 bldsl_buf[7] = {0};
		UINT32 bldb_buf[7] = {0};
		INT32 evc_buf[30] = {0};
		UINT32 e_lowpass_frame = vhdr_set_prm.e_lowpass_frame;
		
		for(i = 0; i < e_lowpass_frame ; i++){
			for(j = 0; j < 7; j++){
				bldofs_buf[j] += vhdrTableData[i].bldofs[j];
				bldsl_buf[j] += vhdrTableData[i].bldsl[j];
				bldb_buf[j] += vhdrTableData[i].bldb[j];
			}
			for(j = 0; j < 30; j++){
				evc_buf[j] += vhdrTableData[i].evc[j];
			}
		}
		
		for(j = 0; j < 7; j++){
			vhdr_table_data.bldofs[j] = bldofs_buf[j] / e_lowpass_frame;
			vhdr_table_data.bldsl[j] = bldsl_buf[j] / e_lowpass_frame;
			vhdr_table_data.bldb[j] = bldb_buf[j] / e_lowpass_frame;
		}
		for(j = 0; j < 30; j++){
			vhdr_table_data.evc[j] = (SHORT)(evc_buf[j] / e_lowpass_frame);
		}
	}
	
	if(vhdr_set_prm.evc_flag){
		//evc table change.
		for(i = 0; i < 10 ; i++){
			evc[i] = vhdr_table_data.evc[i] & 0xFFF;
			evc[i + 10] = vhdr_table_data.evc[i + 10] & 0x3FFF;
			evc[i + 20] = vhdr_table_data.evc[i + 20];
		}
	}
	
	return D_IPC_VHDR_OK;
}

static VOID ipu_vhdr_int_handler_shdr( ULONG interrupt_flag_mask )
{
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_VHDR, FJ_MBALOG_FUNC_ID_HDR, FJ_MBALOG_TYPE_END, "vHDR Start"));
	
	Snd_Event_to_System( E_SYSTEM_EVENT_DOL_COMBINE, gFrame_No_vhdr );
}

