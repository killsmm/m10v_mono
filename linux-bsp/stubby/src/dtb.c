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
#include "asm/utils.h"
#include "mmc.h"
#include "fat.h"
#include "block.h"
#include "io.h"
#include "string.h"
#include "crypto/crypto.h"
#include "device.h"

#define OF_DT_HEADER            0xd00dfeed      /* marker */
#define OF_DT_BEGIN_NODE        0x1             /* Start of node, full name */
#define OF_DT_END_NODE          0x2             /* End node */
#define OF_DT_PROP              0x3             /* Property: name off, size,
                                                 * content */
#define OF_DT_NOP               0x4             /* nop */
#define OF_DT_END               0x9

#define OF_DT_VERSION           0x10

void fdt_set_prop_size(void *start, void *_prop, int new_size)
{
	unsigned char *prop = _prop - 12;
	int rounded_delta, rounded_len, rounded_new;
	int l = __be_to_cpu32(prop + 4);
	u32 total_len = __be_to_cpu32(start + 4);
	unsigned char *c;
	
	rounded_len = (l + 3) & ~(4 - 1);
	rounded_new = (new_size + 3) & ~(4 - 1);
	rounded_delta = rounded_new - rounded_len;

	/* allowing for the rounding padding, does anything need to happen? */
	if (!rounded_delta)
		return;

	if (rounded_delta > 0) /* bigger - leaves current payload adds space */
		for (c = start + total_len - 1; c >= (prop + 12 + l); c--)
			c[rounded_delta] = *c;
	else /* smaller - truncates end of current payload */
		for (c = prop + 12 + rounded_new; c < (u8 *)start + total_len; c++)
			*c = c[-rounded_delta];

	/* change our own record of the length */
	uaw_be32(prop + 4, new_size);
	/* change our total length */
	uaw_be32(start + 4, total_len + rounded_delta);
	/* the strings have also moved accordingly */
	uaw_be32(start + 12, __be_to_cpu32(start + 12) + rounded_delta);
	/* and the structure block changed in size */
	uaw_be32(start + 0x24, __be_to_cpu32(start + 0x24) + rounded_delta);
}

void *fdt_find_path(void *start, const char *path, int *len)
{
	int level = 0;
	const char *match = path, *s;
	int n, match_level = 0;
	const char *strings = start + __be_to_cpu32(start + 12);
	unsigned char *parent = start + __be_to_cpu32(start + 8);

	while (1) {
		switch (__be_to_cpu32(parent)) {
		case OF_DT_BEGIN_NODE:
			parent += 4;
			n = 0;
			while (match[n] && match[n] != '/' &&
					  parent[n] && parent[n] == match[n])
				n++;

			if (!parent[n] && match[n] == '/') {
				match += n + 1;
				match_level++;
			}

			parent += n;
			while (*parent)
				parent++;
			level++;
			parent += (((unsigned long)parent & 3) ^ 3) + 1;
			break;
		case OF_DT_END_NODE:
			level--;
			if (level < match_level || level <= 0)
				return NULL;
			parent += 4;
			break;
		case OF_DT_PROP:
			s = strings + __be_to_cpu32(parent + 8);
			n = 0;
			while (match[n] && match[n] != '/' &&
					   s[n] && s[n] == match[n])
				n++;

			if (match[n] || s[n]) {
				parent += 12 + __be_to_cpu32(parent + 4);
				if ((unsigned long)parent & 3)
					parent += (((unsigned long)parent & 3) ^ 3) + 1;
				break;
			}

			*len = __be_to_cpu32(parent + 4);
			return parent + 12;

		case OF_DT_NOP: /* NOP */
			parent += 4;
			break;
		case OF_DT_END:
		default:
			return NULL;
		}
	}
}

int update_dtb(char *dtb, enum boot_source device, int partition,
	       enum filesystem_types rootfstype)
{
	char rootfs_string[256];
	char *tmp = rootfs_string, *param;
	char part_string[] = "p0 ";
	int m, n;
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES) && !defined(CONFIG_NOR_DEVICE)
	if (device == BS_MEM)
		return 0;
#endif

	/* make the root file system information string */
	strcpy(tmp, " root=/dev/");

#ifdef CONFIG_FS_VFAT
	/*
	 * since VFAT cannot be the rootfs, if kernel found there
	 * try to use next partition + force ext2/3/4
	 */
	if (rootfstype == FS_FAT) {
		partition++;
		rootfstype = FS_EXT2;
	}
#endif

	part_string[1] += partition;

	switch (device) {
#if defined(CONFIG_F_SDH30_SDHCI)
	case BS_SD:
		tmp = strcat(tmp, CONFIG_SD_DEVICE);
		tmp = strcat(tmp, part_string);
		break;
#endif
#if defined(F_EMMC_BASE)
	case BS_EMMC:
		tmp = strcat(tmp, CONFIG_EMMC_DEVICE);
		tmp = strcat(tmp, part_string);
		break;
#endif
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES) && defined(CONFIG_NOR_DEVICE)
	case BS_MEM:
		tmp = strcat(tmp, CONFIG_NOR_DEVICE);
		*tmp++ = ' ';
		*tmp++ = '\0';
		break;
#endif
	default:
		return 1;
	}

	tmp = strcat(tmp, "rootfstype=");

	switch (rootfstype) {
	/* FS_FAT is not possible */
#if defined(CONFIG_FS_EXT2)
	case FS_EXT2:
		tmp = strcat(tmp, "ext4 ");
		break;
#endif
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	case FS_ROMFS:
		tmp = strcat(tmp, "romfs ");
		break;
#endif
	default:
		return 2;
	}

	/* append the bootargs stubby computed */

	param = fdt_find_path(dtb, "/chosen/bootargs", &n);
	if (!param) {
		puts("no chosen node\n");
		return 3;
	}
	fdt_set_prop_size(dtb, param, n + tmp - rootfs_string);
	strncpy(param + n - 1, rootfs_string, tmp - rootfs_string + 1);

	puts("  /chosen/bootargs += \"");
	puts(rootfs_string);
	puts("\"\n");

	return 0;
}
