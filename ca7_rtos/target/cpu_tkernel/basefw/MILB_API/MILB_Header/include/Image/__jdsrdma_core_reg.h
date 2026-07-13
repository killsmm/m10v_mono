#ifndef __JDSRDMA_CORE_REG_H__
#define __JDSRDMA_CORE_REG_H__


/*	structure of RDMATRG (2886_9100h)	*/
union io_rmda_core_rdmatrg{
	unsigned long		word;
	struct {
		unsigned long	RDMATRG		:2;
		unsigned long				:30;
	}bit;
};

/*	structure of RDMAINTENB (2886_9104h)	*/
union io_rmda_core_rdmaintenb{
	unsigned long		word;
	struct {
		unsigned long	RDMAE	:1;
		unsigned long			:3;
		unsigned long	RDMAXE	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of RDMAINTFLG (2886_9108h)	*/
union io_rmda_core_rdmaintflg{
	unsigned long		word;
	struct {
		unsigned long	RDMAF	:1;
		unsigned long			:3;
		unsigned long	RDMAXF	:1;
		unsigned long			:27;
	}bit;
};

/*	structure of RDMAAXCTL (2886_910Ch)	*/
union io_rmda_core_rdmaaxctl{
	unsigned long		word;
	struct {
		unsigned long	AWCACHE	:4;
		unsigned long			:4;
		unsigned long	AWPROT	:3;
		unsigned long			:21;
	}bit;
};

/*	structure of RDMABRESP (2886_9110h)	*/
union io_rmda_core_rdmabresp{
	unsigned long		word;
	struct {
		unsigned long	RDMABRESP	:2;
		unsigned long				:30;
	}bit;
};

/*	structure of RDMAPRCHERR (2886_9114h)	*/
union io_rmda_core_rdmaprcherr{
	unsigned long		word;
	struct {
		unsigned long	RDMAPRCHERR	:1;
		unsigned long				:31;
	}bit;
};

/* Define i/o mapping */
struct io_rdma_core{
	/* JDSIMG */
	union  io_rmda_core_rdmatrg			RDMATRG;		/* 2886_(9100 - 0103h) */
	union  io_rmda_core_rdmaintenb		RDMAINTENB;		/* 2886_(9104 - 0107h) */
	union  io_rmda_core_rdmaintflg		RDMAINTFLG;		/* 2886_(9108 - 010Bh) */
	union  io_rmda_core_rdmaaxctl		RDMAAXCTL;		/* 2886_(910C - 010Fh) */
	union  io_rmda_core_rdmabresp		RDMABRESP;		/* 2886_(9110 - 0113h) */
	union  io_rmda_core_rdmaprcherr		RDMAPRCHERR;	/* 2886_(9114 - 0117h) */

	unsigned char dmy_0018_00FF[0x0100-0x0018];	/* 2886_(9018 - 00FFh) */
};

#endif	  // #define __JDSRDMA_CORE_REG_H__
