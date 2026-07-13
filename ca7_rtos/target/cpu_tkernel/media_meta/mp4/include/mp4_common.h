/**
 * @file		mp4_common.h
 * @brief		mp4 common function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MP4_COMMON_H_
#define _MP4_COMMON_H_

#include "fj_mux.h"
#include "fj_mux_demux_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* result */
#define MP4_COMMON_OK							( 0)		// Normal End
#define MP4_COMMON_OK_MAX_TIME_END				( 1)		// Normal End(MaxTime)
#define MP4_COMMON_OK_MAX_SIZE_END				( 2)		// Normal End(MaxSize)
#define MP4_COMMON_OK_MEDIAFULL_END				( 3)		// Normal End(Media Full)
#define MP4_COMMON_NG							(-1)		// Error
#define MP4_COMMON_NG_INPUT_PARAM				(-2)		// Input Parameter Error
#define MP4_COMMON_NG_MEDIA_AREA_NOTHING		(-3)		// Media Vacant Area Nothing
#define MP4_COMMON_NG_FILE_SIZE					(-4)		// File Size Error
#define MP4_COMMON_NG_SYSTEM					(-5)		// System Error

/* Mux Only */
#define MP4_COMMON_FS_BANK_MAX_NUM				(4)

/* SDRAM bank Max Size */
#define D_MP4_COMMON_SDRAM_BANK_SIZE			(12288)		// MP4 lib spec

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* --- Bit Stream Control --- */
typedef struct{
	UCHAR*	pointer;
	UCHAR	bit_remain;
} T_MP4_COMMON_STREAM_CONTROL;

/* --- Memeory Control --- */
typedef struct {
	UINT32	header_mem_addr;
	UINT32	header_mem_size;
	UINT32	header_mem_table_count;
	UINT32	fs_bank_addr[E_FJ_MUX_CH_MAX][MP4_COMMON_FS_BANK_MAX_NUM];	// only Mux
	UINT32	fs_bank_size;						// only Mux
	UINT32	fs_bank_current[E_FJ_MUX_CH_MAX];	// only Mux
} T_MP4_COMMON_MEMORY_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern	VOID	BF_MP4_Common_BS_Set_Pointer_Address( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR* pointer_address );
extern	VOID	BF_MP4_Common_BS_Skip_Pointer_Address( T_MP4_COMMON_STREAM_CONTROL* bit_stream, ULONG skip_bytes );
extern	VOID	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, ULONG skip_bits );
extern	UCHAR*	BF_MP4_Common_BS_Get_Pointer( T_MP4_COMMON_STREAM_CONTROL* bit_stream );
extern	UCHAR	BF_MP4_Common_BS_Get_Remain_Bits( T_MP4_COMMON_STREAM_CONTROL* bit_stream );
extern	ULONG	BF_MP4_Common_BS_Read_U_1bit( T_MP4_COMMON_STREAM_CONTROL* bit_stream );
extern	ULONG	BF_MP4_Common_BS_Read_U_Nbit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR read_num );
extern	UINT64	BF_MP4_Common_BS_Read_64_U_Nbit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR read_num );
extern	INT32	BF_MP4_Common_BS_Read_U_E( T_MP4_COMMON_STREAM_CONTROL* bit_stream );
extern	INT32	BF_MP4_Common_BS_Read_S_E( T_MP4_COMMON_STREAM_CONTROL* bit_stream );
extern	VOID	BF_MP4_Common_BS_Write_U_1bit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, INT32 write_value );
extern	VOID	BF_MP4_Common_BS_Write_U_Nbit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR write_num, INT32 write_value );
extern	VOID	BF_MP4_Common_BS_Write_64_U_Nbit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR write_num, INT64 write_value );
extern	VOID	BF_MP4_Common_Start_Code_Emuration_Convert( UINT8* src_stream_address, UINT32 src_stream_size, UINT8* dst_stream_address, UINT32* dst_stream_size );
extern	VOID	BF_MP4_Common_Memory_Initialize( T_MP4_COMMON_MEMORY_INFO* mem_info );
extern	VOID	BF_MP4_Common_Memory_Finalize( VOID );
extern	VOID	fj_mux_demux_common_memory_initialize( T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG* mem_config );
extern	VOID	fj_mux_demux_common_memory_finalize( VOID );

extern	VOID	BF_MP4_Common_Fs_Create_Time_Initialize( INT32 file_id );
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
extern	VOID BF_MP4_Common_WIFI_Out_EMMC( BOOL enable );
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---
extern	ULLONG	BF_MP4_Common_Fs_Mediasize_Get( VOID );
extern	INT32	BF_MP4_Common_Fs_Memory_Initialize( INT32 file_id[], INT32 file_cnt );
extern	INT32	BF_MP4_Common_Fs_Memory_Update( INT32 file_id, UCHAR opt );
extern	INT32	BF_MP4_Common_Fs_Memory_Write( INT32 file_id, const VOID *buffer, ULONG count );
extern	INT32	BF_MP4_Common_Fs_Memory_Write_Down( INT32 file_id, UINT32 end_flg );


#endif	// _MP4_COMMON_H_
