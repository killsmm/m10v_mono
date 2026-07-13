/**
 * @file		sdram_map_movie_play.h
 * @brief		SocioNext SDRAM Movie Play Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _SDRAM_MAP_MOVIE_PLAY_H_
#define _SDRAM_MAP_MOVIE_PLAY_H_

#include "sdram_map_common.h"
#include "sdram_map_movie_play_define.h"

#ifdef CO_MOVIE_PORTING_BLOCK
	// SDRAM_MAP is using the excel tool. When a TOP address is changed, Re-create SDRAM_MAP
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define SDRAM_ADR_REL2ABS(relative_addr)	(SDRAM_ADR_TOP|(ULONG)(relative_addr))
#define SDRAM_BNK_LIMIT(pos, total) ((pos < total) ? pos : 0)
#define RIBERY_OFFSET(addr)		(((addr & 0x000003FF) == 0) ? addr : ((addr & ~(0x000003FF)) + 0x400))	// 1k(1024byte) alignment

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern	VOID	Sdram_Map_Movie_Play_H264_Initialize( UINT32 y_bank_size, UINT32 c_bank_size, UINT32* bank_num );
extern	VOID	Sdram_Map_Movie_Play_H264_Stream_Ctrl_Initialize( VOID );
extern	VOID	Sdram_Map_Movie_Play_H264_Get_Vdec( UINT32 idx, UINT32* y_addr, UINT32* c_addr );
extern	VOID	Sdram_Map_Movie_Play_H264_Get_LastVdec( UINT32* y_addr, UINT32* c_addr );
extern	INT32	Sdram_Map_Movie_Play_H264_Get_Stream_Area( UINT32* size, UINT32* address );
extern	BOOL	Sdram_Map_Movie_Play_H264_Check_Stream_Area( UINT32 size, UINT32 address );
extern	VOID	Sdram_Map_Movie_Play_H264_Release_Stream_Area( UINT32 address );
extern	VOID	Sdram_Map_Movie_Play_H264_Get_Stream_Empty_Size( UINT32* size );

extern	VOID	Sdram_Map_Movie_Play_H265_Initialize( UINT32 y_bank_size, UINT32 c_bank_size, UINT32* bank_num );
extern	VOID	Sdram_Map_Movie_Play_H265_Stream_Ctrl_Initialize( VOID );
extern	VOID	Sdram_Map_Movie_Play_H265_Get_Vdec( UINT32 idx, UINT32* y_addr, UINT32* c_addr );
extern	VOID	Sdram_Map_Movie_Play_H265_Get_LastVdec( UINT32* y_addr, UINT32* c_addr );
extern	VOID	Sdram_Map_Movie_Play_H265_Get_Vdec_Area( UINT32* y_start_addr, UINT32* y_end_addr, UINT32* c_start_addr, UINT32* c_end_addr );
extern	INT32	Sdram_Map_Movie_Play_H265_Get_Stream_Area( UINT32* size, UINT32* address );
extern	BOOL	Sdram_Map_Movie_Play_H265_Check_Stream_Area( UINT32 size, UINT32 address );
extern	VOID	Sdram_Map_Movie_Play_H265_Release_Stream_Area( UINT32 address );
extern	VOID	Sdram_Map_Movie_Play_H265_Get_Stream_Empty_Size( UINT32* size );

extern	VOID	Sdram_Map_Movie_Play_Audio_Initialize( VOID );
extern	UINT32	Sdram_Map_Movie_Play_Get_AStream_in( VOID );
extern	INT32	Sdram_Map_Movie_Play_Chg_AStream_in( VOID );
extern	UINT32	Sdram_Map_Movie_Play_Get_AStream_out( VOID );
extern	INT32	Sdram_Map_Movie_Play_Chg_AStream_out( VOID );
extern	UINT32	Sdram_Map_Movie_Play_Get_AStream_Area_Size( VOID );

#endif
