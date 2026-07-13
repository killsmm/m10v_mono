/**
 * @file		ct_im_cnr.h
 * @brief		ct header for im_cnr.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _CT_Im_Cnr_OFL_H_
#define _CT_Im_Cnr_OFL_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IM_CNR_R2Y_MACRO_ENABLE	// R2Y macro process selector

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern UINT32 gCT_Im_Cnr_OFL_In_Addr;
extern UINT32 gCT_Im_Cnr_OFL_Out_Addr;

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern INT32 ct_im_cnr_set_im_r2y_set( UCHAR ch );
extern INT32 ct_im_cnr_set_im_r2y_start( UCHAR ch );
extern INT32 ct_im_cnr_set_im_r2y_stop( UCHAR ch );


extern INT32 CT_Im_Cnr_OFL_1_1( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_2( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_3( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_4( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_5( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_6( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_7( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_8( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_9( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_10( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_11( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_12( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_13( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_14( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_15( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_16( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_17( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_18( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_19( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_20( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_21( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_22( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_23( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_24( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_25( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_26( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_27( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_30( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_31( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_32( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_33( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_34( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_35( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_36( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_37( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_38( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_39( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_40( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_41( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_42( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_43( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_44( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_45( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_46( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_47( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_48( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_49( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_50( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_51( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_52( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_53( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_54( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_1_55( UCHAR ch );

extern INT32 CT_Im_Cnr_OFL_2_1( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_2( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_3( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_4( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_5( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_6( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_7( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_8( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_9( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_10( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_11( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_12( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_13( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_14( UCHAR ch );
extern INT32 CT_Im_Cnr_OFL_2_15( UCHAR ch );

extern INT32 CT_IM_CNR_OTF_3_1( UCHAR ch );
extern INT32 CT_IM_CNR_OTF_3_2( UCHAR ch );
extern INT32 CT_IM_CNR_OTF_3_3( UCHAR ch );
extern INT32 CT_IM_CNR_OTF_3_4( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_5( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_6( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_7( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_8( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_9( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_10( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_11( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_12( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_13( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_14( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_15( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_16( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_17( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_18( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_19( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_20( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_21( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_22( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_23( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_24( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_25( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_26( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_30( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_31( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_32( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_33( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_34( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_35( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_36( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_37( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_38( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_39( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_40( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_41( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_42( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_43( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_44( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_45( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_46( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_47( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_48( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_49( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_50( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_3_51( UCHAR ch );

extern INT32 CT_Im_Cnr_OTF_4_1( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_2( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_3( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_4( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_5( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_6( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_7( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_8( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_9( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_10( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_11( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_12( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_13( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_14( UCHAR ch );
extern INT32 CT_Im_Cnr_OTF_4_15( UCHAR ch );

extern INT32 CT_Im_Cnr_5_1( UCHAR ch );
extern INT32 CT_Im_Cnr_5_2( UCHAR ch );
extern INT32 CT_Im_Cnr_5_3( UCHAR ch );
extern INT32 CT_Im_Cnr_5_4( UCHAR ch );
extern INT32 CT_Im_Cnr_5_5( UCHAR ch );
extern INT32 CT_Im_Cnr_5_6( UCHAR ch );
extern INT32 CT_Im_Cnr_5_7( UCHAR ch );
extern INT32 CT_Im_Cnr_5_8( UCHAR ch );
extern INT32 CT_Im_Cnr_5_9( UCHAR ch );
extern INT32 CT_Im_Cnr_5_10( UCHAR ch );
extern INT32 CT_Im_Cnr_5_11( UCHAR ch );
extern INT32 CT_Im_Cnr_5_12( UCHAR ch );

extern INT32 CT_IM_CNR_6_1( UCHAR ch );
extern INT32 CT_IM_CNR_6_2( UCHAR ch );
extern INT32 CT_IM_CNR_6_3( UCHAR ch );
extern INT32 CT_IM_CNR_6_4( UCHAR ch );
extern INT32 CT_Im_Cnr_6_5( UCHAR ch );
extern INT32 CT_Im_Cnr_6_6( UCHAR ch );
extern INT32 CT_Im_Cnr_6_7( UCHAR ch );
extern INT32 CT_Im_Cnr_6_8( UCHAR ch );
extern INT32 CT_Im_Cnr_6_9( UCHAR ch );
extern INT32 CT_Im_Cnr_6_10( UCHAR ch );
extern INT32 CT_Im_Cnr_6_11( UCHAR ch );
extern INT32 CT_Im_Cnr_6_12( UCHAR ch );
extern INT32 CT_Im_Cnr_6_13( UCHAR ch );
extern INT32 CT_Im_Cnr_6_14( UCHAR ch );
extern INT32 CT_Im_Cnr_6_15( UCHAR ch );
extern INT32 CT_Im_Cnr_6_16( UCHAR ch );
extern INT32 CT_Im_Cnr_6_17( UCHAR ch );
extern INT32 CT_Im_Cnr_6_18( UCHAR ch );
extern INT32 CT_Im_Cnr_6_19( UCHAR ch );
extern INT32 CT_Im_Cnr_6_20( UCHAR ch );
extern INT32 CT_Im_Cnr_6_21( UCHAR ch );
extern INT32 CT_Im_Cnr_6_22( UCHAR ch );
extern INT32 CT_Im_Cnr_6_23( UCHAR ch );
extern INT32 CT_Im_Cnr_6_24( UCHAR ch );

extern VOID CT_IM_CNR_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd );
extern VOID CT_IM_CNR_Main( INT32 argc, CHAR** argv );

#endif
