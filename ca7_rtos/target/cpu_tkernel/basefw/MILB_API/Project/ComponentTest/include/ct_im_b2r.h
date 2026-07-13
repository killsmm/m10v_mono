/**
 * @file		ct_im_b2r.h
 * @brief		This is ct header for im_b2r.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_IM_B2R_H_
#define _CT_IM_B2R_H_

#include "driver_common.h"
#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_IM_B2R_SRC_IMG_PIXS_WIDTH	(640)
#define D_IM_B2R_SRC_IMG_PIXS_LINES	(480)

#define D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK			(D_IM_B2R_SRC_IMG_PIXS_WIDTH * 2)
#define D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_12BIT_PACK			(D_IM_B2R_SRC_IMG_PIXS_WIDTH * 3 / 2)
#define D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_10BIT_PACK			(D_IM_B2R_SRC_IMG_PIXS_WIDTH * 5 / 4)
#define D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_8BIT_NONPACK			(D_IM_B2R_SRC_IMG_PIXS_WIDTH)


#define D_IM_B2R_DST_IMG_PIXS_WIDTH	(640)
#define D_IM_B2R_DST_IMG_PIXS_LINES	(480)

#define D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U8			(D_IM_B2R_DST_IMG_PIXS_WIDTH)
#define D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_P12			(D_IM_B2R_DST_IMG_PIXS_WIDTH * 3 / 2)
#define D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U16			(D_IM_B2R_DST_IMG_PIXS_WIDTH * 2)


#define D_IM_B2R_IMG_MEM_IN_ADDR_TOP					(gct_im_b2r_in_addr)


#define D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP				(D_IM_B2R_IMG_MEM_IN_ADDR_TOP)
#define D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT				(D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK * D_IM_B2R_SRC_IMG_PIXS_LINES)
#define D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT				(D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_12BIT_PACK * D_IM_B2R_SRC_IMG_PIXS_LINES)
#define D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT				(D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_10BIT_PACK * D_IM_B2R_SRC_IMG_PIXS_LINES)
#define D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT				(D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_8BIT_NONPACK * D_IM_B2R_SRC_IMG_PIXS_LINES)
#define D_IM_B2R_IMG_MEM_IN_BAY_BYTES					(D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK * D_IM_B2R_SRC_IMG_PIXS_LINES)
#define D_IM_B2R_IMG_MEM_IN_BAY_ADDR_END				(D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP + D_IM_B2R_IMG_MEM_IN_BAY_BYTES)


#define D_IM_B2R_IMG_MEM_OUT_ADDR_TOP					(gct_im_b2r_out_addr[0])

#define D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP				(D_IM_B2R_IMG_MEM_OUT_ADDR_TOP)
#define D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES				(D_IM_B2R_DST_IMG_PIXS_WIDTH * D_IM_B2R_DST_IMG_PIXS_LINES *3)
#define D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES				(D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_P12 * D_IM_B2R_DST_IMG_PIXS_LINES *3)
#define D_IM_B2R_IMG_MEM_OUT_RGB_U16_BYTES				(D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U16 * D_IM_B2R_DST_IMG_PIXS_LINES *3)

#define D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP)
#define D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_U8_R_ADDR_TOP + D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U8 * D_IM_B2R_DST_IMG_PIXS_LINES)
#define D_IM_B2R_IMG_MEM_OUT_RGB_U8_B_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_U8_G_ADDR_TOP + D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U8 * D_IM_B2R_DST_IMG_PIXS_LINES)

#define D_IM_B2R_IMG_MEM_OUT_RGB_P12_R_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP)
#define D_IM_B2R_IMG_MEM_OUT_RGB_P12_G_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_P12_R_ADDR_TOP + D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_P12 * D_IM_B2R_DST_IMG_PIXS_LINES)
#define D_IM_B2R_IMG_MEM_OUT_RGB_P12_B_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_P12_G_ADDR_TOP + D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_P12 * D_IM_B2R_DST_IMG_PIXS_LINES)

#define D_IM_B2R_IMG_MEM_OUT_RGB_U16_R_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP)
#define D_IM_B2R_IMG_MEM_OUT_RGB_U16_G_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_U16_R_ADDR_TOP + D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U16 * D_IM_B2R_DST_IMG_PIXS_LINES)
#define D_IM_B2R_IMG_MEM_OUT_RGB_U16_B_ADDR_TOP			(D_IM_B2R_IMG_MEM_OUT_RGB_U16_G_ADDR_TOP + D_IM_B2R_DST_IMG_GLOBAL_WIDTH_RGB_U16 * D_IM_B2R_DST_IMG_PIXS_LINES)

#define D_IM_B2R_IMG_MEM_OUT2_ADDR_TOP					(gct_im_b2r_out_addr[1])


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
extern ULONG gct_im_b2r_in_addr;
extern ULONG gct_im_b2r_out_addr[2];


/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern VOID CT_Im_B2r_PrintReg( VOID );
extern INT32 CT_Im_B2r_1_1( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_2( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_3( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_4( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_5( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_6( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_7( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_8( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_9( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_10( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_11( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_12( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_13( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_14( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_15( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_16( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_17( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_18( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_19( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_20( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_21( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_22( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_23( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_24( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_25( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_26( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_27( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_28( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_29( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_30( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_31( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_32( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_33( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_34( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_35( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_36( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_37( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_38( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_39( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_40( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_41( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_42( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_43( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_44( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_45( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_46( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_47( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_48( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_49( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_50( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_51( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_52( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_53( UCHAR pipe_no );
extern INT32 CT_Im_B2r_1_54( UCHAR pipe_no );

extern INT32 CT_Im_B2r_2_1( UCHAR pipe_no );
extern INT32 CT_Im_B2r_2_2( UCHAR pipe_no );

extern INT32 CT_Im_B2r_3_1( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_2( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_3( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_4( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_5( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_6( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_7( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_8( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_9( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_10( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_11( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_12( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_13( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_14( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_15( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_16( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_17( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_18( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_19( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_20( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_21( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_22( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_23( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_24( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_25( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_26( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_27( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_28( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_29( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_30( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_31( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_32( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_33( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_34( UCHAR pipe_no );
extern INT32 CT_Im_B2r_3_35( UCHAR pipe_no );

extern VOID CT_Im_B2R_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd );
extern VOID CT_Im_B2R_Main( INT32 argc, CHAR** argv );

#endif
