/**************************************************************************/
/*  <Contents>   Definition IIP MFT UNIT register                         */
/*  <Filename>   _ipc_iip_unit1d.h                                        */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*               2014/07/16   H.Hata                                 1.10 */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
#include "_ipc_iip_packet_header.h"

/* structure of MFTTOPCNF				(0000h) */
union ipc_iip_mfttopcnf{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF		:6;
		unsigned long					:10;
		
		unsigned long	DATACONF		:6;
		unsigned long					:10;
	}bit;
};

/* structure of MFTMD				(0004h) */
union ipc_iip_mftmd{
	unsigned long		word;
	struct{
		unsigned long	MFTMD			:1;
		unsigned long					:15;
		
		unsigned long					:16;
	}bit;
};

/* structure of FLTTPSZ				(0008h) */
union ipc_iip_flttpsz{
	unsigned long		word;
	struct{
		unsigned long	FLTTPSZ			:11;
		unsigned long					:5;
		
		unsigned long					:16;
	}bit;
};

/* structure of FLTOUTHSZ			(000Ch) */
union ipc_iip_fltouthsz{
	unsigned long		word;
	struct{
		unsigned long	FLTOUTHSZ		:15;
		unsigned long					:1;
		
		unsigned long					:16;
	}bit;
};

/* structure of MFTCTLD0			(0010h) */
/* structure of MFTCTLD1			(0014h) */
/* structure of MFTCTLD2			(0018h) */
/* structure of MFTCTLD3			(001Ch) */
union ipc_iip_mftctl{
	unsigned long		word;
	struct{
		unsigned long	FIX				:1;					/* D3 None */
		unsigned long					:15;
		
		  signed long	VAL				:15;				/* D3 9bit */
		unsigned long					:1;
	}bit;
};



/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_mfttopcnf				MFTTOPCNF;			/* 0000 - 0003h */
	union ipc_iip_mftmd					MFTMD;				/* 0004 - 0007h */
	union ipc_iip_flttpsz				FLTTPSZ;			/* 0008 - 000Bh */
	union ipc_iip_fltouthsz				FLTOUTHSZ;			/* 000C - 000Fh */
	union ipc_iip_mftctl				MFTCTL[4];			/* 0010 - 001Fh */
	
} T_IPC_IIP_UNIT_MFT;



#define D_IPC_IIP_MFT_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_MFT_PARAM_PACKET_REGADR_0					(0x0000)

#define D_IPC_IIP_MFT_PARAM_LARGE_PACKET_REGSIZ				(((0x0020-0x0000)>>2)+1)
#define D_IPC_IIP_MFT_PARAM_PACKET_REGSIZ_0					(((0x0020-0x0000)>>2)+1)

