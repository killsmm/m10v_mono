/**
 * @file		_jmlexs.h
 * @brief		Definition JMLEXS Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "__jmlexs.h"

/*	Structure of XDMAC LowPower Control	(1B11_0000h)	*/
union io_exstop_xdmaclowpowctrl {
	unsigned long		word;
	struct {
		unsigned long	CSYSREQ				:1;		// CSYS Request
		unsigned long						:7;		// reserved
		unsigned long	CSYSACK				:1;		// CSYS Acknowlege
		unsigned long						:7;		// reserved
		unsigned long	CACTIVE				:1;		// C Active
		unsigned long						:15;	// reserved
    } bit;
};

/*	Structure of SoftReset	(1B11_000Ch)	*/
union io_exstop_softreset {
	unsigned long		word;
	struct {
		unsigned long	NFRSTN				:1;		// NF Reset
		unsigned long	NFRSTNBCH			:1;		// NF Reset Bch
		unsigned long	NFRSTNREG			:1;		// NF Reset Reg
		unsigned long						:5;		// reserved
		unsigned long	NETSECRST			:1;		// NETSEC Reset
		unsigned long						:7;		// reserved
		unsigned long	EMCRST				:1;		// eMMC Reset x
		unsigned long						:7;		// reserved
		unsigned long	RELCRST				:1;		// RELC Reset
		unsigned long						:7;		// reserved
    } bit;
};

/*	Structure of SoftReset 2	(1B11_0010h)	*/
union io_exstop_softreset2 {
	unsigned long		word;
	struct {
		unsigned long	DEBRSTX_reserve		:1;		// reserved
		unsigned long	DEBRSTX_SD3			:1;		// SD3( UHS2 )
		unsigned long	DEBRSTX_SD4			:1;		// SD4( UHS1 Card )
		unsigned long	DEBRSTX_PCIE0		:1;		// PCIe0
		unsigned long	DEBRSTX_PCIE1		:1;		// PCIe1
		unsigned long	DEBRSTX_USB2VBUS	:1;		// USB2 VBUS
		unsigned long	DEBRSTX_USB2ID		:1;		// USB2 ID
		unsigned long	DEBRSTX_USB3VBUS	:1;		// USB3 VBUS
		unsigned long	DEBRSTX_USB3ID		:1;		// USB3 ID
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of INTMSK register	(1B11_0014h)	*/
union io_exstop_intmsk {
	unsigned long		word;
	struct {
		unsigned long	INTMSK00			:1;		// reserved
		unsigned long	INTMSK01			:1;		// reserved
		unsigned long	INTMSK02			:1;		// SD3 (UHS2) INT Mask(Rising Edge)
		unsigned long	INTMSK03			:1;		// SD3 (UHS2) INT Mask(Falling Edge)
		unsigned long	INTMSK04			:1;		// SD4 (UHS1 Card) INT Mask(Rising Edge)
		unsigned long	INTMSK05			:1;		// SD4 (UHS1 Card) INT Mask(Falling Edge)
		unsigned long	INTMSK06			:1;		// PCIe0 INT Mask(Rising Edge)
		unsigned long	INTMSK07			:1;		// PCIe0 INT Mask(Falling Edge)
		unsigned long	INTMSK08			:1;		// PCIe1 INT Mask(Rising Edge)
		unsigned long	INTMSK09			:1;		// PCIe1 INT Mask(Falling Edge)
		unsigned long	INTMSK10			:1;		// USB2 VBUS INT Mask(Rising Edge)
		unsigned long	INTMSK11			:1;		// USB2 VBUS INT Mask(Falling Edge)
		unsigned long	INTMSK12			:1;		// USB2 ID INT Mask(Rising Edge)
		unsigned long	INTMSK13			:1;		// USB2 ID INT Mask(Falling Edge)
		unsigned long	INTMSK14			:1;		// USB3 VBUS INT Mask(Rising Edge)
		unsigned long	INTMSK15			:1;		// USB3 VBUS INT Mask(Falling Edge)
		unsigned long	INTMSK16			:1;		// USB3 ID INT Mask(Rising Edge)
		unsigned long	INTMSK17			:1;		// USB3 ID INT Mask(Falling Edge)
		unsigned long						:14;	// reserved
    } bit;
};

/*	Structure of SIGMON register	(1B11_0018h)	*/
union io_exstop_sigmon {
	unsigned long		word;
	struct {
		unsigned long	SIGMON_reserve		:1;		// SIGMON reserved
		unsigned long	SIGMON_SD3			:1;		// SIGMON SD3( UHS2 )
		unsigned long	SIGMON_SD4			:1;		// SIGMON SD4( UHS1 Card )
		unsigned long	SIGMON_PCIE0		:1;		// SIGMON PCIe0
		unsigned long	SIGMON_PCIE1		:1;		// SIGMON PCIe1
		unsigned long	SIGMON_USB2VBUS		:1;		// SIGMON USB2 VBUS
		unsigned long	SIGMON_USB2ID		:1;		// SIGMON USB2 ID
		unsigned long	SIGMON_USB3VBUS		:1;		// SIGMON USB3 VBUS
		unsigned long	SIGMON_USB3ID		:1;		// SIGMON USB3 ID
		unsigned long						:7;		// reserved
		unsigned long	SIGMON2_reserve		:1;		// SIGMON2 reserved
		unsigned long	SIGMON2_SD3			:1;		// SIGMON2 SD3( UHS2 )
		unsigned long	SIGMON2_SD4			:1;		// SIGMON2 SD4( UHS1 Card )
		unsigned long	SIGMON2_PCIE0		:1;		// SIGMON2 PCIe0
		unsigned long	SIGMON2_PCIE1		:1;		// SIGMON2 PCIe1
		unsigned long	SIGMON2_USB2VBUS	:1;		// SIGMON2 USB2 VBUS
		unsigned long	SIGMON2_USB2ID		:1;		// SIGMON2 USB2 ID
		unsigned long	SIGMON2_USB3VBUS	:1;		// SIGMON2 USB3 VBUS
		unsigned long	SIGMON2_USB3ID		:1;		// SIGMON2 USB3 ID
		unsigned long						:7;		// reserved
    } bit;
};

/*	Structure of DEBEN register	(1B11_001Ch)	*/
union io_exstop_deben {
	unsigned long		word;
	struct {
		unsigned long	DEBEN_reserve		:1;		// DEBEN reserved
		unsigned long	DEBEN_SD3			:1;		// DEBEN SD3( UHS2 )
		unsigned long	DEBEN_SD4			:1;		// DEBEN SD4( UHS1 Card )
		unsigned long	DEBEN_PCIE0			:1;		// DEBEN PCIe0
		unsigned long	DEBEN_PCIE1			:1;		// DEBEN PCIe1
		unsigned long	DEBEN_USB2VBUS		:1;		// DEBEN USB2 VBUS
		unsigned long	DEBEN_USB2ID		:1;		// DEBEN USB2 ID
		unsigned long	DEBEN_USB3VBUS		:1;		// DEBEN USB3 VBUS
		unsigned long	DEBEN_USB3ID		:1;		// DEBEN USB3 ID
		unsigned long						:1;		// reserved
		unsigned long	SD2DETIM			:2;		// reserved
		unsigned long	SD3DETIM			:2;		// SD3( UHS2 )
		unsigned long	SD4DETIM			:2;		// SD4( UHS1 Card )
		unsigned long	PCIE0DETIM			:2;		// PCIe0
		unsigned long	PCIE1DETIM			:2;		// PCIe1
		unsigned long	USB2VBUSDETIM		:2;		// USB2 VBUS
		unsigned long	USB2IDDETIM			:2;		// USB2 ID
		unsigned long	USB3VBUSDETIM		:2;		// USB3 VBUS
		unsigned long	USB3IDDETIM			:2;		// USB3 ID
		unsigned long						:4;		// reserved
    } bit;
};

/*	Structure of CDINS MOD Control	(1B11_0020h)	*/
union io_exstop_cdimodctrl {
	unsigned long		word;
	struct {
		unsigned long	CDIMOD_reserve		:1;		// CDIMOD reserved
		unsigned long	CDIMOD_SD3			:1;		// CDIMOD SD3( UHS2 )
		unsigned long	CDIMOD_SD4			:1;		// CDIMOD SD4( UHS1 Card )
		unsigned long	CDIMOD_PCIE0		:1;		// CDIMOD PCIe0
		unsigned long	CDIMOD_PCIE1		:1;		// CDIMOD PCIe1
		unsigned long	CDIMOD_USB2VBUS		:1;		// CDIMOD USB2 VBUS
		unsigned long	CDIMOD_USB2ID		:1;		// CDIMOD USB2 ID
		unsigned long	CDIMOD_USB3VBUS		:1;		// CDIMOD USB3 VBUS
		unsigned long	CDIMOD_USB3ID		:1;		// CDIMOD USB3 ID
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of RELC HPROT Control	(1B11_0030h)	*/
union io_exstop_relchptctrl {
	unsigned long		word;
	struct {
		unsigned long	RERHPT				:4;		// RELC R HPROT
		unsigned long						:4;		// reserved
		unsigned long	REWHPT				:4;		// RELC W HPROT
		unsigned long						:20;	// reserved
    } bit;
};

/*	Structure of RAM PD Control	(1B11_0040h)	*/
union io_exstop_rampdctrl {
	unsigned long		word;
	struct {
		unsigned long	RAMPD_SD2			:1;		// SD2( UHS1 Wifi )
		unsigned long	RAMPD_SD3			:1;		// SD3( UHS2 )
		unsigned long	RAMPD_SD4			:1;		// SD4( UHS1 Card )
		unsigned long	RAMPD_NF			:1;		// Nand Flash
		unsigned long	RAMPD_RELC			:1;		// RELC
		unsigned long	RAMPD_NETSEC		:1;		// NETSEC
		unsigned long	RAMPD_USB2			:1;		// USB2
		unsigned long	RAMPD_USB3			:1;		// USB3
		unsigned long	RAMPD_PCIE0			:1;		// PCIe0
		unsigned long	RAMPD_PCIE1			:1;		// PCIe1
		unsigned long	RAMPD_EMMC			:1;		// eMMC
		unsigned long						:21;	// reserved
    } bit;
};

/*	Structure of BUS CLOCK Stop Control	(1B11_0060h)	*/
union io_exstop_busckstpctrl {
	unsigned long		word;
	struct {
		unsigned long	NF_CKSTP			:1;		// Nand Flash
		unsigned long	PCIE0_CKSTP			:1;		// PCIe0
		unsigned long	PCIE1_CKSTP			:1;		// PCIe1
		unsigned long	XDMAC_CKSTP			:1;		// XDMAC
		unsigned long	HDMAC_CKSTP			:1;		// HDMAC
		unsigned long	EMMC_CKSTP			:1;		// eMMC
		unsigned long	SD2_CKSTP			:1;		// SD2( Wifi )
		unsigned long	SD3_CKSTP			:1;		// SD3( UHS2 )
		unsigned long	SD4_CKSTP			:1;		// SD4( UHS1 Card )
		unsigned long	NETSEC_CKSTP		:1;		// NETSEC
		unsigned long	RELC_CKSTP			:1;		// RELC
		unsigned long						:21;	// reserved
    } bit;
};

/*	Structure of NETSEC PTPCLK Stop Control	(1B11_0070h)	*/
union io_exstop_netsecptpclkstopctrl {
	unsigned long		word;
	struct {
		unsigned long	NETSECPTPSTP		:1;		// NETSEC PTP_CLK Stop Control
		unsigned long						:31;	// reserved
    } bit;
};

/*	Structure of EMMC_CKSTP register	(1B11_0074h)	*/
union io_exstop_emmcckstp {
	unsigned long		word;
	struct {
		unsigned long	EMCLKSTP			:1;		// MMC_BCLK_I Control
		unsigned long						:31;	// reserved
    } bit;
};

/*	Structure of EM_DRVSTR register	(1B11_0090h)	*/
union io_exstop_emdrvstr {
	unsigned long		word;
	struct {
		unsigned long	EM_DSEL				:2;		// EM_DSEL
		unsigned long						:30;	// reserved
    } bit;
};

/*	Structure of EM_PV_DTIMEC register	(1B11_00B4h)	*/
union io_exstop_empvdtimec {
	unsigned long		word;
	struct {
		unsigned long	PV_DTIMEC			:24;	// PV_DTIMEC
		unsigned long						:8;		// reserved
    } bit;
};

/*	Structure of EM_PV_AMPBL register	(1B11_00B8h)	*/
union io_exstop_empvampbl {
	unsigned long		word;
	struct {
		unsigned long	PV_AMPBL			:4;		// PV_AMPBL
		unsigned long						:28;	// reserved
    } bit;
};

/*	Structure of EM_CR_SLOTTYPE register	(1B11_00BCh)	*/
union io_exstop_emcrslottype {
	unsigned long		word;
	struct {
		unsigned long	CR_SLOTTYPE			:1;		// CR_SLOTTYPE
		unsigned long						:31;	// reserved
    } bit;
};

/*	Structure of EM_CR_BCLKFREQ register	(1B11_00C0h)	*/
union io_exstop_emcrbclkfreq {
	unsigned long		word;
	struct {
		unsigned long	CR_BCLKFREQ			:8;		// CR_BCLKFREQ
		unsigned long						:24;	// reserved
    } bit;
};

/*	Structure of EM_CDDET register	(1B11_00C4h)	*/
union io_exstop_emcddet {
	unsigned long		word;
	struct {
		unsigned long	EMCD				:1;		// EMCD
		unsigned long						:7;		// reserved
		unsigned long	EMPHYLK				:1;		// EMPHYLK
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of USB2_CKCTL register	(1B11_0200h)	*/
union io_exstop_usb2ckctl {
	unsigned long		word;
	struct {
		unsigned long	HDC_HCLKEN			:1;		// HDC_HCLKEN
		unsigned long						:7;		// reserved
		unsigned long	HO_HCLKEN			:1;		// HO_HCLKEN
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of USB2_RCTL register	(1B11_0204h)	*/
union io_exstop_usb2rctl {
	unsigned long		word;
	struct {
		unsigned long	HDC_SRST			:1;		// HDC_SRST
		unsigned long						:7;		// reserved
		unsigned long	HO_SRST				:1;		// HO_SRST
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of USB2_ANPD register	(1B11_0208h)	*/
union io_exstop_usb2anpd {
	unsigned long		word;
	struct {
		unsigned long	ANPD				:1;		// ANPD
		unsigned long						:31;	// reserved
    } bit;
};

/*	Structure of USB2_HFSEL register	(1B11_020Ch)	*/
union io_exstop_usb2hfsel {
	unsigned long		word;
	struct {
		unsigned long	HF_SEL				:1;		// HF_SEL
		unsigned long						:31;	// reserved
    } bit;
};

/*	Structure of USB2_FSEL register	(1B11_0210h)	*/
union io_exstop_usb2fsel {
	unsigned long		word;
	struct {
		unsigned long	FSEL				:3;		// FSEL
		unsigned long	PLLBTUNE			:1;		// PLLBTUNE
		unsigned long						:28;	// reserved
    } bit;
};

/*	Structure of USB2_EHCI_SB register	(1B11_0214h)	*/
union io_exstop_usb2ehcisb {
	unsigned long		word;
	struct {
		unsigned long	EHCI_XFER_CNT_O		:11;	// EHCI_XFER_CNT_O
		unsigned long	EHCI_XFER_PRDC_O	:1;		// EHCI_XFER_PRDC_O
		unsigned long	EHCI_BUFACC_O		:1;		// EHCI_BUFACC_O
		unsigned long	MCU_INC_FRDX_O		:1;		// MCU_INC_FRDX_O
		unsigned long	EOR_RUNSTOP_O		:1;		// EOR_RUNSTOP_O
		unsigned long						:1;		// reserved
		unsigned long	EHCI_USBSTS_O		:6;		// EHCI_USBSTS_O
		unsigned long						:10;	// reserved
    } bit;
};

/*	Structure of USB2_EHCI_STRAP register	(1B11_0218h)	*/
union io_exstop_usb2ehcistrap {
	unsigned long		word;
	struct {
		unsigned long	SS_FADJ_HOST_I		:6;		// SS_FADJ_HOST_I
		unsigned long						:2;		// reserved
		unsigned long	SS_FLADJ_VAL_I		:6;		// SS_FLADJ_VAL_I
		unsigned long						:18;	// reserved
    } bit;
};

/*	Structure of USB2_OHCI_IF register	(1B11_021Ch)	*/
union io_exstop_usb2ohciif {
	unsigned long		word;
	struct {
		unsigned long	OHCI_CLKSEL_I		:1;		// OHCI_CLKSEL_I
		unsigned long						:7;		// reserved
		unsigned long	UTMI_SUSPEND_O_N	:1;		// UTMI_SUSPEND_O_N
		unsigned long	OHCI_GSUSPEND_O		:1;		// OHCI_GSUSPEND_O
		unsigned long	OHCI_RWE_O			:1;		// OHCI_RWE_O
		unsigned long	OHCI_RMTWKP_O		:1;		// OHCI_RMTWKP_O
		unsigned long	OHCI_DRWE_O			:1;		// OHCI_DRWE_O
		unsigned long	OHCI_CCS_O			:1;		// OHCI_CCS_O
		unsigned long	OHCI_SOF_O_N		:1;		// OHCI_SOF_O_N
		unsigned long	OHCI_BUFACC_O		:1;		// OHCI_BUFACC_O
		unsigned long						:16;	// reserved
    } bit;
};

/*	Structure of USB2_AHB_SLVIF register	(1B11_0220h)	*/
union io_exstop_usb2ahbslvif {
	unsigned long		word;
	struct {
		unsigned long	REG_RDY_O			:1;		// REG_RDY_O
		unsigned long						:31;	// reserved
    } bit;
};

/*	Structure of USB2_LMODSET register	(1B11_0224h)	*/
union io_exstop_usb2lmodset {
	unsigned long		word;
	struct {
		unsigned long	SS_ENA_INCR4_I		:1;		// SS_ENA_INCR4_I
		unsigned long	SS_ENA_INCR8_I		:1;		// SS_ENA_INCR8_I
		unsigned long	SS_ENA_INCR16_I		:1;		// SS_ENA_INCR16_I
		unsigned long	SS_ENA_INC_AL_I		:1;		// SS_ENA_INC_AL_I
		unsigned long	SS_EHCI64B_EN_I		:1;		// SS_EHCI64B_EN_I
		unsigned long						:26;	// reserved
    } bit;
};

/*	Structure of USB2_PMODSET register	(1B11_0228h)	*/
union io_exstop_usb2pmodset {
	unsigned long		word;
	struct {
		unsigned long	RPDPEN_I			:1;		// RPDPEN_I
		unsigned long						:7;		// reserved
		unsigned long	RPDMEN_I			:1;		// RPDMEN_I
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of USB2_HPT_CTL register	(1B11_022Ch)	*/
union io_exstop_usb2hptctl {
	unsigned long		word;
	struct {
		unsigned long	USB2_EHPT			:4;		// RPDPEN_I
		unsigned long						:4;		// reserved
		unsigned long	USB2_OHPT			:4;		// RPDMEN_I
		unsigned long						:20;	// reserved
    } bit;
};

/*	Structure of USB2_IDVBUSCTL register	(1B11_0230h)	*/
union io_exstop_usb2idvbusctl {
	unsigned long		word;
	struct {
		unsigned long	USB2_IDPU_VAL		:1;		// USB2_IDPU_VAL
		unsigned long						:3;		// reserved
		unsigned long	USB2_IDPU_SEL		:1;		// USB2_IDPU_SEL
		unsigned long						:3;		// reserved
		unsigned long	USB2_ID_SEL			:1;		// USB2_ID_SEL
		unsigned long						:3;		// reserved
		unsigned long	USB2_ID_VAL			:1;		// USB2_ID_VAL
		unsigned long						:3;		// reserved
		unsigned long	USB2_VB_SEL			:1;		// USB2_VB_SEL
		unsigned long						:3;		// reserved
		unsigned long	USB2_VB_VAL			:1;		// USB2_VB_VAL
		unsigned long						:3;		// reserved
		unsigned long	USB2_VBDET_SEL		:2;		// USB2_VBDET_SEL
		unsigned long						:2;		// reserved
		unsigned long	USB2_VBDET_VAL		:1;		// USB2_VBDET_VAL
		unsigned long						:3;		// reserved
    } bit;
};

/*	Structure of USB2_HDMAC1 register	(1B11_0238h)	*/
union io_exstop_usb2hdmac1 {
	unsigned long		word;
	struct {
		unsigned long	HDMAC_BC1			:4;		// HDMAC_BC1
		unsigned long						:4;		// reserved
		unsigned long	HDMAC_BT1			:2;		// HDMAC_BT1
		unsigned long						:6;		// reserved
		unsigned long	HDMAC_MS1			:1;		// HDMAC_MS1
		unsigned long						:15;	// reserved
    } bit;
};

/*	Structure of USB2_HDMAC2 register	(1B11_023Ch)	*/
union io_exstop_usb2hdmac2 {
	unsigned long		word;
	struct {
		unsigned long	HDMAC_BC2			:4;		// HDMAC_BC2
		unsigned long						:4;		// reserved
		unsigned long	HDMAC_BT2			:2;		// HDMAC_BT2
		unsigned long						:6;		// reserved
		unsigned long	HDMAC_MS2			:1;		// HDMAC_MS2
		unsigned long						:15;	// reserved
    } bit;
};

/*	Structure of USB2_DMAFSM1 register	(1B11_0240h)	*/
union io_exstop_usb2dmafsm1 {
	unsigned long		word;
	struct {
		unsigned long	FSMRST1				:1;		// FSMRST1
		unsigned long	DMA_TRANS_STATE1	:1;		// DMA_TRANS_STATE1
		unsigned long						:30;	// reserved
    } bit;
};

/*	Structure of USB2_DMAFSM2 register	(1B11_0244h)	*/
union io_exstop_usb2dmafsm2 {
	unsigned long		word;
	struct {
		unsigned long	FSMRST2				:1;		// FSMRST2
		unsigned long	DMA_TRANS_STATE2	:1;		// DMA_TRANS_STATE2
		unsigned long						:30;	// reserved
    } bit;
};

/*	Structure of USB3_CKCTL register	(1B11_0280h)	*/
union io_exstop_usb3ckctl {
	unsigned long		word;
	struct {
		unsigned long	ACLKEN				:1;		// ACLKEN
		unsigned long						:30;	// reserved
    } bit;
};

/*	Structure of USB3_RCTL register	(1B11_0284h)	*/
union io_exstop_usb3rctl {
	unsigned long		word;
	struct {
		unsigned long	SFTRST				:1;		// SFTRST
		unsigned long						:7;		// reserved
		unsigned long	PHY_RESET_N			:1;		// PHY_RESET_N
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of USB3_RCV_SNS register	(1B11_0288h)	*/
union io_exstop_usb3rcvsns {
	unsigned long		word;
	struct {
		unsigned long	ACJT_LEVEL			:5;		// ACJT_LEVEL
		unsigned long						:27;	// reserved
    } bit;
};

/*	Structure of USB3_FSEL register	(1B11_028Ch)	*/
union io_exstop_usb3fsel {
	unsigned long		word;
	struct {
		unsigned long	FSEL3				:6;		// FSEL3
		unsigned long						:2;		// reserved
		unsigned long	MPLL_MULTIPLIER		:7;		// MPLL_MULTIPLIER
		unsigned long						:1;		// reserved
		unsigned long	SSC_REF_CLK_SEL		:9;		// SSC_REF_CLK_SEL
		unsigned long						:3;		// reserved
		unsigned long	REF_CLKDIV2			:1;		// REF_CLKDIV2
		unsigned long						:3;		// reserved
    } bit;
};

/*	Structure of USB3_RFC_CTL register	(1B11_0290h)	*/
union io_exstop_usb3rfcctl {
	unsigned long		word;
	struct {
		unsigned long	REF_SSP_EN			:1;		// REF_SSP_EN
		unsigned long						:7;		// reserved
		unsigned long	REF_USE_PAD			:1;		// REF_USE_PAD
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of USB3_HO_IF register	(1B11_0294h)	*/
union io_exstop_usb3hoif {
	unsigned long		word;
	struct {
		unsigned long	PCI_IMPLEMENT		:1;		// PCI_IMPLEMENT
		unsigned long						:7;		// reserved
		unsigned long	HOST_MSI_ENABLE		:1;		// HOST_MSI_ENABLE
		unsigned long						:7;		// reserved
		unsigned long	XHC_BME				:1;		// XHC_BME
		unsigned long	XHC_AC64			:1;		// XHC_AC64
		unsigned long						:6;		// reserved
		unsigned long	HOST_SYSTEM_ERR		:1;		// HOST_SYSTEM_ERR
		unsigned long						:7;		// reserved
    } bit;
};

/*	Structure of USB3_JT_ADJ register	(1B11_0298h)	*/
union io_exstop_usb3jtadj {
	unsigned long		word;
	struct {
		unsigned long	FLADJ_30MHZ_REG		:6;		// FLADJ_30MHZ_REG
		unsigned long						:10;	// reserved
		unsigned long	USB3_OVRCUR_EN		:1;		// USB3_OVRCUR_EN
		unsigned long						:15;	// reserved
    } bit;
};

/*	Structure of USB3_IDVBUSCTL register	(1B11_02A0h)	*/
union io_exstop_usb3idvbusctl {
	unsigned long		word;
	struct {
		unsigned long	USB3_IDPU_VAL		:1;		// USB3_IDPU_VAL
		unsigned long						:7;		// reserved
		unsigned long	USB3_ID_SEL			:1;		// USB3_ID_SEL
		unsigned long						:3;		// reserved
		unsigned long	USB3_ID_VAL			:1;		// USB3_ID_VAL
		unsigned long						:3;		// reserved
		unsigned long	USB3_VB_SEL			:1;		// USB3_VB_SEL
		unsigned long						:3;		// reserved
		unsigned long	USB3_VB_VAL			:1;		// USB3_VB_VAL
		unsigned long						:3;		// reserved
		unsigned long	USB3_VBDET_SEL		:2;		// USB3_VBDET_SEL
		unsigned long						:2;		// reserved
		unsigned long	USB3_VBDET_VAL		:1;		// USB3_VBDET_VAL
		unsigned long						:3;		// reserved
    } bit;
};

/*	Structure of NF_IP_CTL register	(1B11_0300h)	*/
union io_exstop_nfipctl {
	unsigned long		word;
	struct {
		unsigned long	DSCV_INHIBIT		:1;		// Nand Flash IP ( cdns_hpnfc ) discovery_inhibit Control
		unsigned long	DSCV_IGNR_CRC		:1;		// Nand Flash IP ( cdns_hpnfc ) discovery_ignore_crc Control
		unsigned long						:2;		// reserved
		unsigned long	BOOT_EN				:1;		// Nand Flash IP ( cdns_hpnfc ) boot_en Control
		unsigned long						:3;		// reserved
		unsigned long	IDDQ_EN				:1;		// Nand Flash IP ( cdns_flash_dll_phy ) iddq_en Control
		unsigned long						:23;	// reserved
    } bit;
};

/*	Structure of NF_BTSQ_STATE register	(1B11_0304h)	*/
union io_exstop_nfbtsqstate {
	unsigned long		word;
	struct {
		unsigned long	INIT_FAIL			:1;		// Nand Flash IP ( cdns_hpnfc ) init_fail monitor
		unsigned long						:3;		// reserved
		unsigned long	BOOT_COMP			:1;		// Nand Flash IP ( cdns_hpnfc ) boot_comp monitor
		unsigned long						:3;		// reserved
		unsigned long	BOOT_ERROR			:1;		// Nand Flash IP ( cdns_hpnfc ) boot_error monitor
		unsigned long						:3;		// reserved
		unsigned long	CTRL_BUSY			:1;		// Nand Flash IP ( cdns_hpnfc ) ctrl_busy monitor
		unsigned long						:19;	// reserved
    } bit;
};

/*	Structure of NF_RB_VALID_TM register	(1B11_0308h)	*/
union io_exstop_nfbvalidtm {
	unsigned long		word;
	struct {
		unsigned long	RB_VALID_TIME		:16;	// Nand Flash IP ( cdns_hpnfc ) rb_valid_time[15:0] Control
		unsigned long						:16;	// reserved
    } bit;
};

/*	Structure of NF_PHY_CTRL_REG register	(1B11_030Ch)	*/
union io_exstop_nfphyctrlreg {
	unsigned long		word;
	struct {
		unsigned long	PHY_CTRL			:32;	// Nand Flash IP ( cdns_hpnfc )  phy_ctrl_reg[31:0] Control
    } bit;
};

/*	Structure of NF_PHY_DQ_TIM register	(1B11_0310h)	*/
union io_exstop_nfphydqtim {
	unsigned long		word;
	struct {
		unsigned long	PHY_DQ_TIMING		:32;	// Nand Flash IP ( cdns_hpnfc )  phy_dq_timing[31:0] Control
    } bit;
};

/*	Structure of NF_PHY_DQS_TIM register	(1B11_0314h)	*/
union io_exstop_nfphydqstim {
	unsigned long		word;
	struct {
		unsigned long	PHY_DQS_TIMING		:32;	// Nand Flash IP ( cdns_hpnfc )  phy_dqs_timing[31:0] Control
    } bit;
};

/*	Structure of NF_PHY_GT_LPBK register	(1B11_0318h)	*/
union io_exstop_nfphygtlpbk {
	unsigned long		word;
	struct {
		unsigned long	PHY_GT_LPBK_CTL		:32;	// Nand Flash IP ( cdns_hpnfc )  phy_gate_lpbk_ctrl_reg[31:0] Control
    } bit;
};

/*	Structure of NF_PHY_DLL_MCTL register	(1B11_031Ch)	*/
union io_exstop_nfphydllmctl {
	unsigned long		word;
	struct {
		unsigned long	PHY_DLL_MST_CTL		:32;	// Nand Flash IP ( cdns_hpnfc )  phy_dll_master_ctrl_reg[31:0] Control
    } bit;
};

/*	Structure of NF_PHY_DLL_SCTL register	(1B11_0320h)	*/
union io_exstop_nfphydllsctl {
	unsigned long		word;
	struct {
		unsigned long	PHY_DLL_SLV_CTL		:32;	// Nand Flash IP ( cdns_hpnfc )  phy_dll_slave_ctrl_reg[31:0] Control
    } bit;
};

/*	Structure of NF_BOOT_ECC_CTL register	(1B11_0324h)	*/
union io_exstop_nfbooteccctl {
	unsigned long		word;
	struct {
		unsigned long	BOOT_ECC_SEC_SIZ	:16;	// Nand Flash IP ( cdns_hpnfc )  boot_ecc_sec_size[15:0] Control
		unsigned long	BT_ECC_CORR_STR		:3;		// Nand Flash IP ( cdns_hpnfc )  boot_ecc_corr_str[2:0] Control
		unsigned long						:5;		// reserved
		unsigned long	BOOT_ECC_EN			:1;		// Nand Flash IP ( cdns_hpnfc )  boot_ecc_enable Control
		unsigned long						:7;		// reserved
    } bit;
};

/*	Structure of NF_NFWPX_CNT register	(1B11_0328h)	*/
union io_exstop_nfwpxcnt {
	unsigned long		word;
	struct {
		unsigned long	NFWPX_CNT			:1;		// Nand Flash IP ( cdns_hpnfc )  NFWPX mask Control
		unsigned long						:31;	// reserved
    } bit;
};

/*	Structure of INTSTAT register	(1B11_0F00h)	*/
union io_exstop_intstat {
	unsigned long		word;
	struct {
		unsigned long	__INTSTAT00			:1;		// SD2 (UHS1 Wifi) rising edge detection
		unsigned long	__INTSTAT01			:1;		// SD2 (UHS1 Wifi) falling edge detection
		unsigned long	__INTSTAT02			:1;		// SD3 (UHS2) rising edge detection
		unsigned long	__INTSTAT03			:1;		// SD3 (UHS2) falling edge detection
		unsigned long	__INTSTAT04			:1;		// SD4 (UHS1 Card) rising edge detection
		unsigned long	__INTSTAT05			:1;		// SD4 (UHS1 Card) falling edge detection
		unsigned long	__INTSTAT06			:1;		// PCIe0 rising edge detection
		unsigned long	__INTSTAT07			:1;		// PCIe0 falling edge detection
		unsigned long	__INTSTAT08			:1;		// PCIe1 rising edge detection
		unsigned long	__INTSTAT09			:1;		// PCIe1 falling edge detection
		unsigned long	__INTSTAT10			:1;		// USB2 VBUS rising edge detection
		unsigned long	__INTSTAT11			:1;		// USB2 VBUS falling edge detection
		unsigned long	__INTSTAT12			:1;		// USB2 ID rising edge detection
		unsigned long	__INTSTAT13			:1;		// USB2 ID falling edge detection
		unsigned long	__INTSTAT14			:1;		// USB3 VBUS rising edge detection
		unsigned long	__INTSTAT15			:1;		// USB3 VBUS falling edge detection
		unsigned long	__INTSTAT16			:1;		// USB3 ID rising edge detection
		unsigned long	__INTSTAT17			:1;		// USB3 ID falling edge detection
		unsigned long						:14;	// reserved
    } bit;
};

struct io_exstop {
	union io_exstop_xdmaclowpowctrl			XDMACLPCTRL;							/*	1B11_(0000 - 0003h)	*/
	unsigned char							dmy_exstop_0004_000B[0x000C-0x0004];	/*	1B11_(0004 - 000Bh)	*/
	union io_exstop_softreset				SOFTRESET;								/*	1B11_(000C - 000Fh)	*/
	union io_exstop_softreset2				SOFTRESET2;								/*	1B11_(0010 - 0013h)	*/
	union io_exstop_intmsk					INTMSK;									/*	1B11_(0014 - 0017h)	*/
	union io_exstop_sigmon					SIGMON;									/*	1B11_(0018 - 001Bh)	*/
	union io_exstop_deben					DEBEN;									/*	1B11_(001C - 001Fh)	*/
	union io_exstop_cdimodctrl				CDINSMDCTL;								/*	1B11_(0020 - 0023h)	*/
	unsigned char							dmy_exstop_0024_002F[0x0030-0x0024];	/*	1B11_(0024 - 002Fh)	*/
	union io_exstop_relchptctrl				RLC_HPT_CTL;							/*	1B11_(0030 - 0033h)	*/
	unsigned char							dmy_exstop_0034_003F[0x0040-0x0034];	/*	1B11_(0034 - 003Fh)	*/
	union io_exstop_rampdctrl				RAM_PD;									/*	1B11_(0040 - 0043h)	*/
	unsigned char							dmy_exstop_0044_005F[0x0060-0x0044];	/*	1B11_(0044 - 005Fh)	*/
	union io_exstop_busckstpctrl			BUSCKSTP;								/*	1B11_(0060 - 0063h)	*/
	unsigned char							dmy_exstop_0064_006F[0x0070-0x0064];	/*	1B11_(0064 - 006Fh)	*/
	union io_exstop_netsecptpclkstopctrl	NSEC_CKSTP;								/*	1B11_(0070 - 0073h)	*/
	union io_exstop_emmcckstp				EMMC_CKSTP;								/*	1B11_(0074 - 0077h)	*/
	unsigned char							dmy_exstop_0078_008F[0x0090-0x0078];	/*	1B11_(0078 - 008Fh)	*/
	union io_exstop_emdrvstr				EM_DRVSTR;								/*	1B11_(0090 - 0093h)	*/
	unsigned char							dmy_exstop_0094_00B3[0x00B4-0x0094];	/*	1B11_(0094 - 00B3h)	*/
	union io_exstop_empvdtimec				EM_PV_DTIMEC;							/*	1B11_(00B4 - 00B7h)	*/
	union io_exstop_empvampbl				EM_PV_AMPBL;							/*	1B11_(00B8 - 00BBh)	*/
	union io_exstop_emcrslottype			EM_CR_SLOTTYPE;							/*	1B11_(00BC - 00BFh)	*/
	union io_exstop_emcrbclkfreq			EM_CR_BCLKFREQ;							/*	1B11_(00C0 - 00C3h)	*/
	union io_exstop_emcddet					EM_CDDET;								/*	1B11_(00C4 - 00C7h)	*/
	unsigned char							dmy_exstop_00C8_01FF[0x0200-0x00C8];	/*	1B11_(00C8 - 01FFh)	*/
	union io_exstop_usb2ckctl				USB2_CKCTL;								/*  1B11_(0200 - 0203h)	*/
	union io_exstop_usb2rctl				USB2_RCTL;								/*  1B11_(0204 - 0207h)	*/
	union io_exstop_usb2anpd				USB2_ANPD;								/*  1B11_(0208 - 020Bh)	*/
	union io_exstop_usb2hfsel				USB2_HFSEL;								/*  1B11_(020C - 020Fh)	*/
	union io_exstop_usb2fsel				USB2_FSEL;								/*  1B11_(0210 - 0213h)	*/
	union io_exstop_usb2ehcisb				USB2_EHCI_SB;							/*  1B11_(0214 - 0217h)	*/
	union io_exstop_usb2ehcistrap			USB2_EHCI_STRAP;						/*  1B11_(0218 - 021Bh)	*/
	union io_exstop_usb2ohciif				USB2_OHCI_IF;							/*  1B11_(021C - 021Fh)	*/
	union io_exstop_usb2ahbslvif			USB2_AHB_SLVIF;							/*  1B11_(0220 - 0223h)	*/
	union io_exstop_usb2lmodset				USB2_LMODSET;							/*  1B11_(0224 - 0227h)	*/
	union io_exstop_usb2pmodset				USB2_PMODSET;							/*  1B11_(0228 - 022Bh)	*/
	union io_exstop_usb2hptctl				USB2_HPT_CTL;							/*  1B11_(022C - 022Fh)	*/
	union io_exstop_usb2idvbusctl			USB2_IDVBUSCTL;							/*  1B11_(0230 - 0233h)	*/
	unsigned char							dmy_exstop_0234_0237[0x0238-0x0234];	/*	1B11_(0234 - 0237h)	*/
	union io_exstop_usb2hdmac1				USB2_HDMAC1;							/*  1B11_(0238 - 023Bh)	*/
	union io_exstop_usb2hdmac2				USB2_HDMAC2;							/*  1B11_(023C - 023Fh)	*/
	union io_exstop_usb2dmafsm1				USB2_DMAFSM1;							/*  1B11_(0240 - 0243h)	*/
	union io_exstop_usb2dmafsm2				USB2_DMAFSM2;							/*  1B11_(0244 - 0247h)	*/
	unsigned char							dmy_exstop_0248_027F[0x0280-0x0248];	/*	1B11_(0248 - 027Fh)	*/
	union io_exstop_usb3ckctl				USB3_CKCTL;								/*  1B11_(0280 - 0283h)	*/
	union io_exstop_usb3rctl				USB3_RCTL;								/*  1B11_(0284 - 0287h)	*/
	union io_exstop_usb3rcvsns				USB3_RCV_SNS;							/*  1B11_(0288 - 028Bh)	*/
	union io_exstop_usb3fsel				USB3_FSEL;								/*  1B11_(028C - 028Fh)	*/
	union io_exstop_usb3rfcctl				USB3_RFC_CTL;							/*  1B11_(0290 - 0293h)	*/
	union io_exstop_usb3hoif				USB3_HO_IF;								/*  1B11_(0294 - 0298h)	*/
	union io_exstop_usb3jtadj				USB3_JT_ADJ;							/*  1B11_(0298 - 029Bh)	*/
	unsigned char							dmy_exstop_029C_029F[0x02A0-0x029C];	/*	1B11_(029C - 029Fh)	*/
	union io_exstop_usb3idvbusctl			USB3_IDVBUSCTL;							/*  1B11_(02A0 - 02A3h)	*/
	unsigned char							dmy_exstop_02A4_02FF[0x0300-0x02A4];	/*	1B11_(02A4 - 02FFh)	*/
	union io_exstop_nfipctl					NF_IP_CTL;								/*	1B11_(0300 - 0303h)	*/
	union io_exstop_nfbtsqstate				NF_BTSQ_STATE;							/*	1B11_(0304 - 0307h)	*/
	union io_exstop_nfbvalidtm				NF_RB_VALID_TM;							/*	1B11_(0308 - 030Bh)	*/
	union io_exstop_nfphyctrlreg			NF_PHY_CTRL_REG;						/*	1B11_(030C - 030Fh)	*/
	union io_exstop_nfphydqtim				NF_PHY_DQ_TIM;							/*	1B11_(0310 - 0313h)	*/
	union io_exstop_nfphydqstim				NF_PHY_DQS_TIM;							/*	1B11_(0314 - 0317h)	*/
	union io_exstop_nfphygtlpbk				NF_PHY_GT_LPBK;							/*	1B11_(0318 - 031Bh)	*/
	union io_exstop_nfphydllmctl			NF_PHY_DLL_MCTL;						/*	1B11_(031C - 031Fh)	*/
	union io_exstop_nfphydllsctl			NF_PHY_DLL_SCTL;						/*	1B11_(0320 - 0323h)	*/
	union io_exstop_nfbooteccctl			NF_BOOT_ECC_CTL;						/*	1B11_(0324 - 0327h)	*/
	union io_exstop_nfwpxcnt				NF_NFWPX_CNT;							/*	1B11_(0328 - 032Bh)	*/
	unsigned char							dmy_exstop_032C_0EFF[0x0F00-0x032C];	/*	1B11_(032C - 0EFFh)	*/
	union io_exstop_intstat					INTSTAT;								/*	1B11_(0F00 - 0F03h)	*/
	unsigned char							dmy_exstop_0F04_FFFF[0x10000-0x0F04];	/*	1B11_(0F04 - FFFFh)	*/
};


extern volatile struct io_gpv    IO_GPV;							// addr 19000000h-
extern volatile struct io_exstop IO_EXSTOP;							// addr 1B110000h-

