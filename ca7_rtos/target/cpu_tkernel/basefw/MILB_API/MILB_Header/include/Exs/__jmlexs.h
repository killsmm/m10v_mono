/**
 * @file		__jmlexs.h
 * @brief		Definition JMLEXS (GPV) Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* Structure of Address region control remap		(1900_0000h) */
union io_address_region_ctrl_remap {
	unsigned long		word;
	struct {
		unsigned long	RM				:8;
		unsigned long					:24;
	}bit;
};

/* Structure of M_Usb3_r_ib */
struct io_gpv_M_Usb3_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Usb3_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_Usb3_r_ib fn_mod2 Register
	unsigned char						dmy_0028_002B[0x002C - 0x0028];			// Base_(0028h - 002Bh) Reserved
	unsigned long						FML;									// Base_(002Ch - 002Fh) M_Usb3_r_ib fn_mod_lb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x0030];			// Base_(0030h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Usb3_r_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_Emmc_r_ib */
struct io_gpv_M_Emmc_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Emmc_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_Emmc_r_ib fn_mod2 Register
	unsigned char						dmy_0028_002B[0x002C - 0x0028];			// Base_(0028h - 002Bh) Reserved
	unsigned long						FML;									// Base_(002Ch - 002Fh) M_Emmc_r_ib fn_mod_lb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x0030];			// Base_(0030h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Emmc_r_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_Uhs2_r_ib */
struct io_gpv_M_Uhs2_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Uhs2_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_Uhs2_r_ib fn_mod2 Register
	unsigned char						dmy_0028_002B[0x002C - 0x0028];			// Base_(0028h - 002Bh) Reserved
	unsigned long						FML;									// Base_(002Ch - 002Fh) M_Uhs2_r_ib fn_mod_lb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x0030];			// Base_(0030h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Uhs2_r_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_Mxic1_m_ib */
struct io_gpv_M_Mxic1_m_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Mxic1_m_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_003F[0x0040 - 0x000C];			// Base_(000Ch - 003Fh) Reserved
	unsigned long						WT;										// Base_(0040h - 0043h) M_Mxic1_m_ib wr_tidemark Register
	unsigned char						dmy_0044_0107[0x0108 - 0x0044];			// Base_(0044h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Mxic1_m_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_Mxic2_m_ib */
struct io_gpv_M_Mxic2_m_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Mxic2_m_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_003F[0x0040 - 0x000C];			// Base_(000Ch - 003Fh) Reserved
	unsigned long						WT;										// Base_(0040h - 0043h) M_Mxic2_m_ib wr_tidemark Register
	unsigned char						dmy_0044_0107[0x0108 - 0x0044];			// Base_(0044h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Mxic2_m_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_Xdmac_r_ib */
struct io_gpv_M_Xdmac_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Xdmac_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_Xdmac_r_ib fn_mod2 Register
	unsigned char						dmy_0028_0107[0x0108 - 0x0028];			// Base_(0028h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Xdmac_r_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_Nf_r_ib */
struct io_gpv_M_Nf_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Nf_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_Nf_r_ib fn_mod2 Register
	unsigned char						dmy_0028_002B[0x002C - 0x0028];			// Base_(0028h - 002Bh) Reserved
	unsigned long						FML;									// Base_(002Ch - 002Fh) M_Nf_r_ib fn_mod_lb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x0030];			// Base_(0030h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Nf_r_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_Pcie0_r_ib */
struct io_gpv_M_Pcie0_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Pcie0_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_Pcie0_r_ib fn_mod2 Register
	unsigned char						dmy_0028_002B[0x002C - 0x0028];			// Base_(0028h - 002Bh) Reserved
	unsigned long						FML;									// Base_(002Ch - 002Fh) M_Pcie0_r_ib fn_mod_lb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x0030];			// Base_(0030h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Pcie0_r_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_Pcie1_r_ib */
struct io_gpv_M_Pcie1_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_Pcie1_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_Pcie1_r_ib fn_mod2 Register
	unsigned char						dmy_0028_002B[0x002C - 0x0028];			// Base_(0028h - 002Bh) Reserved
	unsigned long						FML;									// Base_(002Ch - 002Fh) M_Pcie1_r_ib fn_mod_lb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x0030];			// Base_(0030h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) M_Pcie1_r_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of M_NicPeriAHB_m_ib */
struct io_gpv_M_NicPeriAHB_m_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_NicPeriAHB_m_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_NicPeriAHB_m_ib fn_mod2 Register
	unsigned char						dmy_0028_0043[0x0044 - 0x0028];			// Base_(0028h - 0043h) Reserved
	unsigned long						AC;										// Base_(0044h - 0047h) M_NicPeriAHB_m_ib ahb_cntl Register
	unsigned char						dmy_0048_0FFF[0x1000 - 0x0048];			// Base_(0048h - 0FFFh) Reserved
};

/* Structure of M_NicBmAHB1_r_ib */
struct io_gpv_M_NicBmAHB1_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_NicBmAHB1_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0043[0x0044 - 0x000C];			// Base_(000Ch - 0043h) Reserved
	unsigned long						AC;										// Base_(0044h - 0047h) M_NicBmAHB1_r_ib ahb_cntl Register
	unsigned char						dmy_0048_0FFF[0x1000 - 0x0048];			// Base_(0048h - 0FFFh) Reserved
};

/* Structure of NsecX2P_ib */
struct io_gpv_NsecX2P_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) NsecX2P_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) NsecX2P_ib fn_mod2 Register
	unsigned char						dmy_0028_0FFF[0x1000 - 0x0028];			// Base_(0028h - 0FFFh) Reserved
};

/* Structure of ToprX2P_ib */
struct io_gpv_ToprX2P_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) ToprX2P_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) ToprX2P_ib fn_mod2 Register
	unsigned char						dmy_0028_0FFF[0x1000 - 0x0028];			// Base_(0028h - 0FFFh) Reserved
};

/* Structure of M_NicBmAHB2_r_ib */
struct io_gpv_M_NicBmAHB2_r_ib {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) M_NicBmAHB2_r_ib fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) M_NicBmAHB2_r_ib fn_mod2 Register
	unsigned char						dmy_0028_0043[0x0044 - 0x0028];			// Base_(0028h - 0043h) Reserved
	unsigned long						AC;										// Base_(0044h - 0047h) M_NicBmAHB2_r_ib ahb_cntl Register
	unsigned char						dmy_0048_0FFF[0x1000 - 0x0048];			// Base_(0048h - 0FFFh) Reserved
};

/* Structure of S_Main_rm_ib */
struct io_gpv_S_Main_rm_ib {
	unsigned char						dmy_0000_0007[0x0108 - 0x0000];			// Base_(0000h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_Main_rm_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_Xdmac_m_ib */
struct io_gpv_S_Xdmac_m_ib {
	unsigned char						dmy_0000_0007[0x0108 - 0x0000];			// Base_(0000h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_Xdmac_m_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_Netsec_m */
struct io_gpv_S_Netsec_m {
	unsigned char						dmy_0000_0007[0x0108 - 0x0000];			// Base_(0000h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_Netsec_m fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_WifiAHB_m_ib */
struct io_gpv_S_WifiAHB_m_ib {
	unsigned char						dmy_0000_0007[0x0028 - 0x0000];			// Base_(0000h - 0027h) Reserved
	unsigned long						FMA;									// Base_(0028h - 002Bh) S_WifiAHB_m_ib fn_mod_ahb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x002C];			// Base_(002Ch - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_WifiAHB_m_ib fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_Uhs2AHB_m */
struct io_gpv_S_Uhs2AHB_m {
	unsigned char						dmy_0000_0007[0x0028 - 0x0000];			// Base_(0000h - 0027h) Reserved
	unsigned long						FMA;									// Base_(0028h - 002Bh) S_Uhs2AHB_m fn_mod_ahb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x002C];			// Base_(002Ch - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_Uhs2AHB_m fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_RelcrAHB_m */
struct io_gpv_S_RelcrAHB_m {
	unsigned char						dmy_0000_0007[0x0028 - 0x0000];			// Base_(0000h - 0027h) Reserved
	unsigned long						FMA;									// Base_(0028h - 002Bh) S_RelcrAHB_m fn_mod_ahb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x002C];			// Base_(002Ch - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_RelcrAHB_m fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_Uhs1AHB_m */
struct io_gpv_S_Uhs1AHB_m {
	unsigned char						dmy_0000_0007[0x0028 - 0x0000];			// Base_(0000h - 0027h) Reserved
	unsigned long						FMA;									// Base_(0028h - 002Bh) S_Uhs1AHB_m fn_mod_ahb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x002C];			// Base_(002Ch - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_Uhs1AHB_m fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_RelcwAHB_m */
struct io_gpv_S_RelcwAHB_m {
	unsigned char						dmy_0000_0007[0x0028 - 0x0000];			// Base_(0000h - 0027h) Reserved
	unsigned long						FMA;									// Base_(0028h - 002Bh) S_RelcwAHB_m fn_mod_ahb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x002C];			// Base_(002Ch - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_RelcwAHB_m fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_BmNicAHB_m */
struct io_gpv_S_BmNicAHB_m {
	unsigned char						dmy_0000_0007[0x0028 - 0x0000];			// Base_(0000h - 0027h) Reserved
	unsigned long						FMA;									// Base_(0028h - 002Bh) S_BmNicAHB_m fn_mod_ahb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x002C];			// Base_(002Ch - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_BmNicAHB_m fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_Usb2AHB_ehci */
struct io_gpv_S_Usb2AHB_ehci {
	unsigned char						dmy_0000_0007[0x0028 - 0x0000];			// Base_(0000h - 0027h) Reserved
	unsigned long						FMA;									// Base_(0028h - 002Bh) S_Usb2AHB_ehci fn_mod_ahb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x002C];			// Base_(002Ch - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_Usb2AHB_ehci fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_Usb2AHB_ohci */
struct io_gpv_S_Usb2AHB_ohci {
	unsigned char						dmy_0000_0007[0x0028 - 0x0000];			// Base_(0000h - 0027h) Reserved
	unsigned long						FMA;									// Base_(0028h - 002Bh) S_Usb2AHB_ohci fn_mod_ahb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x002C];			// Base_(002Ch - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_Usb2AHB_ohci fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of S_Netsec2_m */
struct io_gpv_S_Netsec2_m {
	unsigned char						dmy_0000_0107[0x0108 - 0x0000];			// Base_(0000h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) S_Netsec2_m fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of ib8 */
struct io_gpv_ib8 {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) ib8 fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) ib8 fn_mod2 Register
	unsigned char						dmy_0028_003F[0x0040 - 0x0028];			// Base_(0028h - 003Fh) Reserved
	unsigned long						WT;										// Base_(0040h - 0043h) ib8 wr_tidemark Register
	unsigned char						dmy_0044_0107[0x0108 - 0x0044];			// Base_(0044h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) ib8 fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of ib13 */
struct io_gpv_ib13 {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) ib13 fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) ib13 fn_mod2 Register
	unsigned char						dmy_0028_002B[0x002C - 0x0028];			// Base_(0028h - 002Bh) Reserved
	unsigned long						FML;									// Base_(002Ch - 002Fh) ib13 fn_mod_lb Register
	unsigned char						dmy_0030_0107[0x0108 - 0x0030];			// Base_(0030h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) ib13 fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of ib12 */
struct io_gpv_ib12 {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) ib12 fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) ib12 fn_mod2 Register
	unsigned char						dmy_0028_0107[0x0108 - 0x0028];			// Base_(0028h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) ib12 fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

/* Structure of ib9 */
struct io_gpv_ib9 {
	unsigned char						dmy_0000_0007[0x0008 - 0x0000];			// Base_(0000h - 0007h) Reserved
	unsigned long						FMIB;									// Base_(0008h - 000Bh) ib9 fn_mod_iss_bm Register
	unsigned char						dmy_000C_0023[0x0024 - 0x000C];			// Base_(000Ch - 0023h) Reserved
	unsigned long						FM2;									// Base_(0024h - 0027h) ib9 fn_mod2 Register
	unsigned char						dmy_0028_003F[0x0040 - 0x0028];			// Base_(0028h - 003Fh) Reserved
	unsigned long						WT;										// Base_(0040h - 0043h) ib9 wr_tidemark Register
	unsigned char						dmy_0044_0107[0x0108 - 0x0044];			// Base_(0044h - 0107h) Reserved
	unsigned long						FM;										// Base_(0108h - 010Bh) ib9 fn_mod Register
	unsigned char						dmy_010C_0FFF[0x1000 - 0x010C];			// Base_(010Ch - 0FFFh) Reserved
};

struct io_gpv {
	union io_address_region_ctrl_remap	REMAP;									// 1900_(0000h - 0003h) Address region control registers
	unsigned char						dmy_0004_2FFF[0x3000 - 0x0004];			// 1900_(0004h - 2FFFh) Reserved
	struct io_gpv_M_Usb3_r_ib			MUSB3RIB;								// 1900_(3000h - 3FFFh) M_Usb3_r_ib Registers
	struct io_gpv_M_Emmc_r_ib			MEMMCRIB;								// 1900_(4000h - 4FFFh) M_Emmc_r_ib Registers
	struct io_gpv_M_Uhs2_r_ib			MUHS2RIB;								// 1900_(5000h - 5FFFh) M_Uhs2_r_ib Registers
	struct io_gpv_M_Mxic1_m_ib			MMXIC1MIB;								// 1900_(6000h - 6FFFh) M_Mxic1_m_ib Registers
	struct io_gpv_M_Mxic2_m_ib			MMXIC2MIB;								// 1900_(7000h - 7FFFh) M_Mxic2_m_ib Registers
	unsigned char						dmy_8000_AFFF[0xB000 - 0x8000];			// 1900_(8000h - AFFFh) Reserved
	struct io_gpv_M_Xdmac_r_ib			MXDMACRIB;								// 1900_(B000h - BFFFh) M_Xdmac_r_ib Registers
	struct io_gpv_M_Nf_r_ib				MNFRIB;									// 1900_(C000h - CFFFh) M_Nf_r_ib Registers
	struct io_gpv_M_Pcie0_r_ib			MPCIE0RIB;								// 1900_(D000h - DFFFh) M_Pcie0_r_ib Registers
	struct io_gpv_M_Pcie1_r_ib			MPCIE1RIB;								// 1900_(E000h - EFFFh) M_Pcie1_r_ib Registers
	struct io_gpv_M_NicPeriAHB_m_ib		MNICPERIAHBMIB;							// 1900_(F000h - FFFFh) M_NicPeriAHB_m_ib Registers
	struct io_gpv_M_NicBmAHB1_r_ib		MNICBMAHB1RIB;							// 190_(10000h - 10FFFh) M_NicBmAHB1_r_ib Registers
	struct io_gpv_NsecX2P_ib			NSECX2PIB;								// 190_(11000h - 11FFFh) NsecX2P_ib Registers
	struct io_gpv_ToprX2P_ib			TOPRX2PIB;								// 190_(12000h - 12FFFh) ToprX2P_ib Registers
	struct io_gpv_M_NicBmAHB2_r_ib		MNICBMAHB2RIB;							// 190_(13000h - 13FFFh) M_NicBmAHB2_r_ib Registers
	unsigned char						dmy_14000_41FFF[0x42000 - 0x14000];		// 190_(14000h - 41FFFh) Reserved
	struct io_gpv_S_Main_rm_ib			SMAINRMIB;								// 190_(42000h - 42FFFh) S_Main_rm_ib Registers
	struct io_gpv_S_Xdmac_m_ib			SXDMACMIB;								// 190_(43000h - 43FFFh) S_Xdmac_m_ib Registers
	unsigned char						dmy_44000_44FFF[0x45000 - 0x44000];		// 190_(44000h - 44FFFh) Reserved
	struct io_gpv_S_Netsec_m			SNETSECM;								// 190_(45000h - 45FFFh) S_Netsec_m Registers
	unsigned char						dmy_46000_4AFFF[0x4B000 - 0x46000];		// 190_(46000h - 4AFFFh) Reserved
	struct io_gpv_S_WifiAHB_m_ib		SWIFIAHBMIB;							// 190_(4B000h - 4BFFFh) S_WifiAHB_m_ib Registers
	struct io_gpv_S_Uhs2AHB_m			SUHS2AHBM;								// 190_(4C000h - 4CFFFh) S_Uhs2AHB_m Registers
	struct io_gpv_S_RelcrAHB_m			SRELCRAHBM;								// 190_(4D000h - 4DFFFh) S_RelcrAHB_m Registers
	struct io_gpv_S_Uhs1AHB_m			SUHS1AHBM;								// 190_(4E000h - 4EFFFh) S_Uhs1AHB_m Registers
	struct io_gpv_S_RelcwAHB_m			SRELCWAHBM;								// 190_(4F000h - 4FFFFh) S_RelcwAHB_m Registers
	struct io_gpv_S_BmNicAHB_m			SBMNICAHBM;								// 190_(50000h - 50FFFh) S_BmNicAHB_m Registers
	struct io_gpv_S_Usb2AHB_ehci		SUSB2AHBEHCI;							// 190_(51000h - 51FFFh) S_Usb2AHB_ehci Registers
	struct io_gpv_S_Usb2AHB_ohci		SUSB2AHBOHCI;							// 190_(52000h - 52FFFh) S_Usb2AHB_ohci Registers
	struct io_gpv_S_Netsec2_m			SNETSEC2M;								// 190_(53000h - 53FFFh) S_Netsec2_m Registers
	unsigned char						dmy_54000_C2FFF[0xC3000 - 0x54000];		// 190_(54000h - C2FFFh) Reserved
	struct io_gpv_ib8					IB8;									// 190_(C3000h - C3FFFh) IB8 Registers
	unsigned char						dmy_C4000_C4FFF[0xC5000 - 0xC4000];		// 190_(C4000h - C4FFFh) Reserved
	struct io_gpv_ib13					IB13;									// 190_(C5000h - C5FFFh) IB13 Registers
	unsigned char						dmy_C6000_C7FFF[0xC8000 - 0xC6000];		// 190_(C6000h - C7FFFh) Reserved
	struct io_gpv_ib12					IB12;									// 190_(C8000h - C8FFFh) IB12 Registers
	struct io_gpv_ib9					IB9;									// 190_(C9000h - C9FFFh) IB9 Registers
	unsigned char						dmy_CA000_FFFFF[0x100000 - 0xCA000];	// 190_(CA000h - FFFFFh) Reserved
};
