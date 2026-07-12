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
#include "io.h"

#define MALLOC_POOL_EXTENT (100 * 1024 * 1024)

void __div0(void)
{
	puts("div by 0\n");
}

void udelay(int us)
{
	volatile int q = 1;
	volatile unsigned int us1;

	us1 = us * US_SPIN_FACTOR;

	while (--us1)
		q += us1 * q;
}

void printnybble(unsigned char n, char *leading)
{
	if (!n && !*leading)
		return;

	*leading = 1;
	if (n < 10)
		putc('0' + n);
	else
		putc('a' + n - 10);
}

void _print8(unsigned char n, char *leading)
{
	printnybble((n >> 4) & 15, leading);
	printnybble(n & 15, leading);
}

void print8(unsigned char n)
{
	char leading = 1;
	_print8(n, &leading);
}

void _print32(unsigned int u, char *leading)
{
	_print8(u >> 24, leading);
	_print8(u >> 16, leading);
	_print8(u >> 8, leading);
	printnybble((u >> 4) & 15, leading);
	*leading = 1;
	printnybble(u & 15, leading);
}

void print32(unsigned int u)
{
	char leading = 1;
	_print32(u, &leading);
}

void print32_noleading(unsigned int u)
{
	char leading = 0;
	_print32(u, &leading);
}

void printdec(int n)
{
	static int d[] = {
		1 * 1000 * 1000 * 1000,
		     100 * 1000 * 1000,
		      10 * 1000 * 1000,
		       1 * 1000 * 1000,
			    100 * 1000,
			     10 * 1000,
			      1 * 1000,
				   100,
				    10,
				     1,
				     0
	};
	int flag = 0;
	int div = 0;

	if (n < 0) {
		putc('-');
		n = -n;
	}

	while (d[div]) {
		int r = 0;
		while (n >= d[div]) {
			r++;
			n -= d[div];
		}
		if (r || flag || (d[div] == 1)) {
			putc('0' + r);
			flag = 1;
		}
		div++;
	}
}

#if 0
void hexdump(void *start, int len)
{
	int n;
	u8 *p = start;

	while (len > 0) {
		print32((int)start);
		putc(':');
		putc(' ');
		for (n = 0; n < 16; n++) {
			print8(*p++);
			putc(' ');
		}
		putc('\n');
		len -= 16;
		start += 16;
	}
}
#endif

int raise(int signum)
{
       return 0;
}

void __aeabi_unwind_cpp_pr0(void)
{
}

void replay_dump(const struct dump *dump, int count, u32 offset)
{
	while (count--) {
		__raw_writel(dump->value, dump->ads + offset);
		dump++;
	}
}

unsigned int start_ads;

void *pic_dereference(const void *relative_ptr)
{
	unsigned int start_ads;

		asm("mov    %[result], pc"

		   : [result]"=r" (start_ads)
		   :
		   : /* No clobbers */
		);

	/* don't fixup something that is already too big to be an offset */
	if ((u32)relative_ptr >
			((start_ads - (u32)pic_dereference) & 0xfffffff0))
		return (void *)relative_ptr;

	return (void *)((((start_ads - (u32)pic_dereference) & 0xfffffff0) +
			(unsigned int)(relative_ptr)));
}

extern u32 malloc_pool;
int malloc_pointer;

void *malloc(size_t size)
{
	unsigned int ptr = malloc_pointer;

#if 0
	print32((unsigned long)((void *)&malloc_pool + ptr));
	puts(" ");
	printdec(size);
	puts("\n");
#endif
	malloc_pointer += (size + 3) & ~3;

	return (void *)&malloc_pool + ptr;
}

void uaw_be32(void *dest, u32 n)
{
	u8 *p = (u8 *)dest;

	*p++ = n >> 24;
	*p++ = n >> 16;
	*p++ = n >> 8;
	*p++ = n;
}

u32 __swap32(unsigned char *x)
{
	return (x[3]) | (x[2] << 8) | (x[1] << 16) | (x[0] << 24);
}

 void *memcpy(void *dest, const void *src, u32 n)
 {
	 u8 *d = dest;
	 const u8 *s = src;

	while (n--)
		*d++ = *s++;
	
	return dest;
 }
