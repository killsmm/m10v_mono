/**
 * @file		im_sdramc.h
 * @brief		SDRAMC macro wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup image
@{
	@addtogroup im_sdramc
	@{
*/

#ifndef __IM_SDRAMC_H__
#define __IM_SDRAMC_H__

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** Result.
*/
#define	D_IM_SDRAMC_INPUT_PARAM_ERROR		(D_IM_SDRAMC | D_DDIM_INPUT_PARAM_ERROR)	/**< Processing result code<br> Input parameter error			(0x11000001) */
#define	D_IM_SDRAMC_MACRO_BUSY				(D_IM_SDRAMC | D_DDIM_MACRO_BUSY_NG)		/**< Processing result code<br> Macro busy 						(0x11000005) */
#define D_IM_SDRAMC_SEM_NG					(D_IM_SDRAMC | D_DDIM_SEM_NG)				/**< Processing result code<br> Semaphore acquisition NG		(0x11000010) */
#define	D_IM_SDRAMC_SEM_TIMEOUT_ERR			(D_IM_SDRAMC | D_DDIM_SEM_TIMEOUT)			/**< Processing result code<br> Semaphore acquisition Time Out	(0x11000011) */
#define	D_IM_SDRAMC_TIMEOUT					(D_IM_SDRAMC | D_DDIM_TIMEOUT)				/**< Processing result code<br> Time Out 						(0x11000013) */
#define	D_IM_SDRAMC_AXI_ERROR				(D_IM_SDRAMC | D_DDIM_AXI_ERROR)			/**< Processing result code<br> AXI BUS error					(0x1100001A) */
#define	D_IM_SDRAMC_NG						(D_IM_SDRAMC | D_DDIM_SYSTEM_ERROR)			/**< Processing result code<br> Processing NG					(0x11000099) */
#define	D_IM_SDRAMC_PROCESS_END				(D_IM_SDRAMC | 0x000000FF)					/**< Processing result code<br> Process End 					(0x110000FF) */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** SDRAMC channel.
*/
typedef enum {
	E_IM_SDRAMC_CH_0 = 0,					/**< Channel 0.				*/
	E_IM_SDRAMC_CH_1,						/**< Channel 1.				*/
	E_IM_SDRAMC_CH_MAX						/**< Channel MAX.			*/
} E_IM_SDRAMC_CH;

/** Port number.
*/
typedef enum {
	E_IM_SDRAMC_PORT_MX10 = 0,				/**< MXIC1(MX10).			*/
	E_IM_SDRAMC_PORT_MX11,					/**< MXIC1(MX11).			*/
	E_IM_SDRAMC_PORT_MX12,					/**< MXIC1(MX12).			*/
	E_IM_SDRAMC_PORT_MX13,					/**< MXIC1(MX13).			*/
	E_IM_SDRAMC_PORT_MX20,					/**< MXIC2(MX20).			*/
	E_IM_SDRAMC_PORT_MX21,					/**< MXIC2(MX21).			*/
	E_IM_SDRAMC_PORT_MX22,					/**< MXIC2(MX22).			*/
	E_IM_SDRAMC_PORT_MX23,					/**< MXIC2(MX23).			*/
	E_IM_SDRAMC_PORT_MX30,					/**< MXIC3(MX30).			*/
	E_IM_SDRAMC_PORT_MX31,					/**< MXIC3(MX31).			*/
	E_IM_SDRAMC_PORT_MX32,					/**< MXIC3(MX32).			*/
	E_IM_SDRAMC_PORT_MX33,					/**< MXIC3(MX33).			*/
	E_IM_SDRAMC_PORT_MX40,					/**< MXIC4(MX40).			*/
	E_IM_SDRAMC_PORT_MX50,					/**< MXIC5(MX50).			*/
	E_IM_SDRAMC_PORT_MX51,					/**< MXIC5(MX51).			*/
	E_IM_SDRAMC_PORT_MX00,					/**< MXIC0(MX00).			*/
	E_IM_SDRAMC_PORT_MX01,					/**< MXIC0(MX01).			*/
	E_IM_SDRAMC_PORT_MX60,					/**< MXIC6(MX60).			*/
	E_IM_SDRAMC_PORT_RB0,					/**< H.264(RB0).			*/
	E_IM_SDRAMC_PORT_RB1,					/**< H.264(RB1).			*/
	E_IM_SDRAMC_PORT_RB2,					/**< H.264(RB2).			*/
	E_IM_SDRAMC_PORT_DS0,					/**< H.265 Common(DS0).		*/
	E_IM_SDRAMC_PORT_DS1,					/**< H.265 Common(DS1).		*/
	E_IM_SDRAMC_PORT_SDM,					/**< H.265 Common(SDM).		*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	E_IM_SDRAMC_PORT_VE0,					/**< H.265 Encode(VE0).		*/
	E_IM_SDRAMC_PORT_VE1,					/**< H.265 Encode(VE1).		*/
	E_IM_SDRAMC_PORT_VE2,					/**< H.265 Encode(VE2).		*/
	E_IM_SDRAMC_PORT_VD0,					/**< H.265 Decode(VD0).		*/
	E_IM_SDRAMC_PORT_VD1,					/**< H.265 Decode(VD1).		*/
	E_IM_SDRAMC_PORT_VD2,					/**< H.265 Decode(VD2).		*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_IM_SDRAMC_PORT_VCOREW,				/**< H.265 Encode(VCOREW).	*/
	E_IM_SDRAMC_PORT_VCORER0,				/**< H.265 Encode(VCORER0).	*/
	E_IM_SDRAMC_PORT_VCORER1,				/**< H.265 Encode(VCORER1).	*/
	E_IM_SDRAMC_PORT_VD0,					/**< H.265 Decode(VD0).		*/
	E_IM_SDRAMC_PORT_VD1,					/**< H.265 Decode(VD1).		*/
	E_IM_SDRAMC_PORT_VD2,					/**< H.265 Decode(VD2).		*/
	E_IM_SDRAMC_PORT_VCPU,					/**< H.265 Encode(VCPU).	*/
	E_IM_SDRAMC_PORT_VCOREM,				/**< H.265 Encode(VCOREM).	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	E_IM_SDRAMC_PORT_MAX					/**< Port MAX.				*/
} E_IM_SDRAMC_PORT;

/** MXIC-ID number.
*/
typedef enum {
	E_IM_SDRAMC_SUBID_MXIC1_1 = 0,			/**< MXIC1(W1/R1).			*/
	E_IM_SDRAMC_SUBID_MXIC1_2,				/**< MXIC1(W2/R2).			*/
	E_IM_SDRAMC_SUBID_MXIC1_3,				/**< MXIC1(W3/R3).			*/
	E_IM_SDRAMC_SUBID_MXIC1_4,				/**< MXIC1(W4).				*/
	E_IM_SDRAMC_SUBID_MXIC1_5,				/**< MXIC1(W5).				*/
	E_IM_SDRAMC_SUBID_MXIC1_6,				/**< MXIC1(W6/R6).			*/
	E_IM_SDRAMC_SUBID_MXIC1_7,				/**< MXIC1(R7).				*/
	E_IM_SDRAMC_SUBID_MXIC1_8,				/**< MXIC1(W8/R8).			*/
	E_IM_SDRAMC_SUBID_MXIC1_9,				/**< MXIC1(W9/R9).			*/
	E_IM_SDRAMC_SUBID_MXIC1_10,				/**< MXIC1(W10/R10).		*/
	E_IM_SDRAMC_SUBID_MXIC1_11,				/**< MXIC1(W11/R11).		*/
	E_IM_SDRAMC_SUBID_MXIC1_12,				/**< MXIC1(W12/R12).		*/
	E_IM_SDRAMC_SUBID_MXIC1_13,				/**< MXIC1(W13).			*/
	E_IM_SDRAMC_SUBID_MXIC1_14,				/**< MXIC1(W14/R14).		*/
	E_IM_SDRAMC_SUBID_MXIC1_15,				/**< MXIC1(W15).			*/
	E_IM_SDRAMC_SUBID_MXIC2_1,				/**< MXIC2(W1/R1).			*/
	E_IM_SDRAMC_SUBID_MXIC2_2,				/**< MXIC2(W2/R2).			*/
	E_IM_SDRAMC_SUBID_MXIC2_3,				/**< MXIC2(W3/R3).			*/
	E_IM_SDRAMC_SUBID_MXIC2_4,				/**< MXIC2(W4).				*/
	E_IM_SDRAMC_SUBID_MXIC2_5,				/**< MXIC2(W5).				*/
	E_IM_SDRAMC_SUBID_MXIC2_6,				/**< MXIC2(W6/R6).			*/
	E_IM_SDRAMC_SUBID_MXIC2_7,				/**< MXIC2(W7/R7).			*/
	E_IM_SDRAMC_SUBID_MXIC2_8,				/**< MXIC2(W8/R8).			*/
	E_IM_SDRAMC_SUBID_MXIC2_9,				/**< MXIC2(W9/R9).			*/
	E_IM_SDRAMC_SUBID_MXIC2_10,				/**< MXIC2(W10/R10).		*/
	E_IM_SDRAMC_SUBID_MXIC2_11,				/**< MXIC2(W11/R11).		*/
	E_IM_SDRAMC_SUBID_MXIC2_12,				/**< MXIC2(W12/R12).		*/
	E_IM_SDRAMC_SUBID_MXIC2_13,				/**< MXIC2(W13).			*/
	E_IM_SDRAMC_SUBID_MXIC2_14,				/**< MXIC2(W14/R14).		*/
	E_IM_SDRAMC_SUBID_MXIC2_15,				/**< MXIC2(W15).			*/
	E_IM_SDRAMC_SUBID_MXIC3_1,				/**< MXIC3(W1/R1).			*/
	E_IM_SDRAMC_SUBID_MXIC3_2,				/**< MXIC3(W2/R2).			*/
	E_IM_SDRAMC_SUBID_MXIC3_3,				/**< MXIC3(W3/R3).			*/
	E_IM_SDRAMC_SUBID_MXIC3_4,				/**< MXIC3(W4/R4).			*/
	E_IM_SDRAMC_SUBID_MXIC3_5,				/**< MXIC3(W5/R5).			*/
	E_IM_SDRAMC_SUBID_MXIC3_6,				/**< MXIC3(W6/R6).			*/
	E_IM_SDRAMC_SUBID_MXIC3_7,				/**< MXIC3(W7/R7).			*/
	E_IM_SDRAMC_SUBID_MXIC3_8,				/**< MXIC3(R8).				*/
	E_IM_SDRAMC_SUBID_MXIC3_9,				/**< MXIC3(W9/R9).			*/
	E_IM_SDRAMC_SUBID_MXIC3_10,				/**< MXIC3(W10/R10).		*/
	E_IM_SDRAMC_SUBID_MXIC3_11,				/**< MXIC3(R11).			*/
	E_IM_SDRAMC_SUBID_MXIC3_12,				/**< MXIC3(W12/R12).		*/
	E_IM_SDRAMC_SUBID_MXIC3_13,				/**< MXIC3(W13/R13).		*/
	E_IM_SDRAMC_SUBID_MXIC3_14,				/**< MXIC3(W14).			*/
	E_IM_SDRAMC_SUBID_MXIC3_15,				/**< MXIC3(W15/R15).		*/
	E_IM_SDRAMC_SUBID_MXIC3_16,				/**< MXIC3(W16/R16).		*/
	E_IM_SDRAMC_SUBID_MXIC3_17,				/**< MXIC3(W17/R17).		*/
	E_IM_SDRAMC_SUBID_MXIC3_18,				/**< MXIC3(R18).			*/
	E_IM_SDRAMC_SUBID_MXIC3_64,				/**< MXIC3(W64/R64).		*/
	E_IM_SDRAMC_SUBID_MXIC3_65,				/**< MXIC3(W65/R65).		*/
	E_IM_SDRAMC_SUBID_MXIC3_66,				/**< MXIC3(W66/R66).		*/
	E_IM_SDRAMC_SUBID_MXIC3_67,				/**< MXIC3(W67/R67).		*/
	E_IM_SDRAMC_SUBID_MXIC5_1,				/**< MXIC5(W1/R1).			*/
	E_IM_SDRAMC_SUBID_MXIC5_2,				/**< MXIC5(W2/R2).			*/
	E_IM_SDRAMC_SUBID_MXIC5_3,				/**< MXIC5(W3/R3).			*/
	E_IM_SDRAMC_SUBID_MXIC5_4,				/**< MXIC5(W4/R4).			*/
	E_IM_SDRAMC_SUBID_MXIC5_5,				/**< MXIC5(W5/R5).			*/
	E_IM_SDRAMC_SUBID_MXIC6_0,				/**< MXIC6(CA7-Core0).		*/
	E_IM_SDRAMC_SUBID_MXIC6_1,				/**< MXIC6(CA7-Core1).		*/
	E_IM_SDRAMC_SUBID_MXIC6_2,				/**< MXIC6(CA7-Core2).		*/
	E_IM_SDRAMC_SUBID_MXIC6_3,				/**< MXIC6(CA7-Core3).		*/
	E_IM_SDRAMC_SUBID_MXIC6_ETC,			/**< MXIC6(CA7-Etc).		*/
	E_IM_SDRAMC_SUBID_MAX					/**< Port MAX.				*/
} E_IM_SDRAMC_SUBID;

/** Data buffer number.
*/
typedef enum {
	E_IM_SDRAMC_DATABUF_0 = 0,				/**< Data buffer  0(MX10/MX11).		*/
	E_IM_SDRAMC_DATABUF_1,					/**< Data buffer  1(MX12/MX13).		*/
	E_IM_SDRAMC_DATABUF_2,					/**< Data buffer  2(MX20/MX21).		*/
	E_IM_SDRAMC_DATABUF_3,					/**< Data buffer  3(MX22/MX23).		*/
	E_IM_SDRAMC_DATABUF_4,					/**< Data buffer  4(MX30/MX31).		*/
	E_IM_SDRAMC_DATABUF_5,					/**< Data buffer  5(MX32/MX33).		*/
	E_IM_SDRAMC_DATABUF_6,					/**< Data buffer  6(MX50/MX51).		*/
	E_IM_SDRAMC_DATABUF_7,					/**< Data buffer  7(MX40).			*/
	E_IM_SDRAMC_DATABUF_8,					/**< Data buffer  8(MX00/MX01).		*/
	E_IM_SDRAMC_DATABUF_9,					/**< Data buffer  9(MX60).			*/
	E_IM_SDRAMC_DATABUF_10,					/**< Data buffer 10(RB0).			*/
	E_IM_SDRAMC_DATABUF_11,					/**< Data buffer 11(RB1).			*/
	E_IM_SDRAMC_DATABUF_12,					/**< Data buffer 12(RB2).			*/
	E_IM_SDRAMC_DATABUF_13,					/**< Data buffer 13(DS0/DS1).		*/
	E_IM_SDRAMC_DATABUF_14,					/**< Data buffer 14(SDM).			*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	E_IM_SDRAMC_DATABUF_15,					/**< Data buffer 15(VE0/VE1/VE2).	*/
	E_IM_SDRAMC_DATABUF_16,					/**< Data buffer 16(VD0/VD1).		*/
	E_IM_SDRAMC_DATABUF_17,					/**< Data buffer 17(VD2).			*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_IM_SDRAMC_DATABUF_16,					/**< Data buffer 16(VD0/VD1).		*/
	E_IM_SDRAMC_DATABUF_18,					/**< Data buffer 18(VCPU).			*/
	E_IM_SDRAMC_DATABUF_19,					/**< Data buffer 19(VCOREW/VCORER0).*/
	E_IM_SDRAMC_DATABUF_20,					/**< Data buffer 20(VCORER1).		*/
	E_IM_SDRAMC_DATABUF_21,					/**< Data buffer 21(VCOREM).		*/
	E_IM_SDRAMC_DATABUF_22,					/**< Data buffer 22(VD2).			*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	E_IM_SDRAMC_DATABUF_MAX					/**< Data buffer number MAX.		*/
} E_IM_SDRAMC_DATABUF;

/** enable/disable.
*/
typedef enum {
	E_IM_SDRAMC_EN_DISABLE = 0,				/**< disable.				*/
	E_IM_SDRAMC_EN_ENABLE					/**< enable.				*/
} E_IM_SDRAMC_EN;

/** Read/Write access.
*/
typedef enum {
	E_IM_SDRAMC_ACCESS_READ = 0,			/**< Read acsess.			*/
	E_IM_SDRAMC_ACCESS_WRITE				/**< Write access.			*/
} E_IM_SDRAMC_ACCESS;

/** Access monitor mode.
*/
typedef enum {
	E_IM_SDRAMC_MONMODE_SIZE = 0,			/**< Transfer data size.	*/
	E_IM_SDRAMC_MONMODE_COUNT				/**< Access recieve count.	*/
} E_IM_SDRAMC_MONMODE;

/** Access rank.
*/
typedef enum {
	E_IM_SDRAMC_RANK_0 = 1,					/**< MR command for Rank0.	*/
	E_IM_SDRAMC_RANK_1						/**< MR command for Rank1.	*/
} E_IM_SDRAMC_RANK;

/** Access detection area.
*/
typedef enum {
	E_IM_SDRAMC_AREA_A = 0,					/**< Area A.				*/
	E_IM_SDRAMC_AREA_B						/**< Area B.				*/
} E_IM_SDRAMC_AREA;

/** DDR-PHY trainig mode.
*/
typedef enum {
	E_IM_SDRAMC_TRAINING_MODE_WRITE = 0,	/**< Write leveling.					*/
	E_IM_SDRAMC_TRAINING_MODE_GATE,			/**< Gate training.						*/
	E_IM_SDRAMC_TRAINING_MODE_RDATA_SYNC,	/**< Read Data Synchronizer training.	*/
	E_IM_SDRAMC_TRAINING_MODE_RDATA_EYE,	/**< Read Data Eye training.			*/
	E_IM_SDRAMC_TRAINING_MODE_WDATA_EYE,	/**< Write Data Eye training.			*/
	E_IM_SDRAMC_TRAINING_MODE_RW_TEST,		/**< Read & Write test mode.			*/
	E_IM_SDRAMC_TRAINING_MODE_CA,			/**< CA training.						*/
	E_IM_SDRAMC_TRAINING_MODE_RESERVED		/**< Reserved.							*/
} E_IM_SDRAMC_TRAINING_MODE;

/** Burst limit.
*/
typedef enum {
	E_IM_SDRAMC_BURST_LIMIT_256BYTE = 4,	/**< 256 byte */
	E_IM_SDRAMC_BURST_LIMIT_512BYTE = 5,	/**< 512 byte */
} E_IM_SDRAMC_BURST_LIMIT;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Commnad arbitration information for rate control.
*/
typedef struct {
	USHORT							throughput[ E_IM_SDRAMC_PORT_MAX ];		/**< Throughput of arbitration permission (average interval) (12bits).	*/
	USHORT							latency[ E_IM_SDRAMC_PORT_MAX ];		/**< Initial latency until arbitration permission (12bits).				*/
	E_IM_SDRAMC_EN					enable[ E_IM_SDRAMC_PORT_MAX ];			/**< Rate control enable.												*/
} T_IM_SDRAMC_CMD_ARB_RATE;

/** Commnad arbitration information for priority control.
*/
typedef struct {
	UCHAR							priority[ E_IM_SDRAMC_PORT_MAX ];		/**< Priority between ports (5bits).
																				 [remarks1]A priority is high so as to be small.
																				 [remarks2]It is forbidden setting the same value between ports.	*/
	E_IM_SDRAMC_EN					enable[ E_IM_SDRAMC_PORT_MAX ];			/**< Priority control enable.											*/
} T_IM_SDRAMC_CMD_ARB_PRI;

/** Access monitor table.
*/
typedef struct {
	ULONG							data[ E_IM_SDRAMC_PORT_MAX ];			/**< Access monitor data (32bits).
																				 [remarks]The unit of transfer data size to SDRAM is 8 byte.		*/
} T_IM_SDRAMC_MONITOR;

/** Access monitor table for MXIC.
*/
typedef struct {
	ULONG							data[ E_IM_SDRAMC_SUBID_MAX ];			/**< Access monitor data for MXIC (32bits).
																				 [remarks]The unit of transfer data size to SDRAM is 8 byte.		*/
} T_IM_SDRAMC_MONITOR_MXIC;

/** User callback function pointer.
@param[in]		int_cause							Interupt cause.
														int_cause != 0 : Access detection (Particular area).
														int_cause == 0 : Access detection (Non-implementation area).
*/
typedef VOID (*T_IM_SDRAMC_CALLBACK)( ULONG int_cause );

/** Access detectin table.
*/
typedef struct {
	ULONG							s_addr;									/**< Access detection start address (32bits, 4KByte unit).				*/
	ULONG							e_addr;									/**< Access detection end address (32bits, 4KByte unit).				*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	ULONG							w_port;									/**< Write access detection port (bit array. bit[0]=0x1:MX10).			*/
	ULONG							r_port;									/**< Read access detection port (bit array. bit[0]=0x1:MX10).			*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	ULONG							w_port_l;								/**< Write access detection port (bit array. bit[0]=0x1:MX10).			*/
	ULONG							r_port_l;								/**< Read access detection port (bit array. bit[0]=0x1:MX10).			*/
	ULONG							w_port_u;								/**< Write access detection port (bit array. bit[0]=0x1:VCPU).			*/
	ULONG							r_port_u;								/**< Read access detection port (bit array. bit[0]=0x1:VCPU).			*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	T_IM_SDRAMC_CALLBACK			pCallBack;								/**< User callback function.											*/
} T_IM_SDRAMC_ACCESS_DETECTION;

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
SDRAMC acceptance capabilit set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[in]		rnum							read acceptance capability.
@param[in]		wnum							write acceptance capability.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_MX00/MX01/MX40 are setable.
*/
extern INT32 Im_SDRAMC_Set_AcceptanceCapability( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, const UCHAR rnum, const UCHAR wnum );

/**
SDRAMC acceptance capabilit get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		rnum							read acceptance capability.
@param[out]		wnum							write acceptance capability.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_MX00/MX01/MX40 are setable.
*/
extern INT32 Im_SDRAMC_Get_AcceptanceCapability( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, UCHAR* const rnum, UCHAR* const wnum );

/**
SDRAMC burst limit size set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[in]		rsize							read bytes. @ref E_IM_SDRAMC_BURST_LIMIT.
@param[in]		wsize							write bytes. @ref E_IM_SDRAMC_BURST_LIMIT.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_MX00/MX01/MX40 are setable.
*/
extern INT32 Im_SDRAMC_Set_BurstLimit( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, const E_IM_SDRAMC_BURST_LIMIT rsize, const E_IM_SDRAMC_BURST_LIMIT wsize );

/**
SDRAMC burst limit size get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		rsize							read bytes. @ref E_IM_SDRAMC_BURST_LIMIT.
@param[out]		wsize							write bytes. @ref E_IM_SDRAMC_BURST_LIMIT.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_MX00/MX01/MX40 are setable.
*/
extern INT32 Im_SDRAMC_Get_BurstLimit( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, E_IM_SDRAMC_BURST_LIMIT* const rsize, E_IM_SDRAMC_BURST_LIMIT* const wsize );

/**
SDRAMC remap address set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[in]		addr							Remap address(unit 512Mbit).
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_DS0/DS1/SDM/VE0/VE1/VE2/VD0/VD1/VD2 are setable.
*/
extern INT32 Im_SDRAMC_Set_RemapAddr( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, const LONG addr );

/**
SDRAMC remap address get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		addr							Remap address(unit 512Mbit).
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_DS0/DS1/SDM/VE0/VE1/VE2/VD0/VD1/VD2 are setable.
*/
extern INT32 Im_SDRAMC_Get_RemapAddr( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, LONG* const addr );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
SDRAMC CHSEL set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_VE0/VE1/VD1 are setable.
*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
SDRAMC CHSEL set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_VD1 are setable.
*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern INT32 Im_SDRAMC_Set_Ch_Select( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port);

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
SDRAMC CHSEL get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		chsel							CH select.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_VE0/VE1/VD1 are setable.
*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
SDRAMC CHSEL get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		chsel							CH select.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_VD1 are setable.
*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern INT32 Im_SDRAMC_Get_Ch_Select( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, E_IM_SDRAMC_CH* chsel );

/**
SDRAMC command arbitration set (rate contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		cmd_arb							Commnad arbitration information for rate control @ref T_IM_SDRAMC_CMD_ARB_RATE.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Set_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH ch, const T_IM_SDRAMC_CMD_ARB_RATE* const cmd_arb );

/**
SDRAMC command arbitration get(rate contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[out]		cmd_arb							Commnad arbitration information for rate control @ref T_IM_SDRAMC_CMD_ARB_RATE.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Get_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH ch, T_IM_SDRAMC_CMD_ARB_RATE* const cmd_arb );

/**
SDRAMC command arbitration set (priority contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		cmd_arb							Commnad arbitration information for priority control @ref T_IM_SDRAMC_CMD_ARB_PRI.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		A priority is high so as to be small.
@remarks		It is forbidden setting the same value between ports.
*/
extern INT32 Im_SDRAMC_Set_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH ch, const T_IM_SDRAMC_CMD_ARB_PRI* const cmd_arb );

/**
SDRAMC command arbitration get (priority contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		cmd_arb							Commnad arbitration information for priority control @ref T_IM_SDRAMC_CMD_ARB_PRI.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Get_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH ch, T_IM_SDRAMC_CMD_ARB_PRI* const cmd_arb );

/**
SDRAMC mode register set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		rank							Access rank.
													<ul><li>@ref E_IM_SDRAMC_RANK_0.
														<li>@ref E_IM_SDRAMC_RANK_1.</ul>
@param[in]		ma								MA value.
@param[in]		data							Write data.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Set_Mode_Register( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_RANK rank, const UCHAR ma, const UCHAR data );

/**
SDRAMC mode register get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		rank							Access rank.
													<ul><li>@ref E_IM_SDRAMC_RANK_0.
														<li>@ref E_IM_SDRAMC_RANK_1.</ul>
@param[in]		ma								MA value.
@param[out]		data							Read data.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Get_Mode_Register( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_RANK rank, const UCHAR ma, UCHAR* const data );

/**
SDRAMC BusIF clock enable.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[in]		access							Read/Write access.
													<ul><li>@ref E_IM_SDRAMC_ACCESS_READ.
														<li>@ref E_IM_SDRAMC_ACCESS_WRITE.</ul>
@param[in]		enable							Clock enable.
													<ul><li>@ref E_IM_SDRAMC_EN_DISABLE.
														<li>@ref E_IM_SDRAMC_EN_ENABLE.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		If "port" is RBx or DSx or SDM or VDx, "access" is ignored.
@remarks		If "port" is MX00 or MX01 and "access" is E_IM_SDRAMC_ACCESS_READ, "enable" is E_IM_SDRAMC_EN_DISABLE.
*/
extern INT32 Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, E_IM_SDRAMC_ACCESS access, E_IM_SDRAMC_EN enable );

/**
SDRAMC Data buffer clock enable.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		dbno							Data buffer number @ref E_IM_SDRAMC_DATABUF.
@param[in]		enable							Clock enable.
													<ul><li>@ref E_IM_SDRAMC_EN_DISABLE.
														<li>@ref E_IM_SDRAMC_EN_ENABLE.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Set_Clock_Enable_DataBuf( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_DATABUF dbno, E_IM_SDRAMC_EN enable );

/**
SDRAMC self refresh mode start.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Strat_SelfRefresh( E_IM_SDRAMC_CH ch );

/**
SDRAMC self refresh mode stop.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Stop_SelfRefresh( E_IM_SDRAMC_CH ch );

/**
SDRAMC ILP_REQ manual mode start.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Start_ManualMode( E_IM_SDRAMC_CH ch );

/**
SDRAMC ILP_REQ manual mode stop.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Stop_ManualMode( E_IM_SDRAMC_CH ch );

/**
SDRAMC DLL Re-Lock.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_ReLock_DLL( E_IM_SDRAMC_CH ch );

/**
SDRAMC DDR-PHY training start.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		mode							DDR-PHY training mode.
													<ul><li>@ref E_IM_SDRAMC_TRAINING_MODE_WRITE.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_GATE.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_RDATA_SYNC.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_RDATA_EYE.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_WDATA_EYE.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_RW_TEST.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_CA.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@retval			D_IM_SDRAMC_NG					Fail - Trainig ng.
*/
extern INT32 Im_SDRAMC_Start_Training( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_TRAINING_MODE mode );

/**
SDRAMC access detection start (Particular & Non-implementation area).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		area							Access detection area.
													<ul><li>@ref E_IM_SDRAMC_AREA_A.
														<li>@ref E_IM_SDRAMC_AREA_B.</ul>
@param[in]		acs_det							Access detection table @ref T_IM_SDRAMC_ACCESS_DETECTION.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_AREA area, const T_IM_SDRAMC_ACCESS_DETECTION* const acs_det );

/**
SDRAMC access detection stop (Particular & Non-implementation area).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		area							Access detection area.
													<ul><li>@ref E_IM_SDRAMC_AREA_A.
														<li>@ref E_IM_SDRAMC_AREA_B.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Stop_Access_Detection( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_AREA area );

/**
SDRAMC access detection information get (Particular area).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[out]		area							Access detection area.
													<ul><li>@ref E_IM_SDRAMC_AREA_A.
														<li>@ref E_IM_SDRAMC_AREA_B.</ul>
@param[out]		port							Access detection Port number @ref E_IM_SDRAMC_PORT.
@param[out]		addr							Access detection Address.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Call from user callback function.
*/
extern INT32 Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_AREA *area, E_IM_SDRAMC_PORT* port, ULONG* const addr );

/**
SDRAMC access monitor start.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		mode							Access monitor mode.
													<ul><li>@ref E_IM_SDRAMC_MONMODE_SIZE.
														<li>@ref E_IM_SDRAMC_MONMODE_COUNT.</ul>
@param[in]		write_access					Write access flag(TRUE:measurement on, FALSE:measurement off).
@param[in]		read_access						Read  access flag(TRUE:measurement on, FALSE:measurement off).
@param[in]		auto_clear						Status reg auto clear flag(TRUE:auto clear on, FALSE:auto clear off).
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_MONMODE mode, BOOL write_access, BOOL read_access, BOOL auto_clear );

/**
SDRAMC access monitor stop.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Stop_Monitor( E_IM_SDRAMC_CH ch );

/**
SDRAMC access monitor get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[out]		mon								Access monitor table @ref T_IM_SDRAMC_MONITOR.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The unit of transfer data size to SDRAM is 8 byte.
*/
extern INT32 Im_SDRAMC_Get_Monitor( E_IM_SDRAMC_CH ch, T_IM_SDRAMC_MONITOR* const mon );

/**
SDRAMC access monitor start for MXIC.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		mode							Access monitor mode.
													<ul><li>@ref E_IM_SDRAMC_MONMODE_SIZE.
														<li>@ref E_IM_SDRAMC_MONMODE_COUNT.</ul>
@param[in]		write_access					Write access flag(TRUE:measurement on, FALSE:measurement off).
@param[in]		read_access						Read  access flag(TRUE:measurement on, FALSE:measurement off).
@param[in]		auto_clear						Status reg auto clear flag(TRUE:auto clear on, FALSE:auto clear off).
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_MONMODE mode, BOOL write_access, BOOL read_access, BOOL auto_clear );

/**
SDRAMC access monitor stop for MXIC.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_SDRAMC_Stop_Monitor_Mxic( E_IM_SDRAMC_CH ch );

/**
SDRAMC access monitor stop for MXIC.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[out]		mon								Access monitor table for MXIC @ref T_IM_SDRAMC_MONITOR_MXIC.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The unit of transfer data size to SDRAM is 8 byte.
*/
extern INT32 Im_SDRAMC_Get_Monitor_Mxic( E_IM_SDRAMC_CH ch, T_IM_SDRAMC_MONITOR_MXIC* const mon );

/**
SDRAMC interupt handler.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			None.
*/
extern VOID Im_SDRAMC_Int_Handler( E_IM_SDRAMC_CH ch );

#ifdef __cplusplus
}
#endif

#endif	// __IM_SDRAMC_H__

/*	@}*/
/*@}*/
