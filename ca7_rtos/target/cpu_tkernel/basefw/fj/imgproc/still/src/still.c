/**
 * @file		still.c
 * @brief		capture operator
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "still.h"
#include "still_iip.h"
// delete\basefw\fj\imgproc\still
//#include "still_sync_ctrl.h"
#include "still_attr.h"
#include "still_dcache.h"

#include "still_attr.h"
#include "osd_shape_comm.h"

#include "sdram_map_common.h"
#include "sdram_map_cache_define.h"

#include "fj_std.h"
#include "fj_host.h"

#include "bf_iip_resource.h"

#include "mba_if.h"

#include "debug.h"

#include "im_xch.h"
#include "im_iip.h"
#include "im_iip_param.h"
#include "dd_arm.h"
#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CO_STILL_DEBUG_ON

// IIP resource define of BF_Still_Resize_Image()
#define D_BF_STILL_RESCONV_UNIT_PARAM_MASTER_IF	D_IM_IIP_MASTER_IF0

#define D_BF_STILL_IIP_CSC_MDSEL				(1)
#define D_BF_STILL_IIP_CSCK_X1_0				(0x1000L)		// when MDSEL=1

#define BF_Still_RoundUp_Bit_64(a)	(((a) + 63) & ~0x3F)	// 64bit Roundup
#define D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK		(1024)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// nothing special.


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

// --- REMOVE_DCF_EXIF BEGIN ---
// Exif information for Capture, Panorama
volatile T_EXIF_INFO_SET gExif_Info_Set[MAX_OF_CONTINUOUS];
// --- REMOVE_DCF_EXIF END ---

volatile UINT32 g_still_spf_unit_dump_tid = 0xFFFF;


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

#define M_BF_STILL_CALC_IIP_CSCK( f_val )				((LONG)((f_val) * D_BF_STILL_IIP_CSCK_X1_0))


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#if 0	// ### begin MILB_Porting
static VOID bf_still_image_draw_face_frame_convert_pos( USHORT pos_x_src, USHORT pos_y_src, USHORT gl_width, ULONG* offset_addr )
{
	USHORT line_block_times;
	USHORT column_block_times;
	USHORT line_remainder;
	USHORT column_remainder;

	line_block_times = pos_y_src >> 4;
	line_remainder = pos_y_src & 15;
	column_block_times = pos_x_src >> 6;
	column_remainder = pos_x_src & 63;

	*offset_addr = line_block_times * gl_width * 16 + D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK * column_block_times + (line_remainder << 6) + column_remainder;
	
	return;
}

static INT32 bf_still_image_draw_one_face_frame_videofmt( FJ_IMG_INFO* src_frame, const T_FACE_FRAME_EACH_SET* face_pos_array )
{
	USHORT i;
	USHORT loop_x;
	USHORT loop_y;
	ULONG col_block_total;
	USHORT gl_width_64;
	ULONG line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_END];
	ULONG line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_END];
	ULONG offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_END];
	ULONG offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_END];
	T_BF_OSD_SHAPE_COMM_SHAPE_INFO rect_data;
	T_POS frame_theckness;
	T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_END];
	USHORT copy_size;
	CHAR cc_string[65];
	USHORT current_pos_x;
	USHORT current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_END];
	USHORT block_start_x;
	USHORT block_end_x;

	rect_data.Rect.Postion.StartX = face_pos_array->start_pos.Position.StartX;
	rect_data.Rect.Postion.StartY = face_pos_array->start_pos.Position.StartY;
	rect_data.Rect.Width = face_pos_array->start_pos.Width;
	rect_data.Rect.Height = face_pos_array->start_pos.Height;
	frame_theckness.StartX = face_pos_array->v_thickness;
	frame_theckness.StartY = face_pos_array->h_thickness;
	
	BF_Osd_Shape_Comm_Divide_Rect_5_Parts(&rect_data, &frame_theckness, display_data);

	gl_width_64 = BF_Still_RoundUp_Bit_64(src_frame->g_y_width);
	col_block_total = gl_width_64 << 4;

	for (loop_x = 0; loop_x < 64; loop_x += 2) {
		cc_string[loop_x] = face_pos_array->rgba.AYCC.Cb;
		cc_string[loop_x + 1] = face_pos_array->rgba.AYCC.Cr;
	}

	//top and bottom line
	i = E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP;

	bf_still_image_draw_face_frame_convert_pos(display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].rect_data.Rect.Postion.StartX, display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].rect_data.Rect.Postion.StartY, gl_width_64, &(line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP]));
	bf_still_image_draw_face_frame_convert_pos(display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].rect_data.Rect.Postion.StartX, (display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].rect_data.Rect.Postion.StartY >> 1), gl_width_64, &(line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP]));
	bf_still_image_draw_face_frame_convert_pos(display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].rect_data.Rect.Postion.StartX, display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].rect_data.Rect.Postion.StartY, gl_width_64, &(line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM]));
	bf_still_image_draw_face_frame_convert_pos(display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].rect_data.Rect.Postion.StartX, (display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].rect_data.Rect.Postion.StartY >> 1), gl_width_64, &(line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM]));

	for (loop_y = 0; loop_y < display_data[i].rect_data.Rect.Height; loop_y++) {
		current_pos_x = display_data[i].rect_data.Rect.Postion.StartX;
		current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] = display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].rect_data.Rect.Postion.StartY + loop_y;
		current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] = display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].rect_data.Rect.Postion.StartY + loop_y;

		if ((loop_y & 1) == 0) {
			if (loop_y != 0) {
				if ((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] & 15) == 0) {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] +=  col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] += 64;
				}
				
				if (((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] >> 1) & 15) == 0) {
					line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] +=  col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] += 64;
				}
				
				if ((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] & 15) == 0) {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] +=  col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] += 64;
				}
				
				if (((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] >> 1) & 15) == 0) {
					line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] +=  col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] += 64;
				}
			}

			offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] = line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP];
			offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] = line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP];
			offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] = line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM];
			offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] = line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM];
			
			for (;;) {
				block_start_x = (current_pos_x & ~0x3F);
				block_end_x = block_start_x + 64;
				
				if ((block_end_x - display_data[i].rect_data.Rect.Postion.StartX) >= display_data[i].rect_data.Rect.Width) {
					copy_size = display_data[i].rect_data.Rect.Postion.StartX + display_data[i].rect_data.Rect.Width - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP]), face_pos_array->rgba.AYCC.Y, copy_size);
					memcpy((CHAR*)(src_frame->addr.AddressCb + offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP]), cc_string, copy_size);
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM]), face_pos_array->rgba.AYCC.Y, copy_size);
					memcpy((CHAR*)(src_frame->addr.AddressCb + offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM]), cc_string, copy_size);
					break;
				}
				else {
					copy_size = block_end_x - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP]), face_pos_array->rgba.AYCC.Y, copy_size);
					memcpy((CHAR*)(src_frame->addr.AddressCb + offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP]), cc_string, copy_size);
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM]), face_pos_array->rgba.AYCC.Y, copy_size);
					memcpy((CHAR*)(src_frame->addr.AddressCb + offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM]), cc_string, copy_size);
				}
				
				offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				current_pos_x += copy_size;
			}
		}
		else {
			if (loop_y != 0) {
				if ((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] & 15) == 0) {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] +=  col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] += 64;
				}
				
				if ((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] & 15) == 0) {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] +=  col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] += 64;
				}
			}

			offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] = line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP];
			offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] = line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM];
			
			for (;;) {
				block_start_x = (current_pos_x & ~0x3F);
				block_end_x = block_start_x + 64;
				
				if ((block_end_x - display_data[i].rect_data.Rect.Postion.StartX) >= display_data[i].rect_data.Rect.Width) {
					copy_size = display_data[i].rect_data.Rect.Postion.StartX + display_data[i].rect_data.Rect.Width - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP]), face_pos_array->rgba.AYCC.Y, copy_size);
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM]), face_pos_array->rgba.AYCC.Y, copy_size);
					break;
				}
				else {
					copy_size = block_end_x - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP]), face_pos_array->rgba.AYCC.Y, copy_size);
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM]), face_pos_array->rgba.AYCC.Y, copy_size);
				}
				
				offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				current_pos_x += copy_size;
			}
		}
	}

	//left line
	i = E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT;

	bf_still_image_draw_face_frame_convert_pos(display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect.Postion.StartX, display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect.Postion.StartY, gl_width_64, &(line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT]));
	bf_still_image_draw_face_frame_convert_pos(display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect.Postion.StartX, (display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect.Postion.StartY >> 1), gl_width_64, &(line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT]));

	for (loop_y = 0; loop_y < display_data[i].rect_data.Rect.Height; loop_y++) {
		current_pos_x = display_data[i].rect_data.Rect.Postion.StartX;
		current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] = display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect.Postion.StartY + loop_y;

		if ((loop_y & 1) == 0) {
			if (loop_y != 0) {
				if ((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] & 15) == 0) {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] +=	col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] += 64;
				}
				
				if (((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] >> 1) & 15) == 0) {
					line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] +=	col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] += 64;
				}
			}

			offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] = line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT];
			offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] = line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT];
			
			for (;;) {
				block_start_x = (current_pos_x & ~0x3F);
				block_end_x = block_start_x + 64;
				
				if ((block_end_x - display_data[i].rect_data.Rect.Postion.StartX) >= display_data[i].rect_data.Rect.Width) {
					copy_size = display_data[i].rect_data.Rect.Postion.StartX + display_data[i].rect_data.Rect.Width - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT]), face_pos_array->rgba.AYCC.Y, copy_size);
					memcpy((CHAR*)(src_frame->addr.AddressCb + offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT]), cc_string, copy_size);
					break;
				}
				else {
					copy_size = block_end_x - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT]), face_pos_array->rgba.AYCC.Y, copy_size);
					memcpy((CHAR*)(src_frame->addr.AddressCb + offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT]), cc_string, copy_size);
				}
				
				offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				current_pos_x += copy_size;
			}
		}
		else {
			if (loop_y != 0) {
				if ((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] & 15) == 0) {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] +=	col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] += 64;
				}
			}

			offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] = line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT];
			
			for (;;) {
				block_start_x = (current_pos_x & ~0x3F);
				block_end_x = block_start_x + 64;
				
				if ((block_end_x - display_data[i].rect_data.Rect.Postion.StartX) >= display_data[i].rect_data.Rect.Width) {
					copy_size = display_data[i].rect_data.Rect.Postion.StartX + display_data[i].rect_data.Rect.Width - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT]), face_pos_array->rgba.AYCC.Y, copy_size);
					break;
				}
				else {
					copy_size = block_end_x - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT]), face_pos_array->rgba.AYCC.Y, copy_size);
				}
				
				offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				current_pos_x += copy_size;
			}
		}
	}

	//right line
	i = E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT;

	bf_still_image_draw_face_frame_convert_pos(display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Postion.StartX, display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Postion.StartY, gl_width_64, &(line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT]));
	bf_still_image_draw_face_frame_convert_pos(display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Postion.StartX, (display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Postion.StartY >> 1), gl_width_64, &(line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT]));

	for (loop_y = 0; loop_y < display_data[i].rect_data.Rect.Height; loop_y++) {
		current_pos_x = display_data[i].rect_data.Rect.Postion.StartX;
		current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] = display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Postion.StartY + loop_y;

		if ((loop_y & 1) == 0) {
			if (loop_y != 0) {
				if ((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] & 15) == 0) {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] +=	col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] += 64;
				}
				
				if (((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] >> 1) & 15) == 0) {
					line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] +=	col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] += 64;
				}
			}

			offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] = line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT];
			offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] = line_start_offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT];
			
			for (;;) {
				block_start_x = (current_pos_x & ~0x3F);
				block_end_x = block_start_x + 64;
				
				if ((block_end_x - display_data[i].rect_data.Rect.Postion.StartX) >= display_data[i].rect_data.Rect.Width) {
					copy_size = display_data[i].rect_data.Rect.Postion.StartX + display_data[i].rect_data.Rect.Width - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT]), face_pos_array->rgba.AYCC.Y, copy_size);
					memcpy((CHAR*)(src_frame->addr.AddressCb + offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT]), cc_string, copy_size);
					break;
				}
				else {
					copy_size = block_end_x - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT]), face_pos_array->rgba.AYCC.Y, copy_size);
					memcpy((CHAR*)(src_frame->addr.AddressCb + offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT]), cc_string, copy_size);
				}
				
				offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				offset_addr_c[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				current_pos_x += copy_size;
			}
		}
		else {
			if (loop_y != 0) {
				if ((current_pos_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] & 15) == 0) {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] +=	col_block_total - D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK + 64;
				}
				else {
					line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] += 64;
				}
			}

			offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] = line_start_offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT];
			
			for (;;) {
				block_start_x = (current_pos_x & ~0x3F);
				block_end_x = block_start_x + 64;
				
				if ((block_end_x - display_data[i].rect_data.Rect.Postion.StartX) >= display_data[i].rect_data.Rect.Width) {
					copy_size = display_data[i].rect_data.Rect.Postion.StartX + display_data[i].rect_data.Rect.Width - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT]), face_pos_array->rgba.AYCC.Y, copy_size);
					break;
				}
				else {
					copy_size = block_end_x - current_pos_x;
					memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT]), face_pos_array->rgba.AYCC.Y, copy_size);
				}
				
				offset_addr_y[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT] += D_BF_STILL_IMAGE_DRAW_FACE_FRAME_VIDEOFMT_BLOCK - (current_pos_x - block_start_x);
				current_pos_x += copy_size;
			}
		}
	}
	
	return 0;
}
#endif	// ### end MILB_Porting

static VOID bf_still_image_draw_face_frame_fill_line_cbcr( ULONG start_addr, USHORT line_width, CHAR* cbcr_string )
{
	USHORT cnt_width = 0;
	
	for (cnt_width = 0; cnt_width < line_width; cnt_width += 4) {
		if ((line_width - cnt_width) < 4) {
			memcpy((CHAR*)(start_addr + cnt_width), cbcr_string, line_width - cnt_width);
			break;
		}
		memcpy((CHAR*)(start_addr + cnt_width), cbcr_string, 4);
	}
}

static BOOL bf_still_is_mask_enable( const BF_MASK_CTRL* const mask_ctrl )
{
	UINT32	loopcnt;

	if( mask_ctrl == NULL ) {
		return FALSE;
	}


	for( loopcnt = 0; loopcnt < mask_ctrl->mask_num; loopcnt++ ) {
		if( mask_ctrl->mask_info[loopcnt].enable != FALSE ) {
			return TRUE;
		}
	}

	return FALSE;
}

static INT32 bf_still_image_draw_one_face_frame_ycc( FJ_IMG_INFO* src_frame, const T_FACE_FRAME_EACH_SET* face_pos_array )
{
	ULONG i;
	USHORT loop_x;
	USHORT loop_y;
	ULONG offset_addr_y;
	ULONG offset_addr_c;
	T_BF_OSD_SHAPE_COMM_SHAPE_INFO rect_data;
	T_POS frame_theckness;
	T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_END];
	CHAR cbcr_string[5] = { 0 };

	rect_data.Rect.Postion.StartX = face_pos_array->start_pos.Position.StartX;
	rect_data.Rect.Postion.StartY = face_pos_array->start_pos.Position.StartY;
	rect_data.Rect.Width = face_pos_array->start_pos.Width;
	rect_data.Rect.Height = face_pos_array->start_pos.Height;
	frame_theckness.StartX = face_pos_array->v_thickness;
	frame_theckness.StartY = face_pos_array->h_thickness;
	
	BF_Osd_Shape_Comm_Divide_Rect_5_Parts(&rect_data, &frame_theckness, display_data);

	for (loop_x = 0; loop_x < 4; loop_x += 2) {
		cbcr_string[loop_x] = face_pos_array->rgba.AYCC.Cb;
		cbcr_string[loop_x + 1] = face_pos_array->rgba.AYCC.Cr;
	}

	for (i = E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT; i < E_BF_OSD_SHAPE_COMM_DIV_RECT_END; i++) {
		for (loop_y = 0; loop_y < display_data[i].rect_data.Rect.Height; loop_y++) {
			offset_addr_y = src_frame->width * (display_data[i].rect_data.Rect.Postion.StartY + loop_y) + display_data[i].rect_data.Rect.Postion.StartX;
			memset((CHAR*)(src_frame->addr.AddressY + offset_addr_y), face_pos_array->rgba.AYCC.Y, display_data[i].rect_data.Rect.Width);

			if( FJ_IMG_PFMT_YCC420 == src_frame->pfmt ) {
				if (loop_y % 2 == 0) {
					offset_addr_c = src_frame->width * (display_data[i].rect_data.Rect.Postion.StartY/2 + loop_y/2) + display_data[i].rect_data.Rect.Postion.StartX;
					bf_still_image_draw_face_frame_fill_line_cbcr( src_frame->addr.AddressCb + offset_addr_c, display_data[i].rect_data.Rect.Width, cbcr_string );
				}
			}
			else if( FJ_IMG_PFMT_YCC422 == src_frame->pfmt ) {
				offset_addr_c = src_frame->width * (display_data[i].rect_data.Rect.Postion.StartY + loop_y) + display_data[i].rect_data.Rect.Postion.StartX;
				bf_still_image_draw_face_frame_fill_line_cbcr( src_frame->addr.AddressCb + offset_addr_c, display_data[i].rect_data.Rect.Width, cbcr_string );
			}
		}
	}
	
	return 0;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * static calc target size
 * @param USHORT width
 * @param USHORT lines
 * @param UCHAR   mode (0 : Still Capture / 1 : Movie Capture / 2 : Play Back)
 * @return ULONG taget_size
 */
ULONG BF_Still_Calc_Target_Size( const USHORT width, const USHORT lines, const E_BF_JPGEPC_MODE mode, USHORT cr )
{
	ULONG result;
//	USHORT cr;

	result= width * lines * 2;
//	cr = BF_Still_Attr_Get_Comp_Main();

	if (width == WIDTH_THUMB) {
		if ( (cr >= 40) || (cr == 0) ) {
			cr = 1;		// Max 4_5
		}
		else if (cr > 19) {
			cr = 19;	// min 20_0
		}
	}
	else {
		if ( (mode == E_BF_JPGEPC_MODE_CAPTURE) || (mode == E_BF_JPGEPC_MODE_PLAY) ) {
			if (cr >= 41) {
				cr = 0;		// Max 4_0
			}
		}
	}

	// cr to div value
	switch ( cr ) {
		case FJ_CR_4_0:
		case FJ_CR_5_0:
		case FJ_CR_6_0:
		case FJ_CR_7_0:
		case FJ_CR_8_0:
		case FJ_CR_9_0:
		case FJ_CR_10_0:
			result /= (ULONG)cr / 2 + 4;
			break;
		case FJ_CR_4_5:
		case FJ_CR_5_5:
		case FJ_CR_6_5:
		case FJ_CR_7_5:
		case FJ_CR_8_5:
		case FJ_CR_9_5:
			result /= ((DOUBLE)cr + 8) / 2;
			break;
		case FJ_CR_12_0:
			result /= 12;
			break;
		case FJ_CR_14_0:
			result /= 14;
			break;
		case FJ_CR_15_0:
			result /= 15;
			break;
		case FJ_CR_16_0:
		case FJ_CR_17_0:
		case FJ_CR_18_0:
		case FJ_CR_19_0:
		case FJ_CR_20_0:
		case FJ_CR_21_0:
		case FJ_CR_22_0:
		case FJ_CR_23_0:
		case FJ_CR_24_0:
		case FJ_CR_25_0:
		case FJ_CR_26_0:
		case FJ_CR_27_0:
		case FJ_CR_28_0:
		case FJ_CR_29_0:
		case FJ_CR_30_0:
		case FJ_CR_31_0:
		case FJ_CR_32_0:
		case FJ_CR_33_0:
		case FJ_CR_34_0:
		case FJ_CR_35_0:
		case FJ_CR_36_0:
		case FJ_CR_37_0:
		case FJ_CR_38_0:
		case FJ_CR_39_0:
		case FJ_CR_40_0:
			result /= (ULONG)cr;	/* pgr0074 */
			break;
		case FJ_CR_3_0:
			result /= 3;
			break;
		case FJ_CR_3_5:
			result /= (DOUBLE)7 / 2;
			break;
		case FJ_CR_2_0:
			result /= 2;
			break;
		case FJ_CR_2_5:
			result /= (DOUBLE)5 / 2;
			break;
		case FJ_CR_2_7:
			result /= (DOUBLE)27 / 10;
			break;
		default:
			break;
	}
	return result;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ IIP - Resize(AFN)
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

/**
 *	@brief		Resolution Conversion
 *	@param		T_BF_STILL_IMG *src : source image info.
 *	@param		T_BF_STILL_IMG *dst : destination image info.
 *	@param		T_RESIZE_DOWNSAMP *down : downsampling config (REC skip).
 *	@param		E_RESIZE_MODE resize_mode : resizing method.
 *	@return		None
 *	@note		support Video format.
 *	@attention	This function uses 2ch REC.
 */
FJ_ERR_CODE BF_Still_Resize_Image( const T_RESIZE_IMG* const src, const T_RESIZE_IMG* const dst,
						   const E_RESIZE_MODE resize_mode, const E_IM_IIP_UTIL_ROTATE rotation )
{
	T_FJ_IIP_RES_CONFIG_OUT		res_config;
	INT32						res_ercd;
	FJ_ERR_CODE					fj_ercd;
	T_IM_IIP_UTIL_RR			iip_util_rr;
	T_BF_STILL_IIP_RESOURCE		res = {
		// Auto allocation by bf_still_proc_subr_alloc_iip_res().
		.wait_factor = 0, .unitid_bitmask64 = 0, .pixid_bitmask = 0,
		.src_unitid = E_IM_IIP_UNIT_ID_MAX,       .dst_unitid = E_IM_IIP_UNIT_ID_MAX,
		.src_portid = E_IM_IIP_PARAM_PORTID_NONE, .dst_portid = E_IM_IIP_PARAM_PORTID_NONE,
		.src_pixid = 0xff, .dst_pixid = 0xff,
		.src_master_if = D_IM_IIP_MASTER_IF0,
		.dst_master_if = D_IM_IIP_MASTER_IF0,
		.src_cache_sel = E_IM_IIP_PARAM_CSEL_0,
		.open_res_bitmask = E_IM_IIP_OPEN_RES_NONE,
		// Manual tuning.
		.wait_time = 1000,
	};
	T_BF_STILL_IIP_RESOURCE_RR	res_rr = {
		.param_master_if = D_BF_STILL_RESCONV_UNIT_PARAM_MASTER_IF,
	};
	T_FJ_IIP_RES_CONFIG_ARG		iip_arg_conf = {
		.unit = {
			.oned	= 0,	.flt	= 0,	.afn_0	= 0,	.afn_1	= 0,	.afn_2	= 0,	.afn	= 1,
			.frect	= 0,	.mon	= 0,	.csc	= 0,	.lut	= 0,	.gpc	= 0,	.blend	= 0,
			.cfl	= 0,	.mft	= 0,	.sl		= 0,	.sl_0_2	= 0,	.sl_3_8	= 0,
			.sl0	= 0,	.sl1	= 0,	.sl2	= 0,	.sl3	= 0,	.sl4	= 0,	.sl5	= 0,
			.sl6	= 0,	.sl7	= 0,	.sl8	= 0,	.sl_mon = 0,
			.sl_master_if_0 = 1,	.sl_master_if_1 = 0,	.sl_master_if_2 = 0,
		},
		.pixid = 2,			// request 2 pixids.
		.other = D_FJ_IIP_RES_OTHER_NONE
	};
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES + D_BF_STILL_IIP_TBL_ALIGN]; // 268byte + 32byte
	ULONG param_buffer_addr = M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 ercd;
	const CHAR							errmsg[] = "BF_Still_Resize_Image: ercd=%d no=%u\n";

	res_ercd = BF_IIP_Res_Config_Lock_BaseFW( &res_config, &iip_arg_conf, 1, res.wait_time *3 );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, res_ercd, 1 ));
		return FJ_ERR_NG;
	}

	while( 1 ) {
		fj_ercd = BF_Still_Iip_Set_Config( &res_config, &res );
		if( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, fj_ercd, 2 ));
			break;
		}


		res_rr.src_unitid = res.src_unitid;
		res_rr.dst_unitid = res.dst_unitid;
		res_rr.src_pixid = BF_Still_Iip_Conv_Pixid2UtilPixid( res.src_pixid );
		res_rr.dst_pixid = BF_Still_Iip_Conv_Pixid2UtilPixid( res.dst_pixid );
		res_rr.src_cache_sel = res.src_cache_sel;
		res_rr.src_master_if = res.src_master_if;
		res_rr.dst_master_if = res.dst_master_if;
		res_rr.wait_time = res.wait_time;

//		printf( "pix=%u, %u, unit=%u, %u\n", res.src_pixid, res.dst_pixid, res_rr.src_unitid, res_rr.dst_unitid );


		BF_Still_Iip_Set_ResizeRotate( &src->img, &dst->img, &res_rr, &iip_util_rr );
		iip_util_rr.src.rect.top = (DOUBLE)src->rect.top;
		iip_util_rr.src.rect.left = (DOUBLE)src->rect.left;
		iip_util_rr.src.rect.width = src->rect.width;
		iip_util_rr.src.rect.lines = src->rect.lines;
		iip_util_rr.dst.rect.top = (DOUBLE)dst->rect.top;
		iip_util_rr.dst.rect.left = (DOUBLE)dst->rect.left;
		iip_util_rr.dst.rect.width = dst->rect.width;
		iip_util_rr.dst.rect.lines = dst->rect.lines;
		iip_util_rr.rotation = rotation;

		switch( resize_mode ) {
			case E_RESIZE_MODE_NEAREST:
				iip_util_rr.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_NEAREST;
				break;
			case E_RESIZE_MODE_BI_LINEAR:
				iip_util_rr.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				break;
			case E_RESIZE_MODE_BI_CUBIC:
				iip_util_rr.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC;
				break;
			default:
				BF_Debug_Print_Warning(( "BF_Still_Resize_Image: unknown parameter. use BI-LINEAR.\n" ));
				iip_util_rr.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				break;
		}


		ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr,
											NULL,
											res_rr.param_master_if,
											M_SDRAM_MAP_COMMON_ADR_LOG2PHY(param_buffer_addr),
											res_rr.wait_time );
		if( ercd != D_IM_IIP_OK ) {
			M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_VIEW, FJ_MBALOG_TYPE_POINT, "resize err" ));
			BF_Debug_Print_Error(( "IM_IIP_Utility_ResizeRotate result = 0x%x.\n", ercd ));
			fj_ercd = FJ_ERR_NG;
			break;
		}

		break;
	}


	res_ercd = BF_IIP_Res_Config_Free_BaseFW( &res_config, 1, res.wait_time *3 );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, res_ercd, 3 ));
		return FJ_ERR_NG;
	}

	return fj_ercd;
}

VOID BF_Still_Resize_Image_CbCrOnly( const T_RESIZE_IMG* const src, const T_RESIZE_IMG* const dst,
						   const E_RESIZE_MODE resize_mode, const UINT32 wait_time, const UCHAR src_mif, const UCHAR dst_mif, const BF_MASK_CTRL* const mask_ctrl )	/* pgr0339 */
{
	T_FJ_IIP_RES_CONFIG_OUT		res_config;
	INT32						res_ercd;
	FJ_ERR_CODE					fj_ercd;
	UCHAR								afn_unit_inf_buffer[sizeof(T_IM_IIP_PARAM_AFN) + D_BF_STILL_IIP_TBL_ALIGN];
	UCHAR								sts_unit_inf_buffer[sizeof(T_IM_IIP_PARAM_STS_FILL) + D_BF_STILL_IIP_TBL_ALIGN];
	UCHAR								afn_tbl_buffer[sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) + D_BF_STILL_IIP_TBL_ALIGN];
	T_BF_STILL_IIP_AFN_PARAM			afn_param = {
		.grain_pixs.width = 64,
		.grain_pixs.lines = 8,
		.dst_pos = {
			.sta_x = 0,
			.sta_y = 0,
			.width = 0,
			.lines = 0,
		},
		.prefetch_off = E_IM_IIP_PARAM_PFOFF_RUN,
		.fil_md = 0,
		.mask_enable = FALSE,
		.alignment.width = 1,
		.alignment.lines = 1,
		.rotate_deg = FJ_IMG_ROTATE_DEGREE_0,
		.nearest_neighbor = FALSE,
		.mirror = FALSE,
	};
	T_BF_STILL_IIP_RESOURCE		res = {
		// Auto allocation by bf_still_proc_subr_alloc_iip_res().
		.wait_factor = 0, .unitid_bitmask64 = 0, .pixid_bitmask = 0,
		.src_unitid = E_IM_IIP_UNIT_ID_MAX,       .dst_unitid = E_IM_IIP_UNIT_ID_MAX,
		.src_portid = E_IM_IIP_PARAM_PORTID_NONE, .dst_portid = E_IM_IIP_PARAM_PORTID_NONE,
		.src_pixid = 0xff, .dst_pixid = 0xff,
		.src_master_if = D_IM_IIP_MASTER_IF0,			// Overwrite by BF_Still_Iip_Set_SrcMasterIf()
		.dst_master_if = D_IM_IIP_MASTER_IF0,			// Overwrite by BF_Still_Iip_Set_DstMasterIf()
		.src_cache_sel = E_IM_IIP_PARAM_CSEL_0,			// Overwrite by BF_Still_Iip_Set_SrcCacheSel()
		.open_res_bitmask = E_IM_IIP_OPEN_RES_NONE,
		// Manual tuning.
		.wait_time = wait_time,
	};
	T_FJ_IIP_RES_CONFIG_ARG		iip_arg_conf = {
		.unit = {
			.oned	= 0,	.flt	= 0,	.afn_0	= 0,	.afn_1	= 0,	.afn_2	= 0,	.afn	= 1,
			.frect	= 0,	.mon	= 0,	.csc	= 0,	.lut	= 0,	.gpc	= 0,	.blend	= 0,
			.cfl	= 0,	.mft	= 0,	.sl		= 0,	.sl_0_2	= 0,	.sl_3_8	= 0,
			.sl0	= 0,	.sl1	= 0,	.sl2	= 0,	.sl3	= 0,	.sl4	= 0,	.sl5	= 0,
			.sl6	= 0,	.sl7	= 0,	.sl8	= 0,	.sl_mon = 0,
			.sl_master_if_0 = 0,	.sl_master_if_1 = 0,	.sl_master_if_2 = 0,
			.sl_0_2_master_if_0 = 0,	.sl_0_2_master_if_1 = 0,	.sl_0_2_master_if_2 = 0,
		},
		.pixid = 2,			// request 2 pixids.
		.other = D_FJ_IIP_RES_OTHER_NONE
	};
	INT32 ercd;
	const CHAR							errmsg[] = "BF_Still_Resize_Image_CbCrOnly: ercd=%d no=%u\n";

	afn_param.mask_enable = bf_still_is_mask_enable( mask_ctrl );
	if( afn_param.mask_enable ) {
		afn_param.mask_ctrl = *mask_ctrl;
	}

	//reference function BF_Still_Proc_Execute_AfnMainImage in still_proc_subr.c
	afn_param.afn_unit_inf = (T_IM_IIP_PARAM_AFN*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)afn_unit_inf_buffer );
	afn_param.sl_unit_inf = (T_IM_IIP_PARAM_STS_FILL*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)sts_unit_inf_buffer );
	afn_param.afn_tbl = (T_IM_IIP_PARAM_AFN_OPCOL_0*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)afn_tbl_buffer );
	afn_param.crop_rect.sta_x = 0;
	afn_param.crop_rect.sta_y = 0;
	afn_param.crop_rect.width = M_BF_STILL_AFN_CONV_TBL_VAL( src->img.pixs.width );
	afn_param.crop_rect.lines = M_BF_STILL_AFN_CONV_TBL_VAL( src->img.pixs.lines );

	// Select Master IF
	fj_ercd = BF_Still_Iip_Set_SrcCacheSel( &res.src_cache_sel, src_mif );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, fj_ercd, 1 ));
		return;
	}
	fj_ercd = BF_Still_Iip_Set_SrcMasterIf( &res.src_master_if, src_mif );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, fj_ercd, 2 ));
		return;
	}
	fj_ercd = BF_Still_Iip_Set_DstMasterIf( &res.dst_master_if, &iip_arg_conf, dst_mif, afn_param.mask_enable );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, fj_ercd, 3 ));
		return;
	}

	res_ercd = BF_IIP_Res_Config_Lock_BaseFW( &res_config, &iip_arg_conf, 1, res.wait_time *3 );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, res_ercd, 4 ));
		return;
	}

	while( 1 ) {
		fj_ercd = BF_Still_Iip_Set_Config( &res_config, &res );
		if( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, fj_ercd, 5 ));
			break;
		}


		ercd = BF_Still_Iip_Open( &res );
		if( ercd != D_IM_IIP_OK ) {
			BF_Debug_Print_Error(( errmsg, ercd, 6 ));
			fj_ercd = FJ_ERR_NG;
			break;
		}

		while( 1 ) {
			fj_ercd = BF_Still_Iip_Set_Afn_CbCrOnly( &src->img, &dst->img, &res, &afn_param );
			if( fj_ercd != FJ_ERR_OK ) {
				BF_Debug_Print_Error(( errmsg, fj_ercd, 7 ));
				break;
			}

			ercd = BF_Still_Iip_Start( &res, E_BF_STILL_IIP_LINK_OFF );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, ercd, 8 ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			ercd = BF_Still_Iip_WaitEnd( &res );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, ercd, 9 ));
				fj_ercd = FJ_ERR_NG;
				break;
			}


			fj_ercd = FJ_ERR_OK;
			break;
		}

		ercd = BF_Still_Iip_Close( &res );
		if( ercd != D_IM_IIP_OK ) {
			BF_Debug_Print_Error(( errmsg, ercd, 10 ));
			fj_ercd = FJ_ERR_NG;
			break;
		}

		break;
	}


	res_ercd = BF_IIP_Res_Config_Free_BaseFW( &res_config, 1, res.wait_time *3 );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, res_ercd, 11 ));
		return;
	}
}


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ IIP - SPF
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
FJ_ERR_CODE BF_Still_Process_SPF( const FJ_IMG_SPF_CTRL* const spf_ctrl_info )
{
#if 0	// replace
	INT32							ercd = FJ_ERR_OK;
	INT32							res_ercd;
	FJ_ERR_CODE						fj_ercd;
	T_BF_STILL_IMG src = {
//		.type = E_BF_STILL_IMG_TYPE_YCC420,
//		.plane = E_BF_STILL_PLANE_TYPE_C_INTL,
		.depth = E_BF_STILL_IMG_DEPTH_U8,
//		.addr = {
//			.y = ,
//			.cb = ,
//			.cr = ,
//		},
//		.pixs = {
//			.width = ,
//			.lines = ,
//		},
//		.g_width = {
//			.y = ,
//			.cb = ,
//			.cr = ,
//		},
		.aspect = E_BF_STILL_ASPECT_TYPE_CUSTOM,	// dummy
	};
	T_BF_STILL_IMG dst = {
//		.type = E_BF_STILL_IMG_TYPE_YCC420,
//		.plane = E_BF_STILL_PLANE_TYPE_C_INTL,
		.depth = E_BF_STILL_IMG_DEPTH_U8,
//		.addr = {
//			.y = ,
//			.cb = ,
//			.cr = ,
//		},
//		.pixs = {
//			.width = ,
//			.lines = ,
//		},
//		.g_width = {
//			.y = ,
//			.cb = ,
//			.cr = ,
//		},
		.aspect = E_BF_STILL_ASPECT_TYPE_CUSTOM,	// dummy
	};
	UCHAR flt_unit_inf_buffer[sizeof(T_IM_IIP_PARAM_FLT) + D_BF_STILL_IIP_TBL_ALIGN];
	UCHAR sts_unit_inf_buffer[sizeof(T_IM_IIP_PARAM_STS_FILL) + D_BF_STILL_IIP_TBL_ALIGN];
	T_IM_IIP_PARAM_FLT*				flt_unit_inf = (T_IM_IIP_PARAM_FLT*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)flt_unit_inf_buffer );
	T_IM_IIP_PARAM_STS*				sl_unit_inf = (T_IM_IIP_PARAM_STS*)M_BF_STILL_ROUNDUP_IIP_ALIGN( (ULONG)sts_unit_inf_buffer );
	T_IM_IIP_UNIT_CFG				flt_cfg;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_BF_STILL_IIP_RESOURCE			res = {
		// Auto allocation by bf_still_proc_subr_alloc_iip_res().
		.wait_factor = 0, .unitid_bitmask64 = 0, .pixid_bitmask = 0,
		.src_unitid = E_IM_IIP_UNIT_ID_MAX,       .dst_unitid = E_IM_IIP_UNIT_ID_MAX,
		.src_portid = E_IM_IIP_PARAM_PORTID_NONE, .dst_portid = E_IM_IIP_PARAM_PORTID_NONE,
		.src_pixid = 0xff,
		.dst_pixid = 0xff,
		.src_master_if = D_IM_IIP_MASTER_IF0,			// Overwrite by BF_Still_Iip_Set_SrcMasterIf()
		.dst_master_if = D_IM_IIP_MASTER_IF0,			// Overwrite by BF_Still_Iip_Set_DstMasterIf()
		.src_cache_sel = E_IM_IIP_PARAM_CSEL_0,			// Overwrite by BF_Still_Iip_Set_SrcCacheSel()
		.open_res_bitmask = E_IM_IIP_OPEN_RES_NONE,
		// Manual tuning.
		.wait_time = spf_ctrl_info->timeout,
	};
	T_FJ_IIP_RES_CONFIG_ARG			iip_arg_conf = {
		.unit = {
			.oned	= 0,	.flt	= 1,	.afn_0	= 0,	.afn_1	= 0,	.afn_2	= 0,	.afn	= 0,
			.frect	= 0,	.mon	= 0,	.csc	= 0,	.lut	= 0,	.gpc	= 0,	.blend	= 0,
			.cfl	= 0,	.mft	= 0,	.sl		= 1,	.sl_0_2	= 0,	.sl_3_8	= 0,
			.sl0	= 0,	.sl1	= 0,	.sl2	= 0,	.sl3	= 0,	.sl4	= 0,	.sl5	= 0,
			.sl6	= 0,	.sl7	= 0,	.sl8	= 0,	.sl_mon = 0,
			.sl_master_if_0 = 0,	.sl_master_if_1 = 0,	.sl_master_if_2 = 0,
		},
		.pixid = 2,			// request 2 pixids.
		.other = D_FJ_IIP_RES_OTHER_NONE
	};
	UINT32							wait_factor_result = 0;
	T_FJ_IIP_RES_CONFIG_OUT			res_config;
	const CHAR						errmsg[] = "BF_Still_Process_SPF: no=%u ercd=%d\n";

	// Select Master IF
	fj_ercd = BF_Still_Iip_Set_SrcCacheSel( &res.src_cache_sel, spf_ctrl_info->src_mif_sel );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 1, fj_ercd ));
		return fj_ercd;
	}
	fj_ercd = BF_Still_Iip_Set_SrcMasterIf( &res.src_master_if, spf_ctrl_info->src_mif_sel );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 2, fj_ercd ));
		return fj_ercd;
	}
	fj_ercd = BF_Still_Iip_Set_DstMasterIf( &res.dst_master_if, &iip_arg_conf, spf_ctrl_info->dst_mif_sel, FALSE );
	if( fj_ercd != FJ_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 3, fj_ercd ));
		return fj_ercd;
	}

	res_ercd = BF_IIP_Res_Config_Lock_BaseFW( &res_config, &iip_arg_conf, 1, res.wait_time *3 );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 4, res_ercd ));
		return FJ_ERR_NG;
	}
	
	src.type		= bf_still_conv_image_type( spf_ctrl_info->src_img.pfmt );
	src.plane		= bf_still_conv_image_plane( spf_ctrl_info->src_img.pfmt );
	src.addr.y		= spf_ctrl_info->src_img.addr.AddressY;
	src.addr.cb		= spf_ctrl_info->src_img.addr.AddressCb;
	src.addr.cr		= spf_ctrl_info->src_img.addr.AddressCr;
	src.pixs.width	= spf_ctrl_info->src_img.width;
	src.pixs.lines	= spf_ctrl_info->src_img.lines;
	src.g_width.y	= spf_ctrl_info->src_img.g_y_width;
	src.g_width.cb	= spf_ctrl_info->src_img.g_c_width;
	src.g_width.cr	= spf_ctrl_info->src_img.g_c_width;
	
	dst.type		= bf_still_conv_image_type( spf_ctrl_info->dst_img.pfmt );
	dst.plane		= bf_still_conv_image_plane( spf_ctrl_info->dst_img.pfmt );
	dst.addr.y		= spf_ctrl_info->dst_img.addr.AddressY;
	dst.addr.cb		= spf_ctrl_info->dst_img.addr.AddressCb;
	dst.addr.cr		= spf_ctrl_info->dst_img.addr.AddressCr;
	dst.pixs.width	= spf_ctrl_info->dst_img.width;
	dst.pixs.lines	= spf_ctrl_info->dst_img.lines;
	dst.g_width.y	= spf_ctrl_info->dst_img.g_y_width;
	dst.g_width.cb	= spf_ctrl_info->dst_img.g_c_width;
	dst.g_width.cr	= spf_ctrl_info->dst_img.g_c_width;
	
	while( 1 ){
		fj_ercd = BF_Still_Iip_Set_Config( &res_config, &res );
		if( fj_ercd != FJ_ERR_OK ) {
			BF_Debug_Print_Error(( errmsg, 5, fj_ercd ));
			break;
		}


		ercd = Im_IIP_Open_SWTRG(  res.unitid_bitmask64, res.pixid_bitmask, res.open_res_bitmask, res.wait_time );
		if( ercd != D_IM_IIP_OK ) {
			BF_Debug_Print_Error(( errmsg, 6, ercd ));
			fj_ercd = FJ_ERR_NG;
			break;
		}
		
		while( 1 ){
			ercd = BF_Still_ImgInfo_Set_IipPixFmtTbl( &pixfmt_tbl_0, &src );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 7, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}
//			BF_Still_ImgInfo_Dump( "spf src", &src );
			ercd = BF_Still_ImgInfo_Set_IipPixFmtTbl( &pixfmt_tbl_1, &dst );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 9, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}
//			BF_Still_ImgInfo_Dump( "spf dst", &dst );
			*flt_unit_inf = g_still_iip_param_flt_base;
			flt_unit_inf->TDLTOPCNF.bit.WAITCONF_0 =  res.dst_portid;
			flt_unit_inf->PIXIDDEF.bit.IPIXID0 =  res.src_pixid;
			flt_unit_inf->TDL_HSIZE.bit.HEND = pixfmt_tbl_0.width -1;
			flt_unit_inf->TDL_VSIZE.bit.VEND = pixfmt_tbl_0.lines -1;
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_0  = spf_ctrl_info->spf_kernel.kernel_y[0];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_1  = spf_ctrl_info->spf_kernel.kernel_y[1];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_2  = spf_ctrl_info->spf_kernel.kernel_y[2];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_3  = spf_ctrl_info->spf_kernel.kernel_y[3];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_4  = spf_ctrl_info->spf_kernel.kernel_y[4];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_5  = spf_ctrl_info->spf_kernel.kernel_y[5];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_6  = spf_ctrl_info->spf_kernel.kernel_y[6];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_7  = spf_ctrl_info->spf_kernel.kernel_y[7];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_8  = spf_ctrl_info->spf_kernel.kernel_y[8];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_9  = spf_ctrl_info->spf_kernel.kernel_y[9];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_10 = spf_ctrl_info->spf_kernel.kernel_y[10];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_11 = spf_ctrl_info->spf_kernel.kernel_y[11];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_12 = spf_ctrl_info->spf_kernel.kernel_y[12];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_13 = spf_ctrl_info->spf_kernel.kernel_y[13];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_14 = spf_ctrl_info->spf_kernel.kernel_y[14];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_15 = spf_ctrl_info->spf_kernel.kernel_y[15];
			flt_unit_inf->SPF_FLTW_D0.bit.SPF_FLTW_16 = spf_ctrl_info->spf_kernel.kernel_y[16];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_0  = spf_ctrl_info->spf_kernel.kernel_c[0];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_1  = spf_ctrl_info->spf_kernel.kernel_c[1];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_2  = spf_ctrl_info->spf_kernel.kernel_c[2];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_3  = spf_ctrl_info->spf_kernel.kernel_c[3];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_4  = spf_ctrl_info->spf_kernel.kernel_c[4];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_5  = spf_ctrl_info->spf_kernel.kernel_c[5];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_6  = spf_ctrl_info->spf_kernel.kernel_c[6];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_7  = spf_ctrl_info->spf_kernel.kernel_c[7];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_8  = spf_ctrl_info->spf_kernel.kernel_c[8];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_9  = spf_ctrl_info->spf_kernel.kernel_c[9];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_10 = spf_ctrl_info->spf_kernel.kernel_c[10];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_11 = spf_ctrl_info->spf_kernel.kernel_c[11];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_12 = spf_ctrl_info->spf_kernel.kernel_c[12];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_13 = spf_ctrl_info->spf_kernel.kernel_c[13];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_14 = spf_ctrl_info->spf_kernel.kernel_c[14];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_15 = spf_ctrl_info->spf_kernel.kernel_c[15];
			flt_unit_inf->SPF_FLTW_D1.bit.SPF_FLTW_16 = spf_ctrl_info->spf_kernel.kernel_c[16];
			flt_unit_inf->SPF_FLTW_D2 = flt_unit_inf->SPF_FLTW_D1;
			flt_unit_inf->SPF_FLTW_D3 = flt_unit_inf->SPF_FLTW_D0;
			flt_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
			flt_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
			flt_cfg.unit_param_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)flt_unit_inf );
			flt_cfg.load_unit_param_flag = 0;

			*sl_unit_inf = g_still_iip_param_sts_base;
			sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF =  res.src_portid;
			sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID =  res.dst_pixid;
			sl_unit_inf->BASE.SL_HSIZE.bit.HEND = pixfmt_tbl_1.width -1;
			sl_unit_inf->BASE.SL_VSIZE.bit.VEND = pixfmt_tbl_1.lines -1;

			sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
			sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
			sl_cfg.unit_param_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)sl_unit_inf );
			sl_cfg.load_unit_param_flag = res.unitid_bitmask64;

			BF_Still_Dcache_Clean( flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) );
			BF_Still_Dcache_Clean( sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

			ercd = Im_IIP_Ctrl_PIXFMTTBL(  res.src_pixid, &pixfmt_tbl_0 );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 10, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			ercd = Im_IIP_Ctrl_PIXFMTTBL(  res.dst_pixid, &pixfmt_tbl_1 );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 11, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			ercd = Im_IIP_Ctrl_SWTRG_Unit(  res.src_unitid, &flt_cfg );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 12, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			ercd = Im_IIP_Ctrl_SWTRG_Unit(  res.dst_unitid, &sl_cfg );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 13, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			ercd = Im_IIP_Set_Interrupt(  res.wait_factor, D_IM_IIP_ENABLE_ON );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 14, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			BF_Still_Iip_Dump_UnitParam( "BF_Still_Process_SPF(): unit param\n", &res, &g_still_spf_unit_dump_tid );

			ercd = Im_IIP_Start_SWTRG(  res.dst_unitid, D_IM_IIP_SWTRG_ON );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 15, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			ercd = Im_IIP_Start_SWTRG(  res.src_unitid, D_IM_IIP_SWTRG_ON );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 16, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}

			Dd_ARM_Dsb_Pou();

			ercd = Im_IIP_Wait_End( &wait_factor_result, res.wait_factor, D_IM_IIP_OR_WAIT, res.wait_time );
			if( ercd != D_IM_IIP_OK ) {
				BF_Debug_Print_Error(( errmsg, 17, ercd ));
				fj_ercd = FJ_ERR_NG;
				break;
			}
			else
			if( (wait_factor_result & D_IM_IIP_INT_FACTOR_AXIERR) != 0 ) {
				BF_Debug_Print_Error(( errmsg, 18, wait_factor_result ));
				fj_ercd = FJ_ERR_NG;
				break;
			}


			fj_ercd = FJ_ERR_OK;
			break;
		}

		ercd = Im_IIP_Close_SWTRG( res.unitid_bitmask64, res.pixid_bitmask, res.open_res_bitmask );
		if( ercd != D_IM_IIP_OK ) {
			BF_Debug_Print_Error(( errmsg, 19, ercd ));
			fj_ercd = FJ_ERR_NG;
		}

		break;
	}


	res_ercd = BF_IIP_Res_Config_Free_BaseFW( &res_config, 1, res.wait_time );
	if( res_ercd != D_FJ_IIP_ERR_OK ) {
		BF_Debug_Print_Error(( errmsg, 20, res_ercd ));
		return FJ_ERR_NG;
	}


	return fj_ercd;
#else
	return FJ_ERR_NG;
#endif	// replace
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ Copy
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

/**
 *	@brief		Copy regist image
 *	@param		ULONG src_addr
 *	@param		ULONG dst_addr
 *	@return		None
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE BF_Still_Copy_Image( const ULONG src_addr, const ULONG dst_addr, const UINT32 width, const UINT32 lines )	/* pgr0339 */
{
	T_IM_XCH_COPY_EX copy_param = {
		.xch = E_IM_XCH_CH_SEL_1,
		.copy_type = E_IM_XCH_COPY_SEL_NORMAL,
		.size = {
			.width = width,
			.lines = lines,
			.src_gl_width = width,
			.dst_gl_width = width,
		},
		.addr = {
			.src = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(src_addr),
			.dst = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(dst_addr),
		},
	};
	INT32	ercd;

	ercd = Im_Xch_Copy_Ex_Sync( &copy_param );
	if( ercd != D_IM_XCH_OK ) {
		BF_Debug_Print_Error(( "BF_Still_Copy_Image: Error Y\n" ));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Still_Copy_YccImage( const FJ_IMG_INFO* const src, const FJ_IMG_INFO* const dst )
{
	T_IM_XCH_COPY_EX copy_param = {
		.xch = E_IM_XCH_CH_SEL_1,
		.copy_type = E_IM_XCH_COPY_SEL_NORMAL,
//		.size = {
//			.width = width,
//			.lines = lines,
//			.src_gl_width = width,
//			.dst_gl_width = width,
//		},
//		.addr = {
//			.src = src_addr,
//			.dst = dst_addr,
//		},
	};
	USHORT	c_lines;
	INT32	ercd;

	c_lines = BF_Still_FjImgInfo_Calc_C_Lines( dst );

	// Y
	copy_param.size.src_gl_width = src->g_y_width;
	copy_param.size.dst_gl_width = dst->g_y_width;
	copy_param.size.width = dst->width;
	copy_param.size.lines = dst->lines;
	copy_param.addr.src = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(src->addr.AddressY);
	copy_param.addr.dst = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(dst->addr.AddressY);

	ercd = Im_Xch_Copy_Ex_Sync( &copy_param );
	if( ercd != D_IM_XCH_OK ) {
		BF_Debug_Print_Error(( "BF_Still_Copy_YccImage: Error Y\n" ));
		return FJ_ERR_NG;
	}

	// CbCr
	copy_param.size.src_gl_width = src->g_c_width;
	copy_param.size.dst_gl_width = dst->g_c_width;
	copy_param.size.width = src->g_c_width;	// for Video format
	copy_param.size.lines = c_lines;
	copy_param.addr.src = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(src->addr.AddressCb);
	copy_param.addr.dst = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(dst->addr.AddressCb);

	ercd = Im_Xch_Copy_Ex_Sync( &copy_param );
	if( ercd != D_IM_XCH_OK ) {
		BF_Debug_Print_Error(( "BF_Still_Copy_YccImage: Error C\n" ));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 * @brief		Draw Face Frame For Video format
 * @param		i:FJ_IMG_INFO* src_frame
 * @param		i:T_FACE_FRAME_EACH_SET* face_pos_array
 * @param		i:T_FACE_FRAME_SET* face_set
 * @return		successful: 0
 *				unsuccessful: -1
 * @note		None
 * @attention	None
 */
INT32 BF_Still_Image_Draw_Face_Frame_VideoFmt( FJ_IMG_INFO* src_frame, const T_FACE_FRAME_EACH_SET* face_pos_array, const T_FACE_FRAME_SET* face_set )
{

#if 0	// ### begin MILB_Porting
	for ( USHORT cnt = 0; cnt <= face_set->face_count; ++cnt ) {
		bf_still_image_draw_one_face_frame_videofmt( src_frame, &face_pos_array[cnt] );
	}
	BF_Still_Dcache_CleanFlush_FjImg( src_frame );
#endif	// ### end MILB_Porting
	return 0;
}

/**
 * @brief		Draw Face Frame For YCC422 And YCC420
 * @param		i:FJ_IMG_INFO* src_frame
 * @param		i:T_FACE_FRAME_EACH_SET* face_pos_array
 * @param		i:T_FACE_FRAME_SET* face_set
 * @return		successful: 0
 *				unsuccessful: -1
 * @note		None
 * @attention	None
 */
INT32 BF_Still_Image_Draw_Face_Frame_YCC( FJ_IMG_INFO* src_frame, const T_FACE_FRAME_EACH_SET* face_pos_array, const T_FACE_FRAME_SET* face_set )
{
	for ( USHORT cnt = 0; cnt <= face_set->face_count; ++cnt ) {
		bf_still_image_draw_one_face_frame_ycc( src_frame, &face_pos_array[cnt] );
	}
	BF_Still_Dcache_CleanFlush_FjImg( src_frame );
	return 0;
}

/**
 * @brief		Draw Rect For YCC
 * @param		i:FJ_IMG_INFO* src_frame
 * @param		i:T_FACE_FRAME_EACH_SET* face_pos_array
 * @param		i:T_FACE_FRAME_SET* face_set
 * @return		successful: FJ_ERR_OK
 *				unsuccessful: FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Still_Image_Draw_Rect_YCC(FJ_IMG_INFO* src_frame, const FJ_RECT_FRAME_SET* frame_set )
{
	USHORT box_loop;
	ULONG line_idx;
	ULONG draw_addr;
	ULONG cbcr_data;
	ULONG c_width_cnt;
	ULONG c_lines;
	for (box_loop = 0; box_loop < frame_set->rect_count; ++box_loop){
		//y
		draw_addr = src_frame->addr.AddressY + (src_frame->g_y_width * frame_set->rect[box_loop].pos.Postion.StartY) + frame_set->rect[box_loop].pos.Postion.StartX;
		for (line_idx =0; line_idx < frame_set->rect[box_loop].pos.Height; ++line_idx, draw_addr += src_frame->g_y_width){
			memset((CHAR*)draw_addr, frame_set->rect[box_loop].color_ycc.Y, frame_set->rect[box_loop].pos.Width);
		}

		//cbcr
		cbcr_data = (frame_set->rect[box_loop].color_ycc.Cb) |(frame_set->rect[box_loop].color_ycc.Cr << 8) | (frame_set->rect[box_loop].color_ycc.Cb << 16) | (frame_set->rect[box_loop].color_ycc.Cr << 24);

		if (src_frame->pfmt == FJ_IMG_PFMT_YCC420){
			draw_addr = src_frame->addr.AddressCb+ (src_frame->g_y_width * frame_set->rect[box_loop].pos.Postion.StartY / 2) + frame_set->rect[box_loop].pos.Postion.StartX;
			c_lines = frame_set->rect[box_loop].pos.Height / 2;
		}
		else {
			draw_addr = src_frame->addr.AddressCb+ (src_frame->g_y_width * frame_set->rect[box_loop].pos.Postion.StartY) + frame_set->rect[box_loop].pos.Postion.StartX;
			c_lines = frame_set->rect[box_loop].pos.Height;
		}
		for (line_idx =0; line_idx < c_lines; ++line_idx, draw_addr += src_frame->g_y_width){
			if ((frame_set->rect[box_loop].pos.Width) % 4 == 0){
				for (c_width_cnt = 0; c_width_cnt < frame_set->rect[box_loop].pos.Width; c_width_cnt += 4){
					*((ULONG*)(draw_addr + c_width_cnt)) = cbcr_data;
				}
			}
			else if ((frame_set->rect[box_loop].pos.Width) % 2 == 0){
				for (c_width_cnt = 0; c_width_cnt < (frame_set->rect[box_loop].pos.Width - 2); c_width_cnt += 4){
					*((ULONG*)(draw_addr + c_width_cnt)) = cbcr_data;
				}
				*((USHORT*)(draw_addr + c_width_cnt)) = (USHORT)cbcr_data;
			}
			else if ((frame_set->rect[box_loop].pos.Width) % 4 == 3){
				for (c_width_cnt = 0; c_width_cnt < (frame_set->rect[box_loop].pos.Width - 3); c_width_cnt += 4){
					*((ULONG*)(draw_addr + c_width_cnt)) = cbcr_data;
				}
				*((USHORT*)(draw_addr + c_width_cnt)) = (USHORT)cbcr_data;
				*((UCHAR*)(draw_addr + c_width_cnt + 2)) = frame_set->rect[box_loop].color_ycc.Cb;
			}
			else if ((frame_set->rect[box_loop].pos.Width) % 4 == 1){
				for (c_width_cnt = 0; c_width_cnt < (frame_set->rect[box_loop].pos.Width - 1); c_width_cnt += 4){
					*((ULONG*)(draw_addr + c_width_cnt)) = cbcr_data;
				}
				*((UCHAR*)(draw_addr + c_width_cnt)) = frame_set->rect[box_loop].color_ycc.Cb;
			}
		}

	}
	BF_Still_Dcache_CleanFlush_FjImg( src_frame );
	return FJ_ERR_OK;
}

/**
 * @brief		Draw Rect For Video format
 * @param		i:FJ_IMG_INFO* src_frame
 * @param		i:T_FACE_FRAME_EACH_SET* face_pos_array
 * @param		i:T_FACE_FRAME_SET* face_set
 * @return		successful: FJ_ERR_OK
 *				unsuccessful: FJ_ERR_NG
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Still_mage_Draw_Rect_VideoFmt(FJ_IMG_INFO* src_frame, const FJ_RECT_FRAME_SET* frame_set )
{
	USHORT box_loop;
	static ULONG draw_addr;
	ULONG line_idx;
	ULONG gl_w_block_count;
	ULONG block_row;
	ULONG block_col;
	ULONG block_row_remain;
	ULONG block_col_remain;
	ULONG bytes_per_block_row;
	ULONG start_x, end_x;
	ULONG y_set_size;
	ULONG c_width_cnt;
	ULONG cbcr_data;

	gl_w_block_count = src_frame->g_y_width / 64;
	bytes_per_block_row = 64 * 16 * gl_w_block_count;
	for (box_loop = 0; box_loop < frame_set->rect_count; ++box_loop){
		//y
		for (line_idx =0; line_idx < frame_set->rect[box_loop].pos.Height; ++line_idx){
			block_row = (frame_set->rect[box_loop].pos.Postion.StartY + line_idx) / 16;
			block_row_remain = (frame_set->rect[box_loop].pos.Postion.StartY + line_idx) % 16;
			start_x = frame_set->rect[box_loop].pos.Postion.StartX;
			end_x = frame_set->rect[box_loop].pos.Postion.StartX + frame_set->rect[box_loop].pos.Width;
			while (end_x - start_x != 0){
				block_col = start_x / 64;
				block_col_remain = start_x % 64;

				draw_addr = src_frame->addr.AddressY + bytes_per_block_row * block_row + (64 * 16 * block_col) + (64 * block_row_remain + block_col_remain);
				y_set_size = ((64 - block_col_remain) > (end_x - start_x)) ? (end_x - start_x) : (64 - block_col_remain);
				if (y_set_size >64){
					y_set_size = 64;
				}
				memset((CHAR*)draw_addr, frame_set->rect[box_loop].color_ycc.Y, y_set_size);
				start_x += y_set_size;
			}
		}

		//cbcr
		cbcr_data = (frame_set->rect[box_loop].color_ycc.Cb) |(frame_set->rect[box_loop].color_ycc.Cr << 8) | (frame_set->rect[box_loop].color_ycc.Cb << 16) | (frame_set->rect[box_loop].color_ycc.Cr << 24);
		for (line_idx =0; line_idx < frame_set->rect[box_loop].pos.Height / 2; ++line_idx){
			block_row = (frame_set->rect[box_loop].pos.Postion.StartY / 2 + line_idx) / 16;
			block_row_remain = (frame_set->rect[box_loop].pos.Postion.StartY / 2 + line_idx) % 16;
			start_x = (frame_set->rect[box_loop].pos.Postion.StartX >> 1) << 1;
			end_x = ((frame_set->rect[box_loop].pos.Postion.StartX + frame_set->rect[box_loop].pos.Width) >> 1) << 1;
			while (end_x - start_x != 0){
				block_col = start_x / 64;
				block_col_remain = start_x % 64;

				draw_addr = src_frame->addr.AddressCb + bytes_per_block_row * block_row + (64 * 16 * block_col) + (64 * block_row_remain + block_col_remain);
				y_set_size = ((64 - block_col_remain) > (end_x - start_x)) ? (end_x - start_x) : (64 - block_col_remain);
				if (y_set_size >64){
					y_set_size = 64;
				}
				if (y_set_size % 4 == 0){
					for (c_width_cnt = 0; c_width_cnt < y_set_size; c_width_cnt += 4){
						*((ULONG*)(draw_addr + c_width_cnt)) = cbcr_data;
					}
				}
				else if (y_set_size % 2 == 0){
					for (c_width_cnt = 0; c_width_cnt < (y_set_size - 2); c_width_cnt += 4){
						*((ULONG*)(draw_addr + c_width_cnt)) = cbcr_data;
					}
					*((USHORT*)(draw_addr + c_width_cnt)) = (USHORT)cbcr_data;
				}
				else if (y_set_size % 4 == 3){
					for (c_width_cnt = 0; c_width_cnt < (y_set_size - 3); c_width_cnt += 4){
						*((ULONG*)(draw_addr + c_width_cnt)) = cbcr_data;
					}
					*((USHORT*)(draw_addr + c_width_cnt)) = (USHORT)cbcr_data;
					*((UCHAR*)(draw_addr + c_width_cnt + 2)) = frame_set->rect[box_loop].color_ycc.Cb;
				}
				else if (y_set_size % 4 == 1){
					for (c_width_cnt = 0; c_width_cnt < (y_set_size - 1); c_width_cnt += 4){
						*((ULONG*)(draw_addr + c_width_cnt)) = cbcr_data;
					}
					*((UCHAR*)(draw_addr + c_width_cnt)) = frame_set->rect[box_loop].color_ycc.Cb;
				}
				start_x += y_set_size;
			}
		}
	}
	BF_Still_Dcache_CleanFlush_FjImg( src_frame );
	return FJ_ERR_OK;
}

