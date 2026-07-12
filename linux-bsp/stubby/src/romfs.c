/*
 * (C) Copyright 2013-15 Andy Green <andy.green@linaro.org>
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
 *
 * all of these members are big-endian
 */

#include <asm/utils.h>
#include "block.h"

struct sb {
	unsigned int be_magic1;
	unsigned int be_magic2;
	unsigned int be_size;
	unsigned int be_checksum;
	char name[0];
};

struct inode {
	unsigned int be_next;
	unsigned int be_spec; /* DIR: offset from romfs start to first entry */
	unsigned int be_size;
	unsigned int be_checksum;
	char name[0];
};

/* these magics are already in big-endian and don't need converting */
#define BE_ROMFS_MAGIC1 0x6d6f722d
#define BE_ROMFS_MAGIC2 0x2d736631

const void *romfs;

static unsigned int le(const unsigned int *be)
{
	const unsigned char *c = (const unsigned char *)be;
	return (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
}

/* rule is pad strings to 16 byte boundary */
static int pad(const char *s)
{
	int n = 0;

	while (*s++)
		n++;

	return (n + 0xf) & ~0xf;
}

static const struct inode * lookup(const void *start, const char *filepath)
{
	const struct sb *sb = (struct sb *)romfs;
	const struct inode *inode = (struct inode *)
				(romfs + sizeof(struct sb) + pad(sb->name));
	const struct inode *level = inode;
	const char *target;
	const char *p, *n;
	int m;

	if (start != romfs) {
		inode = start;
		level = start;
	}

	while (inode != romfs) { /* this indicates reached end of files */
		p = filepath;
		n = inode->name;

		while (*p && *p != '/' && *n && *p == *n) {
			p++;
			n++;
		}

		/* we found the whole path? */

		if (!*p && !*n) {
			m = le(&inode->be_next) & 7;
			switch (m) {
			case 0: /* hard link */
				return (struct inode *)
					(romfs + (le(&inode->be_spec) & ~0xf));
			case 3: /* symlink */
				target = ((void *)(inode + 1)) +
							       pad(inode->name);
				if (*target == '/') {
					/* reinterpret symlink path from / */
					level = (struct inode *)
						(romfs + sizeof(struct sb) +
								 pad(sb->name));
					target++;
				} /* else reinterpret from cwd */
				inode = lookup(level, target);
				continue;
			default: /* file of some kind, or dir */
				return inode;
			}
		}

		/* we matched a dir part */
		if (*p == '/' && !*n) {

			m = le(&inode->be_next) & 7;
			switch (m) {
			case 0: /* hard link */
				return (struct inode *)
					(romfs + (le(&inode->be_spec) & ~0xf));
			case 3: /* symlink */
				target = ((void *)(inode + 1)) +
							       pad(inode->name);
				if (*target == '/') {
					/* reinterpret symlink path from / */
					level = (struct inode *)
						(romfs + sizeof(struct sb) +
								 pad(sb->name));
					target++;
				} /* else reinterpret from cwd */
				inode = lookup(level, target);
				if (!inode)
					return 0;
				/*
				 * resume looking one level deeper
				 * directories have special "first entry"
				 * offset that accounts for padding
 				 */
				inode = romfs + le(&inode->be_spec);

				while (*filepath != '/' && *filepath)
					filepath++;
				if (!*filepath)
					return 0;
				filepath++;
				continue;

			default: /* file of some kind, or dir */
				/* move past the / */
				filepath = p + 1;
				if (m == 1) /* dir */
					/* use special ofs to skip padding */
					inode = romfs + le(&inode->be_spec);
				else
					/* resume looking one level deeper */
					inode = (struct inode *)
						(((void *)(inode + 1)) +
						pad(inode->name));
				break;
			}
			level = inode;
			continue;
		}

		/* not a match, try the next at this level */

		if (!(le(&inode->be_next) & ~0xf))
			/* no more at this level */
			return 0;

		inode = (struct inode *)(romfs + (le(&inode->be_next) & ~0xf));
	}

	return 0;
}

int
romfs_filesystem_read(const char *filepath, void *buf, unsigned long maxsize)
{
	const struct inode *inode = lookup(romfs, filepath);
	unsigned int len;

	if (!inode)
		return 0;

	len = le(&inode->be_size);
	if (len > maxsize)
		len = maxsize;
	memcpy(buf, (const char *)inode->name + pad(inode->name), len);

	return len;
}

void *
romfs_filesystem_get_ads(const char *filepath, int *len)
{
	const struct inode *inode = lookup(romfs, filepath);

	if (!inode)
		return NULL;

	*len = le(&inode->be_size);

	return (void *)inode->name + pad(inode->name);
}


int romfs_mount(const void *_romfs)
{
	const struct sb *sb = (struct sb *)_romfs;

	romfs = _romfs;
	filesystem_read = pic_dereference(romfs_filesystem_read);

	return (sb->be_magic1 != BE_ROMFS_MAGIC1 ||
			sb->be_magic2 != BE_ROMFS_MAGIC2);
}

