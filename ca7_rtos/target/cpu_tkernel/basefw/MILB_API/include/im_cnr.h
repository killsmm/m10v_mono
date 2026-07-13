/**
 * @file		im_cnr.h
 * @brief		Header file of JDSCNR1A driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/**
@addtogroup image
@{
	@addtogroup im_cnr
	@{
	- @ref im_cnr_sample
*/
/**	@}
@}*/

#ifndef _IM_CNR_H_
#define _IM_CNR_H_

#include "driver_common.h"

#include "ddim_typedef.h"

// get register value for RDMA
#include "_jdscnr_cspr_reg.h"
#include "_jdscnr_ofl_reg_rw.h"
#include "_jdscnr_ofl_reg_yspr.h"
#include "_jdscnr_otf_reg_rw.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/**
@weakgroup im_cnr
@{
*/
//---------------------------- driver section ----------------------------
#define	D_IM_CNR_SEM_TIMEOUT_ERR		(D_IM_CNR | D_DDIM_SEM_TIMEOUT)			/**< Time Out                 (0x14000011) */
#define	D_IM_CNR_NG						(D_IM_CNR | D_DDIM_SYSTEM_ERROR)		/**< Processing NG            (0x14000099) */
#define D_IM_CNR_SEM_NG					(D_IM_CNR | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG (0x14000010) */
#define D_IM_CNR_INPUT_PARAM_ERROR		(D_IM_CNR | D_DDIM_INPUT_PARAM_ERROR)	/**< parameter error.         (0x14000001) */
#define	D_IM_CNR_AXI_ERR				(D_IM_CNR | 0x100)						/**< AXI bus error.           (0x14000100) */
#define	D_IM_CNR_DRT_ERR				(D_IM_CNR | 0x200)						/**< DRT error.               (0x14000200) */

#define D_IM_CNR_FLG_0_OFL_SPR_END		(0x00000001)	/**< Suppre end flag ch 0 Off Line			*/
#define D_IM_CNR_FLG_1_OFL_SPR_END		(0x00000002)	/**< Suppre end flag ch 1 Off Line			*/
#define D_IM_CNR_FLG_0_OTF_SPR_END		(0x00000004)	/**< Suppre end flag ch 0 On The Fly		*/
#define D_IM_CNR_FLG_1_OTF_SPR_END		(0x00000008)	/**< Suppre end flag ch 1 On The Fly		*/
#define D_IM_CNR_FLG_0_OFL_SPR_AXI_ERR	(0x00000010)	/**< Suppre AXI error flag ch 0 Off Line	*/
#define D_IM_CNR_FLG_1_OFL_SPR_AXI_ERR	(0x00000020)	/**< Suppre AXI error flag ch 1 Off Line	*/
#define D_IM_CNR_FLG_0_OTF_SPR_AXI_ERR	(0x00000040)	/**< Suppre AXI error flag ch 0 On The Fly	*/
#define D_IM_CNR_FLG_1_OTF_SPR_AXI_ERR	(0x00000080)	/**< Suppre AXI error flag ch 1 On The Fly	*/
#define D_IM_CNR_FLG_0_OTF_SPR_DRT_ERR	(0x00000400)	/**< Suppre DRT error flag ch 0 On The Fly	*/
#define D_IM_CNR_FLG_1_OTF_SPR_DRT_ERR	(0x00000800)	/**< Suppre DRT error flag ch 1 On The Fly	*/


#define D_IM_CNR_CH_0					(0)				/**< CNR Channel 0						*/
#define D_IM_CNR_CH_1					(1)				/**< CNR Channel 1						*/
#define D_IM_CNR_CH_2					(2)				/**< CNR Channel 0/1 Simultaneous write	*/
#define D_IM_CNR_CH_READ_NUM_MAX		(2)				/**< Read Channel Max Number			*/
#define D_IM_CNR_CH_NUM_MAX				(3)				/**< Channel Max Number					*/

#define D_IM_CNR_MODE_OFL				(0)				/**< CNR Off Line mode		*/
#define D_IM_CNR_MODE_OTF				(1)				/**< CNR On The Fly mode	*/

#define D_IM_CNR_ADDR_BANK_MAX			(4)				/**< Maximum output bank index number of WCTA */

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

/** SPR YUV mode(OFL)
*/
typedef enum {
	E_IM_CNR_OFL_YCC_MODE_YCC444_PLANE = 0,		/**< YCC444 planar									*/
	E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE,		/**< YCC422 Y packed CbCr simple D_SPR_MODE_C only	*/
	E_IM_CNR_OFL_YCC_MODE_YCC422,				/**< YCC422 Y packed CbCr							*/
	E_IM_CNR_OFL_YCC_MODE_YCC420_SIMPLE,		/**< YCC420 Y packed CbCr simple D_SPR_MODE_C only	*/
	E_IM_CNR_OFL_YCC_MODE_YCC420				/**< YCC420 Y packed CbCr							*/
} E_IM_CNR_OFL_YCC_MODE;

/** SPR YUV mode(OTF)
*/
typedef enum {
	E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE = 1,	/**< YCC422 Y packed CbCr simple D_SPR_MODE_C only	*/
	E_IM_CNR_OTF_YCC_MODE_YCC420_SIMPLE = 3,	/**< YCC420 Y packed CbCr simple D_SPR_MODE_C only	*/
} E_IM_CNR_OTF_YCC_MODE;

/** SPR vertical process type(normal / devide)
*/
typedef enum {
	E_IM_CNR_VPROCESS_NORM = 0,		/**< Normal	*/
	E_IM_CNR_VPROCESS_DIV			/**< devide	*/
} E_IM_CNR_VPROCESS;

/** SPR horizontal process type(normal / devide)
*/
typedef enum {
	E_IM_CNR_HPROCESS_NORM = 0,		/**< Normal	*/
	E_IM_CNR_HPROCESS_DIV			/**< devide	*/
} E_IM_CNR_HPROCESS;

/** SPR level
*/
typedef enum {
	E_IM_CNR_LEVEL_HIGH = 0,		/**< SPR level high (default)	*/
	E_IM_CNR_LEVEL_MID,				/**< SPR level middle			*/
	E_IM_CNR_LEVEL_LOW				/**< SPR level low				*/
} E_IM_CNR_LEVEL;

/** SPR threshold type
*/
typedef enum {
	E_IM_CNR_THRES_REGISTER = 0,	/**< SPR threshold type register	*/
	E_IM_CNR_THRES_DYNAMIC			/**< SPR threshold type dynamic		*/
} E_IM_CNR_THRESHOLD;

/** SPR Whitening base
*/
typedef enum {
	E_IM_CNR_WHITEN_BASE_LOW = 0,		/**< SPR low	*/
	E_IM_CNR_WHITEN_BASE_MIDDLE,		/**< SPR middle	*/
	E_IM_CNR_WHITEN_BASE_HIGH			/**< SPR high	*/
} E_IM_CNR_WHITEN_BASE;

/** SPR C Blend mode
*/
typedef enum {
	E_IM_CNR_C_BLEND_MD_FIX = 0,		/**< Fixed value blend				*/
	E_IM_CNR_C_BLEND_MD_COL_DIFF_EDGE,	/**< Color difference edge blend	*/
	E_IM_CNR_C_BLEND_MD_HUE_SPEC,		/**< Hue specified blend			*/
} E_IM_CNR_BLEND_MD;

/** SPR C range mode
*/
typedef enum {
	E_IM_CNR_C_RANGE_MODE_0_1 = 0,		/**< SPR 0-1	*/
	E_IM_CNR_C_RANGE_MODE_0_2,			/**< SPR 0-2	*/
	E_IM_CNR_C_RANGE_MODE_0_4,			/**< SPR 0-4	*/
	E_IM_CNR_C_RANGE_MODE_0_8,			/**< SPR 0-8	*/
	E_IM_CNR_C_RANGE_MODE_0_16,			/**< SPR 0-16	*/
	E_IM_CNR_C_RANGE_MODE_0_32,			/**< SPR 0-32	*/
	E_IM_CNR_C_RANGE_MODE_0_64,			/**< SPR 0-64	*/
} E_IM_CNR_C_RANGE_MODE;

/** SPR enable / disable
*/
typedef enum {
	E_IM_CNR_ONOFF_DISABLE = 0,		/**< SPR disable	*/
	E_IM_CNR_ONOFF_ENABLE			/**< SPR enable		*/
} E_IM_CNR_ONOFF;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

typedef VOID (*T_IM_CNR_CALLBACK)( UCHAR ch, ULONG interrupt_flag_mask, UINT32 user_param );	/**< Callback function pointer	*/

/** SPR Control info for common
*/
typedef struct {
	E_IM_CNR_OFL_YCC_MODE	mode;		/**< SPR YCC mode	@@YUV_MODE	<br>
														 <ul><li>@ref E_IM_CNR_OFL_YCC_MODE_YCC444_PLANE
															 <li>@ref E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE
															 <li>@ref E_IM_CNR_OFL_YCC_MODE_YCC422
															 <li>@ref E_IM_CNR_OFL_YCC_MODE_YCC420_SIMPLE
															 <li>@ref E_IM_CNR_OFL_YCC_MODE_YCC420</ul>		*/

	USHORT				width;		/**< SPR valid data width [64 ~ 16384(aligned on 16pixel)]		@@HSIZE	*/
	USHORT				lines;		/**< SPR valid data lines [64 ~ 16384(aligned on 16pixel)]		@@VSIZE	*/

	ULONG				r_y_width;	/**< Y width for read (aligned on 16byte)			@@RYDEF	*/
	ULONG				r_y_addr;	/**< Y address for read (aligned on 16byte)			@@RYTA	*/

	ULONG				r_c_width;	/**< CbCr width for read (aligned on 16byte)		@@RCDEF	*/
	ULONG				r_cb_addr;	/**< Cb address for read (aligned on 16byte)		@@RCBTA	*/
	ULONG				r_cr_addr;	/**< Cr address for read (aligned on 16byte)		@@RCRTA	*/

	ULONG				w_y_width;	/**< Y width for write (aligned on 16byte)			@@WYDEF	*/
	ULONG				w_y_addr;	/**< Y address for write (aligned on 16byte)		@@WYTA	*/

	ULONG				w_c_width;	/**< CbCr width for write (aligned on 16byte)		@@WCDEF	*/
	ULONG				w_cb_addr;	/**< Cb address for write (aligned on 16byte)		@@WCBTA	*/
	ULONG				w_cr_addr;	/**< Cr address for write (aligned on 16byte)		@@WCRTA	*/

	ULONG				work_addr;	/**< Address fir work area[288 * VSIZE]		@@TMPTA	*/

	E_IM_CNR_VPROCESS	divide_top;	/**< Virtucal process devide	@@VDIVT	<br>
														 <ul><li>@ref E_IM_CNR_VPROCESS_NORM
															 <li>@ref E_IM_CNR_VPROCESS_DIV</ul>	*/
	E_IM_CNR_VPROCESS	divide_bot;	/**< Virtucal process devide	@@VDIVB	<br>
														 <ul><li>@ref E_IM_CNR_VPROCESS_NORM
															 <li>@ref E_IM_CNR_VPROCESS_DIV</ul>	*/

	T_IM_CNR_CALLBACK	pCallBack;	/**< callback function	*/
	UINT32				user_param;		/**< This set value will return to callback argument value when interrupt occurs.	*/
} T_IM_CNR_OFL_CTRL;

/** SPR Control info for common
*/
typedef struct {
	E_IM_CNR_OTF_YCC_MODE	mode;		/**< SPR YCC mode	@@YUV_MODE	<br>
														 <ul><li>@ref E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE
															 <li>@ref E_IM_CNR_OTF_YCC_MODE_YCC420_SIMPLE</ul>		*/

	UCHAR				extra_mode;		/**< extra_mode @@EXMODE */
	UCHAR				video_format;	/**< video_format @@CNRVFM */
	USHORT				width;			/**< SPR valid data width [64 ~ 16384(aligned on 16pixel)]		@@HSIZE	*/
	USHORT				lines;			/**< SPR valid data lines [64 ~ 16384(aligned on 16pixel)]		@@VSIZE	*/

	ULONG				w_c_width;		/**< CbCr width for write (aligned on 16byte)		@@WCDEF	*/

	ULONG				w_offset_x;		/**< write offset work X direction (aligned on 2byte)		@@CNRWOFSX	*/

	E_IM_CNR_HPROCESS	divide_left;	/**< Virtucal process devide	@@HDIVL	<br>
														 <ul><li>@ref E_IM_CNR_HPROCESS_NORM
															 <li>@ref E_IM_CNR_HPROCESS_DIV</ul>	*/
	E_IM_CNR_HPROCESS	divide_right;	/**< Virtucal process devide	@@HDIVR	<br>
														 <ul><li>@ref E_IM_CNR_HPROCESS_NORM
															 <li>@ref E_IM_CNR_HPROCESS_DIV</ul>	*/
	E_IM_CNR_ONOFF		continuous;		/**< Continuous processing enable @@CNRCSE <br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	T_IM_CNR_CALLBACK	pCallBack;		/**< callback function	*/
	UINT32				user_param;		/**< This set value will return to callback argument value when interrupt occurs.	*/
} T_IM_CNR_OTF_CTRL;

/** Im_CNR_OTF_Set_OutAddr_Info() parameter structure */
typedef struct {
	UCHAR				use_bank_num;						/**< CbCr address bank number @@WCTA
																	Use only one bank in the setting of 1 (d),
																	and use of the four banks in the setting of 4 (d).	*/
	ULONG				w_c_addr[D_IM_CNR_ADDR_BANK_MAX];	/**< CbCr address for write (aligned on 2bytes)		@@WCTA	*/
} T_IM_CNR_OTF_OUTADDR_INFO;

/** Im_CNR_OFL_Ctrl_Axi() parameter structure */
typedef struct {
	UINT32	read_cache_type;			/**< Read Cache type. */
	UINT32	read_protection_type;		/**< Read Protection type. */
	UINT32	write_cache_type;			/**< Write Cache type. */
	UINT32	write_protection_type;		/**< Write Protection type. */
} T_IM_CNR_OFL_AXI;

/** Im_CNR_OFL_Get_AXI_Status() parameter structure */
typedef struct {
	UCHAR	read_channel_response;	/**< RRESP register */
	UCHAR	write_channel_response;	/**< WRESP register */
} T_IM_CNR_OFL_AXI_STATUS;

/** Im_CNR_OTF_Ctrl_Axi() parameter structure */
typedef struct {
	UINT32	write_cache_type;			/**< Write Cache type. */
	UINT32	write_protection_type;		/**< Write Protection type. */
} T_IM_CNR_OTF_AXI;

/** Im_CNR_OTF_Get_AXI_Status() parameter structure */
typedef struct {
	UCHAR	write_channel_response;	/**< WRESP register */
} T_IM_CNR_OTF_AXI_STATUS;

/** SPR Control info for Low/Middle frequency CbCr
*/
typedef struct {
	E_IM_CNR_ONOFF			enable;				/**< Low/Middle CbCr SPR enable @@MCEN, @@LCEN	<br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	UCHAR					h_y_threshold[4];	/**< Low/Middle SPR Y threshold for horizontal		@@MCYTHH, @@LCYTHH	*/
	UCHAR					v_y_threshold[4];	/**< Low/Middle SPR Y threshold for vertical		@@MCYTHV, @@LCYTHV	*/
	UCHAR					h_c_threshold[4];	/**< Low/Middle SPR CbCr threshold for horizontal	@@MCCTHH, @@LCCTHH	*/
	UCHAR					v_c_threshold[4];	/**< Low/Middle SPR CbCr threshold for vertical	@@MCCTHV, @@LCCTHV	*/
	UCHAR					y_threshold_cor;	/**< Low/Middle SPR Y threshold coring	@@MCYDYMYCR, @@LCYDYMYCR	*/
	USHORT					y_threshold_gain;	/**< Low/Middle SPR Y threshold gain (9bit)	@@MCYDYMYG, @@LCYDYMYG	<br>
													value range: [0 - 511(=0/256 - 511/256)]	*/
	E_IM_CNR_ONOFF			y_threshold_dym_en;	/**< Low/Middle Y threshold dynamic setting enable @@MCYDYM, @@LCYDYM <br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	E_IM_CNR_ONOFF			y_threshold_scl_en;	/**< Low/Middle Y threshold scale enable @@MCYSCL, @@LCYSCL <br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	E_IM_CNR_ONOFF			c_y_threshold_scl_en;	/**< Low/Middle CbCr threshold Y scale enable @@MCSSCL, @@LCSSCL <br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	E_IM_CNR_ONOFF			c_threshold_scl_en;	/**< Low/Middle CbCr threshold scale enable @@MCCSCL, @@LCCSCL <br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
} T_IM_CNR_CTRL_C_MC_LC;

/** SPR Control info for CbCr
*/
typedef struct {
	E_IM_CNR_ONOFF			enable;				/**< CbCr SPR enable @@CSPREN	<br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	E_IM_CNR_ONOFF			gradation_keep_en;	/**< gradation keep enable @@CGDKEN <br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	T_IM_CNR_CTRL_C_MC_LC	mid_freq_nr;		/**< Middle range frequency Color NR parmeters */
	T_IM_CNR_CTRL_C_MC_LC	low_freq_nr;		/**< Low range frequency Color NR parmeters */
	E_IM_CNR_ONOFF			whitening_enable;	/**< Whitening enable @@WHTEN <br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	USHORT					whitening_value;	/**< Whitening value[0~31]			@@WHTVAL	*/
	E_IM_CNR_WHITEN_BASE	whitening_base;		/**< Whitening base @@WHTBASE <br>
														 <ul><li>@ref E_IM_CNR_WHITEN_BASE_LOW
															 <li>@ref E_IM_CNR_WHITEN_BASE_MIDDLE
															 <li>@ref E_IM_CNR_WHITEN_BASE_HIGH</ul>	*/
	E_IM_CNR_BLEND_MD		c_blend_md;			/**< Blend mode @@BLENDMD <br>
														 <ul><li>@ref E_IM_CNR_C_BLEND_MD_FIX
															 <li>@ref E_IM_CNR_C_BLEND_MD_COL_DIFF_EDGE
															 <li>@ref E_IM_CNR_C_BLEND_MD_HUE_SPEC</ul>	*/
	USHORT					c_alpha_blend;		/**< Alpha blend value[0~31] @@CSPRALPBD */
	E_IM_CNR_ONOFF			c_range;			/**< C range enable @@CSRGEN <br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	E_IM_CNR_C_RANGE_MODE	c_range_mode;		/**< C range mode @@CSRGMD <br>
														 <ul><li>@ref E_IM_CNR_C_RANGE_MODE_0_1
															 <li>@ref E_IM_CNR_C_RANGE_MODE_0_2
															 <li>@ref E_IM_CNR_C_RANGE_MODE_0_4
															 <li>@ref E_IM_CNR_C_RANGE_MODE_0_8
															 <li>@ref E_IM_CNR_C_RANGE_MODE_0_16
															 <li>@ref E_IM_CNR_C_RANGE_MODE_0_32
															 <li>@ref E_IM_CNR_C_RANGE_MODE_0_64</ul>	*/
} T_IM_CNR_CTRL_C;

/** SPR Control info for Y
*/
typedef struct {
	E_IM_CNR_ONOFF		enable;				/**< Y SPR enable @@YSPREN	<br>
														 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
															 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	E_IM_CNR_THRESHOLD	threshold_type;		/**< threshold type			@@YSPRMODE YSPRMD	<br>
														 <ul><li>@ref E_IM_CNR_THRES_REGISTER
															 <li>@ref E_IM_CNR_THRES_DYNAMIC</ul>	*/
	E_IM_CNR_LEVEL		level;				/**< SPR filter level		@@YSPRMODE YSPRFE	<br>
														 <ul><li>@ref E_IM_CNR_LEVEL_HIGH
															 <li>@ref E_IM_CNR_LEVEL_MID
															 <li>@ref E_IM_CNR_LEVEL_LOW</ul>	*/
	UCHAR				h_y_threshold;		/**< SPR Y threshold for horizontal		@@YSPRFXEYH	*/
	UCHAR				v_y_threshold;		/**< SPR Y threshold for vertical		@@YSPRFXEYV	*/
	UCHAR				h_c_threshold;		/**< SPR CbCr threshold for horizontal	@@YSPRFXECH	*/
	UCHAR				v_c_threshold;		/**< SPR CbCr threshold for vertical	@@YSPRFXECV	*/
	UCHAR				alpha_blend_ratio;	/**< Alpha blend ratio[0~31]			@@YSPRALPBD	*/
} T_IM_CNR_OFL_CTRL_Y;

/** SPR YSPRDYEY/MCYSCL/MCCSCL/MCSSCL/LCYSCL/LCCSCL/LCSSCL/CESCL table
*/
typedef struct {
	UCHAR	bd1;	/**< Boundary1	@@YSPRDYEYBD_1, @@MCYSCLBD_1, @@MCCSCLBD_1, @@MCSSCLBD_1, @@LCYSCLBD_1, @@LCCSCLBD_1, @@LCSSCLBD_1, @@CESCLBD_1 */
	UCHAR	bd2;	/**< Boundary2	@@YSPRDYEYBD_2, @@MCYSCLBD_2, @@MCCSCLBD_2, @@MCSSCLBD_2, @@LCYSCLBD_2, @@LCCSCLBD_2, @@LCSSCLBD_2, @@CESCLBD_2 */
	UCHAR	bd3;	/**< Boundary3	@@YSPRDYEYBD_3, @@MCYSCLBD_3, @@MCCSCLBD_3, @@MCSSCLBD_3, @@LCYSCLBD_3, @@LCCSCLBD_3, @@LCSSCLBD_3, @@CESCLBD_3 */
	UCHAR	bd4;	/**< Boundary4	@@YSPRDYEYBD_4, @@MCYSCLBD_4, @@MCCSCLBD_4, @@MCSSCLBD_4, @@LCYSCLBD_4, @@LCCSCLBD_4, @@LCSSCLBD_4, @@CESCLBD_4 */
	UCHAR	bd5;	/**< Boundary5	@@YSPRDYEYBD_5, @@MCYSCLBD_5, @@MCCSCLBD_5, @@MCSSCLBD_5, @@LCYSCLBD_5, @@LCCSCLBD_5, @@LCSSCLBD_5, @@CESCLBD_5 */
	UCHAR	of0;	/**< Offset0	@@YSPRDYEYOF_0, @@MCYSCLOF_0, @@MCCSCLOF_0, @@MCSSCLOF_0, @@LCYSCLOF_0, @@LCCSCLOF_0, @@LCSSCLOF_0, @@CESCLOF_0 */
	UCHAR	of1;	/**< Offset1	@@YSPRDYEYOF_1, @@MCYSCLOF_1, @@MCCSCLOF_1, @@MCSSCLOF_1, @@LCYSCLOF_1, @@LCCSCLOF_1, @@LCSSCLOF_1, @@CESCLOF_1 */
	UCHAR	of2;	/**< Offset2	@@YSPRDYEYOF_2, @@MCYSCLOF_2, @@MCCSCLOF_2, @@MCSSCLOF_2, @@LCYSCLOF_2, @@LCCSCLOF_2, @@LCSSCLOF_2, @@CESCLOF_2 */
	UCHAR	of3;	/**< Offset3	@@YSPRDYEYOF_3, @@MCYSCLOF_3, @@MCCSCLOF_3, @@MCSSCLOF_3, @@LCYSCLOF_3, @@LCCSCLOF_3, @@LCSSCLOF_3, @@CESCLOF_3 */
	UCHAR	of4;	/**< Offset4	@@YSPRDYEYOF_4, @@MCYSCLOF_4, @@MCCSCLOF_4, @@MCSSCLOF_4, @@LCYSCLOF_4, @@LCCSCLOF_4, @@LCSSCLOF_4, @@CESCLOF_4 */
	UCHAR	of5;	/**< Offset5	@@YSPRDYEYOF_5, @@MCYSCLOF_5, @@MCCSCLOF_5, @@MCSSCLOF_5, @@LCYSCLOF_5, @@LCCSCLOF_5, @@LCSSCLOF_5, @@CESCLOF_5 */
	INT8	gain0;	/**< Gain0 (8bits signed)	@@YSPRDYEYGA_0, @@MCYSCLGA_0, @@MCCSCLGA_0, @@MCSSCLGA_0, @@LCYSCLGA_0, @@LCCSCLGA_0, @@LCSSCLGA_0, @@CESCLGA_0 <br>
						value range: [-128 - 127(=-128/16 - 127/16)]	*/
	INT8	gain1;	/**< Gain1 (8bits signed)	@@YSPRDYEYGA_1, @@MCYSCLGA_1, @@MCCSCLGA_1, @@MCSSCLGA_1, @@LCYSCLGA_1, @@LCCSCLGA_1, @@LCSSCLGA_1, @@CESCLGA_1 <br>
						value range: [-128 - 127(=-128/16 - 127/16)]	*/
	INT8	gain2;	/**< Gain2 (8bits signed)	@@YSPRDYEYGA_2, @@MCYSCLGA_2, @@MCCSCLGA_2, @@MCSSCLGA_2, @@LCYSCLGA_2, @@LCCSCLGA_2, @@LCSSCLGA_2, @@CESCLGA_2 <br>
						value range: [-128 - 127(=-128/16 - 127/16)]	*/
	INT8	gain3;	/**< Gain3 (8bits signed)	@@YSPRDYEYGA_3, @@MCYSCLGA_3, @@MCCSCLGA_3, @@MCSSCLGA_3, @@LCYSCLGA_3, @@LCCSCLGA_3, @@LCSSCLGA_3, @@CESCLGA_3 <br>
						value range: [-128 - 127(=-128/16 - 127/16)]	*/
	INT8	gain4;	/**< Gain4 (8bits signed)	@@YSPRDYEYGA_4, @@MCYSCLGA_4, @@MCCSCLGA_4, @@MCSSCLGA_4, @@LCYSCLGA_4, @@LCCSCLGA_4, @@LCSSCLGA_4, @@CESCLGA_4 <br>
						value range: [-128 - 127(=-128/16 - 127/16)]	*/
	INT8	gain5;	/**< Gain5 (8bits signed)	@@YSPRDYEYGA_5, @@MCYSCLGA_5, @@MCCSCLGA_5, @@MCSSCLGA_5, @@LCYSCLGA_5, @@LCCSCLGA_5, @@LCSSCLGA_5, @@CESCLGA_5 <br>
						value range: [-128 - 127(=-128/16 - 127/16)]	*/
} T_IM_CNR_TABLE;

/** SPR SPRSCB/SPRSCR/SPRDCR/SPRDCR table
*/
typedef struct {
	INT8	bd1;	/**< Boundary1	@@SPRSCBBD_1, @@SPRSCRBD_1, @@SPRDCBBD_1, @@SPRDCRBD_1 */
	INT8	bd2;	/**< Boundary2	@@SPRSCBBD_2, @@SPRSCRBD_2, @@SPRDCBBD_2, @@SPRDCRBD_2 */
	INT8	bd3;	/**< Boundary3	@@SPRSCBBD_3, @@SPRSCRBD_3, @@SPRDCBBD_3, @@SPRDCRBD_3 */
	INT8	bd4;	/**< Boundary4	@@SPRSCBBD_4, @@SPRSCRBD_4, @@SPRDCBBD_4, @@SPRDCRBD_4 */
	INT8	bd5;	/**< Boundary5	@@SPRSCBBD_5, @@SPRSCRBD_5, @@SPRDCBBD_5, @@SPRDCRBD_5 */
	INT8	of0;	/**< Offset0	@@SPRSCBOF_0, @@SPRSCROF_0, @@SPRDCBOF_0, @@SPRDCROF_0 */
	INT8	of1;	/**< Offset1	@@SPRSCBOF_1, @@SPRSCROF_1, @@SPRDCBOF_1, @@SPRDCROF_1 */
	INT8	of2;	/**< Offset2	@@SPRSCBOF_2, @@SPRSCROF_2, @@SPRDCBOF_2, @@SPRDCROF_2 */
	INT8	of3;	/**< Offset3	@@SPRSCBOF_3, @@SPRSCROF_3, @@SPRDCBOF_3, @@SPRDCROF_3 */
	INT8	of4;	/**< Offset4	@@SPRSCBOF_4, @@SPRSCROF_4, @@SPRDCBOF_4, @@SPRDCROF_4 */
	INT8	of5;	/**< Offset5	@@SPRSCBOF_5, @@SPRSCROF_5, @@SPRDCBOF_5, @@SPRDCROF_5 */
	SHORT	gain0;	/**< Gain0 (12bits signed)	@@SPRSCBGA_0, @@SPRSCRGA_0, @@SPRDCBGA_0, @@SPRDCRGA_0<br>
						value range: [-2048 - 2047(=-2048/256 - 2047/256)]	*/
	SHORT	gain1;	/**< Gain1 (12bits signed)	@@SPRSCBGA_1, @@SPRSCRGA_1, @@SPRDCBGA_1, @@SPRDCRGA_1<br>
						value range: [-2048 - 2047(=-2048/256 - 2047/256)]	*/
	SHORT	gain2;	/**< Gain2 (12bits signed)	@@SPRSCBGA_2, @@SPRSCRGA_2, @@SPRDCBGA_2, @@SPRDCRGA_2<br>
						value range: [-2048 - 2047(=-2048/256 - 2047/256)]	*/
	SHORT	gain3;	/**< Gain3 (12bits signed)	@@SPRSCBGA_3, @@SPRSCRGA_3, @@SPRDCBGA_3, @@SPRDCRGA_3<br>
						value range: [-2048 - 2047(=-2048/256 - 2047/256)]	*/
	SHORT	gain4;	/**< Gain4 (12bits signed)	@@SPRSCBGA_4, @@SPRSCRGA_4, @@SPRDCBGA_4, @@SPRDCRGA_4<br>
						value range: [-2048 - 2047(=-2048/256 - 2047/256)]	*/
	SHORT	gain5;	/**< Gain5 (12bits signed)	@@SPRSCBGA_5, @@SPRSCRGA_5, @@SPRDCBGA_5, @@SPRDCRGA_5<br>
						value range: [-2048 - 2047(=-2048/256 - 2047/256)]	*/
} T_IM_CNR_CC_TABLE_SRC_DST;

/** SPR CPHPT/CPHPTK/CDSCLOF/CDSCLGA/CDSCLBD table
*/
typedef struct {
	INT8	cd_cb;	/**< Origin coordinate for Cb	@@CDCB */
	INT8	cd_cr;	/**< Origin coordinate for Cr	@@CDCR */
	USHORT	k_cb;	/**< Origin coordinate coefficient	@@KCB */
	USHORT	k_cr;	/**< Origin coordinate coefficient	@@KCR */
	USHORT	bd1;	/**< Boundary1	@@CDSCLBD_1 */
	USHORT	bd2;	/**< Boundary2	@@CDSCLBD_2 */
	USHORT	bd3;	/**< Boundary3	@@CDSCLBD_3 */
	USHORT	bd4;	/**< Boundary4	@@CDSCLBD_4 */
	USHORT	bd5;	/**< Boundary5	@@CDSCLBD_5 */
	USHORT	of0;	/**< Offset0	@@CDSCLOF_0 */
	USHORT	of1;	/**< Offset1	@@CDSCLOF_1 */
	USHORT	of2;	/**< Offset2	@@CDSCLOF_2 */
	USHORT	of3;	/**< Offset3	@@CDSCLOF_3 */
	USHORT	of4;	/**< Offset4	@@CDSCLOF_4 */
	USHORT	of5;	/**< Offset5	@@CDSCLOF_5 */
	SHORT	gain0;	/**< Gain0 (10bits signed)	@@CDSCLGA_0<br>
						value range: [-64 - 63(=-1024/16 - 1023/16)]	*/
	SHORT	gain1;	/**< Gain1 (10bits signed)	@@CDSCLGA_1<br>
						value range: [-64 - 63(=-1024/16 - 1023/16)]	*/
	SHORT	gain2;	/**< Gain2 (10bits signed)	@@CDSCLGA_2<br>
						value range: [-64 - 63(=-1024/16 - 1023/16)]	*/
	SHORT	gain3;	/**< Gain3 (10bits signed)	@@CDSCLGA_3<br>
						value range: [-64 - 63(=-1024/16 - 1023/16)]	*/
	SHORT	gain4;	/**< Gain4 (10bits signed)	@@CDSCLGA_4<br>
						value range: [-64 - 63(=-1024/16 - 1023/16)]	*/
	SHORT	gain5;	/**< Gain5 (10bits signed)	@@CDSCLGA_5<br>
						value range: [-64 - 63(=-1024/16 - 1023/16)]	*/
} T_IM_CNR_HUE_SPEC_TABLE;

/** SPR Control info for adjustment of Source/Destination CbCr
*/
typedef struct {
	E_IM_CNR_ONOFF					enable;		/**< Source/Destination CbCr adjustment enable @@SPRS, @@SPRD	<br>
													 <ul><li>@ref E_IM_CNR_ONOFF_DISABLE
														 <li>@ref E_IM_CNR_ONOFF_ENABLE</ul>	*/
	T_IM_CNR_CC_TABLE_SRC_DST		cb_table;	/**< Source/Destination Cb adjustment table	*/
	T_IM_CNR_CC_TABLE_SRC_DST		cr_table;	/**< Source/Destination Cr adjustment table	*/
} T_IM_CNR_CTRL_CC_SRC_DST;

/* @} */	// im_cnr

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/** Ctrl_C Table (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	CSPREN;			/**< 2842_(1000 - 1003h),	2842_(3000 - 3003h) */
	ULONG	CGDKEN;			/**< 2842_(1008 - 100Bh),	2842_(3008 - 300Bh) */
	ULONG	MCEN;			/**< 2842_(1020 - 1023h),	2842_(3020 - 3023h) */
	ULONG	MCYTHH;			/**< 2842_(1024 - 1027h),	2842_(3024 - 3027h) */
	ULONG	MCYTHV;			/**< 2842_(1028 - 102Bh),	2842_(3028 - 302Bh) */
	ULONG	MCCTHH;			/**< 2842_(102C - 102Fh),	2842_(302C - 302Fh) */
	ULONG	MCCTHV;			/**< 2842_(1030 - 1033h),	2842_(3030 - 3033h) */
	ULONG	MCYDYM;			/**< 2842_(1034 - 1037h),	2842_(3034 - 3037h) */
	ULONG	MCYDYMDT;		/**< 2842_(1038 - 103Bh),	2842_(3038 - 303Bh) */
	ULONG	MCYSCL;			/**< 2842_(1040 - 1043h),	2842_(3040 - 3043h) */
	ULONG	MCCSCL;			/**< 2842_(1060 - 1063h),	2842_(3060 - 3063h) */
	ULONG	MCSSCL;			/**< 2842_(1080 - 1083h),	2842_(3080 - 3083h) */
	ULONG	LCEN;			/**< 2842_(10C0 - 10C3h),	2842_(30C0 - 30C3h) */
	ULONG	LCYTHH;			/**< 2842_(10C4 - 10C7h),	2842_(30C4 - 30C7h) */
	ULONG	LCYTHV;			/**< 2842_(10C8 - 10CBh),	2842_(30C8 - 30CBh) */
	ULONG	LCCTHH;			/**< 2842_(10CC - 10CFh),	2842_(30CC - 30CFh) */
	ULONG	LCCTHV;			/**< 2842_(10D0 - 10D3h),	2842_(30D0 - 30D3h) */
	ULONG	LCYDYM;			/**< 2842_(10D4 - 10D7h),	2842_(30D4 - 30D7h) */
	ULONG	LCYDYMDT;		/**< 2842_(10D8 - 10DBh),	2842_(30D8 - 30DBh) */
	ULONG	LCYSCL;			/**< 2842_(10E0 - 10E3h),	2842_(30E0 - 30E3h) */
	ULONG	LCCSCL;			/**< 2842_(1100 - 1103h),	2842_(3100 - 3103h) */
	ULONG	LCSSCL;			/**< 2842_(1120 - 1123h),	2842_(3120 - 3123h) */
	ULONG	BLEND;			/**< 2842_(1160 - 1163h),	2842_(3160 - 3163h) */
	ULONG	WHTMD;			/**< 2842_(11D0 - 11D3h),	2842_(31D0 - 31D3h) */
} T_IM_CNR_RDMA_ADDR_CTRL_C;
/** Ctrl_C Table (OTF/OFL)
data
*/
typedef struct {
	//												OTF							OFL
	union	io_cnr_cspr_reg_cspren		CSPREN;		/**< 2842_(1000 - 1003h),	2842_(3000 - 3003h) */
	union	io_cnr_cspr_reg_cgdken		CGDKEN;		/**< 2842_(1008 - 100Bh),	2842_(3008 - 300Bh) */
	union	io_cnr_cspr_reg_mcen		MCEN;		/**< 2842_(1020 - 1023h),	2842_(3020 - 3023h) */
	union	io_cnr_cspr_reg_mcythh		MCYTHH;		/**< 2842_(1024 - 1027h),	2842_(3024 - 3027h) */
	union	io_cnr_cspr_reg_mcythv		MCYTHV;		/**< 2842_(1028 - 102Bh),	2842_(3028 - 302Bh) */
	union	io_cnr_cspr_reg_mccthh		MCCTHH;		/**< 2842_(102C - 102Fh),	2842_(302C - 302Fh) */
	union	io_cnr_cspr_reg_mccthv		MCCTHV;		/**< 2842_(1030 - 1033h),	2842_(3030 - 3033h) */
	union	io_cnr_cspr_reg_mcydym		MCYDYM;		/**< 2842_(1034 - 1037h),	2842_(3034 - 3037h) */
	union	io_cnr_cspr_reg_mcydymdt	MCYDYMDT;	/**< 2842_(1038 - 103Bh),	2842_(3038 - 303Bh) */
	union	io_cnr_cspr_reg_mcyscl		MCYSCL;		/**< 2842_(1040 - 1043h),	2842_(3040 - 3043h) */
	union	io_cnr_cspr_reg_mccscl		MCCSCL;		/**< 2842_(1060 - 1063h),	2842_(3060 - 3063h) */
	union	io_cnr_cspr_reg_mcsscl		MCSSCL;		/**< 2842_(1080 - 1083h),	2842_(3080 - 3083h) */
	union	io_cnr_cspr_reg_lcen		LCEN;		/**< 2842_(10C0 - 10C3h),	2842_(30C0 - 30C3h) */
	union	io_cnr_cspr_reg_lcythh		LCYTHH;		/**< 2842_(10C4 - 10C7h),	2842_(30C4 - 30C7h) */
	union	io_cnr_cspr_reg_lcythv		LCYTHV;		/**< 2842_(10C8 - 10CBh),	2842_(30C8 - 30CBh) */
	union	io_cnr_cspr_reg_lccthh		LCCTHH;		/**< 2842_(10CC - 10CFh),	2842_(30CC - 30CFh) */
	union	io_cnr_cspr_reg_lccthv		LCCTHV;		/**< 2842_(10D0 - 10D3h),	2842_(30D0 - 30D3h) */
	union	io_cnr_cspr_reg_lcydym		LCYDYM;		/**< 2842_(10D4 - 10D7h),	2842_(30D4 - 30D7h) */
	union	io_cnr_cspr_reg_lcydymdt	LCYDYMDT;	/**< 2842_(10D8 - 10DBh),	2842_(30D8 - 30DBh) */
	union	io_cnr_cspr_reg_lcyscl		LCYSCL;		/**< 2842_(10E0 - 10E3h),	2842_(30E0 - 30E3h) */
	union	io_cnr_cspr_reg_lccscl		LCCSCL;		/**< 2842_(1100 - 1103h),	2842_(3100 - 3103h) */
	union	io_cnr_cspr_reg_lcsscl		LCSSCL;		/**< 2842_(1120 - 1123h),	2842_(3120 - 3123h) */
	union	io_cnr_cspr_reg_blend 		BLEND;		/**< 2842_(1160 - 1163h),	2842_(3160 - 3163h) */
	union	io_cnr_cspr_reg_whtmd		WHTMD;		/**< 2842_(11D0 - 11D3h),	2842_(31D0 - 31D3h) */
} T_IM_CNR_RDMA_DATA_CTRL_C;


/** OFL CTRL Y (OFL)
address
*/
typedef struct {
	ULONG	YSPREN;			/**< 2842_(4000 - 4003h) */
	ULONG	YSPRMODE;		/**< 2842_(4004 - 4007h) */
	ULONG	YSPRFXEY;		/**< 2842_(4020 - 4023h) */
	ULONG	YSPRFXEC;		/**< 2842_(4024 - 4027h) */
	ULONG	YSPRALPBD;		/**< 2842_(4028 - 402Bh) */
} T_IM_CNR_RDMA_ADDR_OFL_CTRL_Y;
/** OFL CTRL Y (OFL)
data
*/
typedef struct {
	union	io_cnr_ofl_reg_yspr_yspren		YSPREN;		/**< 2842_(4000 - 4003h) */
	union	io_cnr_ofl_reg_yspr_ysprmode	YSPRMODE;	/**< 2842_(4004 - 4007h) */
	union	io_cnr_ofl_reg_yspr_ysprfxey	YSPRFXEY;	/**< 2842_(4020 - 4023h) */
	union	io_cnr_ofl_reg_yspr_ysprfxec	YSPRFXEC;	/**< 2842_(4024 - 4027h) */
	union	io_cnr_ofl_reg_yspr_yspralpbd	YSPRALPBD;	/**< 2842_(4028 - 402Bh) */
} T_IM_CNR_RDMA_DATA_OFL_CTRL_Y;


/** OFL Y YSPR Table (OFL)
address
*/
typedef struct {
	ULONG	YSPRDYEYOF_1;	/**< 2842_(4030 - 4033h) */
	ULONG	YSPRDYEYOF_2;	/**< 2842_(4034 - 4037h) */
	ULONG	YSPRDYEYGA_1;	/**< 2842_(4038 - 403Bh) */
	ULONG	YSPRDYEYGA_2;	/**< 2842_(403C - 403Fh) */
	ULONG	YSPRDYEYBD_1;	/**< 2842_(4040 - 4043h) */
	ULONG	YSPRDYEYBD_2;	/**< 2842_(4044 - 4047h) */
} T_IM_CNR_RDMA_ADDR_OFL_YSPR_TBL;
/** OFL Y YSPR Table (OFL)
data
*/
typedef struct {
	struct	io_cnr_ofl_reg_yspr_ysprdyeyof	YSPRDYEYOF;	/**< 2842_(4030 - 4037h) */
	struct	io_cnr_ofl_reg_yspr_ysprdyeyga	YSPRDYEYGA;	/**< 2842_(4038 - 403Fh) */
	struct	io_cnr_ofl_reg_yspr_ysprdyeybd	YSPRDYEYBD;	/**< 2842_(4040 - 4047h) */
} T_IM_CNR_RDMA_DATA_OFL_YSPR_TBL;


/** CSPR Middle Y Table (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	MCYSCLOF_1;		/**< 2842_(1048 - 104Bh),	2842_(3048 - 304Bh) */
	ULONG	MCYSCLOF_2;		/**< 2842_(104C - 104Fh),	2842_(304C - 304Fh) */
	ULONG	MCYSCLGA_1;		/**< 2842_(1050 - 1053h),	2842_(3050 - 3053h) */
	ULONG	MCYSCLGA_2;		/**< 2842_(1054 - 1057h),	2842_(3054 - 3057h) */
	ULONG	MCYSCLBD_1;		/**< 2842_(1058 - 105Bh),	2842_(3058 - 305Bh) */
	ULONG	MCYSCLBD_2;		/**< 2842_(105C - 105Fh),	2842_(305C - 305Fh) */
} T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL;
/** CSPR Middle Y Table (OTF/OFL)
data
*/
typedef struct {
	//											OTF							OFL
	struct io_cnr_cspr_reg_mcysclof	MCYSCLOF;	/**< 2842_(1048 - 104Fh),	2842_(3048 - 304Fh) */
	struct io_cnr_cspr_reg_mcysclga	MCYSCLGA;	/**< 2842_(1050 - 1057h),	2842_(3050 - 3057h) */
	struct io_cnr_cspr_reg_mcysclbd	MCYSCLBD;	/**< 2842_(1058 - 105Fh),	2842_(3058 - 305Fh) */
} T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL;


/** CSPR Middle CC Y Table (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	MCCSCLOF_1;		/**< 2842_(1068 - 106Bh),	2842_(3068 - 306Bh) */
	ULONG	MCCSCLOF_2;		/**< 2842_(106C - 106Fh),	2842_(306C - 306Fh) */
	ULONG	MCCSCLGA_1;		/**< 2842_(1070 - 1073h),	2842_(3070 - 3073h) */
	ULONG	MCCSCLGA_2;		/**< 2842_(1074 - 1077h),	2842_(3074 - 3077h) */
	ULONG	MCCSCLBD_1;		/**< 2842_(1078 - 107Bh),	2842_(3078 - 307Bh) */
	ULONG	MCCSCLBD_2;		/**< 2842_(107C - 107Fh),	2842_(307C - 307Fh) */
} T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL;
/** CSPR Middle CC Y Table (OTF/OFL)
data
*/
typedef struct {
	//											OTF							OFL
	struct io_cnr_cspr_reg_mccsclof	MCCSCLOF;	/**< 2842_(1068 - 106Fh),	2842_(3068 - 306Fh) */
	struct io_cnr_cspr_reg_mccsclga	MCCSCLGA;	/**< 2842_(1070 - 1077h),	2842_(3070 - 3077h) */
	struct io_cnr_cspr_reg_mccsclbd	MCCSCLBD;	/**< 2842_(1078 - 107Fh),	2842_(3078 - 307Fh) */
} T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL;


/** CSPR Middle CC C Table (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	MCSSCLOF_1;		/**< 2842_(1088 - 108Bh),	2842_(3088 - 308Bh) */
	ULONG	MCSSCLOF_2;		/**< 2842_(108C - 108Fh),	2842_(308C - 308Fh) */
	ULONG	MCSSCLGA_1;		/**< 2842_(1090 - 1093h),	2842_(3090 - 3093h) */
	ULONG	MCSSCLGA_2;		/**< 2842_(1094 - 1097h),	2842_(3094 - 3097h) */
	ULONG	MCSSCLBD_1;		/**< 2842_(1098 - 109Bh),	2842_(3098 - 309Bh) */
	ULONG	MCSSCLBD_2;		/**< 2842_(109C - 109Fh),	2842_(309C - 309Fh) */
} T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL;
/** CSPR Middle CC C Table (OTF/OFL)
data
*/
typedef struct {
	//											OTF							OFL
	struct io_cnr_cspr_reg_mcssclof	MCSSCLOF;	/**< 2842_(1088 - 108Fh),	2842_(3088 - 308Fh) */
	struct io_cnr_cspr_reg_mcssclga	MCSSCLGA;	/**< 2842_(1090 - 1097h),	2842_(3090 - 3097h) */
	struct io_cnr_cspr_reg_mcssclbd	MCSSCLBD;	/**< 2842_(1098 - 109Fh),	2842_(3098 - 309Fh) */
} T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL;


/** CSPR Low Y Table (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	LCYSCLOF_1;		/**< 2842_(10E8 - 10EBh),	2842_(30E8 - 30EBh) */
	ULONG	LCYSCLOF_2;		/**< 2842_(10EC - 10EFh),	2842_(30EC - 30EFh) */
	ULONG	LCYSCLGA_1;		/**< 2842_(10F0 - 10F3h),	2842_(30F0 - 30F3h) */
	ULONG	LCYSCLGA_2;		/**< 2842_(10F4 - 10F7h),	2842_(30F4 - 30F7h) */
	ULONG	LCYSCLBD_1;		/**< 2842_(10F8 - 10F8h),	2842_(30F8 - 30F8h) */
	ULONG	LCYSCLBD_2;		/**< 2842_(10FC - 10FFh),	2842_(30FC - 30FFh) */
} T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL;
/** CSPR Low Y Table (OTF/OFL)
data
*/
typedef struct {
	//											OTF							OFL
	struct io_cnr_cspr_reg_lcysclof	LCYSCLOF;	/**< 2842_(10E8 - 10EFh),	2842_(30E8 - 30EFh) */
	struct io_cnr_cspr_reg_lcysclga	LCYSCLGA;	/**< 2842_(10F0 - 10F7h),	2842_(30F0 - 30F7h) */
	struct io_cnr_cspr_reg_lcysclbd	LCYSCLBD;	/**< 2842_(10F8 - 10FFh),	2842_(30F8 - 30FFh) */
} T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL;


/** CSPR Low CC Y Table (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	LCCSCLOF_1;		/**< 2842_(1108 - 110Bh),	2842_(3108 - 310Bh) */
	ULONG	LCCSCLOF_2;		/**< 2842_(110C - 110Fh),	2842_(310C - 310Fh) */
	ULONG	LCCSCLGA_1;		/**< 2842_(1110 - 1113h),	2842_(3110 - 3113h) */
	ULONG	LCCSCLGA_2;		/**< 2842_(1114 - 1117h),	2842_(3114 - 3117h) */
	ULONG	LCCSCLBD_1;		/**< 2842_(1118 - 1118h),	2842_(3118 - 3118h) */
	ULONG	LCCSCLBD_2;		/**< 2842_(111C - 111Fh),	2842_(311C - 311Fh) */
} T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL;
/** CSPR Low CC Y Table (OTF/OFL)
data
*/
typedef struct {
	//											OTF							OFL
	struct io_cnr_cspr_reg_lccsclof	LCCSCLOF;	/**< 2842_(1108 - 110Fh),	2842_(3108 - 310Fh) */
	struct io_cnr_cspr_reg_lccsclga	LCCSCLGA;	/**< 2842_(1110 - 1117h),	2842_(3110 - 3117h) */
	struct io_cnr_cspr_reg_lccsclbd	LCCSCLBD;	/**< 2842_(1118 - 111Fh),	2842_(3118 - 311Fh) */
} T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL;


/** CSPR Low CC C Table (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	LCSSCLOF_1;		/**< 2842_(1128 - 112Bh),	2842_(3128 - 312Bh) */
	ULONG	LCSSCLOF_2;		/**< 2842_(112C - 112Fh),	2842_(312C - 312Fh) */
	ULONG	LCSSCLGA_1;		/**< 2842_(1130 - 1133h),	2842_(3130 - 3133h) */
	ULONG	LCSSCLGA_2;		/**< 2842_(1134 - 1137h),	2842_(3134 - 3137h) */
	ULONG	LCSSCLBD_1;		/**< 2842_(1138 - 1138h),	2842_(3138 - 3138h) */
	ULONG	LCSSCLBD_2;		/**< 2842_(113C - 113Fh),	2842_(313C - 313Fh) */
} T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL;
/** CSPR Low CC C Table (OTF/OFL)
data
*/
typedef struct {
	//											OTF							OFL
	struct io_cnr_cspr_reg_lcssclof	LCSSCLOF;	/**< 2842_(1128 - 112Fh),	2842_(3128 - 312Fh) */
	struct io_cnr_cspr_reg_lcssclga	LCSSCLGA;	/**< 2842_(1130 - 1137h),	2842_(3130 - 3137h) */
	struct io_cnr_cspr_reg_lcssclbd	LCSSCLBD;	/**< 2842_(1138 - 113Fh),	2842_(3138 - 313Fh) */
} T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL;


/** CSPR Edge Table (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	CESCLOF_1;		/**< 2842_(1168 - 116Bh),	2842_(3168 - 316Bh) */
	ULONG	CESCLOF_2;		/**< 2842_(116C - 116Fh),	2842_(316C - 316Fh) */
	ULONG	CESCLGA_1;		/**< 2842_(1170 - 1173h),	2842_(3170 - 3173h) */
	ULONG	CESCLGA_2;		/**< 2842_(1174 - 1177h),	2842_(3174 - 3177h) */
	ULONG	CESCLBD_1;		/**< 2842_(1178 - 1178h),	2842_(3178 - 3178h) */
	ULONG	CESCLBD_2;		/**< 2842_(117C - 117Fh),	2842_(317C - 317Fh) */
} T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL;
/** CSPR Edge Table (OTF/OFL)
data
*/
typedef struct {
	//											OTF							OFL
	struct io_cnr_cspr_reg_cesclof	CESCLOF;	/**< 2842_(1168 - 116Fh),	2842_(3168 - 316Fh) */
	struct io_cnr_cspr_reg_cesclga	CESCLGA;	/**< 2842_(1170 - 1177h),	2842_(3170 - 3177h) */
	struct io_cnr_cspr_reg_cesclbd	CESCLBD;	/**< 2842_(1178 - 117Fh),	2842_(3178 - 317Fh) */
} T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL;


/** CSPR_Hue_Specified (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	CPHPT;			/**< 2842_(1190 - 1193h),	2842_(3190 - 3193h) */
	ULONG	CPHPTK;			/**< 2842_(1194 - 1197h),	2842_(3194 - 3197h) */
	ULONG	CDSCLOF_1;		/**< 2842_(11A0 - 11A3h),	2842_(31A0 - 31A3h) */
	ULONG	CDSCLOF_2;		/**< 2842_(11A4 - 11A7h),	2842_(31A4 - 31A7h) */
	ULONG	CDSCLOF_3;		/**< 2842_(11A8 - 11ABh),	2842_(31A8 - 31ABh) */
	ULONG	CDSCLGA_1;		/**< 2842_(11B0 - 11B3h),	2842_(31B0 - 31B3h) */
	ULONG	CDSCLGA_2;		/**< 2842_(11B4 - 11B7h),	2842_(31B4 - 31B7h) */
	ULONG	CDSCLGA_3;		/**< 2842_(11B8 - 11BBh),	2842_(31B8 - 31BBh) */
	ULONG	CDSCLBD_1;		/**< 2842_(11C0 - 11C3h),	2842_(31C0 - 31C3h) */
	ULONG	CDSCLBD_2;		/**< 2842_(11C4 - 11C7h),	2842_(31C4 - 31C7h) */
	ULONG	CDSCLBD_3;		/**< 2842_(11C8 - 11CBh),	2842_(31C8 - 31CBh) */
} T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL;
/** CSPR_Hue_Specified (OTF/OFL)
data
*/
typedef struct {
	//												OTF							OFL
	union	io_cnr_cspr_reg_cphpt		CPHPT;		/**< 2842_(1190 - 1193h),	2842_(3190 - 3193h) */
	union	io_cnr_cspr_reg_cphptk		CPHPTK;		/**< 2842_(1194 - 1197h),	2842_(3194 - 3197h) */
	struct	io_cnr_cspr_reg_cdsclof		CDSCLOF;	/**< 2842_(11A0 - 11ABh),	2842_(31A0 - 31ABh) */
	struct	io_cnr_cspr_reg_cdsclga		CDSCLGA;	/**< 2842_(11B0 - 11BBh),	2842_(31B0 - 31BBh) */
	struct	io_cnr_cspr_reg_cdsclbd		CDSCLBD;	/**< 2842_(11C0 - 11CBh),	2842_(31C0 - 31CBh) */
} T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL;


/** SPRS (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	SPRS;			/**< 2842_(1200 - 1203h),	2842_(3200 - 3203h) */
	ULONG	SPRSCBOF_1;		/**< 2842_(1208 - 120Bh),	2842_(3208 - 320Bh) */
	ULONG	SPRSCBOF_2;		/**< 2842_(120C - 120Fh),	2842_(320C - 320Fh) */
	ULONG	SPRSCBGA_1;		/**< 2842_(1210 - 1213h),	2842_(3210 - 3213h) */
	ULONG	SPRSCBGA_2;		/**< 2842_(1214 - 1217h),	2842_(3214 - 3217h) */
	ULONG	SPRSCBGA_3;		/**< 2842_(1218 - 121Bh),	2842_(3218 - 321Bh) */
	ULONG	SPRSCBBD_1;		/**< 2842_(1220 - 1223h),	2842_(3220 - 3223h) */
	ULONG	SPRSCBBD_2;		/**< 2842_(1224 - 1227h),	2842_(3224 - 3227h) */
	ULONG	SPRSCROF_1;		/**< 2842_(1228 - 122Bh),	2842_(3228 - 322Bh) */
	ULONG	SPRSCROF_2;		/**< 2842_(122C - 122Fh),	2842_(322C - 322Fh) */
	ULONG	SPRSCRGA_1;		/**< 2842_(1230 - 1233h),	2842_(3230 - 3233h) */
	ULONG	SPRSCRGA_2;		/**< 2842_(1234 - 1237h),	2842_(3234 - 3237h) */
	ULONG	SPRSCRGA_3;		/**< 2842_(1238 - 123Bh),	2842_(3238 - 323Bh) */
	ULONG	SPRSCRBD_1;		/**< 2842_(1240 - 1243h),	2842_(3240 - 3243h) */
	ULONG	SPRSCRBD_2;		/**< 2842_(1244 - 1247h),	2842_(3244 - 3247h) */
} T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL;
/** SPRS (OTF/OFL)
data
*/
typedef struct {
	//												OTF							OFL
	union	io_cnr_cspr_reg_sprs		SPRS;		/**< 2842_(1200 - 1203h),	2842_(3200 - 3203h) */
	struct	io_cnr_cspr_reg_sprscbof	SPRSCBOF;	/**< 2842_(1208 - 120Fh),	2842_(3208 - 320Fh) */
	struct	io_cnr_cspr_reg_sprscbga	SPRSCBGA;	/**< 2842_(1210 - 121Bh),	2842_(3210 - 321Bh) */
	struct	io_cnr_cspr_reg_sprscbbd	SPRSCBBD;	/**< 2842_(1220 - 1227h),	2842_(3220 - 3227h) */
	struct	io_cnr_cspr_reg_sprscrof	SPRSCROF;	/**< 2842_(1228 - 122Fh),	2842_(3228 - 322Fh) */
	struct	io_cnr_cspr_reg_sprscrga	SPRSCRGA;	/**< 2842_(1230 - 123Bh),	2842_(3230 - 323Bh) */
	struct	io_cnr_cspr_reg_sprscrbd	SPRSCRBD;	/**< 2842_(1240 - 1247h),	2842_(3240 - 3247h) */
} T_IM_CNR_RDMA_DATA_SPRS_CC_TBL;


/** SPRD (OTF/OFL)
address
*/
typedef struct {
	//						OTF							OFL
	ULONG	SPRD;			/**< 2842_(1280 - 1283h),	2842_(3280 - 3283h) */
	ULONG	SPRDCBOF_1;		/**< 2842_(1288 - 128Bh),	2842_(3288 - 328Bh) */
	ULONG	SPRDCBOF_2;		/**< 2842_(128C - 128Fh),	2842_(328C - 328Fh) */
	ULONG	SPRDCBGA_1;		/**< 2842_(1290 - 1293h),	2842_(3290 - 3293h) */
	ULONG	SPRDCBGA_2;		/**< 2842_(1294 - 1297h),	2842_(3294 - 3297h) */
	ULONG	SPRDCBGA_3;		/**< 2842_(1298 - 129Bh),	2842_(3298 - 329Bh) */
	ULONG	SPRDCBBD_1;		/**< 2842_(12A0 - 12A3h),	2842_(32A0 - 32A3h) */
	ULONG	SPRDCBBD_2;		/**< 2842_(12A4 - 12A7h),	2842_(32A4 - 32A7h) */
	ULONG	SPRDCROF_1;		/**< 2842_(12A8 - 122Ah),	2842_(32A8 - 322Ah) */
	ULONG	SPRDCROF_2;		/**< 2842_(12AC - 122Ah),	2842_(32AC - 322Ah) */
	ULONG	SPRDCRGA_1;		/**< 2842_(12B0 - 12B3h),	2842_(32B0 - 32B3h) */
	ULONG	SPRDCRGA_2;		/**< 2842_(12B4 - 12B7h),	2842_(32B4 - 32B7h) */
	ULONG	SPRDCRGA_3;		/**< 2842_(12B8 - 12BBh),	2842_(32B8 - 32BBh) */
	ULONG	SPRDCRBD_1;		/**< 2842_(12C0 - 12C3h),	2842_(32C0 - 32C3h) */
	ULONG	SPRDCRBD_2;		/**< 2842_(12C4 - 12C7h),	2842_(32C4 - 32C7h) */
} T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL;
/** SPRD (OTF/OFL)
data
*/
typedef struct {
	//												OTF							OFL
	union	io_cnr_cspr_reg_sprd		SPRD;		/**< 2842_(1280 - 1283h),	2842_(3280 - 3283h) */
	struct	io_cnr_cspr_reg_sprdcbof	SPRDCBOF;	/**< 2842_(1288 - 128Fh),	2842_(3288 - 328Fh) */
	struct	io_cnr_cspr_reg_sprdcbga	SPRDCBGA;	/**< 2842_(1290 - 129Bh),	2842_(3290 - 329Bh) */
	struct	io_cnr_cspr_reg_sprdcbbd	SPRDCBBD;	/**< 2842_(12A0 - 12A7h),	2842_(32A0 - 32A7h) */
	struct	io_cnr_cspr_reg_sprdcrof	SPRDCROF;	/**< 2842_(12A8 - 12AFh),	2842_(32A8 - 32AFh) */
	struct	io_cnr_cspr_reg_sprdcrga	SPRDCRGA;	/**< 2842_(12B0 - 12BBh),	2842_(32B0 - 32BBh) */
	struct	io_cnr_cspr_reg_sprdcrbd	SPRDCRBD;	/**< 2842_(12C0 - 12C7h),	2842_(32C0 - 32C7h) */
} T_IM_CNR_RDMA_DATA_SPRD_CC_TBL;
#endif

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
//---------------------------- driver section ----------------------------

/**
@weakgroup im_cnr
@{
*/
/**
PCLK ON
@param [in]	ch				channel[0 - 1]
@remarks		For debugging use only (for register dump on debugger)
*/
extern	VOID Im_CNR_On_Pclk( UCHAR ch );

/**
PCLK OFF
@param [in]	ch				channel[0 - 1]
@remarks		For debugging use only (for register dump on debugger)
*/
extern	VOID Im_CNR_Off_Pclk( UCHAR ch );

/**
The semaphore of SPR is acquired for Off Line
@param [in]	ch				channel[0 - 1]
@param[in]	tmout		Time of timeout<br>
						<ul><li>Positive Value(Time of timeout)
							<li>@ref D_DDIM_USER_SEM_WAIT_POL
							<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_CNR_SEM_NG					Lock Error (System Using designated channel number)
@retval		D_IM_CNR_TIMEOUT				Input Parameter Error
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32 Im_CNR_OFL_Open( UCHAR ch, const INT32 tmout );

/**
Set software reset and operating mode for Off Line
@param [in]	ch				channel[0 - 2]
@retval		D_DDIM_OK						Success
*/
extern	INT32 Im_CNR_OFL_Init( UCHAR ch );

/**
Suppre Control for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_ctrl
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the Suppre is set.
			Please call Y, C or both setting after this function if necessary.
*/
extern	INT32 Im_CNR_OFL_Ctrl( UCHAR ch, const T_IM_CNR_OFL_CTRL* const cnr_ctrl );

/**
Suppre Control set AXI bus I/F Control for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	axi_ctrl	AXI bus I/F control parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_OFL_Ctrl_Axi( UCHAR ch, const T_IM_CNR_OFL_AXI* const axi_ctrl );

/**
Get AXI bus I/F Status for Off Line
@param [in]	ch				channel[0 - 1]
@param[out]	sts	AXI bus I/F status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_OFL_Get_AXI_Status( UCHAR ch, T_IM_CNR_OFL_AXI_STATUS* const sts );

/**
Set SPR setting for Y for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_ctrl_y	Y suppre parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the Suppre for Y is set.
*/
extern	INT32 Im_CNR_OFL_Ctrl_Y( UCHAR ch, const T_IM_CNR_OFL_CTRL_Y* const cnr_ctrl_y );

/**
Set SPR table for Y for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_yspr	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre YSPR table for Y is set.
*/
extern	INT32 Im_CNR_OFL_Set_YSPR_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_yspr );

/**
SPR Start Sync for Off Line
@param [in]	ch				channel[0 - 1]
@param[in]	wait_time			Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval		D_DDIM_OK : success.
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_CNR_AXI_ERR				Fail - AXI bus error.
@retval		D_IM_CNR_TIMEOUT				Fail - Processing NG. (Time outed)
@retval		D_IM_CNR_NG						Fail - Process NG. (system error)
@remarks	SPR function is started(enable) Sync.
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Im_CNR_OFL_Start_Sync( UCHAR ch, const INT32 wait_time );

/**
SPR Start Async for Off Line
@param [in]	ch				channel[0 - 1]
@retval		D_DDIM_OK						Success
@remarks	SPR function is started(enable) Async.
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Im_CNR_OFL_Start_Async( UCHAR ch );

/**
SPR Wait End for Off Line
@param [in]	ch				channel[0 - 1]
@param[out]	p_wait_factor	: Pointer of factor bitfield parameter which release wait process.<br>
								  <ul>
										 <li>@ref D_IM_CNR_FLG_0_OFL_SPR_END
										 <li>@ref D_IM_CNR_FLG_1_OFL_SPR_AXI_ERR
								  </ul>
@param[in]	wait_time			Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval		D_DDIM_OK						Success.
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_CNR_AXI_ERR				Fail - AXI bus error.
@retval		D_IM_CNR_TIMEOUT				Fail - Processing NG.(Time outed)
@retval		D_IM_CNR_SEM_NG					Fail - Process NG. (Event flag error)
@remarks	Wait for SPR function is ended.
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern	INT32 Im_CNR_OFL_Wait_End( UCHAR ch, UINT32* const p_wait_factor, const INT32 wait_time );

/**
SPR Stop for Off Line
@param [in]	ch				channel[0 - 2]
@retval		D_DDIM_OK						Success.
@remarks	SPR function is stopped(disable).
*/
extern	INT32 Im_CNR_OFL_Stop( UCHAR ch );

/**
The semaphore of SPR is returned for Off Line
@param [in]	ch				channel[0 - 1]
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_SEM_NG					Fail - Processing NG (system error)
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern	INT32 Im_CNR_OFL_Close( UCHAR ch );

/**
SPR Reset for Off Line
@param [in]	ch				channel[0 - 2]
@retval		D_DDIM_OK						Success
@remarks	SPR function soft reset
*/
extern	INT32 Im_CNR_OFL_Reset( UCHAR ch );

/**
SPR Macro Interrupt handler for ch0
@remarks This API uses DDIM_User_Set_Flg().
*/
extern	VOID Im_CNR0_Int_Handler( VOID );

/**
SPR Macro Interrupt handler for ch1
@remarks This API uses DDIM_User_Set_Flg().
*/
extern	VOID Im_CNR1_Int_Handler( VOID );

/**
Set SPR Y table for Middle frequency CbCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_mid_y	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Y table for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Set_CSPR_Mid_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_mid_y );

/**
Set SPR Y table for Middle frequency CbCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_mid_c	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Y table for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Set_CSPR_Mid_CC_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_mid_c );

/**
Set SPR CbCr table for Middle frequency CbCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_mid_c	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR CbCr table for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Set_CSPR_Mid_CC_C_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_mid_c );

/**
Set SPR Y table for Low frequency CbCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_low_y	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Y table for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Set_CSPR_Low_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_low_y );

/**
Set SPR Y table for Low frequency CbCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_low_c	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Y table for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Set_CSPR_Low_CC_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_low_c );

/**
Set SPR CbCr table for Low frequency CbCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_low_c	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR CbCr table for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Set_CSPR_Low_CC_C_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_low_c );

/**
Set SPR setting for CbCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_ctrl_c	CbCr suppre parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the Suppre for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Ctrl_C( UCHAR ch, const T_IM_CNR_CTRL_C* const cnr_ctrl_c );

/**
Set SPR Edge for CbcCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_edge	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Edge table for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Set_CSPR_Edge_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_edge );

/**
Set SPR Hue specified for CbcCr for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	spr_cspr_hue	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Hue specified table for CbCr is set.
*/
extern	INT32 Im_CNR_OFL_Set_CSPR_Hue_Specified_Table( UCHAR ch, const T_IM_CNR_HUE_SPEC_TABLE* const spr_cspr_hue );

/**
Set CbCr setting of source for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	sprs_ctrl	Source CbCr adjustment parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the source CbCr Adjustment is set.
*/
extern	INT32 Im_CNR_OFL_Ctrl_SPRS_CC( UCHAR ch, const T_IM_CNR_CTRL_CC_SRC_DST* const sprs_ctrl );

/**
Set CbCr setting of destination for Off Line
@param [in]	ch				channel[0 - 2]
@param[in]	sprd_ctrl	Destination CbCr adjustment parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the destination CbCr Adjustment is set.
*/
extern	INT32 Im_CNR_OFL_Ctrl_SPRD_CC( UCHAR ch, const T_IM_CNR_CTRL_CC_SRC_DST* const sprd_ctrl );



/**
The semaphore of SPR is acquired for On The Fly
@param [in]	ch				channel[0 - 1]
@param[in]	tmout		Time of timeout<br>
						<ul><li>Positive Value(Time of timeout)
							<li>@ref D_DDIM_USER_SEM_WAIT_POL
							<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_CNR_SEM_NG					Lock Error (System Using designated channel number)
@retval		D_IM_CNR_TIMEOUT				Input Parameter Error
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32 Im_CNR_OTF_Open( UCHAR ch, const INT32 tmout );

/**
Set software reset and operating mode for On The Fly
@param [in]	ch				channel[0 - 2]
@retval		D_DDIM_OK						Success
*/
extern	INT32 Im_CNR_OTF_Init( UCHAR ch );

/**
Suppre Control for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_ctrl
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the Suppre is set.
			Please call Y, C or both setting after this function if necessary.
*/
extern	INT32 Im_CNR_OTF_Ctrl( UCHAR ch, const T_IM_CNR_OTF_CTRL* const cnr_ctrl );

/**
Set Suppre output address information for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	addr_info
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_OTF_Set_OutAddr_Info( UCHAR ch, const T_IM_CNR_OTF_OUTADDR_INFO* const addr_info );

/**
Increment Suppre output address index for On The Fly
@param [in]	ch				channel[0 - 1]
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_OTF_Increment_OutAddr_Index( UCHAR ch );

/**
Suppre Control set AXI bus I/F Control for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	axi_ctrl	AXI bus I/F control parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_OTF_Ctrl_Axi( UCHAR ch, const T_IM_CNR_OTF_AXI* const axi_ctrl );

/**
Get AXI bus I/F Status for On The Fly
@param [in]	ch				channel[0 - 1]
@param[out]	sts	AXI bus I/F status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_OTF_Get_AXI_Status( UCHAR ch, T_IM_CNR_OTF_AXI_STATUS* const sts );

/**
SPR Start Sync for On The Fly
@param [in]	ch				channel[0 - 1]
@param[in]	wait_time			Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval		D_DDIM_OK : success.
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_CNR_AXI_ERR				Fail - AXI bus error.
@retval		D_IM_CNR_TIMEOUT				Fail - Processing NG. (Time outed)
@retval		D_IM_CNR_NG						Fail - Process NG. (system error)
@remarks	SPR function is started(enable) Sync.
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Im_CNR_OTF_Start_Sync( UCHAR ch, const INT32 wait_time );

/**
SPR Start Async for On The Fly
@param [in]	ch				channel[0 - 1]
@retval		D_DDIM_OK						Success
@remarks	SPR function is started(enable) Async.
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Im_CNR_OTF_Start_Async( UCHAR ch );

/**
SPR Wait End for On The Fly
@param [in]	ch				channel[0 - 1]
@param[out]	p_wait_factor	: Pointer of factor bitfield parameter which release wait process.<br>
								  <ul>
										 <li>@ref D_IM_CNR_FLG_0_OTF_SPR_END
										 <li>@ref D_IM_CNR_FLG_1_OTF_SPR_AXI_ERR
								  </ul>
@param[in]	wait_time			Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval		D_DDIM_OK						Success.
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_CNR_AXI_ERR				Fail - AXI bus error.
@retval		D_IM_CNR_TIMEOUT				Fail - Processing NG.(Time outed)
@retval		D_IM_CNR_SEM_NG					Fail - Process NG. (Event flag error)
@remarks	Wait for SPR function is ended.
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern	INT32 Im_CNR_OTF_Wait_End( UCHAR ch, UINT32* const p_wait_factor, const INT32 wait_time );

/**
SPR Stop for On The Fly
@param [in]	ch				channel[0 - 2]
@retval		D_DDIM_OK						Success.
@remarks	SPR function is stopped(disable).
*/
extern	INT32 Im_CNR_OTF_Stop( UCHAR ch );

/**
The semaphore of SPR is returned for On The Fly
@param [in]	ch				channel[0 - 1]
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_SEM_NG					Fail - Processing NG (system error)
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern	INT32 Im_CNR_OTF_Close( UCHAR ch );

/**
SPR Reset for On The Fly
@param [in]	ch				channel[0 - 2]
@retval		D_DDIM_OK						Success
@remarks	SPR function soft reset
*/
extern	INT32 Im_CNR_OTF_Reset( UCHAR ch );

/**
SPR Get Latest Address information for On The Fly
@param [in]		ch				channel[0 - 1]
@param [out]	latest_addr		latest CbCr image address
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_OTF_Get_Latest_OutAddr( UCHAR ch, ULONG* const latest_addr );

/**
Set SPR Y table for Middle frequency CbCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_mid_y	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Y table for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Set_CSPR_Mid_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_mid_y );

/**
Set SPR Y table for Middle frequency CbCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_mid_c	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Y table for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Set_CSPR_Mid_CC_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_mid_c );

/**
Set SPR CbCr table for Middle frequency CbCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_mid_c	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR CbCr table for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Set_CSPR_Mid_CC_C_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_mid_c );

/**
Set SPR Y table for Low frequency CbCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_low_y	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Y table for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Set_CSPR_Low_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_low_y );

/**
Set SPR Y table for Low frequency CbCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_low_c	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Y table for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Set_CSPR_Low_CC_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_low_c );

/**
Set SPR CbCr table for Low frequency CbCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_low_c	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR CbCr table for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Set_CSPR_Low_CC_C_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_low_c );

/**
Set SPR setting for CbCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_ctrl_c	CbCr suppre parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the Suppre for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Ctrl_C( UCHAR ch, const T_IM_CNR_CTRL_C* const cnr_ctrl_c );

/**
Set SPR Edge for CbcCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	cnr_cspr_edge	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Edge table for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Set_CSPR_Edge_Table( UCHAR ch, const T_IM_CNR_TABLE* const cnr_cspr_edge );

/**
Set SPR Hue specified for CbcCr for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	spr_cspr_fue	table value
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	Suppre CSPR Hue specified table for CbCr is set.
*/
extern	INT32 Im_CNR_OTF_Set_CSPR_Hue_Specified_Table( UCHAR ch, const T_IM_CNR_HUE_SPEC_TABLE* const spr_cspr_fue );

/**
Set CbCr setting of source for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	sprs_ctrl	Source CbCr adjustment parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the source CbCr Adjustment is set.
*/
extern	INT32 Im_CNR_OTF_Ctrl_SPRS_CC( UCHAR ch, const T_IM_CNR_CTRL_CC_SRC_DST* const sprs_ctrl );

/**
Set CbCr setting of destination for On The Fly
@param [in]	ch				channel[0 - 2]
@param[in]	sprd_ctrl	Destination CbCr adjustment parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
@remarks	The calcurate mode of the destination CbCr Adjustment is set.
*/
extern	INT32 Im_CNR_OTF_Ctrl_SPRD_CC( UCHAR ch, const T_IM_CNR_CTRL_CC_SRC_DST* const sprd_ctrl );
/* @} */


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
//------------ for RDMA ----------------
/**
Get SPR setting address information for C for Off Line
@param[in]	ch				channel[0 - 2]
@param[out]	cnr_ctrl_c		Top address of the address array of C suppre parameter
@param[out]	size			Size of the address array of C suppre parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_Ctrl_C( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CTRL_C** const cnr_ctrl_c, ULONG* const size );

/**
Set SPR setting parameter information for C for Off Line
@param[in]	spr_ctrl_c		C suppre parameter
@param[out]	rdma_ctrl_c		C suppre parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_Ctrl_C( const T_IM_CNR_CTRL_C* const spr_ctrl_c, T_IM_CNR_RDMA_DATA_CTRL_C* const rdma_ctrl_c );

/**
Get SPR setting address information for C for On The Fly
@param[in]	ch				channel[0 - 2]
@param[out]	cnr_ctrl_c		Top address of the address array of C suppre parameter
@param[out]	size			Size of the address array of C suppre parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_Ctrl_C( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CTRL_C** const cnr_ctrl_c, ULONG* const size );

/**
Set SPR setting parameter information for C for On The Fly
@param[in]	spr_ctrl_c		C suppre parameter
@param[out]	rdma_ctrl_c		C suppre parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_Ctrl_C( const T_IM_CNR_CTRL_C* const spr_ctrl_c, T_IM_CNR_RDMA_DATA_CTRL_C* const rdma_ctrl_c );


/**
Get SPR setting address information for Y for Off Line
@param[in]	ch				channel[0 - 2]
@param[out]	cnr_ctrl_y		Top address of the address array of Y suppre parameter
@param[out]	size			Size of the address array of Y suppre parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y( UCHAR ch, const T_IM_CNR_RDMA_ADDR_OFL_CTRL_Y** const cnr_ctrl_y, ULONG* const size );

/**
Set SPR setting parameter information for Y for Off Line
@param[in]	spr_ctrl_y		Y suppre parameter
@param[out]	rdma_ctrl_y		Y suppre parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_Ctrl_Y( const T_IM_CNR_OFL_CTRL_Y* const spr_ctrl_y, T_IM_CNR_RDMA_DATA_OFL_CTRL_Y* const rdma_ctrl_y );


/**
Get SPR table setting address information for Y for Off Line
@param[in]	ch				channel[0 - 2]
@param[out]	cnr_yspr_tbl	Top address of the address array of Y suppre table parameter
@param[out]	size			Size of the address array of Y suppre table parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_YSPR_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_OFL_YSPR_TBL** const cnr_yspr_tbl, ULONG* const size );

/**
Set SPR table setting parameter information for Y for Off Line
@param[in]	yspr_table		Y suppre table parameter
@param[out]	rdma_yspr_table	Y suppre table parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_YSPR_Table( const T_IM_CNR_TABLE* const yspr_table, T_IM_CNR_RDMA_DATA_OFL_YSPR_TBL* const rdma_yspr_table );


/**
Get CSPR Mid-Y setting address information for C for Off Line
@param[in]	ch				channel[0 - 2]
@param[out]	cspr_mid_y_tbl	Top address of the address array of C suppre Middle-Y parameter
@param[out]	size			Size of the address array of C suppre Middle-Y parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL** const cspr_mid_y_tbl, ULONG* const size );

/**
Set CSPR Mid-Y setting parameter information for C for Off Line
@param[in]	spr_cspr_mid_y			C suppre Middle-Y parameter
@param[out]	rdma_cspr_mid_y_table	C suppre Middle-Y parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_y, T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL* const rdma_cspr_mid_y_table );

/**
Get CSPR Mid-Y setting address information for C for On The Fly
@param[in]	ch				channel[0 - 2]
@param[out]	cspr_mid_y_tbl	Top address of the address array of C suppre Middle-Y parameter
@param[out]	size			Size of the address array of C suppre Middle-Y parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL** const cspr_mid_y_tbl, ULONG* const size );

/**
Set CSPR Mid-Y setting parameter information for C for On The Fly
@param[in]	spr_cspr_mid_y			C suppre Middle-Y parameter
@param[out]	rdma_cspr_mid_y_table	C suppre Middle-Y parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_y, T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL* const rdma_cspr_mid_y_table );


/**
Get CSPR Mid-CC-Y setting address information for C for Off Line
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_mid_cc_y_tbl	Top address of the address array of C suppre Middle-CC-Y parameter
@param[out]	size				Size of the address array of C suppre Middle-CC-Y parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL** const cspr_mid_cc_y_tbl, ULONG* const size );

/**
Set CSPR Mid-CC-Y setting parameter information for C for Off Line
@param[in]	spr_cspr_mid_cc_y			C suppre Middle-CC-Y parameter
@param[out]	rdma_cspr_mid_cc_y_table	C suppre Middle-CC-Y parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_cc_y, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL* const rdma_cspr_mid_cc_y_table );

/**
Get CSPR Mid-CC-Y setting address information for C for On The Fly
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_mid_cc_y_tbl	Top address of the address array of C suppre Middle-CC-Y parameter
@param[out]	size				Size of the address array of C suppre Middle-CC-Y parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL** const cspr_mid_cc_y_tbl, ULONG* const size );

/**
Set CSPR Mid-CC-Y setting parameter information for C for On The Fly
@param[in]	spr_cspr_mid_cc_y			C suppre Middle-CC-Y parameter
@param[out]	rdma_cspr_mid_cc_y_table	C suppre Middle-CC-Y parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_cc_y, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL* const rdma_cspr_mid_cc_y_table );


/**
Get CSPR Mid-CC-C setting address information for C for Off Line
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_mid_cc_c_tbl	Top address of the address array of C suppre Middle-CC-C parameter
@param[out]	size				Size of the address array of C suppre Middle-CC-C parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL** const cspr_mid_cc_c_tbl, ULONG* const size );

/**
Set CSPR Mid-CC-C setting parameter information for C for Off Line
@param[in]	spr_cspr_mid_cc_c			C suppre Middle-CC-C parameter
@param[out]	rdma_cspr_mid_cc_c_table	C suppre Middle-CC-C parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_cc_c, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL* const rdma_cspr_mid_cc_c_table );

/**
Get CSPR Mid-CC-C setting address information for C for On The Fly
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_mid_cc_c_tbl	Top address of the address array of C suppre Middle-CC-C parameter
@param[out]	size				Size of the address array of C suppre Middle-CC-C parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL** const cspr_mid_cc_c_tbl, ULONG* const size );

/**
Set CSPR Mid-CC-C setting parameter information for C for On The Fly
@param[in]	spr_cspr_mid_cc_c			C suppre Middle-CC-C Middle-CC-C parameter
@param[out]	rdma_cspr_mid_cc_c_table	C suppre Middle-CC-C Middle-CC-C parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_cc_c, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL* const rdma_cspr_mid_cc_c_table );


/**
Get CSPR Low-Y setting address information for C for Off Line
@param[in]	ch				channel[0 - 2]
@param[out]	cspr_low_y_tbl	Top address of the address array of C suppre Low-Y parameter
@param[out]	size			Size of the address array of C suppre Low-Y parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL** const cspr_low_y_tbl, ULONG* const size );

/**
Set CSPR Low-Y setting parameter information for C for Off Line
@param[in]	spr_cspr_low_y			C suppre Low-Y parameter
@param[out]	rdma_cspr_low_y_table	C suppre Low-Y parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_low_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL* const rdma_cspr_low_y_table );

/**
Get CSPR Low-Y setting address information for C for On The Fly
@param[in]	ch				channel[0 - 2]
@param[out]	cspr_low_y_tbl	Top address of the address array of C suppre Low-Y parameter
@param[out]	size			Size of the address array of C suppre Low-Y parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL** const cspr_low_y_tbl, ULONG* const size );

/**
Set CSPR Low-Y setting parameter information for C for On The Fly
@param[in]	spr_cspr_low_y			C suppre Low-Y parameter
@param[out]	rdma_cspr_low_y_table	C suppre Low-Y parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_low_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL* const rdma_cspr_low_y_table );


/**
Get CSPR Low-CC-Y setting address information for C for Off Line
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_low_cc_y_tbl	Top address of the address array of C suppre Low-CC-Y parameter
@param[out]	size				Size of the address array of C suppre Low-CC-Y parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL** const cspr_low_cc_y_tbl, ULONG* const size );

/**
Set CSPR Low-CC-Y setting parameter information for C for Off Line
@param[in]	spr_cspr_low_cc_y			C suppre Low-CC-Y parameter
@param[out]	rdma_cspr_low_cc_y_table	C suppre Low-CC-Y parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_low_cc_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL* const rdma_cspr_low_cc_y_table );

/**
Get CSPR Low-CC-Y setting address information for C for On The Fly
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_low_cc_y_tbl	Top address of the address array of C suppre Low-CC-Y parameter
@param[out]	size				Size of the address array of C suppre Low-CC-Y parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL** const cspr_low_cc_y_tbl, ULONG* const size );

/**
Set CSPR Low-CC-Y setting parameter information for C for On The Fly
@param[in]	spr_cspr_low_cc_y			C suppre Low-CC-Y parameter
@param[out]	rdma_cspr_low_cc_y_table	C suppre Low-CC-Y parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_low_cc_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL* const rdma_cspr_low_cc_y_table );


/**
Get CSPR Low-CC-C setting address information for C for Off Line
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_low_cc_c_tbl	Top address of the address array of C suppre Low-CC-C parameter
@param[out]	size				Size of the address array of C suppre Low-CC-C parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL** const cspr_low_cc_c_tbl, ULONG* const size );

/**
Set CSPR Low-CC-C setting parameter information for C for Off Line
@param[in]	spr_cspr_low_cc_c			C suppre Low-CC-C parameter
@param[out]	rdma_cspr_low_cc_c_table	C suppre Low-CC-C parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table( const T_IM_CNR_TABLE* const spr_cspr_low_cc_c, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL* const rdma_cspr_low_cc_c_table );

/**
Get CSPR Low-CC-C setting address information for C for On The Fly
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_low_cc_c_tbl	Top address of the address array of C suppre Low-CC-C parameter
@param[out]	size				Size of the address array of C suppre Low-CC-C parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL** const cspr_low_cc_c_tbl, ULONG* const size );

/**
Set CSPR Low-CC-C setting parameter information for C for On The Fly
@param[in]	spr_cspr_low_cc_c			C suppre Low-CC-C parameter
@param[out]	rdma_cspr_low_cc_c_table	C suppre Low-CC-C parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table( const T_IM_CNR_TABLE* const spr_cspr_low_cc_c, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL* const rdma_cspr_low_cc_c_table );


/**
Get CSPR Edge setting address information for C for Off Line
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_edge_tbl		Top address of the address array of C suppre Edge parameter
@param[out]	size				Size of the address array of C suppre Edge parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL** const cspr_edge_tbl, ULONG* const size );

/**
Set CSPR Edge setting parameter information for C for Off Line
@param[in]	spr_cspr_edge			C suppre Edge parameter
@param[out]	rdma_cspr_edge_table	C suppre Edge parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table( const T_IM_CNR_TABLE* const spr_cspr_edge, T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL* const rdma_cspr_edge_table );

/**
Get CSPR Edge setting address information for C for On The Fly
@param[in]	ch				channel[0 - 2]
@param[out]	cspr_edge_tbl	Top address of the address array of C suppre Edge parameter
@param[out]	size			Size of the address array of C suppre Edge parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL** const cspr_edge_tbl, ULONG* const size );

/**
Set CSPR Edge setting parameter information for C for On The Fly
@param[in]	spr_cspr_edge			C suppre Edge parameter
@param[out]	rdma_cspr_edge_table	C suppre Edge parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table( const T_IM_CNR_TABLE* const spr_cspr_edge, T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL* const rdma_cspr_edge_table );


/**
Get CSPR Hue Specified setting address information for C for Off Line
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_hue_tbl		Top address of the address array of C suppre Hue Specified parameter
@param[out]	size				Size of the address array of C suppre Hue Specified parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL** const cspr_hue_tbl, ULONG* const size );

/**
Set CSPR Hue Specified setting parameter information for C for Off Line
@param[in]	spr_cspr_hue			C suppre Hue Specified parameter
@param[out]	rdma_cspr_hue_table		C suppre Hue Specified parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table( const T_IM_CNR_HUE_SPEC_TABLE* const spr_cspr_hue, T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL* const rdma_cspr_hue_table );

/**
Get CSPR Hue Specified setting address information for C for On The Fly
@param[in]	ch				channel[0 - 2]
@param[out]	cspr_hue_tbl	Top address of the address array of C suppre Hue Specified parameter
@param[out]	size			Size of the address array of C suppre Hue Specified parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL** const cspr_hue_tbl, ULONG* const size );

/**
Set CSPR Hue Specified setting parameter information for C for On The Fly
@param[in]	spr_cspr_hue			C suppre Hue Specified parameter
@param[out]	rdma_cspr_hue_table		C suppre Hue Specified parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table( const T_IM_CNR_HUE_SPEC_TABLE* const spr_cspr_hue, T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL* const rdma_cspr_hue_table );


/**
Get CSPR SPRS setting address information for C for Off Line
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_sprs_cc_tbl	Top address of the address array of C suppre SPRS CC parameter
@param[out]	size				Size of the address array of C suppre SPRS CC parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL** const cspr_sprs_cc_tbl, ULONG* const size );

/**
Set CSPR SPRS setting parameter information for C for Off Line
@param[in]	cspr_sprs_cc			C suppre SPRS CC parameter
@param[out]	rdma_cspr_sprs_cc		C suppre SPRS CC parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table( const T_IM_CNR_CTRL_CC_SRC_DST* const cspr_sprs_cc, T_IM_CNR_RDMA_DATA_SPRS_CC_TBL* const rdma_cspr_sprs_cc );

/**
Get CSPR SPRS setting address information for C for On The Fly
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_sprs_cc_tbl	Top address of the address array of C suppre SPRS CC parameter
@param[out]	size				Size of the address array of C suppre SPRS CC parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL** const cspr_sprs_cc_tbl, ULONG* const size );

/**
Set CSPR SPRS setting parameter information for C for On The Fly
@param[in]	cspr_sprs_cc			C suppre SPRS CC parameter
@param[out]	rdma_cspr_sprs_cc		C suppre SPRS CC parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table( const T_IM_CNR_CTRL_CC_SRC_DST* const cspr_sprs_cc, T_IM_CNR_RDMA_DATA_SPRS_CC_TBL* const rdma_cspr_sprs_cc );


/**
Get CSPR SPRD setting address information for C for Off Line
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_sprd_cc_tbl	Top address of the address array of C suppre SPRD CC parameter
@param[out]	size				Size of the address array of C suppre SPRD CC parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL** const cspr_sprd_cc_tbl, ULONG* const size );

/**
Set CSPR SPRD setting parameter information for C for Off Line
@param[in]	cspr_sprd_cc			C suppre SPRD CC parameter
@param[out]	rdma_cspr_sprd_cc		C suppre SPRD CC parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table( const T_IM_CNR_CTRL_CC_SRC_DST* const cspr_sprd_cc, T_IM_CNR_RDMA_DATA_SPRD_CC_TBL* const rdma_cspr_sprd_cc );

/**
Get CSPR SPRD setting address information for C for On The Fly
@param[in]	ch					channel[0 - 2]
@param[out]	cspr_sprd_cc_tbl	Top address of the address array of C suppre SPRD CC parameter
@param[out]	size				Size of the address array of C suppre SPRD CC parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL** const cspr_sprd_cc_tbl, ULONG* const size );

/**
Set CSPR SPRD setting parameter information for C for On The Fly
@param[in]	cspr_sprd_cc			C suppre SPRD CC parameter
@param[out]	rdma_cspr_sprd_cc		C suppre SPRD CC parameter for RDMA
@retval		D_DDIM_OK						Success
@retval		D_IM_CNR_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table( const T_IM_CNR_CTRL_CC_SRC_DST* const cspr_sprd_cc, T_IM_CNR_RDMA_DATA_SPRD_CC_TBL* const rdma_cspr_sprd_cc );
#endif


#ifdef __cplusplus
}
#endif

#endif

/**
@weakgroup im_cnr
@{

<hr>

@section im_cnr_sample	Sample code
@code
// SAMPLE CODE //
#include "im_cnr.h"

INT32 sample_cnr( VOID )
{
	INT32 im_ercd;
	INT32 im_ercd2;
	T_IM_CNR_OFL_CTRL cnr_ctrl = {
		.mode = E_IM_CNR_YCC_MODE_YCC422,			// mode			SPR YCC mode	@@YUV_MODE

		.width = 4000,								// width		SPR valid data width	@@HSIZE
		.lines = 3000,								// lines		SPR valid data lines	@@VSIZE

		.r_y_width = 4000,							// r_y_width	Y width for read	@@RYDEF
		.r_y_addr = 0x42000000,						// r_y_addr		Y address for read	@@RYTA

		.r_c_width = 4000 /2,						// r_c_width	CbCr width for read	@@RCDEF
		.r_cb_addr = 0x43000000,					// r_cb_addr	Cb address for read	@@RCBTA
		.r_cr_addr = 0x44000000,					// r_cr_addr	Cr address for read	@@RCRTA

		.w_y_width = 4000,							// w_y_width	Y width for write	@@WYDEF
		.w_y_addr = 0x45000000,						// w_y_addr		Y address for write	@@WYTA

		.w_c_width = 4000 /2,						// w_c_width	CbCr width for write	@@WCDEF
		.w_cb_addr = 0x46000000,					// w_cb_addr	Cb address for write	@@WCBTA
		.w_cr_addr = 0x47000000,					// w_cr_addr	Cr address for write	@@WCRTA

		.work_addr = 0x48000000,					// work_addr	Address fir work area	@@TMPTA

		.divide_top = E_IM_CNR_VPROCESS_NORM,		// divide_top	Virtucal process devide	@@VDIVT
		.divide_bot = E_IM_CNR_VPROCESS_NORM,		// divide_bot	Virtucal process devide	@@VDIVB

		.pCallBack = NULL,							// pCallBack	callback function
		.user_param = 0,							// user_param	callback arg
	};
	T_IM_CNR_CTRL_C cnr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,					// enable				CbCr SPR enable @@CSPREN
		.gradation_keep_en = E_IM_CNR_ONOFF_ENABLE,			// enable				gradation keep enable @@CGDKEN
		.mid_freq_nr = {									// Middle range frequency Color NR parmeter
			.enable = E_IM_CNR_ONOFF_ENABLE,				// Middle CbCr SPR enable @@MCEN
			.h_y_threshold = {								// Middle SPR Y threshold for horizontal	@@MCYTHH
				0, 0, 0, 0										// register default. Need tuning.
			},
			.v_y_threshold = {								// Middle SPR Y threshold for vertical		@@MCYTHV
				0, 0, 0, 0										// register default. Need tuning.
			},
			.h_c_threshold = {								// Middle SPR CbCr threshold for horizontal	@@MCCTHH
				0, 0, 0, 0										// register default. Need tuning.
			},
			.v_c_threshold = {								// Middle SPR CbCr threshold for vertical	@@MCCTHV
				0, 0, 0, 0										// register default. Need tuning.
			},
			.y_threshold_cor = 0,							// Middle SPR Y threshold coring	@@MCYDYMYCR
			.y_threshold_gain = 0,							// register default. Need tuning.
			.y_threshold_dym_en = E_IM_CNR_ONOFF_DISABLE,	// Middle Y threshold dynamic setting enable @@MCYDYM
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,	// Middle Y threshold scale enable @@MCYSCL
			.c_y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,	// Middle CbCr threshold Y scale enable @@MCSSCL
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,	// Middle CbCr threshold scale enable @@MCCSCL
		},
		.low_freq_nr = {									// Low range frequency Color NR parmeters
			.enable = E_IM_CNR_ONOFF_ENABLE,					// Low CbCr SPR enable @@LCEN
			.h_y_threshold = {								// Low SPR Y threshold for horizontal		@@LCYTHH
				0, 0, 0, 0										// register default. Need tuning.
			},
			.v_y_threshold = {								// Low SPR Y threshold for vertical			@@LCYTHV
				0, 0, 0, 0										// register default. Need tuning.
			},
			.h_c_threshold = {								// Low SPR CbCr threshold for horizontal	@@LCCTHH
				0, 0, 0, 0										// register default. Need tuning.
			},
			.v_c_threshold = {								// Low SPR CbCr threshold for vertical		@@LCCTHV
				0, 0, 0, 0										// register default. Need tuning.
			},
			.y_threshold_cor = 0,							// Low SPR Y threshold coring	@@LCYDYMYCR
			.y_threshold_gain = 0,							// register default. Need tuning.
			.y_threshold_dym_en = E_IM_CNR_ONOFF_DISABLE,	// Low Y threshold dynamic setting enable @@LCYDYM
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,	// Low Y threshold scale enable @@LCYSCL
			.c_y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,	// Low CbCr threshold Y scale enable @@LCSSCL
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,	// Low CbCr threshold scale enable @@LCCSCL
		},
	};
	T_IM_CNR_OFL_CTRL_Y cnr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_ENABLE,					// enable				Y SPR enable @@YSPREN
		.threshold_type = E_IM_CNR_THRES_REGISTER,			// threshold_type		threshold type for Y	@@YSPRMODE YSPRMD
		.level = E_IM_CNR_LEVEL_LOW,						// level				filter level		@@YSPRMODE YSPRFE
		.h_y_threshold = 0,									// h_y_threshold		SPR Y threshold for horizontal		@@YSPRFXEYH
		.v_y_threshold = 0,									// v_y_threshold		SPR Y threshold for vertical		@@YSPRFXEYV
		.h_c_threshold = 0,									// h_c_threshold		SPR CbCr threshold for horizontal	@@YSPRFXECH
		.v_c_threshold = 0,									// v_c_threshold		SPR CbCr threshold for vertical		@@YSPRFXECV
		.alpha_blend_ratio = 0,								// alpha_blend_ratio	Alpha blend ratio			@@YSPRALPBD
	};



	im_ercd = Im_CNR_OFL_Open( 0, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ) {
		return im_ercd;
	}

	// don't forget Im_CNR_OFL_Close() after this line

	while( 1 ) {
		im_ercd = Im_CNR_OFL_Ctrl( 0, &cnr_ctrl );
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		im_ercd = Im_CNR_OFL_Ctrl_C( 0, &cnr_ctrl_c );
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		im_ercd = Im_CNR_OFL_Ctrl_Y( 0, &cnr_ctrl_y );
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		im_ercd = Im_CNR_OFL_Start_Sync( 0, 1000 );	// wait timeout 1000 ticks
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		break;
	}

	im_ercd2 = Im_CNR_OFL_Stop( 0 );
	if( im_ercd2 != D_DDIM_OK ) {
		im_ercd = im_ercd2;
	}

	if( im_ercd == D_DDIM_OK ) {
		im_ercd2 = Im_CNR_OFL_Close( 0 );
		if( im_ercd2 != D_DDIM_OK ) {
			im_ercd = im_ercd2;
		}
	}
	else {
		(VOID)Im_CNR_OFL_Close( 0 );
	}

	return im_ercd;
}
@endcode
@}*/


