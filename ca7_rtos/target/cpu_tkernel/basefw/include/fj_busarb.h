/**
 * @file		fj_busarb.h
 * @brief		BUS Arbitration API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __FJ_BUSARB_H__
#define __FJ_BUSARB_H__

#include "fj_std.h"
#include "ddim_typedef.h"


///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_busarb_overview
@{
	The content of this chapter is described set AXI bus master and AXI bus slave (SDRAM controller) arbitration related APIs.<br>
	<br>
	<hr>
	
	@section fj_busarb_block Specification of Summary block diagram of image macro
	- MXIC arbitration control the Data transfer between each image Macro and SDRAMC.<br>
	- SDRAMC is arbitration control the Data transfer of SDRAM.<br>
	<br>
	@image html fj_busarb_block.png
	<hr>
	
	@section fj_busarb_mxic Specification of Summary block diagram of MXIC
	- Priority control of the channel in the slot.<br>
	- Priority control between slots.<br>
	- Assign the port to the master(image macro) of the group arbitration.<br>
	- Set number of transactions for masking the master(image macro).<br>
	<br>
	MXIC configuration<br>
	<table>
		<tr>
		<th>Type</th>
		<th>Quantity</th>
		</tr>
		<tr>
		<td>
			Unit
		</td>
		<td>
			7 Units
		</td>
		</tr>
		<tr>
		<td>
			Arbitration
		</td>
		<td>
			4 Write Arbitration<br>
			4 Read Arbitration
		</td>
		</tr>
		<tr>
		<td>
			Group Arbitration
		</td>
		<td>
			8 Write Arbitration<br>
			8 Read Arbitration
		</td>
		</tr>
		<tr>
		<td>
			Port
		</td>
		<td>
			3 Port for each Arbitration
		</td>
		</tr>
		<tr>
		<td>
			Slot
		</td>
		<td>
			4 Slot for each Port, for each Arbitration
		</td>
		</tr>
		<tr>
		<td>
			Channel
		</td>
		<td>
			8 Channel for each Slot
		</td>
		</tr>
	</table>
	<br>
	-# Channel priority
	<br>
	Please call FJ_BusArb_Set_MxicMasterAllArbiter() function is to set.<br>
	Channel priority is ch0 > ch1 > ch2 > ch3 > ch4 > ch5 > ch6 > ch7.<br>
	Assign the master(image macro) to Channel.<br>
	<br>
	-# Slots priority
	<br>
	Please call FJ_BusArb_Set_MxicSlotPriorityAllArbiter() function is to set.<br>
	<br>
	Priority select.<br>
	<table>
		<tr>
		<th>Value</th>
		<th>Content</th>
		</tr>
		<tr>
		<td>
			D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER
		</td>
		<td>
			Control the priority in the priority ratio.<br>
			Immediately after setting the upper slot.
		</td>
		</tr>
		<tr>
		<td>
			D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LOWWER
		</td>
		<td>
			Control the priority in the priority ratio.<br>
			Immediately after setting the lower slot.
		</td>
		</tr>
		<tr>
		<td>
			D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY
		</td>
		<td>
			Priority the upper slot fixed.
		</td>
		</tr>
		<tr>
		<td>
			D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LOWWER_ONLY
		</td>
		<td>
			Priority the lower slot fixed
		</td>
		</tr>
	</table>
	<br>
	Priority raito. (1 - 64)<br>
	<table>
		<tr>
		<th>Content</th>
		</tr>
		<tr>
		<td>
			Upper slot : Lower slot = (1-64) : (1-64)
		</td>
		</tr>
	</table>
	<br>
	-# Assign the port
	<br>
	Please call FJ_BusArb_Set_MxicOutputPortAll() function is to set.<br>
	Priority is determined by the settings of the group master.<br>
	<br>
	Port select. (0 or 1)<br>
	<table>
		<tr>
		<th>Value</th>
		<th>Content</th>
		</tr>
		<tr>
		<td>
			0
		</td>
		<td>
			Port0
		</td>
		</tr>
		<tr>
		<td>
			1
		</td>
		<td>
			Port1
		</td>
		</tr>
	</table>
	<br>
	-# Masking the master
	<br>
	Please call FJ_BusArb_Set_MxicMasterTransaction() function is to set.<br>
	Master request is masked when it reaches the number of transactions.<br>
	Number of transactions. (1 - 64)
	@image html fj_busarb_mxic.png
	<hr>
	
	@section fj_busarb_sdramc Specification of Summary block diagram of SDRAMC
	- SDRAMC unit two channels. (0/1)<br>
	- Arbitration unit thirty. (MX10/MX11/MX12/MX13/MX20/MX21/MX22/MX23/MX30/MX31/MX32/MX33/MX40/MX50/MX51/MX00/MX01/MX60/RB0/RB1/RB2/DS0/DS1/SDM/VE0/VE1/VE2/VD0/VD1/VD2)<br>
	- Set the throughput of the arbitration unit.<br>
	- Set the priority of the arbitration unit.<br>
	<br>
	-# Arbitration throughput
	<br>
	Please call FJ_BusArb_Set_Sdramc_Cmd_Arbitration_Rate() function is to set.<br>
	<br>
	Average interval. (0 to 4095)<br>
	<br>
	-# Arbitration priority
	<br>
	Please call FJ_BusArb_Set_Sdramc_Cmd_Arbitration_Priority() function is to set.<br>
	<br>
	Level of priority. (0 - 29)<br>
	<br>
	@image html fj_busarb_sdramc.png
@}*//* --- end of fj_busarb_overview */


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** @weakgroup fj_busarb_definition
@{*/

// Slot priority level
#define D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER		(0)			/**< Slot priority level.<br> The priority is on upper level slot */
#define D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LOWWER		(1)			/**< Slot priority level.<br> The priority is on lower level slot */
#define D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY	(2)			/**< Slot priority level.<br> The priority is fixed upper level slot */
#define D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LOWWER_ONLY	(3)			/**< Slot priority level.<br> The priority is fixed lower level slot */

// Slot size
#define D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8				(8)			/**< Slot size.<br> 8 columns in slot */

// MXIC common master number (slave)
#define D_FJ_BUS_ARB_MXIC_W_RESERVE					(0x00)		/**< Write master number.<br> Reserve.			(W00). */
#define D_FJ_BUS_ARB_MXIC_R_RESERVE					(0x80)		/**< Read master number.<br> Reserve.			(R00). */

// MXIC0 Write master number (slave)
#define D_FJ_BUS_ARB_MXIC_0_W_SEN_0					(0x01)		/**< Write master number.<br> SEN0				(W01). */
#define D_FJ_BUS_ARB_MXIC_0_W_SEN_1					(0x02)		/**< Write master number.<br> SEN1				(W02). */
#define D_FJ_BUS_ARB_MXIC_0_W_SEN_2					(0x03)		/**< Write master number.<br> SEN2				(W03). */
#define D_FJ_BUS_ARB_MXIC_0_W_SEN_3					(0x04)		/**< Write master number.<br> SEN3				(W04). */

// MXIC1 Write master number (slave)
#define D_FJ_BUS_ARB_MXIC_1_W_SRO					(0x01)		/**< Write master number.<br> SRO				(W01). */
#define D_FJ_BUS_ARB_MXIC_1_W_B2B					(0x02)		/**< Write master number.<br> B2B				(W02). */
#define D_FJ_BUS_ARB_MXIC_1_W_B2R_A					(0x03)		/**< Write master number.<br> B2RA				(W03). */
#define D_FJ_BUS_ARB_MXIC_1_W_B2R_B					(0x04)		/**< Write master number.<br> B2RB				(W04). */
#define D_FJ_BUS_ARB_MXIC_1_W_B2R_C					(0x05)		/**< Write master number.<br> B2RC				(W05). */
#define D_FJ_BUS_ARB_MXIC_1_W_LTMDT					(0x06)		/**< Write master number.<br> LTMDT				(W06). */
#define D_FJ_BUS_ARB_MXIC_1_W_NONE_7				(0x07)		/**< Write master number.<br> none				(W07). */
#define D_FJ_BUS_ARB_MXIC_1_W_NONE_8				(0x08)		/**< Write master number.<br> none				(W08). */
#define D_FJ_BUS_ARB_MXIC_1_W_NONE_9				(0x09)		/**< Write master number.<br> none				(W09). */
#define D_FJ_BUS_ARB_MXIC_1_W_R2Y_A					(0x0A)		/**< Write master number.<br> R2YA				(W10). */
#define D_FJ_BUS_ARB_MXIC_1_W_R2Y_B					(0x0B)		/**< Write master number.<br> R2YB				(W11). */
#define D_FJ_BUS_ARB_MXIC_1_W_R2Y_C					(0x0C)		/**< Write master number.<br> R2YC				(W12). */
#define D_FJ_BUS_ARB_MXIC_1_W_R2Y_D					(0x0D)		/**< Write master number.<br> R2YD				(W13). */
#define D_FJ_BUS_ARB_MXIC_1_W_CNR_A					(0x0E)		/**< Write master number.<br> CNRA				(W14). */
#define D_FJ_BUS_ARB_MXIC_1_W_CNR_B					(0x0F)		/**< Write master number.<br> CNRB				(W15). */

// MXIC1 Read master number (slave)
#define D_FJ_BUS_ARB_MXIC_1_R_SRO					(0x81)		/**< Read master number.<br> SRO				(R01). */
#define D_FJ_BUS_ARB_MXIC_1_R_B2B					(0x82)		/**< Read master number.<br> B2B				(R02). */
#define D_FJ_BUS_ARB_MXIC_1_R_B2R_A					(0x83)		/**< Read master number.<br> B2RA				(R03). */
#define D_FJ_BUS_ARB_MXIC_1_R_NONE_4				(0x84)		/**< Read master number.<br> none				(R04). */
#define D_FJ_BUS_ARB_MXIC_1_R_NONE_5				(0x85)		/**< Read master number.<br> none				(R05). */
#define D_FJ_BUS_ARB_MXIC_1_R_LTMDT					(0x86)		/**< Read master number.<br> LTMDT				(R06). */
#define D_FJ_BUS_ARB_MXIC_1_R_LTM_R					(0x87)		/**< Read master number.<br> LTMR				(R07). */
#define D_FJ_BUS_ARB_MXIC_1_R_LTM_G					(0x88)		/**< Read master number.<br> LTMG				(R08). */
#define D_FJ_BUS_ARB_MXIC_1_R_LTM_B					(0x89)		/**< Read master number.<br> LTMB				(R09). */
#define D_FJ_BUS_ARB_MXIC_1_R_R2Y_A					(0x8A)		/**< Read master number.<br> R2YA				(R10). */
#define D_FJ_BUS_ARB_MXIC_1_R_R2Y_B					(0x8B)		/**< Read master number.<br> R2YB				(R11). */
#define D_FJ_BUS_ARB_MXIC_1_R_R2Y_C					(0x8C)		/**< Read master number.<br> R2YC				(R12). */
#define D_FJ_BUS_ARB_MXIC_1_R_NONE_13				(0x8D)		/**< Read master number.<br> none				(R13). */
#define D_FJ_BUS_ARB_MXIC_1_R_CNR_B					(0x8E)		/**< Read master number.<br> CNRB				(R14). */
#define D_FJ_BUS_ARB_MXIC_1_R_NONE_15				(0x8F)		/**< Read master number.<br> none				(R15). */

// MXIC2 Write master number (slave)
#define D_FJ_BUS_ARB_MXIC_2_W_SRO					(0x01)		/**< Write master number.<br> SRO				(W01). */
#define D_FJ_BUS_ARB_MXIC_2_W_B2B					(0x02)		/**< Write master number.<br> B2B				(W02). */
#define D_FJ_BUS_ARB_MXIC_2_W_B2R_A					(0x03)		/**< Write master number.<br> B2RA				(W03). */
#define D_FJ_BUS_ARB_MXIC_2_W_B2R_B					(0x04)		/**< Write master number.<br> B2RB				(W04). */
#define D_FJ_BUS_ARB_MXIC_2_W_B2R_C					(0x05)		/**< Write master number.<br> B2RC				(W05). */
#define D_FJ_BUS_ARB_MXIC_2_W_LTMDT					(0x06)		/**< Write master number.<br> LTMDT				(W06). */
#define D_FJ_BUS_ARB_MXIC_2_W_NONE_7				(0x07)		/**< Write master number.<br> none				(W07). */
#define D_FJ_BUS_ARB_MXIC_2_W_NONE_8				(0x08)		/**< Write master number.<br> none				(W08). */
#define D_FJ_BUS_ARB_MXIC_2_W_NONE_9				(0x09)		/**< Write master number.<br> none				(W09). */
#define D_FJ_BUS_ARB_MXIC_2_W_R2Y_A					(0x0A)		/**< Write master number.<br> R2YA				(W10). */
#define D_FJ_BUS_ARB_MXIC_2_W_R2Y_B					(0x0B)		/**< Write master number.<br> R2YB				(W11). */
#define D_FJ_BUS_ARB_MXIC_2_W_R2Y_C					(0x0C)		/**< Write master number.<br> R2YC				(W12). */
#define D_FJ_BUS_ARB_MXIC_2_W_R2Y_D					(0x0D)		/**< Write master number.<br> R2YD				(W13). */
#define D_FJ_BUS_ARB_MXIC_2_W_CNR_A					(0x0E)		/**< Write master number.<br> CNRA				(W14). */
#define D_FJ_BUS_ARB_MXIC_2_W_CNR_B					(0x0F)		/**< Write master number.<br> CNRB				(W15). */

// MXIC2 Read master number (slave)
#define D_FJ_BUS_ARB_MXIC_2_R_SRO					(0x81)		/**< Read master number.<br> SRO				(R01). */
#define D_FJ_BUS_ARB_MXIC_2_R_B2B					(0x82)		/**< Read master number.<br> B2B				(R02). */
#define D_FJ_BUS_ARB_MXIC_2_R_B2R_A					(0x83)		/**< Read master number.<br> B2RA				(R03). */
#define D_FJ_BUS_ARB_MXIC_2_R_NONE_4				(0x84)		/**< Read master number.<br> none				(R04). */
#define D_FJ_BUS_ARB_MXIC_2_R_NONE_5				(0x85)		/**< Read master number.<br> none				(R05). */
#define D_FJ_BUS_ARB_MXIC_2_R_LTMDT					(0x86)		/**< Read master number.<br> LTMDT				(R06). */
#define D_FJ_BUS_ARB_MXIC_2_R_LTM_R					(0x87)		/**< Read master number.<br> LTMR				(R07). */
#define D_FJ_BUS_ARB_MXIC_2_R_LTM_G					(0x88)		/**< Read master number.<br> LTMG				(R08). */
#define D_FJ_BUS_ARB_MXIC_2_R_LTM_B					(0x89)		/**< Read master number.<br> LTMB				(R09). */
#define D_FJ_BUS_ARB_MXIC_2_R_R2Y_A					(0x8A)		/**< Read master number.<br> R2YA				(R10). */
#define D_FJ_BUS_ARB_MXIC_2_R_R2Y_B					(0x8B)		/**< Read master number.<br> R2YB				(R11). */
#define D_FJ_BUS_ARB_MXIC_2_R_R2Y_C					(0x8C)		/**< Read master number.<br> R2YC				(R12). */
#define D_FJ_BUS_ARB_MXIC_2_R_NONE_13				(0x8D)		/**< Read master number.<br> none				(R13). */
#define D_FJ_BUS_ARB_MXIC_2_R_CNR_B					(0x8E)		/**< Read master number.<br> CNRB				(R14). */
#define D_FJ_BUS_ARB_MXIC_2_R_NONE_15				(0x8F)		/**< Read master number.<br> none				(R15). */

// MXIC3 Write master number (slave)
#define D_FJ_BUS_ARB_MXIC_3_W_STAT					(0x01)		/**< Write master number.<br> STAT				(W01). */
#define D_FJ_BUS_ARB_MXIC_3_W_IIP_A					(0x02)		/**< Write master number.<br> IIPA				(W02). */
#define D_FJ_BUS_ARB_MXIC_3_W_IIP_B					(0x03)		/**< Write master number.<br> IIPB				(W03). */
#define D_FJ_BUS_ARB_MXIC_3_W_IIP_C					(0x04)		/**< Write master number.<br> IIPC				(W04). */
#define D_FJ_BUS_ARB_MXIC_3_W_SHDR_A				(0x05)		/**< Write master number.<br> SHDRA				(W05). */
#define D_FJ_BUS_ARB_MXIC_3_W_SHDR_B				(0x06)		/**< Write master number.<br> SHDRB				(W06). */
#define D_FJ_BUS_ARB_MXIC_3_W_SHDR_C				(0x07)		/**< Write master number.<br> SHDRC				(W07). */
#define D_FJ_BUS_ARB_MXIC_3_W_NONE_8				(0x08)		/**< Write master number.<br> none				(W08). */
#define D_FJ_BUS_ARB_MXIC_3_W_ME					(0x09)		/**< Write master number.<br> ME				(W09). */
#define D_FJ_BUS_ARB_MXIC_3_W_IPU_A					(0x0A)		/**< Write master number.<br> IPU_A				(W10). */
#define D_FJ_BUS_ARB_MXIC_3_W_NONE_11				(0x0B)		/**< Write master number.<br> none				(W11). */
#define D_FJ_BUS_ARB_MXIC_3_W_IPU_C					(0x0C)		/**< Write master number.<br> IPU_C				(W12). */
#define D_FJ_BUS_ARB_MXIC_3_W_IPU_D					(0x0D)		/**< Write master number.<br> IPU_D				(W13). */
#define D_FJ_BUS_ARB_MXIC_3_W_IPU_E					(0x0E)		/**< Write master number.<br> IPU_E				(W14). */
#define D_FJ_BUS_ARB_MXIC_3_W_DSP_A					(0x0F)		/**< Write master number.<br> DSP_A				(W15). */
#define D_FJ_BUS_ARB_MXIC_3_W_DSP_B					(0x10)		/**< Write master number.<br> DSP_B				(W16). */
#define D_FJ_BUS_ARB_MXIC_3_W_GPU					(0x11)		/**< Write master number.<br> GPU				(W17). */
#define D_FJ_BUS_ARB_MXIC_3_W_DW					(0x12)		/**< Write master number.<br> DW				(W18). */
#define D_FJ_BUS_ARB_MXIC_3_W_JPEG					(0x13)		/**< Write master number.<br> JPEG				(W19). */
#define D_FJ_BUS_ARB_MXIC_3_W_RAW					(0x14)		/**< Write master number.<br> RAW				(W20). */
#define D_FJ_BUS_ARB_MXIC_3_W_NONE_21				(0x15)		/**< Write master number.<br> none				(W21). */
#define D_FJ_BUS_ARB_MXIC_3_W_GROUP0_M0				(0x30)		/**< Write master number.<br> Group0 Master0	(W48). */
#define D_FJ_BUS_ARB_MXIC_3_W_GROUP0_M1				(0x31)		/**< Write master number.<br> Group0 Master1	(W49). */

// MXIC3 Write master number (group)
#define D_FJ_BUS_ARB_MXIC_3_W_ELA					(0x40)		/**< Write master number.<br> ELA				(W64). */
#define D_FJ_BUS_ARB_MXIC_3_W_XCH					(0x41)		/**< Write master number.<br> XCH				(W65). */
#define D_FJ_BUS_ARB_MXIC_3_W_FPT_0					(0x42)		/**< Write master number.<br> FPT0				(W66). */
#define D_FJ_BUS_ARB_MXIC_3_W_FPT_1					(0x43)		/**< Write master number.<br> FPT1				(W67). */

// MXIC3 Read master number (slave)
#define D_FJ_BUS_ARB_MXIC_3_R_STAT					(0x81)		/**< Read master number.<br> STAT				(R01). */
#define D_FJ_BUS_ARB_MXIC_3_R_IIP_A					(0x82)		/**< Read master number.<br> IIPA				(R02). */
#define D_FJ_BUS_ARB_MXIC_3_R_IIP_B					(0x83)		/**< Read master number.<br> IIPB				(R03). */
#define D_FJ_BUS_ARB_MXIC_3_R_IIP_C					(0x84)		/**< Read master number.<br> IIPC				(R04). */
#define D_FJ_BUS_ARB_MXIC_3_R_SHDR_A				(0x85)		/**< Read master number.<br> SHDRA				(R05). */
#define D_FJ_BUS_ARB_MXIC_3_R_SHDR_B				(0x86)		/**< Read master number.<br> SHDRB				(R06). */
#define D_FJ_BUS_ARB_MXIC_3_R_SHDR_C				(0x87)		/**< Read master number.<br> SHDRC				(R07). */
#define D_FJ_BUS_ARB_MXIC_3_R_SHDR_D				(0x88)		/**< Read master number.<br> SHDRD				(R08). */
#define D_FJ_BUS_ARB_MXIC_3_R_ME					(0x89)		/**< Read master number.<br> ME					(R09). */
#define D_FJ_BUS_ARB_MXIC_3_R_IPU_A					(0x8A)		/**< Read master number.<br> IPU_A				(R10). */
#define D_FJ_BUS_ARB_MXIC_3_R_IPU_B					(0x8B)		/**< Read master number.<br> IPU_B				(R11). */
#define D_FJ_BUS_ARB_MXIC_3_R_IPU_C					(0x8C)		/**< Read master number.<br> IPU_C				(R12). */
#define D_FJ_BUS_ARB_MXIC_3_R_IPU_D					(0x8D)		/**< Read master number.<br> IPU_D				(R13). */
#define D_FJ_BUS_ARB_MXIC_3_R_NONE_14				(0x8E)		/**< Read master number.<br> none				(R14). */
#define D_FJ_BUS_ARB_MXIC_3_R_DSP_A					(0x8F)		/**< Read master number.<br> DSP_A				(R15). */
#define D_FJ_BUS_ARB_MXIC_3_R_DSP_B					(0x90)		/**< Read master number.<br> DSP_B				(R16). */
#define D_FJ_BUS_ARB_MXIC_3_R_GPU					(0x91)		/**< Read master number.<br> GPU				(R17). */
#define D_FJ_BUS_ARB_MXIC_3_R_NONE_18				(0x92)		/**< Read master number.<br> none				(R18). */
#define D_FJ_BUS_ARB_MXIC_3_R_JPEG					(0x93)		/**< Read master number.<br> JPEG				(R19). */
#define D_FJ_BUS_ARB_MXIC_3_R_RAW					(0x94)		/**< Read master number.<br> RAW				(R20). */
#define D_FJ_BUS_ARB_MXIC_3_R_RDMA					(0x95)		/**< Read master number.<br> RDMA				(R21). */
#define D_FJ_BUS_ARB_MXIC_3_R_GROUP0_M0				(0xB0)		/**< Read master number.<br> Group0 Master0		(R48). */
#define D_FJ_BUS_ARB_MXIC_3_R_GROUP0_M1				(0xB1)		/**< Read master number.<br> Group0 Master1		(R49). */

// MXIC3 Read master number (group0)
#define D_FJ_BUS_ARB_MXIC_3_R_ELA					(0xC0)		/**< Write master number.<br> ELA				(R64). */
#define D_FJ_BUS_ARB_MXIC_3_R_XCH					(0xC1)		/**< Write master number.<br> XCH				(R65). */
#define D_FJ_BUS_ARB_MXIC_3_R_FPT_0					(0xC2)		/**< Write master number.<br> FPT0				(R66). */
#define D_FJ_BUS_ARB_MXIC_3_R_FPT_1					(0xC3)		/**< Write master number.<br> FPT1				(R67). */

// MXIC4 Read master number (slave)
#define D_FJ_BUS_ARB_MXIC_4_R_DISP_A				(0x81)		/**< Write master number.<br> DISPA				(R01). */
#define D_FJ_BUS_ARB_MXIC_4_R_DISP_B				(0x82)		/**< Write master number.<br> DISPB				(R02). */

// MXIC5 Write master number (slave)
#define D_FJ_BUS_ARB_MXIC_5_W_EXS_0					(0x01)		/**< Write master number.<br> EXS0				(W01). */
#define D_FJ_BUS_ARB_MXIC_5_W_EXS_1					(0x02)		/**< Write master number.<br> EXS1				(W02). */
#define D_FJ_BUS_ARB_MXIC_5_W_NIC					(0x03)		/**< Write master number.<br> NIC				(W03). */
#define D_FJ_BUS_ARB_MXIC_5_W_BMH_0					(0x04)		/**< Write master number.<br> BMH0				(W04). */
#define D_FJ_BUS_ARB_MXIC_5_W_BMH_1					(0x05)		/**< Write master number.<br> BMH1				(W05). */

// MXIC5 Read master number (slave)
#define D_FJ_BUS_ARB_MXIC_5_R_EXS_0					(0x81)		/**< Write master number.<br> EXS0				(R01). */
#define D_FJ_BUS_ARB_MXIC_5_R_EXS_1					(0x82)		/**< Write master number.<br> EXS1				(R02). */
#define D_FJ_BUS_ARB_MXIC_5_R_NIC					(0x83)		/**< Write master number.<br> NIC				(R03). */
#define D_FJ_BUS_ARB_MXIC_5_R_BMH_0					(0x84)		/**< Write master number.<br> BMH0				(R04). */
#define D_FJ_BUS_ARB_MXIC_5_R_BMH_1					(0x85)		/**< Write master number.<br> BMH1				(R05). */

// MXIC6 Write master number (slave)
#define D_FJ_BUS_ARB_MXIC_6_W_CA7					(0x01)		/**< Write master number.<br> CA7				(W01). */

// MXIC6 Read master number (slave)
#define D_FJ_BUS_ARB_MXIC_6_R_CA7					(0x81)		/**< Write master number.<br> CA7				(R01). */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/**	Slave write or read Arbiter selection. */
typedef enum {
	E_FJ_BUS_ARB_MXIC_UNIT_0 = 0,						/**< MXIC0				*/
	E_FJ_BUS_ARB_MXIC_UNIT_1,							/**< MXIC1				*/
	E_FJ_BUS_ARB_MXIC_UNIT_2,							/**< MXIC2				*/
	E_FJ_BUS_ARB_MXIC_UNIT_3,							/**< MXIC3				*/
	E_FJ_BUS_ARB_MXIC_UNIT_4,							/**< MXIC4				*/
	E_FJ_BUS_ARB_MXIC_UNIT_5,							/**< MXIC5				*/
	E_FJ_BUS_ARB_MXIC_UNIT_6,							/**< MXIC6				*/
	E_FJ_BUS_ARB_MXIC_UNIT_MAX							/**< Enumeration stopper */
} E_FJ_BUS_ARB_MXIC_UNIT;

/**	Slave write arbiter selection. (slave) */
typedef enum {
	E_FJ_BUS_ARB_MXIC_W_ARBITER_W1 = 0,					/**< W1 arbiter */
	E_FJ_BUS_ARB_MXIC_W_ARBITER_W2,						/**< W2 arbiter */
	E_FJ_BUS_ARB_MXIC_W_ARBITER_W3,						/**< W3 arbiter */
	E_FJ_BUS_ARB_MXIC_W_ARBITER_W4,						/**< W4 arbiter */
	E_FJ_BUS_ARB_MXIC_W_ARBITER_MAX						/**< Enumeration stopper */
} E_FJ_BUS_ARB_MXIC_W_ARBITER;

/**	Slave read arbiter selection. (slave) */
typedef enum {
	E_FJ_BUS_ARB_MXIC_R_ARBITER_R1 = 0,					/**< R1 arbiter */
	E_FJ_BUS_ARB_MXIC_R_ARBITER_R2,						/**< R2 arbiter */
	E_FJ_BUS_ARB_MXIC_R_ARBITER_R3,						/**< R3 arbiter */
	E_FJ_BUS_ARB_MXIC_R_ARBITER_R4,						/**< R4 arbiter */
	E_FJ_BUS_ARB_MXIC_R_ARBITER_MAX						/**< Enumeration stopper */
} E_FJ_BUS_ARB_MXIC_R_ARBITER;

/**	Port selection. */
typedef enum {
	E_FJ_BUS_ARB_MXIC_PORT_0 = 0,						/**< Port-0	*/
	E_FJ_BUS_ARB_MXIC_PORT_1,							/**< Port-1	*/
	E_FJ_BUS_ARB_MXIC_PORT_2,							/**< Port-2	*/
	E_FJ_BUS_ARB_MXIC_PORT_3,							/**< Port-3	*/
	E_FJ_BUS_ARB_MXIC_PORT_MAX							/**< Enumeration stopper */
} E_FJ_BUS_ARB_MXIC_PORT;

/**	Port selection.  (group) */
typedef enum {
	E_FJ_BUS_ARB_MXIC_PORT_GR_0 = 0,					/**< Port-0	*/
	E_FJ_BUS_ARB_MXIC_PORT_GR_1,						/**< Port-1	*/
	E_FJ_BUS_ARB_MXIC_PORT_GR_MAX						/**< Enumeration stopper */
} E_FJ_BUS_ARB_MXIC_PORT_GR;


/** SDRAMC channel.
*/
typedef enum {
	E_FJ_SUB_ARB_SDRAMC_CH_0 = 0,					/**< Channel 0.				*/
	E_FJ_SUB_ARB_SDRAMC_CH_1,						/**< Channel 1.				*/
	E_FJ_SUB_ARB_SDRAMC_CH_MAX						/**< Channel MAX.			*/
} E_FJ_SUB_ARB_SDRAMC_CH;

/** Port number.
*/
typedef enum {
	E_FJ_SUB_ARB_SDRAMC_PORT_MX10 = 0,				/**< MXIC1(MX10).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX11,					/**< MXIC1(MX11).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX12,					/**< MXIC1(MX12).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX13,					/**< MXIC1(MX13).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX20,					/**< MXIC2(MX20).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX21,					/**< MXIC2(MX21).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX22,					/**< MXIC2(MX22).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX23,					/**< MXIC2(MX23).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX30,					/**< MXIC3(MX30).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX31,					/**< MXIC3(MX31).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX32,					/**< MXIC3(MX32).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX33,					/**< MXIC3(MX33).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX40,					/**< MXIC4(MX40).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX50,					/**< MXIC5(MX50).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX51,					/**< MXIC5(MX51).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX00,					/**< MXIC0(MX00).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX01,					/**< MXIC0(MX01).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_MX60,					/**< MXIC6(MX60).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_RB0,					/**< H.264(RB0).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_RB1,					/**< H.264(RB1).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_RB2,					/**< H.264(RB2).			*/
	E_FJ_SUB_ARB_SDRAMC_PORT_DS0,					/**< H.265 Common(DS0).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_DS1,					/**< H.265 Common(DS1).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_SDM,					/**< H.265 Common(SDM).		*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	E_FJ_SUB_ARB_SDRAMC_PORT_VE0,					/**< H.265 Encode(VE0).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VE1,					/**< H.265 Encode(VE1).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VE2,					/**< H.265 Encode(VE2).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VD0,					/**< H.265 Decode(VD0).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VD1,					/**< H.265 Decode(VD1).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VD2,					/**< H.265 Decode(VD2).		*/
// ---FJ_SUB_ARB_SDRAMC_PORT_ REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_FJ_SUB_ARB_SDRAMC_PORT_VCOREW,				/**< H.265 Encode(VCOREW).	*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VCORER0,				/**< H.265 Encode(VCORER0).	*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VCORER1,				/**< H.265 Encode(VCORER1).	*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VD0,					/**< H.265 Decode(VD0).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VD1,					/**< H.265 Decode(VD1).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VD2,					/**< H.265 Decode(VD2).		*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VCPU,					/**< H.265 Encode(VCPU).	*/
	E_FJ_SUB_ARB_SDRAMC_PORT_VCOREM,				/**< H.265 Encode(VCOREM).	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	E_FJ_SUB_ARB_SDRAMC_PORT_MAX					/**< Port MAX.				*/
} E_FJ_SUB_ARB_SDRAMC_PORT;

/** enable/disable.
*/
typedef enum {
	E_FJ_SUB_ARB_SDRAMC_EN_DISABLE = 0,				/**< disable.				*/
	E_FJ_SUB_ARB_SDRAMC_EN_ENABLE					/**< enable.				*/
} E_FJ_SUB_ARB_SDRAMC_EN;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/** Master assignment into all the slave slots of W arbiter. */
typedef struct{
	UCHAR		slot0[D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot0 */
	UCHAR		slot1[D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot1 */
	UCHAR		slot2[D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot2 */
	UCHAR		slot3[D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8];	/**< Write master assignment in slot3 */
} T_FJ_BUS_ARB_MXIC_W_ARBITER_ASSIGN;

/** Master assignment into all the slave slots of R arbiter. */
typedef struct{
	UCHAR		slot0[D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot0 */
	UCHAR		slot1[D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot1 */
	UCHAR		slot2[D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot2 */
	UCHAR		slot3[D_FJ_BUS_ARB_MXIC_SLOT_SIZE_8];	/**< Read master assignment in slot3 */
} T_FJ_BUS_ARB_MXIC_R_ARBITER_ASSIGN;

/** Master assignment into all the slave slots of each port of W arbiter. */
typedef struct{
	T_FJ_BUS_ARB_MXIC_W_ARBITER_ASSIGN		port[E_FJ_BUS_ARB_MXIC_PORT_MAX];	/**< Write master assignment in port */
} T_FJ_BUS_ARB_MXIC_W_ARBITER_ASSIGN_PORT;

/** Master assignment into all the slave slots of each port of R arbiter. */
typedef struct{
	T_FJ_BUS_ARB_MXIC_R_ARBITER_ASSIGN		port[E_FJ_BUS_ARB_MXIC_PORT_MAX];	/**< Read master assignment in port */
} T_FJ_BUS_ARB_MXIC_R_ARBITER_ASSIGN_PORT;

/** Master assignment into all the slots of all arbiter. */
typedef struct{
	T_FJ_BUS_ARB_MXIC_W_ARBITER_ASSIGN_PORT	w_arbiter[E_FJ_BUS_ARB_MXIC_W_ARBITER_MAX];	/**< Slave Write master assignment in W arbiter */
	T_FJ_BUS_ARB_MXIC_R_ARBITER_ASSIGN_PORT	r_arbiter[E_FJ_BUS_ARB_MXIC_R_ARBITER_MAX];	/**< Slave Read master assignment in R arbiter */
} T_FJ_BUS_ARB_MXIC_ALL_ARBITER_ASSIGN;

/** Slot priority level */
typedef struct{
	UCHAR			priority_level_type;		/**< TLVL*.bit.LPL*: Priority level type<br>
													<ul><li>@ref D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER
														<li>@ref D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LOWWER
														<li>@ref D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY
														<li>@ref D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LOWWER_ONLY</ul> */
	UCHAR			upper_priority_level;		/**< TLVL*.bit.LPCCL*: Upper priority ratio<br>
													 The setting priority ranges are from 1 to 64("0" value stands for 64) */
	UCHAR			lower_priority_level;		/**< TLVL*.bit.LPCSL*: Lower priority ratio<br>
													 The setting priority ranges are from 1 to 64("0" value stands for 64) */
} T_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LEVEL;

/** Slot priority level for each port */
typedef struct{
	// W1 arbiter
	T_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LEVEL level0[E_FJ_BUS_ARB_MXIC_PORT_MAX];				/**< Setting of level 0 control block for each port */
	T_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LEVEL level1[E_FJ_BUS_ARB_MXIC_PORT_MAX];				/**< Setting of level 1 control block for each port */
	T_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LEVEL level2[E_FJ_BUS_ARB_MXIC_PORT_MAX];				/**< Setting of level 2 control block for each port */
} T_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LEVEL_PORT;

/** Slot priority level in all arbiter */
typedef struct{
	// W arbiter
	T_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LEVEL_PORT w_level[E_FJ_BUS_ARB_MXIC_W_ARBITER_MAX];	/**< Setting of Write arbiter in all arbiter */
	// R arbiter
	T_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_LEVEL_PORT r_level[E_FJ_BUS_ARB_MXIC_R_ARBITER_MAX];	/**< Setting of Read arbiter in all arbiter */
} T_FJ_BUS_ARB_MXIC_ALL_SLOT_PRIORITY_LEVEL;

/** Write master output port */
typedef struct {
	E_FJ_BUS_ARB_MXIC_PORT_GR	ela;				/**< Setting the output port of the JDSELA Master I/F.	*/
	E_FJ_BUS_ARB_MXIC_PORT_GR	xch;				/**< Setting the output port of the JDSXH Master I/F.	*/
	E_FJ_BUS_ARB_MXIC_PORT_GR	fpt_0;				/**< Setting the output port of the FPT Master I/F-0.	*/
	E_FJ_BUS_ARB_MXIC_PORT_GR	fpt_1;				/**< Setting the output port of the FPT Master I/F-1.	*/
} T_FJ_BUS_ARB_MXIC_W_OUTPUT_PORT;

/** Read master output port */
typedef struct {
	E_FJ_BUS_ARB_MXIC_PORT_GR	ela;				/**< Setting the output port of the JDSELA Master I/F.	*/
	E_FJ_BUS_ARB_MXIC_PORT_GR	xch;				/**< Setting the output port of the JDSXH Master I/F.	*/
	E_FJ_BUS_ARB_MXIC_PORT_GR	fpt_0;				/**< Setting the output port of the FPT Master I/F-0.	*/
	E_FJ_BUS_ARB_MXIC_PORT_GR	fpt_1;				/**< Setting the output port of the FPT Master I/F-1.	*/
} T_FJ_BUS_ARB_MXIC_R_OUTPUT_PORT;

/** Output port setting */
typedef struct {
	T_FJ_BUS_ARB_MXIC_W_OUTPUT_PORT	w_out_port;		/**< TPORTMW.bit.TPORTMW_127_64: Write output port setting master channel.	*/
	T_FJ_BUS_ARB_MXIC_R_OUTPUT_PORT	r_out_port;		/**< TPORTMR.bit.TPORTMR_127_64: Read output port setting master channel.	*/
} T_FJ_BUS_ARB_MXIC_OUTPUT_PORT;


/** Commnad arbitration information for rate control.
*/
typedef struct {
	USHORT							throughput[ E_FJ_SUB_ARB_SDRAMC_PORT_MAX ];		/**< Throughput of arbitration permission (average interval) (12bits).	*/
	USHORT							latency[ E_FJ_SUB_ARB_SDRAMC_PORT_MAX ];		/**< Initial latency until arbitration permission (12bits).				*/
	E_FJ_SUB_ARB_SDRAMC_EN			enable[ E_FJ_SUB_ARB_SDRAMC_PORT_MAX ];			/**< Rate control enable.												*/
} T_FJ_SUB_ARB_SDRAMC_CMD_ARB_RATE;

/** Commnad arbitration information for priority control.
*/
typedef struct {
	UCHAR							priority[ E_FJ_SUB_ARB_SDRAMC_PORT_MAX ];		/**< Priority between ports (5bits).
																						 [remarks1]A priority is high so as to be small.
																						 [remarks2]It is forbidden setting the same value between ports.	*/
	E_FJ_SUB_ARB_SDRAMC_EN			enable[ E_FJ_SUB_ARB_SDRAMC_PORT_MAX ];			/**< Priority control enable.											*/
} T_FJ_SUB_ARB_SDRAMC_CMD_ARB_PRI;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special


/* @} */	// fj_busarb_definition group

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/** @weakgroup fj_busarb_api
@{
*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set MXIC output port.<br>
@param[in]		out_port					Setting of output port.<br>
@retval			FJ_ERR_OK					Success.
@retval			FJ_ERR_NG_INPUT_PARAM		Fail - Parameter error.
*/
extern	FJ_ERR_CODE	FJ_BusArb_Set_MxicOutputPortAll( const T_FJ_BUS_ARB_MXIC_OUTPUT_PORT* const out_port );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set the MXIC master to the all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_FJ_BUS_ARB_MXIC_UNIT for a set value.<br>
@param[in]		all_assign		 		Setting of master assign.
@retval			FJ_ERR_OK				Success.
@retval			FJ_ERR_NG_INPUT_PARAM	Fail - Parameter error.
@remarks		The setting changed by this function is reflected by the configration start.
*/
extern	FJ_ERR_CODE	FJ_BusArb_Set_MxicMasterAllArbiter( const E_FJ_BUS_ARB_MXIC_UNIT unit, const T_FJ_BUS_ARB_MXIC_ALL_ARBITER_ASSIGN* const all_assign );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set the MXIC slot priority level control to all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_FJ_BUS_ARB_MXIC_UNIT for a set value.<br>
@param[in]		all_priority			Slot priority level control setting of all arbiter.
@retval			FJ_ERR_OK				Success.
@retval			FJ_ERR_NG_INPUT_PARAM	Fail - Parameter error.
@remarks		The setting  ranges are from 1 to 64.
*/
extern	FJ_ERR_CODE	FJ_BusArb_Set_MxicSlotPriorityAllArbiter( const E_FJ_BUS_ARB_MXIC_UNIT unit, const T_FJ_BUS_ARB_MXIC_ALL_SLOT_PRIORITY_LEVEL* const all_priority );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set MXIC number of transactions for masking the master.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_FJ_BUS_ARB_MXIC_UNIT for a set value.<br>
@param[in]		master					Master number.<br>
										<ul>
											<li>@ref D_FJ_BUS_ARB_MXIC_0_W_SEN_0
											<li>@ref D_FJ_BUS_ARB_MXIC_0_W_SEN_1
											<li>@ref D_FJ_BUS_ARB_MXIC_0_W_SEN_2
											<li>@ref D_FJ_BUS_ARB_MXIC_0_W_SEN_3
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_SRO
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_B2B
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_B2R_A
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_B2R_B
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_B2R_C
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_LTMDT
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_NONE_7
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_NONE_8
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_NONE_9
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_R2Y_A
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_R2Y_B
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_R2Y_C
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_R2Y_D
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_CNR_A
											<li>@ref D_FJ_BUS_ARB_MXIC_1_W_CNR_B
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_SRO
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_B2B
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_B2R_A
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_NONE_4
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_NONE_5
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_LTMDT
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_LTM_R
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_LTM_G
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_LTM_B
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_R2Y_A
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_R2Y_B
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_R2Y_C
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_NONE_13
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_CNR_B
											<li>@ref D_FJ_BUS_ARB_MXIC_1_R_NONE_15
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_SRO
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_B2B
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_B2R_A
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_B2R_B
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_B2R_C
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_LTMDT
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_NONE_7
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_NONE_8
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_NONE_9
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_R2Y_A
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_R2Y_B
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_R2Y_C
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_R2Y_D
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_CNR_A
											<li>@ref D_FJ_BUS_ARB_MXIC_2_W_CNR_B
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_SRO
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_B2B
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_B2R_A
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_NONE_4
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_NONE_5
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_LTMDT
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_LTM_R
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_LTM_G
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_LTM_B
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_R2Y_A
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_R2Y_B
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_R2Y_C
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_NONE_13
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_CNR_B
											<li>@ref D_FJ_BUS_ARB_MXIC_2_R_NONE_15
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_STAT
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_IIP_A
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_IIP_B
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_IIP_C
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_SHDR_A
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_SHDR_B
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_SHDR_C
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_NONE_8
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_ME
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_IPU_A
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_NONE_11
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_IPU_C
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_IPU_D
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_IPU_E
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_DSP_A
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_DSP_B
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_GPU
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_DW
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_JPEG
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_RAW
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_NONE_21
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_GROUP0_M0
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_GROUP0_M1
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_ELA
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_XCH
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_FPT_0
											<li>@ref D_FJ_BUS_ARB_MXIC_3_W_FPT_1
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_STAT
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_IIP_A
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_IIP_B
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_IIP_C
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_SHDR_A
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_SHDR_B
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_SHDR_C
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_SHDR_D
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_ME
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_IPU_A
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_IPU_B
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_IPU_C
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_IPU_D
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_NONE_14
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_DSP_A
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_DSP_B
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_GPU
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_NONE_18
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_JPEG
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_RAW
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_RDMA
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_GROUP0_M0
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_GROUP0_M1
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_ELA
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_XCH
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_FPT_0
											<li>@ref D_FJ_BUS_ARB_MXIC_3_R_FPT_1
											<li>@ref D_FJ_BUS_ARB_MXIC_4_R_DISP_A
											<li>@ref D_FJ_BUS_ARB_MXIC_4_R_DISP_B
											<li>@ref D_FJ_BUS_ARB_MXIC_5_W_EXS_0
											<li>@ref D_FJ_BUS_ARB_MXIC_5_W_EXS_1
											<li>@ref D_FJ_BUS_ARB_MXIC_5_W_NIC
											<li>@ref D_FJ_BUS_ARB_MXIC_5_W_BMH_0
											<li>@ref D_FJ_BUS_ARB_MXIC_5_W_BMH_1
											<li>@ref D_FJ_BUS_ARB_MXIC_5_R_EXS_0
											<li>@ref D_FJ_BUS_ARB_MXIC_5_R_EXS_1
											<li>@ref D_FJ_BUS_ARB_MXIC_5_R_NIC
											<li>@ref D_FJ_BUS_ARB_MXIC_5_R_BMH_0
											<li>@ref D_FJ_BUS_ARB_MXIC_5_R_BMH_1
											<li>@ref D_FJ_BUS_ARB_MXIC_6_W_CA7
											<li>@ref D_FJ_BUS_ARB_MXIC_6_R_CA7
										</ul>
@param[in]		transaction				Master transaction.(0~63)<br>
@retval			FJ_ERR_OK				Success
@retval			FJ_ERR_NG_INPUT_PARAM	Fail - Parameter error
*/
extern	FJ_ERR_CODE	FJ_BusArb_Set_MxicMasterTransaction( const E_FJ_BUS_ARB_MXIC_UNIT unit, const UCHAR master, const UCHAR transaction );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set the following parmeters to boot time default.<br>
- The output port.
- The master to the all arbiter.
- The slot priority level control to all arbiter.

@retval			FJ_ERR_OK				Success
@retval			FJ_ERR_NG_INPUT_PARAM	Fail - Internal error (Driver returned parameter error.)
*/
extern	FJ_ERR_CODE	FJ_BusArb_Set_MxicBootDefault( VOID );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
SDRAMC command arbitration set (rate contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_FJ_SUB_ARB_SDRAMC_CH_0.
														<li>@ref E_FJ_SUB_ARB_SDRAMC_CH_1.</ul>
@param[in]		cmd_arb							Commnad arbitration information for rate control @ref T_FJ_SUB_ARB_SDRAMC_CMD_ARB_RATE.
@retval			D_DDIM_OK						Success.
@retval			FJ_ERR_NG_INPUT_PARAM			Fail - Parameter error.
*/
extern FJ_ERR_CODE FJ_BusArb_Set_Sdramc_Cmd_Arbitration_Rate( const E_FJ_SUB_ARB_SDRAMC_CH ch, const T_FJ_SUB_ARB_SDRAMC_CMD_ARB_RATE* const cmd_arb );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
SDRAMC command arbitration set (priority contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_FJ_SUB_ARB_SDRAMC_CH_0.
														<li>@ref E_FJ_SUB_ARB_SDRAMC_CH_1.</ul>
@param[in]		cmd_arb							Commnad arbitration information for priority control @ref T_FJ_SUB_ARB_SDRAMC_CMD_ARB_PRI.
@retval			D_DDIM_OK						Success.
@retval			FJ_ERR_NG_INPUT_PARAM			Fail - Parameter error.
@remarks		A priority is high so as to be small.
@remarks		It is forbidden setting the same value between ports.
*/
extern FJ_ERR_CODE FJ_BusArb_Set_Sdramc_Cmd_Arbitration_Priority( const E_FJ_SUB_ARB_SDRAMC_CH ch, const T_FJ_SUB_ARB_SDRAMC_CMD_ARB_PRI* const cmd_arb );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set the arbitration parmeters to boot time default.<br>
- Arbitration priority.
- AXI arbitration priority.

@retval		FJ_ERR_OK 					: Set OK
@retval		FJ_ERR_NG_INPUT_PARAM		: Internal error (Driver returned parameter error.)
*/
extern	FJ_ERR_CODE	FJ_BusArb_Set_SdramcBootDefault( VOID );


/* @} */	// fj_busarb_message group

#endif
