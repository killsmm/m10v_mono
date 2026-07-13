/**************************************************************************/
/*  <Contents>   Definition IIP MON UNIT register                         */
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

/* structure of MONCTL				(0000h) */
union ipc_iip_monctl{
	unsigned long		word;
	struct{
		unsigned long	HISTAE			:1;
		unsigned long					:3;
		unsigned long	HISTBE			:1;
		unsigned long					:3;
		unsigned long	HISTCE			:1;
		unsigned long					:3;
		unsigned long	HISTDE			:1;
		unsigned long					:3;
		
		unsigned long	MAXE			:1;
		unsigned long					:3;
		unsigned long	MINE			:1;
		unsigned long					:3;
		unsigned long	ADDE			:1;
		unsigned long					:7;
	}bit;
};

/* structure of MONDSEL				(0004h) */
union ipc_iip_mondsel{
	unsigned long		word;
	struct{
		unsigned long	DSEL			:3;
		unsigned long					:1;
		unsigned long	SGNEN0			:1;
		unsigned long	SGNEN1			:1;
		unsigned long	SGNEN2			:1;
		unsigned long	SGNEN3			:1;
		unsigned long					:8;
		
		unsigned long					:16;
	}bit;
};

/* structure of HISTCTL0			(0010h) */
/* structure of HISTCTL1			(0014h) */
/* structure of HISTCTL2			(0018h) */
/* structure of HISTCTL3			(001Ch) */
union ipc_iip_histctl{
	unsigned long		word;
	struct{
		unsigned long	HISTSEL			:2;
		unsigned long					:2;
		unsigned long	BITSEL			:3;
		unsigned long					:1;
		unsigned long	HISTSKIP_H		:6;
		unsigned long					:2;
		
		unsigned long	HISTSKIP_V		:6;
		unsigned long					:10;
	}bit;
};

/* structure of ABSMAX				(0020h) */
union ipc_iip_absmax{
	unsigned long		word;
	struct{
		unsigned long	ABSMAX			:1;
		unsigned long					:15;
		
		unsigned long					:16;
	}bit;
};

/* structure of ABSMIN				(0024h) */
union ipc_iip_absmin{
	unsigned long		word;
	struct{
		unsigned long	ABSMIN			:1;
		unsigned long					:15;
		
		unsigned long					:16;
	}bit;
};

/* structure of ABSADD				(0040h) */
union ipc_iip_absadd{
	unsigned long		word;
	struct{
		unsigned long	ABSADD			:1;
		unsigned long					:15;
		
		unsigned long					:16;
	}bit;
};

/* structure of OVERVALUE0			(0050h) */
/* structure of OVERVALUE1			(0054h) */
/* structure of OVERVALUE2			(0058h) */
/* structure of OVERVALUE3			(005Ch) */
union ipc_iip_overvalue{
	signed long			word;
	struct{
		  signed long	OVERVALUE		:32;
	}bit;
};



/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_monctl				MONCTL;				/* 0000 - 0003h */
	union ipc_iip_mondsel				MONDSEL;			/* 0004 - 0007h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_mon_0008_000F[0x000F-0x0008+1];		/* 0008 - 000Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_histctl				HISTCTL[4];			/* 0010 - 001Fh */
	union ipc_iip_absmax				ABSMAX;				/* 0020 - 0023h */
	union ipc_iip_absmin				ABSMIN;				/* 0024 - 0027h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_mon_0028_003F[0x003F-0x0028+1];		/* 0028 - 003Fh */
#else
	union ipc_iip_packet_header			header2;
#endif
	
	union ipc_iip_absadd				ABSADD;				/* 0040 - 0043h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_mon_0044_004F[0x004F-0x0044+1];		/* 0044 - 004Fh */
#else
	union ipc_iip_packet_header			header3;
#endif
	
	union ipc_iip_overvalue				OVERVALUE[4];		/* 0050 - 005Fh */
	
} T_IPC_IIP_UNIT_MON;



#define D_IPC_IIP_MON_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_MON_PARAM_PACKET_REGADR_0					(0x0000)
#define D_IPC_IIP_MON_PARAM_PACKET_REGADR_1					(0x0010)
#define D_IPC_IIP_MON_PARAM_PACKET_REGADR_2					(0x0040)
#define D_IPC_IIP_MON_PARAM_PACKET_REGADR_3					(0x0050)

#define D_IPC_IIP_MON_PARAM_LARGE_PACKET_REGSIZ				(((0x0060-0x0000)>>2)+1)
#define D_IPC_IIP_MON_PARAM_PACKET_REGSIZ_0					(((0x0008-0x0000)>>2)+1)
#define D_IPC_IIP_MON_PARAM_PACKET_REGSIZ_1					(((0x0028-0x0010)>>2)+1)
#define D_IPC_IIP_MON_PARAM_PACKET_REGSIZ_2					(((0x0044-0x0040)>>2)+1)
#define D_IPC_IIP_MON_PARAM_PACKET_REGSIZ_3					(((0x0060-0x0050)>>2)+1)

