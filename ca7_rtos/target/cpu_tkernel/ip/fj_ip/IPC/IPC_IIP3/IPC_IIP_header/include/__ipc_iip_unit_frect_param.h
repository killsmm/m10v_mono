/**************************************************************************/
/*  <Contents>   Definition IIP FRECT UNIT register                       */
/*  <Filename>   __ipc_iip_unit_afn_param.h                               */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
/* Structures of Frect parameter                                          */
union ipc_iip_frect_param_ax{
	unsigned long		word;
	struct{
		unsigned long	AX				:23;
		unsigned long					:9;
	}bit;
};

union ipc_iip_frect_param_ay{
	unsigned long		word;
	struct{
		unsigned long	AY				:23;
		unsigned long					:9;
	}bit;
};

union ipc_iip_frect_param_bx{
	unsigned long		word;
	struct{
		unsigned long	BX				:23;
		unsigned long					:9;
	}bit;
};

union ipc_iip_frect_param_by{
	unsigned long		word;
	struct{
		unsigned long	BY				:23;
		unsigned long					:9;
	}bit;
};

union ipc_iip_frect_param_cx{
	unsigned long		word;
	struct{
		unsigned long	CX				:23;
		unsigned long					:9;
	}bit;
};

union ipc_iip_frect_param_cy{
	unsigned long		word;
	struct{
		unsigned long	CY				:23;
		unsigned long					:9;
	}bit;
};

union ipc_iip_frect_param_dx{
	unsigned long		word;
	struct{
		unsigned long	DX				:23;
		unsigned long					:9;
	}bit;
};

union ipc_iip_frect_param_dy{
	unsigned long		word;
	struct{
		unsigned long	DY				:23;
		unsigned long					:9;
	}bit;
};

union ipc_iip_frect_param_szpow{
	unsigned long		word;
	struct{
		unsigned long	SZPOWH			:3;
		unsigned long					:5;
		unsigned long	SZPOWV			:3;
		unsigned long					:5;
		
		unsigned long					:16;
	}bit;
};



typedef struct{
	union ipc_iip_frect_param_ax		AX0;				/* 0000 - 0003h */
	union ipc_iip_frect_param_ay		AY0;				/* 0004 - 0007h */
	union ipc_iip_frect_param_ax		BX0;				/* 0008 - 000Bh */
	union ipc_iip_frect_param_ay		BY0;				/* 000C - 000Fh */
	union ipc_iip_frect_param_ax		CX0;				/* 0010 - 0013h */
	union ipc_iip_frect_param_ay		CY0;				/* 0014 - 0017h */
	union ipc_iip_frect_param_ax		DX0;				/* 0018 - 001Bh */
	union ipc_iip_frect_param_ay		DY0;				/* 001C - 001Fh */
	union ipc_iip_frect_param_szpow		SZPOW;				/* 0020 - 0023h */
	
	unsigned char dumy_fr_param_0024_0027[0x0028-0x0024];	/* 0024 - 0027h */
	
} T_IPC_IIP_UNIT_FRECT_PARAM0;	/* for CALMETHOD.OPCOL=0 */



typedef struct{
	union ipc_iip_frect_param_ax		AX0;				/* 0000 - 0003h */
	union ipc_iip_frect_param_ay		AY0;				/* 0004 - 0007h */
	union ipc_iip_frect_param_ax		BX0;				/* 0008 - 000Bh */
	union ipc_iip_frect_param_ay		BY0;				/* 000C - 000Fh */
	union ipc_iip_frect_param_ax		CX0;				/* 0010 - 0013h */
	union ipc_iip_frect_param_ay		CY0;				/* 0014 - 0017h */
	union ipc_iip_frect_param_ax		DX0;				/* 0018 - 001Bh */
	union ipc_iip_frect_param_ay		DY0;				/* 001C - 001Fh */
	union ipc_iip_frect_param_szpow		SZPOW;				/* 0020 - 0023h */
	
	unsigned char dumy_fr_param_0024_002F[0x0030-0x0024];	/* 0024 - 002Fh */
	
	union ipc_iip_frect_param_ax		AX1;				/* 0030 - 0033h */
	union ipc_iip_frect_param_ay		AY1;				/* 0034 - 0037h */
	union ipc_iip_frect_param_ax		BX1;				/* 0038 - 003Bh */
	union ipc_iip_frect_param_ay		BY1;				/* 003C - 003Fh */
	union ipc_iip_frect_param_ax		CX1;				/* 0040 - 0043h */
	union ipc_iip_frect_param_ay		CY1;				/* 0044 - 0047h */
	union ipc_iip_frect_param_ax		DX1;				/* 0048 - 004Bh */
	union ipc_iip_frect_param_ay		DY1;				/* 004C - 004Fh */
	
	union ipc_iip_frect_param_ax		AX2;				/* 0050 - 0053h */
	union ipc_iip_frect_param_ay		AY2;				/* 0054 - 0057h */
	union ipc_iip_frect_param_ax		BX2;				/* 0058 - 005Bh */
	union ipc_iip_frect_param_ay		BY2;				/* 005C - 005Fh */
	union ipc_iip_frect_param_ax		CX2;				/* 0060 - 0063h */
	union ipc_iip_frect_param_ay		CY2;				/* 0064 - 0067h */
	union ipc_iip_frect_param_ax		DX2;				/* 0068 - 006Bh */
	union ipc_iip_frect_param_ay		DY2;				/* 006C - 006Fh */
	
	union ipc_iip_frect_param_ax		AX3;				/* 0070 - 0073h */
	union ipc_iip_frect_param_ay		AY3;				/* 0074 - 0077h */
	union ipc_iip_frect_param_ax		BX3;				/* 0078 - 007Bh */
	union ipc_iip_frect_param_ay		BY3;				/* 007C - 007Fh */
	union ipc_iip_frect_param_ax		CX3;				/* 0080 - 0083h */
	union ipc_iip_frect_param_ay		CY3;				/* 0084 - 0087h */
	union ipc_iip_frect_param_ax		DX3;				/* 0088 - 008Bh */
	union ipc_iip_frect_param_ay		DY3;				/* 008C - 008Fh */
	
} T_IPC_IIP_UNIT_FRECT_PARAM1;	/* for CALMETHOD.OPCOL=1 */

