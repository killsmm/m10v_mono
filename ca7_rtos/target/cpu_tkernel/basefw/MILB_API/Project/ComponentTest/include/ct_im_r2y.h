/**
 * @file		ct_im_r2y.h
 * @brief		This is ct header for im_r2y.c
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_IM_R2Y_H_
#define _CT_IM_R2Y_H_

#include "driver_common.h"
#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_IM_R2Y_SRC_IMG_PIXS_WIDTH	(640)
#define D_IM_R2Y_SRC_IMG_PIXS_LINES	(480)

#define D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_U8			(D_IM_R2Y_SRC_IMG_PIXS_WIDTH)
#define D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_P12			(D_IM_R2Y_SRC_IMG_PIXS_WIDTH * 3 / 2)
#define D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_U16			(D_IM_R2Y_SRC_IMG_PIXS_WIDTH * 2)
#define D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_BAY_U16			(D_IM_R2Y_SRC_IMG_PIXS_WIDTH * 2)



#define D_IM_R2Y_DST_IMG_PIXS_WIDTH	(640)
#define D_IM_R2Y_DST_IMG_PIXS_LINES	(480)

#define D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC444_Y			(D_IM_R2Y_DST_IMG_PIXS_WIDTH)
#define D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC444_C			(D_IM_R2Y_DST_IMG_PIXS_WIDTH)

#define D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_Y			(D_IM_R2Y_DST_IMG_PIXS_WIDTH)
#define D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_C			(D_IM_R2Y_DST_IMG_PIXS_WIDTH)

#define D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC420_Y			(D_IM_R2Y_DST_IMG_PIXS_WIDTH)
#define D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC420_C_P		(D_IM_R2Y_DST_IMG_PIXS_WIDTH / 2)
#define D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC420_C_SP		(D_IM_R2Y_DST_IMG_PIXS_WIDTH)



#define D_IM_R2Y_IMG_MEM_IN_ADDR_TOP					(gct_im_r2y_in_addr)

#define D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP				(D_IM_R2Y_IMG_MEM_IN_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES				(D_IM_R2Y_SRC_IMG_PIXS_WIDTH * D_IM_R2Y_SRC_IMG_PIXS_LINES * 3)
#define D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES				(D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_P12 * D_IM_R2Y_SRC_IMG_PIXS_LINES * 3)
#define D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES				(D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_U16 * D_IM_R2Y_SRC_IMG_PIXS_LINES * 3)
#define D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES				(D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_BAY_U16 * D_IM_R2Y_SRC_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_IN_RGB_U8_R_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_IN_RGB_U8_G_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_U8_R_ADDR_TOP + D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_U8 * D_IM_R2Y_SRC_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_IN_RGB_U8_B_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_U8_G_ADDR_TOP + D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_U8 * D_IM_R2Y_SRC_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_IN_RGB_P12_R_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_IN_RGB_P12_G_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_P12_R_ADDR_TOP + D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_P12 * D_IM_R2Y_SRC_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_IN_RGB_P12_B_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_P12_G_ADDR_TOP + D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_P12 * D_IM_R2Y_SRC_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_IN_RGB_U16_R_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_IN_RGB_U16_G_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_U16_R_ADDR_TOP + D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_U16 * D_IM_R2Y_SRC_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_IN_RGB_U16_B_ADDR_TOP			(D_IM_R2Y_IMG_MEM_IN_RGB_U16_G_ADDR_TOP + D_IM_R2Y_SRC_IMG_GLOBAL_WIDTH_RGB_U16 * D_IM_R2Y_SRC_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP				(D_IM_R2Y_IMG_MEM_IN_ADDR_TOP)


#define D_IM_R2Y_IMG_MEM_OUT_ADDR_TOP					(gct_im_r2y_out_addr[0])

#define D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP				(D_IM_R2Y_IMG_MEM_OUT_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_OUT_YCC444_BYTES				(D_IM_R2Y_DST_IMG_PIXS_WIDTH * D_IM_R2Y_DST_IMG_PIXS_LINES * 3)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES				(D_IM_R2Y_DST_IMG_PIXS_WIDTH * D_IM_R2Y_DST_IMG_PIXS_LINES * 2)
#define D_IM_R2Y_IMG_MEM_OUT_YCC420_BYTES				(D_IM_R2Y_DST_IMG_PIXS_WIDTH * D_IM_R2Y_DST_IMG_PIXS_LINES * 3 / 2)

#define D_IM_R2Y_IMG_MEM_OUT_YCC444_Y_ADDR_TOP			(D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_OUT_YCC444_CB_ADDR_TOP			(D_IM_R2Y_IMG_MEM_OUT_YCC444_Y_ADDR_TOP  + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC444_Y * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC444_CR_ADDR_TOP			(D_IM_R2Y_IMG_MEM_OUT_YCC444_CB_ADDR_TOP + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC444_C * D_IM_R2Y_DST_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP			(D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP			(D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP  + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_Y * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CR_ADDR_TOP			(D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_C * D_IM_R2Y_DST_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_0		(D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_0		(D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CR_ADDR_TOP_0		(D_IM_R2Y_IMG_MEM_OUT_YCC422_CR_ADDR_TOP)

#define D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_1		(D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_0 + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_C * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_1		(D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_1  + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_Y * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CR_ADDR_TOP_1		(D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_1 + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_C * D_IM_R2Y_DST_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_2		(D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_1 + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_C * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_2		(D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_2  + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_Y * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CR_ADDR_TOP_2		(D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_2 + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_C * D_IM_R2Y_DST_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_3		(D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_2 + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_C * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_3		(D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_3  + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_Y * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC422_CR_ADDR_TOP_3		(D_IM_R2Y_IMG_MEM_OUT_YCC422_CB_ADDR_TOP_3 + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC422_C * D_IM_R2Y_DST_IMG_PIXS_LINES)

#define D_IM_R2Y_IMG_MEM_OUT_YCC420_Y_ADDR_TOP			(D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP)
#define D_IM_R2Y_IMG_MEM_OUT_YCC420_CB_ADDR_TOP			(D_IM_R2Y_IMG_MEM_OUT_YCC420_Y_ADDR_TOP  + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC420_Y    * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC420_CR_P_ADDR_TOP		(D_IM_R2Y_IMG_MEM_OUT_YCC420_CB_ADDR_TOP + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC420_C_P  * D_IM_R2Y_DST_IMG_PIXS_LINES)
#define D_IM_R2Y_IMG_MEM_OUT_YCC420_CR_SP_ADDR_TOP		(D_IM_R2Y_IMG_MEM_OUT_YCC420_CB_ADDR_TOP + D_IM_R2Y_DST_IMG_GLOBAL_WIDTH_YCC420_C_SP * D_IM_R2Y_DST_IMG_PIXS_LINES)


#define D_IM_R2Y_TBL_MEM_OUT_TCTDATA					(40 * 30 * 4)
#define D_IM_R2Y_TBL_MEM_OUT_TCHSYDATA					(4096 * 2)
#define D_IM_R2Y_TBL_MEM_OUT_TCHSRGBDATA				(256 * 2)

#define D_IM_R2Y_IMG_MEM_OUT2_ADDR_TOP					(gct_im_r2y_out_addr[1])

#define D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCTDATA			(D_IM_R2Y_IMG_MEM_OUT2_ADDR_TOP)
#define D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSYDATA			(D_IM_R2Y_IMG_MEM_OUT2_ADDR_TOP)
#define D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSRDATA			(D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSYDATA + D_IM_R2Y_TBL_MEM_OUT_TCHSYDATA)
#define D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSGDATA			(D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSRDATA + D_IM_R2Y_TBL_MEM_OUT_TCHSRGBDATA)
#define D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSBDATA			(D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSGDATA + D_IM_R2Y_TBL_MEM_OUT_TCHSRGBDATA)


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
extern ULONG gct_im_r2y_in_addr;
extern ULONG gct_im_r2y_out_addr[2];


/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern VOID CT_Im_R2y_PrintReg( VOID );
extern INT32 CT_Im_R2y_1_1( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_2( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_3( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_4( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_5( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_6( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_7( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_8( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_9( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_10( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_11( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_12( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_13( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_14( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_15( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_16( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_17( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_18( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_19( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_20( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_21( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_22( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_23( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_24( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_25( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_26( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_27( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_28( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_29( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_30( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_31( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_32( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_33( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_34( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_35( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_36( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_37( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_38( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_39( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_40( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_41( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_42( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_43( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_44( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_45( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_46( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_47( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_48( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_49( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_50( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_51( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_52( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_53( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_54( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_55( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_56( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_57( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_58( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_59( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_60( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_61( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_62( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_63( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_64( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_65( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_66( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_67( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_68( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_69( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_70( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_71( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_72( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_73( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_74( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_75( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_76( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_77( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_78( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_79( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_80( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_81( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_82( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_83( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_84( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_85( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_86( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_87( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_88( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_89( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_90( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_91( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_92( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_93( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_94( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_95( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_96( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_97( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_98( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_99( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_100( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_101( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_102( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_103( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_104( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_105( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_106( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_107( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_108( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_109( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_110( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_111( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_112( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_113( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_114( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_115( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_116( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_117( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_118( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_119( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_120( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_121( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_122( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_123( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_124( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_125( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_126( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_127( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_128( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_129( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_130( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_131( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_132( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_133( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_134( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_135( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_136( UCHAR pipe_no );
extern INT32 CT_Im_R2y_1_137( UCHAR pipe_no );

extern INT32 CT_Im_R2y_2_1( UCHAR pipe_no );
extern INT32 CT_Im_R2y_2_2( UCHAR pipe_no );

extern INT32 CT_Im_R2y_3_1( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_2( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_3( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_4( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_5( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_6( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_7( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_8( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_9( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_10( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_11( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_12( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_13( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_14( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_15( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_16( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_17( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_18( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_19( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_20( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_21( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_22( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_23( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_24( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_25( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_26( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_27( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_28( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_29( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_30( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_31( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_32( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_33( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_34( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_35( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_36( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_37( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_38( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_39( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_40( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_41( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_42( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_44( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_46( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_48( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_50( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_51( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_52( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_53( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_54( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_55( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_56( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_57( UCHAR pipe_no );
extern INT32 CT_Im_R2y_3_58( UCHAR pipe_no );

extern VOID CT_Im_R2Y_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd );
extern VOID CT_Im_R2Y_Main( INT32 argc, CHAR** argv );

#endif
