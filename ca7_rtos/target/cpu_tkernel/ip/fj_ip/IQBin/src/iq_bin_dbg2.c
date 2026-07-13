
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#include "iq_bin_dbg2.h"
#include "im_pro.h"
#include "im_b2r.h"
#include "im_r2y.h"
#include "im_cnr.h"
#include "im_shdr.h"
#include "im_ltm.h"
#include "jdsr2y.h"
#include "jdscnr.h"

#include "fj_filesystem.h"
#include "l1l2cache.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_IQ_DBG2_DATA_MAX			128
#define D_IQ_DBG2_MARKER_SIZE		8
#define D_IQ_DBG2_NAME_SIZE			16
#define D_IQ_DBG2_HEAD_SIZE			(512*7)

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	CHAR		start_marker[D_IQ_DBG2_MARKER_SIZE];
	UINT32		total_size;
	UINT32		header_size;
	UINT32		data_num;
	UINT32		data_ofst[D_IQ_DBG2_DATA_MAX];
	UINT32		data_size[D_IQ_DBG2_DATA_MAX];
	UCHAR		data_type[D_IQ_DBG2_DATA_MAX];
	CHAR		data_name[D_IQ_DBG2_DATA_MAX][D_IQ_DBG2_NAME_SIZE];
} T_IQ_DBG2_HEAD; // <= D_IQ_DBG2_HEAD_SIZE

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_IQ_DBG2_HEAD	gIQ_Dbg2_Head_Work;
static T_IQ_DBG2_HEAD*	gIQ_Dbg2_Head;
static VOID*			gIQ_Dbg2_Data[ D_IQ_DBG2_DATA_MAX ];
static UCHAR			gIQ_Dbg2_Pipe_Bk[ D_IQ_DBG2_DATA_MAX ];
static UCHAR			gIQ_Dbg2_End_Flg[ D_IQ_DBG2_DATA_MAX ];
static UINT32			gIQ_Dbg2_Cnt = 0;
static BOOL				gIQ_Dbg2_En = FALSE;
static BOOL				gIQ_Dbg2_R2Y_Tbl_Dynamic = FALSE;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 copy( VOID* dst, VOID* src, ULONG size );
static BOOL check_end( const T_IQ_DBG2_HEAD* head );
static BOOL find_name( const T_IQ_DBG2_HEAD* head, const char* name, UINT32* index );
static INT32 get_size_addr( E_IQ_DBG2_DATA_TYPE type, UCHAR pipe_no, UINT32* size, UINT32* addr );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 IQ_Dbg2_Enable( BOOL enable )
{
	gIQ_Dbg2_En = enable;
	return 0;
}


INT32 IQ_Dbg2_Init( void )
{
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_START, "IQ2 Init"));
	
	gIQ_Dbg2_Head = NULL;
	memset( (void*)&gIQ_Dbg2_Head_Work, 0, sizeof(T_IQ_DBG2_HEAD) );
	strncpy( gIQ_Dbg2_Head_Work.start_marker, "REG_DATA", D_IQ_DBG2_MARKER_SIZE );
	gIQ_Dbg2_Head_Work.header_size = D_IQ_DBG2_HEAD_SIZE;
	gIQ_Dbg2_Head_Work.total_size  = gIQ_Dbg2_Head_Work.header_size;
	
	memset( gIQ_Dbg2_Data, 0, sizeof( gIQ_Dbg2_Data ) );
	memset( gIQ_Dbg2_Pipe_Bk, 0, sizeof( gIQ_Dbg2_Pipe_Bk ) );
	memset( &gIQ_Dbg2_End_Flg, 0, sizeof( gIQ_Dbg2_End_Flg ) );
	
	gIQ_Dbg2_Cnt = 0;
	gIQ_Dbg2_R2Y_Tbl_Dynamic = FALSE;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_END, ""));
	
	return 0;
}


INT32 IQ_Dbg2_Assign_Data( const char* name, E_IQ_DBG2_DATA_TYPE type, UCHAR pipe_no )
{
	UINT32	size = 0;
	UINT32	index = 0;
	
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	if( get_size_addr( type, pipe_no, &size, NULL ) ){
		return -1;
	}
	
	if( find_name( &gIQ_Dbg2_Head_Work, name, &index ) ){
		UPRINTF_ERR(1);
		UPRINTF( "%s Already Exist\n", name );
		return -1;
	}
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_START, "IQ2 Assign %d", (INT32)type ));
	
	// add new data
	gIQ_Dbg2_Head_Work.data_ofst[ gIQ_Dbg2_Cnt ] = gIQ_Dbg2_Head_Work.total_size;
	gIQ_Dbg2_Head_Work.data_size[ gIQ_Dbg2_Cnt ] = size;
	gIQ_Dbg2_Head_Work.data_type[ gIQ_Dbg2_Cnt ] = type;
	strncpy( gIQ_Dbg2_Head_Work.data_name[ gIQ_Dbg2_Cnt ], name, D_IQ_DBG2_NAME_SIZE );
	gIQ_Dbg2_Data[ gIQ_Dbg2_Cnt ] = (VOID*)( (UINT32)gIQ_Dbg2_Head_Work.total_size );
	gIQ_Dbg2_Pipe_Bk[ gIQ_Dbg2_Cnt ] = pipe_no;
	// update
	gIQ_Dbg2_Head_Work.total_size += size;
	gIQ_Dbg2_Head_Work.data_num ++;
	
	gIQ_Dbg2_Cnt ++;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_END, ""));
	UPRINTF( "IQ DBG2 Assign %s\n", name );
	
	return 0;
}


INT32 IQ_Dbg2_Set_Address( ULONG address, ULONG* next_address, BOOL r2y_tbl_dynamic )
{
	INT32 i;
	
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	if( address == 0 ){
		return -1;
	}
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_START, "IQ2 Address %x", (INT32)address ));
	
	gIQ_Dbg2_R2Y_Tbl_Dynamic = r2y_tbl_dynamic;
	
	// Head from Work
	gIQ_Dbg2_Head = (T_IQ_DBG2_HEAD*)address;
	memcpy( gIQ_Dbg2_Head, &gIQ_Dbg2_Head_Work, D_IQ_DBG2_HEAD_SIZE );
	
	// Data
	for( i = 0; i < gIQ_Dbg2_Head->data_num; i ++ ){
		gIQ_Dbg2_Data[ i ] = (VOID*)( (ULONG)gIQ_Dbg2_Data[ i ] + address );
	}
	
	*next_address = address + gIQ_Dbg2_Head->total_size;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_END, ""));
	UPRINTF( "IQ DBG2 Addr = 0x%x, Next = 0x%x\n", (INT32)address, (INT32)*next_address );
	
	return 0;
}


INT32 IQ_Dbg2_Set_Data_for_Pipe_Direct( void )
{
	INT32 i;
	
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	if( gIQ_Dbg2_R2Y_Tbl_Dynamic ){
		return 0;
	}
	
	if( gIQ_Dbg2_Head == NULL ){
		return -1;
	}
	
	for( i = 0; i < gIQ_Dbg2_Head->data_num; i ++ ){
		if( gIQ_Dbg2_Head->data_type[ i ] == E_IQ_DBG2_DATA_TYPE_R2Y_TBL ){
			IQ_Dbg2_Set_Data( gIQ_Dbg2_Head->data_name[ i ], gIQ_Dbg2_Head->data_type[ i ], gIQ_Dbg2_Pipe_Bk[ i ] );
		}
	}
		
	return 0;
}

INT32 IQ_Dbg2_Start( void )
{
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	UPRINTF( "IQ DBG2 Start\n" );
	memset( &gIQ_Dbg2_End_Flg, 0, sizeof( gIQ_Dbg2_End_Flg ) );
	return 0;
}

INT32 IQ_Dbg2_Set_Data( const char* name, E_IQ_DBG2_DATA_TYPE type, UCHAR pipe_no )
{
	UINT32	size = 0;
	UINT32	index = 0;
	UINT32	addr = 0;
	
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	if( gIQ_Dbg2_Head == NULL ){
		return -1;
	}
	
	if( get_size_addr( type, pipe_no, &size, &addr ) ){
		return -1;
	}
	
	if( find_name( gIQ_Dbg2_Head, name, &index ) ){
		if( index < D_IQ_DBG2_DATA_MAX ){
			UPRINTF( "IQ DBG2 Copy %s\n", name );
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_START, "IQ2 Data%d %x(%x)to%x", (INT32)type, (INT32)addr, (INT32)size, (INT32)gIQ_Dbg2_Data[ index ] ));
			copy( gIQ_Dbg2_Data[ index ], (VOID*)addr, size );
			gIQ_Dbg2_End_Flg[ index ] = 1;
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_END, " " ));
		}else{
			UPRINTF_ERR(1);
			UPRINTF( "Index Error : %d\n", index );
		}
	}else{
		UPRINTF_ERR(1);
		UPRINTF( "%s Not Found\n", name );
		return -1;
	}
	
	if( check_end( gIQ_Dbg2_Head ) ){
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_POINT, "IQ2 End" ));
		UPRINTF( "IQ DBG2 End\n" );
	}
	
	return 0;
}


INT32 IQ_Dbg2_Wait_End( INT32 timeout_ms )
{
	INT32	ms_cnt = 0;
	BOOL	end = FALSE;
	
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	if( gIQ_Dbg2_Head == NULL ){
		return -1;
	}
	
	while( 1 ){
		
		end = check_end( gIQ_Dbg2_Head );
		
		if( end ){
			return 0;
		}
		
		if( timeout_ms == -1 ){
			UPRINTF_ERR(1);
			UPRINTF( "Not End at IQ_Dbg2_Wait_End() -1 \n");
			return -1;
		}
		
		if( ms_cnt == timeout_ms ){
			UPRINTF_ERR(1);
			UPRINTF( "Not End at IQ_Dbg2_Wait_End() %d \n", (INT32)ms_cnt );
			return -1;
		}
		
		OS_User_Dly_Tsk( 1 );
		
		ms_cnt ++;
	} 

	return -1;
}


INT32 IQ_Dgb2_Get_Head( ULONG* addr, ULONG* size, ULONG* total )
{
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	if( gIQ_Dbg2_Head == NULL ){
		return -1;
	}
	
	*addr = (ULONG)gIQ_Dbg2_Head;
	*size = gIQ_Dbg2_Head->header_size;
	*total = gIQ_Dbg2_Head->total_size;
	
	return 0;
}


INT32 IQ_Dbg2_Save_File( void )
{
	ULONG	fid;
	ULONG	write_size = 0;
	CHAR	fname[64];
	static	ULONG fcnt = 0;
	INT32	i;
	
	if( !gIQ_Dbg2_En ){
		return 0;
	}
	
	if( gIQ_Dbg2_Head == NULL ){
		return -1;
	}
	
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)gIQ_Dbg2_Head, (UINT32)gIQ_Dbg2_Head->total_size );
	
	// Head
	sprintf( fname, "I:\\iqdbg2_%lu_%s.bin", fcnt, "__HEAD" );
	fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)gIQ_Dbg2_Head, gIQ_Dbg2_Head->header_size, &write_size );
	FJ_Fs_Close( fid );
	
	// Data
	for( i = 0; i < gIQ_Dbg2_Head->data_num; i ++ ){
		sprintf( fname, "I:\\iqdbg2_%lu_%s.bin", fcnt, gIQ_Dbg2_Head->data_name[ i ] );
		fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
		FJ_Fs_Write ( fid, (VOID*)gIQ_Dbg2_Data[ i ], gIQ_Dbg2_Head->data_size[ i ], &write_size );
		FJ_Fs_Close( fid );
	}
	fcnt++;
	
	return 0;
}


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 copy( VOID* dst, VOID* src, ULONG size )
{
#if 1 // dma copy // use FJ_MEMORY_COPY_XDMACFP_CH_1, which is used by D_MP4_COMMON_DMA_USE_CH
	ULONG cpu_copy_size;
	ULONG dma_copy_size;

	// Address adjust
	cpu_copy_size = size % 128;
	dma_copy_size = size - cpu_copy_size;

	if( dma_copy_size > 0 ){
		FJ_MEMORY_Copy_Sdram_Sync( FJ_MEMORY_COPY_XDMACFP_CH_1, (UINT32)src, (UINT32)dst, dma_copy_size, FJ_MEMORY_WAITMODE_EVENT );
	}
	if( cpu_copy_size > 0 ){
		memcpy( dst, src, size );
	}
#else // cpu copy
	memcpy( dst, src, size );
#endif
	
	return 0;
}


static BOOL check_end( const T_IQ_DBG2_HEAD* head )
{
	INT32 i;
	
	if( head == NULL ){
		return FALSE;
	}
	
	for( i = 0; i < head->data_num; i ++ ){
		if( !gIQ_Dbg2_R2Y_Tbl_Dynamic && head->data_type[ i ] == E_IQ_DBG2_DATA_TYPE_R2Y_TBL ){
			continue;
		}
		if( gIQ_Dbg2_End_Flg[ i ] == 0 ){
			return FALSE;
		}
	}
	
	return TRUE;
}


static BOOL find_name( const T_IQ_DBG2_HEAD* head, const char* name, UINT32* index )
{
	INT32 i;
	BOOL found = FALSE;
	
	if( head == NULL ){
		return found;
	}
	
	for( i = 0; i < head->data_num; i ++ ){
		if( strncmp( head->data_name[ i ], name, D_IQ_DBG2_NAME_SIZE ) == 0 ){
			found = TRUE;
			break;
		}
	}
	*index = i;
	return found;
}


static INT32 get_size_addr( E_IQ_DBG2_DATA_TYPE type, UCHAR pipe_no, UINT32* size, UINT32* addr )
{
	UINT32	size_tmp = 0;
	UINT32	addr_tmp = 0;
	
	if( pipe_no > 1 ){
		return -1;
	}
	
	switch( type ){
	case E_IQ_DBG2_DATA_TYPE_SEN:
		size_tmp = sizeof(  IO_PRO.SEN );
		addr_tmp = (UINT32)&IO_PRO.SEN;
		break;
	case E_IQ_DBG2_DATA_TYPE_SEN_TBL:
		size_tmp = sizeof(  IO_PRO_TBL.SEN_TBL );
		addr_tmp = (UINT32)&IO_PRO_TBL.SEN_TBL;
		break;
	case E_IQ_DBG2_DATA_TYPE_SRO:
		size_tmp = sizeof(  IO_PRO.IMG_PIPE[ pipe_no ].SRO );
		addr_tmp = (UINT32)&IO_PRO.IMG_PIPE[ pipe_no ].SRO;
		break;
	case E_IQ_DBG2_DATA_TYPE_B2B:
		size_tmp = sizeof(  IO_PRO.IMG_PIPE[ pipe_no ].B2B );
		addr_tmp = (UINT32)&IO_PRO.IMG_PIPE[ pipe_no ].B2B;
		break;
	case E_IQ_DBG2_DATA_TYPE_B2R:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_B2R_P1 ) : sizeof(  IO_B2R_P2 );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_B2R_P1   : (UINT32)&IO_B2R_P2;
		break;
	case E_IQ_DBG2_DATA_TYPE_LTM:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_LTM_P1 ) : sizeof(  IO_LTM_P2 );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_LTM_P1   : (UINT32)&IO_LTM_P2;
		break;
	case E_IQ_DBG2_DATA_TYPE_R2Y:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_R2Y_P1 ) : sizeof(  IO_R2Y_P2 );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_R2Y_P1   : (UINT32)&IO_R2Y_P2;
		break;
	case E_IQ_DBG2_DATA_TYPE_CNR:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_CNR1 ) : sizeof(  IO_CNR2 );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_CNR1   : (UINT32)&IO_CNR2;
		break;
	case E_IQ_DBG2_DATA_TYPE_SRO_TBL:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_PRO_TBL.SRO1_TBL ) : sizeof(  IO_PRO_TBL.SRO2_TBL );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_PRO_TBL.SRO1_TBL   : (UINT32)&IO_PRO_TBL.SRO2_TBL;
		break;
	case E_IQ_DBG2_DATA_TYPE_B2B_TBL:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_PRO_TBL.B2B1_TBL ) : sizeof(  IO_PRO_TBL.B2B2_TBL );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_PRO_TBL.B2B1_TBL   : (UINT32)&IO_PRO_TBL.B2B2_TBL;
		break;
	case E_IQ_DBG2_DATA_TYPE_B2R_TBL:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_B2R_TBL_P1 ) : sizeof(  IO_B2R_TBL_P2 );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_B2R_TBL_P1   : (UINT32)&IO_B2R_TBL_P2;
		break;
	case E_IQ_DBG2_DATA_TYPE_LTM_TBL:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_LTM_TBL_P1 ) : sizeof(  IO_LTM_TBL_P2 );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_LTM_TBL_P1   : (UINT32)&IO_LTM_TBL_P2;
		break;
	case E_IQ_DBG2_DATA_TYPE_R2Y_TBL:
		size_tmp = ( pipe_no == 0 ) ? sizeof(  IO_R2Y_TBL_P1 ) : sizeof(  IO_R2Y_TBL_P2 );
		addr_tmp = ( pipe_no == 0 ) ? (UINT32)&IO_R2Y_TBL_P1   : (UINT32)&IO_R2Y_TBL_P2;
		break;
	default:
		return -1;//break;
	}
	
	if( addr ){
		*addr = addr_tmp;
	}
	
	if( size ){
		*size = size_tmp;
	}
	
	return 0;
}
