/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef AL_AWBPLATFORM_H
#define AL_AWBPLATFORM_H
//===========================================================================**
//
//
//	include files
//
//
//---------------------------------------------------------------------------**

/*---------------------------------------------------------------------------**
 * Select Integral Color Format
**---------------------------------------------------------------------------*/
#define	AL_AWB_INTEGRAL_CF_R	( 1 )			//	R,Gr : Gb,B
#define	AL_AWB_INTEGRAL_CF_GR	( 2 )			//	Gr,R : B,Gb
#define	AL_AWB_INTEGRAL_CF_GB	( 3 )			//	Gb,B : R,Gr
#define	AL_AWB_INTEGRAL_CF_B	( 4 )			//	B,Gb : Gr,R
#define	AL_AWB_INTEGRAL_CF		AL_AWB_INTEGRAL_CF_R

//---------------------------------------------------------------------------**
//	ADI / ADSP BF561
//---------------------------------------------------------------------------**
#if (AL_PLATFORM == AL_PLATFORM_CA00)

//---------------------------------------------------------------------------**
//	SocioNext / Milbeaut
//---------------------------------------------------------------------------**
#elif ((AL_PLATFORM == AL_PLATFORM_EA00) \
    || (AL_PLATFORM == AL_PLATFORM_EA01) \
    || (AL_PLATFORM == AL_PLATFORM_EA02) \
    || (AL_PLATFORM == AL_PLATFORM_EB00) \
    || (AL_PLATFORM == AL_PLATFORM_EB01) \
    || (AL_PLATFORM == AL_PLATFORM_EB02) \
    || (AL_PLATFORM == AL_PLATFORM_EB03) \
    || (AL_PLATFORM == AL_PLATFORM_EB04) \
    || (AL_PLATFORM == AL_PLATFORM_EC01) \
    || (AL_PLATFORM == AL_PLATFORM_EC02) \
    || (AL_PLATFORM == AL_PLATFORM_ED01) \
    || (AL_PLATFORM == AL_PLATFORM_ED02) \
    || (AL_PLATFORM == AL_PLATFORM_EE01) \
      )

//---------------------------------------------------------------------------**
//	TI / DM270
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_DA00)

//---------------------------------------------------------------------------**
//	NECEL / CE131
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_GA00 || AL_PLATFORM == AL_PLATFORM_GA01)

//---------------------------------------------------------------------------**
//	inSilica / Garfield
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_HA00)

//---------------------------------------------------------------------------**
//	Avisonic / AcuteX
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_IA00)

//---------------------------------------------------------------------------**
//	Unknown platform
//---------------------------------------------------------------------------**
#else
	#error Not defined AL_PLATFORM

#endif	//AL_PLATFORM
//===========================================================================**
//
//
//	#define
//
//
//---------------------------------------------------------------------------**
//---------------------------------------------------------------------------**
//	ADI / ADSP BF561
//---------------------------------------------------------------------------**
#if (AL_PLATFORM == AL_PLATFORM_CA00)
#define ASSERT(val) 			
#define DIVIDE(a, b)			(AlFixDiv((a),(b)) >> 16)

//---------------------------------------------------------------------------**
//	SocioNext / Milbeaut
//---------------------------------------------------------------------------**
#elif ((AL_PLATFORM == AL_PLATFORM_EA00) \
    || (AL_PLATFORM == AL_PLATFORM_EA01) \
    || (AL_PLATFORM == AL_PLATFORM_EA02) \
    || (AL_PLATFORM == AL_PLATFORM_EB00) \
    || (AL_PLATFORM == AL_PLATFORM_EB01) \
    || (AL_PLATFORM == AL_PLATFORM_EB02) \
    || (AL_PLATFORM == AL_PLATFORM_EB03) \
    || (AL_PLATFORM == AL_PLATFORM_EB04) \
    || (AL_PLATFORM == AL_PLATFORM_EC01) \
    || (AL_PLATFORM == AL_PLATFORM_EC02) \
    || (AL_PLATFORM == AL_PLATFORM_ED01) \
    || (AL_PLATFORM == AL_PLATFORM_ED02) \
    || (AL_PLATFORM == AL_PLATFORM_EE01) \
      )
#define ASSERT(val) 			AlAwbAssert(val)
#define DIVIDE(a, b)			(AlFixDiv((a),(b)) >> 16)
//---------------------------------------------------------------------------**
//	TI / DM270
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_DA00)
#define ASSERT(val) 			
#define DIVIDE(a, b)			(AlFixDiv((a),(b)) >> 16)
//---------------------------------------------------------------------------**
//	NECEL / CE131
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_GA00 || AL_PLATFORM == AL_PLATFORM_GA01)
#define ASSERT(val) 
#define DIVIDE(a, b)			(AlFixDiv((a),(b)) >> 16)
//---------------------------------------------------------------------------**
//	inSilica / Garfield
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_HA00)
#define ASSERT(val) 			
#define DIVIDE(a, b)			(AlFixDiv((a),(b)) >> 16)
//---------------------------------------------------------------------------**
//	Avisonic / AcuteX
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_IA00)
#define ASSERT(val) 			
#define DIVIDE(a, b)			(AlFixDiv((a),(b)) >> 16)
//---------------------------------------------------------------------------**
//	Unknown platform
//---------------------------------------------------------------------------**
#else
	#error Not defined AL_PLATFORM

#endif	//AL_PLATFORM

//===========================================================================**
//
//
//	function prototype
//
//
//---------------------------------------------------------------------------**
//---------------------------------------------------------------------------**
//	ADI / ADSP BF561
//---------------------------------------------------------------------------**
#if (AL_PLATFORM == AL_PLATFORM_CA00)

//---------------------------------------------------------------------------**
//	SocioNext / Milbeaut
//---------------------------------------------------------------------------**
#elif ((AL_PLATFORM == AL_PLATFORM_EA00) \
    || (AL_PLATFORM == AL_PLATFORM_EA01) \
    || (AL_PLATFORM == AL_PLATFORM_EA02) \
    || (AL_PLATFORM == AL_PLATFORM_EB00) \
    || (AL_PLATFORM == AL_PLATFORM_EB01) \
    || (AL_PLATFORM == AL_PLATFORM_EB02) \
    || (AL_PLATFORM == AL_PLATFORM_EB03) \
    || (AL_PLATFORM == AL_PLATFORM_EB04) \
    || (AL_PLATFORM == AL_PLATFORM_EC01) \
    || (AL_PLATFORM == AL_PLATFORM_EC02) \
    || (AL_PLATFORM == AL_PLATFORM_ED01) \
    || (AL_PLATFORM == AL_PLATFORM_ED02) \
    || (AL_PLATFORM == AL_PLATFORM_EE01) \
      )
PUBLIC void		AlAwbAssert(SI_32 val);

//---------------------------------------------------------------------------**
//	TI / DM270
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_DA00)

//---------------------------------------------------------------------------**
//	NECEL / CE131
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_GA00 || AL_PLATFORM == AL_PLATFORM_GA01)

//---------------------------------------------------------------------------**
//	inSilica / Garfield
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_HA00)

//---------------------------------------------------------------------------**
//	Avisonic / AcuteX
//---------------------------------------------------------------------------**
#elif (AL_PLATFORM == AL_PLATFORM_IA00)

//---------------------------------------------------------------------------**
//	Unknown platform
//---------------------------------------------------------------------------**
#else
	#error Not defined AL_PLATFORM
#endif	//AL_PLATFORM


//==================================================================== EOF ==**
#endif /* AL_AWBPLATFORM_H */
