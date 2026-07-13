/**
 * @file		ctrl_ltm.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _CTRL_LTM_H_
#define _CTRL_LTM_H_

#include "share.h"
#include "share_data.h"
#include "ctrl_shdr.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	ULONG		rgb_addr_r;
	ULONG		rgb_addr_g;
	ULONG		rgb_addr_b;
	ULONG		rgb_addr_ofst;
	USHORT		rgb_h_byte;
	USHORT		rgb_h_size;
	USHORT		rgb_v_size;
	
	ULONG		map_addr_yp;
	ULONG		map_addr_yn;
	USHORT		map_h_byte;
	USHORT		map_h_size;
	USHORT		map_v_size;
	INT32		map_h_sta;
	INT32		map_v_sta;
	UINT32		map_h_pit;
	UINT32		map_v_pit;
} T_CTRL_LTM_LTM;

typedef struct {
	ULONG		rgb_addr_r;
	ULONG		rgb_addr_g;
	ULONG		rgb_addr_b;
	ULONG		rgb_addr_ofst;
	USHORT		rgb_h_byte;
	USHORT		rgb_h_size;
	USHORT		rgb_v_size;
	
	ULONG		rbk_addr_rgb;
	ULONG		rbk_addr_ofst;
	USHORT		rbk_h_byte;
	USHORT		rbk_hven;
	USHORT		rbk_sta;
	USHORT		rbk_pit;
	USHORT		rbk_rdcen;
	USHORT		rbk_rdcmd;
} T_CTRL_LTM_RBK;

typedef struct {
	ULONG		rbk_addr_rgb;
	ULONG		map_addr_yp;
	ULONG		map_addr_yn;
	
	USHORT		rbk_h_byte;
	USHORT		map_h_byte;
	USHORT		h_size;
	USHORT		v_size;
	
} T_CTRL_LTM_MAP;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

void Init_LTM_All( void );
void Start_LTM_RBK( const T_CTRL_LTM_RBK* p0, const T_CTRL_LTM_RBK* p1 );
void Stop_LTM_RBK( void );
void Start_LTM_MAP( const T_CTRL_LTM_MAP* p0 );
void Stop_LTM_MAP( void );

void Start_LTM_LTM2R2Y( const T_CTRL_LTM_LTM* p0, const T_CTRL_LTM_LTM* p1 );
void Stop_LTM_LTM2R2Y( BOOL stop_p0, BOOL stop_p1 );
void Set_LTM_LTM_IQ_Basic( BOOL p0, BOOL p1 );
void Set_LTM_MAP_IQ_Basic( BOOL p0, BOOL p1 );

#ifdef __cplusplus
}
#endif

#endif	// _CTRL_LTM_H_

