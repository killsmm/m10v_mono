/*
 *----------------------------------------------------------------------
 *    SMP T-Kernel
 *
 *    Copyright (C) 2007-2010 Ken Sakamura. All Rights Reserved.
 *    SMP T-Kernel is distributed under the T-License for SMP T-Kernel.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.01
 *    Released by T-Engine Forum(http://www.t-engine.org) at 2010/02/19.
 *
 *----------------------------------------------------------------------
 */

/*
 *	@(#)segment.h (sys)
 *
 *	Segment management
 */

#ifndef __MPTK_SYS_SEGMENT_H__
#define __MPTK_SYS_SEGMENT_H__

#include <mptk_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * GetSpaceInfo()
 */
typedef struct t_spinfo {
	VP	paddr;
	VP	page;
	INT	pagesz;
	INT	cachesz;
	INT	cont;
} T_SPINFO;

/*
 * FlushMemCache() mode
 * ControlCache() mode
 * ControlCacheAll() mode
 */
#define CC_FLUSH			0x01	/* Cache Fluch (write back) */
#define CC_INVALIDATE		0x02	/* Cache Invalidate */
#define CC_SELF				0x04	/* Only my processor specifies */

/*
 * FlushMemCache() cache
 * ControlCache() cache
 * ControlCacheAll() mode
 */
#define INSTRUCTION_CACHE	0x01	/* Apply to Instruction cache */
#define DATA_CACHE			0x02	/* Apply to Data cache */

/*
 * MapMemory() attr
 */
#define MM_USER		0x20U	/* User */
#define MM_SYSTEM	0x00U	/* System */
#define MM_READ		0x00U	/* Read */
#define MM_WRITE	0x10U	/* Write */
#define MM_EXECUTE	0x01U	/* Execute */
#define MM_CDIS		0x0cU	/* Cache disabled */

/* [BEGIN SYSCALLS] */
/* ALIGN_NO 0x100 */
IMPORT ER GetSpaceInfo( VP addr, INT len, T_SPINFO *pk_spinfo );
IMPORT INT CnvPhysicalAddr( VP laddr, INT len, VP *paddr );
IMPORT ER  ControlCache( VP laddr, INT len, UINT mode, UINT cache );
IMPORT ER  ControlCacheAll( UINT mode, UINT cache );

/* ALIGN_NO 0x100 */
IMPORT ER MapMemory( VP paddr, INT len, UINT attr, VP *laddr );
IMPORT ER UnmapMemory( VP laddr );
/* [END SYSCALLS] */

#ifdef __cplusplus
}
#endif
#endif /* __MPTK_SYS_SEGMENT_H__ */
