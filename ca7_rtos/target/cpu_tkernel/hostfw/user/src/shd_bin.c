/**
 * @file		shading_bin.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "shd_bin.h"
#include "l1l2cache.h"
#include "math.h"

#pragma GCC optimize ("O0")
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
SHORT g_sample_cshd_tbl[ E_SHARE_SENSOR_ID_MAX ][ D_SHARE_CSHD_TBL_SIZE ]; // u4.12

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 SHD_BIN_Make_Tbl( const T_SHD_BIN_TBL_INFO_IN* in, T_SHD_BIN_TBL_INFO_OUT* out, FJ_PRO_CTRL_FIRST_PIX_COLOR pix )
{
	T_FSHD_TBL_HEADER		*head;
	USHORT					*bin_RR, *bin_GR, *bin_GB, *bin_BB;
	USHORT					*bay_TopL, *bay_TopR, *bay_BotL, *bay_BotR;
	USHORT					*tbl;
	USHORT					blk_h = 0;
	USHORT					blk_v = 0;
	USHORT					blk_byte = 0;
	USHORT					left_shift = 0;
	UCHAR					first_pix = 0;
	INT32					sensor_id, index, cntx, cnty;
	
	tbl = (USHORT*)in->available_memory_addr;
	
	for( sensor_id = 0; sensor_id < in->sensor_num && sensor_id < E_SHARE_SENSOR_ID_MAX; sensor_id ++ ){
		
		// head
		head = (T_FSHD_TBL_HEADER*)in->bin_addr[sensor_id][E_SHD_INDEX_0]; // E_SHD_INDEX_1/2 header should be same as E_SHD_INDEX_1
		
		// check
		if( head == NULL ){
			UPRINTF_ERR(1);
			return NG;
		}
		
		// get blk num, SENSOR_1/2/3 should be same as SENSOR_0
		if( sensor_id == 0 ){
			blk_h = head->version.tbl_columns;
			blk_v = head->version.tbl_rows;
			blk_byte = head->version.tbl_global_col_bytes;
			
			// adjust left_shit to match SHD HW format u4.12
			switch( head->version.frame_tbl_bit ){
			case 1: // u6.10
				left_shift = 2;
				break;
			case 0: // u4.12
			default:
				left_shift = 0;
				break;
			}
			
			
			first_pix = pix;
		}
		
		// make table
		for( index = 0; index < E_SHD_INDEX_MAX; index ++ ){
			
			// get header again
			head = (T_FSHD_TBL_HEADER*)in->bin_addr[sensor_id][index];
			// get from bin
			bin_RR = (USHORT*)( (ULONG)head + sizeof( T_FSHD_TBL_HEADER ) + head->version.tbl_frame_fld_bytelen * 0 );
			bin_GR = (USHORT*)( (ULONG)head + sizeof( T_FSHD_TBL_HEADER ) + head->version.tbl_frame_fld_bytelen * 1 );
			bin_GB = (USHORT*)( (ULONG)head + sizeof( T_FSHD_TBL_HEADER ) + head->version.tbl_frame_fld_bytelen * 2 );
			bin_BB = (USHORT*)( (ULONG)head + sizeof( T_FSHD_TBL_HEADER ) + head->version.tbl_frame_fld_bytelen * 3 );
			
			// change order by first pix
			switch( first_pix ){
			case FJ_PRO_CTRL_FIRST_PIX_COLOR_B:
				bay_TopL = bin_BB;	bay_TopR = bin_GB;
				bay_BotL = bin_GR;	bay_BotR = bin_RR;
				break;
				
			case FJ_PRO_CTRL_FIRST_PIX_COLOR_GR:
				bay_TopL = bin_GR;	bay_TopR = bin_RR;
				bay_BotL = bin_BB;	bay_BotR = bin_GB;
				break;
				
			case FJ_PRO_CTRL_FIRST_PIX_COLOR_GB:
				bay_TopL = bin_GB;	bay_TopR = bin_BB;
				bay_BotL = bin_RR;	bay_BotR = bin_GR;
				break;
				
			case FJ_PRO_CTRL_FIRST_PIX_COLOR_R:
			default:
				bay_TopL = bin_RR;	bay_TopR = bin_GR;
				bay_BotL = bin_GB;	bay_BotR = bin_BB;
				break;
			}
			
			// make table
			out->tbl_addr[sensor_id][index] = (ULONG)tbl;
			for( cnty = 0; cnty < blk_v; cnty++ ){
				
				for( cntx = 0; cntx < blk_h; cntx ++ ){
					*tbl++ = (*bay_TopL++) << left_shift;
					*tbl++ = (*bay_TopR++) << left_shift;
				}
				
				for( cntx = 0; cntx < blk_h; cntx ++ ){
					*tbl++ = (*bay_BotL++) << left_shift;
					*tbl++ = (*bay_BotR++) << left_shift;
				}
				
				// align
				for( cntx = 0; cntx < (blk_byte/2 - blk_h); cntx++ ){
					bay_TopL++;
					bay_TopR++;
					bay_BotL++;
					bay_BotR++;
				}
				
			}
			
			// align if need
			
		}
	}
	
	out->tbl_h_byte = blk_h * 2 * 2; // 2(R GR) 2(byte)
	out->tbl_h_blk_num = blk_h;
	out->tbl_v_blk_num = blk_v;
	out->available_memory_addr = (ULONG)tbl;
	out->available_memory_addr += 1023;
	out->available_memory_addr &= 0xFFFFFC00; // align 1024 for safety
	
	BF_L1l2cache_Flush_Addr(in->available_memory_addr, out->available_memory_addr - in->available_memory_addr);
	return OK;
}


INT32 CSHD_Make_Tbl( UCHAR sensor_id, UCHAR sensor_mode, ULONG center_h ,ULONG center_v, T_SHARE_CSHD_PARAM* out )
{
	INT32 i;
	FLOAT iad; // distance
	UCHAR lir; // LUT value correction coefficient
	USHORT tbl_size = D_SHARE_CSHD_TBL_SIZE; 

	// find out the suitable LIR and table size
	iad = sqrtf(center_h * center_h + center_v * center_v);
	for(lir = 0; lir <= 7; lir++){
		tbl_size = (USHORT)(iad * powf(2, 0 - lir) + 1.0);
		if(tbl_size <= D_SHARE_CSHD_TBL_SIZE){
			break;
		}
	}		

	if(tbl_size > D_SHARE_CSHD_TBL_SIZE){
		// no sutiable LIR??
		UPRINTF_ERR(1);
		lir = 7;
		tbl_size = D_SHARE_CSHD_TBL_SIZE;
	}
	
	// make table for sensor id and sensor mode.
	switch( sensor_id ){
	case E_SHARE_SENSOR_ID_0:
	case E_SHARE_SENSOR_ID_1:
	case E_SHARE_SENSOR_ID_2:
	case E_SHARE_SENSOR_ID_3:
	default:
		// make table
		for( i = 0; i < tbl_size; i ++ ){
			#if 1
			{ // sample
				USHORT num = i * 16 / tbl_size;
				if((num % 2) == 0)
					g_sample_cshd_tbl[ sensor_id ][ i ] = num * 1024; // x1.0 (u4.12)
				else
					g_sample_cshd_tbl[ sensor_id ][ i ] = num * 1024 + 4096; // increase x0.25 in every 2 step + x1.0 (u4.12)
				}
			#endif
		}
		BF_L1l2cache_Flush_Addr( (UINT32)g_sample_cshd_tbl[ sensor_id ], D_SHARE_CSHD_TBL_SIZE*2 );
		// set output
		out->LIR_interpolated	= lir;	// 0 ~ 7
		out->PARH_interval_h	= 64;	// 64 ~ 16
		out->PARV_interval_v	= 64;	// 64 ~ 16
		out->tbl_size			= tbl_size;
		out->tbl				= (SHORT*)&g_sample_cshd_tbl[ sensor_id ];
	break;
	}
	
	return OK;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
