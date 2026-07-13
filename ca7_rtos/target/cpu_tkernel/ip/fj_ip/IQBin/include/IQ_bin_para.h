/**
* @file		iq_bin_para.h
* @brief		None
* @note		None
* @attention	None
*
* <B><I>Copyright 2018 Socionext Inc.</I></B>
*/

#ifndef _IQ_BIN_PARA_H_
#define _IQ_BIN_PARA_H_
#ifdef WIN32
#include "..\MILB_API\include\ddim_typedef.h"
#else
#include "ddim_typedef.h"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define _IQ_Force_Update_

#ifdef __GNUC__ 
#define PACKED(class_to_pack) class_to_pack __attribute__((__packed__))
#else
#define PACKED( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

// ****************************************************************************
//  IQ.bin èÓïÒ
// ****************************************************************************
// Common Header
PACKED(
typedef struct {
	UCHAR		version[4];
	UINT32		total_size;
	UINT32		head_size;
	UCHAR		chip[8];
	UCHAR		mode;
	UCHAR		dummy[19];
	UINT32		main_addr;
	UINT32		main_size;
} 
)T_IQ_CMN_HEAD_v0121e;

PACKED(
typedef struct {
	USHORT		wblvrr;
	USHORT		wblvbb;
	USHORT		wblvgr;
	USHORT		wblvgb;
	USHORT		wblvr;
	USHORT		wblvg;
	USHORT		wblvb;
} 
)T_IQ_CMN_MAIN_v0121e;

PACKED(
	typedef struct {
	UCHAR		version[4];
	UINT32		total_size;
	UINT32		head_size;
	UCHAR		chip[8];
	UCHAR		mode;
	UCHAR		dummy[3];
	UINT32		sensorid;
	UINT32		prj_id;
	UINT32		version2;
	UINT32		date;
	UINT32		main_addr;
	UINT32		main_size;
	UCHAR		iso_index_num;
	UCHAR		scene_index_num;
	UCHAR		zoom_index_num;
	UCHAR		lens_index_num;
	UINT32		zaddr_sensor_linearity;
	UINT32		zaddr_obt;
	UINT32		zaddr_dpc;					
	UINT32		zaddr_elf;			
	UINT32		zaddr_ela;				
	UINT32		zaddr_offset_bay;
	UINT32		zaddr_demosaic;
	UINT32		zaddr_hfe_hfa;				
	UINT32		zaddr_offset_rgb;
	UINT32		zaddr_rgb_yb;
	UINT32		zaddr_mcc0;
	UINT32		zaddr_mcc1;
	UINT32		zaddr_tone;
	UINT32		zaddr_gamma;
	UINT32		zaddr_yc_conversion;
	UINT32		zaddr_yblend;
	UINT32		zaddr_ynr;
	UINT32		zaddr_ee_hpf;
	UINT32		zaddr_ee_coring;
	UINT32		zaddr_ee_scale;	
	UINT32		zaddr_ee_tone;	
	UINT32		zaddr_ee_clip;			
	UINT32		zaddr_ee_dnc;			
	UINT32		zaddr_ee_mpscl;			
	UINT32		zaddr_ee_cmp;			
	UINT32		zaddr_ee_crv;			
	UINT32		zaddr_clpf;
	UINT32		zaddr_csco;
	UINT32		zaddr_eear0;
	UINT32		zaddr_eear1;
	UINT32		zaddr_cnr_otf;
	UINT32		zaddr_cnr_ofl;
	UINT32		zaddr_cnr_ofl_ex;
	UINT32		zaddr_ltm;
	UINT32		zaddr_3dnr;
	UINT32		zaddr_mfnr;
	UINT32		zaddr_shdr;
	UINT32		zaddr_cag_sro;
	UINT32		zaddr_cag_b2b;
	UINT32		zaddr_ffc;
	UINT32		zaddr_ldc;

	USHORT		saddr_sensor_linearity;
	USHORT		saddr_obt;
	USHORT		saddr_dpc;
	USHORT		saddr_elf;
	USHORT		saddr_ela;
	USHORT		saddr_offset_bay;
	USHORT		saddr_demosaic;
	USHORT		saddr_hfe_hfa;
	USHORT		saddr_offset_rgb;
	USHORT		saddr_rgb_yb;
	USHORT		saddr_mcc_ctrl;
	USHORT		saddr_mcc0;
	USHORT		saddr_mcc1;
	USHORT		saddr_tone;
	USHORT		saddr_gamma;
	USHORT		saddr_yc_conversion;
	USHORT		saddr_yblend;
	USHORT		saddr_ynr;
	USHORT		saddr_ee_hpf;
	USHORT		saddr_ee_coring;
	USHORT		saddr_ee_scale;
	USHORT		saddr_ee_tone;
	USHORT		saddr_ee_clip;
	USHORT		saddr_ee_dnc;
	USHORT		saddr_ee_mpscl;
	USHORT		saddr_ee_cmp;
	USHORT		saddr_ee_crv;
	USHORT		saddr_clpf;
	USHORT		saddr_csco;
	USHORT		saddr_eear0;
	USHORT		saddr_eear1;
	USHORT		saddr_cnr_ctrl;
	USHORT		saddr_cnr_otf;
	USHORT		saddr_cnr_ofl;
	USHORT		saddr_cnr_ofl_ex;
	USHORT		saddr_ltm;
	USHORT		saddr_3dnr;
	USHORT		saddr_mfnr;
	USHORT		saddr_shdr;
	USHORT		saddr_cag_sro_ctrl;
	USHORT		saddr_cag_sro;
	USHORT		saddr_cag_b2b_ctrl;
	USHORT		saddr_cag_b2b;
	USHORT		saddr_ffc_ctrl;
	USHORT		saddr_ffc;
	ULONG		ab_saddr_ffc;
	USHORT		saddr_ldc_ctrl;
	USHORT		saddr_ldc;
	ULONG		ab_saddr_ldc;
} 
)T_IQ_MODE_HEAD_v0121e;


typedef struct {
	T_IQ_CMN_HEAD_v0121e*		cmn_head;
	T_IQ_CMN_MAIN_v0121e*		cmn_main;
	T_IQ_MODE_HEAD_v0121e*		mode_head;
} T_IQ_BIN;

typedef struct {
	struct {
		SHORT	CCM1[9];
	} LIST[11][11];
} T_IQ_CCM1_PARAM;

typedef struct {
	SHORT obt;
	USHORT *gamma_FL;
	ULLONG *gamma_DF;
	SHORT	saturation; // -99% ~ +99%
	SHORT	sharpness; // -100% ~ +200%
	USHORT	level_3dnr; // 0 ~ 8
	SHORT	nr_2d_level; // 0: Normal (By IQ.bin), 1:Off, 2:Low, 3:Mid, 4:High
	SHORT	*ccm1;
	USHORT	wdr_en;
	SHORT	contrast; // -100% ~ +100%
	USHORT	scene_normal;
	USHORT	scene_hdr_1st_run;
} T_IQ_PARAM;
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/


#endif
