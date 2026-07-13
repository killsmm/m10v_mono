/**
 * @file		fj_pcie.h
 * @brief		PCIe Controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_PCIE_H_
#define _FJ_PCIE_H_

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
//  overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_overview
@{
<span style="font-weight: bold;">Introduction</span><br>
&nbsp; <br>
The PCIe controls can be achieved by using this API.<br>
<br>
<span style="font-weight: bold;">Supported function</span><br>
<br>
BaseFW supports following functions.<br>
<ul>
 <li>Root Port of PCI Express Root Complex. See @ref fj_pcie_rc</li>
 <li>PCI Express Endpoint. See @ref fj_pcie_ep</li>
</ul><br>
@}*//* --- end of fj_pcie_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_pcie_sequence
@{
@}*//* --- end of fj_pcie_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_definition
@{*/

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/** PCIe channel */
typedef enum {
	E_FJ_PCIE_CH0 = 0,						/**< PCIe channel 0	*/
	E_FJ_PCIE_CH1							/**< PCIe channel 1	*/
} E_FJ_PCIE_CH;

/** Bifurcation mode */
typedef enum {
	E_FJ_PCIE_BIFURCATION_DISABLE = 0,		/**< Bifurcation disable	*/
	E_FJ_PCIE_BIFURCATION_ENABLE			/**< Bifurcation enable		*/
} E_FJ_PCIE_BIFURCATION;

/** DMA transfer direction */
typedef enum {
	E_FJ_PCIE_DMA_DIR_DTOS = 0,				/**< Transfer source from the transfer destination.	*/
	E_FJ_PCIE_DMA_DIR_STOD					/**< Destination from the transfer source.			*/
} E_FJ_PCIE_DMA_DIR;

/** Link speed */
typedef enum {
	E_FJ_PCIE_LINK_SPEED_GEN1 = 1,			/**< 2.5GT/s	*/
	E_FJ_PCIE_LINK_SPEED_GEN2				/**< 5.0GT/s	*/
} E_FJ_PCIE_LINK_SPEED;

/** State of data communication link up  */
typedef enum {
	E_FJ_PCIE_ST_DL_UP_NG = 0,			/**< Not the data link up, TLP communication impossible.	*/
	E_FJ_PCIE_ST_DL_UP_OK				/**< Data link-up and TLP possible communication.			*/
} E_FJ_PCIE_ST_DL_UP;

/** STate of Link Training and Status State Machine  */
typedef enum {
	E_FJ_PCIE_S_DETECT_QUIET = 0,	/**< DETECT_QUIET		*/
	E_FJ_PCIE_S_DETECT_ACT,			/**< DETECT_ACT			*/
	E_FJ_PCIE_S_POLL_ACTIVE,		/**< POLL_ACTIVE		*/
	E_FJ_PCIE_S_POLL_COMPLIANCE,	/**< POLL_COMPLIANCE	*/
	E_FJ_PCIE_S_POLL_CONFIG,		/**< POLL_CONFIG		*/
	E_FJ_PCIE_S_PRE_DETECT_QUIET,	/**< PRE_DETECT_QUIET	*/
	E_FJ_PCIE_S_DETECT_WAIT,		/**< DETECT_WAIT		*/
	E_FJ_PCIE_S_CFG_LINKWD_START,	/**< CFG_LINKWD_START	*/
	E_FJ_PCIE_S_CFG_LINKWD_ACEPT,	/**< CFG_LINKWD_ACEPT	*/
	E_FJ_PCIE_S_CFG_LANENUM_WAI,	/**< CFG_LANENUM_WAI	*/
	E_FJ_PCIE_S_CFG_LANENUM_ACEPT,	/**< CFG_LANENUM_ACEPT	*/
	E_FJ_PCIE_S_CFG_COMPLETE,		/**< CFG_COMPLETE		*/
	E_FJ_PCIE_S_CFG_IDLE,			/**< CFG_IDLE			*/
	E_FJ_PCIE_S_RCVRY_LOCK,			/**< RCVRY_LOCK			*/
	E_FJ_PCIE_S_RCVRY_SPEED,		/**< RCVRY_SPEED		*/
	E_FJ_PCIE_S_RCVRY_RCVRCFG,		/**< RCVRY_RCVRCFG		*/
	E_FJ_PCIE_S_RCVRY_IDLE,			/**< RCVRY_IDLE			*/
	E_FJ_PCIE_S_L0,					/**< L0					*/
	E_FJ_PCIE_S_L0S,				/**< L0S				*/
	E_FJ_PCIE_S_L123_SEND_EIDLE,	/**< L123_SEND_EIDLE	*/
	E_FJ_PCIE_S_L1_IDLE,			/**< L1_IDLE			*/
	E_FJ_PCIE_S_L2_IDLE,			/**< L2_IDLE			*/
	E_FJ_PCIE_S_L2_WAKE,			/**< L2_WAKE			*/
	E_FJ_PCIE_S_DISABLED_ENTRY,		/**< DISABLED_ENTRY		*/
	E_FJ_PCIE_S_DISABLED_IDLE,		/**< DISABLED_IDLE		*/
	E_FJ_PCIE_S_DISABLED,			/**< DISABLED			*/
	E_FJ_PCIE_S_LPBK_ENTRY,			/**< LPBK_ENTRY			*/
	E_FJ_PCIE_S_LPBK_ACTIVE,		/**< LPBK_ACTIVE		*/
	E_FJ_PCIE_S_LPBK_EXIT,			/**< LPBK_EXIT			*/
	E_FJ_PCIE_S_LPBK_EXIT_TIMEOUT,	/**< LPBK_EXIT_TIMEOUT	*/
	E_FJ_PCIE_S_HOT_RESET_ENTRY,	/**< HOT_RESET_ENTRY	*/
	E_FJ_PCIE_S_HOT_RESET,			/**< HOT_RESET			*/
	E_FJ_PCIE_S_RCVRY_EQ0,			/**< RCVRY_EQ0			*/
	E_FJ_PCIE_S_RCVRY_EQ1,			/**< RCVRY_EQ1			*/
	E_FJ_PCIE_S_RCVRY_EQ2,			/**< RCVRY_EQ2			*/
	E_FJ_PCIE_S_RCVRY_EQ3			/**< RCVRY_EQ3			*/
} E_FJ_PCIE_ST_LTSSM;

/** Max payload size */
typedef enum {
	E_FJ_PCIE_DMA_MPS_128 = 0,		/**< 128 bytes	*/
	E_FJ_PCIE_DMA_MPS_256,			/**< 256 bytes	*/
	E_FJ_PCIE_DMA_MPS_512			/**< 512 bytes	*/
} E_FJ_PCIE_MPS;

/** Power state */
typedef enum {
	E_FJ_PCIE_PMST_D0 = 0,			/**< D0 State		*/
	E_FJ_PCIE_PMST_D1,				/**< D1 State		*/
	E_FJ_PCIE_PMST_D2,				/**< D2 State		*/
	E_FJ_PCIE_PMST_D3,				/**< D3 State		*/
	E_FJ_PCIE_PMST_UNINITIALIZED	/**< Uninitialized	*/
} E_FJ_PCIE_PMST;

/** MSI Number */
typedef enum {
	E_FJ_PCIE_MSI_NUM_00 = 0,	/**< msi number 0	*/
	E_FJ_PCIE_MSI_NUM_01,		/**< msi number 1	*/
	E_FJ_PCIE_MSI_NUM_02,		/**< msi number 2	*/
	E_FJ_PCIE_MSI_NUM_03,		/**< msi number 3	*/
	E_FJ_PCIE_MSI_NUM_04,		/**< msi number 4	*/
	E_FJ_PCIE_MSI_NUM_05,		/**< msi number 5	*/
	E_FJ_PCIE_MSI_NUM_06,		/**< msi number 6	*/
	E_FJ_PCIE_MSI_NUM_07,		/**< msi number 7	*/
	E_FJ_PCIE_MSI_NUM_08,		/**< msi number 8	*/
	E_FJ_PCIE_MSI_NUM_09,		/**< msi number 9	*/
	E_FJ_PCIE_MSI_NUM_10,		/**< msi number 10	*/
	E_FJ_PCIE_MSI_NUM_11,		/**< msi number 11	*/
	E_FJ_PCIE_MSI_NUM_12,		/**< msi number 12	*/
	E_FJ_PCIE_MSI_NUM_13,		/**< msi number 13	*/
	E_FJ_PCIE_MSI_NUM_14,		/**< msi number 14	*/
	E_FJ_PCIE_MSI_NUM_15,		/**< msi number 15	*/
	E_FJ_PCIE_MSI_NUM_16,		/**< msi number 16	*/
	E_FJ_PCIE_MSI_NUM_17,		/**< msi number 17	*/
	E_FJ_PCIE_MSI_NUM_18,		/**< msi number 18	*/
	E_FJ_PCIE_MSI_NUM_19,		/**< msi number 19	*/
	E_FJ_PCIE_MSI_NUM_20,		/**< msi number 20	*/
	E_FJ_PCIE_MSI_NUM_21,		/**< msi number 21	*/
	E_FJ_PCIE_MSI_NUM_22,		/**< msi number 22	*/
	E_FJ_PCIE_MSI_NUM_23,		/**< msi number 23	*/
	E_FJ_PCIE_MSI_NUM_24,		/**< msi number 24	*/
	E_FJ_PCIE_MSI_NUM_25,		/**< msi number 25	*/
	E_FJ_PCIE_MSI_NUM_26,		/**< msi number 26	*/
	E_FJ_PCIE_MSI_NUM_27,		/**< msi number 27	*/
	E_FJ_PCIE_MSI_NUM_28,		/**< msi number 28	*/
	E_FJ_PCIE_MSI_NUM_29,		/**< msi number 29	*/
	E_FJ_PCIE_MSI_NUM_30,		/**< msi number 30	*/
	E_FJ_PCIE_MSI_NUM_31		/**< msi number 31	*/
} E_FJ_PCIE_MSI_NUM;

/** Type is defined to Callback function pointer for PCIe */
typedef void (*PCIE_CALLBACK)( UINT32 status );

/** DMA control data */
typedef struct{
	E_FJ_PCIE_CH		ch;					/**< PCIe channel							*/
	UINT32				src_addr_low;		/**< DMA transfer source lower address		*/
	UINT32				src_addr_upp;		/**< DMA transfer source upper address		*/
	UINT32				dst_addr_low;		/**< DMA transfer destination lower address	*/
	UINT32				dst_addr_upp;		/**< DMA transfer destination upper address	*/
	UINT32				trans_size;			/**< DMA transfer total size				*/
	E_FJ_PCIE_DMA_DIR	direction;			/**< DMA transfer direction					*/
	PCIE_CALLBACK		int_dma_callback;	/**< Callback function of the DMA interrupt	*/
} T_FJ_PCIE_DMA_CTRL;

/** Memory control data */
typedef struct{
	E_FJ_PCIE_CH	ch;						/**< PCIe channel	*/
	UINT32			base_adr;				/**< AXI slave memory base address.<br>
													Please set the 16-31 bit.<br>
													0-15 bit not used.<br>
													The start address must be aligned to a 64 kB boundary.<br>
													PCIe ch0 PIO area : 0x19400000 .. 0x197FFFFF<br>
													PCIe ch1 PIO area : 0x19800000 .. 0x198FFFFF
											*/
	UINT32			limit_adr;				/**< AXI slave memory limit address.<br>
													Please set the 16-31 bit.<br>
													0-15 bit not used.<br>
													The end address must be aligned to a 64 kB boundary.<br>
													PCIe ch0 PIO area : 0x19400000 .. 0x197FFFFF<br>
													PCIe ch1 PIO area : 0x19800000 .. 0x198FFFFF
											*/
	UINT32			target_adr_low;			/**< AXI slave memory target lower address.<br>
													Please set the 16-31 bit.<br>
													0-15 bit not used.<br>
													The start address must be aligned to a 64 kB boundary.<br>
													Please set the address of the Endpoint.
											*/
	UINT32			target_adr_upp;			/**< AXI slave memory target upper address.	*/
} T_FJ_PCIE_MEMORY_CTRL;

/*@} --- end of fj_pcie_definition */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_pcie_api
@{*/
/*@} --- end of fj_pcie_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_pcie_message
@{*/
/*@} --- end of fj_pcie_message */

#endif	// _FJ_PCIE_H_
