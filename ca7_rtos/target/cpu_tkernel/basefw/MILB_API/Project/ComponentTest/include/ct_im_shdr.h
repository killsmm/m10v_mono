/**
 * @file		ct_im_shdr.h
 * @brief		This is ct header for im_shdr.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _CT_IM_SHDR_H_
#define _CT_IM_SHDR_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define ct_im_shdr_roundup_16( val )					((((val) +15) /16) *16)
#define ct_im_shdr_roundup_32( val )					((((val) +31) /32) *32)
#define ct_im_shdr_roundup_128( val )					((((val) +127) /128) *128)
#define ct_im_shdr_roundup_256( val )					((((val) +255) /256) *256)
#define ct_im_shdr_roundup_16k( val )					((((val) +16383) /16384) *16384)
#define ct_im_shdr_roundup_ribery_width( width )		(ct_im_shdr_roundup_256((width)))
#define ct_im_shdr_roundup_ribery_lines( lines )		(ct_im_shdr_roundup_32((lines)))

#define D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF		(3840)
#define D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_REF		(2160)
#define D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD		(3840 + 32 * 2)
#define D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_STD		(2160 + 32 * 2)

#define D_IM_SHDR_DST_IMG_PIXS_WIDTH_HEVC				(3840)		// [A]HEVC   4K2K
#define D_IM_SHDR_DST_IMG_PIXS_LINES_HEVC				(2160)		// [A]HEVC   4K2K
#define D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN				(960)		// [B]ME input
#define D_IM_SHDR_DST_IMG_PIXS_LINES_MEIN				(540)		// [B]ME input
#define D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_FHD			(1920)		// [C]Ribery FHD
#define D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_FHD			(1080)		// [C]Ribery FHD
#define D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_HD				(1280)		// [D]Ribery HD
#define D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_HD				(720)		// [D]Ribery HD
#define D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI			(1280)		// [E]DISP   HDMI
#define D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_HDMI			(720)		// [E]DISP   HDMI
#define D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD			(640)		// [F]DISP   LCD
#define D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_LCD			(360)		// [F]DISP   LCD

#define D_IM_SHDR_IMG_MEM_IN0_ADDR_TOP					(gct_im_shdr_in_addr[ 0 ])
#define D_IM_SHDR_IMG_MEM_IN1_ADDR_TOP					(gct_im_shdr_in_addr[ 1 ])
#define D_IM_SHDR_IMG_MEM_IN2_ADDR_TOP					(gct_im_shdr_in_addr[ 2 ])
#define D_IM_SHDR_IMG_MEM_IN3_ADDR_TOP					(gct_im_shdr_in_addr[ 3 ])
#define D_IM_SHDR_IMG_MEM_IN4_ADDR_TOP					(gct_im_shdr_in_addr[ 4 ])

#define D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_TOP			(D_IM_SHDR_IMG_MEM_IN0_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_TOP			(D_IM_SHDR_IMG_MEM_IN1_ADDR_TOP)

#define D_IM_SHDR_IMG_MEM_4K2K_YCC_REF_U8_BYTES			(D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF * D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_REF * 3 / 2)
#define D_IM_SHDR_IMG_MEM_4K2K_YCC_STD_U8_BYTES			(D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD * D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_STD * 3 / 2)

#define D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_Y				(D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_C				(D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_Y + ( D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF * D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_REF ) )
#define D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_Y				(D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_C				(D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_Y + ( D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD * D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_STD ) )

#define D_IM_SHDR_IMG_MEM_OUT0_ADDR_TOP					(gct_im_shdr_out_addr[ 0 ])
#define D_IM_SHDR_IMG_MEM_OUT1_ADDR_TOP					(gct_im_shdr_out_addr[ 1 ])
#define D_IM_SHDR_IMG_MEM_OUT2_ADDR_TOP					(gct_im_shdr_out_addr[ 2 ])
#define D_IM_SHDR_IMG_MEM_OUT3_ADDR_TOP					(gct_im_shdr_out_addr[ 3 ])
#define D_IM_SHDR_IMG_MEM_OUT4_ADDR_TOP					(gct_im_shdr_out_addr[ 4 ])
#define D_IM_SHDR_IMG_MEM_OUT5_ADDR_TOP					(gct_im_shdr_out_addr[ 5 ])

#define D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_TOP				(D_IM_SHDR_IMG_MEM_OUT0_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_TOP				(D_IM_SHDR_IMG_MEM_OUT1_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_TOP			(D_IM_SHDR_IMG_MEM_OUT2_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_TOP			(D_IM_SHDR_IMG_MEM_OUT3_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_TOP		(D_IM_SHDR_IMG_MEM_OUT4_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_TOP			(D_IM_SHDR_IMG_MEM_OUT5_ADDR_TOP)

#define D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_WIDTH		(ct_im_shdr_roundup_ribery_width(D_IM_SHDR_DST_IMG_PIXS_WIDTH_HEVC))
#define D_IM_SHDR_IMG_MEM_HEVC_U8_C_GLOBAL_WIDTH		(ct_im_shdr_roundup_ribery_width(D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_WIDTH))
#define D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_LINES		(ct_im_shdr_roundup_ribery_lines( D_IM_SHDR_DST_IMG_PIXS_LINES_HEVC ))
#define D_IM_SHDR_IMG_MEM_HEVC_U8_C_GLOBAL_LINES		(ct_im_shdr_roundup_ribery_lines( D_IM_SHDR_DST_IMG_PIXS_LINES_HEVC /2 ))
#define D_IM_SHDR_IMG_MEM_HEVC_U8_BYTES					( ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_LINES) \
														+ ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_HEVC_U8_C_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_HEVC_U8_C_GLOBAL_LINES) )
#define D_IM_SHDR_IMG_MEM_MEIN_YCC_U8_BYTES				(D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN * D_IM_SHDR_DST_IMG_PIXS_LINES_MEIN)

#define D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_WIDTH		(ct_im_shdr_roundup_ribery_width(D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_FHD))
#define D_IM_SHDR_IMG_MEM_RIB_FHD_U8_C_GLOBAL_WIDTH		(ct_im_shdr_roundup_ribery_width(D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_WIDTH))
#define D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_LINES		(ct_im_shdr_roundup_ribery_lines( D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_FHD ))
#define D_IM_SHDR_IMG_MEM_RIB_FHD_U8_C_GLOBAL_LINES		(ct_im_shdr_roundup_ribery_lines( D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_FHD /2 ))
#define D_IM_SHDR_IMG_MEM_RIB_FHD_U8_BYTES				( ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_LINES) \
														+ ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_RIB_FHD_U8_C_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_RIB_FHD_U8_C_GLOBAL_LINES) )
#define D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_WIDTH		(ct_im_shdr_roundup_ribery_width(D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_HD))
#define D_IM_SHDR_IMG_MEM_RIB_HD_U8_C_GLOBAL_WIDTH		(ct_im_shdr_roundup_ribery_width(D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_WIDTH))
#define D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_LINES		(ct_im_shdr_roundup_ribery_lines( D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_HD ))
#define D_IM_SHDR_IMG_MEM_RIB_HD_U8_C_GLOBAL_LINES		(ct_im_shdr_roundup_ribery_lines( D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_HD /2 ))
#define D_IM_SHDR_IMG_MEM_RIB_HD_U8_BYTES				( ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_LINES) \
														+ ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_RIB_HD_U8_C_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_RIB_HD_U8_C_GLOBAL_LINES) )
#define D_IM_SHDR_IMG_MEM_DISP_HDMI_YCC_U8_BYTES		(D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI * D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_HDMI * 3 / 2)

#define D_IM_SHDR_IMG_MEM_DISP_LCD_YCC_U8_BYTES			(D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD  * D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_LCD  * 3 / 2)

#define D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_Y				(ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_TOP))
#define D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_C				(D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_Y      + ct_im_shdr_roundup_16k( D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_LINES ) )
#define D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_Y				(D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_C				(D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_Y      + ( D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN      * D_IM_SHDR_DST_IMG_PIXS_LINES_MEIN      ) )
#define D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_Y			(ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_TOP))
#define D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_C			(D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_Y   + ct_im_shdr_roundup_16k( D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_LINES ) )
#define D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_Y				(ct_im_shdr_roundup_16k(D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_TOP))
#define D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_C				(D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_Y    + ct_im_shdr_roundup_16k( D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_WIDTH * D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_LINES ) )
#define D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_Y			(D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_C			(D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_Y + ( D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI * D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_HDMI ) )
#define D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_Y			(D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_TOP)
#define D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_C			(D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_Y  + ( D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD  * D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_LCD  ) )

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
extern ULONG gct_im_shdr_in_addr[ 5 ];
extern ULONG gct_im_shdr_out_addr[ 6 ];

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
void Ct_Im_SHDR_Main(int argc, char** argv);

#endif	// _CT_IM_SHDR_H_
