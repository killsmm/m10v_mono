/**
 * @file		ipc_vhdr.h
 * @brief		IPC_VHDR Header
 * @author		yamauchi
 * @since		2017/03/23
 * @date		
 * @note		IPC_VHDR Header
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2017</I></B>
 */

#ifndef _IPC_VHDR_H_
#define _IPC_VHDR_H_

//#define	CO_ES3_HARDWARE

#include "ddim_typedef.h"
#include "ipc_if.h"
#include "im_shdr.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/

#define D_IPC_VHDR_BLDOFS_NUM   7  /**< Number of bldofs  registers */
#define D_IPC_VHDR_BLDSL_NUM    7  /**< Number of bldsl   registers */
#define D_IPC_VHDR_BLDB_NUM     7  /**< Number of bldb    registers */
#define D_IPC_VHDR_EVKNB_NUM   10  /**< Number of evknb   registers */
#define D_IPC_VHDR_EVKNOFS_NUM 10  /**< Number of evknofs registers */
#define D_IPC_VHDR_EVKNSL_NUM  10  /**< Number of evknsl  registers */


/**
   @name Returned value
   @{
*/
#define D_IPC_VHDR_OK				(D_IPC_OK)					/**< OK */
#define D_IPC_VHDR_NG				(D_IPC_NG|0x00190000)		/**< NG */
#define D_IPC_VHDR_NG_ARG			(D_IPC_VHDR_NG|0x00000001)	/**< Argument  Error		*/
#define D_IPC_VHDR_NG_OPEN			(D_IPC_VHDR_NG|0x00000002)	/**< Open      Error		*/
#define D_IPC_VHDR_NG_CLOSE			(D_IPC_VHDR_NG|0x00000003)	/**< Close     Error		*/
#define D_IPC_VHDR_NG_SEQ			(D_IPC_VHDR_NG|0x00000004)	/**< Sequence  Error		*/
#define D_IPC_VHDR_NG_FORMAT		(D_IPC_VHDR_NG|0x00000005)	/**< Format    Error		*/
#define D_IPC_VHDR_NG_SIZE			(D_IPC_VHDR_NG|0x00000006)	/**< Size      Error		*/
#define D_IPC_VHDR_NG_PARAM			(D_IPC_VHDR_NG|0x00000007)	/**< Parameter Error		*/
#define D_IPC_VHDR_NG_MEMORY		(D_IPC_VHDR_NG|0x00000008)	/**< Memory    Error		*/
#define D_IPC_VHDR_NG_HW			(D_IPC_VHDR_NG|0x00000010)	/**< HW        Error		*/
#define D_IPC_VHDR_NG_UNKNOWN		(D_IPC_VHDR_NG|0x00008000)	/**< Unknown   Error		*/

#define D_IPC_VHDR_NG_TO			(-2)			/**< Error(Time out). 		*/
/** @} */	// name


/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/
/** filter margin. */
typedef enum {
	E_IPC_VHDR_FIL_MARGIN_16 = 0,				/**< 16 pixel.	*/
	E_IPC_VHDR_FIL_MARGIN_32,					/**< 32 pixel.	*/
} E_IPC_VHDR_FIL_MARGIN;

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

typedef struct	 {
    IPC_UINT bin_num;
    IPC_UINT freq_bit_depth;
    IPC_VOID* addr;
} T_IPC_VHDR_HIST;


typedef struct T_IPC_VHDR_BLD_TBL {
    IPC_USHORT bldofs[D_IPC_VHDR_BLDOFS_NUM];
    IPC_USHORT bldsl[D_IPC_VHDR_BLDSL_NUM]; 
    IPC_USHORT bldb[D_IPC_VHDR_BLDB_NUM];
} T_IPC_VHDR_BLD_TBL;

/** VHDR start parameter. */
typedef struct {
	BOOL					bypassMode;				/**< [In]Bypass mode(True:On/False:Off).											*/
	BOOL					compMode;				/**< [In]Compression mode(True:Compresssion/False:Decompression).					*/
	BOOL					edgeCpy;				/**< [In]Edge copy(True:On/False:Off).												*/
	BOOL					edgeCut;				/**< [In]Edge cut(True:On/False:Off).												*/
	BOOL					brcEnable;				/**< [In]Brightness correct enable(True:enable/False:disable).						*/
	BOOL					brcMode;				/**< [In]Brightness correct mode(True:for Reference image/False:for Source image).	*/
	E_IPC_VHDR_FIL_MARGIN	filMargin;				/**< [In]Filter margin(16/32 pixels).												*/
	T_IPC_IMAGE				imgShort[ 3 ];			/**< [In]Short exposure image.<br>
																- Image address is 8byte align.
															<ul>
																<li>[0]:YUV8         from SGDE/R2Y.
																<li>[1]:Y8 1/4       from SGDE/R2Y.
																<li>[2]:Y8 reduction from SGDE/R2Y (only GMD_FPT mode).
															</ul>
														*/
	T_IPC_IMAGE				imgLong[ 3 ];			/**< [In]Long exposure image.<br>
																- Don't care of this parameter in Bypass mode.<br>
																- Image address is 8byte align.
															<ul>
																<li>[0]:YUV8         from SGDE/R2Y.
																<li>[1]:Y8 1/4       from SGDE/R2Y.
																<li>[2]:Y8 reduction from SGDE/R2Y (only GMD_FPT mode).
															</ul>
														*/
	T_IPC_IMAGE				imgMe;					/**< [Out]Vector data.<br>
																- Don't care of this parameter in Bypass mode.<br>
																- Image address is 8byte align.
														*/
	T_IPC_IMAGE				imgDst[ 6 ];			/**< [Out]Destination image.<br>
															<ul>
																<li>[0]:HEVC enc image(compression/decompression).
																<li>[1]:Me input/VHDR input reduction image (unused).
																<li>[2]:H264 enc image(HD).
																<li>[3]:H264 enc image(VGA).
																<li>[4]:Display image(HDMI).
																<li>[5]:Display image(LCD).
															</ul>
														*/
	T_IPC_IMAGE				cmpTbl[ 2 ];			/**< [In/Out]Comp table.<br>
															<ul>
																<li>[0]:Comp table(source).
																<li>[1]:Comp table(reference).
															</ul>
														*/
	SHORT					reserve0;				/**< [In]EIS shift x.											*/
	SHORT					reserve1;				/**< [In]EIS shift y.											*/
} T_IPC_VHDR_START_PRM;


/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function  																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API		  																			*/
/*--------------------------------------------------------------------------------------*/
/**
	Opens the VHDR library.
*/
extern IPC_INT IPC_VHDR_Open( IPC_VOID );
/**
	Initializes the VHDR library.
*/
extern	IPC_INT IPC_VHDR_Init(
					IPC_UINT heap_addr, IPC_UINT* heap_size,
					IPC_UINT img_w, IPC_UINT img_h,
					IPC_INT gamma_tbl_type, IPC_USHORT* gamma_tbl);
/**
	Set the static parameters.
*/
extern	IPC_INT IPC_VHDR_Calc_Blend_Table(
					IPC_UCHAR th_y, IPC_UCHAR* lc_lut_lo2sh,
					T_IPC_VHDR_HIST* hist_lo, T_IPC_VHDR_HIST* hist_sh,
					IPC_INT calc_mode, IPC_UCHAR min_bld_ratio,
					T_IPC_VHDR_BLD_TBL* bld_tbl );

extern	IPC_INT	IPC_VHDR_Start( 
					T_IPC_VHDR_START_PRM *startPrm,
					IPC_BOOL iq_update, IPC_SHORT *evc, T_IM_SHDR_CALLBACK func_cb );


#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
extern	IPC_VOID IPC_VHDR_Get_RegTbl( T_IM_SHDR_CTRL** shdr_ctrl, T_IM_SHDR_PRE** shdr_pre,
										T_IM_SHDR_MSK** shdr_msk,
										T_IM_SHDR_BLD** shdr_bld, T_IM_SHDR_POST** shdr_post,
										UINT32** shdr_in );
#elif defined(CO_IQBIN_KUBOTA)	// --- REMOVE_IQBIN_KUBOTA ELSEIF ---
extern	IPC_VOID IPC_VHDR_Get_RegTbl( T_IM_SHDR_CTRL** shdr_ctrl, T_IM_SHDR_PRE** shdr_pre,
										T_IM_SHDR_FMD** shdr_fmd, T_IM_SHDR_SPNR** shdr_spnr,
										T_IM_SHDR_MSK** shdr_msk );
#endif

extern IPC_INT IPC_VHDR_Close(IPC_VOID);
/**
	Gets the VHDR library version.
*/
extern IPC_VOID IPC_VHDR_Get_Version(IPC_CHAR** str);

/*--------------------------------------------------------------------------------------*/
/* Version  																			*/
/*--------------------------------------------------------------------------------------*/
#define V_Ipc_Vhdr_Version		"flib_version:V00L00;\r\n"

#endif	// _IPC_VHDR_H_

/* V00L00 2017.03.23 IPC_VHDR ver0.0 */

