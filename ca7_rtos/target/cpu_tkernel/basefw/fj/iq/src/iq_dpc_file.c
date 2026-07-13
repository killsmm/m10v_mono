/**
 * @file		iq_dpc_file.c
 * @brief		Badpix data table file load/save API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_iq.h"

#include "iq_dpc_file.h"
#include "iq_dpc_tbl.h"
#include "iq_storage.h"
#include "iq_dcache.h"
#include "iq_common.h"

#include "debug.h"

#include "string.h"

#include "ddim_typedef.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_BF_IQ_DPC_MAGIC_STR							"DPC_M9M\0"
#define D_BF_IQ_DPC_VER_400								"0400"
#define D_BF_IQ_DPC_MAX_FIELDS_VER400					(2)


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

// T_BF_IQ_DPC_TBL_FILE_HEADER_V400
typedef struct {
	UCHAR	magic_str[8];							// magic string(ASCII)
	UCHAR	format_ver[4];							// file format version(ASCII)
	UCHAR	reserved1[4];							// Reserved
	ULLONG	ui_tbl_bytelen;							// table bytes(not include of header bytes)
	USHORT	htrim;									// htrim
	USHORT	vtrim;									// vtrim
	USHORT	bay_width;								// Bayer width
	USHORT	bay_lines;								// Bayer lines
	UCHAR	field_num;								// Field number
	UCHAR	reserved2[3];							// reserved
	UINT32	ui_pixel_num;							// Compensation pixel count
	ULLONG	ui_tbl_bytelen_by_fields[D_BF_IQ_DPC_MAX_FIELDS_VER400];	// tbl bytes (by fields)
} T_BF_IQ_DPC_TBL_FILE_HEADER_V400;

// T_BF_IQ_DPC_LOAD_BODY_PARAM - bf_iq_dpc_load_badpix_body()'s parameter
typedef struct {
	T_BF_IQ_DPC_TBL_INFO* tbl_info;					// dpc SDRAM table information
	ULLONG	tbl_bytelen;							// table bytes(not include of header bytes)
	UINT32	pixel_num;								// Compensation pixel count
	ULLONG*	tbl_bytelen_by_fields;					// tbl bytes (by fields)
} T_BF_IQ_DPC_LOAD_BODY_PARAM;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static volatile UINT32 g_bf_iq_dpc_file_write_buf[(M_IQ_ROUNDUP_8(M_BF_IQ_DPC_CALC_TBL_BYTE(D_BF_IQ_DPC_MAX_PIXELS)) + sizeof(T_BF_IQ_DPC_TBL_FILE_HEADER_V400)) / sizeof(UINT32) +1] __attribute__((aligned(8)));


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
static VOID bf_iq_dpc_load_badpix_body( T_BF_IQ_DPC_LOAD_BODY_PARAM* const param )
{
	UINT32 ui_field_idx;
	UINT32 ui_tbl_offset_byte;
	BYTE* puc_tbl_top_addr = NULL;

	puc_tbl_top_addr = (BYTE*)param->tbl_info->top_addr;

	/*** store table bytelen by fields ***/
	for( ui_field_idx = 0; ui_field_idx < D_BF_IQ_DPC_MAX_FIELDS; ui_field_idx++ ) {
		param->tbl_info->bytelen_by_fields[ui_field_idx]
			= param->tbl_bytelen_by_fields[ui_field_idx];
	}

	/*** calc. table absoute address ***/
	for( ui_field_idx = 0, ui_tbl_offset_byte = 0; ui_field_idx < D_BF_IQ_DPC_MAX_FIELDS; ui_field_idx++ ) {
		if( param->tbl_bytelen_by_fields[ui_field_idx] != 0 ) {
			param->tbl_info->addr_by_fields[ui_field_idx]
				= (U_BF_IQ_DPC_TBL_DATA *)(puc_tbl_top_addr + ui_tbl_offset_byte);
			ui_tbl_offset_byte += param->tbl_info->bytelen_by_fields[ui_field_idx];
		} else {
			param->tbl_info->addr_by_fields[ui_field_idx] = NULL;
		}
	}

	/*** store table bytelen ***/
	param->tbl_info->bytelen = param->tbl_bytelen;

	/*** store defect pixel num in table ***/
	param->tbl_info->pixel_num = param->pixel_num;
}


static BOOL bf_iq_dpc_load_badpix_v400( T_BF_STORAGE_INFO* const strg_info, T_BF_IQ_DPC_TBL_FILE_HEADER_V400* const t_dpc_tbl_file_header, T_BF_IQ_DPC_TBL_INFO* const tbl_info )
{
	UINT32 loopcnt;
	FJ_ERR_CODE fj_ercd;
	UINT32* puc_tbl_top_addr = NULL;
	T_BF_IQ_DPC_LOAD_BODY_PARAM load_param;
	ULONG ul_read_byte;

	t_dpc_tbl_file_header->ui_tbl_bytelen = M_IQBIN_NTOHL(t_dpc_tbl_file_header->ui_tbl_bytelen);
	t_dpc_tbl_file_header->htrim = M_IQBIN_NTOHS(t_dpc_tbl_file_header->htrim);
	t_dpc_tbl_file_header->vtrim = M_IQBIN_NTOHS(t_dpc_tbl_file_header->vtrim);
	t_dpc_tbl_file_header->bay_width = M_IQBIN_NTOHS(t_dpc_tbl_file_header->bay_width);
	t_dpc_tbl_file_header->bay_lines = M_IQBIN_NTOHS(t_dpc_tbl_file_header->bay_lines);
	t_dpc_tbl_file_header->ui_pixel_num = M_IQBIN_NTOHL(t_dpc_tbl_file_header->ui_pixel_num);
	for( loopcnt = 0; loopcnt < D_BF_IQ_DPC_MAX_FIELDS_VER400; loopcnt++ ) {
		t_dpc_tbl_file_header->ui_tbl_bytelen_by_fields[loopcnt] = M_IQBIN_NTOHL(t_dpc_tbl_file_header->ui_tbl_bytelen_by_fields[loopcnt]);
	}

	if( t_dpc_tbl_file_header->ui_tbl_bytelen > tbl_info->maxbytelen ) {
		return FALSE;
	}


	/*** read table body ***/
	puc_tbl_top_addr = (UINT32*)tbl_info->top_addr;
	ul_read_byte = t_dpc_tbl_file_header->ui_tbl_bytelen;
	fj_ercd = BF_IQ_Storage_Read( strg_info, (BYTE*)puc_tbl_top_addr, &ul_read_byte );
	if(( fj_ercd != FJ_ERR_OK ) && ( fj_ercd != FJ_ERR_READ_EOF)) {
		return FALSE;
	}
	if( ul_read_byte != (ULONG)t_dpc_tbl_file_header->ui_tbl_bytelen ) {
		return FALSE;
	}


	/*** read successful. ***/


	load_param.tbl_info = tbl_info;
	load_param.tbl_bytelen = t_dpc_tbl_file_header->ui_tbl_bytelen;
	load_param.pixel_num = t_dpc_tbl_file_header->ui_pixel_num;
	load_param.tbl_bytelen_by_fields = t_dpc_tbl_file_header->ui_tbl_bytelen_by_fields;
	bf_iq_dpc_load_badpix_body( &load_param );

	tbl_info->sta_x = t_dpc_tbl_file_header->htrim;
	tbl_info->sta_y = t_dpc_tbl_file_header->vtrim;
	tbl_info->width = t_dpc_tbl_file_header->bay_width;
	tbl_info->lines = t_dpc_tbl_file_header->bay_lines;
	tbl_info->field_num = t_dpc_tbl_file_header->field_num;

	fj_ercd = BF_IQ_Dcache_Clear( (ULONG)tbl_info->top_addr, tbl_info->bytelen );
	if( fj_ercd != FJ_ERR_OK ) {
		return FALSE;
	}

	/*** table status changed to valid ***/
	tbl_info->valid = TRUE;

	return TRUE;
}


static BOOL bf_iq_dpc_load_badpix( T_BF_STORAGE_INFO* const strg_info, T_BF_IQ_DPC_TBL_INFO* const tbl_info )
{
	BOOL b_ret = FALSE;
	T_BF_IQ_DPC_TBL_FILE_HEADER_V400 t_dpc_tbl_file_header;
	ULONG ul_read_byte;
	FJ_ERR_CODE fj_ercd;

	tbl_info->valid = FALSE;

	while( 1 ) {
		// read table file header and check value
		ul_read_byte = sizeof(t_dpc_tbl_file_header);
		fj_ercd = BF_IQ_Storage_Read( strg_info, (BYTE*)&t_dpc_tbl_file_header, &ul_read_byte );
		if(( fj_ercd != FJ_ERR_OK ) && ( fj_ercd != FJ_ERR_READ_EOF)) {
			b_ret = FALSE;
			break;	/* goto return process */
		}
		if( ul_read_byte != (ULONG)(sizeof(t_dpc_tbl_file_header)) ) {
			b_ret = FALSE;
			break;	/* goto return process */
		}

		if( memcmp( t_dpc_tbl_file_header.magic_str, D_BF_IQ_DPC_MAGIC_STR, sizeof(t_dpc_tbl_file_header.magic_str) ) == 0 ) {
			if( memcmp( t_dpc_tbl_file_header.format_ver, D_BF_IQ_DPC_VER_400, sizeof(t_dpc_tbl_file_header.format_ver) ) == 0 ) {
				b_ret = bf_iq_dpc_load_badpix_v400( strg_info, &t_dpc_tbl_file_header, tbl_info );
			}
		}
		break;
	}


	BF_IQ_Storage_Close( strg_info );

	return b_ret;
}


static BOOL bf_iq_dpc_save_badpix_v0400( T_BF_STORAGE_INFO* const strg_info, T_BF_IQ_DPC_TBL_INFO* const tbl_info, ULONG* const write_bytes_sdram )
{
	T_BF_IQ_DPC_TBL_FILE_HEADER_V400	t_dpc_tbl_file_header;
	FJ_ERR_CODE							fj_ercd;
	UINT32								ui_field_idx;
	BOOL								b_ret = FALSE;

	while( 1 ) {
		/*** create table file header ***/
		memset( &t_dpc_tbl_file_header, 0, sizeof(t_dpc_tbl_file_header) );
		memcpy( t_dpc_tbl_file_header.magic_str, D_BF_IQ_DPC_MAGIC_STR, sizeof(t_dpc_tbl_file_header.magic_str) );	/* pgr0832 */
		memcpy( t_dpc_tbl_file_header.format_ver, D_BF_IQ_DPC_VER_400, sizeof(t_dpc_tbl_file_header.format_ver) );	/* pgr0832 */
		t_dpc_tbl_file_header.htrim = M_IQBIN_HTONS(tbl_info->sta_x);
		t_dpc_tbl_file_header.vtrim = M_IQBIN_HTONS(tbl_info->sta_y);
		t_dpc_tbl_file_header.bay_width = M_IQBIN_HTONS(tbl_info->width);
		t_dpc_tbl_file_header.bay_lines = M_IQBIN_HTONS(tbl_info->lines);
		t_dpc_tbl_file_header.field_num = tbl_info->field_num;
		memset( t_dpc_tbl_file_header.reserved1, 0, sizeof(t_dpc_tbl_file_header.reserved1) );
		memset( t_dpc_tbl_file_header.reserved2, 0, sizeof(t_dpc_tbl_file_header.reserved2) );

		t_dpc_tbl_file_header.ui_tbl_bytelen = M_IQBIN_HTONLL(tbl_info->bytelen);

		t_dpc_tbl_file_header.ui_pixel_num = M_IQBIN_HTONLL(tbl_info->pixel_num);

		for( ui_field_idx = 0; ui_field_idx < D_BF_IQ_DPC_MAX_FIELDS; ui_field_idx++ ) {
			t_dpc_tbl_file_header.ui_tbl_bytelen_by_fields[ui_field_idx]
				= M_IQBIN_HTONL(tbl_info->bytelen_by_fields[ui_field_idx]);
		}


		/*** write table file header ***/
		fj_ercd = BF_IQ_Storage_Write( strg_info, (BYTE*)&t_dpc_tbl_file_header, sizeof(t_dpc_tbl_file_header) );
		if( fj_ercd != FJ_ERR_OK ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "FJ_IQ_CalibrationSaveBadpixTable(): header write error\n" ));
#endif
			b_ret = FALSE;
			break;	/* goto return process */
		}


		/*** write table body ***/
		fj_ercd = BF_IQ_Storage_Write( strg_info, (BYTE*)tbl_info->top_addr, tbl_info->bytelen );
		if( fj_ercd != FJ_ERR_OK ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "FJ_IQ_CalibrationSaveBadpixTable(): body write error\n" ));
#endif
			b_ret = FALSE;
			break;	/* goto return process */
		}

		b_ret = TRUE;
		break;	/* goto return process */
	}


	(VOID)BF_IQ_Storage_Get_WriteBytesSdram( strg_info, write_bytes_sdram );

	BF_IQ_Storage_Close( strg_info );

	return b_ret;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/


/** @brief		FJ_IQ_CalibrationSaveBadpixTable<BR>
 * 				Save bad pixel table to NF partition for specified target
 * @param[in]	e_target		Calibration target(view/movie/capture)
 * @param[in]	pc_to_file_path	save file absolute path(begin drive letter)
 * @retval		TRUE	success of table saving.
 * @retval		FALSE	failure of table saving.<br>
 * 								 o parameter error<br>
 * 								 o file open error(ex. file not found)<br>
 * 								 o file write error(ex. drive is full)
 * @note		None
 * @attention	None
 */
BOOL fj_iq_calibrationsavebadpixtable( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx, const CHAR* const pc_to_file_path )
{
	T_BF_STORAGE_INFO		strg_info;
	FJ_ERR_CODE				fj_ercd;
	T_BF_IQ_DPC_TBL_INFO*	tbl_info;
	ULONG					dummy;

	if( (e_calib_tgt <= E_IQ_CALIB_TGT_INIT)
	 || (e_calib_tgt >= E_IQ_CALIB_TGT_MAX)
	 ) {
		return FALSE;
	}
	if( pc_to_file_path == NULL ) {
		return FALSE;
	}
	if( pc_to_file_path[0] == '\0' ) {
		return FALSE;
	}

	tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
	if( tbl_info == NULL ) {
		return FALSE;
	}


	if( tbl_info->valid == FALSE ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "%s(): target table is empty. %u-%u\n", __func__, e_calib_tgt, tbl_idx ));
#endif
		return FALSE;
	}


	fj_ercd = BF_IQ_Storage_WriteOpen_File( &strg_info, pc_to_file_path );
	if( fj_ercd != FJ_ERR_OK ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "%s(): file open error. %s\n", __func__, pc_to_file_path ));
#endif
		return FALSE;
	}

	return bf_iq_dpc_save_badpix_v0400( &strg_info, tbl_info, &dummy );
}


/** @brief		FJ_IQ_CalibrationLoadBadpixTable<BR>
 * 				Load bad pixel table from NF partition for specified target.
 * 				if loading successful, table set to enabled.
 * @param[in]	e_target		Calibration target(view/movie/capture)
 * @param[in]	pc_to_file_path	save file absolute path(begin drive letter)
 * @retval		TRUE	success of table loading and set to valid.
 * @retval		FALSE	failure of table loading.<br>
 * 								 o parameter error<br>
 * 								 o file open error(ex. file not found)<br>
 * 								 o file read error
 * @note		None
 * @attention	None
 */
BOOL fj_iq_calibrationloadbadpixtable( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx, const CHAR* const pc_from_file_path )
{
	T_BF_STORAGE_INFO		strg_info;
	T_BF_IQ_DPC_TBL_INFO*	tbl_info;
	FJ_ERR_CODE				fj_ercd;

	if( (e_calib_tgt <= E_IQ_CALIB_TGT_INIT)
	 || (e_calib_tgt >= E_IQ_CALIB_TGT_MAX)
	 ) {
		return FALSE;
	}
	if( pc_from_file_path == NULL ) {
		return FALSE;
	}
	if( pc_from_file_path[0] == '\0' ) {
		return FALSE;
	}

	tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
	if( tbl_info == NULL ) {
		return FALSE;
	}




	fj_ercd = BF_IQ_Storage_ReadOpen_File( &strg_info, pc_from_file_path );
	if( fj_ercd != FJ_ERR_OK ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "%s(): file open error. %s\n", __func__, pc_from_file_path ));
#endif
		return FALSE;
	}

	return bf_iq_dpc_load_badpix( &strg_info, tbl_info );
}


/** @brief		FJ_IQ_CalibrationDeleteBadpixTable<BR>
 * 				Delete bad pixel table from NF partition for specified target.
 * 				table on RAM has been kept intact.
 * @param[in]	e_target		Calibration target(view/movie/capture)
 * @param[in]	pc_to_file_path	save file absolute path(begin drive letter)
 * @retval		TRUE	success of delete.
 * @retval		FALSE	failure of delete.<br>
 * 								 o parameter error<br>
 * 								 o file not found.
 * @note		None
 * @attention	None
 */
BOOL fj_iq_calibrationdeletebadpixtable( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx, const CHAR* const pc_from_file_path )
{
	if( (e_calib_tgt <= E_IQ_CALIB_TGT_INIT)
	 || (e_calib_tgt >= E_IQ_CALIB_TGT_MAX)
	 ) {
		return FALSE;
	}
	if( pc_from_file_path == NULL ) {
		return FALSE;
	}
	if( pc_from_file_path[0] == '\0' ) {
		return FALSE;
	}

	return( (BF_IQ_Storage_Rmove_File( pc_from_file_path ) != FJ_ERR_NG)?(TRUE):(FALSE) );
}


BOOL fj_iq_notify_loading_badpixtable_complete( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx, const BYTE* const addr, const ULONG bytes )
{
	T_BF_STORAGE_INFO		strg_info;
	T_BF_IQ_DPC_TBL_INFO*	tbl_info;
	FJ_ERR_CODE				fj_ercd;

	if( (e_calib_tgt <= E_IQ_CALIB_TGT_INIT)
	 || (e_calib_tgt >= E_IQ_CALIB_TGT_MAX)
	 ) {
		return FALSE;
	}
	if( addr == NULL ) {
		return FALSE;
	}
	if( bytes == 0 ) {
		return FALSE;
	}

	tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
	if( tbl_info == NULL ) {
		return FALSE;
	}




	fj_ercd = BF_IQ_Storage_ReadOpen_Sdram( &strg_info, addr, bytes );
	if( fj_ercd != FJ_ERR_OK ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "%s(): SDRAM open error. %s\n", __func__, addr ));
#endif
		return FALSE;
	}

	return bf_iq_dpc_load_badpix( &strg_info, tbl_info );
}


FJ_ERR_CODE fj_iq_get_badpixtableaddr( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx, BYTE** const addr, UINT32* const bytes )
{
	T_BF_STORAGE_INFO		strg_info;
	FJ_ERR_CODE				fj_ercd;
	BOOL					b_ercd;
	T_BF_IQ_DPC_TBL_INFO*	tbl_info;
	ULONG					out_bytes;

	if( (e_calib_tgt <= E_IQ_CALIB_TGT_INIT)
	 || (e_calib_tgt >= E_IQ_CALIB_TGT_MAX)
	 ) {
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( addr == NULL ) {
		return FJ_ERR_NG_INPUT_PARAM;
	}

	tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
	if( tbl_info == NULL ) {
		return FJ_ERR_IQ_TBL_INVALID;
	}


	if( tbl_info->valid == FALSE ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "%s(): target table is empty. %u-%u\n", __func__, e_calib_tgt, tbl_idx ));
#endif
		return FJ_ERR_IQ_TBL_INVALID;
	}


	fj_ercd = BF_IQ_Storage_WriteOpen_Sdram( &strg_info, (BYTE*)&g_bf_iq_dpc_file_write_buf, sizeof(g_bf_iq_dpc_file_write_buf) );
	if( fj_ercd != FJ_ERR_OK ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "%s(): SDRAM open error.\n", __func__ ));
#endif
		return fj_ercd;
	}

	b_ercd = bf_iq_dpc_save_badpix_v0400( &strg_info, tbl_info, &out_bytes );
	if( b_ercd == FALSE ) {
		return FJ_ERR_IQ_TBL_FULL;
	}

	fj_ercd = BF_IQ_Dcache_Clear( (ULONG)&g_bf_iq_dpc_file_write_buf, out_bytes );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	*addr = (BYTE*)&g_bf_iq_dpc_file_write_buf;
	*bytes = out_bytes;

	return fj_ercd;
}


/** @brief		FJ_IQ_CalibrationLoadBadpixTable_Sdram<BR>
 * 				Load bad pixel table from SDRAM for specified target.
 * 				if loading successful, table set to enabled.
 * @param[in]	e_target		Calibration target(view/movie/capture)
 * @param[in]	tbl_addr		save file absolute address
 * @retval		TRUE	success of table loading and set to valid.
 * @retval		FALSE	failure of table loading.<br>
 * 								 o parameter error<br>
 * 								 o file open error(ex. file not found)<br>
 * 								 o file read error
 * @note		None
 * @attention	None
 */
BOOL fj_iq_calibrationloadbadpixtable_sdram( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx, CHAR* tbl_addr )
{
	T_BF_IQ_DPC_TBL_INFO*				tbl_info;
	T_BF_IQ_DPC_TBL_FILE_HEADER_V400*	t_dpc_tbl_file_header;
	T_BF_IQ_DPC_LOAD_BODY_PARAM			load_param;
	FJ_ERR_CODE							fj_ercd;

	if( (e_calib_tgt <= E_IQ_CALIB_TGT_INIT)
	 || (e_calib_tgt >= E_IQ_CALIB_TGT_MAX)
	 ) {
		return FALSE;
	}
	if( tbl_addr == NULL ) {
		return FALSE;
	}
	
	tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
	if( tbl_info == NULL ) {
		return FALSE;
	}
	
	tbl_info->valid = FALSE;
	t_dpc_tbl_file_header = (T_BF_IQ_DPC_TBL_FILE_HEADER_V400*)tbl_addr;
	
	if( memcmp( t_dpc_tbl_file_header->magic_str, D_BF_IQ_DPC_MAGIC_STR, sizeof(t_dpc_tbl_file_header->magic_str) ) != 0 ) {
		return FALSE;
	}
	if( memcmp( t_dpc_tbl_file_header->format_ver, D_BF_IQ_DPC_VER_400, sizeof(t_dpc_tbl_file_header->format_ver) ) != 0 ) {
		return FALSE;
	}
	
	if( t_dpc_tbl_file_header->ui_tbl_bytelen > tbl_info->maxbytelen ) {
		return FALSE;
	}
	
	/*** copy body ***/
	memcpy( (VOID*)tbl_info->top_addr, (VOID*)(tbl_addr + sizeof(T_BF_IQ_DPC_TBL_FILE_HEADER_V400) ), (ULONG)t_dpc_tbl_file_header->ui_tbl_bytelen );
	
	/*** tbl ***/
	load_param.tbl_info = tbl_info;
	load_param.tbl_bytelen = t_dpc_tbl_file_header->ui_tbl_bytelen;
	load_param.pixel_num = t_dpc_tbl_file_header->ui_pixel_num;
	load_param.tbl_bytelen_by_fields = t_dpc_tbl_file_header->ui_tbl_bytelen_by_fields;
	bf_iq_dpc_load_badpix_body( &load_param );

	tbl_info->sta_x = t_dpc_tbl_file_header->htrim;
	tbl_info->sta_y = t_dpc_tbl_file_header->vtrim;
	tbl_info->width = t_dpc_tbl_file_header->bay_width;
	tbl_info->lines = t_dpc_tbl_file_header->bay_lines;
	tbl_info->field_num = t_dpc_tbl_file_header->field_num;

	fj_ercd = BF_IQ_Dcache_Clear( (ULONG)tbl_info->top_addr, tbl_info->bytelen );
	if( fj_ercd != FJ_ERR_OK ) {
		return FALSE;
	}

	/*** table status changed to valid ***/
	tbl_info->valid = TRUE;

	return TRUE;
}

