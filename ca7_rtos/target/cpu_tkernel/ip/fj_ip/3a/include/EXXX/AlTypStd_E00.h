/*============================================================================**
	Copyright 2015 Socionext Inc.
	Copyright Confidential. All rights reserved.
**----------------------------------------------------------------------------**

 	<<<<< comply with ALEMB standard. >>>>>
	[Note]
		This file will be overwritten in every project.

	[Warning]
		All c-source files must include this file.


**----------------------------------------------------------------------------*/
#ifndef AL_TYP_STD_E00_H
#define AL_TYP_STD_E00_H


/*============================================================================**

	Standard typedef

**============================================================================*/
/*----------------------------------------------------------**
	integer
**----------------------------------------------------------*/
typedef	signed		char	SI_08;
typedef	unsigned	char	UI_08;
typedef	signed		short	SI_16;
typedef	unsigned	short	UI_16;
typedef	signed		int		SI_32;
typedef	unsigned	int		UI_32;

/*----------------------------------------------------------**
	 fixed point
**----------------------------------------------------------*/
typedef	signed		short	SQ_16;		// s.15 format
typedef	unsigned	short	UQ_16;		// 0.16 format
typedef	signed		int		SQ_32;		// s15.16 format
typedef	unsigned	int		UQ_32;		// 16.16 format

/*----------------------------------------------------------**
	Floationg point
**----------------------------------------------------------*/
typedef float				FT_32;
typedef double				FT_64;

/*----------------------------------------------------------**
	 Boolean
**----------------------------------------------------------*/
typedef enum tg_BOOL{
  bAlFalse = 0,
  bAlTrue  = 1
} TB_BOOL;


/*============================================================================**

	Other Core dependenient Macro
		
**============================================================================*/

/*--------------------------------------------------**
	Endian Information
**--------------------------------------------------*/
#define	AL_ENDIAN					(AL_ENDIAN_BIG)

/*--------------------------------------------------**
	 ALC Basic Type Macro function::
**--------------------------------------------------*/
#define	MIN_HANDLING_BIT_WIDTH		(8)
#define	BYTE_SIZE_OF( xxx )			(sizeof(xxx) * (MIN_HANDLING_BIT_WIDTH/8))


#endif /*#ifndef  AL_TYP_STD_E00_H */
