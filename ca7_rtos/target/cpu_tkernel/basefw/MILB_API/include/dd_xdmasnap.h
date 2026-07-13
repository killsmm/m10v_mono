/**
 * @file		dd_xdmasnap.h
 * @brief		AXI DMA driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_xdmasnap
@{
	- @ref DMA_Sync_Sample
	- @ref DMA_Async_Sample
*/

#ifndef _DD_XDMASNAP_H_
#define _DD_XDMASNAP_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Interrupt flag
#define	D_DD_XDMASNAP_FLG_CH0						(0x00000001)					/**< Event flag of channel 0 */
#define	D_DD_XDMASNAP_FLG_CH1						(0x00000002)					/**< Event flag of channel 1 */
#define	D_DD_XDMASNAP_FLG_CH2						(0x00000004)					/**< Event flag of channel 2 */
#define	D_DD_XDMASNAP_FLG_CH3						(0x00000008)					/**< Event flag of channel 3 */

// Num of channel
#define D_DD_XDMASNAP_CH_NUM_MAX					(4)								/**< The maximum values of number of channels */

// Return value of DMA processing result
#define	D_DD_XDMASNAP_SYSTEM_ERR			(D_DD_XDMASNAP | D_DDIM_SYSTEM_ERROR)			/**< System Error */
#define	D_DD_XDMASNAP_INPUT_PARAM_ERR		(D_DD_XDMASNAP | D_DDIM_INPUT_PARAM_ERROR)		/**< Input Parameter Error */
#define	D_DD_XDMASNAP_TIMEOUT_ERR			(D_DD_XDMASNAP | D_DDIM_TIMEOUT)				/**< Time Out */
#define	D_DD_XDMASNAP_EXC_LOCK_NG			(D_DD_XDMASNAP | D_DDIM_EXT_LOCK_NG)			/**< Exclusive control NG */
#define	D_DD_XDMASNAP_BUSY_ERR				(D_DD_XDMASNAP | D_DDIM_MACRO_BUSY_NG)			/**< DMA Busy */
#define	D_DD_XDMASNAP_SEM_NG				(D_DD_XDMASNAP | D_DDIM_SEM_NG)					/**< XDMASNAP Semaphore acquisition NG */
#define	D_DD_XDMASNAP_SEM_TIMEOUT			(D_DD_XDMASNAP | D_DDIM_SEM_TIMEOUT)			/**< XDMASNAP Semaphore acquisition Time Out */

// wait mode
#define	D_DD_XDMASNAP_WAITMODE_CPU					(0)								/**< CPU observes register mode  (interrupt no use) */
#define	D_DD_XDMASNAP_WAITMODE_EVENT				(1)								/**< event flg wait mode         (interrupt use) */

// DMA exclusion mode
#define D_DD_XDMASNAP_EXC_OFF						(0)								/**< Exclusive control release */
#define D_DD_XDMASNAP_EXC_ON						(1)								/**< Exclusive control setting */

/*** Value of setting XDACS register ***/
// XE(XDMAC Enable)
#define D_DD_XDMASNAP_XDACS_XE_DISABLE				(0x00)							/**< XDMAC (All channels) is disabled. */
#define D_DD_XDMASNAP_XDACS_XE_ENABLE				(0x01)							/**< XDMAC (All channels) is enabled. */

// CP(Channel Priority)
#define D_DD_XDMASNAP_XDACS_CP_FIXED				(0x00)							/**< Fixed (Ch0>Ch1>Ch2>Ch3>Ch4>Ch5>Ch6>Ch7) */
#define D_DD_XDMASNAP_XDACS_CP_ROTATED				(0x01)							/**< Rotated (Round-robin mode) */

// LP(Low Power)
#define D_DD_XDMASNAP_XDACS_LP_DISABLE				(0x00)							/**< Low Power interface is disabled. */
#define D_DD_XDMASNAP_XDACS_LP_ENABLE				(0x01)							/**< Low Power interface is enabled. */

// XS(XDMAC Status )
#define D_DD_XDMASNAP_XDACS_XS_NOTACTIVE			(0x00)							/**< The XDMAC is not active. */
#define D_DD_XDMASNAP_XDACS_XS_ACTIVE				(0x01)							/**< The XDMAC is active. */

/*** Value of setting XDSAC register ***/
// SBS( Source Access Burst Size)
#define D_DD_XDMASNAP_XDSAC_SBS_BYTE				(0x00)							/**< Burst Size is 1 / Transfer size is Byte */
#define D_DD_XDMASNAP_XDSAC_SBS_HALFWORD			(0x01)							/**< Burst Size is 2 / Transfer size is Half word */
#define D_DD_XDMASNAP_XDSAC_SBS_WORD				(0x02)							/**< Burst Size is 4 / Transfer size is Word */
#define D_DD_XDMASNAP_XDSAC_SBS_DOUBLEWORD			(0x03)							/**< Burst Size is 8 / Transfer size is Double word */

// SBL(Source Access Burst Length)
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_01			(0x00)							/**< Burst Length is 1 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_02			(0x01)							/**< Burst Length is 2 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_03			(0x02)							/**< Burst Length is 3 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_04			(0x03)							/**< Burst Length is 4 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_05			(0x04)							/**< Burst Length is 5 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_06			(0x05)							/**< Burst Length is 6 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_07			(0x06)							/**< Burst Length is 7 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_08			(0x07)							/**< Burst Length is 8 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_09			(0x08)							/**< Burst Length is 9 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_10			(0x09)							/**< Burst Length is 10 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_11			(0x0a)							/**< Burst Length is 11 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_12			(0x0b)							/**< Burst Length is 12 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_13			(0x0c)							/**< Burst Length is 13 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_14			(0x0d)							/**< Burst Length is 14 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_15			(0x0e)							/**< Burst Length is 15 */
#define D_DD_XDMASNAP_XDSAC_SBL_LENGTH_16			(0x0f)							/**< Burst Length is 16 */

// SAF ( Source Address Fix )
#define D_DD_XDMASNAP_XDSAC_SAF_NOTFIX				(0x00)							/**< Source Address is not fixed. */
#define D_DD_XDMASNAP_XDSAC_SAF_FIX					(0x01)							/**< Source Address is fixed. */

// SRL(Source Address Reload )
#define D_DD_XDMASNAP_XDSAC_SRL_DISABLE				(0x00)							/**< Source address reload is disabled. */
#define D_DD_XDMASNAP_XDSAC_SRL_ENABLE				(0x01)							/**< Source address reload is enabled. */

/*** Value of setting XDDAC register ***/
// DBS(Destination Access Burst Size )
#define D_DD_XDMASNAP_XDDAC_DBS_BYTE				(0x00)							/**< Burst Size is 1 / Transfer size is Byte */
#define D_DD_XDMASNAP_XDDAC_DBS_HALFWORD			(0x01)							/**< Burst Size is 2 / Transfer size is Half word */
#define D_DD_XDMASNAP_XDDAC_DBS_WORD				(0x02)							/**< Burst Size is 4 / Transfer size is Word */
#define D_DD_XDMASNAP_XDDAC_DBS_DOUBLEWORD			(0x03)							/**< Burst Size is 8 / Transfer size is Double word */

// DBL(Destination Access Burst Length)
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_01			(0x00)							/**< Burst Length is 1 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_02			(0x01)							/**< Burst Length is 2 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_03			(0x02)							/**< Burst Length is 3 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_04			(0x03)							/**< Burst Length is 4 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_05			(0x04)							/**< Burst Length is 5 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_06			(0x05)							/**< Burst Length is 6 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_07			(0x06)							/**< Burst Length is 7 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_08			(0x07)							/**< Burst Length is 8 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_09			(0x08)							/**< Burst Length is 9 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_10			(0x09)							/**< Burst Length is 10 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_11			(0x0a)							/**< Burst Length is 11 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_12			(0x0b)							/**< Burst Length is 12 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_13			(0x0c)							/**< Burst Length is 13 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_14			(0x0d)							/**< Burst Length is 14 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_15			(0x0e)							/**< Burst Length is 15 */
#define D_DD_XDMASNAP_XDDAC_DBL_LENGTH_16			(0x0f)							/**< Burst Length is 16 */

// DAF(Destination Address Fix)
#define D_DD_XDMASNAP_XDDAC_DAF_NOTFIX				(0x00)							/**< Destination Address is not fixed. */
#define D_DD_XDMASNAP_XDDAC_DAF_FIX					(0x01)							/**< Destination Address is fixed. */

// DRL(Destination Address Reload )
#define D_DD_XDMASNAP_XDDAC_DRL_DISABLE				(0x00)							/**< Destination address reload is disabled. */
#define D_DD_XDMASNAP_XDDAC_DRL_ENABLE				(0x01)							/**< Destination address reload is enabled. */

/*** Value of setting XDDCC register ***/
// DCN (Descriptor Chain Next Valid  )
#define D_DD_XDMASNAP_XDDCC_DCN_INVALID				(0x00)							/**< Next Descriptor Chain is invalid. */
#define D_DD_XDMASNAP_XDDCC_DCN_VALID				(0x01)							/**< Next Descriptor Chain is valid. */

/*** Value of setting XDDES register ***/
// CE(Channel Enable.)
#define D_DD_XDMASNAP_XDDES_CE_DISABLE				(0x00)							/**< Channel is disabled. */
#define D_DD_XDMASNAP_XDDES_CE_ENABLE				(0x01)							/**< Channel is enabled. */

// SE(Software Enable )
#define D_DD_XDMASNAP_XDDES_SE_DISABLE				(0x00)							/**< Software DMA is disabled. */
#define D_DD_XDMASNAP_XDDES_SE_ENABLE				(0x01)							/**< Software DMA is enabled. */

// TF(Transfer Factor )
#define D_DD_XDMASNAP_XDDES_TF_NONE					(0x00)							/**< Factor is None. Detection is - */
#define D_DD_XDMASNAP_XDDES_TF_SOFT					(0x01)							/**< Factor is Software. Detection is Register write (to SE bit) */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_0				(0x02)							/**< Factor is DREQ[0].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_1				(0x03)							/**< Factor is DREQ[1].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_2				(0x04)							/**< Factor is DREQ[2].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_3				(0x05)							/**< Factor is DREQ[3].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_4				(0x06)							/**< Factor is DREQ[4].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_5				(0x07)							/**< Factor is DREQ[5].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_6				(0x08)							/**< Factor is DREQ[6].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_7				(0x09)							/**< Factor is DREQ[7].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_8				(0x0a)							/**< Factor is DREQ[8].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_9				(0x0b)							/**< Factor is DREQ[9].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_10				(0x0c)							/**< Factor is DREQ[10].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_11				(0x0d)							/**< Factor is DREQ[11].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_12				(0x0e)							/**< Factor is DREQ[12].Detection is High level */
#define D_DD_XDMASNAP_XDDES_TF_IDREQ_13				(0x0f)							/**< Factor is DREQ[13].Detection is High level */

// SA(Serial Access)
#define D_DD_XDMASNAP_XDDES_SA_DISABLE				(0x00)							/**< Serial Access is disabled. */
#define D_DD_XDMASNAP_XDDES_SA_ENABLE				(0x01)							/**< Serial Access is enabled. */

// BT(Block transfer )
#define D_DD_XDMASNAP_XDDES_BT_DISABLE				(0x00)							/**< Block transfer is disabled. (Burst transfer is enabled.) */
#define D_DD_XDMASNAP_XDDES_BT_ENABLE				(0x01)							/**< Block transfer is enabled. (Burst transfer is disabled.) */

// BR(Transfer Byte Count Reload)
#define D_DD_XDMASNAP_XDDES_BR_DISABLE				(0x00)							/**< The transfer byte count reload is disabled. */
#define D_DD_XDMASNAP_XDDES_BR_ENABLE				(0x01)							/**< The transfer byte count reload is enabled. */

// AT(DACK Assert Timing)
#define D_DD_XDMASNAP_XDDES_AT_SOURCE				(0x00)							/**< DACK is asserted while the channel accesses the source slave. */
#define D_DD_XDMASNAP_XDDES_AT_DESTINATION			(0x01)							/**< DACK is asserted while the channel accesses the destination slave. */

// EI(Error Interrupt)
#define D_DD_XDMASNAP_XDDES_EI_DISABLE				(0x00)							/**< The error interrupt is disabled. */
#define D_DD_XDMASNAP_XDDES_EI_ENABLE				(0x01)							/**< The error interrupt is enabled. */

// TI(Termination Interupt )
#define D_DD_XDMASNAP_XDDES_TI_DISABLE				(0x00)							/**< The termination interrupt is disabled. */
#define D_DD_XDMASNAP_XDDES_TI_ENABLE				(0x01)							/**< The termination interrupt is enabled. */

/*** Value of setting XDDPC register ***/
// SP(Source Access Protection Level )
#define D_DD_XDMASNAP_XDDPC_SP_NORMAL_NONSECURE		(0x00)							/**< Normal access and Non-secure access */
#define D_DD_XDMASNAP_XDDPC_SP_PRIVILEGED_NONSECURE	(0x01)							/**< Privileged access and Non-secure access */
#define D_DD_XDMASNAP_XDDPC_SP_NORMAL_SECURE		(0x02)							/**< Normal access and Secure access */
#define D_DD_XDMASNAP_XDDPC_SP_PRIVILEGED_SECURE	(0x03)							/**< Privileged access and Secure access */

// DP( Destination Access Protection Level)
#define D_DD_XDMASNAP_XDDPC_DP_NORMAL_NONSECURE		(0x00)							/**< Normal access and Non-secure access */
#define D_DD_XDMASNAP_XDDPC_DP_PRIVILEGED_NONSECURE	(0x01)							/**< Privileged access and Non-secure access */
#define D_DD_XDMASNAP_XDDPC_DP_NORMAL_SECURE		(0x02)							/**< Normal access and Secure access */
#define D_DD_XDMASNAP_XDDPC_DP_PRIVILEGED_SECURE	(0x03)							/**< Privileged access and Secure access */

/*** Value of setting XDDSD register ***/
// TS(DMA transfer status )
#define D_DD_XDMASNAP_XDDSD_TS_NOTRUNNING			(0x00)							/**< The DMA transfer is not running. */
#define D_DD_XDMASNAP_XDDSD_TS_RUNNING				(0x01)							/**< The DMA transfer is running. */

// IS(Interrupt Status )
#define D_DD_XDMASNAP_XDDSD_IS_NONE					(0x00)							/**< Initial or Clear the interrupt / Status attribute is none. */
#define D_DD_XDMASNAP_XDDSD_IS_DMA_STOP_DSTP		(0x01)							/**< DMA stop by DSTP / Status attribute is ERROR. */
#define D_DD_XDMASNAP_XDDSD_IS_DMA_STOP_DISABLING	(0x02)							/**< DMA stop by disabling CE bit of XDDES reg. or XE bit of XDACS reg / Status attribute is ERROR. */
#define D_DD_XDMASNAP_XDDSD_IS_SOURCE_ERR			(0x04)							/**< Source access error / Status attribute is ERROR. */
#define D_DD_XDMASNAP_XDDSD_IS_DESTINATION_ERR		(0x05)							/**< Destination access error / Status attribute is ERROR. */
#define D_DD_XDMASNAP_XDDSD_IS_MEMORY_ERR			(0x06)							/**< Memory Access error for Descriptor Chain DMA / Status attribute is ERROR. */
#define D_DD_XDMASNAP_XDDSD_IS_NORMAL_END			(0x08)							/**< Normal End / Status attribute is END. */

/*** Value of setting XDMAC LowPower Control register ***/
// CACTIV
#define D_DD_XDMASNAP_LOWPOWER_CACTIV_NOTEQUESTING	(0x00)							/**< The bus clock is not requesting. */
#define D_DD_XDMASNAP_LOWPOWER_CACTIV_EQUESTING		(0x01)							/**< The bus clock is requesting. */

// CSYSAC
#define D_DD_XDMASNAP_LOWPOWER_CSYSAC_NOTACCEPTED	(0x00)							/**< Start request is not accepted bus clock. */
#define D_DD_XDMASNAP_LOWPOWER_CSYSAC_ACCEPTED		(0x01)							/**< Start request is accepted bus clock. */

// CSYSRE
#define D_DD_XDMASNAP_LOWPOWER_CSYSRE_STOP			(0x00)							/**< Bus clock stop request. */
#define D_DD_XDMASNAP_LOWPOWER_CSYSRE_START			(0x01)							/**< Bus clock start request. */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/** DMA Control Common */
typedef union {
	ULONG		word;						/**< for ULONG access */
	struct {
		ULONG					:30;		/**< Reserved */
		ULONG	CP				:1;			/**< Channel Priority ( 0 or 1 ) */
		ULONG	XE				:1;			/**< XDMAC Enable ( 0 or 1 ) */
	} bit;									/**< for bit access */
} U_DD_XDMASNAP_COMMON;

/** DMA Control Trns */
typedef struct {
	U_DD_XDMASNAP_COMMON	common_config;				/**< Configuration */
} T_DD_XDMASNAP_COMMON;

/** DMA Control xdsac and xddac and xddes */
typedef union {
	ULONG		word;						/**< for ULONG access */
	struct {
		ULONG	SRL				:1;			/**< Source Address Reload ( 0 or 1 ) */
		ULONG	SAF				:1;			/**< Source Address Fix ( 0 or 1 ) */
		ULONG	SBL				:4;			/**< Source Access Burst Length ( 0 to 15 ) */
		ULONG	SBS				:2;			/**< Source Access Burst Size ( 0 or 3 ) */
		ULONG	DRL				:1;			/**< Destination Address Reload ( 0 or 1 ) */
		ULONG	DAF				:1;			/**< Destination Address Fix ( 0 or 1 ) */
		ULONG	DBL				:4;			/**< Destination Access Burst Length ( 0 to 15 ) */
		ULONG	DBS				:2;			/**< Destination Access Burst Size ( 0 or 3 ) */
		ULONG	TI				:1;			/**< Termination Interupt ( 0 or 1 ) */
		ULONG	EI				:1;			/**< Error Interrupt ( 0 or 1 ) */
		ULONG	AT				:1;			/**< DACK Assert Timing ( 0 or 1 ) */
		ULONG	BR				:1;			/**< Transfer Byte Count Reload ( 0 or 1 ) */
		ULONG	BT				:1;			/**< Block transfer ( 0 or 1 ) */
		ULONG	SA				:1;			/**< Serial Access ( 0 or 1 ) */
		ULONG	TF				:4;			/**< Transfer Factor ( 0 to 15 ) */
		ULONG	SE				:1;			/**< Software Enable ( 0 or 1 ) */
		ULONG					:5;			/**< Reserved */
	} bit;									/**< for bit access */
} U_DD_XDMASNAP_CTRL1;

/** DMA Control xddcc */
typedef union {
	ULONG		word;						/**< for ULONG access */
	struct {
		ULONG	DCN				:1;			/**< Descriptor Chain Next Valid ( 0 or 1 ) */
		ULONG					:3;			/**< Reserved */
		ULONG	DCA				:28;		/**< Descriptor Chain Address */
	} bit;									/**< for bit access */
} U_DD_XDMASNAP_CTRL2;

/** DMA Control Trns */
typedef struct {
	U_DD_XDMASNAP_CTRL1	config_1;			/**< Configuration 1 */
	U_DD_XDMASNAP_CTRL2	config_2;			/**< Configuration 2 */
	ULONG			src_addr;				/**< Source Address */
	ULONG			dst_addr;				/**< Destination Address */
	ULONG			trns_size;				/**< Transfer Size (Please specify one or more) */
	VP_CALLBACK		int_handler;			/**< Interrupt Handler */
} T_DD_XDMASNAP_CTRL;

/** DMA Transfer size */
typedef struct {
	ULONG			src_addr;				/**< Source Address */
	ULONG			dst_addr;				/**< Destination Address */
	ULONG			trns_size;				/**< Transfer Size (Please specify one or more) */
} T_DD_XDMASNAP_TRNS_SIZE;

/*----------------------------------------------------------------------*/
/* Grobal Data															*/
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

//---------------------- driver  section -------------------------------
/**
Execute exclusive control for designated XDMA channel.<br>
When the designated channel is idle (un-lock) state, change the channel state to use(lock) and return "D_DDIM_OK",<br>
if the channel state is in-use (lock), return "D_DD_XDMASNAP_EXC_LOCK_NG".
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	tmout						: Time of timeout
										<ul>
										<li>Positive Value
										<li>@ref D_DDIM_USER_SEM_WAIT_POL
										<li>@ref D_DDIM_USER_SEM_WAIT_FEVR
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_EXC_LOCK_NG		: Lock Error (System Using designated channel number)
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_SEM_TIMEOUT		: Semaphore acquisition Time out
@retval	D_DD_XDMASNAP_SEM_NG			: Semaphore acquisition NG
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_XDMASNAP_Open(UCHAR ch, INT32 tmout);

/**
The member who exists in the structure of "T_DD_XDMASNAP_COMMON" is set.
@param [in]	dma_ctrl					: DMA control data. See @ref T_DD_XDMASNAP_CTRL
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_BUSY_ERR			: DMA is being forwarded
@remarks	In the case of synchronous mode, please don't setting "int_handler". (It ignores, even if it has setting.)<br>
			In the case of the asynchronous mode, an end can be supervised by calling "Dd_XDMASNAP_Wait_End()" or setting up "intHandler".<br>
			When you don't call "Dd_XDMASNAP_Wait_End()", please be sure to set up "int_handler".
*/
extern	INT32	Dd_XDMASNAP_Ctrl_Common(T_DD_XDMASNAP_COMMON const *const dma_ctrl);

/**
The member who exists in the structure of "T_DD_XDMASNAP_CTRL" is set.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	dma_ctrl_trans				: DMA control data. See @ref T_DD_XDMASNAP_CTRL
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_BUSY_ERR			: DMA is being forwarded
@remarks	In the case of synchronous mode, please don't setting "int_handler". (It ignores, even if it has setting.)<br>
			In the case of the asynchronous mode, an end can be supervised by calling "Dd_XDMASNAP_Wait_End()" or setting up "intHandler".<br>
			When you don't call "Dd_XDMASNAP_Wait_End()", please be sure to set up "int_handler".
*/
extern	INT32	Dd_XDMASNAP_Ctrl_Trns(UCHAR ch, T_DD_XDMASNAP_CTRL const *const dma_ctrl_trans);

/**
The member who exists in the structure of "T_DD_XDMASNAP_TRNS_SIZE" is set.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	dma_trns_size				: DMA transfer size data. See @ref T_DD_XDMASNAP_TRNS_SIZE
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_BUSY_ERR			: DMA is being forwarded
@remarks	Transfer MAX size of this API as following.
			<ul>
			<li>WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.
			<li>HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.
			<li>BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.
			</ul>
*/
extern	INT32	Dd_XDMASNAP_Set_Trns_Size(UCHAR ch, T_DD_XDMASNAP_TRNS_SIZE const *const dma_trns_size);

/**
DMA start of "Synchronous" mode.<br>
The operation of DMA of specified ch begins.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	status						: Channel Status Register pointer
@param [in]	wait_mode					: DMA end wait mode
										<ul>
										<li>@ref D_DD_XDMASNAP_WAITMODE_CPU
										<li>@ref D_DD_XDMASNAP_WAITMODE_EVENT
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_TIMEOUT_ERR		: Time out
@retval	D_DD_XDMASNAP_SYSTEM_ERR		: System Error
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Dd_XDMASNAP_Start_Sync(UCHAR ch, USHORT *const status, UINT32 wait_mode);

/**
DMA start of "Asynchronous" mode.<br>
The operation of DMA of specified ch begins.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	When this mode is used, the CALLBACK function is registered or Dd_XDMASNAP_Wait_End() API please call, so check the DMA forwarding completion.
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Dd_XDMASNAP_Start_Async(UCHAR ch);

/**
The operation of DMA of specified ch is stopped.

@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern	INT32	Dd_XDMASNAP_Stop(UCHAR ch);

/**
Wait end time of transfer process of designated channel.

@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	wait_time					: Wait end time of transfer process
										<ul>
										<li>Positive Value
										<li>@ref D_DDIM_USER_SEM_WAIT_POL
										<li>@ref D_DDIM_USER_SEM_WAIT_FEVR
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern	INT32	Dd_XDMASNAP_Set_Wait_Time(UCHAR ch, INT32 wait_time);

/**
Wait end of transfer process of designated channel.<br>
The value of the CSTR register is passed by the out parameter.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [out]	status					: Channel Status Register pointer
@param [in]	wait_mode					: DMA end wait mode
										<ul>
										<li>@ref D_DD_XDMASNAP_WAITMODE_CPU
										<li>@ref D_DD_XDMASNAP_WAITMODE_EVENT
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_TIMEOUT_ERR		: Time out
@retval	D_DD_XDMASNAP_SYSTEM_ERR		: System Error
@remarks	This API is used only in the asynchronous mode. (It doesn't use it in the synchronous mode.)<br>
			The DMA forwarding is observed to be completed in this API.<br>
			When the DMA forwarding is completed, the CALLBACK function is called when CALLBACK function (user interrupt handler) is registered.
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Dd_XDMASNAP_Wait_End(UCHAR ch, USHORT *const status, UINT32 wait_mode);

/**
The status bit of the DMACB register is cleared.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern	INT32	Dd_XDMASNAP_Clear_Status(UCHAR ch);

/**
Cancel exclusive control for designated DMA channel.<br>
When the designated channel is under transferring, stop transfer immediately and cancel lock.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_SEM_NG			: Semaphore acquisition NG
@remarks	This API forced cancel exclusive control if process is under executing.
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_XDMASNAP_Close(UCHAR ch);

/**
The content of the DMACB register is acquired.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [out]	xdmac_status			: XDMAC Status Register pointer
@param [out]	transfer_status			: Channel Transfer Status Register pointer
@param [out]	interrupt_status		: Channel Interrupt Status Register pointer
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern	INT32	Dd_XDMASNAP_Get_Status(UCHAR ch, USHORT *const xdmac_status, USHORT *const transfer_status, USHORT *const interrupt_status);

/**
The value of the transferd size of the specified channel is acquired.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@retval	size							: Transferd size of the specified channel
@remarks	This API forced cancel exclusive control if process is under executing.
*/
extern	ULONG	Dd_XDMASNAP_Get_Trns_Size(UCHAR ch);

/**
The value of the source address (DMACSA) of the specified channel is acquired.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@retval	src_addr						: Source address of the specified channel
*/
extern	ULONG	Dd_XDMASNAP_Get_Src_Addr(UCHAR ch);

/**
The value of the destination address (DMACDA) of the specified channel is acquired.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@retval	dst_addr						: Destination address of the specified channel
*/
extern	ULONG	Dd_XDMASNAP_Get_Dst_Addr(UCHAR ch);

/**
Low Power is set.
@param [in]	lowpower					: Low Power
										<ul>
										<li>0:D_DD_XDMASNAP_XDACS_LP_DISABLE
										<li>1:D_DD_XDMASNAP_XDACS_LP_ENABLE
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern	INT32	Dd_XDMASNAP_Set_LowPower(UCHAR lowpower);

/**
Source Protection code is set. # Current vesion, DO NOT support this API.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	protect_code				: Protection Code
										<ul>
										<li>0:D_DD_XDMASNAP_XDDPC_SP_NORMAL_NONSECURE
										<li>1:D_DD_XDMASNAP_XDDPC_SP_PRIVILEGED_NONSECURE
										<li>2:D_DD_XDMASNAP_XDDPC_SP_NORMAL_SECURE
										<li>3:D_DD_XDMASNAP_XDDPC_SP_PRIVILEGED_SECURE
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	It is allowed for a master who is not in the privileged (secure) mode to read this register.
*/
extern	INT32	Dd_XDMASNAP_Set_Source_Protect(UCHAR ch, UCHAR protect_code);

/**
Destination Protection code is set. # Current vesion, DO NOT support this API.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	protect_code				: Protection Code
										<ul>
										<li>0:D_DD_XDMASNAP_XDDPC_SP_NORMAL_NONSECURE
										<li>1:D_DD_XDMASNAP_XDDPC_SP_PRIVILEGED_NONSECURE
										<li>2:D_DD_XDMASNAP_XDDPC_SP_NORMAL_SECURE
										<li>3:D_DD_XDMASNAP_XDDPC_SP_PRIVILEGED_SECURE
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	It is allowed for a master who is not in the privileged (secure) mode to read this register.
*/
extern	INT32	Dd_XDMASNAP_Set_Destination_Protect(UCHAR ch, UCHAR protect_code);

/**
The operation of All DMA channel is stopped.
*/
extern	VOID	Dd_XDMASNAP_Stop_All_Ch(VOID);

/**
Set Interrupt handler address.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	int_handler					: Callback function pointer
*/
extern	VOID	Dd_XDMASNAP_Set_Int_Handler(UCHAR ch, VOID (*int_handler)(VOID));

/**
Interrupt handler of DMA for transfer process is finished.<br>
The register value of CSTR is set to the argument of the callback function of the user registration.<br>
The type of the argument is "USHORT*" type.
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern	VOID	Dd_XDMASNAP_Int_Handler(UCHAR ch);

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/** @name Utility Functions
@{*/
/**
DMA forwarding between SDRAM-SDRAM.<br>
It waits until forwarding ends. (synchronization)
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	src_addr					: Source address
@param [in]	dst_addr					: Destination address
@param [in]	size						: Transfer size
@param [in]	wait_mode					: DMA end wait mode
										<ul>
										<li>@ref D_DD_XDMASNAP_WAITMODE_CPU
										<li>@ref D_DD_XDMASNAP_WAITMODE_EVENT
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_EXC_LOCK_NG		: Lock Error (System Using designated channel number)
@retval	D_DD_XDMASNAP_BUSY_ERR			: DMA is being forwarded. 
@remarks	The unit of forwarding makes a large size give priority.<br>
			Transfer MAX size of this API as following.
			<ul>
			<li>WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.
			<li>HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.
			<li>BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.
			</ul>
*/
extern	INT32	Dd_XDMASNAP_Copy_SDRAM_Sync(UCHAR ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode);

/**
DMA forwarding between SDRAM-SDRAM.<br>
It doesn't wait until the forwarding completion is done. (Asynchronization)
@param [in]	ch							: Channel number (0 to @ref D_DD_XDMASNAP_CH_NUM_MAX - 1)
@param [in]	src_addr					: Source address
@param [in]	dst_addr					: Destination address
@param [in]	size						: Transfer size
@param [in]	intHandler					: Pointer of callback function
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_XDMASNAP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_XDMASNAP_BUSY_ERR			: DMA is being forwarded
@remarks	The unit of forwarding makes a large size give priority.<br>
			Please call Dd_XDMASNAP_Open() before calling this function.<br>
			Please call Dd_XDMASNAP_Close() when the DMA forwarding ends.<br>
			Transfer MAX size of this API as following.<br>
			<ul>
			<li>WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.<br>
			<li>HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.<br>
			<li>BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.<br>
			</ul>
*/
extern	INT32	Dd_XDMASNAP_Copy_SDRAM_Async(UCHAR ch, ULONG src_addr, ULONG dst_addr, ULONG size, VP_CALLBACK intHandler);
/*@}*/
#endif

#ifdef __cplusplus
}
#endif

#endif	// _DD_XDMASNAP_H_

/*@}*/
/** @weakgroup dd_xdmasnap
@{
	@section DMA_Sync_Sample		Sample of DMA (Sync)
	The example of executing synchronous processing is as follows.
	<br>
	@code
	INT32 ret;
	USHORT status = 0;
	T_DD_XDMASNAP_CTRL dma_ctrl_trns;
	T_DD_XDMASNAP_COMMON dma_common;
	UCHAR ch;
	UINT32 wait_mode;
	
	wait_mode = D_DD_XDMASNAP_WAITMODE_CPU;
	ch = 0;
	
	// Check transfer size
	if (((SRC_TOP_ADDR & 0x07) == 0) && ((DST_TOP_ADDR & 0x07) == 0) && ((TRANS_SIZE & 0x07) == 0)) {
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_DOUBLEWORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_DOUBLEWORD;
	}
	else if (((SRC_TOP_ADDR & 0x03) == 0) && ((DST_TOP_ADDR & 0x03) == 0) && ((TRANS_SIZE & 0x03) == 0)) {
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_WORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_WORD;
	}
	else if (((SRC_TOP_ADDR & 0x01) == 0) && ((DST_TOP_ADDR & 0x01) == 0) && ((TRANS_SIZE & 0x01) == 0)) {
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_HALFWORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_HALFWORD;
	}
	else {
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_BYTE;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_BYTE;
	}
	
	dma_ctrl_trns.trns_size       = TRANS_SIZE;
	dma_ctrl_trns.src_addr        = SRC_TOP_ADDR;
	dma_ctrl_trns.dst_addr        = DST_TOP_ADDR;
	dma_ctrl_trns.int_handler     = 0;
	
	dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMASNAP_XDSAC_SRL_DISABLE;
	dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMASNAP_XDSAC_SAF_NOTFIX;
	dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMASNAP_XDSAC_SBL_LENGTH_16;
	dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMASNAP_XDDAC_DRL_DISABLE;
	dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMASNAP_XDDAC_DAF_NOTFIX;
	dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMASNAP_XDDAC_DBL_LENGTH_16;
	
	if (wait_mode == D_DD_XDMASNAP_WAITMODE_EVENT) {
		dma_ctrl_trns.config_1.bit.TI = D_DD_XDMASNAP_XDDES_TI_ENABLE;
		dma_ctrl_trns.config_1.bit.EI = D_DD_XDMASNAP_XDDES_EI_ENABLE;
	}
	else {
		dma_ctrl_trns.config_1.bit.TI = D_DD_XDMASNAP_XDDES_TI_DISABLE;
		dma_ctrl_trns.config_1.bit.EI = D_DD_XDMASNAP_XDDES_EI_DISABLE;
	}
	
	dma_ctrl_trns.config_1.bit.AT = D_DD_XDMASNAP_XDDES_AT_SOURCE;
	dma_ctrl_trns.config_1.bit.BR = D_DD_XDMASNAP_XDDES_BR_DISABLE;
	dma_ctrl_trns.config_1.bit.BT = D_DD_XDMASNAP_XDDES_BT_DISABLE;
	dma_ctrl_trns.config_1.bit.SA = D_DD_XDMASNAP_XDDES_SA_DISABLE;
	dma_ctrl_trns.config_1.bit.TF = D_DD_XDMASNAP_XDDES_TF_NONE;
	dma_ctrl_trns.config_1.bit.SE = D_DD_XDMASNAP_XDDES_SE_DISABLE;
	dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMASNAP_XDDCC_DCN_INVALID;
	dma_ctrl_trns.config_2.bit.DCA = 0;
	
	dma_common.common_config.bit.XE = D_DD_XDMASNAP_XDACS_XE_ENABLE;
	dma_common.common_config.bit.CP = D_DD_XDMASNAP_XDACS_CP_FIXED;
	
	ret = Dd_XDMASNAP_Open(ch,D_DDIM_WAIT_END_TIME);
	if (ret != D_DDIM_OK) {
		return ret;
	}
	
	ret = Dd_XDMASNAP_Ctrl_Common(&dma_common);
	if (ret != D_DDIM_OK) {
		return ret;// ERROR
	}
	
	while (1) {
		ret = Dd_XDMASNAP_Ctrl_Trns(ch, &dma_ctrl_trns);
		if (ret != D_DDIM_OK) {
			break;// ERROR
		}
		ret = Dd_XDMASNAP_Start_Sync(ch, &status, wait_mode);
		if (ret != D_DDIM_OK) {
			;// ERROR
		}
		break;
	}
	
	ret = Dd_XDMASNAP_Close(ch);
	if (ret != D_DDIM_OK) {
		;// ERROR
	}
	@endcode
	<br>
	@section DMA_Async_Sample		Sample of DMA (Async)
	The example of executing Asynchronous processing is as follows.
	@code
	INT32              ret;
	USHORT             status = 0;
	T_DD_XDMASNAP_CTRL dma_ctrl_trns;
	T_DD_XDMASNAP_COMMON dma_common;
	UCHAR                ch;
	UINT32               wait_mode;
	wait_mode                = D_DD_XDMASNAP_WAITMODE_EVENT;
	ch = 0;
	
	// Check transfer size
	if (((SRC_TOP_ADDR & 0x07) == 0) && ((DST_TOP_ADDR & 0x07) == 0) && ((TRANS_SIZE & 0x07) == 0)) {
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_DOUBLEWORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_DOUBLEWORD;
	}
	else if (((SRC_TOP_ADDR & 0x03) == 0) && ((DST_TOP_ADDR & 0x03) == 0) && ((TRANS_SIZE & 0x03) == 0)) {
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_WORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_WORD;
	}
	else if (((SRC_TOP_ADDR & 0x01) == 0) && ((DST_TOP_ADDR & 0x01) == 0) && ((TRANS_SIZE & 0x01) == 0)) {
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_HALFWORD;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_HALFWORD;
	}
	else {
		dma_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_BYTE;
		dma_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_BYTE;
	}
	
	dma_ctrl_trns.trns_size       = TRANS_SIZE;
	dma_ctrl_trns.src_addr        = SRC_TOP_ADDR;
	dma_ctrl_trns.dst_addr        = DST_TOP_ADDR;
	dma_ctrl_trns.int_handler     = 0;
	
	dma_ctrl_trns.config_1.bit.SRL = D_DD_XDMASNAP_XDSAC_SRL_DISABLE;
	dma_ctrl_trns.config_1.bit.SAF = D_DD_XDMASNAP_XDSAC_SAF_NOTFIX;
	dma_ctrl_trns.config_1.bit.SBL = D_DD_XDMASNAP_XDSAC_SBL_LENGTH_16;
	dma_ctrl_trns.config_1.bit.DRL = D_DD_XDMASNAP_XDDAC_DRL_DISABLE;
	dma_ctrl_trns.config_1.bit.DAF = D_DD_XDMASNAP_XDDAC_DAF_NOTFIX;
	dma_ctrl_trns.config_1.bit.DBL = D_DD_XDMASNAP_XDDAC_DBL_LENGTH_16;
	
	if (wait_mode == D_DD_XDMASNAP_WAITMODE_EVENT) {
		dma_ctrl_trns.config_1.bit.TI = D_DD_XDMASNAP_XDDES_TI_ENABLE;
		dma_ctrl_trns.config_1.bit.EI = D_DD_XDMASNAP_XDDES_EI_ENABLE;
	}
	else {
		dma_ctrl_trns.config_1.bit.TI = D_DD_XDMASNAP_XDDES_TI_DISABLE;
		dma_ctrl_trns.config_1.bit.EI = D_DD_XDMASNAP_XDDES_EI_DISABLE;
	}
	
	dma_ctrl_trns.config_1.bit.AT = D_DD_XDMASNAP_XDDES_AT_SOURCE;
	dma_ctrl_trns.config_1.bit.BR = D_DD_XDMASNAP_XDDES_BR_DISABLE;
	dma_ctrl_trns.config_1.bit.BT = D_DD_XDMASNAP_XDDES_BT_DISABLE;
	dma_ctrl_trns.config_1.bit.SA = D_DD_XDMASNAP_XDDES_SA_DISABLE;
	dma_ctrl_trns.config_1.bit.TF = D_DD_XDMASNAP_XDDES_TF_NONE;
	dma_ctrl_trns.config_1.bit.SE = D_DD_XDMASNAP_XDDES_SE_DISABLE;
	dma_ctrl_trns.config_2.bit.DCN = D_DD_XDMASNAP_XDDCC_DCN_INVALID;
	dma_ctrl_trns.config_2.bit.DCA = 0;
	
	dma_common.common_config.bit.XE = D_DD_XDMASNAP_XDACS_XE_ENABLE;
	dma_common.common_config.bit.CP = D_DD_XDMASNAP_XDACS_CP_FIXED;
	
	ret = Dd_XDMASNAP_Open(ch,D_DDIM_WAIT_END_TIME);
	if (ret != D_DDIM_OK) {
		return ret;
	}
	
	ret = Dd_XDMASNAP_Ctrl_Common(&dma_common);
	if (ret != D_DDIM_OK) {
		return ret;// ERROR
	}
	
	while (1) {
		ret = Dd_XDMASNAP_Ctrl_Trns(ch, &dma_ctrl_trns);
		if (ret != D_DDIM_OK) {
			break;// ERROR
		}
		ret = Dd_XDMASNAP_Start_Async(ch);
		if (ret != D_DDIM_OK) {
			;// ERROR
		}
		ret = Dd_XDMASNAP_Wait_End(ch, &status, wait_mode);
		if (ret != 0) {
			// ERROR
			break;
		}
		break;
	}
	
	ret = Dd_XDMASNAP_Close(ch);
	if (ret != D_DDIM_OK) {
		;// ERROR
	}
	@endcode
@}*/
