/**
 * @file		osd_blend_table.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef OSD_BLEND_TABLE_H_
#define OSD_BLEND_TABLE_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define BF_OSD_BLEND_TABLE_BYTE_DIV_BY_CPU
#define BF_OSD_BLEND_TABLE_BYTE_DIV_CCCA_BY_CPU

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifndef BF_OSD_BLEND_TABLE_BYTE_DIV_BY_CPU
extern const BYTE gBF_Osd_Blend_Table_Div_LUT[256][256];
#endif // BF_OSD_BLEND_TABLE_BYTE_DIV_BY_CPU

#ifndef BF_OSD_BLEND_TABLE_BYTE_DIV_CCCA_BY_CPU
extern const UINT32 gBF_Osd_Blend_Table_Div_LUT_CCCA[256][256];
#endif // BF_OSD_BLEND_TABLE_BYTE_DIV_CCCA_BY_CPU

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#if 1
#define BF_OSD_BLEND_TABLE_MULTI_255_TO(val) ((val << D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL) - val)
#else
#define BF_OSD_BLEND_TABLE_MULTI_255_TO(val) (val * 255)
#endif


#ifdef BF_OSD_BLEND_TABLE_BYTE_DIV_BY_CPU
// non-data cache version
#define BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B(A, B) 			(BF_OSD_BLEND_TABLE_MULTI_255_TO(A)/B)
// data cache version, need to specify the cacheable address of the LUT 
#define BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B(tbl, A, B) 		(BF_OSD_BLEND_TABLE_MULTI_255_TO(A)/B)
#else
// non-data cache version [RECOMMENDED, even faster]
#define BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B(A, B) 			(gBF_Osd_Blend_Table_Div_LUT[A][B])
// data cache version, need to specify the cacheable address of the LUT 
#define BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B_TBL(tbl, A, B) 	(tbl[(A<<D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL) + B])
#endif // BF_OSD_BLEND_TABLE_BYTE_DIV_BY_CPU


#ifndef BF_OSD_BLEND_TABLE_BYTE_DIV_CCCA_BY_CPU
// non-data cache version [RECOMMENDED, even faster]
#define BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B_CCCA(A, B) 			(gBF_Osd_Blend_Table_Div_LUT_CCCA[A][B])
// data cache version, need to specify the cacheable address of the LUT 
#define BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B_CCCA_TBL(tbl, A, B) 	(tbl[(A<<8) + B])
#endif // BF_OSD_BLEND_TABLE_BYTE_DIV_CCCA_BY_CPU


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
// Nothing Special


#endif /* OSD_BLEND_TABLE_H_ */
