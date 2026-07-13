/**
 * @file		im_hdmi.h
 * @brief		HDMI Transmitter driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
/**
@weakgroup im_hdmi
@{
@}
*/

/**
@addtogroup im_hdmi
@{
	- @ref Hdmi_Sample_Basic
*/

#ifndef _IM_HDMI_H_
#define _IM_HDMI_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

//---------------------- driver  section -------------------------------
// Return value of ARM watchdog timer processing result.
#define D_IM_HDMI_INPUT_PARAM_ERROR	(D_IM_HDMI | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define D_IM_HDMI_NO_RECEIVE		(D_IM_HDMI | D_DDIM_NO_RECEIVE)			/**< No receive */
#define D_IM_HDMI_TIMEOUT			(D_IM_HDMI | D_DDIM_TIMEOUT)			/**< Time out */
#define D_IM_HDMI_NG				(D_IM_HDMI | D_DDIM_SYSTEM_ERROR)		/**< System error */

// CSC Matrix number.
#define D_IM_HDMI_CSC_CONF_COLUMN_NUM	(3)			/**< CSC Matrix columns:A-C.	*/
#define D_IM_HDMI_CSC_CONF_ROW_NUM		(4)			/**< CSC Matrix rows:1-4.		*/

// Frame Composer SPD packet Data number.
#define D_IM_HDMI_FC_SPD_VENDER_NAME_NUM	(8)		/**< Vendor Name:8.		*/
#define D_IM_HDMI_FC_SPD_PRO_NAME_NUM		(16)	/**< Product Name:16.	*/

// Frame Composer VSD packet Data number.
#define D_IM_HDMI_FC_VSD_IEEE_DATA_NUM	(3)			/**< IEEE:3.		*/
#define D_IM_HDMI_FC_VSD_PAYLOAD_NUM	(24)		/**< Payload:24.	*/

// General-purpose definition of register value (Effectiveness selection)
#define D_IM_HDMI_ENABLE_OFF			(0)			/**< OFF	*/
#define D_IM_HDMI_ENABLE_ON				(1)			/**< ON		*/

// definition of register value (Active Low/High)
#define D_IM_HDMI_ACTIVE_LOW			(0)			/**< Acvtice Low	*/
#define D_IM_HDMI_ACTIVE_HIGH			(1)			/**< Acvtice High	*/

// definition of register value (Video Mode)
#define D_IM_HDMI_VIDEO_MODE_HDMI		(0x20)		/**< HDMI Video Mode	*/
#define D_IM_HDMI_VIDEO_MODE_3D			(0x40)		/**< 3D Video Mode		*/

// fc_invidconf Register - in_I_P (Input video mode)
#define D_IM_HDMI_IN_I_P_PROGRESSIVE	(0)			/**< Progressive	*/
#define D_IM_HDMI_IN_I_P_INTERLACED		(1)			/**< Interlaced		*/

// fc_invidconf Register - DVI_modez (DVI mode setected)
#define D_IM_HDMI_DVI_MODEZ_DVI			(0)			/**< DVI mode selected	*/
#define D_IM_HDMI_DVI_MODEZ_HDMI		(1)			/**< HDMI mode selected	*/

// mc_phyrstz Register - phyrstz (HDMI Source PHY active high reset control)
#define D_IM_HDMI_PHYRSTZ_CANCEL		(0)			/**< Reset cancel.	*/
#define D_IM_HDMI_PHYRSTZ_RESET			(1)			/**< Reset.			*/

#define D_IM_HDMI_PHY_REG_NUM			(8)			/**< PHY Register number	*/

/* HDMI Video Output format */
#define D_IM_HDMI_OUT_RGB				(0x00)		/**< RGB */
#define D_IM_HDMI_OUT_YCC420			(0x10)		/**< YCbCr420 */
#define D_IM_HDMI_OUT_YCC422			(0x20)		/**< YCbCr422 */
#define D_IM_HDMI_OUT_YCC444			(0x40)		/**< YCbCr444 */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Video Input mapping (color space/color depth) */
typedef enum{
	E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT			= 0x01,		/**< RGB 4:4:4 8 bits				*/
	E_IM_HDMI_VIDEO_MAPPING_RGB444_10BIT		= 0x03,		/**< RGB 4:4:4/10 bits				*/
	E_IM_HDMI_VIDEO_MAPPING_RGB444_12BIT		= 0x05,		/**< RGB 4:4:4/12 bits				*/
	E_IM_HDMI_VIDEO_MAPPING_RGB444_16BIT		= 0x07,		/**< RGB 4:4:4/16 bits				*/
	E_IM_HDMI_VIDEO_MAPPING_YCC444_420_8BIT		= 0x09,		/**< YCbCr 4:4:4 or 4:2:0/8 bits	*/
	E_IM_HDMI_VIDEO_MAPPING_YCC444_420_10BIT	= 0x0B,		/**< YCbCr 4:4:4 or 4:2:0/10 bits	*/
	E_IM_HDMI_VIDEO_MAPPING_YCC444_420_12BIT	= 0x0D,		/**< YCbCr 4:4:4 or 4:2:0/12 bits	*/
	E_IM_HDMI_VIDEO_MAPPING_YCC444_420_16BIT	= 0x0F,		/**< YCbCr 4:4:4 or 4:2:0/16 bits	*/
	E_IM_HDMI_VIDEO_MAPPING_YCC422_12BIT		= 0x12,		/**< YCbCr 4:2:2/12 bits			*/
	E_IM_HDMI_VIDEO_MAPPING_YCC422_10BIT		= 0x14,		/**< YCbCr 4:2:2/10 bits			*/
	E_IM_HDMI_VIDEO_MAPPING_YCC422_8BIT			= 0x16,		/**< YCbCr 4:2:2/8 bits				*/
	E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_8BIT		= 0x17,		/**< YCbCr 4:4:4 (IPI)/8 bits		*/
	E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_10BIT	= 0x18,		/**< YCbCr 4:4:4 (IPI)/10 bits		*/
	E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_12BIT	= 0x19,		/**< YCbCr 4:4:4 (IPI)/12 bits		*/
	E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_16BIT	= 0x1A,		/**< YCbCr 4:4:4 (IPI)/16 bits		*/
	E_IM_HDMI_VIDEO_MAPPING_YCC422_IPI_12BIT	= 0x1B,		/**< YCbCr 4:2:2 (IPI)/12 bits		*/
	E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_8BIT		= 0x1C,		/**< YCbCr 4:2:0 (IPI)/8 bits		*/
	E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_10BIT	= 0x1D,		/**< YCbCr 4:2:0 (IPI)/10 bits		*/
	E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_12BIT	= 0x1E,		/**< YCbCr 4:2:0 (IPI)/12 bits		*/
	E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_16BIT	= 0x1F		/**< YCbCr 4:2:0 (IPI)/16 bits		*/
} E_IM_HDMI_VIDEO_MAPPING;

/** Color depth */
typedef enum{
	E_IM_HDMI_COLOR_DEPTH_8BIT	= 4,	/**< 8-bit packing mode(24 bits per pixel video).	*/
	E_IM_HDMI_COLOR_DEPTH_10BIT	= 5,	/**< 10-bit packing mode(30 bits per pixel video).	*/
	E_IM_HDMI_COLOR_DEPTH_12BIT	= 6,	/**< 12-bit packing mode(36 bits per pixel video).	*/
	E_IM_HDMI_COLOR_DEPTH_16BIT	= 7		/**< 16-bit packing mode(48 bits per pixel video).	*/
} E_IM_HDMI_COLOR_DEPTH;

/** Interrupt register */
typedef enum{
	E_IM_HDMI_INT_REG_FC_STAT0,				/**< ih_fc_stat0.		*/
	E_IM_HDMI_INT_REG_FC_STAT1,				/**< ih_fc_stat1.		*/
	E_IM_HDMI_INT_REG_FC_STAT2,				/**< ih_fc_stat2.		*/
	E_IM_HDMI_INT_REG_AS_STAT0,				/**< ih_as_stat0.		*/
	E_IM_HDMI_INT_REG_PHY_STAT0,			/**< ih_phy_stat0.		*/
	E_IM_HDMI_INT_REG_I2CM_STAT0,			/**< ih_i2cm_stat0.		*/
	E_IM_HDMI_INT_REG_CEC_STAT0,			/**< ih_cec_stat0.		*/
	E_IM_HDMI_INT_REG_VP_STAT0,				/**< ih_vp_stat0.		*/
	E_IM_HDMI_INT_REG_I2CMPHY_STAT0			/**< ih_i2cmphy_stat0.	*/
} E_IM_HDMI_INT_REG;

/** Interrupt type */
typedef enum{
	E_IM_HDMI_INT_TYPE_FC_NULL	= 0,		/**< ih_fc_stat0 Register - NULL.					*/
	E_IM_HDMI_INT_TYPE_FC_ACR,				/**< ih_fc_stat0 Register - ACR.					*/
	E_IM_HDMI_INT_TYPE_FC_AUDS,				/**< ih_fc_stat0 Register - AUDS.					*/
	E_IM_HDMI_INT_TYPE_FC_NVBI,				/**< ih_fc_stat0 Register - NVBI.					*/
	E_IM_HDMI_INT_TYPE_FC_MAS,				/**< ih_fc_stat0 Register - MAS.					*/
	E_IM_HDMI_INT_TYPE_FC_HBR,				/**< ih_fc_stat0 Register - HBR.					*/
	E_IM_HDMI_INT_TYPE_FC_ACP,				/**< ih_fc_stat0 Register - ACP.					*/
	E_IM_HDMI_INT_TYPE_FC_AUDI,				/**< ih_fc_stat0 Register - AUDI.					*/
	E_IM_HDMI_INT_TYPE_FC_GCP,				/**< ih_fc_stat1 Register - GCP.					*/
	E_IM_HDMI_INT_TYPE_FC_AVI,				/**< ih_fc_stat1 Register - AVI.					*/
	E_IM_HDMI_INT_TYPE_FC_AMP,				/**< ih_fc_stat1 Register - AMP.					*/
	E_IM_HDMI_INT_TYPE_FC_SPD,				/**< ih_fc_stat1 Register - SPD.					*/
	E_IM_HDMI_INT_TYPE_FC_VSD,				/**< ih_fc_stat1 Register - VSD.					*/
	E_IM_HDMI_INT_TYPE_FC_ISCR2,			/**< ih_fc_stat1 Register - ISCR2.					*/
	E_IM_HDMI_INT_TYPE_FC_ISCR1,			/**< ih_fc_stat1 Register - ISCR1.					*/
	E_IM_HDMI_INT_TYPE_FC_GMD,				/**< ih_fc_stat1 Register - GMD.					*/
	E_IM_HDMI_INT_TYPE_FC_HP_OF,			/**< ih_fc_stat2 Register - HighPriority_overflow.	*/
	E_IM_HDMI_INT_TYPE_FC_LP_OF,			/**< ih_fc_stat2 Register - LowPriority_overflow.	*/
	E_IM_HDMI_INT_TYPE_AS_OF,				/**< ih_as_stat0 Register - Aud_fifo_overflow.		*/
	E_IM_HDMI_INT_TYPE_AS_UF,				/**< ih_as_stat0 Register - Aud_fifo_underflow.		*/
	E_IM_HDMI_INT_TYPE_AS_OR,				/**< ih_as_stat0 Register - fifo_overrun.			*/
	E_IM_HDMI_INT_TYPE_PHY_HDP,				/**< ih_phy_stat0 Register - HDP.					*/
	E_IM_HDMI_INT_TYPE_PHY_TX_PHY_LOCK,		/**< ih_phy_stat0 Register - TX_PHY_LOCK.			*/
	E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_0,		/**< ih_phy_stat0 Register - RX_SENSE_0.			*/
	E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_1,		/**< ih_phy_stat0 Register - RX_SENSE_1.			*/
	E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_2,		/**< ih_phy_stat0 Register - RX_SENSE_2.			*/
	E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_3,		/**< ih_phy_stat0 Register - RX_SENSE_3.			*/
	E_IM_HDMI_INT_TYPE_I2CM_M_ERR,			/**< ih_i2cm_stat0 Register - I2Cmastererror.		*/
	E_IM_HDMI_INT_TYPE_I2CM_M_DONE,			/**< ih_i2cm_stat0 Register - I2Cmasterdone.		*/
	E_IM_HDMI_INT_TYPE_I2CM_SDSC,			/**< ih_i2cm_stat0 Register - scdc_readreq.			*/
	E_IM_HDMI_INT_TYPE_CEC_DONE,			/**< ih_cec_stat0 Register - DONE.					*/
	E_IM_HDMI_INT_TYPE_CEC_EOM,				/**< ih_cec_stat0 Register - EOM.					*/
	E_IM_HDMI_INT_TYPE_CEC_NACK,			/**< ih_cec_stat0 Register - NACK.					*/
	E_IM_HDMI_INT_TYPE_CEC_ARB_LOST,		/**< ih_cec_stat0 Register - ARB_LOST.				*/
	E_IM_HDMI_INT_TYPE_CEC_ERR_INIT,		/**< ih_cec_stat0 Register - ERROR_INITIATOR.		*/
	E_IM_HDMI_INT_TYPE_CEC_ERR_FOL,			/**< ih_cec_stat0 Register - ERROR_FOLLOW.			*/
	E_IM_HDMI_INT_TYPE_CEC_WAKEUP,			/**< ih_cec_stat0 Register - WAKEUP.				*/
	E_IM_HDMI_INT_TYPE_VP_E_BYP,			/**< ih_vp_stat0 Register - fifoemptybyp.			*/
	E_IM_HDMI_INT_TYPE_VP_F_BYP,			/**< ih_vp_stat0 Register - fifofullbyp.			*/
	E_IM_HDMI_INT_TYPE_VP_E_REMAP,			/**< ih_vp_stat0 Register - fifoemptyremap.			*/
	E_IM_HDMI_INT_TYPE_VP_F_REMAP,			/**< ih_vp_stat0 Register - fifofullremap.			*/
	E_IM_HDMI_INT_TYPE_VP_E_PP,				/**< ih_vp_stat0 Register - fifoemptypp.			*/
	E_IM_HDMI_INT_TYPE_VP_F_PP,				/**< ih_vp_stat0 Register - fifofullpp.				*/
	E_IM_HDMI_INT_TYPE_VP_E_REPET,			/**< ih_vp_stat0 Register - fifoemptyrepet.			*/
	E_IM_HDMI_INT_TYPE_VP_F_REPET,			/**< ih_vp_stat0 Register - fifofullrepet.			*/
	E_IM_HDMI_INT_TYPE_I2CMPHY_ERR,			/**< ih_i2cmphy_stat0 Register - I2Cmphyerror.		*/
	E_IM_HDMI_INT_TYPE_I2CMPHY_DONE,		/**< ih_i2cmphy_stat0 Register - I2Cmphydone.		*/
	E_IM_HDMI_INT_TYPE_MAX					/**< Interrupt type maximum value.					*/
} E_IM_HDMI_INT_TYPE;

/** ih_fc_stat0 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_FC_STAT0_NULL	= 0x01,		/**< Successful transmission of Null packet.				*/
	E_IM_HDMI_IH_FC_STAT0_ACR	= 0x02,		/**< Successful transmission of ACR packet.					*/
	E_IM_HDMI_IH_FC_STAT0_AUDS	= 0x04,		/**< Successful transmission of Audio Sample packet.		*/
	E_IM_HDMI_IH_FC_STAT0_NVBI	= 0x08,		/**< successful transmission of NTSC VBI packet.			*/
	E_IM_HDMI_IH_FC_STAT0_MAS	= 0x10,		/**< successful transmission of MultiStream Audio packet.	*/
	E_IM_HDMI_IH_FC_STAT0_HBR	= 0x20,		/**< successful transmission of Audio HBR packet.			*/
	E_IM_HDMI_IH_FC_STAT0_ACP	= 0x40,		/**< successful transmission of ACP packet.					*/
	E_IM_HDMI_IH_FC_STAT0_AUDI	= 0x80		/**< successful transmission of Audio InfoFrame packet.		*/
} E_IM_HDMI_IH_FC_STAT0;

/** ih_fc_stat1 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_FC_STAT1_GCP	= 0x01,		/**< Successful transmission of General Control Packet.		*/
	E_IM_HDMI_IH_FC_STAT1_AVI	= 0x02,		/**< Successful transmission of AVI InfoFrame packet.		*/
	E_IM_HDMI_IH_FC_STAT1_AMP	= 0x04,		/**< Successful transmission of Audio Metadata packet.		*/
	E_IM_HDMI_IH_FC_STAT1_SPD	= 0x08,		/**< successful transmission of SPD InfoFrame packet.		*/
	E_IM_HDMI_IH_FC_STAT1_VSD	= 0x10,		/**< successful transmission of VSD InfoFrame packet.		*/
	E_IM_HDMI_IH_FC_STAT1_ISCR2	= 0x20,		/**< successful transmission of ISCR2 packet.				*/
	E_IM_HDMI_IH_FC_STAT1_ISCR1	= 0x40,		/**< successful transmission of ISCR1 packet.				*/
	E_IM_HDMI_IH_FC_STAT1_GMD	= 0x80		/**< successful transmission of Gamut metadata packet.		*/
} E_IM_HDMI_IH_FC_STAT1;

/** ih_fc_stat2 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_FC_STAT2_HP_OF	= 0x01,		/**< High priority packet queue descriptor overflow indication.	*/
	E_IM_HDMI_IH_FC_STAT2_LP_OF	= 0x02		/**< Low priority packet queue descriptor overflow indication.	*/
} E_IM_HDMI_IH_FC_STAT2;

/** ih_as_stat0 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_AS_STAT0_OF	= 0x01,		/**< Audio Sampler audio FIFO full indication.	*/
	E_IM_HDMI_IH_AS_STAT0_UF	= 0x02,		/**< Audio Sampler audio FIFO empty indication.	*/
	E_IM_HDMI_IH_AS_STAT0_OR	= 0x08		/**< Indicates an overrun on the audio FIFO.	*/
} E_IM_HDMI_IH_AS_STAT0;

/** ih_phy_stat0 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_PHY_STAT0_HDP			= 0x01,		/**< HDMI Hot Plug Detect indication.			*/
	E_IM_HDMI_IH_PHY_STAT0_TX_PHY_LOCK	= 0x02,		/**< TX PHY PLL lock indication.				*/
	E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_0	= 0x04,		/**< TX PHY RX_SENSE indication for driver 0.	*/
	E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_1	= 0x08,		/**< TX PHY RX_SENSE indication for driver 1.	*/
	E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_2	= 0x10,		/**< TX PHY RX_SENSE indication for driver 2.	*/
	E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_3	= 0x20		/**< TX PHY RX_SENSE indication for driver 3.	*/
} E_IM_HDMI_IH_PHY_STAT0;

/** ih_i2cm_stat0 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_I2CM_STAT0_MASTER_ERR	= 0x01,		/**< I2C Master error indication.				*/
	E_IM_HDMI_IH_I2CM_STAT0_MASTER_DONE	= 0x02,		/**< I2C Master done indication.				*/
	E_IM_HDMI_IH_I2CM_STAT0_SDSC		= 0x04		/**< I2C Master SCDC read request indication.	*/
} E_IM_HDMI_IH_I2CM_STAT0;

/** ih_cec_stat0 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_CEC_STAT0_DONE				= 0x01,		/**< CEC Done Indication.				*/
	E_IM_HDMI_IH_CEC_STAT0_EOM				= 0x02,		/**< CEC End of Message Indication.		*/
	E_IM_HDMI_IH_CEC_STAT0_NACK				= 0x04,		/**< CEC Not Acknowledge indication.	*/
	E_IM_HDMI_IH_CEC_STAT0_ARB_LOST			= 0x08,		/**< CEC Arbitration Lost indication.	*/
	E_IM_HDMI_IH_CEC_STAT0_ERR_INITIATOR	= 0x10,		/**< CEC Error Initiator indication.	*/
	E_IM_HDMI_IH_CEC_STAT0_ERR_FOLLOW		= 0x20,		/**< CEC Error Follow indication.		*/
	E_IM_HDMI_IH_CEC_STAT0_WAKEUP			= 0x40		/**< CEC Wake-up indication.			*/
} E_IM_HDMI_IH_CEC_STAT0;

/** ih_vp_stat0 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_VP_STAT0_EMPTY_BYP		= 0x01,		/**< 8 bit bypass FIFO empty interrupt.				*/
	E_IM_HDMI_IH_VP_STAT0_FULL_BYP		= 0x02,		/**< 8 bit bypass FIFO full interrupt.				*/
	E_IM_HDMI_IH_VP_STAT0_EMPTY_REMAP	= 0x04,		/**< Pixel YCC422 re-mapper FIFO empty interrupt.	*/
	E_IM_HDMI_IH_VP_STAT0_FULL_REMAP	= 0x08,		/**< Pixel YCC422 re-mapper FIFO full interrupt.	*/
	E_IM_HDMI_IH_VP_STAT0_EMPTY_PP		= 0x10,		/**< Pixel packing FIFO empty interrupt.			*/
	E_IM_HDMI_IH_VP_STAT0_FULL_PP		= 0x20,		/**< Pixel packing FIFO full interrupt.				*/
	E_IM_HDMI_IH_VP_STAT0_EMPTY_REPET	= 0x40,		/**< Pixel repeater FIFO empty interrupt.			*/
	E_IM_HDMI_IH_VP_STAT0_FULL_REPET	= 0x80		/**< Pixel repeater FIFO full interrupt.			*/
} E_IM_HDMI_IH_VP_STAT0;

/** ih_i2cmphy_stat0 Register - Interrupt type. */
typedef enum{
	E_IM_HDMI_IH_I2CMPHY_STAT0_MASTER_ERR	= 0x01,		/**< I2C Master PHY error indication.	*/
	E_IM_HDMI_IH_I2CMPHY_STAT0_MASTER_DONE	= 0x02		/**< I2C Master PHY done indication.	*/
} E_IM_HDMI_IH_I2CMPHY_STAT0;

/** vp_remap Register - ycc422_size (YCC 422 remap input video size) */
typedef enum{
	E_IM_HDMI_VP_REMAP_16BIT	= 0,	/**< YCC 422 16-bit input video (8 bits per component).		*/
	E_IM_HDMI_VP_REMAP_20BIT	= 1,	/**< YCC 422 20-bit input video (10 bits per component).	*/
	E_IM_HDMI_VP_REMAP_24BIT	= 2		/**< YCC 422 24-bit input video (12 bits per component).	*/
} E_IM_HDMI_VP_REMAP;

/** vp_conf Register - output_selector (Video Packetizer output selection) */
typedef enum{
	E_IM_HDMI_OUTPUT_SELECTOR_PIXEL_PACKING		= 0,	/**< Data from pixel packing block.	*/
	E_IM_HDMI_OUTPUT_SELECTOR_YCC422_REMAP,				/**< Data from YCC 422 remap block.	*/
	E_IM_HDMI_OUTPUT_SELECTOR_8BIT_BYPASS				/**< Data from 8-bit bypass block.	*/
} E_IM_HDMI_OUTPUT_SELECTOR;

/** vp_conf Register - bypass_select (bypass select) */
typedef enum{
	E_IM_HDMI_BYPASS_SELECT_PIXEL_REPEATER		= 0,	/**< Data from pixel repeater block.			*/
	E_IM_HDMI_BYPASS_SELECT_VIDEO_PACKETIZER			/**< Data from input of Video Packetizer block.	*/
} E_IM_HDMI_BYPASS_SELECT;

/** aud_conf0 Register - i2s_select (Select I2S interface) */
typedef enum{
	E_IM_HDMI_I2S_SELECT_S_PDIF	= 0,	/**< Selects SPDIF interface		*/
	E_IM_HDMI_I2S_SELECT_I2S			/**< Selects I2S Audio Interface	*/
} E_IM_HDMI_I2S_SELECT;

/** aud_conf1 Register - I2S_mode (I2S input data mode) */
typedef enum{
	E_IM_HDMI_I2S_MODE_STANDARD			= 0,	/**< Standard I2S mode			*/
	E_IM_HDMI_I2S_MODE_RIGHT_JUSTIFIED,			/**< Right-justified I2S mode	*/
	E_IM_HDMI_I2S_MODE_LEFT_JUSTIFIED			/**< Left-justified I2S mode	*/
} E_IM_HDMI_I2S_MODE;

/** aud_cts3 Register - CTS_manual (Select CTS Automatic/Manual) */
typedef enum{
	E_IM_HDMI_CTS_MANUAL_AUTO	= 0,	/**< 00:Divides by 1		*/
	E_IM_HDMI_CTS_MANUAL_MANUAL			/**< Manual			*/
} E_IM_HDMI_CTS_MANUAL;

/** aud_inputclkfs Register - ifsfactor (Fs factor configuration) */
typedef enum{
	E_IM_HDMI_AUD_IFSFACTOR_128XFS	= 0,	/**< 128xFs		*/
	E_IM_HDMI_AUD_IFSFACTOR_256XFS	= 1,	/**< 256xFs		*/
	E_IM_HDMI_AUD_IFSFACTOR_512XFS	= 2,	/**< 512xFs		*/
	E_IM_HDMI_AUD_IFSFACTOR_64XFS	= 4		/**< 64xFs		*/
} E_IM_HDMI_AUD_IFSFACTOR;


/** i2cm_operation Register */
typedef enum{
	E_IM_HDMI_I2CM_OPERATION_RD			= 0x01,		/**< Single byte read operation request.					*/
	E_IM_HDMI_I2CM_OPERATION_RD_EXT		= 0x02,		/**< Extended data read operation(E-DDC read operation).	*/
	E_IM_HDMI_I2CM_OPERATION_RD8		= 0x04,		/**< Sequential read operation request.						*/
	E_IM_HDMI_I2CM_OPERATION_RD8_EXT	= 0x08,		/**< Extended sequential read operation request.			*/
	E_IM_HDMI_I2CM_OPERATION_WR			= 0x10		/**< Single byte write operation request.					*/
} E_IM_HDMI_I2CM_OPERATION;

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/** structure of aud_conf0 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	I2S_in_en			:4;		/**< I2S_in_en : enable I2C data.<br>
												 bit d[0]:I2Sdata[0] ~ d[3]:I2Sdata[3].					*/
		UCHAR						:1;		/**< Reserve												*/
		UCHAR	i2s_select			:1;		/**< i2s_select : Interface selection I2S/SPDIF.<br>
												 <ul><li>@ref E_IM_HDMI_I2S_SELECT_S_PDIF
													 <li>@ref E_IM_HDMI_I2S_SELECT_I2S</ul>				*/
		UCHAR						:1;		/**< Reserve												*/
		UCHAR	sw_audio_fifo_rst	:1;		/**< sw_audio_fifo_rst : Audio FIFOs software reset.<br>
												 Writing 0b: no action taken.<br>
												 Writing 1b: Resets all audio FIFOs.<br>
												 Reading from this register always returns 0b.			*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_AUD_CONF0;

/** structure of aud_conf1 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	I2S_width			:5;		/**< I2S_width : I2S input data width.<br>
												 Value range : 16~24.									*/
		UCHAR	I2S_mode			:3;		/**< I2S_mode : I2S input data mode.<br>
												 <ul><li>@ref E_IM_HDMI_I2S_MODE_STANDARD
													 <li>@ref E_IM_HDMI_I2S_MODE_RIGHT_JUSTIFIED
													 <li>@ref E_IM_HDMI_I2S_MODE_LEFT_JUSTIFIED</ul>	*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_AUD_CONF1;

/** structure of aud_conf2 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	HBR					:1;		/**< HBR : I2S HBR Mode Enable.<br>
												 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
													 <li>@ref D_IM_HDMI_ENABLE_ON</ul>	*/
		UCHAR	NLPCM				:1;		/**< NLPCM : I2S NLPCM Mode Enable.<br>
												 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
													 <li>@ref D_IM_HDMI_ENABLE_ON</ul>	*/
		UCHAR						:6;		/**< Reserve								*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_AUD_CONF2;

/** structure of aud_spdif1 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	spdif_width			:5;		/**< spdif_width : SPDIF input data width.<br>
												 Value range : 16~24.								*/
		UCHAR						:1;		/**< Reserve											*/
		UCHAR	spdif_hbr_mode		:1;		/**< spdif_hbr_mode : SPDIF HBR mode.<br>
												 Value 0: Transmitted in the HDMI HBR packets.<br>
												 Value 1: Transmitted in the HDMI AUDS packets.		*/
		UCHAR	setnlpcm			:1;		/**< setnlpcm : Select Non-Linear/Linear PCM mode.<br>
												 Value 0: Non-Linear PCM mode.<br>
												 Value 1: Linear PCM mode.							*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_AUD_SPDIF1;

/** structure of fc_audsconf register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	aud_packet_layout	:1;		/**< aud_packet_layout : Set the audio packet layout.<br>
												 Value 0:layout 0, 1:layout 1.							*/
		UCHAR						:3;		/**< Reserve												*/
		UCHAR	aud_packet_sampflt	:4;		/**< Set the audio packet sample flat value.<br>
												 Value range : 0~15.									*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_AUDSCONF;

/** structure of fc_invidconf register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	in_I_P				:1;		/**< in_I_P : Input video mode.<br>
												 <ul><li>@ref D_IM_HDMI_IN_I_P_PROGRESSIVE
													 <li>@ref D_IM_HDMI_IN_I_P_INTERLACED</ul>	*/
		UCHAR	r_v_blank_in_osc	:1;		/**< r_v_blank_in_osc : Used for CEA861-D modes with fractional Vblank.<br>
												 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
													 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>								*/
		UCHAR						:1;		/**< Reserve																*/
		UCHAR	DVI_modez			:1;		/**< DVI_modez : DVI mode selected.<br>
												 <ul><li>@ref D_IM_HDMI_DVI_MODEZ_DVI
													 <li>@ref D_IM_HDMI_DVI_MODEZ_HDMI</ul>								*/
		UCHAR	de_in_polarity		:1;		/**< de_in_polarity : Data enable input polarity.<br>
												 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
													 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>								*/
		UCHAR	hsync_in_polarity	:1;		/**< hsync_in_polarity : Hsync input polarity.<br>
												 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
													 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>								*/
		UCHAR	vsync_in_polarity	:1;		/**< vsync_in_polarity : Vsync input polarity.<br>
												 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
													 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>								*/
		UCHAR	HDCP_keepout		:1;		/**< HDCP_keepout : Start/stop HDCP keepout window generation.<br>
												 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
													 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>								*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_INVIDCONF;

/** structure of fc_aviconf3 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	CN					:2;		/**< CN : IT content type according to CEA the specification.<br>
												 Value range : 0~3.														*/
		UCHAR	YQ					:2;		/**< YQ : YCC Quantization range according to the CEA specification.<br>
												 Value range : 0~3.														*/
		UCHAR						:4;		/**< Reserve																*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_AVICONF3;

/** structure of fc_aviconf0 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	rgc_ycc_indication		:2;		/**< rgc_ycc_indication : Y1,Y0 RGB or YCC indicator.<br>
													 Value 0:RGB, 1:YCC422 2:YCC444 3:YCC420.						*/
		UCHAR	bar_information			:2;		/**< bar_information : Bar information data valid.<br>
													 Value range : 0~3.												*/
		UCHAR	scan_information		:2;		/**< scan_information : Scan information.<br>
													 Value range : 0~3.												*/
		UCHAR	active_format_present	:1;		/**< active_format_present : Active format present.<br>
													 Value range : 0~1.												*/
		UCHAR	rgc_ycc_indication_2	:1;		/**< rgc_ycc_indication_2 : Y2, Bit 2 of rgc_ycc_indication.<br>
													 Value range : 0~1.												*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_AVICONF0;

/** structure of fc_aviconf1 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	active_aspect_ratio		:4;		/**< active_aspect_ratio : Active aspect ratio.<br>
													 Value range : 0~15.								*/
		UCHAR	picture_aspect_ratio	:2;		/**< picture_aspect_ratio : Picture aspect ratio.<br>
													 Value range : 0~3.									*/
		UCHAR	Colorimetry				:2;		/**< Colorimetry : Colorimetry.<br>
													 Value range : 0~3.									*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_AVICONF1;

/** structure of fc_aviconf2 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	non_uniform_picture_scaling	:2;		/**< non_uniform_picture_scaling : Non-uniform picture scaling.<br>
														 Value range : 0~3.													*/
		UCHAR	quantization_range			:2;		/**< quantization_range : Quantization range.<br>
														 Value range : 0~3.													*/
		UCHAR	extended_colorimetry		:3;		/**< extended_colorimetry : Extended colorimetry.<br>
														 Value range : 0~7.													*/
		UCHAR	it_content					:1;		/**< it_content : IT content.<br>
														 Value range : 0~1.													*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_AVICONF2;

/** structure of fc_audiconf0 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	CT							:4;		/**< CT : Coding Type.<br>
														 Value range : 0~31.		*/
		UCHAR	CC							:3;		/**< CC : Channel count.<br>
														 Value range : 0~7.			*/
		UCHAR								:1;		/**< Reserve					*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_AUDICONF0;

/** structure of fc_audiconf1 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	SF							:3;		/**< SF : Sampling frequency.<br>
														 Value range : 0~7.				*/
		UCHAR								:1;		/**< Reserve						*/
		UCHAR	SS							:2;		/**< SS : Sampling size.<br>
														 Value range : 0~3.				*/
		UCHAR								:2;		/**< Reserve						*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_AUDICONF1;

/** structure of fc_audiconf3 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	LSV							:4;		/**< LSV : Level shift value (for down mixing).<br>
														 Value range : 0x0~0xF.							*/
		UCHAR	DM_INH						:1;		/**< DM_INH : Down mix enable.<br>
														 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
															 <li>@ref D_IM_HDMI_ENABLE_ON</ul>			*/
		UCHAR	LFEPBL						:2;		/**< LFEPBL : LFE playback information.<br>
														 Value range : 0~3.								*/
		UCHAR								:1;		/**< Reserve										*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_AUDICONF3;

/** structure of fc_prconf register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	output_pr_factor			:4;		/**< output_pr_factor : Outout video pixel repetition.<br>
														 Value:<br>
														 0:No action. Not used.<br>
														 1:No pixel repetition (pixel sent only once).<br>
														 2~10:Pixel will be sent a specified number of times.<br>
														 other:Reserved. Not used.								 	*/
		UCHAR	incoming_pr_factor			:4;		/**< incoming_pr_factor : Input video pixel repetition.<br>
														 Value:<br>
														 0:No action. Not used.<br>
														 1:No pixel repetition (pixel sent only once).<br>
														 2~10:Pixel will be sent a specified number of times.<br>
														 other:Reserved. Not used.								 	*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_PRCONF;

/** structure of fc_actspc_hdlr_cfg register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	actspc_hdlr_en				:1;		/**< actspc_hdlr_en : active space value mode enable.<br>
														 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
															 <li>@ref D_IM_HDMI_ENABLE_ON</ul>				*/
		UCHAR	actspc_hdlr_tgl				:1;		/**< actspc_hdlr_tgl : Active space oscillating.<br>
														 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
															 <li>@ref D_IM_HDMI_ENABLE_ON</ul>				*/
		UCHAR								:6;		/**< Reserve											*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_FC_ACTSPC_HDLR_CFG;

/** structure of tx_invid0 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	video_mapping			:5;		/**< video_mapping : Video Input mapping (color space/color depth).<br>
													 <ul><li>@ref E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_RGB444_10BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_RGB444_12BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_RGB444_16BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_420_8BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_420_10BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_420_12BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_420_16BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC422_12BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC422_10BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC422_8BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_8BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_10BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_12BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_16BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC422_IPI_12BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_8BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_10BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_12BIT
														 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_16BIT</ul><br>			*/
		UCHAR							:2;		/**< Reserve																*/
		UCHAR	internal_de_generator	:1;		/**< internal_de_generator : Internal data enable (DE) generator enable.<br>
													 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
														 <li>@ref D_IM_HDMI_ENABLE_ON</ul>									*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_TX_INVID0;

/** structure of vp_pr_cd register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	desired_pr_factor	:4;		/**< desired_pr_factor : Desired pixel repetition factor configuration.<br>
												 Value:<br>
												 0:No pixel repetition (pixel sent only once).<br>
												 1:Pixel sent 2 times (pixel repeated once) ~ 9:Pixel sent 10 times.<br>
												 other:Reserved. Not used.											 	*/
		UCHAR	color_depth			:4;		/**< color_depth : Color depth.<br>
												 <ul><li>@ref E_IM_HDMI_COLOR_DEPTH_8BIT
													 <li>@ref E_IM_HDMI_COLOR_DEPTH_10BIT
													 <li>@ref E_IM_HDMI_COLOR_DEPTH_12BIT
													 <li>@ref E_IM_HDMI_COLOR_DEPTH_16BIT</ul><br>
												 If the value is 0, same as E_IM_HDMI_COLOR_DEPTH_8BIT(= 4).			*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_VP_PR_CD;

/** structure of vp_conf register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	output_selector	:4;		/**< output_selector : Video Packetizer output selection.<br>
											 <ul><li>@ref E_IM_HDMI_OUTPUT_SELECTOR_PIXEL_PACKING
												 <li>@ref E_IM_HDMI_OUTPUT_SELECTOR_YCC422_REMAP
												 <li>@ref E_IM_HDMI_OUTPUT_SELECTOR_8BIT_BYPASS</ul><br>
											 If the value is 3, same as E_IM_HDMI_OUTPUT_SELECTOR_8BIT_BYPASS(= 2).	*/
		UCHAR	bypass_select	:4;		/**< bypass_select : bypass select.<br>
											 <ul><li>@ref E_IM_HDMI_BYPASS_SELECT_PIXEL_REPEATER
												 <li>@ref E_IM_HDMI_BYPASS_SELECT_VIDEO_PACKETIZER</ul>				*/
		UCHAR	ycc422_en		:1;		/**< ycc422_en : YCC422 select enable.<br>
											 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
												 <li>@ref D_IM_HDMI_ENABLE_ON</ul>									*/
		UCHAR	pr_en			:1;		/**< pr_en : Pixel repeater enable.<br>
											 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
												 <li>@ref D_IM_HDMI_ENABLE_ON</ul>									*/
		UCHAR	pp_en			:1;		/**< pp_en : Pixel packing enable.<br>
											 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
												 <li>@ref D_IM_HDMI_ENABLE_ON</ul>									*/
		UCHAR	bypass_en		:1;		/**< bypass_en : Bypass enable.<br>
											 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
												 <li>@ref D_IM_HDMI_ENABLE_ON</ul>									*/
		UCHAR					:1;		/**< Reserve																*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_VP_CONF;

/** structure of csc_cfg register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	decmode			:2;		/**< decmode : Chroma decimation configuration.<br>
											 Value:<br>
											 0:decimation disabled<br>
											 1:Hd(z) = 1<br>
											 2:Hu(z) = 1/ 2 + z^(-11)+1/2 z^(-2)<br>
											 3:Hd(z)x2^(11) = -5+12z^(-2) - 22z^(-4)+39z^(-8)+109z^(-10) <br>
															  -204z^(-12)+648z^(-14) + 1024z^(-15) <br>
															  +648z^(-16)-204z^(-18) +109z^(-20)- 65z^(-22) <br>
															  +39z^(-24) -22z^(-26)+12z^(-28)-5z^(-30)				*/
		UCHAR					:2;		/**< Reserve																*/
		UCHAR	intmode			:2;		/**< intmode : Chroma interpolation configuration.<br>
											 Value:<br>
											 0:interpolation disabled<br>
											 1:Hd(z) = Hu (z) =1 + z^(-1)<br>
											 2:Hu(z) = Hu(z)=1/ 2 + z^(-11)+1/2 z^(-2)<br>
											 3:interpolation disabled												*/
		UCHAR					:1;		/**< Reserve																*/
		UCHAR	csc_limit		:1;		/**< csc_limit : CSC limit setting enable.<br>
											 <ul><li>@ref D_IM_HDMI_ENABLE_OFF
												 <li>@ref D_IM_HDMI_ENABLE_ON</ul>									*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_CSC_CFG;

/** structure of csc_scale register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	cscscale		:2;		/**< cscscale : Scale factor.<br>
											 Value range : 0~2.														*/
		UCHAR					:2;		/**< Reserve																*/
		UCHAR	csc_color_depth	:4;		/**< csc_color_depth : Color space converter color depth configuration.<br>
											 <ul><li>@ref E_IM_HDMI_COLOR_DEPTH_8BIT
												 <li>@ref E_IM_HDMI_COLOR_DEPTH_10BIT
												 <li>@ref E_IM_HDMI_COLOR_DEPTH_12BIT
												 <li>@ref E_IM_HDMI_COLOR_DEPTH_16BIT</ul><br>
											 If the value is 0, same as E_IM_HDMI_COLOR_DEPTH_8BIT(= 4).			*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_CSC_SCALE;

/** structure of phy_stat0 register */
typedef union {
	UCHAR		byte;		/**< UCHAR data access		*/
	struct {
		UCHAR	TX_PHY_LOCK		:1;		/**< TX_PHY_LOCK : TX PHY PLL lock indication.<br>
											 Value 0:PLL is unlocked, 1:PLL is locked.								*/
		UCHAR	HPD				:1;		/**< HPD : HDMI Hot Plug Detect indication.<br>
											 Value 0:non active, 1:active.											*/
		UCHAR					:2;		/**< Reserve																*/
		UCHAR	RX_SENSE_0		:1;		/**< RX_SENSE_0 : TX PHY RX_SENSE indication for TMDS channel 0 driver.<br>
											 Value 0:non active, 1:active.											*/
		UCHAR	RX_SENSE_1		:1;		/**< RX_SENSE_1 : TX PHY RX_SENSE indication for TMDS channel 1 driver.<br>
											 Value 0:non active, 1:active.											*/
		UCHAR	RX_SENSE_2		:1;		/**< RX_SENSE_2 : TX PHY RX_SENSE indication for TMDS channel 2 driver.<br>
											 Value 0:non active, 1:active.											*/
		UCHAR	RX_SENSE_3		:1;		/**< RX_SENSE_3 : TX PHY RX_SENSE indication for TMDS channel 3 driver.<br>
											 Value 0:non active, 1:active.											*/
	}bit;					/**< for bit access			*/
} U_IM_HDMI_PHY_STAT0;


/**
Control data of Video Packetizer.<br>
*/
typedef struct {
	U_IM_HDMI_VP_PR_CD					vp_pr_cd;				/**< vp_pr_cd: Video Packetizer Pixel Repetition and Color Depth Register.			*/
	E_IM_HDMI_VP_REMAP					vp_remap;				/**< vp_remap: Video Packetizer YCC422 Remapping Register.<br>
																	 <ul><li>@ref E_IM_HDMI_VP_REMAP_16BIT
																		 <li>@ref E_IM_HDMI_VP_REMAP_20BIT
																		 <li>@ref E_IM_HDMI_VP_REMAP_24BIT</ul>										*/
	U_IM_HDMI_VP_CONF					vp_conf;				/**< vp_conf: Video Packetizer Output, Bypass and Enable Configuration Register.	*/
} T_IM_HDMI_VIDEO_PACKETIZER;

/**
Control data of Frame Composer.<br>
*/
typedef struct {
	U_IM_HDMI_FC_INVIDCONF				fc_invidconf;			/**< fc_invidconf: Frame Composer Input Video Configuration.						*/
	USHORT								fc_inhactiv;			/**< fc_inhactiv: Frame Composer Input Video HActive Pixels Register 0/1.<br>
																	 Value range : 0~16838.															*/
	USHORT								fc_inhblank;			/**< fc_inhblank: Frame Composer Input Video HBlank Pixels Register 0/1.<br>
																	 Value range : 0~8191.															*/
	USHORT								fc_invactiv;			/**< fc_invactiv: Frame Composer Input Video VActive Pixels Register 0/1.<br>
																	 Value range : 0~8191.															*/
	UCHAR								fc_invblank;			/**< fc_invblank: Frame Composer Input Video VBlank Pixels Register.<br>
																	 Value range : 0~255.															*/
	USHORT								fc_hsyncindelay;		/**< fc_hsyncindelay: Frame Composer Input Video HSync Front Porch Register 0/1.<br>
																	 Value range : 0~8191.															*/
	USHORT								fc_hsyncinwidth;		/**< fc_hsyncinwidth: Frame Composer Input Video HSync Width Register 0/1.<br>
																	 Value range : 0~1023.															*/
	UCHAR								fc_vsyncindelay;		/**< fc_vsyncindelay: Frame Composer Input Video VSync Front Porch Register.<br>
																	 Value range : 0~255.															*/
	UCHAR								fc_vsyncinwidth;		/**< fc_vsyncinwidth: Frame Composer Input Video VSync Width Register.<br>
																	 Value range : 0~63.															*/
	U_IM_HDMI_FC_AVICONF3				fc_aviconf3;			/**< fc_aviconf3: Frame Composer AVI Packet Configuration Register 3.				*/
	U_IM_HDMI_FC_AVICONF0				fc_aviconf0;			/**< fc_aviconf0: Frame Composer AVI Packet Configuration Register 0.				*/
	U_IM_HDMI_FC_AVICONF1				fc_aviconf1;			/**< fc_aviconf1: Frame Composer AVI Packet Configuration Register 1.				*/
	U_IM_HDMI_FC_AVICONF2				fc_aviconf2;			/**< fc_aviconf2: Frame Composer AVI Packet Configuration Register 2.				*/
	UCHAR								fc_avivid;				/**< fc_avivid: Frame Composer AVI Packet VIC Register.<br>
																	 Value range : 0~255.															*/
	U_IM_HDMI_FC_PRCONF					fc_prconf;				/**< fc_prconf: Frame Composer Pixel Repetition Configuration Register.				*/
} T_IM_HDMI_FRAME_COMPOSER;

/**
Control data of Frame Composer SPD Packet data.<br>
*/
typedef struct {
	UCHAR	fc_spdvendorname[D_IM_HDMI_FC_SPD_VENDER_NAME_NUM];	/**< fc_spdvendorname[]: Vendor Name Regsiter Array.		*/
	UCHAR	fc_spdproductname[D_IM_HDMI_FC_SPD_PRO_NAME_NUM];	/**< fc_spdproductname[]: Product Name Regsiter Array.		*/
	UCHAR	fc_spddeviceinf;									/**< fc_spddeviceinf: Source Product Descriptor Regsiter.	*/
} T_IM_HDMI_FC_SPD;

/**
Control data of Frame Composer VSI Packet data.<br>
*/
typedef struct {
	UCHAR	fc_vsdieeeid[D_IM_HDMI_FC_VSD_IEEE_DATA_NUM];		/**< fc_vsdieeeid[]: VSI IEEE registration identifier.			*/
	UCHAR	fc_vsdsize;											/**< fc_vsdsize: VSI Packet Data Size.							*/
	UCHAR	fc_vsdpayload[D_IM_HDMI_FC_VSD_PAYLOAD_NUM];		/**< fc_vsdpayload[]: VSI Packet Data Payload Register Array.	*/
} T_IM_HDMI_FC_VSD;

/**
Control data of Audio.<br>
*/
typedef struct {
	U_IM_HDMI_AUD_CONF0					aud_conf0;				/**< aud_conf0: Audio I2S Control Register 0.											*/
	U_IM_HDMI_AUD_CONF1					aud_conf1;				/**< aud_conf1: Audio I2S Control Register 1.											*/
	U_IM_HDMI_AUD_CONF2					aud_conf2;				/**< aud_conf2: Audio I2S Control Register 2.											*/
	UCHAR								aud_n1;					/**< aud_n1 : Audio Clock Regenerator N Value Register 1.<br>
																	 Value range : 0x00~0xFF.															*/
	UCHAR								aud_n2;					/**< aud_n2 : Audio Clock Regenerator N Value Register 2.<br>
																	 Value range : 0x00~0xFF.															*/
	UCHAR								aud_n3_audn;			/**< aud_n3.AudN : HDMI Audio Clock Regenerator N value.<br>
																	 Value range : 0x0~0xF.																*/
	UCHAR								aud_ncts_atomic_write;	/**< aud_n3.ncts_atomic_write : HDMI Audio Clock Regenerator N value.<br>
																	 Value range : 0x0~0x1.																*/
	E_IM_HDMI_CTS_MANUAL				cts_manual;				/**< aud_cts3.CTS_manual : To select whether the CTS value manually to use.
																	 <ul><li>@ref E_IM_HDMI_CTS_MANUAL_AUTO
																		 <li>@ref E_IM_HDMI_CTS_MANUAL_MANUAL</ul>										*/
	UCHAR								aud_cts1;				/**< aud_cts1 : Audio Clock Regenerator CTS Value Register 1.<br>
																	 Value range : 0x00~0xFF.															*/
	UCHAR								aud_cts2;				/**< aud_cts2 : Audio Clock Regenerator CTS Value Register 2.<br>
																	 Value range : 0x00~0xFF.															*/
	UCHAR								aud_cts3_audcts;		/**< aud_cts3.AudCTS : HDMI Audio Clock Regenerator CTS calculated value.<br>
																	 Value range : 0x0~0xF.													*/
	U_IM_HDMI_AUD_SPDIF1				aud_spdif1;				/**< aud_spdif1: Audio SPDIF Control Register 1.										*/
	UCHAR								aud_inputclkfs;			/**< aud_inputclkfs: Audio Input Clock FS Factor Register.<br>
																	 <ul><li>@ref E_IM_HDMI_AUD_IFSFACTOR_128XFS
																		 <li>@ref E_IM_HDMI_AUD_IFSFACTOR_256XFS
																		 <li>@ref E_IM_HDMI_AUD_IFSFACTOR_512XFS
																		 <li>@ref E_IM_HDMI_AUD_IFSFACTOR_64XFS</ul><br>
																	 If the value is 3, invalid.<br>
																	 If the value is 5-7, same as E_IM_HDMI_AUD_IFSFACTOR_128XFS(= 0).					*/
	U_IM_HDMI_FC_AUDICONF0				fc_audiconf0;			/**< fc_audiconf0 : Frame Composer AUD Packet Configuration Register 0.			*/
	U_IM_HDMI_FC_AUDICONF1				fc_audiconf1;			/**< fc_audiconf1 : Frame Composer AUD Packet Configuration Register 1.			*/
	UCHAR								fc_audiconf2;			/**< fc_audiconf2 : Frame Composer AUD Packet Configuration Register 2.<br>
																	 Value range : 0~255.														*/
	U_IM_HDMI_FC_AUDICONF3				fc_audiconf3;			/**< fc_audiconf3 : Frame Composer AUD Packet Configuration Register 3.			*/
	U_IM_HDMI_FC_AUDSCONF				fc_audsconf;			/**< fc_audsconf: Frame Composer Audio Sample Flat and Layout Configuration Register.	*/
	UCHAR								sampfreq;				/**< fc_audschnl7.oiec_sampfreq: Sampling frequency.<br>					*/
	UCHAR								oiec_wordlength;		/**< fc_audschnl8.oiec_wordlength: Word length configuration.<br>			*/
} T_IM_HDMI_AUDIO;

/**
Color Space Converter configuration.<br>
*/
typedef struct {
	U_IM_HDMI_CSC_CFG					csc_cfg;				/**< Color Space Converter Interpolation and Decimation Configuration Register.	*/
	U_IM_HDMI_CSC_SCALE					csc_scale;				/**< Color Space Converter Scale and Deep Color Configuration Register.			*/
} T_IM_HDMI_CSC_CONFIG;

/**
Color Space Converter Matrix Coefficient table.<br>
*/
typedef struct {
	UCHAR								msb;					/**< Color Space Converter Matrix Coefficient Register MSB.<br>
																	 Value range :0x00~0xFF.									*/
	UCHAR								lsb;					/**< Color Space Converter Matrix Coefficient Register LSB.<br>
																	 Value range :0x00~0xFF.									*/
} T_IM_HDMI_CSC_COEF;

/**
Control data of PLL configuration.<br>
*/
typedef struct {
	U_IM_HDMI_VP_PR_CD					vp_pr_cd;								/**< vp_pr_cd: Video Packetizer Pixel Repetition and Color Depth Register.	*/
	UCHAR								phy_reg_num;							/**< phy_reg_addr: PHY Register Number.<br>
																					 Value range : 1~8														*/
	UCHAR								phy_reg_addr[D_IM_HDMI_PHY_REG_NUM];	/**< phy_reg_addr : PHY Register Address (Offset).<br>
																					 Address:
																					 <ul><li>0x00 : PWRCTRL<br>
																						 <li>0x06 : OPMODE_PLLCFG<br>
																						 <li>0x09 : CKSYMTXCTRL<br>
																						 <li>0x0E : VLEVCTRL<br>
																						 <li>0x10 : PLLCURRCTRL<br>
																						 <li>0x14 : PLLCTRL<br>
																						 <li>0x15 : PLLGMPCTRL<br>
																						 <li>0x19 : TXTERM<br></ul>										*/
	USHORT								phy_reg_data[D_IM_HDMI_PHY_REG_NUM];	/**< phy_reg_data : PHY Register Value.<br>
																					 Value range : 0x0000~0xFFFF.											*/
} T_IM_HDMI_PLL_CONFIG;

/**
Control data of Audio configuration.<br>
*/
typedef struct {
	E_IM_HDMI_I2S_SELECT				i2s_select;				/**< i2s_select : Interface selction I2S/SPDIF.<br>
																	 <ul><li>@ref E_IM_HDMI_I2S_SELECT_S_PDIF
																		 <li>@ref E_IM_HDMI_I2S_SELECT_I2S</ul>								*/
	U_IM_HDMI_AUD_CONF1					aud_conf1;				/**< aud_conf1: Audio I2S Control Register 1.								*/
	U_IM_HDMI_AUD_CONF2					aud_conf2;				/**< aud_conf2: Audio I2S Control Register 2.								*/
	U_IM_HDMI_AUD_SPDIF1				aud_spdif1;				/**< aud_spdif1: Audio SPDIF Control Register 1.							*/
	UCHAR								aud_inputclkfs;			/**< aud_inputclkfs: Audio Input Clock FS Factor Register.<br>
																	 <ul><li>@ref E_IM_HDMI_AUD_IFSFACTOR_128XFS
																		 <li>@ref E_IM_HDMI_AUD_IFSFACTOR_256XFS
																		 <li>@ref E_IM_HDMI_AUD_IFSFACTOR_512XFS
																		 <li>@ref E_IM_HDMI_AUD_IFSFACTOR_64XFS</ul><br>
																	 If the value is 3, invalid.<br>
																	 If the value is 5-7, same as E_IM_HDMI_AUD_IFSFACTOR_128XFS(= 0).		*/
	UCHAR								aud_n1;					/**< aud_n1 : Audio Clock Regenerator N Value Register 1.<br>
																	 Value range : 0x00~0xFF.												*/
	UCHAR								aud_n2;					/**< aud_n2 : Audio Clock Regenerator N Value Register 2.<br>
																	 Value range : 0x00~0xFF.												*/
	UCHAR								aud_n3_audn;			/**< aud_n3.AudN : HDMI Audio Clock Regenerator N value.<br>
																	 Value range : 0x0~0xF.													*/
	UCHAR								aud_ncts_atomic_write;	/**< aud_n3.ncts_atomic_write : HDMI Audio Clock Regenerator N value.<br>
																	 Value range : 0x0~0x1.													*/
	E_IM_HDMI_CTS_MANUAL				cts_manual;				/**< aud_cts3.CTS_manual : To select whether the CTS value manually to use.
																	 <ul><li>@ref E_IM_HDMI_CTS_MANUAL_AUTO
																		 <li>@ref E_IM_HDMI_CTS_MANUAL_MANUAL</ul>							*/
	UCHAR								aud_cts1;				/**< aud_cts1 : Audio Clock Regenerator CTS Value Register 1.<br>
																	 Value range : 0x00~0xFF.												*/
	UCHAR								aud_cts2;				/**< aud_cts2 : Audio Clock Regenerator CTS Value Register 2.<br>
																	 Value range : 0x00~0xFF.												*/
	UCHAR								aud_cts3_audcts;		/**< aud_cts3.AudCTS : HDMI Audio Clock Regenerator CTS calculated value.<br>
																	 Value range : 0x0~0xF.													*/
	UCHAR								sampfreq;				/**< fc_audschnl7.oiec_sampfreq: Sampling frequency.<br>					*/
	UCHAR								oiec_wordlength;		/**< fc_audschnl8.oiec_wordlength: Word length configuration.<br>			*/
	U_IM_HDMI_FC_AUDSCONF				fc_audsconf;			/**< fc_audsconf: Frame Composer Audio Sample Flat and Layout Configuration Register.	*/
} T_IM_HDMI_AUDIO_CONFIG;

/**
Control data of Infoframes configuration.<br>
*/
typedef struct {
	U_IM_HDMI_FC_AVICONF0				fc_aviconf0;				/**< fc_aviconf0: Frame Composer AVI Packet Configuration Register 0.			*/
	U_IM_HDMI_FC_AVICONF1				fc_aviconf1;				/**< fc_aviconf1: Frame Composer AVI Packet Configuration Register 1.			*/
	U_IM_HDMI_FC_AVICONF2				fc_aviconf2;				/**< fc_aviconf2: Frame Composer AVI Packet Configuration Register 2.			*/
	U_IM_HDMI_FC_AVICONF3				fc_aviconf3;				/**< fc_aviconf3: Frame Composer AVI Packet Configuration Register 3.			*/
	UCHAR								fc_avivid;					/**< fc_avivid: Frame Composer AVI Packet VIC Register.<br>
																		 Value range : 0~255.														*/
	U_IM_HDMI_FC_PRCONF					fc_prconf;					/**< fc_prconf: Frame Composer Pixel Repetition Configuration Register.			*/
	U_IM_HDMI_FC_AUDICONF0				fc_audiconf0;				/**< fc_audiconf0 : Frame Composer AUD Packet Configuration Register 0.			*/
	U_IM_HDMI_FC_AUDICONF1				fc_audiconf1;				/**< fc_audiconf1 : Frame Composer AUD Packet Configuration Register 1.			*/
	UCHAR								fc_audiconf2;				/**< fc_audiconf2 : Frame Composer AUD Packet Configuration Register 2.<br>
																		 Value range : 0~255.														*/
	U_IM_HDMI_FC_AUDICONF3				fc_audiconf3;				/**< fc_audiconf3 : Frame Composer AUD Packet Configuration Register 3.			*/
	UCHAR								fc_vsdieeeid[3];			/**< fc_vsdieeeid0/1/2 : 24bits IEEE ID.<br>
																		 index=0 -> fc_vsdieeeid0 ~ index=2 -> fc_vsdieeeid2.<br>
																		 Value range : 0~255.														*/
	UCHAR								video_mode;					/**< Video Mode.<br>
																		 <ul><li>@ref D_IM_HDMI_VIDEO_MODE_HDMI
																			 <li>@ref D_IM_HDMI_VIDEO_MODE_3D</ul>									*/
	UCHAR								video_format;				/**< Video Format.<br>
																		 If Video Mode=HDMI, this is HDMI Video Format.<br>
																		 - Value range : 0~255.<br>
																		 If Video Mode=3D, this is Format of 3D Video data and 3D_Meta_present.<br>
																		 - bit[7:4]: Format of 3D Video data.<br>
																		 - bit[3]: 3D_Meta_present.													*/
	UCHAR								meta_present;				/**< 3D_Meta_present.<br>
																		 Value range : 0~1.															*/
	UCHAR								additional_video_format;	/**< 3D_EXT_DATA – Additional Video Format.<br>
																		 Value range : 0~15.														*/
	UCHAR								meta_type;					/**< 3D_Metadata_Type.<br>
																		 Value range : 0~7.															*/
	UCHAR								meta_length;				/**< 3D_Metadata_Length.<br>
																		 Value range : 0~31.														*/
	UCHAR								meta_data[8];				/**< 3D_Metadata.<br>
																		 Value range : 0~255.														*/
	UCHAR								scrambler_on;				/**< the HDMI 2.0 scrambler feature.											*/
} T_IM_HDMI_INFO_FRAMES;

/**
Control data of phy_i2cm.<br>
*/
typedef struct {
	UCHAR								phy_i2cm_address;		/**< phy_i2cm_address: PHY I2C Address Configuration Register.<br>
																	 Value range : 0x00~0xFF.												*/
	UCHAR								phy_i2cm_datao[2];		/**< phy_i2cm_datao_0/phy_i2cm_datao_1: PHY I2C Data Write Register.<br>
																	 Value range : 0x00~0xFF.												*/
} T_IM_HDMI_PHY_I2CM_CONFIG;

/**
Control data of i2cm.<br>
*/
typedef struct {
	UCHAR								i2cm_slaveaddr;		/**< i2cm_address: I2C DDC Slave Configuration Register.<br>
																 Value range : 0x00~0xFF.										*/
	UCHAR								i2cm_address;		/**< i2cm_address: I2C DDC Address Configuration Register.<br>
																 Value range : 0x00~0xFF.										*/
	UCHAR								i2cm_segaddr;		/**< i2cm_segaddr: PHY I2C Data Write Register.<br>
																 Value range : 0x00~0xFF.										*/
	UCHAR								i2cm_segptr;		/**< i2cm_segptr: PHY I2C Data Write Register.<br>
																 Value range : 0x00~0xFF.										*/
	E_IM_HDMI_I2CM_OPERATION			i2cm_operation;		/**< i2cm_operation: I2C DDC RD/RD_EXT/WR Operation Register.<br>
																 <ul><li>@ref E_IM_HDMI_I2CM_OPERATION_RD
																	 <li>@ref E_IM_HDMI_I2CM_OPERATION_RD_EXT
																	 <li>@ref E_IM_HDMI_I2CM_OPERATION_RD8
																	 <li>@ref E_IM_HDMI_I2CM_OPERATION_RD8_EXT
																	 <li>@ref E_IM_HDMI_I2CM_OPERATION_WR</ul>					*/
} T_IM_HDMI_I2CM_CONFIG;

/**
Control data of HDMI Transmitter.<br>
*/
typedef struct {
	E_IM_HDMI_VIDEO_MAPPING				video_input_mapping;	/**< tx_invid0.video_mapping: Video Input mapping.<br>
																	 <ul><li>@ref E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_RGB444_10BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_RGB444_12BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_RGB444_16BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_420_8BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_420_10BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_420_12BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_420_16BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC422_12BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC422_10BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC422_8BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_8BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_10BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_12BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC444_IPI_16BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC422_IPI_12BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_8BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_10BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_12BIT
																		 <li>@ref E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_16BIT</ul><br>					*/
	UCHAR								hdcp_keepout;			/**< fc_invidconf.hdcp_keepout: Start/stop HDCP keepout window generation.<br>
																	 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
																		 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>										*/
	UCHAR								vsync_in_polarity;		/**< fc_invidconf.vsync_in_polarity: Vsync input polarity.<br>
																	 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
																		 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>										*/
	UCHAR								hsync_in_polarity;		/**< fc_invidconf.hsync_in_polarity: Hsync input polarity.<br>
																	 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
																		 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>										*/
	UCHAR								de_in_polarity;			/**< fc_invidconf.de_in_polarity: Data enable input polarity.<br>
																	 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
																		 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>										*/
	UCHAR								DVI_modez;				/**< fc_invidconf.DVI_modez : DVI mode selected.<br>
																	 <ul><li>@ref D_IM_HDMI_DVI_MODEZ_DVI
																		 <li>@ref D_IM_HDMI_DVI_MODEZ_HDMI</ul>										*/
	UCHAR								r_v_blank_in_osc;		/**< fc_invidconf.r_v_blank_in_osc: Used for CEA861-D modes.<br>
																	 <ul><li>@ref D_IM_HDMI_ACTIVE_LOW
																		 <li>@ref D_IM_HDMI_ACTIVE_HIGH</ul>										*/
	UCHAR								in_i_p;					/**< fc_invidconf.in_I_P: Input video mode.<br>
																	 <ul><li>@ref D_IM_HDMI_IN_I_P_PROGRESSIVE
																		 <li>@ref D_IM_HDMI_IN_I_P_INTERLACED</ul>									*/
	USHORT								h_in_activ;				/**< fc_inhactiv0/1: Input video Horizontal active pixel region width.<br>
																	 Value range :0~16383.															*/
	USHORT								v_in_activ;				/**< fc_invactiv0/1: Input video Vertical active pixel region width.<br>
																	 Value range :0~8191.															*/
	USHORT								fc_invact_2d;			/**< fc_invact_2d_0/1: 2D Input video vertical active pixel region width.<br>
																	 Value range :0~4095.															*/
	U_IM_HDMI_FC_ACTSPC_HDLR_CFG		fc_actspc_hdlr_cfg;		/**< fc_actspc_hdlr_cfg: Frame Composer Active Space Control.						*/
	USHORT								fc_inhblank;			/**< fc_inhblank: Frame Composer Input Video HBlank Pixels Register 0/1.<br>
																	 Value range : 0~8191.															*/
	UCHAR								fc_invblank;			/**< fc_invblank: Frame Composer Input Video VBlank Pixels Register.<br>
																	 Value range : 0~255.															*/
	USHORT								fc_hsyncindelay;		/**< fc_hsyncindelay: Frame Composer Input Video HSync Front Porch Register 0/1.<br>
																	 Value range : 0~8191.															*/
	USHORT								fc_hsyncinwidth;		/**< fc_hsyncinwidth: Frame Composer Input Video HSync Width Register 0/1.<br>
																	 Value range : 0~1023.															*/
	UCHAR								fc_vsyncindelay;		/**< fc_vsyncindelay: Frame Composer Input Video VSync Front Porch Register.<br>
																	 Value range : 0~255.															*/
	UCHAR								fc_vsyncinwidth;		/**< fc_vsyncinwidth: Frame Composer Input Video VSync Width Register.<br>
																	 Value range : 0~63.															*/
	T_IM_HDMI_PLL_CONFIG				pll_config;				/**< PLL configuration data.														*/
	T_IM_HDMI_AUDIO_CONFIG				audio_config;			/**< Audio configuration data.														*/
	T_IM_HDMI_INFO_FRAMES				info_frames;			/**< Infoframes configuration data.													*/
} T_IM_HDMI_CTRL;

typedef void	(*VP_HDMI_CALLBACK)(UCHAR);		/**< Type is defined to Callback function pointer */

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
 * @brief	HDMI PHY Power On/Off.
 * @param[in]	power		Power On/Off.
 * 							<ul><li>@ref D_IM_HDMI_ENABLE_OFF
 * 								<li>@ref D_IM_HDMI_ENABLE_ON</ul>
 * @retval		D_DDIM_OK						Success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR		Input parameter error.
 */
extern	INT32 Im_HDMI_Power_On(UCHAR power);

/**
 * @brief	Initialize HDMI Transmitter macro.
 * @retval		D_DDIM_OK						Success.
 * @retval		D_IM_HDMI_NG					NG.
 */
extern	INT32 Im_HDMI_Init(VOID);

/**
 * @brief	Configure VGA DVI Video Mode.
 * @param[in]	pll_config		PLL configure paramter.
 * @retval		D_DDIM_OK						Success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR		Input parameter error.
 * @retval		D_IM_HDMI_NG					NG.
 * @remarks		This API uses DDIM_User_Clr_Flg().
 * @remarks		This API uses DDIM_User_Twai_Flg().
 */
extern	INT32 Im_HDMI_Configure_VGA_DVI_Video_Mode(T_IM_HDMI_PLL_CONFIG const *const pll_config);

/**
 * @brief	Set TMDS Scramble by I2C
 * @retval	D_DDIM_OK						Success.
 * @retval	D_IM_HDMI_TIMEOUT				timeout.
 * @remarks	This API uses DDIM_User_Clr_Flg().
 * @remarks		This API uses DDIM_User_Twai_Flg().
 */
extern INT32 Im_HDMI_Set_TMDS_Scramble(BOOL enable);

/**
 * @brief		Read Sink's E-EDID.
 * @param[in]	i2cm_config		configure paramter.
 * @param[out]	read_data		read data.
 * 								Value range:0x00~0xFF.
 * @retval		D_DDIM_OK						Success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR		Input parameter error.
 * @retval		D_IM_HDMI_TIMEOUT				timeout.
 * @remarks		This API uses DDIM_User_Clr_Flg().
 * @remarks		This API uses DDIM_User_Twai_Flg().
 */
extern	INT32 Im_HDMI_Read_Sinks_E_EDID(T_IM_HDMI_I2CM_CONFIG const *const i2cm_config, UCHAR read_data[8]);

/**
 * @brief		Set control data of HDMI Transmitter. 
 * @param[in]	ctrl			Control data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 * @remarks		This API uses DDIM_User_Clr_Flg().
 * @remarks		This API uses DDIM_User_Twai_Flg().
 */
extern	INT32 Im_HDMI_Ctrl( T_IM_HDMI_CTRL const *const ctrl );

/**
 * @brief	Get control data of HDMI Transmitter. 
 * @param[out]	ctrl			Control data.
 * @retval		D_DDIM_OK						success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR		Input Parameter Error.
 * @remarks		This API uses DDIM_User_Clr_Flg().
 */
extern	INT32 Im_HDMI_Get_Ctrl(T_IM_HDMI_CTRL *const ctrl);

/**
 * @brief	Set Color Space Converter.
 * @param[in]	config		CSC configuration data.
 * @param[in]	matrix		Color Space Converter Matrix.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_CSC(T_IM_HDMI_CSC_CONFIG const *const config, const T_IM_HDMI_CSC_COEF matrix[D_IM_HDMI_CSC_CONF_COLUMN_NUM][D_IM_HDMI_CSC_CONF_ROW_NUM]);

/**
 * @brief	Set Video Sampler.
 * @param[in]	tx_invid0		Video Sampler register.
 * @retval		VOID.
 */
extern	VOID Im_HDMI_Set_Video_Sampler(U_IM_HDMI_TX_INVID0 tx_invid0);

/**
 * @brief	Get Video Sampler.
 * @param[out]	tx_invid0		Video Sampler register.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Get_Video_Sampler(U_IM_HDMI_TX_INVID0* tx_invid0);

/**
 * @brief	Set Video Packetizer.
 * @param[in]	video_packetizer		Video Packetizer data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Set_Video_Packetizer(T_IM_HDMI_VIDEO_PACKETIZER const *const video_packetizer);

/**
 * @brief	Get Video Packetizer.
 * @param[out]	video_packetizer		Video Packetizer data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Get_Video_Packetizer(T_IM_HDMI_VIDEO_PACKETIZER *const video_packetizer);

/**
 * @brief	Set Frame Composer.
 * @param[in]	frame_composer		Frame Composer data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Set_Frame_Composer(T_IM_HDMI_FRAME_COMPOSER const *const frame_composer);

/**
 * @brief	Get Frame Composer.
 * @param[out]	frame_composer		Frame Composer data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Get_Frame_Composer(T_IM_HDMI_FRAME_COMPOSER *const frame_composer);

/**
 * @brief	Set Frame Composer (SPD Packet Data).
 * @param[in]	fc_spd		Frame Composer SPD Packet data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Set_Frame_Composer_Spd(T_IM_HDMI_FC_SPD const *const fc_spd);

/**
 * @brief	Get Frame Composer (SPD Packet Data).
 * @param[out]	fc_spd		Frame Composer SPD Packet data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Get_Frame_Composer_Spd(T_IM_HDMI_FC_SPD *const fc_spd);

/**
 * @brief	Set Frame Composer (Vendor Specific).
 * @param[in]	fc_vsd		Frame Composer VSI Packet data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Set_Frame_Composer_Vsd(T_IM_HDMI_FC_VSD const *const fc_vsd);

/**
 * @brief	Get Frame Composer (Vendor Specific).
 * @param[out]	fc_vsd		Frame Composer VSI Packet data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Get_Frame_Composer_Vsd(T_IM_HDMI_FC_VSD *const fc_vsd);

/**
 * @brief	Set Main Controller.
 * @param[in]	mc_flowctrl		Main Controller Feed Through Control Register.<br>
 * 								Value 0:CSC is bypassed, 1:CSC is in the video data path.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Set_Main_Controller(UCHAR mc_flowctrl);

/**
 * @brief	Get Main Controller.
 * @param[out]	mc_flowctrl		Main Controller Feed Through Control Register.
 * 								Value 0:CSC is bypassed, 1:CSC is in the video data path.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Get_Main_Controller(UCHAR* mc_flowctrl);

/**
 * @brief	Set Audio data.
 * @param[in]	audio		Audio data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Set_Audio(T_IM_HDMI_AUDIO const *const audio);

/**
 * @brief	Get Audio data.
 * @param[out]	audio		Audio data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Get_Audio(T_IM_HDMI_AUDIO *const audio);

/**
 * @brief	Set Interrupt callback function.
 * @param[in]	vp_callback		callback funcion pointer.
 * @retval		VOID.
 */
extern	VOID Im_HDMI_Set_Int_Callback(VP_HDMI_CALLBACK vp_callback);

/**
 * @brief	Set Interrupt Mute.
 * @param[in]	interrupt_register	Interrupt register type.<br>
 * 									<ul><li>@ref E_IM_HDMI_INT_REG_FC_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_FC_STAT1
 * 										<li>@ref E_IM_HDMI_INT_REG_FC_STAT2
 * 										<li>@ref E_IM_HDMI_INT_REG_AS_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_PHY_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_I2CM_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_CEC_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_VP_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_I2CMPHY_STAT0</ul>
 * @param[in]	mute				mute setting value.<br>
 * 									bit d[0] ~ d[7]:Detailed interrupt types.<br>
 * 									Value 0:mute cancel, 1:mute.
 * @retval		VOID.
 * @remarks
 * The 2nd parameter (mute) is the interrupt type is determined for each bit of each interrupt register.<br>
 * For more information see below.<br><br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_FC_STAT0		- @ref E_IM_HDMI_IH_FC_STAT0<br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_FC_STAT1		- @ref E_IM_HDMI_IH_FC_STAT1<br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_FC_STAT2		- @ref E_IM_HDMI_IH_FC_STAT2<br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_AS_STAT0		- @ref E_IM_HDMI_IH_AS_STAT0<br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_PHY_STAT0		- @ref E_IM_HDMI_IH_PHY_STAT0<br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_I2CM_STAT0	- @ref E_IM_HDMI_IH_I2CM_STAT0<br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_CEC_STAT0		- @ref E_IM_HDMI_IH_CEC_STAT0<br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_VP_STAT0		- @ref E_IM_HDMI_IH_VP_STAT0<br>
 * 	Interrupt register type:E_IM_HDMI_INT_REG_I2CMPHY_STAT0	- @ref E_IM_HDMI_IH_I2CMPHY_STAT0
 */
extern	VOID Im_HDMI_Set_Int_Mute(E_IM_HDMI_INT_REG interrupt_register, UCHAR mute);

/**
 * @brief	Get Interrupt Mute.
 * @param[in]	interrupt_register	Interrupt register type.
 * 									<ul><li>@ref E_IM_HDMI_INT_REG_FC_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_FC_STAT1
 * 										<li>@ref E_IM_HDMI_INT_REG_FC_STAT2
 * 										<li>@ref E_IM_HDMI_INT_REG_AS_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_PHY_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_I2CM_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_CEC_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_VP_STAT0
 * 										<li>@ref E_IM_HDMI_INT_REG_I2CMPHY_STAT0</ul>
 * @param[out]	mute				mute setting value.<br>
 * 									bit d[0] ~ d[7]:Detailed interrupt types.<br>
 * 									Value 0:mute cancel, 1:mute.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 * @remarks
 * Specification of the second parameter is similar to Im_HDMI_Set_Int_Mute().
 */
extern	INT32 Im_HDMI_Get_Int_Mute(E_IM_HDMI_INT_REG interrupt_register, UCHAR* mute);

/**
 * @brief	Get PHY status.
 * @param[out]	status				PHY status register.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error.
 */
extern	INT32 Im_HDMI_Get_Phy_Status(U_IM_HDMI_PHY_STAT0 *status);

/**
 * @brief	HDMI Interrupt Handler.
 * @retval		VOID.
 * @remarks		This API uses DDIM_User_Set_Flg().
 */
extern	VOID Im_HDMI_Int_Handler(VOID);

#ifdef __cplusplus
}
#endif

#endif	// _IM_HDMI_H_

/*@}*/
/*@}*/

/************************************************************************/
/* Sample Code															*/
/************************************************************************/
/** @weakgroup im_hdmi
@{
<hr>
@section Hdmi_Sample_Basic		Sample of HDMI(Basic sequence)
@code
// SAMPLE CODE //
VOID im_hdmi_sample_initialize(VOID)
{
	INT32 ret = D_DDIM_OK;
	T_IM_HDMI_PLL_CONFIG	pll_config;
	T_IM_HDMI_I2CM_CONFIG	i2cm_config;
	UCHAR					read_sinks_e_edid[8];
	T_IM_HDMI_CTRL			ctrl;
	
	// PLL Configuration.
	pll_config.vp_pr_cd.desired_pr_factor	= 0;
	pll_config.vp_pr_cd.color_depth			= E_IM_HDMI_COLOR_DEPTH_8BIT;
	pll_config.phy_i2cm_address				= 0x00;
	pll_config.phy_i2cm_datao[0]			= 0x00;
	pll_config.phy_i2cm_datao[0]			= 0x00;
	
	// Init.
	ret = Im_HDMI_Init();
	if (ret != D_DDIM_OK){
		// NG.
		return;
	}
	
	// i2cm paramter.
	i2cm_config.i2cm_address	= 0x00;
	i2cm_config.i2cm_segaddr	= 0x00;
	i2cm_config.i2cm_segptr		= 0x00;
	i2cm_config.i2cm_operation	= E_IM_HDMI_I2CM_OPERATION_RD;
	
	// Read Sink’s E-EDID.
	ret = Im_HDMI_Read_Sinks_E_EDID(&i2cm_config, read_sinks_e_edid);
	if (ret != D_DDIM_OK){
		// NG.
		return;
	}
	
	// Read data check.
	
	// Configuration.
	ctrl.video_input_mapping						= E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT;
	ctrl.vsync_in_polarity							= D_IM_HDMI_ACTIVE_LOW;
	ctrl.hsync_in_polarity							= D_IM_HDMI_ACTIVE_LOW;
	ctrl.de_in_polarity								= D_IM_HDMI_ACTIVE_LOW;
	ctrl.DVI_modez									= D_IM_HDMI_DVI_MODEZ_DVI;
	ctrl.r_v_blank_in_osc							= D_IM_HDMI_ACTIVE_LOW;
	ctrl.in_i_p										= D_IM_HDMI_IN_I_P_PROGRESSIVE;
	ctrl.h_in_activ									= 0x280;
	ctrl.v_in_activ									= 0x1E0;
	ctrl.fc_invact_2d;								= 0x1E0;
	ctrl.fc_actspc_hdlr_cfg.actspc_hdlr_en			= D_IM_HDMI_ENABLE_OFF;
	ctrl.fc_actspc_hdlr_cfg.actspc_hdlr_tgl			= D_IM_HDMI_ENABLE_OFF;
	ctrl.fc_inhblank								= 0xA0;
	ctrl.fc_invblank								= 0x2D;
	ctrl.fc_hsyncindelay							= 0x10;
	ctrl.fc_hsyncinwidth							= 0x60;
	ctrl.fc_vsyncindelay							= 0x02;
	ctrl.fc_vsyncinwidth							= 0x02;
	// PLL
	ctrl.pll_config.vp_pr_cd.desired_pr_factor		= 0;
	ctrl.pll_config.vp_pr_cd.color_depth			= E_IM_HDMI_COLOR_DEPTH_8BIT;
	ctrl.pll_config.phy_i2cm_address				= 0;
	ctrl.pll_config.phy_i2cm_datao[0]				= 0;
	ctrl.pll_config.phy_i2cm_datao[0]				= 0;
	
	// When DVI mode (ctrl.DVI_modez=D_IM_HDMI_DVI_MODEZ_DVI),
	// Audio config and Infoframes setting is omitted.
	
	ret = Im_HDMI_Ctrl(&ctrl);
	if (ret != D_DDIM_OK){
		// NG.
		return;
	}
}
@endcode
@}*//* end of im_hdmi*/

