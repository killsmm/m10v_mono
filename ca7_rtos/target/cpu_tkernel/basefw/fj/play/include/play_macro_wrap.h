/**
 * @file		play_macro_wrap.h
 * @brief		playback macro wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _PLAY_MACRO_WRAP_H_
#define _PLAY_MACRO_WRAP_H_

#include "im_xch.h"
#include "im_disp.h"
#include "im_iip.h"

#include "sdram_map_play.h"
// --- REMOVE_DCF_EXIF BEGIN ---
#include "dcf_if.h"
// --- REMOVE_DCF_EXIF END ---

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
// Limit size of Encode Main
// see: T_IM_JPEG_ENC_MNG->limit_size
// limit_size should be able to be divided by 512.
#define D_BF_PLAY_MACRO_WRAP_LIMIT_JPEG_MAIN_ENC	(((PIX_SIZ_36M / 2) / 512) * 512)
#define	D_BF_PLAY_MACRO_WRAP_LIMIT_JPEG_THUMB_ENC	((PIX_SIZ_THUMB / 512) * 512)
#define D_BF_PLAY_MACRO_WRAP_LIMIT_JPEG_CLS2_ENC    ((PIX_SIZ_FHD / 512) * 512) 
#define D_BF_PLAY_MACRO_WRAP_LIMIT_JPEG_CLS1_ENC    ((PIX_SIZ_VGA / 512) * 512)
#define	D_BF_PLAY_MACRO_WRAP_JPEG_SCR_ENC_LIMIT		(0xFE00)	// APP2 area limit size = 0xFFFF (include APP2 header)
#define D_BF_PLAY_MACRO_WRAP_JPEG_YCC_COUNT			(3)

#define D_BF_PLAY_MACRO_WRAP_FILL_BLACK_Y		(0x00)
#define D_BF_PLAY_MACRO_WRAP_FILL_BLACK_CB		(0x80)
#define D_BF_PLAY_MACRO_WRAP_FILL_BLACK_CR		(0x80)

/**
 * Sync or Async
 */
#define D_BF_PLAY_MACRO_WRAP_SYNC	(0)
#define D_BF_PLAY_MACRO_WRAP_ASYNC	(1)

/**
 * Resize mode select
 */
#define D_BF_PLAY_MACRO_WRAP_RESIZE_BILINEAR	(0)
#define D_BF_PLAY_MACRO_WRAP_RESIZE_NEAREST	(1)
#define D_BF_PLAY_MACRO_WRAP_RESIZE_BICUBIC	(2)

/**
 * default wait time
 */
#define D_BF_PLAY_MACRO_WRAP_WAIT_TIME	(D_DDIM_WAIT_END_TIME)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
/**
 * JPEG Quantization flag
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_JPEG_QT_OFF = 0,	/* not use Quantization table */
	E_BF_PLAY_MACRO_WRAP_JPEG_QT_ON  = 1,	/* use Quantization table */
	E_BF_PLAY_MACRO_WRAP_JPEG_QT_IND = 2	/* use Individual Quantization table */
} E_BF_PLAY_MACRO_WRAP_JPEG_QT;

/**
 *
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION_RIGHT				= 0,
	E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION_LEFT				= 1,
	E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION_RIGHT_UD_MIRROR	= 2,
	E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION_LEFT_UD_MIRR0R		= 3,
	E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION_180				= 4,
	E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION_LR_MIRROR			= 5,
	E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION_UD_MIRROR			= 6,
	E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION_NON				= 7
} E_BF_PLAY_MACRO_WRAP_IMAGE_DIRECTION;

/**
 * Aspect Adjust flag
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_ADJUST_OFF = 0,		/* not use Aspect Adjust */
	E_BF_PLAY_MACRO_WRAP_ADJUST_ON  = 1			/* use Aspect Adjust */
} E_BF_PLAY_MACRO_WRAP_ADJUST;

/**
 * Fill target
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_FILL_ALL = 0,	/* Fill Draw (YCC All) */
	E_BF_PLAY_MACRO_WRAP_FILL_Y,		/* Fill Draw (Y) */
	E_BF_PLAY_MACRO_WRAP_FILL_CC,		/* Fill Draw (CbCr) */
	E_BF_PLAY_MACRO_WRAP_FILL_OFF		/* Fill Off */
} E_BF_PLAY_MACRO_WRAP_FILL;

/**
 * parameter setting format
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG = 0,
	E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR,
	E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG,
} E_BF_PLAY_MACRO_WRAP_SET_FORM;

/**
 * 
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE_PB = 0,
	E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE_OSD,
} E_BF_PLAY_MACRO_WRAP_DISPLAY_TYPE;

/**
 * Encode target
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_ENC_TYPE_MAIN = 0,
	E_BF_PLAY_MACRO_WRAP_ENC_TYPE_SCREEN,
	E_BF_PLAY_MACRO_WRAP_ENC_TYPE_THUMB,
} E_BF_PLAY_MACRO_WRAP_ENC_TYPE;

/**
 * Aspect type
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_UNDEFINE = 0,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_4_3,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_16_9,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_2,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_5_4,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_4,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_9_16,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_2_3,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_4_5,
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_1_1
} E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE;

/**
 * Use channel type
 */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_USED_1CH = 0,    /* use x1ch */
	E_BF_PLAY_MACRO_WRAP_USED_2CH    	  /* use x2ch together */
} E_BF_PLAY_MACRO_WRAP_USED_CH;

/**
 * Image convert type select
 */
typedef enum{
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_444_444 = 0,		/**< 444 to 444 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_444_422,			/**< 444 to 422 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_444_420,			/**< 444 to 420 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_422_422,			/**< 422 to 422 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_422_420,			/**< 422 to 420 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_420_420,			/**< 420 to 420 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_420_422,			/**< 420 to 422 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_420_444,			/**< 420 to 444 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_RIB_444,			/**< Ribery to 444 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_RIB_422,			/**< Ribery to 422 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_RIB_420,			/**< Ribery to 420 */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_444_RIB,			/**< 444 to Ribery */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_422_RIB,			/**< 422 to Ribery */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_420_RIB,			/**< 420 to Ribery */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_RIB_RIB,			/**< Ribery to Ribery */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE_NONE				/**< not */
} E_BF_PLAY_MACRO_WRAP_CONV_TYPE;

/**
 * Image pixel format
 */
typedef enum{
	E_BF_PLAY_MACRO_WRAP_PFMT_YCC444 = 0,			/**< YCC422 */
	E_BF_PLAY_MACRO_WRAP_PFMT_YCC422,				/**< YCC444 */
	E_BF_PLAY_MACRO_WRAP_PFMT_YCC420,				/**< YCC420 */
	E_BF_PLAY_MACRO_WRAP_PFMT_NONE					/**< not */
} E_BF_PLAY_MACRO_WRAP_PFMT;

/** HDMI/TV and LCD enable disply device */
typedef enum {
	E_BF_PLAY_MACRO_WRAP_EFFECTIVE_NONE = 0,		/**< Show nothing */
	E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD  = 1,		/**< Enable show LCD */
	E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI = 2,		/**< Enable show HDMI	*/
} E_BF_PLAY_MACRO_WRAP_EFFECTIVE_DEVICE;

/**
 * Jpeg sampling type
 */
typedef enum{
	E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC444 = 0,		/**< YCC444 */
	E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422,			/**< YCC422 */
	E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420,			/**< YCC420 */
	E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC400,			/**< YCC400 */
	E_BF_PLAY_MACRO_WRAP_JPG_SMPL_MAX				/**< not use */
} E_BF_PLAY_MACRO_WRAP_JPG_SMPL;

/**
 * Jpeg memory format
 */
typedef enum{
	E_BF_PLAY_MACRO_WRAP_JPG_MFMT_PLANAR = 0,		/**< planar */
	E_BF_PLAY_MACRO_WRAP_JPG_MFMT_SEMI_PLANAR,		/**< semi-planar */
	E_BF_PLAY_MACRO_WRAP_JPG_MFMT_TILE,				/**< tile */
	E_BF_PLAY_MACRO_WRAP_JPG_MFMT_MAX				/**< not use */
} E_BF_PLAY_MACRO_WRAP_JPG_MFMT;

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
/**
 * structure of fill adjust for size convert
 */
typedef struct {
	USHORT	after_rec_gl_width;
	USHORT	after_rec_width;
	USHORT	after_rec_lines;
	USHORT	left_fill_width;
	USHORT	up_fill_lines;
} T_BF_PLAY_MACRO_WRAP_FILL_ADJUST;

/**
 * structure of address
 */
typedef struct {
	ULONG addr_a;			/* address A (ex.Y) */
	ULONG addr_b;			/* address B (ex.Cb) */
	ULONG addr_c;			/* address B (ex.Cr) */
} T_BF_PLAY_MACRO_WRAP_ADDR;

/**
 * structure of color
 */
typedef struct {
	UCHAR fill_y_color;		/* color A (ex.Y) */
	UCHAR fill_cb_color;	/* color B (ex.Cb) */
	UCHAR fill_cr_color;	/* color C (ex.Cr) */
} T_BF_PLAY_MACRO_WRAP_COLOR;

/**
 * structure of color pos
 */
typedef struct {
	USHORT fill_x_pos;		/* color X pos (ex.X) */
	USHORT fill_y_pos;		/* color Y pos (ex.Y) */
} T_BF_PLAY_MACRO_WRAP_FILL_POS;

/**
 * structure of size
 */
typedef struct {
	USHORT width;			/* width */
	USHORT lines;			/* lines */
	USHORT global_width;	/* global width */
	USHORT enc_width;		/* encode width */
} T_BF_PLAY_MACRO_WRAP_SIZE;

/**
 * structure of size and address
 */
typedef struct {
	T_BF_PLAY_MACRO_WRAP_SIZE size;		/* size    */
	T_BF_PLAY_MACRO_WRAP_ADDR addrs;	/* address */
} T_BF_PLAY_MACRO_WRAP_SIZE_ADDR;

/**
 * structure of size and address
 */
typedef struct {
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	src_param;		/* source size & address */
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	dst_param;		/* destination size & address */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE	conv_type;		/* image convert type select */
} T_BF_PLAY_MACRO_WRAP_RESIZE_PARAM;

/* Display Window parameter */
typedef struct {
	USHORT		gl_width;				/* global width */
	USHORT		gl_lines;				/* global lines */
	USHORT		width;					/* width */
	USHORT		lines;					/* lines */
	USHORT		pos_x;					/* position x */
	USHORT		pos_y;					/* position y */
	T_BF_PLAY_MACRO_WRAP_ADDR	addr;	/* address Y */
} T_BF_PLAY_MACRO_WRAP_DISP_PARAM;

typedef struct {
	UCHAR	mode;
	USHORT	src_h_size;
	USHORT	dst_filed_h_size;
	USHORT	dst_h_size;
	USHORT	dst_v_size;
	ULONG	hen;
	ULONG	ven;
	ULONG	src_addrs;
	ULONG	dst_addrs;
	UCHAR	fill_data;
	UCHAR	hcyc;
	UCHAR	vcyc;
	UCHAR	src_offset;
	UCHAR	dst_offset;
} T_BF_PLAY_MACRO_WRAP_THIN_OUT_PARAM;

/* for one componet */
typedef struct {
	ULONG	src_addr[3];	/* source data address */
	ULONG	dst_addr[3];	/* distination data address */
	ULONG   afn_addr[3];	/* affine data address */
	ULONG	src_width[3];	/* source image width */
	ULONG	src_lines[3];	/* source image lines */
	ULONG	org_width[3];	/* input global image lines */
	ULONG	dst_width[3];	/* distination image width */
	ULONG	dst_lines[3];	/* distination image lines */
	CHAR	direction;		/* rotate direction */
	E_BF_PLAY_MACRO_WRAP_CONV_TYPE	mode;
} T_BF_PLAY_MACRO_WRAP_ROTATE_INFO;

/**
 * structure of ycc addr and encode addr
 */
typedef struct {
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	param;		/* ycc param */
	ULONG							enc_addr;	/* encode address */
} T_BF_PLAY_MACRO_WRAP_SAVE_PARAM;

typedef struct {
	UCHAR	mode;			/**< XMD, Operation mode at XCTL */
	UCHAR	fill_data;		/**< XFDT, Fill data at XCTL */
	UCHAR	hcyc;			/**< XHCYC, Horizontal downsampling cycle at XCYC */
	UCHAR	vcyc;			/**< XVCYC, Vertical downsampling cycle at XCYC */
	UINT32	hen;			/**< XHEN0,1 : Horizontal import data at XHEN */
	UINT32	ven;			/**< XVEN0,1 : Vertical import data at XVEN */
	USHORT	s_hsize;		/**< XSHW, Source image horizontal size at XSHW */
	USHORT	g_hsize;		/**< XGHW, Global image horizontal size at XGHW */
	USHORT	d_hsize;		/**< XDHW, Destination image horizontal size at XDHW */
	USHORT	d_vsize;		/**< XDVW, Destination image vertical size at XDVW */
	UINT32	src_addr;		/**< XSA, Source image area address at XSA0,1 */
	UINT32	dst_addr;		/**< XDA, Destination image area address at XDA0,1 */
	UCHAR	hcnt;			/**< XHCNT, Continuance of histogram data acquisition at XHCNT */
	VP_CALLBACK	pCallBack;	/**< Call Back function pointer */
} T_BF_PLAY_MACRO_WRAP_XCH_MNG;

typedef struct {
	struct {
		ULONG		Address[3];
		UCHAR		gain[3];
	} src1, src2;
	struct {
		ULONG		Address[3];
	} dst;
	struct {
		USHORT		s_width;
		USHORT		d_width;
		USHORT		width;
		USHORT		lines;
	} size;
} T_BF_PLAY_MACRO_WRAP_MBCH_MIX_INFO;

/*  wrapper of the Disp macro structure */
typedef struct {
	T_IM_DISP_IMAGE_ADDR	addrs;
} T_BF_PLAY_MACRO_WRAP_DISP_ADDR;

/* Used by xch copy */
typedef struct{ 
	struct {
		ULONG 	src_addr;
		USHORT 	src_gl_width;
		USHORT 	dst_gl_width;
		ULONG 	dst_addr;
		USHORT 	width;
		USHORT 	lines;
	} y, c;
} T_BF_PLAY_MACRO_WRAP_YCC_COPY_PARAM;

/* Used by xch fill */
typedef struct{
	E_BF_PLAY_MACRO_WRAP_FILL 	fill_type;
	UCHAR 	fill_data_y;
	UCHAR 	fill_data_cb;
	UCHAR 	fill_data_cr;
	struct {
		ULONG 	dst_addr;
		USHORT 	gl_width;
		USHORT 	width;
		USHORT 	lines;
	} y, c;
} T_BF_PLAY_MACRO_WRAP_YCC_FILL_PARAM;

/**
 *	structure of the Jpeg decode parameter
 */
typedef struct {
	E_BF_PLAY_MACRO_WRAP_JPG_MFMT	mem_format;		// Memory format
	E_BF_PLAY_MACRO_WRAP_JPG_SMPL	ycbcr_smpl;		// Sub-simpling type
	ULONG							gl_width;		// Width of the Y component area
	ULONG							gl_lines;		// Lines of the Y component area
	ULONG							ycbcr_addr[3];	// Address of the each component
	ULONG							code_addr;		// Jpeg decode address
	ULONG							code_size;		// Jpeg code size
	USHORT							width;			// Width of the original jpeg image
	USHORT							lines;			// Lines of the original jpeg image
} T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG;

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/
/* use DISP */
VOID		BF_Play_Macro_Wrap_Display( E_IM_DISP_SEL disp_sel, const T_BF_PLAY_MACRO_WRAP_DISP_ADDR* src_addr );
VOID		BF_Play_Macro_Wrap_Display_Lcd( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR vga_size_addr );
VOID		BF_Play_Macro_Wrap_Display_Hdmi( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR hd_size_addr );
VOID		BF_Play_Macro_Wrap_Show_Lcd( VOID );
VOID		BF_Play_Macro_Wrap_Show_Hdmi( VOID );
FJ_ERR_CODE BF_Play_Macro_Wrap_Get_Hdmi_Output_Size(T_BF_PLAY_MACRO_WRAP_SIZE* output_size);

/* use Xch */
VOID		BF_Play_Macro_Wrap_Make_Image_With_Back(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src_image, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR dst_image, E_BF_PLAY_MACRO_WRAP_PFMT dst_ycc_type);
VOID		BF_Play_Macro_Wrap_Fill_Image(VOID* fill_addrs, 
											USHORT fill_gl_width, USHORT fill_width, USHORT fill_lines, 
											UCHAR fill_y_color, UCHAR fill_cb_color, UCHAR fill_cr_color, 
											E_BF_PLAY_MACRO_WRAP_PFMT ycc_type, 
											E_BF_PLAY_MACRO_WRAP_SET_FORM fill_form, 
											E_BF_PLAY_MACRO_WRAP_FILL fill_type,
											E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode);
VOID		BF_Play_Macro_Wrap_Copy_Image(ULONG src_addr_y, ULONG src_addr_c, USHORT src_gl_width, 
											ULONG dst_addr_y, ULONG dst_addr_c, USHORT dst_gl_width, 
											USHORT copy_width, USHORT copy_lines,
											E_BF_PLAY_MACRO_WRAP_PFMT ycc_type, 
											E_BF_PLAY_MACRO_WRAP_USED_CH ch_mode);
FJ_ERR_CODE	BF_Play_Macro_Wrap_Get_Histogram_Data( USHORT width, USHORT lines, USHORT gl_width, ULONG hist_addr );


/* use JPEG */
// --- REMOVE_DCF_EXIF BEGIN ---
FW_INT32	BF_Play_Macro_Wrap_Get_Jpeg( FJ_MODE mode, ULONG image_index, UCHAR* load_addr, UCHAR* decode_addr, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, BOOL dec_flag, BOOL bDetect_mode_change );
INT32		BF_Play_Macro_Wrap_Encoder_Jpeg( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src_size_addrs, ULONG dst_addr, E_BF_PLAY_MACRO_WRAP_JPEG_QT qt, T_BF_DCF_IF_JPEG_QT* quant,
											 ULONG* dst_size, E_BF_PLAY_MACRO_WRAP_ENC_TYPE enc, ULONG max_size, E_BF_PLAY_MACRO_WRAP_JPG_SMPL ycc_smpl );
/* use IIP(Resize and Rotate) */
INT32		BF_Play_Macro_Wrap_Rotate_Convert_Image( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src, E_BF_PLAY_MACRO_WRAP_PFMT src_ycc_type, 
															T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst, E_BF_PLAY_MACRO_WRAP_PFMT dst_ycc_type,
											   				E_BF_DCF_IF_ROTATE rotate, E_BF_PLAY_MACRO_WRAP_ADJUST aspect );

// --- REMOVE_DCF_EXIF END ---
FJ_ERR_CODE	BF_Play_Macro_Wrap_Decode_Jpeg( T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpg_dec_mng, INT32 tout );

/* use IIP(LUT) */
VOID		BF_Play_Macro_Wrap_Set_Lut_Param( T_BF_PLAY_MACRO_WRAP_ADDR src_addr, 
												USHORT src_gl_width, USHORT src_width, USHORT src_lines,
												T_BF_PLAY_MACRO_WRAP_ADDR dst_addr,
												USHORT dst_gl_width, USHORT dst_width, USHORT dst_lines,
												E_IM_IIP_PFMT iip_pfmt, T_IM_IIP_LUTRAM iip_lutram,
												T_IM_IIP_UTIL_LUT* iip_util_lut );
INT32		BF_Play_Macro_Wrap_Lut_Image( T_IM_IIP_UTIL_LUT* iip_util_lut );

/* use IIP(AlphaBlend) */
VOID		BF_Play_Macro_Wrap_Set_AlphaBlend_Common_Param( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src1, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src2,
										T_BF_PLAY_MACRO_WRAP_SIZE_ADDR dst_size,
										T_IM_IIP_UTIL_ALPHABLEND* alpha_blend_param );
INT32		BF_Play_Macro_Wrap_AlphaBlend_Image( T_IM_IIP_UTIL_ALPHABLEND* iip_util_blend );

/* common */
VOID		BF_Play_Macro_Wrap_Set_Size_Addr( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* set_size_addrs, VOID*  set_addrs, USHORT set_width, USHORT set_lines, USHORT set_global_width, E_BF_PLAY_MACRO_WRAP_SET_FORM set_form );

VOID		BF_Play_Macro_Wrap_Set_Adjust_Param( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR src, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst, E_BF_PLAY_MACRO_WRAP_PFMT dst_ycc_type, E_BF_PLAY_MACRO_WRAP_ADJUST adjust_onoff );

UCHAR		BF_Play_Macro_Wrap_Get_Effective_Device( VOID );

BOOL		BF_Play_Macro_Wrap_Check_Jpeg_Thumbnail(ULONG image_index);

#endif	// _PLAY_MACRO_WRAP_H_
