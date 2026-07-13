/**
 * @file		stm_sgde2shdr.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "mode_stm.h"
#include "ctrl_shdr.h"
#include "camera_config.h"
#include "fj_still.h"
#include "fj_memory.h"
#include "fj_filesystem.h"
#include "movie_video.h"
#include "sdram_map_movie_record.h"
#include "category_parameter.h"
#include "still.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#ifdef D_IPU_3DNR_PROCESS_BY_TASK
#define D_3DNR_TSK_BY_SEQUENTIAL

#ifdef D_3DNR_TSK_BY_SEQUENTIAL
#define D_3DNR_SKIP_FRAME_IIP_IN	(0)
#define D_3DNR_SKIP_FRAME_GMD_IN	(0)
#define D_3DNR_SKIP_FRAME_ME_IN		(0)
#define D_3DNR_SKIP_FRAME_SHDR_IN	(0)
#else
#define D_3DNR_SKIP_FRAME_IIP_IN	(D_SGDE_MEM_BANK_YUV - 1)
#define D_3DNR_SKIP_FRAME_GMD_IN	(D_SGDE_MEM_BANK_YUV - 1)
#define D_3DNR_SKIP_FRAME_ME_IN		(D_SHDR_MEM_BANK_YUV - 1)
#define D_3DNR_SKIP_FRAME_SHDR_IN	(D_SGDE_MEM_BANK_YUV - 2)
#endif

#else
#define D_3DNR_SKIP_FRAME_IIP_IN	(0)
#define D_3DNR_SKIP_FRAME_GMD_IN	(0)
#define D_3DNR_SKIP_FRAME_ME_IN		(0)
#define D_3DNR_SKIP_FRAME_SHDR_IN	(0)
#endif


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define BANK_SET_AND_INC_NOLOCK(a, b) a[E_MEM_BANK_ING] = a[E_MEM_BANK_SET];\
									  a[E_MEM_BANK_SET] = ( a[E_MEM_BANK_SET] + 1) % b;

#define BANK_SET_AND_INC_LOCK(a, b, mem, lock) 	a[E_MEM_BANK_ING] = a[E_MEM_BANK_SET];\
												a[E_MEM_BANK_SET] = ( a[E_MEM_BANK_SET] + 1) % b;\
												if( mem[ a[E_MEM_BANK_SET] ] == lock ){\
													a[E_MEM_BANK_SET] = ( a[E_MEM_BANK_SET] + 1) % b;\
												}

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	FJ_GYRO_DATA_SAMPLE				gyro_data;
	FJ_GYRO_DATA_SAMPLE				accel_data;
} FJ_GYRO_CALI_INFO;

typedef struct {
	FJ_IMG_CONV_CTRL	ctrl;
	ULONG				addr_y;
	ULONG				addr_c;
} T_SCALE_INFO;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_STM_IPU2SHDR_IN  		g_i2s_in;
static T_IPU2SHDR_MNG	 		g_i2s_mng;
static T_SHARE_PIPE_YUV			g_share_pipe_yuv;
static T_SHARE_YUV				g_share_vhdr_yuv;
static T_SHARE_PIPE_DOL_YUV		g_share_pipe_dol_yuv;
static T_SHARE_R2Y_DOL_HIST		g_share_r2y_dol_hist;
static T_SHARE_R2Y_HIST			g_share_r2y_hist;
static T_SHARE_DOL_COMBINE		g_share_dol_combine;
static T_SHARE_YUV				g_share_sgde_yuv;
static T_SHARE_YUV				g_share_shdr_yuv;
static UCHAR					g_rbk2layout_tbl[E_SHDR_OUT_MAX];
static UCHAR					g_shdr_ref_rbk = E_SHDR_OUT_NONE;
static T_SCALE_INFO				g_scale_info[2];
static UCHAR					g_scale_idx=0;
static ULLONG					g_pip_frame_no=0;




#ifdef D_IPU_3DNR_PROCESS_BY_TASK
#define D_3DNR_TSK_SKIP_FRAME	(5)

static ULLONG gFrame_No_ShdrTsk = 0;
static ULLONG gFrame_No_VhdrTsk = 0;

static ULLONG gFrame_No_GmdTsk = 0;
static ULLONG gFrame_No_MeTsk = 0;

static ULLONG gFrameSkip_No_ShdrTsk = 0;
static ULLONG g3dnrSkip_No_ShdrTsk = 0;
#endif
static UCHAR					g_sgdebank_cnt;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void get_AutoFraming_Win_Pos(T_SHDR_MNG_OUT dstImage, UINT32* winSizeW, UINT32* winSizeH, UINT32* offsetY, UINT32* offsetC);
static void get_AutoFraming_Face_Pos(T_SHDR_MNG_IN srcImage, UINT32 *faceWinW, UINT32 *faceWinH, UINT32* offsetY, UINT32* offsetC);
static void shdr_reconfig( ULLONG frame_no );

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void get_AutoFraming_Win_Pos(T_SHDR_MNG_OUT dstImage, UINT32* winSizeW, UINT32* winSizeH, UINT32* offsetY, UINT32* offsetC) {
	UINT32 winX, winY, winW, winH, ofstY, ofstC;
	UINT32 devide, position;
	
	// cal win position
	Get_Share_AutoFraming_Win_Pos(&devide, &position);
	devide += 2;
	
	if ( devide > 5 )
		devide = 2;
	winW = ((FLOAT)dstImage.yuv_h_size/(FLOAT)devide);
	winH = ((FLOAT)dstImage.yuv_v_size/(FLOAT)devide);
	winW &= 0xFFFFFFFC;
	winH &= 0xFFFFFFFC;

	if ( position >= (devide*devide) )
		position = 0;
	winX = (position % devide) * winW;
	winY = (position / devide) * winH;

	ofstY = winY*dstImage.yuv_h_byte + winX;
	if( dstImage.yuv_format == E_SHARE_YUV_FORMAT_422_SEMI ){
		ofstC = ofstY;
	} else {
		ofstC = (winY*dstImage.yuv_h_byte)/2 + winX;
	}

	ofstY &= 0xFFFFFFF8;
	ofstC &= 0xFFFFFFF8;

	*winSizeW = winW;
	*winSizeH = winH;
	*offsetY  = ofstY;
	*offsetC  = ofstC;
}

static void get_AutoFraming_Face_Pos(T_SHDR_MNG_IN srcImage, UINT32 *faceWinW, UINT32 *faceWinH, UINT32* offsetY, UINT32* offsetC)
{
	T_SHARE_SYSTEM_PARAM g_share_prm;
	INT32 faceX=0, faceY=0, faceW=0, faceH=0, oriW=0, oriH=0;
	static INT32 lastFaceX=0, lastFaceY=0, lastFaceW=0, lastFaceH=0;
	static UINT32 frameCnt=0;
	UINT32 faceWinDelay = 20;
	INT32 faceWinDiff = 200; /* full=4K */
	
	Get_Share_System_Param(&g_share_prm);
	
	if (g_share_prm.num_of_face) {
		// "face movement > 200" & "face window stop > 20 frames", update face window position
		if ( ((g_share_prm.fd_info[0].rect.left - lastFaceX) > faceWinDiff) ||
			 ((g_share_prm.fd_info[0].rect.left - lastFaceX) < (-faceWinDiff)) ||
			 ((g_share_prm.fd_info[0].rect.top - lastFaceY) > faceWinDiff) ||
			 ((g_share_prm.fd_info[0].rect.top - lastFaceY) < (-faceWinDiff)) ) {
			 if (! (frameCnt%faceWinDelay) ) {
				lastFaceX = faceX = g_share_prm.fd_info[0].rect.left;
				lastFaceY = faceY = g_share_prm.fd_info[0].rect.top;
				lastFaceW = faceW = g_share_prm.fd_info[0].rect.width;
				lastFaceH = faceH = g_share_prm.fd_info[0].rect.height;
			 }
			frameCnt++;
		}
		faceX = lastFaceX;
		faceY = lastFaceY;
		faceW = lastFaceW;
		faceH = lastFaceH;
	}
	else {
		frameCnt  = 0;
		*faceWinW = 0;
		*faceWinH = 0;
		*offsetY  = 0;
		*offsetC  = 0;
		return;
	}
	
	// from sgde_user1 = 1/4 full size
	faceX /= 2;
	faceY /= 2;
	faceW /= 2;
	faceH /= 2;
	
	// enlarge window & ratio=16:9
	oriW = faceW;
	oriH = faceH;
	faceH *= 2;
	if ( faceH > srcImage.yuv_v_size ) {
		faceH = srcImage.yuv_v_size;
	}
	faceW = (faceH*16) / 9;
	faceW &= 0xFFFFFFFE;
	faceH &= 0xFFFFFFFE;
	
	faceX -= ((faceW-oriW)/2);
	faceY -= ((faceH-oriH)/2);
	if ( (faceX+faceW) > srcImage.yuv_h_size ) {
		faceX = srcImage.yuv_h_size - faceW;
	}
	if ( (faceY+faceH) > srcImage.yuv_v_size ) {
		faceY = srcImage.yuv_v_size - faceH;
	}
	if (faceX<0)
		faceX = 0;
	if (faceY<0)
		faceY = 0;
	
	faceX &= 0xFFFFFFFE;
	faceY &= 0xFFFFFFFE;
	
	*faceWinW = (UINT32)faceW;
	*faceWinH = (UINT32)faceH;
	*offsetY = (UINT32)(srcImage.yuv_h_byte*faceY+faceX);
	*offsetC = (UINT32)(srcImage.yuv_h_byte*faceY/2+faceX); // 420_SEMI
}

static void shdr_reconfig( ULLONG frame_no )
{
	T_SHARE_SYSTEM_CONFIG config;
	UCHAR i;
	E_SHDR_OUT_RBK rbk = E_SHDR_OUT_RBK4_YUV_0;
	E_SHARE_YUV_OUT layout_max = E_SHARE_YUV_OUT_H265;
	E_SHDR_OUT_RBK 	rbk_265 = E_SHDR_OUT_RBK0_H265;
	USHORT max_h = 0, max_v = 0;
	ULONG  max_hv = 0;
	
	UPRINTF("[E_SYSTEM_EVENT_VIEW_RECONFIG] %s(%d)\n", __func__, __LINE__);
	Get_Share_System_Config( &config );
	
	UPRINTF("============OLD============\n");
	UPRINTF("RBK0_H265   (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK0_H265], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].yuv_v_size);
	UPRINTF("RBK2_H264_0 (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK2_H264_0], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].yuv_v_size);
	UPRINTF("RBK3_H264_1 (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK3_H264_1], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].yuv_v_size);
	UPRINTF("RBK4_YUV_0  (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK4_YUV_0], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_v_size);
	UPRINTF("RBK5_YUV_1  (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK5_YUV_1], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size);
	UPRINTF("===========================\n");
	
	max_h = 0;
	max_v = 0;
	max_hv = 0;
	// Find Max
	for( i = E_SHARE_YUV_OUT_H265; i < E_SHARE_YUV_OUT_RESERVE; i ++ ){
		if(config.sensor_yuv[i].h_size > 0){
			if(config.sensor_yuv[i].h_size * config.sensor_yuv[i].v_size > max_hv){
				max_h = config.sensor_yuv[i].h_size;
				max_v = config.sensor_yuv[i].v_size;
				max_hv = (ULONG)max_h * max_v;
				layout_max = i;

				switch(i)
				{
					case E_SHARE_YUV_OUT_H265:
						rbk_265 = E_SHDR_OUT_RBK0_H265;
					break;
					case E_SHARE_YUV_OUT_H264_0:
						rbk_265 = E_SHDR_OUT_RBK2_H264_0;
					break;
					case E_SHARE_YUV_OUT_H264_1:
						rbk_265 = E_SHDR_OUT_RBK3_H264_1;
					break;
					case E_SHARE_YUV_OUT_USER_0:
						rbk_265 = E_SHDR_OUT_RBK4_YUV_0;
					break;
					case E_SHARE_YUV_OUT_USER_1:
						rbk_265 = E_SHDR_OUT_RBK5_YUV_1;
					break;
					default:
						rbk_265 = E_SHDR_OUT_NONE;
					break;
				}
			}
		}
	}
	UPRINTF("Layout_Max:%d RBK_265:%d\n", layout_max, rbk_265);
	
	// SHDR
	for( i = E_SHARE_YUV_OUT_H265; i < E_SHARE_YUV_OUT_MAX; i ++ ){
		if(layout_max == i){
			rbk = E_SHDR_OUT_RBK0_H265;
		}
		else{
			switch(i)
			{
				case E_SHARE_YUV_OUT_H265:
					rbk = rbk_265;
				break;
				case E_SHARE_YUV_OUT_H264_0:
					rbk = E_SHDR_OUT_RBK2_H264_0;
				break;
				case E_SHARE_YUV_OUT_H264_1:
					rbk = E_SHDR_OUT_RBK3_H264_1;
				break;
				case E_SHARE_YUV_OUT_USER_0:
					rbk = E_SHDR_OUT_RBK4_YUV_0;
				break;
				case E_SHARE_YUV_OUT_USER_1:
					rbk = E_SHDR_OUT_RBK5_YUV_1;
				break;
				default:
					rbk = E_SHDR_OUT_NONE;
				break;
			}
		}
		
		if(rbk != E_SHDR_OUT_NONE)
		{
			g_rbk2layout_tbl[rbk] = i;
			// Calc Size
			g_i2s_mng.shdr_out[rbk].yuv_h_byte = config.sensor_yuv[i].h_size;
			g_i2s_mng.shdr_out[rbk].yuv_h_size = config.sensor_yuv[i].h_size;
			g_i2s_mng.shdr_out[rbk].yuv_v_size = config.sensor_yuv[i].v_size;
			g_i2s_mng.shdr_out[rbk].yuv_format = config.sensor_yuv[i].format;
			g_i2s_mng.shdr_out[rbk].yuv_video = config.sensor_yuv[i].video;
			if(g_i2s_mng.shdr_out[rbk].yuv_video != E_SHARE_YUV_VIDEO_NONE){
				g_i2s_mng.shdr_out[rbk].yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[rbk].yuv_h_size, D_IPU_ALIGN_VIDEO_G_WIDTH);
			}
			else{
				g_i2s_mng.shdr_out[rbk].yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[rbk].yuv_h_size, 16);
			}
			g_i2s_in.yuv_ask_user_for_video_addr[i] = config.sensor_yuv[i].ask_user_for_video_addr;
		}
	}
	
	rbk = E_SHDR_OUT_RBK0_H265;
	if((g_i2s_in.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_ME) && (g_i2s_in.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE))
	{
		// SGDE
		// Calc Size
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_video = E_SHARE_YUV_VIDEO_NONE;
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size = g_i2s_mng.shdr_out[rbk].yuv_h_size + D_IPU_3DNR_MARGIN; // with margin
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[rbk].yuv_v_size, 16);
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size = g_i2s_mng.shdr_out[rbk].yuv_v_size + D_IPU_3DNR_MARGIN; // with margin
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_byte = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size;
		g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_video = E_SHARE_YUV_VIDEO_NONE;
		g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size / 4;
		g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size / 4;
		g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size, D_IPU_ALIGN_ME_G_WIDTH);
	
		g_i2s_mng.shdr_out[rbk].yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size - D_IPU_3DNR_MARGIN;
		g_i2s_mng.shdr_out[rbk].yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size - D_IPU_3DNR_MARGIN;
			
		// Make IIP In Info.
		g_i2s_mng.iip_out.yuv_format = E_SHARE_YUV_FORMAT_NONE;
		g_i2s_mng.iip_out.yuv_video = E_SHARE_YUV_VIDEO_264;
		g_i2s_mng.iip_out.yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size;
		g_i2s_mng.iip_out.yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size;
		g_i2s_mng.iip_out.yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.iip_out.yuv_h_size, D_IPU_ALIGN_ME_G_WIDTH);
			
		// Make ME Out Info.
		g_i2s_mng.me_out.yuv_format = E_SHARE_YUV_FORMAT_422_SEMI;
		g_i2s_mng.me_out.yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size/4;
		g_i2s_mng.me_out.yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size/4;
		g_i2s_mng.me_out.yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.me_out.yuv_h_size/4*2, D_IPU_ALIGN_ME_G_WIDTH);
	}
	
	UPRINTF("============NOW============\n");
	UPRINTF("RBK0_H265   (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK0_H265], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].yuv_v_size);
	UPRINTF("RBK2_H264_0 (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK2_H264_0], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].yuv_v_size);
	UPRINTF("RBK3_H264_1 (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK3_H264_1], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].yuv_v_size);
	UPRINTF("RBK4_YUV_0  (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK4_YUV_0], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_v_size);
	UPRINTF("RBK5_YUV_1  (0x%02x)GW:%04u [%04u x %04u]\n", g_rbk2layout_tbl[E_SHDR_OUT_RBK5_YUV_1], g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_byte, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size, g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size);
	UPRINTF("===========================\n");
}

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 STM_IPU2SHDR_Config( const T_STM_IPU2SHDR_IN* in, T_STM_IPU2SHDR_OUT* const out )
{
	UCHAR i, j;
	E_SHDR_OUT_RBK rbk = E_SHDR_OUT_RBK4_YUV_0;
	UINT32  mem_size_y[E_SHDR_OUT_MAX],mem_size_c[E_SHDR_OUT_MAX];
	UINT32	mem_addr_ch0, mem_addr_ch1, mem_addr_tmp;
	ULONG	yuv0_ch, yuv1_ch;
	ULONG 	h265_map_addr, h265_map_size;

	E_SHARE_YUV_OUT layout_max = E_SHARE_YUV_OUT_H265;
	E_SHDR_OUT_RBK 	rbk_265 = E_SHDR_OUT_RBK0_H265;
	USHORT max_h = 0, max_v = 0;
	ULONG  max_hv = 0;
	
	/* clear 0 */
	memset( (VOID*)&g_i2s_mng, 0, sizeof(g_i2s_mng) );
	memset( (VOID*)&g_rbk2layout_tbl, E_SHDR_OUT_NONE, sizeof(g_rbk2layout_tbl) );
	memset( (VOID*)&g_share_pipe_yuv, 0, sizeof(g_share_pipe_yuv));
	memset( (VOID*)&g_share_vhdr_yuv, 0, sizeof(g_share_vhdr_yuv));
	memset( (VOID*)&g_share_pipe_dol_yuv, 0, sizeof(g_share_pipe_dol_yuv));
	memset( (VOID*)&g_share_r2y_dol_hist, 0, sizeof(g_share_r2y_dol_hist));
	memset( (VOID*)&g_share_r2y_hist, 0, sizeof(g_share_r2y_hist));
	memset( (VOID*)&g_share_dol_combine, 0, sizeof(g_share_dol_combine));
	memset( (VOID*)&g_share_sgde_yuv, 0, sizeof(g_share_sgde_yuv));
	memset( (VOID*)&g_share_shdr_yuv, 0, sizeof(g_share_shdr_yuv));
	memset( (VOID*)&mem_size_y, 0, sizeof(mem_size_y) );
	memset( (VOID*)&mem_size_c, 0, sizeof(mem_size_c) );
	g_sgdebank_cnt = 0;
	
	// Save input
	g_i2s_in = *in;

	mem_addr_ch0 = g_i2s_in.available_memory_addr_ch0;
	mem_addr_ch1 = g_i2s_in.available_memory_addr_ch1;

	g_i2s_mng.sen_stream.data = g_i2s_in.sen_stream.data;

	// Set debug
	Debug_3DNR( g_i2s_in.debug_3dnr_0, g_i2s_in.debug_3dnr_1 );
	
	// set out
	if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_PIPE_ONLY || g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY )
	{
		out->available_memory_addr_ch0 = mem_addr_ch0;
		out->available_memory_addr_ch1 = mem_addr_ch1;
		return D_STM_OK;
	}
	
	max_h = 0;
	max_v = 0;
	max_hv = 0;
	// Find Max
	for( i = E_SHARE_YUV_OUT_H265; i < E_SHARE_YUV_OUT_RESERVE; i ++ ){
		if(g_i2s_in.yuv_h_size[i] > 0){
			if(g_i2s_in.yuv_h_size[i] * g_i2s_in.yuv_v_size[i] > max_hv){
				max_h = g_i2s_in.yuv_h_size[i];
				max_v = g_i2s_in.yuv_v_size[i];
				max_hv = (ULONG)max_h * max_v;
				layout_max = i;

				switch(i)
				{
					case E_SHARE_YUV_OUT_H265:
						rbk_265 = E_SHDR_OUT_RBK0_H265;
					break;
					case E_SHARE_YUV_OUT_H264_0:
						rbk_265 = E_SHDR_OUT_RBK2_H264_0;
					break;
					case E_SHARE_YUV_OUT_H264_1:
						rbk_265 = E_SHDR_OUT_RBK3_H264_1;
					break;
					case E_SHARE_YUV_OUT_USER_0:
						rbk_265 = E_SHDR_OUT_RBK4_YUV_0;
					break;
					case E_SHARE_YUV_OUT_USER_1:
						rbk_265 = E_SHDR_OUT_RBK5_YUV_1;
					break;
					default:
						rbk_265 = E_SHDR_OUT_NONE;
					break;
				}
			}
		}
	}
	UPRINTF("Layout_Max:%d RBK_265:%d\n", layout_max, rbk_265);
	
	// SHDR
	for( i = E_SHARE_YUV_OUT_H265; i < E_SHARE_YUV_OUT_MAX; i ++ ){
		if(layout_max == i){
			rbk = E_SHDR_OUT_RBK0_H265;
		}
		else{
			switch(i)
			{
				case E_SHARE_YUV_OUT_H265:
					rbk = rbk_265;
				break;
				case E_SHARE_YUV_OUT_H264_0:
					rbk = E_SHDR_OUT_RBK2_H264_0;
				break;
				case E_SHARE_YUV_OUT_H264_1:
					rbk = E_SHDR_OUT_RBK3_H264_1;
				break;
				case E_SHARE_YUV_OUT_USER_0:
					rbk = E_SHDR_OUT_RBK4_YUV_0;
				break;
				case E_SHARE_YUV_OUT_USER_1:
					rbk = E_SHDR_OUT_RBK5_YUV_1;
				break;
				default:
					rbk = E_SHDR_OUT_NONE;
				break;
			}
		}
		
		if(rbk != E_SHDR_OUT_NONE)
		{
			g_rbk2layout_tbl[rbk] = i;
			
			// Calc Size
			g_i2s_mng.shdr_out[rbk].yuv_h_byte = g_i2s_in.yuv_h_size[i];
			g_i2s_mng.shdr_out[rbk].yuv_h_size = g_i2s_in.yuv_h_size[i];
			g_i2s_mng.shdr_out[rbk].yuv_v_size = g_i2s_in.yuv_v_size[i];
			g_i2s_mng.shdr_out[rbk].yuv_format = g_i2s_in.yuv_format[i];
			g_i2s_mng.shdr_out[rbk].yuv_video = g_i2s_in.yuv_video[i];

			if(g_i2s_mng.shdr_out[rbk].yuv_video != E_SHARE_YUV_VIDEO_NONE)
			{
				g_i2s_mng.shdr_out[rbk].yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[rbk].yuv_h_size, D_IPU_ALIGN_VIDEO_G_WIDTH);
				if((g_i2s_in.mem_width == 0) || (g_i2s_in.mem_height == 0)){
					mem_size_y[rbk] = g_i2s_mng.shdr_out[rbk].yuv_h_byte * M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[rbk].yuv_v_size, D_IPU_ALIGN_VIDEO_G_LINE);
				}
				else{
					mem_size_y[rbk] = g_i2s_in.mem_width * M_IPU_MAKE_ALIGNMENT(g_i2s_in.mem_height, D_IPU_ALIGN_VIDEO_G_LINE);
				}
			}
			else
			{
				g_i2s_mng.shdr_out[rbk].yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[rbk].yuv_h_size, 16);
				if((g_i2s_in.mem_width == 0) || (g_i2s_in.mem_height == 0)){
					mem_size_y[rbk] = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[rbk].yuv_h_byte, D_IPU_ALIGN_MEM_SIZE) * g_i2s_mng.shdr_out[rbk].yuv_v_size;
				}
				else{
					mem_size_y[rbk] = M_IPU_MAKE_ALIGNMENT(g_i2s_in.mem_width, D_IPU_ALIGN_MEM_SIZE) * g_i2s_in.mem_height;
				}
			}
			if( g_i2s_mng.shdr_out[rbk].yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
				mem_size_c[rbk] = mem_size_y[rbk] / 2;
			}
			else{
				mem_size_c[rbk] = mem_size_y[rbk];
			}

			if((rbk == E_SHDR_OUT_RBK4_YUV_0) || (rbk == E_SHDR_OUT_RBK5_YUV_1) ){
				if((g_i2s_in.cap_mode == TRUE) && (rbk == E_SHDR_OUT_RBK4_YUV_0)){
					mem_addr_tmp = mem_addr_ch0;
				}else{
					#ifdef CO_MEMORY_1GB
					if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR){
						mem_addr_tmp = mem_addr_ch0;
					}else{
						mem_addr_tmp = mem_addr_ch1;
					}
					#else
					mem_addr_tmp = mem_addr_ch1;
					#endif
				}
			}else{
				mem_addr_tmp = in->h265_ch0 ? mem_addr_ch0 : mem_addr_ch1; // E_SHDR_OUT_RBK0_H265 will get CH0 address from MOVIE memory map or // For encoder YUV in case, bandwidth issue?
			}
			
			// Calc Memory
			if((rbk != E_SHDR_OUT_RBK2_H264_0) && (rbk != E_SHDR_OUT_RBK3_H264_1))
			{
				if((rbk == E_SHDR_OUT_RBK0_H265) && (in->h265_map_adr == TRUE)){
					if((g_i2s_in.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_NO_ME) && (g_i2s_in.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE) ){
						h265_map_addr = Sdram_Map_Movie_Record_Get_H265_Temp_Addr_Y(in->h265_map_ch);
						h265_map_size = Sdram_Map_Movie_Record_Get_H265_Temp_Size_Y(in->h265_map_ch);
						
						for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
							g_i2s_mng.shdr_mem_addr_y[rbk][j] = h265_map_addr;
							h265_map_addr = g_i2s_mng.shdr_mem_addr_y[rbk][j] + h265_map_size;
						}

						h265_map_addr = Sdram_Map_Movie_Record_Get_H265_Temp_Addr_C(in->h265_map_ch);
						h265_map_size = Sdram_Map_Movie_Record_Get_H265_Temp_Size_C(in->h265_map_ch);
						
						for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
							g_i2s_mng.shdr_mem_addr_c[rbk][j] = h265_map_addr;
							h265_map_addr = g_i2s_mng.shdr_mem_addr_c[rbk][j] + h265_map_size;
						}
					}
				}else if (mem_size_y[rbk]) {
					// Only assign memoery for USER_0/1 and H265(3DNR) since H264_0/1 memory from MOVIE memory_map
					if(g_i2s_mng.shdr_out[rbk].yuv_video != E_SHARE_YUV_VIDEO_NONE) {
						mem_addr_tmp = M_IPU_MAKE_ALIGNMENT(mem_addr_tmp, D_IPU_ALIGN_VIDEO_ADDR);
					}
					if( g_i2s_in.yuv_packed ){
						for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
							g_i2s_mng.shdr_mem_addr_y[rbk][j] = mem_addr_tmp;
							mem_addr_tmp = g_i2s_mng.shdr_mem_addr_y[rbk][j] + mem_size_y[rbk];
							if(g_i2s_mng.shdr_out[rbk].yuv_video != E_SHARE_YUV_VIDEO_NONE) {
								mem_addr_tmp = M_IPU_MAKE_ALIGNMENT(mem_addr_tmp, D_IPU_ALIGN_VIDEO_ADDR);
							}
						//}
						//for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
							g_i2s_mng.shdr_mem_addr_c[rbk][j] = mem_addr_tmp;
							mem_addr_tmp = g_i2s_mng.shdr_mem_addr_c[rbk][j] + mem_size_c[rbk];
							if(g_i2s_mng.shdr_out[rbk].yuv_video != E_SHARE_YUV_VIDEO_NONE) {
								mem_addr_tmp = M_IPU_MAKE_ALIGNMENT(mem_addr_tmp, D_IPU_ALIGN_VIDEO_ADDR);
							}
						}
					}
					else{
						for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
							g_i2s_mng.shdr_mem_addr_y[rbk][j] = mem_addr_tmp;
							mem_addr_tmp = g_i2s_mng.shdr_mem_addr_y[rbk][j] + mem_size_y[rbk];
							if(g_i2s_mng.shdr_out[rbk].yuv_video != E_SHARE_YUV_VIDEO_NONE) {
								mem_addr_tmp = M_IPU_MAKE_ALIGNMENT(mem_addr_tmp, D_IPU_ALIGN_VIDEO_ADDR);
							}
						}
						for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
							g_i2s_mng.shdr_mem_addr_c[rbk][j] = mem_addr_tmp;
							mem_addr_tmp = g_i2s_mng.shdr_mem_addr_c[rbk][j] + mem_size_c[rbk];
							if(g_i2s_mng.shdr_out[rbk].yuv_video != E_SHARE_YUV_VIDEO_NONE) {
								mem_addr_tmp = M_IPU_MAKE_ALIGNMENT(mem_addr_tmp, D_IPU_ALIGN_VIDEO_ADDR);
							}
						}
					}
				}
			}

			if((rbk == E_SHDR_OUT_RBK4_YUV_0) || (rbk == E_SHDR_OUT_RBK5_YUV_1) ){
				if((g_i2s_in.cap_mode == TRUE) && (rbk == E_SHDR_OUT_RBK4_YUV_0)){
					mem_addr_ch0 = mem_addr_tmp;
				}else{
					#ifdef CO_MEMORY_1GB
					if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR){
						mem_addr_ch0 = mem_addr_tmp;
					}else{
						mem_addr_ch1 = mem_addr_tmp;
					}
					#else
					mem_addr_ch1 = mem_addr_tmp;
					#endif
				}
			}else{
				if(in->h265_ch0) {
					mem_addr_ch0 = mem_addr_tmp; // E_SHDR_OUT_RBK0_H265 will get CH0 address from MOVIE memory map
				} else {
					mem_addr_ch1 = mem_addr_tmp; // For encoder YUV in case, bandwidth issue?
				}
			}
		}
	}

	// Make Ref Info.
	if(g_rbk2layout_tbl[E_SHDR_OUT_RBK0_H265] != E_SHDR_OUT_NONE)
		rbk = E_SHDR_OUT_RBK0_H265;
	else
		rbk = E_SHDR_OUT_RBK4_YUV_0;
	g_shdr_ref_rbk = rbk;
	
	if(g_rbk2layout_tbl[E_SHDR_OUT_RBK0_H265] != E_SHDR_OUT_NONE) {
		// Make COMPIP Info.
		g_i2s_mng.cmptbl_out = g_i2s_mng.shdr_out[rbk];
		g_i2s_mng.cmptbl_out.yuv_video = E_SHARE_YUV_VIDEO_NONE;
		
		// COMPIP memory comes from MOVIE memory map CH1 directly. but assign memory for view to reduce bandwidth.
#if 1
		if((g_i2s_in.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_NO_ME) && (g_i2s_in.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE) && g_i2s_in.h265_compress ){
			if(in->h265_map_adr == TRUE){
				h265_map_addr = Sdram_Map_Movie_Record_Get_H265_Temp_Addr_CMP(in->h265_map_ch);
				h265_map_size = Sdram_Map_Movie_Record_Get_H265_Temp_Size_CMP(in->h265_map_ch);
					
				// Calc Memory
				for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
					g_i2s_mng.shdr_cmp_mem_addr_y[j] = h265_map_addr;
					g_i2s_mng.shdr_cmp_mem_addr_c[j] = g_i2s_mng.shdr_cmp_mem_addr_y[j] + h265_map_size * 2 / 3;
					h265_map_addr = g_i2s_mng.shdr_cmp_mem_addr_y[j] + h265_map_size;
				}
			}else{
				// Calc Memory
				for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
					mem_addr_ch1 = M_IPU_MAKE_ALIGNMENT(mem_addr_ch1, D_IPU_ALIGN_VIDEO_ADDR);
					g_i2s_mng.shdr_cmp_mem_addr_y[j] = mem_addr_ch1;
					g_i2s_mng.shdr_cmp_mem_addr_c[j] = g_i2s_mng.shdr_cmp_mem_addr_y[j] + D_IPU_CMP_TABLE_SIZE;
					mem_addr_ch1 = g_i2s_mng.shdr_cmp_mem_addr_c[j] + D_IPU_CMP_TABLE_SIZE;
				}
			}
		}
#endif
	}

	if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR)
	{
		#ifdef CO_MEMORY_1GB
		UINT32 mem_addr_temp = mem_addr_ch1;
		#else
		UINT32 mem_addr_temp = mem_addr_ch0;
		#endif
		// Make vHDR Out Info.
		g_i2s_mng.vhdr_on = TRUE;
		g_i2s_mng.vhdr_out.bit_depth = 16;
		g_i2s_mng.vhdr_out.rgb_h_size = g_i2s_in.r2y_out_h_size;
		g_i2s_mng.vhdr_out.rgb_v_size = g_i2s_in.r2y_out_v_size;
		g_i2s_mng.vhdr_out.rgb_h_byte = g_i2s_in.r2y_out_h_size * 2;
		for( j = 0; j < D_VHDR_MEM_BANK_YUV; j ++ ){
			mem_addr_temp = M_IPU_MAKE_ALIGNMENT(mem_addr_temp, D_IPU_ALIGN_MEM_SIZE);
			g_i2s_mng.vhdr_mem_addr_r[j] = mem_addr_temp;
			g_i2s_mng.vhdr_mem_addr_g[j] = g_i2s_mng.vhdr_mem_addr_r[j] + g_i2s_mng.vhdr_out.rgb_h_byte * g_i2s_mng.vhdr_out.rgb_v_size;
			g_i2s_mng.vhdr_mem_addr_b[j] = g_i2s_mng.vhdr_mem_addr_g[j] + g_i2s_mng.vhdr_out.rgb_h_byte * g_i2s_mng.vhdr_out.rgb_v_size;
			mem_addr_temp = g_i2s_mng.vhdr_mem_addr_b[j] + g_i2s_mng.vhdr_out.rgb_h_byte * g_i2s_mng.vhdr_out.rgb_v_size;
		}
		#ifdef CO_MEMORY_1GB
		mem_addr_ch1 = mem_addr_temp;
		#else
		mem_addr_ch0 = mem_addr_temp;
		#endif
		#ifdef CO_MEMORY_1GB
		g_i2s_mng.vhdr_heap_addr = mem_addr_ch0;
		// ??? Consider the margin for HDRPRM lib ???
		g_i2s_mng.vhdr_heap_size = 0x7000 + (g_i2s_in.r2y_out_h_size * g_i2s_in.r2y_out_v_size * 4); // min size for heap.
		mem_addr_ch0 = mem_addr_ch0 + g_i2s_mng.vhdr_heap_size;
		#else
		g_i2s_mng.vhdr_heap_addr = mem_addr_ch1;
		// ??? Consider the margin for HDRPRM lib ???
		g_i2s_mng.vhdr_heap_size = 1024 * 28 + M_IPU_MAKE_ALIGNMENT(g_i2s_in.r2y_out_h_size, 1024) * (M_IPU_MAKE_ALIGNMENT(g_i2s_in.r2y_out_v_size, 1024) + 200) * 4;
		mem_addr_ch1 = mem_addr_ch1 + g_i2s_mng.vhdr_heap_size;
		#endif
	}
	
	if((g_i2s_in.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_ME) && (g_i2s_in.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE))
	{
		// SGDE
		// Calc Size
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_video = E_SHARE_YUV_VIDEO_NONE;
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size = g_i2s_mng.shdr_out[rbk].yuv_h_size + D_IPU_3DNR_MARGIN; // with margin
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[rbk].yuv_v_size, 16);
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size = g_i2s_mng.shdr_out[rbk].yuv_v_size + D_IPU_3DNR_MARGIN; // with margin
		g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_byte = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size;
		g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_video = E_SHARE_YUV_VIDEO_NONE;
		g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size / 4;
		g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size / 4;
		g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size, D_IPU_ALIGN_ME_G_WIDTH);

		g_i2s_mng.shdr_out[rbk].yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size - D_IPU_3DNR_MARGIN;
		g_i2s_mng.shdr_out[rbk].yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size - D_IPU_3DNR_MARGIN;
			
		// Make IIP In Info.
		g_i2s_mng.iip_out.yuv_format = E_SHARE_YUV_FORMAT_NONE;
		g_i2s_mng.iip_out.yuv_video = E_SHARE_YUV_VIDEO_264;
		g_i2s_mng.iip_out.yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size;
		g_i2s_mng.iip_out.yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size;
		g_i2s_mng.iip_out.yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.iip_out.yuv_h_size, D_IPU_ALIGN_ME_G_WIDTH);
		mem_addr_ch0 = M_IPU_MAKE_ALIGNMENT(mem_addr_ch0, D_IPU_ALIGN_ME_IMG_ADDR);
		for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
			g_i2s_mng.iip_mem_addr_y[j] = mem_addr_ch0;
			if((g_i2s_in.mem_width == 0) || (g_i2s_in.mem_height == 0)){
				mem_addr_ch0 = g_i2s_mng.iip_mem_addr_y[j] + g_i2s_mng.iip_out.yuv_h_byte * M_IPU_MAKE_ALIGNMENT(g_i2s_mng.iip_out.yuv_v_size, D_IPU_ALIGN_VIDEO_G_LINE);
			}
			else{
				mem_addr_ch0 = g_i2s_mng.iip_mem_addr_y[j] + (g_i2s_in.mem_width / 4) * (g_i2s_in.mem_height / 4);
			}
			mem_addr_ch0 = M_IPU_MAKE_ALIGNMENT(mem_addr_ch0, D_IPU_ALIGN_ME_IMG_ADDR);
		}
		for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
			g_i2s_mng.iip_mem_addr_c[j] = mem_addr_ch0;
			if((g_i2s_in.mem_width == 0) || (g_i2s_in.mem_height == 0)){
				mem_addr_ch0 = g_i2s_mng.iip_mem_addr_c[j] + g_i2s_mng.iip_out.yuv_h_byte * M_IPU_MAKE_ALIGNMENT(g_i2s_mng.iip_out.yuv_v_size, D_IPU_ALIGN_VIDEO_G_LINE);
			}
			else{
				mem_addr_ch0 = g_i2s_mng.iip_mem_addr_c[j] + (g_i2s_in.mem_width / 4) * (g_i2s_in.mem_height / 4);
			}
			mem_addr_ch0 = M_IPU_MAKE_ALIGNMENT(mem_addr_ch0, D_IPU_ALIGN_ME_IMG_ADDR);
		}
			
		// Make ME Out Info.
		g_i2s_mng.me_out.yuv_format = E_SHARE_YUV_FORMAT_422_SEMI;
		g_i2s_mng.me_out.yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size/4;
		g_i2s_mng.me_out.yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size/4;
		g_i2s_mng.me_out.yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.me_out.yuv_h_size/4*2, D_IPU_ALIGN_ME_G_WIDTH);
		mem_addr_ch1 = M_IPU_MAKE_ALIGNMENT(mem_addr_ch1, D_IPU_ALIGN_ME_VEC_ADDR);
		for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
			g_i2s_mng.me_mem_addr_y[j] = mem_addr_ch1;
			if((g_i2s_in.mem_width == 0) || (g_i2s_in.mem_height == 0)){
				mem_addr_ch1 = g_i2s_mng.me_mem_addr_y[j] + g_i2s_mng.me_out.yuv_h_byte * g_i2s_mng.me_out.yuv_v_size;
			}
			else{
				mem_addr_ch1 = g_i2s_mng.me_mem_addr_y[j] + (g_i2s_in.mem_width / 4) * (g_i2s_in.mem_height / 4);
			}
			mem_addr_ch1 = M_IPU_MAKE_ALIGNMENT(mem_addr_ch1, D_IPU_ALIGN_ME_VEC_ADDR);
		}
		for( j = 0; j < D_SHDR_MEM_BANK_YUV; j ++ ){
			g_i2s_mng.me_mem_addr_c[j] = mem_addr_ch1;
			if((g_i2s_in.mem_width == 0) || (g_i2s_in.mem_height == 0)){
				mem_addr_ch1 = g_i2s_mng.me_mem_addr_c[j] + g_i2s_mng.me_out.yuv_h_byte * g_i2s_mng.me_out.yuv_v_size;
			}
			else{
				mem_addr_ch1 = g_i2s_mng.me_mem_addr_c[j] + (g_i2s_in.mem_width / 4) * (g_i2s_in.mem_height / 4);
			}
			mem_addr_ch1 = M_IPU_MAKE_ALIGNMENT(mem_addr_ch1, D_IPU_ALIGN_ME_VEC_ADDR);
		}
			
		if(g_i2s_in.op_3dnr_mode == E_SHARE_OP_3DNR_MODE_FPT)
		{
			// GMD
			g_i2s_mng.gmd_heap_addr = mem_addr_ch0;
			mem_addr_ch0 = mem_addr_ch0 + D_IPU_GMD_HEAP_SIZE;
		}
	}
	
	// Print out information
	UPRINTF( "========== STM IPU2SHDR ==========\n");
	for( i = 0; i < E_SGDE_OUT_MAX; i ++ ){
		UPRINTF( "sgde out[%d]:\n",i);
		UPRINTF( "sgde_out => GW:%d H:%d V:%d Format:%d Video:%d\n"
			, g_i2s_mng.sgde_out[i].yuv_h_byte
			, g_i2s_mng.sgde_out[i].yuv_h_size
			, g_i2s_mng.sgde_out[i].yuv_v_size
			, g_i2s_mng.sgde_out[i].yuv_format
			, g_i2s_mng.sgde_out[i].yuv_video );
	}	
	
	UPRINTF( "GMD Heap = (0x%08lx)\n", g_i2s_mng.gmd_heap_addr);
	
	UPRINTF( "IIP Out:\n");
	UPRINTF( "iip_out => GW:%d H:%d V:%d Format:%d Video:%d\n"
		, g_i2s_mng.iip_out.yuv_h_byte
		, g_i2s_mng.iip_out.yuv_h_size
		, g_i2s_mng.iip_out.yuv_v_size
		, g_i2s_mng.iip_out.yuv_format
		, g_i2s_mng.iip_out.yuv_video );
	UPRINTF( "iip_out => Y[0]:0x%08lx Y[1]:0x%08lx Y[2]:0x%08lx\n"
		, g_i2s_mng.iip_mem_addr_y[0]
		, g_i2s_mng.iip_mem_addr_y[1]
		, g_i2s_mng.iip_mem_addr_y[2] );
	UPRINTF( "iip_out => C[0]:0x%08lx C[1]:0x%08lx C[2]:0x%08lx\n"
		, g_i2s_mng.iip_mem_addr_c[0]
		, g_i2s_mng.iip_mem_addr_c[1]
		, g_i2s_mng.iip_mem_addr_c[2] );
	
	UPRINTF( "ME Out:\n");
	UPRINTF( "me_out => GW:%d H:%d V:%d Format:%d\n"
		, g_i2s_mng.me_out.yuv_h_byte
		, g_i2s_mng.me_out.yuv_h_size
		, g_i2s_mng.me_out.yuv_v_size
		, g_i2s_mng.me_out.yuv_format );
	UPRINTF( "me_in => Y[0]:0x%08lx Y[1]:0x%08lx Y[2]:0x%08lx\n"
		, g_i2s_mng.me_mem_addr_y[0]
		, g_i2s_mng.me_mem_addr_y[1]
		, g_i2s_mng.me_mem_addr_y[2] );
	UPRINTF( "me_in => C[0]:0x%08lx C[1]:0x%08lx C[2]:0x%08lx\n"
		, g_i2s_mng.me_mem_addr_c[0]
		, g_i2s_mng.me_mem_addr_c[1]
		, g_i2s_mng.me_mem_addr_c[2] );
	
	for( rbk = 0; rbk < E_SHDR_OUT_MAX; rbk ++ ){
		if(rbk == g_shdr_ref_rbk)
			UPRINTF( "RBK[%d]:(0x%02x) (Ref)\n",rbk, g_rbk2layout_tbl[rbk]);
		else
			UPRINTF( "RBK[%d]:(0x%02x)\n",rbk, g_rbk2layout_tbl[rbk]);
		UPRINTF( "shdr_out => GW:%d H:%d V:%d Format:%d Video:%d\n"
			, g_i2s_mng.shdr_out[rbk].yuv_h_byte
			, g_i2s_mng.shdr_out[rbk].yuv_h_size
			, g_i2s_mng.shdr_out[rbk].yuv_v_size
			, g_i2s_mng.shdr_out[rbk].yuv_format
			, g_i2s_mng.shdr_out[rbk].yuv_video );
		UPRINTF( "shdr_size => Y:0x%08x C:0x%08x\n"
			, mem_size_y[rbk]
			, mem_size_c[rbk] );
		UPRINTF( "shdr_addr => Y[0]:0x%08lx Y[1]:0x%08lx Y[2]:0x%08lx\n"
			, g_i2s_mng.shdr_mem_addr_y[rbk][0]
			, g_i2s_mng.shdr_mem_addr_y[rbk][1]
			, g_i2s_mng.shdr_mem_addr_y[rbk][2] );
		UPRINTF( "shdr_addr => C[0]:0x%08lx C[1]:0x%08lx C[2]:0x%08lx\n"
			, g_i2s_mng.shdr_mem_addr_c[rbk][0]
			, g_i2s_mng.shdr_mem_addr_c[rbk][1]
			, g_i2s_mng.shdr_mem_addr_c[rbk][2] );
	}
	
	UPRINTF( "CMPIP:\n");
	UPRINTF( "cmptbl_out => GW:%d H:%d V:%d Format:%d Video:%d\n"
		, g_i2s_mng.cmptbl_out.yuv_h_byte
		, g_i2s_mng.cmptbl_out.yuv_h_size
		, g_i2s_mng.cmptbl_out.yuv_v_size
		, g_i2s_mng.cmptbl_out.yuv_format
		, g_i2s_mng.cmptbl_out.yuv_video );
	UPRINTF( "cmptbl_out => Y[0]:0x%08lx Y[1]:0x%08lx Y[2]:0x%08lx\n"
		, g_i2s_mng.shdr_cmp_mem_addr_y[0]
		, g_i2s_mng.shdr_cmp_mem_addr_y[1]
		, g_i2s_mng.shdr_cmp_mem_addr_y[2] );
	UPRINTF( "cmptbl_out => C[0]:0x%08lx C[1]:0x%08lx C[2]:0x%08lx\n"
		, g_i2s_mng.shdr_cmp_mem_addr_c[0]
		, g_i2s_mng.shdr_cmp_mem_addr_c[1]
		, g_i2s_mng.shdr_cmp_mem_addr_c[2] );
	
	UPRINTF( "vHDR Out:\n");
	UPRINTF( "heap_addr = (0x%08lx), heap_size = (0x%08lx)\n", g_i2s_mng.vhdr_heap_addr, g_i2s_mng.vhdr_heap_size);
	UPRINTF( "vhdr_out => GW:%d H:%d V:%d\n"
		, g_i2s_mng.vhdr_out.rgb_h_byte
		, g_i2s_mng.vhdr_out.rgb_h_size
		, g_i2s_mng.vhdr_out.rgb_v_size );
	#ifdef CO_MEMORY_1GB
	UPRINTF( "vhdr_out => R[0]:0x%08lx R[1]:0x%08lx\n"
		, g_i2s_mng.vhdr_mem_addr_r[0]
		, g_i2s_mng.vhdr_mem_addr_r[1] );
	UPRINTF( "vhdr_out => G[0]:0x%08lx G[1]:0x%08lx\n"
		, g_i2s_mng.vhdr_mem_addr_g[0]
		, g_i2s_mng.vhdr_mem_addr_g[1] );
	UPRINTF( "vhdr_out => B[0]:0x%08lx B[1]:0x%08lx\n"
		, g_i2s_mng.vhdr_mem_addr_b[0]
		, g_i2s_mng.vhdr_mem_addr_b[1] );
	UPRINTF( "==================================\n");
	#else
	UPRINTF( "vhdr_out => R[0]:0x%08lx R[1]:0x%08lx R[2]:0x%08lx\n"
		, g_i2s_mng.vhdr_mem_addr_r[0]
		, g_i2s_mng.vhdr_mem_addr_r[1]
		, g_i2s_mng.vhdr_mem_addr_r[2] );
	UPRINTF( "vhdr_out => G[0]:0x%08lx G[1]:0x%08lx G[2]:0x%08lx\n"
		, g_i2s_mng.vhdr_mem_addr_g[0]
		, g_i2s_mng.vhdr_mem_addr_g[1]
		, g_i2s_mng.vhdr_mem_addr_g[2] );
	UPRINTF( "vhdr_out => B[0]:0x%08lx B[1]:0x%08lx B[2]:0x%08lx\n"
		, g_i2s_mng.vhdr_mem_addr_b[0]
		, g_i2s_mng.vhdr_mem_addr_b[1]
		, g_i2s_mng.vhdr_mem_addr_b[2] );
	UPRINTF( "==================================\n");
	#endif
	// set out
	out->available_memory_addr_ch0 = mem_addr_ch0;
	out->available_memory_addr_ch1 = mem_addr_ch1;
	
	// ------------------------------
	// Bandwidth
	// ------------------------------
	yuv0_ch = Get_Share_SDRAM_CH(g_i2s_mng.shdr_mem_addr_y[ E_SHDR_OUT_RBK4_YUV_0 ][0]);;
	yuv1_ch = Get_Share_SDRAM_CH(g_i2s_mng.shdr_mem_addr_y[ E_SHDR_OUT_RBK5_YUV_1 ][0]);;
	out->bw_byte[ yuv0_ch ][ E_BW_ITEM_SHDR_W_RBK4 ] = ( mem_size_y[E_SHDR_OUT_RBK4_YUV_0] + mem_size_c[E_SHDR_OUT_RBK4_YUV_0] );
	out->bw_byte[ yuv1_ch ][ E_BW_ITEM_SHDR_W_RBK5 ] = ( mem_size_y[E_SHDR_OUT_RBK5_YUV_1] + mem_size_c[E_SHDR_OUT_RBK5_YUV_1] );
	out->bw_yuv_ch[ E_SHARE_YUV_OUT_H265 ]   = in->h265_ch0 ? E_SDRAM_CH0 : E_SDRAM_CH1;
	out->bw_yuv_ch[ E_SHARE_YUV_OUT_H264_0 ] = E_SDRAM_CH1;
	out->bw_yuv_ch[ E_SHARE_YUV_OUT_H264_1 ] = E_SDRAM_CH1;
	out->bw_yuv_ch[ E_SHARE_YUV_OUT_USER_0 ] = yuv0_ch;
	out->bw_yuv_ch[ E_SHARE_YUV_OUT_USER_1 ] = yuv1_ch;
	
#ifdef D_IPU_3DNR_PROCESS_BY_TASK
	gFrameSkip_No_ShdrTsk = 0;
	g3dnrSkip_No_ShdrTsk = 0;

	OS_User_Clr_Flg( FID_IPU_3DNR, 0 );
	OS_User_Clr_Flg( FID_IPU_HDR, 0 );

	OS_User_Sta_Tsk(TID_IPU_3DNR_GMD, 0);
	OS_User_Sta_Tsk(TID_IPU_3DNR_ME, 0);
	OS_User_Sta_Tsk(TID_IPU_3DNR_SHDR, 0);

	OS_User_Sta_Tsk(TID_IPU_HDR_VHDR, 0);
	if (g_i2s_in.auto_framing_pip_mode) {
		OS_User_Clr_Flg( FID_IIP_SCALE, 0 );
		OS_User_Sta_Tsk(TID_IIP_SCALE, 0);
	}
#endif
	
	
	return D_STM_OK;
}


INT32 STM_IPU2SHDR_Start( void)
{
	
	switch(g_i2s_in.op_ipu_mode)
	{
		case E_SHARE_OP_IPU_MODE_NORMAL:
		case E_SHARE_OP_IPU_MODE_DOL_HDR:
			switch(g_i2s_in.op_3dnr_mode)
			{
				case E_SHARE_OP_3DNR_MODE_OFF:
				case E_SHARE_OP_3DNR_MODE_NO_ME:
				break;
				case E_SHARE_OP_3DNR_MODE_ME:
					Init_ME_Proc();
				break;
				case E_SHARE_OP_3DNR_MODE_FPT:
					Init_GMD_Proc(g_i2s_mng.gmd_heap_addr, (const T_SHDR_MNG_OUT*)&g_i2s_mng.sgde_out[E_SGDE_OUT_1], (const T_SHDR_MNG_OUT*)&g_i2s_mng.sgde_out[E_SGDE_OUT_0]);
					Init_ME_Proc();
				break;
				case E_SHARE_OP_3DNR_MODE_GYRO:
					Init_ME_Proc();
				break;
				case E_SHARE_OP_3DNR_MODE_PREHRE:
					Init_ME_Proc();
				break;
				default:
					return D_STM_OK;
				break;
			}
			if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL)
			{
				Init_SHDR_Proc();
			}
			else if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR)
			{
				Init_VHDR_Proc(g_i2s_mng.vhdr_heap_addr, g_i2s_mng.vhdr_heap_size, g_i2s_in.r2y_out_h_size, g_i2s_in.r2y_out_v_size);
			}
		break;
		case E_SHARE_OP_IPU_MODE_DUAL_STITCH:
		case E_SHARE_OP_IPU_MODE_DUAL_QR:
		case E_SHARE_OP_IPU_MODE_ROI_ZOOM:
		case E_SHARE_OP_IPU_MODE_PIPE_ONLY:
		case E_SHARE_OP_IPU_MODE_SHDR_ONLY:
			Init_SHDR_Proc();
		break;
		case E_SHARE_OP_IPU_MODE_IPU_ONLY:
		break;
		default:
			return D_STM_OK;
		break;
	}
	
	return D_STM_OK;
}


INT32 STM_IPU2SHDR_Event( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	INT32 i;
#ifdef D_IPU_3DNR_PROCESS_BY_TASK
	OS_USER_FLGPTN	flg;
#endif

	if(( g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_PIPE_ONLY ))
	{
		switch( event )
		{
			case E_SYSTEM_EVENT_PIPE_YUV:
				Get_Frame_Share_PIPE_YUV(&g_share_pipe_yuv, frame_no);

				for( i = 0; i < E_SHARE_SENSOR_ID_MAX; i ++ ){
					g_share_shdr_yuv.sensor[i].yuv[E_SHARE_YUV_OUT_USER_0] = g_share_pipe_yuv.sensor[i].yuv;
				}
				g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_USER_0] = g_share_pipe_yuv.layout;
				Set_Frame_Share_SHDR_YUV(&g_share_shdr_yuv, frame_no);
				Snd_Event_to_System( E_SYSTEM_EVENT_SHDR_YUV, frame_no );
			break;
			default:
			break;
		}
		return D_STM_BUSY;
	}
	else if( g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY )
	{
		switch( event )
		{
			case E_SYSTEM_EVENT_SGDE_YUV:
				Get_Frame_Share_SGDE_YUV(&g_share_sgde_yuv, frame_no);
				g_share_shdr_yuv = g_share_sgde_yuv;
				Set_Frame_Share_SHDR_YUV(&g_share_shdr_yuv, frame_no);
				Snd_Event_to_System( E_SYSTEM_EVENT_SHDR_YUV, frame_no );
			break;
			default:
			break;
		}
		return D_STM_BUSY;
	}
	else if((g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_STITCH) || (g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_QR))
	{
		switch( event )
		{
			case E_SYSTEM_EVENT_SGDE_YUV:
				// handle by stm_stitch
				return D_STM_BUSY;
			break;
			default:
			break;
		}
	}
	
	switch( event )
	{
		case E_SYSTEM_EVENT_PIPE_DOL_YUV:
		
			Get_Frame_Share_PIPE_DOL_YUV(&g_share_pipe_dol_yuv, frame_no);
			Get_Frame_Share_R2Y_DOL_Hist(&g_share_r2y_dol_hist, frame_no);
		
			if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR)
			{
				// Update vHDR In information - short exposure
				g_i2s_mng.vhdr_in[0].mem_addr_y = g_share_pipe_dol_yuv.short_yuv.addr_y;
				g_i2s_mng.vhdr_in[0].mem_addr_c = g_share_pipe_dol_yuv.short_yuv.addr_c;
				g_i2s_mng.vhdr_in[0].yuv_h_byte = g_share_pipe_dol_yuv.short_yuv.h_byte;
				g_i2s_mng.vhdr_in[0].yuv_h_size = g_share_pipe_dol_yuv.short_yuv.h_size;
				g_i2s_mng.vhdr_in[0].yuv_v_size = g_share_pipe_dol_yuv.short_yuv.v_size;
				g_i2s_mng.vhdr_in[0].yuv_format = g_share_pipe_dol_yuv.short_yuv.format;

				// Update vHDR In information - long exposure
				g_i2s_mng.vhdr_in[1].mem_addr_y = g_share_pipe_dol_yuv.long_yuv.addr_y;
				g_i2s_mng.vhdr_in[1].mem_addr_c = g_share_pipe_dol_yuv.long_yuv.addr_c;
				g_i2s_mng.vhdr_in[1].yuv_h_byte = g_share_pipe_dol_yuv.long_yuv.h_byte;
				g_i2s_mng.vhdr_in[1].yuv_h_size = g_share_pipe_dol_yuv.long_yuv.h_size;
				g_i2s_mng.vhdr_in[1].yuv_v_size = g_share_pipe_dol_yuv.long_yuv.v_size;
				g_i2s_mng.vhdr_in[1].yuv_format = g_share_pipe_dol_yuv.long_yuv.format;

				// Update vHDR Out address
				BANK_SET_AND_INC_NOLOCK(g_i2s_mng.vhdr_mem_bank_r, D_VHDR_MEM_BANK_YUV);
				BANK_SET_AND_INC_NOLOCK(g_i2s_mng.vhdr_mem_bank_g, D_VHDR_MEM_BANK_YUV);
				BANK_SET_AND_INC_NOLOCK(g_i2s_mng.vhdr_mem_bank_b, D_VHDR_MEM_BANK_YUV);
				g_i2s_mng.vhdr_out.mem_addr_r = g_i2s_mng.vhdr_mem_addr_r[ g_i2s_mng.vhdr_mem_bank_r[E_MEM_BANK_SET] ];
				g_i2s_mng.vhdr_out.mem_addr_g = g_i2s_mng.vhdr_mem_addr_g[ g_i2s_mng.vhdr_mem_bank_g[E_MEM_BANK_SET] ];
				g_i2s_mng.vhdr_out.mem_addr_b = g_i2s_mng.vhdr_mem_addr_b[ g_i2s_mng.vhdr_mem_bank_b[E_MEM_BANK_SET] ];
				
				// Update share memory
				g_share_dol_combine.dol_rgb.mem_addr_r = g_i2s_mng.vhdr_out.mem_addr_r;
				g_share_dol_combine.dol_rgb.mem_addr_g = g_i2s_mng.vhdr_out.mem_addr_g;
				g_share_dol_combine.dol_rgb.mem_addr_b = g_i2s_mng.vhdr_out.mem_addr_b;
				g_share_dol_combine.dol_rgb.rgb_h_byte = g_i2s_mng.vhdr_out.rgb_h_byte;
				g_share_dol_combine.dol_rgb.rgb_h_size = g_i2s_mng.vhdr_out.rgb_h_size;
				g_share_dol_combine.dol_rgb.rgb_v_size = g_i2s_mng.vhdr_out.rgb_v_size;
				g_share_dol_combine.dol_rgb.bit_depth = g_i2s_mng.vhdr_out.bit_depth;
				Set_Frame_Share_Dol_Combine(&g_share_dol_combine, frame_no);

				#ifdef D_IPU_3DNR_PROCESS_BY_TASK
					gFrame_No_VhdrTsk = frame_no;
					OS_User_Set_Flg( FID_IPU_HDR, FLG_IPU_HDR_VHDR_START );
				#else
					Start_VHDR_Proc(&g_i2s_mng, &g_share_r2y_dol_hist, frame_no);
				#endif
			}
		break;
		case E_SYSTEM_EVENT_SGDE_YUV:
		case E_SYSTEM_EVENT_STITCH_YUV:
			
			if(event == E_SYSTEM_EVENT_SGDE_YUV)
			{
				Get_Frame_Share_SGDE_YUV(&g_share_sgde_yuv, frame_no);
			}
			else if(event == E_SYSTEM_EVENT_STITCH_YUV)
			{
				Get_Frame_Share_Stitch_YUV(&g_share_sgde_yuv, frame_no);
			}
			Get_Frame_Share_R2Y_Hist(&g_share_r2y_hist, frame_no);
			
			// Update share memory
			//g_share_shdr_yuv = g_share_sgde_yuv;
			for( i = 0; i < E_SHARE_SENSOR_ID_MAX; i ++ ){
				g_share_shdr_yuv.sensor[i].yuv[E_SHARE_YUV_OUT_USER_0] = g_share_sgde_yuv.sensor[i].yuv[E_SHARE_YUV_OUT_USER_0];
			}
			
			if((g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_ROI_ZOOM) || 
				(g_i2s_in.op_3dnr_mode == E_SHARE_OP_3DNR_MODE_OFF))
			{
				// Update shdr information
				g_i2s_mng.shdr_in.mem_addr_y = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
				g_i2s_mng.shdr_in.mem_addr_c = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
				g_i2s_mng.shdr_in.yuv_h_byte = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
				g_i2s_mng.shdr_in.yuv_h_size = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size;
				g_i2s_mng.shdr_in.yuv_v_size = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size;
				g_i2s_mng.shdr_in.yuv_format = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].format;
			}
			else
			{
				// Update sgde information
				g_i2s_mng.sgde_out[E_SGDE_OUT_0].mem_addr_y = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
				g_i2s_mng.sgde_out[E_SGDE_OUT_0].mem_addr_c = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
				g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_byte = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
				g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size;
				g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size;
				g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_format = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].format;
				
				g_i2s_mng.sgde_out[E_SGDE_OUT_1].mem_addr_y = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
				g_i2s_mng.sgde_out[E_SGDE_OUT_1].mem_addr_c = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
				g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_byte = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
				g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size;
				g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size;
				g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_format = g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].format;

				// Update YUV memory of SGDE output
				for( i = 0; i < E_SGDE_OUT_MAX; i ++ ){
					g_i2s_mng.sgde_mem_addr_y[i][ g_i2s_mng.sgde_mem_bank_y[i] ] = g_i2s_mng.sgde_out[i].mem_addr_y;
					g_i2s_mng.sgde_mem_addr_c[i][ g_i2s_mng.sgde_mem_bank_c[i] ] = g_i2s_mng.sgde_out[i].mem_addr_c;
					g_i2s_mng.sgde_mem_bank_y[i] = ( g_i2s_mng.sgde_mem_bank_y[i] + 1) % D_SGDE_MEM_BANK_YUV;
					g_i2s_mng.sgde_mem_bank_c[i] = ( g_i2s_mng.sgde_mem_bank_c[i] + 1) % D_SGDE_MEM_BANK_YUV;
				}
				
				g_i2s_mng.gmd_in[0].yuv_h_byte = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_byte;
				g_i2s_mng.gmd_in[0].yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size;
				g_i2s_mng.gmd_in[0].yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size;
				g_i2s_mng.gmd_in[0].yuv_format = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_format;
				g_i2s_mng.gmd_in[0].yuv_video = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_video;
				
				g_i2s_mng.gmd_in[1].yuv_h_byte = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_byte;
				g_i2s_mng.gmd_in[1].yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size;
				g_i2s_mng.gmd_in[1].yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size;
				g_i2s_mng.gmd_in[1].yuv_format = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_format;
				g_i2s_mng.gmd_in[1].yuv_video = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_video;

				g_i2s_mng.iip_in.yuv_h_byte = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_byte;
				g_i2s_mng.iip_in.yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_h_size;
				g_i2s_mng.iip_in.yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_v_size;
				g_i2s_mng.iip_in.yuv_format = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_format;
				g_i2s_mng.iip_in.yuv_video = g_i2s_mng.sgde_out[E_SGDE_OUT_1].yuv_video;

				g_i2s_mng.shdr_in.yuv_h_byte = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_byte;
				g_i2s_mng.shdr_in.yuv_h_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_h_size;
				g_i2s_mng.shdr_in.yuv_v_size = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_v_size;
				g_i2s_mng.shdr_in.yuv_format = g_i2s_mng.sgde_out[E_SGDE_OUT_0].yuv_format;
			}
			
			IPU_3DNR_IQ_SetParam();

			g_i2s_mng.me_in.yuv_h_byte = g_i2s_mng.me_out.yuv_h_byte;
			g_i2s_mng.me_in.yuv_h_size = g_i2s_mng.me_out.yuv_h_size;
			g_i2s_mng.me_in.yuv_v_size = g_i2s_mng.me_out.yuv_v_size;
			g_i2s_mng.me_in.yuv_format = g_i2s_mng.me_out.yuv_format;

			// Start SHDR
			#ifdef D_IPU_3DNR_PROCESS_BY_TASK
				if((g_i2s_in.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_ME) && (g_i2s_in.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE))
				{
					#ifdef D_3DNR_TSK_BY_SEQUENTIAL
					gFrame_No_GmdTsk = frame_no;
					flg = FLG_IPU_3DNR_GMD_START;
					#else
					flg = FLG_IPU_3DNR_GMD_START | FLG_IPU_3DNR_ME_START | FLG_IPU_3DNR_SHDR_START;
					#endif
				}
				else
				{
					gFrame_No_ShdrTsk = frame_no;
					flg = FLG_IPU_3DNR_SHDR_START;
				}
				OS_User_Set_Flg( FID_IPU_3DNR, flg );
			#else
				Start_SHDR_Proc(&g_i2s_mng, g_i2s_in.op_3dnr_mode, g_i2s_in.h265_compress, frame_no, g_i2s_in.cbcr_global_x2);
			#endif
		break;
		case E_SYSTEM_EVENT_VIEW_RECONFIG:
			#ifdef D_IPU_3DNR_PROCESS_BY_TASK
				gFrameSkip_No_ShdrTsk = 1;
				g3dnrSkip_No_ShdrTsk = 5;
				gFrame_No_ShdrTsk = frame_no;
				OS_User_Set_Flg( FID_IPU_3DNR, FLG_IPU_3DNR_SHDR_RECONFIG );
			#else
				shdr_reconfig(frame_no);
			#endif
		break;
		default:
		break;
	}
	return D_STM_BUSY;
}


INT32 STM_IPU2SHDR_Stop( void)
{
	
#ifdef D_IPU_3DNR_PROCESS_BY_TASK
	T_OS_USER_RTSK pk_rtsk;

	OS_User_Set_Flg( FID_IPU_3DNR, FLG_IPU_3DNR_STOP );
	OS_User_Set_Flg( FID_IPU_HDR, FLG_IPU_HDR_STOP );

	OS_User_Ref_Tsk( TID_IPU_3DNR_GMD, &pk_rtsk );
	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
		OS_User_Dly_Tsk( 1 );
		OS_User_Ref_Tsk( TID_IPU_3DNR_GMD, &pk_rtsk );
	}

	OS_User_Ref_Tsk( TID_IPU_3DNR_ME, &pk_rtsk );
	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
		OS_User_Dly_Tsk( 1 );
		OS_User_Ref_Tsk( TID_IPU_3DNR_ME, &pk_rtsk );
	}

	OS_User_Ref_Tsk( TID_IPU_3DNR_SHDR, &pk_rtsk );
	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
		OS_User_Dly_Tsk( 1 );
		OS_User_Ref_Tsk( TID_IPU_3DNR_SHDR, &pk_rtsk );
	}

	OS_User_Ref_Tsk( TID_IPU_HDR_VHDR, &pk_rtsk );
	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
		OS_User_Dly_Tsk( 1 );
		OS_User_Ref_Tsk( TID_IPU_HDR_VHDR, &pk_rtsk );
	}
	
	if (g_i2s_in.auto_framing_pip_mode) {
		OS_User_Set_Flg( FID_IIP_SCALE, FLG_IIP_SCALE_STOP );
		
		OS_User_Ref_Tsk( TID_IIP_SCALE, &pk_rtsk );
		while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
			OS_User_Dly_Tsk( 1 );
			OS_User_Ref_Tsk( TID_IIP_SCALE, &pk_rtsk );
		}
	}
#endif
	
	switch(g_i2s_in.op_ipu_mode)
	{
		case E_SHARE_OP_IPU_MODE_NORMAL:
		case E_SHARE_OP_IPU_MODE_DOL_HDR:
			switch(g_i2s_in.op_3dnr_mode)
			{
				case E_SHARE_OP_3DNR_MODE_OFF:
				case E_SHARE_OP_3DNR_MODE_NO_ME:
				break;
				case E_SHARE_OP_3DNR_MODE_ME:
					Close_ME_Proc();
				break;
				case E_SHARE_OP_3DNR_MODE_FPT:
					Close_GMD_Proc();
					Close_ME_Proc();
				break;
				case E_SHARE_OP_3DNR_MODE_GYRO:
					Close_ME_Proc();
				break;
				case E_SHARE_OP_3DNR_MODE_PREHRE:
					Close_ME_Proc();
				break;
				default:
					return D_STM_OK;
				break;
			}
			
			if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL)
			{
				Close_SHDR_Proc();
			}
			else if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR)
			{
				Close_VHDR_Proc();
			}
		break;
		case E_SHARE_OP_IPU_MODE_DUAL_STITCH:
		case E_SHARE_OP_IPU_MODE_DUAL_QR:
		case E_SHARE_OP_IPU_MODE_ROI_ZOOM:
		case E_SHARE_OP_IPU_MODE_PIPE_ONLY:
		case E_SHARE_OP_IPU_MODE_SHDR_ONLY:
			Close_SHDR_Proc();
		break;
		case E_SHARE_OP_IPU_MODE_IPU_ONLY:
		break;
		default:
			return D_STM_OK;
		break;
	}
	
	return D_STM_IDLE;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

/*==============================================================================*/
/* IPU 3DNR Task Function														*/
/*==============================================================================*/
#ifdef D_IPU_3DNR_PROCESS_BY_TASK

VOID Tsk_IPU_3DNR_GMD_Main( INT32 stacd )
{
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;
	ULLONG			frame_no = 0;
	T_SHARE_YUV		share_sgde_yuv;
		
	while(1){
		ercd = OS_User_Wai_Flg( FID_IPU_3DNR, (FLG_IPU_3DNR_GMD_START | FLG_IPU_3DNR_STOP), D_OS_USER_TWF_ORW, &flgptn );
		if( ercd != D_OS_USER_E_OK ){
			UPRINTF("3DNR GMD Task Wai_Flg error %d\n", ercd);
		}
	
		if( flgptn & FLG_IPU_3DNR_STOP ){
			break;
		}
			
		if( flgptn & FLG_IPU_3DNR_GMD_START ){
			OS_User_Clr_Flg( FID_IPU_3DNR, ~FLG_IPU_3DNR_GMD_START );
			
			frame_no = gFrame_No_GmdTsk; // save local
			
			if((g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL) || (g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR))
			{
				if(g_i2s_in.op_3dnr_mode == E_SHARE_OP_3DNR_MODE_FPT)
				{
					Get_Frame_Share_SGDE_YUV( &share_sgde_yuv, frame_no );
					g_i2s_mng.gmd_in[0].mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
					g_i2s_mng.gmd_in[0].mem_addr_c = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
					g_i2s_mng.gmd_in[1].mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
					g_i2s_mng.gmd_in[1].mem_addr_c = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
					// for MPRINTF
					g_i2s_mng.sgde_out[E_SGDE_OUT_0].mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
					g_i2s_mng.sgde_out[E_SGDE_OUT_1].mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
					// in Start_GMD_Proc
					//    g_i2s_mng.gmd_ref[0-1] is set;
					//    Set_Frame_Share_Motion_Vector() is called
					Start_GMD_Proc(&g_i2s_mng, g_i2s_in.op_3dnr_mode, frame_no);
				}
			}
				
			#ifdef D_3DNR_TSK_BY_SEQUENTIAL
			gFrame_No_MeTsk = frame_no;
			OS_User_Set_Flg( FID_IPU_3DNR, FLG_IPU_3DNR_ME_START );
			#endif
		}
			
	}
		
	OS_User_Ext_Tsk();
}


VOID Tsk_IPU_3DNR_ME_Main( INT32 stacd )
{
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;
	ULLONG			frame_no = 0;
	
	while(1){
		ercd = OS_User_Wai_Flg( FID_IPU_3DNR, (FLG_IPU_3DNR_ME_START | FLG_IPU_3DNR_STOP), D_OS_USER_TWF_ORW, &flgptn );
		if( ercd != D_OS_USER_E_OK ){
			UPRINTF("3DNR ME Task Wai_Flg error %d\n", ercd);
		}

		if( flgptn & FLG_IPU_3DNR_STOP ){
			break;
		}
		
		if( flgptn & FLG_IPU_3DNR_ME_START ){
			OS_User_Clr_Flg( FID_IPU_3DNR, ~FLG_IPU_3DNR_ME_START );
			
			frame_no = gFrame_No_MeTsk; // save local
			
			if((g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL) || (g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR))
			{
				if((g_i2s_in.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_ME) && (g_i2s_in.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE))
				{
					// gnso
					{
						T_SHARE_YUV share_sgde_yuv;
						Get_Frame_Share_SGDE_YUV(&share_sgde_yuv, frame_no);
						
						g_i2s_mng.iip_in.mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
						g_i2s_mng.iip_in.mem_addr_c = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
						
						// Update IIP Out address
						BANK_SET_AND_INC_NOLOCK(g_i2s_mng.iip_mem_bank_y, D_SHDR_MEM_BANK_YUV);
						BANK_SET_AND_INC_NOLOCK(g_i2s_mng.iip_mem_bank_c, D_SHDR_MEM_BANK_YUV);
						g_i2s_mng.iip_out.mem_addr_y = g_i2s_mng.iip_mem_addr_y[ g_i2s_mng.iip_mem_bank_y[E_MEM_BANK_SET] ];
						g_i2s_mng.iip_out.mem_addr_c = g_i2s_mng.iip_mem_addr_c[ g_i2s_mng.iip_mem_bank_c[E_MEM_BANK_SET] ];
						
						// Update ME Out address of ME output
						BANK_SET_AND_INC_NOLOCK(g_i2s_mng.me_mem_bank_y, D_SHDR_MEM_BANK_YUV);
						BANK_SET_AND_INC_NOLOCK(g_i2s_mng.me_mem_bank_c, D_SHDR_MEM_BANK_YUV);
						g_i2s_mng.me_out.mem_addr_y = g_i2s_mng.me_mem_addr_y[ g_i2s_mng.me_mem_bank_y[E_MEM_BANK_SET] ];
						g_i2s_mng.me_out.mem_addr_c = g_i2s_mng.me_mem_addr_c[ g_i2s_mng.me_mem_bank_c[E_MEM_BANK_SET] ];
						
						// in Start_ME_Proc
						//    g_i2s_mng.ref_in[1] is set;
						//    Get_Frame_Share_Motion_Vector() is called
					}
					Start_ME_Proc(&g_i2s_mng, frame_no);
				}
			}
			
			#ifdef D_3DNR_TSK_BY_SEQUENTIAL
			gFrame_No_ShdrTsk = frame_no;
			OS_User_Set_Flg( FID_IPU_3DNR, FLG_IPU_3DNR_SHDR_START );
			#endif
		}
		
	}
	
	OS_User_Ext_Tsk();
}

VOID Tsk_IPU_3DNR_SHDR_Main( INT32 stacd )
{
	T_SHARE_SYSTEM_PARAM param;
	static UCHAR	frame_skip = 0;//D_3DNR_TSK_SKIP_FRAME;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;
	ULLONG			frame_no = 0;
	
	T_BF_ENCODER_GET_IMAGE_ADDR_PARAM	addr_param; // need to get addr from user
	T_SHARE_YUV							share_sgde_yuv;
	INT32	ret;
	INT32	i;
	
	while(1){
		ercd = OS_User_Wai_Flg( FID_IPU_3DNR, (FLG_IPU_3DNR_SHDR_START | FLG_IPU_3DNR_SHDR_RECONFIG | FLG_IPU_3DNR_STOP), D_OS_USER_TWF_ORW, &flgptn );
		if( ercd != D_OS_USER_E_OK ){
			UPRINTF("3DNR SHDR Task Wai_Flg error %d\n", ercd);
		}

		if( flgptn & FLG_IPU_3DNR_STOP ){
			//frame_skip = D_3DNR_TSK_SKIP_FRAME;
			break;
		}
		
		if( flgptn & FLG_IPU_3DNR_SHDR_START ){
			OS_User_Clr_Flg( FID_IPU_3DNR, ~FLG_IPU_3DNR_SHDR_START );
			
			frame_no = gFrame_No_ShdrTsk; // save local
			
			Get_Share_System_Param(&param);

			if (Cate_Get_Drop_Frame_Half() && (frame_no & 1)) frame_skip = 1;

			if (frame_skip == 0) {
				Get_Frame_Share_SGDE_YUV(&share_sgde_yuv, frame_no);

				
				g_i2s_mng.shdr_in.mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
				g_i2s_mng.shdr_in.mem_addr_c = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
				
				// use from Tsk_IPU_3DNR_ME_Main, need check critical
				g_i2s_mng.me_in.mem_addr_y = g_i2s_mng.me_out.mem_addr_y;
				g_i2s_mng.me_in.mem_addr_c = g_i2s_mng.me_out.mem_addr_c;
				
				// Update YUV address of SHDR output
				BANK_SET_AND_INC_NOLOCK(g_i2s_mng.shdr_cmp_mem_bank_y, D_SHDR_MEM_BANK_YUV);
				BANK_SET_AND_INC_NOLOCK(g_i2s_mng.shdr_cmp_mem_bank_c, D_SHDR_MEM_BANK_YUV);
				g_i2s_mng.cmptbl_out.mem_addr_y = g_i2s_mng.shdr_cmp_mem_addr_y[ g_i2s_mng.shdr_cmp_mem_bank_y[E_MEM_BANK_SET] ];
				g_i2s_mng.cmptbl_out.mem_addr_c = g_i2s_mng.shdr_cmp_mem_addr_c[ g_i2s_mng.shdr_cmp_mem_bank_c[E_MEM_BANK_SET] ];
			
				for( i = 0; i < E_SHDR_OUT_MAX; i ++ ){
					if(g_rbk2layout_tbl[i] != E_SHDR_OUT_NONE) {
						if(g_i2s_mng.shdr_out[i].yuv_h_byte != 0){
							BANK_SET_AND_INC_LOCK(g_i2s_mng.shdr_mem_bank_y[i], D_SHDR_MEM_BANK_YUV, g_i2s_mng.shdr_mem_addr_y[i], param.lock_ipu_address_y);
							BANK_SET_AND_INC_LOCK(g_i2s_mng.shdr_mem_bank_c[i], D_SHDR_MEM_BANK_YUV, g_i2s_mng.shdr_mem_addr_c[i], param.lock_ipu_address_c);
							g_i2s_mng.shdr_out[i].mem_addr_y = g_i2s_mng.shdr_mem_addr_y[i][ g_i2s_mng.shdr_mem_bank_y[i][E_MEM_BANK_SET] ];
							g_i2s_mng.shdr_out[i].mem_addr_c = g_i2s_mng.shdr_mem_addr_c[i][ g_i2s_mng.shdr_mem_bank_c[i][E_MEM_BANK_SET] ];
						}
						else{
							g_i2s_mng.shdr_out[i].mem_addr_y = 0;
							g_i2s_mng.shdr_out[i].mem_addr_c = 0;
						}
						
						if(g_i2s_mng.shdr_out[i].yuv_video != E_SHARE_YUV_VIDEO_NONE) {
							g_i2s_mng.shdr_out[i].yuv_h_byte = M_IPU_MAKE_ALIGNMENT(g_i2s_mng.shdr_out[i].yuv_h_size, D_IPU_ALIGN_VIDEO_G_WIDTH);
							// get movie address from user
							if(g_i2s_in.yuv_ask_user_for_video_addr[g_rbk2layout_tbl[i]] != NULL){
								ret = g_i2s_in.yuv_ask_user_for_video_addr[g_rbk2layout_tbl[i]]( &addr_param, g_rbk2layout_tbl[i], frame_no );
								if(( ret == OK ) && ( addr_param.dummy_flg == FALSE )){
									g_i2s_mng.shdr_out[i].mem_addr_y = addr_param.y_addr;
									g_i2s_mng.shdr_out[i].mem_addr_c = addr_param.c_addr;
									g_i2s_mng.shdr_out[i].yuv_h_byte = addr_param.g_width;

									if(i == E_SHDR_OUT_RBK0_H265) {
										g_i2s_mng.cmptbl_out.mem_addr_y = addr_param.y_cmp_addr;
										g_i2s_mng.cmptbl_out.mem_addr_c = addr_param.c_cmp_addr;
									}
								} else {
									if(i == E_SHDR_OUT_RBK0_H265) {
										// not use cmptbl for 3dnr
									//	g_i2s_mng.cmptbl_out.mem_addr_y = 0;
									//	g_i2s_mng.cmptbl_out.mem_addr_c = 0;
									} else {
										// not use 264_0 and 264_1
										g_i2s_mng.shdr_out[i].mem_addr_y = 0;
										g_i2s_mng.shdr_out[i].mem_addr_c = 0;
										g_i2s_mng.shdr_out[i].yuv_h_byte = 0;
									}
								}
							}
						}

						g_share_shdr_yuv.layout[g_rbk2layout_tbl[i]].addr_y = g_i2s_mng.shdr_out[i].mem_addr_y;
						g_share_shdr_yuv.layout[g_rbk2layout_tbl[i]].addr_c = g_i2s_mng.shdr_out[i].mem_addr_c;
						g_share_shdr_yuv.layout[g_rbk2layout_tbl[i]].h_size = g_i2s_mng.shdr_out[i].yuv_h_size;
						g_share_shdr_yuv.layout[g_rbk2layout_tbl[i]].v_size = g_i2s_mng.shdr_out[i].yuv_v_size;
						g_share_shdr_yuv.layout[g_rbk2layout_tbl[i]].format = g_i2s_mng.shdr_out[i].yuv_format;
						g_share_shdr_yuv.layout[g_rbk2layout_tbl[i]].video = g_i2s_mng.shdr_out[i].yuv_video;
						g_share_shdr_yuv.layout[g_rbk2layout_tbl[i]].h_byte = g_i2s_mng.shdr_out[i].yuv_h_byte;
					}
				}
				// printf("###zxl:share_sgde_yuv.is_cap_without_reset_image = %d\n", share_sgde_yuv.is_cap_without_reset_image);
				// Update share memory
				Set_Frame_Share_SHDR_YUV(&g_share_shdr_yuv, frame_no);

				// in Start_SHDR_Proc
				//    g_i2s_mng.ref_in[0], combtbl_in is set for 3dnr
				//    Get_Frame_Share_Motion_Vector() is called for 3dnr

				if( g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_SHDR_ONLY ) {
					if(g_i2s_mng.sen_stream.data == 0) {
						// No SEN/Stream mapping
						Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, g_i2s_in.h265_compress, frame_no, g_i2s_in.cbcr_global_x2);
					} else {
						// SEN/Stream mapping, like SEN0:H265_0, SEN1:H264_0
						T_SHDR_MNG_IN  bk_shdr_in			= {0};
						T_SHDR_MNG_OUT bk_shdr_out_264_0	= {0};
						T_SHDR_MNG_OUT bk_shdr_out_264_1	= {0};
						T_SHDR_MNG_OUT bk_shdr_out_265		= {0};
						T_SHDR_MNG_OUT bk_shdr_out_yuv_0	= {0};
						T_SHDR_MNG_OUT bk_shdr_out_yuv_1	= {0};
						UCHAR iSen		= 0xFF;
						UCHAR iSen_265  = 0xFF;
						UCHAR iSen_0	= 0xFF;
						UCHAR iSen_1	= 0xFF;
						ULONG max_hv_0	= 0;
						ULONG max_hv_1	= 0;

						// back up
						bk_shdr_in			= g_i2s_mng.shdr_in;
						bk_shdr_out_265		= g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265];
						bk_shdr_out_264_0	= g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0];
						bk_shdr_out_264_1	= g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1];
						bk_shdr_out_yuv_0	= g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0];
						bk_shdr_out_yuv_1	= g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1];

						// Search which SEN with H265_0 stream
						for(i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
							if( ((g_i2s_mng.sen_stream.data >> (i*8)) & 0x000000FF) == E_SHARE_STREAM_H265_0 ) {
								iSen_265 = i;
							} else if( ((g_i2s_mng.sen_stream.data >> (i*8)) & 0x000000FF) == E_SHARE_STREAM_H264_0 ) {
								iSen_0 = i;
							} else if( ((g_i2s_mng.sen_stream.data >> (i*8)) & 0x000000FF) == E_SHARE_STREAM_H264_1) {
								iSen_1 = i;
							}
						}

						if(iSen_265 != 0xFF) {
							if(share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].addr_y == 0 ||
							   share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].addr_c == 0 ||
							   share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].h_byte == 0 ||
							   share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].h_size == 0 ||
							   share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].v_size == 0) {
								UPRINTF_ERR(-1);
								UPRINTF("No YUV data for iSen_265\n");
							}
							// H265_0 is main image
							g_i2s_mng.shdr_in.mem_addr_y = share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].addr_y;
							g_i2s_mng.shdr_in.mem_addr_c = share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].addr_c;
							g_i2s_mng.shdr_in.yuv_h_byte = share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].h_byte;
							g_i2s_mng.shdr_in.yuv_h_size = share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].h_size;
							g_i2s_mng.shdr_in.yuv_v_size = share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].v_size;
							g_i2s_mng.shdr_in.yuv_format = share_sgde_yuv.sensor[iSen_265].yuv[E_SHARE_YUV_OUT_USER_0].format;
							// Disable H264 since video format cannot convert together with H265
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y = 0;
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y = 0;

							if( (g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].mem_addr_y != 0)  ||
								(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y != 0) ||
								(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y != 0)){
								// start for entire image without 264_0/264_1
								Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, g_i2s_in.h265_compress, frame_no, g_i2s_in.cbcr_global_x2);
							}

							if(iSen_0 != 0xFF && iSen_1 != 0xFF) {
								UPRINTF_ERR(-1);
								UPRINTF("User need to set SEN_STREAM_MAPPING correctly\n");
							} else if(iSen_0 == 0xFF && iSen_1 == 0xFF) {
								UPRINTF_ERR(-2);
								UPRINTF("User need to set SEN_STREAM_MAPPING correctly\n");
							} else {
								// only one of h264_0/1 will be used
								iSen = iSen_0 != 0xFF ? iSen_0 : iSen_1;
								if(share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].addr_y == 0 ||
								   share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].addr_c == 0 ||
								   share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].h_byte == 0 ||
								   share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].h_size == 0 ||
								   share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].v_size == 0) {
									UPRINTF_ERR(-1);
									UPRINTF("No YUV data for iSen\n");
								}
								g_i2s_mng.shdr_in.mem_addr_y = share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].addr_y;
								g_i2s_mng.shdr_in.mem_addr_c = share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].addr_c;
								g_i2s_mng.shdr_in.yuv_h_byte = share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].h_byte;
								g_i2s_mng.shdr_in.yuv_h_size = share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].h_size;
								g_i2s_mng.shdr_in.yuv_v_size = share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].v_size;
								g_i2s_mng.shdr_in.yuv_format = share_sgde_yuv.sensor[iSen].yuv[E_SHARE_YUV_OUT_USER_0].format;

								// backup h264_0/1 for mem_addr_y, due to previous one of them is set 0
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0 ] = bk_shdr_out_264_0;
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1 ] = bk_shdr_out_264_1;
								// Disable H265 since video format cannot convert together with H264
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].mem_addr_y = 0;
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y = 0;
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y = iSen_0 == 0xFF ? 0 : g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y;
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y = iSen_1 == 0xFF ? 0 : g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y;
								// make USER_1 display divided by 2 to smaller window
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size >>= 1;
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size >>= 1;
								if( (g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y != 0) ||
									(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y != 0) ||
									(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y != 0) ){
									Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, FALSE, frame_no, g_i2s_in.cbcr_global_x2); // no comp ( no h265 )
								}
							}
							
						} else if(iSen_0 != 0xFF && iSen_1 != 0xFF) {
							// only H264_0 and H264_1, need to find max resolution
							max_hv_0 = share_sgde_yuv.sensor[iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].h_size * share_sgde_yuv.sensor[iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].v_size;
							max_hv_1 = share_sgde_yuv.sensor[iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].h_size * share_sgde_yuv.sensor[iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].v_size;

							if(share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].addr_y == 0 ||
							   share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].addr_c == 0 ||
							   share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].h_byte == 0 ||
							   share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].h_size == 0 ||
							   share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].v_size == 0) {
								UPRINTF_ERR(-1);
								UPRINTF("No YUV data for [max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1]\n");
							}
							// 1st run
							g_i2s_mng.shdr_in.mem_addr_y = share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].addr_y;
							g_i2s_mng.shdr_in.mem_addr_c = share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].addr_c;
							g_i2s_mng.shdr_in.yuv_h_byte = share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].h_byte;
							g_i2s_mng.shdr_in.yuv_h_size = share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].h_size;
							g_i2s_mng.shdr_in.yuv_v_size = share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].v_size;
							g_i2s_mng.shdr_in.yuv_format = share_sgde_yuv.sensor[max_hv_0 >= max_hv_1 ? iSen_0 : iSen_1].yuv[E_SHARE_YUV_OUT_USER_0].format;

							// Disable H265 since video format cannot convert together with H264
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].mem_addr_y = 0;

							if(max_hv_0 >= max_hv_1) {
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y = 0;
							} else {
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y = 0;
							}
							if( (g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y != 0) ||
								(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y != 0) ||
								(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y != 0) ||
								(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y != 0)) {
								Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, FALSE, frame_no, g_i2s_in.cbcr_global_x2); // no comp ( no h265 )
							}
							// 2nd run
							if(share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].addr_y == 0 ||
							   share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].addr_c == 0 ||
							   share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].h_byte == 0 ||
							   share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].h_size == 0 ||
							   share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].v_size == 0) {
								UPRINTF_ERR(-1);
								UPRINTF("No YUV data for [max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0]\n");
							}
							g_i2s_mng.shdr_in.mem_addr_y = share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].addr_y;
							g_i2s_mng.shdr_in.mem_addr_c = share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].addr_c;
							g_i2s_mng.shdr_in.yuv_h_byte = share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].h_byte;
							g_i2s_mng.shdr_in.yuv_h_size = share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].h_size;
							g_i2s_mng.shdr_in.yuv_v_size = share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].v_size;
							g_i2s_mng.shdr_in.yuv_format = share_sgde_yuv.sensor[max_hv_1 <= max_hv_0 ? iSen_1 : iSen_0].yuv[E_SHARE_YUV_OUT_USER_0].format;
							// make USER_1 display divided by 2 to smaller window
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size >>= 1;
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size >>= 1;
							// backup h264_0/1 for mem_addr_y, due to previous one of them is set 0
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0 ] = bk_shdr_out_264_0;
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1 ] = bk_shdr_out_264_1;

							if(max_hv_1 <= max_hv_0) {
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y = 0;
							} else {
								g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y = 0;
							}
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y = 0;

							if( (g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y != 0) ||
								(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y != 0) ||
								(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y != 0) ||
								(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y != 0)) {
								Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, FALSE, frame_no, g_i2s_in.cbcr_global_x2); // no comp ( no h265 )
							}
						} else {
							UPRINTF_ERR(-3);
							UPRINTF("User need to set SEN_STREAM_MAPPING correctly\n");
						}

						g_i2s_mng.shdr_in = bk_shdr_in;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265 ] = bk_shdr_out_265;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0 ] = bk_shdr_out_264_0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1 ] = bk_shdr_out_264_1;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0] = bk_shdr_out_yuv_0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1] = bk_shdr_out_yuv_1;
					}
				} else if((g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_ROI_ZOOM) || ((g_i2s_in.roi_mode == 2)&&(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR))) {
					T_SHDR_MNG_IN  bk_shdr_in        = {0};
					T_SHDR_MNG_OUT bk_shdr_out_265   = {0};
					T_SHDR_MNG_OUT bk_shdr_out_264_0 = {0};
					T_SHDR_MNG_OUT bk_shdr_out_264_1 = {0};
					T_SHDR_MNG_OUT bk_shdr_out_yuv_0 = {0};
					T_SHDR_MNG_OUT bk_shdr_out_yuv_1 = {0};

					if (param.auto_framing_pip_mode) {
						T_SHDR_MNG_IN  scaleSrc = {0};
						T_SHDR_MNG_OUT scaleDst = {0};
						UINT32 pip_win_w, pip_win_h, pip_ofst_y, pip_ofst_c;
						UINT32 face_src_w, face_src_h, face_ofst_y, face_ofst_c;
						
						// back up
						bk_shdr_in        = g_i2s_mng.shdr_in;
						bk_shdr_out_265   = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265  ];
						bk_shdr_out_264_0 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0];
						bk_shdr_out_264_1 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1];
						bk_shdr_out_yuv_0 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0 ];
						bk_shdr_out_yuv_1 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1 ];
					
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265  ].mem_addr_y = 0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK1_ME	 ].mem_addr_y = 0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y = 0;
						
						/***************************************
	 					  1. make PIP background (ROI or FULL)  
						****************************************/
						// mode==2: PIP = FACE + ROI
						if (param.auto_framing_pip_mode==2) {
							g_i2s_mng.shdr_in.mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_y;
							g_i2s_mng.shdr_in.mem_addr_c = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_c;
							g_i2s_mng.shdr_in.yuv_h_byte = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_byte;
							g_i2s_mng.shdr_in.yuv_h_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_size;
							g_i2s_mng.shdr_in.yuv_v_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].v_size; 
						}
						// mode==1: PIP = ROI + FULL
						// mode==3: PIP = FACE + FULL
						else {
							;
						}

						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y = 0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0 ].mem_addr_y = 0;
						//g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1] 
						
						// paste SGDE_H264_1(ROI) or SGDE_USER_0(FULL) on SHDR_YUV_1
						Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, g_i2s_in.h265_compress, frame_no, g_i2s_in.cbcr_global_x2);
						
						/***************************************
	 					  2. make PIP foreground by iip         
						****************************************/
						// scale source = SGDE_USER_1 (1/4 ROI or 1/4 FULL)
						scaleSrc.mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
						scaleSrc.mem_addr_c = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
						scaleSrc.yuv_h_byte = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
						scaleSrc.yuv_h_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size;
						scaleSrc.yuv_v_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size;
						scaleSrc.yuv_format = share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_1].format;
						
						// scale destnation = SHDR_YUV_1
						scaleDst = bk_shdr_out_yuv_1;
							
						// mode==1: PIP = ROI + FULL
						if (param.auto_framing_pip_mode==1) {
							face_ofst_y = 0;
							face_ofst_c = 0;
							face_src_w  = scaleSrc.yuv_h_size;
							face_src_h  = scaleSrc.yuv_v_size;
						}
						// mode==2: PIP = FACE + ROI
						// mode==3: PIP = FACE + FULL
						else {
							get_AutoFraming_Face_Pos(scaleSrc, &face_src_w, &face_src_h, &face_ofst_y, &face_ofst_c);
						}

						get_AutoFraming_Win_Pos(scaleDst, &pip_win_w, &pip_win_h, &pip_ofst_y, &pip_ofst_c);

						if ( (param.auto_framing_pip_mode==1) ||
							((param.auto_framing_pip_mode!=1) && (face_src_w != 0)) ) {
							g_scale_info[g_scale_idx].ctrl.src_img.addr.Y_G			= scaleSrc.mem_addr_y + face_ofst_y;
							g_scale_info[g_scale_idx].ctrl.src_img.addr.Cb_B		= scaleSrc.mem_addr_c + face_ofst_c;
							g_scale_info[g_scale_idx].ctrl.src_img.addr.Cr_R		= 0;
							g_scale_info[g_scale_idx].ctrl.src_img.addr.Alpha		= 0;
							g_scale_info[g_scale_idx].ctrl.src_img.g_y_g_width		= scaleSrc.yuv_h_byte;
							g_scale_info[g_scale_idx].ctrl.src_img.g_cb_b_width		= scaleSrc.yuv_h_byte;
							g_scale_info[g_scale_idx].ctrl.src_img.g_cr_r_width		= 0;
							g_scale_info[g_scale_idx].ctrl.src_img.g_alpha_width	= 0;
							g_scale_info[g_scale_idx].ctrl.src_img.width			= face_src_w;
							g_scale_info[g_scale_idx].ctrl.src_img.lines			= face_src_h;
							g_scale_info[g_scale_idx].ctrl.src_img.line_bytes.Y_G	= scaleSrc.yuv_h_byte;
							g_scale_info[g_scale_idx].ctrl.src_img.line_bytes.Cb_B	= scaleSrc.yuv_h_byte;
							g_scale_info[g_scale_idx].ctrl.src_img.line_bytes.Cr_R	= 0;
							g_scale_info[g_scale_idx].ctrl.src_img.line_bytes.Alpha	= 0;
							g_scale_info[g_scale_idx].ctrl.src_img.pfmt				= FJ_IMG_PFMT_YCC420;
						
							g_scale_info[g_scale_idx].ctrl.dst_img.addr.Y_G			= scaleDst.mem_addr_y + pip_ofst_y;
							g_scale_info[g_scale_idx].ctrl.dst_img.addr.Cb_B		= scaleDst.mem_addr_c + pip_ofst_c;
							g_scale_info[g_scale_idx].ctrl.dst_img.addr.Cr_R		= 0;
							g_scale_info[g_scale_idx].ctrl.dst_img.addr.Alpha		= 0;
							g_scale_info[g_scale_idx].ctrl.dst_img.g_y_g_width 		= scaleDst.yuv_h_byte;
							g_scale_info[g_scale_idx].ctrl.dst_img.g_cb_b_width		= scaleDst.yuv_h_byte;
							g_scale_info[g_scale_idx].ctrl.dst_img.g_cr_r_width		= 0;
							g_scale_info[g_scale_idx].ctrl.dst_img.g_alpha_width	= 0;
							g_scale_info[g_scale_idx].ctrl.dst_img.width			= pip_win_w;
							g_scale_info[g_scale_idx].ctrl.dst_img.lines			= pip_win_h;
							g_scale_info[g_scale_idx].ctrl.dst_img.line_bytes.Y_G	= scaleDst.yuv_h_byte;
							g_scale_info[g_scale_idx].ctrl.dst_img.line_bytes.Cb_B 	= scaleDst.yuv_h_byte;
							g_scale_info[g_scale_idx].ctrl.dst_img.line_bytes.Cr_R 	= 0;
							g_scale_info[g_scale_idx].ctrl.dst_img.line_bytes.Alpha	= 0;
							g_scale_info[g_scale_idx].ctrl.dst_img.pfmt				= FJ_IMG_PFMT_YCC420;
							
							g_scale_info[g_scale_idx].ctrl.fill_enable 				= 0;
							g_scale_info[g_scale_idx].ctrl.resize_mode 				= FJ_IMG_RESIZE_MODE_BILINEAR;
							g_scale_info[g_scale_idx].ctrl.timeout 					= -1;
							
							g_scale_info[g_scale_idx].addr_y						= scaleDst.mem_addr_y;
							g_scale_info[g_scale_idx].addr_c						= scaleDst.mem_addr_c;
							
							// paste SGDE_USER_1(ROI or FACE) on SHDR_YUV_1(FULL) by iip 
							OS_User_Set_Flg( FID_IIP_SCALE, (FLG_IIP_SCALE_START | (g_scale_idx&0xF)) );
						}

						/***************************************
	 					  3. convert YUV_1 to H264_0/YUV_0      
						****************************************/
						if ( g_pip_frame_no < 1 ) {
							// no iip result, ...make background only
							g_i2s_mng.shdr_in.mem_addr_y = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y;
							g_i2s_mng.shdr_in.mem_addr_c = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_c;
						}
						else {
							// use iip scale result of prev frame
							g_i2s_mng.shdr_in.mem_addr_y = g_scale_info[(g_scale_idx+1)&1].addr_y;
							g_i2s_mng.shdr_in.mem_addr_c = g_scale_info[(g_scale_idx+1)&1].addr_c;
						}
						g_i2s_mng.shdr_in.yuv_h_byte = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_byte;
						g_i2s_mng.shdr_in.yuv_h_size = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size;
						g_i2s_mng.shdr_in.yuv_v_size = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size;
						g_i2s_mng.shdr_in.yuv_format = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_format;

						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0] = bk_shdr_out_264_0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0 ] = bk_shdr_out_yuv_0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1 ].mem_addr_y = 0;
						
						// paste SHDR_YUV_1(PIP) on SHDR_H264_0/SHDR_YUV_0
						Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, FALSE, frame_no, g_i2s_in.cbcr_global_x2); // no comp
						
						/***************************************
	 					  4. update index                       
						 ***************************************/
						// mode==1: PIP = ROI + FULL
						if (param.auto_framing_pip_mode==1) {
							g_scale_idx = (g_scale_idx+1) & 1;
							g_pip_frame_no++;
						}
						// mode==2: PIP = FACE + ROI
						// mode==3: PIP = FACE + FULL
						else {
							if ( face_src_w != 0 ) {
								g_scale_idx = (g_scale_idx+1) & 1;
								g_pip_frame_no++;
							}
							else {
								g_scale_idx = 0;
								g_pip_frame_no = 0;
							}
						}
					
						// restore
						g_i2s_mng.shdr_in                          = bk_shdr_in;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265  ] = bk_shdr_out_265;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0] = bk_shdr_out_264_0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1] = bk_shdr_out_264_1;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0 ] = bk_shdr_out_yuv_0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1 ] = bk_shdr_out_yuv_1;
					}
					else {
						// back up
						bk_shdr_in = g_i2s_mng.shdr_in;
						bk_shdr_out_264_1 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1];
						bk_shdr_out_265 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265];
						bk_shdr_out_yuv_1 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1];
						
						//  [MAIN] or [Stream 2]
						// disable 264_1 and YUV_0
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y = 0;
						if(g_i2s_in.roi_mode >= 1){
							// disable H265
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].mem_addr_y = 0;
						}
					
						if((g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].mem_addr_y != 0)||
							(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y != 0)||
							(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y != 0)){
							// start for entire image without 264_1 and/or YUV_1
							Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, g_i2s_in.h265_compress, frame_no, g_i2s_in.cbcr_global_x2);
						}
						
						// [Stream 0]
						// change input to ROI
						g_i2s_mng.shdr_in.mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_y;
						g_i2s_mng.shdr_in.mem_addr_c = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_c;
						g_i2s_mng.shdr_in.yuv_h_byte = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_byte;
						g_i2s_mng.shdr_in.yuv_h_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_size;
						g_i2s_mng.shdr_in.yuv_v_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].v_size;
						// disable except 264_1 and YUV_1
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265  ].mem_addr_y = 0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK1_ME	 ].mem_addr_y = 0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y = 0;
						// enable 264_1
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1] = bk_shdr_out_264_1;
						// Overwrite ROI over YUV_1
						if(share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_size > g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size){
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_size >> 2;
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].v_size >> 2;
						}else{
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_size >> 1;
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_H264_1].v_size >> 1;
						}
						if(param.roi0_trim_ratio == 0){
							// disable YUV_1
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1 ].mem_addr_y = 0;
						}

						if((g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y != 0)||
							(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y != 0)||
							(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y != 0)){
							// start for ROI image with 264_1, YUV_0 and YUV_1
							Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, FALSE, frame_no, g_i2s_in.cbcr_global_x2); // no comp
						}
							
						// restore
						g_i2s_mng.shdr_in = bk_shdr_in;;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1] = bk_shdr_out_264_1;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265  ] = bk_shdr_out_265;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1 ] = bk_shdr_out_yuv_1;
					}
					
					if(g_i2s_in.roi_mode >= 1){
						
						// back up
						bk_shdr_in = g_i2s_mng.shdr_in;
						bk_shdr_out_265 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265];
						bk_shdr_out_yuv_1 = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1];
						
						// [Stream 1]
						// change input to ROI
						g_i2s_mng.shdr_in.mem_addr_y = share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].addr_y;
						g_i2s_mng.shdr_in.mem_addr_c = share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].addr_c;
						g_i2s_mng.shdr_in.yuv_h_byte = share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].h_byte;
						g_i2s_mng.shdr_in.yuv_h_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].h_size;
						g_i2s_mng.shdr_in.yuv_v_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].v_size;
						// disable except YUV_1 and H265
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK1_ME	 ].mem_addr_y = 0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y = 0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y = 0;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0 ].mem_addr_y = 0;
						// enable H265
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265 ] = bk_shdr_out_265;
						// Overwrite ROI over YUV_1
						if(g_rbk2layout_tbl[E_SHDR_OUT_RBK5_YUV_1] != E_SHDR_OUT_NONE){
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y += (bk_shdr_out_yuv_1.yuv_h_byte - share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].h_size / 2);
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_c += (bk_shdr_out_yuv_1.yuv_h_byte - share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].h_size / 2);
						}
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].h_size / 4;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size = share_sgde_yuv.layout[E_SHARE_YUV_OUT_RESERVE].v_size / 4;
						if(param.roi1_trim_ratio == 0){
							// disable YUV_1
							g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1 ].mem_addr_y = 0;
						}
						if((g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].mem_addr_y != 0)||
							(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y != 0)){
							// start for ROI image with YUV_1 and YUV_0
							Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, FALSE, frame_no, g_i2s_in.cbcr_global_x2); // no comp
						}
						
						// restore except address
						g_i2s_mng.shdr_in = bk_shdr_in;;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265 ] = bk_shdr_out_265;
						g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1] = bk_shdr_out_yuv_1;
						
					}
				} else {
					if(g_i2s_mng.shdr_out[E_SHDR_OUT_RBK0_H265].mem_addr_y != 0 ||
					   g_i2s_mng.shdr_out[E_SHDR_OUT_RBK1_ME].mem_addr_y != 0 ||
					   g_i2s_mng.shdr_out[E_SHDR_OUT_RBK2_H264_0].mem_addr_y != 0 ||
					   g_i2s_mng.shdr_out[E_SHDR_OUT_RBK3_H264_1].mem_addr_y != 0 ||
					   g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y != 0 ||
					   g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y != 0) {
						if((g_i2s_in.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_NO_ME) && (g_i2s_in.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE) ){
							if(g3dnrSkip_No_ShdrTsk == 0){
								Start_SHDR_Proc(&g_i2s_mng, g_i2s_in.op_3dnr_mode, g_i2s_in.h265_compress, frame_no, g_i2s_in.cbcr_global_x2);
							}
							else{
								Start_SHDR_Proc(&g_i2s_mng, E_SHARE_OP_3DNR_MODE_OFF, g_i2s_in.h265_compress, frame_no, g_i2s_in.cbcr_global_x2);
								g3dnrSkip_No_ShdrTsk--;
							}
						}
						else{
							Start_SHDR_Proc(&g_i2s_mng, g_i2s_in.op_3dnr_mode, g_i2s_in.h265_compress, frame_no, g_i2s_in.cbcr_global_x2);
						}
					}
				}

				if((g_i2s_in.cap_mode == TRUE) && ((g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL) || (g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY))){
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_0].addr_y = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_0].addr_c = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_c;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_0].h_size = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_size;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_0].v_size = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_v_size;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_0].format = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_format;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_0].video = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_video;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_0].h_byte = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_byte;
					
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_y = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_y;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_1].addr_c = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].mem_addr_c;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_size = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_size;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_1].v_size = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_v_size;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_1].format = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_format;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_1].video = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_video;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_H264_1].h_byte = g_i2s_mng.shdr_out[E_SHDR_OUT_RBK5_YUV_1].yuv_h_byte;
					
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_y = g_i2s_mng.shdr_in.mem_addr_y;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_USER_0].addr_c = g_i2s_mng.shdr_in.mem_addr_c;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_size = g_i2s_mng.shdr_in.yuv_h_size;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_USER_0].v_size = g_i2s_mng.shdr_in.yuv_v_size;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_USER_0].format = g_i2s_mng.shdr_in.yuv_format;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_USER_0].video = E_SHARE_YUV_VIDEO_NONE;
					g_share_shdr_yuv.layout[E_SHARE_YUV_OUT_USER_0].h_byte = g_i2s_mng.shdr_in.yuv_h_byte;

					// Update share memory
					Set_Frame_Share_SHDR_YUV(&g_share_shdr_yuv, frame_no);
				}
				FJ_Record_Timer_Handler();
				Snd_Event_to_System( E_SYSTEM_EVENT_SHDR_YUV, frame_no );
			} else { // if(frame_skip == 0)
				frame_skip--;
			}
		} // if( flgptn & FLG_IPU_3DNR_SHDR_START ){
		
		if( flgptn & FLG_IPU_3DNR_SHDR_RECONFIG ){
			OS_User_Clr_Flg( FID_IPU_3DNR, ~FLG_IPU_3DNR_SHDR_RECONFIG );
			
			frame_no = gFrame_No_ShdrTsk; // save local
			shdr_reconfig(frame_no);

			if(gFrameSkip_No_ShdrTsk > 0){
				frame_skip = gFrameSkip_No_ShdrTsk;
				gFrameSkip_No_ShdrTsk = 0;
			}
			
		} // if( flgptn & FLG_IPU_3DNR_SHDR_START ){
	} // while(1){
	
	OS_User_Ext_Tsk();
}

VOID Tsk_IPU_HDR_VHDR_Main( INT32 stacd )
{
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;
	
	while(1){
		ercd = OS_User_Wai_Flg( FID_IPU_HDR, (FLG_IPU_HDR_VHDR_START | FLG_IPU_HDR_STOP), D_OS_USER_TWF_ORW, &flgptn );
		if( ercd != D_OS_USER_E_OK ){
			UPRINTF("HDR VHDR Task Wai_Flg error %d\n", ercd);
		}

		if( flgptn & FLG_IPU_HDR_STOP ){
			break;
		}
		
		if( flgptn & FLG_IPU_HDR_VHDR_START ){
			OS_User_Clr_Flg( FID_IPU_HDR, ~FLG_IPU_HDR_VHDR_START );

			if(g_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR)
			{
				Start_VHDR_Proc(&g_i2s_mng, &g_share_r2y_dol_hist, gFrame_No_VhdrTsk);
			}
		}
		
	}
	
	OS_User_Ext_Tsk();
}

VOID Tsk_IIP_Image_Scale( INT32 stacd )
{
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;
	FJ_ERR_CODE		fjerr;
	UINT32			idx;
	
	while(1){
		ercd = OS_User_Wai_Flg( FID_IIP_SCALE, (FLG_IIP_SCALE_START | FLG_IIP_SCALE_STOP | 0xF), D_OS_USER_TWF_ORW, &flgptn );
		if( ercd != D_OS_USER_E_OK ){
			UPRINTF("IIP SCALE Task Wai_Flg error %d\n", ercd);
		}

		if( flgptn & FLG_IIP_SCALE_STOP ){
			break;
		}
		if( flgptn & FLG_IIP_SCALE_START ){
			idx = flgptn & 0xF;
			OS_User_Clr_Flg( FID_IIP_SCALE, ~(FLG_IIP_SCALE_START|0xF) );
			
			fjerr = FJ_ImageConvert( &(g_scale_info[idx].ctrl) );
			if (fjerr != FJ_ERR_OK) {
				UPRINTF("FJ_ImageConvert error %d\n", fjerr);
			}
		}
	}
	
	OS_User_Ext_Tsk();
}


#else
VOID Tsk_IPU_3DNR_GMD_Main( INT32 stacd )
{
	OS_User_Ext_Tsk();
}

VOID Tsk_IPU_3DNR_ME_Main( INT32 stacd )
{
	OS_User_Ext_Tsk();
}
	
VOID Tsk_IPU_3DNR_SHDR_Main( INT32 stacd )
{
	OS_User_Ext_Tsk();
}
	
VOID Tsk_IPU_HDR_VHDR_Main( INT32 stacd )
{
	OS_User_Ext_Tsk();
}
	
#endif

