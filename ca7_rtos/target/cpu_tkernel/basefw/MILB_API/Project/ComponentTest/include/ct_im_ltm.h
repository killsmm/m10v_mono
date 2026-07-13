/**
 * @file		ct_im_ltm.h
 * @brief		This is ct header for im_ltm_common.c/im_ltm_rbk.c/im_ltm_map.c/im_ltm_ltm.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */
#ifndef _CT_IM_LTM_H_
#define _CT_IM_LTM_H_

#include "im_ltm.h"
#include "driver_common.h"
#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH			(1280)
#define D_IM_LTM_RBK_SRC_IMG_PIXS_LINES			(960)

#define D_IM_LTM_RBK_SRC_IMG_RGB_GLOBAL_WIDTH	(D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH * 2)		// RGB planar 16bit
#define D_IM_LTM_RBK_SRC_IMG_BAY_GLOBAL_WIDTH	(D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH * 3 / 2)	// Bayer 12bit pack

#define D_IM_LTM_RBK_SRC_IMG_RGB_BYTES			(D_IM_LTM_RBK_SRC_IMG_RGB_GLOBAL_WIDTH * D_IM_LTM_RBK_SRC_IMG_PIXS_LINES * 3)
#define D_IM_LTM_RBK_SRC_IMG_BAY_BYTES			(D_IM_LTM_RBK_SRC_IMG_BAY_GLOBAL_WIDTH * D_IM_LTM_RBK_SRC_IMG_PIXS_LINES)

#define D_IM_LTM_RBK_DST_IMG_PIXS_WIDTH			(640)
#define D_IM_LTM_RBK_DST_IMG_PIXS_LINES			(480)

#define D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_UP	(((( D_IM_LTM_RBK_DST_IMG_PIXS_WIDTH * 3 * 2     ) + ( 512 - 1 )) / 512 ) * 512 )	// RGB dot 12bit unpack 512byte alignment
#define D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_P		(((( D_IM_LTM_RBK_DST_IMG_PIXS_WIDTH * 3 * 3 / 2 ) + ( 512 - 1 )) / 512 ) * 512 )	// RGB dot 12bit pack 512byte alignment

#define D_IM_LTM_RBK_DST_IMG_BYTES_UP			(D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_UP * D_IM_LTM_RBK_DST_IMG_PIXS_LINES)
#define D_IM_LTM_RBK_DST_IMG_BYTES_P			(D_IM_LTM_RBK_DST_IMG_GLOBAL_WIDTH_P  * D_IM_LTM_RBK_DST_IMG_PIXS_LINES)

#define D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP	(0x60000000)
#define D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP		(0x60000000)
#define D_IM_LTM_RBK_IMG_MEM_IN_G_ADDR_TOP		(D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP + D_IM_LTM_RBK_SRC_IMG_RGB_GLOBAL_WIDTH * D_IM_LTM_RBK_SRC_IMG_PIXS_LINES)
#define D_IM_LTM_RBK_IMG_MEM_IN_B_ADDR_TOP		(D_IM_LTM_RBK_IMG_MEM_IN_G_ADDR_TOP + D_IM_LTM_RBK_SRC_IMG_RGB_GLOBAL_WIDTH * D_IM_LTM_RBK_SRC_IMG_PIXS_LINES)
#define D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP	(0x64000000)



#define D_IM_LTM_MAP_SRC_IMG_PIXS_WIDTH	(320)
#define D_IM_LTM_MAP_SRC_IMG_PIXS_LINES	(240)

#define D_IM_LTM_MAP_SRC_IMG_GLOBAL_WIDTH_UP	(((( D_IM_LTM_MAP_SRC_IMG_PIXS_WIDTH * 3 * 2     ) + ( 512 - 1 )) / 512 ) * 512 )	// RGB dot 12bit unpack
#define D_IM_LTM_MAP_SRC_IMG_GLOBAL_WIDTH_P		(((( D_IM_LTM_MAP_SRC_IMG_PIXS_WIDTH * 3 * 3 / 2 ) + ( 512 - 1 )) / 512 ) * 512 )	// RGB dot 12bit pack

#define D_IM_LTM_MAP_SRC_IMG_BYTES_UP			(D_IM_LTM_MAP_SRC_IMG_GLOBAL_WIDTH_UP * D_IM_LTM_MAP_SRC_IMG_PIXS_LINES)
#define D_IM_LTM_MAP_SRC_IMG_BYTES_P			(D_IM_LTM_MAP_SRC_IMG_GLOBAL_WIDTH_P  * D_IM_LTM_MAP_SRC_IMG_PIXS_LINES)

#define D_IM_LTM_MAP_DST_IMG_PIXS_WIDTH	(320)
#define D_IM_LTM_MAP_DST_IMG_PIXS_LINES	(240)

#define D_IM_LTM_MAP_DST_IMG_GLOBAL_WIDTH_UP	(D_IM_LTM_MAP_DST_IMG_PIXS_WIDTH * 2)			// Y 12bit unpack
#define D_IM_LTM_MAP_DST_IMG_GLOBAL_WIDTH_P		(D_IM_LTM_MAP_DST_IMG_PIXS_WIDTH * 3 / 2)		// Y 12bit pack

#define D_IM_LTM_MAP_DST_IMG_BYTES_UP			(D_IM_LTM_MAP_DST_IMG_GLOBAL_WIDTH_UP * D_IM_LTM_MAP_DST_IMG_PIXS_LINES)
#define D_IM_LTM_MAP_DST_IMG_BYTES_P			(D_IM_LTM_MAP_DST_IMG_GLOBAL_WIDTH_P  * D_IM_LTM_MAP_DST_IMG_PIXS_LINES)

#define D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP	(0x64000000)
#define D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP	(0x65000000)
#define D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP	(0x66000000)



#define D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_WIDTH	(640)
#define D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES	(480)
#define D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH	(320)
#define D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES	(240)

#define D_IM_LTM_LTM_SRC_IMG_RGB_GLOBAL_WIDTH		(D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_WIDTH * 2)		// RGB planar 16bit
#define D_IM_LTM_LTM_SRC_IMG_MAP_GLOBAL_WIDTH_UP	(D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH * 2)		// Y 12bit unpack
#define D_IM_LTM_LTM_SRC_IMG_MAP_GLOBAL_WIDTH_P		(D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_WIDTH * 3 / 2)	// Y 12bit pack

#define D_IM_LTM_LTM_SRC_IMG_RGB_BYTES			(D_IM_LTM_LTM_SRC_IMG_RGB_GLOBAL_WIDTH    * D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES * 3)
#define D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP		(D_IM_LTM_LTM_SRC_IMG_MAP_GLOBAL_WIDTH_UP * D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES)
#define D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_P		(D_IM_LTM_LTM_SRC_IMG_MAP_GLOBAL_WIDTH_P  * D_IM_LTM_LTM_SRC_IMG_MAP_PIXS_LINES)

#define D_IM_LTM_LTM_DST_IMG_PIXS_WIDTH	(640)
#define D_IM_LTM_LTM_DST_IMG_PIXS_LINES	(480)

#define D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP	(D_IM_LTM_LTM_DST_IMG_PIXS_WIDTH * 2)		// RGB planar 12bit unpack
#define D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_P		(D_IM_LTM_LTM_DST_IMG_PIXS_WIDTH * 3 / 2)	// RGB planar 12bit pack

#define D_IM_LTM_LTM_DST_IMG_BYTES_UP			(D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP * D_IM_LTM_LTM_DST_IMG_PIXS_LINES * 3)
#define D_IM_LTM_LTM_DST_IMG_BYTES_P			(D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_P  * D_IM_LTM_LTM_DST_IMG_PIXS_LINES * 3)
#define D_IM_LTM_LTM_DST_IMG_BYTES_YCC			(D_IM_LTM_LTM_DST_IMG_PIXS_WIDTH      * D_IM_LTM_LTM_DST_IMG_PIXS_LINES * 2)

#define D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP		(0x60000000)
#define D_IM_LTM_LTM_IMG_MEM_IN_G_ADDR_TOP		(D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP + D_IM_LTM_LTM_SRC_IMG_RGB_GLOBAL_WIDTH * D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES)
#define D_IM_LTM_LTM_IMG_MEM_IN_B_ADDR_TOP		(D_IM_LTM_LTM_IMG_MEM_IN_G_ADDR_TOP + D_IM_LTM_LTM_SRC_IMG_RGB_GLOBAL_WIDTH * D_IM_LTM_LTM_SRC_IMG_RGB_PIXS_LINES)
#define D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP		(0x65000000)
#define D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP		(0x66000000)
#define D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP		(0x67000000)
#define D_IM_LTM_LTM_IMG_MEM_OUT_G_ADDR_TOP		(D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP + D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP * D_IM_LTM_LTM_DST_IMG_PIXS_LINES)
#define D_IM_LTM_LTM_IMG_MEM_OUT_B_ADDR_TOP		(D_IM_LTM_LTM_IMG_MEM_OUT_G_ADDR_TOP + D_IM_LTM_LTM_DST_IMG_GLOBAL_WIDTH_UP * D_IM_LTM_LTM_DST_IMG_PIXS_LINES)
#define D_IM_LTM_LTM_IMG_MEM_OUT_Y_ADDR_TOP		(0x68000000)
#define D_IM_LTM_LTM_IMG_MEM_OUT_C_ADDR_TOP		(D_IM_LTM_LTM_IMG_MEM_OUT_Y_ADDR_TOP  + (D_IM_LTM_LTM_DST_IMG_PIXS_WIDTH * D_IM_LTM_LTM_DST_IMG_PIXS_LINES))

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern INT32 CT_Im_Ltm_Rbk_1_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_12( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_13( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_14( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_15( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_16( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_1_17( UCHAR pipe_no );

extern INT32 CT_Im_Ltm_Rbk_2_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_12( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_2_13( UCHAR pipe_no );

extern INT32 CT_Im_Ltm_Rbk_3_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_12( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_13( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_14( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_15( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_16( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Rbk_3_18( UCHAR pipe_no );

extern VOID CT_Im_LTM_RBK_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd );
extern VOID CT_Im_LTM_RBK_Main( INT32 argc, CHAR** argv );


extern INT32 CT_Im_Ltm_Map_1_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_12( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_13( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_14( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_15( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_16( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_17( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_18( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_19( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_20( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_1_21( UCHAR pipe_no );

extern INT32 CT_Im_Ltm_Map_2_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_12( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_13( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_14( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_15( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_16( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_2_17( UCHAR pipe_no );

extern INT32 CT_Im_Ltm_Map_3_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Map_3_12( UCHAR pipe_no );

extern VOID CT_Im_LTM_MAP_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd );
extern VOID CT_Im_LTM_MAP_Main( INT32 argc, CHAR** argv );

extern INT32 CT_Im_Ltm_Ltm_1_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_12( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_13( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_14( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_15( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_16( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_17( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_18( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_19( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_20( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_21( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_22( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_23( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_24( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_1_25( UCHAR pipe_no );

extern INT32 CT_Im_Ltm_Ltm_2_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_12( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_13( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_14( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_15( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_16( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_17( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_18( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_19( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_20( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_2_21( UCHAR pipe_no );

extern INT32 CT_Im_Ltm_Ltm_3_1( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_2( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_3( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_4( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_5( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_6( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_7( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_8( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_9( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_10( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_11( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_12( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_13( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_14( UCHAR pipe_no );
extern INT32 CT_Im_Ltm_Ltm_3_15( UCHAR pipe_no );

extern VOID CT_Im_LTM_LTM_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd );
extern VOID CT_Im_LTM_LTM_Main( INT32 argc, CHAR** argv );

#endif
