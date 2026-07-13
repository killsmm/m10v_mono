/**
 * @file		im_me.h
 * @brief		JDSME macro wrapper.
 * @note		None.
 * @attention	None.
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

/**
	@addtogroup im_me
	@{
*/

#ifndef __IM_ME_H__
#define __IM_ME_H__

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Result.
#define	D_IM_ME_INPUT_PARAM_ERROR	(D_IM_ME | D_DDIM_INPUT_PARAM_ERROR)	/**< Processing result code<br> Input parameter error			(0x50000001) */
#define	D_IM_ME_MACRO_BUSY			(D_IM_ME | D_DDIM_MACRO_BUSY_NG)		/**< Processing result code<br> Macro busy 						(0x50000005) */
#define D_IM_ME_SEM_NG				(D_IM_ME | D_DDIM_SEM_NG)				/**< Processing result code<br> Semaphore acquisition NG		(0x50000010) */
#define	D_IM_ME_SEM_TIMEOUT_ERR		(D_IM_ME | D_DDIM_SEM_TIMEOUT)			/**< Processing result code<br> Semaphore acquisition Time Out	(0x50000011) */
#define	D_IM_ME_TIMEOUT				(D_IM_ME | D_DDIM_TIMEOUT)				/**< Processing result code<br> Time Out 						(0x50000013) */
#define	D_IM_ME_AXI_ERROR			(D_IM_ME | D_DDIM_AXI_ERROR)			/**< Processing result code<br> AXI BUS error					(0x5000001A) */
#define	D_IM_ME_NG					(D_IM_ME | D_DDIM_SYSTEM_ERROR)			/**< Processing result code<br> Processing NG					(0x50000099) */
#define	D_IM_ME_PROCESS_END			(D_IM_ME | 0x000000FF)					/**< Processing result code<br> Process End 					(0x500000FF) */

// AXI Result.
#define D_IM_ME_AXI_NORMAL_END					(0)			/**< Normal end.													*/
#define D_IM_ME_AXI_ERR_CH_W_PRE				(1)			/**< The AXI error occurred by the write channel (Previous).		*/
#define D_IM_ME_AXI_ERR_CH_W_VEC				(2)			/**< The AXI error occurred by the write channel (Vector).			*/
#define D_IM_ME_AXI_ERR_CH_R_CUR				(3)			/**< The AXI error occurred by the read channel  (Current).			*/
#define D_IM_ME_AXI_ERR_CH_RW					(4)			/**< The AXI error occurred by the read channel and write channel.	*/

// OFF/ON.
#define D_IM_ME_OFF								(0)			/**< OFF.												*/
#define D_IM_ME_ON								(1)			/**< ON.												*/

// DISABLE/ENABLE.
#define D_IM_ME_DISABLE							(0)			/**< DISABLE.											*/
#define D_IM_ME_ENABLE							(1)			/**< ENABLE.											*/

// Vector cancel mode.
#define D_IM_ME_VEC_CANCEL_MODE_OFF				(0)			/**< Cancel off. (Ribery. PME Interchangeable output)	*/
#define D_IM_ME_VEC_CANCEL_MODE_CUR1			(1)			/**< Current image.	Pixel comparison.					*/
#define D_IM_ME_VEC_CANCEL_MODE_CUR2			(2)			/**< Current image.	Average value comparison.			*/

// Vector height size.
#define D_IM_ME_PRE_VEC_HSIZE_512				(512)		/**<  512 byte.											*/
#define D_IM_ME_PRE_VEC_HSIZE_1024				(1024)		/**< 1024 byte.											*/

//Interrupt mode (MEINT).
#define D_IM_ME_INTMODE_AFTER_CYCLE				(0)			/**< After a certain cycle, interrupt issuing preparation start.	*/
#define D_IM_ME_INTMODE_LOGICAL_OR				(1)			/**< Logical OR output.												*/

// Current image margin size.
#define D_IM_ME_CUR_MARGIN_0					(0)			/**< 0 pixel.											*/
#define D_IM_ME_CUR_MARGIN_4					(4)			/**< 4 pixel.											*/
#define D_IM_ME_CUR_MARGIN_8					(8)			/**< 8 pixel.											*/

// Request arbitration method selection.
#define D_IM_ME_ABT_FIXED						(0)			/**< Fixed priority.									*/
#define D_IM_ME_ABT_ROUND						(1)			/**< Round robin.										*/

// MB number max.
#define D_IM_ME_MBNUM_H_MAX						(260)		/**< 260.												*/
#define D_IM_ME_MBNUM_V_MAX						(192)		/**< 192.												*/

// Input image mode.
#define D_IM_ME_INPUTMODE_INTERLACE				(0)			/**< Interlace.											*/
#define D_IM_ME_INPUTMODE_PROGRESSIVE			(1)			/**< Progressive.										*/

// Reference image Top/Bottom.
#define D_IM_ME_REFTXB_TOP						(0)			/**< Top field.											*/
#define D_IM_ME_REFTXB_BOTTOM					(1)			/**< Bottom field.										*/

// The limit of the use of off-screen as a reference pixel.
#define D_IM_ME_OUTLIMIT_NOLIMIT0				(0)			/**< No limit.											*/
#define D_IM_ME_OUTLIMIT_NOLIMIT1				(1)			/**< No limit.											*/
#define D_IM_ME_OUTLIMIT_BOTTOM					(2)			/**< Outside of bottom image is out of reference.		*/
#define D_IM_ME_OUTLIMIT_TOP					(3)			/**< Outside of top image is out of reference.			*/

// PME processing target number of screens.
#define D_IM_ME_BANKNUM_1						(1)			/**< 1 (Progressive P, Interlaced P).					*/
#define D_IM_ME_BANKNUM_2						(2)			/**< 2 (Progressive B).									*/
#define D_IM_ME_BANKNUM_3						(3)			/**< 3.													*/
#define D_IM_ME_BANKNUM_4						(4)			/**< 4 (Interlaced B).									*/

// Search direction.
#define D_IM_ME_M1ORG_DIR_FWD					(0)			/**< FWD side.											*/
#define D_IM_ME_M1ORG_DIR_BWD					(1)			/**< BWD side.											*/

// Parity.
#define D_IM_ME_M1ORG_PARI_TOP					(0)			/**< Top.												*/
#define D_IM_ME_M1ORG_PARI_BOTTOM				(1)			/**< Bottom.											*/

// ME1 information read mode.
#define D_IM_ME_M1ORG_READ_FIXED				(0)			/**< Fixed 0. 											*/
#define D_IM_ME_M1ORG_READ_SDRAM				(1)			/**< Read from SDRAM.									*/

// SAD0/1 write mode.
#define D_IM_ME_M1ORG_SAD_SAVE					(0)			/**< Save MB_SAD0/1.									*/
#define D_IM_ME_M1ORG_SAD_WRITE					(1)			/**< Write SAD to MB_SAD0/1.							*/
#define D_IM_ME_M1ORG_SAD_COPY					(2)			/**< Copy MB_SAD0/1.									*/
#define D_IM_ME_M1ORG_SAD_MIN0					(3)			/**< Write MIN(MB_SAD0, SAD) to MB_SAD0/1.				*/
#define D_IM_ME_M1ORG_SAD_MIN1					(4)			/**< Write MIN(MB_SAD1, SAD) to MB_SAD0/1.				*/

// Vector write mode.
#define D_IM_ME_M1ORG_VECM_WRITE0				(0)			/**< Write MB_MV0, MB_MH0								*/
#define D_IM_ME_M1ORG_VECM_WRITE1				(1)			/**< Write MB_MV1, MB_MH1								*/

// Vector selection.
#define D_IM_ME_M1ORG_VECS_SAVE					(0)			/**< Save MB_MV2, MB_MH2.								*/
#define D_IM_ME_M1ORG_VECS_COPY0				(2)			/**< Copy MB_MV0, MB_MH0 to MB_MV2, MB_MH2.				*/
#define D_IM_ME_M1ORG_VECS_COPY1				(3)			/**< Copy MB_MV1, MB_MH1 to MB_MV2, MB_MH2.				*/

// Picture type.
#define D_IM_ME_M1ORG_PICT_P					(0)			/**< P Pic.												*/
#define D_IM_ME_M1ORG_PICT_B					(1)			/**< B Pic.												*/
#define D_IM_ME_M1ORG_PICT_I					(2)			/**< I Pic.												*/

// M1 correct KY/X max.
#define D_IM_ME_M1CRCT_KY_MAX					(4)			/**< KY max.											*/
#define D_IM_ME_M1CRCT_KX_MAX					(3)			/**< KX max.											*/

// M1 split mode.
#define D_IM_ME_SPLITMODE_NORMAL				(0)			/**< Normal.											*/
#define D_IM_ME_SPLITMODE_HORIZONTAL			(1)			/**< Horizontal split.									*/
#define D_IM_ME_SPLITMODE_VERTICAL				(2)			/**< Vertical split.									*/

// PME Core mode parameter.
#define D_IM_ME_LINEINTV_0QMB					(0)			/**< 0QMB line interval.								*/
#define D_IM_ME_LINEINTV_1QMB					(1)			/**< 1QMB line interval.								*/
#define D_IM_ME_LINEINTV_2QMB					(2)			/**< 2QMB line interval.								*/
#define D_IM_ME_LINEINTV_3QMB					(3)			/**< 3QMB line interval.								*/

// Vector search limitation max.
#define	D_IM_ME_VEC_LIMITV_MAX					(12)		/**< Horizon max.										*/
#define	D_IM_ME_VEC_LIMITH_MAX					(16)		/**< Vertical max.										*/

// Previous image width min/max.
#define	D_IM_ME_PRE_WIDTH_MIN					(48)		/**< Min.												*/
#define	D_IM_ME_PRE_WIDTH_MAX					(1024)		/**< Max.												*/

// Previous image height min/max.
#define	D_IM_ME_PRE_HEIGHT_MIN					(32)		/**< Min.												*/
#define	D_IM_ME_PRE_HEIGHT_MAX					(752)		/**< Max.												*/

// Prefetch image width min/max.
#define	D_IM_ME_PREF_WIDTH_MIN					(192)		/**< Min.												*/
#define	D_IM_ME_PREF_WIDTH_MAX					(4160)		/**< Max.												*/

// Prefetch image height min/max.
#define	D_IM_ME_PREF_HEIGHT_MIN					(128)		/**< Min.												*/
#define	D_IM_ME_PREF_HEIGHT_MAX					(3072)		/**< Max.												*/

// Search width min/max.
#define	D_IM_ME_SRCH_WIDTH_MIN					(40)		/**< Min.												*/
#define	D_IM_ME_SRCH_WIDTH_MAX					(128)		/**< Max.												*/

// Search height min/max.
#define	D_IM_ME_SRCH_HEIGHT_MIN					(8)			/**< Min.												*/
#define	D_IM_ME_SRCH_HEIGHT_MAX					(48)		/**< Max.												*/

// Previous image shift min/max.
#define	D_IM_ME_PRE_SHIFT_MIN					(-64)		/**< Min.												*/
#define	D_IM_ME_PRE_SHIFT_MAX					(63)		/**< Max.												*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** PP number */
typedef enum {
	E_IM_ME_PPNUM_PPA = 0,				/**< PPA						*/
	E_IM_ME_PPNUM_PPB,					/**< PPB						*/
	E_IM_ME_PPNUM_PPC,					/**< PPC						*/
	E_IM_ME_PPNUM_MAX,					/**< Maximum value				*/
} E_IM_ME_PPNUM;

/** BANK number */
typedef enum {
	E_IM_ME_BANKNUM_0 = 0,				/**< BANK 0						*/
	E_IM_ME_BANKNUM_1,					/**< BANK 1						*/
	E_IM_ME_BANKNUM_2,					/**< BANK 2						*/
	E_IM_ME_BANKNUM_3,					/**< BANK 3						*/
	E_IM_ME_BANKNUM_MAX,				/**< Maximum value				*/
} E_IM_ME_BANKNUM;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** User callback function pointer(Frame complete) */
typedef VOID (*T_IM_ME_CALLBACK)( ULONG interrupt_flag_mask );

/** APBBRG control parameter	*/
typedef struct {
	UCHAR					pp_enable[ E_IM_ME_PPNUM_MAX ];
														/**< PP part A/B/C core enable.
															<ul>
																<li>@ref D_IM_ME_DISABLE
																<li>@ref D_IM_ME_ENABLE
															</ul>
														*/
	UCHAR					int_mode;					/**< Interrupt mode (MEINT).
															<ul>
																<li>@ref D_IM_ME_INTMODE_AFTER_CYCLE
																<li>@ref D_IM_ME_INTMODE_LOGICAL_OR
															</ul>
														*/
	USHORT					interval;					/**< Interrupt output interval (10bit).		*/
} T_IM_ME_CTRL_APBBRG;

/** M1 Original image mode parameter	*/
typedef struct {
	UCHAR					direction;					/**< Search direction.
															<ul>
																<li>@ref D_IM_ME_M1ORG_DIR_FWD
																<li>@ref D_IM_ME_M1ORG_DIR_BWD
															</ul>
														*/
	UCHAR					parity;						/**< Parity.
															<ul>
																<li>@ref D_IM_ME_M1ORG_PARI_TOP
																<li>@ref D_IM_ME_M1ORG_PARI_BOTTOM
															</ul>
														*/
	UCHAR					read_mode;					/**< ME1 information read mode.
															<ul>
																<li>@ref D_IM_ME_M1ORG_READ_FIXED
																<li>@ref D_IM_ME_M1ORG_READ_SDRAM
															</ul>
														*/
	UCHAR					sad0_mode;					/**< SAD0 write mode.
															<ul>
																<li>@ref D_IM_ME_M1ORG_SAD_SAVE
																<li>@ref D_IM_ME_M1ORG_SAD_WRITE
																<li>@ref D_IM_ME_M1ORG_SAD_COPY
																<li>@ref D_IM_ME_M1ORG_SAD_MIN0
																<li>@ref D_IM_ME_M1ORG_SAD_MIN1
															</ul>
														*/
	UCHAR					sad1_mode;					/**< SAD1 write mode.
															<ul>
																<li>@ref D_IM_ME_M1ORG_SAD_SAVE
																<li>@ref D_IM_ME_M1ORG_SAD_WRITE
																<li>@ref D_IM_ME_M1ORG_SAD_COPY
																<li>@ref D_IM_ME_M1ORG_SAD_MIN0
																<li>@ref D_IM_ME_M1ORG_SAD_MIN1
															</ul>
														*/
	UCHAR					vec_mode;					/**< Vector write mode.
															<ul>
																<li>@ref D_IM_ME_M1ORG_VECM_WRITE0
																<li>@ref D_IM_ME_M1ORG_VECM_WRITE1
															</ul>
														*/
	UCHAR					vec_select;					/**< Vector selection.
															<ul>
																<li>@ref D_IM_ME_M1ORG_VECS_SAVE
																<li>@ref D_IM_ME_M1ORG_VECS_COPY0
																<li>@ref D_IM_ME_M1ORG_VECS_COPY1
															</ul>
														*/
	UCHAR					pic_type;					/**< Picture type.
															<ul>
																<li>@ref D_IM_ME_M1ORG_PICT_P
																<li>@ref D_IM_ME_M1ORG_PICT_B
																<li>@ref D_IM_ME_M1ORG_PICT_I
															</ul>
														*/
} T_IM_ME_CTRL_PME_M1ORG;

/** M1 Correct parameter	*/
typedef struct {
	UCHAR					front_ky;					/**< Front ky for ME1(L0 direction). (3bit 0~4)				*/
	UCHAR					back_ky;					/**< Back  ky for ME1(L1 direction). (3bit 0~4)				*/
	UCHAR					front_kx;					/**< Front kx for ME1(L0 direction). (2bit 0~3)				*/
	UCHAR					back_kx;					/**< Back  kx for ME1(L1 direction). (2bit 0~3)				*/
	UCHAR					mode;						/**< mode[ 0 ]:CPU specified vector base.<br>
															 mode[ 1 ]:0 vector.<br>
															 mode[ 2 ]:Lower left MB vector base.<br>
															 mode[ 3 ]:Upper left MB vector base.<br>
														*/
	UCHAR					front_h;					/**< Correction base position. (Front, horizontal, 6bit, mode[ 0 ]=1 valid only)	*/
	UCHAR					front_v;					/**< Correction base position. (Front, vertical,   5bit, mode[ 0 ]=1 valid only)	*/
	UCHAR					back_h;						/**< Correction base position. (Back,  horizontal, 6bit, mode[ 0 ]=1 valid only)	*/
	UCHAR					back_v;						/**< Correction base position. (Back,  vertical,   5bit, mode[ 0 ]=1 valid only)	*/
} T_IM_ME_CTRL_PME_M1CRCT;

/** M1 MC on/off parameter	*/
typedef struct {
	USHORT					p_pic;						/**< MC on/off decision threshold for P pic. (13bit).<br>
															 1. Normal.<br>
															  - Accept.
															 2. SAD of fixed 0 vector.<br>
															  - Set 0x1FFE.
															 3. SAD is 0.<br>
															  - Set 0x1FFF.
														*/
	USHORT					b_pic;						/**< MC on/off decision threshold for B pic. (13bit).
															 1. Normal.<br>
															  - Accept.
															 2. SAD of fixed 0 vector.<br>
															  - Set 0x1FFE.
															 3. SAD is 0.<br>
															  - Set 0x1FFF.
														*/
} T_IM_ME_CTRL_PME_M1MC;

/** M1 Split parameter	*/
typedef struct {
	UCHAR					mode;						/**< Split mode.
															<ul>
																<li>@ref D_IM_ME_SPLITMODE_NORMAL
																<li>@ref D_IM_ME_SPLITMODE_HORIZONTAL
																<li>@ref D_IM_ME_SPLITMODE_VERTICAL
															</ul>
														*/
	UCHAR					th_h;						/**< Threshold for horizontal split. (8bit)	*/
	UCHAR					th_v;						/**< Threshold fot vertical split. (8bit)	*/
} T_IM_ME_CTRL_PME_M1SPLT;

/** M1 Sad parameter	*/
typedef struct {
	USHORT					upper;						/**< Upper value. (12bit)	*/
	USHORT					lower;						/**< Lower value. (12bit)	*/
} T_IM_ME_CTRL_PME_M1SAD;

/** M1 Adjust parameter	*/
typedef struct {
	USHORT					wait_cycle;					/**< Wait cycle. (15bit)	*/
	UCHAR					wait_onoff;					/**< Wait on/off.
															<ul>
																<li>@ref D_IM_ME_OFF
																<li>@ref D_IM_ME_ON
															</ul>
														*/
} T_IM_ME_CTRL_PME_M1ADJUST;

/** M1 Pause parameter	*/
typedef struct {
	UCHAR					pause_line;					/**< Pause QMB line. (0~QMB line-1, 7bit)	*/
	UCHAR					pause_onoff;				/**< Pause on/off.
															<ul>
																<li>@ref D_IM_ME_OFF
																<li>@ref D_IM_ME_ON
															</ul>
														*/
} T_IM_ME_CTRL_PME_M1PAUSE;

/** M1 Limit parameter	*/
typedef struct {
	UCHAR					v_plus;						/**< Vector search limitation parameter (Vertical   plus  side. 4bit. 0~12).	*/
	UCHAR					v_minus;					/**< Vector search limitation parameter (Vertical   minus side. 4bit. 0~12).	*/
	UCHAR					h_plus;						/**< Vector search limitation parameter (Horizontal plus  side. 5bit. 0~16).	*/
	UCHAR					h_minus;					/**< Vector search limitation parameter (Horizontal minus side. 5bit. 0~16).	*/
} T_IM_ME_CTRL_PME_M1LIMIT;

/** PME Core mode parameter		*/
typedef struct {
	UCHAR					line_interval;				/**< QME line interval.
															<ul>
																<li>@ref D_IM_ME_LINEINTV_0QMB
																<li>@ref D_IM_ME_LINEINTV_1QMB
																<li>@ref D_IM_ME_LINEINTV_2QMB
																<li>@ref D_IM_ME_LINEINTV_3QMB
															</ul>
														*/
	UCHAR					init_pos_y;					/**< QMB initial y position. (2bit)	*/
} T_IM_ME_CTRL_PME_COREMODE;

/** PME control parameter		*/
typedef struct {
	UCHAR					input_mode;					/**< Input image mode.
															<ul>
																<li>@ref D_IM_ME_INPUTMODE_INTERLACE
																<li>@ref D_IM_ME_INPUTMODE_PROGRESSIVE
															</ul>
														*/
	UCHAR					ref_txb;					/**< Reference image Top/Bottom (if input_mode is progressive, select D_IM_ME_REFTXB_TOP).
															<ul>
																<li>@ref D_IM_ME_REFTXB_TOP
																<li>@ref D_IM_ME_REFTXB_BOTTOM
															</ul>
														*/
	UCHAR					out_limit;					/**< The limit of the use of off-screen as a reference pixel.
															<ul>
																<li>@ref D_IM_ME_OUTLIMIT_NOLIMIT0
																<li>@ref D_IM_ME_OUTLIMIT_NOLIMIT1
																<li>@ref D_IM_ME_OUTLIMIT_BOTTOM
																<li>@ref D_IM_ME_OUTLIMIT_TOP
															</ul>
														*/
	UCHAR					bank_num;					/**< PME processing target number of screens.
															<ul>
																<li>@ref D_IM_ME_BANKNUM_1
																<li>@ref D_IM_ME_BANKNUM_2
																<li>@ref D_IM_ME_BANKNUM_3
																<li>@ref D_IM_ME_BANKNUM_4
															</ul>
														*/
														/**< M1 Original image mode.				*/
	T_IM_ME_CTRL_PME_M1ORG		m1org_mode[ E_IM_ME_BANKNUM_MAX ];
	T_IM_ME_CTRL_PME_M1CRCT		m1correct;				/**< M1 Correct parameter.					*/
	T_IM_ME_CTRL_PME_M1MC		m1mc;					/**< M1 MC on/off parameter.				*/
	T_IM_ME_CTRL_PME_M1SPLT		m1splt;					/**< M1 Split parameter.					*/
	UCHAR						m1skip;					/**< M1 Skip parameter.
															<ul>
																<li>@ref D_IM_ME_OFF
																<li>@ref D_IM_ME_ON
															</ul>
														*/
	T_IM_ME_CTRL_PME_M1SAD		m1sad;					/**< M1 Sad parameter.						*/
	T_IM_ME_CTRL_PME_M1ADJUST	m1adjust;				/**< M1 Adjust parameter.					*/
	T_IM_ME_CTRL_PME_M1PAUSE	m1pause;				/**< M1 Pause parameter.					*/
	T_IM_ME_CTRL_PME_M1LIMIT	m1limit;				/**< M1 Limit parameterer.					*/
	T_IM_ME_CTRL_PME_COREMODE	core_mode;				/**< PME Core mode parameterer.				*/
} T_IM_ME_CTRL_PME;

/** Bank table parameter	*/
typedef struct {
	UCHAR					fwd_pvec;					/**< Physical bank number for foward vector data read/write access. (8bit)	*/
	UCHAR					top;						/**< Physical bank number for TopField of Interlace.  (8bit)				*/
	UCHAR					fwd_8pel;					/**< Physical bank number for 1/8 reference image read access. (8bit)		*/
} T_IM_ME_CTRL_RMCIF_BNKTBL;

/** AXI interface			*/
typedef struct {
	UCHAR					r_cache_type;				/**< Read cache type for AXI IF (4bit)									*/
	UCHAR					r_protect_type;				/**< Read protect type for AXI IF (3bit)								*/
	UCHAR					w_cache_type;				/**< Write cache type for AXI IF (4bit)									*/
	UCHAR					w_protect_type;				/**< Write protect type for AXI IF (3bit)								*/
} T_IM_ME_CTRL_RMCIF_AXIIF;

/** RMCIF control parameter		*/
typedef struct {
	UCHAR						pme_num;				/**< Number of PME use. (2bit. 0~2)<br>
															pme_num = 0 : PME 1 use.<br>
															pme_num = 1 : PME 2 use.<br>
															pme_num = 2 : PME 3 use.<br>
														*/
	UCHAR						abt_type;				/**< Request arbitration method selection.
															<ul>
																<li>@ref D_IM_ME_ABT_FIXED
																<li>@ref D_IM_ME_ABT_ROUND
															</ul>
														*/
	T_IM_ME_CTRL_RMCIF_BNKTBL	bank_tbl;				/* Bank table parameter.					*/
	T_IM_ME_CTRL_RMCIF_AXIIF	axi_if;					/* AXI interface.							*/
} T_IM_ME_CTRL_RMCIF;

/** ME control parameter */
typedef struct {
	T_IM_ME_CTRL_APBBRG		apbbrg;						/**< APBBRG control parameter.				*/
	T_IM_ME_CTRL_PME		pme[ E_IM_ME_PPNUM_MAX ];	/**< PME    control parameter.				*/
	T_IM_ME_CTRL_RMCIF		rmcif;						/**< RMCIF  control parameter.				*/
	T_IM_ME_CALLBACK		pCallBack;					/**< User callback function. (Frame comp)	*/
} T_IM_ME_CTRL;

/** PME start parameter */
typedef struct {
	USHORT					mbnum_h;					/**< MB number (Horizontal 9bit. Max:256)	*/
	USHORT					mbnum_v;					/**< MB number (Vertical   8bit. Max:144)	*/
	UCHAR					cancel_mode;				/**< Vector cancel mode.
															<ul>
																<li>@ref D_IM_ME_VEC_CANCEL_MODE_OFF
																<li>@ref D_IM_ME_VEC_CANCEL_MODE_CUR1
																<li>@ref D_IM_ME_VEC_CANCEL_MODE_CUR2
															</ul>
														*/
	UCHAR					cancel_threshold_hi;		/**< Vector cancel comparison threshold high. (8bit)	*/
	UCHAR					cancel_threshold_lo;		/**< Vector cancel comparison threshold low.  (8bit)	*/
	USHORT					vec_hsize;					/**< Vector output foramt horizontal size.<br>
															 - Reduction image horizontal size <= 1024 : set  512 byte.<br>
															 - Reduction image horizontal size  > 1024 : set 1024 byte.<br>
															<ul>
																<li>@ref D_IM_ME_PRE_VEC_HSIZE_512
																<li>@ref D_IM_ME_PRE_VEC_HSIZE_1024
															</ul>
														*/
} T_IM_ME_START_PME;

/** RMCIF start parameter */
typedef struct {
	CHAR					pre_shift_x;				/**< Previous image shift X. (7bit. -64~63)								*/
	CHAR					pre_shift_y;				/**< Previous image shift Y. (7bit. -64~63)								*/

	USHORT					pre_r_addr_hi;				/**< Previous image base address high[31:16]. (16bit)					*/
	USHORT					pre_r_addr_lo;				/**< Previous image base address low [15:12].
															(16bit. Previous image base address[31:12] is 16K byte align)		*/
	USHORT					cur_r_addr_hi;				/**< Current image base address high[31:16]. (16bit)					*/
	USHORT					cur_r_addr_lo;				/**< Current image base address low[15:12].
															(16bit. Current image base address[31:12] is 16K byte align)		*/
	USHORT					vec_w_addr_hi;				/**< Vector information base address high[31:16]. (16bit)				*/
	USHORT					vec_w_addr_lo;				/**< Vector information base address low[15:10].
															(16bit. Vector information base address[31:10] is 1K byte align)	*/
	USHORT					pre_gwidth;					/**< Previous image global width. (11bit. 128 byte align)				*/
	USHORT					pre_width;					/**< Previous image width.        (11bit. 48~1024 pixel. unit 4 pixel)	*/
	USHORT					pre_height;					/**< Previous image height.       (10bit. 32~ 752 pixel. unit 4 pixel)	*/
	USHORT					cur_gwidth;					/**< Current  image global width. (11bit. 128 byte align)				*/
	USHORT					cur_margin_width;			/**< Current  image margin size Left/Right.
															<ul>
																<li>@ref D_IM_ME_CUR_MARGIN_0
																<li>@ref D_IM_ME_CUR_MARGIN_4
																<li>@ref D_IM_ME_CUR_MARGIN_8
															</ul>
														*/
	USHORT					cur_margin_height;			/**< Current  image margin size Top/Bottom.
															<ul>
																<li>@ref D_IM_ME_CUR_MARGIN_0
																<li>@ref D_IM_ME_CUR_MARGIN_4
																<li>@ref D_IM_ME_CUR_MARGIN_8
															</ul>
														*/
	USHORT					prefetch_width;				/**< Prefetch image width. (13bit. 192~4160 pixel)<br>
															ex. prefetch_width = ( pre_width + ( cur_margin_width * 2 ) ) * 4
														*/
	USHORT					prefetch_height;			/**< Prefetch image height. (12bit. 128~3072 pixel)<br>
															ex. prefetch_height = ( pre_height + ( cur_margin_height * 2 ) ) * 4
														*/
	UCHAR					search_width;				/**< ME1 initialize search width. ( 8bit. 40~136 pixel)<br>
															ex1. cur_margin_width = 4 or 8 : search_width = 136<br>
															ex2. cur_margin_width = 0      : search_width = 136 - 8<br>
														*/
	UCHAR					search_height;				/**< ME1 initialize search height.( 6bit. 8~48 pixel)<br>
															ex1. pme_num = 2 : search_height = 40<br>
															ex2. pme_num = 1 : search_height = 32<br>
															ex3. pme_num = 0 : search_height = 24<br>
														*/
} T_IM_ME_START_RMCIF;

/** ME start parameter */
typedef struct {
	T_IM_ME_START_PME		pme[ E_IM_ME_PPNUM_MAX ];	/**< PME   start parameter.		*/
	T_IM_ME_START_RMCIF		rmcif;						/**< RMCIF start parameter.		*/
} T_IM_ME_START;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special.

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special.

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
ME initialize.<br>
@retval			D_DDIM_OK					Success.
*/
extern INT32 Im_ME_Init( VOID );

/**
The semaphore of ME is acquired.<br>
@param[in]		wait_time					Time-out period.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_INPUT_PARAM_ERROR	Parameter error.
@retval			D_IM_ME_SEM_NG				Semaphore acquisition NG.
@remarks		This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
				This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Im_ME_Open( const INT32 wait_time );

/**
The semaphore of ME is returned.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_SEM_NG				Semaphore acquisition NG.
@remarks		This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Im_ME_Close( VOID );

/**
ME control parameters set.<br>
@param[in]		me_ctrl						ME control parameters. See @ref T_IM_ME_CTRL.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_INPUT_PARAM_ERROR	Parameter error.
@remarks		This API uses DDIM_User_Dly_Tsk().
*/
extern INT32 Im_ME_Ctrl( const T_IM_ME_CTRL* const me_ctrl );

/**
ME start.<br>
@param[in]		me_start					ME start parameters. See @ref T_IM_ME_START.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_INPUT_PARAM_ERROR	Parameter error.
@remarks		This API uses DDIM_User_Dly_Tsk().
@remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_ME_Start( const T_IM_ME_START* const me_start );

/**
ME wait end.<br>
@param[out]		p_wait_factor				Pointer of factor parameter which release wait process.
@param[in]		wait_time					Wait time [msec]. The valid range is -1, 0, ... .
											If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_INPUT_PARAM_ERROR	Parameter error.
@retval			D_IM_ME_TIMEOUT				Time out.
@retval			D_IM_ME_AXI_ERROR			AXI BUS error.
@retval			D_IM_ME_NG					System NG.
@remarks		This API uses DDIM_User_Clr_Flg().
@remarks		This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_ME_Wait_End( UINT32* const p_wait_factor, const INT32 wait_time );

/**
ME interupt handler.<br>
@retval			None.
@remarks		This API uses DDIM_User_Set_Flg().
*/
extern VOID Im_ME_Int_Handler( VOID );

/*@}*/	// end of im_me

#ifdef __cplusplus
}
#endif

#endif	// __IM_ME_H__

/**
@weakgroup im_me
@{
	@section im_me_code Sample code
	- @ref ME_Init
	- @ref ME_Start

	<hr>

	@section ME_Init			ME init
	@code
	Im_ME_Init();													// Release software-reset

	Dd_GIC_Ctrl( E_DD_GIC_INTID_ME_INT, 1, D_DD_GIC_PRI30, 1 );		// Enable interrupt
	@endcode
	<br>
	<br>
	@section ME_Start			ME start
	@code
	T_IM_ME_CTRL	me_ctrl;
	T_IM_ME_START	me_start;
	UINT32			wait_factor;

	Im_ME_Open( 100 );

	Im_ME_Ctrl( &me_ctrl );

	Im_ME_Start( &me_start);

	Im_ME_Wait_End( &wait_factor, 1000 );

	Im_ME_Close();
	@endcode
	<br>
	<br>
@}*/ /* end of im_me_code */
