/**
 * @file        ipc_hdrprm.h
 * @brief       IPC Still-HDR Library
 * @author      tanabata
 * @since       2016/05/17
 * @note        None
 * @attention   None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#ifndef _IPC_HDRPRM_H_
#define _IPC_HDRPRM_H_

#include "ipc_if.h"

/*--------------------------------------------------------------------------------------*/
/* Definition                                                                           */
/*--------------------------------------------------------------------------------------*/

/**
@name Returned value
@{
*/
#define D_IPC_HDRPRM_OK           (D_IPC_OK)                        /**< OK */
#define D_IPC_HDRPRM_NG           (D_IPC_NG|0x00220000)             /**< NG */
#define D_IPC_HDRPRM_NG_OPEN      (D_IPC_HDRPRM_NG|0x00000001)      /**< Open       Error */
#define D_IPC_HDRPRM_NG_SEQ       (D_IPC_HDRPRM_NG|0x00000002)      /**< Sequuence  Error */
#define D_IPC_HDRPRM_NG_PARAM     (D_IPC_HDRPRM_NG|0x00000004)      /**< Parameter  Error */
#define D_IPC_HDRPRM_NG_MEMORY    (D_IPC_HDRPRM_NG|0x00000040)      /**< Momory     Error */
/** @} */

/**
@name HDRPRM Macro Register Elements Number
@{
*/
#define D_IPC_HDRPRM_BLDOFS_NUM   7  /**< Number of bldofs  registers */
#define D_IPC_HDRPRM_BLDSL_NUM    7  /**< Number of bldsl   registers */
#define D_IPC_HDRPRM_BLDB_NUM     7  /**< Number of bldb    registers */
#define D_IPC_HDRPRM_EVKNB_NUM   10  /**< Number of evknb   registers */
#define D_IPC_HDRPRM_EVKNOFS_NUM 10  /**< Number of evknofs registers */
#define D_IPC_HDRPRM_EVKNSL_NUM  10  /**< Number of evknsl  registers */
/** @} */

/*--------------------------------------------------------------------------------------*/
/* Enumeration                                                                          */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure                                                                            */
/*--------------------------------------------------------------------------------------*/
/**
@structure T_IPC_HDRPRM_EXPO_PARAM
@brief The T_IPC_HDRPRM_EXPO_PARAM structure defines exposure condition between
long exposure image and short exposure image.
*/
typedef struct _T_IPC_HDRPRM_EXPO_PARAM {
    IPC_FLOAT expo_diff;
    IPC_FLOAT expo_ofs;
    IPC_INT ext_mode;
    IPC_VOID* ext_data;
} T_IPC_HDRPRM_EXPO_PARAM;


/**
@structure T_IPC_HDRPRM_HIST
@brief The T_IPC_HDRPRM_HIST structure defines image histogram.
*/
typedef struct _T_IPC_HDRPRM_HIST {
    IPC_UINT bin_num;
    IPC_UINT freq_bit_depth;
    IPC_VOID* addr;
} T_IPC_HDRPRM_HIST;

/**
@structure T_IPC_HDRPRM_BLD_TBL
@brief The T_IPC_HDRPRM_BLD_TBL structure defines the bldofs, bldsl, bldb.
*/
typedef struct _T_IPC_HDRPRM_BLD_TBL {
    IPC_USHORT bldofs[D_IPC_HDRPRM_BLDOFS_NUM];
    IPC_USHORT bldsl[D_IPC_HDRPRM_BLDSL_NUM]; 
    IPC_USHORT bldb[D_IPC_HDRPRM_BLDB_NUM];
} T_IPC_HDRPRM_BLD_TBL;

/**
@structure T_IPC_HDRPRM_EVC_TBL
@brief The T_IPC_HDRPRM_EVC_TBL structure defines the evknb, evknofs, evkns, gainup tbl.
*/
typedef struct _T_IPC_HDRPRM_EVC_TBL {
    IPC_UINT postmulqpos;
    IPC_USHORT evknb[D_IPC_HDRPRM_EVKNB_NUM];
    IPC_USHORT evknofs[D_IPC_HDRPRM_EVKNOFS_NUM];
    IPC_SHORT evknsl[D_IPC_HDRPRM_EVKNSL_NUM];
} T_IPC_HDRPRM_EVC_TBL;

/*--------------------------------------------------------------------------------------*/
/* Global Data                                                                          */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Macro                                                                                */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function                                                                             */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API                                                                                  */
/*--------------------------------------------------------------------------------------*/
/**
Opens the HDRPRM library.
*/
IPC_INT IPC_HDRPRM_Open(IPC_VOID);
/**
Initializes the HDRPRM library.
*/
IPC_INT IPC_HDRPRM_Init(
    IPC_UINT heap_addr,
    IPC_UINT* heap_size,
    IPC_UINT img_w, IPC_UINT img_h,
    IPC_INT gamma_tbl_type, IPC_USHORT* gamma_tbl);
/**
Control the HDRPRM library.
*/
IPC_INT IPC_HDRPRM_Ctrl(IPC_VOID);
/**
Set the static parameters.
*/
IPC_INT IPC_HDRPRM_Get_Luma_Compensation_Param(
    IPC_UCHAR th_y,
    IPC_INT calc_mode,
    T_IPC_HDRPRM_EXPO_PARAM* expo_param,
    T_IPC_IMAGE* img_lo,
    T_IPC_IMAGE* img_sh,
    IPC_UCHAR* lut_lo2sh,
    IPC_UCHAR* lut_sh2lo
    );
/**
Get the HDRPRM blend-table.
*/
IPC_INT IPC_HDRPRM_Get_Blend_Table(
    IPC_UCHAR th_y, IPC_UCHAR* lc_lut_lo2sh,
    T_IPC_HDRPRM_HIST* hist_lo, T_IPC_HDRPRM_HIST* hist_sh,
    IPC_INT calc_mode, IPC_UCHAR min_bld_ratio,
    T_IPC_HDRPRM_BLD_TBL* bld_tbl);
/**
Get the HDRPRM EV Compensation parameters.
*/
IPC_INT IPC_HDRPRM_Get_EVC_Table(
    IPC_UCHAR th_y, IPC_UCHAR* lc_lut_lo2sh,
    T_IPC_HDRPRM_HIST* hist_lo, T_IPC_HDRPRM_HIST* hist_sh,
    IPC_INT calc_mode, IPC_INT type,
    T_IPC_HDRPRM_BLD_TBL* bld_tbl, T_IPC_HDRPRM_EVC_TBL* evc_tbl);
/**
Get the HDRPRM Tone Table for Y2R.
*/
IPC_INT IPC_HDRPRM_Get_Tone_Table(
    T_IPC_HDRPRM_HIST* hist,
    IPC_INT calc_mode, IPC_INT type,
    IPC_UCHAR* tcintbit,
    IPC_USHORT* tc);
/**
Closes the HDRPRM library.
*/
IPC_INT IPC_HDRPRM_Close(IPC_VOID);
/**
Get the HDRPRM library version.
*/
IPC_VOID IPC_HDRPRM_Get_Version(IPC_CHAR** str);

/*--------------------------------------------------------------------------------------*/
/* Version                                                                              */
/*--------------------------------------------------------------------------------------*/
#define V_Ipc_HdrPrm_Version              "slib_version:V01L00;\r\n" /**< Library version */

#endif /* _IPC_HDRPRM_H_ */

/* V01L00 2017.03.13 IPC_HDRPRM ver1.0 */
