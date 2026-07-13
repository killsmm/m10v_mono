/**
 * @file		im_iip_param.h
 * @brief		Header file of im_iip_param.c
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
   @weakgroup im_iip_param
   @{

   @section im_iip_param_overview Overview
   IIP SDRAM parameter<br>
   <hr>
   @section im_iip_param_unit_param Unit parameter

   IIP loads unit parameter in packet. <br>
   Packet has header part and data part.<br>
   Data part has the value of registers which the unit has.<br>
   These followning structure has several packets and all registers.<br>
   If user requires the minimum unit parameter, user must configure the unit parameter without the following structures.<br>
   <br>
   @ref T_IM_IIP_PARAM_1DL : 1D Loader <br>
   @ref T_IM_IIP_PARAM_FLT : SPF <br>
   @ref T_IM_IIP_PARAM_AFN : AFN <br>
   @ref T_IM_IIP_PARAM_FRECT : FRECT <br>
   @ref T_IM_IIP_PARAM_MON : Monitor <br>
   @ref T_IM_IIP_PARAM_STS : Store <br>
   @ref T_IM_IIP_PARAM_CSC : CSC <br>
   @ref T_IM_IIP_PARAM_LUT : LUT <br>
   @ref T_IM_IIP_PARAM_GPC : GPC <br>
   @ref T_IM_IIP_PARAM_BLEND : Blend <br>
   @ref T_IM_IIP_PARAM_CFLOW : Cache flow controller <br>
   <hr>
   @section im_iip_param_afn_struct_param AFN struct parameter
   <br>
   @ref T_IM_IIP_PARAM_AFN_OPCOL_0 <br>
   @ref T_IM_IIP_PARAM_AFN_OPCOL_1 <br>
   <hr>
   @section im_iip_param_frect_struct_param FRECT struct parameter
   <br>
   @ref T_IM_IIP_PARAM_FRECT_OPCOL_0 <br>
   @ref T_IM_IIP_PARAM_FRECT_OPCOL_1 <br>
   <hr>
   @section im_iip_param_gpc_struct_param GPC struct parameter
   <br>
   @ref T_IM_IIP_PARAM_GPC_SEQ_CTRL <br>
   @ref T_IM_IIP_PARAM_GPC_PRECAL <br>
   @ref T_IM_IIP_PARAM_GPC_CALC <br>
   @ref T_IM_IIP_PARAM_GPC_ALPHA <br>
   @ref T_IM_IIP_PARAM_GPC_INTER_COMP_CALC <br>
*/
/** @} */	// end of im_iip_param group

#ifndef _IM_IIP_PARAM_H_
#define	_IM_IIP_PARAM_H_

#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_IM_IIP_PARAM_AFN_VAL_1_0		(0x10000L)	/**< "1.0" value of U_IM_IIP_PARAM_AFN_STAXY and U_IM_IIP_PARAM_AFN_D_OUT_IN */
#define D_IM_IIP_PARAM_FRECT_VAL_1_0	(0x100L)	/**< "1.0" value of U_IM_IIP_PARAM_FRECT_XY */

#define M_IM_IIP_AFNTBL_CALC_POS(pos)	((ULONG)((pos) * D_IM_IIP_PARAM_AFN_VAL_1_0))		/**< Convert float type pixel position to U_IM_IIP_PARAM_AFN_STAXY and U_IM_IIP_PARAM_AFN_D_OUT_IN register value. */
#define M_IM_IIP_FRECTTBL_CALC_POS(pos)	((ULONG)((pos) * D_IM_IIP_PARAM_FRECT_VAL_1_0))		/**< Convert float type pixel position to U_IM_IIP_PARAM_FRECT_XY register value. */



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/** Control code of packet header */
typedef enum {
	E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY = 0x11,			/**< Load packet. And continue next parameter. */
	E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET = 0x01,		/**< Load packet and Execute Unit. (Last Packet header) */
	E_IM_IIP_PARAM_CTL_CODE_EXE = 0x02,					/**< Load packet and Execute Unit. And continue next parameter. */
	E_IM_IIP_PARAM_CTL_CODE_EXE_NEXT_CHAIN = 0x03,		/**< Load packet and Execute Unit. Chain next unit after executing when chain mode enable and next parameter nothing. */
} E_IM_IIP_PARAM_CTL_CODE;

/** PORT ID */
typedef enum {
	E_IM_IIP_PARAM_PORTID_LD0			 = 0,			/**< Loader 0			*/
	E_IM_IIP_PARAM_PORTID_LD1			 = 1,			/**< Loader 1			*/
	E_IM_IIP_PARAM_PORTID_LD2			 = 2,			/**< Loader 2			*/
	E_IM_IIP_PARAM_PORTID_LD3			 = 3,			/**< Loader 2			*/
	E_IM_IIP_PARAM_PORTID_FLT_P0		 = 4,			/**< FLT Port 0			*/
	E_IM_IIP_PARAM_PORTID_FLT_P1		 = 5,			/**< FLT Port 1			*/
	E_IM_IIP_PARAM_PORTID_AFN0			 = 6,			/**< AFN 0				*/
	E_IM_IIP_PARAM_PORTID_AFN1			 = 7,			/**< AFN 1				*/
	E_IM_IIP_PARAM_PORTID_AFN2			 = 8,			/**< AFN 2				*/
	E_IM_IIP_PARAM_PORTID_FRECT0		 = 9,			/**< FRECT 0			*/
	E_IM_IIP_PARAM_PORTID_SL0			 = 10,			/**< Store 0			*/
	E_IM_IIP_PARAM_PORTID_SL1			 = 12,			/**< Store 1			*/
	E_IM_IIP_PARAM_PORTID_SL2			 = 14,			/**< Store 2			*/
	E_IM_IIP_PARAM_PORTID_SL3			 = 16,			/**< Store 3			*/
	E_IM_IIP_PARAM_PORTID_SL4			 = 18,			/**< Store 4			*/
	E_IM_IIP_PARAM_PORTID_SL5			 = 20,			/**< Store 5			*/
	E_IM_IIP_PARAM_PORTID_SL6			 = 22,			/**< Store 6			*/
	E_IM_IIP_PARAM_PORTID_FRECT1		 = 24,			/**< FRECT 1			*/
	E_IM_IIP_PARAM_PORTID_SL7			 = 26,			/**< Store 6			*/
	E_IM_IIP_PARAM_PORTID_SL8			 = 28,			/**< Store 6			*/
	E_IM_IIP_PARAM_PORTID_CSC0			 = 32,			/**< CSC 0				*/
	E_IM_IIP_PARAM_PORTID_CSC1			 = 33,			/**< CSC 1				*/
	E_IM_IIP_PARAM_PORTID_LUT_P0		 = 34,			/**< LUT Port 0			*/
	E_IM_IIP_PARAM_PORTID_LUT_P1		 = 35,			/**< LUT Port 1			*/
	E_IM_IIP_PARAM_PORTID_LUT_P2		 = 36,			/**< LUT Port 2			*/
	E_IM_IIP_PARAM_PORTID_LUT_P3		 = 37,			/**< LUT Port 3			*/
	E_IM_IIP_PARAM_PORTID_GPC_P0		 = 38,			/**< GPC Port 0			*/
	E_IM_IIP_PARAM_PORTID_GPC_P1		 = 39,			/**< GPC Port 1			*/
	E_IM_IIP_PARAM_PORTID_BLEND0_P0		 = 40,			/**< Blend 0 Port 0		*/
	E_IM_IIP_PARAM_PORTID_BLEND0_P1		 = 41,			/**< Blend 0 Port 1		*/
	E_IM_IIP_PARAM_PORTID_BLEND0_P2		 = 42,			/**< Blend 0 Port 2		*/
	E_IM_IIP_PARAM_PORTID_CFL0_P0		 = 44,			/**< Cache Flow 0 Port 0	*/
	E_IM_IIP_PARAM_PORTID_CFL0_P1		 = 45,			/**< Cache Flow 0 Port 1	*/
	E_IM_IIP_PARAM_PORTID_CSC2			 = 52,			/**< CSC 2				*/
	E_IM_IIP_PARAM_PORTID_CSC3			 = 53,			/**< CSC 3				*/
	E_IM_IIP_PARAM_PORTID_LD4			 = 54,			/**< Loader 4			*/
	E_IM_IIP_PARAM_PORTID_LD5			 = 55,			/**< Loader 5			*/
	E_IM_IIP_PARAM_PORTID_LD6			 = 56,			/**< Loader 6			*/
	E_IM_IIP_PARAM_PORTID_LD7			 = 57,			/**< Loader 7			*/
	E_IM_IIP_PARAM_PORTID_AFN3			 = 58,			/**< AFN 3				*/
	E_IM_IIP_PARAM_PORTID_BLEND1_P0		 = 59,			/**< Blend 1 Port 0		*/
	E_IM_IIP_PARAM_PORTID_BLEND1_P1		 = 60,			/**< Blend 1 Port 1		*/
	E_IM_IIP_PARAM_PORTID_BLEND1_P2		 = 61,			/**< Blend 1 Port 2		*/
	E_IM_IIP_PARAM_PORTID_MFT			 = 62,			/**< MFT				*/
	E_IM_IIP_PARAM_PORTID_NONE			 = 0x3F			/**< None unit			*/
} E_IM_IIP_PARAM_PORTID;

// AFN, FRECT, 1DL and FLT
/** Unit of upper stream */
typedef enum {
	E_IM_IIP_PARAM_UPSU_NONE = 0x0,			/**< None */
	E_IM_IIP_PARAM_UPSU_B2Y = 0x1,			/**< B2Y macro [FRECT and AFN unit only] */
	E_IM_IIP_PARAM_UPSU_AFN0 = 0x2,			/**< AFN0-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_AFN1 = 0x3,			/**< AFN1-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_AFN2 = 0x4,			/**< AFN2-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_AFN3 = 0x5,			/**< AFN3-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_FRECT0 = 0x6,		/**< FRECT0-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_FRECT1 = 0x7,		/**< FRECT1-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_FLT = 0x8,			/**< FLT-UNIT [AFN and LD unit only] */
	E_IM_IIP_PARAM_UPSU_LD0 = 0x9,			/**< LD0-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_LD1 = 0xA,			/**< LD1-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_LD2 = 0xB,			/**< LD2-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_LD3 = 0xC,			/**< LD3-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_LD4 = 0xD,			/**< LD4-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_LD5 = 0xE,			/**< LD5-UNIT [AFN, FLT and LD unit only] */
	E_IM_IIP_PARAM_UPSU_LD6 = 0xF,			/**< LD6-UNIT [AFN, FLT and LD unit only] */
} E_IM_IIP_PARAM_UPSU;

/** Unit number of upper stream */
typedef enum {
	E_IM_IIP_PARAM_UPNUM_NONE = 0x0,		/**< None */
	E_IM_IIP_PARAM_UPNUM_1 = 0x1,			/**< Unit number is 1 */
	E_IM_IIP_PARAM_UPNUM_2 = 0x2,			/**< Unit number is 2 */
} E_IM_IIP_PARAM_UPNUM;

/** Unit of lower stream */
typedef enum {
	E_IM_IIP_PARAM_DWSU_NONE = 0x0,			/**< None */
	E_IM_IIP_PARAM_DWSU_AFN0 = 0x2,			/**< AFN0-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_AFN1 = 0x3,			/**< AFN1-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_AFN2 = 0x4,			/**< AFN2-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_AFN3 = 0x5,			/**< AFN3-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_FLT = 0x8,			/**< FLT-UNIT [AFN, FRECT, LD unit only] */
	E_IM_IIP_PARAM_DWSU_LD0 = 0x9,			/**< LD0-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_LD1 = 0xA,			/**< LD1-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_LD2 = 0xB,			/**< LD2-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_LD3 = 0xC,			/**< LD3-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_LD4 = 0xD,			/**< LD4-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_LD5 = 0xE,			/**< LD5-UNIT [AFN, FRECT, FLT and LD unit only] */
	E_IM_IIP_PARAM_DWSU_LD6 = 0xF,			/**< LD6-UNIT [AFN, FRECT, FLT and LD unit only] */
} E_IM_IIP_PARAM_DWSU;

/** Unit number of lower stream */
typedef enum {
	E_IM_IIP_PARAM_DWNUM_NONE = 0x0,		/**< None */
	E_IM_IIP_PARAM_DWNUM_1 = 0x1,			/**< Unit number is 1 */
	E_IM_IIP_PARAM_DWNUM_2 = 0x2,			/**< Unit number is 2 */
} E_IM_IIP_PARAM_DWNUM;

/** Dirty bit preserved cache read */
typedef enum {
	E_IM_IIP_PARAM_DTHD_NORMAL = 0x0,				/**< When PIXFMTTBL Pbit=H */
} E_IM_IIP_PARAM_DTHD;

/** Corner Blur Correction mode */
typedef enum {
	E_IM_IIP_PARAM_CBC_DISABLE = 0x0,		/**< Corner Blur Correction mode is disable. */
	E_IM_IIP_PARAM_CBC_ENABLE = 0x1,		/**< Corner Blur Correction mode is enable. */
} E_IM_IIP_PARAM_CBC;

/**
   GHSZ of AFNCTL<br>
   GHSZ of FRCTL
 */
typedef enum {
	E_IM_IIP_PARAM_GHSZ_64PIX = 0,		/**< 64 pixels */
	E_IM_IIP_PARAM_GHSZ_32PIX = 1,		/**< 32 pixels */
} E_IM_IIP_PARAM_GHSZ;

/**
   GVSZ of AFNCTL<br>
   GVSZ of FRCTL
 */
typedef enum {
	E_IM_IIP_PARAM_GVSZ_SAME_AS_VERTICAL_PIX = 0,	/**< Same as vertical pixels	*/
	E_IM_IIP_PARAM_GVSZ_2PIX = 1,					/**< 2 pixels					*/
	E_IM_IIP_PARAM_GVSZ_4PIX = 2,					/**< 4 pixels					*/
	E_IM_IIP_PARAM_GVSZ_8PIX = 3,					/**< 8 pixels					*/
	E_IM_IIP_PARAM_GVSZ_16PIX = 4,					/**< 16 pixels					*/
	E_IM_IIP_PARAM_GVSZ_32PIX = 5,					/**< 32 pixels					*/
	E_IM_IIP_PARAM_GVSZ_64PIX = 6,					/**< 64 pixels					*/
	E_IM_IIP_PARAM_GVSZ_128PIX = 7,					/**< 128 pixels					*/
	E_IM_IIP_PARAM_GVSZ_256PIX = 8,					/**< 256 pixels					*/
} E_IM_IIP_PARAM_GVSZ;

/**
   LPLVL of AFNCTL<br>
   LPLVL of FRCTL
 */
typedef enum {
	E_IM_IIP_PARAM_LPLVL_H_LEVEL = 1,			/**< Output High level when the last pixel is transfered. */
} E_IM_IIP_PARAM_LPLVL;

/**
   HDBL of AFNCTL<br>
   HDBL of FRCTL
 */
typedef enum {
	E_IM_IIP_PARAM_HDBL_SAME_AS_PICHSZ = 0,		/**< Same as PICHSZ */
	E_IM_IIP_PARAM_HDBL_DOUBLE_PICHSZ = 1,		/**< Double PICHSZ	*/
} E_IM_IIP_PARAM_HDBL;

/**
   VDBL of AFNCTL<br>
   VDBL of FRCTL
 */
typedef enum {
	E_IM_IIP_PARAM_VDBL_SAME_AS_PICVSZ = 0,		/**< Same as PICVSZ */
	E_IM_IIP_PARAM_VDBL_DOUBLE_PICVSZ = 1,		/**< Double PICVSZ	*/
} E_IM_IIP_PARAM_VDBL;

/**
   HSMD of AFNCTL<br>
   HSMD of FRCTL
 */
typedef enum {
	E_IM_IIP_PARAM_HSMD_NORMAL = 0,				/**< Normal speed */
	E_IM_IIP_PARAM_HSMD_HIGH = 1,				/**< High speed */
} E_IM_IIP_PARAM_HSMD;

/**
   PFSP of PFCTL
 */
typedef enum {
	E_IM_IIP_PARAM_PFSP_1 = 0,		/**< Prefetch interval 1	*/
	E_IM_IIP_PARAM_PFSP_2 = 1,		/**< Prefetch interval 2	*/
	E_IM_IIP_PARAM_PFSP_4 = 2,		/**< Prefetch interval 4	*/
	E_IM_IIP_PARAM_PFSP_8 = 3,		/**< Prefetch interval 8	*/
	E_IM_IIP_PARAM_PFSP_16 = 4,		/**< Prefetch interval 16	*/
	E_IM_IIP_PARAM_PFSP_32 = 5,		/**< Prefetch interval 32	*/
	E_IM_IIP_PARAM_PFSP_64 = 6,		/**< Prefetch interval 64	*/
	E_IM_IIP_PARAM_PFSP_128 = 7,	/**< Prefetch interval 128	*/
} E_IM_IIP_PARAM_PFSP;

/**
   PFVSP of PFCTL
 */
typedef enum {
	E_IM_IIP_PARAM_PFVSP_VERTICAL_0 = 0,		/**< Prefetch vertical interval 0 */
} E_IM_IIP_PARAM_PFVSP;


// 1D & AFN & FRECT
/**
   PFOFF of LD_PREFETCH<br>
   PFOFF of PFCTL<br>
 */
typedef enum {
	E_IM_IIP_PARAM_PFOFF_RUN = 0,				/**< Run Prefetch */
	E_IM_IIP_PARAM_PFOFF_STOP = 1,				/**< Stop Prefetch */
} E_IM_IIP_PARAM_PFOFF;


// AFN & FRECT
/**
   EXA of PIXIDEF
 */
typedef enum {
	E_IM_IIP_PARAM_EXA_NO_ALPHA_TO_NEXT_UNIT = 0,	/**< No transfer alpha to next unit */
	E_IM_IIP_PARAM_EXA_ALPHA_TO_NEXT_UNIT = 1,		/**< Transfer alpha to next unit	*/
} E_IM_IIP_PARAM_EXA;

/**
   ITMD of CALMETHOD
 */
typedef enum {
	E_IM_IIP_PARAM_ITMD_BI_LINEAR = 0,	/**< Bi-linear */
	E_IM_IIP_PARAM_ITMD_NEAREST = 1,	/**< Nearest neighbor */
	E_IM_IIP_PARAM_ITMD_BI_CUBIC = 2,	/**< Bi-cubic [only 8bit depth] */
} E_IM_IIP_PARAM_ITMD;

/**
   ARA of CALMETHOD
 */
typedef enum {
	E_IM_IIP_PARAM_ARA_ITMD = 0,		/**< Follow ITMD register. */
	E_IM_IIP_PARAM_ARA_REDUCE_1_4 = 1,	/**< 1/4 reduce row and columns. [AFN only] */
} E_IM_IIP_PARAM_ARA;

/**
   CUBSEL of CALMETHOD
 */
typedef enum {
	E_IM_IIP_PARAM_CUBSEL_AFN0 = 0,		/**< AFN0-UNIT */
	E_IM_IIP_PARAM_CUBSEL_AFN1 = 1,		/**< AFN1-UNIT */
	E_IM_IIP_PARAM_CUBSEL_AFN2 = 2,		/**< AFN2-UNIT */
	E_IM_IIP_PARAM_CUBSEL_AFN3 = 3,		/**< AFN3-UNIT */
	E_IM_IIP_PARAM_CUBSEL_FRECT0 = 4,	/**< FRECT0-UNIT */
	E_IM_IIP_PARAM_CUBSEL_FRECT1 = 5,	/**< FRECT1-UNIT */
} E_IM_IIP_PARAM_CUBSEL;

/**
   SCUB of CALMETHOD
 */
typedef enum {
	E_IM_IIP_PARAM_SCUB_0 = 0,			/**< Use Bi-cubic unit No.0 */
	E_IM_IIP_PARAM_SCUB_1 = 1,			/**< Use Bi-cubic unit No.1 */
} E_IM_IIP_PARAM_SCUB;

/**
   OPMD
 */
typedef enum {
	E_IM_IIP_PARAM_OPMD_NON = 0,		/**< Nothing	*/
	E_IM_IIP_PARAM_OPMD_ADD = 1,		/**< ADD		*/
	E_IM_IIP_PARAM_OPMD_SUB = 2,		/**< SUB		*/
	E_IM_IIP_PARAM_OPMD_EOR = 3,		/**< EOR		*/
	E_IM_IIP_PARAM_OPMD_OR	= 4,		/**< OR			*/
	E_IM_IIP_PARAM_OPMD_AND = 5,		/**< AND		*/
} E_IM_IIP_PARAM_OPMD;


// 1D & AFN & FRECT & FLT & STS
/** CSEL of LD_CTL
	CSEL of TDL_CTL
	CSEL of PIXIDDEF
	CSEL of PIXIDEF
*/
typedef enum {
	E_IM_IIP_PARAM_CSEL_0 = 0x0,		/**< Cache controller 0. */
	E_IM_IIP_PARAM_CSEL_1 = 0x1,		/**< Cache controller 1. */
	E_IM_IIP_PARAM_CSEL_2 = 0x2,		/**< Cache controller 2. */
} E_IM_IIP_PARAM_CSEL;


// 1D Loader
/** LD_MODE of LD_CTL */
typedef enum {
	E_IM_IIP_PARAM_LD_MODE_NORMAL_LOAD = 0,			/**< Normal load mode			*/
	E_IM_IIP_PARAM_LD_MODE_LINK = 3,				/**< link mode					*/
} E_IM_IIP_PARAM_LD_MODE;

/** SLOW of LD_CTL */
typedef enum {
	E_IM_IIP_PARAM_SLOW_2PIX_CYC = 0,				/**< 2 pixel / cycle */
	E_IM_IIP_PARAM_SLOW_1PIX_CYC = 1,				/**< 1 pixel / cycle */
} E_IM_IIP_PARAM_SLOW;

/**
   PF_PDIST of LD_PREFETCH
 */
typedef enum {
	E_IM_IIP_PARAM_PF_PDIST_8PIX  = 0,		/**< Prefetch pixel interval : 8pixels */
	E_IM_IIP_PARAM_PF_PDIST_16PIX = 1,		/**< Prefetch pixel interval : 16pixels */
	E_IM_IIP_PARAM_PF_PDIST_32PIX = 2,		/**< Prefetch pixel interval : 32pixels */
	E_IM_IIP_PARAM_PF_PDIST_64PIX = 3,		/**< Prefetch pixel interval : 64pixels */
} E_IM_IIP_PARAM_PF_PDIST;

/**
   PF_1ST of LD_PREFETCH
 */
typedef enum {
	E_IM_IIP_PARAM_PF_1ST_ACCESS_BEFORE_PF = 0,		/**< Access before prefetch */
	E_IM_IIP_PARAM_PF_1ST_ACCESS_AFTER_PF = 1,		/**< Access after prefetch	*/
} E_IM_IIP_PARAM_PF_1ST;


// BLEND
/**
   BLD_MD of BLENDCTL
 */
typedef enum {
	E_IM_IIP_PARAM_BLD_MD_ALPHA_BLEND = 0,		/**< Alpha blend */
	E_IM_IIP_PARAM_BLD_MD_W_ADD = 1,			/**< Weighted addition */
	E_IM_IIP_PARAM_BLD_MD_SUB = 2,				/**< Subtraction */
	E_IM_IIP_PARAM_BLD_MD_MASK_TBL = 3,			/**< Mask table output */
	E_IM_IIP_PARAM_BLD_MD_MUL = 4,				/**< Multiple */
} E_IM_IIP_PARAM_BLD_MD;

/**
   CLP_MD of BLENDCTL
 */
typedef enum {
	E_IM_IIP_PARAM_CLP_MD_NO_CLIP = 0,			/**< No clip */
	E_IM_IIP_PARAM_CLP_MD_CLIP_UNSIGNED_8 = 1,	/**< Clip [0~255] */
	E_IM_IIP_PARAM_CLP_MD_CLIP_SIGNED_8 = 2,	/**< Clip [-128~+127] */
	E_IM_IIP_PARAM_CLP_MD_CLIP_HYBRID_8 = 3,	/**< Clip D0:[0~255] D1:D2[-128~+127] */
} E_IM_IIP_PARAM_CLP_MD;

/**
   ALPHA_OUT of BLENDCTL
 */
typedef enum {
	E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA = 0,		/**< Alpha of Port 0 */
	E_IM_IIP_PARAM_ALPHA_OUT_P1_ALPHA = 1,		/**< Alpha of Port 1 */
	E_IM_IIP_PARAM_ALPHA_OUT_FIXED_VAL = 2,		/**< Fixed value */
	E_IM_IIP_PARAM_ALPHA_OUT_NONE = 3,			/**< None */
} E_IM_IIP_PARAM_ALPHA_OUT;

/**
   ALPHA_STEP of ALPHACTL0
 */
typedef enum {
	E_IM_IIP_PARAM_ALPHA_STEP_8BITS = 0,	/**< 8 bits */
	E_IM_IIP_PARAM_ALPHA_STEP_4BITS = 3,	/**< 4 bits */
} E_IM_IIP_PARAM_ALPHA_STEP;

/**
   ALPHA_SEL of ALHACTL
 */
typedef enum {
	E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA = 0,			/**< Alpha of Port 0 */
	E_IM_IIP_PARAM_ALPHA_SEL_P1_ALPHA = 1,			/**< Alpha of Port 1 */
	E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL = 2,			/**< Fixed value */
	E_IM_IIP_PARAM_ALPHA_SEL_P2_D0_D1_D2_ALPHA = 3,	/**< Alpha of Port 0 D0, D1 and D2 */
	E_IM_IIP_PARAM_ALPHA_SEL_P2_D0_ALPHA = 4,		/**< Alpha of Port 0 D0 */
} E_IM_IIP_PARAM_ALPHA_SEL;

/**
   SFTSEL of MULCTL
 */
typedef enum {
	E_IM_IIP_PARAM_SFTSEL_14BIT = 0,			/**< 14bit Right shift */
	E_IM_IIP_PARAM_SFTSEL_12BIT = 1,			/**< 12bit Right shift */
	E_IM_IIP_PARAM_SFTSEL_8BIT = 2,				/**< 8bit Right shift */
	E_IM_IIP_PARAM_SFTSEL_NONE = 2,				/**< Through */
} E_IM_IIP_PARAM_SFTSEL;


// CSC
/**
   ALPSEL
 */
typedef enum {
	E_IM_IIP_PARAM_ALPSEL_DIRECT = 0,			/**< Direct */
	E_IM_IIP_PARAM_ALPSEL_ALPVAL_REG = 1,		/**< ALPVAL register */
} E_IM_IIP_PARAM_ALPSEL;

/**
   ALOSEL
 */
typedef enum {
	E_IM_IIP_PARAM_ALOSEL_CALCULATED = 0,		/**< Calculated value */
	E_IM_IIP_PARAM_ALOSEL_THROUGH = 1,			/**< Through(Input value) */
} E_IM_IIP_PARAM_ALOSEL;

/**
   MAXMIN
 */
typedef enum {
	E_IM_IIP_PARAM_MAXMIN_THROUGH = 0,		/**< Through */
	E_IM_IIP_PARAM_MAXMIN_MAX = 2,			/**< Output maximum value */
	E_IM_IIP_PARAM_MAXMIN_MIN = 3,			/**< Output minimum value */
} E_IM_IIP_PARAM_MAXMIN;


// GPC
/**
   COMMONPARA of GPCCONTROL
 */
typedef enum {
	E_IM_IIP_PARAM_COMMONPARA_D0_D1_D2 = 0,		/**< D0:D0 parameter | D1:D1 parameter | D2:D2 parameter */
	E_IM_IIP_PARAM_COMMONPARA_D0_D1_D1 = 1,		/**< D0:D0 parameter | D1:D1 parameter | D2:D1 parameter */
	E_IM_IIP_PARAM_COMMONPARA_D0_D0_D0 = 2,		/**< D0:D0 parameter | D1:D0 parameter | D2:D0 parameter */
} E_IM_IIP_PARAM_COMMONPARA;

/**
   ATRB0/ATRB1
 */
typedef enum {
	E_IM_IIP_PARAM_ATRB_PORT0 = 0x0,	/**< Through input port 0 image type */
	E_IM_IIP_PARAM_ATRB_YUV_RGB = 0x8,	/**< YUV(RGB) image type */
	E_IM_IIP_PARAM_ATRB_CBCR = 0x9,		/**< CbCr image type */
	E_IM_IIP_PARAM_ATRB_PALNE = 0xB,	/**< Plane slow image type */
} E_IM_IIP_PARAM_ATRB;


// LUT
/**
   USE of LUTCTL
 */
typedef enum {
	E_IM_IIP_PARAM_LUTUSE_PRECALC = 0,			/**< Output of Precal (No LUT) */
	E_IM_IIP_PARAM_LUTUSE_LUT_OUT = 1,			/**< Output of LUT			 */
} E_IM_IIP_PARAM_LUTUSE;

/**
   SGN of LUTCTL
 */
typedef enum {
	E_IM_IIP_PARAM_LUTSGN_UNSIGN = 0,		/**< Unsigned */
	E_IM_IIP_PARAM_LUTSGN_SIGN = 1,			/**< Signed   */
} E_IM_IIP_PARAM_LUTSGN;


// MON
/**
   DSEL of MONDSEL
 */
typedef enum {
	E_IM_IIP_PARAM_DSEL_SL0 = 0,	/**< SL 0 */
	E_IM_IIP_PARAM_DSEL_SL1 = 1,	/**< SL 1 */
	E_IM_IIP_PARAM_DSEL_SL2 = 2,	/**< SL 2 */
	E_IM_IIP_PARAM_DSEL_SL3 = 3,	/**< SL 3 */
	E_IM_IIP_PARAM_DSEL_SL4 = 4,	/**< SL 4 */
	E_IM_IIP_PARAM_DSEL_SL5 = 5,	/**< SL 5 */
	E_IM_IIP_PARAM_DSEL_SL6 = 6,	/**< SL 6 */
	E_IM_IIP_PARAM_DSEL_SL7 = 7,	/**< SL 7 */
} E_IM_IIP_PARAM_DSEL;

/**
   SGNEN0 of MONDSEL<br>
   SGNEN1 of MONDSEL<br>
   SGNEN2 of MONDSEL<br>
   SGNEN3 of MONDSEL
 */
typedef enum {
	E_IM_IIP_PARAM_SGNEN_UNSIGN = 0,		/**< Unsigned */
	E_IM_IIP_PARAM_SGNEN_SIGN = 1,			/**< Signed   */
} E_IM_IIP_PARAM_SGNEN;

/**
   BITSEL_0 of HISTCTL<br>
   BITSEL_1 of HISTCTL<br>
   BITSEL_2 of HISTCTL
 */
typedef enum {
	E_IM_IIP_PARAM_BITSEL_7_0 = 0,		/**< [7:0] */
	E_IM_IIP_PARAM_BITSEL_9_2 = 1,		/**< [9:2] */
	E_IM_IIP_PARAM_BITSEL_11_4 = 2,		/**< [11:4] */
	E_IM_IIP_PARAM_BITSEL_13_6 = 3,		/**< [13:6] */
} E_IM_IIP_PARAM_BITSEL;


// FLT
/**
   FM of TDL_CTL
 */
typedef enum {
	E_IM_IIP_PARAM_FM_3x3 = 0,		/**< 3x3 */
	E_IM_IIP_PARAM_FM_4x4 = 1,		/**< 4x4 */
	E_IM_IIP_PARAM_FM_5x5 = 2,		/**< 5x5 */
} E_IM_IIP_PARAM_FM;

/**
   LNUM of TDL_CTL
 */
typedef enum {
	E_IM_IIP_PARAM_LNUM_SLOW = 0,	/**< Slow */
	E_IM_IIP_PARAM_LNUM_FAST = 1,	/**< Fast */
} E_IM_IIP_PARAM_LNUM;

/**
   INPUT_MODE of TDL_CTL
 */
typedef enum {
	E_IM_IIP_PARAM_INPUT_MODE_NORMAL = 0,		/**< Normal */
	E_IM_IIP_PARAM_INPUT_MODE_SLINK = 1,		/**< Slink */
	E_IM_IIP_PARAM_INPUT_MODE_DLINK = 3,		/**< Dlink */
} E_IM_IIP_PARAM_INPUT_MODE;

/**
   PF_ON of TDL_PF
 */
typedef enum {
	E_IM_IIP_PARAM_PF_ON_INVALID = 0,	/**< Invalid prefetch */
	E_IM_IIP_PARAM_PF_ON_ACTIVE = 1,	/**< Active prefetch  */
} E_IM_IIP_PARAM_PF_ON;

/**
   PF_MD of TDL_PF
 */
typedef enum {
	E_IM_IIP_PARAM_PF_MD_ACCESS = 0,	/**< Real access has higher priority. */
	E_IM_IIP_PARAM_PF_MD_PREFETCH = 1,	/**< Prefetch has higher priority. */
} E_IM_IIP_PARAM_PF_MD;

/**
   UNIT_OUT_A_D0 of UNIT_OUT<br>
   UNIT_OUT_A_D1 of UNIT_OUT<br>
   UNIT_OUT_A_D2 of UNIT_OUT<br>
   UNIT_OUT_B_D0 of UNIT_OUT<br>
   UNIT_OUT_B_D1 of UNIT_OUT<br>
   UNIT_OUT_B_D2 of UNIT_OUT
 */
typedef enum {
	E_IM_IIP_PARAM_UNIT_OUT_THROUGH = 0,	/**< Through */
	E_IM_IIP_PARAM_UNIT_OUT_ISO = 1,		/**< ISO [only UNIT_OUT_A_D0] */
	E_IM_IIP_PARAM_UNIT_OUT_EDGE = 2,		/**< EDGE [only UNIT_OUT_A_D0~D2] */
	E_IM_IIP_PARAM_UNIT_OUT_EP = 3,			/**< EP [only UNIT_OUT_A_D0~D2] */
	E_IM_IIP_PARAM_UNIT_OUT_SPF = 4,		/**< SPF */
	E_IM_IIP_PARAM_UNIT_OUT_BADC = 5,		/**< ADAPT [only UNIT_OUT_A_D0] */
	E_IM_IIP_PARAM_UNIT_OUT_DISABLE = 7,	/**< Disable */
} E_IM_IIP_PARAM_UNIT_OUT;

/**
   SPF_MD
 */
typedef enum {
	E_IM_IIP_PARAM_SPF_MD_SPF = 0,			/**< SPF  */
	E_IM_IIP_PARAM_SPF_MD_EDGE_ENHANCE = 1,	/**< Edge Enhance */
	E_IM_IIP_PARAM_SPF_MD_EDGE_DETECT = 2,	/**< Edge Detection */
} E_IM_IIP_PARAM_SPF_MD;

/**
   EP_MD
 */
typedef enum {
	E_IM_IIP_PARAM_EP_MD_NR_LOW = 0,		/**< Low Noise Reduction  */
	E_IM_IIP_PARAM_EP_MD_NR_MID = 1,		/**< Middle Noise Reduction */
	E_IM_IIP_PARAM_EP_MD_NR_HIGH = 2,		/**< High Noise Reduction */
} E_IM_IIP_PARAM_EP_MD;

/**
   ED_FM
 */
typedef enum {
	E_IM_IIP_PARAM_ED_FM_NEAR4 = 0,		/**< Near 4 pxiels */
	E_IM_IIP_PARAM_ED_FM_NEAR8 = 1,		/**< near 8 pixels */
} E_IM_IIP_PARAM_ED_FM;

/**
   ED_OUT
 */
typedef enum {
	E_IM_IIP_PARAM_ED_OUT_DIFF_ABS_MIN = 0,		/**< min(|P0 - Px|) */
	E_IM_IIP_PARAM_ED_OUT_DIFF_ABS_MAX = 1,		/**< max(|P0 - Px|) */
} E_IM_IIP_PARAM_ED_OUT;

/**
   ADAPT_DIV
 */
typedef enum {
	E_IM_IIP_PARAM_ADAPT_DIV_1_DIVBY_1 = 0,		/**< 1/1 */
	E_IM_IIP_PARAM_ADAPT_DIV_1_DIVBY_2 = 1,		/**< 1/2 */
	E_IM_IIP_PARAM_ADAPT_DIV_1_DIVBY_3 = 2,		/**< 1/3 */
	E_IM_IIP_PARAM_ADAPT_DIV_1_DIVBY_4 = 3,		/**< 1/4 */
	E_IM_IIP_PARAM_ADAPT_DIV_1_DIVBY_8 = 4,		/**< 1/8 */
} E_IM_IIP_PARAM_ADAPT_DIV;

/**
   CALC_MD
 */
typedef enum {
	E_IM_IIP_PARAM_CALC_MD_NORMAL = 0,		/**< Normal mode. */
	E_IM_IIP_PARAM_CALC_MD_CBC = 1,			/**< Corner Blur Correction mode. */
	E_IM_IIP_PARAM_CALC_MD_WSF = 2,			/**< Weighted Smooth filter mode. */
} E_IM_IIP_PARAM_CALC_MD;

/**
   BF_FMT
 */
typedef enum {
	E_IM_IIP_PARAM_BF_FMT_VALID_D0_D1_D2 = 0,	/**< D0, D1 and D2 is valid. */
	E_IM_IIP_PARAM_BF_FMT_VALID_D0 = 1,			/**< D0 is valid. */
} E_IM_IIP_PARAM_BF_FMT;

/**
   BFPRT
 */
typedef enum {
	E_IM_IIP_PARAM_BF_PRT_PORT0 = 0,			/**< Output port is 0. */
	E_IM_IIP_PARAM_BF_PRT_PORT0_AND_1 = 1,		/**< Output port is 0 and 1. */
} E_IM_IIP_PARAM_BF_PRT;


// SL
/**
   DIV1ST of PIXIDDEF
 */
typedef enum {
	E_IM_IIP_PARAM_DIV1ST_NON_1ST = 0,		/**< None 1st block */
	E_IM_IIP_PARAM_DIV1ST_1ST = 1,			/**< 1st block */
} E_IM_IIP_PARAM_DIV1ST;

/**
   MASKEN of SL_CTL
 */
typedef enum {
	E_IM_IIP_PARAM_MASKEN_NO_MASK = 0,			/**< No mask */
	E_IM_IIP_PARAM_MASKEN_DEPEND_ON_ALPHA = 1,	/**< Mask depends on alpha */
	E_IM_IIP_PARAM_MASKEN_FRAME_MASK = 2,		/**< Frame mask */
} E_IM_IIP_PARAM_MASKEN;

/**
   SFTM_0 of SFT_0<br>
   SFTM_1 of SFT_1
 */
typedef enum {
	E_IM_IIP_PARAM_SFTM_ROUNDDOWN = 0,		/**< Round-down */
	E_IM_IIP_PARAM_SFTM_ROUNDOFF = 1,		/**< Round-off	*/
} E_IM_IIP_PARAM_SFTM;

/**
 * FILL_EN
 */
typedef enum {
	E_IM_IIP_PARAM_FILL_EN_DISABLE = 0x0,		/**< Rectangle fill is disable. */
	E_IM_IIP_PARAM_FILL_EN_ENABLE = 0x1,		/**< Rectangle fill is enable. */
} E_IM_IIP_PARAM_FILL_EN;


// MFT
/**
   MFTMD
 */
typedef enum {
	E_IM_IIP_PARAM_MFTMD_MIN = 0,		/**< Min mode */
	E_IM_IIP_PARAM_MFTMD_MAX = 1,		/**< Max mode */
} E_IM_IIP_PARAM_MFTMD;

/**
   D0FIX/D1FIX/D2FIX
 */
typedef enum {
	E_IM_IIP_PARAM_DNFIX_FILTER = 0,	/**< Output is filterd value. */
	E_IM_IIP_PARAM_DNFIX_FIXVAL = 1,	/**< Output is fixed value. */
} E_IM_IIP_PARAM_DNFIX;


///////////////////////////////////////////////////////////////
// GPC Struct Parameter
///////////////////////////////////////////////////////////////

/** Valid value of PARAAD (associated with GPC unit) */
typedef enum {
	E_IM_IIP_PARAM_PARAAD_SEQ_CTRL		= 0x000,	/**< Sequence control */
	E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D0_1  = 0x100,	/**< Pre-calculation (PORT0 D0, D1) */
	E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D2_3  = 0x140,	/**< Pre-calculation (PORT0 D2, D3) */
	E_IM_IIP_PARAM_PARAAD_PRECAL_P1_D0_1  = 0x180,	/**< Pre-calculation (PORT1 D0, D1) */
	E_IM_IIP_PARAM_PARAAD_PRECAL_P1_D2_3  = 0x1C0,	/**< Pre-calculation (PORT1 D2, D3) */
	E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG0	= 0x200,	/**< Configuration[0] of General calculation for D0 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG1	= 0x240,	/**< Configuration[1] of General calculation for D0 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG2	= 0x280,	/**< Configuration[2] of General calculation for D0 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG3	= 0x2C0,	/**< Configuration[3] of General calculation for D0 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG0	= 0x300,	/**< Configuration[0] of General calculation for D1 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG1	= 0x340,	/**< Configuration[1] of General calculation for D1 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG2	= 0x380,	/**< Configuration[2] of General calculation for D1 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG3	= 0x3C0,	/**< Configuration[3] of General calculation for D1 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG0	= 0x400,	/**< Configuration[0] of General calculation for D2 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG1	= 0x440,	/**< Configuration[1] of General calculation for D2 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG2	= 0x480,	/**< Configuration[2] of General calculation for D2 compoment */
	E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG3	= 0x4C0,	/**< Configuration[3] of General calculation for D2 compoment */
	E_IM_IIP_PARAM_PARAAD_ALPHA			= 0x500,	/**< Configuration[0][1][2][3] of Alpha */
	E_IM_IIP_PARAM_PARAAD_INTER_COMP_CALC = 0x540	/**< Inter color calculation */
} E_IM_IIP_PARAM_PARAAD;

/** 
   COMPSEL register<br>
   COMPSEL
 */
typedef enum {
	E_IM_IIP_PARAM_COMPSEL_P0D0 = 0,	/**< D0 of Port 0	*/
	E_IM_IIP_PARAM_COMPSEL_P1D0 = 1,	/**< D0 of Port 1	*/
	E_IM_IIP_PARAM_COMPSEL_P0D3 = 2,	/**< D3 of Port 0	*/
	E_IM_IIP_PARAM_COMPSEL_P1D3 = 3,	/**< D3 of Port 1	*/
	E_IM_IIP_PARAM_COMPSEL_CFG0 = 4,	/**< Config register 0	*/
	E_IM_IIP_PARAM_COMPSEL_CFG1 = 5,	/**< Config register 1	*/
	E_IM_IIP_PARAM_COMPSEL_CFG2 = 6,	/**< Config register 2	*/
	E_IM_IIP_PARAM_COMPSEL_CFG3 = 7,	/**< Config register 3	*/
	E_IM_IIP_PARAM_COMPSEL_CFG4 = 8,	/**< Config register 4	*/
	E_IM_IIP_PARAM_COMPSEL_CFG5 = 9,	/**< Config register 5	*/
	E_IM_IIP_PARAM_COMPSEL_CFG6 = 10,	/**< Config register 6	*/
	E_IM_IIP_PARAM_COMPSEL_CFG7 = 11,	/**< Config register 7	*/
	E_IM_IIP_PARAM_COMPSEL_CFG8 = 12,	/**< Config register 8	*/
	E_IM_IIP_PARAM_COMPSEL_CFG9 = 13,	/**< Config register 9	*/
	E_IM_IIP_PARAM_COMPSEL_P0D1 = 14,	/**< D1 of Port 0	*/
	E_IM_IIP_PARAM_COMPSEL_P1D1 = 15,	/**< D1 of Port 1	*/
	E_IM_IIP_PARAM_COMPSEL_P0D2 = 16,	/**< D2 of Port 0	*/
	E_IM_IIP_PARAM_COMPSEL_P1D2 = 17,	/**< D2 of Port 1	*/
} E_IM_IIP_PARAM_COMPSEL;

/**
   GPC0PARA1 register<br>
   G0RND_0 / G0RND_1 / G0RND_2 / G0RND_3 / G0RND_4 / G0RND_5 / G0RND_6<br>
   <br>
   GPC1PARA1 register<br>
   G1RND_0 / G1RND_1 / G1RND_2 / G1RND_3 / G1RND_4 / G1RND_5 / G1RND_6<br>
   <br>
   COMPSEL register<br>
   CMPRND<br>
   <br>
   P1CNT register<br>
   P1RNDO_0 / P1RNDO_1 / P1RNDO_2<br>
   <br>
   ELECAL register<br>
   ELERD0_0 / ELERD0_1 / ELERD0_2 / ELERD0_3 /
   ELERD1_0 / ELERD1_1 / ELERD1_2 / ELERD1_3 /
   ELERD2_0 / ELERD2_1 / ELERD2_2 / ELERD2_3 /
   ELERD3_0 / ELERD3_1 / ELERD3_2 / ELERD3_3 /
   ELERDO_0 / ELERDO_1 / ELERDO_2 / ELERDO_3
*/
typedef enum {
	E_IM_IIP_PARAM_RND_ROUNDDOWN = 0,	/**< Round-down */
	E_IM_IIP_PARAM_RND_ROUNDOFF = 1,	/**< Round-off */
} E_IM_IIP_PARAM_RND;

/** 
   CONFCNT register<br>
   CONFREGSEL0 / CONFREGSEL1 / CONFREGSEL2 / CONFREGSEL3 / CONFREGSEL4 /
   CONFREGSEL5 / CONFREGSEL6 / CONFREGSEL7 / CONFREGSEL8 / CONFREGSEL9
 */
typedef enum {
	E_IM_IIP_PARAM_CONFREGSEL_P0D0	= 0,	/**< D0 of Port 0	*/
	E_IM_IIP_PARAM_CONFREGSEL_P1D0	= 1,	/**< D0 of Port 1	*/
	E_IM_IIP_PARAM_CONFREGSEL_P0D3	= 2,	/**< D3 of Port 0	*/
	E_IM_IIP_PARAM_CONFREGSEL_P1D3	= 3,	/**< D3 of Port 1	*/
	E_IM_IIP_PARAM_CONFREGSEL_CALC0 = 4,	/**< Calculation 0	*/
	E_IM_IIP_PARAM_CONFREGSEL_CALC1 = 5,	/**< Calculation 1	*/
	E_IM_IIP_PARAM_CONFREGSEL_COMP	= 6,	/**< Comparator		*/
	E_IM_IIP_PARAM_CONFREGSEL_CFG_n_1 = 7,	/**< Config register n-1 */
	E_IM_IIP_PARAM_CONFREGSEL_CFG_n = 8,	/**< Config register n	*/
	E_IM_IIP_PARAM_CONFREGSEL_P0D1	= 9,	/**< D0 of Port 0	*/
	E_IM_IIP_PARAM_CONFREGSEL_P1D1	= 10,	/**< D0 of Port 1	*/
	E_IM_IIP_PARAM_CONFREGSEL_P0D2	= 11,	/**< D3 of Port 0	*/
	E_IM_IIP_PARAM_CONFREGSEL_P1D2	= 12,	/**< D3 of Port 1	*/
} E_IM_IIP_PARAM_CONFREGSEL;

/**
   ELECAL register<br>
   ELESEL0_0 / ELESEL0_1 / ELESEL0_2 / ELESEL0_3 /
   ELESEL1_0 / ELESEL1_1 / ELESEL1_2 / ELESEL1_3 /
   ELESEL2_0 / ELESEL2_1 / ELESEL2_2 / ELESEL2_3 /
   ELESEL3_0 / ELESEL3_1 / ELESEL3_2 / ELESEL3_3
 */
typedef enum {
	E_IM_IIP_PARAM_ELESEL_D0_CFG9 = 0,		/**< Config register 9 of D0 */
	E_IM_IIP_PARAM_ELESEL_D1_CFG9 = 1,		/**< Config register 9 of D1 */
	E_IM_IIP_PARAM_ELESEL_D2_CFG9 = 2,		/**< Config register 9 of D2 */
	E_IM_IIP_PARAM_ELESEL_D3_ALPHA = 3,		/**< Alpha calculator */
} E_IM_IIP_PARAM_ELESEL;

/**
   ELECAL register<br>
   ELESHIFT0_0 / ELESHIFT0_1 / ELESHIFT0_2 / ELESHIFT0_3 /
   ELESHIFT1_0 / ELESHIFT1_1 / ELESHIFT1_2 / ELESHIFT1_3 /
   ELESHIFT2_0 / ELESHIFT2_1 / ELESHIFT2_2 / ELESHIFT2_3 /
   ELESHIFT3_0 / ELESHIFT3_1 / ELESHIFT3_2 / ELESHIFT3_3 /
   ELESHIFTO_0 / ELESHIFTO_1 / ELESHIFTO_2 / ELESHIFTO_3
 */
typedef enum {
	E_IM_IIP_PARAM_ELESHIFT_RIGHT = 0,	/**< right shift */
	E_IM_IIP_PARAM_ELESHIFT_LEFT = 1,	/**< left shift */
} E_IM_IIP_PARAM_ELESHIFT;

/**
   ELECAL register<br>
   ELEMODE0_0 / ELEMODE0_1 / ELEMODE0_2 / ELEMODE0_3 /
   ELEMODE1_0 / ELEMODE1_1 / ELEMODE1_2 / ELEMODE1_3 /
   ELEMODE2_0 / ELEMODE2_1 / ELEMODE2_2 / ELEMODE2_3
 */
typedef enum {
	E_IM_IIP_PARAM_ELEMODE_THROUGH = 0,		/**< Through */
	E_IM_IIP_PARAM_ELEMODE_AND = 1,			/**< AND */
	E_IM_IIP_PARAM_ELEMODE_OR = 2,			/**< OR */
	E_IM_IIP_PARAM_ELEMODE_EOR = 3,			/**< EOR */
	E_IM_IIP_PARAM_ELEMODE_ADD = 4,			/**< ADD */
	E_IM_IIP_PARAM_ELEMODE_SUB = 5,			/**< SUB */
	E_IM_IIP_PARAM_ELEMODE_ABS = 6,			/**< ABS */
} E_IM_IIP_PARAM_ELEMODE;

/** 
   GPC0PARA0 register<br>
   GPC0SEL0 / GPC0SEL1 / GPC0SEL2 / GPC0SEL3<br>
   <br>
   GPC1PARA0 register<br>
   GPC1SEL0 / GPC1SEL1 / GPC1SEL2<br>
 */
typedef enum {
	E_IM_IIP_PARAM_GPCSEL_P0D0 = 0,		/**< D0 of Port 0	*/
	E_IM_IIP_PARAM_GPCSEL_P1D0 = 1,		/**< D0 of Port 1	*/
	E_IM_IIP_PARAM_GPCSEL_P0D3 = 2,		/**< D3 of Port 0	*/
	E_IM_IIP_PARAM_GPCSEL_P1D3 = 3,		/**< D3 of Port 1	*/
	E_IM_IIP_PARAM_GPCSEL_CFG0 = 4,		/**< Config register 0	*/
	E_IM_IIP_PARAM_GPCSEL_CFG1 = 5,		/**< Config register 1	*/
	E_IM_IIP_PARAM_GPCSEL_CFG2 = 6,		/**< Config register 2	*/
	E_IM_IIP_PARAM_GPCSEL_CFG3 = 7,		/**< Config register 3	*/
	E_IM_IIP_PARAM_GPCSEL_CFG4 = 8,		/**< Config register 4	*/
	E_IM_IIP_PARAM_GPCSEL_CFG5 = 9,		/**< Config register 5	*/
	E_IM_IIP_PARAM_GPCSEL_CFG6 = 10,	/**< Config register 6	*/
	E_IM_IIP_PARAM_GPCSEL_CFG7 = 11,	/**< Config register 7	*/
	E_IM_IIP_PARAM_GPCSEL_CFG8 = 12,	/**< Config register 8	*/
	E_IM_IIP_PARAM_GPCSEL_CFG9 = 13,	/**< Config register 9	*/
	E_IM_IIP_PARAM_GPCSEL_P0D1 = 14,	/**< D1 of Port 0	*/
	E_IM_IIP_PARAM_GPCSEL_P1D1 = 15,	/**< D1 of Port 1	*/
	E_IM_IIP_PARAM_GPCSEL_P0D2 = 16,	/**< D2 of Port 0	*/
	E_IM_IIP_PARAM_GPCSEL_P1D2 = 17,	/**< D2 of Port 1	*/
} E_IM_IIP_PARAM_GPCSEL;

/**
   GPC0PARA0 register<br>
   G0MD0 / G0MD1<br>
   <br>
   GPC1PARA0 register<br>
   G1MD0
 */
typedef enum {
	E_IM_IIP_PARAM_GMD_MUL_THROUGH = 0,		/**< Through */
	E_IM_IIP_PARAM_GMD_MUL_MUL = 1,			/**< Multiplication */
	E_IM_IIP_PARAM_GMD_MUL_MAX = 2,			/**< Max [G1MD0 only] */
	E_IM_IIP_PARAM_GMD_MUL_MIN = 3,			/**< Min [G1MD0 only] */
} E_IM_IIP_PARAM_GMD_MUL;

/**
   GPC0PARA0 register<br>
   G0MD2<br>
   <br>
   GPC1PARA0 register<br>
   G1MD1
 */
typedef enum {
	E_IM_IIP_PARAM_GMD_BIN_THROUGH = 0,		/**< Through */
	E_IM_IIP_PARAM_GMD_BIN_AND = 1,			/**< AND */
	E_IM_IIP_PARAM_GMD_BIN_OR = 2,			/**< OR */
	E_IM_IIP_PARAM_GMD_BIN_EOR = 3,			/**< EOR */
	E_IM_IIP_PARAM_GMD_BIN_ADD = 4,			/**< ADD */
	E_IM_IIP_PARAM_GMD_BIN_SUB = 5,			/**< SUB */
	E_IM_IIP_PARAM_GMD_BIN_ABS = 6,			/**< ABS */
} E_IM_IIP_PARAM_GMD_BIN;

/** 
   GPCALP register<br>
   GPCALPSEL0_0 / GPCALPSEL0_1 / GPCALPSEL0_2 / GPCALPSEL0_3 /
   GPCALPSEL1_0 / GPCALPSEL1_1 / GPCALPSEL1_2 / GPCALPSEL1_3
 */
typedef enum {
	E_IM_IIP_PARAM_GPCALPSEL_P0D3 = 0,			/**< D3 of port 0 */
	E_IM_IIP_PARAM_GPCALPSEL_P1D3 = 1,			/**< D3 of port 1 */
	E_IM_IIP_PARAM_GPCALPSEL_GPCALPFIX = 2,		/**< GPCALPFIX */
} E_IM_IIP_PARAM_GPCALPSEL;

/**
   GPCALP register<br>
   GPCALPMODE_0 / GPCALPMODE_1 / GPCALPMODE_2 / GPCALPMODE_3
 */
typedef enum {
	E_IM_IIP_PARAM_GPCALPMODE_THROUGH = 0,	/**< Through */
	E_IM_IIP_PARAM_GPCALPMODE_AND = 1,		/**< AND */
	E_IM_IIP_PARAM_GPCALPMODE_OR = 2,		/**< OR */
	E_IM_IIP_PARAM_GPCALPMODE_EOR = 3,		/**< EOR */
	E_IM_IIP_PARAM_GPCALPMODE_ADD = 4,		/**< ADD */
	E_IM_IIP_PARAM_GPCALPMODE_SUB = 5,		/**< SUB */
	E_IM_IIP_PARAM_GPCALPMODE_ABS = 6,		/**< ABS */
} E_IM_IIP_PARAM_GPCALPMODE;

/**
   SERPASS register<br>
   SEDLY1 / SEDLY2 / SEDLY3
   */
typedef enum {
	E_IM_IIP_PARAM_SEDLY_0_STEP = 0,	/**< 0 step */
	E_IM_IIP_PARAM_SEDLY_4_STEP = 1,	/**< 4 step */
	E_IM_IIP_PARAM_SEDLY_8_STEP = 2,	/**< 8 step */
} E_IM_IIP_PARAM_SEDLY;

/**
   SERPASS register<br>
   SESEL0
   */
typedef enum {
	E_IM_IIP_PARAM_SESEL0_INPUT_SELECTOR = 0,	/**< Input selector */
	E_IM_IIP_PARAM_SESEL0_CALC0_OUT = 1,		/**< Output of calcuration 0 */
} E_IM_IIP_PARAM_SESEL0;

/**
   SERPASS register<br>
   SESEL1
   */
typedef enum {
	E_IM_IIP_PARAM_SESEL1_INPUT_SELECTOR = 0,	/**< Input selector */
	E_IM_IIP_PARAM_SESEL1_COMP_OUT = 1,			/**< Output of comparator */
	E_IM_IIP_PARAM_SESEL1_CALC0_OUT = 2,		/**< Output of calculation 0 */
} E_IM_IIP_PARAM_SESEL1;

/**
   P1SELO_0/P1SELO_1/P1SELO_2/P1SELO_3 register
   */
typedef enum {
	E_IM_IIP_PARAM_P1SELO_CONF6_D0 = 0,			/**< Select D0 value of Config register 6 */
	E_IM_IIP_PARAM_P1SELO_CONF6_D1 = 1,			/**< Select D1 value of Config register 6 */
	E_IM_IIP_PARAM_P1SELO_CONF6_D2 = 2,			/**< Select D2 value of Config register 6 */
	E_IM_IIP_PARAM_P1SELO_ALPHA = 3,			/**< Select Alpha calculator */
} E_IM_IIP_PARAM_P1SELO;

/**
   P0SELO_D3 register
   */
typedef enum {
	E_IM_IIP_PARAM_P0SELO_CONF9_D0 = 0,			/**< Select D0 value of Config register 9 */
	E_IM_IIP_PARAM_P0SELO_CONF9_D1 = 1,			/**< Select D1 value of Config register 9 */
	E_IM_IIP_PARAM_P0SELO_CONF9_D2 = 2,			/**< Select D2 value of Config register 9 */
	E_IM_IIP_PARAM_P0SELO_ALPHA = 3,			/**< Select Alpha calculator */
} E_IM_IIP_PARAM_P0SELO;


///////////////////////////////////////////////////////////////
// FRECT Struct Parameter
///////////////////////////////////////////////////////////////

/** SZPOWH */
typedef enum {
	E_IM_IIP_PARAM_SZPOWH_256 = 0,	/**< 256 pixels */
	E_IM_IIP_PARAM_SZPOWH_8 = 3,	/**< 8 pixels */
	E_IM_IIP_PARAM_SZPOWH_16 = 4,	/**< 16 pixels */
	E_IM_IIP_PARAM_SZPOWH_32 = 5,	/**< 32 pixels */
	E_IM_IIP_PARAM_SZPOWH_64 = 6,	/**< 64 pixels */
	E_IM_IIP_PARAM_SZPOWH_128 = 7	/**< 128 pixels */
} E_IM_IIP_PARAM_SZPOWH;

/** SZPOWV */
typedef enum {
	E_IM_IIP_PARAM_SZPOWV_256 = 0,	/**< 256 pixels */
	E_IM_IIP_PARAM_SZPOWV_8 = 3,	/**< 8 pixels */
	E_IM_IIP_PARAM_SZPOWV_16 = 4,	/**< 16 pixels */
	E_IM_IIP_PARAM_SZPOWV_32 = 5,	/**< 32 pixels */
	E_IM_IIP_PARAM_SZPOWV_64 = 6,	/**< 64 pixels */
	E_IM_IIP_PARAM_SZPOWV_128 = 7	/**< 128 pixels */
} E_IM_IIP_PARAM_SZPOWV;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////
// AFN UNIT Register
///////////////////////////////////////////////////////////////

/** structure of AFNTOPCNF	(0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF	:6;	/**< WAITCONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF	:6;	/**< DATACONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_AFNTOPCNF;

/** structure of AFNLDCNF	(0004h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG			:3;
		ULONG			:1;
		ULONG			:3;
		ULONG			:1;
		ULONG	DTHD	:1;	/**< DTHD<br>Refer to @ref E_IM_IIP_PARAM_DTHD. */
		ULONG			:3;
		ULONG	CBC		:1;	/**< CBC<br>Refer to @ref E_IM_IIP_PARAM_CBC. */
		ULONG			:19;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_AFNLDCNF;

/** structure of AFNCTL (0008h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GHSZ	:1;	/**< GHSZ<br>Refer to @ref E_IM_IIP_PARAM_GHSZ. */
		ULONG			:3;
		ULONG	GVSZ	:4;	/**< GVSZ<br>Refer to @ref E_IM_IIP_PARAM_GVSZ. */
		ULONG	LPLVL	:1;	/**< LPLVL<br>Refer to @ref E_IM_IIP_PARAM_LPLVL. */
		ULONG			:3;
		ULONG			:4;
		ULONG	HDBL	:1;	/**< HDBL<br>Refer to @ref E_IM_IIP_PARAM_HDBL */
		ULONG	VDBL	:1;	/**< VDBL<br>Refer to @ref E_IM_IIP_PARAM_VDBL */
		ULONG	HSMD	:1;	/**< HSMD<br>Refer to @ref E_IM_IIP_PARAM_HSMD */
		ULONG			:13;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_AFNCTL;

/** structure of PFCTL	(000Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PFEXTMC		:3;	/**< PFEXTMC */
		ULONG				:5;
		ULONG	PFEXTADC	:8;	/**< PFEXTADC */
		ULONG	PFSP1		:3;	/**< PFSP1<br>Refer to @ref E_IM_IIP_PARAM_PFSP */
		ULONG				:1;
		ULONG	PFSP2		:3;	/**< PFSP2<br>Refer to @ref E_IM_IIP_PARAM_PFSP */
		ULONG				:1;
		ULONG	PFVSP		:2;	/**< PFVSP<br>Refer to @ref E_IM_IIP_PARAM_PFVSP. */
		ULONG				:2;
		ULONG	PFOFF		:1;	/**< PFOFF<br>Refer to @ref E_IM_IIP_PARAM_PFOFF. */
		ULONG				:3;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_PFCTL;

/** structure of PIXIDEF	(0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG			:2;
		ULONG			:2;
		ULONG	IPIXID	:4;	/**< IPIXID */
		ULONG	EXA		:1;	/**< EXA<br>Refer to @ref E_IM_IIP_PARAM_EXA. */
		ULONG			:3;
		ULONG	CSEL	:2;	/**< CSEL<br>Refer to @ref E_IM_IIP_PARAM_CSEL */
		ULONG			:18;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_PIXIDEF;

/** structure of CALMETHOD	(0014h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPCOL	:1;	/**< OPCOL */
		ULONG			:3;
		ULONG	ITMD	:2;	/**< ITMD<br>Refer to @ref E_IM_IIP_PARAM_ITMD. */
		ULONG	ARA		:2;	/**< ARA<br>Refer to @ref E_IM_IIP_PARAM_ARA. */
		ULONG	FILMD	:1;	/**< FILMD */
		ULONG	DANTI	:1;	/**< DANTI */
		ULONG	AANTI	:1;	/**< AANTI */
		ULONG			:1;
		ULONG	CUBSEL	:3;	/**< CUBSEL<br>Refer to @ref E_IM_IIP_PARAM_CUBSEL. */
		ULONG	SCUB	:1;	/**< SCUB<br>Refer to @ref E_IM_IIP_PARAM_SCUB. */
		ULONG			:16;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_CALMETHOD;

/** structure of FILVAL0	(0018h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FILVALYG	:16;	/**< FILVALYG */
		ULONG	FILVALB		:16;	/**< FILVALB */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_FILVAL0;

/** structure of FILVAL1	(001Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FILVALR :16;	/**< FILVALR */
		ULONG	FILVALA :16;	/**< FILVALA */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_FILVAL1;

/** structure of OPMD	(0020h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPMD0	:3;	/**< OPMD0<br>Refer to @ref E_IM_IIP_PARAM_OPMD. */
		ULONG			:1;
		ULONG			:1;
		ULONG			:27;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_OPMD;

/** structure of OPY	(0030h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPBY	:16;	/**< OPBY */
		ULONG	OPAY	:8;		/**< OPAY */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_OPY;

/** structure of OPB	(0034h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPBB	:16;	/**< OPBB */
		ULONG	OPAB	:8;		/**< OPAB */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_OPB;

/** structure of OPR	(0038h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPBR	:16;	/**< OPBR */
		ULONG	OPAR	:8;		/**< OPAR */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_OPR;

/** structure of OPA	(003Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPBA	:16;	/**< OPBA */
		ULONG	OPAA	:8;		/**< OPAA */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_OPA;

/** structure of CLIPLVLY	(0040h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLIPLVLYH	:16;	/**< CLIPLVLYH */
		ULONG	CLIPLVLYL	:16;	/**< CLIPLVLYL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_CLIPLVLY;

/** structure of CLIPLVLB	(0044h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLIPLVLBH	:16;	/**< CLIPLVLBH */
		ULONG	CLIPLVLBL	:16;	/**< CLIPLVLBL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_CLIPLVLB;

/** structure of CLIPLVLR	(0048h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLIPLVLRH	:16;	/**< CLIPLVLRH */
		ULONG	CLIPLVLRL	:16;	/**< CLIPLVLRL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_CLIPLVLR;

/** structure of CLIPLVLA	(004Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLIPLVLAH	:16;	/**< CLIPLVLAH */
		ULONG	CLIPLVLAL	:16;	/**< CLIPLVLAL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_CLIPLVLA;

/** structure of AFNPCNT	(0050h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	AFNPHCNT	:9;	/**< AFNPHCNT */
		ULONG				:7;
		ULONG	AFNPVCNT	:8;	/**< AFNPVCNT */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_AFNPCNT;

/** structure of AFNPA	(0054h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	AFNPA	:32;	/**< AFNPA */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_AFNPA;

/** structure of OUTSIZE	(0058h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OUTHSZ	:14;	/**< OUTHSZ */
		ULONG			:2;
		ULONG	OUTVSZ	:14;	/**< OUTVSZ */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_OUTSIZE;

/** structure of OUTSTARTPOS	(005Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	STARTX	:14;	/**< STARTX */
		ULONG			:2;
		ULONG	STARTY	:14;	/**< STARTY */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_OUTSTARTPOS;

/** structure of UPLK_TARGET	(0060h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UPSU0	:4;	/**< UPSU0<br>Refer to @ref E_IM_IIP_PARAM_UPSU. */
		ULONG			:4;
		ULONG	UPSU1	:4;	/**< UPSU1<br>Refer to @ref E_IM_IIP_PARAM_UPSU. */
		ULONG			:4;
		ULONG	UPSU2	:4;	/**< UPSU2 */
		ULONG			:4;
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_UPLK_TARGET;

/** structure of DWLK_TARGET	(0064h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DWSU0	:4;	/**< DWSU0<br>Refer to @ref E_IM_IIP_PARAM_DWSU. */
		ULONG			:4;
		ULONG	DWSU1	:4;	/**< DWSU1<br>Refer to @ref E_IM_IIP_PARAM_DWSU. */
		ULONG			:4;
		ULONG	DWSU2	:4;	/**< DWSU2 */
		ULONG			:4;
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_DWLK_TARGET;

/** structure of LKNUM	(0068h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UPNUM	:2;	/**< UPNUM<br>Refer to @ref E_IM_IIP_PARAM_UPNUM. */
		ULONG			:2;
		ULONG	DWNUM	:2;	/**< DWNUM<br>Refer to @ref E_IM_IIP_PARAM_DWNUM. */
		ULONG			:26;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_LKNUM;

/** structure of RING	(006Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	URINGSIZE	:8;	/**< URINGSIZE */
		ULONG	UMARGINIYSZ :4;	/**< UMARGINIYSZ */
		ULONG				:4;
		ULONG	DRINGSIZE	:8;	/**< DRINGSIZE */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_AFN_RING;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_AFN_AFNTOPCNF	AFNTOPCNF;		/**< (0000 - 0003h) */
	U_IM_IIP_PARAM_AFN_AFNLDCNF		AFNLDCNF;		/**< (0004 - 0007h) */
	U_IM_IIP_PARAM_AFN_AFNCTL		AFNCTL;			/**< (0008 - 000Bh) */
	U_IM_IIP_PARAM_AFN_PFCTL		PFCTL;			/**< (000C - 000Fh) */
	U_IM_IIP_PARAM_AFN_PIXIDEF		PIXIDEF;		/**< (0010 - 0013h) */
	U_IM_IIP_PARAM_AFN_CALMETHOD	CALMETHOD;		/**< (0014 - 0017h) */
	U_IM_IIP_PARAM_AFN_FILVAL0		FILVAL0;		/**< (0018 - 001Bh) */
	U_IM_IIP_PARAM_AFN_FILVAL1		FILVAL1;		/**< (001C - 001Fh) */
	U_IM_IIP_PARAM_AFN_OPMD			OPMD;			/**< (0020 - 0023h) */

	UCHAR dmy_0024_002F[0x0030-0x0024]; /**< (0024 - 002Fh) */

	U_IM_IIP_PARAM_AFN_OPY			OPY;			/**< (0030 - 0033h) */
	U_IM_IIP_PARAM_AFN_OPB			OPB;			/**< (0034 - 0037h) */
	U_IM_IIP_PARAM_AFN_OPR			OPR;			/**< (0038 - 003Bh) */
	U_IM_IIP_PARAM_AFN_OPA			OPA;			/**< (003C - 003Fh) */
	U_IM_IIP_PARAM_AFN_CLIPLVLY		CLIPLVLY;		/**< (0040 - 0043h) */
	U_IM_IIP_PARAM_AFN_CLIPLVLB		CLIPLVLB;		/**< (0044 - 0047h) */
	U_IM_IIP_PARAM_AFN_CLIPLVLR		CLIPLVLR;		/**< (0048 - 004Bh) */
	U_IM_IIP_PARAM_AFN_CLIPLVLA		CLIPLVLA;		/**< (004C - 004Fh) */
	U_IM_IIP_PARAM_AFN_AFNPCNT		AFNPCNT;		/**< (0050 - 0053h) */
	U_IM_IIP_PARAM_AFN_AFNPA		AFNPA;			/**< (0054 - 0057h) */
	U_IM_IIP_PARAM_AFN_OUTSIZE		OUTSIZE;		/**< (0058 - 005Bh) */
	U_IM_IIP_PARAM_AFN_OUTSTARTPOS	OUTSTARTPOS;	/**< (005C - 005Fh) */
	U_IM_IIP_PARAM_AFN_UPLK_TARGET	UPLK_TARGET;	/**< (0060 - 0063h) */
	U_IM_IIP_PARAM_AFN_DWLK_TARGET	DWLK_TARGET;	/**< (0064 - 0067h) */
	U_IM_IIP_PARAM_AFN_LKNUM		LKNUM;			/**< (0068 - 006Bh) */
	U_IM_IIP_PARAM_AFN_RING			RING;			/**< (006C - 006Fh) */

	UCHAR dmy_10070_1FFFF[0x20000-0x10070]; /**< 0070 - FFFFh */
} T_IM_IIP_PARAM_AFN_DUMP;


///////////////////////////////////////////////////////////////
// FRECT UNIT Register
///////////////////////////////////////////////////////////////

/** structure of FRTOPCNF	(0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF	:6;	/**< WAITCONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF	:6;	/**< DATACONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_FRTOPCNF;

/** structure of FRLDCNF	(0004h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG			:3;
		ULONG			:1;
		ULONG			:3;
		ULONG			:1;
		ULONG	DTHD	:1;	/**< DTHD<br>Refer to @ref E_IM_IIP_PARAM_DTHD. */
		ULONG			:23;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_FRLDCNF;

/** structure of FRCTL	(0008h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GHSZ	:1;	/**< GHSZ<br>Refer to @ref E_IM_IIP_PARAM_GHSZ. */
		ULONG			:3;
		ULONG	GVSZ	:4;	/**< GVSZ<br>Refer to @ref E_IM_IIP_PARAM_GVSZ. */
		ULONG	LPLVL	:1;	/**< LPLVL<br>Refer to @ref E_IM_IIP_PARAM_LPLVL. */
		ULONG			:3;
		ULONG			:4;
		ULONG	HDBL	:1;	/**< HDBL<br>Refer to @ref E_IM_IIP_PARAM_HDBL */
		ULONG	VDBL	:1;	/**< VDBL<br>Refer to @ref E_IM_IIP_PARAM_VDBL */
		ULONG	HSMD	:1;	/**< HSMD<br>Refer to @ref E_IM_IIP_PARAM_HSMD */
		ULONG			:13;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_FRCTL;

/** structure of PFCTL	(000Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PFEXTMC		:3;	/**< PFEXTMC */
		ULONG				:5;
		ULONG	PFEXTADC	:8;	/**< PFEXTADC */
		ULONG	PFSP1		:3;	/**< PFSP1<br>Refer to @ref E_IM_IIP_PARAM_PFSP */
		ULONG				:1;
		ULONG	PFSP2		:3;	/**< PFSP2<br>Refer to @ref E_IM_IIP_PARAM_PFSP */
		ULONG				:1;
		ULONG	PFVSP		:2;	/**< PFVSP<br>Refer to @ref E_IM_IIP_PARAM_PFVSP. */
		ULONG				:2;
		ULONG	PFOFF		:1;	/**< PFOFF<br>Refer to @ref E_IM_IIP_PARAM_PFOFF. */
		ULONG				:3;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_PFCTL;

/** structure of PIXIDEF	(0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG			:2;
		ULONG			:2;
		ULONG	IPIXID	:4;	/**< IPIXID */
		ULONG	EXA		:1;	/**< EXA<br>Refer to @ref E_IM_IIP_PARAM_EXA. */
		ULONG			:3;
		ULONG	CSEL	:2;	/**< CSEL<br>Refer to @ref E_IM_IIP_PARAM_CSEL */
		ULONG			:18;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_PIXIDEF;

/** structure of CALMETHOD	(0014h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPCOL	:1;	/**< OPCOL */
		ULONG			:3;
		ULONG	ITMD	:2;	/**< ITMD<br>Refer to @ref E_IM_IIP_PARAM_ITMD. */
		ULONG	ARA		:2;	/**< ARA<br>Refer to @ref E_IM_IIP_PARAM_ARA. */
		ULONG	FILMD	:1;	/**< FILMD */
		ULONG	DANTI	:1;	/**< DANTI */
		ULONG	AANTI	:1;	/**< AANTI */
		ULONG			:1;
		ULONG	CUBSEL	:3;	/**< CUBSEL<br>Refer to @ref E_IM_IIP_PARAM_CUBSEL. */
		ULONG	SCUB	:1;	/**< SCUB<br>Refer to @ref E_IM_IIP_PARAM_SCUB. */
		ULONG			:16;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_CALMETHOD;

/** structure of FILVAL0	(0018h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FILVALYG	:16;	/**< FILVALYG */
		ULONG	FILVALB		:16;	/**< FILVALB */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_FILVAL0;

/** structure of FILVAL1	(001Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FILVALR :16;	/**< FILVALR */
		ULONG	FILVALA :16;	/**< FILVALA */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_FILVAL1;

/** structure of OPMD	(0020h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPMD0	:3;	/**< OPMD0<br>Refer to @ref E_IM_IIP_PARAM_OPMD. */
		ULONG			:1;
		ULONG			:1;
		ULONG			:27;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_OPMD;

/** structure of OPY	(0030h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPBY	:16;	/**< OPBY */
		ULONG	OPAY	:8;		/**< OPAY */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_OPY;

/** structure of OPB	(0034h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPBB	:16;	/**< OPBB */
		ULONG	OPAB	:8;		/**< OPAB */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_OPB;

/** structure of OPR	(0038h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPBR	:16;	/**< OPBR */
		ULONG	OPAR	:8;		/**< OPAR */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_OPR;

/** structure of OPA	(003Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPBA	:16;	/**< OPBA */
		ULONG	OPAA	:8;		/**< OPAA */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_OPA;

/** structure of CLIPLVLY	(0040h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLIPLVLYH	:16;	/**< CLIPLVLYH */
		ULONG	CLIPLVLYL	:16;	/**< CLIPLVLYL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_CLIPLVLY;

/** structure of CLIPLVLB	(0044h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLIPLVLBH	:16;	/**< CLIPLVLBH */
		ULONG	CLIPLVLBL	:16;	/**< CLIPLVLBL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_CLIPLVLB;

/** structure of CLIPLVLR	(0048h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLIPLVLRH	:16;	/**< CLIPLVLRH */
		ULONG	CLIPLVLRL	:16;	/**< CLIPLVLRL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_CLIPLVLR;

/** structure of CLIPLVLA	(004Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLIPLVLAH	:16;	/**< CLIPLVLAH */
		ULONG	CLIPLVLAL	:16;	/**< CLIPLVLAL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_CLIPLVLA;

/** structure of FRPCNT (0050h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FRPHCNT :9;	/**< FRPHCNT */
		ULONG			:7;
		ULONG	FRPVCNT :8;	/**< FRPVCNT */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_FRPCNT;

/** structure of PADRS	(0054h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PADRS	:32;	/**< PADRS */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_PADRS;

/** structure of OUTSIZE	(0058h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OUTHSZ	:14;	/**< OUTHSZ */
		ULONG			:2;
		ULONG	OUTVSZ	:14;	/**< OUTVSZ */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_OUTSIZE;

/** structure of OUTSTARTPOS	(005Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	STARTX	:14;	/**< STARTX */
		ULONG			:2;
		ULONG	STARTY	:14;	/**< STARTY */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_OUTSTARTPOS;

/** structure of UPLK_TARGET	(0060h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UPSU0	:4;	/**< UPSU0<br>Refer to @ref E_IM_IIP_PARAM_UPSU. */
		ULONG			:4;
		ULONG	UPSU1	:4;	/**< UPSU1<br>Refer to @ref E_IM_IIP_PARAM_UPSU. */
		ULONG			:4;
		ULONG	UPSU2	:4;	/**< UPSU2 */
		ULONG			:4;
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_UPLK_TARGET;

/** structure of DWLK_TARGET	(0064h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DWSU0	:4;	/**< DWSU0<br>Refer to @ref E_IM_IIP_PARAM_DWSU. */
		ULONG			:4;
		ULONG	DWSU1	:4;	/**< DWSU1<br>Refer to @ref E_IM_IIP_PARAM_DWSU. */
		ULONG			:4;
		ULONG	DWSU2	:4;	/**< DWSU2 */
		ULONG			:4;
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_DWLK_TARGET;

/** structure of LKNUM	(0068h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UPNUM	:2;	/**< UPNUM<br>Refer to @ref E_IM_IIP_PARAM_UPNUM. */
		ULONG			:2;
		ULONG	DWNUM	:2;	/**< DWNUM<br>Refer to @ref E_IM_IIP_PARAM_DWNUM. */
		ULONG			:26;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_LKNUM;

/** structure of RING	(006Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	URINGSIZE	:8;	/**< URINGSIZE */
		ULONG	UMARGINIYSZ :4;	/**< UMARGINIYSZ */
		ULONG				:4;
		ULONG	DRINGSIZE	:8;	/**< DRINGSIZE */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FRECT_RING;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_FRECT_FRTOPCNF		FRTOPCNF;		/**< (0000 - 0003h) */
	U_IM_IIP_PARAM_FRECT_FRLDCNF		FRLDCNF;		/**< (0004 - 0007h) */
	U_IM_IIP_PARAM_FRECT_FRCTL			FRCTL;			/**< (0008 - 000Bh) */
	U_IM_IIP_PARAM_FRECT_PFCTL			PFCTL;			/**< (000C - 000Fh) */
	U_IM_IIP_PARAM_FRECT_PIXIDEF		PIXIDEF;		/**< (0010 - 0013h) */
	U_IM_IIP_PARAM_FRECT_CALMETHOD		CALMETHOD;		/**< (0014 - 0017h) */
	U_IM_IIP_PARAM_FRECT_FILVAL0		FILVAL0;		/**< (0018 - 001Bh) */
	U_IM_IIP_PARAM_FRECT_FILVAL1		FILVAL1;		/**< (001C - 001Fh) */
	U_IM_IIP_PARAM_FRECT_OPMD			OPMD;			/**< (0020 - 0023h) */

	UCHAR dmy_0024_002F[0x0030-0x0024]; /**< (0024 - 002Fh) */

	U_IM_IIP_PARAM_FRECT_OPY			OPY;			/**< (0030 - 0033h) */
	U_IM_IIP_PARAM_FRECT_OPB			OPB;			/**< (0034 - 0037h) */
	U_IM_IIP_PARAM_FRECT_OPR			OPR;			/**< (0038 - 003Bh) */
	U_IM_IIP_PARAM_FRECT_OPA			OPA;			/**< (003C - 003Fh) */
	U_IM_IIP_PARAM_FRECT_CLIPLVLY		CLIPLVLY;		/**< (0040 - 0043h) */
	U_IM_IIP_PARAM_FRECT_CLIPLVLB		CLIPLVLB;		/**< (0044 - 0047h) */
	U_IM_IIP_PARAM_FRECT_CLIPLVLR		CLIPLVLR;		/**< (0048 - 004Bh) */
	U_IM_IIP_PARAM_FRECT_CLIPLVLA		CLIPLVLA;		/**< (004C - 004Fh) */
	U_IM_IIP_PARAM_FRECT_FRPCNT			FRPCNT;			/**< (0050 - 0053h) */
	U_IM_IIP_PARAM_FRECT_PADRS			PADRS;			/**< (0054 - 0057h) */
	U_IM_IIP_PARAM_FRECT_OUTSIZE		OUTSIZE;		/**< (0058 - 005Bh) */
	U_IM_IIP_PARAM_FRECT_OUTSTARTPOS	OUTSTARTPOS;	/**< (005C - 005Fh) */
	U_IM_IIP_PARAM_FRECT_UPLK_TARGET	UPLK_TARGET;	/**< (0060 - 0063h) */
	U_IM_IIP_PARAM_FRECT_DWLK_TARGET	DWLK_TARGET;	/**< (0064 - 0067h) */
	U_IM_IIP_PARAM_FRECT_LKNUM			LKNUM;			/**< (0068 - 006Bh) */
	U_IM_IIP_PARAM_FRECT_RING			RING;			/**< (006C - 006Fh) */

	UCHAR dmy_60070_6FFFF[0x70000-0x60070]; /**< 0070 - FFFFh */
} T_IM_IIP_PARAM_FRECT_DUMP;


///////////////////////////////////////////////////////////////
// 1D UNIT Register
///////////////////////////////////////////////////////////////

/** structure of LD_TOPCNF0 (0004h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF	:6;	/**< WAITCONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:26;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LD_TOPCNF0;

/** structure of LD_TOPCNF1 (0008h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	LINK_UID	:6;	/**< LINK_UID<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:26;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LD_TOPCNF1;

/** structure of PIXIDDEF	(0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG			:2;
		ULONG			:2;
		ULONG	IPIXID	:4;	/**< IPIXID */
		ULONG			:24;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_PIXIDDEF;

/** structure of LD_CTL (0014h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	LD_MODE :2;	/**< LD_MODE<br>Refer to @ref E_IM_IIP_PARAM_LD_MODE. */
		ULONG			:2;
		ULONG	LD_OUT	:2;	/**< LD_OUT */
		ULONG			:2;
		ULONG	SLOW	:1;	/**< SLOW<br>Refer to @ref E_IM_IIP_PARAM_SLOW. */
		ULONG			:3;
		ULONG	SAVERD	:2;	/**< SAVERD */
		ULONG			:2;
		ULONG	CSEL	:2;	/**< CSEL<br>Refer to @ref E_IM_IIP_PARAM_CSEL */
		ULONG			:14;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LD_CTL;

/** structure of LD_PREFETCH	(0018h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PFOFF		:1;	/**< PFOFF<br>Refer to @ref E_IM_IIP_PARAM_PFOFF. */
		ULONG	PF_PDIST	:2;	/**< PF_PDIST<br>Refer to @ref E_IM_IIP_PARAM_PF_PDIST. */
		ULONG	PF_1ST		:1;	/**< PF_1ST<br>Refer to @ref E_IM_IIP_PARAM_PF_1ST. */
		ULONG				:1;
		ULONG				:3;
		ULONG				:2;
		ULONG				:2;
		ULONG				:1;
		ULONG				:3;
		ULONG	PFB_NUM		:5;	/**< PFB_NUM */
		ULONG				:3;
		ULONG	PF_ADD		:5;	/**< PF_ADD */
		ULONG				:3;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LD_PREFETCH;

/** structure of LD_HSIZE	(0020h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	HSTART	:14;	/**< HSTART */
		ULONG			:2;
		ULONG	HEND	:14;	/**< HEND */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LD_HSIZE;

/** structure of LD_VSIZE	(0024h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	VSTART	:14;	/**< VSTART */
		ULONG			:2;
		ULONG	VEND	:14;	/**< VEND */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LD_VSIZE;

/** structure of LD_PHSZ	(0028h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PHSZ0	:14;	/**< PHSZ0 */
		ULONG			:2;
		ULONG	PHSZ1	:14;	/**< PHSZ1 */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LD_PHSZ;

/** structure of LD_PVSZ	(002Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PVSZ0	:14;	/**< PVSZ0 */
		ULONG			:2;
		ULONG	PVSZ1	:14;	/**< PVSZ1 */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LD_PVSZ;

/** structure of UPLK_TARGET	(0030h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UPSU0	:4;	/**< UPSU0<br>Refer to @ref E_IM_IIP_PARAM_UPSU. */
		ULONG			:4;
		ULONG	UPSU1	:4;	/**< UPSU1<br>Refer to @ref E_IM_IIP_PARAM_UPSU. */
		ULONG			:4;
		ULONG	UPSU2	:4;	/**< UPSU2 */
		ULONG			:12;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_UPLK_TARGET;

/** structure of DWLK_TARGET	(0034h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DWSU0	:4;	/**< DWSU0<br>Refer to @ref E_IM_IIP_PARAM_DWSU. */
		ULONG			:4;
		ULONG	DWSU1	:4;	/**< DWSU1<br>Refer to @ref E_IM_IIP_PARAM_DWSU. */
		ULONG			:4;
		ULONG	DWSU2	:4;	/**< DWSU2 */
		ULONG			:12;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_DWLK_TARGET;

/** structure of LKNUM	(0038h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UPNUM	:2;	/**< UPNUM<br>Refer to @ref E_IM_IIP_PARAM_UPNUM. */
		ULONG			:2;
		ULONG	DWNUM	:2;	/**< DWNUM<br>Refer to @ref E_IM_IIP_PARAM_DWNUM. */
		ULONG			:26;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_LKNUM;

/** structure of RING	(003Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	URINGSIZE	:8;	/**< URINGSIZE */
		ULONG	UMARGINIYSZ :4;	/**< UMARGINIYSZ */
		ULONG				:4;
		ULONG	DRINGSIZE	:8;	/**< DRINGSIZE */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_1DL_RING;

/** Define i/o mapping */
typedef struct {
	UCHAR dmy_0000_0003[0x0004-0x0000]; /**< (0000 - 0003h) */

	U_IM_IIP_PARAM_1DL_LD_TOPCNF0	LD_TOPCNF0;		/**< (0004 - 0007h) */
	U_IM_IIP_PARAM_1DL_LD_TOPCNF1	LD_TOPCNF1;		/**< (0008 - 000Bh) */

	UCHAR dmy_000C_000F[0x0010-0x000C]; /**< (000C - 000Fh) */

	U_IM_IIP_PARAM_1DL_PIXIDDEF		PIXIDDEF;		/**< (0010 - 0013h) */
	U_IM_IIP_PARAM_1DL_LD_CTL		LD_CTL;			/**< (0014 - 0017h) */
	U_IM_IIP_PARAM_1DL_LD_PREFETCH	LD_PREFETCH;	/**< (0018 - 001Bh) */

	UCHAR dmy_001C_001F[0x0020-0x001C]; /**< (001C - 001Fh) */

	U_IM_IIP_PARAM_1DL_LD_HSIZE		LD_HSIZE;		/**< (0020 - 0023h) */
	U_IM_IIP_PARAM_1DL_LD_VSIZE		LD_VSIZE;		/**< (0024 - 0027h) */
	U_IM_IIP_PARAM_1DL_LD_PHSZ		LD_PHSZ;		/**< (0028 - 002Bh) */
	U_IM_IIP_PARAM_1DL_LD_PVSZ		LD_PVSZ;		/**< (002C - 002Fh) */
	U_IM_IIP_PARAM_1DL_UPLK_TARGET	UPLK_TARGET;	/**< (0030 - 0033h) */
	U_IM_IIP_PARAM_1DL_DWLK_TARGET	DWLK_TARGET;	/**< (0034 - 0037h) */
	U_IM_IIP_PARAM_1DL_LKNUM		LKNUM;			/**< (0038 - 003Bh) */
	U_IM_IIP_PARAM_1DL_RING			RING;			/**< (003C - 003Fh) */

	UCHAR dmy_00040_0FFFF[0x10000-0x00040]; /**< 0040 - FFFFh */
} T_IM_IIP_PARAM_1DL_DUMP;


///////////////////////////////////////////////////////////////
// BLEND UNIT Register
///////////////////////////////////////////////////////////////

/** structure of BLENDTOPCNF0	(0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF_0	:6;	/**< WAITCONF_0<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF_0	:6;	/**< DATACONF_0<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_BLENDTOPCNF0;

/** structure of BLENDTOPCNF1	(0004h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF_1	:6;	/**< WAITCONF_1<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF_1	:6;	/**< DATACONF_1<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_BLENDTOPCNF1;

/** structure of BLENDTOPCNF2	(0008h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG				:16;
		ULONG	DATACONF_2	:6;	/**< DATACONF_2<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_BLENDTOPCNF2;

/** structure of BLENDCTL	(0080h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	BLD_MD		:3;	/**< BLD_MD<br>Refer to @ref E_IM_IIP_PARAM_BLD_MD. */
		ULONG				:1;
		ULONG	CLP_MD		:2;	/**< CLP_MD<br>Refer to @ref E_IM_IIP_PARAM_CLP_MD. */
		ULONG				:2;
		ULONG	ALPHA_OUT	:2;	/**< ALPHA_OUT<br>Refer to @ref E_IM_IIP_PARAM_ALPHA_OUT. */
		ULONG				:22;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_BLENDCTL;

/** structure of ALPHACTL0	(0088h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ALPHA_STEP	:2;	/**< ALPHA_STEP<br>Refer to @ref E_IM_IIP_PARAM_ALPHA_STEP. */
		ULONG				:2;
		ULONG	ALPHA_SEL	:3;	/**< ALPHA_SEL<br>Refer to @ref E_IM_IIP_PARAM_ALPHA_SEL. */
		ULONG				:25;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_ALPHACTL0;

/** structure of ALPHACTL1	(008Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ALPHA_VAL0	:8;	/**< ALPHA_VAL0 */
		ULONG				:8;
		ULONG	ALPHA_VAL1	:8;	/**< ALPHA_VAL1 */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_ALPHACTL1;

/** structure of ALPHACTL2	(0090h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ALPHA_VAL2	:8;	/**< ALPHA_VAL2 */
		ULONG				:8;
		ULONG	ALPHA_VAL3	:8;	/**< ALPHA_VAL3 */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_ALPHACTL2;

/** structure of WADDCTL0	(0098h)
	structure of WADDCTL1	(009Ch)
	structure of WADDCTL2	(00A0h)
	structure of WADDCTL3	(00A4h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	WADD_KA		:13;	/**< WADD_KA0<br>WADD_KA1<br>WADD_KA2<br>WADD_KA3 */
		ULONG				:3;
		LONG	WADD_KB		:13;	/**< WADD_KB0<br>WADD_KB1<br>WADD_KB2<br>WADD_KB3 */
		ULONG				:3;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_WADDCTL;

/** structure of DIVCTL0	(00ACh) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	RSFT	:4;	/**< RSFT */
		ULONG			:28;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_DIVCTL0;

/** structure of MSKCTL0	(00B4h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	MSKSEL	:1;	/**< MSKSEL */
		ULONG			:3;
		ULONG	DINSEL	:1;	/**< DINSEL */
		ULONG			:27;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_MSKCTL0;

/** structure of MULCTL0	(00BCh)
	structure of MULCTL1	(00C0h)
	structure of MULCTL2	(00C4h)
	structure of MULCTL3	(00C8h)
	structure of MULCTL4	(00CCh)
	structure of MULCTL5	(00D0h)
	structure of MULCTL6	(00D4h)
	structure of MULCTL7	(00D8h) */
typedef union {
	ULONG		word[2];	/**< All bits */
	struct {
		LONG	ADDVAL_A	:15;	/**< ADDVAL_A0<br>ADDVAL_A1<br>ADDVAL_A2<br>ADDVAL_A3 */
		ULONG				:1;
		LONG	ADDVAL_B	:15;	/**< ADDVAL_B0<br>ADDVAL_B1<br>ADDVAL_B2<br>ADDVAL_B3 */
		ULONG				:1;
		LONG	ADDVAL_C	:15;	/**< ADDVAL_C0<br>ADDVAL_C1<br>ADDVAL_C2<br>ADDVAL_C3 */
		ULONG				:1;
		ULONG	SFTSEL		:2;	/**< SFTSEL0<br>SFTSEL1<br>SFTSEL2<br>SFTSEL3<br>Refer to @ref E_IM_IIP_PARAM_SFTSEL */
		ULONG				:14;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_BLEND_MULCTL;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_BLEND_BLENDTOPCNF0	BLENDTOPCNF0;	/**< (0000 - 0003h) */
	U_IM_IIP_PARAM_BLEND_BLENDTOPCNF1	BLENDTOPCNF1;	/**< (0004 - 0007h) */
	U_IM_IIP_PARAM_BLEND_BLENDTOPCNF2	BLENDTOPCNF2;	/**< (0008 - 000Bh) */

	UCHAR dmy_000C_007F[0x0080-0x000C]; /**< (000C - 007Fh) */

	U_IM_IIP_PARAM_BLEND_BLENDCTL		BLENDCTL;		/**< (0080 - 0083h) */

	UCHAR dmy_0084_0087[0x0088-0x0084]; /**< (0084 - 0087h) */

	U_IM_IIP_PARAM_BLEND_ALPHACTL0		ALPHACTL0;		/**< (0088 - 008Bh) */
	U_IM_IIP_PARAM_BLEND_ALPHACTL1		ALPHACTL1;		/**< (008C - 008Fh) */
	U_IM_IIP_PARAM_BLEND_ALPHACTL2		ALPHACTL2;		/**< (0090 - 0093h) */

	UCHAR dmy_0094_0097[0x0098-0x0094]; /**< (0094 - 0097h) */

	U_IM_IIP_PARAM_BLEND_WADDCTL		WADDCTL[4];		/**< (0098 - 00A7h) */

	UCHAR dmy_00A8_00AB[0x00AC-0x00A8]; /**< (00A8 - 00ABh) */

	U_IM_IIP_PARAM_BLEND_DIVCTL0		DIVCTL0;		/**< (00AC - 00AFh) */

	UCHAR dmy_00B0_00B3[0x00B4-0x00B0]; /**< (00B0 - 00B3h) */

	U_IM_IIP_PARAM_BLEND_MSKCTL0		MSKCTL0;		/**< (00B4 - 00B7h) */

	UCHAR dmy_00B8_00BB[0x00BC-0x00B8]; /**< (00B8 - 00BBh) */

	U_IM_IIP_PARAM_BLEND_MULCTL			MULCTL[4];		/**< (00BC - 00DBh) */

	UCHAR dmy_200DC_2FFFF[0x30000-0x200DC]; /**< 00DC - FFFFh */
} T_IM_IIP_PARAM_BLEND_DUMP;


///////////////////////////////////////////////////////////////
// CFC UNIT Register
///////////////////////////////////////////////////////////////

/** structure of CFTOPCONF	(0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DATACONF_0	:6;	/**< DATACONF_0<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG	DATACONF_1	:6;	/**< DATACONF_1<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:18;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CFLOW_CFTOPCONF;

/** structure of DIFF_PREWAIT	(0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DIFF_PREWAIT	:19;	/**< DIFF_PREWAIT */
		ULONG					:13;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CFLOW_DIFF_PREWAIT;

/** structure of DIFF_POSTWAIT	(0014h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DIFF_POSTWAIT	:19;	/**< DIFF_POSTWAIT */
		ULONG					:13;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CFLOW_DIFF_POSTWAIT;

/** structure of MAGPOW (0018h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PREMAG	:7;	/**< PREMAG */
		ULONG			:1;
		ULONG	POSTMAG :7;	/**< POSTMAG */
		ULONG			:17;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CFLOW_MAGPOW;

/** structure of CFMODE (001Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPC_ON	:1;	/**< SPC_ON */
		ULONG			:31;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CFLOW_CFMODE;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_CFLOW_CFTOPCONF		CFTOPCONF;		/**< (0000 - 0003h) */

	UCHAR dmy_0004_000F[0x0010-0x0004]; /**< (0004 - 000Fh) */

	U_IM_IIP_PARAM_CFLOW_DIFF_PREWAIT	DIFF_PREWAIT;	/**< (0010 - 0013h) */
	U_IM_IIP_PARAM_CFLOW_DIFF_POSTWAIT	DIFF_POSTWAIT;	/**< (0014 - 0017h) */
	U_IM_IIP_PARAM_CFLOW_MAGPOW			MAGPOW;			/**< (0018 - 001Bh) */
	U_IM_IIP_PARAM_CFLOW_CFMODE			CFMODE;			/**< (001C - 001Fh) */

	UCHAR dmy_30020_3FFFF[0x40000-0x30020]; /**< 0020 - FFFFh */

} T_IM_IIP_PARAM_CFLOW_DUMP;


///////////////////////////////////////////////////////////////
// CSC UNIT Register
///////////////////////////////////////////////////////////////

/** structure of CSCTOPCNF	(0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF	:6;	/**< WAITCONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF	:6;	/**< DATACONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CSC_CSCTOPCNF;

/** structure of CSCK  (0020h) */
typedef union {
	ULONG		word[2];	/**< All bits */
	struct {
		LONG	CSCK_0		:15;	/**< CSCK[n][0] (0020h) */
		ULONG				:1;
		LONG	CSCK_1		:15;	/**< CSCK[n][1] (0022h) */
		ULONG				:1;
		LONG	CSCK_2		:15;	/**< CSCK[n][2] (0024h) */
		ULONG				:1;
		LONG	CSCK_3		:15;	/**< CSCK[n][3] (0026h) */
		ULONG				:1;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CSC_CSCK;

/** structure of MDSEL	(0040h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	MDSEL	:2;	/**< MDSEL */
		ULONG			:30;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CSC_MDSEL;

/** structure of ALPSEL (0044h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ALPSEL	:1;	/**< ALPSEL<br>Refer to @ref E_IM_IIP_PARAM_ALPSEL. */
		ULONG			:3;
		ULONG	ALOSEL	:1;	/**< ALOSEL<br>Refer to @ref E_IM_IIP_PARAM_ALOSEL. */
		ULONG			:27;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CSC_ALPSEL;

/** structure of ALPVAL (0048h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	ALPVAL	:15;	/**< ALPVAL */
		ULONG			:17;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CSC_ALPVAL;

/** structure of MAXMIN (004Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	MAXMIN0	:2;	/**< MAXMIN0<br>Refer to @ref E_IM_IIP_PARAM_MAXMIN. */
		ULONG			:2;
		ULONG	MAXMIN1	:2;	/**< MAXMIN1<br>Refer to @ref E_IM_IIP_PARAM_MAXMIN. */
		ULONG			:2;
		ULONG	MAXMIN2	:2;	/**< MAXMIN2<br>Refer to @ref E_IM_IIP_PARAM_MAXMIN. */
		ULONG			:2;
		ULONG	MAXMIN3	:2;	/**< MAXMIN3<br>Refer to @ref E_IM_IIP_PARAM_MAXMIN. */
		ULONG			:18;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CSC_MAXMIN;

/** structure of OUTCLIP	(0050h) */
typedef union {
	ULONG		word[4];	/**< All bits */
	struct {
		LONG	CLPMAX0	:15;	/**< CLPMAX0 */
		ULONG			:1;
		LONG	CLPMIN0	:15;	/**< CLPMIN0 */
		ULONG			:1;
		LONG	CLPMAX1	:15;	/**< CLPMAX1 */
		ULONG			:1;
		LONG	CLPMIN1	:15;	/**< CLPMIN1 */
		ULONG			:1;
		LONG	CLPMAX2	:15;	/**< CLPMAX2 */
		ULONG			:1;
		LONG	CLPMIN2	:15;	/**< CLPMIN2 */
		ULONG			:1;
		LONG	CLPMAX3	:9;		/**< CLPMAX3 */
		ULONG			:7;
		LONG	CLPMIN3	:9;		/**< CLPMIN3 */
		ULONG			:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_CSC_OUTCLIP;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_CSC_CSCTOPCNF	CSCTOPCNF;	/**< (0000 - 0003h) */

	UCHAR dmy_0004_001F[0x0020-0x0004]; /**< (0004 - 001Fh) */

	U_IM_IIP_PARAM_CSC_CSCK			CSCK[4];	/**< (0020 - 003Fh) */
	U_IM_IIP_PARAM_CSC_MDSEL		MDSEL;		/**< (0040 - 0043h) */
	U_IM_IIP_PARAM_CSC_ALPSEL		ALPSEL;		/**< (0044 - 0047h) */
	U_IM_IIP_PARAM_CSC_ALPVAL		ALPVAL;		/**< (0048 - 004Bh) */
	U_IM_IIP_PARAM_CSC_MAXMIN		MAXMIN;		/**< (004C - 004Fh) */
	U_IM_IIP_PARAM_CSC_OUTCLIP		OUTCLIP;	/**< (0050 - 005Fh) */

	UCHAR dmy_40060_4FFFF[0x50000-0x40060]; /**< 0060 - FFFFh */
} T_IM_IIP_PARAM_CSC_DUMP;


///////////////////////////////////////////////////////////////
// GPC UNIT Register
///////////////////////////////////////////////////////////////

/** structure of GPCTOPCNF	(0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF	:6;	/**< WAITCONF_0<br>WAITCONF_1<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF	:6;	/**< DATACONF_0<br>DATACONF_1<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_GPC_GPCTOPCNF;

/** structure of GPCCONTROL (0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	COMMONPARA	:2;	/**< COMMONPARA<br>Refer to @ref E_IM_IIP_PARAM_COMMONPARA. */
		ULONG				:2;
		ULONG	MD2DLK		:1;	/**< MD2DLK */
		ULONG				:3;
		ULONG	ALPEN0		:1;	/**< ALPEN0 */
		ULONG				:3;
		ULONG	ALPEN1		:1;	/**< ALPEN1 */
		ULONG				:3;
		ULONG	ATRB0		:4;	/**< ATRB0<br>Refer to @ref E_IM_IIP_PARAM_ATRB */
		ULONG	ATRB1		:4;	/**< ATRB1<br>Refer to @ref E_IM_IIP_PARAM_ATRB */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_GPC_GPCCONTROL;

/** structure of GPCCLOADADR	(0014h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLOADADR	:32;	/**< CLOADADR */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_GPC_GPCCLOADADR;

/** structure of GPCMON (0018h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GPCRUN	:1;		/**< GPCRUN */
		ULONG			:3;
		ULONG	SEQERR	:1;		/**< SEQERR */
		ULONG			:11;
		ULONG	MONPAD	:12;	/**< MONPAD */
		ULONG	MONSEQ	:1;		/**< MONSEQ */
		ULONG	MONLPA	:1;		/**< MONLPA */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_GPC_GPCMON;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_GPC_GPCTOPCNF	GPCTOPCNF[2];	/**< (0000 - 0007h) */

	UCHAR dmy_0008_000F[0x0010-0x0008]; /**< (0008 - 000Fh) */

	U_IM_IIP_PARAM_GPC_GPCCONTROL	GPCCONTROL;		/**< (0010 - 0013h) */
	U_IM_IIP_PARAM_GPC_GPCCLOADADR	GPCCLOADADR;	/**< (0014 - 0017h) */
	U_IM_IIP_PARAM_GPC_GPCMON		GPCMON;			/**< (0018 - 001Bh) */

	UCHAR dmy_7001C_7FFFF[0x80000-0x7001C]; /**< 001C - FFFFh */
} T_IM_IIP_PARAM_GPC_DUMP;


///////////////////////////////////////////////////////////////
// LUT UNIT Register
///////////////////////////////////////////////////////////////

/** structure of LUTTOPCNF	(0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF	:6;	/**< WAITCONF_0<br>WAITCONF_1<br>WAITCONF_2<br>WAITCONF_3<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF	:6;	/**< DATACONF_0<br>DATACONF_1<br>DATACONF_2<br>DATACONF_3<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_LUT_LUTTOPCNF;

/** structure of LUTUNITCTL (0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PORTEN		:4;	/**< PORTEN */
		ULONG	PORT3SELI	:2;	/**< PORT3SELI */
		ULONG				:2;
		ULONG	PORT3SELO	:2;	/**< PORT3SELO */
		ULONG				:2;
		ULONG	BRANCH		:2;	/**< BRANCH */
		ULONG				:2;
		ULONG	PORT2SELI	:2;	/**< PORT2SELI */
		ULONG				:2;
		ULONG	PORT2SELO	:2;	/**< PORT2SELO */
		ULONG				:2;
		ULONG	OUTMD0		:1;	/**< OUTMD0 */
		ULONG	OUTMD1		:1;	/**< OUTMD1 */
		ULONG	OUTMD2		:1;	/**< OUTMD2 */
		ULONG	OUTMD3		:1;	/**< OUTMD3 */
		ULONG				:4;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_LUT_LUTUNITCTL;

/** structure of LUTCTL_A	(0020h)
	structure of LUTCTL_B	(0040h)
	structure of LUTCTL_C	(0060h)
	structure of LUTCTL_D	(0080h)
	structure of LUTCTL_E	(00A0h)
	structure of LUTCTL_F	(00C0h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	LUTBIT		:4;	/**< LUTBIT */
		ULONG	USE			:1;	/**< LUTUSE<br>Refer to @ref E_IM_IIP_PARAM_LUTUSE. */
		ULONG				:3;
		ULONG	SGN			:1;	/**< LUTSGN<br>Refer to @ref E_IM_IIP_PARAM_LUTSGN. */
		ULONG				:3;
		ULONG	SPLBIT		:4;	/**< SPLBIT */
		ULONG				:16;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_LUT_LUTCTL;

/** structure of LUTPRECAL_A	(0030h)
	structure of LUTPRECAL_B	(0050h)
	structure of LUTPRECAL_C	(0070h)
	structure of LUTPRECAL_D	(0090h)
	structure of LUTPRECAL_E	(00B0h)
	structure of LUTPRECAL_F	(00D0h) */
typedef union {
	ULONG	 word[4];	/**< All bits */
	struct {
		ULONG	SHIFT	:4;		/**< LUTSHIFT */
		ULONG			:12;
		LONG	OFSET	:15;	/**< LUTOFSEL */
		ULONG			:1;
		LONG	CLPMAX	:15;	/**< LUTCLPMAX */
		ULONG			:1;
		LONG	THMAX	:15;	/**< LUTTHMAX */
		ULONG			:1;
		LONG	CLPMIN	:15;	/**< LUTCLPMIN */
		ULONG			:1;
		LONG	THMIN	:15;	/**< LUTTHMIN */
		ULONG			:1;
		ULONG	ABS		:1;		/**< LUTABS */
		ULONG			:15;
		ULONG	LUTADRS	:15;	/**< LUTADRS */
		ULONG			:1;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_LUT_LUTPRECAL;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_LUT_LUTTOPCNF	LUTTOPCNF[4];	/**< (0000 - 000Fh) */
	U_IM_IIP_PARAM_LUT_LUTUNITCTL	LUTUNITCTL;		/**< (0010 - 0013h) */

	UCHAR dmy_0014_001F[0x0020-0x0014]; /**< (0014 - 001Fh) */

	U_IM_IIP_PARAM_LUT_LUTCTL		LUTCTL_A;		/**< (0020 - 0023h) */

	UCHAR dmy_0024_002F[0x0030-0x0024]; /**< (0024 - 002Fh) */

	U_IM_IIP_PARAM_LUT_LUTPRECAL	LUTPRECAL_A;	/**< (0030 - 003Fh) */
	U_IM_IIP_PARAM_LUT_LUTCTL		LUTCTL_B;		/**< (0040 - 0043h) */

	UCHAR dmy_0044_004F[0x0050-0x0044]; /**< (0044 - 004Fh) */

	U_IM_IIP_PARAM_LUT_LUTPRECAL	LUTPRECAL_B;	/**< (0050 - 005Fh) */
	U_IM_IIP_PARAM_LUT_LUTCTL		LUTCTL_C;		/**< (0060 - 0063h) */

	UCHAR dmy_0064_006F[0x0070-0x0064]; /**< (0064 - 006Fh) */

	U_IM_IIP_PARAM_LUT_LUTPRECAL	LUTPRECAL_C;	/**< (0070 - 007Fh) */
	U_IM_IIP_PARAM_LUT_LUTCTL		LUTCTL_D;		/**< (0080 - 0083h) */

	UCHAR dmy_0084_008F[0x0090-0x0084]; /**< (0084 - 008Fh) */

	U_IM_IIP_PARAM_LUT_LUTPRECAL	LUTPRECAL_D;	/**< (0090 - 009Fh) */
	U_IM_IIP_PARAM_LUT_LUTCTL		LUTCTL_E;		/**< (00A0 - 00A3h) */

	UCHAR dmy_00A4_00AF[0x00B0-0x00A4]; /**< (00A4 - 00AFh) */

	U_IM_IIP_PARAM_LUT_LUTPRECAL	LUTPRECAL_E;	/**< (00B0 - 00BFh) */
	U_IM_IIP_PARAM_LUT_LUTCTL		LUTCTL_F;		/**< (00C0 - 00C3h) */

	UCHAR dmy_00C4_00CF[0x00D0-0x00C4]; /**< (00C4 - 00CFh) */

	U_IM_IIP_PARAM_LUT_LUTPRECAL	LUTPRECAL_F;	/**< (00D0 - 00DFh) */

	UCHAR dmy_800E0_8FFFF[0x90000-0x800E0]; /**< 00E0 - FFFFh */
} T_IM_IIP_PARAM_LUT_DUMP;


///////////////////////////////////////////////////////////////
// MFT UNIT Register
///////////////////////////////////////////////////////////////

/** structure of MFTTOPCONF (0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF	:6;	/**< WAITCONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF	:6;	/**< DATACONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MFT_MFTTOPCONF;

/** structure of MFTMD	(0004h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	MFTMD	:1;	/**< MFTMD<br>Refer to @ref E_IM_IIP_PARAM_MFTMD. */
		ULONG			:31;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MFT_MFTMD;

/** structure of FLTTPSZ	(0008h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FLTTPSZ	:11;	/**< FLTTPSZ */
		ULONG			:21;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MFT_FLTTPSZ;

/** structure of FLTOUTHSZ	(000Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FLTOUTHSZ	:15;	/**< FLTOUTHSZ */
		ULONG				:17;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MFT_FLTOUTHSZ;

/** structure of MFTCTLD0	(0010h)
	structure of MFTCTLD1	(0014h)
	structure of MFTCTLD2	(0018h)
	structure of MFTCTLD3	(001Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FIX		:1;		/**< D0FIX<br>D1FIX<br>D2FIX<br>Refer to @ref E_IM_IIP_PARAM_DNFIX. */
		ULONG			:15;
		LONG	VAL		:15;	/**< D0VAL<br>D1VAL<br>D2VAL<br>D3VAL */
		ULONG			:1;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MFT_MFTCTL;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_MFT_MFTTOPCONF	FTTOPCONF;	/**< (0000 - 0003h) */
	U_IM_IIP_PARAM_MFT_MFTMD		MFTMD;		/**< (0004 - 0007h) */
	U_IM_IIP_PARAM_MFT_FLTTPSZ		FLTTPSZ;	/**< (0008 - 000Bh) */
	U_IM_IIP_PARAM_MFT_FLTOUTHSZ	FLTOUTHSZ;	/**< (000C - 000Fh) */
	U_IM_IIP_PARAM_MFT_MFTCTL		MFTCTLD[4];	/**< (0010 - 001Fh) */

	UCHAR dmy_90020_9FFFF[0xA0000-0x90020]; /**< 0020 - FFFFh */
} T_IM_IIP_PARAM_MFT_DUMP;


///////////////////////////////////////////////////////////////
// MON UNIT Register
///////////////////////////////////////////////////////////////

/** structure of MONCTL (0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	HISTAE	:1;	/**< HISTAE */
		ULONG			:3;
		ULONG	HISTBE	:1;	/**< HISTBE */
		ULONG			:3;
		ULONG	HISTCE	:1;	/**< HISTCE */
		ULONG			:3;
		ULONG	HISTDE	:1;	/**< HISTDE */
		ULONG			:3;
		ULONG	MAXE	:1;	/**< MAXE */
		ULONG			:3;
		ULONG	MINE	:1;	/**< MINE */
		ULONG			:3;
		ULONG	ADDE	:1;	/**< ADDE */
		ULONG			:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MON_MONCTL;

/** structure of MONDSEL	(0004h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DSEL	:3;	/**< DSEL<br>Refer to @ref E_IM_IIP_PARAM_DSEL. */
		ULONG			:1;
		ULONG	SGNEN0	:1;	/**< SGNEN0<br>Refer to @ref E_IM_IIP_PARAM_SGNEN. */
		ULONG	SGNEN1	:1;	/**< SGNEN1<br>Refer to @ref E_IM_IIP_PARAM_SGNEN. */
		ULONG	SGNEN2	:1;	/**< SGNEN2<br>Refer to @ref E_IM_IIP_PARAM_SGNEN. */
		ULONG	SGNEN3	:1;	/**< SGNEN3<br>Refer to @ref E_IM_IIP_PARAM_SGNEN. */
		ULONG			:24;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MON_MONDSEL;

/** structure of HISTCTL	(0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	HISTSEL			:2;	/**< HISTSEL_0<br>HISTSEL_1<br>HISTSEL_2<br>HISTSEL_3 */
		ULONG					:2;
		ULONG	BITSEL			:3;	/**< BITSEL_0<br>BITSEL_1<br>BITSEL_2<br>BITSEL_3<br>Refer to @ref E_IM_IIP_PARAM_BITSEL. */
		ULONG					:1;
		ULONG	HISTSKIP_H		:6;	/**< HISTSKIP_H_0<br>HISTSKIP_H_1<br>HISTSKIP_H_2<br>HISTSKIP_H_3 */
		ULONG					:2;
		ULONG	HISTSKIP_V		:6;	/**< HISTSKIP_V_0<br>HISTSKIP_V_1<br>HISTSKIP_V_2<br>HISTSKIP_V_3 */
		ULONG					:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MON_HISTCTL;

/** structure of ABSMAX (0020h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ABSMAX	:1;	/**< ABSMAX */
		ULONG			:31;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MON_ABSMAX;

/** structure of ABSMIN (0024h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ABSMIN	:1;	/**< ABSMIN */
		ULONG			:31;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MON_ABSMIN;

/** structure of ABSADD (0040h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ABSADD	:1;	/**< ABSADD */
		ULONG			:31;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MON_ABSADD;

/** structure of OVERVALUE	(0050h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	OVERVALUE :32;	/**< OVERVALUE */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_MON_OVERVALUE;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_MON_MONCTL		MONCTL;			/**< (0000 - 0003h) */
	U_IM_IIP_PARAM_MON_MONDSEL		MONDSEL;		/**< (0004 - 0007h) */

	UCHAR dmy_0008_000F[0x0010-0x0008]; /**< (0008 - 000Fh) */

	U_IM_IIP_PARAM_MON_HISTCTL		HISTCTL[4];		/**< (0010 - 001Fh) */
	U_IM_IIP_PARAM_MON_ABSMAX		ABSMAX;			/**< (0020 - 0023h) */
	U_IM_IIP_PARAM_MON_ABSMIN		ABSMIN;			/**< (0024 - 0027h) */

	UCHAR dmy_0028_003F[0x0040-0x0028];		/**< (0028 - 003Fh) */

	U_IM_IIP_PARAM_MON_ABSADD		ABSADD;			/**< (0040 - 0043h) */

	UCHAR dmy_0044_004F[0x0050-0x0044];		/**< (0044 - 004Fh) */

	U_IM_IIP_PARAM_MON_OVERVALUE	OVERVALUE[4];	/**< (0050 - 005Fh) */

	UCHAR dmy_A0060_AFFFF[0xB0000-0xA0060]; /**< 0060 - FFFFh */
} T_IM_IIP_PARAM_MON_DUMP;


///////////////////////////////////////////////////////////////
// FLT UNIT Register
///////////////////////////////////////////////////////////////

/** structure of TDLTOPCNF	(0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF_0	:6;	/**< WAITCONF_0<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG	WAITCONF_1	:6;	/**< WAITCONF_1<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG	DATACONF	:6;	/**< DATACONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_TDLTOPCNF;

/** structure of PIXIDDEF	(0004h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	IPIXID0 :4;	/**< IPIXID0 */
		ULONG			:12;
		ULONG	IPIXID1 :4;	/**< IPIXID1 */
		ULONG			:12;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_PIXIDDEF;

/** structure of TDL_CTL	(0008h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FM			:2;	/**< FM<br>Refer to @ref E_IM_IIP_PARAM_FM. */
		ULONG				:2;
		ULONG	LNUM		:1;	/**< LNUM<br>Refer to @ref E_IM_IIP_PARAM_LNUM. */
		ULONG				:3;
		ULONG	SAVERD		:2;	/**< SAVERD */
		ULONG				:2;
		ULONG	INPUT_MODE	:2;	/**< INPUT_MODE<br>Refer to @ref E_IM_IIP_PARAM_INPUT_MODE. */
		ULONG				:2;
		ULONG	CSEL		:2;	/**< CSEL<br>Refer to @ref E_IM_IIP_PARAM_CSEL */
		ULONG				:14;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_TDL_CTL;

/** structure of CALC_MD	(000Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CALC_MD :2;	/**< CALC_MD<br>Refer to @ref E_IM_IIP_PARAM_CALC_MD */
		ULONG			:30;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_CALC_MD;

/** structure of TDL_HSIZE	(0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	HSTART	:14;	/**< HSTART */
		ULONG			:2;
		ULONG	HEND	:14;	/**< HEND */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_TDL_HSIZE;

/** structure of TDL_VSIZE	(0014h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	VSTART	:14;	/**< VSTART */
		ULONG			:2;
		ULONG	VEND	:14;	/**< VEND */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_TDL_VSIZE;

/** structure of TDL_PHSZ	(0018h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PHSZ0	:14;	/**< PHSZ0 */
		ULONG			:2;
		ULONG	PHSZ1	:14;	/**< PHSZ1 */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_TDL_PHSZ;

/** structure of TDL_PVSZ	(001Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PVSZ0	:14;	/**< PVSZ0 */
		ULONG			:2;
		ULONG	PVSZ1	:14;	/**< PVSZ1 */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_TDL_PVSZ;

/** structure of TDL_PF (0020h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PF_ON		:1;		/**< PF_ON<br>Refer to @ref E_IM_IIP_PARAM_PF_ON. */
		ULONG				:1;
		ULONG	PF_MD		:1;		/**< PF_MD<br>Refer to @ref E_IM_IIP_PARAM_PF_MD. */
		ULONG				:1;
		ULONG	PF_NUM		:6;		/**< PF_NUM */
		ULONG				:2;
		ULONG	PF_ADJ_NUM	:4;		/**< PF_ADJ_NUM */
		ULONG	PF_ADJ_VAL	:16;	/**< PF_ADJ_VAL */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_TDL_PF;

/** structure of UNIT_OUT	(0024h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UNIT_OUT_A_D0	:3;	/**< UNIT_OUT_A_D0<br>Refer to @ref E_IM_IIP_PARAM_UNIT_OUT. */
		ULONG					:1;
		ULONG	UNIT_OUT_A_D1	:3;	/**< UNIT_OUT_A_D1<br>Refer to @ref E_IM_IIP_PARAM_UNIT_OUT. */
		ULONG					:1;
		ULONG	UNIT_OUT_A_D2	:3;	/**< UNIT_OUT_A_D2<br>Refer to @ref E_IM_IIP_PARAM_UNIT_OUT. */
		ULONG					:5;
		ULONG	UNIT_OUT_B_D0	:3;	/**< UNIT_OUT_B_D0<br>Refer to @ref E_IM_IIP_PARAM_UNIT_OUT. */
		ULONG					:1;
		ULONG	UNIT_OUT_B_D1	:3;	/**< UNIT_OUT_B_D1<br>Refer to @ref E_IM_IIP_PARAM_UNIT_OUT. */
		ULONG					:1;
		ULONG	UNIT_OUT_B_D2	:3;	/**< UNIT_OUT_B_D2<br>Refer to @ref E_IM_IIP_PARAM_UNIT_OUT. */
		ULONG					:5;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_UNIT_OUT;

/** structure of FIN_CLP_A_D0	(0028h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	FIN_CLVLP_A_D0	:15;	/**< FIN_CLVLP_A_D0 */
		ULONG					:1;
		LONG	FIN_CLVLM_A_D0	:15;	/**< FIN_CLVLM_A_D0 */
		ULONG					:1;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_FIN_CLP_A_D0;

/** structure of FIN_CLP_A_D1	(002Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	FIN_CLVLP_A_D1	:9;	/**< FIN_CLVLP_A_D1 */
		ULONG					:7;
		LONG	FIN_CLVLM_A_D1	:9;	/**< FIN_CLVLM_A_D1 */
		ULONG					:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_FIN_CLP_A_D1;

/** structure of FIN_CLP_A_D2	(0030h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	FIN_CLVLP_A_D2	:9;	/**< FIN_CLVLP_A_D2 */
		ULONG					:7;
		LONG	FIN_CLVLM_A_D2	:9;	/**< FIN_CLVLM_A_D2 */
		ULONG					:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_FIN_CLP_A_D2;

/** structure of FIN_CLP_B_D0	(0034h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	FIN_CLVLP_B_D0	:15;	/**< FIN_CLVLP_B_D0 */
		ULONG					:1;
		LONG	FIN_CLVLM_B_D0	:15;	/**< FIN_CLVLM_B_D0 */
		ULONG					:1;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_FIN_CLP_B_D0;

/** structure of FIN_CLP_B_D1	(0038h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	FIN_CLVLP_B_D1	:9;	/**< FIN_CLVLP_B_D1 */
		ULONG					:7;
		LONG	FIN_CLVLM_B_D1	:9;	/**< FIN_CLVLM_B_D1 */
		ULONG					:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_FIN_CLP_B_D1;

/** structure of FIN_CLP_B_D2	(003Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	FIN_CLVLP_B_D2	:9;	/**< FIN_CLVLP_B_D2 */
		ULONG					:7;
		LONG	FIN_CLVLM_B_D2	:9;	/**< FIN_CLVLM_B_D2 */
		ULONG					:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_FIN_CLP_B_D2;

/** structure of ISO_MD (0040h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ISO_JM	:1;	/**< ISO_JM */
		ULONG			:15;
		ULONG	ISO_CM	:1;	/**< ISO_CM */
		ULONG			:15;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_ISO_MD;

/** structure of ISO_TH_D0	(0044h)
	structure of ISO_TH_D1	(0048h)
	structure of ISO_TH_D2	(004Ch)
	structure of ISO_TH_D3	(0050h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ISO_UTH_D	:15;	/**< ISO_UTH_D */
		ULONG				:1;
		ULONG	ISO_LTH_D	:15;	/**< ISO_LTH_D */
		ULONG				:1;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_ISO_TH_D;

/** structure of ISO_GAIN_D0	(0054h)
	structure of ISO_GAIN_D1	(0058h)
	structure of ISO_GAIN_D2	(005Ch)
	structure of ISO_GAIN_D3	(0060h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ISO_GA_D	:12;	/**< ISO_GA_D */
		ULONG				:4;
		ULONG	ISO_GB_D	:12;	/**< ISO_GB_D */
		ULONG				:4;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_ISO_GAIN_D;

/** structure of SPF_MD (0064h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_MD	:2;	/**< SPF_MD<br>Refer to @ref E_IM_IIP_PARAM_SPF_MD. */
		ULONG			:30;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_MD;

/** structure of SPF_FLTW_D0	(0080h)
	structure of SPF_FLTW_D1	(00C0h)
	structure of SPF_FLTW_D2	(0100h)
	structure of SPF_FLTW_D3	(0140h) */
typedef union {
	ULONG		word[9];	/**< All bits */
	struct {
		LONG	SPF_FLTW_0		:13;	/**< SPF_FLTW_0 (+00h) */
		ULONG					:3;
		LONG	SPF_FLTW_1		:13;	/**< SPF_FLTW_1 (+02h) */
		ULONG					:3;
		LONG	SPF_FLTW_2		:13;	/**< SPF_FLTW_2 (+04h) */
		ULONG					:3;
		LONG	SPF_FLTW_3		:13;	/**< SPF_FLTW_3 (+06h) */
		ULONG					:3;
		LONG	SPF_FLTW_4		:13;	/**< SPF_FLTW_4 (+08h) */
		ULONG					:3;
		LONG	SPF_FLTW_5		:13;	/**< SPF_FLTW_5 (+0Ah) */
		ULONG					:3;
		LONG	SPF_FLTW_6		:13;	/**< SPF_FLTW_6 (+0Ch) */
		ULONG					:3;
		LONG	SPF_FLTW_7		:13;	/**< SPF_FLTW_7 (+0Eh) */
		ULONG					:3;
		LONG	SPF_FLTW_8		:13;	/**< SPF_FLTW_8 (+10h) */
		ULONG					:3;
		LONG	SPF_FLTW_9		:13;	/**< SPF_FLTW_9 (+12h) */
		ULONG					:3;
		LONG	SPF_FLTW_10		:13;	/**< SPF_FLTW_10 (+14h) */
		ULONG					:3;
		LONG	SPF_FLTW_11		:13;	/**< SPF_FLTW_11 (+16h) */
		ULONG					:3;
		LONG	SPF_FLTW_12		:13;	/**< SPF_FLTW_12 (+18h) */
		ULONG					:3;
		LONG	SPF_FLTW_13		:13;	/**< SPF_FLTW_13 (+1Ah) */
		ULONG					:3;
		LONG	SPF_FLTW_14		:13;	/**< SPF_FLTW_14 (+1Ch) */
		ULONG					:3;
		LONG	SPF_FLTW_15		:13;	/**< SPF_FLTW_15 (+1Eh) */
		ULONG					:3;
		LONG	SPF_FLTW_16		:13;	/**< SPF_FLTW_16 (+20h) */
		ULONG					:3;
		ULONG					:16;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_FLTW;

/** structure of SPF_VTH_D0 (0180h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_UTH_D0	:15;	/**< SPF_UTH_D0 */
		ULONG				:1;
		ULONG	SPF_LTH_D0	:15;	/**< SPF_LTH_D0 */
		ULONG				:1;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_VTH_D0;

/** structure of SPF_VTH_D1 (0184h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_UTH_D1	:9;	/**< SPF_UTH_D1 */
		ULONG				:7;
		ULONG	SPF_LTH_D1	:9;	/**< SPF_LTH_D1 */
		ULONG				:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_VTH_D1;

/** structure of SPF_VTH_D2 (0188h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_UTH_D2	:9;	/**< SPF_UTH_D2 */
		ULONG				:7;
		ULONG	SPF_LTH_D2	:9;	/**< SPF_LTH_D2 */
		ULONG				:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_VTH_D2;

/** structure of SPF_CORE_D0	(018Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_CTHP_D0	:14;	/**< SPF_CTHP_D0 */
		ULONG				:2;
		ULONG	SPF_CTHM_D0	:14;	/**< SPF_CTHM_D0 */
		ULONG				:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_CORE_D0;

/** structure of SPF_CORE_D1	(0190h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_CTHP_D1	:8;	/**< SPF_CTHP_D1 */
		ULONG				:8;
		ULONG	SPF_CTHM_D1	:8;	/**< SPF_CTHM_D1 */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_CORE_D1;

/** structure of SPF_CORE_D2	(0194h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_CTHP_D2	:8;	/**< SPF_CTHP_D2 */
		ULONG				:8;
		ULONG	SPF_CTHM_D2	:8;	/**< SPF_CTHM_D2 */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_CORE_D2;

/** structure of SPF_CLP_D0 (0198h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	SPF_CLVLP_D0	:15;	/**< SPF_CLVLP_D0 */
		ULONG					:1;
		LONG	SPF_CLVLM_D0	:15;	/**< SPF_CLVLM_D0 */
		ULONG					:1;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_CLP_D0;

/** structure of SPF_CLP_D1 (019Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	SPF_CLVLP_D1	:9;	/**< SPF_CLVLP_D1 */
		ULONG					:7;
		LONG	SPF_CLVLM_D1	:9;	/**< SPF_CLVLM_D1 */
		ULONG					:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_CLP_D1;

/** structure of SPF_CLP_D2 (01A0h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	SPF_CLVLP_D2	:9;	/**< SPF_CLVLP_D2 */
		ULONG					:7;
		LONG	SPF_CLVLM_D2	:9;	/**< SPF_CLVLM_D2 */
		ULONG					:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_CLP_D2;

/** structure of SPF_EOADD_D0	(01A4h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_EOADD_D0	:14;	/**< SPF_EOADD_D0 */
		ULONG					:18;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_EOADD_D0;

/** structure of SPF_EOADD_D1	(01A8h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_EOADD_D1	:8;	/**< SPF_EOADD_D1 */
		ULONG					:24;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_EOADD_D1;

/** structure of SPF_EOADD_D2	(01ACh) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SPF_EOADD_D2	:8;	/**< SPF_EOADD_D2 */
		ULONG					:24;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_SPF_EOADD_D2;

/** structure of EP_MD	(01B0h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	EP_MD	:2;	/**< EP_MD<br>Refer to @ref E_IM_IIP_PARAM_EP_MD. */
		ULONG			:30;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_EP_MD;

/** structure of EP_GA_D0	(01B8h)
	structure of EP_GA_D1	(01C0h)
	structure of EP_GA_D2	(01C8h)
	structure of EP_GA_D3	(01D0h) */
typedef union {
	ULONG		word[2];	/**< All bits */
	struct {
		LONG	EP_GA_0		:11;	/**< EP_GA[0] (+00h) */
		ULONG				:5;
		LONG	EP_GA_1		:11;	/**< EP_GA[1] (+02h) */
		ULONG				:5;
		LONG	EP_GA_2		:11;	/**< EP_GA[2] (+04h) */
		ULONG				:5;
		ULONG				:16;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_EP_GA;

/** structure of EP_OFS_D0	(01D8h)
	structure of EP_OFS_D1	(01E0h)
	structure of EP_OFS_D2	(01E8h)
	structure of EP_OFS_D3	(01F0h) */
typedef union {
	ULONG		word[2];	/**< All bits */
	struct {
		LONG	EP_OFS_0	:15;	/**< EP_OFS[0] (+00h) */
		ULONG				:1;
		LONG	EP_OFS_1	:15;	/**< EP_OFS[1] (+02h) */
		ULONG				:1;
		LONG	EP_OFS_2	:15;	/**< EP_OFS[2] (+04h) */
		ULONG				:1;
		ULONG				:16;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_EP_OFS;

/** structure of EP_BD_D0	(01F8h)
	structure of EP_BD_D1	(0200h)
	structure of EP_BD_D2	(0208h)
	structure of EP_BD_D3	(0210h) */
typedef union {
	ULONG		word[2];	/**< All bits */
	struct {
		LONG	EP_BD_0		:15;	/**< EP_BD[0] (+00h) */
		ULONG				:1;
		LONG	EP_BD_1		:15;	/**< EP_BD[1] (+02h) */
		ULONG				:1;
		LONG	EP_BD_2		:15;	/**< EP_BD[2] (+04h) */
		ULONG				:1;
		ULONG				:16;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_EP_BD;

/** structure of ED_FM	(0218h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ED_FM_0 :1;	/**< ED_FM_0<br>Refer to @ref E_IM_IIP_PARAM_ED_FM. */
		ULONG			:7;
		ULONG	ED_FM_1 :1;	/**< ED_FM_1<br>Refer to @ref E_IM_IIP_PARAM_ED_FM. */
		ULONG			:7;
		ULONG	ED_FM_2 :1;	/**< ED_FM_2<br>Refer to @ref E_IM_IIP_PARAM_ED_FM. */
		ULONG			:7;
		ULONG	ED_FM_3 :1;	/**< ED_FM_3<br>Refer to @ref E_IM_IIP_PARAM_ED_FM. */
		ULONG			:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_ED_FM;

/** structure of ED_OUT (021Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ED_OUT_0	:1;	/**< ED_OUT_0<br>Refer to @ref E_IM_IIP_PARAM_ED_OUT. */
		ULONG				:7;
		ULONG	ED_OUT_1	:1;	/**< ED_OUT_1<br>Refer to @ref E_IM_IIP_PARAM_ED_OUT. */
		ULONG				:7;
		ULONG	ED_OUT_2	:1;	/**< ED_OUT_2<br>Refer to @ref E_IM_IIP_PARAM_ED_OUT. */
		ULONG				:7;
		ULONG	ED_OUT_3	:1;	/**< ED_OUT_3<br>Refer to @ref E_IM_IIP_PARAM_ED_OUT. */
		ULONG				:7;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_ED_OUT;

/** structure of ADAPT_K	(0220h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	ADAPT_K0	:6;	/**< ADAPT_K0 */
		ULONG				:2;
		LONG	ADAPT_K1	:6;	/**< ADAPT_K1 */
		ULONG				:2;
		LONG	ADAPT_K2	:6;	/**< ADAPT_K2 */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_ADAPT_K;

/** structure of ADAPT_TH	(0224h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ADAPT_TH	:10;	/**< ADAPT_TH */
		ULONG				:22;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_ADAPT_TH;

/** structure of ADAPT_DIV	(0228h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ADAPT_DIV	:3;	/**< ADAPT_DIV<br>Refer to @ref E_IM_IIP_PARAM_ADAPT_DIV. */
		ULONG				:29;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_ADAPT_DIV;

/** structure of UPLK_TARGET	(022Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UPSU0	:4;	/**< UPSU0<br>Refer to @ref E_IM_IIP_PARAM_UPSU. */
		ULONG			:4;
		ULONG	UPSU1	:4;	/**< UPSU1<br>Refer to @ref E_IM_IIP_PARAM_UPSU. */
		ULONG			:4;
		ULONG	UPSU2	:4;	/**< UPSU2 */
		ULONG			:12;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_UPLK_TARGET;

/** structure of DWLK_TARGET	(0230h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DWSU0	:4;	/**< DWSU0<br>Refer to @ref E_IM_IIP_PARAM_DWSU. */
		ULONG			:4;
		ULONG	DWSU1	:4;	/**< DWSU1<br>Refer to @ref E_IM_IIP_PARAM_DWSU. */
		ULONG			:4;
		ULONG	DWSU2	:4;	/**< DWSU2 */
		ULONG			:12;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_DWLK_TARGET;

/** structure of LKNUM	(0234h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	UPNUM	:2;	/**< UPNUM<br>Refer to @ref E_IM_IIP_PARAM_UPNUM. */
		ULONG			:2;
		ULONG	DWNUM	:2;	/**< DWNUM<br>Refer to @ref E_IM_IIP_PARAM_DWNUM. */
		ULONG			:26;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_LKNUM;

/** structure of RING	(0238h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	URINGSIZE	:8;	/**< URINGSIZE */
		ULONG	UMARGINIYSZ :4;	/**< UMARGINIYSZ */
		ULONG				:4;
		ULONG	DRINGSIZE	:8;	/**< DRINGSIZE */
		ULONG				:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_RING;

/** structure of BF_MD	(023Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	BF_FMT	:1;	/**< BF_FMT<br>Refer to @ref E_IM_IIP_PARAM_BF_FMT. */
		ULONG			:3;
		ULONG	BF_PRT	:1;	/**< BF_PRT<br>Refer to @ref E_IM_IIP_PARAM_BF_PRT. */
		ULONG			:27;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_FLT_BF_MD;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_FLT_TDLTOPCNF	TDLTOPCNF;		/**< (0000 - 0003h) */
	U_IM_IIP_PARAM_FLT_PIXIDDEF		PIXIDDEF;		/**< (0004 - 0007h) */
	U_IM_IIP_PARAM_FLT_TDL_CTL		TDL_CTL;		/**< (0008 - 000Bh) */
	U_IM_IIP_PARAM_FLT_CALC_MD		CALC_MD;		/**< (000C - 000Fh) */
	U_IM_IIP_PARAM_FLT_TDL_HSIZE	TDL_HSIZE;		/**< (0010 - 0013h) */
	U_IM_IIP_PARAM_FLT_TDL_VSIZE	TDL_VSIZE;		/**< (0014 - 0017h) */
	U_IM_IIP_PARAM_FLT_TDL_PHSZ		TDL_PHSZ;		/**< (0018 - 001Bh) */
	U_IM_IIP_PARAM_FLT_TDL_PVSZ		TDL_PVSZ;		/**< (001C - 001Fh) */
	U_IM_IIP_PARAM_FLT_TDL_PF		TDL_PF;			/**< (0020 - 0023h) */
	U_IM_IIP_PARAM_FLT_UNIT_OUT		UNIT_OUT;		/**< (0024 - 0027h) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_A_D0	FIN_CLP_A_D0;	/**< (0028 - 002Bh) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_A_D1	FIN_CLP_A_D1;	/**< (002C - 002Fh) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_A_D2	FIN_CLP_A_D2;	/**< (0030 - 0033h) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_B_D0	FIN_CLP_B_D0;	/**< (0034 - 0037h) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_B_D1	FIN_CLP_B_D1;	/**< (0038 - 003Bh) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_B_D2	FIN_CLP_B_D2;	/**< (003C - 003Fh) */
	U_IM_IIP_PARAM_FLT_ISO_MD		ISO_MD;			/**< (0040 - 0043h) */
	U_IM_IIP_PARAM_FLT_ISO_TH_D		ISO_TH_D[4];	/**< (0044 - 0053h) */
	U_IM_IIP_PARAM_FLT_ISO_GAIN_D	ISO_GAIN_D[4];	/**< (0054 - 0063h) */
	U_IM_IIP_PARAM_FLT_SPF_MD		SPF_MD;			/**< (0064 - 0067h) */

	UCHAR dmy_0068_007F[0x0080-0x0068]; /**< (0068 - 007Fh) */

	U_IM_IIP_PARAM_FLT_SPF_FLTW		SPF_FLTW_D0;	/**< (0080 - 00A3h) */

	UCHAR dmy_00A4_00BF[0x00C0-0x00A4]; /**< (00A4 - 00BFh) */

	U_IM_IIP_PARAM_FLT_SPF_FLTW		SPF_FLTW_D1;	/**< (00C0 - 00E3h) */

	UCHAR dmy_00E4_00FF[0x0100-0x00E4]; /**< (00E4 - 00FFh) */

	U_IM_IIP_PARAM_FLT_SPF_FLTW		SPF_FLTW_D2;	/**< (0100 - 0123h) */

	UCHAR dmy_0124_013F[0x0140-0x0124]; /**< (0124 - 013Fh) */

	U_IM_IIP_PARAM_FLT_SPF_FLTW		SPF_FLTW_D3;	/**< (0140 - 0163h) */

	UCHAR dmy_0164_017F[0x0180-0x0164]; /**< (0164 - 017Fh) */

	U_IM_IIP_PARAM_FLT_SPF_VTH_D0	SPF_VTH_D0;		/**< (0180 - 0183h) */
	U_IM_IIP_PARAM_FLT_SPF_VTH_D1	SPF_VTH_D1;		/**< (0184 - 0187h) */
	U_IM_IIP_PARAM_FLT_SPF_VTH_D2	SPF_VTH_D2;		/**< (0188 - 018Bh) */
	U_IM_IIP_PARAM_FLT_SPF_CORE_D0	SPF_CORE_D0;	/**< (018C - 018Fh) */
	U_IM_IIP_PARAM_FLT_SPF_CORE_D1	SPF_CORE_D1;	/**< (0190 - 0193h) */
	U_IM_IIP_PARAM_FLT_SPF_CORE_D2	SPF_CORE_D2;	/**< (0194 - 0197h) */
	U_IM_IIP_PARAM_FLT_SPF_CLP_D0	SPF_CLP_D0;		/**< (0198 - 019Bh) */
	U_IM_IIP_PARAM_FLT_SPF_CLP_D1	SPF_CLP_D1;		/**< (019C - 019Fh) */
	U_IM_IIP_PARAM_FLT_SPF_CLP_D2	SPF_CLP_D2;		/**< (01A0 - 01A3h) */
	U_IM_IIP_PARAM_FLT_SPF_EOADD_D0	SPF_EOADD_D0;	/**< (01A4 - 01A7h) */
	U_IM_IIP_PARAM_FLT_SPF_EOADD_D1	SPF_EOADD_D1;	/**< (01A8 - 01ABh) */
	U_IM_IIP_PARAM_FLT_SPF_EOADD_D2	SPF_EOADD_D2;	/**< (01AC - 01AFh) */
	U_IM_IIP_PARAM_FLT_EP_MD		EP_MD;			/**< (01B0 - 01B3h) */

	UCHAR dmy_01B4_01B7[0x01B8-0x01B4]; /**< (01B4 - 01B7h) */

	U_IM_IIP_PARAM_FLT_EP_GA		EP_GA_D[4];		/**< (01B8 - 01BFh) */
	U_IM_IIP_PARAM_FLT_EP_OFS		EP_OFS[4];		/**< (01D8 - 01F7h) */
	U_IM_IIP_PARAM_FLT_EP_BD		EP_BD[4];		/**< (01F8 - 0217h) */
	U_IM_IIP_PARAM_FLT_ED_FM		ED_FM;			/**< (0218 - 021Bh) */
	U_IM_IIP_PARAM_FLT_ED_OUT		ED_OUT;			/**< (021C - 021Fh) */
	U_IM_IIP_PARAM_FLT_ADAPT_K		ADAPT_K;		/**< (0220 - 0223h) */
	U_IM_IIP_PARAM_FLT_ADAPT_TH		ADAPT_TH;		/**< (0224 - 0227h) */
	U_IM_IIP_PARAM_FLT_ADAPT_DIV	ADAPT_DIV;		/**< (0228 - 022Bh) */
	U_IM_IIP_PARAM_FLT_UPLK_TARGET	UPLK_TARGET;	/**< (022C - 022Fh) */
	U_IM_IIP_PARAM_FLT_DWLK_TARGET	DWLK_TARGET;	/**< (0230 - 0233h) */
	U_IM_IIP_PARAM_FLT_LKNUM		LKNUM;			/**< (0234 - 0237h) */
	U_IM_IIP_PARAM_FLT_RING			RING;			/**< (0238 - 023Bh) */
	U_IM_IIP_PARAM_FLT_BF_MD		BF_MD;			/**< (023C - 023Fh) */

	UCHAR dmy_50240_5FFFF[0x60000-0x50240]; /**< 0240 - FFFFh */
} T_IM_IIP_PARAM_FLT_DUMP;


///////////////////////////////////////////////////////////////
// SL UNIT Register
///////////////////////////////////////////////////////////////

/** structure of SL_TOPCNF0 (0000h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	WAITCONF	:6;	/**< WAITCONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:2;
		ULONG				:8;
		ULONG	DATACONF	:6;	/**< DATACONF<br>Refer to @ref E_IM_IIP_PARAM_PORTID. */
		ULONG				:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_SL_TOPCNF0;

/** structure of PIXIDDEF	(0004h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OPIXID	:4;	/**< OPIXID */
		ULONG			:4;
		ULONG	CSEL	:2;	/**< CSEL<br>Refer to @ref E_IM_IIP_PARAM_CSEL */
		ULONG			:2;
		ULONG			:1;
		ULONG			:3;
		ULONG	DIV1ST	:1;	/**< DIV1ST<br>Refer to @ref E_IM_IIP_PARAM_DIV1ST. */
		ULONG			:3;
		ULONG	LKDIV	:2;	/**< LKDIV */
		ULONG			:10;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_PIXIDDEF;

/** structure of PLDUNIT_SL (0008h) */
typedef union {
	ULONG		word[2];	/**< All bits */
	struct {
		ULONG	PLDUNIT_SL_LO	:32;	/**< PLDUNIT_SL_LO<br>Lower 32bit of D_IM_IIP_PARAM_PLDUNIT_xxx */
		ULONG	PLDUNIT_SL_HI	:32;	/**< PLDUNIT_SL_HI<br>Upper 32bit of D_IM_IIP_PARAM_PLDUNIT_xxx */
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_PLDUNIT_SL;

/** structure of SL_CTL (0010h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SL_MODE :3;	/**< SL_MODE */
		ULONG			:1;
		ULONG	LOADMD	:1;	/**< LOADMD */
		ULONG			:3;
		ULONG	MASKEN	:2;	/**< MASKEN<br>Refer to @ref E_IM_IIP_PARAM_MASKEN. */
		ULONG			:22;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_SL_CTL;

/** structure of OUTCOL (0014h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OUTCOL	:4;	/**< OUTCOL */
		ULONG			:28;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_OUTCOL;

/** structure of SL_HSIZE	(0020h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	HSTART	:14;	/**< HSTART */
		ULONG			:2;
		ULONG	HEND	:14;	/**< HEND */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_SL_HSIZE;

/** structure of SL_VSIZE	(0024h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	VSTART	:14;	/**< VSTART */
		ULONG			:2;
		ULONG	VEND	:14;	/**< VEND */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_SL_VSIZE;

/** structure of SL_PHSZ	(0028h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PHSZ0	:14;	/**< PHSZ0 */
		ULONG			:2;
		ULONG	PHSZ1	:14;	/**< PHSZ1 */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_SL_PHSZ;

/** structure of SL_PVSZ	(002Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PVSZ0	:14;	/**< PVSZ0 */
		ULONG			:2;
		ULONG	PVSZ1	:14;	/**< PVSZ1 */
		ULONG			:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_SL_PVSZ;

/** structure of SEL_OPARA	(0040h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SEL_OPARA	:4;	/**< SEL_OPARA */
		ULONG				:28;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_SEL_OPARA;

/** structure of OFSET_0	(0048h)
	structure of OFSET_1	(0064h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	OFSET	:15;	/**< OFSET_0<br>OFSET_1 */
		ULONG			:17;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_OFSET;

/** structure of CLPTH_U_0	(004Ch)
	structure of CLPTH_U_1	(0068h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	CLPTH_U		:15;	/**< CLPTH_U_0<br>CLPTH_U_1 */
		ULONG				:17;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_CLPTH_U;

/** structure of CLPTH_L_0	(0050h)
	structure of CLPTH_L_1	(006Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	CLPTH_L		:15;	/**< CLPTH_L_0<br>CLPTH_L_1 */
		ULONG				:17;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_CLPTH_L;

/** structure of CLPVAL_U_0 (0054h)
	structure of CLPVAL_U_1 (0070h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	CLPVAL_U	:15;	/**< CLPVAL_U_0<br>CLPVAL_U_1 */
		ULONG				:17;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_CLPVAL_U;

/** structure of CLPVAL_L_0 (0058h)
	structure of CLPVAL_L_1 (0074h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		LONG	CLPVAL_L	:15;	/**< CLPVAL_L_0<br>CLPVAL_L_1 */
		ULONG				:17;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_CLPVAL_L;

/** structure of SFT_0	(005Ch)
	structure of SFT_1	(0078h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SFTM		:1;	/**< SFTM_0<br>SFTM_1<br>Refer to @ref E_IM_IIP_PARAM_SFTM. */
		ULONG				:3;
		ULONG	SFTVAL		:4;	/**< SFTVAL_0<br>SFTVAL_1 */
		ULONG				:24;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_SFT;

/** structure of W14MD_0	(0060h)
	structure of W14MD_1	(007Ch) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	W14LSFT		:2;	/**< W14LSFT0<br>W14LSFT1 */
		ULONG				:2;
		ULONG	W14SAT		:2;	/**< W14SAT0<br>W14SAT1 */
		ULONG				:26;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_W14MD;

/** structure of FILL_EN	(0200h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FILL_EN0	:1;	/**< FILL_EN0<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN1	:1;	/**< FILL_EN1<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN2	:1;	/**< FILL_EN2<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN3	:1;	/**< FILL_EN3<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN4	:1;	/**< FILL_EN4<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN5	:1;	/**< FILL_EN5<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN6	:1;	/**< FILL_EN6<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN7	:1;	/**< FILL_EN7<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN8	:1;	/**< FILL_EN8<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN9	:1;	/**< FILL_EN9<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN10	:1;	/**< FILL_EN10<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN11	:1;	/**< FILL_EN11<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN12	:1;	/**< FILL_EN12<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN13	:1;	/**< FILL_EN13<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN14	:1;	/**< FILL_EN14<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN15	:1;	/**< FILL_EN15<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN16	:1;	/**< FILL_EN16<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN17	:1;	/**< FILL_EN17<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN18	:1;	/**< FILL_EN18<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG	FILL_EN19	:1;	/**< FILL_EN19<br>Refer to @ref E_IM_IIP_PARAM_FILL_EN. */
		ULONG				:12;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_FILL_EN;

/** structure of FILL_DATA	(0204h) */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FILL_D0	:8;	/**< FILL_D0 */
		ULONG	FILL_D1	:8;	/**< FILL_D1 */
		ULONG	FILL_D2	:8;	/**< FILL_D2 */
		ULONG			:8;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_FILL_DATA;

/** structure of FILL_SIZE	(0300h) */
typedef union {
	ULONG	 word[2];	/**< All bits */
	struct {
		ULONG	FILL_HSTA	:14;	/**< FILL_HSTA_0~19 */
		ULONG				:2;
		ULONG	FILL_HEND	:14;	/**< FILL_HEND_0~19 */
		ULONG				:2;
		ULONG	FILL_VSTA	:14;	/**< FILL_VSTA_0~19 */
		ULONG				:2;
		ULONG	FILL_VEND	:14;	/**< FILL_VEND_0~19 */
		ULONG				:2;
	}bit;		/**< bit field */
} U_IM_IIP_PARAM_STS_FILL_SIZE;

/** Define i/o mapping */
typedef struct {
	U_IM_IIP_PARAM_STS_SL_TOPCNF0	SL_TOPCNF0;		/**< (0000 - 0003h) */
	U_IM_IIP_PARAM_STS_PIXIDDEF		PIXIDDEF;		/**< (0004 - 0007h) */
	U_IM_IIP_PARAM_STS_PLDUNIT_SL	PLDUNIT_SL;		/**< (0008 - 000Fh) */
	U_IM_IIP_PARAM_STS_SL_CTL		SL_CTL;			/**< (0010 - 0013h) */
	U_IM_IIP_PARAM_STS_OUTCOL		OUTCOL;			/**< (0014 - 0017h) */

	UCHAR dmy_0018_001F[0x0020-0x0018]; /**< (0018 - 001Fh) */

	U_IM_IIP_PARAM_STS_SL_HSIZE		SL_HSIZE;		/**< (0020 - 0023h) */
	U_IM_IIP_PARAM_STS_SL_VSIZE		SL_VSIZE;		/**< (0024 - 0027h) */
	U_IM_IIP_PARAM_STS_SL_PHSZ		SL_PHSZ;		/**< (0028 - 002Bh) */
	U_IM_IIP_PARAM_STS_SL_PVSZ		SL_PVSZ;		/**< (002C - 002Fh) */

	UCHAR dmy_0030_003F[0x0040-0x0030]; /**< (0030 - 003Fh) */

	U_IM_IIP_PARAM_STS_SEL_OPARA	SEL_OPARA;		/**< (0040 - 0043h) */

	UCHAR dmy_0044_0047[0x0048-0x0044]; /**< (0044 - 0047h) */

	U_IM_IIP_PARAM_STS_OFSET		OFSET_0;		/**< (0048 - 004Bh) */
	U_IM_IIP_PARAM_STS_CLPTH_U		CLPTH_U_0;		/**< (004C - 004Fh) */
	U_IM_IIP_PARAM_STS_CLPTH_L		CLPTH_L_0;		/**< (0050 - 0053h) */
	U_IM_IIP_PARAM_STS_CLPVAL_U		CLPVAL_U_0; 	/**< (0054 - 0057h) */
	U_IM_IIP_PARAM_STS_CLPVAL_L		CLPVAL_L_0; 	/**< (0058 - 005Bh) */
	U_IM_IIP_PARAM_STS_SFT			SFT_0;			/**< (005C - 005Fh) */
	U_IM_IIP_PARAM_STS_W14MD		W14MD_0;		/**< (0060 - 0063h) */
	U_IM_IIP_PARAM_STS_OFSET		OFSET_1;		/**< (0064 - 0067h) */
	U_IM_IIP_PARAM_STS_CLPTH_U		CLPTH_U_1;		/**< (0068 - 006Bh) */
	U_IM_IIP_PARAM_STS_CLPTH_L		CLPTH_L_1;		/**< (006C - 006Fh) */
	U_IM_IIP_PARAM_STS_CLPVAL_U		CLPVAL_U_1;		/**< (0070 - 0073h) */
	U_IM_IIP_PARAM_STS_CLPVAL_L		CLPVAL_L_1;		/**< (0074 - 0077h) */
	U_IM_IIP_PARAM_STS_SFT			SFT_1;			/**< (0078 - 007Bh) */
	U_IM_IIP_PARAM_STS_W14MD		W14MD_1;		/**< (007C - 007Fh) */

	UCHAR dmy_0080_01FF[0x0200-0x0080]; /**< (0080 - 01FFh) */

	U_IM_IIP_PARAM_STS_FILL_EN		FILL_EN;		/**< (0200 - 0203h) */
	U_IM_IIP_PARAM_STS_FILL_DATA	FILL_DATA;		/**< (0204 - 0207h) */

	UCHAR dmy_0208_02FF[0x0300-0x0208]; /**< (0208 - 02FFh) */

	U_IM_IIP_PARAM_STS_FILL_SIZE	FILL_SIZE[20];	/**< (0300 - 039Fh) */

	UCHAR dmy_B03A0_BFFFF[0xC0000-0xB03A0]; /**< 03A0 - FFFFh */
} T_IM_IIP_PARAM_STS_DUMP;


///////////////////////////////////////////////////////////////
// UNIT Parameter
///////////////////////////////////////////////////////////////

/** Packet header of Unit Parameters */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	BeginningAddress	:16;	/**< Beginning address of the register */
		UINT32	WordLength			:8;		/**< Word Length (including Packed header) */
		UINT32	CtrlCode			:8;		/**< Control code<br>Refer to @ref E_IM_IIP_PARAM_CTL_CODE. */
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_PACKET_HEADER;

/** 1DL Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=16, BeginningAddress=0x0004) */
	U_IM_IIP_PARAM_1DL_LD_TOPCNF0		LD_TOPCNF0;		/**< LD_TOPCNF0		(+0004h) */
	U_IM_IIP_PARAM_1DL_LD_TOPCNF1		LD_TOPCNF1;		/**< LD_TOPCNF1		(+0008h) */

	UCHAR				dmy_000C_000F[0x0010-0x000C];	/**< Unused variable */

	U_IM_IIP_PARAM_1DL_PIXIDDEF			PIXIDDEF;		/**< PIXIDDEF		(+0010h) */
	U_IM_IIP_PARAM_1DL_LD_CTL			LD_CTL;			/**< LD_CTL			(+0014h) */
	U_IM_IIP_PARAM_1DL_LD_PREFETCH		LD_PREFETCH;	/**< LD_PREFETCH	(+0018h) */

	UCHAR				dmy_001C_001F[0x0020-0x001C];	/**< Unused variable */

	U_IM_IIP_PARAM_1DL_LD_HSIZE			LD_HSIZE;		/**< LD_HSIZE		(+0020h) */
	U_IM_IIP_PARAM_1DL_LD_VSIZE			LD_VSIZE;		/**< LD_VSIZE		(+0024h) */
	U_IM_IIP_PARAM_1DL_LD_PHSZ			LD_PHSZ;		/**< LD_PHSZ		(+0028h) */
	U_IM_IIP_PARAM_1DL_LD_PVSZ			LD_PVSZ;		/**< LD_PVSZ		(+002Ch) */
	U_IM_IIP_PARAM_1DL_UPLK_TARGET		UPLK_TARGET;	/**< UPLK_TARGET	(+0030h) */
	U_IM_IIP_PARAM_1DL_DWLK_TARGET		DWLK_TARGET;	/**< DWLK_TARGET	(+0034h) */
	U_IM_IIP_PARAM_1DL_LKNUM			LKNUM;			/**< LKNUM			(+0038h) */
	U_IM_IIP_PARAM_1DL_RING				RING;			/**< RING			(+003Ch) */
} T_IM_IIP_PARAM_1DL;

/** FLT Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=145, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_FLT_TDLTOPCNF		TDLTOPCNF;		/**< TDLTOPCNF		(+0000h) */
	U_IM_IIP_PARAM_FLT_PIXIDDEF			PIXIDDEF;		/**< PIXIDDEF		(+0004h) */
	U_IM_IIP_PARAM_FLT_TDL_CTL			TDL_CTL;		/**< TDL_CTL		(+0008h) */
	U_IM_IIP_PARAM_FLT_CALC_MD			CALC_MD;		/**< CALC_MD		(+000Ch) */
	U_IM_IIP_PARAM_FLT_TDL_HSIZE		TDL_HSIZE;		/**< TDL_HSIZE		(+0010h) */
	U_IM_IIP_PARAM_FLT_TDL_VSIZE		TDL_VSIZE;		/**< TDL_VSIZE		(+0014h) */
	U_IM_IIP_PARAM_FLT_TDL_PHSZ			TDL_PHSZ;		/**< TDL_PHSZ		(+0018h) */
	U_IM_IIP_PARAM_FLT_TDL_PVSZ			TDL_PVSZ;		/**< TDL_PVSZ		(+001Ch) */
	U_IM_IIP_PARAM_FLT_TDL_PF			TDL_PF;			/**< TDL_PF			(+0020h) */
	U_IM_IIP_PARAM_FLT_UNIT_OUT			UNIT_OUT;		/**< UNIT_OUT		(+0024h) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_A_D0		FIN_CLP_A_D0;	/**< FIN_CLP_A_D0	(+0028h) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_A_D1		FIN_CLP_A_D1;	/**< FIN_CLP_A_D1	(+002Ch) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_A_D2		FIN_CLP_A_D2;	/**< FIN_CLP_A_D2	(+0030h) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_B_D0		FIN_CLP_B_D0;	/**< FIN_CLP_B_D0	(+0034h) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_B_D1		FIN_CLP_B_D1;	/**< FIN_CLP_B_D1	(+0038h) */
	U_IM_IIP_PARAM_FLT_FIN_CLP_B_D2		FIN_CLP_B_D2;	/**< FIN_CLP_B_D2	(+003Ch) */
	U_IM_IIP_PARAM_FLT_ISO_MD			ISO_MD;			/**< ISO_MD			(+0040h) */
	U_IM_IIP_PARAM_FLT_ISO_TH_D			ISO_TH_D[4];	/**< ISO_TH_D0~3	(+0044h) */
	U_IM_IIP_PARAM_FLT_ISO_GAIN_D		ISO_GAIN_D[4];	/**< ISO_GAIN_D0~3	(+0054h) */
	U_IM_IIP_PARAM_FLT_SPF_MD			SPF_MD;			/**< SPF_MD			(+0064h) */

	UCHAR dmy_0068_007F[0x0080-0x0068]; /**< Unused variable */

	U_IM_IIP_PARAM_FLT_SPF_FLTW			SPF_FLTW_D0;	/**< SPF_FLTW_D0	(+0080h) */

	UCHAR dmy_00A4_00BF[0x00C0-0x00A4]; /**< Unused variable */

	U_IM_IIP_PARAM_FLT_SPF_FLTW			SPF_FLTW_D1;	/**< SPF_FLTW_D1	(+00C0h) */

	UCHAR dmy_00E4_00FF[0x0100-0x00E4]; /**< Unused variable */

	U_IM_IIP_PARAM_FLT_SPF_FLTW			SPF_FLTW_D2;	/**< SPF_FLTW_D2	(+0100h) */

	UCHAR dmy_0124_013F[0x0140-0x0124]; /**< Unused variable */

	U_IM_IIP_PARAM_FLT_SPF_FLTW			SPF_FLTW_D3;	/**< SPF_FLTW_D3	(+0140h) */

	UCHAR dmy_0164_017F[0x0180-0x0164]; /**< Unused variable */

	U_IM_IIP_PARAM_FLT_SPF_VTH_D0		SPF_VTH_D0;		/**< SPF_VTH_D0		(+0180h) */
	U_IM_IIP_PARAM_FLT_SPF_VTH_D1		SPF_VTH_D1;		/**< SPF_VTH_D1		(+0184h) */
	U_IM_IIP_PARAM_FLT_SPF_VTH_D2		SPF_VTH_D2;		/**< SPF_VTH_D2		(+0188h) */
	U_IM_IIP_PARAM_FLT_SPF_CORE_D0		SPF_CORE_D0;	/**< SPF_CORE_D0	(+018Ch) */
	U_IM_IIP_PARAM_FLT_SPF_CORE_D1		SPF_CORE_D1;	/**< SPF_CORE_D1	(+0190h) */
	U_IM_IIP_PARAM_FLT_SPF_CORE_D2		SPF_CORE_D2;	/**< SPF_CORE_D2	(+0194h) */
	U_IM_IIP_PARAM_FLT_SPF_CLP_D0		SPF_CLP_D0;		/**< SPF_CLP_D0		(+0198h) */
	U_IM_IIP_PARAM_FLT_SPF_CLP_D1		SPF_CLP_D1;		/**< SPF_CLP_D1		(+019Ch) */
	U_IM_IIP_PARAM_FLT_SPF_CLP_D2		SPF_CLP_D2;		/**< SPF_CLP_D2		(+01A0h) */
	U_IM_IIP_PARAM_FLT_SPF_EOADD_D0		SPF_EOADD_D0;	/**< SPF_EOADD_D0	(+01A4h) */
	U_IM_IIP_PARAM_FLT_SPF_EOADD_D1		SPF_EOADD_D1;	/**< SPF_EOADD_D1	(+01A8h) */
	U_IM_IIP_PARAM_FLT_SPF_EOADD_D2		SPF_EOADD_D2;	/**< SPF_EOADD_D2	(+01ACh) */
	U_IM_IIP_PARAM_FLT_EP_MD			EP_MD;			/**< EP_MD			(+01B0h) */

	UCHAR dmy_01B4_01B7[0x01B8-0x01B4];			/**< Unused variable */

	U_IM_IIP_PARAM_FLT_EP_GA			EP_GA_D[4];		/**< EP_GA_D0~3		(+01B8h,+01C0h,+01C8h,+01D0h) */
	U_IM_IIP_PARAM_FLT_EP_OFS			EP_OFS_D[4];	/**< EP_OFS_D0~3	(+01D8h,+01E0h,+01E8h,+01F0h) */
	U_IM_IIP_PARAM_FLT_EP_BD			EP_BD_D[4];		/**< EP_BD_D0~3		(+01F8h,+0200h,+0208h,+0210h) */
	U_IM_IIP_PARAM_FLT_ED_FM			ED_FM;			/**< ED_FM			(+0218h) */
	U_IM_IIP_PARAM_FLT_ED_OUT			ED_OUT;			/**< ED_OUT			(+021Ch) */
	U_IM_IIP_PARAM_FLT_ADAPT_K			ADAPT_K;		/**< ADAPT_K		(+0220h) */
	U_IM_IIP_PARAM_FLT_ADAPT_TH			ADAPT_TH;		/**< ADAPT_TH		(+0224h) */
	U_IM_IIP_PARAM_FLT_ADAPT_DIV		ADAPT_DIV;		/**< ADAPT_DIV		(+0228h) */
	U_IM_IIP_PARAM_FLT_UPLK_TARGET		UPLK_TARGET;	/**< UPLK_TARGET	(+022Ch) */
	U_IM_IIP_PARAM_FLT_DWLK_TARGET		DWLK_TARGET;	/**< DWLK_TARGET	(+0230h) */
	U_IM_IIP_PARAM_FLT_LKNUM			LKNUM;			/**< LKNUM			(+0234h) */
	U_IM_IIP_PARAM_FLT_RING				RING;			/**< RING			(+0238h) */
	U_IM_IIP_PARAM_FLT_BF_MD			BF_MD;			/**< BF_MD			(+023Ch) */
} T_IM_IIP_PARAM_FLT;

/** AFN Unit Parameter */
typedef struct {
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=29, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_AFN_AFNTOPCNF		AFNTOPCNF;		/**< AFNTOPCNF		(+0000h) */
	U_IM_IIP_PARAM_AFN_AFNLDCNF			AFNLDCNF;		/**< AFNLDCNF		(+0004h) */
	U_IM_IIP_PARAM_AFN_AFNCTL			AFNCTL;			/**< AFNCTL			(+0008h) */
	U_IM_IIP_PARAM_AFN_PFCTL			PFCTL;			/**< PFCTL			(+000Ch) */
	U_IM_IIP_PARAM_AFN_PIXIDEF			PIXIDEF;		/**< PIXIDEF		(+0010h) */
	U_IM_IIP_PARAM_AFN_CALMETHOD		CALMETHOD;		/**< CALMETHOD		(+0014h) */
	U_IM_IIP_PARAM_AFN_FILVAL0			FILVAL0;		/**< FILVAL0		(+0018h) */
	U_IM_IIP_PARAM_AFN_FILVAL1			FILVAL1;		/**< FILVAL1		(+001Ch) */
	U_IM_IIP_PARAM_AFN_OPMD				OPMD;			/**< OPMD			(+0020h) */

	UCHAR dmy_0024_002F[0x0030-0x0024]; /**< Unused variable */

	U_IM_IIP_PARAM_AFN_OPY				OPY;			/**< OPY			(+0030h) */
	U_IM_IIP_PARAM_AFN_OPB				OPB;			/**< OPB			(+0034h) */
	U_IM_IIP_PARAM_AFN_OPR				OPR;			/**< OPR			(+0038h) */
	U_IM_IIP_PARAM_AFN_OPA				OPA;			/**< OPA			(+003Ch) */
	U_IM_IIP_PARAM_AFN_CLIPLVLY			CLIPLVLY;		/**< CLIPLVLY		(+0040h) */
	U_IM_IIP_PARAM_AFN_CLIPLVLB			CLIPLVLB;		/**< CLIPLVLB		(+0044h) */
	U_IM_IIP_PARAM_AFN_CLIPLVLR			CLIPLVLR;		/**< CLIPLVLR		(+0048h) */
	U_IM_IIP_PARAM_AFN_CLIPLVLA			CLIPLVLA;		/**< CLIPLVLA		(+004Ch) */
	U_IM_IIP_PARAM_AFN_AFNPCNT			AFNPCNT;		/**< AFNPCNT		(+0050h) */
	U_IM_IIP_PARAM_AFN_AFNPA			AFNPA;			/**< AFNPA			(+0054h) */
	U_IM_IIP_PARAM_AFN_OUTSIZE			OUTSIZE;		/**< OUTSIZE		(+0058h) */
	U_IM_IIP_PARAM_AFN_OUTSTARTPOS		OUTSTARTPOS;	/**< OUTSTARTPOS	(+005Ch) */
	U_IM_IIP_PARAM_AFN_UPLK_TARGET		UPLK_TARGET;	/**< UPLK_TARGET	(+0060h) */
	U_IM_IIP_PARAM_AFN_DWLK_TARGET		DWLK_TARGET;	/**< DWLK_TARGET	(+0064h) */
	U_IM_IIP_PARAM_AFN_LKNUM			LKNUM;			/**< LKNUM			(+0068h) */
	U_IM_IIP_PARAM_AFN_RING				RING;			/**< RING			(+006Ch) */
} T_IM_IIP_PARAM_AFN;

/** FRECT Unit Parameter */
typedef struct {
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=29, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_FRECT_FRTOPCNF		FRTOPCNF;		/**< FRTOPCNF		(+0000h) */
	U_IM_IIP_PARAM_FRECT_FRLDCNF		FRLDCNF;		/**< FRLDCNF		(+0004h) */
	U_IM_IIP_PARAM_FRECT_FRCTL			FRCTL;			/**< FRCTL			(+0008h) */
	U_IM_IIP_PARAM_FRECT_PFCTL			PFCTL;			/**< PFCTL			(+000Ch) */
	U_IM_IIP_PARAM_FRECT_PIXIDEF		PIXIDEF;		/**< PIXIDEF		(+0010h) */
	U_IM_IIP_PARAM_FRECT_CALMETHOD		CALMETHOD;		/**< CALMETHOD		(+0014h) */
	U_IM_IIP_PARAM_FRECT_FILVAL0		FILVAL0;		/**< FILVAL0		(+0018h) */
	U_IM_IIP_PARAM_FRECT_FILVAL1		FILVAL1;		/**< FILVAL1		(+001Ch) */
	U_IM_IIP_PARAM_FRECT_OPMD			OPMD;			/**< OPMD			(+0020h) */

	UCHAR	dmy_0024_002F[0x0030-0x0024]; /**< Unused variable */

	U_IM_IIP_PARAM_FRECT_OPY			OPY;			/**< OPY			(+0030h) */
	U_IM_IIP_PARAM_FRECT_OPB			OPB;			/**< OPB			(+0034h) */
	U_IM_IIP_PARAM_FRECT_OPR			OPR;			/**< OPR			(+0038h) */
	U_IM_IIP_PARAM_FRECT_OPA			OPA;			/**< OPA			(+003Ch) */
	U_IM_IIP_PARAM_FRECT_CLIPLVLY		CLIPLVLY;		/**< CLIPLVLY		(+0040h) */
	U_IM_IIP_PARAM_FRECT_CLIPLVLB		CLIPLVLB;		/**< CLIPLVLB		(+0044h) */
	U_IM_IIP_PARAM_FRECT_CLIPLVLR		CLIPLVLR;		/**< CLIPLVLR		(+0048h) */
	U_IM_IIP_PARAM_FRECT_CLIPLVLA		CLIPLVLA;		/**< CLIPLVLA		(+004Ch) */
	U_IM_IIP_PARAM_FRECT_FRPCNT			FRPCNT;			/**< FRPCNT			(+0050h) */
	U_IM_IIP_PARAM_FRECT_PADRS			PADRS;			/**< PADRS			(+0054h) */
	U_IM_IIP_PARAM_FRECT_OUTSIZE		OUTSIZE;		/**< OUTSIZE		(+0058h) */
	U_IM_IIP_PARAM_FRECT_OUTSTARTPOS	OUTSTARTPOS;	/**< OUTSTARTPOS	(+005Ch) */
	U_IM_IIP_PARAM_FRECT_UPLK_TARGET	UPLK_TARGET;	/**< UPLK_TARGET	(+0060h) */
	U_IM_IIP_PARAM_FRECT_DWLK_TARGET	DWLK_TARGET;	/**< DWLK_TARGET	(+0064h) */
	U_IM_IIP_PARAM_FRECT_LKNUM			LKNUM;			/**< LKNUM			(+0068h) */
	U_IM_IIP_PARAM_FRECT_RING			RING;			/**< RING			(+006Ch) */
} T_IM_IIP_PARAM_FRECT;

/** STS Unit Parameter (Basic) */
typedef struct{
	U_IM_IIP_PARAM_STS_SL_TOPCNF0		SL_TOPCNF0;	/**< SL_TOPCNF0		(+0000h) */
	U_IM_IIP_PARAM_STS_PIXIDDEF			PIXIDDEF;	/**< PIXIDDEF		(+0004h) */
	U_IM_IIP_PARAM_STS_PLDUNIT_SL		PLDUNIT_SL;	/**< PLDUNIT_SL		(+0008h) */
	U_IM_IIP_PARAM_STS_SL_CTL			SL_CTL;		/**< SL_CTL			(+000Ch) */
	U_IM_IIP_PARAM_STS_OUTCOL			OUTCOL;		/**< OUTCOL			(+0010h) */

	UCHAR				dmy_0018_001F[0x0020-0x0018];	/**< Unused variable */

	U_IM_IIP_PARAM_STS_SL_HSIZE			SL_HSIZE;	/**< SL_HSIZE		(+0020h) */
	U_IM_IIP_PARAM_STS_SL_VSIZE			SL_VSIZE;	/**< SL_VSIZE		(+0024h) */
	U_IM_IIP_PARAM_STS_SL_PHSZ			SL_PHSZ;	/**< SL_PHSZ		(+0028h) */
	U_IM_IIP_PARAM_STS_SL_PVSZ			SL_PVSZ;	/**< SL_PVSZ		(+002Ch) */

	UCHAR				dmy_0030_003F[0x0040-0x0030];	/**< Unused variable */

	U_IM_IIP_PARAM_STS_SEL_OPARA		SEL_OPARA;	/**< SEL_OPARA		(+0040h) */

	UCHAR				dmy_0044_0047[0x0048-0x0044];	/**< Unused variable */

	U_IM_IIP_PARAM_STS_OFSET			OFSET_0;	/**< OFSET_0		(+0048h) */
	U_IM_IIP_PARAM_STS_CLPTH_U			CLPTH_U_0;	/**< CLPTH_U_0		(+004Ch) */
	U_IM_IIP_PARAM_STS_CLPTH_L			CLPTH_L_0;	/**< CLPTH_L_0		(+0050h) */
	U_IM_IIP_PARAM_STS_CLPVAL_U			CLPVAL_U_0;	/**< CLPVAL_U_0		(+0054h) */
	U_IM_IIP_PARAM_STS_CLPVAL_L			CLPVAL_L_0;	/**< CLPVAL_L_0		(+0058h) */
	U_IM_IIP_PARAM_STS_SFT				SFT_0;		/**< SFT_0			(+005Ch) */
	U_IM_IIP_PARAM_STS_W14MD			W14MD_0;	/**< W14MD_0		(+0060h) */
} T_IM_IIP_PARAM_STS_BASE;

/** STS Unit Parameter (Out parmeter 1) */
typedef struct{
	U_IM_IIP_PARAM_STS_OFSET			OFSET_1;	/**< OFSET_1		(+0064h) */
	U_IM_IIP_PARAM_STS_CLPTH_U			CLPTH_U_1;	/**< CLPTH_U_1		(+0068h) */
	U_IM_IIP_PARAM_STS_CLPTH_L			CLPTH_L_1;	/**< CLPTH_L_1		(+006Ch) */
	U_IM_IIP_PARAM_STS_CLPVAL_U			CLPVAL_U_1;	/**< CLPVAL_U_1		(+0070h) */
	U_IM_IIP_PARAM_STS_CLPVAL_L			CLPVAL_L_1;	/**< CLPVAL_L_1		(+0074h) */
	U_IM_IIP_PARAM_STS_SFT				SFT_1;		/**< SFT_1			(+0078h) */
	U_IM_IIP_PARAM_STS_W14MD			W14MD_1;	/**< W14MD_1		(+007Ch) */
} T_IM_IIP_PARAM_STS_OPARA1;

/** STS Unit Parameter (Set all registers) */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY, WordLength=33, BeginningAddress=0x0000) */
	T_IM_IIP_PARAM_STS_BASE				BASE;		/**< basic parameter */
	T_IM_IIP_PARAM_STS_OPARA1			OPARA1;		/**< Output parameter 1 */

	U_IM_IIP_PARAM_PACKET_HEADER		header1;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY, WordLength=3, BeginningAddress=0x0200) */
	U_IM_IIP_PARAM_STS_FILL_EN			FILL_EN;	/**< FILL_EN			(+0200h) */
	U_IM_IIP_PARAM_STS_FILL_DATA		FILL_DATA;	/**< FILL_DATA			(+0204h) */

	U_IM_IIP_PARAM_PACKET_HEADER		header2;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=41, BeginningAddress=0x0300) */
	U_IM_IIP_PARAM_STS_FILL_SIZE		FILL_SIZE[20]; /**< FILL_SIZE[20]	(+0300h) */
} T_IM_IIP_PARAM_STS_ALL;

/** STS Unit Parameter (Use out parameter 0, Fill enable) */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY, WordLength=26, BeginningAddress=0x0000) */
	T_IM_IIP_PARAM_STS_BASE				BASE;		/**< basic parameter */

	U_IM_IIP_PARAM_PACKET_HEADER		header1;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY, WordLength=3, BeginningAddress=0x0200) */
	U_IM_IIP_PARAM_STS_FILL_EN			FILL_EN;	/**< FILL_EN			(+0200h) */
	U_IM_IIP_PARAM_STS_FILL_DATA		FILL_DATA;	/**< FILL_DATA			(+0204h) */

	U_IM_IIP_PARAM_PACKET_HEADER		header2;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=41, BeginningAddress=0x0300) */
	U_IM_IIP_PARAM_STS_FILL_SIZE		FILL_SIZE[20]; /**< FILL_SIZE[20]	(+0300h) */
} T_IM_IIP_PARAM_STS_FILL;

/** STS Unit Parameter (Use out parameter 0, Fill disable) */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY, WordLength=26, BeginningAddress=0x0000) */
	T_IM_IIP_PARAM_STS_BASE				BASE;		/**< basic parameter */

	U_IM_IIP_PARAM_PACKET_HEADER		header1;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=2, BeginningAddress=0x0200) */
	U_IM_IIP_PARAM_STS_FILL_EN			FILL_EN;	/**< FILL_EN			(+0200h) */
} T_IM_IIP_PARAM_STS;

/** CSC Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;	/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=25, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_CSC_CSCTOPCNF		CSCTOPCNF;	/**< CSCTOPCNF		(+0000h) */

	UCHAR dmy_0004_001F[0x0020-0x0004]; /**< Unused variable */

	U_IM_IIP_PARAM_CSC_CSCK				CSCK[4];	/**< CSCK			(+0020h) */
	U_IM_IIP_PARAM_CSC_MDSEL			MDSEL;		/**< MDSEL			(+0040h) */
	U_IM_IIP_PARAM_CSC_ALPSEL			ALPSEL;		/**< ALPSEL			(+0044h) */
	U_IM_IIP_PARAM_CSC_ALPVAL			ALPVAL;		/**< ALPVAL			(+0048h) */
	U_IM_IIP_PARAM_CSC_MAXMIN			MAXMIN;		/**< MAXMIN			(+004Ch) */
	U_IM_IIP_PARAM_CSC_OUTCLIP			OUTCLIP;	/**< OUTCLIP		(+0050h) */
} T_IM_IIP_PARAM_CSC;

/** LUT Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=57, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_LUT_LUTTOPCNF		LUTTOPCNF[4];	/**< LUTTOPCNF		(+0000h) */
	U_IM_IIP_PARAM_LUT_LUTUNITCTL		LUTUNITCTL;		/**< LUTUNITCTL		(+0010h) */

	UCHAR dmy_0014_001F[0x0020-0x0014]; /**< Unused variable */

	U_IM_IIP_PARAM_LUT_LUTCTL			LUTCTL_A;		/**< LUTCTL_A		(+0020h) */

	UCHAR dmy_0024_002F[0x0030-0x0024]; /**< Unused variable */

	U_IM_IIP_PARAM_LUT_LUTPRECAL		LUTPRECAL_A;	/**< LUTPRECAL_A	(+0030h) */
	U_IM_IIP_PARAM_LUT_LUTCTL			LUTCTL_B;		/**< LUTCTL_B		(+0040h) */

	UCHAR dmy_0044_004F[0x0050-0x0044]; /**< Unused variable */

	U_IM_IIP_PARAM_LUT_LUTPRECAL		LUTPRECAL_B;	/**< LUTPRECAL_B	(+0050h) */
	U_IM_IIP_PARAM_LUT_LUTCTL			LUTCTL_C;		/**< LUTCTL_C		(+0060h) */

	UCHAR dmy_0064_006F[0x0070-0x0064]; /**< Unused variable */

	U_IM_IIP_PARAM_LUT_LUTPRECAL		LUTPRECAL_C;	/**< LUTPRECAL_C	(+0070h) */
	U_IM_IIP_PARAM_LUT_LUTCTL			LUTCTL_D;		/**< LUTCTL_D		(+0080h) */

	UCHAR dmy_0084_008F[0x0090-0x0084]; /**< Unused variable */

	U_IM_IIP_PARAM_LUT_LUTPRECAL		LUTPRECAL_D;	/**< LUTPRECAL_D	(+0090h) */
	U_IM_IIP_PARAM_LUT_LUTCTL			LUTCTL_E;		/**< LUTCTL_E		(+00A0h) */

	UCHAR dmy_00A4_00AF[0x00B0-0x00A4]; /**< Unused variable */

	U_IM_IIP_PARAM_LUT_LUTPRECAL		LUTPRECAL_E;	/**< LUTPRECAL_E	(+00B0h) */
	U_IM_IIP_PARAM_LUT_LUTCTL			LUTCTL_F;		/**< LUTCTL_F		(+00C0h) */

	UCHAR dmy_00C4_00CF[0x00D0-0x00C4]; /**< Unused variable */

	U_IM_IIP_PARAM_LUT_LUTPRECAL		LUTPRECAL_F;	/**< LUTPRECAL_F	(+00D0h) */
} T_IM_IIP_PARAM_LUT;

/** GPC Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=7, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_GPC_GPCTOPCNF		GPCTOPCNF[2];	/**< GPCTOPCNF		(+0000h) */

	UCHAR				dmy_0008_000F[0x0010-0x0008];	/**< Unused variable */

	U_IM_IIP_PARAM_GPC_GPCCONTROL		GPCCONTROL;		/**< GPCCONTROL		(+0010h) */
	U_IM_IIP_PARAM_GPC_GPCCLOADADR		GPCCLOADADR;	/**< GPCCLOADADR	(+0014h) */
} T_IM_IIP_PARAM_GPC;

/** Blend Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY, WordLength=4, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_BLEND_BLENDTOPCNF0	BLENDTOPCNF0;	/**< BLENDTOPCNF0	(+0000h) */
	U_IM_IIP_PARAM_BLEND_BLENDTOPCNF1	BLENDTOPCNF1;	/**< BLENDTOPCNF1	(+0004h) */
	U_IM_IIP_PARAM_BLEND_BLENDTOPCNF2	BLENDTOPCNF2;	/**< BLENDTOPCNF2	(+0008h) */

	U_IM_IIP_PARAM_PACKET_HEADER		header1;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=24, BeginningAddress=0x0080) */
	U_IM_IIP_PARAM_BLEND_BLENDCTL		BLENDCTL;		/**< BLENDCTL		(+0080h) */

	UCHAR dmy_0084_0087[0x0088-0x0084]; /**< Unused variable */

	U_IM_IIP_PARAM_BLEND_ALPHACTL0		ALPHACTL0;		/**< ALPHACTL0		(+0088h) */
	U_IM_IIP_PARAM_BLEND_ALPHACTL1		ALPHACTL1;		/**< ALPHACTL1		(+008Ch) */
	U_IM_IIP_PARAM_BLEND_ALPHACTL2		ALPHACTL2;		/**< ALPHACTL2		(+0090h) */

	UCHAR dmy_0094_0097[0x0098-0x0094]; /**< Unused variable */

	U_IM_IIP_PARAM_BLEND_WADDCTL		WADDCTL[4];		/**< WADDCTL0~3		(+0098h) */

	UCHAR dmy_00A8_00AB[0x00AC-0x00A8]; /**< Unused variable */

	U_IM_IIP_PARAM_BLEND_DIVCTL0		DIVCTL0;		/**< DIVCTL0		(+00ACh) */

	UCHAR dmy_00B0_00B3[0x00B4-0x00B0]; /**< Unused variable */

	U_IM_IIP_PARAM_BLEND_MSKCTL0		MSKCTL0;		/**< MSKCTL0		(+00B4h) */

	UCHAR dmy_00B8_00BB[0x00BC-0x00B8]; /**< Unused variable */

	U_IM_IIP_PARAM_BLEND_MULCTL			MULCTL[4];		/**< MULCTL[4]		(+00BCh) */
} T_IM_IIP_PARAM_BLEND;

/** MON Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=25, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_MON_MONCTL			MONCTL;			/**< MONCTL			(+0000h) */
	U_IM_IIP_PARAM_MON_MONDSEL			MONDSEL;		/**< MONDSEL		(+0004h) */

	UCHAR dmy_0008_000F[0x0010-0x0008]; /**< Unused variable */

	U_IM_IIP_PARAM_MON_HISTCTL			HISTCTL[4];		/**< HISTCTL		(+0010h) */
	U_IM_IIP_PARAM_MON_ABSMAX			ABSMAX;			/**< ABSMAX			(+0020h) */
	U_IM_IIP_PARAM_MON_ABSMIN			ABSMIN;			/**< ABSMIN			(+0024h) */

	UCHAR dmy_0028_003F[0x0040-0x0028]; /**< Unused variable */

	U_IM_IIP_PARAM_MON_ABSADD			ABSADD;			/**< ABSADD			(+0040h) */

	UCHAR dmy_0044_004F[0x0050-0x0044]; /**< Unused variable */

	U_IM_IIP_PARAM_MON_OVERVALUE		OVERVALUE[4];	/**< OVERVALUE		(+0050h) */
} T_IM_IIP_PARAM_MON;

/** CFLOW Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER		header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=9, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_CFLOW_CFTOPCONF		CFTOPCONF;		/**< CFTOPCONF		(+0000h) */

	UCHAR				dmy_0004_000F[0x0010-0x0004];	/**< Unused variable */

	U_IM_IIP_PARAM_CFLOW_DIFF_PREWAIT	DIFF_PREWAIT;	/**< DIFF_PREWAIT	(+0010h) */
	U_IM_IIP_PARAM_CFLOW_DIFF_POSTWAIT	DIFF_POSTWAIT;	/**< DIFF_POSTWAIT	(+0014h) */
	U_IM_IIP_PARAM_CFLOW_MAGPOW			MAGPOW;			/**< MAGPOW			(+0018h) */
	U_IM_IIP_PARAM_CFLOW_CFMODE			CFMODE;			/**< CFMODE			(+001Ch) */
} T_IM_IIP_PARAM_CFLOW;

/** MFT Unit Parameter */
typedef struct{
	U_IM_IIP_PARAM_PACKET_HEADER	header0;		/**< Packet header (CtrlCode=E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET, WordLength=9, BeginningAddress=0x0000) */
	U_IM_IIP_PARAM_MFT_MFTTOPCONF	FTTOPCONF;	/**< FTTOPCONF		(+0000h) */
	U_IM_IIP_PARAM_MFT_MFTMD		MFTMD;		/**< MFTMD			(+0004h) */
	U_IM_IIP_PARAM_MFT_FLTTPSZ		FLTTPSZ;	/**< FLTTPSZ		(+0008h) */
	U_IM_IIP_PARAM_MFT_FLTOUTHSZ	FLTOUTHSZ;	/**< FLTOUTHSZ		(+000Ch) */
	U_IM_IIP_PARAM_MFT_MFTCTL		MFTCTLD[4];	/**< MFTCTLD[4]		(+0010h) */
} T_IM_IIP_PARAM_MFT;

/** Unit Registers of UNITDEBUG register space. */
typedef union {
	T_IM_IIP_PARAM_AFN_DUMP		afn;	/**< AFN unit registers. */
	T_IM_IIP_PARAM_FRECT_DUMP	frect;	/**< FRECT unit registers. */
	T_IM_IIP_PARAM_1DL_DUMP		ld;		/**< 1DLOADER unit registers. */
	T_IM_IIP_PARAM_BLEND_DUMP	blend;	/**< BLEND unit registers. */
	T_IM_IIP_PARAM_CFLOW_DUMP	cfl;	/**< CacheFlow unit registers. */
	T_IM_IIP_PARAM_CSC_DUMP		csc;	/**< CSC unit registers. */
	T_IM_IIP_PARAM_GPC_DUMP		gpc;	/**< GPC unit registers. */
	T_IM_IIP_PARAM_LUT_DUMP		lut;	/**< LUT unit registers. */
	T_IM_IIP_PARAM_MFT_DUMP		mft;	/**< MinMaxFilter unit registers. */
	T_IM_IIP_PARAM_MON_DUMP		mon;	/**< Monitor unit registers. */
	T_IM_IIP_PARAM_FLT_DUMP		flt;	/**< Space filter unit registers. */
	T_IM_IIP_PARAM_STS_DUMP		sts;	/**< Store unit registers. */
} U_IM_IIP_PARAM_UNITDEBUG;



///////////////////////////////////////////////////////////////
// GPC Struct Parameter
///////////////////////////////////////////////////////////////

/**
   PARACNT <br>
   (+0000h/+0100h/+0140h/+0180h/+01C0h/+0200h/+0240h/+0280h/+02C0h/+0300h/
	+0340h/+0380h/+03C0h/+0400h/+0440h/+0480h/+04C0h/+0500h/+0540h) <br>

	64 bytes of GPC struct parameters has header part.
	The structure of header part is described in U_IM_IIP_PARAM_GPC_PARACNT.
*/
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	PARAAD		:12;	/**< PARAAD<br>Refer to @ref E_IM_IIP_PARAM_PARAAD. */
		UINT32	SEQEN		:1;		/**< SEQEN */
		UINT32				:3;
		UINT32	LPARA		:1;		/**< LPARA */
		UINT32				:15;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_PARACNT;

// E_IM_IIP_PARAM_PARAAD_SEQ_CTRL
/** SEQLOOP (+0004h) */

/** SEQCTL (+0010h) */
typedef union {
	UINT32	word[4];	/**< All bits */
	struct{
		UINT32	CNTEND_0	:20;	/**< CNTEND_0 */
		UINT32	SEQCONF_0	:2;		/**< SEQCONF_0 */
		UINT32				:2;
		UINT32	CNFOUT_0	:1;		/**< CNFOUT_0 */
		UINT32				:3;
		UINT32	STEPNUM		:4;		/**< STEPNUM */

		UINT32	CNTEND_1	:20;	/**< CNTEND_1 */
		UINT32	SEQCONF_1	:2;		/**< SEQCONF_1 */
		UINT32				:2;
		UINT32	CNFOUT_1	:1;		/**< CNFOUT_1 */
		UINT32				:7;

		UINT32	CNTEND_2	:20;	/**< CNTEND_2 */
		UINT32	SEQCONF_2	:2;		/**< SEQCONF_2 */
		UINT32				:2;
		UINT32	CNFOUT_2	:1;		/**< CNFOUT_2 */
		UINT32				:7;

		UINT32	CNTEND_3	:20;	/**< CNTEND_3 */
		UINT32	SEQCONF_3	:2;		/**< SEQCONF_3 */
		UINT32				:2;
		UINT32	CNFOUT_3	:1;		/**< CNFOUT_3 */
		UINT32				:7;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_SEQCTL;

// E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D0_1
// E_IM_IIP_PARAM_PARAAD_PRECAL_P0_D2_3
// E_IM_IIP_PARAM_PARAAD_PRECAL_P1_D0_1
// E_IM_IIP_PARAM_PARAAD_PRECAL_P1_D2_3

/**
   PRECAL (+0104h/+0144h/+0184h/+01C4h) <br>
   Beginning 4bytes of PRECAL(+0100h/+0140h/+0180h/+01C0h) is same structure as U_IM_IIP_PARAM_GPC_PARACNT.
 */
typedef union {
	UINT32	word[5];	/**< All bits */
	struct{
		INT32	PREOFSET	:15;	/**< PREOFSET0<br>PREOFSET1 */
		UINT32				:17;

		INT32	PRECLIPMAX	:15;	/**< PRECLIPMAX0<br>PRECLIPMAX1 */
		UINT32				:17;

		INT32	PRECTHMAX	:15;	/**< PRECTHMAX0<br>PRECTHMAX1 */
		UINT32				:17;

		INT32	PRECLIPMIN	:15;	/**< PRECLIPMIN0<br>PRECLIPMIN1 */
		UINT32				:17;

		INT32	PRECTHMIN	:15;	/**< PRECTHMIN0<br>PRECTHMIN1 */
		UINT32				:5;
		UINT32	PREAE		:1;		/**< PREAE0<br>PREAE1 */
		UINT32				:11;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_PRECAL;

// E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG0
// E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG1
// E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG2
// E_IM_IIP_PARAM_PARAAD_CALC_D0_CFG3
// E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG0
// E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG1
// E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG2
// E_IM_IIP_PARAM_PARAAD_CALC_D1_CFG3
// E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG0
// E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG1
// E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG2
// E_IM_IIP_PARAM_PARAAD_CALC_D2_CFG3

/** GPC0PARA0 (+0204h/+0244h/+0284h/+02C4h/+0304h/+0344h/+0384h/+03C4h/+0404h/+0444h/+0484h/+04C4h) */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	GPC0SEL0	:5;	/**< GPC0SEL0<br>Refer to @ref E_IM_IIP_PARAM_GPCSEL. */
		UINT32	GPC0SEL1	:5;	/**< GPC0SEL1<br>Refer to @ref E_IM_IIP_PARAM_GPCSEL. */
		UINT32	GPC0SEL2	:5;	/**< GPC0SEL2<br>Refer to @ref E_IM_IIP_PARAM_GPCSEL. */
		UINT32				:1;
		UINT32	GPC0SEL3	:5;	/**< GPC0SEL3<br>Refer to @ref E_IM_IIP_PARAM_GPCSEL. */
		UINT32				:3;
		UINT32	G0MD0		:1;	/**< G0MD0<br>Refer to @ref E_IM_IIP_PARAM_GMD_MUL. */
		UINT32	G0MD1		:1;	/**< G0MD1<br>Refer to @ref E_IM_IIP_PARAM_GMD_MUL. */
		UINT32				:2;
		UINT32	G0MD2		:3;	/**< G0MD2<br>Refer to @ref E_IM_IIP_PARAM_GMD_BIN. */
		UINT32				:1;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_GPC0PARA0;

/** GPC0PARA1 (+0208h/+0248h/+0288h/+02C8h/+0308h/+0348h/+0388h/+03C8h/+0408h/+0448h/+0488h/+04C8h) */
typedef union {
	UINT32	word[2];	/**< All bits */
	struct{
		UINT32	G0RND_0		:1;	/**< G0RND_0<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G0SFT_0		:4;	/**< G0SFT_0 */
		UINT32	G0RND_1		:1;	/**< G0RND_1<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G0SFT_1		:4;	/**< G0SFT_1 */
		UINT32	G0RND_2		:1;	/**< G0RND_2<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G0SFT_2		:4;	/**< G0SFT_2 */
		UINT32	G0RND_3		:1;	/**< G0RND_3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G0SFT_3		:4;	/**< G0SFT_3 */

		UINT32	G0RND_4		:1;	/**< G0RND_4<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G0SFT_4		:4;	/**< G0SFT_4 */
		UINT32	G0RND_5		:1;	/**< G0RND_5<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G0SFT_5		:4;	/**< G0SFT_5 */
		UINT32	G0RND_6		:1;	/**< G0RND_6<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G0SFT_6		:4;	/**< G0SFT_6 */
		UINT32				:8;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_GPC0PARA1;

/** GPC1PARA0 (+0210h/+0250h/+0290h/+02D0h/+0310h/+0350h/+0390h/+03D0h/+0410h/+0450h/+0490h/+04D0h) */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	GPC1SEL0	:5;	/**< GPC1SEL0<br>Refer to @ref E_IM_IIP_PARAM_GPCSEL. */
		UINT32				:3;
		UINT32	GPC1SEL1	:5;	/**< GPC1SEL1<br>Refer to @ref E_IM_IIP_PARAM_GPCSEL. */
		UINT32				:3;
		UINT32	GPC1SEL2	:5;	/**< GPC1SEL2<br>Refer to @ref E_IM_IIP_PARAM_GPCSEL. */
		UINT32				:3;
		UINT32	G1MD0		:2;	/**< G1MD0<br>Refer to @ref E_IM_IIP_PARAM_GMD_MUL. */
		UINT32				:2;
		UINT32	G1MD1		:3;	/**< G1MD1<br>Refer to @ref E_IM_IIP_PARAM_GMD_BIN. */
		UINT32				:1;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_GPC1PARA0;

/** GPC1PARA1 (+0218h/+0258h/+0298h/+02D8h/+0318h/+0358h/+0398h/+03D8h/+0418h/+0458h/+0498h/+04D8h) */
typedef union {
	UINT32	word[2];	/**< All bits */
	struct{
		UINT32	G1RND_0		:1;	/**< G1RND_0<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G1SFT_0		:4;	/**< G1SFT_0 */
		UINT32	G1RND_1		:1;	/**< G1RND_1<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G1SFT_1		:4;	/**< G1SFT_1 */
		UINT32	G1RND_2		:1;	/**< G1RND_2<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G1SFT_2		:4;	/**< G1SFT_2 */
		UINT32	G1RND_3		:1;	/**< G1RND_3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G1SFT_3		:4;	/**< G1SFT_3 */

		UINT32	G1RND_4		:1;	/**< G1RND_4<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	G1SFT_4		:4;	/**< G1SFT_4 */
		UINT32				:8;
		UINT32				:8;
		UINT32				:8;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_GPC1PARA1;

/** SERPASS (+0220h/+0260h/+02A0h/+02E0h/+0320h/+0360h/+03A0h/+03E0h/+0420h/+0460h/+04A0h/+04E0h) */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	SEDLY0		:1;	/**< SEDLY0 */
		UINT32				:3;
		UINT32	SESEL0		:1;	/**< SESEL0<br>Refer to @ref E_IM_IIP_PARAM_SESEL0. */
		UINT32				:3;
		UINT32	SEDLY1		:2;	/**< SEDLY1<br>Refer to @ref E_IM_IIP_PARAM_SEDLY. */
		UINT32				:2;
		UINT32	SESEL1		:2;	/**< SESEL1<br>Refer to @ref E_IM_IIP_PARAM_SESEL1. */
		UINT32				:2;
		UINT32	SEDLY2		:2;	/**< SEDLY2<br>Refer to @ref E_IM_IIP_PARAM_SEDLY. */
		UINT32				:2;
		UINT32	SEDLY3		:2;	/**< SEDLY3<br>Refer to @ref E_IM_IIP_PARAM_SEDLY. */
		UINT32				:2;
		UINT32	SEDLY4		:2;	/**< SEDLY4<br>Refer to @ref E_IM_IIP_PARAM_SEDLY. */
		UINT32	SEDLY5		:2;	/**< SEDLY5<br>Refer to @ref E_IM_IIP_PARAM_SEDLY. */
		UINT32	SEDLY6		:2;	/**< SEDLY6<br>Refer to @ref E_IM_IIP_PARAM_SEDLY. */
		UINT32	SEDLY7		:2;	/**< SEDLY7<br>Refer to @ref E_IM_IIP_PARAM_SEDLY. */
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_SERPASS;

/** COMPSEL (+0224h/+0264h/+02A4h/+02E4h/+0324h/+0364h/+03A4h/+03E4h/+0424h/+0464h/+04A4h/+04E4h) */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	COMPSEL		:5;	/**< COMPSEL<br>Refer to @ref E_IM_IIP_PARAM_COMPSEL. */
		UINT32				:3;
		UINT32	CMPRND		:1;	/**< CMPRND<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32				:3;
		UINT32	CMPSFT		:4;	/**< CMPSFT */
		UINT32	CORPEN		:1;	/**< CORPEN */
		UINT32				:3;
		UINT32	CORMEN		:1;	/**< CORMEN */
		UINT32				:3;
		UINT32	CLPPEN		:1;	/**< CLPPEN */
		UINT32				:3;
		UINT32	CLPMEN		:1;	/**< CLPMEN */
		UINT32				:3;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_COMPSEL;

/** CONFCNT (+0228h/+0268h/+02A8h/+02E8h/+0328h/+0368h/+03A8h/+03E8h/+0428h/+0468h/+04A8h/+04E8h) */
typedef union {
	UINT32	word[2];	/**< All bits */
	struct{
		UINT32	CONFWEN0	:1;	/**< CONFWEN0 */
		UINT32	CONFWEN1	:1;	/**< CONFWEN1 */
		UINT32	CONFWEN2	:1;	/**< CONFWEN2 */
		UINT32	CONFWEN3	:1;	/**< CONFWEN3 */
		UINT32				:12;
		UINT32	CONFREGSEL0	:4;	/**< CONFREGSEL0<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32	CONFREGSEL1	:4;	/**< CONFREGSEL1<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32	CONFREGSEL2	:4;	/**< CONFREGSEL2<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32	CONFREGSEL3	:4;	/**< CONFREGSEL3<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */

		UINT32	CONFREGSEL4	:4;	/**< CONFREGSEL4<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32	CONFREGSEL5	:4;	/**< CONFREGSEL5<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32	CONFREGSEL6	:4;	/**< CONFREGSEL6<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32	CONFREGSEL7	:4;	/**< CONFREGSEL7<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32	CONFREGSEL8	:4;	/**< CONFREGSEL8<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32	CONFREGSEL9	:4;	/**< CONFREGSEL9<br>Refer to @ref E_IM_IIP_PARAM_CONFREGSEL. */
		UINT32				:8;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_CONFCNT;

/** CONFREG (+0230h) */

// E_IM_IIP_PARAM_PARAAD_ALPHA

/** GPCALP (+0510h) */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		INT32	GPCALPFIX	:9;	/**< GPCALPFIX_0<br>GPCALPFIX_1<br>GPCALPFIX_2<br>GPCALPFIX_3 */
		UINT32				:11;
		UINT32	GPCALPSEL0	:2;	/**< GPCALPSEL0_0<br>GPCALPSEL0_1<br>GPCALPSEL0_2<br>GPCALPSEL0_3<br>Refer to @ref E_IM_IIP_PARAM_GPCALPSEL. */
		UINT32	GPCALPSEL1	:2;	/**< GPCALPSEL1_0<br>GPCALPSEL1_1<br>GPCALPSEL1_2<br>GPCALPSEL1_3<br>Refer to @ref E_IM_IIP_PARAM_GPCALPSEL. */
		UINT32	GPCALPMODE	:3;	/**< GPCALPMODE_0<br>GPCALPMODE_1<br>GPCALPMODE_2<br>GPCALPMODE_3<br>Refer to @ref E_IM_IIP_PARAM_GPCALPMODE. */
		UINT32				:5;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_GPCALP;

// E_IM_IIP_PARAM_PARAAD_INTER_COMP_CALC

/** P1OUTMD (+0550h) */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	P1RNDO_0	:1;	/**< P1RNDO_0<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	P1SFTO_0	:4;	/**< P1SFTO_0 */
		UINT32	P1SELO_0	:2;	/**< P1SELO_0<br>Refer to @ref E_IM_IIP_PARAM_P1SELO */
		UINT32				:1;
		UINT32	P1RNDO_1	:1;	/**< P1RNDO_1<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	P1SFTO_1	:4;	/**< P1SFTO_1 */
		UINT32	P1SELO_1	:2;	/**< P1SELO_1<br>Refer to @ref E_IM_IIP_PARAM_P1SELO */
		UINT32				:1;
		UINT32	P1RNDO_2	:1;	/**< P1RNDO_2<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	P1SFTO_2	:4;	/**< P1SFTO_2 */
		UINT32	P1SELO_2	:2;	/**< P1SELO_2<br>Refer to @ref E_IM_IIP_PARAM_P1SELO */
		UINT32				:1;
		UINT32	P1RNDO_3	:1;	/**< P1RNDO_3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	P1SFTO_3	:4;	/**< P1SFTO_3 */
		UINT32	P1SELO_3	:2;	/**< P1SELO_3<br>Refer to @ref E_IM_IIP_PARAM_P1SELO */
		UINT32				:1;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_P1OUTMD;

/** P0OUTMD_D3 (+0554h) */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	P0RNDO_D3	:1;	/**< P0RNDO_D3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	P0SFTO_D3	:4;	/**< P0SFTO_D3 */
		UINT32	P0SELO_D3	:2;	/**< P0SELO_D3<br>Refer to @ref E_IM_IIP_PARAM_P0SELO */
		UINT32				:1;
		UINT32				:24;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_P0OUTMD_D3;

/** ELECAL (+0560h) */
typedef union {
	UINT32	word[2];	/**< All bits */
	struct{
		UINT32	ELESEL0		:2;	/**< ELESEL0_0<br>ELESEL0_1<br>ELESEL0_2<br>ELESEL0_3<br>Refer to @ref E_IM_IIP_PARAM_ELESEL. */
		UINT32	ELERD0		:1;	/**< ELERD0_0<br>ELERD0_1<br>ELERD0_2<br>ELERD0_3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	ELESHIFT0	:5;	/**< ELESHIFT0_0<br>ELESHIFT0_1<br>ELESHIFT0_2<br>ELESHIFT0_3<br>Refer to @ref E_IM_IIP_PARAM_ELESHIFT. */
		UINT32	ELESEL1		:2;	/**< ELESEL1_0<br>ELESEL1_1<br>ELESEL1_2<br>ELESEL1_3<br>Refer to @ref E_IM_IIP_PARAM_ELESEL. */
		UINT32	ELERD1		:1;	/**< ELERD1_0<br>ELERD1_1<br>ELERD1_2<br>ELERD1_3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	ELESHIFT1	:5;	/**< ELESHIFT1_0<br>ELESHIFT1_1<br>ELESHIFT1_2<br>ELESHIFT1_3<br>Refer to @ref E_IM_IIP_PARAM_ELESHIFT. */
		UINT32	ELESEL2		:2;	/**< ELESEL2_0<br>ELESEL2_1<br>ELESEL2_2<br>ELESEL2_3<br>Refer to @ref E_IM_IIP_PARAM_ELESEL. */
		UINT32	ELERD2		:1;	/**< ELERD2_0<br>ELERD2_1<br>ELERD2_2<br>ELERD2_3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	ELESHIFT2	:5;	/**< ELESHIFT2_0<br>ELESHIFT2_1<br>ELESHIFT2_2<br>ELESHIFT2_3<br>Refer to @ref E_IM_IIP_PARAM_ELESHIFT. */
		UINT32	ELESEL3		:2;	/**< ELESEL3_0<br>ELESEL3_1<br>ELESEL3_2<br>ELESEL3_3<br>Refer to @ref E_IM_IIP_PARAM_ELESEL. */
		UINT32	ELERD3		:1;	/**< ELERD3_0<br>ELERD3_1<br>ELERD3_2<br>ELERD3_3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	ELESHIFT3	:5;	/**< ELESHIFT3_0<br>ELESHIFT3_1<br>ELESHIFT3_2<br>ELESHIFT3_3<br>Refer to @ref E_IM_IIP_PARAM_ELESHIFT. */

		UINT32	ELEMODE0	:3;	/**< ELEMODE0_0<br>ELEMODE0_1<br>ELEMODE0_2<br>ELEMODE0_3<br>Refer to @ref E_IM_IIP_PARAM_ELEMODE. */
		UINT32				:1;
		UINT32	ELEMODE1	:3;	/**< ELEMODE1_0<br>ELEMODE1_1<br>ELEMODE1_2<br>ELEMODE1_3<br>Refer to @ref E_IM_IIP_PARAM_ELEMODE. */
		UINT32				:1;
		UINT32	ELEMODE2	:3;	/**< ELEMODE2_0<br>ELEMODE2_1<br>ELEMODE2_2<br>ELEMODE2_3<br>Refer to @ref E_IM_IIP_PARAM_ELEMODE. */
		UINT32				:1;
		UINT32	ELERDO		:1;	/**< ELERDO_0<br>ELERDO_1<br>ELERDO_2<br>ELERDO_3<br>Refer to @ref E_IM_IIP_PARAM_RND. */
		UINT32	ELESHIFTO	:4;	/**< ELESHIFTO_0<br>ELESHIFTO_1<br>ELESHIFTO_2<br>ELESHIFTO_3 */
		UINT32				:15;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_GPC_ELECAL;

/**
 GPC struct paramters which be associated with
 <ul>
	 <li>Sequence control
 </ul>
 */
typedef struct{
	U_IM_IIP_PARAM_GPC_PARACNT		PARACNT;	/**< PARACNT (+0000h) */
	UINT32							SEQLOOP;	/**< SEQLOOP (+0004h) */

	UCHAR							dmy_0008_000F[0x0010-0x0008];	/**< Unused variable */

	U_IM_IIP_PARAM_GPC_SEQCTL		SEQCTL;		/**< SEQCTL (+0010h) */

	UCHAR							dmy_0020_003F[0x0040-0x0020];	/**< Unused variable */
} T_IM_IIP_PARAM_GPC_SEQ_CTRL;

/**
 GPC struct paramters which be associated with
 <ul>
	 <li>Pre-calculation (PORT0 D0, D1)
	 <li>Pre-calculation (PORT0 D2, D3)
	 <li>Pre-calculation (PORT1 D0, D1)
	 <li>Pre-calculation (PORT1 D2, D3)
 </ul>
 */
typedef struct{
	U_IM_IIP_PARAM_GPC_PARACNT		PARACNT;	/**< PRECAL (+0100h/+0140h/+180h/+1C0h) */
	U_IM_IIP_PARAM_GPC_PRECAL		PRECAL[2];	/**< PRECAL (+0104h/+0144h/+184h/+1C04) */

	UCHAR							dmy_002C_003F[0x0040-0x002C];	/**< Unused variable */
} T_IM_IIP_PARAM_GPC_PRECAL;

/**
 GPC struct paramters which be associated with
 <ul>
	 <li>Configuration[0] of General calculation for D0 compoment
	 <li>Configuration[1] of General calculation for D0 compoment
	 <li>Configuration[2] of General calculation for D0 compoment
	 <li>Configuration[3] of General calculation for D0 compoment
	 <li>Configuration[0] of General calculation for D1 compoment
	 <li>Configuration[1] of General calculation for D1 compoment
	 <li>Configuration[2] of General calculation for D1 compoment
	 <li>Configuration[3] of General calculation for D1 compoment
	 <li>Configuration[0] of General calculation for D2 compoment
	 <li>Configuration[1] of General calculation for D2 compoment
	 <li>Configuration[2] of General calculation for D2 compoment
	 <li>Configuration[3] of General calculation for D2 compoment
 </ul>
 */
typedef struct{
	U_IM_IIP_PARAM_GPC_PARACNT		PARACNT;	/**< PARACNT   (+0200h/+0240h/+0280h/+02C0h/+0300h/+0340h/+0380h/+03C0h/+0400h/+0440h/+0480h/+04C0h) */
	U_IM_IIP_PARAM_GPC_GPC0PARA0	GPC0PARA0;	/**< GPC0PARA0 (+0204h/+0244h/+0284h/+02C4h/+0304h/+0344h/+0384h/+03C4h/+0404h/+0444h/+0484h/+04C4h) */
	U_IM_IIP_PARAM_GPC_GPC0PARA1	GPC0PARA1;	/**< GPC0PARA1 (+0208h/+0248h/+0288h/+02C8h/+0308h/+0348h/+0388h/+03C8h/+0408h/+0448h/+0488h/+04C8h) */
	U_IM_IIP_PARAM_GPC_GPC1PARA0	GPC1PARA0;	/**< GPC1PARA0 (+0210h/+0250h/+0290h/+02D0h/+0310h/+0350h/+0390h/+03D0h/+0410h/+0450h/+0490h/+04D0h) */

	UCHAR							dmy_0014_0017[0x0018-0x0014];	/**< Unused variable */

	U_IM_IIP_PARAM_GPC_GPC1PARA1	GPC1PARA1;	/**< GPC1PARA1 (+0218h/+0258h/+0298h/+02D8h/+0318h/+0358h/+0398h/+03D8h/+0418h/+0458h/+0498h/+04D8h) */
	U_IM_IIP_PARAM_GPC_SERPASS		SERPASS;	/**< SERPASS   (+0220h/+0260h/+02A0h/+02E0h/+0320h/+0360h/+03A0h/+03E0h/+0420h/+0460h/+04A0h/+04E0h) */
	U_IM_IIP_PARAM_GPC_COMPSEL		COMPSEL;	/**< COMPSEL   (+0224h/+0264h/+02A4h/+02E4h/+0324h/+0364h/+03A4h/+03E4h/+0424h/+0464h/+04A4h/+04E4h) */
	U_IM_IIP_PARAM_GPC_CONFCNT		CONFCNT;	/**< CONFCNT   (+0228h/+0268h/+02A8h/+02E8h/+0328h/+0368h/+03A8h/+03E8h/+0428h/+0468h/+04A8h/+04E8h) */
	UINT32							CONFREG[4];	/**< CONFREG   (+0230h/+0270h/+02B0h/+02F0h/+0330h/+0370h/+03B0h/+03F0h/+0430h/+0470h/+04B0h/+04F0h) */
} T_IM_IIP_PARAM_GPC_CALC;

/**
 GPC struct paramters which be associated with
 <ul>
	 <li>Configuration[0][1][2][3] of Alpha
 </ul>
 */
typedef struct{
	U_IM_IIP_PARAM_GPC_PARACNT		PARACNT;	/**< PARACNT (+0500h) */

	UCHAR							dmy_0004_000F[0x0010-0x0004];	/**< Unused variable */

	U_IM_IIP_PARAM_GPC_GPCALP		GPCALP[4];	/**< GPCALP (+0510h) */

	UCHAR							dmy_0020_003F[0x0040-0x0020];	/**< Unused variable */
} T_IM_IIP_PARAM_GPC_ALPHA;

/**
 GPC struct paramters which be associated with
 <ul>
	 <li>Inter color calculation
 </ul>
 */
typedef struct{
	U_IM_IIP_PARAM_GPC_PARACNT		PARACNT;	/**< PARACNT (+0540h) */

	UCHAR							dmy_0004_000F[0x0010-0x0004];	/**< Unused variable */

	U_IM_IIP_PARAM_GPC_P1OUTMD		P1OUTMD;	/**< P1OUTMD (+0550h) */
	U_IM_IIP_PARAM_GPC_P0OUTMD_D3	P0OUTMD_D3;	/**< P0OUTMD_D3 (+0554h) */

	UCHAR							dmy_0018_001F[0x0020-0x0018];	/**< Unused variable */

	U_IM_IIP_PARAM_GPC_ELECAL		ELECAL[3];	/**< ELECAL (+0560h) */

	UCHAR							dmy_0038_003F[0x0040-0x0038];	/**< Unused variable */
} T_IM_IIP_PARAM_GPC_INTER_COMP_CALC;


///////////////////////////////////////////////////////////////
// AFN Struct Parameter
///////////////////////////////////////////////////////////////

/**
   AFN Struct Parameter STAX/STAY
   <br>
   [+0000~0007]
   <br>
   [+0020~0027]
   <br>
   [+0040~0047]
   <br>
   [+0060~0067]
   <br>
 */
typedef union {
	UINT32	word[2];	/**< All bits */
	struct{
		INT32  STAX		:31;	/**< STAX0 [CALMETHOD.OPCOL=0]<br>STAX0/STAX1/STAX2/STAX3 [CALMETHOD.OPCOL=1] */
		UINT32			:1;

		INT32  STAY		:31;	/**< STAY0 [CALMETHOD.OPCOL=0]<br>STAY0/STAY1/STAY2/STAY3 [CALMETHOD.OPCOL=1] */
		UINT32			:1;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_AFN_STAXY;

/**
   AFN Struct Parameter DXX/DXY/DYX/DYY
   <br>
   [+0008~0017]
   <br>
   [+0028~0037]
   <br>
   [+0048~0057]
   <br>
   [+0068~0077]
   <br>
 */
typedef union {
	UINT32	word[4];	/**< All bits */
	struct{
		INT32  DXX			:24;	/**< DXX0 [CALMETHOD.OPCOL=0]<br>DXX0/DXX1/DXX2/DXX3 [CALMETHOD.OPCOL=1] */
		UINT32				:8;

		INT32  DXY			:24;	/**< DXY0 [CALMETHOD.OPCOL=0]<br>DXY0/DXY1/DXY2/DXY3 [CALMETHOD.OPCOL=1] */
		UINT32				:8;

		INT32  DYX			:24;	/**< DYX0 [CALMETHOD.OPCOL=0]<br>DYX0/DYX1/DYX2/DYX3 [CALMETHOD.OPCOL=1] */
		UINT32				:8;

		INT32  DYY			:24;	/**< DYY0 [CALMETHOD.OPCOL=0]<br>DYY0/DYY1/DYY2/DYY3 [CALMETHOD.OPCOL=1] */
		UINT32				:8;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_AFN_D_OUT_IN;

/**
   AFN Struct Parameter AFNDVSZ/AFNDHSZ
   <br>
   [+0018~001B]
 */
typedef union {
	UINT32	word;	/**< All bits */
	struct{
		UINT32	AFNDHSZ		:14;	/**< AFNDHSZ */
		UINT32				:2;

		UINT32	AFNDVSZ		:14;	/**< AFNDVSZ */
		UINT32				:2;
	}bit;	/**< Bit field */
} U_IM_IIP_PARAM_AFN_SZ;


/** AFN Struct Paramter [CALMETHOD.OPCOL=0] */
typedef struct{
	U_IM_IIP_PARAM_AFN_STAXY		STAXY0;		/**< [+0000~0007] */
	U_IM_IIP_PARAM_AFN_D_OUT_IN		D_OUT_IN_0;	/**< [+0008~0017] */
	U_IM_IIP_PARAM_AFN_SZ			SZ;			/**< [+0018~001B] */

	UCHAR							dmy_001C_001F[0x0020-0x001C];	/**< Unused variable */
} T_IM_IIP_PARAM_AFN_OPCOL_0;

/** AFN Struct Paramter [CALMETHOD.OPCOL=1] */
typedef struct{
	U_IM_IIP_PARAM_AFN_STAXY		STAXY0;		/**< [+0000~0007] */
	U_IM_IIP_PARAM_AFN_D_OUT_IN		D_OUT_IN_0;	/**< [+0008~0017] */
	U_IM_IIP_PARAM_AFN_SZ			SZ;			/**< [+0018~001B] */

	UCHAR							dmy_001C_001F[0x0020-0x001C];	/**< Unused variable */

	U_IM_IIP_PARAM_AFN_STAXY		STAXY1;		/**< [+0020~0027] */
	U_IM_IIP_PARAM_AFN_D_OUT_IN		D_OUT_IN_1;	/**< [+0028~0037] */

	UCHAR							dmy_0038_003F[0x0040-0x0038];	/**< Unused variable */

	U_IM_IIP_PARAM_AFN_STAXY		STAXY2;		/**< [+0040~0047] */
	U_IM_IIP_PARAM_AFN_D_OUT_IN		D_OUT_IN_2;	/**< [+0048~0057] */

	UCHAR							dmy_0058_005F[0x0060-0x0058];	/**< Unused variable */

	U_IM_IIP_PARAM_AFN_STAXY		STAXY3;		/**< [+0060~0067] */
	U_IM_IIP_PARAM_AFN_D_OUT_IN		D_OUT_IN_3;	/**< [+0068~0077] */
} T_IM_IIP_PARAM_AFN_OPCOL_1;


///////////////////////////////////////////////////////////////
// FRECT Struct Parameter
///////////////////////////////////////////////////////////////

/**
   FRECT Struct Parameter AX/AY/BX/BY/CX/CY/DX/DY
   <br>
	[+0000~001F]
   <br>
	[+0030~004F]
   <br>
	[+0050~006F]
   <br>
	[+0070~008F]
   <br>
 */
typedef union {
	struct{
		INT32	AX			:23;	/**< AX0 [CALMETHOD.OPCOL=0]<br>AX0/AX1/AX2/AX3 [CALMETHOD.OPCOL=1] */
		UINT32				:9;

		INT32	AY			:23;	/**< AY0 [CALMETHOD.OPCOL=0]<br>AY0/AY1/AY2/AY3 [CALMETHOD.OPCOL=1] */
		UINT32				:9;

		INT32	BX			:23;	/**< BX0 [CALMETHOD.OPCOL=0]<br>BX0/BX1/BX2/BX3 [CALMETHOD.OPCOL=1] */
		UINT32				:9;

		INT32	BY			:23;	/**< BY0 [CALMETHOD.OPCOL=0]<br>BY0/BY1/BY2/BY3 [CALMETHOD.OPCOL=1] */
		UINT32				:9;

		INT32	CX			:23;	/**< CX0 [CALMETHOD.OPCOL=0]<br>CX0/CX1/CX2/CX3 [CALMETHOD.OPCOL=1] */
		UINT32				:9;

		INT32	CY			:23;	/**< CY0 [CALMETHOD.OPCOL=0]<br>CY0/CY1/CY2/CY3 [CALMETHOD.OPCOL=1] */
		UINT32				:9;

		INT32	DX			:23;	/**< DX0 [CALMETHOD.OPCOL=0]<br>DX0/DX1/DX2/DX3 [CALMETHOD.OPCOL=1] */
		UINT32				:9;

		INT32	DY			:23;	/**< DY0 [CALMETHOD.OPCOL=0]<br>DY0/DY1/DY2/DY3 [CALMETHOD.OPCOL=1] */
		UINT32				:9;
	}bit;	/**< Bit field */
	UINT32	word[8];	/**< All bits */
} U_IM_IIP_PARAM_FRECT_XY;

/**
   FRECT Struct Parameter SZPOWH/SZPOWV
   <br>
   [+0020~0023]
   */
typedef union {
	struct{
		UINT32	SZPOWH		:3;	/**< SZPOWH<br>Refer to @ref E_IM_IIP_PARAM_SZPOWH */
		UINT32				:5;
		UINT32	SZPOWV		:3;	/**< SZPOWV<br>Refer to @ref E_IM_IIP_PARAM_SZPOWV */
		UINT32				:5;
		UINT32				:16;
	}bit;	/**< Bit field */
	UINT32	word;	/**< All bits */
} U_IM_IIP_PARAM_FRECT_SZ;

/** FRECT Struct Paramter [CALMETHOD.OPCOL=0] */
typedef struct{
	U_IM_IIP_PARAM_FRECT_XY			XY0;	/**< [+0000~001F] */
	U_IM_IIP_PARAM_FRECT_SZ			SZ;		/**< [+0020~0023] */

	UCHAR							dmy_0024_0027[0x0028-0x0024];	/**< Unused variable */
} T_IM_IIP_PARAM_FRECT_OPCOL_0;

/** FRECT Struct Paramter [CALMETHOD.OPCOL=1] */
typedef struct{
	U_IM_IIP_PARAM_FRECT_XY			XY0;	/**< [+0000~001F] */
	U_IM_IIP_PARAM_FRECT_SZ			SZ;		/**< [+0020~0023] */

	UCHAR							dmy_0024_002F[0x0030-0x0024];	/**< Unused variable */

	U_IM_IIP_PARAM_FRECT_XY			XY1;	/**< [+0030~004F] */
	U_IM_IIP_PARAM_FRECT_XY			XY2;	/**< [+0050~006F] */
	U_IM_IIP_PARAM_FRECT_XY			XY3;	/**< [+0070~008F] */
} T_IM_IIP_PARAM_FRECT_OPCOL_1;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif	// _IM_IIP_PARAM_H_

