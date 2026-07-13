/**
 * @file		ctrl_ive_qr_marker.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _CTRL_IVE_QR_MARKER_H_
#define _CTRL_IVE_QR_MARKER_H_

#if CO_IVE_QR_MARKER_ON

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
	CHAR		qr_start[4];			// qr marker parameter start string
	UINT32		qr_senid_of_imindex0;	// sensor id of image index 0		// now these values are ignored
	UINT32		qr_senid_of_imindex1;	// sensor id of image index 1		// now these values are ignored
	UINT32		qr_senid_of_imindex2;	// sensor id of image index 2		// now these values are ignored
	UINT32		qr_senid_of_imindex3;	// sensor id of image index 3		// now these values are ignored
	UINT32		qr_set_primary;			// set primary sensor. 0 ... right front 1 ... left front 2 ... left back 3 ... right back
	UINT32		qr_layout_columns;
	UINT32		qr_layout_lines;
	UINT32		qr_color;
	UINT32		qr_correction_level;
	UINT32		qr_camera_orientation;
	UCHAR		qr_rpl[8];
	UINT32		qr_source_width[4];
	UINT32		qr_source_height[4];
	FLOAT		qr_rotation[4];
	FLOAT		qr_center_x[4];
	FLOAT		qr_center_y[4];
	FLOAT		qr_width[4];
	FLOAT		qr_height[4];
	FLOAT		qr_calib_pan[4];
	FLOAT		qr_calib_tilt[4];
	FLOAT		qr_calib_roll[4];
	FLOAT		qr_calib_x[4];
	FLOAT		qr_calib_y[4];
	FLOAT		qr_calib_z[4];
	CHAR		qr_end[4];				// qr marker parameter end string
	UINT32 		qr_used_sen_num;
	UINT32 		qr_sen_idx[4];
} T_IVE_QR_MARKER_CONFIG;

typedef struct {
	UCHAR*		mark_img[4];
	UINT32		g_width;
	UINT32		width;
	UINT32		lines;
} T_IVE_QR_MARKER_OUT;

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

extern CHAR* Ctrl_IVE_QR_Marker_ParaInit(T_IVE_QR_MARKER_CONFIG* para_config);
extern void Ctrl_IVE_QR_Marker_Run(CHAR* markFront);
extern T_IVE_QR_MARKER_OUT* Ctrl_IVE_Get_QRMarkerCB( void );
extern void Ctrl_IVE_QR_Marker_Stop(CHAR* markFront);

#ifdef __cplusplus
}
#endif

#endif
#endif	// _CTRL_IVE_QR_MARKER_H_

