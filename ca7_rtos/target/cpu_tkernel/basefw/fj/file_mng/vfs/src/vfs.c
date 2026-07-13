/**
 * @file		vfs.c
 * @brief		Virtual FileSystem function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_typedef.h"
#include "vfs.h"
#include "ipcu.h"
#include "l1l2cache.h"
#include "memory.h"
#include "sdram_map_share.h"

/*----------------------------------------------------------------*/
/* Definition													  */
/*----------------------------------------------------------------*/
/* IPCU event flag pattern */
#define FLG_FS_VFS_SEND			(0x0000007F)
#define FLG_FS_VFS_RECEIVE		(0x0000007F)

/* Index Max size */
#define FILE_CNT_MAX			(8)			/* The number of the maximum files available for access */
#define REQ_CNT_MAX				(8)			/* The number of the maximum requests for request */

/* Data Buffer Max size */
#define COMM_BUFF_MAX_SIZE		(0x40000)	/* The maximum size of one VFS common buffer */

/* Shared Memory area */
#define FS_VFS_SHARE_MOVIE_DFS			( SDRAM_ADR_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA )
#define FS_VFS_SHARE_MOVIE_DFS_END		( SDRAM_ADR_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA + SDRAM_SIZ_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA )
#define FS_VFS_SHARE_STILL_DFS			( SDRAM_ADR_STILL_MODE_0_HOST_STILL_DFS_AREA )
#define FS_VFS_SHARE_STILL_DFS_END		( SDRAM_ADR_STILL_MODE_0_HOST_STILL_DFS_AREA + SDRAM_SIZ_STILL_MODE_0_HOST_STILL_DFS_AREA )
#define FS_VFS_SHARE_MOVIE_MUX_H		( SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA )
#define FS_VFS_SHARE_MOVIE_MUX_H_END	( SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA + SDRAM_SIZ_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA )
#define	FS_VFS_SHARE_MOVIE_MUX_L		( SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM2_AREA )
#define	FS_VFS_SHARE_MOVIE_MUX_L_END	( SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM2_AREA + SDRAM_SIZ_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM2_AREA )
#define FS_VFS_SHARE_PB_DEMUX			( SDRAM_ADR_HOST_MP4_DEMUXER_READ_DATA_AREA )
#define FS_VFS_SHARE_PB_DEMUX_END		( SDRAM_ADR_HOST_MP4_DEMUXER_READ_DATA_AREA + SDRAM_SIZ_HOST_MP4_DEMUXER_READ_DATA_AREA )
#define FS_VFS_SHARE_PB_THM				( SDRAM_ADR_PLAY_THM_RGB_0 )
#define FS_VFS_SHARE_PB_THM_END			( SDRAM_ADR_PLAY_THM_RGB_0 + SDRAM_SIZ_PLAY_THM_RGB )
#define FS_VFS_SHARE_PB_JPEG			( SDRAM_ADR_HOST_JPEG_DEMUXER_READ_DATA_AREA )
#define FS_VFS_SHARE_PB_JPEG_END		( SDRAM_ADR_HOST_JPEG_DEMUXER_READ_DATA_AREA + SDRAM_SIZ_HOST_JPEG_DEMUXER_READ_DATA_AREA )

/* Retry Max for IPCU communication */
#define FS_VFS_SEND_RETRY_MAX			(5000)

/* Total buffer max */
#define BUFF_NUM_MAX					(8)

/* VFS common buffer is not vacant */
#define FS_VFS_NO_VACANT				(0x0007)

/* VFS state of each buffer */
#define VFS_INIT						(0x00)
#define VFS_SEND						(0x01)
#define VFS_RESEIVE_ACK					(0x02)
#define VFS_RECEIVE_RESPONSE			(0x04)
#define	VFS_FINISH						(0x07)
#define VFS_NOT_SEND					(0x80)

/* VFS DOS file name max */
#define VFS_DOS_FNAME_MAX				(13)	/* MDF spec */

/* Drive name max */
#define VFS_DRV_NAME_MAX				(9)


/*----------------------------------------------------------------*/
/* Enumeration													  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Structure													  */
/*----------------------------------------------------------------*/
/* VFS control table */
typedef struct {
	FW_UINT32				file_id;					/* File ID 								*/
	FW_UCHAR				used_flag;					/* Info TBL used flag					*/
	FW_UCHAR				total_buff_num;				/* The total number of using buffers	*/
	FW_UINT32				buff_addr;					/* Buffer address						*/
	FW_UINT32				remain_size	;				/* Remain buffer size					*/
	FW_UCHAR				proc_state;					/* Process state						*/
} T_FS_VFS_CTRL_TBL;

/* VFS File using table */
typedef struct {
	FW_CHAR					drvname[VFS_DRV_NAME_MAX];	/* Drive name							*/
	FW_INT32				file_use;					/* File use			 					*/
} T_FS_VFS_FILE_USE;


/*----------------------------------------------------------------*/
/* Global Data													  */
/*----------------------------------------------------------------*/
/* VFS request table */
static volatile T_FS_VFS_CTRL_TBL g_bf_vfs_ctrl[FILE_CNT_MAX + 1][REQ_CNT_MAX];

/* VFS File using table */
static volatile T_FS_VFS_FILE_USE g_bf_vfs_file_use[FILE_CNT_MAX];

/* VFS Receive Flag ID */
static volatile FW_INT32 g_bf_vfs_receive_flg_id[FILE_CNT_MAX + 1];


/* VFS Pointer of callback function for notice of writing error */
static volatile VFS_SET_WRITE_ERR_CB g_bf_vfs_write_error[FILE_CNT_MAX] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief Get Info Table number counter.
 * @param i:fNo File No.
 * @param o:*vfs_info_num VFS Info TBL No.	(0 is error)
 * @note none
 * @attention none.
 */
static FW_VOID vfs_get_info_table_num_counter( FW_INT32 fNo, FW_SHORT* vfs_info_num )
{
	FW_UCHAR file_cnt = 0;
	FW_UCHAR req_cnt = 0;
	
	*vfs_info_num = 0;		/* vfs_info_num = 0 is error. */
	
	while( *vfs_info_num == 0 ) {
		if( fNo == 0 ) {
			/* The demand without a argument file No. */
			for( req_cnt = 0; req_cnt < REQ_CNT_MAX; req_cnt++ ) {
				if( g_bf_vfs_ctrl[FILE_CNT_MAX][req_cnt].used_flag == 0 ) {
					g_bf_vfs_ctrl[FILE_CNT_MAX][req_cnt].used_flag = 1;
					*vfs_info_num = (FW_SHORT)( FILE_CNT_MAX * REQ_CNT_MAX + req_cnt + 1 );
					return;
				}
			}
		}
		else {
			/* Is it already fNo with the demand ? */
			for( file_cnt = 0; file_cnt < FILE_CNT_MAX; file_cnt++ ) {
				if( g_bf_vfs_file_use[file_cnt].file_use == fNo ) {
					break;
				}
			}
			
			/* file No. is already used. */
			if( file_cnt != FILE_CNT_MAX ) {
	 			for( req_cnt = 0; req_cnt < REQ_CNT_MAX; req_cnt++ ) {
					if( g_bf_vfs_ctrl[file_cnt][req_cnt].used_flag == 0 ) {
						g_bf_vfs_ctrl[file_cnt][req_cnt].used_flag = 1;
						*vfs_info_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + req_cnt + 1 );
						return;
					}
				}
			}
			else {
				/* fNo. is a new request. */
				for( file_cnt = 0; file_cnt < FILE_CNT_MAX; file_cnt++ ) {
					if( g_bf_vfs_file_use[file_cnt].file_use == 0 ) {
						g_bf_vfs_file_use[file_cnt].file_use = fNo;
						g_bf_vfs_ctrl[file_cnt][0].used_flag = 1;
						*vfs_info_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + 1 );
						break;
					}
				}
				return;
			}
		}
	}
}

/**
 * @brief Clear Info Table number counter.
 * @param i:fNo File No.
 * @param i:*vfs_info_num VFS Info TBL No.
 * @note none
 * @attention none.
 */
static FW_VOID vfs_clear_info_table_num_counter( FW_INT32 fNo, FW_SHORT vfs_info_num )
{
	FW_UCHAR file_cnt = 0;
	FW_UCHAR req_cnt = 0;
	
	if( fNo == 0 ) {
		req_cnt = vfs_info_num - ( FILE_CNT_MAX * REQ_CNT_MAX + 1 );
		g_bf_vfs_ctrl[FILE_CNT_MAX][req_cnt].file_id = 0;
		g_bf_vfs_ctrl[FILE_CNT_MAX][req_cnt].used_flag = 0;
	}
	else {
		for( file_cnt = 0; file_cnt < FILE_CNT_MAX; file_cnt++ ) {
			req_cnt = vfs_info_num - ( file_cnt * REQ_CNT_MAX + 1 );

			if( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id == fNo ) {						/* Check a request to which fNo. */
				g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = 0;
				g_bf_vfs_ctrl[file_cnt][req_cnt].used_flag = 0;
				break;
			}
		}

 		for( req_cnt = 0; req_cnt < REQ_CNT_MAX; req_cnt++ ) {
			if( g_bf_vfs_ctrl[file_cnt][req_cnt].used_flag != 0 ) {
				break;																	/* There is used Info TBL */
			}
		}
	}
}

/**
 * @brief Get file using table.
 * @param none
 * @return vacant file index
 * @note none
 * @attention none.
 */
static FW_CHAR vfs_get_file_use_table( FW_VOID )
{
	FW_UCHAR file_cnt;
	FW_CHAR ret_code = FILE_CNT_MAX;
	
	/* Object request to specify the file id only */
	for( file_cnt = 0; file_cnt < FILE_CNT_MAX; file_cnt++ ) {
		if( g_bf_vfs_file_use[file_cnt].file_use == 0 ) {
			ret_code = file_cnt;
			break;
		}
	}
	return ret_code;
}

/**
 * @brief Set file using table.
 * @param i:file_cnt file counter.
 * @param i:fNo file No.
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 vfs_set_file_use_table( FW_UCHAR file_cnt, const FW_CHAR* fname, FW_INT32 fNo )
{
	FW_CHAR* find_ptr;
	FW_INT32 ret_code = 0;
	FW_CHAR drvname[VFS_DRV_NAME_MAX];
	FW_UINT32 fname_len;
	
	memset( &g_bf_vfs_file_use[file_cnt].drvname, 0, sizeof( g_bf_vfs_file_use[file_cnt].drvname ));
	memset( drvname, 0, sizeof( drvname ));

	
	find_ptr = strchr( fname, ':' );
	if (find_ptr != NULL) {
		/* Set drive name */
		strncpy( (FW_CHAR*)&g_bf_vfs_file_use[file_cnt].drvname, ( const FW_CHAR* )fname, ( find_ptr - fname ));
	}
	else {
		/* Get a drive name */
		ret_code = BF_Vfs_Get_Cur_Drv( drvname );
		if( ret_code < 0 ) {
			BF_Fs_User_Custom_Err_Printer((" [VFS][File Use Set] err=%d, fNo=%d\n", ret_code, fNo ));
			return ret_code;
		}
		/* Set drive name */
		fname_len = strlen( drvname );
		strncpy( (FW_CHAR*)&g_bf_vfs_file_use[file_cnt].drvname, ( const FW_CHAR* )drvname, fname_len );
	}
	
	/* Set file No. */
	g_bf_vfs_file_use[file_cnt].file_use = fNo;
	
	return ret_code;
}

/**
 * @brief Clear file using table.
 * @param i:file_cnt File counter.
 * @note none
 * @attention none.
 */
static FW_VOID vfs_clear_file_use_table( FW_UCHAR file_cnt )
{
	memset( ( FW_VOID* )&g_bf_vfs_file_use[file_cnt], 0, sizeof( g_bf_vfs_file_use[file_cnt] ) );

}

/**
 * @brief Clear file using table which related designated drive.
 * @param i:drvname Drive name.
 * @note none
 * @attention none.
 */
static FW_VOID vfs_clear_drive_info( FW_CHAR* drvname )
{
	FW_UCHAR file_cnt;
	
	for( file_cnt = 0; file_cnt < FILE_CNT_MAX; file_cnt++ ) {
		if( strcmp((const CHAR*)drvname, (const CHAR*)&g_bf_vfs_file_use[file_cnt].drvname ) == 0){
			vfs_clear_file_use_table( file_cnt );
		}
	}
}


#ifdef CO_LINUX_VERSION_ON
/**
 * @brief IPCU Ack Callback.
 * @param i:id IPCU ID
 * @param i:*data IPCU receive data top address
 * @param i:length IPCU receive data length
 * @note none
 * @attention none.
 */
static FW_VOID vfs_ipcu_event_ack_cb( UCHAR id, UCHAR *data, UINT32 length )
{
	FW_CHAR file_cnt, req_cnt;
	T_FS_VFS_INFO_TABLE_COMM* info_tbl_comm;
	
	info_tbl_comm = ( T_FS_VFS_INFO_TABLE_COMM* )data;
//	BF_Fs_User_Custom_Info_Printer(("	[[IPCU]]: Callback for IPCU acknowledge.  Info table num = %d\n",  info_tbl_comm->info_tbl_no));

	file_cnt = ( info_tbl_comm->info_tbl_no - 1 ) / REQ_CNT_MAX;
	req_cnt  = ( info_tbl_comm->info_tbl_no - 1 ) % REQ_CNT_MAX;
	
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_RESEIVE_ACK;
}

/**
 * @brief IPCU Response Callback.
 * @param i:id IPCU ID
 * @param i:*data IPCU receive data top address
 * @param i:length IPCU receive data length
 * @param i:count callback number of times
 * @param i:total_length IPCU receive data total length
 * @note none
 * @attention none.
 */
static FW_VOID vfs_ipcu_event_response_cb( UCHAR id, UCHAR *data, UINT32 length, UINT32 cont, UINT32 total_length )
{
	T_FS_VFS_INFO_TABLE_COMM* info_tbl_comm;
	FW_UCHAR file_cnt, req_cnt;

	info_tbl_comm = ( T_FS_VFS_INFO_TABLE_COMM* )data;

//	BF_Fs_User_Custom_Info_Printer(("	[[IPCU]]: Callback for IPCU response.  Info table num = %d\n",  info_tbl_comm->info_tbl_no));
	
	/* IPCU receive process */
	file_cnt = ( info_tbl_comm->info_tbl_no - 1 ) / REQ_CNT_MAX;
	req_cnt  = ( info_tbl_comm->info_tbl_no - 1 ) % REQ_CNT_MAX;
	
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_RECEIVE_RESPONSE;

	OS_User_Set_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ));
}
#endif // CO_LINUX_VERSION_ON

/**
 * @brief Request of writing for a remainder data
 * @param i:fileNo File ID
 * @param i:info_table_num Info TBL No.
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 vfs_send_remain_write_data( FW_SHORT info_table_num )
{
	T_FS_VFS_INFO_TABLE_WRITE* info_table_write;
	FW_UCHAR file_cnt, req_cnt;
	OS_USER_FLGPTN	flgptn;
	FW_UINT32 buff_addr;
	FW_INT32 ret_code;
	
	file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
	req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
	
	info_table_write = (T_FS_VFS_INFO_TABLE_WRITE*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	buff_addr = ( FW_UINT32 )( D_BF_FS_VFS_ADR_INFO_AREA + (D_BF_FS_VFS_BLK_SIZ_INF_TBL * (D_BF_FS_VFS_BLK_NUM_INF_TBL ))
			   + COMM_BUFF_MAX_SIZE * file_cnt );
	
	/* Set VFS common data */
	info_table_write->common_data.ope_cmd = D_BF_FS_VFS_WRITE;
	info_table_write->common_data.info_tbl_no = info_table_num;
	info_table_write->common_data.memory_select = 0;
	info_table_write->common_data.data_buffer_pointer = 0;
	
	/* Set VFS write data */
	info_table_write->file_id = g_bf_vfs_ctrl[file_cnt][req_cnt].file_id;
	info_table_write->count = COMM_BUFF_MAX_SIZE - g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size;
	info_table_write->buffer_pointer = ( FW_UINT32* )buff_addr;
#if 0
	BF_Fs_User_Custom_Info_Printer(("ope_cmd = 0x%04x, info_table_num = %d\n", info_table_write->common_data.ope_cmd, 
									info_table_write->common_data.info_tbl_no ));
	BF_Fs_User_Custom_Info_Printer(("file_id = 0x%08x, buffer_pointer = %08p, count = %d\n", info_table_write->file_id, 
									info_table_write->buffer_pointer, info_table_write->count ));
#endif
	/* Set VFS management table */
	g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr = buff_addr;
	g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size = 0;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state &= ~VFS_NOT_SEND;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	BF_L1l2cache_Clean_Flush_Addr( buff_addr, ( COMM_BUFF_MAX_SIZE - g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size ));
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state == VFS_FINISH ) {
			break;
		}
		OS_User_Dly_Tsk(1);
	}
	
	ret_code = info_table_write->error_code;
	
	if( ret_code == 0 ){
		BF_Fs_User_Custom_Info_Printer((" [VFS][Remain Write] Success fNo=%d, req=%u, write=%u.\n", info_table_write->file_id, info_table_write->count, info_table_write->length ));
	}
	else {
		BF_Fs_User_Custom_Err_Printer((" [VFS][Remain Write] err=%d, fNo=%d, req=%u, write=%u.\n", ret_code, info_table_write->file_id, info_table_write->count, info_table_write->length ));
		
		if( g_bf_vfs_write_error[file_cnt] != NULL ) {
			g_bf_vfs_write_error[file_cnt]( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_write->error_code );
		}
	}
	
	/* Clear process */
	vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
	
	return ret_code;
}

/**
 * @brief Get a information of remainder buffer.
 * @param i:fileNo File ID
 * @param o:file_cnt file count
 * @param o:req_cnt request count
 * @return none.
 * @note none
 * @attention none.
 */
static FW_VOID vfs_get_remain_info( FW_INT32 fileNo, FW_UCHAR* file_count, FW_UCHAR* req_count )
{
	FW_UCHAR file_cnt = 0;
	FW_UCHAR req_cnt = 0;

	*file_count = FILE_CNT_MAX;
	*req_count = REQ_CNT_MAX;

	/* Get information of a buffer where data is left */
	for( file_cnt = 0; file_cnt < FILE_CNT_MAX; file_cnt++ ) {
		if( g_bf_vfs_file_use[file_cnt].file_use == fileNo ) {
			*file_count = file_cnt;
			break;
		}
	}
	
	if( file_cnt != FILE_CNT_MAX ) {
		for( req_cnt = 0; req_cnt < REQ_CNT_MAX; req_cnt++ ) {
			if( g_bf_vfs_ctrl[file_cnt][req_cnt].used_flag == 1 ) {
				if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state & VFS_NOT_SEND ) {	/* write data exist ? */
					*req_count = req_cnt;
					break;
				}
			}
		}
	}
}

/**
 * @brief Send and receive write request.
 * @param i:fileNo File ID
 * @param i:addr Write address
 * @param i:count Write size
 * @param i:info_table_num Info TBL No.
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 vfs_send_receive_write_request( FW_INT32 fileNo, FW_VOID* addr, FW_ULONG count, FW_ULONG* length )
{
	T_FS_VFS_INFO_TABLE_WRITE* info_table_write;
	OS_USER_FLGPTN	flgptn;
	FW_SHORT info_table_num;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 write_size;
	FW_UINT32 buff_addr;
	FW_UINT32 copy_addr;
	FW_UINT32 copy_size;
	FW_INT32 ret_code = 0;
	FW_ULONG total_length = 0;
	
	*length = 0;
	
	/* shared memory or non shared memory ? */
	if( Sdram_Map_Is_Shared( (UINT32)addr )) {
		/*#####################
		  ##  shared memory  ##
		  #####################*/
		
		vfs_get_info_table_num_counter( fileNo, &info_table_num );
		info_table_write = (T_FS_VFS_INFO_TABLE_WRITE*)(D_BF_FS_VFS_ADR_INFO_AREA 
						 + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
		
		/* Initialize Info TBL */
		memset( info_table_write, 0, sizeof( T_FS_VFS_INFO_TABLE_WRITE ) );
		
		file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
		req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
		
		/* Set VFS common data */
		info_table_write->common_data.ope_cmd = D_BF_FS_VFS_WRITE;
		info_table_write->common_data.info_tbl_no = info_table_num;
		info_table_write->common_data.memory_select = 1;
		info_table_write->common_data.data_buffer_pointer = 1;

		/* Set VFS write data */
		info_table_write->file_id = fileNo;
		info_table_write->buffer_pointer = ( FW_UINT32* )addr;
		info_table_write->count = count;
#if 0
		BF_Fs_User_Custom_Info_Printer(("### vfs_send_receive_write_request : root = 1 ###\n"));
		BF_Fs_User_Custom_Info_Printer(("    ope_cmd = 0x%04x, info_table_num = %d\n", 
			info_table_write->common_data.ope_cmd, info_table_write->common_data.info_tbl_no ));
		BF_Fs_User_Custom_Info_Printer(("    file_id = 0x%08x\n", info_table_write->file_id ));
		BF_Fs_User_Custom_Info_Printer(("    buffer_pointer = %08p, count = %d\n", 
			info_table_write->buffer_pointer, info_table_write->count ));
#endif
		/* Set VFS management table */
		g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
		g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr = ( FW_INT32 )addr;
		g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size = 0;
		g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
		
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)addr, count );
		
		/* IPCU Send */
		OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );

		/* IPCU receive */
		OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
		
		while( 1 ) {
			if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state == VFS_FINISH ) { 
				break;
			}
			OS_User_Dly_Tsk(1);
		}
		
		ret_code = info_table_write->error_code;
		
		total_length = info_table_write->length;

		if( ret_code == 0 ){
//			BF_Fs_User_Custom_Info_Printer((" [VFS][Write][SH] Success fNo=%d, req=%lu, write=%lu%.\n", fileNo, info_table_write->count, info_table_write->length));
		}
		else {
			BF_Fs_User_Custom_Err_Printer((" [VFS][Write][SH] err=%d, fNo=%d, req=%u, write=%u.\n", ret_code, fileNo, info_table_write->count, info_table_write->length));
		}
		
		/* Clear process */
		vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
		
		*length = total_length;
		
		return ret_code;

	}
	else {
		/*#########################
		  ##  non shared memory  ##
		  #########################*/
		
		write_size = ( FW_INT32 )count;				/* designated write size	 */
		copy_addr = ( FW_UINT32 )addr;				/* designated write address  */
		
		while( write_size > 0 ) {
			/* Get information of remainder data */
			vfs_get_remain_info( fileNo, &file_cnt, &req_cnt );
			
			if( req_cnt == REQ_CNT_MAX ) {
				/* Remainder data exist */
				vfs_get_info_table_num_counter( fileNo, &info_table_num ); 
				file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
				req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
				
				buff_addr = ( FW_UINT32 )( D_BF_FS_VFS_ADR_INFO_AREA + (D_BF_FS_VFS_BLK_SIZ_INF_TBL * (D_BF_FS_VFS_BLK_NUM_INF_TBL ))
						   + COMM_BUFF_MAX_SIZE * file_cnt );
				g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr = buff_addr;
				g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size = COMM_BUFF_MAX_SIZE;
			}
			
			if( write_size >= g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size ) {
			/*### Send process ###*/
				/* Copy the write data to the vacant buffer area */
				buff_addr = g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr;
				copy_size = g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size;
				memcpy( ( FW_VOID* )buff_addr, ( const FW_VOID* )copy_addr, copy_size );
#if 0				
				printf("[VFS][non shared]write_size = 0x%x, copy_addr = 0x%x, copy_size = 0x%x\n", write_size, copy_addr, copy_size ); // for debug H.Ito
				printf("                 file_cnt = %d, req_cnt = %d\n", file_cnt, req_cnt );	// for debug H.Ito
#endif				
				g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size += ( COMM_BUFF_MAX_SIZE - g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size );
				
				/*## Send the remainder data ##*/
				info_table_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + req_cnt + 1 );
				
				/* Set Info TBL address and buffer address */
				info_table_write = (T_FS_VFS_INFO_TABLE_WRITE*)(D_BF_FS_VFS_ADR_INFO_AREA
								 + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
				
				/* Initialize Info TBL */
				memset( info_table_write, 0, sizeof( T_FS_VFS_INFO_TABLE_WRITE ) );	
				
				/* Set VFS common data to Info TBL */
				info_table_write->common_data.ope_cmd = D_BF_FS_VFS_WRITE;
				info_table_write->common_data.info_tbl_no = info_table_num;
				info_table_write->common_data.memory_select = 0;		
				info_table_write->common_data.data_buffer_pointer = 0;	
				
				/* Set VFS write data */
				buff_addr = ( FW_UINT32 )( D_BF_FS_VFS_ADR_INFO_AREA + (D_BF_FS_VFS_BLK_SIZ_INF_TBL * (D_BF_FS_VFS_BLK_NUM_INF_TBL ))
					   + COMM_BUFF_MAX_SIZE * file_cnt );
					   
				info_table_write->file_id = fileNo;
				info_table_write->buffer_pointer = ( FW_UINT32* )buff_addr;
				info_table_write->count = COMM_BUFF_MAX_SIZE;
				
				/* Set VFS management table */
				g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
				g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr = buff_addr;
				g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state &= ~VFS_NOT_SEND;
				g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
				
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)buff_addr, copy_size );
				
				/* IPCU Send */
				OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );
				
				/* Update write information */
				copy_addr = ( FW_UINT32 )( copy_addr + copy_size );
				write_size -= ( FW_INT32 )copy_size;

				/* IPCU receive */
				OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
				OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ) );
				
				while( 1 ) {
					if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state == VFS_FINISH ) {
						break;
					}
					OS_User_Dly_Tsk(1);
				}
				
				ret_code = info_table_write->error_code;
				
				total_length += info_table_write->length;

				if( ret_code == 0 ){
//					BF_Fs_User_Custom_Info_Printer((" [VFS][Write][NSH] Success fNo=%d, req=%lu, write=%lu%.\n", fileNo, count, info_table_write->length));
				}
				else {
					BF_Fs_User_Custom_Err_Printer((" [VFS][Write][NSH] err=%d, fNo=%d, req=%lu, write=%lu.\n", ret_code, fileNo, count, total_length ));
				}
				
				vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
				
				if(( write_size <= 0 ) || (  ret_code < 0  )) {
					/* Clear VFS control management table */
					memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );
					
					*length = total_length;
					return ret_code;
				}
			}
			else {
			/*### Save process ###*/
				/* Save the write data temporarily 	*/
				if( g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size == COMM_BUFF_MAX_SIZE ) {
					buff_addr = ( FW_UINT32 )( D_BF_FS_VFS_ADR_INFO_AREA + (D_BF_FS_VFS_BLK_SIZ_INF_TBL * (D_BF_FS_VFS_BLK_NUM_INF_TBL ))
							   + COMM_BUFF_MAX_SIZE * file_cnt );
					g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr = buff_addr;
				}
				else{
					buff_addr = g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr;
				}
				
				/* Set VFS management table */
				memcpy( ( FW_VOID* )buff_addr, ( const FW_VOID* )copy_addr, write_size );
				g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
				g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size -= write_size;
				g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state = VFS_NOT_SEND;
				g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr += write_size;
				
				/* Set total length to writing length */
				total_length += write_size;
				
				*length = total_length;
				
				BF_Fs_User_Custom_Info_Printer((" [VFS][Write][NSH] Remain Info: fNo=%d, req=%lu, write=%lu, remain=%u.\n", fileNo, count, (total_length - write_size), write_size));

				return ret_code;
			}
		}
		*length = total_length;
		
		return ret_code;
	}
}


/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief VFS sender task [TASK]
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_VOID VFS_sender(INT32 stacd)
{
#ifdef CO_LINUX_VERSION_ON
	T_FS_VFS_INFO_TABLE_COMM* info_tbl_comm;
	OS_USER_FLGPTN	flgptn;
	FW_UCHAR		ipcu_id;
	FW_INT32		retry_cnt;
	FJ_ERR_CODE		ret;

	g_bf_vfs_receive_flg_id[FILE_CNT_MAX + 1] = { FID_FS_VFS_RECEIVE0, FID_FS_VFS_RECEIVE1, FID_FS_VFS_RECEIVE2,
													FID_FS_VFS_RECEIVE3, FID_FS_VFS_RECEIVE4, FID_FS_VFS_RECEIVE5,
													FID_FS_VFS_RECEIVE6, FID_FS_VFS_RECEIVE7, FID_FS_VFS_RECEIVE8 };

	BF_IPCU_Open( E_FJ_IPCU_MAILBOX_TYPE_2, &ipcu_id );
 	BF_IPCU_SetResCB( ipcu_id, vfs_ipcu_event_ack_cb );

	memset( (FW_VOID*)g_bf_vfs_ctrl, 0, sizeof( g_bf_vfs_ctrl ) ); 
	memset( (FW_VOID*)g_bf_vfs_file_use, 0, sizeof( g_bf_vfs_file_use ) ); 

	/* Wait for a IPCU request from FS task */
	while( 1 ) {
		OS_User_Twai_Flg( FID_FS_VFS_SEND, FLG_FS_VFS_SEND, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );	
		
//		BF_Fs_User_Custom_Info_Printer((" [VFS][Send] IPCU Request on VFS_sender task.  ipcu_id = %d\n", ipcu_id ));
		
		info_tbl_comm = (T_FS_VFS_INFO_TABLE_COMM*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * flgptn ));	/* flgptn = Info Num(1 - 72) */
		OS_User_Clr_Flg( FID_FS_VFS_SEND, ~flgptn );
		
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)info_tbl_comm, D_BF_FS_VFS_BLK_SIZ_INF_TBL );
		
//		BF_Fs_User_Custom_Info_Printer((" [VFS][Send] Info table num = %d, Operation command = 0x%x\n", info_tbl_comm->info_tbl_no, info_tbl_comm->ope_cmd));
		
		/* Send command with retry by IPCU */
		for( retry_cnt = 0; retry_cnt < FS_VFS_SEND_RETRY_MAX; retry_cnt++ ) {
			ret = BF_IPCU_Send(ipcu_id, (FW_UCHAR*)info_tbl_comm, D_BF_FS_VFS_BLK_SIZ_INF_TBL, 0 );
			if( ret == FJ_ERR_NG_COMMUNICATON ) {
				BF_Fs_User_Custom_Wrn_Printer((" [VFS][Send] Wait for ready status (Retry=%d)\n", retry_cnt));
				OS_User_Dly_Tsk(100);
			}
			else if( ret == FJ_ERR_OK ) {
//				BF_Fs_User_Custom_Info_Printer((" [VFS][Send] success.\n"));
				break;
			}
			else if( ret == FJ_ERR_NG_CONDITION ) {
				BF_Fs_User_Custom_Err_Printer((" [VFS][Send] Failed: condition error.\n"));
				break;
			}
			else {
				BF_Fs_User_Custom_Err_Printer((" [VFS][Send] Failed: parameter error.\n"));
				break;
			}
		}
	}
#endif // CO_LINUX_VERSION_ON
}

/**
 * @brief VFS receiver task [TASK]
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_VOID VFS_receiver(INT32 stacd)
{
#ifdef CO_LINUX_VERSION_ON
	FW_UCHAR ipcu_id;
	
	BF_IPCU_Open( E_FJ_IPCU_MAILBOX_TYPE_3, &ipcu_id );
	BF_IPCU_SetReqCB( ipcu_id, vfs_ipcu_event_response_cb );
#endif // CO_LINUX_VERSION_ON
}

FW_INT32 BF_Vfs_Open(const FW_CHAR* fname, FW_USHORT mode, FW_USHORT amode)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_OPEN* info_table_open;
	FW_UINT32 fname_len;
	FW_INT32 fNo = 0;
	OS_USER_FLGPTN flgptn;
	FW_UCHAR file_cnt, req_cnt;
	FW_UCHAR file_use_cnt = 0;
	FW_INT32 ret_code = 0;
	FW_UCHAR temp_file_cnt = 0xFF;
	
	/* Check whether a open request can accept. */
	file_use_cnt = vfs_get_file_use_table();
	
	if( file_use_cnt != FILE_CNT_MAX ) {
		/* Set temporary fNo to file using table */
		ret_code = vfs_set_file_use_table( file_use_cnt, fname, temp_file_cnt );
		if( ret_code < 0 ) {
			BF_Fs_User_Custom_Err_Printer((" [VFS] Error in File using table set by OPEN request [fname = %s].\n", fname ));
		}
	}
	else{
		return D_BF_FS_VFS_NO_MORE_OPEN;
	}
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );  
	
	info_table_open = ( T_FS_VFS_INFO_TABLE_OPEN* )( D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_open, 0, sizeof( T_FS_VFS_INFO_TABLE_OPEN ) );
	
	/* Set VFS common data to Info TBL */
	info_table_open->common_data.ope_cmd = D_BF_FS_VFS_OPEN;
	info_table_open->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS open data to Info TBL */
	info_table_open->mode = mode;
	info_table_open->access_mode = amode;
	
	fname_len = strlen(fname);
	strncpy( info_table_open->file_name, fname, fname_len );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_open->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	if( ret_code > 0 ) {
		/* Set fNo to file using table */
		vfs_set_file_use_table( file_use_cnt, fname, ret_code );
	}
	else{
		/* Claer fNo to file using table */
		vfs_clear_file_use_table( file_use_cnt );
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Close(FW_INT32 fileNo)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_CLOSE* info_table_close;
	OS_USER_FLGPTN flgptn;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	/* Get information of remainder data */
	vfs_get_remain_info( fileNo, &file_cnt, &req_cnt );

	if( req_cnt != REQ_CNT_MAX ) {		/* Is there a remainder data ? */
		info_table_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + req_cnt + 1 );
		
		/* Send remainder data */
		ret_code = vfs_send_remain_write_data( info_table_num );
		
		if( ret_code < 0 ) {
			BF_Fs_User_Custom_Err_Printer((" [VFS] Error in Remain write proc by CLOSE request [fNo=%d].\n", fileNo ));
		}
		
		vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
		
	}
	
	/* Start Close process */
	vfs_get_info_table_num_counter( fileNo, &info_table_num );
	
	info_table_close = (T_FS_VFS_INFO_TABLE_CLOSE*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_close, 0, sizeof( T_FS_VFS_INFO_TABLE_CLOSE ) );
	
	/* Set VFS common data to Info TBL */
	info_table_close->common_data.ope_cmd = D_BF_FS_VFS_CLOSE;
	info_table_close->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS close data to Info TBL */
	info_table_close->file_id = fileNo;
	
	/* Set VFS management table */
	file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
	req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_close->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	if( g_bf_vfs_write_error[file_cnt] != NULL ) {
		g_bf_vfs_write_error[file_cnt] = 0;
	}
	vfs_clear_info_table_num_counter( fileNo, info_table_num );
	vfs_clear_file_use_table( file_cnt );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Read(FW_INT32 fileNo, FW_VOID* addr, FW_ULONG count, FW_ULONG* length)
{
	FW_SHORT info_table_num = 0;
	T_FS_VFS_INFO_TABLE_READ* info_table_read;
	OS_USER_FLGPTN flgptn;
	FW_INT32 ret_code = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_UINT32 buff_addr = 0;
	FW_UINT32 copy_addr = 0;
	FW_UINT32 copy_size = 0;
	FW_INT32 read_size = 0;
	FW_ULONG total_length = 0;
	
	*length = 0;

	/* Get information of remainder data */
	vfs_get_remain_info( fileNo, &file_cnt, &req_cnt );
	
	if( req_cnt != REQ_CNT_MAX ) {		/* Is there a remainder data ? */
		/* Set write buffer for non shared memory */
		info_table_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + req_cnt + 1 );
		
		/* Send remainder data */
		ret_code = vfs_send_remain_write_data( info_table_num );

		if( ret_code < 0 ) {
			BF_Fs_User_Custom_Err_Printer((" [VFS] Error in Remain write proc by READ request [fNo=%d].\n", fileNo ));
		}
		vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
		
	}
	
	/* Start Read process */
	
	/* shared memory or non shared memory ? */
	if( Sdram_Map_Is_Shared( (UINT32)addr )) {
		
		/*#####################
		  ##  shared memory  ##
		  #####################*/
		
		vfs_get_info_table_num_counter( fileNo, &info_table_num );
		info_table_read = (T_FS_VFS_INFO_TABLE_READ*)(D_BF_FS_VFS_ADR_INFO_AREA 
						+ ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
		
		/* Initialize Info TBL */
		memset( info_table_read, 0, sizeof( T_FS_VFS_INFO_TABLE_READ ) );
		
		/* Set VFS common data to Info TBL */
		info_table_read->common_data.ope_cmd = D_BF_FS_VFS_READ;
		info_table_read->common_data.info_tbl_no = info_table_num;
		info_table_read->common_data.memory_select = 1;	
		info_table_read->common_data.data_buffer_pointer = 1;
		
		/* Set VFS read data to Info TBL */
		info_table_read->file_id = fileNo;
		info_table_read->count = count;
		info_table_read->buffer_pointer = (FW_UINT32 *)addr;	
		
		/* Set VFS management table */
		file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
		req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
		g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
		g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr = ( FW_INT32 )addr;
		g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size = 0;
		g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
		
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)addr, count );
		
		/* IPCU Send */
		OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
		
		/* IPCU Receive */
		OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
		
		while( 1 ) {
			if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state == VFS_FINISH ) { 
				break;
			}
			OS_User_Dly_Tsk(1);
		}
		
		/* Update total read size by IPCU responce */
		total_length += info_table_read->length;
		
		/* Check operation result */ 
		ret_code = info_table_read->error_code;
		if( ret_code < 0 ) {
			if( ret_code == D_BF_FS_VFS_EOF ){
				BF_Fs_User_Custom_Wrn_Printer((" [VFS][Read][SH] EOF: fNo=%d, req=%lu, read=%lu.\n", fileNo, count, total_length));
			}
			else {
				BF_Fs_User_Custom_Err_Printer((" [VFS][Read][SH] err=%d, fNo=%d, req=%lu, read=%lu.\n", ret_code, fileNo, count, total_length));
			}
			/* Clear process */
			vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
			memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
			
			*length = total_length;	/* Set output information */
			
			return ret_code;
		}
		
		/* Clear process */
		vfs_clear_info_table_num_counter( fileNo, info_table_num );
		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
		
		*length = total_length;	/* Set output information */
		
		return ret_code;
	}
	else {
		/*#########################
		  ##  non shared memory  ##
		  #########################*/

		read_size = ( FW_INT32 )count;						/* copy size from VFS buff to designated write address */
		copy_addr = ( FW_UINT32 )addr;						/* designated write size	*/
		
		/* IPCU Send receive loop */
		while( read_size > 0 ) {
			/* Get buffer info. */
			vfs_get_info_table_num_counter( fileNo, &info_table_num );  									/* Get another Info tBL No. for 2nd */
			
			info_table_read = (T_FS_VFS_INFO_TABLE_READ*)(D_BF_FS_VFS_ADR_INFO_AREA 
							 + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
			
			memset( info_table_read, 0, sizeof( T_FS_VFS_INFO_TABLE_READ ) );								/* Initialize Info TBL */
			
			file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
			
			buff_addr = ( FW_UINT32 )( D_BF_FS_VFS_ADR_INFO_AREA + (D_BF_FS_VFS_BLK_SIZ_INF_TBL * (D_BF_FS_VFS_BLK_NUM_INF_TBL ))
					   + COMM_BUFF_MAX_SIZE * file_cnt );
			
			BF_L1l2cache_Clean_Flush_Addr( (UINT32)buff_addr, COMM_BUFF_MAX_SIZE );
			
			/* Set VFS common data to Info TBL */
			info_table_read->common_data.ope_cmd = D_BF_FS_VFS_READ;
			info_table_read->common_data.info_tbl_no = info_table_num;
			
			/* Set VFS Read data */
			if( read_size > COMM_BUFF_MAX_SIZE ) {
				copy_size = COMM_BUFF_MAX_SIZE;
			}
			else {
				copy_size = read_size;
			}
			
			info_table_read->file_id = fileNo;
			info_table_read->buffer_pointer = ( FW_UINT32 * )buff_addr;
			info_table_read->count = copy_size;
			
#if 0		
			BF_Fs_User_Custom_Info_Printer(("### BF_Vfs_Read ###\n"));
			BF_Fs_User_Custom_Info_Printer(("    ope_cmd = 0x%04x, info_table_num = %d\n", 
				info_table_read->common_data.ope_cmd, info_table_read->common_data.info_tbl_no ));
			BF_Fs_User_Custom_Info_Printer(("    file_id = 0x%08x\n", info_table_read->file_id ));
			BF_Fs_User_Custom_Info_Printer(("    buffer_pointer = %08p, count = %d\n", 
				info_table_read->buffer_pointer, info_table_read->count ));
#endif		
			/* Set VFS management table */
			file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
			req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
			g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
			g_bf_vfs_ctrl[file_cnt][req_cnt].buff_addr = buff_addr;
			g_bf_vfs_ctrl[file_cnt][req_cnt].remain_size = 0;
			g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
			
			/* IPCU Send */
			OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );

			/* IPCU receive */
			OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
			OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ) );
			
			while( 1 ) {
				if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state == VFS_FINISH ) { 
					break;
				}
				OS_User_Dly_Tsk(1);
			}
			
			/* Update total read size by IPCU responce */
			total_length += info_table_read->length;
			
			/* Clear process */
			vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
			memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
			
			/* Check operation result */ 
			ret_code = info_table_read->error_code;
			if( ret_code < 0 ) {
				if( ret_code == D_BF_FS_VFS_EOF ){
					BF_Fs_User_Custom_Wrn_Printer((" [VFS][Read][NSH] EOF: fNo=%d, req=%lu, read=%lu.\n", fileNo, count, total_length));
				}
				else {
					BF_Fs_User_Custom_Err_Printer((" [VFS][Read][NSH] err=%d, fNo=%d, req=%lu, read=%lu.\n", ret_code, fileNo, count, total_length));
				}
				
				*length = total_length;	/* Set output information */
				
				return ret_code;
			}
			
			memcpy( ( FW_VOID* )copy_addr, ( const FW_VOID* )buff_addr, copy_size );
			BF_L1l2cache_Clean_Flush_Addr( (UINT32)copy_addr, copy_size );
			
			copy_addr = ( FW_UINT32 )( copy_addr + COMM_BUFF_MAX_SIZE );
			read_size -= ( FW_INT32 )copy_size;
		}
		
		/* Clear process */
//		vfs_clear_info_table_num_counter( fileNo, info_table_num );
//		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );		/* Clear VFS control management table */
		
		*length = total_length;	/* Set output information */
		
		return ret_code;
	}
}

FW_INT32 BF_Vfs_Write(FW_INT32 fileNo, const FW_VOID* addr, FW_ULONG count, FW_ULONG* length)
{
	FW_INT32 ret_code;

	/* IPCU Send and Receive*/
	ret_code = vfs_send_receive_write_request( fileNo, (FW_VOID*)addr, count, length );	
	
	return ret_code;
}

FW_INT32 BF_Vfs_Lseek(FW_INT32 fileNo, FW_INT32 offset, FW_USHORT whence, FW_ULONG* length)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_LSEEK* info_table_lseek;
	OS_USER_FLGPTN flgptn;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	/* Get information of remainder data */
	vfs_get_remain_info( fileNo, &file_cnt, &req_cnt );

	if( req_cnt != REQ_CNT_MAX ) {	/* Is there a remainder data ? */
		info_table_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + req_cnt + 1 );
		
		/* Send remainder data */
		ret_code = vfs_send_remain_write_data( info_table_num );
		
		if( ret_code < 0 ) {
			BF_Fs_User_Custom_Err_Printer((" [VFS] Error in Remain write proc by LSEEK request [fNo=%d].\n", fileNo ));
		}
		
		vfs_clear_info_table_num_counter( fileNo, info_table_num );
		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
		
	}
	
	/* Start Lseek process */
	vfs_get_info_table_num_counter( fileNo, &info_table_num );
	
	info_table_lseek = (T_FS_VFS_INFO_TABLE_LSEEK*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
	req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
	
	memset( info_table_lseek, 0, sizeof( T_FS_VFS_INFO_TABLE_LSEEK ) );
	
	/* Set VFS common data to Info TBL */
	info_table_lseek->common_data.ope_cmd = D_BF_FS_VFS_LSEEK;
	info_table_lseek->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS seek data  to Info TBL */
	info_table_lseek->file_id = fileNo;
	info_table_lseek->offset = offset;
	info_table_lseek->whence = whence;
	
	/* Set VFS management table */
	g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	*length = info_table_lseek->length;
	ret_code = info_table_lseek->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fileNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );

	return ret_code;
}

FW_INT32 BF_Vfs_Lseek64(FW_INT32 fileNo, FW_INT64 offset, FW_USHORT whence, FW_INT64* length)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_LSEEK64* info_table_lseek64;
	OS_USER_FLGPTN flgptn;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	/* Get information of remainder data */
	vfs_get_remain_info( fileNo, &file_cnt, &req_cnt );

	if( req_cnt != REQ_CNT_MAX ) {	/* Is there a remainder data ? */
		info_table_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + req_cnt + 1 );

		/* Send remainder data */
		ret_code = vfs_send_remain_write_data( info_table_num );
		
		if( ret_code < 0 ) {
			BF_Fs_User_Custom_Err_Printer((" [VFS] Error in Remain write proc by LSEEK64 request [fNo=%d].\n", fileNo ));
		}

		vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
		
	}
	
	/* Start Lseek64 process */
	vfs_get_info_table_num_counter( fileNo, &info_table_num );

	info_table_lseek64 = (T_FS_VFS_INFO_TABLE_LSEEK64*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
	req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
	
	memset( info_table_lseek64, 0, sizeof( T_FS_VFS_INFO_TABLE_LSEEK64 ) );
	
	/* Set VFS common data to Info TBL */
	info_table_lseek64->common_data.ope_cmd = D_BF_FS_VFS_LSEEK64;
	info_table_lseek64->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS common data to Info TBL */
	info_table_lseek64->file_id = fileNo;
	info_table_lseek64->offset = offset;
	info_table_lseek64->whence = whence;
	
	/* Set VFS management table */
	g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	*length = info_table_lseek64->length;
	ret_code = info_table_lseek64->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fileNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Start_MDF( FW_VOID )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_START_MDF* info_table_start_mdf;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;

	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_start_mdf = (T_FS_VFS_INFO_TABLE_START_MDF*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_start_mdf, 0, sizeof( T_FS_VFS_INFO_TABLE_START_MDF ) );
	
	/* Set VFS common data to Info TBL */
	info_table_start_mdf->common_data.ope_cmd = D_BF_FS_VFS_START_MDF; 
	info_table_start_mdf->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_start_mdf->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Init_Drv( FW_CHAR* drvname, FW_INT32 mediaid )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_INIT_DRV* info_table_init_drv;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 dname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_init_drv = (T_FS_VFS_INFO_TABLE_INIT_DRV*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_init_drv, 0, sizeof( T_FS_VFS_INFO_TABLE_INIT_DRV ) );
	
	/* Set VFS common data to Info TBL */
	info_table_init_drv->common_data.ope_cmd = D_BF_FS_VFS_INIT_DRV; 
	info_table_init_drv->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS init drv data to Info TBL */
	info_table_init_drv->media_id = mediaid;
	dname_len = strlen(drvname);
	strncpy( info_table_init_drv->drive_name, drvname, dname_len);
//	memcpy( info_table_init_drv->drive_name, drvname, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );

	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_init_drv->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_End_Drv( FW_CHAR* drvname )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_END_DRV* info_table_end_drv;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 dname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_end_drv = (T_FS_VFS_INFO_TABLE_END_DRV*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_end_drv, 0, sizeof( T_FS_VFS_INFO_TABLE_END_DRV ) );
	
	/* Set VFS common data to Info TBL */
	info_table_end_drv->common_data.ope_cmd = D_BF_FS_VFS_END_DRV; 
	info_table_end_drv->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS init drv data to Info TBL */
	dname_len = strlen(drvname);
	strncpy( info_table_end_drv->drive_name, drvname, dname_len);
//	memcpy( info_table_end_drv->drive_name, drvname, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_end_drv->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	vfs_clear_drive_info( drvname );

	return ret_code;
}

FW_INT32 BF_Vfs_Stat_VFS( FW_CHAR* drvname, T_BF_FS_IF_STATVFS* buf )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_STAT_VFS* info_table_stat_vfs;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 dname_len;

	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_stat_vfs = (T_FS_VFS_INFO_TABLE_STAT_VFS*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_stat_vfs, 0, sizeof( T_FS_VFS_INFO_TABLE_STAT_VFS ) );
	
	/* Set VFS common data to Info TBL */
	info_table_stat_vfs->common_data.ope_cmd = D_BF_FS_VFS_STATVFS; 
	info_table_stat_vfs->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS stat data to Info TBL */
	dname_len = strlen(drvname);
	strncpy( info_table_stat_vfs->drive_name, drvname, dname_len);
//	memcpy( info_table_stat_vfs->drive_name, drvname, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;

	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	buf->blksize = info_table_stat_vfs->block_size;
	buf->blocks = info_table_stat_vfs->block_nums;
	buf->bfree = info_table_stat_vfs->block_free;
	buf->fstype = info_table_stat_vfs->fs_type;
	buf->dstat = info_table_stat_vfs->drive_stat;
	
	ret_code = info_table_stat_vfs->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Get_Free_Space( FW_CHAR* drvname, FW_UINT32* blksize, FW_UINT32* blocks, FW_UINT32* bfree )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_GET_FREE_SPACE* info_table_get_free_space;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 dname_len;

	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_get_free_space = (T_FS_VFS_INFO_TABLE_GET_FREE_SPACE*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_get_free_space, 0, sizeof( T_FS_VFS_INFO_TABLE_GET_FREE_SPACE ) );
	
	/* Set VFS common data to Info TBL */
	info_table_get_free_space->common_data.ope_cmd = D_BF_FS_VFS_GET_FREESPACE; 
	info_table_get_free_space->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS get free space data to Info TBL */
	dname_len = strlen(drvname);
	strncpy( info_table_get_free_space->drive_name, drvname, dname_len);
//	memcpy( info_table_get_free_space->drive_name, drvname, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	*blksize = info_table_get_free_space->block_size;
	*blocks = info_table_get_free_space->block_nums;
	*blksize = info_table_get_free_space->block_free;
	
	ret_code = info_table_get_free_space->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Get_Cur_Drv( FW_CHAR* drvname )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_GET_CUR_DRV* info_table_get_cur_drv;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 dname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_get_cur_drv = (T_FS_VFS_INFO_TABLE_GET_CUR_DRV*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_get_cur_drv, 0, sizeof( T_FS_VFS_INFO_TABLE_GET_CUR_DRV ) );
	
	/* Set VFS common data to Info TBL */
	info_table_get_cur_drv->common_data.ope_cmd = D_BF_FS_VFS_GET_CURDRV; 
	info_table_get_cur_drv->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	dname_len = strlen(info_table_get_cur_drv->drive_name);
	strncpy( drvname, info_table_get_cur_drv->drive_name, dname_len);
//	memcpy( drvname, info_table_get_cur_drv->drive_name, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );
	
	ret_code = info_table_get_cur_drv->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Set_Vol_Label( FW_CHAR* drvname, FW_CHAR* label )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_SET_VOL_LABEL* info_table_set_vol_label;
	FW_UINT32 label_len;
	FW_UINT32 dname_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_set_vol_label = (T_FS_VFS_INFO_TABLE_SET_VOL_LABEL*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_set_vol_label, 0, sizeof( T_FS_VFS_INFO_TABLE_SET_VOL_LABEL ) );
	
	/* Set VFS common data to Info TBL */
	info_table_set_vol_label->common_data.ope_cmd = D_BF_FS_VFS_SET_VOLLABEL; 
	info_table_set_vol_label->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS set vol label data to Info TBL */
	dname_len = strlen(drvname);
	strncpy( info_table_set_vol_label->drive_name, drvname, dname_len);
//	memcpy( info_table_set_vol_label->drive_name, drvname, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );

	label_len = strlen(label);
	strncpy( info_table_set_vol_label->vol_label, label, label_len );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_set_vol_label->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Get_Vol_Label( FW_CHAR* drvname, FW_CHAR* label )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_GET_VOL_LABEL* info_table_get_vol_label;
	FW_UINT32 label_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 dname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_get_vol_label = (T_FS_VFS_INFO_TABLE_GET_VOL_LABEL*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_get_vol_label, 0, sizeof( T_FS_VFS_INFO_TABLE_GET_VOL_LABEL ) );
	
	/* Set VFS common data to Info TBL */
	info_table_get_vol_label->common_data.ope_cmd = D_BF_FS_VFS_GET_VOLLABEL; 
	info_table_get_vol_label->common_data.info_tbl_no = info_table_num;
	
	dname_len = strlen(drvname);
	strncpy( info_table_get_vol_label->drive_name, drvname, dname_len);
//	memcpy( info_table_get_vol_label->drive_name, drvname, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	label_len = strlen(info_table_get_vol_label->vol_label);
	strncpy( label, info_table_get_vol_label->vol_label, label_len );
	
	ret_code = info_table_get_vol_label->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Fsync( FW_INT32 fileNo )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_FSYNC* info_table_fsync;
	OS_USER_FLGPTN flgptn;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	/* Get information of remainder data */
	vfs_get_remain_info( fileNo, &file_cnt, &req_cnt );
	
	if( req_cnt != REQ_CNT_MAX ) {	/* Is there a remainder data ? */
		info_table_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + req_cnt + 1 );
		
		/* Send remainder data */
		ret_code = vfs_send_remain_write_data( info_table_num );
		
		if( ret_code < 0 ) {
			BF_Fs_User_Custom_Err_Printer((" [VFS] Error in Remain write proc by FSYNC request [fNo=%d].\n", fileNo ));
		}
		
		vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
		
	}
	
	/* Start Fsync process */
	vfs_get_info_table_num_counter( fileNo, &info_table_num );
	
	info_table_fsync = (T_FS_VFS_INFO_TABLE_FSYNC*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	/* Set VFS management table */
	g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fileNo;
	
	memset( info_table_fsync, 0, sizeof( T_FS_VFS_INFO_TABLE_FSYNC ) );
	
	/* Set VFS common data to Info TBL */
	info_table_fsync->common_data.ope_cmd = D_BF_FS_VFS_FSYNC; 
	info_table_fsync->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS fsync data to Info TBL */
	info_table_fsync->file_id = fileNo;
	
	/* Set VFS management table */
	file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
	req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_fsync->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fileNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Sync( FW_CHAR* drvname )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_SYNC* info_table_sync;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;

	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_sync = (T_FS_VFS_INFO_TABLE_SYNC*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_sync, 0, sizeof( T_FS_VFS_INFO_TABLE_SYNC ) );
	
	/* Set VFS common data to Info TBL */
	info_table_sync->common_data.ope_cmd = D_BF_FS_VFS_SYNC; 
	info_table_sync->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS sync data to Info TBL */
	memcpy( info_table_sync->drive_name, drvname, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_sync->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Set_Protection_Mode( FW_CHAR* drvname, FW_UINT32 mode )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_SET_PROTECTION_MODE* info_table_get_protection_mode;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 dname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_get_protection_mode = (T_FS_VFS_INFO_TABLE_SET_PROTECTION_MODE*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_get_protection_mode, 0, sizeof( T_FS_VFS_INFO_TABLE_SET_PROTECTION_MODE ) );
	
	/* Set VFS common data to Info TBL */
	info_table_get_protection_mode->common_data.ope_cmd = D_BF_FS_VFS_SET_PROTECTION_MODE; 
	info_table_get_protection_mode->common_data.info_tbl_no = info_table_num;
	info_table_get_protection_mode->mode = mode;
	
	/* Set VFS protection mode data to Info TBL */
	dname_len = strlen(drvname);
	strncpy( info_table_get_protection_mode->drive_name, drvname, dname_len);
//	memcpy( info_table_get_protection_mode->drive_name, drvname, sizeof(D_BF_FS_VFS_INFO_DRV_NAME_MAX) );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_get_protection_mode->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Rename( FW_CHAR* srcname, FW_CHAR* dstname )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_RENAME* info_table_rename;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 fname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_rename = (T_FS_VFS_INFO_TABLE_RENAME*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_rename, 0, sizeof( T_FS_VFS_INFO_TABLE_RENAME ) );
	
	/* Set VFS common data to Info TBL */
	info_table_rename->common_data.ope_cmd = D_BF_FS_VFS_RENAME; 
	info_table_rename->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS rename data to Info TBL */
	fname_len = strlen(srcname);
	strncpy( info_table_rename->src_name, srcname, fname_len);
//	memcpy( info_table_rename->src_name, srcname, D_BF_FS_VFS_FNAME_MAX );
	fname_len = strlen(dstname);
	strncpy( info_table_rename->dst_name, dstname, fname_len);
//	memcpy( info_table_rename->dst_name, dstname, D_BF_FS_VFS_FNAME_MAX );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_rename->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Stat( FW_CHAR* srcname, T_BF_FS_IF_STAT* sbuf )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_STAT* info_table_stat;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 fname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_stat = (T_FS_VFS_INFO_TABLE_STAT*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_stat, 0, sizeof( T_FS_VFS_INFO_TABLE_STAT ) );
	
	/* Set VFS common data to Info TBL */
	info_table_stat->common_data.ope_cmd = D_BF_FS_VFS_STAT; 
	info_table_stat->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS stat data to Info TBL */
	fname_len = strlen(srcname);
	strncpy( info_table_stat->file_name, srcname, fname_len);
//	memcpy( info_table_stat->file_name, srcname, D_BF_FS_VFS_FNAME_MAX );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	sbuf->m_date = info_table_stat->create_date;
	sbuf->m_time = info_table_stat->create_time;
	sbuf->date = info_table_stat->modify_date;
	sbuf->time = info_table_stat->modify_time;
	sbuf->fileSize = info_table_stat->size;
	
	ret_code = info_table_stat->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}


FW_INT32 BF_Vfs_Set_Attribute( FW_CHAR* fname, FW_INT32 attributes )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_SET_ATTRIBUTE* info_table_set_attribute;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 fname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_set_attribute = (T_FS_VFS_INFO_TABLE_SET_ATTRIBUTE*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_set_attribute, 0, sizeof( T_FS_VFS_INFO_TABLE_SET_ATTRIBUTE ) );
	
	/* Set VFS common data to Info TBL */
	info_table_set_attribute->common_data.ope_cmd = D_BF_FS_VFS_SET_ATTRIBUTE; 
	info_table_set_attribute->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS set attribute data to Info TBL */
	fname_len = strlen(fname);
	strncpy( info_table_set_attribute->file_name, fname, fname_len);
//	memcpy( info_table_set_attribute->file_name, fname, D_BF_FS_VFS_FNAME_MAX );
	info_table_set_attribute->access_mode = attributes;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_set_attribute->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}


FW_INT32 BF_Vfs_Set_Attribute_Multi( FW_CHAR* filename_list[][D_BF_FS_VFS_SETATTRIBUTE_MULTI_SHORTNAME_MAX], FW_UCHAR* attributes, FW_UCHAR* attr_mode, FW_INT32* fnum )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_SET_ATTRIBUTE_MULTI* info_table_set_attribute_multi;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;

	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_set_attribute_multi = (T_FS_VFS_INFO_TABLE_SET_ATTRIBUTE_MULTI*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_set_attribute_multi, 0, sizeof( T_FS_VFS_INFO_TABLE_SET_ATTRIBUTE_MULTI ) );
	
	/* Set VFS common data to Info TBL */
	info_table_set_attribute_multi->common_data.ope_cmd = D_BF_FS_VFS_SET_ATTRIBUTE_MULTI; 
	info_table_set_attribute_multi->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS set attribute multi data to Info TBL */
	memcpy( info_table_set_attribute_multi->file_name_list, filename_list, sizeof(info_table_set_attribute_multi->file_name_list) );
	info_table_set_attribute_multi->access_mode = *attributes;
	info_table_set_attribute_multi->mode = *attr_mode;
	info_table_set_attribute_multi->file_num = *fnum;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_set_attribute_multi->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}


FW_INT32 BF_Vfs_Get_Attribute( FW_CHAR* fname, FW_UCHAR* attributes )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_GET_ATTRIBUTE* info_table_get_attribute;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 fname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_get_attribute = (T_FS_VFS_INFO_TABLE_GET_ATTRIBUTE*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_get_attribute, 0, sizeof( T_FS_VFS_INFO_TABLE_GET_ATTRIBUTE ) );
	
	/* Set VFS common data to Info TBL */
	info_table_get_attribute->common_data.ope_cmd = D_BF_FS_VFS_GET_ATTRIBUTE; 
	info_table_get_attribute->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS get attribute data to Info TBL */
	fname_len = strlen(fname);
	strncpy( info_table_get_attribute->file_name, fname, fname_len);
//	memcpy( info_table_get_attribute->file_name, fname, D_BF_FS_VFS_FNAME_MAX );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	*attributes = info_table_get_attribute->access_mode;
	ret_code = info_table_get_attribute->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}


FW_INT32 BF_Vfs_Utime( FW_CHAR* fname, T_BF_FS_utimbuf* times )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_UTIME* info_table_utime;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 fname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_utime = (T_FS_VFS_INFO_TABLE_UTIME*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_utime, 0, sizeof( T_FS_VFS_INFO_TABLE_UTIME ) );
	
	/* Set VFS common data to Info TBL */
	info_table_utime->common_data.ope_cmd = D_BF_FS_VFS_UTIME; 
	info_table_utime->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS utime data to Info TBL */
	info_table_utime->date = times->date;
	info_table_utime->time = times->time;
	
	fname_len = strlen(fname);
	strncpy( info_table_utime->file_name, fname, fname_len);
//	memcpy( info_table_utime->file_name, fname, D_BF_FS_VFS_FNAME_MAX );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_utime->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}


FW_INT32 BF_Vfs_Utime_Ex( FW_CHAR* fname, FW_UINT32 utime_mode, T_BF_FS_utimbufEx* times )
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_UTIME_EX* info_table_utime_ex;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_UINT32 fname_len;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_utime_ex = (T_FS_VFS_INFO_TABLE_UTIME_EX*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_utime_ex, 0, sizeof( T_FS_VFS_INFO_TABLE_UTIME_EX ) );
	
	/* Set VFS common data to Info TBL */
	info_table_utime_ex->common_data.ope_cmd = D_BF_FS_VFS_UTIME_EX; 
	info_table_utime_ex->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS utime ex data to Info TBL */
	info_table_utime_ex->date = times->date;
	info_table_utime_ex->time = times->time;
	info_table_utime_ex->sec_10m = times->sec10m;
	info_table_utime_ex->mode = utime_mode;
	
	fname_len = strlen(fname);
	strncpy( info_table_utime_ex->file_name, fname, fname_len);
//	memcpy( info_table_utime_ex->file_name, fname, D_BF_FS_VFS_FNAME_MAX );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_utime_ex->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Create(FW_CHAR* dirname, FW_INT32 amode)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_CREATE* info_table_create;
	FW_UINT32 dname_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_create = (T_FS_VFS_INFO_TABLE_CREATE*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_create, 0, sizeof( T_FS_VFS_INFO_TABLE_CREATE ) );
	
	/* Set VFS common data to Info TBL */
	info_table_create->common_data.ope_cmd = D_BF_FS_VFS_CREATE; 
	info_table_create->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS create data to Info TBL */
	info_table_create->amode = amode;
	dname_len = strlen(dirname);
	strncpy( info_table_create->file_name, dirname, dname_len );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_create->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Copy(FW_CHAR* srcname, FW_CHAR* dstname)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_COPY* info_table_copy;
	FW_UINT32 name_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_copy = (T_FS_VFS_INFO_TABLE_COPY*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_copy, 0, sizeof( T_FS_VFS_INFO_TABLE_COPY ) );
	
	/* Set VFS common data to Info TBL */
	info_table_copy->common_data.ope_cmd = D_BF_FS_VFS_COPY; 
	info_table_copy->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS copy data to Info TBL */
	name_len = strlen(srcname);
	strncpy( info_table_copy->src_name, srcname, name_len );
	
	name_len = strlen(dstname);
	strncpy( info_table_copy->dst_name, dstname, name_len );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_copy->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Remove(FW_CHAR* fname)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_REMOVE* info_table_remove;
	FW_UINT32 fname_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_remove = (T_FS_VFS_INFO_TABLE_REMOVE*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_remove, 0, sizeof( T_FS_VFS_INFO_TABLE_REMOVE ) );
	
	/* Set VFS common data to Info TBL */
	info_table_remove->common_data.ope_cmd = D_BF_FS_VFS_REMOVE; 
	info_table_remove->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS remove data to Info TBL */
	fname_len = strlen(fname);
	strncpy( info_table_remove->file_name, fname, fname_len );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_remove->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_RemoveMulti(FW_CHAR* dirname, FW_CHAR filename_list[][13], FW_INT32 fnum)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_REMOVEMULTI* info_table_remove_multi;
	FW_UINT32 name_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_remove_multi = (T_FS_VFS_INFO_TABLE_REMOVEMULTI*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_remove_multi, 0, sizeof( T_FS_VFS_INFO_TABLE_REMOVEMULTI ) );
	
	/* Set VFS common data to Info TBL */
	info_table_remove_multi->common_data.ope_cmd = D_BF_FS_VFS_REMOVEMULTI; 
	info_table_remove_multi->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS remove multi data to Info TBL */
	name_len = strlen(dirname);
	strncpy( info_table_remove_multi->dir_name, dirname, name_len );
	
	strncpy( (FW_CHAR*)info_table_remove_multi->filename_list, (FW_CHAR*)filename_list, (fnum*13) );
	
	info_table_remove_multi->fnum = fnum;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_remove_multi->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Fstat(FW_INT32 fNo, T_BF_FS_IF_STAT* sbuf)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_FSTAT* info_table_fstat;
	OS_USER_FLGPTN flgptn;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	/* Get information of remainder data */
	vfs_get_remain_info( fNo, &file_cnt, &req_cnt );

	if( req_cnt != REQ_CNT_MAX ) {	/* Is there a remainder data ? */
		info_table_num = (FW_SHORT)( file_cnt * REQ_CNT_MAX + req_cnt + 1 );
		
		/* Send remainder data */
		ret_code = vfs_send_remain_write_data( info_table_num );
		
		if( ret_code < 0 ) {
			BF_Fs_User_Custom_Err_Printer((" [VFS] Error in Remain write proc by FSTAT request [fNo=%d].\n", fNo ));
		}

		vfs_clear_info_table_num_counter( g_bf_vfs_ctrl[file_cnt][req_cnt].file_id, info_table_num );
		memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof(g_bf_vfs_ctrl[file_cnt][req_cnt]) );	/* Clear VFS control management table */
		
	}
	
	/* Start Fstat process */
	vfs_get_info_table_num_counter( fNo, &info_table_num );

	info_table_fstat = (T_FS_VFS_INFO_TABLE_FSTAT*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	/* Set VFS management table */
	g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fNo;

	memset( info_table_fstat, 0, sizeof( T_FS_VFS_INFO_TABLE_FSTAT ) );
	
	/* Set VFS common data to Info TBL */
	info_table_fstat->common_data.ope_cmd = D_BF_FS_VFS_FSTAT; 
	info_table_fstat->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS fstat data to Info TBL */
	info_table_fstat->file_id = fNo;
	
	/* Set VFS management table */
	file_cnt = ( info_table_num - 1 ) / REQ_CNT_MAX;
	req_cnt  = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].file_id = fNo;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	sbuf->date = info_table_fstat->modify_date;
	sbuf->time = info_table_fstat->modify_time;
	sbuf->m_date = info_table_fstat->create_date;
	sbuf->m_time = info_table_fstat->create_time;
	sbuf->fileSize = info_table_fstat->size;
	
	ret_code = info_table_fstat->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Mkdir(FW_CHAR* dirname)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_MKDIR* info_table_mkdir;
	FW_UINT32 dname_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_mkdir = (T_FS_VFS_INFO_TABLE_MKDIR*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_mkdir, 0, sizeof( T_FS_VFS_INFO_TABLE_MKDIR ) );
	
	/* Set VFS common data to Info TBL */
	info_table_mkdir->common_data.ope_cmd = D_BF_FS_VFS_MKDIR; 
	info_table_mkdir->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS mkdir data to Info TBL */
	dname_len = strlen(dirname);
	strncpy( info_table_mkdir->dir_name, dirname, dname_len );
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_mkdir->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Rmdir(FW_CHAR* dirname)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_RMDIR* info_table_rmdir;
	FW_UINT32 dname_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_rmdir = (T_FS_VFS_INFO_TABLE_RMDIR*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_rmdir, 0, sizeof( T_FS_VFS_INFO_TABLE_RMDIR ) );
	
	/* Set VFS common data to Info TBL */
	info_table_rmdir->common_data.ope_cmd = D_BF_FS_VFS_RMDIR; 
	info_table_rmdir->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS rmdir data to Info TBL */
	dname_len = strlen(dirname);
	strncpy( info_table_rmdir->dir_name, dirname, dname_len);
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_rmdir->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Opendir(FW_CHAR* dirname, FW_INT32* dNo)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_OPENDIR* info_table_opendir;
	FW_UINT32 dname_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_opendir = (T_FS_VFS_INFO_TABLE_OPENDIR*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_opendir, 0, sizeof( T_FS_VFS_INFO_TABLE_OPENDIR ) );
	
	/* Set VFS common data to Info TBL */
	info_table_opendir->common_data.ope_cmd = D_BF_FS_VFS_OPENDIR; 
	info_table_opendir->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS opendir data to Info TBL */
	dname_len = strlen(dirname);
	strncpy( info_table_opendir->dir_name, dirname, dname_len);
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	*dNo = info_table_opendir->dir_id;
	
	ret_code = info_table_opendir->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Readdir(FW_INT32 dNo, T_BF_FS_IF_Dir_Info* fsdirent)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_READDIR* info_table_readdir;
	FW_UINT32 fname_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_readdir = (T_FS_VFS_INFO_TABLE_READDIR*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_readdir, 0, sizeof( T_FS_VFS_INFO_TABLE_READDIR ) );
	
	/* Set VFS common data to Info TBL */
	info_table_readdir->common_data.ope_cmd = D_BF_FS_VFS_READDIR; 
	info_table_readdir->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS readdir data to Info TBL */
	info_table_readdir->dir_id = dNo;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	fname_len = strlen(info_table_readdir->file_name);
	memset( fsdirent->fname, 0, sizeof(fsdirent->fname) );
	strncpy( fsdirent->fname, info_table_readdir->file_name, fname_len);
	
	memset( fsdirent->fname, 0, sizeof(fsdirent->fname) );
	if( fname_len > VFS_DOS_FNAME_MAX )
		strncpy( fsdirent->fname, info_table_readdir->file_name, VFS_DOS_FNAME_MAX);
	else{
		strncpy( fsdirent->fname, info_table_readdir->file_name, fname_len);
	}
	
	fsdirent->attributes = info_table_readdir->amode;
	fsdirent->fileSize = info_table_readdir->file_size;
	
	ret_code = info_table_readdir->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Closedir(FW_INT32 dNo)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_CLOSEDIR* info_table_closedir;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_closedir = (T_FS_VFS_INFO_TABLE_CLOSEDIR*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_closedir, 0, sizeof( T_FS_VFS_INFO_TABLE_CLOSEDIR ) );
	
	/* Set VFS common data to Info TBL */
	info_table_closedir->common_data.ope_cmd = D_BF_FS_VFS_CLOSEDIR; 
	info_table_closedir->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS closedir data to Info TBL */
	info_table_closedir->dir_id = dNo;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_closedir->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Chdir(FW_CHAR* dirname)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_CHDIR* info_table_chdir;
	FW_UINT32 dname_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_chdir = (T_FS_VFS_INFO_TABLE_CHDIR*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_chdir, 0, sizeof( T_FS_VFS_INFO_TABLE_CHDIR ) );
	
	/* Set VFS common data to Info TBL */
	info_table_chdir->common_data.ope_cmd = D_BF_FS_VFS_CHDIR; 
	info_table_chdir->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS chdir data to Info TBL */
	dname_len = strlen(dirname);
	strncpy( info_table_chdir->dir_name, dirname, dname_len);
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_chdir->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Getcurdir(FW_CHAR* dname, FW_CHAR* curpath, FW_INT32 size)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_GETCURDIR* info_table_getcurdir;
	FW_UINT32 dname_len, path_len;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_getcurdir = (T_FS_VFS_INFO_TABLE_GETCURDIR*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_getcurdir, 0, sizeof( T_FS_VFS_INFO_TABLE_GETCURDIR ) );
	
	/* Set VFS common data to Info TBL */
	info_table_getcurdir->common_data.ope_cmd = D_BF_FS_VFS_GETCURDIR; 
	info_table_getcurdir->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS getcurdir data to Info TBL */
	info_table_getcurdir->size = size;
	
	dname_len = strlen(dname);
	strncpy( info_table_getcurdir->drive_name, dname, dname_len);
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	path_len = strlen(info_table_getcurdir->curpath);
	if (path_len > size) {
		path_len = size;
	}
	strncpy( curpath, info_table_getcurdir->curpath, path_len);
	
	ret_code = info_table_getcurdir->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Qformat(FW_INT32 media_id)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_QFORMAT* info_table_qformat;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_qformat = (T_FS_VFS_INFO_TABLE_QFORMAT*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_qformat, 0, sizeof( T_FS_VFS_INFO_TABLE_QFORMAT ) );
	
	/* Set VFS common data to Info TBL */
	info_table_qformat->common_data.ope_cmd = D_BF_FS_VFS_QFORMAT; 
	info_table_qformat->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS qformat data to Info TBL */
	info_table_qformat->media_id = media_id;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_qformat->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Nformat(FW_INT32 media_id)
{
	FW_SHORT info_table_num;
	T_FS_VFS_INFO_TABLE_NFORMAT* info_table_nformat;
	OS_USER_FLGPTN flgptn;
	FW_INT32 fNo = 0;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_nformat = (T_FS_VFS_INFO_TABLE_NFORMAT*)(D_BF_FS_VFS_ADR_INFO_AREA + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_nformat, 0, sizeof( T_FS_VFS_INFO_TABLE_NFORMAT ) );
	
	/* Set VFS common data to Info TBL */
	info_table_nformat->common_data.ope_cmd = D_BF_FS_VFS_NFORMAT; 
	info_table_nformat->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS nformat data to Info TBL */
	info_table_nformat->media_id = media_id;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;
	
	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));
	
	ret_code = info_table_nformat->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}
	
	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

FW_INT32 BF_Vfs_Set_Write_Err_Cb( FW_UINT32 fNo, VFS_SET_WRITE_ERR_CB pCallback )
{
	FW_INT32 ercd = 0;
	FW_UCHAR file_cnt;
	
	for( file_cnt = 0; file_cnt < FILE_CNT_MAX; file_cnt++ ) {
		if( g_bf_vfs_file_use[file_cnt].file_use == fNo ) {
			break;
		}
	}
	
	if( file_cnt == FILE_CNT_MAX ) {
		return D_BF_FS_VFS_NO_MORE_OPEN;
	}
	
	g_bf_vfs_write_error[file_cnt] = pCallback;
	
	return ercd;
}

FW_INT32 BF_Vfs_Set_User_Setting( const FW_VOID* addr, FW_ULONG count )
{
	T_FS_VFS_INFO_TABLE_USERSET* info_table_userset;
	OS_USER_FLGPTN	flgptn;
	FW_SHORT info_table_num;
	FW_UCHAR file_cnt, req_cnt;
	FW_INT32 ret_code = 0;
	FW_INT32 fNo = 0;
	
	vfs_get_info_table_num_counter( fNo, &info_table_num );
	info_table_userset = (T_FS_VFS_INFO_TABLE_USERSET*)(D_BF_FS_VFS_ADR_INFO_AREA 
					 + ( D_BF_FS_VFS_BLK_SIZ_INF_TBL * info_table_num ));
	
	memset( info_table_userset, 0, sizeof( T_FS_VFS_INFO_TABLE_USERSET ) );
	
	/* Set VFS common data */
	info_table_userset->common_data.ope_cmd = D_BF_FS_VFS_USERSET;
	info_table_userset->common_data.info_tbl_no = info_table_num;
	
	/* Set VFS write data */
	info_table_userset->buffer_pointer = ( FW_UINT32* )addr;
	info_table_userset->count = count;
	
	/* Set VFS management table */
	file_cnt = FILE_CNT_MAX;
	req_cnt = ( info_table_num - 1 ) % REQ_CNT_MAX;
	g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state |= VFS_SEND;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)addr, count );

	/* IPCU Send */
	OS_User_Set_Flg( FID_FS_VFS_SEND, info_table_num );	
	
	/* IPCU Receive */
	OS_User_Twai_Flg( g_bf_vfs_receive_flg_id[file_cnt], ( 0x00000001 << req_cnt ), D_OS_USER_TWF_ANDW, &flgptn, D_OS_USER_TMO_FEVR );
	OS_User_Clr_Flg( g_bf_vfs_receive_flg_id[file_cnt], ~( 0x00000001 << req_cnt ));

	ret_code = info_table_userset->error_code;
	
	while( 1 ) {
		if( g_bf_vfs_ctrl[file_cnt][req_cnt].proc_state != VFS_FINISH ) {
			OS_User_Dly_Tsk(1);
		}
		break;
	}

	vfs_clear_info_table_num_counter( fNo, info_table_num );
	memset( ( FW_VOID* )&g_bf_vfs_ctrl[file_cnt][req_cnt], 0, sizeof( g_bf_vfs_ctrl[file_cnt][req_cnt]) );
	
	return ret_code;
}

