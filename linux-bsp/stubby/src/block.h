/*
 * block.h: block and filesystem I/O header
 *
 * (C) Copyright 2013 Linaro Ltd.
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

enum boot_source {
#if defined(CONFIG_F_SDH30_SDHCI)
	BS_SD,
#endif
#if defined(F_EMMC_BASE)
	BS_EMMC,
#endif
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	BS_MEM,
#endif
	BS_COUNT
};

enum filesystem_types {
#if defined(CONFIG_FS_VFAT)
	FS_FAT,
#endif
#if defined(CONFIG_FS_EXT2)
	FS_EXT2,
#endif
#if defined(CONFIG_ROMFS_POSSIBLE_ADDRESSES)
	FS_ROMFS,
#endif
	FS_COUNT,
};

extern unsigned int partition_offset_blocks, partition_length_blocks;                  
extern unsigned long (*bread)(ulong start, unsigned int blkcnt, void *dst);
extern int (*filesystem_read)(const char *filename, void *buffer, unsigned long maxsize);
extern int parse_partition(int partition_index);
extern int romfs_mount(const void *_romfs);
extern int partition_read_table(void);
extern void *romfs_filesystem_get_ads(const char *filepath, int *len);

