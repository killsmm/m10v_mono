#ifndef __JDSCNR_OFL_REG_YSPR_H__
#define __JDSCNR_OFL_REG_YSPR_H__


/*	structure of YSPREN (2842_4000h)	*/
union io_cnr_ofl_reg_yspr_yspren{
	unsigned long		word;
	struct {
		unsigned long	YSPREN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of YSPRMODE	(2842_4004h)	*/
union io_cnr_ofl_reg_yspr_ysprmode{
	unsigned long		word;
	struct {
		unsigned long	YSPRMD	:1;
		unsigned long			:3;
		unsigned long	YSPRFE	:2;
		unsigned long			:26;
	}bit;
};

/*	structure of YSPRFXEY	(2842_4020h)	*/
union io_cnr_ofl_reg_yspr_ysprfxey{
	unsigned long		word;
	struct {
		unsigned long	YSPRFXEYH	:8;
		unsigned long				:8;
		unsigned long	YSPRFXEYV	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of YSPRFXEC	(2842_4024h)	*/
union io_cnr_ofl_reg_yspr_ysprfxec{
	unsigned long		word;
	struct {
		unsigned long	YSPRFXECH	:8;
		unsigned long				:8;
		unsigned long	YSPRFXECV	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of YSPRALPBD	(2842_4028h)	*/
union io_cnr_ofl_reg_yspr_yspralpbd{
	unsigned long		word;
	struct {
		unsigned long	YSPRALPBD	:5;
		unsigned long				:27;
	}bit;
};

/*	structure of YSPRDYEYOF (2842_4030h)	*/
union io_cnr_ofl_reg_yspr_ysprdyeyof_1{
	unsigned long		word;
	struct {
		unsigned long	YSPRDYEYOF_0	:8;
		unsigned long	YSPRDYEYOF_1	:8;
		unsigned long	YSPRDYEYOF_2	:8;
		unsigned long	YSPRDYEYOF_3	:8;
	}bit;
};
union io_cnr_ofl_reg_yspr_ysprdyeyof_2{
	unsigned long		word;
	struct {
		unsigned long	YSPRDYEYOF_4	:8;
		unsigned long	YSPRDYEYOF_5	:8;
		unsigned long					:16;
	}bit;
};
struct io_cnr_ofl_reg_yspr_ysprdyeyof{
	union io_cnr_ofl_reg_yspr_ysprdyeyof_1	YSPRDYEYOF1;
	union io_cnr_ofl_reg_yspr_ysprdyeyof_2	YSPRDYEYOF2;
};

/*	structure of YSPRDYEYGA (2842_4038h)	*/
union io_cnr_ofl_reg_yspr_ysprdyeyga_1{
	unsigned long		word;
	struct {
		long			YSPRDYEYGA_0	:8;
		long			YSPRDYEYGA_1	:8;
		long			YSPRDYEYGA_2	:8;
		long			YSPRDYEYGA_3	:8;
	}bit;
};
union io_cnr_ofl_reg_yspr_ysprdyeyga_2{
	unsigned long		word;
	struct {
		long			YSPRDYEYGA_4	:8;
		long			YSPRDYEYGA_5	:8;
		unsigned long					:16;
	}bit;
};
struct io_cnr_ofl_reg_yspr_ysprdyeyga{
	union io_cnr_ofl_reg_yspr_ysprdyeyga_1	YSPRDYEYGA1;
	union io_cnr_ofl_reg_yspr_ysprdyeyga_2	YSPRDYEYGA2;
};

/*	structure of YSPRDYEYBD (2842_4040h)	*/
union io_cnr_ofl_reg_yspr_ysprdyeybd_1{
	unsigned long		word;
	struct {
		unsigned long	YSPRDYEYBD_1	:8;
		unsigned long	YSPRDYEYBD_2	:8;
		unsigned long	YSPRDYEYBD_3	:8;
		unsigned long	YSPRDYEYBD_4	:8;
	}bit;
};
union io_cnr_ofl_reg_yspr_ysprdyeybd_2{
	unsigned long		word;
	struct {
		unsigned long	YSPRDYEYBD_5	:8;
		unsigned long					:24;
	}bit;
};
struct io_cnr_ofl_reg_yspr_ysprdyeybd{
	union io_cnr_ofl_reg_yspr_ysprdyeybd_1	YSPRDYEYBD1;
	union io_cnr_ofl_reg_yspr_ysprdyeybd_2	YSPRDYEYBD2;
};

/* Define i/o mapping */
struct io_cnr_ofl_reg_yspr{
	/* JDSIMG */
	union  io_cnr_ofl_reg_yspr_yspren		YSPREN;		/* 2842_(4000 - 4003h) */
	union  io_cnr_ofl_reg_yspr_ysprmode		YSPRMODE;	/* 2842_(4004 - 4007h) */

	unsigned char dmy_4008_401F[0x4020-0x4008];			/* 2842_(4008 - 401Fh) */

	union  io_cnr_ofl_reg_yspr_ysprfxey		YSPRFXEY;	/* 2842_(4020 - 4023h) */
	union  io_cnr_ofl_reg_yspr_ysprfxec		YSPRFXEC;	/* 2842_(4024 - 4027h) */
	union  io_cnr_ofl_reg_yspr_yspralpbd	YSPRALPBD;	/* 2842_(4028 - 402Bh) */

	unsigned char dmy_402C_402F[0x4030-0x402C];			/* 2842_(402C - 402Fh) */

	struct io_cnr_ofl_reg_yspr_ysprdyeyof	YSPRDYEYOF;	/* 2842_(4030 - 4037h) */
	struct io_cnr_ofl_reg_yspr_ysprdyeyga	YSPRDYEYGA;	/* 2842_(4038 - 403Fh) */
	struct io_cnr_ofl_reg_yspr_ysprdyeybd	YSPRDYEYBD;	/* 2842_(4040 - 4047h) */

	unsigned char dmy_4048_4FFF[0x5000-0x4048];			/* 2842_(4048 - 4FFFh) */

};

#endif	// #define __JDSCNR_OFL_REG_YSPR_H__
