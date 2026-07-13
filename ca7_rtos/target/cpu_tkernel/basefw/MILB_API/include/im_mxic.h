/**
 * @file		im_mxic.h
 * @brief		MXIC driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup im_mxic
@{
	- @ref im_mxic_sample_section1
	- @ref im_mxic_sample_section2
	- @ref im_mxic_sample_section3
	- @ref im_mxic_sample_section4
*/

#ifndef __IM_MXIC_H__
#define __IM_MXIC_H__

#include "driver_common.h"
#include "jdsmxic.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Return value of processing result
#define D_IM_MXIC_INPUT_PARAM_ERROR			(D_IM_MXIC | D_DDIM_INPUT_PARAM_ERROR)	/**< Processing result code.<br> Input parameter error. */

// General-purpose definition of register value (Effectiveness selection)
#define D_IM_MXIC_OFF						(0)			/**< Effectiveness selection of processing.<br> Invalidity	*/
#define D_IM_MXIC_ON						(1)			/**< Effectiveness selection of processing.<br> Effective	*/

// Monitor start trigger
#define D_IM_MXIC_MONITOR_START_OFF			(0x00)		/**< Monitor start trigger.<br> Monitor is not executed */
#define D_IM_MXIC_MONITOR_START_ON			(0x01)		/**< Monitor start trigger.<br> Monitor is started by configuration starting. */
#define D_IM_MXIC_MONITOR_START_ASSERT		(0x02)		/**< Monitor start trigger.<br> Monitor is started by asserting of a monitor start terminal. */
#define D_IM_MXIC_MONITOR_START_TRG			(0x03)		/**< Monitor start trigger.<br> Monitor is started by "1" writing to TMONTRG.TONTRG. */

// Monitor end trigger
#define D_IM_MXIC_MONITOR_ENDLESS			(0x00)		/**< Monitor end trigger.<br> Monitor operation is continued. */
#define D_IM_MXIC_MONITOR_END_ASSERT		(0x02)		/**< Monitor end trigger.<br> Monitor is ended by asserting of the end terminal of a monitor. */
#define D_IM_MXIC_MONITOR_END_LIMIT			(0x03)		/**< Monitor end trigger.<br> Monitor is ended by the amount limit judging of transmission. */

// Monitor update trigger interval
#define D_IM_MXIC_MONITOR_UPDATE_1			(0x00)		/**< Monitor update trigger interval.<br> Assert of monitor update terminal */
#define D_IM_MXIC_MONITOR_UPDATE_8			(0x01)		/**< Monitor update trigger interval.<br> Assert of monitor update terminal 8 times */
#define D_IM_MXIC_MONITOR_UPDATE_16			(0x02)		/**< Monitor update trigger interval.<br> Assert of monitor update terminal 16 times */
#define D_IM_MXIC_MONITOR_UPDATE_32			(0x03)		/**< Monitor update trigger interval.<br> Assert of monitor update terminal 32 times */
#define D_IM_MXIC_MONITOR_UPDATE_64			(0x04)		/**< Monitor update trigger interval.<br> Assert of monitor update terminal 64 times */
#define D_IM_MXIC_MONITOR_UPDATE_128		(0x05)		/**< Monitor update trigger interval.<br> Assert of monitor update terminal 128 times */
#define D_IM_MXIC_MONITOR_UPDATE_256		(0x06)		/**< Monitor update trigger interval.<br> Assert of monitor update terminal 256 times */
#define D_IM_MXIC_MONITOR_UPDATE_512		(0x07)		/**< Monitor update trigger interval.<br> Assert of monitor update terminal 512 times */

// Monitor processing state
#define D_IM_MXIC_MONITOR_OPERATION_PROCESS	(0x00)		/**< Monitor processing state.<br> The monitor is not ended. */
#define D_IM_MXIC_MONITOR_OPERATION_END		(0x01)		/**< Monitor processing state.<br> The monitor is ended. */

// Monitor reading value
#define D_IM_MXIC_VALUE_WHEN_UPDATE_TRG		(0)			/**< Monitor reading value.<br> Amount of transfer data when update trigger generated */
#define D_IM_MXIC_TRANS_COUNT_VALUE			(1)			/**< Monitor reading value.<br> Amount of transfer data count value */

// Monitor type
#define D_IM_MXIC_MONITOR_TRANSFER			(0)			/**< Monitor type.<br> Amount of transfer data */
#define D_IM_MXIC_MONITOR_ACCESS			(1)			/**< Monitor type.<br> Access count */

// Master priority level
#define D_IM_MXIC_MASTER_MOVE_LAST			(0x00)		/**< Master priority level.<br> Master moves to the last of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_1				(0x01)		/**< Master priority level.<br> Master moves to the 1st of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_2				(0x02)		/**< Master priority level.<br> Master moves to the 2nd of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_3				(0x03)		/**< Master priority level.<br> Master moves to the 3rd of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_4				(0x04)		/**< Master priority level.<br> Master moves to the 4th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_5				(0x05)		/**< Master priority level.<br> Master moves to the 5th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_6				(0x06)		/**< Master priority level.<br> Master moves to the 6th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_7				(0x07)		/**< Master priority level.<br> Master moves to the 7th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_8				(0x08)		/**< Master priority level.<br> Master moves to the 8th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_9				(0x09)		/**< Master priority level.<br> Master moves to the 9th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_10			(0x0A)		/**< Master priority level.<br> Master moves to the 10th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_11			(0x0B)		/**< Master priority level.<br> Master moves to the 11th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_12			(0x0C)		/**< Master priority level.<br> Master moves to the 12th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_13			(0x0D)		/**< Master priority level.<br> Master moves to the 13th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_14			(0x0E)		/**< Master priority level.<br> Master moves to the 14th of a slot, after receiving service. */
#define D_IM_MXIC_MASTER_MOVE_15			(0x0F)		/**< Master priority level.<br> Master moves to the 15th of a slot, after receiving service. */

// Slot priority level
#define D_IM_MXIC_SLOT_PRIORITY_UPPER		(0)			/**< Slot priority level.<br> The priority is on upper level slot */
#define D_IM_MXIC_SLOT_PRIORITY_LOWWER		(1)			/**< Slot priority level.<br> The priority is on lower level slot */
#define D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY	(2)			/**< Slot priority level.<br> The priority is fixed upper level slot */
#define D_IM_MXIC_SLOT_PRIORITY_LOWWER_ONLY	(3)			/**< Slot priority level.<br> The priority is fixed lower level slot */

// Master mask setting
#define D_IM_MXIC_MASTER_MASK_OFF			(0x00)		/**< Master mask setting.<br> Transfer request mask OFF */
#define D_IM_MXIC_MASTER_MASK_ON			(0x01)		/**< Master mask setting.<br> Transfer request mask ON */
#define D_IM_MXIC_MASTER_MASK_PERIOD_ON		(0x02)		/**< Master mask setting.<br> Transfer request mask ON during specified period. */

// Number of monitor target
#define D_IM_MXIC_MAX_MONITOR_TARGET_NUM	(4)			/**< The number of monitor target */

// Number of service history
#define D_IM_MXIC_MAX_MONITOR_HISTORY_NUM	(16)		/**< The number of service history monitor */

// Number of master
#define D_IM_MXIC_MAX_W_MASTER_NUM			(127 + 1)	/**< The number of slave write master */
#define D_IM_MXIC_MAX_R_MASTER_NUM			(127 + 1)	/**< The number of slave read master */

// Number of slot on each port
#define D_IM_MXIC_MAX_W_SLOT_NUM			(4)			/**< The number of slot in write arbiter */
#define D_IM_MXIC_MAX_R_SLOT_NUM			(4)			/**< The number of slot in read arbiter */

// Slot size
#define D_IM_MXIC_SLOT_SIZE_8				(8)			/**< Slot size.<br> 8 columns in slot */
#define D_IM_MXIC_SLOT_SIZE_16				(16)		/**< Slot size.<br> 16 columns in slot */

// MXIC common master number (slave)
#define D_IM_MXIC_W_RESERVE					(0x00)		/**< Write master number.<br> Reserve.			(W00). */
#define D_IM_MXIC_R_RESERVE					(0x80)		/**< Read master number.<br> Reserve.			(R00). */

// MXIC0 Write master number (slave)
#define D_IM_MXIC_0_W_SEN_0					(0x01)		/**< Write master number.<br> SEN0				(W01). */
#define D_IM_MXIC_0_W_SEN_1					(0x02)		/**< Write master number.<br> SEN1				(W02). */
#define D_IM_MXIC_0_W_SEN_2					(0x03)		/**< Write master number.<br> SEN2				(W03). */
#define D_IM_MXIC_0_W_SEN_3					(0x04)		/**< Write master number.<br> SEN3				(W04). */

// MXIC1 Write master number (slave)
#define D_IM_MXIC_1_W_SRO					(0x01)		/**< Write master number.<br> SRO				(W01). */
#define D_IM_MXIC_1_W_B2B					(0x02)		/**< Write master number.<br> B2B				(W02). */
#define D_IM_MXIC_1_W_B2R_A					(0x03)		/**< Write master number.<br> B2RA				(W03). */
#define D_IM_MXIC_1_W_B2R_B					(0x04)		/**< Write master number.<br> B2RB				(W04). */
#define D_IM_MXIC_1_W_B2R_C					(0x05)		/**< Write master number.<br> B2RC				(W05). */
#define D_IM_MXIC_1_W_LTMDT					(0x06)		/**< Write master number.<br> LTMDT				(W06). */
#define D_IM_MXIC_1_W_NONE_7				(0x07)		/**< Write master number.<br> none				(W07). */
#define D_IM_MXIC_1_W_NONE_8				(0x08)		/**< Write master number.<br> none				(W08). */
#define D_IM_MXIC_1_W_NONE_9				(0x09)		/**< Write master number.<br> none				(W09). */
#define D_IM_MXIC_1_W_R2Y_A					(0x0A)		/**< Write master number.<br> R2YA				(W10). */
#define D_IM_MXIC_1_W_R2Y_B					(0x0B)		/**< Write master number.<br> R2YB				(W11). */
#define D_IM_MXIC_1_W_R2Y_C					(0x0C)		/**< Write master number.<br> R2YC				(W12). */
#define D_IM_MXIC_1_W_R2Y_D					(0x0D)		/**< Write master number.<br> R2YD				(W13). */
#define D_IM_MXIC_1_W_CNR_A					(0x0E)		/**< Write master number.<br> CNRA				(W14). */
#define D_IM_MXIC_1_W_CNR_B					(0x0F)		/**< Write master number.<br> CNRB				(W15). */

// MXIC1 Read master number (slave)
#define D_IM_MXIC_1_R_SRO					(0x81)		/**< Read master number.<br> SRO				(R01). */
#define D_IM_MXIC_1_R_B2B					(0x82)		/**< Read master number.<br> B2B				(R02). */
#define D_IM_MXIC_1_R_B2R_A					(0x83)		/**< Read master number.<br> B2RA				(R03). */
#define D_IM_MXIC_1_R_NONE_4				(0x84)		/**< Read master number.<br> none				(R04). */
#define D_IM_MXIC_1_R_NONE_5				(0x85)		/**< Read master number.<br> none				(R05). */
#define D_IM_MXIC_1_R_LTMDT					(0x86)		/**< Read master number.<br> LTMDT				(R06). */
#define D_IM_MXIC_1_R_LTM_R					(0x87)		/**< Read master number.<br> LTMR				(R07). */
#define D_IM_MXIC_1_R_LTM_G					(0x88)		/**< Read master number.<br> LTMG				(R08). */
#define D_IM_MXIC_1_R_LTM_B					(0x89)		/**< Read master number.<br> LTMB				(R09). */
#define D_IM_MXIC_1_R_R2Y_A					(0x8A)		/**< Read master number.<br> R2YA				(R10). */
#define D_IM_MXIC_1_R_R2Y_B					(0x8B)		/**< Read master number.<br> R2YB				(R11). */
#define D_IM_MXIC_1_R_R2Y_C					(0x8C)		/**< Read master number.<br> R2YC				(R12). */
#define D_IM_MXIC_1_R_NONE_13				(0x8D)		/**< Read master number.<br> none				(R13). */
#define D_IM_MXIC_1_R_CNR_B					(0x8E)		/**< Read master number.<br> CNRB				(R14). */
#define D_IM_MXIC_1_R_NONE_15				(0x8F)		/**< Read master number.<br> none				(R15). */

// MXIC2 Write master number (slave)
#define D_IM_MXIC_2_W_SRO					(0x01)		/**< Write master number.<br> SRO				(W01). */
#define D_IM_MXIC_2_W_B2B					(0x02)		/**< Write master number.<br> B2B				(W02). */
#define D_IM_MXIC_2_W_B2R_A					(0x03)		/**< Write master number.<br> B2RA				(W03). */
#define D_IM_MXIC_2_W_B2R_B					(0x04)		/**< Write master number.<br> B2RB				(W04). */
#define D_IM_MXIC_2_W_B2R_C					(0x05)		/**< Write master number.<br> B2RC				(W05). */
#define D_IM_MXIC_2_W_LTMDT					(0x06)		/**< Write master number.<br> LTMDT				(W06). */
#define D_IM_MXIC_2_W_NONE_7				(0x07)		/**< Write master number.<br> none				(W07). */
#define D_IM_MXIC_2_W_NONE_8				(0x08)		/**< Write master number.<br> none				(W08). */
#define D_IM_MXIC_2_W_NONE_9				(0x09)		/**< Write master number.<br> none				(W09). */
#define D_IM_MXIC_2_W_R2Y_A					(0x0A)		/**< Write master number.<br> R2YA				(W10). */
#define D_IM_MXIC_2_W_R2Y_B					(0x0B)		/**< Write master number.<br> R2YB				(W11). */
#define D_IM_MXIC_2_W_R2Y_C					(0x0C)		/**< Write master number.<br> R2YC				(W12). */
#define D_IM_MXIC_2_W_R2Y_D					(0x0D)		/**< Write master number.<br> R2YD				(W13). */
#define D_IM_MXIC_2_W_CNR_A					(0x0E)		/**< Write master number.<br> CNRA				(W14). */
#define D_IM_MXIC_2_W_CNR_B					(0x0F)		/**< Write master number.<br> CNRB				(W15). */

// MXIC2 Read master number (slave)
#define D_IM_MXIC_2_R_SRO					(0x81)		/**< Read master number.<br> SRO				(R01). */
#define D_IM_MXIC_2_R_B2B					(0x82)		/**< Read master number.<br> B2B				(R02). */
#define D_IM_MXIC_2_R_B2R_A					(0x83)		/**< Read master number.<br> B2RA				(R03). */
#define D_IM_MXIC_2_R_NONE_4				(0x84)		/**< Read master number.<br> none				(R04). */
#define D_IM_MXIC_2_R_NONE_5				(0x85)		/**< Read master number.<br> none				(R05). */
#define D_IM_MXIC_2_R_LTMDT					(0x86)		/**< Read master number.<br> LTMDT				(R06). */
#define D_IM_MXIC_2_R_LTM_R					(0x87)		/**< Read master number.<br> LTMR				(R07). */
#define D_IM_MXIC_2_R_LTM_G					(0x88)		/**< Read master number.<br> LTMG				(R08). */
#define D_IM_MXIC_2_R_LTM_B					(0x89)		/**< Read master number.<br> LTMB				(R09). */
#define D_IM_MXIC_2_R_R2Y_A					(0x8A)		/**< Read master number.<br> R2YA				(R10). */
#define D_IM_MXIC_2_R_R2Y_B					(0x8B)		/**< Read master number.<br> R2YB				(R11). */
#define D_IM_MXIC_2_R_R2Y_C					(0x8C)		/**< Read master number.<br> R2YC				(R12). */
#define D_IM_MXIC_2_R_NONE_13				(0x8D)		/**< Read master number.<br> none				(R13). */
#define D_IM_MXIC_2_R_CNR_B					(0x8E)		/**< Read master number.<br> CNRB				(R14). */
#define D_IM_MXIC_2_R_NONE_15				(0x8F)		/**< Read master number.<br> none				(R15). */

// MXIC3 Write master number (slave)
#define D_IM_MXIC_3_W_STAT					(0x01)		/**< Write master number.<br> STAT				(W01). */
#define D_IM_MXIC_3_W_IIP_A					(0x02)		/**< Write master number.<br> IIPA				(W02). */
#define D_IM_MXIC_3_W_IIP_B					(0x03)		/**< Write master number.<br> IIPB				(W03). */
#define D_IM_MXIC_3_W_IIP_C					(0x04)		/**< Write master number.<br> IIPC				(W04). */
#define D_IM_MXIC_3_W_SHDR_A				(0x05)		/**< Write master number.<br> SHDRA				(W05). */
#define D_IM_MXIC_3_W_SHDR_B				(0x06)		/**< Write master number.<br> SHDRB				(W06). */
#define D_IM_MXIC_3_W_SHDR_C				(0x07)		/**< Write master number.<br> SHDRC				(W07). */
#define D_IM_MXIC_3_W_NONE_8				(0x08)		/**< Write master number.<br> none				(W08). */
#define D_IM_MXIC_3_W_ME					(0x09)		/**< Write master number.<br> ME				(W09). */
#define D_IM_MXIC_3_W_IPU_A					(0x0A)		/**< Write master number.<br> IPU_A				(W10). */
#define D_IM_MXIC_3_W_NONE_11				(0x0B)		/**< Write master number.<br> none				(W11). */
#define D_IM_MXIC_3_W_IPU_C					(0x0C)		/**< Write master number.<br> IPU_C				(W12). */
#define D_IM_MXIC_3_W_IPU_D					(0x0D)		/**< Write master number.<br> IPU_D				(W13). */
#define D_IM_MXIC_3_W_IPU_E					(0x0E)		/**< Write master number.<br> IPU_E				(W14). */
#define D_IM_MXIC_3_W_DSP_A					(0x0F)		/**< Write master number.<br> DSP_A				(W15). */
#define D_IM_MXIC_3_W_DSP_B					(0x10)		/**< Write master number.<br> DSP_B				(W16). */
#define D_IM_MXIC_3_W_GPU					(0x11)		/**< Write master number.<br> GPU				(W17). */
#define D_IM_MXIC_3_W_DW					(0x12)		/**< Write master number.<br> DW				(W18). */
#define D_IM_MXIC_3_W_JPEG					(0x13)		/**< Write master number.<br> JPEG				(W19). */
#define D_IM_MXIC_3_W_RAW					(0x14)		/**< Write master number.<br> RAW				(W20). */
#define D_IM_MXIC_3_W_NONE_21				(0x15)		/**< Write master number.<br> none				(W21). */
#define D_IM_MXIC_3_W_GROUP0_M0				(0x30)		/**< Write master number.<br> Group0 Master0	(W48). */
#define D_IM_MXIC_3_W_GROUP0_M1				(0x31)		/**< Write master number.<br> Group0 Master1	(W49). */

// MXIC3 Write master number (group)
#define D_IM_MXIC_3_W_ELA					(0x40)		/**< Write master number.<br> ELA				(W64). */
#define D_IM_MXIC_3_W_XCH					(0x41)		/**< Write master number.<br> XCH				(W65). */
#define D_IM_MXIC_3_W_FPT_0					(0x42)		/**< Write master number.<br> FPT0				(W66). */
#define D_IM_MXIC_3_W_FPT_1					(0x43)		/**< Write master number.<br> FPT1				(W67). */

// MXIC3 Read master number (slave)
#define D_IM_MXIC_3_R_STAT					(0x81)		/**< Read master number.<br> STAT				(R01). */
#define D_IM_MXIC_3_R_IIP_A					(0x82)		/**< Read master number.<br> IIPA				(R02). */
#define D_IM_MXIC_3_R_IIP_B					(0x83)		/**< Read master number.<br> IIPB				(R03). */
#define D_IM_MXIC_3_R_IIP_C					(0x84)		/**< Read master number.<br> IIPC				(R04). */
#define D_IM_MXIC_3_R_SHDR_A				(0x85)		/**< Read master number.<br> SHDRA				(R05). */
#define D_IM_MXIC_3_R_SHDR_B				(0x86)		/**< Read master number.<br> SHDRB				(R06). */
#define D_IM_MXIC_3_R_SHDR_C				(0x87)		/**< Read master number.<br> SHDRC				(R07). */
#define D_IM_MXIC_3_R_SHDR_D				(0x88)		/**< Read master number.<br> SHDRD				(R08). */
#define D_IM_MXIC_3_R_ME					(0x89)		/**< Read master number.<br> ME					(R09). */
#define D_IM_MXIC_3_R_IPU_A					(0x8A)		/**< Read master number.<br> IPU_A				(R10). */
#define D_IM_MXIC_3_R_IPU_B					(0x8B)		/**< Read master number.<br> IPU_B				(R11). */
#define D_IM_MXIC_3_R_IPU_C					(0x8C)		/**< Read master number.<br> IPU_C				(R12). */
#define D_IM_MXIC_3_R_IPU_D					(0x8D)		/**< Read master number.<br> IPU_D				(R13). */
#define D_IM_MXIC_3_R_NONE_14				(0x8E)		/**< Read master number.<br> none				(R14). */
#define D_IM_MXIC_3_R_DSP_A					(0x8F)		/**< Read master number.<br> DSP_A				(R15). */
#define D_IM_MXIC_3_R_DSP_B					(0x90)		/**< Read master number.<br> DSP_B				(R16). */
#define D_IM_MXIC_3_R_GPU					(0x91)		/**< Read master number.<br> GPU				(R17). */
#define D_IM_MXIC_3_R_NONE_18				(0x92)		/**< Read master number.<br> none				(R18). */
#define D_IM_MXIC_3_R_JPEG					(0x93)		/**< Read master number.<br> JPEG				(R19). */
#define D_IM_MXIC_3_R_RAW					(0x94)		/**< Read master number.<br> RAW				(R20). */
#define D_IM_MXIC_3_R_RDMA					(0x95)		/**< Read master number.<br> RDMA				(R21). */
#define D_IM_MXIC_3_R_GROUP0_M0				(0xB0)		/**< Read master number.<br> Group0 Master0		(R48). */
#define D_IM_MXIC_3_R_GROUP0_M1				(0xB1)		/**< Read master number.<br> Group0 Master1		(R49). */

// MXIC3 Read master number (group0)
#define D_IM_MXIC_3_R_ELA					(0xC0)		/**< Write master number.<br> ELA				(R64). */
#define D_IM_MXIC_3_R_XCH					(0xC1)		/**< Write master number.<br> XCH				(R65). */
#define D_IM_MXIC_3_R_FPT_0					(0xC2)		/**< Write master number.<br> FPT0				(R66). */
#define D_IM_MXIC_3_R_FPT_1					(0xC3)		/**< Write master number.<br> FPT1				(R67). */

// MXIC4 Read master number (slave)
#define D_IM_MXIC_4_R_DISP_A				(0x81)		/**< Write master number.<br> DISPA				(R01). */
#define D_IM_MXIC_4_R_DISP_B				(0x82)		/**< Write master number.<br> DISPB				(R02). */

// MXIC5 Write master number (slave)
#define D_IM_MXIC_5_W_EXS_0					(0x01)		/**< Write master number.<br> EXS0				(W01). */
#define D_IM_MXIC_5_W_EXS_1					(0x02)		/**< Write master number.<br> EXS1				(W02). */
#define D_IM_MXIC_5_W_NIC					(0x03)		/**< Write master number.<br> NIC				(W03). */
#define D_IM_MXIC_5_W_BMH_0					(0x04)		/**< Write master number.<br> BMH0				(W04). */
#define D_IM_MXIC_5_W_BMH_1					(0x05)		/**< Write master number.<br> BMH1				(W05). */

// MXIC5 Read master number (slave)
#define D_IM_MXIC_5_R_EXS_0					(0x81)		/**< Write master number.<br> EXS0				(R01). */
#define D_IM_MXIC_5_R_EXS_1					(0x82)		/**< Write master number.<br> EXS1				(R02). */
#define D_IM_MXIC_5_R_NIC					(0x83)		/**< Write master number.<br> NIC				(R03). */
#define D_IM_MXIC_5_R_BMH_0					(0x84)		/**< Write master number.<br> BMH0				(R04). */
#define D_IM_MXIC_5_R_BMH_1					(0x85)		/**< Write master number.<br> BMH1				(R05). */

// MXIC6 Write master number (slave)
#define D_IM_MXIC_6_W_CA7					(0x01)		/**< Write master number.<br> CA7				(W01). */

// MXIC6 Read master number (slave)
#define D_IM_MXIC_6_R_CA7					(0x81)		/**< Write master number.<br> CA7				(R01). */

// Monitor target slave write channel
#define D_IM_MXIC_SLAVE_W1_PORT0			(0x104)		/**< Monitor target slave write channel.<br> W1-Port0 */
#define D_IM_MXIC_SLAVE_W1_PORT1			(0x105)		/**< Monitor target slave write channel.<br> W1-Port1 */
#define D_IM_MXIC_SLAVE_W1_PORT2			(0x106)		/**< Monitor target slave write channel.<br> W1-Port2 */
#define D_IM_MXIC_SLAVE_W1_PORT3			(0x107)		/**< Monitor target slave write channel.<br> W1-Port3 */
#define D_IM_MXIC_SLAVE_W2_PORT0			(0x108)		/**< Monitor target slave write channel.<br> W2-Port0 */
#define D_IM_MXIC_SLAVE_W2_PORT1			(0x109)		/**< Monitor target slave write channel.<br> W2-Port1 */
#define D_IM_MXIC_SLAVE_W2_PORT2			(0x10A)		/**< Monitor target slave write channel.<br> W2-Port2 */
#define D_IM_MXIC_SLAVE_W2_PORT3			(0x10B)		/**< Monitor target slave write channel.<br> W2-Port3 */
#define D_IM_MXIC_SLAVE_W3_PORT0			(0x10C)		/**< Monitor target slave write channel.<br> W3-Port0 */
#define D_IM_MXIC_SLAVE_W3_PORT1			(0x10D)		/**< Monitor target slave write channel.<br> W3-Port1 */
#define D_IM_MXIC_SLAVE_W3_PORT2			(0x10E)		/**< Monitor target slave write channel.<br> W3-Port2 */
#define D_IM_MXIC_SLAVE_W3_PORT3			(0x10F)		/**< Monitor target slave write channel.<br> W3-Port3 */
#define D_IM_MXIC_SLAVE_W4_PORT0			(0x110)		/**< Monitor target slave write channel.<br> W4-Port0 */
#define D_IM_MXIC_SLAVE_W4_PORT1			(0x111)		/**< Monitor target slave write channel.<br> W4-Port1 */
#define D_IM_MXIC_SLAVE_W4_PORT2			(0x112)		/**< Monitor target slave write channel.<br> W4-Port2 */
#define D_IM_MXIC_SLAVE_W4_PORT3			(0x113)		/**< Monitor target slave write channel.<br> W4-Port3 */

// Monitor target slave read channel
#define D_IM_MXIC_SLAVE_R1_PORT0			(0x184)		/**< Monitor target slave read channel.<br> R1-Port0 */
#define D_IM_MXIC_SLAVE_R1_PORT1			(0x185)		/**< Monitor target slave read channel.<br> R1-Port1 */
#define D_IM_MXIC_SLAVE_R1_PORT2			(0x186)		/**< Monitor target slave read channel.<br> R1-Port2 */
#define D_IM_MXIC_SLAVE_R1_PORT3			(0x187)		/**< Monitor target slave read channel.<br> R1-Port3 */
#define D_IM_MXIC_SLAVE_R2_PORT0			(0x188)		/**< Monitor target slave read channel.<br> R2-Port0 */
#define D_IM_MXIC_SLAVE_R2_PORT1			(0x189)		/**< Monitor target slave read channel.<br> R2-Port1 */
#define D_IM_MXIC_SLAVE_R2_PORT2			(0x18A)		/**< Monitor target slave read channel.<br> R2-Port2 */
#define D_IM_MXIC_SLAVE_R2_PORT3			(0x18B)		/**< Monitor target slave read channel.<br> R2-Port3 */
#define D_IM_MXIC_SLAVE_R3_PORT0			(0x18C)		/**< Monitor target slave read channel.<br> R3-Port0 */
#define D_IM_MXIC_SLAVE_R3_PORT1			(0x18D)		/**< Monitor target slave read channel.<br> R3-Port1 */
#define D_IM_MXIC_SLAVE_R3_PORT2			(0x18E)		/**< Monitor target slave read channel.<br> R3-Port2 */
#define D_IM_MXIC_SLAVE_R3_PORT3			(0x18F)		/**< Monitor target slave read channel.<br> R3-Port3 */
#define D_IM_MXIC_SLAVE_R4_PORT0			(0x190)		/**< Monitor target slave read channel.<br> R4-Port0 */
#define D_IM_MXIC_SLAVE_R4_PORT1			(0x191)		/**< Monitor target slave read channel.<br> R4-Port1 */
#define D_IM_MXIC_SLAVE_R4_PORT2			(0x192)		/**< Monitor target slave read channel.<br> R4-Port2 */
#define D_IM_MXIC_SLAVE_R4_PORT3			(0x193)		/**< Monitor target slave read channel.<br> R4-Port3 */

// Monitor target nothing
#define D_IM_MXIC_TARGET_NOTHING			(0x00)		/**< Monitor target nothing */

// Master number max.
#define D_IM_MXIC_MASTER_NUMBER_MAX			(0x40)		/**< Max master number.		*/

// The kind of channel which the error generated
#define D_IM_MXIC_DEC_ERR_CH_W				(0x01)		/**< The decoding error occurred by the write channel.<br> */
#define D_IM_MXIC_DEC_ERR_CH_R				(0x02)		/**< The decoding error occurred by the read channel.<br> */
#define D_IM_MXIC_DEC_ERR_CH_RW				(0x04)		/**< The decoding error occurred by the read channel and write channel.<br> */

typedef VOID (*IM_MXIC_DEC_ERR_CALLBACK)(UCHAR ch_type);	/**< Type is defined to callback function pointer for decode error.<br>
															 Decode error information is initialized after the end of processing of a Callback function.<br>
															 For this reason, please be sure to use an @ref Im_MXIC_Get_Decode_Error function from a Callback function and to refer to error information. <br>
															 The meaning of the argument is as follows.<br>
															 - UCHAR ch_type: The kind of channel which the error generated.<br>
																<ul><li>@ref D_IM_MXIC_DEC_ERR_CH_W
																	<li>@ref D_IM_MXIC_DEC_ERR_CH_R
																	<li>@ref D_IM_MXIC_DEC_ERR_CH_RW</ul> */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/**	Slave write or read Arbiter selection. */
typedef enum {
	E_IM_MXIC_UNIT_0 = 0,						/**< MXIC0				*/
	E_IM_MXIC_UNIT_1,							/**< MXIC1				*/
	E_IM_MXIC_UNIT_2,							/**< MXIC2				*/
	E_IM_MXIC_UNIT_3,							/**< MXIC3				*/
	E_IM_MXIC_UNIT_4,							/**< MXIC4				*/
	E_IM_MXIC_UNIT_5,							/**< MXIC5				*/
	E_IM_MXIC_UNIT_6,							/**< MXIC6				*/
	E_IM_MXIC_UNIT_MAX							/**< Enumeration stopper */
} E_IM_MXIC_UNIT;

/**	Slave write or read Arbiter selection. */
typedef enum {
	E_IM_MXIC_WR_ARBITER_W = 0,					/**< Write arbiter */
	E_IM_MXIC_WR_ARBITER_R,						/**< Read arbiter */
	E_IM_MXIC_WR_ARBITER_MAX					/**< Enumeration stopper */
} E_IM_MXIC_WR_ARBITER;

/**	Slave Spec Arbiter selection. (slave) */
typedef enum {
	E_IM_MXIC_SPEC_ARBITER_1 = 0,				/**< slave1 arbiter */
	E_IM_MXIC_SPEC_ARBITER_2,					/**< slave2 Read arbiter */
	E_IM_MXIC_SPEC_ARBITER_3,					/**< slave3 Read arbiter */
	E_IM_MXIC_SPEC_ARBITER_4,					/**< slave4 Read arbiter */
	E_IM_MXIC_SPEC_ARBITER_MAX					/**< Enumeration stopper */
} E_IM_MXIC_SPEC_ARBITER;

/**	Slave write arbiter selection. (slave) */
typedef enum {
	E_IM_MXIC_W_ARBITER_W1 = 0,					/**< W1 arbiter */
	E_IM_MXIC_W_ARBITER_W2,						/**< W2 arbiter */
	E_IM_MXIC_W_ARBITER_W3,						/**< W3 arbiter */
	E_IM_MXIC_W_ARBITER_W4,						/**< W4 arbiter */
	E_IM_MXIC_W_ARBITER_MAX						/**< Enumeration stopper */
} E_IM_MXIC_W_ARBITER;

/**	Slave read arbiter selection. (slave) */
typedef enum {
	E_IM_MXIC_R_ARBITER_R1 = 0,					/**< R1 arbiter */
	E_IM_MXIC_R_ARBITER_R2,						/**< R2 arbiter */
	E_IM_MXIC_R_ARBITER_R3,						/**< R3 arbiter */
	E_IM_MXIC_R_ARBITER_R4,						/**< R4 arbiter */
	E_IM_MXIC_R_ARBITER_MAX						/**< Enumeration stopper */
} E_IM_MXIC_R_ARBITER;

/**	Slave Spec Arbiter selection. (group) */
typedef enum {
	E_IM_MXIC_SPEC_ARBITER_GR_0 = 0,			/**< group0 arbiter */
	E_IM_MXIC_SPEC_ARBITER_GR_1,				/**< group1 arbiter */
	E_IM_MXIC_SPEC_ARBITER_GR_2,				/**< group2 arbiter */
	E_IM_MXIC_SPEC_ARBITER_GR_3,				/**< group3 arbiter */
	E_IM_MXIC_SPEC_ARBITER_GR_4,				/**< group4 arbiter */
	E_IM_MXIC_SPEC_ARBITER_GR_5,				/**< group5 arbiter */
	E_IM_MXIC_SPEC_ARBITER_GR_6,				/**< group6 arbiter */
	E_IM_MXIC_SPEC_ARBITER_GR_7,				/**< group7 arbiter */
	E_IM_MXIC_SPEC_ARBITER_GR_MAX				/**< Enumeration stopper */
} E_IM_MXIC_SPEC_ARBITER_GR;

/**	Slave write arbiter selection. (group) */
typedef enum {
	E_IM_MXIC_W_ARBITER_GR_W0 = 0,				/**< W0 arbiter */
	E_IM_MXIC_W_ARBITER_GR_W1,					/**< W1 arbiter */
	E_IM_MXIC_W_ARBITER_GR_W2,					/**< W2 arbiter */
	E_IM_MXIC_W_ARBITER_GR_W3,					/**< W3 arbiter */
	E_IM_MXIC_W_ARBITER_GR_W4,					/**< W4 arbiter */
	E_IM_MXIC_W_ARBITER_GR_W5,					/**< W5 arbiter */
	E_IM_MXIC_W_ARBITER_GR_W6,					/**< W6 arbiter */
	E_IM_MXIC_W_ARBITER_GR_W7,					/**< W7 arbiter */
	E_IM_MXIC_W_ARBITER_GR_MAX					/**< Enumeration stopper */
} E_IM_MXIC_W_ARBITER_GR;

/**	Slave read arbiter selection. (group) */
typedef enum {
	E_IM_MXIC_R_ARBITER_GR_R0 = 0,				/**< R0 arbiter */
	E_IM_MXIC_R_ARBITER_GR_R1,					/**< R1 arbiter */
	E_IM_MXIC_R_ARBITER_GR_R2,					/**< R2 arbiter */
	E_IM_MXIC_R_ARBITER_GR_R3,					/**< R3 arbiter */
	E_IM_MXIC_R_ARBITER_GR_R4,					/**< R4 arbiter */
	E_IM_MXIC_R_ARBITER_GR_R5,					/**< R5 arbiter */
	E_IM_MXIC_R_ARBITER_GR_R6,					/**< R6 arbiter */
	E_IM_MXIC_R_ARBITER_GR_R7,					/**< R7 arbiter */
	E_IM_MXIC_R_ARBITER_GR_MAX					/**< Enumeration stopper */
} E_IM_MXIC_R_ARBITER_GR;

/**	Slot selection. */
typedef enum {
	E_IM_MXIC_SLOT_0 = 0,						/**< Slot0 */
	E_IM_MXIC_SLOT_1,							/**< Slot1 */
	E_IM_MXIC_SLOT_2,							/**< Slot2 */
	E_IM_MXIC_SLOT_3,							/**< Slot3 */
	E_IM_MXIC_SLOT_MAX							/**< Enumeration stopper */
} E_IM_MXIC_SLOT;

/**	Slot level control selection. */
typedef enum {
	E_IM_MXIC_SLOT_LEVEL_CTRL_0 = 0,			/**< Slot level control block 0 */
	E_IM_MXIC_SLOT_LEVEL_CTRL_1,				/**< Slot level control block 1 */
	E_IM_MXIC_SLOT_LEVEL_CTRL_2,				/**< Slot level control block 2 */
	E_IM_MXIC_SLOT_LEVEL_CTRL_MAX				/**< Enumeration stopper */
} E_IM_MXIC_SLOT_LEVEL_CTRL;

/**	Port selection. */
typedef enum {
	E_IM_MXIC_PORT_0 = 0,						/**< Port-0	*/
	E_IM_MXIC_PORT_1,							/**< Port-1	*/
	E_IM_MXIC_PORT_2,							/**< Port-2	*/
	E_IM_MXIC_PORT_3,							/**< Port-3	*/
	E_IM_MXIC_PORT_MAX							/**< Enumeration stopper */
} E_IM_MXIC_PORT;

/**	Port selection.  (group) */
typedef enum {
	E_IM_MXIC_PORT_GR_0 = 0,					/**< Port-0	*/
	E_IM_MXIC_PORT_GR_1,						/**< Port-1	*/
	E_IM_MXIC_PORT_GR_MAX						/**< Enumeration stopper */
} E_IM_MXIC_PORT_GR;

/** Slave number. */
typedef enum {
	E_IM_MXIC_SLAVE_NUMBER_1 = 0,				/**< Slave number-1 */
	E_IM_MXIC_SLAVE_NUMBER_2,					/**< Slave number-2 */
	E_IM_MXIC_SLAVE_NUMBER_3,					/**< Slave number-3 */
	E_IM_MXIC_SLAVE_NUMBER_4,					/**< Slave number-4 */
	E_IM_MXIC_SLAVE_NUMBER_MAX					/**< Enumeration stopper */
} E_IM_MXIC_SLAVE_NUMBER;

/**	Slot assigned to a port. */
typedef enum {
	E_IM_MXIC_PORT_ASSIGN_SLOT_0     = 0,		/**< Slot-0 is assigned to Port-0.		*/
	E_IM_MXIC_PORT_ASSIGN_SLOT_0_1   = 1,		/**< Slot-0~1 is assigned to Port-0.	*/
	E_IM_MXIC_PORT_ASSIGN_SLOT_0_1_2 = 2,		/**< Slot-0~2 is assigned to Port-0.	*/
	E_IM_MXIC_PORT_ASSIGN_SLOT_ALL   = 3,		/**< Slot-0~3 is assigned to Port-0.	*/
	E_IM_MXIC_PORT_ASSIGN_MAX					/**< Enumeration stopper */
} E_IM_MXIC_PORT_ASSIGN;

/** Mask group selection. */
typedef enum{
	E_IM_MXIC_MASK_GROUP_A = 0,					/**< Mask group A */
	E_IM_MXIC_MASK_GROUP_B,						/**< Mask group B */
	E_IM_MXIC_MASK_GROUP_MAX					/**< Enumeration stopper */
} E_IM_MXIC_MASK_GROUP;

/**	Level selection. */
typedef enum {
	E_IM_MXIC_LEVEL_0 = 0,						/**< Level-0	*/
	E_IM_MXIC_LEVEL_1,							/**< Level-1	*/
	E_IM_MXIC_LEVEL_2,							/**< Level-2	*/
	E_IM_MXIC_LEVEL_3,							/**< Level-3	*/
	E_IM_MXIC_LEVEL_MAX							/**< Enumeration stopper */
} E_IM_MXIC_LEVEL;

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/** Master assignment into all the slave slots of W arbiter. */
typedef struct{
	UCHAR		slot0[D_IM_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot0 */
	UCHAR		slot1[D_IM_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot1 */
	UCHAR		slot2[D_IM_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot2 */
	UCHAR		slot3[D_IM_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot3 */
} T_IM_MXIC_W_ARBITER_ASSIGN;

/** Master assignment into all the slave slots of R arbiter. */
typedef struct{
	UCHAR		slot0[D_IM_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot0 */
	UCHAR		slot1[D_IM_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot1 */
	UCHAR		slot2[D_IM_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot2 */
	UCHAR		slot3[D_IM_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot3 */
} T_IM_MXIC_R_ARBITER_ASSIGN;

/** Master assignment into all the slave slots of each port of W arbiter. */
typedef struct{
	T_IM_MXIC_W_ARBITER_ASSIGN		port[E_IM_MXIC_PORT_MAX];	/**< Write master assignment in port */
} T_IM_MXIC_W_ARBITER_ASSIGN_PORT;

/** Master assignment into all the slave slots of each port of R arbiter. */
typedef struct{
	T_IM_MXIC_R_ARBITER_ASSIGN		port[E_IM_MXIC_PORT_MAX];	/**< Read master assignment in port */
} T_IM_MXIC_R_ARBITER_ASSIGN_PORT;

/** Master assignment into all the slave slots of W(group) arbiter. */
typedef struct{
	UCHAR		slot[D_IM_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot0 */
} T_IM_MXIC_W_ARBITER_ASSIGN_GR;

/** Master assignment into all the slave slots of R(group) arbiter. */
typedef struct{
	UCHAR		slot[D_IM_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot0 */
} T_IM_MXIC_R_ARBITER_ASSIGN_GR;

/** Master assignment into all the slots of all arbiter. */
typedef struct{
	T_IM_MXIC_W_ARBITER_ASSIGN_PORT	w_arbiter[E_IM_MXIC_W_ARBITER_MAX];	/**< Slave Write master assignment in W arbiter */
	T_IM_MXIC_R_ARBITER_ASSIGN_PORT	r_arbiter[E_IM_MXIC_R_ARBITER_MAX];	/**< Slave Read master assignment in R arbiter */
} T_IM_MXIC_ALL_ARBITER_ASSIGN;

/** Setting of configration start enable */
typedef struct{
	UCHAR		sl_w1_config_on_flg;			/**< TCFMD.bit.TENW_1: Configration start enable flag in slave W1 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		sl_w2_config_on_flg;			/**< TCFMD.bit.TENW_2: Configration start enable flag in slave W2 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		sl_w3_config_on_flg;			/**< TCFMD.bit.TENW_3: Configration start enable flag in slave W3 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		sl_w4_config_on_flg;			/**< TCFMD.bit.TENW_4: Configration start enable flag in slave W4 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		sl_r1_config_on_flg;			/**< TCFMD.bit.TENR_1: Configration start enable flag in slave R1 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		sl_r2_config_on_flg;			/**< TCFMD.bit.TENR_2: Configration start enable flag in slave R2 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		sl_r3_config_on_flg;			/**< TCFMD.bit.TENR_3: Configration start enable flag in slave R3 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		sl_r4_config_on_flg;			/**< TCFMD.bit.TENR_4: Configration start enable flag in slave R4 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_w0_config_on_flg;			/**< TGCFMD.bit.TEGW_0: Configration start enable flag in group W0 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_w1_config_on_flg;			/**< TGCFMD.bit.TEGW_1: Configration start enable flag in group W1 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_w2_config_on_flg;			/**< TGCFMD.bit.TEGW_2: Configration start enable flag in group W2 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_w3_config_on_flg;			/**< TGCFMD.bit.TEGW_3: Configration start enable flag in group W3 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_w4_config_on_flg;			/**< TGCFMD.bit.TEGW_4: Configration start enable flag in group W4 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_w5_config_on_flg;			/**< TGCFMD.bit.TEGW_5: Configration start enable flag in group W5 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_w6_config_on_flg;			/**< TGCFMD.bit.TEGW_6: Configration start enable flag in group W6 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_w7_config_on_flg;			/**< TGCFMD.bit.TEGW_7: Configration start enable flag in group W7 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_r0_config_on_flg;			/**< TGCFMD.bit.TEGR_0: Configration start enable flag in group R0 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_r1_config_on_flg;			/**< TGCFMD.bit.TEGR_1: Configration start enable flag in group R1 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_r2_config_on_flg;			/**< TGCFMD.bit.TEGR_2: Configration start enable flag in group R2 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_r3_config_on_flg;			/**< TGCFMD.bit.TEGR_3: Configration start enable flag in group R3 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_r4_config_on_flg;			/**< TGCFMD.bit.TEGR_4: Configration start enable flag in group R4 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_r5_config_on_flg;			/**< TGCFMD.bit.TEGR_5: Configration start enable flag in group R5 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_r6_config_on_flg;			/**< TGCFMD.bit.TEGR_6: Configration start enable flag in group R6 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		gr_r7_config_on_flg;			/**< TGCFMD.bit.TEGR_7: Configration start enable flag in group R7 arbiter<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
} T_IM_MXIC_CONFIG_ARBITER;

/** Setting of clock enable */
typedef struct {
	UCHAR		clk_en_slave_1_0;				/**< TCKENP.bit.TCKENP_1: Clock enable of slave-1 port-0 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_1_1;				/**< TCKENP.bit.TCKENP_1: Clock enable of slave-1 port-1 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_1_2;				/**< TCKENP.bit.TCKENP_1: Clock enable of slave-1 port-2 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_1_3;				/**< TCKENP.bit.TCKENP_1: Clock enable of slave-1 port-3 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_2_0;				/**< TCKENP.bit.TCKENP_2: Clock enable of slave-2 port-0 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_2_1;				/**< TCKENP.bit.TCKENP_2: Clock enable of slave-2 port-1 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_2_2;				/**< TCKENP.bit.TCKENP_2: Clock enable of slave-2 port-2 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_2_3;				/**< TCKENP.bit.TCKENP_2: Clock enable of slave-2 port-3 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_3_0;				/**< TCKENP.bit.TCKENP_3: Clock enable of slave-3 port-0 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_3_1;				/**< TCKENP.bit.TCKENP_3: Clock enable of slave-3 port-1 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_3_2;				/**< TCKENP.bit.TCKENP_3: Clock enable of slave-3 port-2 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_3_3;				/**< TCKENP.bit.TCKENP_3: Clock enable of slave-3 port-3 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_4_0;				/**< TCKENP.bit.TCKENP_4: Clock enable of slave-4 port-0 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_4_1;				/**< TCKENP.bit.TCKENP_4: Clock enable of slave-4 port-1 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_4_2;				/**< TCKENP.bit.TCKENP_4: Clock enable of slave-4 port-2 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_slave_4_3;				/**< TCKENP.bit.TCKENP_4: Clock enable of slave-4 port-3 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_group_0;					/**< TGCKEN.bit.TGCKSEN: Clock enable of group-0 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_group_1;					/**< TGCKEN.bit.TGCKSEN: Clock enable of group-1 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_group_2;					/**< TGCKEN.bit.TGCKSEN: Clock enable of group-2 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_group_3;					/**< TGCKEN.bit.TGCKSEN: Clock enable of group-3 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_group_4;					/**< TGCKEN.bit.TGCKSEN: Clock enable of group-4 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_group_5;					/**< TGCKEN.bit.TGCKSEN: Clock enable of group-5 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_group_6;					/**< TGCKEN.bit.TGCKSEN: Clock enable of group-6 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_group_7;					/**< TGCKEN.bit.TGCKSEN: Clock enable of group-7 arbiter.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_dec_err;					/**< TCKEN.bit.TCKSEN: Clock enable of decode error block.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR		clk_en_monitor;					/**< TCKEN.bit.TCKMEN: Clock enable of monitor and memory access function block.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
} T_IM_MXIC_CLOCK;

/** Setting of acceptance capability */
typedef struct {
	UCHAR		w_capability[E_IM_MXIC_W_ARBITER_MAX][E_IM_MXIC_PORT_MAX];				/**< TACPTCW.bit.TACPTCW*: Acceptance capability of write port. (1~8)<br> */
	UCHAR		r_capability[E_IM_MXIC_R_ARBITER_MAX][E_IM_MXIC_PORT_MAX];				/**< TACPTCR.bit.TACPTCR*: Acceptance capability of read port. (1~8)<br> */
	UCHAR		w_capability_gr[E_IM_MXIC_W_ARBITER_GR_MAX][E_IM_MXIC_PORT_GR_MAX];		/**< TGACPTCW.bit.TGACPTCW*: Acceptance capability of write port.(group) (1~8)<br> */
	UCHAR		r_capability_gr[E_IM_MXIC_R_ARBITER_GR_MAX][E_IM_MXIC_PORT_GR_MAX];		/**< TGACPTCR.bit.TGACPTCR*: Acceptance capability of read port.(group) (1~8)<br> */
} T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY;

/** Setting of slave area */
typedef struct {
	UINT32		size;							/**< TREGION.bit.TSIZE: Size of slave area.<br> */
	UINT32		address;						/**< TREGION.bit.TSTAD: Start address of slave area.<br> */
} T_IM_MXIC_SLAVE_AREA;

/** Setting of all slave area */
typedef struct {
	T_IM_MXIC_SLAVE_AREA		slave_area[E_IM_MXIC_SLAVE_NUMBER_MAX];	/**< Setting of all slave area.<br> */
} T_IM_MXIC_ALL_SLAVE_AREA;

/** Setting of decoding error interruption */
typedef struct {
	UCHAR					w_err_int_en;		/**< TMIE.bit.TDEWE: Enable setting of interruption by the decode error of write channel.<br>
													 <ul><li>@ref D_IM_MXIC_ON
														 <li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR					r_err_int_en;		/**< TMIE.bit.TDERE: Enable setting of interruption by the decode error of read channel.<br>
													 <ul><li>@ref D_IM_MXIC_ON
														 <li>@ref D_IM_MXIC_OFF</ul>  */
	IM_MXIC_DEC_ERR_CALLBACK	pCallBack;			/**< Call Back function pointer	for decode error.	*/
} T_IM_MXIC_DEC_ERR_INT;

/** Decode error status */
typedef struct {
	UINT32			w_req_addr;					/**< TDESTW.bit.TDEADW: Decode error demand address of write channel. */
	UINT32			r_req_addr;					/**< TDESTR.bit.TDEADR: Decode error demand address of read channel. */
	UCHAR			w_req_master;				/**< TDESTW.bit.TDEMNW: Decode error demand master of write channel. */
	UCHAR			r_req_master;				/**< TDESTR.bit.TDEMNR: Decode error demand master of read channel. */
} T_IM_MXIC_DEC_ERR;

/** Setting of slot assigned to all ports */
typedef struct {
	E_IM_MXIC_PORT_ASSIGN	w_assign[E_IM_MXIC_W_ARBITER_MAX];	/**< TPORTBW.bit.TPORTBW*: Setting of slot assigned to W ports. */
	E_IM_MXIC_PORT_ASSIGN	r_assign[E_IM_MXIC_R_ARBITER_MAX];	/**< TPORTBR.bit.TPORTBR*: Setting of slot assigned to R ports. */
} T_IM_MXIC_ALL_PORT_ASSIGN;

/** Slot priority level */
typedef struct{
	UCHAR			priority_level_type;		/**< TLVL*.bit.LPL*: Priority level type<br>
													<ul><li>@ref D_IM_MXIC_SLOT_PRIORITY_UPPER
														<li>@ref D_IM_MXIC_SLOT_PRIORITY_LOWWER
														<li>@ref D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY
														<li>@ref D_IM_MXIC_SLOT_PRIORITY_LOWWER_ONLY</ul> */
	UCHAR			upper_priority_level;		/**< TLVL*.bit.LPCCL*: Upper priority ratio<br>
													 The setting priority ranges are from 1 to 64("0" value stands for 64) */
	UCHAR			lower_priority_level;		/**< TLVL*.bit.LPCSL*: Lower priority ratio<br>
													 The setting priority ranges are from 1 to 64("0" value stands for 64) */
} T_IM_MXIC_SLOT_PRIORITY_LEVEL;

/** Slot priority level for each port */
typedef struct{
	// W1 arbiter
	T_IM_MXIC_SLOT_PRIORITY_LEVEL level0[E_IM_MXIC_PORT_MAX];				/**< Setting of level 0 control block for each port */
	T_IM_MXIC_SLOT_PRIORITY_LEVEL level1[E_IM_MXIC_PORT_MAX];				/**< Setting of level 1 control block for each port */
	T_IM_MXIC_SLOT_PRIORITY_LEVEL level2[E_IM_MXIC_PORT_MAX];				/**< Setting of level 2 control block for each port */
} T_IM_MXIC_SLOT_PRIORITY_LEVEL_PORT;

/** Slot priority level in all arbiter */
typedef struct{
	// W arbiter
	T_IM_MXIC_SLOT_PRIORITY_LEVEL_PORT w_level[E_IM_MXIC_W_ARBITER_MAX];	/**< Setting of Write arbiter in all arbiter */
	// R arbiter
	T_IM_MXIC_SLOT_PRIORITY_LEVEL_PORT r_level[E_IM_MXIC_R_ARBITER_MAX];	/**< Setting of Read arbiter in all arbiter */
} T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL;

/** Setting of master transfer mask */
typedef struct{
	USHORT		mask_period;					/**< TRMC*.bit.TRMCVP*: Mask period.<br>
													 The setting mask ranges are from 4 to 4096("0" value stands for 4096) */
	UCHAR		mask_type;						/**< TRMC*.bit.TRM*: Mask type.<br>
													<ul><li>@ref D_IM_MXIC_MASTER_MASK_OFF
														<li>@ref D_IM_MXIC_MASTER_MASK_ON
														<li>@ref D_IM_MXIC_MASTER_MASK_PERIOD_ON</ul> */
} T_IM_MXIC_MASTER_MASK;

/** Service history monitor result of port */
typedef struct {
	UCHAR		history[D_IM_MXIC_MAX_MONITOR_HISTORY_NUM];	/**< THST*.bit.THST*: Service history monitor result. */
} T_IM_MXIC_HISTORY_MONITOR;

/** Service history monitor result of all port */
typedef struct {
	T_IM_MXIC_HISTORY_MONITOR	w_arbiter[E_IM_MXIC_W_ARBITER_MAX][E_IM_MXIC_PORT_MAX];	/**< Service history monitor result of W ports. */
	T_IM_MXIC_HISTORY_MONITOR	r_arbiter[E_IM_MXIC_R_ARBITER_MAX][E_IM_MXIC_PORT_MAX];	/**< Service history monitor result of R ports. */
} T_IM_MXIC_ALL_HISTORY_MONITOR;

/** Slot status monitor result of all arbiter */
typedef struct {
	T_IM_MXIC_W_ARBITER_ASSIGN_PORT	w_arbiter[E_IM_MXIC_W_ARBITER_MAX];					/**< TSLSW.bit.SLOTSW*: Slot status monitor result of W(slave) arbiter. */
	T_IM_MXIC_R_ARBITER_ASSIGN_PORT	r_arbiter[E_IM_MXIC_R_ARBITER_MAX];					/**< TSLSR.bit.SLOTSR*: Slot status monitor result of R(slave) arbiter. */
	T_IM_MXIC_W_ARBITER_ASSIGN_GR	w_arbiter_gr[E_IM_MXIC_W_ARBITER_GR_MAX];			/**< TGSLSW.bit.GSLOTSW_*: Slot status monitor result of W(group) arbiter. */
	T_IM_MXIC_R_ARBITER_ASSIGN_GR	r_arbiter_gr[E_IM_MXIC_R_ARBITER_GR_MAX];			/**< TGSLSR.bit.GSLOTSR_*: Slot status monitor result of R(group) arbiter. */
} T_IM_MXIC_ALL_SLOT_STATUS_MONITOR;

/** All master status monitor result */
typedef struct {
	UCHAR		w_master[D_IM_MXIC_MAX_W_MASTER_NUM];	/**< TMSTW.bit.TMSTW_*: write master request status.	*/
	UCHAR		r_master[D_IM_MXIC_MAX_R_MASTER_NUM];	/**< TMSTR.bit.TMSTR_*: read master request status.	*/
} T_IM_MXIC_MASTER_STATUS_MONITOR;

/** Selection monitor target */
typedef struct {
	UCHAR		monitor_sel;					/**< TMSEL.bit.TMAS*: Selection monitor function.<br>
													<ul><li>@ref D_IM_MXIC_MONITOR_TRANSFER
														<li>@ref D_IM_MXIC_MONITOR_ACCESS</ul> */
	UINT32		monitor_target;					/**< TMSEL.bit.TMSEL*: Selection monitor target from all arbiter or all master.	*/
} T_IM_MXIC_MONITOR_TARGET;

/** Access count or transfer amount monitor parameter */
typedef struct{
	UCHAR					start_condition;							/**< TMMD.bit.TMON: Selection monitor start condition.<br>
																			<ul><li>@ref D_IM_MXIC_MONITOR_START_OFF
																				<li>@ref D_IM_MXIC_MONITOR_START_ON
																				<li>@ref D_IM_MXIC_MONITOR_START_ASSERT
																				<li>@ref D_IM_MXIC_MONITOR_START_TRG</ul> */
	UCHAR					end_condition;								/**< TMMD.bit.TMOFF: Selection monitor end condition.<br>
																			<ul><li>@ref D_IM_MXIC_MONITOR_ENDLESS
																				<li>@ref D_IM_MXIC_MONITOR_END_ASSERT
																				<li>@ref D_IM_MXIC_MONITOR_END_LIMIT</ul> */
	UCHAR					update_enable;								/**< TMMD.bit.TMUP: Monitor update terminal enable.<br>
																			<ul><li>@ref D_IM_MXIC_ON
																				<li>@ref D_IM_MXIC_OFF</ul> */
	UCHAR					update_trigger_interval;					/**< TMMD.bit.TMUPC: Update trigger generation interval.<br>
																			<ul><li>@ref D_IM_MXIC_MONITOR_UPDATE_1
																				<li>@ref D_IM_MXIC_MONITOR_UPDATE_8
																				<li>@ref D_IM_MXIC_MONITOR_UPDATE_16
																				<li>@ref D_IM_MXIC_MONITOR_UPDATE_32
																				<li>@ref D_IM_MXIC_MONITOR_UPDATE_64
																				<li>@ref D_IM_MXIC_MONITOR_UPDATE_128
																				<li>@ref D_IM_MXIC_MONITOR_UPDATE_256
																				<li>@ref D_IM_MXIC_MONITOR_UPDATE_512</ul> */
	UCHAR					update_clear_en;							/**< TMMD.bit.TMCLR: Selection monitor clear by update trigger.<br>
																			<ul><li>@ref D_IM_MXIC_ON
																				<li>@ref D_IM_MXIC_OFF</ul> */
	UCHAR					start_clear_en;								/**< TMMD.bit.TMCLRC: Selection monitor clear by asserting of a monitor start terminal.<br>
																			<ul><li>@ref D_IM_MXIC_ON
																				<li>@ref D_IM_MXIC_OFF</ul> */
	UCHAR					cnt_val;									/**< TMMD.bit.TMRSEL: Selection switch of reading value.<br>
																			<ul><li>@ref D_IM_MXIC_VALUE_WHEN_UPDATE_TRG
																				<li>@ref D_IM_MXIC_TRANS_COUNT_VALUE</ul> */
	UCHAR					limit_access_trans;							/**< TMLIMSEL.bit.TMALMS: Selection limit judgment<br>
																			<ul><li>@ref D_IM_MXIC_MONITOR_TRANSFER
																				<li>@ref D_IM_MXIC_MONITOR_ACCESS</ul> */
	UINT32					limit_detection_target;						/**< TMLIMSEL.bit.TLIMSEL: Selection limit target from all arbiter or all master.<br>
																		<ul><li>@ref D_IM_MXIC_3_W_STAT
																			<li>@ref D_IM_MXIC_3_W_IIP_A
																			<li>@ref D_IM_MXIC_3_W_IIP_B
																			<li>@ref D_IM_MXIC_3_W_IIP_C
																			<li>@ref D_IM_MXIC_3_W_SHDR_A
																			<li>@ref D_IM_MXIC_3_W_SHDR_B
																			<li>@ref D_IM_MXIC_3_W_SHDR_C
																			<li>@ref D_IM_MXIC_3_W_ME
																			<li>@ref D_IM_MXIC_3_W_IPU_A
																			<li>@ref D_IM_MXIC_3_W_IPU_C
																			<li>@ref D_IM_MXIC_3_W_IPU_D
																			<li>@ref D_IM_MXIC_3_W_IPU_E
																			<li>@ref D_IM_MXIC_3_W_DSP_A
																			<li>@ref D_IM_MXIC_3_W_DSP_B
																			<li>@ref D_IM_MXIC_3_W_GPU
																			<li>@ref D_IM_MXIC_3_W_DW
																			<li>@ref D_IM_MXIC_3_W_JPEG
																			<li>@ref D_IM_MXIC_3_W_RAW
																			<li>@ref D_IM_MXIC_3_W_GROUP0_M0
																			<li>@ref D_IM_MXIC_3_W_GROUP0_M1
																			<li>@ref D_IM_MXIC_3_W_ELA
																			<li>@ref D_IM_MXIC_3_W_XCH
																			<li>@ref D_IM_MXIC_3_W_FPT_0
																			<li>@ref D_IM_MXIC_3_W_FPT_1
																			<li>@ref D_IM_MXIC_3_R_STAT
																			<li>@ref D_IM_MXIC_3_R_IIP_A
																			<li>@ref D_IM_MXIC_3_R_IIP_B
																			<li>@ref D_IM_MXIC_3_R_IIP_C
																			<li>@ref D_IM_MXIC_3_R_SHDR_A
																			<li>@ref D_IM_MXIC_3_R_SHDR_B
																			<li>@ref D_IM_MXIC_3_R_SHDR_C
																			<li>@ref D_IM_MXIC_3_R_SHDR_D
																			<li>@ref D_IM_MXIC_3_R_ME
																			<li>@ref D_IM_MXIC_3_R_IPU_A
																			<li>@ref D_IM_MXIC_3_R_IPU_B
																			<li>@ref D_IM_MXIC_3_R_IPU_C
																			<li>@ref D_IM_MXIC_3_R_IPU_D
																			<li>@ref D_IM_MXIC_3_R_DSP_A
																			<li>@ref D_IM_MXIC_3_R_DSP_B
																			<li>@ref D_IM_MXIC_3_R_GPU
																			<li>@ref D_IM_MXIC_3_R_JPEG
																			<li>@ref D_IM_MXIC_3_R_RAW
																			<li>@ref D_IM_MXIC_3_R_RDMA
																			<li>@ref D_IM_MXIC_3_R_GROUP0_M0
																			<li>@ref D_IM_MXIC_3_R_GROUP0_M1
																			<li>@ref D_IM_MXIC_3_R_ELA
																			<li>@ref D_IM_MXIC_3_R_XCH
																			<li>@ref D_IM_MXIC_3_R_FPT_0
																			<li>@ref D_IM_MXIC_3_R_FPT_1
																			<li>@ref D_IM_MXIC_SLAVE_W1_PORT0
																			<li>@ref D_IM_MXIC_SLAVE_W1_PORT1
																			<li>@ref D_IM_MXIC_SLAVE_W1_PORT2
																			<li>@ref D_IM_MXIC_SLAVE_W1_PORT3
																			<li>@ref D_IM_MXIC_SLAVE_W2_PORT0
																			<li>@ref D_IM_MXIC_SLAVE_W2_PORT1
																			<li>@ref D_IM_MXIC_SLAVE_W2_PORT2
																			<li>@ref D_IM_MXIC_SLAVE_W2_PORT3
																			<li>@ref D_IM_MXIC_SLAVE_W3_PORT0
																			<li>@ref D_IM_MXIC_SLAVE_W3_PORT1
																			<li>@ref D_IM_MXIC_SLAVE_W3_PORT2
																			<li>@ref D_IM_MXIC_SLAVE_W3_PORT3
																			<li>@ref D_IM_MXIC_SLAVE_W4_PORT0
																			<li>@ref D_IM_MXIC_SLAVE_W4_PORT1
																			<li>@ref D_IM_MXIC_SLAVE_W4_PORT2
																			<li>@ref D_IM_MXIC_SLAVE_W4_PORT3
																			<li>@ref D_IM_MXIC_SLAVE_R1_PORT0
																			<li>@ref D_IM_MXIC_SLAVE_R1_PORT1
																			<li>@ref D_IM_MXIC_SLAVE_R1_PORT2
																			<li>@ref D_IM_MXIC_SLAVE_R1_PORT3
																			<li>@ref D_IM_MXIC_SLAVE_R2_PORT0
																			<li>@ref D_IM_MXIC_SLAVE_R2_PORT1
																			<li>@ref D_IM_MXIC_SLAVE_R2_PORT2
																			<li>@ref D_IM_MXIC_SLAVE_R2_PORT3
																			<li>@ref D_IM_MXIC_SLAVE_R3_PORT0
																			<li>@ref D_IM_MXIC_SLAVE_R3_PORT1
																			<li>@ref D_IM_MXIC_SLAVE_R3_PORT2
																			<li>@ref D_IM_MXIC_SLAVE_R3_PORT3
																			<li>@ref D_IM_MXIC_SLAVE_R4_PORT0
																			<li>@ref D_IM_MXIC_SLAVE_R4_PORT1
																			<li>@ref D_IM_MXIC_SLAVE_R4_PORT2
																			<li>@ref D_IM_MXIC_SLAVE_R4_PORT3
																			<li>@ref D_IM_MXIC_TARGET_NOTHING</ul> */
	UINT32					limit;										/**< TMLIMIT.bit.TMLIMT: Limiting value of amount of transfer data.<br>
																			 Setting values are from 0 to 256*256*256*256-1(bit). "0" value stands for no limit.<br>
																			 When limit_access_trans is D_IM_MXIC_MONITOR_TRANSFER, please set the value by the unit of 256 bytes.  */
	T_IM_MXIC_MONITOR_TARGET	target[D_IM_MXIC_MAX_MONITOR_TARGET_NUM];	/**< Selection monitor target. */
	// CallBack function
	VP_CALLBACK				pCallBack;									/**< Call Back function pointer.<br>
																		     When the monitor end flag(TMIF.TMF) becomes "1", this callback function is called. */
} T_IM_MXIC_MONITOR_PARAMETER;

/** All access count or transfer amount monitor */
typedef struct {
	UINT32					count[D_IM_MXIC_MAX_MONITOR_TARGET_NUM];		/**< TMCNT.bit.TMCNT: Monitor result of access count or transfer amount monitor */
} T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR;

/** Write master output port */
typedef struct {
	E_IM_MXIC_PORT_GR	ela;				/**< Setting the output port of the JDSELA Master I/F.	*/
	E_IM_MXIC_PORT_GR	xch;				/**< Setting the output port of the JDSXH Master I/F.	*/
	E_IM_MXIC_PORT_GR	fpt_0;				/**< Setting the output port of the FPT Master I/F-0.	*/
	E_IM_MXIC_PORT_GR	fpt_1;				/**< Setting the output port of the FPT Master I/F-1.	*/
}T_IM_MXIC_W_OUTPUT_PORT;

/** Read master output port */
typedef struct {
	E_IM_MXIC_PORT_GR	ela;				/**< Setting the output port of the JDSELA Master I/F.	*/
	E_IM_MXIC_PORT_GR	xch;				/**< Setting the output port of the JDSXH Master I/F.	*/
	E_IM_MXIC_PORT_GR	fpt_0;				/**< Setting the output port of the FPT Master I/F-0.	*/
	E_IM_MXIC_PORT_GR	fpt_1;				/**< Setting the output port of the FPT Master I/F-1.	*/
}T_IM_MXIC_R_OUTPUT_PORT;

/** Output port setting */
typedef struct {
	T_IM_MXIC_W_OUTPUT_PORT	w_out_port;		/**< TPORTMW.bit.TPORTMW_127_64: Write output port setting master channel.	*/
	T_IM_MXIC_R_OUTPUT_PORT	r_out_port;		/**< TPORTMR.bit.TPORTMR_127_64: Read output port setting master channel.	*/
} T_IM_MXIC_OUTPUT_PORT;

/** LevelPort all setting. */
typedef struct{
	E_IM_MXIC_PORT					port[D_IM_MXIC_MASTER_NUMBER_MAX-1];		/**< Target port.	*/
	E_IM_MXIC_LEVEL					level[D_IM_MXIC_MASTER_NUMBER_MAX-1];		/**< Target level.	*/
} T_IM_MXIC_ARBITER_LEVELPORT;

/** LevelPort all setting. */
typedef struct{
	T_IM_MXIC_ARBITER_LEVELPORT		w_arbiter[E_IM_MXIC_W_ARBITER_MAX];		/**< LevelPort in W arbiter.	*/
	T_IM_MXIC_ARBITER_LEVELPORT		r_arbiter[E_IM_MXIC_R_ARBITER_MAX];		/**< LevelPort in R arbiter.	*/
} T_IM_MXIC_ALL_LEVELPORT;

/** Detection parameters of memory access */
typedef struct {
	UCHAR				start_trigger;			/**< TSASETW.bit.TSATRGW: Start trigger of memory access.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UINT64				master47_1;				/**< TSASETW.bit.TSAMASW47_1: Master of memory access.	*/
	UINT64				master127_64;			/**< TSASETW.bit.TSAMASW127_64: Master of memory access.	*/
	UINT32				start_address;			/**< TSASETW.bit.TSASADW: Start address of memory access.	*/
	UINT32				end_address;			/**< TSASETW.bit.TSAEADW: End address of memory access.	*/
	UCHAR				mode;					/**< TSASETW.bit.TSARW: Mode of memory access.<br>
													<ul><li>@ref D_IM_MXIC_ON
														<li>@ref D_IM_MXIC_OFF</ul>  */
	UCHAR				detect_master;			/**< TSASETW.bit.TSAMNSTW: Detection master of memory access.	*/
	UCHAR				detect_address;			/**< TSASETW.bit.TSAADSTW: Detection address of memory access.	*/
} T_IM_MXIC_MEMORY_ACCESS_PARAMETER;

/** Detection parameters of slave each memory access */
typedef struct {
	T_IM_MXIC_MEMORY_ACCESS_PARAMETER	slave[E_IM_MXIC_W_ARBITER_MAX];		/**< TPORTMW.bit.TPORTMW_127_64: Write output port setting master channel.	*/
	VP_CALLBACK							pCallBack;							/**< Call Back function pointer.<br> */
} T_IM_MXIC_MEMORY_ACCESS_SLAVE;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define Im_MXIC_Get_TMIF0		(IO_MXIC0.TMIF.word)				/**< Get Interrupt flag register */
#define Im_MXIC_Get_TDEADW0		(IO_MXIC0.TDESTW.bit.TDEADW)		/**< Get Write channel decode error required address */
#define Im_MXIC_Get_TDEMNW0		(IO_MXIC0.TDESTW.bit.TDEMNW)		/**< Get Write channel decode error required master */
#define Im_MXIC_Get_TDEADR0		(IO_MXIC0.TDESTR.bit.TDEADR)		/**< Get Read channel decode error required address */
#define Im_MXIC_Get_TDEMNR0		(IO_MXIC0.TDESTR.bit.TDEMNR)		/**< Get Read channel decode error required master */
#define Im_MXIC_Get_TMIF1		(IO_MXIC1.TMIF.word)				/**< Get Interrupt flag register */
#define Im_MXIC_Get_TDEADW1		(IO_MXIC1.TDESTW.bit.TDEADW)		/**< Get Write channel decode error required address */
#define Im_MXIC_Get_TDEMNW1		(IO_MXIC1.TDESTW.bit.TDEMNW)		/**< Get Write channel decode error required master */
#define Im_MXIC_Get_TDEADR1		(IO_MXIC1.TDESTR.bit.TDEADR)		/**< Get Read channel decode error required address */
#define Im_MXIC_Get_TDEMNR1		(IO_MXIC1.TDESTR.bit.TDEMNR)		/**< Get Read channel decode error required master */
#define Im_MXIC_Get_TMIF2		(IO_MXIC2.TMIF.word)				/**< Get Interrupt flag register */
#define Im_MXIC_Get_TDEADW2		(IO_MXIC2.TDESTW.bit.TDEADW)		/**< Get Write channel decode error required address */
#define Im_MXIC_Get_TDEMNW2		(IO_MXIC2.TDESTW.bit.TDEMNW)		/**< Get Write channel decode error required master */
#define Im_MXIC_Get_TDEADR2		(IO_MXIC2.TDESTR.bit.TDEADR)		/**< Get Read channel decode error required address */
#define Im_MXIC_Get_TDEMNR2		(IO_MXIC2.TDESTR.bit.TDEMNR)		/**< Get Read channel decode error required master */
#define Im_MXIC_Get_TMIF3		(IO_MXIC3.TMIF.word)				/**< Get Interrupt flag register */
#define Im_MXIC_Get_TDEADW3		(IO_MXIC3.TDESTW.bit.TDEADW)		/**< Get Write channel decode error required address */
#define Im_MXIC_Get_TDEMNW3		(IO_MXIC3.TDESTW.bit.TDEMNW)		/**< Get Write channel decode error required master */
#define Im_MXIC_Get_TDEADR3		(IO_MXIC3.TDESTR.bit.TDEADR)		/**< Get Read channel decode error required address */
#define Im_MXIC_Get_TDEMNR3		(IO_MXIC3.TDESTR.bit.TDEMNR)		/**< Get Read channel decode error required master */
#define Im_MXIC_Get_TMIF4		(IO_MXIC4.TMIF.word)				/**< Get Interrupt flag register */
#define Im_MXIC_Get_TDEADW4		(IO_MXIC4.TDESTW.bit.TDEADW)		/**< Get Write channel decode error required address */
#define Im_MXIC_Get_TDEMNW4		(IO_MXIC4.TDESTW.bit.TDEMNW)		/**< Get Write channel decode error required master */
#define Im_MXIC_Get_TDEADR4		(IO_MXIC4.TDESTR.bit.TDEADR)		/**< Get Read channel decode error required address */
#define Im_MXIC_Get_TDEMNR4		(IO_MXIC4.TDESTR.bit.TDEMNR)		/**< Get Read channel decode error required master */
#define Im_MXIC_Get_TMIF5		(IO_MXIC5.TMIF.word)				/**< Get Interrupt flag register */
#define Im_MXIC_Get_TDEADW5		(IO_MXIC5.TDESTW.bit.TDEADW)		/**< Get Write channel decode error required address */
#define Im_MXIC_Get_TDEMNW5		(IO_MXIC5.TDESTW.bit.TDEMNW)		/**< Get Write channel decode error required master */
#define Im_MXIC_Get_TDEADR5		(IO_MXIC5.TDESTR.bit.TDEADR)		/**< Get Read channel decode error required address */
#define Im_MXIC_Get_TDEMNR5		(IO_MXIC5.TDESTR.bit.TDEMNR)		/**< Get Read channel decode error required master */
#define Im_MXIC_Get_TMIF6		(IO_MXIC6.TMIF.word)				/**< Get Interrupt flag register */
#define Im_MXIC_Get_TDEADW6		(IO_MXIC6.TDESTW.bit.TDEADW)		/**< Get Write channel decode error required address */
#define Im_MXIC_Get_TDEMNW6		(IO_MXIC6.TDESTW.bit.TDEMNW)		/**< Get Write channel decode error required master */
#define Im_MXIC_Get_TDEADR6		(IO_MXIC6.TDESTR.bit.TDEADR)		/**< Get Read channel decode error required address */
#define Im_MXIC_Get_TDEMNR6		(IO_MXIC6.TDESTR.bit.TDEMNR)		/**< Get Read channel decode error required master */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


/**
This function does software reset.<br>
@param [in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Reset( E_IM_MXIC_UNIT unit );

/**
This function set initial value to register.But this function is not set software reset.<br>
@retval			D_DDIM_OK					Success.
*/
extern	INT32	Im_MXIC_Init( E_IM_MXIC_UNIT unit );

/**
This function select arbiters of executed "configration start" and start configuration.<br>
@param [in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param [in]		target_arbiter			Target arbiter of configration start.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Duplication setup of the master number to another port of the same slave including a read channel / write channel is prohibition.<br>
				In this case, this function returns the @ref D_IM_MXIC_INPUT_PARAM_ERROR error.<br>
				And, the allocation setting of the master is returned to the setting operating now.
*/
extern	INT32	Im_MXIC_Start_Config( E_IM_MXIC_UNIT unit, T_IM_MXIC_CONFIG_ARBITER const* const target_arbiter );

/**
This function set clock supply to a functional block. 
@param [in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param [in]		clock					Clock enable setting.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Clock_Enable( E_IM_MXIC_UNIT unit, T_IM_MXIC_CLOCK const* const clock );

/**
This function get clock supply to a functional block. 
@param [in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param [out]	clock					Clock enable setting.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Clock_Enable( E_IM_MXIC_UNIT unit, T_IM_MXIC_CLOCK* const clock );

/**
This function set acceptance capability of the specified port.<br> 
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value. <br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value. <br>
@param[in]		capability				Acceptance capability. (1~8)<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
extern	INT32	Im_MXIC_Set_Acceptance_Capability( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, UCHAR capability );

/**
This function get acceptance capability of the specified port.<br> 
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value. <br>
@param[out]		capability				Acceptance capability. (1~8)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Acceptance_Capability( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, UCHAR* capability);

/**
This function set acceptance capability (gropu) of the specified port.<br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER_GR for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT_GR for a set value. <br>
@param[in]		capability				Acceptance capability. (1~8)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Acceptance_Capability_Group( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER_GR arbiter, E_IM_MXIC_PORT_GR port, UCHAR capability );

/**
This function get acceptance capability (gropu) of the specified port.<br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER_GR for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT_GR for a set value. <br>
@param[out]		capability				Acceptance capability. (1~8)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Acceptance_Capability_Group( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER_GR arbiter, E_IM_MXIC_PORT_GR port, UCHAR* capability );

/**
This function set acceptance capability of all ports.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		all_capability			Acceptance capability of all ports.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Acceptance_Capability_All_Port( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY const* const all_capability );

/**
This function get acceptance capability of all ports.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[out]		all_capability			Acceptance capability of all ports.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Acceptance_Capability_All_Port( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY* const all_capability );

/**
This function set slave area setting of specified slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		slave_number			Slave area number.<br>
										Please refer to @ref E_IM_MXIC_SLAVE_NUMBER for a set value. <br>
@param[in]		slave_area				Slave area setting.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Slave_Area( E_IM_MXIC_UNIT unit, E_IM_MXIC_SLAVE_NUMBER slave_number, T_IM_MXIC_SLAVE_AREA const* const slave_area );

/**
This function get slave area setting of specified slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		slave_number			Slave area number.<br>
										Please refer to @ref E_IM_MXIC_SLAVE_NUMBER for a set value. <br>
@param[out]		slave_area				Slave area setting.<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
extern	INT32	Im_MXIC_Get_Slave_Area( E_IM_MXIC_UNIT unit, E_IM_MXIC_SLAVE_NUMBER slave_number, T_IM_MXIC_SLAVE_AREA* const slave_area );

/**
This function set slave area setting of all slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		all_slave_area			Slave area setting of all slave number.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Slave_Area_All( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLAVE_AREA const* const all_slave_area );

/**
This function get slave area setting of all slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[out]		all_slave_area			Slave area setting of all slave number.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Slave_Area_All( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLAVE_AREA* const all_slave_area );

/**
This function set interruption setting of salave area decode error.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		dec_err_int				Setting of decoding error interruption.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please acquire error status with an Im_MXIC_Get_Decode_Error function 
				at the time of decode error interruption generating. 
*/
extern	INT32	Im_MXIC_Set_Decode_Error_Int( E_IM_MXIC_UNIT unit, T_IM_MXIC_DEC_ERR_INT const* const dec_err_int );

/**
This function get interruption setting of salave area decode error.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[out]		dec_err_int				Setting of decoding error interruption.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Decode_Error_Int( E_IM_MXIC_UNIT unit, T_IM_MXIC_DEC_ERR_INT* const dec_err_int );

/**
This function get decode error status.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[out]		status					Decode error status.<br>
@remarks		Decode error will be notifed by the callback function that set up with the Im_MXIC_Set_Decode_Error_Int function.<br>
				Please acquire error status with Im_MXIC_Get_Decode_Error function at this time.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Decode_Error( E_IM_MXIC_UNIT unit, T_IM_MXIC_DEC_ERR* const status );

/**
This function set the W master to the W arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		arbiter					Target write arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting changed by this function is reflected by the configration start.
@remarks		D_IM_MXIC_W_RESERVE is set to the part where the channel is not set up without fail.
*/
extern	INT32	Im_MXIC_Set_Master_W_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT const* const assign );

/**
This function get the W master from the W arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		arbiter					Target write arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value.<br>
@param[out]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_W_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT* const assign );

/**
This function set the R master to the R arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		arbiter					Target read arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting changed by this function is reflected by the configration start.
@remarks		D_IM_MXIC_R_RESERVE is set to the part where the channel is not set up without fail.
*/
extern	INT32	Im_MXIC_Set_Master_R_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT const* const assign );

/**
This function get the R master from the R arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		arbiter					Target read arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value.<br>
@param[out]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_R_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT* const assign );

/**
This function set the master to the all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		all_assign		 		Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting changed by this function is reflected by the configration start.
*/
extern	INT32	Im_MXIC_Set_Master_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ARBITER_ASSIGN const* const all_assign );

/**
This function get the master from the all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[out]		all_assign		 		Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ARBITER_ASSIGN* const all_assign );

/**
This function set the slot priority level control.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@param[in]		level_number			Target level.<br>
										Please refer to @ref E_IM_MXIC_SLOT_LEVEL_CTRL for a set value.<br>
@param[in]		priority				Priority level control settings.
										Please refer to @ref T_IM_MXIC_SLOT_PRIORITY_LEVEL for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting ranges are from 1 to 64.
*/
extern	INT32	Im_MXIC_Set_Slot_Priority( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT_LEVEL_CTRL level_number, T_IM_MXIC_SLOT_PRIORITY_LEVEL const* const priority );
/**
This function get the slot priority level control.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@param[in]		level_number			Target level.<br>
										Please refer to @ref E_IM_MXIC_SLOT_LEVEL_CTRL for a set value.<br>
@param[in]		priority				Priority level control settings.
										Please refer to @ref T_IM_MXIC_SLOT_PRIORITY_LEVEL for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32 Im_MXIC_Get_Slot_Priority( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT_LEVEL_CTRL level_number, T_IM_MXIC_SLOT_PRIORITY_LEVEL* const priority );
/**
This function set the slot priority level control to all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		all_priority			Slot priority level control setting of all arbiter.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting  ranges are from 1 to 64.
*/
extern	INT32	Im_MXIC_Set_Slot_Priority_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL const* const all_priority );

/**
This function get the slot priority level control from all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[out]		all_priority			Slot priority level control setting of all arbiter.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Slot_Priority_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL* const all_priority );

/**
This function set the movement position after the master process.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		master					Master number.<br>
@param[in]		priority				Master priority level.<br>
										<ul><li>@ref D_IM_MXIC_MASTER_MOVE_LAST
											<li>@ref D_IM_MXIC_MASTER_MOVE_1
											<li>@ref D_IM_MXIC_MASTER_MOVE_2
											<li>@ref D_IM_MXIC_MASTER_MOVE_3
											<li>@ref D_IM_MXIC_MASTER_MOVE_4
											<li>@ref D_IM_MXIC_MASTER_MOVE_5
											<li>@ref D_IM_MXIC_MASTER_MOVE_6
											<li>@ref D_IM_MXIC_MASTER_MOVE_7
											<li>@ref D_IM_MXIC_MASTER_MOVE_8
											<li>@ref D_IM_MXIC_MASTER_MOVE_9
											<li>@ref D_IM_MXIC_MASTER_MOVE_10
											<li>@ref D_IM_MXIC_MASTER_MOVE_11
											<li>@ref D_IM_MXIC_MASTER_MOVE_12
											<li>@ref D_IM_MXIC_MASTER_MOVE_13
											<li>@ref D_IM_MXIC_MASTER_MOVE_14
											<li>@ref D_IM_MXIC_MASTER_MOVE_15</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Master_Priority( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR priority );

/**
This function get the movement position after the master process.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[out]		priority				Master priority level.<br>
										<ul><li>@ref D_IM_MXIC_MASTER_MOVE_LAST
											<li>@ref D_IM_MXIC_MASTER_MOVE_1
											<li>@ref D_IM_MXIC_MASTER_MOVE_2
											<li>@ref D_IM_MXIC_MASTER_MOVE_3
											<li>@ref D_IM_MXIC_MASTER_MOVE_4
											<li>@ref D_IM_MXIC_MASTER_MOVE_5
											<li>@ref D_IM_MXIC_MASTER_MOVE_6
											<li>@ref D_IM_MXIC_MASTER_MOVE_7
											<li>@ref D_IM_MXIC_MASTER_MOVE_8
											<li>@ref D_IM_MXIC_MASTER_MOVE_9
											<li>@ref D_IM_MXIC_MASTER_MOVE_10
											<li>@ref D_IM_MXIC_MASTER_MOVE_11
											<li>@ref D_IM_MXIC_MASTER_MOVE_12
											<li>@ref D_IM_MXIC_MASTER_MOVE_13
											<li>@ref D_IM_MXIC_MASTER_MOVE_14
											<li>@ref D_IM_MXIC_MASTER_MOVE_15</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_Priority( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR* const priority );

/**
This function set the slot mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@param[in]		slot					Target slot number.<br>
										Please refer to @ref E_IM_MXIC_SLOT for a set value. <br>
@param[in]		mask_enable				Slot mask enable.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Slot_Mask( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT slot, UCHAR mask_enable );

/**
This function get the slot mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@param[in]		slot					Target slot number.<br>
										Please refer to @ref E_IM_MXIC_SLOT for a set value. <br>
@param[out]		mask_enable				Slot mask enable.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Slot_Mask( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT slot, UCHAR* const mask_enable );

/**
This function set the master transfer mask and mask period.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[in]		mask					Setting of mask.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting mask ranges are from 4 to 4096.
*/
extern	INT32	Im_MXIC_Set_Master_Mask( E_IM_MXIC_UNIT unit, UCHAR master, T_IM_MXIC_MASTER_MASK const* const mask );

/**
This function get the master transfer mask and mask period.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value.<br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[out]		mask					Setting of mask.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_Mask( E_IM_MXIC_UNIT unit, UCHAR master, T_IM_MXIC_MASTER_MASK* const mask );

/**
This function set factor of transfer demand mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[in]		group					Mask group.<br>
										Please refer to @ref E_IM_MXIC_MASK_GROUP for a set value.<br>
@param[in]		enable					Setting of whether to make a master into a mask factor.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Master_Mask_Factor( E_IM_MXIC_UNIT unit, UCHAR master, E_IM_MXIC_MASK_GROUP group, UCHAR enable );

/**
This function get factor of transfer demand mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[in]		group					Mask group.<br>
										Please refer to @ref E_IM_MXIC_MASK_GROUP for a set value.<br>
@param[out]		enable					Setting of whether to make a master into a mask factor.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_Mask_Factor( E_IM_MXIC_UNIT unit, UCHAR master, E_IM_MXIC_MASK_GROUP group, UCHAR* const enable );

/**
This function set target of transfer demand mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[in]		group					Mask group.<br>
										Please refer to @ref E_IM_MXIC_MASK_GROUP for a set value.<br>
@param[in]		enable					Setting of whether to make a master into a mask target.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Master_Mask_Target( E_IM_MXIC_UNIT unit, UCHAR master, E_IM_MXIC_MASK_GROUP group, UCHAR enable );

/**
This function get target of transfer demand mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[in]		group					Mask group.<br>
										Please refer to @ref E_IM_MXIC_MASK_GROUP for a set value.<br>
@param[out]		enable					Setting of whether to make a master into a mask target.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_Mask_Target( E_IM_MXIC_UNIT unit, UCHAR master, E_IM_MXIC_MASK_GROUP group, UCHAR* const enable );

/**
This function set the mask factor master state.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[in]		select					Master state select.(0:master address ch demand period ,1:master address ch demand to data transfer complete)<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Master_Mask_Select( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR select );

/**
This function get the mask factor master state.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[out]		select					Master state select.(0:master address ch demand period ,1:master address ch demand to data transfer complete)<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_Mask_Select( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR* const select );

/**
This function set the max number of master transaction.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[in]		transaction				Master transaction.(0~63)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Master_Transaction( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR transaction );

/**
This function get the max number of master transaction.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number. See master parameter of function in Im_MXIC_Set_Master_Priority.<br>
@param[out]		transaction				Master transaction.(0~63)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Master_Transaction( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR* const transaction );

/**
This function set the LevelPort(Port).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		master					Master number.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_LevelPort_Port( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, UCHAR master, E_IM_MXIC_PORT port );

/**
This function get the LevelPort(Port).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		master					Master number.<br>
@param[out]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_LevelPort_Port( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, UCHAR master, E_IM_MXIC_PORT* const port );

/**
This function get the LevelPort(Level).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		master					Master number.<br>
@param[out]		level					Target Level.<br>
										Please refer to @ref E_IM_MXIC_LEVEL for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_LevelPort_Level( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, UCHAR master, E_IM_MXIC_LEVEL* const level );

/**
This function set the LevelPort(Level).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		master					Master number.<br>
@param[in]		level					Target Level.<br>
										Please refer to @ref E_IM_MXIC_LEVEL for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_LevelPort_Level( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, UCHAR master, E_IM_MXIC_LEVEL level );

/**
This function set the LevelPort(All).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		all_levelport			All LevelPort information.<br>
										Please refer to @ref T_IM_MXIC_ALL_LEVELPORT.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_MXIC_Set_LevelPort_All( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_LEVELPORT* all_levelport );

/**
This function get the LevelPort(All).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_levelport			All LevelPort information.<br>
										Please refer to @ref T_IM_MXIC_ALL_LEVELPORT.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern INT32 Im_MXIC_Get_LevelPort_All( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_LEVELPORT* all_levelport );

/**
"Service history monitor" and "Access count or amount of transfer monitor" are started with this function.<br>
*/
extern	VOID	Im_MXIC_Start_Monitor( VOID );

/**
"Service history monitor" and "Access count or amount of transfer monitor" are stoped with this function.<br>
*/
extern	VOID	Im_MXIC_Stop_Monitor( VOID );

/**
Set whether to stop service history monitor operation in a THSTOP terminal input.<br>
@param[in]		enable					Setting of whether to stop service history monitor operation in a THSTOP terminal input.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_History_Monitor_Stop_Enable( UCHAR enable );

/**
Get whether to stop service history monitor operation in a THSTOP terminal input.<br>
@param[out]		enable					Setting of whether to stop service history monitor operation in a THSTOP terminal input.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_History_Monitor_Stop_Enable( UCHAR* const enable );

/**
The service history monitor result of the specified port is acquired. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value. <br>
@param[out]		history					Result of service history monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the port for a monitor.</ul>
*/
extern	INT32	Im_MXIC_Get_History_Monitor( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, T_IM_MXIC_HISTORY_MONITOR* const history );

/**
The service history monitor result of the all ports is acquired. <br>
@param[out]		all_history				Result of service history monitor.<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the port for a monitor.</ul>
*/
extern	INT32	Im_MXIC_Get_History_Monitor_All_Port(T_IM_MXIC_ALL_HISTORY_MONITOR* const all_history);

/**
The slot status monitor result of the specified write channel arbiter is acquired. <br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		arbiter					Target write channel arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value. <br>
@param[out]		status					Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
extern	INT32	Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT* const status );

/**
The slot status monitor result of the specified read channel arbiter is acquired. <br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		arbiter					Target read channel arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value. <br>
@param[out]		status					Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
extern	INT32	Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT* const status );

/**
The slot status monitor result of the specified write channel arbiter is acquired. (group)<br>
@param[in]		arbiter					Target write channel arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER_GR for a set value. <br>
@param[out]		status					Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
extern	INT32	Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group( E_IM_MXIC_W_ARBITER_GR arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_GR* const status );

/**
The slot status monitor result of the specified read channel arbiter is acquired. (group)<br>
@param[in]		arbiter					Target read channel arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER_GR for a set value. <br>
@param[out]		status					Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
extern	INT32	Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group( E_IM_MXIC_R_ARBITER_GR arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_GR* const status );

/**
The slot status monitor result of all arbiter is acquired. <br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_status				Result of all slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
extern	INT32	Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLOT_STATUS_MONITOR* const all_status );

/**
The master status monitor result is acquired.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		status					Result of all master status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The master number corresponds to the index of "status.w_master" and "status.r_master" array.<br>
				An example is shown below.<br>
				JDSPRO I/F-0 Write master status = status.w_master[D_IM_MXIC_W_PRO_IF_0]<br>
				JDSDISP I/F-0 Read master status = status.r_master[D_IM_MXIC_R_DISP_IF_0]<br>
*/
extern	INT32 Im_MXIC_Get_Master_Status_Monitor( E_IM_MXIC_UNIT unit, T_IM_MXIC_MASTER_STATUS_MONITOR* const status );

/**
The master status monitor result is acquired.(group)<br>
@param[out]		status					Result of all master status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The master number corresponds to the index of "status.w_master" and "status.r_master" array.<br>
				An example is shown below.<br>
				JDSPRO I/F-0 Write master status = status.w_master[D_IM_MXIC_W_PRO_IF_0]<br>
				JDSDISP I/F-0 Read master status = status.r_master[D_IM_MXIC_R_DISP_IF_0]<br>
*/
extern	INT32 Im_MXIC_Get_Master_Status_Monitor_Group( T_IM_MXIC_MASTER_STATUS_MONITOR* const status );

/**
This function set the parameter of access count or transfer amount monitor.<br>
@param[in]		param					Setting of monitor operation.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Change of a parameter is reflected at the time of the monitor start by @ref Im_MXIC_Start_Monitor function. 
*/
extern	INT32	Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter( T_IM_MXIC_MONITOR_PARAMETER const* const param );

/**
This function get the parameter of access count or transfer amount monitor.<br>
@param[out]		param					Setting of monitor operation.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter( T_IM_MXIC_MONITOR_PARAMETER* const param );

/**
This function get access count or transfer amount monitor result of specified entry.<br>
@param[in]		entry					Monitor number. (0~3)
@param[out]		result					Monitor result.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either when "cnt_val" setting is "D_IM_MXIC_TRANS_COUNT_VALUE".<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the monitor target master(port).</ul>
				<br>
				And, please use this function in the state of fulfilling all the following conditions when "cnt_val" setting is "D_IM_MXIC_VALUE_WHEN_UPDATE_TRG".<br>
				<ul><li>Under a monitor operation processing.
					<li>Timing which the updating trigger generated.</ul>
*/
extern	INT32	Im_MXIC_Get_Access_Or_Trans_Monitor( UCHAR entry, UINT32* const result );

/**
This function get access count or transfer amount monitor result of all entry.<br>
@param[out]		all_result				Monitor result of all entry.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either when "cnt_val" setting is "D_IM_MXIC_TRANS_COUNT_VALUE".<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the monitor target master(port).</ul>
				<br>
				And, please use this function in the state of fulfilling all the following conditions when "cnt_val" setting is "D_IM_MXIC_VALUE_WHEN_UPDATE_TRG".<br>
				<ul><li>Under a monitor operation processing.
					<li>Timing which the updating trigger generated.</ul>
*/
extern	INT32	Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry( T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR* const all_result );

/**
The state of access count or transfer amount monitor is acquired.<br>
Please use this function in only the following the monitor end condition.<br>
- Monitor is ended by the amount limit judging of transmission. (D_IM_MXIC_MONITOR_END_LIMIT)<br><br>
@param[out]		mon_state				Moniter limit end state.<br>
										Please refer to the following for moniter processing state.<br>
										<ul><li>@ref D_IM_MXIC_MONITOR_OPERATION_PROCESS
											<li>@ref D_IM_MXIC_MONITOR_OPERATION_END</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State( UCHAR* mon_state );

/**
Detection of Memory Access Area" are started with this function.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		param					Setting of detection parameters of slave each memory access.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Start_Memory_Access_Detect( E_IM_MXIC_UNIT unit, T_IM_MXIC_MEMORY_ACCESS_SLAVE  const* const param );

/**
Detection of Memory Access Area" are stoped with this function.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Stop_Memory_Access_Detect( E_IM_MXIC_UNIT unit );

/**
This function set the parameter of Detection parameters of slave each memory access.<br>
@param[in]		unit					Target unit number.<br>
@param[in]		param					Setting of detection parameters of slave each memory access.<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
@remarks		Change of a parameter is reflected at the time of the detection start by @ref Im_MXIC_Start_Memory_Access_Detect function. 
*/
extern	INT32	Im_MXIC_Set_Memory_Access_Detect( E_IM_MXIC_UNIT unit, T_IM_MXIC_MEMORY_ACCESS_SLAVE const* const param );

/**
This function get the parameter of Detection parameters of slave each memory access.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		param					Setting of detection parameters of slave each memory access.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Change of a parameter is reflected at the time of the detection start by @ref Im_MXIC_Start_Memory_Access_Detect function. 
*/
extern	INT32	Im_MXIC_Get_Memory_Access_Detect( E_IM_MXIC_UNIT unit, T_IM_MXIC_MEMORY_ACCESS_SLAVE* const param );

/**
Set switch of register read value.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		sw						Switching of the register to be read.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		When set to ON, the read value of the register the following switches to the value of the internal circuit.<br>
					<ul><li>TREGION
						<li>TLVLW, TLVLR
						<li>TPORTMW, TPORTMR
						<li>TACPTCW, TACPTCR
						<li>TGACPTCW, TGACPTCR
						<li>TSLSW, TSLSR
						<li>TGSLSW, TGSLSR</ul>
*/
extern	INT32	Im_MXIC_Set_Reg_Read_Switch( E_IM_MXIC_UNIT unit, UCHAR sw );

/**
Get switch of register read value.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		sw						Switching of the register to be read.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Reg_Read_Switch( E_IM_MXIC_UNIT unit, UCHAR *const sw );

/**
This function set output port.<br>
@param[in]		target						Setting target master.<br>
											<ul><li>@ref D_IM_MXIC_3_W_ELA
												<li>@ref D_IM_MXIC_3_W_XCH
												<li>@ref D_IM_MXIC_3_W_FPT_0
												<li>@ref D_IM_MXIC_3_W_FPT_1
												<li>@ref D_IM_MXIC_3_R_ELA
												<li>@ref D_IM_MXIC_3_R_XCH
												<li>@ref D_IM_MXIC_3_R_FPT_0
												<li>@ref D_IM_MXIC_3_R_FPT_1
@param[in]		output_port					Setting of output port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_TMI_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Output_Port( UCHAR target, E_IM_MXIC_PORT_GR output_port );

/**
This function get output port.<br>
@param[in]		target						Setting target master.<br>
											<ul><li>@ref D_IM_MXIC_3_W_ELA
												<li>@ref D_IM_MXIC_3_W_XCH
												<li>@ref D_IM_MXIC_3_W_FPT_0
												<li>@ref D_IM_MXIC_3_W_FPT_1
												<li>@ref D_IM_MXIC_3_R_ELA
												<li>@ref D_IM_MXIC_3_R_XCH
												<li>@ref D_IM_MXIC_3_R_FPT_0
												<li>@ref D_IM_MXIC_3_R_FPT_1
@param[out]		output_port					Setting of output port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_TMI_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Output_Port( UCHAR target, E_IM_MXIC_PORT_GR* const output_port );

/**
This function set output port.<br>
@param[in]		out_port					Setting of output port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_TMI_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Set_Output_Port_All( T_IM_MXIC_OUTPUT_PORT const* const out_port );

/**
This function get output port.<br>
@param[in]		out_port					Setting of output port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_TMI_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
extern	INT32	Im_MXIC_Get_Output_Port_All( T_IM_MXIC_OUTPUT_PORT *const out_port );

/**
Interruption handler of MXIC.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
*/
extern	VOID	Im_MXIC_Int_Handler( E_IM_MXIC_UNIT unit );

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __IM_MXIC_H__

/*@}*/

/**
@weakgroup im_mxic
@{

@section im_mxic_sample_section1	Sample of "Start MXIC macro".
@code
// SAMPLE CODE //
INT32 mxic_start_sample( E_IM_MXIC_UNIT unit )
{
	INT32						result;
	T_IM_MXIC_CONFIG_ARBITER	config_arbiter;			// Setting of configration start enable
	T_IM_MXIC_CLOCK				clock_enable;			// Setting of clock enable

	// Initialization and a recommendation setup are performed.
	result = Im_MXIC_Init( unit );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	// Clock setting 
	clock_enable.clk_en_slave_1_0	= D_IM_MXIC_ON;
	clock_enable.clk_en_slave_2_0	= D_IM_MXIC_ON;
	clock_enable.clk_en_dec_err		= D_IM_MXIC_OFF;
	clock_enable.clk_en_monitor		= D_IM_MXIC_OFF;

	result = Im_MXIC_Set_Clock_Enable( unit, &clock_enable );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	// (*)Please change only a setup to change with an Im_MXIC_Set_XX function here.

	// Configration start
	config_arbiter.sl_w1_config_on_flg = D_IM_MXIC_ON;
	config_arbiter.sl_w2_config_on_flg = D_IM_MXIC_ON;
	config_arbiter.sl_r1_config_on_flg = D_IM_MXIC_OFF;
	config_arbiter.sl_r2_config_on_flg = D_IM_MXIC_OFF;

	result = Im_MXIC_Start_Config( unit, &config_arbiter );
	if (result != D_DDIM_OK) {
		// Error processing
		return result;
	}

	return result;
}
@endcode


@section im_mxic_sample_section2	Sample of "Service history monitor".
@code
// SAMPLE CODE //
INT32 mxic_service_history_monitor_sample( E_IM_MXIC_UNIT unit )
{
	INT32 result;
	T_IM_MXIC_CONFIG_ARBITER		config_arbiter;		// Setting of configration start enable
	T_IM_MXIC_CLOCK					clock_enable;		// Setting of clock enable
	T_IM_MXIC_ALL_HISTORY_MONITOR	all_history;		// Monitor result of service history.

	// Initialization and a recommendation setup are performed.
	result = Im_MXIC_Init( unit );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	// Clock setting 
	clock_enable.clk_en_slave_1_0	= D_IM_MXIC_ON;
	clock_enable.clk_en_slave_2_0	= D_IM_MXIC_ON;
	clock_enable.clk_en_dec_err		= D_IM_MXIC_OFF;
	clock_enable.clk_en_monitor		= D_IM_MXIC_ON;		// Clock is supplied to a monitor block. 

	result = Im_MXIC_Set_Clock_Enable( unit, &clock_enable );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	// (*)Please change only a setup to change with an Im_MXIC_Set_XX function here.

	// Disable monitor stop by THSTOP assert.
	result = Im_MXIC_Set_History_Monitor_Stop_Enable( D_IM_MXIC_OFF );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	// Configration start
	config_arbiter.sl_w1_config_on_flg = D_IM_MXIC_ON;
	config_arbiter.sl_w2_config_on_flg = D_IM_MXIC_ON;
	config_arbiter.sl_r1_config_on_flg = D_IM_MXIC_OFF;
	config_arbiter.sl_r2_config_on_flg = D_IM_MXIC_OFF;

	result = Im_MXIC_Start_Config( unit, &config_arbiter );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	.
	.
	.

	// Monitor is started to arbitrary timing.
	Im_MXIC_Start_Monitor();

	.
	.
	.

	// Monitor is stopped to arbitrary timing.
	Im_MXIC_Stop_Monitor();

	// Get monitor result of service history.
	result = Im_MXIC_Get_History_Monitor_All_Port( &all_history );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	return result;
}
@endcode


@section im_mxic_sample_section3	Sample of "Master status monitor".
@code
// SAMPLE CODE //
INT32 mxic_master_status_monitor_sample( E_IM_MXIC_UNIT unit )
{
	INT32							result;
	T_IM_MXIC_CONFIG_ARBITER		config_arbiter;		// Setting of configration start enable
	T_IM_MXIC_CLOCK					clock_enable;		// Setting of clock enable
	T_IM_MXIC_MASTER_STATUS_MONITOR	status;				// Monitor result of master status.

	// Initialization and a recommendation setup are performed.
	result = Im_MXIC_Init( unit );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	// Clock setting 
	clock_enable.clk_en_slave_1_0	= D_IM_MXIC_ON;
	clock_enable.clk_en_slave_2_0	= D_IM_MXIC_ON;
	clock_enable.clk_en_dec_err		= D_IM_MXIC_OFF;
	clock_enable.clk_en_monitor		= D_IM_MXIC_ON;		// Clock is supplied to a monitor block. 

	result = Im_MXIC_Set_Clock_Enable( unit, &clock_enable );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	// (*)Please change only a setup to change with an Im_MXIC_Set_XX function here.

	// Configration start
	config_arbiter.sl_w1_config_on_flg = D_IM_MXIC_ON;
	config_arbiter.sl_w2_config_on_flg = D_IM_MXIC_ON;
	config_arbiter.sl_r1_config_on_flg = D_IM_MXIC_OFF;
	config_arbiter.sl_r2_config_on_flg = D_IM_MXIC_OFF;

	result = Im_MXIC_Start_Config( unit, &config_arbiter );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	.
	.
	.

	return result;
}
@endcode


@section im_mxic_sample_section4	Sample of "Access count monitor".
	The precondition of the this sample is as follows.<br>
	<ul>
	  <li>MXIC macro has already been executed.</li>
	  <li>Clock is supplied to a monitor block by Im_MXIC_Set_Clock_Enable function.</li>
	</ul>
@code
// SAMPLE CODE //
INT32 mxic_access_monitor_sample( VOID )
{
	INT32						result;
	T_IM_MXIC_MONITOR_PARAMETER	monitor_param;

	// Set monitor parameter
	monitor_param.start_condition			= D_IM_MXIC_MONITOR_START_TRG;
	monitor_param.end_condition				= D_IM_MXIC_MONITOR_END_LIMIT;
	monitor_param.update_enable				= D_IM_MXIC_OFF;
	monitor_param.update_trigger_interval	= D_IM_MXIC_MONITOR_UPDATE_1;
	monitor_param.update_clear_en			= D_IM_MXIC_OFF;
	monitor_param.start_clear_en			= D_IM_MXIC_OFF;
	monitor_param.cnt_val					= D_IM_MXIC_TRANS_COUNT_VALUE;
	monitor_param.limit_access_trans		= D_IM_MXIC_MONITOR_ACCESS;
	monitor_param.limit_detection_target	= D_IM_MXIC_3_R_XCH;
	monitor_param.limit						= 0x08000000;
	monitor_param.target[0].monitor_sel		= D_IM_MXIC_MONITOR_ACCESS;
	monitor_param.target[1].monitor_sel		= D_IM_MXIC_MONITOR_TRANSFER;
	monitor_param.target[2].monitor_sel		= D_IM_MXIC_MONITOR_TRANSFER;
	monitor_param.target[3].monitor_sel		= D_IM_MXIC_MONITOR_TRANSFER;
	monitor_param.target[0].monitor_target	= D_IM_MXIC_3_R_XCH;
	monitor_param.target[1].monitor_target	= D_IM_MXIC_3_W_FPT_0;
	monitor_param.target[2].monitor_target	= D_IM_MXIC_3_R_FPT_0;
	monitor_param.target[3].monitor_target	= D_IM_MXIC_3_W_XCH;
	monitor_param.pCallBack					= mxic_access_monitor_sample_callback;

	result = Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter( &monitor_param );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return result;
	}

	.
	.
	.

	// Monitor is started to arbitrary timing.
	Im_MXIC_Start_Monitor();

	return result;
}

VOID mxic_access_monitor_sample_callback( VOID )
{
	INT32									result;
	T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR	monitor_result;

	// Get monitor result.
	result = Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry( &monitor_result );
	if ( result != D_DDIM_OK ) {
		// Error processing
		return;
	}
	return;
}
@endcode

*/

/*@}*/
