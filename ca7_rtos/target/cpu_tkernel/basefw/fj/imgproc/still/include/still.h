/**
 * @file		still.h
 * @brief		capture operator
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_H_
#define _STILL_H_

// FJ API
#include "fj_still.h"

// Capture
#include "still_img_info.h"
#include "jpgepc.h"

// --- REMOVE_DCF_EXIF BEGIN ---
// FJ Other
#include "dcf_if.h"	// for T_BF_DCF_IF_EXIF_INFO
// --- REMOVE_DCF_EXIF END ---

#include "ddim_typedef.h"

#include "im_mxic.h"
// delete\basefw\fj\imgproc\still
#include "im_pro.h"
// delete\basefw\fj\imgproc\still
//#include "still_ela_nr.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define MAX_OF_CONTINUOUS 		 (10)


// VGA Work Area Size for Qview and Screennail
#define D_CAPTURE_QVIEW_WORK_Y_WIDTH	(WIDTH_VGA+32)							// 672
#define D_CAPTURE_QVIEW_WORK_C_WIDTH	(D_CAPTURE_QVIEW_WORK_Y_WIDTH>>1)		// 336
#define D_CAPTURE_QVIEW_WORK_Y_LINES	(LINES_VGA+32)							// 512
#define D_CAPTURE_QVIEW_WORK_C_LINES	(LINES_VGA+32)							// 512
#define D_CAPTURE_QVIEW_WORK_Y_SIZE		(D_CAPTURE_QVIEW_WORK_Y_WIDTH*D_CAPTURE_QVIEW_WORK_Y_LINES)
#define D_CAPTURE_QVIEW_WORK_C_SIZE		(D_CAPTURE_QVIEW_WORK_C_WIDTH*D_CAPTURE_QVIEW_WORK_C_LINES)
#define D_CAPTURE_QVIEW_WORK_SIZE		(D_CAPTURE_QVIEW_WORK_Y_SIZE+(D_CAPTURE_QVIEW_WORK_C_SIZE<<1))

#define D_BF_STILL_HIST_MAX		(256)	// Xch histogram buffer number
// delete\basefw\fj\imgproc\still
#define D_BF_STILL_PRO_CTRL_SEN_RAW_OUT_PORT	(0x4)
#define D_BF_STILL_PRO_CTRL_SRO_RAW_OUT_PORT	(0x1)
#define D_BF_STILL_PRO_CTRL_SRO_AFPB_OUT_PORT	(0x2)
#define D_BF_STILL_PRO_CTRL_B2B_RAW_OUT_PORT	(0x1)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum{	// Internal Capture mode
	E_CAP_MODE_12BIT_BAYER 			= 0	,
	E_CAP_MODE_16BIT_BAYER 				,
	E_CAP_MODE_8BIT_BAYER 				,
	E_CAP_MODE_CALIBRATION				,
	E_CAP_MODE_PHOTO_STITCH_12BIT		,
	E_CAP_MODE_PHOTO_STITCH_16BIT		,
	E_CAP_MODE_PHOTO_STITCH_8BIT		,
	E_CAP_MODE_HIGH_LIGHT_12BIT			,
	E_CAP_MODE_HIGH_LIGHT_16BIT			,
	E_CAP_MODE_HIGH_LIGHT_8BIT			,
	E_CAP_MODE_MAX
} E_CAP_MODE;

typedef enum {
	E_CONVERT_MODE_NEAREST = 0,
	E_CONVERT_MODE_BI_LINEAR = 1,
	E_CONVERT_MODE_BI_CUBIC = 2,
} E_CONVERT_MODE;

typedef enum {
	E_RESIZE_MODE_NEAREST = 0,
	E_RESIZE_MODE_BI_LINEAR = 1,
	E_RESIZE_MODE_BI_CUBIC = 2,
} E_RESIZE_MODE;

// delete\basefw\fj\imgproc\still
typedef enum {
	BF_STILL_PRO_BLOCK_TYPE_SEN = 0,	/**< SEN block	*/
	BF_STILL_PRO_BLOCK_TYPE_SRO,		/**< SRO block	*/
	BF_STILL_PRO_BLOCK_TYPE_B2B,		/**< B2B block	*/
	BF_STILL_PRO_BLOCK_TYPE_STAT,		/**< STAT block	*/
	BF_STILL_PRO_BLOCK_TYPE_PAS,		/**< PAS block	*/
	BF_STILL_PRO_BLOCK_TYPE_MAX,		/**< MAX value	*/
} BF_STILL_PRO_BLOCK_TYPE;

/** PRO Unit Number
*/
typedef enum {
	BF_STILL_PRO_UNIT_NUM_1 = 0,		/**< UNIT number 1 block	*/
	BF_STILL_PRO_UNIT_NUM_2,			/**< UNIT number 2 block	*/
	BF_STILL_PRO_BOTH_UNIT,				/**< UNIT 1 & 2 block	*/
	BF_STILL_PRO_UNIT_NUM_MAX,			/**< MAX value	*/
} BF_STILL_PRO_UNIT_NUM;

typedef enum {
	BF_STILL_PRO_VD_SEL = 0,		/**< VD interrupt	*/
	BF_STILL_PRO_HD_SEL,			/**< HD interrupt	*/
} BF_STILL_PRO_VHD_SEL;

typedef enum {
	BF_STILL_PRO_STOP_FRAME = 0,			/**< Frame stop		*/
	BF_STILL_PRO_STOP_FORCE,				/**< Force stop		*/
	BF_STILL_PRO_STOP_MAX,
} BF_STILL_PRO_STOP;

typedef enum {
	BF_PRO_PGAIN_DECI_POS_8 = 0,			/**<  5-bit integer part,8-bit fractional part	*/
	BF_PRO_PGAIN_DECI_POS_7,				/**<  6-bit integer part,7-bit fractional part	*/
	BF_PRO_PGAIN_DECI_POS_6,				/**<  7-bit integer part,6-bit fractional part	*/
	BF_PRO_PGAIN_DECI_POS_5,				/**<  8-bit integer part,5-bit fractional part	*/
	BF_PRO_PGAIN_DECI_POS_4,				/**<  9-bit integer part,4-bit fractional part	*/
	BF_PRO_PGAIN_DECI_POS_3,				/**< 10-bit integer part,3-bit fractional part	*/
} BF_PRO_PGAIN_DECI_POS;

typedef enum {
	BF_STILL_PRO_BLOCK_COMBI_SEN = 0,
	BF_STILL_PRO_BLOCK_COMBI_SEN_SRO,
	BF_STILL_PRO_BLOCK_COMBI_SEN_SRO_B2B,
	BF_STILL_PRO_BLOCK_COMBI_SRO,
	BF_STILL_PRO_BLOCK_COMBI_SRO_B2B,
	BF_STILL_PRO_BLOCK_COMBI_B2B,
} BF_STILL_PRO_BLOCK_COMBI;

// delete\basefw\fj\imgproc\still
typedef struct {
	UCHAR				noise_data_mode;				/**< (OUT)ELA noise data mode. */
	UCHAR				noise_data_precision;			/**< (OUT)ELA noise data precision. */
	UCHAR				noise_data_compress_enable;		/**< (OUT)ELA noise data compression enable. */
	UINT32				noise_top_addr;					/**< (OUT)ELA synthetic noise data top address. */
	USHORT				noise_total_hsize;				/**< (OUT)ELA synthetic noise data total horizontal size. */
} BF_STILL_ELA_OUTPUT;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_DCF_EXIF BEGIN ---
typedef struct {
	T_BF_DCF_IF_EXIF_INFO exif_info;
} T_EXIF_INFO_SET;
// --- REMOVE_DCF_EXIF END ---


typedef struct {
	T_BF_STILL_IMG			src;
	T_BF_STILL_IMG			dst;
} T_REC_CONVERT_INFO;

typedef  struct {
	ULONG	Y;
	ULONG	Cb;
	ULONG	Cr;
}T_ADDR_YCC;

typedef struct {
	USHORT width;
	USHORT lines;
	USHORT top;
	USHORT left;
} T_DZOOM_PARAM;

typedef struct {
	DOUBLE	left;
	DOUBLE	top;
	USHORT	width;
	USHORT	lines;
} T_RESIZE_RECT;

typedef struct {
	T_BF_STILL_IMG		img;
	T_RESIZE_RECT		rect;
} T_RESIZE_IMG;

typedef struct {
	UCHAR	mask_color_y;					/**< Mask color of Y.						*/
	UCHAR	mask_color_cb;					/**< Mask color of Cb.						*/
	UCHAR	mask_color_cr;					/**< Mask color of Cr.						*/
} BF_MASK_COLOR;

typedef struct {
	USHORT	start_x;						/**< Mask start position of X.				*/
	USHORT	end_x;							/**< Mask end position of X.				*/
	USHORT	start_y;						/**< Mask start position of Y.				*/
	USHORT	end_y;							/**< Mask end position of Y.				*/
} BF_MASK_RECT;

typedef struct {
	BOOL				enable;				/**< FALSE:Turn off mask. TRUE:Enable mask.	*/
	BF_MASK_RECT		mask_rect;			/**< Rectangle of mask area					*/
} BF_MASK_INFO;


typedef struct {
	BF_MASK_COLOR		mask_color;			/**< Mask color value						*/
	UINT32				mask_num;			/**< Mask number.<br>
												 value range	:[0-20]<br>
												 */
	BF_MASK_INFO		mask_info[20];		/**< Mask information						*/
} BF_MASK_CTRL;

// delete\basefw\fj\imgproc\still
// RAW dump information.
typedef struct {
	FJ_PRO_CTRL_FIRST_PIX_COLOR	top_color;
	ULONG						addr;
	ULONG						raw_image_bytes;
	UCHAR						bits;
	T_OFFSET					offset;
	USHORT						linefeed;
	USHORT						width;
	USHORT						lines;
	ULLONG						frame_no;
} BF_PRO_RAW_INFO;

typedef struct {
	UCHAR					inst_id;
	BF_STILL_PRO_UNIT_NUM	unit_num;
	BF_STILL_PRO_BLOCK_TYPE	block_type;
	UCHAR					output_port;
	USHORT					bay_start_x;		// May zero.
	USHORT					bay_width_bytes;	// May zero.
	USHORT					bay_width;			// May zero.
	ULONG					bay_addr;			// May NULL.
} BF_PRO_RAW_OUTPUT_INFO;

typedef struct {
	BOOL				enabled;
	BF_STILL_PRO_STOP	stop_type;
} BF_PRO_SETUP_STATUS_TYPE;

typedef struct {
	UINT32	col_idx;
	UINT32	row_idx;
} BF_PRO_SETUP_STATUS_IQ_DIVISION_POS;

typedef struct {
	BOOL				enabled;
	BF_STILL_PRO_STOP	stop_type;
	USHORT				start_x;
	ULONG				addr;
	ULONG				width;
	ULONG				line_feed;
} BF_PRO_SETUP_RAW_STATUS_TYPE;

typedef struct {
	UCHAR				scaler_sel;
	BF_STILL_PRO_STOP	stop_type;
} BF_PRO_SETUP_STATUS_TYPE_RAW_SCALER;

typedef struct {
	BOOL							block_enabled;
	BF_PRO_SETUP_STATUS_TYPE		slvs[E_IM_PRO_SLVS_STREAM_TYPE_BOTH];
	BF_PRO_SETUP_STATUS_TYPE		lvds[D_IM_PRO_SEN_LVDS_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE		dmipi[D_IM_PRO_SEN_DMIPI_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE		cmipi[D_IM_PRO_SEN_CMIPI_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE		ldiv[D_IM_PRO_SEN_LDIV_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE		moni[D_IM_PRO_SEN_MONI_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE		obd[D_IM_PRO_SENTOP_BLOB_NUM];
	BF_PRO_SETUP_STATUS_TYPE		obt[D_IM_PRO_SENTOP_BLOB_NUM];
	BF_PRO_SETUP_STATUS_TYPE		lnr[D_IM_PRO_SENTOP_BLOB_NUM];
	BF_PRO_SETUP_RAW_STATUS_TYPE	raw_output[D_BF_STILL_PRO_CTRL_SEN_RAW_OUT_PORT];
} BF_PRO_SETUP_STATUS_SEN;

typedef struct {
	BOOL								block_enabled;
	ULONG								raw_input_linefeed;
	BF_PRO_SETUP_STATUS_TYPE			raw_input;
	BF_PRO_SETUP_STATUS_TYPE			ff[D_IM_PRO_SRO_FF_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE			pgain;
	BF_PRO_SETUP_STATUS_TYPE			ffsh[D_IM_PRO_SRO_FFSH_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE			sdc[D_IM_PRO_SRO_SDC_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE			pzsft[D_IM_PRO_SRO_PZSFT_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE			dpc;
	BF_PRO_SETUP_STATUS_TYPE			afpb[D_IM_PRO_SRO_AFPB_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE			fshdl[D_IM_PRO_SRO_FSHDL_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE			ffc;
	BF_PRO_SETUP_STATUS_TYPE			cag;
	BF_PRO_SETUP_STATUS_TYPE_RAW_SCALER	raw_scaller;
	BF_PRO_SETUP_STATUS_TYPE			moni;
	BF_PRO_SETUP_RAW_STATUS_TYPE		raw_output[D_BF_STILL_PRO_CTRL_SRO_RAW_OUT_PORT];
	BF_PRO_SETUP_STATUS_IQ_DIVISION_POS	iq_division_pos;
	UCHAR								row_division_idx;
	UCHAR								column_division_idx;
} BF_PRO_SETUP_STATUS_SRO;

typedef struct {
	BOOL								block_enabled;
	BF_PRO_SETUP_STATUS_TYPE			raw_input;
	BF_PRO_SETUP_STATUS_TYPE			pgain[D_IM_PRO_B2B_PGAIN_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE			pzsft[D_IM_PRO_B2B_PZSFT_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE			elf;
	BF_PRO_SETUP_STATUS_TYPE			fshd;
	BF_PRO_SETUP_RAW_STATUS_TYPE		raw_output[D_BF_STILL_PRO_CTRL_B2B_RAW_OUT_PORT];
	BF_PRO_SETUP_STATUS_IQ_DIVISION_POS	iq_division_pos;
	UCHAR								row_division_idx;
	UCHAR								column_division_idx;
} BF_PRO_SETUP_STATUS_B2B;

typedef struct {
	BOOL						block_enabled;
	BF_PRO_SETUP_STATUS_TYPE	af[D_IM_PRO_STAT_AF_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE	aeawb[D_IM_PRO_STAT_AEAWB_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE	hist[D_IM_PRO_STAT_HIST_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE	flck[D_IM_PRO_STAT_FLCK_CH_NUM];
	BF_PRO_SETUP_STATUS_TYPE	pwch[D_IM_PRO_STAT_PWCH_CH_NUM];
} BF_PRO_SETUP_STATUS_STAT;

typedef struct {
	BOOL						block_enabled;
	BF_PRO_SETUP_STATUS_TYPE	top;
	BF_PRO_SETUP_STATUS_TYPE	raw_input;
	BF_PRO_SETUP_STATUS_TYPE	lnr;
	BF_PRO_SETUP_STATUS_TYPE	pgain;
	BF_PRO_SETUP_STATUS_TYPE	spt;
	BF_PRO_SETUP_STATUS_TYPE	raw_output;
} BF_PRO_SETUP_STATUS_PAS;

typedef struct {
	BF_PRO_SETUP_STATUS_SEN		sen_status;
	BF_PRO_SETUP_STATUS_SRO		sro_status[E_IM_PRO_BOTH_UNIT];
	BF_PRO_SETUP_STATUS_B2B		b2b_status[E_IM_PRO_BOTH_UNIT];
	BF_PRO_SETUP_STATUS_STAT	stat_status;
	BF_PRO_SETUP_STATUS_PAS		pas_status;
} BF_PRO_SETUP_STATUS;

typedef struct {
	USHORT						R;							/**< R gain<br>
																value range		:[0 - 8191(=31.124511719)]<br>
																target registor	:@@PGAINR */
	USHORT						Gr;							/**< Gr gain<br>
																value range		:[0 - 8191(=31.124511719)]<br>
																target registor	:@@PGAINGR */
	USHORT						Gb;							/**< Gb gain<br>
																value range		:[0 - 8191(=31.124511719)]<br>
																target registor	:@@PGAINGB */
	USHORT						B;							/**< B gain<br>
																value range		:[0 - 8191(=31.124511719)]<br>
																target registor	:@@PGAINB */
	BF_PRO_PGAIN_DECI_POS		decimal_point_position;		/**< Decimal Point Position<br>
																target registor	:@@PGAINDP */
	SHORT						R_offset;					/**< R offset<br>
																value range		:[0x1000(=-4096) - 0x0FFF(=+4095)]<br>
																target registor	:@@POFSR */
	SHORT						Gr_offset;					/**< Gr offset<br>
																value range		:[0x1000(=-4096) - 0x0FFF(=+4095)]<br>
																target registor	:@@POFSGR */
	SHORT						Gb_offset;					/**< Gb offset<br>
																value range		:[0x1000(=-4096) - 0x0FFF(=+4095)]<br>
																target registor	:@@POFSGB */
	SHORT						B_offset;					/**< B offset<br>
																value range		:[0x1000(=-4096) - 0x0FFF(=+4095)]<br>
																target registor	:@@POFSB */
} BF_PRO_PGAIN_INFO;

typedef struct {
	UCHAR						sen_id_cnt;
	UCHAR						sen_id[D_IM_PRO_SEN_SENCORE_NUM];
	UCHAR						demosaic_id;
	UCHAR						aeawb_id;
	BF_PRO_SETUP_STATUS			setup_status;
//	T_FJ_STILL_IQ_PARAM*		iq_param;
	BF_STILL_ELA_OUTPUT*		ela_info;
	UCHAR						col_div_num;
	UCHAR						unit_div_num;
} BF_PRO_CONTROL_INFO;

typedef struct {
	ULONG	addr;
	ULONG	width;
	ULONG	lines;
	ULONG	linefeed;
	UCHAR	left_unit_no;
	UCHAR	right_unit_no;
} T_BF_STILL_PRO_CTRL_RAW_OUTPUT_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_DCF_EXIF BEGIN ---
extern volatile T_EXIF_INFO_SET gExif_Info_Set[MAX_OF_CONTINUOUS];
// --- REMOVE_DCF_EXIF END ---

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


extern ULONG BF_Still_Calc_Target_Size( const USHORT width, const USHORT lines, const E_BF_JPGEPC_MODE mode, USHORT cr );
extern FJ_ERR_CODE BF_Still_Resize_Image( const T_RESIZE_IMG* const src, const T_RESIZE_IMG* const dst, const E_RESIZE_MODE resize_mode, const E_IM_IIP_UTIL_ROTATE rotation );
extern VOID BF_Still_Resize_Image_CbCrOnly( const T_RESIZE_IMG* const src, const T_RESIZE_IMG* const dst,
						   const E_RESIZE_MODE resize_mode, const UINT32 wait_time, const UCHAR src_mif, const UCHAR dst_mif, const BF_MASK_CTRL* const mask_ctrl );
extern FJ_ERR_CODE BF_Still_Process_SPF( const FJ_IMG_SPF_CTRL* const spf_ctrl_info );
extern FJ_ERR_CODE BF_Still_Copy_Image( const ULONG src_addr, const ULONG dst_addr, const UINT32 width, const UINT32 lines );
extern FJ_ERR_CODE BF_Still_Copy_YccImage( const FJ_IMG_INFO* const src, const FJ_IMG_INFO* const dst );
extern INT32 BF_Still_Image_Draw_Face_Frame_VideoFmt( FJ_IMG_INFO* src_frame, const T_FACE_FRAME_EACH_SET* face_pos_array, const T_FACE_FRAME_SET* face_set );
extern INT32 BF_Still_Image_Draw_Face_Frame_YCC( FJ_IMG_INFO* src_frame, const T_FACE_FRAME_EACH_SET* face_pos_array, const T_FACE_FRAME_SET* face_set );
extern FJ_ERR_CODE BF_Still_Image_Draw_Rect_YCC(FJ_IMG_INFO* src_frame, const FJ_RECT_FRAME_SET* frame_set );
extern FJ_ERR_CODE BF_Still_mage_Draw_Rect_VideoFmt(FJ_IMG_INFO* src_frame, const FJ_RECT_FRAME_SET* frame_set );

#ifdef __cplusplus
}
#endif

#endif	// _STILL_H_

