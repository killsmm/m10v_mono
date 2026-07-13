/**************************************************************************/
/*  <Contents>   Definition IIP BLEND UNIT register                       */
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

/* structure of BLENDTOPCNF0		(0000h) */
union ipc_iip_blendtopconf0{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF_0		:6;
		unsigned long					:10;
		unsigned long	DATACONF_0		:6;
		unsigned long					:10;
	}bit;
};

/* structure of BLENDTOPCNF1		(0004h) */
union ipc_iip_blendtopconf1{
	unsigned long		word;
	struct{
		unsigned long	WAITCONF_1		:6;
		unsigned long					:10;
		unsigned long	DATACONF_1		:6;
		unsigned long					:10;
	}bit;
};

/* structure of BLENDTOPCNF2		(0008h) */
union ipc_iip_blendtopconf2{
	unsigned long		word;
	struct{
		unsigned long					:16;
		
		unsigned long	DATACONF_2		:6;
		unsigned long					:10;
	}bit;
};

/* structure of BLENDCTL			(0080h) */
union ipc_iip_blendctl{
	unsigned long		word;
	struct{
		unsigned long	BLD_MD			:3;
		unsigned long					:1;
		unsigned long	CLP_MD			:2;
		unsigned long					:2;
		unsigned long	ALPHA_OUT		:2;
		unsigned long					:6;
		
		unsigned long					:16;
	}bit;
};

/* structure of ALPHACTL0			(0088h) */
union ipc_iip_blend_alphactl0{
	unsigned long		word;
	struct{
		unsigned long	ALPHA_STEP		:2;
		unsigned long					:2;
		unsigned long	ALPHA_SEL		:3;
		unsigned long					:9;
		unsigned long					:16;
	}bit;
};

/* structure of ALPHACTL0			(008Ch) */
/* structure of ALPHACTL1			(008Eh) */
/* structure of ALPHACTL2			(0090h) */
/* structure of ALPHACTL3			(0092h) */
union ipc_iip_blend_alphactl12{
	unsigned short		word;
	struct{
		unsigned short	ALPHA_VAL		:8;
		unsigned short					:8;
	}bit;
};

/* structure of WADDCTL0			(0098h) */
/* structure of WADDCTL1			(0098h) */
/* structure of WADDCTL2			(0098h) */
/* structure of WADDCTL3			(0098h) */
union ipc_iip_blend_waddctl{
	unsigned long		word;
	struct{
		  signed long	WADD_KA			:13;
		unsigned long					:3;
		
		  signed long	WADD_KB			:13;
		unsigned long					:3;
	}bit;
};

/* structure of DIVCTL0				(00ACh) */
union ipc_iip_blend_divctl0{
	unsigned long		word;
	struct{
		unsigned long	RSFT			:4;
		unsigned long					:12;
		
		unsigned long					:16;
	}bit;
};

/* structure of MSKCTL0				(00B4h) */
union ipc_iip_blend_mskctl0{
	unsigned long		word;
	struct{
		unsigned long	MSKSEL			:1;
		unsigned long					:3;
		unsigned long	DINSEL			:1;
		unsigned long					:11;
		
		unsigned long					:16;
	}bit;
};

/* structure of MULCTL0				(00BCh) */
/* structure of MULCTL1				(00C0h) */
/* structure of MULCTL2				(00C4h) */
/* structure of MULCTL3				(00C8h) */
/* structure of MULCTL4				(00CCh) */
/* structure of MULCTL5				(00D0h) */
/* structure of MULCTL6				(00D4h) */
/* structure of MULCTL7				(00D8h) */
union ipc_iip_blend_mulctl{
	unsigned long		word[2];
	struct{
		  signed long	ADDVAL_A		:15;
		unsigned long					:1;
		  signed long	ADDVAL_B		:15;
		unsigned long					:1;
		  signed long	ADDVAL_C		:15;
		unsigned long					:1;
		unsigned long	SFTSEL			:2;
		unsigned long					:14;
	}bit;
};



/* Define i/o mapping */
typedef struct{
	union ipc_iip_packet_header			header0;
	
	union ipc_iip_blendtopconf0			BLENDTOPCNF0;		/* 0000 - 0003h */
	union ipc_iip_blendtopconf1			BLENDTOPCNF1;		/* 0004 - 0007h */
	union ipc_iip_blendtopconf2			BLENDTOPCNF2;		/* 0008 - 000Bh */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_blend_000C_007F[0x007F-0x000C+1];	/* 000C - 007Fh */
#else
	union ipc_iip_packet_header			header1;
#endif
	
	union ipc_iip_blendctl				BLENDCTL;			/* 0080 - 0083h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_blend_0084_0087[0x0087-0x0084+1];	/* 0084 - 0087h */
#else
	union ipc_iip_packet_header			header2;
#endif
	
	union ipc_iip_blend_alphactl0		ALPHACTL;			/* 0088 - 008Bh */
	union ipc_iip_blend_alphactl12		ALPHAVAL[4];		/* 008C - 0093h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_blend_0094_0097[0x0097-0x0094+1];	/* 0094 - 0097h */
#else
	union ipc_iip_packet_header			header3;
#endif
	
	union ipc_iip_blend_waddctl			WADDCTR[4];			/* 0098 - 00A7h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_blend_00A8_00AB[0x00AB-0x00A8+1];	/* 00A8 - 00ABh */
#else
	union ipc_iip_packet_header			header4;
#endif
	
	union ipc_iip_blend_divctl0			DIVCTL0;			/* 00AC - 00AFh */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_blend_00B0_00B3[0x00B3-0x00B0+1];	/* 00B0 - 00B3h */
#else
	union ipc_iip_packet_header			header5;
#endif
	
	union ipc_iip_blend_mskctl0			MSKCTL0;			/* 00B4 - 00B7h */
	
#ifdef D_IPC_IIP_LARGE_PACKET
	unsigned char dumy_blend_00B8_00BB[0x00BB-0x00B8+1];	/* 00B8 - 00BBh */
#else
	union ipc_iip_packet_header			header6;
#endif
	
	union ipc_iip_blend_mulctl			MULCTL[4];			/* 00BC - 00DBh */
	
} T_IPC_IIP_UNIT_BLEND;



#define D_IPC_IIP_BLEND_PARAM_LARGE_PACKET_REGADR			(0x0000)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGADR_0				(0x0000)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGADR_1				(0x0080)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGADR_2				(0x0088)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGADR_3				(0x0098)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGADR_4				(0x00AC)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGADR_5				(0x00B4)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGADR_6				(0x00BC)

#define D_IPC_IIP_BLEND_PARAM_LARGE_PACKET_REGSIZ			(((0x00DC-0x0000)>>2)+1)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGSIZ_0				(((0x000C-0x0000)>>2)+1)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGSIZ_1				(((0x0084-0x0080)>>2)+1)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGSIZ_2				(((0x0094-0x0088)>>2)+1)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGSIZ_3				(((0x00A8-0x0098)>>2)+1)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGSIZ_4				(((0x00B0-0x00AC)>>2)+1)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGSIZ_5				(((0x00B8-0x00B4)>>2)+1)
#define D_IPC_IIP_BLEND_PARAM_PACKET_REGSIZ_6				(((0x00DC-0x00BC)>>2)+1)

