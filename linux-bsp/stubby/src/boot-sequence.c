/*
 * boot-sequence.c: Stubby boot sequence
 *
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
#include "asm/utils.h"
#include "mmc.h"
#include "fat.h"
#include "block.h"
#include "io.h"
#include "string.h"
#include "crypto/crypto.h"
#include "device.h"
#include "utilsC.h"
#include "ext2.h"

#ifdef CONFIG_NONSECURE_ACCESS
#include "asm/nonsec.h"
#endif

#if !defined(CONFIG_FS_VFAT) && !defined(CONFIG_FS_EXT2) && defined(CONFIG_F_SDH30_SDHCI)
#error "You need to define FAT or EXT2 if you want to use SD or eMMC"
#endif

extern void *load_ads_kernel, *load_ads_dtb;

#if defined(CONFIG_BOOT_SOURCE_MENU)
int source_skip;
int force_part;
int console_key;
#endif

int rootfstype;

/* 
 * AG 2014-09-07
 * putting * const devname here killed the ability of mmc to load a kernel
 * without the const, an extra pic_dereference is needed in describe() that is
 * not needed when using the same arrays in print_stubby_info()
 */

static const char * /* const */ devname[] = {
#if defined(CONFIG_F_SDH30_SDHCI)
	[BS_SD] = "SD",
#endif
#if defined(F_EMMC_BASE)
	[BS_EMMC] = "eMMC",
#endif
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	[BS_MEM] = "mem",
#endif
};

static const char * /* const */ fsname[] = {
#if defined(CONFIG_FS_VFAT)
	[FS_FAT] = "VFAT",
#endif
#if defined(CONFIG_FS_EXT2)
	[FS_EXT2] = "ext4",
#endif
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	[FS_ROMFS] = "ROMFS",
#endif
};

#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
static const u32 romfs_ads[] = { CONFIG_ROMFS_POSSIBLE_ADDRESSES };
#endif

void print_stubby_info(void)
{
	mhu_show_version();

	puts(" CPU 0x");
	print32_noleading(read_mpidr & 0xffff);

#ifdef CONFIG_RSA_SIGNATURE
#ifdef CONFIG_RSA_SIGNATURE_ENFORCING
	puts(" Enforcing");
#else
	puts(" NON-ENFORCING");
#endif
#ifdef CONFIG_HASH_TYPE_SHA1
	puts(" SHA-1/");
#endif
#ifdef CONFIG_HASH_TYPE_SHA256
	puts(" SHA-256/");
#endif
	print_pubkey_fingerprint();
	puts("\n");
#else
	puts(", (No security)\n");
#endif
#if defined(CONFIG_BOOT_SOURCE_MENU)
	puts(" Press Enter for boot source selection\n");
#endif
}

#if defined(CONFIG_BOOT_SOURCE_MENU)
int check_console_key(void)
{
	console_key = getc();
	/* swallow \n from \r\n */
	if (console_key == '\n')
		console_key = 0;

	return console_key > 0;
}
#endif

void describe(int boot_source, int part, int rootfstype)
{
	const char *c = devname[boot_source];

//	puts(c);
	puts(pic_dereference(c));
	puts(" p");
	printdec(part + 1);
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	if (boot_source == BS_MEM && part < ARRAY_SIZE(romfs_ads)) {
		puts("@0x");
		print32((u32)romfs_ads[part]);
	}
#endif
	puts(": ");
	c = fsname[rootfstype];
	puts(pic_dereference(c));
	putc('\n');
}

int select_boot_source(enum boot_source boot_source)
{
	switch (boot_source) {
#if defined(CONFIG_F_SDH30_SDHCI)
	case BS_SD:
		if (f_sdh30_init(0))
			return 1;
		break;
#endif
#if defined(F_EMMC_BASE)
	case BS_EMMC:
		if (f_sdh30_init(1))
			return 1;
		break;
#endif
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	case BS_MEM:
		break;
#endif
	default:
		return 1;
	}

#if defined(CONFIG_FS_VFAT) || defined(CONFIG_FS_EXT2)
	if (
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	    boot_source != BS_MEM &&
#endif
	    partition_read_table()) {
		puts("partition read failed\n");
		return 1;
	}
#endif

	return 0;
}

int try_boot_source(enum boot_source boot_source, int part, int check_only)
{
	void *romfs;
	int r;
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	if (boot_source != BS_MEM)
#endif
	{
		r = parse_partition(part);
		if (r)
			return 2;
#ifdef CONFIG_FS_VFAT
			if (!fat_check()) {
				rootfstype = FS_FAT;
				goto mounted;
			}
#endif
#ifdef CONFIG_FS_EXT2
			if (!ext2fs_mount()) {
				rootfstype = FS_EXT2;
				goto mounted;
			}
#endif
		return 1;
	}
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	else {

		if (part >= ARRAY_SIZE(romfs_ads))
			return 2;

		romfs = (void *)romfs_ads[part];
		if (romfs_mount(romfs))
			return 1;
		
		rootfstype = FS_ROMFS;
	}
#endif

mounted:
	if (check_only)
		return 1;
	
	describe(boot_source, part, rootfstype);

	return load_from_filesystem(boot_source, part + 1, rootfstype);
}

void boot_sequence(void)
{
	enum boot_source boot_source = 0; /* ie, first valid source */
	unsigned int ads;
#if defined(CONFIG_BOOT_SOURCE_MENU)
	char s[12];
	int timeout;
#endif
	int part;
	int check = 0;
	int r;

#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	mb86s7x_hsspi_init();
#endif
	print_stubby_info();
	
#if defined(CONFIG_BOOT_SOURCE_MENU)
redo:
	strcpy(s, "   'a' =");
	boot_source = 0; /* ie, first valid source */

	if (check) {
		puts("  Select: ");
		timeout = 10000;
		while (--timeout && !check_console_key())
			udelay(4000);
		check = 0;
		if (console_key >= 'a' && console_key <= 'z')
			putc(console_key);
		puts("\n");
	}
	if (console_key) {
		if (console_key == '\r') {
			puts("\n  Possible boot sources\n");
			check = 1;
		}

		if (console_key >= 'a' && console_key <= 'z')
			source_skip = console_key - 'a';
		
		console_key = 0;
	}
#endif
	/* we'll try all the boot sources we support, in turn */
	
	while (boot_source < BS_COUNT) {

#if defined(CONFIG_BOOT_SOURCE_MENU)
		if (!check && check_console_key())
			goto redo;
#endif
		
		if (!select_boot_source(boot_source)) {
		
			/* 
			 * and we'll try each partion in turn, until there's an
			 * invalid one.  For BS_MEM each valid memory region is
			 * a "partiton" and none is invalid until we run out
			 */
			part = 0;
			do {
#if defined(CONFIG_BOOT_SOURCE_MENU)
				if (!check && check_console_key())
					goto redo;
#endif
				r = try_boot_source(boot_source, part,
						    	check
#if defined(CONFIG_BOOT_SOURCE_MENU)
							 || source_skip);
				if (r != 2 && source_skip)
					source_skip--;

				if (!check && check_console_key())
					goto redo;
#else
							);
#endif
				if (r == 2) /* invalid partition */
					break;
#if defined(CONFIG_BOOT_SOURCE_MENU)
				if (check) {
					puts(s);
					s[4]++;
				
					describe(boot_source, part, rootfstype);
				}
#endif
				if (!check && !r) /* boot it */ {
					puts(" Booting\n");
					return;
				}
				part++;
			} while (1);
		}

		boot_source++;
	}
#if defined(CONFIG_BOOT_SOURCE_MENU)
	if (check)
		goto redo;
#endif
	/* we have failed to find anything we can boot */

	puts("boot failed\r\n\r\n");

#ifdef CONFIG_MLB01
	{
#if 0
		volatile u32 *p = (volatile u32 *)0x18002000;

		/* force PA4 low output to reset */

//		p[(0x200 + 0x1c) >> 2] = 0;

		p[(0xa0 + 0x1c) >> 2] = 0x10; /* clear data */
		p[(0x100 + 0x1c) >> 2] |= 0x10; /* output */
#endif
	}
#endif

	while (1)
		;
}

void jump_to_linux(void)
{
	void (*kernel_entry)(int zero, int arch, uint params);
	kernel_entry = load_ads_kernel;
	unsigned int* pTimestamp=(unsigned int*)0x1d010000;
	*pTimestamp = 1;//Enable TimeStamp
#if defined(CONFIG_NONSECURE_ACCESS)
		if(!get_boot_secure()) {
			armv7_init_nonsec();
			(_do_nonsec_entry)(kernel_entry, 0, 
				0xFFFFFFFF, (u32)load_ads_dtb);
		}
		else
			kernel_entry(0, 0xFFFFFFFF, (u32)load_ads_dtb);
#else
		kernel_entry(0, 0xFFFFFFFF, (u32)load_ads_dtb);
#endif	
}


