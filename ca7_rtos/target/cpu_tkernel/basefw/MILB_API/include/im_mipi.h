/**
 * @file		im_mipi.h
 * @brief		MIPI driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
/**
@weakgroup im_mipi
@{
@}
*/

/**
@addtogroup im_mipi
@{
	- @ref Mipi_Sample_Basic
*/

#ifndef _IM_MIPI_H_
#define _IM_MIPI_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Return value of ARM watchdog timer processing result
#define D_IM_MIPI_INPUT_PARAM_ERROR	(D_IM_MIPI | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define D_IM_MIPI_NO_RECEIVE		(D_IM_MIPI | D_DDIM_NO_RECEIVE)			/**< No receive */
#define D_IM_MIPI_TIMEOUT			(D_IM_MIPI | D_DDIM_TIMEOUT)			/**< Time out */
#define D_IM_MIPI_NG				(D_IM_MIPI | D_DDIM_SYSTEM_ERROR)		/**< System error */

// General-purpose definition of register value (Effectiveness selection)
#define D_IM_MIPI_ENABLE_OFF						(0)		/**< OFF	*/
#define D_IM_MIPI_ENABLE_ON							(1)		/**< ON		*/

// DPI color coding.
#define D_IM_MIPI_COLOR_CODING_16BIT_CONFIG1		(0)				/**< 16-bit configuration 1				*/
#define D_IM_MIPI_COLOR_CODING_16BIT_CONFIG2		(1)				/**< 16-bit configuration 2				*/
#define D_IM_MIPI_COLOR_CODING_16BIT_CONFIG3		(2)				/**< 16-bit configuration 3				*/
#define D_IM_MIPI_COLOR_CODING_18BIT_CONFIG1		(3)				/**< 18-bit configuration 1				*/
#define D_IM_MIPI_COLOR_CODING_18BIT_CONFIG2		(4)				/**< 18-bit configuration 2				*/
#define D_IM_MIPI_COLOR_CODING_24BIT				(5)				/**< 24-bit								*/
#define D_IM_MIPI_COLOR_CODING_20BIT_YCC422_LP		(6)				/**< 20-bit YCbCr4:2:2 loosely packed	*/
#define D_IM_MIPI_COLOR_CODING_24BIT_YCC422			(7)				/**< 24-bit YCbCr4:2:2					*/
#define D_IM_MIPI_COLOR_CODING_16BIT_YCC422			(8)				/**< 16-bit YCbCr4:2:2					*/
#define D_IM_MIPI_COLOR_CODING_30BIT				(9)				/**< 30-bit								*/
#define D_IM_MIPI_COLOR_CODING_36BIT				(10)			/**< 36-bit								*/
#define D_IM_MIPI_COLOR_CODING_12BIT_YCC420			(11)			/**< 12-bit YCbCr4:2:0					*/

// Video mode transmission type.
#define D_IM_MIPI_VID_MODE_TYPE_NON_BURST_SYNC_PULSES	(0)			/**< 00 : Non-Burst with sync pulses	*/
#define D_IM_MIPI_VID_MODE_TYPE_NON_BURST_SYNC_EVENTS	(1)			/**< 01 : Non-Burst with sync events	*/
#define D_IM_MIPI_VID_MODE_TYPE_BURST					(2)			/**< 10 or 11 : Burst mode				*/

// Number of Pakcets.
#define D_IM_MIPI_PACKET_NUM_MAX					(16)			/**< Max number							*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Data Lane Info */
typedef enum{
	E_IM_MIPI_ACTIVE_DATA_LANE_1 = 0,					/**< One data lane    (lane 0)					*/
	E_IM_MIPI_ACTIVE_DATA_LANE_2,						/**< Two data lanes   (lanes 0 and 1)			*/
	E_IM_MIPI_ACTIVE_DATA_LANE_3,						/**< Three data lanes (lanes 0, 1, and 2)		*/
	E_IM_MIPI_ACTIVE_DATA_LANE_4						/**< Four data lanes  (lanes 0, 1, 2, and 3)	*/
} E_IM_MIPI_ACTIVE_DATA_LANE;

/** Interrupt type */
typedef enum{
	E_IM_MIPI_INT_TYPE_TO_HS_TX			= 0,			/**< Timeout.						*/
	E_IM_MIPI_INT_TYPE_DPI_PLD_WR_ERR,					/**< DPI payload write error.		*/
	E_IM_MIPI_INT_TYPE_GEN_CMD_WR_ERR,					/**< Generic command write error.	*/
	E_IM_MIPI_INT_TYPE_GEN_PLD_WR_ERR,					/**< Generic payload write error.	*/
	E_IM_MIPI_INT_TYPE_GEN_PLD_SEND_ERR,				/**< Generic payload send error.	*/
	E_IM_MIPI_INT_TYPE_MAX								/**< Interrupt type maximum value.	*/
} E_IM_MIPI_INT_TYPE;

/** PHY_RSTZ Register - phy_shutdownz */
typedef enum{
	E_IM_MIPI_PHY_SHUTDOWNZ_POWER_DOWN = 0,				/**< Power-down state	*/
	E_IM_MIPI_PHY_SHUTDOWNZ_POWER_UP					/**< Power-up state		*/
} E_IM_MIPI_PHY_SHUTDOWNZ;

/** PWR_UP Register - shutdownz */
typedef enum{
	E_IM_MIPI_SHUTDOWNZ_RESET = 0,						/**< Reset		*/
	E_IM_MIPI_SHUTDOWNZ_POWER_UP						/**< Power-up	*/
} E_IM_MIPI_SHUTDOWNZ;

/** MODE_CFG Register */
typedef enum{
	E_IM_MIPI_MODE_CFG_VIDEO = 0,						/**< Video mode		*/
	E_IM_MIPI_MODE_CFG_CMD,								/**< Command mode	*/
} E_IM_MIPI_MODE_CFG;

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/** CLKMGR_CFG Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG	tx_esc_clk_division	:8;		/**< The division factor for the TX Escape clock source.<br>
												 Value range : 0~255.<br>
												 The value 0 and 1 stop the TX_ESC clock generation.		*/
		ULONG	to_clk_division		:8;		/**< The division factor the Time Out clock.<br>
												 Value range : 0~255.										*/
		ULONG						:16;	/**< Reserve													*/
	} bit;	/**< Bit field */
} U_IM_MIPI_CLKMGR_CFG;

/** DPI_CFG_POL Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG	dataen_active_low	:1;		/**< Data enable pin.<br>
												 Value 1:active, 0:inactive		*/
		ULONG	vsync_active_low	:1;		/**< Vertical synchronism pin.<br>
												 Value 1:active, 0:inactive		*/
		ULONG	hsync_active_low	:1;		/**< Horizontal synchronism pin.<br>
												 Value 1:active, 0:inactive		*/
		ULONG	shutd_active_low	:1;		/**< Shutdown pin.<br>
												 Value 1:active, 0:inactive		*/
		ULONG	colorm_acvite_low	:1;		/**< Color mode pin.<br>
												 Value 1:active, 0:inactive		*/
		ULONG						:27;	/**< Reserve				*/
	} bit;	/**< Bit field */
} U_IM_MIPI_DPI_CFG_POL;

/** DPI_LP_CMD_TIM Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG	invact_lpcmd_time	:8;		/**< It defines the size of the largest packet(VACT).<br>
												 Value range : 0~255.											*/
		ULONG						:8;		/**< Reserve														*/
		ULONG	outvact_lpcmd_time	:8;		/**< It defines the size of the largest packet(VSA,VBP,VFP).<br>
												 Value range : 0~255.											*/
		ULONG						:8;		/**< Reserve														*/
	} bit;	/**< Bit field */
} U_IM_MIPI_DPI_LP_CMD_TIM;

/** VID_MODE_CFG Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG	vid_mode_type	:2;		/**< Video mode transmission type.<br>
											 <ul><li>@ref D_IM_MIPI_VID_MODE_TYPE_NON_BURST_SYNC_PULSES
												 <li>@ref D_IM_MIPI_VID_MODE_TYPE_NON_BURST_SYNC_EVENTS
												 <li>@ref D_IM_MIPI_VID_MODE_TYPE_BURST</ul>				*/
		ULONG					:6;		/**< Reserve														*/
		ULONG	lp_vsa_en		:1;		/**< Return to low-power inside the VSA period.<br>
											 Value 1:enable, 0:disable										*/
		ULONG	lp_vbp_en		:1;		/**< Return to low-power inside the VBP period.<br>
											 Value 1:enable, 0:disable										*/
		ULONG	lp_vfp_en		:1;		/**< Return to low-power inside the VFP period.<br>
											 Value 1:enable, 0:disable										*/
		ULONG	lp_vact_en		:1;		/**< Return to low-power inside the VACT period.<br>
											 Value 1:enable, 0:disable										*/
		ULONG	lp_hbp_en		:1;		/**< Return to low-power inside the HBP period.<br>
											 Value 1:enable, 0:disable										*/
		ULONG	lp_hfp_en		:1;		/**< Return to low-power inside the HFP period.<br>
											 Value 1:enable, 0:disable										*/
		ULONG					:1;		/**< Reserve														*/
		ULONG	lp_cmd_en		:1;		/**< Command transmission only in low-power mode.<br>
											 Value 1:enable, 0:disable										*/
		ULONG	vpg_en			:1;		/**< Video mode pattern generator.<br>
											 Value 1:enable, 0:disable										*/
		ULONG					:3;		/**< Reserve														*/
		ULONG	vpg_mode        :1;		/**< Pattern select.<br>
											 Value 1:BER pattern, 0:Color bar.								*/
		ULONG					:3;		/**< Reserve														*/
		ULONG	vpg_orientation :1;		/**< Color bar orientation.<br>
											 Value 1:Horizontal mode, 0:Vertical mode.						*/
		ULONG					:7;		/**< Reserve														*/
	} bit;	/**< Bit field */
} U_IM_MIPI_VID_MODE_CFG;

/** CMD_MODE_CFG Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG					:8;		/**< Reserve																		*/
		ULONG	gen_sw_0p_tx    :1;		/**< the Generic short write packet zero paramter command transmission type.<br>
											 Value 1:Low-power, 0:High-speed												*/
		ULONG	gen_sw_1p_tx    :1;		/**< the Generic short write packet one paramter command transmission type.<br>
											 Value 1:Low-power, 0:High-speed												*/
		ULONG	gen_sw_2p_tx    :1;		/**< the Generic short write packet two paramter command transmission type.<br>
											 Value 1:Low-power, 0:High-speed												*/
		ULONG					:3;		/**< Reserve																		*/
		ULONG	gen_lw_tx       :1;		/**< the Generic long write packet command transmission type.<br>
											 Value 1:Low-power, 0:High-speed												*/
		ULONG					:1;		/**< Reserve																		*/
		ULONG	dcs_sw_0p_tx    :1;		/**< the DCS short write packet zero paramter command transmission type.<br>
											 Value 1:Low-power, 0:High-speed												*/
		ULONG	dcs_sw_1p_tx    :1;		/**< the DCS short write packet one paramter command transmission type.<br>
											 Value 1:Low-power, 0:High-speed												*/
		ULONG					:1;		/**< Reserve																		*/
		ULONG	dcs_lw_tx       :1;		/**< the DCS long write packet command transmission type.<br>
											 Value 1:Low-power, 0:High-speed												*/
		ULONG					:12;	/**< Reserve																		*/
	} bit;	/**< Bit field */
} U_IM_MIPI_CMD_MODE_CFG;

/** GEN_HDR Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG	gen_dt				:6;		/**< Packet data type. Value range : 0~63.		*/
		ULONG	gen_vc				:2;		/**< Virtual channel ID. Value range : 0~3.		*/
		ULONG	gen_wc_lsbyte		:8;		/**< header packet's word count(LSB).<br>
												 Value range : 0~255.						*/
		ULONG	gen_wc_msbyte		:8;		/**< header packet's word count(MSB).<br>
												 Value range : 0~255.						*/
		ULONG						:8;		/**< Reserve									*/
	} bit;	/**< Bit field */
} U_IM_MIPI_GEN_HDR;

/** PHY_TMR_LPCLK_CFG Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG	phy_clkhs2lp_time	:8;		/**< Time to switch from low-power to high-speed consumption.<br>
												 Value range : 0~255.											*/
		ULONG						:8;		/**< Reserve														*/
		ULONG	phy_clklp2hs_time	:8;		/**< Time to switch from high-speed to low-power consumption.<br>
												 Value range : 0~255.											*/
		ULONG						:8;		/**< Reserve														*/
	} bit;	/**< Bit field */
} U_IM_MIPI_PHY_TMR_LPCLK_CFG;

/** PHY_TMR_CFG Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG						:16;	/**< Reserve														*/
		ULONG	phy_lp2hs_time		:8;		/**< Time to switch from high-speed to low-power consumption.<br>
												 Value range : 0~255.											*/
		ULONG	phy_hs2lp_time		:8;		/**< Time to switch from low-power to high-speed consumption.<br>
												 Value range : 0~255.											*/
	} bit;	/**< Bit field */
} U_IM_MIPI_PHY_TMR_CFG;

/** INT_MSK1 Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG	to_hs_tx			:1;		/**< Timeout.<br>
												 Value 0:Unmask, 1:Mask.			*/
		ULONG						:6;		/**< Reserve							*/
		ULONG	dpi_pld_wr_err		:1;		/**< DPI payload write error.<br>
												 Value 0:Unmask, 1:Mask.			*/
		ULONG	gen_cmd_wr_err		:1;		/**< Generic command write error.<br>
												 Value 0:Unmask, 1:Mask.			*/
		ULONG	gen_pld_wr_err		:1;		/**< Generic payload write error.<br>
												 Value 0:Unmask, 1:Mask.			*/
		ULONG	gen_pld_send_err	:1;		/**< Generic payload send error.<br>
												 Value 0:Unmask, 1:Mask.			*/
		ULONG						:21;	/**< Reserve							*/
	} bit;	/**< Bit field */
} U_IM_MIPI_INT_MSK1;

/** PHY_SETUP_CL/PHY_SETUP_DL Register structure */
typedef union {
	ULONG		word;				/**< All bits */
	struct {
		ULONG	SAP_TLPX			:8;		/**< TLPX Setup(Clock Lane) / TLPX Setup(Data Lane).<br>
												 Value range : 0~255.												*/
		ULONG	SAP_HS0				:8;		/**< Tclk-ZERO Setup(Clock Lane) / THS-ZERO Setup(Data Lane).<br>
												 Value range : 0~255.												*/
		ULONG	SAP_TRAIL			:8;		/**< TCLK-TRAIL Setup(Clock Lane) / THS-TRAIL Setup(Data Lane).<br>
												 Value range : 0~255.												*/
		ULONG	SAP_PRE				:8;		/**< TCLK-PREPARE Setup(Clock Lane) / THS-PREPARE Setup(Data Lane).<br>
												 Value range : 0~255.												*/
	} bit;	/**< Bit field */
} U_IM_MIPI_PHY_SETUP;

/**
Lane data.<br>
*/
typedef struct {
	U_IM_MIPI_PHY_SETUP				clock_lane;			/**< PHY_SETUP_CL : D-PHY setup register for Clock Lane.			*/
	U_IM_MIPI_PHY_SETUP				data_lane;			/**< PHY_SETUP_DL : D-PHY setup register for Data Lane.				*/
	UCHAR							tclkp_post;			/**< PHY_TCLKPOST.TCLK_POST : D-PHY setup register for Tclkp-post.<br>
															 Value range : 0~63.											*/
} T_IM_MIPI_DPI_LANE_INFO;

/**
DPI Resolution data.<br>
*/
typedef struct {
	E_IM_MIPI_ACTIVE_DATA_LANE		lane;				/**< PHY_IF_CFG : data lane.<br>
															 <ul><li>@ref E_IM_MIPI_ACTIVE_DATA_LANE_1
																 <li>@ref E_IM_MIPI_ACTIVE_DATA_LANE_2
																 <li>@ref E_IM_MIPI_ACTIVE_DATA_LANE_3
																 <li>@ref E_IM_MIPI_ACTIVE_DATA_LANE_4</ul>					*/
	UCHAR							vcid;				/**< DPI_VCID : virtual channel.<br>
															 Value range:0~3<br>											*/
	UCHAR							color_coding;		/**< DPI_COLOR_CODING.dpi_color_coding : color coding.<br>
															 <ul><li>@ref D_IM_MIPI_COLOR_CODING_16BIT_CONFIG1
																 <li>@ref D_IM_MIPI_COLOR_CODING_16BIT_CONFIG2
																 <li>@ref D_IM_MIPI_COLOR_CODING_16BIT_CONFIG3
																 <li>@ref D_IM_MIPI_COLOR_CODING_18BIT_CONFIG1
																 <li>@ref D_IM_MIPI_COLOR_CODING_18BIT_CONFIG2
																 <li>@ref D_IM_MIPI_COLOR_CODING_24BIT
																 <li>@ref D_IM_MIPI_COLOR_CODING_20BIT_YCC422_LP
																 <li>@ref D_IM_MIPI_COLOR_CODING_24BIT_YCC422
																 <li>@ref D_IM_MIPI_COLOR_CODING_16BIT_YCC422
																 <li>@ref D_IM_MIPI_COLOR_CODING_30BIT
																 <li>@ref D_IM_MIPI_COLOR_CODING_36BIT
																 <li>@ref D_IM_MIPI_COLOR_CODING_12BIT_YCC420</ul>			*/
	BOOL							loosely18_en;		/**< DPI_COLOR_CODING.loosely18_en : loosely enable.<br>
															 Value 1:activate, 0:inactive									*/
	U_IM_MIPI_DPI_CFG_POL			cfg_pol;			/**< DPI_CFG_POL : Polarity of the DPI signals.						*/
	E_IM_MIPI_MODE_CFG				mode_cfg;			/**< MODE_CFG : the mode of operation.<br>
															 <ul><li>@ref E_IM_MIPI_MODE_CFG_VIDEO
																 <li>@ref E_IM_MIPI_MODE_CFG_CMD</ul>						*/
	U_IM_MIPI_VID_MODE_CFG			vid_mode_cfg;		/**< VID_MODE_CFG : Several aspects of the Video mode operation		*/
	USHORT							vid_pkt_size;		/**< VID_PKT_SIZE : Video packet size.<br>
															 Value range:1~16383.											*/
	USHORT							vid_num_chunks;		/**< VID_NUM_CHUNKS : Number of chunks to use.<br>
															 Value range:0~16383.											*/
	USHORT							vid_null_size;		/**< VID_NULL_SIZE : Size of null packets.<br>
															 Value range:0~16383.											*/
	USHORT							vid_hline_time;		/**< VID_HLINE_TIME : Overall time for each video line.<br>
															 Value range:1~32767.											*/
    USHORT							vid_hsa_time;		/**< VID_HSA_TIME : Video HSA time.<br>
															 Value range:1~4095.											*/
	USHORT							vid_hbp_time;		/**< VID_HBP_TIME : Video HBP time.<br>
															 Value range:1~4095.											*/
	USHORT							vid_vsa_lines;		/**< VID_VSA_LINES : VSA period.<br>
															 Value range:1~1023.											*/
    USHORT							vid_vbp_lines;		/**< VID_VBP_LINES : VBP period.<br>
															 Value range:1~1023.											*/
	USHORT							vid_vfp_lines;		/**< VID_VFP_LINES : VFP period.<br>
															 Value range:1~1023.											*/
	USHORT							vid_vactive_lines;	/**< VID_VACTIVE_LINES : Vertical resolution of the video.<br>
															 Value range:1~16383.											*/
	U_IM_MIPI_CMD_MODE_CFG			cmd_mode_cfg;		/**< CMD_MODE_CFG : Several aspects of the command mode operation	*/
} T_IM_MIPI_DPI_RESOLUTION;

/**
Transmission configuration data.<br>
*/
typedef struct {
	U_IM_MIPI_CLKMGR_CFG			clkmgr_cfg;			/**< CLKMGR_CFG : Polarity of the DPI signals.					*/
	U_IM_MIPI_DPI_LP_CMD_TIM		dpi_lp_cmd_tim;		/**< DPI_LP_CMD_TIM : Several aspects of the Video mode operation	*/
	USHORT							timer_count;		/**< TO_CNT_CFG.hstx_to_cnt : Timer counter.<br>
															 Value range:0~65535.										*/
	U_IM_MIPI_PHY_TMR_LPCLK_CFG		phy_tmr_lpclk_cfg;	/**< PHY_TMR_LPCLK_CFG : High-speed and low-power consumption to switch time.		*/
	U_IM_MIPI_PHY_TMR_CFG			phy_tmr_cfg;		/**< PHY_TMR_CFG : High-speed and low-power consumption to switch time.		*/
} T_IM_MIPI_TRANS_CONFIG;

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

/**
 * @brief	Initialization and recovery form shutdown.
 * @param[in]	lane		data lane.<br>
 * 							<ul><li>@ref E_IM_MIPI_ACTIVE_DATA_LANE_1
 * 								<li>@ref E_IM_MIPI_ACTIVE_DATA_LANE_2
 * 								<li>@ref E_IM_MIPI_ACTIVE_DATA_LANE_3
 * 								<li>@ref E_IM_MIPI_ACTIVE_DATA_LANE_4</ul>
 * @retval	VOID.
 */
extern	VOID Im_MIPI_Init(E_IM_MIPI_ACTIVE_DATA_LANE lane);

/**
 * @brief	LPCLK control.
 * @param[in]	lpclk		LPCLK control state.<br>
 * @retval	VOID.
 */
extern	VOID Im_MIPI_Lpclk_Ctrl(UCHAR lpclk);

/**
 * @brief	Power-up/down control.
 * @param[in]	power		Power-up/down state.<br>
 * 							<ul><li>@ref E_IM_MIPI_PHY_SHUTDOWNZ_POWER_DOWN
 * 								<li>@ref E_IM_MIPI_PHY_SHUTDOWNZ_POWER_UP</ul>
 * @param[in]	reset		Reset/Power-up.<br>
 * 							<ul><li>@ref E_IM_MIPI_SHUTDOWNZ_RESET
 * 								<li>@ref E_IM_MIPI_SHUTDOWNZ_POWER_UP</ul>
 * @retval	VOID.
 */
extern	VOID Im_MIPI_Power_Ctrl(E_IM_MIPI_PHY_SHUTDOWNZ power, E_IM_MIPI_SHUTDOWNZ reset);

/**
 * @brief	Enter ULPS on both clock lane and data lane.
 * @retval	D_DDIM_OK		success.
 * @retval	D_IM_MIPI_NG	status NG.
 * @remarks	This API uses DDIM_User_Dly_Tsk().
 */
extern	INT32 Im_MIPI_Enter_Ulps(VOID);

/**
 * @brief	Exit ULPS on both clock lane and data lane.
 * @retval	D_DDIM_OK		success.
 * @retval	D_IM_MIPI_NG	status NG.
 * @remarks	This API uses DDIM_User_Dly_Tsk().
 */
extern	INT32 Im_MIPI_Exit_Ulps(VOID);

/**
 * @brief	Set clock lane / data lane / TCLK POST.
 * @param[in]	lane_info		Configure the lane.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input parameter error.
 */
extern	INT32 Im_MIPI_Set_Lane_Info(T_IM_MIPI_DPI_LANE_INFO const *const lane_info);

/**
 * @brief	Get clock lane / data lane / TCLK POST.
 * @param[out]	lane_info		Configure the lane.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input parameter error.
 */
extern	INT32 Im_MIPI_Get_Lane_Info(T_IM_MIPI_DPI_LANE_INFO *const lane_info);

/**
 * @brief	Set DPI Resolution data.
 * @param[in]	dpi			DPI Resolution data.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_MIPI_Set_DPI_Resolution(T_IM_MIPI_DPI_RESOLUTION const *const dpi);

/**
 * @brief	Get DPI Resolution data.
 * @param[out]	dpi			TDPI Resolution data.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_MIPI_Get_DPI_Resolution(T_IM_MIPI_DPI_RESOLUTION *const dpi);

/**
 * @brief	Updating the DPI Configuration.
 * @retval	D_DDIM_OK			success.
 * @retval	D_IM_MIPI_NG		NG.
 */
extern	INT32 Im_MIPI_Update_Dpi(VOID);

/**
 * @brief	Set Transmission Configuration.
 * @param[in]	config			Transmission configuration data.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_MIPI_Set_Transmission_Configuration(T_IM_MIPI_TRANS_CONFIG const *const config);

/**
 * @brief	Get Transmission Configuration.
 * @param[out]	config			Transmission configuration data.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_MIPI_Get_Transmission_Configuration(T_IM_MIPI_TRANS_CONFIG *const config);

/**
 * @brief	Paket Transmission.
 * @param[in]	header_num		header  Number. Value range : 1~16.
 * @param[in]	payload_num		payload Number. Value range : 1~14.
 * @param[in]	header			Packet header.
 * @param[in]	payload			Payload Data.<br>
 * 								Value range : 0x00000000~0xFFFFFFFF.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_MIPI_Packet_Transmission(UCHAR header_num, UCHAR payload_num, const U_IM_MIPI_GEN_HDR header[D_IM_MIPI_PACKET_NUM_MAX], const ULONG payload[D_IM_MIPI_PACKET_NUM_MAX]);

/**
 * @brief	Set Timeer count.
 * @param[in]	count			Timeer count. Value range : 0~255.
 * @retval	VOID.
 */
extern	VOID Im_MIPI_Set_Timer_Count(UCHAR count);

/**
 * @brief	Set Interrupt mask.
 * @param[in]	int_msk1		Interrupt mask Register.
 * @retval	VOID.
 */
extern	VOID Im_MIPI_Set_Int_Mask(U_IM_MIPI_INT_MSK1 int_msk1);

/**
 * @brief	Get Interrupt mask.
 * @param[out]	int_msk1		Interrupt mask Register.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_MIPI_Get_Int_Mask(U_IM_MIPI_INT_MSK1* int_msk1);

/**
 * @brief	Set interrupt callback function.
 * @param[in]	interrupt_type	Interrupt type.<br>
 * 								<ul><li>@ref E_IM_MIPI_INT_TYPE_TO_HS_TX
 * 									<li>@ref E_IM_MIPI_INT_TYPE_DPI_PLD_WR_ERR
 * 									<li>@ref E_IM_MIPI_INT_TYPE_GEN_CMD_WR_ERR
 * 									<li>@ref E_IM_MIPI_INT_TYPE_GEN_PLD_WR_ERR
 * 									<li>@ref E_IM_MIPI_INT_TYPE_GEN_PLD_SEND_ERR
 * 									<li>@ref E_IM_MIPI_INT_TYPE_GEN_PLD_SEND_ERR</ul>
 * @param[in]	vp_callback		callback funcion pointer.
 * @retval	D_DDIM_OK					success.
 * @retval	D_IM_MIPI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_MIPI_Set_Int_Callback(E_IM_MIPI_INT_TYPE interrupt_type, VP_CALLBACK vp_callback);

/**
 * @brief	Interrupt handler.
 * @retval	VOID.
 */
extern	VOID Im_MIPI_Int_Handler(VOID);

#ifdef __cplusplus
}
#endif

#endif	// _IM_MIPI_H_

/*@}*/
/*@}*/

/************************************************************************/
/* Sample Code															*/
/************************************************************************/
/** @weakgroup im_mipi
@{
<hr>
@section Mipi_Sample_Basic		Sample of MIPI(Basic sequence)
@code
// SAMPLE CODE //
VOID im_mipi_sample_initialize(VOID)
{
	INT32 ret = D_DDIM_OK;
	T_IM_MIPI_DPI_RESOLUTION dpi;
	T_IM_MIPI_TRANS_CONFIG config;
	
	// init.
	Im_MIPI_Init(E_IM_MIPI_ACTIVE_DATA_LANE_1);
	
	// DPI_Resolution
	dpi.lane								= E_IM_MIPI_ACTIVE_DATA_LANE_1;
	dpi.vcid								= 0;
	dpi.color_coding						= D_IM_MIPI_COLOR_CODING_16BIT_CONFIG1;
	dpi.loosely18_en						= 1;
	dpi.cfg_pol.bit.dataen_active_low		= 1;
	dpi.cfg_pol.bit.vsync_active_low		= 1;
	dpi.cfg_pol.bit.hsync_active_low		= 1;
	dpi.cfg_pol.bit.shutd_active_low		= 1;
	dpi.cfg_pol.bit.colorm_acvite_low		= 1;
	dpi.mode_cfg							= E_IM_MIPI_MODE_CFG_VIDEO
	dpi.vid_mode_cfg.bit.vid_mode_type		= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.lp_vsa_en			= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.lp_vbp_en			= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.lp_vfp_en			= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.lp_vact_en			= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.lp_hbp_en			= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.lp_hfp_en			= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.lp_cmd_en			= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.vpg_en				= D_IM_MIPI_ENABLE_ON;
	dpi.vid_mode_cfg.bit.vpg_mode			= 1;
	dpi.vid_mode_cfg.bit.vpg_orientation	= 1;
	dpi.vid_pkt_size						= 1;
	dpi.vid_num_chunks						= 0;
	dpi.vid_null_size						= 1;
	dpi.vid_hline_time						= 1;
	dpi.vid_hsa_time						= 1;
	dpi.vid_hbp_time						= 1;
	dpi.vid_vsa_lines						= 1;
	dpi.vid_vbp_lines						= 1;
	dpi.vid_vfp_lines						= 1;
	dpi.vid_vactive_lines					= 1;
	dpi.cmd_mode_cfg.bit.gen_sw_0p_tx		= 0;
	dpi.cmd_mode_cfg.bit.gen_sw_1p_tx		= 0;
	dpi.cmd_mode_cfg.bit.gen_sw_2p_tx		= 0;
	dpi.cmd_mode_cfg.bit.gen_lw_tx			= 0;
	dpi.cmd_mode_cfg.bit.dcs_sw_0p_tx		= 0;
	dpi.cmd_mode_cfg.bit.dcs_se_1p_tx		= 0;
	dpi.cmd_mode_cfg.bit.dcs_lw_tx			= 0;
	
	ret = Im_MIPI_Set_DPI_Resolution(&dpi);
	if (ret != D_DDIM_OK){
		// NG.
		return;
	}
	
	ret = Im_MIPI_Update_Dpi();
	if (ret != D_DDIM_OK){
		// NG.
		return;
	}
	
	// Transmission Configuration
	config.clkmgr_cfg.bit.tx_esc_clk_division		= 0;
	config.clkmgr_cfg.bit.to_clk_division			= 0;
	config.dpi_lp_cmd_tim.bit.invact_lpcmd_time		= 0;
	config.dpi_lp_cmd_tim.bit.outvact_lpcmd_time	= 0;
	config.timer_count								= 0xFFFF;
	config.phy_tmr_cfg.bit.phy_lp2hs_time			= 0;
	config.phy_tmr_cfg.bit.phy_hs2lp_time			= 0;
	
	ret = Im_MIPI_Set_Transmission_Configuration(&config);
	if (ret != D_DDIM_OK){
		// NG.
		return;
	}
}
@endcode
@}*//* end of dd_hdmi*/
