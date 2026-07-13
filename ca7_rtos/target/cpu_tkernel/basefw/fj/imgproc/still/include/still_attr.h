/**
 * @file		still_attr.h
 * @brief		capture attribute
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_ATTR_H_
#define _STILL_ATTR_H_

#include "fj_still.h"
#include "still_img_info.h"
// delete\basefw\fj\imgproc\still
//#include "still_pro_ctrl.h"
//#include "still_b2r_ctrl.h"
//#include "still_r2y_ctrl.h"
#include "sdram_map_common.h"
#include "instance.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_BF_STILL_ATTR_SHUTTER_MODE_GLOBAL		(0)
#define D_BF_STILL_ATTR_SHUTTER_MODE_ROLLING	(1)

#define D_BF_STILL_ATTR_OFF						(0)
#define D_BF_STILL_ATTR_ON						(1)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_ENLARGED_ZOOM = 0,
	E_CLEAN_ZOOM
} E_ZOOM_MODE;

typedef enum {
	E_YUV_MANI_SIZE_NONE = 0,
	E_YUV_MANI_SIZE_QVGA_420,
	E_YUV_MANI_SIZE_QVGA_422,
	E_YUV_MANI_SIZE_VGA_420,
	E_YUV_MANI_SIZE_VGA_422,
	E_YUV_MANI_SIZE_QQVGA_420,
	E_YUV_MANI_SIZE_QQVGA_422
} E_YUV_MANI_SIZE;

typedef enum {
	E_CAP_BAYER_TYPE_12BIT = 0,
	E_CAP_BAYER_TYPE_16BIT,
	E_CAP_BAYER_TYPE_8BIT,
	
	E_CAP_BAYER_TYPE_MAX,
} E_CAP_BAYER_TYPE;

// Shoot Format Type
typedef enum {
	E_SHOOT_FORMAT_JPEG = 0,
	E_SHOOT_FORMAT_RAW,
	E_SHOOT_FORMAT_JPEG_RAW,
} E_SHOOT_FORMAT;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	USHORT pos_x;
	USHORT pos_y;
	USHORT width;
	USHORT lines;
	BOOL ar_flg;
} T_DISP_WINDOW;

typedef struct {
	USHORT					width;
	USHORT					lines;
} T_SET_IMAGE_SIZE;

typedef struct {
	UCHAR	length;
	UCHAR	delay;
} T_SET_SEQ_LENGTH;

typedef struct {
	USHORT	dram_status;
	USHORT	num_of_pend;
} T_CAPTURE_STATUS;

typedef struct {
	UCHAR	dark_nr_trg;
	UCHAR	mb_nr_trg;
	UCHAR	dpc_mode;
	UCHAR	rec_nr_trg;
	UCHAR	sspr_nr_trg;
	UCHAR	spr_nr_trg;
} T_CAPTURE_NR_INFO;

typedef struct {
	// delete basefw\fj\imgproc\still
	//UCHAR		knee_table[BF_PRO_TABLE_MAX_KNEE];									/**< bayer knee inclination		*/
	//USHORT		bayer_deknee_table[D_BF_B2R_TABLE_MAX_DEKNEE];						/**< bayer deknee inclination	*/
	//UCHAR		rgb_knee_table[E_BF_B2R_RGBTBL_MAX][D_BF_B2R_TABLE_MAX_KNEE];		/**< rgb knee inclination		*/
	UCHAR		knee_table[257];									/**< bayer knee inclination		*/
	USHORT		bayer_deknee_table[256];						/**< bayer deknee inclination	*/
	UCHAR		rgb_knee_table[3][257];		/**< rgb knee inclination		*/
	// delete basefw\fj\imgproc\still
	//USHORT		rgb_deknee_table[E_BF_R2Y_RGBTBL_MAX][D_BF_R2Y_TABLE_MAX_DEKNEE];	/**< rgb deknee inclination		*/
	USHORT		rgb_deknee_table[3][256];	/**< rgb deknee inclination		*/
} T_CAPTURE_KNEE_DEKNEE_TBL;

typedef struct {
	E_YUV_MANI_SIZE			yuv_1;
	E_YUV_MANI_SIZE			yuv_2;
} T_YUV_MANI_SIZE;

typedef struct {
	USHORT					yuv_1;
	USHORT					yuv_2;
} T_YUV_MANI_AREA;

typedef struct {
	E_YUV_MANI_SIZE			yuv_1;
	E_YUV_MANI_SIZE			yuv_2;
	E_YUV_MANI_SIZE			yuv_3;
	E_YUV_MANI_SIZE			yuv_4;
} T_CAP_YUV_MANI_SIZE;

typedef struct {
	USHORT					yuv_1;
	USHORT					yuv_2;
	USHORT					yuv_3;
	USHORT					yuv_4;
} T_CAP_YUV_MANI_AREA;

typedef struct {
	UCHAR					obd_en;
	UCHAR					obt_en;
} T_OB_TYPE_SWITCH;

typedef struct {
	FJ_STILL_ADAPTIVE_AE_MODE	adaptive_ae_mode;
	UCHAR					auto_tone_no_fg;
	UCHAR					auto_tone_no_ae;
	UCHAR					fix_tone_no_fg;
	UCHAR					fix_tone_no_ae;
} T_ADAPTIVE_AE_PARAM;

typedef struct {
	USHORT					resize_mode;
	FJ_VIEW_SDHD_MODE		sdhd_mode;
	T_DISP_WINDOW			disp_win_menu;
	T_DISP_WINDOW			disp_win_view;
	UINT32					effect;
	T_SET_IMAGE_SIZE		size_view;
	USHORT					hold_time;
	USHORT					rec_type;
	T_SET_SEQ_LENGTH		seq_len;
	UCHAR					after_seq_len;
	UCHAR					bay_seq_len;
	UCHAR					view_disp_res;
	UCHAR					high_sensitivity_trg;
	T_CAPTURE_STATUS		cap_status_std;
	T_CAPTURE_STATUS		cap_status_night;
	T_CAPTURE_NR_INFO		nr_info;
	E_ZOOM_MODE				zoom_mode;
	UCHAR					smear_enable;
	UCHAR					contrast_level;
	UCHAR					geomdist_enable;
	UCHAR					bayer_type;			// 0:12bit, 1:16bit, 2:8bit
	UCHAR					raw_save_enable;
	UCHAR					raw_compensation_enable;
	UCHAR					pano_img_enable;
	UCHAR					shutter_mode;		// 0:D_BF_STILL_ATTR_SHUTTER_MODE_GLOBAL, 1:D_BF_STILL_ATTR_SHUTTER_MODE_ROLLING
	T_YUV_MANI_SIZE			view_yuv_mani_size;
	T_YUV_MANI_AREA			view_yuv_mani_area;
	T_CAP_YUV_MANI_SIZE			cap_yuv_mani_size;
	T_CAP_YUV_MANI_AREA			cap_yuv_mani_area;
	T_CAPTURE_KNEE_DEKNEE_TBL	knee_deknee_tbl;
	UCHAR						pro_knee_tbl_update_flg;
	UCHAR						pro_deknee_tbl_update_flg;
	UCHAR						ela_deknee_tbl_update_flg;
	T_OB_TYPE_SWITCH			ob_mode;
// Movie still capture start
	ULONG						movie_cap_file_count;
// Movie still capture end
	UCHAR						r2y_hist_enable;
	UCHAR						r2y_mirror_enable[D_BF_INSTANCE_ID_MAX_DEMOSAIC];
	UCHAR						r2y_flip_enable[D_BF_INSTANCE_ID_MAX_DEMOSAIC];
	UCHAR						b2r_flip_enable[D_BF_INSTANCE_ID_MAX_DEMOSAIC];
	UCHAR						r2y_btc_hist_enable;
	UCHAR						r2y_btc_thumb_enable;
	T_ADAPTIVE_AE_PARAM			adaptive_ae_param;
	FJ_STILL_IMAGE_MACRO_MODE	image_macro_mode;
	USHORT						still_count;
	FJ_LTM_MAP_CTRL				ltm_mode;
	UCHAR						raw_insertion_onoff;
	UCHAR						mfnr_onoff;
	UCHAR						tone_manual;
	UCHAR						shdr_normal_image_onoff;
} T_BF_CAPTURE_ATTR;

typedef struct {
	T_SET_IMAGE_SIZE		size_cap;
	T_SET_IMAGE_SIZE		size_screen_vga;
	T_SET_IMAGE_SIZE		size_screen_hd;
	FJ_COMPRESSION_RATIO	cr_main;
	FJ_COMPRESSION_RATIO	cr_screen_vga;
	FJ_COMPRESSION_RATIO	cr_screen_hd;
	USHORT					screen_vga_enable;
	USHORT					screen_hd_enable;
	USHORT					jpg_threshold;
	E_SHOOT_FORMAT			shoot_format;
} T_CAPTURE_ATTR_FIRST;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
// resize mode
extern VOID BF_Still_Attr_Set_Cap_Resize_Mode( USHORT resize_mode );

// shutter mode
extern VOID BF_Still_Attr_Set_Shutter_Mode( UCHAR mode );

// sensor_win_view
extern VOID BF_Still_Attr_Set_Sensor_Win_View_Pos_X(USHORT pos_x);
extern USHORT BF_Still_Attr_Get_Sensor_Win_View_Pos_X(VOID);
extern VOID BF_Still_Attr_Set_Sensor_Win_View_Pos_Y(USHORT pos_y);
extern USHORT BF_Still_Attr_Get_Sensor_Win_View_Pos_Y(VOID);
extern VOID BF_Still_Attr_Set_Sensor_Win_View_Width(USHORT width);
extern USHORT BF_Still_Attr_Get_Sensor_Win_View_Width(VOID);
extern VOID BF_Still_Attr_Set_Sensor_Win_View_Lines(USHORT lines);
extern USHORT BF_Still_Attr_Get_Sensor_Win_View_Lines(VOID);

// sensor_win_cap
extern VOID BF_Still_Attr_Set_Sensor_Win_Cap_Pos_X(USHORT pos_x);
extern USHORT BF_Still_Attr_Get_Sensor_Win_Cap_Pos_X(VOID);
extern VOID BF_Still_Attr_Set_Sensor_Win_Cap_Pos_Y(USHORT pos_y);
extern USHORT BF_Still_Attr_Get_Sensor_Win_Cap_Pos_Y(VOID);
extern VOID BF_Still_Attr_Set_Sensor_Win_Cap_Width(USHORT width);
extern USHORT BF_Still_Attr_Get_Sensor_Win_Cap_Width(VOID);
extern VOID BF_Still_Attr_Set_Sensor_Win_Cap_Lines(USHORT lines);
extern USHORT BF_Still_Attr_Get_Sensor_Win_Cap_Lines(VOID);

// size_cap
extern USHORT BF_Still_Attr_Get_Size_Cap_Width(VOID);
extern USHORT BF_Still_Attr_Get_Size_Cap_Lines(VOID);

// size_screen_vga
extern VOID BF_Still_Attr_Set_Size_Screen_Vga_Width(USHORT width);
extern VOID BF_Still_Attr_Set_Size_Screen_Vga_Lines(USHORT lines);

// size_screen_hd
extern VOID BF_Still_Attr_Set_Size_Screen_Hd_Width(USHORT width);
extern VOID BF_Still_Attr_Set_Size_Screen_Hd_Lines(USHORT lines);

// cr_main
extern FJ_COMPRESSION_RATIO BF_Still_Attr_Get_Comp_Main(VOID);

// cr_screen_vga
extern VOID BF_Still_Attr_Set_Comp_Screen_Vga(FJ_COMPRESSION_RATIO ratio);
extern FJ_COMPRESSION_RATIO BF_Still_Attr_Get_Comp_Screen_Vga(VOID);

// rec_type
extern VOID BF_Still_Attr_Set_Rec_Type(USHORT rec_type);

// jpg_threshold
extern VOID BF_Still_Attr_Set_Jpeg_Threshold(USHORT threshold);

// zoom_mode
extern VOID BF_Still_Attr_Set_Zoom_Mode(E_ZOOM_MODE zoom_mode);

// bayer_type
extern VOID BF_Still_Attr_Set_Bayer_Type(UCHAR bayer_type);

// raw save mode
extern VOID BF_Still_Attr_Set_Raw_Save_Mode(UCHAR raw_save_enable);

// raw compensation mode
extern VOID BF_Still_Attr_Set_Raw_Compensation_Mode(UCHAR raw_compensation_enable);

extern VOID BF_Still_Attr_Set_R2Y_Histogram_Enable( UCHAR enable );
extern VOID BF_Still_Attr_Set_R2Y_Mirror_Enable( UCHAR demosaic_id, UCHAR enable );
extern UCHAR BF_Still_Attr_Get_R2Y_Mirror_Enable( UCHAR demosaic_id );
extern VOID BF_Still_Attr_Set_R2Y_Flip_Enable( UCHAR demosaic_id, UCHAR enable );
extern UCHAR BF_Still_Attr_Get_R2Y_Flip_Enable( UCHAR demosaic_id );
extern VOID BF_Still_Attr_Set_R2Y_Btc_Thumbnail_Enable( UCHAR enable );
extern VOID BF_Still_Attr_Set_R2Y_Btc_Histogram_Enable( UCHAR enable );
extern VOID BF_Still_Attr_Set_Raw_Insertion_OnOff( UCHAR onoff );
extern VOID BF_Still_Attr_Set_Mfnr_OnOff( UCHAR onoff );
extern VOID BF_Still_Attr_Set_Tone_Manual( UCHAR param );
extern VOID BF_Still_Attr_Set_Shdr_Normal_Image_OnOff( UCHAR onoff );

// YUV Manipulation
extern VOID BF_Still_Attr_Set_View_YUV_Manipulation_1st_Size(USHORT select_size);
extern VOID BF_Still_Attr_Set_View_YUV_Manipulation_2nd_Size(USHORT select_size);
extern VOID BF_Still_Attr_Set_View_YUV_Manipulation_1st_Area(USHORT select_area);
extern VOID BF_Still_Attr_Set_Capture_YUV_Manipulation_1st_Size(USHORT select_size);
extern VOID BF_Still_Attr_Set_Capture_YUV_Manipulation_2nd_Size(USHORT select_size);
extern VOID BF_Still_Attr_Set_Capture_YUV_Manipulation_3rd_Size(USHORT select_size);
extern VOID BF_Still_Attr_Set_Capture_YUV_Manipulation_4th_Size(USHORT select_size);
extern VOID BF_Still_Attr_Set_Capture_YUV_Manipulation_1st_Area(USHORT select_area);
extern VOID BF_Still_Attr_Set_Capture_YUV_Manipulation_2nd_Area(USHORT select_area);
extern VOID BF_Still_Attr_Set_Capture_YUV_Manipulation_3rd_Area(USHORT select_area);
extern VOID BF_Still_Attr_Set_Capture_YUV_Manipulation_4th_Area(USHORT select_area);

extern VOID BF_Still_Attr_Set_OBD_Mode( UCHAR mode );
extern VOID BF_Still_Attr_Set_OBT_Mode( UCHAR mode );

extern E_SHOOT_FORMAT BF_Still_Attr_Get_Shoot_Image_Format(VOID);

extern VOID BF_Still_Attr_Set_ImageMacroMode( FJ_STILL_IMAGE_MACRO_MODE mode );

extern VOID BF_Still_Attr_Set_Still_Count( USHORT count );
extern USHORT BF_Still_Attr_Get_Still_Count( VOID );


extern VOID BF_Still_Attr_Set_Ltm_Core_Mode( FJ_LTM_MAP_CTRL mode );

#endif	// _STILL_ATTR_H_
