/**************************************************************************/
/*  <Contents>   Definition IIP SL UNIT register                          */
/*  <Filename>   _ipc_iip_unit_sl.h                                       */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*               2014/08/05   H.Hata                                 1.10 */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
#include "_ipc_iip_packet_header.h"

/* structure of SL_TOPCNF0			(0000h) */
union ipc_iip_sl_topcnf0{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF		:6;
		unsigned long					:10;
		
		unsigned long	DATACONF		:6;
		unsigned long					:10;
	}bit;
};

/* structure of PIXIDDEF			(0004h) */
union ipc_iip_sl_pixiddef{
	unsigned long		word;
	struct{
		unsigned long	OPIXID			:4;
		unsigned long					:4;
		unsigned long	CSEL			:2;
		unsigned long					:6;
		
		unsigned long	DIV1ST			:1;
		unsigned long					:3;
		unsigned long	LKDIV			:2;
		unsigned long					:10;
	}bit;
};

/* structure of PLDUNIT_SL			(0008h -000Fh) */
union ipc_iip_sl_pldunit_sl{
	unsigned long		word[2];
	struct{
		unsigned long	PLDUNIT_SLL		:32;
		unsigned long	PLDUNIT_SLH		:32;
	}bit;
};

/* structure of SL_CTL				(0010h) */
union ipc_iip_sl_ctl{
	unsigned long		word;
	struct{
		unsigned long	SL_MODE			:3;
		unsigned long					:1;
		unsigned long	LOADMD			:1;
		unsigned long					:3;
		unsigned long	MASKEN			:2;
		unsigned long					:6;
		
		unsigned long					:16;
	}bit;
};

/* structure of OUTCOL				(0014h) */
union ipc_iip_sl_outcol{
	unsigned long		word;
	struct{
		unsigned long	OUTCOL			:4;
		unsigned long					:12;
		
		unsigned long					:16;
	}bit;
};

/* structure of SL_HSIZE			(0020h) */
union ipc_iip_sl_hsize{
	unsigned long		word;
	struct{
		unsigned long	HSTART			:14;
		unsigned long					:2;
		
		unsigned long	HEND			:14;
		unsigned long					:2;
	}bit;
};

/* structure of SL_VSIZE			(0024h) */
union ipc_iip_sl_vsize{
	unsigned long		word;
	struct{
		unsigned long	VSTART			:14;
		unsigned long					:2;
		
		unsigned long	VEND			:14;
		unsigned long					:2;
	}bit;
};

/* structure of SL_PHSZ				(0028h) */
union ipc_iip_sl_phsz{
	unsigned long		word;
	struct{
		unsigned long	PHSZ0			:14;
		unsigned long					:2;
		
		unsigned long	PHSZ1			:14;
		unsigned long					:2;
	}bit;
};

/* structure of SL_PVSZ				(002Ch) */
union ipc_iip_sl_pvsz{
	unsigned long		word;
	struct{
		unsigned long	PVSZ0			:14;
		unsigned long					:2;
		
		unsigned long	PVSZ1			:14;
		unsigned long					:2;
	}bit;
};

/* structure of SEL_OPARA			(0040h) */
union ipc_iip_sl_sel_opara{
	unsigned long		word;
	struct{
		unsigned long	SEL_OPARA		:4;
		unsigned long					:12;
		
		unsigned long					:16;
	}bit;
};

/* structure of OFSET_0				(0048h) */
/* structure of OFSET_1				(0064h) */
union ipc_iip_sl_ofset{
	signed long			word;
	struct{
		  signed long	OFSET			:15;
		unsigned long					:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of CLPTH_U_0			(004Ch) */
/* structure of CLPTH_U_1			(0068h) */
union ipc_iip_sl_clpth_u{
	signed long			word;
	struct{
		  signed long	CLPTH_U			:15;
		unsigned long					:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of CLPTH_L_0			(0050h) */
/* structure of CLPTH_L_1			(006Ch) */
union ipc_iip_sl_clpth_l{
	signed long			word;
	struct{
		  signed long	CLPTH_L			:15;
		unsigned long					:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of CLPVAL_U_0			(0054h) */
/* structure of CLPVAL_U_1			(0070h) */
union ipc_iip_sl_clpval_u{
	signed long			word;
	struct{
		  signed long	CLPVAL_U		:15;
		unsigned long					:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of CLPVAL_L_0			(0058h) */
/* structure of CLPVAL_L_1			(0074h) */
union ipc_iip_sl_clpval_l{
	signed long			word;
	struct{
		  signed long	CLPVAL_L		:15;
		unsigned long					:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of SFT_0				(005Ch) */
/* structure of SFT_1				(0078h) */
union ipc_iip_sl_sft{
	unsigned long		word;
	struct{
		unsigned long	SFTM			:1;
		unsigned long					:3;
		unsigned long	SFTVAL			:4;
		unsigned long					:8;
		
		unsigned long					:16;
	}bit;
};

/* structure of W14MD_0				(0060h) */
/* structure of W14MD_1				(007Ch) */
union ipc_iip_sl_w14md{
	unsigned long		word;
	struct{
		unsigned long	_W14LSFT		:2;
		unsigned long					:2;
		unsigned long	_W14SAT			:2;
		unsigned long					:10;
		
		unsigned long					:16;
	}bit;
};

/* structure of FILL_EN				(0200h) */
union ipc_iip_sl_fill_en{
	unsigned long		word;
	struct{
		unsigned long	FILL_EN			:20;
		unsigned long					:12;
	}bit;
};

/* structure of FILL_DATA				(0204h) */
union ipc_iip_sl_fill_data{
	unsigned long		word;
	struct{
		unsigned long	FILL_DATA0		:8;
		unsigned long	FILL_DATA1		:8;
		
		unsigned long	FILL_DATA2		:8;
		unsigned long					:8;
	}bit;
};

/* structure of FILL_SIZE				(0300h - 039Fh) */
union ipc_iip_sl_fill_size{
	unsigned long		word[2];
	struct{
		unsigned long	FILL_HSTA		:14;
		unsigned long					:2;
		
		unsigned long	FILL_HEND		:14;
		unsigned long					:2;
		
		unsigned long	FILL_VSTA		:14;
		unsigned long					:2;
		
		unsigned long	FILL_VEND		:14;
		unsigned long					:2;
	}bit;
};


/* structure of _OUTFMT0				(0048h - 0063h) */
/* structure of _OUTFMT1				(0064h - 007Fh) */
struct ipc_iip_sl_outfmt{
	union ipc_iip_sl_ofset				OFSET;				/* 0048 - 004Bh */
	union ipc_iip_sl_clpth_u			CLPTH_U;			/* 004C - 004Fh */
	union ipc_iip_sl_clpth_l			CLPTH_L;			/* 0050 - 0053h */
	union ipc_iip_sl_clpval_u			CLPVAL_U;			/* 0054 - 0057h */
	union ipc_iip_sl_clpval_l			CLPVAL_L;			/* 0058 - 005Bh */
	union ipc_iip_sl_sft				SFT;				/* 005C - 005Fh */
	union ipc_iip_sl_w14md				W14MD;				/* 0060 - 0063h */
	
};



/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_sl_topcnf0			SL_TOPCNF0;			/* 0000 - 0003h */
	union ipc_iip_sl_pixiddef			PIXIDDEF;			/* 0004 - 0007h */
	union ipc_iip_sl_pldunit_sl			PLDUNIT_SL;			/* 0008 - 000Fh */
	union ipc_iip_sl_ctl				SL_CTL;				/* 0010 - 0013h */
	union ipc_iip_sl_outcol				OUTCOL;				/* 0014 - 0017h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_sl_0018_001F[0x001F-0x0018+1];		/* 0018 - 001Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_sl_hsize				SL_HSIZE;			/* 0020 - 0023h */
	union ipc_iip_sl_vsize				SL_VSIZE;			/* 0024 - 0027h */
	union ipc_iip_sl_phsz				SL_PHSZ;			/* 0028 - 002Bh */
	union ipc_iip_sl_pvsz				SL_PVSZ;			/* 002C - 002Fh */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_sl_0030_003F[0x003F-0x0030+1];		/* 0030 - 003Fh */
#else
	union ipc_iip_packet_header			header2;
#endif
	
	union ipc_iip_sl_sel_opara			SEL_OPARA;			/* 0040 - 0043h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_sl_0044_0047[0x0047-0x0044+1];		/* 0044 - 0047h */
#else
	union ipc_iip_packet_header			header3;
#endif
	
	struct ipc_iip_sl_outfmt			SL_OUTFMT[2];		/* 0048 - 007Fh */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_sl_0080_01FF[0x01FF-0x0080+1];		/* 0080 - 01FFh */
#else
	union ipc_iip_packet_header			header4;
#endif
	
	union ipc_iip_sl_fill_en			FILL_EN;			/* 0200 - 0203h */
	union ipc_iip_sl_fill_data			FILL_DATA;			/* 0204 - 0207h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_sl_0208_02FF[0x02FF-0x0208+1];		/* 0208 - 02FFh */
#else
	union ipc_iip_packet_header			header5;
#endif
	
	union ipc_iip_sl_fill_size			FILL_SIZE[20];		/* 0300 - 039Fh */
	
} T_IPC_IIP_UNIT_SL;



#define D_IPC_IIP_SL_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_SL_PARAM_PACKET_REGADR_0					(0x0000)
#define D_IPC_IIP_SL_PARAM_PACKET_REGADR_1					(0x0020)
#define D_IPC_IIP_SL_PARAM_PACKET_REGADR_2					(0x0040)
#define D_IPC_IIP_SL_PARAM_PACKET_REGADR_3					(0x0048)
#define D_IPC_IIP_SL_PARAM_PACKET_REGADR_4					(0x0200)
#define D_IPC_IIP_SL_PARAM_PACKET_REGADR_5					(0x0300)

#define D_IPC_IIP_SL_PARAM_LARGE_PACKET_REGSIZ				(((0x03A0-0x0000)>>2)+1)
#define D_IPC_IIP_SL_PARAM_PACKET_REGSIZ_0					(((0x0018-0x0000)>>2)+1)
#define D_IPC_IIP_SL_PARAM_PACKET_REGSIZ_1					(((0x0030-0x0020)>>2)+1)
#define D_IPC_IIP_SL_PARAM_PACKET_REGSIZ_2					(((0x0044-0x0040)>>2)+1)
#define D_IPC_IIP_SL_PARAM_PACKET_REGSIZ_3					(((0x0080-0x0048)>>2)+1)
#define D_IPC_IIP_SL_PARAM_PACKET_REGSIZ_4					(((0x0208-0x0200)>>2)+1)
#define D_IPC_IIP_SL_PARAM_PACKET_REGSIZ_5					(((0x03A0-0x0300)>>2)+1)
