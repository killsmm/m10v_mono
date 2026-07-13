/**************************************************************************/
/*  <Contents>   Definition IIP GPC UNIT register                         */
/*  <Filename>   _ipc_iip_unit1d.h                                        */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*               2014/08/16   H.Hata                                 1.30 */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
#include "_ipc_iip_packet_header.h"

/* structure of GPCTOPCNF0			(0000h) */
union ipc_iip_gpctopcnf0{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF_0		:6;
		unsigned long					:10;
		unsigned long	DATACONF_0		:6;
		unsigned long					:10;
	}bit;
};

/* structure of GPCTOPCNF1			(0004h) */
union ipc_iip_gpctopcnf1{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF_1		:6;
		unsigned long					:10;
		unsigned long	DATACONF_1		:6;
		unsigned long					:10;
	}bit;
};

/* structure of GPCCONTROL			(0010h) */
union ipc_iip_gpccontrol{
	unsigned long		word;
	struct{
		unsigned long	COMMONPARA		:2;
		unsigned long					:2;
		unsigned long	MD2DLK			:1;
		unsigned long					:3;
		unsigned long	ALPEN0			:1;
		unsigned long					:3;
		unsigned long	ALPEN1			:1;
		unsigned long					:3;
		
		unsigned long	ATRB0			:4;
		unsigned long	ATRB1			:4;
		unsigned long					:8;
	}bit;
};

/* structure of GPCCLOADADR			(0014h) */
union ipc_iip_gpccloadadr{
	unsigned long		word;
	struct{
		unsigned long	CLOADADR		:32;
	}bit;
};

/* structure of GPCMON				(0018h) */
union ipc_iip_gpcmon{
	unsigned long		word;
	struct{
		unsigned long	GPCRUN			:1;
		unsigned long					:3;
		unsigned long	SEQERR			:1;
		unsigned long					:11;
		
		unsigned long	MONPAD			:12;
		unsigned long	MONSEQ			:1;
		unsigned long	MONLPA			:1;
		unsigned long					:2;
	}bit;
};


/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_gpctopcnf0			GPCTOPCONF0;		/* 0000 - 0003h */
	union ipc_iip_gpctopcnf1			GPCTOPCONF1;		/* 0004 - 0007h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_gpc_0008_000F[0x0010-0x0008];		/* 0008 - 000Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_gpccontrol			GPCCONTROL;			/* 0010 - 0013h */
	union ipc_iip_gpccloadadr			GPCCLOADADR;		/* 0014 - 0017h */
	union ipc_iip_gpcmon				GPCMON;				/* 0018 - 001Bh */
	
} T_IPC_IIP_UNIT_GPC;



#define D_IPC_IIP_GPC_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_GPC_PARAM_PACKET_REGADR_0					(0x0000)
#define D_IPC_IIP_GPC_PARAM_PACKET_REGADR_1					(0x0010)

#define D_IPC_IIP_GPC_PARAM_LARGE_PACKET_REGSIZ				((0x001B>>2)+1)
#define D_IPC_IIP_GPC_PARAM_PACKET_REGSIZ_0					(((0x0008-0x0000)>>2)+1)
#define D_IPC_IIP_GPC_PARAM_PACKET_REGSIZ_1					(((0x001C-0x0010)>>2)+1)

