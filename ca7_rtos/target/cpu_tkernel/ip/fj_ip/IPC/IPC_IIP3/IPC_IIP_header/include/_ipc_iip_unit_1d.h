/**************************************************************************/
/*  <Contents>   Definition IIP 1D UNIT register                          */
/*  <Filename>   _ipc_iip_unit1d.h                                        */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
#include "_ipc_iip_packet_header.h"

/* structure of LD_TOPCNF			(0004h) */
union ipc_iip_ld_topcnf0{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF		:6;
		unsigned long					:10;
		
		unsigned long					:16;
	}bit;
};

/* structure of LD_TOPCNF1			(0008h) */
union ipc_iip_ld_topcnf1{
	unsigned long		word;
	struct{
		unsigned long	LINK_UID		:6;
		unsigned long					:10;
		
		unsigned long					:16;
	}bit;
};

/* structure of PIXIDDEF			(0010h) */
union ipc_iip_1d_pixiddef{
	unsigned long		word;
	struct{
		unsigned long					:4;
		unsigned long	IPIXID			:4;
		unsigned long					:8;
		
		unsigned long					:16;
	}bit;
};

/* structure of LD_CTL				(0014h) */
union ipc_iip_ld_ctl{
	unsigned long		word;
	struct{
		unsigned long	LD_MODE			:2;
		unsigned long					:2;
		unsigned long	LD_OUT			:2;
		unsigned long					:2;
		unsigned long	SLOW			:1;
		unsigned long					:3;
		unsigned long	SAVERD			:2;
		unsigned long					:2;
		
		unsigned long	CSEL			:2;
		unsigned long					:14;
	}bit;
};

/* structure of LD_PREFETCH			(0018h) */
union ipc_iip_ld_prefetch{
	unsigned long		word;
	struct{
		unsigned long	PFOFF			:1;
		unsigned long	PF_PDIST		:2;
		unsigned long	PF_1ST			:1;
		unsigned long					:12;
		
		unsigned long	PFB_NUM			:5;
		unsigned long					:3;
		unsigned long	PF_ADD			:5;
		unsigned long					:3;
	}bit;
};

/* structure of LD_HSIZE			(0020h) */
union ipc_iip_ld_hsize{
	unsigned long		word;
	struct{
		unsigned long	HSTART			:14;
		unsigned long					:2;
		
		unsigned long	HEND			:14;
		unsigned long					:2;
	}bit;
};

/* structure of LD_VSIZE			(0024h) */
union ipc_iip_ld_vsize{
	unsigned long		word;
	struct{
		unsigned long	VSTART			:14;
		unsigned long					:2;
		
		unsigned long	VEND			:14;
		unsigned long					:2;
	}bit;
};

/* structure of LD_PHSZ				(0028h) */
union ipc_iip_ld_phsz{
	unsigned long		word;
	struct{
		unsigned long	PHSZ0			:14;
		unsigned long					:2;
		
		unsigned long	PHSZ1			:14;
		unsigned long					:2;
	}bit;
};

/* structure of LD_PVSZ				(002Ch) */
union ipc_iip_ld_pvsz{
	unsigned long		word;
	struct{
		unsigned long	PVSZ0			:14;
		unsigned long					:2;
		
		unsigned long	PVSZ1			:14;
		unsigned long					:2;
	}bit;
};

/* structure of UPLK_TARGET			(0030h) */
union ipc_iip_ld_uplk_target{
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

/* structure of DWLK_TARGET			(0034h) */
union ipc_iip_ld_dwlk_target{
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

/* structure of LKNUM				(0038h) */
union ipc_iip_ld_lknum{
	unsigned long		word;
	struct{
		unsigned long	UPNUM			:2;
		unsigned long					:2;
		unsigned long	DWNUM			:2;
		unsigned long					:10;
		
		unsigned long					:16;
	}bit;
};

/* structure of RING				(003Ch) */
union ipc_iip_ld_ring{
	unsigned long		word;
	struct{
		unsigned long					:8;
		unsigned long	UMARGINIYSZ		:4;
		unsigned long					:4;
		
		unsigned long	DRINGSIZE		:8;
		unsigned long					:8;
	}bit;
};



/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_1d_0000_0003[0x0003-0x0000+1];		/* 0000 - 0003h */
#endif
	
	union ipc_iip_ld_topcnf0			LD_TOPCONF0;		/* 0004 - 0007h */
	union ipc_iip_ld_topcnf1			LD_TOPCONF1;		/* 0008 - 000Bh */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_1d_000C_000F[0x000F-0x000C+1];		/* 000C - 000Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_1d_pixiddef			PIXIDDEF;			/* 0010 - 0013h */
	union ipc_iip_ld_ctl				LD_CTL;				/* 0014 - 0017h */
	union ipc_iip_ld_prefetch			LD_PREFETCH;		/* 0018 - 001Bh */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_1d_001C_001F[0x001F-0x001C+1];		/* 001C - 001Fh */
#else
	union ipc_iip_packet_header			header2;
#endif
	
	union ipc_iip_ld_hsize				LD_HSIZE;			/* 0020 - 0023h */
	union ipc_iip_ld_vsize				LD_VSIZE;			/* 0024 - 0027h */
	union ipc_iip_ld_phsz				LD_PHSZ;			/* 0028 - 002Bh */
	union ipc_iip_ld_pvsz				LD_PVSZ;			/* 002C - 002Fh */
	union ipc_iip_ld_uplk_target		UPLK_TARGET;		/* 0030 - 0033h */
	union ipc_iip_ld_dwlk_target		DWLK_TARGET;		/* 0034 - 0037h */
	union ipc_iip_ld_lknum				LKNUM;				/* 0038 - 003Bh */
	union ipc_iip_ld_ring				RING;				/* 003C - 003Fh */
	
} T_IPC_IIP_UNIT_1D;



#define D_IPC_IIP_1D_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_1D_PARAM_PACKET_REGADR_0					(0x0004)
#define D_IPC_IIP_1D_PARAM_PACKET_REGADR_1					(0x0010)
#define D_IPC_IIP_1D_PARAM_PACKET_REGADR_2					(0x0020)

#define D_IPC_IIP_1D_PARAM_LARGE_PACKET_REGSIZ				(((0x0040-0x0000)>>2)+1)
#define D_IPC_IIP_1D_PARAM_PACKET_REGSIZ_0					(((0x000C-0x0004)>>2)+1)
#define D_IPC_IIP_1D_PARAM_PACKET_REGSIZ_1					(((0x001C-0x0010)>>2)+1)
#define D_IPC_IIP_1D_PARAM_PACKET_REGSIZ_2					(((0x0040-0x0020)>>2)+1)

