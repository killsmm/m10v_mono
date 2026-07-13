/**
 * @file		ctrl_warp_calc.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include <math.h>
#include "mode_stm.h"
#include "ctrl_warp_calc.h"
#include "os_user_custom.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define IS_FLGPTN_EQUAL(flgptn, FLG)	((((flgptn) & (FLG)) == (FLG)) ? TRUE : FALSE)
/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_IP_GE_LGRID   gInter_local_back;
static T_LDC_IMG_INFO g_img_info;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void ldc_inter_calc_m3(T_LDC_IMG_INFO* img_info, INT32 index);
static void ldc_inter_make_m3(T_LDC_IMG_INFO* img_info);
#if 0
static void ldc_inter_calc_m2_2(T_LDC_IMG_INFO* img_info, INT32 index);
static void ldc_inter_calc_m2_1(T_LDC_IMG_INFO* img_info, INT32 index);
static void ldc_inter_calc_m2(T_LDC_IMG_INFO* img_info, INT32 index);
static void ldc_inter_make_m2(T_LDC_IMG_INFO* img_info);
#endif
#if 0
static void ldc_inter_calc_m1(T_LDC_IMG_INFO* img_info, INT32 index);
static void ldc_inter_make_m1(T_LDC_IMG_INFO* img_info);
#endif
static void ldc_inter_calc_zoom(T_LDC_IMG_INFO* img_info, INT32 index, INT32 zoom_idx, DOUBLE *nodes_x, DOUBLE *nodes_y);
static void ldc_inter_proc(T_LDC_IMG_INFO* img_info);

/*==============================================================================*/
/* Debug Function																*/
/*==============================================================================*/

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void Ctrl_Warp_Calculation(T_LDC_IMG_INFO* img_info)
{
	static USHORT	out_img_h = 0, out_img_v = 0;
	static ULONG	tbl_address = 0;
	static FLOAT	zoom_ratio = 0, zoom_pos_x = 0, zoom_pos_y = 0;
	static BOOL		previous_proc_running = FALSE;
	OS_USER_ER		ercd = 0;
	OS_USER_FLGPTN	flgptn = 0;

	if((out_img_h == img_info->out_img_h) &&
		(out_img_v == img_info->out_img_v) &&
		(tbl_address == img_info->tbl_address) &&
		(zoom_ratio == img_info->zoom_ratio) &&
		(zoom_pos_x == img_info->zoom_pos_x) &&
		(zoom_pos_y == img_info->zoom_pos_y))
	{
		img_info->out_inter_local.width		= gInter_local_back.width;
		img_info->out_inter_local.height	= gInter_local_back.height;
		img_info->out_inter_local.pitchX	= gInter_local_back.pitchX;
		img_info->out_inter_local.pitchY	= gInter_local_back.pitchY;
		img_info->out_inter_local.outputX	= gInter_local_back.outputX;
		img_info->out_inter_local.outputY	= gInter_local_back.outputY;
		img_info->out_inter_local.nodes		= gInter_local_back.nodes;
		img_info->out_inter_local.xform		= gInter_local_back.xform;
		return;
	}

	if (img_info->_1st_ldc_calc) {
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "ldc_calc_s"));
		memcpy((void*)img_info->in_work_nodes, img_info->in_local->nodes, sizeof(T_IP_GE_LNODE) * D_LOCAL_GRID_H_MAX_NUM * D_LOCAL_GRID_V_MAX_NUM);
		memcpy((void*)img_info->in_work_nodes_coor, img_info->in_nodes_coor, sizeof(T_IP_GE_LNODE) * D_LOCAL_GRID_H_MAX_NUM * D_LOCAL_GRID_V_MAX_NUM);
		ldc_inter_proc(img_info);
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "ldc_calc_s"));

		gInter_local_back.width		= img_info->out_inter_local.width 	= img_info->inter_local->width;
		gInter_local_back.height	= img_info->out_inter_local.height 	= img_info->inter_local->height;
		gInter_local_back.pitchX	= img_info->out_inter_local.pitchX 	= img_info->inter_local->pitchX;
		gInter_local_back.pitchY	= img_info->out_inter_local.pitchY 	= img_info->inter_local->pitchY;
		gInter_local_back.outputX	= img_info->out_inter_local.outputX = img_info->inter_local->outputX;
		gInter_local_back.outputY	= img_info->out_inter_local.outputY = img_info->inter_local->outputY;
		gInter_local_back.xform		= img_info->out_inter_local.xform 	= img_info->inter_local->xform;
		memcpy((void*)img_info->out_inter_local.nodes, (void*)img_info->inter_local->nodes, sizeof(T_IP_GE_LNODE) * D_LOCAL_GRID_H_MAX_NUM * D_LOCAL_GRID_V_MAX_NUM);
		gInter_local_back.nodes		= img_info->out_inter_local.nodes;

		out_img_h	= img_info->out_img_h;
		out_img_v	= img_info->out_img_v;
		tbl_address = img_info->tbl_address;
		zoom_ratio	= img_info->zoom_ratio;
		zoom_pos_x	= img_info->zoom_pos_x;
		zoom_pos_y	= img_info->zoom_pos_y;
	} else {
		if (previous_proc_running == FALSE) {
			memcpy((void*)img_info->in_work_nodes, img_info->in_local->nodes, sizeof(T_IP_GE_LNODE) * D_LOCAL_GRID_H_MAX_NUM * D_LOCAL_GRID_V_MAX_NUM);
			memcpy((void*)img_info->in_work_nodes_coor, img_info->in_nodes_coor, sizeof(T_IP_GE_LNODE) * D_LOCAL_GRID_H_MAX_NUM * D_LOCAL_GRID_V_MAX_NUM);
			g_img_info = *img_info;
			ercd = OS_User_Set_Flg( FID_LDC_CALC, FLG_LDC_CALC_START );
			UPRINTF_ERR(ercd);

			img_info->out_inter_local.width		= gInter_local_back.width;
			img_info->out_inter_local.height	= gInter_local_back.height;
			img_info->out_inter_local.pitchX	= gInter_local_back.pitchX;
			img_info->out_inter_local.pitchY	= gInter_local_back.pitchY;
			img_info->out_inter_local.outputX	= gInter_local_back.outputX;
			img_info->out_inter_local.outputY	= gInter_local_back.outputY;
			img_info->out_inter_local.nodes		= gInter_local_back.nodes;
			img_info->out_inter_local.xform		= gInter_local_back.xform;
			previous_proc_running = TRUE;
		} else {
			ercd = OS_User_Twai_Flg( FID_LDC_CALC_DONE, FLG_LDC_CALC_DONE_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, 1 ); // wait for 1ms at least (min unit)
			OS_User_Clr_Flg( FID_LDC_CALC_DONE, ~FLG_LDC_CALC_DONE_WAITPTN );
			if (ercd == D_OS_USER_E_OK) {
				gInter_local_back.width		= img_info->out_inter_local.width 	= g_img_info.inter_local->width;
				gInter_local_back.height	= img_info->out_inter_local.height 	= g_img_info.inter_local->height;
				gInter_local_back.pitchX	= img_info->out_inter_local.pitchX 	= g_img_info.inter_local->pitchX;
				gInter_local_back.pitchY	= img_info->out_inter_local.pitchY 	= g_img_info.inter_local->pitchY;
				gInter_local_back.outputX	= img_info->out_inter_local.outputX = g_img_info.inter_local->outputX;
				gInter_local_back.outputY	= img_info->out_inter_local.outputY = g_img_info.inter_local->outputY;
				gInter_local_back.xform		= img_info->out_inter_local.xform 	= g_img_info.inter_local->xform;
				memcpy((void*)img_info->out_inter_local.nodes, (void*)g_img_info.inter_local->nodes, sizeof(T_IP_GE_LNODE) * D_LOCAL_GRID_H_MAX_NUM * D_LOCAL_GRID_V_MAX_NUM);
				gInter_local_back.nodes		= img_info->out_inter_local.nodes;

				out_img_h	= g_img_info.out_img_h;
				out_img_v	= g_img_info.out_img_v;
				tbl_address = g_img_info.tbl_address;
				zoom_ratio	= g_img_info.zoom_ratio;
				zoom_pos_x	= g_img_info.zoom_pos_x;
				zoom_pos_y	= g_img_info.zoom_pos_y;
				previous_proc_running = FALSE;
			} else if (ercd == D_OS_USER_E_TMOUT) {
				img_info->out_inter_local.width		= gInter_local_back.width;
				img_info->out_inter_local.height	= gInter_local_back.height;
				img_info->out_inter_local.pitchX	= gInter_local_back.pitchX;
				img_info->out_inter_local.pitchY	= gInter_local_back.pitchY;
				img_info->out_inter_local.outputX	= gInter_local_back.outputX;
				img_info->out_inter_local.outputY	= gInter_local_back.outputY;
				img_info->out_inter_local.nodes		= gInter_local_back.nodes;
				img_info->out_inter_local.xform		= gInter_local_back.xform;
			} else {
				UPRINTF_ERR(ercd);
			}
		}
	}
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

static void ldc_inter_calc_m3(T_LDC_IMG_INFO* img_info, INT32 index)
{
	INT32 x, y, idx;
	FLOAT inter_coor_x, inter_coor_y, nodes_tmp;
	INT32 aidx, bidx, cidx, didx;
    T_IP_GE_LNODE ref[2];	
	FLOAT rate[4];

	inter_coor_x = img_info->inter_nodes_coor[index].u;
	inter_coor_y = img_info->inter_nodes_coor[index].v;
	for (y = 0; y < img_info->rsz_local->height; y++) {
		idx = y * img_info->rsz_local->width;
		
		if(inter_coor_y == img_info->rsz_nodes_coor[idx].v){
			aidx = 0;
			bidx = 0;
			for (x = 0; x < img_info->rsz_local->width; x++){
				idx = x + y * img_info->rsz_local->width;
				
				if(inter_coor_x == img_info->rsz_nodes_coor[idx].u){
					img_info->inter_local->nodes[index].u = img_info->rsz_local->nodes[idx].u;
					img_info->inter_local->nodes[index].v = img_info->rsz_local->nodes[idx].v;

					return;
				}else if(inter_coor_x > img_info->rsz_nodes_coor[idx].u){
					aidx = idx;
				}else if(inter_coor_x < img_info->rsz_nodes_coor[idx].u){
					bidx = idx;

					// A(x,y)---P(x,y)---B(x,y)
					// Rate0 = (oBx - oPx)/(oBx - oAx)
					// Rate1 = (oPx - oAx)/(oBx - oAx)
					nodes_tmp = img_info->rsz_nodes_coor[bidx].u - inter_coor_x;
					rate[0] = nodes_tmp / (img_info->rsz_nodes_coor[bidx].u - img_info->rsz_nodes_coor[aidx].u);
					nodes_tmp = inter_coor_x - img_info->rsz_nodes_coor[aidx].u;
					rate[1] = nodes_tmp / (img_info->rsz_nodes_coor[bidx].u - img_info->rsz_nodes_coor[aidx].u);
					// iP(x,y) = Rate0 * iA(x,y) + Rate1 * iB(x,y)
					nodes_tmp = rate[0] * img_info->rsz_local->nodes[aidx].u;
					img_info->inter_local->nodes[index].u = nodes_tmp + rate[1] * img_info->rsz_local->nodes[bidx].u;
					nodes_tmp = rate[0] * img_info->rsz_local->nodes[aidx].v;
					img_info->inter_local->nodes[index].v = nodes_tmp + rate[1] * img_info->rsz_local->nodes[bidx].v;
					return;
				}			
			}

			aidx = index - 2;
			bidx = index - 1;
			
			// A(x,y)---B(x,y)---P(x,y)
			// Rate0 = (oPx - oBx)/(oPx - oAx)
			// Rate1 = (oBx - oAx)/(oPx - oAx)
			nodes_tmp = inter_coor_x - img_info->inter_nodes_coor[bidx].u;
			rate[0] = nodes_tmp / (inter_coor_x - img_info->inter_nodes_coor[aidx].u);
			nodes_tmp = img_info->inter_nodes_coor[bidx].u - img_info->inter_nodes_coor[aidx].u;
			rate[1] = nodes_tmp / (inter_coor_x - img_info->inter_nodes_coor[aidx].u);
			// iP(x,y) = [iB(x,y) - Rate0 * iA(x,y)] / Rate1
			nodes_tmp = img_info->inter_local->nodes[bidx].u - rate[0] * img_info->inter_local->nodes[aidx].u;
			img_info->inter_local->nodes[index].u = nodes_tmp / rate[1];
			nodes_tmp = img_info->inter_local->nodes[bidx].v - rate[0] * img_info->inter_local->nodes[aidx].v;
			img_info->inter_local->nodes[index].v = nodes_tmp / rate[1];
			return;
		}else if(inter_coor_y > img_info->rsz_nodes_coor[idx].v){
			cidx = idx;
		}else if(inter_coor_y < img_info->rsz_nodes_coor[idx].v){
			
			aidx = 0;
			bidx = 0;
			for (x = 0; x < img_info->rsz_local->width; x++){
				idx = x + y * img_info->rsz_local->width;
				
				if(inter_coor_x == img_info->rsz_nodes_coor[idx].u){
					bidx = idx;
					aidx = bidx - img_info->rsz_local->width;

					// A(x,y)---P(x,y)---B(x,y)
					// Rate0 = (oBy - oPy)/(oBy - oAy)
					// Rate1 = (oPy - oAy)/(oBy - oAy)
					nodes_tmp = img_info->rsz_nodes_coor[bidx].v - inter_coor_y;
					rate[0] = nodes_tmp / (img_info->rsz_nodes_coor[bidx].v - img_info->rsz_nodes_coor[aidx].v);
					nodes_tmp = inter_coor_y - img_info->rsz_nodes_coor[aidx].v;
					rate[1] = nodes_tmp / (img_info->rsz_nodes_coor[bidx].v - img_info->rsz_nodes_coor[aidx].v);
					// iP(x,y) = Rate0 * iA(x,y) + Rate1 * iB(x,y)
					nodes_tmp = rate[0] * img_info->rsz_local->nodes[aidx].u;
					img_info->inter_local->nodes[index].u = nodes_tmp + rate[1] * img_info->rsz_local->nodes[bidx].u;
					nodes_tmp = rate[0] * img_info->rsz_local->nodes[aidx].v;
					img_info->inter_local->nodes[index].v = nodes_tmp + rate[1] * img_info->rsz_local->nodes[bidx].v;
					return;
				}else if(inter_coor_x > img_info->rsz_nodes_coor[idx].u){
					aidx = idx;
				}else if(inter_coor_x < img_info->rsz_nodes_coor[idx].u){
					bidx = idx;
					cidx = aidx - img_info->rsz_local->width;					
					didx = bidx - img_info->rsz_local->width;					

					// C(x,y)------------D(x,y)
					// ---------P(x,y)---------
					// A(x,y)------------B(x,y)
					// Rate0 = (oBx - oPx)/(oBx - oAx)
					// Rate1 = (oPx - oAx)/(oBx - oAx)
					// Rate2 = (oCy - oPy)/(oCy - oAy)
					// Rate3 = (oPy - oAy)/(oCy - oAy)
					nodes_tmp = img_info->rsz_nodes_coor[bidx].u - inter_coor_x;
					rate[0] = nodes_tmp / (img_info->rsz_nodes_coor[bidx].u - img_info->rsz_nodes_coor[aidx].u);
					nodes_tmp = inter_coor_x - img_info->rsz_nodes_coor[aidx].u;
					rate[1] = nodes_tmp / (img_info->rsz_nodes_coor[bidx].u - img_info->rsz_nodes_coor[aidx].u);
					nodes_tmp = img_info->rsz_nodes_coor[cidx].v - inter_coor_y;
					rate[2] = nodes_tmp / (img_info->rsz_nodes_coor[cidx].v - img_info->rsz_nodes_coor[aidx].v);
					nodes_tmp = inter_coor_y - img_info->rsz_nodes_coor[aidx].v;
					rate[3] = nodes_tmp / (img_info->rsz_nodes_coor[cidx].v - img_info->rsz_nodes_coor[aidx].v);
					// Ref0(x,y) = Rate0 * iA(x,y) + Rate1 * iB(x,y)
					nodes_tmp = rate[0] * img_info->rsz_local->nodes[aidx].u;
					ref[0].u = nodes_tmp + rate[1] * img_info->rsz_local->nodes[bidx].u;
					nodes_tmp = rate[0] * img_info->rsz_local->nodes[aidx].v;
					ref[0].v = nodes_tmp + rate[1] * img_info->rsz_local->nodes[bidx].v;
					// Ref1(x,y) = Rate0 * iC(x,y) + Rate1 * iD(x,y)
					nodes_tmp = rate[0] * img_info->rsz_local->nodes[cidx].u;
					ref[1].u = nodes_tmp + rate[1] * img_info->rsz_local->nodes[didx].u;
					nodes_tmp = rate[0] * img_info->rsz_local->nodes[cidx].v;
					ref[1].v = nodes_tmp + rate[1] * img_info->rsz_local->nodes[didx].v;
					// iP(x,y) = Rate2 * Ref0(x,y) + Rate3 * Ref1(x,y)
					img_info->inter_local->nodes[index].u = rate[2] * ref[0].u + rate[3] * ref[1].u;
					img_info->inter_local->nodes[index].v = rate[2] * ref[0].v + rate[3] * ref[1].v;
					return;
				}
			}

			aidx = index - 2;
			bidx = index - 1;
			
			// A(x,y)---B(x,y)---P(x,y)
			// Rate0 = (oPx - oBx)/(oPx - oAx)
			// Rate1 = (oBx - oAx)/(oPx - oAx)
			nodes_tmp = inter_coor_x - img_info->inter_nodes_coor[bidx].u;
			rate[0] = nodes_tmp / (inter_coor_x - img_info->inter_nodes_coor[aidx].u);
			nodes_tmp = img_info->inter_nodes_coor[bidx].u - img_info->inter_nodes_coor[aidx].u;
			rate[1] = nodes_tmp / (inter_coor_x - img_info->inter_nodes_coor[aidx].u);
			// iP(x,y) = [iB(x,y) - Rate0 * iA(x,y)] / Rate1
			nodes_tmp = img_info->inter_local->nodes[bidx].u - rate[0] * img_info->inter_local->nodes[aidx].u;
			img_info->inter_local->nodes[index].u = nodes_tmp / rate[1];
			nodes_tmp = img_info->inter_local->nodes[bidx].v - rate[0] * img_info->inter_local->nodes[aidx].v;
			img_info->inter_local->nodes[index].v = nodes_tmp / rate[1];
			return;
		}
	}	

	aidx = index - 2 * img_info->inter_local->width;
	bidx = index - 1 * img_info->inter_local->width;
			
	// A(x,y)---B(x,y)---P(x,y)
	// Rate0 = (oPy - oBy)/(oPy - oAy)
	// Rate1 = (oBy - oAy)/(oPy - oAy)
	nodes_tmp = inter_coor_y - img_info->inter_nodes_coor[bidx].v;
	rate[0] = nodes_tmp / (inter_coor_y - img_info->inter_nodes_coor[aidx].v);
	nodes_tmp = img_info->inter_nodes_coor[bidx].v - img_info->inter_nodes_coor[aidx].v;
	rate[1] = nodes_tmp / (inter_coor_y - img_info->inter_nodes_coor[aidx].v);
	// iP(x,y) = [iB(x,y) - Rate0 * iA(x,y)] / Rate1
	nodes_tmp = img_info->inter_local->nodes[bidx].u - rate[0] * img_info->inter_local->nodes[aidx].u;
	img_info->inter_local->nodes[index].u = nodes_tmp / rate[1];
	nodes_tmp = img_info->inter_local->nodes[bidx].v - rate[0] * img_info->inter_local->nodes[aidx].v;
	img_info->inter_local->nodes[index].v = nodes_tmp / rate[1];
	return;

}

static void ldc_inter_make_m3(T_LDC_IMG_INFO* img_info)
{
	INT32 x, y, index;
	USHORT interval_x, interval_y;
	
	img_info->inter_local->pitchX	= img_info->rsz_local->pitchX;
	img_info->inter_local->pitchY	= img_info->rsz_local->pitchY;

	interval_x = (USHORT)pow(2,img_info->inter_local->pitchX);
	interval_y = (USHORT)pow(2,img_info->inter_local->pitchY);

	if(img_info->out_img_h % interval_x)
		img_info->out_num_cell_h = img_info->out_img_h / interval_x + 1;
	else
		img_info->out_num_cell_h = img_info->out_img_h / interval_x;
		
	if(img_info->out_img_v % interval_y)
		img_info->out_num_cell_v = img_info->out_img_v / interval_y + 1;
	else
		img_info->out_num_cell_v = img_info->out_img_v / interval_y;

	img_info->inter_local->width	= img_info->out_num_cell_h + 1;
	img_info->inter_local->height	= img_info->out_num_cell_v + 1;
	img_info->inter_local->outputX	= 0;
	img_info->inter_local->outputY	= 0;
	img_info->inter_local->xform	= NULL;
	
    printf("LDC Inter Info(m3).\n");
    printf("[input image] width:%d height:%d\n",img_info->in_img_h, img_info->in_img_v);
    printf("[output image] width:%d height:%d\n",img_info->out_img_h, img_info->out_img_v);
    printf("[cell number] width:%d height:%d\n",img_info->out_num_cell_h, img_info->out_num_cell_v);
    printf("[pitch] X:%d(%d) Y:%d(%d)\n",img_info->inter_local->pitchX, interval_x, img_info->inter_local->pitchY, interval_y);
    printf("[nodes] width:%d height:%d\n",img_info->inter_local->width, img_info->inter_local->height);

	for (y = 0; y < img_info->inter_local->height; y++) {
		for (x = 0; x < img_info->inter_local->width; x++) {
			index = x + y * img_info->inter_local->width;
			img_info->inter_nodes_coor[index].u = (FLOAT)interval_x * x;
			img_info->inter_nodes_coor[index].v = (FLOAT)interval_y * y;
				
			ldc_inter_calc_m3(img_info, index);
		}
	}	
}

#if 0
static void ldc_inter_calc_m2_2(T_LDC_IMG_INFO* img_info, INT32 index)
{
	INT32 x, y, idx;
	FLOAT inter_coor_x, inter_coor_y, nodes_tmp, nodes_rate;
	INT32 aidx, bidx;

	inter_coor_x = img_info->inter_nodes_coor[index].u;
	inter_coor_y = img_info->inter_nodes_coor[index].v;

	aidx = 0;
	bidx = 0;
	y = index / img_info->inter_local->width;
	for(x = 0; x < img_info->inter_local->width; x++){
		idx = x + y * img_info->inter_local->width;

		if((img_info->inter_local->nodes[idx].u != 0xFFFFFFFF) && (img_info->inter_local->nodes[idx].v != 0xFFFFFFFF)){
			if(inter_coor_x == img_info->inter_nodes_coor[idx].u){

				printf("\n**TEST** [m2-2] Found X(%d)??? [%f, %f]=[%f, %f]"
				,idx
				,inter_coor_x
				,inter_coor_y
				,img_info->inter_local->nodes[idx].u
				,img_info->inter_local->nodes[idx].v);

				return;
			}else if(inter_coor_x > img_info->inter_nodes_coor[idx].u){
				aidx = idx;
			}else if(inter_coor_x < img_info->inter_nodes_coor[idx].u){
				bidx = idx;

				// Rate = (oABx - oAx) / (oBx - oAx)
				nodes_rate = img_info->inter_nodes_coor[index].u - img_info->inter_nodes_coor[aidx].u;
				nodes_tmp = img_info->inter_nodes_coor[bidx].u - img_info->inter_nodes_coor[aidx].u;
				nodes_rate = nodes_rate / nodes_tmp;
				
				//(iABx - iAx) / (iBx - iAx) = Rate
				//=> iABx = Rate * (iBx - iAx) + iAx;
				nodes_tmp = img_info->inter_local->nodes[bidx].u - img_info->inter_local->nodes[aidx].u;
				img_info->inter_local->nodes[index].u = nodes_rate * nodes_tmp + img_info->inter_local->nodes[aidx].u;
				
				//(iABy - iAy) / (iBy - iAy) = Rate
				//=> iABy = Rate * (iBy - iAy) + iAy;
				nodes_tmp = img_info->inter_local->nodes[bidx].v - img_info->inter_local->nodes[aidx].v;
				img_info->inter_local->nodes[index].v = nodes_rate * nodes_tmp + img_info->inter_local->nodes[aidx].v;
				
				return;
			}			
		}
		
	}


	img_info->inter_local->nodes[index].u = 0.0;
	img_info->inter_local->nodes[index].v = 0.0;
					
	printf("\n**TEST** [m2-2] Not Found X [%f, %f]"
		,img_info->inter_nodes_coor[index].u
		,img_info->inter_nodes_coor[index].v);

	return;

}

static void ldc_inter_calc_m2_1(T_LDC_IMG_INFO* img_info, INT32 index)
{
	INT32 x, y, idx;
	FLOAT inter_coor_x, inter_coor_y, nodes_tmp;
	INT32 aidx, bidx, cidx;

	inter_coor_x = img_info->inter_nodes_coor[index].u;
	inter_coor_y = img_info->inter_nodes_coor[index].v;
	for (y = 0; y < img_info->rsz_local->height; y++) {
		idx = y * img_info->rsz_local->width;
		
		if(inter_coor_y == img_info->rsz_nodes_coor[idx].v){
			aidx = 0;
			bidx = 0;
			for (x = 0; x < img_info->rsz_local->width; x++){
				idx = x + y * img_info->rsz_local->width;
				
				if(inter_coor_x == img_info->rsz_nodes_coor[idx].u){
					img_info->inter_local->nodes[index].u = img_info->rsz_local->nodes[idx].u;
					img_info->inter_local->nodes[index].v = img_info->rsz_local->nodes[idx].v;

					return;
				}else if(inter_coor_x > img_info->rsz_nodes_coor[idx].u){
					aidx = idx;
				}else if(inter_coor_x < img_info->rsz_nodes_coor[idx].u){
					bidx = idx;
					//(iBx - iAx) / (oBx - oAx) = (iABx - iAx) / (oABx - oAx)
					//=> iABx = (iBx - iAx) * (oABx - oAx) / (oBx - oAx) + iAx
					if(img_info->rsz_local->nodes[bidx].v != img_info->rsz_local->nodes[aidx].v){
						nodes_tmp = (img_info->rsz_local->nodes[bidx].u - img_info->rsz_local->nodes[aidx].u);
						nodes_tmp = nodes_tmp * (inter_coor_x - img_info->rsz_nodes_coor[aidx].u);
						nodes_tmp = nodes_tmp / (img_info->rsz_nodes_coor[bidx].u - img_info->rsz_nodes_coor[aidx].u);
						img_info->inter_local->nodes[index].u = nodes_tmp + img_info->rsz_local->nodes[aidx].u;
					}else{
						img_info->inter_local->nodes[index].u = img_info->rsz_local->nodes[aidx].u;
					}

					//(iBy - iAy) / (iBx - iAx) = (iABy - iAy) / (iABx - iAx)
					//=> iABy = (iBy - iAy) * (iABx - iAx) / (iBx - iAx) + iAy
					if(img_info->rsz_local->nodes[bidx].v != img_info->rsz_local->nodes[aidx].v){
						nodes_tmp = (img_info->rsz_local->nodes[bidx].v - img_info->rsz_local->nodes[aidx].v);
						nodes_tmp = nodes_tmp * (img_info->inter_local->nodes[index].u - img_info->rsz_local->nodes[aidx].u);
						nodes_tmp = nodes_tmp / (img_info->rsz_local->nodes[bidx].u - img_info->rsz_local->nodes[aidx].u);
						img_info->inter_local->nodes[index].v = nodes_tmp + img_info->rsz_local->nodes[aidx].v;
					}else{
						img_info->inter_local->nodes[index].v = img_info->rsz_local->nodes[aidx].v;
					}

					return;
				}			
			}

			printf("\n**TEST** Found Y, Not found X [%f, %f]"
			,img_info->inter_nodes_coor[index].u
			,img_info->inter_nodes_coor[index].v);
			
			return;
		}else if(inter_coor_y > img_info->rsz_nodes_coor[idx].v){
			cidx = idx;
		}else if(inter_coor_y < img_info->rsz_nodes_coor[idx].v){
			
			aidx = 0;
			bidx = 0;
			for (x = 0; x < img_info->rsz_local->width; x++){
				idx = x + y * img_info->rsz_local->width;
				
				if(inter_coor_x == img_info->rsz_nodes_coor[idx].u){
					aidx = idx;

					cidx = aidx - img_info->rsz_local->width;					

					//(iCy - iAy) / (oCy - oAy) = (iACy - iAy) / (oACy - oAy)
					//=> iACy = (iCy - iAy) * (oACy - oAy) / (oCy - oAy) + iAy
					nodes_tmp = (img_info->rsz_local->nodes[cidx].v - img_info->rsz_local->nodes[aidx].v);
					nodes_tmp = nodes_tmp * (inter_coor_y - img_info->rsz_nodes_coor[aidx].v);
					nodes_tmp = nodes_tmp / (img_info->rsz_nodes_coor[cidx].v - img_info->rsz_nodes_coor[aidx].v );
					img_info->inter_local->nodes[index].v = nodes_tmp + img_info->rsz_local->nodes[aidx].v;

					//(iCx - iAx) / (iCy - iAy) = (iACx - iAx) / (iACy - iAy)
					//=> iACx = (iCx - iAx) * (iACy - iAy) / (iCy - iAy) + iAx
					nodes_tmp = (img_info->rsz_local->nodes[cidx].u - img_info->rsz_local->nodes[aidx].u);
					nodes_tmp = nodes_tmp * (img_info->inter_local->nodes[index].v - img_info->rsz_local->nodes[aidx].v);
					nodes_tmp = nodes_tmp / (img_info->rsz_local->nodes[cidx].v - img_info->rsz_local->nodes[aidx].v);
					img_info->inter_local->nodes[index].u = nodes_tmp + img_info->rsz_local->nodes[aidx].u;

					return;
				}else if(inter_coor_x > img_info->rsz_nodes_coor[idx].u){
					aidx = idx;
				}else if(inter_coor_x < img_info->rsz_nodes_coor[idx].u){
					bidx = idx;
					
					cidx = aidx - img_info->rsz_local->width;					

					img_info->inter_local->nodes[index].u = 0xFFFFFFFF;
					img_info->inter_local->nodes[index].v = 0xFFFFFFFF;

					return;
				}
			}

			printf("\n**TEST** Not found Y, Not found X [%f, %f]"
			,img_info->inter_nodes_coor[index].u
			,img_info->inter_nodes_coor[index].v);

			return;
		}
	}	

	aidx = index - 2 * img_info->inter_local->width;;
	bidx = index - 1 * img_info->inter_local->width;;
			
	//iBx / iAx = iCx / iBx
	//=>iCx = iBx * iBx / iAx
	nodes_tmp = img_info->inter_local->nodes[bidx].u * img_info->inter_local->nodes[bidx].u;
	nodes_tmp = nodes_tmp / img_info->inter_local->nodes[aidx].u;
	img_info->inter_local->nodes[index].u = nodes_tmp;

	//iBy / iAy = iCy / iBy
	//=>iCy = iBy * iBy / iAy
	nodes_tmp = img_info->inter_local->nodes[bidx].v * img_info->inter_local->nodes[bidx].v;
	nodes_tmp = nodes_tmp / img_info->inter_local->nodes[aidx].v;
	img_info->inter_local->nodes[index].v = nodes_tmp;

	return;

}

static void ldc_inter_calc_m2(T_LDC_IMG_INFO* img_info, INT32 index)
{
	INT32 x, y, idx;
	FLOAT inter_coor_x, inter_coor_y, nodes_tmp;
	INT32 aidx, bidx, cidx;

	inter_coor_x = img_info->inter_nodes_coor[index].u;
	inter_coor_y = img_info->inter_nodes_coor[index].v;
	for (y = 0; y < img_info->rsz_local->height; y++) {
		idx = y * img_info->rsz_local->width;
		
		if(inter_coor_y == img_info->rsz_nodes_coor[idx].v){
			aidx = 0;
			bidx = 0;
			for (x = 0; x < img_info->rsz_local->width; x++){
				idx = x + y * img_info->rsz_local->width;
				
				if(inter_coor_x == img_info->rsz_nodes_coor[idx].u){
					img_info->inter_local->nodes[index].u = img_info->rsz_local->nodes[idx].u;
					img_info->inter_local->nodes[index].v = img_info->rsz_local->nodes[idx].v;

					return;
				}else if(inter_coor_x > img_info->rsz_nodes_coor[idx].u){
					aidx = idx;
				}else if(inter_coor_x < img_info->rsz_nodes_coor[idx].u){
					bidx = idx;
					//(iBx - iAx) / (oBx - oAx) = (iABx - iAx) / (oABx - oAx)
					//=> iABx = (iBx - iAx) * (oABx - oAx) / (oBx - oAx) + iAx
					if(img_info->rsz_local->nodes[bidx].v != img_info->rsz_local->nodes[aidx].v){
						nodes_tmp = (img_info->rsz_local->nodes[bidx].u - img_info->rsz_local->nodes[aidx].u);
						nodes_tmp = nodes_tmp * (inter_coor_x - img_info->rsz_nodes_coor[aidx].u);
						nodes_tmp = nodes_tmp / (img_info->rsz_nodes_coor[bidx].u - img_info->rsz_nodes_coor[aidx].u);
						img_info->inter_local->nodes[index].u = nodes_tmp + img_info->rsz_local->nodes[aidx].u;
					}else{
						img_info->inter_local->nodes[index].u = img_info->rsz_local->nodes[aidx].u;
					}

					//(iBy - iAy) / (iBx - iAx) = (iABy - iAy) / (iABx - iAx)
					//=> iABy = (iBy - iAy) * (iABx - iAx) / (iBx - iAx) + iAy
					if(img_info->rsz_local->nodes[bidx].v != img_info->rsz_local->nodes[aidx].v){
						nodes_tmp = (img_info->rsz_local->nodes[bidx].v - img_info->rsz_local->nodes[aidx].v);
						nodes_tmp = nodes_tmp * (img_info->inter_local->nodes[index].u - img_info->rsz_local->nodes[aidx].u);
						nodes_tmp = nodes_tmp / (img_info->rsz_local->nodes[bidx].u - img_info->rsz_local->nodes[aidx].u);
						img_info->inter_local->nodes[index].v = nodes_tmp + img_info->rsz_local->nodes[aidx].v;
					}else{
						img_info->inter_local->nodes[index].v = img_info->rsz_local->nodes[aidx].v;
					}

					return;
				}			
			}

			aidx = index - 2;
			bidx = index - 1;
			
			//iBx / iAx = iCx / iBx
			//=>iCx = iBx * iBx / iAx
			nodes_tmp = img_info->inter_local->nodes[bidx].u * img_info->inter_local->nodes[bidx].u;
			nodes_tmp = nodes_tmp / img_info->inter_local->nodes[aidx].u;
			img_info->inter_local->nodes[index].u = nodes_tmp;

			//iBy / iAy = iCy / iBy
			//=>iCyx = iBy * iBy / iAy
			nodes_tmp = img_info->inter_local->nodes[bidx].v * img_info->inter_local->nodes[bidx].v;
			nodes_tmp = nodes_tmp / img_info->inter_local->nodes[aidx].v;
			img_info->inter_local->nodes[index].v = nodes_tmp;
			return;
		}else if(inter_coor_y > img_info->rsz_nodes_coor[idx].v){
			cidx = idx;
		}else if(inter_coor_y < img_info->rsz_nodes_coor[idx].v){
			
			aidx = 0;
			bidx = 0;
			for (x = 0; x < img_info->rsz_local->width; x++){
				idx = x + y * img_info->rsz_local->width;
				
				if(inter_coor_x == img_info->rsz_nodes_coor[idx].u){
					aidx = idx;

					cidx = aidx - img_info->rsz_local->width;					

					//(iCy - iAy) / (oCy - oAy) = (iACy - iAy) / (oACy - oAy)
					//=> iACy = (iCy - iAy) * (oACy - oAy) / (oCy - oAy) + iAy
					nodes_tmp = (img_info->rsz_local->nodes[cidx].v - img_info->rsz_local->nodes[aidx].v);
					nodes_tmp = nodes_tmp * (inter_coor_y - img_info->rsz_nodes_coor[aidx].v);
					nodes_tmp = nodes_tmp / (img_info->rsz_nodes_coor[cidx].v - img_info->rsz_nodes_coor[aidx].v );
					img_info->inter_local->nodes[index].v = nodes_tmp + img_info->rsz_local->nodes[aidx].v;

					//(iCx - iAx) / (iCy - iAy) = (iACx - iAx) / (iACy - iAy)
					//=> iACx = (iCx - iAx) * (iACy - iAy) / (iCy - iAy) + iAx
					nodes_tmp = (img_info->rsz_local->nodes[cidx].u - img_info->rsz_local->nodes[aidx].u);
					nodes_tmp = nodes_tmp * (img_info->inter_local->nodes[index].v - img_info->rsz_local->nodes[aidx].v);
					nodes_tmp = nodes_tmp / (img_info->rsz_local->nodes[cidx].v - img_info->rsz_local->nodes[aidx].v);
					img_info->inter_local->nodes[index].u = nodes_tmp + img_info->rsz_local->nodes[aidx].u;

					return;
				}else if(inter_coor_x > img_info->rsz_nodes_coor[idx].u){
					aidx = idx;
				}else if(inter_coor_x < img_info->rsz_nodes_coor[idx].u){
					bidx = idx;
					
					cidx = aidx - img_info->rsz_local->width;					
#if 0
					//(iBx - iAx) / (oBx - oAx) = (iABx - iAx) / (oABx - oAx)
					//=> iABx = (iBx - iAx) * (oABx - oAx) / (oBx - oAx) + iAx
					nodes_tmp = (img_info->rsz_local->nodes[bidx].u - img_info->rsz_local->nodes[aidx].u);
					nodes_tmp = nodes_tmp * (inter_coor_x - img_info->rsz_nodes_coor[aidx].u);
					nodes_tmp = nodes_tmp / (img_info->rsz_nodes_coor[bidx].u - img_info->rsz_nodes_coor[aidx].u);
					img_info->inter_local->nodes[index].u = nodes_tmp + img_info->rsz_local->nodes[aidx].u;

					//(iCy - iAy) / (oCy - oAy) = (iACy - iAy) / (oACy - oAy)
					//=> iACy = (iCy - iAy) * (oACy - oAy) / (oCy - oAy) + iAy
					nodes_tmp = (img_info->rsz_local->nodes[cidx].v - img_info->rsz_local->nodes[aidx].v);
					nodes_tmp = nodes_tmp * (inter_coor_y - img_info->rsz_nodes_coor[aidx].v);
					nodes_tmp = nodes_tmp / (img_info->rsz_nodes_coor[cidx].v - img_info->rsz_nodes_coor[aidx].v );
					img_info->inter_local->nodes[index].v = nodes_tmp + img_info->rsz_local->nodes[aidx].v;
#else

					//(iCy - iAy) / (oCy - oAy) = (iACy - iAy) / (oACy - oAy)
					//=> iACy = (iCy - iAy) * (oACy - oAy) / (oCy - oAy) + iAy
					nodes_tmp = (img_info->rsz_local->nodes[cidx].v - img_info->rsz_local->nodes[aidx].v);
					nodes_tmp = nodes_tmp * (inter_coor_y - img_info->rsz_nodes_coor[aidx].v);
					nodes_tmp = nodes_tmp / (img_info->rsz_nodes_coor[cidx].v - img_info->rsz_nodes_coor[aidx].v );
					img_info->inter_local->nodes[index].v = nodes_tmp + img_info->rsz_local->nodes[aidx].v;

					//(iCx - iAx) / (iCy - iAy) = (iACx - iAx) / (iACy - iAy)
					//=> iACx = (iCx - iAx) * (iACy - iAy) / (iCy - iAy) + iAx
					nodes_tmp = (img_info->rsz_local->nodes[cidx].u - img_info->rsz_local->nodes[aidx].u);
					nodes_tmp = nodes_tmp * (img_info->inter_local->nodes[index].v - img_info->rsz_local->nodes[aidx].v);
					nodes_tmp = nodes_tmp / (img_info->rsz_local->nodes[cidx].v - img_info->rsz_local->nodes[aidx].v);
					img_info->inter_local->nodes[index].u = nodes_tmp + img_info->rsz_local->nodes[aidx].u;
#endif
					return;
				}
			}

			aidx = index - 2;
			bidx = index - 1;
			
			//iBx / iAx = iCx / iBx
			//=>iCx = iBx * iBx / iAx
			nodes_tmp = img_info->inter_local->nodes[bidx].u * img_info->inter_local->nodes[bidx].u;
			nodes_tmp = nodes_tmp / img_info->inter_local->nodes[aidx].u;
			img_info->inter_local->nodes[index].u = nodes_tmp;

			//iBy / iAy = iCy / iBy
			//=>iCy = iBy * iBy / iAy
			nodes_tmp = img_info->inter_local->nodes[bidx].v * img_info->inter_local->nodes[bidx].v;
			nodes_tmp = nodes_tmp / img_info->inter_local->nodes[aidx].v;
			img_info->inter_local->nodes[index].v = nodes_tmp;
			return;
		}
	}	

	aidx = index - 2 * img_info->inter_local->width;;
	bidx = index - 1 * img_info->inter_local->width;;
			
	//iBx / iAx = iCx / iBx
	//=>iCx = iBx * iBx / iAx
	nodes_tmp = img_info->inter_local->nodes[bidx].u * img_info->inter_local->nodes[bidx].u;
	nodes_tmp = nodes_tmp / img_info->inter_local->nodes[aidx].u;
	img_info->inter_local->nodes[index].u = nodes_tmp;

	//iBy / iAy = iCy / iBy
	//=>iCy = iBy * iBy / iAy
	nodes_tmp = img_info->inter_local->nodes[bidx].v * img_info->inter_local->nodes[bidx].v;
	nodes_tmp = nodes_tmp / img_info->inter_local->nodes[aidx].v;
	img_info->inter_local->nodes[index].v = nodes_tmp;
	return;

}

static void ldc_inter_make_m2(T_LDC_IMG_INFO* img_info)
{
	INT32 x, y, index;
	USHORT interval_x, interval_y;
	
	img_info->inter_local->pitchX	= img_info->rsz_local->pitchX;
	img_info->inter_local->pitchY	= img_info->rsz_local->pitchY;

	interval_x = (USHORT)pow(2,img_info->inter_local->pitchX);
	interval_y = (USHORT)pow(2,img_info->inter_local->pitchY);

	if(img_info->out_img_h % interval_x)
		img_info->out_num_cell_h = img_info->out_img_h / interval_x + 1;
	else
		img_info->out_num_cell_h = img_info->out_img_h / interval_x;
		
	if(img_info->out_img_v % interval_y)
		img_info->out_num_cell_v = img_info->out_img_v / interval_y + 1;
	else
		img_info->out_num_cell_v = img_info->out_img_v / interval_y;

	img_info->inter_local->width	= img_info->out_num_cell_h + 1;
	img_info->inter_local->height	= img_info->out_num_cell_v + 1;
	img_info->inter_local->outputX	= 0;
	img_info->inter_local->outputY	= 0;
	img_info->inter_local->xform	= NULL;
	
    printf("LDC Inter Info(m2).\n");
    printf("[input image] width:%d height:%d\n",img_info->in_img_h, img_info->in_img_v);
    printf("[output image] width:%d height:%d\n",img_info->out_img_h, img_info->out_img_v);
    printf("[cell number] width:%d height:%d\n",img_info->out_num_cell_h, img_info->out_num_cell_v);
    printf("[pitch] X:%d(%d) Y:%d(%d)\n",img_info->inter_local->pitchX, interval_x, img_info->inter_local->pitchY, interval_y);
    printf("[nodes] width:%d height:%d\n",img_info->inter_local->width, img_info->inter_local->height);

	if((img_info->out_img_h > img_info->in_img_h) || (img_info->out_img_v > img_info->in_img_v)){
		for (y = 0; y < img_info->inter_local->height; y++) {
			for (x = 0; x < img_info->inter_local->width; x++) {
				index = x + y * img_info->inter_local->width;
				img_info->inter_nodes_coor[index].u = (FLOAT)interval_x * x;
				img_info->inter_nodes_coor[index].v = (FLOAT)interval_y * y;
				
				ldc_inter_calc_m2(img_info, index);
			}
		}	
	}else{
		for (y = 0; y < img_info->inter_local->height; y++) {
			for (x = 0; x < img_info->inter_local->width; x++) {
				index = x + y * img_info->inter_local->width;
				img_info->inter_nodes_coor[index].u = (FLOAT)interval_x * x;
				img_info->inter_nodes_coor[index].v = (FLOAT)interval_y * y;
				
				ldc_inter_calc_m2_1(img_info, index);
			}
		}	
	
		for (y = 0; y < img_info->inter_local->height; y++) {
			for (x = 0; x < img_info->inter_local->width; x++) {
				index = x + y * img_info->inter_local->width;
				if((img_info->inter_local->nodes[index].u == 0xFFFFFFFF) && (img_info->inter_local->nodes[index].v == 0xFFFFFFFF)){
					ldc_inter_calc_m2_2(img_info, index);
				}
			}
		}	
	}


}
#endif

#if 0
static void ldc_inter_calc_m1(T_LDC_IMG_INFO* img_info, INT32 index)
{
	INT32 x, y, idx;
	FLOAT inter_coor_x, inter_coor_y;
	INT32 aidx;

	inter_coor_x = img_info->inter_nodes_coor[index].u;
	inter_coor_y = img_info->inter_nodes_coor[index].v;
	for (y = 0; y < img_info->rsz_local->height; y++) {
		idx = y * img_info->rsz_local->width;
		
		if(inter_coor_y == img_info->rsz_nodes_coor[idx].v){
			for (x = 0; x < img_info->rsz_local->width; x++) {
				idx = x + y * img_info->rsz_local->width;
			
				if(inter_coor_x == img_info->rsz_nodes_coor[idx].u)
				{
					img_info->inter_local->nodes[index].u = img_info->rsz_local->nodes[idx].u;
					img_info->inter_local->nodes[index].v = img_info->rsz_local->nodes[idx].v;
					return;
				}
			}
		}else if(inter_coor_y > img_info->rsz_nodes_coor[idx].v){
			aidx = idx;
		}else if(inter_coor_y < img_info->rsz_nodes_coor[idx].v){
			for (x = 0; x < img_info->rsz_local->width; x++) {
				idx = x + y * img_info->rsz_local->width;
			
				if(inter_coor_x == img_info->rsz_nodes_coor[idx].u)
				{
					img_info->inter_local->nodes[index].u = img_info->rsz_local->nodes[idx].u;
					img_info->inter_local->nodes[index].v = img_info->rsz_local->nodes[idx].v;
					return;
				}
			}
		}
	}	
	
	if(index >= img_info->inter_local->width){
		aidx = index - img_info->inter_local->width;
	}else{
		aidx = index;
	}
	img_info->inter_local->nodes[index].u = img_info->inter_local->nodes[aidx].u;
	img_info->inter_local->nodes[index].v = img_info->inter_local->nodes[aidx].v;

}

static void ldc_inter_make_m1(T_LDC_IMG_INFO* img_info)
{
	INT32 x, y, index;
	USHORT interval_x, interval_y;
	

	img_info->inter_local->pitchX	= img_info->in_local_work.pitchX;
	img_info->inter_local->pitchY	= img_info->in_local_work.pitchY;

	interval_x = (USHORT)pow(2,img_info->inter_local->pitchX);
	interval_y = (USHORT)pow(2,img_info->inter_local->pitchY);

	if(img_info->out_img_h % interval_x)
		img_info->out_num_cell_h = img_info->out_img_h / interval_x + 1;
	else
		img_info->out_num_cell_h = img_info->out_img_h / interval_x;
		
	if(img_info->out_img_v % interval_y)
		img_info->out_num_cell_v = img_info->out_img_v / interval_y + 1;
	else
		img_info->out_num_cell_v = img_info->out_img_v / interval_y;

	img_info->inter_local->width	= img_info->out_num_cell_h + 1;
	img_info->inter_local->height	= img_info->out_num_cell_v + 1;
	img_info->inter_local->outputX	= 0;
	img_info->inter_local->outputY	= 0;
	img_info->inter_local->xform	= NULL;
	
    printf("LDC Inter Info(m1).\n");
    printf("[input image] width:%d height:%d\n",img_info->in_img_h, img_info->in_img_v);
    printf("[output image] width:%d height:%d\n",img_info->out_img_h, img_info->out_img_v);
    printf("[cell number] width:%d height:%d\n",img_info->out_num_cell_h, img_info->out_num_cell_v);
    printf("[pitch] X:%d(%d) Y:%d(%d)\n",img_info->inter_local->pitchX, interval_x, img_info->inter_local->pitchY, interval_y);
    printf("[nodes] width:%d height:%d\n",img_info->inter_local->width, img_info->inter_local->height);

	for (y = 0; y < img_info->inter_local->height; y++) {
		for (x = 0; x < img_info->inter_local->width; x++) {
			index = x + y * img_info->inter_local->width;
			img_info->inter_nodes_coor[index].u = (FLOAT)interval_x * x;
			img_info->inter_nodes_coor[index].v = (FLOAT)interval_y * y;
			
			ldc_inter_calc_m1(img_info, index);
		}
	}	
}
#endif

static void ldc_inter_calc_zoom(T_LDC_IMG_INFO* img_info, INT32 index, INT32 zoom_idx, DOUBLE *nodes_x, DOUBLE *nodes_y)
{
	#if 1
	{
		INT32 aidx, bidx, cidx, didx;
		FLOAT rate[4], rsz_coor_x, rsz_coor_y, nodes_tmp;
   		T_IP_GE_LNODE ref[2];	
		INT32 in_max_index = img_info->in_local_work.width * img_info->in_local_work.height;

		cidx = zoom_idx;
		didx = cidx + 1;
		aidx = cidx + img_info->in_local_work.width;
		bidx = aidx + 1;

		if((aidx < in_max_index) && (bidx < in_max_index) && (cidx < in_max_index) && (bidx < in_max_index)){
			// [index] all in
			if((img_info->in_work_nodes_coor[cidx].v == img_info->in_work_nodes_coor[didx].v) &&
			   (img_info->in_work_nodes_coor[aidx].u == img_info->in_work_nodes_coor[cidx].u)){
				// [coor] all in
				rsz_coor_x = img_info->in_work_nodes_coor[didx].u - img_info->in_work_nodes_coor[cidx].u;
				rsz_coor_x = rsz_coor_x * (img_info->zoom_s_x - (USHORT)img_info->zoom_s_x) + img_info->in_work_nodes_coor[cidx].u;
				rsz_coor_y = img_info->in_work_nodes_coor[aidx].v - img_info->in_work_nodes_coor[cidx].v;
				rsz_coor_y = rsz_coor_y * (img_info->zoom_s_y - (USHORT)img_info->zoom_s_y) + img_info->in_work_nodes_coor[cidx].v;
				// C(x,y)------------D(x,y)
				// ---------P(x,y)---------
				// A(x,y)------------B(x,y)
				// Rate0 = (oBx - oPx)/(oBx - oAx)
				// Rate1 = (oPx - oAx)/(oBx - oAx)
				// Rate2 = (oCy - oPy)/(oCy - oAy)
				// Rate3 = (oPy - oAy)/(oCy - oAy)
				nodes_tmp = img_info->in_work_nodes_coor[bidx].u - rsz_coor_x;
				rate[0] = nodes_tmp / (img_info->in_work_nodes_coor[bidx].u - img_info->in_work_nodes_coor[aidx].u);
				nodes_tmp = rsz_coor_x - img_info->in_work_nodes_coor[aidx].u;
				rate[1] = nodes_tmp / (img_info->in_work_nodes_coor[bidx].u - img_info->in_work_nodes_coor[aidx].u);
				nodes_tmp = img_info->in_work_nodes_coor[cidx].v - rsz_coor_y;
				rate[2] = nodes_tmp / (img_info->in_work_nodes_coor[cidx].v - img_info->in_work_nodes_coor[aidx].v);
				nodes_tmp = rsz_coor_y - img_info->in_work_nodes_coor[aidx].v;
				rate[3] = nodes_tmp / (img_info->in_work_nodes_coor[cidx].v - img_info->in_work_nodes_coor[aidx].v);
				// Ref0(x,y) = Rate0 * iA(x,y) + Rate1 * iB(x,y)
				nodes_tmp = rate[0] * img_info->in_work_nodes[aidx].u;
				ref[0].u = nodes_tmp + rate[1] * img_info->in_work_nodes[bidx].u;
				nodes_tmp = rate[0] * img_info->in_work_nodes[aidx].v;
				ref[0].v = nodes_tmp + rate[1] * img_info->in_work_nodes[bidx].v;
				// Ref1(x,y) = Rate0 * iC(x,y) + Rate1 * iD(x,y)
				nodes_tmp = rate[0] * img_info->in_work_nodes[cidx].u;
				ref[1].u = nodes_tmp + rate[1] * img_info->in_work_nodes[didx].u;
				nodes_tmp = rate[0] * img_info->in_work_nodes[cidx].v;
				ref[1].v = nodes_tmp + rate[1] * img_info->in_work_nodes[didx].v;
				// iP(x,y) = Rate2 * Ref0(x,y) + Rate3 * Ref1(x,y)
				*nodes_x = (DOUBLE)(rate[2] * ref[0].u + rate[3] * ref[1].u) * img_info->out_img_h;
				*nodes_y = (DOUBLE)(rate[2] * ref[0].v + rate[3] * ref[1].v) * img_info->out_img_v;
				return;
			}
			else if(img_info->in_work_nodes_coor[cidx].v == img_info->in_work_nodes_coor[didx].v){
				// [coor] a b out
				cidx = index - 2 * img_info->rsz_local->width;
				aidx = index - 1 * img_info->rsz_local->width;
				
				rsz_coor_x = img_info->rsz_nodes_coor[index].u;
				rsz_coor_y = img_info->rsz_nodes_coor[index].v;
				// C(x,y)---A(x,y)---P(x,y)
				// Rate0 = (oPy - oAy)/(oPy - oCy)
				// Rate1 = (oAy - oCy)/(oPy - oCy)
				nodes_tmp = rsz_coor_y - img_info->rsz_nodes_coor[aidx].v;
				rate[0] = nodes_tmp / (rsz_coor_y - img_info->rsz_nodes_coor[cidx].v);
				nodes_tmp = img_info->rsz_nodes_coor[aidx].v - img_info->rsz_nodes_coor[cidx].v;
				rate[1] = nodes_tmp / (rsz_coor_y - img_info->rsz_nodes_coor[cidx].v);
				// iP(x,y) = [iA(x,y) - Rate0 * iC(x,y)] / Rate1
				nodes_tmp = img_info->rsz_local->nodes[aidx].u - rate[0] * img_info->rsz_local->nodes[cidx].u;
				*nodes_x = nodes_tmp / rate[1] * img_info->in_img_h;
				nodes_tmp = img_info->rsz_local->nodes[aidx].v - rate[0] * img_info->rsz_local->nodes[cidx].v;
				*nodes_y = nodes_tmp / rate[1] * img_info->in_img_v;
				return;
			}
			else if(img_info->in_work_nodes_coor[aidx].u == img_info->in_work_nodes_coor[cidx].u){
				// [coor] b d out
				cidx = index - 2;
				didx = index - 1;
				
				rsz_coor_x = img_info->rsz_nodes_coor[index].u;
				rsz_coor_y = img_info->rsz_nodes_coor[index].v;
				// C(x,y)---D(x,y)---P(x,y)
				// Rate0 = (oPx - oDx)/(oPx - oCx)
				// Rate1 = (oDx - oCx)/(oPx - oCx)
				nodes_tmp = rsz_coor_x - img_info->rsz_nodes_coor[didx].u;
				rate[0] = nodes_tmp / (rsz_coor_x - img_info->rsz_nodes_coor[cidx].u);
				nodes_tmp = img_info->rsz_nodes_coor[didx].u - img_info->rsz_nodes_coor[cidx].u;
				rate[1] = nodes_tmp / (rsz_coor_x - img_info->rsz_nodes_coor[cidx].u);
				// iP(x,y) = [iD(x,y) - Rate0 * iC(x,y)] / Rate1
				nodes_tmp = img_info->rsz_local->nodes[didx].u - rate[0] * img_info->rsz_local->nodes[cidx].u;
				*nodes_x = nodes_tmp / rate[1] * img_info->in_img_h;
				nodes_tmp = img_info->rsz_local->nodes[didx].v - rate[0] * img_info->rsz_local->nodes[cidx].v;
				*nodes_y = nodes_tmp / rate[1] * img_info->in_img_v;
				return;
			}
			printf("!!ERROR!![%d] [coor]unknow a(%d, %5.3f, %5.3f) b(%d, %5.3f, %5.3f) c(%d, %5.3f, %5.3f) d(%d, %5.3f, %5.3f) \n"
				, index
				, aidx, img_info->in_work_nodes_coor[aidx].u, img_info->in_work_nodes_coor[aidx].v
				, bidx, img_info->in_work_nodes_coor[bidx].u, img_info->in_work_nodes_coor[bidx].v
				, cidx, img_info->in_work_nodes_coor[cidx].u, img_info->in_work_nodes_coor[cidx].v
				, didx, img_info->in_work_nodes_coor[didx].u, img_info->in_work_nodes_coor[didx].v);
			*nodes_x = 0;
			*nodes_y = 0;
			return;
		}
		else if((aidx < in_max_index) && (cidx < in_max_index)){
			// [index] b d out
			cidx = index - 2;
			didx = index - 1;
			
			rsz_coor_x = img_info->rsz_nodes_coor[index].u;
			rsz_coor_y = img_info->rsz_nodes_coor[index].v;
			// C(x,y)---D(x,y)---P(x,y)
			// Rate0 = (oPx - oDx)/(oPx - oCx)
			// Rate1 = (oDx - oCx)/(oPx - oCx)
			nodes_tmp = rsz_coor_x - img_info->rsz_nodes_coor[didx].u;
			rate[0] = nodes_tmp / (rsz_coor_x - img_info->rsz_nodes_coor[cidx].u);
			nodes_tmp = img_info->rsz_nodes_coor[didx].u - img_info->rsz_nodes_coor[cidx].u;
			rate[1] = nodes_tmp / (rsz_coor_x - img_info->rsz_nodes_coor[cidx].u);
			// iP(x,y) = [iD(x,y) - Rate0 * iC(x,y)] / Rate1
			nodes_tmp = img_info->rsz_local->nodes[didx].u - rate[0] * img_info->rsz_local->nodes[cidx].u;
			*nodes_x = nodes_tmp / rate[1] * img_info->in_img_h;
			nodes_tmp = img_info->rsz_local->nodes[didx].v - rate[0] * img_info->rsz_local->nodes[cidx].v;
			*nodes_y = nodes_tmp / rate[1] * img_info->in_img_v;
			return;
		}
		else if((cidx < in_max_index) && (didx < in_max_index)){
			// [index] a b out
			cidx = index - 2 * img_info->rsz_local->width;
			aidx = index - 1 * img_info->rsz_local->width;
			
			rsz_coor_x = img_info->rsz_nodes_coor[index].u;
			rsz_coor_y = img_info->rsz_nodes_coor[index].v;
			// C(x,y)---A(x,y)---P(x,y)
			// Rate0 = (oPy - oAy)/(oPy - oCy)
			// Rate1 = (oAy - oCy)/(oPy - oCy)
			nodes_tmp = rsz_coor_y - img_info->rsz_nodes_coor[aidx].v;
			rate[0] = nodes_tmp / (rsz_coor_y - img_info->rsz_nodes_coor[cidx].v);
			nodes_tmp = img_info->rsz_nodes_coor[aidx].v - img_info->rsz_nodes_coor[cidx].v;
			rate[1] = nodes_tmp / (rsz_coor_y - img_info->rsz_nodes_coor[cidx].v);
			// iP(x,y) = [iA(x,y) - Rate0 * iC(x,y)] / Rate1
			nodes_tmp = img_info->rsz_local->nodes[aidx].u - rate[0] * img_info->rsz_local->nodes[cidx].u;
			*nodes_x = nodes_tmp / rate[1] * img_info->in_img_h;
			nodes_tmp = img_info->rsz_local->nodes[aidx].v - rate[0] * img_info->rsz_local->nodes[cidx].v;
			*nodes_y = nodes_tmp / rate[1] * img_info->in_img_v;
			return;
		}
		else if(cidx < in_max_index){
			// [index] a b d out
			cidx = index - 2 * img_info->rsz_local->width;
			aidx = index - 1 * img_info->rsz_local->width;
			
			rsz_coor_x = img_info->rsz_nodes_coor[index].u;
			rsz_coor_y = img_info->rsz_nodes_coor[index].v;
			// C(x,y)---A(x,y)---P(x,y)
			// Rate0 = (oPy - oAy)/(oPy - oCy)
			// Rate1 = (oAy - oCy)/(oPy - oCy)
			nodes_tmp = rsz_coor_y - img_info->rsz_nodes_coor[aidx].v;
			rate[0] = nodes_tmp / (rsz_coor_y - img_info->rsz_nodes_coor[cidx].v);
			nodes_tmp = img_info->rsz_nodes_coor[aidx].v - img_info->rsz_nodes_coor[cidx].v;
			rate[1] = nodes_tmp / (rsz_coor_y - img_info->rsz_nodes_coor[cidx].v);
			// iP(x,y) = [iA(x,y) - Rate0 * iC(x,y)] / Rate1
			nodes_tmp = img_info->rsz_local->nodes[aidx].u - rate[0] * img_info->rsz_local->nodes[cidx].u;
			*nodes_x = nodes_tmp / rate[1] * img_info->in_img_h;
			nodes_tmp = img_info->rsz_local->nodes[aidx].v - rate[0] * img_info->rsz_local->nodes[cidx].v;
			*nodes_y = nodes_tmp / rate[1] * img_info->in_img_v;
			return;
		}
		printf("!!ERROR!![%d] [index]unknow a(%d) b(%d) c(%d) d(%d) \n", index, aidx, bidx, cidx, didx);
		*nodes_x = 0;
		*nodes_y = 0;
		return;
	}
	#else
	{
		INT32 aidx, bidx, cidx, didx;
		FLOAT rate[4], rsz_coor_x, rsz_coor_y, nodes_tmp;
   		T_IP_GE_LNODE ref[2];	

		cidx = zoom_idx;
		didx = cidx + 1;
		aidx = cidx + img_info->in_local_work.width;
		bidx = aidx + 1;
		rsz_coor_x = img_info->in_work_nodes_coor[didx].u - img_info->in_work_nodes_coor[cidx].u;
		rsz_coor_x = rsz_coor_x * (img_info->zoom_s_x - (USHORT)img_info->zoom_s_x) + img_info->in_work_nodes_coor[cidx].u;
		rsz_coor_y = img_info->in_work_nodes_coor[aidx].v - img_info->in_work_nodes_coor[cidx].v;
		rsz_coor_y = rsz_coor_y * (img_info->zoom_s_y - (USHORT)img_info->zoom_s_y) + img_info->in_work_nodes_coor[cidx].v;
		// C(x,y)------------D(x,y)
		// ---------P(x,y)---------
		// A(x,y)------------B(x,y)
		// Rate0 = (oBx - oPx)/(oBx - oAx)
		// Rate1 = (oPx - oAx)/(oBx - oAx)
		// Rate2 = (oCy - oPy)/(oCy - oAy)
		// Rate3 = (oPy - oAy)/(oCy - oAy)
		nodes_tmp = img_info->in_work_nodes_coor[bidx].u - rsz_coor_x;
		rate[0] = nodes_tmp / (img_info->in_work_nodes_coor[bidx].u - img_info->in_work_nodes_coor[aidx].u);
		nodes_tmp = rsz_coor_x - img_info->in_work_nodes_coor[aidx].u;
		rate[1] = nodes_tmp / (img_info->in_work_nodes_coor[bidx].u - img_info->in_work_nodes_coor[aidx].u);
		nodes_tmp = img_info->in_work_nodes_coor[cidx].v - rsz_coor_y;
		rate[2] = nodes_tmp / (img_info->in_work_nodes_coor[cidx].v - img_info->in_work_nodes_coor[aidx].v);
		nodes_tmp = rsz_coor_y - img_info->in_work_nodes_coor[aidx].v;
		rate[3] = nodes_tmp / (img_info->in_work_nodes_coor[cidx].v - img_info->in_work_nodes_coor[aidx].v);
		// Ref0(x,y) = Rate0 * iA(x,y) + Rate1 * iB(x,y)
		nodes_tmp = rate[0] * img_info->in_work_nodes[aidx].u;
		ref[0].u = nodes_tmp + rate[1] * img_info->in_work_nodes[bidx].u;
		nodes_tmp = rate[0] * img_info->in_work_nodes[aidx].v;
		ref[0].v = nodes_tmp + rate[1] * img_info->in_work_nodes[bidx].v;
		// Ref1(x,y) = Rate0 * iC(x,y) + Rate1 * iD(x,y)
		nodes_tmp = rate[0] * img_info->in_work_nodes[cidx].u;
		ref[1].u = nodes_tmp + rate[1] * img_info->in_work_nodes[didx].u;
		nodes_tmp = rate[0] * img_info->in_work_nodes[cidx].v;
		ref[1].v = nodes_tmp + rate[1] * img_info->in_work_nodes[didx].v;
		// iP(x,y) = Rate2 * Ref0(x,y) + Rate3 * Ref1(x,y)
		*nodes_x = (DOUBLE)(rate[2] * ref[0].u + rate[3] * ref[1].u) * img_info->out_img_h;
		*nodes_y = (DOUBLE)(rate[2] * ref[0].v + rate[3] * ref[1].v) * img_info->out_img_v;
	}
	#endif
}

static void ldc_inter_proc(T_LDC_IMG_INFO* img_info)
{
	INT32 x, y, index;
	DOUBLE nodes_x, nodes_y;
	USHORT interval_x, interval_y;
	FLOAT zoom_s_x, zoom_s_y;
	
	#if 0
	y=4;
	for(x= 2; x<=6; x++)
	{
		if(y >= (img_info->out_img_h / img_info->in_num_cell_h))
		{
			img_info->rsz_local->pitchX = x;
			break;
		}
		y = y * 2;
	}

	if(x > 6) img_info->rsz_local->pitchX = 6;

	y=4;
	for(x= 2; x<=5; x++)
	{
		if(y >= (img_info->out_img_v / img_info->in_num_cell_v))
		{
			img_info->rsz_local->pitchY = x;
			break;
		}
		y = y * 2;
	}

	if(x > 5) img_info->rsz_local->pitchY = 5;
	#else
	// recommand to use 64x32 cell size to save the bandwidth
	img_info->rsz_local->pitchX = img_info->in_local_work.pitchX;
	img_info->rsz_local->pitchY = img_info->in_local_work.pitchY;
	#endif
	{
		INT32 zoom_idx;
		USHORT zoom_width, zoom_height;
		
		interval_x = (USHORT)pow(2,img_info->rsz_local->pitchX);
		interval_y = (USHORT)pow(2,img_info->rsz_local->pitchY);
	
		zoom_width = img_info->in_img_h / img_info->zoom_ratio;
		zoom_height = img_info->in_img_v / img_info->zoom_ratio;
	
		if(zoom_width % interval_x)
			img_info->out_num_cell_h = zoom_width / interval_x + 1;
		else
			img_info->out_num_cell_h = zoom_width / interval_x;
			
		if(zoom_height % interval_y)
			img_info->out_num_cell_v = zoom_height / interval_y + 1;
		else
			img_info->out_num_cell_v = zoom_height / interval_y;
	
		img_info->rsz_local->width	= img_info->out_num_cell_h + 1;
		img_info->rsz_local->height = img_info->out_num_cell_v + 1;
		img_info->rsz_local->outputX	= 0;
		img_info->rsz_local->outputY	= 0;
		img_info->rsz_local->xform	= NULL;

		zoom_s_x = (FLOAT)(img_info->in_img_h - zoom_width) * img_info->zoom_pos_x;
		zoom_s_x = zoom_s_x / interval_x;
		zoom_s_y = (FLOAT)(img_info->in_img_v - zoom_height) * img_info->zoom_pos_y;
		zoom_s_y = zoom_s_y / interval_y;

		img_info->zoom_s_x = zoom_s_x;
		img_info->zoom_s_y = zoom_s_y;
		
		printf("LDC Resize(Zoom:x%f) Info.\n", img_info->zoom_ratio);
		printf("[input image] width:%d height:%d\n",img_info->in_img_h, img_info->in_img_v);
		printf("[output image] width:%d height:%d\n",img_info->out_img_h, img_info->out_img_v);
		printf("[cell number] width:%d height:%d\n",img_info->in_num_cell_h, img_info->in_num_cell_v);
		printf("[pitch] X:%d(%d) Y:%d(%d)\n",img_info->rsz_local->pitchX, interval_x, img_info->rsz_local->pitchY, interval_y);
		printf("[nodes] width:%d height:%d\n",img_info->rsz_local->width, img_info->rsz_local->height);
		printf("[zoom] start x:%f start y:%f\n",zoom_s_x, zoom_s_y);
	
		for (y = 0; y < img_info->rsz_local->height; y++) {
			for (x = 0; x < img_info->rsz_local->width; x++) {
				index = y * img_info->rsz_local->width + x;
				zoom_idx = (y + (USHORT)zoom_s_y) * img_info->in_local_work.width + (x + (USHORT)zoom_s_x);
	
				if(img_info->zoom_ratio > 1.0){
					nodes_x = (DOUBLE)interval_x * x * img_info->zoom_ratio * img_info->out_img_h;
					nodes_y = (DOUBLE)interval_y * y * img_info->zoom_ratio * img_info->out_img_v;
				}else{
					nodes_x = (DOUBLE)img_info->in_work_nodes_coor[zoom_idx].u * img_info->out_img_h;
					nodes_y = (DOUBLE)img_info->in_work_nodes_coor[zoom_idx].v * img_info->out_img_v;
				}
				img_info->rsz_nodes_coor[index].u = nodes_x / img_info->in_img_h;
				img_info->rsz_nodes_coor[index].v = nodes_y / img_info->in_img_v;

				if((zoom_s_x != (USHORT)zoom_s_x) || (zoom_s_y != (USHORT)zoom_s_y)){
					ldc_inter_calc_zoom(img_info, index, zoom_idx, &nodes_x, &nodes_y);
				}else{
					nodes_x = (DOUBLE)img_info->in_work_nodes[zoom_idx].u * img_info->out_img_h;
					nodes_y = (DOUBLE)img_info->in_work_nodes[zoom_idx].v * img_info->out_img_v;
				}
				img_info->rsz_local->nodes[index].u =  nodes_x / img_info->in_img_h;
				img_info->rsz_local->nodes[index].v =  nodes_y / img_info->in_img_v;
			}
		}
	}

	ldc_inter_make_m3(img_info);
}

VOID LDC_Calc_Task( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn = 0;
	INT32			ercd;

	for (;;) {
		ercd = OS_User_Twai_Flg( FID_LDC_CALC, FLG_LDC_CALC_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		UPRINTF_ERR(ercd);
		ercd = OS_User_Clr_Flg( FID_LDC_CALC, ~FLG_LDC_CALC_WAITPTN );
		UPRINTF_ERR(ercd);
		
		if (IS_FLGPTN_EQUAL( flgptn, FLG_LDC_CALC_START)) {
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "ldc_calc_as"));
			ldc_inter_proc(&g_img_info);
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "ldc_calc_as"));
			ercd = OS_User_Set_Flg( FID_LDC_CALC_DONE, FLG_LDC_CALC_DONE );
			UPRINTF_ERR(ercd);
		}
		
		if (IS_FLGPTN_EQUAL( flgptn, FLG_LDC_CALC_END)) {
			break;
		}
	}
	OS_User_Ext_Tsk();
}
