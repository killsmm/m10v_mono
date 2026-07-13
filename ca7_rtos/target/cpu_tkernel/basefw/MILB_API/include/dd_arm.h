/**
 * @file		dd_arm.h
 * @brief		ARM driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_arm
@{
	- @ref dd_arm_sample_section1
	- @ref dd_arm_sample_section2
*/

#ifndef _DD_ARM_H_
#define _DD_ARM_H_

#include "driver_common.h"
#include "dd_top.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define Dd_ARM_Critical_Section_Start(s)	{volatile unsigned r=Dd_ARM_Spin_Lock_Composite((ULONG*)&s)				/**< Critical Section Start */
#define Dd_ARM_Critical_Section_End(s)		Dd_ARM_Spin_Unlock((ULONG*)&s);Dd_ARM_EI(r);}							/**< Critical Section End */
#ifndef CO_DEBUG_ON_PC
#define Dd_ARM_Wait_ns(ns)					Dd_ARM_Wait_Count(1+((ns)*((D_DD_TOP_UCLK40I*33/2)/100000)/10000/4))	/**< Wait nsec */
#define Dd_ARM_Dmb_Pou()					__asm__ __volatile__ ("DMB 0xF")										/**< Data Memory Barrier */
#define Dd_ARM_Dsb_Pou()					__asm__ __volatile__ ("DSB 0xF")										/**< Data Synchronization Barrier */
#define Dd_ARM_Isb_Pou()					__asm__ __volatile__ ("ISB 0xF")										/**< Instruction Synchronization Barrier */
#define Dd_ARM_Dmb_Poc()					__asm__ __volatile__ ("DMB 0xF")										/**< Data Memory Barrier */
#define Dd_ARM_Dsb_Poc()					__asm__ __volatile__ ("DSB 0xF")										/**< Data Synchronization Barrier */
#define Dd_ARM_Isb_Poc()					__asm__ __volatile__ ("ISB 0xF")										/**< Instruction Synchronization Barrier */
#else
// for PCSim
#define Dd_ARM_Wait_ns(ns)					Dd_ARM_Wait_Count(ns)	/**< Wait nsec */
#define Dd_ARM_Dmb_Pou()
#define Dd_ARM_Dsb_Pou()
#define Dd_ARM_Isb_Pou()
#define Dd_ARM_Dmb_Poc()
#define Dd_ARM_Dsb_Poc()
#define Dd_ARM_Isb_Poc()
#endif

#define D_DD_ARM_NUM_OF_CORES					(4)	/**< CPU core number. */


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


/**
Wait by machine cycle count.
@param[in]	count	: wait count
*/
VOID Dd_ARM_Wait_Count(UINT32 count);

/**
Enable IRQ and Set program status included IRQ mask bit
@param[in]	sr		: program status included IRQ mask bit
*/
VOID Dd_ARM_EI(unsigned sr);

/**
Disable IRQ and Get program status included IRQ mask bit
@return		program status included IRQ mask bit
*/
unsigned Dd_ARM_DI(VOID);

/**
Spin Lock function.<br>
@param[in]	spin_id	: pointer of spin lock address
*/
VOID Dd_ARM_Spin_Lock(ULONG* spin_id);

/**
Spin Lock function.<br>
@param[in]	spin_id	: pointer of spin lock address
@retval		lock return (0:OK 1:NG)
*/
ULONG Dd_ARM_Spin_Lock_Ret(ULONG* spin_id);

/**
Spin Lock function.<br>
@param[in]	spin_id	: pointer of spin lock address
@return		program status
@remarks	When enable interrupt, It can accept interrupt.
			After this, interrupt is disable.
*/
unsigned Dd_ARM_Spin_Lock_Composite(ULONG* spin_id);

/**
Spin Unlock function.<br>
@param[in]	spin_id	: pointer of spin lock address
*/
VOID Dd_ARM_Spin_Unlock(ULONG* spin_id);

/**
Get CPU core ID.
@retval	0			: CPU0
@retval	1			: CPU1
@retval	2			: CPU2
@retval	3			: CPU3
*/
ULONG Dd_ARM_Get_CPU_ID(VOID);

/**
Get tranlation table base.
@retval	tranlation table base register0
*/
ULONG Dd_ARM_Get_TTB(VOID);

/**
Change vector table base.
@param[in]	address	: base address
*/
VOID Dd_ARM_Change_Vector_Table(UINT32 address);

/**
L1 Prefetch Disable.
*/
VOID Dd_ARM_L1_Prefetch_Disable(VOID);

/**
L1 Prefetch Enable.
*/
VOID Dd_ARM_L1_Prefetch_Enable(VOID);

/**
Enable IRQ & FIQ and Set program status included IRQ & FIQ mask bit.
@param[in]	sr			: program status included IRQ & FIQ mask bit
*/
VOID Dd_ARM_EIF(ULONG sr);

/**
Disable IRQ & FIQ and Get program status included IRQ & FIQ mask bit.
@retval	program status included IRQ & FIQ mask bit
*/
ULONG Dd_ARM_DIF(VOID);

/**
MMU Disable.
*/
VOID Dd_ARM_MMU_Disable(VOID);

/**
MMU Enable.
*/
VOID Dd_ARM_MMU_Enable(VOID);

#ifdef __cplusplus
}
#endif

#endif	// _DD_ARM_H_

/*@}*/

/**
@weakgroup dd_arm
@{
	@section dd_arm_sample_section1		Sample of 'Interrupt Enable/Disable Control' using Dd_ARM_Critical_Section_Start and Dd_ARM_Critical_Section_End functions.
	@code
	static ULONG xxxx_Spin_Lock1 __attribute__((section(".XXXXXXXX"), aligned(64)));
	VOID XXXX_function(VOID)
	{
		Dd_ARM_Critical_Section_Start(xxxx_Spin_Lock);
		//
		// critical setion.
		// disable interrupt area.
		//
		Dd_ARM_Critical_Section_End(xxxx_Spin_Lock);
	}
	@endcode
	<br>
	@section dd_arm_sample_section2		Sample of 'Spin Lock Control' using Dd_ARM_Spin_Lock and Dd_ARM_Spin_Unlock functions.
	@code
	static ULONG xxxx_Spin_Lock2 __attribute__((section(".XXXXXXXX"), aligned(64)));
	VOID XXXX_function(VOID)
	{
		Dd_ARM_Spin_Lock(&xxxx_Spin_Lock2);
		
		// macro settings, etc.
		
		Dd_ARM_Spin_Unlock(&xxxx_Spin_Lock2);
	}
    @endcode
@}*/
