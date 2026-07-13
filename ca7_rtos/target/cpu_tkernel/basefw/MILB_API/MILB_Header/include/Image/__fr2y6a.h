/**
 * @file		__fr2y6a.h
 * @brief		Definition FR2Y6A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*	structure of CNTL	(2841_8000h)	*/
union io_r2y_cntl{
	unsigned long		word;
	struct {
		unsigned long		:8;
		unsigned long	TRG	:2;
		unsigned long		:22;
	}bit;
};

/*	structure of R2YFLAG	(2841_8004h)	*/
union io_r2y_r2yflag{
	unsigned long		word;
	struct {
		unsigned long	TCACT	:1;
		unsigned long	GAMACT	:1;
		unsigned long	YCFACT	:1;
		unsigned long			:5;
		unsigned long	TCHACT	:1;
		unsigned long	TCTACT	:1;
		unsigned long			:22;
	}bit;
};

/*	structure of R2YINTE	(2841_8008h)	*/
union io_r2y_r2yinte{
	unsigned long		word;
	struct {
		unsigned long	YCFERE	:1;
		unsigned long			:3;
		unsigned long	TCTEE	:1;
		unsigned long			:3;
		unsigned long	TCHSEE	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of R2YINTF	(2841_800Ch)	*/
union io_r2y_r2yintf{
	unsigned long		word;
	struct {
		unsigned long	__YCFERF	:1;
		unsigned long				:3;
		unsigned long	__TCTEF		:1;
		unsigned long				:3;
		unsigned long	__TCHSEF	:1;
		unsigned long				:23;
	}bit;
};

/*	structure of FLTAEN	(2841_8010h)	*/
union io_r2y_fltaen{
	unsigned long		word;
	struct {
		unsigned long	YCFAEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of OFST	(2841_8020h)	*/
union io_r2y_ofst{
	unsigned long		word[2];
	struct {
		long			OFSTR	:10;
		unsigned long			:6;
		long			OFSTG	:10;
		unsigned long			:6;
		long			OFSTB	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of WBGAR	(2841_8028h)	*/
union io_r2y_wbgar{
	unsigned long		word[2];
	struct {
		unsigned long	WBGAR	:11;
		unsigned long			:5;
		unsigned long	WBGAG	:11;
		unsigned long			:5;
		unsigned long	WBGAB	:11;
		unsigned long			:21;
	}bit;
};

/*	structure of WBLV	(2841_8030h)	*/
union io_r2y_wblv{
	unsigned long		word[2];
	struct {
		unsigned long	WBLVR	:12;
		unsigned long			:4;
		unsigned long	WBLVG	:12;
		unsigned long			:4;
		unsigned long	WBLVB	:12;
		unsigned long			:20;
	}bit;
};

/*	structure of CC0CTL	(2841_8080h)	*/
union io_r2y_cc0ctl{
	unsigned long		word;
	struct {
		unsigned long	CC0DP	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of CC0K	(2841_80A0h)	*/
union io_r2y_cc0k{
	unsigned long		word[5];
	struct {
		long			CC0K_0_0	:12;
		unsigned long				:4;
		long			CC0K_0_1	:12;
		unsigned long				:4;
		long			CC0K_0_2	:12;
		unsigned long				:4;
		long			CC0K_1_0	:12;
		unsigned long				:4;
		long			CC0K_1_1	:12;
		unsigned long				:4;
		long			CC0K_1_2	:12;
		unsigned long				:4;
		long			CC0K_2_0	:12;
		unsigned long				:4;
		long			CC0K_2_1	:12;
		unsigned long				:4;
		long			CC0K_2_2	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of CC0YBOF	(2841_80C0h)	*/
union io_r2y_cc0ybof{
	unsigned long		word[2];
	struct {
		unsigned long	CC0YBOF_0	:11;
		unsigned long				:5;
		unsigned long	CC0YBOF_1	:11;
		unsigned long				:5;
		unsigned long	CC0YBOF_2	:11;
		unsigned long				:21;
	}bit;
};

/*	structure of CC0YBGA	(2841_80C8h)	*/
union io_r2y_cc0ybga{
	unsigned long		word[2];
	struct {
		long			CC0YBGA_0	:9;
		unsigned long				:7;
		long			CC0YBGA_1	:9;
		unsigned long				:7;
		long			CC0YBGA_2	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of CC0YBBD	(2841_80D0h)	*/
union io_r2y_cc0ybbd{
	unsigned long		word;
	struct {
		unsigned long	CC0YBBD_1	:12;
		unsigned long				:4;
		unsigned long	CC0YBBD_2	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of CCYC	(2841_80D4h)	*/
union io_r2y_ccyc{
	unsigned long		word;
	struct {
		unsigned long	CCYC_0_0	:8;
		unsigned long	CCYC_0_1	:8;
		unsigned long	CCYC_0_2	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of MCYC	(2841_9000h)	*/
union io_r2y_mcyc{
	unsigned long		word[5];
	struct {
		long			MCYC_0_0	:9;
		unsigned long				:7;
		long			MCYC_0_1	:9;
		unsigned long				:7;
		long			MCYC_0_2	:9;
		unsigned long				:7;
		long			MCYC_1_0	:9;
		unsigned long				:7;
		long			MCYC_1_1	:9;
		unsigned long				:7;
		long			MCYC_1_2	:9;
		unsigned long				:7;
		long			MCYC_2_0	:9;
		unsigned long				:7;
		long			MCYC_2_1	:9;
		unsigned long				:7;
		long			MCYC_2_2	:9;
		unsigned long				:23;
	}bit;
};

/*	structure of MCB1AB	(2841_9020h)	*/
union io_r2y_mcb1ab{
	unsigned long		word;
	struct {
		unsigned long	MCB1A	:12;
		unsigned long			:4;
		unsigned long	MCB1B	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of MCB1CD	(2841_9024h)	*/
union io_r2y_mcb1cd{
	unsigned long		word;
	struct {
		unsigned long	MCB1C	:12;
		unsigned long			:4;
		unsigned long	MCB1D	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of MCB2AB	(2841_9028h)	*/
union io_r2y_mcb2ab{
	unsigned long		word;
	struct {
		unsigned long	MCB2A	:12;
		unsigned long			:4;
		unsigned long	MCB2B	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of MCB2CD	(2841_902Ch)	*/
union io_r2y_mcb2cd{
	unsigned long		word;
	struct {
		unsigned long	MCB2C	:12;
		unsigned long			:4;
		unsigned long	MCB2D	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of MCB3AB	(2841_9030h)	*/
union io_r2y_mcb3ab{
	unsigned long		word;
	struct {
		unsigned long	MCB3A	:12;
		unsigned long			:4;
		unsigned long	MCB3B	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of MCB3CD	(2841_9034h)	*/
union io_r2y_mcb3cd{
	unsigned long		word;
	struct {
		unsigned long	MCB3C	:12;
		unsigned long			:4;
		unsigned long	MCB3D	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of MCB4AB	(2841_9038h)	*/
union io_r2y_mcb4ab{
	unsigned long		word;
	struct {
		unsigned long	MCB4A	:12;
		unsigned long			:4;
		unsigned long	MCB4B	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of MCB4CD	(2841_903Ch)	*/
union io_r2y_mcb4cd{
	unsigned long		word;
	struct {
		unsigned long	MCB4C	:12;
		unsigned long			:4;
		unsigned long	MCB4D	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of MCID1	(2841_9040h)	*/
union io_r2y_mcid1{
	unsigned long		word;
	struct {
		unsigned long	MCID1A	:4;
		unsigned long	MCID1B	:4;
		unsigned long	MCID1C	:4;
		unsigned long	MCID1D	:4;
		unsigned long	MCID1E	:4;
		unsigned long			:12;
	}bit;
};

/*	structure of MCID2	(2841_9044h)	*/
union io_r2y_mcid2{
	unsigned long		word;
	struct {
		unsigned long	MCID2A	:4;
		unsigned long	MCID2B	:4;
		unsigned long	MCID2C	:4;
		unsigned long	MCID2D	:4;
		unsigned long	MCID2E	:4;
		unsigned long			:12;
	}bit;
};

/*	structure of MCID3	(2841_9048h)	*/
union io_r2y_mcid3{
	unsigned long		word;
	struct {
		unsigned long	MCID3A	:4;
		unsigned long	MCID3B	:4;
		unsigned long	MCID3C	:4;
		unsigned long	MCID3D	:4;
		unsigned long	MCID3E	:4;
		unsigned long			:12;
	}bit;
};

/*	structure of MCID4	(2841_904Ch)	*/
union io_r2y_mcid4{
	unsigned long		word;
	struct {
		unsigned long	MCID4A	:4;
		unsigned long	MCID4B	:4;
		unsigned long	MCID4C	:4;
		unsigned long	MCID4D	:4;
		unsigned long	MCID4E	:4;
		unsigned long			:12;
	}bit;
};

/*	structure of MCKA	(2841_9080h)	*/
/*	structure of MCKB	(2841_9100h)	*/
/*	structure of MCKC	(2841_9180h)	*/
/*	structure of MCKD	(2841_9200h)	*/
/*	structure of MCKE	(2841_9280h)	*/
/*	structure of MCKF	(2841_9300h)	*/
/*	structure of MCKG	(2841_9380h)	*/
/*	structure of MCKH	(2841_9400h)	*/
/*	structure of MCKI	(2841_9480h)	*/
/*	structure of MCKJ	(2841_9500h)	*/
/*	structure of MCKK	(2841_9580h)	*/
/*	structure of MCKL	(2841_9600h)	*/
union io_r2y_mck_x{
	unsigned long		word[5];
	struct {
		long			MCK_0_0		:12;
		unsigned long				:4;
		long			MCK_0_1		:12;
		unsigned long				:4;
		long			MCK_0_2		:12;
		unsigned long				:4;
		long			MCK_1_0		:12;
		unsigned long				:4;
		long			MCK_1_1		:12;
		unsigned long				:4;
		long			MCK_1_2		:12;
		unsigned long				:4;
		long			MCK_2_0		:12;
		unsigned long				:4;
		long			MCK_2_1		:12;
		unsigned long				:4;
		long			MCK_2_2		:12;
		unsigned long				:20;
	}bit;
};
struct io_r2y_mck{
	union	io_r2y_mck_x	MCK_0;
	union	io_r2y_mck_x	MCK_1;
	union	io_r2y_mck_x	MCK_2;
	union	io_r2y_mck_x	MCK_3;
	union	io_r2y_mck_x	MCK_4;
};

/*	structure of MCLA	(2841_9680h)	*/
/*	structure of MCLB	(2841_96C0h)	*/
/*	structure of MCLC	(2841_9700h)	*/
/*	structure of MCLD	(2841_9740h)	*/
/*	structure of MCLE	(2841_9780h)	*/
/*	structure of MCLF	(2841_97C0h)	*/
/*	structure of MCLG	(2841_9800h)	*/
/*	structure of MCLH	(2841_9840h)	*/
/*	structure of MCLI	(2841_9880h)	*/
/*	structure of MCLJ	(2841_98C0h)	*/
/*	structure of MCLK	(2841_9900h)	*/
/*	structure of MCLL	(2841_9940h)	*/
union io_r2y_mcl_x{
	unsigned long		word[3];
	struct {
		long			MCL_0_0		:10;
		unsigned long				:6;
		long			MCL_0_1		:10;
		unsigned long				:6;
		long			MCL_1_0		:10;
		unsigned long				:6;
		long			MCL_1_1		:10;
		unsigned long				:6;
		long			MCL_2_0		:10;
		unsigned long				:6;
		long			MCL_2_1		:10;
		unsigned long				:6;
	}bit;
};
struct io_r2y_mcl{
	union	io_r2y_mcl_x	MCL_0;
	union	io_r2y_mcl_x	MCL_1;
	union	io_r2y_mcl_x	MCL_2;
	union	io_r2y_mcl_x	MCL_3;
	union	io_r2y_mcl_x	MCL_4;
};

/*	structure of MCYCBALP	(2841_9980h)	*/
union io_r2y_mcycbalp{
	unsigned long		word;
	struct {
		unsigned long	MCYCBALP	:6;
		unsigned long				:26;
	}bit;
};

/*	structure of MCYCBGA	(2841_9988h)	*/
union io_r2y_mcycbga{
	unsigned long		word[2];
	struct {
		unsigned long	MCYCBGA_1	:12;
		unsigned long				:4;
		unsigned long	MCYCBGA_2	:12;
		unsigned long				:4;
		unsigned long	MCYCBGA_3	:12;
		unsigned long				:4;
		unsigned long	MCYCBGA_4	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of MCYCBBD	(2841_9990h)	*/
union io_r2y_mcycbbd{
	unsigned long		word[2];
	struct {
		unsigned long	MCYCBBD_1	:12;
		unsigned long				:4;
		unsigned long	MCYCBBD_2	:12;
		unsigned long				:4;
		unsigned long	MCYCBBD_3	:12;
		unsigned long				:4;
		unsigned long	MCYCBBD_4	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of MCBABALP	(2841_9998h)	*/
union io_r2y_mcbabalp{
	unsigned long		word;
	struct {
		unsigned long	MCBABALP	:6;
		unsigned long				:26;
	}bit;
};

/*	structure of MCBABOF	(2841_99A0h)	*/
union io_r2y_mcbabof{
	unsigned long		word[2];
	struct {
		unsigned long	MCBABOF_0	:11;
		unsigned long				:5;
		unsigned long	MCBABOF_1	:11;
		unsigned long				:5;
		unsigned long	MCBABOF_2	:11;
		unsigned long				:5;
		unsigned long	MCBABOF_3	:11;
		unsigned long				:5;
	}bit;
};

/*	structure of MCBABGA	(2841_99A8h)	*/
union io_r2y_mcbabga{
	unsigned long		word[2];
	struct {
		long			MCBABGA_0	:9;
		unsigned long				:7;
		long			MCBABGA_1	:9;
		unsigned long				:7;
		long			MCBABGA_2	:9;
		unsigned long				:7;
		long			MCBABGA_3	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of MCBABBD	(2841_99B0h)	*/
union io_r2y_mcbabbd{
	unsigned long		word[2];
	struct {
		unsigned long	MCBABBD_1	:12;
		unsigned long				:4;
		unsigned long	MCBABBD_2	:12;
		unsigned long				:4;
		unsigned long	MCBABBD_3	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of TCOF	(2841_A000h)	*/
union io_r2y_tcof{
	unsigned long		word[2];
	struct {
		long			TCOFR	:10;
		unsigned long			:6;
		long			TCOFG	:10;
		unsigned long			:6;
		long			TCOFB	:10;
		unsigned long			:6;
		long			TCOFYB	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of TCTCTL	(2841_A008h)	*/
union io_r2y_tctctl{
	unsigned long		word;
	struct {
		unsigned long	TCTEN	:1;
		unsigned long			:3;
		unsigned long	TCTAEN	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of TCTSTA	(2841_A00Ch)	*/
union io_r2y_tctsta{
	unsigned long		word;
	struct {
		unsigned long	TCTHSTA	:12;
		unsigned long			:4;
		unsigned long	TCTVSTA	:14;
		unsigned long			:2;
	}bit;
};

/*	structure of TCTB	(2841_A010h)	*/
union io_r2y_tctb{
	unsigned long		word;
	struct {
		unsigned long	TCTBHSIZ	:8;
		unsigned long	TCTBVSIZ	:8;
		unsigned long	TCTBHNUM	:6;
		unsigned long				:2;
		unsigned long	TCTBVNUM	:5;
		unsigned long				:3;
	}bit;
};

/*	structure of TCHSCTL	(2841_A014h)	*/
union io_r2y_tchsctl{
	unsigned long		word;
	struct {
		unsigned long	TCHSEN		:1;
		unsigned long				:3;
		unsigned long	TCHAEN		:1;
		unsigned long				:3;
		unsigned long	TCHSHCYC	:2;
		unsigned long				:2;
		unsigned long	TCHSVCYC	:3;
		unsigned long				:1;
		unsigned long	TCHSRGBMD	:3;
		unsigned long				:1;
		unsigned long	TCHSMN		:1;
		unsigned long				:11;
	}bit;
};

/*	structure of TCHSSTA	(2841_A018h)	*/
union io_r2y_tchssta{
	unsigned long		word;
	struct {
		unsigned long	TCHSHSTA	:12;
		unsigned long				:4;
		unsigned long	TCHSVSTA	:14;
		unsigned long				:2;
	}bit;
};

/*	structure of TCHSSIZ	(2841_A01Ch)	*/
union io_r2y_tchssiz{
	unsigned long		word;
	struct {
		unsigned long	TCHSHSIZ	:12;
		unsigned long				:4;
		unsigned long	TCHSVSIZ	:14;
		unsigned long				:2;
	}bit;
};

/*	structure of TCHOV	(2841_A020h)	*/
union io_r2y_tchov{
	unsigned long		word;
	struct {
		unsigned long	TCHYOV	:1;
		unsigned long			:3;
		unsigned long	TCHROV	:1;
		unsigned long			:3;
		unsigned long	TCHGOV	:1;
		unsigned long			:3;
		unsigned long	TCHBOV	:1;
		unsigned long			:19;
	}bit;
};

/*	structure of TCCTL	(2841_A040h)	*/
union io_r2y_tcctl{
	unsigned long		word;
	struct {
		unsigned long	TCEN		:1;
		unsigned long	TCYBEN		:1;
		unsigned long	TCBLEN		:1;
		unsigned long				:1;
		unsigned long	TCAEN		:1;
		unsigned long				:3;
		unsigned long	TCRES		:2;
		unsigned long				:2;
		unsigned long	TCTBL		:2;
		unsigned long				:2;
		unsigned long	TCYOUT		:1;
		unsigned long				:3;
		unsigned long	TCINTBIT	:2;
		unsigned long				:2;
		unsigned long	TCBLND		:5;
		unsigned long				:3;
	}bit;
};

/*	structure of TCYC	(2841_A044h)	*/
union io_r2y_tcyc{
	unsigned long		word;
	struct {
		unsigned long	TCYC_0_0	:8;
		unsigned long	TCYC_0_1	:8;
		unsigned long	TCYC_0_2	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of TCEP	(2841_A048h)	*/
union io_r2y_tcep{
	unsigned long		word[2];
	struct {
		unsigned long	TCEP_0	:15;
		unsigned long			:1;
		unsigned long	TCEP_1	:15;
		unsigned long			:1;
		unsigned long	TCEP_2	:15;
		unsigned long			:1;
		unsigned long	TCEP_3	:15;
		unsigned long			:1;
	}bit;
};

/*	structure of TCCLPR	(2841_A050h)	*/
union io_r2y_tcclpr{
	unsigned long		word;
	struct {
		unsigned long	TCCLPRP	:12;
		unsigned long			:4;
		unsigned long	TCCLPRM	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of TCCLPG	(2841_A054h)	*/
union io_r2y_tcclpg{
	unsigned long		word;
	struct {
		unsigned long	TCCLPGP	:12;
		unsigned long			:4;
		unsigned long	TCCLPGM	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of TCCLPB	(2841_A058h)	*/
union io_r2y_tcclpb{
	unsigned long		word;
	struct {
		unsigned long	TCCLPBP	:12;
		unsigned long			:4;
		unsigned long	TCCLPBM	:12;
		unsigned long			:4;
	}bit;
};

/*	structure of GMCTL	(2841_A060h)	*/
union io_r2y_gmctl{
	unsigned long		word;
	struct {
		unsigned long	GMEN	:1;
		unsigned long	GMMD	:1;
		unsigned long			:2;
		unsigned long	GMAEN	:1;
		unsigned long	GMYAEN	:1;
		unsigned long			:2;
		unsigned long	GAMSW	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of CC1CTL	(2841_A080h)	*/
union io_r2y_cc1ctl{
	unsigned long		word;
	struct {
		unsigned long	CC1DP	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of CC1K	(2841_A0A0h)	*/
union io_r2y_cc1k{
	unsigned long		word[5];
	struct {
		long			CC1K_0_0	:12;
		unsigned long				:4;
		long			CC1K_0_1	:12;
		unsigned long				:4;
		long			CC1K_0_2	:12;
		unsigned long				:4;
		long			CC1K_1_0	:12;
		unsigned long				:4;
		long			CC1K_1_1	:12;
		unsigned long				:4;
		long			CC1K_1_2	:12;
		unsigned long				:4;
		long			CC1K_2_0	:12;
		unsigned long				:4;
		long			CC1K_2_1	:12;
		unsigned long				:4;
		long			CC1K_2_2	:12;
		unsigned long				:20;
	}bit;
};

/*	structure of CC1CLPR	(2841_A0C0h)	*/
union io_r2y_cc1clpr{
	unsigned long		word;
	struct {
		unsigned long	CC1CLPRP	:12;
		unsigned long				:4;
		unsigned long	CC1CLPRM	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of CC1CLPG	(2841_A0C4h)	*/
union io_r2y_cc1clpg{
	unsigned long		word;
	struct {
		unsigned long	CC1CLPGP	:12;
		unsigned long				:4;
		unsigned long	CC1CLPGM	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of CC1CLPB	(2841_A0C8h)	*/
union io_r2y_cc1clpb{
	unsigned long		word;
	struct {
		unsigned long	CC1CLPBP	:12;
		unsigned long				:4;
		unsigned long	CC1CLPBM	:12;
		unsigned long				:4;
	}bit;
};

/*	structure of YC	(2841_A100h)	*/
union io_r2y_yc{
	unsigned long		word[5];
	struct {
		long			YC_0_0	:9;
		unsigned long			:7;
		long			YC_0_1	:9;
		unsigned long			:7;
		long			YC_0_2	:9;
		unsigned long			:7;
		long			YC_1_0	:9;
		unsigned long			:7;
		long			YC_1_1	:9;
		unsigned long			:7;
		long			YC_1_2	:9;
		unsigned long			:7;
		long			YC_2_0	:9;
		unsigned long			:7;
		long			YC_2_1	:9;
		unsigned long			:7;
		long			YC_2_2	:9;
		unsigned long			:23;
	}bit;
};

/*	structure of YBLEND	(2841_A120h)	*/
union io_r2y_yblend{
	unsigned long		word;
	struct {
		unsigned long	YYBLND	:6;
		unsigned long			:2;
		unsigned long	YBBLND	:6;
		unsigned long			:18;
	}bit;
};

/*	structure of NRCTL	(2841_A140h)	*/
union io_r2y_nrctl{
	unsigned long		word;
	struct {
		unsigned long	NRMD	:2;
		unsigned long			:2;
		unsigned long	NRBLEND	:3;
		unsigned long			:25;
	}bit;
};

/*	structure of NROF	(2841_A148h)	*/
union io_r2y_nrof{
	unsigned long		word[2];
	struct {
		unsigned long	NROF_0	:10;
		unsigned long			:6;
		unsigned long	NROF_1	:10;
		unsigned long			:6;
		unsigned long	NROF_2	:10;
		unsigned long			:6;
		unsigned long	NROF_3	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of NRGA	(2841_A150h)	*/
union io_r2y_nrga{
	unsigned long		word[2];
	struct {
		long			NRGA_0	:11;
		unsigned long			:5;
		long			NRGA_1	:11;
		unsigned long			:5;
		long			NRGA_2	:11;
		unsigned long			:5;
		long			NRGA_3	:11;
		unsigned long			:5;
	}bit;
};

/*	structure of NRBD	(2841_A158h)	*/
union io_r2y_nrbd{
	unsigned long		word[2];
	struct {
		unsigned long	NRBD_1	:10;
		unsigned long			:6;
		unsigned long	NRBD_2	:10;
		unsigned long			:6;
		unsigned long	NRBD_3	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of EGSMCTL	(2841_A180h)	*/
union io_r2y_egsmctl{
	unsigned long		word;
	struct {
		unsigned long	EGSMT	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of EGSMTT	(2841_A184h)	*/
union io_r2y_egsmtt{
	unsigned long		word;
	struct {
		unsigned long	EGSMTTH		:12;
		unsigned long				:4;
		unsigned long	EGSMTTXGA	:7;
		unsigned long				:9;
	}bit;
};

/*	structure of EGHWCTL	(2841_A200h)	*/
union io_r2y_eghwctl{
	unsigned long		word;
	struct {
		unsigned long	EGHWEN		:1;
		unsigned long	EGHWMP		:1;
		unsigned long	EGHWTC		:1;
		unsigned long				:5;
		unsigned long	EHSAEN		:1;
		unsigned long	EHTAEN		:1;
		unsigned long				:6;
		unsigned long	EGHWSCLRES	:2;
		unsigned long	EGHWSCLTBL	:2;
		unsigned long	EGHWTCRES	:2;
		unsigned long	EGHWTCTBL	:2;
		unsigned long	EGHWNRLV	:4;
		unsigned long				:4;
	}bit;
};

/*	structure of EGHWHPFK	(2841_A208h)	*/
union io_r2y_eghwhpfk{
	unsigned long		word[2];
	struct {
		unsigned long	EGHWHPFK0	:10;
		unsigned long				:6;
		long			EGHWHPFK1	:8;
		long			EGHWHPFK2	:8;
		long			EGHWHPFK3	:8;
		long			EGHWHPFK4	:8;
		long			EGHWHPFK5	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EGHWCORPOF	(2841_A210h)	*/
union io_r2y_eghwcorpof{
	unsigned long		word[2];
	struct {
		unsigned long	EGHWCORPOF_0	:10;
		unsigned long					:6;
		unsigned long	EGHWCORPOF_1	:10;
		unsigned long					:6;
		unsigned long	EGHWCORPOF_2	:10;
		unsigned long					:6;
		unsigned long	EGHWCORPOF_3	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGHWCORPGA	(2841_A218h)	*/
union io_r2y_eghwcorpga{
	unsigned long		word;
	struct {
		long			EGHWCORPGA_0	:8;
		long			EGHWCORPGA_1	:8;
		long			EGHWCORPGA_2	:8;
		long			EGHWCORPGA_3	:8;
	}bit;
};

/*	structure of EGHWCORPBD	(2841_A220h)	*/
union io_r2y_eghwcorpbd{
	unsigned long		word[2];
	struct {
		unsigned long	EGHWCORPBD_1	:10;
		unsigned long					:6;
		unsigned long	EGHWCORPBD_2	:10;
		unsigned long					:6;
		unsigned long	EGHWCORPBD_3	:10;
		unsigned long					:22;
	}bit;
};

/*	structure of EGHWCORMOF	(2841_A228h)	*/
union io_r2y_eghwcormof{
	unsigned long		word[2];
	struct {
		unsigned long	EGHWCORMOF_0	:10;
		unsigned long					:6;
		unsigned long	EGHWCORMOF_1	:10;
		unsigned long					:6;
		unsigned long	EGHWCORMOF_2	:10;
		unsigned long					:6;
		unsigned long	EGHWCORMOF_3	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGHWCORMGA	(2841_A230h)	*/
union io_r2y_eghwcormga{
	unsigned long		word;
	struct {
		long			EGHWCORMGA_0	:8;
		long			EGHWCORMGA_1	:8;
		long			EGHWCORMGA_2	:8;
		long			EGHWCORMGA_3	:8;
	}bit;
};

/*	structure of EGHWCORMBD	(2841_A238h)	*/
union io_r2y_eghwcormbd{
	unsigned long		word[2];
	struct {
		unsigned long	EGHWCORMBD_1	:10;
		unsigned long					:6;
		unsigned long	EGHWCORMBD_2	:10;
		unsigned long					:6;
		unsigned long	EGHWCORMBD_3	:10;
		unsigned long					:22;
	}bit;
};

/*	structure of EGHWSCLSUP	(2841_A240h)	*/
union io_r2y_eghwsclsup{
	unsigned long		word;
	struct {
		unsigned long	EGHWSCLSUP	:6;
		unsigned long				:26;
	}bit;
};

/*	structure of EGHWCLPPOF	(2841_A248h)	*/
union io_r2y_eghwclppof{
	unsigned long		word[2];
	struct {
		unsigned long	EGHWCLPPOF_0	:9;
		unsigned long					:7;
		unsigned long	EGHWCLPPOF_1	:9;
		unsigned long					:7;
		unsigned long	EGHWCLPPOF_2	:9;
		unsigned long					:23;
	}bit;
};

/*	structure of EGHWCLPPGA	(2841_A250h)	*/
union io_r2y_eghwclppga{
	unsigned long		word;
	struct {
		long			EGHWCLPPGA_0	:8;
		long			EGHWCLPPGA_1	:8;
		long			EGHWCLPPGA_2	:8;
		unsigned long					:8;
	}bit;
};

/*	structure of EGHWCLPPBD	(2841_A254h)	*/
union io_r2y_eghwclppbd{
	unsigned long		word;
	struct {
		unsigned long	EGHWCLPPBD_1	:10;
		unsigned long					:6;
		unsigned long	EGHWCLPPBD_2	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGHWCLPMOF	(2841_A258h)	*/
union io_r2y_eghwclpmof{
	unsigned long		word[2];
	struct {
		unsigned long	EGHWCLPMOF_0	:9;
		unsigned long					:7;
		unsigned long	EGHWCLPMOF_1	:9;
		unsigned long					:7;
		unsigned long	EGHWCLPMOF_2	:9;
		unsigned long					:23;
	}bit;
};

/*	structure of EGHWCLPMGA	(2841_A260h)	*/
union io_r2y_eghwclpmga{
	unsigned long		word;
	struct {
		long			EGHWCLPMGA_0	:8;
		long			EGHWCLPMGA_1	:8;
		long			EGHWCLPMGA_2	:8;
		unsigned long					:8;
	}bit;
};

/*	structure of EGHWCLPMBD	(2841_A264h)	*/
union io_r2y_eghwclpmbd{
	unsigned long		word;
	struct {
		unsigned long	EGHWCLPMBD_1	:10;
		unsigned long					:6;
		unsigned long	EGHWCLPMBD_2	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGMWCTL	(2841_A280h)	*/
union io_r2y_egmwctl{
	unsigned long		word;
	struct {
		unsigned long	EGMWEN		:1;
		unsigned long	EGMWMP		:1;
		unsigned long	EGMWTC		:1;
		unsigned long				:5;
		unsigned long	EMSAEN		:1;
		unsigned long	EMTAEN		:1;
		unsigned long				:6;
		unsigned long	EGMWSCLRES	:2;
		unsigned long	EGMWSCLTBL	:2;
		unsigned long	EGMWTCRES	:2;
		unsigned long	EGMWTCTBL	:2;
		unsigned long	EGMWNRLV	:4;
		unsigned long				:4;
	}bit;
};

/*	structure of EGMWHPFK	(2841_A288h)	*/
union io_r2y_egmwhpfk{
	unsigned long		word[2];
	struct {
		unsigned long	EGMWHPFK0	:10;
		unsigned long				:6;
		long			EGMWHPFK1	:8;
		long			EGMWHPFK2	:8;
		long			EGMWHPFK3	:8;
		long			EGMWHPFK4	:8;
		long			EGMWHPFK5	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EGMWCORPOF	(2841_A290h)	*/
union io_r2y_egmwcorpof{
	unsigned long		word[2];
	struct {
		unsigned long	EGMWCORPOF_0	:10;
		unsigned long					:6;
		unsigned long	EGMWCORPOF_1	:10;
		unsigned long					:6;
		unsigned long	EGMWCORPOF_2	:10;
		unsigned long					:6;
		unsigned long	EGMWCORPOF_3	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGMWCORPGA	(2841_A298h)	*/
union io_r2y_egmwcorpga{
	unsigned long		word;
	struct {
		long			EGMWCORPGA_0	:8;
		long			EGMWCORPGA_1	:8;
		long			EGMWCORPGA_2	:8;
		long			EGMWCORPGA_3	:8;
	}bit;
};

/*	structure of EGMWCORPBD	(2841_A2A0h)	*/
union io_r2y_egmwcorpbd{
	unsigned long		word[2];
	struct {
		unsigned long	EGMWCORPBD_1	:10;
		unsigned long					:6;
		unsigned long	EGMWCORPBD_2	:10;
		unsigned long					:6;
		unsigned long	EGMWCORPBD_3	:10;
		unsigned long					:22;
	}bit;
};

/*	structure of EGMWCORMOF	(2841_A2A8h)	*/
union io_r2y_egmwcormof{
	unsigned long		word[2];
	struct {
		unsigned long	EGMWCORMOF_0	:10;
		unsigned long					:6;
		unsigned long	EGMWCORMOF_1	:10;
		unsigned long					:6;
		unsigned long	EGMWCORMOF_2	:10;
		unsigned long					:6;
		unsigned long	EGMWCORMOF_3	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGMWCORMGA	(2841_A2B0h)	*/
union io_r2y_egmwcormga{
	unsigned long		word;
	struct {
		long			EGMWCORMGA_0	:8;
		long			EGMWCORMGA_1	:8;
		long			EGMWCORMGA_2	:8;
		long			EGMWCORMGA_3	:8;
	}bit;
};

/*	structure of EGMWCORMBD	(2841_A2B8h)	*/
union io_r2y_egmwcormbd{
	unsigned long		word[2];
	struct {
		unsigned long	EGMWCORMBD_1	:10;
		unsigned long					:6;
		unsigned long	EGMWCORMBD_2	:10;
		unsigned long					:6;
		unsigned long	EGMWCORMBD_3	:10;
		unsigned long					:22;
	}bit;
};

/*	structure of EGMWSCLSUP	(2841_A2C0h)	*/
union io_r2y_egmwsclsup{
	unsigned long		word;
	struct {
		unsigned long	EGMWSCLSUP	:6;
		unsigned long				:26;
	}bit;
};

/*	structure of EGMWCLPPOF	(2841_A2C8h)	*/
union io_r2y_egmwclppof{
	unsigned long		word[2];
	struct {
		unsigned long	EGMWCLPPOF_0	:9;
		unsigned long					:7;
		unsigned long	EGMWCLPPOF_1	:9;
		unsigned long					:7;
		unsigned long	EGMWCLPPOF_2	:9;
		unsigned long					:23;
	}bit;
};

/*	structure of EGMWCLPPGA	(2841_A2D0h)	*/
union io_r2y_egmwclppga{
	unsigned long		word;
	struct {
		long			EGMWCLPPGA_0	:8;
		long			EGMWCLPPGA_1	:8;
		long			EGMWCLPPGA_2	:8;
		unsigned long					:8;
	}bit;
};

/*	structure of EGMWCLPPBD	(2841_A2D4h)	*/
union io_r2y_egmwclppbd{
	unsigned long		word;
	struct {
		unsigned long	EGMWCLPPBD_1	:10;
		unsigned long					:6;
		unsigned long	EGMWCLPPBD_2	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGMWCLPMOF	(2841_A2D8h)	*/
union io_r2y_egmwclpmof{
	unsigned long		word[2];
	struct {
		unsigned long	EGMWCLPMOF_0	:9;
		unsigned long					:7;
		unsigned long	EGMWCLPMOF_1	:9;
		unsigned long					:7;
		unsigned long	EGMWCLPMOF_2	:9;
		unsigned long					:23;
	}bit;
};

/*	structure of EGMWCLPMGA	(2841_A2E0h)	*/
union io_r2y_egmwclpmga{
	unsigned long		word;
	struct {
		long			EGMWCLPMGA_0	:8;
		long			EGMWCLPMGA_1	:8;
		long			EGMWCLPMGA_2	:8;
		unsigned long					:8;
	}bit;
};

/*	structure of EGMWCLPMBD	(2841_A2E4h)	*/
union io_r2y_egmwclpmbd{
	unsigned long		word;
	struct {
		unsigned long	EGMWCLPMBD_1	:10;
		unsigned long					:6;
		unsigned long	EGMWCLPMBD_2	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGLWCTL	(2841_A300h)	*/
union io_r2y_eglwctl{
	unsigned long		word;
	struct {
		unsigned long	EGLWEN		:1;
		unsigned long	EGLWMP		:1;
		unsigned long	EGLWTC		:1;
		unsigned long				:5;
		unsigned long	ELSAEN		:1;
		unsigned long	ELTAEN		:1;
		unsigned long				:6;
		unsigned long	EGLWSCLRES	:2;
		unsigned long	EGLWSCLTBL	:2;
		unsigned long	EGLWTCRES	:2;
		unsigned long	EGLWTCTBL	:2;
		unsigned long				:8;
	}bit;
};

/*	structure of EGLWHPFK	(2841_A310h)	*/
union io_r2y_eglwhpfk{
	unsigned long		word[3];
	struct {
		unsigned long	EGLWHPFK0	:10;
		unsigned long				:6;
		long			EGLWHPFK1	:8;
		long			EGLWHPFK2	:8;
		long			EGLWHPFK3	:8;
		long			EGLWHPFK4	:8;
		long			EGLWHPFK5	:8;
		long			EGLWHPFK6	:8;
		long			EGLWHPFK7	:8;
		long			EGLWHPFK8	:8;
		long			EGLWHPFK9	:8;
		unsigned long				:8;
	}bit;
};

/*	structure of EGLWCORPOF	(2841_A320h)	*/
union io_r2y_eglwcorpof{
	unsigned long		word[2];
	struct {
		unsigned long	EGLWCORPOF_0	:10;
		unsigned long					:6;
		unsigned long	EGLWCORPOF_1	:10;
		unsigned long					:6;
		unsigned long	EGLWCORPOF_2	:10;
		unsigned long					:6;
		unsigned long	EGLWCORPOF_3	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGLWCORPGA	(2841_A328h)	*/
union io_r2y_eglwcorpga{
	unsigned long		word;
	struct {
		long			EGLWCORPGA_0	:8;
		long			EGLWCORPGA_1	:8;
		long			EGLWCORPGA_2	:8;
		long			EGLWCORPGA_3	:8;
	}bit;
};

/*	structure of EGLWCORPBD	(2841_A330h)	*/
union io_r2y_eglwcorpbd{
	unsigned long		word[2];
	struct {
		unsigned long	EGLWCORPBD_1	:10;
		unsigned long					:6;
		unsigned long	EGLWCORPBD_2	:10;
		unsigned long					:6;
		unsigned long	EGLWCORPBD_3	:10;
		unsigned long					:22;
	}bit;
};

/*	structure of EGLWCORMOF	(2841_A338h)	*/
union io_r2y_eglwcormof{
	unsigned long		word[2];
	struct {
		unsigned long	EGLWCORMOF_0	:10;
		unsigned long					:6;
		unsigned long	EGLWCORMOF_1	:10;
		unsigned long					:6;
		unsigned long	EGLWCORMOF_2	:10;
		unsigned long					:6;
		unsigned long	EGLWCORMOF_3	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGLWCORMGA	(2841_A340h)	*/
union io_r2y_eglwcormga{
	unsigned long		word;
	struct {
		long			EGLWCORMGA_0	:8;
		long			EGLWCORMGA_1	:8;
		long			EGLWCORMGA_2	:8;
		long			EGLWCORMGA_3	:8;
	}bit;
};

/*	structure of EGLWCORMBD	(2841_A348h)	*/
union io_r2y_eglwcormbd{
	unsigned long		word[2];
	struct {
		unsigned long	EGLWCORMBD_1	:10;
		unsigned long					:6;
		unsigned long	EGLWCORMBD_2	:10;
		unsigned long					:6;
		unsigned long	EGLWCORMBD_3	:10;
		unsigned long					:22;
	}bit;
};

/*	structure of EGLWSCLSUP	(2841_A350h)	*/
union io_r2y_eglwsclsup{
	unsigned long		word;
	struct {
		unsigned long	EGLWSCLSUP	:6;
		unsigned long				:26;
	}bit;
};

/*	structure of EGLWCLPPOF	(2841_A358h)	*/
union io_r2y_eglwclppof{
	unsigned long		word[2];
	struct {
		unsigned long	EGLWCLPPOF_0	:9;
		unsigned long					:7;
		unsigned long	EGLWCLPPOF_1	:9;
		unsigned long					:7;
		unsigned long	EGLWCLPPOF_2	:9;
		unsigned long					:23;
	}bit;
};

/*	structure of EGLWCLPPGA	(2841_A360h)	*/
union io_r2y_eglwclppga{
	unsigned long		word;
	struct {
		long			EGLWCLPPGA_0	:8;
		long			EGLWCLPPGA_1	:8;
		long			EGLWCLPPGA_2	:8;
		unsigned long					:8;
	}bit;
};

/*	structure of EGLWCLPPBD	(2841_A364h)	*/
union io_r2y_eglwclppbd{
	unsigned long		word;
	struct {
		unsigned long	EGLWCLPPBD_1	:10;
		unsigned long					:6;
		unsigned long	EGLWCLPPBD_2	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGLWCLPMOF	(2841_A368h)	*/
union io_r2y_eglwclpmof{
	unsigned long		word[2];
	struct {
		unsigned long	EGLWCLPMOF_0	:9;
		unsigned long					:7;
		unsigned long	EGLWCLPMOF_1	:9;
		unsigned long					:7;
		unsigned long	EGLWCLPMOF_2	:9;
		unsigned long					:23;
	}bit;
};

/*	structure of EGLWCLPMGA	(2841_A370h)	*/
union io_r2y_eglwclpmga{
	unsigned long		word;
	struct {
		long			EGLWCLPMGA_0	:8;
		long			EGLWCLPMGA_1	:8;
		long			EGLWCLPMGA_2	:8;
		unsigned long					:8;
	}bit;
};

/*	structure of EGLWCLPMBD	(2841_A374h)	*/
union io_r2y_eglwclpmbd{
	unsigned long		word;
	struct {
		unsigned long	EGLWCLPMBD_1	:10;
		unsigned long					:6;
		unsigned long	EGLWCLPMBD_2	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGDTCTL	(2841_A380h)	*/
union io_r2y_egdtctl{
	unsigned long		word;
	struct {
		unsigned long	EGDTHW	:1;
		unsigned long			:3;
		unsigned long	EGDTMW	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of EGDTHWTH	(2841_A384h)	*/
union io_r2y_egdthwth{
	unsigned long		word;
	struct {
		unsigned long	EGDTHWTHP	:10;
		unsigned long				:6;
		unsigned long	EGDTHWTHM	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of EGDTHWK	(2841_A388h)	*/
union io_r2y_egdthwk{
	unsigned long		word;
	struct {
		unsigned long	EGDTHWKP	:4;
		unsigned long	EGDTHWKM	:4;
		unsigned long				:24;
	}bit;
};

/*	structure of EGDTMWTH	(2841_A38Ch)	*/
union io_r2y_egdtmwth{
	unsigned long		word;
	struct {
		unsigned long	EGDTMWTHP	:10;
		unsigned long				:6;
		unsigned long	EGDTMWTHM	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of EGDTMWK	(2841_A390h)	*/
union io_r2y_egdtmwk{
	unsigned long		word;
	struct {
		unsigned long	EGDTMWKP	:4;
		unsigned long	EGDTMWKM	:4;
		unsigned long				:24;
	}bit;
};

/*	structure of EGMPCTL	(2841_A3A0h)	*/
union io_r2y_egmpctl{
	unsigned long		word;
	struct {
		unsigned long	EMPAEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of EGMPSIZ	(2841_A3A4h)	*/
union io_r2y_egmpsiz{
	unsigned long		word;
	struct {
		unsigned long	EGMPSIZH	:10;
		unsigned long				:6;
		unsigned long	EGMPSIZV	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of EGMPSIZHINV	(2841_A3A8h)	*/
union io_r2y_egmpsizhinv{
	unsigned long		word;
	struct {
		unsigned long	EGMPINVH	:14;
		unsigned long				:2;
		unsigned long	EGMPINVSFTH	:3;
		unsigned long				:13;
	}bit;
};

/*	structure of EGMPSIZVINV	(2841_A3ACh)	*/
union io_r2y_egmpsizvinv{
	unsigned long		word;
	struct {
		unsigned long	EGMPINVV	:14;
		unsigned long				:2;
		unsigned long	EGMPINVSFTV	:3;
		unsigned long				:13;
	}bit;
};

/*	structure of EGMPSTAPH	(2841_A3B0h)	*/
union io_r2y_egmpstaph{
	unsigned long		word;
	struct {
		unsigned long	EGMPSTABH	:4;
		unsigned long				:12;
		unsigned long	EGMPSTACH	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of EGMPSTAPV	(2841_A3B4h)	*/
union io_r2y_egmpstapv{
	unsigned long		word;
	struct {
		unsigned long	EGMPSTABV	:4;
		unsigned long				:12;
		unsigned long	EGMPSTACV	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of EGCMPCTL	(2841_A3C0h)	*/
union io_r2y_egcmpctl{
	unsigned long		word;
	struct {
		unsigned long	EGCMPS	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of EGCMPBD	(2841_A3C8h)	*/
union io_r2y_egcmpbd{
	unsigned long		word[2];
	struct {
		unsigned long	EGCMPBD_1	:10;
		unsigned long				:6;
		unsigned long	EGCMPBD_2	:10;
		unsigned long				:6;
		unsigned long	EGCMPBD_3	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of EGCMPALPOF	(2841_A3D0h)	*/
union io_r2y_egcmpalpof{
	unsigned long		word;
	struct {
		unsigned long	EGCMPALPOF_0	:8;
		unsigned long	EGCMPALPOF_1	:8;
		unsigned long	EGCMPALPOF_2	:8;
		unsigned long	EGCMPALPOF_3	:8;
	}bit;
};

/*	structure of EGCMPALPGA	(2841_A3D8h)	*/
union io_r2y_egcmpalpga{
	unsigned long		word[2];
	struct {
		long			EGCMPALPGA_0	:10;
		unsigned long					:6;
		long			EGCMPALPGA_1	:10;
		unsigned long					:6;
		long			EGCMPALPGA_2	:10;
		unsigned long					:6;
		long			EGCMPALPGA_3	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGCMPBTAOF	(2841_A3E0h)	*/
union io_r2y_egcmpbtaof{
	unsigned long		word;
	struct {
		unsigned long	EGCMPBTAOF_0	:8;
		unsigned long	EGCMPBTAOF_1	:8;
		unsigned long	EGCMPBTAOF_2	:8;
		unsigned long	EGCMPBTAOF_3	:8;
	}bit;
};

/*	structure of EGCMPBTAGA	(2841_A3E8h)	*/
union io_r2y_egcmpbtaga{
	unsigned long		word[2];
	struct {
		long			EGCMPBTAGA_0	:10;
		unsigned long					:6;
		long			EGCMPBTAGA_1	:10;
		unsigned long					:6;
		long			EGCMPBTAGA_2	:10;
		unsigned long					:6;
		long			EGCMPBTAGA_3	:10;
		unsigned long					:6;
	}bit;
};

/*	structure of EGCMPCLP	(2841_A3F0h)	*/
union io_r2y_egcmpclp{
	unsigned long		word;
	struct {
		unsigned long	EGCMPCLPP	:9;
		unsigned long				:7;
		unsigned long	EGCMPCLPM	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of CRVAF	(2841_A400h)	*/
union io_r2y_crvaf{
	unsigned long		word;
	struct {
		long			CRVAFX1	:8;
		long			CRVAFY1	:8;
		long			CRVAFX2	:8;
		long			CRVAFY2	:8;
	}bit;
};

/*	structure of CRVBF	(2841_A404h)	*/
union io_r2y_crvbf{
	unsigned long		word;
	struct {
		long			CRVBFX1	:8;
		long			CRVBFY1	:8;
		long			CRVBFX2	:8;
		long			CRVBFY2	:8;
	}bit;
};

/*	structure of EGCRVCTL	(2841_A440h)	*/
union io_r2y_egcrvctl{
	unsigned long		word;
	struct {
		unsigned long	EGCRAE		:1;
		unsigned long				:3;
		unsigned long	EGCRBE		:1;
		unsigned long				:3;
		unsigned long	EGCRVMRG	:2;
		unsigned long				:22;
	}bit;
};

/*	structure of EGCRVASCLOF	(2841_A444h)	*/
union io_r2y_egcrvasclof{
	unsigned long		word;
	struct {
		unsigned long	EGCRVASCLOF1	:8;
		unsigned long					:24;
	}bit;
};

/*	structure of EGCRVASCLGA	(2841_A448h)	*/
union io_r2y_egcrvasclga{
	unsigned long		word;
	struct {
		long			EGCRVASCLGA_0	:11;
		unsigned long					:5;
		long			EGCRVASCLGA_1	:11;
		unsigned long					:5;
	}bit;
};

/*	structure of EGCRVASCLBD	(2841_A44Ch)	*/
union io_r2y_egcrvasclbd{
	unsigned long		word;
	struct {
		unsigned long	EGCRVASCLBD1	:7;
		unsigned long					:25;
	}bit;
};

/*	structure of EGCRVASCLCP	(2841_A450h)	*/
union io_r2y_egcrvasclcp{
	unsigned long		word;
	struct {
		unsigned long	EGCRVASCLCPL	:8;
		unsigned long	EGCRVASCLCPH	:8;
		unsigned long					:16;
	}bit;
};

/*	structure of EGYASCLGA	(2841_A454h)	*/
union io_r2y_egyasclga{
	unsigned long		word;
	struct {
		unsigned long	EGYASCLGA0	:8;
		unsigned long	EGYASCLGA1	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EGYASCLBD	(2841_A458h)	*/
union io_r2y_egyasclbd{
	unsigned long		word;
	struct {
		unsigned long	EGYASCLBD_0	:8;
		unsigned long	EGYASCLBD_1	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EGYASCLCLP	(2841_A45Ch)	*/
union io_r2y_egyasclclp{
	unsigned long		word;
	struct {
		unsigned long	EGYASCLCLPL	:9;
		unsigned long				:7;
		unsigned long	EGYASCLCLPH	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of EGCRVBSCLOF	(2841_A460h)	*/
union io_r2y_egcrvbsclof{
	unsigned long		word;
	struct {
		unsigned long	EGCRVBSCLOF1	:8;
		unsigned long					:24;
	}bit;
};

/*	structure of EGCRVBSCLGA	(2841_A464h)	*/
union io_r2y_egcrvbsclga{
	unsigned long		word;
	struct {
		long			EGCRVBSCLGA_0	:11;
		unsigned long					:5;
		long			EGCRVBSCLGA_1	:11;
		unsigned long					:5;
	}bit;
};

/*	structure of EGCRVBSCLBD	(2841_A468h)	*/
union io_r2y_egcrvbsclbd{
	unsigned long		word;
	struct {
		unsigned long	EGCRVBSCLBD1	:7;
		unsigned long					:25;
	}bit;
};

/*	structure of EGCRVBSCLCP	(2841_A46Ch)	*/
union io_r2y_egcrvbsclcp{
	unsigned long		word;
	struct {
		unsigned long	EGCRVBSCLCPL	:8;
		unsigned long	EGCRVBSCLCPH	:8;
		unsigned long					:16;
	}bit;
};

/*	structure of EGYBSCLGA	(2841_A470h)	*/
union io_r2y_egybsclga{
	unsigned long		word;
	struct {
		unsigned long	EGYBSCLGA0	:8;
		unsigned long	EGYBSCLGA1	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EGYBSCLBD	(2841_A474h)	*/
union io_r2y_egybsclbd{
	unsigned long		word;
	struct {
		unsigned long	EGYBSCLBD_0	:8;
		unsigned long	EGYBSCLBD_1	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of EGYBSCLCLP	(2841_A478h)	*/
union io_r2y_egybsclclp{
	unsigned long		word;
	struct {
		unsigned long	EGYBSCLCLPL	:9;
		unsigned long				:7;
		unsigned long	EGYBSCLCLPH	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of EGCRVCLP	(2841_A47Ch)	*/
union io_r2y_egcrvclp{
	unsigned long		word;
	struct {
		unsigned long	EGCRVCLPP	:9;
		unsigned long				:7;
		unsigned long	EGCRVCLPM	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of YBCRVCTL	(2841_A480h)	*/
union io_r2y_ybcrvctl{
	unsigned long		word;
	struct {
		unsigned long	YBCRAE	:1;
		unsigned long			:3;
		unsigned long	YBCRBE	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of YBCRVAALPOF	(2841_A484h)	*/
union io_r2y_ybcrvaalpof{
	unsigned long		word;
	struct {
		unsigned long	YBCRVAALPOF1	:8;
		unsigned long					:24;
	}bit;
};

/*	structure of YBCRVAALPGA	(2841_A488h)	*/
union io_r2y_ybcrvaalpga{
	unsigned long		word;
	struct {
		long			YBCRVAALPGA_0	:11;
		unsigned long					:5;
		long			YBCRVAALPGA_1	:11;
		unsigned long					:5;
	}bit;
};

/*	structure of YBCRVAALPBD	(2841_A48Ch)	*/
union io_r2y_ybcrvaalpbd{
	unsigned long		word;
	struct {
		unsigned long	YBCRVAALPBD1	:7;
		unsigned long					:25;
	}bit;
};

/*	structure of YBCRVAALPCP	(2841_A490h)	*/
union io_r2y_ybcrvaalpcp{
	unsigned long		word;
	struct {
		unsigned long	YBCRVAALPCPL	:8;
		unsigned long	YBCRVAALPCPH	:8;
		unsigned long					:16;
	}bit;
};

/*	structure of YBYAALPGA	(2841_A494h)	*/
union io_r2y_ybyaalpga{
	unsigned long		word;
	struct {
		unsigned long	YBYAALPGA0	:8;
		unsigned long	YBYAALPGA1	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of YBYAALPBD	(2841_A498h)	*/
union io_r2y_ybyaalpbd{
	unsigned long		word;
	struct {
		unsigned long	YBYAALPBD_0	:8;
		unsigned long	YBYAALPBD_1	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of YBYAALPCLP	(2841_A49Ch)	*/
union io_r2y_ybyaalpclp{
	unsigned long		word;
	struct {
		unsigned long	YBYAALPCLPL	:8;
		unsigned long	YBYAALPCLPH	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of YBCRVBALPOF	(2841_A4A0h)	*/
union io_r2y_ybcrvbalpof{
	unsigned long		word;
	struct {
		unsigned long	YBCRVBALPOF1	:8;
		unsigned long					:24;
	}bit;
};

/*	structure of YBCRVBALPGA	(2841_A4A4h)	*/
union io_r2y_ybcrvbalpga{
	unsigned long		word;
	struct {
		long			YBCRVBALPGA_0	:11;
		unsigned long					:5;
		long			YBCRVBALPGA_1	:11;
		unsigned long					:5;
	}bit;
};

/*	structure of YBCRVBALPBD	(2841_A4A8h)	*/
union io_r2y_ybcrvbalpbd{
	unsigned long		word;
	struct {
		unsigned long	YBCRVBALPBD1	:7;
		unsigned long					:25;
	}bit;
};

/*	structure of YBCRVBALPCP	(2841_A4ACh)	*/
union io_r2y_ybcrvbalpcp{
	unsigned long		word;
	struct {
		unsigned long	YBCRVBALPCPL	:8;
		unsigned long	YBCRVBALPCPH	:8;
		unsigned long					:16;
	}bit;
};

/*	structure of YBYBALPGA	(2841_A4B0h)	*/
union io_r2y_ybybalpga{
	unsigned long		word;
	struct {
		unsigned long	YBYBALPGA0	:8;
		unsigned long	YBYBALPGA1	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of YBYBALPBD	(2841_A4B4h)	*/
union io_r2y_ybybalpbd{
	unsigned long		word;
	struct {
		unsigned long	YBYBALPBD_0	:8;
		unsigned long	YBYBALPBD_1	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of YBYBALPCLP	(2841_A4B8h)	*/
union io_r2y_ybybalpclp{
	unsigned long		word;
	struct {
		unsigned long	YBYBALPCLPL	:8;
		unsigned long	YBYBALPCLPH	:8;
		unsigned long				:16;
	}bit;
};

/*	structure of CLPFCTL	(2841_A500h)	*/
union io_r2y_clpfctl{
	unsigned long		word;
	struct {
		unsigned long	CLPFEN	:1;
		unsigned long			:3;
		unsigned long	CLPFYA	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of CLPFK	(2841_A520h)	*/
union io_r2y_clpfk{
	unsigned long		word[5];
	struct {
		long			CLPFK_0	:9;
		unsigned long			:7;
		long			CLPFK_1	:9;
		unsigned long			:7;
		long			CLPFK_2	:9;
		unsigned long			:7;
		long			CLPFK_3	:9;
		unsigned long			:7;
		long			CLPFK_4	:9;
		unsigned long			:7;
		long			CLPFK_5	:9;
		unsigned long			:7;
		long			CLPFK_6	:9;
		unsigned long			:7;
		long			CLPFK_7	:9;
		unsigned long			:7;
		long			CLPFK_8	:9;
		unsigned long			:23;
	}bit;
};

/*	structure of CLPFOF	(2841_A540h)	*/
union io_r2y_clpfof{
	unsigned long		word;
	struct {
		unsigned long	CLPFOF_0	:8;
		unsigned long	CLPFOF_1	:8;
		unsigned long	CLPFOF_2	:8;
		unsigned long	CLPFOF_3	:8;
	}bit;
};

/*	structure of CLPFGA	(2841_A548h)	*/
union io_r2y_clpfga{
	unsigned long		word[2];
	struct {
		long			CLPFGA_0	:10;
		unsigned long				:6;
		long			CLPFGA_1	:10;
		unsigned long				:6;
		long			CLPFGA_2	:10;
		unsigned long				:6;
		long			CLPFGA_3	:10;
		unsigned long				:6;
	}bit;
};

/*	structure of CLPFBD	(2841_A550h)	*/
union io_r2y_clpfbd{
	unsigned long		word[2];
	struct {
		unsigned long	CLPFBD_1	:10;
		unsigned long				:6;
		unsigned long	CLPFBD_2	:10;
		unsigned long				:6;
		unsigned long	CLPFBD_3	:10;
		unsigned long				:22;
	}bit;
};

/*	structure of CSYCTL	(2841_A580h)	*/
union io_r2y_csyctl{
	unsigned long		word;
	struct {
		unsigned long	CSYEN	:1;
		unsigned long			:3;
		unsigned long	CSYKY	:4;
		unsigned long			:4;
		unsigned long	CSYTBL	:1;
		unsigned long			:19;
	}bit;
};

/*	structure of CSYOF	(2841_A588h)	*/
union io_r2y_csyof{
	unsigned long		word[2];
	struct {
		unsigned long	CSYOF_0	:10;
		unsigned long			:6;
		unsigned long	CSYOF_1	:10;
		unsigned long			:6;
		unsigned long	CSYOF_2	:10;
		unsigned long			:6;
		unsigned long	CSYOF_3	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of CSYGA	(2841_A590h)	*/
union io_r2y_csyga{
	unsigned long		word[2];
	struct {
		long			CSYGA_0	:11;
		unsigned long			:5;
		long			CSYGA_1	:11;
		unsigned long			:5;
		long			CSYGA_2	:11;
		unsigned long			:5;
		long			CSYGA_3	:11;
		unsigned long			:5;
	}bit;
};

/*	structure of CSYBD	(2841_A598h)	*/
union io_r2y_csybd{
	unsigned long		word[2];
	struct {
		unsigned long	CSYBD_1	:10;
		unsigned long			:6;
		unsigned long	CSYBD_2	:10;
		unsigned long			:6;
		unsigned long	CSYBD_3	:10;
		unsigned long			:22;
	}bit;
};

/*	structure of YCRVFX	(2841_A5A0h)	*/
union io_r2y_ycrvfx{
	unsigned long		word;
	struct {
		unsigned long	YRV		:1;
		unsigned long			:3;
		unsigned long	CRV		:1;
		unsigned long			:3;
		unsigned long	CFIXEN	:1;
		unsigned long			:23;
	}bit;
};

/*	structure of CFIX	(2841_A5A4h)	*/
union io_r2y_cfix{
	unsigned long		word;
	struct {
		long			CFIXB	:10;
		unsigned long			:6;
		long			CFIXR	:10;
		unsigned long			:6;
	}bit;
};

/*	structure of YCOF	(2841_A5A8h)	*/
union io_r2y_ycof{
	unsigned long		word[2];
	struct {
		long			YOF		:11;
		unsigned long			:21;
		long			COFB	:11;
		unsigned long			:5;
		long			COFR	:11;
		unsigned long			:5;
	}bit;
};

/*	structure of FR2Y control area	2841_(8000 - 801Fh) */
struct io_r2y_fr2y_cntl{
	union	io_r2y_cntl			CNTL;			/* 2841_(8000 - 8003h) */
	union	io_r2y_r2yflag		R2YFLAG;		/* 2841_(8004 - 8007h) */
	union	io_r2y_r2yinte		R2YINTE;		/* 2841_(8008 - 800Bh) */
	union	io_r2y_r2yintf		R2YINTF;		/* 2841_(800C - 800Fh) */
	union	io_r2y_fltaen		FLTAEN;			/* 2841_(8010 - 8013h) */

	unsigned char dmy_8014_801F[0x8020-0x8014];	/* 2841_(8014 - 801Fh) */
};

/*	structure of Offset & WBGain area	2841_(8020 - 807Fh) */
struct io_r2y_ofg{
	union	io_r2y_ofst			OFST;			/* 2841_(8020 - 8027h) */
	union	io_r2y_wbgar		WBGAR;			/* 2841_(8028 - 802Fh) */
	union	io_r2y_wblv			WBLV;			/* 2841_(8030 - 8037h) */

	unsigned char dmy_8038_807F[0x8080-0x8038];	/* 2841_(8038 - 807Fh) */
};

/*	structure of CCA0 area	2841_(8080 - 8FFFh) */
struct io_r2y_cca0{
	union	io_r2y_cc0ctl		CC0CTL;			/* 2841_(8080 - 8083h) */

	unsigned char dmy_8084_809F[0x80A0-0x8084];	/* 2841_(8084 - 809Fh) */

	union	io_r2y_cc0k			CC0K;			/* 2841_(80A0 - 80B3h) */

	unsigned char dmy_80B4_80BF[0x80C0-0x80B4];	/* 2841_(80B4 - 80BFh) */

	union	io_r2y_cc0ybof		CC0YBOF;		/* 2841_(80C0 - 80C7h) */
	union	io_r2y_cc0ybga		CC0YBGA;		/* 2841_(80C8 - 80CFh) */
	union	io_r2y_cc0ybbd		CC0YBBD;		/* 2841_(80D0 - 80D3h) */
	union	io_r2y_ccyc			CCYC;			/* 2841_(80D4 - 80D7h) */

	unsigned char dmy_80D8_8FFF[0x9000-0x80D8];	/* 2841_(80D8 - 8FFFh) */
};

/*	structure of MCC area	2841_(9000 - 9FFFh) */
struct io_r2y_mcc{
	union	io_r2y_mcyc			MCYC;			/* 2841_(9000 - 9013h) */

	unsigned char dmy_9014_901F[0x9020-0x9014];	/* 2841_(9014 - 901Fh) */

	union	io_r2y_mcb1ab		MCB1AB;			/* 2841_(9020 - 9023h) */
	union	io_r2y_mcb1cd		MCB1CD;			/* 2841_(9024 - 9027h) */
	union	io_r2y_mcb2ab		MCB2AB;			/* 2841_(9028 - 902Bh) */
	union	io_r2y_mcb2cd		MCB2CD;			/* 2841_(902C - 902Fh) */
	union	io_r2y_mcb3ab		MCB3AB;			/* 2841_(9030 - 9033h) */
	union	io_r2y_mcb3cd		MCB3CD;			/* 2841_(9034 - 9037h) */
	union	io_r2y_mcb4ab		MCB4AB;			/* 2841_(9038 - 903Bh) */
	union	io_r2y_mcb4cd		MCB4CD;			/* 2841_(903C - 903Fh) */
	union	io_r2y_mcid1		MCID1;			/* 2841_(9040 - 9043h) */
	union	io_r2y_mcid2		MCID2;			/* 2841_(9044 - 9047h) */
	union	io_r2y_mcid3		MCID3;			/* 2841_(9048 - 904Bh) */
	union	io_r2y_mcid4		MCID4;			/* 2841_(904C - 904Fh) */

	unsigned char dmy_9050_907F[0x9080-0x9050];	/* 2841_(9050 - 907Fh) */

	struct io_r2y_mck			MCKA;			/* 2841_(9080 - 90E3h) */

	unsigned char dmy_90E4_90FF[0x9100-0x90E4];	/* 2841_(90E4 - 90FFh) */

	struct io_r2y_mck			MCKB;			/* 2841_(9100 - 9163h) */

	unsigned char dmy_9164_917F[0x9180-0x9164];	/* 2841_(9164 - 917Fh) */

	struct io_r2y_mck			MCKC;			/* 2841_(9180 - 91E3h) */

	unsigned char dmy_91E4_91FF[0x9200-0x91E4];	/* 2841_(91E4 - 91FFh) */

	struct io_r2y_mck			MCKD;			/* 2841_(9200 - 9263h) */

	unsigned char dmy_9264_927F[0x9280-0x9264];	/* 2841_(9264 - 927Fh) */

	struct io_r2y_mck			MCKE;			/* 2841_(9280 - 92E3h) */

	unsigned char dmy_92E4_92FF[0x9300-0x92E4];	/* 2841_(92E4 - 92FFh) */

	struct io_r2y_mck			MCKF;			/* 2841_(9300 - 9363h) */

	unsigned char dmy_9364_937F[0x9380-0x9364];	/* 2841_(9364 - 937Fh) */

	struct io_r2y_mck			MCKG;			/* 2841_(9380 - 93E3h) */

	unsigned char dmy_93E4_93FF[0x9400-0x93E4];	/* 2841_(93E4 - 93FFh) */

	struct io_r2y_mck			MCKH;			/* 2841_(9400 - 9463h) */

	unsigned char dmy_9464_947F[0x9480-0x9464];	/* 2841_(9464 - 947Fh) */

	struct io_r2y_mck			MCKI;			/* 2841_(9480 - 94E3h) */

	unsigned char dmy_94E4_94FF[0x9500-0x94E4];	/* 2841_(94E4 - 94FFh) */

	struct io_r2y_mck			MCKJ;			/* 2841_(9500 - 9563h) */

	unsigned char dmy_9564_957F[0x9580-0x9564];	/* 2841_(9564 - 957Fh) */

	struct io_r2y_mck			MCKK;			/* 2841_(9580 - 95E3h) */

	unsigned char dmy_95E4_95FF[0x9600-0x95E4];	/* 2841_(95E4 - 95FFh) */

	struct io_r2y_mck			MCKL;			/* 2841_(9600 - 9663h) */

	unsigned char dmy_9664_967F[0x9680-0x9664];	/* 2841_(9664 - 967Fh) */

	struct io_r2y_mcl			MCLA;			/* 2841_(9680 - 96BBh) */

	unsigned char dmy_96BC_96BF[0x96C0-0x96BC];	/* 2841_(96BC - 96BFh) */

	struct io_r2y_mcl			MCLB;			/* 2841_(96C0 - 96FBh) */

	unsigned char dmy_96FC_96FF[0x9700-0x96FC];	/* 2841_(96FC - 96FFh) */

	struct io_r2y_mcl			MCLC;			/* 2841_(9700 - 973Bh) */

	unsigned char dmy_973C_973F[0x9740-0x973C];	/* 2841_(973C - 973Fh) */

	struct io_r2y_mcl			MCLD;			/* 2841_(9740 - 977Bh) */

	unsigned char dmy_977C_977F[0x9780-0x977C];	/* 2841_(977C - 977Fh) */

	struct io_r2y_mcl			MCLE;			/* 2841_(9780 - 97BBh) */

	unsigned char dmy_97BC_97BF[0x97C0-0x97BC];	/* 2841_(97BC - 97BFh) */

	struct io_r2y_mcl			MCLF;			/* 2841_(97C0 - 97FBh) */

	unsigned char dmy_97FC_97FF[0x9800-0x97FC];	/* 2841_(97FC - 97FFh) */

	struct io_r2y_mcl			MCLG;			/* 2841_(9800 - 983Bh) */

	unsigned char dmy_983C_983F[0x9840-0x983C];	/* 2841_(983C - 983Fh) */

	struct io_r2y_mcl			MCLH;			/* 2841_(9840 - 987Bh) */

	unsigned char dmy_987C_987F[0x9880-0x987C];	/* 2841_(987C - 987Fh) */

	struct io_r2y_mcl			MCLI;			/* 2841_(9880 - 98BBh) */

	unsigned char dmy_98BC_98BF[0x98C0-0x98BC];	/* 2841_(98BC - 98BFh) */

	struct io_r2y_mcl			MCLJ;			/* 2841_(98C0 - 98FBh) */

	unsigned char dmy_98FC_98FF[0x9900-0x98FC];	/* 2841_(98FC - 98FFh) */

	struct io_r2y_mcl			MCLK;			/* 2841_(9900 - 993Bh) */

	unsigned char dmy_993C_993F[0x9940-0x993C];	/* 2841_(993C - 993Fh) */

	struct io_r2y_mcl			MCLL;			/* 2841_(9940 - 997Bh) */

	unsigned char dmy_997C_997F[0x9980-0x997C];	/* 2841_(997C - 997Fh) */

	union	io_r2y_mcycbalp		MCYCBALP;		/* 2841_(9980 - 9983h) */

	unsigned char dmy_9984_9987[0x9988-0x9984];	/* 2841_(9984 - 9987h) */

	union	io_r2y_mcycbga		MCYCBGA;		/* 2841_(9988 - 998Fh) */
	union	io_r2y_mcycbbd		MCYCBBD;		/* 2841_(9990 - 9997h) */
	union	io_r2y_mcbabalp		MCBABALP;		/* 2841_(9998 - 999Bh) */

	unsigned char dmy_999C_999F[0x99A0-0x999C];	/* 2841_(999C - 999Fh) */

	union	io_r2y_mcbabof		MCBABOF;		/* 2841_(99A0 - 99A7h) */
	union	io_r2y_mcbabga		MCBABGA;		/* 2841_(99A8 - 99AFh) */
	union	io_r2y_mcbabbd		MCBABBD;		/* 2841_(99B0 - 99B7h) */

	unsigned char dmy_99B8_9FFF[0xA000-0x99B8];	/* 2841_(99B8 - 9FFFh) */
};

/*	structure of Offset/Luminance Evaluation/Histogram before TC area	2841_(A000 - A03Fh) */
struct io_r2y_btc{
	union	io_r2y_tcof			TCOF;			/* 2841_(A000 - A007h) */
	union	io_r2y_tctctl		TCTCTL;			/* 2841_(A008 - A00Bh) */
	union	io_r2y_tctsta		TCTSTA;			/* 2841_(A00C - A00Fh) */
	union	io_r2y_tctb			TCTB;			/* 2841_(A010 - A013h) */
	union	io_r2y_tchsctl		TCHSCTL;		/* 2841_(A014 - A017h) */
	union	io_r2y_tchssta		TCHSSTA;		/* 2841_(A018 - A01Bh) */
	union	io_r2y_tchssiz		TCHSSIZ;		/* 2841_(A01C - A01Fh) */
	union	io_r2y_tchov		TCHOV;			/* 2841_(A020 - A023h) */

	unsigned char dmy_A024_A03F[0xA040-0xA024];	/* 2841_(A024 - A03Fh) */
};

/*	structure of Tone area	2841_(A040 - A05Fh) */
struct io_r2y_tc_area{
	union	io_r2y_tcctl		TCCTL;			/* 2841_(A040 - A043h) */
	union	io_r2y_tcyc			TCYC;			/* 2841_(A044 - A047h) */
	union	io_r2y_tcep			TCEP;			/* 2841_(A048 - A04Fh) */
	union	io_r2y_tcclpr		TCCLPR;			/* 2841_(A050 - A053h) */
	union	io_r2y_tcclpg		TCCLPG;			/* 2841_(A054 - A057h) */
	union	io_r2y_tcclpb		TCCLPB;			/* 2841_(A058 - A05Bh) */

	unsigned char dmy_A05C_A05F[0xA060-0xA05C];	/* 2841_(A05C - A05Fh) */
};

/*	structure of Gamma area	2841_(A060 - A07Fh) */
struct io_r2y_gam{
	union	io_r2y_gmctl		GMCTL;			/* 2841_(A060 - A063h) */

	unsigned char dmy_A064_A07F[0xA080-0xA064];	/* 2841_(A064 - A07Fh) */
};

/*	structure of CCA1 area	2841_(A080 - A0FFh) */
struct io_r2y_cca1{
	union	io_r2y_cc1ctl		CC1CTL;			/* 2841_(A080 - A083h) */

	unsigned char dmy_A084_A09F[0xA0A0-0xA084];	/* 2841_(A084 - A09Fh) */

	union	io_r2y_cc1k			CC1K;			/* 2841_(A0A0 - A0B3h) */

	unsigned char dmy_A0B4_A0BF[0xA0C0-0xA0B4];	/* 2841_(A0B4 - A0BFh) */

	union	io_r2y_cc1clpr		CC1CLPR;		/* 2841_(A0C0 - A0C3h) */
	union	io_r2y_cc1clpg		CC1CLPG;		/* 2841_(A0C4 - A0C7h) */
	union	io_r2y_cc1clpb		CC1CLPB;		/* 2841_(A0C8 - A0CBh) */

	unsigned char dmy_A0CC_A0FF[0xA100-0xA0CC];	/* 2841_(A0CC - A0FFh) */
};

/*	structure of YC area	2841_(A100 - A13Fh) */
struct io_r2y_yc_area{
	union	io_r2y_yc			YC;				/* 2841_(A100 - A113h) */

	unsigned char dmy_A114_A11F[0xA120-0xA114];	/* 2841_(A114 - A11Fh) */

	union	io_r2y_yblend		YBLEND;			/* 2841_(A120 - A123h) */

	unsigned char dmy_A124_A13F[0xA140-0xA124];	/* 2841_(A124 - A13Fh) */
};

/*	structure of YNR area	2841_(A140 - A17Fh) */
struct io_r2y_ynr{
	union	io_r2y_nrctl		NRCTL;			/* 2841_(A140 - A143h) */

	unsigned char dmy_A144_A147[0xA148-0xA144];	/* 2841_(A144 - A147h) */

	union	io_r2y_nrof			NROF;			/* 2841_(A148 - A14Fh) */
	union	io_r2y_nrga			NRGA;			/* 2841_(A150 - A157h) */
	union	io_r2y_nrbd			NRBD;			/* 2841_(A158 - A15Fh) */

	unsigned char dmy_A160_A17F[0xA180-0xA160];	/* 2841_(A160 - A17Fh) */
};

/*	structure of EGHW & EGMW common area	2841_(A180 - A1FFh) */
struct io_r2y_egsm{
	union	io_r2y_egsmctl		EGSMCTL;		/* 2841_(A180 - A183h) */
	union	io_r2y_egsmtt		EGSMTT;			/* 2841_(A184 - A187h) */

	unsigned char dmy_A188_A1FF[0xA200-0xA188];	/* 2841_(A188 - A1FFh) */
};

/*	structure of EGHW area	2841_(A200 - A27Fh) */
struct io_r2y_eghw{
	union	io_r2y_eghwctl		EGHWCTL;		/* 2841_(A200 - A203h) */

	unsigned char dmy_A204_A207[0xA208-0xA204];	/* 2841_(A204 - A207h) */

	union	io_r2y_eghwhpfk		EGHWHPFK;		/* 2841_(A208 - A20Fh) */
	union	io_r2y_eghwcorpof	EGHWCORPOF;		/* 2841_(A210 - A217h) */
	union	io_r2y_eghwcorpga	EGHWCORPGA;		/* 2841_(A218 - A21Bh) */

	unsigned char dmy_A21C_A21F[0xA220-0xA21C];	/* 2841_(A21C - A21Fh) */

	union	io_r2y_eghwcorpbd	EGHWCORPBD;		/* 2841_(A220 - A227h) */
	union	io_r2y_eghwcormof	EGHWCORMOF;		/* 2841_(A228 - A22Fh) */
	union	io_r2y_eghwcormga	EGHWCORMGA;		/* 2841_(A230 - A233h) */

	unsigned char dmy_A234_A237[0xA238-0xA234];	/* 2841_(A234 - A237h) */

	union	io_r2y_eghwcormbd	EGHWCORMBD;		/* 2841_(A238 - A23Fh) */
	union	io_r2y_eghwsclsup	EGHWSCLSUP;		/* 2841_(A240 - A243h) */

	unsigned char dmy_A244_A247[0xA248-0xA244];	/* 2841_(A244 - A247h) */

	union	io_r2y_eghwclppof	EGHWCLPPOF;		/* 2841_(A248 - A24Fh) */
	union	io_r2y_eghwclppga	EGHWCLPPGA;		/* 2841_(A250 - A253h) */
	union	io_r2y_eghwclppbd	EGHWCLPPBD;		/* 2841_(A254 - A257h) */
	union	io_r2y_eghwclpmof	EGHWCLPMOF;		/* 2841_(A258 - A25Fh) */
	union	io_r2y_eghwclpmga	EGHWCLPMGA;		/* 2841_(A260 - A263h) */
	union	io_r2y_eghwclpmbd	EGHWCLPMBD;		/* 2841_(A264 - A267h) */

	unsigned char dmy_A268_A27F[0xA280-0xA268];	/* 2841_(A268 - A27Fh) */
};

/*	structure of EGMW area	2841_(A280 - A2FFh) */
struct io_r2y_egmw{
	union	io_r2y_egmwctl		EGMWCTL;		/* 2841_(A280 - A283h) */

	unsigned char dmy_A284_A287[0xA288-0xA284];	/* 2841_(A284 - A287h) */

	union	io_r2y_egmwhpfk		EGMWHPFK;		/* 2841_(A288 - A28Fh) */
	union	io_r2y_egmwcorpof	EGMWCORPOF;		/* 2841_(A290 - A297h) */
	union	io_r2y_egmwcorpga	EGMWCORPGA;		/* 2841_(A298 - A29Bh) */

	unsigned char dmy_A29C_A29F[0xA2A0-0xA29C];	/* 2841_(A29C - A29Fh) */

	union	io_r2y_egmwcorpbd	EGMWCORPBD;		/* 2841_(A2A0 - A2A7h) */
	union	io_r2y_egmwcormof	EGMWCORMOF;		/* 2841_(A2A8 - A2AFh) */
	union	io_r2y_egmwcormga	EGMWCORMGA;		/* 2841_(A2B0 - A2B3h) */

	unsigned char dmy_A2B4_A2B7[0xA2B8-0xA2B4];	/* 2841_(A2B4 - A2B7h) */

	union	io_r2y_egmwcormbd	EGMWCORMBD;		/* 2841_(A2B8 - A2BFh) */
	union	io_r2y_egmwsclsup	EGMWSCLSUP;		/* 2841_(A2C0 - A2C3h) */

	unsigned char dmy_A2C4_A2C7[0xA2C8-0xA2C4];	/* 2841_(A2C4 - A2C7h) */

	union	io_r2y_egmwclppof	EGMWCLPPOF;		/* 2841_(A2C8 - A2CFh) */
	union	io_r2y_egmwclppga	EGMWCLPPGA;		/* 2841_(A2D0 - A2D3h) */
	union	io_r2y_egmwclppbd	EGMWCLPPBD;		/* 2841_(A2D4 - A2D7h) */
	union	io_r2y_egmwclpmof	EGMWCLPMOF;		/* 2841_(A2D8 - A2DFh) */
	union	io_r2y_egmwclpmga	EGMWCLPMGA;		/* 2841_(A2E0 - A2E3h) */
	union	io_r2y_egmwclpmbd	EGMWCLPMBD;		/* 2841_(A2E4 - A2E7h) */

	unsigned char dmy_A2E8_A2FF[0xA300-0xA2E8];	/* 2841_(A2E8 - A2FFh) */
};

/*	structure of EGLW area	2841_(A300 - A37Fh) */
struct io_r2y_eglw{
	union	io_r2y_eglwctl		EGLWCTL;		/* 2841_(A300 - A303h) */

	unsigned char dmy_A304_A30F[0xA310-0xA304];	/* 2841_(A304 - A30Fh) */

	union	io_r2y_eglwhpfk		EGLWHPFK;		/* 2841_(A310 - A31Bh) */

	unsigned char dmy_A31C_A31F[0xA320-0xA31C];	/* 2841_(A31C - A31Fh) */

	union	io_r2y_eglwcorpof	EGLWCORPOF;		/* 2841_(A320 - A327h) */
	union	io_r2y_eglwcorpga	EGLWCORPGA;		/* 2841_(A328 - A32Bh) */

	unsigned char dmy_A32C_A32F[0xA330-0xA32C];	/* 2841_(A32C - A32Fh) */

	union	io_r2y_eglwcorpbd	EGLWCORPBD;		/* 2841_(A330 - A337h) */
	union	io_r2y_eglwcormof	EGLWCORMOF;		/* 2841_(A338 - A33Fh) */
	union	io_r2y_eglwcormga	EGLWCORMGA;		/* 2841_(A340 - A343h) */

	unsigned char dmy_A344_A347[0xA348-0xA344];	/* 2841_(A344 - A347h) */

	union	io_r2y_eglwcormbd	EGLWCORMBD;		/* 2841_(A348 - A34Fh) */
	union	io_r2y_eglwsclsup	EGLWSCLSUP;		/* 2841_(A350 - A353h) */

	unsigned char dmy_A354_A357[0xA358-0xA354];	/* 2841_(A354 - A357h) */

	union	io_r2y_eglwclppof	EGLWCLPPOF;		/* 2841_(A358 - A35Fh) */
	union	io_r2y_eglwclppga	EGLWCLPPGA;		/* 2841_(A360 - A363h) */
	union	io_r2y_eglwclppbd	EGLWCLPPBD;		/* 2841_(A364 - A367h) */
	union	io_r2y_eglwclpmof	EGLWCLPMOF;		/* 2841_(A368 - A36Fh) */
	union	io_r2y_eglwclpmga	EGLWCLPMGA;		/* 2841_(A370 - A373h) */
	union	io_r2y_eglwclpmbd	EGLWCLPMBD;		/* 2841_(A374 - A377h) */

	unsigned char dmy_A378_A37F[0xA380-0xA378];	/* 2841_(A378 - A37Fh) */
};

/*	structure of EGDT area	2841_(A380 - A39Fh) */
struct io_r2y_egdt{
	union	io_r2y_egdtctl		EGDTCTL;		/* 2841_(A380 - A383h) */
	union	io_r2y_egdthwth		EGDTHWTH;		/* 2841_(A384 - A387h) */
	union	io_r2y_egdthwk		EGDTHWK;		/* 2841_(A388 - A38Bh) */
	union	io_r2y_egdtmwth		EGDTMWTH;		/* 2841_(A38C - A38Fh) */
	union	io_r2y_egdtmwk		EGDTMWK;		/* 2841_(A390 - A393h) */

	unsigned char dmy_A394_A39F[0xA3A0-0xA394];	/* 2841_(A394 - A39Fh) */
};

/*	structure of MAPSCL area	2841_(A3A0 - A3BFh) */
struct io_r2y_mapscl{
	union	io_r2y_egmpctl		EGMPCTL;		/* 2841_(A3A0 - A3A3h) */
	union	io_r2y_egmpsiz		EGMPSIZ;		/* 2841_(A3A4 - A3A7h) */
	union	io_r2y_egmpsizhinv	EGMPSIZHINV;	/* 2841_(A3A8 - A3ABh) */
	union	io_r2y_egmpsizvinv	EGMPSIZVINV;	/* 2841_(A3AC - A3AFh) */
	union	io_r2y_egmpstaph	EGMPSTAPH;		/* 2841_(A3B0 - A3B3h) */
	union	io_r2y_egmpstapv	EGMPSTAPV;		/* 2841_(A3B4 - A3B7h) */

	unsigned char dmy_A3B8_A3BF[0xA3C0-0xA3B8];	/* 2841_(A3B8 - A3BFh) */
};

/*	structure of EGCMP area	2841_(A3C0 - A3FFh) */
struct io_r2y_egcmp{
	union	io_r2y_egcmpctl		EGCMPCTL;		/* 2841_(A3C0 - A3C3h) */

	unsigned char dmy_A3C4_A3C7[0xA3C8-0xA3C4];	/* 2841_(A3C4 - A3C7h) */

	union	io_r2y_egcmpbd		EGCMPBD;		/* 2841_(A3C8 - A3CFh) */
	union	io_r2y_egcmpalpof	EGCMPALPOF;		/* 2841_(A3D0 - A3D3h) */

	unsigned char dmy_A3D4_A3D7[0xA3D8-0xA3D4];	/* 2841_(A3D4 - A3D7h) */

	union	io_r2y_egcmpalpga	EGCMPALPGA;		/* 2841_(A3D8 - A3DFh) */
	union	io_r2y_egcmpbtaof	EGCMPBTAOF;		/* 2841_(A3E0 - A3E3h) */

	unsigned char dmy_A3E4_A3E7[0xA3E8-0xA3E4];	/* 2841_(A3E4 - A3E7h) */

	union	io_r2y_egcmpbtaga	EGCMPBTAGA;		/* 2841_(A3E8 - A3EFh) */
	union	io_r2y_egcmpclp		EGCMPCLP;		/* 2841_(A3F0 - A3F3h) */

	unsigned char dmy_A3F4_A3FF[0xA400-0xA3F4];	/* 2841_(A3F4 - A3FFh) */
};

/*	structure of CRV area	2841_(A400 - A43Fh) */
struct io_r2y_crv{
	union	io_r2y_crvaf		CRVAF;			/* 2841_(A400 - A403h) */
	union	io_r2y_crvbf		CRVBF;			/* 2841_(A404 - A407h) */

	unsigned char dmy_A408_A43F[0xA440-0xA408];	/* 2841_(A408 - A43Fh) */
};

/*	structure of EGCRV area	2841_(A440 - A47Fh) */
struct io_r2y_egcrv{
	union	io_r2y_egcrvctl		EGCRVCTL;		/* 2841_(A440 - A443h) */
	union	io_r2y_egcrvasclof	EGCRVASCLOF;	/* 2841_(A444 - A447h) */
	union	io_r2y_egcrvasclga	EGCRVASCLGA;	/* 2841_(A448 - A44Bh) */
	union	io_r2y_egcrvasclbd	EGCRVASCLBD;	/* 2841_(A44C - A44Fh) */
	union	io_r2y_egcrvasclcp	EGCRVASCLCP;	/* 2841_(A450 - A453h) */
	union	io_r2y_egyasclga	EGYASCLGA;		/* 2841_(A454 - A457h) */
	union	io_r2y_egyasclbd	EGYASCLBD;		/* 2841_(A458 - A45Bh) */
	union	io_r2y_egyasclclp	EGYASCLCLP;		/* 2841_(A45C - A45Fh) */
	union	io_r2y_egcrvbsclof	EGCRVBSCLOF;	/* 2841_(A460 - A463h) */
	union	io_r2y_egcrvbsclga	EGCRVBSCLGA;	/* 2841_(A464 - A467h) */
	union	io_r2y_egcrvbsclbd	EGCRVBSCLBD;	/* 2841_(A468 - A46Bh) */
	union	io_r2y_egcrvbsclcp	EGCRVBSCLCP;	/* 2841_(A46C - A46Fh) */
	union	io_r2y_egybsclga	EGYBSCLGA;		/* 2841_(A470 - A473h) */
	union	io_r2y_egybsclbd	EGYBSCLBD;		/* 2841_(A474 - A477h) */
	union	io_r2y_egybsclclp	EGYBSCLCLP;		/* 2841_(A478 - A47Bh) */
	union	io_r2y_egcrvclp		EGCRVCLP;		/* 2841_(A47C - A47Fh) */
};

/*	structure of YBCRV area	2841_(A480 - A4FFh) */
struct io_r2y_ybcrv{
	union	io_r2y_ybcrvctl		YBCRVCTL;		/* 2841_(A480 - A483h) */
	union	io_r2y_ybcrvaalpof	YBCRVAALPOF;	/* 2841_(A484 - A487h) */
	union	io_r2y_ybcrvaalpga	YBCRVAALPGA;	/* 2841_(A488 - A48Bh) */
	union	io_r2y_ybcrvaalpbd	YBCRVAALPBD;	/* 2841_(A48C - A48Fh) */
	union	io_r2y_ybcrvaalpcp	YBCRVAALPCP;	/* 2841_(A490 - A493h) */
	union	io_r2y_ybyaalpga	YBYAALPGA;		/* 2841_(A494 - A497h) */
	union	io_r2y_ybyaalpbd	YBYAALPBD;		/* 2841_(A498 - A49Bh) */
	union	io_r2y_ybyaalpclp	YBYAALPCLP;		/* 2841_(A49C - A49Fh) */
	union	io_r2y_ybcrvbalpof	YBCRVBALPOF;	/* 2841_(A4A0 - A4A3h) */
	union	io_r2y_ybcrvbalpga	YBCRVBALPGA;	/* 2841_(A4A4 - A4A7h) */
	union	io_r2y_ybcrvbalpbd	YBCRVBALPBD;	/* 2841_(A4A8 - A4ABh) */
	union	io_r2y_ybcrvbalpcp	YBCRVBALPCP;	/* 2841_(A4AC - A4AFh) */
	union	io_r2y_ybybalpga	YBYBALPGA;		/* 2841_(A4B0 - A4B3h) */
	union	io_r2y_ybybalpbd	YBYBALPBD;		/* 2841_(A4B4 - A4B7h) */
	union	io_r2y_ybybalpclp	YBYBALPCLP;		/* 2841_(A4B8 - A4BBh) */

	unsigned char dmy_A4BC_A4FF[0xA500-0xA4BC];	/* 2841_(A4BC - A4FFh) */
};

/*	structure of CLPF area	2841_(A500 - A57Fh) */
struct io_r2y_clpf{
	union	io_r2y_clpfctl		CLPFCTL;		/* 2841_(A500 - A503h) */

	unsigned char dmy_A504_A51F[0xA520-0xA504];	/* 2841_(A504 - A51Fh) */

	union	io_r2y_clpfk		CLPFK;			/* 2841_(A520 - A533h) */

	unsigned char dmy_A534_A53F[0xA540-0xA534];	/* 2841_(A534 - A53Fh) */

	union	io_r2y_clpfof		CLPFOF;			/* 2841_(A540 - A543h) */

	unsigned char dmy_A544_A547[0xA548-0xA544];	/* 2841_(A544 - A547h) */

	union	io_r2y_clpfga		CLPFGA;			/* 2841_(A548 - A54Fh) */
	union	io_r2y_clpfbd		CLPFBD;			/* 2841_(A550 - A557h) */

	unsigned char dmy_A558_A57F[0xA580-0xA558];	/* 2841_(A558 - A57Fh) */
};

/*	structure of CSP area	2841_(A580 - BFFFh) */
struct io_r2y_csp{
	union	io_r2y_csyctl		CSYCTL;			/* 2841_(A580 - A583h) */

	unsigned char dmy_A584_A587[0xAD88-0xAD84];	/* 2841_(A584 - A587h) */

	union	io_r2y_csyof		CSYOF;			/* 2841_(A588 - A58Fh) */
	union	io_r2y_csyga		CSYGA;			/* 2841_(A590 - A597h) */
	union	io_r2y_csybd		CSYBD;			/* 2841_(A598 - A59Fh) */
	union	io_r2y_ycrvfx		YCRVFX;			/* 2841_(A5A0 - A5A3h) */
	union	io_r2y_cfix			CFIX;			/* 2841_(A5A4 - A5A7h) */
	union	io_r2y_ycof			YCOF;			/* 2841_(A5A8 - A5AFh) */

	unsigned char dmy_A5B0_BFFF[0xC000-0xA5B0];	/* 2841_(A5B0 - BFFFh) */
};

/* Define i/o mapping */
struct io_r2y_fr2y{
	/* JDSR2Y */
	struct io_r2y_fr2y_cntl		CNTL;			/* 2841_(8000 - 801Fh) */
	struct io_r2y_ofg			OFG;			/* 2841_(8020 - 807Fh) */
	struct io_r2y_cca0			CCA0;			/* 2841_(8080 - 8FFFh) */
	struct io_r2y_mcc			MCC;			/* 2841_(9000 - 9FFFh) */
	struct io_r2y_btc			BTC;			/* 2841_(A000 - A03Fh) */
	struct io_r2y_tc_area		TC;				/* 2841_(A040 - A05Fh) */
	struct io_r2y_gam			GAM;			/* 2841_(A060 - A07Fh) */
	struct io_r2y_cca1			CCA1;			/* 2841_(A080 - A0FFh) */
	struct io_r2y_yc_area		YC;				/* 2841_(A100 - A13Fh) */
	struct io_r2y_ynr			YNR;			/* 2841_(A140 - A17Fh) */
	struct io_r2y_egsm			EGSM;			/* 2841_(A180 - A1FFh) */
	struct io_r2y_eghw			EGHW;			/* 2841_(A200 - A27Fh) */
	struct io_r2y_egmw			EGMW;			/* 2841_(A280 - A2FFh) */
	struct io_r2y_eglw			EGLW;			/* 2841_(A300 - A37Fh) */
	struct io_r2y_egdt			EGDT;			/* 2841_(A380 - A39Fh) */
	struct io_r2y_mapscl		MAPSCL;			/* 2841_(A3A0 - A3BFh) */
	struct io_r2y_egcmp			EGCMP;			/* 2841_(A3C0 - A3FFh) */
	struct io_r2y_crv			CRV;			/* 2841_(A400 - A43Fh) */
	struct io_r2y_egcrv			EGCRV;			/* 2841_(A440 - A47Fh) */
	struct io_r2y_ybcrv			YBCRV;			/* 2841_(A480 - A4FFh) */
	struct io_r2y_clpf			CLPF;			/* 2841_(A500 - A57Fh) */
	struct io_r2y_csp			CSP;			/* 2841_(A580 - BFFFh) */
};
