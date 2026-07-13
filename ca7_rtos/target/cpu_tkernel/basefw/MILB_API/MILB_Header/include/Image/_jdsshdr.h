/**
 * @file		_jdsshdr.h
 * @brief		Definition JDSSHDR Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include	"__jdsshdr_tbl.h"

/*	structure of SHDRACTL0		*/
union io_shdr_shdractl0{
	unsigned long		word;
	struct {
		unsigned long	SR				:1;
		unsigned long					:31;
	}bit;
};

/*	structure of SHDRACTL1		*/
union io_shdr_shdractl1{
	unsigned long		word;
	struct {
		unsigned long	TRG				:2;
		unsigned long					:30;
	}bit;
};

/*	structure of SHDRACTL2		*/
union io_shdr_shdractl2{
	unsigned long		word;
	struct {
		unsigned long	SRAMAEN			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of SHDRINT0		*/
union io_shdr_shdrint0{
	unsigned long		word;
	struct {
		unsigned long	SHDREN			:1;
		unsigned long					:3;
		unsigned long	AXREEN			:1;
		unsigned long	AXWEEN			:1;
		unsigned long					:26;
	}bit;
};

/*	structure of SHDRINT1		*/
union io_shdr_shdrint1{
	unsigned long		word;
	struct {
		unsigned long	__SHDRFLG		:1;
		unsigned long					:3;
		unsigned long	__AXREFLG		:1;
		unsigned long	__AXWEFLG		:1;
		unsigned long					:26;
	}bit;
};

/*	structure of AXMD			*/
union io_shdr_axmd{
	unsigned long		word;
	struct {
		unsigned long	BURSTR			:2;
		unsigned long					:2;
		unsigned long	BURSTW			:2;
		unsigned long					:2;
		unsigned long	WSTBMSK			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of AXICTLARA		*/
union io_shdr_axictlara{
	unsigned long		word;
	struct {
		unsigned long	ARCACHEA_0		:4;
		unsigned long	ARPROTA_0		:3;
		unsigned long					:1;
		unsigned long	ARCACHEA_1		:4;
		unsigned long	ARPROTA_1		:3;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of AXICTLARB		*/
union io_shdr_axictlarb{
	unsigned long		word;
	struct {
		unsigned long	ARCACHEB_0		:4;
		unsigned long	ARPROTB_0		:3;
		unsigned long					:1;
		unsigned long	ARCACHEB_1		:4;
		unsigned long	ARPROTB_1		:3;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of AXICTLARC		*/
union io_shdr_axictlarc{
	unsigned long		word;
	struct {
		unsigned long	ARCACHEC_0		:4;
		unsigned long	ARPROTC_0		:3;
		unsigned long					:1;
		unsigned long	ARCACHEC_1		:4;
		unsigned long	ARPROTC_1		:3;
		unsigned long					:1;
		unsigned long	ARCACHEC_2		:4;
		unsigned long	ARPROTC_2		:3;
		unsigned long					:1;
		unsigned long	ARCACHEC_3		:4;
		unsigned long	ARPROTC_3		:3;
		unsigned long					:1;
	}bit;
};

/*	structure of AXICTLARD		*/
union io_shdr_axictlard{
	unsigned long		word;
	struct {
		unsigned long	ARCACHED_0		:4;
		unsigned long	ARPROTD_0		:3;
		unsigned long					:1;
		unsigned long	ARCACHED_1		:4;
		unsigned long	ARPROTD_1		:3;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of AXICTLAW_A		*/
union io_shdr_axictlawa{
	unsigned long		word;
	struct {
		unsigned long	AWCACHE_0		:4;
		unsigned long	AWPROT_0		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_1		:4;
		unsigned long	AWPROT_1		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_2		:4;
		unsigned long	AWPROT_2		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_3		:4;
		unsigned long	AWPROT_3		:3;
		unsigned long					:1;
	}bit;
};

/*	structure of AXICTLAW_B		*/
union io_shdr_axictlawb{
	unsigned long		word[2];
	struct {
		unsigned long	AWCACHE_0		:4;
		unsigned long	AWPROT_0		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_1		:4;
		unsigned long	AWPROT_1		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_2		:4;
		unsigned long	AWPROT_2		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_3		:4;
		unsigned long	AWPROT_3		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_4		:4;
		unsigned long	AWPROT_4		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_5		:4;
		unsigned long	AWPROT_5		:3;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of AXICTLAW_C		*/
union io_shdr_axictlawc{
	unsigned long		word[2];
	struct {
		unsigned long	AWCACHE_0		:4;
		unsigned long	AWPROT_0		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_1		:4;
		unsigned long	AWPROT_1		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_2		:4;
		unsigned long	AWPROT_2		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_3		:4;
		unsigned long	AWPROT_3		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_4		:4;
		unsigned long	AWPROT_4		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_5		:4;
		unsigned long	AWPROT_5		:3;
		unsigned long					:1;
		unsigned long	AWCACHE_6		:4;
		unsigned long	AWPROT_6		:3;
		unsigned long					:1;
		unsigned long					:8;
	}bit;
};

/*	structure of AXICTLRA		*/
union io_shdr_axictlra{
	unsigned long		word;
	struct {
		unsigned long	RRESPA_0		:2;
		unsigned long					:6;
		unsigned long	RRESPA_1		:2;
		unsigned long					:6;
		unsigned long					:16;
	}bit;
};

/*	structure of AXICTLRB		*/
union io_shdr_axictlrb{
	unsigned long		word;
	struct {
		unsigned long	RRESPB_0		:2;
		unsigned long					:6;
		unsigned long	RRESPB_1		:2;
		unsigned long					:6;
		unsigned long					:16;
	}bit;
};

/*	structure of AXICTLRC		*/
union io_shdr_axictlrc{
	unsigned long		word;
	struct {
		unsigned long	RRESPC_0		:2;
		unsigned long					:6;
		unsigned long	RRESPC_1		:2;
		unsigned long					:6;
		unsigned long	RRESPC_2		:2;
		unsigned long					:6;
		unsigned long	RRESPC_3		:2;
		unsigned long					:6;
	}bit;
};

/*	structure of AXICTLRD		*/
union io_shdr_axictlrd{
	unsigned long		word;
	struct {
		unsigned long	RRESPD_0		:2;
		unsigned long					:6;
		unsigned long	RRESPD_1		:2;
		unsigned long					:6;
		unsigned long					:16;
	}bit;
};

/*	structure of AXICTLWA	*/
union io_shdr_axictlwa{
	unsigned long		word;
	struct {
		unsigned long	BRESPA_0		:2;
		unsigned long					:6;
		unsigned long	BRESPA_1		:2;
		unsigned long					:6;
		unsigned long	BRESPA_2		:2;
		unsigned long					:6;
		unsigned long	BRESPA_3		:2;
		unsigned long					:6;
	}bit;
};

/*	structure of AXICTLWB	*/
union io_shdr_axictlwb{
	unsigned long		word[2];
	struct {
		unsigned long	BRESPB_0		:2;
		unsigned long					:6;
		unsigned long	BRESPB_1		:2;
		unsigned long					:6;
		unsigned long	BRESPB_2		:2;
		unsigned long					:6;
		unsigned long	BRESPB_3		:2;
		unsigned long					:6;
		unsigned long	BRESPB_4		:2;
		unsigned long					:6;
		unsigned long	BRESPB_5		:2;
		unsigned long					:6;
		unsigned long					:16;
	}bit;
};

/*	structure of AXICTLWC	*/
union io_shdr_axictlwc{
	unsigned long		word[2];
	struct {
		unsigned long	BRESPC_0		:2;
		unsigned long					:6;
		unsigned long	BRESPC_1		:2;
		unsigned long					:6;
		unsigned long	BRESPC_2		:2;
		unsigned long					:6;
		unsigned long	BRESPC_3		:2;
		unsigned long					:6;
		unsigned long	BRESPC_4		:2;
		unsigned long					:6;
		unsigned long	BRESPC_5		:2;
		unsigned long					:6;
		unsigned long	BRESPC_6		:2;
		unsigned long					:6;
		unsigned long					:8;
	}bit;
};

/*	structure of COREMON0	*/
union io_shdr_coremon0{
	unsigned long		word;
	struct {
		unsigned long	MON_ILINNUM		:11;
		unsigned long					:1;
		unsigned long					:4;
		unsigned long	MON_ITHNUM		:4;
		unsigned long					:4;
		unsigned long	MON_ITVNUM		:5;
		unsigned long					:3;
	}bit;
};

/*	structure of COREMON1		*/
union io_shdr_coremon1{
	unsigned long		word;
	struct {
		unsigned long	MON_OLINNUM		:11;
		unsigned long					:1;
		unsigned long					:4;
		unsigned long	MON_OTHNUM		:4;
		unsigned long					:4;
		unsigned long	MON_OTVNUM		:5;
		unsigned long					:3;
	}bit;
};

/*	structure of DGT_RSEL		*/
union io_shdr_dgt_rsel{
	unsigned long		word;
	struct {
		unsigned long	DGT_RSEL		:3;
		unsigned long					:29;
	}bit;
};

/*	structure of SDRR_ADDR_A	*/
union io_shdr_sdrr_addr_a{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_ADDR_A_0	:32;
		unsigned long	SDRR_ADDR_A_1	:32;
	}bit;
};

/*	structure of SDRR_ADDR_B	*/
union io_shdr_sdrr_addr_b{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_ADDR_B_0	:32;
		unsigned long	SDRR_ADDR_B_1	:32;
	}bit;
};

/*	structure of SDRR_ADDR_C	*/
union io_shdr_sdrr_addr_c{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_ADDR_C_0	:32;
		unsigned long	SDRR_ADDR_C_1	:32;
	}bit;
};

/*	structure of SDRR_ADDR_D	*/
union io_shdr_sdrr_addr_d{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_ADDR_D_0	:32;
		unsigned long	SDRR_ADDR_D_1	:32;
	}bit;
};

/*	structure of SDRR_ADDR_E	*/
union io_shdr_sdrr_addr_e{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_ADDR_E_0	:32;
		unsigned long	SDRR_ADDR_E_1	:32;
	}bit;
};

/*	structure of SDRR_ADDR_F	*/
union io_shdr_sdrr_addr_f{
	unsigned long		word;
	struct {
		unsigned long	SDRR_ADDR_F_0	:32;
	}bit;
};

/*	structure of SDRR_GHSIZE_A	*/
union io_shdr_sdrr_ghsize_a{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_GHSIZE_A_0	:16;
		unsigned long					:16;
		unsigned long	SDRR_GHSIZE_A_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRR_GHSIZE_B	*/
union io_shdr_sdrr_ghsize_b{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_GHSIZE_B_0	:16;
		unsigned long					:16;
		unsigned long	SDRR_GHSIZE_B_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRR_GHSIZE_C	*/
union io_shdr_sdrr_ghsize_c{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_GHSIZE_C_0	:16;
		unsigned long					:16;
		unsigned long	SDRR_GHSIZE_C_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRR_GHSIZE_D	*/
union io_shdr_sdrr_ghsize_d{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_GHSIZE_D_0	:16;
		unsigned long					:16;
		unsigned long	SDRR_GHSIZE_D_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRR_GHSIZE_E	*/
union io_shdr_sdrr_ghsize_e{
	unsigned long		word[2];
	struct {
		unsigned long	SDRR_GHSIZE_E_0	:16;
		unsigned long					:16;
		unsigned long	SDRR_GHSIZE_E_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRR_GHSIZE_F	*/
union io_shdr_sdrr_ghsize_f{
	unsigned long		word;
	struct {
		unsigned long	SDRR_GHSIZE_F_0	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRR_VFM		*/
union io_shdr_sdrr_vfm{
	unsigned long		word;
	struct {
		unsigned long	SDRR_VFM_A		:1;
		unsigned long					:15;
		unsigned long					:1;
		unsigned long					:15;
	}bit;
};

/*	structure of SDRW_ADDR_B	*/
union io_shdr_sdrw_addr_b{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_ADDR_B_0	:32;
		unsigned long	SDRW_ADDR_B_1	:32;
	}bit;
};

/*	structure of SDRW_ADDR_E	*/
union io_shdr_sdrw_addr_e{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_ADDR_E_0	:32;
		unsigned long	SDRW_ADDR_E_1	:32;
	}bit;
};

/*	structure of SDRW_ADDR_F	*/
union io_shdr_sdrw_addr_f{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_ADDR_F_0	:32;
		unsigned long	SDRW_ADDR_F_1	:32;
	}bit;
};

/*	structure of SDRW_ADDR_G	*/
union io_shdr_sdrw_addr_g{
	unsigned long		word[3];
	struct {
		unsigned long	SDRW_ADDR_G_0	:32;
		unsigned long	SDRW_ADDR_G_1	:32;
		unsigned long	SDRW_ADDR_G_2	:32;
	}bit;
};

/*	structure of SDRW_ADDR_H	*/
union io_shdr_sdrw_addr_h{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_ADDR_H_0	:32;
		unsigned long	SDRW_ADDR_H_1	:32;
	}bit;
};

/*	structure of SDRW_ADDR_I	*/
union io_shdr_sdrw_addr_i{
	unsigned long		word;
	struct {
		unsigned long	SDRW_ADDR_I_0	:32;
	}bit;
};

/*	structure of SDRW_GHSIZE_B	*/
union io_shdr_sdrw_ghsize_b{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_GHSIZE_B_0	:16;
		unsigned long					:16;
		unsigned long	SDRW_GHSIZE_B_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRW_GHSIZE_E	*/
union io_shdr_sdrw_ghsize_e{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_GHSIZE_E_0	:16;
		unsigned long					:16;
		unsigned long	SDRW_GHSIZE_E_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRW_GHSIZE_F	*/
union io_shdr_sdrw_ghsize_f{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_GHSIZE_F_0	:16;
		unsigned long					:16;
		unsigned long	SDRW_GHSIZE_F_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRW_GHSIZE_G	*/
union io_shdr_sdrw_ghsize_g{
	unsigned long		word[3];
	struct {
		unsigned long	SDRW_GHSIZE_G_0	:16;
		unsigned long					:16;
		unsigned long	SDRW_GHSIZE_G_1	:16;
		unsigned long					:16;
		unsigned long	SDRW_GHSIZE_G_2	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRW_GHSIZE_H	*/
union io_shdr_sdrw_ghsize_h{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_GHSIZE_H_0	:16;
		unsigned long					:16;
		unsigned long	SDRW_GHSIZE_H_1	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRW_GHSIZE_I	*/
union io_shdr_sdrw_ghsize_i{
	unsigned long		word;
	struct {
		unsigned long	SDRW_GHSIZE_I_0	:15;
		unsigned long	SDRW_VFM_A		:1;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRW_VFM		*/
union io_shdr_sdrw_vfm{
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	SDRW_VFM_B		:1;
		unsigned long					:27;
	}bit;
};

/*	structure of SDRW_BA_A		*/
union io_shdr_sdrw_ba_a{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_BA_A_0		:32;
		unsigned long	SDRW_BA_A_1		:32;
	}bit;
};

/*	structure of SDRW_BA_C		*/
union io_shdr_sdrw_ba_c{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_BA_C_0		:32;
		unsigned long	SDRW_BA_C_1		:32;
	}bit;
};

/*	structure of SDRW_BA_D		*/
union io_shdr_sdrw_ba_d{
	unsigned long		word[2];
	struct {
		unsigned long	SDRW_BA_D_0		:32;
		unsigned long	SDRW_BA_D_1		:32;
	}bit;
};

/*	structure of SDRW_FW_A		*/
union io_shdr_sdrw_fw_a{
	unsigned long		word;
	struct {
		unsigned long	SDRW_FW_A		:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRW_FW_C		*/
union io_shdr_sdrw_fw_c{
	unsigned long		word;
	struct {
		unsigned long	SDRW_FW_C		:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SDRW_FW_D		*/
union io_shdr_sdrw_fw_d{
	unsigned long		word;
	struct {
		unsigned long	SDRW_FW_D		:16;
		unsigned long					:16;
	}bit;
};

/*	structure of SHDRIFRM		*/
union io_shdr_shdrifrm{
	unsigned long		word;
	struct {
		unsigned long	IBITS0			:1;
		unsigned long					:3;
		unsigned long					:4;
		unsigned long	INCFMT			:1;
		unsigned long					:3;
		unsigned long					:4;
		unsigned long	EDGECPY			:1;
		unsigned long					:3;
		unsigned long					:4;
		unsigned long	EDGECUT			:1;
		unsigned long					:7;
	}bit;
};

/*	structure of SHDROFRM		*/
union io_shdr_shdrofrm{
	unsigned long		word;
	struct {
		unsigned long	OUTCTRL			:1;
		unsigned long					:3;
		unsigned long					:4;
		unsigned long	OUTSEL			:1;
		unsigned long					:3;
		unsigned long	OUTSELEN		:1;
		unsigned long					:19;
	}bit;
};

/*	structure of SDRR_HSIZE_A	*/
union io_shdr_sdrr_hsize_a{
	unsigned long		word;
	struct {
		unsigned long	SDRR_HSIZE_A	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_HSIZE_B	*/
union io_shdr_sdrr_hsize_b{
	unsigned long		word;
	struct {
		unsigned long	SDRR_HSIZE_B	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_HSIZE_C	*/
union io_shdr_sdrr_hsize_c{
	unsigned long		word;
	struct {
		unsigned long	SDRR_HSIZE_C	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_HSIZE_D	*/
union io_shdr_sdrr_hsize_d{
	unsigned long		word;
	struct {
		unsigned long	SDRR_HSIZE_D	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_HSIZE_E	*/
union io_shdr_sdrr_hsize_e{
	unsigned long		word;
	struct {
		unsigned long	SDRR_HSIZE_E	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_VSIZE_A	*/
union io_shdr_sdrr_vsize_a{
	unsigned long		word;
	struct {
		unsigned long	SDRR_VSIZE_A	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_VSIZE_B	*/
union io_shdr_sdrr_vsize_b{
	unsigned long		word;
	struct {
		unsigned long	SDRR_VSIZE_B	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_VSIZE_C	*/
union io_shdr_sdrr_vsize_c{
	unsigned long		word;
	struct {
		unsigned long	SDRR_VSIZE_C	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_VSIZE_D	*/
union io_shdr_sdrr_vsize_d{
	unsigned long		word;
	struct {
		unsigned long	SDRR_VSIZE_D	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of SDRR_VSIZE_E	*/
union io_shdr_sdrr_vsize_e{
	unsigned long		word;
	struct {
		unsigned long	SDRR_VSIZE_E	:14;
		unsigned long					:18;
	}bit;
};

/*	structure of EXMAG		*/
union io_shdr_exmag{
	unsigned long		word;
	struct {
		unsigned long	EXMAG			:14;
		unsigned long					:18;
	}bit;
};

/*	structure of REMVAL		*/
union io_shdr_remval{
	unsigned long		word;
	struct {
		unsigned long	REXMAG			:17;
		unsigned long					:3;
		unsigned long					:4;
		long			REMSFT			:4;
		unsigned long					:4;
	}bit;
};

/*	structure of SHIFT		*/
union io_shdr_shift{
	unsigned long		word;
	struct {
		long			SHIFTX			:13;
		unsigned long					:3;
		long			SHIFTY			:13;
		unsigned long					:3;
	}bit;
};

/*	structure of MRGN		*/
union io_shdr_mrgn{
	unsigned long		word;
	struct {
		unsigned long	MRGN			:7;
		unsigned long					:25;
	}bit;
};

/*	structure of FLWCNT		*/
union io_shdr_flwcnt{
	unsigned long		word;
	struct {
		unsigned long	FMDEN			:1;
		unsigned long					:3;
		unsigned long					:4;
		unsigned long	BPEN			:1;
		unsigned long					:3;
		unsigned long					:4;
		unsigned long	SMCIN			:2;
		unsigned long					:2;
		unsigned long					:4;
		unsigned long	BRSEL			:2;
		unsigned long					:6;
	}bit;
};

/*	structure of TMODE		*/
union io_shdr_tmode{
	unsigned long		word;
	struct {
		unsigned long	TMODE			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of MODESEL	*/
union io_shdr_modesel{
	unsigned long		word;
	struct {
		unsigned long	MODE			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of MC_MODESEL	*/
union io_shdr_mcmodesel{
	unsigned long		word;
	struct {
		unsigned long	MODE			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of COMP_INCFMSELSEL	*/
union io_shdr_compincfmsel{
	unsigned long		word;
	struct {
		unsigned long	MODE			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of BYPASSON	*/
union io_shdr_bypasson{
	unsigned long		word;
	struct {
		unsigned long	RMD				:1;
		unsigned long					:3;
		unsigned long	WMD				:1;
		unsigned long					:27;
	}bit;
};

/*	structure of AXISIZE	*/
union io_shdr_axisize{
	unsigned long		word;
	struct {
		unsigned long	RYC				:1;
		unsigned long					:3;
		unsigned long	RCI				:1;
		unsigned long					:3;
		unsigned long	WR				:1;
		unsigned long					:23;
	}bit;
};

/*	structure of LINT	*/
union io_shdr_lint{
	unsigned long		word[3];
	struct {
		unsigned long	NO				:5;
		unsigned long					:27;
		unsigned long	MSK				:1;
		unsigned long					:31;
		unsigned long	ST				:1;
		unsigned long					:31;
	}bit;
};

/*	structure of WRSTARTADDR	*/
union io_shdr_wrstartaddr{
	unsigned long		word[2];
	struct {
		unsigned long	ADDR_Y			:32;
		unsigned long	ADDR_C			:32;
	}bit;
};

/*	structure of RDSTARTADDR	*/
union io_shdr_rdstartaddr{
	unsigned long		word[2];
	struct {
		unsigned long	ADDR_Y			:32;
		unsigned long	ADDR_C			:32;
	}bit;
};

/*	structure of PREMD		*/
union io_shdr_premd{
	unsigned long		word;
	struct {
		unsigned long	CUSMD			:3;
		unsigned long					:1;
		unsigned long					:4;
		unsigned long	DGMD			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of Y2RCOEF	*/
union io_shdr_y2rcoef{
	unsigned long		word[5];
	struct {
		long			Y2RCOEF_0		:15;
		unsigned long					:1;
		long			Y2RCOEF_1		:15;
		unsigned long					:1;
		long			Y2RCOEF_2		:15;
		unsigned long					:1;
		long			Y2RCOEF_3		:15;
		unsigned long					:1;
		long			Y2RCOEF_4		:15;
		unsigned long					:1;
		long			Y2RCOEF_5		:15;
		unsigned long					:1;
		long			Y2RCOEF_6		:15;
		unsigned long					:1;
		long			Y2RCOEF_7		:15;
		unsigned long					:1;
		long			Y2RCOEF_8		:15;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of PREY2RCLPUL0	*/
union io_shdr_prey2rclpul0{
	unsigned long		word[2];
	struct {
		unsigned long	PREY2RCLPUL0_0	:16;
		unsigned long	PREY2RCLPUL0_1	:16;
		unsigned long	PREY2RCLPUL0_2	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of PREY2RCLPLL0	*/
union io_shdr_prey2rclpll0{
	unsigned long		word[2];
	struct {
		unsigned long	PREY2RCLPLL0_0	:16;
		unsigned long	PREY2RCLPLL0_1	:16;
		unsigned long	PREY2RCLPLL0_2	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of PREY2RCLPUS	*/
union io_shdr_prey2rclpus{
	unsigned long		word[2];
	struct {
		unsigned long	PREY2RCLPUS_0	:8;
		unsigned long					:8;
		unsigned long	PREY2RCLPUS_1	:8;
		unsigned long					:8;
		unsigned long	PREY2RCLPUS_2	:8;
		unsigned long					:8;
		unsigned long					:16;
	}bit;
};

/*	structure of PREY2RCLPLS	*/
union io_shdr_prey2rclpls{
	unsigned long		word[2];
	struct {
		unsigned long	PREY2RCLPLS_0	:8;
		unsigned long					:8;
		unsigned long	PREY2RCLPLS_1	:8;
		unsigned long					:8;
		unsigned long	PREY2RCLPLS_2	:8;
		unsigned long					:8;
		unsigned long					:16;
	}bit;
};

/*	structure of R2YCOEF	*/
union io_shdr_r2ycoef{
	unsigned long		word[5];
	struct {
		long			R2YCOEF_0		:15;
		unsigned long					:1;
		long			R2YCOEF_1		:15;
		unsigned long					:1;
		long			R2YCOEF_2		:15;
		unsigned long					:1;
		long			R2YCOEF_3		:15;
		unsigned long					:1;
		long			R2YCOEF_4		:15;
		unsigned long					:1;
		long			R2YCOEF_5		:15;
		unsigned long					:1;
		long			R2YCOEF_6		:15;
		unsigned long					:1;
		long			R2YCOEF_7		:15;
		unsigned long					:1;
		long			R2YCOEF_8		:15;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of PRER2YCLPUL	*/
union io_shdr_prer2yclpul{
	unsigned long		word[3];
	struct {
		unsigned long	PRER2YCLPUL_0	:16;
		unsigned long					:16;
		long			PRER2YCLPUL_1	:16;
		unsigned long					:16;
		long			PRER2YCLPUL_2	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of PRER2YCLPLL	*/
union io_shdr_prer2yclpll{
	unsigned long		word[3];
	struct {
		unsigned long	PRER2YCLPLL_0	:16;
		unsigned long					:16;
		long			PRER2YCLPLL_1	:16;
		unsigned long					:16;
		long			PRER2YCLPLL_2	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of PRER2YCLPUS	*/
union io_shdr_prer2yclpus{
	unsigned long		word[2];
	struct {
		unsigned long	PRER2YCLPUS_0	:12;
		unsigned long					:4;
		long			PRER2YCLPUS_1	:12;
		unsigned long					:4;
		long			PRER2YCLPUS_2	:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of PRER2YCLPLS	*/
union io_shdr_prer2yclpls{
	unsigned long		word[2];
	struct {
		unsigned long	PRER2YCLPLS_0	:12;
		unsigned long					:4;
		long			PRER2YCLPLS_1	:12;
		unsigned long					:4;
		long			PRER2YCLPLS_2	:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of SKNOFS		*/
union io_shdr_sknofs{
	unsigned long		word[3];
	struct {
		unsigned long	SKNOFS_0		:8;
		unsigned long					:24;
		unsigned long	SKNOFS_1		:8;
		unsigned long					:24;
		unsigned long	SKNOFS_2		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of SKNSL		*/
union io_shdr_sknsl{
	unsigned long		word[3];
	struct {
		unsigned long	SKNSL_0			:8;
		unsigned long					:24;
		unsigned long	SKNSL_1			:8;
		unsigned long					:24;
		unsigned long	SKNSL_2			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of SKNB		*/
union io_shdr_sknb{
	unsigned long		word;
	struct {
		unsigned long	SKNB			:10;
		unsigned long					:22;
	}bit;
};

/*	structure of LKNOFS		*/
union io_shdr_lknofs{
	unsigned long		word[3];
	struct {
		unsigned long	LKNOFS_0		:8;
		unsigned long					:24;
		unsigned long	LKNOFS_1		:8;
		unsigned long					:24;
		unsigned long	LKNOFS_2		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of LKNSL		*/
union io_shdr_lknsl{
	unsigned long		word[3];
	struct {
		unsigned long	LKNSL_0			:8;
		unsigned long					:24;
		unsigned long	LKNSL_1			:8;
		unsigned long					:24;
		unsigned long	LKNSL_2			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of LKNB		*/
union io_shdr_lknb{
	unsigned long		word;
	struct {
		unsigned long	LKNB			:10;
		unsigned long					:22;
	}bit;
};

/*	structure of EPBD		*/
union io_shdr_epbd{
	unsigned long		word[2];
	struct {
		unsigned long					:16;
		unsigned long	EPBD_1			:8;
		unsigned long					:8;
		unsigned long	EPBD_2			:8;
		unsigned long					:8;
		unsigned long	EPBD_3			:8;
		unsigned long					:8;
	}bit;
};

/*	structure of EPOF	*/
union io_shdr_epof{
	unsigned long		word[2];
	struct {
		unsigned long	EPOF_0			:8;
		unsigned long					:8;
		unsigned long	EPOF_1			:8;
		unsigned long					:8;
		unsigned long	EPOF_2			:8;
		unsigned long					:8;
		unsigned long	EPOF_3			:8;
		unsigned long					:8;
	}bit;
};

/*	structure of EPBLD		*/
union io_shdr_epbld{
	unsigned long		word;
	struct {
		unsigned long	EPBLD			:3;
		unsigned long					:29;
	}bit;
};

/*	structure of FDWEIGHT	*/
union io_shdr_fdweight{
	unsigned long		word;
	struct {
		unsigned long	FDWEIGHT		:5;
		unsigned long					:27;
	}bit;
};

/*	structure of MVCNT		*/
union io_shdr_mvcnt{
	unsigned long		word;
	struct {
		unsigned long	MVBD			:8;
		unsigned long	MVTH			:8;
		long			MVGR			:13;
		unsigned long					:3;
	}bit;
};

/*	structure of BLDROFS	*/
union io_shdr_bldrofs{
	unsigned long		word[3];
	struct {
		unsigned long	BLDROFS_0		:8;
		unsigned long					:8;
		unsigned long	BLDROFS_1		:8;
		unsigned long					:8;
		unsigned long	BLDROFS_2		:8;
		unsigned long					:8;
		unsigned long	BLDROFS_3		:8;
		unsigned long					:8;
		unsigned long	BLDROFS_4		:8;
		unsigned long					:8;
		unsigned long					:16;
	}bit;
};

/*	structure of BLDRSL	*/
union io_shdr_bldrsl{
	unsigned long		word[3];
	struct {
		unsigned long	BLDRSL_0		:12;
		unsigned long					:4;
		unsigned long	BLDRSL_1		:12;
		unsigned long					:4;
		unsigned long	BLDRSL_2		:12;
		unsigned long					:4;
		unsigned long	BLDRSL_3		:12;
		unsigned long					:4;
		unsigned long	BLDRSL_4		:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of BLDRB	*/
union io_shdr_bldrb{
	unsigned long		word[3];
	struct {
		unsigned long					:16;
		unsigned long	BLDRB_1			:8;
		unsigned long					:8;
		unsigned long	BLDRB_2			:8;
		unsigned long					:8;
		unsigned long	BLDRB_3			:8;
		unsigned long					:8;
		unsigned long	BLDRB_4			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of WGHTR2YCOE	*/
union io_shdr_wghtr2ycoef{
	unsigned long		word[2];
	struct {
		long			WGHTR2YCOEF_0	:15;
		unsigned long					:1;
		long			WGHTR2YCOEF_1	:15;
		unsigned long					:1;
		long			WGHTR2YCOEF_2	:15;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of WGHTR2YCLP	*/
union io_shdr_wghtr2yclp{
	unsigned long		word;
	struct {
		unsigned long	WGHTR2YCLPU		:8;
		unsigned long					:8;
		unsigned long	WGHTR2YCLPL		:8;
		unsigned long					:8;
	}bit;
};

/*	structure of WGHTBASE	*/
union io_shdr_wghtbase{
	unsigned long		word;
	struct {
		unsigned long	WGHTBASE		:1;
		unsigned long					:31;
	}bit;
};

/*	structure of WGHTROFS	*/
union io_shdr_wghtrofs{
	unsigned long		word[3];
	struct {
		unsigned long	WGHTROFS_0		:12;
		unsigned long					:4;
		unsigned long	WGHTROFS_1		:12;
		unsigned long					:4;
		unsigned long	WGHTROFS_2		:12;
		unsigned long					:4;
		unsigned long	WGHTROFS_3		:12;
		unsigned long					:4;
		unsigned long	WGHTROFS_4		:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of WGHTGOFS	*/
union io_shdr_wghtgofs{
	unsigned long		word[3];
	struct {
		unsigned long	WGHTGOFS_0		:12;
		unsigned long					:4;
		unsigned long	WGHTGOFS_1		:12;
		unsigned long					:4;
		unsigned long	WGHTGOFS_2		:12;
		unsigned long					:4;
		unsigned long	WGHTGOFS_3		:12;
		unsigned long					:4;
		unsigned long	WGHTGOFS_4		:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of WGHTBOFS	*/
union io_shdr_wghtbofs{
	unsigned long		word[3];
	struct {
		unsigned long	WGHTBOFS_0		:12;
		unsigned long					:4;
		unsigned long	WGHTBOFS_1		:12;
		unsigned long					:4;
		unsigned long	WGHTBOFS_2		:12;
		unsigned long					:4;
		unsigned long	WGHTBOFS_3		:12;
		unsigned long					:4;
		unsigned long	WGHTBOFS_4		:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of WGHTRSL	*/
union io_shdr_wghtrsl{
	unsigned long		word[3];
	struct {
		unsigned long	WGHTRSL_0		:12;
		unsigned long					:4;
		unsigned long	WGHTRSL_1		:12;
		unsigned long					:4;
		unsigned long	WGHTRSL_2		:12;
		unsigned long					:4;
		unsigned long	WGHTRSL_3		:12;
		unsigned long					:4;
		unsigned long	WGHTRSL_4		:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of WGHTGSL	*/
union io_shdr_wghtgsl{
	unsigned long		word[3];
	struct {
		unsigned long	WGHTGSL_0		:12;
		unsigned long					:4;
		unsigned long	WGHTGSL_1		:12;
		unsigned long					:4;
		unsigned long	WGHTGSL_2		:12;
		unsigned long					:4;
		unsigned long	WGHTGSL_3		:12;
		unsigned long					:4;
		unsigned long	WGHTGSL_4		:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of WGHTBSL	*/
union io_shdr_wghtbsl{
	unsigned long		word[3];
	struct {
		unsigned long	WGHTBSL_0		:12;
		unsigned long					:4;
		unsigned long	WGHTBSL_1		:12;
		unsigned long					:4;
		unsigned long	WGHTBSL_2		:12;
		unsigned long					:4;
		unsigned long	WGHTBSL_3		:12;
		unsigned long					:4;
		unsigned long	WGHTBSL_4		:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of WGHTRB		*/
union io_shdr_wghtrb{
	unsigned long		word[3];
	struct {
		unsigned long					:16;
		unsigned long	WGHTRB_1		:8;
		unsigned long					:8;
		unsigned long	WGHTRB_2		:8;
		unsigned long					:8;
		unsigned long	WGHTRB_3		:8;
		unsigned long					:8;
		unsigned long	WGHTRB_4		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of WGHTGB		*/
union io_shdr_wghtgb{
	unsigned long		word[3];
	struct {
		unsigned long					:16;
		unsigned long	WGHTGB_1		:8;
		unsigned long					:8;
		unsigned long	WGHTGB_2		:8;
		unsigned long					:8;
		unsigned long	WGHTGB_3		:8;
		unsigned long					:8;
		unsigned long	WGHTGB_4		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of WGHTBB	*/
union io_shdr_wghtbb{
	unsigned long		word[3];
	struct {
		unsigned long					:16;
		unsigned long	WGHTBB_1		:8;
		unsigned long					:8;
		unsigned long	WGHTBB_2		:8;
		unsigned long					:8;
		unsigned long	WGHTBB_3		:8;
		unsigned long					:8;
		unsigned long	WGHTBB_4		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of ISOTH		*/
union io_shdr_isoth{
	unsigned long		word;
	struct {
		unsigned long	ISOTH2			:9;
		unsigned long					:23;
	}bit;
};

/*	structure of ISOCOEF	*/
union io_shdr_isocoef{
	unsigned long		word;
	struct {
		unsigned long	ISO2K0			:2;
		unsigned long	ISO2K1			:2;
		unsigned long	ISO2K2			:2;
		unsigned long					:2;
		unsigned long	ISO2K3			:5;
		unsigned long					:3;
		unsigned long	ISO2K4			:5;
		unsigned long					:3;
		unsigned long	ISO2K5			:8;
	}bit;
};

/*	structure of SATTH		*/
union io_shdr_satth{
	unsigned long		word;
	struct {
		unsigned long	SATTH			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of EXP77MD	*/
union io_shdr_exp77md{
	unsigned long		word;
	struct {
		unsigned long	EXP77MD			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of FMDSEL		*/
union io_shdr_fmdsel{
	unsigned long		word;
	struct {
		unsigned long	FMDSEL			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of TLUT		*/
union io_shdr_tlut{
	unsigned long		word[5];
	struct {
		unsigned long	TLUT_0			:8;
		unsigned long	TLUT_1			:8;
		unsigned long	TLUT_2			:8;
		unsigned long	TLUT_3			:8;
		unsigned long	TLUT_4			:8;
		unsigned long	TLUT_5			:8;
		unsigned long	TLUT_6			:8;
		unsigned long	TLUT_7			:8;
		unsigned long	TLUT_8			:8;
		unsigned long	TLUT_9			:8;
		unsigned long	TLUT_10			:8;
		unsigned long	TLUT_11			:8;
		unsigned long	TLUT_12			:8;
		unsigned long	TLUT_13			:8;
		unsigned long	TLUT_14			:8;
		unsigned long	TLUT_15			:8;
		unsigned long	TLUT_16			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of PMSKMD		*/
union io_shdr_pmskmd{
	unsigned long		word;
	struct {
		unsigned long	PMSKMD			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of SPNREN		*/
union io_shdr_spnren{
	unsigned long		word;
	struct {
		unsigned long	SPNREN			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of SPEBD		*/
union io_shdr_spebd{
	unsigned long		word[2];
	struct {
		unsigned long					:16;
		unsigned long	SPEBD_1			:12;
		unsigned long					:4;
		unsigned long	SPEBD_2			:12;
		unsigned long					:4;
		unsigned long	SPEBD_3			:12;
		unsigned long					:4;
	}bit;
};

/*	structure of SPEOF		*/
union io_shdr_speof{
	unsigned long		word[2];
	struct {
		unsigned long	SPEOF_0			:12;
		unsigned long					:4;
		unsigned long	SPEOF_1			:12;
		unsigned long					:4;
		unsigned long	SPEOF_2			:12;
		unsigned long					:4;
		unsigned long	SPEOF_3			:12;
		unsigned long					:4;
	}bit;
};

/*	structure of SPEBLD		*/
union io_shdr_spebld{
	unsigned long		word;
	struct {
		unsigned long	SPEBLD			:3;
		unsigned long					:29;
	}bit;
};

/*	structure of MSKY2RCLPUL	*/
union io_shdr_msky2rclpul{
	unsigned long		word[2];
	struct {
		unsigned long	MSKY2RCLPUL_0	:12;
		unsigned long					:4;
		unsigned long	MSKY2RCLPUL_1	:12;
		unsigned long					:4;
		unsigned long	MSKY2RCLPUL_2	:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of MSKY2RCLPLL	*/
union io_shdr_msky2rclpll{
	unsigned long		word[2];
	struct {
		unsigned long	MSKY2RCLPLL_0	:12;
		unsigned long					:4;
		unsigned long	MSKY2RCLPLL_1	:12;
		unsigned long					:4;
		unsigned long	MSKY2RCLPLL_2	:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of MSKY2RCLPUS	*/
union io_shdr_msky2rclpus{
	unsigned long		word[2];
	struct {
		unsigned long	MSKY2RCLPUS_0	:12;
		unsigned long					:4;
		unsigned long	MSKY2RCLPUS_1	:12;
		unsigned long					:4;
		unsigned long	MSKY2RCLPUS_2	:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of MSKY2RCLPLS	*/
union io_shdr_msky2rclpls{
	unsigned long		word[2];
	struct {
		unsigned long	MSKY2RCLPLS_0	:12;
		unsigned long					:4;
		unsigned long	MSKY2RCLPLS_1	:12;
		unsigned long					:4;
		unsigned long	MSKY2RCLPLS_2	:12;
		unsigned long					:4;
		unsigned long					:16;
	}bit;
};

/*	structure of ALPHA		*/
union io_shdr_alpha{
	unsigned long		word;
	struct {
		unsigned long	ALPHA			:6;
		unsigned long					:26;
	}bit;
};

/*	structure of LPFC		*/
union io_shdr_lpfc{
	unsigned long		word[5];
	struct {
		unsigned long	LPFC_0_0		:8;
		unsigned long					:8;
		unsigned long	LPFC_0_1		:8;
		unsigned long					:8;
		unsigned long	LPFC_0_2		:8;
		unsigned long					:8;
		unsigned long	LPFC_1_0		:8;
		unsigned long					:8;
		unsigned long	LPFC_1_1		:8;
		unsigned long					:8;
		unsigned long	LPFC_1_2		:8;
		unsigned long					:8;
		unsigned long	LPFC_2_0		:8;
		unsigned long					:8;
		unsigned long	LPFC_2_1		:8;
		unsigned long					:8;
		unsigned long	LPFC_2_2		:8;
		unsigned long					:8;
		unsigned long					:16;
	}bit;
};

/*	structure of BLDOFS		*/
union io_shdr_bldofs{
	unsigned long		word[4];
	struct {
		unsigned long	BLDOFS_0		:8;
		unsigned long					:8;
		unsigned long	BLDOFS_1		:8;
		unsigned long					:8;
		unsigned long	BLDOFS_2		:8;
		unsigned long					:8;
		unsigned long	BLDOFS_3		:8;
		unsigned long					:8;
		unsigned long	BLDOFS_4		:8;
		unsigned long					:8;
		unsigned long	BLDOFS_5		:8;
		unsigned long					:8;
		unsigned long	BLDOFS_6		:8;
		unsigned long					:8;
		unsigned long					:16;
	}bit;
};

/*	structure of BLDSL	*/
union io_shdr_bldsl{
	unsigned long		word[7];
	struct {
		unsigned long	BLDSL_0			:16;
		unsigned long					:16;
		unsigned long	BLDSL_1			:16;
		unsigned long					:16;
		unsigned long	BLDSL_2			:16;
		unsigned long					:16;
		unsigned long	BLDSL_3			:16;
		unsigned long					:16;
		unsigned long	BLDSL_4			:16;
		unsigned long					:16;
		unsigned long	BLDSL_5			:16;
		unsigned long					:16;
		unsigned long	BLDSL_6			:16;
		unsigned long					:16;
	}bit;
};

/*	structure of BLDB	*/
union io_shdr_bldb{
	unsigned long		word[4];
	struct {
		unsigned long					:12;
		unsigned long					:4;
		unsigned long	BLDB_1			:12;
		unsigned long					:4;
		unsigned long	BLDB_2			:12;
		unsigned long					:4;
		unsigned long	BLDB_3			:12;
		unsigned long					:4;
		unsigned long	BLDB_4			:12;
		unsigned long					:4;
		unsigned long	BLDB_5			:12;
		unsigned long					:4;
		unsigned long	BLDB_6			:12;
		unsigned long					:20;
	}bit;
};

/*	structure of NORMGAIN	*/
union io_shdr_normgain{
	unsigned long		word[2];
	struct {
		unsigned long	NORMGAIN_0		:8;
		unsigned long					:8;
		unsigned long	NORMGAIN_1		:8;
		unsigned long					:8;
		unsigned long	NORMGAIN_2		:8;
		unsigned long					:8;
		unsigned long					:16;
	}bit;
};

/*	structure of NORMOFS	*/
union io_shdr_normofs{
	unsigned long		word[2];
	struct {
		unsigned long	NORMOFS_0		:8;
		unsigned long					:8;
		unsigned long	NORMOFS_1		:8;
		unsigned long					:8;
		unsigned long	NORMOFS_2		:8;
		unsigned long					:8;
		unsigned long					:16;
	}bit;
};

/*	structure of MSKMD		*/
union io_shdr_mskmd{
	unsigned long		word;
	struct {
		unsigned long	MSKMD			:2;
		unsigned long					:30;
	}bit;
};

/*	structure of BLDSEL		*/
union io_shdr_bldsel{
	unsigned long		word;
	struct {
		unsigned long	BLDSEL			:1;
		unsigned long					:31;
	}bit;
};

/*	structure of ALPHASEL	*/
union io_shdr_alphasel{
	unsigned long		word;
	struct {
		unsigned long	ALPHASEL		:2;
		unsigned long					:30;
	}bit;
};

/*	structure of ALPHAVAL	*/
union io_shdr_alphaval{
	unsigned long		word[2];
	struct {
		unsigned long	ALPHAVALY		:8;
		unsigned long					:8;
		unsigned long	ALPHAVALU		:8;
		unsigned long					:8;
		unsigned long	ALPHAVALV		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of POSTSFT1RND	*/
union io_shdr_postsft1rnd{
	unsigned long		word;
	struct {
		unsigned long	POSTSFT1RNDEN	:1;
		unsigned long					:3;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long					:2;
		unsigned long					:22;
	}bit;
};

/*	structure of POSTY2RCOEF	*/
union io_shdr_posty2rcoef{
	unsigned long		word[5];
	struct {
		long			POSTY2RCOEF_0	:15;
		unsigned long					:1;
		long			POSTY2RCOEF_1	:15;
		unsigned long					:1;
		long			POSTY2RCOEF_2	:15;
		unsigned long					:1;
		long			POSTY2RCOEF_3	:15;
		unsigned long					:1;
		long			POSTY2RCOEF_4	:15;
		unsigned long					:1;
		long			POSTY2RCOEF_5	:15;
		unsigned long					:1;
		long			POSTY2RCOEF_6	:15;
		unsigned long					:1;
		long			POSTY2RCOEF_7	:15;
		unsigned long					:1;
		long			POSTY2RCOEF_8	:15;
		unsigned long					:1;
		unsigned long					:16;
	}bit;
};

/*	structure of POSTY2RCLPTHU	*/
union io_shdr_posty2rclpthu{
	unsigned long		word[2];
	struct {
		unsigned long	POSTY2RCLPTHU_0	:16;
		unsigned long	POSTY2RCLPTHU_1	:16;
		unsigned long	POSTY2RCLPTHU_2	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of POSTY2RCLPTHL	*/
union io_shdr_posty2rclpthl{
	unsigned long		word[2];
	struct {
		unsigned long	POSTY2RCLPTHL_0	:16;
		unsigned long	POSTY2RCLPTHL_1	:16;
		unsigned long	POSTY2RCLPTHL_2	:16;
		unsigned long					:16;
	}bit;
};

/*	structure of POSTSEL1	*/
union io_shdr_postsel1{
	unsigned long		word;
	struct {
		unsigned long	POSTSEL1		:1;
		unsigned long					:31;
	}bit;
};

/*	structure of POSTMULQPOS	*/
union io_shdr_postmulqpos{
	unsigned long		word;
	struct {
		unsigned long	POSTMULQPOS		:2;
		unsigned long					:30;
	}bit;
};

/*	structure of EVKNB		*/
union io_shdr_evknb{
	unsigned long		word[5];
	struct {
		unsigned long					:12;
		unsigned long					:4;
		unsigned long	EVKNB_1			:12;
		unsigned long					:4;
		unsigned long	EVKNB_2			:12;
		unsigned long					:4;
		unsigned long	EVKNB_3			:12;
		unsigned long					:4;
		unsigned long	EVKNB_4			:12;
		unsigned long					:4;
		unsigned long	EVKNB_5			:12;
		unsigned long					:4;
		unsigned long	EVKNB_6			:12;
		unsigned long					:4;
		unsigned long	EVKNB_7			:12;
		unsigned long					:4;
		unsigned long	EVKNB_8			:12;
		unsigned long					:4;
		unsigned long	EVKNB_9			:12;
		unsigned long					:4;
	}bit;
};

/*	structure of EVKNOFS	*/
union io_shdr_evknofs{
	unsigned long		word[5];
	struct {
		unsigned long	EVKNOFS_0		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_1		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_2		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_3		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_4		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_5		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_6		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_7		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_8		:14;
		unsigned long					:2;
		unsigned long	EVKNOFS_9		:14;
		unsigned long					:2;
	}bit;
};

/*	structure of EVKNSL		*/
union io_shdr_evknsl{
	unsigned long		word[5];
	struct {
		long			EVKNSL_0		:15;
		unsigned long					:1;
		long			EVKNSL_1		:15;
		unsigned long					:1;
		long			EVKNSL_2		:15;
		unsigned long					:1;
		long			EVKNSL_3		:15;
		unsigned long					:1;
		long			EVKNSL_4		:15;
		unsigned long					:1;
		long			EVKNSL_5		:15;
		unsigned long					:1;
		long			EVKNSL_6		:15;
		unsigned long					:1;
		long			EVKNSL_7		:15;
		unsigned long					:1;
		long			EVKNSL_8		:15;
		unsigned long					:1;
		long			EVKNSL_9		:15;
		unsigned long					:1;
	}bit;
};

/*	structure of POSTSEL0	*/
union io_shdr_postsel0{
	unsigned long		word;
	struct {
		unsigned long	POSTSEL0		:1;
		unsigned long					:31;
	}bit;
};

/*	structure of HDRCSSMD	*/
union io_shdr_hdrcssmd{
	unsigned long		word;
	struct {
		unsigned long	HDRCSSMD		:1;
		unsigned long					:31;
	}bit;
};

/*	structure of CH0MODE	*/
union io_shdr_ch0mode{
	unsigned long		word;
	struct {
		unsigned long	WEN0			:1;
		unsigned long					:3;
		unsigned long					:2;
		unsigned long					:2;
		unsigned long					:1;
		unsigned long					:1;
		unsigned long					:2;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long	DRCEN0			:1;
		unsigned long					:3;
		unsigned long	CSSMD0			:2;
		unsigned long					:10;
	}bit;
};

/*	structure of CH1MODE	*/
union io_shdr_ch1mode{
	unsigned long		word;
	struct {
		unsigned long	WEN1			:1;
		unsigned long					:3;
		unsigned long					:2;
		unsigned long					:2;
		unsigned long					:1;
		unsigned long					:1;
		unsigned long	RDCMD1			:2;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long	CSSMD1			:2;
		unsigned long					:10;
	}bit;
};

/*	structure of CH2MODE	*/
union io_shdr_ch2mode{
	unsigned long		word;
	struct {
		unsigned long	WEN2			:1;
		unsigned long					:3;
		unsigned long	RSZMD2			:2;
		unsigned long					:2;
		unsigned long	RDCEN2			:1;
		unsigned long					:1;
		unsigned long	RDCMD2			:2;
		unsigned long	TRMEN2			:1;
		unsigned long					:3;
		unsigned long	DRCEN2			:1;
		unsigned long					:3;
		unsigned long	CSSMD2			:2;
		unsigned long					:10;
	}bit;
};

/*	structure of CH3MODE	*/
union io_shdr_ch3mode{
	unsigned long		word;
	struct {
		unsigned long	WEN3			:1;
		unsigned long					:3;
		unsigned long	RSZMD3			:2;
		unsigned long					:2;
		unsigned long	RDCEN3			:1;
		unsigned long					:1;
		unsigned long	RDCMD3			:2;
		unsigned long	TRMEN3			:1;
		unsigned long					:3;
		unsigned long	DRCEN3			:1;
		unsigned long					:3;
		unsigned long	CSSMD3			:2;
		unsigned long					:10;
	}bit;
};

/*	structure of CH4MODE	*/
union io_shdr_ch4mode{
	unsigned long		word;
	struct {
		unsigned long	WEN4			:1;
		unsigned long					:3;
		unsigned long	RSZMD4			:2;
		unsigned long					:2;
		unsigned long	RDCEN4			:1;
		unsigned long					:1;
		unsigned long	RDCMD4			:2;
		unsigned long	TRMEN4			:1;
		unsigned long					:3;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long	CSSMD4			:2;
		unsigned long					:10;
	}bit;
};

/*	structure of CH5MODE	*/
union io_shdr_ch5mode{
	unsigned long		word;
	struct {
		unsigned long	WEN5			:1;
		unsigned long					:3;
		unsigned long	RSZMD5			:2;
		unsigned long					:2;
		unsigned long	RDCEN5			:1;
		unsigned long					:1;
		unsigned long	RDCMD5			:2;
		unsigned long	TRMEN5			:1;
		unsigned long					:3;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long	CSSMD5			:2;
		unsigned long					:10;
	}bit;
};

/*	structure of WOUTSZ		*/
union io_shdr_woutsz{
	unsigned long		word[6];
	struct {
		unsigned long	WOUTHSZ_0		:14;
		unsigned long					:2;
		unsigned long	WOUTVSZ_0		:14;
		unsigned long					:2;
		unsigned long	WOUTHSZ_1		:14;
		unsigned long					:2;
		unsigned long	WOUTVSZ_1		:14;
		unsigned long					:2;
		unsigned long	WOUTHSZ_2		:14;
		unsigned long					:2;
		unsigned long	WOUTVSZ_2		:14;
		unsigned long					:2;
		unsigned long	WOUTHSZ_3		:14;
		unsigned long					:2;
		unsigned long	WOUTVSZ_3		:14;
		unsigned long					:2;
		unsigned long	WOUTHSZ_4		:14;
		unsigned long					:2;
		unsigned long	WOUTVSZ_4		:14;
		unsigned long					:2;
		unsigned long	WOUTHSZ_5		:14;
		unsigned long					:2;
		unsigned long	WOUTVSZ_5		:14;
		unsigned long					:2;
	}bit;
};

/*	structure of RSZHCNT_2/3/4/5	*/
union io_shdr_rszhcnt{
	unsigned long		word;
	struct {
		unsigned long	RSZHSTA			:14;
		unsigned long					:2;
		unsigned long	RSZHPIT			:15;
		unsigned long					:1;
	}bit;
};

/*	structure of RSZVCNT_2/3/4/5	*/
union io_shdr_rszvcnt{
	unsigned long		word;
	struct {
		unsigned long	RSZVSTA			:14;
		unsigned long					:2;
		unsigned long	RSZVPIT			:15;
		unsigned long					:1;
	}bit;
};

/*	structure of TRMSTA_2/3/4/5		*/
union io_shdr_trmsta{
	unsigned long		word;
	struct {
		unsigned long	TRMHSTA			:14;
		unsigned long					:2;
		unsigned long	TRMVSTA			:14;
		unsigned long					:2;
	}bit;
};

/*	structure of DRCOFST_0/2/3		*/
union io_shdr_drcofst{
	unsigned long		word;
	struct {
		unsigned long	DRCYOFST		:5;
		unsigned long					:3;
		unsigned long					:8;
		long			DRCCBOFST		:5;
		unsigned long					:3;
		long			DRCCROFST		:5;
		unsigned long					:3;
	}bit;
};

/*	structure of DRCGAIN_0/2/3	*/
union io_shdr_drcgain{
	unsigned long		word;
	struct {
		unsigned long	DRCYGAIN		:9;
		unsigned long					:7;
		unsigned long	DRCCBGAIN		:8;
		unsigned long	DRCCRGAIN		:8;
	}bit;
};

/*	structure of DRCCLPH_0/2/3	*/
union io_shdr_drcclph{
	unsigned long		word;
	struct {
		unsigned long	DRCYCLPH		:8;
		unsigned long					:8;
		long			DRCCBCLPH		:8;
		long			DRCCRCLPH		:8;
	}bit;
};

/*	structure of DRCCLPL_0/2/3	*/
union io_shdr_drcclpl{
	unsigned long		word;
	struct {
		unsigned long	DRCYCLPL		:8;
		unsigned long					:8;
		long			DRCCBCLPL		:8;
		long			DRCCRCLPL		:8;
	}bit;
};

/*	structure of MCARMD		*/
union io_shdr_mcarmd{
	unsigned long		word;
	struct {
		unsigned long	MCAMD			:2;
		unsigned long					:2;
		unsigned long					:4;
		unsigned long	MCRMD			:2;
		unsigned long					:22;
	}bit;
};

/*	structure of MCMPMD		*/
union io_shdr_mcmpmd{
	unsigned long		word;
	struct {
		unsigned long	MCMPMD			:1;
		unsigned long					:3;
		unsigned long					:4;
		unsigned long	MVWEN			:1;
		unsigned long					:23;
	}bit;
};

/*	structure of MMVVAL		*/
union io_shdr_mmvval{
	unsigned long		word;
	struct {
		unsigned long	MMVX			:7;
		unsigned long					:1;
		unsigned long					:8;
		unsigned long	MMVY			:7;
		unsigned long					:9;
	}bit;
};

/*	structure of MMVRCNT	*/
union io_shdr_mmvrcnt{
	unsigned long		word;
	struct {
		unsigned long	MMVROX			:5;
		unsigned long					:3;
		unsigned long	MMVROY			:5;
		unsigned long					:3;
		unsigned long	MMVRKX			:3;
		unsigned long					:1;
		unsigned long					:4;
		unsigned long	MMVRKY			:3;
		unsigned long					:5;
	}bit;
};

/*	structure of MDBCNT		*/
union io_shdr_mdbcnt{
	unsigned long		word;
	struct {
		unsigned long	MDBX			:3;
		unsigned long					:5;
		unsigned long					:8;
		unsigned long	MDBY			:3;
		unsigned long					:13;
	}bit;
};

/*	structure of MDBROCNT	*/
union io_shdr_mdbrocnt{
	unsigned long		word;
	struct {
		unsigned long	MDBROX			:5;
		unsigned long					:3;
		unsigned long	MDBROY			:5;
		unsigned long					:3;
		unsigned long	MDBRKX			:3;
		unsigned long					:1;
		unsigned long					:4;
		unsigned long	MDBRKY			:3;
		unsigned long					:5;
	}bit;
};

/*	structure of MOLRCNT	*/
union io_shdr_molrcnt{
	unsigned long		word;
	struct {
		unsigned long	MOLRSM			:4;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long	MOLRSS			:4;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long	MOLRO			:5;
		unsigned long					:3;
		unsigned long	MOLRK			:2;
		unsigned long					:6;
	}bit;
};

/*	structure of MCPCNT		*/
union io_shdr_mcpcnt{
	unsigned long		word;
	struct {
		unsigned long	MCPXS			:8;
		unsigned long	MCPXO			:5;
		unsigned long					:3;
		unsigned long	MCPXK			:3;
		unsigned long					:13;
	}bit;
};

/*	structure of MMCRCNT	*/
union io_shdr_mmcrcnt{
	unsigned long		word;
	struct {
		unsigned long	MMCRS			:10;
		unsigned long					:2;
		unsigned long	MMCRK			:3;
		unsigned long					:17;
	}bit;
};

/*	structure of MOLECNT	*/
union io_shdr_molecnt{
	unsigned long		word;
	struct {
		unsigned long	MOLES			:10;
		unsigned long					:6;
		unsigned long	MOLEO			:5;
		unsigned long					:3;
		unsigned long	MOLEK			:3;
		unsigned long					:5;
	}bit;
};

/*	structure of MMVECNT	*/
union io_shdr_mmvecnt{
	unsigned long		word;
	struct {
		unsigned long	MMVES			:10;
		unsigned long					:6;
		unsigned long	MMVEO			:5;
		unsigned long					:3;
		unsigned long	MMVEK			:3;
		unsigned long					:5;
	}bit;
};

/*	structure of MCPXECNT	*/
union io_shdr_mcpxecnt{
	unsigned long		word;
	struct {
		unsigned long	MCPXES			:10;
		unsigned long					:6;
		unsigned long	MCPXEO			:5;
		unsigned long					:3;
		unsigned long	MCPXEK			:3;
		unsigned long					:5;
	}bit;
};

/*	structure of MVLCNT		*/
union io_shdr_mvlcnt{
	unsigned long		word;
	struct {
		unsigned long	MVLEN			:1;
		unsigned long					:7;
		unsigned long	MVLTH			:7;
		unsigned long					:1;
		unsigned long	MVLCTH			:7;
		unsigned long					:9;
	}bit;
};

/* Define i/o mapping */
struct io_jdsshdr{
	/* JDSIMG */
	union  io_shdr_shdractl0		SHDRACTL0;			/* (0000 - 0003h) */
	union  io_shdr_shdractl1		SHDRACTL1;			/* (0004 - 0007h) */
	union  io_shdr_shdractl2		SHDRACTL2;			/* (0008 - 000Bh) */

	unsigned char dmy_A00C_A01F[0xA020-0xA00C];			/* (000C - 001Fh) */

	union  io_shdr_shdrint0			SHDRINT0;			/* (0020 - 0023h) */
	union  io_shdr_shdrint1			SHDRINT1;			/* (0024 - 0027h) */

	unsigned char dmy_A028_A07F[0xA080-0xA028];			/* (0028 - 007Fh) */

	union  io_shdr_axmd				AXMD;				/* (0080 - 0083h) */
	union  io_shdr_axictlara		AXICTLARA;			/* (0084 - 0087h) */
	union  io_shdr_axictlarb		AXICTLARB;			/* (0088 - 008Bh) */
	union  io_shdr_axictlarc		AXICTLARC;			/* (008C - 008Fh) */
	union  io_shdr_axictlard		AXICTLARD;			/* (0090 - 0093h) */
	union  io_shdr_axictlawa		AXICTLAWA;			/* (0094 - 0097h) */
	union  io_shdr_axictlawb		AXICTLAWB;			/* (0098 - 009Fh) */
	union  io_shdr_axictlawc		AXICTLAWC;			/* (00A0 - 00A7h) */
	union  io_shdr_axictlra			AXICTLRA;			/* (00A8 - 00ABh) */
	union  io_shdr_axictlrb			AXICTLRB;			/* (00AC - 00AFh) */
	union  io_shdr_axictlrc			AXICTLRC;			/* (00B0 - 00B3h) */
	union  io_shdr_axictlrd			AXICTLRD;			/* (00B4 - 00B7h) */
	union  io_shdr_axictlwa			AXICTLWA;			/* (00B8 - 00BBh) */

	unsigned char dmy_A0BC_A0BF[0xA0C0-0xA0BC];			/* (00BC - 00BFh) */

	union  io_shdr_axictlwb			AXICTLWB;			/* (00C0 - 00C7h) */
	union  io_shdr_axictlwc			AXICTLWC;			/* (00C8 - 00CFh) */
	union  io_shdr_coremon0			COREMON0;			/* (00D0 - 00D3h) */
	union  io_shdr_coremon1			COREMON1;			/* (00D4 - 00D7h) */
	union  io_shdr_dgt_rsel			DGT_RSEL;			/* (00D8 - 00DBh) */

	unsigned char dmy_A0DC_A0FF[0xA100-0xA0DC];			/* (00DC - 00FFh) */

	union  io_shdr_sdrr_addr_a		SDRR_ADDR_A;		/* (0100 - 0107h) */
	union  io_shdr_sdrr_addr_b		SDRR_ADDR_B;		/* (0108 - 010Fh) */
	union  io_shdr_sdrr_addr_c		SDRR_ADDR_C;		/* (0110 - 0117h) */
	union  io_shdr_sdrr_addr_d		SDRR_ADDR_D;		/* (0118 - 011Fh) */
	union  io_shdr_sdrr_addr_e		SDRR_ADDR_E;		/* (0120 - 0127h) */
	union  io_shdr_sdrr_addr_f		SDRR_ADDR_F;		/* (0128 - 012Bh) */

	unsigned char dmy_A12C_A12F[0xA130-0xA12C];			/* (012C - 012Fh) */

	union  io_shdr_sdrr_ghsize_a	SDRR_GHSIZE_A;		/* (0130 - 0137h) */
	union  io_shdr_sdrr_ghsize_b	SDRR_GHSIZE_B;		/* (0138 - 013Fh) */
	union  io_shdr_sdrr_ghsize_c	SDRR_GHSIZE_C;		/* (0140 - 0147h) */
	union  io_shdr_sdrr_ghsize_d	SDRR_GHSIZE_D;		/* (0148 - 014Fh) */
	union  io_shdr_sdrr_ghsize_e	SDRR_GHSIZE_E;		/* (0150 - 0157h) */
	union  io_shdr_sdrr_ghsize_f	SDRR_GHSIZE_F;		/* (0158 - 015Bh) */

	unsigned char dmy_A15C_A15F[0xA160-0xA15C];			/* (015C - 015Fh) */

	union  io_shdr_sdrr_vfm			SDRR_VFM;			/* (0160 - 0163h) */

	unsigned char dmy_A164_A16F[0xA170-0xA164];			/* (0164 - 016Fh) */

	union  io_shdr_sdrw_addr_b		SDRW_ADDR_B;		/* (0170 - 0177h) */
	union  io_shdr_sdrw_addr_e		SDRW_ADDR_E;		/* (0178 - 017Fh) */
	union  io_shdr_sdrw_addr_f		SDRW_ADDR_F;		/* (0180 - 0187h) */
	union  io_shdr_sdrw_addr_g		SDRW_ADDR_G;		/* (0188 - 0193h) */
	union  io_shdr_sdrw_addr_h		SDRW_ADDR_H;		/* (0194 - 019Bh) */
	union  io_shdr_sdrw_addr_i		SDRW_ADDR_I;		/* (019C - 019Fh) */

	union  io_shdr_sdrw_ghsize_b	SDRW_GHSIZE_B;		/* (01A0 - 01A7h) */
	union  io_shdr_sdrw_ghsize_e	SDRW_GHSIZE_E;		/* (01A8 - 01AFh) */
	union  io_shdr_sdrw_ghsize_f	SDRW_GHSIZE_F;		/* (01B0 - 01B7h) */
	union  io_shdr_sdrw_ghsize_g	SDRW_GHSIZE_G;		/* (01B8 - 01C3h) */
	union  io_shdr_sdrw_ghsize_h	SDRW_GHSIZE_H;		/* (01C4 - 01CBh) */
	union  io_shdr_sdrw_ghsize_i	SDRW_GHSIZE_I;		/* (01CC - 01CFh) */

	union  io_shdr_sdrw_vfm			SDRW_VFM;			/* (01D0 - 01D3h) */

	unsigned char dmy_A1D4_A1D7[0xA1D8-0xA1D4];			/* (01D4 - 01D7h) */

	union  io_shdr_sdrw_ba_a		SDRW_BA_A;			/* (01D8 - 01DFh) */
	union  io_shdr_sdrw_ba_c		SDRW_BA_C;			/* (01E0 - 01E7h) */
	union  io_shdr_sdrw_ba_d		SDRW_BA_D;			/* (01E8 - 01EFh) */
	union  io_shdr_sdrw_fw_a		SDRW_FW_A;			/* (01F0 - 01F3h) */
	union  io_shdr_sdrw_fw_c		SDRW_FW_C;			/* (01F4 - 01F7h) */
	union  io_shdr_sdrw_fw_d		SDRW_FW_D;			/* (01F8 - 01FBh) */

	unsigned char dmy_A1FC_A1FF[0xA200-0xA1FC];			/* (01FC - 01FFh) */

	union  io_shdr_shdrifrm			SHDRIFRM;			/* (0200 - 0203h) */
	union  io_shdr_shdrofrm			SHDROFRM;			/* (0204 - 0207h) */

	unsigned char dmy_A208_A20F[0xA210-0xA208];			/* (0208 - 020Fh) */

	union  io_shdr_sdrr_hsize_a		SDRR_HSIZE_A;		/* (0210 - 0213h) */
	union  io_shdr_sdrr_hsize_b		SDRR_HSIZE_B;		/* (0214 - 0217h) */
	union  io_shdr_sdrr_hsize_c		SDRR_HSIZE_C;		/* (0218 - 021Bh) */
	union  io_shdr_sdrr_hsize_d		SDRR_HSIZE_D;		/* (021C - 021Fh) */
	union  io_shdr_sdrr_hsize_e		SDRR_HSIZE_E;		/* (0220 - 0223h) */

	unsigned char dmy_A224_A22F[0xA230-0xA224];			/* (0224 - 022Fh) */

	union  io_shdr_sdrr_vsize_a		SDRR_VSIZE_A;		/* (0230 - 0233h) */
	union  io_shdr_sdrr_vsize_b		SDRR_VSIZE_B;		/* (0234 - 0237h) */
	union  io_shdr_sdrr_vsize_c		SDRR_VSIZE_C;		/* (0238 - 023Bh) */
	union  io_shdr_sdrr_vsize_d		SDRR_VSIZE_D;		/* (023C - 023Fh) */
	union  io_shdr_sdrr_vsize_e		SDRR_VSIZE_E;		/* (0240 - 0243h) */

	unsigned char dmy_A244_A24F[0xA250-0xA244];			/* (0244 - 024Fh) */

	union  io_shdr_exmag			EXMAG;				/* (0250 - 0253h) */
	union  io_shdr_remval			REMVAL;				/* (0254 - 0257h) */

	unsigned char dmy_A258_A25F[0xA260-0xA258];			/* (0258 - 025Fh) */

	union  io_shdr_shift			SHIFT;				/* (0260 - 0263h) */
	union  io_shdr_mrgn				MRGN;				/* (0264 - 0267h) */

	unsigned char dmy_A268_A26F[0xA270-0xA268];			/* (0268 - 026Fh) */

	union  io_shdr_flwcnt			FLWCNT;				/* (0270 - 0273h) */
	union  io_shdr_tmode			TMODE;				/* (0274 - 0277h) */

	unsigned char dmy_A278_A2FF[0xA300-0xA278];			/* (0278 - 02FFh) */

	/* COMPIP	*/
	union  io_shdr_modesel			MODESEL;			/* (0300 - 0303h) */
	union  io_shdr_mcmodesel		MC_MODESEL;			/* (0304 - 0307h) */
	union  io_shdr_compincfmsel		COMP_INCFMSEL;		/* (0308 - 030Bh) */
	union  io_shdr_bypasson			BYPASSON;			/* (030C - 030Fh) */
	union  io_shdr_axisize			AXISIZE;			/* (0310 - 0313h) */
	union  io_shdr_lint				LINT;				/* (0314 - 031Fh) */
	union  io_shdr_wrstartaddr		WRSTARTADDR;		/* (0320 - 0327h) */
	union  io_shdr_rdstartaddr		RDSTARTADDR;		/* (0328 - 032Fh) */

	unsigned char dmy_A330_A3FF[0xA400-0xA330];			/* (0330 - 03FFh) */

	union  io_shdr_premd			PREMD;				/* (0400 - 0403h) */

	unsigned char dmy_A404_A41F[0xA420-0xA404];			/* (0404 - 041Fh) */

	union  io_shdr_y2rcoef			Y2RCOEF;			/* (0420 - 0433h) */

	unsigned char dmy_A434_A43F[0xA440-0xA434];			/* (0434 - 043Fh) */

	union  io_shdr_prey2rclpul0		PREY2RCLPUL0;		/* (0440 - 0447h) */
	union  io_shdr_prey2rclpll0		PREY2RCLPLL0;		/* (0448 - 044Fh) */
	unsigned char dmy_A450_A45F[0xA460-0xA450];			/* (0450 - 0457h) */
														/* (0458 - 045Fh) */
	union  io_shdr_prey2rclpus		PREY2RCLPUS;		/* (0460 - 0467h) */
	union  io_shdr_prey2rclpls		PREY2RCLPLS;		/* (0468 - 046Fh) */

	unsigned char dmy_A470_A47F[0xA480-0xA470];			/* (0470 - 047Fh) */

	union  io_shdr_r2ycoef			R2YCOEF;			/* (0480 - 0493h) */

	unsigned char dmy_A494_A49F[0xA4A0-0xA494];			/* (0494 - 049Fh) */

	union  io_shdr_prer2yclpul		PRER2YCLPUL;		/* (04A0 - 04ABh) */

	unsigned char dmy_A4AC_A4AF[0xA4B0-0xA4AC];			/* (04AC - 04AFh) */

	union  io_shdr_prer2yclpll		PRER2YCLPLL;		/* (04B0 - 04BBh) */

	unsigned char dmy_A4BC_A4BF[0xA4C0-0xA4BC];			/* (04BC - 04BFh) */

	union  io_shdr_prer2yclpus		PRER2YCLPUS;		/* (04C0 - 04C7h) */
	union  io_shdr_prer2yclpls		PRER2YCLPLS;		/* (04C8 - 04CFh) */

	unsigned char dmy_A4D0_A4F3[0xA4F4-0xA4D0];			/* (04D0 - 04DFh) */

														/* (04E0 - 04F3h) */

	unsigned char dmy_A4F4_A513[0xA514-0xA4F4];			/* (04F4 - 04FFh) */

														/* (0500 - 0513h) */

	unsigned char dmy_A514_A51F[0xA520-0xA514];			/* (0514 - 051Fh) */

	union  io_shdr_sknofs			SKNOFS;				/* (0520 - 052Bh) */

	unsigned char dmy_A52C_A52F[0xA530-0xA52C];			/* (052C - 052Fh) */

	union  io_shdr_sknsl			SKNSL;				/* (0530 - 053Bh) */

	unsigned char dmy_A53C_A543[0xA544-0xA53C];			/* (053C - 0543h) */

	union  io_shdr_sknb				SKNB1;				/* (0544 - 0547h) */
	union  io_shdr_sknb				SKNB2;				/* (0548 - 054Bh) */

	unsigned char dmy_A54C_A54F[0xA550-0xA54C];			/* (054C - 054Fh) */

	union  io_shdr_lknofs			LKNOFS;				/* (0550 - 055Bh) */

	unsigned char dmy_A55C_A55F[0xA560-0xA55C];			/* (055C - 055Fh) */

	union  io_shdr_lknsl			LKNSL;				/* (0560 - 056Bh) */

	unsigned char dmy_A56C_A573[0xA574-0xA56C];			/* (056C - 0573h) */

	union  io_shdr_lknb				LKNB1;				/* (0574 - 0577h) */
	union  io_shdr_lknb				LKNB2;				/* (0578 - 057Bh) */

	unsigned char dmy_A57C_A5FF[0xA600-0xA57C];			/* (057C - 05FFh) */

	union  io_shdr_epbd				EPBD;				/* (0600 - 0607h) */
	union  io_shdr_epof				EPOF;				/* (0608 - 060Fh) */
	union  io_shdr_epbld			EPBLD;				/* (0610 - 0613h) */
	union  io_shdr_fdweight			FDWEIGHT;			/* (0614 - 0617h) */
	union  io_shdr_mvcnt			MVCNT;				/* (0618 - 061Bh) */

	unsigned char dmy_A61C_A61F[0xA620-0xA61C];			/* (061C - 061Fh) */

	union  io_shdr_bldrofs			BLDROFS;			/* (0620 - 062Bh) */

	unsigned char dmy_A62C_A62F[0xA630-0xA62C];			/* (062C - 062Fh) */

	union  io_shdr_bldrsl			BLDRSL;				/* (0630 - 063Bh) */

	unsigned char dmy_A63C_A63F[0xA640-0xA63C];			/* (063C - 063Fh) */

	union  io_shdr_bldrb			BLDRB;				/* (0640 - 064Bh) */

	unsigned char dmy_A64C_A64F[0xA650-0xA64C];			/* (064C - 064Fh) */

	union  io_shdr_wghtr2ycoef		WGHTR2YCOEF;		/* (0650 - 0657h) */
	union  io_shdr_wghtr2yclp		WGHTR2YCLP;			/* (0658 - 065Bh) */

	unsigned char dmy_A65C_A65F[0xA660-0xA65C];			/* (065C - 065Fh) */

	union  io_shdr_wghtbase			WGHTBASE;			/* (0660 - 0663h) */

	unsigned char dmy_A664_A66F[0xA670-0xA664];			/* (0664 - 066Fh) */

	union  io_shdr_wghtrofs			WGHTROFS;			/* (0670 - 067Bh) */

	unsigned char dmy_A67C_A67F[0xA680-0xA67C];			/* (067C - 067Fh) */

	union  io_shdr_wghtgofs			WGHTGOFS;			/* (0680 - 068Bh) */

	unsigned char dmy_A68C_A68F[0xA690-0xA68C];			/* (068C - 068Fh) */

	union  io_shdr_wghtbofs			WGHTBOFS;			/* (0690 - 069Bh) */

	unsigned char dmy_A69C_A69F[0xA6A0-0xA69C];			/* (069C - 069Fh) */

	union  io_shdr_wghtrsl			WGHTRSL;			/* (06A0 - 06ABh) */

	unsigned char dmy_A6AC_A6AF[0xA6B0-0xA6AC];			/* (06AC - 06AFh) */

	union  io_shdr_wghtgsl			WGHTGSL;			/* (06B0 - 06BBh) */

	unsigned char dmy_A6BC_A6BF[0xA6C0-0xA6BC];			/* (06BC - 06BFh) */

	union  io_shdr_wghtbsl			WGHTBSL;			/* (06C0 - 06CBh) */

	unsigned char dmy_A6CC_A6CF[0xA6D0-0xA6CC];			/* (06CC - 06CFh) */

	union  io_shdr_wghtrb			WGHTRB;				/* (06D0 - 06DBh) */

	unsigned char dmy_A6DC_A6DF[0xA6E0-0xA6DC];			/* (06DC - 06DFh) */

	union  io_shdr_wghtgb			WGHTGB;				/* (06E0 - 06EBh) */

	unsigned char dmy_A6EC_A6EF[0xA6F0-0xA6EC];			/* (06EC - 06EFh) */

	union  io_shdr_wghtbb			WGHTBB;				/* (06F0 - 06FBh) */

	unsigned char dmy_A6FC_A6FF[0xA700-0xA6FC];			/* (06FC - 06FFh) */

	union  io_shdr_isoth			ISOTH;				/* (0700 - 0703h) */
	union  io_shdr_isocoef			ISOCOEF;			/* (0704 - 0707h) */
	union  io_shdr_satth			SATTH;				/* (0708 - 070Bh) */

	unsigned char dmy_A70C_A713[0xA714-0xA70C];			/* (070C - 070Fh) */

														/* (0710 - 0713h) */

	unsigned char dmy_A714_A723[0xA724-0xA714];			/* (0714 - 0717h) */

														/* (0718 - 071Fh) */
														/* (0720 - 0723h) */
	union  io_shdr_exp77md			EXP77MD;			/* (0724 - 0727h) */
	union  io_shdr_fmdsel			FMDSEL;				/* (0728 - 072Bh) */

	unsigned char dmy_A72C_A7FF[0xA800-0xA72C];			/* (072C - 07FFh) */

	union  io_shdr_tlut				TLUT;				/* (0800 - 0813h) */

	unsigned char dmy_A814_A81F[0xA820-0xA814];			/* (0814 - 081Fh) */

	union  io_shdr_pmskmd			PMSKMD;				/* (0820 - 0823h) */

	unsigned char dmy_A824_A83F[0xA840-0xA824];			/* (0824 - 083Fh) */

	union  io_shdr_spnren			SPNREN;				/* (0840 - 0843h) */
	union  io_shdr_spebd			SPEBD;				/* (0844 - 084Bh) */

	unsigned char dmy_A84C_A84F[0xA850-0xA84C];			/* (084C - 084Fh) */

	union  io_shdr_speof			SPEOF;				/* (0850 - 0857h) */
	union  io_shdr_spebld			SPEBLD;				/* (0858 - 085Bh) */

	unsigned char dmy_A85C_A8FF[0xA900-0xA85C];			/* (085C - 08FFh) */

	union  io_shdr_msky2rclpul		MSKY2RCLPUL;		/* (0900 - 0907h) */
	union  io_shdr_msky2rclpll		MSKY2RCLPLL;		/* (0908 - 090Fh) */
	union  io_shdr_msky2rclpus		MSKY2RCLPUS;		/* (0910 - 0917h) */
	union  io_shdr_msky2rclpls		MSKY2RCLPLS;		/* (0918 - 091Fh) */
	union  io_shdr_alpha			ALPHA;				/* (0920 - 0923h) */

	unsigned char dmy_A924_A93F[0xA940-0xA924];			/* (0924 - 093Fh) */

	union  io_shdr_lpfc				LPFC;				/* (0940 - 0953h) */

	unsigned char dmy_A954_A963[0xA964-0xA954];			/* (0954 - 095Fh) */

														/* (0960 - 0963h) */

	unsigned char dmy_A964_A96F[0xA970-0xA964];			/* (0964 - 096Fh) */

	union  io_shdr_bldofs			BLDOFS;				/* (0970 - 097Fh) */
	union  io_shdr_bldsl			BLDSL;				/* (0980 - 099Bh) */

	unsigned char dmy_A99C_A99F[0xA9A0-0xA99C];			/* (099C - 099Fh) */

	union  io_shdr_bldb				BLDB;				/* (09A0 - 09AFh) */
	union  io_shdr_normgain			NORMGAIN;			/* (09B0 - 09B7h) */
	union  io_shdr_normofs			NORMOFS;			/* (09B8 - 09BFh) */
	union  io_shdr_mskmd			MSKMD;				/* (09C0 - 09C3h) */

	unsigned char dmy_A9C4_A9FF[0xAA00-0xA9C4];			/* (09C4 - 09FFh) */

	union  io_shdr_bldsel			BLDSEL;				/* (0A00 - 0A03h) */

	unsigned char dmy_AA04_AA0F[0xAA10-0xAA04];			/* (0A04 - 0A0Fh) */

	union  io_shdr_alphasel			ALPHASEL;			/* (0A10 - 0A13h) */

	unsigned char dmy_AA14_AA1F[0xAA20-0xAA14];			/* (0A14 - 0A1Fh) */

	union  io_shdr_alphaval			ALPHAVAL;			/* (0A20 - 0A27h) */

	unsigned char dmy_AA28_AA33[0xAA34-0xAA28];			/* (0A28 - 0A2Fh) */

														/* (0A30 - 0A33h) */

	unsigned char dmy_AA34_AAFF[0xAB00-0xAA34];			/* (0A34 - 0AFFh) */

	union  io_shdr_postsft1rnd		POSTSFT1RND;		/* (0B00 - 0B03h) */

	unsigned char dmy_AB04_AB1F[0xAB20-0xAB04];			/* (0B04 - 0B1Fh) */

	union  io_shdr_posty2rcoef		POSTY2RCOEF;		/* (0B20 - 0B33h) */

	unsigned char dmy_AB34_AB3F[0xAB40-0xAB34];			/* (0B34 - 0B3Fh) */

	union  io_shdr_posty2rclpthu	POSTY2RCLPTHU;		/* (0B40 - 0B47h) */
	union  io_shdr_posty2rclpthl	POSTY2RCLPTHL;		/* (0B48 - 0B4Fh) */
	union  io_shdr_postsel1			POSTSEL1;			/* (0B50 - 0B53h) */
	union  io_shdr_postmulqpos		POSTMULQPOS;		/* (0B54 - 0B57h) */

	unsigned char dmy_AB58_AB5F[0xAB60-0xAB58];			/* (0B58 - 0B5Fh) */

	union  io_shdr_evknb			EVKNB;				/* (0B60 - 0B73h) */

	unsigned char dmy_AB74_AB7F[0xAB80-0xAB74];			/* (0B74 - 0B7Fh) */

	union  io_shdr_evknofs			EVKNOFS;			/* (0B80 - 0B93h) */

	unsigned char dmy_AB94_AB9F[0xABA0-0xAB94];			/* (0B94 - 0B9Fh) */

	union  io_shdr_evknsl			EVKNSL;				/* (0BA0 - 0BB3h) */

	unsigned char dmy_ABB4_ABBF[0xABC0-0xABB4];			/* (0BB4 - 0BBFh) */

	union  io_shdr_postsel0			POSTSEL0;			/* (0BC0 - 0BC3h) */
	union  io_shdr_hdrcssmd			HDRCSSMD;			/* (0BC4 - 0BC7h) */

	unsigned char dmy_ABC8_ABFF[0xAC00-0xABC8];			/* (0BC8 - 0BFFh) */

	union  io_shdr_ch0mode			CH0MODE;			/* (0C00 - 0C03h) */
	union  io_shdr_ch1mode			CH1MODE;			/* (0C04 - 0C07h) */
	union  io_shdr_ch2mode			CH2MODE;			/* (0C08 - 0C0Bh) */
	union  io_shdr_ch3mode			CH3MODE;			/* (0C0C - 0C0Fh) */
	union  io_shdr_ch4mode			CH4MODE;			/* (0C10 - 0C13h) */
	union  io_shdr_ch5mode			CH5MODE;			/* (0C14 - 0C17h) */

	unsigned char dmy_AC18_AC1F[0xAC20-0xAC18];			/* (0C18 - 0C1Fh) */

	union  io_shdr_woutsz			WOUTSZ;				/* (0C20 - 0C37h) */

	unsigned char dmy_AC38_AC47[0xAC48-0xAC38];			/* (0C38 - 0C47h) */

	union  io_shdr_rszhcnt			RSZHCNT_2;			/* (0C48 - 0C4Bh) */
	union  io_shdr_rszhcnt			RSZHCNT_3;			/* (0C4C - 0C4Fh) */
	union  io_shdr_rszhcnt			RSZHCNT_4;			/* (0C50 - 0C53h) */
	union  io_shdr_rszhcnt			RSZHCNT_5;			/* (0C54 - 0C57h) */

	unsigned char dmy_AC58_AC67[0xAC68-0xAC58];			/* (0C58 - 0C67h) */

	union  io_shdr_rszvcnt			RSZVCNT_2;			/* (0C68 - 0C6Bh) */
	union  io_shdr_rszvcnt			RSZVCNT_3;			/* (0C6C - 0C6Fh) */
	union  io_shdr_rszvcnt			RSZVCNT_4;			/* (0C70 - 0C73h) */
	union  io_shdr_rszvcnt			RSZVCNT_5;			/* (0C74 - 0C77h) */

	unsigned char dmy_AC78_AC87[0xAC88-0xAC78];			/* (0C78 - 0C87h) */

	union  io_shdr_trmsta			TRMSTA_2;			/* (0C88 - 0C8Bh) */
	union  io_shdr_trmsta			TRMSTA_3;			/* (0C8C - 0C8Fh) */
	union  io_shdr_trmsta			TRMSTA_4;			/* (0C90 - 0C93h) */
	union  io_shdr_trmsta			TRMSTA_5;			/* (0C94 - 0C97h) */

	unsigned char dmy_AC98_AC9F[0xACA0-0xAC98];			/* (0C98 - 0C9Fh) */

	union  io_shdr_drcofst			DRCOFST_0;			/* (0CA0 - 0CA3h) */

	unsigned char dmy_ACA4_ACA7[0xACA8-0xACA4];			/* (0CA4 - 0CA7h) */

	union  io_shdr_drcofst			DRCOFST_2;			/* (0CA8 - 0CABh) */
	union  io_shdr_drcofst			DRCOFST_3;			/* (0CAC - 0CAFh) */
	union  io_shdr_drcgain			DRCGAIN_0;			/* (0CB0 - 0CB3h) */

	unsigned char dmy_ACB4_ACB7[0xACB8-0xACB4];			/* (0CB4 - 0CB7h) */

	union  io_shdr_drcgain			DRCGAIN_2;			/* (0CB8 - 0CBBh) */
	union  io_shdr_drcgain			DRCGAIN_3;			/* (0CBC - 0CBFh) */
	union  io_shdr_drcclph			DRCCLPH_0;			/* (0CC0 - 0CC3h) */

	unsigned char dmy_ACC4_ACC7[0xACC8-0xACC4];			/* (0CC4 - 0CC7h) */

	union  io_shdr_drcclph			DRCCLPH_2;			/* (0CC8 - 0CCBh) */
	union  io_shdr_drcclph			DRCCLPH_3;			/* (0CCC - 0CCFh) */
	union  io_shdr_drcclpl			DRCCLPL_0;			/* (0CD0 - 0CD3h) */

	unsigned char dmy_ACD4_ACD7[0xACD8-0xACD4];			/* (0CD4 - 0CD7h) */

	union  io_shdr_drcclpl			DRCCLPL_2;			/* (0CD8 - 0CDBh) */
	union  io_shdr_drcclpl			DRCCLPL_3;			/* (0CDC - 0CDFh) */

	unsigned char dmy_ACE0_ACFF[0xAD00-0xACE0];			/* (0CE0 - 0CFFh) */

	union  io_shdr_mcarmd			MCARMD;				/* (0D00 - 0D03h) */
	union  io_shdr_mcmpmd			MCMPMD;				/* (0D04 - 0D07h) */

	unsigned char dmy_AD08_AD0F[0xAD10-0xAD08];			/* (0D08 - 0D0Fh) */

	union  io_shdr_mmvval			MMVVAL;				/* (0D10 - 0D13h) */
	union  io_shdr_mmvrcnt			MMVRCNT;			/* (0D14 - 0D17h) */
	union  io_shdr_mdbcnt			MDBCNT;				/* (0D18 - 0D1Bh) */
	union  io_shdr_mdbrocnt			MDBROCNT;			/* (0D1C - 0D1Fh) */
	union  io_shdr_molrcnt			MOLRCNT;			/* (0D20 - 0D23h) */

	unsigned char dmy_AD24_AD27[0xAD28-0xAD24];			/* (0D24 - 0D27h) */

	union  io_shdr_mcpcnt			MCPCNT;				/* (0D28 - 0D2Bh) */

	unsigned char dmy_AD2C_AD2F[0xAD30-0xAD2C];			/* (0D2C - 0D2Fh) */

	union  io_shdr_mmcrcnt			MMCRCNT;			/* (0D30 - 0D33h) */

	unsigned char dmy_AD34_AD37[0xAD38-0xAD34];			/* (0D34 - 0D37h) */

	union  io_shdr_molecnt			MOLECNT;			/* (0D38 - 0D3Bh) */
	union  io_shdr_mmvecnt			MMVECNT;			/* (0D3C - 0D3Fh) */
	union  io_shdr_mcpxecnt			MCPXECNT;			/* (0D40 - 0D43h) */

	unsigned char dmy_AD44_AD47[0xAD48-0xAD44];			/* (0D44 - 0D47h) */

	union  io_shdr_mvlcnt			MVLCNT;				/* (0D48 - 0D4Bh) */

	unsigned char dmy_AD4C_AFFF[0xB000-0xAD4C];			/* (0D4C - 0FFFh) */
};

extern volatile struct io_jdsshdr	IO_SHDR;
