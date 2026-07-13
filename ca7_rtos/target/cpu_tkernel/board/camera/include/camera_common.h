/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_COMMON_H
#define _CAMERA_COMMON_H

#include "ddim_typedef.h"
#include "fj_still.h"
#include "dd_top.h"
#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IMX_SENSOR_SLAVE7BIT		0x1A
#define D_IMX_SENSOR_SLAVE7BIT2		0x10

#define D_OV_SENSOR_SLAVE7BIT		0x36
#define D_OV_SENSOR_SLAVE7BIT2		0x10

#define D_ONSEMI_SENSOR_SLAVE7BIT	0x36
#define D_ONSEMI_SENSOR_SLAVE7BIT2	0x37

#define D_CAMEARA_ACCESS_SIZE_MAX	16

#define D_CAMERA_ACCESS_I2C_WRITE	0
#define D_CAMERA_ACCESS_I2C_READ	1

// AV
#define D_CAMERA_AV_F180			0x0001B22C		// F1.8 (AV1.696)
#define D_CAMERA_AV_F194			0x0001E980		// F1.94(AV1.912)
#define D_CAMERA_AV_F200			0x00020000		// F2.0 (AV2.000)
#define D_CAMERA_AV_F220			0x00023333		// F2.2 (AV2.200)
#define D_CAMERA_AV_F240			0x000286AC		// F2.4 (AV2.526)
#define D_CAMERA_AV_F260			0x0002C1CC		// F2.6 (AV2.757)
#define D_CAMERA_AV_F280			0x0002F889		// F2.8 (AV2.970)
#define D_CAMERA_AV_F360			0x0003B22C		// F3.6 (AV3.696)
#define D_CAMERA_AV_F449			0x0004555B		// F4.49(AV4.333)
#define D_CAMERA_AV_F547			0x0004E730		// F5.47(AV4.903)

// FPS
#define D_CAMERA_FPS_AUTO			(0.0)

#define D_FOCUS_DAC2LENS	0
#define D_FOCUS_LENS2DAC	1
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_CAMERA_ERR_OK		= 0x0000,
	E_CAMERA_ERR_NG		= 0x0001,
} E_CAMERA_ERR_CODE;

typedef enum {
	E_CAMERA_ID_0 = 0,
	E_CAMERA_ID_1,
	E_CAMERA_ID_2,
	E_CAMERA_ID_3,
	E_CAMERA_ID_MAX,
} E_CAMERA_ID;

typedef enum {
	E_CAMERA_ACCESS_I2C = 0,
	E_CAMERA_ACCESS_SPI,
	E_CAMERA_ACCESS_SPI_EX,
	E_CAMERA_ACCESS_MAX
} E_CAMERA_ACCESS;

typedef enum {
	E_CAMERA_SENSOR_INCLK_12MHz = 0,
	E_CAMERA_SENSOR_INCLK_24MHz,
	E_CAMERA_SENSOR_INCLK_36MHz,
	E_CAMERA_SENSOR_INCLK_37_125MHz,
	E_CAMERA_SENSOR_INCLK_72MHz,
	E_CAMERA_SENSOR_INCLK_MAX,
} E_CAMERA_SENSOR_INCLK;

typedef enum {
	E_CAMERA_SENSOR_IF_LVDS = 0,
	E_CAMERA_SENSOR_IF_SLVS,
	E_CAMERA_SENSOR_IF_DMIPI,
	E_CAMERA_SENSOR_IF_CMIPI,
	E_CAMERA_SENSOR_IF_MAX,
} E_CAMERA_SENSOR_IF; // should be same order as E_IM_PRO_SENTOP_CONN_TYPE_SEL

typedef enum {
	E_CAMERA_SENSOR_UPDATE_TIMING_VD = 0,
	E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING,
} E_CAMERA_SENSOR_UPDATE_TIMING;

typedef enum {
	E_CAMERA_SENSOR_REG_HDOCYC = 0,
	E_CAMERA_SENSOR_REG_VDOCYC,
	E_CAMERA_SENSOR_REG_EXP,
	E_CAMERA_SENSOR_REG_AGAIN,
	E_CAMERA_SENSOR_REG_DGAIN,
	
	E_CAMERA_SENSOR_REG_DOL_SHORT_OFST,
	E_CAMERA_SENSOR_REG_DOL_SHORT_EXP,
	E_CAMERA_SENSOR_REG_DOL_SHORT_AGAIN,
	E_CAMERA_SENSOR_REG_DOL_SHORT_DGAIN,
	// add if you need
	E_CAMERA_SENSOR_REG_MAX,
} E_CAMERA_SENSOR_REG;

typedef enum {
	E_CAMERA_GYRO_BUF_SIZE_SEND = 0,
	E_CAMERA_GYRO_BUF_SIZE_RECV,
	E_CAMERA_GYRO_BUF_SIZE_STAMP,
	E_CAMERA_GYRO_BUF_SIZE_MAX,
} E_CAMERA_GYRO_BUF_SIZE;

typedef enum {
	E_CAMERA_LVDS_SNCLK_SEL_STOP = 0,
	E_CAMERA_LVDS_SNCLK_SEL_SNCLK0,
	E_CAMERA_LVDS_SNCLK_SEL_SNCLK1,
	E_CAMERA_LVDS_SNCLK_SEL_SNCLK2,
	E_CAMERA_LVDS_SNCLK_SEL_SNCLK3
} E_CAMERA_LVDS_SNCLK_SEL;

typedef enum {
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA0 = 0,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA1,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA2,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA3,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA4,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA5,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA6,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA7,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA8,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA9,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA10,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA11,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA12,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA13,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA14,
	E_CAMERA_LVDS_SNDATA_SEL_SNDATA15
} E_CAMERA_LVDS_SNDATA_SEL;

typedef enum {
	E_CAMERA_SENSOR_SHUTTER_G_OFF_M_OPEN = 0x00,
	E_CAMERA_SENSOR_SHUTTER_G_OFF_M_CLOSE,
	E_CAMERA_SENSOR_SHUTTER_G_ON_M_OPEN,
	E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE,
} E_CAMERA_SENSOR_SHUTTER;

typedef enum {
	E_CAMERA_FOCUS_IF_VCM = 0,
	E_CAMERA_FOCUS_IF_MAX,
} E_CAMERA_FOCUS_IF;

typedef enum {
	E_CAMERA_FOCUS_MOVE_DIR_FORWARD_INIT = 0,
	E_CAMERA_FOCUS_MOVE_DIR_BACKWARD_INIT,
	E_CAMERA_FOCUS_MOVE_DIR_FORWARD,	// Infinity to Macro
	E_CAMERA_FOCUS_MOVE_DIR_BACKWARD,	// Macro to Infinity
	E_CAMERA_FOCUS_MOVE_DIR_MAX,
} E_CAMERA_FOCUS_MOVE_DIR;

typedef enum {
	E_CAMERA_FOCUS_DOF_INF = 0,
	E_CAMERA_FOCUS_DOF_1000CM,
	E_CAMERA_FOCUS_DOF_500CM,
	E_CAMERA_FOCUS_DOF_400CM,
	E_CAMERA_FOCUS_DOF_300CM,
	E_CAMERA_FOCUS_DOF_200CM,
	E_CAMERA_FOCUS_DOF_190CM,
	E_CAMERA_FOCUS_DOF_180CM,
	E_CAMERA_FOCUS_DOF_170CM,
	E_CAMERA_FOCUS_DOF_160CM,
	E_CAMERA_FOCUS_DOF_150CM,
	E_CAMERA_FOCUS_DOF_140CM,
	E_CAMERA_FOCUS_DOF_130CM,
	E_CAMERA_FOCUS_DOF_120CM,
	E_CAMERA_FOCUS_DOF_110CM,
	E_CAMERA_FOCUS_DOF_100CM,
	E_CAMERA_FOCUS_DOF_90CM,
	E_CAMERA_FOCUS_DOF_80CM,
	E_CAMERA_FOCUS_DOF_70CM,
	E_CAMERA_FOCUS_DOF_60CM,
	E_CAMERA_FOCUS_DOF_50CM,
	E_CAMERA_FOCUS_DOF_45CM,
	E_CAMERA_FOCUS_DOF_40CM,
	E_CAMERA_FOCUS_DOF_35CM,
	E_CAMERA_FOCUS_DOF_30CM,
	E_CAMERA_FOCUS_DOF_25CM,
	E_CAMERA_FOCUS_DOF_20CM,
	E_CAMERA_FOCUS_DOF_15CM,
	E_CAMERA_FOCUS_DOF_14CM,
	E_CAMERA_FOCUS_DOF_13CM,
	E_CAMERA_FOCUS_DOF_12CM,
	E_CAMERA_FOCUS_DOF_11CM,
	E_CAMERA_FOCUS_DOF_10CM,
	E_CAMERA_FOCUS_DOF_9CM,
	E_CAMERA_FOCUS_DOF_8CM,
	E_CAMERA_FOCUS_DOF_7CM,
	E_CAMERA_FOCUS_DOF_6CM,
	E_CAMERA_FOCUS_DOF_5CM,
	E_CAMERA_FOCUS_DOF_MAX,
} E_CAMERA_FOCUS_DOF_DISTANCE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	USHORT		address;
	UCHAR		data[2];
} T_CAMERA_SENSOR_REG;

typedef struct {
	UCHAR		chipid;
	UCHAR		address;
	UCHAR		data;
} T_CAMERA_SENSOR_REG_EX;

typedef struct {
	USHORT	sav[4];
	USHORT	eav[4];
	USHORT	sab[4];
	USHORT	eab[4];
} T_CAMERA_SENSOR_SYNC_CODE;

typedef struct {
	USHORT	msav[4];
	USHORT	meav[4];
	USHORT	msab[4];
	USHORT	meab[4];
} T_CAMERA_SENSOR_SYNC_MASK;

typedef struct {
	UCHAR		ch;
	UCHAR		slave7bit;
	UCHAR		rw;
	USHORT		address_len; // 0, 1, 2
	UCHAR		address[2];
	USHORT		data_len;
	UCHAR*		data;
} T_CAMERA_ACCESS_I2C;

typedef struct {
	E_CAMERA_SENSOR_INCLK			input_clock;
	E_CAMERA_ACCESS					access;
	E_CAMERA_SENSOR_IF				sensor_if;
	E_CAMERA_SENSOR_UPDATE_TIMING	update_timing;
} T_CAMERA_SENSOR_BASIC;

typedef struct {
	const T_CAMERA_SENSOR_REG*		mode_reg;
	const T_CAMERA_SENSOR_REG*		dol_mode_reg;
	const T_CAMERA_SENSOR_REG*		pdaf_mode_reg;

	const T_CAMERA_SENSOR_REG_EX*	mode_reg_ex;

	UINT32							embedded_lines;
	UINT32							total_pix_width;
	UINT32							total_pix_lines;
	UINT32							ob_pix_x_pos;
	UINT32							ob_pix_y_pos;
	UINT32							ob_pix_width;
	UINT32							ob_pix_lines;
	UINT32							active_pix_x_pos;
	UINT32							active_pix_y_pos;
	UINT32							active_pix_width;
	UINT32							active_pix_lines;
	FLOAT							active_pix_x_rsz_ratio;
	FLOAT							active_pix_y_rsz_ratio;
	USHORT							dualpd_line_rsz_rate; // [15:8] / [7:0]

	USHORT							pd_data_extend_lines;
	USHORT							pd_g_cnt;
	USHORT							pd_win_h_cnt;
	USHORT							pd_win_v_cnt;

	UINT32							hdocyc;	// or line length pck
	UINT32							vdocyc; // or frame length line
	UINT32							dol_hdocyc;	// or line length pck
	UINT32							dol_vdocyc; // or frame length line
	UINT32							exp_start_line_min;
	UINT32							exp_end_line_offset;
	UINT32							exp_clk_offset;
	FLOAT							exp_fine;

	FLOAT							base_SV;

	UCHAR							input_bit_width;
	UCHAR							dol_input_bit_width;
	FJ_PRO_CTRL_FIRST_PIX_COLOR		first_active_pix_color;
	FJ_PRO_CTRL_FIRST_PIX_COLOR		dol_first_active_pix_color;

	FLOAT							fps;
	FLOAT							dol_fps;

	FLOAT							Mpps_HDclk;
	FLOAT							one_lane_Mbps;
	UCHAR							lane_num;

	const T_CAMERA_SENSOR_SYNC_CODE*	sync_code; // for lvds only
	const T_CAMERA_SENSOR_SYNC_MASK*	sync_mask; // for lvds only
	const E_CAMERA_LVDS_SNCLK_SEL*		clock_select; // for lvds only
	const E_CAMERA_LVDS_SNDATA_SEL*		data_select; // for lvds only

} T_CAMERA_SENSOR_MODE;

typedef struct {
	FLOAT		max_fps;
	FLOAT		min_fps;
	FLOAT		exp_msec;
	FLOAT		gain_times;
	
	FLOAT		DOL_short_exp_msec;
	FLOAT		DOL_short_gain_times;
	UINT32		DOL_short_max_gap_lines;
	UCHAR		flicker_mode;
} T_CAMERA_CALC_EXP_IN;

typedef struct {
	FLOAT		fps;
	FLOAT		exp_msec;
	FLOAT		again_times;
	FLOAT		dgain_times;
	UINT32		hdocyc;
	UINT32		vdocyc;
	
	FLOAT		DOL_short_exp_msec;
	FLOAT		DOL_short_again_times;
	FLOAT		DOL_short_dgain_times;
} T_CAMERA_EXP;

typedef struct {
	UINT32		reg[E_CAMERA_SENSOR_REG_MAX];
} T_CAMERA_EXP_REG;

typedef struct {
	FLOAT	level;
	FLOAT	pd;
} T_CAMERA_PD_DATA;

typedef struct {
	FLOAT accel[3];		// 
	FLOAT gyro[3];		// 
	FLOAT compass[3];	// 
	FLOAT orientation[3];	// 
	FLOAT quat[4];	// 
} T_CAMERA_GYRO_MPU_DATA;

typedef struct {
	FLOAT	lens_um;
	FLOAT	obj_best_m;
	FLOAT	obj_far_m;
	FLOAT	obj_near_m;
	UCHAR	adj_on;
	FLOAT	adj_lens_um;
} T_CAMERA_FOCUS_DOF;

typedef struct {
	E_CAMERA_ACCESS					access;
	E_CAMERA_FOCUS_IF				focus_if;
	FJ_PERI_CH						i2c_ch;
	UCHAR							i2c_slave7bit;
	USHORT							mini_value;
	USHORT							max_value;
	USHORT							coarse_fw_step_num;
	USHORT							coarse_bw_step_num;
	USHORT							fine_fw_step_num;
	USHORT							fine_bw_step_num;
} T_CAMERA_FOCUS_BASIC;

typedef struct {
	FLOAT	now_um;
	USHORT	now_dac;
	USHORT	inf_dac;
	USHORT	mac_dac;
	USHORT	ini_dac;
	USHORT	sta_dac;
	USHORT	end_dac;
	USHORT	pos_dac;
	FLOAT	kando_slope;
	FLOAT	kando_intercept;
} T_CAMERA_FOCUS_PARAM;

typedef struct {

	UINT32								sensor_num;
	E_CAMERA_ERR_CODE					(*Func_Clock_GPIO_Init)(VOID);
	char								*sensor_str;
	UCHAR								start_frame_num;

	struct {
		
		const T_CAMERA_SENSOR_BASIC*	basic;
		const T_CAMERA_SENSOR_MODE*		mode;
		UINT32							mode_num;
		UINT32							mode_full;

		E_DD_TOP_GPIO_PORT				port_xclr;
		E_DD_TOP_GPIO_PORT				port_xhs;
		E_DD_TOP_GPIO_PORT				port_xvs;
		E_DD_TOP_GPIO_PORT				port_inck;
		FJ_PERI_CH						spi_ch;
		BOOL							is_spics_gpio;
		FJ_PERI_CH						i2c_ch;
		UCHAR							i2c_slave7bit;
		
		FLOAT							F_number;
		USHORT							ob_12bit;

		ULONG							wait_stable_ms;
		
		E_CAMERA_ERR_CODE				(*Func_Init)			(E_CAMERA_ID sensor_id);
		E_CAMERA_ERR_CODE				(*Func_Standby)			(E_CAMERA_ID sensor_id);
		E_CAMERA_ERR_CODE				(*Func_CancelStandby)	(E_CAMERA_ID sensor_id);
		E_CAMERA_ERR_CODE				(*Func_GlobalShutter)	(E_CAMERA_ID sensor_id, E_CAMERA_SENSOR_SHUTTER shutter_mode);
		E_CAMERA_ERR_CODE				(*Func_Mode)			(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
		E_CAMERA_ERR_CODE				(*Func_Pdaf_Mode)		(E_CAMERA_ID sensor_id, UINT32 mode);
		E_CAMERA_ERR_CODE				(*Func_Multi)			(E_CAMERA_ID sensor_id, BOOL is_multi, BOOL is_master);
		E_CAMERA_ERR_CODE				(*Func_SgVHD)			(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc);
		E_CAMERA_ERR_CODE				(*Func_Stream)			(E_CAMERA_ID sensor_id);
		E_CAMERA_ERR_CODE				(*Func_Callback)			(E_CAMERA_ID sensor_id, UCHAR enable );

		E_CAMERA_ERR_CODE				(*Func_Calc_Exp)		(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
		E_CAMERA_ERR_CODE				(*Func_Set_Exp)			(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);
		
		E_CAMERA_ERR_CODE				(*Func_DOL_Calc_Exp)	(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
		E_CAMERA_ERR_CODE				(*Func_DOL_Set_Exp)		(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);
		
		E_CAMERA_ERR_CODE				(*Func_Get_PdInfo)		(E_CAMERA_ID sensor_id, UINT32 mode, ULONG sta_addr, USHORT pd_num, T_CAMERA_PD_DATA* pd_info);
		
	} sensor[E_CAMERA_ID_MAX];
	
	struct {
		UINT32*							buf_size;
		FJ_GYROIF_READ_CTRL* 			ctrl;
		FLOAT							Sampling_Hz_Gyro;
		FLOAT							Sampling_Hz_Accel;
		
		E_CAMERA_ERR_CODE				(*Func_Init)			(VOID);
		E_CAMERA_ERR_CODE				(*Func_Standby)			(VOID);
		E_CAMERA_ERR_CODE				(*Func_CancelStandby)	(VOID);
		E_CAMERA_ERR_CODE				(*Func_Calc_Bias)		(ULONG time_ms, T_CAMERA_GYRO_MPU_DATA* bias );
		E_CAMERA_ERR_CODE				(*Func_GetData)			(INT32 read_cnt, T_CAMERA_GYRO_MPU_DATA* data );
		E_CAMERA_ERR_CODE				(*Func_StartRead)		(VOID);
		E_CAMERA_ERR_CODE				(*Func_StopRead)		(VOID);
		E_CAMERA_ERR_CODE				(*Func_GetReadCount)	(UINT32* read_count);
		FLOAT							(*Func_Accel2Gravity)	(const UCHAR data_h, const UCHAR data_l);
		FLOAT							(*Func_Gyro2Gravity)	(const UCHAR data_h, const UCHAR data_l);
		VOID							(*Func_DebugDump)		(VOID);
		
	} gyro;
	
	struct {
		const T_CAMERA_FOCUS_BASIC*		basic;
		const T_CAMERA_FOCUS_DOF*		dof;
		
		E_CAMERA_ERR_CODE				(*Func_VCM_Init)		(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm);
		E_CAMERA_ERR_CODE				(*Func_VCM_SetDAC)		(E_CAMERA_ID focus_id, USHORT dac);
		E_CAMERA_ERR_CODE				(*Func_VCM_GetDAC)		(E_CAMERA_ID focus_id, USHORT* value);
		E_CAMERA_ERR_CODE				(*Func_VCM_Move) 		(E_CAMERA_ID focus_id, E_CAMERA_FOCUS_MOVE_DIR dir, BOOL is_coarse, USHORT* value);
		E_CAMERA_ERR_CODE				(*Func_VCM_CalcDAC) 	(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm, UCHAR convert);
		E_CAMERA_ERR_CODE    			(*Func_VCM_Get_Cur_Focus_Value)  (USHORT* focus_Value);
	} focus[E_CAMERA_ID_MAX];
	
} T_CAMERA;
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define CAMERA_PRINT_ERROR(ercd)	do { if( ercd )printf( "(x_x) %s, %d, ercd=0x%x\n", __FUNCTION__, __LINE__, ercd );}while(0);

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Camera_Common_SPI_Write(FJ_PERI_CH ch, USHORT address,const  UCHAR* data, UCHAR size, BOOL is_cs_gpio);
E_CAMERA_ERR_CODE Camera_Common_SPI_Read(FJ_PERI_CH ch, USHORT address, UCHAR* data, UCHAR size, BOOL is_cs_gpio);
E_CAMERA_ERR_CODE Camera_Common_SPI_Write_Ex(UCHAR chipid, FJ_PERI_CH ch, UCHAR address, const  UCHAR* data, UCHAR size, BOOL is_cs_gpio);
E_CAMERA_ERR_CODE Camera_Common_SPI_Read_Ex(UCHAR chipid, FJ_PERI_CH ch, UCHAR address, UCHAR* data, UCHAR size, BOOL is_cs_gpio);

E_CAMERA_ERR_CODE Camera_Common_I2C_Write(FJ_PERI_CH ch, UCHAR slave7bit, USHORT address, UCHAR* data, UCHAR size);
E_CAMERA_ERR_CODE Camera_Common_I2C_Read(FJ_PERI_CH ch, UCHAR slave7bit, USHORT address, UCHAR* data, UCHAR size);
E_CAMERA_ERR_CODE Camera_Common_I2C_Write_2(FJ_PERI_CH ch, UCHAR slave7bit, UCHAR address, UCHAR* data, UCHAR size);
E_CAMERA_ERR_CODE Camera_Common_I2C_Read_2(FJ_PERI_CH ch, UCHAR slave7bit, UCHAR address, UCHAR* data, UCHAR size);

#endif
