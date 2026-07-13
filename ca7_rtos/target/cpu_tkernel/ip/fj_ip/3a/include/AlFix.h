/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef	AL_FIX_H
#define	AL_FIX_H

/*===========================================================================**
	Include
**============================================================================*/
#include "AlTypStd.h"
/*===========================================================================**
	Macro
**============================================================================*/
// constant definition for SQ_32 format
#define	AL_FIX_SQ32_MIN		(0x80000000)	/**< SQ32 type minimum value  (-32768.0)*/
#define	AL_FIX_SQ32_M1		(0xffff0000)	/**< SQ32 type -1.0 value*/
#define	AL_FIX_SQ32_0		(0x00000000)	/**< SQ32 type  0.0 value*/
#define	AL_FIX_SQ32_1		(0x00010000)	/**< SQ32 type  1.0 value*/
#define	AL_FIX_SQ32_2		(0x00020000)	/**< SQ32 type  2.0 value*/
#define	AL_FIX_SQ32_MAX		(0x7fffffff)	/**< SQ32 type maximum (32767.99998)*/

#define	AL_FIX_SQ32_PI		(0x0003243F)	/**< SQ32 type	pie number π (3.141592654...)*/

/*===========================================================================**
	Typedef
**============================================================================*/

/*===========================================================================**
	Prototype
**============================================================================*/

/*===========================================================================*/
//#define AL_FIX_VAL(n)	((SI_32)(((n)>0) ? ((n)*65536+0.5) : ((n)*65536-0.5)))
#define AL_FIX_VAL(n)	((SI_32)((double)(n)*65536.0))
/*------------------------------------------------------------------------*//**
convert to Fixpoint value from Integer

	
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
// When there is ";" before comment, Doxygen does not recognize it normally.

/*---------------------------------------------------------------------------**
Description	:	y = a * b
Return		:	y[s15.16]
Note		:	no overflow check, underflow check.
			:	no saturation.
**---------------------------------------------------------------------------*/
/*===========================================================================*/
extern	SQ_32	AlFixMul(
				SQ_32		auqArgA	,		/**<[I ] a[s15.16]*/
				SQ_32		auqArgB	)		/**<[I ] b[s15.16]*/
/*------------------------------------------------------------------------*//**
Signed fixed point multiplication

	y[s15.16] = a[s15.16] * b[s15.16]
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.


/*===========================================================================*/
extern	UQ_32	AluFixMul(
				UQ_32		auqArgA	,		/**<[I ] a[u16.16]*/
				UQ_32		auqArgB	)		/**<[I ] b[u16.16]*/
/*------------------------------------------------------------------------*//**
Unsigned fixed point multiplication

	y[u16.16] = a[u16.16] * b[u16.16]
@return         y[u16.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.


/*===========================================================================*/
extern	SQ_32	AlFixDiv(
				SQ_32		asqDividend	,	/**<[I ] a[s15.16]*/
				SQ_32		asqDivisor	)	/**<[I ] b[s15.16]*/
/*------------------------------------------------------------------------*//**
Signed fixed point division

	y[s15.16] = a[s15.16] / b[s15.16]
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.


/*===========================================================================*/
extern	UQ_32	AluFixDiv(
				UQ_32		auqDividend	,	/**<[I ] a[u16.16]*/
				UQ_32		auqDivisor	)	/**<[I ] b[u16.16]*/
/*------------------------------------------------------------------------*//**
Unsigned fixed point division

	y[u16.16] = a[u16.16] / b[u16.16]
@return         y[u16.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	SQ_32	AlFixPow2(
				SQ_32		asqX	)		/**<[I ] x[u16.16]*/
/*------------------------------------------------------------------------*//**
signed fixed point power

	y[s15.16] = 2^(x[s15.16])
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.


/*===========================================================================*/
extern	SQ_32	AlFixLog2(
				SQ_32		asqX	)		/**<[I ] x[u16.16]*/
/*------------------------------------------------------------------------*//**
signed fixed point logarithmic ( base is 2)

	y[s15.16] = LOG2(x[s15.16])
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	SQ_32	AlFixLog10(
				SQ_32		asqX	)		/**<[I ] x[u16.16]*/
/*------------------------------------------------------------------------*//**
signed fixed point logarithmic ( base is 10)

	y[s15.16] = LOG2(x[s15.16])
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	SQ_32	AlFixSqrt(
				SQ_32		asqX	)		/**<[I ] x[u16.16]*/
/*------------------------------------------------------------------------*//**
signed fixed point square root

	y[s15.16] = square_root(x[s15.16])
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	SQ_32	AlFixMac2(
				SQ_32		asqVarA	,		/**<[I ] a[s15.16]*/
				SQ_32		asqVarB	,		/**<[I ] b[s15.16]*/
				SQ_32		asqVarC	,		/**<[I ] c[s15.16]*/
				SQ_32		asqVarD	)		/**<[I ] d[s15.16]*/
/*------------------------------------------------------------------------*//**

	y = a*b + c*d
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	SQ_32	AlFixMac3(
				SQ_32		asqVarA	,		/**<[I ] a[s15.16]*/
				SQ_32		asqVarB	,		/**<[I ] b[s15.16]*/
				SQ_32		asqVarC	,		/**<[I ] c[s15.16]*/
				SQ_32		asqVarD	,		/**<[I ] d[s15.16]*/
				SQ_32		asqVarE	,		/**<[I ] e[s15.16]*/
				SQ_32		asqVarF	)		/**<[I ] f[s15.16]*/
/*------------------------------------------------------------------------*//**

	y = a*b + c*d + e*f
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	SQ_32	AlFixMac4(
				SQ_32		asqVarA	,		/**<[I ] a[s15.16]*/
				SQ_32		asqVarB	,		/**<[I ] b[s15.16]*/
				SQ_32		asqVarC	,		/**<[I ] c[s15.16]*/
				SQ_32		asqVarD	,		/**<[I ] d[s15.16]*/
				SQ_32		asqVarE	,		/**<[I ] e[s15.16]*/
				SQ_32		asqVarF	,		/**<[I ] f[s15.16]*/
				SQ_32		asqVarG	,		/**<[I ] g[s15.16]*/
				SQ_32		asqVarH	)		/**<[I ] h[s15.16]*/
/*------------------------------------------------------------------------*//**

	y = a*b + c*d + e*f + g*h
@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	void	AlFixMul3x3(	
				SQ_32*		apqDstM	,	/**<[ O] destination matrix( matrix c )*/
				SQ_32*		apqMatA	,	/**<[I ] matrix a*/
				SQ_32*		apqMatB	)	/**<[I ] matrix b*/
/*------------------------------------------------------------------------*//**
3x3 matrix multiply	{c} = {a} * {b}

@code
	| c00 c01 c02 |   | a00 a01 a02 |   | b00 b01 b02 |
	| c10 c11 c12 | = | a10 a11 a12 | * | b10 b11 b12 |
	| c20 c21 c22 |   | a20 a21 a22 |   | b20 b21 b22 |

	c00 = a00*b00 + a01*b10 + a02*b20;
	c01 = a00*b01 + a01*b11 + a02*b21;
	c02 = a00*b02 + a01*b12 + a02*b22;

	c10 = a10*b00 + a11*b10 + a12*b20;
	c11 = a10*b01 + a11*b11 + a12*b21;
	c12 = a10*b02 + a11*b12 + a12*b22;

	c20 = a20*b00 + a21*b10 + a22*b20;
	c21 = a20*b01 + a21*b11 + a22*b21;
	c22 = a20*b02 + a21*b12 + a22*b22;
@endcode

@return         void
@note			no overflow check, underflow check.
@n				no saturation.
@n
@n	array sample ...

@code
	SQ_32 matrixA[3*3] =
	{
		a00, a01, a02,
		a10, a11, a12,
		a20, a21, a22,
	};
@endcode
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	void	AlFixMul3x1(	
				SQ_32*		apqDstM	,	/**<[ O] destination matrix( matrix c )*/
				SQ_32*		apqMatA	,	/**<[I ] matrix a*/
				SQ_32*		apqMatB	)	/**<[I ] matrix b*/
/*------------------------------------------------------------------------*//**
3x1 matrix multiply	{c} = {a} * {b}

@code
	| c0 |   | a00 a01 a02 |   | b0 |
	| c1 | = | a10 a11 a12 | * | b1 |
	| c2 |   | a20 a21 a22 |   | b2 |

	c0 = a00*b0 + a01*b1 + a02*b2;
	c1 = a10*b0 + a11*b1 + a12*b2;
	c2 = a20*b0 + a21*b1 + a22*b2;
@endcode

@return         void
@note			no overflow check, underflow check.
@n				no saturation.
@n
@n	array sample ...

@code
	SQ_32 matrixA[3*3] =
	{
		a00, a01, a02,
		a10, a11, a12,
		a20, a21, a22,
	};
@endcode
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern TB_BOOL		AlFixMInv3x3(
					SQ_32*	psqInvMat	,	/* [ O]逆行列演算結果			 */
					SQ_32*	psqMat		)	/* [ O]算出元行列				 */
/*------------------------------------------------------------------------*//**
inverse matrix calculation ( 3x3 Matrix )

	

@return         Status
@retval         bAlTrue  :success
@retval         bAlFalse :There is no solution
@note			no overflow check, underflow check.
@n				no saturation.
@n
@n	array sample ...

@code
	SQ_32 matrixA[3*3] =
	{
		a00, a01, a02,
		a10, a11, a12,
		a20, a21, a22,
	};
@endcode
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	SQ_32	AlFixSin(
				SQ_32		asqX	)		/**<[I ] x[u16.16]*/
/*------------------------------------------------------------------------*//**
sine calculation ( 3x3 Matrix )

	y = SIN(x)

@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.

*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*---------------------------------------------------------------------------**
Description	:	y = COS(x)
Return		:	y[s15.16]
Note		:	none.
**---------------------------------------------------------------------------*/
/*===========================================================================*/
extern	SQ_32	AlFixCos(
				SQ_32		asqX	)		/**<[I ] x[u16.16]*/
/*------------------------------------------------------------------------*//**
cosine calculation ( 3x3 Matrix )

	y = COS(x)

@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.

*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
#define	AlFixTan( x )	( AlFixDiv( AlFixSin( (x) ) , AlFixCos( (x) )) )
/*------------------------------------------------------------------------*//**
tangent calculation ( 3x3 Matrix )

	y = TAN(x)

@return         y[s15.16]
@note			no overflow check, underflow check.
@n				no saturation.

*//*-------------------------------------------------------------------------*/
// When there is ";" before comment, Doxygen does not recognize it normally.

/*=============================================================================
	Variable
==============================================================================*/



#endif	/* AL_FIX_H */
