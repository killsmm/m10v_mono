/**
 * @file		fj_monitor.h
 * @brief		Monitor api header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_MONITOR_H
#define _FJ_MONITOR_H

#include "ddim_typedef.h"
#include "fj_std.h"

/**
@addtogroup fj_monitor
@{
	- @ref fj_monitor_description
	- @ref fj_monitor_mxic_sample
	- @ref fj_monitor_sdramc_sample
	- @ref fj_monitor_pmu_sample
*/

/** @weakgroup fj_monitor_overview
@{
	@section fj_monitor_description Monitor Description
	- MXIC Monitor<br>
	MXIC(Milbeaut AXI Interconnect) provides that monitoring the transfer size and access count for each master interface. <br>
	<br>
	Monitor Procedure Please See @ref fj_monitor_mxic_sample.<br>
	Specify the type and target To monitor.<br>
	Please call FJ_Monitor_MXIC_Config() function is to set.<br>
	<br>
	Type.<br>
	<table>
		<tr>
		<th>Value</th>
		<th>Content</th>
		</tr>
		<tr>
		<td>
			FJ_MONITOR_MXIC_TYPE_TRANSFER
		</td>
		<td>
			Transfer size
		</td>
		</tr>
		<tr>
		<td>
			FJ_MONITOR_MXIC_TYPE_ACCESS
		</td>
		<td>
			Access count
		</td>
		</tr>
	</table>
	<br>
	Target.<br>
	Master(image macro) or Group master or Port of Arbitration.<br>
	Please See @ref FJ_MONITOR_MXIC_TARGET.<br>
	<br>

	- SDRAMC Monitor<br>
	SDRAMC(SDRAM I/F controller) provides that monitoring the transfer size and how many times the SDRAM access requests <br>
	are received from each interface controller. <br>
	<br>
	Monitor Procedure Please See @ref fj_monitor_sdramc_sample.<br>
	Get monitor data for each arbitration to specify the SDRAMC channel and type.<br>
	Please call FJ_Monitor_SDRAMC_Get_Result() function is to set.<br>
	<br>
	SDRAMC channel.<br>
	<table>
		<tr>
		<th>Value</th>
		<th>Content</th>
		</tr>
		<tr>
		<td>
			FJ_MONITOR_SDRAMC_CH_0
		</td>
		<td>
			SDRAMC Channel 0
		</td>
		</tr>
		<tr>
		<td>
			FJ_MONITOR_SDRAMC_CH_1
		</td>
		<td>
			SDRAMC Channel 1
		</td>
		</tr>
	</table>
	<br>
	Type.<br>
	<table>
		<tr>
		<th>Value</th>
		<th>Content</th>
		</tr>
		<tr>
		<td>
			FJ_MONITOR_SDRAMC_MONITOR_TYPE_TRANSFER
		</td>
		<td>
			Transfer size
		</td>
		</tr>
		<tr>
		<td>
			FJ_MONITOR_SDRAMC_MONITOR_TYPE_ACCESS
		</td>
		<td>
			Access count
		</td>
		</tr>
	</table>
	<br>
	
	- PMU Monitor<br>
	PMU(Performance Monitoring Unit) provides two counters to gather statistics on the operation of the core and memory system. <br>
	Each counter can count any of the events available in the Cortex-A7 MPCore processor. <br>
	It also provides a single 32-bit cycle counter. <br>
	<br>
	Monitor Procedure Please See @ref fj_monitor_pmu_sample.<br>
	Please call FJ_Monitor_PMU_Ctrl_Cycle() function is to set cycle counter enabl.<br>
	Please call FJ_Monitor_PMU_Ctrl_Event() function is to set event.<br>
	<br>
	@par Using the Monitors:
	<br>
	Please change to enable the following compile options in "cpu_rtos/project", and build. <br>
	<table>
		<tr>
		<th>Monitor</th>
		<th>Compile Option</th>
		</tr>
		<tr>
		<td>MXIC Monitor</td>
		<td>CO_MXIC_MONITOR_ON</td>
		</tr>
		<tr>
		<td>SDRAMC Monitor</td>
		<td>CO_SDRAMC_MONITOR_ON</td>
		</tr>
		<tr>
		<td>PMU Monitor</td>
		<td>CO_PMU_MONITOR_ON</td>
		</tr>
	</table>
@}*/	// fj_monitor_overview

/*@}*/

/** @weakgroup fj_monitor_definition
@{*/
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define FJ_MONITOR_MXIC_TARGET_MAX		(4)				/**< Number of monitor target. */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** MXIC Monitor Type */
typedef enum {
	FJ_MONITOR_MXIC_TYPE_TRANSFER = 0x00,				/**< 0x00:	Amount of transfer data */
	FJ_MONITOR_MXIC_TYPE_ACCESS							/**< 0x01:	Access count */
} FJ_MONITOR_MXIC_TYPE;

/** MXIC Monitor Target */
typedef enum {
	FJ_MONITOR_MXIC_TARGET_NOTHING = 0x00,				/**< 0x00:	Monitor target is nothing */
	FJ_MONITOR_MXIC_TARGET_W_IMGBUS_0,					/**< 0x01:	NIC301 ImgBusInterConnect-0(W01) */
	FJ_MONITOR_MXIC_TARGET_W_IMGBUS_1,					/**< 0x02:	NIC301 ImgBusInterConnect-1(W02) */
	FJ_MONITOR_MXIC_TARGET_W_CA5_PORT_1,				/**< 0x03:	NIC301 Cortex-A5Core Master Port1(W03) */
	FJ_MONITOR_MXIC_TARGET_W_STORAGE_M0,				/**< 0x04:	External Storage Block-M0(W04) */
	FJ_MONITOR_MXIC_TARGET_W_STORAGE_M1,				/**< 0x05:	External Storage Block-M1(W05) */
	FJ_MONITOR_MXIC_TARGET_W_PRO_IF_0,					/**< 0x06:	JDSPRO Master I/F-0(W06) */
	FJ_MONITOR_MXIC_TARGET_W_PRO_IF_1,					/**< 0x07:	JDSPRO Master I/F-1(W07) */
	FJ_MONITOR_MXIC_TARGET_W_B2Y_IF_0,					/**< 0x08:	JDSB2Y Master I/F-0(W08) */
	FJ_MONITOR_MXIC_TARGET_W_B2Y_IF_1,					/**< 0x09:	JDSB2Y Master I/F-1(W09) */
	FJ_MONITOR_MXIC_TARGET_W_IIP_IF_0 = 0x0B,			/**< 0x0B:	JDSIIP Master I/F-0(W11) */
	FJ_MONITOR_MXIC_TARGET_W_IIP_IF_1,					/**< 0x0C:	JDSIIP Master I/F-1(W12) */
	FJ_MONITOR_MXIC_TARGET_W_IIP_IF_2,					/**< 0x0D:	JDSIIP Master I/F-2(W13) */
	FJ_MONITOR_MXIC_TARGET_W_SPR_IF_0,					/**< 0x0E:	JDSSPR Master I/F-0(W14) */
	FJ_MONITOR_MXIC_TARGET_W_AHIP_IF_0 = 0x11,			/**< 0x11:	AHIP Master I/F-0(W17) */
	FJ_MONITOR_MXIC_TARGET_W_AHIP_IF_1,					/**< 0x12:	AHIP Master I/F-1(W18) */
	FJ_MONITOR_MXIC_TARGET_W_DSP_IF_0,					/**< 0x13:	DSP Master I/F-0(W19) */
	FJ_MONITOR_MXIC_TARGET_W_DSP_IF_1,					/**< 0x14:	DSP Master I/F-1(W20) */
	FJ_MONITOR_MXIC_TARGET_W_GROUP0_M0 = 0x30,			/**< 0x30:	Group0 Master0(W48) */
	FJ_MONITOR_MXIC_TARGET_W_GROUP0_M1,					/**< 0x31:	Group0 Master1(W49) */
	FJ_MONITOR_MXIC_TARGET_W_GROUP1_M0,					/**< 0x32:	Group1 Master0(W50) */
	FJ_MONITOR_MXIC_TARGET_W_GROUP1_M1,					/**< 0x33:	Group1 Master1(W51) */
	FJ_MONITOR_MXIC_TARGET_W_GROUP2_M0,					/**< 0x34:	Group2 Master0(W52) */
	FJ_MONITOR_MXIC_TARGET_W_GROUP2_M1,					/**< 0x35:	Group2 Master1(W53) */
	FJ_MONITOR_MXIC_TARGET_W_ELA_IF_0 = 0x40,			/**< 0x40:	JDSELA Master I/F-0(W64) */
	FJ_MONITOR_MXIC_TARGET_W_ELA_IF_1,					/**< 0x41:	JDSELA Master I/F-1(W65) */
	FJ_MONITOR_MXIC_TARGET_W_XCH_IF,					/**< 0x42:	JDSXH Master I/F(W66) */
	FJ_MONITOR_MXIC_TARGET_W_JPGENC_IF_0,				/**< 0x43:	JDSJPG Encode Master I/F-0(W67) */
	FJ_MONITOR_MXIC_TARGET_W_JPGDEC_IF_0 = 0x45,		/**< 0x45:	JDSJPG Decode Master I/F-0(W69) */
	FJ_MONITOR_MXIC_TARGET_W_JPGXT_IF = 0x48,			/**< 0x48:	JPGXT Master I/F(W72) */
	FJ_MONITOR_MXIC_TARGET_W_FTP_IF,					/**< 0x49:	FTP Master I/F(W73) */
	FJ_MONITOR_MXIC_TARGET_W_MVR_IF_0,					/**< 0x50:	MVR Master I/F-0(W80) */
	FJ_MONITOR_MXIC_TARGET_W_MVR_IF_1,					/**< 0x51:	MVR Master I/F-1(W81) */
	FJ_MONITOR_MXIC_TARGET_W_MVR_IF_2,					/**< 0x52:	MVR Master I/F-2(W82) */
	FJ_MONITOR_MXIC_TARGET_W_MVR_IF_3,					/**< 0x53:	MVR Master I/F-3(W83) */
	FJ_MONITOR_MXIC_TARGET_R_IMGBUS_0 = 0x81,			/**< 0x81:	NIC301 ImgBusInterConnect-0(R01) */
	FJ_MONITOR_MXIC_TARGET_R_IMGBUS_1,					/**< 0x82:	NIC301 ImgBusInterConnect-1(R02) */
	FJ_MONITOR_MXIC_TARGET_R_CA5_PORT_1,				/**< 0x83:	NIC301 Cortex-A5Core Master Port1(R03) */
	FJ_MONITOR_MXIC_TARGET_R_STORAGE_M0,				/**< 0x84:	External Storage Block-M0(R04) */
	FJ_MONITOR_MXIC_TARGET_R_STORAGE_M1,				/**< 0x85:	External Storage Block-M1(R05) */
	FJ_MONITOR_MXIC_TARGET_R_PRO_IF_0,					/**< 0x86:	JDSPRO Master I/F-0(R06) */
	FJ_MONITOR_MXIC_TARGET_R_PRO_IF_1,					/**< 0x87:	JDSPRO Master I/F-1(R07) */
	FJ_MONITOR_MXIC_TARGET_R_B2Y_IF_0,					/**< 0x88:	JDSB2Y Master I/F-0(R08) */
	FJ_MONITOR_MXIC_TARGET_R_IIP_IF_0 = 0x8B,			/**< 0x8B:	JDSIIP Master I/F-0(R11) */
	FJ_MONITOR_MXIC_TARGET_R_IIP_IF_1,					/**< 0x8C:	JDSIIP Master I/F-1(R12) */
	FJ_MONITOR_MXIC_TARGET_R_IIP_IF_2,					/**< 0x8D:	JDSIIP Master I/F-2(R13) */
	FJ_MONITOR_MXIC_TARGET_R_SPR_IF_0,					/**< 0x8E:	JDSSPR Master I/F-0(R14) */
	FJ_MONITOR_MXIC_TARGET_R_DISP_IF_0,					/**< 0x8F:	DISP Master I/F-0(R15) */
	FJ_MONITOR_MXIC_TARGET_R_DISP_IF_1,					/**< 0x90:	DISP Master I/F-1(R16) */
	FJ_MONITOR_MXIC_TARGET_R_AHIP_IF_0,					/**< 0x91:	AHIP Master I/F-0(R17) */
	FJ_MONITOR_MXIC_TARGET_R_AHIP_IF_1,					/**< 0x92:	AHIP Master I/F-1(R18) */
	FJ_MONITOR_MXIC_TARGET_R_DSP_IF_0,					/**< 0x93:	DSP Master I/F-0(R19) */
	FJ_MONITOR_MXIC_TARGET_R_DSP_IF_1,					/**< 0x94:	DSP Master I/F-1(R20) */
	FJ_MONITOR_MXIC_TARGET_R_GROUP0_M0 = 0xB0,			/**< 0xB0:	Group0 Master0(R48) */
	FJ_MONITOR_MXIC_TARGET_R_GROUP0_M1,					/**< 0xB1:	Group0 Master1(R49) */
	FJ_MONITOR_MXIC_TARGET_R_GROUP1_M0,					/**< 0xB2:	Group1 Master0(R50) */
	FJ_MONITOR_MXIC_TARGET_R_GROUP1_M1,					/**< 0xB3:	Group1 Master1(R51) */
	FJ_MONITOR_MXIC_TARGET_R_GROUP2_M0,					/**< 0xB4:	Group2 Master0(R52) */
	FJ_MONITOR_MXIC_TARGET_R_GROUP2_M1,					/**< 0xB5:	Group2 Master1(R53) */
	FJ_MONITOR_MXIC_TARGET_R_ELA_IF_0 = 0xC0,			/**< 0xC0:	JDSELA Master I/F-0(R64) */
	FJ_MONITOR_MXIC_TARGET_R_ELA_IF_1,					/**< 0xC1:	JDSELA Master I/F-1(R65) */
	FJ_MONITOR_MXIC_TARGET_R_XCH_IF,					/**< 0xC2:	JDSXH Master I/F(R66) */
	FJ_MONITOR_MXIC_TARGET_R_JPGENC_IF_0,				/**< 0xC3:	JDSJPG Encode Master I/F-0(R67) */
	FJ_MONITOR_MXIC_TARGET_R_JPGENC_IF_1,				/**< 0xC4:	JDSJPG Encode Master I/F-1(R68) */
	FJ_MONITOR_MXIC_TARGET_R_JPGDEC_IF_0,				/**< 0xC5:	JDSJPG Decode Master I/F-0(R69) */
	FJ_MONITOR_MXIC_TARGET_R_JPGXT_IF = 0xC8,			/**< 0xC8:	JPGXT Master I/F(R72) */
	FJ_MONITOR_MXIC_TARGET_R_FTP_IF,					/**< 0xC9:	FTP Master I/F(R73) */
	FJ_MONITOR_MXIC_TARGET_R_MVR_IF_0 = 0xD0,			/**< 0xD0:	MVR Master I/F-0(R80) */
	FJ_MONITOR_MXIC_TARGET_R_MVR_IF_1,					/**< 0xD1:	MVR Master I/F-1(R81) */
	FJ_MONITOR_MXIC_TARGET_R_MVR_IF_2,					/**< 0xD2:	MVR Master I/F-2(R82) */
	FJ_MONITOR_MXIC_TARGET_R_MVR_IF_3,					/**< 0xD3:	MVR Master I/F-3(R83) */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W1_PORT_0 = 0x104,		/**< 0x104:	Slave Write Channel-1 Port0 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W1_PORT_1,				/**< 0x105:	Slave Write Channel-1 Port1 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W1_PORT_2,				/**< 0x106:	Slave Write Channel-1 Port2 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W2_PORT_0 = 0x108,		/**< 0x108:	Slave Write Channel-2 Port0 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W2_PORT_1,				/**< 0x109:	Slave Write Channel-2 Port1 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W2_PORT_2,				/**< 0x10A:	Slave Write Channel-2 Port2 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W3_PORT_0 = 0x10C,		/**< 0x10C:	Slave Write Channel-3 Port0 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W3_PORT_1,				/**< 0x10D:	Slave Write Channel-3 Port1 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_W3_PORT_2,				/**< 0x10E:	Slave Write Channel-3 Port2 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R1_PORT_0 = 0x184,		/**< 0x184:	Slave Read Channel-1 Port0 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R1_PORT_1,				/**< 0x185:	Slave Read Channel-1 Port1 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R1_PORT_2,				/**< 0x186:	Slave Read Channel-1 Port2 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R2_PORT_0 = 0x188,		/**< 0x188:	Slave Read Channel-2 Port0 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R2_PORT_1,				/**< 0x189:	Slave Read Channel-2 Port1 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R2_PORT_2,				/**< 0x18A:	Slave Read Channel-2 Port2 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R3_PORT_0 = 0x18C,		/**< 0x18C:	Slave Read Channel-3 Port0 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R3_PORT_1,				/**< 0x18D:	Slave Read Channel-3 Port1 */
	FJ_MONITOR_MXIC_TARGET_SLAVE_R3_PORT_2				/**< 0x18E:	Slave Read Channel-3 Port2 */
} FJ_MONITOR_MXIC_TARGET;

/** SDRAMC Channel Number */
typedef enum {
	FJ_MONITOR_SDRAMC_CH_0 = 0x00,						/**< 0x00:	SDRAMC ch0 */
	FJ_MONITOR_SDRAMC_CH_1,								/**< 0x01:	SDRAMC ch1 */
} FJ_MONITOR_SDRAMC_CH;

/** SDRAMC Access Type */
typedef enum {
	FJ_MONITOR_SDRAMC_ACCESS_TYPE_WRITE_READ = 0x00,	/**< 0x00:	Write & Read */
	FJ_MONITOR_SDRAMC_ACCESS_TYPE_WRITE,				/**< 0x01:	Write */
	FJ_MONITOR_SDRAMC_ACCESS_TYPE_READ					/**< 0x02:	Read */
} FJ_MONITOR_SDRAMC_ACCESS_TYPE;

/** SDRAMC Monitor Type */
typedef enum {
	FJ_MONITOR_SDRAMC_MONITOR_TYPE_TRANSFER = 0x00,		/**< 0x00:	Amount of transfer data */
	FJ_MONITOR_SDRAMC_MONITOR_TYPE_ACCESS				/**< 0x01:	Access count */
} FJ_MONITOR_SDRAMC_MONITOR_TYPE;

/** SDRAMC Monitor Update Mode */
typedef enum {
	FJ_MONITOR_SDRAMC_MODE_NOT_CLEAR = 0x00,			/**< 0x00:	Not Clear Mode (monitor will not be cleared after being updated.) */
	FJ_MONITOR_SDRAMC_MODE_UPDATE						/**< 0x01:	Update Mode (monitor will be cleared after being updated.) */
} FJ_MONITOR_SDRAMC_MODE;

/** PMU Counter Number */
typedef enum {
	FJ_MONITOR_PMU_COUNTER_NUM_0 = 0x00,				/**< 0x00:	Counter #0 */
	FJ_MONITOR_PMU_COUNTER_NUM_1,						/**< 0x01:	Counter #1 */
	FJ_MONITOR_PMU_COUNTER_NUM_2,						/**< 0x02:	Counter #2 */
	FJ_MONITOR_PMU_COUNTER_NUM_3,						/**< 0x03:	Counter #3 */
	FJ_MONITOR_PMU_COUNTER_NUM_31 = 0x1F				/**< 0x1F:	Counter #31 */
} FJ_MONITOR_PMU_COUNTER;

/** PMU Event Number */
typedef enum {
	FJ_MONITOR_PMU_EVENT_DISABLE = -1,					/**< -1:	Event Counter is disabled. */
	FJ_MONITOR_PMU_EVENT_SW_INCR = 0x00,				/**< 0x00:	Software increment. The register is incremented only on writes to the Software Increment Register. */
	FJ_MONITOR_PMU_EVENT_L1I_CACHE_REFILL,				/**< 0x01:	Instruction fetch that causes a refill at (at least) the lowest level of instruction or unified cache. <br>
																	Includes the speculative linefills in the count. */
	FJ_MONITOR_PMU_EVENT_L1I_TLB_REFILL,				/**< 0x02:	Instruction fetch that causes a TLB refill at (at least) the lowest level of TLB. <br>
																	Includes the speculative requests in the count. */
	FJ_MONITOR_PMU_EVENT_L1D_CACHE_REFILL,				/**< 0x03:	Data read or write operation that causes a refill at (at least) the lowest level of data or unified cache. <br>
																	Counts the number of allocations performed in the Data Cache because of a read or a write. */
	FJ_MONITOR_PMU_EVENT_L1D_CACHE,						/**< 0x04:	Data read or write operation that causes a cache access at (at least) the lowest level of data or unified cache. <br>
																	This includes speculative reads. */
	FJ_MONITOR_PMU_EVENT_L1D_TLB_REFILL,				/**< 0x05:	Data read or write operation that causes a TLB refill at (at least) the lowest level of TLB. <br>
																	This does not include micro TLB misses because of PLD, PLI, CP15 Cache operation by MVA and CP15 VA to PA operations. */
	FJ_MONITOR_PMU_EVENT_LD_RETIRED,					/**< 0x06:	Data read architecturally executed. Counts the number of data read instructions accepted by the Load Store Unit. <br>
																	This includes counting the speculative and aborted LDR/LDM, and the reads because of the SWP instructions. */
	FJ_MONITOR_PMU_EVENT_ST_RETIRED,					/**< 0x07:	Data write architecturally executed. Counts the number of data write instructions accepted by the Load Store Unit. <br>
																	This includes counting the speculative and aborted STR/STM, and the writes because of the SWP instructions. */
	FJ_MONITOR_PMU_EVENT_INST_RETIRED,					/**< 0x08:	Instruction architecturally executed. */
	FJ_MONITOR_PMU_EVENT_EXC_TAKEN,						/**< 0x09:	Exception taken. Counts the number of exceptions architecturally taken. */
	FJ_MONITOR_PMU_EVENT_EXC_RETURN,					/**< 0x0A:	Exception return architecturally executed. */
	FJ_MONITOR_PMU_EVENT_CID_WRITE_RETIRED,				/**< 0x0B:	Change to ContextID retired. Counts the number of instructions architecturally executed writing into the ContextID Register. */
	FJ_MONITOR_PMU_EVENT_PC_WRITE_RETIRED,				/**< 0x0C:	Software change of PC. */
	FJ_MONITOR_PMU_EVENT_BR_IMMED_RETIRED,				/**< 0x0D:	Immediate branch architecturally executed (taken or not taken). <br>
																	This includes the branches which are flushed due to a previous load/store which aborts late. */
	FJ_MONITOR_PMU_EVENT_BR_RETURN_RETIRED,				/**< 0x0E:	Procedure return (other than exception returns) architecturally executed. */
	FJ_MONITOR_PMU_EVENT_UNALIGNED_LDST_RETIRED,		/**< 0x0F:	Unaligned load-store. */
	FJ_MONITOR_PMU_EVENT_BR_MIS_PRED,					/**< 0x10:	Branch mispredicted/not predicted. Counts the number of mispredicted or not-predicted branches executed. <br>
																	This includes the branches which are flushed because of a previous load/store which aborts late. */
	FJ_MONITOR_PMU_EVENT_CPU_CYCLES,					/**< 0x11:	Cycle counter. */
	FJ_MONITOR_PMU_EVENT_BR_PRED,						/**< 0x12:	Branches or other change in program flow that could have been predicted by the branch prediction resources of the processor. <br>
																	This includes the branches which are flushed because of a previous load/store which aborts late. */
	FJ_MONITOR_PMU_EVENT_MEM_ACCESS,					/**< 0x13:	Data memory access. */
	FJ_MONITOR_PMU_EVENT_L1I_CACHE,						/**< 0x14:	Instruction Cache access. */
	FJ_MONITOR_PMU_EVENT_L1D_CACHE_WB,					/**< 0x15:	Data cache eviction. */
	FJ_MONITOR_PMU_EVENT_L2D_CACHE_ACCESS,				/**< 0x16:	Level 2 data cache access. */
	FJ_MONITOR_PMU_EVENT_L2D_CACHE_REFILL,				/**< 0x17:	Level 2 data cache refill. */
	FJ_MONITOR_PMU_EVENT_L2D_CACHE_WB,					/**< 0x18:	Level 2 data cache write-back. */
	FJ_MONITOR_PMU_EVENT_BUS_ACCESS,					/**< 0x19:	Bus accesses. */
	FJ_MONITOR_PMU_EVENT_BUS_CYCLE = 0x1D,				/**< 0x1D:	Bus cycle. */
	FJ_MONITOR_PMU_EVENT_BUS_ACCESS_READ = 0x60,		/**< 0x60:	Bus accesses read. */
	FJ_MONITOR_PMU_EVENT_BUS_ACCESS_WRITE,				/**< 0x61:	Bus accesses write. */
	FJ_MONITOR_PMU_EVENT_EXC_IRQ = 0x86,				/**< 0x86:	IRQ exception taken. */
	FJ_MONITOR_PMU_EVENT_EXC_FIQ,						/**< 0x87:	FIQ exception taken. */
	FJ_MONITOR_PMU_EVENT_EXT_MEMORY_REQUEST = 0xC0,		/**< 0xC0:	External memory request. */
	FJ_MONITOR_PMU_EVENT_NC_EXT_MEMORY_REQUEST,			/**< 0xC1:	Non-cacheable external memory request. */
	FJ_MONITOR_PMU_EVENT_PREFETCH_LINEFILL,				/**< 0xC2:	Linefill because of prefetch. */
	FJ_MONITOR_PMU_EVENT_PREFETCH_LINEFILL_DROPPED,		/**< 0xC3:	Prefetch linefill dropped. */
	FJ_MONITOR_PMU_EVENT_ENTERING_READ_ALLOCATE_MODE,	/**< 0xC4:	Entering read allocate mode. */
	FJ_MONITOR_PMU_EVENT_READ_ALLOCATE_MODE,			/**< 0xC5:	Read allocate mode. */
	FJ_MONITOR_PMU_EVENT_ETM_OUT0 = 0xC7,				/**< 0xC7:	ETM Ext Out[0]. */
	FJ_MONITOR_PMU_EVENT_ETM_OUT1,						/**< 0xC8:	ETM Ext Out[1]. */
	FJ_MONITOR_PMU_EVENT_DATA_WRITE_OPERATION,			/**< 0xC9:	Data Write operation that stalls the pipeline because the store buffer is full. */
	FJ_MONITOR_PMU_EVENT_DATA_SNOOP_PROCESS				/**< 0xCA:	Data snooped from other processor. */
} FJ_MONITOR_PMU_EVENT;

/** PMU Reset Counter */
typedef enum {
	FJ_MONITOR_PMU_RESET_NO_ACTION = 0x00,				/**< 0x00:	No Reset Action */
	FJ_MONITOR_PMU_RESET_EVENT_COUNTERS,				/**< 0x01:	Reset Evnet Counter #0 and #1 to Zero */
	FJ_MONITOR_PMU_RESET_CYCLE_COUNTER,					/**< 0x02:	Reset Cycle Counter to Zero */
	FJ_MONITOR_PMU_RESET_ALL_COUNTERS					/**< 0x03:	Reset All Counters to Zero */
} FJ_MONITOR_PMU_RESET;

/** PMU Cycle Divider */
typedef enum {
	FJ_MONITOR_PMU_DIVIDER_NO_DIVIDE = 0x00,			/**< 0x00:	Count Every Clock Cycle */
	FJ_MONITOR_PMU_DIVIDER_DIVIDE_64					/**< 0x01:	Count Every 64th Clock Cycle */
} FJ_MONITOR_PMU_DIVIDER;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** MXIC Monitor Config */
typedef struct {
	FJ_MONITOR_MXIC_TYPE		monitor_type[FJ_MONITOR_MXIC_TARGET_MAX];		/**< Selection monitor type. */
	FJ_MONITOR_MXIC_TARGET		monitor_target[FJ_MONITOR_MXIC_TARGET_MAX];		/**< Selection monitor target. */
} FJ_MONITOR_MXIC_CFG;

/** MXIC Monitor Result */
typedef struct {
	UINT32						count[FJ_MONITOR_MXIC_TARGET_MAX];				/**< Monitor result of access count or transfer amount */
} FJ_MONITOR_MXIC_RESULT;

/** SDRAMC Monitor Result */
typedef struct {
	ULONG	mxic00;										/**< MXIC0(MX00). */
	ULONG	mxic01;										/**< MXIC0(MX01). */
	ULONG	mxic10;										/**< MXIC1(MX10). */
	ULONG	mxic11;										/**< MXIC1(MX11). */
	ULONG	mxic12;										/**< MXIC1(MX12). */
	ULONG	mxic13;										/**< MXIC1(MX13). */
	ULONG	mxic20;										/**< MXIC2(MX20). */
	ULONG	mxic21;										/**< MXIC2(MX21). */
	ULONG	mxic22;										/**< MXIC2(MX22). */
	ULONG	mxic23;										/**< MXIC2(MX23). */
	ULONG	mxic30;										/**< MXIC3(MX30). */
	ULONG	mxic31;										/**< MXIC3(MX31). */
	ULONG	mxic32;										/**< MXIC3(MX32). */
	ULONG	mxic33;										/**< MXIC3(MX33). */
	ULONG	mxic40;										/**< MXIC4(MX40). */
	ULONG	mxic50;										/**< MXIC5(MX50). */
	ULONG	mxic51;										/**< MXIC5(MX51). */
	ULONG	mxic60;										/**< MXIC6(MX60). */
	ULONG	ribery0;									/**< H.264(RB0). */
	ULONG	ribery1;									/**< H.264(RB1). */
	ULONG	ribery2;									/**< H.264(RB2). */
	ULONG	h265_com0;									/**< H.265 Common(DS0). */
	ULONG	h265_com1;									/**< H.265 Common(DS1). */
	ULONG	h265_com2;									/**< H.265 Common(SDM). */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	ULONG	h265_enc0;									/**< H.265 Encode(VE0). */
	ULONG	h265_enc1;									/**< H.265 Encode(VE1). */
	ULONG	h265_enc2;									/**< H.265 Encode(VE2). */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	ULONG	h265_enc0;									/**< H.265 Encode(VCOREW).  */
	ULONG	h265_enc1;									/**< H.265 Encode(VCORER0). */
	ULONG	h265_enc2;									/**< H.265 Encode(VCORER1). */
	ULONG	h265_enc3;									/**< H.265 Encode(VCPU).    */
	ULONG	h265_enc4;									/**< H.265 Encode(VCOREM).  */
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	ULONG	h265_dec0;									/**< H.265 Decode(VD0). */
	ULONG	h265_dec1;									/**< H.265 Decode(VD1). */
	ULONG	h265_dec2;									/**< H.265 Decode(VD2). */
} FJ_MONITOR_SDRAMC_RESULT;
/*@}*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/** @weakgroup fj_monitor_api
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
MXIC Monitor Control On
<b><br>
This function includes clock contorl. <br>
Always performs clock control when transfer request of master does not occur.
</b>
*/
VOID FJ_Monitor_MXIC_Ctrl_On(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
MXIC Monitor Control Off
*/
VOID FJ_Monitor_MXIC_Ctrl_Off(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
MXIC Monitor Config
@param[in] cfg				: MXIC Monitor Type and Target
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@retval FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
FJ_ERR_CODE FJ_Monitor_MXIC_Config(FJ_MONITOR_MXIC_CFG* cfg);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
MXIC Monitor Start
<br><br>
*/
VOID FJ_Monitor_MXIC_Start(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
MXIC Monitor Stop and Get Result
@param[out] mxic_result		: Transfer Size or Access Count
@param[out] measure_time	: Measured Time[us]
*/
VOID FJ_Monitor_MXIC_Stop(FJ_MONITOR_MXIC_RESULT* mxic_result, UINT64* const measure_time);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
SDRAMC Monitor Start
@param[in] sdramc_ch			: SDRAMC Channel (0 to 1)
@param[in] monitor_type			: Monitor Type
@param[in] access_type			: Access Type
*/
VOID FJ_Monitor_SDRAMC_Start(FJ_MONITOR_SDRAMC_CH sdramc_ch, FJ_MONITOR_SDRAMC_MONITOR_TYPE monitor_type, FJ_MONITOR_SDRAMC_ACCESS_TYPE access_type);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
SDRAMC Monitor Stop
@param[in] sdramc_ch			: SDRAMC Channel (0 to 1)
*/
VOID FJ_Monitor_SDRAMC_Stop(FJ_MONITOR_SDRAMC_CH sdramc_ch);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
SDRAMC Monitor Get Result
@param[in] sdramc_ch			: SDRAMC Channel (0 to 1)
@param[out] sdramc_result		: Transfer Size or Access Count
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@retval FJ_ERR_NG_INPUT_PARAM : Input Parameter error
*/
FJ_ERR_CODE FJ_Monitor_SDRAMC_Get_Result(FJ_MONITOR_SDRAMC_CH sdramc_ch, FJ_MONITOR_SDRAMC_RESULT* sdramc_result);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
PMU Configure Cycle Counter
@param[in] flag			: Cycle Counter (0:disable, 1:enable)
*/
VOID FJ_Monitor_PMU_Ctrl_Cycle(BOOL flag);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
PMU Configure Evnet Counter
@param[in] counter_num		: Counter Number (0 to 1)
@param[in] event_num		: Event Number
*/
VOID FJ_Monitor_PMU_Ctrl_Event(FJ_MONITOR_PMU_COUNTER counter_num, FJ_MONITOR_PMU_EVENT event_num);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
PMU Start to Count
@param[in] reset_counter	: Reset Counter (0 to 4)
@param[in] cycle_divider	: Cycle Divider (0 to 1)
*/
VOID FJ_Monitor_PMU_Start(FJ_MONITOR_PMU_RESET reset_counter, FJ_MONITOR_PMU_DIVIDER cycle_divider);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
PMU Stop to Count
*/
VOID FJ_Monitor_PMU_Stop(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
PMU Get Cycle Counter Value
@retval Cycle Counter Value
*/
UINT32 FJ_Monitor_PMU_Get_Cycle_Count(VOID);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
PMU Get Event Counter Value
@param[in] counter_num		: Counter Number (0 to 1)
@retval Event Counter Value
*/
UINT32 FJ_Monitor_PMU_Get_Event_Count(FJ_MONITOR_PMU_COUNTER counter_num);

/*@}*/	// fj_monitor_api

#ifdef __cplusplus
}
#endif

/*@}*/
/** @weakgroup fj_monitor_api
@{
	@section fj_monitor_mxic_sample MXIC Monitor Sample
	The example of executing the "MXIC Monitor" start to end is as follows.
	@code
	{
		FJ_MONITOR_MXIC_CFG cfg;
		FJ_MONITOR_MXIC_RESULT result;
		UINT64 time;
		FJ_ERR_CODE fj_ret;
		
		// MXIC Monitor is ON.
		FJ_Monitor_MXIC_Ctrl_On();
		
		// Select monitor type.
		cfg.monitor_type[0] = FJ_MONITOR_MXIC_TYPE_TRANSFER;
		cfg.monitor_type[1] = FJ_MONITOR_MXIC_TYPE_ACCESS;
		cfg.monitor_type[2] = FJ_MONITOR_MXIC_TYPE_TRANSFER;
		cfg.monitor_type[3] = FJ_MONITOR_MXIC_TYPE_ACCESS;
		//
		// Select monitor target.
		cfg.monitor_target[0] = FJ_MONITOR_MXIC_TARGET_SLAVE_W2_PORT_0;
		cfg.monitor_target[1] = FJ_MONITOR_MXIC_TARGET_SLAVE_W2_PORT_1;
		cfg.monitor_target[2] = FJ_MONITOR_MXIC_TARGET_SLAVE_R2_PORT_0;
		cfg.monitor_target[3] = FJ_MONITOR_MXIC_TARGET_SLAVE_R2_PORT_1;
		
		// Set Monitor configure.
		fj_ret = FJ_Monitor_MXIC_Config(&cfg);
		if(fj_ret != FJ_ERR_OK){
			return -1;	// Error
		}
		
		// Start to monitor.
		FJ_Monitor_MXIC_Start();
		
		// Stop to monitor and Get the selected values of transfer size or access count, and measured time.
		FJ_Monitor_MXIC_Stop(&result, &time);
		
		// MXIC Monitor is OFF.
		FJ_Monitor_MXIC_Ctrl_Off();
	}
	@endcode
	
	@section fj_monitor_sdramc_sample SDRAMC Monitor Sample
	The example of executing the "SDRAMC Monitor" start to end is as follows.
	@code
	{
		FJ_MONITOR_SDRAMC_RESULT size;
		FJ_ERR_CODE fj_ret;
		
		// Start to monitor.
		FJ_Monitor_SDRAMC_Start(FJ_MONITOR_SDRAMC_CH_1, FJ_MONITOR_SDRAMC_MONITOR_TYPE_TRANSFER, FJ_MONITOR_SDRAMC_ACCESS_TYPE_WRITE_READ);
		
		// Stop to monitor.
		FJ_Monitor_SDRAMC_Stop(FJ_MONITOR_SDRAMC_CH_1);
		
		// Get the values of transfer size each interface.
		fj_ret = FJ_Monitor_SDRAMC_Get_Result(FJ_MONITOR_SDRAMC_CH_1, &size);
		if(fj_ret != FJ_ERR_OK){
			return -1;	// Error
		}
	}
	@endcode
	
	@section fj_monitor_pmu_sample PMU Monitor Sample
	The example of executing the "PMU Monitor" start to end is as follows.
	@code
	{
		UINT32 cycle;
		UINT32 event0;
		UINT32 event1;
		
		// Cycle counter is enable.
		FJ_Monitor_PMU_Ctrl_Cycle(TRUE);
		
		// Event counter #0 will count a L1 cache instruction fetch that causes a refill.
		FJ_Monitor_PMU_Ctrl_Event(FJ_MONITOR_PMU_COUNTER_NUM_0, FJ_MONITOR_PMU_EVENT_L1I_CACHE_REFILL);
		
		// Event counter #1 will count a L1 cache data read or write operation that causes a refill.
		FJ_Monitor_PMU_Ctrl_Event(FJ_MONITOR_PMU_COUNTER_NUM_1, FJ_MONITOR_PMU_EVENT_L1D_CACHE_REFILL);
		
		// Start to monitor.
		FJ_Monitor_PMU_Start(FJ_MONITOR_PMU_RESET_ALL_COUNTERS, FJ_MONITOR_PMU_DIVIDER_NO_DIVIDE);
		
		// Stop to monitor.
		FJ_Monitor_PMU_Stop();
		
		// Get a value of Cycle counter.
		cycle = FJ_Monitor_PMU_Get_Cycle_Count();
		
		// Get a value of Event counter #0.
		event0 = FJ_Monitor_PMU_Get_Event_Count(FJ_MONITOR_PMU_COUNTER_NUM_0);
		
		// Get a value of Event counter #1.
		event1 = FJ_Monitor_PMU_Get_Event_Count(FJ_MONITOR_PMU_COUNTER_NUM_1);
	}
	@endcode
@}*/	// fj_monitor_sample

#endif	// _FJ_MONITOR_H
