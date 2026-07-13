/**************************************************************************/
/*  <Contents>   Definition IIP CSC UNIT register                         */
/*  <Filename>   _ipc_iip_unit_csc.h                                      */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
#include "_ipc_iip_packet_header.h"

/* structure of CSCTOPCNF			(0000h) */
union ipc_iip_csctopcnf{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF		:6;
		unsigned long					:10;
		
		unsigned long	DATACONF		:6;
		unsigned long					:10;
	}bit;
};

/* structure of CSCK				(0020h - 003Fh) */
union ipc_iip_csck{
	signed short		hword;
	struct{
		  signed short	CSCK			:15;
		unsigned short					:1;
	}bit;
};

/* structure of MDSEL				(0040h) */
union ipc_iip_csc_mdsel{
	unsigned long		word;
	struct{
		unsigned long	MDSEL			:2;
		unsigned long					:14;
		
		unsigned long					:16;
	}bit;
};

/* structure of ALPSEL				(0044h) */
union ipc_iip_csc_alpsel{
	unsigned long		word;
	struct{
		unsigned long	ALPSEL			:1;
		unsigned long					:3;
		unsigned long	ALOSEL			:1;
		unsigned long					:11;
		
		unsigned long					:16;
	}bit;
};

/* structure of ALPVAL				(0048h) */
union ipc_iip_csc_alpval{
	signed long			word;
	struct{
		  signed long	ALPVAL			:15;
		unsigned long					:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of MAXMIN				(004Ch) */
union ipc_iip_csc_maxmin{
	unsigned long		word;
	struct{
		unsigned long	MAXMIN0			:2;
		unsigned long					:2;
		unsigned long	MAXMIN1			:2;
		unsigned long					:2;
		unsigned long	MAXMIN2			:2;
		unsigned long					:2;
		unsigned long	MAXMIN3			:2;
		unsigned long					:2;
		
		unsigned long					:16;
	}bit;
};

/* structure of OUTCLIP0			(0050h) */
/* structure of OUTCLIP1			(0054h) */
/* structure of OUTCLIP2			(0058h) */
/* structure of OUTCLIP3			(005Ch) */
union ipc_iip_csc_outclip{
	unsigned long		word;
	struct{
		  signed long	CLPMAX			:15;				/* D3 9bit */
		unsigned long					:1;
		
		  signed long	CLPMIN			:15;				/* D3 9bit */
		unsigned long					:1;
	}bit;
};



/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_csctopcnf				CSCTOPCNF;			/* 0000 - 0003h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_csc_0004_001F[0x001F-0x0004+1];		/* 0004 - 001Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_csck					CSCK[4][4];			/* 0020 - 003Fh */
	union ipc_iip_csc_mdsel				MDSEL;				/* 0040 - 0043h */
	union ipc_iip_csc_alpsel			ALPSEL;				/* 0044 - 0047h */
	union ipc_iip_csc_alpval			ALPVAL;				/* 0048 - 004Bh */
	union ipc_iip_csc_maxmin			MAXMIN;				/* 004C - 004Fh */
	union ipc_iip_csc_outclip			OUTCLIP[4];			/* 0050 - 005Fh */
	
} T_IPC_IIP_UNIT_CSC;



#define D_IPC_IIP_CSC_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_CSC_PARAM_PACKET_REGADR_0					(0x0000)
#define D_IPC_IIP_CSC_PARAM_PACKET_REGADR_1					(0x0020)

#define D_IPC_IIP_CSC_PARAM_LARGE_PACKET_REGSIZ				(((0x0060-0x0000)>>2)+1)
#define D_IPC_IIP_CSC_PARAM_PACKET_REGSIZ_0					(((0x0004-0x0000)>>2)+1)
#define D_IPC_IIP_CSC_PARAM_PACKET_REGSIZ_1					(((0x0060-0x0020)>>2)+1)

