/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef	AL_FIX_FFT_H
#define	AL_FIX_FFT_H

/*=============================================================================
	Macro definition Check
==============================================================================*/

/*=============================================================================
	Macro
==============================================================================*/

/*=============================================================================
	typedef
==============================================================================*/
/**	FFT management table	*/
typedef struct {
	UI_16		muiNum;	/**< [I ]Sampling count										*/
	SQ_32*		mpqRl;	/**< [I ]Sampling data(s03.16) / OUT:Real array(s15.16)		*/
	SQ_32*		mpqIm;	/**< [ O]Imaginary array(s15.16)							*/
	SQ_32*		mpqPw;	/**< [ O]Power spectrum(s15.16)								*/
	SQ_32*		mpqWk;	/**< [I ]Work area											*/
}TT_AlFixFft;

typedef UI_32	UI_AlFixFft;/**<FFT management table for work	*/

/*=============================================================================
	Prototype
==============================================================================*/
/*===========================================================================*/
extern	void	AlFixFftInit(	TT_AlFixFft	*	aptMng				,
								UI_16			auiNum				,
								UI_AlFixFft	*	apqWork				)
/*------------------------------------------------------------------------*//**
FFT initialize function



@return         void
@note			create Bit reverse table
@n				create SIN table
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.



/*===========================================================================*/
extern	void	AlFixFftExec(	
				TT_AlFixFft		*aptMng				)
/*------------------------------------------------------------------------*//**
FFT(Cooley-Tykey Fast Fourier transform)

@return         void
@note			no overflow check, underflow check.
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*=============================================================================
	Variable
==============================================================================*/

#endif	/* AL_FIX_FFT_H */
