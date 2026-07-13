/**************************************************************************/
/*  <Contents>   Definition IIP FLT UNIT register                         */
/*  <Filename>   _ipc_iip_unit_flt.h                                      */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
#include "_ipc_iip_packet_header.h"

/* structure of TDLTOPCNF			(0000h) */
union ipc_iip_tdltopcnf{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF_0		:6;
		unsigned long					:2;
		unsigned long	WAITCONF_1		:6;
		unsigned long					:2;
		
		unsigned long	DATACONF		:6;
		unsigned long					:10;
	}bit;
};

/* structure of PIXIDDEF			(0004h) */
union ipc_iip_flt_pixiddef{
	unsigned long		word;
	struct{
		unsigned long	IPIXID0			:4;
		unsigned long					:12;
		
		unsigned long	IPIXID1			:4;
		unsigned long					:12;
	}bit;
};

/* structure of TDL_CTL				(0008h) */
union ipc_iip_tdl_ctl{
	unsigned long		word;
	struct{
		unsigned long	FM				:2;
		unsigned long					:2;
		unsigned long	LNUM			:1;
		unsigned long					:3;
		unsigned long	SAVERD			:2;
		unsigned long					:2;
		unsigned long	INPUT_MODE		:2;
		unsigned long					:2;
		
		unsigned long	CSEL			:2;
		unsigned long					:14;
	}bit;
};

/* structure of CALC_MD				(000Ch) */
union ipc_iip_calc_md{
	unsigned long		word;
	struct{
		unsigned long	CALC_MD			:2;
		unsigned long					:14;
		
		unsigned long					:16;
	}bit;
};

/* structure of TDL_HSIZE			(0010h) */
union ipc_iip_tdl_hsize{
	unsigned long		word;
	struct{
		unsigned long	HSTART			:14;
		unsigned long					:2;
		
		unsigned long	HEND			:14;
		unsigned long					:2;
	}bit;
};

/* structure of TDL_VSIZE			(0014h) */
union ipc_iip_tdl_vsize{
	unsigned long		word;
	struct{
		unsigned long	VSTART			:14;
		unsigned long					:2;
		
		unsigned long	VEND			:14;
		unsigned long					:2;
	}bit;
};

/* structure of TDL_PHSZ			(0018h) */
union ipc_iip_tdl_phsz{
	unsigned long		word;
	struct{
		unsigned long	PHSZ0			:14;
		unsigned long					:2;
		
		unsigned long	PHSZ1			:14;
		unsigned long					:2;
	}bit;
};

/* structure of TDL_PVSZ			(001Ch) */
union ipc_iip_tdl_pvsz{
	unsigned long		word;
	struct{
		unsigned long	PVSZ0			:14;
		unsigned long					:2;
		
		unsigned long	PVSZ1			:14;
		unsigned long					:2;
	}bit;
};

/* structure of TDL_PF				(0020h) */
union ipc_iip_tdl_pf{
	unsigned long		word;
	struct{
		unsigned long	PF_ON			:1;
		unsigned long					:1;
		unsigned long	PF_MD			:1;
		unsigned long					:1;
		unsigned long	PF_NUM			:6;
		unsigned long					:2;
		unsigned long	PF_ADJ_NUM		:4;
		
		unsigned long	PF_ADJ_VAL		:16;
	}bit;
};

/* structure of UPLK_TARGET			(022Ch) */
union ipc_iip_spf_uplk_target{
	unsigned long		word;
	struct{
		unsigned long	UPSU0			:4;
		unsigned long					:4;
		unsigned long	UPSU1			:4;
		unsigned long					:4;
		
		unsigned long	UPSU2			:4;
		unsigned long					:4;
		unsigned long					:4;
		unsigned long					:4;
	}bit;
};

/* structure of DWLK_TARGET			(0230h) */
union ipc_iip_spf_dwlk_target{
	unsigned long		word;
	struct{
		unsigned long	DWSU0			:4;
		unsigned long					:4;
		unsigned long	DWSU1			:4;
		unsigned long					:4;
		
		unsigned long	DWSU2			:4;
		unsigned long					:4;
		unsigned long					:4;
		unsigned long					:4;
	}bit;
};

/* structure of LKNUM				(0234h) */
union ipc_iip_spf_lknum{
	unsigned long		word;
	struct{
		unsigned long	UPNUM			:2;
		unsigned long					:2;
		unsigned long	DWNUM			:2;
		unsigned long					:10;
		
		unsigned long					:16;
	}bit;
};

/* structure of RING				(0238h) */
union ipc_iip_spf_ring{
	unsigned long		word;
	struct{
		unsigned long	URINGSIZE		:8;
		unsigned long	UMARGINIYSZ		:4;
		unsigned long					:4;
		
		unsigned long	DRINGSIZE		:8;
		unsigned long					:8;
	}bit;
};

/* structure of UNIT_OUT			(0024h) */
union ipc_iip_unit_out{
	unsigned long		word;
	struct{
		unsigned long	UNIT_OUT_A_D0	:3;
		unsigned long					:1;
		unsigned long	UNIT_OUT_A_D1	:3;
		unsigned long					:1;
		unsigned long	UNIT_OUT_A_D2	:3;
		unsigned long					:5;
		
		unsigned long	UNIT_OUT_B_D0	:3;
		unsigned long					:1;
		unsigned long	UNIT_OUT_B_D1	:3;
		unsigned long					:1;
		unsigned long	UNIT_OUT_B_D2	:3;
		unsigned long					:5;
	}bit;
};

/* structure of FIN_CLP_A			(0028h - 0033h) */
/* structure of FIN_CLP_B			(0034h - 003Fh) */
union ipc_iip_fin_clp{
	unsigned long		word;
	struct{
		  signed long	FIN_CLVLP		:15;
		unsigned long					:1;
		
		  signed long	FIN_CLVLM		:15;
		unsigned long					:1;
	}bit;
};

/* structure of ISO_MD				(0040h) */
union ipc_iip_iso_md{
	unsigned long		word;
	struct{
		unsigned long	ISO_JM			:1;
		unsigned long					:15;
		
		unsigned long	ISO_CM			:1;
		unsigned long					:15;
	}bit;
};

/* structure of ISO_TH_D0			(0044h) */
/* structure of ISO_TH_D1			(0048h) */
/* structure of ISO_TH_D2			(004Ch) */
/* structure of ISO_TH_D3			(0050h) */
union ipc_iip_iso_th{
	unsigned long		word;
	struct{
		unsigned long	ISO_UTH			:15;
		unsigned long					:1;
		
		unsigned long	ISO_LTH			:15;
		unsigned long					:1;
	}bit;
};

/* structure of ISO_GAIN_D0			(0054h) */
/* structure of ISO_GAIN_D1			(0058h) */
/* structure of ISO_GAIN_D2			(005Ch) */
/* structure of ISO_GAIN_D3			(0060h) */
union ipc_iip_iso_gain{
	unsigned long		word;
	struct{
		unsigned long	ISO_GA			:12;
		unsigned long					:4;
		
		unsigned long	ISO_GB			:12;
		unsigned long					:4;
	}bit;
};

/* structure of SPF_MD				(0064h) */
union ipc_iip_spf_md{
	unsigned long		word;
	struct{
		unsigned long	SPF_MD			:2;
		unsigned long					:14;
		
		unsigned long					:16;
	}bit;
};

/* structure of SPF_FLTW_D0			(0080h - 00A1h) */
/* structure of SPF_FLTW_D1			(00C0h - 00E1h) */
/* structure of SPF_FLTW_D2			(0100h - 0121h) */
/* structure of SPF_FLTW_D3			(0140h - 0161h) */
union ipc_iip_spf_fltw{
	signed short		hword;
	struct{
		  signed short	SPF_FLTW		:13;
		unsigned short					:3;
	}bit;
};

/* structure of SPF_VTH_D0			(0180h) */
/* structure of SPF_VTH_D1			(0184h) */
/* structure of SPF_VTH_D2			(0188h) */
union ipc_iip_spf_vth{
	unsigned long		word;
	struct{
		unsigned long	SPF_UTH			:15;				/* D1,D2 9bit */
		unsigned long					:1;
		
		unsigned long	SPF_LTH			:15;
		unsigned long					:1;
	}bit;
};

/* structure of SPF_CORE_D0			(018Ch) */
/* structure of SPF_CORE_D1			(0190h) */
/* structure of SPF_CORE_D2			(0194h) */
union ipc_iip_spf_core{
	unsigned long		word;
	struct{
		unsigned long	SPF_CTHP		:14;				/* D1,D2 8bit */
		unsigned long					:2;
		
		unsigned long	SPF_CTHM		:14;
		unsigned long					:2;
	}bit;
};

/* structure of SPF_CLP_D0			(0198h) */
/* structure of SPF_CLP_D1			(019Ch) */
/* structure of SPF_CLP_D2			(01A0h) */
union ipc_iip_spf_clp{
	unsigned long		word;
	struct{
		  signed long	SPF_CLVLP		:15;				/* D1,D2 9bit */
		unsigned long					:1;
		
		  signed long	SPF_CLVLM		:15;
		unsigned long					:1;
	}bit;
};

/* structure of SPF_EOADD_D0		(01A4h) */
/* structure of SPF_EOADD_D1		(01A8h) */
/* structure of SPF_EOADD_D2		(01ACh) */
union ipc_iip_spf_eoadd{
	unsigned long		word;
	struct{
		unsigned long	SPF_EOADD		:14;				/* D1,D2 8bit */
		unsigned long					:2;
		
		unsigned long					:16;
	}bit;
};

/* structure of EP_MD				(01B0h) */
union ipc_iip_ep_md{
	unsigned long		word;
	struct{
		unsigned long	EP_MD			:2;
		unsigned long					:14;
		
		unsigned long					:16;
	}bit;
};

/* structure of EP_GA_D0			(01B8h - 01BDh) */
/* structure of EP_GA_D1			(01C0h - 01C5h) */
/* structure of EP_GA_D2			(01C8h - 01CDh) */
/* structure of EP_GA_D3			(01D0h - 01D5h) */
union ipc_iip_ep_ga{
	signed short		hword[4];
	struct{
		  signed short	EP_GA_0			:11;
		unsigned short					:5;
		
		  signed short	EP_GA_1			:11;
		unsigned short					:5;
		
		  signed short	EP_GA_2			:11;
		unsigned short					:5;
		
		unsigned short					:11;
		unsigned short					:5;
	}bit;
};

/* structure of EP_OFS_D0			(01D8h - 01DDh) */
/* structure of EP_OFS_D1			(01E0h - 01E5h) */
/* structure of EP_OFS_D2			(01E8h - 01EDh) */
/* structure of EP_OFS_D3			(01F0h - 01F5h) */
union ipc_iip_ep_ofs{
	signed short		hword[4];
	struct{
		  signed short	EP_OFS_0		:15;
		unsigned short					:1;
		
		  signed short	EP_OFS_1		:15;
		unsigned short					:1;
		
		  signed short	EP_OFS_2		:15;
		unsigned short					:1;
		
		unsigned short					:15;
		unsigned short					:1;
	}bit;
};

/* structure of EP_BD_D0			(01F8h - 01FDh) */
/* structure of EP_BD_D1			(0200h - 0205h) */
/* structure of EP_BD_D2			(0208h - 020Dh) */
/* structure of EP_BD_D3			(0210h - 0215h) */
union ipc_iip_ep_bd{
	signed short		hword[4];
	struct{
		  signed short	EP_BD_0			:15;
		unsigned short					:1;
		
		  signed short	EP_BD_1			:15;
		unsigned short					:1;
		
		  signed short	EP_BD_2			:15;
		unsigned short					:1;
		
		unsigned short					:15;
		unsigned short					:1;
	}bit;
};

/* structure of ED_FM				(0218h) */
union ipc_iip_ed_fm{
	unsigned long		word;
	struct{
		unsigned long	ED_FM0			:1;
		unsigned long					:7;
		unsigned long	ED_FM1			:1;
		unsigned long					:7;
		
		unsigned long	ED_FM2			:1;
		unsigned long					:7;
		unsigned long	ED_FM3			:1;
		unsigned long					:7;
	}bit;
};

/* structure of ED_OUT				(021Ch) */
union ipc_iip_ed_out{
	unsigned long		word;
	struct{
		unsigned long	ED_OUT0			:1;
		unsigned long					:7;
		unsigned long	ED_OUT1			:1;
		unsigned long					:7;
		
		unsigned long	ED_OUT2			:1;
		unsigned long					:7;
		unsigned long	ED_OUT3			:1;
		unsigned long					:7;
	}bit;
};

/* structure of ADAPT_K				(0220h) */
union ipc_iip_adapt_k{
	unsigned long		word;
	struct{
		  signed long	ADAPT_K0		:6;
		unsigned long					:2;
		  signed long	ADAPT_K1		:6;
		unsigned long					:2;
		
		  signed long	ADAPT_K2		:6;
		unsigned long					:10;
	}bit;
};

/* structure of ADAPT_TH			(0224h) */
union ipc_iip_adapt_th{
	unsigned long		word;
	struct{
		unsigned long	ADAPT_TH		:10;
		unsigned long					:6;
		
		unsigned long					:16;
	}bit;
};

/* structure of ADAPT_DIV			(0228h) */
union ipc_iip_adapt_div{
	unsigned long		word;
	struct{
		unsigned long	ADAPT_DIV		:3;
		unsigned long					:13;
		
		unsigned long					:16;
	}bit;
};

/* structure of BF_MD				(023Ch) */
union ipc_iip_bf_md{
	unsigned long		word;
	struct{
		unsigned long	BF_FMT			:1;
		unsigned long					:3;
		unsigned long	BF_PRT			:1;
		
		unsigned long					:16;
	}bit;
};



/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;

	union ipc_iip_tdltopcnf				TDLTOPCONF;			/* 0000 - 0003h */
	union ipc_iip_flt_pixiddef			PIXIDDEF;			/* 0004 - 0007h */
	union ipc_iip_tdl_ctl				TDL_CTL;			/* 0008 - 000Bh */
	union ipc_iip_calc_md				CALC_MD;			/* 000C - 000Fh */
	union ipc_iip_tdl_hsize				TDL_HSIZE;			/* 0010 - 0013h */
	union ipc_iip_tdl_vsize				TDL_VSIZE;			/* 0014 - 0017h */
	union ipc_iip_tdl_phsz				TDL_PHSZ;			/* 0018 - 001Bh */
	union ipc_iip_tdl_pvsz				TDL_PVSZ;			/* 001C - 001Fh */
	union ipc_iip_tdl_pf				TDL_PF;				/* 0020 - 0023h */
	union ipc_iip_unit_out				UNIT_OUT;			/* 0024 - 0027h */
	union ipc_iip_fin_clp				FIN_CLP_A[3];		/* 0028 - 0033h */
	union ipc_iip_fin_clp				FIN_CLP_B[3];		/* 0034 - 003Fh */
	union ipc_iip_iso_md				ISO_MD;				/* 0040 - 0043h */
	union ipc_iip_iso_th				ISO_TH[4];			/* 0044 - 0053h */
	union ipc_iip_iso_gain				ISO_GAIN[4];		/* 0054 - 0063h */
	union ipc_iip_spf_md				SPF_MD;				/* 0064 - 0067h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_flt_0068_007F[0x007F-0x0068+1];		/* 0068 - 007Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
		
	union ipc_iip_spf_fltw				SPF_FLTW_D0[17];	/* 0080 - 00A1h */
	unsigned char dumy_flt_00A2_00A3[0x00A3-0x00A2+1];		/* 00A2 - 00A3h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_flt_00A4_00BF[0x00BF-0x00A4+1];		/* 00A4 - 00BFh */
#else
	union ipc_iip_packet_header			header2;
#endif
	
	union ipc_iip_spf_fltw				SPF_FLTW_D1[17];	/* 00C0 - 00E1h */
	unsigned char dumy_flt_00E2_00E3[0x00E3-0x00E2+1];		/* 00E2 - 00E3h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_flt_00E4_00FF[0x00FF-0x00E4+1];		/* 00E4 - 00FFh */
#else
	union ipc_iip_packet_header			header3;
#endif
	
	union ipc_iip_spf_fltw				SPF_FLTW_D2[17];	/* 0100 - 0121h */
	unsigned char dumy_flt_0122_0123[0x0123-0x0122+1];		/* 0122 - 0123h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_flt_0124_013F[0x013F-0x0124+1];		/* 0124 - 013Fh */
#else
	union ipc_iip_packet_header			header4;
#endif
	
	union ipc_iip_spf_fltw				SPF_FLTW_D3[17];	/* 0140 - 0161h */
	unsigned char dumy_flt_0162_0163[0x0163-0x0162+1];		/* 0162 - 0163h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_flt_0164_017F[0x017F-0x0164+1];		/* 0164 - 017Fh */
#else
	union ipc_iip_packet_header			header5;
#endif
	
	union ipc_iip_spf_vth				SPF_VTH[3];			/* 0180 - 018Bh */
	union ipc_iip_spf_core				SPF_CORE[3];		/* 018C - 0197h */
	union ipc_iip_spf_clp				SPF_CLP[3];			/* 0198 - 01A3h */
	union ipc_iip_spf_eoadd				SPF_EOADD[3];		/* 01A4 - 01AFh */
	union ipc_iip_ep_md					EP_MD;				/* 01B0 - 01B3h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_flt_01B4_01B7[0x01B7-0x01B4+1];		/* 01B4 - 01B7h */
#else
	union ipc_iip_packet_header			header6;
#endif
	
	union ipc_iip_ep_ga					EP_GA[4];			/* 01B8 - 01D7h */
	union ipc_iip_ep_ofs				EP_OFS[4];			/* 01D8 - 01F7h */
	union ipc_iip_ep_bd					EP_BD[4];			/* 01F8 - 0217h */
	union ipc_iip_ed_fm					ED_FM;				/* 0218 - 021Bh */
	union ipc_iip_ed_out				ED_OUT;				/* 021C - 021Fh */
	union ipc_iip_adapt_k				ADAPT_K;			/* 0220 - 0223h */
	union ipc_iip_adapt_th				ADAPT_TH;			/* 0224 - 0227h */
	union ipc_iip_adapt_div				ADAPT_DIV;			/* 0227 - 022Bh */
	union ipc_iip_spf_uplk_target		UPLK_TARGET;		/* 022C - 022Fh */
	union ipc_iip_spf_dwlk_target		DWLK_TARGET;		/* 0230 - 0233h */
	union ipc_iip_spf_lknum				LKNUM;				/* 0234 - 0237h */
	union ipc_iip_spf_ring				RING;				/* 0238 - 023Bh */
	union ipc_iip_bf_md					BF_MD;				/* 023C - 023Fh */
	
} T_IPC_IIP_UNIT_FLT;



#define D_IPC_IIP_FLT_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGADR_0					(0x0000)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGADR_1					(0x0080)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGADR_2					(0x00C0)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGADR_3					(0x0100)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGADR_4					(0x0140)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGADR_5					(0x0180)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGADR_6					(0x01B8)

#define D_IPC_IIP_FLT_PARAM_LARGE_PACKET_REGSIZ				((0x0240>>2)+1)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGSIZ_0					(((0x0068-0x0000)>>2)+1)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGSIZ_1					(((0x00A4-0x0080)>>2)+1)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGSIZ_2					(((0x00E4-0x00C0)>>2)+1)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGSIZ_3					(((0x0124-0x0100)>>2)+1)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGSIZ_4					(((0x0164-0x0140)>>2)+1)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGSIZ_5					(((0x01B4-0x0180)>>2)+1)
#define D_IPC_IIP_FLT_PARAM_PACKET_REGSIZ_6					(((0x0240-0x01B8)>>2)+1)

