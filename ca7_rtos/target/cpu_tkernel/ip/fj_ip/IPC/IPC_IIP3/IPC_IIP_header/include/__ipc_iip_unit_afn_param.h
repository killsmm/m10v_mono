/**************************************************************************/
/*  <Contents>   Definition IIP AFN UNIT register                         */
/*  <Filename>   __ipc_iip_unit_afn_param.h                               */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*               2016/05/31   H.Hata                                 1.00 */
/*                                                                        */
/*                                    Socionext Middleware Library        */
/*                                    COPYRIGHT Socionext Inc. 2015-2016  */
/**************************************************************************/
/* Structures of Affine parameter                                         */
union ipc_iip_afn_param_stax{
	signed long			word;
	struct{
		signed long		STAX			:31;
		signed long						:1;
	}bit;
};

union ipc_iip_afn_param_stay{
	signed long			word;
	struct{
		signed long		STAY			:31;
		signed long						:1;
	}bit;
};

union ipc_iip_afn_param_dxx{
	signed long			word;
	struct{
		signed long		DXX				:24;
		signed long						:8;
	}bit;
};

union ipc_iip_afn_param_dxy{
	signed long			word;
	struct{
		signed long		DXY				:24;
		signed long						:8;
	}bit;
};

union ipc_iip_afn_param_dyx{
	signed long			word;
	struct{
		signed long		DYX				:24;
		signed long						:8;
	}bit;
};

union ipc_iip_afn_param_dyy{
	signed long			word;
	struct{
		signed long		DYY				:24;
		signed long						:8;
	}bit;
};

union ipc_iip_afn_param_dhsz{
	unsigned short		hword;
	struct{
		unsigned short	AFNDHSZ			:14;
		unsigned short					:2;
	}bit;
};

union ipc_iip_afn_param_dvsz{
	unsigned short		hword;
	struct{
		unsigned short	AFNDVSZ			:14;
		unsigned short					:2;
	}bit;
};



typedef struct{
	union ipc_iip_afn_param_stax		STAX0;				/* 0000 - 0003h */
	union ipc_iip_afn_param_stay		STAY0;				/* 0004 - 0007h */
	union ipc_iip_afn_param_dxx			DXX0;				/* 0008 - 000Bh */
	union ipc_iip_afn_param_dxy			DXY0;				/* 000C - 000Fh */
	union ipc_iip_afn_param_dyx			DYX0;				/* 0010 - 0013h */
	union ipc_iip_afn_param_dyy			DYY0;				/* 0014 - 0017h */
	union ipc_iip_afn_param_dhsz		AFNDHSZ;			/* 0018 - 0019h */
	union ipc_iip_afn_param_dvsz		AFNDVSZ;			/* 001A - 001Bh */
	
	unsigned char dumy_afn_param_001C_001F[0x0020-0x001C];	/* 001C - 001Fh */
	
} T_IPC_IIP_UNIT_AFN_PARAM0;	/* for CALMETHOD.OPCOL=0 */



typedef struct{
	union ipc_iip_afn_param_stax		STAX0;				/* 0000 - 0003h */
	union ipc_iip_afn_param_stay		STAY0;				/* 0004 - 0007h */
	union ipc_iip_afn_param_dxx			DXX0;				/* 0008 - 000Bh */
	union ipc_iip_afn_param_dxy			DXY0;				/* 000C - 000Fh */
	union ipc_iip_afn_param_dyx			DYX0;				/* 0010 - 0013h */
	union ipc_iip_afn_param_dyy			DYY0;				/* 0014 - 0017h */
	union ipc_iip_afn_param_dhsz		AFNDHSZ;			/* 0018 - 0019h */
	union ipc_iip_afn_param_dvsz		AFNDVSZ;			/* 001A - 001Bh */
	
	unsigned char dumy_afn_param_001C_001F[0x0020-0x001C];	/* 001C - 001Fh */
	
	union ipc_iip_afn_param_stax		STAX1;				/* 0020 - 0023h */
	union ipc_iip_afn_param_stay		STAY1;				/* 0024 - 0027h */
	union ipc_iip_afn_param_dxx			DXX1;				/* 0028 - 002Bh */
	union ipc_iip_afn_param_dxy			DXY1;				/* 002C - 002Fh */
	union ipc_iip_afn_param_dyx			DYX1;				/* 0030 - 0033h */
	union ipc_iip_afn_param_dyy			DYY1;				/* 0034 - 0037h */
	
	unsigned char dumy_afn_param_0038_003F[0x0040-0x0038];	/* 0038 - 003Fh */
	
	union ipc_iip_afn_param_stax		STAX2;				/* 0040 - 0043h */
	union ipc_iip_afn_param_stay		STAY2;				/* 0044 - 0047h */
	union ipc_iip_afn_param_dxx			DXX2;				/* 0048 - 004Bh */
	union ipc_iip_afn_param_dxy			DXY2;				/* 004C - 004Fh */
	union ipc_iip_afn_param_dyx			DYX2;				/* 0050 - 0053h */
	union ipc_iip_afn_param_dyy			DYY2;				/* 0054 - 0057h */
	
	unsigned char dumy_afn_param_0058_005F[0x0060-0x0058];	/* 0058 - 005Fh */
	
	union ipc_iip_afn_param_stax		STAX3;				/* 0060 - 0063h */
	union ipc_iip_afn_param_stay		STAY3;				/* 0064 - 0067h */
	union ipc_iip_afn_param_dxx			DXX3;				/* 0068 - 006Bh */
	union ipc_iip_afn_param_dxy			DXY3;				/* 006C - 006Fh */
	union ipc_iip_afn_param_dyx			DYX3;				/* 0070 - 0073h */
	union ipc_iip_afn_param_dyy			DYY3;				/* 0074 - 0077h */
	
	unsigned char dumy_afn_param_0078_007F[0x0080-0x0078];	/* 0078 - 007Fh */
	
} T_IPC_IIP_UNIT_AFN_PARAM1;	/* for CALMETHOD.OPCOL=1 */

