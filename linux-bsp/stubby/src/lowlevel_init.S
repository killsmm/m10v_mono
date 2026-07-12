/*
 * u-boot/arch/arm/cpu/armv7/mb8ac0300/lowlevel_init.S
 *
 * Copyright (C) 2011-2012 FUJITSU SEMICONDUCTOR LIMITED
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <asm/gic.h>

.extern serial_init
.extern get_boot_secure

/*
 * lowlevel_init
 */
//---------------------------------------------------------------
//   Define(s)
//---------------------------------------------------------------
	.equ	GENERICTIMER_FRQ,		0x02FAF080	//50MHZ

.globl lowlevel_init
.globl	lowlevel_init_2
lowlevel_init:
	//Initialization Generic timer
	ldr		r0, =GENERICTIMER_FRQ
	MCR p15, 0, r0, c14, c0, 0 // Write R0 to CNTFRQ

lowlevel_init_2:
#ifdef CONFIG_MLB01_BOOT_LINUX_ONLY_BSP
	#if defined(CONFIG_NONSECURE_ACCESS)
		bl _get_secondary_entry
		teq r0, #0
		blne SECONDARY_SLEEP
	#endif

#endif //CONFIG_MLB01_BOOT_LINUX_ONLY_BSP
	/* initialize bss area in DDR */

	mov	r0, #0
	ldr	r1, =__bss_start
	ldr	r2, =__bss_end
	b	cn1
cbss:
	str	r0, [r1]
	add	r1, r1, #4
cn1:
	cmp	r1, r2
	bne	cbss
	bl	serial_init
	/* larger stack in DDR now (at least SDMA needs) */

	ldr	sp, =CONFIG_SYS_SDRAM_BASE + OFFSET_DDR_STACK
	isb

	bl boot_sequence

	bl jump_to_linux


