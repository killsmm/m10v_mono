/**
 * @file		dd_top.h
 * @brief		CHIPTOP driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _DD_TOP_H_
#define _DD_TOP_H_

/**
@addtogroup lsi_top
@{
*/

#include "ddim_typedef.h"
#include "driver_common.h"
#include "chiptop.h"
#include "dd_arm.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/**
// Error Code
*/
#define	D_DD_TOP_INPUT_PARAM_ERROR		(D_DD_TOP | D_DDIM_INPUT_PARAM_ERROR)	/**< Input parameter error. */
#define	D_DD_TOP_STATUS_ABNORMAL		(D_DD_TOP | D_DDIM_STATUS_ABNORMAL) 	/**< Abnormal status error. */

// Base Clock
#define D_DD_TOP_UCLK40I			(40000000)	/**< Frequency of External Clock UCLK40I port */

// PLL out
#define D_DD_TOP_2200_PLL_OUT		(D_DD_TOP_UCLK40I * 55)					/**< 2200MHz : Frequency of PLL2200 */
#define D_DD_TOP_1600_PLL_OUT		(D_DD_TOP_UCLK40I * 40)					/**< 1600MHz : Frequency of PLL1600 */
#define D_DD_TOP_1500_PLL_OUT		(D_DD_TOP_UCLK40I / 2 * 75)				/**< 1500MHz : Frequency of PLL1500 */
#define D_DD_TOP_1400_PLL_OUT		(D_DD_TOP_UCLK40I * 35)					/**< 1400MHz : Frequency of PLL1400 */
#define D_DD_TOP_1200_PLL_OUT		(D_DD_TOP_UCLK40I * 30)					/**< 1200MHz : Frequency of PLL1200 */
//#define D_DD_TOP_1188_PLL_OUT		(D_DD_TOP_2200_PLL_OUT / 50 * (54 / 2))	/**< 1188MHz : Frequency of PLL1188 */
#define D_DD_TOP_1188_PLL_OUT		(D_DD_TOP_UCLK40I * 11 / 10 * 27)		/**< 1188MHz : Frequency of PLL1188 */
#define D_DD_TOP_864_PLL_OUT		(D_DD_TOP_UCLK40I / 5 * 108)			/**< 864MHz : Frequency of PLL864 */
#define D_DD_TOP_Audio_96_PLL_OUT	(D_DD_TOP_864_PLL_OUT / 125 * 96)	/**< 663.552MHz : Frequency of Audio PLL */
#define D_DD_TOP_Audio_98_PLL_OUT	(D_DD_TOP_864_PLL_OUT / 125 * 98)	/**< 677.376MHz : Frequency of Audio PLL */
#define D_DD_TOP_Audio_128_PLL_OUT	(D_DD_TOP_864_PLL_OUT / 125 * 128)	/**< 884.736MHz : Frequency of Audio PLL */

// CLKSTOP
#define Dd_Top_Get_CLKSTOP1()			(IO_CHIPTOP.CLKSTOP1.word)					/**< Get Clock Stop  1 */
#define Dd_Top_Get_CLKSTOP2()			(IO_CHIPTOP.CLKSTOP2.word)					/**< Get Clock Stop  2 */
#define Dd_Top_Get_CLKSTOP3()			(IO_CHIPTOP.CLKSTOP3.word)					/**< Get Clock Stop  3 */
#define Dd_Top_Get_CLKSTOP4()			(IO_CHIPTOP.CLKSTOP4.word)					/**< Get Clock Stop  4 */
#define Dd_Top_Get_CLKSTOP5()			(IO_CHIPTOP.CLKSTOP5.word)					/**< Get Clock Stop  5 */
#define Dd_Top_Get_CLKSTOP6()			(IO_CHIPTOP.CLKSTOP6.word)					/**< Get Clock Stop  6 */
#define Dd_Top_Get_CLKSTOP7()			(IO_CHIPTOP.CLKSTOP7.word)					/**< Get Clock Stop  7 */
#define Dd_Top_Get_CLKSTOP8()			(IO_CHIPTOP.CLKSTOP8.word)					/**< Get Clock Stop  8 */
#define Dd_Top_Get_CLKSTOP9()			(IO_CHIPTOP.CLKSTOP9.word)					/**< Get Clock Stop  9 */
#define Dd_Top_Get_CLKSTOP10()			(IO_CHIPTOP.CLKSTOP10.word)					/**< Get Clock Stop 10 */
#define Dd_Top_Get_CLKSTOP11()			(IO_CHIPTOP.CLKSTOP11.word)					/**< Get Clock Stop 11 */
#define Dd_Top_Get_CLKSTOP12()			(IO_CHIPTOP.CLKSTOP12.word)					/**< Get Clock Stop 12 */
#define Dd_Top_Get_CLKSTOP13()			(IO_CHIPTOP.CLKSTOP13.word)					/**< Get Clock Stop 13 */
#define Dd_Top_Get_CLKSTOP14()			(IO_CHIPTOP.CLKSTOP14.word)					/**< Get Clock Stop 14 */

// CLKSTOP-1
#define	D_DD_TOP_DSPCK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_DSPAX_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_SENCK_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_SENAX_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_SENAH_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_SENAP_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_GPIOAP_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_AU0CK_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_AU2CK_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_AU3CK_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_AU4CK_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_AU5CK_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_NETAUCK_BIT		(1<<24)		/**< Bit Location of * on CLKSTOP-1 */
#define	D_DD_TOP_TEMPCK_BIT			(1<<28)		/**< Bit Location of * on CLKSTOP-1 */

// CLKSTOP-2
#define	D_DD_TOP_RCK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_UHS1CK0_BIT		(1<<2)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_UHS1CK1_BIT		(1<<4)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_UHS1CK2_BIT		(1<<6)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_UHS2CK_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_NFCK_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_EMMCCK_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_NETSECCK_BIT		(1<<14)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_NETRCK_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_EXSAX_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_SPICK_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_SLIMB00CK_BIT		(1<<22)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_SLIMB01CK_BIT		(1<<24)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_SLIMB10CK_BIT		(1<<26)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_SLIMB11CK_BIT		(1<<28)		/**< Bit Location of * on CLKSTOP-2 */
#define	D_DD_TOP_PCISUPPCK_BIT		(1<<30)		/**< Bit Location of * on CLKSTOP-2 */

// CLKSTOP-3
#define	D_DD_TOP_IIPCK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_IIPAP_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_IIPAH_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_IIPAX_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_LCDCK_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_HIFCK_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_MIFCK_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_DISPAP_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_DISPAH_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_DISPAX_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_JPGCK_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_JPGAX_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_JPGAH_BIT			(1<<24)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_JPGAP_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_PDM0CK_BIT			(1<<28)		/**< Bit Location of * on CLKSTOP-3 */
#define	D_DD_TOP_PDM1CK_BIT			(1<<30)		/**< Bit Location of * on CLKSTOP-3 */

// CLKSTOP-4
#define	D_DD_TOP_GPUCK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_GPUAP_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_GPUAH_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_GPUAX_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_FPT0CK_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_FPT0AX_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_FPT0AH_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_FPT0AP_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_FPT1CK_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_FPT1AP_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_FPT1AH_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_FPT1AX_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_APCK0_BIT			(1<<24)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_APCK1_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_APCK2_BIT			(1<<28)		/**< Bit Location of * on CLKSTOP-4 */
#define	D_DD_TOP_APCK3_BIT			(1<<30)		/**< Bit Location of * on CLKSTOP-4 */

// CLKSTOP-5
#define	D_DD_TOP_MICAX0_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAX1_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAX2_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAX3_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAX4_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAX5_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAX6_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAP0_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAP1_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAP2_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAP3_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAP4_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAP5_BIT			(1<<24)		/**< Bit Location of * on CLKSTOP-5 */
#define	D_DD_TOP_MICAP6_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-5 */

// CLKSTOP-6
#define	D_DD_TOP_MICAH1_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_MICAH2_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_MICAH3_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_IMGAX_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_IMGAH0_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_IMGAH1_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_IMGAH3_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_IMGAP3_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_REGAP_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_XCHAX_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_XCHAP_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_ELACK_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_ELAAX_BIT			(1<<24)		/**< Bit Location of * on CLKSTOP-6 */
#define	D_DD_TOP_ELAAP_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-6 */

// CLKSTOP-7
#define	D_DD_TOP_IPUFDCK_BIT		(1<<0)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_IPUVISCK_BIT		(1<<2)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_IPUAX_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_IPUAH_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_RAWCK_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_RAWAX_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_RAWAP_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_SHDRCK_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_SHDRAX_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_SHDRAH_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_SHDRAP_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_M0CK_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_MECK_BIT			(1<<24)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_MEAX_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-7 */
#define	D_DD_TOP_MEAP_BIT			(1<<28)		/**< Bit Location of * on CLKSTOP-7 */

// CLKSTOP-8
#define	D_DD_TOP_RIBCK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_RIBAH_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_HEVDFCK_BIT		(1<<4)		/**< Bit Location of * on CLKSTOP-8 */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_DD_TOP_HEPXFCK_BIT		(1<<6)		/**< Bit Location of * on CLKSTOP-8 */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_DD_TOP_HEVENCCK_BIT		(1<<6)		/**< Bit Location of * on CLKSTOP-8 */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_DD_TOP_HEIPPCK_BIT		(1<<8)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC0HEVCMX2_BIT	(1<<10)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC0HEVCMX4_BIT	(1<<12)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC0RBRMX4_BIT		(1<<14)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC1HEVCMX2_BIT	(1<<16)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC1HEVCMX4_BIT	(1<<18)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC1RBRMX4_BIT		(1<<20)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC0CMNAX_BIT		(1<<22)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC0MX1AX_BIT		(1<<24)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC0MX2AX_BIT		(1<<26)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC0MX3AX_BIT		(1<<28)		/**< Bit Location of * on CLKSTOP-8 */
#define	D_DD_TOP_UMC0MX4AX_BIT		(1<<30)		/**< Bit Location of * on CLKSTOP-8 */

// CLKSTOP-9
#define	D_DD_TOP_UMC0MX5AX_BIT		(1<<0)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1CMNAX_BIT		(1<<2)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1MX1AX_BIT		(1<<4)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1MX2AX_BIT		(1<<6)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1MX3AX_BIT		(1<<8)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1MX4AX_BIT		(1<<10)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1MX5AX_BIT		(1<<12)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC0MX0AX3_BIT		(1<<14)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC0MX6AX4_BIT		(1<<16)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC0HEVCAX4_BIT	(1<<18)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1MX0AX3_BIT		(1<<20)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1MX6AX4_BIT		(1<<22)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1HEVCAX4_BIT	(1<<24)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC0AP_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-9 */
#define	D_DD_TOP_UMC1AP_BIT			(1<<28)		/**< Bit Location of * on CLKSTOP-9 */

// CLKSTOP-10
#define	D_DD_TOP_SRO1CK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_SRO1CK_2_BIT		(1<<2)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_SRO1AX_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_SRO1AH_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_SRO1AP_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_B2B1CK_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_B2B1AX_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_B2B1AH_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_B2B1AP_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_LTMRBK1CK_BIT		(1<<18)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_B2R1CK_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_B2R1AX_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_B2R1AH_BIT			(1<<24)		/**< Bit Location of * on CLKSTOP-10 */
#define	D_DD_TOP_B2R1AP_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-10 */

// CLKSTOP-11
#define	D_DD_TOP_LTM1CK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_LTM1AX_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_LTM1AH_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_LTM1AP_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_R2Y1CK_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_R2Y1AX_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_R2Y1AH_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_R2Y1AP_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_CNR1CK_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_CNR1AX_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_CNR1AP_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_APAH_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_DBGAP_BIT			(1<<24)		/**< Bit Location of * on CLKSTOP-11 */
#define	D_DD_TOP_NFBCHCK_BIT		(1<<26)		/**< Bit Location of * on CLKSTOP-11 */

// CLKSTOP-12
#define	D_DD_TOP_SRO2CK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_SRO2CK_2_BIT		(1<<2)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_SRO2AX_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_SRO2AH_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_SRO2AP_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_B2B2CK_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_B2B2AX_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_B2B2AH_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_B2B2AP_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_LTMRBK2CK_BIT		(1<<18)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_B2R2CK_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_B2R2AX_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_B2R2AH_BIT			(1<<24)		/**< Bit Location of * on CLKSTOP-12 */
#define	D_DD_TOP_B2R2AP_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-12 */

// CLKSTOP-13
#define	D_DD_TOP_LTM2CK_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_LTM2AX_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_LTM2AH_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_LTM2AP_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_R2Y2CK_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_R2Y2AX_BIT			(1<<10)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_R2Y2AH_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_R2Y2AP_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_CNR2CK_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_CNR2AX_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_CNR2AP_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_UMCVDFMX4_BIT		(1<<22)		/**< Bit Location of * on CLKSTOP-13 */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_DD_TOP_UMCPXFMX4_BIT		(1<<24)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_UMCVDFMX2_BIT		(1<<26)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_UMCPXFMX2_BIT		(1<<28)		/**< Bit Location of * on CLKSTOP-13 */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_DD_TOP_UMCHEVENCAX4_BIT	(1<<24)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_UMCVDFMX2_BIT		(1<<26)		/**< Bit Location of * on CLKSTOP-13 */
#define	D_DD_TOP_HEVENCAP_BIT		(1<<28)		/**< Bit Location of * on CLKSTOP-13 */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_DD_TOP_BMH1CK_BIT			(1<<30)		/**< Bit Location of * on CLKSTOP-13 */

// CLKSTOP-14
#define	D_DD_TOP_STATAX_BIT			(1<<0)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_STATAH_BIT			(1<<2)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_STATAP_BIT			(1<<4)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_PERIAH_BIT			(1<<6)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_PERIAP_BIT			(1<<8)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_SENMSKCK_BIT		(1<<10)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_GYROCK_BIT			(1<<12)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_EXSAH_BIT			(1<<14)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_EXSAP_BIT			(1<<16)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_PASCK_BIT			(1<<18)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_BMH0CK_BIT			(1<<20)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_BMH0AX_BIT			(1<<22)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_RDMAAX_BIT			(1<<26)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_RDMAAP_BIT			(1<<28)		/**< Bit Location of * on CLKSTOP-14 */
#define	D_DD_TOP_BMH1AX_BIT			(1<<30)		/**< Bit Location of * on CLKSTOP-14 */

// GPIO
// GPIO functional type (EPCR)
#define D_DD_TOP_GPIO_FUNC_GPIO		(0)		/**< GPIO mode */
#define D_DD_TOP_GPIO_FUNC_HWMODE	(1)		/**< Hardware mode (Resource output signal) */

// GPIO direction (DDR)
#define D_DD_TOP_GPIO_DIR_IN		(0)		/**< Input port */
#define D_DD_TOP_GPIO_DIR_OUT		(1)		/**< Output port */

// GPIO status (PDR)
#define D_DD_TOP_GPIO_STATUS_LOW	(0)		/**< Low status */
#define D_DD_TOP_GPIO_STATUS_HIGH	(1)		/**< High status */

// GPIO pull-up/down register enable (PUDER)
#define D_DD_TOP_GPIO_PUD_OFF		(0)		/**< Pull-up/down disable */
#define D_DD_TOP_GPIO_PUD_ON		(1)		/**< Pull-up/down enable */

// GPIO pull-up/down register control (PUDCR)
#define D_DD_TOP_GPIO_PUD_DOWN		(0)		/**< Pull-down */
#define D_DD_TOP_GPIO_PUD_UP		(1)		/**< Pull-Up */



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** GPIO port name */
typedef enum {
	E_DD_TOP_GPIO_P60	= 0,		/**< GPIO port P60 */
	E_DD_TOP_GPIO_P61,				/**< GPIO port P61 */
	E_DD_TOP_GPIO_P62,				/**< GPIO port P62 */
	E_DD_TOP_GPIO_P63,				/**< GPIO port P63 */
	E_DD_TOP_GPIO_P64,				/**< GPIO port P64 */
	E_DD_TOP_GPIO_P65,				/**< GPIO port P65 */
	E_DD_TOP_GPIO_P66,				/**< GPIO port P66 */
	E_DD_TOP_GPIO_P67,				/**< GPIO port P67 */
	E_DD_TOP_GPIO_P70,				/**< GPIO port P70 */
	E_DD_TOP_GPIO_P71,				/**< GPIO port P71 */
	E_DD_TOP_GPIO_P72,				/**< GPIO port P72 */
	E_DD_TOP_GPIO_P73,				/**< GPIO port P73 */
	E_DD_TOP_GPIO_P74,				/**< GPIO port P74 */
	E_DD_TOP_GPIO_P75,				/**< GPIO port P75 */
	E_DD_TOP_GPIO_P76,				/**< GPIO port P76 */
	E_DD_TOP_GPIO_P77,				/**< GPIO port P77 */
	E_DD_TOP_GPIO_P80,				/**< GPIO port P80 */
	E_DD_TOP_GPIO_P81,				/**< GPIO port P81 */
	E_DD_TOP_GPIO_P82,				/**< GPIO port P82 */
	E_DD_TOP_GPIO_P83,				/**< GPIO port P83 */
	E_DD_TOP_GPIO_P84,				/**< GPIO port P84 */
	E_DD_TOP_GPIO_P85,				/**< GPIO port P85 */
	E_DD_TOP_GPIO_P86,				/**< GPIO port P86 */
	E_DD_TOP_GPIO_P87,				/**< GPIO port P87 */
	E_DD_TOP_GPIO_P90,				/**< GPIO port P90 */
	E_DD_TOP_GPIO_P91,				/**< GPIO port P91 */
	E_DD_TOP_GPIO_P92,				/**< GPIO port P92 */
	E_DD_TOP_GPIO_P93,				/**< GPIO port P93 */
	E_DD_TOP_GPIO_PA0,				/**< GPIO port PA0 */
	E_DD_TOP_GPIO_PA1,				/**< GPIO port PA1 */
	E_DD_TOP_GPIO_PA2,				/**< GPIO port PA2 */
	E_DD_TOP_GPIO_PA3,				/**< GPIO port PA3 */
	E_DD_TOP_GPIO_PA4,				/**< GPIO port PA4 */
	E_DD_TOP_GPIO_PA5,				/**< GPIO port PA5 */
	E_DD_TOP_GPIO_PA6,				/**< GPIO port PA6 */
	E_DD_TOP_GPIO_PA7,				/**< GPIO port PB7 */
	E_DD_TOP_GPIO_PB0,				/**< GPIO port PB0 */
	E_DD_TOP_GPIO_PB1,				/**< GPIO port PB1 */
	E_DD_TOP_GPIO_PB2,				/**< GPIO port PB2 */
	E_DD_TOP_GPIO_PB3,				/**< GPIO port PB3 */
	E_DD_TOP_GPIO_PB4,				/**< GPIO port PB4 */
	E_DD_TOP_GPIO_PC0L,				/**< GPIO port PC0L */
	E_DD_TOP_GPIO_PC1L,				/**< GPIO port PC1L */
	E_DD_TOP_GPIO_PC2L,				/**< GPIO port PC2L */
	E_DD_TOP_GPIO_PC3L,				/**< GPIO port PC3L */
	E_DD_TOP_GPIO_PC4L,				/**< GPIO port PC4L */
	E_DD_TOP_GPIO_PC5L,				/**< GPIO port PC5L */
	E_DD_TOP_GPIO_PD0,				/**< GPIO port PD0 */
	E_DD_TOP_GPIO_PD1,				/**< GPIO port PD1 */
	E_DD_TOP_GPIO_PD2,				/**< GPIO port PD2 */
	E_DD_TOP_GPIO_PD3,				/**< GPIO port PD3 */
	E_DD_TOP_GPIO_PD4,				/**< GPIO port PD4 */
	E_DD_TOP_GPIO_PD5,				/**< GPIO port PD5 */
	E_DD_TOP_GPIO_PD6,				/**< GPIO port PD6 */
	E_DD_TOP_GPIO_PE0,				/**< GPIO port PE0 */
	E_DD_TOP_GPIO_PE1,				/**< GPIO port PE1 */
	E_DD_TOP_GPIO_PE2,				/**< GPIO port PE2 */
	E_DD_TOP_GPIO_PE3,				/**< GPIO port PE3 */
	E_DD_TOP_GPIO_PE4,				/**< GPIO port PE4 */
	E_DD_TOP_GPIO_PE5,				/**< GPIO port PE5 */
	E_DD_TOP_GPIO_PF0,				/**< GPIO port PF0 */
	E_DD_TOP_GPIO_PF1,				/**< GPIO port PF1 */
	E_DD_TOP_GPIO_PF2,				/**< GPIO port PF2 */
	E_DD_TOP_GPIO_PF3,				/**< GPIO port PF3 */
	E_DD_TOP_GPIO_PF4,				/**< GPIO port PF4 */
	E_DD_TOP_GPIO_PG0,				/**< GPIO port PG0 */
	E_DD_TOP_GPIO_PG1,				/**< GPIO port PG1 */
	E_DD_TOP_GPIO_PG2,				/**< GPIO port PG2 */
	E_DD_TOP_GPIO_PG3,				/**< GPIO port PG3 */
	E_DD_TOP_GPIO_PG4,				/**< GPIO port PG4 */
	E_DD_TOP_GPIO_PG5,				/**< GPIO port PG5 */
	E_DD_TOP_GPIO_PG6,				/**< GPIO port PG6 */
	E_DD_TOP_GPIO_PG7,				/**< GPIO port PG7 */
	E_DD_TOP_GPIO_PH0,				/**< GPIO port PH0 */
	E_DD_TOP_GPIO_PH1,				/**< GPIO port PH1 */
	E_DD_TOP_GPIO_PH2,				/**< GPIO port PH2 */
	E_DD_TOP_GPIO_PH3,				/**< GPIO port PH3 */
	E_DD_TOP_GPIO_PW0,				/**< GPIO port PW0 */
	E_DD_TOP_GPIO_PW1,				/**< GPIO port PW1 */
	E_DD_TOP_GPIO_PW2,				/**< GPIO port PW2 */
	E_DD_TOP_GPIO_PW3,				/**< GPIO port PW3 */
	E_DD_TOP_GPIO_PW4,				/**< GPIO port PW4 */
	E_DD_TOP_GPIO_PW5,				/**< GPIO port PW5 */
	E_DD_TOP_GPIO_PW6,				/**< GPIO port PW6 */
	E_DD_TOP_GPIO_PW7,				/**< GPIO port PW7 */
	E_DD_TOP_GPIO_PJ0,				/**< GPIO port PJ0 */
	E_DD_TOP_GPIO_PJ1,				/**< GPIO port PJ1 */
	E_DD_TOP_GPIO_PJ2,				/**< GPIO port PJ2 */
	E_DD_TOP_GPIO_PJ3,				/**< GPIO port PJ3 */
	E_DD_TOP_GPIO_PJ4,				/**< GPIO port PJ4 */
	E_DD_TOP_GPIO_PJ5,				/**< GPIO port PJ5 */
	E_DD_TOP_GPIO_PJ6,				/**< GPIO port PJ6 */
	E_DD_TOP_GPIO_PJ7,				/**< GPIO port PJ7 */
	E_DD_TOP_GPIO_PK0,				/**< GPIO port PK0 */
	E_DD_TOP_GPIO_PK1,				/**< GPIO port PK1 */
	E_DD_TOP_GPIO_PK2,				/**< GPIO port PK2 */
	E_DD_TOP_GPIO_PK3,				/**< GPIO port PK3 */
	E_DD_TOP_GPIO_PK4,				/**< GPIO port PK4 */
	E_DD_TOP_GPIO_PK5,				/**< GPIO port PK5 */
	E_DD_TOP_GPIO_PK6,				/**< GPIO port PK6 */
	E_DD_TOP_GPIO_PK7,				/**< GPIO port PK7 */
	E_DD_TOP_GPIO_PL0,				/**< GPIO port PL0 */
	E_DD_TOP_GPIO_PL1,				/**< GPIO port PL1 */
	E_DD_TOP_GPIO_PL2,				/**< GPIO port PL2 */
	E_DD_TOP_GPIO_PL3,				/**< GPIO port PL3 */
	E_DD_TOP_GPIO_PL4,				/**< GPIO port PL4 */
	E_DD_TOP_GPIO_PL5,				/**< GPIO port PL5 */
	E_DD_TOP_GPIO_PM0,				/**< GPIO port PM0 */
	E_DD_TOP_GPIO_PM1,				/**< GPIO port PM1 */
	E_DD_TOP_GPIO_PM2,				/**< GPIO port PM2 */
	E_DD_TOP_GPIO_PM3,				/**< GPIO port PM3 */
	E_DD_TOP_GPIO_PM4,				/**< GPIO port PM4 */
	E_DD_TOP_GPIO_PN0,				/**< GPIO port PN0 */
	E_DD_TOP_GPIO_PN1,				/**< GPIO port PN1 */
	E_DD_TOP_GPIO_PN2,				/**< GPIO port PN2 */
	E_DD_TOP_GPIO_PN3,				/**< GPIO port PN3 */
	E_DD_TOP_GPIO_PN4,				/**< GPIO port PN4 */
	E_DD_TOP_GPIO_PY0,				/**< GPIO port PY0 */
	E_DD_TOP_GPIO_PY1,				/**< GPIO port PY1 */
	E_DD_TOP_GPIO_PY2,				/**< GPIO port PY2 */
	E_DD_TOP_GPIO_PY3,				/**< GPIO port PY3 */
	E_DD_TOP_GPIO_PY4,				/**< GPIO port PY4 */
	E_DD_TOP_GPIO_PY5,				/**< GPIO port PY5 */
	E_DD_TOP_GPIO_PY6,				/**< GPIO port PY6 */
	E_DD_TOP_GPIO_PY7,				/**< GPIO port PY7 */
	E_DD_TOP_GPIO_PP0,				/**< GPIO port PP0 */
	E_DD_TOP_GPIO_PP1,				/**< GPIO port PP1 */
	E_DD_TOP_GPIO_PP2,				/**< GPIO port PP2 */
	E_DD_TOP_GPIO_PP3,				/**< GPIO port PP3 */
	E_DD_TOP_GPIO_PORT_MAX			/**< GPIO port maximum value */
} E_DD_TOP_GPIO_PORT;

#if 1    // special key control
/** GPIO CM0 port name */
typedef enum {
	E_DD_TOP_GPIO_CM0_P00	= 0,	/**< GPIO CM0 port P00 */
	E_DD_TOP_GPIO_CM0_P01,			/**< GPIO CM0 port P01 */
	E_DD_TOP_GPIO_CM0_P02,			/**< GPIO CM0 port P02 */
	E_DD_TOP_GPIO_CM0_P03,			/**< GPIO CM0 port P03 */
	E_DD_TOP_GPIO_CM0_P04,			/**< GPIO CM0 port P04 */
	E_DD_TOP_GPIO_CM0_P05,			/**< GPIO CM0 port P05 */
	E_DD_TOP_GPIO_CM0_P06,			/**< GPIO CM0 port P06 */
	E_DD_TOP_GPIO_CM0_P07,			/**< GPIO CM0 port P07 */
	E_DD_TOP_GPIO_CM0_P10,			/**< GPIO CM0 port P10 */
	E_DD_TOP_GPIO_CM0_P11,			/**< GPIO CM0 port P11 */
	E_DD_TOP_GPIO_CM0_P12,			/**< GPIO CM0 port P12 */
	E_DD_TOP_GPIO_CM0_P13,			/**< GPIO CM0 port P13 */
	E_DD_TOP_GPIO_CM0_P14,			/**< GPIO CM0 port P14 */
	E_DD_TOP_GPIO_CM0_P15,			/**< GPIO CM0 port P15 */
	E_DD_TOP_GPIO_CM0_P16,			/**< GPIO CM0 port P16 */
	E_DD_TOP_GPIO_CM0_P17,			/**< GPIO CM0 port P17 */
	E_DD_TOP_GPIO_CM0_P20,			/**< GPIO CM0 port P20 */
	E_DD_TOP_GPIO_CM0_P21,			/**< GPIO CM0 port P21 */
	E_DD_TOP_GPIO_CM0_P22,			/**< GPIO CM0 port P22 */
	E_DD_TOP_GPIO_CM0_P23,			/**< GPIO CM0 port P23 */
	E_DD_TOP_GPIO_CM0_P24,			/**< GPIO CM0 port P24 */
	E_DD_TOP_GPIO_CM0_P25,			/**< GPIO CM0 port P25 */
	E_DD_TOP_GPIO_CM0_P26,			/**< GPIO CM0 port P26 */
	E_DD_TOP_GPIO_CM0_P27,			/**< GPIO CM0 port P27 */
	E_DD_TOP_GPIO_CM0_P30,			/**< GPIO CM0 port P30 */
	E_DD_TOP_GPIO_CM0_P31,			/**< GPIO CM0 port P31 */
	E_DD_TOP_GPIO_CM0_P32,			/**< GPIO CM0 port P32 */
	E_DD_TOP_GPIO_CM0_P33,			/**< GPIO CM0 port P33 */
	E_DD_TOP_GPIO_CM0_P34,			/**< GPIO CM0 port P34 */
	E_DD_TOP_GPIO_CM0_P35,			/**< GPIO CM0 port P35 */
	E_DD_TOP_GPIO_CM0_P36,			/**< GPIO CM0 port P36 */
	E_DD_TOP_GPIO_CM0_P37,			/**< GPIO CM0 port P37 */
	E_DD_TOP_GPIO_CM0_P44,			/**< GPIO CM0 port P44 */
	E_DD_TOP_GPIO_CM0_P45,			/**< GPIO CM0 port P45 */
	E_DD_TOP_GPIO_CM0_P46,			/**< GPIO CM0 port P46 */
	E_DD_TOP_GPIO_CM0_PORT_MAX		/**< GPIO CM0 port maximum value */
} E_DD_TOP_GPIO_CM0_PORT;
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//@cond
extern ULONG gDD_Top_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));
//@endcond

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

#define	Dd_Top_Get_PERSEL1_I2C0CNT()		(IO_CHIPTOP.PERSEL1.bit.I2C0CNT)											/**< Get step number of filter for ISCL and ISDA ports */
#define	Dd_Top_Set_PERSEL1_I2C0CNT(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.I2C0CNT=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set step number of filter for ISCL and ISDA ports */

#define	Dd_Top_Get_PERSEL1_SDANC0()			(IO_CHIPTOP.PERSEL1.bit.SDANC0)												/**< Get filter enable for ISDA port */
#define	Dd_Top_Set_PERSEL1_SDANC0(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.SDANC0=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set filter enable for ISDA port */

#define	Dd_Top_Get_PERSEL1_SCLNC0()			(IO_CHIPTOP.PERSEL1.bit.SCLNC0)												/**< Get filter enable for ISCL port */
#define	Dd_Top_Set_PERSEL1_SCLNC0(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.SCLNC0=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set filter enable for ISCL port */

#define	Dd_Top_Get_PERSEL1_I2C1CNT()		(IO_CHIPTOP.PERSEL1.bit.I2C1CNT)											/**< Get step number of filter for IS1CL and IS1DA ports */
#define	Dd_Top_Set_PERSEL1_I2C1CNT(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.I2C1CNT=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set step number of filter for IS1CL and IS1DA ports */

#define	Dd_Top_Get_PERSEL1_SDANC1()			(IO_CHIPTOP.PERSEL1.bit.SDANC1)												/**< Get filter enable for IS1DA port */
#define	Dd_Top_Set_PERSEL1_SDANC1(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.SDANC1=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set filter enable for IS1DA port */

#define	Dd_Top_Get_PERSEL1_SCLNC1()			(IO_CHIPTOP.PERSEL1.bit.SCLNC1)												/**< Get filter enable for IS1CL port */
#define	Dd_Top_Set_PERSEL1_SCLNC1(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.SCLNC1=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set filter enable for IS1CL port */

#define	Dd_Top_Get_PERSEL1_I2C2CNT()		(IO_CHIPTOP.PERSEL1.bit.I2C2CNT)											/**< Get step number of filter for IS2CL and IS2DA ports */
#define	Dd_Top_Set_PERSEL1_I2C2CNT(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.I2C2CNT=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set step number of filter for IS2CL and IS2DA ports */

#define	Dd_Top_Get_PERSEL1_SDANC2()			(IO_CHIPTOP.PERSEL1.bit.SDANC2)												/**< Get filter enable for IS2DA port */
#define	Dd_Top_Set_PERSEL1_SDANC2(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.SDANC2=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set filter enable for IS2DA port */

#define	Dd_Top_Get_PERSEL1_SCLNC2()			(IO_CHIPTOP.PERSEL1.bit.SCLNC2)												/**< Get filter enable for IS2CL port */
#define	Dd_Top_Set_PERSEL1_SCLNC2(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.SCLNC2=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set filter enable for IS2CL port */

#define	Dd_Top_Get_PERSEL1_DVWT2()			(IO_CHIPTOP.PERSEL1.bit.DVWT2)												/**< Get HD to WTrg2 Select */
#define	Dd_Top_Set_PERSEL1_DVWT2(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.DVWT2=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set HD to WTrg2 Select */

#define	Dd_Top_Get_PERSEL1_DHWT3()			(IO_CHIPTOP.PERSEL1.bit.DHWT3)												/**< Get HD to WTrg3 Select */
#define	Dd_Top_Set_PERSEL1_DHWT3(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.DHWT3=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set HD to WTrg3 Select */

#define	Dd_Top_Get_PERSEL1_AU0DO0C()		(IO_CHIPTOP.PERSEL1.bit.AU0DO0C)											/**< Get AU0DO Signal0 IO Control */
#define	Dd_Top_Set_PERSEL1_AU0DO0C(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.AU0DO0C=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set AU0DO Signal0 IO Control */

#define	Dd_Top_Get_PERSEL1_AU0DO1C()		(IO_CHIPTOP.PERSEL1.bit.AU0DO1C)											/**< Get AU0DO Signal1 IO Control */
#define	Dd_Top_Set_PERSEL1_AU0DO1C(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.AU0DO1C=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set AU0DO Signal1 IO Control */

#define	Dd_Top_Get_PERSEL1_AU1DOC()			(IO_CHIPTOP.PERSEL1.bit.AU1DOC)												/**< Get AU1DO Signal IO Control */
#define	Dd_Top_Set_PERSEL1_AU1DOC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.AU1DOC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set AU1DO Signal IO Control */

#define	Dd_Top_Get_PERSEL1_AU0MCC()			(IO_CHIPTOP.PERSEL1.bit.AU0MCC)												/**< Get AU0MCLKO Signal IO Control */
#define	Dd_Top_Set_PERSEL1_AU0MCC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.AU0MCC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set AU0MCLKO Signal IO Control */

#define	Dd_Top_Get_PERSEL1_AU1MCC()			(IO_CHIPTOP.PERSEL1.bit.AU1MCC)												/**< Get AU1MCLK Signal IO Control */
#define	Dd_Top_Set_PERSEL1_AU1MCC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.AU1MCC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set AU1MCLK Signal IO Control */

#define	Dd_Top_Get_PERSEL1_RLSEL3()			(IO_CHIPTOP.PERSEL1.bit.RLSEL3)												/**< Get Peripheral Timer Trigger ch3 Select */
#define	Dd_Top_Set_PERSEL1_RLSEL3(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.RLSEL3=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Peripheral Timer Trigger ch3 Select */

#define	Dd_Top_Get_PERSEL1_RLSEL4()			(IO_CHIPTOP.PERSEL1.bit.RLSEL4)												/**< Get Peripheral Timer Trigger ch4 Select */
#define	Dd_Top_Set_PERSEL1_RLSEL4(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.RLSEL4=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Peripheral Timer Trigger ch4 Select */

#define	Dd_Top_Get_PERSEL1_RLSEL5()			(IO_CHIPTOP.PERSEL1.bit.RLSEL5)												/**< Get Peripheral Timer Trigger ch5 Select */
#define	Dd_Top_Set_PERSEL1_RLSEL5(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.RLSEL5=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Peripheral Timer Trigger ch5 Select */

#define	Dd_Top_Get_PERSEL1_INTVDSEL()		(IO_CHIPTOP.PERSEL1.bit.INTVDSEL)											/**< Get Interrupt VD Select */
#define	Dd_Top_Set_PERSEL1_INTVDSEL(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL1.bit.INTVDSEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Interrupt VD Select */

#define	Dd_Top_Get_PERSEL2_UDCTRG()			(IO_CHIPTOP.PERSEL2.bit.UDCTRG)												/**< Get UDC function Select */
#define	Dd_Top_Set_PERSEL2_UDCTRG(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDCTRG=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC function Select */

#define	Dd_Top_Get_PERSEL2_UDC0AM()			(IO_CHIPTOP.PERSEL2.bit.UDC0AM)												/**< Get UDC A-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC0AM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC0AM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC A-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC0BM()			(IO_CHIPTOP.PERSEL2.bit.UDC0BM)												/**< Get UDC B-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC0BM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC0BM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC B-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC1AM()			(IO_CHIPTOP.PERSEL2.bit.UDC1AM)												/**< Get UDC A-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC1AM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC1AM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC A-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC1BM()			(IO_CHIPTOP.PERSEL2.bit.UDC1BM)												/**< Get UDC B-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC1BM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC1BM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC B-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC2AM()			(IO_CHIPTOP.PERSEL2.bit.UDC2AM)												/**< Get UDC A-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC2AM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC2AM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC A-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC2BM()			(IO_CHIPTOP.PERSEL2.bit.UDC2BM)												/**< Get UDC B-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC2BM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC2BM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC B-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC3AM()			(IO_CHIPTOP.PERSEL2.bit.UDC3AM)												/**< Get UDC A-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC3AM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC3AM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC A-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC3BM()			(IO_CHIPTOP.PERSEL2.bit.UDC3BM)												/**< Get UDC B-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC3BM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC3BM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC B-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC4AM()			(IO_CHIPTOP.PERSEL2.bit.UDC4AM)												/**< Get UDC A-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC4AM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC4AM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC A-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC4BM()			(IO_CHIPTOP.PERSEL2.bit.UDC4BM)												/**< Get UDC B-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC4BM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC4BM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC B-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC5AM()			(IO_CHIPTOP.PERSEL2.bit.UDC5AM)												/**< Get UDC A-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC5AM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC5AM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC A-pin trigger input signal Mask */

#define	Dd_Top_Get_PERSEL2_UDC5BM()			(IO_CHIPTOP.PERSEL2.bit.UDC5BM)												/**< Get UDC B-pin trigger input signal Mask */
#define	Dd_Top_Set_PERSEL2_UDC5BM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.UDC5BM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set UDC B-pin trigger input signal Mask */

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	Dd_Top_Get_PERSEL2_SPIMSEN0()			(IO_CHIPTOP.PERSEL2.bit.SPIMSEN0)										/**< Get SPI I/O ch0 direction input/output */
#define	Dd_Top_Set_PERSEL2_SPIMSEN0(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.SPIMSEN0=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SPI I/O ch0 direction input/output */

#define	Dd_Top_Get_PERSEL2_SPIMSEN1()			(IO_CHIPTOP.PERSEL2.bit.SPIMSEN1)										/**< Get SPI I/O ch1 direction input/output */
#define	Dd_Top_Set_PERSEL2_SPIMSEN1(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.SPIMSEN1=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SPI I/O ch1 direction input/output */

#define	Dd_Top_Get_PERSEL2_SPIMSEN2()			(IO_CHIPTOP.PERSEL2.bit.SPIMSEN2)										/**< Get SPI I/O ch2 direction input/output */
#define	Dd_Top_Set_PERSEL2_SPIMSEN2(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL2.bit.SPIMSEN2=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SPI I/O ch2 direction input/output */

// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	Dd_Top_Get_PERSEL3_AU1WPPS()		(IO_CHIPTOP.PERSEL3.bit.AU1WPPS)											/**< Get AU0DO1 SPI3 IO Control */
#define	Dd_Top_Set_PERSEL3_AU1WPPS(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.AU1WPPS=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set AU0DO1 SPI3 IO Control */

#define	Dd_Top_Get_PERSEL3_AU0WP11()		(IO_CHIPTOP.PERSEL3.bit.AU0WP11)											/**< Get PA5/AU0DO1/WPPG11 IO Control */
#define	Dd_Top_Set_PERSEL3_AU0WP11(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.AU0WP11=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PA5/AU0DO1/WPPG11 IO Control */

#define	Dd_Top_Get_PERSEL3_SNH2WP04()		(IO_CHIPTOP.PERSEL3.bit.SNH2WP04)											/**< Get PG6/SNHD2/WPPG4 IO Control */
#define	Dd_Top_Set_PERSEL3_SNH2WP04(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.SNH2WP04=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PG6/SNHD2/WPPG4 IO Control */

#define	Dd_Top_Get_PERSEL3_SNV2WP05()		(IO_CHIPTOP.PERSEL3.bit.SNV2WP05)											/**< Get PG7/SNVD2/WPPG5 IO Control */
#define	Dd_Top_Set_PERSEL3_SNV2WP05(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.SNV2WP05=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PG7/SNVD2/WPPG5 IO Control */

#define	Dd_Top_Get_PERSEL3_SNA2WP06()		(IO_CHIPTOP.PERSEL3.bit.SNA2WP06)											/**< Get PH0/SNAPCLKO2/WPPG6 IO Control */
#define	Dd_Top_Set_PERSEL3_SNA2WP06(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.SNA2WP06=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PH0/SNAPCLKO2/WPPG6 IO Control */

#define	Dd_Top_Get_PERSEL3_SNH3WP07()		(IO_CHIPTOP.PERSEL3.bit.SNH3WP07)											/**< Get PH1/SNHD3/WPPG7 IO Control */
#define	Dd_Top_Set_PERSEL3_SNH3WP07(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.SNH3WP07=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PH1/SNHD3/WPPG7 IO Control */

#define	Dd_Top_Get_PERSEL3_SNV3WP08()		(IO_CHIPTOP.PERSEL3.bit.SNV3WP08)											/**< Get PH2/SNVD3/WPPG8 IO Control */
#define	Dd_Top_Set_PERSEL3_SNV3WP08(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.SNV3WP08=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PH2/SNVD3/WPPG8 IO Control */

#define	Dd_Top_Get_PERSEL3_SNA3WP09()		(IO_CHIPTOP.PERSEL3.bit.SNA3WP09)											/**< Get PH3/SNAPCLKO3/WPPG9 IO Control */
#define	Dd_Top_Set_PERSEL3_SNA3WP09(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.SNA3WP09=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PH3/SNAPCLKO3/WPPG9 IO Control */

#define	Dd_Top_Get_PERSEL3_U1VBSOU5()		(IO_CHIPTOP.PERSEL3.bit.U1VBSOU5)											/**< Get U1VBUSE/PM1/FPSOUT6 IO Control */
#define	Dd_Top_Set_PERSEL3_U1VBSOU5(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.U1VBSOU5=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set U1VBUSE/PM1/FPSOUT6 IO Control */

#define	Dd_Top_Get_PERSEL3_U2VBSOU6()		(IO_CHIPTOP.PERSEL3.bit.U2VBSOU6)											/**< Get U2VBUSE/PN1/FPSOUT7 IO Control */
#define	Dd_Top_Set_PERSEL3_U2VBSOU6(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.U2VBSOU6=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set U2VBUSE/PN1/FPSOUT7 IO Control */

#define	Dd_Top_Get_PERSEL3_U2IDWP14()		(IO_CHIPTOP.PERSEL3.bit.U2IDWP14)											/**< Get U2IDPU/PN4/WPPG14_1 IO Control */
#define	Dd_Top_Set_PERSEL3_U2IDWP14(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.U2IDWP14=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set U2IDPU/PN4/WPPG14_1 IO Control */

#define	Dd_Top_Get_PERSEL3_PRT1SOU3()		(IO_CHIPTOP.PERSEL3.bit.PRT1SOU3)											/**< Get PY1/PXRSTX1/FPSOUT4 IO Control */
#define	Dd_Top_Set_PERSEL3_PRT1SOU3(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.PRT1SOU3=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PY1/PXRSTX1/FPSOUT4 IO Control */

#define	Dd_Top_Get_PERSEL3_PXW0WP12()		(IO_CHIPTOP.PERSEL3.bit.PXW0WP12)											/**< Get PY4/PXWAKE0/WPPG12_1 IO Control */
#define	Dd_Top_Set_PERSEL3_PXW0WP12(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.PXW0WP12=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PY4/PXWAKE0/WPPG12_1 IO Control */

#define	Dd_Top_Get_PERSEL3_PXW1SOU7()		(IO_CHIPTOP.PERSEL3.bit.PXW1SOU7)											/**< Get PY5/PXWAKE1/FPSOUT8 IO Control */
#define	Dd_Top_Set_PERSEL3_PXW1SOU7(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.PXW1SOU7=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PY5/PXWAKE1/FPSOUT8 IO Control */

#define	Dd_Top_Get_PERSEL3_PXC0SCK7()		(IO_CHIPTOP.PERSEL3.bit.PXC0SCK7)											/**< Get PY6/PXCLKREQ0/FPSCK8 IO Control */
#define	Dd_Top_Set_PERSEL3_PXC0SCK7(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.PXC0SCK7=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PY6/PXCLKREQ0/FPSCK8 IO Control */

#define	Dd_Top_Get_PERSEL3_PXC1WP15()		(IO_CHIPTOP.PERSEL3.bit.PXC1WP15)											/**< Get PY7/PXCLKREQ1/WPPG15_1 IO Control */
#define	Dd_Top_Set_PERSEL3_PXC1WP15(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.PXC1WP15=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PY7/PXCLKREQ1/WPPG15_1 IO Control */

#define	Dd_Top_Get_PERSEL3_SCK4WP13()		(IO_CHIPTOP.PERSEL3.bit.SCK4WP13)											/**< Get SD3WP/PP1/FPSCK5/WPPG13_2 IO Control */
#define	Dd_Top_Set_PERSEL3_SCK4WP13(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.SCK4WP13=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SD3WP/PP1/FPSCK5/WPPG13_2 IO Control */

#define	Dd_Top_Get_PERSEL3_S3LSOU5WP14()	(IO_CHIPTOP.PERSEL3.bit.S3LSOU5WP14)										/**< Get SD3LEDX/PP2/FPSOUT5/WPPG14_2 IO Control */
#define	Dd_Top_Set_PERSEL3_S3LSOU5WP14(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.S3LSOU5WP14=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SD3LEDX/PP2/FPSOUT5/WPPG14_2 IO Control */

#define	Dd_Top_Get_PERSEL3_S3SWP15()		(IO_CHIPTOP.PERSEL3.bit.S3SWP15)											/**< Get SD3SW/PP3/UDTRG5B/WPPG15_2 IO Control */
#define	Dd_Top_Set_PERSEL3_S3SWP15(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.S3SWP15=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SD3SW/PP3/UDTRG5B/WPPG15_2 IO Control */

#define	Dd_Top_Get_PERSEL3_RTS1SCK2()		(IO_CHIPTOP.PERSEL3.bit.RTS1SCK2)											/**< Get P85/FPSRTS2/WTRG1/FPSCK3 IO Control */
#define	Dd_Top_Set_PERSEL3_RTS1SCK2(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.RTS1SCK2=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set P85/FPSRTS2/WTRG1/FPSCK3 IO Control */

#define	Dd_Top_Get_PERSEL3_U1IDWP13()		(IO_CHIPTOP.PERSEL3.bit.U1IDWP13)											/**< Get U1IDPU/PM4/WPPG13_1 IO Control */
#define	Dd_Top_Set_PERSEL3_U1IDWP13(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.U1IDWP13=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set U1IDPU/PM4/WPPG13_1 IO Control */

#define	Dd_Top_Get_PERSEL3_PRT0SCK3()		(IO_CHIPTOP.PERSEL3.bit.PRT0SCK3)											/**< Get PY0/PXRSTX0/FPSCK4 IO Control */
#define	Dd_Top_Set_PERSEL3_PRT0SCK3(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL3.bit.PRT0SCK3=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PY0/PXRSTX0/FPSCK4 IO Control */


#define	Dd_Top_Get_PERSEL4_EI22UD0A()		(IO_CHIPTOP.PERSEL4.bit.EI22UD0A)											/**< Get external interrupt request 22 */
#define	Dd_Top_Set_PERSEL4_EI22UD0A(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI22UD0A=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 22 */
#define	Dd_Top_Get_PERSEL4_EI23UD0B()		(IO_CHIPTOP.PERSEL4.bit.EI23UD0B)											/**< Get external interrupt request 23 */
#define	Dd_Top_Set_PERSEL4_EI23UD0B(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI23UD0B=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 23 */
#define	Dd_Top_Get_PERSEL4_EI24UD1A()		(IO_CHIPTOP.PERSEL4.bit.EI24UD1A)											/**< Get external interrupt request 24 */
#define	Dd_Top_Set_PERSEL4_EI24UD1A(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI24UD1A=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 24 */
#define	Dd_Top_Get_PERSEL4_EI25UD1B()		(IO_CHIPTOP.PERSEL4.bit.EI25UD1B)											/**< Get external interrupt request 25 */
#define	Dd_Top_Set_PERSEL4_EI25UD1B(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI25UD1B=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 25 */
#define	Dd_Top_Get_PERSEL4_EI26UD2A()		(IO_CHIPTOP.PERSEL4.bit.EI26UD2A)											/**< Get external interrupt request 26 */
#define	Dd_Top_Set_PERSEL4_EI26UD2A(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI26UD2A=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 26 */
#define	Dd_Top_Get_PERSEL4_EI27UD2B()		(IO_CHIPTOP.PERSEL4.bit.EI27UD2B)											/**< Get external interrupt request 27 */
#define	Dd_Top_Set_PERSEL4_EI27UD2B(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI27UD2B=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 27 */
#define	Dd_Top_Get_PERSEL4_EI28UD3A()		(IO_CHIPTOP.PERSEL4.bit.EI28UD3A)											/**< Get external interrupt request 28 */
#define	Dd_Top_Set_PERSEL4_EI28UD3A(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI28UD3A=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 28 */
#define	Dd_Top_Get_PERSEL4_EI29UD3B()		(IO_CHIPTOP.PERSEL4.bit.EI29UD3B)											/**< Get external interrupt request 29 */
#define	Dd_Top_Set_PERSEL4_EI29UD3B(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI29UD3B=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 29 */
#define	Dd_Top_Get_PERSEL4_EI30UD4A()		(IO_CHIPTOP.PERSEL4.bit.EI30UD4A)											/**< Get external interrupt request 30 */
#define	Dd_Top_Set_PERSEL4_EI30UD4A(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI30UD4A=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 30 */
#define	Dd_Top_Get_PERSEL4_EI31UD4B()		(IO_CHIPTOP.PERSEL4.bit.EI31UD4B)											/**< Get external interrupt request 31 */
#define	Dd_Top_Set_PERSEL4_EI31UD4B(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.EI31UD4B=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set external interrupt request 31 */
#define	Dd_Top_Get_PERSEL4_SD3CDXS()		(IO_CHIPTOP.PERSEL4.bit.SD3CDXS)											/**< Get SD card ch3 connection check */
#define	Dd_Top_Set_PERSEL4_SD3CDXS(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.SD3CDXS=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SD card ch3 connection check */
#define	Dd_Top_Get_PERSEL4_SD3WPS()			(IO_CHIPTOP.PERSEL4.bit.SD3WPS)												/**< Get SD card ch3 write protection */
#define	Dd_Top_Set_PERSEL4_SD3WPS(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.SD3WPS=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SD card ch3 write protection */
#define	Dd_Top_Get_PERSEL4_U1OCDXS()		(IO_CHIPTOP.PERSEL4.bit.U1OCDXS	)											/**< Get USB2.0 macro overcurrent detection */
#define	Dd_Top_Set_PERSEL4_U1OCDXS(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.U1OCDXS=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set USB2.0 macro overcurrent detection */
#define	Dd_Top_Get_PERSEL4_U1IDDIGS()		(IO_CHIPTOP.PERSEL4.bit.U1IDDIGS)											/**< Get USB2.0 macro ID detection */
#define	Dd_Top_Set_PERSEL4_U1IDDIGS(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.U1IDDIGS=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set USB2.0 macro ID detection */
#define	Dd_Top_Get_PERSEL4_U2OCDXS()		(IO_CHIPTOP.PERSEL4.bit.U2OCDXS)											/**< Get USB3.0&USB2.0 macro overcurrent detection */
#define	Dd_Top_Set_PERSEL4_U2OCDXS(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.U2OCDXS=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set USB3.0&USB2.0 macro overcurrent detection */
#define	Dd_Top_Get_PERSEL4_U2IDDIGS()		(IO_CHIPTOP.PERSEL4.bit.U2IDDIGS)											/**< Get USB3.0&USB2.0 macro ID detection */
#define	Dd_Top_Set_PERSEL4_U2IDDIGS(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PERSEL4.bit.U2IDDIGS=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set USB3.0&USB2.0 macro ID detection */


#define	Dd_Top_Get_MSELC_MSEL()				(IO_CHIPTOP.MSELC.bit.MSEL)													/**< Get power-supply voltage Control */
#define	Dd_Top_Set_MSELC_MSEL(val)			{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.MSELC.bit.MSEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set power-supply voltage Control */
	

#define	Dd_Top_Get_DBCNT1_T07DC()			(IO_CHIPTOP.DBCNT1.bit.T07DC)												/**< Get Driving Force Control 07 */
#define	Dd_Top_Set_DBCNT1_T07DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T07DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 07 */
#define	Dd_Top_Get_DBCNT1_T08DC()			(IO_CHIPTOP.DBCNT1.bit.T08DC)												/**< Get Driving Force Control 08 */
#define	Dd_Top_Set_DBCNT1_T08DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T08DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 08 */
#define	Dd_Top_Get_DBCNT1_T09DC()			(IO_CHIPTOP.DBCNT1.bit.T09DC)												/**< Get Driving Force Control 09 */
#define	Dd_Top_Set_DBCNT1_T09DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T09DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 09 */
#define	Dd_Top_Get_DBCNT1_T10DC()			(IO_CHIPTOP.DBCNT1.bit.T10DC)												/**< Get Driving Force Control 10 */
#define	Dd_Top_Set_DBCNT1_T10DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T10DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 10 */
#define	Dd_Top_Get_DBCNT1_T11DC()			(IO_CHIPTOP.DBCNT1.bit.T11DC)												/**< Get Driving Force Control 11 */
#define	Dd_Top_Set_DBCNT1_T11DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T11DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 11 */
#define	Dd_Top_Get_DBCNT1_T12DC()			(IO_CHIPTOP.DBCNT1.bit.T12DC)												/**< Get Driving Force Control 12 */
#define	Dd_Top_Set_DBCNT1_T12DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T12DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 12 */
#define	Dd_Top_Get_DBCNT1_T13DC()			(IO_CHIPTOP.DBCNT1.bit.T13DC)												/**< Get Driving Force Control 13 */
#define	Dd_Top_Set_DBCNT1_T13DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T13DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 13 */
#define	Dd_Top_Get_DBCNT1_T14DC()			(IO_CHIPTOP.DBCNT1.bit.T14DC)												/**< Get Driving Force Control 14 */
#define	Dd_Top_Set_DBCNT1_T14DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T14DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 14 */
#define	Dd_Top_Get_DBCNT1_T15DC()			(IO_CHIPTOP.DBCNT1.bit.T15DC)												/**< Get Driving Force Control 15 */
#define	Dd_Top_Set_DBCNT1_T15DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT1.bit.T15DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 15 */
#define	Dd_Top_Get_DBCNT2_T16DC()			(IO_CHIPTOP.DBCNT2.bit.T16DC)												/**< Get Driving Force Control 16 */
#define	Dd_Top_Set_DBCNT2_T16DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T16DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 16 */
#define	Dd_Top_Get_DBCNT2_T17DC()			(IO_CHIPTOP.DBCNT2.bit.T17DC)												/**< Get Driving Force Control 17 */
#define	Dd_Top_Set_DBCNT2_T17DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T17DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 17 */
#define	Dd_Top_Get_DBCNT2_T18DC()			(IO_CHIPTOP.DBCNT2.bit.T18DC)												/**< Get Driving Force Control 18 */
#define	Dd_Top_Set_DBCNT2_T18DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T18DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 18 */
#define	Dd_Top_Get_DBCNT2_T19DC()			(IO_CHIPTOP.DBCNT2.bit.T19DC)												/**< Get Driving Force Control 19 */
#define	Dd_Top_Set_DBCNT2_T19DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T19DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 19 */
#define	Dd_Top_Get_DBCNT2_T20DC()			(IO_CHIPTOP.DBCNT2.bit.T20DC)												/**< Get Driving Force Control 20 */
#define	Dd_Top_Set_DBCNT2_T20DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T20DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 20 */
#define	Dd_Top_Get_DBCNT2_T21DC()			(IO_CHIPTOP.DBCNT2.bit.T21DC)												/**< Get Driving Force Control 21 */
#define	Dd_Top_Set_DBCNT2_T21DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T21DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 21 */
#define	Dd_Top_Get_DBCNT2_T22DC()			(IO_CHIPTOP.DBCNT2.bit.T22DC)												/**< Get Driving Force Control 22 */
#define	Dd_Top_Set_DBCNT2_T22DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T22DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 22 */
#define	Dd_Top_Get_DBCNT2_T23DC()			(IO_CHIPTOP.DBCNT2.bit.T23DC)												/**< Get Driving Force Control 23 */
#define	Dd_Top_Set_DBCNT2_T23DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T23DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 23 */
#define	Dd_Top_Get_DBCNT2_T24DC()			(IO_CHIPTOP.DBCNT2.bit.T24DC)												/**< Get Driving Force Control 24 */
#define	Dd_Top_Set_DBCNT2_T24DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T24DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 24 */
#define	Dd_Top_Get_DBCNT2_T25DC()			(IO_CHIPTOP.DBCNT2.bit.T25DC)												/**< Get Driving Force Control 25 */
#define	Dd_Top_Set_DBCNT2_T25DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T25DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 25 */
#define	Dd_Top_Get_DBCNT2_T26DC()			(IO_CHIPTOP.DBCNT2.bit.T26DC)												/**< Get Driving Force Control 26 */
#define	Dd_Top_Set_DBCNT2_T26DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T26DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 26 */
#define	Dd_Top_Get_DBCNT2_T27DC()			(IO_CHIPTOP.DBCNT2.bit.T27DC)												/**< Get Driving Force Control 27 */
#define	Dd_Top_Set_DBCNT2_T27DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T27DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 27 */
#define	Dd_Top_Get_DBCNT2_T28DC()			(IO_CHIPTOP.DBCNT2.bit.T28DC)												/**< Get Driving Force Control 28 */
#define	Dd_Top_Set_DBCNT2_T28DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T28DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 28 */
#define	Dd_Top_Get_DBCNT2_T29DC()			(IO_CHIPTOP.DBCNT2.bit.T29DC)												/**< Get Driving Force Control 29 */
#define	Dd_Top_Set_DBCNT2_T29DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T29DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 29 */
#define	Dd_Top_Get_DBCNT2_T30DC()			(IO_CHIPTOP.DBCNT2.bit.T30DC)												/**< Get Driving Force Control 30 */
#define	Dd_Top_Set_DBCNT2_T30DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T30DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 30 */
#define	Dd_Top_Get_DBCNT2_T31DC()			(IO_CHIPTOP.DBCNT2.bit.T31DC)												/**< Get Driving Force Control 31 */
#define	Dd_Top_Set_DBCNT2_T31DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT2.bit.T31DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 31 */
#define	Dd_Top_Get_DBCNT3_T32DC()			(IO_CHIPTOP.DBCNT3.bit.T32DC)												/**< Get Driving Force Control 32 */
#define	Dd_Top_Set_DBCNT3_T32DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T32DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 32 */
#define	Dd_Top_Get_DBCNT3_T33DC()			(IO_CHIPTOP.DBCNT3.bit.T33DC)												/**< Get Driving Force Control 33 */
#define	Dd_Top_Set_DBCNT3_T33DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T33DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 33 */
#define	Dd_Top_Get_DBCNT3_T34DC()			(IO_CHIPTOP.DBCNT3.bit.T34DC)												/**< Get Driving Force Control 34 */
#define	Dd_Top_Set_DBCNT3_T34DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T34DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 34 */
#define	Dd_Top_Get_DBCNT3_T35DC()			(IO_CHIPTOP.DBCNT3.bit.T35DC)												/**< Get Driving Force Control 35 */
#define	Dd_Top_Set_DBCNT3_T35DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T35DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 35 */
#define	Dd_Top_Get_DBCNT3_T36DC()			(IO_CHIPTOP.DBCNT3.bit.T36DC)												/**< Get Driving Force Control 36 */
#define	Dd_Top_Set_DBCNT3_T36DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T36DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 36 */
#define	Dd_Top_Get_DBCNT3_T37DC()			(IO_CHIPTOP.DBCNT3.bit.T37DC)												/**< Get Driving Force Control 37 */
#define	Dd_Top_Set_DBCNT3_T37DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T37DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 37 */
#define	Dd_Top_Get_DBCNT3_T38DC()			(IO_CHIPTOP.DBCNT3.bit.T38DC)												/**< Get Driving Force Control 38 */
#define	Dd_Top_Set_DBCNT3_T38DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T38DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 38 */
#define	Dd_Top_Get_DBCNT3_T39DC()			(IO_CHIPTOP.DBCNT3.bit.T39DC)												/**< Get Driving Force Control 39 */
#define	Dd_Top_Set_DBCNT3_T39DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T39DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 39 */
#define	Dd_Top_Get_DBCNT3_T40DC()			(IO_CHIPTOP.DBCNT3.bit.T40DC)												/**< Get Driving Force Control 40 */
#define	Dd_Top_Set_DBCNT3_T40DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T40DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 40 */
#define	Dd_Top_Get_DBCNT3_T41DC()			(IO_CHIPTOP.DBCNT3.bit.T41DC)												/**< Get Driving Force Control 41 */
#define	Dd_Top_Set_DBCNT3_T41DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T41DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 41 */
#define	Dd_Top_Get_DBCNT3_T42DC()			(IO_CHIPTOP.DBCNT3.bit.T42DC)												/**< Get Driving Force Control 42 */
#define	Dd_Top_Set_DBCNT3_T42DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T42DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 42 */
#define	Dd_Top_Get_DBCNT3_T43DC()			(IO_CHIPTOP.DBCNT3.bit.T43DC)												/**< Get Driving Force Control 43 */
#define	Dd_Top_Set_DBCNT3_T43DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T43DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 43 */
#define	Dd_Top_Get_DBCNT3_T44DC()			(IO_CHIPTOP.DBCNT3.bit.T44DC)												/**< Get Driving Force Control 44 */
#define	Dd_Top_Set_DBCNT3_T44DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T44DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 44 */
#define	Dd_Top_Get_DBCNT3_T45DC()			(IO_CHIPTOP.DBCNT3.bit.T45DC)												/**< Get Driving Force Control 45 */
#define	Dd_Top_Set_DBCNT3_T45DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T45DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 45 */
#define	Dd_Top_Get_DBCNT3_T46DC()			(IO_CHIPTOP.DBCNT3.bit.T46DC)												/**< Get Driving Force Control 46 */
#define	Dd_Top_Set_DBCNT3_T46DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T46DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 46 */
#define	Dd_Top_Get_DBCNT3_T47DC()			(IO_CHIPTOP.DBCNT3.bit.T47DC)												/**< Get Driving Force Control 47 */
#define	Dd_Top_Set_DBCNT3_T47DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT3.bit.T47DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 47 */
#define	Dd_Top_Get_DBCNT4_T48DC()			(IO_CHIPTOP.DBCNT4.bit.T48DC)												/**< Get Driving Force Control 48 */
#define	Dd_Top_Set_DBCNT4_T48DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T48DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 48 */
#define	Dd_Top_Get_DBCNT4_T49DC()			(IO_CHIPTOP.DBCNT4.bit.T49DC)												/**< Get Driving Force Control 49 */
#define	Dd_Top_Set_DBCNT4_T49DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T49DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 49 */
#define	Dd_Top_Get_DBCNT4_T50DC()			(IO_CHIPTOP.DBCNT4.bit.T50DC)												/**< Get Driving Force Control 50 */
#define	Dd_Top_Set_DBCNT4_T50DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T50DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 50 */
#define	Dd_Top_Get_DBCNT4_T51DC()			(IO_CHIPTOP.DBCNT4.bit.T51DC)												/**< Get Driving Force Control 51 */
#define	Dd_Top_Set_DBCNT4_T51DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock) \
												;IO_CHIPTOP.DBCNT4.bit.T51DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 51 */
#define	Dd_Top_Get_DBCNT4_T52DC()			(IO_CHIPTOP.DBCNT4.bit.T52DC)												/**< Get Driving Force Control 52 */
#define	Dd_Top_Set_DBCNT4_T52DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T52DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 52 */
#define	Dd_Top_Get_DBCNT4_T53DC()			(IO_CHIPTOP.DBCNT4.bit.T53DC)												/**< Get Driving Force Control 53 */
#define	Dd_Top_Set_DBCNT4_T53DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T53DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 53 */
#define	Dd_Top_Get_DBCNT4_T54DC()			(IO_CHIPTOP.DBCNT4.bit.T54DC)												/**< Get Driving Force Control 54 */
#define	Dd_Top_Set_DBCNT4_T54DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T54DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 54 */
#define	Dd_Top_Get_DBCNT4_T55DC()			(IO_CHIPTOP.DBCNT4.bit.T55DC)												/**< Get Driving Force Control 55 */
#define	Dd_Top_Set_DBCNT4_T55DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T55DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 55 */
#define	Dd_Top_Get_DBCNT4_T56DC()			(IO_CHIPTOP.DBCNT4.bit.T56DC)												/**< Get Driving Force Control 56 */
#define	Dd_Top_Set_DBCNT4_T56DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T56DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 56 */
#define	Dd_Top_Get_DBCNT4_T57DC()			(IO_CHIPTOP.DBCNT4.bit.T57DC)												/**< Get Driving Force Control 57 */
#define	Dd_Top_Set_DBCNT4_T57DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T57DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 57 */
#define	Dd_Top_Get_DBCNT4_T58DC()			(IO_CHIPTOP.DBCNT4.bit.T58DC)												/**< Get Driving Force Control 58 */
#define	Dd_Top_Set_DBCNT4_T58DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T58DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 58 */
#define	Dd_Top_Get_DBCNT4_T59DC()			(IO_CHIPTOP.DBCNT4.bit.T59DC)												/**< Get Driving Force Control 59 */
#define	Dd_Top_Set_DBCNT4_T59DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T59DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 59 */
#define	Dd_Top_Get_DBCNT4_T60DC()			(IO_CHIPTOP.DBCNT4.bit.T60DC)												/**< Get Driving Force Control 60 */
#define	Dd_Top_Set_DBCNT4_T60DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T60DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 60 */
#define	Dd_Top_Get_DBCNT4_T61DC()			(IO_CHIPTOP.DBCNT4.bit.T61DC)												/**< Get Driving Force Control 61 */
#define	Dd_Top_Set_DBCNT4_T61DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T61DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 61 */
#define	Dd_Top_Get_DBCNT4_T62DC()			(IO_CHIPTOP.DBCNT4.bit.T62DC)												/**< Get Driving Force Control 62 */
#define	Dd_Top_Set_DBCNT4_T62DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T62DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 62 */
#define	Dd_Top_Get_DBCNT4_T63DC()			(IO_CHIPTOP.DBCNT4.bit.T63DC)												/**< Get Driving Force Control 63 */
#define	Dd_Top_Set_DBCNT4_T63DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT4.bit.T63DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 63 */
#define	Dd_Top_Get_DBCNT5_T64DC()			(IO_CHIPTOP.DBCNT5.bit.T64DC)												/**< Get Driving Force Control 64 */
#define	Dd_Top_Set_DBCNT5_T64DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT5.bit.T64DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 64 */
#define	Dd_Top_Get_DBCNT5_T65DC()			(IO_CHIPTOP.DBCNT5.bit.T65DC)												/**< Get Driving Force Control 65 */
#define	Dd_Top_Set_DBCNT5_T65DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT5.bit.T65DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 65 */
#define	Dd_Top_Get_DBCNT5_T66DC()			(IO_CHIPTOP.DBCNT5.bit.T66DC)												/**< Get Driving Force Control 66 */
#define	Dd_Top_Set_DBCNT5_T66DC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.DBCNT5.bit.T66DC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set Driving Force Control 66 */

#define	Dd_Top_Get_PUDCNT_EMMCNICS()		(IO_CHIPTOP.PUDCNT.bit.EMMCNICS)											/**< Get resource signal Select */
#define	Dd_Top_Set_PUDCNT_EMMCNICS(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.EMMCNICS=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set resource signal Select */
#define	Dd_Top_Get_PUDCNT_EMDATPUE()		(IO_CHIPTOP.PUDCNT.bit.EMDATPUE)											/**< Get EMDATA Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_EMDATPUE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.EMDATPUE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set EMDATA Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_EMCMDPUE()		(IO_CHIPTOP.PUDCNT.bit.EMCMDPUE)											/**< Get EMCMD Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_EMCMDPUE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.EMCMDPUE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set EMCMD Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_EMDSPDE()			(IO_CHIPTOP.PUDCNT.bit.EMDSPDE)												/**< Get EMDS Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_EMDSPDE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.EMDSPDE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set EMDS Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_NFDATPUE()		(IO_CHIPTOP.PUDCNT.bit.NFDATPUE)											/**< Get NFD Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_NFDATPUE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.NFDATPUE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set NFD Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_NFRBXPUE()		(IO_CHIPTOP.PUDCNT.bit.NFRBXPUE)											/**< Get NFRBX Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_NFRBXPUE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.NFRBXPUE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set NFRBX Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_NFDQSPUE()		(IO_CHIPTOP.PUDCNT.bit.NFDQSPUE)											/**< Get NFDQS Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_NFDQSPUE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.NFDQSPUE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set NFDQS Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_SP1MISOPE()		(IO_CHIPTOP.PUDCNT.bit.SP1MISOPE)											/**< Get SP2MISO Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_SP1MISOPE(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.SP1MISOPE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SP2MISO Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_SP1MOSIPE()		(IO_CHIPTOP.PUDCNT.bit.SP1MOSIPE)											/**< Get SP2MOSI Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_SP1MOSIPE(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.SP1MOSIPE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SP2MOSI Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_SP1SCKPE()		(IO_CHIPTOP.PUDCNT.bit.SP1SCKPE)											/**< Get SP2SCK Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_SP1SCKPE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.SP1SCKPE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SP2SCK Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_SP1CS0PE()		(IO_CHIPTOP.PUDCNT.bit.SP1CS0PE)											/**< Get SP2CS0X Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_SP1CS0PE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.SP1CS0PE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SP2CS0X Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_SP1MISOUDC()		(IO_CHIPTOP.PUDCNT.bit.SP1MISOUDC)											/**< Get SP2MISO Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_SP1MISOUDC(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.SP1MISOUDC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SP2MISO Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_SP1MOSIUDC()		(IO_CHIPTOP.PUDCNT.bit.SP1MOSIUDC)											/**< Get SP2MOSI Pull up/down Registance */
#define	Dd_Top_Set_PUDCNT_SP1MOSIUDC(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.SP1MOSIUDC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SP2MOSI Pull up/down Registance */
#define	Dd_Top_Get_PUDCNT_SP1SCKUDC()		(IO_CHIPTOP.PUDCNT.bit.SP1SCKUDC)											/**< Get SP2SCK Pull up/down Select */
#define	Dd_Top_Set_PUDCNT_SP1SCKUDC(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.SP1SCKUDC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SP2SCK Pull up/down Select */
#define	Dd_Top_Get_PUDCNT_SP1CS0UDC()		(IO_CHIPTOP.PUDCNT.bit.SP1CS0UDC)											/**< Get SP2CS0X Pull up/down Select */
#define	Dd_Top_Set_PUDCNT_SP1CS0UDC(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PUDCNT.bit.SP1CS0UDC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set SP2CS0X Pull up/down Select */

// CLKSEL
#define	Dd_Top_Get_CLKSEL1()					(IO_CHIPTOP.CLKSEL1.word)												/**< Get Clock Select  */
#define	Dd_Top_Get_CLKSEL2()				(IO_CHIPTOP.CLKSEL2.word)													/**< Get Clock Select 2 */
#define	Dd_Top_Get_CLKSEL3()				(IO_CHIPTOP.CLKSEL3.word)													/**< Get Clock Select 3 */
#define	Dd_Top_Get_CLKSEL4()				(IO_CHIPTOP.CLKSEL4.word)													/**< Get Clock Select 4 */
#define	Dd_Top_Get_CLKSEL5()				(IO_CHIPTOP.CLKSEL5.word)													/**< Get Clock Select 5 */
#define	Dd_Top_Get_CLKSEL6()				(IO_CHIPTOP.CLKSEL6.word)													/**< Get Clock Select 6 */
#define	Dd_Top_Get_CLKSEL7()				(IO_CHIPTOP.CLKSEL7.word)													/**< Get Clock Select 7 */
#define	Dd_Top_Get_CLKSEL8()				(IO_CHIPTOP.CLKSEL8.word)													/**< Get Clock Select 8 */
#define	Dd_Top_Get_CLKSEL9()				(IO_CHIPTOP.CLKSEL9.word)													/**< Get Clock Select 9 */
#define	Dd_Top_Get_CLKSEL10()				(IO_CHIPTOP.CLKSEL10.word)													/**< Get Clock Select 10 */
#define	Dd_Top_Get_CLKSEL11()				(IO_CHIPTOP.CLKSEL11.word)													/**< Get Clock Select 11 */
#define	Dd_Top_Get_CLKSEL12()				(IO_CHIPTOP.CLKSEL12.word)													/**< Get Clock Select 12 */


// CLKSEL1
#define	Dd_Top_Get_CLKSEL1_RCLK()			(IO_CHIPTOP.CLKSEL1.bit.RCLK&0x03)											/**< Get Peripheral Clock Select */
#define	Dd_Top_Set_CLKSEL1_RCLK(val)		(IO_CHIPTOP.CLKSEL1.bit.RCLK=(0x04|val))									/**< Set Peripheral Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL1_UHS1CLK0()		(IO_CHIPTOP.CLKSEL1.bit.UHS1CLK0&0x0F)										/**< Get UHS1 ch0 Clock Select */
#define	Dd_Top_Set_CLKSEL1_UHS1CLK0(val)	(IO_CHIPTOP.CLKSEL1.bit.UHS1CLK0=(0x10|val))								/**< Set UHS1 ch0 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL1_UHS1CLK1()		(IO_CHIPTOP.CLKSEL1.bit.UHS1CLK1&0x0F)										/**< Get UHS1 ch1 Clock Select */
#define	Dd_Top_Set_CLKSEL1_UHS1CLK1(val)	(IO_CHIPTOP.CLKSEL1.bit.UHS1CLK1=(0x10|val))								/**< Set UHS1 ch1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL1_UHS1CLK2()		(IO_CHIPTOP.CLKSEL1.bit.UHS1CLK2&0x0F)										/**< Get UHS1 ch2 Clock Select */
#define	Dd_Top_Set_CLKSEL1_UHS1CLK2(val)	(IO_CHIPTOP.CLKSEL1.bit.UHS1CLK2=(0x10|val))								/**< Set UHS1 ch2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL1_UHS2CLK()		(IO_CHIPTOP.CLKSEL1.bit.UHS2CLK&0x07)										/**< Get UHS2 Clock Select */
#define	Dd_Top_Set_CLKSEL1_UHS2CLK(val)		(IO_CHIPTOP.CLKSEL1.bit.UHS2CLK=(0x08|val))									/**< Set UHS2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL1_NFCLK()			(IO_CHIPTOP.CLKSEL1.bit.NFCLK&0x1F)											/**< Get Nand Flash I/F Clock Select */
#define	Dd_Top_Set_CLKSEL1_NFCLK(val)		(IO_CHIPTOP.CLKSEL1.bit.NFCLK=(0x20|val))									/**< Set Nand Flash I/F Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL1_EMMCCLK()		(IO_CHIPTOP.CLKSEL1.bit.EMMCCLK&0x03)										/**< Get eMMC I/F Clock Select */
#define	Dd_Top_Set_CLKSEL1_EMMCCLK(val)		(IO_CHIPTOP.CLKSEL1.bit.EMMCCLK=(0x04|val))									/**< Set eMMC I/F Clock Select (Set write bit internally) */
// CLKSEL2
#define	Dd_Top_Get_CLKSEL2_ELACLK()			(IO_CHIPTOP.CLKSEL2.bit.ELACLK&0x07)										/**< Get ELA Clock Select */
#define	Dd_Top_Set_CLKSEL2_ELACLK(val)		(IO_CHIPTOP.CLKSEL2.bit.ELACLK=(0x08|val))									/**< Set ELA Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL2_JPGCLK()			(IO_CHIPTOP.CLKSEL2.bit.JPGCLK&0x07)										/**< Get JPEG Clock Select */
#define	Dd_Top_Set_CLKSEL2_JPGCLK(val)		(IO_CHIPTOP.CLKSEL2.bit.JPGCLK=(0x08|val))									/**< Set JPEG Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL2_GPUCLK()			(IO_CHIPTOP.CLKSEL2.bit.GPUCLK&0x07)										/**< Get GPU Clock Select */
#define	Dd_Top_Set_CLKSEL2_GPUCLK(val)		(IO_CHIPTOP.CLKSEL2.bit.GPUCLK=(0x08|val))									/**< Set GPU Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL2_IPUTMECLK()		(IO_CHIPTOP.CLKSEL2.bit.IPUTMECLK&0x07)										/**< Get IPU TME Clock Select */
#define	Dd_Top_Set_CLKSEL2_IPUTMECLK(val)	(IO_CHIPTOP.CLKSEL2.bit.IPUTMECLK=(0x08|val))								/**< Set IPU TME Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL2_IPUCLK()			(IO_CHIPTOP.CLKSEL2.bit.IPUCLK&0x07)										/**< Get IPU System Clock Select */
#define	Dd_Top_Set_CLKSEL2_IPUCLK(val)		(IO_CHIPTOP.CLKSEL2.bit.IPUCLK=(0x08|val))									/**< Set IPU System Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL2_MIFCLK()			(IO_CHIPTOP.CLKSEL2.bit.MIFCLK&0x07)										/**< Get DISP macro MIF Clock Select */
#define	Dd_Top_Set_CLKSEL2_MIFCLK(val)		(IO_CHIPTOP.CLKSEL2.bit.MIFCLK=(0x08|val))									/**< Set DISP macro MIF Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL2_HIFCLK()			(IO_CHIPTOP.CLKSEL2.bit.HIFCLK&0x07)										/**< Get DISP macro HIF Clock Select */
#define	Dd_Top_Set_CLKSEL2_HIFCLK(val)		(IO_CHIPTOP.CLKSEL2.bit.HIFCLK=(0x08|val))									/**< Set DISP macro HIF Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL2_RAWCLK()			(IO_CHIPTOP.CLKSEL2.bit.RAWCLK&0x03)										/**< Get RAW macro Clock Select */
#define	Dd_Top_Set_CLKSEL2_RAWCLK(val)		(IO_CHIPTOP.CLKSEL2.bit.RAWCLK=(0x04|val))									/**< Set RAW macro Clock Select (Set write bit internally) */
// CLKSEL3
#define	Dd_Top_Get_CLKSEL3_VDFCLK()			(IO_CHIPTOP.CLKSEL3.bit.VDFCLK&0x07)										/**< Get HEVC macro VXF Clock Select */
#define	Dd_Top_Set_CLKSEL3_VDFCLK(val)		(IO_CHIPTOP.CLKSEL3.bit.VDFCLK=(0x08|val))									/**< Set HEVC macro VXF Clock Select (Set write bit internally) */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSEL3_PXFCLK()			(IO_CHIPTOP.CLKSEL3.bit.PXFCLK&0x07)										/**< Get HEVC macro PXF Clock Select */
#define	Dd_Top_Set_CLKSEL3_PXFCLK(val)		(IO_CHIPTOP.CLKSEL3.bit.PXFCLK=(0x08|val))									/**< Set HEVC macro PXF Clock Select (Set write bit internally) */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSEL3_HEVENCCLK()		(IO_CHIPTOP.CLKSEL3.bit.HEVENCCLK&0x07)										/**< Get HEVC macro ENC Clock Select */
#define	Dd_Top_Set_CLKSEL3_HEVENCCLK(val)	(IO_CHIPTOP.CLKSEL3.bit.HEVENCCLK=(0x08|val))								/**< Set HEVC macro ENC Clock Select (Set write bit internally) */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	Dd_Top_Get_CLKSEL3_IPPCLK()			(IO_CHIPTOP.CLKSEL3.bit.IPPCLK&0x0F)										/**< Get HEVC macro IPP Clock Select */
#define	Dd_Top_Set_CLKSEL3_IPPCLK(val)		(IO_CHIPTOP.CLKSEL3.bit.IPPCLK=(0x10|val))									/**< Set HEVC macro IPP Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL3_PASCLK()			(IO_CHIPTOP.CLKSEL3.bit.PASCLK&0x07)										/**< Get STAT macro Clock Select */
#define	Dd_Top_Set_CLKSEL3_PASCLK(val)		(IO_CHIPTOP.CLKSEL3.bit.PASCLK=(0x08|val))									/**< Set STAT macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL3_IIPCLK()			(IO_CHIPTOP.CLKSEL3.bit.IIPCLK&0x07)										/**< Get IIP macro Clock Select */
#define	Dd_Top_Set_CLKSEL3_IIPCLK(val)		(IO_CHIPTOP.CLKSEL3.bit.IIPCLK=(0x08|val))									/**< Set IIP macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL3_SENMSKCLK()		(IO_CHIPTOP.CLKSEL3.bit.SENMSKCLK&0x01)										/**< Get SENSOR macro Clock Select */
#define	Dd_Top_Set_CLKSEL3_SENMSKCLK(val)	(IO_CHIPTOP.CLKSEL3.bit.SENMSKCLK=(0x02|val))								/**< Set SENSOR macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL3_SENCLK()			(IO_CHIPTOP.CLKSEL3.bit.SENCLK&0x07)										/**< Get SENSOR macro Clock Select */
#define	Dd_Top_Set_CLKSEL3_SENCLK(val)		(IO_CHIPTOP.CLKSEL3.bit.SENCLK=(0x08|val))									/**< Set SENSOR macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL3_PIPESEL()		(IO_CHIPTOP.CLKSEL3.bit.PIPESEL&0x01)										/**< Get Source clock of image pipe macro Select */
#define	Dd_Top_Set_CLKSEL3_PIPESEL(val)		(IO_CHIPTOP.CLKSEL3.bit.PIPESEL=(0x02|val))									/**< Set Source clock of image pipe macro Select (Set write bit internally) */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSEL3_HEVENCSEL()		(IO_CHIPTOP.CLKSEL3.bit.HEVENCSEL&0x01)										/**< Get HEVENC macro Select */
#define	Dd_Top_Set_CLKSEL3_HEVENCSEL(val)	(IO_CHIPTOP.CLKSEL3.bit.HEVENCSEL=(0x02|val))								/**< Set HEVENC macro Select (Set write bit internally) */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// CLKSEL4
#define	Dd_Top_Get_CLKSEL4_CNR1CLK()		(IO_CHIPTOP.CLKSEL4.bit.CNR1CLK&0x3F)										/**< Get CNR macro pipe1 Clock Select */
#define	Dd_Top_Set_CLKSEL4_CNR1CLK(val)		(IO_CHIPTOP.CLKSEL4.bit.CNR1CLK=(0x40|val))									/**< Set CNR macro pipe1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL4_B2R1CLK()		(IO_CHIPTOP.CLKSEL4.bit.B2R1CLK&0x3F)										/**< Get B2R macro pipe1 Clock Select */
#define	Dd_Top_Set_CLKSEL4_B2R1CLK(val)		(IO_CHIPTOP.CLKSEL4.bit.B2R1CLK=(0x40|val))									/**< Set B2R macro pipe1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL4_LTM1CLK()		(IO_CHIPTOP.CLKSEL4.bit.LTM1CLK&0x3F)										/**< Get LTM macro pipe1 Clock Select */
#define	Dd_Top_Set_CLKSEL4_LTM1CLK(val)		(IO_CHIPTOP.CLKSEL4.bit.LTM1CLK=(0x40|val))									/**< Set LTM macro pipe1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL4_R2Y1CLK()		(IO_CHIPTOP.CLKSEL4.bit.R2Y1CLK&0x3F)										/**< Get R2Y macro pipe1 Clock Select */
#define	Dd_Top_Set_CLKSEL4_R2Y1CLK(val)		(IO_CHIPTOP.CLKSEL4.bit.R2Y1CLK=(0x40|val))									/**< Set R2Y macro pipe1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL4_SRO1CLK_2()		(IO_CHIPTOP.CLKSEL4.bit.SRO1CLK_2&0x07)										/**< Get SRO macro pipe1 input2 Clock Select */
#define	Dd_Top_Set_CLKSEL4_SRO1CLK_2(val)	(IO_CHIPTOP.CLKSEL4.bit.SRO1CLK_2=(0x08|val))								/**< Set SRO macro pipe1 input2 Clock Select (Set write bit internally) */
// CLKSEL5
#define	Dd_Top_Get_CLKSEL5_LTMRBK1CLK()		(IO_CHIPTOP.CLKSEL5.bit.LTMRBK1CLK&0x1F)									/**< Get LTMRBK macro pipe1 Clock Select */
#define	Dd_Top_Set_CLKSEL5_LTMRBK1CLK(val)	(IO_CHIPTOP.CLKSEL5.bit.LTMRBK1CLK=(0x20|val))								/**< Set LTMRBK macro pipe1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL5_B2B1CLK()		(IO_CHIPTOP.CLKSEL5.bit.B2B1CLK&0x1F)										/**< Get B2B macro pipe1 Clock Select */
#define	Dd_Top_Set_CLKSEL5_B2B1CLK(val)		(IO_CHIPTOP.CLKSEL5.bit.B2B1CLK=(0x20|val))									/**< Set B2B macro pipe1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL5_SRO1CLK()		(IO_CHIPTOP.CLKSEL5.bit.SRO1CLK&0x0F)										/**< Get SRO macro pipe1 Clock Select */
#define	Dd_Top_Set_CLKSEL5_SRO1CLK(val)		(IO_CHIPTOP.CLKSEL5.bit.SRO1CLK=(0x10|val))									/**< Set SRO macro pipe1 Clock Select (Set write bit internally) */
// CLKSEL6
#define	Dd_Top_Get_CLKSEL6_CNR2CLK()		(IO_CHIPTOP.CLKSEL6.bit.CNR2CLK&0x3F)										/**< Get CNR macro pipe2 Clock Select */
#define	Dd_Top_Set_CLKSEL6_CNR2CLK(val)		(IO_CHIPTOP.CLKSEL6.bit.CNR2CLK=(0x40|val))									/**< Set CNR macro pipe2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL6_B2R2CLK()		(IO_CHIPTOP.CLKSEL6.bit.B2R2CLK&0x3F)										/**< Get B2R macro pipe2 Clock Select */
#define	Dd_Top_Set_CLKSEL6_B2R2CLK(val)		(IO_CHIPTOP.CLKSEL6.bit.B2R2CLK=(0x40|val))									/**< Set B2R macro pipe2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL6_LTM2CLK()		(IO_CHIPTOP.CLKSEL6.bit.LTM2CLK&0x3F)										/**< Get LTM macro pipe2 Clock Select */
#define	Dd_Top_Set_CLKSEL6_LTM2CLK(val)		(IO_CHIPTOP.CLKSEL6.bit.LTM2CLK=(0x40|val))									/**< Set LTM macro pipe2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL6_R2Y2CLK()		(IO_CHIPTOP.CLKSEL6.bit.R2Y2CLK&0x3F)										/**< Get R2Y macro pipe2 Clock Select */
#define	Dd_Top_Set_CLKSEL6_R2Y2CLK(val)		(IO_CHIPTOP.CLKSEL6.bit.R2Y2CLK=(0x40|val))									/**< Set R2Y macro pipe2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL6_SRO2CLK_2()		(IO_CHIPTOP.CLKSEL6.bit.SRO2CLK_2&0x07)										/**< Get SRO macro pipe2 input2 Clock Select */
#define	Dd_Top_Set_CLKSEL6_SRO2CLK_2(val)	(IO_CHIPTOP.CLKSEL6.bit.SRO2CLK_2=(0x08|val))								/**< Set SRO macro pipe2 input2 Clock Select (Set write bit internally) */
// CLKSEL7
#define	Dd_Top_Get_CLKSEL7_LTMRBK2CLK()		(IO_CHIPTOP.CLKSEL7.bit.LTMRBK2CLK&0x1F)									/**< Get LTMRBK macro pipe2 Clock Select */
#define	Dd_Top_Set_CLKSEL7_LTMRBK2CLK(val)	(IO_CHIPTOP.CLKSEL7.bit.LTMRBK2CLK=(0x20|val))								/**< Set LTMRBK macro pipe2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL7_B2B2CLK()		(IO_CHIPTOP.CLKSEL7.bit.B2B2CLK&0x1F)										/**< Get B2B macro pipe2 Clock Select */
#define	Dd_Top_Set_CLKSEL7_B2B2CLK(val)		(IO_CHIPTOP.CLKSEL7.bit.B2B2CLK=(0x20|val))									/**< Set B2B macro pipe2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL7_SRO2CLK()		(IO_CHIPTOP.CLKSEL7.bit.SRO2CLK&0x0F)										/**< Get SRO macro pipe2 Clock Select */
#define	Dd_Top_Set_CLKSEL7_SRO2CLK(val)		(IO_CHIPTOP.CLKSEL7.bit.SRO2CLK=(0x10|val))									/**< Set SRO macro pipe2 Clock Select (Set write bit internally) */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSEL7_MIPISEL()		(IO_CHIPTOP.CLKSEL7.bit.MIPISEL&0x01)										/**< Get MIPI Clock Select */
#define	Dd_Top_Set_CLKSEL7_MIPISEL(val)		(IO_CHIPTOP.CLKSEL7.bit.MIPISEL=(0x02|val))									/**< Set MIPI Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL7_PDM0SEL()		(IO_CHIPTOP.CLKSEL7.bit.PDM0SEL&0x03)										/**< Get PDM0 Clock Select */
#define	Dd_Top_Set_CLKSEL7_PDM0SEL(val)		(IO_CHIPTOP.CLKSEL7.bit.PDM0SEL=(0x04|val))									/**< Set PDM0 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL7_PDM1SEL()		(IO_CHIPTOP.CLKSEL7.bit.PDM1SEL&0x03)										/**< Get PDM1 Clock Select */
#define	Dd_Top_Set_CLKSEL7_PDM1SEL(val)		(IO_CHIPTOP.CLKSEL7.bit.PDM1SEL=(0x04|val))									/**< Set PDM1 Clock Select (Set write bit internally) */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// CLKSEL8
#define	Dd_Top_Get_CLKSEL8_DSPCLK()			(IO_CHIPTOP.CLKSEL8.bit.DSPCLK&0x03)										/**< Get DSP macro Clock Select */
#define	Dd_Top_Set_CLKSEL8_DSPCLK(val)		(IO_CHIPTOP.CLKSEL8.bit.DSPCLK=(0x04|val))									/**< Set DSP macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_SPICLK()			(IO_CHIPTOP.CLKSEL8.bit.SPICLK&0x03)										/**< Get SPI macro Clock Select */
#define	Dd_Top_Set_CLKSEL8_SPICLK(val)		(IO_CHIPTOP.CLKSEL8.bit.SPICLK=(0x04|val))									/**< Set SPI macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_AUCLK()			(IO_CHIPTOP.CLKSEL8.bit.AUCLK&0x03)											/**< Get audio Clock Select */
#define	Dd_Top_Set_CLKSEL8_AUCLK(val)		(IO_CHIPTOP.CLKSEL8.bit.AUCLK=(0x04|val))									/**< Set audio Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_AU0SEL()			(IO_CHIPTOP.CLKSEL8.bit.AU0SEL&0x01)										/**< Get audio macro 0ch Clock Select */
#define	Dd_Top_Set_CLKSEL8_AU0SEL(val)		(IO_CHIPTOP.CLKSEL8.bit.AU0SEL=(0x02|val))									/**< Set audio macro 0ch Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_AU2SEL()			(IO_CHIPTOP.CLKSEL8.bit.AU2SEL&0x01)										/**< Get audio macro 2ch Clock Select */
#define	Dd_Top_Set_CLKSEL8_AU2SEL(val)		(IO_CHIPTOP.CLKSEL8.bit.AU2SEL=(0x02|val))									/**< Set audio macro 2ch Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_AU3SEL()			(IO_CHIPTOP.CLKSEL8.bit.AU3SEL&0x01)										/**< Get audio macro 3ch Clock Select */
#define	Dd_Top_Set_CLKSEL8_AU3SEL(val)		(IO_CHIPTOP.CLKSEL8.bit.AU3SEL=(0x02|val))									/**< Set audio macro 3ch Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_NETAUSEL()		(IO_CHIPTOP.CLKSEL8.bit.NETAUSEL&0x01)										/**< Get NETSEC macro Clock Select */
#define	Dd_Top_Set_CLKSEL8_NETAUSEL(val)	(IO_CHIPTOP.CLKSEL8.bit.NETAUSEL=(0x02|val))								/**< Set NETSEC macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_APCLK()			(IO_CHIPTOP.CLKSEL8.bit.APCLK&0x1F)											/**< Get APCLK Clock Select */
#define	Dd_Top_Set_CLKSEL8_APCLK(val)		(IO_CHIPTOP.CLKSEL8.bit.APCLK=(0x20|val))									/**< Set APCLK Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_AP0SEL()			(IO_CHIPTOP.CLKSEL8.bit.AP0SEL&0x01)										/**< Get SNAPCLKO0 Clock Select */
#define	Dd_Top_Set_CLKSEL8_AP0SEL(val)		(IO_CHIPTOP.CLKSEL8.bit.AP0SEL=(0x02|val))									/**< Set SNAPCLKO0 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_AP1SEL()			(IO_CHIPTOP.CLKSEL8.bit.AP1SEL&0x01)										/**< Get SNAPCLKO1 Clock Select */
#define	Dd_Top_Set_CLKSEL8_AP1SEL(val)		(IO_CHIPTOP.CLKSEL8.bit.AP1SEL=(0x02|val))									/**< Set SNAPCLKO1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_AP2SEL()			(IO_CHIPTOP.CLKSEL8.bit.AP2SEL&0x01)										/**< Get SNAPCLKO2 Clock Select */
#define	Dd_Top_Set_CLKSEL8_AP2SEL(val)		(IO_CHIPTOP.CLKSEL8.bit.AP2SEL=(0x02|val))									/**< Set SNAPCLKO2 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL8_AP3SEL()			(IO_CHIPTOP.CLKSEL8.bit.AP3SEL&0x01)										/**< Get SNAPCLKO3 Clock Select */
#define	Dd_Top_Set_CLKSEL8_AP3SEL(val)		(IO_CHIPTOP.CLKSEL8.bit.AP3SEL=(0x02|val))									/**< Set SNAPCLKO3 Clock Select (Set write bit internally) */
// CLKSEL9
#define	Dd_Top_Get_CLKSEL9_PCLK()			(IO_CHIPTOP.CLKSEL9.bit.PCLK&0x3F)											/**< Get APB bus Clock Select */
#define	Dd_Top_Set_CLKSEL9_PCLK(val)		(IO_CHIPTOP.CLKSEL9.bit.PCLK=(0x40|val))									/**< Set APB bus Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL9_HCLK()			(IO_CHIPTOP.CLKSEL9.bit.HCLK&0x0F)											/**< Get AHB bus Clock Select */
#define	Dd_Top_Set_CLKSEL9_HCLK(val)		(IO_CHIPTOP.CLKSEL9.bit.HCLK=(0x10|val))									/**< Set AHB bus Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL9_HCLKBMH()		(IO_CHIPTOP.CLKSEL9.bit.HCLKBMH&0x07)										/**< Get AHB bus BMH block Clock Select */
#define	Dd_Top_Set_CLKSEL9_HCLKBMH(val)		(IO_CHIPTOP.CLKSEL9.bit.HCLKBMH=(0x08|val))									/**< Set AHB bus BMH block Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL9_ACLKEXS()		(IO_CHIPTOP.CLKSEL9.bit.ACLKEXS&0x07)										/**< Get AXI bus EXS block Clock Select */
#define	Dd_Top_Set_CLKSEL9_ACLKEXS(val)		(IO_CHIPTOP.CLKSEL9.bit.ACLKEXS=(0x08|val))									/**< Set AXI bus EXS block Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL9_ACLK()			(IO_CHIPTOP.CLKSEL9.bit.ACLK&0x07)											/**< Get AXI bus Clock Select */
#define	Dd_Top_Set_CLKSEL9_ACLK(val)		(IO_CHIPTOP.CLKSEL9.bit.ACLK=(0x08|val))									/**< Set AXI bus Clock Select (Set write bit internally) */
// CLKSEL10
#define	Dd_Top_Get_CLKSEL10_ACLK400()		(IO_CHIPTOP.CLKSEL10.bit.ACLK400&0x03)										/**< Get AXI bus Clock Select */
#define	Dd_Top_Set_CLKSEL10_ACLK400(val)	(IO_CHIPTOP.CLKSEL10.bit.ACLK400=(0x04|val))								/**< Set AXI bus Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL10_MCLK200()		(IO_CHIPTOP.CLKSEL10.bit.MCLK200&0x07)										/**< Get MXI bus Clock Select */
#define	Dd_Top_Set_CLKSEL10_MCLK200(val)	(IO_CHIPTOP.CLKSEL10.bit.MCLK200=(0x08|val))								/**< Set MXI bus Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL10_MCLK400()		(IO_CHIPTOP.CLKSEL10.bit.MCLK400&0x03)										/**< Get MXI bus Clock Select */
#define	Dd_Top_Set_CLKSEL10_MCLK400(val)	(IO_CHIPTOP.CLKSEL10.bit.MCLK400=(0x04|val))								/**< Set MXI bus Clock Select (Set write bit internally) */
// CLKSEL11
#define	Dd_Top_Get_CLKSEL11_DCHREQ()		(IO_CHIPTOP.CLKSEL11.bit.DCHREQ)											/**< Get CLKSEL9 and CLKSEL10 division ratio Select */
#define	Dd_Top_Set_CLKSEL11_DCHREQ(val)		(IO_CHIPTOP.CLKSEL11.bit.DCHREQ=(0x01|val))									/**< Set CLKSEL9 and CLKSEL10 division ratio Select (Set write bit internally) */
// CLKSEL12
#define	Dd_Top_Get_CLKSEL12_ACLK300()		(IO_CHIPTOP.CLKSEL12.bit.ACLK300&0x01)										/**< Get AXI bus Clock Select */
#define	Dd_Top_Set_CLKSEL12_ACLK300(val)	(IO_CHIPTOP.CLKSEL12.bit.ACLK300=(0x02|val))								/**< Set AXI bus Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL12_GYROCLK()		(IO_CHIPTOP.CLKSEL12.bit.GYROCLK&0x03)										/**< Get Gyro macro Clock Select */
#define	Dd_Top_Set_CLKSEL12_GYROCLK(val)	(IO_CHIPTOP.CLKSEL12.bit.GYROCLK=(0x04|val))								/**< Set Gyro macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL12_FPT0CLK()		(IO_CHIPTOP.CLKSEL12.bit.FPT0CLK&0x07)										/**< Get FPT0 Clock Select */
#define	Dd_Top_Set_CLKSEL12_FPT0CLK(val)	(IO_CHIPTOP.CLKSEL12.bit.FPT0CLK=(0x08|val))								/**< Set FPT0 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL12_FPT1CLK()		(IO_CHIPTOP.CLKSEL12.bit.FPT1CLK&0x07)										/**< Get FPT1 Clock Select */
#define	Dd_Top_Set_CLKSEL12_FPT1CLK(val)	(IO_CHIPTOP.CLKSEL12.bit.FPT1CLK=(0x08|val))								/**< Set FPT1 Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL12_MECLK()			(IO_CHIPTOP.CLKSEL12.bit.MECLK&0x03)										/**< Get ME macro Clock Select */
#define	Dd_Top_Set_CLKSEL12_MECLK(val)		(IO_CHIPTOP.CLKSEL12.bit.MECLK=(0x04|val))									/**< Set ME macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL12_NFBCHCLK()		(IO_CHIPTOP.CLKSEL12.bit.NFBCHCLK&0x01)										/**< Get Nand Flash Bch macro Clock Select */
#define	Dd_Top_Set_CLKSEL12_NFBCHCLK(val)	(IO_CHIPTOP.CLKSEL12.bit.NFBCHCLK=(0x02|val))								/**< Set Nand Flash Bch macro Clock Select (Set write bit internally) */
#define	Dd_Top_Get_CLKSEL12_RIBCLK()		(IO_CHIPTOP.CLKSEL12.bit.RIBCLK&0x0F)										/**< Get RIBERY macro Clock Select */
#define	Dd_Top_Set_CLKSEL12_RIBCLK(val)		(IO_CHIPTOP.CLKSEL12.bit.RIBCLK=(0x10|val))									/**< Set RIBERY macro Clock Select (Set write bit internally) */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSEL12_SHDRCLK()		(IO_CHIPTOP.CLKSEL12.bit.SHDRCLK&0x07)										/**< Get 3DNR macro Clock Select */
#define	Dd_Top_Set_CLKSEL12_SHDRCLK(val)	(IO_CHIPTOP.CLKSEL12.bit.SHDRCLK=(0x08|val))								/**< Set 3DNR macro Clock Select (Set write bit internally) */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSEL12_SHDRCLK()		(IO_CHIPTOP.CLKSEL12.bit.SHDRCLK&0x0F)										/**< Get 3DNR macro Clock Select */
#define	Dd_Top_Set_CLKSEL12_SHDRCLK(val)	(IO_CHIPTOP.CLKSEL12.bit.SHDRCLK=(0x10|val))								/**< Set 3DNR macro Clock Select (Set write bit internally) */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---


// PLLCNT1
#define	Dd_Top_Get_PLLCNT1_PL00ST()			(IO_CHIPTOP.PLLCNT1.bit.PL00ST)												/**< Get DDR PLL 00 Stop */
#define	Dd_Top_Set_PLLCNT1_PL00ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL00ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 00 Stop */
#define	Dd_Top_Get_PLLCNT1_PL01ST()			(IO_CHIPTOP.PLLCNT1.bit.PL01ST)												/**< Get DDR PLL 01 Stop */
#define	Dd_Top_Set_PLLCNT1_PL01ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL01ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 01 Stop */
#define	Dd_Top_Get_PLLCNT1_PL02ST()			(IO_CHIPTOP.PLLCNT1.bit.PL02ST)												/**< Get DDR PLL 02 Stop */
#define	Dd_Top_Set_PLLCNT1_PL02ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL02ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 02 Stop */
#define	Dd_Top_Get_PLLCNT1_PL03ST()			(IO_CHIPTOP.PLLCNT1.bit.PL03ST)												/**< Get DDR PLL 03 Stop */
#define	Dd_Top_Set_PLLCNT1_PL03ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL03ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 03 Stop */
#define	Dd_Top_Get_PLLCNT1_PL04ST()			(IO_CHIPTOP.PLLCNT1.bit.PL04ST)												/**< Get DDR PLL 04 Stop */
#define	Dd_Top_Set_PLLCNT1_PL04ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL04ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 04 Stop */
#define	Dd_Top_Get_PLLCNT1_PL05ST()			(IO_CHIPTOP.PLLCNT1.bit.PL05ST)												/**< Get DDR PLL 05 Stop */
#define	Dd_Top_Set_PLLCNT1_PL05ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL05ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 05 Stop */
#define	Dd_Top_Get_PLLCNT1_PL05AST()		(IO_CHIPTOP.PLLCNT1.bit.PL05AST)											/**< Get DDR PLL 05A Stop */
#define	Dd_Top_Set_PLLCNT1_PL05AST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL05AST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 05A Stop */
#define	Dd_Top_Get_PLLCNT1_PL06ST()			(IO_CHIPTOP.PLLCNT1.bit.PL06ST)												/**< Get DDR PLL 06 Stop */
#define	Dd_Top_Set_PLLCNT1_PL06ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL06ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 06 Stop */
#define	Dd_Top_Get_PLLCNT1_PL07ST()			(IO_CHIPTOP.PLLCNT1.bit.PL07ST)												/**< Get DDR PLL 07 Stop */
#define	Dd_Top_Set_PLLCNT1_PL07ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL07ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 07 Stop */
#define	Dd_Top_Get_PLLCNT1_PL08ST()			(IO_CHIPTOP.PLLCNT1.bit.PL08ST)												/**< Get DDR PLL 08 Stop */
#define	Dd_Top_Set_PLLCNT1_PL08ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL08ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 08 Stop */
#define	Dd_Top_Get_PLLCNT1_PL10ST()			(IO_CHIPTOP.PLLCNT1.bit.PL10ST)												/**< Get DDR PLL 10 Stop */
#define	Dd_Top_Set_PLLCNT1_PL10ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL10ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 10 Stop */
#define	Dd_Top_Get_PLLCNT1_PL10AST()		(IO_CHIPTOP.PLLCNT1.bit.PL10AST)											/**< Get DDR PLL 10A Stop */
#define	Dd_Top_Set_PLLCNT1_PL10AST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL10AST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 10A Stop */
#define	Dd_Top_Get_PLLCNT1_PL11ST()			(IO_CHIPTOP.PLLCNT1.bit.PL11ST)												/**< Get DDR PLL 11 Stop */
#define	Dd_Top_Set_PLLCNT1_PL11ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.PL11ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 11 Stop */
#define	Dd_Top_Get_PLLCNT1_DPL00ST()		(IO_CHIPTOP.PLLCNT1.bit.DPL00ST)											/**< Get DPL 00 Stop */
#define	Dd_Top_Set_PLLCNT1_DPL00ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.DPL00ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DPL 00 Stop */
#define	Dd_Top_Get_PLLCNT1_DPL01ST()		(IO_CHIPTOP.PLLCNT1.bit.DPL01ST)											/**< Get DPL 01 Stop */
#define	Dd_Top_Set_PLLCNT1_DPL01ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.DPL01ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DPL 01 Stop */
#define	Dd_Top_Get_PLLCNT1_DPL02ST()		(IO_CHIPTOP.PLLCNT1.bit.DPL02ST)											/**< Get DPL 02 Stop */
#define	Dd_Top_Set_PLLCNT1_DPL02ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.DPL02ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DPL 02 Stop */
#define	Dd_Top_Get_PLLCNT1_DPL10ST()		(IO_CHIPTOP.PLLCNT1.bit.DPL10ST)											/**< Get DPL 10 Stop */
#define	Dd_Top_Set_PLLCNT1_DPL10ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.DPL10ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DPL 10 Stop */
#define	Dd_Top_Get_PLLCNT1_DPL11ST()		(IO_CHIPTOP.PLLCNT1.bit.DPL11ST)											/**< Get DPL 11 Stop */
#define	Dd_Top_Set_PLLCNT1_DPL11ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.DPL11ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DPL 11 Stop */
#define	Dd_Top_Get_PLLCNT1_DPL12ST()		(IO_CHIPTOP.PLLCNT1.bit.DPL12ST)											/**< Get DPL 12 Stop */
#define	Dd_Top_Set_PLLCNT1_DPL12ST(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT1.bit.DPL12ST=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DPL 12 Stop */
// PLLCNT2
#define	Dd_Top_Get_PLLCNT2_PL00SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL00SEL)											/**< Get DDR PLL 00 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL00SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL00SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 00 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL01SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL01SEL)											/**< Get DDR PLL 01 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL01SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL01SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 01 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL02SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL02SEL)											/**< Get DDR PLL 02 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL02SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL02SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 02 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL03SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL03SEL)											/**< Get DDR PLL 03 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL03SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL03SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 03 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL04SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL04SEL)											/**< Get DDR PLL 04 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL04SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL04SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 04 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL05SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL05SEL)											/**< Get DDR PLL 05 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL05SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL05SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 05 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL05ASEL()		(IO_CHIPTOP.PLLCNT2.bit.PL05ASEL)											/**< Get DDR PLL 05A Output Select */
#define	Dd_Top_Set_PLLCNT2_PL05ASEL(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL05ASEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 05A Output Select */
#define	Dd_Top_Get_PLLCNT2_PL06SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL06SEL)											/**< Get DDR PLL 06 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL06SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL06SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 06 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL07SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL07SEL)											/**< Get DDR PLL 07 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL07SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL07SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 07 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL08SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL08SEL)											/**< Get DDR PLL 08 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL08SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL08SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 08 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL10SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL10SEL)											/**< Get DDR PLL 10 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL10SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL10SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 10 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL10ASEL()		(IO_CHIPTOP.PLLCNT2.bit.PL10ASEL)											/**< Get DDR PLL 10A Output Select */
#define	Dd_Top_Set_PLLCNT2_PL10ASEL(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL10ASEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 10A Output Select */
#define	Dd_Top_Get_PLLCNT2_PL11SEL()		(IO_CHIPTOP.PLLCNT2.bit.PL11SEL)											/**< Get DDR PLL 11 Output Select */
#define	Dd_Top_Set_PLLCNT2_PL11SEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL11SEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 11 Output Select */
#define	Dd_Top_Get_PLLCNT2_PL01SSEN()		(IO_CHIPTOP.PLLCNT2.bit.PL01SSEN)											/**< Get DDR PLL 01 Spread spectrumt */
#define	Dd_Top_Set_PLLCNT2_PL01SSEN(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL01SSEN=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 01 Spread spectrumt */
#define	Dd_Top_Get_PLLCNT2_PL02SSEN()		(IO_CHIPTOP.PLLCNT2.bit.PL02SSEN)											/**< Get DDR PLL 02 Spread spectrumt */
#define	Dd_Top_Set_PLLCNT2_PL02SSEN(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT2.bit.PL02SSEN=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DDR PLL 02 Spread spectrumt */
// PLLCNT3
#define	Dd_Top_Get_PLLCNT3_P00POSTDIV()		(IO_CHIPTOP.PLLCNT3.bit.P00POSTDIV)											/**< Get PLL 00 back Clock Divider */
#define	Dd_Top_Set_PLLCNT3_P00POSTDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT3.bit.P00POSTDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 00 back Clock Divider */
#define	Dd_Top_Get_PLLCNT3_P00PREDIV()		(IO_CHIPTOP.PLLCNT3.bit.P00PREDIV)											/**< Get PLL 00 front Clock Divider */
#define	Dd_Top_Set_PLLCNT3_P00PREDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT3.bit.P00PREDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 00 front Clock Divider */
#define	Dd_Top_Get_PLLCNT3_P00PLLDIV()		(IO_CHIPTOP.PLLCNT3.bit.P00PLLDIV)											/**< Get PLL 00 Clock Divider */
#define	Dd_Top_Set_PLLCNT3_P00PLLDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT3.bit.P00PLLDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 00 Clock Divider */
// PLLCNT4
#define	Dd_Top_Get_PLLCNT4_P00FNUM()		(IO_CHIPTOP.PLLCNT4.bit.P00FNUM)											/**< Get PLL 00 divider fractional numerator ratio */
#define	Dd_Top_Set_PLLCNT4_P00FNUM(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT4.bit.P00FNUM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 00 divider fractional numerator ratio */
// PLLCNT5
#define	Dd_Top_Get_PLLCNT5_P00FDEN()		(IO_CHIPTOP.PLLCNT5.bit.P00FDEN)											/**< Get PLL 00 divider fractional denominator ratio */
#define	Dd_Top_Set_PLLCNT5_P00FDEN(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT5.bit.P00FDEN=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 00 divider fractional denominator ratio */
// PLLCNT6
#define	Dd_Top_Get_PLLCNT6_P01RATE()		(IO_CHIPTOP.PLLCNT6.bit.P01RATE)											/**< Get PLL 01 SSC  */
#define	Dd_Top_Set_PLLCNT6_P01RATE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT6.bit.P01RATE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 01 SSC modulation rate */
#define	Dd_Top_Get_PLLCNT6_P01FREQ()		(IO_CHIPTOP.PLLCNT6.bit.P01FREQ)											/**< Get PLL 01 SSC modulation frequency */
#define	Dd_Top_Set_PLLCNT6_P01FREQ(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT6.bit.P01FREQ=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 01 SSC modulation frequency */
#define	Dd_Top_Get_PLLCNT6_P01MODE()		(IO_CHIPTOP.PLLCNT6.bit.P01MODE)											/**< Get PLL 01 SSC modulation mode */
#define	Dd_Top_Set_PLLCNT6_P01MODE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT6.bit.P01MODE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 01 SSC modulation mode */
#define	Dd_Top_Get_PLLCNT6_P02RATE()		(IO_CHIPTOP.PLLCNT6.bit.P02RATE)											/**< Get PLL 02 SSC modulation rate */
#define	Dd_Top_Set_PLLCNT6_P02RATE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT6.bit.P02RATE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 02 SSC modulation rate */
#define	Dd_Top_Get_PLLCNT6_P02FREQ()		(IO_CHIPTOP.PLLCNT6.bit.P02FREQ)											/**< Get PLL 02 SSC modulation frequency */
#define	Dd_Top_Set_PLLCNT6_P02FREQ(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT6.bit.P02FREQ=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 02 SSC modulation frequency */
#define	Dd_Top_Get_PLLCNT6_P02MODE()		(IO_CHIPTOP.PLLCNT6.bit.P02MODE)											/**< Get PLL 02 SSC modulation mode */
#define	Dd_Top_Set_PLLCNT6_P02MODE(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT6.bit.P02MODE=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 02 SSC modulation mode */
// PLLCNT7
#define	Dd_Top_Get_PLLCNT7_P03POSTDIV0()	(IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV0)										/**< Get PLL 03 back Clock Divider */
#define	Dd_Top_Set_PLLCNT7_P03POSTDIV0(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV0=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 03 back Clock Divider */
#define	Dd_Top_Get_PLLCNT7_P03POSTDIV1()	(IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV1)										/**< Get PLL 03 back Clock Divider */
#define	Dd_Top_Set_PLLCNT7_P03POSTDIV1(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV1=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 03 back Clock Divider */
#define	Dd_Top_Get_PLLCNT7_P03PREDIV()		(IO_CHIPTOP.PLLCNT7.bit.P03PREDIV)											/**< Get PLL 03 front Clock Divider */
#define	Dd_Top_Set_PLLCNT7_P03PREDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.P03PREDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 03 front Clock Divider */
#define	Dd_Top_Get_PLLCNT7_P03PLLDIV()		(IO_CHIPTOP.PLLCNT7.bit.P03PLLDIV)											/**< Get PLL 03 Clock Divider */
#define	Dd_Top_Set_PLLCNT7_P03PLLDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.P03PLLDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 03 Clock Divider */
#define	Dd_Top_Get_PLLCNT7_P03OC0()			(IO_CHIPTOP.PLLCNT7.bit.P03OC0)												/**< Get PLL 03 Clock Disable */
#define	Dd_Top_Set_PLLCNT7_P03OC0(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.P03OC0=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 03 Clock Disable */
#define	Dd_Top_Get_PLLCNT7_P03OC1()			(IO_CHIPTOP.PLLCNT7.bit.P03OC1)												/**< Get PLL 03 Clock Disable */
#define	Dd_Top_Set_PLLCNT7_P03OC1(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.P03OC1=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 03 Clock Disable */
#define	Dd_Top_Get_PLLCNT7_D0XOC()			(IO_CHIPTOP.PLLCNT7.bit.D0XOC)												/**< Get DPL0 Clock Disable */
#define	Dd_Top_Set_PLLCNT7_D0XOC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.D0XOC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DPL0 Clock Disable */
#define	Dd_Top_Get_PLLCNT7_D1XOC()			(IO_CHIPTOP.PLLCNT7.bit.D1XOC)												/**< Get DPL1 Clock Disable */
#define	Dd_Top_Set_PLLCNT7_D1XOC(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.D1XOC=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set DPL1 Clock Disable */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	Dd_Top_Get_PLLCNT7_DPLX8()			(IO_CHIPTOP.PLLCNT7.bit.DPLX8)												/**< Get multiplication rate of DDR subsequent stage PLL */
#define	Dd_Top_Set_PLLCNT7_DPLX8(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.DPLX8=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set multiplication rate of DDR subsequent stage PLL */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	Dd_Top_Get_PLLCNT7_DSEL()			(IO_CHIPTOP.PLLCNT7.bit.DSEL)												/**< Get CLK path of the PLL */
#define	Dd_Top_Set_PLLCNT7_DSEL(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT7.bit.DSEL=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set CLK path of the PLL */
// PLLCNT8
#define	Dd_Top_Get_PLLCNT8_P04POSTDIV()		(IO_CHIPTOP.PLLCNT8.bit.P04POSTDIV)											/**< Get PLL 04 back Clock Divider */
#define	Dd_Top_Set_PLLCNT8_P04POSTDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT8.bit.P04POSTDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 04 back Clock Divider */
#define	Dd_Top_Get_PLLCNT8_P04PREDIV()		(IO_CHIPTOP.PLLCNT8.bit.P04PREDIV)											/**< Get PLL 04 front Clock Divider */
#define	Dd_Top_Set_PLLCNT8_P04PREDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT8.bit.P04PREDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 04 front Clock Divider */
#define	Dd_Top_Get_PLLCNT8_P04PLLDIV()		(IO_CHIPTOP.PLLCNT8.bit.P04PLLDIV)											/**< Get PLL 04 Clock Divider */
#define	Dd_Top_Set_PLLCNT8_P04PLLDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT8.bit.P04PLLDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 04 Clock Divider */
// PLLCNT9
#define	Dd_Top_Get_PLLCNT9_P10APLLDIV()		(IO_CHIPTOP.PLLCNT9.bit.P10APLLDIV)											/**< Get PLL 10 Clock Divider */
#define	Dd_Top_Set_PLLCNT9_P10APLLDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT9.bit.P10APLLDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 10 Clock Divider */
#define	Dd_Top_Get_PLLCNT9_P08PLLDIV()		(IO_CHIPTOP.PLLCNT9.bit.P08PLLDIV)											/**< Get PLL 08 Clock Divider */
#define	Dd_Top_Set_PLLCNT9_P08PLLDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLCNT9.bit.P08PLLDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 08 Clock Divider */
#define	Dd_Top_Get_PLLCNT9_P05CHG()			(IO_CHIPTOP.PLLCNT9.bit.P05CHG)												/**< Get PLL 05 frefrequency generated */
#define	Dd_Top_Set_PLLCNT9_P05CHG(val)		{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
                                                IO_CHIPTOP.PLLCNT9.bit.P05CHG=(val); \
                                                Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}						/**< Set PLL 05 frequency generated */

// CLKSTOP
#define	Dd_Top_Get_CLKSTOP1()				(IO_CHIPTOP.CLKSTOP1.word)														/**< Get Clock Stop 1 */
#define	Dd_Top_Get_CLKSTOP2()				(IO_CHIPTOP.CLKSTOP2.word)														/**< Get Clock Stop 2 */
#define	Dd_Top_Get_CLKSTOP3()				(IO_CHIPTOP.CLKSTOP3.word)														/**< Get Clock Stop 3 */
#define	Dd_Top_Get_CLKSTOP4()				(IO_CHIPTOP.CLKSTOP4.word)														/**< Get Clock Stop 4 */
#define	Dd_Top_Get_CLKSTOP5()				(IO_CHIPTOP.CLKSTOP5.word)														/**< Get Clock Stop 5 */
#define	Dd_Top_Get_CLKSTOP6()				(IO_CHIPTOP.CLKSTOP6.word)														/**< Get Clock Stop 6 */
#define	Dd_Top_Get_CLKSTOP7()				(IO_CHIPTOP.CLKSTOP7.word)														/**< Get Clock Stop 7 */
#define	Dd_Top_Get_CLKSTOP8()				(IO_CHIPTOP.CLKSTOP8.word)														/**< Get Clock Stop 8 */
#define	Dd_Top_Get_CLKSTOP9()				(IO_CHIPTOP.CLKSTOP9.word)														/**< Get Clock Stop 9 */
#define	Dd_Top_Get_CLKSTOP10()				(IO_CHIPTOP.CLKSTOP10.word)														/**< Get Clock Stop 10 */
#define	Dd_Top_Get_CLKSTOP11()				(IO_CHIPTOP.CLKSTOP11.word)														/**< Get Clock Stop 11 */
#define	Dd_Top_Get_CLKSTOP12()				(IO_CHIPTOP.CLKSTOP12.word)														/**< Get Clock Stop 12 */
#define	Dd_Top_Get_CLKSTOP13()				(IO_CHIPTOP.CLKSTOP13.word)														/**< Get Clock Stop 13 */
#define	Dd_Top_Get_CLKSTOP14()				(IO_CHIPTOP.CLKSTOP14.word)														/**< Get Clock Stop 14 */

// CLKSTOP1
#define	Dd_Top_Get_CLKSTOP1_DSPCK()				(IO_CHIPTOP.CLKSTOP1.bit.DSPCK)												/**< Get DSP Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_DSPCK(val)			(IO_CHIPTOP.CLKSTOP1.bit.DSPCK=(0x02|val))									/**< Set DSP Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_DSPAX()				(IO_CHIPTOP.CLKSTOP1.bit.DSPAX)												/**< Get EXS AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_DSPAX(val)			(IO_CHIPTOP.CLKSTOP1.bit.DSPAX=(0x02|val))									/**< Set EXS AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_SENCK()				(IO_CHIPTOP.CLKSTOP1.bit.SENCK)												/**< Get SENSOR Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_SENCK(val)			(IO_CHIPTOP.CLKSTOP1.bit.SENCK=(0x02|val))									/**< Set SENSOR Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_SENAX()				(IO_CHIPTOP.CLKSTOP1.bit.SENAX)												/**< Get SENSOR AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_SENAX(val)			(IO_CHIPTOP.CLKSTOP1.bit.SENAX=(0x02|val))									/**< Set SENSOR AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_SENAH()				(IO_CHIPTOP.CLKSTOP1.bit.SENAH)												/**< Get SENSOR AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_SENAH(val)			(IO_CHIPTOP.CLKSTOP1.bit.SENAH=(0x02|val))									/**< Set SENSOR AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_SENAP()				(IO_CHIPTOP.CLKSTOP1.bit.SENAP)												/**< Get SENSOR APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_SENAP(val)			(IO_CHIPTOP.CLKSTOP1.bit.SENAP=(0x02|val))									/**< Set SENSOR APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_GPIOAP()			(IO_CHIPTOP.CLKSTOP1.bit.GPIOAP)											/**< Get GPIO APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_GPIOAP(val)			(IO_CHIPTOP.CLKSTOP1.bit.GPIOAP=(0x02|val))									/**< Set GPIO APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_AU0CK()				(IO_CHIPTOP.CLKSTOP1.bit.AU0CK)												/**< Get Audio 0ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_AU0CK(val)			(IO_CHIPTOP.CLKSTOP1.bit.AU0CK=(0x02|val))									/**< Set Audio 0ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_AU2CK()				(IO_CHIPTOP.CLKSTOP1.bit.AU2CK)												/**< Get Audio 2ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_AU2CK(val)			(IO_CHIPTOP.CLKSTOP1.bit.AU2CK=(0x02|val))									/**< Set Audio 2ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_AU3CK()				(IO_CHIPTOP.CLKSTOP1.bit.AU3CK)												/**< Get Audio 3ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_AU3CK(val)			(IO_CHIPTOP.CLKSTOP1.bit.AU3CK=(0x02|val))									/**< Set Audio 3ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_AU4CK()				(IO_CHIPTOP.CLKSTOP1.bit.AU4CK)												/**< Get Audio 4ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_AU4CK(val)			(IO_CHIPTOP.CLKSTOP1.bit.AU4CK=(0x02|val))									/**< Set Audio 4ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_AU5CK()				(IO_CHIPTOP.CLKSTOP1.bit.AU5CK)												/**< Get Audio 5ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_AU5CK(val)			(IO_CHIPTOP.CLKSTOP1.bit.AU5CK=(0x02|val))									/**< Set Audio 5ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_NETAUCK()			(IO_CHIPTOP.CLKSTOP1.bit.NETAUCK)											/**< Get NETSEC Audio Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_NETAUCK(val)		(IO_CHIPTOP.CLKSTOP1.bit.NETAUCK=(0x02|val))								/**< Set NETSEC Audio Clock Stop */
#define	Dd_Top_Get_CLKSTOP1_TEMPCK()			(IO_CHIPTOP.CLKSTOP1.bit.TEMPCK)											/**< Get Temperature Sensor Clock Stop */
#define	Dd_Top_Set_CLKSTOP1_TEMPCK(val)			(IO_CHIPTOP.CLKSTOP1.bit.TEMPCK=(0x02|val))									/**< Set Temperature Sensor Clock Stop */
// CLKSTOP2
#define	Dd_Top_Get_CLKSTOP2_RCK()				(IO_CHIPTOP.CLKSTOP2.bit.RCK)												/**< Get PERI RCK Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_RCK(val)			(IO_CHIPTOP.CLKSTOP2.bit.RCK=(0x02|val))									/**< Set PERI RCK Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_UHS1CK0()			(IO_CHIPTOP.CLKSTOP2.bit.UHS1CK0)											/**< Get UHS1 ch0 Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_UHS1CK0(val)		(IO_CHIPTOP.CLKSTOP2.bit.UHS1CK0=(0x02|val))								/**< Set UHS1 ch0 Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_UHS1CK1()			(IO_CHIPTOP.CLKSTOP2.bit.UHS1CK1)											/**< Get UHS1 ch1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_UHS1CK1(val)		(IO_CHIPTOP.CLKSTOP2.bit.UHS1CK1=(0x02|val))								/**< Set UHS1 ch1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_UHS1CK2()			(IO_CHIPTOP.CLKSTOP2.bit.UHS1CK2)											/**< Get UHS1 ch2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_UHS1CK2(val)		(IO_CHIPTOP.CLKSTOP2.bit.UHS1CK2=(0x02|val))								/**< Set UHS1 ch2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_UHS2CK()			(IO_CHIPTOP.CLKSTOP2.bit.UHS2CK)											/**< Get UHS2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_UHS2CK(val)			(IO_CHIPTOP.CLKSTOP2.bit.UHS2CK=(0x02|val))									/**< Set UHS2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_NFCK()				(IO_CHIPTOP.CLKSTOP2.bit.NFCK)												/**< Get Nand Flash Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_NFCK(val)			(IO_CHIPTOP.CLKSTOP2.bit.NFCK=(0x02|val))									/**< Set Nand Flash Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_EMMCCK()			(IO_CHIPTOP.CLKSTOP2.bit.EMMCCK)											/**< Get eMMC Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_EMMCCK(val)			(IO_CHIPTOP.CLKSTOP2.bit.EMMCCK=(0x02|val))									/**< Set eMMC Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_NETSECCK()			(IO_CHIPTOP.CLKSTOP2.bit.NETSECCK)											/**< Get NETSEC Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_NETSECCK(val)		(IO_CHIPTOP.CLKSTOP2.bit.NETSECCK=(0x02|val))								/**< Set NETSEC Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_NETRCK()			(IO_CHIPTOP.CLKSTOP2.bit.NETRCK)											/**< Get NETSEC RCK Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_NETRCK(val)			(IO_CHIPTOP.CLKSTOP2.bit.NETRCK=(0x02|val))									/**< Set NETSEC RCK Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_EXSAX()				(IO_CHIPTOP.CLKSTOP2.bit.EXSAX)												/**< Get EXS AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_EXSAX(val)			(IO_CHIPTOP.CLKSTOP2.bit.EXSAX=(0x02|val))									/**< Set EXS AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_SPICK()				(IO_CHIPTOP.CLKSTOP2.bit.SPICK)												/**< Get SPI Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_SPICK(val)			(IO_CHIPTOP.CLKSTOP2.bit.SPICK=(0x02|val))									/**< Set SPI Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_SLIMB00CK()			(IO_CHIPTOP.CLKSTOP2.bit.SLIMB00CK)											/**< Get SLIMbus 00ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_SLIMB00CK(val)		(IO_CHIPTOP.CLKSTOP2.bit.SLIMB00CK=(0x02|val))								/**< Set SLIMbus 00ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_SLIMB01CK()			(IO_CHIPTOP.CLKSTOP2.bit.SLIMB01CK)											/**< Get SLIMbus 01ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_SLIMB01CK(val)		(IO_CHIPTOP.CLKSTOP2.bit.SLIMB01CK=(0x02|val))								/**< Set SLIMbus 01ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_SLIMB10CK()			(IO_CHIPTOP.CLKSTOP2.bit.SLIMB10CK)											/**< Get SLIMbus 10ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_SLIMB10CK(val)		(IO_CHIPTOP.CLKSTOP2.bit.SLIMB10CK=(0x02|val))								/**< Set SLIMbus 10ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_SLIMB11CK()			(IO_CHIPTOP.CLKSTOP2.bit.SLIMB11CK)											/**< Get SLIMbus 11ch Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_SLIMB11CK(val)		(IO_CHIPTOP.CLKSTOP2.bit.SLIMB11CK=(0x02|val))								/**< Set SLIMbus 11ch Clock Stop */
#define	Dd_Top_Get_CLKSTOP2_PCISUPPCK()			(IO_CHIPTOP.CLKSTOP2.bit.PCISUPPCK)											/**< Get PCI sub Clock Stop */
#define	Dd_Top_Set_CLKSTOP2_PCISUPPCK(val)		(IO_CHIPTOP.CLKSTOP2.bit.PCISUPPCK=(0x02|val))								/**< Set PCI sub Clock Stop */
// CLKSTOP3
#define	Dd_Top_Get_CLKSTOP3_IIPCK()				(IO_CHIPTOP.CLKSTOP3.bit.IIPCK)												/**< Get IIP Macro Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_IIPCK(val)			(IO_CHIPTOP.CLKSTOP3.bit.IIPCK=(0x02|val))									/**< Set IIP Macro Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_IIPAP()				(IO_CHIPTOP.CLKSTOP3.bit.IIPAP)												/**< Get IIP Macro APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_IIPAP(val)			(IO_CHIPTOP.CLKSTOP3.bit.IIPAP=(0x02|val))									/**< Set IIP Macro APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_IIPAH()				(IO_CHIPTOP.CLKSTOP3.bit.IIPAH)												/**< Get IIP Macro AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_IIPAH(val)			(IO_CHIPTOP.CLKSTOP3.bit.IIPAH=(0x02|val))									/**< Set IIP Macro AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_IIPAX()				(IO_CHIPTOP.CLKSTOP3.bit.IIPAX)												/**< Get IIP Macro AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_IIPAX(val)			(IO_CHIPTOP.CLKSTOP3.bit.IIPAX=(0x02|val))									/**< Set IIP Macro AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_LCDCK()				(IO_CHIPTOP.CLKSTOP3.bit.LCDCK)												/**< Get DISP LCD Macro Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_LCDCK(val)			(IO_CHIPTOP.CLKSTOP3.bit.LCDCK=(0x02|val))									/**< Set DISP LCD Macro Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_HIFCK()				(IO_CHIPTOP.CLKSTOP3.bit.HIFCK)												/**< Get DISP HIF Macro Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_HIFCK(val)			(IO_CHIPTOP.CLKSTOP3.bit.HIFCK=(0x02|val))									/**< Set DISP HIF Macro Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_MIFCK()				(IO_CHIPTOP.CLKSTOP3.bit.MIFCK)												/**< Get DISP MIF Macro Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_MIFCK(val)			(IO_CHIPTOP.CLKSTOP3.bit.MIFCK=(0x02|val))									/**< Set DISP MIF Macro Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_DISPAP()			(IO_CHIPTOP.CLKSTOP3.bit.DISPAP)											/**< Get DISP Macro APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_DISPAP(val)			(IO_CHIPTOP.CLKSTOP3.bit.DISPAP=(0x02|val))									/**< Set DISP Macro APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_DISPAH()			(IO_CHIPTOP.CLKSTOP3.bit.DISPAH)											/**< Get DISP Macro AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_DISPAH(val)			(IO_CHIPTOP.CLKSTOP3.bit.DISPAH=(0x02|val))									/**< Set DISP Macro AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_DISPAX()			(IO_CHIPTOP.CLKSTOP3.bit.DISPAX)											/**< Get DISP Macro AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_DISPAX(val)			(IO_CHIPTOP.CLKSTOP3.bit.DISPAX=(0x02|val))									/**< Set DISP Macro AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_JPGCK()				(IO_CHIPTOP.CLKSTOP3.bit.JPGCK)												/**< Get JPEG Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_JPGCK(val)			(IO_CHIPTOP.CLKSTOP3.bit.JPGCK=(0x02|val))									/**< Set JPEG Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_JPGAX()				(IO_CHIPTOP.CLKSTOP3.bit.JPGAX)												/**< Get JPEG AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_JPGAX(val)			(IO_CHIPTOP.CLKSTOP3.bit.JPGAX=(0x02|val))									/**< Set JPEG AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_JPGAH()				(IO_CHIPTOP.CLKSTOP3.bit.JPGAH)												/**< Get JPEG AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_JPGAH(val)			(IO_CHIPTOP.CLKSTOP3.bit.JPGAH=(0x02|val))									/**< Set JPEG AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_JPGAP()				(IO_CHIPTOP.CLKSTOP3.bit.JPGAP)												/**< Get JPEG APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_JPGAP(val)			(IO_CHIPTOP.CLKSTOP3.bit.JPGAP=(0x02|val))									/**< Set JPEG APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_PDM0CK()			(IO_CHIPTOP.CLKSTOP3.bit.PDM0CK)											/**< Get PDM0 Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_PDM0CK(val)			(IO_CHIPTOP.CLKSTOP3.bit.PDM0CK=(0x02|val))									/**< Set PDM0 Clock Stop */
#define	Dd_Top_Get_CLKSTOP3_PDM1CK()			(IO_CHIPTOP.CLKSTOP3.bit.PDM1CK)											/**< Get PDM1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP3_PDM1CK(val)			(IO_CHIPTOP.CLKSTOP3.bit.PDM1CK=(0x02|val))									/**< Set PDM1 Clock Stop */
// CLKSTOP4
#define	Dd_Top_Get_CLKSTOP4_GPUCK()				(IO_CHIPTOP.CLKSTOP4.bit.GPUCK)												/**< Get MXIC Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_GPUCK(val)			(IO_CHIPTOP.CLKSTOP4.bit.GPUCK=(0x02|val))									/**< Set MXIC Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_GPUAP()				(IO_CHIPTOP.CLKSTOP4.bit.GPUAP)												/**< Get MXIC APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_GPUAP(val)			(IO_CHIPTOP.CLKSTOP4.bit.GPUAP=(0x02|val))									/**< Set MXIC APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_GPUAH()				(IO_CHIPTOP.CLKSTOP4.bit.GPUAH)												/**< Get MXIC AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_GPUAH(val)			(IO_CHIPTOP.CLKSTOP4.bit.GPUAH=(0x02|val))									/**< Set MXIC AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_GPUAX()				(IO_CHIPTOP.CLKSTOP4.bit.GPUAX)												/**< Get MXIC AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_GPUAX(val)			(IO_CHIPTOP.CLKSTOP4.bit.GPUAX=(0x02|val))									/**< Set MXIC AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_FPT0CK()			(IO_CHIPTOP.CLKSTOP4.bit.FPT0CK)											/**< Get FPT0 Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_FPT0CK(val)			(IO_CHIPTOP.CLKSTOP4.bit.FPT0CK=(0x02|val))									/**< Set FPT0 Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_FPT0AX()			(IO_CHIPTOP.CLKSTOP4.bit.FPT0AX)											/**< Get FPT0 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_FPT0AX(val)			(IO_CHIPTOP.CLKSTOP4.bit.FPT0AX=(0x02|val))									/**< Set FPT0 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_FPT0AH()			(IO_CHIPTOP.CLKSTOP4.bit.FPT0AH)											/**< Get FPT0 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_FPT0AH(val)			(IO_CHIPTOP.CLKSTOP4.bit.FPT0AH=(0x02|val))									/**< Set FPT0 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_FPT0AP()			(IO_CHIPTOP.CLKSTOP4.bit.FPT0AP)											/**< Get FPT0 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_FPT0AP(val)			(IO_CHIPTOP.CLKSTOP4.bit.FPT0AP=(0x02|val))									/**< Set FPT0 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_FPT1CK()			(IO_CHIPTOP.CLKSTOP4.bit.FPT1CK)											/**< Get FPT1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_FPT1CK(val)			(IO_CHIPTOP.CLKSTOP4.bit.FPT1CK=(0x02|val))									/**< Set FPT1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_FPT1AP()			(IO_CHIPTOP.CLKSTOP4.bit.FPT1AP)											/**< Get FPT1 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_FPT1AP(val)			(IO_CHIPTOP.CLKSTOP4.bit.FPT1AP=(0x02|val))									/**< Set FPT1 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_FPT1AH()			(IO_CHIPTOP.CLKSTOP4.bit.FPT1AH)											/**< Get FPT1 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_FPT1AH(val)			(IO_CHIPTOP.CLKSTOP4.bit.FPT1AH=(0x02|val))									/**< Set FPT1 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_FPT1AX()			(IO_CHIPTOP.CLKSTOP4.bit.FPT1AX)											/**< Get FPT1 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_FPT1AX(val)			(IO_CHIPTOP.CLKSTOP4.bit.FPT1AX=(0x02|val))									/**< Set FPT1 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_APCK0()				(IO_CHIPTOP.CLKSTOP4.bit.APCK0)												/**< Get SNAPCLKO0 Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_APCK0(val)			(IO_CHIPTOP.CLKSTOP4.bit.APCK0=(0x02|val))									/**< Set SNAPCLKO0 Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_APCK1()				(IO_CHIPTOP.CLKSTOP4.bit.APCK1)												/**< Get SNAPCLKO1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_APCK1(val)			(IO_CHIPTOP.CLKSTOP4.bit.APCK1=(0x02|val))									/**< Set SNAPCLKO1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_APCK2()				(IO_CHIPTOP.CLKSTOP4.bit.APCK2)												/**< Get SNAPCLKO2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_APCK2(val)			(IO_CHIPTOP.CLKSTOP4.bit.APCK2=(0x02|val))									/**< Set SNAPCLKO2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP4_APCK3()				(IO_CHIPTOP.CLKSTOP4.bit.APCK3)												/**< Get SNAPCLKO3 Clock Stop */
#define	Dd_Top_Set_CLKSTOP4_APCK3(val)			(IO_CHIPTOP.CLKSTOP4.bit.APCK3=(0x02|val))									/**< Set SNAPCLKO3 Clock Stop */
// CLKSTOP5
#define	Dd_Top_Get_CLKSTOP5_MICAX0()			(IO_CHIPTOP.CLKSTOP5.bit.MICAX0)											/**< Get MXIC AXI Bus Clock0 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAX0(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAX0=(0x02|val))									/**< Set MXIC AXI Bus Clock0 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAX1()			(IO_CHIPTOP.CLKSTOP5.bit.MICAX1)											/**< Get MXIC AXI Bus Clock1 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAX1(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAX1=(0x02|val))									/**< Set MXIC AXI Bus Clock1 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAX2()			(IO_CHIPTOP.CLKSTOP5.bit.MICAX2)											/**< Get MXIC AXI Bus Clock2 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAX2(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAX2=(0x02|val))									/**< Set MXIC AXI Bus Clock2 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAX3()			(IO_CHIPTOP.CLKSTOP5.bit.MICAX3)											/**< Get MXIC AXI Bus Clock3 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAX3(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAX3=(0x02|val))									/**< Set MXIC AXI Bus Clock3 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAX4()			(IO_CHIPTOP.CLKSTOP5.bit.MICAX4)											/**< Get MXIC AXI Bus Clock4 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAX4(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAX4=(0x02|val))									/**< Set MXIC AXI Bus Clock4 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAX5()			(IO_CHIPTOP.CLKSTOP5.bit.MICAX5)											/**< Get MXIC AXI Bus Clock5 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAX5(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAX5=(0x02|val))									/**< Set MXIC AXI Bus Clock5 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAX6()			(IO_CHIPTOP.CLKSTOP5.bit.MICAX6)											/**< Get MXIC AXI Bus Clock6 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAX6(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAX6=(0x02|val))									/**< Set MXIC AXI Bus Clock6 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAP0()			(IO_CHIPTOP.CLKSTOP5.bit.MICAP0)											/**< Get MXIC APB Bus Clock0 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAP0(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAP0=(0x02|val))									/**< Set MXIC APB Bus Clock0 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAP1()			(IO_CHIPTOP.CLKSTOP5.bit.MICAP1)											/**< Get MXIC APB Bus Clock1 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAP1(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAP1=(0x02|val))									/**< Set MXIC APB Bus Clock1 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAP2()			(IO_CHIPTOP.CLKSTOP5.bit.MICAP2)											/**< Get MXIC APB Bus Clock2 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAP2(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAP2=(0x02|val))									/**< Set MXIC APB Bus Clock2 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAP3()			(IO_CHIPTOP.CLKSTOP5.bit.MICAP3)											/**< Get MXIC APB Bus Clock3 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAP3(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAP3=(0x02|val))									/**< Set MXIC APB Bus Clock3 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAP4()			(IO_CHIPTOP.CLKSTOP5.bit.MICAP4)											/**< Get MXIC APB Bus Clock4 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAP4(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAP4=(0x02|val))									/**< Set MXIC APB Bus Clock4 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAP5()			(IO_CHIPTOP.CLKSTOP5.bit.MICAP5)											/**< Get MXIC APB Bus Clock5 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAP5(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAP5=(0x02|val))									/**< Set MXIC APB Bus Clock5 Stop */
#define	Dd_Top_Get_CLKSTOP5_MICAP6()			(IO_CHIPTOP.CLKSTOP5.bit.MICAP6)											/**< Get MXIC APB Bus Clock6 Stop */
#define	Dd_Top_Set_CLKSTOP5_MICAP6(val)			(IO_CHIPTOP.CLKSTOP5.bit.MICAP6=(0x02|val))									/**< Set MXIC APB Bus Clock6 Stop */
// CLKSTOP6
#define	Dd_Top_Get_CLKSTOP6_MICAH1()			(IO_CHIPTOP.CLKSTOP6.bit.MICAH1)											/**< Get MXIC AHB Bus Clock1 Stop */
#define	Dd_Top_Set_CLKSTOP6_MICAH1(val)			(IO_CHIPTOP.CLKSTOP6.bit.MICAH1=(0x02|val))									/**< Set MXIC AHB Bus Clock1 Stop */
#define	Dd_Top_Get_CLKSTOP6_MICAH2()			(IO_CHIPTOP.CLKSTOP6.bit.MICAH2)											/**< Get MXIC AHB Bus Clock2 Stop */
#define	Dd_Top_Set_CLKSTOP6_MICAH2(val)			(IO_CHIPTOP.CLKSTOP6.bit.MICAH2=(0x02|val))									/**< Set MXIC AHB Bus Clock2 Stop */
#define	Dd_Top_Get_CLKSTOP6_MICAH3()			(IO_CHIPTOP.CLKSTOP6.bit.MICAH3)											/**< Get MXIC AHB Bus Clock3 Stop */
#define	Dd_Top_Set_CLKSTOP6_MICAH3(val)			(IO_CHIPTOP.CLKSTOP6.bit.MICAH3=(0x02|val))									/**< Set MXIC AHB Bus Clock2 Stop */
#define	Dd_Top_Get_CLKSTOP6_IMGAX()				(IO_CHIPTOP.CLKSTOP6.bit.IMGAX)												/**< Get Image macro AXI Bus Clock0 Stop */
#define	Dd_Top_Set_CLKSTOP6_IMGAX(val)			(IO_CHIPTOP.CLKSTOP6.bit.IMGAX=(0x02|val))									/**< Set Image macro AXI Bus Clock0 Stop */
#define	Dd_Top_Get_CLKSTOP6_IMGAH0()			(IO_CHIPTOP.CLKSTOP6.bit.IMGAH0)											/**< Get Image macro AHB Bus Clock0 Stop */
#define	Dd_Top_Set_CLKSTOP6_IMGAH0(val)			(IO_CHIPTOP.CLKSTOP6.bit.IMGAH0=(0x02|val))									/**< Set Image macro AHB Bus Clock0 Stop */
#define	Dd_Top_Get_CLKSTOP6_IMGAH1()			(IO_CHIPTOP.CLKSTOP6.bit.IMGAH1)											/**< Get Image macro AHB Bus Clock1 Stop */
#define	Dd_Top_Set_CLKSTOP6_IMGAH1(val)			(IO_CHIPTOP.CLKSTOP6.bit.IMGAH1=(0x02|val))									/**< Set Image macro AHB Bus Clock1 Stop */
#define	Dd_Top_Get_CLKSTOP6_IMGAH3()			(IO_CHIPTOP.CLKSTOP6.bit.IMGAH3)											/**< Get Image macro AHB Bus Clock3 Stop */
#define	Dd_Top_Set_CLKSTOP6_IMGAH3(val)			(IO_CHIPTOP.CLKSTOP6.bit.IMGAH3=(0x02|val))									/**< Set Image macro AHB Bus Clock3 Stop */
#define	Dd_Top_Get_CLKSTOP6_IMGAP3()			(IO_CHIPTOP.CLKSTOP6.bit.IMGAP3)											/**< Get Image macro APB Bus Clock3 Stop */
#define	Dd_Top_Set_CLKSTOP6_IMGAP3(val)			(IO_CHIPTOP.CLKSTOP6.bit.IMGAP3=(0x02|val))									/**< Set Image macro APB Bus Clock3 Stop */
#define	Dd_Top_Get_CLKSTOP6_REGAP()				(IO_CHIPTOP.CLKSTOP6.bit.REGAP)												/**< Get Chip top Register APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP6_REGAP(val)			(IO_CHIPTOP.CLKSTOP6.bit.REGAP=(0x02|val))									/**< Set Chip top Register APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP6_XCHAX()				(IO_CHIPTOP.CLKSTOP6.bit.XCHAX)												/**< Get XCH AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP6_XCHAX(val)			(IO_CHIPTOP.CLKSTOP6.bit.XCHAX=(0x02|val))									/**< Set XCH AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP6_XCHAP()				(IO_CHIPTOP.CLKSTOP6.bit.XCHAP)												/**< Get XCH APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP6_XCHAP(val)			(IO_CHIPTOP.CLKSTOP6.bit.XCHAP=(0x02|val))									/**< Set XCH APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP6_ELACK()				(IO_CHIPTOP.CLKSTOP6.bit.ELACK)												/**< Get ELA Clock Stop */
#define	Dd_Top_Set_CLKSTOP6_ELACK(val)			(IO_CHIPTOP.CLKSTOP6.bit.ELACK=(0x02|val))									/**< Set ELA Clock Stop */
#define	Dd_Top_Get_CLKSTOP6_ELAAX()				(IO_CHIPTOP.CLKSTOP6.bit.ELAAX)												/**< Get ELA AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP6_ELAAX(val)			(IO_CHIPTOP.CLKSTOP6.bit.ELAAX=(0x02|val))									/**< Set ELA AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP6_ELAAP()				(IO_CHIPTOP.CLKSTOP6.bit.ELAAP)												/**< Get ELA APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP6_ELAAP(val)			(IO_CHIPTOP.CLKSTOP6.bit.ELAAP=(0x02|val))									/**< Set ELA APB Bus Clock Stop */
// CLKSTOP7
#define	Dd_Top_Get_CLKSTOP7_IPUFDCK()			(IO_CHIPTOP.CLKSTOP7.bit.IPUFDCK)											/**< Get IPUFD AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_IPUFDCK(val)		(IO_CHIPTOP.CLKSTOP7.bit.IPUFDCK=(0x02|val))								/**< Set IPUFD AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_IPUVISCK()			(IO_CHIPTOP.CLKSTOP7.bit.IPUVISCK)											/**< Get IPUVIS AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_IPUVISCK(val)		(IO_CHIPTOP.CLKSTOP7.bit.IPUVISCK=(0x02|val))								/**< Set IPUVIS AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_IPUAX()				(IO_CHIPTOP.CLKSTOP7.bit.IPUAX)												/**< Get IPU AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_IPUAX(val)			(IO_CHIPTOP.CLKSTOP7.bit.IPUAX=(0x02|val))									/**< Set IPU AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_IPUAH()				(IO_CHIPTOP.CLKSTOP7.bit.IPUAH)												/**< Get IPU AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_IPUAH(val)			(IO_CHIPTOP.CLKSTOP7.bit.IPUAH=(0x02|val))									/**< Set IPU AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_RAWCK()				(IO_CHIPTOP.CLKSTOP7.bit.RAWCK)												/**< Get RAW Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_RAWCK(val)			(IO_CHIPTOP.CLKSTOP7.bit.RAWCK=(0x02|val))									/**< Set RAW Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_RAWAX()				(IO_CHIPTOP.CLKSTOP7.bit.RAWAX)												/**< Get RAW AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_RAWAX(val)			(IO_CHIPTOP.CLKSTOP7.bit.RAWAX=(0x02|val))									/**< Set RAW AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_RAWAP()				(IO_CHIPTOP.CLKSTOP7.bit.RAWAP)												/**< Get RAW APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_RAWAP(val)			(IO_CHIPTOP.CLKSTOP7.bit.RAWAP=(0x02|val))									/**< Set RAW APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_SHDRCK()			(IO_CHIPTOP.CLKSTOP7.bit.SHDRCK)											/**< Get 3DNR Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_SHDRCK(val)			(IO_CHIPTOP.CLKSTOP7.bit.SHDRCK=(0x02|val))									/**< Set 3DNR Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_SHDRAX()			(IO_CHIPTOP.CLKSTOP7.bit.SHDRAX)											/**< Get 3DNR AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_SHDRAX(val)			(IO_CHIPTOP.CLKSTOP7.bit.SHDRAX=(0x02|val))									/**< Set 3DNR AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_SHDRAH()			(IO_CHIPTOP.CLKSTOP7.bit.SHDRAH)											/**< Get 3DNR AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_SHDRAH(val)			(IO_CHIPTOP.CLKSTOP7.bit.SHDRAH=(0x02|val))									/**< Set 3DNR AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_SHDRAP()			(IO_CHIPTOP.CLKSTOP7.bit.SHDRAP)											/**< Get 3DNR APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_SHDRAP(val)			(IO_CHIPTOP.CLKSTOP7.bit.SHDRAP=(0x02|val))									/**< Set 3DNR APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_M0CK()				(IO_CHIPTOP.CLKSTOP7.bit.M0CK)												/**< Get M0 Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_M0CK(val)			(IO_CHIPTOP.CLKSTOP7.bit.M0CK=(0x02|val))									/**< Set M0 Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_MECK()				(IO_CHIPTOP.CLKSTOP7.bit.MECK)												/**< Get ME Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_MECK(val)			(IO_CHIPTOP.CLKSTOP7.bit.MECK=(0x02|val))									/**< Set ME Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_MEAX()				(IO_CHIPTOP.CLKSTOP7.bit.MEAX)												/**< Get ME AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_MEAX(val)			(IO_CHIPTOP.CLKSTOP7.bit.MEAX=(0x02|val))									/**< Set ME AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP7_MEAP()				(IO_CHIPTOP.CLKSTOP7.bit.MEAP)												/**< Get ME APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP7_MEAP(val)			(IO_CHIPTOP.CLKSTOP7.bit.MEAP=(0x02|val))									/**< Set ME APB Bus Clock Stop */
// CLKSTOP8
#define	Dd_Top_Get_CLKSTOP8_RIBCK()				(IO_CHIPTOP.CLKSTOP8.bit.RIBCK)												/**< Get RIBERY(H.264 Codec) Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_RIBCK(val)			(IO_CHIPTOP.CLKSTOP8.bit.RIBCK=(0x02|val))									/**< Set RIBERY(H.264 Codec) Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_RIBAH()				(IO_CHIPTOP.CLKSTOP8.bit.RIBAH)												/**< Get RIBERY(H.264 Codec) AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_RIBAH(val)			(IO_CHIPTOP.CLKSTOP8.bit.RIBAH=(0x02|val))									/**< Set RIBERY(H.264 Codec) AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_HEVDFCK()			(IO_CHIPTOP.CLKSTOP8.bit.HEVDFCK)											/**< Get HEVC VDF AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_HEVDFCK(val)		(IO_CHIPTOP.CLKSTOP8.bit.HEVDFCK=(0x02|val))								/**< Set HEVC VDF AHB Bus Clock Stop */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSTOP8_HEPXFCK()			(IO_CHIPTOP.CLKSTOP8.bit.HEPXFCK)											/**< Get HEVC PXF AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_HEPXFCK(val)		(IO_CHIPTOP.CLKSTOP8.bit.HEPXFCK=(0x02|val))								/**< Set HEVC PXF AHB Bus Clock Stop */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSTOP8_HEVENCCK()			(IO_CHIPTOP.CLKSTOP8.bit.HEVENCCK)											/**< Get HEVC ENC AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_HEVENCCK(val)		(IO_CHIPTOP.CLKSTOP8.bit.HEVENCCK=(0x02|val))								/**< Set HEVC ENC AHB Bus Clock Stop */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	Dd_Top_Get_CLKSTOP8_HEIPPCK()			(IO_CHIPTOP.CLKSTOP8.bit.HEIPPCK)											/**< Get HEVC IPP AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_HEIPPCK(val)		(IO_CHIPTOP.CLKSTOP8.bit.HEIPPCK=(0x02|val))								/**< Set HEVC IPP AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC0HEVCMX2()		(IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX2)										/**< Get UMC0 HEVC MXI Bus 200MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC0HEVCMX2(val)	(IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX2=(0x02|val))							/**< Set UMC0 HEVC MXI Bus 200MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC0HEVCMX4()		(IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX4)										/**< Get UMC0 HEVC MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC0HEVCMX4(val)	(IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX4=(0x02|val))							/**< Set UMC0 HEVC MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC0RBRMX4()		(IO_CHIPTOP.CLKSTOP8.bit.UMC0RBRMX4)										/**< Get UMC0 RIBERY MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC0RBRMX4(val)		(IO_CHIPTOP.CLKSTOP8.bit.UMC0RBRMX4=(0x02|val))								/**< Set UMC0 RIBERY MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC1HEVCMX2()		(IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX2)										/**< Get UMC1 HEVC MXI Bus 200MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC1HEVCMX2(val)	(IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX2=(0x02|val))							/**< Set UMC1 HEVC MXI Bus 200MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC1HEVCMX4()		(IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX4)										/**< Get UMC1 HEVC MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC1HEVCMX4(val)	(IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX4=(0x02|val))							/**< Set UMC1 HEVC MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC1RBRMX4()		(IO_CHIPTOP.CLKSTOP8.bit.UMC1RBRMX4)										/**< Get UMC1 RIBERY MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC1RBRMX4(val)		(IO_CHIPTOP.CLKSTOP8.bit.UMC1RBRMX4=(0x02|val))								/**< Set UMC1 RIBERY MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC0CMNAX()			(IO_CHIPTOP.CLKSTOP8.bit.UMC0CMNAX)											/**< Get UMC0 CMN AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC0CMNAX(val)		(IO_CHIPTOP.CLKSTOP8.bit.UMC0CMNAX=(0x02|val))								/**< Set UMC0 CMN AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC0MX1AX()			(IO_CHIPTOP.CLKSTOP8.bit.UMC0MX1AX)											/**< Get UMC0 MXIC1 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC0MX1AX(val)		(IO_CHIPTOP.CLKSTOP8.bit.UMC0MX1AX=(0x02|val))								/**< Set UMC0 MXIC1 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC0MX2AX()			(IO_CHIPTOP.CLKSTOP8.bit.UMC0MX2AX)											/**< Get UMC0 MXIC2 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC0MX2AX(val)		(IO_CHIPTOP.CLKSTOP8.bit.UMC0MX2AX=(0x02|val))								/**< Set UMC0 MXIC2 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC0MX3AX()			(IO_CHIPTOP.CLKSTOP8.bit.UMC0MX3AX)											/**< Get UMC0 MXIC3 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC0MX3AX(val)		(IO_CHIPTOP.CLKSTOP8.bit.UMC0MX3AX=(0x02|val))								/**< Set UMC0 MXIC3 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP8_UMC0MX4AX()			(IO_CHIPTOP.CLKSTOP8.bit.UMC0MX4AX)											/**< Get UMC0 MXIC4 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP8_UMC0MX4AX(val)		(IO_CHIPTOP.CLKSTOP8.bit.UMC0MX4AX=(0x02|val))								/**< Set UMC0 MXIC4 MXI Bus Clock Stop */
// CLKSTOP9
#define	Dd_Top_Get_CLKSTOP9_UMC0MX5AX()			(IO_CHIPTOP.CLKSTOP9.bit.UMC0MX5AX)											/**< Get UMC0 MXIC5 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC0MX5AX(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC0MX5AX=(0x02|val))								/**< Set UMC0 MXIC5 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1CMNAX()			(IO_CHIPTOP.CLKSTOP9.bit.UMC1CMNAX)											/**< Get UMC1 CMN AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1CMNAX(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC1CMNAX=(0x02|val))								/**< Set UMC1 CMN AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1MX1AX()			(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX1AX)											/**< Get UMC1 MXIC1 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1MX1AX(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX1AX=(0x02|val))								/**< Set UMC1 MXIC1 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1MX2AX()			(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX2AX)											/**< Get UMC1 MXIC2 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1MX2AX(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX2AX=(0x02|val))								/**< Set UMC1 MXIC2 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1MX3AX()			(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX3AX)											/**< Get UMC1 MXIC3 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1MX3AX(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX3AX=(0x02|val))								/**< Set UMC1 MXIC3 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1MX4AX()			(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX4AX)											/**< Get UMC1 MXIC4 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1MX4AX(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX4AX=(0x02|val))								/**< Set UMC1 MXIC4 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1MX5AX()			(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX5AX)											/**< Get UMC1 MXIC5 MXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1MX5AX(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX5AX=(0x02|val))								/**< Set UMC1 MXIC5 MXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC0MX0AX3()		(IO_CHIPTOP.CLKSTOP9.bit.UMC0MX0AX3)										/**< Get UMC0 MXIC0 MXI Bus 300MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC0MX0AX3(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC0MX0AX3=(0x02|val))								/**< Set UMC0 MXIC0 MXI Bus 300MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC0MX6AX4()		(IO_CHIPTOP.CLKSTOP9.bit.UMC0MX6AX4)										/**< Get UMC0 MXIC6 MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC0MX6AX4(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC0MX6AX4=(0x02|val))								/**< Set UMC0 MXIC6 MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC0HEVCAX4()		(IO_CHIPTOP.CLKSTOP9.bit.UMC0HEVCAX4)										/**< Get UMC0 HEVC AXI Bus 400MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC0HEVCAX4(val)	(IO_CHIPTOP.CLKSTOP9.bit.UMC0HEVCAX4=(0x02|val))							/**< Set UMC0 HEVC AXI Bus 400MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1MX0AX3()		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX0AX3)										/**< Get UMC1 MXIC0 MXI Bus 300MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1MX0AX3(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX0AX3=(0x02|val))								/**< Set UMC1 MXIC0 MXI Bus 300MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1MX6AX4()		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX6AX4)										/**< Get UMC1 MXIC6 MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1MX6AX4(val)		(IO_CHIPTOP.CLKSTOP9.bit.UMC1MX6AX4=(0x02|val))								/**< Set UMC1 MXIC6 MXI Bus 400MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1HEVCAX4()		(IO_CHIPTOP.CLKSTOP9.bit.UMC1HEVCAX4)										/**< Get UMC1 HEVC AXI Bus 400MHz Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1HEVCAX4(val)	(IO_CHIPTOP.CLKSTOP9.bit.UMC1HEVCAX4=(0x02|val))							/**< Set UMC1 HEVC AXI Bus 400MHz Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC0AP()			(IO_CHIPTOP.CLKSTOP9.bit.UMC0AP)											/**< Get UMC0 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC0AP(val)			(IO_CHIPTOP.CLKSTOP9.bit.UMC0AP=(0x02|val))									/**< Set UMC0 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP9_UMC1AP()			(IO_CHIPTOP.CLKSTOP9.bit.UMC1AP)											/**< Get UMC1 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP9_UMC1AP(val)			(IO_CHIPTOP.CLKSTOP9.bit.UMC1AP=(0x02|val))									/**< Set UMC1 APB Bus Clock Stop */
// CLKSTOP10
#define	Dd_Top_Get_CLKSTOP10_SRO1CK()			(IO_CHIPTOP.CLKSTOP10.bit.SRO1CK)											/**< Get SRO pipe1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_SRO1CK(val)		(IO_CHIPTOP.CLKSTOP10.bit.SRO1CK=(0x02|val))								/**< Set SRO pipe1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_SRO1CK_2()			(IO_CHIPTOP.CLKSTOP10.bit.SRO1CK_2)											/**< Get SRO pipe1 Clock2 Stop */
#define	Dd_Top_Set_CLKSTOP10_SRO1CK_2(val)		(IO_CHIPTOP.CLKSTOP10.bit.SRO1CK_2=(0x02|val))								/**< Set SRO pipe1 Clock2 Stop */
#define	Dd_Top_Get_CLKSTOP10_SRO1AX()			(IO_CHIPTOP.CLKSTOP10.bit.SRO1AX)											/**< Get SRO pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_SRO1AX(val)		(IO_CHIPTOP.CLKSTOP10.bit.SRO1AX=(0x02|val))								/**< Set SRO pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_SRO1AH()			(IO_CHIPTOP.CLKSTOP10.bit.SRO1AH)											/**< Get SRO pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_SRO1AH(val)		(IO_CHIPTOP.CLKSTOP10.bit.SRO1AH=(0x02|val))								/**< Set SRO pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_SRO1AP()			(IO_CHIPTOP.CLKSTOP10.bit.SRO1AP)											/**< Get SRO pipe1 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_SRO1AP(val)		(IO_CHIPTOP.CLKSTOP10.bit.SRO1AP=(0x02|val))								/**< Set SRO pipe1 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_B2B1CK()			(IO_CHIPTOP.CLKSTOP10.bit.B2B1CK)											/**< Get B2B pipe1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_B2B1CK(val)		(IO_CHIPTOP.CLKSTOP10.bit.B2B1CK=(0x02|val))								/**< Set B2B pipe1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_B2B1AX()			(IO_CHIPTOP.CLKSTOP10.bit.B2B1AX)											/**< Get B2B pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_B2B1AX(val)		(IO_CHIPTOP.CLKSTOP10.bit.B2B1AX=(0x02|val))								/**< Set B2B pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_B2B1AH()			(IO_CHIPTOP.CLKSTOP10.bit.B2B1AH)											/**< Get B2B pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_B2B1AH(val)		(IO_CHIPTOP.CLKSTOP10.bit.B2B1AH=(0x02|val))								/**< Set B2B pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_B2B1AP()			(IO_CHIPTOP.CLKSTOP10.bit.B2B1AP)											/**< Get B2B pipe1 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_B2B1AP(val)		(IO_CHIPTOP.CLKSTOP10.bit.B2B1AP=(0x02|val))								/**< Set B2B pipe1 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_LTMRBK1CK()		(IO_CHIPTOP.CLKSTOP10.bit.LTMRBK1CK)										/**< Get LTMRBK1 pipe1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_LTMRBK1CK(val)		(IO_CHIPTOP.CLKSTOP10.bit.LTMRBK1CK=(0x02|val))								/**< Set LTMRBK1 pipe1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_B2R1CK()			(IO_CHIPTOP.CLKSTOP10.bit.B2R1CK)											/**< Get B2R pipe1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_B2R1CK(val)		(IO_CHIPTOP.CLKSTOP10.bit.B2R1CK=(0x02|val))								/**< Set B2R pipe1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_B2R1AX()			(IO_CHIPTOP.CLKSTOP10.bit.B2R1AX)											/**< Get B2R pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_B2R1AX(val)		(IO_CHIPTOP.CLKSTOP10.bit.B2R1AX=(0x02|val))								/**< Set B2R pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_B2R1AH()			(IO_CHIPTOP.CLKSTOP10.bit.B2R1AH)											/**< Get B2R pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_B2R1AH(val)		(IO_CHIPTOP.CLKSTOP10.bit.B2R1AH=(0x02|val))								/**< Set B2R pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP10_B2R1AP()			(IO_CHIPTOP.CLKSTOP10.bit.B2R1AP)											/**< Get B2R pipe1 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP10_B2R1AP(val)		(IO_CHIPTOP.CLKSTOP10.bit.B2R1AP=(0x02|val))								/**< Set B2R pipe1 APB Bus Clock Stop */
// CLKSTOP11
#define	Dd_Top_Get_CLKSTOP11_LTM1CK()			(IO_CHIPTOP.CLKSTOP11.bit.LTM1CK)											/**< Get LTM pipe1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_LTM1CK(val)		(IO_CHIPTOP.CLKSTOP11.bit.LTM1CK=(0x02|val))								/**< Set LTM pipe1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_LTM1AX()			(IO_CHIPTOP.CLKSTOP11.bit.LTM1AX)											/**< Get LTM pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_LTM1AX(val)		(IO_CHIPTOP.CLKSTOP11.bit.LTM1AX=(0x02|val))								/**< Set LTM pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_LTM1AH()			(IO_CHIPTOP.CLKSTOP11.bit.LTM1AH)											/**< Get LTM pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_LTM1AH(val)		(IO_CHIPTOP.CLKSTOP11.bit.LTM1AH=(0x02|val))								/**< Set LTM pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_LTM1AP()			(IO_CHIPTOP.CLKSTOP11.bit.LTM1AP)											/**< Get LTM pipe1 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_LTM1AP(val)		(IO_CHIPTOP.CLKSTOP11.bit.LTM1AP=(0x02|val))								/**< Set LTM pipe1 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_R2Y1CK()			(IO_CHIPTOP.CLKSTOP11.bit.R2Y1CK)											/**< Get R2Y pipe1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_R2Y1CK(val)		(IO_CHIPTOP.CLKSTOP11.bit.R2Y1CK=(0x02|val))								/**< Set R2Y pipe1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_R2Y1AX()			(IO_CHIPTOP.CLKSTOP11.bit.R2Y1AX)											/**< Get R2Y pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_R2Y1AX(val)		(IO_CHIPTOP.CLKSTOP11.bit.R2Y1AX=(0x02|val))								/**< Set R2Y pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_R2Y1AH()			(IO_CHIPTOP.CLKSTOP11.bit.R2Y1AH)											/**< Get R2Y pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_R2Y1AH(val)		(IO_CHIPTOP.CLKSTOP11.bit.R2Y1AH=(0x02|val))								/**< Set R2Y pipe1 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_R2Y1AP()			(IO_CHIPTOP.CLKSTOP11.bit.R2Y1AP)											/**< Get R2Y pipe1 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_R2Y1AP(val)		(IO_CHIPTOP.CLKSTOP11.bit.R2Y1AP=(0x02|val))								/**< Set R2Y pipe1 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_CNR1CK()			(IO_CHIPTOP.CLKSTOP11.bit.CNR1CK)											/**< Get CNR pipe1 Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_CNR1CK(val)		(IO_CHIPTOP.CLKSTOP11.bit.CNR1CK=(0x02|val))								/**< Set CNR pipe1 Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_CNR1AX()			(IO_CHIPTOP.CLKSTOP11.bit.CNR1AX)											/**< Get CNR pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_CNR1AX(val)		(IO_CHIPTOP.CLKSTOP11.bit.CNR1AX=(0x02|val))								/**< Set CNR pipe1 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_CNR1AP()			(IO_CHIPTOP.CLKSTOP11.bit.CNR1AP)											/**< Get CNR pipe1 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_CNR1AP(val)		(IO_CHIPTOP.CLKSTOP11.bit.CNR1AP=(0x02|val))								/**< Set CNR pipe1 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_APAH()				(IO_CHIPTOP.CLKSTOP11.bit.APAH)												/**< Get CPU HCLKAP AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_APAH(val)			(IO_CHIPTOP.CLKSTOP11.bit.APAH=(0x02|val))									/**< Set CPU HCLKAP AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_DBGAP()			(IO_CHIPTOP.CLKSTOP11.bit.DBGAP)											/**< Get CPU PCLKDBG AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_DBGAP(val)			(IO_CHIPTOP.CLKSTOP11.bit.DBGAP=(0x02|val))									/**< Set CPU PCLKDBG AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP11_NFBCHCK()			(IO_CHIPTOP.CLKSTOP11.bit.NFBCHCK)											/**< Get CPU NFBCHCK AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP11_NFBCHCK(val)		(IO_CHIPTOP.CLKSTOP11.bit.NFBCHCK=(0x02|val))								/**< Set CPU NFBCHCK AHB Bus Clock Stop */
// CLKSTOP12
#define	Dd_Top_Get_CLKSTOP12_SRO2CK()			(IO_CHIPTOP.CLKSTOP12.bit.SRO2CK)											/**< Get SRO pipe2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_SRO2CK(val)		(IO_CHIPTOP.CLKSTOP12.bit.SRO2CK=(0x02|val))								/**< Set SRO pipe2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_SRO2CK_2()			(IO_CHIPTOP.CLKSTOP12.bit.SRO2CK_2)											/**< Get SRO pipe2 Clock2 Stop */
#define	Dd_Top_Set_CLKSTOP12_SRO2CK_2(val)		(IO_CHIPTOP.CLKSTOP12.bit.SRO2CK_2=(0x02|val))								/**< Set SRO pipe2 Clock2 Stop */
#define	Dd_Top_Get_CLKSTOP12_SRO2AX()			(IO_CHIPTOP.CLKSTOP12.bit.SRO2AX)											/**< Get SRO pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_SRO2AX(val)		(IO_CHIPTOP.CLKSTOP12.bit.SRO2AX=(0x02|val))								/**< Set SRO pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_SRO2AH()			(IO_CHIPTOP.CLKSTOP12.bit.SRO2AH)											/**< Get SRO pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_SRO2AH(val)		(IO_CHIPTOP.CLKSTOP12.bit.SRO2AH=(0x02|val))								/**< Set SRO pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_SRO2AP()			(IO_CHIPTOP.CLKSTOP12.bit.SRO2AP)											/**< Get SRO pipe2 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_SRO2AP(val)		(IO_CHIPTOP.CLKSTOP12.bit.SRO2AP=(0x02|val))								/**< Set SRO pipe2 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_B2B2CK()			(IO_CHIPTOP.CLKSTOP12.bit.B2B2CK)											/**< Get B2B pipe2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_B2B2CK(val)		(IO_CHIPTOP.CLKSTOP12.bit.B2B2CK=(0x02|val))								/**< Set B2B pipe2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_B2B2AX()			(IO_CHIPTOP.CLKSTOP12.bit.B2B2AX)											/**< Get B2B pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_B2B2AX(val)		(IO_CHIPTOP.CLKSTOP12.bit.B2B2AX=(0x02|val))								/**< Set B2B pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_B2B2AH()			(IO_CHIPTOP.CLKSTOP12.bit.B2B2AH)											/**< Get B2B pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_B2B2AH(val)		(IO_CHIPTOP.CLKSTOP12.bit.B2B2AH=(0x02|val))								/**< Set B2B pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_B2B2AP()			(IO_CHIPTOP.CLKSTOP12.bit.B2B2AP)											/**< Get B2B pipe2 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_B2B2AP(val)		(IO_CHIPTOP.CLKSTOP12.bit.B2B2AP=(0x02|val))								/**< Set B2B pipe2 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_LTMRBK2CK()		(IO_CHIPTOP.CLKSTOP12.bit.LTMRBK2CK)										/**< Get LTMRBK1 pipe2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_LTMRBK2CK(val)		(IO_CHIPTOP.CLKSTOP12.bit.LTMRBK2CK=(0x02|val))								/**< Set LTMRBK1 pipe2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_B2R2CK()			(IO_CHIPTOP.CLKSTOP12.bit.B2R2CK)											/**< Get B2R pipe2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_B2R2CK(val)		(IO_CHIPTOP.CLKSTOP12.bit.B2R2CK=(0x02|val))								/**< Set B2R pipe2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_B2R2AX()			(IO_CHIPTOP.CLKSTOP12.bit.B2R2AX)											/**< Get B2R pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_B2R2AX(val)		(IO_CHIPTOP.CLKSTOP12.bit.B2R2AX=(0x02|val))								/**< Set B2R pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_B2R2AH()			(IO_CHIPTOP.CLKSTOP12.bit.B2R2AH)											/**< Get B2R pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_B2R2AH(val)		(IO_CHIPTOP.CLKSTOP12.bit.B2R2AH=(0x02|val))								/**< Set B2R pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP12_B2R2AP()			(IO_CHIPTOP.CLKSTOP12.bit.B2R2AP)											/**< Get B2R pipe2 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP12_B2R2AP(val)		(IO_CHIPTOP.CLKSTOP12.bit.B2R2AP=(0x02|val))								/**< Set B2R pipe2 APB Bus Clock Stop */
// CLKSTOP13
#define	Dd_Top_Get_CLKSTOP13_LTM2CK()			(IO_CHIPTOP.CLKSTOP13.bit.LTM2CK)											/**< Get LTM pipe2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_LTM2CK(val)		(IO_CHIPTOP.CLKSTOP13.bit.LTM2CK=(0x02|val))								/**< Set LTM pipe2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_LTM2AX()			(IO_CHIPTOP.CLKSTOP13.bit.LTM2AX)											/**< Get LTM pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_LTM2AX(val)		(IO_CHIPTOP.CLKSTOP13.bit.LTM2AX=(0x02|val))								/**< Set LTM pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_LTM2AH()			(IO_CHIPTOP.CLKSTOP13.bit.LTM2AH)											/**< Get LTM pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_LTM2AH(val)		(IO_CHIPTOP.CLKSTOP13.bit.LTM2AH=(0x02|val))								/**< Set LTM pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_LTM2AP()			(IO_CHIPTOP.CLKSTOP13.bit.LTM2AP)											/**< Get LTM pipe2 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_LTM2AP(val)		(IO_CHIPTOP.CLKSTOP13.bit.LTM2AP=(0x02|val))								/**< Set LTM pipe2 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_R2Y2CK()			(IO_CHIPTOP.CLKSTOP13.bit.R2Y2CK)											/**< Get R2Y pipe2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_R2Y2CK(val)		(IO_CHIPTOP.CLKSTOP13.bit.R2Y2CK=(0x02|val))								/**< Set R2Y pipe2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_R2Y2AX()			(IO_CHIPTOP.CLKSTOP13.bit.R2Y2AX)											/**< Get R2Y pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_R2Y2AX(val)		(IO_CHIPTOP.CLKSTOP13.bit.R2Y2AX=(0x02|val))								/**< Set R2Y pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_R2Y2AH()			(IO_CHIPTOP.CLKSTOP13.bit.R2Y2AH)											/**< Get R2Y pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_R2Y2AH(val)		(IO_CHIPTOP.CLKSTOP13.bit.R2Y2AH=(0x02|val))								/**< Set R2Y pipe2 AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_R2Y2AP()			(IO_CHIPTOP.CLKSTOP13.bit.R2Y2AP)											/**< Get R2Y pipe2 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_R2Y2AP(val)		(IO_CHIPTOP.CLKSTOP13.bit.R2Y2AP=(0x02|val))								/**< Set R2Y pipe2 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_CNR2CK()			(IO_CHIPTOP.CLKSTOP13.bit.CNR2CK)											/**< Get CNR pipe2 Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_CNR2CK(val)		(IO_CHIPTOP.CLKSTOP13.bit.CNR2CK=(0x02|val))								/**< Set CNR pipe2 Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_CNR2AX()			(IO_CHIPTOP.CLKSTOP13.bit.CNR2AX)											/**< Get CNR pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_CNR2AX(val)		(IO_CHIPTOP.CLKSTOP13.bit.CNR2AX=(0x02|val))								/**< Set CNR pipe2 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_CNR2AP()			(IO_CHIPTOP.CLKSTOP13.bit.CNR2AP)											/**< Get CNR pipe2 APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP13_CNR2AP(val)		(IO_CHIPTOP.CLKSTOP13.bit.CNR2AP=(0x02|val))								/**< Set CNR pipe2 APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP13_UMCVDFMX4()		(IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX4)										/**< Get UMC VDF 400MHz MXI Bus Clock Stataus */
#define	Dd_Top_Set_CLKSTOP13_UMCVDFMX4(val)		(IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX4=(0x02|val))								/**< Set UMC VDF 400MHz MXI Bus Clock Stataus */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSTOP13_UMCPXFMX4()		(IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX4)										/**< Get UMC PXF 400MHz MXI Bus Clock Stataus */
#define	Dd_Top_Set_CLKSTOP13_UMCPXFMX4(val)		(IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX4=(0x02|val))								/**< Set UMC PXF 400MHz MXI Bus Clock Stataus */
#define	Dd_Top_Get_CLKSTOP13_UMCVDFMX2()		(IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX2)										/**< Get UMC VDF 200MHz MXI Bus Clock Stataus */
#define	Dd_Top_Set_CLKSTOP13_UMCVDFMX2(val)		(IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX2=(0x02|val))								/**< Set UMC VDF 200MHz MXI Bus Clock Stataus */
#define	Dd_Top_Get_CLKSTOP13_UMCPXFMX2()		(IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX2)										/**< Get UMC PXF 200MHz MXI Bus Clock Stataus */
#define	Dd_Top_Set_CLKSTOP13_UMCPXFMX2(val)		(IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX2=(0x02|val))								/**< Set UMC PXF 200MHz MXI Bus Clock Stataus */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	Dd_Top_Get_CLKSTOP13_UMCHEVENCAX4()		(IO_CHIPTOP.CLKSTOP13.bit.UMCHEVENCAX4)										/**< Get HEVENC 400MHz AXI Bus Clock Stataus */
#define	Dd_Top_Set_CLKSTOP13_UMCHEVENCAX4(val)	(IO_CHIPTOP.CLKSTOP13.bit.UMCHEVENCAX4=(0x02|val))							/**< Set HEVENC 400MHz AXI Bus Clock Stataus */
#define	Dd_Top_Get_CLKSTOP13_UMCVDFMX2()		(IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX2)										/**< Get UMC VDF 200MHz MXI Bus Clock Stataus */
#define	Dd_Top_Set_CLKSTOP13_UMCVDFMX2(val)		(IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX2=(0x02|val))								/**< Set UMC VDF 200MHz MXI Bus Clock Stataus */
#define	Dd_Top_Get_CLKSTOP13_HEVENCAP()			(IO_CHIPTOP.CLKSTOP13.bit.HEVENCAP)											/**< Get HEVENC APB Bus Clock of Stataus */
#define	Dd_Top_Set_CLKSTOP13_HEVENCAP(val)		(IO_CHIPTOP.CLKSTOP13.bit.HEVENCAP=(0x02|val))								/**< Set HEVENC APB Bus Clock of Stataus */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	Dd_Top_Get_CLKSTOP13_BMH1CK()			(IO_CHIPTOP.CLKSTOP13.bit.BMH1CK)											/**< Get UMC BMH1 Bus Clock Stataus */
#define	Dd_Top_Set_CLKSTOP13_BMH1CK(val)		(IO_CHIPTOP.CLKSTOP13.bit.BMH1CK=(0x02|val))								/**< Set UMC BMH1 Bus Clock Stataus */
// CLKSTOP14
#define	Dd_Top_Get_CLKSTOP14_STATAX()			(IO_CHIPTOP.CLKSTOP14.bit.STATAX)											/**< Get STAT AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_STATAX(val)		(IO_CHIPTOP.CLKSTOP14.bit.STATAX=(0x02|val))								/**< Set STAT AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_STATAH()			(IO_CHIPTOP.CLKSTOP14.bit.STATAH)											/**< Get STAT AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_STATAH(val)		(IO_CHIPTOP.CLKSTOP14.bit.STATAH=(0x02|val))								/**< Set STAT AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_STATAP()			(IO_CHIPTOP.CLKSTOP14.bit.STATAP)											/**< Get STAT APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_STATAP(val)		(IO_CHIPTOP.CLKSTOP14.bit.STATAP=(0x02|val))								/**< Set STAT APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_PERIAH()			(IO_CHIPTOP.CLKSTOP14.bit.PERIAH)											/**< Get Peripheral AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_PERIAH(val)		(IO_CHIPTOP.CLKSTOP14.bit.PERIAH=(0x02|val))								/**< Set Peripheral AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_PERIAP()			(IO_CHIPTOP.CLKSTOP14.bit.PERIAP)											/**< Get Peripheral APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_PERIAP(val)		(IO_CHIPTOP.CLKSTOP14.bit.PERIAP=(0x02|val))								/**< Set Peripheral APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_SENMSKCK()			(IO_CHIPTOP.CLKSTOP14.bit.SENMSKCK)											/**< Get mask clock of SENSOR Stop */
#define	Dd_Top_Set_CLKSTOP14_SENMSKCK(val)		(IO_CHIPTOP.CLKSTOP14.bit.SENMSKCK=(0x02|val))								/**< Set mask clock of SENSOR Stop */
#define	Dd_Top_Get_CLKSTOP14_GYROCK()			(IO_CHIPTOP.CLKSTOP14.bit.GYROCK)											/**< Get GYRO Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_GYROCK(val)		(IO_CHIPTOP.CLKSTOP14.bit.GYROCK=(0x02|val))								/**< Set GYRO Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_EXSAH()			(IO_CHIPTOP.CLKSTOP14.bit.EXSAH)											/**< Get EXS AHB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_EXSAH(val)			(IO_CHIPTOP.CLKSTOP14.bit.EXSAH=(0x02|val))									/**< Set EXS AHB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_EXSAP()			(IO_CHIPTOP.CLKSTOP14.bit.EXSAP)											/**< Get EXS APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_EXSAP(val)			(IO_CHIPTOP.CLKSTOP14.bit.EXSAP=(0x02|val))									/**< Set EXS APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_PASCK()			(IO_CHIPTOP.CLKSTOP14.bit.PASCK)											/**< Get PAS Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_PASCK(val)			(IO_CHIPTOP.CLKSTOP14.bit.PASCK=(0x02|val))									/**< Set PAS Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_BMH0CK()			(IO_CHIPTOP.CLKSTOP14.bit.BMH0CK)											/**< Get BMH0 Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_BMH0CK(val)		(IO_CHIPTOP.CLKSTOP14.bit.BMH0CK=(0x02|val))								/**< Set BMH0 Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_BMH0AX()			(IO_CHIPTOP.CLKSTOP14.bit.BMH0AX)											/**< Get BMH0 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_BMH0AX(val)		(IO_CHIPTOP.CLKSTOP14.bit.BMH0AX=(0x02|val))								/**< Set BMH0 AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_RDMAAX()			(IO_CHIPTOP.CLKSTOP14.bit.RDMAAX)											/**< Get RDMA AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_RDMAAX(val)		(IO_CHIPTOP.CLKSTOP14.bit.RDMAAX=(0x02|val))								/**< Set RDMA AXI Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_RDMAAP()			(IO_CHIPTOP.CLKSTOP14.bit.RDMAAP)											/**< Get RDMA APB Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_RDMAAP(val)		(IO_CHIPTOP.CLKSTOP14.bit.RDMAAP=(0x02|val))								/**< Set RDMA APB Bus Clock Stop */
#define	Dd_Top_Get_CLKSTOP14_BMH1AX()			(IO_CHIPTOP.CLKSTOP14.bit.BMH1AX)											/**< Get BMH1 AXI Bus Clock Stop */
#define	Dd_Top_Set_CLKSTOP14_BMH1AX(val)		(IO_CHIPTOP.CLKSTOP14.bit.BMH1AX=(0x02|val))								/**< Set BMH1 AXI Bus Clock Stop */

// CRRSM(Reset and monitor register)
#define	Dd_Top_Get_CRRSM_WDRST()				(IO_CHIPTOP.CRRSM.bit.__WDRST)												/**< Get Watchdog Reset Monitor */
#define	Dd_Top_Clear_CRRSM_WDRST()				{IO_CHIPTOP.CRRSM.word = ~(1 << 0);}										/**< Set Watchdog Reset Clear */
#define	Dd_Top_Get_CRRSM_SWRST()				(IO_CHIPTOP.CRRSM.bit.__SWRST)												/**< Get SRREQ Bit Reset Monitor */
#define	Dd_Top_Clear_CRRSM_SWRST()				{IO_CHIPTOP.CRRSM.word = ~(1 << 1);}										/**< Set SRREQ Bit Reset Clear */
#define	Dd_Top_Get_CRRSM_SRST()					(IO_CHIPTOP.CRRSM.bit.__SRST)												/**< Get TSRSTX Terminal Reset Monitor */
#define	Dd_Top_Clear_CRRSM_SRST()				{IO_CHIPTOP.CRRSM.word = ~(1 << 2);}										/**< Set TSRSTX Terminal Reset Clear */
#define	Dd_Top_Get_CRRSM_PORST()				(IO_CHIPTOP.CRRSM.bit.__PORST)												/**< Get RSTX Terminal Reset Monitor */
#define	Dd_Top_Clear_CRRSM_PORST()				{IO_CHIPTOP.CRRSM.word = ~(1 << 3);}										/**< Set RSTX Terminal Reset Clear */

// CRRRS(Register controlled request register)
#define	Dd_Top_Get_CRRRS_RIBSR()				(IO_CHIPTOP.CRRRS.bit.RIBSR)												/**< Get RIBERY(H.264 Codec) Macro Reset */
#define	Dd_Top_Reset_CRRRS_RIBSR()				{IO_CHIPTOP.CRRRS.bit.RIBSR=1;}												/**< Set RIBERY(H.264 Codec) Macro Reset Generation */
#define	Dd_Top_Clear_CRRRS_RIBSR()				{IO_CHIPTOP.CRRRS.bit.RIBSR=0;}												/**< Set RIBERY(H.264 Codec) Macro Reset Clear */
#define	Dd_Top_Get_CRRRS_HEIPPASYNCSR()			(IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR)											/**< Get HEIPP async Reset */
#define	Dd_Top_Reset_CRRRS_HEIPPASYNCSR()		{IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR=1;}										/**< Set HEIPP async Reset Generation */
#define	Dd_Top_Clear_CRRRS_HEIPPASYNCSR()		{IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR=0;}										/**< Set HEIPP async Reset Clear */
#define	Dd_Top_Get_CRRRS_HEIPPSYNCSR()			(IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR)											/**< Get HEIPP sync Reset */
#define	Dd_Top_Reset_CRRRS_HEIPPSYNCSR()		{IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR=1;}										/**< Set HEIPP sync Reset Generation */
#define	Dd_Top_Clear_CRRRS_HEIPPSYNCSR()		{IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR=0;}										/**< Set HEIPP sync Reset Clear */
#define	Dd_Top_Get_CRRRS_HEVDFSR()				(IO_CHIPTOP.CRRRS.bit.HEVDFSR)												/**< Get HEVDF async Reset */
#define	Dd_Top_Reset_CRRRS_HEVDFSR()			{IO_CHIPTOP.CRRRS.bit.HEVDFSR=1;}											/**< Set HEVDF async Reset Generation */
#define	Dd_Top_Clear_CRRRS_HEVDFSR()			{IO_CHIPTOP.CRRRS.bit.HEVDFSR=0;}											/**< Set HEVDF async Reset Clear */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	Dd_Top_Get_CRRRS_HEPXFSR()				(IO_CHIPTOP.CRRRS.bit.HEPXFSR)												/**< Get HEPXF sync Reset */
#define	Dd_Top_Reset_CRRRS_HEPXFSR()			{IO_CHIPTOP.CRRRS.bit.HEPXFSR=1;}											/**< Set HEPXF sync Reset Generation */
#define	Dd_Top_Clear_CRRRS_HEPXFSR()			{IO_CHIPTOP.CRRRS.bit.HEPXFSR=0;}											/**< Set HEPXF sync Reset Clear */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	Dd_Top_Get_CRRRS_HEVENCSR()				(IO_CHIPTOP.CRRRS.bit.HEVENCSR)												/**< Get HEVENC sync Reset */
#define	Dd_Top_Reset_CRRRS_HEVENCSR()			{IO_CHIPTOP.CRRRS.bit.HEVENCSR=1;}											/**< Set HEVENC sync Reset Generation */
#define	Dd_Top_Clear_CRRRS_HEVENCSR()			{IO_CHIPTOP.CRRRS.bit.HEVENCSR=0;}											/**< Set HEVENC sync Reset Clear */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	Dd_Top_Get_CRRRS_DSPSR()				(IO_CHIPTOP.CRRRS.bit.DSPSR)												/**< Get DSP Reset */
#define	Dd_Top_Reset_CRRRS_DSPSR()				{IO_CHIPTOP.CRRRS.bit.DSPSR=1;}												/**< Set DSP Reset Generation */
#define	Dd_Top_Clear_CRRRS_DSPSR()				{IO_CHIPTOP.CRRRS.bit.DSPSR=0;}												/**< Set DSP Reset Clear */
#define	Dd_Top_Get_CRRRS_CPU0_SR()				(IO_CHIPTOP.CRRRS.bit.CPU0_SR)												/**< Get Cortex-A7 MPCore CPU0 Reset */
#define	Dd_Top_Reset_CRRRS_CPU0_SR()			{IO_CHIPTOP.CRRRS.bit.CPU0_SR=0;}											/**< Set Cortex-A7 MPCore CPU0 Reset Generation */
#define	Dd_Top_Clear_CRRRS_CPU0_SR()			{IO_CHIPTOP.CRRRS.bit.CPU0_SR=1;}											/**< Set Cortex-A7 MPCore CPU0 Reset Clear */
#define	Dd_Top_Get_CRRRS_CPU1_SR()				(IO_CHIPTOP.CRRRS.bit.CPU1_SR)												/**< Get Cortex-A7 MPCore CPU1 Reset */
#define	Dd_Top_Reset_CRRRS_CPU1_SR()			{IO_CHIPTOP.CRRRS.bit.CPU1_SR=0;}											/**< Set Cortex-A7 MPCore CPU1 Reset Generation */
#define	Dd_Top_Clear_CRRRS_CPU1_SR()			{IO_CHIPTOP.CRRRS.bit.CPU1_SR=1;}											/**< Set Cortex-A7 MPCore CPU1 Reset Clear */
#define	Dd_Top_Get_CRRRS_CPU2_SR()				(IO_CHIPTOP.CRRRS.bit.CPU2_SR)												/**< Get Cortex-A7 MPCore CPU2 Reset */
#define	Dd_Top_Reset_CRRRS_CPU2_SR()			{IO_CHIPTOP.CRRRS.bit.CPU2_SR=0;}											/**< Set Cortex-A7 MPCore CPU2 Reset Generation */
#define	Dd_Top_Clear_CRRRS_CPU2_SR()			{IO_CHIPTOP.CRRRS.bit.CPU2_SR=1;}											/**< Set Cortex-A7 MPCore CPU2 Reset Clear */
#define	Dd_Top_Get_CRRRS_CPU3_SR()				(IO_CHIPTOP.CRRRS.bit.CPU3_SR)												/**< Get Cortex-A7 MPCore CPU3 Reset */
#define	Dd_Top_Reset_CRRRS_CPU3_SR()			{IO_CHIPTOP.CRRRS.bit.CPU3_SR=0;}											/**< Set Cortex-A7 MPCore CPU3 Reset Generation */
#define	Dd_Top_Clear_CRRRS_CPU3_SR()			{IO_CHIPTOP.CRRRS.bit.CPU3_SR=1;}											/**< Set Cortex-A7 MPCore CPU3 Reset Clear */

// CRSWR(Software reset request register 0)
#define	Dd_Top_Set_CRSWR_SRREQ()				{IO_CHIPTOP.CRSWR.bit.SRREQ=1;}												/**< Set ARESETn Generation */

// PLLFREQ1
#define	Dd_Top_Get_PLLFREQ1_PLLIDIV()		(IO_CHIPTOP.PLLFREQ1.bit.PLLIDIV)												/**< Get PLLFREQ1 Divider */
#define	Dd_Top_Set_PLLFREQ1_PLLIDIV(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.PLLFREQ1.bit.PLLIDIV=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}							/**< Set PLLFREQ1 Divider */

// ODIVCH0
#define	Dd_Top_Get_ODIVCH0_ODIVCH0NUM()		(IO_CHIPTOP.ODIVCH0.bit.ODIVCH0NUM)												/**< Get ODIVCH0 Divider */
#define	Dd_Top_Set_ODIVCH0_ODIVCH0NUM(val)	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.ODIVCH0.bit.ODIVCH0NUM=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}							/**< Set ODIVCH0 Divider */

// CMDEN
#define	Dd_Top_Get_CMDEN_CMDEN()			(IO_CHIPTOP.CMDEN.bit.CMDEN)													/**< Get CMDEN Divider */
#define	Dd_Top_Set_CMDEN_CMDEN(val)			{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock); \
												IO_CHIPTOP.CMDEN.bit.CMDEN=(val); \
												Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}							/**< Set CMDEN Divider */


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
Get Frequency of eMMC clock
@retval	Hz
*/
ULONG Dd_Top_Get_EMMCCLK(VOID);

/**
Get Frequency of NAND Flash clock
@retval	Hz
*/
ULONG Dd_Top_Get_NFCLK(VOID);

/**
Get Frequency of UHS2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_UHS2CLK(VOID);

/**
Get Frequency of UHS1 ch2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_UHS1CLK2(VOID);

/**
Get Frequency of UHS1 ch1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_UHS1CLK1(VOID);

/**
Get Frequency of UHS1 ch0 clock
@retval	Hz
*/
ULONG Dd_Top_Get_UHS1CLK0(VOID);

/**
Get Frequency of Cortex-A7 MPCore Peri clock
@retval	Hz
*/
ULONG Dd_Top_Get_RCLK(VOID);

/**
Get Frequency of RAW clock
@retval	Hz
*/
ULONG Dd_Top_Get_RAWCLK(VOID);

/**
Get Frequency of DISP HIF clock
@retval	Hz
*/
ULONG Dd_Top_Get_HIFCLK(VOID);

/**
Get Frequency of DISP MIF clock
@retval	Hz
*/
ULONG Dd_Top_Get_MIFCLK(VOID);

/**
Get Frequency of IPU system clock
@retval	Hz
*/
ULONG Dd_Top_Get_IPUCLK(VOID);

/**
Get Frequency of IPU TME clock
@retval	Hz
*/
ULONG Dd_Top_Get_IPUTMECLK(VOID);

/**
Get Frequency of GPU clock
@retval	Hz
*/
ULONG Dd_Top_Get_GPUCLK(VOID);

/**
Get Frequency of JPEG clock
@retval	Hz
*/
ULONG Dd_Top_Get_JPEGCLK(VOID);

/**
Get Frequency of ELA clock
@retval	Hz
*/
ULONG Dd_Top_Get_ELACLK(VOID);

/**
Get Frequency of SENSOR clock
@retval	Hz
*/
ULONG Dd_Top_Get_SENCLK(VOID);

/**
Get Frequency of SENSOR clock
@retval	Hz
*/
ULONG Dd_Top_Get_SENMSKCLK(VOID);

/**
Get Frequency of IIP clock
@retval	Hz
*/
ULONG Dd_Top_Get_IIPCLK(VOID);

/**
Get Frequency of STAT clock
@retval	Hz
*/
ULONG Dd_Top_Get_PASCLK(VOID);

/**
Get Frequency of HEVC IPP clock
@retval	Hz
*/
ULONG Dd_Top_Get_IPPCLK(VOID);

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
Get Frequency of HEVC PXF clock
@retval	Hz
*/
ULONG Dd_Top_Get_PXFCLK(VOID);
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
Get Frequency of HEVC ENC clock
@retval	Hz
*/
ULONG Dd_Top_Get_HEVENCCLK(VOID);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/**
Get Frequency of HEVC VDF clock
@retval	Hz
*/
ULONG Dd_Top_Get_VDFCLK(VOID);

/**
Get Frequency of SRO pipe1 input2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_SRO1CLK_2(VOID);

/**
Get Frequency of R2Y pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_R2Y1CLK(VOID);

/**
Get Frequency of LTM pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_LTM1CLK(VOID);

/**
Get Frequency of B2R pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_B2R1CLK(VOID);

/**
Get Frequency of CNR pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_CNR1CLK(VOID);

/**
Get Frequency of SRO pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_SRO1CLK(VOID);

/**
Get Frequency of B2B pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_B2B1CLK(VOID);

/**
Get Frequency of LTMRBK pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_LTMRBK1CLK(VOID);

/**
Get Frequency of SRO pipe2 input2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_SRO2CLK_2(VOID);

/**
Get Frequency of R2Y pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_R2Y2CLK(VOID);

/**
Get Frequency of LTM pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_LTM2CLK(VOID);

/**
Get Frequency of B2R pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_B2R2CLK(VOID);

/**
Get Frequency of CNR pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_CNR2CLK(VOID);

/**
Get Frequency of SRO pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_SRO2CLK(VOID);

/**
Get Frequency of B2B pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_B2B2CLK(VOID);

/**
Get Frequency of LTMRBK pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_LTMRBK2CLK(VOID);

/**
Get Frequency of APCLK clock
@retval	Hz
*/
ULONG Dd_Top_Get_APCLK(VOID);

/**
Get Frequency of Audio clock
@retval	Hz
*/
ULONG Dd_Top_Get_AUCLK(VOID);

/**
Get Frequency of SPI clock
@retval	Hz
*/
ULONG Dd_Top_Get_SPICLK(VOID);

/**
Get Frequency of DSP clock
@retval	Hz
*/
ULONG Dd_Top_Get_DSPCLK(VOID);

/**
Get Frequency of AXI bus clock
@retval	Hz
*/
ULONG Dd_Top_Get_ACLK(VOID);

/**
Get Frequency of AXI bus EXS clock
@retval	Hz
*/
ULONG Dd_Top_Get_ACLKEXS(VOID);

/**
Get Frequency of AHB bus BMH clock
@retval	Hz
*/
ULONG Dd_Top_Get_HCLKBMH(VOID);

/**
Get Frequency of AHB bus clock
@retval	Hz
*/
ULONG Dd_Top_Get_HCLK(VOID);

/**
Get Frequency of APB bus clock
@retval	Hz
*/
ULONG Dd_Top_Get_PCLK(VOID);

/**
Get Frequency of MXI bus 400 clock
@retval	Hz
*/
ULONG Dd_Top_Get_MCLK400(VOID);

/**
Get Frequency of MXI bus 200 clock
@retval	Hz
*/
ULONG Dd_Top_Get_MCLK200(VOID);

/**
Get Frequency of AXI bus 400 clock
@retval	Hz
*/
ULONG Dd_Top_Get_ACLK400(VOID);

/**
Get Frequency of RIBERY clock
@retval	Hz
*/
ULONG Dd_Top_Get_RIBCLK(VOID);

/**
Get Frequency of NF Bch clock
@retval	Hz
*/
ULONG Dd_Top_Get_NFBCHCLK(VOID);

/**
Get Frequency of ME clock
@retval	Hz
*/
ULONG Dd_Top_Get_MECLK(VOID);

/**
Get Frequency of FPT1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_FPT1CLK(VOID);

/**
Get Frequency of FPT0 clock
@retval	Hz
*/
ULONG Dd_Top_Get_FPT0CLK(VOID);

/**
Get Frequency of 3DNR clock
@retval	Hz
*/
ULONG Dd_Top_Get_SHDRCLK(VOID);

/**
Get Frequency of GYRO clock
@retval	Hz
*/
ULONG Dd_Top_Get_GYROCLK(VOID);

/**
Get Frequency of AXI bus 300 clock
@retval	Hz
*/
ULONG Dd_Top_Get_ACLK300(VOID);


/**
Control Clock Stop to start clock.<br>
@param[in,out]	counter		address of Garding counter
@param[in,out]	reg_addr	address of clock stop register
@param[in]		val			setting value
@remarks Set 1 to XXXSTOPC register internally.<br><br>
		*(reg_addr + D_DD_TOP_CLKSTOPC_OFFSET) |= ~val
*/
VOID Dd_Top_Start_Clock( UCHAR* counter, volatile unsigned long* reg_addr, unsigned long val );

/**
Control Clock Stop to stop clock.
@param[in,out]	counter		address of Garding counter
@param[in,out]	reg_addr	address of clock stop register
@param[in]		val			setting value
@remarks Set 1 to XXXSTOPS register internally.<br><br>
		*(reg_addr + D_DD_TOP_CLKSTOPS_OFFSET) |= val
*/
VOID Dd_Top_Stop_Clock( UCHAR* counter, volatile unsigned long* reg_addr, unsigned long val );

/**
Start PLL00.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll00(VOID);

/**
Stop PLL00.
*/
VOID Dd_Top_Stop_Pll00(VOID);

/**
Start PLL01.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll01(VOID);

/**
Stop PLL01.
*/
VOID Dd_Top_Stop_Pll01(VOID);

/**
Start PLL02.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll02(VOID);

/**
Stop PLL02.
*/
VOID Dd_Top_Stop_Pll02(VOID);

/**
Start PLL03.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll03(VOID);

/**
Stop PLL03.
*/
VOID Dd_Top_Stop_Pll03(VOID);

/**
Start PLL04.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll04(VOID);

/**
Stop PLL04.
*/
VOID Dd_Top_Stop_Pll04(VOID);

/**
Start PLL05.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll05(VOID);

/**
Stop PLL05.
*/
VOID Dd_Top_Stop_Pll05(VOID);

/**
Start PLL05A.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll05A(VOID);

/**
Stop PLL05A.
*/
VOID Dd_Top_Stop_Pll05A(VOID);

/**
Start PLL06.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll06(VOID);

/**
Stop PLL06.
*/
VOID Dd_Top_Stop_Pll06(VOID);

/**
Start PLL07.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll07(VOID);

/**
Stop PLL07.
*/
VOID Dd_Top_Stop_Pll07(VOID);

/**
Start PLL08.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll08(VOID);

/**
Stop PLL08.
*/
VOID Dd_Top_Stop_Pll08(VOID);

/**
Start PLL10.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll10(VOID);

/**
Stop PLL10.
*/
VOID Dd_Top_Stop_Pll10(VOID);

/**
Start PLL10A.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll10A(VOID);

/**
Stop PLL100A.
*/
VOID Dd_Top_Stop_Pll10A(VOID);

/**
Start PLL11.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Pll11(VOID);

/**
Stop PLL11.
*/
VOID Dd_Top_Stop_Pll11(VOID);

/**
Start DDR 00 PLL.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Ddr_Pll00(VOID);

/**
Stop DDR PLL00.
*/
VOID Dd_Top_Stop_Ddr_Pll00(VOID);

/**
Start DDR PLL01.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Ddr_Pll01(VOID);

/**
Stop DDR PLL01.
*/
VOID Dd_Top_Stop_Ddr_Pll01(VOID);

/**
Start DDR PLL02.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Ddr_Pll02(VOID);

/**
Stop DDR PLL02.
*/
VOID Dd_Top_Stop_Ddr_Pll02(VOID);

/**
Start DDR PLL10.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Ddr_Pll10(VOID);

/**
Stop DDR PLL10.
*/
VOID Dd_Top_Stop_Ddr_Pll10(VOID);

/**
Start DDR PLL11.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Ddr_Pll11(VOID);

/**
Stop DDR PLL11.
*/
VOID Dd_Top_Stop_Ddr_Pll11(VOID);

/**
Start DDR PLL12.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
INT32 Dd_Top_Start_Ddr_Pll12(VOID);

/**
Stop DDR PLL12.
*/
VOID Dd_Top_Stop_Ddr_Pll12(VOID);


/**
Set parameters for Audio PLL.
@param [in]	clock		select clock
@param [in]	select[]	ch0-ch3 select Audio Clock(Internal clock or External clock)<br>
						<ul><li>select[0]:Audio ch0 Clock
							<li>select[1]:Audio ch1 Clock
							<li>select[2]:Audio ch2 Clock
							<li>select[3]:Audio Ethernet Clock</ul>
@param [in]	pll_select	Audio PLL select
*/
VOID Dd_Top_Set_Aud_Pll(UCHAR clock, UCHAR const *const select, UCHAR pll_select);

/**
Get Frequency of Audio PLL.
@retval	Hz
*/
ULONG Dd_Top_Get_Aud_Pll(VOID);

/**
Set the functionality of a selectable GPIO port.<br>
It is set to the EPCR(External Port Control Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [in]  function	Function type.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_FUNC_GPIO
							  <li>1:@ref D_DD_TOP_GPIO_FUNC_HWMODE
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Input parameter error.
*/
INT32 Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PORT port, UCHAR function );

/**
Get the functionality of a selectable GPIO port.<br>
It is get from the EPCR(External Port Control Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [out] function	Function type.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_FUNC_GPIO
							  <li>1:@ref D_DD_TOP_GPIO_FUNC_HWMODE
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Input parameter error.
*/
INT32 Dd_Top_Get_Gpio_Function( E_DD_TOP_GPIO_PORT port, UCHAR* function );

/**
Set the direction of a selectable GPIO port.<br>
It is set to the DDR(Data Direction Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [in]  direction	Direction type.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_DIR_IN
							  <li>1:@ref D_DD_TOP_GPIO_DIR_OUT
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Set_Gpio_Direction( E_DD_TOP_GPIO_PORT port, UCHAR direction );

/**
Get the direction of a selectable GPIO port.<br>
It is get from the DDR(Data Direction Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [out] direction	Direction type.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_DIR_IN
							  <li>1:@ref D_DD_TOP_GPIO_DIR_OUT
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Get_Gpio_Direction( E_DD_TOP_GPIO_PORT port, UCHAR* direction );

/**
Set the state of a selectable GPIO port.<br>
It is set to the PDR(Port Data Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [in]  status		Port status.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_STATUS_LOW
							  <li>1:@ref D_DD_TOP_GPIO_STATUS_HIGH
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_PORT port, UCHAR status );

/**
Get the state of a selectable GPIO port.<br>
It is get from the PDR(Port Data Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [out] status		Port status.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_STATUS_LOW
							  <li>1:@ref D_DD_TOP_GPIO_STATUS_HIGH
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Get_Gpio_Status( E_DD_TOP_GPIO_PORT port, UCHAR* status );

#if 1    // special key control
INT32 Dd_Top_Set_Gpio_CM0_Function( E_DD_TOP_GPIO_CM0_PORT port, UCHAR function );
INT32 Dd_Top_Get_Gpio_CM0_Function( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* function );
INT32 Dd_Top_Set_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_PORT port, UCHAR direction );
INT32 Dd_Top_Get_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* direction );
INT32 Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Enable( E_DD_TOP_GPIO_CM0_PORT port, UCHAR enable );
INT32 Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Enable( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* enable );
INT32 Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_CM0_PORT port, UCHAR control );
INT32 Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* control );
INT32 Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_PORT port, UCHAR status );
/**
Get the state of a selectable GPIO CM0 port.<br>
It is get from the PDR(Port Data Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_CM0_PORT
@param [out] status		Port status.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_STATUS_LOW
							  <li>1:@ref D_DD_TOP_GPIO_STATUS_HIGH
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Get_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* status );
#endif

/**
Set the Pull-up/down enable state of a selectable GPIO port.<br>
It is set to the PUDER(Pull-up/down Enable Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [in]  enable		Pull-up/down enable state.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_PUD_OFF
							  <li>1:@ref D_DD_TOP_GPIO_PUD_ON
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Set_Gpio_Pull_Up_Down_Enable( E_DD_TOP_GPIO_PORT port, UCHAR enable );

/**
Get the Pull-up/down enable state of a selectable GPIO port.<br>
It is get from the PUDER(Pull-up/down Enable Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [out] enable		Pull-up/down enable state.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_PUD_OFF
							  <li>1:@ref D_DD_TOP_GPIO_PUD_ON
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Get_Gpio_Pull_Up_Down_Enable( E_DD_TOP_GPIO_PORT port, UCHAR* enable );

/**
Set the Pull-up/down state of a selectable GPIO port.<br>
It is set to the PUDCR(Pull-up/down Control Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [in]  control	Pull-up/down state.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_PUD_DOWN
							  <li>1:@ref D_DD_TOP_GPIO_PUD_UP
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_PORT port, UCHAR control );

/**
Get the Pull-up/down state of a selectable GPIO port.<br>
It is get from the PUDCR(Pull-up/down Control Register) value.
@param [in]  port		Port name. See @ref E_DD_TOP_GPIO_PORT
@param [out] control	Pull-up/down state.
						  <ul>
							  <li>0:@ref D_DD_TOP_GPIO_PUD_DOWN
							  <li>1:@ref D_DD_TOP_GPIO_PUD_UP
						  </ul>
@retval D_DDIM_OK					Success.
@retval D_DD_TOP_INPUT_PARAM_ERROR	Error occurred.
*/
INT32 Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_PORT port, UCHAR* control );

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
