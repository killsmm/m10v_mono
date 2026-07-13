/**
 * @file		_audio_if.h
 * @brief		Definition Audio I/F Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*	structure of AURES	*/
union io_audio_aures{
	unsigned long		word;
	struct {
		unsigned long	AUIF0_RST	:1;
		unsigned long	AUIF1_RST	:1;
		unsigned long	AUIF2_RST	:1;
		unsigned long	AUIF3_RST	:1;
		unsigned long	AUIF4_RST	:1;
		unsigned long	AUIF5_RST	:1;
		unsigned long				:26;
	}bit;
};

/*	structure of AUIOE	*/
union io_audio_auioe{
	unsigned long		word;
	struct {
		unsigned long	AUIE0		:2;
		unsigned long	AUIE1		:2;
		unsigned long	AUIE2		:2;
		unsigned long	AUIE3		:2;
		unsigned long	AUIE4		:2;
		unsigned long	AUIE5		:2;
		unsigned long				:4;
		unsigned long	AUOE0		:2;
		unsigned long	AUOE1		:2;
		unsigned long	AUOE2		:2;
		unsigned long	AUOE3		:2;
		unsigned long	AUOE4		:2;
		unsigned long	AUOE5		:2;
		unsigned long				:4;
	}bit;
};

/*	structure of AULBE	*/
union io_audio_aulbe{
	unsigned long		word;
	struct {
		unsigned long	AULBE0		:2;
		unsigned long	AULBE1		:2;
		unsigned long	AULBE2		:2;
		unsigned long	AULBE3		:2;
		unsigned long	AULBE4		:2;
		unsigned long	AULBE5		:2;
		unsigned long				:20;
	}bit;
};

/*	structure of AUCR	*/
union io_audio_aucr{
	unsigned long		word;
	struct {
		unsigned long	AUIDSEL 	:1;
		unsigned long	AUODSEL 	:1;
		unsigned long				:6;
		unsigned long	INTI		:1;
		unsigned long	INTO		:1;
		unsigned long	INTIE		:1;
		unsigned long	INTOE		:1;
		unsigned long	AUIEF		:1;
		unsigned long	AUOEF		:1;
		unsigned long	DMIE		:1;
		unsigned long	DMOE		:1;
		unsigned long	AUIF		:4;
		unsigned long				:3;
		unsigned long	AUIRF		:1;
		unsigned long	AUOF		:7;
		unsigned long	AUORF		:1;
	}bit;
};

/*	structure of AUMD	*/
union io_audio_aumd{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	IBYTE_SWP	:1;
		unsigned long	IHW_SWP		:1;
		unsigned long	OBYTE_SWP	:1;
		unsigned long	OHW_SWP		:1;
		unsigned long				:4;
		unsigned long	ISTG16		:1;
		unsigned long	OSTG16		:1;
		unsigned long				:1;
		unsigned long	STEREO		:1;
		unsigned long				:3;
		unsigned long	LBF 		:1;
	}bit;
};

/*	structure of AUST	*/
union io_audio_aust{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	IROF		:1;
		unsigned long	ORUF		:1;
		unsigned long	OFIE		:1;
		unsigned long	UFIE		:1;
		unsigned long				:4;
		unsigned long	EINTI		:1;
		unsigned long	EINTO		:1;
		unsigned long	EINTIE		:1;
		unsigned long	EINTOE		:1;
		unsigned long	EDMIE		:1;
		unsigned long	EDMOE		:1;
		unsigned long	ESTG		:2;
	}bit;
};

/*	structure of AUCC	*/
union io_audio_aucc{
	unsigned long		word;
	struct {
		unsigned long				:16;
		unsigned long	DIVLR		:2;
		unsigned long	DIVCK		:3;
		unsigned long				:3;
		unsigned long	DIVMCK		:5;
		unsigned long				:1;
		unsigned long	DIVE		:1;
		unsigned long	AUCKOE		:1;
	}bit;
};

/*	structure of AUDP	*/
union io_audio_audp{
	unsigned long		word;
	struct {
		unsigned long	MIXPLAY 	:1;
		unsigned long				:7;
		unsigned long	AULRCP		:1;
		unsigned long				:7;
		unsigned long	AUIDF		:1;
		unsigned long	AUODF		:1;
		unsigned long				:6;
		unsigned long	AUODS		:2;
		unsigned long				:2;
		unsigned long	AUIDS		:2;
		unsigned long				:2;
	}bit;
};

/*	structure of AUIFST	*/
union io_audio_auifst{
	unsigned long		word;
	struct {
		unsigned long	AUISL0		:1;
		unsigned long	AUISR0		:1;
		unsigned long	AUISL1		:1;
		unsigned long	AUISR1		:1;
		unsigned long	AUISL2		:1;
		unsigned long	AUISR2		:1;
		unsigned long	AUISL3		:1;
		unsigned long	AUISR3		:1;
		unsigned long	AUISL4		:1;
		unsigned long	AUISR4		:1;
		unsigned long	AUISL5		:1;
		unsigned long	AUISR5		:1;
		unsigned long	AUISL6		:1;
		unsigned long	AUISR6		:1;
		unsigned long	AUISL7		:1;
		unsigned long	AUISR7		:1;
		unsigned long	AUISL8		:1;
		unsigned long	AUISR8		:1;
		unsigned long	AUISL9		:1;
		unsigned long	AUISR9		:1;
		unsigned long	AUISL10 	:1;
		unsigned long	AUISR10 	:1;
		unsigned long	AUISL11 	:1;
		unsigned long	AUISR11 	:1;
		unsigned long	AUISL12 	:1;
		unsigned long	AUISR12 	:1;
		unsigned long	AUISL13 	:1;
		unsigned long	AUISR13 	:1;
		unsigned long	AUISL14 	:1;
		unsigned long	AUISR14 	:1;
		unsigned long	AUISL15 	:1;
		unsigned long	AUISR15 	:1;
	}bit;
};

/*	structure of AUOFST	*/
union io_audio_auofst{
	unsigned long		word;
	struct {
		unsigned long	AUOSL0		:1;
		unsigned long	AUOSR0		:1;
		unsigned long	AUOSL1		:1;
		unsigned long	AUOSR1		:1;
		unsigned long	AUOSL2		:1;
		unsigned long	AUOSR2		:1;
		unsigned long	AUOSL3		:1;
		unsigned long	AUOSR3		:1;
		unsigned long	AUOSL4		:1;
		unsigned long	AUOSR4		:1;
		unsigned long	AUOSL5		:1;
		unsigned long	AUOSR5		:1;
		unsigned long	AUOSL6		:1;
		unsigned long	AUOSR6		:1;
		unsigned long	AUOSL7		:1;
		unsigned long	AUOSR7		:1;
		unsigned long	AUOSL8		:1;
		unsigned long	AUOSR8		:1;
		unsigned long	AUOSL9		:1;
		unsigned long	AUOSR9		:1;
		unsigned long	AUOSL10 	:1;
		unsigned long	AUOSR10 	:1;
		unsigned long	AUOSL11 	:1;
		unsigned long	AUOSR11 	:1;
		unsigned long	AUOSL12 	:1;
		unsigned long	AUOSR12 	:1;
		unsigned long	AUOSL13 	:1;
		unsigned long	AUOSR13 	:1;
		unsigned long	AUOSL14 	:1;
		unsigned long	AUOSR14 	:1;
		unsigned long	AUOSL15 	:1;
		unsigned long	AUOSR15 	:1;
	}bit;
};

/*	structure of AUDMA2CTL	*/
union io_audio_audma2ctl{
	unsigned long		word;
	struct {
		unsigned long	DMI2CHEN	:1;
		unsigned long				:7;
		unsigned long	DMO2CHEN	:1;
		unsigned long				:23;
	}bit;
};

/*	structure of AUDMISAMPLE	*/
union io_audio_audmisample{
	unsigned long		word;
	struct {
		unsigned long	DMISPL		:16;
		unsigned long				:16;
	}bit;
};

/*	structure of AUDMOSAMPLE	*/
union io_audio_audmosample{
	unsigned long		word;
	struct {
		unsigned long	DMOSPL		:16;
		unsigned long				:16;
	}bit;
};

/* AudioIF Control	(1E20_(0000 - 0FFh)*/
struct io_audioifctrl {
	union io_audio_aures		AURES;											/* 1E20_(0000 - 0003h)	*/
	unsigned char				dmy_1E200004_1E200007[0x1E200008 - 0x1E200004]; /* 1E20_(0004 - 0007h)	*/
	unsigned char				dmy_1E200008_1E20000B[0x1E20000C - 0x1E200008]; /* 1E20_(0008 - 000Bh)	*/
	union io_audio_auioe		AUIOE;											/* 1E20_(000C - 000Fh)	*/
	unsigned char				dmy_1E200010_1E200013[0x1E200014 - 0x1E200010]; /* 1E20_(0010 - 0013h)	*/
	union io_audio_aulbe		AULBE;											/* 1E20_(0014 - 0017h)	*/
	unsigned char				dmy_1E200018_1E2000FF[0x1E200100 - 0x1E200018]; /* 1E20_(0018 - 00FFh)	*/
};

/* AudioIF ch0	(1E20_(0100 - 1FFh)*/
/* AudioIF ch1	(1E20_(0200 - 2FFh)*/
/* AudioIF ch2	(1E20_(0300 - 3FFh)*/
/* AudioIF ch3	(1E20_(0400 - 4FFh)*/
/* AudioIF ch4	(1E20_(0500 - 5FFh)*/
/* AudioIF ch5	(1E20_(0600 - 6FFh)*/
struct io_audioif {
	unsigned long				AUIDLR;											/* 1E20_(0100 - 0103h)	*/
	unsigned long				AUODLR;											/* 1E20_(0104 - 0107h)	*/
	union io_audio_aucr			AUCR;											/* 1E20_(0108 - 010Bh)	*/
	union io_audio_aumd			AUMD;											/* 1E20_(010C - 010Fh)	*/
	union io_audio_aust			AUST;											/* 1E20_(0110 - 0113h)	*/
	union io_audio_aucc			AUCC;											/* 1E20_(0114 - 0117h)	*/
	union io_audio_audp			AUDP;											/* 1E20_(0118 - 011Bh)	*/
	union io_audio_auifst		AUIFST;											/* 1E20_(011C - 011Fh)	*/
	union io_audio_auofst		AUOFST;											/* 1E20_(0120 - 0123h)	*/
	unsigned char				dmy_1E200124_1E200127[0x1E200128 - 0x1E200124]; /* 1E20_(0124 - 0127h)	*/
	unsigned long				AUIDL; 											/* 1E20_(0128 - 012Bh)	*/
	unsigned long				AUIDR; 											/* 1E20_(012C - 012Fh)	*/
	unsigned long				AUODL; 											/* 1E20_(0130 - 0133h)	*/
	unsigned long				AUODR; 											/* 1E20_(0134 - 0137h)	*/
	unsigned long				AUIDDMAPT; 										/* 1E20_(0138 - 013Bh)	*/
	unsigned long				AUODDMAPT; 										/* 1E20_(013C - 013Fh)	*/
	union io_audio_audma2ctl 	AUDMA2CTL; 										/* 1E20_(0140 - 0143h)	*/
	union io_audio_audmisample 	AUDMISAMPLE;									/* 1E20_(0144 - 0147h)	*/
	union io_audio_audmosample 	AUDMOSAMPLE;									/* 1E20_(0148 - 014Bh)	*/
	unsigned char				dmy_1E20014C_1E2001FF[0x1E200200 - 0x1E20014C]; /* 1E20_(014C - 01FFh)	*/
};

/* Define I/O Mapping Audio */
struct io_audio {
	struct io_audioifctrl		AUDIOIF_CTRL;									/* 1E20_(0000 - 00FFh)	*/
	struct io_audioif			AUDIOIF[6]; 									/* 1E20_(0100 - 06FFh)	*/
};


extern volatile struct io_audio 	IO_AUDIO;		/* addr 1E200000h-	*/

