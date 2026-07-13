#ifndef __JDSRDMA_TOP_REG_H__
#define __JDSRDMA_TOP_REG_H__


/*	structure of SR (2886_9000h)	*/
union io_rmda_top_sr{
	unsigned long		word;
	struct {
		unsigned long	SR		:1;
		unsigned long			:31;
	}bit;
};

/*	structure of INTMON1 (2886_9004h)	*/
union io_rmda_top_intmon1{
	unsigned long		word;
	struct {
		unsigned long	RDMAF	:1;
		unsigned long	PRF0	:1;
		unsigned long	PRF1	:1;
		unsigned long			:29;
	}bit;
};

/*	structure of INTMON2 (2886_9008h)	*/
union io_rmda_top_intmon2{
	unsigned long		word;
	struct {
		unsigned long	RDMAXF	:1;
		unsigned long	PRXF0	:1;
		unsigned long	PRXF1	:1;
		unsigned long			:29;
	}bit;
};

/*	structure of INTCTL (2886_900Ch)	*/
union io_rmda_top_intctl{
	unsigned long		word;
	struct {
		unsigned long	INTMD	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of VDGEN0 (2886_9010h)	*/
union io_rmda_top_vdgen0{
	unsigned long		word;
	struct {
		unsigned long	VDGEN0	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of VDGEN1 (2886_9014h)	*/
union io_rmda_top_vdgen1{
	unsigned long		word;
	struct {
		unsigned long	VDGEN1	:1;
		unsigned long			:31;
	}bit;
};

/* Define i/o mapping */
struct io_rdma_top{
	/* JDSIMG */
	union  io_rmda_top_sr			SR;			/* 2886_(9000 - 0003h) */
	union  io_rmda_top_intmon1		INTMON1;	/* 2886_(9004 - 0007h) */
	union  io_rmda_top_intmon2		INTMON2;	/* 2886_(9008 - 000Bh) */
	union  io_rmda_top_intctl		INTCTL;		/* 2886_(900C - 000Fh) */
	union  io_rmda_top_vdgen0		VDGEN0;		/* 2886_(9010 - 0013h) */
	union  io_rmda_top_vdgen1		VDGEN1;		/* 2886_(9014 - 0017h) */

	unsigned char dmy_0018_00FF[0x0100-0x0018];	/* 2886_(9018 - 00FFh) */
};

#endif	  // #define __JDSRDMA_TOP_REG_H__
