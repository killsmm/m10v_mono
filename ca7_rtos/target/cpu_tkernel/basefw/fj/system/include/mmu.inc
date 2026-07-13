/**
 * @file		mmu.inc
 * @brief		MMU (Memory Management Unit)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

	#ifndef	_MMU_INC
	#define	_MMU_INC

// Definitions for section descriptors
	.equ	S_NGLOBAL,					(1 << 17)
	.equ	S_SHARED,					(1 << 16)
	.equ	S_APX,						(1 << 15)
	.equ	S_TEX2,						(1 << 14)
	.equ	S_TEX1,						(1 << 13)
	.equ	S_TEX0,						(1 << 12)
	.equ	S_AP1,						(1 << 11)
	.equ	S_AP0,						(1 << 10)
	.equ	S_PARITY,					(1 << 9)
	.equ	S_DOMAIN8,					(1 << 8)
	.equ	S_DOMAIN4,					(1 << 7)
	.equ	S_DOMAIN2,					(1 << 6)
	.equ	S_DOMAIN1,					(1 << 5)
	.equ	S_XN,						(1 << 4)
	.equ	S_C,						(1 << 3)
	.equ	S_B,						(1 << 2)

	.equ	IS_A_SECTION,				(1 << 1)

	.equ	S_NORMAL,					(S_TEX2 | IS_A_SECTION | S_DOMAIN1)
	.equ	S_SHARED_NORMAL,			(S_NORMAL | S_SHARED)
	.equ	S_INNER_NC,					(0)
	.equ	S_INNER_WB_WA,				(S_B)
	.equ	S_INNER_WT_NWA,				(S_C)
	.equ	S_INNER_WB_NWA,				(S_C | S_B)
	.equ	S_OUTER_NC,					(0)
	.equ	S_OUTER_WB_WA,				(S_TEX0)
	.equ	S_OUTER_WT_NWA,				(S_TEX1)
	.equ	S_OUTER_WB_NWA,				(S_TEX1 | S_TEX0)
	.equ	S_DEVICE,					(S_TEX1 | IS_A_SECTION)
	.equ	S_SHARED_DEVICE,			(S_B | S_SHARED | IS_A_SECTION)
	.equ	S_STRONGLY_ORDERED,			(IS_A_SECTION)
	.equ	S_SHARED_STRONGLY_ORDERED,	(S_SHARED | IS_A_SECTION)
	.equ	S_NO_ACCESS,				(IS_A_SECTION)

	.equ	S_PERMISSION_FAULTS,		(0)
	.equ	S_PRIVILEGED_READ_ONLY,		(S_APX | S_AP0)
	.equ	S_FULL_ACCESS,				(S_AP0 | S_AP1)


// Definitions for page descriptors
	.equ	P_NGLOBAL,					(1 << 11)
	.equ	P_SHARED,					(1 << 10)
	.equ	P_APX,						(1 << 9)
	.equ	P_TEX1,						(1 << 6)
	.equ	P_AP1,						(1 << 5)
	.equ	P_AP0,						(1 << 4)
	.equ	P_CACHE,					(1 << 3)
	.equ	P_BUFFER,					(1 << 2)
	.equ	IS_A_PAGE,					(1 << 1)
	.equ	P_XN,						(1 << 0)

	.equ	IS_A_PAGETABLE,				(1 << 0)

	.equ	P_NORMAL,					(P_TEX1 | P_CACHE | P_BUFFER | IS_A_PAGE | P_AP0 | P_AP1)
	.equ	P_SHARED_NORMAL,			(P_NORMAL | P_SHARED)
	.equ	P_SHARED_STRONGLY_ORDERED,	(P_SHARED | P_AP0 | P_AP1 | IS_A_PAGE)

	#endif
