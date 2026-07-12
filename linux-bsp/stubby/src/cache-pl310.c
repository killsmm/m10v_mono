/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Aneesh V <aneesh@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "asm/utils.h"
#include "io.h"

#define printf(fmt,args...)

#ifdef CONFIG_SYS_PL310_BASE

static void pl310_cache_sync(void)
{
	writel(0, PL310_CACHE_SYNC);
}

static void pl310_background_op_all_ways(u32 op_reg)
{
	u32 assoc_16, associativity, way_mask;

	assoc_16 = readl(PL310_AUX_CTRL) &
			PL310_AUX_CTRL_ASSOCIATIVITY_MASK;
	if (assoc_16)
		associativity = 16;
	else
		associativity = 8;

	way_mask = (1 << associativity) - 1;
	/* Invalidate all ways */
	writel(way_mask, op_reg);
	/* Wait for all ways to be invalidated */
	while (readl(op_reg) && way_mask)
		;
	pl310_cache_sync();
}

void v7_outer_cache_inval_all(void)
{
	pl310_background_op_all_ways(PL310_INV_BY_WAY);
}

void v7_outer_cache_flush_all(void)
{
	pl310_background_op_all_ways(PL310_CLEAN_INV_WAY);
}

/* Flush(clean invalidate) memory from start to stop-1 */
void v7_outer_cache_flush_range(u32 start, u32 stop)
{
	/* PL310 currently supports only 32 bytes cache line */
	u32 pa, line_size = 32;

	/*
	 * Align to the beginning of cache-line - this ensures that
	 * the first 5 bits are 0 as required by PL310 TRM
	 */
	start &= ~(line_size - 1);

	for (pa = start; pa < stop; pa = pa + line_size)
		writel(pa, PL310_CLEAN_INV_LINE_PA);

	pl310_cache_sync();
}

/* invalidate memory from start to stop-1 */
void v7_outer_cache_inval_range(u32 start, u32 stop)
{
	/* PL310 currently supports only 32 bytes cache line */
	u32 pa, line_size = 32;

	/*
	 * If start address is not aligned to cache-line do not
	 * invalidate the first cache-line
	 */
	if (start & (line_size - 1)) {
		printf("ERROR: %s - start address is not aligned - 0x%08x\n",
			__func__, start);
		/* move to next cache line */
		start = (start + line_size - 1) & ~(line_size - 1);
	}

	/*
	 * If stop address is not aligned to cache-line do not
	 * invalidate the last cache-line
	 */
	if (stop & (line_size - 1)) {
		printf("ERROR: %s - stop address is not aligned - 0x%08x\n",
			__func__, stop);
		/* align to the beginning of this cache line */
		stop &= ~(line_size - 1);
	}

	for (pa = start; pa < stop; pa = pa + line_size)
		writel(pa, PL310_INV_LINE_PA);

	pl310_cache_sync();
}

void v7_outer_cache_enable(void)
{
	writel(0x1, PL310_CTRL); /* reg1_control Register */
}

void v7_outer_cache_disable(void)
{
	writel(0x0, PL310_CTRL); /* reg1_control Register */
}
#endif
