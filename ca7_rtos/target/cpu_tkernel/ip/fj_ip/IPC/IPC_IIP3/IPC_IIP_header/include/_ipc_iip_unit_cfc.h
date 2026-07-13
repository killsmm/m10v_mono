/**************************************************************************/
/*  <Contents>   Definition IIP CFC UNIT register                         */
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

/* structure of CFTOPCNF			(0000h) */
union ipc_iip_cftopconf{
	unsigned long		word;
	struct{
		unsigned long	DATACONF_0		:6;
		unsigned long					:2;
		unsigned long	DATACONF_1		:6;
		unsigned long					:2;
		
		unsigned long					:16;
	}bit;
};

/* structure of DIFF_PREWAIT		(0010h) */
union ipc_iip_diff_prewait{
	unsigned long		word;
	struct{
		unsigned long	DIFF_PREWAIT	:19;
		unsigned long					:13;
	}bit;
};

/* structure of DIFF_POSTWAIT		(0014h) */
union ipc_iip_diff_postwait{
	unsigned long		word;
	struct{
		unsigned long	DIFF_POSTWAIT	:19;
		unsigned long					:13;
	}bit;
};

/* structure of MAGPOW				(0018h) */
union ipc_iip_magpow{
	unsigned long		word;
	struct{
		unsigned long	PREMAG			:7;
		unsigned long					:1;
		unsigned long	POSTMAG			:7;
		unsigned long					:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of CFMODE				(001Ch) */
union ipc_iip_cfmode{
	unsigned long		word;
	struct{
		unsigned long	SPC_ON			:1;
		unsigned long					:15;
		
		unsigned long					:16;
	}bit;
};



/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_cftopconf				CFTOPCONF;			/* 0000 - 0003h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_cfc_0004_000F[0x000F-0x0004+1];		/* 0004 - 000Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_diff_prewait			DIFF_PREWAIT;		/* 0010 - 0013h */
	union ipc_iip_diff_postwait			DIFF_POSTWAIT;		/* 0014 - 0017h */
	union ipc_iip_magpow				MAGPOW;				/* 0018 - 001Bh */
	union ipc_iip_cfmode				CFMODE;				/* 001C - 001Fh */
	
} T_IPC_IIP_UNIT_CFC;



#define D_IPC_IIP_CFC_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_CFC_PARAM_PACKET_REGADR_0					(0x0000)
#define D_IPC_IIP_CFC_PARAM_PACKET_REGADR_1					(0x0010)

#define D_IPC_IIP_CFC_PARAM_LARGE_PACKET_REGSIZ				(((0x0020-0x0000)>>2)+1)
#define D_IPC_IIP_CFC_PARAM_PACKET_REGSIZ_0					(((0x0004-0x0000)>>2)+1)
#define D_IPC_IIP_CFC_PARAM_PACKET_REGSIZ_1					(((0x0020-0x0010)>>2)+1)

