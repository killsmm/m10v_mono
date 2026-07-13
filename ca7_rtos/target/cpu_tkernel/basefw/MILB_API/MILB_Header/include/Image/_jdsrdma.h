#ifndef __JDSRDMA_REG_H__
#define __JDSRDMA_REG_H__

#include "__jdsrdma_top_reg.h"
#include "__jdsrdma_core_reg.h"
#include "__jdspro_prch.h"

/* Define i/o mapping */
struct io_rdma{
	/* JDSIMG */
	struct io_rdma_top		 	RDMA_TOP;					/* 2886_(9000 - 00FFh) */
	struct io_rdma_core			RDMA_CORE;					/* 2886_(9100 - 01FFh) */
	struct io_prch				PRCH0;	 					/* 2886_(9200 - 02FFh) */
	struct io_prch				PRCH1;	 					/* 2886_(9300 - 03FFh) */

	unsigned char dmy_28869400_288697FF[0x28869800-0x28869400];	/* 2886_9400 - 2886_97FFh */

	unsigned char dmy_28869800_28869FFF[0x2886A000-0x28869800];	/* 2886_9800 - 2886_9FFFh */
};

extern volatile struct io_rdma		IO_RDMA;

#endif	  // #define __JDSRDMA_REG_H__

