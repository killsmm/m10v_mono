/**
 * @file		ctrl_ive_qr_marker.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#if CO_IVE_QR_MARKER_ON
#include <math.h>
#include "mode_stm.h"
#include "fj_memory.h"
#include "l1l2cache.h"
#include "ctrl_ive_qr_marker.h"
#include "IVE_MarkInterfaceC.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_CTRL_IVE_QR_MARK_GYRO_ON

#define D_CTRL_IVE_QR_Mark_width (86)	//43*2
#define D_CTRL_IVE_QR_Mark_lines (86)	//43*2
#define D_CTRL_IVE_QR_Mark_width_global (96)	//16 bit units
#define D_CTRL_IVE_QR_MARKER_FACE	(4)

#define D_CTRL_IVE_GYRO_SAMPLE_MAX	(65536)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	INT32	ImageIndex;
	INT32	LayoutNbColumns;
	INT32	LayoutNbLines;
	INT32	ColorFormat;
	INT32	CorrectionLevel;
	INT32	CameraOrientation;
	CHAR	LensRPL[6];
	CHAR	dummy[2];
} T_IVE_QR_MARKER_COMMON;	//30+2

typedef struct {
	INT32	calib_SrcWidth;
	INT32	calib_SrcHeight;
	FLOAT	calib_rotation;
	FLOAT	calib_center_x;
	FLOAT	calib_center_y;
	FLOAT	calib_width;
	FLOAT	calib_height;
} T_IVE_QR_MARKER_CALIB_IMAGE;	//28

typedef struct {
	FLOAT	calib_yaw;
	FLOAT	calib_pitch;
	FLOAT	calib_roll;
	FLOAT	calib_TransX;
	FLOAT	calib_TransY;
	FLOAT	calib_TransZ;
} T_IVE_QR_MARKER_CALIB_POSITION;	//24

typedef struct {
	FLOAT	quaternion_w;
	FLOAT	quaternion_x;
	FLOAT	quaternion_y;
	FLOAT	quaternion_z;
	FLOAT	accel_x;
	FLOAT	accel_y;
	FLOAT	accel_z;
	FLOAT	gyro_x;
	FLOAT	gyro_y;
	FLOAT	gyro_z;
	FLOAT	compass_x;
	FLOAT	compass_y;
	FLOAT	compass_z;
} T_IVE_QR_MARKER_DMP_RAW;	// 52

typedef struct {
	FLOAT	gyro_yaw;
	FLOAT	gyro_pitch;
	FLOAT	gyro_roll;
	ULONG	frame_no;		// set sensor divider here.
} T_IVE_QR_MARKER_GYRO;	//12+4


typedef struct {
	T_IVE_QR_MARKER_COMMON			qm_com;			// 32byte
	T_IVE_QR_MARKER_CALIB_IMAGE		qm_cal_img;		// 28byte
	T_IVE_QR_MARKER_CALIB_POSITION	qm_cal_pos;		// 24byte
	T_IVE_QR_MARKER_DMP_RAW			qm_dmp_raw;		// 36byte	ICM20948:52byte
	T_IVE_QR_MARKER_GYRO			qm_gyro;		// 16byte
} T_IVE_QR_MARKER_PARAM;  // 136	ICM20948:152byte

typedef struct {
	FLOAT fACCEL_X_OUT;
	FLOAT fACCEL_Y_OUT;
	FLOAT fACCEL_Z_OUT;
	FLOAT fGYRO_X_OUT;
	FLOAT fGYRO_Y_OUT;
	FLOAT fGYRO_Z_OUT;
	FLOAT fDEGREE_X;
	FLOAT fDEGREE_Y;
	FLOAT fDEGREE_Z;
	
	FLOAT fDEGREE_AX;
	FLOAT fDEGREE_AY;
	FLOAT fDEGREE_AZ;
	
	FLOAT fDEGREE_GX;
	FLOAT fDEGREE_GY;
	FLOAT fDEGREE_GZ;
	
	USHORT ACCEL_X_OUT;
	USHORT ACCEL_Y_OUT;
	USHORT ACCEL_Z_OUT;
	USHORT GYRO_X_OUT;
	USHORT GYRO_Y_OUT;
	USHORT GYRO_Z_OUT;
	USHORT MAG_X_OUT;
	USHORT MAG_Y_OUT;
	USHORT MAG_Z_OUT;
	UINT32 GYRO_SQR;		// from ois for af
	UINT32 GYRO_SQR_AVE;	// from ois for af
} T_IVE_GYRO_INFO;

typedef struct {
	FLOAT	ax;
	FLOAT	ay;
	FLOAT	az;
	
	FLOAT	gx;
	FLOAT	gy;
	FLOAT	gz;
	
	FLOAT	mx;
	FLOAT	my;
	FLOAT	mz;
	
	FLOAT	dx;
	FLOAT	dy;
	FLOAT	dz;
	
	// work
	FLOAT	adx;
	FLOAT	ady;
	FLOAT	adz;
	
	FLOAT	gdx;
	FLOAT	gdy;
	FLOAT	gdz;
	
} T_IVE_GYRO_SAMPLE;

#ifdef D_CTRL_IVE_QR_MARK_GYRO_ON
typedef struct {
	UCHAR*	p_gyro_send_data;
	UCHAR*	p_gyro_recv_data; 
	ULONG*	p_gyro_stamp_data;
	UINT32	gyro_send_num;
	UINT32	gyro_recv_num;
	UINT32	gyro_stamp_num;
	UINT32	last_gyro_num;
	UINT32	last_sample_idx;
	ULLONG	last_gyro_ts;
} T_IVE_GYRO_IF;
#endif
/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static UCHAR g_Ctrl_IVE_QR_raw_img[D_CTRL_IVE_QR_Mark_width_global*D_CTRL_IVE_QR_Mark_width_global] __attribute__((aligned(16))) = {};
static UCHAR g_Ctrl_IVE_Mark_img_Front[D_CTRL_IVE_QR_MARKER_FACE][D_CTRL_IVE_QR_Mark_width_global*D_CTRL_IVE_QR_Mark_width_global] __attribute__((aligned(16))) = {};
static UCHAR g_Ctrl_IVE_Mark_img_Right[D_CTRL_IVE_QR_Mark_width_global*D_CTRL_IVE_QR_Mark_width_global] __attribute__((aligned(16))) = {};
static UCHAR g_Ctrl_IVE_Mark_img_Left[D_CTRL_IVE_QR_Mark_width_global*D_CTRL_IVE_QR_Mark_width_global] __attribute__((aligned(16))) = {};
static UCHAR g_Ctrl_IVE_Mark_img_Rear[D_CTRL_IVE_QR_Mark_width_global*D_CTRL_IVE_QR_Mark_width_global] __attribute__((aligned(16))) = {};
static T_IVE_QR_MARKER_CONFIG* g_Ctrl_IVE_Mark_Para_Config;
static T_IVE_QR_MARKER_OUT g_Ctrl_IVE_Mark_img;
static T_IVE_QR_MARKER_PARAM g_Ctrl_IVE_qr_param[4];
static unsigned long g_Ctrl_IVE_qr_sizev;
static unsigned long g_Ctrl_IVE_qr_sizeh;
static INT32 g_Ctrl_IVE_qr_Read_Pointer = 0;
static INT32 g_Ctrl_IVE_qr_Write_Pointer = 0;

#ifdef D_CTRL_IVE_QR_MARK_GYRO_ON
static T_IVE_GYRO_IF g_Ctrl_IVE_GyroIF;
static T_CAMERA_GYRO_MPU_DATA	g_Ctrl_IVE_gyro_sample[D_CTRL_IVE_GYRO_SAMPLE_MAX];
static T_CAMERA_GYRO_MPU_DATA	g_Ctrl_IVE_gyro_bias;
#endif
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static VOID ctrl_ive_qr_drawmark(UCHAR* out_img, UCHAR* in_img);
#ifdef D_CTRL_IVE_QR_MARK_GYRO_ON
static void ctrl_ive_gyro_start(void);
static void ctrl_ive_gyro_read(void);
static void ctrl_ive_gyro_stop(void);
#endif

/*==============================================================================*/
/* Debug Function																*/
/*==============================================================================*/

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
CHAR* Ctrl_IVE_QR_Marker_ParaInit(T_IVE_QR_MARKER_CONFIG* para_config)
{
	FLOAT RotationCameraOther[3] = {180.0, 0.0, 0.0};
	FLOAT TranslationCameraFront[3] = {0.0, 0.0, 0.0};
	FLOAT TranslationCameraOther[3] = {0.0, 0.0, 0.0};
	CHAR *lens_rpl;
	INT32 i;
	IVE_MarkInterface *markFront, *markOther;
	
	g_Ctrl_IVE_Mark_Para_Config = para_config;
	g_Ctrl_IVE_qr_Read_Pointer = 0;
	g_Ctrl_IVE_qr_Write_Pointer = 0;
	// Parameters set
	memset( (VOID*)g_Ctrl_IVE_qr_param, 0, sizeof(T_IVE_QR_MARKER_PARAM)*4);
#ifdef D_CTRL_IVE_QR_MARK_GYRO_ON
	memset( (VOID*)g_Ctrl_IVE_gyro_sample, 0, sizeof(T_CAMERA_GYRO_MPU_DATA)*D_CTRL_IVE_GYRO_SAMPLE_MAX);
#endif
	
	for( i = 0; i < para_config->qr_used_sen_num; i++ ) {
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.ImageIndex		= i;
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.LayoutNbColumns	= (INT32)para_config->qr_layout_columns;
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.LayoutNbLines	= (INT32)para_config->qr_layout_lines;
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.ColorFormat		= (INT32)para_config->qr_color;
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.CorrectionLevel	= (INT32)para_config->qr_correction_level;
		strncpy((CHAR*)g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.LensRPL, (CHAR*)para_config->qr_rpl, 6);
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.CameraOrientation	= (INT32)para_config->qr_camera_orientation;

		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_SrcWidth	= (INT32)para_config->qr_source_width[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_SrcHeight	= (INT32)para_config->qr_source_height[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_rotation	= para_config->qr_rotation[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_center_x	= para_config->qr_center_x[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_center_y	= para_config->qr_center_y[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_width		= para_config->qr_width[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_height		= para_config->qr_height[i];

		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_yaw		= para_config->qr_calib_pan[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_pitch		= para_config->qr_calib_tilt[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_roll		= para_config->qr_calib_roll[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_TransX		= para_config->qr_calib_x[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_TransY		= para_config->qr_calib_y[i];
		g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_TransZ		= para_config->qr_calib_z[i];
	}


	g_Ctrl_IVE_Mark_img.g_width = D_CTRL_IVE_QR_Mark_width_global;
	g_Ctrl_IVE_Mark_img.width = D_CTRL_IVE_QR_Mark_width;
	g_Ctrl_IVE_Mark_img.lines = D_CTRL_IVE_QR_Mark_lines;
	// Front sensor initialization
	g_Ctrl_IVE_Mark_img.mark_img[0] = g_Ctrl_IVE_Mark_img_Front[0];
	markFront = CreateMarkInterface();

	lens_rpl = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_com.LensRPL;
	IVE_MarkInterface_SetColorFormat(markFront, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_com.ColorFormat);
	IVE_MarkInterface_SetRPL(markFront, lens_rpl);

	IVE_MarkInterface_SetCameraOrientation(markFront, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_com.CameraOrientation);
	IVE_MarkInterface_SetCorrectionLevel(markFront, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_com.CorrectionLevel);

	IVE_MarkInterface_AddTag(markFront, "assemblyLayoutNbColumns", &g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_com.LayoutNbColumns, sizeof(INT32));
	IVE_MarkInterface_AddTag(markFront, "assemblyLayoutNbLines", &g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_com.LayoutNbLines, sizeof(INT32));

	IVE_MarkInterface_SetLSP2( markFront, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_img.calib_center_x, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_img.calib_center_y, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_img.calib_width, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_img.calib_height, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_img.calib_rotation, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_img.calib_SrcWidth, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_img.calib_SrcHeight);

	TranslationCameraFront[0] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_pos.calib_TransX;
	TranslationCameraFront[1] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_pos.calib_TransY;
	TranslationCameraFront[2] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_cal_pos.calib_TransZ;

	IVE_MarkInterface_AddTag(markFront, "CalibrationTranslation", TranslationCameraFront, sizeof(FLOAT)*3);

	IVE_MarkInterface_AddTag(markFront, "assemblyImageIndex", &g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[0]].qm_com.ImageIndex,sizeof(INT32));

	// Other sensor initialization 
	g_Ctrl_IVE_Mark_img.mark_img[1] = g_Ctrl_IVE_Mark_img_Rear;
	g_Ctrl_IVE_Mark_img.mark_img[2] = g_Ctrl_IVE_Mark_img_Left;
	g_Ctrl_IVE_Mark_img.mark_img[3] = g_Ctrl_IVE_Mark_img_Right;

	for( i = 1; i < para_config->qr_used_sen_num; i++) {
		markOther = CreateMarkInterface();
		if( markOther == NULL ) {
			UPRINTF("QR marker initialization [%d] error!!\n", i);
			continue;
		}

		IVE_MarkInterface_SetColorFormat(markOther, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.ColorFormat);
		IVE_MarkInterface_SetRPL(markOther, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.LensRPL);
	
		IVE_MarkInterface_SetCameraOrientation(markOther, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.CameraOrientation);
		IVE_MarkInterface_SetCorrectionLevel(markOther, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.CorrectionLevel);
	
		IVE_MarkInterface_AddTag(markOther, "assemblyLayoutNbColumns", &g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.LayoutNbColumns, sizeof(INT32));
		IVE_MarkInterface_AddTag(markOther, "assemblyLayoutNbLines", &g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.LayoutNbLines, sizeof(INT32));

		IVE_MarkInterface_SetLSP2( markOther, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_center_x, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_center_y, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_width, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_height, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_rotation, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_SrcWidth, g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_img.calib_SrcHeight);

		RotationCameraOther[0] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_yaw;
		RotationCameraOther[1] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_pitch;
		RotationCameraOther[2] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_roll;
		TranslationCameraOther[0] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_TransX;
		TranslationCameraOther[1] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_TransY;
		TranslationCameraOther[2] = g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_cal_pos.calib_TransZ;

		IVE_MarkInterface_AddTag(markOther, "CalibrationRotation", RotationCameraOther, sizeof(FLOAT)*3);
		IVE_MarkInterface_AddTag(markOther, "CalibrationTranslation", TranslationCameraOther, sizeof(FLOAT)*3);

		IVE_MarkInterface_AddTag(markOther, "assemblyImageIndex", &g_Ctrl_IVE_qr_param[para_config->qr_sen_idx[i]].qm_com.ImageIndex,sizeof(INT32));
	
		IVE_MarkInterface_GetMarkSize(markOther, &g_Ctrl_IVE_qr_sizeh, &g_Ctrl_IVE_qr_sizev);
		IVE_MarkInterface_GetMark(markOther, g_Ctrl_IVE_QR_raw_img);
	
		ctrl_ive_qr_drawmark(g_Ctrl_IVE_Mark_img.mark_img[i], g_Ctrl_IVE_QR_raw_img);
	
		DeleteMarkInterface(markOther);
	}

#ifdef D_CTRL_IVE_QR_MARK_GYRO_ON
	// Start Gyro Here??
	ctrl_ive_gyro_start();
#endif

	return (CHAR *)markFront;
}

void Ctrl_IVE_QR_Marker_Run(CHAR* markFront)
{
	INT32 tmp_write_pointer = g_Ctrl_IVE_qr_Write_Pointer;

#ifdef D_CTRL_IVE_QR_MARK_GYRO_ON
	// Get Gyro Here??
	ctrl_ive_gyro_read();
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.quaternion_w	= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].quat[0];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.quaternion_x	= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].quat[1];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.quaternion_y	= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].quat[2];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.quaternion_z	= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].quat[3];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.accel_x		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].accel[0];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.accel_y		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].accel[1];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.accel_z		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].accel[2];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.gyro_x		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].gyro[0];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.gyro_y		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].gyro[1];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.gyro_z		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].gyro[2];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.compass_x 	= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].compass[0];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.compass_y 	= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].compass[1];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_dmp_raw.compass_z 	= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].compass[2];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_gyro.gyro_yaw 		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].orientation[0];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_gyro.gyro_pitch		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].orientation[1];
	g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_gyro.gyro_roll		= g_Ctrl_IVE_gyro_sample[g_Ctrl_IVE_GyroIF.last_sample_idx].orientation[2];
#endif

	IVE_MarkInterface_SetCameraRotation((IVE_MarkInterface*)markFront, g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_gyro.gyro_yaw, g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_gyro.gyro_pitch, g_Ctrl_IVE_qr_param[g_Ctrl_IVE_Mark_Para_Config->qr_sen_idx[0]].qm_gyro.gyro_roll);
	IVE_MarkInterface_GetMarkSize((IVE_MarkInterface*)markFront, &g_Ctrl_IVE_qr_sizeh, &g_Ctrl_IVE_qr_sizev);
	IVE_MarkInterface_GetMark((IVE_MarkInterface*)markFront, g_Ctrl_IVE_QR_raw_img);
	
	ctrl_ive_qr_drawmark(g_Ctrl_IVE_Mark_img_Front[g_Ctrl_IVE_qr_Write_Pointer], g_Ctrl_IVE_QR_raw_img);
	
	g_Ctrl_IVE_qr_Write_Pointer = (g_Ctrl_IVE_qr_Write_Pointer + 1) % D_CTRL_IVE_QR_MARKER_FACE;
	if( g_Ctrl_IVE_qr_Write_Pointer == g_Ctrl_IVE_qr_Read_Pointer ) {
		g_Ctrl_IVE_qr_Write_Pointer = tmp_write_pointer;
	}
	
}

T_IVE_QR_MARKER_OUT* Ctrl_IVE_Get_QRMarkerCB( void )
{
	static INT32 tmp_read_pointer = 0;

	if( g_Ctrl_IVE_qr_Read_Pointer == g_Ctrl_IVE_qr_Write_Pointer ) {
		g_Ctrl_IVE_qr_Read_Pointer = tmp_read_pointer;
	}
	tmp_read_pointer = g_Ctrl_IVE_qr_Read_Pointer;
	g_Ctrl_IVE_Mark_img.mark_img[0] = g_Ctrl_IVE_Mark_img_Front[g_Ctrl_IVE_qr_Read_Pointer];

	g_Ctrl_IVE_qr_Read_Pointer = (g_Ctrl_IVE_qr_Read_Pointer+1) % D_CTRL_IVE_QR_MARKER_FACE;

	return (T_IVE_QR_MARKER_OUT*)&g_Ctrl_IVE_Mark_img;
}

void Ctrl_IVE_QR_Marker_Stop(CHAR* markFront)
{
	DeleteMarkInterface((IVE_MarkInterface*)markFront);

#ifdef D_CTRL_IVE_QR_MARK_GYRO_ON
	// Stop Gyro Here??
	ctrl_ive_gyro_stop();
#endif
	
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void ctrl_ive_qr_drawmark(UCHAR* out_img, UCHAR* in_img)
{
    UINT32 mPos = 0;
    unsigned int BlockSize = D_CTRL_IVE_QR_Mark_width/g_Ctrl_IVE_qr_sizeh;
    INT32 i,j;
    INT32 bsX, bsY, mPos2;

	//mPos  ... in_img line
	//mPos2 ... in_img row

    for( j = 0; j<g_Ctrl_IVE_qr_sizev; j++) {
        for( bsY = 0; bsY<BlockSize; bsY++) {
            mPos2 = mPos;
            for( i =0; i<g_Ctrl_IVE_qr_sizeh; i++) {
                for( bsX = 0; bsX<BlockSize; bsX++) {
                    *out_img = in_img[mPos2];
                    out_img++;
                }
                mPos2++;
            }
			out_img += D_CTRL_IVE_QR_Mark_width_global - D_CTRL_IVE_QR_Mark_width;
        }
        mPos+=g_Ctrl_IVE_qr_sizeh;
    }

}

#ifdef D_CTRL_IVE_QR_MARK_GYRO_ON
static void ctrl_ive_gyro_start(void)
{
	g_Ctrl_IVE_GyroIF.last_gyro_num = 0xFFFFFFFF;
	g_Ctrl_IVE_GyroIF.last_sample_idx = 0;
	g_Ctrl_IVE_GyroIF.last_gyro_ts = 0;
	
	FJ_Top_Set_Clock_Select_GYRO( FJ_TOP_GYRO_CLKSEL_2MHZ );
	CAMERA.gyro.Func_Init();
	CAMERA.gyro.Func_Calc_Bias( 1000, &g_Ctrl_IVE_gyro_bias );
	g_Ctrl_IVE_GyroIF.p_gyro_send_data = (UCHAR *)CAMERA.gyro.ctrl->data_ctrl.send_addr;
	g_Ctrl_IVE_GyroIF.p_gyro_recv_data = (UCHAR *)CAMERA.gyro.ctrl->data_ctrl.recv_addr; 
	g_Ctrl_IVE_GyroIF.p_gyro_stamp_data = (ULONG *)CAMERA.gyro.ctrl->stamp_ctrl.buff_addr;
	g_Ctrl_IVE_GyroIF.gyro_send_num = CAMERA.gyro.buf_size[E_CAMERA_GYRO_BUF_SIZE_SEND];
	g_Ctrl_IVE_GyroIF.gyro_recv_num = CAMERA.gyro.buf_size[E_CAMERA_GYRO_BUF_SIZE_RECV];
	g_Ctrl_IVE_GyroIF.gyro_stamp_num = CAMERA.gyro.buf_size[E_CAMERA_GYRO_BUF_SIZE_STAMP];
	memset(g_Ctrl_IVE_GyroIF.p_gyro_send_data,  0, g_Ctrl_IVE_GyroIF.gyro_send_num);
	memset(g_Ctrl_IVE_GyroIF.p_gyro_recv_data,  0, g_Ctrl_IVE_GyroIF.gyro_recv_num);
	memset(g_Ctrl_IVE_GyroIF.p_gyro_stamp_data, 0, g_Ctrl_IVE_GyroIF.gyro_stamp_num);
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN,       (UINT32)g_Ctrl_IVE_GyroIF.p_gyro_send_data,  g_Ctrl_IVE_GyroIF.gyro_send_num );
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)g_Ctrl_IVE_GyroIF.p_gyro_recv_data,  g_Ctrl_IVE_GyroIF.gyro_recv_num);
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)g_Ctrl_IVE_GyroIF.p_gyro_stamp_data, g_Ctrl_IVE_GyroIF.gyro_stamp_num);
	CAMERA.gyro.Func_StartRead();
}

static void ctrl_ive_gyro_read(void)
{
	INT32					i;
	UINT32					cur_num;
	UINT32					loop_num;
	UINT32					index = 0;
	ULLONG					ts_us;
	
	// Gyro Data
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)g_Ctrl_IVE_GyroIF.p_gyro_recv_data, g_Ctrl_IVE_GyroIF.gyro_recv_num );
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)g_Ctrl_IVE_GyroIF.p_gyro_stamp_data, g_Ctrl_IVE_GyroIF.gyro_stamp_num );
	
	// Current Data Num
	CAMERA.gyro.Func_GetReadCount( (UINT32*)&cur_num );
	
	// Set Data from Last to Current
	if( g_Ctrl_IVE_GyroIF.last_gyro_num == 0xFFFFFFFF ){	// first time
		loop_num = cur_num;
	}else{
		if( cur_num >= g_Ctrl_IVE_GyroIF.last_gyro_num ){
			loop_num = cur_num - g_Ctrl_IVE_GyroIF.last_gyro_num;
		}else{
			loop_num = D_CTRL_IVE_GYRO_SAMPLE_MAX - g_Ctrl_IVE_GyroIF.last_gyro_num + cur_num;
		}
	}
	for( i = 0; i < loop_num && i < D_CTRL_IVE_GYRO_SAMPLE_MAX; i ++ ){
		index  = (i + g_Ctrl_IVE_GyroIF.last_gyro_num ) % D_CTRL_IVE_GYRO_SAMPLE_MAX;
		ts_us  = (ULLONG)g_Ctrl_IVE_GyroIF.p_gyro_stamp_data[ index * 2 ] | ( (ULLONG)g_Ctrl_IVE_GyroIF.p_gyro_stamp_data[ index * 2 + 1] << 32 ); 
	
		if( g_Ctrl_IVE_GyroIF.last_gyro_ts < ts_us ){
			CAMERA.gyro.Func_GetData(index, &g_Ctrl_IVE_gyro_sample[index]);
			
			g_Ctrl_IVE_gyro_sample[index].gyro[0] -= g_Ctrl_IVE_gyro_bias.gyro[0];
			g_Ctrl_IVE_gyro_sample[index].gyro[1] -= g_Ctrl_IVE_gyro_bias.gyro[1];
			g_Ctrl_IVE_gyro_sample[index].gyro[2] -= g_Ctrl_IVE_gyro_bias.gyro[2];
			g_Ctrl_IVE_gyro_sample[index].accel[0] -= g_Ctrl_IVE_gyro_bias.accel[0];
			g_Ctrl_IVE_gyro_sample[index].accel[1] -= (g_Ctrl_IVE_gyro_bias.accel[1]+1.0);// gravity
			g_Ctrl_IVE_gyro_sample[index].accel[2] -= g_Ctrl_IVE_gyro_bias.accel[2];
			g_Ctrl_IVE_gyro_sample[index].orientation[0] = g_Ctrl_IVE_gyro_sample[index].gyro[0]; // temporary
			g_Ctrl_IVE_gyro_sample[index].orientation[1] = g_Ctrl_IVE_gyro_sample[index].gyro[1]; // temporary
			g_Ctrl_IVE_gyro_sample[index].orientation[2] = g_Ctrl_IVE_gyro_sample[index].gyro[2]; // temporary
		}
	}
	g_Ctrl_IVE_GyroIF.last_gyro_num = cur_num;
	g_Ctrl_IVE_GyroIF.last_sample_idx = index;
	g_Ctrl_IVE_GyroIF.last_gyro_ts = (ULLONG)g_Ctrl_IVE_GyroIF.p_gyro_stamp_data[ (cur_num-1) * 2 ] | ( (ULLONG)g_Ctrl_IVE_GyroIF.p_gyro_stamp_data[ (cur_num-1) * 2 + 1] << 32 ); 
}

static void ctrl_ive_gyro_stop(void)
{
	CAMERA.gyro.Func_StopRead();
	CAMERA.gyro.Func_Standby();
}
#endif

#endif


