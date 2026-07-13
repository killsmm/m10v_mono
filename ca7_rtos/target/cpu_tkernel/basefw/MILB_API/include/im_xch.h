/**
 * @file		im_xch.h
 * @brief		Header file of XH2D driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup im_xch
@{
	- @ref XCH_HIGH-SPEED_DOWNSAMPLEING
	- @ref XCH_DOWNSAMPLEING_ASYNC_WAIT
	- @ref XCH_HISTOGRAM_ASYNC_CALLBACK
	- @ref XCH_SIMPLICITY_COPY
	- @ref XCH_SIMPLICITY_FILL
	- @ref XCH_MEMCPY
	- @ref XCH_SIMPLICITY_HISTOGRAM
	- @ref XCH_SIMPLICITY_DOWNSAMPLEING
*/

#ifndef _IM_XCH_H_
#define _IM_XCH_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// #define D_IM_XCH_PRINT_ST	// for debug

/* Return value of processing result
*/
#define D_IM_XCH_OK						(D_DDIM_OK)								/**< Normal end						*/
#define D_IM_XCH_NG						(D_IM_XCH | D_DDIM_SYSTEM_ERROR)		/**< Processing NG					*/
#define D_IM_XCH_INPUT_PARAM_ERR		(D_IM_XCH | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error			*/
#define D_IM_XCH_SEM_TIMEOUT			(D_IM_XCH | D_DDIM_SEM_TIMEOUT)			/**< Semaphore acquisition Time Out */
#define D_IM_XCH_SEM_NG					(D_IM_XCH | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG		*/
#define D_IM_XCH_MACRO_BUSY_NG			(D_IM_XCH | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy Error				*/


/* Interrupt flag
*/
#define D_IM_XCH_FLG_0_WAIT_END			(0x00000001)							/**< Xch-0 flag to wait for async processing end */
#define D_IM_XCH_FLG_1_WAIT_END			(0x00000002)							/**< Xch-1 flag to wait for async processing end */

/* XMD (Mode select)
*/
#define D_IM_XCH_MD_DOWN_HI_SPEED		(0)										/**< Downsampling process mode (high-speed) */
#define D_IM_XCH_MD_DOWN				(1)										/**< Downsampling process mode */
#define D_IM_XCH_MD_HISTGRAM_DOWN		(2)										/**< Get histogram data mode (downsampling output transfer available) */
#define D_IM_XCH_MD_HISTGRAM			(3)										/**< Get histogram data mode (no downsampling output transfer) */
#define D_IM_XCH_MD_HORI_1_2_EVEN		(4)										/**< Horizontal 1/2 downsampling mode (even pixel output) */
#define D_IM_XCH_MD_HORI_1_2_ODD		(5)										/**< Horizontal 1/2 downsampling mode (odd pixel output) */
#define D_IM_XCH_MD_COPY				(6)										/**< Copy transfer mode */
#define D_IM_XCH_MD_FILL				(7)										/**< Fill mode */

/* XDTYPE (Type select)
*/
#define D_IM_XCH_XDTYPE_8				(0)										/**< Input data width of 8bit */
#define D_IM_XCH_XDTYPE_16				(1)										/**< Input data width of 16bit */

/* HCNT0 (Histgram initizlize setting)
*/
#define D_IM_XCH_INITIALIZE				(0)										/**< Initialize the histgram data */
#define D_IM_XCH_INHERITING				(1)										/**< Inheriting the histgram data */

#ifdef CO_DDIM_UTILITY_USE
/**
@name Utility Definition
@{*/

//---------------------- utility section -------------------------------

/* Down sampling rate select */
#define D_IM_XCH_THIN_SEL_1_1			(0)										/**< 1/1 (not downsampling) */
#define D_IM_XCH_THIN_SEL_1_2			(1)										/**< 1/2 downsampling out */
#define D_IM_XCH_THIN_SEL_1_4			(2)										/**< 1/4 downsampling out */
#define D_IM_XCH_THIN_SEL_3_4			(3)										/**< 3/4 downsampling out */

//---------------------- colabo  section -------------------------------

/*@}*/
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Xch0 or Xch1 select
*/
typedef enum {
	E_IM_XCH_CH_SEL_0		= 0,		/**< Select X0ch */
	E_IM_XCH_CH_SEL_1					/**< Select X1ch */
} E_IM_XCH_CH_SEL;

/** Xch0 or Xch1 transfer status
*/
typedef enum {
	E_IM_XCH_ST_STOPPED	= 0,			/**< Xch stopped */
	E_IM_XCH_ST_RUNNING					/**< Xch running */
} E_IM_XCH_ST;

#ifdef CO_DDIM_UTILITY_USE
/**
@name Utility Enumeration
@{*/

//---------------------- utility section -------------------------------

/** Copy mode select
*/
typedef enum {
	E_IM_XCH_COPY_SEL_HI		= 0,	/**< Select hi-speed copy */
	E_IM_XCH_COPY_SEL_NORMAL			/**< Select normal copy */
} E_IM_XCH_COPY_SEL;

//---------------------- colabo  section -------------------------------

/*@}*/
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Xch driver common control structure
*/
typedef struct {
	UCHAR	mode;								/**< Operation mode. @@XMD
													<ul>
														<li>@ref D_IM_XCH_MD_DOWN_HI_SPEED
														<li>@ref D_IM_XCH_MD_DOWN
														<li>@ref D_IM_XCH_MD_HISTGRAM_DOWN
														<li>@ref D_IM_XCH_MD_HISTGRAM
														<li>@ref D_IM_XCH_MD_HORI_1_2_EVEN
														<li>@ref D_IM_XCH_MD_HORI_1_2_ODD
														<li>@ref D_IM_XCH_MD_COPY
														<li>@ref D_IM_XCH_MD_FILL
													</ul>
												 */
	USHORT	g_hsize;							/**< Global horizontal size of the output area. @@XGHW <br>
													 Set at 16 byte units.
												 */
	USHORT	d_hsize;							/**< Horizontal size of the output area. @@XDHW <br>
													 Set at 16 byte units if you use the following mode.<br>
													<ul>
														<li>@ref D_IM_XCH_MD_HORI_1_2_EVEN
														<li>@ref D_IM_XCH_MD_HORI_1_2_ODD
														<li>@ref D_IM_XCH_MD_COPY
													</ul>
												 */
	USHORT	d_vsize;							/**< Vertical size of the output area. @@XDVW
												 */
	UINT32	dst_addr;							/**< Start address of the output area. @@XDA <br>
													 Set at 16 byte units if you use the following mode.<br>
													<ul>
														<li>@ref D_IM_XCH_MD_HORI_1_2_EVEN
														<li>@ref D_IM_XCH_MD_HORI_1_2_ODD
														<li>@ref D_IM_XCH_MD_COPY
													</ul><br>
													 But, When accessing the SRAM, Set at 32 byte units.
												 */
	VP_CALLBACK	pCallBack;						/**< Callback function pointer.
												 */
} T_IM_XCH_CTRL_CMN;

/** Xch driver downsampling control structure
*/
typedef struct {
	UCHAR	xdtype;								/**< Input data width. @@XDDTYPE @@XSDTYPE <br>
													<ul>
														<li>@ref D_IM_XCH_XDTYPE_8
														<li>@ref D_IM_XCH_XDTYPE_16
													</ul>
												 */
	UCHAR	hcyc;								/**< Down sampling cycle of the horizontal. @@XHCYC <br>
													 Value Range :[0x00-0x1F] <br>
													  0x00 : 32 byte cycle<br>
													  0x01 : 1 byte cycle<br>
													  :<br>
													  0x1F : 31 byte cycle<br>
												 */
	UCHAR	vcyc;								/**< Down sampling cycle of the vertical.  @@XVCYC <br>
													 Value Range :[0x00-0x1F] <br>
													  0x00 : 32 line cycle<br>
													  0x01 : 1 line cycle<br>
													  :<br>
													  0x1F : 31 line cycle<br>
												 */
	UINT32	hen;								/**< Data acquisition in the horizontal direction. @@XHEN <br>
													 (Set in the MSB-justified)
													 Please set so that there is valid data to within range of "hcyc".
												 */
	UINT32	ven;								/**< Line acquisition in the vertical direction. @@XVEN <br>
													 (Set in the MSB-justified)
													 Please set so that there is valid data to within range of "vcyc".
												 */
	USHORT	s_hsize;							/**< The horizontal size of the original data. @@XSHW <br>
													 Set at 16 byte units.
												 */
	UINT32	src_addr;							/**< Start address of the source area. @@XSA <br>
													 When accessing the SRAM, Set at 64 byte units.
												 */
} T_IM_XCH_CTRL_THIN;

/** Xch driver histogram control structure
*/
typedef struct {
	UCHAR	xdtype;								/**< Input data width. @@XDDTYPE @@XSDTYPE <br>
													<ul>
														<li>@ref D_IM_XCH_XDTYPE_8
														<li>@ref D_IM_XCH_XDTYPE_16
													</ul>
												 */
	UCHAR	hcyc;								/**< Down sampling cycle of the horizontal. @@XHCYC <br>
													 Value Range :[0x00-0x1F] <br>
													  0x00 : 32 byte cycle<br>
													  0x01 : 1 byte cycle<br>
													  :<br>
													  0x1F : 31 byte cycle<br>
												*/
	UCHAR	vcyc;								/**< Down sampling cycle of the vertical.  @@XVCYC <br>
													 Value Range :[0x00-0x1F] <br>
													  0x00 : 32 line cycle<br>
													  0x01 : 1 line cycle<br>
													  :<br>
													  0x1F : 31 line cycle<br>
												 */
	UINT32	hen;								/**< Data acquisition in the horizontal direction. @@XHEN <br>
													 (Set in the MSB-justified)<br>
													 Please set so that there is valid data to within range of "hcyc".
												 */
	UINT32	ven;								/**< Line acquisition in the vertical direction. @@XVEN <br>
													 (Set in the MSB-justified)<br>
													 Please set so that there is valid data to within range of "vcyc".
												 */
	USHORT	s_hsize;							/**< The horizontal size of the original data. @@XSHW <br>
													 Set at 16 byte units.
												 */
	UINT32	src_addr;							/**< Start address of the source area. @@XSA <br>
													 When accessing the SRAM, Set at 64 byte units.
												 */
	UCHAR	hcnt;								/**< The initialize or inheriting of the histogram data. @@XHCNT0 <br>
													<ul>
														<li>@ref D_IM_XCH_INITIALIZE
														<li>@ref D_IM_XCH_INHERITING
													</ul>
												 */
} T_IM_XCH_CTRL_HIST;

/** Xch driver half downsampling control structure
*/
typedef struct {
	UCHAR	vcyc;								/**< Down sampling cycle of the vertical.  @@XVCYC <br>
													 Value Range :[0x00-0x1F] <br>
													  0x00 : 32 line cycle<br>
													  0x01 : 1 line cycle<br>
													  :<br>
													  0x1F : 31 line cycle<br>
												 */
	UINT32	ven;								/**< Line acquisition in the vertical direction. @@XVEN <br>
													 (Packed to the left)
												 */
	USHORT	s_hsize;							/**< The horizontal size of the original data. @@XSHW <br>
													 Set at 16 byte units.
												 */
	UINT32	src_addr;							/**< Start address of the source area. @@XSA <br>
													 Set at 16 byte units.<br>
													 But, When accessing the SRAM, Set at 64 byte units.
												 */
} T_IM_XCH_CTRL_HALF_THIN;

/** Xch driver copy control structure
*/
typedef struct {
	UCHAR	vcyc;								/**< Down sampling cycle of the vertical.  @@XVCYC <br>
													 Value Range :[0x00-0x1F] <br>
													  0x00 : 32 line cycle<br>
													  0x01 : 1 line cycle<br>
													  :<br>
													  0x1F : 31 line cycle<br>
												 */
	UINT32	ven;								/**< Line acquisition in the vertical direction. @@XVEN <br>
													 (Packed to the left)
												 */
	USHORT	s_hsize;							/**< The horizontal size of the original data. @@XSHW <br>
													 Set at 16 byte units.
												 */
	UINT32	src_addr;							/**< Start address of the source area. @@XSA <br>
													 Set at 16 byte units.<br>
													 But, When accessing the SRAM, Set at 64 byte units.
												 */
} T_IM_XCH_CTRL_COPY;

#ifdef CO_DDIM_UTILITY_USE

//---------------------- utility section -------------------------------

/**
@name Utility Data Structures
@{*/

/** Xch driver copy structure
*/
typedef struct {
	E_IM_XCH_CH_SEL		xch;					/**< Channel select.<br>
													<ul>
														<li>@ref E_IM_XCH_CH_SEL_0
														<li>@ref E_IM_XCH_CH_SEL_1
													</ul>
												 */
	E_IM_XCH_COPY_SEL	copy_type;				/**< Copy type select. <br>
													<ul>
														<li>@ref E_IM_XCH_COPY_SEL_HI
														<li>@ref E_IM_XCH_COPY_SEL_NORMAL
													</ul>
												 */
	struct {
		USHORT			width;					/**< Copy image width. @@XDHW <br><br>
													 Restrictions by the use type.<br>
													 <ul>
														<li>@ref E_IM_XCH_COPY_SEL_HI &nbsp;: Set at 16 byte units.
													 </ul>
												 */
		USHORT			lines;					/**< Copy image lines. @@XDVW
												 */
		USHORT			src_gl_width;			/**< Source image width. @@XSHW<br>
													 Set at 16 byte units.
												 */
		USHORT			dst_gl_width;			/**< Destination image global width. @@XGHW<br>
													 Set at 16 byte units.
												 */
	} size;										/**< Src and Dst image size. */
	struct {
		ULONG			src;					/**< Source data address. @@XSA <br><br>
													 Restrictions by the use type.<br>
													 <ul>
														<li>@ref E_IM_XCH_COPY_SEL_HI &nbsp;: Set at 16 byte units.
													 </ul>
													 But, When accessing the SRAM, Set at 64 byte units.
												 */
		ULONG			dst;					/**< Destination data address. @@XDA <br><br>
													 Restrictions by the use type.<br>
													 <ul>
														<li>@ref E_IM_XCH_COPY_SEL_HI &nbsp;: Set at 16 byte units.
													 </ul>
													 But, When accessing the SRAM, Set at 32 byte units.
												 */
	} addr;										/**< Src and Dst image address. */
} T_IM_XCH_COPY_EX;


/** Xch driver fill structure
*/
typedef struct {
	E_IM_XCH_CH_SEL		xch;					/**< Channel select.<br>
													<ul>
														<li>@ref E_IM_XCH_CH_SEL_0
														<li>@ref E_IM_XCH_CH_SEL_1
													</ul>
												 */
	UCHAR				fill_data;				/**< Data to fill. @@XFDT
												 */
	ULONG				fill_addr;				/**< Start address to fill. @@XDA <br>
													 When accessing the SRAM, Set at 32 byte units.
												 */
	struct {
		USHORT			width;					/**< Image width. @@XDHW
												 */
		USHORT			lines;					/**< Image lines. @@XDVW
												 */
		USHORT			gl_width;				/**< Image global width. @@XGHW<br>
													 Set at 16 byte units.
												 */
	} size;										/**< fill size */
} T_IM_XCH_FILL_EX;

/** Xch driver histogram structure
*/
typedef struct {
	ULONG				hist_addr;				/**< Histogram image address. @@XDA @@XSA <br>
													 When accessing the SRAM, Set at 64 byte units.
												 */
	struct {
		USHORT			width;					/**< Image width. @@XGHW @@XSHW @@XDHW<br>
													 Set at 16 byte units.
												 */
		USHORT			lines;					/**< Image lines. @@XDVW
												 */
	} size;										/**< histogram image size */
} T_IM_XCH_EXAMINE_HIST;

/** Xch driver downsampling structure
*/
typedef struct {
	E_IM_XCH_CH_SEL		xch;					/**< Channel select.<br>
													<ul>
														<li>@ref E_IM_XCH_CH_SEL_0
														<li>@ref E_IM_XCH_CH_SEL_1
													</ul>
												 */
	UINT32				thin_width;				/**< Thinning out rate select of width.<br>
													<ul>
														<li>@ref D_IM_XCH_THIN_SEL_1_1
														<li>@ref D_IM_XCH_THIN_SEL_1_2
														<li>@ref D_IM_XCH_THIN_SEL_1_4
														<li>@ref D_IM_XCH_THIN_SEL_3_4
													</ul>
												 */
	UINT32				thin_lines;				/**< Thinning out rate select of lines.<br>
													<ul>
														<li>@ref D_IM_XCH_THIN_SEL_1_1
														<li>@ref D_IM_XCH_THIN_SEL_1_2
														<li>@ref D_IM_XCH_THIN_SEL_1_4
														<li>@ref D_IM_XCH_THIN_SEL_3_4
													</ul>
												 */
	struct {
		USHORT			width;					/**< Source image width.
												 */
		USHORT			lines;					/**< Source image lines.
												 */
		USHORT			src_gl_width;			/**< Source image width. @@XSHW <br>
													 Set at 16 byte units.
												 */
		USHORT			dst_gl_width;			/**< Destination image global width. @@XGHW <br>
													 Set at 16 byte units.
												 */
	} size;										/**< Src and Dst image size. */
	struct {
		ULONG			src;					/**< Source data address. @@XSA <br>
													 When accessing the SRAM, Set at 64 byte units.<br>
												 */
		ULONG			dst;					/**< Destination data address. @@XDA <br>
													 When accessing the SRAM, Set at 32 byte units.<br>
												 */
	} addr;										/**< Src and Dst image address. */
} T_IM_XCH_CONV_THIN;

/*@}*/

//---------------------- colabo  section -------------------------------



#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
  The semaphore of Xch is acquired.
  @param[in]	xch							: Xch select
  @param[in]	tmout						: Timeout time
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG
  @retval		D_IM_XCH_SEM_TIMEOUT		: Semaphore acquisition TimeOut
  @remarks	Please call it before using Xch.
  @remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Im_Xch_Open( E_IM_XCH_CH_SEL xch, INT32 tmout );


/**
  Xch initialize.
*/
extern	VOID	Im_Xch_Init( VOID );


/**
  Set common control data.
  @param[in]	xch							: Xch select
  @param[in]	xch_ctrl_comm				: Common control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_CMN* xch_ctrl_comm );


/**
  Set down sampling control data.
  @param[in]	xch							: Xch select
  @param[in]	xch_ctrl_thin				: downsampling control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_THIN* xch_ctrl_thin );


/**
  Set histogram control data.
  @param[in]	xch							: Xch select
  @param[in]	xch_ctrl_hist				: histogram control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @remarks		X0ch only.
*/
extern	INT32	Im_Xch_Ctrl_Hist( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_HIST* xch_ctrl_hist );


/**
  Set half downsampling control data.
  @param[in]	xch							: Xch select
  @param[in]	xch_ctrl_half_thin			: half downsampling control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Ctrl_Half_Thin( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_HALF_THIN* xch_ctrl_half_thin );


/**
  Set copy control data.
  @param[in]	xch							: Xch select
  @param[in]	xch_ctrl_copy				: copy control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Ctrl_Copy( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_COPY* xch_ctrl_copy );


/**
  Set fill control data.
  @param[in]	xch							: Xch select
  @param[in]	fill_data					: fill data.
  @retval		D_IM_XCH_OK					: Success
*/
extern	INT32	Im_Xch_Ctrl_Fill( E_IM_XCH_CH_SEL xch, UCHAR fill_data );


/**
  Get common control data.
  @param[in]	xch							: Xch select
  @param[out]	xch_ctrl_comm				: common control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Get_Ctrl_Common( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_CMN* xch_ctrl_comm );


/**
  Get downsampling control data.
  @param[in]	xch							: Xch select
  @param[out]	xch_ctrl_thin				: downsampling control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Get_Ctrl_Thin( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_THIN* xch_ctrl_thin );


/**
  Get histogram control data.
  @param[in]	xch							: Xch select
  @param[out]	xch_ctrl_hist				: histogram control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @remarks		X0ch only.
*/
extern	INT32	Im_Xch_Get_Ctrl_Hist( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_HIST* xch_ctrl_hist );


/**
  Get half downsampling control data.
  @param[in]	xch							: Xch select
  @param[out]	xch_ctrl_half_thin			: half downsampling control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Get_Ctrl_Half_Thin( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_HALF_THIN* xch_ctrl_half_thin );


/**
  Get copy control data.
  @param[in]	xch							: Xch select
  @param[out]	xch_ctrl_copy				: copy control data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Get_Ctrl_Copy( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_COPY* xch_ctrl_copy );


/**
  Get fill control data.
  @param[in]	xch							: Xch select
  @param[out]	fill_data					: fill data.
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_Xch_Get_Ctrl_Fill( E_IM_XCH_CH_SEL xch, UCHAR* fill_data );


/**
  The processing of Xch begins (sync).
  @param[in]	xch							: Xch select
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_NG					: Processing error
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error
  @remarks		This API uses DDIM_User_Clr_Flg().
  @remarks		This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Im_Xch_Start_Sync( E_IM_XCH_CH_SEL xch );


/**
  The processing of Xch begins (async)
  @param[in]	xch							: Xch select
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error
  @retval		D_IM_XCH_NG					: Processing error
  @remarks		After the Xch end, user callback function is called.
  @remarks		This API uses DDIM_User_Clr_Flg().
  @remarks		This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Im_Xch_Start_Async( E_IM_XCH_CH_SEL xch );


/**
  The wait Xch end.
  @param[in]	xch							: Xch select
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error
  @retval		D_IM_XCH_NG					: Processing error
  @remarks		When it is asynchronous, it uses Xch processing and the end of Xch processing is no waited by CallBack which entered. <br>
				Please be sure to Call this API after Im_Xch_Start_Async() execution and to Wait the completion of processing.<br>
  @remarks		This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Im_Xch_Wait_End( E_IM_XCH_CH_SEL xch );


/**
  The maximum appearance frequency of the histogram is sent back.
  @return		The maximum appearance frequency
  @remarks		Either is returned on behalf of one when there are two or more steps with the same appearance frequency.<br>
				When the X0ch mode is only a histogram data acquisition mode, it is effective.<br>
				Please calls after the completion of processing.
*/
extern	UCHAR	Im_Xch_Get_Histogram_Max( VOID );


/**
  The appearance frequency in each step in the histogram buffer is sent back.
  @param[out]	histogram_buf				: histogram data buffer
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error
  @remarks		Please calls after the completion of processing.<br>
				When the X0ch mode is only a histogram data acquisition mode, it is effective.<br><br>
				 Sample of call this function.<br>
				 @code
				 VOID Sample( VOID )
				 {
					ULONG hist_buff[256];
					INT32 ret;

					ret = Im_Xch_Get_Histogram( hist_buff );
					if ( ret != D_IM_XCH_OK ) {
						// Error
					}
				 }
				 @endcode
*/
extern	INT32	Im_Xch_Get_Histogram( ULONG* histogram_buf );


/**
  The semaphore of Xch is returned.
  @param[in]	xch							: Xch select
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG
  @remarks		This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Im_Xch_Close( E_IM_XCH_CH_SEL xch );


/**
  Xch function is stopped
  @retval		D_IM_XCH_OK					: Success
  @remarks		Stop X0ch, the X1ch both.
				Issue the software reset internally.
  @remarks		This API uses DDIM_User_Dly_Tsk().
 */
extern	INT32	Im_Xch_Stop( VOID );


/**
  It is an interrupt handler that starts when the processing of Xch ends.<br>
  The end meeting flag is set.
  @remarks		This API uses DDIM_User_Set_Flg().
*/
extern	VOID	Im_Xch_Int_Handler(VOID);


/**
  The acquisition of the start control register.
  @param[in]	xch							: Xch select
  @param[out]	status						: Channel status
  @retval		D_IM_XCH_OK					: Success
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error
*/
extern	INT32	Im_Xch_Get_Xtrg( E_IM_XCH_CH_SEL xch, E_IM_XCH_ST* status );


#ifdef CO_DDIM_UTILITY_USE
/**
@name Utility Functions
@{*/

//---------------------- utility section -------------------------------
/**
  The processing of Xch Copy (sync).<br>
  @param[in]	xch							: Xch select
  @param[in]	src_addr					: copy source address
  @param[in]	src_gl_width				: copy source width
  @param[in]	dst_gl_width				: copy destination global width
  @param[in]	dst_addr					: copy destination address
  @param[in]	width						: copy area width
  @param[in]	lines						: copy area lines
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG.
  @retval		D_IM_XCH_SEM_TIMEOUT		: Semaphore acquisition TimeOut.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
  @remarks		Restriction:<br>
				&nbsp;Set at 16 byte units.<br>
				&nbsp;&nbsp;src_addr <br>
				&nbsp;&nbsp;dst_addr <br>
				&nbsp;&nbsp;src_gl_width <br>
				&nbsp;&nbsp;dst_gl_width <br>
				&nbsp;&nbsp;width <br>
				Restriction of access to the SRAM:<br>
				&nbsp;Set at 32 byte units.<br>
				&nbsp;&nbsp;dst_addr <br>
				&nbsp;Set at 64 byte units.<br>
				&nbsp;&nbsp;src_addr <br>
*/
extern	INT32	Im_Xch_Copy( E_IM_XCH_CH_SEL xch, UINT32 src_addr, USHORT src_gl_width, USHORT dst_gl_width, UINT32 dst_addr, USHORT width, USHORT lines );

/**
  The processing of Xch copy (Sync)<br>
  @param[in]	copy_param					: Parameter for copy process.
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG.
  @retval		D_IM_XCH_SEM_TIMEOUT		: Semaphore acquisition TimeOut.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
*/
extern	INT32	Im_Xch_Copy_Ex_Sync( T_IM_XCH_COPY_EX* copy_param );

/**
  The processing of Xch copy (Async)<br>
  @param[in]	copy_param					: Parameter for copy process.
  @param[in]	pCallBack					: Callback function pointer.
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
  @remarks		After the Xch end, user call back function is called.
*/
extern	INT32	Im_Xch_Copy_Ex_Async( T_IM_XCH_COPY_EX* copy_param, VP_CALLBACK pCallBack );

/**
  The processing of Xch Fill (sync).<br>
  @param[in]	xch							: Xch select
  @param[in]	fill_data					: fill data
  @param[in]	gl_width					: fill global width
  @param[in]	dst_addr					: fill destination address
  @param[in]	width						: fill area width
  @param[in]	lines						: fill area lines
  @retval		D_DDIM_OK					: Success.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG.
  @retval		D_IM_XCH_SEM_TIMEOUT		: Semaphore acquisition TimeOut.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
  @remarks		Restriction:<br>
				&nbsp;Set at 16 byte units.<br>
				&nbsp;&nbsp;gl_width.<br>
				Restriction of access to the SRAM:<br>
				&nbsp;Set at 32 byte units.<br>
				&nbsp;&nbsp;dst_addr <br>
				&nbsp;Set at 64 byte units.<br>
				&nbsp;&nbsp;src_addr <br>
*/
extern	INT32	Im_Xch_Fill( E_IM_XCH_CH_SEL xch, UCHAR fill_data, USHORT gl_width, UINT32 dst_addr, USHORT width, USHORT lines );

/**
  The processing of Xch fill (Sync)<br>
  @param[in]	fill_param					: Parameter for fill process.
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG.
  @retval		D_IM_XCH_SEM_TIMEOUT		: Semaphore acquisition TimeOut.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
*/
extern	INT32	Im_Xch_Fill_Ex_Sync( T_IM_XCH_FILL_EX* fill_param );

/**
  The processing of Xch fill (Async)<br>
  @param[in]	fill_param					: Parameter for fill process
  @param[in]	pCallBack					: Callback function pointer
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
  @remarks		After the Xch end, user call back function is called.
*/
extern	INT32	Im_Xch_Fill_Ex_Async( T_IM_XCH_FILL_EX* fill_param, VP_CALLBACK pCallBack );

/**
  The processing of Xch Histogram (Sync)<br>
  @param[in]	hist_param					: histogram necessary data
  @param[out]	hist_buffer					: histogram data buffer
  @param[out]	hist_max					: maximum appearance frequency
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG.
  @retval		D_IM_XCH_SEM_TIMEOUT		: Semaphore acquisition TimeOut.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
  @remarks		Please use it when there is no use of the X0ch mode in another.
*/
extern	INT32	Im_Xch_Examine_Histogram_Sync( T_IM_XCH_EXAMINE_HIST* hist_param, ULONG* hist_buffer, UCHAR* hist_max );

/**
  The processing of Xch Histogram (Async)<br>
  @param[in]	hist_param					: Parameter for histogram process
  @param[in]	pCallBack					: Callback function pointer
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
  @remarks		Please use it when there is no use of the X0ch mode in another.<br>
				After the Xch end, user call back function is called.
*/
extern	INT32	Im_Xch_Examine_Histogram_Async( T_IM_XCH_EXAMINE_HIST* hist_param, VP_CALLBACK pCallBack );

/**
  The processing of Xch Downsampling out (Sync)<br>
  @param[in]	thin_param					: Parameter for downsampling process
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG.
  @retval		D_IM_XCH_SEM_TIMEOUT		: Semaphore acquisition TimeOut.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
*/
extern	INT32	Im_Xch_Convert_Thinning_Sync( T_IM_XCH_CONV_THIN* thin_param );

/**
  The processing of Xch Downsampling out (Async)<br>
  @param[in]	thin_param					: Parameter for downsampling process
  @param[in]	pCallBack					: Callback function pointer
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error.
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
  @remarks		After the Xch end, user call back function is called.
*/
extern	INT32	Im_Xch_Convert_Thinning_Async( T_IM_XCH_CONV_THIN* thin_param, VP_CALLBACK pCallBack );

/**
  Copy process using the channel 0 and channel 1 (Sync)<br>
  @param[in]	dst_addr					: copy destination address
  @param[in]	src_addr					: copy source address
  @param[in]	size						: copy size
  @retval		D_IM_XCH_OK					: Success.
  @retval		D_IM_XCH_SEM_NG				: Semaphore acquisition NG.
  @retval		D_IM_XCH_SEM_TIMEOUT		: Semaphore acquisition TimeOut.
  @retval		D_IM_XCH_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_XCH_NG					: Processing error
  @retval		D_IM_XCH_MACRO_BUSY_NG		: Macro busy error.
  @remarks		Restriction:<br>
				&nbsp;Set at 16 byte units.<br>
				&nbsp;&nbsp;src_addr <br>
				&nbsp;&nbsp;dst_addr <br>
				Restriction of access to the SRAM:<br>
				&nbsp;Set at 32 byte units.<br>
				&nbsp;&nbsp;dst_addr <br>
				&nbsp;Set at 64 byte units.<br>
				&nbsp;&nbsp;src_addr <br>
  @remarks		This API uses DDIM_User_Ref_Sem().
*/
extern	INT32	Im_Xch_Copy_Dual_Channel( VOID* dst_addr, VOID* src_addr, INT32 size );

//---------------------- colabo  section -------------------------------

/*@}*/

#endif	// CO_DDIM_UTILITY_USE


#ifdef __cplusplus
}
#endif
#endif	// _IM_XCH_H_

/*@}*/

/**
@weakgroup im_xch
@{
	<hr>
	@section XCH_HIGH-SPEED_DOWNSAMPLEING Sample code for synchronism high-speed downsampling mode
	@code
	// This processing 3/4 downsampling 4000 * 3000 images.

	INT32 ret;

	T_IM_XCH_CTRL_CMN		xch_ctrl_cmn;
	T_IM_XCH_CTRL_THIN		xch_ctrl_thin;

	memset(&xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN));
	memset(&xch_ctrl_thin, 0, sizeof(T_IM_XCH_CTRL_THIN));

	xch_ctrl_cmn.mode      = D_IM_XCH_MD_DOWN_HI_SPEED;	// Opration mode
	xch_ctrl_cmn.g_hsize   = 4000;
	xch_ctrl_cmn.d_hsize   = (4000 / 4) * 3;
	xch_ctrl_cmn.d_vsize   = (3000 / 4) * 3;
	xch_ctrl_cmn.dst_addr  = 0x83000000;		// Set destination image area address
	xch_ctrl_cmn.pCallBack = NULL;

	xch_ctrl_thin.xdtype   = D_IM_XCH_XDTYPE_8;	// XDTYPE Select 8bit
	xch_ctrl_thin.hcyc     = 0x00;				// Horizontal downsampling 32 byte cycle
	xch_ctrl_thin.vcyc     = 0x00;				// Vertical downsampling 32 byte cycle
	xch_ctrl_thin.hen      = 0xBBBBBBBB;		// 3/4
	xch_ctrl_thin.ven      = 0xBBBBBBBB;		// 3/4
	xch_ctrl_thin.s_hsize  = 4000;
	xch_ctrl_thin.src_addr = 0x81000000;		// Set source image area address

	ret = Im_Xch_Open( E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_FOREVER );
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Open error ret=%d *****\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Common(E_IM_XCH_CH_SEL_0, &xch_ctrl_cmn);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Ctrl_Common error ret=%d *****\n", ret));
		ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
		if (ret != D_IM_XCH_OK) {
			Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	ret = Im_Xch_Ctrl_Thin(E_IM_XCH_CH_SEL_0, &xch_ctrl_thin);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Ctrl_Thin error ret=%d *****\n", ret));
		ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
		if (ret != D_IM_XCH_OK) {
			Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	ret = Im_Xch_Start_Sync(E_IM_XCH_CH_SEL_0);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Start_Sync error ret=%d *****\n", ret));
	}

	ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		return ret;
	}
	@endcode
	<br>
	<br>
	@section XCH_DOWNSAMPLEING_ASYNC_WAIT	Sample code for asynchronous downsampling mode (wait ver)
	@code
	// This processing 1/2 downsampling 4000 * 3000 images.

	INT32 ret;

	T_IM_XCH_CTRL_CMN		xch_ctrl_cmn;
	T_IM_XCH_CTRL_THIN		xch_ctrl_thin;

	memset(&xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN));
	memset(&xch_ctrl_thin, 0, sizeof(T_IM_XCH_CTRL_THIN));

	xch_ctrl_cmn.mode      = D_IM_XCH_MD_DOWN;	// Opration mode
	xch_ctrl_cmn.g_hsize   = 4000;
	xch_ctrl_cmn.d_hsize   = 4000 / 2;
	xch_ctrl_cmn.d_vsize   = 3000 / 2;
	xch_ctrl_cmn.dst_addr  = 0x83000000;		// Set destination image area address
	xch_ctrl_cmn.pCallBack = NULL;

	xch_ctrl_thin.xdtype   = D_IM_XCH_XDTYPE_8;	// XDTYPE Select 8bit
	xch_ctrl_thin.hcyc     = 0x00;				// Horizontal downsampling 32 byte cycle
	xch_ctrl_thin.vcyc     = 0x00;				// Vertical downsampling 32 byte cycle
	xch_ctrl_thin.hen      = 0x44444444;		// 1/2
	xch_ctrl_thin.ven      = 0x44444444;		// 1/2
	xch_ctrl_thin.s_hsize  = 4000;
	xch_ctrl_thin.src_addr = 0x81000000;		// Set source image area address

	ret = Im_Xch_Open(E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_FOREVER);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Open error ret=%d *****\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Common(E_IM_XCH_CH_SEL_0, &xch_ctrl_cmn);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Ctrl_Common error ret=%d *****\n", ret));
		ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
		if (ret != D_IM_XCH_OK) {
			Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	ret = Im_Xch_Ctrl_Thin(E_IM_XCH_CH_SEL_0, &xch_ctrl_thin);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Ctrl_Thin error ret=%d *****\n", ret));
		ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
		if (ret != D_IM_XCH_OK) {
			Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	Im_Xch_Start_Async(E_IM_XCH_CH_SEL_0);

	// The end of the asynchronization processing is waited for.
	ret = Im_Xch_Wait_End(E_IM_XCH_CH_SEL_0);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Wait_End error ret=%d *****\n", ret));
	}

	ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		return ret;
	}
	@endcode
	<br>
	<br>
	@section XCH_HISTOGRAM_ASYNC_CALLBACK	Sample code for asynchronous histogram mode (callback function ver).
	@code
	// This processing acquires the histogram data of 4000*3000 images.

	INT32 ret;
	CHAR hist_max;
	ULONG hist_buf[256];
	E_IM_XCH_ST status;

	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn;
	T_IM_XCH_CTRL_HIST	xch_ctrl_hist;

	memset(&xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN));
	memset(&xch_ctrl_hist, 0, sizeof(T_IM_XCH_CTRL_HIST));

	xch_ctrl_cmn.mode      = D_IM_XCH_MD_HISTGRAM;	// Opration mode
	xch_ctrl_cmn.g_hsize   = 4000;
	xch_ctrl_cmn.d_hsize   = 4000;
	xch_ctrl_cmn.d_vsize   = 3000;
	xch_ctrl_cmn.dst_addr  = 0x83000000;			// Set destination image area address
	xch_ctrl_cmn.pCallBack = (VP_CALLBACK)CallBackFunction;	// Set callback function address

	xch_ctrl_hist.xdtype   = D_IM_XCH_XDTYPE_8;		// XDTYPE Select 8bit
	xch_ctrl_hist.hcyc     = 0x00;					// Horizontal downsampling cycle
	xch_ctrl_hist.vcyc     = 0x00;					// Vertical downsampling cycle
	xch_ctrl_hist.hen      = 0xFFFFFFFF;
	xch_ctrl_hist.ven      = 0xFFFFFFFF;
	xch_ctrl_hist.s_hsize  = 4000;
	xch_ctrl_hist.src_addr = 0x81000000;			// Set source image area address
	xch_ctrl_hist.hcnt     = D_IM_XCH_INITIALIZE;

	ret = Im_Xch_Open(E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_FOREVER);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Open error ret=%d *****\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Common(E_IM_XCH_CH_SEL_0, &xch_ctrl_cmn);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Ctrl_Common error ret=%d *****\n", ret));
		ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
		if (ret != D_IM_XCH_OK) {
			Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	ret = Im_Xch_Ctrl_Hist(E_IM_XCH_CH_SEL_0, &xch_ctrl_hist);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Ctrl_Hist error ret=%d *****\n", ret));
		ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
		if (ret != D_IM_XCH_OK) {
			Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	Im_Xch_Start_Async(E_IM_XCH_CH_SEL_0);

	// The end of the asynchronization processing is waited for.
	// The ending flag is waited for by the callback function until setting it.
	while(!gXCH_XCH0_END_Flag){	// gXCH_XCH0_END_Flag is set by the callback function.
		DDIM_User_Dly_Tsk(10);	// wait 10mS
	}

	while(1) {
		// check macro status
		Im_Xch_Get_Xtrg(E_IM_XCH_CH_SEL_0, &status);
		if ( status == E_IM_XCH_ST_STOPPED ) {
			break;
		}
	}

	hist_max = Im_Xch_Get_Histogram_Max();

	memset(hist_buf, 0, sizeof(hist_buf));
	Im_Xch_Get_Histogram(hist_buf);

	ret = Im_Xch_Close(E_IM_XCH_CH_SEL_0);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Close error ret=%d *****\n", ret));
		return ret;
	}
	@endcode
	<br>
	<br>
	@section XCH_SIMPLICITY_COPY	Sample code for Simplicity Copy ex
	@code
	// This processing copies of 4000 * 3000 images.

	T_IM_XCH_COPY_EX xch_ctrl_copy;
	INT32 ret;

	memset(&xch_ctrl_copy, 0, sizeof(T_IM_XCH_COPY_EX));

	xch_ctrl_copy.xch               = E_IM_XCH_CH_SEL_0;
	xch_ctrl_copy.copy_type         = E_IM_XCH_COPY_SEL_HI;
	xch_ctrl_copy.size.width        = 4000;
	xch_ctrl_copy.size.lines        = 3000;
	xch_ctrl_copy.size.src_gl_width = 4000;
	xch_ctrl_copy.size.dst_gl_width = 4000;
	xch_ctrl_copy.addr.src          = 0x81000000;
	xch_ctrl_copy.addr.dst          = 0x83000000;

	ret = Im_Xch_Copy_Ex_Sync(&xch_ctrl_copy);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Im_Xch_Copy_Ex_Sync error ret=%d *****\n", ret));
		return ret;
	}
	@endcode
	<br>
	<br>
	@section XCH_SIMPLICITY_FILL	Sample code for Simplicity Fill ex
	@code
	// This processing fill of 640 * 480 area part of 4000 * 3000 images.

	INT32				ret;
	T_IM_XCH_FILL_EX	fill_param;

	fill_param.size.gl_width = 4000;
	fill_param.size.width    = 640;
	fill_param.size.lines    = 480;
	fill_param.fill_addr     = 0x83000000;
	fill_param.fill_data     = 0xFF;
	fill_param.xch           = E_IM_XCH_CH_SEL_1;

	ret = Im_Xch_Open(E_IM_XCH_CH_SEL_1, D_DDIM_WAIT_END_FOREVER);
	if (ret != D_IM_XCH_OK) {
		// Xch1 Im_Xch_Open error
		Ddim_Print(("***** Xch1 Im_Xch_Open error ret=%d *****\n", ret));
		return D_IM_XCH_NG;
	}

	ret = Im_Xch_Fill_Ex_Async(&fill_param, NULL);
	if (ret != D_IM_XCH_OK) {
		// Xch1 Im_Xch_Fill_Ex_Async error
		Im_Xch_Close(E_IM_XCH_CH_SEL_1);
		Ddim_Print(("***** Xch1 Im_Xch_Fill_Ex_Async error ret=%d *****\n", ret));
		return ret;
	}

	Im_Xch_Wait_End(E_IM_XCH_CH_SEL_1);

	ret = Im_Xch_Close(E_IM_XCH_CH_SEL_1);
	if (ret != D_IM_XCH_OK) {
		// Xch1 Im_Xch_Close error
		Ddim_Print(("***** Xch1 Im_Xch_Close error ret=%d *****\n", ret));
		return ret;
	}
	@endcode
	<br>
	<br>
	@section XCH_MEMCPY		Sample code for memcpy substitute using Xch Copy
	@code
	// This processing copy of 0x2000byte.

	INT32		ret;
	INT32		size;
	ULONG		src_addr = 0x81000000;
	ULONG		dst_addr = 0x83000000;

	size	= 0x2000;

	ret = Im_Xch_Copy_Dual_Channel((VOID*)dst_addr, (VOID*)src_addr, size);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("***** Xch Im_Xch_Copy_Dual_Channel error ret=%d *****\n", ret));
		return D_IM_XCH_NG;
	}
	@endcode
	<br>
	<br>
	@section XCH_SIMPLICITY_HISTOGRAM		Sample code for Simplicity Histogram ex
	@code
	// This processing acquires the histogram data of 4000 * 3000 images.

	INT32					ret;
	T_IM_XCH_EXAMINE_HIST	hist_param;
	ULONG					hist_buf[256];
	UCHAR					hist_max;

	hist_param.size.width = 4000;
	hist_param.size.lines = 3000;
	hist_param.hist_addr  = 0x81000000;

	ret = Im_Xch_Examine_Histogram_Sync(&hist_param, hist_buf, &hist_max);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Xch0 Im_Xch_Examine_Histogram_Sync error ret=%d *****\n", ret));
	}

	return ret;

	@endcode
	<br>
	<br>
	@section XCH_SIMPLICITY_DOWNSAMPLEING		Sample code for Simplicity Downsampling ex
	@code
	// This processing downsampling width 3/4, lines 1/2 of 4000 * 3000 images.

	INT32				ret;
	T_IM_XCH_CONV_THIN	thin_param;

	thin_param.xch               = E_IM_XCH_CH_SEL_0;
	thin_param.size.src_gl_width = 4000;
	thin_param.size.dst_gl_width = 4000;
	thin_param.size.width        = 4000;
	thin_param.size.lines        = 3000;
	thin_param.addr.src          = 0x81000000;
	thin_param.addr.dst          = 0x83000000;

	thin_param.thin_width        = D_IM_XCH_THIN_SEL_3_4;
	thin_param.thin_lines        = D_IM_XCH_THIN_SEL_1_2;

	// Xch start
	ret = Im_Xch_Convert_Thinning_Sync(&thin_param);
	if (ret != D_IM_XCH_OK) {
		Ddim_Print(("***** Xch0 Im_Xch_Convert_Thinning_Sync error ret=%d *****\n", ret));
		return ret;
	}
	@endcode
@}*/

/* end of im_xch */
