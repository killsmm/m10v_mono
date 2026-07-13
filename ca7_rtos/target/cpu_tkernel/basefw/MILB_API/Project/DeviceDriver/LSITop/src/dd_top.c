/**
 * @file		dd_top.c
 * @brief		CHIPTOP driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_top.h"
#include "dd_arm.h"
#include "driver_common.h"
#include "ddim_user_custom.h"
#if 1    // special key control
#include "pmc.h"
#include <string.h>
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_DD_TOP_PDR_WR_EN_B16	( 1 << 16 )
#define D_DD_TOP_PDR_WR_EN_B17	(( 1 << 1 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B18	(( 1 << 2 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B19	(( 1 << 3 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B20	(( 1 << 4 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B21	(( 1 << 5 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B22	(( 1 << 6 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B23	(( 1 << 7 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B24	(( 1 << 8 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B25	(( 1 << 9 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B26	(( 1 << 10 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B27	(( 1 << 11 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B28	(( 1 << 12 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B29	(( 1 << 13 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B30	(( 1 << 14 ) << 16 )
#define D_DD_TOP_PDR_WR_EN_B31	(( 1 << 15 ) << 16 )



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
ULONG gDD_Top_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_PCLOCK
#define CO_ACT_GPIO_CLOCK_CONTROL
#endif //CO_ACT_PCLOCK

#ifdef CO_ACT_GPIO_CLOCK_CONTROL
#define Dd_Top_Start_GPIO_CLK_WITH_CS()	{Dd_ARM_Critical_Section_Start(gDD_Top_Spin_Lock)Dd_Top_Set_CLKSTOP_GPIOAP(0);(void)IO_CHIPTOP.PDR.bit.P30;Dd_ARM_Dsb_Pou();}
#define Dd_Top_Stop_GPIO_CLK_WITH_CS()	{Dd_Top_Set_CLKSTOP_GPIOAP(1);Dd_ARM_Critical_Section_End(gDD_Top_Spin_Lock);}
#else
#define Dd_Top_Start_GPIO_CLK_WITH_CS()
#define Dd_Top_Stop_GPIO_CLK_WITH_CS()
#endif // CO_ACT_GPIO_CLOCK_CONTROL

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
Get Frequency of eMMC clock
@retval	Hz
*/
ULONG Dd_Top_Get_EMMCCLK(VOID)
{
	// 0:187.5MHz(=1500MHz/8)
	// 1:166.666MHz(=1500MHz/9)
	// 2:150MHz(=1500MHz/10)
	// 3:100MHz(=1500MHz/15)
	static const UCHAR divid[] = {8, 9, 10, 15};

	return (D_DD_TOP_1500_PLL_OUT / divid[Dd_Top_Get_CLKSEL1_EMMCCLK()]);
}

/**
Get Frequency of NAND Flash clock
@retval	Hz
*/
ULONG Dd_Top_Get_NFCLK(VOID)
{
	// 0:100MHz 	(=1600MHz/16)
	// 1:80MHz 		(=1600MHz/20)
	// 2:61.538MHz 	(=1600MHz/26)
	// 3:50MHz 		(=1600MHz/32)
	// 4:20MHz 		(=1600MHz/80)
	// 8:32MHz 		(=1600MHz/50)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 8, 255};
	static const UCHAR divid[] = {16, 20, 26, 32, 80, 50};
	int i;

	ULONG clock = 0;
	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL1_NFCLK() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}
	return clock;
}

/**
Get Frequency of UHS2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_UHS2CLK(VOID)
{
	// 0: 51.851MHz(=1400MHz/27)
	// 1: 46.666MHz(=1400MHz/30)
	// 2: 42.424MHz(=1400MHz/33)
	// 3: 38.888MHz(=1400MHz/36)
	// 4: 35.897MHz(=1400MHz/39)
	// 5: 33.333MHz(=1400MHz/42)
	// 6: 29.166MHz(=1400MHz/48)
	// 7: 25.925MHz(=1400MHz/54)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 6, 7, 255};
	static const UCHAR divid[] = {27, 30, 33, 36, 39, 42, 48, 54};
	int i;

	ULONG clock = 0;
	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL1_UHS2CLK() == id[i]) {
			clock = (D_DD_TOP_1400_PLL_OUT / divid[i]);
			break;
		}
	}
	return clock;
}

/**
Get Frequency of UHS1 ch2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_UHS1CLK2(VOID)
{
	// 2: 400MHz(=1600MHz/4)
	// 3: 200MHz(=1600MHz/8)
	// 4: 100MHz(=1600MHz/16)
	// 8: 700MHz(=1400MHz/2)
	static const UCHAR id[] = {2, 3, 4, 8, 255};
	static const UCHAR divid[] = {4, 8, 16, 2};
	static const ULONG pllid[] = {D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, 
									D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1400_PLL_OUT};
	int i;

	ULONG clock = 0;
	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL1_UHS1CLK2() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}
	return clock;
}

/**
Get Frequency of UHS1 ch1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_UHS1CLK1(VOID)
{
	// 3: 200MHz(=1600MHz/8)
	// 4: 100MHz(=1600MHz/16)
	// 8: 700MHz(=1400MHz/2)
	static const UCHAR id[] = {3, 4, 8, 255};
	static const UCHAR divid[] = {8, 16, 2};
	static const ULONG pllid[] = {D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, 
									D_DD_TOP_1400_PLL_OUT};
	int i;

	ULONG clock = 0;
	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL1_UHS1CLK1() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}
	return clock;
}

/**
Get Frequency of UHS1 ch0 clock
@retval	Hz
*/
ULONG Dd_Top_Get_UHS1CLK0(VOID)
{
	// 0: 800MHz(=1600MHz/2)
	// 1: 533.333MHz (=1600MHz/3)
	// 2: 400MHz(=1600MHz/4)
	// 3: 200MHz(=1600MHz/8)
	// 4: 100MHz(=1600MHz/16)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 255};
	static const UCHAR divid[] = {2, 3, 4, 8, 16};
	static const ULONG pllid[] = {D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, 
									D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, 
									D_DD_TOP_1600_PLL_OUT};
	int i;

	ULONG clock = 0;
	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL1_UHS1CLK0() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}
	return clock;
}

/**
Get Frequency of Cortex-A7 MPCore Peri clock
@retval	Hz
*/
ULONG Dd_Top_Get_RCLK(VOID)
{
	// 0:54MHz(=864MHz/16)
	// 1:27MHz(=864MHz/32)
	// 2:18MHz(=864MHz/48)
	// 3:13.5MHz(=864MHz/64)
	static const UCHAR divid[] = {16, 32, 48, 64};

	return (D_DD_TOP_864_PLL_OUT / divid[Dd_Top_Get_CLKSEL1_RCLK()]);
}

/**
Get Frequency of RAW clock
@retval	Hz
*/
ULONG Dd_Top_Get_RAWCLK(VOID)
{
	// 0: 200MHz(=1600MHz/8)
	// 1: 160MHz(=1600MHz/10)
	// 2: 100MHz(=1600MHz/16)
	// 3: 50MHz(=1600MHz/32)
	static const UCHAR divid[] = {8, 10, 16, 32};

	return (D_DD_TOP_1600_PLL_OUT / divid[Dd_Top_Get_CLKSEL2_RAWCLK()]);
}

/**
Get Frequency of DISP HIF clock
@retval	Hz
*/
ULONG Dd_Top_Get_HIFCLK(VOID)
{
	// 0:594MHz(=1188MHz/2)
	// 1:297MHz(=1188MHz/4)
	// 2:148.5MHz(=1188MHz/8)
	// 3:74.25MHz(=1188MHz/16)
	// 4:27MHz(=1188MHz/44)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 255};
	static const UCHAR divid[] = {2, 4, 8, 16, 44};
	int i;

	ULONG clock = 0;
	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL2_HIFCLK() == id[i]) {
			clock = (D_DD_TOP_1188_PLL_OUT / divid[i]);
			break;
		}
	}
	return clock;
}

/**
Get Frequency of DISP MIF clock
@retval	Hz
*/
ULONG Dd_Top_Get_MIFCLK(VOID)
{
	// 0: 169.714MHz (=1188MHz/7)
	// 1: 148.5MHz (=1188MHz/8)
	// 2: 108MHz (=1188MHz/11)
	// 3: 74.25MHz (=1188MHz/16)
	// 4: 66MHz (=1188MHz/18)
	// 5: 54MHz (=1188MHz/22)
	// 6: 49.5MHz (=1188MHz/24)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 6, 255};
	static const UCHAR divid[] = {7, 8, 11, 16, 18, 22, 24};
	int i;

	ULONG clock = 0;
	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL2_MIFCLK() == id[i]) {
			clock = (D_DD_TOP_1188_PLL_OUT / divid[i]);
			break;
		}
	}
	return clock;
}

/**
Get Frequency of IPU system clock
@retval	Hz
*/
ULONG Dd_Top_Get_IPUCLK(VOID)
{
	// 0: 400MHz (=1600MHz/4/(8/8))
	// 1: 50MHz  (=1600MHz/4/(1/8))
	// 2: 100MHz (=1600MHz/4/(2/8))
	// 3: 150MHz (=1600MHz/4/(3/8))
	// 4: 200MHz (=1600MHz/4/(4/8))
	// 5: 250MHz (=1600MHz/4/(5/8))
	// 6: 300MHz (=1600MHz/4/(6/8))
	// 7: 350MHz (=1600MHz/4/(7/8))
	static const UCHAR divid[] = {8, 1, 2, 3, 4, 5, 6, 7};

	return (D_DD_TOP_1600_PLL_OUT / 32  * divid[Dd_Top_Get_CLKSEL2_IPUCLK()]);
}

/**
Get Frequency of IPU TME clock
@retval	Hz
*/
ULONG Dd_Top_Get_IPUTMECLK(VOID)
{
	// 0:500MHz   (=1500MHz/3/(8/8))
	// 1:62.5MHz  (=1500MHz/3/(1/8))
	// 2:125MHz   (=1500MHz/3/(2/8))
	// 3:187.5MHz (=1500MHz/3/(3/8))
	// 4:250MHz   (=1500MHz/3/(4/8))
	// 5:312.5MHz (=1500MHz/3/(5/8))
	// 6:375MHz   (=1500MHz/3/(6/8))
	// 7:437.5MHz (=1500MHz/3/(7/8))
	static const UCHAR divid[] = {8, 1, 2, 3, 4, 5, 6, 7};

	return (D_DD_TOP_1500_PLL_OUT / 24  * divid[Dd_Top_Get_CLKSEL2_IPUTMECLK()]);
}
/**
Get Frequency of GPU clock
@retval	Hz
*/
ULONG Dd_Top_Get_GPUCLK(VOID)
{
	// 0:466.666MHz(=1400MHz/3/(8/8))
	// 1:58.333MHz(=1400MHz/3/(1/8))
	// 2:116.666MHz(=1400MHz/3/(2/8))
	// 3:175MHz(=1400MHz/3/(3/8))
	// 4:233.333MHz(=1400MHz/3/(4/8))
	// 5:291.666MHz(=1400MHz/3/(5/8))
	// 6:350MHz(=1400MHz/3/(6/8))
	// 7:408.333MHz(=1400MHz/3/(7/8))
	static const UCHAR divid[] = {8, 1, 2, 3, 4, 5, 6, 7};

	return (D_DD_TOP_1400_PLL_OUT / 24  * divid[Dd_Top_Get_CLKSEL2_GPUCLK()]);
}

/**
Get Frequency of JPEG clock
@retval	Hz
*/
ULONG Dd_Top_Get_JPEGCLK(VOID)
{
	// 0:200MHz(=1600MHz/8/(8/8))
	// 1:25MHz(=1600MHz/8/(1/8))
	// 2:50MHz(=1600MHz/8/(2/8))
	// 3:75MHz(=1600MHz/8/(3/8))
	// 4:100MHz(=1600MHz/8/(4/8))
	// 5:125MHz(=1600MHz/8/(5/8))
	// 6:150MHz(=1600MHz/8/(6/8))
	// 7:175MHz(=1600MHz/8/(7/8))
	static const UCHAR divid[] = {8, 1, 2, 3, 4, 5, 6, 7};

	return (D_DD_TOP_1600_PLL_OUT / 64  * divid[Dd_Top_Get_CLKSEL2_JPGCLK()]);
}

/**
Get Frequency of ELA clock
@retval	Hz
*/
ULONG Dd_Top_Get_ELACLK(VOID)
{
	// 0:300MHz(=1200MHz/4)
	// 1:200MHz(=1200MHz/6)
	// 2:150MHz(=1200MHz/8)
	// 3:100MHz(=1200MHz/12)
	// 4:75MHz(=1200MHz/16)
	// 5:60MHz(=1200MHz/20)
	// 6:50MHz(=1200MHz/24)
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};

	return (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL2_ELACLK()]);
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
Get type of image pipe macro Select
@retval	Select value
*/
ULONG Dd_Top_Get_HEVENCSEL(VOID)
{
	// 0:500MHz(=1500MHz/3)
	// 1:HEVENCCLK

	return Dd_Top_Get_CLKSEL3_HEVENCSEL();
}

// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
/**
Get type of image pipe macro Select
@retval	Select value
*/
ULONG Dd_Top_Get_PIPESEL(VOID)
{
	// 10:Use PLL02 (300MHz`50MHz)
	// 11:Use PLL01 (266.666MHz`66.666MHz)

	return Dd_Top_Get_CLKSEL3_PIPESEL();
}

/**
Get Frequency of SENSOR clock
@retval	Hz
*/
ULONG Dd_Top_Get_SENCLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 6, 255};
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL3_SENCLK() == id[i]) {
			clock = (D_DD_TOP_1200_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of SENSOR clock
@retval	Hz
*/
ULONG Dd_Top_Get_SENMSKCLK(VOID)
{
	// 0:100MHz(=1600MHz/16)
	// 1:80MHz(=1600MHz/20)
	static const UCHAR divid[] = {16, 20};

	return (D_DD_TOP_1600_PLL_OUT / divid[Dd_Top_Get_CLKSEL3_SENMSKCLK()]);
}

/**
Get Frequency of IIP clock
@retval	Hz
*/
ULONG Dd_Top_Get_IIPCLK(VOID)
{
	// 0:300MHz(=1200MHz/4)
	// 1:200MHz(=1200MHz/6)
	// 2:150MHz(=1200MHz/8)
	// 3:100MHz(=1200MHz/12)
	// 4:75MHz(=1200MHz/16)
	// 5:60MHz(=1200MHz/20)
	// 6:50MHz(=1200MHz/24)
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};

	return (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL3_IIPCLK()]);
}

/**
Get Frequency of STAT clock
@retval	Hz
*/
ULONG Dd_Top_Get_PASCLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)v
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 6, 255};
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL3_PASCLK() == id[i]) {
			clock = (D_DD_TOP_1200_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of HEVC IPP clock
@retval	Hz
*/
ULONG Dd_Top_Get_IPPCLK(VOID)
{
	// 0:800MHz    (=1600MHz/2)
	// 1:533.333MHz(=1600MHz/3)
	// 2:400MHz    (=1600MHz/4)
	// 3:200MHz    (=1600MHz/8)
	// 4:700MHz    (=1400MHz/2)
	// 8:600MHz    (=1200MHz/2)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 8, 255};
	static const UCHAR divid[] = {2, 3, 4, 8, 2, 2};
	static const ULONG pllid[] = {D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, 
									D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1400_PLL_OUT, D_DD_TOP_1200_PLL_OUT};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL3_IPPCLK() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}

	return clock;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
Get Frequency of HEVC PXF clock
@retval	Hz
*/
ULONG Dd_Top_Get_PXFCLK(VOID)
{
	// 0: 700MHz(=1400MHz/2)
	// 1: 280MHz(=1400MHz/5)
	// 2: 155.555MHz(=1400MHz/9)
	// 4: 533.333MHz(=1600MHz/3)
	// 5: 400MHz(=1600MHz/4)
	// 6: 100MHz(=1600MHz/16)
	static const UCHAR id[] = {0, 1, 2, 4, 5, 6, 255};
	static const UCHAR divid[] = {2, 5, 9, 3, 4, 16};
	static const ULONG pllid[] = {D_DD_TOP_1400_PLL_OUT, D_DD_TOP_1400_PLL_OUT, D_DD_TOP_1400_PLL_OUT, 
									D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL3_PXFCLK() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}

	return clock;
}
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
ULONG Dd_Top_Get_HEVENCCLK(VOID)
{
	// 0:466.666MHz(=1400MHz/3)
	// 1:350MHz(=1400MHz/4)
	// 2:280MHz(=1400MHz/5)
	// 4:200MHz(=1400MHz/7)
	// 5:100MHz(=1400MHz/14)
	static const UCHAR id[] = {0, 1, 2, 4, 5, 255};
	static const UCHAR divid[] = {3, 4, 5, 7, 14};
	static const ULONG pllid[] = {D_DD_TOP_1400_PLL_OUT, D_DD_TOP_1400_PLL_OUT, D_DD_TOP_1400_PLL_OUT, 
									D_DD_TOP_1400_PLL_OUT, D_DD_TOP_1400_PLL_OUT};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL3_HEVENCCLK() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}

	return clock;
}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/**
Get Frequency of HEVC VDF clock
@retval	Hz
*/
ULONG Dd_Top_Get_VDFCLK(VOID)
{
	// 0: 700MHz(=1400MHz/2)
	// 1: 280MHz(=1400MHz/5)
	// 2: 155.555MHz(=1400MHz/9)
	// 4: 533.333MHz(=1600MHz/3)
	// 5: 400MHz(=1600MHz/4)
	// 6: 100MHz(=1600MHz/16)
	static const UCHAR id[] = {0, 1, 2, 4, 5, 6, 255};
	static const UCHAR divid[] = {2, 5, 9, 3, 4, 16};
	static const ULONG pllid[] = {D_DD_TOP_1400_PLL_OUT, D_DD_TOP_1400_PLL_OUT, D_DD_TOP_1400_PLL_OUT, 
									D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL3_VDFCLK() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of SRO pipe1 input2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_SRO1CLK_2(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 6, 255};
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL4_SRO1CLK_2() == id[i]) {
			clock = (D_DD_TOP_1200_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of R2Y pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_R2Y1CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};

	return (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL4_R2Y1CLK()]);
}

/**
Get Frequency of LTM pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_LTM1CLK(VOID)
{
	// 0:  300MHz(=1200MHz/4)
	// 1:  200MHz(=1200MHz/6)
	// 2:  150MHz(=1200MHz/8)
	// 3:  100MHz(=1200MHz/12)
	// 4:  75MHz(=1200MHz/16)
	// 5:  60MHz(=1200MHz/20)
	// 6:  50MHz(=1200MHz/24)
	// 8:  SRO1CLK
	// 16: SRO1CLK_2
	// 24: B2B1CLK
	// 32: B2R1CLK
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL4_LTM1CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL4_LTM1CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_SRO1CLK();
			break;
		case 16:
			clock = Dd_Top_Get_SRO1CLK_2();
			break;
		case 24:
			clock = Dd_Top_Get_B2B1CLK();
			break;
		case 32:
			clock = Dd_Top_Get_B2R1CLK();
			break;
	}

	return clock;
}

/**
Get Frequency of B2R pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_B2R1CLK(VOID)
{
	// 0:  300MHz(=1200MHz/4)
	// 1:  200MHz(=1200MHz/6)
	// 2:  150MHz(=1200MHz/8)
	// 3:  100MHz(=1200MHz/12)
	// 4:  75MHz(=1200MHz/16)
	// 5:  60MHz(=1200MHz/20)
	// 6:  50MHz(=1200MHz/24)
	// 8:  SRO1CLK
	// 16: SRO1CLK_2
	// 24: B2B1CLK
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL4_B2R1CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL4_B2R1CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_SRO1CLK();
			break;
		case 16:
			clock = Dd_Top_Get_SRO1CLK_2();
			break;
		case 24:
			clock = Dd_Top_Get_SRO1CLK_2();
			break;
		case 32:
			clock = Dd_Top_Get_B2B1CLK();
			break;
	}

	return clock;
}

/**
Get Frequency of CNR pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_CNR1CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	// 8: R2Y1CLK
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL4_CNR1CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL4_CNR1CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_R2Y1CLK();
			break;
	}

	return clock;
}

/**
Get Frequency of SRO pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_SRO1CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};

	return (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL5_SRO1CLK()]);
}

/**
Get Frequency of B2B pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_B2B1CLK(VOID)
{
	// 0:  300MHz(=1200MHz/4)
	// 1:  200MHz(=1200MHz/6)
	// 2:  150MHz(=1200MHz/8)
	// 3:  100MHz(=1200MHz/12)
	// 4:  75MHz(=1200MHz/16)
	// 5:  60MHz(=1200MHz/20)
	// 6:  50MHz(=1200MHz/24)
	// 8:  SRO1CLK
	// 16: SRO1CLK_2
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL5_B2B1CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL5_B2B1CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_SRO1CLK();
			break;
		case 16:
			clock = Dd_Top_Get_SRO1CLK_2();
			break;
	}

	return clock;
}

/**
Get Frequency of LTMRBK pipe1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_LTMRBK1CLK(VOID)
{
	// 0:  300MHz(=1200MHz/4)
	// 1:  200MHz(=1200MHz/6)
	// 2:  150MHz(=1200MHz/8)
	// 3:  100MHz(=1200MHz/12)
	// 4:  75MHz(=1200MHz/16)
	// 5:  60MHz(=1200MHz/20)
	// 6:  50MHz(=1200MHz/24)
	// 8:  SRO1CLK
	// 16: SRO1CLK_2
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL5_LTMRBK1CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL5_LTMRBK1CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_SRO1CLK();
			break;
		case 16:
			clock = Dd_Top_Get_SRO1CLK_2();
			break;
	}

	return clock;
}

/**
Get Frequency of SRO pipe2 input2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_SRO2CLK_2(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 6, 255};
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL6_SRO2CLK_2() == id[i]) {
			clock = (D_DD_TOP_1200_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of R2Y pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_R2Y2CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};

	return (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL6_R2Y2CLK()]);
}

/**
Get Frequency of LTM pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_LTM2CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	// 8: SRO2CLK
	// 16:SRO2CLK_2
	// 24:B2B2CLK
	// 32:B2R2CLK
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL6_LTM2CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL6_LTM2CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_SRO2CLK();
			break;
		case 16:
			clock = Dd_Top_Get_SRO2CLK_2();
			break;
		case 24:
			clock = Dd_Top_Get_B2B2CLK();
			break;
		case 32:
			clock = Dd_Top_Get_B2R2CLK();
			break;
	}

	return clock;
}

/**
Get Frequency of B2R pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_B2R2CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	// 8: SRO2CLK
	// 16:SRO2CLK_2
	// 24:B2B2CLK
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL6_B2R2CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL6_B2R2CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_SRO2CLK();
			break;
		case 16:
			clock = Dd_Top_Get_SRO2CLK_2();
			break;
		case 24:
			clock = Dd_Top_Get_B2B2CLK();
			break;
	}

	return clock;
}

/**
Get Frequency of CNR pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_CNR2CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	// 8: R2Y2CLK
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL6_CNR2CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL6_CNR2CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_R2Y2CLK();
			break;
	}

	return clock;
}

/**
Get Frequency of SRO pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_SRO2CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};

	return (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL7_SRO2CLK()]);
}

/**
Get Frequency of B2B pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_B2B2CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	// 8 :SRO2CLK
	// 16:SRO2CLK_2
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL7_B2B2CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL7_B2B2CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_SRO2CLK();
			break;
		case 16:
			clock = Dd_Top_Get_SRO2CLK_2();
			break;
	}

	return clock;
}

/**
Get Frequency of LTMRBK pipe2 clock
@retval	Hz
*/
ULONG Dd_Top_Get_LTMRBK2CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	// 8 :SRO2CLK
	// 16:SRO2CLK_2
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	ULONG clock = 0;

	switch (Dd_Top_Get_CLKSEL7_LTMRBK2CLK()) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			clock = (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL7_LTMRBK2CLK()]);
			break;
		case 8:
			clock = Dd_Top_Get_SRO2CLK();
			break;
		case 16:
			clock = Dd_Top_Get_SRO2CLK_2();
			break;
	}

	return clock;
}

/**
Get Frequency of APCLK clock
@retval	Hz
*/
ULONG Dd_Top_Get_APCLK(VOID)
{
	// 0: 74.25MHz(=1188MHz/16)
	// 1: 54MHz(=1188MHz/22)
	// 2: 37.125MHz(=1188MHz/32)
	// 3: 27MHz(=1188MHz/44)
	// 4: 72MHz(=864MHz/12)
	// 5: 48MHz(=864MHz/18)
	// 8: 64MHz(=1600MHz/25)
	// 9: 45.714MHz(=1600MHz/35)
	// 12: 60MHz(=1500MHz/25)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 8, 9, 12, 255};
	static const UCHAR divid[] = {16, 22, 32, 44, 12, 18, 25, 35, 25};
	static const ULONG pllid[] = {D_DD_TOP_1188_PLL_OUT, D_DD_TOP_1188_PLL_OUT, D_DD_TOP_1188_PLL_OUT, 
									D_DD_TOP_1188_PLL_OUT, D_DD_TOP_864_PLL_OUT, D_DD_TOP_864_PLL_OUT, 
									D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1500_PLL_OUT};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL8_APCLK() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of Audio clock
@retval	Hz
*/
ULONG Dd_Top_Get_AUCLK(VOID)
{
	// 0:36.864MHz (set to 96 PL10ADIV)
	// 1:22.5792MHz (set to 98 PL10ADIV)
	// 2:24.576MHz (set to 128 PL10ADIV)
	// 3:8.192MHz (set to 128 PL10ADIV)
	static const UCHAR divid[] = {18, 30, 36, 108};
	static const ULONG plld[] = {D_DD_TOP_Audio_96_PLL_OUT, D_DD_TOP_Audio_98_PLL_OUT, D_DD_TOP_Audio_128_PLL_OUT, D_DD_TOP_Audio_128_PLL_OUT};

	return (plld[Dd_Top_Get_CLKSEL8_AUCLK()] / divid[Dd_Top_Get_CLKSEL8_AUCLK()]);

}

/**
Get Frequency of SPI clock
@retval	Hz
*/
ULONG Dd_Top_Get_SPICLK(VOID)
{
	// 0:216MHz(=864MHz/4)
	// 1:108MHz(=864MHz/8)
	// 2:100MHz(=1600MHz/16)
	static const UCHAR id[] = {0, 1, 2, 255};
	static const UCHAR divid[] = {4, 8, 16};
	static const ULONG pllid[] = {D_DD_TOP_864_PLL_OUT, D_DD_TOP_864_PLL_OUT, D_DD_TOP_1600_PLL_OUT};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL8_SPICLK() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of DSP clock
@retval	Hz
*/
ULONG Dd_Top_Get_DSPCLK(VOID)
{
	// 0:400MHz(=1600MHz/4)
	// 1:266.666MHz(=1600MHz/6)
	// 2:200MHz(=1600MHz/8)
	// 3:100MHz(=1600MHz/16)
	static const UCHAR divid[] = {4, 6, 8, 16};

	return (D_DD_TOP_1600_PLL_OUT / divid[Dd_Top_Get_CLKSEL8_DSPCLK()]);
}

/**
Get Frequency of AXI bus clock
@retval	Hz
*/
ULONG Dd_Top_Get_ACLK(VOID)
{
	// 3:200MHz(=1600MHz/8)
	// 7:100MHz(=1600MHz/16)
	static const UCHAR id[] = {3, 7, 255};
	static const UCHAR divid[] = {8, 16};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL9_ACLK() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of AXI bus EXS clock
@retval	Hz
*/
ULONG Dd_Top_Get_ACLKEXS(VOID)
{
	// 3:200MHz(=1600MHz/8)
	// 4:160MHz(=1600MHz/10)
	// 5:133.333MHz(=1600MHz/12)
	// 7:100MHz(=1600MHz/16)
	static const UCHAR id[] = {3, 4, 5, 7, 255};
	static const UCHAR divid[] = {8, 10, 12, 16};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL9_ACLKEXS() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of AHB bus BMH clock
@retval	Hz
*/
ULONG Dd_Top_Get_HCLKBMH(VOID)
{
	// 3:200MHz(=1600MHz/8)
	// 7:100MHz(=1600MHz/16)
	static const UCHAR id[] = {3, 7, 255};
	static const UCHAR divid[] = {8, 16};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL9_HCLKBMH() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of AHB bus clock
@retval	Hz
*/
ULONG Dd_Top_Get_HCLK(VOID)
{
	// 7:100MHz(=1600MHz/16)
	// 15:50MHz(=1600MHz/32)
	static const UCHAR id[] = {7, 15, 255};
	static const UCHAR divid[] = {16, 32};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL9_HCLK() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of APB bus clock
@retval	Hz
*/
ULONG Dd_Top_Get_PCLK(VOID)
{
	// 15:50MHz(=1600MHz/32)
	// 31:25MHz(=1600MHz/64)
	static const UCHAR id[] = {15, 31, 255};
	static const UCHAR divid[] = {32, 64};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL9_PCLK() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of MXI bus 400 clock
@retval	Hz
*/
ULONG Dd_Top_Get_MCLK400(VOID)
{
	// 1:400MHz(=1600MHz/4)
	// 3:200MHz(=1600MHz/8)
	static const UCHAR id[] = {1, 3, 255};
	static const UCHAR divid[] = {4, 8};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL10_MCLK400() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of MXI bus 200 clock
@retval	Hz
*/
ULONG Dd_Top_Get_MCLK200(VOID)
{
	// 3:200MHz(=1600MHz/8)
	// 7:100MHz(=1600MHz/16)
	static const UCHAR id[] = {3, 7, 255};
	static const UCHAR divid[] = {8, 16};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL10_MCLK200() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of AXI bus 400 clock
@retval	Hz
*/
ULONG Dd_Top_Get_ACLK400(VOID)
{
	// 1:400MHz(=1600MHz/4)
	// 3:200MHz(=1600MHz/8)
	static const UCHAR id[] = {1, 3, 255};
	static const UCHAR divid[] = {4, 8};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL10_ACLK400() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of 3DNR clock
@retval	Hz
*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
ULONG Dd_Top_Get_SHDRCLK(VOID)
{
	// 0: 400MHz(=1600MHz/4)
	// 1: 266.666MHz(=1600MHz/6)
	// 2: 200MHz(=1600MHz/8)
	// 3: 160MHz(=1600MHz/10)
	// 4: 133.333MHz(=1600MHz/12)
	// 5: 100MHz(=1600MHz/16)
	// 6: 80MHz(=1600MHz/20)
	// 7: 66.666MHz(=1600MHz/24)
	static const UCHAR divid[] = {4, 6, 8, 10, 12, 16, 20, 24};

	return (D_DD_TOP_1600_PLL_OUT / divid[Dd_Top_Get_CLKSEL12_SHDRCLK()]);
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
ULONG Dd_Top_Get_SHDRCLK(VOID)
{
	// 0: 432MHz(=864MHz/2)
	// 8: 400MHz(=1600MHz/4)
	// 9: 266.666MHz(=1600MHz/6)
	// 10: 200MHz(=1600MHz/8)
	// 11: 160MHz(=1600MHz/10)
	// 12: 133.333MHz(=1600MHz/12)
	// 13: 100MHz(=1600MHz/16)
	// 14: 80MHz(=1600MHz/20)
	// 15: 66.666MHz(=1600MHz/24)
	static const UCHAR id[] = {0, 8, 9, 10, 11, 12, 13, 14, 15, 255};
	static const UCHAR divid[] = {2, 4, 6, 8, 10, 12, 16, 20, 24};
	static const ULONG pllid[] = {D_DD_TOP_864_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT, D_DD_TOP_1600_PLL_OUT};

	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL12_SHDRCLK() == id[i]) {
			clock = (pllid[i] / divid[i]);
			break;
		}
	}

	return clock;
}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/**
Get Frequency of RIBERY clock
@retval	Hz
*/
ULONG Dd_Top_Get_RIBCLK(VOID)
{
	// 0:466.666MHz(=1400MHz/3)
	// 1:350MHz(=1400MHz/4)
	// 2:233.333MHz(=1400MHz/6)
	// 3:116.666MHz(=1400MHz/12)
	// 4:58.333MHz(=1400MHz/24)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 255};
	static const UCHAR divid[] = {3, 4, 6, 12, 24};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL12_RIBCLK() == id[i]) {
			clock = (D_DD_TOP_1400_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}


/**
Get Frequency of NFBCHCLK clock
@retval	Hz
*/
ULONG Dd_Top_Get_NFBCHCLK(VOID)
{
	// 0:200MHz(=1600MHz/8)
	// 1:100MHz(=1600MHz/16)
	static const UCHAR divid[] = {8, 16};

	return (D_DD_TOP_1600_PLL_OUT / divid[Dd_Top_Get_CLKSEL12_NFBCHCLK()]);
}

/**
Get Frequency of MECLK clock
@retval	Hz
*/
ULONG Dd_Top_Get_MECLK(VOID)
{
	// 0: 466.666MHz(=1400MHz/3)
	// 1: 233.333MHz(=1400MHz/6)
	// 2: 116.666MHz(=1400MHz/12)
	// 3: 58.333MHz(=1400MHz/24)
	static const UCHAR divid[] = {3, 6, 12, 24};

	return (D_DD_TOP_1400_PLL_OUT / divid[Dd_Top_Get_CLKSEL12_MECLK()]);
}

/**
Get Frequency of FPT1 clock
@retval	Hz
*/
ULONG Dd_Top_Get_FPT1CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 6, 255};
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL12_FPT1CLK() == id[i]) {
			clock = (D_DD_TOP_1200_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of FPT0 clock
@retval	Hz
*/
ULONG Dd_Top_Get_FPT0CLK(VOID)
{
	// 0: 300MHz(=1200MHz/4)
	// 1: 200MHz(=1200MHz/6)
	// 2: 150MHz(=1200MHz/8)
	// 3: 100MHz(=1200MHz/12)
	// 4: 75MHz(=1200MHz/16)
	// 5: 60MHz(=1200MHz/20)
	// 6: 50MHz(=1200MHz/24)
	static const UCHAR id[] = {0, 1, 2, 3, 4, 5, 6, 255};
	static const UCHAR divid[] = {4, 6, 8, 12, 16, 20, 24};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL12_FPT0CLK() == id[i]) {
			clock = (D_DD_TOP_1200_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of GYRO clock
@retval	Hz
*/
ULONG Dd_Top_Get_GYROCLK(VOID)
{
	// 0:100MHz(=1600MHz/16)
	// 1:32MHz(=1600MHz/50)
	// 2:2MHz(=1600MHz/800)
	static const ULONG id[] = {0, 1, 2, 255};
	static const ULONG divid[] = {16, 50, 800};
	int i;
	ULONG clock = 0;

	for (i = 0; id[i] != 255; i ++) {
		if (Dd_Top_Get_CLKSEL12_GYROCLK() == id[i]) {
			clock = (D_DD_TOP_1600_PLL_OUT / divid[i]);
			break;
		}
	}

	return clock;
}

/**
Get Frequency of AXI bus 300 clock
@retval	Hz
*/
ULONG Dd_Top_Get_ACLK300(VOID)
{
	// 0:300MHz(=1200MHz/4)
	// 1:200MHz(=1200MHz/6)
	static const UCHAR divid[] = {4, 6};


	return (D_DD_TOP_1200_PLL_OUT / divid[Dd_Top_Get_CLKSEL12_ACLK300()]);
}


/**
Control Clock Stop to start clock.<br>
@param[in,out]	counter		address of Garding counter
@param[in,out]	reg_addr	address of clock stop register
@param[in]		val			setting value
@remarks Set 1 to XXXSTOPC register internally.<br><br>
		*reg_addr |= ~val
*/
VOID Dd_Top_Start_Clock( UCHAR* counter, volatile unsigned long* reg_addr, unsigned long val )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	unsigned long reg = *reg_addr;

	Dd_ARM_Critical_Section_Start( critseq );
	(*counter)++;
	if ( 1 == (*counter) ) {
		reg &= ~(~val | (~val << 1));
		reg += (~val << 1);
		*reg_addr = reg;
	}
	Dd_ARM_Critical_Section_End( critseq );
}

/**
Control Clock Stop to stop clock.
@param[in,out]	counter		address of Garding counter
@param[in,out]	reg_addr	address of clock stop register
@param[in]		val			setting value
@remarks Set 1 to XXXSTOPS register internally.<br><br>
		*reg_addr |= val
*/
VOID Dd_Top_Stop_Clock( UCHAR* counter, volatile unsigned long* reg_addr, unsigned long val )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	if ( 0 < (*counter) ) {
		(*counter)--;
	}
	if ( 0 == (*counter) ) {
		*reg_addr = ((*reg_addr & ~(val | (val << 1))) + (val | (val << 1))) | val;
	}
	Dd_ARM_Critical_Section_End( critseq );
}

/**
Start PLL00.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll00(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL00ST() != 0 || Dd_Top_Get_PLLCNT2_PL00SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL00ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL00SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL00.
*/
VOID Dd_Top_Stop_Pll00(VOID)
{
	Dd_Top_Set_PLLCNT2_PL00SEL(0);
	Dd_Top_Set_PLLCNT1_PL00ST(0);
	Dd_ARM_Dsb_Pou();

}

/**
Start PLL01.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll01(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL01ST() != 0 || Dd_Top_Get_PLLCNT2_PL01SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL01ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL01SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL01.
*/
VOID Dd_Top_Stop_Pll01(VOID)
{
	Dd_Top_Set_PLLCNT2_PL01SEL(0);
	Dd_Top_Set_PLLCNT1_PL01ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL02.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll02(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL02ST() != 0 || Dd_Top_Get_PLLCNT2_PL02SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL02ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL02SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL02.
*/
VOID Dd_Top_Stop_Pll02(VOID)
{
	Dd_Top_Set_PLLCNT2_PL02SEL(0);
	Dd_Top_Set_PLLCNT1_PL02ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL03.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll03(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL03ST() != 0 || Dd_Top_Get_PLLCNT2_PL03SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL03ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL03SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL03.
*/
VOID Dd_Top_Stop_Pll03(VOID)
{
	Dd_Top_Set_PLLCNT2_PL03SEL(0);
	Dd_Top_Set_PLLCNT1_PL03ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL04.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll04(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL04ST() != 0 || Dd_Top_Get_PLLCNT2_PL04SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL04ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL04SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL04.
*/
VOID Dd_Top_Stop_Pll04(VOID)
{
	Dd_Top_Set_PLLCNT2_PL04SEL(0);
	Dd_Top_Set_PLLCNT1_PL04ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL05.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll05(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL05ST() != 0 || Dd_Top_Get_PLLCNT2_PL05SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL05ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL05SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL05.
*/
VOID Dd_Top_Stop_Pll05(VOID)
{
	Dd_Top_Set_PLLCNT2_PL05SEL(0);
	Dd_Top_Set_PLLCNT1_PL05ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL05A.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll05A(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL05AST() != 0 || Dd_Top_Get_PLLCNT1_PL05ST() != 1 || Dd_Top_Get_PLLCNT2_PL05ASEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL05AST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL05ASEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL05A.
*/
VOID Dd_Top_Stop_Pll05A(VOID)
{
	Dd_Top_Set_PLLCNT2_PL05ASEL(0);
	Dd_Top_Set_PLLCNT1_PL05AST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL06.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll06(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL06ST() != 0 || Dd_Top_Get_PLLCNT2_PL06SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL06ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL06SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL06.
*/
VOID Dd_Top_Stop_Pll06(VOID)
{
	Dd_Top_Set_PLLCNT2_PL06SEL(0);
	Dd_Top_Set_PLLCNT1_PL06ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL07.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll07(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL07ST() != 0 || Dd_Top_Get_PLLCNT2_PL07SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL07ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL07SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL07.
*/
VOID Dd_Top_Stop_Pll07(VOID)
{
	Dd_Top_Set_PLLCNT2_PL07SEL(0);
	Dd_Top_Set_PLLCNT1_PL07ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL08.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll08(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL08ST() != 0 || Dd_Top_Get_PLLCNT2_PL08SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL08ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL08SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL08.
*/
VOID Dd_Top_Stop_Pll08(VOID)
{
	Dd_Top_Set_PLLCNT2_PL08SEL(0);
	Dd_Top_Set_PLLCNT1_PL08ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL10.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll10(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL10ST() != 0 || Dd_Top_Get_PLLCNT2_PL10SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL10ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL10SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL10.
*/
VOID Dd_Top_Stop_Pll10(VOID)
{
	Dd_Top_Set_PLLCNT2_PL10SEL(0);
	Dd_Top_Set_PLLCNT1_PL10ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL10A.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll10A(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL10AST() != 0 || Dd_Top_Get_PLLCNT1_PL10ST() != 1 || Dd_Top_Get_PLLCNT2_PL10ASEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL10AST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL10ASEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL100A.
*/
VOID Dd_Top_Stop_Pll10A(VOID)
{
	Dd_Top_Set_PLLCNT2_PL10ASEL(0);
	Dd_Top_Set_PLLCNT1_PL10AST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start PLL11.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Pll11(VOID)
{
	if (Dd_Top_Get_PLLCNT1_PL11ST() != 0 || Dd_Top_Get_PLLCNT2_PL11SEL() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_PL11ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	Dd_Top_Set_PLLCNT2_PL11SEL(1);
	Dd_ARM_Dsb_Pou();

	return(D_DDIM_OK);
}

/**
Stop PLL11.
*/
VOID Dd_Top_Stop_Pll11(VOID)
{
	Dd_Top_Set_PLLCNT2_PL11SEL(0);
	Dd_Top_Set_PLLCNT1_PL11ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start DDR 00 PLL.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Ddr_Pll00(VOID)
{
	if (Dd_Top_Get_PLLCNT1_DPL00ST() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_DPL00ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	return(D_DDIM_OK);
}

/**
Stop DDR PLL00.
*/
VOID Dd_Top_Stop_Ddr_Pll00(VOID)
{
	Dd_Top_Set_PLLCNT1_DPL00ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start DDR PLL01.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Ddr_Pll01(VOID)
{
	if (Dd_Top_Get_PLLCNT1_DPL01ST() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_DPL01ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	return(D_DDIM_OK);
}

/**
Stop DDR PLL01.
*/
VOID Dd_Top_Stop_Ddr_Pll01(VOID)
{
	Dd_Top_Set_PLLCNT1_DPL01ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start DDR PLL02.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Ddr_Pll02(VOID)
{
	if (Dd_Top_Get_PLLCNT1_DPL02ST() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_DPL02ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	return(D_DDIM_OK);
}

/**
Stop DDR PLL02.
*/
VOID Dd_Top_Stop_Ddr_Pll02(VOID)
{
	Dd_Top_Set_PLLCNT1_DPL02ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start DDR PLL10.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Ddr_Pll10(VOID)
{
	if (Dd_Top_Get_PLLCNT1_DPL10ST() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_DPL10ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	return(D_DDIM_OK);
}

/**
Stop DDR PLL10.
*/
VOID Dd_Top_Stop_Ddr_Pll10(VOID)
{
	Dd_Top_Set_PLLCNT1_DPL10ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start DDR PLL11.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Ddr_Pll11(VOID)
{
	if (Dd_Top_Get_PLLCNT1_DPL11ST() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_DPL11ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	return(D_DDIM_OK);
}

/**
Stop DDR PLL11.
*/
VOID Dd_Top_Stop_Ddr_Pll11(VOID)
{
	Dd_Top_Set_PLLCNT1_DPL11ST(0);
	Dd_ARM_Dsb_Pou();
}

/**
Start DDR PLL12.
@retval	D_DD_OK						Success
@retval	D_DD_TOP_STATUS_ABNORMAL	Fail - output condition
*/
INT32 Dd_Top_Start_Ddr_Pll12(VOID)
{
	if (Dd_Top_Get_PLLCNT1_DPL12ST() != 0) {
		return(D_DD_TOP_STATUS_ABNORMAL);
	}

	Dd_Top_Set_PLLCNT1_DPL12ST(1);
	Dd_ARM_Dsb_Pou();

	DDIM_User_Dly_Tsk(1);

	return(D_DDIM_OK);
}

/**
Stop DDR PLL12.
*/
VOID Dd_Top_Stop_Ddr_Pll12(VOID)
{
	Dd_Top_Set_PLLCNT1_DPL12ST(0);
	Dd_ARM_Dsb_Pou();
}


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
INT32 Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PORT port, UCHAR function )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Function() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( function > D_DD_TOP_GPIO_FUNC_HWMODE ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Function() error. function parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P60: IO_CHIPTOP.EPCR.bit.P60 = function; break;
		case E_DD_TOP_GPIO_P61: IO_CHIPTOP.EPCR.bit.P61 = function; break;
		case E_DD_TOP_GPIO_P62: IO_CHIPTOP.EPCR.bit.P62 = function; break;
		case E_DD_TOP_GPIO_P63: IO_CHIPTOP.EPCR.bit.P63 = function; break;
		case E_DD_TOP_GPIO_P80: IO_CHIPTOP.EPCR.bit.P80 = function; break;
		case E_DD_TOP_GPIO_P81: IO_CHIPTOP.EPCR.bit.P81 = function; break;
		case E_DD_TOP_GPIO_P82: IO_CHIPTOP.EPCR.bit.P82 = function; break;
		case E_DD_TOP_GPIO_P83: IO_CHIPTOP.EPCR.bit.P83 = function; break;
		case E_DD_TOP_GPIO_P84: IO_CHIPTOP.EPCR.bit.P84 = function; break;
		case E_DD_TOP_GPIO_P85: IO_CHIPTOP.EPCR.bit.P85 = function; break;
		case E_DD_TOP_GPIO_P86: IO_CHIPTOP.EPCR.bit.P86 = function; break;
		case E_DD_TOP_GPIO_P87: IO_CHIPTOP.EPCR.bit.P87 = function; break;
		case E_DD_TOP_GPIO_P90: IO_CHIPTOP.EPCR.bit.P90 = function; break;
		case E_DD_TOP_GPIO_P92: IO_CHIPTOP.EPCR.bit.P92 = function; break;
		case E_DD_TOP_GPIO_PA0: IO_CHIPTOP.EPCR.bit.PA0 = function; break;
		case E_DD_TOP_GPIO_PA1: IO_CHIPTOP.EPCR.bit.PA1 = function; break;
		case E_DD_TOP_GPIO_PA3: IO_CHIPTOP.EPCR.bit.PA3 = function; break;
		case E_DD_TOP_GPIO_PA4: IO_CHIPTOP.EPCR.bit.PA4 = function; break;
		case E_DD_TOP_GPIO_PA5: IO_CHIPTOP.EPCR.bit.PA5 = function; break;
		case E_DD_TOP_GPIO_PA7: IO_CHIPTOP.EPCR.bit.PA7 = function; break;
		case E_DD_TOP_GPIO_PB0: IO_CHIPTOP.EPCR.bit.PB0 = function; break;
		case E_DD_TOP_GPIO_PB1: IO_CHIPTOP.EPCR.bit.PB1 = function; break;
		case E_DD_TOP_GPIO_PB2: IO_CHIPTOP.EPCR.bit.PB2 = function; break;
		case E_DD_TOP_GPIO_PB3: IO_CHIPTOP.EPCR.bit.PB3 = function; break;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		case E_DD_TOP_GPIO_PB4: IO_CHIPTOP.EPCR.bit.PB4 = function; break;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		case E_DD_TOP_GPIO_PB4: IO_CHIPTOP.EPCR.bit.TRGEN = function; break;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		case E_DD_TOP_GPIO_PC0L: IO_CHIPTOP.EPCR.bit.PC0 = function; break;
		case E_DD_TOP_GPIO_PC1L: IO_CHIPTOP.EPCR.bit.PC1 = function; break;
		case E_DD_TOP_GPIO_PC2L: IO_CHIPTOP.EPCR.bit.PC2 = function; break;
		case E_DD_TOP_GPIO_PC3L: IO_CHIPTOP.EPCR.bit.PC3 = function; break;
		case E_DD_TOP_GPIO_PC4L: IO_CHIPTOP.EPCR.bit.PC4 = function; break;
		case E_DD_TOP_GPIO_PC5L: IO_CHIPTOP.EPCR.bit.PC5 = function; break;
		case E_DD_TOP_GPIO_PD0: IO_CHIPTOP.EPCR.bit.PD0 = function; break;
		case E_DD_TOP_GPIO_PD1: IO_CHIPTOP.EPCR.bit.PD1 = function; break;
		case E_DD_TOP_GPIO_PD2: IO_CHIPTOP.EPCR.bit.PD2 = function; break;
		case E_DD_TOP_GPIO_PD3: IO_CHIPTOP.EPCR.bit.PD3 = function; break;
		case E_DD_TOP_GPIO_PD4: IO_CHIPTOP.EPCR.bit.PD4 = function; break;
		case E_DD_TOP_GPIO_PD5: IO_CHIPTOP.EPCR.bit.PD5 = function; break;
		case E_DD_TOP_GPIO_PD6: IO_CHIPTOP.EPCR.bit.PD6 = function; break;
		case E_DD_TOP_GPIO_PE1: IO_CHIPTOP.EPCR.bit.PE1 = function; break;
		case E_DD_TOP_GPIO_PE3: IO_CHIPTOP.EPCR.bit.PE3 = function; break;
		case E_DD_TOP_GPIO_PE5: IO_CHIPTOP.EPCR.bit.PE5 = function; break;
		case E_DD_TOP_GPIO_PF4: IO_CHIPTOP.EPCR.bit.PF4 = function; break;
		case E_DD_TOP_GPIO_PG0: IO_CHIPTOP.EPCR.bit.PG0 = function; break;
		case E_DD_TOP_GPIO_PG1: IO_CHIPTOP.EPCR.bit.PG1 = function; break;
		case E_DD_TOP_GPIO_PG2: IO_CHIPTOP.EPCR.bit.PG2 = function; break;
		case E_DD_TOP_GPIO_PG3: IO_CHIPTOP.EPCR.bit.PG3 = function; break;
		case E_DD_TOP_GPIO_PG4: IO_CHIPTOP.EPCR.bit.PG4 = function; break;
		case E_DD_TOP_GPIO_PG5: IO_CHIPTOP.EPCR.bit.PG5 = function; break;
		case E_DD_TOP_GPIO_PG6: IO_CHIPTOP.EPCR.bit.PG6 = function; break;
		case E_DD_TOP_GPIO_PG7: IO_CHIPTOP.EPCR.bit.PG7 = function; break;
		case E_DD_TOP_GPIO_PH0: IO_CHIPTOP.EPCR.bit.PH0 = function; break;
		case E_DD_TOP_GPIO_PH1: IO_CHIPTOP.EPCR.bit.PH1 = function; break;
		case E_DD_TOP_GPIO_PH2: IO_CHIPTOP.EPCR.bit.PH2 = function; break;
		case E_DD_TOP_GPIO_PH3: IO_CHIPTOP.EPCR.bit.PH3 = function; break;
		case E_DD_TOP_GPIO_PW0: IO_CHIPTOP.EPCR.bit.PW0 = function; break;
		case E_DD_TOP_GPIO_PW1: IO_CHIPTOP.EPCR.bit.PW1 = function; break;
		case E_DD_TOP_GPIO_PW2: IO_CHIPTOP.EPCR.bit.PW2 = function; break;
		case E_DD_TOP_GPIO_PW3: IO_CHIPTOP.EPCR.bit.PW3 = function; break;
		case E_DD_TOP_GPIO_PW4: IO_CHIPTOP.EPCR.bit.PW4 = function; break;
		case E_DD_TOP_GPIO_PW5: IO_CHIPTOP.EPCR.bit.PW5 = function; break;
		case E_DD_TOP_GPIO_PW6: IO_CHIPTOP.EPCR.bit.PW6 = function; break;
		case E_DD_TOP_GPIO_PW7: IO_CHIPTOP.EPCR.bit.PW7 = function; break;
		case E_DD_TOP_GPIO_PJ0: IO_CHIPTOP.EPCR.bit.PJ0 = function; break;
		case E_DD_TOP_GPIO_PJ1: IO_CHIPTOP.EPCR.bit.PJ1 = function; break;
		case E_DD_TOP_GPIO_PJ2: IO_CHIPTOP.EPCR.bit.PJ2 = function; break;
		case E_DD_TOP_GPIO_PJ3: IO_CHIPTOP.EPCR.bit.PJ3 = function; break;
		case E_DD_TOP_GPIO_PJ4: IO_CHIPTOP.EPCR.bit.PJ4 = function; break;
		case E_DD_TOP_GPIO_PJ5: IO_CHIPTOP.EPCR.bit.PJ5 = function; break;
		case E_DD_TOP_GPIO_PJ6: IO_CHIPTOP.EPCR.bit.PJ6 = function; break;
		case E_DD_TOP_GPIO_PJ7: IO_CHIPTOP.EPCR.bit.PJ7 = function; break;
		case E_DD_TOP_GPIO_PK0: IO_CHIPTOP.EPCR.bit.PK0 = function; break;
		case E_DD_TOP_GPIO_PK1: IO_CHIPTOP.EPCR.bit.PK1 = function; break;
		case E_DD_TOP_GPIO_PK2: IO_CHIPTOP.EPCR.bit.PK2 = function; break;
		case E_DD_TOP_GPIO_PK3: IO_CHIPTOP.EPCR.bit.PK3 = function; break;
		case E_DD_TOP_GPIO_PK4: IO_CHIPTOP.EPCR.bit.PK4 = function; break;
		case E_DD_TOP_GPIO_PK5: IO_CHIPTOP.EPCR.bit.PK5 = function; break;
		case E_DD_TOP_GPIO_PK6: IO_CHIPTOP.EPCR.bit.PK6 = function; break;
		case E_DD_TOP_GPIO_PK7: IO_CHIPTOP.EPCR.bit.PK7 = function; break;
		case E_DD_TOP_GPIO_PL0: IO_CHIPTOP.EPCR.bit.PL0 = function; break;
		case E_DD_TOP_GPIO_PL1: IO_CHIPTOP.EPCR.bit.PL1 = function; break;
		case E_DD_TOP_GPIO_PL2: IO_CHIPTOP.EPCR.bit.PL2 = function; break;
		case E_DD_TOP_GPIO_PL3: IO_CHIPTOP.EPCR.bit.PL3 = function; break;
		case E_DD_TOP_GPIO_PL4: IO_CHIPTOP.EPCR.bit.PL4 = function; break;
		case E_DD_TOP_GPIO_PL5: IO_CHIPTOP.EPCR.bit.PL5 = function; break;
		case E_DD_TOP_GPIO_PM1: IO_CHIPTOP.EPCR.bit.PM1 = function; break;
		case E_DD_TOP_GPIO_PM3: IO_CHIPTOP.EPCR.bit.PM3 = function; break;
		case E_DD_TOP_GPIO_PM4: IO_CHIPTOP.EPCR.bit.PM4 = function; break;
		case E_DD_TOP_GPIO_PN1: IO_CHIPTOP.EPCR.bit.PN1 = function; break;
		case E_DD_TOP_GPIO_PN3: IO_CHIPTOP.EPCR.bit.PN3 = function; break;
		case E_DD_TOP_GPIO_PN4: IO_CHIPTOP.EPCR.bit.PN4 = function; break;
		case E_DD_TOP_GPIO_PY0: IO_CHIPTOP.EPCR.bit.PY0 = function; break;
		case E_DD_TOP_GPIO_PY1: IO_CHIPTOP.EPCR.bit.PY1 = function; break;
		case E_DD_TOP_GPIO_PY4: IO_CHIPTOP.EPCR.bit.PY4 = function; break;
		case E_DD_TOP_GPIO_PY5: IO_CHIPTOP.EPCR.bit.PY5 = function; break;
		case E_DD_TOP_GPIO_PY6: IO_CHIPTOP.EPCR.bit.PY6 = function; break;
		case E_DD_TOP_GPIO_PY7: IO_CHIPTOP.EPCR.bit.PY7 = function; break;
		case E_DD_TOP_GPIO_PP0: IO_CHIPTOP.EPCR.bit.PP0 = function; break;
		case E_DD_TOP_GPIO_PP1: IO_CHIPTOP.EPCR.bit.PP1 = function; break;
		case E_DD_TOP_GPIO_PP2: IO_CHIPTOP.EPCR.bit.PP2 = function; break;
		case E_DD_TOP_GPIO_PP3: IO_CHIPTOP.EPCR.bit.PP3 = function; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_Function() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Get_Gpio_Function( E_DD_TOP_GPIO_PORT port, UCHAR* function )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Function() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( function == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Function() error. function parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P60: *function = IO_CHIPTOP.EPCR.bit.P60; break;
		case E_DD_TOP_GPIO_P61: *function = IO_CHIPTOP.EPCR.bit.P61; break;
		case E_DD_TOP_GPIO_P62: *function = IO_CHIPTOP.EPCR.bit.P62; break;
		case E_DD_TOP_GPIO_P63: *function = IO_CHIPTOP.EPCR.bit.P63; break;
		case E_DD_TOP_GPIO_P80: *function = IO_CHIPTOP.EPCR.bit.P80; break;
		case E_DD_TOP_GPIO_P81: *function = IO_CHIPTOP.EPCR.bit.P81; break;
		case E_DD_TOP_GPIO_P82: *function = IO_CHIPTOP.EPCR.bit.P82; break;
		case E_DD_TOP_GPIO_P83: *function = IO_CHIPTOP.EPCR.bit.P83; break;
		case E_DD_TOP_GPIO_P84: *function = IO_CHIPTOP.EPCR.bit.P84; break;
		case E_DD_TOP_GPIO_P85: *function = IO_CHIPTOP.EPCR.bit.P85; break;
		case E_DD_TOP_GPIO_P86: *function = IO_CHIPTOP.EPCR.bit.P86; break;
		case E_DD_TOP_GPIO_P87: *function = IO_CHIPTOP.EPCR.bit.P87; break;
		case E_DD_TOP_GPIO_P90: *function = IO_CHIPTOP.EPCR.bit.P90; break;
		case E_DD_TOP_GPIO_P92: *function = IO_CHIPTOP.EPCR.bit.P92; break;
		case E_DD_TOP_GPIO_PA0: *function = IO_CHIPTOP.EPCR.bit.PA0; break;
		case E_DD_TOP_GPIO_PA1: *function = IO_CHIPTOP.EPCR.bit.PA1; break;
		case E_DD_TOP_GPIO_PA3: *function = IO_CHIPTOP.EPCR.bit.PA3; break;
		case E_DD_TOP_GPIO_PA4: *function = IO_CHIPTOP.EPCR.bit.PA4; break;
		case E_DD_TOP_GPIO_PA5: *function = IO_CHIPTOP.EPCR.bit.PA5; break;
		case E_DD_TOP_GPIO_PA7: *function = IO_CHIPTOP.EPCR.bit.PA7; break;
		case E_DD_TOP_GPIO_PB0: *function = IO_CHIPTOP.EPCR.bit.PB0; break;
		case E_DD_TOP_GPIO_PB1: *function = IO_CHIPTOP.EPCR.bit.PB1; break;
		case E_DD_TOP_GPIO_PB2: *function = IO_CHIPTOP.EPCR.bit.PB2; break;
		case E_DD_TOP_GPIO_PB3: *function = IO_CHIPTOP.EPCR.bit.PB3; break;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		case E_DD_TOP_GPIO_PB4: *function = IO_CHIPTOP.EPCR.bit.PB4; break;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		case E_DD_TOP_GPIO_PB4: *function = IO_CHIPTOP.EPCR.bit.TRGEN; break;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		case E_DD_TOP_GPIO_PC0L: *function = IO_CHIPTOP.EPCR.bit.PC0; break;
		case E_DD_TOP_GPIO_PC1L: *function = IO_CHIPTOP.EPCR.bit.PC1; break;
		case E_DD_TOP_GPIO_PC2L: *function = IO_CHIPTOP.EPCR.bit.PC2; break;
		case E_DD_TOP_GPIO_PC3L: *function = IO_CHIPTOP.EPCR.bit.PC3; break;
		case E_DD_TOP_GPIO_PC4L: *function = IO_CHIPTOP.EPCR.bit.PC4; break;
		case E_DD_TOP_GPIO_PC5L: *function = IO_CHIPTOP.EPCR.bit.PC5; break;
		case E_DD_TOP_GPIO_PD0: *function = IO_CHIPTOP.EPCR.bit.PD0; break;
		case E_DD_TOP_GPIO_PD1: *function = IO_CHIPTOP.EPCR.bit.PD1; break;
		case E_DD_TOP_GPIO_PD2: *function = IO_CHIPTOP.EPCR.bit.PD2; break;
		case E_DD_TOP_GPIO_PD3: *function = IO_CHIPTOP.EPCR.bit.PD3; break;
		case E_DD_TOP_GPIO_PD4: *function = IO_CHIPTOP.EPCR.bit.PD4; break;
		case E_DD_TOP_GPIO_PD5: *function = IO_CHIPTOP.EPCR.bit.PD5; break;
		case E_DD_TOP_GPIO_PD6: *function = IO_CHIPTOP.EPCR.bit.PD6; break;
		case E_DD_TOP_GPIO_PE1: *function = IO_CHIPTOP.EPCR.bit.PE1; break;
		case E_DD_TOP_GPIO_PE3: *function = IO_CHIPTOP.EPCR.bit.PE3; break;
		case E_DD_TOP_GPIO_PE5: *function = IO_CHIPTOP.EPCR.bit.PE5; break;
		case E_DD_TOP_GPIO_PF4: *function = IO_CHIPTOP.EPCR.bit.PF4; break;
		case E_DD_TOP_GPIO_PG0: *function = IO_CHIPTOP.EPCR.bit.PG0; break;
		case E_DD_TOP_GPIO_PG1: *function = IO_CHIPTOP.EPCR.bit.PG1; break;
		case E_DD_TOP_GPIO_PG2: *function = IO_CHIPTOP.EPCR.bit.PG2; break;
		case E_DD_TOP_GPIO_PG3: *function = IO_CHIPTOP.EPCR.bit.PG3; break;
		case E_DD_TOP_GPIO_PG4: *function = IO_CHIPTOP.EPCR.bit.PG4; break;
		case E_DD_TOP_GPIO_PG5: *function = IO_CHIPTOP.EPCR.bit.PG5; break;
		case E_DD_TOP_GPIO_PG6: *function = IO_CHIPTOP.EPCR.bit.PG6; break;
		case E_DD_TOP_GPIO_PG7: *function = IO_CHIPTOP.EPCR.bit.PG7; break;
		case E_DD_TOP_GPIO_PH0: *function = IO_CHIPTOP.EPCR.bit.PH0; break;
		case E_DD_TOP_GPIO_PH1: *function = IO_CHIPTOP.EPCR.bit.PH1; break;
		case E_DD_TOP_GPIO_PH2: *function = IO_CHIPTOP.EPCR.bit.PH2; break;
		case E_DD_TOP_GPIO_PH3: *function = IO_CHIPTOP.EPCR.bit.PH3; break;
		case E_DD_TOP_GPIO_PW0: *function = IO_CHIPTOP.EPCR.bit.PW0; break;
		case E_DD_TOP_GPIO_PW1: *function = IO_CHIPTOP.EPCR.bit.PW1; break;
		case E_DD_TOP_GPIO_PW2: *function = IO_CHIPTOP.EPCR.bit.PW2; break;
		case E_DD_TOP_GPIO_PW3: *function = IO_CHIPTOP.EPCR.bit.PW3; break;
		case E_DD_TOP_GPIO_PW4: *function = IO_CHIPTOP.EPCR.bit.PW4; break;
		case E_DD_TOP_GPIO_PW5: *function = IO_CHIPTOP.EPCR.bit.PW5; break;
		case E_DD_TOP_GPIO_PW6: *function = IO_CHIPTOP.EPCR.bit.PW6; break;
		case E_DD_TOP_GPIO_PW7: *function = IO_CHIPTOP.EPCR.bit.PW7; break;
		case E_DD_TOP_GPIO_PJ0: *function = IO_CHIPTOP.EPCR.bit.PJ0; break;
		case E_DD_TOP_GPIO_PJ1: *function = IO_CHIPTOP.EPCR.bit.PJ1; break;
		case E_DD_TOP_GPIO_PJ2: *function = IO_CHIPTOP.EPCR.bit.PJ2; break;
		case E_DD_TOP_GPIO_PJ3: *function = IO_CHIPTOP.EPCR.bit.PJ3; break;
		case E_DD_TOP_GPIO_PJ4: *function = IO_CHIPTOP.EPCR.bit.PJ4; break;
		case E_DD_TOP_GPIO_PJ5: *function = IO_CHIPTOP.EPCR.bit.PJ5; break;
		case E_DD_TOP_GPIO_PJ6: *function = IO_CHIPTOP.EPCR.bit.PJ6; break;
		case E_DD_TOP_GPIO_PJ7: *function = IO_CHIPTOP.EPCR.bit.PJ7; break;
		case E_DD_TOP_GPIO_PK0: *function = IO_CHIPTOP.EPCR.bit.PK0; break;
		case E_DD_TOP_GPIO_PK1: *function = IO_CHIPTOP.EPCR.bit.PK1; break;
		case E_DD_TOP_GPIO_PK2: *function = IO_CHIPTOP.EPCR.bit.PK2; break;
		case E_DD_TOP_GPIO_PK3: *function = IO_CHIPTOP.EPCR.bit.PK3; break;
		case E_DD_TOP_GPIO_PK4: *function = IO_CHIPTOP.EPCR.bit.PK4; break;
		case E_DD_TOP_GPIO_PK5: *function = IO_CHIPTOP.EPCR.bit.PK5; break;
		case E_DD_TOP_GPIO_PK6: *function = IO_CHIPTOP.EPCR.bit.PK6; break;
		case E_DD_TOP_GPIO_PK7: *function = IO_CHIPTOP.EPCR.bit.PK7; break;
		case E_DD_TOP_GPIO_PL0: *function = IO_CHIPTOP.EPCR.bit.PL0; break;
		case E_DD_TOP_GPIO_PL1: *function = IO_CHIPTOP.EPCR.bit.PL1; break;
		case E_DD_TOP_GPIO_PL2: *function = IO_CHIPTOP.EPCR.bit.PL2; break;
		case E_DD_TOP_GPIO_PL3: *function = IO_CHIPTOP.EPCR.bit.PL3; break;
		case E_DD_TOP_GPIO_PL4: *function = IO_CHIPTOP.EPCR.bit.PL4; break;
		case E_DD_TOP_GPIO_PL5: *function = IO_CHIPTOP.EPCR.bit.PL5; break;
		case E_DD_TOP_GPIO_PM1: *function = IO_CHIPTOP.EPCR.bit.PM1; break;
		case E_DD_TOP_GPIO_PM3: *function = IO_CHIPTOP.EPCR.bit.PM3; break;
		case E_DD_TOP_GPIO_PM4: *function = IO_CHIPTOP.EPCR.bit.PM4; break;
		case E_DD_TOP_GPIO_PN1: *function = IO_CHIPTOP.EPCR.bit.PN1; break;
		case E_DD_TOP_GPIO_PN3: *function = IO_CHIPTOP.EPCR.bit.PN3; break;
		case E_DD_TOP_GPIO_PN4: *function = IO_CHIPTOP.EPCR.bit.PN4; break;
		case E_DD_TOP_GPIO_PY0: *function = IO_CHIPTOP.EPCR.bit.PY0; break;
		case E_DD_TOP_GPIO_PY1: *function = IO_CHIPTOP.EPCR.bit.PY1; break;
		case E_DD_TOP_GPIO_PY4: *function = IO_CHIPTOP.EPCR.bit.PY4; break;
		case E_DD_TOP_GPIO_PY5: *function = IO_CHIPTOP.EPCR.bit.PY5; break;
		case E_DD_TOP_GPIO_PY6: *function = IO_CHIPTOP.EPCR.bit.PY6; break;
		case E_DD_TOP_GPIO_PY7: *function = IO_CHIPTOP.EPCR.bit.PY7; break;
		case E_DD_TOP_GPIO_PP0: *function = IO_CHIPTOP.EPCR.bit.PP0; break;
		case E_DD_TOP_GPIO_PP1: *function = IO_CHIPTOP.EPCR.bit.PP1; break;
		case E_DD_TOP_GPIO_PP2: *function = IO_CHIPTOP.EPCR.bit.PP2; break;
		case E_DD_TOP_GPIO_PP3: *function = IO_CHIPTOP.EPCR.bit.PP3; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Get_Gpio_Function() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Set_Gpio_Direction( E_DD_TOP_GPIO_PORT port, UCHAR direction )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Direction() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( direction > D_DD_TOP_GPIO_DIR_OUT ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Direction() error. direction parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P60: IO_CHIPTOP.DDR.bit.P60 = direction; break;
		case E_DD_TOP_GPIO_P61: IO_CHIPTOP.DDR.bit.P61 = direction; break;
		case E_DD_TOP_GPIO_P62: IO_CHIPTOP.DDR.bit.P62 = direction; break;
		case E_DD_TOP_GPIO_P63: IO_CHIPTOP.DDR.bit.P63 = direction; break;
		case E_DD_TOP_GPIO_P64: IO_CHIPTOP.DDR.bit.P64 = direction; break;
		case E_DD_TOP_GPIO_P65: IO_CHIPTOP.DDR.bit.P65 = direction; break;
		case E_DD_TOP_GPIO_P66: IO_CHIPTOP.DDR.bit.P66 = direction; break;
		case E_DD_TOP_GPIO_P67: IO_CHIPTOP.DDR.bit.P67 = direction; break;
		case E_DD_TOP_GPIO_P70: IO_CHIPTOP.DDR.bit.P70 = direction; break;
		case E_DD_TOP_GPIO_P71: IO_CHIPTOP.DDR.bit.P71 = direction; break;
		case E_DD_TOP_GPIO_P72: IO_CHIPTOP.DDR.bit.P72 = direction; break;
		case E_DD_TOP_GPIO_P73: IO_CHIPTOP.DDR.bit.P73 = direction; break;
		case E_DD_TOP_GPIO_P74: IO_CHIPTOP.DDR.bit.P74 = direction; break;
		case E_DD_TOP_GPIO_P75: IO_CHIPTOP.DDR.bit.P75 = direction; break;
		case E_DD_TOP_GPIO_P76: IO_CHIPTOP.DDR.bit.P76 = direction; break;
		case E_DD_TOP_GPIO_P77: IO_CHIPTOP.DDR.bit.P77 = direction; break;
		case E_DD_TOP_GPIO_P80: IO_CHIPTOP.DDR.bit.P80 = direction; break;
		case E_DD_TOP_GPIO_P81: IO_CHIPTOP.DDR.bit.P81 = direction; break;
		case E_DD_TOP_GPIO_P82: IO_CHIPTOP.DDR.bit.P82 = direction; break;
		case E_DD_TOP_GPIO_P83: IO_CHIPTOP.DDR.bit.P83 = direction; break;
		case E_DD_TOP_GPIO_P84: IO_CHIPTOP.DDR.bit.P84 = direction; break;
		case E_DD_TOP_GPIO_P85: IO_CHIPTOP.DDR.bit.P85 = direction; break;
		case E_DD_TOP_GPIO_P86: IO_CHIPTOP.DDR.bit.P86 = direction; break;
		case E_DD_TOP_GPIO_P87: IO_CHIPTOP.DDR.bit.P87 = direction; break;
		case E_DD_TOP_GPIO_P90: IO_CHIPTOP.DDR.bit.P90 = direction; break;
		case E_DD_TOP_GPIO_P91: IO_CHIPTOP.DDR.bit.P91 = direction; break;
		case E_DD_TOP_GPIO_P92: IO_CHIPTOP.DDR.bit.P92 = direction; break;
		case E_DD_TOP_GPIO_P93: IO_CHIPTOP.DDR.bit.P93 = direction; break;
		case E_DD_TOP_GPIO_PA0: IO_CHIPTOP.DDR.bit.PA0 = direction; break;
		case E_DD_TOP_GPIO_PA1: IO_CHIPTOP.DDR.bit.PA1 = direction; break;
		case E_DD_TOP_GPIO_PA2: IO_CHIPTOP.DDR.bit.PA2 = direction; break;
		case E_DD_TOP_GPIO_PA3: IO_CHIPTOP.DDR.bit.PA3 = direction; break;
		case E_DD_TOP_GPIO_PA4: IO_CHIPTOP.DDR.bit.PA4 = direction; break;
		case E_DD_TOP_GPIO_PA5: IO_CHIPTOP.DDR.bit.PA5 = direction; break;
		case E_DD_TOP_GPIO_PA6: IO_CHIPTOP.DDR.bit.PA6 = direction; break;
		case E_DD_TOP_GPIO_PA7: IO_CHIPTOP.DDR.bit.PA7 = direction; break;
		case E_DD_TOP_GPIO_PB0: IO_CHIPTOP.DDR.bit.PB0 = direction; break;
		case E_DD_TOP_GPIO_PB1: IO_CHIPTOP.DDR.bit.PB1 = direction; break;
		case E_DD_TOP_GPIO_PB2: IO_CHIPTOP.DDR.bit.PB2 = direction; break;
		case E_DD_TOP_GPIO_PB3: IO_CHIPTOP.DDR.bit.PB3 = direction; break;
		case E_DD_TOP_GPIO_PB4: IO_CHIPTOP.DDR.bit.PB4 = direction; break;
		case E_DD_TOP_GPIO_PC0L: IO_CHIPTOP.DDR.bit.PC0 = direction; break;
		case E_DD_TOP_GPIO_PC1L: IO_CHIPTOP.DDR.bit.PC1 = direction; break;
		case E_DD_TOP_GPIO_PC2L: IO_CHIPTOP.DDR.bit.PC2 = direction; break;
		case E_DD_TOP_GPIO_PC3L: IO_CHIPTOP.DDR.bit.PC3 = direction; break;
		case E_DD_TOP_GPIO_PC4L: IO_CHIPTOP.DDR.bit.PC4 = direction; break;
		case E_DD_TOP_GPIO_PC5L: IO_CHIPTOP.DDR.bit.PC5 = direction; break;
		case E_DD_TOP_GPIO_PD0: IO_CHIPTOP.DDR.bit.PD0 = direction; break;
		case E_DD_TOP_GPIO_PD1: IO_CHIPTOP.DDR.bit.PD1 = direction; break;
		case E_DD_TOP_GPIO_PD2: IO_CHIPTOP.DDR.bit.PD2 = direction; break;
		case E_DD_TOP_GPIO_PD3: IO_CHIPTOP.DDR.bit.PD3 = direction; break;
		case E_DD_TOP_GPIO_PD4: IO_CHIPTOP.DDR.bit.PD4 = direction; break;
		case E_DD_TOP_GPIO_PD5: IO_CHIPTOP.DDR.bit.PD5 = direction; break;
		case E_DD_TOP_GPIO_PD6: IO_CHIPTOP.DDR.bit.PD6 = direction; break;
		case E_DD_TOP_GPIO_PE0: IO_CHIPTOP.DDR.bit.PE0 = direction; break;
		case E_DD_TOP_GPIO_PE1: IO_CHIPTOP.DDR.bit.PE1 = direction; break;
		case E_DD_TOP_GPIO_PE2: IO_CHIPTOP.DDR.bit.PE2 = direction; break;
		case E_DD_TOP_GPIO_PE3: IO_CHIPTOP.DDR.bit.PE3 = direction; break;
		case E_DD_TOP_GPIO_PE4: IO_CHIPTOP.DDR.bit.PE4 = direction; break;
		case E_DD_TOP_GPIO_PE5: IO_CHIPTOP.DDR.bit.PE5 = direction; break;
		case E_DD_TOP_GPIO_PF0: IO_CHIPTOP.DDR.bit.PF0 = direction; break;
		case E_DD_TOP_GPIO_PF1: IO_CHIPTOP.DDR.bit.PF1 = direction; break;
		case E_DD_TOP_GPIO_PF2: IO_CHIPTOP.DDR.bit.PF2 = direction; break;
		case E_DD_TOP_GPIO_PF3: IO_CHIPTOP.DDR.bit.PF3 = direction; break;
		case E_DD_TOP_GPIO_PF4: IO_CHIPTOP.DDR.bit.PF4 = direction; break;
		case E_DD_TOP_GPIO_PG0: IO_CHIPTOP.DDR.bit.PG0 = direction; break;
		case E_DD_TOP_GPIO_PG1: IO_CHIPTOP.DDR.bit.PG1 = direction; break;
		case E_DD_TOP_GPIO_PG2: IO_CHIPTOP.DDR.bit.PG2 = direction; break;
		case E_DD_TOP_GPIO_PG3: IO_CHIPTOP.DDR.bit.PG3 = direction; break;
		case E_DD_TOP_GPIO_PG4: IO_CHIPTOP.DDR.bit.PG4 = direction; break;
		case E_DD_TOP_GPIO_PG5: IO_CHIPTOP.DDR.bit.PG5 = direction; break;
		case E_DD_TOP_GPIO_PG6: IO_CHIPTOP.DDR.bit.PG6 = direction; break;
		case E_DD_TOP_GPIO_PG7: IO_CHIPTOP.DDR.bit.PG7 = direction; break;
		case E_DD_TOP_GPIO_PH0: IO_CHIPTOP.DDR.bit.PH0 = direction; break;
		case E_DD_TOP_GPIO_PH1: IO_CHIPTOP.DDR.bit.PH1 = direction; break;
		case E_DD_TOP_GPIO_PH2: IO_CHIPTOP.DDR.bit.PH2 = direction; break;
		case E_DD_TOP_GPIO_PH3: IO_CHIPTOP.DDR.bit.PH3 = direction; break;
		case E_DD_TOP_GPIO_PW0: IO_CHIPTOP.DDR.bit.PW0 = direction; break;
		case E_DD_TOP_GPIO_PW1: IO_CHIPTOP.DDR.bit.PW1 = direction; break;
		case E_DD_TOP_GPIO_PW2: IO_CHIPTOP.DDR.bit.PW2 = direction; break;
		case E_DD_TOP_GPIO_PW3: IO_CHIPTOP.DDR.bit.PW3 = direction; break;
		case E_DD_TOP_GPIO_PW4: IO_CHIPTOP.DDR.bit.PW4 = direction; break;
		case E_DD_TOP_GPIO_PW5: IO_CHIPTOP.DDR.bit.PW5 = direction; break;
		case E_DD_TOP_GPIO_PW6: IO_CHIPTOP.DDR.bit.PW6 = direction; break;
		case E_DD_TOP_GPIO_PW7: IO_CHIPTOP.DDR.bit.PW7 = direction; break;
		case E_DD_TOP_GPIO_PJ0: IO_CHIPTOP.DDR.bit.PJ0 = direction; break;
		case E_DD_TOP_GPIO_PJ1: IO_CHIPTOP.DDR.bit.PJ1 = direction; break;
		case E_DD_TOP_GPIO_PJ2: IO_CHIPTOP.DDR.bit.PJ2 = direction; break;
		case E_DD_TOP_GPIO_PJ3: IO_CHIPTOP.DDR.bit.PJ3 = direction; break;
		case E_DD_TOP_GPIO_PJ4: IO_CHIPTOP.DDR.bit.PJ4 = direction; break;
		case E_DD_TOP_GPIO_PJ5: IO_CHIPTOP.DDR.bit.PJ5 = direction; break;
		case E_DD_TOP_GPIO_PJ6: IO_CHIPTOP.DDR.bit.PJ6 = direction; break;
		case E_DD_TOP_GPIO_PJ7: IO_CHIPTOP.DDR.bit.PJ7 = direction; break;
		case E_DD_TOP_GPIO_PK0: IO_CHIPTOP.DDR.bit.PK0 = direction; break;
		case E_DD_TOP_GPIO_PK1: IO_CHIPTOP.DDR.bit.PK1 = direction; break;
		case E_DD_TOP_GPIO_PK2: IO_CHIPTOP.DDR.bit.PK2 = direction; break;
		case E_DD_TOP_GPIO_PK3: IO_CHIPTOP.DDR.bit.PK3 = direction; break;
		case E_DD_TOP_GPIO_PK4: IO_CHIPTOP.DDR.bit.PK4 = direction; break;
		case E_DD_TOP_GPIO_PK5: IO_CHIPTOP.DDR.bit.PK5 = direction; break;
		case E_DD_TOP_GPIO_PK6: IO_CHIPTOP.DDR.bit.PK6 = direction; break;
		case E_DD_TOP_GPIO_PK7: IO_CHIPTOP.DDR.bit.PK7 = direction; break;
		case E_DD_TOP_GPIO_PL0: IO_CHIPTOP.DDR.bit.PL0 = direction; break;
		case E_DD_TOP_GPIO_PL1: IO_CHIPTOP.DDR.bit.PL1 = direction; break;
		case E_DD_TOP_GPIO_PL2: IO_CHIPTOP.DDR.bit.PL2 = direction; break;
		case E_DD_TOP_GPIO_PL3: IO_CHIPTOP.DDR.bit.PL3 = direction; break;
		case E_DD_TOP_GPIO_PL4: IO_CHIPTOP.DDR.bit.PL4 = direction; break;
		case E_DD_TOP_GPIO_PL5: IO_CHIPTOP.DDR.bit.PL5 = direction; break;
		case E_DD_TOP_GPIO_PM0: IO_CHIPTOP.DDR.bit.PM0 = direction; break;
		case E_DD_TOP_GPIO_PM1: IO_CHIPTOP.DDR.bit.PM1 = direction; break;
		case E_DD_TOP_GPIO_PM2: IO_CHIPTOP.DDR.bit.PM2 = direction; break;
		case E_DD_TOP_GPIO_PM3: IO_CHIPTOP.DDR.bit.PM3 = direction; break;
		case E_DD_TOP_GPIO_PM4: IO_CHIPTOP.DDR.bit.PM4 = direction; break;
		case E_DD_TOP_GPIO_PN0: IO_CHIPTOP.DDR.bit.PN0 = direction; break;
		case E_DD_TOP_GPIO_PN1: IO_CHIPTOP.DDR.bit.PN1 = direction; break;
		case E_DD_TOP_GPIO_PN2: IO_CHIPTOP.DDR.bit.PN2 = direction; break;
		case E_DD_TOP_GPIO_PN3: IO_CHIPTOP.DDR.bit.PN3 = direction; break;
		case E_DD_TOP_GPIO_PN4: IO_CHIPTOP.DDR.bit.PN4 = direction; break;
		case E_DD_TOP_GPIO_PY0: IO_CHIPTOP.DDR.bit.PY0 = direction; break;
		case E_DD_TOP_GPIO_PY1: IO_CHIPTOP.DDR.bit.PY1 = direction; break;
		case E_DD_TOP_GPIO_PY2: IO_CHIPTOP.DDR.bit.PY2 = direction; break;
		case E_DD_TOP_GPIO_PY3: IO_CHIPTOP.DDR.bit.PY3 = direction; break;
		case E_DD_TOP_GPIO_PY4: IO_CHIPTOP.DDR.bit.PY4 = direction; break;
		case E_DD_TOP_GPIO_PY5: IO_CHIPTOP.DDR.bit.PY5 = direction; break;
		case E_DD_TOP_GPIO_PY6: IO_CHIPTOP.DDR.bit.PY6 = direction; break;
		case E_DD_TOP_GPIO_PY7: IO_CHIPTOP.DDR.bit.PY7 = direction; break;
		case E_DD_TOP_GPIO_PP0: IO_CHIPTOP.DDR.bit.PP0 = direction; break;
		case E_DD_TOP_GPIO_PP1: IO_CHIPTOP.DDR.bit.PP1 = direction; break;
		case E_DD_TOP_GPIO_PP2: IO_CHIPTOP.DDR.bit.PP2 = direction; break;
		case E_DD_TOP_GPIO_PP3: IO_CHIPTOP.DDR.bit.PP3 = direction; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_Direction() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Get_Gpio_Direction( E_DD_TOP_GPIO_PORT port, UCHAR* direction )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Direction() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( direction == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Direction() error. direction parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P60: *direction = IO_CHIPTOP.DDR.bit.P60; break;
		case E_DD_TOP_GPIO_P61: *direction = IO_CHIPTOP.DDR.bit.P61; break;
		case E_DD_TOP_GPIO_P62: *direction = IO_CHIPTOP.DDR.bit.P62; break;
		case E_DD_TOP_GPIO_P63: *direction = IO_CHIPTOP.DDR.bit.P63; break;
		case E_DD_TOP_GPIO_P64: *direction = IO_CHIPTOP.DDR.bit.P64; break;
		case E_DD_TOP_GPIO_P65: *direction = IO_CHIPTOP.DDR.bit.P65; break;
		case E_DD_TOP_GPIO_P66: *direction = IO_CHIPTOP.DDR.bit.P66; break;
		case E_DD_TOP_GPIO_P67: *direction = IO_CHIPTOP.DDR.bit.P67; break;
		case E_DD_TOP_GPIO_P70: *direction = IO_CHIPTOP.DDR.bit.P70; break;
		case E_DD_TOP_GPIO_P71: *direction = IO_CHIPTOP.DDR.bit.P71; break;
		case E_DD_TOP_GPIO_P72: *direction = IO_CHIPTOP.DDR.bit.P72; break;
		case E_DD_TOP_GPIO_P73: *direction = IO_CHIPTOP.DDR.bit.P73; break;
		case E_DD_TOP_GPIO_P74: *direction = IO_CHIPTOP.DDR.bit.P74; break;
		case E_DD_TOP_GPIO_P75: *direction = IO_CHIPTOP.DDR.bit.P75; break;
		case E_DD_TOP_GPIO_P76: *direction = IO_CHIPTOP.DDR.bit.P76; break;
		case E_DD_TOP_GPIO_P77: *direction = IO_CHIPTOP.DDR.bit.P77; break;
		case E_DD_TOP_GPIO_P80: *direction = IO_CHIPTOP.DDR.bit.P80; break;
		case E_DD_TOP_GPIO_P81: *direction = IO_CHIPTOP.DDR.bit.P81; break;
		case E_DD_TOP_GPIO_P82: *direction = IO_CHIPTOP.DDR.bit.P82; break;
		case E_DD_TOP_GPIO_P83: *direction = IO_CHIPTOP.DDR.bit.P83; break;
		case E_DD_TOP_GPIO_P84: *direction = IO_CHIPTOP.DDR.bit.P84; break;
		case E_DD_TOP_GPIO_P85: *direction = IO_CHIPTOP.DDR.bit.P85; break;
		case E_DD_TOP_GPIO_P86: *direction = IO_CHIPTOP.DDR.bit.P86; break;
		case E_DD_TOP_GPIO_P87: *direction = IO_CHIPTOP.DDR.bit.P87; break;
		case E_DD_TOP_GPIO_P90: *direction = IO_CHIPTOP.DDR.bit.P90; break;
		case E_DD_TOP_GPIO_P91: *direction = IO_CHIPTOP.DDR.bit.P91; break;
		case E_DD_TOP_GPIO_P92: *direction = IO_CHIPTOP.DDR.bit.P92; break;
		case E_DD_TOP_GPIO_P93: *direction = IO_CHIPTOP.DDR.bit.P93; break;
		case E_DD_TOP_GPIO_PA0: *direction = IO_CHIPTOP.DDR.bit.PA0; break;
		case E_DD_TOP_GPIO_PA1: *direction = IO_CHIPTOP.DDR.bit.PA1; break;
		case E_DD_TOP_GPIO_PA2: *direction = IO_CHIPTOP.DDR.bit.PA2; break;
		case E_DD_TOP_GPIO_PA3: *direction = IO_CHIPTOP.DDR.bit.PA3; break;
		case E_DD_TOP_GPIO_PA4: *direction = IO_CHIPTOP.DDR.bit.PA4; break;
		case E_DD_TOP_GPIO_PA5: *direction = IO_CHIPTOP.DDR.bit.PA5; break;
		case E_DD_TOP_GPIO_PA6: *direction = IO_CHIPTOP.DDR.bit.PA6; break;
		case E_DD_TOP_GPIO_PA7: *direction = IO_CHIPTOP.DDR.bit.PA7; break;
		case E_DD_TOP_GPIO_PB0: *direction = IO_CHIPTOP.DDR.bit.PB0; break;
		case E_DD_TOP_GPIO_PB1: *direction = IO_CHIPTOP.DDR.bit.PB1; break;
		case E_DD_TOP_GPIO_PB2: *direction = IO_CHIPTOP.DDR.bit.PB2; break;
		case E_DD_TOP_GPIO_PB3: *direction = IO_CHIPTOP.DDR.bit.PB3; break;
		case E_DD_TOP_GPIO_PB4: *direction = IO_CHIPTOP.DDR.bit.PB4; break;
		case E_DD_TOP_GPIO_PC0L: *direction = IO_CHIPTOP.DDR.bit.PC0; break;
		case E_DD_TOP_GPIO_PC1L: *direction = IO_CHIPTOP.DDR.bit.PC1; break;
		case E_DD_TOP_GPIO_PC2L: *direction = IO_CHIPTOP.DDR.bit.PC2; break;
		case E_DD_TOP_GPIO_PC3L: *direction = IO_CHIPTOP.DDR.bit.PC3; break;
		case E_DD_TOP_GPIO_PC4L: *direction = IO_CHIPTOP.DDR.bit.PC4; break;
		case E_DD_TOP_GPIO_PC5L: *direction = IO_CHIPTOP.DDR.bit.PC5; break;
		case E_DD_TOP_GPIO_PD0: *direction = IO_CHIPTOP.DDR.bit.PD0; break;
		case E_DD_TOP_GPIO_PD1: *direction = IO_CHIPTOP.DDR.bit.PD1; break;
		case E_DD_TOP_GPIO_PD2: *direction = IO_CHIPTOP.DDR.bit.PD2; break;
		case E_DD_TOP_GPIO_PD3: *direction = IO_CHIPTOP.DDR.bit.PD3; break;
		case E_DD_TOP_GPIO_PD4: *direction = IO_CHIPTOP.DDR.bit.PD4; break;
		case E_DD_TOP_GPIO_PD5: *direction = IO_CHIPTOP.DDR.bit.PD5; break;
		case E_DD_TOP_GPIO_PD6: *direction = IO_CHIPTOP.DDR.bit.PD6; break;
		case E_DD_TOP_GPIO_PE0: *direction = IO_CHIPTOP.DDR.bit.PE0; break;
		case E_DD_TOP_GPIO_PE1: *direction = IO_CHIPTOP.DDR.bit.PE1; break;
		case E_DD_TOP_GPIO_PE2: *direction = IO_CHIPTOP.DDR.bit.PE2; break;
		case E_DD_TOP_GPIO_PE3: *direction = IO_CHIPTOP.DDR.bit.PE3; break;
		case E_DD_TOP_GPIO_PE4: *direction = IO_CHIPTOP.DDR.bit.PE4; break;
		case E_DD_TOP_GPIO_PE5: *direction = IO_CHIPTOP.DDR.bit.PE5; break;
		case E_DD_TOP_GPIO_PF0: *direction = IO_CHIPTOP.DDR.bit.PF0; break;
		case E_DD_TOP_GPIO_PF1: *direction = IO_CHIPTOP.DDR.bit.PF1; break;
		case E_DD_TOP_GPIO_PF2: *direction = IO_CHIPTOP.DDR.bit.PF2; break;
		case E_DD_TOP_GPIO_PF3: *direction = IO_CHIPTOP.DDR.bit.PF3; break;
		case E_DD_TOP_GPIO_PF4: *direction = IO_CHIPTOP.DDR.bit.PF4; break;
		case E_DD_TOP_GPIO_PG0: *direction = IO_CHIPTOP.DDR.bit.PG0; break;
		case E_DD_TOP_GPIO_PG1: *direction = IO_CHIPTOP.DDR.bit.PG1; break;
		case E_DD_TOP_GPIO_PG2: *direction = IO_CHIPTOP.DDR.bit.PG2; break;
		case E_DD_TOP_GPIO_PG3: *direction = IO_CHIPTOP.DDR.bit.PG3; break;
		case E_DD_TOP_GPIO_PG4: *direction = IO_CHIPTOP.DDR.bit.PG4; break;
		case E_DD_TOP_GPIO_PG5: *direction = IO_CHIPTOP.DDR.bit.PG5; break;
		case E_DD_TOP_GPIO_PG6: *direction = IO_CHIPTOP.DDR.bit.PG6; break;
		case E_DD_TOP_GPIO_PG7: *direction = IO_CHIPTOP.DDR.bit.PG7; break;
		case E_DD_TOP_GPIO_PH0: *direction = IO_CHIPTOP.DDR.bit.PH0; break;
		case E_DD_TOP_GPIO_PH1: *direction = IO_CHIPTOP.DDR.bit.PH1; break;
		case E_DD_TOP_GPIO_PH2: *direction = IO_CHIPTOP.DDR.bit.PH2; break;
		case E_DD_TOP_GPIO_PH3: *direction = IO_CHIPTOP.DDR.bit.PH3; break;
		case E_DD_TOP_GPIO_PW0: *direction = IO_CHIPTOP.DDR.bit.PW0; break;
		case E_DD_TOP_GPIO_PW1: *direction = IO_CHIPTOP.DDR.bit.PW1; break;
		case E_DD_TOP_GPIO_PW2: *direction = IO_CHIPTOP.DDR.bit.PW2; break;
		case E_DD_TOP_GPIO_PW3: *direction = IO_CHIPTOP.DDR.bit.PW3; break;
		case E_DD_TOP_GPIO_PW4: *direction = IO_CHIPTOP.DDR.bit.PW4; break;
		case E_DD_TOP_GPIO_PW5: *direction = IO_CHIPTOP.DDR.bit.PW5; break;
		case E_DD_TOP_GPIO_PW6: *direction = IO_CHIPTOP.DDR.bit.PW6; break;
		case E_DD_TOP_GPIO_PW7: *direction = IO_CHIPTOP.DDR.bit.PW7; break;
		case E_DD_TOP_GPIO_PJ0: *direction = IO_CHIPTOP.DDR.bit.PJ0; break;
		case E_DD_TOP_GPIO_PJ1: *direction = IO_CHIPTOP.DDR.bit.PJ1; break;
		case E_DD_TOP_GPIO_PJ2: *direction = IO_CHIPTOP.DDR.bit.PJ2; break;
		case E_DD_TOP_GPIO_PJ3: *direction = IO_CHIPTOP.DDR.bit.PJ3; break;
		case E_DD_TOP_GPIO_PJ4: *direction = IO_CHIPTOP.DDR.bit.PJ4; break;
		case E_DD_TOP_GPIO_PJ5: *direction = IO_CHIPTOP.DDR.bit.PJ5; break;
		case E_DD_TOP_GPIO_PJ6: *direction = IO_CHIPTOP.DDR.bit.PJ6; break;
		case E_DD_TOP_GPIO_PJ7: *direction = IO_CHIPTOP.DDR.bit.PJ7; break;
		case E_DD_TOP_GPIO_PK0: *direction = IO_CHIPTOP.DDR.bit.PK0; break;
		case E_DD_TOP_GPIO_PK1: *direction = IO_CHIPTOP.DDR.bit.PK1; break;
		case E_DD_TOP_GPIO_PK2: *direction = IO_CHIPTOP.DDR.bit.PK2; break;
		case E_DD_TOP_GPIO_PK3: *direction = IO_CHIPTOP.DDR.bit.PK3; break;
		case E_DD_TOP_GPIO_PK4: *direction = IO_CHIPTOP.DDR.bit.PK4; break;
		case E_DD_TOP_GPIO_PK5: *direction = IO_CHIPTOP.DDR.bit.PK5; break;
		case E_DD_TOP_GPIO_PK6: *direction = IO_CHIPTOP.DDR.bit.PK6; break;
		case E_DD_TOP_GPIO_PK7: *direction = IO_CHIPTOP.DDR.bit.PK7; break;
		case E_DD_TOP_GPIO_PL0: *direction = IO_CHIPTOP.DDR.bit.PL0; break;
		case E_DD_TOP_GPIO_PL1: *direction = IO_CHIPTOP.DDR.bit.PL1; break;
		case E_DD_TOP_GPIO_PL2: *direction = IO_CHIPTOP.DDR.bit.PL2; break;
		case E_DD_TOP_GPIO_PL3: *direction = IO_CHIPTOP.DDR.bit.PL3; break;
		case E_DD_TOP_GPIO_PL4: *direction = IO_CHIPTOP.DDR.bit.PL4; break;
		case E_DD_TOP_GPIO_PL5: *direction = IO_CHIPTOP.DDR.bit.PL5; break;
		case E_DD_TOP_GPIO_PM0: *direction = IO_CHIPTOP.DDR.bit.PM0; break;
		case E_DD_TOP_GPIO_PM1: *direction = IO_CHIPTOP.DDR.bit.PM1; break;
		case E_DD_TOP_GPIO_PM2: *direction = IO_CHIPTOP.DDR.bit.PM2; break;
		case E_DD_TOP_GPIO_PM3: *direction = IO_CHIPTOP.DDR.bit.PM3; break;
		case E_DD_TOP_GPIO_PM4: *direction = IO_CHIPTOP.DDR.bit.PM4; break;
		case E_DD_TOP_GPIO_PN0: *direction = IO_CHIPTOP.DDR.bit.PN0; break;
		case E_DD_TOP_GPIO_PN1: *direction = IO_CHIPTOP.DDR.bit.PN1; break;
		case E_DD_TOP_GPIO_PN2: *direction = IO_CHIPTOP.DDR.bit.PN2; break;
		case E_DD_TOP_GPIO_PN3: *direction = IO_CHIPTOP.DDR.bit.PN3; break;
		case E_DD_TOP_GPIO_PN4: *direction = IO_CHIPTOP.DDR.bit.PN4; break;
		case E_DD_TOP_GPIO_PY0: *direction = IO_CHIPTOP.DDR.bit.PY0; break;
		case E_DD_TOP_GPIO_PY1: *direction = IO_CHIPTOP.DDR.bit.PY1; break;
		case E_DD_TOP_GPIO_PY2: *direction = IO_CHIPTOP.DDR.bit.PY2; break;
		case E_DD_TOP_GPIO_PY3: *direction = IO_CHIPTOP.DDR.bit.PY3; break;
		case E_DD_TOP_GPIO_PY4: *direction = IO_CHIPTOP.DDR.bit.PY4; break;
		case E_DD_TOP_GPIO_PY5: *direction = IO_CHIPTOP.DDR.bit.PY5; break;
		case E_DD_TOP_GPIO_PY6: *direction = IO_CHIPTOP.DDR.bit.PY6; break;
		case E_DD_TOP_GPIO_PY7: *direction = IO_CHIPTOP.DDR.bit.PY7; break;
		case E_DD_TOP_GPIO_PP0: *direction = IO_CHIPTOP.DDR.bit.PP0; break;
		case E_DD_TOP_GPIO_PP1: *direction = IO_CHIPTOP.DDR.bit.PP1; break;
		case E_DD_TOP_GPIO_PP2: *direction = IO_CHIPTOP.DDR.bit.PP2; break;
		case E_DD_TOP_GPIO_PP3: *direction = IO_CHIPTOP.DDR.bit.PP3; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Get_Gpio_Direction() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_PORT port, UCHAR status )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Status() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( status > D_DD_TOP_GPIO_STATUS_HIGH ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Status() error. status parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P60: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_P61: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_P62: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_P63: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_P64: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_P65: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_P66: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B22 | ( status << 6 )); break;
		case E_DD_TOP_GPIO_P67: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B23 | ( status << 7 )); break;
		case E_DD_TOP_GPIO_P70: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_P71: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_P72: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_P73: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		case E_DD_TOP_GPIO_P74: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B28 | ( status << 12 )); break;
		case E_DD_TOP_GPIO_P75: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B29 | ( status << 13 )); break;
		case E_DD_TOP_GPIO_P76: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B30 | ( status << 14 )); break;
		case E_DD_TOP_GPIO_P77: IO_CHIPTOP.PDR.word[0] = ( D_DD_TOP_PDR_WR_EN_B31 | ( status << 15 )); break;
		
		case E_DD_TOP_GPIO_P80: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_P81: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_P82: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_P83: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_P84: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_P85: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_P86: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B22 | ( status << 6 )); break;
		case E_DD_TOP_GPIO_P87: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B23 | ( status << 7 )); break;
		case E_DD_TOP_GPIO_P90: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_P91: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_P92: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_P93: IO_CHIPTOP.PDR.word[1] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		
		case E_DD_TOP_GPIO_PA0: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_PA1: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_PA2: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_PA3: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_PA4: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_PA5: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_PA6: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B22 | ( status << 6 )); break;
		case E_DD_TOP_GPIO_PA7: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B23 | ( status << 7 )); break;
		case E_DD_TOP_GPIO_PB0: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_PB1: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_PB2: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_PB3: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		case E_DD_TOP_GPIO_PB4: IO_CHIPTOP.PDR.word[2] = ( D_DD_TOP_PDR_WR_EN_B28 | ( status << 12 )); break;
		
		case E_DD_TOP_GPIO_PC0L: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_PC1L: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_PC2L: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_PC3L: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_PC4L: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_PC5L: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_PD0: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_PD1: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_PD2: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_PD3: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		case E_DD_TOP_GPIO_PD4: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B28 | ( status << 12 )); break;
		case E_DD_TOP_GPIO_PD5: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B29 | ( status << 13 )); break;
		case E_DD_TOP_GPIO_PD6: IO_CHIPTOP.PDR.word[3] = ( D_DD_TOP_PDR_WR_EN_B30 | ( status << 14 )); break;
		
		case E_DD_TOP_GPIO_PE0: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_PE1: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_PE2: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_PE3: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_PE4: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_PE5: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_PF0: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_PF1: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_PF2: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_PF3: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		case E_DD_TOP_GPIO_PF4: IO_CHIPTOP.PDR.word[4] = ( D_DD_TOP_PDR_WR_EN_B28 | ( status << 12 )); break;
		
		case E_DD_TOP_GPIO_PG0: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_PG1: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_PG2: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_PG3: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_PG4: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_PG5: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_PG6: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B22 | ( status << 6 )); break;
		case E_DD_TOP_GPIO_PG7: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B23 | ( status << 7 )); break;
		case E_DD_TOP_GPIO_PH0: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_PH1: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_PH2: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_PH3: IO_CHIPTOP.PDR.word[5] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		
		case E_DD_TOP_GPIO_PW0: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_PW1: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_PW2: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_PW3: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_PW4: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_PW5: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_PW6: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B22 | ( status << 6 )); break;
		case E_DD_TOP_GPIO_PW7: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B23 | ( status << 7 )); break;
		case E_DD_TOP_GPIO_PJ0: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_PJ1: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_PJ2: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_PJ3: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		case E_DD_TOP_GPIO_PJ4: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B28 | ( status << 12 )); break;
		case E_DD_TOP_GPIO_PJ5: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B29 | ( status << 13 )); break;
		case E_DD_TOP_GPIO_PJ6: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B30 | ( status << 14 )); break;
		case E_DD_TOP_GPIO_PJ7: IO_CHIPTOP.PDR.word[6] = ( D_DD_TOP_PDR_WR_EN_B31 | ( status << 15 )); break;
		
		case E_DD_TOP_GPIO_PK0: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_PK1: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_PK2: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_PK3: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_PK4: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_PK5: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_PK6: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B22 | ( status << 6 )); break;
		case E_DD_TOP_GPIO_PK7: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B23 | ( status << 7 )); break;
		case E_DD_TOP_GPIO_PL0: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_PL1: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_PL2: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_PL3: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		case E_DD_TOP_GPIO_PL4: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B28 | ( status << 12 )); break;
		case E_DD_TOP_GPIO_PL5: IO_CHIPTOP.PDR.word[7] = ( D_DD_TOP_PDR_WR_EN_B29 | ( status << 13 )); break;
		
		case E_DD_TOP_GPIO_PM0: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_PM1: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_PM2: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_PM3: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_PM4: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_PN0: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_PN1: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_PN2: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_PN3: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;
		case E_DD_TOP_GPIO_PN4: IO_CHIPTOP.PDR.word[8] = ( D_DD_TOP_PDR_WR_EN_B28 | ( status << 12 )); break;
		
		case E_DD_TOP_GPIO_PY0: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B16 | status ); break;
		case E_DD_TOP_GPIO_PY1: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B17 | ( status << 1 )); break;
		case E_DD_TOP_GPIO_PY2: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B18 | ( status << 2 )); break;
		case E_DD_TOP_GPIO_PY3: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B19 | ( status << 3 )); break;
		case E_DD_TOP_GPIO_PY4: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B20 | ( status << 4 )); break;
		case E_DD_TOP_GPIO_PY5: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B21 | ( status << 5 )); break;
		case E_DD_TOP_GPIO_PY6: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B22 | ( status << 6 )); break;
		case E_DD_TOP_GPIO_PY7: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B23 | ( status << 7 )); break;
		case E_DD_TOP_GPIO_PP0: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B24 | ( status << 8 )); break;
		case E_DD_TOP_GPIO_PP1: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B25 | ( status << 9 )); break;
		case E_DD_TOP_GPIO_PP2: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B26 | ( status << 10 )); break;
		case E_DD_TOP_GPIO_PP3: IO_CHIPTOP.PDR.word[9] = ( D_DD_TOP_PDR_WR_EN_B27 | ( status << 11 )); break;

		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_Status() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Get_Gpio_Status( E_DD_TOP_GPIO_PORT port, UCHAR* status )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Status() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( status == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Status() error. status parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P60: *status = IO_CHIPTOP.PDR.bit.P60; break;
		case E_DD_TOP_GPIO_P61: *status = IO_CHIPTOP.PDR.bit.P61; break;
		case E_DD_TOP_GPIO_P62: *status = IO_CHIPTOP.PDR.bit.P62; break;
		case E_DD_TOP_GPIO_P63: *status = IO_CHIPTOP.PDR.bit.P63; break;
		case E_DD_TOP_GPIO_P64: *status = IO_CHIPTOP.PDR.bit.P64; break;
		case E_DD_TOP_GPIO_P65: *status = IO_CHIPTOP.PDR.bit.P65; break;
		case E_DD_TOP_GPIO_P66: *status = IO_CHIPTOP.PDR.bit.P66; break;
		case E_DD_TOP_GPIO_P67: *status = IO_CHIPTOP.PDR.bit.P67; break;
		case E_DD_TOP_GPIO_P70: *status = IO_CHIPTOP.PDR.bit.P70; break;
		case E_DD_TOP_GPIO_P71: *status = IO_CHIPTOP.PDR.bit.P71; break;
		case E_DD_TOP_GPIO_P72: *status = IO_CHIPTOP.PDR.bit.P72; break;
		case E_DD_TOP_GPIO_P73: *status = IO_CHIPTOP.PDR.bit.P73; break;
		case E_DD_TOP_GPIO_P74: *status = IO_CHIPTOP.PDR.bit.P74; break;
		case E_DD_TOP_GPIO_P75: *status = IO_CHIPTOP.PDR.bit.P75; break;
		case E_DD_TOP_GPIO_P76: *status = IO_CHIPTOP.PDR.bit.P76; break;
		case E_DD_TOP_GPIO_P77: *status = IO_CHIPTOP.PDR.bit.P77; break;
		case E_DD_TOP_GPIO_P80: *status = IO_CHIPTOP.PDR.bit.P80; break;
		case E_DD_TOP_GPIO_P81: *status = IO_CHIPTOP.PDR.bit.P81; break;
		case E_DD_TOP_GPIO_P82: *status = IO_CHIPTOP.PDR.bit.P82; break;
		case E_DD_TOP_GPIO_P83: *status = IO_CHIPTOP.PDR.bit.P83; break;
		case E_DD_TOP_GPIO_P84: *status = IO_CHIPTOP.PDR.bit.P84; break;
		case E_DD_TOP_GPIO_P85: *status = IO_CHIPTOP.PDR.bit.P85; break;
		case E_DD_TOP_GPIO_P86: *status = IO_CHIPTOP.PDR.bit.P86; break;
		case E_DD_TOP_GPIO_P87: *status = IO_CHIPTOP.PDR.bit.P87; break;
		case E_DD_TOP_GPIO_P90: *status = IO_CHIPTOP.PDR.bit.P90; break;
		case E_DD_TOP_GPIO_P91: *status = IO_CHIPTOP.PDR.bit.P91; break;
		case E_DD_TOP_GPIO_P92: *status = IO_CHIPTOP.PDR.bit.P92; break;
		case E_DD_TOP_GPIO_P93: *status = IO_CHIPTOP.PDR.bit.P93; break;
		case E_DD_TOP_GPIO_PA0: *status = IO_CHIPTOP.PDR.bit.PA0; break;
		case E_DD_TOP_GPIO_PA1: *status = IO_CHIPTOP.PDR.bit.PA1; break;
		case E_DD_TOP_GPIO_PA2: *status = IO_CHIPTOP.PDR.bit.PA2; break;
		case E_DD_TOP_GPIO_PA3: *status = IO_CHIPTOP.PDR.bit.PA3; break;
		case E_DD_TOP_GPIO_PA4: *status = IO_CHIPTOP.PDR.bit.PA4; break;
		case E_DD_TOP_GPIO_PA5: *status = IO_CHIPTOP.PDR.bit.PA5; break;
		case E_DD_TOP_GPIO_PA6: *status = IO_CHIPTOP.PDR.bit.PA6; break;
		case E_DD_TOP_GPIO_PA7: *status = IO_CHIPTOP.PDR.bit.PA7; break;
		case E_DD_TOP_GPIO_PB0: *status = IO_CHIPTOP.PDR.bit.PB0; break;
		case E_DD_TOP_GPIO_PB1: *status = IO_CHIPTOP.PDR.bit.PB1; break;
		case E_DD_TOP_GPIO_PB2: *status = IO_CHIPTOP.PDR.bit.PB2; break;
		case E_DD_TOP_GPIO_PB3: *status = IO_CHIPTOP.PDR.bit.PB3; break;
		case E_DD_TOP_GPIO_PB4: *status = IO_CHIPTOP.PDR.bit.PB4; break;
		case E_DD_TOP_GPIO_PC0L: *status = IO_CHIPTOP.PDR.bit.PC0; break;
		case E_DD_TOP_GPIO_PC1L: *status = IO_CHIPTOP.PDR.bit.PC1; break;
		case E_DD_TOP_GPIO_PC2L: *status = IO_CHIPTOP.PDR.bit.PC2; break;
		case E_DD_TOP_GPIO_PC3L: *status = IO_CHIPTOP.PDR.bit.PC3; break;
		case E_DD_TOP_GPIO_PC4L: *status = IO_CHIPTOP.PDR.bit.PC4; break;
		case E_DD_TOP_GPIO_PC5L: *status = IO_CHIPTOP.PDR.bit.PC5; break;
		case E_DD_TOP_GPIO_PD0: *status = IO_CHIPTOP.PDR.bit.PD0; break;
		case E_DD_TOP_GPIO_PD1: *status = IO_CHIPTOP.PDR.bit.PD1; break;
		case E_DD_TOP_GPIO_PD2: *status = IO_CHIPTOP.PDR.bit.PD2; break;
		case E_DD_TOP_GPIO_PD3: *status = IO_CHIPTOP.PDR.bit.PD3; break;
		case E_DD_TOP_GPIO_PD4: *status = IO_CHIPTOP.PDR.bit.PD4; break;
		case E_DD_TOP_GPIO_PD5: *status = IO_CHIPTOP.PDR.bit.PD5; break;
		case E_DD_TOP_GPIO_PD6: *status = IO_CHIPTOP.PDR.bit.PD6; break;
		case E_DD_TOP_GPIO_PE0: *status = IO_CHIPTOP.PDR.bit.PE0; break;
		case E_DD_TOP_GPIO_PE1: *status = IO_CHIPTOP.PDR.bit.PE1; break;
		case E_DD_TOP_GPIO_PE2: *status = IO_CHIPTOP.PDR.bit.PE2; break;
		case E_DD_TOP_GPIO_PE3: *status = IO_CHIPTOP.PDR.bit.PE3; break;
		case E_DD_TOP_GPIO_PE4: *status = IO_CHIPTOP.PDR.bit.PE4; break;
		case E_DD_TOP_GPIO_PE5: *status = IO_CHIPTOP.PDR.bit.PE5; break;
		case E_DD_TOP_GPIO_PF0: *status = IO_CHIPTOP.PDR.bit.PF0; break;
		case E_DD_TOP_GPIO_PF1: *status = IO_CHIPTOP.PDR.bit.PF1; break;
		case E_DD_TOP_GPIO_PF2: *status = IO_CHIPTOP.PDR.bit.PF2; break;
		case E_DD_TOP_GPIO_PF3: *status = IO_CHIPTOP.PDR.bit.PF3; break;
		case E_DD_TOP_GPIO_PF4: *status = IO_CHIPTOP.PDR.bit.PF4; break;
		case E_DD_TOP_GPIO_PG0: *status = IO_CHIPTOP.PDR.bit.PG0; break;
		case E_DD_TOP_GPIO_PG1: *status = IO_CHIPTOP.PDR.bit.PG1; break;
		case E_DD_TOP_GPIO_PG2: *status = IO_CHIPTOP.PDR.bit.PG2; break;
		case E_DD_TOP_GPIO_PG3: *status = IO_CHIPTOP.PDR.bit.PG3; break;
		case E_DD_TOP_GPIO_PG4: *status = IO_CHIPTOP.PDR.bit.PG4; break;
		case E_DD_TOP_GPIO_PG5: *status = IO_CHIPTOP.PDR.bit.PG5; break;
		case E_DD_TOP_GPIO_PG6: *status = IO_CHIPTOP.PDR.bit.PG6; break;
		case E_DD_TOP_GPIO_PG7: *status = IO_CHIPTOP.PDR.bit.PG7; break;
		case E_DD_TOP_GPIO_PH0: *status = IO_CHIPTOP.PDR.bit.PH0; break;
		case E_DD_TOP_GPIO_PH1: *status = IO_CHIPTOP.PDR.bit.PH1; break;
		case E_DD_TOP_GPIO_PH2: *status = IO_CHIPTOP.PDR.bit.PH2; break;
		case E_DD_TOP_GPIO_PH3: *status = IO_CHIPTOP.PDR.bit.PH3; break;
		case E_DD_TOP_GPIO_PW0: *status = IO_CHIPTOP.PDR.bit.PW0; break;
		case E_DD_TOP_GPIO_PW1: *status = IO_CHIPTOP.PDR.bit.PW1; break;
		case E_DD_TOP_GPIO_PW2: *status = IO_CHIPTOP.PDR.bit.PW2; break;
		case E_DD_TOP_GPIO_PW3: *status = IO_CHIPTOP.PDR.bit.PW3; break;
		case E_DD_TOP_GPIO_PW4: *status = IO_CHIPTOP.PDR.bit.PW4; break;
		case E_DD_TOP_GPIO_PW5: *status = IO_CHIPTOP.PDR.bit.PW5; break;
		case E_DD_TOP_GPIO_PW6: *status = IO_CHIPTOP.PDR.bit.PW6; break;
		case E_DD_TOP_GPIO_PW7: *status = IO_CHIPTOP.PDR.bit.PW7; break;
		case E_DD_TOP_GPIO_PJ0: *status = IO_CHIPTOP.PDR.bit.PJ0; break;
		case E_DD_TOP_GPIO_PJ1: *status = IO_CHIPTOP.PDR.bit.PJ1; break;
		case E_DD_TOP_GPIO_PJ2: *status = IO_CHIPTOP.PDR.bit.PJ2; break;
		case E_DD_TOP_GPIO_PJ3: *status = IO_CHIPTOP.PDR.bit.PJ3; break;
		case E_DD_TOP_GPIO_PJ4: *status = IO_CHIPTOP.PDR.bit.PJ4; break;
		case E_DD_TOP_GPIO_PJ5: *status = IO_CHIPTOP.PDR.bit.PJ5; break;
		case E_DD_TOP_GPIO_PJ6: *status = IO_CHIPTOP.PDR.bit.PJ6; break;
		case E_DD_TOP_GPIO_PJ7: *status = IO_CHIPTOP.PDR.bit.PJ7; break;
		case E_DD_TOP_GPIO_PK0: *status = IO_CHIPTOP.PDR.bit.PK0; break;
		case E_DD_TOP_GPIO_PK1: *status = IO_CHIPTOP.PDR.bit.PK1; break;
		case E_DD_TOP_GPIO_PK2: *status = IO_CHIPTOP.PDR.bit.PK2; break;
		case E_DD_TOP_GPIO_PK3: *status = IO_CHIPTOP.PDR.bit.PK3; break;
		case E_DD_TOP_GPIO_PK4: *status = IO_CHIPTOP.PDR.bit.PK4; break;
		case E_DD_TOP_GPIO_PK5: *status = IO_CHIPTOP.PDR.bit.PK5; break;
		case E_DD_TOP_GPIO_PK6: *status = IO_CHIPTOP.PDR.bit.PK6; break;
		case E_DD_TOP_GPIO_PK7: *status = IO_CHIPTOP.PDR.bit.PK7; break;
		case E_DD_TOP_GPIO_PL0: *status = IO_CHIPTOP.PDR.bit.PL0; break;
		case E_DD_TOP_GPIO_PL1: *status = IO_CHIPTOP.PDR.bit.PL1; break;
		case E_DD_TOP_GPIO_PL2: *status = IO_CHIPTOP.PDR.bit.PL2; break;
		case E_DD_TOP_GPIO_PL3: *status = IO_CHIPTOP.PDR.bit.PL3; break;
		case E_DD_TOP_GPIO_PL4: *status = IO_CHIPTOP.PDR.bit.PL4; break;
		case E_DD_TOP_GPIO_PL5: *status = IO_CHIPTOP.PDR.bit.PL5; break;
		case E_DD_TOP_GPIO_PM0: *status = IO_CHIPTOP.PDR.bit.PM0; break;
		case E_DD_TOP_GPIO_PM1: *status = IO_CHIPTOP.PDR.bit.PM1; break;
		case E_DD_TOP_GPIO_PM2: *status = IO_CHIPTOP.PDR.bit.PM2; break;
		case E_DD_TOP_GPIO_PM3: *status = IO_CHIPTOP.PDR.bit.PM3; break;
		case E_DD_TOP_GPIO_PM4: *status = IO_CHIPTOP.PDR.bit.PM4; break;
		case E_DD_TOP_GPIO_PN0: *status = IO_CHIPTOP.PDR.bit.PN0; break;
		case E_DD_TOP_GPIO_PN1: *status = IO_CHIPTOP.PDR.bit.PN1; break;
		case E_DD_TOP_GPIO_PN2: *status = IO_CHIPTOP.PDR.bit.PN2; break;
		case E_DD_TOP_GPIO_PN3: *status = IO_CHIPTOP.PDR.bit.PN3; break;
		case E_DD_TOP_GPIO_PN4: *status = IO_CHIPTOP.PDR.bit.PN4; break;
		case E_DD_TOP_GPIO_PY0: *status = IO_CHIPTOP.PDR.bit.PY0; break;
		case E_DD_TOP_GPIO_PY1: *status = IO_CHIPTOP.PDR.bit.PY1; break;
		case E_DD_TOP_GPIO_PY2: *status = IO_CHIPTOP.PDR.bit.PY2; break;
		case E_DD_TOP_GPIO_PY3: *status = IO_CHIPTOP.PDR.bit.PY3; break;
		case E_DD_TOP_GPIO_PY4: *status = IO_CHIPTOP.PDR.bit.PY4; break;
		case E_DD_TOP_GPIO_PY5: *status = IO_CHIPTOP.PDR.bit.PY5; break;
		case E_DD_TOP_GPIO_PY6: *status = IO_CHIPTOP.PDR.bit.PY6; break;
		case E_DD_TOP_GPIO_PY7: *status = IO_CHIPTOP.PDR.bit.PY7; break;
		case E_DD_TOP_GPIO_PP0: *status = IO_CHIPTOP.PDR.bit.PP0; break;
		case E_DD_TOP_GPIO_PP1: *status = IO_CHIPTOP.PDR.bit.PP1; break;
		case E_DD_TOP_GPIO_PP2: *status = IO_CHIPTOP.PDR.bit.PP2; break;
		case E_DD_TOP_GPIO_PP3: *status = IO_CHIPTOP.PDR.bit.PP3; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Get_Gpio_Status() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

#if 1    // special key control
INT32 Dd_Top_Set_Gpio_CM0_Function( E_DD_TOP_GPIO_CM0_PORT port, UCHAR function )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
		INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL
		
#ifdef CO_PARAM_CHECK
		if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
			Ddim_Assertion(("Dd_TOP_GPIO_Set_Gpio_CM0_Function() error. port parameters is abnormal.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
		}
		if( function > D_DD_TOP_GPIO_FUNC_HWMODE ) {
			Ddim_Assertion(("Dd_TOP_GPIO_Set_Gpio_CM0_Function() error. function parameters is abnormal.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
		}
#endif
	
		Dd_Top_Start_GPIO_CLK_WITH_CS();
		
		switch( port ) {
			case E_DD_TOP_GPIO_CM0_P22: IO_PMC.GPIO.EPCR.bit.P22 = function; break;
			case E_DD_TOP_GPIO_CM0_P23: IO_PMC.GPIO.EPCR.bit.P23 = function; break;
			case E_DD_TOP_GPIO_CM0_P31: IO_PMC.GPIO.EPCR.bit.P31 = function; break;
			case E_DD_TOP_GPIO_CM0_P33: IO_PMC.GPIO.EPCR.bit.P33 = function; break;
			case E_DD_TOP_GPIO_CM0_P35: IO_PMC.GPIO.EPCR.bit.P35 = function; break;
			case E_DD_TOP_GPIO_CM0_P37: IO_PMC.GPIO.EPCR.bit.P37 = function; break;
			case E_DD_TOP_GPIO_CM0_P45: IO_PMC.GPIO.EPCR.bit.P45 = function; break;
			default:
				Dd_Top_Stop_GPIO_CLK_WITH_CS();
				Ddim_Print(("Dd_PMC_GPIO_Set_Gpio_Function() error. port not exist.\n"));
				return D_DD_TOP_INPUT_PARAM_ERROR;
		}
	
		Dd_ARM_Dsb_Pou();
		Dd_Top_Stop_GPIO_CLK_WITH_CS();
	
		return D_DDIM_OK;
}

INT32 Dd_Top_Get_Gpio_CM0_Function( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* function )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_GPIO_Get_Gpio_CM0_Function() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( function == NULL ) {
		Ddim_Assertion(("Dd_Top_GPIO_Get_Gpio_CM0_Function() error. function parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P22: *function = IO_PMC.GPIO.EPCR.bit.P22; break;
		case E_DD_TOP_GPIO_CM0_P23: *function = IO_PMC.GPIO.EPCR.bit.P23; break;
		case E_DD_TOP_GPIO_CM0_P31: *function = IO_PMC.GPIO.EPCR.bit.P31; break;
		case E_DD_TOP_GPIO_CM0_P33: *function = IO_PMC.GPIO.EPCR.bit.P33; break;
		case E_DD_TOP_GPIO_CM0_P35: *function = IO_PMC.GPIO.EPCR.bit.P35; break;
		case E_DD_TOP_GPIO_CM0_P37: *function = IO_PMC.GPIO.EPCR.bit.P37; break;
		case E_DD_TOP_GPIO_CM0_P45: *function = IO_PMC.GPIO.EPCR.bit.P45; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_GPIO_Get_Gpio_CM0_Function() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
INT32 Dd_Top_Set_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_PORT port, UCHAR direction )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_CM0_Direction() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( direction > D_DD_TOP_GPIO_DIR_OUT ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_CM0_Direction() error. direction parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();
	
	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P00: IO_PMC.GPIO.DDR.bit.P00 = direction; break;
		case E_DD_TOP_GPIO_CM0_P01: IO_PMC.GPIO.DDR.bit.P01 = direction; break;
		case E_DD_TOP_GPIO_CM0_P02: IO_PMC.GPIO.DDR.bit.P02 = direction; break;
		case E_DD_TOP_GPIO_CM0_P03: IO_PMC.GPIO.DDR.bit.P03 = direction; break;
		case E_DD_TOP_GPIO_CM0_P04: IO_PMC.GPIO.DDR.bit.P04 = direction; break;
		case E_DD_TOP_GPIO_CM0_P05: IO_PMC.GPIO.DDR.bit.P05 = direction; break;
		case E_DD_TOP_GPIO_CM0_P06: IO_PMC.GPIO.DDR.bit.P06 = direction; break;
		case E_DD_TOP_GPIO_CM0_P07: IO_PMC.GPIO.DDR.bit.P07 = direction; break;
		case E_DD_TOP_GPIO_CM0_P10: IO_PMC.GPIO.DDR.bit.P10 = direction; break;
		case E_DD_TOP_GPIO_CM0_P11: IO_PMC.GPIO.DDR.bit.P11 = direction; break;
		case E_DD_TOP_GPIO_CM0_P12: IO_PMC.GPIO.DDR.bit.P12 = direction; break;
		case E_DD_TOP_GPIO_CM0_P13: IO_PMC.GPIO.DDR.bit.P13 = direction; break;
		case E_DD_TOP_GPIO_CM0_P14: IO_PMC.GPIO.DDR.bit.P14 = direction; break;
		case E_DD_TOP_GPIO_CM0_P15: IO_PMC.GPIO.DDR.bit.P15 = direction; break;
		case E_DD_TOP_GPIO_CM0_P16: IO_PMC.GPIO.DDR.bit.P16 = direction; break;
		case E_DD_TOP_GPIO_CM0_P17: IO_PMC.GPIO.DDR.bit.P17 = direction; break;
		case E_DD_TOP_GPIO_CM0_P20: IO_PMC.GPIO.DDR.bit.P20 = direction; break;
		case E_DD_TOP_GPIO_CM0_P21: IO_PMC.GPIO.DDR.bit.P21 = direction; break;
		case E_DD_TOP_GPIO_CM0_P22: IO_PMC.GPIO.DDR.bit.P22 = direction; break;
		case E_DD_TOP_GPIO_CM0_P23: IO_PMC.GPIO.DDR.bit.P23 = direction; break;
		case E_DD_TOP_GPIO_CM0_P24: IO_PMC.GPIO.DDR.bit.P24 = direction; break;
		case E_DD_TOP_GPIO_CM0_P25: IO_PMC.GPIO.DDR.bit.P25 = direction; break;
		case E_DD_TOP_GPIO_CM0_P26: IO_PMC.GPIO.DDR.bit.P26 = direction; break;
		case E_DD_TOP_GPIO_CM0_P27: IO_PMC.GPIO.DDR.bit.P27 = direction; break;
		case E_DD_TOP_GPIO_CM0_P30: IO_PMC.GPIO.DDR.bit.P30 = direction; break;
		case E_DD_TOP_GPIO_CM0_P31: IO_PMC.GPIO.DDR.bit.P31 = direction; break;
		case E_DD_TOP_GPIO_CM0_P32: IO_PMC.GPIO.DDR.bit.P32 = direction; break;
		case E_DD_TOP_GPIO_CM0_P33: IO_PMC.GPIO.DDR.bit.P33 = direction; break;
		case E_DD_TOP_GPIO_CM0_P34: IO_PMC.GPIO.DDR.bit.P34 = direction; break;
		case E_DD_TOP_GPIO_CM0_P35: IO_PMC.GPIO.DDR.bit.P35 = direction; break;
		case E_DD_TOP_GPIO_CM0_P36: IO_PMC.GPIO.DDR.bit.P36 = direction; break;
		case E_DD_TOP_GPIO_CM0_P37: IO_PMC.GPIO.DDR.bit.P37 = direction; break;
		case E_DD_TOP_GPIO_CM0_P44: IO_PMC.GPIO.DDR.bit.P44 = direction; break;
		case E_DD_TOP_GPIO_CM0_P45: IO_PMC.GPIO.DDR.bit.P45 = direction; break;
		case E_DD_TOP_GPIO_CM0_P46: IO_PMC.GPIO.DDR.bit.P46 = direction; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_CM0_Direction() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	
	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
INT32 Dd_Top_Get_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* direction )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_CM0_Direction() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( direction == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_CM0_Direction() error. direction parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P00: *direction = IO_PMC.GPIO.DDR.bit.P00; break;
		case E_DD_TOP_GPIO_CM0_P01: *direction = IO_PMC.GPIO.DDR.bit.P01; break;
		case E_DD_TOP_GPIO_CM0_P02: *direction = IO_PMC.GPIO.DDR.bit.P02; break;
		case E_DD_TOP_GPIO_CM0_P03: *direction = IO_PMC.GPIO.DDR.bit.P03; break;
		case E_DD_TOP_GPIO_CM0_P04: *direction = IO_PMC.GPIO.DDR.bit.P04; break;
		case E_DD_TOP_GPIO_CM0_P05: *direction = IO_PMC.GPIO.DDR.bit.P05; break;
		case E_DD_TOP_GPIO_CM0_P06: *direction = IO_PMC.GPIO.DDR.bit.P06; break;
		case E_DD_TOP_GPIO_CM0_P07: *direction = IO_PMC.GPIO.DDR.bit.P07; break;
		case E_DD_TOP_GPIO_CM0_P10: *direction = IO_PMC.GPIO.DDR.bit.P10; break;
		case E_DD_TOP_GPIO_CM0_P11: *direction = IO_PMC.GPIO.DDR.bit.P11; break;
		case E_DD_TOP_GPIO_CM0_P12: *direction = IO_PMC.GPIO.DDR.bit.P12; break;
		case E_DD_TOP_GPIO_CM0_P13: *direction = IO_PMC.GPIO.DDR.bit.P13; break;
		case E_DD_TOP_GPIO_CM0_P14: *direction = IO_PMC.GPIO.DDR.bit.P14; break;
		case E_DD_TOP_GPIO_CM0_P15: *direction = IO_PMC.GPIO.DDR.bit.P15; break;
		case E_DD_TOP_GPIO_CM0_P16: *direction = IO_PMC.GPIO.DDR.bit.P16; break;
		case E_DD_TOP_GPIO_CM0_P17: *direction = IO_PMC.GPIO.DDR.bit.P17; break;
		case E_DD_TOP_GPIO_CM0_P20: *direction = IO_PMC.GPIO.DDR.bit.P20; break;
		case E_DD_TOP_GPIO_CM0_P21: *direction = IO_PMC.GPIO.DDR.bit.P21; break;
		case E_DD_TOP_GPIO_CM0_P22: *direction = IO_PMC.GPIO.DDR.bit.P22; break;
		case E_DD_TOP_GPIO_CM0_P23: *direction = IO_PMC.GPIO.DDR.bit.P23; break;
		case E_DD_TOP_GPIO_CM0_P24: *direction = IO_PMC.GPIO.DDR.bit.P24; break;
		case E_DD_TOP_GPIO_CM0_P25: *direction = IO_PMC.GPIO.DDR.bit.P25; break;
		case E_DD_TOP_GPIO_CM0_P26: *direction = IO_PMC.GPIO.DDR.bit.P26; break;
		case E_DD_TOP_GPIO_CM0_P27: *direction = IO_PMC.GPIO.DDR.bit.P27; break;
		case E_DD_TOP_GPIO_CM0_P30: *direction = IO_PMC.GPIO.DDR.bit.P30; break;
		case E_DD_TOP_GPIO_CM0_P31: *direction = IO_PMC.GPIO.DDR.bit.P31; break;
		case E_DD_TOP_GPIO_CM0_P32: *direction = IO_PMC.GPIO.DDR.bit.P32; break;
		case E_DD_TOP_GPIO_CM0_P33: *direction = IO_PMC.GPIO.DDR.bit.P33; break;
		case E_DD_TOP_GPIO_CM0_P34: *direction = IO_PMC.GPIO.DDR.bit.P34; break;
		case E_DD_TOP_GPIO_CM0_P35: *direction = IO_PMC.GPIO.DDR.bit.P35; break;
		case E_DD_TOP_GPIO_CM0_P36: *direction = IO_PMC.GPIO.DDR.bit.P36; break;
		case E_DD_TOP_GPIO_CM0_P37: *direction = IO_PMC.GPIO.DDR.bit.P37; break;
		case E_DD_TOP_GPIO_CM0_P44: *direction = IO_PMC.GPIO.DDR.bit.P44; break;
		case E_DD_TOP_GPIO_CM0_P45: *direction = IO_PMC.GPIO.DDR.bit.P45; break;
		case E_DD_TOP_GPIO_CM0_P46: *direction = IO_PMC.GPIO.DDR.bit.P46; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Get_Gpio_CM0_Direction() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
INT32 Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Enable( E_DD_TOP_GPIO_CM0_PORT port, UCHAR enable )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Enable() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( enable > D_DD_TOP_GPIO_PUD_ON ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Enable() error. enable parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P00: IO_PMC.GPIO.PUDER.bit.P00 = enable; break;
		case E_DD_TOP_GPIO_CM0_P01: IO_PMC.GPIO.PUDER.bit.P01 = enable; break;
		case E_DD_TOP_GPIO_CM0_P02: IO_PMC.GPIO.PUDER.bit.P02 = enable; break;
		case E_DD_TOP_GPIO_CM0_P03: IO_PMC.GPIO.PUDER.bit.P03 = enable; break;
		case E_DD_TOP_GPIO_CM0_P04: IO_PMC.GPIO.PUDER.bit.P04 = enable; break;
		case E_DD_TOP_GPIO_CM0_P05: IO_PMC.GPIO.PUDER.bit.P05 = enable; break;
		case E_DD_TOP_GPIO_CM0_P06: IO_PMC.GPIO.PUDER.bit.P06 = enable; break;
		case E_DD_TOP_GPIO_CM0_P07: IO_PMC.GPIO.PUDER.bit.P07 = enable; break;
		case E_DD_TOP_GPIO_CM0_P10: IO_PMC.GPIO.PUDER.bit.P10 = enable; break;
		case E_DD_TOP_GPIO_CM0_P11: IO_PMC.GPIO.PUDER.bit.P11 = enable; break;
		case E_DD_TOP_GPIO_CM0_P12: IO_PMC.GPIO.PUDER.bit.P12 = enable; break;
		case E_DD_TOP_GPIO_CM0_P13: IO_PMC.GPIO.PUDER.bit.P13 = enable; break;
		case E_DD_TOP_GPIO_CM0_P14: IO_PMC.GPIO.PUDER.bit.P14 = enable; break;
		case E_DD_TOP_GPIO_CM0_P15: IO_PMC.GPIO.PUDER.bit.P15 = enable; break;
		case E_DD_TOP_GPIO_CM0_P16: IO_PMC.GPIO.PUDER.bit.P16 = enable; break;
		case E_DD_TOP_GPIO_CM0_P17: IO_PMC.GPIO.PUDER.bit.P17 = enable; break;
		case E_DD_TOP_GPIO_CM0_P20: IO_PMC.GPIO.PUDER.bit.P20 = enable; break;
		case E_DD_TOP_GPIO_CM0_P21: IO_PMC.GPIO.PUDER.bit.P21 = enable; break;
		case E_DD_TOP_GPIO_CM0_P22: IO_PMC.GPIO.PUDER.bit.P22 = enable; break;
		case E_DD_TOP_GPIO_CM0_P23: IO_PMC.GPIO.PUDER.bit.P23 = enable; break;
		case E_DD_TOP_GPIO_CM0_P24: IO_PMC.GPIO.PUDER.bit.P24 = enable; break;
		case E_DD_TOP_GPIO_CM0_P25: IO_PMC.GPIO.PUDER.bit.P25 = enable; break;
		case E_DD_TOP_GPIO_CM0_P26: IO_PMC.GPIO.PUDER.bit.P26 = enable; break;
		case E_DD_TOP_GPIO_CM0_P27: IO_PMC.GPIO.PUDER.bit.P27 = enable; break;
		case E_DD_TOP_GPIO_CM0_P30: IO_PMC.GPIO.PUDER.bit.P30 = enable; break;
		case E_DD_TOP_GPIO_CM0_P31: IO_PMC.GPIO.PUDER.bit.P31 = enable; break;
		case E_DD_TOP_GPIO_CM0_P32: IO_PMC.GPIO.PUDER.bit.P32 = enable; break;
		case E_DD_TOP_GPIO_CM0_P33: IO_PMC.GPIO.PUDER.bit.P33 = enable; break;
		case E_DD_TOP_GPIO_CM0_P34: IO_PMC.GPIO.PUDER.bit.P34 = enable; break;
		case E_DD_TOP_GPIO_CM0_P35: IO_PMC.GPIO.PUDER.bit.P35 = enable; break;
		case E_DD_TOP_GPIO_CM0_P36: IO_PMC.GPIO.PUDER.bit.P36 = enable; break;
		case E_DD_TOP_GPIO_CM0_P37: IO_PMC.GPIO.PUDER.bit.P37 = enable; break;
		case E_DD_TOP_GPIO_CM0_P44: IO_PMC.GPIO.PUDER.bit.P44 = enable; break;
		case E_DD_TOP_GPIO_CM0_P45: IO_PMC.GPIO.PUDER.bit.P45 = enable; break;
		case E_DD_TOP_GPIO_CM0_P46: IO_PMC.GPIO.PUDER.bit.P46 = enable; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Enable() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	
	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
INT32 Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Enable( E_DD_TOP_GPIO_CM0_PORT port, UCHAR *enable )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Enable() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( enable == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Enable() error. enable parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P00: *enable = IO_PMC.GPIO.PUDER.bit.P00; break;
		case E_DD_TOP_GPIO_CM0_P01: *enable = IO_PMC.GPIO.PUDER.bit.P01; break;
		case E_DD_TOP_GPIO_CM0_P02: *enable = IO_PMC.GPIO.PUDER.bit.P02; break;
		case E_DD_TOP_GPIO_CM0_P03: *enable = IO_PMC.GPIO.PUDER.bit.P03; break;
		case E_DD_TOP_GPIO_CM0_P04: *enable = IO_PMC.GPIO.PUDER.bit.P04; break;
		case E_DD_TOP_GPIO_CM0_P05: *enable = IO_PMC.GPIO.PUDER.bit.P05; break;
		case E_DD_TOP_GPIO_CM0_P06: *enable = IO_PMC.GPIO.PUDER.bit.P06; break;
		case E_DD_TOP_GPIO_CM0_P07: *enable = IO_PMC.GPIO.PUDER.bit.P07; break;
		case E_DD_TOP_GPIO_CM0_P10: *enable = IO_PMC.GPIO.PUDER.bit.P10; break;
		case E_DD_TOP_GPIO_CM0_P11: *enable = IO_PMC.GPIO.PUDER.bit.P11; break;
		case E_DD_TOP_GPIO_CM0_P12: *enable = IO_PMC.GPIO.PUDER.bit.P12; break;
		case E_DD_TOP_GPIO_CM0_P13: *enable = IO_PMC.GPIO.PUDER.bit.P13; break;
		case E_DD_TOP_GPIO_CM0_P14: *enable = IO_PMC.GPIO.PUDER.bit.P14; break;
		case E_DD_TOP_GPIO_CM0_P15: *enable = IO_PMC.GPIO.PUDER.bit.P15; break;
		case E_DD_TOP_GPIO_CM0_P16: *enable = IO_PMC.GPIO.PUDER.bit.P16; break;
		case E_DD_TOP_GPIO_CM0_P17: *enable = IO_PMC.GPIO.PUDER.bit.P17; break;
		case E_DD_TOP_GPIO_CM0_P20: *enable = IO_PMC.GPIO.PUDER.bit.P20; break;
		case E_DD_TOP_GPIO_CM0_P21: *enable = IO_PMC.GPIO.PUDER.bit.P21; break;
		case E_DD_TOP_GPIO_CM0_P22: *enable = IO_PMC.GPIO.PUDER.bit.P22; break;
		case E_DD_TOP_GPIO_CM0_P23: *enable = IO_PMC.GPIO.PUDER.bit.P23; break;
		case E_DD_TOP_GPIO_CM0_P24: *enable = IO_PMC.GPIO.PUDER.bit.P24; break;
		case E_DD_TOP_GPIO_CM0_P25: *enable = IO_PMC.GPIO.PUDER.bit.P25; break;
		case E_DD_TOP_GPIO_CM0_P26: *enable = IO_PMC.GPIO.PUDER.bit.P26; break;
		case E_DD_TOP_GPIO_CM0_P27: *enable = IO_PMC.GPIO.PUDER.bit.P27; break;
		case E_DD_TOP_GPIO_CM0_P30: *enable = IO_PMC.GPIO.PUDER.bit.P30; break;
		case E_DD_TOP_GPIO_CM0_P31: *enable = IO_PMC.GPIO.PUDER.bit.P31; break;
		case E_DD_TOP_GPIO_CM0_P32: *enable = IO_PMC.GPIO.PUDER.bit.P32; break;
		case E_DD_TOP_GPIO_CM0_P33: *enable = IO_PMC.GPIO.PUDER.bit.P33; break;
		case E_DD_TOP_GPIO_CM0_P34: *enable = IO_PMC.GPIO.PUDER.bit.P34; break;
		case E_DD_TOP_GPIO_CM0_P35: *enable = IO_PMC.GPIO.PUDER.bit.P35; break;
		case E_DD_TOP_GPIO_CM0_P36: *enable = IO_PMC.GPIO.PUDER.bit.P36; break;
		case E_DD_TOP_GPIO_CM0_P37: *enable = IO_PMC.GPIO.PUDER.bit.P37; break;
		case E_DD_TOP_GPIO_CM0_P44: *enable = IO_PMC.GPIO.PUDER.bit.P44; break;
		case E_DD_TOP_GPIO_CM0_P45: *enable = IO_PMC.GPIO.PUDER.bit.P45; break;
		case E_DD_TOP_GPIO_CM0_P46: *enable = IO_PMC.GPIO.PUDER.bit.P46; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_PMC_GPIO_Get_Gpio_Pull_Up_Down_Enable() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
INT32 Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_CM0_PORT port, UCHAR control )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Ctrl() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( control > D_DD_TOP_GPIO_PUD_UP ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Ctrl() error. enable parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P00: IO_PMC.GPIO.PUDCR.bit.P00 = control; break;
		case E_DD_TOP_GPIO_CM0_P01: IO_PMC.GPIO.PUDCR.bit.P01 = control; break;
		case E_DD_TOP_GPIO_CM0_P02: IO_PMC.GPIO.PUDCR.bit.P02 = control; break;
		case E_DD_TOP_GPIO_CM0_P03: IO_PMC.GPIO.PUDCR.bit.P03 = control; break;
		case E_DD_TOP_GPIO_CM0_P04: IO_PMC.GPIO.PUDCR.bit.P04 = control; break;
		case E_DD_TOP_GPIO_CM0_P05: IO_PMC.GPIO.PUDCR.bit.P05 = control; break;
		case E_DD_TOP_GPIO_CM0_P06: IO_PMC.GPIO.PUDCR.bit.P06 = control; break;
		case E_DD_TOP_GPIO_CM0_P07: IO_PMC.GPIO.PUDCR.bit.P07 = control; break;
		case E_DD_TOP_GPIO_CM0_P10: IO_PMC.GPIO.PUDCR.bit.P10 = control; break;
		case E_DD_TOP_GPIO_CM0_P11: IO_PMC.GPIO.PUDCR.bit.P11 = control; break;
		case E_DD_TOP_GPIO_CM0_P12: IO_PMC.GPIO.PUDCR.bit.P12 = control; break;
		case E_DD_TOP_GPIO_CM0_P13: IO_PMC.GPIO.PUDCR.bit.P13 = control; break;
		case E_DD_TOP_GPIO_CM0_P14: IO_PMC.GPIO.PUDCR.bit.P14 = control; break;
		case E_DD_TOP_GPIO_CM0_P15: IO_PMC.GPIO.PUDCR.bit.P15 = control; break;
		case E_DD_TOP_GPIO_CM0_P16: IO_PMC.GPIO.PUDCR.bit.P16 = control; break;
		case E_DD_TOP_GPIO_CM0_P17: IO_PMC.GPIO.PUDCR.bit.P17 = control; break;
		case E_DD_TOP_GPIO_CM0_P20: IO_PMC.GPIO.PUDCR.bit.P20 = control; break;
		case E_DD_TOP_GPIO_CM0_P21: IO_PMC.GPIO.PUDCR.bit.P21 = control; break;
		case E_DD_TOP_GPIO_CM0_P22: IO_PMC.GPIO.PUDCR.bit.P22 = control; break;
		case E_DD_TOP_GPIO_CM0_P23: IO_PMC.GPIO.PUDCR.bit.P23 = control; break;
		case E_DD_TOP_GPIO_CM0_P24: IO_PMC.GPIO.PUDCR.bit.P24 = control; break;
		case E_DD_TOP_GPIO_CM0_P25: IO_PMC.GPIO.PUDCR.bit.P25 = control; break;
		case E_DD_TOP_GPIO_CM0_P26: IO_PMC.GPIO.PUDCR.bit.P26 = control; break;
		case E_DD_TOP_GPIO_CM0_P27: IO_PMC.GPIO.PUDCR.bit.P27 = control; break;
		case E_DD_TOP_GPIO_CM0_P30: IO_PMC.GPIO.PUDCR.bit.P30 = control; break;
		case E_DD_TOP_GPIO_CM0_P31: IO_PMC.GPIO.PUDCR.bit.P31 = control; break;
		case E_DD_TOP_GPIO_CM0_P32: IO_PMC.GPIO.PUDCR.bit.P32 = control; break;
		case E_DD_TOP_GPIO_CM0_P33: IO_PMC.GPIO.PUDCR.bit.P33 = control; break;
		case E_DD_TOP_GPIO_CM0_P34: IO_PMC.GPIO.PUDCR.bit.P34 = control; break;
		case E_DD_TOP_GPIO_CM0_P35: IO_PMC.GPIO.PUDCR.bit.P35 = control; break;
		case E_DD_TOP_GPIO_CM0_P36: IO_PMC.GPIO.PUDCR.bit.P36 = control; break;
		case E_DD_TOP_GPIO_CM0_P37: IO_PMC.GPIO.PUDCR.bit.P37 = control; break;
		case E_DD_TOP_GPIO_CM0_P44: IO_PMC.GPIO.PUDCR.bit.P44 = control; break;
		case E_DD_TOP_GPIO_CM0_P45: IO_PMC.GPIO.PUDCR.bit.P45 = control; break;
		case E_DD_TOP_GPIO_CM0_P46: IO_PMC.GPIO.PUDCR.bit.P46 = control; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_CM0_Pull_Up_Down_Ctrl() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	
	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
INT32 Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* control )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Ctrl() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( control == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Ctrl() error. enable parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P00: *control = IO_PMC.GPIO.PUDCR.bit.P00; break;
		case E_DD_TOP_GPIO_CM0_P01: *control = IO_PMC.GPIO.PUDCR.bit.P01; break;
		case E_DD_TOP_GPIO_CM0_P02: *control = IO_PMC.GPIO.PUDCR.bit.P02; break;
		case E_DD_TOP_GPIO_CM0_P03: *control = IO_PMC.GPIO.PUDCR.bit.P03; break;
		case E_DD_TOP_GPIO_CM0_P04: *control = IO_PMC.GPIO.PUDCR.bit.P04; break;
		case E_DD_TOP_GPIO_CM0_P05: *control = IO_PMC.GPIO.PUDCR.bit.P05; break;
		case E_DD_TOP_GPIO_CM0_P06: *control = IO_PMC.GPIO.PUDCR.bit.P06; break;
		case E_DD_TOP_GPIO_CM0_P07: *control = IO_PMC.GPIO.PUDCR.bit.P07; break;
		case E_DD_TOP_GPIO_CM0_P10: *control = IO_PMC.GPIO.PUDCR.bit.P10; break;
		case E_DD_TOP_GPIO_CM0_P11: *control = IO_PMC.GPIO.PUDCR.bit.P11; break;
		case E_DD_TOP_GPIO_CM0_P12: *control = IO_PMC.GPIO.PUDCR.bit.P12; break;
		case E_DD_TOP_GPIO_CM0_P13: *control = IO_PMC.GPIO.PUDCR.bit.P13; break;
		case E_DD_TOP_GPIO_CM0_P14: *control = IO_PMC.GPIO.PUDCR.bit.P14; break;
		case E_DD_TOP_GPIO_CM0_P15: *control = IO_PMC.GPIO.PUDCR.bit.P15; break;
		case E_DD_TOP_GPIO_CM0_P16: *control = IO_PMC.GPIO.PUDCR.bit.P16; break;
		case E_DD_TOP_GPIO_CM0_P17: *control = IO_PMC.GPIO.PUDCR.bit.P17; break;
		case E_DD_TOP_GPIO_CM0_P20: *control = IO_PMC.GPIO.PUDCR.bit.P20; break;
		case E_DD_TOP_GPIO_CM0_P21: *control = IO_PMC.GPIO.PUDCR.bit.P21; break;
		case E_DD_TOP_GPIO_CM0_P22: *control = IO_PMC.GPIO.PUDCR.bit.P22; break;
		case E_DD_TOP_GPIO_CM0_P23: *control = IO_PMC.GPIO.PUDCR.bit.P23; break;
		case E_DD_TOP_GPIO_CM0_P24: *control = IO_PMC.GPIO.PUDCR.bit.P24; break;
		case E_DD_TOP_GPIO_CM0_P25: *control = IO_PMC.GPIO.PUDCR.bit.P25; break;
		case E_DD_TOP_GPIO_CM0_P26: *control = IO_PMC.GPIO.PUDCR.bit.P26; break;
		case E_DD_TOP_GPIO_CM0_P27: *control = IO_PMC.GPIO.PUDCR.bit.P27; break;
		case E_DD_TOP_GPIO_CM0_P30: *control = IO_PMC.GPIO.PUDCR.bit.P30; break;
		case E_DD_TOP_GPIO_CM0_P31: *control = IO_PMC.GPIO.PUDCR.bit.P31; break;
		case E_DD_TOP_GPIO_CM0_P32: *control = IO_PMC.GPIO.PUDCR.bit.P32; break;
		case E_DD_TOP_GPIO_CM0_P33: *control = IO_PMC.GPIO.PUDCR.bit.P33; break;
		case E_DD_TOP_GPIO_CM0_P34: *control = IO_PMC.GPIO.PUDCR.bit.P34; break;
		case E_DD_TOP_GPIO_CM0_P35: *control = IO_PMC.GPIO.PUDCR.bit.P35; break;
		case E_DD_TOP_GPIO_CM0_P36: *control = IO_PMC.GPIO.PUDCR.bit.P36; break;
		case E_DD_TOP_GPIO_CM0_P37: *control = IO_PMC.GPIO.PUDCR.bit.P37; break;
		case E_DD_TOP_GPIO_CM0_P44: *control = IO_PMC.GPIO.PUDCR.bit.P44; break;
		case E_DD_TOP_GPIO_CM0_P45: *control = IO_PMC.GPIO.PUDCR.bit.P45; break;
		case E_DD_TOP_GPIO_CM0_P46: *control = IO_PMC.GPIO.PUDCR.bit.P46; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Get_Gpio_CM0_Pull_Up_Down_Ctrl() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
INT32 Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_PORT port, UCHAR status )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL
	int						index = 0;
	union io_pmc_gpio_pdr	pdr;
	
#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_CM0_Status() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( status > D_DD_TOP_GPIO_STATUS_HIGH ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_CM0_Status() error. enable parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif
	memset(&pdr, 0, sizeof(pdr));

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P00: pdr.bit.P00 = status; pdr.bit.PWE00 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P01: pdr.bit.P01 = status; pdr.bit.PWE01 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P02: pdr.bit.P02 = status; pdr.bit.PWE02 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P03: pdr.bit.P03 = status; pdr.bit.PWE03 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P04: pdr.bit.P04 = status; pdr.bit.PWE04 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P05: pdr.bit.P05 = status; pdr.bit.PWE05 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P06: pdr.bit.P06 = status; pdr.bit.PWE06 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P07: pdr.bit.P07 = status; pdr.bit.PWE07 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P10: pdr.bit.P10 = status; pdr.bit.PWE10 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P11: pdr.bit.P11 = status; pdr.bit.PWE11 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P12: pdr.bit.P12 = status; pdr.bit.PWE12 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P13: pdr.bit.P13 = status; pdr.bit.PWE13 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P14: pdr.bit.P14 = status; pdr.bit.PWE14 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P15: pdr.bit.P15 = status; pdr.bit.PWE15 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P16: pdr.bit.P16 = status; pdr.bit.PWE16 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P17: pdr.bit.P17 = status; pdr.bit.PWE17 = 1; index = 0; break;
		case E_DD_TOP_GPIO_CM0_P20: pdr.bit.P20 = status; pdr.bit.PWE20 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P21: pdr.bit.P21 = status; pdr.bit.PWE21 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P22: pdr.bit.P22 = status; pdr.bit.PWE22 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P23: pdr.bit.P23 = status; pdr.bit.PWE23 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P24: pdr.bit.P24 = status; pdr.bit.PWE24 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P25: pdr.bit.P25 = status; pdr.bit.PWE25 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P26: pdr.bit.P26 = status; pdr.bit.PWE26 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P27: pdr.bit.P27 = status; pdr.bit.PWE27 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P30: pdr.bit.P30 = status; pdr.bit.PWE30 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P31: pdr.bit.P31 = status; pdr.bit.PWE31 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P32: pdr.bit.P32 = status; pdr.bit.PWE32 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P33: pdr.bit.P33 = status; pdr.bit.PWE33 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P34: pdr.bit.P34 = status; pdr.bit.PWE34 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P35: pdr.bit.P35 = status; pdr.bit.PWE35 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P36: pdr.bit.P36 = status; pdr.bit.PWE36 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P37: pdr.bit.P37 = status; pdr.bit.PWE37 = 1; index = 1; break;
		case E_DD_TOP_GPIO_CM0_P44: pdr.bit.P44 = status; pdr.bit.PWE44 = 1; index = 2; break;
		case E_DD_TOP_GPIO_CM0_P45: pdr.bit.P45 = status; pdr.bit.PWE45 = 1; index = 2; break;
		case E_DD_TOP_GPIO_CM0_P46: pdr.bit.P46 = status; pdr.bit.PWE46 = 1; index = 2; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_CM0_Status() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	// Set PDR
	IO_PMC.GPIO.PDR.word[index] = pdr.word[index];

	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Get_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_PORT port, UCHAR* status )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_CM0_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_CM0_Status() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( status == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_CM0_Status() error. status parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_CM0_P00: *status = IO_PMC.GPIO.PDR.bit.P00; break;
		case E_DD_TOP_GPIO_CM0_P01: *status = IO_PMC.GPIO.PDR.bit.P01; break;
		case E_DD_TOP_GPIO_CM0_P02: *status = IO_PMC.GPIO.PDR.bit.P02; break;
		case E_DD_TOP_GPIO_CM0_P03: *status = IO_PMC.GPIO.PDR.bit.P03; break;
		case E_DD_TOP_GPIO_CM0_P04: *status = IO_PMC.GPIO.PDR.bit.P04; break;
		case E_DD_TOP_GPIO_CM0_P05: *status = IO_PMC.GPIO.PDR.bit.P05; break;
		case E_DD_TOP_GPIO_CM0_P06: *status = IO_PMC.GPIO.PDR.bit.P06; break;
		case E_DD_TOP_GPIO_CM0_P07: *status = IO_PMC.GPIO.PDR.bit.P07; break;
		case E_DD_TOP_GPIO_CM0_P10: *status = IO_PMC.GPIO.PDR.bit.P10; break;
		case E_DD_TOP_GPIO_CM0_P11: *status = IO_PMC.GPIO.PDR.bit.P11; break;
		case E_DD_TOP_GPIO_CM0_P12: *status = IO_PMC.GPIO.PDR.bit.P12; break;
		case E_DD_TOP_GPIO_CM0_P13: *status = IO_PMC.GPIO.PDR.bit.P13; break;
		case E_DD_TOP_GPIO_CM0_P14: *status = IO_PMC.GPIO.PDR.bit.P14; break;
		case E_DD_TOP_GPIO_CM0_P15: *status = IO_PMC.GPIO.PDR.bit.P15; break;
		case E_DD_TOP_GPIO_CM0_P16: *status = IO_PMC.GPIO.PDR.bit.P16; break;
		case E_DD_TOP_GPIO_CM0_P17: *status = IO_PMC.GPIO.PDR.bit.P17; break;
		case E_DD_TOP_GPIO_CM0_P20: *status = IO_PMC.GPIO.PDR.bit.P20; break;
		case E_DD_TOP_GPIO_CM0_P21: *status = IO_PMC.GPIO.PDR.bit.P21; break;
		case E_DD_TOP_GPIO_CM0_P22: *status = IO_PMC.GPIO.PDR.bit.P22; break;
		case E_DD_TOP_GPIO_CM0_P23: *status = IO_PMC.GPIO.PDR.bit.P23; break;
		case E_DD_TOP_GPIO_CM0_P24: *status = IO_PMC.GPIO.PDR.bit.P24; break;
		case E_DD_TOP_GPIO_CM0_P25: *status = IO_PMC.GPIO.PDR.bit.P25; break;
		case E_DD_TOP_GPIO_CM0_P26: *status = IO_PMC.GPIO.PDR.bit.P26; break;
		case E_DD_TOP_GPIO_CM0_P27: *status = IO_PMC.GPIO.PDR.bit.P27; break;
		case E_DD_TOP_GPIO_CM0_P30: *status = IO_PMC.GPIO.PDR.bit.P30; break;
		case E_DD_TOP_GPIO_CM0_P31: *status = IO_PMC.GPIO.PDR.bit.P31; break;
		case E_DD_TOP_GPIO_CM0_P32: *status = IO_PMC.GPIO.PDR.bit.P32; break;
		case E_DD_TOP_GPIO_CM0_P33: *status = IO_PMC.GPIO.PDR.bit.P33; break;
		case E_DD_TOP_GPIO_CM0_P34: *status = IO_PMC.GPIO.PDR.bit.P34; break;
		case E_DD_TOP_GPIO_CM0_P35: *status = IO_PMC.GPIO.PDR.bit.P35; break;
		case E_DD_TOP_GPIO_CM0_P36: *status = IO_PMC.GPIO.PDR.bit.P36; break;
		case E_DD_TOP_GPIO_CM0_P37: *status = IO_PMC.GPIO.PDR.bit.P37; break;
		case E_DD_TOP_GPIO_CM0_P44: *status = IO_PMC.GPIO.PDR.bit.P44; break;
		case E_DD_TOP_GPIO_CM0_P45: *status = IO_PMC.GPIO.PDR.bit.P45; break;
		case E_DD_TOP_GPIO_CM0_P46: *status = IO_PMC.GPIO.PDR.bit.P46; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Get_Gpio_CM0_Status() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
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
INT32 Dd_Top_Set_Gpio_Pull_Up_Down_Enable( E_DD_TOP_GPIO_PORT port, UCHAR enable )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Pull_Up_Down_Enable() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( enable > D_DD_TOP_GPIO_PUD_ON ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Pull_Up_Down_Enable() error. enable parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P80: IO_CHIPTOP.PUDER.bit.P80 = enable; break;
		case E_DD_TOP_GPIO_P81: IO_CHIPTOP.PUDER.bit.P81 = enable; break;
		case E_DD_TOP_GPIO_P82: IO_CHIPTOP.PUDER.bit.P82 = enable; break;
		case E_DD_TOP_GPIO_P83: IO_CHIPTOP.PUDER.bit.P83 = enable; break;
		case E_DD_TOP_GPIO_P86: IO_CHIPTOP.PUDER.bit.P86 = enable; break;
		case E_DD_TOP_GPIO_P87: IO_CHIPTOP.PUDER.bit.P87 = enable; break;
		case E_DD_TOP_GPIO_P90: IO_CHIPTOP.PUDER.bit.P90 = enable; break;
		case E_DD_TOP_GPIO_P91: IO_CHIPTOP.PUDER.bit.P91 = enable; break;
		case E_DD_TOP_GPIO_P92: IO_CHIPTOP.PUDER.bit.P92 = enable; break;
		case E_DD_TOP_GPIO_P93: IO_CHIPTOP.PUDER.bit.P93 = enable; break;
		case E_DD_TOP_GPIO_PA0: IO_CHIPTOP.PUDER.bit.PA0 = enable; break;
		case E_DD_TOP_GPIO_PA1: IO_CHIPTOP.PUDER.bit.PA1 = enable; break;
		case E_DD_TOP_GPIO_PA2: IO_CHIPTOP.PUDER.bit.PA2 = enable; break;
		case E_DD_TOP_GPIO_PA3: IO_CHIPTOP.PUDER.bit.PA3 = enable; break;
		case E_DD_TOP_GPIO_PA4: IO_CHIPTOP.PUDER.bit.PA4 = enable; break;
		case E_DD_TOP_GPIO_PA5: IO_CHIPTOP.PUDER.bit.PA5 = enable; break;
		case E_DD_TOP_GPIO_PA6: IO_CHIPTOP.PUDER.bit.PA6 = enable; break;
		case E_DD_TOP_GPIO_PA7: IO_CHIPTOP.PUDER.bit.PA7 = enable; break;
		case E_DD_TOP_GPIO_PB0: IO_CHIPTOP.PUDER.bit.PB0 = enable; break;
		case E_DD_TOP_GPIO_PB1: IO_CHIPTOP.PUDER.bit.PB1 = enable; break;
		case E_DD_TOP_GPIO_PB2: IO_CHIPTOP.PUDER.bit.PB2 = enable; break;
		case E_DD_TOP_GPIO_PB3: IO_CHIPTOP.PUDER.bit.PB3 = enable; break;
		case E_DD_TOP_GPIO_PB4: IO_CHIPTOP.PUDER.bit.PB4 = enable; break;
		case E_DD_TOP_GPIO_PD0: IO_CHIPTOP.PUDER.bit.PD0 = enable; break;
		case E_DD_TOP_GPIO_PD1: IO_CHIPTOP.PUDER.bit.PD1 = enable; break;
		case E_DD_TOP_GPIO_PD2: IO_CHIPTOP.PUDER.bit.PD2 = enable; break;
		case E_DD_TOP_GPIO_PD3: IO_CHIPTOP.PUDER.bit.PD3 = enable; break;
		case E_DD_TOP_GPIO_PD4: IO_CHIPTOP.PUDER.bit.PD4 = enable; break;
		case E_DD_TOP_GPIO_PD5: IO_CHIPTOP.PUDER.bit.PD5 = enable; break;
		case E_DD_TOP_GPIO_PD6: IO_CHIPTOP.PUDER.bit.PD6 = enable; break;
		case E_DD_TOP_GPIO_PE0: IO_CHIPTOP.PUDER.bit.PE0 = enable; break;
		case E_DD_TOP_GPIO_PE1: IO_CHIPTOP.PUDER.bit.PE1 = enable; break;
		case E_DD_TOP_GPIO_PF2: IO_CHIPTOP.PUDER.bit.PF2 = enable; break;
		case E_DD_TOP_GPIO_PF3: IO_CHIPTOP.PUDER.bit.PF3 = enable; break;
		case E_DD_TOP_GPIO_PF4: IO_CHIPTOP.PUDER.bit.PF4 = enable; break;
		case E_DD_TOP_GPIO_PG0: IO_CHIPTOP.PUDER.bit.PG0 = enable; break;
		case E_DD_TOP_GPIO_PG1: IO_CHIPTOP.PUDER.bit.PG1 = enable; break;
		case E_DD_TOP_GPIO_PG2: IO_CHIPTOP.PUDER.bit.PG2 = enable; break;
		case E_DD_TOP_GPIO_PG3: IO_CHIPTOP.PUDER.bit.PG3 = enable; break;
		case E_DD_TOP_GPIO_PG4: IO_CHIPTOP.PUDER.bit.PG4 = enable; break;
		case E_DD_TOP_GPIO_PG5: IO_CHIPTOP.PUDER.bit.PG5 = enable; break;
		case E_DD_TOP_GPIO_PG6: IO_CHIPTOP.PUDER.bit.PG6 = enable; break;
		case E_DD_TOP_GPIO_PG7: IO_CHIPTOP.PUDER.bit.PG7 = enable; break;
		case E_DD_TOP_GPIO_PH0: IO_CHIPTOP.PUDER.bit.PH0 = enable; break;
		case E_DD_TOP_GPIO_PH1: IO_CHIPTOP.PUDER.bit.PH1 = enable; break;
		case E_DD_TOP_GPIO_PH2: IO_CHIPTOP.PUDER.bit.PH2 = enable; break;
		case E_DD_TOP_GPIO_PH3: IO_CHIPTOP.PUDER.bit.PH3 = enable; break;
		case E_DD_TOP_GPIO_PW0: IO_CHIPTOP.PUDER.bit.PW0 = enable; break;
		case E_DD_TOP_GPIO_PW1: IO_CHIPTOP.PUDER.bit.PW1 = enable; break;
		case E_DD_TOP_GPIO_PW2: IO_CHIPTOP.PUDER.bit.PW2 = enable; break;
		case E_DD_TOP_GPIO_PW3: IO_CHIPTOP.PUDER.bit.PW3 = enable; break;
		case E_DD_TOP_GPIO_PW4: IO_CHIPTOP.PUDER.bit.PW4 = enable; break;
		case E_DD_TOP_GPIO_PW5: IO_CHIPTOP.PUDER.bit.PW5 = enable; break;
		case E_DD_TOP_GPIO_PW6: IO_CHIPTOP.PUDER.bit.PW6 = enable; break;
		case E_DD_TOP_GPIO_PW7: IO_CHIPTOP.PUDER.bit.PW7 = enable; break;
		case E_DD_TOP_GPIO_PJ0: IO_CHIPTOP.PUDER.bit.PJ0 = enable; break;
		case E_DD_TOP_GPIO_PJ1: IO_CHIPTOP.PUDER.bit.PJ1 = enable; break;
		case E_DD_TOP_GPIO_PJ2: IO_CHIPTOP.PUDER.bit.PJ2 = enable; break;
		case E_DD_TOP_GPIO_PJ3: IO_CHIPTOP.PUDER.bit.PJ3 = enable; break;
		case E_DD_TOP_GPIO_PJ4: IO_CHIPTOP.PUDER.bit.PJ4 = enable; break;
		case E_DD_TOP_GPIO_PJ5: IO_CHIPTOP.PUDER.bit.PJ5 = enable; break;
		case E_DD_TOP_GPIO_PJ6: IO_CHIPTOP.PUDER.bit.PJ6 = enable; break;
		case E_DD_TOP_GPIO_PJ7: IO_CHIPTOP.PUDER.bit.PJ7 = enable; break;
		case E_DD_TOP_GPIO_PK0: IO_CHIPTOP.PUDER.bit.PK0 = enable; break;
		case E_DD_TOP_GPIO_PK1: IO_CHIPTOP.PUDER.bit.PK1 = enable; break;
		case E_DD_TOP_GPIO_PK2: IO_CHIPTOP.PUDER.bit.PK2 = enable; break;
		case E_DD_TOP_GPIO_PK3: IO_CHIPTOP.PUDER.bit.PK3 = enable; break;
		case E_DD_TOP_GPIO_PK4: IO_CHIPTOP.PUDER.bit.PK4 = enable; break;
		case E_DD_TOP_GPIO_PK5: IO_CHIPTOP.PUDER.bit.PK5 = enable; break;
		case E_DD_TOP_GPIO_PK6: IO_CHIPTOP.PUDER.bit.PK6 = enable; break;
		case E_DD_TOP_GPIO_PK7: IO_CHIPTOP.PUDER.bit.PK7 = enable; break;
		case E_DD_TOP_GPIO_PL0: IO_CHIPTOP.PUDER.bit.PL0 = enable; break;
		case E_DD_TOP_GPIO_PL1: IO_CHIPTOP.PUDER.bit.PL1 = enable; break;
		case E_DD_TOP_GPIO_PL2: IO_CHIPTOP.PUDER.bit.PL2 = enable; break;
		case E_DD_TOP_GPIO_PL3: IO_CHIPTOP.PUDER.bit.PL3 = enable; break;
		case E_DD_TOP_GPIO_PL4: IO_CHIPTOP.PUDER.bit.PL4 = enable; break;
		case E_DD_TOP_GPIO_PL5: IO_CHIPTOP.PUDER.bit.PL5 = enable; break;
		case E_DD_TOP_GPIO_PM0: IO_CHIPTOP.PUDER.bit.PM0 = enable; break;
		case E_DD_TOP_GPIO_PM1: IO_CHIPTOP.PUDER.bit.PM1 = enable; break;
		case E_DD_TOP_GPIO_PM3: IO_CHIPTOP.PUDER.bit.PM3 = enable; break;
		case E_DD_TOP_GPIO_PM4: IO_CHIPTOP.PUDER.bit.PM4 = enable; break;
		case E_DD_TOP_GPIO_PN0: IO_CHIPTOP.PUDER.bit.PN0 = enable; break;
		case E_DD_TOP_GPIO_PN1: IO_CHIPTOP.PUDER.bit.PN1 = enable; break;
		case E_DD_TOP_GPIO_PN3: IO_CHIPTOP.PUDER.bit.PN3 = enable; break;
		case E_DD_TOP_GPIO_PN4: IO_CHIPTOP.PUDER.bit.PN4 = enable; break;
		case E_DD_TOP_GPIO_PY0: IO_CHIPTOP.PUDER.bit.PY0 = enable; break;
		case E_DD_TOP_GPIO_PY1: IO_CHIPTOP.PUDER.bit.PY1 = enable; break;
		case E_DD_TOP_GPIO_PY2: IO_CHIPTOP.PUDER.bit.PY2 = enable; break;
		case E_DD_TOP_GPIO_PY3: IO_CHIPTOP.PUDER.bit.PY3 = enable; break;
		case E_DD_TOP_GPIO_PY4: IO_CHIPTOP.PUDER.bit.PY4 = enable; break;
		case E_DD_TOP_GPIO_PY5: IO_CHIPTOP.PUDER.bit.PY5 = enable; break;
		case E_DD_TOP_GPIO_PY6: IO_CHIPTOP.PUDER.bit.PY6 = enable; break;
		case E_DD_TOP_GPIO_PY7: IO_CHIPTOP.PUDER.bit.PY7 = enable; break;
		case E_DD_TOP_GPIO_PP0: IO_CHIPTOP.PUDER.bit.PP0 = enable; break;
		case E_DD_TOP_GPIO_PP1: IO_CHIPTOP.PUDER.bit.PP1 = enable; break;
		case E_DD_TOP_GPIO_PP2: IO_CHIPTOP.PUDER.bit.PP2 = enable; break;
		case E_DD_TOP_GPIO_PP3: IO_CHIPTOP.PUDER.bit.PP3 = enable; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_Pull_Up_Down_Enable() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Get_Gpio_Pull_Up_Down_Enable( E_DD_TOP_GPIO_PORT port, UCHAR* enable )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Pull_Up_Down_Enable() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( enable == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Pull_Up_Down_Enable() error. enable parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P80: *enable = IO_CHIPTOP.PUDER.bit.P80; break;
		case E_DD_TOP_GPIO_P81: *enable = IO_CHIPTOP.PUDER.bit.P81; break;
		case E_DD_TOP_GPIO_P82: *enable = IO_CHIPTOP.PUDER.bit.P82; break;
		case E_DD_TOP_GPIO_P83: *enable = IO_CHIPTOP.PUDER.bit.P83; break;
		case E_DD_TOP_GPIO_P86: *enable = IO_CHIPTOP.PUDER.bit.P86; break;
		case E_DD_TOP_GPIO_P87: *enable = IO_CHIPTOP.PUDER.bit.P87; break;
		case E_DD_TOP_GPIO_P90: *enable = IO_CHIPTOP.PUDER.bit.P90; break;
		case E_DD_TOP_GPIO_P91: *enable = IO_CHIPTOP.PUDER.bit.P91; break;
		case E_DD_TOP_GPIO_P92: *enable = IO_CHIPTOP.PUDER.bit.P92; break;
		case E_DD_TOP_GPIO_P93: *enable = IO_CHIPTOP.PUDER.bit.P93; break;
		case E_DD_TOP_GPIO_PA0: *enable = IO_CHIPTOP.PUDER.bit.PA0; break;
		case E_DD_TOP_GPIO_PA1: *enable = IO_CHIPTOP.PUDER.bit.PA1; break;
		case E_DD_TOP_GPIO_PA2: *enable = IO_CHIPTOP.PUDER.bit.PA2; break;
		case E_DD_TOP_GPIO_PA3: *enable = IO_CHIPTOP.PUDER.bit.PA3; break;
		case E_DD_TOP_GPIO_PA4: *enable = IO_CHIPTOP.PUDER.bit.PA4; break;
		case E_DD_TOP_GPIO_PA5: *enable = IO_CHIPTOP.PUDER.bit.PA5; break;
		case E_DD_TOP_GPIO_PA6: *enable = IO_CHIPTOP.PUDER.bit.PA6; break;
		case E_DD_TOP_GPIO_PA7: *enable = IO_CHIPTOP.PUDER.bit.PA7; break;
		case E_DD_TOP_GPIO_PB0: *enable = IO_CHIPTOP.PUDER.bit.PB0; break;
		case E_DD_TOP_GPIO_PB1: *enable = IO_CHIPTOP.PUDER.bit.PB1; break;
		case E_DD_TOP_GPIO_PB2: *enable = IO_CHIPTOP.PUDER.bit.PB2; break;
		case E_DD_TOP_GPIO_PB3: *enable = IO_CHIPTOP.PUDER.bit.PB3; break;
		case E_DD_TOP_GPIO_PB4: *enable = IO_CHIPTOP.PUDER.bit.PB4; break;
		case E_DD_TOP_GPIO_PD0: *enable = IO_CHIPTOP.PUDER.bit.PD0; break;
		case E_DD_TOP_GPIO_PD1: *enable = IO_CHIPTOP.PUDER.bit.PD1; break;
		case E_DD_TOP_GPIO_PD2: *enable = IO_CHIPTOP.PUDER.bit.PD2; break;
		case E_DD_TOP_GPIO_PD3: *enable = IO_CHIPTOP.PUDER.bit.PD3; break;
		case E_DD_TOP_GPIO_PD4: *enable = IO_CHIPTOP.PUDER.bit.PD4; break;
		case E_DD_TOP_GPIO_PD5: *enable = IO_CHIPTOP.PUDER.bit.PD5; break;
		case E_DD_TOP_GPIO_PD6: *enable = IO_CHIPTOP.PUDER.bit.PD6; break;
		case E_DD_TOP_GPIO_PE0: *enable = IO_CHIPTOP.PUDER.bit.PE0; break;
		case E_DD_TOP_GPIO_PE1: *enable = IO_CHIPTOP.PUDER.bit.PE1; break;
		case E_DD_TOP_GPIO_PF2: *enable = IO_CHIPTOP.PUDER.bit.PF2; break;
		case E_DD_TOP_GPIO_PF3: *enable = IO_CHIPTOP.PUDER.bit.PF3; break;
		case E_DD_TOP_GPIO_PF4: *enable = IO_CHIPTOP.PUDER.bit.PF4; break;
		case E_DD_TOP_GPIO_PG0: *enable = IO_CHIPTOP.PUDER.bit.PG0; break;
		case E_DD_TOP_GPIO_PG1: *enable = IO_CHIPTOP.PUDER.bit.PG1; break;
		case E_DD_TOP_GPIO_PG2: *enable = IO_CHIPTOP.PUDER.bit.PG2; break;
		case E_DD_TOP_GPIO_PG3: *enable = IO_CHIPTOP.PUDER.bit.PG3; break;
		case E_DD_TOP_GPIO_PG4: *enable = IO_CHIPTOP.PUDER.bit.PG4; break;
		case E_DD_TOP_GPIO_PG5: *enable = IO_CHIPTOP.PUDER.bit.PG5; break;
		case E_DD_TOP_GPIO_PG6: *enable = IO_CHIPTOP.PUDER.bit.PG6; break;
		case E_DD_TOP_GPIO_PG7: *enable = IO_CHIPTOP.PUDER.bit.PG7; break;
		case E_DD_TOP_GPIO_PH0: *enable = IO_CHIPTOP.PUDER.bit.PH0; break;
		case E_DD_TOP_GPIO_PH1: *enable = IO_CHIPTOP.PUDER.bit.PH1; break;
		case E_DD_TOP_GPIO_PH2: *enable = IO_CHIPTOP.PUDER.bit.PH2; break;
		case E_DD_TOP_GPIO_PH3: *enable = IO_CHIPTOP.PUDER.bit.PH3; break;
		case E_DD_TOP_GPIO_PW0: *enable = IO_CHIPTOP.PUDER.bit.PW0; break;
		case E_DD_TOP_GPIO_PW1: *enable = IO_CHIPTOP.PUDER.bit.PW1; break;
		case E_DD_TOP_GPIO_PW2: *enable = IO_CHIPTOP.PUDER.bit.PW2; break;
		case E_DD_TOP_GPIO_PW3: *enable = IO_CHIPTOP.PUDER.bit.PW3; break;
		case E_DD_TOP_GPIO_PW4: *enable = IO_CHIPTOP.PUDER.bit.PW4; break;
		case E_DD_TOP_GPIO_PW5: *enable = IO_CHIPTOP.PUDER.bit.PW5; break;
		case E_DD_TOP_GPIO_PW6: *enable = IO_CHIPTOP.PUDER.bit.PW6; break;
		case E_DD_TOP_GPIO_PW7: *enable = IO_CHIPTOP.PUDER.bit.PW7; break;
		case E_DD_TOP_GPIO_PJ0: *enable = IO_CHIPTOP.PUDER.bit.PJ0; break;
		case E_DD_TOP_GPIO_PJ1: *enable = IO_CHIPTOP.PUDER.bit.PJ1; break;
		case E_DD_TOP_GPIO_PJ2: *enable = IO_CHIPTOP.PUDER.bit.PJ2; break;
		case E_DD_TOP_GPIO_PJ3: *enable = IO_CHIPTOP.PUDER.bit.PJ3; break;
		case E_DD_TOP_GPIO_PJ4: *enable = IO_CHIPTOP.PUDER.bit.PJ4; break;
		case E_DD_TOP_GPIO_PJ5: *enable = IO_CHIPTOP.PUDER.bit.PJ5; break;
		case E_DD_TOP_GPIO_PJ6: *enable = IO_CHIPTOP.PUDER.bit.PJ6; break;
		case E_DD_TOP_GPIO_PJ7: *enable = IO_CHIPTOP.PUDER.bit.PJ7; break;
		case E_DD_TOP_GPIO_PK0: *enable = IO_CHIPTOP.PUDER.bit.PK0; break;
		case E_DD_TOP_GPIO_PK1: *enable = IO_CHIPTOP.PUDER.bit.PK1; break;
		case E_DD_TOP_GPIO_PK2: *enable = IO_CHIPTOP.PUDER.bit.PK2; break;
		case E_DD_TOP_GPIO_PK3: *enable = IO_CHIPTOP.PUDER.bit.PK3; break;
		case E_DD_TOP_GPIO_PK4: *enable = IO_CHIPTOP.PUDER.bit.PK4; break;
		case E_DD_TOP_GPIO_PK5: *enable = IO_CHIPTOP.PUDER.bit.PK5; break;
		case E_DD_TOP_GPIO_PK6: *enable = IO_CHIPTOP.PUDER.bit.PK6; break;
		case E_DD_TOP_GPIO_PK7: *enable = IO_CHIPTOP.PUDER.bit.PK7; break;
		case E_DD_TOP_GPIO_PL0: *enable = IO_CHIPTOP.PUDER.bit.PL0; break;
		case E_DD_TOP_GPIO_PL1: *enable = IO_CHIPTOP.PUDER.bit.PL1; break;
		case E_DD_TOP_GPIO_PL2: *enable = IO_CHIPTOP.PUDER.bit.PL2; break;
		case E_DD_TOP_GPIO_PL3: *enable = IO_CHIPTOP.PUDER.bit.PL3; break;
		case E_DD_TOP_GPIO_PL4: *enable = IO_CHIPTOP.PUDER.bit.PL4; break;
		case E_DD_TOP_GPIO_PL5: *enable = IO_CHIPTOP.PUDER.bit.PL5; break;
		case E_DD_TOP_GPIO_PM0: *enable = IO_CHIPTOP.PUDER.bit.PM0; break;
		case E_DD_TOP_GPIO_PM1: *enable = IO_CHIPTOP.PUDER.bit.PM1; break;
		case E_DD_TOP_GPIO_PM3: *enable = IO_CHIPTOP.PUDER.bit.PM3; break;
		case E_DD_TOP_GPIO_PM4: *enable = IO_CHIPTOP.PUDER.bit.PM4; break;
		case E_DD_TOP_GPIO_PN0: *enable = IO_CHIPTOP.PUDER.bit.PN0; break;
		case E_DD_TOP_GPIO_PN1: *enable = IO_CHIPTOP.PUDER.bit.PN1; break;
		case E_DD_TOP_GPIO_PN3: *enable = IO_CHIPTOP.PUDER.bit.PN3; break;
		case E_DD_TOP_GPIO_PN4: *enable = IO_CHIPTOP.PUDER.bit.PN4; break;
		case E_DD_TOP_GPIO_PY0: *enable = IO_CHIPTOP.PUDER.bit.PY0; break;
		case E_DD_TOP_GPIO_PY1: *enable = IO_CHIPTOP.PUDER.bit.PY1; break;
		case E_DD_TOP_GPIO_PY2: *enable = IO_CHIPTOP.PUDER.bit.PY2; break;
		case E_DD_TOP_GPIO_PY3: *enable = IO_CHIPTOP.PUDER.bit.PY3; break;
		case E_DD_TOP_GPIO_PY4: *enable = IO_CHIPTOP.PUDER.bit.PY4; break;
		case E_DD_TOP_GPIO_PY5: *enable = IO_CHIPTOP.PUDER.bit.PY5; break;
		case E_DD_TOP_GPIO_PY6: *enable = IO_CHIPTOP.PUDER.bit.PY6; break;
		case E_DD_TOP_GPIO_PY7: *enable = IO_CHIPTOP.PUDER.bit.PY7; break;
		case E_DD_TOP_GPIO_PP0: *enable = IO_CHIPTOP.PUDER.bit.PP0; break;
		case E_DD_TOP_GPIO_PP1: *enable = IO_CHIPTOP.PUDER.bit.PP1; break;
		case E_DD_TOP_GPIO_PP2: *enable = IO_CHIPTOP.PUDER.bit.PP2; break;
		case E_DD_TOP_GPIO_PP3: *enable = IO_CHIPTOP.PUDER.bit.PP3; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Get_Gpio_Pull_Up_Down_Enable() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_PORT port, UCHAR control )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( control > D_DD_TOP_GPIO_PUD_UP ) {
		Ddim_Assertion(("Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl() error. control parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P80: IO_CHIPTOP.PUDCR.bit.P80 = control; break;
		case E_DD_TOP_GPIO_P81: IO_CHIPTOP.PUDCR.bit.P81 = control; break;
		case E_DD_TOP_GPIO_P82: IO_CHIPTOP.PUDCR.bit.P82 = control; break;
		case E_DD_TOP_GPIO_P83: IO_CHIPTOP.PUDCR.bit.P83 = control; break;
		case E_DD_TOP_GPIO_P86: IO_CHIPTOP.PUDCR.bit.P86 = control; break;
		case E_DD_TOP_GPIO_P87: IO_CHIPTOP.PUDCR.bit.P87 = control; break;
		case E_DD_TOP_GPIO_P90: IO_CHIPTOP.PUDCR.bit.P90 = control; break;
		case E_DD_TOP_GPIO_P91: IO_CHIPTOP.PUDCR.bit.P91 = control; break;
		case E_DD_TOP_GPIO_P92: IO_CHIPTOP.PUDCR.bit.P92 = control; break;
		case E_DD_TOP_GPIO_P93: IO_CHIPTOP.PUDCR.bit.P93 = control; break;
		case E_DD_TOP_GPIO_PA0: IO_CHIPTOP.PUDCR.bit.PA0 = control; break;
		case E_DD_TOP_GPIO_PA1: IO_CHIPTOP.PUDCR.bit.PA1 = control; break;
		case E_DD_TOP_GPIO_PA2: IO_CHIPTOP.PUDCR.bit.PA2 = control; break;
		case E_DD_TOP_GPIO_PA3: IO_CHIPTOP.PUDCR.bit.PA3 = control; break;
		case E_DD_TOP_GPIO_PA4: IO_CHIPTOP.PUDCR.bit.PA4 = control; break;
		case E_DD_TOP_GPIO_PA5: IO_CHIPTOP.PUDCR.bit.PA5 = control; break;
		case E_DD_TOP_GPIO_PA6: IO_CHIPTOP.PUDCR.bit.PA6 = control; break;
		case E_DD_TOP_GPIO_PA7: IO_CHIPTOP.PUDCR.bit.PA7 = control; break;
		case E_DD_TOP_GPIO_PB0: IO_CHIPTOP.PUDCR.bit.PB0 = control; break;
		case E_DD_TOP_GPIO_PB1: IO_CHIPTOP.PUDCR.bit.PB1 = control; break;
		case E_DD_TOP_GPIO_PB2: IO_CHIPTOP.PUDCR.bit.PB2 = control; break;
		case E_DD_TOP_GPIO_PB3: IO_CHIPTOP.PUDCR.bit.PB3 = control; break;
		case E_DD_TOP_GPIO_PB4: IO_CHIPTOP.PUDCR.bit.PB4 = control; break;
		case E_DD_TOP_GPIO_PD0: IO_CHIPTOP.PUDCR.bit.PD0 = control; break;
		case E_DD_TOP_GPIO_PD1: IO_CHIPTOP.PUDCR.bit.PD1 = control; break;
		case E_DD_TOP_GPIO_PD2: IO_CHIPTOP.PUDCR.bit.PD2 = control; break;
		case E_DD_TOP_GPIO_PD3: IO_CHIPTOP.PUDCR.bit.PD3 = control; break;
		case E_DD_TOP_GPIO_PD4: IO_CHIPTOP.PUDCR.bit.PD4 = control; break;
		case E_DD_TOP_GPIO_PD5: IO_CHIPTOP.PUDCR.bit.PD5 = control; break;
		case E_DD_TOP_GPIO_PD6: IO_CHIPTOP.PUDCR.bit.PD6 = control; break;
		case E_DD_TOP_GPIO_PE0: IO_CHIPTOP.PUDCR.bit.PE0 = control; break;
		case E_DD_TOP_GPIO_PE1: IO_CHIPTOP.PUDCR.bit.PE1 = control; break;
		case E_DD_TOP_GPIO_PF2: IO_CHIPTOP.PUDCR.bit.PF2 = control; break;
		case E_DD_TOP_GPIO_PF3: IO_CHIPTOP.PUDCR.bit.PF3 = control; break;
		case E_DD_TOP_GPIO_PF4: IO_CHIPTOP.PUDCR.bit.PF4 = control; break;
		case E_DD_TOP_GPIO_PG0: IO_CHIPTOP.PUDCR.bit.PG0 = control; break;
		case E_DD_TOP_GPIO_PG1: IO_CHIPTOP.PUDCR.bit.PG1 = control; break;
		case E_DD_TOP_GPIO_PG2: IO_CHIPTOP.PUDCR.bit.PG2 = control; break;
		case E_DD_TOP_GPIO_PG3: IO_CHIPTOP.PUDCR.bit.PG3 = control; break;
		case E_DD_TOP_GPIO_PG4: IO_CHIPTOP.PUDCR.bit.PG4 = control; break;
		case E_DD_TOP_GPIO_PG5: IO_CHIPTOP.PUDCR.bit.PG5 = control; break;
		case E_DD_TOP_GPIO_PG6: IO_CHIPTOP.PUDCR.bit.PG6 = control; break;
		case E_DD_TOP_GPIO_PG7: IO_CHIPTOP.PUDCR.bit.PG7 = control; break;
		case E_DD_TOP_GPIO_PH0: IO_CHIPTOP.PUDCR.bit.PH0 = control; break;
		case E_DD_TOP_GPIO_PH1: IO_CHIPTOP.PUDCR.bit.PH1 = control; break;
		case E_DD_TOP_GPIO_PH2: IO_CHIPTOP.PUDCR.bit.PH2 = control; break;
		case E_DD_TOP_GPIO_PH3: IO_CHIPTOP.PUDCR.bit.PH3 = control; break;
		case E_DD_TOP_GPIO_PW0: IO_CHIPTOP.PUDCR.bit.PW0 = control; break;
		case E_DD_TOP_GPIO_PW1: IO_CHIPTOP.PUDCR.bit.PW1 = control; break;
		case E_DD_TOP_GPIO_PW2: IO_CHIPTOP.PUDCR.bit.PW2 = control; break;
		case E_DD_TOP_GPIO_PW3: IO_CHIPTOP.PUDCR.bit.PW3 = control; break;
		case E_DD_TOP_GPIO_PW4: IO_CHIPTOP.PUDCR.bit.PW4 = control; break;
		case E_DD_TOP_GPIO_PW5: IO_CHIPTOP.PUDCR.bit.PW5 = control; break;
		case E_DD_TOP_GPIO_PW6: IO_CHIPTOP.PUDCR.bit.PW6 = control; break;
		case E_DD_TOP_GPIO_PW7: IO_CHIPTOP.PUDCR.bit.PW7 = control; break;
		case E_DD_TOP_GPIO_PJ0: IO_CHIPTOP.PUDCR.bit.PJ0 = control; break;
		case E_DD_TOP_GPIO_PJ1: IO_CHIPTOP.PUDCR.bit.PJ1 = control; break;
		case E_DD_TOP_GPIO_PJ2: IO_CHIPTOP.PUDCR.bit.PJ2 = control; break;
		case E_DD_TOP_GPIO_PJ3: IO_CHIPTOP.PUDCR.bit.PJ3 = control; break;
		case E_DD_TOP_GPIO_PJ4: IO_CHIPTOP.PUDCR.bit.PJ4 = control; break;
		case E_DD_TOP_GPIO_PJ5: IO_CHIPTOP.PUDCR.bit.PJ5 = control; break;
		case E_DD_TOP_GPIO_PJ6: IO_CHIPTOP.PUDCR.bit.PJ6 = control; break;
		case E_DD_TOP_GPIO_PJ7: IO_CHIPTOP.PUDCR.bit.PJ7 = control; break;
		case E_DD_TOP_GPIO_PK0: IO_CHIPTOP.PUDCR.bit.PK0 = control; break;
		case E_DD_TOP_GPIO_PK1: IO_CHIPTOP.PUDCR.bit.PK1 = control; break;
		case E_DD_TOP_GPIO_PK2: IO_CHIPTOP.PUDCR.bit.PK2 = control; break;
		case E_DD_TOP_GPIO_PK3: IO_CHIPTOP.PUDCR.bit.PK3 = control; break;
		case E_DD_TOP_GPIO_PK4: IO_CHIPTOP.PUDCR.bit.PK4 = control; break;
		case E_DD_TOP_GPIO_PK5: IO_CHIPTOP.PUDCR.bit.PK5 = control; break;
		case E_DD_TOP_GPIO_PK6: IO_CHIPTOP.PUDCR.bit.PK6 = control; break;
		case E_DD_TOP_GPIO_PK7: IO_CHIPTOP.PUDCR.bit.PK7 = control; break;
		case E_DD_TOP_GPIO_PL0: IO_CHIPTOP.PUDCR.bit.PL0 = control; break;
		case E_DD_TOP_GPIO_PL1: IO_CHIPTOP.PUDCR.bit.PL1 = control; break;
		case E_DD_TOP_GPIO_PL2: IO_CHIPTOP.PUDCR.bit.PL2 = control; break;
		case E_DD_TOP_GPIO_PL3: IO_CHIPTOP.PUDCR.bit.PL3 = control; break;
		case E_DD_TOP_GPIO_PL4: IO_CHIPTOP.PUDCR.bit.PL4 = control; break;
		case E_DD_TOP_GPIO_PL5: IO_CHIPTOP.PUDCR.bit.PL5 = control; break;
		case E_DD_TOP_GPIO_PM0: IO_CHIPTOP.PUDCR.bit.PM0 = control; break;
		case E_DD_TOP_GPIO_PM1: IO_CHIPTOP.PUDCR.bit.PM1 = control; break;
		case E_DD_TOP_GPIO_PM3: IO_CHIPTOP.PUDCR.bit.PM3 = control; break;
		case E_DD_TOP_GPIO_PM4: IO_CHIPTOP.PUDCR.bit.PM4 = control; break;
		case E_DD_TOP_GPIO_PN0: IO_CHIPTOP.PUDCR.bit.PN0 = control; break;
		case E_DD_TOP_GPIO_PN1: IO_CHIPTOP.PUDCR.bit.PN1 = control; break;
		case E_DD_TOP_GPIO_PN3: IO_CHIPTOP.PUDCR.bit.PN3 = control; break;
		case E_DD_TOP_GPIO_PN4: IO_CHIPTOP.PUDCR.bit.PN4 = control; break;
		case E_DD_TOP_GPIO_PY0: IO_CHIPTOP.PUDCR.bit.PY0 = control; break;
		case E_DD_TOP_GPIO_PY1: IO_CHIPTOP.PUDCR.bit.PY1 = control; break;
		case E_DD_TOP_GPIO_PY2: IO_CHIPTOP.PUDCR.bit.PY2 = control; break;
		case E_DD_TOP_GPIO_PY3: IO_CHIPTOP.PUDCR.bit.PY3 = control; break;
		case E_DD_TOP_GPIO_PY4: IO_CHIPTOP.PUDCR.bit.PY4 = control; break;
		case E_DD_TOP_GPIO_PY5: IO_CHIPTOP.PUDCR.bit.PY5 = control; break;
		case E_DD_TOP_GPIO_PY6: IO_CHIPTOP.PUDCR.bit.PY6 = control; break;
		case E_DD_TOP_GPIO_PY7: IO_CHIPTOP.PUDCR.bit.PY7 = control; break;
		case E_DD_TOP_GPIO_PP0: IO_CHIPTOP.PUDCR.bit.PP0 = control; break;
		case E_DD_TOP_GPIO_PP1: IO_CHIPTOP.PUDCR.bit.PP1 = control; break;
		case E_DD_TOP_GPIO_PP2: IO_CHIPTOP.PUDCR.bit.PP2 = control; break;
		case E_DD_TOP_GPIO_PP3: IO_CHIPTOP.PUDCR.bit.PP3 = control; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_ARM_Dsb_Pou();
	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}

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
INT32 Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_PORT port, UCHAR* control )
{
#ifdef CO_ACT_GPIO_CLOCK_CONTROL
	INT32 s;
#endif // CO_ACT_GPIO_CLOCK_CONTROL

#ifdef CO_PARAM_CHECK
	if( port >= E_DD_TOP_GPIO_PORT_MAX ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl() error. port parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
	if( control == NULL ) {
		Ddim_Assertion(("Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl() error. control parameters is abnormal.\n"));
		return D_DD_TOP_INPUT_PARAM_ERROR;
	}
#endif

	Dd_Top_Start_GPIO_CLK_WITH_CS();

	switch( port ) {
		case E_DD_TOP_GPIO_P80: *control = IO_CHIPTOP.PUDCR.bit.P80; break;
		case E_DD_TOP_GPIO_P81: *control = IO_CHIPTOP.PUDCR.bit.P81; break;
		case E_DD_TOP_GPIO_P82: *control = IO_CHIPTOP.PUDCR.bit.P82; break;
		case E_DD_TOP_GPIO_P83: *control = IO_CHIPTOP.PUDCR.bit.P83; break;
		case E_DD_TOP_GPIO_P86: *control = IO_CHIPTOP.PUDCR.bit.P86; break;
		case E_DD_TOP_GPIO_P87: *control = IO_CHIPTOP.PUDCR.bit.P87; break;
		case E_DD_TOP_GPIO_P90: *control = IO_CHIPTOP.PUDCR.bit.P90; break;
		case E_DD_TOP_GPIO_P91: *control = IO_CHIPTOP.PUDCR.bit.P91; break;
		case E_DD_TOP_GPIO_P92: *control = IO_CHIPTOP.PUDCR.bit.P92; break;
		case E_DD_TOP_GPIO_P93: *control = IO_CHIPTOP.PUDCR.bit.P93; break;
		case E_DD_TOP_GPIO_PA0: *control = IO_CHIPTOP.PUDCR.bit.PA0; break;
		case E_DD_TOP_GPIO_PA1: *control = IO_CHIPTOP.PUDCR.bit.PA1; break;
		case E_DD_TOP_GPIO_PA2: *control = IO_CHIPTOP.PUDCR.bit.PA2; break;
		case E_DD_TOP_GPIO_PA3: *control = IO_CHIPTOP.PUDCR.bit.PA3; break;
		case E_DD_TOP_GPIO_PA4: *control = IO_CHIPTOP.PUDCR.bit.PA4; break;
		case E_DD_TOP_GPIO_PA5: *control = IO_CHIPTOP.PUDCR.bit.PA5; break;
		case E_DD_TOP_GPIO_PA6: *control = IO_CHIPTOP.PUDCR.bit.PA6; break;
		case E_DD_TOP_GPIO_PA7: *control = IO_CHIPTOP.PUDCR.bit.PA7; break;
		case E_DD_TOP_GPIO_PB0: *control = IO_CHIPTOP.PUDCR.bit.PB0; break;
		case E_DD_TOP_GPIO_PB1: *control = IO_CHIPTOP.PUDCR.bit.PB1; break;
		case E_DD_TOP_GPIO_PB2: *control = IO_CHIPTOP.PUDCR.bit.PB2; break;
		case E_DD_TOP_GPIO_PB3: *control = IO_CHIPTOP.PUDCR.bit.PB3; break;
		case E_DD_TOP_GPIO_PB4: *control = IO_CHIPTOP.PUDCR.bit.PB4; break;
		case E_DD_TOP_GPIO_PD0: *control = IO_CHIPTOP.PUDCR.bit.PD0; break;
		case E_DD_TOP_GPIO_PD1: *control = IO_CHIPTOP.PUDCR.bit.PD1; break;
		case E_DD_TOP_GPIO_PD2: *control = IO_CHIPTOP.PUDCR.bit.PD2; break;
		case E_DD_TOP_GPIO_PD3: *control = IO_CHIPTOP.PUDCR.bit.PD3; break;
		case E_DD_TOP_GPIO_PD4: *control = IO_CHIPTOP.PUDCR.bit.PD4; break;
		case E_DD_TOP_GPIO_PD5: *control = IO_CHIPTOP.PUDCR.bit.PD5; break;
		case E_DD_TOP_GPIO_PD6: *control = IO_CHIPTOP.PUDCR.bit.PD6; break;
		case E_DD_TOP_GPIO_PE0: *control = IO_CHIPTOP.PUDCR.bit.PE0; break;
		case E_DD_TOP_GPIO_PE1: *control = IO_CHIPTOP.PUDCR.bit.PE1; break;
		case E_DD_TOP_GPIO_PF2: *control = IO_CHIPTOP.PUDCR.bit.PF2; break;
		case E_DD_TOP_GPIO_PF3: *control = IO_CHIPTOP.PUDCR.bit.PF3; break;
		case E_DD_TOP_GPIO_PF4: *control = IO_CHIPTOP.PUDCR.bit.PF4; break;
		case E_DD_TOP_GPIO_PG0: *control = IO_CHIPTOP.PUDCR.bit.PG0; break;
		case E_DD_TOP_GPIO_PG1: *control = IO_CHIPTOP.PUDCR.bit.PG1; break;
		case E_DD_TOP_GPIO_PG2: *control = IO_CHIPTOP.PUDCR.bit.PG2; break;
		case E_DD_TOP_GPIO_PG3: *control = IO_CHIPTOP.PUDCR.bit.PG3; break;
		case E_DD_TOP_GPIO_PG4: *control = IO_CHIPTOP.PUDCR.bit.PG4; break;
		case E_DD_TOP_GPIO_PG5: *control = IO_CHIPTOP.PUDCR.bit.PG5; break;
		case E_DD_TOP_GPIO_PG6: *control = IO_CHIPTOP.PUDCR.bit.PG6; break;
		case E_DD_TOP_GPIO_PG7: *control = IO_CHIPTOP.PUDCR.bit.PG7; break;
		case E_DD_TOP_GPIO_PH0: *control = IO_CHIPTOP.PUDCR.bit.PH0; break;
		case E_DD_TOP_GPIO_PH1: *control = IO_CHIPTOP.PUDCR.bit.PH1; break;
		case E_DD_TOP_GPIO_PH2: *control = IO_CHIPTOP.PUDCR.bit.PH2; break;
		case E_DD_TOP_GPIO_PH3: *control = IO_CHIPTOP.PUDCR.bit.PH3; break;
		case E_DD_TOP_GPIO_PW0: *control = IO_CHIPTOP.PUDCR.bit.PW0; break;
		case E_DD_TOP_GPIO_PW1: *control = IO_CHIPTOP.PUDCR.bit.PW1; break;
		case E_DD_TOP_GPIO_PW2: *control = IO_CHIPTOP.PUDCR.bit.PW2; break;
		case E_DD_TOP_GPIO_PW3: *control = IO_CHIPTOP.PUDCR.bit.PW3; break;
		case E_DD_TOP_GPIO_PW4: *control = IO_CHIPTOP.PUDCR.bit.PW4; break;
		case E_DD_TOP_GPIO_PW5: *control = IO_CHIPTOP.PUDCR.bit.PW5; break;
		case E_DD_TOP_GPIO_PW6: *control = IO_CHIPTOP.PUDCR.bit.PW6; break;
		case E_DD_TOP_GPIO_PW7: *control = IO_CHIPTOP.PUDCR.bit.PW7; break;
		case E_DD_TOP_GPIO_PJ0: *control = IO_CHIPTOP.PUDCR.bit.PJ0; break;
		case E_DD_TOP_GPIO_PJ1: *control = IO_CHIPTOP.PUDCR.bit.PJ1; break;
		case E_DD_TOP_GPIO_PJ2: *control = IO_CHIPTOP.PUDCR.bit.PJ2; break;
		case E_DD_TOP_GPIO_PJ3: *control = IO_CHIPTOP.PUDCR.bit.PJ3; break;
		case E_DD_TOP_GPIO_PJ4: *control = IO_CHIPTOP.PUDCR.bit.PJ4; break;
		case E_DD_TOP_GPIO_PJ5: *control = IO_CHIPTOP.PUDCR.bit.PJ5; break;
		case E_DD_TOP_GPIO_PJ6: *control = IO_CHIPTOP.PUDCR.bit.PJ6; break;
		case E_DD_TOP_GPIO_PJ7: *control = IO_CHIPTOP.PUDCR.bit.PJ7; break;
		case E_DD_TOP_GPIO_PK0: *control = IO_CHIPTOP.PUDCR.bit.PK0; break;
		case E_DD_TOP_GPIO_PK1: *control = IO_CHIPTOP.PUDCR.bit.PK1; break;
		case E_DD_TOP_GPIO_PK2: *control = IO_CHIPTOP.PUDCR.bit.PK2; break;
		case E_DD_TOP_GPIO_PK3: *control = IO_CHIPTOP.PUDCR.bit.PK3; break;
		case E_DD_TOP_GPIO_PK4: *control = IO_CHIPTOP.PUDCR.bit.PK4; break;
		case E_DD_TOP_GPIO_PK5: *control = IO_CHIPTOP.PUDCR.bit.PK5; break;
		case E_DD_TOP_GPIO_PK6: *control = IO_CHIPTOP.PUDCR.bit.PK6; break;
		case E_DD_TOP_GPIO_PK7: *control = IO_CHIPTOP.PUDCR.bit.PK7; break;
		case E_DD_TOP_GPIO_PL0: *control = IO_CHIPTOP.PUDCR.bit.PL0; break;
		case E_DD_TOP_GPIO_PL1: *control = IO_CHIPTOP.PUDCR.bit.PL1; break;
		case E_DD_TOP_GPIO_PL2: *control = IO_CHIPTOP.PUDCR.bit.PL2; break;
		case E_DD_TOP_GPIO_PL3: *control = IO_CHIPTOP.PUDCR.bit.PL3; break;
		case E_DD_TOP_GPIO_PL4: *control = IO_CHIPTOP.PUDCR.bit.PL4; break;
		case E_DD_TOP_GPIO_PL5: *control = IO_CHIPTOP.PUDCR.bit.PL5; break;
		case E_DD_TOP_GPIO_PM0: *control = IO_CHIPTOP.PUDCR.bit.PM0; break;
		case E_DD_TOP_GPIO_PM1: *control = IO_CHIPTOP.PUDCR.bit.PM1; break;
		case E_DD_TOP_GPIO_PM3: *control = IO_CHIPTOP.PUDCR.bit.PM3; break;
		case E_DD_TOP_GPIO_PM4: *control = IO_CHIPTOP.PUDCR.bit.PM4; break;
		case E_DD_TOP_GPIO_PN0: *control = IO_CHIPTOP.PUDCR.bit.PN0; break;
		case E_DD_TOP_GPIO_PN1: *control = IO_CHIPTOP.PUDCR.bit.PN1; break;
		case E_DD_TOP_GPIO_PN3: *control = IO_CHIPTOP.PUDCR.bit.PN3; break;
		case E_DD_TOP_GPIO_PN4: *control = IO_CHIPTOP.PUDCR.bit.PN4; break;
		case E_DD_TOP_GPIO_PY0: *control = IO_CHIPTOP.PUDCR.bit.PY0; break;
		case E_DD_TOP_GPIO_PY1: *control = IO_CHIPTOP.PUDCR.bit.PY1; break;
		case E_DD_TOP_GPIO_PY2: *control = IO_CHIPTOP.PUDCR.bit.PY2; break;
		case E_DD_TOP_GPIO_PY3: *control = IO_CHIPTOP.PUDCR.bit.PY3; break;
		case E_DD_TOP_GPIO_PY4: *control = IO_CHIPTOP.PUDCR.bit.PY4; break;
		case E_DD_TOP_GPIO_PY5: *control = IO_CHIPTOP.PUDCR.bit.PY5; break;
		case E_DD_TOP_GPIO_PY6: *control = IO_CHIPTOP.PUDCR.bit.PY6; break;
		case E_DD_TOP_GPIO_PY7: *control = IO_CHIPTOP.PUDCR.bit.PY7; break;
		case E_DD_TOP_GPIO_PP0: *control = IO_CHIPTOP.PUDCR.bit.PP0; break;
		case E_DD_TOP_GPIO_PP1: *control = IO_CHIPTOP.PUDCR.bit.PP1; break;
		case E_DD_TOP_GPIO_PP2: *control = IO_CHIPTOP.PUDCR.bit.PP2; break;
		case E_DD_TOP_GPIO_PP3: *control = IO_CHIPTOP.PUDCR.bit.PP3; break;
		default:
			Dd_Top_Stop_GPIO_CLK_WITH_CS();
			Ddim_Print(("Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl() error. port not exist.\n"));
			return D_DD_TOP_INPUT_PARAM_ERROR;
	}

	Dd_Top_Stop_GPIO_CLK_WITH_CS();

	return D_DDIM_OK;
}
