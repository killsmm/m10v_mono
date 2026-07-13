#ifndef __JDSCNR_REG_H__
#define __JDSCNR_REG_H__

#include "_jdscnr_otf_reg_rw.h"
#include "_jdscnr_cspr_reg.h"
#include "_jdscnr_ofl_reg_rw.h"
#include "_jdscnr_ofl_reg_yspr.h"

/* Define i/o mapping */
struct io_cnr_reg{
	/* JDSIMG */
	struct io_cnr_otf_reg_rw	OTF_REG_RW;					/* 2842_(0000 - 0FFFh) */
	struct io_cnr_cspr_reg		OTF_CSPR_REG;				/* 2842_(1000 - 1FFFh) */
	struct io_cnr_ofl_reg_rw	OFL_REG_RW;					/* 2842_(2000 - 2FFFh) */
	struct io_cnr_cspr_reg		OFL_CSPR_REG;				/* 2842_(3000 - 3FFFh) */
	struct io_cnr_ofl_reg_yspr	OFL_REG_YSPR;				/* 2842_(4000 - 4FFFh) */

	unsigned char dmy_28425000_2842FFFF[0x28430000-0x28425000];	/* 2842_5000 - 2842_FFFFh */

};

extern volatile struct io_cnr_reg		IO_CNR1;
extern volatile struct io_cnr_reg		IO_CNR2;
extern volatile struct io_cnr_reg		IO_CNR3;

#endif	// #define __JDSCNR_REG_H__

