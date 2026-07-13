/**
 * @file        ipc_shdr.h
 * @brief       IPC Still-HDR Library
 * @author      tanabata
 * @since       2016/05/17
 * @note        None
 * @attention   None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2016</I></B>
 */

#ifndef _IPC_SHDR_H_
#define _IPC_SHDR_H_

#include "ipc_if.h"

/*--------------------------------------------------------------------------------------*/
/* Definition                                                                           */
/*--------------------------------------------------------------------------------------*/

/**
@name Returned value
@{
*/
#define D_IPC_SHDR_OK           (D_IPC_OK)                      /**< OK */
#define D_IPC_SHDR_NG           (D_IPC_NG|0x000A0000)           /**< NG */
#define D_IPC_SHDR_NG_OPEN      (D_IPC_SHDR_NG|0x00000001)      /**< Open       Error */
#define D_IPC_SHDR_NG_SEQ       (D_IPC_SHDR_NG|0x00000002)      /**< Sequuence  Error */
#define D_IPC_SHDR_NG_PARAM     (D_IPC_SHDR_NG|0x00000004)      /**< Parameter  Error */
/** @} */       // name

// parameter for SHDR Hardware register num
#define D_IPC_SHDR_BLDOFS_NUM 7
#define D_IPC_SHDR_BLDSL_NUM 7
#define D_IPC_SHDR_BLDB_NUM 7
#define D_IPC_SHDR_EVKNB_NUM 10
#define D_IPC_SHDR_EVKNOFS_NUM 10
#define D_IPC_SHDR_EVKNSL_NUM 10

/*--------------------------------------------------------------------------------------*/
/* Enumeration                                                                          */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure                                                                            */
/*--------------------------------------------------------------------------------------*/
/**
The T_IPC_SHDR_HIST structure defines image histogram.
*/
typedef struct _T_IPC_SHDR_HIST {
    IPC_UINT bin_num;
    IPC_UINT freq_bit_depth;
    IPC_VOID* addr;
} T_IPC_SHDR_HIST;

/**
The T_IPC_SHDR_EVC_TBL structure defines the bldofs, bldsl, bldb.
*/
typedef struct _T_IPC_SHDR_BLD_TBL {
    IPC_USHORT bldofs[D_IPC_SHDR_BLDOFS_NUM];
    IPC_USHORT bldsl[D_IPC_SHDR_BLDSL_NUM]; 
    IPC_USHORT bldb[D_IPC_SHDR_BLDB_NUM];
} T_IPC_SHDR_BLD_TBL;

/**
The T_IPC_SHDR_EVC_TBL structure defines the evknb, evknofs, evkns, gainup tbl.
*/
typedef struct _T_IPC_SHDR_EVC_TBL {
    IPC_USHORT evknb[D_IPC_SHDR_EVKNB_NUM];
    IPC_USHORT evknofs[D_IPC_SHDR_EVKNOFS_NUM];
    IPC_SHORT evknsl[D_IPC_SHDR_EVKNSL_NUM];
} T_IPC_SHDR_EVC_TBL;

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
Opens the SHDR library.
*/
IPC_INT IPC_SHDR_Open(IPC_VOID);
/**
Initializes the SHDR library.
*/
IPC_INT IPC_SHDR_Init(
    IPC_UINT heap_addr,
    IPC_INT img_w, IPC_INT img_h,
    IPC_INT gamma_tbl_type, IPC_USHORT* gamma_tbl);
/**
Set the static parameters.
*/
IPC_INT IPC_SHDR_Get_Luma_Compensation_Param(
    IPC_UINT heap_addr,
    T_IPC_IMAGE* img_lo,
    T_IPC_IMAGE* img_sh,
    IPC_UCHAR th_y,
    IPC_UCHAR* lut_lo2sh,
    IPC_UCHAR* lut_sh2lo
    );
/**
Gets the SHDR blend-table.
*/
IPC_INT IPC_SHDR_Get_Blend_Table(
    IPC_UINT heap_addr,
    T_IPC_SHDR_HIST* hist_lo, T_IPC_SHDR_HIST* hist_sh,
    IPC_INT calc_mode, T_IPC_SHDR_BLD_TBL* bld_tbl);
/**
Gets the SHDR EV Compensation parameters.
*/
IPC_INT IPC_SHDR_Get_EVC_Table(
    IPC_UINT heap_addr,
    T_IPC_SHDR_HIST* hist_lo, T_IPC_SHDR_HIST* hist_sh,
    IPC_INT calc_mode,
    T_IPC_SHDR_BLD_TBL* bld_tbl, T_IPC_SHDR_EVC_TBL* evc_tbl);
/**
Gets the SHDR Tone Table for Y2R.
*/
IPC_INT IPC_SHDR_Get_Tone_Table(
    IPC_UINT heap_addr,
    T_IPC_SHDR_HIST* hist,
    IPC_INT calc_mode,
    IPC_UCHAR* tcintbit,
    IPC_USHORT* tc);
/**
Closes the SHDR library.
*/
IPC_INT IPC_SHDR_Close(IPC_VOID);
/**
Gets the SHDR library version.
*/
IPC_VOID IPC_SHDR_Get_Version(IPC_CHAR** str);

/*--------------------------------------------------------------------------------------*/
/* Version                                                                              */
/*--------------------------------------------------------------------------------------*/
#define V_Ipc_Shdr_Version              "flib_version:V00L00;\r\n"

#endif /* _IPC_SHDR_H_ */

/* V00L00 2016.05.19 IPC_SHDR ver0.1 */
