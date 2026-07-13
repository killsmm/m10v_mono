/**************************************************************************/
/*  <Contents>   Definition IIP GPC UNIT register                         */
/*  <Filename>   __ipc_iip_unit_gpc_param.h                               */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/
/* Structures of GPC parameter                                            */
union ipc_iip_gpc_param_paracnt{
	unsigned long		word;
	struct{
		unsigned long	PARAAD			:12;
		unsigned long	SEQEN			:1;
		unsigned long					:3;
		
		unsigned long	LPARA			:1;
		unsigned long					:15;
	}bit;
};

union ipc_iip_gpc_param_seqloop{
	unsigned long		word;
	struct{
		unsigned long	LOOPEND			:32;
	}bit;
};

union ipc_iip_gpc_param_seqctl{
	unsigned long		word[4];
	struct{
		unsigned long	CNTEND_0		:20;
		unsigned long	SEQCONF_0		:2;
		unsigned long					:2;
		unsigned long	CNFOUT_0		:1;
		unsigned long					:3;
		unsigned long	STEPNUM_0		:4;
		
		unsigned long	CNTEND_1		:20;
		unsigned long	SEQCONF_1		:2;
		unsigned long					:2;
		unsigned long	CNFOUT_1		:1;
		unsigned long					:3;
		unsigned long					:4;
		
		unsigned long	CNTEND_2		:20;
		unsigned long	SEQCONF_2		:2;
		unsigned long					:2;
		unsigned long	CNFOUT_2		:1;
		unsigned long					:3;
		unsigned long					:4;
		
		unsigned long	CNTEND_3		:20;
		unsigned long	SEQCONF_3		:2;
		unsigned long					:2;
		unsigned long	CNFOUT_3		:1;
		unsigned long					:3;
		unsigned long					:4;
	}bit;
};

union ipc_iip_gpc_param_precal{
	unsigned long		word[5];
	struct{
		  signed long	PREOFSET		:15;
		unsigned long					:1;
		unsigned long					:16;
		
		  signed long	PRECLIPMAX		:15;
		unsigned long					:1;
		unsigned long					:16;
		
		  signed long	PRECTHMAX		:15;
		unsigned long					:1;
		unsigned long					:16;
		
		  signed long	PRECLIPMIN		:15;
		unsigned long					:1;
		unsigned long					:16;
		
		  signed long	PRECTHMIN		:15;
		unsigned long					:1;
		unsigned long					:4;
		unsigned long	PREAE			:1;
		unsigned long					:11;
	}bit;
};

union ipc_iip_gpc_param_gpc0para0{
	unsigned long		word;
	struct{
		unsigned long	GPC0SEL0		:5;
		unsigned long	GPC0SEL1		:5;
		unsigned long	GPC0SEL2		:5;
		unsigned long					:1;
		
		unsigned long	GPC0SEL3		:5;
		unsigned long					:3;
		unsigned long	G0MD0			:1;
		unsigned long	G0MD1			:1;
		unsigned long					:2;
		unsigned long	G0MD2			:3;
		unsigned long					:1;
	}bit;
};

union ipc_iip_gpc_param_gpc0para1{
	unsigned long		word[2];
	struct{
		unsigned long	G0RND_0			:1;
		unsigned long					:3;
		unsigned long	G0SFT_0			:4;
		unsigned long	G0RND_1			:1;
		unsigned long					:3;
		unsigned long	G0SFT_1			:4;
		
		unsigned long	G0RND_2			:1;
		unsigned long					:3;
		unsigned long	G0SFT_2			:4;
		unsigned long	G0RND_3			:1;
		unsigned long					:3;
		unsigned long	G0SFT_3			:4;
		
		unsigned long	G0RND_4			:1;
		unsigned long					:3;
		unsigned long	G0SFT_4			:4;
		unsigned long	G0RND_5			:1;
		unsigned long					:3;
		unsigned long	G0SFT_5			:4;
		
		unsigned long	G0RND_6			:1;
		unsigned long					:3;
		unsigned long	G0SFT_6			:4;
		unsigned long					:1;
		unsigned long					:3;
		unsigned long					:4;
	}bit;
};

union ipc_iip_gpc_param_gpc1para0{
	unsigned long		word;
	struct{
		unsigned long	GPC1SEL0		:5;
		unsigned long					:3;
		unsigned long	GPC1SEL1		:5;
		unsigned long					:3;
		
		unsigned long	GPC1SEL2		:5;
		unsigned long					:3;
		unsigned long	G1MD0			:2;
		unsigned long					:2;
		unsigned long	G1MD1			:3;
		unsigned long					:1;
	}bit;
};

union ipc_iip_gpc_param_gpc1para1{
	unsigned long		word[2];
	struct{
		unsigned long	G1RND_0			:1;
		unsigned long					:3;
		unsigned long	G1SFT_0			:4;
		unsigned long	G1RND_1			:1;
		unsigned long					:3;
		unsigned long	G1SFT_1			:4;
		
		unsigned long	G1RND_2			:1;
		unsigned long					:3;
		unsigned long	G1SFT_2			:4;
		unsigned long	G1RND_3			:1;
		unsigned long					:3;
		unsigned long	G1SFT_3			:4;
		
		unsigned long	G1RND_4			:1;
		unsigned long					:3;
		unsigned long	G1SFT_4			:4;
		unsigned long					:8;
		
		unsigned long					:16;
	}bit;
};

union ipc_iip_gpc_param_compsel{
	unsigned long		word;
	struct{
		unsigned long	COMPSEL			:5;
		unsigned long					:3;
		unsigned long	CMPRND			:1;
		unsigned long					:3;
		unsigned long	CMPSFT			:4;
		
		unsigned long	CORPEN			:1;
		unsigned long					:3;
		unsigned long	CORMEN			:1;
		unsigned long					:3;
		unsigned long	CLPPEN			:1;
		unsigned long					:3;
		unsigned long	CLPMEN			:1;
		unsigned long					:3;
	}bit;
};

union ipc_iip_gpc_param_serpass{
	unsigned long		word;
	struct{
		unsigned long	_SEDLY0			:1;
		unsigned long					:3;
		unsigned long	SESEL0			:1;
		unsigned long					:3;
		unsigned long	SEDLY1			:2;
		unsigned long					:2;
		unsigned long	SESEL1			:2;
		unsigned long					:2;
		
		unsigned long	SEDLY2			:2;
		unsigned long					:2;
		unsigned long	SEDLY3			:2;
		unsigned long					:2;
		unsigned long	SEDLY4			:2;
		unsigned long	SEDLY5			:2;
		unsigned long	SEDLY6			:2;
		unsigned long	SEDLY7			:2;
	}bit;
};

union ipc_iip_gpc_param_confcnt{
	unsigned long		word[2];
	struct{
		unsigned long	CONFWEN			:4;
		unsigned long					:4;
		unsigned long					:8;
		
		unsigned long	CONFREGSEL0		:4;
		unsigned long	CONFREGSEL1		:4;
		unsigned long	CONFREGSEL2		:4;
		unsigned long	CONFREGSEL3		:4;
		
		unsigned long	CONFREGSEL4		:4;
		unsigned long	CONFREGSEL5		:4;
		unsigned long	CONFREGSEL6		:4;
		unsigned long	CONFREGSEL7		:4;
		
		unsigned long	CONFREGSEL8		:4;
		unsigned long	CONFREGSEL9		:4;
		unsigned long					:4;
		unsigned long					:4;
	}bit;
};

union ipc_iip_gpc_param_confreg{
	unsigned long		word[4];
	struct{
		unsigned long	CONFREG0		:29;
		unsigned long					:3;
		
		unsigned long	CONFREG1		:29;
		unsigned long					:3;
		
		unsigned long	CONFREG2		:29;
		unsigned long					:3;
		
		unsigned long	CONFREG3		:29;
		unsigned long					:3;
	}bit;
};

union ipc_iip_gpc_param_gpcalp{
	unsigned long		word[4];
	struct{
		signed long		GPCALPFIX_0		:9;
		unsigned long					:7;
		unsigned long					:4;
		unsigned long	GPCALPSEL0_0	:2;
		unsigned long	GPCALPSEL1_0	:2;
		unsigned long	GPCALPMODE_0	:3;
		unsigned long					:5;
		
		signed long		GPCALPFIX_1		:9;
		unsigned long					:7;
		unsigned long					:4;
		unsigned long	GPCALPSEL0_1	:2;
		unsigned long	GPCALPSEL1_1	:2;
		unsigned long	GPCALPMODE_1	:3;
		unsigned long					:5;
		
		signed long		GPCALPFIX_2		:9;
		unsigned long					:7;
		unsigned long					:4;
		unsigned long	GPCALPSEL0_2	:2;
		unsigned long	GPCALPSEL1_2	:2;
		unsigned long	GPCALPMODE_2	:3;
		unsigned long					:5;
		
		signed long		GPCALPFIX_3		:9;
		unsigned long					:7;
		unsigned long					:4;
		unsigned long	GPCALPSEL0_3	:2;
		unsigned long	GPCALPSEL1_3	:2;
		unsigned long	GPCALPMODE_3	:3;
		unsigned long					:5;
	}bit;
};

union ipc_iip_gpc_param_p1outmd{
	unsigned long		word;
	struct{
		unsigned long	P1RNDO_0		:1;
		unsigned long	P1SFTO_0		:4;
		unsigned long	P1SELO_0		:2;
		unsigned long					:1;
		unsigned long	P1RNDO_1		:1;
		unsigned long	P1SFTO_1		:4;
		unsigned long	P1SELO_1		:2;
		unsigned long					:1;
		
		unsigned long	P1RNDO_2		:1;
		unsigned long	P1SFTO_2		:4;
		unsigned long	P1SELO_2		:2;
		unsigned long					:1;
		unsigned long	P1RNDO_3		:1;
		unsigned long	P1SFTO_3		:4;
		unsigned long	P1SELO_3		:2;
		unsigned long					:1;
	}bit;
};

union ipc_iip_gpc_param_p0outmd_d3{
	unsigned long		word;
	struct{
		unsigned long	P0RNDO_D3		:1;
		unsigned long	P0SFTO_D3		:4;
		unsigned long	P0SELO_D3		:2;
		unsigned long					:1;
		unsigned long					:1;
		unsigned long					:4;
		unsigned long					:2;
		unsigned long					:1;
		
		unsigned long					:1;
		unsigned long					:4;
		unsigned long					:2;
		unsigned long					:1;
		unsigned long					:1;
		unsigned long					:4;
		unsigned long					:2;
		unsigned long					:1;
	}bit;
};

union ipc_iip_gpc_param_elecal{
	unsigned long		word[2];
	struct{
		unsigned long	ELESEL0			:2;
		unsigned long	ELERD0			:1;
		unsigned long	ELESHIFT0		:5;
		unsigned long	ELESEL1			:2;
		unsigned long	ELERD1			:1;
		unsigned long	ELESHIFT1		:5;
		unsigned long	ELESEL2			:2;
		unsigned long	ELERD2			:1;
		unsigned long	ELESHIFT2		:5;
		unsigned long	ELESEL3			:2;
		unsigned long	ELERD3			:1;
		unsigned long	ELESHIFT3		:5;
		
		unsigned long	ELEMODE0		:3;
		unsigned long					:1;
		unsigned long	ELEMODE1		:3;
		unsigned long					:1;
		unsigned long	ELEMODE2		:3;
		unsigned long					:1;
		unsigned long	ELERDO			:1;
		unsigned long	ELESHIFTO		:4;
		unsigned long					:7;
		unsigned long					:8;
	}bit;
};


typedef struct t_ipc_iip_unit_gpcseqctl{
	union ipc_iip_gpc_param_paracnt		PARACNT;			/* 0000 - 0003h */
	union ipc_iip_gpc_param_seqloop		SEQLOOP;			/* 0004 - 0007h */
	
	unsigned char dumy_gpc_param_0008_000F[0x0010-0x0008];	/* 0008 - 000Fh */
	
	union ipc_iip_gpc_param_seqctl		SEQCTL;				/* 0010 - 001Fh */
	
	unsigned char dumy_gpc_param_0020_003F[0x0040-0x0020];	/* 0020 - 003Fh */
	
} T_IPC_IIP_UNIT_GPC_SEQCTL;


typedef struct t_ipc_iip_unit_gpc_precal{
	union ipc_iip_gpc_param_paracnt		PARACNT;			/* 0100 - 0103h */
	union ipc_iip_gpc_param_precal		PRECAL[2];			/* 0104 - 012Bh */
	
	unsigned char dumy_gpc_param_012C_013F[0x0140-0x012C];	/* 012C - 013Fh */
	
} T_IPC_IIP_UNIT_GPC_PRECAL;


typedef struct t_ipc_iip_unit_gpc_conf{
	union ipc_iip_gpc_param_paracnt		PARACNT;			/* 0200 - 0203h */
	union ipc_iip_gpc_param_gpc0para0	GPC0PARA0;			/* 0204 - 020Fh */
	union ipc_iip_gpc_param_gpc0para1	GPC0PARA1;			/* 0208 - 000Fh */
	union ipc_iip_gpc_param_gpc1para0	GPC1PARA0;			/* 0210 - 0213h */
	
	unsigned char dumy_gpc_param_0214_0217[0x0218-0x0214];	/* 0214 - 0217h */
	
	union ipc_iip_gpc_param_gpc1para1	GPC1PARA1;			/* 0218 - 021Fh */
	union ipc_iip_gpc_param_serpass		SERPASS;			/* 0220 - 0223h */
	union ipc_iip_gpc_param_compsel		COMPSEL;			/* 0224 - 0227h */
	union ipc_iip_gpc_param_confcnt		CONFCNT;			/* 0228 - 022Fh */
	union ipc_iip_gpc_param_confreg		CONFREG;			/* 0230 - 023Fh */
	
} T_IPC_IIP_UNIT_GPC_CONF;


typedef struct t_ipc_iip_unit_gpc_alpha{
	union ipc_iip_gpc_param_paracnt		PARACNT;			/* 0500 - 0503h */
	
	unsigned char dumy_gpc_param_0504_050F[0x0510-0x0504];	/* 0504 - 050Fh */
	
	union ipc_iip_gpc_param_gpcalp		GCALP;				/* 0510 - 051Fh */
	
	unsigned char dumy_gpc_param_0520_053F[0x0540-0x0520];	/* 0520 - 053Fh */
	
} T_IPC_IIP_UNIT_GPC_ALPHA;


typedef struct t_ipc_iip_unit_gpc_elecal{
	union ipc_iip_gpc_param_paracnt		PARACNT;			/* 0540 - 0543h */
	
	unsigned char dumy_gpc_param_0544_054F[0x0550-0x0544];	/* 0544 - 054Fh */
	
	union ipc_iip_gpc_param_p1outmd		P1OUTMD;			/* 0550 - 0553h */
	union ipc_iip_gpc_param_p0outmd_d3	P0OUTMD_D3;			/* 0554 - 0557h */
	
	unsigned char dumy_gpc_param_0558_055F[0x0560-0x0558];	/* 0558 - 055Fh */
	
	union ipc_iip_gpc_param_elecal		ELECAL[3];			/* 0560 - 0577h */
	
	unsigned char dumy_gpc_param_0578_057F[0x0580-0x0578];	/* 0578 - 057Fh */
	
} T_IPC_IIP_UNIT_GPC_ELECAL;



typedef struct{
	struct t_ipc_iip_unit_gpcseqctl		GPCSEQCTL;			/* 0000 - 003Fh */
	
	struct t_ipc_iip_unit_gpc_precal	GPCPRECAL0[2];		/* 0100 - 017Fh */
	struct t_ipc_iip_unit_gpc_precal	GPCPRECAL1[2];		/* 0180 - 01FFh */
	
	struct t_ipc_iip_unit_gpc_conf		GPCCONF0[4];		/* 0200 - 02FFh */
	struct t_ipc_iip_unit_gpc_conf		GPCCONF1[4];		/* 0300 - 03FFh */
	struct t_ipc_iip_unit_gpc_conf		GPCCONF2[4];		/* 0400 - 04FFh */
	
	struct t_ipc_iip_unit_gpc_alpha		GPCALPHA;			/* 0500 - 053Fh */
	struct t_ipc_iip_unit_gpc_elecal	GPCELECAL;			/* 0540 - 057Fh */
	
} T_IPC_IIP_UNIT_GPC_PARAM;

