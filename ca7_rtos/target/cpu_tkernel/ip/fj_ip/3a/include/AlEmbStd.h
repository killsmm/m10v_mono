/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef	ALC_EMB_STD_H
#define ALC_EMB_STD_H

/*-------------------------------------------------------------------------------
	Object scope
--------------------------------------------------------------------------------*/
	#define	PRIVATE			static				/**< def. for file   scope var 	*/
	#define	PUBLIC								/**< def. for global scope var 	*/
	#define	EXTERN			extern

/*-------------------------------------------------------------------------------
	Bit operator
--------------------------------------------------------------------------------*/
	#define	AL_BIT_AND(a,b)		((a) & (b))
	#define	AL_BIT_OR(a,b)			((a) | (b))
	#define	AL_BIT_XOR(a,b)		((a) ^ (b))
	#define	AL_BIT_REV(a)			(~(a))


	/* clrbit: 1=clear,0=not change */
	#define	AL_BIT_CLR(data,clrbit)			( AL_BIT_AND( (data) ,  AL_BIT_REV((clrbit) ) ) )
	/*---------------------------------------------**
	** Set masked bit data after clear bit.
	**
	**	AL_BIT_SET(0xABCD,0x0F00,0xFACE)
	**	=> result = 0xAACD
	**---------------------------------------------*/
	#define	AL_BIT_SET(tgt,msk,set)	( AL_BIT_OR( (AL_BIT_CLR( (tgt) , (msk) )) , (AL_BIT_AND( (set), (msk))) ) )

/*-------------------------------------------------------------------------------
	Build S/W Macro
--------------------------------------------------------------------------------*/
	#define	AL_DSBL				(0)
	#define	AL_ENBL				(1)

#endif /* ALC_EMB_STD_H*/
/*EOF*/
