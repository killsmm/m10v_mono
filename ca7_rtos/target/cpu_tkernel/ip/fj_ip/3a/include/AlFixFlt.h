/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef	AL_FIX_FLT_H
#define	AL_FIX_FLT_H

/*=============================================================================
	Macro definition Check
==============================================================================*/

/*=============================================================================
	Macro
==============================================================================*/

/*=============================================================================
	typedef
==============================================================================*/

/*=============================================================================
	Prototype
==============================================================================*/
/*===========================================================================*/
extern	void	AlFixIirHpfSetCoef(
				SQ_32		*apiCoef	,		/**<[ O] Filter Coefficient  		*/
				SQ_32		 asqFsmp	,		/**<[I ] Sampling frequency[s15.16] */
				SQ_32		 asqFcut	,		/**<[I ] Cutoff   frequency[s15.16] */
				SQ_32		 asqGain	)		/**<[I ] Output gain       [s15.16] */
/*------------------------------------------------------------------------*//**
Set coefficient filter for HPF

  calculating Hpf coefficient from argument.

@return         void
@note			quadric IIR filter( Butterworth filter)
@n
@n
parameter structure
@code
				apiCoef->[0]output gain
						 [1]coeff:a1
						 [2]coeff:a2
						 [3]coeff:b0
						 [4]coeff:b1
						 [5]coeff:b2
				apiZreg->[0]Z^-1
						 [1]Z^-2
@endcode
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	void	AlFixIir1LpfSetCoef(
				SQ_32		*apiCoef	,		// Filter Coefficient  
				SQ_32		 asqFsmp	,		// Sampling frequency[s15.16] 
				SQ_32		 asqFcut	,		// Cutoff   frequency[s15.16] 
				SQ_32		 asqGain	)		// Output gain       [s15.16]
/*------------------------------------------------------------------------*//**
Set coefficient filter for LPF

  calculating Lpf coefficient from argument.

@return         void
@note			quadric IIR filter( Butterworth filter)
@n
@n
parameter structure
@code
				apiCoef->[0]output gain
						 [1]coeff:a1
						 [2]coeff:a2
						 [3]coeff:b0
						 [4]coeff:b1
						 [5]coeff:b2
				apiZreg->[0]Z^-1
						 [1]Z^-2
@endcode
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	void	AlFixIirLpfSetCoef(
				SQ_32		*apiCoef	,		// Filter Coefficient  
				SQ_32		 asqFsmp	,		// Sampling frequency[s15.16] 
				SQ_32		 asqFcut	,		// Cutoff   frequency[s15.16] 
				SQ_32		 asqGain	)		// Output gain       [s15.16]
/*------------------------------------------------------------------------*//**
Set coefficient filter for LPF

  calculating Lpf coefficient from argument.

@return         void
@note			quadric IIR filter( Butterworth filter)
@n
@n
parameter structure
@code
				apiCoef->[0]output gain
						 [1]coeff:a1
						 [2]coeff:a2
						 [3]coeff:b0
						 [4]coeff:b1
						 [5]coeff:b2
				apiZreg->[0]Z^-1
						 [1]Z^-2
@endcode
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	void	AlFixIirLpfSetZreg(
				SQ_32		*apiCoef	,		// Filter Coefficient  
				SQ_32		*apiZreg	,		// Filter Z^-n Register
				SQ_32		 asqFill	)		// Initialize Fill Value  [s15.16]
/*------------------------------------------------------------------------*//**
Set Zreg coefficient for LPF

  calculating Lpf coefficient from argument.

@return         void
@note			quadric IIR filter( Butterworth filter)
@n
@n
parameter structure
@code
				apiCoef->[0]output gain
						 [1]coeff:a1
						 [2]coeff:a2
						 [3]coeff:b0
						 [4]coeff:b1
						 [5]coeff:b2
				apiZreg->[0]Z^-1
						 [1]Z^-2
				asqFa =  tan(pi * omega_d / omega_s)
						 omega_d : Cutoff   frequency
						 omega_s : Sampling frequency
@endcode
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
extern	SQ_32	AlFixIirLpf(
				SQ_32		*apiCoef	,		// Filter Coefficient  
				SQ_32		*apiZreg	,		// Filter Z^-n Register
				SQ_32		 asqXn		)		// Input data
/*------------------------------------------------------------------------*//**
calculating LPF


@return         output data = H(z)
@note			quadric IIR filter( Butterworth filter)
@n
@n
parameter structure
@code
				         b0 + b1*Z^-1 + b2*Z^-2
				H(z) = --------------------------
				         1  - a1*Z^-1 - a2*Z^-2

				X(n) --->(+)----------+---(b0)-->(+)-->(gain)--> Y(n)
						  A           |           A
						  |           V           |
						 (+)<--(a1)--[Z]--(b1)-->(+)
						  A           |           A
						  |           V           |
						  +----(a2)--[Z]--(b2)----+
@endcode
*//*-------------------------------------------------------------------------*/
;// When there is ";" before comment, Doxygen does not recognize it normally.

/*===========================================================================*/
#define		AlFixIirHpfSetZreg(apiCoef,apiZreg,asqFill)	AlFixIirLpfSetZreg((apiCoef),(apiZreg),(asqFill))
/*------------------------------------------------------------------------*//**
Set Zreg coefficient for HPF

  calculating Lpf coefficient from argument.

@return         void
@note			quadric IIR filter( Butterworth filter)
*//*-------------------------------------------------------------------------*/

/*===========================================================================*/
#define		AlFixIirHpf(apiCoef,apiZreg,asqXn)	AlFixIirLpf((apiCoef),(apiZreg),(asqXn))
/*------------------------------------------------------------------------*//**
calculating HPF


@return         output data = H(z)
@note			quadric IIR filter( Butterworth filter)
*//*-------------------------------------------------------------------------*/

/*=============================================================================
	Variable
==============================================================================*/


#endif	/* AL_FIX_FLT_H */
