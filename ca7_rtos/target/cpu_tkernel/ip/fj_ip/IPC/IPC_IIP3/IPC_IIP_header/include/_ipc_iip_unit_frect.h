/**************************************************************************/
/*  <Contents>   Definition IIP FRECT UNIT register                       */
/*  <Filename>   _ipc_iip_unit_frect.h                                    */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
#include "_ipc_iip_packet_header.h"

/* structure of FRTOPCNF			(0000h) */
union ipc_iip_frtopcnf{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF		:6;
		unsigned long					:10;
		
		unsigned long	DATACONF		:6;
		unsigned long					:10;
	}bit;
};

/* structure of FRLDCNF				(0004h) */
union ipc_iip_frldcnf{
	unsigned long		word;
	struct{
		unsigned long					:3;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long					:1;
		unsigned long	DTHD			:1;
		unsigned long					:7;
		
		unsigned long					:16;
	}bit;
};

/* structure of FRCTL				(0008h) */
union ipc_iip_frctl{
	unsigned long		word;
	struct{
		unsigned long	GHSZ			:1;
		unsigned long					:3;
		unsigned long	GVSZ			:4;
		unsigned long	LPLVL			:1;
		unsigned long					:7;
		
		unsigned long	HDBL			:1;
		unsigned long	VDBL			:1;
		unsigned long	HSMD			:1;
		unsigned long					:13;
	}bit;
};

/* structure of PFCTL				(000Ch) */
union ipc_iip_frect_pfctl{
	unsigned long		word;
	struct{
		unsigned long	PFEXTMC			:3;
		unsigned long					:5;
		unsigned long	PFEXTADC		:8;
		
		unsigned long	PFSP1			:3;
		unsigned long					:1;
		unsigned long	PFSP2			:3;
		unsigned long					:1;
		unsigned long	PFVSP			:2;
		unsigned long					:2;
		unsigned long	PFOFF			:1;
		unsigned long					:3;
	}bit;
};

/* structure of PIXIDEF			(0010h) */
union ipc_iip_frect_pixidef{
	unsigned long		word;
	struct{
		unsigned long					:4;
		unsigned long	IPIXID			:4;
		unsigned long	EXA				:1;
		unsigned long					:3;
		unsigned long	CSEL			:2;
		unsigned long					:2;
		
		unsigned long					:16;
	}bit;
};

/* structure of OUTSIZE				(0058h) */
union ipc_iip_frect_outsize{
	unsigned long		word;
	struct{
		unsigned long	OUTHSZ			:14;
		unsigned long					:2;
		
		unsigned long	OUTVSZ			:14;
		unsigned long					:2;
	}bit;
};

/* structure of OUTSTARTPOS			(005Ch) */
union ipc_iip_frect_outstartpos{
	unsigned long		word;
	struct{
		unsigned long	STARTX			:14;
		unsigned long					:2;
		
		unsigned long	STARTY			:14;
		unsigned long					:2;
	}bit;
};

/* structure of CALMETHOD			(0014h) */
union ipc_iip_frect_calmethod{
	unsigned long		word;
	struct{
		unsigned long	OPCOL			:1;
		unsigned long					:3;
		unsigned long	ITMD			:2;
		unsigned long	_ARA			:2;
		unsigned long	FILMD			:1;
		unsigned long	DANTI			:1;
		unsigned long	AANTI			:1;
		unsigned long					:1;
		unsigned long	CUBSEL			:3;
		unsigned long	SCUB			:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of FILVALYG			(0018h) */
/* structure of FILVALB				(001Ah) */
/* structure of FILVALR				(001Ch) */
/* structure of FILVALA				(001Eh) */
union ipc_iip_frect_filval{
	unsigned short		hword;
	struct{
		unsigned short	FILVAL			:16;
	}bit;
};

/* structure of OPMD				(0020h) */
union ipc_iip_frect_opmd{
	unsigned long		word;
	struct{
		unsigned long	OPMD0			:3;
		unsigned long					:13;
		
		unsigned long					:16;
	}bit;
};

/* structure of OPY					(0030h) */
/* structure of OPB					(0034h) */
/* structure of OPR					(0038h) */
/* structure of OPA					(003Ch) */
union ipc_iip_frect_op{
	unsigned long		word;
	struct{
		unsigned long	OPB				:16;
		
		unsigned long	OPA				:8;
		unsigned long					:8;
	}bit;
};

/* structure of CLIPLVLY			(0040h) */
/* structure of CLIPLVLB			(0044h) */
/* structure of CLIPLVLR			(0048h) */
/* structure of CLIPLVLA			(004Ch) */
union ipc_iip_frect_cliplvl{
	unsigned long		word;
	struct{
		unsigned long	CLIPLVLH		:16;
		unsigned long	CLIPLVLL		:16;
	}bit;
};

/* structure of FRPCNT				(0050h) */
union ipc_iip_frpcnt{
	unsigned long		word;
	struct{
		unsigned long	FRPHCNT			:9;
		unsigned long					:7;
		
		unsigned long	FRPVCNT			:8;
		unsigned long					:8;
	}bit;
};

/* structure of PADRS				(0054h) */
union ipc_iip_padrs{
	unsigned long		word;
	struct{
		unsigned long	PADRS			:32;
	}bit;
};

/* structure of UPLK_TARGET			(0060h) */
union ipc_iip_frect_uplk_target{
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

/* structure of DWLK_TARGET			(0064h) */
union ipc_iip_frect_dwlk_target{
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

/* structure of LKNUM				(0068h) */
union ipc_iip_frect_lknum{
	unsigned long		word;
	struct{
		unsigned long	UPNUM			:2;
		unsigned long					:2;
		unsigned long	DWNUM			:2;
		unsigned long					:10;
		
		unsigned long					:16;
	}bit;
};

/* structure of RING				(006Ch) */
union ipc_iip_frect_ring{
	unsigned long		word;
	struct{
		unsigned long	URINGSIZE			:8;
		unsigned long	UMARGINIYSZ			:4;
		unsigned long						:4;
		
		unsigned long	DRINGSIZE			:8;
		unsigned long						:8;
	}bit;
};


/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_frtopcnf				FRTOPCNF;			/* 0000 - 0003h */
	union ipc_iip_frldcnf				FRLDCNF;			/* 0004 - 0007h */
	union ipc_iip_frctl					FRCTL;				/* 0008 - 000Bh */
	union ipc_iip_frect_pfctl			PFCTL;				/* 000C - 000Fh */
	union ipc_iip_frect_pixidef			PIXIDEF;			/* 0010 - 0013h */
	union ipc_iip_frect_calmethod		CALMETHOD;			/* 0014 - 0017h */
	union ipc_iip_frect_filval			FILVAL[4];			/* 0018 - 001Fh */
	union ipc_iip_frect_opmd			OPMD;				/* 0020 - 0023h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_frect_0024_002F[0x0030-0x0024];		/* 0024 - 002Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_frect_op				OP[4];				/* 0030 - 003Fh */
	union ipc_iip_frect_cliplvl			CLIPLVL[4];			/* 0040 - 004Fh */
	union ipc_iip_frpcnt				FRPCNT;				/* 0050 - 0053h */
	union ipc_iip_padrs					PADRS;				/* 0054 - 0057h */
	union ipc_iip_frect_outsize			OUTSIZE;			/* 0058 - 005Bh */
	union ipc_iip_frect_outstartpos		OUTSTARTPOS;		/* 005C - 005Fh */
	union ipc_iip_frect_uplk_target		UPLK_TARGET;		/* 0060 - 0063h */
	union ipc_iip_frect_dwlk_target		DWLK_TARGET;		/* 0064 - 0067h */
	union ipc_iip_frect_lknum			LKNUM;				/* 0068 - 006Bh */
	union ipc_iip_frect_ring			RING;				/* 006C - 006Fh */
	
} T_IPC_IIP_UNIT_FRECT;



#define D_IPC_IIP_FRECT_PARAM_LARGE_PACKET_REGADR			(0x0000)
#define D_IPC_IIP_FRECT_PARAM_PACKET_REGADR_0				(0x0000)
#define D_IPC_IIP_FRECT_PARAM_PACKET_REGADR_1				(0x0030)

#define D_IPC_IIP_FRECT_PARAM_LARGE_PACKET_REGSIZ			(((0x0070-0x0000)>>2)+1)
#define D_IPC_IIP_FRECT_PARAM_PACKET_REGSIZ_0				(((0x0024-0x0000)>>2)+1)
#define D_IPC_IIP_FRECT_PARAM_PACKET_REGSIZ_1				(((0x0070-0x0030)>>2)+1)

