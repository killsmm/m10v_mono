/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Aneesh V <aneesh@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
#ifndef _UTILS_H_
#define _UTILS_H_

#define __packed __attribute__((packed))

//typedef unsigned int __u32;
//typedef		__u32		uint32_t;

//#define u32 unsigned long
//#define s32 long

#include "../types.h"
#include "../io.h"

#define printf(fmt,args...)

#define uswap_32(x) \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | \
	 (((x) & 0x000000ff) << 24))

#define __be32_to_cpu(x) uswap_32((x))

#define min(X, Y)				\
	({ typeof (X) __x = (X);		\
		typeof (Y) __y = (Y);		\
		(__x < __y) ? __x : __y; })

#define max(X, Y)				\
	({ typeof (X) __x = (X);		\
		typeof (Y) __y = (Y);		\
		(__x > __y) ? __x : __y; })

#define MIN(x, y)  min(x, y)
//#define MAX(x, y)  max(x, y)
#define MAX(a,b)	((a)>(b) ? (a) : (b))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

struct dump {
	u32 ads;
	u32 value;
};

extern void replay_dump(const struct dump *dump, int count, u32 offset);

static inline s32 log_2_n_round_up(u32 n)
{
	s32 log2n = -1;
	u32 temp = n;

	while (temp) {
		log2n++;
		temp >>= 1;
	}

	if (n & (n - 1))
		return log2n + 1; /* not power of 2 - round up */
	else
		return log2n; /* power of 2 */
}

static inline s32 log_2_n_round_down(u32 n)
{
	s32 log2n = -1;
	u32 temp = n;

	while (temp) {
		log2n++;
		temp >>= 1;
	}

	return log2n;
}

/* -1 or pending char */
int getc(void);
void putc(const char c);
void puts(const char *s);
void *memcpy(void *dest, const void *src, u32 count);

void *malloc(size_t size);

void udelay(int n);
int raise(int signum);
int timer_init(void);
void __udelay(unsigned long usec);
void reset_timer_masked(void);

void *pic_dereference(const void *relative_ptr);

//void printnybble(unsigned char n);
void print8(unsigned char n);
void print32(unsigned int u);
void printdec(int n);
void hexdump(void *start, int len);

void uaw_be32(void *dest, u32 n);
u32 __swap32(unsigned char *x);

#define __be_to_cpu32(x) __swap32(x)
#define __cpu32_to_be(x) __swap32(x)

#define read_mpidr							\
	({								\
		unsigned int __val;					\
		asm("mrc	p15, 0, %0, c0, c0, 5"			\
		    : "=r" (__val)					\
		    :							\
		    : "cc");						\
		__val;							\
	})

#endif
