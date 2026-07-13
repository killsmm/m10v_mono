/**
 * @file		ipc_gmd.h
 * @brief		IPC_GMD Header
 * @author		sano
 * @since		2016/05/11
 * @date		
 * @note		IPC_GMD Header
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2016</I></B>
 */

#ifndef _IPC_GMD_H_
#define _IPC_GMD_H_

#include "ipc_if.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/

/**
   @name Returned value
   @{
*/
#define D_IPC_GMD_OK					(D_IPC_OK)					/**< OK */
#define D_IPC_GMD_NG					(D_IPC_NG|0x001A0000)		/**< NG */
#define D_IPC_GMD_NG_OPEN				(D_IPC_GMD_NG|0x00000001)	/**< Open       Error		*/
#define D_IPC_GMD_NG_SEQ				(D_IPC_GMD_NG|0x00000002)	/**< Sequence   Error		*/
#define D_IPC_GMD_NG_PARAM				(D_IPC_GMD_NG|0x00000004)	/**< Parameter  Error		*/
#define D_IPC_GMD_NG_MEMORY				(D_IPC_GMD_NG|0x00000008)	/**< Memory     Error		*/
#define D_IPC_GMD_NG_RANGE_OVER			(D_IPC_GMD_NG|0x00000010)	/**< RANGE      Error		*/
#define D_IPC_GMD_NG_POINT_SHORTAGE		(D_IPC_GMD_NG|0x00000020)	/**< POINT      Error		*/
#define D_IPC_GMD_NG_DETECT				(D_IPC_GMD_NG|0x00000040)	/**< DETECT     Error		*/
#define D_IPC_GMD_NG_HW_MACRO			(D_IPC_GMD_NG|0x00000080)	/**< Hw_Macro   Error		*/
/** @} */	// name

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

/** 
   The T_IPC_GMD_EXP_PRM structure defines the exposure difference alignment, brc_en, brc_mod,
   lut_sel, gainup tbl0, gainup tbl1 setup.
 */
typedef struct _T_IPC_GMD_EXP_PRM{
	IPC_UCHAR  brc_en;
	IPC_UCHAR  brc_mod;
	IPC_UCHAR  lut_sel;
	IPC_UCHAR* gainup_tbl0;
	IPC_UCHAR* gainup_tbl1;
}T_IPC_GMD_EXP_PRM;

/** 
   The T_IPC_GMD_CTRL_PRM structure defines the heap address, base image, target image.
 */
typedef struct _T_IPC_GMD_CTRL_PRM{
	IPC_ULONG   heap_addr;
	IPC_ULONG   width;
	IPC_ULONG   height;
	IPC_ULONG   full_width;
	IPC_ULONG   full_height;
}T_IPC_GMD_CTRL_PRM;

/** 
   The T_IPC_GMD_START_PRM structure defines the output global motion amount in the x-direction,
   output global motion amount in the y-direction, affin parameter, permissible options.
 */
typedef struct _T_IPC_GMD_START_PRM{
	IPC_INT     shift_x;
	IPC_INT     shift_y;
	IPC_FLOAT	affin_a;
	IPC_FLOAT	affin_b;
	IPC_FLOAT	affin_c;
	IPC_FLOAT	affin_d;
	IPC_FLOAT	affin_e;
	IPC_FLOAT	affin_f;
	IPC_UCHAR	sel_affin;
	IPC_UCHAR	shift_permissible_range;
	IPC_UCHAR	affin_permissible_degree;
	IPC_UCHAR	affin_permissible_expand;
	T_IPC_IMAGE base_img;
	T_IPC_IMAGE target_img;
	T_IPC_IMAGE base_full_img;
	T_IPC_IMAGE target_full_img;
}T_IPC_GMD_START_PRM;


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
	Opens the GMD library.
*/
extern IPC_INT IPC_GMD_Open(IPC_VOID);
/**
	Initializes the GMD library.
*/
extern IPC_INT IPC_GMD_Init(IPC_VOID);
/**
	Set the static parameters.
*/
extern IPC_INT IPC_GMD_Ctrl(IPC_ULONG* heapSize, T_IPC_GMD_CTRL_PRM* ctrl_prm);
/**
	Starts the stabilizer process.
*/
extern IPC_INT IPC_GMD_Start(T_IPC_GMD_START_PRM* start_prm, T_IPC_GMD_EXP_PRM* exp_prm);
/**
	Closes the GMD library.
*/
extern IPC_INT IPC_GMD_Close(IPC_VOID);
/**
	Gets the GMD library version.
*/
extern IPC_VOID IPC_GMD_Get_Version(IPC_CHAR** str);

/*--------------------------------------------------------------------------------------*/
/* Version  																			*/
/*--------------------------------------------------------------------------------------*/
#define V_Ipc_Gmd_Version		"flib_version:V01L02;\r\n"

#endif	// _IPC_GMD_H_

/* V01L02 2016.09.02 IPC_GMD ver1.2 */
/* V01L01 2016.07.12 IPC_GMD ver1.1 */
/* V01L00 2016.07.08 IPC_GMD ver1.0 */
/* V00L04 2016.06.30 IPC_GMD ver0.4 */
/* V00L03 2016.06.07 IPC_GMD ver0.3 */
/* V00L02 2016.06.02 IPC_GMD ver0.2 */
/* V00L01 2016.05.11 IPC_GMD ver0.1 */
