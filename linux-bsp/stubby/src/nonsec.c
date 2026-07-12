/*
 * (C) Copyright 2014 Linaro Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifdef CONFIG_NONSECURE_ACCESS

#include "asm/utils.h"
#include "io.h"
#include "asm/gic.h"
#include "asm/nonsec.h"
#include "mhu.h"

static unsigned int read_id_pfr1(void)
{
	unsigned int reg;

	asm("mrc p15, 0, %0, c0, c1, 1\n" : "=r"(reg));
	return reg;
}

static unsigned long get_gicd_base_address(void)
{
	return CONFIG_ARM_GIC_BASE_ADDRESS + GIC_DIST_OFFSET;
}

int armv7_init_nonsec(void)
{
	unsigned int reg;
	unsigned itlinesnr, i;
	unsigned long gic_dist_addr;
	void (*fn)(void);

	/* check whether the CPU supports the security extensions */
	reg = read_id_pfr1();
	if ((reg & 0xF0) == 0) {
		printf("nonsec: Security extensions not implemented.\n");
		return -1;
	}

	/* the SCR register will be set directly in the monitor mode handler,
	 * according to the spec one should not tinker with it in secure state
	 * in SVC mode. Do not try to read it once in non-secure state,
	 * any access to it will trap.
	 */

	gic_dist_addr = get_gicd_base_address();
	if (gic_dist_addr == -1)
		return -1;

	/* enable the GIC distributor */
	writel(readl(gic_dist_addr + GICD_CTLR) | 0x03,
	       gic_dist_addr + GICD_CTLR);

	/* TYPER[4:0] contains an encoded number of available interrupts */
	itlinesnr = readl(gic_dist_addr + GICD_TYPER) & 0x1f;

	/* set all bits in the GIC group registers to one to allow access
	 * from non-secure state. The first 32 interrupts are private per
	 * CPU and will be set later when enabling the GIC for each core
	 */
	for (i = 1; i <= itlinesnr; i++)
		writel((unsigned)-1, gic_dist_addr + GICD_IGROUPRn + 4 * i);

	/* call the non-sec switching code on this CPU also */
	fn = pic_dereference(_nonsec_init);
	fn();
	return 0;
}

extern unsigned int secure_boot;
int get_boot_secure(void)
{
	int *secure = (int *)pic_dereference(&secure_boot);
	if(*secure != 0xffffffff)
		return *secure;
	
	*secure = mhu_check_secure_boot(); 

	/* In secure, we need to use VA, but in non-secure we can use PA */
	if (*secure) {
		asm("ldr r1, =0xfe100200\n\t" \
			"mcr	p15, 0, r1, c12, c0, 1\n\t"
			: : :  
		);
	}
	return *secure;
}

#endif

