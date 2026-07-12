#ifndef __ASM_ARM_TYPES_H
#define __ASM_ARM_TYPES_H

typedef unsigned int	__kernel_size_t;
#ifndef _SIZE_T
#define _SIZE_T
typedef __kernel_size_t		size_t;
#endif

typedef int			__kernel_ptrdiff_t;
typedef __kernel_ptrdiff_t	ptrdiff_t;

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char uchar;

typedef unsigned short umode_t;

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define __le32_to_cpu(__X) __X
#define __le16_to_cpu(__X) __X

#if defined(__GNUC__)
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#endif

/*
 * These aren't exported outside the kernel to avoid name space clashes
 */

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

//typedef signed int s32;
//typedef unsigned int u32;
#define u32 unsigned long
#define s32 long


typedef signed long long s64;
typedef unsigned long long u64;

typedef s32 int32_t;
typedef s64 int64_t;
typedef u32 uint32_t;
typedef u64 uint64_t;

#define BITS_PER_LONG 32

/* Dma addresses are 32-bits wide.  */

typedef u32 dma_addr_t;

typedef unsigned long phys_addr_t;
typedef unsigned long phys_size_t;

#define NULL 0

#endif
