/**
 * @file		USBHDC_AccessIF.c
 * @brief		Register & memory access function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USBHDC_AccessIF.h"

#include "USBHDC_MemMap.h"

#ifdef SAMPLE_DRIVER
#include "lib_test_device.h"
#endif

extern u8 byteOrder;  /* 0=little endian, 1=big endian */

extern u32 EndianChange(u32 indata)
{
	union 
	{
		unsigned int  In;
		unsigned char ibyte[4];
	} iData;
	union 
	{
		unsigned int  Out;
		unsigned char obyte[4];
	} oData;
	oData.Out = 0; /* initial value */
	
	iData.In = indata;
	oData.obyte[0]=iData.ibyte[3];
	oData.obyte[1]=iData.ibyte[2];
	oData.obyte[2]=iData.ibyte[1];
	oData.obyte[3]=iData.ibyte[0];
	return( oData.Out );
}

/*****************************************************************
 * [ Function ]		register read function					     *
 * [ Return code ]	u32 : register value.                        *
 * [ Parameter ]	u32 reg_addr : register address              *
 *****************************************************************/
u32 ReadRegister(u32 reg_addr)
{
	u32 val = 0;
	if( byteOrder == 0 )
	{
		val = *((u32 *)reg_addr);
	}
	else
	{
		val = EndianChange( *((u32 *)reg_addr) );
	}
	return val;
}


/*****************************************************************
 * [ Function ]		register write function                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 reg_addr : register address              *
 *                  u32 val : register value                     *
 *****************************************************************/
void WriteRegister(u32 reg_addr, u32 val)
{
	u32 tmp_val;
	if( byteOrder == 0 )
	{
		tmp_val = val;
	}
	else
	{
		tmp_val = EndianChange( val );
	}
	*((u32 *)reg_addr) = tmp_val;
}

#if 0
/*****************************************************************
 * [ Function ]		register read function                       *
 * [ Return code ]	u8 : register value.                         *
 * [ Parameter ]	u32 reg_addr : register address              *
 *****************************************************************/
u8 ReadRegister8(u32 reg_addr)
{
	u8 val = 0;

	/* user definistion */
	val = *((u8 *)reg_addr);

	return val;
}


/*****************************************************************
 * [ Function ]		register write function                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 reg_addr : register address              *
 *                  u8 val : register value                      *
 *****************************************************************/
void WriteRegister8(u32 reg_addr, u8 val)
{
	*((u8 *)reg_addr) = val;
}
#endif

/*****************************************************************
 * [ Function ]		memory 32bit read function                   *
 * [ Return code ]	u32 : memory value                           *
 * [ Parameter ]	u32 mem_addr : memory address                *
 *****************************************************************/
u32 ReadMemory32(u32 mem_addr)
{
	u32 val = 0;

	/* user definistion */
	val = *((u32 *)mem_addr);

	return val;
}


/*****************************************************************
 * [ Function ]		memory 32bit write function                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 mem_addr : memory address                *
 *                  u32 val : memory value                       *
 *****************************************************************/
void WriteMemory32(u32 mem_addr, u32 val)
{
	/* user definistion */
	*((u32 *)mem_addr) = val;
}


/*****************************************************************
 * [ Function ]		memory 8bit read function                    *
 * [ Return code ]	u8 : memory value                            *
 * [ Parameter ]	u32 mem_addr : memory address                *
 *****************************************************************/
u8 ReadMemory8(u32 mem_addr)
{
	u8 val = 0;

	/* user definistion */
//	val = *((u8 *)mem_addr);
	val = *((volatile u8 *)mem_addr);

	return val;
}


/*****************************************************************
 * [ Function ]		memory 8bit write function                   *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 mem_addr : memory address                *
 *                  u8 val : memory value                        *
 *****************************************************************/
void WriteMemory8(u32 mem_addr, u8 val)
{
	/* user definistion */
//	*((u8 *)mem_addr) = val;
	*((volatile u8 *)mem_addr) = val;
}

