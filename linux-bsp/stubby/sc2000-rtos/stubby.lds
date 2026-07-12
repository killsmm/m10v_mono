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
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)

/*
 * The sizes of the stacks used by the application. NOTE: you need to adjust
 */

IRQ_STACK_SIZE = 2048;
FIQ_STACK_SIZE = 128;
SVC_STACK_SIZE = 2048;
SYS_STACK_SIZE = 2048;
ABT_STACK_SIZE = 128;
UND_STACK_SIZE = 128;

/*
 * All Stubby builds follow this layout template
 */

SECTIONS
{
	. = 0x0;

	stubby_start = .;
	.text :{
		*(.initialtext)
		. = ALIGN(0x100);
		*(._secure.text)
		*(.got.plt)
		*(.got)
		*(.igot.plt)
		*(.igot)
		*(.text)
		*(.text.unlikely)
		. = ALIGN(4);
		/* 
		 * gcc has problems dealing with PIC data that is not stored in
		 * .text, so we store it in the image directly, which disallows
		 * RO XIP unfortunately
		 */
		*(.rodata)
		*(.rodata.str1.1)
		. = ALIGN(4);
		*(.data*)
	}

	/* if we don't mention him as NOLOAD then ld adds him in a bad way */
	.exidx (NOLOAD) : {
		*(.ARM.exidx)
	}

	stubby_end = .;

	/*
	 * rest is NOLOAD in DDR
	 */

	. = 0x00004000+0x40000000;
	
	__bss_start = .;
	.bss (NOLOAD):  {
		*(.bss)
		. = ALIGN(4);
		*(.COMMON)
	}
	. = ALIGN(4);
	__bss_end = .;

	.sdram (NOLOAD):  {
		*(.sdram_data)
	}

	.stack : {
	    __stack_start__ = . ;

		STACK_AREA = .;
		* (.USER_STACK_AREA);
		. += 0x80;
		. = ALIGN(64);
		STACK_AREA_UNDEF = .;
		. += 0x80;
		. = ALIGN(64);
		STACK_AREA_ABT = .;
		. += 0x800;
		. = ALIGN(64);
		STACK_AREA_SVC_CPU0 = .;
		. += 0x800;
		STACK_AREA_SVC_CPU1 = .;
		. += 0x80;
		STACK_AREA_SVC_CPU2 = .;
		. += 0x800;
		STACK_AREA_SVC_CPU3 = .;
		. = ALIGN(64);
		STACK_AREA_FIQ = .;
		. += 0x800;
		. = ALIGN(64);
		STACK_AREA_IRQ = .;
		. += 0x80;
		. = ALIGN(64);
		STACK_AREA_IDLE = .;

	    __stack_end__ = .;
	}

	/*
	 * remainder of DDR until pagetables is for malloc malloc_pool
	 */

	malloc_pool = ALIGN(4);
}
