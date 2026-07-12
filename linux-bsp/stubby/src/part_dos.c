/*
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
#include "asm/utils.h"
#include "string.h"
#include "asm/system.h"

unsigned int partition_offset_blocks, partition_length_blocks;
unsigned long (*bread)(ulong start, unsigned int blkcnt, void *dst);
int (*filesystem_read)(const char *filename, void *buffer,
						unsigned long maxsize);
unsigned char table[1024];

int partition_read_table(void)
{

	partition_offset_blocks = 0;
	partition_length_blocks = 0xffffffff;

	return bread(0, 1, table) != 1;
}

int parse_partition(int partition_index)
{
	unsigned char *t;
#ifdef CONFIG_FOLLOW_PARTITION_BOOT_FLAG
	/* 
	 * with this config, the rule is if anyone has a
	 * bootable flag, skip the ones that don't have it
	 */
	unsigned int anybody_bootable = (table[0x1be] & 0x80) |
			       (table[0x1ce] & 0x80) |
			       (table[0x1de] & 0x80) |
			       (table[0x1ee] & 0x80);
#endif

	if ((table[0x1fe] != 0x55) || (table[0x1ff] != 0xaa)) {
		puts("bad part table\n");
		return 1;
	}

	t = table + 0x1be + (0x10 * partition_index);

	/* system type 0 definitely means unused partition */
	if (t[4] == 0)
		return 2;

#if 0
	partition_offset_blocks = (((u32)t[11]) << 24) |
				  (((u32)t[10]) << 16) |
				  (((u32)t[9])  << 8) |
				  t[8];
	DMB();
    partition_length_blocks = (((u32)t[15]) << 24) |
                  (((u32)t[14]) << 16) |
                  (((u32)t[13]) << 8) |
                  t[12];
#else
	partition_offset_blocks = (t[11] << 24);
	partition_offset_blocks |=(t[10] << 16);
	partition_offset_blocks |=(t[9]  << 8);
	DMB();
	partition_offset_blocks |= t[8];
	DMB();
	partition_length_blocks = (t[15] << 24);
	DMB();
	partition_length_blocks |=(t[14] << 16);
	DMB();
	partition_length_blocks |=(t[13] << 8);
	DMB();
	partition_length_blocks |= t[12];
#endif
	DMB();
	/* zero length partition would mean unused partition */
	if (partition_length_blocks == 0)
		return 1;

#ifdef CONFIG_FOLLOW_PARTITION_BOOT_FLAG
	DMB();
	if ((!(t[0] & 0x80)) && anybody_bootable)
		/* if anyone bootable, reject this non-bootable guy */
		return 3;
#endif
	return 0;
}

