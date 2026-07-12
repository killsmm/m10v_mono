/*
 *(C) Copyright 2004
 *  esd gmbh <www.esd-electronics.com>
 *  Reinhard Arlt <reinhard.arlt@esd-electronics.com>
 *
 *  based on code from grub2 fs/ext2.c and fs/fshelp.c by
 *
 * Adapted by Andy Green <andy.green@linaro.org> 2010-2013
 * Malloc can't fail in a bootloader
 *
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright(C) 2003, 2004  Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "types.h"
#include "ext2.h"
#include "string.h"
#include "block.h"
#include "asm/utils.h"

/* Magic value used to identify an ext2 filesystem.  */
#define	EXT2_MAGIC		0xEF53
/* Amount of indirect blocks in an inode.  */
#define INDIRECT_BLOCKS		12
/* Maximum lenght of a pathname.  */
#define EXT2_PATH_MAX		4096
/* Maximum nesting of symlinks, used to prevent a loop.  */
#define	EXT2_MAX_SYMLINKCNT	8

/* Filetype used in directory entry.  */
#define	FILETYPE_UNKNOWN	0
#define	FILETYPE_REG		1
#define	FILETYPE_DIRECTORY	2
#define	FILETYPE_SYMLINK	7

/* Filetype information as used in inodes.  */
#define FILETYPE_INO_MASK	0170000
#define FILETYPE_INO_REG	0100000
#define FILETYPE_INO_DIRECTORY	0040000
#define FILETYPE_INO_SYMLINK	0120000

/* Bits used as offset in sector */
#define DISK_SECTOR_BITS        SECTOR_BITS

/* Log2 size of ext2 block in 512 blocks.  */
#define LOG2_EXT2_BLOCK_SIZE(data)(__le32_to_cpu(data->sblock.log2_block_size) + 1)

/* Log2 size of ext2 block in bytes.  */
#define LOG2_BLOCK_SIZE(data)	  (__le32_to_cpu(data->sblock.log2_block_size) + 10)

/* The size of an ext2 block in bytes.  */
#define EXT2_BLOCK_SIZE(data)	  (1 << LOG2_BLOCK_SIZE(data))

#define EXT2_GOOD_OLD_REV       0       /* The good old (original) format */
#define EXT2_DYNAMIC_REV        1       /* V2 format w/ dynamic inode sizes */

#define EXT2_GOOD_OLD_INODE_SIZE 128
uint32_t ext2_inode_size;

/* The ext2 superblock.  */
struct ext2_sblock {
	uint32_t total_inodes;
	uint32_t total_blocks;
	uint32_t reserved_blocks;
	uint32_t free_blocks;
	uint32_t free_inodes;
	uint32_t first_data_block;
	uint32_t log2_block_size;
	uint32_t log2_fragment_size;
	uint32_t blocks_per_group;
	uint32_t fragments_per_group;
	uint32_t inodes_per_group;
	uint32_t mtime;
	uint32_t utime;
	uint16_t mnt_count;
	uint16_t max_mnt_count;
	uint16_t magic;
	uint16_t fs_state;
	uint16_t error_handling;
	uint16_t minor_revision_level;
	uint32_t lastcheck;
	uint32_t checkinterval;
	uint32_t creator_os;
	uint32_t revision_level;
	uint16_t uid_reserved;
	uint16_t gid_reserved;
	uint32_t first_inode;
	uint16_t inode_size;
	uint16_t block_group_number;
	uint32_t feature_compatibility;
	uint32_t feature_incompat;
	uint32_t feature_ro_compat;
	uint32_t unique_id[4];
	char volume_name[16];
	char last_mounted_on[64];
	uint32_t compression_info;
};

/* The ext2 blockgroup.  */
struct ext2_block_group {
	uint32_t block_id;
	uint32_t inode_id;
	uint32_t inode_table_id;
	uint16_t free_blocks;
	uint16_t free_inodes;
	uint16_t  used_dir_cnt;     /* Directories count */
	uint16_t bg_flags;
	uint32_t bg_reserved[2];
	uint16_t bg_itable_unused; /* Unused inodes count */
	uint16_t bg_checksum;      /* crc16(s_uuid+grouo_num+group_desc)*/
};

/* The ext2 inode.  */
struct ext2_inode {
	uint16_t mode;
	uint16_t uid;
	uint32_t size;
	uint32_t atime;
	uint32_t ctime;
	uint32_t mtime;
	uint32_t dtime;
	uint16_t gid;
	uint16_t nlinks;
	uint32_t blockcnt;	/* Blocks of 512 bytes!! */
	uint32_t flags;
	uint32_t osd1;
	union {
		struct datablocks {
			uint32_t dir_blocks[INDIRECT_BLOCKS];
			uint32_t indir_block;
			uint32_t double_indir_block;
			uint32_t tripple_indir_block;
		} blocks;
		char symlink[60];
	} b;
	uint32_t version;
	uint32_t acl;
	uint32_t dir_acl;
	uint32_t fragment_addr;
	uint32_t osd2[3];
};

/* The header of an ext2 directory entry.  */
struct ext2_dirent {
	uint32_t inode;
	uint16_t direntlen;
	uint8_t namelen;
	uint8_t filetype;
};

struct ext2fs_node {
	struct ext2_data *data;
	struct ext2_inode inode;
	int ino;
	int inode_read;
};

/* Information about a "mounted" ext2 filesystem.  */
struct ext2_data {
	struct ext2_sblock sblock;
	struct ext2_inode *inode;
	struct ext2fs_node diropen;
};


#define EXT4_EXTENTS_FL		0x00080000 /* Inode uses extents */
#define EXT4_EXT_MAGIC			0xf30a
#define EXT4_FEATURE_RO_COMPAT_GDT_CSUM	0x0010
#define EXT4_FEATURE_INCOMPAT_EXTENTS	0x0040
#define EXT4_INDIRECT_BLOCKS		12

#define EXT4_BG_INODE_UNINIT		0x0001
#define EXT4_BG_BLOCK_UNINIT		0x0002
#define EXT4_BG_INODE_ZEROED		0x0004



typedef struct ext2fs_node *ext2fs_node_t;


#define EXT4_EXTENTS_FL		0x00080000 /* Inode uses extents */
#define EXT4_EXT_MAGIC			0xf30a
#define EXT4_FEATURE_RO_COMPAT_GDT_CSUM	0x0010
#define EXT4_FEATURE_INCOMPAT_EXTENTS	0x0040
#define EXT4_INDIRECT_BLOCKS		12

#define EXT4_BG_INODE_UNINIT		0x0001
#define EXT4_BG_BLOCK_UNINIT		0x0002
#define EXT4_BG_INODE_ZEROED		0x0004

/*
 * ext4_inode has i_block array (60 bytes total).
 * The first 12 bytes store ext4_extent_header;
 * the remainder stores an array of ext4_extent.
 */

/*
 * This is the extent on-disk structure.
 * It's used at the bottom of the tree.
 */
struct ext4_extent {
	uint32_t	ee_block;	/* first logical block extent covers */
	uint16_t	ee_len;		/* number of blocks covered by extent */
	uint16_t	ee_start_hi;	/* high 16 bits of physical block */
	uint32_t	ee_start_lo;	/* low 32 bits of physical block */
};

/*
 * This is index on-disk structure.
 * It's used at all the levels except the bottom.
 */
struct ext4_extent_idx {
	uint32_t	ei_block;	/* index covers logical blocks from 'block' */
	uint32_t	ei_leaf_lo;	/* pointer to the physical block of the next *
				 * level. leaf or next index could be there */
	uint16_t	ei_leaf_hi;	/* high 16 bits of physical block */
	uint16_t	ei_unused;
};

/* Each block (leaves and indexes), even inode-stored has header. */
struct ext4_extent_header {
	uint16_t	eh_magic;	/* probably will support different formats */
	uint16_t	eh_entries;	/* number of valid entries */
	uint16_t	eh_max;		/* capacity of store in entries */
	uint16_t	eh_depth;	/* has tree real underlying blocks? */
	uint32_t	eh_generation;	/* generation of the tree */
};

struct ext2_data *ext2fs_root = NULL;
ext2fs_node_t ext2fs_file = NULL;
int symlinknest = 0;
uint32_t *indir1_block = NULL;
int indir1_size = 0;
int indir1_blkno = 0;
uint32_t *indir2_block = NULL;
int indir2_size = 0;
int indir2_blkno = 0;
u8 tps[SECTOR_SIZE];
unsigned char sec_buf[4096];


int ext2fs_devread(int sector, int filesystem_block_log2, int byte_offset, int byte_len, u8 *buf)
{
	unsigned block_len;
	u8 *obuf = buf;

	sector = sector << filesystem_block_log2;

	sector += byte_offset >> SECTOR_BITS;
	byte_offset &= SECTOR_SIZE - 1;

	if (byte_offset) {
		int minimum = SECTOR_SIZE - byte_offset;

		if (byte_len < minimum)
			minimum = byte_len;

		if (bread(sector, 1, sec_buf) < 0) { 
			//puts(" ** ext2_fail8 **\n");
			return 0;
		}
		memcpy(buf, sec_buf + byte_offset, minimum);
		buf += minimum;
		byte_len -= minimum;
		sector++;
	}

	if (!byte_len)
		return 1;

	/*  read sector aligned part */
	block_len = byte_len & ~(SECTOR_SIZE - 1);

	if (block_len == 0) {
		block_len = SECTOR_SIZE;
		if (bread(sector, 1, tps) < 0) {
			//puts("ext2_fail9\n");
		}
		memcpy(buf, tps, byte_len);
		return 1;
	}

	if (bread(sector, block_len / SECTOR_SIZE, buf) < 0) {
		//puts(" ** ext2_fail10\n");
		return 0;
	}
	block_len = byte_len & ~(SECTOR_SIZE - 1);
	buf += block_len;
	byte_len -= block_len;
	sector += block_len / SECTOR_SIZE;

	if (byte_len) {
		/* read rest of data which are not in whole sector */
		if (bread(sector, 1, sec_buf) < 0) {
			//puts(" ** ext2_fail11\n");
			return 0;
		}
		memcpy (buf, sec_buf, byte_len);
	}

	return 1;
}


static int ext2fs_blockgroup
	(struct ext2_data *data, int group, struct ext2_block_group *blkgrp)
{
	return ext2fs_devread((__le32_to_cpu(data->sblock.first_data_block) +
		   1), LOG2_EXT2_BLOCK_SIZE(data),
		 group * sizeof(struct ext2_block_group),
		 sizeof(struct ext2_block_group),(u8 *) blkgrp);
}


static int ext2fs_read_inode
	(struct ext2_data *data, int ino, struct ext2_inode *inode)
{
	struct ext2_block_group blkgrp;
	struct ext2_sblock *sblock = &data->sblock;
	int inodes_per_block;
	int status;

	unsigned int blkno;
	unsigned int blkoff;

	/* It is easier to calculate if the first inode is 0.  */
	ino--;
	status = ext2fs_blockgroup(data,
				    ino /
				    __le32_to_cpu(sblock->inodes_per_group),
				    &blkgrp);
	if (status == 0)
		return 0;

	inodes_per_block = EXT2_BLOCK_SIZE(data) / ext2_inode_size;
	blkno =(ino % __le32_to_cpu(sblock->inodes_per_group)) /
		inodes_per_block;
	blkoff =(ino % __le32_to_cpu(sblock->inodes_per_group)) %
		inodes_per_block;

	/* Read the inode.  */
	status = ext2fs_devread(__le32_to_cpu(blkgrp.inode_table_id) + blkno,
				LOG2_EXT2_BLOCK_SIZE(data),
				ext2_inode_size * blkoff,
				sizeof(struct ext2_inode), (u8 *)inode);

	return !!status;
}


static struct ext4_extent_header *ext4fs_get_extent_block
	(struct ext2_data *data, char *buf,
		struct ext4_extent_header *ext_block,
		uint32_t fileblock, int log2_blksz)
{
	struct ext4_extent_idx *index;
	unsigned long long block;
	int i;

	while (1) {
		index = (struct ext4_extent_idx *)(ext_block + 1);

		if (__le32_to_cpu(ext_block->eh_magic) != EXT4_EXT_MAGIC)
			return 0;

		if (ext_block->eh_depth == 0)
			return ext_block;
		i = -1;
		do {
			i++;
			if (i >= __le32_to_cpu(ext_block->eh_entries))
				break;
		} while (fileblock > __le32_to_cpu(index[i].ei_block));

		if (--i < 0)
			return 0;

		block = __le32_to_cpu(index[i].ei_leaf_hi);
		block = (block << 32) + __le32_to_cpu(index[i].ei_leaf_lo);

		if (ext2fs_devread(block << log2_blksz, 0, 0, EXT2_BLOCK_SIZE(data), buf))
			ext_block = (struct ext4_extent_header *)buf;
		else
			return 0;
	}
}


static int ext2fs_read_block(ext2fs_node_t node, int fileblock) {
	struct ext2_data *data = node->data;
	struct ext2_inode *inode = &node->inode;
	int blknr;
	int blksz = EXT2_BLOCK_SIZE(data);
	int log2_blksz = LOG2_EXT2_BLOCK_SIZE(data);
	int status;

	if (__le32_to_cpu(inode->flags) & EXT4_EXTENTS_FL) {
		char *buf = (void *)malloc(blksz);
		struct ext4_extent_header *ext_block;
		struct ext4_extent *extent;
		int i = -1;
		unsigned long long start;

		memset(buf, 0, blksz);
		ext_block = ext4fs_get_extent_block(ext2fs_root, buf,
						    (struct ext4_extent_header
						     *)inode->b.
						    blocks.dir_blocks,
						    fileblock, log2_blksz);
		if (!ext_block) {
			puts("error getting ext block 0x");
			print32(fileblock);
			puts("\n");
			return -1;
		}

		extent = (struct ext4_extent *)(ext_block + 1);

		do {
			i++;
			if (i >= __le32_to_cpu(ext_block->eh_entries))
				break;
		} while (fileblock >= __le32_to_cpu(extent[i].ee_block));
		if (--i >= 0) {
			fileblock -= __le32_to_cpu(extent[i].ee_block);
			if (fileblock >= __le32_to_cpu(extent[i].ee_len))
				return 0;

			start = __le32_to_cpu(extent[i].ee_start_hi);
			start = (start << 32) +
					__le32_to_cpu(extent[i].ee_start_lo);
			return fileblock + start;
		}

		puts("Extent Error\n");
		return -1;
	}

	/* Direct blocks.  */
	if (fileblock < INDIRECT_BLOCKS)
		return __le32_to_cpu(inode->b.blocks.dir_blocks[fileblock]);

	/* Indirect.  */
	if (fileblock <(INDIRECT_BLOCKS +(blksz / 4))) {
		if (indir1_block == NULL) {
			indir1_block =(uint32_t *) malloc(blksz);
			indir1_size = blksz;
			indir1_blkno = -1;
		}
		if (blksz != indir1_size) {
			indir1_block = NULL;
			indir1_size = 0;
			indir1_blkno = -1;
			indir1_block =(uint32_t *) malloc(blksz);
			indir1_size = blksz;
		}
		if ((__le32_to_cpu(inode->b.blocks.indir_block) <<
		     log2_blksz) != indir1_blkno) {
			status = ext2fs_devread(
				__le32_to_cpu(inode->b.blocks.indir_block),
				log2_blksz, 0, blksz, (u8 *) indir1_block);
			if (status == 0) {
				puts("rd blk fail\n");
				return 0;
			}
			indir1_blkno =
				__le32_to_cpu(inode->b.blocks.
					       indir_block) << log2_blksz;
		}
		blknr = __le32_to_cpu(indir1_block
				       [fileblock - INDIRECT_BLOCKS]);

		return blknr;
	}

	/* Double indirect.  */
	if (fileblock < (INDIRECT_BLOCKS +(blksz / 4 *(blksz / 4 + 1)))) {
		unsigned int perblock = blksz / 4;
		unsigned int rblock = fileblock -(INDIRECT_BLOCKS
						   + blksz / 4);

		if (indir1_block == NULL) {
			indir1_block =(uint32_t *) malloc(blksz);
			indir1_size = blksz;
			indir1_blkno = -1;
		}
		if (blksz != indir1_size) {
			indir1_block = NULL;
			indir1_size = 0;
			indir1_blkno = -1;
			indir1_block =(uint32_t *) malloc(blksz);
			indir1_size = blksz;
		}
		if ((__le32_to_cpu(inode->b.blocks.double_indir_block) <<
		     log2_blksz) != indir1_blkno) {
			status = ext2fs_devread(__le32_to_cpu(inode->b.blocks.double_indir_block), log2_blksz,
						0, blksz,
						(u8 *) indir1_block);
			if (status == 0) {
				//puts("ext2_fail30\n");
				return -1;
			}
			indir1_blkno =
				__le32_to_cpu(inode->b.blocks.double_indir_block) << log2_blksz;
		}

		if (indir2_block == NULL) {
			indir2_block =(uint32_t *) malloc(blksz);
			if (indir2_block == NULL)
				return -1;

			indir2_size = blksz;
			indir2_blkno = -1;
		}
		if (blksz != indir2_size) {
			indir2_block = NULL;
			indir2_size = 0;
			indir2_blkno = -1;
			indir2_block =(uint32_t *) malloc(blksz);
			if (indir2_block == NULL)
				return -1;

			indir2_size = blksz;
		}
		if ((__le32_to_cpu(indir1_block[rblock / perblock]) <<
		     log2_blksz) != indir2_blkno) {
			status = ext2fs_devread(__le32_to_cpu(indir1_block[rblock / perblock]), log2_blksz,
						 0, blksz,
						(u8 *) indir2_block);
			if (status == 0) {
				//puts("ext2_fail31\n");
				return -1;
			}
			indir2_blkno =
				__le32_to_cpu(indir1_block[rblock / perblock]) << log2_blksz;
		}
		blknr = __le32_to_cpu(indir2_block[rblock % perblock]);

		return blknr;
	}

	/* triple indirect not supported */	
	//puts("ext2_fail32\n");

	return -1;
}


int ext2fs_read_file(ext2fs_node_t node, int pos, unsigned int len, char *buf)
{
	int i;
	int blockcnt;
	int log2blocksize = LOG2_EXT2_BLOCK_SIZE(node->data);
	int blocksize = 1 <<(log2blocksize + DISK_SECTOR_BITS);
	unsigned int filesize = __le32_to_cpu(node->inode.size);
	int previous_block_number = -1;
	int delayed_start = 0;
	int delayed_extent = 0;
	int delayed_skipfirst = 0;
	int delayed_next = 0;
	char * delayed_buf = NULL;
	int status;

	/* Adjust len so it we can't read past the end of the file.  */
	if (len > filesize) {
		len = filesize;
	}
	blockcnt = ((len + pos) + blocksize - 1) / blocksize;

	for(i = pos / blocksize; i < blockcnt; i++) {
		int blknr;
		int blockoff = pos % blocksize;
		int blockend = blocksize;

		int skipfirst = 0;

		blknr = ext2fs_read_block(node, i);
		if (blknr < 0)
			return -1;

		blknr = blknr << log2blocksize;

		/* Last block.  */
		if (i == blockcnt - 1) {
			blockend =(len + pos) % blocksize;

			/* The last portion is exactly blocksize.  */
			if (!blockend) {
				blockend = blocksize;
			}
		}

		/* First block.  */
		if (i == pos / blocksize) {
			skipfirst = blockoff;
			blockend -= skipfirst;
		}

		/* If the block number is 0 this block is not stored on disk but
		   is zero filled instead.  */
		if (blknr) {
			int status;

			if (previous_block_number != -1) {
				if (delayed_next == blknr) {
					delayed_extent += blockend;
					delayed_next += blockend >> SECTOR_BITS;
				} else { /* spill */
					status = ext2fs_devread(delayed_start,
						0, delayed_skipfirst,
						delayed_extent,
						(u8 *)delayed_buf);
					if (status == 0)
						return -1;
					previous_block_number = blknr;
					delayed_start = blknr;
					delayed_extent = blockend;
					delayed_skipfirst = skipfirst;
					delayed_buf = buf;
					delayed_next = blknr +
						     (blockend >> SECTOR_BITS);
				}
			} else {
				previous_block_number = blknr;
				delayed_start = blknr;
				delayed_extent = blockend;
				delayed_skipfirst = skipfirst;
				delayed_buf = buf;
				delayed_next = blknr + (blockend >> SECTOR_BITS);
			}

		} else {
			if (previous_block_number != -1) {
				/* spill */
				status = ext2fs_devread(delayed_start,
						0, delayed_skipfirst,
						delayed_extent,
						(u8 *)delayed_buf);
				if (status == 0)
					return -1;
				previous_block_number = -1;
			}
			memset(buf, 0, blocksize - skipfirst);
		}
		buf += blocksize - skipfirst;
	}

	if (previous_block_number != -1) {
		/* spill */
		status = ext2fs_devread(delayed_start,
				0, delayed_skipfirst,
				delayed_extent, (u8 *)delayed_buf);
		if (status == 0)
			return -1;
		previous_block_number = -1;
	}

	return(len);
}


static int ext2fs_iterate_dir(ext2fs_node_t dir, char *name,
					ext2fs_node_t * fnode, int *ftype)
{
	unsigned int fpos = 0;
	int status;
	struct ext2fs_node *diro = dir;

	if (!diro->inode_read) {
		status = ext2fs_read_inode(diro->data, diro->ino,
					    &diro->inode);
		if (status == 0) {
			//printdec(diro->ino);
			//puts("ext2_f12\n");
			return(0);
		}
	}

	/* Search the file.  */
	while (fpos < __le32_to_cpu(diro->inode.size)) {
		struct ext2_dirent dirent;

		status = ext2fs_read_file(diro, fpos,
					   sizeof(struct ext2_dirent),
					  (char *) &dirent);
		if (status < 1) {
			//puts("ext2_f13\n");
			return 0;
		}

		if (dirent.namelen) {
			char filename[256];
			ext2fs_node_t fdiro;
			int type = FILETYPE_UNKNOWN;

			status = ext2fs_read_file(diro,
						   fpos + sizeof(struct ext2_dirent),
						   dirent.namelen, filename);
			if (status < 1) {
				//puts("ext2_f14\n");
				return(0);
			}

			fdiro = (void *)malloc(sizeof(struct ext2fs_node));
			fdiro->data = diro->data;
			fdiro->ino = __le32_to_cpu(dirent.inode);

			filename[dirent.namelen] = '\0';

			if (dirent.filetype != FILETYPE_UNKNOWN) {
				fdiro->inode_read = 0;

				if (dirent.filetype == FILETYPE_DIRECTORY) {
					type = FILETYPE_DIRECTORY;
				} else if (dirent.filetype ==
					   FILETYPE_SYMLINK) {
					type = FILETYPE_SYMLINK;
				} else if (dirent.filetype == FILETYPE_REG) {
					type = FILETYPE_REG;
				}
			} else {
				/* The filetype can not be read from the dirent, get it from inode */

				status = ext2fs_read_inode(diro->data,
							    __le32_to_cpu(dirent.inode),
							    &fdiro->inode);
				if (status == 0) {
					//puts("ext2_fail3\n");
					return 0;
				}
				fdiro->inode_read = 1;

				if ((__le16_to_cpu(fdiro->inode.mode) &
				     FILETYPE_INO_MASK) ==
				    FILETYPE_INO_DIRECTORY) {
					type = FILETYPE_DIRECTORY;
				} else if ((__le16_to_cpu(fdiro->inode.mode)
					    & FILETYPE_INO_MASK) ==
					   FILETYPE_INO_SYMLINK) {
					type = FILETYPE_SYMLINK;
				} else if ((__le16_to_cpu(fdiro->inode.mode)
					    & FILETYPE_INO_MASK) ==
					   FILETYPE_INO_REG) {
					type = FILETYPE_REG;
				}
			}
			if ((name != NULL) &&(fnode != NULL)
			    &&(ftype != NULL)) {
				if (strcmp(filename, name) == 0) {
					*ftype = type;
					*fnode = fdiro;
					return 1;
				}
			} else {
				if (fdiro->inode_read == 0) {
					status = ext2fs_read_inode(diro->data,
							    __le32_to_cpu(dirent.inode),
							    &fdiro->inode);
					if (status == 0) {
						//puts("ext2fs_fail4\n");
						return 0;
					}
					fdiro->inode_read = 1;
				}
			}
		}
		fpos += __le16_to_cpu(dirent.direntlen);
	}

	return 0;
}


static char *ext2fs_read_symlink(ext2fs_node_t node) {
	char *symlink;
	struct ext2fs_node *diro = node;

	if (!diro->inode_read) {
		if (!ext2fs_read_inode(diro->data, diro->ino,
					    &diro->inode))
			return 0;
	}
	symlink = (char *)malloc(__le32_to_cpu(diro->inode.size) + 1);
	if (!symlink)
		return(0);

	/* If the filesize of the symlink is bigger than
	   60 the symlink is stored in a separate block,
	   otherwise it is stored in the inode.  */
	if (__le32_to_cpu(diro->inode.size) < 60)
		strncpy(symlink, diro->inode.b.symlink,
					 __le32_to_cpu(diro->inode.size));
	else
		if (ext2fs_read_file(diro, 0, __le32_to_cpu(diro->inode.size),
								 symlink) <= 0)
			return 0;

	symlink[__le32_to_cpu(diro->inode.size)] = '\0';

	return symlink;
}


int ext2fs_find_file1
	(const char *currpath,
	 ext2fs_node_t currroot, ext2fs_node_t * currfound, int *foundtype)
{
	char fpath[strlen(currpath) + 1];
	char *name = fpath;
	char *next;
	int status;
	int type = FILETYPE_DIRECTORY;
	ext2fs_node_t currnode = currroot;
	ext2fs_node_t oldnode = currroot;

	strncpy(fpath, currpath, strlen(currpath) + 1);

	/* Remove all leading slashes.  */
	while (*name == '/')
		name++;

	if (!*name) {
		*currfound = currnode;
		return 1;
	}

	for(;;) {
		int found;

		/* Extract the actual part from the pathname.  */
		next = strchr(name, '/');
		if (next) {
			/* Remove all leading slashes.  */
			while (*next == '/') {
				*(next++) = '\0';
			}
		}

		/* At this point it is expected that the current node is a directory, check if this is true.  */
		if (type != FILETYPE_DIRECTORY)
			return 0;

		oldnode = currnode;

		/* Iterate over the directory.  */
		found = ext2fs_iterate_dir(currnode, name, &currnode, &type);
		if (found <= 0)
			//puts("ext2_fail20\n");
			return 0;

		if (found == -1)
			break;

		/* Read in the symlink and follow it.  */
		if (type == FILETYPE_SYMLINK) {
			char *symlink;

			/* Test if the symlink does not loop.  */
			if (++symlinknest == 8)
				return 0;

			symlink = ext2fs_read_symlink(currnode);

			if (!symlink)
				return 0;
			/* The symlink is an absolute path, go back to the root inode.  */
			if (symlink[0] == '/')
				oldnode = &ext2fs_root->diropen;

			/* Lookup the node the symlink points to.  */
			if (!ext2fs_find_file1(symlink, oldnode,
						    &currnode, &type))
				return 0;
		}

		/* Found the node!  */
		if (!next || *next == '\0') {
			*currfound = currnode;
			*foundtype = type;
			return 1;
		}
		name = next;
	}
	return -1;
}


int ext2fs_find_file(const char *path,
	 ext2fs_node_t rootnode, ext2fs_node_t * foundnode, int expecttype)
{
	int foundtype = FILETYPE_DIRECTORY;

	symlinknest = 0;
	if (!path)
		return 0;

	if (ext2fs_find_file1(path, rootnode, foundnode, &foundtype) <= 0)
		return 0;

	/* Check if the node that was found was of the expected type.  */
	if (expecttype == FILETYPE_REG && foundtype != expecttype)
		return 0;

	if (expecttype == FILETYPE_DIRECTORY && foundtype != expecttype)
		return 0;

	return 1;
}

int ext2fs_close(void)
{
	ext2fs_file = NULL;
	indir1_block = NULL;
	indir1_size = 0;
	indir1_blkno = -1;
	indir2_block = NULL;
	indir2_size = 0;
	indir2_blkno = -1;
	ext2fs_file = NULL;
	symlinknest = 0;

	return 0;
}

int ext2fs_open(const char *filename)
{
	ext2fs_node_t fdiro = NULL;

	ext2fs_close();

	if (!ext2fs_root)
		return -1;

	if (!ext2fs_find_file(filename, &ext2fs_root->diropen, &fdiro,
							   FILETYPE_REG)) {
		//puts(filename);
		//puts(": open no find file\n");
		return -2;
	}

	if (!fdiro->inode_read)
		if (!ext2fs_read_inode(fdiro->data, fdiro->ino,
					    &fdiro->inode)) {
			puts("reading inode fail\n");
			return -3;
		}
	ext2fs_file = fdiro;

	return __le32_to_cpu(fdiro->inode.size);
}

int ext2fs_read(char *buf, unsigned len) {
	int status;

	if (ext2fs_root == NULL)
		return 0;

	if (ext2fs_file == NULL)
		return 0;

	status = ext2fs_read_file(ext2fs_file, 0, len, buf);
	if (status < 0)
		puts("ext2fs_read <0\n");
	return status;
}

int
do_ext2_read(const char *filename, void *buffer, unsigned long maxsize)
{
	int n;
	
	if ((void *)filename < (void *)0x00100000)
		filename = (const char *)pic_dereference((void *)filename);

	n = ext2fs_open(filename);

	if (n < 0)
		return 0;

	if (n > maxsize)
		n = maxsize;

	n = ext2fs_read(buffer, n);

	ext2fs_close();

	return n;
}


int ext2fs_mount(void) {
	struct ext2_data *data;
	int status;

	/* initialize blkno here */
	indir1_blkno = -1;
	indir2_blkno = -1;

	data = (void *)malloc(sizeof(struct ext2_data));

	/* Read the superblock.  */
	status = ext2fs_devread(1 * 2, 0, 0, sizeof(struct ext2_sblock),
				(u8 *) &data->sblock);
	if (!status)
		goto fail;

	/* Make sure this is an ext2 filesystem.  */
	if (__le16_to_cpu(data->sblock.magic) != EXT2_MAGIC)
		goto fail;

	if (__le32_to_cpu(data->sblock.revision_level) == EXT2_GOOD_OLD_REV)
		ext2_inode_size = EXT2_GOOD_OLD_INODE_SIZE;
	else
		ext2_inode_size = __le16_to_cpu (data->sblock.inode_size);

	data->diropen.data = data;
	data->diropen.ino = 2;
	data->diropen.inode_read = 1;
	data->inode = &data->diropen.inode;

	if (!ext2fs_read_inode(data, 2, data->inode))
		goto fail;

	ext2fs_root = data;
	filesystem_read = pic_dereference(do_ext2_read);
	
	return 0;

fail:
	ext2fs_close();

	return 1;
}


