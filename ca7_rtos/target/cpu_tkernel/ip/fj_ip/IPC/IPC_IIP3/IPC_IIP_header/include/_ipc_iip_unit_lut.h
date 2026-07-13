/**************************************************************************/
/*  <Contents>   Definition IIP LUT UNIT register                         */
/*  <Filename>   _ipc_iip_unit_lut.h                                      */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
#include "_ipc_iip_packet_header.h"

/* structure of LUTTOPCNF0			(0000h) */
/* structure of LUTTOPCNF1			(0004h) */
/* structure of LUTTOPCNF2			(0008h) */
/* structure of LUTTOPCNF3			(000Ch) */
union ipc_iip_luttopcnf{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF		:6;
		unsigned long					:10;
		
		unsigned long	DATACONF		:6;
		unsigned long					:10;
	}bit;
};

/* structure of LUTUNITCTL			(0010h) */
union ipc_iip_lutunitctl{
	unsigned long		word;
	struct{
		unsigned long	PORTEN			:4;
		unsigned long	PORT3SELI		:2;
		unsigned long					:2;
		unsigned long	PORT3SELO		:2;
		unsigned long					:2;
		unsigned long	BRANCH			:2;
		unsigned long					:2;
		
		unsigned long	PORT2SELI		:2;
		unsigned long					:2;
		unsigned long	PORT2SELO		:2;
		unsigned long					:2;
		unsigned long	OUTMD0			:1;
		unsigned long	OUTMD1			:1;
		unsigned long	OUTMD2			:1;
		unsigned long	OUTMD3			:1;
		unsigned long					:4;
	}bit;
};

/* structure of LUTCTL_A			(0020h) */
/* structure of LUTCTL_B			(0040h) */
/* structure of LUTCTL_C			(0060h) */
/* structure of LUTCTL_D			(0080h) */
/* structure of LUTCTL_E			(00A0h) */
/* structure of LUTCTL_F			(00C0h) */
union ipc_iip_lutctl{
	unsigned long		word;
	struct{
		unsigned long	LUTBIT			:4;
		unsigned long	USE				:1;
		unsigned long					:3;
		unsigned long	SGN				:1;
		unsigned long					:3;
		unsigned long	SPLBIT			:4;
		
		unsigned long					:16;
	}bit;
};

/* structure of LUTPRECAL_A			(0030h) */
/* structure of LUTPRECAL_B			(0050h) */
/* structure of LUTPRECAL_C			(0070h) */
/* structure of LUTPRECAL_D			(0090h) */
/* structure of LUTPRECAL_E			(00B0h) */
/* structure of LUTPRECAL_F			(00D0h) */
union ipc_iip_lutprecal{
	unsigned long		word[4];
	struct {
		unsigned long	SHIFT			:4;
		unsigned long					:12;
		  signed long	OFSET			:15;
		unsigned long					:1;
		
		  signed long	CLPMAX			:15;
		unsigned long					:1;
		  signed long	THMAX			:15;
		unsigned long					:1;
		
		  signed long	CLPMIN			:15;
		unsigned long					:1;
		  signed long	THMIN			:15;
		unsigned long					:1;
		
		unsigned long	ABS				:1;
		unsigned long					:15;
		unsigned long	LUTADRS			:15;
		unsigned long					:1;
	}bit;
};

/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_luttopcnf				LUTTOPCNF[4];		/* 0000 - 000Fh */
	union ipc_iip_lutunitctl			LUTUNITCTL;			/* 0010 - 0013h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_lut_0014_001F[0x001F-0x0014+1];		/* 0014 - 001Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_lutctl				LUTCTL_A;			/* 0020 - 0023h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_lut_0024_002F[0x002F-0x0024+1];		/* 0024 - 002Fh */
#else
	union ipc_iip_packet_header			header2;
#endif
	
	union ipc_iip_lutprecal				LUTPRECAL_A;		/* 0030 - 003Fh */
	
	union ipc_iip_lutctl				LUTCTL_B;			/* 0040 - 0043h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_lut_0044_004F[0x004F-0x0044+1];		/* 0044 - 004Fh */
#else
	union ipc_iip_packet_header			header3;
#endif
	
	union ipc_iip_lutprecal				LUTPRECAL_B;		/* 0050 - 005Fh */
	
	union ipc_iip_lutctl				LUTCTL_C;			/* 0060 - 0063h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_lut_0064_006F[0x006F-0x0064+1];		/* 0064 - 006Fh */
#else
	union ipc_iip_packet_header			header4;
#endif
	
	union ipc_iip_lutprecal				LUTPRECAL_C;		/* 0070 - 007Fh */
	
	union ipc_iip_lutctl				LUTCTL_D;			/* 0080 - 0083h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_lut_0084_008F[0x008F-0x0084+1];		/* 0084 - 008Fh */
#else
	union ipc_iip_packet_header			header5;
#endif
	
	union ipc_iip_lutprecal				LUTPRECAL_D;		/* 0090 - 009Fh */
	
	union ipc_iip_lutctl				LUTCTL_E;			/* 00A0 - 00A3h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_lut_00A4_00AF[0x00AF-0x00A4+1];		/* 00A4 - 00AFh */
#else
	union ipc_iip_packet_header			header6;
#endif
	
	union ipc_iip_lutprecal				LUTPRECAL_E;		/* 00B0 - 00BFh */
	
	union ipc_iip_lutctl				LUTCTL_F;			/* 00C0 - 00C3h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_lut_00C4_00CF[0x00CF-0x00C4+1];		/* 00C4 - 00CFh */
#else
	union ipc_iip_packet_header			header7;
#endif
	
	union ipc_iip_lutprecal				LUTPRECAL_F;		/* 00D0 - 00DFh */
	
} T_IPC_IIP_UNIT_LUT;



#define D_IPC_IIP_LUT_PARAM_LARGE_PACKET_REGADR				(0x0000)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGADR_0					(0x0000)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGADR_1					(0x0020)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGADR_2					(0x0030)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGADR_3					(0x0050)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGADR_4					(0x0070)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGADR_5					(0x0090)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGADR_6					(0x00B0)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGADR_7					(0x00D0)

#define D_IPC_IIP_LUT_PARAM_LARGE_PACKET_REGSIZ				(((0x00E0-0x0000)>>2)+1)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGSIZ_0					(((0x0014-0x0000)>>2)+1)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGSIZ_1					(((0x0024-0x0020)>>2)+1)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGSIZ_2					(((0x0044-0x0030)>>2)+1)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGSIZ_3					(((0x0064-0x0050)>>2)+1)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGSIZ_4					(((0x0084-0x0070)>>2)+1)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGSIZ_5					(((0x00A4-0x0090)>>2)+1)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGSIZ_6					(((0x00C4-0x00B0)>>2)+1)
#define D_IPC_IIP_LUT_PARAM_PACKET_REGSIZ_7					(((0x00E0-0x00D0)>>2)+1)

