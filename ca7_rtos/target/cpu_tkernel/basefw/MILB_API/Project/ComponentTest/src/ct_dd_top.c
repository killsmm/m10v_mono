/**
 * @file		ct_dd_top.c
 * @brief		This is ct code for dd_top.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "chiptop.h"
#include "dd_top.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define D_DD_TOP_PC_DEBUG

// GPIO register definition
#define D_DD_TOP_GPIO_EPCR				(0)		/**< EPCR  : External Port Control Register */
#define D_DD_TOP_GPIO_DDR				(1)		/**< DDR   : Port Direction Register        */
#define D_DD_TOP_GPIO_PDR				(2)		/**< PDR   : Port Data Register             */
#define D_DD_TOP_GPIO_PUDER				(3)		/**< PUDER : Pull-Up/Down Enable Register   */
#define D_DD_TOP_GPIO_PUDCR				(4)		/**< PUDCR : Pull-Up/Down Control Register  */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static CHAR gDD_Top_Port_Name[E_DD_TOP_GPIO_PORT_MAX][4] = {"P60",
															"P61",
															"P62",
															"P63",
															"P64",
															"P65",
															"P66",
															"P67",
															"P70",
															"P71",
															"P72",
															"P73",
															"P74",
															"P75",
															"P76",
															"P77",
															"P80",
															"P81",
															"P82",
															"P83",
															"P84",
															"P85",
															"P86",
															"P87",
															"P90",
															"P91",
															"P92",
															"P93",
															"PA0",
															"PA1",
															"PA2",
															"PA3",
															"PA4",
															"PA5",
															"PA6",
															"PA7",
															"PB0",
															"PB1",
															"PB2",
															"PB3",
															"PB4",
															"PC0",
															"PC1",
															"PC2",
															"PC3",
															"PC4",
															"PC5",
															"PD0",
															"PD1",
															"PD2",
															"PD3",
															"PD4",
															"PD5",
															"PD6",
															"PE0",
															"PE1",
															"PE2",
															"PE3",
															"PE4",
															"PE5",
															"PF0",
															"PF1",
															"PF2",
															"PF3",
															"PF4",
															"PG0",
															"PG1",
															"PG2",
															"PG3",
															"PG4",
															"PG5",
															"PG6",
															"PG7",
															"PH0",
															"PH1",
															"PH2",
															"PH3",
															"PW0",
															"PW1",
															"PW2",
															"PW3",
															"PW4",
															"PW5",
															"PW6",
															"PW7",
															"PJ0",
															"PJ1",
															"PJ2",
															"PJ3",
															"PJ4",
															"PJ5",
															"PJ6",
															"PJ7",
															"PK0",
															"PK1",
															"PK2",
															"PK3",
															"PK4",
															"PK5",
															"PK6",
															"PK7",
															"PL0",
															"PL1",
															"PL2",
															"PL3",
															"PL4",
															"PL5",
															"PM0",
															"PM1",
															"PM2",
															"PM3",
															"PM4",
															"PN0",
															"PN1",
															"PN2",
															"PN3",
															"PN4",
															"PY0",
															"PY1",
															"PY2",
															"PY3",
															"PY4",
															"PY5",
															"PY6",
															"PY7",
															"PP0",
															"PP1",
															"PP2",
															"PP3"};

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

static void ct_get_ctrl_persel1(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PERSEL1.bit.I2C0CNT	=	2;
	IO_CHIPTOP.PERSEL1.bit.SDANC0	=	1;
	IO_CHIPTOP.PERSEL1.bit.SCLNC0	=	1;
	IO_CHIPTOP.PERSEL1.bit.I2C1CNT	=	2;
	IO_CHIPTOP.PERSEL1.bit.SDANC1	=	1;
	IO_CHIPTOP.PERSEL1.bit.SCLNC1	=	1;
	IO_CHIPTOP.PERSEL1.bit.I2C2CNT	=	2;
	IO_CHIPTOP.PERSEL1.bit.SDANC2	=	1;
	IO_CHIPTOP.PERSEL1.bit.SCLNC2	=	1;
	IO_CHIPTOP.PERSEL1.bit.DVWT2	=	1;
	IO_CHIPTOP.PERSEL1.bit.DHWT3	=	1;
	IO_CHIPTOP.PERSEL1.bit.AU0DO0C	=	1;
	IO_CHIPTOP.PERSEL1.bit.AU0DO1C	=	1;
	IO_CHIPTOP.PERSEL1.bit.AU1DOC	=	1;
	IO_CHIPTOP.PERSEL1.bit.AU0MCC	=	1;
	IO_CHIPTOP.PERSEL1.bit.AU1MCC	=	1;
	IO_CHIPTOP.PERSEL1.bit.RLSEL3	=	1;
	IO_CHIPTOP.PERSEL1.bit.RLSEL4	=	1;
	IO_CHIPTOP.PERSEL1.bit.RLSEL5	=	1;
	IO_CHIPTOP.PERSEL1.bit.INTVDSEL	=	1;
#endif

	Ddim_Print(("PERSEL1_I2C0CNT	= %d\n", Dd_Top_Get_PERSEL1_I2C0CNT()));
	Ddim_Print(("PERSEL1_SDANC0	    = %d\n", Dd_Top_Get_PERSEL1_SDANC0()));
	Ddim_Print(("PERSEL1_SCLNC0	    = %d\n", Dd_Top_Get_PERSEL1_SCLNC0()));
	Ddim_Print(("PERSEL1_I2C1CNT	= %d\n", Dd_Top_Get_PERSEL1_I2C1CNT()));
	Ddim_Print(("PERSEL1_SDANC1	    = %d\n", Dd_Top_Get_PERSEL1_SDANC1()));
	Ddim_Print(("PERSEL1_SCLNC1	    = %d\n", Dd_Top_Get_PERSEL1_SCLNC1()));
	Ddim_Print(("PERSEL1_I2C2CNT	= %d\n", Dd_Top_Get_PERSEL1_I2C2CNT()));
	Ddim_Print(("PERSEL1_SDANC2	    = %d\n", Dd_Top_Get_PERSEL1_SDANC2()));
	Ddim_Print(("PERSEL1_SCLNC2	    = %d\n", Dd_Top_Get_PERSEL1_SCLNC2()));
	Ddim_Print(("PERSEL1_DVWT2	    = %d\n", Dd_Top_Get_PERSEL1_DVWT2()));
	Ddim_Print(("PERSEL1_DHWT3	    = %d\n", Dd_Top_Get_PERSEL1_DHWT3()));
	Ddim_Print(("PERSEL1_AU0DO0C	= %d\n", Dd_Top_Get_PERSEL1_AU0DO0C()));
	Ddim_Print(("PERSEL1_AU0DO1C	= %d\n", Dd_Top_Get_PERSEL1_AU0DO1C()));
	Ddim_Print(("PERSEL1_AU1DOC	    = %d\n", Dd_Top_Get_PERSEL1_AU1DOC()));
	Ddim_Print(("PERSEL1_AU0MCC	    = %d\n", Dd_Top_Get_PERSEL1_AU0MCC()));
	Ddim_Print(("PERSEL1_AU1MCC	    = %d\n", Dd_Top_Get_PERSEL1_AU1MCC()));
	Ddim_Print(("PERSEL1_RLSEL3	    = %d\n", Dd_Top_Get_PERSEL1_RLSEL3()));
	Ddim_Print(("PERSEL1_RLSEL4	    = %d\n", Dd_Top_Get_PERSEL1_RLSEL4()));
	Ddim_Print(("PERSEL1_RLSEL5	    = %d\n", Dd_Top_Get_PERSEL1_RLSEL5()));
	Ddim_Print(("PERSEL1_INTVDSEL	= %d\n", Dd_Top_Get_PERSEL1_INTVDSEL()));
}

static void ct_get_ctrl_persel2(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PERSEL2.bit.UDCTRG	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC0AM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC0BM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC1AM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC1BM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC2AM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC2BM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC3AM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC3BM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC4AM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC4BM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC5AM	=	1;
	IO_CHIPTOP.PERSEL2.bit.UDC5BM	=	1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	IO_CHIPTOP.PERSEL2.bit.SPIMSEN0	=	1;
	IO_CHIPTOP.PERSEL2.bit.SPIMSEN1	=	1;
	IO_CHIPTOP.PERSEL2.bit.SPIMSEN2	=	1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#endif

	Ddim_Print(("PERSEL2_UDCTRG		= %d\n", Dd_Top_Get_PERSEL2_UDCTRG()));
	Ddim_Print(("PERSEL2_UDC0AM	    = %d\n", Dd_Top_Get_PERSEL2_UDC0AM()));
	Ddim_Print(("PERSEL2_UDC0BM	    = %d\n", Dd_Top_Get_PERSEL2_UDC0BM()));
	Ddim_Print(("PERSEL2_UDC1AM		= %d\n", Dd_Top_Get_PERSEL2_UDC1AM()));
	Ddim_Print(("PERSEL2_UDC1BM	    = %d\n", Dd_Top_Get_PERSEL2_UDC1BM()));
	Ddim_Print(("PERSEL2_UDC2AM	    = %d\n", Dd_Top_Get_PERSEL2_UDC2AM()));
	Ddim_Print(("PERSEL2_UDC2BM		= %d\n", Dd_Top_Get_PERSEL2_UDC2BM()));
	Ddim_Print(("PERSEL2_UDC3AM	    = %d\n", Dd_Top_Get_PERSEL2_UDC3AM()));
	Ddim_Print(("PERSEL2_UDC3BM	    = %d\n", Dd_Top_Get_PERSEL2_UDC3BM()));
	Ddim_Print(("PERSEL2_UDC4AM	    = %d\n", Dd_Top_Get_PERSEL2_UDC4AM()));
	Ddim_Print(("PERSEL2_UDC4BM	    = %d\n", Dd_Top_Get_PERSEL2_UDC4BM()));
	Ddim_Print(("PERSEL2_UDC5AM		= %d\n", Dd_Top_Get_PERSEL2_UDC5AM()));
	Ddim_Print(("PERSEL2_UDC5BM		= %d\n", Dd_Top_Get_PERSEL2_UDC5BM()));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("PERSEL2_SPIMSEN0		= %d\n", Dd_Top_Get_PERSEL2_SPIMSEN0()));
	Ddim_Print(("PERSEL2_SPIMSEN1		= %d\n", Dd_Top_Get_PERSEL2_SPIMSEN1()));
	Ddim_Print(("PERSEL2_SPIMSEN2		= %d\n", Dd_Top_Get_PERSEL2_SPIMSEN2()));
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
}

static void ct_get_ctrl_persel3(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PERSEL3.bit.AU1WPPS	   = 1;
	IO_CHIPTOP.PERSEL3.bit.AU0WP11 	   = 1;
	IO_CHIPTOP.PERSEL3.bit.SNH2WP04    = 1;
	IO_CHIPTOP.PERSEL3.bit.SNV2WP05    = 1;
	IO_CHIPTOP.PERSEL3.bit.SNA2WP06    = 1;
	IO_CHIPTOP.PERSEL3.bit.SNH3WP07    = 1;
	IO_CHIPTOP.PERSEL3.bit.SNV3WP08    = 1;
	IO_CHIPTOP.PERSEL3.bit.SNA3WP09    = 1;
	IO_CHIPTOP.PERSEL3.bit.U1VBSOU5    = 1;
	IO_CHIPTOP.PERSEL3.bit.U2VBSOU6    = 1;
	IO_CHIPTOP.PERSEL3.bit.U2IDWP14    = 1;
	IO_CHIPTOP.PERSEL3.bit.PRT1SOU3    = 1;
	IO_CHIPTOP.PERSEL3.bit.PXW0WP12    = 1;
	IO_CHIPTOP.PERSEL3.bit.PXW1SOU7    = 1;
	IO_CHIPTOP.PERSEL3.bit.PXC0SCK7    = 1;
	IO_CHIPTOP.PERSEL3.bit.PXC1WP15    = 1;
	IO_CHIPTOP.PERSEL3.bit.SCK4WP13    = 1;
	IO_CHIPTOP.PERSEL3.bit.S3LSOU5WP14 = 1;
	IO_CHIPTOP.PERSEL3.bit.S3SWP15     = 1;
	IO_CHIPTOP.PERSEL3.bit.RTS1SCK2    = 1;
	IO_CHIPTOP.PERSEL3.bit.U1IDWP13    = 1;
	IO_CHIPTOP.PERSEL3.bit.PRT0SCK3    = 1;
#endif

	Ddim_Print(("PERSEL3_AU1WPPS     = %d\n", Dd_Top_Get_PERSEL3_AU1WPPS()));
	Ddim_Print(("PERSEL3_AU0WP11     = %d\n", Dd_Top_Get_PERSEL3_AU0WP11()));
	Ddim_Print(("PERSEL3_SNH2WP04    = %d\n", Dd_Top_Get_PERSEL3_SNH2WP04()));
	Ddim_Print(("PERSEL3_SNV2WP05    = %d\n", Dd_Top_Get_PERSEL3_SNV2WP05()));
	Ddim_Print(("PERSEL3_SNA2WP06    = %d\n", Dd_Top_Get_PERSEL3_SNA2WP06()));
	Ddim_Print(("PERSEL3_SNH3WP07    = %d\n", Dd_Top_Get_PERSEL3_SNH3WP07()));
	Ddim_Print(("PERSEL3_SNV3WP08    = %d\n", Dd_Top_Get_PERSEL3_SNV3WP08()));
	Ddim_Print(("PERSEL3_SNA3WP09    = %d\n", Dd_Top_Get_PERSEL3_SNA3WP09()));
	Ddim_Print(("PERSEL3_U1VBSOU5    = %d\n", Dd_Top_Get_PERSEL3_U1VBSOU5()));
	Ddim_Print(("PERSEL3_U2VBSOU6    = %d\n", Dd_Top_Get_PERSEL3_U2VBSOU6()));
	Ddim_Print(("PERSEL3_U2IDWP14    = %d\n", Dd_Top_Get_PERSEL3_U2IDWP14()));
	Ddim_Print(("PERSEL3_PRT1SOU3    = %d\n", Dd_Top_Get_PERSEL3_PRT1SOU3()));
	Ddim_Print(("PERSEL3_PXW0WP12    = %d\n", Dd_Top_Get_PERSEL3_PXW0WP12()));
	Ddim_Print(("PERSEL3_PXW1SOU7    = %d\n", Dd_Top_Get_PERSEL3_PXW1SOU7()));
	Ddim_Print(("PERSEL3_PXC0SCK7    = %d\n", Dd_Top_Get_PERSEL3_PXC0SCK7()));
	Ddim_Print(("PERSEL3_PXC1WP15    = %d\n", Dd_Top_Get_PERSEL3_PXC1WP15()));
	Ddim_Print(("PERSEL3_SCK4WP13    = %d\n", Dd_Top_Get_PERSEL3_SCK4WP13()));
	Ddim_Print(("PERSEL3_S3LSOU5WP14 = %d\n", Dd_Top_Get_PERSEL3_S3LSOU5WP14()));
	Ddim_Print(("PERSEL3_S3SWP15     = %d\n", Dd_Top_Get_PERSEL3_S3SWP15()));
	Ddim_Print(("PERSEL3_RTS1SCK2    = %d\n", Dd_Top_Get_PERSEL3_RTS1SCK2()));
	Ddim_Print(("PERSEL3_U1IDWP13    = %d\n", Dd_Top_Get_PERSEL3_U1IDWP13()));
	Ddim_Print(("PERSEL3_PRT0SCK3    = %d\n", Dd_Top_Get_PERSEL3_PRT0SCK3()));
}

static void ct_get_ctrl_persel4(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PERSEL4.bit.EI22UD0A	   = 1;
	IO_CHIPTOP.PERSEL4.bit.EI23UD0B    = 1;
	IO_CHIPTOP.PERSEL4.bit.EI24UD1A    = 1;
	IO_CHIPTOP.PERSEL4.bit.EI25UD1B    = 1;
	IO_CHIPTOP.PERSEL4.bit.EI26UD2A    = 1;
	IO_CHIPTOP.PERSEL4.bit.EI27UD2B    = 1;
	IO_CHIPTOP.PERSEL4.bit.EI28UD3A    = 1;
	IO_CHIPTOP.PERSEL4.bit.EI29UD3B    = 1;
	IO_CHIPTOP.PERSEL4.bit.EI30UD4A    = 1;
	IO_CHIPTOP.PERSEL4.bit.EI31UD4B    = 1;
	IO_CHIPTOP.PERSEL4.bit.SD3CDXS     = 1;
	IO_CHIPTOP.PERSEL4.bit.SD3WPS      = 1;
	IO_CHIPTOP.PERSEL4.bit.U1OCDXS     = 1;
	IO_CHIPTOP.PERSEL4.bit.U1IDDIGS    = 1;
	IO_CHIPTOP.PERSEL4.bit.U2OCDXS     = 1;
	IO_CHIPTOP.PERSEL4.bit.U2IDDIGS    = 1;
#endif

	Ddim_Print(("PERSEL4_EI22UD0A    = %d\n", Dd_Top_Get_PERSEL4_EI22UD0A()));
	Ddim_Print(("PERSEL4_EI23UD0B    = %d\n", Dd_Top_Get_PERSEL4_EI23UD0B()));
	Ddim_Print(("PERSEL4_EI24UD1A    = %d\n", Dd_Top_Get_PERSEL4_EI24UD1A()));
	Ddim_Print(("PERSEL4_EI25UD1B    = %d\n", Dd_Top_Get_PERSEL4_EI25UD1B()));
	Ddim_Print(("PERSEL4_EI26UD2A    = %d\n", Dd_Top_Get_PERSEL4_EI26UD2A()));
	Ddim_Print(("PERSEL4_EI27UD2B    = %d\n", Dd_Top_Get_PERSEL4_EI27UD2B()));
	Ddim_Print(("PERSEL4_EI28UD3A    = %d\n", Dd_Top_Get_PERSEL4_EI28UD3A()));
	Ddim_Print(("PERSEL4_EI29UD3B    = %d\n", Dd_Top_Get_PERSEL4_EI29UD3B()));
	Ddim_Print(("PERSEL4_EI30UD4A    = %d\n", Dd_Top_Get_PERSEL4_EI30UD4A()));
	Ddim_Print(("PERSEL4_EI31UD4B    = %d\n", Dd_Top_Get_PERSEL4_EI31UD4B()));
	Ddim_Print(("PERSEL4_SD3CDXS     = %d\n", Dd_Top_Get_PERSEL4_SD3CDXS()));
	Ddim_Print(("PERSEL4_SD3WPS      = %d\n", Dd_Top_Get_PERSEL4_SD3WPS()));
	Ddim_Print(("PERSEL4_U1OCDXS     = %d\n", Dd_Top_Get_PERSEL4_U1OCDXS()));
	Ddim_Print(("PERSEL4_U1IDDIGS    = %d\n", Dd_Top_Get_PERSEL4_U1IDDIGS()));
	Ddim_Print(("PERSEL4_U2OCDXS     = %d\n", Dd_Top_Get_PERSEL4_U2OCDXS()));
	Ddim_Print(("PERSEL4_U2IDDIGS    = %d\n", Dd_Top_Get_PERSEL4_U2IDDIGS()));
}

static void ct_get_ctrl_msel(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.MSELC.bit.MSEL = 7;
#endif

	Ddim_Print(("MSELC_MSEL  = %d\n", Dd_Top_Get_MSELC_MSEL()));
}

static void ct_get_ctrl_dbcnt1(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.DBCNT1.bit.T07DC	= 1;
	IO_CHIPTOP.DBCNT1.bit.T08DC	= 1;
	IO_CHIPTOP.DBCNT1.bit.T09DC	= 1;
	IO_CHIPTOP.DBCNT1.bit.T10DC	= 1;
	IO_CHIPTOP.DBCNT1.bit.T11DC	= 1;
	IO_CHIPTOP.DBCNT1.bit.T12DC	= 1;
	IO_CHIPTOP.DBCNT1.bit.T13DC	= 1;
	IO_CHIPTOP.DBCNT1.bit.T14DC	= 1;
	IO_CHIPTOP.DBCNT1.bit.T15DC	= 1;
#endif

	Ddim_Print(("DBCNT1_T07DC	= %d\n", Dd_Top_Get_DBCNT1_T07DC()));
	Ddim_Print(("DBCNT1_T08DC	= %d\n", Dd_Top_Get_DBCNT1_T08DC()));
	Ddim_Print(("DBCNT1_T09DC	= %d\n", Dd_Top_Get_DBCNT1_T09DC()));
	Ddim_Print(("DBCNT1_T10DC	= %d\n", Dd_Top_Get_DBCNT1_T10DC()));
	Ddim_Print(("DBCNT1_T11DC	= %d\n", Dd_Top_Get_DBCNT1_T11DC()));
	Ddim_Print(("DBCNT1_T12DC	= %d\n", Dd_Top_Get_DBCNT1_T12DC()));
	Ddim_Print(("DBCNT1_T13DC	= %d\n", Dd_Top_Get_DBCNT1_T13DC()));
	Ddim_Print(("DBCNT1_T14DC	= %d\n", Dd_Top_Get_DBCNT1_T14DC()));
	Ddim_Print(("DBCNT1_T15DC	= %d\n", Dd_Top_Get_DBCNT1_T15DC()));
}

static void ct_get_ctrl_dbcnt2(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.DBCNT2.bit.T16DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T17DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T18DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T19DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T20DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T21DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T22DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T23DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T24DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T25DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T26DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T27DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T28DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T29DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T30DC	= 1;
	IO_CHIPTOP.DBCNT2.bit.T31DC	= 1;
#endif

	Ddim_Print(("DBCNT2_T16DC	= %d\n", Dd_Top_Get_DBCNT2_T16DC()));
	Ddim_Print(("DBCNT2_T17DC	= %d\n", Dd_Top_Get_DBCNT2_T17DC()));
	Ddim_Print(("DBCNT2_T18DC	= %d\n", Dd_Top_Get_DBCNT2_T18DC()));
	Ddim_Print(("DBCNT2_T19DC	= %d\n", Dd_Top_Get_DBCNT2_T19DC()));
	Ddim_Print(("DBCNT2_T20DC	= %d\n", Dd_Top_Get_DBCNT2_T20DC()));
	Ddim_Print(("DBCNT2_T21DC	= %d\n", Dd_Top_Get_DBCNT2_T21DC()));
	Ddim_Print(("DBCNT2_T22DC	= %d\n", Dd_Top_Get_DBCNT2_T22DC()));
	Ddim_Print(("DBCNT2_T23DC	= %d\n", Dd_Top_Get_DBCNT2_T23DC()));
	Ddim_Print(("DBCNT2_T24DC	= %d\n", Dd_Top_Get_DBCNT2_T24DC()));
	Ddim_Print(("DBCNT2_T25DC	= %d\n", Dd_Top_Get_DBCNT2_T25DC()));
	Ddim_Print(("DBCNT2_T26DC	= %d\n", Dd_Top_Get_DBCNT2_T26DC()));
	Ddim_Print(("DBCNT2_T27DC	= %d\n", Dd_Top_Get_DBCNT2_T27DC()));
	Ddim_Print(("DBCNT2_T28DC	= %d\n", Dd_Top_Get_DBCNT2_T28DC()));
	Ddim_Print(("DBCNT2_T29DC	= %d\n", Dd_Top_Get_DBCNT2_T29DC()));
	Ddim_Print(("DBCNT2_T30DC	= %d\n", Dd_Top_Get_DBCNT2_T30DC()));
	Ddim_Print(("DBCNT2_T31DC	= %d\n", Dd_Top_Get_DBCNT2_T31DC()));
}

static void ct_get_ctrl_dbcnt3(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.DBCNT3.bit.T32DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T33DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T34DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T35DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T36DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T37DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T38DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T39DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T40DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T41DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T42DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T43DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T44DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T45DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T46DC	= 1;
	IO_CHIPTOP.DBCNT3.bit.T47DC	= 1;
#endif

	Ddim_Print(("DBCNT3_T32DC	= %d\n", Dd_Top_Get_DBCNT3_T32DC()));
	Ddim_Print(("DBCNT3_T33DC	= %d\n", Dd_Top_Get_DBCNT3_T33DC()));
	Ddim_Print(("DBCNT3_T34DC	= %d\n", Dd_Top_Get_DBCNT3_T34DC()));
	Ddim_Print(("DBCNT3_T35DC	= %d\n", Dd_Top_Get_DBCNT3_T35DC()));
	Ddim_Print(("DBCNT3_T36DC	= %d\n", Dd_Top_Get_DBCNT3_T36DC()));
	Ddim_Print(("DBCNT3_T37DC	= %d\n", Dd_Top_Get_DBCNT3_T37DC()));
	Ddim_Print(("DBCNT3_T38DC	= %d\n", Dd_Top_Get_DBCNT3_T38DC()));
	Ddim_Print(("DBCNT3_T39DC	= %d\n", Dd_Top_Get_DBCNT3_T39DC()));
	Ddim_Print(("DBCNT3_T40DC	= %d\n", Dd_Top_Get_DBCNT3_T40DC()));
	Ddim_Print(("DBCNT3_T41DC	= %d\n", Dd_Top_Get_DBCNT3_T41DC()));
	Ddim_Print(("DBCNT3_T42DC	= %d\n", Dd_Top_Get_DBCNT3_T42DC()));
	Ddim_Print(("DBCNT3_T43DC	= %d\n", Dd_Top_Get_DBCNT3_T43DC()));
	Ddim_Print(("DBCNT3_T44DC	= %d\n", Dd_Top_Get_DBCNT3_T44DC()));
	Ddim_Print(("DBCNT3_T45DC	= %d\n", Dd_Top_Get_DBCNT3_T45DC()));
	Ddim_Print(("DBCNT3_T46DC	= %d\n", Dd_Top_Get_DBCNT3_T46DC()));
	Ddim_Print(("DBCNT3_T47DC	= %d\n", Dd_Top_Get_DBCNT3_T47DC()));
}

static void ct_get_ctrl_dbcnt4(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.DBCNT4.bit.T48DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T49DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T50DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T51DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T52DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T53DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T54DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T55DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T56DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T57DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T58DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T59DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T60DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T61DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T62DC	= 1;
	IO_CHIPTOP.DBCNT4.bit.T63DC	= 1;
#endif

	Ddim_Print(("DBCNT4_T48DC	= %d\n", Dd_Top_Get_DBCNT4_T48DC()));
	Ddim_Print(("DBCNT4_T49DC	= %d\n", Dd_Top_Get_DBCNT4_T49DC()));
	Ddim_Print(("DBCNT4_T50DC	= %d\n", Dd_Top_Get_DBCNT4_T50DC()));
	Ddim_Print(("DBCNT4_T51DC	= %d\n", Dd_Top_Get_DBCNT4_T51DC()));
	Ddim_Print(("DBCNT4_T52DC	= %d\n", Dd_Top_Get_DBCNT4_T52DC()));
	Ddim_Print(("DBCNT4_T53DC	= %d\n", Dd_Top_Get_DBCNT4_T53DC()));
	Ddim_Print(("DBCNT4_T54DC	= %d\n", Dd_Top_Get_DBCNT4_T54DC()));
	Ddim_Print(("DBCNT4_T55DC	= %d\n", Dd_Top_Get_DBCNT4_T55DC()));
	Ddim_Print(("DBCNT4_T56DC	= %d\n", Dd_Top_Get_DBCNT4_T56DC()));
	Ddim_Print(("DBCNT4_T57DC	= %d\n", Dd_Top_Get_DBCNT4_T57DC()));
	Ddim_Print(("DBCNT4_T58DC	= %d\n", Dd_Top_Get_DBCNT4_T58DC()));
	Ddim_Print(("DBCNT4_T59DC	= %d\n", Dd_Top_Get_DBCNT4_T59DC()));
	Ddim_Print(("DBCNT4_T60DC	= %d\n", Dd_Top_Get_DBCNT4_T60DC()));
	Ddim_Print(("DBCNT4_T61DC	= %d\n", Dd_Top_Get_DBCNT4_T61DC()));
	Ddim_Print(("DBCNT4_T62DC	= %d\n", Dd_Top_Get_DBCNT4_T62DC()));
	Ddim_Print(("DBCNT4_T63DC	= %d\n", Dd_Top_Get_DBCNT4_T63DC()));
}

static void ct_get_ctrl_dbcnt5(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.DBCNT5.bit.T64DC	= 1;
	IO_CHIPTOP.DBCNT5.bit.T65DC	= 1;
	IO_CHIPTOP.DBCNT5.bit.T66DC	= 1;
#endif

	Ddim_Print(("DBCNT5_T64DC  = %d\n", Dd_Top_Get_DBCNT5_T64DC()));
	Ddim_Print(("DBCNT5_T65DC  = %d\n", Dd_Top_Get_DBCNT5_T65DC()));
	Ddim_Print(("DBCNT5_T66DC  = %d\n", Dd_Top_Get_DBCNT5_T66DC()));
}

static void ct_get_ctrl_pudcnt(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PUDCNT.bit.EMMCNICS	 = 1;
	IO_CHIPTOP.PUDCNT.bit.EMDATPUE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.EMCMDPUE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.EMDSPDE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.NFDATPUE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.NFRBXPUE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.NFDQSPUE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.SP1MISOPE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.SP1MOSIPE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.SP1SCKPE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.SP1CS0PE	 = 1;
	IO_CHIPTOP.PUDCNT.bit.SP1MISOUDC = 1;
	IO_CHIPTOP.PUDCNT.bit.SP1MOSIUDC = 1;
	IO_CHIPTOP.PUDCNT.bit.SP1SCKUDC	 = 1;
	IO_CHIPTOP.PUDCNT.bit.SP1CS0UDC	 = 1;
#endif

	Ddim_Print(("PUDCNT_EMMCNICS   = %d\n", Dd_Top_Get_PUDCNT_EMMCNICS()));
	Ddim_Print(("PUDCNT_EMDATPUE   = %d\n", Dd_Top_Get_PUDCNT_EMDATPUE()));
	Ddim_Print(("PUDCNT_EMCMDPUE   = %d\n", Dd_Top_Get_PUDCNT_EMCMDPUE()));
	Ddim_Print(("PUDCNT_EMDSPDE    = %d\n", Dd_Top_Get_PUDCNT_EMDSPDE()));
	Ddim_Print(("PUDCNT_NFDATPUE   = %d\n", Dd_Top_Get_PUDCNT_NFDATPUE()));
	Ddim_Print(("PUDCNT_NFRBXPUE   = %d\n", Dd_Top_Get_PUDCNT_NFRBXPUE()));
	Ddim_Print(("PUDCNT_NFDQSPUE   = %d\n", Dd_Top_Get_PUDCNT_NFDQSPUE()));
	Ddim_Print(("PUDCNT_SP1MISOPE  = %d\n", Dd_Top_Get_PUDCNT_SP1MISOPE()));
	Ddim_Print(("PUDCNT_SP1MOSIPE  = %d\n", Dd_Top_Get_PUDCNT_SP1MOSIPE()));
	Ddim_Print(("PUDCNT_SP1SCKPE   = %d\n", Dd_Top_Get_PUDCNT_SP1SCKPE()));
	Ddim_Print(("PUDCNT_SP1CS0PE   = %d\n", Dd_Top_Get_PUDCNT_SP1CS0PE()));
	Ddim_Print(("PUDCNT_SP1MISOUDC = %d\n", Dd_Top_Get_PUDCNT_SP1MISOUDC()));
	Ddim_Print(("PUDCNT_SP1MOSIUDC = %d\n", Dd_Top_Get_PUDCNT_SP1MOSIUDC()));
	Ddim_Print(("PUDCNT_SP1SCKUDC  = %d\n", Dd_Top_Get_PUDCNT_SP1SCKUDC()));
	Ddim_Print(("PUDCNT_SP1CS0UDC  = %d\n", Dd_Top_Get_PUDCNT_SP1CS0UDC()));
}

static void ct_set_ctrl_persel1(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "I2C0CNT" ) ) {
			Dd_Top_Set_PERSEL1_I2C0CNT(val);
			Ddim_Print(("It set PERSEL1_I2C0CNT select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.I2C0CNT	=%d\n", IO_CHIPTOP.PERSEL1.bit.I2C0CNT));
		}
		else if ( 0 == strcmp( argv[4], "SDANC0" ) ) {
			Dd_Top_Set_PERSEL1_SDANC0(val);
			Ddim_Print(("It set PERSEL1_SDANC0 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.SDANC0	=%d\n", IO_CHIPTOP.PERSEL1.bit.SDANC0));
		}
		else if ( 0 == strcmp( argv[4], "SCLNC0" ) ) {
			Dd_Top_Set_PERSEL1_SCLNC0(val);
			Ddim_Print(("It set PERSEL1_SCLNC0 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.SCLNC0	=%d\n", IO_CHIPTOP.PERSEL1.bit.SCLNC0));
		}
		else if ( 0 == strcmp( argv[4], "I2C1CNT" ) ) {
			Dd_Top_Set_PERSEL1_I2C1CNT(val);
			Ddim_Print(("It set PERSEL1_I2C1CNT select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.I2C1CNT	=%d\n", IO_CHIPTOP.PERSEL1.bit.I2C1CNT));
		}
		else if ( 0 == strcmp( argv[4], "SDANC1" ) ) {
			Dd_Top_Set_PERSEL1_SDANC1(val);
			Ddim_Print(("It set PERSEL1_SDANC1 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.SDANC1	=%d\n", IO_CHIPTOP.PERSEL1.bit.SDANC1));
		}
		else if ( 0 == strcmp( argv[4], "SCLNC1" ) ) {
			Dd_Top_Set_PERSEL1_SCLNC1(val);
			Ddim_Print(("It set PERSEL1_SCLNC1 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.SCLNC1	=%d\n", IO_CHIPTOP.PERSEL1.bit.SCLNC1));
		}
		else if ( 0 == strcmp( argv[4], "I2C2CNT" ) ) {
			Dd_Top_Set_PERSEL1_I2C2CNT(val);
			Ddim_Print(("It set PERSEL1_I2C2CNT select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.I2C2CNT	=%d\n", IO_CHIPTOP.PERSEL1.bit.I2C2CNT));
		}
		else if ( 0 == strcmp( argv[4], "SDANC2" ) ) {
			Dd_Top_Set_PERSEL1_SDANC2(val);
			Ddim_Print(("It set PERSEL1_SDANC2 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.SDANC2	=%d\n", IO_CHIPTOP.PERSEL1.bit.SDANC2));
		}
		else if ( 0 == strcmp( argv[4], "SCLNC2" ) ) {
			Dd_Top_Set_PERSEL1_SCLNC2(val);
			Ddim_Print(("It set PERSEL1_SCLNC2 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.SCLNC2	=%d\n", IO_CHIPTOP.PERSEL1.bit.SCLNC2));
		}
		else if ( 0 == strcmp( argv[4], "DVWT2" ) ) {
			Dd_Top_Set_PERSEL1_DVWT2(val);
			Ddim_Print(("It set PERSEL1_DVWT2 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.DVWT2	=%d\n", IO_CHIPTOP.PERSEL1.bit.DVWT2));
		}
		else if ( 0 == strcmp( argv[4], "DHWT3" ) ) {
			Dd_Top_Set_PERSEL1_DHWT3(val);
			Ddim_Print(("It set PERSEL1_DHWT3 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.DHWT3	=%d\n", IO_CHIPTOP.PERSEL1.bit.DHWT3));
		}
		else if ( 0 == strcmp( argv[4], "AU0DO0C" ) ) {
			Dd_Top_Set_PERSEL1_AU0DO0C(val);
			Ddim_Print(("It set PERSEL1_AU0DO0C select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.AU0DO0C	=%d\n", IO_CHIPTOP.PERSEL1.bit.AU0DO0C));
		}
		else if ( 0 == strcmp( argv[4], "AU0DO1C" ) ) {
			Dd_Top_Set_PERSEL1_AU0DO1C(val);
			Ddim_Print(("It set PERSEL1_AU0DO1C select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.AU0DO1C	=%d\n", IO_CHIPTOP.PERSEL1.bit.AU0DO1C));
		}
		else if ( 0 == strcmp( argv[4], "AU1DOC" ) ) {
			Dd_Top_Set_PERSEL1_AU1DOC(val);
			Ddim_Print(("It set PERSEL1_AU1DOC select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.AU1DOC	=%d\n", IO_CHIPTOP.PERSEL1.bit.AU1DOC));
		}
		else if ( 0 == strcmp( argv[4], "AU0MCC" ) ) {
			Dd_Top_Set_PERSEL1_AU0MCC(val);	
			Ddim_Print(("It set PERSEL1_AU0MCC select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.AU0MCC	=%d\n", IO_CHIPTOP.PERSEL1.bit.AU0MCC));
		}
		else if ( 0 == strcmp( argv[4], "AU1MCC" ) ) {
			Dd_Top_Set_PERSEL1_AU1MCC(val);
			Ddim_Print(("It set PERSEL1_AU1MCC select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.AU1MCC	=%d\n", IO_CHIPTOP.PERSEL1.bit.AU1MCC));
		}
		else if ( 0 == strcmp( argv[4], "RLSEL3" ) ) {
			Dd_Top_Set_PERSEL1_RLSEL3(val);
			Ddim_Print(("It set PERSEL1_RLSEL3 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.RLSEL3	=%d\n", IO_CHIPTOP.PERSEL1.bit.RLSEL3));
		}
		else if ( 0 == strcmp( argv[4], "RLSEL4" ) ) {
			Dd_Top_Set_PERSEL1_RLSEL4(val);
			Ddim_Print(("It set PERSEL1_RLSEL4 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.RLSEL4	=%d\n", IO_CHIPTOP.PERSEL1.bit.RLSEL4));
		}
		else if ( 0 == strcmp( argv[4], "RLSEL5" ) ) {
			Dd_Top_Set_PERSEL1_RLSEL5(val);
			Ddim_Print(("It set PERSEL1_RLSEL5 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.RLSEL5	=%d\n", IO_CHIPTOP.PERSEL1.bit.RLSEL5));
		}
		else if ( 0 == strcmp( argv[4], "INTVDSEL" ) ) {
			Dd_Top_Set_PERSEL1_INTVDSEL(val);
			Ddim_Print(("It set PERSEL1_INTVDSEL select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL1.bit.INTVDSEL	=%d\n", IO_CHIPTOP.PERSEL1.bit.INTVDSEL));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_persel2(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "UDCTRG" ) ) {
			Dd_Top_Set_PERSEL2_UDCTRG(val);
			Ddim_Print(("It set PERSEL2_UDCTRG select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDCTRG	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDCTRG));
		}
		else if ( 0 == strcmp( argv[4], "UDC0AM" ) ) {
			Dd_Top_Set_PERSEL2_UDC0AM(val);
			Ddim_Print(("It set PERSEL2_UDC0AM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC0AM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC0AM));
		}
		else if ( 0 == strcmp( argv[4], "UDC0BM" ) ) {
			Dd_Top_Set_PERSEL2_UDC0BM(val);
			Ddim_Print(("It set PERSEL2_UDC0BM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC0BM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC0BM));
		}
		else if ( 0 == strcmp( argv[4], "UDC1AM" ) ) {
			Dd_Top_Set_PERSEL2_UDC1AM(val);
			Ddim_Print(("It set PERSEL2_UDC1AM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC1AM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC1AM));
		}
		else if ( 0 == strcmp( argv[4], "UDC1BM" ) ) {
			Dd_Top_Set_PERSEL2_UDC1BM(val);
			Ddim_Print(("It set PERSEL2_UDC1BM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC1BM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC1BM));
		}
		else if ( 0 == strcmp( argv[4], "UDC2AM" ) ) {
			Dd_Top_Set_PERSEL2_UDC2AM(val);
			Ddim_Print(("It set PERSEL2_UDC2AM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC2AM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC2AM));
		}
		else if ( 0 == strcmp( argv[4], "UDC2BM" ) ) {
			Dd_Top_Set_PERSEL2_UDC2BM(val);
			Ddim_Print(("It set PERSEL2_UDC2BM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC2BM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC2BM));
		}
		else if ( 0 == strcmp( argv[4], "UDC3AM" ) ) {
			Dd_Top_Set_PERSEL2_UDC3AM(val);
			Ddim_Print(("It set PERSEL2_UDC3AM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC3AM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC3AM));
		}
		else if ( 0 == strcmp( argv[4], "UDC3BM" ) ) {
			Dd_Top_Set_PERSEL2_UDC3BM(val);
			Ddim_Print(("It set PERSEL2_UDC3BM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC3BM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC3BM));
		}
		else if ( 0 == strcmp( argv[4], "UDC4AM" ) ) {
			Dd_Top_Set_PERSEL2_UDC4AM(val);
			Ddim_Print(("It set PERSEL2_UDC4AM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC4AM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC4AM));
		}
		else if ( 0 == strcmp( argv[4], "UDC4BM" ) ) {
			Dd_Top_Set_PERSEL2_UDC4BM(val);
			Ddim_Print(("It set PERSEL2_UDC4BM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC4BM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC4BM));
		}
		else if ( 0 == strcmp( argv[4], "UDC5AM" ) ) {
			Dd_Top_Set_PERSEL2_UDC5AM(val);
			Ddim_Print(("It set PERSEL2_UDC5AM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC5AM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC5AM));
		}
		else if ( 0 == strcmp( argv[4], "UDC5BM" ) ) {
			Dd_Top_Set_PERSEL2_UDC5BM(val);
			Ddim_Print(("It set PERSEL2_UDC5BM select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDC5BM	=%d\n", IO_CHIPTOP.PERSEL2.bit.UDC5BM));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "SPIMSEN0" ) ) {
			Dd_Top_Set_PERSEL2_SPIMSEN0(val);
			Ddim_Print(("It set PERSEL2_SPIMSEN0 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.SPIMSEN0	=%d\n", IO_CHIPTOP.PERSEL2.bit.SPIMSEN0));
		}
		else if ( 0 == strcmp( argv[4], "SPIMSEN1" ) ) {
			Dd_Top_Set_PERSEL2_SPIMSEN1(val);
			Ddim_Print(("It set PERSEL2_SPIMSEN1 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.SPIMSEN1	=%d\n", IO_CHIPTOP.PERSEL2.bit.SPIMSEN1));
		}
		else if ( 0 == strcmp( argv[4], "SPIMSEN2" ) ) {
			Dd_Top_Set_PERSEL2_SPIMSEN2(val);
			Ddim_Print(("It set PERSEL2_SPIMSEN2 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.SPIMSEN2	=%d\n", IO_CHIPTOP.PERSEL2.bit.SPIMSEN2));
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_persel3(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "AU1WPPS" ) ) {
			Dd_Top_Set_PERSEL3_AU1WPPS(val);
			Ddim_Print(("It set PERSEL3_AU1WPPS select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.AU1WPPS	=%d\n", IO_CHIPTOP.PERSEL3.bit.AU1WPPS));
		}
		else if ( 0 == strcmp( argv[4], "AU0WP11" ) ) {
			Dd_Top_Set_PERSEL3_AU0WP11(val);
			Ddim_Print(("It set PERSEL3_AU0WP11 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.AU0WP11	=%d\n", IO_CHIPTOP.PERSEL3.bit.AU0WP11));
		}
		else if ( 0 == strcmp( argv[4], "SNH2WP04" ) ) {
			Dd_Top_Set_PERSEL3_SNH2WP04(val);
			Ddim_Print(("It set PERSEL3_SNH2WP04 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.SNH2WP04	=%d\n", IO_CHIPTOP.PERSEL3.bit.SNH2WP04));
		}
		else if ( 0 == strcmp( argv[4], "SNV2WP05" ) ) {
			Dd_Top_Set_PERSEL3_SNV2WP05(val);
			Ddim_Print(("It set PERSEL3_SNV2WP05 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.SNV2WP05	=%d\n", IO_CHIPTOP.PERSEL3.bit.SNV2WP05));
		}
		else if ( 0 == strcmp( argv[4], "SNA2WP06" ) ) {
			Dd_Top_Set_PERSEL3_SNA2WP06(val);
			Ddim_Print(("It set PERSEL3_SNA2WP06 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.SNA2WP06	=%d\n", IO_CHIPTOP.PERSEL3.bit.SNA2WP06));
		}
		else if ( 0 == strcmp( argv[4], "SNH3WP07" ) ) {
			Dd_Top_Set_PERSEL3_SNH3WP07(val);
			Ddim_Print(("It set PERSEL3_SNH3WP07 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.SNH3WP07	=%d\n", IO_CHIPTOP.PERSEL3.bit.SNH3WP07));
		}
		else if ( 0 == strcmp( argv[4], "SNV3WP08" ) ) {
			Dd_Top_Set_PERSEL3_SNV3WP08(val);
			Ddim_Print(("It set PERSEL3_SNV3WP08 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.SNV3WP08	=%d\n", IO_CHIPTOP.PERSEL3.bit.SNV3WP08));
		}
		else if ( 0 == strcmp( argv[4], "SNA3WP09" ) ) {
			Dd_Top_Set_PERSEL3_SNA3WP09(val);
			Ddim_Print(("It set PERSEL3_SNA3WP09 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.SNA3WP09	=%d\n", IO_CHIPTOP.PERSEL3.bit.SNA3WP09));
		}
		else if ( 0 == strcmp( argv[4], "U1VBSOU5" ) ) {
			Dd_Top_Set_PERSEL3_U1VBSOU5(val);
			Ddim_Print(("It set PERSEL3_U1VBSOU5 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.U1VBSOU5	=%d\n", IO_CHIPTOP.PERSEL3.bit.U1VBSOU5));
		}
		else if ( 0 == strcmp( argv[4], "U2VBSOU6" ) ) {
			Dd_Top_Set_PERSEL3_U2VBSOU6(val);
			Ddim_Print(("It set PERSEL3_U2VBSOU6 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.U2VBSOU6	=%d\n", IO_CHIPTOP.PERSEL3.bit.U2VBSOU6));
		}
		else if ( 0 == strcmp( argv[4], "U2IDWP14" ) ) {
			Dd_Top_Set_PERSEL3_U2IDWP14(val);
			Ddim_Print(("It set PERSEL3_U2IDWP14 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.U2IDWP14	=%d\n", IO_CHIPTOP.PERSEL3.bit.U2IDWP14));
		}
		else if ( 0 == strcmp( argv[4], "PRT1SOU3" ) ) {
			Dd_Top_Set_PERSEL3_PRT1SOU3(val);
			Ddim_Print(("It set PERSEL3_PRT1SOU3 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.PRT1SOU3	=%d\n", IO_CHIPTOP.PERSEL3.bit.PRT1SOU3));
		}
		else if ( 0 == strcmp( argv[4], "PXW0WP12" ) ) {
			Dd_Top_Set_PERSEL3_PXW0WP12(val);
			Ddim_Print(("It set PERSEL3_PXW0WP12 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.PXW0WP12	=%d\n", IO_CHIPTOP.PERSEL3.bit.PXW0WP12));
		}
		else if ( 0 == strcmp( argv[4], "PXW1SOU7" ) ) {
			Dd_Top_Set_PERSEL3_PXW1SOU7(val);
			Ddim_Print(("It set PERSEL3_PXW1SOU7 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.PXW1SOU7	=%d\n", IO_CHIPTOP.PERSEL3.bit.PXW1SOU7));
		}
		else if ( 0 == strcmp( argv[4], "PXC0SCK7" ) ) {
			Dd_Top_Set_PERSEL3_PXC0SCK7(val);
			Ddim_Print(("It set PERSEL3_PXC0SCK7 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.PXC0SCK7	=%d\n", IO_CHIPTOP.PERSEL3.bit.PXC0SCK7));
		}
		else if ( 0 == strcmp( argv[4], "PXC1WP15" ) ) {
			Dd_Top_Set_PERSEL3_PXC1WP15(val);
			Ddim_Print(("It set PERSEL3_PXC1WP15 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.PXC1WP15	=%d\n", IO_CHIPTOP.PERSEL3.bit.PXC1WP15));
		}
		else if ( 0 == strcmp( argv[4], "SCK4WP13" ) ) {
			Dd_Top_Set_PERSEL3_PXC1WP15(val);
			Ddim_Print(("It set PERSEL3_SCK4WP13 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.SCK4WP13	=%d\n", IO_CHIPTOP.PERSEL3.bit.SCK4WP13));
		}
		else if ( 0 == strcmp( argv[4], "S3LSOU5WP14" ) ) {
			Dd_Top_Set_PERSEL3_S3LSOU5WP14(val);
			Ddim_Print(("It set PERSEL3_S3LSOU5WP14 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.S3LSOU5WP14	=%d\n", IO_CHIPTOP.PERSEL3.bit.S3LSOU5WP14));
		}
		else if ( 0 == strcmp( argv[4], "S3SWP15" ) ) {
			Dd_Top_Set_PERSEL3_S3SWP15(val);
			Ddim_Print(("It set PERSEL3_S3SWP15 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.S3SWP15	=%d\n", IO_CHIPTOP.PERSEL3.bit.S3SWP15));
		}
		else if ( 0 == strcmp( argv[4], "RTS1SCK2" ) ) {
			Dd_Top_Set_PERSEL3_RTS1SCK2(val);
			Ddim_Print(("It set PERSEL3_RTS1SCK2 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.RTS1SCK2	=%d\n", IO_CHIPTOP.PERSEL3.bit.RTS1SCK2));
		}
		else if ( 0 == strcmp( argv[4], "U1IDWP13" ) ) {
			Dd_Top_Set_PERSEL3_U1IDWP13(val);
			Ddim_Print(("It set PERSEL3_U1IDWP13 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.U1IDWP13	=%d\n", IO_CHIPTOP.PERSEL3.bit.U1IDWP13));
		}
		else if ( 0 == strcmp( argv[4], "PRT0SCK3" ) ) {
			Dd_Top_Set_PERSEL3_PRT0SCK3(val);
			Ddim_Print(("It set PERSEL3_PRT0SCK3 select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL3.bit.PRT0SCK3	=%d\n", IO_CHIPTOP.PERSEL3.bit.PRT0SCK3));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_persel4(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "EI22UD0A" ) ) {
			Dd_Top_Set_PERSEL4_EI22UD0A(val);
			Ddim_Print(("It set PERSEL4_EI22UD0A select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI22UD0A	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI22UD0A));
		}
		else if ( 0 == strcmp( argv[4], "EI23UD0B" ) ) {
			Dd_Top_Set_PERSEL4_EI23UD0B(val);
			Ddim_Print(("It set PERSEL4_EI23UD0B select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI23UD0B	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI23UD0B));
		}
		else if ( 0 == strcmp( argv[4], "EI24UD1A" ) ) {
			Dd_Top_Set_PERSEL4_EI24UD1A(val);
			Ddim_Print(("It set PERSEL4_EI24UD1A select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI24UD1A	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI24UD1A));
		}
		else if ( 0 == strcmp( argv[4], "EI25UD1B" ) ) {
			Dd_Top_Set_PERSEL4_EI25UD1B(val);
			Ddim_Print(("It set PERSEL4_EI25UD1B select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI25UD1B	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI25UD1B));
		}
		else if ( 0 == strcmp( argv[4], "EI26UD2A" ) ) {
			Dd_Top_Set_PERSEL4_EI26UD2A(val);
			Ddim_Print(("It set PERSEL4_EI26UD2A select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI26UD2A	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI26UD2A));
		}
		else if ( 0 == strcmp( argv[4], "EI27UD2B" ) ) {
			Dd_Top_Set_PERSEL4_EI27UD2B(val);
			Ddim_Print(("It set PERSEL4_EI27UD2B select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI27UD2B	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI27UD2B));
		}
		else if ( 0 == strcmp( argv[4], "EI28UD3A" ) ) {
			Dd_Top_Set_PERSEL4_EI28UD3A(val);
			Ddim_Print(("It set PERSEL4_EI28UD3A select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI28UD3A	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI28UD3A));
		}
		else if ( 0 == strcmp( argv[4], "EI29UD3B" ) ) {
			Dd_Top_Set_PERSEL4_EI29UD3B(val);
			Ddim_Print(("It set PERSEL4_EI29UD3B select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI29UD3B	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI29UD3B));
		}
		else if ( 0 == strcmp( argv[4], "EI30UD4A" ) ) {
			Dd_Top_Set_PERSEL4_EI30UD4A(val);
			Ddim_Print(("It set PERSEL4_EI30UD4A select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI30UD4A	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI30UD4A));
		}
		else if ( 0 == strcmp( argv[4], "EI31UD4B" ) ) {
			Dd_Top_Set_PERSEL4_EI31UD4B(val);
			Ddim_Print(("It set PERSEL4_EI31UD4B select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.EI31UD4B	=%d\n", IO_CHIPTOP.PERSEL4.bit.EI31UD4B));
		}
		else if ( 0 == strcmp( argv[4], "SD3CDXS" ) ) {
			Dd_Top_Set_PERSEL4_SD3CDXS(val);
			Ddim_Print(("It set PERSEL4_SD3CDXS select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.SD3CDXS	=%d\n", IO_CHIPTOP.PERSEL4.bit.SD3CDXS));
		}
		else if ( 0 == strcmp( argv[4], "SD3WPS" ) ) {
			Dd_Top_Set_PERSEL4_SD3WPS(val);
			Ddim_Print(("It set PERSEL4_SD3WPS select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.SD3WPS	=%d\n", IO_CHIPTOP.PERSEL4.bit.SD3WPS));
		}
		else if ( 0 == strcmp( argv[4], "U1OCDXS" ) ) {
			Dd_Top_Set_PERSEL4_U1OCDXS(val);
			Ddim_Print(("It set PERSEL4_U1OCDXS select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.U1OCDXS	=%d\n", IO_CHIPTOP.PERSEL4.bit.U1OCDXS));
		}
		else if ( 0 == strcmp( argv[4], "U1IDDIGS" ) ) {
			Dd_Top_Set_PERSEL4_U1IDDIGS(val);
			Ddim_Print(("It set PERSEL4_U1IDDIGS select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.U1IDDIGS =%d\n", IO_CHIPTOP.PERSEL4.bit.U1IDDIGS));
		}
		else if ( 0 == strcmp( argv[4], "U2OCDXS" ) ) {
			Dd_Top_Set_PERSEL4_U2OCDXS(val);
			Ddim_Print(("It set PERSEL4_U2OCDXS select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.U2OCDXS =%d\n", IO_CHIPTOP.PERSEL4.bit.U2OCDXS));
		}
		else if ( 0 == strcmp( argv[4], "U2IDDIGS" ) ) {
			Dd_Top_Set_PERSEL4_U2IDDIGS(val);
			Ddim_Print(("It set PERSEL4_U2IDDIGS select.\n"));
			Ddim_Print(("IO_CHIPTOP.PERSEL4.bit.U2IDDIGS =%d\n", IO_CHIPTOP.PERSEL4.bit.U2IDDIGS));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_msel(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "MSEL" ) ) {
			Dd_Top_Set_MSELC_MSEL(val);
			Ddim_Print(("It set MSEL_MSEL select.\n"));
			Ddim_Print(("IO_CHIPTOP.MSEL.bit.MSEL = %d\n", IO_CHIPTOP.MSELC.bit.MSEL));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_dbcnt1(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "T07DC" ) ) {
			Dd_Top_Set_DBCNT1_T07DC(val);
			Ddim_Print(("It set DBCNT1_T07DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T07DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T07DC));
		}
		else if ( 0 == strcmp( argv[4], "T08DC" ) ) {
			Dd_Top_Set_DBCNT1_T08DC(val);
			Ddim_Print(("It set DBCNT1_T08DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T08DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T08DC));
		}
		else if ( 0 == strcmp( argv[4], "T09DC" ) ) {
			Dd_Top_Set_DBCNT1_T09DC(val);
			Ddim_Print(("It set DBCNT1_T09DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T09DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T09DC));
		}
		else if ( 0 == strcmp( argv[4], "T10DC" ) ) {
			Dd_Top_Set_DBCNT1_T10DC(val);
			Ddim_Print(("It set DBCNT1_T10DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T10DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T10DC));
		}
		else if ( 0 == strcmp( argv[4], "T11DC" ) ) {
			Dd_Top_Set_DBCNT1_T11DC(val);
			Ddim_Print(("It set DBCNT1_T11DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T11DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T11DC));
		}
		else if ( 0 == strcmp( argv[4], "T12DC" ) ) {
			Dd_Top_Set_DBCNT1_T12DC(val);
			Ddim_Print(("It set DBCNT1_T12DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T12DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T12DC));
		}
		else if ( 0 == strcmp( argv[4], "T13DC" ) ) {
			Dd_Top_Set_DBCNT1_T13DC(val);
			Ddim_Print(("It set DBCNT1_T13DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T13DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T13DC));
		}
		else if ( 0 == strcmp( argv[4], "T14DC" ) ) {
			Dd_Top_Set_DBCNT1_T14DC(val);
			Ddim_Print(("It set DBCNT1_T14DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T14DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T14DC));
		}
		else if ( 0 == strcmp( argv[4], "T15DC" ) ) {
			Dd_Top_Set_DBCNT1_T15DC(val);
			Ddim_Print(("It set DBCNT1_T15DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT1.bit.T15DC = %d\n", IO_CHIPTOP.DBCNT1.bit.T15DC));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_dbcnt2(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "T16DC" ) ) {
			Dd_Top_Set_DBCNT2_T16DC(val);
			Ddim_Print(("It set DBCNT2_T16DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T16DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T16DC));
		}
		else if ( 0 == strcmp( argv[4], "T17DC" ) ) {
			Dd_Top_Set_DBCNT2_T17DC(val);
			Ddim_Print(("It set DBCNT2_T17DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T17DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T17DC));
		}
		else if ( 0 == strcmp( argv[4], "T18DC" ) ) {
			Dd_Top_Set_DBCNT2_T18DC(val);
			Ddim_Print(("It set DBCNT2_T18DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T18DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T18DC));
		}
		else if ( 0 == strcmp( argv[4], "T19DC" ) ) {
			Dd_Top_Set_DBCNT2_T19DC(val);
			Ddim_Print(("It set DBCNT2_T19DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T19DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T19DC));
		}
		else if ( 0 == strcmp( argv[4], "T20DC" ) ) {
			Dd_Top_Set_DBCNT2_T20DC(val);
			Ddim_Print(("It set DBCNT2_T20DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T20DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T20DC));
		}
		else if ( 0 == strcmp( argv[4], "T21DC" ) ) {
			Dd_Top_Set_DBCNT2_T21DC(val);
			Ddim_Print(("It set DBCNT2_T21DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T21DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T21DC));
		}
		else if ( 0 == strcmp( argv[4], "T22DC" ) ) {
			Dd_Top_Set_DBCNT2_T22DC(val);
			Ddim_Print(("It set DBCNT2_T22DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T22DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T22DC));
		}
		else if ( 0 == strcmp( argv[4], "T23DC" ) ) {
			Dd_Top_Set_DBCNT2_T23DC(val);
			Ddim_Print(("It set DBCNT2_T23DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T23DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T23DC));
		}
		else if ( 0 == strcmp( argv[4], "T24DC" ) ) {
			Dd_Top_Set_DBCNT2_T24DC(val);
			Ddim_Print(("It set DBCNT2_T24DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T24DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T24DC));
		}
		else if ( 0 == strcmp( argv[4], "T25DC" ) ) {
			Dd_Top_Set_DBCNT2_T25DC(val);
			Ddim_Print(("It set DBCNT2_T25DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T25DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T25DC));
		}
		else if ( 0 == strcmp( argv[4], "T26DC" ) ) {
			Dd_Top_Set_DBCNT2_T26DC(val);
			Ddim_Print(("It set DBCNT2_T26DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T26DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T26DC));
		}
		else if ( 0 == strcmp( argv[4], "T27DC" ) ) {
			Dd_Top_Set_DBCNT2_T27DC(val);
			Ddim_Print(("It set DBCNT2_T27DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T27DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T27DC));
		}
		else if ( 0 == strcmp( argv[4], "T28DC" ) ) {
			Dd_Top_Set_DBCNT2_T28DC(val);
			Ddim_Print(("It set DBCNT2_T28DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T28DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T28DC));
		}
		else if ( 0 == strcmp( argv[4], "T29DC" ) ) {
			Dd_Top_Set_DBCNT2_T29DC(val);
			Ddim_Print(("It set DBCNT2_T29DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T29DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T29DC));
		}
		else if ( 0 == strcmp( argv[4], "T30DC" ) ) {
			Dd_Top_Set_DBCNT2_T30DC(val);
			Ddim_Print(("It set DBCNT2_T30DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T30DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T30DC));
		}
		else if ( 0 == strcmp( argv[4], "T31DC" ) ) {
			Dd_Top_Set_DBCNT2_T31DC(val);
			Ddim_Print(("It set DBCNT2_T31DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT2.bit.T31DC = %d\n", IO_CHIPTOP.DBCNT2.bit.T31DC));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_dbcnt3(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "T32DC" ) ) {
			Dd_Top_Set_DBCNT3_T32DC(val);
			Ddim_Print(("It set DBCNT3_T32DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T32DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T32DC));
		}
		else if ( 0 == strcmp( argv[4], "T33DC" ) ) {
			Dd_Top_Set_DBCNT3_T33DC(val);
			Ddim_Print(("It set DBCNT3_T33DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T33DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T33DC));
		}
		else if ( 0 == strcmp( argv[4], "T34DC" ) ) {
			Dd_Top_Set_DBCNT3_T34DC(val);
			Ddim_Print(("It set DBCNT3_T34DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T34DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T34DC));
		}
		else if ( 0 == strcmp( argv[4], "T35DC" ) ) {
			Dd_Top_Set_DBCNT3_T35DC(val);
			Ddim_Print(("It set DBCNT3_T35DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T35DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T35DC));
		}
		else if ( 0 == strcmp( argv[4], "T36DC" ) ) {
			Dd_Top_Set_DBCNT3_T36DC(val);
			Ddim_Print(("It set DBCNT3_T36DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T36DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T36DC));
		}
		else if ( 0 == strcmp( argv[4], "T37DC" ) ) {
			Dd_Top_Set_DBCNT3_T37DC(val);
			Ddim_Print(("It set DBCNT3_T37DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T37DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T37DC));
		}
		else if ( 0 == strcmp( argv[4], "T38DC" ) ) {
			Dd_Top_Set_DBCNT3_T38DC(val);
			Ddim_Print(("It set DBCNT3_T38DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T38DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T38DC));
		}
		else if ( 0 == strcmp( argv[4], "T39DC" ) ) {
			Dd_Top_Set_DBCNT3_T39DC(val);
			Ddim_Print(("It set DBCNT3_T39DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T39DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T39DC));
		}
		else if ( 0 == strcmp( argv[4], "T40DC" ) ) {
			Dd_Top_Set_DBCNT3_T40DC(val);
			Ddim_Print(("It set DBCNT3_T40DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T40DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T40DC));
		}
		else if ( 0 == strcmp( argv[4], "T41DC" ) ) {
			Dd_Top_Set_DBCNT3_T41DC(val);
			Ddim_Print(("It set DBCNT3_T41DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T41DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T41DC));
		}
		else if ( 0 == strcmp( argv[4], "T42DC" ) ) {
			Dd_Top_Set_DBCNT3_T42DC(val);
			Ddim_Print(("It set DBCNT3_T42DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T42DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T42DC));
		}
		else if ( 0 == strcmp( argv[4], "T43DC" ) ) {
			Dd_Top_Set_DBCNT3_T43DC(val);
			Ddim_Print(("It set DBCNT3_T43DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T43DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T43DC));
		}
		else if ( 0 == strcmp( argv[4], "T44DC" ) ) {
			Dd_Top_Set_DBCNT3_T44DC(val);
			Ddim_Print(("It set DBCNT3_T44DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T44DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T44DC));
		}
		else if ( 0 == strcmp( argv[4], "T45DC" ) ) {
			Dd_Top_Set_DBCNT3_T45DC(val);
			Ddim_Print(("It set DBCNT3_T45DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T45DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T45DC));
		}
		else if ( 0 == strcmp( argv[4], "T46DC" ) ) {
			Dd_Top_Set_DBCNT3_T46DC(val);
			Ddim_Print(("It set DBCNT3_T46DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T46DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T46DC));
		}
		else if ( 0 == strcmp( argv[4], "T47DC" ) ) {
			Dd_Top_Set_DBCNT3_T47DC(val);
			Ddim_Print(("It set DBCNT3_T47DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.IO_CHIPTOP.DBCNT3.bit.T47DC = %d\n", IO_CHIPTOP.DBCNT3.bit.T47DC));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_dbcnt4(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "T48DC" ) ) {
			Dd_Top_Set_DBCNT4_T48DC(val);
			Ddim_Print(("It set DBCNT4_T48DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T48DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T48DC));
		}
		else if ( 0 == strcmp( argv[4], "T49DC" ) ) {
			Dd_Top_Set_DBCNT4_T49DC(val);
			Ddim_Print(("It set DBCNT4_T49DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T49DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T49DC));
		}
		else if ( 0 == strcmp( argv[4], "T50DC" ) ) {
			Dd_Top_Set_DBCNT4_T50DC(val);
			Ddim_Print(("It set DBCNT4_T50DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T50DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T50DC));
		}
		else if ( 0 == strcmp( argv[4], "T51DC" ) ) {
			Dd_Top_Set_DBCNT4_T51DC(val);
			Ddim_Print(("It set DBCNT4_T51DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T51DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T51DC));
		}
		else if ( 0 == strcmp( argv[4], "T52DC" ) ) {
			Dd_Top_Set_DBCNT4_T52DC(val);
			Ddim_Print(("It set DBCNT4_T52DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T52DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T52DC));
		}
		else if ( 0 == strcmp( argv[4], "T53DC" ) ) {
			Dd_Top_Set_DBCNT4_T53DC(val);
			Ddim_Print(("It set DBCNT4_T53DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T53DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T53DC));
		}
		else if ( 0 == strcmp( argv[4], "T54DC" ) ) {
			Dd_Top_Set_DBCNT4_T54DC(val);
			Ddim_Print(("It set DBCNT4_T54DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T54DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T54DC));
		}
		else if ( 0 == strcmp( argv[4], "T55DC" ) ) {
			Dd_Top_Set_DBCNT4_T55DC(val);
			Ddim_Print(("It set DBCNT4_T55DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T55DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T55DC));
		}
		else if ( 0 == strcmp( argv[4], "T56DC" ) ) {
			Dd_Top_Set_DBCNT4_T56DC(val);
			Ddim_Print(("It set DBCNT4_T56DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T56DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T56DC));
		}
		else if ( 0 == strcmp( argv[4], "T57DC" ) ) {
			Dd_Top_Set_DBCNT4_T57DC(val);
			Ddim_Print(("It set DBCNT4_T57DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T57DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T57DC));
		}
		else if ( 0 == strcmp( argv[4], "T58DC" ) ) {
			Dd_Top_Set_DBCNT4_T58DC(val);
			Ddim_Print(("It set DBCNT4_T58DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T58DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T58DC));
		}
		else if ( 0 == strcmp( argv[4], "T59DC" ) ) {
			Dd_Top_Set_DBCNT4_T59DC(val);
			Ddim_Print(("It set DBCNT4_T59DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T59DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T59DC));
		}
		else if ( 0 == strcmp( argv[4], "T60DC" ) ) {
			Dd_Top_Set_DBCNT4_T60DC(val);
			Ddim_Print(("It set DBCNT4_T60DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T60DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T60DC));
		}
		else if ( 0 == strcmp( argv[4], "T61DC" ) ) {
			Dd_Top_Set_DBCNT4_T61DC(val);
			Ddim_Print(("It set DBCNT4_T61DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T61DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T61DC));
		}
		else if ( 0 == strcmp( argv[4], "T62DC" ) ) {
			Dd_Top_Set_DBCNT4_T62DC(val);
			Ddim_Print(("It set DBCNT4_T62DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T62DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T62DC));
		}
		else if ( 0 == strcmp( argv[4], "T63DC" ) ) {
			Dd_Top_Set_DBCNT4_T63DC(val);
			Ddim_Print(("It set DBCNT4_T63DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT4.bit.T63DC = %d\n", IO_CHIPTOP.DBCNT4.bit.T63DC));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_dbcnt5(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "T64DC" ) ) {
			Dd_Top_Set_DBCNT5_T64DC(val);
			Ddim_Print(("It set DBCNT5_T64DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT5.bit.T64DC = %d\n", IO_CHIPTOP.DBCNT5.bit.T64DC));
		}
		else if ( 0 == strcmp( argv[4], "T65DC" ) ) {
			Dd_Top_Set_DBCNT5_T65DC(val);
			Ddim_Print(("It set DBCNT5_T65DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT5.bit.T65DC = %d\n", IO_CHIPTOP.DBCNT5.bit.T65DC));
		}
		else if ( 0 == strcmp( argv[4], "T66DC" ) ) {
			Dd_Top_Set_DBCNT5_T66DC(val);
			Ddim_Print(("It set DBCNT5_T66DC select.\n"));
			Ddim_Print(("IO_CHIPTOP.DBCNT5.bit.T66DC = %d\n", IO_CHIPTOP.DBCNT5.bit.T66DC));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_ctrl_pudcnt(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "EMMCNICS" ) ) {
			Dd_Top_Set_PUDCNT_EMMCNICS(val);
			Ddim_Print(("It set PUDCNT_EMMCNICS select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.EMMCNICS = %d\n", IO_CHIPTOP.PUDCNT.bit.EMMCNICS));
		}
		else if ( 0 == strcmp( argv[4], "EMDATPUE" ) ) {
			Dd_Top_Set_PUDCNT_EMDATPUE(val);
			Ddim_Print(("It set PUDCNT_EMDATPUE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.EMDATPUE = %d\n", IO_CHIPTOP.PUDCNT.bit.EMDATPUE));
		}
		else if ( 0 == strcmp( argv[4], "EMCMDPUE" ) ) {
			Dd_Top_Set_PUDCNT_EMCMDPUE(val);
			Ddim_Print(("It set PUDCNT_EMCMDPUE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.EMCMDPUE = %d\n", IO_CHIPTOP.PUDCNT.bit.EMCMDPUE));
		}
		else if ( 0 == strcmp( argv[4], "EMDSPDE" ) ) {
			Dd_Top_Set_PUDCNT_EMDSPDE(val);
			Ddim_Print(("It set PUDCNT_EMDSPDE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.EMDSPDE = %d\n", IO_CHIPTOP.PUDCNT.bit.EMDSPDE));
		}
		else if ( 0 == strcmp( argv[4], "NFDATPUE" ) ) {
			Dd_Top_Set_PUDCNT_NFDATPUE(val);
			Ddim_Print(("It set PUDCNT_NFDATPUE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.NFDATPUE = %d\n", IO_CHIPTOP.PUDCNT.bit.NFDATPUE));
		}
		else if ( 0 == strcmp( argv[4], "NFRBXPUE" ) ) {
			Dd_Top_Set_PUDCNT_NFRBXPUE(val);
			Ddim_Print(("It set PUDCNT_NFRBXPUE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.NFRBXPUE = %d\n", IO_CHIPTOP.PUDCNT.bit.NFRBXPUE));
		}
		else if ( 0 == strcmp( argv[4], "NFDQSPUE" ) ) {
			Dd_Top_Set_PUDCNT_NFDQSPUE(val);
			Ddim_Print(("It set PUDCNT_NFDQSPUE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.NFDQSPUE = %d\n", IO_CHIPTOP.PUDCNT.bit.NFDQSPUE));
		}
		else if ( 0 == strcmp( argv[4], "SP1MISOPE" ) ) {
			Dd_Top_Set_PUDCNT_SP1MISOPE(val);
			Ddim_Print(("It set PUDCNT_SP1MISOPE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.SP1MISOPE = %d\n", IO_CHIPTOP.PUDCNT.bit.SP1MISOPE));
		}
		else if ( 0 == strcmp( argv[4], "SP1MOSIPE" ) ) {
			Dd_Top_Set_PUDCNT_SP1MOSIPE(val);
			Ddim_Print(("It set PUDCNT_SP1MOSIPE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.SP1MOSIPE = %d\n", IO_CHIPTOP.PUDCNT.bit.SP1MOSIPE));
		}
		else if ( 0 == strcmp( argv[4], "SP1SCKPE" ) ) {
			Dd_Top_Set_PUDCNT_SP1SCKPE(val);
			Ddim_Print(("It set PUDCNT_SP1SCKPE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.SP1SCKPE = %d\n", IO_CHIPTOP.PUDCNT.bit.SP1SCKPE));
		}
		else if ( 0 == strcmp( argv[4], "SP1CS0PE" ) ) {
			Dd_Top_Set_PUDCNT_SP1CS0PE(val);
			Ddim_Print(("It set PUDCNT_SP1CS0PE select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.SP1CS0PE = %d\n", IO_CHIPTOP.PUDCNT.bit.SP1CS0PE));
		}
		else if ( 0 == strcmp( argv[4], "SP1MISOUDC" ) ) {
			Dd_Top_Set_PUDCNT_SP1MISOUDC(val);
			Ddim_Print(("It set PUDCNT_SP1MISOUDC select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.SP1MISOUDC = %d\n", IO_CHIPTOP.PUDCNT.bit.SP1MISOUDC));
		}
		else if ( 0 == strcmp( argv[4], "SP1MOSIUDC" ) ) {
			Dd_Top_Set_PUDCNT_SP1MOSIUDC(val);
			Ddim_Print(("It set PUDCNT_SP1MOSIUDC select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.SP1MOSIUDC = %d\n", IO_CHIPTOP.PUDCNT.bit.SP1MOSIUDC));
		}
		else if ( 0 == strcmp( argv[4], "SP1SCKUDC" ) ) {
			Dd_Top_Set_PUDCNT_SP1SCKUDC(val);
			Ddim_Print(("It set PUDCNT_SP1SCKUDC select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.SP1SCKUDC = %d\n", IO_CHIPTOP.PUDCNT.bit.SP1SCKUDC));
		}
		else if ( 0 == strcmp( argv[4], "SP1CS0UDC" ) ) {
			Dd_Top_Set_PUDCNT_SP1CS0UDC(val);
			Ddim_Print(("It set PUDCNT_SP1CS0UDC select.\n"));
			Ddim_Print(("IO_CHIPTOP.PUDCNT.bit.SP1CS0UDC = %d\n", IO_CHIPTOP.PUDCNT.bit.SP1CS0UDC));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}


static void ct_get_reset_ctrl(int argc, char** argv)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CRRSM.bit.__WDRST		= 1;
	IO_CHIPTOP.CRRSM.bit.__SWRST		= 1;
	IO_CHIPTOP.CRRSM.bit.__SRST			= 1;
	IO_CHIPTOP.CRRSM.bit.__PORST		= 1;
#endif
	Ddim_Print(("### CRRSM Register ###\n"));
	Ddim_Print(("IO_CHIPTOP.CRRSM.bit.__WDRST		= %d, CRRSM_WDRST			= %d\n", IO_CHIPTOP.CRRSM.bit.__WDRST, Dd_Top_Get_CRRSM_WDRST()));
	Ddim_Print(("IO_CHIPTOP.CRRSM.bit.__SWRST		= %d, CRRSM_SWRST			= %d\n", IO_CHIPTOP.CRRSM.bit.__SWRST, Dd_Top_Get_CRRSM_SWRST()));
	Ddim_Print(("IO_CHIPTOP.CRRSM.bit.__SRST		= %d, CRRSM_SRST			= %d\n", IO_CHIPTOP.CRRSM.bit.__SRST, Dd_Top_Get_CRRSM_SRST()));
	Ddim_Print(("IO_CHIPTOP.CRRSM.bit.__PORST		= %d, CRRSM_PORST			= %d\n", IO_CHIPTOP.CRRSM.bit.__PORST, Dd_Top_Get_CRRSM_PORST()));
	Ddim_Print(("\n"));
	Ddim_Print(("### CRRRS Register ###\n"));
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.RIBSR			= %d, CRRRS_RIBSR			= %d\n", IO_CHIPTOP.CRRRS.bit.RIBSR, Dd_Top_Get_CRRRS_RIBSR()));
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR	= %d, CRRRS_HEIPPASYNCSR	= %d\n", IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR, Dd_Top_Get_CRRRS_HEIPPASYNCSR()));
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR	= %d, CRRRS_HEIPPSYNCSR		= %d\n", IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR, Dd_Top_Get_CRRRS_HEIPPSYNCSR()));
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEVDFSR		= %d, CRRRS_HEVDFSR			= %d\n", IO_CHIPTOP.CRRRS.bit.HEVDFSR, Dd_Top_Get_CRRRS_HEVDFSR()));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEPXFSR		= %d, CRRRS_HEPXFSR			= %d\n", IO_CHIPTOP.CRRRS.bit.HEPXFSR, Dd_Top_Get_CRRRS_HEPXFSR()));
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEVENCSR		= %d, CRRRS_HEVENCSR		= %d\n", IO_CHIPTOP.CRRRS.bit.HEVENCSR, Dd_Top_Get_CRRRS_HEVENCSR()));
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.DSPSR			= %d, CRRRS_DSPSR			= %d\n", IO_CHIPTOP.CRRRS.bit.DSPSR, Dd_Top_Get_CRRRS_DSPSR()));
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU0_SR		= %d, CRRRS_CPU0_SR			= %d\n", IO_CHIPTOP.CRRRS.bit.CPU0_SR, Dd_Top_Get_CRRRS_CPU0_SR()));
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU1_SR		= %d, CRRRS_CPU1_SR			= %d\n", IO_CHIPTOP.CRRRS.bit.CPU1_SR, Dd_Top_Get_CRRRS_CPU1_SR()));
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU2_SR		= %d, CRRRS_CPU2_SR			= %d\n", IO_CHIPTOP.CRRRS.bit.CPU2_SR, Dd_Top_Get_CRRRS_CPU2_SR()));
	Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU3_SR		= %d, CRRRS_CPU3_SR			= %d\n", IO_CHIPTOP.CRRRS.bit.CPU3_SR, Dd_Top_Get_CRRRS_CPU3_SR()));
}

static void ct_set_reset_ctrl(int argc, char** argv)
{
	if (argc == 4) {
		if ( 0 == strcmp( argv[3], "CRRSM_WDRST" ) ) {
			Dd_Top_Clear_CRRSM_WDRST();
			Ddim_Print(("It set CRRSM_WDRST select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRSM.bit.__WDRST=%d\n", IO_CHIPTOP.CRRSM.bit.__WDRST));
		}
		else if ( 0 == strcmp( argv[3], "CRRSM_SWRST" ) ) {
			Dd_Top_Clear_CRRSM_SWRST();
			Ddim_Print(("It set CRRSM_SWRST select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRSM.bit.__SWRST=%d\n", IO_CHIPTOP.CRRSM.bit.__SWRST));
		}
		else if ( 0 == strcmp( argv[3], "CRRSM_SRST" ) ) {
			Dd_Top_Clear_CRRSM_SRST();
			Ddim_Print(("It set CRRSM_SRST select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRSM.bit.__SRST=%d\n", IO_CHIPTOP.CRRSM.bit.__SRST));
		}
		else if ( 0 == strcmp( argv[3], "CRRSM_PORST" ) ) {
			Dd_Top_Clear_CRRSM_PORST();
			Ddim_Print(("It set CRRSM_PORST select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRSM.bit.__PORST=%d\n", IO_CHIPTOP.CRRSM.bit.__PORST));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_CPU0_SR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_CPU0_SR();
			Ddim_Print(("It set CRRRS_CPU0_SR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU0_SR=%d\n", IO_CHIPTOP.CRRRS.bit.CPU0_SR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_CPU0_SR_RES" ) ) {
			Dd_Top_Reset_CRRRS_CPU0_SR();
			Ddim_Print(("It set CRRRS_CPU0_SR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU0_SR=%d\n", IO_CHIPTOP.CRRRS.bit.CPU0_SR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_CPU1_SR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_CPU1_SR();
			Ddim_Print(("It set CRRRS_CPU1_SR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU1_SR=%d\n", IO_CHIPTOP.CRRRS.bit.CPU1_SR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_CPU1_SR_RES" ) ) {
			Dd_Top_Reset_CRRRS_CPU1_SR();
			Ddim_Print(("It set CRRRS_CPU1_SR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU1_SR=%d\n", IO_CHIPTOP.CRRRS.bit.CPU1_SR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_CPU2_SR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_CPU2_SR();
			Ddim_Print(("It set CRRRS_CPU2_SR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU2_SR=%d\n", IO_CHIPTOP.CRRRS.bit.CPU2_SR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_CPU2_SR_RES" ) ) {
			Dd_Top_Reset_CRRRS_CPU2_SR();
			Ddim_Print(("It set CRRRS_CPU2_SR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU2_SR=%d\n", IO_CHIPTOP.CRRRS.bit.CPU2_SR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_CPU3_SR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_CPU3_SR();
			Ddim_Print(("It set CRRRS_CPU3_SR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU3_SR=%d\n", IO_CHIPTOP.CRRRS.bit.CPU3_SR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_CPU3_SR_RES" ) ) {
			Dd_Top_Reset_CRRRS_CPU3_SR();
			Ddim_Print(("It set CRRRS_CPU3_SR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.CPU3_SR=%d\n", IO_CHIPTOP.CRRRS.bit.CPU3_SR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_RIBSR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_RIBSR();
			Ddim_Print(("It set CRRRS_RIBSR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.RIBSR=%d\n", IO_CHIPTOP.CRRRS.bit.RIBSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_RIBSR_RES" ) ) {
			Dd_Top_Reset_CRRRS_RIBSR();
			Ddim_Print(("It set CRRRS_RIBSR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.RIBSR=%d\n", IO_CHIPTOP.CRRRS.bit.RIBSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_HEIPPASYNCSR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_HEIPPASYNCSR();
			Ddim_Print(("It set CRRRS_HEIPPASYNCSR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_HEIPPASYNCSR_RES" ) ) {
			Dd_Top_Reset_CRRRS_HEIPPASYNCSR();
			Ddim_Print(("It set CRRRS_HEIPPASYNCSR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEIPPASYNCSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_HEIPPSYNCSR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_HEIPPSYNCSR();
			Ddim_Print(("It set CRRRS_HEIPPSYNCSR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_HEIPPSYNCSR_RES" ) ) {
			Dd_Top_Reset_CRRRS_HEIPPSYNCSR();
			Ddim_Print(("It set CRRRS_HEIPPSYNCSR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEIPPSYNCSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_HEVDFSR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_HEVDFSR();
			Ddim_Print(("It set CRRRS_HEVDFSR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEVDFSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEVDFSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_HEVDFSR_RES" ) ) {
			Dd_Top_Reset_CRRRS_HEVDFSR();
			Ddim_Print(("It set CRRRS_HEVDFSR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEVDFSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEVDFSR));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[3], "CRRRS_HEPXFSR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_HEPXFSR();
			Ddim_Print(("It set CRRRS_HEPXFSR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEPXFSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEPXFSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_HEPXFSR_RES" ) ) {
			Dd_Top_Reset_CRRRS_HEPXFSR();
			Ddim_Print(("It set CRRRS_HEPXFSR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEPXFSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEPXFSR));
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[3], "CRRRS_HEVENCSR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_HEVENCSR();
			Ddim_Print(("It set CRRRS_HEVENCSR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEVENCSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEVENCSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_HEVENCSR_RES" ) ) {
			Dd_Top_Reset_CRRRS_HEVENCSR();
			Ddim_Print(("It set CRRRS_HEVENCSR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.HEVENCSR=%d\n", IO_CHIPTOP.CRRRS.bit.HEVENCSR));
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[3], "CRRRS_DSPSR_CLR" ) ) {
			Dd_Top_Clear_CRRRS_DSPSR();
			Ddim_Print(("It set CRRRS_DSPSR_CLR select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.DSPSR=%d\n", IO_CHIPTOP.CRRRS.bit.DSPSR));
		}
		else if ( 0 == strcmp( argv[3], "CRRRS_DSPSR_RES" ) ) {
			Dd_Top_Reset_CRRRS_DSPSR();
			Ddim_Print(("It set CRRRS_DSPSR_RES select.\n"));
			Ddim_Print(("IO_CHIPTOP.CRRRS.bit.DSPSR=%d\n", IO_CHIPTOP.CRRRS.bit.DSPSR));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}


#ifdef D_DD_TOP_PC_DEBUG
INT32 ct_gpio_copy_pdr_pc( E_DD_TOP_GPIO_PORT port, UCHAR status )
{
	switch( port ) {
		case E_DD_TOP_GPIO_P60: IO_CHIPTOP.PDR.bit.P60 = status; IO_CHIPTOP.PDR.bit.PWE60 = 1; break;
		case E_DD_TOP_GPIO_P61: IO_CHIPTOP.PDR.bit.P61 = status; IO_CHIPTOP.PDR.bit.PWE61 = 1; break;
		case E_DD_TOP_GPIO_P62: IO_CHIPTOP.PDR.bit.P62 = status; IO_CHIPTOP.PDR.bit.PWE62 = 1; break;
		case E_DD_TOP_GPIO_P63: IO_CHIPTOP.PDR.bit.P63 = status; IO_CHIPTOP.PDR.bit.PWE63 = 1; break;
		case E_DD_TOP_GPIO_P64: IO_CHIPTOP.PDR.bit.P64 = status; IO_CHIPTOP.PDR.bit.PWE64 = 1; break;
		case E_DD_TOP_GPIO_P65: IO_CHIPTOP.PDR.bit.P65 = status; IO_CHIPTOP.PDR.bit.PWE65 = 1; break;
		case E_DD_TOP_GPIO_P66: IO_CHIPTOP.PDR.bit.P66 = status; IO_CHIPTOP.PDR.bit.PWE66 = 1; break;
		case E_DD_TOP_GPIO_P67: IO_CHIPTOP.PDR.bit.P67 = status; IO_CHIPTOP.PDR.bit.PWE67 = 1; break;
		case E_DD_TOP_GPIO_P70: IO_CHIPTOP.PDR.bit.P70 = status; IO_CHIPTOP.PDR.bit.PWE70 = 1; break;
		case E_DD_TOP_GPIO_P71: IO_CHIPTOP.PDR.bit.P71 = status; IO_CHIPTOP.PDR.bit.PWE71 = 1; break;
		case E_DD_TOP_GPIO_P72: IO_CHIPTOP.PDR.bit.P72 = status; IO_CHIPTOP.PDR.bit.PWE72 = 1; break;
		case E_DD_TOP_GPIO_P73: IO_CHIPTOP.PDR.bit.P73 = status; IO_CHIPTOP.PDR.bit.PWE73 = 1; break;
		case E_DD_TOP_GPIO_P74: IO_CHIPTOP.PDR.bit.P74 = status; IO_CHIPTOP.PDR.bit.PWE74 = 1; break;
		case E_DD_TOP_GPIO_P75: IO_CHIPTOP.PDR.bit.P75 = status; IO_CHIPTOP.PDR.bit.PWE75 = 1; break;
		case E_DD_TOP_GPIO_P76: IO_CHIPTOP.PDR.bit.P76 = status; IO_CHIPTOP.PDR.bit.PWE76 = 1; break;
		case E_DD_TOP_GPIO_P77: IO_CHIPTOP.PDR.bit.P77 = status; IO_CHIPTOP.PDR.bit.PWE77 = 1; break;
		case E_DD_TOP_GPIO_P80: IO_CHIPTOP.PDR.bit.P80 = status; IO_CHIPTOP.PDR.bit.PWE80 = 1; break;
		case E_DD_TOP_GPIO_P81: IO_CHIPTOP.PDR.bit.P81 = status; IO_CHIPTOP.PDR.bit.PWE81 = 1; break;
		case E_DD_TOP_GPIO_P82: IO_CHIPTOP.PDR.bit.P82 = status; IO_CHIPTOP.PDR.bit.PWE82 = 1; break;
		case E_DD_TOP_GPIO_P83: IO_CHIPTOP.PDR.bit.P83 = status; IO_CHIPTOP.PDR.bit.PWE83 = 1; break;
		case E_DD_TOP_GPIO_P84: IO_CHIPTOP.PDR.bit.P84 = status; IO_CHIPTOP.PDR.bit.PWE84 = 1; break;
		case E_DD_TOP_GPIO_P85: IO_CHIPTOP.PDR.bit.P85 = status; IO_CHIPTOP.PDR.bit.PWE85 = 1; break;
		case E_DD_TOP_GPIO_P86: IO_CHIPTOP.PDR.bit.P86 = status; IO_CHIPTOP.PDR.bit.PWE86 = 1; break;
		case E_DD_TOP_GPIO_P87: IO_CHIPTOP.PDR.bit.P87 = status; IO_CHIPTOP.PDR.bit.PWE87 = 1; break;
		case E_DD_TOP_GPIO_P90: IO_CHIPTOP.PDR.bit.P90 = status; IO_CHIPTOP.PDR.bit.PWE90 = 1; break;
		case E_DD_TOP_GPIO_P91: IO_CHIPTOP.PDR.bit.P91 = status; IO_CHIPTOP.PDR.bit.PWE91 = 1; break;
		case E_DD_TOP_GPIO_P92: IO_CHIPTOP.PDR.bit.P92 = status; IO_CHIPTOP.PDR.bit.PWE92 = 1; break;
		case E_DD_TOP_GPIO_P93: IO_CHIPTOP.PDR.bit.P93 = status; IO_CHIPTOP.PDR.bit.PWE93 = 1; break;
		case E_DD_TOP_GPIO_PA0: IO_CHIPTOP.PDR.bit.PA0 = status; IO_CHIPTOP.PDR.bit.PWEA0 = 1; break;
		case E_DD_TOP_GPIO_PA1: IO_CHIPTOP.PDR.bit.PA1 = status; IO_CHIPTOP.PDR.bit.PWEA1 = 1; break;
		case E_DD_TOP_GPIO_PA2: IO_CHIPTOP.PDR.bit.PA2 = status; IO_CHIPTOP.PDR.bit.PWEA2 = 1; break;
		case E_DD_TOP_GPIO_PA3: IO_CHIPTOP.PDR.bit.PA3 = status; IO_CHIPTOP.PDR.bit.PWEA3 = 1; break;
		case E_DD_TOP_GPIO_PA4: IO_CHIPTOP.PDR.bit.PA4 = status; IO_CHIPTOP.PDR.bit.PWEA4 = 1; break;
		case E_DD_TOP_GPIO_PA5: IO_CHIPTOP.PDR.bit.PA5 = status; IO_CHIPTOP.PDR.bit.PWEA5 = 1; break;
		case E_DD_TOP_GPIO_PA6: IO_CHIPTOP.PDR.bit.PA6 = status; IO_CHIPTOP.PDR.bit.PWEA6 = 1; break;
		case E_DD_TOP_GPIO_PA7: IO_CHIPTOP.PDR.bit.PA7 = status; IO_CHIPTOP.PDR.bit.PWEA7 = 1; break;
		case E_DD_TOP_GPIO_PB0: IO_CHIPTOP.PDR.bit.PB0 = status; IO_CHIPTOP.PDR.bit.PWEB0 = 1; break;
		case E_DD_TOP_GPIO_PB1: IO_CHIPTOP.PDR.bit.PB1 = status; IO_CHIPTOP.PDR.bit.PWEB1 = 1; break;
		case E_DD_TOP_GPIO_PB2: IO_CHIPTOP.PDR.bit.PB2 = status; IO_CHIPTOP.PDR.bit.PWEB2 = 1; break;
		case E_DD_TOP_GPIO_PB3: IO_CHIPTOP.PDR.bit.PB3 = status; IO_CHIPTOP.PDR.bit.PWEB3 = 1; break;
		case E_DD_TOP_GPIO_PB4: IO_CHIPTOP.PDR.bit.PB4 = status; IO_CHIPTOP.PDR.bit.PWEB4 = 1; break;
		case E_DD_TOP_GPIO_PC0L: IO_CHIPTOP.PDR.bit.PC0 = status; IO_CHIPTOP.PDR.bit.PWEC0 = 1; break;
		case E_DD_TOP_GPIO_PC1L: IO_CHIPTOP.PDR.bit.PC1 = status; IO_CHIPTOP.PDR.bit.PWEC1 = 1; break;
		case E_DD_TOP_GPIO_PC2L: IO_CHIPTOP.PDR.bit.PC2 = status; IO_CHIPTOP.PDR.bit.PWEC2 = 1; break;
		case E_DD_TOP_GPIO_PC3L: IO_CHIPTOP.PDR.bit.PC3 = status; IO_CHIPTOP.PDR.bit.PWEC3 = 1; break;
		case E_DD_TOP_GPIO_PC4L: IO_CHIPTOP.PDR.bit.PC4 = status; IO_CHIPTOP.PDR.bit.PWEC4 = 1; break;
		case E_DD_TOP_GPIO_PC5L: IO_CHIPTOP.PDR.bit.PC5 = status; IO_CHIPTOP.PDR.bit.PWEC5 = 1; break;
		case E_DD_TOP_GPIO_PD0: IO_CHIPTOP.PDR.bit.PD0 = status; IO_CHIPTOP.PDR.bit.PWED0 = 1; break;
		case E_DD_TOP_GPIO_PD1: IO_CHIPTOP.PDR.bit.PD1 = status; IO_CHIPTOP.PDR.bit.PWED1 = 1; break;
		case E_DD_TOP_GPIO_PD2: IO_CHIPTOP.PDR.bit.PD2 = status; IO_CHIPTOP.PDR.bit.PWED2 = 1; break;
		case E_DD_TOP_GPIO_PD3: IO_CHIPTOP.PDR.bit.PD3 = status; IO_CHIPTOP.PDR.bit.PWED3 = 1; break;
		case E_DD_TOP_GPIO_PD4: IO_CHIPTOP.PDR.bit.PD4 = status; IO_CHIPTOP.PDR.bit.PWED4 = 1; break;
		case E_DD_TOP_GPIO_PD5: IO_CHIPTOP.PDR.bit.PD5 = status; IO_CHIPTOP.PDR.bit.PWED5 = 1; break;
		case E_DD_TOP_GPIO_PD6: IO_CHIPTOP.PDR.bit.PD6 = status; IO_CHIPTOP.PDR.bit.PWED6 = 1; break;
		case E_DD_TOP_GPIO_PE0: IO_CHIPTOP.PDR.bit.PE0 = status; IO_CHIPTOP.PDR.bit.PWEE0 = 1; break;
		case E_DD_TOP_GPIO_PE1: IO_CHIPTOP.PDR.bit.PE1 = status; IO_CHIPTOP.PDR.bit.PWEE1 = 1; break;
		case E_DD_TOP_GPIO_PE2: IO_CHIPTOP.PDR.bit.PE2 = status; IO_CHIPTOP.PDR.bit.PWEE2 = 1; break;
		case E_DD_TOP_GPIO_PE3: IO_CHIPTOP.PDR.bit.PE3 = status; IO_CHIPTOP.PDR.bit.PWEE3 = 1; break;
		case E_DD_TOP_GPIO_PE4: IO_CHIPTOP.PDR.bit.PE4 = status; IO_CHIPTOP.PDR.bit.PWEE4 = 1; break;
		case E_DD_TOP_GPIO_PE5: IO_CHIPTOP.PDR.bit.PE5 = status; IO_CHIPTOP.PDR.bit.PWEE5 = 1; break;
		case E_DD_TOP_GPIO_PF0: IO_CHIPTOP.PDR.bit.PF0 = status; IO_CHIPTOP.PDR.bit.PWEF0 = 1; break;
		case E_DD_TOP_GPIO_PF1: IO_CHIPTOP.PDR.bit.PF1 = status; IO_CHIPTOP.PDR.bit.PWEF1 = 1; break;
		case E_DD_TOP_GPIO_PF2: IO_CHIPTOP.PDR.bit.PF2 = status; IO_CHIPTOP.PDR.bit.PWEF2 = 1; break;
		case E_DD_TOP_GPIO_PF3: IO_CHIPTOP.PDR.bit.PF3 = status; IO_CHIPTOP.PDR.bit.PWEF3 = 1; break;
		case E_DD_TOP_GPIO_PF4: IO_CHIPTOP.PDR.bit.PF4 = status; IO_CHIPTOP.PDR.bit.PWEF4 = 1; break;
		case E_DD_TOP_GPIO_PG0: IO_CHIPTOP.PDR.bit.PG0 = status; IO_CHIPTOP.PDR.bit.PWEG0 = 1; break;
		case E_DD_TOP_GPIO_PG1: IO_CHIPTOP.PDR.bit.PG1 = status; IO_CHIPTOP.PDR.bit.PWEG1 = 1; break;
		case E_DD_TOP_GPIO_PG2: IO_CHIPTOP.PDR.bit.PG2 = status; IO_CHIPTOP.PDR.bit.PWEG2 = 1; break;
		case E_DD_TOP_GPIO_PG3: IO_CHIPTOP.PDR.bit.PG3 = status; IO_CHIPTOP.PDR.bit.PWEG3 = 1; break;
		case E_DD_TOP_GPIO_PG4: IO_CHIPTOP.PDR.bit.PG4 = status; IO_CHIPTOP.PDR.bit.PWEG4 = 1; break;
		case E_DD_TOP_GPIO_PG5: IO_CHIPTOP.PDR.bit.PG5 = status; IO_CHIPTOP.PDR.bit.PWEG5 = 1; break;
		case E_DD_TOP_GPIO_PG6: IO_CHIPTOP.PDR.bit.PG6 = status; IO_CHIPTOP.PDR.bit.PWEG6 = 1; break;
		case E_DD_TOP_GPIO_PG7: IO_CHIPTOP.PDR.bit.PG7 = status; IO_CHIPTOP.PDR.bit.PWEG7 = 1; break;
		case E_DD_TOP_GPIO_PH0: IO_CHIPTOP.PDR.bit.PH0 = status; IO_CHIPTOP.PDR.bit.PWEH0 = 1; break;
		case E_DD_TOP_GPIO_PH1: IO_CHIPTOP.PDR.bit.PH1 = status; IO_CHIPTOP.PDR.bit.PWEH1 = 1; break;
		case E_DD_TOP_GPIO_PH2: IO_CHIPTOP.PDR.bit.PH2 = status; IO_CHIPTOP.PDR.bit.PWEH2 = 1; break;
		case E_DD_TOP_GPIO_PH3: IO_CHIPTOP.PDR.bit.PH3 = status; IO_CHIPTOP.PDR.bit.PWEH3 = 1; break;
		case E_DD_TOP_GPIO_PW0: IO_CHIPTOP.PDR.bit.PW0 = status; IO_CHIPTOP.PDR.bit.PWEW0 = 1; break;
		case E_DD_TOP_GPIO_PW1: IO_CHIPTOP.PDR.bit.PW1 = status; IO_CHIPTOP.PDR.bit.PWEW1 = 1; break;
		case E_DD_TOP_GPIO_PW2: IO_CHIPTOP.PDR.bit.PW2 = status; IO_CHIPTOP.PDR.bit.PWEW2 = 1; break;
		case E_DD_TOP_GPIO_PW3: IO_CHIPTOP.PDR.bit.PW3 = status; IO_CHIPTOP.PDR.bit.PWEW3 = 1; break;
		case E_DD_TOP_GPIO_PW4: IO_CHIPTOP.PDR.bit.PW4 = status; IO_CHIPTOP.PDR.bit.PWEW4 = 1; break;
		case E_DD_TOP_GPIO_PW5: IO_CHIPTOP.PDR.bit.PW5 = status; IO_CHIPTOP.PDR.bit.PWEW5 = 1; break;
		case E_DD_TOP_GPIO_PW6: IO_CHIPTOP.PDR.bit.PW6 = status; IO_CHIPTOP.PDR.bit.PWEW6 = 1; break;
		case E_DD_TOP_GPIO_PW7: IO_CHIPTOP.PDR.bit.PW7 = status; IO_CHIPTOP.PDR.bit.PWEW7 = 1; break;
		case E_DD_TOP_GPIO_PJ0: IO_CHIPTOP.PDR.bit.PJ0 = status; IO_CHIPTOP.PDR.bit.PWEJ0 = 1; break;
		case E_DD_TOP_GPIO_PJ1: IO_CHIPTOP.PDR.bit.PJ1 = status; IO_CHIPTOP.PDR.bit.PWEJ1 = 1; break;
		case E_DD_TOP_GPIO_PJ2: IO_CHIPTOP.PDR.bit.PJ2 = status; IO_CHIPTOP.PDR.bit.PWEJ2 = 1; break;
		case E_DD_TOP_GPIO_PJ3: IO_CHIPTOP.PDR.bit.PJ3 = status; IO_CHIPTOP.PDR.bit.PWEJ3 = 1; break;
		case E_DD_TOP_GPIO_PJ4: IO_CHIPTOP.PDR.bit.PJ4 = status; IO_CHIPTOP.PDR.bit.PWEJ4 = 1; break;
		case E_DD_TOP_GPIO_PJ5: IO_CHIPTOP.PDR.bit.PJ5 = status; IO_CHIPTOP.PDR.bit.PWEJ5 = 1; break;
		case E_DD_TOP_GPIO_PJ6: IO_CHIPTOP.PDR.bit.PJ6 = status; IO_CHIPTOP.PDR.bit.PWEJ6 = 1; break;
		case E_DD_TOP_GPIO_PJ7: IO_CHIPTOP.PDR.bit.PJ7 = status; IO_CHIPTOP.PDR.bit.PWEJ7 = 1; break;
		case E_DD_TOP_GPIO_PK0: IO_CHIPTOP.PDR.bit.PK0 = status; IO_CHIPTOP.PDR.bit.PWEK0 = 1; break;
		case E_DD_TOP_GPIO_PK1: IO_CHIPTOP.PDR.bit.PK1 = status; IO_CHIPTOP.PDR.bit.PWEK1 = 1; break;
		case E_DD_TOP_GPIO_PK2: IO_CHIPTOP.PDR.bit.PK2 = status; IO_CHIPTOP.PDR.bit.PWEK2 = 1; break;
		case E_DD_TOP_GPIO_PK3: IO_CHIPTOP.PDR.bit.PK3 = status; IO_CHIPTOP.PDR.bit.PWEK3 = 1; break;
		case E_DD_TOP_GPIO_PK4: IO_CHIPTOP.PDR.bit.PK4 = status; IO_CHIPTOP.PDR.bit.PWEK4 = 1; break;
		case E_DD_TOP_GPIO_PK5: IO_CHIPTOP.PDR.bit.PK5 = status; IO_CHIPTOP.PDR.bit.PWEK5 = 1; break;
		case E_DD_TOP_GPIO_PK6: IO_CHIPTOP.PDR.bit.PK6 = status; IO_CHIPTOP.PDR.bit.PWEK6 = 1; break;
		case E_DD_TOP_GPIO_PK7: IO_CHIPTOP.PDR.bit.PK7 = status; IO_CHIPTOP.PDR.bit.PWEK7 = 1; break;
		case E_DD_TOP_GPIO_PL0: IO_CHIPTOP.PDR.bit.PL0 = status; IO_CHIPTOP.PDR.bit.PWEL0 = 1; break;
		case E_DD_TOP_GPIO_PL1: IO_CHIPTOP.PDR.bit.PL1 = status; IO_CHIPTOP.PDR.bit.PWEL1 = 1; break;
		case E_DD_TOP_GPIO_PL2: IO_CHIPTOP.PDR.bit.PL2 = status; IO_CHIPTOP.PDR.bit.PWEL2 = 1; break;
		case E_DD_TOP_GPIO_PL3: IO_CHIPTOP.PDR.bit.PL3 = status; IO_CHIPTOP.PDR.bit.PWEL3 = 1; break;
		case E_DD_TOP_GPIO_PL4: IO_CHIPTOP.PDR.bit.PL4 = status; IO_CHIPTOP.PDR.bit.PWEL4 = 1; break;
		case E_DD_TOP_GPIO_PL5: IO_CHIPTOP.PDR.bit.PL5 = status; IO_CHIPTOP.PDR.bit.PWEL5 = 1; break;
		case E_DD_TOP_GPIO_PM0: IO_CHIPTOP.PDR.bit.PM0 = status; IO_CHIPTOP.PDR.bit.PWEM0 = 1; break;
		case E_DD_TOP_GPIO_PM1: IO_CHIPTOP.PDR.bit.PM1 = status; IO_CHIPTOP.PDR.bit.PWEM1 = 1; break;
		case E_DD_TOP_GPIO_PM2: IO_CHIPTOP.PDR.bit.PM2 = status; IO_CHIPTOP.PDR.bit.PWEM2 = 1; break;
		case E_DD_TOP_GPIO_PM3: IO_CHIPTOP.PDR.bit.PM3 = status; IO_CHIPTOP.PDR.bit.PWEM3 = 1; break;
		case E_DD_TOP_GPIO_PM4: IO_CHIPTOP.PDR.bit.PM4 = status; IO_CHIPTOP.PDR.bit.PWEM4 = 1; break;
		case E_DD_TOP_GPIO_PN0: IO_CHIPTOP.PDR.bit.PN0 = status; IO_CHIPTOP.PDR.bit.PWEN0 = 1; break;
		case E_DD_TOP_GPIO_PN1: IO_CHIPTOP.PDR.bit.PN1 = status; IO_CHIPTOP.PDR.bit.PWEN1 = 1; break;
		case E_DD_TOP_GPIO_PN2: IO_CHIPTOP.PDR.bit.PN2 = status; IO_CHIPTOP.PDR.bit.PWEN2 = 1; break;
		case E_DD_TOP_GPIO_PN3: IO_CHIPTOP.PDR.bit.PN3 = status; IO_CHIPTOP.PDR.bit.PWEN3 = 1; break;
		case E_DD_TOP_GPIO_PN4: IO_CHIPTOP.PDR.bit.PN4 = status; IO_CHIPTOP.PDR.bit.PWEN4 = 1; break;
		case E_DD_TOP_GPIO_PY0: IO_CHIPTOP.PDR.bit.PY0 = status; IO_CHIPTOP.PDR.bit.PWEY0 = 1; break;
		case E_DD_TOP_GPIO_PY1: IO_CHIPTOP.PDR.bit.PY1 = status; IO_CHIPTOP.PDR.bit.PWEY1 = 1; break;
		case E_DD_TOP_GPIO_PY2: IO_CHIPTOP.PDR.bit.PY2 = status; IO_CHIPTOP.PDR.bit.PWEY2 = 1; break;
		case E_DD_TOP_GPIO_PY3: IO_CHIPTOP.PDR.bit.PY3 = status; IO_CHIPTOP.PDR.bit.PWEY3 = 1; break;
		case E_DD_TOP_GPIO_PY4: IO_CHIPTOP.PDR.bit.PY4 = status; IO_CHIPTOP.PDR.bit.PWEY4 = 1; break;
		case E_DD_TOP_GPIO_PY5: IO_CHIPTOP.PDR.bit.PY5 = status; IO_CHIPTOP.PDR.bit.PWEY5 = 1; break;
		case E_DD_TOP_GPIO_PY6: IO_CHIPTOP.PDR.bit.PY6 = status; IO_CHIPTOP.PDR.bit.PWEY6 = 1; break;
		case E_DD_TOP_GPIO_PY7: IO_CHIPTOP.PDR.bit.PY7 = status; IO_CHIPTOP.PDR.bit.PWEY7 = 1; break;
		case E_DD_TOP_GPIO_PP0: IO_CHIPTOP.PDR.bit.PP0 = status; IO_CHIPTOP.PDR.bit.PWEP0 = 1; break;
		case E_DD_TOP_GPIO_PP1: IO_CHIPTOP.PDR.bit.PP1 = status; IO_CHIPTOP.PDR.bit.PWEP1 = 1; break;
		case E_DD_TOP_GPIO_PP2: IO_CHIPTOP.PDR.bit.PP2 = status; IO_CHIPTOP.PDR.bit.PWEP2 = 1; break;
		case E_DD_TOP_GPIO_PP3: IO_CHIPTOP.PDR.bit.PP3 = status; IO_CHIPTOP.PDR.bit.PWEP3 = 1; break;
		default:
			break;;
	}

	return D_DDIM_OK;
}
#endif

static void ct_get_gpio_ctrl(CHAR* reg, CHAR* port)
{
	UCHAR val;
	INT32 ret;
	E_DD_TOP_GPIO_PORT port_index;

	for ( port_index = (E_DD_TOP_GPIO_PORT)0; port_index < E_DD_TOP_GPIO_PORT_MAX; port_index++ ) {
		if ( 0 == strcmp( port, gDD_Top_Port_Name[port_index] ) ) {
			break;
		}
	}
	if ( port_index == E_DD_TOP_GPIO_PORT_MAX ){
		Ddim_Print(("please check 4th parameter!!\n"));
		return;
	}

	if ( 0 == strcmp( reg, "EPCR" ) ) {
		ret = Dd_Top_Get_Gpio_Function( port_index, &val );
		Ddim_Print(("Dd_Top_Get_Gpio_Function(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else if ( 0 == strcmp( reg, "DDR" ) ) {
		ret = Dd_Top_Get_Gpio_Direction( port_index, &val );
		Ddim_Print(("Dd_Top_Get_Gpio_Direction(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else if ( 0 == strcmp( reg, "PDR" ) ) {
		ret = Dd_Top_Get_Gpio_Status( port_index, &val );
		Ddim_Print(("Dd_Top_Get_Gpio_Status(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else if ( 0 == strcmp( reg, "PUDER" ) ) {
		ret = Dd_Top_Get_Gpio_Pull_Up_Down_Enable( port_index, &val );
		Ddim_Print(("Dd_Top_Get_Gpio_Pull_Up_Down_Enable(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else if ( 0 == strcmp( reg, "PUDCR" ) ) {
		ret = Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl( port_index, &val );
		Ddim_Print(("Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else {
		Ddim_Print(("please check 3rd parameter!!\n"));
	}
}

static void ct_set_gpio_ctrl(CHAR* reg, CHAR* port, UCHAR val)
{
	INT32 ret;
	E_DD_TOP_GPIO_PORT port_index;

	for ( port_index = (E_DD_TOP_GPIO_PORT)0; port_index < E_DD_TOP_GPIO_PORT_MAX; port_index++ ) {
		if ( 0 == strcmp( port, gDD_Top_Port_Name[port_index] ) ) {
			break;
		}
	}
	if ( port_index == E_DD_TOP_GPIO_PORT_MAX ){
		Ddim_Print(("please check 4th parameter!!\n"));
		return;
	}

	if ( 0 == strcmp( reg, "EPCR" ) ) {
		ret = Dd_Top_Set_Gpio_Function( port_index, val );
		Ddim_Print(("Dd_Top_Set_Gpio_Function(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else if ( 0 == strcmp( reg, "DDR" ) ) {
		ret = Dd_Top_Set_Gpio_Direction( port_index, val );
		Ddim_Print(("Dd_Top_Set_Gpio_Direction(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else if ( 0 == strcmp( reg, "PDR" ) ) {
		ret = Dd_Top_Set_Gpio_Status( port_index, val );
#ifdef D_DD_TOP_PC_DEBUG
		ct_gpio_copy_pdr_pc( port_index, val );
#endif
		Ddim_Print(("Dd_Top_Set_Gpio_Status(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else if ( 0 == strcmp( reg, "PUDER" ) ) {
		ret = Dd_Top_Set_Gpio_Pull_Up_Down_Enable( port_index, val );
		Ddim_Print(("Dd_Top_Set_Gpio_Pull_Up_Down_Enable(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else if ( 0 == strcmp( reg, "PUDCR" ) ) {
		ret = Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl( port_index, val );
		Ddim_Print(("Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(): ret=%d, %s.%s=%d\n", ret, reg, port, val));
	}
	else {
		Ddim_Print(("please check 3rd parameter!!\n"));
	}
}

static void ct_gpio_test_pdr( int argc, char** argv )
{

	/* PDR */
	ct_set_gpio_ctrl( "PDR", "P60", 1 );
	ct_get_gpio_ctrl( "PDR", "P60");
	ct_set_gpio_ctrl( "PDR", "P60", 0 );
	ct_get_gpio_ctrl( "PDR", "P60");
	ct_set_gpio_ctrl( "PDR", "P61", 1 );
	ct_get_gpio_ctrl( "PDR", "P61");
	ct_set_gpio_ctrl( "PDR", "P61", 0 );
	ct_get_gpio_ctrl( "PDR", "P61");
	ct_set_gpio_ctrl( "PDR", "P62", 1 );
	ct_get_gpio_ctrl( "PDR", "P62");
	ct_set_gpio_ctrl( "PDR", "P62", 0 );
	ct_get_gpio_ctrl( "PDR", "P62");
	ct_set_gpio_ctrl( "PDR", "P63", 1 );
	ct_get_gpio_ctrl( "PDR", "P63");
	ct_set_gpio_ctrl( "PDR", "P63", 0 );
	ct_get_gpio_ctrl( "PDR", "P63");
	ct_set_gpio_ctrl( "PDR", "P64", 1 );
	ct_get_gpio_ctrl( "PDR", "P64");
	ct_set_gpio_ctrl( "PDR", "P64", 0 );
	ct_get_gpio_ctrl( "PDR", "P64");
	ct_set_gpio_ctrl( "PDR", "P65", 1 );
	ct_get_gpio_ctrl( "PDR", "P65");
	ct_set_gpio_ctrl( "PDR", "P65", 0 );
	ct_get_gpio_ctrl( "PDR", "P65");
	ct_set_gpio_ctrl( "PDR", "P66", 1 );
	ct_get_gpio_ctrl( "PDR", "P66");
	ct_set_gpio_ctrl( "PDR", "P66", 0 );
	ct_get_gpio_ctrl( "PDR", "P66");
	ct_set_gpio_ctrl( "PDR", "P67", 1 );
	ct_get_gpio_ctrl( "PDR", "P67");
	ct_set_gpio_ctrl( "PDR", "P67", 0 );
	ct_get_gpio_ctrl( "PDR", "P67");
	ct_set_gpio_ctrl( "PDR", "P70", 1 );
	ct_get_gpio_ctrl( "PDR", "P70");
	ct_set_gpio_ctrl( "PDR", "P70", 0 );
	ct_get_gpio_ctrl( "PDR", "P70");
	ct_set_gpio_ctrl( "PDR", "P71", 1 );
	ct_get_gpio_ctrl( "PDR", "P71");
	ct_set_gpio_ctrl( "PDR", "P71", 0 );
	ct_get_gpio_ctrl( "PDR", "P71");
	ct_set_gpio_ctrl( "PDR", "P72", 1 );
	ct_get_gpio_ctrl( "PDR", "P72");
	ct_set_gpio_ctrl( "PDR", "P72", 0 );
	ct_get_gpio_ctrl( "PDR", "P72");
	ct_set_gpio_ctrl( "PDR", "P73", 1 );
	ct_get_gpio_ctrl( "PDR", "P73");
	ct_set_gpio_ctrl( "PDR", "P73", 0 );
	ct_get_gpio_ctrl( "PDR", "P73");
	ct_set_gpio_ctrl( "PDR", "P74", 1 );
	ct_get_gpio_ctrl( "PDR", "P74");
	ct_set_gpio_ctrl( "PDR", "P74", 0 );
	ct_get_gpio_ctrl( "PDR", "P74");
	ct_set_gpio_ctrl( "PDR", "P75", 1 );
	ct_get_gpio_ctrl( "PDR", "P75");
	ct_set_gpio_ctrl( "PDR", "P75", 0 );
	ct_get_gpio_ctrl( "PDR", "P75");
	ct_set_gpio_ctrl( "PDR", "P76", 1 );
	ct_get_gpio_ctrl( "PDR", "P76");
	ct_set_gpio_ctrl( "PDR", "P76", 0 );
	ct_get_gpio_ctrl( "PDR", "P76");
	ct_set_gpio_ctrl( "PDR", "P77", 1 );
	ct_get_gpio_ctrl( "PDR", "P77");
	ct_set_gpio_ctrl( "PDR", "P77", 0 );
	ct_get_gpio_ctrl( "PDR", "P77");
	ct_set_gpio_ctrl( "PDR", "P80", 1 );
	ct_get_gpio_ctrl( "PDR", "P80");
	ct_set_gpio_ctrl( "PDR", "P80", 0 );
	ct_get_gpio_ctrl( "PDR", "P80");
	ct_set_gpio_ctrl( "PDR", "P81", 1 );
	ct_get_gpio_ctrl( "PDR", "P81");
	ct_set_gpio_ctrl( "PDR", "P81", 0 );
	ct_get_gpio_ctrl( "PDR", "P81");
	ct_set_gpio_ctrl( "PDR", "P82", 1 );
	ct_get_gpio_ctrl( "PDR", "P82");
	ct_set_gpio_ctrl( "PDR", "P82", 0 );
	ct_get_gpio_ctrl( "PDR", "P82");
	ct_set_gpio_ctrl( "PDR", "P83", 1 );
	ct_get_gpio_ctrl( "PDR", "P83");
	ct_set_gpio_ctrl( "PDR", "P83", 0 );
	ct_get_gpio_ctrl( "PDR", "P83");
	ct_set_gpio_ctrl( "PDR", "P84", 1 );
	ct_get_gpio_ctrl( "PDR", "P84");
	ct_set_gpio_ctrl( "PDR", "P84", 0 );
	ct_get_gpio_ctrl( "PDR", "P84");
	ct_set_gpio_ctrl( "PDR", "P85", 1 );
	ct_get_gpio_ctrl( "PDR", "P85");
	ct_set_gpio_ctrl( "PDR", "P85", 0 );
	ct_get_gpio_ctrl( "PDR", "P85");
	ct_set_gpio_ctrl( "PDR", "P86", 1 );
	ct_get_gpio_ctrl( "PDR", "P86");
	ct_set_gpio_ctrl( "PDR", "P86", 0 );
	ct_get_gpio_ctrl( "PDR", "P86");
	ct_set_gpio_ctrl( "PDR", "P87", 1 );
	ct_get_gpio_ctrl( "PDR", "P87");
	ct_set_gpio_ctrl( "PDR", "P87", 0 );
	ct_get_gpio_ctrl( "PDR", "P87");
	ct_set_gpio_ctrl( "PDR", "P90", 1 );
	ct_get_gpio_ctrl( "PDR", "P90");
	ct_set_gpio_ctrl( "PDR", "P90", 0 );
	ct_get_gpio_ctrl( "PDR", "P90");
	ct_set_gpio_ctrl( "PDR", "P91", 1 );
	ct_get_gpio_ctrl( "PDR", "P91");
	ct_set_gpio_ctrl( "PDR", "P91", 0 );
	ct_get_gpio_ctrl( "PDR", "P91");
	ct_set_gpio_ctrl( "PDR", "P92", 1 );
	ct_get_gpio_ctrl( "PDR", "P92");
	ct_set_gpio_ctrl( "PDR", "P92", 0 );
	ct_get_gpio_ctrl( "PDR", "P92");
	ct_set_gpio_ctrl( "PDR", "P93", 1 );
	ct_get_gpio_ctrl( "PDR", "P93");
	ct_set_gpio_ctrl( "PDR", "P93", 0 );
	ct_get_gpio_ctrl( "PDR", "P93");
	ct_set_gpio_ctrl( "PDR", "PA0", 1 );
	ct_get_gpio_ctrl( "PDR", "PA0");
	ct_set_gpio_ctrl( "PDR", "PA0", 0 );
	ct_get_gpio_ctrl( "PDR", "PA0");
	ct_set_gpio_ctrl( "PDR", "PA1", 1 );
	ct_get_gpio_ctrl( "PDR", "PA1");
	ct_set_gpio_ctrl( "PDR", "PA1", 0 );
	ct_get_gpio_ctrl( "PDR", "PA1");
	ct_set_gpio_ctrl( "PDR", "PA2", 1 );
	ct_get_gpio_ctrl( "PDR", "PA2");
	ct_set_gpio_ctrl( "PDR", "PA2", 0 );
	ct_get_gpio_ctrl( "PDR", "PA2");
	ct_set_gpio_ctrl( "PDR", "PA3", 1 );
	ct_get_gpio_ctrl( "PDR", "PA3");
	ct_set_gpio_ctrl( "PDR", "PA3", 0 );
	ct_get_gpio_ctrl( "PDR", "PA3");
	ct_set_gpio_ctrl( "PDR", "PA4", 1 );
	ct_get_gpio_ctrl( "PDR", "PA4");
	ct_set_gpio_ctrl( "PDR", "PA4", 0 );
	ct_get_gpio_ctrl( "PDR", "PA4");
	ct_set_gpio_ctrl( "PDR", "PA5", 1 );
	ct_get_gpio_ctrl( "PDR", "PA5");
	ct_set_gpio_ctrl( "PDR", "PA5", 0 );
	ct_get_gpio_ctrl( "PDR", "PA5");
	ct_set_gpio_ctrl( "PDR", "PA6", 1 );
	ct_get_gpio_ctrl( "PDR", "PA6");
	ct_set_gpio_ctrl( "PDR", "PA6", 0 );
	ct_get_gpio_ctrl( "PDR", "PA6");
	ct_set_gpio_ctrl( "PDR", "PA7", 1 );
	ct_get_gpio_ctrl( "PDR", "PA7");
	ct_set_gpio_ctrl( "PDR", "PA7", 0 );
	ct_get_gpio_ctrl( "PDR", "PA7");
	ct_set_gpio_ctrl( "PDR", "PB0", 1 );
	ct_get_gpio_ctrl( "PDR", "PB0");
	ct_set_gpio_ctrl( "PDR", "PB0", 0 );
	ct_get_gpio_ctrl( "PDR", "PB0");
	ct_set_gpio_ctrl( "PDR", "PB1", 1 );
	ct_get_gpio_ctrl( "PDR", "PB1");
	ct_set_gpio_ctrl( "PDR", "PB1", 0 );
	ct_get_gpio_ctrl( "PDR", "PB1");
	ct_set_gpio_ctrl( "PDR", "PB2", 1 );
	ct_get_gpio_ctrl( "PDR", "PB2");
	ct_set_gpio_ctrl( "PDR", "PB2", 0 );
	ct_get_gpio_ctrl( "PDR", "PB2");
	ct_set_gpio_ctrl( "PDR", "PB3", 1 );
	ct_get_gpio_ctrl( "PDR", "PB3");
	ct_set_gpio_ctrl( "PDR", "PB3", 0 );
	ct_get_gpio_ctrl( "PDR", "PB3");
	ct_set_gpio_ctrl( "PDR", "PB4", 1 );
	ct_get_gpio_ctrl( "PDR", "PB4");
	ct_set_gpio_ctrl( "PDR", "PB4", 0 );
	ct_get_gpio_ctrl( "PDR", "PB4");
	ct_set_gpio_ctrl( "PDR", "PC0", 1 );
	ct_get_gpio_ctrl( "PDR", "PC0");
	ct_set_gpio_ctrl( "PDR", "PC0", 0 );
	ct_get_gpio_ctrl( "PDR", "PC0");
	ct_set_gpio_ctrl( "PDR", "PC1", 1 );
	ct_get_gpio_ctrl( "PDR", "PC1");
	ct_set_gpio_ctrl( "PDR", "PC1", 0 );
	ct_get_gpio_ctrl( "PDR", "PC1");
	ct_set_gpio_ctrl( "PDR", "PC2", 1 );
	ct_get_gpio_ctrl( "PDR", "PC2");
	ct_set_gpio_ctrl( "PDR", "PC2", 0 );
	ct_get_gpio_ctrl( "PDR", "PC2");
	ct_set_gpio_ctrl( "PDR", "PC3", 1 );
	ct_get_gpio_ctrl( "PDR", "PC3");
	ct_set_gpio_ctrl( "PDR", "PC3", 0 );
	ct_get_gpio_ctrl( "PDR", "PC3");
	ct_set_gpio_ctrl( "PDR", "PC4", 1 );
	ct_get_gpio_ctrl( "PDR", "PC4");
	ct_set_gpio_ctrl( "PDR", "PC4", 0 );
	ct_get_gpio_ctrl( "PDR", "PC4");
	ct_set_gpio_ctrl( "PDR", "PC5", 1 );
	ct_get_gpio_ctrl( "PDR", "PC5");
	ct_set_gpio_ctrl( "PDR", "PC5", 0 );
	ct_get_gpio_ctrl( "PDR", "PC5");
	ct_set_gpio_ctrl( "PDR", "PD0", 1 );
	ct_get_gpio_ctrl( "PDR", "PD0");
	ct_set_gpio_ctrl( "PDR", "PD0", 0 );
	ct_get_gpio_ctrl( "PDR", "PD0");
	ct_set_gpio_ctrl( "PDR", "PD1", 1 );
	ct_get_gpio_ctrl( "PDR", "PD1");
	ct_set_gpio_ctrl( "PDR", "PD1", 0 );
	ct_get_gpio_ctrl( "PDR", "PD1");
	ct_set_gpio_ctrl( "PDR", "PD2", 1 );
	ct_get_gpio_ctrl( "PDR", "PD2");
	ct_set_gpio_ctrl( "PDR", "PD2", 0 );
	ct_get_gpio_ctrl( "PDR", "PD2");
	ct_set_gpio_ctrl( "PDR", "PD3", 1 );
	ct_get_gpio_ctrl( "PDR", "PD3");
	ct_set_gpio_ctrl( "PDR", "PD3", 0 );
	ct_get_gpio_ctrl( "PDR", "PD3");
	ct_set_gpio_ctrl( "PDR", "PD4", 1 );
	ct_get_gpio_ctrl( "PDR", "PD4");
	ct_set_gpio_ctrl( "PDR", "PD4", 0 );
	ct_get_gpio_ctrl( "PDR", "PD4");
	ct_set_gpio_ctrl( "PDR", "PD5", 1 );
	ct_get_gpio_ctrl( "PDR", "PD5");
	ct_set_gpio_ctrl( "PDR", "PD5", 0 );
	ct_get_gpio_ctrl( "PDR", "PD5");
	ct_set_gpio_ctrl( "PDR", "PD6", 1 );
	ct_get_gpio_ctrl( "PDR", "PD6");
	ct_set_gpio_ctrl( "PDR", "PD6", 0 );
	ct_get_gpio_ctrl( "PDR", "PD6");
//	ct_set_gpio_ctrl( "PDR", "PE0", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE0");
//	ct_set_gpio_ctrl( "PDR", "PE0", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE0");
//	ct_set_gpio_ctrl( "PDR", "PE1", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE1");
//	ct_set_gpio_ctrl( "PDR", "PE1", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE1");
//	ct_set_gpio_ctrl( "PDR", "PE2", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE2");
//	ct_set_gpio_ctrl( "PDR", "PE2", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE2");
//	ct_set_gpio_ctrl( "PDR", "PE3", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE3");
//	ct_set_gpio_ctrl( "PDR", "PE3", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE3");
//	ct_set_gpio_ctrl( "PDR", "PE4", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE4");
//	ct_set_gpio_ctrl( "PDR", "PE4", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE4");
//	ct_set_gpio_ctrl( "PDR", "PE5", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE5");
//	ct_set_gpio_ctrl( "PDR", "PE5", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE5");
//	ct_set_gpio_ctrl( "PDR", "PF0", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF0");
//	ct_set_gpio_ctrl( "PDR", "PF0", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF0");
//	ct_set_gpio_ctrl( "PDR", "PF1", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF1");
//	ct_set_gpio_ctrl( "PDR", "PF1", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF1");
//	ct_set_gpio_ctrl( "PDR", "PF2", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF2");
//	ct_set_gpio_ctrl( "PDR", "PF2", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF2");
//	ct_set_gpio_ctrl( "PDR", "PF3", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF3");
//	ct_set_gpio_ctrl( "PDR", "PF3", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF3");
//	ct_set_gpio_ctrl( "PDR", "PF4", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF4");
//	ct_set_gpio_ctrl( "PDR", "PF4", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF4");
	ct_set_gpio_ctrl( "PDR", "PG0", 1 );
	ct_get_gpio_ctrl( "PDR", "PG0");
	ct_set_gpio_ctrl( "PDR", "PG0", 0 );
	ct_get_gpio_ctrl( "PDR", "PG0");
	ct_set_gpio_ctrl( "PDR", "PG1", 1 );
	ct_get_gpio_ctrl( "PDR", "PG1");
	ct_set_gpio_ctrl( "PDR", "PG1", 0 );
	ct_get_gpio_ctrl( "PDR", "PG1");
	ct_set_gpio_ctrl( "PDR", "PG2", 1 );
	ct_get_gpio_ctrl( "PDR", "PG2");
	ct_set_gpio_ctrl( "PDR", "PG2", 0 );
	ct_get_gpio_ctrl( "PDR", "PG2");
	ct_set_gpio_ctrl( "PDR", "PG3", 1 );
	ct_get_gpio_ctrl( "PDR", "PG3");
	ct_set_gpio_ctrl( "PDR", "PG3", 0 );
	ct_get_gpio_ctrl( "PDR", "PG3");
	ct_set_gpio_ctrl( "PDR", "PG4", 1 );
	ct_get_gpio_ctrl( "PDR", "PG4");
	ct_set_gpio_ctrl( "PDR", "PG4", 0 );
	ct_get_gpio_ctrl( "PDR", "PG4");
	ct_set_gpio_ctrl( "PDR", "PG5", 1 );
	ct_get_gpio_ctrl( "PDR", "PG5");
	ct_set_gpio_ctrl( "PDR", "PG5", 0 );
	ct_get_gpio_ctrl( "PDR", "PG5");
	ct_set_gpio_ctrl( "PDR", "PG6", 1 );
	ct_get_gpio_ctrl( "PDR", "PG6");
	ct_set_gpio_ctrl( "PDR", "PG6", 0 );
	ct_get_gpio_ctrl( "PDR", "PG6");
	ct_set_gpio_ctrl( "PDR", "PG7", 1 );
	ct_get_gpio_ctrl( "PDR", "PG7");
	ct_set_gpio_ctrl( "PDR", "PG7", 0 );
	ct_get_gpio_ctrl( "PDR", "PG7");
	ct_set_gpio_ctrl( "PDR", "PH0", 1 );
	ct_get_gpio_ctrl( "PDR", "PH0");
	ct_set_gpio_ctrl( "PDR", "PH0", 0 );
	ct_get_gpio_ctrl( "PDR", "PH0");
	ct_set_gpio_ctrl( "PDR", "PH1", 1 );
	ct_get_gpio_ctrl( "PDR", "PH1");
	ct_set_gpio_ctrl( "PDR", "PH1", 0 );
	ct_get_gpio_ctrl( "PDR", "PH1");
	ct_set_gpio_ctrl( "PDR", "PH2", 1 );
	ct_get_gpio_ctrl( "PDR", "PH2");
	ct_set_gpio_ctrl( "PDR", "PH2", 0 );
	ct_get_gpio_ctrl( "PDR", "PH2");
	ct_set_gpio_ctrl( "PDR", "PH3", 1 );
	ct_get_gpio_ctrl( "PDR", "PH3");
	ct_set_gpio_ctrl( "PDR", "PH3", 0 );
	ct_get_gpio_ctrl( "PDR", "PH3");
	ct_set_gpio_ctrl( "PDR", "PW0", 1 );
	ct_get_gpio_ctrl( "PDR", "PW0");
	ct_set_gpio_ctrl( "PDR", "PW0", 0 );
	ct_get_gpio_ctrl( "PDR", "PW0");
	ct_set_gpio_ctrl( "PDR", "PW1", 1 );
	ct_get_gpio_ctrl( "PDR", "PW1");
	ct_set_gpio_ctrl( "PDR", "PW1", 0 );
	ct_get_gpio_ctrl( "PDR", "PW1");
	ct_set_gpio_ctrl( "PDR", "PW2", 1 );
	ct_get_gpio_ctrl( "PDR", "PW2");
	ct_set_gpio_ctrl( "PDR", "PW2", 0 );
	ct_get_gpio_ctrl( "PDR", "PW2");
	ct_set_gpio_ctrl( "PDR", "PW3", 1 );
	ct_get_gpio_ctrl( "PDR", "PW3");
	ct_set_gpio_ctrl( "PDR", "PW3", 0 );
	ct_get_gpio_ctrl( "PDR", "PW3");
	ct_set_gpio_ctrl( "PDR", "PW4", 1 );
	ct_get_gpio_ctrl( "PDR", "PW4");
	ct_set_gpio_ctrl( "PDR", "PW4", 0 );
	ct_get_gpio_ctrl( "PDR", "PW4");
	ct_set_gpio_ctrl( "PDR", "PW5", 1 );
	ct_get_gpio_ctrl( "PDR", "PW5");
	ct_set_gpio_ctrl( "PDR", "PW5", 0 );
	ct_get_gpio_ctrl( "PDR", "PW5");
	ct_set_gpio_ctrl( "PDR", "PW6", 1 );
	ct_get_gpio_ctrl( "PDR", "PW6");
	ct_set_gpio_ctrl( "PDR", "PW6", 0 );
	ct_get_gpio_ctrl( "PDR", "PW6");
	ct_set_gpio_ctrl( "PDR", "PW7", 1 );
	ct_get_gpio_ctrl( "PDR", "PW7");
	ct_set_gpio_ctrl( "PDR", "PW7", 0 );
	ct_get_gpio_ctrl( "PDR", "PW7");
	ct_set_gpio_ctrl( "PDR", "PJ0", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ0");
	ct_set_gpio_ctrl( "PDR", "PJ0", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ0");
	ct_set_gpio_ctrl( "PDR", "PJ1", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ1");
	ct_set_gpio_ctrl( "PDR", "PJ1", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ1");
	ct_set_gpio_ctrl( "PDR", "PJ2", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ2");
	ct_set_gpio_ctrl( "PDR", "PJ2", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ2");
	ct_set_gpio_ctrl( "PDR", "PJ3", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ3");
	ct_set_gpio_ctrl( "PDR", "PJ3", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ3");
	ct_set_gpio_ctrl( "PDR", "PJ4", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ4");
	ct_set_gpio_ctrl( "PDR", "PJ4", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ4");
	ct_set_gpio_ctrl( "PDR", "PJ5", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ5");
	ct_set_gpio_ctrl( "PDR", "PJ5", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ5");
	ct_set_gpio_ctrl( "PDR", "PJ6", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ6");
	ct_set_gpio_ctrl( "PDR", "PJ6", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ6");
	ct_set_gpio_ctrl( "PDR", "PJ7", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ7");
	ct_set_gpio_ctrl( "PDR", "PJ7", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ7");
	ct_set_gpio_ctrl( "PDR", "PK0", 1 );
	ct_get_gpio_ctrl( "PDR", "PK0");
	ct_set_gpio_ctrl( "PDR", "PK0", 0 );
	ct_get_gpio_ctrl( "PDR", "PK0");
	ct_set_gpio_ctrl( "PDR", "PK1", 1 );
	ct_get_gpio_ctrl( "PDR", "PK1");
	ct_set_gpio_ctrl( "PDR", "PK1", 0 );
	ct_get_gpio_ctrl( "PDR", "PK1");
	ct_set_gpio_ctrl( "PDR", "PK2", 1 );
	ct_get_gpio_ctrl( "PDR", "PK2");
	ct_set_gpio_ctrl( "PDR", "PK2", 0 );
	ct_get_gpio_ctrl( "PDR", "PK2");
	ct_set_gpio_ctrl( "PDR", "PK3", 1 );
	ct_get_gpio_ctrl( "PDR", "PK3");
	ct_set_gpio_ctrl( "PDR", "PK3", 0 );
	ct_get_gpio_ctrl( "PDR", "PK3");
	ct_set_gpio_ctrl( "PDR", "PK4", 1 );
	ct_get_gpio_ctrl( "PDR", "PK4");
	ct_set_gpio_ctrl( "PDR", "PK4", 0 );
	ct_get_gpio_ctrl( "PDR", "PK4");
	ct_set_gpio_ctrl( "PDR", "PK5", 1 );
	ct_get_gpio_ctrl( "PDR", "PK5");
	ct_set_gpio_ctrl( "PDR", "PK5", 0 );
	ct_get_gpio_ctrl( "PDR", "PK5");
	ct_set_gpio_ctrl( "PDR", "PK6", 1 );
	ct_get_gpio_ctrl( "PDR", "PK6");
	ct_set_gpio_ctrl( "PDR", "PK6", 0 );
	ct_get_gpio_ctrl( "PDR", "PK6");
	ct_set_gpio_ctrl( "PDR", "PK7", 1 );
	ct_get_gpio_ctrl( "PDR", "PK7");
	ct_set_gpio_ctrl( "PDR", "PK7", 0 );
	ct_get_gpio_ctrl( "PDR", "PK7");
	ct_set_gpio_ctrl( "PDR", "PL0", 1 );
	ct_get_gpio_ctrl( "PDR", "PL0");
	ct_set_gpio_ctrl( "PDR", "PL0", 0 );
	ct_get_gpio_ctrl( "PDR", "PL0");
	ct_set_gpio_ctrl( "PDR", "PL1", 1 );
	ct_get_gpio_ctrl( "PDR", "PL1");
	ct_set_gpio_ctrl( "PDR", "PL1", 0 );
	ct_get_gpio_ctrl( "PDR", "PL1");
	ct_set_gpio_ctrl( "PDR", "PL2", 1 );
	ct_get_gpio_ctrl( "PDR", "PL2");
	ct_set_gpio_ctrl( "PDR", "PL2", 0 );
	ct_get_gpio_ctrl( "PDR", "PL2");
	ct_set_gpio_ctrl( "PDR", "PL3", 1 );
	ct_get_gpio_ctrl( "PDR", "PL3");
	ct_set_gpio_ctrl( "PDR", "PL3", 0 );
	ct_get_gpio_ctrl( "PDR", "PL3");
	ct_set_gpio_ctrl( "PDR", "PL4", 1 );
	ct_get_gpio_ctrl( "PDR", "PL4");
	ct_set_gpio_ctrl( "PDR", "PL4", 0 );
	ct_get_gpio_ctrl( "PDR", "PL4");
	ct_set_gpio_ctrl( "PDR", "PL5", 1 );
	ct_get_gpio_ctrl( "PDR", "PL5");
	ct_set_gpio_ctrl( "PDR", "PL5", 0 );
	ct_get_gpio_ctrl( "PDR", "PL5");
	ct_set_gpio_ctrl( "PDR", "PM0", 1 );
	ct_get_gpio_ctrl( "PDR", "PM0");
	ct_set_gpio_ctrl( "PDR", "PM0", 0 );
	ct_get_gpio_ctrl( "PDR", "PM0");
	ct_set_gpio_ctrl( "PDR", "PM1", 1 );
	ct_get_gpio_ctrl( "PDR", "PM1");
	ct_set_gpio_ctrl( "PDR", "PM1", 0 );
	ct_get_gpio_ctrl( "PDR", "PM1");
	ct_set_gpio_ctrl( "PDR", "PM2", 1 );
	ct_get_gpio_ctrl( "PDR", "PM2");
	ct_set_gpio_ctrl( "PDR", "PM2", 0 );
	ct_get_gpio_ctrl( "PDR", "PM2");
	ct_set_gpio_ctrl( "PDR", "PM3", 1 );
	ct_get_gpio_ctrl( "PDR", "PM3");
	ct_set_gpio_ctrl( "PDR", "PM3", 0 );
	ct_get_gpio_ctrl( "PDR", "PM3");
	ct_set_gpio_ctrl( "PDR", "PM4", 1 );
	ct_get_gpio_ctrl( "PDR", "PM4");
	ct_set_gpio_ctrl( "PDR", "PM4", 0 );
	ct_get_gpio_ctrl( "PDR", "PM4");
	ct_set_gpio_ctrl( "PDR", "PN0", 1 );
	ct_get_gpio_ctrl( "PDR", "PN0");
	ct_set_gpio_ctrl( "PDR", "PN0", 0 );
	ct_get_gpio_ctrl( "PDR", "PN0");
	ct_set_gpio_ctrl( "PDR", "PN1", 1 );
	ct_get_gpio_ctrl( "PDR", "PN1");
	ct_set_gpio_ctrl( "PDR", "PN1", 0 );
	ct_get_gpio_ctrl( "PDR", "PN1");
	ct_set_gpio_ctrl( "PDR", "PN2", 1 );
	ct_get_gpio_ctrl( "PDR", "PN2");
	ct_set_gpio_ctrl( "PDR", "PN2", 0 );
	ct_get_gpio_ctrl( "PDR", "PN2");
	ct_set_gpio_ctrl( "PDR", "PN3", 1 );
	ct_get_gpio_ctrl( "PDR", "PN3");
	ct_set_gpio_ctrl( "PDR", "PN3", 0 );
	ct_get_gpio_ctrl( "PDR", "PN3");
	ct_set_gpio_ctrl( "PDR", "PN4", 1 );
	ct_get_gpio_ctrl( "PDR", "PN4");
	ct_set_gpio_ctrl( "PDR", "PN4", 0 );
	ct_get_gpio_ctrl( "PDR", "PN4");
	ct_set_gpio_ctrl( "PDR", "PY0", 1 );
	ct_get_gpio_ctrl( "PDR", "PY0");
	ct_set_gpio_ctrl( "PDR", "PY0", 0 );
	ct_get_gpio_ctrl( "PDR", "PY0");
	ct_set_gpio_ctrl( "PDR", "PY1", 1 );
	ct_get_gpio_ctrl( "PDR", "PY1");
	ct_set_gpio_ctrl( "PDR", "PY1", 0 );
	ct_get_gpio_ctrl( "PDR", "PY1");
	ct_set_gpio_ctrl( "PDR", "PY2", 1 );
	ct_get_gpio_ctrl( "PDR", "PY2");
	ct_set_gpio_ctrl( "PDR", "PY2", 0 );
	ct_get_gpio_ctrl( "PDR", "PY2");
	ct_set_gpio_ctrl( "PDR", "PY3", 1 );
	ct_get_gpio_ctrl( "PDR", "PY3");
	ct_set_gpio_ctrl( "PDR", "PY3", 0 );
	ct_get_gpio_ctrl( "PDR", "PY3");
	ct_set_gpio_ctrl( "PDR", "PY4", 1 );
	ct_get_gpio_ctrl( "PDR", "PY4");
	ct_set_gpio_ctrl( "PDR", "PY4", 0 );
	ct_get_gpio_ctrl( "PDR", "PY4");
	ct_set_gpio_ctrl( "PDR", "PY5", 1 );
	ct_get_gpio_ctrl( "PDR", "PY5");
	ct_set_gpio_ctrl( "PDR", "PY5", 0 );
	ct_get_gpio_ctrl( "PDR", "PY5");
	ct_set_gpio_ctrl( "PDR", "PY6", 1 );
	ct_get_gpio_ctrl( "PDR", "PY6");
	ct_set_gpio_ctrl( "PDR", "PY6", 0 );
	ct_get_gpio_ctrl( "PDR", "PY6");
	ct_set_gpio_ctrl( "PDR", "PY7", 1 );
	ct_get_gpio_ctrl( "PDR", "PY7");
	ct_set_gpio_ctrl( "PDR", "PY7", 0 );
	ct_get_gpio_ctrl( "PDR", "PY7");
	ct_set_gpio_ctrl( "PDR", "PP0", 1 );
	ct_get_gpio_ctrl( "PDR", "PP0");
	ct_set_gpio_ctrl( "PDR", "PP0", 0 );
	ct_get_gpio_ctrl( "PDR", "PP0");
	ct_set_gpio_ctrl( "PDR", "PP1", 1 );
	ct_get_gpio_ctrl( "PDR", "PP1");
	ct_set_gpio_ctrl( "PDR", "PP1", 0 );
	ct_get_gpio_ctrl( "PDR", "PP1");
	ct_set_gpio_ctrl( "PDR", "PP2", 1 );
	ct_get_gpio_ctrl( "PDR", "PP2");
	ct_set_gpio_ctrl( "PDR", "PP2", 0 );
	ct_get_gpio_ctrl( "PDR", "PP2");
	ct_set_gpio_ctrl( "PDR", "PP3", 1 );
	ct_get_gpio_ctrl( "PDR", "PP3");
	ct_set_gpio_ctrl( "PDR", "PP3", 0 );
	ct_get_gpio_ctrl( "PDR", "PP3");
}


static void ct_gpio_test( int argc, char** argv )
{
	/* EPCR */
	ct_set_gpio_ctrl( "EPCR", "P60", 1 );
	ct_get_gpio_ctrl( "EPCR", "P60");
	ct_set_gpio_ctrl( "EPCR", "P60", 0 );
	ct_get_gpio_ctrl( "EPCR", "P60");
	ct_set_gpio_ctrl( "EPCR", "P61", 1 );
	ct_get_gpio_ctrl( "EPCR", "P61");
	ct_set_gpio_ctrl( "EPCR", "P61", 0 );
	ct_get_gpio_ctrl( "EPCR", "P61");
	ct_set_gpio_ctrl( "EPCR", "P62", 1 );
	ct_get_gpio_ctrl( "EPCR", "P62");
	ct_set_gpio_ctrl( "EPCR", "P62", 0 );
	ct_get_gpio_ctrl( "EPCR", "P62");
	ct_set_gpio_ctrl( "EPCR", "P63", 1 );
	ct_get_gpio_ctrl( "EPCR", "P63");
	ct_set_gpio_ctrl( "EPCR", "P63", 0 );
	ct_get_gpio_ctrl( "EPCR", "P63");
	ct_set_gpio_ctrl( "EPCR", "P80", 1 );
	ct_get_gpio_ctrl( "EPCR", "P80");
	ct_set_gpio_ctrl( "EPCR", "P80", 0 );
	ct_get_gpio_ctrl( "EPCR", "P80");
	ct_set_gpio_ctrl( "EPCR", "P81", 1 );
	ct_get_gpio_ctrl( "EPCR", "P81");
	ct_set_gpio_ctrl( "EPCR", "P81", 0 );
	ct_get_gpio_ctrl( "EPCR", "P81");
	ct_set_gpio_ctrl( "EPCR", "P82", 1 );
	ct_get_gpio_ctrl( "EPCR", "P82");
	ct_set_gpio_ctrl( "EPCR", "P82", 0 );
	ct_get_gpio_ctrl( "EPCR", "P82");
	ct_set_gpio_ctrl( "EPCR", "P83", 1 );
	ct_get_gpio_ctrl( "EPCR", "P83");
	ct_set_gpio_ctrl( "EPCR", "P83", 0 );
	ct_get_gpio_ctrl( "EPCR", "P83");
	ct_set_gpio_ctrl( "EPCR", "P84", 1 );
	ct_get_gpio_ctrl( "EPCR", "P84");
	ct_set_gpio_ctrl( "EPCR", "P84", 0 );
	ct_get_gpio_ctrl( "EPCR", "P84");
	ct_set_gpio_ctrl( "EPCR", "P85", 1 );
	ct_get_gpio_ctrl( "EPCR", "P85");
	ct_set_gpio_ctrl( "EPCR", "P85", 0 );
	ct_get_gpio_ctrl( "EPCR", "P85");
	ct_set_gpio_ctrl( "EPCR", "P86", 1 );
	ct_get_gpio_ctrl( "EPCR", "P86");
	ct_set_gpio_ctrl( "EPCR", "P86", 0 );
	ct_get_gpio_ctrl( "EPCR", "P86");
	ct_set_gpio_ctrl( "EPCR", "P87", 1 );
	ct_get_gpio_ctrl( "EPCR", "P87");
	ct_set_gpio_ctrl( "EPCR", "P87", 0 );
	ct_get_gpio_ctrl( "EPCR", "P87");
	ct_set_gpio_ctrl( "EPCR", "P90", 1 );
	ct_get_gpio_ctrl( "EPCR", "P90");
	ct_set_gpio_ctrl( "EPCR", "P90", 0 );
	ct_get_gpio_ctrl( "EPCR", "P90");
	ct_set_gpio_ctrl( "EPCR", "P92", 1 );
	ct_get_gpio_ctrl( "EPCR", "P92");
	ct_set_gpio_ctrl( "EPCR", "P92", 0 );
	ct_get_gpio_ctrl( "EPCR", "P92");
	ct_set_gpio_ctrl( "EPCR", "PA0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PA0");
	ct_set_gpio_ctrl( "EPCR", "PA0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PA0");
	ct_set_gpio_ctrl( "EPCR", "PA1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PA1");
	ct_set_gpio_ctrl( "EPCR", "PA1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PA1");
	ct_set_gpio_ctrl( "EPCR", "PA3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PA3");
	ct_set_gpio_ctrl( "EPCR", "PA3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PA3");
	ct_set_gpio_ctrl( "EPCR", "PA5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PA5");
	ct_set_gpio_ctrl( "EPCR", "PA5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PA5");
	ct_set_gpio_ctrl( "EPCR", "PA7", 1 );
	ct_get_gpio_ctrl( "EPCR", "PA7");
	ct_set_gpio_ctrl( "EPCR", "PA7", 0 );
	ct_get_gpio_ctrl( "EPCR", "PA7");
	ct_set_gpio_ctrl( "EPCR", "PB0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PB0");
	ct_set_gpio_ctrl( "EPCR", "PB0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PB0");
	ct_set_gpio_ctrl( "EPCR", "PB1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PB1");
	ct_set_gpio_ctrl( "EPCR", "PB1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PB1");
	ct_set_gpio_ctrl( "EPCR", "PB3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PB3");
	ct_set_gpio_ctrl( "EPCR", "PB3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PB3");
	ct_set_gpio_ctrl( "EPCR", "PB4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PB4");
	ct_set_gpio_ctrl( "EPCR", "PB4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PB4");
	ct_set_gpio_ctrl( "EPCR", "PC0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PC0");
	ct_set_gpio_ctrl( "EPCR", "PC0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PC0");
	ct_set_gpio_ctrl( "EPCR", "PC1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PC1");
	ct_set_gpio_ctrl( "EPCR", "PC1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PC1");
	ct_set_gpio_ctrl( "EPCR", "PC2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PC2");
	ct_set_gpio_ctrl( "EPCR", "PC2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PC2");
	ct_set_gpio_ctrl( "EPCR", "PC3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PC3");
	ct_set_gpio_ctrl( "EPCR", "PC3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PC3");
	ct_set_gpio_ctrl( "EPCR", "PC4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PC4");
	ct_set_gpio_ctrl( "EPCR", "PC4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PC4");
	ct_set_gpio_ctrl( "EPCR", "PC5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PC5");
	ct_set_gpio_ctrl( "EPCR", "PC5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PC5");
	ct_set_gpio_ctrl( "EPCR", "PD1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PD1");
	ct_set_gpio_ctrl( "EPCR", "PD1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PD1");
	ct_set_gpio_ctrl( "EPCR", "PD2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PD2");
	ct_set_gpio_ctrl( "EPCR", "PD2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PD2");
	ct_set_gpio_ctrl( "EPCR", "PD3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PD3");
	ct_set_gpio_ctrl( "EPCR", "PD3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PD3");
	ct_set_gpio_ctrl( "EPCR", "PD4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PD4");
	ct_set_gpio_ctrl( "EPCR", "PD4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PD4");
	ct_set_gpio_ctrl( "EPCR", "PD5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PD5");
	ct_set_gpio_ctrl( "EPCR", "PD5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PD5");
	ct_set_gpio_ctrl( "EPCR", "PD6", 1 );
	ct_get_gpio_ctrl( "EPCR", "PD6");
	ct_set_gpio_ctrl( "EPCR", "PD6", 0 );
	ct_get_gpio_ctrl( "EPCR", "PD6");
//	ct_set_gpio_ctrl( "EPCR", "PE1", 1 );
//	ct_get_gpio_ctrl( "EPCR", "PE1");
//	ct_set_gpio_ctrl( "EPCR", "PE1", 0 );
//	ct_get_gpio_ctrl( "EPCR", "PE1");
//	ct_set_gpio_ctrl( "EPCR", "PE3", 1 );
//	ct_get_gpio_ctrl( "EPCR", "PE3");
//	ct_set_gpio_ctrl( "EPCR", "PE3", 0 );
//	ct_get_gpio_ctrl( "EPCR", "PE3");
//	ct_set_gpio_ctrl( "EPCR", "PE5", 1 );
//	ct_get_gpio_ctrl( "EPCR", "PE5");
//	ct_set_gpio_ctrl( "EPCR", "PE5", 0 );
//	ct_get_gpio_ctrl( "EPCR", "PE5");
//	ct_set_gpio_ctrl( "EPCR", "PF4", 1 );
//	ct_get_gpio_ctrl( "EPCR", "PF4");
//	ct_set_gpio_ctrl( "EPCR", "PF4", 0 );
//	ct_get_gpio_ctrl( "EPCR", "PF4");
	ct_set_gpio_ctrl( "EPCR", "PG0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PG0");
	ct_set_gpio_ctrl( "EPCR", "PG0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PG0");
	ct_set_gpio_ctrl( "EPCR", "PG1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PG1");
	ct_set_gpio_ctrl( "EPCR", "PG1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PG1");
	ct_set_gpio_ctrl( "EPCR", "PG2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PG2");
	ct_set_gpio_ctrl( "EPCR", "PG2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PG2");
	ct_set_gpio_ctrl( "EPCR", "PG3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PG3");
	ct_set_gpio_ctrl( "EPCR", "PG3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PG3");
	ct_set_gpio_ctrl( "EPCR", "PG4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PG4");
	ct_set_gpio_ctrl( "EPCR", "PG4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PG4");
	ct_set_gpio_ctrl( "EPCR", "PG5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PG5");
	ct_set_gpio_ctrl( "EPCR", "PG5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PG5");
	ct_set_gpio_ctrl( "EPCR", "PG6", 1 );
	ct_get_gpio_ctrl( "EPCR", "PG6");
	ct_set_gpio_ctrl( "EPCR", "PG6", 0 );
	ct_get_gpio_ctrl( "EPCR", "PG6");
	ct_set_gpio_ctrl( "EPCR", "PG7", 1 );
	ct_get_gpio_ctrl( "EPCR", "PG7");
	ct_set_gpio_ctrl( "EPCR", "PG7", 0 );
	ct_get_gpio_ctrl( "EPCR", "PG7");
	ct_set_gpio_ctrl( "EPCR", "PH0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PH0");
	ct_set_gpio_ctrl( "EPCR", "PH0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PH0");
	ct_set_gpio_ctrl( "EPCR", "PH1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PH1");
	ct_set_gpio_ctrl( "EPCR", "PH1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PH1");
	ct_set_gpio_ctrl( "EPCR", "PH2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PH2");
	ct_set_gpio_ctrl( "EPCR", "PH2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PH2");
	ct_set_gpio_ctrl( "EPCR", "PH3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PH3");
	ct_set_gpio_ctrl( "EPCR", "PH3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PH3");
	ct_set_gpio_ctrl( "EPCR", "PW0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PW0");
	ct_set_gpio_ctrl( "EPCR", "PW0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PW0");
	ct_set_gpio_ctrl( "EPCR", "PW1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PW1");
	ct_set_gpio_ctrl( "EPCR", "PW1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PW1");
	ct_set_gpio_ctrl( "EPCR", "PW2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PW2");
	ct_set_gpio_ctrl( "EPCR", "PW2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PW2");
	ct_set_gpio_ctrl( "EPCR", "PW3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PW3");
	ct_set_gpio_ctrl( "EPCR", "PW3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PW3");
	ct_set_gpio_ctrl( "EPCR", "PW4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PW4");
	ct_set_gpio_ctrl( "EPCR", "PW4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PW4");
	ct_set_gpio_ctrl( "EPCR", "PW5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PW5");
	ct_set_gpio_ctrl( "EPCR", "PW5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PW5");
	ct_set_gpio_ctrl( "EPCR", "PW6", 1 );
	ct_get_gpio_ctrl( "EPCR", "PW6");
	ct_set_gpio_ctrl( "EPCR", "PW6", 0 );
	ct_get_gpio_ctrl( "EPCR", "PW6");
	ct_set_gpio_ctrl( "EPCR", "PW7", 1 );
	ct_get_gpio_ctrl( "EPCR", "PW7");
	ct_set_gpio_ctrl( "EPCR", "PW7", 0 );
	ct_get_gpio_ctrl( "EPCR", "PW7");
	ct_set_gpio_ctrl( "EPCR", "PJ0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PJ0");
	ct_set_gpio_ctrl( "EPCR", "PJ0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PJ0");
	ct_set_gpio_ctrl( "EPCR", "PJ1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PJ1");
	ct_set_gpio_ctrl( "EPCR", "PJ1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PJ1");
	ct_set_gpio_ctrl( "EPCR", "PJ2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PJ2");
	ct_set_gpio_ctrl( "EPCR", "PJ2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PJ2");
	ct_set_gpio_ctrl( "EPCR", "PJ3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PJ3");
	ct_set_gpio_ctrl( "EPCR", "PJ3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PJ3");
	ct_set_gpio_ctrl( "EPCR", "PJ4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PJ4");
	ct_set_gpio_ctrl( "EPCR", "PJ4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PJ4");
	ct_set_gpio_ctrl( "EPCR", "PJ5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PJ5");
	ct_set_gpio_ctrl( "EPCR", "PJ5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PJ5");
	ct_set_gpio_ctrl( "EPCR", "PJ6", 1 );
	ct_get_gpio_ctrl( "EPCR", "PJ6");
	ct_set_gpio_ctrl( "EPCR", "PJ6", 0 );
	ct_get_gpio_ctrl( "EPCR", "PJ6");
	ct_set_gpio_ctrl( "EPCR", "PJ7", 1 );
	ct_get_gpio_ctrl( "EPCR", "PJ7");
	ct_set_gpio_ctrl( "EPCR", "PJ7", 0 );
	ct_get_gpio_ctrl( "EPCR", "PJ7");
	ct_set_gpio_ctrl( "EPCR", "PK0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PK0");
	ct_set_gpio_ctrl( "EPCR", "PK0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PK0");
	ct_set_gpio_ctrl( "EPCR", "PK1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PK1");
	ct_set_gpio_ctrl( "EPCR", "PK1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PK1");
	ct_set_gpio_ctrl( "EPCR", "PK2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PK2");
	ct_set_gpio_ctrl( "EPCR", "PK2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PK2");
	ct_set_gpio_ctrl( "EPCR", "PK3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PK3");
	ct_set_gpio_ctrl( "EPCR", "PK3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PK3");
	ct_set_gpio_ctrl( "EPCR", "PK4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PK4");
	ct_set_gpio_ctrl( "EPCR", "PK4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PK4");
	ct_set_gpio_ctrl( "EPCR", "PK5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PK5");
	ct_set_gpio_ctrl( "EPCR", "PK5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PK5");
	ct_set_gpio_ctrl( "EPCR", "PK6", 1 );
	ct_get_gpio_ctrl( "EPCR", "PK6");
	ct_set_gpio_ctrl( "EPCR", "PK6", 0 );
	ct_get_gpio_ctrl( "EPCR", "PK6");
	ct_set_gpio_ctrl( "EPCR", "PK7", 1 );
	ct_get_gpio_ctrl( "EPCR", "PK7");
	ct_set_gpio_ctrl( "EPCR", "PK7", 0 );
	ct_get_gpio_ctrl( "EPCR", "PK7");
	ct_set_gpio_ctrl( "EPCR", "PL0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PL0");
	ct_set_gpio_ctrl( "EPCR", "PL0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PL0");
	ct_set_gpio_ctrl( "EPCR", "PL1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PL1");
	ct_set_gpio_ctrl( "EPCR", "PL1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PL1");
	ct_set_gpio_ctrl( "EPCR", "PL2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PL2");
	ct_set_gpio_ctrl( "EPCR", "PL2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PL2");
	ct_set_gpio_ctrl( "EPCR", "PL3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PL3");
	ct_set_gpio_ctrl( "EPCR", "PL3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PL3");
	ct_set_gpio_ctrl( "EPCR", "PL4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PL4");
	ct_set_gpio_ctrl( "EPCR", "PL4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PL4");
	ct_set_gpio_ctrl( "EPCR", "PL5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PL5");
	ct_set_gpio_ctrl( "EPCR", "PL5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PL5");
	ct_set_gpio_ctrl( "EPCR", "PM1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PM1");
	ct_set_gpio_ctrl( "EPCR", "PM1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PM1");
	ct_set_gpio_ctrl( "EPCR", "PM3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PM3");
	ct_set_gpio_ctrl( "EPCR", "PM3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PM3");
	ct_set_gpio_ctrl( "EPCR", "PM4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PM4");
	ct_set_gpio_ctrl( "EPCR", "PM4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PM4");
	ct_set_gpio_ctrl( "EPCR", "PN1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PN1");
	ct_set_gpio_ctrl( "EPCR", "PN1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PN1");
	ct_set_gpio_ctrl( "EPCR", "PN4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PN4");
	ct_set_gpio_ctrl( "EPCR", "PN4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PN4");
	ct_set_gpio_ctrl( "EPCR", "PY0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PY0");
	ct_set_gpio_ctrl( "EPCR", "PY0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PY0");
	ct_set_gpio_ctrl( "EPCR", "PY1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PY1");
	ct_set_gpio_ctrl( "EPCR", "PY1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PY1");
	ct_set_gpio_ctrl( "EPCR", "PY4", 1 );
	ct_get_gpio_ctrl( "EPCR", "PY4");
	ct_set_gpio_ctrl( "EPCR", "PY4", 0 );
	ct_get_gpio_ctrl( "EPCR", "PY4");
	ct_set_gpio_ctrl( "EPCR", "PY5", 1 );
	ct_get_gpio_ctrl( "EPCR", "PY5");
	ct_set_gpio_ctrl( "EPCR", "PY5", 0 );
	ct_get_gpio_ctrl( "EPCR", "PY5");
	ct_set_gpio_ctrl( "EPCR", "PY6", 1 );
	ct_get_gpio_ctrl( "EPCR", "PY6");
	ct_set_gpio_ctrl( "EPCR", "PY6", 0 );
	ct_get_gpio_ctrl( "EPCR", "PY6");
	ct_set_gpio_ctrl( "EPCR", "PY7", 1 );
	ct_get_gpio_ctrl( "EPCR", "PY7");
	ct_set_gpio_ctrl( "EPCR", "PY7", 0 );
	ct_get_gpio_ctrl( "EPCR", "PY7");
	ct_set_gpio_ctrl( "EPCR", "PP0", 1 );
	ct_get_gpio_ctrl( "EPCR", "PP0");
	ct_set_gpio_ctrl( "EPCR", "PP0", 0 );
	ct_get_gpio_ctrl( "EPCR", "PP0");
	ct_set_gpio_ctrl( "EPCR", "PP1", 1 );
	ct_get_gpio_ctrl( "EPCR", "PP1");
	ct_set_gpio_ctrl( "EPCR", "PP1", 0 );
	ct_get_gpio_ctrl( "EPCR", "PP1");
	ct_set_gpio_ctrl( "EPCR", "PP2", 1 );
	ct_get_gpio_ctrl( "EPCR", "PP2");
	ct_set_gpio_ctrl( "EPCR", "PP2", 0 );
	ct_get_gpio_ctrl( "EPCR", "PP2");
	ct_set_gpio_ctrl( "EPCR", "PP3", 1 );
	ct_get_gpio_ctrl( "EPCR", "PP3");
	ct_set_gpio_ctrl( "EPCR", "PP3", 0 );
	ct_get_gpio_ctrl( "EPCR", "PP3");

	/* DDR */
	ct_set_gpio_ctrl( "DDR", "P60", 1 );
	ct_get_gpio_ctrl( "DDR", "P60");
	ct_set_gpio_ctrl( "DDR", "P60", 0 );
	ct_get_gpio_ctrl( "DDR", "P60");
	ct_set_gpio_ctrl( "DDR", "P61", 1 );
	ct_get_gpio_ctrl( "DDR", "P61");
	ct_set_gpio_ctrl( "DDR", "P61", 0 );
	ct_get_gpio_ctrl( "DDR", "P61");
	ct_set_gpio_ctrl( "DDR", "P62", 1 );
	ct_get_gpio_ctrl( "DDR", "P62");
	ct_set_gpio_ctrl( "DDR", "P62", 0 );
	ct_get_gpio_ctrl( "DDR", "P62");
	ct_set_gpio_ctrl( "DDR", "P63", 1 );
	ct_get_gpio_ctrl( "DDR", "P63");
	ct_set_gpio_ctrl( "DDR", "P63", 0 );
	ct_get_gpio_ctrl( "DDR", "P63");
	ct_set_gpio_ctrl( "DDR", "P64", 1 );
	ct_get_gpio_ctrl( "DDR", "P64");
	ct_set_gpio_ctrl( "DDR", "P64", 0 );
	ct_get_gpio_ctrl( "DDR", "P64");
	ct_set_gpio_ctrl( "DDR", "P65", 1 );
	ct_get_gpio_ctrl( "DDR", "P65");
	ct_set_gpio_ctrl( "DDR", "P65", 0 );
	ct_get_gpio_ctrl( "DDR", "P65");
	ct_set_gpio_ctrl( "DDR", "P66", 1 );
	ct_get_gpio_ctrl( "DDR", "P66");
	ct_set_gpio_ctrl( "DDR", "P66", 0 );
	ct_get_gpio_ctrl( "DDR", "P66");
	ct_set_gpio_ctrl( "DDR", "P67", 1 );
	ct_get_gpio_ctrl( "DDR", "P67");
	ct_set_gpio_ctrl( "DDR", "P67", 0 );
	ct_get_gpio_ctrl( "DDR", "P67");
	ct_set_gpio_ctrl( "DDR", "P70", 1 );
	ct_get_gpio_ctrl( "DDR", "P70");
	ct_set_gpio_ctrl( "DDR", "P70", 0 );
	ct_get_gpio_ctrl( "DDR", "P70");
	ct_set_gpio_ctrl( "DDR", "P71", 1 );
	ct_get_gpio_ctrl( "DDR", "P71");
	ct_set_gpio_ctrl( "DDR", "P71", 0 );
	ct_get_gpio_ctrl( "DDR", "P71");
	ct_set_gpio_ctrl( "DDR", "P72", 1 );
	ct_get_gpio_ctrl( "DDR", "P72");
	ct_set_gpio_ctrl( "DDR", "P72", 0 );
	ct_get_gpio_ctrl( "DDR", "P72");
	ct_set_gpio_ctrl( "DDR", "P73", 1 );
	ct_get_gpio_ctrl( "DDR", "P73");
	ct_set_gpio_ctrl( "DDR", "P73", 0 );
	ct_get_gpio_ctrl( "DDR", "P73");
	ct_set_gpio_ctrl( "DDR", "P74", 1 );
	ct_get_gpio_ctrl( "DDR", "P74");
	ct_set_gpio_ctrl( "DDR", "P74", 0 );
	ct_get_gpio_ctrl( "DDR", "P74");
	ct_set_gpio_ctrl( "DDR", "P75", 1 );
	ct_get_gpio_ctrl( "DDR", "P75");
	ct_set_gpio_ctrl( "DDR", "P75", 0 );
	ct_get_gpio_ctrl( "DDR", "P75");
	ct_set_gpio_ctrl( "DDR", "P76", 1 );
	ct_get_gpio_ctrl( "DDR", "P76");
	ct_set_gpio_ctrl( "DDR", "P76", 0 );
	ct_get_gpio_ctrl( "DDR", "P76");
	ct_set_gpio_ctrl( "DDR", "P77", 1 );
	ct_get_gpio_ctrl( "DDR", "P77");
	ct_set_gpio_ctrl( "DDR", "P77", 0 );
	ct_get_gpio_ctrl( "DDR", "P77");
	ct_set_gpio_ctrl( "DDR", "P80", 1 );
	ct_get_gpio_ctrl( "DDR", "P80");
	ct_set_gpio_ctrl( "DDR", "P80", 0 );
	ct_get_gpio_ctrl( "DDR", "P80");
	ct_set_gpio_ctrl( "DDR", "P81", 1 );
	ct_get_gpio_ctrl( "DDR", "P81");
	ct_set_gpio_ctrl( "DDR", "P81", 0 );
	ct_get_gpio_ctrl( "DDR", "P81");
	ct_set_gpio_ctrl( "DDR", "P82", 1 );
	ct_get_gpio_ctrl( "DDR", "P82");
	ct_set_gpio_ctrl( "DDR", "P82", 0 );
	ct_get_gpio_ctrl( "DDR", "P82");
	ct_set_gpio_ctrl( "DDR", "P83", 1 );
	ct_get_gpio_ctrl( "DDR", "P83");
	ct_set_gpio_ctrl( "DDR", "P83", 0 );
	ct_get_gpio_ctrl( "DDR", "P83");
	ct_set_gpio_ctrl( "DDR", "P84", 1 );
	ct_get_gpio_ctrl( "DDR", "P84");
	ct_set_gpio_ctrl( "DDR", "P84", 0 );
	ct_get_gpio_ctrl( "DDR", "P84");
	ct_set_gpio_ctrl( "DDR", "P85", 1 );
	ct_get_gpio_ctrl( "DDR", "P85");
	ct_set_gpio_ctrl( "DDR", "P85", 0 );
	ct_get_gpio_ctrl( "DDR", "P85");
	ct_set_gpio_ctrl( "DDR", "P86", 1 );
	ct_get_gpio_ctrl( "DDR", "P86");
	ct_set_gpio_ctrl( "DDR", "P86", 0 );
	ct_get_gpio_ctrl( "DDR", "P86");
	ct_set_gpio_ctrl( "DDR", "P87", 1 );
	ct_get_gpio_ctrl( "DDR", "P87");
	ct_set_gpio_ctrl( "DDR", "P87", 0 );
	ct_get_gpio_ctrl( "DDR", "P87");
	ct_set_gpio_ctrl( "DDR", "P90", 1 );
	ct_get_gpio_ctrl( "DDR", "P90");
	ct_set_gpio_ctrl( "DDR", "P90", 0 );
	ct_get_gpio_ctrl( "DDR", "P90");
	ct_set_gpio_ctrl( "DDR", "P91", 1 );
	ct_get_gpio_ctrl( "DDR", "P91");
	ct_set_gpio_ctrl( "DDR", "P91", 0 );
	ct_get_gpio_ctrl( "DDR", "P91");
	ct_set_gpio_ctrl( "DDR", "P92", 1 );
	ct_get_gpio_ctrl( "DDR", "P92");
	ct_set_gpio_ctrl( "DDR", "P92", 0 );
	ct_get_gpio_ctrl( "DDR", "P92");
	ct_set_gpio_ctrl( "DDR", "P93", 1 );
	ct_get_gpio_ctrl( "DDR", "P93");
	ct_set_gpio_ctrl( "DDR", "P93", 0 );
	ct_get_gpio_ctrl( "DDR", "P93");
	ct_set_gpio_ctrl( "DDR", "PA0", 1 );
	ct_get_gpio_ctrl( "DDR", "PA0");
	ct_set_gpio_ctrl( "DDR", "PA0", 0 );
	ct_get_gpio_ctrl( "DDR", "PA0");
	ct_set_gpio_ctrl( "DDR", "PA1", 1 );
	ct_get_gpio_ctrl( "DDR", "PA1");
	ct_set_gpio_ctrl( "DDR", "PA1", 0 );
	ct_get_gpio_ctrl( "DDR", "PA1");
	ct_set_gpio_ctrl( "DDR", "PA2", 1 );
	ct_get_gpio_ctrl( "DDR", "PA2");
	ct_set_gpio_ctrl( "DDR", "PA2", 0 );
	ct_get_gpio_ctrl( "DDR", "PA2");
	ct_set_gpio_ctrl( "DDR", "PA3", 1 );
	ct_get_gpio_ctrl( "DDR", "PA3");
	ct_set_gpio_ctrl( "DDR", "PA3", 0 );
	ct_get_gpio_ctrl( "DDR", "PA3");
	ct_set_gpio_ctrl( "DDR", "PA4", 1 );
	ct_get_gpio_ctrl( "DDR", "PA4");
	ct_set_gpio_ctrl( "DDR", "PA4", 0 );
	ct_get_gpio_ctrl( "DDR", "PA4");
	ct_set_gpio_ctrl( "DDR", "PA5", 1 );
	ct_get_gpio_ctrl( "DDR", "PA5");
	ct_set_gpio_ctrl( "DDR", "PA5", 0 );
	ct_get_gpio_ctrl( "DDR", "PA5");
	ct_set_gpio_ctrl( "DDR", "PA6", 1 );
	ct_get_gpio_ctrl( "DDR", "PA6");
	ct_set_gpio_ctrl( "DDR", "PA6", 0 );
	ct_get_gpio_ctrl( "DDR", "PA6");
	ct_set_gpio_ctrl( "DDR", "PA7", 1 );
	ct_get_gpio_ctrl( "DDR", "PA7");
	ct_set_gpio_ctrl( "DDR", "PA7", 0 );
	ct_get_gpio_ctrl( "DDR", "PA7");
	ct_set_gpio_ctrl( "DDR", "PB0", 1 );
	ct_get_gpio_ctrl( "DDR", "PB0");
	ct_set_gpio_ctrl( "DDR", "PB0", 0 );
	ct_get_gpio_ctrl( "DDR", "PB0");
	ct_set_gpio_ctrl( "DDR", "PB1", 1 );
	ct_get_gpio_ctrl( "DDR", "PB1");
	ct_set_gpio_ctrl( "DDR", "PB1", 0 );
	ct_get_gpio_ctrl( "DDR", "PB1");
	ct_set_gpio_ctrl( "DDR", "PB2", 1 );
	ct_get_gpio_ctrl( "DDR", "PB2");
	ct_set_gpio_ctrl( "DDR", "PB2", 0 );
	ct_get_gpio_ctrl( "DDR", "PB2");
	ct_set_gpio_ctrl( "DDR", "PB3", 1 );
	ct_get_gpio_ctrl( "DDR", "PB3");
	ct_set_gpio_ctrl( "DDR", "PB3", 0 );
	ct_get_gpio_ctrl( "DDR", "PB3");
	ct_set_gpio_ctrl( "DDR", "PB4", 1 );
	ct_get_gpio_ctrl( "DDR", "PB4");
	ct_set_gpio_ctrl( "DDR", "PB4", 0 );
	ct_get_gpio_ctrl( "DDR", "PB4");
	ct_set_gpio_ctrl( "DDR", "PC0", 1 );
	ct_get_gpio_ctrl( "DDR", "PC0");
	ct_set_gpio_ctrl( "DDR", "PC0", 0 );
	ct_get_gpio_ctrl( "DDR", "PC0");
	ct_set_gpio_ctrl( "DDR", "PC1", 1 );
	ct_get_gpio_ctrl( "DDR", "PC1");
	ct_set_gpio_ctrl( "DDR", "PC1", 0 );
	ct_get_gpio_ctrl( "DDR", "PC1");
	ct_set_gpio_ctrl( "DDR", "PC2", 1 );
	ct_get_gpio_ctrl( "DDR", "PC2");
	ct_set_gpio_ctrl( "DDR", "PC2", 0 );
	ct_get_gpio_ctrl( "DDR", "PC2");
	ct_set_gpio_ctrl( "DDR", "PC3", 1 );
	ct_get_gpio_ctrl( "DDR", "PC3");
	ct_set_gpio_ctrl( "DDR", "PC3", 0 );
	ct_get_gpio_ctrl( "DDR", "PC3");
	ct_set_gpio_ctrl( "DDR", "PC4", 1 );
	ct_get_gpio_ctrl( "DDR", "PC4");
	ct_set_gpio_ctrl( "DDR", "PC4", 0 );
	ct_get_gpio_ctrl( "DDR", "PC4");
	ct_set_gpio_ctrl( "DDR", "PC5", 1 );
	ct_get_gpio_ctrl( "DDR", "PC5");
	ct_set_gpio_ctrl( "DDR", "PC5", 0 );
	ct_get_gpio_ctrl( "DDR", "PC5");
	ct_set_gpio_ctrl( "DDR", "PD0", 1 );
	ct_get_gpio_ctrl( "DDR", "PD0");
	ct_set_gpio_ctrl( "DDR", "PD0", 0 );
	ct_get_gpio_ctrl( "DDR", "PD0");
	ct_set_gpio_ctrl( "DDR", "PD1", 1 );
	ct_get_gpio_ctrl( "DDR", "PD1");
	ct_set_gpio_ctrl( "DDR", "PD1", 0 );
	ct_get_gpio_ctrl( "DDR", "PD1");
	ct_set_gpio_ctrl( "DDR", "PD2", 1 );
	ct_get_gpio_ctrl( "DDR", "PD2");
	ct_set_gpio_ctrl( "DDR", "PD2", 0 );
	ct_get_gpio_ctrl( "DDR", "PD2");
	ct_set_gpio_ctrl( "DDR", "PD3", 1 );
	ct_get_gpio_ctrl( "DDR", "PD3");
	ct_set_gpio_ctrl( "DDR", "PD3", 0 );
	ct_get_gpio_ctrl( "DDR", "PD3");
	ct_set_gpio_ctrl( "DDR", "PD4", 1 );
	ct_get_gpio_ctrl( "DDR", "PD4");
	ct_set_gpio_ctrl( "DDR", "PD4", 0 );
	ct_get_gpio_ctrl( "DDR", "PD4");
	ct_set_gpio_ctrl( "DDR", "PD5", 1 );
	ct_get_gpio_ctrl( "DDR", "PD5");
	ct_set_gpio_ctrl( "DDR", "PD5", 0 );
	ct_get_gpio_ctrl( "DDR", "PD5");
	ct_set_gpio_ctrl( "DDR", "PD6", 1 );
	ct_get_gpio_ctrl( "DDR", "PD6");
	ct_set_gpio_ctrl( "DDR", "PD6", 0 );
	ct_get_gpio_ctrl( "DDR", "PD6");
//	ct_set_gpio_ctrl( "DDR", "PE0", 1 );
//	ct_get_gpio_ctrl( "DDR", "PE0");
//	ct_set_gpio_ctrl( "DDR", "PE0", 0 );
//	ct_get_gpio_ctrl( "DDR", "PE0");
//	ct_set_gpio_ctrl( "DDR", "PE1", 1 );
//	ct_get_gpio_ctrl( "DDR", "PE1");
//	ct_set_gpio_ctrl( "DDR", "PE1", 0 );
//	ct_get_gpio_ctrl( "DDR", "PE1");
//	ct_set_gpio_ctrl( "DDR", "PE2", 1 );
//	ct_get_gpio_ctrl( "DDR", "PE2");
//	ct_set_gpio_ctrl( "DDR", "PE2", 0 );
//	ct_get_gpio_ctrl( "DDR", "PE2");
//	ct_set_gpio_ctrl( "DDR", "PE3", 1 );
//	ct_get_gpio_ctrl( "DDR", "PE3");
//	ct_set_gpio_ctrl( "DDR", "PE3", 0 );
//	ct_get_gpio_ctrl( "DDR", "PE3");
//	ct_set_gpio_ctrl( "DDR", "PE4", 1 );
//	ct_get_gpio_ctrl( "DDR", "PE4");
//	ct_set_gpio_ctrl( "DDR", "PE4", 0 );
//	ct_get_gpio_ctrl( "DDR", "PE4");
//	ct_set_gpio_ctrl( "DDR", "PE5", 1 );
//	ct_get_gpio_ctrl( "DDR", "PE5");
//	ct_set_gpio_ctrl( "DDR", "PE5", 0 );
//	ct_get_gpio_ctrl( "DDR", "PE5");
//	ct_set_gpio_ctrl( "DDR", "PF0", 1 );
//	ct_get_gpio_ctrl( "DDR", "PF0");
//	ct_set_gpio_ctrl( "DDR", "PF0", 0 );
//	ct_get_gpio_ctrl( "DDR", "PF0");
//	ct_set_gpio_ctrl( "DDR", "PF1", 1 );
//	ct_get_gpio_ctrl( "DDR", "PF1");
//	ct_set_gpio_ctrl( "DDR", "PF1", 0 );
//	ct_get_gpio_ctrl( "DDR", "PF1");
//	ct_set_gpio_ctrl( "DDR", "PF2", 1 );
//	ct_get_gpio_ctrl( "DDR", "PF2");
//	ct_set_gpio_ctrl( "DDR", "PF2", 0 );
//	ct_get_gpio_ctrl( "DDR", "PF2");
//	ct_set_gpio_ctrl( "DDR", "PF3", 1 );
//	ct_get_gpio_ctrl( "DDR", "PF3");
//	ct_set_gpio_ctrl( "DDR", "PF3", 0 );
//	ct_get_gpio_ctrl( "DDR", "PF3");
//	ct_set_gpio_ctrl( "DDR", "PF4", 1 );
//	ct_get_gpio_ctrl( "DDR", "PF4");
//	ct_set_gpio_ctrl( "DDR", "PF4", 0 );
//	ct_get_gpio_ctrl( "DDR", "PF4");
	ct_set_gpio_ctrl( "DDR", "PG0", 1 );
	ct_get_gpio_ctrl( "DDR", "PG0");
	ct_set_gpio_ctrl( "DDR", "PG0", 0 );
	ct_get_gpio_ctrl( "DDR", "PG0");
	ct_set_gpio_ctrl( "DDR", "PG1", 1 );
	ct_get_gpio_ctrl( "DDR", "PG1");
	ct_set_gpio_ctrl( "DDR", "PG1", 0 );
	ct_get_gpio_ctrl( "DDR", "PG1");
	ct_set_gpio_ctrl( "DDR", "PG2", 1 );
	ct_get_gpio_ctrl( "DDR", "PG2");
	ct_set_gpio_ctrl( "DDR", "PG2", 0 );
	ct_get_gpio_ctrl( "DDR", "PG2");
	ct_set_gpio_ctrl( "DDR", "PG3", 1 );
	ct_get_gpio_ctrl( "DDR", "PG3");
	ct_set_gpio_ctrl( "DDR", "PG3", 0 );
	ct_get_gpio_ctrl( "DDR", "PG3");
	ct_set_gpio_ctrl( "DDR", "PG4", 1 );
	ct_get_gpio_ctrl( "DDR", "PG4");
	ct_set_gpio_ctrl( "DDR", "PG4", 0 );
	ct_get_gpio_ctrl( "DDR", "PG4");
	ct_set_gpio_ctrl( "DDR", "PG5", 1 );
	ct_get_gpio_ctrl( "DDR", "PG5");
	ct_set_gpio_ctrl( "DDR", "PG5", 0 );
	ct_get_gpio_ctrl( "DDR", "PG5");
	ct_set_gpio_ctrl( "DDR", "PG6", 1 );
	ct_get_gpio_ctrl( "DDR", "PG6");
	ct_set_gpio_ctrl( "DDR", "PG6", 0 );
	ct_get_gpio_ctrl( "DDR", "PG6");
	ct_set_gpio_ctrl( "DDR", "PG7", 1 );
	ct_get_gpio_ctrl( "DDR", "PG7");
	ct_set_gpio_ctrl( "DDR", "PG7", 0 );
	ct_get_gpio_ctrl( "DDR", "PG7");
	ct_set_gpio_ctrl( "DDR", "PH0", 1 );
	ct_get_gpio_ctrl( "DDR", "PH0");
	ct_set_gpio_ctrl( "DDR", "PH0", 0 );
	ct_get_gpio_ctrl( "DDR", "PH0");
	ct_set_gpio_ctrl( "DDR", "PH1", 1 );
	ct_get_gpio_ctrl( "DDR", "PH1");
	ct_set_gpio_ctrl( "DDR", "PH1", 0 );
	ct_get_gpio_ctrl( "DDR", "PH1");
	ct_set_gpio_ctrl( "DDR", "PH2", 1 );
	ct_get_gpio_ctrl( "DDR", "PH2");
	ct_set_gpio_ctrl( "DDR", "PH2", 0 );
	ct_get_gpio_ctrl( "DDR", "PH2");
	ct_set_gpio_ctrl( "DDR", "PH3", 1 );
	ct_get_gpio_ctrl( "DDR", "PH3");
	ct_set_gpio_ctrl( "DDR", "PH3", 0 );
	ct_get_gpio_ctrl( "DDR", "PH3");
	ct_set_gpio_ctrl( "DDR", "PW0", 1 );
	ct_get_gpio_ctrl( "DDR", "PW0");
	ct_set_gpio_ctrl( "DDR", "PW0", 0 );
	ct_get_gpio_ctrl( "DDR", "PW0");
	ct_set_gpio_ctrl( "DDR", "PW1", 1 );
	ct_get_gpio_ctrl( "DDR", "PW1");
	ct_set_gpio_ctrl( "DDR", "PW1", 0 );
	ct_get_gpio_ctrl( "DDR", "PW1");
	ct_set_gpio_ctrl( "DDR", "PW2", 1 );
	ct_get_gpio_ctrl( "DDR", "PW2");
	ct_set_gpio_ctrl( "DDR", "PW2", 0 );
	ct_get_gpio_ctrl( "DDR", "PW2");
	ct_set_gpio_ctrl( "DDR", "PW3", 1 );
	ct_get_gpio_ctrl( "DDR", "PW3");
	ct_set_gpio_ctrl( "DDR", "PW3", 0 );
	ct_get_gpio_ctrl( "DDR", "PW3");
	ct_set_gpio_ctrl( "DDR", "PW4", 1 );
	ct_get_gpio_ctrl( "DDR", "PW4");
	ct_set_gpio_ctrl( "DDR", "PW4", 0 );
	ct_get_gpio_ctrl( "DDR", "PW4");
	ct_set_gpio_ctrl( "DDR", "PW5", 1 );
	ct_get_gpio_ctrl( "DDR", "PW5");
	ct_set_gpio_ctrl( "DDR", "PW5", 0 );
	ct_get_gpio_ctrl( "DDR", "PW5");
	ct_set_gpio_ctrl( "DDR", "PW6", 1 );
	ct_get_gpio_ctrl( "DDR", "PW6");
	ct_set_gpio_ctrl( "DDR", "PW6", 0 );
	ct_get_gpio_ctrl( "DDR", "PW6");
	ct_set_gpio_ctrl( "DDR", "PW7", 1 );
	ct_get_gpio_ctrl( "DDR", "PW7");
	ct_set_gpio_ctrl( "DDR", "PW7", 0 );
	ct_get_gpio_ctrl( "DDR", "PW7");
	ct_set_gpio_ctrl( "DDR", "PJ0", 1 );
	ct_get_gpio_ctrl( "DDR", "PJ0");
	ct_set_gpio_ctrl( "DDR", "PJ0", 0 );
	ct_get_gpio_ctrl( "DDR", "PJ0");
	ct_set_gpio_ctrl( "DDR", "PJ1", 1 );
	ct_get_gpio_ctrl( "DDR", "PJ1");
	ct_set_gpio_ctrl( "DDR", "PJ1", 0 );
	ct_get_gpio_ctrl( "DDR", "PJ1");
	ct_set_gpio_ctrl( "DDR", "PJ2", 1 );
	ct_get_gpio_ctrl( "DDR", "PJ2");
	ct_set_gpio_ctrl( "DDR", "PJ2", 0 );
	ct_get_gpio_ctrl( "DDR", "PJ2");
	ct_set_gpio_ctrl( "DDR", "PJ3", 1 );
	ct_get_gpio_ctrl( "DDR", "PJ3");
	ct_set_gpio_ctrl( "DDR", "PJ3", 0 );
	ct_get_gpio_ctrl( "DDR", "PJ3");
	ct_set_gpio_ctrl( "DDR", "PJ4", 1 );
	ct_get_gpio_ctrl( "DDR", "PJ4");
	ct_set_gpio_ctrl( "DDR", "PJ4", 0 );
	ct_get_gpio_ctrl( "DDR", "PJ4");
	ct_set_gpio_ctrl( "DDR", "PJ5", 1 );
	ct_get_gpio_ctrl( "DDR", "PJ5");
	ct_set_gpio_ctrl( "DDR", "PJ5", 0 );
	ct_get_gpio_ctrl( "DDR", "PJ5");
	ct_set_gpio_ctrl( "DDR", "PJ6", 1 );
	ct_get_gpio_ctrl( "DDR", "PJ6");
	ct_set_gpio_ctrl( "DDR", "PJ6", 0 );
	ct_get_gpio_ctrl( "DDR", "PJ6");
	ct_set_gpio_ctrl( "DDR", "PJ7", 1 );
	ct_get_gpio_ctrl( "DDR", "PJ7");
	ct_set_gpio_ctrl( "DDR", "PJ7", 0 );
	ct_get_gpio_ctrl( "DDR", "PJ7");
	ct_set_gpio_ctrl( "DDR", "PK0", 1 );
	ct_get_gpio_ctrl( "DDR", "PK0");
	ct_set_gpio_ctrl( "DDR", "PK0", 0 );
	ct_get_gpio_ctrl( "DDR", "PK0");
	ct_set_gpio_ctrl( "DDR", "PK1", 1 );
	ct_get_gpio_ctrl( "DDR", "PK1");
	ct_set_gpio_ctrl( "DDR", "PK1", 0 );
	ct_get_gpio_ctrl( "DDR", "PK1");
	ct_set_gpio_ctrl( "DDR", "PK2", 1 );
	ct_get_gpio_ctrl( "DDR", "PK2");
	ct_set_gpio_ctrl( "DDR", "PK2", 0 );
	ct_get_gpio_ctrl( "DDR", "PK2");
	ct_set_gpio_ctrl( "DDR", "PK3", 1 );
	ct_get_gpio_ctrl( "DDR", "PK3");
	ct_set_gpio_ctrl( "DDR", "PK3", 0 );
	ct_get_gpio_ctrl( "DDR", "PK3");
	ct_set_gpio_ctrl( "DDR", "PK4", 1 );
	ct_get_gpio_ctrl( "DDR", "PK4");
	ct_set_gpio_ctrl( "DDR", "PK4", 0 );
	ct_get_gpio_ctrl( "DDR", "PK4");
	ct_set_gpio_ctrl( "DDR", "PK5", 1 );
	ct_get_gpio_ctrl( "DDR", "PK5");
	ct_set_gpio_ctrl( "DDR", "PK5", 0 );
	ct_get_gpio_ctrl( "DDR", "PK5");
	ct_set_gpio_ctrl( "DDR", "PK6", 1 );
	ct_get_gpio_ctrl( "DDR", "PK6");
	ct_set_gpio_ctrl( "DDR", "PK6", 0 );
	ct_get_gpio_ctrl( "DDR", "PK6");
	ct_set_gpio_ctrl( "DDR", "PK7", 1 );
	ct_get_gpio_ctrl( "DDR", "PK7");
	ct_set_gpio_ctrl( "DDR", "PK7", 0 );
	ct_get_gpio_ctrl( "DDR", "PK7");
	ct_set_gpio_ctrl( "DDR", "PL0", 1 );
	ct_get_gpio_ctrl( "DDR", "PL0");
	ct_set_gpio_ctrl( "DDR", "PL0", 0 );
	ct_get_gpio_ctrl( "DDR", "PL0");
	ct_set_gpio_ctrl( "DDR", "PL1", 1 );
	ct_get_gpio_ctrl( "DDR", "PL1");
	ct_set_gpio_ctrl( "DDR", "PL1", 0 );
	ct_get_gpio_ctrl( "DDR", "PL1");
	ct_set_gpio_ctrl( "DDR", "PL2", 1 );
	ct_get_gpio_ctrl( "DDR", "PL2");
	ct_set_gpio_ctrl( "DDR", "PL2", 0 );
	ct_get_gpio_ctrl( "DDR", "PL2");
	ct_set_gpio_ctrl( "DDR", "PL3", 1 );
	ct_get_gpio_ctrl( "DDR", "PL3");
	ct_set_gpio_ctrl( "DDR", "PL3", 0 );
	ct_get_gpio_ctrl( "DDR", "PL3");
	ct_set_gpio_ctrl( "DDR", "PL4", 1 );
	ct_get_gpio_ctrl( "DDR", "PL4");
	ct_set_gpio_ctrl( "DDR", "PL4", 0 );
	ct_get_gpio_ctrl( "DDR", "PL4");
	ct_set_gpio_ctrl( "DDR", "PL5", 1 );
	ct_get_gpio_ctrl( "DDR", "PL5");
	ct_set_gpio_ctrl( "DDR", "PL5", 0 );
	ct_get_gpio_ctrl( "DDR", "PL5");
	ct_set_gpio_ctrl( "DDR", "PM0", 1 );
	ct_get_gpio_ctrl( "DDR", "PM0");
	ct_set_gpio_ctrl( "DDR", "PM0", 0 );
	ct_get_gpio_ctrl( "DDR", "PM0");
	ct_set_gpio_ctrl( "DDR", "PM1", 1 );
	ct_get_gpio_ctrl( "DDR", "PM1");
	ct_set_gpio_ctrl( "DDR", "PM1", 0 );
	ct_get_gpio_ctrl( "DDR", "PM1");
	ct_set_gpio_ctrl( "DDR", "PM2", 1 );
	ct_get_gpio_ctrl( "DDR", "PM2");
	ct_set_gpio_ctrl( "DDR", "PM2", 0 );
	ct_get_gpio_ctrl( "DDR", "PM2");
	ct_set_gpio_ctrl( "DDR", "PM3", 1 );
	ct_get_gpio_ctrl( "DDR", "PM3");
	ct_set_gpio_ctrl( "DDR", "PM3", 0 );
	ct_get_gpio_ctrl( "DDR", "PM3");
	ct_set_gpio_ctrl( "DDR", "PM4", 1 );
	ct_get_gpio_ctrl( "DDR", "PM4");
	ct_set_gpio_ctrl( "DDR", "PM4", 0 );
	ct_get_gpio_ctrl( "DDR", "PM4");
	ct_set_gpio_ctrl( "DDR", "PN0", 1 );
	ct_get_gpio_ctrl( "DDR", "PN0");
	ct_set_gpio_ctrl( "DDR", "PN0", 0 );
	ct_get_gpio_ctrl( "DDR", "PN0");
	ct_set_gpio_ctrl( "DDR", "PN1", 1 );
	ct_get_gpio_ctrl( "DDR", "PN1");
	ct_set_gpio_ctrl( "DDR", "PN1", 0 );
	ct_get_gpio_ctrl( "DDR", "PN1");
	ct_set_gpio_ctrl( "DDR", "PN2", 1 );
	ct_get_gpio_ctrl( "DDR", "PN2");
	ct_set_gpio_ctrl( "DDR", "PN2", 0 );
	ct_get_gpio_ctrl( "DDR", "PN2");
	ct_set_gpio_ctrl( "DDR", "PN3", 1 );
	ct_get_gpio_ctrl( "DDR", "PN3");
	ct_set_gpio_ctrl( "DDR", "PN3", 0 );
	ct_get_gpio_ctrl( "DDR", "PN3");
	ct_set_gpio_ctrl( "DDR", "PN4", 1 );
	ct_get_gpio_ctrl( "DDR", "PN4");
	ct_set_gpio_ctrl( "DDR", "PN4", 0 );
	ct_get_gpio_ctrl( "DDR", "PN4");
	ct_set_gpio_ctrl( "DDR", "PY0", 1 );
	ct_get_gpio_ctrl( "DDR", "PY0");
	ct_set_gpio_ctrl( "DDR", "PY0", 0 );
	ct_get_gpio_ctrl( "DDR", "PY0");
	ct_set_gpio_ctrl( "DDR", "PY1", 1 );
	ct_get_gpio_ctrl( "DDR", "PY1");
	ct_set_gpio_ctrl( "DDR", "PY1", 0 );
	ct_get_gpio_ctrl( "DDR", "PY1");
	ct_set_gpio_ctrl( "DDR", "PY2", 1 );
	ct_get_gpio_ctrl( "DDR", "PY2");
	ct_set_gpio_ctrl( "DDR", "PY2", 0 );
	ct_get_gpio_ctrl( "DDR", "PY2");
	ct_set_gpio_ctrl( "DDR", "PY3", 1 );
	ct_get_gpio_ctrl( "DDR", "PY3");
	ct_set_gpio_ctrl( "DDR", "PY3", 0 );
	ct_get_gpio_ctrl( "DDR", "PY3");
	ct_set_gpio_ctrl( "DDR", "PY4", 1 );
	ct_get_gpio_ctrl( "DDR", "PY4");
	ct_set_gpio_ctrl( "DDR", "PY4", 0 );
	ct_get_gpio_ctrl( "DDR", "PY4");
	ct_set_gpio_ctrl( "DDR", "PY5", 1 );
	ct_get_gpio_ctrl( "DDR", "PY5");
	ct_set_gpio_ctrl( "DDR", "PY5", 0 );
	ct_get_gpio_ctrl( "DDR", "PY5");
	ct_set_gpio_ctrl( "DDR", "PY6", 1 );
	ct_get_gpio_ctrl( "DDR", "PY6");
	ct_set_gpio_ctrl( "DDR", "PY6", 0 );
	ct_get_gpio_ctrl( "DDR", "PY6");
	ct_set_gpio_ctrl( "DDR", "PY7", 1 );
	ct_get_gpio_ctrl( "DDR", "PY7");
	ct_set_gpio_ctrl( "DDR", "PY7", 0 );
	ct_get_gpio_ctrl( "DDR", "PY7");
	ct_set_gpio_ctrl( "DDR", "PP0", 1 );
	ct_get_gpio_ctrl( "DDR", "PP0");
	ct_set_gpio_ctrl( "DDR", "PP0", 0 );
	ct_get_gpio_ctrl( "DDR", "PP0");
	ct_set_gpio_ctrl( "DDR", "PP1", 1 );
	ct_get_gpio_ctrl( "DDR", "PP1");
	ct_set_gpio_ctrl( "DDR", "PP1", 0 );
	ct_get_gpio_ctrl( "DDR", "PP1");
	ct_set_gpio_ctrl( "DDR", "PP2", 1 );
	ct_get_gpio_ctrl( "DDR", "PP2");
	ct_set_gpio_ctrl( "DDR", "PP2", 0 );
	ct_get_gpio_ctrl( "DDR", "PP2");
	ct_set_gpio_ctrl( "DDR", "PP3", 1 );
	ct_get_gpio_ctrl( "DDR", "PP3");
	ct_set_gpio_ctrl( "DDR", "PP3", 0 );
	ct_get_gpio_ctrl( "DDR", "PP3");

	/* PUDER */
	ct_set_gpio_ctrl( "PUDER", "P80", 1 );
	ct_get_gpio_ctrl( "PUDER", "P80");
	ct_set_gpio_ctrl( "PUDER", "P80", 0 );
	ct_get_gpio_ctrl( "PUDER", "P80");
	ct_set_gpio_ctrl( "PUDER", "P81", 1 );
	ct_get_gpio_ctrl( "PUDER", "P81");
	ct_set_gpio_ctrl( "PUDER", "P81", 0 );
	ct_get_gpio_ctrl( "PUDER", "P81");
	ct_set_gpio_ctrl( "PUDER", "P82", 1 );
	ct_get_gpio_ctrl( "PUDER", "P82");
	ct_set_gpio_ctrl( "PUDER", "P82", 0 );
	ct_get_gpio_ctrl( "PUDER", "P82");
	ct_set_gpio_ctrl( "PUDER", "P83", 1 );
	ct_get_gpio_ctrl( "PUDER", "P83");
	ct_set_gpio_ctrl( "PUDER", "P83", 0 );
	ct_get_gpio_ctrl( "PUDER", "P83");
	ct_set_gpio_ctrl( "PUDER", "P86", 1 );
	ct_get_gpio_ctrl( "PUDER", "P86");
	ct_set_gpio_ctrl( "PUDER", "P86", 0 );
	ct_get_gpio_ctrl( "PUDER", "P86");
	ct_set_gpio_ctrl( "PUDER", "P87", 1 );
	ct_get_gpio_ctrl( "PUDER", "P87");
	ct_set_gpio_ctrl( "PUDER", "P87", 0 );
	ct_get_gpio_ctrl( "PUDER", "P87");
	ct_set_gpio_ctrl( "PUDER", "P90", 1 );
	ct_get_gpio_ctrl( "PUDER", "P90");
	ct_set_gpio_ctrl( "PUDER", "P90", 0 );
	ct_get_gpio_ctrl( "PUDER", "P90");
	ct_set_gpio_ctrl( "PUDER", "P91", 1 );
	ct_get_gpio_ctrl( "PUDER", "P91");
	ct_set_gpio_ctrl( "PUDER", "P91", 0 );
	ct_get_gpio_ctrl( "PUDER", "P91");
	ct_set_gpio_ctrl( "PUDER", "P92", 1 );
	ct_get_gpio_ctrl( "PUDER", "P92");
	ct_set_gpio_ctrl( "PUDER", "P92", 0 );
	ct_get_gpio_ctrl( "PUDER", "P92");
	ct_set_gpio_ctrl( "PUDER", "P93", 1 );
	ct_get_gpio_ctrl( "PUDER", "P93");
	ct_set_gpio_ctrl( "PUDER", "P93", 0 );
	ct_get_gpio_ctrl( "PUDER", "P93");
	ct_set_gpio_ctrl( "PUDER", "PA0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PA0");
	ct_set_gpio_ctrl( "PUDER", "PA0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PA0");
	ct_set_gpio_ctrl( "PUDER", "PA1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PA1");
	ct_set_gpio_ctrl( "PUDER", "PA1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PA1");
	ct_set_gpio_ctrl( "PUDER", "PA2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PA2");
	ct_set_gpio_ctrl( "PUDER", "PA2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PA2");
	ct_set_gpio_ctrl( "PUDER", "PA3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PA3");
	ct_set_gpio_ctrl( "PUDER", "PA3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PA3");
	ct_set_gpio_ctrl( "PUDER", "PA4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PA4");
	ct_set_gpio_ctrl( "PUDER", "PA4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PA4");
	ct_set_gpio_ctrl( "PUDER", "PA5", 1 );
	ct_get_gpio_ctrl( "PUDER", "PA5");
	ct_set_gpio_ctrl( "PUDER", "PA5", 0 );
	ct_get_gpio_ctrl( "PUDER", "PA5");
	ct_set_gpio_ctrl( "PUDER", "PA6", 1 );
	ct_get_gpio_ctrl( "PUDER", "PA6");
	ct_set_gpio_ctrl( "PUDER", "PA6", 0 );
	ct_get_gpio_ctrl( "PUDER", "PA6");
	ct_set_gpio_ctrl( "PUDER", "PA7", 1 );
	ct_get_gpio_ctrl( "PUDER", "PA7");
	ct_set_gpio_ctrl( "PUDER", "PA7", 0 );
	ct_get_gpio_ctrl( "PUDER", "PA7");
	ct_set_gpio_ctrl( "PUDER", "PB0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PB0");
	ct_set_gpio_ctrl( "PUDER", "PB0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PB0");
	ct_set_gpio_ctrl( "PUDER", "PB1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PB1");
	ct_set_gpio_ctrl( "PUDER", "PB1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PB1");
	ct_set_gpio_ctrl( "PUDER", "PB2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PB2");
	ct_set_gpio_ctrl( "PUDER", "PB2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PB2");
	ct_set_gpio_ctrl( "PUDER", "PB3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PB3");
	ct_set_gpio_ctrl( "PUDER", "PB3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PB3");
	ct_set_gpio_ctrl( "PUDER", "PB4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PB4");
	ct_set_gpio_ctrl( "PUDER", "PB4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PB4");
	ct_set_gpio_ctrl( "PUDER", "PD0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PD0");
	ct_set_gpio_ctrl( "PUDER", "PD0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PD0");
	ct_set_gpio_ctrl( "PUDER", "PD1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PD1");
	ct_set_gpio_ctrl( "PUDER", "PD1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PD1");
	ct_set_gpio_ctrl( "PUDER", "PD2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PD2");
	ct_set_gpio_ctrl( "PUDER", "PD2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PD2");
	ct_set_gpio_ctrl( "PUDER", "PD3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PD3");
	ct_set_gpio_ctrl( "PUDER", "PD3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PD3");
	ct_set_gpio_ctrl( "PUDER", "PD4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PD4");
	ct_set_gpio_ctrl( "PUDER", "PD4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PD4");
	ct_set_gpio_ctrl( "PUDER", "PD5", 1 );
	ct_get_gpio_ctrl( "PUDER", "PD5");
	ct_set_gpio_ctrl( "PUDER", "PD5", 0 );
	ct_get_gpio_ctrl( "PUDER", "PD5");
	ct_set_gpio_ctrl( "PUDER", "PD6", 1 );
	ct_get_gpio_ctrl( "PUDER", "PD6");
	ct_set_gpio_ctrl( "PUDER", "PD6", 0 );
	ct_get_gpio_ctrl( "PUDER", "PD6");
//	ct_set_gpio_ctrl( "PUDER", "PE0", 1 );
//	ct_get_gpio_ctrl( "PUDER", "PE0");
//	ct_set_gpio_ctrl( "PUDER", "PE0", 0 );
//	ct_get_gpio_ctrl( "PUDER", "PE0");
//	ct_set_gpio_ctrl( "PUDER", "PE1", 1 );
//	ct_get_gpio_ctrl( "PUDER", "PE1");
//	ct_set_gpio_ctrl( "PUDER", "PE1", 0 );
//	ct_get_gpio_ctrl( "PUDER", "PE1");
//	ct_set_gpio_ctrl( "PUDER", "PF2", 1 );
//	ct_get_gpio_ctrl( "PUDER", "PF2");
//	ct_set_gpio_ctrl( "PUDER", "PF2", 0 );
//	ct_get_gpio_ctrl( "PUDER", "PF2");
//	ct_set_gpio_ctrl( "PUDER", "PF3", 1 );
//	ct_get_gpio_ctrl( "PUDER", "PF3");
//	ct_set_gpio_ctrl( "PUDER", "PF3", 0 );
//	ct_get_gpio_ctrl( "PUDER", "PF3");
//	ct_set_gpio_ctrl( "PUDER", "PF4", 1 );
//	ct_get_gpio_ctrl( "PUDER", "PF4");
//	ct_set_gpio_ctrl( "PUDER", "PF4", 0 );
//	ct_get_gpio_ctrl( "PUDER", "PF4");
	ct_set_gpio_ctrl( "PUDER", "PG0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PG0");
	ct_set_gpio_ctrl( "PUDER", "PG0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PG0");
	ct_set_gpio_ctrl( "PUDER", "PG1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PG1");
	ct_set_gpio_ctrl( "PUDER", "PG1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PG1");
	ct_set_gpio_ctrl( "PUDER", "PG2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PG2");
	ct_set_gpio_ctrl( "PUDER", "PG2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PG2");
	ct_set_gpio_ctrl( "PUDER", "PG3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PG3");
	ct_set_gpio_ctrl( "PUDER", "PG3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PG3");
	ct_set_gpio_ctrl( "PUDER", "PG4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PG4");
	ct_set_gpio_ctrl( "PUDER", "PG4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PG4");
	ct_set_gpio_ctrl( "PUDER", "PG5", 1 );
	ct_get_gpio_ctrl( "PUDER", "PG5");
	ct_set_gpio_ctrl( "PUDER", "PG5", 0 );
	ct_get_gpio_ctrl( "PUDER", "PG5");
	ct_set_gpio_ctrl( "PUDER", "PG6", 1 );
	ct_get_gpio_ctrl( "PUDER", "PG6");
	ct_set_gpio_ctrl( "PUDER", "PG6", 0 );
	ct_get_gpio_ctrl( "PUDER", "PG6");
	ct_set_gpio_ctrl( "PUDER", "PG7", 1 );
	ct_get_gpio_ctrl( "PUDER", "PG7");
	ct_set_gpio_ctrl( "PUDER", "PG7", 0 );
	ct_get_gpio_ctrl( "PUDER", "PG7");
	ct_set_gpio_ctrl( "PUDER", "PH0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PH0");
	ct_set_gpio_ctrl( "PUDER", "PH0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PH0");
	ct_set_gpio_ctrl( "PUDER", "PH1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PH1");
	ct_set_gpio_ctrl( "PUDER", "PH1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PH1");
	ct_set_gpio_ctrl( "PUDER", "PH2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PH2");
	ct_set_gpio_ctrl( "PUDER", "PH2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PH2");
	ct_set_gpio_ctrl( "PUDER", "PH3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PH3");
	ct_set_gpio_ctrl( "PUDER", "PH3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PH3");
	ct_set_gpio_ctrl( "PUDER", "PW0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PW0");
	ct_set_gpio_ctrl( "PUDER", "PW0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PW0");
	ct_set_gpio_ctrl( "PUDER", "PW1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PW1");
	ct_set_gpio_ctrl( "PUDER", "PW1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PW1");
	ct_set_gpio_ctrl( "PUDER", "PW2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PW2");
	ct_set_gpio_ctrl( "PUDER", "PW2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PW2");
	ct_set_gpio_ctrl( "PUDER", "PW3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PW3");
	ct_set_gpio_ctrl( "PUDER", "PW3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PW3");
	ct_set_gpio_ctrl( "PUDER", "PW4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PW4");
	ct_set_gpio_ctrl( "PUDER", "PW4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PW4");
	ct_set_gpio_ctrl( "PUDER", "PW5", 1 );
	ct_get_gpio_ctrl( "PUDER", "PW5");
	ct_set_gpio_ctrl( "PUDER", "PW5", 0 );
	ct_get_gpio_ctrl( "PUDER", "PW5");
	ct_set_gpio_ctrl( "PUDER", "PW6", 1 );
	ct_get_gpio_ctrl( "PUDER", "PW6");
	ct_set_gpio_ctrl( "PUDER", "PW6", 0 );
	ct_get_gpio_ctrl( "PUDER", "PW6");
	ct_set_gpio_ctrl( "PUDER", "PW7", 1 );
	ct_get_gpio_ctrl( "PUDER", "PW7");
	ct_set_gpio_ctrl( "PUDER", "PW7", 0 );
	ct_get_gpio_ctrl( "PUDER", "PW7");
	ct_set_gpio_ctrl( "PUDER", "PJ0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PJ0");
	ct_set_gpio_ctrl( "PUDER", "PJ0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PJ0");
	ct_set_gpio_ctrl( "PUDER", "PJ1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PJ1");
	ct_set_gpio_ctrl( "PUDER", "PJ1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PJ1");
	ct_set_gpio_ctrl( "PUDER", "PJ2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PJ2");
	ct_set_gpio_ctrl( "PUDER", "PJ2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PJ2");
	ct_set_gpio_ctrl( "PUDER", "PJ3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PJ3");
	ct_set_gpio_ctrl( "PUDER", "PJ3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PJ3");
	ct_set_gpio_ctrl( "PUDER", "PJ4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PJ4");
	ct_set_gpio_ctrl( "PUDER", "PJ4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PJ4");
	ct_set_gpio_ctrl( "PUDER", "PJ5", 1 );
	ct_get_gpio_ctrl( "PUDER", "PJ5");
	ct_set_gpio_ctrl( "PUDER", "PJ5", 0 );
	ct_get_gpio_ctrl( "PUDER", "PJ5");
	ct_set_gpio_ctrl( "PUDER", "PJ6", 1 );
	ct_get_gpio_ctrl( "PUDER", "PJ6");
	ct_set_gpio_ctrl( "PUDER", "PJ6", 0 );
	ct_get_gpio_ctrl( "PUDER", "PJ6");
	ct_set_gpio_ctrl( "PUDER", "PJ7", 1 );
	ct_get_gpio_ctrl( "PUDER", "PJ7");
	ct_set_gpio_ctrl( "PUDER", "PJ7", 0 );
	ct_get_gpio_ctrl( "PUDER", "PJ7");
	ct_set_gpio_ctrl( "PUDER", "PK0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PK0");
	ct_set_gpio_ctrl( "PUDER", "PK0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PK0");
	ct_set_gpio_ctrl( "PUDER", "PK1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PK1");
	ct_set_gpio_ctrl( "PUDER", "PK1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PK1");
	ct_set_gpio_ctrl( "PUDER", "PK2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PK2");
	ct_set_gpio_ctrl( "PUDER", "PK2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PK2");
	ct_set_gpio_ctrl( "PUDER", "PK3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PK3");
	ct_set_gpio_ctrl( "PUDER", "PK3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PK3");
	ct_set_gpio_ctrl( "PUDER", "PK4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PK4");
	ct_set_gpio_ctrl( "PUDER", "PK4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PK4");
	ct_set_gpio_ctrl( "PUDER", "PK5", 1 );
	ct_get_gpio_ctrl( "PUDER", "PK5");
	ct_set_gpio_ctrl( "PUDER", "PK5", 0 );
	ct_get_gpio_ctrl( "PUDER", "PK5");
	ct_set_gpio_ctrl( "PUDER", "PK6", 1 );
	ct_get_gpio_ctrl( "PUDER", "PK6");
	ct_set_gpio_ctrl( "PUDER", "PK6", 0 );
	ct_get_gpio_ctrl( "PUDER", "PK6");
	ct_set_gpio_ctrl( "PUDER", "PK7", 1 );
	ct_get_gpio_ctrl( "PUDER", "PK7");
	ct_set_gpio_ctrl( "PUDER", "PK7", 0 );
	ct_get_gpio_ctrl( "PUDER", "PK7");
	ct_set_gpio_ctrl( "PUDER", "PL0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PL0");
	ct_set_gpio_ctrl( "PUDER", "PL0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PL0");
	ct_set_gpio_ctrl( "PUDER", "PL1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PL1");
	ct_set_gpio_ctrl( "PUDER", "PL1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PL1");
	ct_set_gpio_ctrl( "PUDER", "PL2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PL2");
	ct_set_gpio_ctrl( "PUDER", "PL2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PL2");
	ct_set_gpio_ctrl( "PUDER", "PL3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PL3");
	ct_set_gpio_ctrl( "PUDER", "PL3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PL3");
	ct_set_gpio_ctrl( "PUDER", "PL4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PL4");
	ct_set_gpio_ctrl( "PUDER", "PL4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PL4");
	ct_set_gpio_ctrl( "PUDER", "PL5", 1 );
	ct_get_gpio_ctrl( "PUDER", "PL5");
	ct_set_gpio_ctrl( "PUDER", "PL5", 0 );
	ct_get_gpio_ctrl( "PUDER", "PL5");
	ct_set_gpio_ctrl( "PUDER", "PM0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PM0");
	ct_set_gpio_ctrl( "PUDER", "PM0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PM0");
	ct_set_gpio_ctrl( "PUDER", "PM1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PM1");
	ct_set_gpio_ctrl( "PUDER", "PM1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PM1");
	ct_set_gpio_ctrl( "PUDER", "PM3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PM3");
	ct_set_gpio_ctrl( "PUDER", "PM3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PM3");
	ct_set_gpio_ctrl( "PUDER", "PM4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PM4");
	ct_set_gpio_ctrl( "PUDER", "PM4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PM4");
	ct_set_gpio_ctrl( "PUDER", "PN0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PN0");
	ct_set_gpio_ctrl( "PUDER", "PN0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PN0");
	ct_set_gpio_ctrl( "PUDER", "PN1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PN1");
	ct_set_gpio_ctrl( "PUDER", "PN1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PN1");
	ct_set_gpio_ctrl( "PUDER", "PN3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PN3");
	ct_set_gpio_ctrl( "PUDER", "PN3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PN3");
	ct_set_gpio_ctrl( "PUDER", "PN4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PN4");
	ct_set_gpio_ctrl( "PUDER", "PN4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PN4");
	ct_set_gpio_ctrl( "PUDER", "PY0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PY0");
	ct_set_gpio_ctrl( "PUDER", "PY0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PY0");
	ct_set_gpio_ctrl( "PUDER", "PY1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PY1");
	ct_set_gpio_ctrl( "PUDER", "PY1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PY1");
	ct_set_gpio_ctrl( "PUDER", "PY2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PY2");
	ct_set_gpio_ctrl( "PUDER", "PY2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PY2");
	ct_set_gpio_ctrl( "PUDER", "PY3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PY3");
	ct_set_gpio_ctrl( "PUDER", "PY3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PY3");
	ct_set_gpio_ctrl( "PUDER", "PY4", 1 );
	ct_get_gpio_ctrl( "PUDER", "PY4");
	ct_set_gpio_ctrl( "PUDER", "PY4", 0 );
	ct_get_gpio_ctrl( "PUDER", "PY4");
	ct_set_gpio_ctrl( "PUDER", "PY5", 1 );
	ct_get_gpio_ctrl( "PUDER", "PY5");
	ct_set_gpio_ctrl( "PUDER", "PY5", 0 );
	ct_get_gpio_ctrl( "PUDER", "PY5");
	ct_set_gpio_ctrl( "PUDER", "PY6", 1 );
	ct_get_gpio_ctrl( "PUDER", "PY6");
	ct_set_gpio_ctrl( "PUDER", "PY6", 0 );
	ct_get_gpio_ctrl( "PUDER", "PY6");
	ct_set_gpio_ctrl( "PUDER", "PY7", 1 );
	ct_get_gpio_ctrl( "PUDER", "PY7");
	ct_set_gpio_ctrl( "PUDER", "PY7", 0 );
	ct_get_gpio_ctrl( "PUDER", "PY7");
	ct_set_gpio_ctrl( "PUDER", "PP0", 1 );
	ct_get_gpio_ctrl( "PUDER", "PP0");
	ct_set_gpio_ctrl( "PUDER", "PP0", 0 );
	ct_get_gpio_ctrl( "PUDER", "PP0");
	ct_set_gpio_ctrl( "PUDER", "PP1", 1 );
	ct_get_gpio_ctrl( "PUDER", "PP1");
	ct_set_gpio_ctrl( "PUDER", "PP1", 0 );
	ct_get_gpio_ctrl( "PUDER", "PP1");
	ct_set_gpio_ctrl( "PUDER", "PP2", 1 );
	ct_get_gpio_ctrl( "PUDER", "PP2");
	ct_set_gpio_ctrl( "PUDER", "PP2", 0 );
	ct_get_gpio_ctrl( "PUDER", "PP2");
	ct_set_gpio_ctrl( "PUDER", "PP3", 1 );
	ct_get_gpio_ctrl( "PUDER", "PP3");
	ct_set_gpio_ctrl( "PUDER", "PP3", 0 );
	ct_get_gpio_ctrl( "PUDER", "PP3");

	/* PUDCR */
	ct_set_gpio_ctrl( "PUDCR", "P80", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P80");
	ct_set_gpio_ctrl( "PUDCR", "P80", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P80");
	ct_set_gpio_ctrl( "PUDCR", "P81", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P81");
	ct_set_gpio_ctrl( "PUDCR", "P81", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P81");
	ct_set_gpio_ctrl( "PUDCR", "P82", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P82");
	ct_set_gpio_ctrl( "PUDCR", "P82", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P82");
	ct_set_gpio_ctrl( "PUDCR", "P83", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P83");
	ct_set_gpio_ctrl( "PUDCR", "P83", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P83");
	ct_set_gpio_ctrl( "PUDCR", "P86", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P86");
	ct_set_gpio_ctrl( "PUDCR", "P86", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P86");
	ct_set_gpio_ctrl( "PUDCR", "P87", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P87");
	ct_set_gpio_ctrl( "PUDCR", "P87", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P87");
	ct_set_gpio_ctrl( "PUDCR", "P90", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P90");
	ct_set_gpio_ctrl( "PUDCR", "P90", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P90");
	ct_set_gpio_ctrl( "PUDCR", "P91", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P91");
	ct_set_gpio_ctrl( "PUDCR", "P91", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P91");
	ct_set_gpio_ctrl( "PUDCR", "P92", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P92");
	ct_set_gpio_ctrl( "PUDCR", "P92", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P92");
	ct_set_gpio_ctrl( "PUDCR", "P93", 1 );
	ct_get_gpio_ctrl( "PUDCR", "P93");
	ct_set_gpio_ctrl( "PUDCR", "P93", 0 );
	ct_get_gpio_ctrl( "PUDCR", "P93");
	ct_set_gpio_ctrl( "PUDCR", "PA0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PA0");
	ct_set_gpio_ctrl( "PUDCR", "PA0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PA0");
	ct_set_gpio_ctrl( "PUDCR", "PA1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PA1");
	ct_set_gpio_ctrl( "PUDCR", "PA1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PA1");
	ct_set_gpio_ctrl( "PUDCR", "PA2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PA2");
	ct_set_gpio_ctrl( "PUDCR", "PA2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PA2");
	ct_set_gpio_ctrl( "PUDCR", "PA3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PA3");
	ct_set_gpio_ctrl( "PUDCR", "PA3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PA3");
	ct_set_gpio_ctrl( "PUDCR", "PA4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PA4");
	ct_set_gpio_ctrl( "PUDCR", "PA4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PA4");
	ct_set_gpio_ctrl( "PUDCR", "PA5", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PA5");
	ct_set_gpio_ctrl( "PUDCR", "PA5", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PA5");
	ct_set_gpio_ctrl( "PUDCR", "PA6", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PA6");
	ct_set_gpio_ctrl( "PUDCR", "PA6", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PA6");
	ct_set_gpio_ctrl( "PUDCR", "PA7", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PA7");
	ct_set_gpio_ctrl( "PUDCR", "PA7", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PA7");
	ct_set_gpio_ctrl( "PUDCR", "PB0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PB0");
	ct_set_gpio_ctrl( "PUDCR", "PB0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PB0");
	ct_set_gpio_ctrl( "PUDCR", "PB1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PB1");
	ct_set_gpio_ctrl( "PUDCR", "PB1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PB1");
	ct_set_gpio_ctrl( "PUDCR", "PB2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PB2");
	ct_set_gpio_ctrl( "PUDCR", "PB2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PB2");
	ct_set_gpio_ctrl( "PUDCR", "PB3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PB3");
	ct_set_gpio_ctrl( "PUDCR", "PB3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PB3");
	ct_set_gpio_ctrl( "PUDCR", "PB4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PB4");
	ct_set_gpio_ctrl( "PUDCR", "PB4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PB4");
	ct_set_gpio_ctrl( "PUDCR", "PD0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PD0");
	ct_set_gpio_ctrl( "PUDCR", "PD0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PD0");
	ct_set_gpio_ctrl( "PUDCR", "PD1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PD1");
	ct_set_gpio_ctrl( "PUDCR", "PD1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PD1");
	ct_set_gpio_ctrl( "PUDCR", "PD2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PD2");
	ct_set_gpio_ctrl( "PUDCR", "PD2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PD2");
	ct_set_gpio_ctrl( "PUDCR", "PD3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PD3");
	ct_set_gpio_ctrl( "PUDCR", "PD3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PD3");
	ct_set_gpio_ctrl( "PUDCR", "PD4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PD4");
	ct_set_gpio_ctrl( "PUDCR", "PD4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PD4");
	ct_set_gpio_ctrl( "PUDCR", "PD5", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PD5");
	ct_set_gpio_ctrl( "PUDCR", "PD5", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PD5");
	ct_set_gpio_ctrl( "PUDCR", "PD6", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PD6");
	ct_set_gpio_ctrl( "PUDCR", "PD6", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PD6");
//	ct_set_gpio_ctrl( "PUDCR", "PE0", 1 );
//	ct_get_gpio_ctrl( "PUDCR", "PE0");
//	ct_set_gpio_ctrl( "PUDCR", "PE0", 0 );
//	ct_get_gpio_ctrl( "PUDCR", "PE0");
//	ct_set_gpio_ctrl( "PUDCR", "PE1", 1 );
//	ct_get_gpio_ctrl( "PUDCR", "PE1");
//	ct_set_gpio_ctrl( "PUDCR", "PE1", 0 );
//	ct_get_gpio_ctrl( "PUDCR", "PE1");
//	ct_set_gpio_ctrl( "PUDCR", "PF2", 1 );
//	ct_get_gpio_ctrl( "PUDCR", "PF2");
//	ct_set_gpio_ctrl( "PUDCR", "PF2", 0 );
//	ct_get_gpio_ctrl( "PUDCR", "PF2");
//	ct_set_gpio_ctrl( "PUDCR", "PF3", 1 );
//	ct_get_gpio_ctrl( "PUDCR", "PF3");
//	ct_set_gpio_ctrl( "PUDCR", "PF3", 0 );
//	ct_get_gpio_ctrl( "PUDCR", "PF3");
//	ct_set_gpio_ctrl( "PUDCR", "PF4", 1 );
//	ct_get_gpio_ctrl( "PUDCR", "PF4");
//	ct_set_gpio_ctrl( "PUDCR", "PF4", 0 );
//	ct_get_gpio_ctrl( "PUDCR", "PF4");
	ct_set_gpio_ctrl( "PUDCR", "PG0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PG0");
	ct_set_gpio_ctrl( "PUDCR", "PG0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PG0");
	ct_set_gpio_ctrl( "PUDCR", "PG1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PG1");
	ct_set_gpio_ctrl( "PUDCR", "PG1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PG1");
	ct_set_gpio_ctrl( "PUDCR", "PG2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PG2");
	ct_set_gpio_ctrl( "PUDCR", "PG2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PG2");
	ct_set_gpio_ctrl( "PUDCR", "PG3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PG3");
	ct_set_gpio_ctrl( "PUDCR", "PG3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PG3");
	ct_set_gpio_ctrl( "PUDCR", "PG4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PG4");
	ct_set_gpio_ctrl( "PUDCR", "PG4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PG4");
	ct_set_gpio_ctrl( "PUDCR", "PG5", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PG5");
	ct_set_gpio_ctrl( "PUDCR", "PG5", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PG5");
	ct_set_gpio_ctrl( "PUDCR", "PG6", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PG6");
	ct_set_gpio_ctrl( "PUDCR", "PG6", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PG6");
	ct_set_gpio_ctrl( "PUDCR", "PG7", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PG7");
	ct_set_gpio_ctrl( "PUDCR", "PG7", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PG7");
	ct_set_gpio_ctrl( "PUDCR", "PH0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PH0");
	ct_set_gpio_ctrl( "PUDCR", "PH0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PH0");
	ct_set_gpio_ctrl( "PUDCR", "PH1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PH1");
	ct_set_gpio_ctrl( "PUDCR", "PH1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PH1");
	ct_set_gpio_ctrl( "PUDCR", "PH2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PH2");
	ct_set_gpio_ctrl( "PUDCR", "PH2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PH2");
	ct_set_gpio_ctrl( "PUDCR", "PH3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PH3");
	ct_set_gpio_ctrl( "PUDCR", "PH3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PH3");
	ct_set_gpio_ctrl( "PUDCR", "PW0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PW0");
	ct_set_gpio_ctrl( "PUDCR", "PW0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PW0");
	ct_set_gpio_ctrl( "PUDCR", "PW1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PW1");
	ct_set_gpio_ctrl( "PUDCR", "PW1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PW1");
	ct_set_gpio_ctrl( "PUDCR", "PW2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PW2");
	ct_set_gpio_ctrl( "PUDCR", "PW2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PW2");
	ct_set_gpio_ctrl( "PUDCR", "PW3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PW3");
	ct_set_gpio_ctrl( "PUDCR", "PW3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PW3");
	ct_set_gpio_ctrl( "PUDCR", "PW4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PW4");
	ct_set_gpio_ctrl( "PUDCR", "PW4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PW4");
	ct_set_gpio_ctrl( "PUDCR", "PW5", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PW5");
	ct_set_gpio_ctrl( "PUDCR", "PW5", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PW5");
	ct_set_gpio_ctrl( "PUDCR", "PW6", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PW6");
	ct_set_gpio_ctrl( "PUDCR", "PW6", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PW6");
	ct_set_gpio_ctrl( "PUDCR", "PW7", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PW7");
	ct_set_gpio_ctrl( "PUDCR", "PW7", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PW7");
	ct_set_gpio_ctrl( "PUDCR", "PJ0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PJ0");
	ct_set_gpio_ctrl( "PUDCR", "PJ0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PJ0");
	ct_set_gpio_ctrl( "PUDCR", "PJ1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PJ1");
	ct_set_gpio_ctrl( "PUDCR", "PJ1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PJ1");
	ct_set_gpio_ctrl( "PUDCR", "PJ2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PJ2");
	ct_set_gpio_ctrl( "PUDCR", "PJ2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PJ2");
	ct_set_gpio_ctrl( "PUDCR", "PJ3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PJ3");
	ct_set_gpio_ctrl( "PUDCR", "PJ3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PJ3");
	ct_set_gpio_ctrl( "PUDCR", "PJ4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PJ4");
	ct_set_gpio_ctrl( "PUDCR", "PJ4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PJ4");
	ct_set_gpio_ctrl( "PUDCR", "PJ5", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PJ5");
	ct_set_gpio_ctrl( "PUDCR", "PJ5", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PJ5");
	ct_set_gpio_ctrl( "PUDCR", "PJ6", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PJ6");
	ct_set_gpio_ctrl( "PUDCR", "PJ6", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PJ6");
	ct_set_gpio_ctrl( "PUDCR", "PJ7", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PJ7");
	ct_set_gpio_ctrl( "PUDCR", "PJ7", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PJ7");
	ct_set_gpio_ctrl( "PUDCR", "PK0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PK0");
	ct_set_gpio_ctrl( "PUDCR", "PK0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PK0");
	ct_set_gpio_ctrl( "PUDCR", "PK1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PK1");
	ct_set_gpio_ctrl( "PUDCR", "PK1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PK1");
	ct_set_gpio_ctrl( "PUDCR", "PK2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PK2");
	ct_set_gpio_ctrl( "PUDCR", "PK2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PK2");
	ct_set_gpio_ctrl( "PUDCR", "PK3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PK3");
	ct_set_gpio_ctrl( "PUDCR", "PK3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PK3");
	ct_set_gpio_ctrl( "PUDCR", "PK4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PK4");
	ct_set_gpio_ctrl( "PUDCR", "PK4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PK4");
	ct_set_gpio_ctrl( "PUDCR", "PK5", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PK5");
	ct_set_gpio_ctrl( "PUDCR", "PK5", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PK5");
	ct_set_gpio_ctrl( "PUDCR", "PK6", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PK6");
	ct_set_gpio_ctrl( "PUDCR", "PK6", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PK6");
	ct_set_gpio_ctrl( "PUDCR", "PK7", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PK7");
	ct_set_gpio_ctrl( "PUDCR", "PK7", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PK7");
	ct_set_gpio_ctrl( "PUDCR", "PL0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PL0");
	ct_set_gpio_ctrl( "PUDCR", "PL0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PL0");
	ct_set_gpio_ctrl( "PUDCR", "PL1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PL1");
	ct_set_gpio_ctrl( "PUDCR", "PL1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PL1");
	ct_set_gpio_ctrl( "PUDCR", "PL2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PL2");
	ct_set_gpio_ctrl( "PUDCR", "PL2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PL2");
	ct_set_gpio_ctrl( "PUDCR", "PL3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PL3");
	ct_set_gpio_ctrl( "PUDCR", "PL3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PL3");
	ct_set_gpio_ctrl( "PUDCR", "PL4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PL4");
	ct_set_gpio_ctrl( "PUDCR", "PL4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PL4");
	ct_set_gpio_ctrl( "PUDCR", "PL5", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PL5");
	ct_set_gpio_ctrl( "PUDCR", "PL5", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PL5");
	ct_set_gpio_ctrl( "PUDCR", "PM0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PM0");
	ct_set_gpio_ctrl( "PUDCR", "PM0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PM0");
	ct_set_gpio_ctrl( "PUDCR", "PM1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PM1");
	ct_set_gpio_ctrl( "PUDCR", "PM1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PM1");
	ct_set_gpio_ctrl( "PUDCR", "PM3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PM3");
	ct_set_gpio_ctrl( "PUDCR", "PM3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PM3");
	ct_set_gpio_ctrl( "PUDCR", "PM4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PM4");
	ct_set_gpio_ctrl( "PUDCR", "PM4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PM4");
	ct_set_gpio_ctrl( "PUDCR", "PN0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PN0");
	ct_set_gpio_ctrl( "PUDCR", "PN0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PN0");
	ct_set_gpio_ctrl( "PUDCR", "PN1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PN1");
	ct_set_gpio_ctrl( "PUDCR", "PN1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PN1");
	ct_set_gpio_ctrl( "PUDCR", "PN3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PN3");
	ct_set_gpio_ctrl( "PUDCR", "PN3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PN3");
	ct_set_gpio_ctrl( "PUDCR", "PN4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PN4");
	ct_set_gpio_ctrl( "PUDCR", "PN4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PN4");
	ct_set_gpio_ctrl( "PUDCR", "PY0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PY0");
	ct_set_gpio_ctrl( "PUDCR", "PY0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PY0");
	ct_set_gpio_ctrl( "PUDCR", "PY1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PY1");
	ct_set_gpio_ctrl( "PUDCR", "PY1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PY1");
	ct_set_gpio_ctrl( "PUDCR", "PY2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PY2");
	ct_set_gpio_ctrl( "PUDCR", "PY2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PY2");
	ct_set_gpio_ctrl( "PUDCR", "PY3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PY3");
	ct_set_gpio_ctrl( "PUDCR", "PY3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PY3");
	ct_set_gpio_ctrl( "PUDCR", "PY4", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PY4");
	ct_set_gpio_ctrl( "PUDCR", "PY4", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PY4");
	ct_set_gpio_ctrl( "PUDCR", "PY5", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PY5");
	ct_set_gpio_ctrl( "PUDCR", "PY5", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PY5");
	ct_set_gpio_ctrl( "PUDCR", "PY6", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PY6");
	ct_set_gpio_ctrl( "PUDCR", "PY6", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PY6");
	ct_set_gpio_ctrl( "PUDCR", "PY7", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PY7");
	ct_set_gpio_ctrl( "PUDCR", "PY7", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PY7");
	ct_set_gpio_ctrl( "PUDCR", "PP0", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PP0");
	ct_set_gpio_ctrl( "PUDCR", "PP0", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PP0");
	ct_set_gpio_ctrl( "PUDCR", "PP1", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PP1");
	ct_set_gpio_ctrl( "PUDCR", "PP1", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PP1");
	ct_set_gpio_ctrl( "PUDCR", "PP2", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PP2");
	ct_set_gpio_ctrl( "PUDCR", "PP2", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PP2");
	ct_set_gpio_ctrl( "PUDCR", "PP3", 1 );
	ct_get_gpio_ctrl( "PUDCR", "PP3");
	ct_set_gpio_ctrl( "PUDCR", "PP3", 0 );
	ct_get_gpio_ctrl( "PUDCR", "PP3");

#if 1
	/* PDR */
	ct_set_gpio_ctrl( "PDR", "P60", 1 );
	ct_get_gpio_ctrl( "PDR", "P60");
	ct_set_gpio_ctrl( "PDR", "P60", 0 );
	ct_get_gpio_ctrl( "PDR", "P60");
	ct_set_gpio_ctrl( "PDR", "P61", 1 );
	ct_get_gpio_ctrl( "PDR", "P61");
	ct_set_gpio_ctrl( "PDR", "P61", 0 );
	ct_get_gpio_ctrl( "PDR", "P61");
	ct_set_gpio_ctrl( "PDR", "P62", 1 );
	ct_get_gpio_ctrl( "PDR", "P62");
	ct_set_gpio_ctrl( "PDR", "P62", 0 );
	ct_get_gpio_ctrl( "PDR", "P62");
	ct_set_gpio_ctrl( "PDR", "P63", 1 );
	ct_get_gpio_ctrl( "PDR", "P63");
	ct_set_gpio_ctrl( "PDR", "P63", 0 );
	ct_get_gpio_ctrl( "PDR", "P63");
	ct_set_gpio_ctrl( "PDR", "P64", 1 );
	ct_get_gpio_ctrl( "PDR", "P64");
	ct_set_gpio_ctrl( "PDR", "P64", 0 );
	ct_get_gpio_ctrl( "PDR", "P64");
	ct_set_gpio_ctrl( "PDR", "P65", 1 );
	ct_get_gpio_ctrl( "PDR", "P65");
	ct_set_gpio_ctrl( "PDR", "P65", 0 );
	ct_get_gpio_ctrl( "PDR", "P65");
	ct_set_gpio_ctrl( "PDR", "P66", 1 );
	ct_get_gpio_ctrl( "PDR", "P66");
	ct_set_gpio_ctrl( "PDR", "P66", 0 );
	ct_get_gpio_ctrl( "PDR", "P66");
	ct_set_gpio_ctrl( "PDR", "P67", 1 );
	ct_get_gpio_ctrl( "PDR", "P67");
	ct_set_gpio_ctrl( "PDR", "P67", 0 );
	ct_get_gpio_ctrl( "PDR", "P67");
	ct_set_gpio_ctrl( "PDR", "P70", 1 );
	ct_get_gpio_ctrl( "PDR", "P70");
	ct_set_gpio_ctrl( "PDR", "P70", 0 );
	ct_get_gpio_ctrl( "PDR", "P70");
	ct_set_gpio_ctrl( "PDR", "P71", 1 );
	ct_get_gpio_ctrl( "PDR", "P71");
	ct_set_gpio_ctrl( "PDR", "P71", 0 );
	ct_get_gpio_ctrl( "PDR", "P71");
	ct_set_gpio_ctrl( "PDR", "P72", 1 );
	ct_get_gpio_ctrl( "PDR", "P72");
	ct_set_gpio_ctrl( "PDR", "P72", 0 );
	ct_get_gpio_ctrl( "PDR", "P72");
	ct_set_gpio_ctrl( "PDR", "P73", 1 );
	ct_get_gpio_ctrl( "PDR", "P73");
	ct_set_gpio_ctrl( "PDR", "P73", 0 );
	ct_get_gpio_ctrl( "PDR", "P73");
	ct_set_gpio_ctrl( "PDR", "P74", 1 );
	ct_get_gpio_ctrl( "PDR", "P74");
	ct_set_gpio_ctrl( "PDR", "P74", 0 );
	ct_get_gpio_ctrl( "PDR", "P74");
	ct_set_gpio_ctrl( "PDR", "P75", 1 );
	ct_get_gpio_ctrl( "PDR", "P75");
	ct_set_gpio_ctrl( "PDR", "P75", 0 );
	ct_get_gpio_ctrl( "PDR", "P75");
	ct_set_gpio_ctrl( "PDR", "P76", 1 );
	ct_get_gpio_ctrl( "PDR", "P76");
	ct_set_gpio_ctrl( "PDR", "P76", 0 );
	ct_get_gpio_ctrl( "PDR", "P76");
	ct_set_gpio_ctrl( "PDR", "P77", 1 );
	ct_get_gpio_ctrl( "PDR", "P77");
	ct_set_gpio_ctrl( "PDR", "P77", 0 );
	ct_get_gpio_ctrl( "PDR", "P77");
	ct_set_gpio_ctrl( "PDR", "P80", 1 );
	ct_get_gpio_ctrl( "PDR", "P80");
	ct_set_gpio_ctrl( "PDR", "P80", 0 );
	ct_get_gpio_ctrl( "PDR", "P80");
	ct_set_gpio_ctrl( "PDR", "P81", 1 );
	ct_get_gpio_ctrl( "PDR", "P81");
	ct_set_gpio_ctrl( "PDR", "P81", 0 );
	ct_get_gpio_ctrl( "PDR", "P81");
	ct_set_gpio_ctrl( "PDR", "P82", 1 );
	ct_get_gpio_ctrl( "PDR", "P82");
	ct_set_gpio_ctrl( "PDR", "P82", 0 );
	ct_get_gpio_ctrl( "PDR", "P82");
	ct_set_gpio_ctrl( "PDR", "P83", 1 );
	ct_get_gpio_ctrl( "PDR", "P83");
	ct_set_gpio_ctrl( "PDR", "P83", 0 );
	ct_get_gpio_ctrl( "PDR", "P83");
	ct_set_gpio_ctrl( "PDR", "P84", 1 );
	ct_get_gpio_ctrl( "PDR", "P84");
	ct_set_gpio_ctrl( "PDR", "P84", 0 );
	ct_get_gpio_ctrl( "PDR", "P84");
	ct_set_gpio_ctrl( "PDR", "P85", 1 );
	ct_get_gpio_ctrl( "PDR", "P85");
	ct_set_gpio_ctrl( "PDR", "P85", 0 );
	ct_get_gpio_ctrl( "PDR", "P85");
	ct_set_gpio_ctrl( "PDR", "P86", 1 );
	ct_get_gpio_ctrl( "PDR", "P86");
	ct_set_gpio_ctrl( "PDR", "P86", 0 );
	ct_get_gpio_ctrl( "PDR", "P86");
	ct_set_gpio_ctrl( "PDR", "P87", 1 );
	ct_get_gpio_ctrl( "PDR", "P87");
	ct_set_gpio_ctrl( "PDR", "P87", 0 );
	ct_get_gpio_ctrl( "PDR", "P87");
	ct_set_gpio_ctrl( "PDR", "P90", 1 );
	ct_get_gpio_ctrl( "PDR", "P90");
	ct_set_gpio_ctrl( "PDR", "P90", 0 );
	ct_get_gpio_ctrl( "PDR", "P90");
	ct_set_gpio_ctrl( "PDR", "P91", 1 );
	ct_get_gpio_ctrl( "PDR", "P91");
	ct_set_gpio_ctrl( "PDR", "P91", 0 );
	ct_get_gpio_ctrl( "PDR", "P91");
	ct_set_gpio_ctrl( "PDR", "P92", 1 );
	ct_get_gpio_ctrl( "PDR", "P92");
	ct_set_gpio_ctrl( "PDR", "P92", 0 );
	ct_get_gpio_ctrl( "PDR", "P92");
	ct_set_gpio_ctrl( "PDR", "P93", 1 );
	ct_get_gpio_ctrl( "PDR", "P93");
	ct_set_gpio_ctrl( "PDR", "P93", 0 );
	ct_get_gpio_ctrl( "PDR", "P93");
	ct_set_gpio_ctrl( "PDR", "PA0", 1 );
	ct_get_gpio_ctrl( "PDR", "PA0");
	ct_set_gpio_ctrl( "PDR", "PA0", 0 );
	ct_get_gpio_ctrl( "PDR", "PA0");
	ct_set_gpio_ctrl( "PDR", "PA1", 1 );
	ct_get_gpio_ctrl( "PDR", "PA1");
	ct_set_gpio_ctrl( "PDR", "PA1", 0 );
	ct_get_gpio_ctrl( "PDR", "PA1");
	ct_set_gpio_ctrl( "PDR", "PA2", 1 );
	ct_get_gpio_ctrl( "PDR", "PA2");
	ct_set_gpio_ctrl( "PDR", "PA2", 0 );
	ct_get_gpio_ctrl( "PDR", "PA2");
	ct_set_gpio_ctrl( "PDR", "PA3", 1 );
	ct_get_gpio_ctrl( "PDR", "PA3");
	ct_set_gpio_ctrl( "PDR", "PA3", 0 );
	ct_get_gpio_ctrl( "PDR", "PA3");
	ct_set_gpio_ctrl( "PDR", "PA4", 1 );
	ct_get_gpio_ctrl( "PDR", "PA4");
	ct_set_gpio_ctrl( "PDR", "PA4", 0 );
	ct_get_gpio_ctrl( "PDR", "PA4");
	ct_set_gpio_ctrl( "PDR", "PA5", 1 );
	ct_get_gpio_ctrl( "PDR", "PA5");
	ct_set_gpio_ctrl( "PDR", "PA5", 0 );
	ct_get_gpio_ctrl( "PDR", "PA5");
	ct_set_gpio_ctrl( "PDR", "PA6", 1 );
	ct_get_gpio_ctrl( "PDR", "PA6");
	ct_set_gpio_ctrl( "PDR", "PA6", 0 );
	ct_get_gpio_ctrl( "PDR", "PA6");
	ct_set_gpio_ctrl( "PDR", "PA7", 1 );
	ct_get_gpio_ctrl( "PDR", "PA7");
	ct_set_gpio_ctrl( "PDR", "PA7", 0 );
	ct_get_gpio_ctrl( "PDR", "PA7");
	ct_set_gpio_ctrl( "PDR", "PB0", 1 );
	ct_get_gpio_ctrl( "PDR", "PB0");
	ct_set_gpio_ctrl( "PDR", "PB0", 0 );
	ct_get_gpio_ctrl( "PDR", "PB0");
	ct_set_gpio_ctrl( "PDR", "PB1", 1 );
	ct_get_gpio_ctrl( "PDR", "PB1");
	ct_set_gpio_ctrl( "PDR", "PB1", 0 );
	ct_get_gpio_ctrl( "PDR", "PB1");
	ct_set_gpio_ctrl( "PDR", "PB2", 1 );
	ct_get_gpio_ctrl( "PDR", "PB2");
	ct_set_gpio_ctrl( "PDR", "PB2", 0 );
	ct_get_gpio_ctrl( "PDR", "PB2");
	ct_set_gpio_ctrl( "PDR", "PB3", 1 );
	ct_get_gpio_ctrl( "PDR", "PB3");
	ct_set_gpio_ctrl( "PDR", "PB3", 0 );
	ct_get_gpio_ctrl( "PDR", "PB3");
	ct_set_gpio_ctrl( "PDR", "PB4", 1 );
	ct_get_gpio_ctrl( "PDR", "PB4");
	ct_set_gpio_ctrl( "PDR", "PB4", 0 );
	ct_get_gpio_ctrl( "PDR", "PB4");
	ct_set_gpio_ctrl( "PDR", "PC0", 1 );
	ct_get_gpio_ctrl( "PDR", "PC0");
	ct_set_gpio_ctrl( "PDR", "PC0", 0 );
	ct_get_gpio_ctrl( "PDR", "PC0");
	ct_set_gpio_ctrl( "PDR", "PC1", 1 );
	ct_get_gpio_ctrl( "PDR", "PC1");
	ct_set_gpio_ctrl( "PDR", "PC1", 0 );
	ct_get_gpio_ctrl( "PDR", "PC1");
	ct_set_gpio_ctrl( "PDR", "PC2", 1 );
	ct_get_gpio_ctrl( "PDR", "PC2");
	ct_set_gpio_ctrl( "PDR", "PC2", 0 );
	ct_get_gpio_ctrl( "PDR", "PC2");
	ct_set_gpio_ctrl( "PDR", "PC3", 1 );
	ct_get_gpio_ctrl( "PDR", "PC3");
	ct_set_gpio_ctrl( "PDR", "PC3", 0 );
	ct_get_gpio_ctrl( "PDR", "PC3");
	ct_set_gpio_ctrl( "PDR", "PC4", 1 );
	ct_get_gpio_ctrl( "PDR", "PC4");
	ct_set_gpio_ctrl( "PDR", "PC4", 0 );
	ct_get_gpio_ctrl( "PDR", "PC4");
	ct_set_gpio_ctrl( "PDR", "PC5", 1 );
	ct_get_gpio_ctrl( "PDR", "PC5");
	ct_set_gpio_ctrl( "PDR", "PC5", 0 );
	ct_get_gpio_ctrl( "PDR", "PC5");
	ct_set_gpio_ctrl( "PDR", "PD0", 1 );
	ct_get_gpio_ctrl( "PDR", "PD0");
	ct_set_gpio_ctrl( "PDR", "PD0", 0 );
	ct_get_gpio_ctrl( "PDR", "PD0");
	ct_set_gpio_ctrl( "PDR", "PD1", 1 );
	ct_get_gpio_ctrl( "PDR", "PD1");
	ct_set_gpio_ctrl( "PDR", "PD1", 0 );
	ct_get_gpio_ctrl( "PDR", "PD1");
	ct_set_gpio_ctrl( "PDR", "PD2", 1 );
	ct_get_gpio_ctrl( "PDR", "PD2");
	ct_set_gpio_ctrl( "PDR", "PD2", 0 );
	ct_get_gpio_ctrl( "PDR", "PD2");
	ct_set_gpio_ctrl( "PDR", "PD3", 1 );
	ct_get_gpio_ctrl( "PDR", "PD3");
	ct_set_gpio_ctrl( "PDR", "PD3", 0 );
	ct_get_gpio_ctrl( "PDR", "PD3");
	ct_set_gpio_ctrl( "PDR", "PD4", 1 );
	ct_get_gpio_ctrl( "PDR", "PD4");
	ct_set_gpio_ctrl( "PDR", "PD4", 0 );
	ct_get_gpio_ctrl( "PDR", "PD4");
	ct_set_gpio_ctrl( "PDR", "PD5", 1 );
	ct_get_gpio_ctrl( "PDR", "PD5");
	ct_set_gpio_ctrl( "PDR", "PD5", 0 );
	ct_get_gpio_ctrl( "PDR", "PD5");
	ct_set_gpio_ctrl( "PDR", "PD6", 1 );
	ct_get_gpio_ctrl( "PDR", "PD6");
	ct_set_gpio_ctrl( "PDR", "PD6", 0 );
	ct_get_gpio_ctrl( "PDR", "PD6");
//	ct_set_gpio_ctrl( "PDR", "PE0", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE0");
//	ct_set_gpio_ctrl( "PDR", "PE0", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE0");
//	ct_set_gpio_ctrl( "PDR", "PE1", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE1");
//	ct_set_gpio_ctrl( "PDR", "PE1", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE1");
//	ct_set_gpio_ctrl( "PDR", "PE2", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE2");
//	ct_set_gpio_ctrl( "PDR", "PE2", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE2");
//	ct_set_gpio_ctrl( "PDR", "PE3", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE3");
//	ct_set_gpio_ctrl( "PDR", "PE3", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE3");
//	ct_set_gpio_ctrl( "PDR", "PE4", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE4");
//	ct_set_gpio_ctrl( "PDR", "PE4", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE4");
//	ct_set_gpio_ctrl( "PDR", "PE5", 1 );
//	ct_get_gpio_ctrl( "PDR", "PE5");
//	ct_set_gpio_ctrl( "PDR", "PE5", 0 );
//	ct_get_gpio_ctrl( "PDR", "PE5");
//	ct_set_gpio_ctrl( "PDR", "PF0", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF0");
//	ct_set_gpio_ctrl( "PDR", "PF0", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF0");
//	ct_set_gpio_ctrl( "PDR", "PF1", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF1");
//	ct_set_gpio_ctrl( "PDR", "PF1", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF1");
//	ct_set_gpio_ctrl( "PDR", "PF2", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF2");
//	ct_set_gpio_ctrl( "PDR", "PF2", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF2");
//	ct_set_gpio_ctrl( "PDR", "PF3", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF3");
//	ct_set_gpio_ctrl( "PDR", "PF3", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF3");
//	ct_set_gpio_ctrl( "PDR", "PF4", 1 );
//	ct_get_gpio_ctrl( "PDR", "PF4");
//	ct_set_gpio_ctrl( "PDR", "PF4", 0 );
//	ct_get_gpio_ctrl( "PDR", "PF4");
	ct_set_gpio_ctrl( "PDR", "PG0", 1 );
	ct_get_gpio_ctrl( "PDR", "PG0");
	ct_set_gpio_ctrl( "PDR", "PG0", 0 );
	ct_get_gpio_ctrl( "PDR", "PG0");
	ct_set_gpio_ctrl( "PDR", "PG1", 1 );
	ct_get_gpio_ctrl( "PDR", "PG1");
	ct_set_gpio_ctrl( "PDR", "PG1", 0 );
	ct_get_gpio_ctrl( "PDR", "PG1");
	ct_set_gpio_ctrl( "PDR", "PG2", 1 );
	ct_get_gpio_ctrl( "PDR", "PG2");
	ct_set_gpio_ctrl( "PDR", "PG2", 0 );
	ct_get_gpio_ctrl( "PDR", "PG2");
	ct_set_gpio_ctrl( "PDR", "PG3", 1 );
	ct_get_gpio_ctrl( "PDR", "PG3");
	ct_set_gpio_ctrl( "PDR", "PG3", 0 );
	ct_get_gpio_ctrl( "PDR", "PG3");
	ct_set_gpio_ctrl( "PDR", "PG4", 1 );
	ct_get_gpio_ctrl( "PDR", "PG4");
	ct_set_gpio_ctrl( "PDR", "PG4", 0 );
	ct_get_gpio_ctrl( "PDR", "PG4");
	ct_set_gpio_ctrl( "PDR", "PG5", 1 );
	ct_get_gpio_ctrl( "PDR", "PG5");
	ct_set_gpio_ctrl( "PDR", "PG5", 0 );
	ct_get_gpio_ctrl( "PDR", "PG5");
	ct_set_gpio_ctrl( "PDR", "PG6", 1 );
	ct_get_gpio_ctrl( "PDR", "PG6");
	ct_set_gpio_ctrl( "PDR", "PG6", 0 );
	ct_get_gpio_ctrl( "PDR", "PG6");
	ct_set_gpio_ctrl( "PDR", "PG7", 1 );
	ct_get_gpio_ctrl( "PDR", "PG7");
	ct_set_gpio_ctrl( "PDR", "PG7", 0 );
	ct_get_gpio_ctrl( "PDR", "PG7");
	ct_set_gpio_ctrl( "PDR", "PH0", 1 );
	ct_get_gpio_ctrl( "PDR", "PH0");
	ct_set_gpio_ctrl( "PDR", "PH0", 0 );
	ct_get_gpio_ctrl( "PDR", "PH0");
	ct_set_gpio_ctrl( "PDR", "PH1", 1 );
	ct_get_gpio_ctrl( "PDR", "PH1");
	ct_set_gpio_ctrl( "PDR", "PH1", 0 );
	ct_get_gpio_ctrl( "PDR", "PH1");
	ct_set_gpio_ctrl( "PDR", "PH2", 1 );
	ct_get_gpio_ctrl( "PDR", "PH2");
	ct_set_gpio_ctrl( "PDR", "PH2", 0 );
	ct_get_gpio_ctrl( "PDR", "PH2");
	ct_set_gpio_ctrl( "PDR", "PH3", 1 );
	ct_get_gpio_ctrl( "PDR", "PH3");
	ct_set_gpio_ctrl( "PDR", "PH3", 0 );
	ct_get_gpio_ctrl( "PDR", "PH3");
	ct_set_gpio_ctrl( "PDR", "PW0", 1 );
	ct_get_gpio_ctrl( "PDR", "PW0");
	ct_set_gpio_ctrl( "PDR", "PW0", 0 );
	ct_get_gpio_ctrl( "PDR", "PW0");
	ct_set_gpio_ctrl( "PDR", "PW1", 1 );
	ct_get_gpio_ctrl( "PDR", "PW1");
	ct_set_gpio_ctrl( "PDR", "PW1", 0 );
	ct_get_gpio_ctrl( "PDR", "PW1");
	ct_set_gpio_ctrl( "PDR", "PW2", 1 );
	ct_get_gpio_ctrl( "PDR", "PW2");
	ct_set_gpio_ctrl( "PDR", "PW2", 0 );
	ct_get_gpio_ctrl( "PDR", "PW2");
	ct_set_gpio_ctrl( "PDR", "PW3", 1 );
	ct_get_gpio_ctrl( "PDR", "PW3");
	ct_set_gpio_ctrl( "PDR", "PW3", 0 );
	ct_get_gpio_ctrl( "PDR", "PW3");
	ct_set_gpio_ctrl( "PDR", "PW4", 1 );
	ct_get_gpio_ctrl( "PDR", "PW4");
	ct_set_gpio_ctrl( "PDR", "PW4", 0 );
	ct_get_gpio_ctrl( "PDR", "PW4");
	ct_set_gpio_ctrl( "PDR", "PW5", 1 );
	ct_get_gpio_ctrl( "PDR", "PW5");
	ct_set_gpio_ctrl( "PDR", "PW5", 0 );
	ct_get_gpio_ctrl( "PDR", "PW5");
	ct_set_gpio_ctrl( "PDR", "PW6", 1 );
	ct_get_gpio_ctrl( "PDR", "PW6");
	ct_set_gpio_ctrl( "PDR", "PW6", 0 );
	ct_get_gpio_ctrl( "PDR", "PW6");
	ct_set_gpio_ctrl( "PDR", "PW7", 1 );
	ct_get_gpio_ctrl( "PDR", "PW7");
	ct_set_gpio_ctrl( "PDR", "PW7", 0 );
	ct_get_gpio_ctrl( "PDR", "PW7");
	ct_set_gpio_ctrl( "PDR", "PJ0", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ0");
	ct_set_gpio_ctrl( "PDR", "PJ0", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ0");
	ct_set_gpio_ctrl( "PDR", "PJ1", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ1");
	ct_set_gpio_ctrl( "PDR", "PJ1", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ1");
	ct_set_gpio_ctrl( "PDR", "PJ2", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ2");
	ct_set_gpio_ctrl( "PDR", "PJ2", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ2");
	ct_set_gpio_ctrl( "PDR", "PJ3", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ3");
	ct_set_gpio_ctrl( "PDR", "PJ3", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ3");
	ct_set_gpio_ctrl( "PDR", "PJ4", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ4");
	ct_set_gpio_ctrl( "PDR", "PJ4", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ4");
	ct_set_gpio_ctrl( "PDR", "PJ5", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ5");
	ct_set_gpio_ctrl( "PDR", "PJ5", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ5");
	ct_set_gpio_ctrl( "PDR", "PJ6", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ6");
	ct_set_gpio_ctrl( "PDR", "PJ6", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ6");
	ct_set_gpio_ctrl( "PDR", "PJ7", 1 );
	ct_get_gpio_ctrl( "PDR", "PJ7");
	ct_set_gpio_ctrl( "PDR", "PJ7", 0 );
	ct_get_gpio_ctrl( "PDR", "PJ7");
	ct_set_gpio_ctrl( "PDR", "PK0", 1 );
	ct_get_gpio_ctrl( "PDR", "PK0");
	ct_set_gpio_ctrl( "PDR", "PK0", 0 );
	ct_get_gpio_ctrl( "PDR", "PK0");
	ct_set_gpio_ctrl( "PDR", "PK1", 1 );
	ct_get_gpio_ctrl( "PDR", "PK1");
	ct_set_gpio_ctrl( "PDR", "PK1", 0 );
	ct_get_gpio_ctrl( "PDR", "PK1");
	ct_set_gpio_ctrl( "PDR", "PK2", 1 );
	ct_get_gpio_ctrl( "PDR", "PK2");
	ct_set_gpio_ctrl( "PDR", "PK2", 0 );
	ct_get_gpio_ctrl( "PDR", "PK2");
	ct_set_gpio_ctrl( "PDR", "PK3", 1 );
	ct_get_gpio_ctrl( "PDR", "PK3");
	ct_set_gpio_ctrl( "PDR", "PK3", 0 );
	ct_get_gpio_ctrl( "PDR", "PK3");
	ct_set_gpio_ctrl( "PDR", "PK4", 1 );
	ct_get_gpio_ctrl( "PDR", "PK4");
	ct_set_gpio_ctrl( "PDR", "PK4", 0 );
	ct_get_gpio_ctrl( "PDR", "PK4");
	ct_set_gpio_ctrl( "PDR", "PK5", 1 );
	ct_get_gpio_ctrl( "PDR", "PK5");
	ct_set_gpio_ctrl( "PDR", "PK5", 0 );
	ct_get_gpio_ctrl( "PDR", "PK5");
	ct_set_gpio_ctrl( "PDR", "PK6", 1 );
	ct_get_gpio_ctrl( "PDR", "PK6");
	ct_set_gpio_ctrl( "PDR", "PK6", 0 );
	ct_get_gpio_ctrl( "PDR", "PK6");
	ct_set_gpio_ctrl( "PDR", "PK7", 1 );
	ct_get_gpio_ctrl( "PDR", "PK7");
	ct_set_gpio_ctrl( "PDR", "PK7", 0 );
	ct_get_gpio_ctrl( "PDR", "PK7");
	ct_set_gpio_ctrl( "PDR", "PL0", 1 );
	ct_get_gpio_ctrl( "PDR", "PL0");
	ct_set_gpio_ctrl( "PDR", "PL0", 0 );
	ct_get_gpio_ctrl( "PDR", "PL0");
	ct_set_gpio_ctrl( "PDR", "PL1", 1 );
	ct_get_gpio_ctrl( "PDR", "PL1");
	ct_set_gpio_ctrl( "PDR", "PL1", 0 );
	ct_get_gpio_ctrl( "PDR", "PL1");
	ct_set_gpio_ctrl( "PDR", "PL2", 1 );
	ct_get_gpio_ctrl( "PDR", "PL2");
	ct_set_gpio_ctrl( "PDR", "PL2", 0 );
	ct_get_gpio_ctrl( "PDR", "PL2");
	ct_set_gpio_ctrl( "PDR", "PL3", 1 );
	ct_get_gpio_ctrl( "PDR", "PL3");
	ct_set_gpio_ctrl( "PDR", "PL3", 0 );
	ct_get_gpio_ctrl( "PDR", "PL3");
	ct_set_gpio_ctrl( "PDR", "PL4", 1 );
	ct_get_gpio_ctrl( "PDR", "PL4");
	ct_set_gpio_ctrl( "PDR", "PL4", 0 );
	ct_get_gpio_ctrl( "PDR", "PL4");
	ct_set_gpio_ctrl( "PDR", "PL5", 1 );
	ct_get_gpio_ctrl( "PDR", "PL5");
	ct_set_gpio_ctrl( "PDR", "PL5", 0 );
	ct_get_gpio_ctrl( "PDR", "PL5");
	ct_set_gpio_ctrl( "PDR", "PM0", 1 );
	ct_get_gpio_ctrl( "PDR", "PM0");
	ct_set_gpio_ctrl( "PDR", "PM0", 0 );
	ct_get_gpio_ctrl( "PDR", "PM0");
	ct_set_gpio_ctrl( "PDR", "PM1", 1 );
	ct_get_gpio_ctrl( "PDR", "PM1");
	ct_set_gpio_ctrl( "PDR", "PM1", 0 );
	ct_get_gpio_ctrl( "PDR", "PM1");
	ct_set_gpio_ctrl( "PDR", "PM2", 1 );
	ct_get_gpio_ctrl( "PDR", "PM2");
	ct_set_gpio_ctrl( "PDR", "PM2", 0 );
	ct_get_gpio_ctrl( "PDR", "PM2");
	ct_set_gpio_ctrl( "PDR", "PM3", 1 );
	ct_get_gpio_ctrl( "PDR", "PM3");
	ct_set_gpio_ctrl( "PDR", "PM3", 0 );
	ct_get_gpio_ctrl( "PDR", "PM3");
	ct_set_gpio_ctrl( "PDR", "PM4", 1 );
	ct_get_gpio_ctrl( "PDR", "PM4");
	ct_set_gpio_ctrl( "PDR", "PM4", 0 );
	ct_get_gpio_ctrl( "PDR", "PM4");
	ct_set_gpio_ctrl( "PDR", "PN0", 1 );
	ct_get_gpio_ctrl( "PDR", "PN0");
	ct_set_gpio_ctrl( "PDR", "PN0", 0 );
	ct_get_gpio_ctrl( "PDR", "PN0");
	ct_set_gpio_ctrl( "PDR", "PN1", 1 );
	ct_get_gpio_ctrl( "PDR", "PN1");
	ct_set_gpio_ctrl( "PDR", "PN1", 0 );
	ct_get_gpio_ctrl( "PDR", "PN1");
	ct_set_gpio_ctrl( "PDR", "PN2", 1 );
	ct_get_gpio_ctrl( "PDR", "PN2");
	ct_set_gpio_ctrl( "PDR", "PN2", 0 );
	ct_get_gpio_ctrl( "PDR", "PN2");
	ct_set_gpio_ctrl( "PDR", "PN3", 1 );
	ct_get_gpio_ctrl( "PDR", "PN3");
	ct_set_gpio_ctrl( "PDR", "PN3", 0 );
	ct_get_gpio_ctrl( "PDR", "PN3");
	ct_set_gpio_ctrl( "PDR", "PN4", 1 );
	ct_get_gpio_ctrl( "PDR", "PN4");
	ct_set_gpio_ctrl( "PDR", "PN4", 0 );
	ct_get_gpio_ctrl( "PDR", "PN4");
	ct_set_gpio_ctrl( "PDR", "PY0", 1 );
	ct_get_gpio_ctrl( "PDR", "PY0");
	ct_set_gpio_ctrl( "PDR", "PY0", 0 );
	ct_get_gpio_ctrl( "PDR", "PY0");
	ct_set_gpio_ctrl( "PDR", "PY1", 1 );
	ct_get_gpio_ctrl( "PDR", "PY1");
	ct_set_gpio_ctrl( "PDR", "PY1", 0 );
	ct_get_gpio_ctrl( "PDR", "PY1");
	ct_set_gpio_ctrl( "PDR", "PY2", 1 );
	ct_get_gpio_ctrl( "PDR", "PY2");
	ct_set_gpio_ctrl( "PDR", "PY2", 0 );
	ct_get_gpio_ctrl( "PDR", "PY2");
	ct_set_gpio_ctrl( "PDR", "PY3", 1 );
	ct_get_gpio_ctrl( "PDR", "PY3");
	ct_set_gpio_ctrl( "PDR", "PY3", 0 );
	ct_get_gpio_ctrl( "PDR", "PY3");
	ct_set_gpio_ctrl( "PDR", "PY4", 1 );
	ct_get_gpio_ctrl( "PDR", "PY4");
	ct_set_gpio_ctrl( "PDR", "PY4", 0 );
	ct_get_gpio_ctrl( "PDR", "PY4");
	ct_set_gpio_ctrl( "PDR", "PY5", 1 );
	ct_get_gpio_ctrl( "PDR", "PY5");
	ct_set_gpio_ctrl( "PDR", "PY5", 0 );
	ct_get_gpio_ctrl( "PDR", "PY5");
	ct_set_gpio_ctrl( "PDR", "PY6", 1 );
	ct_get_gpio_ctrl( "PDR", "PY6");
	ct_set_gpio_ctrl( "PDR", "PY6", 0 );
	ct_get_gpio_ctrl( "PDR", "PY6");
	ct_set_gpio_ctrl( "PDR", "PY7", 1 );
	ct_get_gpio_ctrl( "PDR", "PY7");
	ct_set_gpio_ctrl( "PDR", "PY7", 0 );
	ct_get_gpio_ctrl( "PDR", "PY7");
	ct_set_gpio_ctrl( "PDR", "PP0", 1 );
	ct_get_gpio_ctrl( "PDR", "PP0");
	ct_set_gpio_ctrl( "PDR", "PP0", 0 );
	ct_get_gpio_ctrl( "PDR", "PP0");
	ct_set_gpio_ctrl( "PDR", "PP1", 1 );
	ct_get_gpio_ctrl( "PDR", "PP1");
	ct_set_gpio_ctrl( "PDR", "PP1", 0 );
	ct_get_gpio_ctrl( "PDR", "PP1");
	ct_set_gpio_ctrl( "PDR", "PP2", 1 );
	ct_get_gpio_ctrl( "PDR", "PP2");
	ct_set_gpio_ctrl( "PDR", "PP2", 0 );
	ct_get_gpio_ctrl( "PDR", "PP2");
	ct_set_gpio_ctrl( "PDR", "PP3", 1 );
	ct_get_gpio_ctrl( "PDR", "PP3");
	ct_set_gpio_ctrl( "PDR", "PP3", 0 );
	ct_get_gpio_ctrl( "PDR", "PP3");
#endif
}


static void ct_gpio_err( VOID )
{
	INT32 ret;
	E_DD_TOP_GPIO_PORT port;
	UCHAR val;

	// port error
	port = E_DD_TOP_GPIO_PORT_MAX;
	val = 0;
	ret = Dd_Top_Set_Gpio_Function( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Function(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Set_Gpio_Direction( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Direction(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Set_Gpio_Status( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Status(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Set_Gpio_Pull_Up_Down_Enable( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Pull_Up_Down_Enable(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Get_Gpio_Function( port, &val );
	Ddim_Print(("Dd_Top_Get_Gpio_Function(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Get_Gpio_Direction( port, &val );
	Ddim_Print(("Dd_Top_Get_Gpio_Direction(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Get_Gpio_Status( port, &val );
	Ddim_Print(("Dd_Top_Get_Gpio_Status(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Get_Gpio_Pull_Up_Down_Enable( port, &val );
	Ddim_Print(("Dd_Top_Get_Gpio_Pull_Up_Down_Enable(): port error. ret=%d, port=%d\n", ret, port));

	ret = Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl( port, &val );
	Ddim_Print(("Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl(): port error. ret=%d, port=%d\n", ret, port));


	// value error
	port = E_DD_TOP_GPIO_PP3;
	val = 2;
	ret = Dd_Top_Set_Gpio_Function( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Function(): value error. ret=%d, val=%d\n", ret, val));

	ret = Dd_Top_Set_Gpio_Direction( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Direction(): value error. ret=%d, val=%d\n", ret, val));

	ret = Dd_Top_Set_Gpio_Status( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Status(): value error. ret=%d, val=%d\n", ret, val));

	ret = Dd_Top_Set_Gpio_Pull_Up_Down_Enable( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Pull_Up_Down_Enable(): value error. ret=%d, val=%d\n", ret, val));

	ret = Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl( port, val );
	Ddim_Print(("Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(): value error. ret=%d, val=%d\n", ret, val));

	ret = Dd_Top_Get_Gpio_Function( port, NULL );
	Ddim_Print(("Dd_Top_Get_Gpio_Function(): value error. ret=%d, val=NULL\n", ret));

	ret = Dd_Top_Get_Gpio_Direction( port, NULL );
	Ddim_Print(("Dd_Top_Get_Gpio_Direction(): value error. ret=%d, val=NULL\n", ret));

	ret = Dd_Top_Get_Gpio_Status( port, NULL );
	Ddim_Print(("Dd_Top_Get_Gpio_Status(): value error. ret=%d, val=NULL\n", ret));

	ret = Dd_Top_Get_Gpio_Pull_Up_Down_Enable( port, NULL );
	Ddim_Print(("Dd_Top_Get_Gpio_Pull_Up_Down_Enable(): value error. ret=%d, val=NULL\n", ret));

	ret = Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl( port, NULL );
	Ddim_Print(("Dd_Top_Get_Gpio_Pull_Up_Down_Ctrl(): value error. ret=%d, val=NULL\n", ret));

}


static void ct_start_clock(int argc, char** argv)
{
	ULONG val;
	UCHAR count;

	if (argc == 6) {

		count = atoi(argv[4]);
		val = strtoul(argv[5], NULL, 0);
		
		if(val == 0 ) {
			Ddim_Print(("please check parameter!!\n"));
		}
		
		if( 0 == strcmp( argv[3], "clkstop1" ) ) {
			IO_CHIPTOP.CLKSTOP1.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP1(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP1=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP1.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop2" ) ) {
			IO_CHIPTOP.CLKSTOP2.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP2(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP2=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP2.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop3" ) ) {
			IO_CHIPTOP.CLKSTOP3.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP3(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP3=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP3.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop4" ) ) {
			IO_CHIPTOP.CLKSTOP4.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP4(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP4=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP4.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop5" ) ) {
			IO_CHIPTOP.CLKSTOP5.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP5(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP5=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP5.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop6" ) ) {
			IO_CHIPTOP.CLKSTOP6.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP6(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP6=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP6.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop7" ) ) {
			IO_CHIPTOP.CLKSTOP7.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP7(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP7=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP7.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop8" ) ) {
			IO_CHIPTOP.CLKSTOP8.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP8(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP8=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP8.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop9" ) ) {
			IO_CHIPTOP.CLKSTOP9.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP9(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP9=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP9.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop10" ) ) {
			IO_CHIPTOP.CLKSTOP10.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP10(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP10=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP10.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop11" ) ) {
			IO_CHIPTOP.CLKSTOP11.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP11(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP11=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP11.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop12" ) ) {
			IO_CHIPTOP.CLKSTOP12.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP12(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP12=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP12.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop13" ) ) {
			IO_CHIPTOP.CLKSTOP13.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP13(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP13=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP13.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop14" ) ) {
			IO_CHIPTOP.CLKSTOP14.word |= (0x02|val);	// Clock Stop for Test
			Dd_Top_Start_Clock(&count, &Dd_Top_Get_CLKSTOP14(), ~val);
			Ddim_Print(("Clock Start. Count=%d, CLKSTOP14=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP14.word));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_stop_clock(int argc, char** argv)
{
	ULONG val;
	UCHAR count;
	if (argc == 6) {
		count = atoi(argv[4]);
		val = strtoul(argv[5], NULL, 0);

		if(val == 0 ) {
			Ddim_Print(("please check parameter!!\n"));
		}

		if( 0 == strcmp( argv[3], "clkstop1" ) ) {
			IO_CHIPTOP.CLKSTOP1.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP1(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP1=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP1.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop2" ) ) {
			IO_CHIPTOP.CLKSTOP2.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP2(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP2=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP2.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop3" ) ) {
			IO_CHIPTOP.CLKSTOP3.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP3(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP3=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP3.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop4" ) ) {
			IO_CHIPTOP.CLKSTOP4.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP4(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP4=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP4.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop5" ) ) {
			IO_CHIPTOP.CLKSTOP5.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP5(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP5=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP5.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop6" ) ) {
			IO_CHIPTOP.CLKSTOP6.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP6(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP6=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP6.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop7" ) ) {
			IO_CHIPTOP.CLKSTOP7.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP7(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP7=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP7.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop8" ) ) {
			IO_CHIPTOP.CLKSTOP8.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP8(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP8=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP8.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop9" ) ) {
			IO_CHIPTOP.CLKSTOP9.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP9(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP9=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP9.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop10" ) ) {
			IO_CHIPTOP.CLKSTOP10.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP10(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP10=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP10.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop11" ) ) {
			IO_CHIPTOP.CLKSTOP11.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP11(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP11=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP11.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop12" ) ) {
			IO_CHIPTOP.CLKSTOP12.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP12(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP12=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP12.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop13" ) ) {
			IO_CHIPTOP.CLKSTOP13.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP13(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP13=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP13.word));
		}
		else if( 0 == strcmp( argv[3], "clkstop14" ) ) {
			IO_CHIPTOP.CLKSTOP14.word |= val;	// Clock Start for Test
			Dd_Top_Stop_Clock(&count, &Dd_Top_Get_CLKSTOP14(), val);
			Ddim_Print(("Clock Stop. Count=%d, CLKSTOP14=0x%08lX\n", count, IO_CHIPTOP.CLKSTOP14.word));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static INT32 ct_start_Pll00(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL00ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL00SEL = 0;
#endif
	ret = Dd_Top_Start_Pll00();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL00ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL00ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL00SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL00SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll00(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL00SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL00ST = 1;
#endif
	Dd_Top_Stop_Pll00();
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL00ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL00ST));
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL00SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL00SEL));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll01(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL01ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL01SEL = 0;
#endif
	ret = Dd_Top_Start_Pll01();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL01ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL01ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL01SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL01SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll01(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL01SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL01ST = 1;
#endif
	Dd_Top_Stop_Pll01();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL01SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL01SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL01ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL01ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll02(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL02ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL02SEL = 0;
#endif
	ret = Dd_Top_Start_Pll02();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL02ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL02ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL02SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL02SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll02(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL02SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL02ST = 1;
#endif
	Dd_Top_Stop_Pll02();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL02SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL02SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL02ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL02ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll03(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL03ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL03SEL = 0;
#endif
	ret = Dd_Top_Start_Pll03();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL03ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL03ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL03SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL03SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll03(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL03SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL03ST = 1;
#endif
	Dd_Top_Stop_Pll03();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL03SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL03SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL03ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL03ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll04(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL04ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL04SEL = 0;
#endif
	ret = Dd_Top_Start_Pll04();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL04ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL04ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL04SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL04SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll04(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL04SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL04ST = 1;
#endif
	Dd_Top_Stop_Pll04();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL04SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL04SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL04ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL04ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll05(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL05ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL05SEL = 0;
#endif
	ret = Dd_Top_Start_Pll05();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL05ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL05ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL05SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL05SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll05(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL05SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL05ST = 1;
#endif
	Dd_Top_Stop_Pll05();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL05SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL05SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL05ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL05ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll05A(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL05AST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL05ASEL = 0;
#endif
	ret = Dd_Top_Start_Pll05A();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL05AST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL05AST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL05ASEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL05ASEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll05A(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL05SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL05ST = 1;
#endif
	Dd_Top_Stop_Pll05A();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL05ASEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL05SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL05AST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL05ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll06(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL06ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL06SEL = 0;
#endif
	ret = Dd_Top_Start_Pll06();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL06ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL06ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL06SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL06SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll06(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL06SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL06ST = 1;
#endif
	Dd_Top_Stop_Pll06();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL06SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL06SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL06ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL06ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll07(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL07ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL07SEL = 0;
#endif
	ret = Dd_Top_Start_Pll07();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL07ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL07ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL07SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL07SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll07(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL07SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL07ST = 1;
#endif
	Dd_Top_Stop_Pll07();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL07SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL07SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL07ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL07ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll08(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL08ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL08SEL = 0;
#endif
	ret = Dd_Top_Start_Pll08();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL08ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL08ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL08SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL08SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll08(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL08SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL08ST = 1;
#endif
	Dd_Top_Stop_Pll08();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL08SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL08SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL08ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL08ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll10(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL10ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL10SEL = 0;
#endif
	ret = Dd_Top_Start_Pll10();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL10ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL10ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL10SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL10SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll10(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL10SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL10ST = 1;
#endif
	Dd_Top_Stop_Pll10();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL10SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL10SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL10ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL10ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll10A(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL10AST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL10ASEL = 0;
#endif
	ret = Dd_Top_Start_Pll10A();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL10AST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL10AST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL10ASEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL10ASEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll10A(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL10ASEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL10AST = 1;
#endif
	Dd_Top_Stop_Pll10A();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL10ASEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL10ASEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL10AST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL10AST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Pll11(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL11ST = 0;
	IO_CHIPTOP.PLLCNT2.bit.PL11SEL = 0;
#endif
	ret = Dd_Top_Start_Pll11();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL11ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL11ST));
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL11SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL11SEL));
	}
	return(ret);
}

static INT32 ct_stop_Pll11(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL11SEL = 1;
	IO_CHIPTOP.PLLCNT1.bit.PL11ST = 1;
#endif
	Dd_Top_Stop_Pll11();
	Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL11SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL11SEL));
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL11ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.PL11ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Ddr_Pll00(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL00ST = 0;
#endif
	ret = Dd_Top_Start_Ddr_Pll00();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL00ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL00ST));
	}
	return(ret);
}

static INT32 ct_stop_Ddr_Pll00(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL00ST = 1;
#endif
	Dd_Top_Stop_Ddr_Pll00();
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL00ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL00ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Ddr_Pll01(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL01ST = 0;
#endif
	ret = Dd_Top_Start_Ddr_Pll01();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL01ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL01ST));
	}
	return(ret);
}

static INT32 ct_stop_Ddr_Pll01(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL01ST = 1;
#endif
	Dd_Top_Stop_Ddr_Pll01();
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL01ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL01ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Ddr_Pll02(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL02ST = 0;
#endif
	ret = Dd_Top_Start_Ddr_Pll02();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL02ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL02ST));
	}
	return(ret);
}

static INT32 ct_stop_Ddr_Pll02(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL02ST = 1;
#endif
	Dd_Top_Stop_Ddr_Pll02();
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL02ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL02ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Ddr_Pll10(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL10ST = 0;
#endif
	ret = Dd_Top_Start_Ddr_Pll10();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL10ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL10ST));
	}
	return(ret);
}

static INT32 ct_stop_Ddr_Pll10(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL10ST = 1;
#endif
	Dd_Top_Stop_Ddr_Pll10();
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL10ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL10ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Ddr_Pll11(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL11ST = 0;
#endif
	ret = Dd_Top_Start_Ddr_Pll11();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL11ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL11ST));
	}
	return(ret);
}

static INT32 ct_stop_Ddr_Pll11(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL11ST = 1;
#endif
	Dd_Top_Stop_Ddr_Pll11();
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL11ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL11ST));
	return(D_DDIM_OK);
}

static INT32 ct_start_Ddr_Pll12(VOID)
{
	INT32 ret;

#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL12ST = 0;
#endif
	ret = Dd_Top_Start_Ddr_Pll12();
	if( ret == D_DDIM_OK ){
		Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL12ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL12ST));
	}
	return(ret);
}

static INT32 ct_stop_Ddr_Pll12(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.DPL12ST = 1;
#endif
	Dd_Top_Stop_Ddr_Pll12();
	Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL12ST = %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL12ST));
	return(D_DDIM_OK);
}

static void ct_set_pllcnt2_pl00sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL00SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL00SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL00SEL));
	}
}

static void ct_set_pllcnt2_pl01sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL01SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL01SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL01SEL));
	}
}

static void ct_set_pllcnt2_pl02sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL02SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL02SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL02SEL));
	}
}

static void ct_set_pllcnt2_pl03sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL03SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL03SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL03SEL));
	}
}

static void ct_set_pllcnt2_pl04sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL04SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL04SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL04SEL));
	}
}

static void ct_set_pllcnt2_pl05sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL05SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL05SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL05SEL));
	}
}

static void ct_set_pllcnt2_pl05asel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL05ASEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL05ASEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL05ASEL));
	}
}

static void ct_set_pllcnt2_pl06sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL06SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL06SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL06SEL));
	}
}

static void ct_set_pllcnt2_pl07sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL07SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL07SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL07SEL));
	}
}

static void ct_set_pllcnt2_pl08sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL08SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL08SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL08SEL));
	}
}

static void ct_set_pllcnt2_pl10sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL10SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL10SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL10SEL));
	}
}

static void ct_set_pllcnt2_pl10asel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL10ASEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL10ASEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL10ASEL));
	}
}

static void ct_set_pllcnt2_pl11sel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL11SEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL11SEL = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL11SEL));
	}
}

static void ct_set_pllcnt2_pl01ssen(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL01SSEN(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL01SSEN = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL01SSEN));
	}
}

static void ct_set_pllcnt2_pl02ssen(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT2_PL02SSEN(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL02SSEN = %d\n", IO_CHIPTOP.PLLCNT2.bit.PL02SSEN));
	}
}

static void ct_set_pllcnt3_p00postdiv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT3_P00POSTDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT3.bit.P00POSTDIV = %d\n", IO_CHIPTOP.PLLCNT3.bit.P00POSTDIV));
	}
}

static void ct_set_pllcnt3_p00prediv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT3_P00PREDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT3.bit.P00PREDIV = %d\n", IO_CHIPTOP.PLLCNT3.bit.P00PREDIV));
	}
}

static void ct_set_pllcnt3_p00plldiv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT3_P00PLLDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT3.bit.P00PLLDIV = %d\n", IO_CHIPTOP.PLLCNT3.bit.P00PLLDIV));
	}
}

static void ct_set_pllcnt4_p00fnum(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT4_P00FNUM(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT4.bit.P00FNUM = %lu\n", IO_CHIPTOP.PLLCNT4.bit.P00FNUM));
	}
}

static void ct_set_pllcnt5_p00fden(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT5_P00FDEN(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT5.bit.P00FDEN = %lu\n", IO_CHIPTOP.PLLCNT5.bit.P00FDEN));
	}
}

static void ct_set_pllcnt7_p03postdiv0(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_P03POSTDIV0(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV0 = %d\n", IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV0));
	}
}

static void ct_set_pllcnt7_p03postdiv1(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_P03POSTDIV1(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV1 = %d\n", IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV1));
	}
}

static void ct_set_pllcnt7_p03prediv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_P03PREDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03PREDIV = %d\n", IO_CHIPTOP.PLLCNT7.bit.P03PREDIV));
	}
}

static void ct_set_pllcnt7_p03plldiv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_P03PLLDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03PLLDIV = %d\n", IO_CHIPTOP.PLLCNT7.bit.P03PLLDIV));
	}
}

static void ct_set_pllcnt7_p03oc0(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_P03OC0(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03OC0 = %d\n", IO_CHIPTOP.PLLCNT7.bit.P03OC0));
	}
}

static void ct_set_pllcnt7_p03oc1(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_P03OC1(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03OC1 = %d\n", IO_CHIPTOP.PLLCNT7.bit.P03OC1));
	}
}

static void ct_set_pllcnt7_d0xoc(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_D0XOC(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.D0XOC = %d\n", IO_CHIPTOP.PLLCNT7.bit.D0XOC));
	}
}

static void ct_set_pllcnt7_d1xoc(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_D1XOC(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.D1XOC = %d\n", IO_CHIPTOP.PLLCNT7.bit.D1XOC));
	}
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
static void ct_set_pllcnt7_dplx8(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_DPLX8(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.DPLX8 = %d\n", IO_CHIPTOP.PLLCNT7.bit.DPLX8));
	}
}

// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
static void ct_set_pllcnt7_dsel(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT7_DSEL(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.DSEL = %d\n", IO_CHIPTOP.PLLCNT7.bit.DSEL));
	}
}

static void ct_set_pllcnt8_p04postdiv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT8_P04POSTDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT8.bit.P04POSTDIV = %d\n", IO_CHIPTOP.PLLCNT8.bit.P04POSTDIV));
	}
}

static void ct_set_pllcnt8_p04prediv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT8_P04PREDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT8.bit.P04PREDIV = %d\n", IO_CHIPTOP.PLLCNT8.bit.P04PREDIV));
	}
}

static void ct_set_pllcnt8_p04plldiv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT8_P04PLLDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT8.bit.P04PLLDIV = %d\n", IO_CHIPTOP.PLLCNT8.bit.P04PLLDIV));
	}
}

static void ct_set_pllcnt9_p10aplldiv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT9_P10APLLDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT9.bit.P10APLLDIV = %d\n", IO_CHIPTOP.PLLCNT9.bit.P10APLLDIV));
	}
}

static void ct_set_pllcnt9_p08plldiv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT9_P08PLLDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT9.bit.P08PLLDIV = %d\n", IO_CHIPTOP.PLLCNT9.bit.P08PLLDIV));
	}
}

static void ct_set_pllcnt9_p05chg(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLCNT9_P05CHG(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLCNT9.bit.P05CHG = %d\n", IO_CHIPTOP.PLLCNT9.bit.P05CHG));
	}
}

static void ct_set_pllfreq1_plldiv(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_PLLFREQ1_PLLIDIV(pll_select);
		Ddim_Print(("IO_CHIPTOP.PLLFREQ1.bit.PLLIDIV = %d\n", IO_CHIPTOP.PLLFREQ1.bit.PLLIDIV));
	}
}

static void ct_set_odivch0_odivch0num(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_ODIVCH0_ODIVCH0NUM(pll_select);
		Ddim_Print(("IO_CHIPTOP.ODIVCH0.bit.ODIVCH0NUM = %d\n", IO_CHIPTOP.ODIVCH0.bit.ODIVCH0NUM));
	}
}

static void ct_set_cmden_cmden(int argc, char **argv)
{
	UCHAR pll_select;

	if (argc == 4) {
		pll_select = atoi(argv[3]);
		Dd_Top_Set_CMDEN_CMDEN(pll_select);
		Ddim_Print(("IO_CHIPTOP.CMDEN.bit.CMDEN = %d\n", IO_CHIPTOP.CMDEN.bit.CMDEN));
	}
}

static void ct_pll_err()
{
	INT32 ret;

	ret = Dd_Top_Start_Pll00();
	Ddim_Print(("Dd_Top_Start_Pll00(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll01();
	Ddim_Print(("Dd_Top_Start_Pll01(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll02();
	Ddim_Print(("Dd_Top_Start_Pll02(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll03();
	Ddim_Print(("Dd_Top_Start_Pll03(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll04();
	Ddim_Print(("Dd_Top_Start_Pll04(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll05();
	Ddim_Print(("Dd_Top_Start_Pll05(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll05A();
	Ddim_Print(("Dd_Top_Start_Pll05A(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll06();
	Ddim_Print(("Dd_Top_Start_Pll06(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll07();
	Ddim_Print(("Dd_Top_Start_Pll07(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll08();
	Ddim_Print(("Dd_Top_Start_Pll08(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll10();
	Ddim_Print(("Dd_Top_Start_Pll10(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll10A();
	Ddim_Print(("Dd_Top_Start_Pll10A(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Pll11();
	Ddim_Print(("Dd_Top_Start_Pll11(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Ddr_Pll00();
	Ddim_Print(("Dd_Top_Start_Ddr_Pll00(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Ddr_Pll01();
	Ddim_Print(("Dd_Top_Start_Ddr_Pll01(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Ddr_Pll02();
	Ddim_Print(("Dd_Top_Start_Ddr_Pll02(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Ddr_Pll10();
	Ddim_Print(("Dd_Top_Start_Ddr_Pll10(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Ddr_Pll11();
	Ddim_Print(("Dd_Top_Start_Ddr_Pll11(): status error. ret=%d\n", ret));

	ret = Dd_Top_Start_Ddr_Pll12();
	Ddim_Print(("Dd_Top_Start_Ddr_Pll12(): status error. ret=%d\n", ret));
}

static void ct_get_clock_clksel1(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL1.bit.RCLK     = 1;
	IO_CHIPTOP.CLKSEL1.bit.UHS1CLK0 = 1;
	IO_CHIPTOP.CLKSEL1.bit.UHS1CLK1 = 1;
	IO_CHIPTOP.CLKSEL1.bit.UHS1CLK2 = 1;
	IO_CHIPTOP.CLKSEL1.bit.UHS2CLK  = 1;
	IO_CHIPTOP.CLKSEL1.bit.NFCLK    = 1;
	IO_CHIPTOP.CLKSEL1.bit.EMMCCLK  = 1;
#endif

	Ddim_Print(("CLKSEL1_RCLK     = %d\n", Dd_Top_Get_CLKSEL1_RCLK()));
	Ddim_Print(("CLKSEL1_UHS1CLK0 = %d\n", Dd_Top_Get_CLKSEL1_UHS1CLK0()));
	Ddim_Print(("CLKSEL1_UHS1CLK1 = %d\n", Dd_Top_Get_CLKSEL1_UHS1CLK1()));
	Ddim_Print(("CLKSEL1_UHS1CLK2 = %d\n", Dd_Top_Get_CLKSEL1_UHS1CLK2()));
	Ddim_Print(("CLKSEL1_UHS2CLK  = %d\n", Dd_Top_Get_CLKSEL1_UHS2CLK()));
	Ddim_Print(("CLKSEL1_NFCLK    = %d\n", Dd_Top_Get_CLKSEL1_NFCLK()));
	Ddim_Print(("CLKSEL1_EMMCCLK  = %d\n", Dd_Top_Get_CLKSEL1_EMMCCLK()));

	Ddim_Print(("CLKSEL1 = %08lX\n", Dd_Top_Get_CLKSEL1()));
}

static void ct_get_clock_clksel2(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL2.bit.ELACLK		= 1;
	IO_CHIPTOP.CLKSEL2.bit.JPGCLK		= 1;
	IO_CHIPTOP.CLKSEL2.bit.GPUCLK		= 1;
	IO_CHIPTOP.CLKSEL2.bit.IPUTMECLK	= 1;
	IO_CHIPTOP.CLKSEL2.bit.IPUCLK		= 1;
	IO_CHIPTOP.CLKSEL2.bit.MIFCLK		= 1;
	IO_CHIPTOP.CLKSEL2.bit.HIFCLK		= 1;
	IO_CHIPTOP.CLKSEL2.bit.RAWCLK		= 1;
#endif

	Ddim_Print(("CLKSEL2_ELACLK    = %d\n", Dd_Top_Get_CLKSEL2_ELACLK()));
	Ddim_Print(("CLKSEL2_JPGCLK    = %d\n", Dd_Top_Get_CLKSEL2_JPGCLK()));
	Ddim_Print(("CLKSEL2_GPUCLK    = %d\n", Dd_Top_Get_CLKSEL2_GPUCLK()));
	Ddim_Print(("CLKSEL2_IPUTMECLK = %d\n", Dd_Top_Get_CLKSEL2_IPUTMECLK()));
	Ddim_Print(("CLKSEL2_IPUCLK	   = %d\n", Dd_Top_Get_CLKSEL2_IPUCLK()));
	Ddim_Print(("CLKSEL2_MIFCLK    = %d\n", Dd_Top_Get_CLKSEL2_MIFCLK()));
	Ddim_Print(("CLKSEL2_HIFCLK    = %d\n", Dd_Top_Get_CLKSEL2_HIFCLK()));
	Ddim_Print(("CLKSEL2_RAWCLK    = %d\n", Dd_Top_Get_CLKSEL2_RAWCLK()));

	Ddim_Print(("CLKSEL2 = %08lX\n", Dd_Top_Get_CLKSEL2()));
}

static void ct_get_clock_clksel3(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL3.bit.VDFCLK		= 1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	IO_CHIPTOP.CLKSEL3.bit.PXFCLK		= 1;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	IO_CHIPTOP.CLKSEL3.bit.HEVENCCLK	= 1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	IO_CHIPTOP.CLKSEL3.bit.IPPCLK		= 1;
	IO_CHIPTOP.CLKSEL3.bit.PASCLK		= 1;
	IO_CHIPTOP.CLKSEL3.bit.IIPCLK		= 1;
	IO_CHIPTOP.CLKSEL3.bit.SENMSKCLK	= 1;
	IO_CHIPTOP.CLKSEL3.bit.SENCLK		= 1;
	IO_CHIPTOP.CLKSEL3.bit.PIPESEL		= 1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	IO_CHIPTOP.CLKSEL3.bit.HEVENCSEL	= 1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#endif

	Ddim_Print(("CLKSEL3_VDFCLK		= %d\n", Dd_Top_Get_CLKSEL3_VDFCLK()));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("CLKSEL3_PXFCLK		= %d\n", Dd_Top_Get_CLKSEL3_PXFCLK()));
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("CLKSEL3_HEVENCCLK	= %d\n", Dd_Top_Get_CLKSEL3_HEVENCCLK()));
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	Ddim_Print(("CLKSEL3_IPPCLK		= %d\n", Dd_Top_Get_CLKSEL3_IPPCLK()));
	Ddim_Print(("CLKSEL3_PASCLK		= %d\n", Dd_Top_Get_CLKSEL3_PASCLK()));
	Ddim_Print(("CLKSEL3_IIPCLK		= %d\n", Dd_Top_Get_CLKSEL3_IIPCLK()));
	Ddim_Print(("CLKSEL3_SENMSKCLK	= %d\n", Dd_Top_Get_CLKSEL3_SENMSKCLK()));
	Ddim_Print(("CLKSEL3_SENCLK		= %d\n", Dd_Top_Get_CLKSEL3_SENCLK()));
	Ddim_Print(("CLKSEL3_PIPESEL	= %d\n", Dd_Top_Get_CLKSEL3_PIPESEL()));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("CLKSEL3_HEVENCSEL	= %d\n", Dd_Top_Get_CLKSEL3_HEVENCSEL()));
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("CLKSEL3 = %08lX\n", Dd_Top_Get_CLKSEL3()));
}

static void ct_get_clock_clksel4(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL4.bit.CNR1CLK		= 1;
	IO_CHIPTOP.CLKSEL4.bit.B2R1CLK		= 1;
	IO_CHIPTOP.CLKSEL4.bit.LTM1CLK		= 1;
	IO_CHIPTOP.CLKSEL4.bit.R2Y1CLK		= 1;
	IO_CHIPTOP.CLKSEL4.bit.SRO1CLK_2	= 1;
#endif

	Ddim_Print(("CLKSEL4_CNR1CLK	= %d\n", Dd_Top_Get_CLKSEL4_CNR1CLK()));
	Ddim_Print(("CLKSEL4_B2R1CLK	= %d\n", Dd_Top_Get_CLKSEL4_B2R1CLK()));
	Ddim_Print(("CLKSEL4_LTM1CLK	= %d\n", Dd_Top_Get_CLKSEL4_LTM1CLK()));
	Ddim_Print(("CLKSEL4_R2Y1CLK	= %d\n", Dd_Top_Get_CLKSEL4_R2Y1CLK()));
	Ddim_Print(("CLKSEL4_SRO1CLK_2	= %d\n", Dd_Top_Get_CLKSEL4_SRO1CLK_2()));

	Ddim_Print(("CLKSEL4 = %08lX\n", Dd_Top_Get_CLKSEL4()));
}

static void ct_get_clock_clksel5(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL5.bit.LTMRBK1CLK	= 1;
	IO_CHIPTOP.CLKSEL5.bit.B2B1CLK		= 1;
	IO_CHIPTOP.CLKSEL5.bit.SRO1CLK		= 1;
#endif

	Ddim_Print(("CLKSEL5_LTMRBK1CLK	= %d\n", Dd_Top_Get_CLKSEL5_LTMRBK1CLK()));
	Ddim_Print(("CLKSEL5_B2B1CLK	= %d\n", Dd_Top_Get_CLKSEL5_B2B1CLK()));
	Ddim_Print(("CLKSEL5_SRO1CLK	= %d\n", Dd_Top_Get_CLKSEL5_SRO1CLK()));

	Ddim_Print(("CLKSEL5 = %08lX\n", Dd_Top_Get_CLKSEL5()));
}

static void ct_get_clock_clksel6(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL6.bit.CNR2CLK		= 1;
	IO_CHIPTOP.CLKSEL6.bit.B2R2CLK		= 1;
	IO_CHIPTOP.CLKSEL6.bit.LTM2CLK		= 1;
	IO_CHIPTOP.CLKSEL6.bit.R2Y2CLK		= 1;
	IO_CHIPTOP.CLKSEL6.bit.SRO2CLK_2	= 1;
#endif

	Ddim_Print(("CLKSEL6_CNR2CLK	= %d\n", Dd_Top_Get_CLKSEL6_CNR2CLK()));
	Ddim_Print(("CLKSEL6_B2R2CLK	= %d\n", Dd_Top_Get_CLKSEL6_B2R2CLK()));
	Ddim_Print(("CLKSEL6_LTM2CLK	= %d\n", Dd_Top_Get_CLKSEL6_LTM2CLK()));
	Ddim_Print(("CLKSEL6_R2Y2CLK	= %d\n", Dd_Top_Get_CLKSEL6_R2Y2CLK()));
	Ddim_Print(("CLKSEL6_SRO2CLK_2	= %d\n", Dd_Top_Get_CLKSEL6_SRO2CLK_2()));

	Ddim_Print(("CLKSEL6 = %08lX\n", Dd_Top_Get_CLKSEL6()));
}

static void ct_get_clock_clksel7(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL7.bit.LTMRBK2CLK	= 1;
	IO_CHIPTOP.CLKSEL7.bit.B2B2CLK		= 1;
	IO_CHIPTOP.CLKSEL7.bit.SRO2CLK		= 1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	IO_CHIPTOP.CLKSEL7.bit.MIPISEL		= 1;
	IO_CHIPTOP.CLKSEL7.bit.PDM0SEL		= 1;
	IO_CHIPTOP.CLKSEL7.bit.PDM1SEL		= 1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#endif

	Ddim_Print(("CLKSEL7_LTMRBK2CLK	= %d\n", Dd_Top_Get_CLKSEL7_LTMRBK2CLK()));
	Ddim_Print(("CLKSEL7_B2B2CLK	= %d\n", Dd_Top_Get_CLKSEL7_B2B2CLK()));
	Ddim_Print(("CLKSEL7_SRO2CLK	= %d\n", Dd_Top_Get_CLKSEL7_SRO2CLK()));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("CLKSEL7_MIPISEL	= %d\n", Dd_Top_Get_CLKSEL7_MIPISEL()));
	Ddim_Print(("CLKSEL7_PDM0SEL	= %d\n", Dd_Top_Get_CLKSEL7_PDM0SEL()));
	Ddim_Print(("CLKSEL7_PDM1SEL	= %d\n", Dd_Top_Get_CLKSEL7_PDM1SEL()));
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("CLKSEL7 = %08lX\n", Dd_Top_Get_CLKSEL7()));
}

static void ct_get_clock_clksel8(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL8.bit.DSPCLK	= 1;
	IO_CHIPTOP.CLKSEL8.bit.SPICLK	= 1;
	IO_CHIPTOP.CLKSEL8.bit.AUCLK	= 1;
	IO_CHIPTOP.CLKSEL8.bit.AU0SEL	= 1;
	IO_CHIPTOP.CLKSEL8.bit.AU2SEL	= 1;
	IO_CHIPTOP.CLKSEL8.bit.AU3SEL	= 1;
	IO_CHIPTOP.CLKSEL8.bit.NETAUSEL	= 1;
	IO_CHIPTOP.CLKSEL8.bit.APCLK	= 1;
	IO_CHIPTOP.CLKSEL8.bit.AP0SEL	= 1;
	IO_CHIPTOP.CLKSEL8.bit.AP1SEL	= 1;
	IO_CHIPTOP.CLKSEL8.bit.AP2SEL	= 1;
	IO_CHIPTOP.CLKSEL8.bit.AP3SEL	= 1;
#endif

	Ddim_Print(("CLKSEL8_DSPCLK		= %d\n", Dd_Top_Get_CLKSEL8_DSPCLK()));
	Ddim_Print(("CLKSEL8_SPICLK		= %d\n", Dd_Top_Get_CLKSEL8_SPICLK()));
	Ddim_Print(("CLKSEL8_AUCLK		= %d\n", Dd_Top_Get_CLKSEL8_AUCLK()));
	Ddim_Print(("CLKSEL8_AU0SEL		= %d\n", Dd_Top_Get_CLKSEL8_AU0SEL()));
	Ddim_Print(("CLKSEL8_AU2SEL		= %d\n", Dd_Top_Get_CLKSEL8_AU2SEL()));
	Ddim_Print(("CLKSEL8_AU3SEL		= %d\n", Dd_Top_Get_CLKSEL8_AU3SEL()));
	Ddim_Print(("CLKSEL8_NETAUSEL	= %d\n", Dd_Top_Get_CLKSEL8_NETAUSEL()));
	Ddim_Print(("CLKSEL8_APCLK		= %d\n", Dd_Top_Get_CLKSEL8_APCLK()));
	Ddim_Print(("CLKSEL8_AP0SEL		= %d\n", Dd_Top_Get_CLKSEL8_AP0SEL()));
	Ddim_Print(("CLKSEL8_AP1SEL		= %d\n", Dd_Top_Get_CLKSEL8_AP1SEL()));
	Ddim_Print(("CLKSEL8_AP2SEL		= %d\n", Dd_Top_Get_CLKSEL8_AP2SEL()));
	Ddim_Print(("CLKSEL8_AP3SEL		= %d\n", Dd_Top_Get_CLKSEL8_AP3SEL()));

	Ddim_Print(("CLKSEL8 = %08lX\n", Dd_Top_Get_CLKSEL8()));
}

static void ct_get_clock_clksel9(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL9.bit.PCLK		= 1;
	IO_CHIPTOP.CLKSEL9.bit.HCLK		= 1;
	IO_CHIPTOP.CLKSEL9.bit.HCLKBMH	= 1;
	IO_CHIPTOP.CLKSEL9.bit.ACLKEXS	= 1;
	IO_CHIPTOP.CLKSEL9.bit.ACLK		= 1;
#endif

	Ddim_Print(("CLKSEL9_PCLK		= %d\n", Dd_Top_Get_CLKSEL9_PCLK()));
	Ddim_Print(("CLKSEL9_HCLK		= %d\n", Dd_Top_Get_CLKSEL9_HCLK()));
	Ddim_Print(("CLKSEL9_HCLKBMH	= %d\n", Dd_Top_Get_CLKSEL9_HCLKBMH()));
	Ddim_Print(("CLKSEL9_ACLKEXS	= %d\n", Dd_Top_Get_CLKSEL9_ACLKEXS()));
	Ddim_Print(("CLKSEL9_ACLK		= %d\n", Dd_Top_Get_CLKSEL9_ACLK()));

	Ddim_Print(("CLKSEL9 = %08lX\n", Dd_Top_Get_CLKSEL9()));
}

static void ct_get_clock_clksel10(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL10.bit.ACLK400 = 1;
	IO_CHIPTOP.CLKSEL10.bit.MCLK200 = 1;
	IO_CHIPTOP.CLKSEL10.bit.MCLK400 = 1;
#endif

	Ddim_Print(("CLKSEL10_ACLK400	= %d\n", Dd_Top_Get_CLKSEL10_ACLK400()));
	Ddim_Print(("CLKSEL10_MCLK200	= %d\n", Dd_Top_Get_CLKSEL10_MCLK200()));
	Ddim_Print(("CLKSEL10_MCLK400	= %d\n", Dd_Top_Get_CLKSEL10_MCLK400()));

	Ddim_Print(("CLKSEL10 = %08lX\n", Dd_Top_Get_CLKSEL10()));
}

static void ct_get_clock_clksel11(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL11.bit.DCHREQ = 1;
#endif

	Ddim_Print(("CLKSEL11_DCHREQ = %d\n", Dd_Top_Get_CLKSEL11_DCHREQ()));

	Ddim_Print(("CLKSEL11 = %08lX\n", Dd_Top_Get_CLKSEL11()));
}

static void ct_get_clock_clksel12(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSEL12.bit.ACLK300	= 1;
	IO_CHIPTOP.CLKSEL12.bit.GYROCLK	= 1;
	IO_CHIPTOP.CLKSEL12.bit.FPT0CLK	= 1;
	IO_CHIPTOP.CLKSEL12.bit.FPT1CLK	= 1;
	IO_CHIPTOP.CLKSEL12.bit.MECLK	= 1;
	IO_CHIPTOP.CLKSEL12.bit.NFBCHCLK = 1;
	IO_CHIPTOP.CLKSEL12.bit.RIBCLK	= 1;
	IO_CHIPTOP.CLKSEL12.bit.SHDRCLK	= 1;
#endif

	Ddim_Print(("CLKSEL12_ACLK300	= %d\n", Dd_Top_Get_CLKSEL12_ACLK300()));
	Ddim_Print(("CLKSEL12_GYROCLK	= %d\n", Dd_Top_Get_CLKSEL12_GYROCLK()));
	Ddim_Print(("CLKSEL12_FPT0CLK	= %d\n", Dd_Top_Get_CLKSEL12_FPT0CLK()));
	Ddim_Print(("CLKSEL12_FPT1CLK	= %d\n", Dd_Top_Get_CLKSEL12_FPT1CLK()));
	Ddim_Print(("CLKSEL12_MECLK		= %d\n", Dd_Top_Get_CLKSEL12_MECLK()));
	Ddim_Print(("CLKSEL12_NFBCHCLK	= %d\n", Dd_Top_Get_CLKSEL12_NFBCHCLK()));
	Ddim_Print(("CLKSEL12_RIBCLK	= %d\n", Dd_Top_Get_CLKSEL12_RIBCLK()));
	Ddim_Print(("CLKSEL12_SHDRCLK	= %d\n", Dd_Top_Get_CLKSEL12_SHDRCLK()));

	Ddim_Print(("CLKSEL12 = %08lX\n", Dd_Top_Get_CLKSEL12()));
}

static void ct_get_clock_pllcnt1(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT1.bit.PL00ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL01ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL02ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL03ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL04ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL05ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL05AST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL06ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL07ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL08ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL10ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL10AST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.PL11ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.DPL00ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.DPL01ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.DPL02ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.DPL10ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.DPL11ST		= 1;
	IO_CHIPTOP.PLLCNT1.bit.DPL12ST		= 1;
#endif

	Ddim_Print(("PLLCNT1_PL00ST	= %d\n", Dd_Top_Get_PLLCNT1_PL00ST()));
	Ddim_Print(("PLLCNT1_PL01ST	= %d\n", Dd_Top_Get_PLLCNT1_PL01ST()));
	Ddim_Print(("PLLCNT1_PL02ST	= %d\n", Dd_Top_Get_PLLCNT1_PL02ST()));
	Ddim_Print(("PLLCNT1_PL03ST	= %d\n", Dd_Top_Get_PLLCNT1_PL03ST()));
	Ddim_Print(("PLLCNT1_PL04ST	= %d\n", Dd_Top_Get_PLLCNT1_PL04ST()));
	Ddim_Print(("PLLCNT1_PL05ST	= %d\n", Dd_Top_Get_PLLCNT1_PL05ST()));
	Ddim_Print(("PLLCNT1_PL05AST	= %d\n", Dd_Top_Get_PLLCNT1_PL05AST()));
	Ddim_Print(("PLLCNT1_PL06ST	= %d\n", Dd_Top_Get_PLLCNT1_PL06ST()));
	Ddim_Print(("PLLCNT1_PL07ST	= %d\n", Dd_Top_Get_PLLCNT1_PL07ST()));
	Ddim_Print(("PLLCNT1_PL08ST	= %d\n", Dd_Top_Get_PLLCNT1_PL08ST()));
	Ddim_Print(("PLLCNT1_PL10ST	= %d\n", Dd_Top_Get_PLLCNT1_PL10ST()));
	Ddim_Print(("PLLCNT1_PL10AST	= %d\n", Dd_Top_Get_PLLCNT1_PL10AST()));
	Ddim_Print(("PLLCNT1_PL11ST	= %d\n", Dd_Top_Get_PLLCNT1_PL11ST()));
	Ddim_Print(("PLLCNT1_DPL00ST	= %d\n", Dd_Top_Get_PLLCNT1_DPL00ST()));
	Ddim_Print(("PLLCNT1_DPL01ST	= %d\n", Dd_Top_Get_PLLCNT1_DPL01ST()));
	Ddim_Print(("PLLCNT1_DPL02ST	= %d\n", Dd_Top_Get_PLLCNT1_DPL02ST()));
	Ddim_Print(("PLLCNT1_DPL10ST	= %d\n", Dd_Top_Get_PLLCNT1_DPL10ST()));
	Ddim_Print(("PLLCNT1_DPL11ST	= %d\n", Dd_Top_Get_PLLCNT1_DPL11ST()));
	Ddim_Print(("PLLCNT1_DPL12ST	= %d\n", Dd_Top_Get_PLLCNT1_DPL12ST()));
}

static void ct_get_clock_pllcnt2(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT2.bit.PL00SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL01SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL02SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL03SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL04SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL05SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL05ASEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL06SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL07SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL08SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL10SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL10ASEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL11SEL		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL01SSEN		= 1;
	IO_CHIPTOP.PLLCNT2.bit.PL02SSEN		= 1;
#endif

	Ddim_Print(("PLLCNT2_PL00SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL00SEL()));
	Ddim_Print(("PLLCNT2_PL01SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL01SEL()));
	Ddim_Print(("PLLCNT2_PL02SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL02SEL()));
	Ddim_Print(("PLLCNT2_PL03SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL03SEL()));
	Ddim_Print(("PLLCNT2_PL04SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL04SEL()));
	Ddim_Print(("PLLCNT2_PL05SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL05SEL()));
	Ddim_Print(("PLLCNT2_PL05ASEL	= %d\n", Dd_Top_Get_PLLCNT2_PL05ASEL()));
	Ddim_Print(("PLLCNT2_PL06SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL06SEL()));
	Ddim_Print(("PLLCNT2_PL07SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL07SEL()));
	Ddim_Print(("PLLCNT2_PL08SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL08SEL()));
	Ddim_Print(("PLLCNT2_PL10SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL10SEL()));
	Ddim_Print(("PLLCNT2_PL10ASEL	= %d\n", Dd_Top_Get_PLLCNT2_PL10ASEL()));
	Ddim_Print(("PLLCNT2_PL11SEL	= %d\n", Dd_Top_Get_PLLCNT2_PL11SEL()));
	Ddim_Print(("PLLCNT2_PL01SSEN	= %d\n", Dd_Top_Get_PLLCNT2_PL01SSEN()));
	Ddim_Print(("PLLCNT2_PL02SSEN	= %d\n", Dd_Top_Get_PLLCNT2_PL02SSEN()));
}                                       

static void ct_get_clock_pllcnt3(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT3.bit.P00POSTDIV	= 1;
	IO_CHIPTOP.PLLCNT3.bit.P00PREDIV	= 1;
	IO_CHIPTOP.PLLCNT3.bit.P00PLLDIV	= 1;
#endif

	Ddim_Print(("PLLCNT3_P00POSTDIV	= %d\n", Dd_Top_Get_PLLCNT3_P00POSTDIV()));
	Ddim_Print(("PLLCNT3_P00PREDIV	= %d\n", Dd_Top_Get_PLLCNT3_P00PREDIV()));
	Ddim_Print(("PLLCNT3_P00PLLDIV	= %d\n", Dd_Top_Get_PLLCNT3_P00PLLDIV()));
}

static void ct_get_clock_pllcnt4(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT4.bit.P00FNUM		= 1;
#endif

	Ddim_Print(("PLLCNT4_P00FNUM	= %lu\n", Dd_Top_Get_PLLCNT4_P00FNUM()));
}

static void ct_get_clock_pllcnt5(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT5.bit.P00FDEN		= 1;
#endif

	Ddim_Print(("PLLCNT5_P00FDEN	= %lu\n", Dd_Top_Get_PLLCNT5_P00FDEN()));
}

static void ct_get_clock_pllcnt6(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT6.bit.P01RATE		= 1;
	IO_CHIPTOP.PLLCNT6.bit.P01FREQ		= 1;
	IO_CHIPTOP.PLLCNT6.bit.P01MODE		= 1;
	IO_CHIPTOP.PLLCNT6.bit.P02RATE		= 1;
	IO_CHIPTOP.PLLCNT6.bit.P02FREQ		= 1;
	IO_CHIPTOP.PLLCNT6.bit.P02MODE		= 1;
#endif

	Ddim_Print(("PLLCNT6_P01RATE	= %d\n", Dd_Top_Get_PLLCNT6_P01RATE()));
	Ddim_Print(("PLLCNT6_P01FREQ	= %d\n", Dd_Top_Get_PLLCNT6_P01FREQ()));
	Ddim_Print(("PLLCNT6_P01MODE	= %d\n", Dd_Top_Get_PLLCNT6_P01MODE()));
	Ddim_Print(("PLLCNT6_P02RATE	= %d\n", Dd_Top_Get_PLLCNT6_P02RATE()));
	Ddim_Print(("PLLCNT6_P02FREQ	= %d\n", Dd_Top_Get_PLLCNT6_P02FREQ()));
	Ddim_Print(("PLLCNT6_P02MODE	= %d\n", Dd_Top_Get_PLLCNT6_P02MODE()));
}

static void ct_get_clock_pllcnt7(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV0	= 1;
	IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV1	= 1;
	IO_CHIPTOP.PLLCNT7.bit.P03PREDIV	= 1;
	IO_CHIPTOP.PLLCNT7.bit.P03PLLDIV	= 1;
	IO_CHIPTOP.PLLCNT7.bit.P03OC0		= 1;
	IO_CHIPTOP.PLLCNT7.bit.P03OC1		= 1;
	IO_CHIPTOP.PLLCNT7.bit.D0XOC		= 1;
	IO_CHIPTOP.PLLCNT7.bit.D1XOC		= 1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	IO_CHIPTOP.PLLCNT7.bit.DPLX8		= 1;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	IO_CHIPTOP.PLLCNT7.bit.DSEL			= 1;
#endif

	Ddim_Print(("PLLCNT7_P03POSTDIV0	= %d\n", Dd_Top_Get_PLLCNT7_P03POSTDIV0()));
	Ddim_Print(("PLLCNT7_P03POSTDIV1	= %d\n", Dd_Top_Get_PLLCNT7_P03POSTDIV1()));
	Ddim_Print(("PLLCNT7_P03PREDIV	= %d\n", Dd_Top_Get_PLLCNT7_P03PREDIV()));
	Ddim_Print(("PLLCNT7_P03PLLDIV	= %d\n", Dd_Top_Get_PLLCNT7_P03PLLDIV()));
	Ddim_Print(("PLLCNT7_P03OC0		= %d\n", Dd_Top_Get_PLLCNT7_P03OC0()));
	Ddim_Print(("PLLCNT7_P03OC1		= %d\n", Dd_Top_Get_PLLCNT7_P03OC1()));
	Ddim_Print(("PLLCNT7_D0XOC		= %d\n", Dd_Top_Get_PLLCNT7_D0XOC()));
	Ddim_Print(("PLLCNT7_D1XOC		= %d\n", Dd_Top_Get_PLLCNT7_D1XOC()));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("PLLCNT7_DPLX8		= %d\n", Dd_Top_Get_PLLCNT7_DPLX8()));
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	Ddim_Print(("PLLCNT7_DSEL		= %d\n", Dd_Top_Get_PLLCNT7_DSEL()));
}

static void ct_get_clock_pllcnt8(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT8.bit.P04POSTDIV	= 1;
	IO_CHIPTOP.PLLCNT8.bit.P04PREDIV	= 1;
	IO_CHIPTOP.PLLCNT8.bit.P04PLLDIV	= 1;
#endif
	Ddim_Print(("PLLCNT8_P04POSTDIV	= %d\n", Dd_Top_Get_PLLCNT8_P04POSTDIV()));
	Ddim_Print(("PLLCNT8_P04PREDIV	= %d\n", Dd_Top_Get_PLLCNT8_P04PREDIV()));
	Ddim_Print(("PLLCNT8_P04PLLDIV	= %d\n", Dd_Top_Get_PLLCNT8_P04PLLDIV()));
}

static void ct_get_clock_pllcnt9(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.PLLCNT9.bit.P10APLLDIV	= 1;
	IO_CHIPTOP.PLLCNT9.bit.P08PLLDIV	= 1;
	IO_CHIPTOP.PLLCNT9.bit.P05CHG	= 1;
#endif
	Ddim_Print(("PLLCNT9_P10APLLDIV	= %d\n", Dd_Top_Get_PLLCNT9_P10APLLDIV()));
	Ddim_Print(("PLLCNT9_P08PLLDIV	= %d\n", Dd_Top_Get_PLLCNT9_P08PLLDIV()));
	Ddim_Print(("PLLCNT9_P05CHG	    = %d\n", Dd_Top_Get_PLLCNT9_P05CHG()));
}

static void ct_get_clock_clkstop1(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP1.bit.DSPCK		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.DSPAX		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.SENCK		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.SENAX		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.SENAH		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.SENAP		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.GPIOAP		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.AU0CK		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.AU2CK		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.AU3CK		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.AU4CK		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.AU5CK		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.NETAUCK		= 1;
	IO_CHIPTOP.CLKSTOP1.bit.TEMPCK		= 1;
#endif

	Ddim_Print(("CLKSTOP1_DSPCK		= %d\n", Dd_Top_Get_CLKSTOP1_DSPCK()));
	Ddim_Print(("CLKSTOP1_DSPAX		= %d\n", Dd_Top_Get_CLKSTOP1_DSPAX()));
	Ddim_Print(("CLKSTOP1_SENCK		= %d\n", Dd_Top_Get_CLKSTOP1_SENCK()));
	Ddim_Print(("CLKSTOP1_SENAX		= %d\n", Dd_Top_Get_CLKSTOP1_SENAX()));
	Ddim_Print(("CLKSTOP1_SENAH		= %d\n", Dd_Top_Get_CLKSTOP1_SENAH()));
	Ddim_Print(("CLKSTOP1_SENAP		= %d\n", Dd_Top_Get_CLKSTOP1_SENAP()));
	Ddim_Print(("CLKSTOP1_GPIOAP		= %d\n", Dd_Top_Get_CLKSTOP1_GPIOAP()));
	Ddim_Print(("CLKSTOP1_AU0CK		= %d\n", Dd_Top_Get_CLKSTOP1_AU0CK()));
	Ddim_Print(("CLKSTOP1_AU2CK		= %d\n", Dd_Top_Get_CLKSTOP1_AU2CK()));
	Ddim_Print(("CLKSTOP1_AU3CK		= %d\n", Dd_Top_Get_CLKSTOP1_AU3CK()));
	Ddim_Print(("CLKSTOP1_AU4CK		= %d\n", Dd_Top_Get_CLKSTOP1_AU4CK()));
	Ddim_Print(("CLKSTOP1_AU5CK		= %d\n", Dd_Top_Get_CLKSTOP1_AU5CK()));
	Ddim_Print(("CLKSTOP1_NETAUCK		= %d\n", Dd_Top_Get_CLKSTOP1_NETAUCK()));
	Ddim_Print(("CLKSTOP1_TEMPCK		= %d\n", Dd_Top_Get_CLKSTOP1_TEMPCK()));
	Ddim_Print(("CLKSTOP1 = %08lX\n", Dd_Top_Get_CLKSTOP1()));
}

static void ct_get_clock_clkstop2(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP2.bit.RCK		    = 1;
	IO_CHIPTOP.CLKSTOP2.bit.UHS1CK0		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.UHS1CK1		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.UHS1CK2		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.UHS2CK		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.NFCK		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.EMMCCK		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.NETSECCK	= 1;
	IO_CHIPTOP.CLKSTOP2.bit.NETRCK		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.EXSAX		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.SPICK		= 1;
	IO_CHIPTOP.CLKSTOP2.bit.SLIMB00CK	= 1;
	IO_CHIPTOP.CLKSTOP2.bit.SLIMB01CK	= 1;
	IO_CHIPTOP.CLKSTOP2.bit.SLIMB10CK	= 1;
	IO_CHIPTOP.CLKSTOP2.bit.SLIMB11CK	= 1;
	IO_CHIPTOP.CLKSTOP2.bit.PCISUPPCK	= 1;
#endif

	Ddim_Print(("CLKSTOP2_RCK			= %d\n", Dd_Top_Get_CLKSTOP2_RCK()));
	Ddim_Print(("CLKSTOP2_UHS1CK0		= %d\n", Dd_Top_Get_CLKSTOP2_UHS1CK0()));
	Ddim_Print(("CLKSTOP2_UHS1CK1		= %d\n", Dd_Top_Get_CLKSTOP2_UHS1CK1()));
	Ddim_Print(("CLKSTOP2_UHS1CK2		= %d\n", Dd_Top_Get_CLKSTOP2_UHS1CK2()));
	Ddim_Print(("CLKSTOP2_UHS2CK		= %d\n", Dd_Top_Get_CLKSTOP2_UHS2CK()));
	Ddim_Print(("CLKSTOP2_NFCK			= %d\n", Dd_Top_Get_CLKSTOP2_NFCK()));
	Ddim_Print(("CLKSTOP2_EMMCCK		= %d\n", Dd_Top_Get_CLKSTOP2_EMMCCK()));
	Ddim_Print(("CLKSTOP2_NETSECCK		= %d\n", Dd_Top_Get_CLKSTOP2_NETSECCK()));
	Ddim_Print(("CLKSTOP2_NETRCK		= %d\n", Dd_Top_Get_CLKSTOP2_NETRCK()));
	Ddim_Print(("CLKSTOP2_EXSAX			= %d\n", Dd_Top_Get_CLKSTOP2_EXSAX()));
	Ddim_Print(("CLKSTOP2_SPICK			= %d\n", Dd_Top_Get_CLKSTOP2_SPICK()));
	Ddim_Print(("CLKSTOP2_SLIMB00CK		= %d\n", Dd_Top_Get_CLKSTOP2_SLIMB00CK()));
	Ddim_Print(("CLKSTOP2_SLIMB01CK		= %d\n", Dd_Top_Get_CLKSTOP2_SLIMB01CK()));
	Ddim_Print(("CLKSTOP2_SLIMB10CK		= %d\n", Dd_Top_Get_CLKSTOP2_SLIMB10CK()));
	Ddim_Print(("CLKSTOP2_SLIMB11CK		= %d\n", Dd_Top_Get_CLKSTOP2_SLIMB11CK()));
	Ddim_Print(("CLKSTOP2_PCISUPPCK		= %d\n", Dd_Top_Get_CLKSTOP2_PCISUPPCK()));
	Ddim_Print(("CLKSTOP2 = %08lX\n", Dd_Top_Get_CLKSTOP2()));
}

static void ct_get_clock_clkstop3(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP3.bit.IIPCK		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.IIPAP		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.IIPAH		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.IIPAX		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.LCDCK		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.HIFCK		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.MIFCK		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.DISPAP		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.DISPAH		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.DISPAX		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.JPGCK		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.JPGAX		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.JPGAH		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.JPGAP		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.PDM0CK		= 1;
	IO_CHIPTOP.CLKSTOP3.bit.PDM1CK		= 1;
#endif

	Ddim_Print(("CLKSTOP3_IIPCK		= %d\n", Dd_Top_Get_CLKSTOP3_IIPCK()));
	Ddim_Print(("CLKSTOP3_IIPAP		= %d\n", Dd_Top_Get_CLKSTOP3_IIPAP()));
	Ddim_Print(("CLKSTOP3_IIPAH		= %d\n", Dd_Top_Get_CLKSTOP3_IIPAH()));
	Ddim_Print(("CLKSTOP3_IIPAX		= %d\n", Dd_Top_Get_CLKSTOP3_IIPAX()));
	Ddim_Print(("CLKSTOP3_LCDCK		= %d\n", Dd_Top_Get_CLKSTOP3_LCDCK()));
	Ddim_Print(("CLKSTOP3_HIFCK		= %d\n", Dd_Top_Get_CLKSTOP3_HIFCK()));
	Ddim_Print(("CLKSTOP3_MIFCK		= %d\n", Dd_Top_Get_CLKSTOP3_MIFCK()));
	Ddim_Print(("CLKSTOP3_DISPAP	= %d\n", Dd_Top_Get_CLKSTOP3_DISPAP()));
	Ddim_Print(("CLKSTOP3_DISPAH	= %d\n", Dd_Top_Get_CLKSTOP3_DISPAH()));
	Ddim_Print(("CLKSTOP3_DISPAX	= %d\n", Dd_Top_Get_CLKSTOP3_DISPAX()));
	Ddim_Print(("CLKSTOP3_JPGAX		= %d\n", Dd_Top_Get_CLKSTOP3_JPGAX()));
	Ddim_Print(("CLKSTOP3_JPGCK		= %d\n", Dd_Top_Get_CLKSTOP3_JPGCK()));
	Ddim_Print(("CLKSTOP3_JPGAH		= %d\n", Dd_Top_Get_CLKSTOP3_JPGAH()));
	Ddim_Print(("CLKSTOP3_JPGAP		= %d\n", Dd_Top_Get_CLKSTOP3_JPGAP()));
	Ddim_Print(("CLKSTOP3_PDM0CK	= %d\n", Dd_Top_Get_CLKSTOP3_PDM0CK()));
	Ddim_Print(("CLKSTOP3_PDM1CK	= %d\n", Dd_Top_Get_CLKSTOP3_PDM1CK()));
	Ddim_Print(("CLKSTOP3 = %08lX\n", Dd_Top_Get_CLKSTOP3()));
}

static void ct_get_clock_clkstop4(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP4.bit.GPUCK		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.GPUAP		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.GPUAH		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.GPUAX		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.FPT0CK		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.FPT0AP		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.FPT0AH		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.FPT0AX		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.FPT1CK		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.FPT1AP		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.FPT1AH		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.FPT1AX		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.APCK0		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.APCK1		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.APCK2		= 1;
	IO_CHIPTOP.CLKSTOP4.bit.APCK3		= 1;
#endif

	Ddim_Print(("CLKSTOP4_GPUCK		= %d\n", Dd_Top_Get_CLKSTOP4_GPUCK()));
	Ddim_Print(("CLKSTOP4_GPUAP		= %d\n", Dd_Top_Get_CLKSTOP4_GPUAP()));
	Ddim_Print(("CLKSTOP4_GPUAH		= %d\n", Dd_Top_Get_CLKSTOP4_GPUAH()));
	Ddim_Print(("CLKSTOP4_GPUAX		= %d\n", Dd_Top_Get_CLKSTOP4_GPUAX()));
	Ddim_Print(("CLKSTOP4_FPT0CK	= %d\n", Dd_Top_Get_CLKSTOP4_FPT0CK()));
	Ddim_Print(("CLKSTOP4_FPT0AP	= %d\n", Dd_Top_Get_CLKSTOP4_FPT0AP()));
	Ddim_Print(("CLKSTOP4_FPT0AH	= %d\n", Dd_Top_Get_CLKSTOP4_FPT0AH()));
	Ddim_Print(("CLKSTOP4_FPT0AX	= %d\n", Dd_Top_Get_CLKSTOP4_FPT0AX()));
	Ddim_Print(("CLKSTOP4_FPT1CK	= %d\n", Dd_Top_Get_CLKSTOP4_FPT1CK()));
	Ddim_Print(("CLKSTOP4_FPT1AP	= %d\n", Dd_Top_Get_CLKSTOP4_FPT1AP()));
	Ddim_Print(("CLKSTOP4_FPT1AH	= %d\n", Dd_Top_Get_CLKSTOP4_FPT1AH()));
	Ddim_Print(("CLKSTOP4_FPT1AX	= %d\n", Dd_Top_Get_CLKSTOP4_FPT1AX()));
	Ddim_Print(("CLKSTOP4_APCK0		= %d\n", Dd_Top_Get_CLKSTOP4_APCK0()));
	Ddim_Print(("CLKSTOP4_APCK1		= %d\n", Dd_Top_Get_CLKSTOP4_APCK1()));
	Ddim_Print(("CLKSTOP4_APCK2		= %d\n", Dd_Top_Get_CLKSTOP4_APCK2()));
	Ddim_Print(("CLKSTOP4_APCK3		= %d\n", Dd_Top_Get_CLKSTOP4_APCK3()));
	Ddim_Print(("CLKSTOP4 = %08lX\n", Dd_Top_Get_CLKSTOP4()));
}

static void ct_get_clock_clkstop5(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP5.bit.MICAX0		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAX1		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAX2		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAX3		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAX4		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAX5		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAX6		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAP0		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAP1		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAP2		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAP3		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAP4		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAP5		= 1;
	IO_CHIPTOP.CLKSTOP5.bit.MICAP6		= 1;
#endif

	Ddim_Print(("CLKSTOP5_MICAX0		= %d\n", Dd_Top_Get_CLKSTOP5_MICAX0()));
	Ddim_Print(("CLKSTOP5_MICAX1		= %d\n", Dd_Top_Get_CLKSTOP5_MICAX1()));
	Ddim_Print(("CLKSTOP5_MICAX2		= %d\n", Dd_Top_Get_CLKSTOP5_MICAX2()));
	Ddim_Print(("CLKSTOP5_MICAX3		= %d\n", Dd_Top_Get_CLKSTOP5_MICAX3()));
	Ddim_Print(("CLKSTOP5_MICAX4		= %d\n", Dd_Top_Get_CLKSTOP5_MICAX4()));
	Ddim_Print(("CLKSTOP5_MICAX5		= %d\n", Dd_Top_Get_CLKSTOP5_MICAX5()));
	Ddim_Print(("CLKSTOP5_MICAX6		= %d\n", Dd_Top_Get_CLKSTOP5_MICAX6()));
	Ddim_Print(("CLKSTOP5_MICAP0		= %d\n", Dd_Top_Get_CLKSTOP5_MICAP0()));
	Ddim_Print(("CLKSTOP5_MICAP1		= %d\n", Dd_Top_Get_CLKSTOP5_MICAP1()));
	Ddim_Print(("CLKSTOP5_MICAP2		= %d\n", Dd_Top_Get_CLKSTOP5_MICAP2()));
	Ddim_Print(("CLKSTOP5_MICAP3		= %d\n", Dd_Top_Get_CLKSTOP5_MICAP3()));
	Ddim_Print(("CLKSTOP5_MICAP4		= %d\n", Dd_Top_Get_CLKSTOP5_MICAP4()));
	Ddim_Print(("CLKSTOP5_MICAP5		= %d\n", Dd_Top_Get_CLKSTOP5_MICAP5()));
	Ddim_Print(("CLKSTOP5_MICAP6		= %d\n", Dd_Top_Get_CLKSTOP5_MICAP6()));
	Ddim_Print(("CLKSTOP5 = %08lX\n", Dd_Top_Get_CLKSTOP5()));
}

static void ct_get_clock_clkstop6(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP6.bit.MICAH1		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.MICAH2		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.MICAH3		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.IMGAX		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.IMGAH0		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.IMGAH1		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.IMGAH3		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.IMGAP3		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.REGAP		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.XCHAX		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.XCHAP		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.ELACK		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.ELAAX		= 1;
	IO_CHIPTOP.CLKSTOP6.bit.ELAAP		= 1;
#endif

	Ddim_Print(("CLKSTOP6_MICAH1		= %d\n", Dd_Top_Get_CLKSTOP6_MICAH1()));
	Ddim_Print(("CLKSTOP6_MICAH2		= %d\n", Dd_Top_Get_CLKSTOP6_MICAH2()));
	Ddim_Print(("CLKSTOP6_MICAH3		= %d\n", Dd_Top_Get_CLKSTOP6_MICAH3()));
	Ddim_Print(("CLKSTOP6_IMGAX		= %d\n", Dd_Top_Get_CLKSTOP6_IMGAX()));
	Ddim_Print(("CLKSTOP6_IMGAH0		= %d\n", Dd_Top_Get_CLKSTOP6_IMGAH0()));
	Ddim_Print(("CLKSTOP6_IMGAH1		= %d\n", Dd_Top_Get_CLKSTOP6_IMGAH1()));
	Ddim_Print(("CLKSTOP6_IMGAH3		= %d\n", Dd_Top_Get_CLKSTOP6_IMGAH3()));
	Ddim_Print(("CLKSTOP6_IMGAP3		= %d\n", Dd_Top_Get_CLKSTOP6_IMGAP3()));
	Ddim_Print(("CLKSTOP6_REGAP		= %d\n", Dd_Top_Get_CLKSTOP6_REGAP()));
	Ddim_Print(("CLKSTOP6_XCHAX		= %d\n", Dd_Top_Get_CLKSTOP6_XCHAX()));
	Ddim_Print(("CLKSTOP6_XCHAP		= %d\n", Dd_Top_Get_CLKSTOP6_XCHAP()));
	Ddim_Print(("CLKSTOP6_ELACK		= %d\n", Dd_Top_Get_CLKSTOP6_ELACK()));
	Ddim_Print(("CLKSTOP6_ELAAX		= %d\n", Dd_Top_Get_CLKSTOP6_ELAAX()));
	Ddim_Print(("CLKSTOP6_ELAAP		= %d\n", Dd_Top_Get_CLKSTOP6_ELAAP()));
	Ddim_Print(("CLKSTOP6 = %08lX\n", Dd_Top_Get_CLKSTOP6()));
}

static void ct_get_clock_clkstop7(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP7.bit.IPUFDCK		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.IPUVISCK	= 1;
	IO_CHIPTOP.CLKSTOP7.bit.IPUAX		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.IPUAH		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.RAWCK		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.RAWAX		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.RAWAP		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.SHDRCK		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.SHDRAX		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.SHDRAH		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.SHDRAP		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.M0CK		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.MECK		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.MEAX		= 1;
	IO_CHIPTOP.CLKSTOP7.bit.MEAP		= 1;
#endif

	Ddim_Print(("CLKSTOP7_IPUFDCK	= %d\n", Dd_Top_Get_CLKSTOP7_IPUFDCK()));
	Ddim_Print(("CLKSTOP7_IPUVISCK	= %d\n", Dd_Top_Get_CLKSTOP7_IPUVISCK()));
	Ddim_Print(("CLKSTOP7_IPUAX		= %d\n", Dd_Top_Get_CLKSTOP7_IPUAX()));
	Ddim_Print(("CLKSTOP7_IPUAH		= %d\n", Dd_Top_Get_CLKSTOP7_IPUAH()));
	Ddim_Print(("CLKSTOP7_RAWCK		= %d\n", Dd_Top_Get_CLKSTOP7_RAWCK()));
	Ddim_Print(("CLKSTOP7_RAWAX		= %d\n", Dd_Top_Get_CLKSTOP7_RAWAX()));
	Ddim_Print(("CLKSTOP7_RAWAP		= %d\n", Dd_Top_Get_CLKSTOP7_RAWAP()));
	Ddim_Print(("CLKSTOP7_SHDRCK	= %d\n", Dd_Top_Get_CLKSTOP7_SHDRCK()));
	Ddim_Print(("CLKSTOP7_SHDRAX	= %d\n", Dd_Top_Get_CLKSTOP7_SHDRAX()));
	Ddim_Print(("CLKSTOP7_SHDRAH	= %d\n", Dd_Top_Get_CLKSTOP7_SHDRAH()));
	Ddim_Print(("CLKSTOP7_SHDRAP	= %d\n", Dd_Top_Get_CLKSTOP7_SHDRAP()));
	Ddim_Print(("CLKSTOP7_M0CK		= %d\n", Dd_Top_Get_CLKSTOP7_M0CK()));
	Ddim_Print(("CLKSTOP7_MECK		= %d\n", Dd_Top_Get_CLKSTOP7_MECK()));
	Ddim_Print(("CLKSTOP7_MEAX		= %d\n", Dd_Top_Get_CLKSTOP7_MEAX()));
	Ddim_Print(("CLKSTOP7_MEAP		= %d\n", Dd_Top_Get_CLKSTOP7_MEAP()));
	Ddim_Print(("CLKSTOP7 = %08lX\n", Dd_Top_Get_CLKSTOP7()));
}

static void ct_get_clock_clkstop8(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP8.bit.RIBCK		= 1;
	IO_CHIPTOP.CLKSTOP8.bit.RIBAH		= 1;
	IO_CHIPTOP.CLKSTOP8.bit.HEVDFCK		= 1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	IO_CHIPTOP.CLKSTOP8.bit.HEPXFCK		= 1;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	IO_CHIPTOP.CLKSTOP8.bit.HEVENCCK	= 1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	IO_CHIPTOP.CLKSTOP8.bit.HEIPPCK		= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX2	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX4	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC0RBRMX4	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX2	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX4	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC1RBRMX4	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC0CMNAX	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC0MX1AX	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC0MX2AX	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC0MX3AX	= 1;
	IO_CHIPTOP.CLKSTOP8.bit.UMC0MX4AX	= 1;
#endif

	Ddim_Print(("CLKSTOP8_RIBCK			= %d\n", Dd_Top_Get_CLKSTOP8_RIBCK()));
	Ddim_Print(("CLKSTOP8_RIBAH			= %d\n", Dd_Top_Get_CLKSTOP8_RIBAH()));
	Ddim_Print(("CLKSTOP8_HEVDFCK		= %d\n", Dd_Top_Get_CLKSTOP8_HEVDFCK()));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("CLKSTOP8_HEPXFCK		= %d\n", Dd_Top_Get_CLKSTOP8_HEPXFCK()));
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("CLKSTOP8_HEVENCCK		= %d\n", Dd_Top_Get_CLKSTOP8_HEVENCCK()));
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	Ddim_Print(("CLKSTOP8_HEIPPCK		= %d\n", Dd_Top_Get_CLKSTOP8_HEIPPCK()));
	Ddim_Print(("CLKSTOP8_UMC0HEVCMX2	= %d\n", Dd_Top_Get_CLKSTOP8_UMC0HEVCMX2()));
	Ddim_Print(("CLKSTOP8_UMC0HEVCMX4	= %d\n", Dd_Top_Get_CLKSTOP8_UMC0HEVCMX4()));
	Ddim_Print(("CLKSTOP8_UMC0RBRMX4	= %d\n", Dd_Top_Get_CLKSTOP8_UMC0RBRMX4()));
	Ddim_Print(("CLKSTOP8_UMC1HEVCMX2	= %d\n", Dd_Top_Get_CLKSTOP8_UMC1HEVCMX2()));
	Ddim_Print(("CLKSTOP8_UMC1HEVCMX4	= %d\n", Dd_Top_Get_CLKSTOP8_UMC1HEVCMX4()));
	Ddim_Print(("CLKSTOP8_UMC1RBRMX4	= %d\n", Dd_Top_Get_CLKSTOP8_UMC1RBRMX4()));
	Ddim_Print(("CLKSTOP8_UMC0CMNAX		= %d\n", Dd_Top_Get_CLKSTOP8_UMC0CMNAX()));
	Ddim_Print(("CLKSTOP8_UMC0MX1AX		= %d\n", Dd_Top_Get_CLKSTOP8_UMC0MX1AX()));
	Ddim_Print(("CLKSTOP8_UMC0MX2AX		= %d\n", Dd_Top_Get_CLKSTOP8_UMC0MX2AX()));
	Ddim_Print(("CLKSTOP8_UMC0MX3AX		= %d\n", Dd_Top_Get_CLKSTOP8_UMC0MX3AX()));
	Ddim_Print(("CLKSTOP8_UMC0MX4AX		= %d\n", Dd_Top_Get_CLKSTOP8_UMC0MX4AX()));
	Ddim_Print(("CLKSTOP8 = %08lX\n", Dd_Top_Get_CLKSTOP8()));
}

static void ct_get_clock_clkstop9(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP9.bit.UMC0MX5AX	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1CMNAX	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1MX1AX	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1MX2AX	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1MX3AX	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1MX4AX	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1MX5AX	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC0MX0AX3	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC0MX6AX4	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC0HEVCAX4	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1MX0AX3	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1MX6AX4	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1HEVCAX4	= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC0AP		= 1;
	IO_CHIPTOP.CLKSTOP9.bit.UMC1AP		= 1;
#endif

	Ddim_Print(("CLKSTOP9_UMC0MX5AX		= %d\n", Dd_Top_Get_CLKSTOP9_UMC0MX5AX()));
	Ddim_Print(("CLKSTOP9_UMC1CMNAX		= %d\n", Dd_Top_Get_CLKSTOP9_UMC1CMNAX()));
	Ddim_Print(("CLKSTOP9_UMC1MX1AX		= %d\n", Dd_Top_Get_CLKSTOP9_UMC1MX1AX()));
	Ddim_Print(("CLKSTOP9_UMC1MX2AX		= %d\n", Dd_Top_Get_CLKSTOP9_UMC1MX2AX()));
	Ddim_Print(("CLKSTOP9_UMC1MX3AX		= %d\n", Dd_Top_Get_CLKSTOP9_UMC1MX3AX()));
	Ddim_Print(("CLKSTOP9_UMC1MX4AX		= %d\n", Dd_Top_Get_CLKSTOP9_UMC1MX4AX()));
	Ddim_Print(("CLKSTOP9_UMC1MX5AX		= %d\n", Dd_Top_Get_CLKSTOP9_UMC1MX5AX()));
	Ddim_Print(("CLKSTOP9_UMC0MX0AX3	= %d\n", Dd_Top_Get_CLKSTOP9_UMC0MX0AX3()));
	Ddim_Print(("CLKSTOP9_UMC0MX6AX4	= %d\n", Dd_Top_Get_CLKSTOP9_UMC0MX6AX4()));
	Ddim_Print(("CLKSTOP9_UMC0HEVCAX4	= %d\n", Dd_Top_Get_CLKSTOP9_UMC0HEVCAX4()));
	Ddim_Print(("CLKSTOP9_UMC1MX0AX3	= %d\n", Dd_Top_Get_CLKSTOP9_UMC1MX0AX3()));
	Ddim_Print(("CLKSTOP9_UMC1MX6AX4	= %d\n", Dd_Top_Get_CLKSTOP9_UMC1MX6AX4()));
	Ddim_Print(("CLKSTOP9_UMC1HEVCAX4	= %d\n", Dd_Top_Get_CLKSTOP9_UMC1HEVCAX4()));
	Ddim_Print(("CLKSTOP9_UMC0AP		= %d\n", Dd_Top_Get_CLKSTOP9_UMC0AP()));
	Ddim_Print(("CLKSTOP9_UMC1AP		= %d\n", Dd_Top_Get_CLKSTOP9_UMC1AP()));
	Ddim_Print(("CLKSTOP9 = %08lX\n", Dd_Top_Get_CLKSTOP9()));
}

static void ct_get_clock_clkstop10(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP10.bit.SRO1CK		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.SRO1CK_2	= 1;
	IO_CHIPTOP.CLKSTOP10.bit.SRO1AX		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.SRO1AH		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.SRO1AP		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.B2B1CK		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.B2B1AX		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.B2B1AH		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.B2B1AP		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.LTMRBK1CK	= 1;
	IO_CHIPTOP.CLKSTOP10.bit.B2R1CK		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.B2R1AX		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.B2R1AH		= 1;
	IO_CHIPTOP.CLKSTOP10.bit.B2R1AP		= 1;
#endif

	Ddim_Print(("CLKSTOP10_SRO1CK		= %d\n", Dd_Top_Get_CLKSTOP10_SRO1CK()));
	Ddim_Print(("CLKSTOP10_SRO1CK_2		= %d\n", Dd_Top_Get_CLKSTOP10_SRO1CK_2()));
	Ddim_Print(("CLKSTOP10_SRO1AX		= %d\n", Dd_Top_Get_CLKSTOP10_SRO1AX()));
	Ddim_Print(("CLKSTOP10_SRO1AH		= %d\n", Dd_Top_Get_CLKSTOP10_SRO1AH()));
	Ddim_Print(("CLKSTOP10_SRO1AP		= %d\n", Dd_Top_Get_CLKSTOP10_SRO1AP()));
	Ddim_Print(("CLKSTOP10_B2B1CK		= %d\n", Dd_Top_Get_CLKSTOP10_B2B1CK()));
	Ddim_Print(("CLKSTOP10_B2B1AX		= %d\n", Dd_Top_Get_CLKSTOP10_B2B1AX()));
	Ddim_Print(("CLKSTOP10_B2B1AH		= %d\n", Dd_Top_Get_CLKSTOP10_B2B1AH()));
	Ddim_Print(("CLKSTOP10_B2B1AP		= %d\n", Dd_Top_Get_CLKSTOP10_B2B1AP()));
	Ddim_Print(("CLKSTOP10_LTMRBK1CK	= %d\n", Dd_Top_Get_CLKSTOP10_LTMRBK1CK()));
	Ddim_Print(("CLKSTOP10_B2R1CK		= %d\n", Dd_Top_Get_CLKSTOP10_B2R1CK()));
	Ddim_Print(("CLKSTOP10_B2R1AX		= %d\n", Dd_Top_Get_CLKSTOP10_B2R1AX()));
	Ddim_Print(("CLKSTOP10_B2R1AH		= %d\n", Dd_Top_Get_CLKSTOP10_B2R1AH()));
	Ddim_Print(("CLKSTOP10_B2R1AP		= %d\n", Dd_Top_Get_CLKSTOP10_B2R1AP()));
	Ddim_Print(("CLKSTOP10 = %08lX\n", Dd_Top_Get_CLKSTOP10()));
}

static void ct_get_clock_clkstop11(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP11.bit.LTM1CK		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.LTM1AX		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.LTM1AH		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.LTM1AP		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.R2Y1CK		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.R2Y1AX		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.R2Y1AH		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.R2Y1AP		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.CNR1CK		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.CNR1AX		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.CNR1AP		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.APAH		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.DBGAP		= 1;
	IO_CHIPTOP.CLKSTOP11.bit.NFBCHCK	= 1;
#endif

	Ddim_Print(("CLKSTOP11_LTM1CK		= %d\n", Dd_Top_Get_CLKSTOP11_LTM1CK()));
	Ddim_Print(("CLKSTOP11_LTM1AX		= %d\n", Dd_Top_Get_CLKSTOP11_LTM1AX()));
	Ddim_Print(("CLKSTOP11_LTM1AH		= %d\n", Dd_Top_Get_CLKSTOP11_LTM1AH()));
	Ddim_Print(("CLKSTOP11_LTM1AP		= %d\n", Dd_Top_Get_CLKSTOP11_LTM1AP()));
	Ddim_Print(("CLKSTOP11_R2Y1CK		= %d\n", Dd_Top_Get_CLKSTOP11_R2Y1CK()));
	Ddim_Print(("CLKSTOP11_R2Y1AX		= %d\n", Dd_Top_Get_CLKSTOP11_R2Y1AX()));
	Ddim_Print(("CLKSTOP11_R2Y1AH		= %d\n", Dd_Top_Get_CLKSTOP11_R2Y1AH()));
	Ddim_Print(("CLKSTOP11_R2Y1AP		= %d\n", Dd_Top_Get_CLKSTOP11_R2Y1AP()));
	Ddim_Print(("CLKSTOP11_CNR1CK		= %d\n", Dd_Top_Get_CLKSTOP11_CNR1CK()));
	Ddim_Print(("CLKSTOP11_CNR1AX		= %d\n", Dd_Top_Get_CLKSTOP11_CNR1AX()));
	Ddim_Print(("CLKSTOP11_CNR1AP		= %d\n", Dd_Top_Get_CLKSTOP11_CNR1AP()));
	Ddim_Print(("CLKSTOP11_APAH			= %d\n", Dd_Top_Get_CLKSTOP11_APAH()));
	Ddim_Print(("CLKSTOP11_DBGAP		= %d\n", Dd_Top_Get_CLKSTOP11_DBGAP()));
	Ddim_Print(("CLKSTOP11_NFBCHCK		= %d\n", Dd_Top_Get_CLKSTOP11_NFBCHCK()));
	Ddim_Print(("CLKSTOP11 = %08lX\n", Dd_Top_Get_CLKSTOP11()));
}

static void ct_get_clock_clkstop12(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP12.bit.SRO2CK		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.SRO2CK_2	= 1;
	IO_CHIPTOP.CLKSTOP12.bit.SRO2AX		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.SRO2AH		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.SRO2AP		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.B2B2CK		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.B2B2AX		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.B2B2AH		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.B2B2AP		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.LTMRBK2CK	= 1;
	IO_CHIPTOP.CLKSTOP12.bit.B2R2CK		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.B2R2AX		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.B2R2AH		= 1;
	IO_CHIPTOP.CLKSTOP12.bit.B2R2AP		= 1;
#endif

	Ddim_Print(("CLKSTOP12_SRO2CK		= %d\n", Dd_Top_Get_CLKSTOP12_SRO2CK()));
	Ddim_Print(("CLKSTOP12_SRO2CK_2		= %d\n", Dd_Top_Get_CLKSTOP12_SRO2CK_2()));
	Ddim_Print(("CLKSTOP12_SRO2AX		= %d\n", Dd_Top_Get_CLKSTOP12_SRO2AX()));
	Ddim_Print(("CLKSTOP12_SRO2AH		= %d\n", Dd_Top_Get_CLKSTOP12_SRO2AH()));
	Ddim_Print(("CLKSTOP12_SRO2AP		= %d\n", Dd_Top_Get_CLKSTOP12_SRO2AP()));
	Ddim_Print(("CLKSTOP12_B2B2CK		= %d\n", Dd_Top_Get_CLKSTOP12_B2B2CK()));
	Ddim_Print(("CLKSTOP12_B2B2AX		= %d\n", Dd_Top_Get_CLKSTOP12_B2B2AX()));
	Ddim_Print(("CLKSTOP12_B2B2AH		= %d\n", Dd_Top_Get_CLKSTOP12_B2B2AH()));
	Ddim_Print(("CLKSTOP12_B2B2AP		= %d\n", Dd_Top_Get_CLKSTOP12_B2B2AP()));
	Ddim_Print(("CLKSTOP12_LTMRBK2CK	= %d\n", Dd_Top_Get_CLKSTOP12_LTMRBK2CK()));
	Ddim_Print(("CLKSTOP12_B2R2CK		= %d\n", Dd_Top_Get_CLKSTOP12_B2R2CK()));
	Ddim_Print(("CLKSTOP12_B2R2AX		= %d\n", Dd_Top_Get_CLKSTOP12_B2R2AX()));
	Ddim_Print(("CLKSTOP12_B2R2AH		= %d\n", Dd_Top_Get_CLKSTOP12_B2R2AH()));
	Ddim_Print(("CLKSTOP12_B2R2AP		= %d\n", Dd_Top_Get_CLKSTOP12_B2R2AP()));
	Ddim_Print(("CLKSTOP12 = %08lX\n", Dd_Top_Get_CLKSTOP12()));
}

static void ct_get_clock_clkstop13(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP13.bit.LTM2CK		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.LTM2AX		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.LTM2AH		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.LTM2AP		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.R2Y2CK		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.R2Y2AX		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.R2Y2AH		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.R2Y2AP		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.CNR2CK		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.CNR2AX		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.CNR2AP		= 1;
	IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX4	= 1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX4	= 1;
	IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX2	= 1;
	IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX2	= 1;// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	IO_CHIPTOP.CLKSTOP13.bit.UMCHEVENCAX4	= 1;
	IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX2	= 1;
	IO_CHIPTOP.CLKSTOP13.bit.HEVENCAP	= 1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	IO_CHIPTOP.CLKSTOP13.bit.BMH1CK		= 1;
#endif

	Ddim_Print(("CLKSTOP13_LTM2CK		= %d\n", Dd_Top_Get_CLKSTOP13_LTM2CK()));
	Ddim_Print(("CLKSTOP13_LTM2AX		= %d\n", Dd_Top_Get_CLKSTOP13_LTM2AX()));
	Ddim_Print(("CLKSTOP13_LTM2AH		= %d\n", Dd_Top_Get_CLKSTOP13_LTM2AH()));
	Ddim_Print(("CLKSTOP13_LTM2AP		= %d\n", Dd_Top_Get_CLKSTOP13_LTM2AP()));
	Ddim_Print(("CLKSTOP13_R2Y2CK		= %d\n", Dd_Top_Get_CLKSTOP13_R2Y2CK()));
	Ddim_Print(("CLKSTOP13_R2Y2AX		= %d\n", Dd_Top_Get_CLKSTOP13_R2Y2AX()));
	Ddim_Print(("CLKSTOP13_R2Y2AH		= %d\n", Dd_Top_Get_CLKSTOP13_R2Y2AH()));
	Ddim_Print(("CLKSTOP13_R2Y2AP		= %d\n", Dd_Top_Get_CLKSTOP13_R2Y2AP()));
	Ddim_Print(("CLKSTOP13_CNR2CK		= %d\n", Dd_Top_Get_CLKSTOP13_CNR2CK()));
	Ddim_Print(("CLKSTOP13_CNR2AX		= %d\n", Dd_Top_Get_CLKSTOP13_CNR2AX()));
	Ddim_Print(("CLKSTOP13_CNR2AP		= %d\n", Dd_Top_Get_CLKSTOP13_CNR2AP()));
	Ddim_Print(("CLKSTOP13_UMCVDFMX4	= %d\n", Dd_Top_Get_CLKSTOP13_UMCVDFMX4()));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("CLKSTOP13_UMCPXFMX4	= %d\n", Dd_Top_Get_CLKSTOP13_UMCPXFMX4()));
	Ddim_Print(("CLKSTOP13_UMCVDFMX2	= %d\n", Dd_Top_Get_CLKSTOP13_UMCVDFMX2()));
	Ddim_Print(("CLKSTOP13_UMCPXFMX2	= %d\n", Dd_Top_Get_CLKSTOP13_UMCPXFMX2()));
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("CLKSTOP13_UMCHEVENCAX4	= %d\n", Dd_Top_Get_CLKSTOP13_UMCHEVENCAX4()));
	Ddim_Print(("CLKSTOP13_UMCVDFMX2	= %d\n", Dd_Top_Get_CLKSTOP13_UMCVDFMX2()));
	Ddim_Print(("CLKSTOP13_HEVENCAP		= %d\n", Dd_Top_Get_CLKSTOP13_HEVENCAP()));
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	Ddim_Print(("CLKSTOP13_BMH1CK		= %d\n", Dd_Top_Get_CLKSTOP13_BMH1CK()));
	Ddim_Print(("CLKSTOP13 = %08lX\n", Dd_Top_Get_CLKSTOP13()));
}

static void ct_get_clock_clkstop14(VOID)
{
#ifdef D_DD_TOP_PC_DEBUG
	IO_CHIPTOP.CLKSTOP14.bit.STATAX		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.STATAH		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.STATAP		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.PERIAH		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.PERIAP		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.SENMSKCK	= 1;
	IO_CHIPTOP.CLKSTOP14.bit.GYROCK		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.EXSAH		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.EXSAP		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.PASCK		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.BMH0CK		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.BMH0AX		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.RDMAAX		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.RDMAAP		= 1;
	IO_CHIPTOP.CLKSTOP14.bit.BMH1AX		= 1;
#endif

	Ddim_Print(("CLKSTOP14_STATAX		= %d\n", Dd_Top_Get_CLKSTOP14_STATAX()));
	Ddim_Print(("CLKSTOP14_STATAH		= %d\n", Dd_Top_Get_CLKSTOP14_STATAH()));
	Ddim_Print(("CLKSTOP14_STATAP		= %d\n", Dd_Top_Get_CLKSTOP14_STATAP()));
	Ddim_Print(("CLKSTOP14_PERIAH		= %d\n", Dd_Top_Get_CLKSTOP14_PERIAH()));
	Ddim_Print(("CLKSTOP14_PERIAP		= %d\n", Dd_Top_Get_CLKSTOP14_PERIAP()));
	Ddim_Print(("CLKSTOP14_SENMSKCK		= %d\n", Dd_Top_Get_CLKSTOP14_SENMSKCK()));
	Ddim_Print(("CLKSTOP14_GYROCK		= %d\n", Dd_Top_Get_CLKSTOP14_GYROCK()));
	Ddim_Print(("CLKSTOP14_EXSAH		= %d\n", Dd_Top_Get_CLKSTOP14_EXSAH()));
	Ddim_Print(("CLKSTOP14_EXSAP		= %d\n", Dd_Top_Get_CLKSTOP14_EXSAP()));
	Ddim_Print(("CLKSTOP14_PASCK		= %d\n", Dd_Top_Get_CLKSTOP14_PASCK()));
	Ddim_Print(("CLKSTOP14_BMH0CK		= %d\n", Dd_Top_Get_CLKSTOP14_BMH0CK()));
	Ddim_Print(("CLKSTOP14_BMH0AX		= %d\n", Dd_Top_Get_CLKSTOP14_BMH0AX()));
	Ddim_Print(("CLKSTOP14_RDMAAX		= %d\n", Dd_Top_Get_CLKSTOP14_RDMAAX()));
	Ddim_Print(("CLKSTOP14_RDMAAP		= %d\n", Dd_Top_Get_CLKSTOP14_RDMAAP()));
	Ddim_Print(("CLKSTOP14_BMH1AX		= %d\n", Dd_Top_Get_CLKSTOP14_BMH1AX()));
	Ddim_Print(("CLKSTOP14 = %08lX\n", Dd_Top_Get_CLKSTOP14()));
}

static void ct_set_clock_clksel1(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "RCLK" ) ) {
			Dd_Top_Set_CLKSEL1_RCLK(val);
			Ddim_Print(("It set CLKSEL_RCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL1.bit.RCLK	 = %d\n", IO_CHIPTOP.CLKSEL1.bit.RCLK));
		}
		else if ( 0 == strcmp( argv[4], "UHS1CLK0" ) ) {
			Dd_Top_Set_CLKSEL1_UHS1CLK0(val);
			Ddim_Print(("It set CLKSEL_UHS1CLK0	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL1.bit.UHS1CLK0	 = %d\n", IO_CHIPTOP.CLKSEL1.bit.UHS1CLK0));
		}
		else if ( 0 == strcmp( argv[4], "UHS1CLK1" ) ) {
			Dd_Top_Set_CLKSEL1_UHS1CLK1(val);
			Ddim_Print(("It set CLKSEL_UHS1CLK1	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL1.bit.UHS1CLK1	 = %d\n", IO_CHIPTOP.CLKSEL1.bit.UHS1CLK1));
		}
		else if ( 0 == strcmp( argv[4], "UHS1CLK2" ) ) {
			Dd_Top_Set_CLKSEL1_UHS1CLK2(val);
			Ddim_Print(("It set CLKSEL_UHS1CLK2	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL1.bit.UHS1CLK2	 = %d\n", IO_CHIPTOP.CLKSEL1.bit.UHS1CLK2));
		}
		else if ( 0 == strcmp( argv[4], "UHS2CLK" ) ) {
			Dd_Top_Set_CLKSEL1_UHS2CLK(val);
			Ddim_Print(("It set CLKSEL_UHS2CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL1.bit.UHS2CLK	 = %d\n", IO_CHIPTOP.CLKSEL1.bit.UHS2CLK));
		}
		else if ( 0 == strcmp( argv[4], "NFCLK" ) ) {
			Dd_Top_Set_CLKSEL1_NFCLK(val);
			Ddim_Print(("It set CLKSEL_NFCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL1.bit.NFCLK	 = %d\n", IO_CHIPTOP.CLKSEL1.bit.NFCLK));
		}
		else if ( 0 == strcmp( argv[4], "EMMCCLK" ) ) {
			Dd_Top_Set_CLKSEL1_EMMCCLK(val);
			Ddim_Print(("It set CLKSEL_EMMCCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL1.bit.EMMCCLK	 = %d\n", IO_CHIPTOP.CLKSEL1.bit.EMMCCLK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel2(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "ELACLK" ) ) {
			Dd_Top_Set_CLKSEL2_ELACLK(val);
			Ddim_Print(("It set CLKSEL_ELACLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL2.bit.ELACLK	 = %d\n", IO_CHIPTOP.CLKSEL2.bit.ELACLK));
		}
		else if ( 0 == strcmp( argv[4], "JPGCLK" ) ) {
			Dd_Top_Set_CLKSEL2_JPGCLK(val);
			Ddim_Print(("It set CLKSEL_JPGCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL2.bit.JPGCLK	 = %d\n", IO_CHIPTOP.CLKSEL2.bit.JPGCLK));
		}
		else if ( 0 == strcmp( argv[4], "GPUCLK" ) ) {
			Dd_Top_Set_CLKSEL2_GPUCLK(val);
			Ddim_Print(("It set CLKSEL_GPUCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL2.bit.GPUCLK	 = %d\n", IO_CHIPTOP.CLKSEL2.bit.GPUCLK));
		}
		else if ( 0 == strcmp( argv[4], "IPUTMECLK" ) ) {
			Dd_Top_Set_CLKSEL2_IPUTMECLK(val);
			Ddim_Print(("It set CLKSEL_IPUTMECLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL2.bit.IPUTMECLK	 = %d\n", IO_CHIPTOP.CLKSEL2.bit.IPUTMECLK));
		}
		else if ( 0 == strcmp( argv[4], "IPUCLK" ) ) {
			Dd_Top_Set_CLKSEL2_IPUCLK(val);
			Ddim_Print(("It set CLKSEL_IPUCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL2.bit.IPUCLK	 = %d\n", IO_CHIPTOP.CLKSEL2.bit.IPUCLK));
		}
		else if ( 0 == strcmp( argv[4], "MIFCLK" ) ) {
			Dd_Top_Set_CLKSEL2_MIFCLK(val);
			Ddim_Print(("It set CLKSEL_MIFCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL2.bit.MIFCLK	 = %d\n", IO_CHIPTOP.CLKSEL2.bit.MIFCLK));
		}
		else if ( 0 == strcmp( argv[4], "HIFCLK" ) ) {
			Dd_Top_Set_CLKSEL2_HIFCLK(val);
			Ddim_Print(("It set CLKSEL_HIFCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL2.bit.HIFCLK	 = %d\n", IO_CHIPTOP.CLKSEL2.bit.HIFCLK));
		}
		else if ( 0 == strcmp( argv[4], "RAWCLK" ) ) {
			Dd_Top_Set_CLKSEL2_RAWCLK(val);
			Ddim_Print(("It set CLKSEL_RAWCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL2.bit.RAWCLK	 = %d\n", IO_CHIPTOP.CLKSEL2.bit.RAWCLK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel3(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "VDFCLK" ) ) {
			Dd_Top_Set_CLKSEL3_VDFCLK(val);
			Ddim_Print(("It set CLKSEL_VDFCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.VDFCLK	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.VDFCLK));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "PXFCLK" ) ) {
			Dd_Top_Set_CLKSEL3_PXFCLK(val);
			Ddim_Print(("It set CLKSEL_PXFCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.PXFCLK	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.PXFCLK));
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "HEVENCCLK" ) ) {
			Dd_Top_Set_CLKSEL3_HEVENCCLK(val);
			Ddim_Print(("It set CLKSEL_HEVENCCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.HEVENCCLK	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.HEVENCCLK));
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[4], "IPPCLK" ) ) {
			Dd_Top_Set_CLKSEL3_IPPCLK(val);
			Ddim_Print(("It set CLKSEL_IPPCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.IPPCLK	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.IPPCLK));
		}
		else if ( 0 == strcmp( argv[4], "PASCLK" ) ) {
			Dd_Top_Set_CLKSEL3_PASCLK(val);
			Ddim_Print(("It set CLKSEL_PASCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.PASCLK	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.PASCLK));
		}
		else if ( 0 == strcmp( argv[4], "IIPCLK" ) ) {
			Dd_Top_Set_CLKSEL3_IIPCLK(val);
			Ddim_Print(("It set CLKSEL_IIPCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.IIPCLK	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.IIPCLK));
		}
		else if ( 0 == strcmp( argv[4], "SENMSKCLK" ) ) {
			Dd_Top_Set_CLKSEL3_SENMSKCLK(val);
			Ddim_Print(("It set CLKSEL_SENMSKCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.SENMSKCLK	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.SENMSKCLK));
		}
		else if ( 0 == strcmp( argv[4], "SENCLK" ) ) {
			Dd_Top_Set_CLKSEL3_SENCLK(val);
			Ddim_Print(("It set CLKSEL_SENCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.SENCLK	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.SENCLK));
		}
		else if ( 0 == strcmp( argv[4], "PIPESEL" ) ) {
			Dd_Top_Set_CLKSEL3_PIPESEL(val);
			Ddim_Print(("It set CLKSEL_PIPESEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.PIPESEL	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.PIPESEL));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "HEVENCSEL" ) ) {
			Dd_Top_Set_CLKSEL3_HEVENCSEL(val);
			Ddim_Print(("It set CLKSEL_HEVENCSEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL3.bit.HEVENCSEL	 = %d\n", IO_CHIPTOP.CLKSEL3.bit.HEVENCSEL));
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel4(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "CNR1CLK" ) ) {
			Dd_Top_Set_CLKSEL4_CNR1CLK(val);
			Ddim_Print(("It set CLKSEL_CNR1CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL4.bit.CNR1CLK	 = %d\n", IO_CHIPTOP.CLKSEL4.bit.CNR1CLK));
		}
		else if ( 0 == strcmp( argv[4], "B2R1CLK" ) ) {
			Dd_Top_Set_CLKSEL4_B2R1CLK(val);
			Ddim_Print(("It set CLKSEL_B2R1CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL4.bit.B2R1CLK	 = %d\n", IO_CHIPTOP.CLKSEL4.bit.B2R1CLK));
		}
		else if ( 0 == strcmp( argv[4], "LTM1CLK" ) ) {
			Dd_Top_Set_CLKSEL4_LTM1CLK(val);
			Ddim_Print(("It set CLKSEL_LTM1CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL4.bit.LTM1CLK	 = %d\n", IO_CHIPTOP.CLKSEL4.bit.LTM1CLK));
		}
		else if ( 0 == strcmp( argv[4], "R2Y1CLK" ) ) {
			Dd_Top_Set_CLKSEL4_R2Y1CLK(val);
			Ddim_Print(("It set CLKSEL_R2Y1CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL4.bit.R2Y1CLK	 = %d\n", IO_CHIPTOP.CLKSEL4.bit.R2Y1CLK));
		}
		else if ( 0 == strcmp( argv[4], "SRO1CLK_2" ) ) {
			Dd_Top_Set_CLKSEL4_SRO1CLK_2(val);
			Ddim_Print(("It set CLKSEL_SRO1CLK_2	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL4.bit.SRO1CLK_2	 = %d\n", IO_CHIPTOP.CLKSEL4.bit.SRO1CLK_2));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel5(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "LTMRBK1CLK" ) ) {
			Dd_Top_Set_CLKSEL5_LTMRBK1CLK(val);
			Ddim_Print(("It set CLKSEL_LTMRBK1CLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL5.bit.LTMRBK1CLK	 = %d\n", IO_CHIPTOP.CLKSEL5.bit.LTMRBK1CLK));
		}
		else if ( 0 == strcmp( argv[4], "B2B1CLK" ) ) {
			Dd_Top_Set_CLKSEL5_B2B1CLK(val);
			Ddim_Print(("It set CLKSEL_B2B1CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL5.bit.B2B1CLK	 = %d\n", IO_CHIPTOP.CLKSEL5.bit.B2B1CLK));
		}
		else if ( 0 == strcmp( argv[4], "SRO1CLK" ) ) {
			Dd_Top_Set_CLKSEL5_SRO1CLK(val);
			Ddim_Print(("It set CLKSEL_SRO1CLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL5.bit.SRO1CLK	 = %d\n", IO_CHIPTOP.CLKSEL5.bit.SRO1CLK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel6(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "CNR2CLK" ) ) {
			Dd_Top_Set_CLKSEL6_CNR2CLK(val);
			Ddim_Print(("It set CLKSEL_CNR2CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL6.bit.CNR2CLK	 = %d\n", IO_CHIPTOP.CLKSEL6.bit.CNR2CLK));
		}
		else if ( 0 == strcmp( argv[4], "B2R2CLK" ) ) {
			Dd_Top_Set_CLKSEL6_B2R2CLK(val);
			Ddim_Print(("It set CLKSEL_B2R2CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL6.bit.B2R2CLK	 = %d\n", IO_CHIPTOP.CLKSEL6.bit.B2R2CLK));
		}
		else if ( 0 == strcmp( argv[4], "LTM2CLK" ) ) {
			Dd_Top_Set_CLKSEL6_LTM2CLK(val);
			Ddim_Print(("It set CLKSEL_LTM2CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL6.bit.LTM2CLK	 = %d\n", IO_CHIPTOP.CLKSEL6.bit.LTM2CLK));
		}
		else if ( 0 == strcmp( argv[4], "R2Y2CLK" ) ) {
			Dd_Top_Set_CLKSEL6_R2Y2CLK(val);
			Ddim_Print(("It set CLKSEL_R2Y2CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL6.bit.R2Y2CLK	 = %d\n", IO_CHIPTOP.CLKSEL6.bit.R2Y2CLK));
		}
		else if ( 0 == strcmp( argv[4], "SRO2CLK_2" ) ) {
			Dd_Top_Set_CLKSEL6_SRO2CLK_2(val);
			Ddim_Print(("It set CLKSEL_SRO2CLK_2	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL6.bit.SRO2CLK_2	 = %d\n", IO_CHIPTOP.CLKSEL6.bit.SRO2CLK_2));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel7(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "LTMRBK2CLK" ) ) {
			Dd_Top_Set_CLKSEL7_LTMRBK2CLK(val);
			Ddim_Print(("It set CLKSEL_LTMRBK2CLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL7.bit.LTMRBK2CLK	 = %d\n", IO_CHIPTOP.CLKSEL7.bit.LTMRBK2CLK));
		}
		else if ( 0 == strcmp( argv[4], "B2B2CLK" ) ) {
			Dd_Top_Set_CLKSEL7_B2B2CLK(val);
			Ddim_Print(("It set CLKSEL_B2B2CLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL7.bit.B2B2CLK	 = %d\n", IO_CHIPTOP.CLKSEL7.bit.B2B2CLK));
		}
		else if ( 0 == strcmp( argv[4], "SRO2CLK" ) ) {
			Dd_Top_Set_CLKSEL7_SRO2CLK(val);
			Ddim_Print(("It set CLKSEL_SRO2CLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL7.bit.SRO2CLK	 = %d\n", IO_CHIPTOP.CLKSEL7.bit.SRO2CLK));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "MIPISEL" ) ) {
			Dd_Top_Set_CLKSEL7_MIPISEL(val);
			Ddim_Print(("It set CLKSEL_MIPISEL	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL7.bit.MIPISEL	 = %d\n", IO_CHIPTOP.CLKSEL7.bit.MIPISEL));
		}
		else if ( 0 == strcmp( argv[4], "PDM0SEL" ) ) {
			Dd_Top_Set_CLKSEL7_PDM0SEL(val);
			Ddim_Print(("It set CLKSEL_PDM0SEL	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL7.bit.PDM0SEL	 = %d\n", IO_CHIPTOP.CLKSEL7.bit.PDM0SEL));
		}
		else if ( 0 == strcmp( argv[4], "PDM1SEL" ) ) {
			Dd_Top_Set_CLKSEL7_PDM1SEL(val);
			Ddim_Print(("It set CLKSEL_PDM1SEL	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL7.bit.PDM1SEL	 = %d\n", IO_CHIPTOP.CLKSEL7.bit.PDM1SEL));
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel8(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "DSPCLK" ) ) {
			Dd_Top_Set_CLKSEL8_DSPCLK(val);
			Ddim_Print(("It set CLKSEL_DSPCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.DSPCLK	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.DSPCLK));
		}
		else if ( 0 == strcmp( argv[4], "SPICLK" ) ) {
			Dd_Top_Set_CLKSEL8_SPICLK(val);
			Ddim_Print(("It set CLKSEL_SPICLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.SPICLK	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.SPICLK));
		}
		else if ( 0 == strcmp( argv[4], "AUCLK" ) ) {
			Dd_Top_Set_CLKSEL8_AUCLK(val);
			Ddim_Print(("It set CLKSEL_AUCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.AUCLK	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.AUCLK));
		}
		else if ( 0 == strcmp( argv[4], "AU0SEL" ) ) {
			Dd_Top_Set_CLKSEL8_AU0SEL(val);
			Ddim_Print(("It set CLKSEL_AU0SEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.AU0SEL	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.AU0SEL));
		}
		else if ( 0 == strcmp( argv[4], "AU2SEL" ) ) {
			Dd_Top_Set_CLKSEL8_AU2SEL(val);
			Ddim_Print(("It set CLKSEL_AU2SEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.AU2SEL	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.AU2SEL));
		}
		else if ( 0 == strcmp( argv[4], "AU3SEL" ) ) {
			Dd_Top_Set_CLKSEL8_AU3SEL(val);
			Ddim_Print(("It set CLKSEL_AU3SEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.AU3SEL	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.AU3SEL));
		}
		else if ( 0 == strcmp( argv[4], "NETAUSEL" ) ) {
			Dd_Top_Set_CLKSEL8_NETAUSEL(val);
			Ddim_Print(("It set CLKSEL_NETAUSEL	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.NETAUSEL	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.NETAUSEL));
		}
		else if ( 0 == strcmp( argv[4], "APCLK" ) ) {
			Dd_Top_Set_CLKSEL8_APCLK(val);
			Ddim_Print(("It set CLKSEL_APCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.APCLK	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.APCLK));
		}
		else if ( 0 == strcmp( argv[4], "AP0SEL" ) ) {
			Dd_Top_Set_CLKSEL8_AP0SEL(val);
			Ddim_Print(("It set CLKSEL_AP0SEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.AP0SEL	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.AP0SEL));
		}
		else if ( 0 == strcmp( argv[4], "AP1SEL" ) ) {
			Dd_Top_Set_CLKSEL8_AP1SEL(val);
			Ddim_Print(("It set CLKSEL_AP1SEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.AP1SEL	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.AP1SEL));
		}
		else if ( 0 == strcmp( argv[4], "AP2SEL" ) ) {
			Dd_Top_Set_CLKSEL8_AP2SEL(val);
			Ddim_Print(("It set CLKSEL_AP2SEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.AP2SEL	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.AP2SEL));
		}
		else if ( 0 == strcmp( argv[4], "AP3SEL" ) ) {
			Dd_Top_Set_CLKSEL8_AP3SEL(val);
			Ddim_Print(("It set CLKSEL_AP3SEL		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL8.bit.AP3SEL	 = %d\n", IO_CHIPTOP.CLKSEL8.bit.AP3SEL));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel9(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "PCLK" ) ) {
			Dd_Top_Set_CLKSEL9_PCLK(val);
			Ddim_Print(("It set CLKSEL_PCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL9.bit.PCLK	 = %d\n", IO_CHIPTOP.CLKSEL9.bit.PCLK));
		}
		else if ( 0 == strcmp( argv[4], "HCLK" ) ) {
			Dd_Top_Set_CLKSEL9_HCLK(val);
			Ddim_Print(("It set CLKSEL_HCLK		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL9.bit.HCLK	 = %d\n", IO_CHIPTOP.CLKSEL9.bit.HCLK));
		}
		else if ( 0 == strcmp( argv[4], "HCLKBMH" ) ) {
			Dd_Top_Set_CLKSEL9_HCLKBMH(val);
			Ddim_Print(("It set CLKSEL_HCLKBMH	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL9.bit.HCLKBMH	 = %d\n", IO_CHIPTOP.CLKSEL9.bit.HCLKBMH));
		}
		else if ( 0 == strcmp( argv[4], "ACLKEXS" ) ) {
			Dd_Top_Set_CLKSEL9_ACLKEXS(val);
			Ddim_Print(("It set CLKSEL_ACLKEXS	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL9.bit.ACLKEXS	 = %d\n", IO_CHIPTOP.CLKSEL9.bit.ACLKEXS));
		}
		else if ( 0 == strcmp( argv[4], "ACLK" ) ) {
			Dd_Top_Set_CLKSEL9_ACLK(val);
			Ddim_Print(("It set CLKSEL_ACLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL9.bit.ACLK	 = %d\n", IO_CHIPTOP.CLKSEL9.bit.ACLK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel10(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "ACLK400" ) ) {
			Dd_Top_Set_CLKSEL10_ACLK400(val);
			Ddim_Print(("It set CLKSEL_ACLK400		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL10.bit.ACLK400	 = %d\n", IO_CHIPTOP.CLKSEL10.bit.ACLK400));
		}
		else if ( 0 == strcmp( argv[4], "MCLK200" ) ) {
			Dd_Top_Set_CLKSEL10_MCLK200(val);
			Ddim_Print(("It set CLKSEL_MCLK200	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL10.bit.MCLK200	 = %d\n", IO_CHIPTOP.CLKSEL10.bit.MCLK200));
		}
		else if ( 0 == strcmp( argv[4], "MCLK400" ) ) {
			Dd_Top_Set_CLKSEL10_MCLK400(val);
			Ddim_Print(("It set CLKSEL_MCLK400		 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL10.bit.MCLK400	 = %d\n", IO_CHIPTOP.CLKSEL10.bit.MCLK400));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel11(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "DCHREQ" ) ) {
			Dd_Top_Set_CLKSEL11_DCHREQ(val);
			Ddim_Print(("It set CLKSEL_DCHREQ	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL11.bit.DCHREQ	 = %d\n", IO_CHIPTOP.CLKSEL11.bit.DCHREQ));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clksel12(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "ACLK300" ) ) {
			Dd_Top_Set_CLKSEL12_ACLK300(val);
			Ddim_Print(("It set CLKSEL_ACLK300	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL12.bit.ACLK300	 = %d\n", IO_CHIPTOP.CLKSEL12.bit.ACLK300));
		}
		else if ( 0 == strcmp( argv[4], "GYROCLK" ) ) {
			Dd_Top_Set_CLKSEL12_GYROCLK(val);
			Ddim_Print(("It set CLKSEL_GYROCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL12.bit.GYROCLK	 = %d\n", IO_CHIPTOP.CLKSEL12.bit.GYROCLK));
		}
		else if ( 0 == strcmp( argv[4], "FPT0CLK" ) ) {
			Dd_Top_Set_CLKSEL12_FPT0CLK(val);
			Ddim_Print(("It set CLKSEL_FPT0CLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL12.bit.FPT0CLK	 = %d\n", IO_CHIPTOP.CLKSEL12.bit.FPT0CLK));
		}
		else if ( 0 == strcmp( argv[4], "FPT1CLK" ) ) {
			Dd_Top_Set_CLKSEL12_FPT1CLK(val);
			Ddim_Print(("It set CLKSEL_FPT1CLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL12.bit.FPT1CLK	 = %d\n", IO_CHIPTOP.CLKSEL12.bit.FPT1CLK));
		}
		else if ( 0 == strcmp( argv[4], "MECLK" ) ) {
			Dd_Top_Set_CLKSEL12_MECLK(val);
			Ddim_Print(("It set CLKSEL_MECLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL12.bit.MECLK	 = %d\n", IO_CHIPTOP.CLKSEL12.bit.MECLK));
		}
		else if ( 0 == strcmp( argv[4], "NFBCHCLK" ) ) {
			Dd_Top_Set_CLKSEL12_NFBCHCLK(val);
			Ddim_Print(("It set CLKSEL_NFBCHCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL12.bit.NFBCHCLK	 = %d\n", IO_CHIPTOP.CLKSEL12.bit.NFBCHCLK));
		}
		else if ( 0 == strcmp( argv[4], "RIBCLK" ) ) {
			Dd_Top_Set_CLKSEL12_RIBCLK(val);
			Ddim_Print(("It set CLKSEL_RIBCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL12.bit.RIBCLK	 = %d\n", IO_CHIPTOP.CLKSEL12.bit.RIBCLK));
		}
		else if ( 0 == strcmp( argv[4], "SHDRCLK" ) ) {
			Dd_Top_Set_CLKSEL12_SHDRCLK(val);
			Ddim_Print(("It set CLKSEL_SHDRCLK	 Clock select.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSEL12.bit.SHDRCLK	 = %d\n", IO_CHIPTOP.CLKSEL12.bit.SHDRCLK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt1(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "PL00ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL00ST(val);
			Ddim_Print(("It set PLLCNT1_PL00ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL00ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL00ST));
		}
		else if ( 0 == strcmp( argv[4], "PL01ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL01ST(val);
			Ddim_Print(("It set PLLCNT1_PL01ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL01ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL01ST));
		}
		else if ( 0 == strcmp( argv[4], "PL02ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL02ST(val);
			Ddim_Print(("It set PLLCNT1_PL02ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL02ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL02ST));
		}
		else if ( 0 == strcmp( argv[4], "PL03ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL03ST(val);
			Ddim_Print(("It set PLLCNT1_PL03ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL03ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL03ST));
		}
		else if ( 0 == strcmp( argv[4], "PL04ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL04ST(val);
			Ddim_Print(("It set PLLCNT1_PL04ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL04ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL04ST));
		}
		else if ( 0 == strcmp( argv[4], "PL05ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL05ST(val);
			Ddim_Print(("It set PLLCNT1_PL05ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL05ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL05ST));
		}
		else if ( 0 == strcmp( argv[4], "PL05AST" ) ) {
			Dd_Top_Set_PLLCNT1_PL05AST(val);
			Ddim_Print(("It set PLLCNT1_PL05AST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL05AST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL05AST));
		}
		else if ( 0 == strcmp( argv[4], "PL06ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL06ST(val);
			Ddim_Print(("It set PLLCNT1_PL06ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL06ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL06ST));
		}
		else if ( 0 == strcmp( argv[4], "PL07ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL07ST(val);
			Ddim_Print(("It set PLLCNT1_PL07ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL07ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL07ST));
		}
		else if ( 0 == strcmp( argv[4], "PL08ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL08ST(val);
			Ddim_Print(("It set PLLCNT1_PL08ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL08ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL08ST));
		}
		else if ( 0 == strcmp( argv[4], "PL10ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL10ST(val);
			Ddim_Print(("It set PLLCNT1_PL10ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL10ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL10ST));
		}
		else if ( 0 == strcmp( argv[4], "PL10AST" ) ) {
			Dd_Top_Set_PLLCNT1_PL10AST(val);
			Ddim_Print(("It set PLLCNT1_PL10AST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL10AST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL10AST));
		}
		else if ( 0 == strcmp( argv[4], "PL11ST" ) ) {
			Dd_Top_Set_PLLCNT1_PL11ST(val);
			Ddim_Print(("It set PLLCNT1_PL11ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.PL11ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.PL11ST));
		}
		else if ( 0 == strcmp( argv[4], "DPL00ST" ) ) {
			Dd_Top_Set_PLLCNT1_DPL00ST(val);
			Ddim_Print(("It set PLLCNT1_DPL00ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL00ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL00ST));
		}
		else if ( 0 == strcmp( argv[4], "DPL01ST" ) ) {
			Dd_Top_Set_PLLCNT1_DPL01ST(val);
			Ddim_Print(("It set PLLCNT1_DPL01ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL01ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL01ST));
		}
		else if ( 0 == strcmp( argv[4], "DPL02ST" ) ) {
			Dd_Top_Set_PLLCNT1_DPL02ST(val);
			Ddim_Print(("It set PLLCNT1_DPL02ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL02ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL02ST));
		}
		else if ( 0 == strcmp( argv[4], "DPL10ST" ) ) {
			Dd_Top_Set_PLLCNT1_DPL10ST(val);
			Ddim_Print(("It set PLLCNT1_DPL10ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL10ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL10ST));
		}
		else if ( 0 == strcmp( argv[4], "DPL11ST" ) ) {
			Dd_Top_Set_PLLCNT1_DPL11ST(val);
			Ddim_Print(("It set PLLCNT1_DPL11ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL11ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL11ST));
		}
		else if ( 0 == strcmp( argv[4], "DPL12ST" ) ) {
			Dd_Top_Set_PLLCNT1_DPL12ST(val);
			Ddim_Print(("It set PLLCNT1_DPL12ST PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT1.bit.DPL12ST	= %d\n", IO_CHIPTOP.PLLCNT1.bit.DPL12ST));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt2(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "PL00SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL00SEL(val);
			Ddim_Print(("It set PLLCNT2_PL00SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL00SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL00SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL01SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL01SEL(val);
			Ddim_Print(("It set PLLCNT2_PL01SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL01SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL01SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL02SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL02SEL(val);
			Ddim_Print(("It set PLLCNT2_PL02SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL02SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL02SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL03SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL03SEL(val);
			Ddim_Print(("It set PLLCNT2_PL03SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL03SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL03SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL04SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL04SEL(val);
			Ddim_Print(("It set PLLCNT2_PL04SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL04SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL04SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL05SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL05SEL(val);
			Ddim_Print(("It set PLLCNT2_PL05SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL05SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL05SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL05ASEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL05ASEL(val);
			Ddim_Print(("It set PLLCNT2_PL05ASEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL05ASEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL05ASEL));
		}
		else if ( 0 == strcmp( argv[4], "PL06SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL06SEL(val);
			Ddim_Print(("It set PLLCNT2_PL06SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL06SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL06SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL07SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL07SEL(val);
			Ddim_Print(("It set PLLCNT2_PL07SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL07SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL07SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL08SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL08SEL(val);
			Ddim_Print(("It set PLLCNT2_PL08SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL08SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL08SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL10SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL10SEL(val);
			Ddim_Print(("It set PLLCNT2_PL10SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL10SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL10SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL10ASEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL10ASEL(val);
			Ddim_Print(("It set PLLCNT2_PL10ASEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL10ASEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL10ASEL));
		}
		else if ( 0 == strcmp( argv[4], "PL11SEL" ) ) {
			Dd_Top_Set_PLLCNT2_PL11SEL(val);
			Ddim_Print(("It set PLLCNT2_PL11SEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL11SEL	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL11SEL));
		}
		else if ( 0 == strcmp( argv[4], "PL01SSEN" ) ) {
			Dd_Top_Set_PLLCNT2_PL01SSEN(val);
			Ddim_Print(("It set PLLCNT2_PL01SSEN PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL01SSEN	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL01SSEN));
		}
		else if ( 0 == strcmp( argv[4], "PL02SSEN" ) ) {
			Dd_Top_Set_PLLCNT2_PL02SSEN(val);
			Ddim_Print(("It set PLLCNT2_PL02SSEN PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT2.bit.PL02SSEN	= %d\n", IO_CHIPTOP.PLLCNT2.bit.PL02SSEN));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt3(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "P00POSTDIV" ) ) {
			Dd_Top_Set_PLLCNT3_P00POSTDIV(val);
			Ddim_Print(("It set PLLCNT3_P00POSTDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT3.bit.P00POSTDIV	= %d\n", IO_CHIPTOP.PLLCNT3.bit.P00POSTDIV));
		}
		else if ( 0 == strcmp( argv[4], "P00PREDIV" ) ) {
			Dd_Top_Set_PLLCNT3_P00PREDIV(val);
			Ddim_Print(("It set PLLCNT3_P00PREDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT3.bit.P00PREDIV	= %d\n", IO_CHIPTOP.PLLCNT3.bit.P00PREDIV));
		}
		else if ( 0 == strcmp( argv[4], "P00PLLDIV" ) ) {
			Dd_Top_Set_PLLCNT3_P00PLLDIV(val);
			Ddim_Print(("It set PLLCNT3_P00PLLDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT3.bit.P00PLLDIV	= %d\n", IO_CHIPTOP.PLLCNT3.bit.P00PLLDIV));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt4(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "P00FNUM" ) ) {
			Dd_Top_Set_PLLCNT4_P00FNUM(val);
			Ddim_Print(("It set PLLCNT4_P00FNUM PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT4.bit.P00FNUM	= %lu\n", IO_CHIPTOP.PLLCNT4.bit.P00FNUM));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt5(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "P00FDEN" ) ) {
			Dd_Top_Set_PLLCNT5_P00FDEN(val);
			Ddim_Print(("It set PLLCNT5_P00FDEN PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT5.bit.P00FDEN	= %lu\n", IO_CHIPTOP.PLLCNT5.bit.P00FDEN));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt6(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "P01RATE" ) ) {
			Dd_Top_Set_PLLCNT6_P01RATE(val);
			Ddim_Print(("It set PLLCNT6_P01RATE PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT6.bit.P01RATE	= %d\n", IO_CHIPTOP.PLLCNT6.bit.P01RATE));
		}
		else if ( 0 == strcmp( argv[4], "P01FREQ" ) ) {
			Dd_Top_Set_PLLCNT6_P01FREQ(val);
			Ddim_Print(("It set PLLCNT6_P01FREQ PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT6.bit.P01FREQ	= %d\n", IO_CHIPTOP.PLLCNT6.bit.P01FREQ));
		}
		else if ( 0 == strcmp( argv[4], "P01MODE" ) ) {
			Dd_Top_Set_PLLCNT6_P01MODE(val);
			Ddim_Print(("It set PLLCNT6_P01MODE PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT6.bit.P01MODE	= %d\n", IO_CHIPTOP.PLLCNT6.bit.P01MODE));
		}
		else if ( 0 == strcmp( argv[4], "P02RATE" ) ) {
			Dd_Top_Set_PLLCNT6_P02RATE(val);
			Ddim_Print(("It set PLLCNT6_P02RATE PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT6.bit.P02RATE	= %d\n", IO_CHIPTOP.PLLCNT6.bit.P02RATE));
		}
		else if ( 0 == strcmp( argv[4], "P02FREQ" ) ) {
			Dd_Top_Set_PLLCNT6_P02FREQ(val);
			Ddim_Print(("It set PLLCNT6_P02FREQ PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT6.bit.P02FREQ	= %d\n", IO_CHIPTOP.PLLCNT6.bit.P02FREQ));
		}
		else if ( 0 == strcmp( argv[4], "P02MODE" ) ) {
			Dd_Top_Set_PLLCNT6_P02MODE(val);
			Ddim_Print(("It set PLLCNT6_P02MODE PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT6.bit.P02MODE	= %d\n", IO_CHIPTOP.PLLCNT6.bit.P02MODE));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt7(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "P03POSTDIV0" ) ) {
			Dd_Top_Set_PLLCNT7_P03POSTDIV0(val);
			Ddim_Print(("It set PLLCNT7_P03POSTDIV0 PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV0	= %d\n", IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV0));
		}
		else if ( 0 == strcmp( argv[4], "P03POSTDIV1" ) ) {
			Dd_Top_Set_PLLCNT7_P03POSTDIV1(val);
			Ddim_Print(("It set PLLCNT7_P03POSTDIV1 PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV1	= %d\n", IO_CHIPTOP.PLLCNT7.bit.P03POSTDIV1));
		}
		else if ( 0 == strcmp( argv[4], "P03PREDIV" ) ) {
			Dd_Top_Set_PLLCNT7_P03PREDIV(val);
			Ddim_Print(("It set PLLCNT7_P03PREDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03PREDIV	= %d\n", IO_CHIPTOP.PLLCNT7.bit.P03PREDIV));
		}
		else if ( 0 == strcmp( argv[4], "P03PLLDIV" ) ) {
			Dd_Top_Set_PLLCNT7_P03PLLDIV(val);
			Ddim_Print(("It set PLLCNT7_P03PLLDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03PLLDIV	= %d\n", IO_CHIPTOP.PLLCNT7.bit.P03PLLDIV));
		}
		else if ( 0 == strcmp( argv[4], "P03OC0" ) ) {
			Dd_Top_Set_PLLCNT7_P03OC0(val);
			Ddim_Print(("It set PLLCNT8_P03OC0 PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03OC0	= %d\n", IO_CHIPTOP.PLLCNT7.bit.P03OC0));
		}
		else if ( 0 == strcmp( argv[4], "P03OC1" ) ) {
			Dd_Top_Set_PLLCNT7_P03OC1(val);
			Ddim_Print(("It set PLLCNT8_P03OC1 PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.P03OC1	= %d\n", IO_CHIPTOP.PLLCNT7.bit.P03OC1));
		}
		else if ( 0 == strcmp( argv[4], "D0XOC" ) ) {
			Dd_Top_Set_PLLCNT7_D0XOC(val);
			Ddim_Print(("It set PLLCNT8_D0XOC PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.D0XOC	= %d\n", IO_CHIPTOP.PLLCNT7.bit.D0XOC));
		}
		else if ( 0 == strcmp( argv[4], "D1XOC" ) ) {
			Dd_Top_Set_PLLCNT7_D1XOC(val);
			Ddim_Print(("It set PLLCNT8_D1XOC PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.D1XOC	= %d\n", IO_CHIPTOP.PLLCNT7.bit.D1XOC));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "DPLX8" ) ) {
			Dd_Top_Set_PLLCNT7_DPLX8(val);
			Ddim_Print(("It set PLLCNT8_DPLX8 PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.DPLX8	= %d\n", IO_CHIPTOP.PLLCNT7.bit.DPLX8));
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[4], "DSEL" ) ) {
			Dd_Top_Set_PLLCNT7_DSEL(val);
			Ddim_Print(("It set PLLCNT8_DSEL PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT7.bit.DSEL	= %d\n", IO_CHIPTOP.PLLCNT7.bit.DSEL));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt8(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "P04POSTDIV" ) ) {
			Dd_Top_Set_PLLCNT8_P04POSTDIV(val);
			Ddim_Print(("It set PLLCNT8_P04POSTDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT8.bit.P04POSTDIV	= %d\n", IO_CHIPTOP.PLLCNT8.bit.P04POSTDIV));
		}
		else if ( 0 == strcmp( argv[4], "P04PREDIV" ) ) {
			Dd_Top_Set_PLLCNT8_P04PREDIV(val);
			Ddim_Print(("It set PLLCNT8_P04PREDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT8.bit.P04PREDIV	= %d\n", IO_CHIPTOP.PLLCNT8.bit.P04PREDIV));
		}
		else if ( 0 == strcmp( argv[4], "P04PLLDIV" ) ) {
			Dd_Top_Set_PLLCNT8_P04PLLDIV(val);
			Ddim_Print(("It set PLLCNT8_P04PLLDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT8.bit.P04PLLDIV	= %d\n", IO_CHIPTOP.PLLCNT8.bit.P04PLLDIV));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_pllcnt9(int argc, char** argv)
{
	ULONG val;
	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "P10APLLDIV" ) ) {
			Dd_Top_Set_PLLCNT9_P10APLLDIV(val);
			Ddim_Print(("It set PLLCNT9_P10APLLDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT9.bit.P10APLLDIV	= %d\n", IO_CHIPTOP.PLLCNT9.bit.P10APLLDIV));
		}
		else if ( 0 == strcmp( argv[4], "P08PLLDIV" ) ) {
			Dd_Top_Set_PLLCNT9_P08PLLDIV(val);
			Ddim_Print(("It set PLLCNT9_P08PLLDIV PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT9.bit.P08PLLDIV	= %d\n", IO_CHIPTOP.PLLCNT9.bit.P08PLLDIV));
		}
		else if ( 0 == strcmp( argv[4], "P05CHG" ) ) {
			Dd_Top_Set_PLLCNT9_P05CHG(val);
			Ddim_Print(("It set PLLCNT9_P05CHG PLL State.\n"));
			Ddim_Print(("IO_CHIPTOP.PLLCNT9.bit.P05CHG	= %d\n", IO_CHIPTOP.PLLCNT9.bit.P05CHG));
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop1(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "DSPCK" ) ) {
			Dd_Top_Set_CLKSTOP1_DSPCK(val);
			Ddim_Print(("It set CLKSTOP1_DSPCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.DSPCK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.DSPCK));
		}
		else if ( 0 == strcmp( argv[4], "DSPAX" ) ) {
			Dd_Top_Set_CLKSTOP1_DSPAX(val);
			Ddim_Print(("It set CLKSTOP1_DSPAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.DSPAX	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.DSPAX));
		}
		else if ( 0 == strcmp( argv[4], "SENCK" ) ) {
			Dd_Top_Set_CLKSTOP1_SENCK(val);
			Ddim_Print(("It set CLKSTOP1_SENCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.SENCK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.SENCK));
		}
		else if ( 0 == strcmp( argv[4], "SENAX" ) ) {
			Dd_Top_Set_CLKSTOP1_SENAX(val);
			Ddim_Print(("It set CLKSTOP1_SENAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.SENAX	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.SENAX));
		}
		else if ( 0 == strcmp( argv[4], "SENAH" ) ) {
			Dd_Top_Set_CLKSTOP1_SENAH(val);
			Ddim_Print(("It set CLKSTOP1_SENAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.SENAH	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.SENAH));
		}
		else if ( 0 == strcmp( argv[4], "SENAP" ) ) {
			Dd_Top_Set_CLKSTOP1_SENAP(val);
			Ddim_Print(("It set CLKSTOP1_SENAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.SENAP	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.SENAP));
		}
		else if ( 0 == strcmp( argv[4], "GPIOAP" ) ) {
			Dd_Top_Set_CLKSTOP1_GPIOAP(val);
			Ddim_Print(("It set CLKSTOP1_GPIOAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.GPIOAP	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.GPIOAP));
		}
		else if ( 0 == strcmp( argv[4], "AU0CK" ) ) {
			Dd_Top_Set_CLKSTOP1_AU0CK(val);
			Ddim_Print(("It set CLKSTOP1_AU0CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.AU0CK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.AU0CK));
		}
		else if ( 0 == strcmp( argv[4], "AU2CK" ) ) {
			Dd_Top_Set_CLKSTOP1_AU2CK(val);
			Ddim_Print(("It set CLKSTOP1_AU2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.AU2CK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.AU2CK));
		}
		else if ( 0 == strcmp( argv[4], "AU3CK" ) ) {
			Dd_Top_Set_CLKSTOP1_AU3CK(val);
			Ddim_Print(("It set CLKSTOP1_AU3CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.AU3CK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.AU3CK));
		}
		else if ( 0 == strcmp( argv[4], "AU4CK" ) ) {
			Dd_Top_Set_CLKSTOP1_AU4CK(val);
			Ddim_Print(("It set CLKSTOP1_AU4CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.AU4CK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.AU4CK));
		}
		else if ( 0 == strcmp( argv[4], "AU5CK" ) ) {
			Dd_Top_Set_CLKSTOP1_AU5CK(val);
			Ddim_Print(("It set CLKSTOP1_AU5CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.AU5CK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.AU5CK));
		}
		else if ( 0 == strcmp( argv[4], "NETAUCK" ) ) {
			Dd_Top_Set_CLKSTOP1_NETAUCK(val);
			Ddim_Print(("It set CLKSTOP1_NETAUCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.NETAUCK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.NETAUCK));
		}
		else if ( 0 == strcmp( argv[4], "TEMPCK" ) ) {
			Dd_Top_Set_CLKSTOP1_TEMPCK(val);
			Ddim_Print(("It set CLKSTOP1_TEMPCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP1.bit.TEMPCK	= %d\n", IO_CHIPTOP.CLKSTOP1.bit.TEMPCK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop2(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "RCK" ) ) {
			Dd_Top_Set_CLKSTOP2_RCK(val);
			Ddim_Print(("It set CLKSTOP2_RCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.RCK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.RCK));
		}
		else if ( 0 == strcmp( argv[4], "UHS1CK0" ) ) {
			Dd_Top_Set_CLKSTOP2_UHS1CK0(val);
			Ddim_Print(("It set CLKSTOP2_UHS1CK0 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.UHS1CK0	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.UHS1CK0));
		}
		else if ( 0 == strcmp( argv[4], "UHS1CK1" ) ) {
			Dd_Top_Set_CLKSTOP2_UHS1CK1(val);
			Ddim_Print(("It set CLKSTOP2_UHS1CK1 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.UHS1CK1	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.UHS1CK1));
		}
		else if ( 0 == strcmp( argv[4], "UHS1CK2" ) ) {
			Dd_Top_Set_CLKSTOP2_UHS1CK2(val);
			Ddim_Print(("It set CLKSTOP2_UHS1CK2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.UHS1CK2	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.UHS1CK2));
		}
		else if ( 0 == strcmp( argv[4], "UHS2CK" ) ) {
			Dd_Top_Set_CLKSTOP2_UHS2CK(val);
			Ddim_Print(("It set CLKSTOP2_UHS2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.UHS2CK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.UHS2CK));
		}
		else if ( 0 == strcmp( argv[4], "NFCK" ) ) {
			Dd_Top_Set_CLKSTOP2_NFCK(val);
			Ddim_Print(("It set CLKSTOP2_NFCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.NFCK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.NFCK));
		}
		else if ( 0 == strcmp( argv[4], "EMMCCK" ) ) {
			Dd_Top_Set_CLKSTOP2_EMMCCK(val);
			Ddim_Print(("It set CLKSTOP2_EMMCCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.EMMCCK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.EMMCCK));
		}
		else if ( 0 == strcmp( argv[4], "NETSECCK" ) ) {
			Dd_Top_Set_CLKSTOP2_NETSECCK(val);
			Ddim_Print(("It set CLKSTOP2_NETSECCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.NETSECCK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.NETSECCK));
		}
		else if ( 0 == strcmp( argv[4], "NETRCK" ) ) {
			Dd_Top_Set_CLKSTOP2_NETRCK(val);
			Ddim_Print(("It set CLKSTOP2_NETRCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.NETRCK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.NETRCK));
		}
		else if ( 0 == strcmp( argv[4], "EXSAX" ) ) {
			Dd_Top_Set_CLKSTOP2_EXSAX(val);
			Ddim_Print(("It set CLKSTOP2_EXSAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.EXSAX	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.EXSAX));
		}
		else if ( 0 == strcmp( argv[4], "SPICK" ) ) {
			Dd_Top_Set_CLKSTOP2_SPICK(val);
			Ddim_Print(("It set CLKSTOP2_SPICK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.SPICK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.SPICK));
		}
		else if ( 0 == strcmp( argv[4], "SLIMB00CK" ) ) {
			Dd_Top_Set_CLKSTOP2_SLIMB00CK(val);
			Ddim_Print(("It set CLKSTOP2_SLIMB00CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.SLIMB00CK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.SLIMB00CK));
		}
		else if ( 0 == strcmp( argv[4], "SLIMB01CK" ) ) {
			Dd_Top_Set_CLKSTOP2_SLIMB01CK(val);
			Ddim_Print(("It set CLKSTOP2_SLIMB01CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.SLIMB01CK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.SLIMB01CK));
		}
		else if ( 0 == strcmp( argv[4], "SLIMB10CK" ) ) {
			Dd_Top_Set_CLKSTOP2_SLIMB10CK(val);
			Ddim_Print(("It set CLKSTOP2_SLIMB10CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.SLIMB10CK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.SLIMB10CK));
		}
		else if ( 0 == strcmp( argv[4], "SLIMB11CK" ) ) {
			Dd_Top_Set_CLKSTOP2_SLIMB11CK(val);
			Ddim_Print(("It set CLKSTOP2_SLIMB11CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.SLIMB11CK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.SLIMB11CK));
		}
		else if ( 0 == strcmp( argv[4], "PCISUPPCK" ) ) {
			Dd_Top_Set_CLKSTOP2_PCISUPPCK(val);
			Ddim_Print(("It set CLKSTOP2_PCISUPPCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP2.bit.PCISUPPCK	= %d\n", IO_CHIPTOP.CLKSTOP2.bit.PCISUPPCK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop3(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "IIPCK" ) ) {
			Dd_Top_Set_CLKSTOP3_IIPCK(val);
			Ddim_Print(("It set CLKSTOP3_IIPCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.IIPCK	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.IIPCK));
		}
		else if ( 0 == strcmp( argv[4], "IIPAP" ) ) {
			Dd_Top_Set_CLKSTOP3_IIPAP(val);
			Ddim_Print(("It set CLKSTOP3_IIPAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.IIPAP	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.IIPAP));
		}
		else if ( 0 == strcmp( argv[4], "IIPAH" ) ) {
			Dd_Top_Set_CLKSTOP3_IIPAH(val);
			Ddim_Print(("It set CLKSTOP3_IIPAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.IIPAH	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.IIPAH));
		}
		else if ( 0 == strcmp( argv[4], "IIPAX" ) ) {
			Dd_Top_Set_CLKSTOP3_IIPAX(val);
			Ddim_Print(("It set CLKSTOP3_IIPAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.IIPAX	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.IIPAX));
		}
		else if ( 0 == strcmp( argv[4], "LCDCK" ) ) {
			Dd_Top_Set_CLKSTOP3_LCDCK(val);
			Ddim_Print(("It set CLKSTOP3_LCDCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.LCDCK	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.LCDCK));
		}
		else if ( 0 == strcmp( argv[4], "HIFCK" ) ) {
			Dd_Top_Set_CLKSTOP3_HIFCK(val);
			Ddim_Print(("It set CLKSTOP3_HIFCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.HIFCK	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.HIFCK));
		}
		else if ( 0 == strcmp( argv[4], "MIFCK" ) ) {
			Dd_Top_Set_CLKSTOP3_MIFCK(val);
			Ddim_Print(("It set CLKSTOP3_MIFCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.MIFCK	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.MIFCK));
		}
		else if ( 0 == strcmp( argv[4], "DISPAP" ) ) {
			Dd_Top_Set_CLKSTOP3_DISPAP(val);
			Ddim_Print(("It set CLKSTOP3_DISPAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.DISPAP	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.DISPAP));
		}
		else if ( 0 == strcmp( argv[4], "DISPAH" ) ) {
			Dd_Top_Set_CLKSTOP3_DISPAH(val);
			Ddim_Print(("It set CLKSTOP3_DISPAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.DISPAH	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.DISPAH));
		}
		else if ( 0 == strcmp( argv[4], "DISPAX" ) ) {
			Dd_Top_Set_CLKSTOP3_DISPAX(val);
			Ddim_Print(("It set CLKSTOP3_DISPAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.DISPAX	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.DISPAX));
		}
		else if ( 0 == strcmp( argv[4], "JPGCK" ) ) {
			Dd_Top_Set_CLKSTOP3_JPGCK(val);
			Ddim_Print(("It set CLKSTOP3_JPGCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.JPGCK	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.JPGCK));
		}
		else if ( 0 == strcmp( argv[4], "JPGAP" ) ) {
			Dd_Top_Set_CLKSTOP3_JPGAP(val);
			Ddim_Print(("It set CLKSTOP3_JPGAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.JPGAP	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.JPGAP));
		}
		else if ( 0 == strcmp( argv[4], "JPGAH" ) ) {
			Dd_Top_Set_CLKSTOP3_JPGAH(val);
			Ddim_Print(("It set CLKSTOP3_JPGAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.JPGAH	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.JPGAH));
		}
		else if ( 0 == strcmp( argv[4], "JPGAX" ) ) {
			Dd_Top_Set_CLKSTOP3_JPGAX(val);
			Ddim_Print(("It set CLKSTOP3_JPGAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.JPGAX	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.JPGAX));
		}
		else if ( 0 == strcmp( argv[4], "PDM0CK" ) ) {
			Dd_Top_Set_CLKSTOP3_PDM0CK(val);
			Ddim_Print(("It set CLKSTOP3_PDM0CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.PDM0CK	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.PDM0CK));
		}
		else if ( 0 == strcmp( argv[4], "PDM1CK" ) ) {
			Dd_Top_Set_CLKSTOP3_PDM1CK(val);
			Ddim_Print(("It set CLKSTOP3_PDM1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP3.bit.PDM1CK	= %d\n", IO_CHIPTOP.CLKSTOP3.bit.PDM1CK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop4(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "GPUCK" ) ) {
			Dd_Top_Set_CLKSTOP4_GPUCK(val);
			Ddim_Print(("It set CLKSTOP4_GPUCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.GPUCK	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.GPUCK));
		}
		else if ( 0 == strcmp( argv[4], "GPUAP" ) ) {
			Dd_Top_Set_CLKSTOP4_GPUAP(val);
			Ddim_Print(("It set CLKSTOP4_GPUAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.GPUAP	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.GPUAP));
		}
		else if ( 0 == strcmp( argv[4], "GPUAH" ) ) {
			Dd_Top_Set_CLKSTOP4_GPUAH(val);
			Ddim_Print(("It set CLKSTOP4_GPUAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.GPUAH	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.GPUAH));
		}
		else if ( 0 == strcmp( argv[4], "GPUAX" ) ) {
			Dd_Top_Set_CLKSTOP4_GPUAX(val);
			Ddim_Print(("It set CLKSTOP4_GPUAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.GPUAX	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.GPUAX));
		}
		else if ( 0 == strcmp( argv[4], "FPT0CK" ) ) {
			Dd_Top_Set_CLKSTOP4_FPT0CK(val);
			Ddim_Print(("It set CLKSTOP4_FPT0CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.FPT0CK	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.FPT0CK));
		}
		else if ( 0 == strcmp( argv[4], "FPT0AX" ) ) {
			Dd_Top_Set_CLKSTOP4_FPT0AX(val);
			Ddim_Print(("It set CLKSTOP4_FPT0AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.FPT0AX	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.FPT0AX));
		}
		else if ( 0 == strcmp( argv[4], "FPT0AH" ) ) {
			Dd_Top_Set_CLKSTOP4_FPT0AH(val);
			Ddim_Print(("It set CLKSTOP4_FPT0AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.FPT0AH	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.FPT0AH));
		}
		else if ( 0 == strcmp( argv[4], "FPT0AP" ) ) {
			Dd_Top_Set_CLKSTOP4_FPT0AP(val);
			Ddim_Print(("It set CLKSTOP4_FPT0AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.FPT0AP	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.FPT0AP));
		}
		else if ( 0 == strcmp( argv[4], "FPT1CK" ) ) {
			Dd_Top_Set_CLKSTOP4_FPT1CK(val);
			Ddim_Print(("It set CLKSTOP4_FPT1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.FPT1CK	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.FPT1CK));
		}
		else if ( 0 == strcmp( argv[4], "FPT1AP" ) ) {
			Dd_Top_Set_CLKSTOP4_FPT1AP(val);
			Ddim_Print(("It set CLKSTOP4_FPT1AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.FPT1AP	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.FPT1AP));
		}
		else if ( 0 == strcmp( argv[4], "FPT1AH" ) ) {
			Dd_Top_Set_CLKSTOP4_FPT1AH(val);
			Ddim_Print(("It set CLKSTOP4_FPT1AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.FPT1AH	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.FPT1AH));
		}
		else if ( 0 == strcmp( argv[4], "FPT1AX" ) ) {
			Dd_Top_Set_CLKSTOP4_FPT1AX(val);
			Ddim_Print(("It set CLKSTOP4_FPT1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.FPT1AX	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.FPT1AX));
		}
		else if ( 0 == strcmp( argv[4], "APCK0" ) ) {
			Dd_Top_Set_CLKSTOP4_APCK0(val);
			Ddim_Print(("It set CLKSTOP4_APCK0 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.APCK0	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.APCK0));
		}
		else if ( 0 == strcmp( argv[4], "APCK1" ) ) {
			Dd_Top_Set_CLKSTOP4_APCK1(val);
			Ddim_Print(("It set CLKSTOP4_APCK1 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.APCK1	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.APCK1));
		}
		else if ( 0 == strcmp( argv[4], "APCK2" ) ) {
			Dd_Top_Set_CLKSTOP4_APCK2(val);
			Ddim_Print(("It set CLKSTOP4_APCK2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.APCK2	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.APCK2));
		}
		else if ( 0 == strcmp( argv[4], "APCK3" ) ) {
			Dd_Top_Set_CLKSTOP4_APCK3(val);
			Ddim_Print(("It set CLKSTOP4_APCK3 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP4.bit.APCK3	= %d\n", IO_CHIPTOP.CLKSTOP4.bit.APCK3));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop5(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "MICAX0" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAX0(val);
			Ddim_Print(("It set CLKSTOP5_MICAX0 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAX0	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAX0));
		}
		else if ( 0 == strcmp( argv[4], "MICAX1" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAX1(val);
			Ddim_Print(("It set CLKSTOP5_MICAX1 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAX1	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAX1));
		}
		else if ( 0 == strcmp( argv[4], "MICAX2" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAX2(val);
			Ddim_Print(("It set CLKSTOP5_MICAX2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAX2	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAX2));
		}
		else if ( 0 == strcmp( argv[4], "MICAX3" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAX3(val);
			Ddim_Print(("It set CLKSTOP5_MICAX3 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAX3	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAX3));
		}
		else if ( 0 == strcmp( argv[4], "MICAX4" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAX4(val);
			Ddim_Print(("It set CLKSTOP5_MICAX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAX4	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAX4));
		}
		else if ( 0 == strcmp( argv[4], "MICAX5" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAX5(val);
			Ddim_Print(("It set CLKSTOP5_MICAX5 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAX5	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAX5));
		}
		else if ( 0 == strcmp( argv[4], "MICAX6" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAX6(val);
			Ddim_Print(("It set CLKSTOP5_MICAX6 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAX6	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAX6));
		}
		else if ( 0 == strcmp( argv[4], "MICAP0" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAP0(val);
			Ddim_Print(("It set CLKSTOP5_MICAP0 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAP0	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAP0));
		}
		else if ( 0 == strcmp( argv[4], "MICAP1" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAP1(val);
			Ddim_Print(("It set CLKSTOP5_MICAP1 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAP1	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAP1));
		}
		else if ( 0 == strcmp( argv[4], "MICAP2" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAP2(val);
			Ddim_Print(("It set CLKSTOP5_MICAP2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAP2	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAP2));
		}
		else if ( 0 == strcmp( argv[4], "MICAP3" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAP3(val);
			Ddim_Print(("It set CLKSTOP5_MICAP3 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAP3	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAP3));
		}
		else if ( 0 == strcmp( argv[4], "MICAP4" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAP4(val);
			Ddim_Print(("It set CLKSTOP5_MICAP4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAP4	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAP4));
		}
		else if ( 0 == strcmp( argv[4], "MICAP5" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAP5(val);
			Ddim_Print(("It set CLKSTOP5_MICAP5 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAP5	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAP5));
		}
		else if ( 0 == strcmp( argv[4], "MICAP6" ) ) {
			Dd_Top_Set_CLKSTOP5_MICAP6(val);
			Ddim_Print(("It set CLKSTOP5_MICAP6 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP5.bit.MICAP6	= %d\n", IO_CHIPTOP.CLKSTOP5.bit.MICAP6));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop6(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "MICAH1" ) ) {
			Dd_Top_Set_CLKSTOP6_MICAH1(val);
			Ddim_Print(("It set CLKSTOP6_MICAH1 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.MICAH1	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.MICAH1));
		}
		else if ( 0 == strcmp( argv[4], "MICAH2" ) ) {
			Dd_Top_Set_CLKSTOP6_MICAH2(val);
			Ddim_Print(("It set CLKSTOP6_MICAH2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.MICAH2	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.MICAH2));
		}
		else if ( 0 == strcmp( argv[4], "MICAH3" ) ) {
			Dd_Top_Set_CLKSTOP6_MICAH3(val);
			Ddim_Print(("It set CLKSTOP6_MICAH3 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.MICAH3	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.MICAH3));
		}
		else if ( 0 == strcmp( argv[4], "IMGAX" ) ) {
			Dd_Top_Set_CLKSTOP6_IMGAX(val);
			Ddim_Print(("It set CLKSTOP6_IMGAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.IMGAX	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.IMGAX));
		}
		else if ( 0 == strcmp( argv[4], "IMGAH0" ) ) {
			Dd_Top_Set_CLKSTOP6_IMGAH0(val);
			Ddim_Print(("It set CLKSTOP6_IMGAH0 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.IMGAH0	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.IMGAH0));
		}
		else if ( 0 == strcmp( argv[4], "IMGAH1" ) ) {
			Dd_Top_Set_CLKSTOP6_IMGAH1(val);
			Ddim_Print(("It set CLKSTOP6_IMGAH1 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.IMGAH1	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.IMGAH1));
		}
		else if ( 0 == strcmp( argv[4], "IMGAH3" ) ) {
			Dd_Top_Set_CLKSTOP6_IMGAH3(val);
			Ddim_Print(("It set CLKSTOP6_IMGAH3 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.IMGAH3	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.IMGAH3));
		}
		else if ( 0 == strcmp( argv[4], "IMGAP3" ) ) {
			Dd_Top_Set_CLKSTOP6_IMGAP3(val);
			Ddim_Print(("It set CLKSTOP6_IMGAP3 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.IMGAP3	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.IMGAP3));
		}
		else if ( 0 == strcmp( argv[4], "REGAP" ) ) {
			Dd_Top_Set_CLKSTOP6_REGAP(val);
			Ddim_Print(("It set CLKSTOP6_REGAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.REGAP	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.REGAP));
		}
		else if ( 0 == strcmp( argv[4], "XCHAX" ) ) {
			Dd_Top_Set_CLKSTOP6_XCHAX(val);
			Ddim_Print(("It set CLKSTOP6_XCHAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.XCHAX	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.XCHAX));
		}
		else if ( 0 == strcmp( argv[4], "XCHAP" ) ) {
			Dd_Top_Set_CLKSTOP6_XCHAP(val);
			Ddim_Print(("It set CLKSTOP6_XCHAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.XCHAP	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.XCHAP));
		}
		else if ( 0 == strcmp( argv[4], "ELACK" ) ) {
			Dd_Top_Set_CLKSTOP6_ELACK(val);
			Ddim_Print(("It set CLKSTOP6_ELACK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.ELACK	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.ELACK));
		}
		else if ( 0 == strcmp( argv[4], "ELAAX" ) ) {
			Dd_Top_Set_CLKSTOP6_ELAAX(val);
			Ddim_Print(("It set CLKSTOP6_ELAAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.ELAAX	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.ELAAX));
		}
		else if ( 0 == strcmp( argv[4], "ELAAP" ) ) {
			Dd_Top_Set_CLKSTOP6_ELAAP(val);
			Ddim_Print(("It set CLKSTOP6_ELAAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP6.bit.ELAAP	= %d\n", IO_CHIPTOP.CLKSTOP6.bit.ELAAP));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop7(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "IPUFDCK" ) ) {
			Dd_Top_Set_CLKSTOP7_IPUFDCK(val);
			Ddim_Print(("It set CLKSTOP7_IPUFDCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.IPUFDCK	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.IPUFDCK));
		}
		else if ( 0 == strcmp( argv[4], "IPUVISCK" ) ) {
			Dd_Top_Set_CLKSTOP7_IPUVISCK(val);
			Ddim_Print(("It set CLKSTOP7_IPUVISCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.IPUVISCK	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.IPUVISCK));
		}
		else if ( 0 == strcmp( argv[4], "IPUAX" ) ) {
			Dd_Top_Set_CLKSTOP7_IPUAX(val);
			Ddim_Print(("It set CLKSTOP7_IPUAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.IPUAX	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.IPUAX));
		}
		else if ( 0 == strcmp( argv[4], "IPUAH" ) ) {
			Dd_Top_Set_CLKSTOP7_IPUAH(val);
			Ddim_Print(("It set CLKSTOP7_IPUAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.IPUAH	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.IPUAH));
		}
		else if ( 0 == strcmp( argv[4], "RAWCK" ) ) {
			Dd_Top_Set_CLKSTOP7_RAWCK(val);
			Ddim_Print(("It set CLKSTOP7_RAWCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.RAWCK	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.RAWCK));
		}
		else if ( 0 == strcmp( argv[4], "RAWAX" ) ) {
			Dd_Top_Set_CLKSTOP7_RAWAX(val);
			Ddim_Print(("It set CLKSTOP7_RAWAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.RAWAX	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.RAWAX));
		}
		else if ( 0 == strcmp( argv[4], "RAWAP" ) ) {
			Dd_Top_Set_CLKSTOP7_RAWAP(val);
			Ddim_Print(("It set CLKSTOP7_RAWAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.RAWAP	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.RAWAP));
		}
		else if ( 0 == strcmp( argv[4], "SHDRCK" ) ) {
			Dd_Top_Set_CLKSTOP7_SHDRCK(val);
			Ddim_Print(("It set CLKSTOP7_SHDRCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.SHDRCK	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.SHDRCK));
		}
		else if ( 0 == strcmp( argv[4], "SHDRAX" ) ) {
			Dd_Top_Set_CLKSTOP7_SHDRAX(val);
			Ddim_Print(("It set CLKSTOP7_SHDRAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.SHDRAX	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.SHDRAX));
		}
		else if ( 0 == strcmp( argv[4], "SHDRAH" ) ) {
			Dd_Top_Set_CLKSTOP7_SHDRAH(val);
			Ddim_Print(("It set CLKSTOP7_SHDRAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.SHDRAH	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.SHDRAH));
		}
		else if ( 0 == strcmp( argv[4], "SHDRAP" ) ) {
			Dd_Top_Set_CLKSTOP7_SHDRAP(val);
			Ddim_Print(("It set CLKSTOP7_SHDRAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.SHDRAP	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.SHDRAP));
		}
		else if ( 0 == strcmp( argv[4], "M0CK" ) ) {
			Dd_Top_Set_CLKSTOP7_M0CK(val);
			Ddim_Print(("It set CLKSTOP7_M0CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.M0CK	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.M0CK));
		}
		else if ( 0 == strcmp( argv[4], "MECK" ) ) {
			Dd_Top_Set_CLKSTOP7_MECK(val);
			Ddim_Print(("It set CLKSTOP7_MECK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.MECK	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.MECK));
		}
		else if ( 0 == strcmp( argv[4], "MEAX" ) ) {
			Dd_Top_Set_CLKSTOP7_MEAX(val);
			Ddim_Print(("It set CLKSTOP7_MEAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.MEAX	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.MEAX));
		}
		else if ( 0 == strcmp( argv[4], "MEAP" ) ) {
			Dd_Top_Set_CLKSTOP7_MEAP(val);
			Ddim_Print(("It set CLKSTOP7_MEAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP7.bit.MEAP	= %d\n", IO_CHIPTOP.CLKSTOP7.bit.MEAP));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop8(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "RIBCK" ) ) {
			Dd_Top_Set_CLKSTOP8_RIBCK(val);
			Ddim_Print(("It set CLKSTOP8_RIBCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.RIBCK	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.RIBCK));
		}
		else if ( 0 == strcmp( argv[4], "RIBAH" ) ) {
			Dd_Top_Set_CLKSTOP8_RIBAH(val);
			Ddim_Print(("It set CLKSTOP8_RIBAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.RIBAH	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.RIBAH));
		}
		else if ( 0 == strcmp( argv[4], "HEVDFCK" ) ) {
			Dd_Top_Set_CLKSTOP8_HEVDFCK(val);
			Ddim_Print(("It set CLKSTOP8_HEVDFCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.HEVDFCK	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.HEVDFCK));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "HEPXFCK" ) ) {
			Dd_Top_Set_CLKSTOP8_HEPXFCK(val);
			Ddim_Print(("It set CLKSTOP8_HEPXFCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.HEPXFCK	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.HEPXFCK));
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "HEVENCCK" ) ) {
			Dd_Top_Set_CLKSTOP8_HEVENCCK(val);
			Ddim_Print(("It set CLKSTOP8_HEVENCCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.HEVENCCK	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.HEVENCCK));
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[4], "HEIPPCK" ) ) {
			Dd_Top_Set_CLKSTOP8_HEIPPCK(val);
			Ddim_Print(("It set CLKSTOP8_HEIPPCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.HEIPPCK	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.HEIPPCK));
		}
		else if ( 0 == strcmp( argv[4], "UMC0HEVCMX2" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC0HEVCMX2(val);
			Ddim_Print(("It set CLKSTOP8_UMC0HEVCMX2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX2	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX2));
		}
		else if ( 0 == strcmp( argv[4], "UMC0HEVCMX4" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC0HEVCMX4(val);
			Ddim_Print(("It set CLKSTOP8_UMC0HEVCMX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX4	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC0HEVCMX4));
		}
		else if ( 0 == strcmp( argv[4], "UMC0RBRMX4" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC0RBRMX4(val);
			Ddim_Print(("It set CLKSTOP8_UMC0RBRMX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC0RBRMX4	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC0RBRMX4));
		}
		else if ( 0 == strcmp( argv[4], "UMC1HEVCMX2" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC1HEVCMX2(val);
			Ddim_Print(("It set CLKSTOP8_UMC1HEVCMX2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX2	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX2));
		}
		else if ( 0 == strcmp( argv[4], "UMC1HEVCMX4" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC1HEVCMX4(val);
			Ddim_Print(("It set CLKSTOP8_UMC1HEVCMX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX4	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC1HEVCMX4));
		}
		else if ( 0 == strcmp( argv[4], "UMC1RBRMX4" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC1RBRMX4(val);
			Ddim_Print(("It set CLKSTOP8_UMC1RBRMX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC1RBRMX4	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC1RBRMX4));
		}
		else if ( 0 == strcmp( argv[4], "UMC0CMNAX" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC0CMNAX(val);
			Ddim_Print(("It set CLKSTOP8_UMC0CMNAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC0CMNAX	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC0CMNAX));
		}
		else if ( 0 == strcmp( argv[4], "UMC0MX1AX" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC0MX1AX(val);
			Ddim_Print(("It set CLKSTOP8_UMC0MX1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC0MX1AX	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC0MX1AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC0MX2AX" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC0MX2AX(val);
			Ddim_Print(("It set CLKSTOP8_UMC0MX2AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC0MX2AX	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC0MX2AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC0MX3AX" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC0MX3AX(val);
			Ddim_Print(("It set CLKSTOP8_UMC0MX3AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC0MX3AX	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC0MX3AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC0MX4AX" ) ) {
			Dd_Top_Set_CLKSTOP8_UMC0MX4AX(val);
			Ddim_Print(("It set CLKSTOP8_UMC0MX4AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP8.bit.UMC0MX4AX	= %d\n", IO_CHIPTOP.CLKSTOP8.bit.UMC0MX4AX));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop9(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "UMC0MX5AX" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC0MX5AX(val);
			Ddim_Print(("It set CLKSTOP9_UMC0MX5AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC0MX5AX	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC0MX5AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC1CMNAX" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1CMNAX(val);
			Ddim_Print(("It set CLKSTOP9_UMC1CMNAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1CMNAX	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1CMNAX));
		}
		else if ( 0 == strcmp( argv[4], "UMC1MX1AX" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1MX1AX(val);
			Ddim_Print(("It set CLKSTOP9_UMC1MX1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1MX1AX	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1MX1AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC1MX2AX" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1MX2AX(val);
			Ddim_Print(("It set CLKSTOP9_UMC1MX2AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1MX2AX	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1MX2AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC1MX3AX" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1MX3AX(val);
			Ddim_Print(("It set CLKSTOP9_UMC1MX3AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1MX3AX	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1MX3AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC1MX4AX" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1MX4AX(val);
			Ddim_Print(("It set CLKSTOP9_UMC1MX4AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1MX4AX	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1MX4AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC1MX5AX" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1MX5AX(val);
			Ddim_Print(("It set CLKSTOP9_UMC1MX5AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1MX5AX	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1MX5AX));
		}
		else if ( 0 == strcmp( argv[4], "UMC0MX0AX3" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC0MX0AX3(val);
			Ddim_Print(("It set CLKSTOP9_UMC0MX0AX3 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC0MX0AX3	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC0MX0AX3));
		}
		else if ( 0 == strcmp( argv[4], "UMC0MX6AX4" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC0MX6AX4(val);
			Ddim_Print(("It set CLKSTOP9_UMC0MX6AX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC0MX6AX4	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC0MX6AX4));
		}
		else if ( 0 == strcmp( argv[4], "UMC0HEVCAX4" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC0HEVCAX4(val);
			Ddim_Print(("It set CLKSTOP9_UMC0HEVCAX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC0HEVCAX4	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC0HEVCAX4));
		}
		else if ( 0 == strcmp( argv[4], "UMC1MX0AX3" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1MX0AX3(val);
			Ddim_Print(("It set CLKSTOP9_UMC1MX0AX3 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1MX0AX3	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1MX0AX3));
		}
		else if ( 0 == strcmp( argv[4], "UMC1MX6AX4" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1MX6AX4(val);
			Ddim_Print(("It set CLKSTOP9_UMC1MX6AX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1MX6AX4	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1MX6AX4));
		}
		else if ( 0 == strcmp( argv[4], "UMC1HEVCAX4" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1HEVCAX4(val);
			Ddim_Print(("It set CLKSTOP9_UMC1HEVCAX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1HEVCAX4	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1HEVCAX4));
		}
		else if ( 0 == strcmp( argv[4], "UMC0AP" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC0AP(val);
			Ddim_Print(("It set CLKSTOP9_UMC0AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC0AP	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC0AP));
		}
		else if ( 0 == strcmp( argv[4], "UMC1AP" ) ) {
			Dd_Top_Set_CLKSTOP9_UMC1AP(val);
			Ddim_Print(("It set CLKSTOP9_UMC1AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP9.bit.UMC1AP	= %d\n", IO_CHIPTOP.CLKSTOP9.bit.UMC1AP));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop10(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "SRO1CK" ) ) {
			Dd_Top_Set_CLKSTOP10_SRO1CK(val);
			Ddim_Print(("It set CLKSTOP_10SRO1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.SRO1CK	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.SRO1CK));
		}
		else if ( 0 == strcmp( argv[4], "SRO1CK_2" ) ) {
			Dd_Top_Set_CLKSTOP10_SRO1CK_2(val);
			Ddim_Print(("It set CLKSTOP_10SRO1CK_2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.SRO1CK_2	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.SRO1CK_2));
		}
		else if ( 0 == strcmp( argv[4], "SRO1AX" ) ) {
			Dd_Top_Set_CLKSTOP10_SRO1AX(val);
			Ddim_Print(("It set CLKSTOP_10SRO1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.SRO1AX	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.SRO1AX));
		}
		else if ( 0 == strcmp( argv[4], "SRO1AH" ) ) {
			Dd_Top_Set_CLKSTOP10_SRO1AH(val);
			Ddim_Print(("It set CLKSTOP_10SRO1AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.SRO1AH	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.SRO1AH));
		}
		else if ( 0 == strcmp( argv[4], "SRO1AP" ) ) {
			Dd_Top_Set_CLKSTOP10_SRO1AP(val);
			Ddim_Print(("It set CLKSTOP_10SRO1AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.SRO1AP	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.SRO1AP));
		}
		else if ( 0 == strcmp( argv[4], "B2B1CK" ) ) {
			Dd_Top_Set_CLKSTOP10_B2B1CK(val);
			Ddim_Print(("It set CLKSTOP_10B2B1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.B2B1CK	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.B2B1CK));
		}
		else if ( 0 == strcmp( argv[4], "B2B1AX" ) ) {
			Dd_Top_Set_CLKSTOP10_B2B1AX(val);
			Ddim_Print(("It set CLKSTOP_10B2B1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.B2B1AX	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.B2B1AX));
		}
		else if ( 0 == strcmp( argv[4], "B2B1AH" ) ) {
			Dd_Top_Set_CLKSTOP10_B2B1AH(val);
			Ddim_Print(("It set CLKSTOP_10B2B1AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.B2B1AH	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.B2B1AH));
		}
		else if ( 0 == strcmp( argv[4], "B2B1AP" ) ) {
			Dd_Top_Set_CLKSTOP10_B2B1AP(val);
			Ddim_Print(("It set CLKSTOP_10B2B1AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.B2B1AP	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.B2B1AP));
		}
		else if ( 0 == strcmp( argv[4], "LTMRBK1CK" ) ) {
			Dd_Top_Set_CLKSTOP10_LTMRBK1CK(val);
			Ddim_Print(("It set CLKSTOP_10LTMRBK1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.LTMRBK1CK	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.LTMRBK1CK));
		}
		else if ( 0 == strcmp( argv[4], "B2R1CK" ) ) {
			Dd_Top_Set_CLKSTOP10_B2R1CK(val);
			Ddim_Print(("It set CLKSTOP_10B2R1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.B2R1CK	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.B2R1CK));
		}
		else if ( 0 == strcmp( argv[4], "B2R1AX" ) ) {
			Dd_Top_Set_CLKSTOP10_B2R1AX(val);
			Ddim_Print(("It set CLKSTOP_10B2R1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.B2R1AX	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.B2R1AX));
		}
		else if ( 0 == strcmp( argv[4], "B2R1AH" ) ) {
			Dd_Top_Set_CLKSTOP10_B2R1AH(val);
			Ddim_Print(("It set CLKSTOP_10B2R1AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.B2R1AH	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.B2R1AH));
		}
		else if ( 0 == strcmp( argv[4], "B2R1AP" ) ) {
			Dd_Top_Set_CLKSTOP10_B2R1AP(val);
			Ddim_Print(("It set CLKSTOP_10B2R1AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP10.bit.B2R1AP	= %d\n", IO_CHIPTOP.CLKSTOP10.bit.B2R1AP));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop11(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "LTM1CK" ) ) {
			Dd_Top_Set_CLKSTOP11_LTM1CK(val);
			Ddim_Print(("It set CLKSTOP_11LTM1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.LTM1CK	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.LTM1CK));
		}
		else if ( 0 == strcmp( argv[4], "LTM1AX" ) ) {
			Dd_Top_Set_CLKSTOP11_LTM1AX(val);
			Ddim_Print(("It set CLKSTOP_11LTM1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.LTM1AX	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.LTM1AX));
		}
		else if ( 0 == strcmp( argv[4], "LTM1AH" ) ) {
			Dd_Top_Set_CLKSTOP11_LTM1AH(val);
			Ddim_Print(("It set CLKSTOP_11LTM1AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.LTM1AH	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.LTM1AH));
		}
		else if ( 0 == strcmp( argv[4], "LTM1AP" ) ) {
			Dd_Top_Set_CLKSTOP11_LTM1AP(val);
			Ddim_Print(("It set CLKSTOP_11LTM1AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.LTM1AP	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.LTM1AP));
		}
		else if ( 0 == strcmp( argv[4], "R2Y1CK" ) ) {
			Dd_Top_Set_CLKSTOP11_R2Y1CK(val);
			Ddim_Print(("It set CLKSTOP_11R2Y1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.R2Y1CK	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.R2Y1CK));
		}
		else if ( 0 == strcmp( argv[4], "R2Y1AX" ) ) {
			Dd_Top_Set_CLKSTOP11_R2Y1AX(val);
			Ddim_Print(("It set CLKSTOP_11R2Y1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.R2Y1AX	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.R2Y1AX));
		}
		else if ( 0 == strcmp( argv[4], "R2Y1AH" ) ) {
			Dd_Top_Set_CLKSTOP11_R2Y1AH(val);
			Ddim_Print(("It set CLKSTOP_11R2Y1AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.R2Y1AH	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.R2Y1AH));
		}
		else if ( 0 == strcmp( argv[4], "R2Y1AP" ) ) {
			Dd_Top_Set_CLKSTOP11_R2Y1AP(val);
			Ddim_Print(("It set CLKSTOP_11R2Y1AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.R2Y1AP	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.R2Y1AP));
		}
		else if ( 0 == strcmp( argv[4], "CNR1CK" ) ) {
			Dd_Top_Set_CLKSTOP11_CNR1CK(val);
			Ddim_Print(("It set CLKSTOP_11CNR1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.CNR1CK	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.CNR1CK));
		}
		else if ( 0 == strcmp( argv[4], "CNR1AX" ) ) {
			Dd_Top_Set_CLKSTOP11_CNR1AX(val);
			Ddim_Print(("It set CLKSTOP_11CNR1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.CNR1AX	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.CNR1AX));
		}
		else if ( 0 == strcmp( argv[4], "CNR1AP" ) ) {
			Dd_Top_Set_CLKSTOP11_CNR1AP(val);
			Ddim_Print(("It set CLKSTOP_11CNR1AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.CNR1AP	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.CNR1AP));
		}
		else if ( 0 == strcmp( argv[4], "APAH" ) ) {
			Dd_Top_Set_CLKSTOP11_APAH(val);
			Ddim_Print(("It set CLKSTOP_APAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.APAH	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.APAH));
		}
		else if ( 0 == strcmp( argv[4], "DBGAP" ) ) {
			Dd_Top_Set_CLKSTOP11_DBGAP(val);
			Ddim_Print(("It set CLKSTOP_DBGAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.DBGAP	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.DBGAP));
		}
		else if ( 0 == strcmp( argv[4], "NFBCHCK" ) ) {
			Dd_Top_Set_CLKSTOP11_NFBCHCK(val);
			Ddim_Print(("It set CLKSTOP_NFBCHCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP11.bit.NFBCHCK	= %d\n", IO_CHIPTOP.CLKSTOP11.bit.NFBCHCK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop12(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "SRO2CK" ) ) {
			Dd_Top_Set_CLKSTOP12_SRO2CK(val);
			Ddim_Print(("It set CLKSTOP_12SRO2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.SRO2CK	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.SRO2CK));
		}
		else if ( 0 == strcmp( argv[4], "SRO2CK_2" ) ) {
			Dd_Top_Set_CLKSTOP12_SRO2CK_2(val);
			Ddim_Print(("It set CLKSTOP_12SRO2CK_2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.SRO2CK_2	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.SRO2CK_2));
		}
		else if ( 0 == strcmp( argv[4], "SRO2AX" ) ) {
			Dd_Top_Set_CLKSTOP12_SRO2AX(val);
			Ddim_Print(("It set CLKSTOP_12SRO2AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.SRO2AX	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.SRO2AX));
		}
		else if ( 0 == strcmp( argv[4], "SRO2AH" ) ) {
			Dd_Top_Set_CLKSTOP12_SRO2AH(val);
			Ddim_Print(("It set CLKSTOP_12SRO2AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.SRO2AH	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.SRO2AH));
		}
		else if ( 0 == strcmp( argv[4], "SRO2AP" ) ) {
			Dd_Top_Set_CLKSTOP12_SRO2AP(val);
			Ddim_Print(("It set CLKSTOP_12SRO2AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.SRO2AP	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.SRO2AP));
		}
		else if ( 0 == strcmp( argv[4], "B2B2CK" ) ) {
			Dd_Top_Set_CLKSTOP12_B2B2CK(val);
			Ddim_Print(("It set CLKSTOP_12B2B2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.B2B2CK	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.B2B2CK));
		}
		else if ( 0 == strcmp( argv[4], "B2B2AX" ) ) {
			Dd_Top_Set_CLKSTOP12_B2B2AX(val);
			Ddim_Print(("It set CLKSTOP_12B2B2AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.B2B2AX	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.B2B2AX));
		}
		else if ( 0 == strcmp( argv[4], "B2B2AH" ) ) {
			Dd_Top_Set_CLKSTOP12_B2B2AH(val);
			Ddim_Print(("It set CLKSTOP_12B2B2AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.B2B2AH	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.B2B2AH));
		}
		else if ( 0 == strcmp( argv[4], "B2B2AP" ) ) {
			Dd_Top_Set_CLKSTOP12_B2B2AP(val);
			Ddim_Print(("It set CLKSTOP_12B2B2AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.B2B2AP	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.B2B2AP));
		}
		else if ( 0 == strcmp( argv[4], "LTMRBK2CK" ) ) {
			Dd_Top_Set_CLKSTOP12_LTMRBK2CK(val);
			Ddim_Print(("It set CLKSTOP_12LTMRBK2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.LTMRBK2CK	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.LTMRBK2CK));
		}
		else if ( 0 == strcmp( argv[4], "B2R2CK" ) ) {
			Dd_Top_Set_CLKSTOP12_B2R2CK(val);
			Ddim_Print(("It set CLKSTOP_12B2R2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.B2R2CK	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.B2R2CK));
		}
		else if ( 0 == strcmp( argv[4], "B2R2AX" ) ) {
			Dd_Top_Set_CLKSTOP12_B2R2AX(val);
			Ddim_Print(("It set CLKSTOP_12B2R2AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.B2R2AX	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.B2R2AX));
		}
		else if ( 0 == strcmp( argv[4], "B2R2AH" ) ) {
			Dd_Top_Set_CLKSTOP12_B2R2AH(val);
			Ddim_Print(("It set CLKSTOP_12B2R2AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.B2R2AH	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.B2R2AH));
		}
		else if ( 0 == strcmp( argv[4], "B2R2AP" ) ) {
			Dd_Top_Set_CLKSTOP12_B2R2AP(val);
			Ddim_Print(("It set CLKSTOP_12B2R2AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP12.bit.B2R2AP	= %d\n", IO_CHIPTOP.CLKSTOP12.bit.B2R2AP));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop13(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "LTM2CK" ) ) {
			Dd_Top_Set_CLKSTOP13_LTM2CK(val);
			Ddim_Print(("It set CLKSTOP_13LTM2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.LTM2CK	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.LTM2CK));
		}
		else if ( 0 == strcmp( argv[4], "LTM2AX" ) ) {
			Dd_Top_Set_CLKSTOP13_LTM2AX(val);
			Ddim_Print(("It set CLKSTOP_13LTM2AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.LTM2AX	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.LTM2AX));
		}
		else if ( 0 == strcmp( argv[4], "LTM2AH" ) ) {
			Dd_Top_Set_CLKSTOP13_LTM2AH(val);
			Ddim_Print(("It set CLKSTOP_13LTM2AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.LTM2AH	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.LTM2AH));
		}
		else if ( 0 == strcmp( argv[4], "LTM2AP" ) ) {
			Dd_Top_Set_CLKSTOP13_LTM2AP(val);
			Ddim_Print(("It set CLKSTOP_13LTM2AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.LTM2AP	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.LTM2AP));
		}
		else if ( 0 == strcmp( argv[4], "R2Y2CK" ) ) {
			Dd_Top_Set_CLKSTOP13_R2Y2CK(val);
			Ddim_Print(("It set CLKSTOP_13R2Y2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.R2Y2CK	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.R2Y2CK));
		}
		else if ( 0 == strcmp( argv[4], "R2Y2AX" ) ) {
			Dd_Top_Set_CLKSTOP13_R2Y2AX(val);
			Ddim_Print(("It set CLKSTOP_13R2Y2AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.R2Y2AX	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.R2Y2AX));
		}
		else if ( 0 == strcmp( argv[4], "R2Y2AH" ) ) {
			Dd_Top_Set_CLKSTOP13_R2Y2AH(val);
			Ddim_Print(("It set CLKSTOP_13R2Y2AH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.R2Y2AH	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.R2Y2AH));
		}
		else if ( 0 == strcmp( argv[4], "R2Y2AP" ) ) {
			Dd_Top_Set_CLKSTOP13_R2Y2AP(val);
			Ddim_Print(("It set CLKSTOP_13R2Y2AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.R2Y2AP	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.R2Y2AP));
		}
		else if ( 0 == strcmp( argv[4], "CNR2CK" ) ) {
			Dd_Top_Set_CLKSTOP13_CNR2CK(val);
			Ddim_Print(("It set CLKSTOP_13CNR2CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.CNR2CK	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.CNR2CK));
		}
		else if ( 0 == strcmp( argv[4], "CNR2AX" ) ) {
			Dd_Top_Set_CLKSTOP13_CNR2AX(val);
			Ddim_Print(("It set CLKSTOP_13CNR2AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.CNR2AX	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.CNR2AX));
		}
		else if ( 0 == strcmp( argv[4], "CNR2AP" ) ) {
			Dd_Top_Set_CLKSTOP13_CNR2AP(val);
			Ddim_Print(("It set CLKSTOP_13CNR2AP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.CNR2AP	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.CNR2AP));
		}
		else if ( 0 == strcmp( argv[4], "UMCVDFMX4" ) ) {
			Dd_Top_Set_CLKSTOP13_UMCVDFMX4(val);
			Ddim_Print(("It set CLKSTOP_UMCVDFMX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX4	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX4));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "UMCPXFMX4" ) ) {
			Dd_Top_Set_CLKSTOP13_UMCPXFMX4(val);
			Ddim_Print(("It set CLKSTOP_UMCPXFMX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX4	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX4));
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "UMCHEVENCAX4" ) ) {
			Dd_Top_Set_CLKSTOP13_UMCHEVENCAX4(val);
			Ddim_Print(("It set CLKSTOP_UMCHEVENCAX4 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.UMCHEVENCAX4	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.UMCHEVENCAX4));
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[4], "UMCVDFMX2" ) ) {
			Dd_Top_Set_CLKSTOP13_UMCVDFMX2(val);
			Ddim_Print(("It set CLKSTOP_UMCVDFMX2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX2	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.UMCVDFMX2));
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "UMCPXFMX2" ) ) {
			Dd_Top_Set_CLKSTOP13_UMCPXFMX2(val);
			Ddim_Print(("It set CLKSTOP_UMCPXFMX2 Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX2	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.UMCPXFMX2));
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[4], "HEVENCAP" ) ) {
			Dd_Top_Set_CLKSTOP13_HEVENCAP(val);
			Ddim_Print(("It set CLKSTOP_HEVENCAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.HEVENCAP	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.HEVENCAP));
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[4], "BMH1CK" ) ) {
			Dd_Top_Set_CLKSTOP13_BMH1CK(val);
			Ddim_Print(("It set CLKSTOP_BMH1CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP13.bit.BMH1CK	= %d\n", IO_CHIPTOP.CLKSTOP13.bit.BMH1CK));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

static void ct_set_clock_clkstop14(int argc, char** argv)
{
	ULONG val;

	if (argc == 6) {
		val = atoi(argv[5]);
		if ( 0 == strcmp( argv[4], "STATAX" ) ) {
			Dd_Top_Set_CLKSTOP14_STATAX(val);
			Ddim_Print(("It set CLKSTOP_14STATAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.STATAX	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.STATAX));
		}
		else if ( 0 == strcmp( argv[4], "STATAH" ) ) {
			Dd_Top_Set_CLKSTOP14_STATAH(val);
			Ddim_Print(("It set CLKSTOP_14STATAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.STATAH	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.STATAH));
		}
		else if ( 0 == strcmp( argv[4], "STATAP" ) ) {
			Dd_Top_Set_CLKSTOP14_STATAP(val);
			Ddim_Print(("It set CLKSTOP_14STATAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.STATAP	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.STATAP));
		}
		else if ( 0 == strcmp( argv[4], "PERIAH" ) ) {
			Dd_Top_Set_CLKSTOP14_PERIAH(val);
			Ddim_Print(("It set CLKSTOP_14PERIAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.PERIAH	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.PERIAH));
		}
		else if ( 0 == strcmp( argv[4], "PERIAP" ) ) {
			Dd_Top_Set_CLKSTOP14_PERIAP(val);
			Ddim_Print(("It set CLKSTOP_14PERIAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.PERIAP	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.PERIAP));
		}
		else if ( 0 == strcmp( argv[4], "SENMSKCK" ) ) {
			Dd_Top_Set_CLKSTOP14_SENMSKCK(val);
			Ddim_Print(("It set CLKSTOP_14SENMSKCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.SENMSKCK	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.SENMSKCK));
		}
		else if ( 0 == strcmp( argv[4], "GYROCK" ) ) {
			Dd_Top_Set_CLKSTOP14_GYROCK(val);
			Ddim_Print(("It set CLKSTOP_14GYROCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.GYROCK	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.GYROCK));
		}
		else if ( 0 == strcmp( argv[4], "EXSAH" ) ) {
			Dd_Top_Set_CLKSTOP14_EXSAH(val);
			Ddim_Print(("It set CLKSTOP_EXSAH Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.EXSAH	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.EXSAH));
		}
		else if ( 0 == strcmp( argv[4], "EXSAP" ) ) {
			Dd_Top_Set_CLKSTOP14_EXSAP(val);
			Ddim_Print(("It set CLKSTOP_EXSAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.EXSAP	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.EXSAP));
		}
		else if ( 0 == strcmp( argv[4], "PASCK" ) ) {
			Dd_Top_Set_CLKSTOP14_PASCK(val);
			Ddim_Print(("It set CLKSTOP_PASCK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.PASCK	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.PASCK));
		}
		else if ( 0 == strcmp( argv[4], "BMH0CK" ) ) {
			Dd_Top_Set_CLKSTOP14_BMH0CK(val);
			Ddim_Print(("It set CLKSTOP_BMH0CK Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.BMH0CK	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.BMH0CK));
		}
		else if ( 0 == strcmp( argv[4], "BMH0AX" ) ) {
			Dd_Top_Set_CLKSTOP14_BMH0AX(val);
			Ddim_Print(("It set CLKSTOP_BMH0AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.BMH0AX	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.BMH0AX));
		}
		else if ( 0 == strcmp( argv[4], "RDMAAX" ) ) {
			Dd_Top_Set_CLKSTOP14_RDMAAX(val);
			Ddim_Print(("It set CLKSTOP_RDMAAX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.RDMAAX	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.RDMAAX));
		}
		else if ( 0 == strcmp( argv[4], "RDMAAP" ) ) {
			Dd_Top_Set_CLKSTOP14_RDMAAP(val);
			Ddim_Print(("It set CLKSTOP_RDMAAP Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.RDMAAP	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.RDMAAP));
		}
		else if ( 0 == strcmp( argv[4], "BMH1AX" ) ) {
			Dd_Top_Set_CLKSTOP14_BMH1AX(val);
			Ddim_Print(("It set CLKSTOP_BMH1AX Clock State.\n"));
			Ddim_Print(("IO_CHIPTOP.CLKSTOP14.bit.BMH1AX	= %d\n", IO_CHIPTOP.CLKSTOP14.bit.BMH1AX));
		}
		else {
			Ddim_Print(("please check 4th parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}


static void ct_get_clock_frequency(int argc, char** argv)
{
	ULONG val;
	int i;

	Dd_Top_Set_CLKSEL3_SENCLK(2);
	Dd_Top_Set_CLKSEL5_SRO1CLK(3);
	Dd_Top_Set_CLKSEL4_SRO1CLK_2(4);
	Dd_Top_Set_CLKSEL5_LTMRBK1CLK(3);

	Dd_Top_Set_CLKSEL7_SRO2CLK(3);
	Dd_Top_Set_CLKSEL6_SRO2CLK_2(4);
	Dd_Top_Set_CLKSEL7_LTMRBK2CLK(3);

	if (argc == 6 || argc == 5 || argc == 4) {
		if ( 0 == strcmp( argv[3], "EMMCCLK") ) {
			Ddim_Print(("0:187.5MHz \n"));
			Ddim_Print(("1:166.666MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL1_EMMCCLK(val);
				Ddim_Print(("%02lu: %02d: Dd_Top_Get_EMMCCLK()=%ld\n", val, Dd_Top_Get_CLKSEL1_EMMCCLK(), Dd_Top_Get_EMMCCLK()));
			}
			else {
				for (i = 0; i < 3; i ++) {
					Dd_Top_Set_CLKSEL1_EMMCCLK(i);
					Ddim_Print(("%02d: %02d: Dd_Top_Get_EMMCCLK()=%ld\n", i, Dd_Top_Get_CLKSEL1_EMMCCLK(), Dd_Top_Get_EMMCCLK()));
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "NFCLK") ) {
			Ddim_Print(("1:100MHz \n"));
			Ddim_Print(("2:80MHz \n"));
			Ddim_Print(("3:50MHz \n"));
			Ddim_Print(("4:20MHz \n"));
			Ddim_Print(("16:64MHz \n"));
			Ddim_Print(("24:32MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL1_NFCLK(val);
				if (Dd_Top_Get_NFCLK()) {
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_NFCLK()=%ld\n", val, Dd_Top_Get_CLKSEL1_NFCLK(), Dd_Top_Get_NFCLK()));
				}
			}
			else {
				for (i = 0; i < 9; i ++) {
					Dd_Top_Set_CLKSEL1_NFCLK(i);
					if (Dd_Top_Get_NFCLK()) {
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_NFCLK()=%ld\n", i, Dd_Top_Get_CLKSEL1_NFCLK(), Dd_Top_Get_NFCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "UHS2CLK") ) {
			Ddim_Print(("0:51.851MHz \n"));
			Ddim_Print(("1:46.666MHz \n"));
			Ddim_Print(("2:42.424MHz \n"));
			Ddim_Print(("3:38.888MHz \n"));
			Ddim_Print(("4:35.897MHz \n"));
			Ddim_Print(("5:33.333MHz \n"));
			Ddim_Print(("6:29.166MHz \n"));
			Ddim_Print(("7:25.925MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL1_UHS2CLK(val);
				if (Dd_Top_Get_UHS2CLK()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_UHS2CLK()=%ld\n", val, Dd_Top_Get_CLKSEL1_UHS2CLK(), Dd_Top_Get_UHS2CLK()));
				}
			}
			else {
				for (i = 0; i < 8; i ++) {
					Dd_Top_Set_CLKSEL1_UHS2CLK(i);
					if (Dd_Top_Get_UHS2CLK()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_UHS2CLK()=%ld\n", i, Dd_Top_Get_CLKSEL1_UHS2CLK(), Dd_Top_Get_UHS2CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "UHS1CLK2") ) {
			Ddim_Print(("0:800MHz \n"));
			Ddim_Print(("1:533.333MHz \n"));
			Ddim_Print(("2:400MHz \n"));
			Ddim_Print(("3:200MHz \n"));
			Ddim_Print(("4:100MHz \n"));
			Ddim_Print(("8:700MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL1_UHS1CLK2(val);
				if (Dd_Top_Get_UHS1CLK2()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_UHS1CLK2()=%ld\n", val, Dd_Top_Get_CLKSEL1_UHS1CLK2(), Dd_Top_Get_UHS1CLK2()));
				}
			}
			else {
				for (i = 0; i < 9; i ++) {
					Dd_Top_Set_CLKSEL1_UHS1CLK2(i);
					if (Dd_Top_Get_UHS1CLK2()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_UHS1CLK2()=%ld\n", i, Dd_Top_Get_CLKSEL1_UHS1CLK2(), Dd_Top_Get_UHS1CLK2()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "UHS1CLK1") ) {
			Ddim_Print(("0:800MHz \n"));
			Ddim_Print(("1:533.333MHz \n"));
			Ddim_Print(("2:400MHz \n"));
			Ddim_Print(("3:200MHz \n"));
			Ddim_Print(("4:100MHz \n"));
			Ddim_Print(("8:700MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL1_UHS1CLK1(val);
				if (Dd_Top_Get_UHS1CLK1()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_UHS1CLK1()=%ld\n", val, Dd_Top_Get_CLKSEL1_UHS1CLK1(), Dd_Top_Get_UHS1CLK1()));
				}
			}
			else {
				for (i = 0; i < 9; i ++) {
					Dd_Top_Set_CLKSEL1_UHS1CLK1(i);
					if (Dd_Top_Get_UHS1CLK1()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_UHS1CLK1()=%ld\n", i, Dd_Top_Get_CLKSEL1_UHS1CLK1(), Dd_Top_Get_UHS1CLK1()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "UHS1CLK0") ) {
			Ddim_Print(("0:800MHz \n"));
			Ddim_Print(("1:533.333MHz \n"));
			Ddim_Print(("2:400MHz \n"));
			Ddim_Print(("3:200MHz \n"));
			Ddim_Print(("4:100MHz \n"));
			Ddim_Print(("8:700MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL1_UHS1CLK0(val);
				if (Dd_Top_Get_UHS1CLK0()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_UHS1CLK0()=%ld\n", val, Dd_Top_Get_CLKSEL1_UHS1CLK0(), Dd_Top_Get_UHS1CLK0()));
				}
			}
			else {
				for (i = 0; i < 9; i ++) {
					Dd_Top_Set_CLKSEL1_UHS1CLK0(i);
					if (Dd_Top_Get_UHS1CLK0()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_UHS1CLK0()=%ld\n", i, Dd_Top_Get_CLKSEL1_UHS1CLK0(), Dd_Top_Get_UHS1CLK0()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "RCLK") ) {
			Ddim_Print(("0:54MHz \n"));
			Ddim_Print(("1:27MHz \n"));
			Ddim_Print(("2:18MHz \n"));
			Ddim_Print(("3:13.5MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL1_RCLK(val);
				if (Dd_Top_Get_RCLK()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_RCLK()=%ld\n", val, Dd_Top_Get_CLKSEL1_RCLK(), Dd_Top_Get_RCLK()));
				}
			}
			else {
				for (i = 0; i < 4; i ++) {
					Dd_Top_Set_CLKSEL1_RCLK(i);
					if (Dd_Top_Get_RCLK()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_RCLK()=%ld\n", i, Dd_Top_Get_CLKSEL1_RCLK(), Dd_Top_Get_RCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "RAWCLK") ) {
			Ddim_Print(("0:200MHz \n"));
			Ddim_Print(("1:160MHz \n"));
			Ddim_Print(("2:100MHz \n"));
			Ddim_Print(("3:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL2_RAWCLK(val);
				if (Dd_Top_Get_RAWCLK()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_RAWCLK()=%ld\n", val, Dd_Top_Get_CLKSEL2_RAWCLK(), Dd_Top_Get_RAWCLK()));
				}
			}
			else {
				for (i = 0; i < 4; i ++) {
					Dd_Top_Set_CLKSEL2_RAWCLK(i);
					if (Dd_Top_Get_RAWCLK()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_RAWCLK()=%ld\n", i, Dd_Top_Get_CLKSEL2_RAWCLK(), Dd_Top_Get_RAWCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "HIFCLK") ) {
			Ddim_Print(("0:594MHz \n"));
			Ddim_Print(("1:297MHz \n"));
			Ddim_Print(("2:148.5MHz \n"));
			Ddim_Print(("3:74.25MHz \n"));
			Ddim_Print(("4:27MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL2_HIFCLK(val);
				if (Dd_Top_Get_HIFCLK()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_HIFCLK()=%ld\n", val, Dd_Top_Get_CLKSEL2_HIFCLK(), Dd_Top_Get_HIFCLK()));
				}
			}
			else {
				for (i = 0; i < 5; i ++) {
					Dd_Top_Set_CLKSEL2_HIFCLK(i);
					if (Dd_Top_Get_HIFCLK()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_HIFCLK()=%ld\n", i, Dd_Top_Get_CLKSEL2_HIFCLK(), Dd_Top_Get_HIFCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "MIFCLK") ) {
			Ddim_Print(("0:169.714MHz \n"));
			Ddim_Print(("1:148.5MHz \n"));
			Ddim_Print(("2:108MHz \n"));
			Ddim_Print(("3:74.25MHz \n"));
			Ddim_Print(("4:66MHz \n"));
			Ddim_Print(("5:54MHz \n"));
			Ddim_Print(("6:49.5MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL2_MIFCLK(val);
				if (Dd_Top_Get_MIFCLK()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_MIFCLK()=%ld\n", val, Dd_Top_Get_CLKSEL2_MIFCLK(), Dd_Top_Get_MIFCLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL2_MIFCLK(i);
					if (Dd_Top_Get_MIFCLK()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_MIFCLK()=%ld\n", i, Dd_Top_Get_CLKSEL2_MIFCLK(), Dd_Top_Get_MIFCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "IPUCLK") ) {
			Ddim_Print(("0:400MHz \n"));
			Ddim_Print(("1:50MHz \n"));
			Ddim_Print(("2:100MHz \n"));
			Ddim_Print(("3:150MHz \n"));
			Ddim_Print(("4:200MHz \n"));
			Ddim_Print(("5:250MHz \n"));
			Ddim_Print(("6:300MHz \n"));
			Ddim_Print(("7:350MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL2_IPUCLK(val);
				if (Dd_Top_Get_CLKSEL2_IPUCLK()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_IPUCLK()=%ld\n", val, Dd_Top_Get_CLKSEL2_IPUCLK(), Dd_Top_Get_IPUCLK()));
				}
			}
			else {
				for (i = 0; i < 8; i ++) {
					Dd_Top_Set_CLKSEL2_IPUCLK(i);
					if (Dd_Top_Get_CLKSEL2_IPUCLK()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_IPUCLK()=%ld\n", i, Dd_Top_Get_CLKSEL2_IPUCLK(), Dd_Top_Get_IPUCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "IPUTMECLK") ) {
			Ddim_Print(("0:600MHz \n"));
			Ddim_Print(("1:75MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:225MHz \n"));
			Ddim_Print(("4:300MHz \n"));
			Ddim_Print(("5:375MHz \n"));
			Ddim_Print(("6:450MHz \n"));
			Ddim_Print(("7:525MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL2_IPUTMECLK(val);
				if (Dd_Top_Get_IPUTMECLK()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_IPUTMECLK()=%ld\n", val, Dd_Top_Get_CLKSEL2_IPUTMECLK(), Dd_Top_Get_IPUTMECLK()));
				}
			}
			else {
				for (i = 0; i < 8; i ++) {
					Dd_Top_Set_CLKSEL2_IPUTMECLK(i);
					if (Dd_Top_Get_IPUTMECLK()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_IPUTMECLK()=%ld\n", i, Dd_Top_Get_CLKSEL2_IPUTMECLK(), Dd_Top_Get_IPUTMECLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "GPUCLK") ) {
			Ddim_Print(("0:466.666MHz \n"));
			Ddim_Print(("1:58.333MHz \n"));
			Ddim_Print(("2:116.666MHz \n"));
			Ddim_Print(("3:175MHz \n"));
			Ddim_Print(("4:233.333MHz \n"));
			Ddim_Print(("5:291.666MHz \n"));
			Ddim_Print(("6:350MHz \n"));
			Ddim_Print(("7:408.333MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL2_GPUCLK(val);
				if (Dd_Top_Get_GPUCLK()){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_GPUCLK()=%ld\n", val, Dd_Top_Get_CLKSEL2_GPUCLK(), Dd_Top_Get_GPUCLK()));
				}
			}
			else {
				for (i = 0; i < 8; i ++) {
					Dd_Top_Set_CLKSEL2_GPUCLK(i);
					if (Dd_Top_Get_GPUCLK()){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_GPUCLK()=%ld\n", i, Dd_Top_Get_CLKSEL2_GPUCLK(), Dd_Top_Get_GPUCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "JPGCLK") ) {
			Ddim_Print(("0:200MHz \n"));
			Ddim_Print(("1:25MHz \n"));
			Ddim_Print(("2:50MHz \n"));
			Ddim_Print(("3:75MHz \n"));
			Ddim_Print(("4:100MHz \n"));
			Ddim_Print(("5:125MHz \n"));
			Ddim_Print(("6:150MHz \n"));
			Ddim_Print(("7:175MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL2_JPGCLK(val);

				ULONG clock = Dd_Top_Get_JPEGCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_JPEGCLK=%ld\n", val, Dd_Top_Get_CLKSEL2_JPGCLK(), Dd_Top_Get_JPEGCLK()));
				}
			}
			else {
				for (i = 0; i < 8; i ++) {
					Dd_Top_Set_CLKSEL2_JPGCLK(i);

					ULONG clock = Dd_Top_Get_JPEGCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_JPEGCLK=%ld\n", i, Dd_Top_Get_CLKSEL2_JPGCLK(), Dd_Top_Get_JPEGCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "ELACLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL2_ELACLK(val);

				ULONG clock = Dd_Top_Get_ELACLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_ELACLK=%ld\n", val, Dd_Top_Get_CLKSEL2_ELACLK(), Dd_Top_Get_ELACLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL2_ELACLK(i);

					ULONG clock = Dd_Top_Get_ELACLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_ELACLK=%ld\n", i, Dd_Top_Get_CLKSEL2_ELACLK(), Dd_Top_Get_ELACLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "PIPESEL") ) {
			Ddim_Print(("2:Use PLL02 \n"));
			Ddim_Print(("3:Use PLL01 \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_PIPESEL(val);

				ULONG clock = Dd_Top_Get_CLKSEL3_PIPESEL();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_CLKSEL3_PIPESEL=%d\n", val, Dd_Top_Get_CLKSEL3_PIPESEL(), Dd_Top_Get_CLKSEL3_PIPESEL()));
				}
			}
			else {
				for (i = 0; i < 2; i ++) {
					Dd_Top_Set_CLKSEL3_PIPESEL(i);

					ULONG clock = Dd_Top_Get_CLKSEL3_PIPESEL();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_CLKSEL3_PIPESEL=%d\n", i, Dd_Top_Get_CLKSEL3_PIPESEL(), Dd_Top_Get_CLKSEL3_PIPESEL()));
					}
				}
			}
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[3], "HEVENCSEL") ) {
			Ddim_Print(("0:500MHz \n"));
			Ddim_Print(("1:HEVENCCLK \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_HEVENCSEL(val);

				ULONG clock = Dd_Top_Get_CLKSEL3_HEVENCSEL();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_CLKSEL3_HEVENCSEL=%d\n", val, Dd_Top_Get_CLKSEL3_HEVENCSEL(), Dd_Top_Get_CLKSEL3_HEVENCSEL()));
				}
			}
			else {
				for (i = 0; i < 2; i ++) {
					Dd_Top_Set_CLKSEL3_HEVENCSEL(i);

//					ULONG clock = Dd_Top_Get_CLKSEL3_HEVENCSEL();
//					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_CLKSEL3_HEVENCSEL=%d\n", i, Dd_Top_Get_CLKSEL3_HEVENCSEL(), Dd_Top_Get_CLKSEL3_HEVENCSEL()));
//					}
				}
			}
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[3], "SENCLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_SENCLK(val);

				ULONG clock = Dd_Top_Get_SENCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_SENCLK=%ld\n", val, Dd_Top_Get_CLKSEL3_SENCLK(), Dd_Top_Get_SENCLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL3_SENCLK(i);

					ULONG clock = Dd_Top_Get_SENCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SENCLK=%ld\n", i, Dd_Top_Get_CLKSEL3_SENCLK(), Dd_Top_Get_SENCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "SENMSKCLK") ) {
			Ddim_Print(("0:100MHz \n"));
			Ddim_Print(("1:80MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_SENMSKCLK(val);

				ULONG clock = Dd_Top_Get_SENMSKCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_SENMSKCLK=%ld\n", val, Dd_Top_Get_CLKSEL3_SENMSKCLK(), Dd_Top_Get_SENMSKCLK()));
				}
			}
			else {
				for (i = 0; i < 2; i ++) {
					Dd_Top_Set_CLKSEL3_SENMSKCLK(i);

					ULONG clock = Dd_Top_Get_SENMSKCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SENMSKCLK=%ld\n", i, Dd_Top_Get_CLKSEL3_SENMSKCLK(), Dd_Top_Get_SENMSKCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "IIPCLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_IIPCLK(val);

				ULONG clock = Dd_Top_Get_IIPCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_IIPCLK=%ld\n", val, Dd_Top_Get_CLKSEL3_IIPCLK(), Dd_Top_Get_IIPCLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL3_IIPCLK(i);

					ULONG clock = Dd_Top_Get_IIPCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_IIPCLK=%ld\n", i, Dd_Top_Get_CLKSEL3_IIPCLK(), Dd_Top_Get_IIPCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "PASCLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_PASCLK(val);

				ULONG clock = Dd_Top_Get_PASCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_PASCLK=%ld\n", val, Dd_Top_Get_CLKSEL3_PASCLK(), Dd_Top_Get_PASCLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL3_PASCLK(i);

					ULONG clock = Dd_Top_Get_PASCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_PASCLK=%ld\n", i, Dd_Top_Get_CLKSEL3_PASCLK(), Dd_Top_Get_PASCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "IPPCLK") ) {
			Ddim_Print(("0:800MHz \n"));
			Ddim_Print(("1:533.333MHz \n"));
			Ddim_Print(("2:400MHz \n"));
			Ddim_Print(("3:200MHz \n"));
			Ddim_Print(("4:700MHz \n"));
			Ddim_Print(("8:600MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_IPPCLK(val);

				ULONG clock = Dd_Top_Get_IPPCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_IPPCLK=%ld\n", val, Dd_Top_Get_CLKSEL3_IPPCLK(), Dd_Top_Get_IPPCLK()));
				}
			}
			else {
				for (i = 0; i < 9; i ++) {
					Dd_Top_Set_CLKSEL3_IPPCLK(i);

					ULONG clock = Dd_Top_Get_IPPCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_IPPCLK=%ld\n", i, Dd_Top_Get_CLKSEL3_IPPCLK(), Dd_Top_Get_IPPCLK()));
					}
				}
			}
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[3], "PXFCLK") ) {
			Ddim_Print(("0:700MHz \n"));
			Ddim_Print(("1:280MHz \n"));
			Ddim_Print(("2:155.555MHz \n"));
			Ddim_Print(("4:533.333MHz \n"));
			Ddim_Print(("5:400MHz \n"));
			Ddim_Print(("6:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_PXFCLK(val);

				ULONG clock = Dd_Top_Get_PXFCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_PXFCLK=%ld\n", val, Dd_Top_Get_CLKSEL3_PXFCLK(), Dd_Top_Get_PXFCLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL3_PXFCLK(i);

					ULONG clock = Dd_Top_Get_PXFCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_PXFCLK=%ld\n", i, Dd_Top_Get_CLKSEL3_PXFCLK(), Dd_Top_Get_PXFCLK()));
					}
				}
			}
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[3], "HEVENCCLK") ) {
			Ddim_Print(("0:466.666MHz \n"));
			Ddim_Print(("1:350MHz \n"));
			Ddim_Print(("2:280MHz \n"));
			Ddim_Print(("4:200MHz \n"));
			Ddim_Print(("5:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_HEVENCCLK(val);

				ULONG clock = Dd_Top_Get_HEVENCCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_HEVENCCLK=%ld\n", val, Dd_Top_Get_CLKSEL3_HEVENCCLK(), Dd_Top_Get_HEVENCCLK()));
				}
			}
			else {
				for (i = 0; i < 6; i ++) {
					Dd_Top_Set_CLKSEL3_HEVENCCLK(i);

					ULONG clock = Dd_Top_Get_HEVENCCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_HEVENCCLK=%ld\n", i, Dd_Top_Get_CLKSEL3_HEVENCCLK(), Dd_Top_Get_HEVENCCLK()));
					}
				}
			}
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[3], "VDFCLK") ) {
			Ddim_Print(("0:700MHz \n"));
			Ddim_Print(("1:280MHz \n"));
			Ddim_Print(("2:155.555MHz \n"));
			Ddim_Print(("4:533.333MHz \n"));
			Ddim_Print(("5:400MHz \n"));
			Ddim_Print(("6:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL3_VDFCLK(val);

				ULONG clock = Dd_Top_Get_VDFCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_VDFCLK=%ld\n", val, Dd_Top_Get_CLKSEL3_VDFCLK(), Dd_Top_Get_VDFCLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL3_VDFCLK(i);

					ULONG clock = Dd_Top_Get_VDFCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_VDFCLK=%ld\n", i, Dd_Top_Get_CLKSEL3_VDFCLK(), Dd_Top_Get_VDFCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "SRO1CLK_2") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL4_SRO1CLK_2(val);

				ULONG clock = Dd_Top_Get_SRO1CLK_2();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_SRO1CLK_2=%ld\n", val, Dd_Top_Get_CLKSEL4_SRO1CLK_2(), Dd_Top_Get_SRO1CLK_2()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL4_SRO1CLK_2(i);

					ULONG clock = Dd_Top_Get_SRO1CLK_2();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SRO1CLK_2=%ld\n", i, Dd_Top_Get_CLKSEL4_SRO1CLK_2(), Dd_Top_Get_SRO1CLK_2()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "R2Y1CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL4_R2Y1CLK(val);

				ULONG clock = Dd_Top_Get_R2Y1CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_R2Y1CLK=%ld\n", val, Dd_Top_Get_CLKSEL4_R2Y1CLK(), Dd_Top_Get_R2Y1CLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL4_R2Y1CLK(i);

					ULONG clock = Dd_Top_Get_R2Y1CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_R2Y1CLK=%ld\n", i, Dd_Top_Get_CLKSEL4_R2Y1CLK(), Dd_Top_Get_R2Y1CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "LTM1CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:SRO1CLK\n"));
			Ddim_Print(("16:SRO1CLK_2\n"));
			Ddim_Print(("24:B2B1CLK\n"));
			Ddim_Print(("32:B2R1CLK\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL4_LTM1CLK(val);

				ULONG clock = Dd_Top_Get_LTM1CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_LTM1CLK=%ld\n", val, Dd_Top_Get_CLKSEL4_LTM1CLK(), Dd_Top_Get_LTM1CLK()));
				}
			}
			else {
				for (i = 0; i < 33; i ++) {
					Dd_Top_Set_CLKSEL4_LTM1CLK(i);

					ULONG clock = Dd_Top_Get_LTM1CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_LTM1CLK=%ld\n", i, Dd_Top_Get_CLKSEL4_LTM1CLK(), Dd_Top_Get_LTM1CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "B2R1CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:SRO1CLK\n"));
			Ddim_Print(("16:SRO1CLK_2\n"));
			Ddim_Print(("24:B2B1CLK\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL4_B2R1CLK(val);

				ULONG clock = Dd_Top_Get_B2R1CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_B2R1CLK=%ld\n", val, Dd_Top_Get_CLKSEL4_B2R1CLK(), Dd_Top_Get_B2R1CLK()));
				}
			}
			else {
				for (i = 0; i < 25; i ++) {
					Dd_Top_Set_CLKSEL4_B2R1CLK(i);

					ULONG clock = Dd_Top_Get_B2R1CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_B2R1CLK=%ld\n", i, Dd_Top_Get_CLKSEL4_B2R1CLK(), Dd_Top_Get_B2R1CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "CNR1CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:R2Y1CLK\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL4_CNR1CLK(val);

				ULONG clock = Dd_Top_Get_CNR1CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_CNR1CLK=%ld\n", val, Dd_Top_Get_CLKSEL4_CNR1CLK(), Dd_Top_Get_CNR1CLK()));
				}
			}
			else {
				for (i = 0; i < 9; i ++) {
					Dd_Top_Set_CLKSEL4_CNR1CLK(i);

					ULONG clock = Dd_Top_Get_CNR1CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_CNR1CLK=%ld\n", i, Dd_Top_Get_CLKSEL4_CNR1CLK(), Dd_Top_Get_CNR1CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "SRO1CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL5_SRO1CLK(val);

				ULONG clock = Dd_Top_Get_SRO1CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_SRO1CLK=%ld\n", val, Dd_Top_Get_CLKSEL5_SRO1CLK(), Dd_Top_Get_SRO1CLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL5_SRO1CLK(i);

					ULONG clock = Dd_Top_Get_SRO1CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SRO1CLK=%ld\n", i, Dd_Top_Get_CLKSEL5_SRO1CLK(), Dd_Top_Get_SRO1CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "B2B1CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:SRO1CLK\n"));
			Ddim_Print(("16:SRO1CLK_2\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL5_B2B1CLK(val);

				ULONG clock = Dd_Top_Get_B2B1CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_B2B1CLK=%ld\n", val, Dd_Top_Get_CLKSEL5_B2B1CLK(), Dd_Top_Get_B2B1CLK()));
				}
			}
			else {
				for (i = 0; i < 17; i ++) {
					Dd_Top_Set_CLKSEL5_B2B1CLK(i);

					ULONG clock = Dd_Top_Get_B2B1CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_B2B1CLK=%ld\n", i, Dd_Top_Get_CLKSEL5_B2B1CLK(), Dd_Top_Get_B2B1CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "LTMRBK1CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:SRO1CLK\n"));
			Ddim_Print(("16:SRO1CLK_2\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL5_LTMRBK1CLK(val);

				ULONG clock = Dd_Top_Get_LTMRBK1CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_LTMRBK1CLK=%ld\n", val, Dd_Top_Get_CLKSEL5_LTMRBK1CLK(), Dd_Top_Get_LTMRBK1CLK()));
				}
			}
			else {
				for (i = 0; i < 17; i ++) {
					Dd_Top_Set_CLKSEL5_LTMRBK1CLK(i);

					ULONG clock = Dd_Top_Get_LTMRBK1CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_LTMRBK1CLK=%ld\n", i, Dd_Top_Get_CLKSEL5_LTMRBK1CLK(), Dd_Top_Get_LTMRBK1CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "SRO2CLK_2") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL6_SRO2CLK_2(val);

				ULONG clock = Dd_Top_Get_SRO2CLK_2();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_SRO2CLK_2=%ld\n", val, Dd_Top_Get_CLKSEL6_SRO2CLK_2(), Dd_Top_Get_SRO2CLK_2()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL6_SRO2CLK_2(i);

					ULONG clock = Dd_Top_Get_SRO2CLK_2();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SRO2CLK_2=%ld\n", i, Dd_Top_Get_CLKSEL6_SRO2CLK_2(), Dd_Top_Get_SRO2CLK_2()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "R2Y2CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL6_R2Y2CLK(val);

				ULONG clock = Dd_Top_Get_R2Y2CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_R2Y2CLK=%ld\n", val, Dd_Top_Get_CLKSEL6_R2Y2CLK(), Dd_Top_Get_R2Y2CLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL6_R2Y2CLK(i);

					ULONG clock = Dd_Top_Get_R2Y2CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_R2Y2CLK=%ld\n", i, Dd_Top_Get_CLKSEL6_R2Y2CLK(), Dd_Top_Get_R2Y2CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "LTM2CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:SRO2CLK\n"));
			Ddim_Print(("16:SRO2CLK_2\n"));
			Ddim_Print(("24:B2B2CLK\n"));
			Ddim_Print(("32:B2R2CLK\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL6_LTM2CLK(val);

				ULONG clock = Dd_Top_Get_LTM2CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_LTM2CLK=%ld\n", val, Dd_Top_Get_CLKSEL6_LTM2CLK(), Dd_Top_Get_LTM2CLK()));
				}
			}
			else {
				for (i = 0; i < 33; i ++) {
					Dd_Top_Set_CLKSEL6_LTM2CLK(i);

					ULONG clock = Dd_Top_Get_LTM2CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_LTM2CLK=%ld\n", i, Dd_Top_Get_CLKSEL6_LTM2CLK(), Dd_Top_Get_LTM2CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "B2R2CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:SRO2CLK\n"));
			Ddim_Print(("16:SRO2CLK_2\n"));
			Ddim_Print(("24:B2B2CLK\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL6_B2R2CLK(val);

				ULONG clock = Dd_Top_Get_B2R2CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_B2R2CLK=%ld\n", val, Dd_Top_Get_CLKSEL6_B2R2CLK(), Dd_Top_Get_B2R2CLK()));
				}
			}
			else {
				for (i = 0; i < 25; i ++) {
					Dd_Top_Set_CLKSEL6_B2R2CLK(i);

					ULONG clock = Dd_Top_Get_B2R2CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_B2R2CLK=%ld\n", i, Dd_Top_Get_CLKSEL6_B2R2CLK(), Dd_Top_Get_B2R2CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "CNR2CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:R2Y2CLK\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL6_CNR2CLK(val);

				ULONG clock = Dd_Top_Get_CNR2CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_CNR2CLK=%ld\n", val, Dd_Top_Get_CLKSEL6_CNR2CLK(), Dd_Top_Get_CNR2CLK()));
				}
			}
			else {
				for (i = 0; i < 9; i ++) {
					Dd_Top_Set_CLKSEL6_CNR2CLK(i);

					ULONG clock = Dd_Top_Get_CNR2CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_CNR2CLK=%ld\n", i, Dd_Top_Get_CLKSEL6_CNR2CLK(), Dd_Top_Get_CNR2CLK()));
					}
				}
			}
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		else if ( 0 == strcmp( argv[3], "MIPISEL") ) {
			Ddim_Print(("0:PLL08 \n"));
			Ddim_Print(("1:PLL08/4 \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL7_MIPISEL(val);

				Ddim_Print(("%02lu:\t Dd_Top_Get_CLKSEL7_MIPISEL=%d\n", val, Dd_Top_Get_CLKSEL7_MIPISEL()));
			}
			else {
				for (i = 0; i < 2; i ++) {
					Dd_Top_Set_CLKSEL7_MIPISEL(i);

					Ddim_Print(("%02d:\t Dd_Top_Get_CLKSEL7_MIPISEL=%d\n", i, Dd_Top_Get_CLKSEL7_MIPISEL()));
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "PDM0SEL") ) {
			Ddim_Print(("0:PDMCLK \n"));
			Ddim_Print(("1:AUCLK \n"));
			Ddim_Print(("2:AUCLK/2 \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL7_PDM0SEL(val);

				Ddim_Print(("%02lu:\t Dd_Top_Get_CLKSEL7_PDM0SEL=%d\n", val, Dd_Top_Get_CLKSEL7_PDM0SEL()));
			}
			else {
				for (i = 0; i < 3; i ++) {
					Dd_Top_Set_CLKSEL7_PDM0SEL(i);

					Ddim_Print(("%02d:\t Dd_Top_Get_CLKSEL7_PDM0SEL=%d\n", i, Dd_Top_Get_CLKSEL7_PDM0SEL()));
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "PDM1SEL") ) {
			Ddim_Print(("0:PDMCLK \n"));
			Ddim_Print(("1:AUCLK \n"));
			Ddim_Print(("2:AUCLK/2 \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL7_PDM1SEL(val);

				Ddim_Print(("%02lu:\t Dd_Top_Get_CLKSEL7_PDM1SEL=%d\n", val, Dd_Top_Get_CLKSEL7_PDM1SEL()));
			}
			else {
				for (i = 0; i < 3; i ++) {
					Dd_Top_Set_CLKSEL7_PDM1SEL(i);

					Ddim_Print(("%02d:\t Dd_Top_Get_CLKSEL7_PDM1SEL=%d\n", i, Dd_Top_Get_CLKSEL7_PDM1SEL()));
				}
			}
		}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if ( 0 == strcmp( argv[3], "SRO2CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL7_SRO2CLK(val);

				ULONG clock = Dd_Top_Get_SRO2CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_SRO2CLK=%ld\n", val, Dd_Top_Get_CLKSEL7_SRO2CLK(), Dd_Top_Get_SRO2CLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL7_SRO2CLK(i);

					ULONG clock = Dd_Top_Get_SRO2CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SRO2CLK=%ld\n", i, Dd_Top_Get_CLKSEL7_SRO2CLK(), Dd_Top_Get_SRO2CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "B2B2CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:SRO2CLK\n"));
			Ddim_Print(("16:SRO2CLK_2\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL7_B2B2CLK(val);

				ULONG clock = Dd_Top_Get_B2B2CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_B2B2CLK=%ld\n", val, Dd_Top_Get_CLKSEL7_B2B2CLK(), Dd_Top_Get_B2B2CLK()));
				}
			}
			else {
				for (i = 0; i < 17; i ++) {
					Dd_Top_Set_CLKSEL7_B2B2CLK(i);

					ULONG clock = Dd_Top_Get_B2B2CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_B2B2CLK=%ld\n", i, Dd_Top_Get_CLKSEL7_B2B2CLK(), Dd_Top_Get_B2B2CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "LTMRBK2CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			Ddim_Print(("8:SRO2CLK\n"));
			Ddim_Print(("16:SRO2CLK_2\n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL7_LTMRBK2CLK(val);

				ULONG clock = Dd_Top_Get_LTMRBK2CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_LTMRBK2CLK=%ld\n", val, Dd_Top_Get_CLKSEL7_LTMRBK2CLK(), Dd_Top_Get_LTMRBK2CLK()));
				}
			}
			else {
				for (i = 0; i < 17; i ++) {
					Dd_Top_Set_CLKSEL7_LTMRBK2CLK(i);

					ULONG clock = Dd_Top_Get_LTMRBK2CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_LTMRBK2CLK=%ld\n", i, Dd_Top_Get_CLKSEL7_LTMRBK2CLK(), Dd_Top_Get_LTMRBK2CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "APCLK") ) {
			Ddim_Print(("0:74.25MHz \n"));
			Ddim_Print(("1:54MHz \n"));
			Ddim_Print(("2:37.125MHz \n"));
			Ddim_Print(("3:27MHz \n"));
			Ddim_Print(("4:72MHz \n"));
			Ddim_Print(("5:48MHz \n"));
			Ddim_Print(("8:64MHz \n"));
			Ddim_Print(("9:45.714MHz \n"));
			Ddim_Print(("12:60MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL8_APCLK(val);

				ULONG clock = Dd_Top_Get_APCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_APCLK=%ld\n", val, Dd_Top_Get_CLKSEL8_APCLK(), Dd_Top_Get_APCLK()));
				}
			}
			else {
				for (i = 0; i < 13; i ++) {
					Dd_Top_Set_CLKSEL8_APCLK(i);

					ULONG clock = Dd_Top_Get_APCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_APCLK=%ld\n", i, Dd_Top_Get_CLKSEL8_APCLK(), Dd_Top_Get_APCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "AUCLK") ) {
			Ddim_Print(("0:36.864MHz \n"));
			Ddim_Print(("1:22.5792MHz \n"));
			Ddim_Print(("2:24.576MHz \n"));
			Ddim_Print(("3:8.192MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL8_AUCLK(val);

				ULONG clock = Dd_Top_Get_AUCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_AUCLK=%ld\n", val, Dd_Top_Get_CLKSEL8_AUCLK(), Dd_Top_Get_AUCLK()));
				}
			}
			else {
				for (i = 0; i < 4; i ++) {
					Dd_Top_Set_CLKSEL8_AUCLK(i);

					ULONG clock = Dd_Top_Get_AUCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_AUCLK=%ld\n", i, Dd_Top_Get_CLKSEL8_AUCLK(), Dd_Top_Get_AUCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "SPICLK") ) {
			Ddim_Print(("0:216MHz \n"));
			Ddim_Print(("1:108MHz \n"));
			Ddim_Print(("2:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL8_SPICLK(val);

				ULONG clock = Dd_Top_Get_SPICLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_SPICLK=%ld\n", val, Dd_Top_Get_CLKSEL8_SPICLK(), Dd_Top_Get_SPICLK()));
				}
			}
			else {
				for (i = 0; i < 3; i ++) {
					Dd_Top_Set_CLKSEL8_SPICLK(i);

					ULONG clock = Dd_Top_Get_SPICLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SPICLK=%ld\n", i, Dd_Top_Get_CLKSEL8_SPICLK(), Dd_Top_Get_SPICLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "DSPCLK") ) {
			Ddim_Print(("0:400MHz \n"));
			Ddim_Print(("1:266.666MHz \n"));
			Ddim_Print(("2:200MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL8_DSPCLK(val);

				ULONG clock = Dd_Top_Get_DSPCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_DSPCLK=%ld\n", val, Dd_Top_Get_CLKSEL8_DSPCLK(), Dd_Top_Get_DSPCLK()));
				}
			}
			else {
				for (i = 0; i < 4; i ++) {
					Dd_Top_Set_CLKSEL8_DSPCLK(i);

					ULONG clock = Dd_Top_Get_DSPCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_DSPCLK=%ld\n", i, Dd_Top_Get_CLKSEL8_DSPCLK(), Dd_Top_Get_DSPCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "ACLK") ) {
			Ddim_Print(("0:200MHz \n"));
			Ddim_Print(("1:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL9_ACLK(val);

				ULONG clock = Dd_Top_Get_ACLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_ACLK=%ld\n", val, Dd_Top_Get_CLKSEL9_ACLK(), Dd_Top_Get_ACLK()));
				}
			}
			else {
				for (i = 0; i < 8; i ++) {
					Dd_Top_Set_CLKSEL9_ACLK(i);

					ULONG clock = Dd_Top_Get_ACLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_ACLK=%ld\n", i, Dd_Top_Get_CLKSEL9_ACLK(), Dd_Top_Get_ACLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "ACLKEXS") ) {
			Ddim_Print(("3:200MHz \n"));
			Ddim_Print(("4:160MHz \n"));
			Ddim_Print(("5:133.333MHz \n"));
			Ddim_Print(("7:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL9_ACLKEXS(val);

				ULONG clock = Dd_Top_Get_ACLKEXS();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_ACLKEXS=%ld\n", val, Dd_Top_Get_CLKSEL9_ACLKEXS(), Dd_Top_Get_ACLKEXS()));
				}
			}
			else {
				for (i = 3; i < 8; i ++) {
					Dd_Top_Set_CLKSEL9_ACLKEXS(i);

					ULONG clock = Dd_Top_Get_ACLKEXS();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_ACLKEXS=%ld\n", i, Dd_Top_Get_CLKSEL9_ACLKEXS(), Dd_Top_Get_ACLKEXS()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "HCLKBMH") ) {
			Ddim_Print(("3:200MHz \n"));
			Ddim_Print(("7:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL9_HCLKBMH(val);

				ULONG clock = Dd_Top_Get_HCLKBMH();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_HCLKBMH=%ld\n", val, Dd_Top_Get_CLKSEL9_HCLKBMH(), Dd_Top_Get_HCLKBMH()));
				}
			}
			else {
				for (i = 3; i < 8; i ++) {
					Dd_Top_Set_CLKSEL9_HCLKBMH(i);

					ULONG clock = Dd_Top_Get_HCLKBMH();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_HCLKBMH=%ld\n", i, Dd_Top_Get_CLKSEL9_HCLKBMH(), Dd_Top_Get_HCLKBMH()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "HCLK") ) {
			Ddim_Print(("7:100MHz \n"));
			Ddim_Print(("15:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL9_HCLK(val);

				ULONG clock = Dd_Top_Get_HCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_HCLK=%ld\n", val, Dd_Top_Get_CLKSEL9_HCLK(), Dd_Top_Get_HCLK()));
				}
			}
			else {
				for (i = 7; i < 16; i ++) {
					Dd_Top_Set_CLKSEL9_HCLK(i);

					ULONG clock = Dd_Top_Get_HCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_HCLK=%ld\n", i, Dd_Top_Get_CLKSEL9_HCLK(), Dd_Top_Get_HCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "PCLK") ) {
			Ddim_Print(("15:50MHz \n"));
			Ddim_Print(("31:25MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL9_PCLK(val);

				ULONG clock = Dd_Top_Get_PCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_PCLK=%ld\n", val, Dd_Top_Get_CLKSEL9_PCLK(), Dd_Top_Get_PCLK()));
				}
			}
			else {
				for (i = 15; i < 32; i ++) {
					Dd_Top_Set_CLKSEL9_PCLK(i);

					ULONG clock = Dd_Top_Get_PCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_PCLK=%ld\n", i, Dd_Top_Get_CLKSEL9_PCLK(), Dd_Top_Get_PCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "MCLK400") ) {
			Ddim_Print(("1:400MHz \n"));
			Ddim_Print(("3:200MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL10_MCLK400(val);

				ULONG clock = Dd_Top_Get_MCLK400();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_MCLK400=%ld\n", val, Dd_Top_Get_CLKSEL10_MCLK400(), Dd_Top_Get_MCLK400()));
				}
			}
			else {
				for (i = 1; i < 4; i ++) {
					Dd_Top_Set_CLKSEL10_MCLK400(i);

					ULONG clock = Dd_Top_Get_MCLK400();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_MCLK400=%ld\n", i, Dd_Top_Get_CLKSEL10_MCLK400(), Dd_Top_Get_MCLK400()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "MCLK200") ) {
			Ddim_Print(("3:200MHz \n"));
			Ddim_Print(("7:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL10_MCLK200(val);

				ULONG clock = Dd_Top_Get_MCLK200();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_MCLK200=%ld\n", val, Dd_Top_Get_CLKSEL10_MCLK200(), Dd_Top_Get_MCLK200()));
				}
			}
			else {
				for (i = 3; i < 8; i ++) {
					Dd_Top_Set_CLKSEL10_MCLK200(i);

					ULONG clock = Dd_Top_Get_MCLK200();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_MCLK200=%ld\n", i, Dd_Top_Get_CLKSEL10_MCLK200(), Dd_Top_Get_MCLK200()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "ACLK400") ) {
			Ddim_Print(("1:400MHz \n"));
			Ddim_Print(("3:200MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL10_ACLK400(val);

				ULONG clock = Dd_Top_Get_ACLK400();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_ACLK400=%ld\n", val, Dd_Top_Get_CLKSEL10_ACLK400(), Dd_Top_Get_ACLK400()));
				}
			}
			else {
				for (i = 1; i < 4; i ++) {
					Dd_Top_Set_CLKSEL10_ACLK400(i);

					ULONG clock = Dd_Top_Get_ACLK400();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_ACLK400=%ld\n", i, Dd_Top_Get_CLKSEL10_ACLK400(), Dd_Top_Get_ACLK400()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "SHDRCLK") ) {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			Ddim_Print(("0:400MHz \n"));
			Ddim_Print(("1:266.666MHz \n"));
			Ddim_Print(("2:200MHz \n"));
			Ddim_Print(("3:160MHz \n"));
			Ddim_Print(("4:133.333MHz \n"));
			Ddim_Print(("5:100MHz \n"));
			Ddim_Print(("6:80MHz \n"));
			Ddim_Print(("7:66.666MHz \n"));
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			Ddim_Print(("0:432MHz \n"));
			Ddim_Print(("8:400MHz \n"));
			Ddim_Print(("9:266.666MHz \n"));
			Ddim_Print(("10:200MHz \n"));
			Ddim_Print(("11:160MHz \n"));
			Ddim_Print(("12:133.333MHz \n"));
			Ddim_Print(("13:100MHz \n"));
			Ddim_Print(("14:80MHz \n"));
			Ddim_Print(("15:66.666MHz \n"));
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL12_SHDRCLK(val);

				ULONG clock = Dd_Top_Get_SHDRCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_SHDRCLK=%ld\n", val, Dd_Top_Get_CLKSEL12_SHDRCLK(), Dd_Top_Get_SHDRCLK()));
				}
			}
			else {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
				for (i = 0; i < 8; i ++) {
					Dd_Top_Set_CLKSEL12_SHDRCLK(i);

					ULONG clock = Dd_Top_Get_SHDRCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SHDRCLK=%ld\n", i, Dd_Top_Get_CLKSEL12_SHDRCLK(), Dd_Top_Get_SHDRCLK()));
					}
				}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
				for (i = 0; i < 16; i ++) {
					Dd_Top_Set_CLKSEL12_SHDRCLK(i);

					ULONG clock = Dd_Top_Get_SHDRCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_SHDRCLK=%ld\n", i, Dd_Top_Get_CLKSEL12_SHDRCLK(), Dd_Top_Get_SHDRCLK()));
					}
				}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			}
		}
		else if ( 0 == strcmp( argv[3], "RIBCLK") ) {
			Ddim_Print(("0:466.666MHz \n"));
			Ddim_Print(("1:350MHz \n"));
			Ddim_Print(("2:233.333MHz \n"));
			Ddim_Print(("3:116.666MHz \n"));
			Ddim_Print(("4:58.333MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL12_RIBCLK(val);

				ULONG clock = Dd_Top_Get_RIBCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_RIBCLK=%ld\n", val, Dd_Top_Get_CLKSEL12_RIBCLK(), Dd_Top_Get_RIBCLK()));
				}
			}
			else {
				for (i = 0; i < 5; i ++) {
					Dd_Top_Set_CLKSEL12_RIBCLK(i);

					ULONG clock = Dd_Top_Get_RIBCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_RIBCLK=%ld\n", i, Dd_Top_Get_CLKSEL12_RIBCLK(), Dd_Top_Get_RIBCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "NFBCHCLK") ) {
			Ddim_Print(("0:200MHz \n"));
			Ddim_Print(("1:100MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL12_NFBCHCLK(val);

				ULONG clock = Dd_Top_Get_NFBCHCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_NFBCHCLK=%ld\n", val, Dd_Top_Get_CLKSEL12_NFBCHCLK(), Dd_Top_Get_NFBCHCLK()));
				}
			}
			else {
				for (i = 0; i < 2; i ++) {
					Dd_Top_Set_CLKSEL12_NFBCHCLK(i);

					ULONG clock = Dd_Top_Get_NFBCHCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_NFBCHCLKK=%ld\n", i, Dd_Top_Get_CLKSEL12_NFBCHCLK(), Dd_Top_Get_NFBCHCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "MECLK") ) {
			Ddim_Print(("0:466.666MHz \n"));
			Ddim_Print(("1:233.333MHz \n"));
			Ddim_Print(("2:116.666MHz \n"));
			Ddim_Print(("3:58.333MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL12_MECLK(val);

				ULONG clock = Dd_Top_Get_MECLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_MECLK=%ld\n", val, Dd_Top_Get_CLKSEL12_MECLK(), Dd_Top_Get_MECLK()));
				}
			}
			else {
				for (i = 0; i < 4; i ++) {
					Dd_Top_Set_CLKSEL12_MECLK(i);

					ULONG clock = Dd_Top_Get_MECLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_MECLK=%ld\n", i, Dd_Top_Get_CLKSEL12_MECLK(), Dd_Top_Get_MECLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "FPT1CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL12_FPT1CLK(val);

				ULONG clock = Dd_Top_Get_FPT1CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_FPT1CLK=%ld\n", val, Dd_Top_Get_CLKSEL12_FPT1CLK(), Dd_Top_Get_FPT1CLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL12_FPT1CLK(i);

					ULONG clock = Dd_Top_Get_FPT1CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_FPT1CLK=%ld\n", i, Dd_Top_Get_CLKSEL12_FPT1CLK(), Dd_Top_Get_FPT1CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "FPT0CLK") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			Ddim_Print(("2:150MHz \n"));
			Ddim_Print(("3:100MHz \n"));
			Ddim_Print(("4:75MHz \n"));
			Ddim_Print(("5:60MHz \n"));
			Ddim_Print(("6:50MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL12_FPT0CLK(val);

				ULONG clock = Dd_Top_Get_FPT0CLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_FPT0CLK=%ld\n", val, Dd_Top_Get_CLKSEL12_FPT0CLK(), Dd_Top_Get_FPT0CLK()));
				}
			}
			else {
				for (i = 0; i < 7; i ++) {
					Dd_Top_Set_CLKSEL12_FPT0CLK(i);

					ULONG clock = Dd_Top_Get_FPT0CLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_FPT0CLK=%ld\n", i, Dd_Top_Get_CLKSEL12_FPT0CLK(), Dd_Top_Get_FPT0CLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "GYROCLK") ) {
			Ddim_Print(("0:100MHz \n"));
			Ddim_Print(("1:32MHz \n"));
			Ddim_Print(("2:2MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL12_GYROCLK(val);

				ULONG clock = Dd_Top_Get_GYROCLK();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_GYROCLK=%ld\n", val, Dd_Top_Get_CLKSEL12_GYROCLK(), Dd_Top_Get_GYROCLK()));
				}
			}
			else {
				for (i = 0; i < 3; i ++) {
					Dd_Top_Set_CLKSEL12_GYROCLK(i);

					ULONG clock = Dd_Top_Get_GYROCLK();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_GYROCLK=%ld\n", i, Dd_Top_Get_CLKSEL12_GYROCLK(), Dd_Top_Get_GYROCLK()));
					}
				}
			}
		}
		else if ( 0 == strcmp( argv[3], "ACLK300") ) {
			Ddim_Print(("0:300MHz \n"));
			Ddim_Print(("1:200MHz \n"));
			if (argc == 5) {
				val = atoi(argv[4]);
				Ddim_Print(("value=%lu\n", val));
				Dd_Top_Set_CLKSEL12_ACLK300(val);

				ULONG clock = Dd_Top_Get_ACLK300();
				if (clock){
					Ddim_Print(("%02lu:\t%02d: Dd_Top_Get_ACLK300=%ld\n", val, Dd_Top_Get_CLKSEL12_ACLK300(), Dd_Top_Get_ACLK300()));
				}
			}
			else {
				for (i = 0; i < 2; i ++) {
					Dd_Top_Set_CLKSEL12_ACLK300(i);

					ULONG clock = Dd_Top_Get_ACLK300();
					if (clock){
						Ddim_Print(("%02d:\t%02d: Dd_Top_Get_ACLK300=%ld\n", i, Dd_Top_Get_CLKSEL12_ACLK300(), Dd_Top_Get_ACLK300()));
					}
				}
			}
		}
		else {
			Ddim_Print(("please check 3rd parameter!!\n"));
		}
	}
	else {
		Ddim_Print(("please check parameter!!\n"));
	}
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
 * @brief  Command main function for TOP test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 *
 *	The meaning of parameters
 *
 * P1  P2        P3     P4   P5
 * lsi get_ctrl persel1								// Get persel1 register
 * lsi get_ctrl persel3								// Get persel3 register
 * lsi get_ctrl msel								// Get msel register
 * lsi get_ctrl dbcnt1								// Get dbcnt1 register
 * lsi get_ctrl dbcnt2								// Get dbcnt2 register
 * lsi get_ctrl dbcnt3								// Get dbcnt3 register
 * lsi get_ctrl dbcnt4								// Get dbcnt4 register
 * lsi get_ctrl dbcnt5								// Get dbcnt5 register
 * lsi set_ctrl persel1 I2C0CNT value				// Set persel1 I2C0CNT register
 * lsi set_ctrl persel1 SDANC0 value				// Set persel1 SDANC0 register
 * lsi set_ctrl persel1 SCLNC0 value				// Set persel1 SCLNC0 register
 * lsi set_ctrl persel1 I2C1CNT value				// Set persel1 I2C1CNT register
 * lsi set_ctrl persel1 SDANC1 value				// Set persel1 SDANC1 register
 * lsi set_ctrl persel1 SCLNC1 value				// Set persel1 SCLNC1 register
 * lsi set_ctrl persel1 I2C2CNT value				// Set persel1 I2C2CNT register
 * lsi set_ctrl persel1 SDANC2 value				// Set persel1 SDANC2 register
 * lsi set_ctrl persel1 SCLNC2 value				// Set persel1 SCLNC2 register
 * lsi set_ctrl persel1 DVWT2 value					// Set persel1 DVWT2 register
 * lsi set_ctrl persel1 DHWT3 value					// Set persel1 DHWT3 register
 * lsi set_ctrl persel1 AU0DO0C value				// Set persel1 AU0DO0C register
 * lsi set_ctrl persel1 AU0DO1C value				// Set persel1 AU0DO1C register
 * lsi set_ctrl persel1 AU1DOC value				// Set persel1 AU1DOC register
 * lsi set_ctrl persel1 AU0MCC value				// Set persel1 AU0MCC register
 * lsi set_ctrl persel1 AU1MCC value				// Set persel1 AU1MCC register
 * lsi set_ctrl persel1 RLSEL3 value				// Set persel1 RLSEL3 register
 * lsi set_ctrl persel1 RLSEL4 value				// Set persel1 RLSEL4 register
 * lsi set_ctrl persel1 RLSEL5 value				// Set persel1 RLSEL5 register
 * lsi set_ctrl persel3 AU1WPPS value				// Set persel3 AU1WPPS register
 * lsi set_ctrl mselc MSEL value					// Set mselc MSEL register
 * lsi set_ctrl dbcnt1 T00DC value					// Set dbcnt1 T00DC register
 * lsi set_ctrl dbcnt1 T01DC value					// Set dbcnt1 T01DC register
 * lsi set_ctrl dbcnt1 T02DC value					// Set dbcnt1 T02DC register
 * lsi set_ctrl dbcnt1 T03DC value					// Set dbcnt1 T03DC register
 * lsi set_ctrl dbcnt1 T04DC value					// Set dbcnt1 T04DC register
 * lsi set_ctrl dbcnt1 T05DC value					// Set dbcnt1 T05DC register
 * lsi set_ctrl dbcnt1 T06DC value					// Set dbcnt1 T06DC register
 * lsi set_ctrl dbcnt1 T07DC value					// Set dbcnt1 T07DC register
 * lsi set_ctrl dbcnt1 T08DC value					// Set dbcnt1 T08DC register
 * lsi set_ctrl dbcnt1 T09DC value					// Set dbcnt1 T09DC register
 * lsi set_ctrl dbcnt1 T10DC value					// Set dbcnt1 T10DC register
 * lsi set_ctrl dbcnt1 T11DC value					// Set dbcnt1 T11DC register
 * lsi set_ctrl dbcnt1 T12DC value					// Set dbcnt1 T12DC register
 * lsi set_ctrl dbcnt1 T13DC value					// Set dbcnt1 T13DC register
 * lsi set_ctrl dbcnt1 T14DC value					// Set dbcnt1 T14DC register
 * lsi set_ctrl dbcnt1 T15DC value					// Set dbcnt1 T15DC register
 * lsi set_ctrl dbcnt2 T16DC value					// Set dbcnt2 T16DC register
 * lsi set_ctrl dbcnt2 T17DC value					// Set dbcnt2 T17DC register
 * lsi set_ctrl dbcnt2 T18DC value					// Set dbcnt2 T18DC register
 * lsi set_ctrl dbcnt2 T19DC value					// Set dbcnt2 T19DC register
 * lsi set_ctrl dbcnt2 T20DC value					// Set dbcnt2 T20DC register
 * lsi set_ctrl dbcnt2 T21DC value					// Set dbcnt2 T21DC register
 * lsi set_ctrl dbcnt2 T22DC value					// Set dbcnt2 T22DC register
 * lsi set_ctrl dbcnt2 T23DC value					// Set dbcnt2 T23DC register
 * lsi set_ctrl dbcnt2 T24DC value					// Set dbcnt2 T24DC register
 * lsi set_ctrl dbcnt2 T25DC value					// Set dbcnt2 T25DC register
 * lsi set_ctrl dbcnt2 T26DC value					// Set dbcnt2 T26DC register
 * lsi set_ctrl dbcnt2 T27DC value					// Set dbcnt2 T27DC register
 * lsi set_ctrl dbcnt2 T28DC value					// Set dbcnt2 T28DC register
 * lsi set_ctrl dbcnt2 T29DC value					// Set dbcnt2 T29DC register
 * lsi set_ctrl dbcnt2 T30DC value					// Set dbcnt2 T30DC register
 * lsi set_ctrl dbcnt2 T31DC value					// Set dbcnt2 T31DC register
 * lsi set_ctrl dbcnt3 T32DC value					// Set dbcnt3 T32DC register
 * lsi set_ctrl dbcnt3 T33DC value					// Set dbcnt3 T33DC register
 * lsi set_ctrl dbcnt3 T34DC value					// Set dbcnt3 T34DC register
 * lsi set_ctrl dbcnt3 T35DC value					// Set dbcnt3 T35DC register
 * lsi set_ctrl dbcnt3 T36DC value					// Set dbcnt3 T36DC register
 * lsi set_ctrl dbcnt3 T37DC value					// Set dbcnt3 T37DC register
 * lsi set_ctrl dbcnt3 T38DC value					// Set dbcnt3 T38DC register
 * lsi set_ctrl dbcnt3 T39DC value					// Set dbcnt3 T39DC register
 * lsi set_ctrl dbcnt3 T40DC value					// Set dbcnt3 T40DC register
 * lsi set_ctrl dbcnt3 T41DC value					// Set dbcnt3 T41DC register
 * lsi set_ctrl dbcnt3 T42DC value					// Set dbcnt3 T42DC register
 * lsi set_ctrl dbcnt3 T43DC value					// Set dbcnt3 T43DC register
 * lsi set_ctrl dbcnt3 T44DC value					// Set dbcnt3 T44DC register
 * lsi set_ctrl dbcnt3 T45DC value					// Set dbcnt3 T45DC register
 * lsi set_ctrl dbcnt3 T46DC value					// Set dbcnt3 T46DC register
 * lsi set_ctrl dbcnt3 T47DC value					// Set dbcnt3 T47DC register
 * lsi set_ctrl dbcnt4 T48DC value					// Set dbcnt4 T48DC register
 * lsi set_ctrl dbcnt4 T49DC value					// Set dbcnt4 T49DC register
 * lsi set_ctrl dbcnt4 T50DC value					// Set dbcnt4 T50DC register
 * lsi set_ctrl dbcnt4 T51DC value					// Set dbcnt4 T51DC register
 * lsi set_ctrl dbcnt4 T52DC value					// Set dbcnt4 T52DC register
 * lsi set_ctrl dbcnt4 T53DC value					// Set dbcnt4 T53DC register
 * lsi set_ctrl dbcnt4 T54DC value					// Set dbcnt4 T54DC register
 * lsi set_ctrl dbcnt4 T55DC value					// Set dbcnt4 T55DC register
 * lsi set_ctrl dbcnt4 T56DC value					// Set dbcnt4 T56DC register
 * lsi set_ctrl dbcnt4 T57DC value					// Set dbcnt4 T57DC register
 * lsi set_ctrl dbcnt4 T58DC value					// Set dbcnt4 T58DC register
 * lsi set_ctrl dbcnt4 T59DC value					// Set dbcnt4 T59DC register
 * lsi set_ctrl dbcnt4 T60DC value					// Set dbcnt4 T60DC register
 * lsi set_ctrl dbcnt4 T61DC value					// Set dbcnt4 T61DC register
 * lsi set_ctrl dbcnt4 T62DC value					// Set dbcnt4 T62DC register
 * lsi set_ctrl dbcnt4 T63DC value					// Set dbcnt4 T63DC register
 * lsi set_ctrl dbcnt5 T64DC value					// Set dbcnt5 T64DC register
 * lsi set_ctrl dbcnt5 T65DC value					// Set dbcnt5 T65DC register
 * lsi set_ctrl dbcnt5 T66DC value					// Set dbcnt5 T66DC register
 * 
 * P1  P2        P3     P4   P5
 * clock set_reg clksel1 RCLK value					// Set clksel1 RCLK register
 * clock set_reg clksel1 UHS1CLK0 value				// Set clksel1 UHS1CLK0 register
 * clock set_reg clksel1 UHS1CLK1 value				// Set clksel1 UHS1CLK1 register
 * clock set_reg clksel1 UHS1CLK2 value				// Set clksel1 UHS1CLK2 register
 * clock set_reg clksel1 UHS2CLK value				// Set clksel1 UHS2CLK register
 * clock set_reg clksel1 NFCLK value				// Set clksel1 NFCLK register
 * clock set_reg clksel1 EMMCCLK value				// Set clksel1 EMMCCLK register
 * clock set_reg clksel2 ELACLK value				// Set clksel2 ELACLK register
 * clock set_reg clksel2 JPGCLK value				// Set clksel2 JPGCLK register
 * clock set_reg clksel2 GPUCLK value				// Set clksel2 GPUCLK register
 * clock set_reg clksel2 IPUTMECLK value			// Set clksel2 IPUTMECLK register
 * clock set_reg clksel2 IPUCLK value				// Set clksel2 IPUCLK register
 * clock set_reg clksel2 MIFCLK value				// Set clksel2 MIFCLK register
 * clock set_reg clksel2 HIFCLK value				// Set clksel2 HIFCLK register
 * clock set_reg clksel2 RAWCLK value				// Set clksel2 RAWCLK register
 * clock set_reg clksel3 RIBCLK value				// Set clksel3 RIBCLK register
 * clock set_reg clksel3 VDFCLK value				// Set clksel3 VDFCLK register
 * clock set_reg clksel3 PXFCLK value				// Set clksel3 PXFCLK register
 * clock set_reg clksel3 IPPCLK value				// Set clksel3 IPPCLK register
 * clock set_reg clksel3 PASCLK value				// Set clksel3 PASCLK register
 * clock set_reg clksel3 IIPCLK value				// Set clksel3 IIPCLK register
 * clock set_reg clksel3 SENMSKCLK value			// Set clksel3 SENMSKCLK register
 * clock set_reg clksel3 SENCLK value				// Set clksel3 SENCLK register
 * clock set_reg clksel4 CNR1CLK value				// Set clksel4 CNR1CLK register
 * clock set_reg clksel4 B2R1CLK value				// Set clksel4 B2R1CLK register
 * clock set_reg clksel4 LTM1CLK value				// Set clksel4 LTM1CLK register
 * clock set_reg clksel4 R2Y1CLK value				// Set clksel4 R2Y1CLK register
 * clock set_reg clksel4 SRO1CLK_2 value			// Set clksel4 SRO1CLK_2 register
 * clock set_reg clksel5 LTMRBK1CLK value			// Set clksel5 LTMRBK1CLK register
 * clock set_reg clksel5 B2B1CLK value				// Set clksel5 B2B1CLK register
 * clock set_reg clksel5 SRO1CLK value				// Set clksel5 SRO1CLK register
 * clock set_reg clksel6 CNR2CLK value				// Set clksel6 CNR2CLK register
 * clock set_reg clksel6 B2R2CLK value				// Set clksel6 B2R2CLK register
 * clock set_reg clksel6 LTM2CLK value				// Set clksel6 LTM2CLK register
 * clock set_reg clksel6 R2Y2CLK value				// Set clksel6 R2Y2CLK register
 * clock set_reg clksel6 SRO2CLK_2 value			// Set clksel6 SRO2CLK_2 register
 * clock set_reg clksel7 LTMRBK2CLK value			// Set clksel7 LTMRBK2CLK register
 * clock set_reg clksel7 B2B2CLK value				// Set clksel7 B2B2CLK register
 * clock set_reg clksel7 SRO2CLK value				// Set clksel7 SRO2CLK register
 * clock set_reg clksel8 DSPCLK value				// Set clksel8 DSPCLK register
 * clock set_reg clksel8 SPICLK value				// Set clksel8 SPICLK register
 * clock set_reg clksel8 AUCLK value				// Set clksel8 AUCLK register
 * clock set_reg clksel8 AU0SEL value				// Set clksel8 AU0SEL register
 * clock set_reg clksel8 AU2SEL value				// Set clksel8 AU2SEL register
 * clock set_reg clksel8 AU3SEL value				// Set clksel8 AU3SEL register
 * clock set_reg clksel8 NETAUSEL value				// Set clksel8 NETAUSEL register
 * clock set_reg clksel8 APCLK value				// Set clksel8 APCLK register
 * clock set_reg clksel8 AP0SEL value				// Set clksel8 AP0SEL register
 * clock set_reg clksel8 AP1SEL value				// Set clksel8 AP1SEL register
 * clock set_reg clksel8 AP2SEL value				// Set clksel8 AP2SEL register
 * clock set_reg clksel8 AP3SEL value				// Set clksel8 AP3SEL register
 * clock set_reg clksel9 PCLK value					// Set clksel9 PCLK register
 * clock set_reg clksel9 HCLK value					// Set clksel9 HCLK register
 * clock set_reg clksel9 HCLKBMH value				// Set clksel9 HCLKBMH register
 * clock set_reg clksel9 ACLKEXS value				// Set clksel9 ACLKEXS register
 * clock set_reg clksel9 ACLK value					// Set clksel9 ACLK register
 * clock set_reg clksel10 ACLK400 value				// Set clksel10 ACLK400 register
 * clock set_reg clksel10 MCLK200 value				// Set clksel10 MCLK200 register
 * clock set_reg clksel10 MCLK400 value				// Set clksel10 MCLK400 register
 * clock set_reg clksel11 DCHREQ value				// Set clksel11 DCHREQ register
 * clock set_reg clksel12 ACLK300 value				// Set clksel12 ACLK300 register
 * clock set_reg clksel12 GYROCLK value				// Set clksel12 GYROCLK register
 * clock set_reg clksel12 FPT0CLK value				// Set clksel12 FPT0CLK register
 * clock set_reg clksel12 FPT1CLK value				// Set clksel12 FPT1CLK register
 * clock set_reg clksel12 MECLK value				// Set clksel12 MECLK register
 * clock set_reg clksel12 NFBCHCLK value			// Set clksel12 NFBCHCLK register
 * clock set_reg clksel12 RIBCLK value				// Set clksel12 RIBCLK register
 * clock set_reg clksel12 SHDRCLK value				// Set clksel12 SHDRCLK register
 * clock set_reg pllcnt1 PL00ST value				// Set pllcnt1 PL00ST register
 * clock set_reg pllcnt1 PL01ST value				// Set pllcnt1 PL01ST register
 * clock set_reg pllcnt1 PL02ST value				// Set pllcnt1 PL02ST register
 * clock set_reg pllcnt1 PL03ST value				// Set pllcnt1 PL03ST register
 * clock set_reg pllcnt1 PL04ST value				// Set pllcnt1 PL04ST register
 * clock set_reg pllcnt1 PL05ST value				// Set pllcnt1 PL05ST register
 * clock set_reg pllcnt1 PL05AST value				// Set pllcnt1 PL05AST register
 * clock set_reg pllcnt1 PL06ST value				// Set pllcnt1 PL06ST register
 * clock set_reg pllcnt1 PL07ST value				// Set pllcnt1 PL07ST register
 * clock set_reg pllcnt1 PL08ST value				// Set pllcnt1 PL08ST register
 * clock set_reg pllcnt1 PL10ST value				// Set pllcnt1 PL10ST register
 * clock set_reg pllcnt1 PL10AST value				// Set pllcnt1 PL10AST register
 * clock set_reg pllcnt1 PL11ST value				// Set pllcnt1 PL11ST register
 * clock set_reg pllcnt1 DPL00ST value				// Set pllcnt1 DPL00ST register
 * clock set_reg pllcnt1 DPL01ST value				// Set pllcnt1 DPL01ST register
 * clock set_reg pllcnt1 DPL02ST value				// Set pllcnt1 DPL02ST register
 * clock set_reg pllcnt1 DPL10ST value				// Set pllcnt1 DPL10ST register
 * clock set_reg pllcnt1 DPL11ST value				// Set pllcnt1 DPL11ST register
 * clock set_reg pllcnt1 DPL12ST value				// Set pllcnt1 DPL12ST register
 * clock set_reg pllcnt2 PL00SEL value				// Set pllcnt2 PL00SEL register
 * clock set_reg pllcnt2 PL01SEL value				// Set pllcnt2 PL01SEL register
 * clock set_reg pllcnt2 PL02SEL value				// Set pllcnt2 PL02SEL register
 * clock set_reg pllcnt2 PL03SEL value				// Set pllcnt2 PL03SEL register
 * clock set_reg pllcnt2 PL04SEL value				// Set pllcnt2 PL04SEL register
 * clock set_reg pllcnt2 PL05SEL value				// Set pllcnt2 PL05SEL register
 * clock set_reg pllcnt2 PL05ASEL value				// Set pllcnt2 PL05ASEL register
 * clock set_reg pllcnt2 PL06SEL value				// Set pllcnt2 PL06SEL register
 * clock set_reg pllcnt2 PL07SEL value				// Set pllcnt2 PL07SEL register
 * clock set_reg pllcnt2 PL08SEL value				// Set pllcnt2 PL08SEL register
 * clock set_reg pllcnt2 PL10SEL value				// Set pllcnt2 PL10SEL register
 * clock set_reg pllcnt2 PL10ASEL value				// Set pllcnt2 PL10ASEL register
 * clock set_reg pllcnt2 PL11SEL value				// Set pllcnt2 PL11SEL register
 * clock set_reg pllcnt2 PL01SSEN value				// Set pllcnt2 PL01SSEN register
 * clock set_reg pllcnt2 PL02SSEN value				// Set pllcnt2 PL02SSEN register
 * clock set_reg pllcnt3 P00POSTDIV value			// Set pllcnt3 P00POSTDIV register
 * clock set_reg pllcnt3 P00PREDIV value			// Set pllcnt3 P00PREDIV register
 * clock set_reg pllcnt3 P00PLLDIV value			// Set pllcnt3 P00PLLDIV register
 * clock set_reg pllcnt4 P00FNUM value				// Set pllcnt4 P00FNUM register
 * clock set_reg pllcnt5 P00FDEN value				// Set pllcnt5 P00FDEN register
 * clock set_reg pllcnt7 P03POSTDIV0 value			// Set pllcnt7 P03POSTDIV0 register
 * clock set_reg pllcnt7 P03POSTDIV1 value			// Set pllcnt7 P03POSTDIV1 register
 * clock set_reg pllcnt7 P03PREDIV value			// Set pllcnt7 P03PREDIV register
 * clock set_reg pllcnt7 P03PLLDIV value			// Set pllcnt7 P03PLLDIV register
 * clock set_reg pllcnt7 P03OC0 value				// Set pllcnt7 P03OC0 register
 * clock set_reg pllcnt7 P03OC1 value				// Set pllcnt7 P03OC1 register
 * clock set_reg pllcnt7 D0XOC value				// Set pllcnt7 D0XOC register
 * clock set_reg pllcnt7 D1XOC value				// Set pllcnt7 D1XOC register
 * clock set_reg pllcnt7 DPLX8 value				// Set pllcnt7 DPLX8 register
 * clock set_reg pllcnt7 DSEL value					// Set pllcnt7 DSEL register
 * clock set_reg pllcnt8 P04POSTDIV value			// Set pllcnt8 P04POSTDIV register
 * clock set_reg pllcnt8 P04PREDIV value			// Set pllcnt8 P04PREDIV register
 * clock set_reg pllcnt8 P04PLLDIV value			// Set pllcnt8 P04PLLDIV register
 * clock set_reg pllcnt9 P10APLLDIV value			// Set pllcnt9 P10APLLDIV register
 * clock set_reg pllcnt9 P08PLLDIV value			// Set pllcnt9 P08PLLDIV register
 * clock set_reg clkstop1 DSPCK value				// Set clkstop1 DSPCK register
 * clock set_reg clkstop1 DSPAX value				// Set clkstop1 DSPAX register
 * clock set_reg clkstop1 SENCK value				// Set clkstop1 SENCK register
 * clock set_reg clkstop1 SENAX value				// Set clkstop1 SENAX register
 * clock set_reg clkstop1 SENAH value				// Set clkstop1 SENAH register
 * clock set_reg clkstop1 SENAP value				// Set clkstop1 SENAP register
 * clock set_reg clkstop1 GPIOAP value				// Set clkstop1 GPIOAP register
 * clock set_reg clkstop1 AU0CK value				// Set clkstop1 AU0CK register
 * clock set_reg clkstop1 AU2CK value				// Set clkstop1 AU2CK register
 * clock set_reg clkstop1 AU3CK value				// Set clkstop1 AU3CK register
 * clock set_reg clkstop1 AU4CK value				// Set clkstop1 AU4CK register
 * clock set_reg clkstop1 AU5CK value				// Set clkstop1 AU5CK register
 * clock set_reg clkstop1 NETAUCK value				// Set clkstop1 NETAUCK register
 * clock set_reg clkstop1 TEMPCK value				// Set clkstop1 TEMPCK register
 * clock set_reg clkstop2 RCK value					// Set clkstop2 RCK register
 * clock set_reg clkstop2 UHS1CK0 value				// Set clkstop2 UHS1CK0 register
 * clock set_reg clkstop2 UHS1CK1 value				// Set clkstop2 UHS1CK1 register
 * clock set_reg clkstop2 UHS1CK2 value				// Set clkstop2 UHS1CK2 register
 * clock set_reg clkstop2 UHS2CK value				// Set clkstop2 UHS2CK register
 * clock set_reg clkstop2 NFCK value				// Set clkstop2 NFCK register
 * clock set_reg clkstop2 EMMCCK value				// Set clkstop2 EMMCCK register
 * clock set_reg clkstop2 NETSECCK value			// Set clkstop2 NETSECCK register
 * clock set_reg clkstop2 NETRCK value				// Set clkstop2 NETRCK register
 * clock set_reg clkstop2 EXSAX value				// Set clkstop2 EXSAX register
 * clock set_reg clkstop2 SPICK value				// Set clkstop2 SPICK register
 * clock set_reg clkstop2 SLIMB00CK value			// Set clkstop2 SLIMB00CK register
 * clock set_reg clkstop2 SLIMB01CK value			// Set clkstop2 SLIMB01CK register
 * clock set_reg clkstop2 SLIMB10CK value			// Set clkstop2 SLIMB10CK register
 * clock set_reg clkstop2 SLIMB11CK value			// Set clkstop2 SLIMB11CK register
 * clock set_reg clkstop2 PCISUPPCK value			// Set clkstop2 PCISUPPCK register
 * clock set_reg clkstop3 IIPCK value				// Set clkstop3 IIPCK register
 * clock set_reg clkstop3 IIPAP value				// Set clkstop3 IIPAP register
 * clock set_reg clkstop3 IIPAH value				// Set clkstop3 IIPAH register
 * clock set_reg clkstop3 IIPAX value				// Set clkstop3 IIPAX register
 * clock set_reg clkstop3 LCDCK value				// Set clkstop3 LCDCK register
 * clock set_reg clkstop3 HIFCK value				// Set clkstop3 HIFCK register
 * clock set_reg clkstop3 MIFCK value				// Set clkstop3 MIFCK register
 * clock set_reg clkstop3 DISPAP value				// Set clkstop3 DISPAP register
 * clock set_reg clkstop3 DISPAH value				// Set clkstop3 DISPAH register
 * clock set_reg clkstop3 DISPAX value				// Set clkstop3 DISPAX register
 * clock set_reg clkstop3 JPGCK value				// Set clkstop3 JPGCK register
 * clock set_reg clkstop3 JPGAP value				// Set clkstop3 JPGAP register
 * clock set_reg clkstop3 JPGAH value				// Set clkstop3 JPGAH register
 * clock set_reg clkstop3 JPGAX value				// Set clkstop3 JPGAX register
 * clock set_reg clkstop4 GPUCK value				// Set clkstop4 GPUCK register
 * clock set_reg clkstop4 GPUAP value				// Set clkstop4 GPUAP register
 * clock set_reg clkstop4 GPUAH value				// Set clkstop4 GPUAH register
 * clock set_reg clkstop4 GPUAX value				// Set clkstop4 GPUAX register
 * clock set_reg clkstop4 FPT0CK value				// Set clkstop4 FPT0CK register
 * clock set_reg clkstop4 FPT0AP value				// Set clkstop4 FPT0AP register
 * clock set_reg clkstop4 FPT0AH value				// Set clkstop4 FPT0AH register
 * clock set_reg clkstop4 FPT0AX value				// Set clkstop4 FPT0AX register
 * clock set_reg clkstop4 FPT1CK value				// Set clkstop4 FPT1CK register
 * clock set_reg clkstop4 FPT1AP value				// Set clkstop4 FPT1AP register
 * clock set_reg clkstop4 FPT1AH value				// Set clkstop4 FPT1AH register
 * clock set_reg clkstop4 FPT1AX value				// Set clkstop4 FPT1AX register
 * clock set_reg clkstop4 APCK0 value				// Set clkstop4 APCK0 register
 * clock set_reg clkstop4 APCK1 value				// Set clkstop4 APCK1 register
 * clock set_reg clkstop4 APCK2 value				// Set clkstop4 APCK2 register
 * clock set_reg clkstop4 APCK3 value				// Set clkstop4 APCK3 register
 * clock set_reg clkstop5 MICAX0 value				// Set clkstop5 MICAX0 register
 * clock set_reg clkstop5 MICAX1 value				// Set clkstop5 MICAX1 register
 * clock set_reg clkstop5 MICAX2 value				// Set clkstop5 MICAX2 register
 * clock set_reg clkstop5 MICAX3 value				// Set clkstop5 MICAX3 register
 * clock set_reg clkstop5 MICAX4 value				// Set clkstop5 MICAX4 register
 * clock set_reg clkstop5 MICAX5 value				// Set clkstop5 MICAX5 register
 * clock set_reg clkstop5 MICAX6 value				// Set clkstop5 MICAX6 register
 * clock set_reg clkstop5 MICAP0 value				// Set clkstop5 MICAP0 register
 * clock set_reg clkstop5 MICAP1 value				// Set clkstop5 MICAP1 register
 * clock set_reg clkstop5 MICAP2 value				// Set clkstop5 MICAP2 register
 * clock set_reg clkstop5 MICAP3 value				// Set clkstop5 MICAP3 register
 * clock set_reg clkstop5 MICAP4 value				// Set clkstop5 MICAP4 register
 * clock set_reg clkstop5 MICAP5 value				// Set clkstop5 MICAP5 register
 * clock set_reg clkstop5 MICAP6 value				// Set clkstop5 MICAP6 register
 * clock set_reg clkstop6 MICAH1 value				// Set clkstop6 MICAH1 register
 * clock set_reg clkstop6 MICAH2 value				// Set clkstop6 MICAH2 register
 * clock set_reg clkstop6 MICAH3 value				// Set clkstop6 MICAH3 register
 * clock set_reg clkstop6 IMGAX value				// Set clkstop6 IMGAX register
 * clock set_reg clkstop6 IMGAH0 value				// Set clkstop6 IMGAH0 register
 * clock set_reg clkstop6 IMGAH1 value				// Set clkstop6 IMGAH1 register
 * clock set_reg clkstop6 IMGAH3 value				// Set clkstop6 IMGAH3 register
 * clock set_reg clkstop6 IMGAP3 value				// Set clkstop6 IMGAP3 register
 * clock set_reg clkstop6 REGAP value				// Set clkstop6 REGAP register
 * clock set_reg clkstop6 XCHAX value				// Set clkstop6 XCHAX register
 * clock set_reg clkstop6 XCHAP value				// Set clkstop6 XCHAP register
 * clock set_reg clkstop6 ELACK value				// Set clkstop6 ELACK register
 * clock set_reg clkstop6 ELAAX value				// Set clkstop6 ELAAX register
 * clock set_reg clkstop6 ELAAP value				// Set clkstop6 ELAAP register
 * clock set_reg clkstop7 IPUFDCK value				// Set clkstop7 IPUFDCK register
 * clock set_reg clkstop7 IPUVISCK value			// Set clkstop7 IPUVISCK register
 * clock set_reg clkstop7 IPUAX value				// Set clkstop7 IPUAX register
 * clock set_reg clkstop7 IPUAH value				// Set clkstop7 IPUAH register
 * clock set_reg clkstop7 RAWCK value				// Set clkstop7 RAWCK register
 * clock set_reg clkstop7 RAWAX value				// Set clkstop7 RAWAX register
 * clock set_reg clkstop7 RAWAP value				// Set clkstop7 RAWAP register
 * clock set_reg clkstop7 SHDRCK value				// Set clkstop7 SHDRCK register
 * clock set_reg clkstop7 SHDRAX value				// Set clkstop7 SHDRAX register
 * clock set_reg clkstop7 SHDRAH value				// Set clkstop7 SHDRAH register
 * clock set_reg clkstop7 SHDRAP value				// Set clkstop7 SHDRAP register
 * clock set_reg clkstop7 M0CK value				// Set clkstop7 M0CK register
 * clock set_reg clkstop8 RIBCK value				// Set clkstop8 RIBCK register
 * clock set_reg clkstop8 RIBAH value				// Set clkstop8 RIBAH register
 * clock set_reg clkstop8 HEVDFCK value				// Set clkstop8 HEVDFCK register
 * clock set_reg clkstop8 HEPXFCK value				// Set clkstop8 HEPXFCK register
 * clock set_reg clkstop8 HEIPPCK value				// Set clkstop8 HEIPPCK register
 * clock set_reg clkstop8 UMC0HEVCMX2 value			// Set clkstop8 UMC0HEVCMX2 register
 * clock set_reg clkstop8 UMC0HEVCMX4 value			// Set clkstop8 UMC0HEVCMX4 register
 * clock set_reg clkstop8 UMC0RBRMX4 value			// Set clkstop8 UMC0RBRMX4 register
 * clock set_reg clkstop8 UMC1HEVCMX2 value			// Set clkstop8 UMC1HEVCMX2 register
 * clock set_reg clkstop8 UMC1HEVCMX4 value			// Set clkstop8 UMC1HEVCMX4 register
 * clock set_reg clkstop8 UMC1RBRMX4 value			// Set clkstop8 UMC1RBRMX4 register
 * clock set_reg clkstop8 UMC0CMNAX value			// Set clkstop8 UMC0CMNAX register
 * clock set_reg clkstop8 UMC0MX1AX value			// Set clkstop8 UMC0MX1AX register
 * clock set_reg clkstop8 UMC0MX2AX value			// Set clkstop8 UMC0MX2AX register
 * clock set_reg clkstop8 UMC0MX3AX value			// Set clkstop8 UMC0MX3AX register
 * clock set_reg clkstop8 UMC0MX4AX value			// Set clkstop8 UMC0MX4AX register
 * clock set_reg clkstop9 UMC0MX5AX value			// Set clkstop9 UMC0MX5AX register
 * clock set_reg clkstop9 UMC1CMNAX value			// Set clkstop9 UMC1CMNAX register
 * clock set_reg clkstop9 UMC1MX1AX value			// Set clkstop9 UMC1MX1AX register
 * clock set_reg clkstop9 UMC1MX2AX value			// Set clkstop9 UMC1MX2AX register
 * clock set_reg clkstop9 UMC1MX3AX value			// Set clkstop9 UMC1MX3AX register
 * clock set_reg clkstop9 UMC1MX4AX value			// Set clkstop9 UMC1MX4AX register
 * clock set_reg clkstop9 UMC1MX5AX value			// Set clkstop9 UMC1MX5AX register
 * clock set_reg clkstop9 UMC0MX0AX3 value			// Set clkstop9 UMC0MX0AX3 register
 * clock set_reg clkstop9 UMC0MX6AX4 value			// Set clkstop9 UMC0MX6AX4 register
 * clock set_reg clkstop9 UMC0HEVCAX4 value			// Set clkstop9 UMC0HEVCAX4 register
 * clock set_reg clkstop9 UMC1MX0AX3 value			// Set clkstop9 UMC1MX0AX3 register
 * clock set_reg clkstop9 UMC1MX6AX4 value			// Set clkstop9 UMC1MX6AX4 register
 * clock set_reg clkstop9 UMC1HEVCAX4 value			// Set clkstop9 UMC1HEVCAX4 register
 * clock set_reg clkstop9 UMC0AP value				// Set clkstop9 UMC0AP register
 * clock set_reg clkstop9 UMC1AP value				// Set clkstop9 UMC1AP register
 * clock set_reg clkstop10 SRO1CK value				// Set clkstop10 SRO1CK register
 * clock set_reg clkstop10 SRO1CK_2 value			// Set clkstop10 SRO1CK_2 register
 * clock set_reg clkstop10 SRO1AX value				// Set clkstop10 SRO1AX register
 * clock set_reg clkstop10 SRO1AH value				// Set clkstop10 SRO1AH register
 * clock set_reg clkstop10 SRO1AP value				// Set clkstop10 SRO1AP register
 * clock set_reg clkstop10 B2B1CK value				// Set clkstop10 B2B1CK register
 * clock set_reg clkstop10 B2B1AX value				// Set clkstop10 B2B1AX register
 * clock set_reg clkstop10 B2B1AH value				// Set clkstop10 B2B1AH register
 * clock set_reg clkstop10 B2B1AP value				// Set clkstop10 B2B1AP register
 * clock set_reg clkstop10 LTMRBK1CK value			// Set clkstop10 LTMRBK1CK register
 * clock set_reg clkstop10 B2R1CK value				// Set clkstop10 B2R1CK register
 * clock set_reg clkstop10 B2R1AX value				// Set clkstop10 B2R1AX register
 * clock set_reg clkstop10 B2R1AH value				// Set clkstop10 B2R1AH register
 * clock set_reg clkstop10 B2R1AP value				// Set clkstop10 B2R1AP register
 * clock set_reg clkstop11 LTM1CK value				// Set clkstop11 LTM1CK register
 * clock set_reg clkstop11 LTM1AX value				// Set clkstop11 LTM1AX register
 * clock set_reg clkstop11 LTM1AH value				// Set clkstop11 LTM1AH register
 * clock set_reg clkstop11 LTM1AP value				// Set clkstop11 LTM1AP register
 * clock set_reg clkstop11 R2Y1CK value				// Set clkstop11 R2Y1CK register
 * clock set_reg clkstop11 R2Y1AX value				// Set clkstop11 R2Y1AX register
 * clock set_reg clkstop11 R2Y1AH value				// Set clkstop11 R2Y1AH register
 * clock set_reg clkstop11 R2Y1AP value				// Set clkstop11 R2Y1AP register
 * clock set_reg clkstop11 CNR1CK value				// Set clkstop11 CNR1CK register
 * clock set_reg clkstop11 CNR1AX value				// Set clkstop11 CNR1AX register
 * clock set_reg clkstop11 CNR1AP value				// Set clkstop11 CNR1AP register
 * clock set_reg clkstop12 SRO2CK value				// Set clkstop12 SRO2CK register
 * clock set_reg clkstop12 SRO2CK_2 value			// Set clkstop12 SRO2CK_2 register
 * clock set_reg clkstop12 SRO2AX value				// Set clkstop12 SRO2AX register
 * clock set_reg clkstop12 SRO2AH value				// Set clkstop12 SRO2AH register
 * clock set_reg clkstop12 SRO2AP value				// Set clkstop12 SRO2AP register
 * clock set_reg clkstop12 B2B2CK value				// Set clkstop12 B2B2CK register
 * clock set_reg clkstop12 B2B2AX value				// Set clkstop12 B2B2AX register
 * clock set_reg clkstop12 B2B2AH value				// Set clkstop12 B2B2AH register
 * clock set_reg clkstop12 B2B2AP value				// Set clkstop12 B2B2AP register
 * clock set_reg clkstop12 LTMRBK2CK value			// Set clkstop12 LTMRBK2CK register
 * clock set_reg clkstop12 B2R2CK value				// Set clkstop12 B2R2CK register
 * clock set_reg clkstop12 B2R2AX value				// Set clkstop12 B2R2AX register
 * clock set_reg clkstop12 B2R2AH value				// Set clkstop12 B2R2AH register
 * clock set_reg clkstop12 B2R2AP value				// Set clkstop12 B2R2AP register
 * clock set_reg clkstop13 LTM2CK value				// Set clkstop13 LTM2CK register
 * clock set_reg clkstop13 LTM2AX value				// Set clkstop13 LTM2AX register
 * clock set_reg clkstop13 LTM2AH value				// Set clkstop13 LTM2AH register
 * clock set_reg clkstop13 LTM2AP value				// Set clkstop13 LTM2AP register
 * clock set_reg clkstop13 R2Y2CK value				// Set clkstop13 R2Y2CK register
 * clock set_reg clkstop13 R2Y2AX value				// Set clkstop13 R2Y2AX register
 * clock set_reg clkstop13 R2Y2AH value				// Set clkstop13 R2Y2AH register
 * clock set_reg clkstop13 R2Y2AP value				// Set clkstop13 R2Y2AP register
 * clock set_reg clkstop13 CNR2CK value				// Set clkstop13 CNR2CK register
 * clock set_reg clkstop13 CNR2AX value				// Set clkstop13 CNR2AX register
 * clock set_reg clkstop13 CNR2AP value				// Set clkstop13 CNR2AP register
 * clock set_reg clkstop14 STATAX value				// Set clkstop14 STATAX register
 * clock set_reg clkstop14 STATAH value				// Set clkstop14 STATAH register
 * clock set_reg clkstop14 STATAP value				// Set clkstop14 STATAP register
 * clock set_reg clkstop14 PERIAH value				// Set clkstop14 PERIAH register
 * clock set_reg clkstop14 PERIAP value				// Set clkstop14 PERIAP register
 * clock set_reg clkstop14 SENMSKCK value			// Set clkstop14 SENMSKCK register
 * clock set_reg clkstop14 PDMCK value				// Set clkstop14 PDMCK register
 * clock set_reg clkstop14 GYROCK value				// Set clkstop14 GYROCK register
 * clock get_reg clksel1								// Get clksel1 register
 * clock get_reg clksel2								// Get clksel2 register
 * clock get_reg clksel3								// Get clksel3 register
 * clock get_reg clksel4								// Get clksel4 register
 * clock get_reg clksel5								// Get clksel5 register
 * clock get_reg clksel6								// Get clksel6 register
 * clock get_reg clksel7								// Get clksel7 register
 * clock get_reg clksel8								// Get clksel8 register
 * clock get_reg clksel9								// Get clksel9 register
 * clock get_reg clksel10								// Get clksel10 register
 * clock get_reg clksel11								// Get clksel11 register
 * clock get_reg clksel12								// Get clksel12 register
 * clock get_reg pllcnt1								// Get pllcnt1 register
 * clock get_reg pllcnt2								// Get pllcnt2 register
 * clock get_reg pllcnt3								// Get pllcnt3 register
 * clock get_reg pllcnt4								// Get pllcnt4 register
 * clock get_reg pllcnt5								// Get pllcnt5 register
 * clock get_reg pllcnt7								// Get pllcnt7 register
 * clock get_reg pllcnt8								// Get pllcnt8 register
 * clock get_reg pllcnt9								// Get pllcnt9 register
 * clock get_reg clkstop1								// Get clkstop1 register
 * clock get_reg clkstop2								// Get clkstop2 register
 * clock get_reg clkstop3								// Get clkstop3 register
 * clock get_reg clkstop4								// Get clkstop4 register
 * clock get_reg clkstop5								// Get clkstop5 register
 * clock get_reg clkstop6								// Get clkstop6 register
 * clock get_reg clkstop7								// Get clkstop7 register
 * clock get_reg clkstop8								// Get clkstop8 register
 * clock get_reg clkstop9								// Get clkstop9 register
 * clock get_reg clkstop10								// Get clkstop10 register
 * clock get_reg clkstop11								// Get clkstop11 register
 * clock get_reg clkstop12								// Get clkstop12 register
 * clock get_reg clkstop13								// Get clkstop13 register
 * clock get_reg clkstop14								// Get clkstop14 register
 * clock get_clock EMMCCLK value								// Get EMMCCLK register
 * clock get_clock NFCLK value								// Get NFCLK register
 * clock get_clock UHS2CLK value								// Get UHS2CLK register
 * clock get_clock UHS1CLK2 value								// Get UHS1CLK2 register
 * clock get_clock UHS1CLK1 value								// Get UHS1CLK1 register
 * clock get_clock UHS1CLK0 value								// Get UHS1CLK0 register
 * clock get_clock RCLK value								// Get RCLK register
 * clock get_clock RAWCLK value								// Get RAWCLK register
 * clock get_clock HIFCLK value								// Get HIFCLK register
 * clock get_clock MIFCLK value								// Get MIFCLK register
 * clock get_clock IPUCLK value								// Get IPUCLK register
 * clock get_clock IPUTMECLK value								// Get IPUTMECLK register
 * clock get_clock GPUCLK value								// Get GPUCLK register
 * clock get_clock JPGCLK value								// Get JPGCLK register
 * clock get_clock ELACLK value								// Get ELACLK register
 * clock get_clock SENCLK value								// Get SENCLK register
 * clock get_clock SENMSKCLK value								// Get SENMSKCLK register
 * clock get_clock IIPCLK value								// Get IIPCLK register
 * clock get_clock PASCLK value								// Get PASCLK register
 * clock get_clock IPPCLK value								// Get IPPCLK register
 * clock get_clock PXFCLK value								// Get PXFCLK register
 * clock get_clock VDFCLK value								// Get VDFCLK register
 * clock get_clock RIBCLK value								// Get RIBCLK register
 * clock get_clock SRO1CLK_2 value								// Get SRO1CLK_2 register
 * clock get_clock R2Y1CLK value								// Get R2Y1CLK register
 * clock get_clock SRO1CLK value								// Get SRO1CLK register
 * clock get_clock LTM1CLK value								// Get LTM1CLK register
 * clock get_clock B2R1CLK value								// Get B2R1CLK register
 * clock get_clock CNR1CLK value								// Get CNR1CLK register
 * clock get_clock B2B1CLK value								// Get B2B1CLK register
 * clock get_clock LTMRBK1CLK value								// Get LTMRBK1CLK register
 * clock get_clock SRO2CLK_2 value								// Get SRO2CLK_2 register
 * clock get_clock R2Y2CLK value								// Get R2Y2CLK register
 * clock get_clock LTM2CLK value								// Get LTM2CLK register
 * clock get_clock B2R2CLK value								// Get B2R2CLK register
 * clock get_clock CNR2CLK value								// Get CNR2CLK register
 * clock get_clock SRO2CLK value								// Get SRO2CLK register
 * clock get_clock B2B2CLK value								// Get B2B2CLK register
 * clock get_clock LTMRBK2CLK value								// Get LTMRBK2CLK register
 * clock get_clock APCLK value								// Get APCLK register
 * clock get_clock AUCLK value								// Get AUCLK register
 * clock get_clock SPICLK value								// Get SPICLK register
 * clock get_clock DSPCLK value								// Get DSPCLK register
 * clock get_clock ACLK value								// Get ACLK register
 * clock get_clock ACLKEXS value								// Get ACLKEXS register
 * clock get_clock HCLKBMH value								// Get HCLKBMH register
 * clock get_clock HCLK value								// Get HCLK register
 * clock get_clock PCLK value								// Get PCLK register
 * clock get_clock MCLK400 value								// Get MCLK400 register
 * clock get_clock MCLK200 value								// Get MCLK200 register
 * clock get_clock ACLK400 value								// Get ACLK400 register
 * clock get_clock ACLK300 value								// Get ACLK300 register
 * clock get_clock GYROCLK value								// Get GYROCLK register
 * clock get_clock SHDRCLK value								// Get SHDRCLK register
 * clock get_clock FPT0CLK value								// Get FPT0CLK register
 * clock get_clock FPT1CLK value								// Get FPT1CLK register
 * clock start clkstop1 count value								// Start clkstop1 register
 * clock start clkstop2 count value								// Start clkstop2 register
 * clock start clkstop3 count value								// Start clkstop3 register
 * clock start clkstop4 count value								// Start clkstop4 register
 * clock start clkstop5 count value								// Start clkstop5 register
 * clock start clkstop6 count value								// Start clkstop6 register
 * clock start clkstop7 count value								// Start clkstop7 register
 * clock start clkstop8 count value								// Start clkstop8 register
 * clock start clkstop9 count value								// Start clkstop9 register
 * clock start clkstop10 count value								// Start clkstop10 register
 * clock start clkstop11 count value								// Start clkstop11 register
 * clock start clkstop12 count value								// Start clkstop12 register
 * clock start clkstop13 count value								// Start clkstop13 register
 * clock start clkstop14 count value								// Start clkstop14 register
 * clock stop clkstop1 count value								// Stop clkstop1 register
 * clock stop clkstop2 count value								// Stop clkstop2 register
 * clock stop clkstop3 count value								// Stop clkstop3 register
 * clock stop clkstop4 count value								// Stop clkstop4 register
 * clock stop clkstop5 count value								// Stop clkstop5 register
 * clock stop clkstop6 count value								// Stop clkstop6 register
 * clock stop clkstop7 count value								// Stop clkstop7 register
 * clock stop clkstop8 count value								// Stop clkstop8 register
 * clock stop clkstop9 count value								// Stop clkstop9 register
 * clock stop clkstop10 count value								// Stop clkstop10 register
 * clock stop clkstop11 count value								// Stop clkstop11 register
 * clock stop clkstop12 count value								// Stop clkstop12 register
 * clock stop clkstop13 count value								// Stop clkstop13 register
 * clock stop clkstop14 count value								// Stop clkstop14 register
 * 
 * P1  P2    P3
 * pll pll00 start								// Start pll00 PLL clock
 * pll pll00 stop								// Stop	 pll00 PLL clock
 * pll pll01 start								// Start pll01 PLL clock
 * pll pll01 stop								// Stop	 pll01 PLL clock
 * pll pll02 start								// Start pll02 PLL clock
 * pll pll02 stop								// Stop	 pll02 PLL clock
 * pll pll03 start								// Start pll03 PLL clock
 * pll pll03 stop								// Stop	 pll03 PLL clock
 * pll pll04 start								// Start pll04 PLL clock
 * pll pll04 stop								// Stop	 pll04 PLL clock
 * pll pll05 start								// Start pll05 PLL clock
 * pll pll05 stop								// Stop	 pll05 PLL clock
 * pll pll05a start								// Start pll05a PLL clock
 * pll pll05a stop								// Stop	 pll05a PLL clock
 * pll pll06 start								// Start pll06 PLL clock
 * pll pll06 stop								// Stop	 pll06 PLL clock
 * pll pll07 start								// Start pll07 PLL clock
 * pll pll07 stop								// Stop	 pll07 PLL clock
 * pll pll08 start								// Start pll08 PLL clock
 * pll pll08 stop								// Stop	 pll08 PLL clock
 * pll pll10 start								// Start pll10 PLL clock
 * pll pll10 stop								// Stop	 pll10 PLL clock
 * pll pll10a start								// Start pll10a PLL clock
 * pll pll10a stop								// Stop	 pll10a PLL clock
 * pll pll11 start								// Start pll11 PLL clock
 * pll pll11 stop								// Stop	 pll11 PLL clock
 * pll ddr00 start								// Start ddr00 PLL clock
 * pll ddr00 stop								// Stop	 ddr00 PLL clock
 * pll ddr01 start								// Start ddr01 PLL clock
 * pll ddr01 stop								// Stop	 ddr01 PLL clock
 * pll ddr02 start								// Start ddr02 PLL clock
 * pll ddr02 stop								// Stop	 ddr02 PLL clock
 * pll ddr10 start								// Start ddr10 PLL clock
 * pll ddr10 stop								// Stop	 ddr10 PLL clock
 * pll ddr11 start								// Start ddr11 PLL clock
 * pll ddr11 stop								// Stop	 ddr11 PLL clock
 * pll ddr12 start								// Start ddr12 PLL clock
 * pll ddr12 stop								// Stop	 ddr12 PLL clock
 * pll pl00sel pll_select								// Set pl00sel pll select register
 * pll pl01sel pll_select								// Set pl01sel pll select register
 * pll pl02sel pll_select								// Set pl02sel pll select register
 * pll pl03sel pll_select								// Set pl03sel pll select register
 * pll pl04sel pll_select								// Set pl04sel pll select register
 * pll pl05sel pll_select								// Set pl05sel pll select register
 * pll pl05asel pll_select								// Set pl05asel pll select register
 * pll pl06sel pll_select								// Set pl06sel pll select register
 * pll pl07sel pll_select								// Set pl07sel pll select register
 * pll pl08sel pll_select								// Set pl08sel pll select register
 * pll pl10sel pll_select								// Set pl10sel pll select register
 * pll pl10asel pll_select								// Set pl10asel pll select register
 * pll pl11sel pll_select								// Set pl11sel pll select register
 * pll pl01ssen pll_select								// Set pl01ssen pll select register
 * pll pl02ssen pll_select								// Set pl02ssen pll select register
 * pll p00postdiv pll_select								// Set p00postdiv pll select register
 * pll p00prediv pll_select								// Set p00prediv pll select register
 * pll p00plldiv pll_select								// Set p00plldiv pll select register
 * pll p00fnum pll_select								// Set p00fnum pll select register
 * pll p00fden pll_select								// Set p00fden pll select register
 * pll p03postdiv0 pll_select								// Set p03postdiv0 pll select register
 * pll p03postdiv1 pll_select								// Set p03postdiv1 pll select register
 * pll p03prediv pll_select								// Set p03prediv pll select register
 * pll p03plldiv pll_select								// Set p03plldiv pll select register
 * pll p03oc0 pll_select								// Set p03oc0 pll select register
 * pll p03oc1 pll_select								// Set p03oc1 pll select register
 * pll d0xoc pll_select								// Set d0xoc pll select register
 * pll d1xoc pll_select								// Set d1xoc pll select register
 * pll dodvsel pll_select								// Set dodvsel pll select register
 * pll p04postdiv pll_select								// Set p04postdiv pll select register
 * pll p04prediv pll_select								// Set p04prediv pll select register
 * pll p04plldiv pll_select								// Set p04plldiv pll select register
 * pll p10aplldiv pll_select								// Set p10aplldiv pll select register
 * pll p08plldiv pll_select								// Set p08plldiv pll select register
 * pll err										// API parameter error check

 *
 * P1    P2  P3
 * reset get									// Get CRRSM/CRRRS register
 * reset set CRRSM_WDRST						// Set Watchdog Reset Clear in CRRSM register
 * reset set CRRSM_SWRST						// Set SRREQ Reset Clear in CRRSM register
 * reset set CRRSM_SRST							// Set TSRSTX Terminal Reset Clear in CRRSM register
 * reset set CRRSM_PORST						// Set RSTX Terminal Reset Clear in CRRSM register
 * reset set CRRSM_CPU0_SWRST							// Set CPU0_SR Bit Reset Clear in CRRSM register
 * reset set CRRSM_CPU1_SWRST							// Set CPU1_SR Bit Reset Clear in CRRSM register
 * reset set CRRSM_CPU2_SWRST							// Set CPU2_SR Bit Reset Clear in CRRSM register
 * reset set CRRSM_CPU3_SWRST							// Set CPU3_SR Bit Reset Clear in CRRSM register
 *
 * P1   P2  P3   P4   P5
 * gpio set regi port value						// Set EPCR/PDR/DDR/PUDER/PUDCR register
 * gpio get regi port							// Get EPCR/PDR/DDR/PUDER/PUDCR register
 * gpio test								// Init EPCR/PDR/DDR/PUDER/PUDCR register (0 crear)
 * gpio err										// API parameter error check
 *
 *	 regi:
 *		"EPCR"  :External Port Control Register
 *		"DDR"   :Port Direction Register
 *		"PDR"   :Port Data Register
 *		"PUDER" :Pull-Up/Down Enable Register
 *		"PUDCR" :Pull-Up/Down Control Register
 *
 *	 port:
 *		"Pxx"   ex)"P37"
 */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
 * @brief  Command main function for TOP test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 *
 *	The meaning of parameters
 *
 * P1  P2        P3     P4   P5
 * lsi get_ctrl persel1								// Get persel1 register
 * lsi get_ctrl persel3								// Get persel3 register
 * lsi get_ctrl msel								// Get msel register
 * lsi get_ctrl dbcnt1								// Get dbcnt1 register
 * lsi get_ctrl dbcnt2								// Get dbcnt2 register
 * lsi get_ctrl dbcnt3								// Get dbcnt3 register
 * lsi get_ctrl dbcnt4								// Get dbcnt4 register
 * lsi get_ctrl dbcnt5								// Get dbcnt5 register
 * lsi set_ctrl persel1 I2C0CNT value				// Set persel1 I2C0CNT register
 * lsi set_ctrl persel1 SDANC0 value				// Set persel1 SDANC0 register
 * lsi set_ctrl persel1 SCLNC0 value				// Set persel1 SCLNC0 register
 * lsi set_ctrl persel1 I2C1CNT value				// Set persel1 I2C1CNT register
 * lsi set_ctrl persel1 SDANC1 value				// Set persel1 SDANC1 register
 * lsi set_ctrl persel1 SCLNC1 value				// Set persel1 SCLNC1 register
 * lsi set_ctrl persel1 I2C2CNT value				// Set persel1 I2C2CNT register
 * lsi set_ctrl persel1 SDANC2 value				// Set persel1 SDANC2 register
 * lsi set_ctrl persel1 SCLNC2 value				// Set persel1 SCLNC2 register
 * lsi set_ctrl persel1 DVWT2 value					// Set persel1 DVWT2 register
 * lsi set_ctrl persel1 DHWT3 value					// Set persel1 DHWT3 register
 * lsi set_ctrl persel1 AU0DO0C value				// Set persel1 AU0DO0C register
 * lsi set_ctrl persel1 AU0DO1C value				// Set persel1 AU0DO1C register
 * lsi set_ctrl persel1 AU1DOC value				// Set persel1 AU1DOC register
 * lsi set_ctrl persel1 AU0MCC value				// Set persel1 AU0MCC register
 * lsi set_ctrl persel1 AU1MCC value				// Set persel1 AU1MCC register
 * lsi set_ctrl persel1 RLSEL3 value				// Set persel1 RLSEL3 register
 * lsi set_ctrl persel1 RLSEL4 value				// Set persel1 RLSEL4 register
 * lsi set_ctrl persel1 RLSEL5 value				// Set persel1 RLSEL5 register
 * lsi set_ctrl persel3 AU1WPPS value				// Set persel3 AU1WPPS register
 * lsi set_ctrl mselc MSEL value					// Set mselc MSEL register
 * lsi set_ctrl dbcnt1 T00DC value					// Set dbcnt1 T00DC register
 * lsi set_ctrl dbcnt1 T01DC value					// Set dbcnt1 T01DC register
 * lsi set_ctrl dbcnt1 T02DC value					// Set dbcnt1 T02DC register
 * lsi set_ctrl dbcnt1 T03DC value					// Set dbcnt1 T03DC register
 * lsi set_ctrl dbcnt1 T04DC value					// Set dbcnt1 T04DC register
 * lsi set_ctrl dbcnt1 T05DC value					// Set dbcnt1 T05DC register
 * lsi set_ctrl dbcnt1 T06DC value					// Set dbcnt1 T06DC register
 * lsi set_ctrl dbcnt1 T07DC value					// Set dbcnt1 T07DC register
 * lsi set_ctrl dbcnt1 T08DC value					// Set dbcnt1 T08DC register
 * lsi set_ctrl dbcnt1 T09DC value					// Set dbcnt1 T09DC register
 * lsi set_ctrl dbcnt1 T10DC value					// Set dbcnt1 T10DC register
 * lsi set_ctrl dbcnt1 T11DC value					// Set dbcnt1 T11DC register
 * lsi set_ctrl dbcnt1 T12DC value					// Set dbcnt1 T12DC register
 * lsi set_ctrl dbcnt1 T13DC value					// Set dbcnt1 T13DC register
 * lsi set_ctrl dbcnt1 T14DC value					// Set dbcnt1 T14DC register
 * lsi set_ctrl dbcnt1 T15DC value					// Set dbcnt1 T15DC register
 * lsi set_ctrl dbcnt2 T16DC value					// Set dbcnt2 T16DC register
 * lsi set_ctrl dbcnt2 T17DC value					// Set dbcnt2 T17DC register
 * lsi set_ctrl dbcnt2 T18DC value					// Set dbcnt2 T18DC register
 * lsi set_ctrl dbcnt2 T19DC value					// Set dbcnt2 T19DC register
 * lsi set_ctrl dbcnt2 T20DC value					// Set dbcnt2 T20DC register
 * lsi set_ctrl dbcnt2 T21DC value					// Set dbcnt2 T21DC register
 * lsi set_ctrl dbcnt2 T22DC value					// Set dbcnt2 T22DC register
 * lsi set_ctrl dbcnt2 T23DC value					// Set dbcnt2 T23DC register
 * lsi set_ctrl dbcnt2 T24DC value					// Set dbcnt2 T24DC register
 * lsi set_ctrl dbcnt2 T25DC value					// Set dbcnt2 T25DC register
 * lsi set_ctrl dbcnt2 T26DC value					// Set dbcnt2 T26DC register
 * lsi set_ctrl dbcnt2 T27DC value					// Set dbcnt2 T27DC register
 * lsi set_ctrl dbcnt2 T28DC value					// Set dbcnt2 T28DC register
 * lsi set_ctrl dbcnt2 T29DC value					// Set dbcnt2 T29DC register
 * lsi set_ctrl dbcnt2 T30DC value					// Set dbcnt2 T30DC register
 * lsi set_ctrl dbcnt2 T31DC value					// Set dbcnt2 T31DC register
 * lsi set_ctrl dbcnt3 T32DC value					// Set dbcnt3 T32DC register
 * lsi set_ctrl dbcnt3 T33DC value					// Set dbcnt3 T33DC register
 * lsi set_ctrl dbcnt3 T34DC value					// Set dbcnt3 T34DC register
 * lsi set_ctrl dbcnt3 T35DC value					// Set dbcnt3 T35DC register
 * lsi set_ctrl dbcnt3 T36DC value					// Set dbcnt3 T36DC register
 * lsi set_ctrl dbcnt3 T37DC value					// Set dbcnt3 T37DC register
 * lsi set_ctrl dbcnt3 T38DC value					// Set dbcnt3 T38DC register
 * lsi set_ctrl dbcnt3 T39DC value					// Set dbcnt3 T39DC register
 * lsi set_ctrl dbcnt3 T40DC value					// Set dbcnt3 T40DC register
 * lsi set_ctrl dbcnt3 T41DC value					// Set dbcnt3 T41DC register
 * lsi set_ctrl dbcnt3 T42DC value					// Set dbcnt3 T42DC register
 * lsi set_ctrl dbcnt3 T43DC value					// Set dbcnt3 T43DC register
 * lsi set_ctrl dbcnt3 T44DC value					// Set dbcnt3 T44DC register
 * lsi set_ctrl dbcnt3 T45DC value					// Set dbcnt3 T45DC register
 * lsi set_ctrl dbcnt3 T46DC value					// Set dbcnt3 T46DC register
 * lsi set_ctrl dbcnt3 T47DC value					// Set dbcnt3 T47DC register
 * lsi set_ctrl dbcnt4 T48DC value					// Set dbcnt4 T48DC register
 * lsi set_ctrl dbcnt4 T49DC value					// Set dbcnt4 T49DC register
 * lsi set_ctrl dbcnt4 T50DC value					// Set dbcnt4 T50DC register
 * lsi set_ctrl dbcnt4 T51DC value					// Set dbcnt4 T51DC register
 * lsi set_ctrl dbcnt4 T52DC value					// Set dbcnt4 T52DC register
 * lsi set_ctrl dbcnt4 T53DC value					// Set dbcnt4 T53DC register
 * lsi set_ctrl dbcnt4 T54DC value					// Set dbcnt4 T54DC register
 * lsi set_ctrl dbcnt4 T55DC value					// Set dbcnt4 T55DC register
 * lsi set_ctrl dbcnt4 T56DC value					// Set dbcnt4 T56DC register
 * lsi set_ctrl dbcnt4 T57DC value					// Set dbcnt4 T57DC register
 * lsi set_ctrl dbcnt4 T58DC value					// Set dbcnt4 T58DC register
 * lsi set_ctrl dbcnt4 T59DC value					// Set dbcnt4 T59DC register
 * lsi set_ctrl dbcnt4 T60DC value					// Set dbcnt4 T60DC register
 * lsi set_ctrl dbcnt4 T61DC value					// Set dbcnt4 T61DC register
 * lsi set_ctrl dbcnt4 T62DC value					// Set dbcnt4 T62DC register
 * lsi set_ctrl dbcnt4 T63DC value					// Set dbcnt4 T63DC register
 * lsi set_ctrl dbcnt5 T64DC value					// Set dbcnt5 T64DC register
 * lsi set_ctrl dbcnt5 T65DC value					// Set dbcnt5 T65DC register
 * lsi set_ctrl dbcnt5 T66DC value					// Set dbcnt5 T66DC register
 * 
 * P1  P2        P3     P4   P5
 * clock set_reg clksel1 RCLK value					// Set clksel1 RCLK register
 * clock set_reg clksel1 UHS1CLK0 value				// Set clksel1 UHS1CLK0 register
 * clock set_reg clksel1 UHS1CLK1 value				// Set clksel1 UHS1CLK1 register
 * clock set_reg clksel1 UHS1CLK2 value				// Set clksel1 UHS1CLK2 register
 * clock set_reg clksel1 UHS2CLK value				// Set clksel1 UHS2CLK register
 * clock set_reg clksel1 NFCLK value				// Set clksel1 NFCLK register
 * clock set_reg clksel1 EMMCCLK value				// Set clksel1 EMMCCLK register
 * clock set_reg clksel2 ELACLK value				// Set clksel2 ELACLK register
 * clock set_reg clksel2 JPGCLK value				// Set clksel2 JPGCLK register
 * clock set_reg clksel2 GPUCLK value				// Set clksel2 GPUCLK register
 * clock set_reg clksel2 IPUTMECLK value			// Set clksel2 IPUTMECLK register
 * clock set_reg clksel2 IPUCLK value				// Set clksel2 IPUCLK register
 * clock set_reg clksel2 MIFCLK value				// Set clksel2 MIFCLK register
 * clock set_reg clksel2 HIFCLK value				// Set clksel2 HIFCLK register
 * clock set_reg clksel2 RAWCLK value				// Set clksel2 RAWCLK register
 * clock set_reg clksel3 RIBCLK value				// Set clksel3 RIBCLK register
 * clock set_reg clksel3 VDFCLK value				// Set clksel3 VDFCLK register
 * clock set_reg clksel3 HEVENCCLK value			// Set clksel3 HEVENCCLK register
 * clock set_reg clksel3 IPPCLK value				// Set clksel3 IPPCLK register
 * clock set_reg clksel3 PASCLK value				// Set clksel3 PASCLK register
 * clock set_reg clksel3 IIPCLK value				// Set clksel3 IIPCLK register
 * clock set_reg clksel3 SENMSKCLK value			// Set clksel3 SENMSKCLK register
 * clock set_reg clksel3 SENCLK value				// Set clksel3 SENCLK register
 * clock set_reg clksel3 HEVENCSEL value			// Set clksel3 HEVENCSEL register
 * clock set_reg clksel4 CNR1CLK value				// Set clksel4 CNR1CLK register
 * clock set_reg clksel4 B2R1CLK value				// Set clksel4 B2R1CLK register
 * clock set_reg clksel4 LTM1CLK value				// Set clksel4 LTM1CLK register
 * clock set_reg clksel4 R2Y1CLK value				// Set clksel4 R2Y1CLK register
 * clock set_reg clksel4 SRO1CLK_2 value			// Set clksel4 SRO1CLK_2 register
 * clock set_reg clksel5 LTMRBK1CLK value			// Set clksel5 LTMRBK1CLK register
 * clock set_reg clksel5 B2B1CLK value				// Set clksel5 B2B1CLK register
 * clock set_reg clksel5 SRO1CLK value				// Set clksel5 SRO1CLK register
 * clock set_reg clksel6 CNR2CLK value				// Set clksel6 CNR2CLK register
 * clock set_reg clksel6 B2R2CLK value				// Set clksel6 B2R2CLK register
 * clock set_reg clksel6 LTM2CLK value				// Set clksel6 LTM2CLK register
 * clock set_reg clksel6 R2Y2CLK value				// Set clksel6 R2Y2CLK register
 * clock set_reg clksel6 SRO2CLK_2 value			// Set clksel6 SRO2CLK_2 register
 * clock set_reg clksel7 LTMRBK2CLK value			// Set clksel7 LTMRBK2CLK register
 * clock set_reg clksel7 B2B2CLK value				// Set clksel7 B2B2CLK register
 * clock set_reg clksel7 SRO2CLK value				// Set clksel7 SRO2CLK register
 * clock set_reg clksel7 MIPISEL value				// Set clksel7 MIPISEL register
 * clock set_reg clksel7 PDM0SEL value				// Set clksel7 PDM0SEL register
 * clock set_reg clksel7 PDM1SEL value				// Set clksel7 PDM1SEL register
 * clock set_reg clksel8 DSPCLK value				// Set clksel8 DSPCLK register
 * clock set_reg clksel8 SPICLK value				// Set clksel8 SPICLK register
 * clock set_reg clksel8 AUCLK value				// Set clksel8 AUCLK register
 * clock set_reg clksel8 AU0SEL value				// Set clksel8 AU0SEL register
 * clock set_reg clksel8 AU2SEL value				// Set clksel8 AU2SEL register
 * clock set_reg clksel8 AU3SEL value				// Set clksel8 AU3SEL register
 * clock set_reg clksel8 NETAUSEL value				// Set clksel8 NETAUSEL register
 * clock set_reg clksel8 APCLK value				// Set clksel8 APCLK register
 * clock set_reg clksel8 AP0SEL value				// Set clksel8 AP0SEL register
 * clock set_reg clksel8 AP1SEL value				// Set clksel8 AP1SEL register
 * clock set_reg clksel8 AP2SEL value				// Set clksel8 AP2SEL register
 * clock set_reg clksel8 AP3SEL value				// Set clksel8 AP3SEL register
 * clock set_reg clksel9 PCLK value					// Set clksel9 PCLK register
 * clock set_reg clksel9 HCLK value					// Set clksel9 HCLK register
 * clock set_reg clksel9 HCLKBMH value				// Set clksel9 HCLKBMH register
 * clock set_reg clksel9 ACLKEXS value				// Set clksel9 ACLKEXS register
 * clock set_reg clksel9 ACLK value					// Set clksel9 ACLK register
 * clock set_reg clksel10 ACLK400 value				// Set clksel10 ACLK400 register
 * clock set_reg clksel10 MCLK200 value				// Set clksel10 MCLK200 register
 * clock set_reg clksel10 MCLK400 value				// Set clksel10 MCLK400 register
 * clock set_reg clksel11 DCHREQ value				// Set clksel11 DCHREQ register
 * clock set_reg clksel12 ACLK300 value				// Set clksel12 ACLK300 register
 * clock set_reg clksel12 GYROCLK value				// Set clksel12 GYROCLK register
 * clock set_reg clksel12 FPT0CLK value				// Set clksel12 FPT0CLK register
 * clock set_reg clksel12 FPT1CLK value				// Set clksel12 FPT1CLK register
 * clock set_reg clksel12 MECLK value				// Set clksel12 MECLK register
 * clock set_reg clksel12 NFBCHCLK value			// Set clksel12 NFBCHCLK register
 * clock set_reg clksel12 RIBCLK value				// Set clksel12 RIBCLK register
 * clock set_reg clksel12 SHDRCLK value				// Set clksel12 SHDRCLK register
 * clock set_reg pllcnt1 PL00ST value				// Set pllcnt1 PL00ST register
 * clock set_reg pllcnt1 PL01ST value				// Set pllcnt1 PL01ST register
 * clock set_reg pllcnt1 PL02ST value				// Set pllcnt1 PL02ST register
 * clock set_reg pllcnt1 PL03ST value				// Set pllcnt1 PL03ST register
 * clock set_reg pllcnt1 PL04ST value				// Set pllcnt1 PL04ST register
 * clock set_reg pllcnt1 PL05ST value				// Set pllcnt1 PL05ST register
 * clock set_reg pllcnt1 PL05AST value				// Set pllcnt1 PL05AST register
 * clock set_reg pllcnt1 PL06ST value				// Set pllcnt1 PL06ST register
 * clock set_reg pllcnt1 PL07ST value				// Set pllcnt1 PL07ST register
 * clock set_reg pllcnt1 PL08ST value				// Set pllcnt1 PL08ST register
 * clock set_reg pllcnt1 PL10ST value				// Set pllcnt1 PL10ST register
 * clock set_reg pllcnt1 PL10AST value				// Set pllcnt1 PL10AST register
 * clock set_reg pllcnt1 PL11ST value				// Set pllcnt1 PL11ST register
 * clock set_reg pllcnt1 DPL00ST value				// Set pllcnt1 DPL00ST register
 * clock set_reg pllcnt1 DPL01ST value				// Set pllcnt1 DPL01ST register
 * clock set_reg pllcnt1 DPL02ST value				// Set pllcnt1 DPL02ST register
 * clock set_reg pllcnt1 DPL10ST value				// Set pllcnt1 DPL10ST register
 * clock set_reg pllcnt1 DPL11ST value				// Set pllcnt1 DPL11ST register
 * clock set_reg pllcnt1 DPL12ST value				// Set pllcnt1 DPL12ST register
 * clock set_reg pllcnt2 PL00SEL value				// Set pllcnt2 PL00SEL register
 * clock set_reg pllcnt2 PL01SEL value				// Set pllcnt2 PL01SEL register
 * clock set_reg pllcnt2 PL02SEL value				// Set pllcnt2 PL02SEL register
 * clock set_reg pllcnt2 PL03SEL value				// Set pllcnt2 PL03SEL register
 * clock set_reg pllcnt2 PL04SEL value				// Set pllcnt2 PL04SEL register
 * clock set_reg pllcnt2 PL05SEL value				// Set pllcnt2 PL05SEL register
 * clock set_reg pllcnt2 PL05ASEL value				// Set pllcnt2 PL05ASEL register
 * clock set_reg pllcnt2 PL06SEL value				// Set pllcnt2 PL06SEL register
 * clock set_reg pllcnt2 PL07SEL value				// Set pllcnt2 PL07SEL register
 * clock set_reg pllcnt2 PL08SEL value				// Set pllcnt2 PL08SEL register
 * clock set_reg pllcnt2 PL10SEL value				// Set pllcnt2 PL10SEL register
 * clock set_reg pllcnt2 PL10ASEL value				// Set pllcnt2 PL10ASEL register
 * clock set_reg pllcnt2 PL11SEL value				// Set pllcnt2 PL11SEL register
 * clock set_reg pllcnt2 PL01SSEN value				// Set pllcnt2 PL01SSEN register
 * clock set_reg pllcnt2 PL02SSEN value				// Set pllcnt2 PL02SSEN register
 * clock set_reg pllcnt3 P00POSTDIV value			// Set pllcnt3 P00POSTDIV register
 * clock set_reg pllcnt3 P00PREDIV value			// Set pllcnt3 P00PREDIV register
 * clock set_reg pllcnt3 P00PLLDIV value			// Set pllcnt3 P00PLLDIV register
 * clock set_reg pllcnt4 P00FNUM value				// Set pllcnt4 P00FNUM register
 * clock set_reg pllcnt5 P00FDEN value				// Set pllcnt5 P00FDEN register
 * clock set_reg pllcnt7 P03POSTDIV0 value			// Set pllcnt7 P03POSTDIV0 register
 * clock set_reg pllcnt7 P03POSTDIV1 value			// Set pllcnt7 P03POSTDIV1 register
 * clock set_reg pllcnt7 P03PREDIV value			// Set pllcnt7 P03PREDIV register
 * clock set_reg pllcnt7 P03PLLDIV value			// Set pllcnt7 P03PLLDIV register
 * clock set_reg pllcnt7 P03OC0 value				// Set pllcnt7 P03OC0 register
 * clock set_reg pllcnt7 P03OC1 value				// Set pllcnt7 P03OC1 register
 * clock set_reg pllcnt7 D0XOC value				// Set pllcnt7 D0XOC register
 * clock set_reg pllcnt7 D1XOC value				// Set pllcnt7 D1XOC register
 * clock set_reg pllcnt7 DSEL value					// Set pllcnt7 DSEL register
 * clock set_reg pllcnt8 P04POSTDIV value			// Set pllcnt8 P04POSTDIV register
 * clock set_reg pllcnt8 P04PREDIV value			// Set pllcnt8 P04PREDIV register
 * clock set_reg pllcnt8 P04PLLDIV value			// Set pllcnt8 P04PLLDIV register
 * clock set_reg pllcnt9 P10APLLDIV value			// Set pllcnt9 P10APLLDIV register
 * clock set_reg pllcnt9 P08PLLDIV value			// Set pllcnt9 P08PLLDIV register
 * clock set_reg clkstop1 DSPCK value				// Set clkstop1 DSPCK register
 * clock set_reg clkstop1 DSPAX value				// Set clkstop1 DSPAX register
 * clock set_reg clkstop1 SENCK value				// Set clkstop1 SENCK register
 * clock set_reg clkstop1 SENAX value				// Set clkstop1 SENAX register
 * clock set_reg clkstop1 SENAH value				// Set clkstop1 SENAH register
 * clock set_reg clkstop1 SENAP value				// Set clkstop1 SENAP register
 * clock set_reg clkstop1 GPIOAP value				// Set clkstop1 GPIOAP register
 * clock set_reg clkstop1 AU0CK value				// Set clkstop1 AU0CK register
 * clock set_reg clkstop1 AU2CK value				// Set clkstop1 AU2CK register
 * clock set_reg clkstop1 AU3CK value				// Set clkstop1 AU3CK register
 * clock set_reg clkstop1 AU4CK value				// Set clkstop1 AU4CK register
 * clock set_reg clkstop1 AU5CK value				// Set clkstop1 AU5CK register
 * clock set_reg clkstop1 NETAUCK value				// Set clkstop1 NETAUCK register
 * clock set_reg clkstop1 TEMPCK value				// Set clkstop1 TEMPCK register
 * clock set_reg clkstop2 RCK value					// Set clkstop2 RCK register
 * clock set_reg clkstop2 UHS1CK0 value				// Set clkstop2 UHS1CK0 register
 * clock set_reg clkstop2 UHS1CK1 value				// Set clkstop2 UHS1CK1 register
 * clock set_reg clkstop2 UHS1CK2 value				// Set clkstop2 UHS1CK2 register
 * clock set_reg clkstop2 UHS2CK value				// Set clkstop2 UHS2CK register
 * clock set_reg clkstop2 NFCK value				// Set clkstop2 NFCK register
 * clock set_reg clkstop2 EMMCCK value				// Set clkstop2 EMMCCK register
 * clock set_reg clkstop2 NETSECCK value			// Set clkstop2 NETSECCK register
 * clock set_reg clkstop2 NETRCK value				// Set clkstop2 NETRCK register
 * clock set_reg clkstop2 EXSAX value				// Set clkstop2 EXSAX register
 * clock set_reg clkstop2 SPICK value				// Set clkstop2 SPICK register
 * clock set_reg clkstop2 SLIMB00CK value			// Set clkstop2 SLIMB00CK register
 * clock set_reg clkstop2 SLIMB01CK value			// Set clkstop2 SLIMB01CK register
 * clock set_reg clkstop2 SLIMB10CK value			// Set clkstop2 SLIMB10CK register
 * clock set_reg clkstop2 SLIMB11CK value			// Set clkstop2 SLIMB11CK register
 * clock set_reg clkstop2 PCISUPPCK value			// Set clkstop2 PCISUPPCK register
 * clock set_reg clkstop3 IIPCK value				// Set clkstop3 IIPCK register
 * clock set_reg clkstop3 IIPAP value				// Set clkstop3 IIPAP register
 * clock set_reg clkstop3 IIPAH value				// Set clkstop3 IIPAH register
 * clock set_reg clkstop3 IIPAX value				// Set clkstop3 IIPAX register
 * clock set_reg clkstop3 LCDCK value				// Set clkstop3 LCDCK register
 * clock set_reg clkstop3 HIFCK value				// Set clkstop3 HIFCK register
 * clock set_reg clkstop3 MIFCK value				// Set clkstop3 MIFCK register
 * clock set_reg clkstop3 DISPAP value				// Set clkstop3 DISPAP register
 * clock set_reg clkstop3 DISPAH value				// Set clkstop3 DISPAH register
 * clock set_reg clkstop3 DISPAX value				// Set clkstop3 DISPAX register
 * clock set_reg clkstop3 JPGCK value				// Set clkstop3 JPGCK register
 * clock set_reg clkstop3 JPGAP value				// Set clkstop3 JPGAP register
 * clock set_reg clkstop3 JPGAH value				// Set clkstop3 JPGAH register
 * clock set_reg clkstop3 JPGAX value				// Set clkstop3 JPGAX register
 * clock set_reg clkstop4 GPUCK value				// Set clkstop4 GPUCK register
 * clock set_reg clkstop4 GPUAP value				// Set clkstop4 GPUAP register
 * clock set_reg clkstop4 GPUAH value				// Set clkstop4 GPUAH register
 * clock set_reg clkstop4 GPUAX value				// Set clkstop4 GPUAX register
 * clock set_reg clkstop4 FPT0CK value				// Set clkstop4 FPT0CK register
 * clock set_reg clkstop4 FPT0AP value				// Set clkstop4 FPT0AP register
 * clock set_reg clkstop4 FPT0AH value				// Set clkstop4 FPT0AH register
 * clock set_reg clkstop4 FPT0AX value				// Set clkstop4 FPT0AX register
 * clock set_reg clkstop4 FPT1CK value				// Set clkstop4 FPT1CK register
 * clock set_reg clkstop4 FPT1AP value				// Set clkstop4 FPT1AP register
 * clock set_reg clkstop4 FPT1AH value				// Set clkstop4 FPT1AH register
 * clock set_reg clkstop4 FPT1AX value				// Set clkstop4 FPT1AX register
 * clock set_reg clkstop4 APCK0 value				// Set clkstop4 APCK0 register
 * clock set_reg clkstop4 APCK1 value				// Set clkstop4 APCK1 register
 * clock set_reg clkstop4 APCK2 value				// Set clkstop4 APCK2 register
 * clock set_reg clkstop4 APCK3 value				// Set clkstop4 APCK3 register
 * clock set_reg clkstop5 MICAX0 value				// Set clkstop5 MICAX0 register
 * clock set_reg clkstop5 MICAX1 value				// Set clkstop5 MICAX1 register
 * clock set_reg clkstop5 MICAX2 value				// Set clkstop5 MICAX2 register
 * clock set_reg clkstop5 MICAX3 value				// Set clkstop5 MICAX3 register
 * clock set_reg clkstop5 MICAX4 value				// Set clkstop5 MICAX4 register
 * clock set_reg clkstop5 MICAX5 value				// Set clkstop5 MICAX5 register
 * clock set_reg clkstop5 MICAX6 value				// Set clkstop5 MICAX6 register
 * clock set_reg clkstop5 MICAP0 value				// Set clkstop5 MICAP0 register
 * clock set_reg clkstop5 MICAP1 value				// Set clkstop5 MICAP1 register
 * clock set_reg clkstop5 MICAP2 value				// Set clkstop5 MICAP2 register
 * clock set_reg clkstop5 MICAP3 value				// Set clkstop5 MICAP3 register
 * clock set_reg clkstop5 MICAP4 value				// Set clkstop5 MICAP4 register
 * clock set_reg clkstop5 MICAP5 value				// Set clkstop5 MICAP5 register
 * clock set_reg clkstop5 MICAP6 value				// Set clkstop5 MICAP6 register
 * clock set_reg clkstop6 MICAH1 value				// Set clkstop6 MICAH1 register
 * clock set_reg clkstop6 MICAH2 value				// Set clkstop6 MICAH2 register
 * clock set_reg clkstop6 MICAH3 value				// Set clkstop6 MICAH3 register
 * clock set_reg clkstop6 IMGAX value				// Set clkstop6 IMGAX register
 * clock set_reg clkstop6 IMGAH0 value				// Set clkstop6 IMGAH0 register
 * clock set_reg clkstop6 IMGAH1 value				// Set clkstop6 IMGAH1 register
 * clock set_reg clkstop6 IMGAH3 value				// Set clkstop6 IMGAH3 register
 * clock set_reg clkstop6 IMGAP3 value				// Set clkstop6 IMGAP3 register
 * clock set_reg clkstop6 REGAP value				// Set clkstop6 REGAP register
 * clock set_reg clkstop6 XCHAX value				// Set clkstop6 XCHAX register
 * clock set_reg clkstop6 XCHAP value				// Set clkstop6 XCHAP register
 * clock set_reg clkstop6 ELACK value				// Set clkstop6 ELACK register
 * clock set_reg clkstop6 ELAAX value				// Set clkstop6 ELAAX register
 * clock set_reg clkstop6 ELAAP value				// Set clkstop6 ELAAP register
 * clock set_reg clkstop7 IPUFDCK value				// Set clkstop7 IPUFDCK register
 * clock set_reg clkstop7 IPUVISCK value			// Set clkstop7 IPUVISCK register
 * clock set_reg clkstop7 IPUAX value				// Set clkstop7 IPUAX register
 * clock set_reg clkstop7 IPUAH value				// Set clkstop7 IPUAH register
 * clock set_reg clkstop7 RAWCK value				// Set clkstop7 RAWCK register
 * clock set_reg clkstop7 RAWAX value				// Set clkstop7 RAWAX register
 * clock set_reg clkstop7 RAWAP value				// Set clkstop7 RAWAP register
 * clock set_reg clkstop7 SHDRCK value				// Set clkstop7 SHDRCK register
 * clock set_reg clkstop7 SHDRAX value				// Set clkstop7 SHDRAX register
 * clock set_reg clkstop7 SHDRAH value				// Set clkstop7 SHDRAH register
 * clock set_reg clkstop7 SHDRAP value				// Set clkstop7 SHDRAP register
 * clock set_reg clkstop7 M0CK value				// Set clkstop7 M0CK register
 * clock set_reg clkstop8 RIBCK value				// Set clkstop8 RIBCK register
 * clock set_reg clkstop8 RIBAH value				// Set clkstop8 RIBAH register
 * clock set_reg clkstop8 HEVDFCK value				// Set clkstop8 HEVDFCK register
 * clock set_reg clkstop8 HEVENCCK value			// Set clkstop8 HEVENCCK register
 * clock set_reg clkstop8 HEIPPCK value				// Set clkstop8 HEIPPCK register
 * clock set_reg clkstop8 UMC0HEVCMX2 value			// Set clkstop8 UMC0HEVCMX2 register
 * clock set_reg clkstop8 UMC0HEVCMX4 value			// Set clkstop8 UMC0HEVCMX4 register
 * clock set_reg clkstop8 UMC0RBRMX4 value			// Set clkstop8 UMC0RBRMX4 register
 * clock set_reg clkstop8 UMC1HEVCMX2 value			// Set clkstop8 UMC1HEVCMX2 register
 * clock set_reg clkstop8 UMC1HEVCMX4 value			// Set clkstop8 UMC1HEVCMX4 register
 * clock set_reg clkstop8 UMC1RBRMX4 value			// Set clkstop8 UMC1RBRMX4 register
 * clock set_reg clkstop8 UMC0CMNAX value			// Set clkstop8 UMC0CMNAX register
 * clock set_reg clkstop8 UMC0MX1AX value			// Set clkstop8 UMC0MX1AX register
 * clock set_reg clkstop8 UMC0MX2AX value			// Set clkstop8 UMC0MX2AX register
 * clock set_reg clkstop8 UMC0MX3AX value			// Set clkstop8 UMC0MX3AX register
 * clock set_reg clkstop8 UMC0MX4AX value			// Set clkstop8 UMC0MX4AX register
 * clock set_reg clkstop9 UMC0MX5AX value			// Set clkstop9 UMC0MX5AX register
 * clock set_reg clkstop9 UMC1CMNAX value			// Set clkstop9 UMC1CMNAX register
 * clock set_reg clkstop9 UMC1MX1AX value			// Set clkstop9 UMC1MX1AX register
 * clock set_reg clkstop9 UMC1MX2AX value			// Set clkstop9 UMC1MX2AX register
 * clock set_reg clkstop9 UMC1MX3AX value			// Set clkstop9 UMC1MX3AX register
 * clock set_reg clkstop9 UMC1MX4AX value			// Set clkstop9 UMC1MX4AX register
 * clock set_reg clkstop9 UMC1MX5AX value			// Set clkstop9 UMC1MX5AX register
 * clock set_reg clkstop9 UMC0MX0AX3 value			// Set clkstop9 UMC0MX0AX3 register
 * clock set_reg clkstop9 UMC0MX6AX4 value			// Set clkstop9 UMC0MX6AX4 register
 * clock set_reg clkstop9 UMC0HEVCAX4 value			// Set clkstop9 UMC0HEVCAX4 register
 * clock set_reg clkstop9 UMC1MX0AX3 value			// Set clkstop9 UMC1MX0AX3 register
 * clock set_reg clkstop9 UMC1MX6AX4 value			// Set clkstop9 UMC1MX6AX4 register
 * clock set_reg clkstop9 UMC1HEVCAX4 value			// Set clkstop9 UMC1HEVCAX4 register
 * clock set_reg clkstop9 UMC0AP value				// Set clkstop9 UMC0AP register
 * clock set_reg clkstop9 UMC1AP value				// Set clkstop9 UMC1AP register
 * clock set_reg clkstop10 SRO1CK value				// Set clkstop10 SRO1CK register
 * clock set_reg clkstop10 SRO1CK_2 value			// Set clkstop10 SRO1CK_2 register
 * clock set_reg clkstop10 SRO1AX value				// Set clkstop10 SRO1AX register
 * clock set_reg clkstop10 SRO1AH value				// Set clkstop10 SRO1AH register
 * clock set_reg clkstop10 SRO1AP value				// Set clkstop10 SRO1AP register
 * clock set_reg clkstop10 B2B1CK value				// Set clkstop10 B2B1CK register
 * clock set_reg clkstop10 B2B1AX value				// Set clkstop10 B2B1AX register
 * clock set_reg clkstop10 B2B1AH value				// Set clkstop10 B2B1AH register
 * clock set_reg clkstop10 B2B1AP value				// Set clkstop10 B2B1AP register
 * clock set_reg clkstop10 LTMRBK1CK value			// Set clkstop10 LTMRBK1CK register
 * clock set_reg clkstop10 B2R1CK value				// Set clkstop10 B2R1CK register
 * clock set_reg clkstop10 B2R1AX value				// Set clkstop10 B2R1AX register
 * clock set_reg clkstop10 B2R1AH value				// Set clkstop10 B2R1AH register
 * clock set_reg clkstop10 B2R1AP value				// Set clkstop10 B2R1AP register
 * clock set_reg clkstop11 LTM1CK value				// Set clkstop11 LTM1CK register
 * clock set_reg clkstop11 LTM1AX value				// Set clkstop11 LTM1AX register
 * clock set_reg clkstop11 LTM1AH value				// Set clkstop11 LTM1AH register
 * clock set_reg clkstop11 LTM1AP value				// Set clkstop11 LTM1AP register
 * clock set_reg clkstop11 R2Y1CK value				// Set clkstop11 R2Y1CK register
 * clock set_reg clkstop11 R2Y1AX value				// Set clkstop11 R2Y1AX register
 * clock set_reg clkstop11 R2Y1AH value				// Set clkstop11 R2Y1AH register
 * clock set_reg clkstop11 R2Y1AP value				// Set clkstop11 R2Y1AP register
 * clock set_reg clkstop11 CNR1CK value				// Set clkstop11 CNR1CK register
 * clock set_reg clkstop11 CNR1AX value				// Set clkstop11 CNR1AX register
 * clock set_reg clkstop11 CNR1AP value				// Set clkstop11 CNR1AP register
 * clock set_reg clkstop12 SRO2CK value				// Set clkstop12 SRO2CK register
 * clock set_reg clkstop12 SRO2CK_2 value			// Set clkstop12 SRO2CK_2 register
 * clock set_reg clkstop12 SRO2AX value				// Set clkstop12 SRO2AX register
 * clock set_reg clkstop12 SRO2AH value				// Set clkstop12 SRO2AH register
 * clock set_reg clkstop12 SRO2AP value				// Set clkstop12 SRO2AP register
 * clock set_reg clkstop12 B2B2CK value				// Set clkstop12 B2B2CK register
 * clock set_reg clkstop12 B2B2AX value				// Set clkstop12 B2B2AX register
 * clock set_reg clkstop12 B2B2AH value				// Set clkstop12 B2B2AH register
 * clock set_reg clkstop12 B2B2AP value				// Set clkstop12 B2B2AP register
 * clock set_reg clkstop12 LTMRBK2CK value			// Set clkstop12 LTMRBK2CK register
 * clock set_reg clkstop12 B2R2CK value				// Set clkstop12 B2R2CK register
 * clock set_reg clkstop12 B2R2AX value				// Set clkstop12 B2R2AX register
 * clock set_reg clkstop12 B2R2AH value				// Set clkstop12 B2R2AH register
 * clock set_reg clkstop12 B2R2AP value				// Set clkstop12 B2R2AP register
 * clock set_reg clkstop13 LTM2CK value				// Set clkstop13 LTM2CK register
 * clock set_reg clkstop13 LTM2AX value				// Set clkstop13 LTM2AX register
 * clock set_reg clkstop13 LTM2AH value				// Set clkstop13 LTM2AH register
 * clock set_reg clkstop13 LTM2AP value				// Set clkstop13 LTM2AP register
 * clock set_reg clkstop13 R2Y2CK value				// Set clkstop13 R2Y2CK register
 * clock set_reg clkstop13 R2Y2AX value				// Set clkstop13 R2Y2AX register
 * clock set_reg clkstop13 R2Y2AH value				// Set clkstop13 R2Y2AH register
 * clock set_reg clkstop13 R2Y2AP value				// Set clkstop13 R2Y2AP register
 * clock set_reg clkstop13 CNR2CK value				// Set clkstop13 CNR2CK register
 * clock set_reg clkstop13 CNR2AX value				// Set clkstop13 CNR2AX register
 * clock set_reg clkstop13 CNR2AP value				// Set clkstop13 CNR2AP register
 * clock set_reg clkstop14 STATAX value				// Set clkstop14 STATAX register
 * clock set_reg clkstop14 STATAH value				// Set clkstop14 STATAH register
 * clock set_reg clkstop14 STATAP value				// Set clkstop14 STATAP register
 * clock set_reg clkstop14 PERIAH value				// Set clkstop14 PERIAH register
 * clock set_reg clkstop14 PERIAP value				// Set clkstop14 PERIAP register
 * clock set_reg clkstop14 SENMSKCK value			// Set clkstop14 SENMSKCK register
 * clock set_reg clkstop14 PDMCK value				// Set clkstop14 PDMCK register
 * clock set_reg clkstop14 GYROCK value				// Set clkstop14 GYROCK register
 * clock get_reg clksel1							// Get clksel1 register
 * clock get_reg clksel2							// Get clksel2 register
 * clock get_reg clksel3							// Get clksel3 register
 * clock get_reg clksel4							// Get clksel4 register
 * clock get_reg clksel5							// Get clksel5 register
 * clock get_reg clksel6							// Get clksel6 register
 * clock get_reg clksel7							// Get clksel7 register
 * clock get_reg clksel8							// Get clksel8 register
 * clock get_reg clksel9							// Get clksel9 register
 * clock get_reg clksel10							// Get clksel10 register
 * clock get_reg clksel11							// Get clksel11 register
 * clock get_reg clksel12							// Get clksel12 register
 * clock get_reg pllcnt1							// Get pllcnt1 register
 * clock get_reg pllcnt2							// Get pllcnt2 register
 * clock get_reg pllcnt3							// Get pllcnt3 register
 * clock get_reg pllcnt4							// Get pllcnt4 register
 * clock get_reg pllcnt5							// Get pllcnt5 register
 * clock get_reg pllcnt7							// Get pllcnt7 register
 * clock get_reg pllcnt8							// Get pllcnt8 register
 * clock get_reg pllcnt9							// Get pllcnt9 register
 * clock get_reg clkstop1							// Get clkstop1 register
 * clock get_reg clkstop2							// Get clkstop2 register
 * clock get_reg clkstop3							// Get clkstop3 register
 * clock get_reg clkstop4							// Get clkstop4 register
 * clock get_reg clkstop5							// Get clkstop5 register
 * clock get_reg clkstop6							// Get clkstop6 register
 * clock get_reg clkstop7							// Get clkstop7 register
 * clock get_reg clkstop8							// Get clkstop8 register
 * clock get_reg clkstop9							// Get clkstop9 register
 * clock get_reg clkstop10							// Get clkstop10 register
 * clock get_reg clkstop11							// Get clkstop11 register
 * clock get_reg clkstop12							// Get clkstop12 register
 * clock get_reg clkstop13							// Get clkstop13 register
 * clock get_reg clkstop14							// Get clkstop14 register
 * clock get_clock EMMCCLK value					// Get EMMCCLK register
 * clock get_clock NFCLK value						// Get NFCLK register
 * clock get_clock UHS2CLK value					// Get UHS2CLK register
 * clock get_clock UHS1CLK2 value					// Get UHS1CLK2 register
 * clock get_clock UHS1CLK1 value					// Get UHS1CLK1 register
 * clock get_clock UHS1CLK0 value					// Get UHS1CLK0 register
 * clock get_clock RCLK value						// Get RCLK register
 * clock get_clock RAWCLK value						// Get RAWCLK register
 * clock get_clock HIFCLK value						// Get HIFCLK register
 * clock get_clock MIFCLK value						// Get MIFCLK register
 * clock get_clock IPUCLK value						// Get IPUCLK register
 * clock get_clock IPUTMECLK value					// Get IPUTMECLK register
 * clock get_clock GPUCLK value						// Get GPUCLK register
 * clock get_clock JPGCLK value						// Get JPGCLK register
 * clock get_clock ELACLK value						// Get ELACLK register
 * clock get_clock SENCLK value						// Get SENCLK register
 * clock get_clock SENMSKCLK value					// Get SENMSKCLK register
 * clock get_clock IIPCLK value						// Get IIPCLK register
 * clock get_clock PASCLK value						// Get PASCLK register
 * clock get_clock IPPCLK value						// Get IPPCLK register
 * clock get_clock HEVENCCLK value					// Get HEVENCCLK register
 * clock get_clock VDFCLK value						// Get VDFCLK register
 * clock get_clock RIBCLK value						// Get RIBCLK register
 * clock get_clock SRO1CLK_2 value					// Get SRO1CLK_2 register
 * clock get_clock R2Y1CLK value					// Get R2Y1CLK register
 * clock get_clock SRO1CLK value					// Get SRO1CLK register
 * clock get_clock LTM1CLK value					// Get LTM1CLK register
 * clock get_clock B2R1CLK value					// Get B2R1CLK register
 * clock get_clock CNR1CLK value					// Get CNR1CLK register
 * clock get_clock B2B1CLK value					// Get B2B1CLK register
 * clock get_clock LTMRBK1CLK value					// Get LTMRBK1CLK register
 * clock get_clock SRO2CLK_2 value					// Get SRO2CLK_2 register
 * clock get_clock R2Y2CLK value					// Get R2Y2CLK register
 * clock get_clock LTM2CLK value					// Get LTM2CLK register
 * clock get_clock B2R2CLK value					// Get B2R2CLK register
 * clock get_clock CNR2CLK value					// Get CNR2CLK register
 * clock get_clock SRO2CLK value					// Get SRO2CLK register
 * clock get_clock B2B2CLK value					// Get B2B2CLK register
 * clock get_clock LTMRBK2CLK value					// Get LTMRBK2CLK register
 * clock get_clock APCLK value						// Get APCLK register
 * clock get_clock AUCLK value						// Get AUCLK register
 * clock get_clock SPICLK value						// Get SPICLK register
 * clock get_clock DSPCLK value						// Get DSPCLK register
 * clock get_clock ACLK value						// Get ACLK register
 * clock get_clock ACLKEXS value					// Get ACLKEXS register
 * clock get_clock HCLKBMH value					// Get HCLKBMH register
 * clock get_clock HCLK value						// Get HCLK register
 * clock get_clock PCLK value						// Get PCLK register
 * clock get_clock MCLK400 value					// Get MCLK400 register
 * clock get_clock MCLK200 value					// Get MCLK200 register
 * clock get_clock ACLK400 value					// Get ACLK400 register
 * clock get_clock ACLK300 value					// Get ACLK300 register
 * clock get_clock GYROCLK value					// Get GYROCLK register
 * clock get_clock SHDRCLK value					// Get SHDRCLK register
 * clock get_clock FPT0CLK value					// Get FPT0CLK register
 * clock get_clock FPT1CLK value					// Get FPT1CLK register
 * clock start clkstop1 count value					// Start clkstop1 register
 * clock start clkstop2 count value					// Start clkstop2 register
 * clock start clkstop3 count value					// Start clkstop3 register
 * clock start clkstop4 count value					// Start clkstop4 register
 * clock start clkstop5 count value					// Start clkstop5 register
 * clock start clkstop6 count value					// Start clkstop6 register
 * clock start clkstop7 count value					// Start clkstop7 register
 * clock start clkstop8 count value					// Start clkstop8 register
 * clock start clkstop9 count value					// Start clkstop9 register
 * clock start clkstop10 count value				// Start clkstop10 register
 * clock start clkstop11 count value				// Start clkstop11 register
 * clock start clkstop12 count value				// Start clkstop12 register
 * clock start clkstop13 count value				// Start clkstop13 register
 * clock start clkstop14 count value				// Start clkstop14 register
 * clock stop clkstop1 count value					// Stop clkstop1 register
 * clock stop clkstop2 count value					// Stop clkstop2 register
 * clock stop clkstop3 count value					// Stop clkstop3 register
 * clock stop clkstop4 count value					// Stop clkstop4 register
 * clock stop clkstop5 count value					// Stop clkstop5 register
 * clock stop clkstop6 count value					// Stop clkstop6 register
 * clock stop clkstop7 count value					// Stop clkstop7 register
 * clock stop clkstop8 count value					// Stop clkstop8 register
 * clock stop clkstop9 count value					// Stop clkstop9 register
 * clock stop clkstop10 count value					// Stop clkstop10 register
 * clock stop clkstop11 count value					// Stop clkstop11 register
 * clock stop clkstop12 count value					// Stop clkstop12 register
 * clock stop clkstop13 count value					// Stop clkstop13 register
 * clock stop clkstop14 count value					// Stop clkstop14 register
 * 
 * P1  P2    P3
 * pll pll00 start								// Start pll00 PLL clock
 * pll pll00 stop								// Stop	 pll00 PLL clock
 * pll pll01 start								// Start pll01 PLL clock
 * pll pll01 stop								// Stop	 pll01 PLL clock
 * pll pll02 start								// Start pll02 PLL clock
 * pll pll02 stop								// Stop	 pll02 PLL clock
 * pll pll03 start								// Start pll03 PLL clock
 * pll pll03 stop								// Stop	 pll03 PLL clock
 * pll pll04 start								// Start pll04 PLL clock
 * pll pll04 stop								// Stop	 pll04 PLL clock
 * pll pll05 start								// Start pll05 PLL clock
 * pll pll05 stop								// Stop	 pll05 PLL clock
 * pll pll05a start								// Start pll05a PLL clock
 * pll pll05a stop								// Stop	 pll05a PLL clock
 * pll pll06 start								// Start pll06 PLL clock
 * pll pll06 stop								// Stop	 pll06 PLL clock
 * pll pll07 start								// Start pll07 PLL clock
 * pll pll07 stop								// Stop	 pll07 PLL clock
 * pll pll08 start								// Start pll08 PLL clock
 * pll pll08 stop								// Stop	 pll08 PLL clock
 * pll pll10 start								// Start pll10 PLL clock
 * pll pll10 stop								// Stop	 pll10 PLL clock
 * pll pll10a start								// Start pll10a PLL clock
 * pll pll10a stop								// Stop	 pll10a PLL clock
 * pll pll11 start								// Start pll11 PLL clock
 * pll pll11 stop								// Stop	 pll11 PLL clock
 * pll ddr00 start								// Start ddr00 PLL clock
 * pll ddr00 stop								// Stop	 ddr00 PLL clock
 * pll ddr01 start								// Start ddr01 PLL clock
 * pll ddr01 stop								// Stop	 ddr01 PLL clock
 * pll ddr02 start								// Start ddr02 PLL clock
 * pll ddr02 stop								// Stop	 ddr02 PLL clock
 * pll ddr10 start								// Start ddr10 PLL clock
 * pll ddr10 stop								// Stop	 ddr10 PLL clock
 * pll ddr11 start								// Start ddr11 PLL clock
 * pll ddr11 stop								// Stop	 ddr11 PLL clock
 * pll ddr12 start								// Start ddr12 PLL clock
 * pll ddr12 stop								// Stop	 ddr12 PLL clock
 * pll pl00sel pll_select								// Set pl00sel pll select register
 * pll pl01sel pll_select								// Set pl01sel pll select register
 * pll pl02sel pll_select								// Set pl02sel pll select register
 * pll pl03sel pll_select								// Set pl03sel pll select register
 * pll pl04sel pll_select								// Set pl04sel pll select register
 * pll pl05sel pll_select								// Set pl05sel pll select register
 * pll pl05asel pll_select								// Set pl05asel pll select register
 * pll pl06sel pll_select								// Set pl06sel pll select register
 * pll pl07sel pll_select								// Set pl07sel pll select register
 * pll pl08sel pll_select								// Set pl08sel pll select register
 * pll pl10sel pll_select								// Set pl10sel pll select register
 * pll pl10asel pll_select								// Set pl10asel pll select register
 * pll pl11sel pll_select								// Set pl11sel pll select register
 * pll pl01ssen pll_select								// Set pl01ssen pll select register
 * pll pl02ssen pll_select								// Set pl02ssen pll select register
 * pll p00postdiv pll_select								// Set p00postdiv pll select register
 * pll p00prediv pll_select								// Set p00prediv pll select register
 * pll p00plldiv pll_select								// Set p00plldiv pll select register
 * pll p00fnum pll_select								// Set p00fnum pll select register
 * pll p00fden pll_select								// Set p00fden pll select register
 * pll p03postdiv0 pll_select								// Set p03postdiv0 pll select register
 * pll p03postdiv1 pll_select								// Set p03postdiv1 pll select register
 * pll p03prediv pll_select								// Set p03prediv pll select register
 * pll p03plldiv pll_select								// Set p03plldiv pll select register
 * pll p03oc0 pll_select								// Set p03oc0 pll select register
 * pll p03oc1 pll_select								// Set p03oc1 pll select register
 * pll d0xoc pll_select								// Set d0xoc pll select register
 * pll d1xoc pll_select								// Set d1xoc pll select register
 * pll dodvsel pll_select								// Set dodvsel pll select register
 * pll p04postdiv pll_select								// Set p04postdiv pll select register
 * pll p04prediv pll_select								// Set p04prediv pll select register
 * pll p04plldiv pll_select								// Set p04plldiv pll select register
 * pll p10aplldiv pll_select								// Set p10aplldiv pll select register
 * pll p08plldiv pll_select								// Set p08plldiv pll select register
 * pll err										// API parameter error check

 *
 * P1    P2  P3
 * reset get									// Get CRRSM/CRRRS register
 * reset set CRRSM_WDRST						// Set Watchdog Reset Clear in CRRSM register
 * reset set CRRSM_SWRST						// Set SRREQ Reset Clear in CRRSM register
 * reset set CRRSM_SRST							// Set TSRSTX Terminal Reset Clear in CRRSM register
 * reset set CRRSM_PORST						// Set RSTX Terminal Reset Clear in CRRSM register
 * reset set CRRSM_CPU0_SWRST							// Set CPU0_SR Bit Reset Clear in CRRSM register
 * reset set CRRSM_CPU1_SWRST							// Set CPU1_SR Bit Reset Clear in CRRSM register
 * reset set CRRSM_CPU2_SWRST							// Set CPU2_SR Bit Reset Clear in CRRSM register
 * reset set CRRSM_CPU3_SWRST							// Set CPU3_SR Bit Reset Clear in CRRSM register
 *
 * P1   P2  P3   P4   P5
 * gpio set regi port value						// Set EPCR/PDR/DDR/PUDER/PUDCR register
 * gpio get regi port							// Get EPCR/PDR/DDR/PUDER/PUDCR register
 * gpio test								// Init EPCR/PDR/DDR/PUDER/PUDCR register (0 crear)
 * gpio err										// API parameter error check
 *
 *	 regi:
 *		"EPCR"  :External Port Control Register
 *		"DDR"   :Port Direction Register
 *		"PDR"   :Port Data Register
 *		"PUDER" :Pull-Up/Down Enable Register
 *		"PUDCR" :Pull-Up/Down Control Register
 *
 *	 port:
 *		"Pxx"   ex)"P37"
 */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

void Ct_Dd_Top_Main(int argc, char** argv)
{
	INT32 result;

	if( 0 == strcmp( argv[1], "lsi" ) ) {
		if( 0 == strcmp( argv[2], "get_ctrl" ) ) {
			if( 0 == strcmp( argv[3], "persel1" ) ) {
				ct_get_ctrl_persel1();
			}
			else if( 0 == strcmp( argv[3], "persel2" ) ) {
				ct_get_ctrl_persel2();
			}
			else if( 0 == strcmp( argv[3], "persel3" ) ) {
				ct_get_ctrl_persel3();
			}
			else if( 0 == strcmp( argv[3], "persel4" ) ) {
				ct_get_ctrl_persel4();
			}
			else if( 0 == strcmp( argv[3], "msel" ) ) {
				ct_get_ctrl_msel();
			}
			else if( 0 == strcmp( argv[3], "dbcnt1" ) ) {
				ct_get_ctrl_dbcnt1();
			}
			else if( 0 == strcmp( argv[3], "dbcnt2" ) ) {
				ct_get_ctrl_dbcnt2();
			}
			else if( 0 == strcmp( argv[3], "dbcnt3" ) ) {
				ct_get_ctrl_dbcnt3();
			}
			else if( 0 == strcmp( argv[3], "dbcnt4" ) ) {
				ct_get_ctrl_dbcnt4();
			}
			else if( 0 == strcmp( argv[3], "dbcnt5" ) ) {
				ct_get_ctrl_dbcnt5();
			}
			else if( 0 == strcmp( argv[3], "pudcnt" ) ) {
				ct_get_ctrl_pudcnt();
			}
		}
		else if( 0 == strcmp( argv[2], "set_ctrl" ) ) {
			if( 0 == strcmp( argv[3], "persel1" ) ) {
				ct_set_ctrl_persel1(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "persel2" ) ) {
				ct_set_ctrl_persel2(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "persel3" ) ) {
				ct_set_ctrl_persel3(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "persel4" ) ) {
				ct_set_ctrl_persel4(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "msel" ) ) {
				ct_set_ctrl_msel(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "dbcnt1" ) ) {
				ct_set_ctrl_dbcnt1(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "dbcnt2" ) ) {
				ct_set_ctrl_dbcnt2(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "dbcnt3" ) ) {
				ct_set_ctrl_dbcnt3(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "dbcnt4" ) ) {
				ct_set_ctrl_dbcnt4(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "dbcnt5" ) ) {
				ct_set_ctrl_dbcnt5(argc, argv);
			}
			else if( 0 == strcmp( argv[3], "pudcnt" ) ) {
				ct_set_ctrl_pudcnt(argc, argv);
			}
		}
		else {
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
	else if( 0 == strcmp( argv[1], "clock" ) ) {
		if( 0 == strcmp( argv[2], "start" ) ) {
			ct_start_clock(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "stop" ) ) {
			ct_stop_clock( argc, argv );
		}
		else if ( 0 == strcmp( argv[2], "get_clock" ) ) {
			ct_get_clock_frequency( argc, argv );
		}
		else if ( 0 == strcmp( argv[2], "get_reg" ) ) {
			if( 0 == strcmp( argv[3], "clksel1" ) ) {
				ct_get_clock_clksel1();
			}
			else if( 0 == strcmp( argv[3], "clksel2" ) ) {
				ct_get_clock_clksel2();
			}
			else if( 0 == strcmp( argv[3], "clksel3" ) ) {
				ct_get_clock_clksel3();
			}
			else if( 0 == strcmp( argv[3], "clksel4" ) ) {
				ct_get_clock_clksel4();
			}
			else if( 0 == strcmp( argv[3], "clksel5" ) ) {
				ct_get_clock_clksel5();
			}
			else if( 0 == strcmp( argv[3], "clksel6" ) ) {
				ct_get_clock_clksel6();
			}
			else if( 0 == strcmp( argv[3], "clksel7" ) ) {
				ct_get_clock_clksel7();
			}
			else if( 0 == strcmp( argv[3], "clksel8" ) ) {
				ct_get_clock_clksel8();
			}
			else if( 0 == strcmp( argv[3], "clksel9" ) ) {
				ct_get_clock_clksel9();
			}
			else if( 0 == strcmp( argv[3], "clksel10" ) ) {
				ct_get_clock_clksel10();
			}
			else if( 0 == strcmp( argv[3], "clksel11" ) ) {
				ct_get_clock_clksel11();
			}
			else if( 0 == strcmp( argv[3], "clksel12" ) ) {
				ct_get_clock_clksel12();
			}
			else if( 0 == strcmp( argv[3], "pllcnt1" ) ) {
				ct_get_clock_pllcnt1();
			}
			else if( 0 == strcmp( argv[3], "pllcnt2" ) ) {
				ct_get_clock_pllcnt2();
			}
			else if( 0 == strcmp( argv[3], "pllcnt3" ) ) {
				ct_get_clock_pllcnt3();
			}
			else if( 0 == strcmp( argv[3], "pllcnt4" ) ) {
				ct_get_clock_pllcnt4();
			}
			else if( 0 == strcmp( argv[3], "pllcnt5" ) ) {
				ct_get_clock_pllcnt5();
			}
			else if( 0 == strcmp( argv[3], "pllcnt6" ) ) {
				ct_get_clock_pllcnt6();
			}
			else if( 0 == strcmp( argv[3], "pllcnt7" ) ) {
				ct_get_clock_pllcnt7();
			}
			else if( 0 == strcmp( argv[3], "pllcnt8" ) ) {
				ct_get_clock_pllcnt8();
			}
			else if( 0 == strcmp( argv[3], "pllcnt9" ) ) {
				ct_get_clock_pllcnt9();
			}
			else if( 0 == strcmp( argv[3], "clkstop1" ) ) {
				ct_get_clock_clkstop1();
			}
			else if( 0 == strcmp( argv[3], "clkstop2" ) ) {
				ct_get_clock_clkstop2();
			}
			else if( 0 == strcmp( argv[3], "clkstop3" ) ) {
				ct_get_clock_clkstop3();
			}
			else if( 0 == strcmp( argv[3], "clkstop4" ) ) {
				ct_get_clock_clkstop4();
			}
			else if( 0 == strcmp( argv[3], "clkstop5" ) ) {
				ct_get_clock_clkstop5();
			}
			else if( 0 == strcmp( argv[3], "clkstop6" ) ) {
				ct_get_clock_clkstop6();
			}
			else if( 0 == strcmp( argv[3], "clkstop7" ) ) {
				ct_get_clock_clkstop7();
			}
			else if( 0 == strcmp( argv[3], "clkstop8" ) ) {
				ct_get_clock_clkstop8();
			}
			else if( 0 == strcmp( argv[3], "clkstop9" ) ) {
				ct_get_clock_clkstop9();
			}
			else if( 0 == strcmp( argv[3], "clkstop10" ) ) {
				ct_get_clock_clkstop10();
			}
			else if( 0 == strcmp( argv[3], "clkstop11" ) ) {
				ct_get_clock_clkstop11();
			}
			else if( 0 == strcmp( argv[3], "clkstop12" ) ) {
				ct_get_clock_clkstop12();
			}
			else if( 0 == strcmp( argv[3], "clkstop13" ) ) {
				ct_get_clock_clkstop13();
			}
			else if( 0 == strcmp( argv[3], "clkstop14" ) ) {
				ct_get_clock_clkstop14();
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if ( 0 == strcmp( argv[2], "set_reg" ) ) {
			if( 0 == strcmp( argv[3], "clksel1" ) ) {
				ct_set_clock_clksel1( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel2" ) ) {
				ct_set_clock_clksel2( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel3" ) ) {
				ct_set_clock_clksel3( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel4" ) ) {
				ct_set_clock_clksel4( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel5" ) ) {
				ct_set_clock_clksel5( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel6" ) ) {
				ct_set_clock_clksel6( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel7" ) ) {
				ct_set_clock_clksel7( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel8" ) ) {
				ct_set_clock_clksel8( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel9" ) ) {
				ct_set_clock_clksel9( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel10" ) ) {
				ct_set_clock_clksel10( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel11" ) ) {
				ct_set_clock_clksel11( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clksel12" ) ) {
				ct_set_clock_clksel12( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt1" ) ) {
				ct_set_clock_pllcnt1( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt2" ) ) {
				ct_set_clock_pllcnt2( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt3" ) ) {
				ct_set_clock_pllcnt3( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt4" ) ) {
				ct_set_clock_pllcnt4( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt5" ) ) {
				ct_set_clock_pllcnt5( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt6" ) ) {
				ct_set_clock_pllcnt6( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt7" ) ) {
				ct_set_clock_pllcnt7( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt8" ) ) {
				ct_set_clock_pllcnt8( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "pllcnt9" ) ) {
				ct_set_clock_pllcnt9( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop1" ) ) {
				ct_set_clock_clkstop1( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop2" ) ) {
				ct_set_clock_clkstop2( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop3" ) ) {
				ct_set_clock_clkstop3( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop4" ) ) {
				ct_set_clock_clkstop4( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop5" ) ) {
				ct_set_clock_clkstop5( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop6" ) ) {
				ct_set_clock_clkstop6( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop7" ) ) {
				ct_set_clock_clkstop7( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop8" ) ) {
				ct_set_clock_clkstop8( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop9" ) ) {
				ct_set_clock_clkstop9( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop10" ) ) {
				ct_set_clock_clkstop10( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop11" ) ) {
				ct_set_clock_clkstop11( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop12" ) ) {
				ct_set_clock_clkstop12( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop13" ) ) {
				ct_set_clock_clkstop13( argc, argv );
			}
			else if( 0 == strcmp( argv[3], "clkstop14" ) ) {
				ct_set_clock_clkstop14( argc, argv );
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else {
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}

	else if( 0 == strcmp( argv[1], "pll" ) ) {
		if( 0 == strcmp( argv[2], "pll00" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll00();
				Ddim_Print(("PLL00 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll00();
				Ddim_Print(("PLL00 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll01" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll01();
				Ddim_Print(("PLL01 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll01();
				Ddim_Print(("PLL01 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll02" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll02();
				Ddim_Print(("PLL02 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll02();
				Ddim_Print(("PLL02 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll03" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll03();
				Ddim_Print(("PLL03 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll03();
				Ddim_Print(("PLL03 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll04" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll04();
				Ddim_Print(("PLL04 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll04();
				Ddim_Print(("PLL04 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll05" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll05();
				Ddim_Print(("PLL05 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll05();
				Ddim_Print(("PLL05 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll05a" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll05A();
				Ddim_Print(("PLL05A START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll05A();
				Ddim_Print(("PLL05A STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll06" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll06();
				Ddim_Print(("PLL06 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll06();
				Ddim_Print(("PLL06 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll07" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll07();
				Ddim_Print(("PLL07 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll07();
				Ddim_Print(("PLL07 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll08" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll08();
				Ddim_Print(("PLL08 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll08();
				Ddim_Print(("PLL08 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll10" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll10();
				Ddim_Print(("PLL10 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll10();
				Ddim_Print(("PLL10 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll10a" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll10A();
				Ddim_Print(("PLL10A START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll10A();
				Ddim_Print(("PLL10A STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pll11" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Pll11();
				Ddim_Print(("PLL11 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Pll11();
				Ddim_Print(("PLL11 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "ddr00" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Ddr_Pll00();
				Ddim_Print(("DDR PLL00 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Ddr_Pll00();
				Ddim_Print(("DDR PLL00 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "ddr01" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Ddr_Pll01();
				Ddim_Print(("DDR PLL01 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Ddr_Pll01();
				Ddim_Print(("DDR PLL01 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "ddr02" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Ddr_Pll02();
				Ddim_Print(("DDR PLL02 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Ddr_Pll02();
				Ddim_Print(("DDR PLL02 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "ddr10" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Ddr_Pll10();
				Ddim_Print(("DDR PLL10 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Ddr_Pll10();
				Ddim_Print(("DDR PLL10 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "ddr11" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Ddr_Pll11();
				Ddim_Print(("DDR PLL11 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Ddr_Pll11();
				Ddim_Print(("DDR PLL11 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "ddr12" ) ) {
			if ( 0 == strcmp( argv[3], "start" ) ) {
				result = ct_start_Ddr_Pll12();
				Ddim_Print(("DDR PLL12 START = %d\n", result));
			}
			else if ( 0 == strcmp( argv[3], "stop" )) {
				result = ct_stop_Ddr_Pll12();
				Ddim_Print(("DDR PLL12 STOP\n"));
			}
			else {
				Ddim_Print(("please check 3rd parameter!!\n"));
			}
		}
		else if( 0 == strcmp( argv[2], "pl00sel" ) ) {
			ct_set_pllcnt2_pl00sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl01sel" ) ) {
			ct_set_pllcnt2_pl01sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl02sel" ) ) {
			ct_set_pllcnt2_pl02sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl03sel" ) ) {
			ct_set_pllcnt2_pl03sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl04sel" ) ) {
			ct_set_pllcnt2_pl04sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl05sel" ) ) {
			ct_set_pllcnt2_pl05sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl05asel" ) ) {
			ct_set_pllcnt2_pl05asel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl06sel" ) ) {
			ct_set_pllcnt2_pl06sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl07sel" ) ) {
			ct_set_pllcnt2_pl07sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl08sel" ) ) {
			ct_set_pllcnt2_pl08sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl10sel" ) ) {
			ct_set_pllcnt2_pl10sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl10asel" ) ) {
			ct_set_pllcnt2_pl10asel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl11sel" ) ) {
			ct_set_pllcnt2_pl11sel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl01ssen" ) ) {
			ct_set_pllcnt2_pl01ssen(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "pl02ssen" ) ) {
			ct_set_pllcnt2_pl02ssen(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p00postdiv" ) ) {
			ct_set_pllcnt3_p00postdiv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p00prediv" ) ) {
			ct_set_pllcnt3_p00prediv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p00plldiv" ) ) {
			ct_set_pllcnt3_p00plldiv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p00fnum" ) ) {
			ct_set_pllcnt4_p00fnum(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p00fden" ) ) {
			ct_set_pllcnt5_p00fden(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p03postdiv0" ) ) {
			ct_set_pllcnt7_p03postdiv0(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p03postdiv1" ) ) {
			ct_set_pllcnt7_p03postdiv1(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p03prediv" ) ) {
			ct_set_pllcnt7_p03prediv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p03plldiv" ) ) {
			ct_set_pllcnt7_p03plldiv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p03oc0" ) ) {
			ct_set_pllcnt7_p03oc0(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p03oc1" ) ) {
			ct_set_pllcnt7_p03oc1(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "d0xoc" ) ) {
			ct_set_pllcnt7_d0xoc(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "d1xoc" ) ) {
			ct_set_pllcnt7_d1xoc(argc, argv);
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if( 0 == strcmp( argv[2], "dplx8" ) ) {
			ct_set_pllcnt7_dplx8(argc, argv);
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else if( 0 == strcmp( argv[2], "dsel" ) ) {
			ct_set_pllcnt7_dsel(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p04postdiv" ) ) {
			ct_set_pllcnt8_p04postdiv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p04prediv" ) ) {
			ct_set_pllcnt8_p04prediv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p04plldiv" ) ) {
			ct_set_pllcnt8_p04plldiv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p10aplldiv" ) ) {
			ct_set_pllcnt9_p10aplldiv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p08plldiv" ) ) {
			ct_set_pllcnt9_p08plldiv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "p05chg" ) ) {
			ct_set_pllcnt9_p05chg(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "plldiv" ) ) {
			ct_set_pllfreq1_plldiv(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "odivch0num" ) ) {
			ct_set_odivch0_odivch0num(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "cmden" ) ) {
			ct_set_cmden_cmden(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "err" ) ) {
			ct_pll_err();
		}
		else {
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
	else if( 0 == strcmp( argv[1], "reset" ) ) {
		if( 0 == strcmp( argv[2], "get" ) ) {
			ct_get_reset_ctrl(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "set" ) ) {
			ct_set_reset_ctrl(argc, argv);
		}
		else {
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
	else if( 0 == strcmp( argv[1], "gpio" ) ) {
		if( 0 == strcmp( argv[2], "get" ) ) {
			ct_get_gpio_ctrl(argv[3], argv[4]);
		}
		else if( 0 == strcmp( argv[2], "set" ) ) {
			ct_set_gpio_ctrl(argv[3], argv[4], atoi(argv[5]));
		}
		else if( 0 == strcmp( argv[2], "test" ) ) {
			ct_gpio_test(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "test_pdr" ) ) {
			ct_gpio_test_pdr(argc, argv);
		}
		else if( 0 == strcmp( argv[2], "err" ) ) {
			ct_gpio_err();
		}
		else {
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
	else{
		Ddim_Print(("please check 1st parameter!!\n"));
	}
}

void ct_dd_top_pctest_wrap(CHAR* cmd)
{
	INT32	argc = 0;
	CHAR*	argv[32];
	CHAR	arg[32][32];
	INT32	pos = 0;
	INT32	len = 0;
	INT32	max = strlen(cmd);
	INT32	i;

	printf("CMD> %s\n", cmd);

	// Clear arg buffer
	for(i = 0; i < 32; i++){
		argv[i] = arg[i];
		memset(argv[i], 0, 32);
	}

	// Set arg buffer
	for(i = 0; i < max; i++){
		if(cmd[i] == ' '){
			if(len){
				memcpy(argv[argc], &cmd[pos], len);
				argc++;
				len = 0;
			}
			pos = i + 1;
		}
		else if(i == max - 1){
			memcpy(argv[argc], &cmd[pos], len + 1);
			argc++;
			len = 0;
			pos = i + 1;
		}
		else{
			len++;
		}
	}

	// Execute command
	Ct_Dd_Top_Main(argc, argv);
}

void Ct_Dd_TOP_PcTest_Main(void)
{
	INT32	seq_no = 0;
//	INT32	i, j;

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_EMMCCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock EMMCCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock EMMCCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock EMMCCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock EMMCCLK 3");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_NFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFCLK 8");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_UHS2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS2CLK 7");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_UHS1CLK2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK2 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK2 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK2 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK2 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK2 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK2 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_UHS1CLK1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK1 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK1 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK1 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK1 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK1 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK1 8");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_UHS1CLK0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK0 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK0 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK0 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK0 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK0 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock UHS1CLK0 8");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_RCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock RCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RCLK 3");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_RAWCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock RAWCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RAWCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RAWCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RAWCLK 2");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_HIFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock HIFCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HIFCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HIFCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HIFCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HIFCLK 3");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_MIFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock MIFCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MIFCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MIFCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MIFCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MIFCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MIFCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MIFCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MIFCLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_IPUCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUCLK 7");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_IPUTMECLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPUTMECLK 7");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_GPUCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GPUCLK 7");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_JPEGCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock JPGCLK 7");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_ELACLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock ELACLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ELACLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ELACLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ELACLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ELACLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ELACLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ELACLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ELACLK 6");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_CLKSEL3_HEVENCSEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCSEL");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCSEL 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCSEL 1");

// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_CLKSEL3_PIPESEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock PIPESEL");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PIPESEL 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PIPESEL 1");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_SENCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENCLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_SENMSKCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENMSKCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENMSKCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SENMSKCLK 1");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_IIPCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock IIPCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IIPCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IIPCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IIPCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IIPCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IIPCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IIPCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IIPCLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_PASCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock PASCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PASCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PASCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PASCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PASCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PASCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PASCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PASCLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_IPPCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPPCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPPCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPPCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPPCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPPCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPPCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock IPPCLK 8");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_PXFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock PXFCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PXFCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PXFCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PXFCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PXFCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PXFCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PXFCLK 6");
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_HEVENCCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HEVENCCLK 5");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_VDFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock VDFCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock VDFCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock VDFCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock VDFCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock VDFCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock VDFCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock VDFCLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_SRO1CLK_2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_R2Y1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y1CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y1CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y1CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y1CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y1CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y1CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y1CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y1CLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_LTM1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 16");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 24");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM1CLK 32");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_B2R1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 16");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R1CLK 24");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_CNR1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR1CLK 8");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_SRO1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_B2B1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B1CLK 16");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_LTMRBK1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK1CLK 16");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_SRO2CLK_2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO1CLK_2 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_R2Y2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y2CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y2CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y2CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y2CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y2CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y2CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y2CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock R2Y2CLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_LTM2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 16");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 24");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTM2CLK 32");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_B2R2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 16");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2R2CLK 24");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_CNR2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock CNR2CLK 8");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_SRO2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO2CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO2CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO2CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO2CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO2CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO2CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO2CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SRO2CLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_B2B2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock B2B2CLK 16");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_LTMRBK2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock LTMRBK2CLK 16");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_APCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 9");
	ct_dd_top_pctest_wrap("ddtop clock get_clock APCLK 12");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_AUCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock AUCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock AUCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock AUCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock AUCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock AUCLK 3");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_SPICLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock SPICLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SPICLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SPICLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SPICLK 2");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_DSPCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock DSPCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock DSPCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock DSPCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock DSPCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock DSPCLK 3");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_ACLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK 1");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_ACLKEXS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLKEXS");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLKEXS 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLKEXS 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLKEXS 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLKEXS 7");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_HCLKBMH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock HCLKBMH");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HCLKBMH 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HCLKBMH 7");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_HCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock HCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HCLK 7");
	ct_dd_top_pctest_wrap("ddtop clock get_clock HCLK 15");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_PCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock PCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock get_clock PCLK 31");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_MCLK400()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock MCLK400");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MCLK400 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MCLK400 3");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_MCLK200()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock MCLK200");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MCLK200 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MCLK200 7");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_ACLK400()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK400");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK400 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK400 3");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_SHDRCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 0");
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 6");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 7");
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 8");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 9");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 10");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 11");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 12");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 13");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 14");
	ct_dd_top_pctest_wrap("ddtop clock get_clock SHDRCLK 15");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_RIBCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock RIBCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RIBCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RIBCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RIBCLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RIBCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock RIBCLK 4");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_NFBCHCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFBCHCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFBCHCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock NFBCHCLK 1");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_MECLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock MECLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MECLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MECLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MECLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock MECLK 3");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_FPT1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT1CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT1CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT1CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT1CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT1CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT1CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT1CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT1CLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_FPT0CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT0CLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT0CLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT0CLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT0CLK 2");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT0CLK 3");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT0CLK 4");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT0CLK 5");
	ct_dd_top_pctest_wrap("ddtop clock get_clock FPT0CLK 6");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_GYROCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock GYROCLK");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GYROCLK 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GYROCLK 1");
	ct_dd_top_pctest_wrap("ddtop clock get_clock GYROCLK 2");

	Ddim_Print(("*** [CT] 01-01-%03d : Dd_Top_Get_ACLK300()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK300");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK300 0");
	ct_dd_top_pctest_wrap("ddtop clock get_clock ACLK300 1");

	seq_no = 0;
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll00()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll00 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll00()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll00 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll01()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll01 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll01()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll01 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll02()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll02 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll02()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll02 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll03()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll03 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll03()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll03 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll04()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll04 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll04()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll04 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll05()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll05 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll05()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll05 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll05A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll05a start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll05A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll05a stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll06()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll06 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll06()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll06 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll07()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll07 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll07()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll07 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll08()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll08 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll08()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll08 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll10()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll10 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll10()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll10 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll10A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll10a start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll10A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll10a stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Pll11()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll11 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Pll11()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll11 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Ddr_Pll00()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll00 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Ddr_Pll00()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll00 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Ddr_Pll01()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll01 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Ddr_Pll01()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll01 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Ddr_Pll02()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll02 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Ddr_Pll02()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll02 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Ddr_Pll10()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll10 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Ddr_Pll10()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll10 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Ddr_Pll11()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll11 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Ddr_Pll11()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll11 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Start_Ddr_Pll12()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll12 start");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Ddr_Pll12()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll pll12 stop");

	Ddim_Print(("*** [CT] 01-02-%03d : Dd_Top_Stop_Ddr_Pll12()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop pll err");

	seq_no = 0;
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_PERSEL1_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl persel1");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_PERSEL2_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl persel2");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_PERSEL3_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl persel3");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_PERSEL4_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl persel4");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_MSELC_MSEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl msel");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_DBCNT1_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl dbcnt1");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_DBCNT2_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl dbcnt2");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_DBCNT3_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl dbcnt3");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_DBCNT4_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl dbcnt4");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_DBCNT5_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl dbcnt5");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_PUDCNT_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl pudcnt");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Get_PUDCNT_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi get_ctrl pudcnt");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_I2C0CNT()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C0CNT 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C0CNT 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C0CNT 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C0CNT 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_SDANC0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SDANC0 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SDANC0 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_SCLNC0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SCLNC0 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SCLNC0 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_I2C1CNT()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C1CNT 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C1CNT 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C1CNT 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C1CNT 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_SDANC1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SDANC1 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SDANC1 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_SCLNC1)\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SCLNC1 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SCLNC1 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_I2C2CNT()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C2CNT 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C2CNT 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C2CNT 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 I2C2CNT 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_SDANC2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SDANC2 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SDANC2 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_SCLNC2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SCLNC2 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 SCLNC2 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_DVWT2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 DVWT2 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 DVWT2 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_DHWT3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 DHWT3 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 DHWT3 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_AU0DO0C()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU0DO0C 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU0DO0C 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_AU0DO1C()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU0DO1C 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU0DO1C 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_AU1DOC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU1DOC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU1DOC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_AU0MCC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU0MCC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU0MCC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_AU1MCC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU1MCC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 AU1MCC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_RLSEL3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 RLSEL3 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 RLSEL3 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_RLSEL4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 RLSEL4 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 RLSEL4 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_RLSEL5()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 RLSEL5 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 RLSEL5 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL1_INTVDSEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 INTVDSEL 7");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 INTVDSEL 6");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 INTVDSEL 5");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 INTVDSEL 4");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 INTVDSEL 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 INTVDSEL 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 INTVDSEL 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel1 INTVDSEL 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDCTRG()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDCTRG 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDCTRG 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC0AM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC0AM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC0AM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC0BM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC0BM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC0BM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC1AM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC1AM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC1AM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC1BM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC1BM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC1BM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC2AM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC2AM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC2AM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC2BM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC2BM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC2BM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC3AM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC3AM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC3AM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC3BM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC3BM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC3BM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC4AM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC4AM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC4AM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC4BM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC4BM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC4BM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC5AM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC5AM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC5AM 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_UDC5BM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC5BM 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 UDC5BM 0");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_SPIMSEN0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 SPIMSEN0 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 SPIMSEN0 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_SPIMSEN1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 SPIMSEN1 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 SPIMSEN1 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL2_SPIMSEN2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 SPIMSEN2 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel2 SPIMSEN2 0");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_AU1WPPS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 AU1WPPS 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 AU1WPPS 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_AU0WP11()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 AU0WP11 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 AU0WP11 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_SNH2WP04()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNH2WP04 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNH2WP04 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_SNV2WP05()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP05 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP05 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_SNV2WP06()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP06 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP06 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_SNV2WP07()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP07 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP07 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_SNV2WP08()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP08 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP08 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_SNV2WP09()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP09 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SNV2WP09 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_U1VBSOU5()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 U1VBSOU5 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 U1VBSOU5 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_U1VBSOU7()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 U1VBSOU7 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 U1VBSOU7 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_U2IDWP14()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 U2IDWP14 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 U2IDWP14 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_PRT1SOU3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PRT1SOU3 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PRT1SOU3 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_PXW0WP12()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PXW0WP12 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PXW0WP12 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_PXW1SOU7()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PXW1SOU7 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PXW1SOU7 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_PXC0SCK7()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PXC0SCK7 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PXC0SCK7 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_PXC1WP15()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PXC1WP15 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PXC1WP15 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_SCK4WP13()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SCK4WP13 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 SCK4WP13 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_S3LSOU5WP14()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 S3LSOU5WP14 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 S3LSOU5WP14 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_S3SWP15()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 S3SWP15 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 S3SWP15 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_RTS1SCK2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 RTS1SCK2 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 RTS1SCK2 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_U1IDWP13()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 U1IDWP13 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 U1IDWP13 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL3_PRT0SCK3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PRT0SCK3 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel3 PRT0SCK3 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI22UD0A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI22UD0A 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI22UD0A 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI23UD0B()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI23UD0B 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI23UD0B 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI24UD1A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI24UD1A 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI24UD1A 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI25UD1B()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI25UD1B 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI25UD1B 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI26UD2A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI26UD2A 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI26UD2A 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI27UD2B()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI27UD2B 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI27UD2B 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI28UD3A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI28UD3A 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI28UD3A 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI29UD3B()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI29UD3B 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI29UD3B 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI30UD4A()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI30UD4A 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI30UD4A 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_EI31UD4B()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI31UD4B 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 EI31UD4B 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_SD3CDXS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 SD3CDXS 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 SD3CDXS 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_SD3WPS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 SD3WPS 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 SD3WPS 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_U1OCDXS)\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 U1OCDXS 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 U1OCDXS 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_U1IDDIGS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 U1IDDIGS 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 U1IDDIGS 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_U2OCDXS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 U2OCDXS 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 U2OCDXS 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PERSEL4_U2IDDIGS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 U2IDDIGS 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl persel4 U2IDDIGS 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_MSELC_MSEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 15");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 14");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 13");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 12");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 11");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 10");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 9");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 8");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 7");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 6");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 5");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 4");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl mselc MSEL 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T07DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T07DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T07DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T07DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T07DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T08DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T08DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T08DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T08DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T08DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T09DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T09DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T09DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T09DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T09DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T10DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T10DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T10DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T10DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T10DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T11DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T11DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T11DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T11DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T11DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T12DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T12DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T12DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T12DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T12DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T13DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T13DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T13DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T13DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T13DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T14DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T14DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T14DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T14DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T14DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT1_T15DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T15DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T15DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T15DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt1 T15DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T16DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T16DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T16DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T16DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T16DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T17DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T17DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T17DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T17DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T17DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T18DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T18DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T18DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T18DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T18DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T19DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T19DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T19DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T19DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T19DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T20DC)\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T20DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T20DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T20DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T20DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T21DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T21DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T21DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T21DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T21DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T22DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T22DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T22DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T22DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T22DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T23DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T23DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T23DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T23DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T23DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T24DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T24DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T24DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T24DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T24DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T25DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T25DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T25DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T25DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T25DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T26DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T26DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T26DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T26DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T26DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T27DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T27DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T27DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T27DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T27DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T28DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T28DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T28DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T28DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T28DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T29DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T29DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T29DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T29DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T29DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T30DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T30DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T30DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T30DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T30DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT2_T31DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T31DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T31DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T31DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt2 T31DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T32DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T32DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T32DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T32DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T32DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T33DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T33DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T33DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T33DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T33DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T34DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T34DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T34DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T34DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T34DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T35DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T35DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T35DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T35DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T35DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T36DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T36DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T36DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T36DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T36DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T37DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T37DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T37DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T37DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T37DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T38DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T38DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T38DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T38DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T38DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T39DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T39DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T39DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T39DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T39DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T40DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T40DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T40DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T40DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T40DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T41DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T41DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T41DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T41DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T41DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T42DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T42DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T42DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T42DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T42DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T43DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T43DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T43DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T43DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T43DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T44DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T44DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T44DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T44DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T44DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T45DCC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T45DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T45DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T45DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T45DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T46DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T46DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T46DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T46DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T46DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT3_T47DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T47DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T47DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T47DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt3 T47DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T48DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T48DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T48DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T48DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T48DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T49DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T49DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T49DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T48DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T49DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T50DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T50DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T50DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T50DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T50DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T51DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T51DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T51DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T51DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T51DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T52DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T52DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T52DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T52DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T52DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T53DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T53DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T53DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T53DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T53DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T54DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T54DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T54DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T54DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T54DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T55DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T55DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T54DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T54DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T54DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T56DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T56DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T56DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T56DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T56DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T57DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T57DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T57DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T57DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T57DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T58DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T58DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T58DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T58DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T58DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T59DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T59DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T59DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T59DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T59DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T60DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T60DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T60DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T60DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T60DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T61DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T61DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T61DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T61DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T61DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T62DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T62DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T62DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T62DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T62DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT4_T63DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T63DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T63DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T63DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt4 T63DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT5_T64DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T64DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T64DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T64DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T64DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT5_T65DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T65DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T65DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T65DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T65DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_DBCNT5_T66DC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T66DC 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T66DC 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T66DC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl dbcnt5 T66DC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_EMMCNICS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMMCNICS 3");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMMCNICS 2");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMMCNICS 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMMCNICS 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_EMDATPUE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMDATPUE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMDATPUE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_EMCMDPUE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMCMDPUE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMCMDPUE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_EMDSPDE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMDSPDE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt EMDSPDE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_NFDATPUE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt NFDATPUE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt NFDATPUE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_NFRBXPUE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt NFRBXPUE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt NFRBXPUE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_NFDQSPUE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt NFDQSPUE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt NFDQSPUE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_SP1MISOPE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1MISOPE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1MISOPE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_SP1MOSIPE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1MOSIPE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1MOSIPE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_SP1SCKPE)\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1SCKPE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1SCKPE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_SP1CS0PE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1CS0PE 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1CS0PE 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_SP1MISOUDC)\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1MISOUDC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1MISOUDC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_SP1MOSIUDC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1MOSIUDC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1MOSIUDC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_SP1SCKUDC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1SCKUDC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1SCKUDC 0");

	Ddim_Print(("*** [CT] 01-03-%03d : Dd_Top_Set_PUDCNT_SP1CS0UDC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1CS0UDC 1");
	ct_dd_top_pctest_wrap("ddtop lsi set_ctrl pudcnt SP1CS0UDC 0");

	seq_no = 0;
	Ddim_Print(("*** [CT] 01-04-%03d : CRRSM/CRRRS)\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset get");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRSM_PORST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRSM_PORST");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRSM_SRST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRSM_SRST");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRSM_SWRST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRSM_SWRST");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRSM_WDRST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRSM_WDRST");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_CPU0_SR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_CPU0_SR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_CPU0_SR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_CPU0_SR_RES");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_CPU1_SR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_CPU1_SR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_CPU1_SR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_CPU1_SR_RES");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_CPU2_SR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_CPU2_SR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_CPU2_SR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_CPU2_SR_RES");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_CPU3_SR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_CPU3_SR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_CPU3_SR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_CPU3_SR_RES");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_RIBSR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_RIBSR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_RIBSR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_RIBSR_RES");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEIPPASYNCSR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEIPPASYNCSR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEIPPASYNCSR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEIPPASYNCSR_RES");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEIPPSYNCSR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEIPPSYNCSR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEIPPSYNCSR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEIPPSYNCSR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEVDFSR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEVDFSR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEVDFSR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEVDFSR_RES");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEPXFSR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEPXFSR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEPXFSR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEPXFSR_RES");
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEVENCSR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEVENCSR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_HEVENCSR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_HEVENCSR_RES");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_DSPSR_CLR()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_DSPSR_CLR");

	Ddim_Print(("*** [CT] 01-04-%03d : Dd_Top_Clear_CRRRS_DSPSR_RES()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop reset set CRRRS_DSPSR_RES");

	seq_no = 0;
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop1 0 0x30");
	
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop2 0 0x10");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop3 0 0xF0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop4 0 0xF00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop5 0 0x10000");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop6 0 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop7 0 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop8 0 0x30");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop9 0 0x10");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop10 0 0xF0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop11 0 0xF00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop12 0 0x10000");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop13 0 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop14 0 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop1 1 0x30");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop2 1 0x10");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop3 1 0xF0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop4 1 0xF00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop5 1 0x10000");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop6 1 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop7 1 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop8 1 0x30");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop9 1 0x10");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop10 1 0xF0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop11 1 0xF00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop12 1 0x10000");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop13 1 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop14 1 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Start_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock start clkstop14 1 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Stop_Clock()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock stop clkstop14 2 0xD00");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel1");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel3");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL5()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel5");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL6()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel6");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL7()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel7");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL8()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL9()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel9");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL10()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel10");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL11()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel11");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSEL12()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clksel12");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT1_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt1");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT2_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT3_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt3");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT4_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT5_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt5");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT6_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt6");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT7_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt7");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT8_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_PLLCNT9_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg pllcnt9");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP1_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop1");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP2_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP3_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop3");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP4_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP5_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop5");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP6_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop6");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP7_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop7");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP8_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP9_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop9");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP10_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop10");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP11_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop11");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP12_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop12");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP13_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop13");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Get_CLKSTOP14_xxxxx()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock get_reg clkstop14");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL1_RCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 RCLK 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 RCLK 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL1_UHS1CLK0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 UHS1CLK0 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 UHS1CLK0 16");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL1_UHS1CLK1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 UHS1CLK1 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 UHS1CLK1 16");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL1_UHS1CLK2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 UHS1CLK2 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 UHS1CLK2 16");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL1_UHS2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 UHS2CLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 UHS2CLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL1_NFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 NFCLK 63");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 NFCLK 32");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL1_EMMCCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 EMMCCLK 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel1 EMMCCLK 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL2_ELACLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 ELACLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 ELACLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL2_JPGCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 ELACLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 ELACLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL2_GPUCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 GPUCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 GPUCLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL2_IPUTMECLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 IPUTMECLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 IPUTMECLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL2_IPUCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 IPUCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 IPUCLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL2_MIFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 MIFCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 MIFCLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL2_HIFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 HIFCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 HIFCLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL2_RAWCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 RAWCLK 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel2 RAWCLK 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_VDFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 VDFCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 VDFCLK 8");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_PXFCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 PXFCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 PXFCLK 8");
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_HEVENCCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 HEVENCCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 HEVENCCLK 8");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_IPPCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 IPPCLK 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 IPPCLK 16");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_PASCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 PASCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 PASCLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_IIPCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 IIPCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 IIPCLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_SENMSKCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 SENMSKCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 SENMSKCLK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_SENCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 SENCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 SENCLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_PIPESEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 PIPESEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 PIPESEL 2");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL3_HEVENCSEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 HEVENCSEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel3 HEVENCSEL 2");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL4_CNR1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 CNR1CLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 CNR1CLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL4_B2R1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 B2R1CLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 B2R1CLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL4_LTM1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 LTM1CLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 LTM1CLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL4_R2Y1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 R2Y1CLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 R2Y1CLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL4_SRO1CLK_2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 SRO1CLK_2 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel4 SRO1CLK_2 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL5_LTMRBK1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel5 LTMRBK1CLK 63");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel5 LTMRBK1CLK 32");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL5_B2B1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel5 B2B1CLK 63");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel5 B2B1CLK 32");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL5_SRO1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel5 SRO1CLK 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel5 SRO1CLK 16");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL6_CNR2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 CNR2CLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 CNR2CLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL6_B2R2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 B2R2CLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 B2R2CLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL6_LTM2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 LTM2CLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 LTM2CLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL6_R2Y2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 R2Y2CLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 R2Y2CLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL6_SRO2CLK_2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 SRO2CLK_2 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel6 SRO2CLK_2 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL7_LTMRBK2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 LTMRBK2CLK 63");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 LTMRBK2CLK 32");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL7_B2B2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 B2B2CLK 63");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 B2B2CLK 32");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL7_SRO2CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 SRO2CLK 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 SRO2CLK 16");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL7_MIPISEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 MIPISEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 MIPISEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL7_PDM0SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 PDM0SEL 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 PDM0SEL 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL7_PDM1SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 PDM1SEL 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel7 PDM1SEL 4");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_DSPCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 DSPCLK 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 DSPCLK 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_SPICLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 SPICLK 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 SPICLK 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_AUCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AUCLK 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AUCLK 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_AU0SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AU0SEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AU0SEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_AU2SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AU2SEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AU2SEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_AU3SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AU3SEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AU3SEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_NETAUSEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 NETAUSEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 NETAUSEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_APCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 APCLK 63");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 APCLK 32");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_AP0SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AP0SEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AP0SEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_AP1SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AP1SEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AP1SEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_AP2SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AP2SEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AP2SEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL8_AP3SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AP3SEL 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel8 AP3SEL 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL9_PCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 PCLK 127");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 PCLK 64");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL9_HCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 HCLK 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 HCLK 16");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL9_HCLKBMH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 HCLKBMH 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 HCLKBMH 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL9_ACLKEXS()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 ACLKEXS 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 ACLKEXS 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL9_ACLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 ACLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel9 ACLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL10_ACLK400()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel10 ACLK400 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel10 ACLK400 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL10_MCLK200()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel10 MCLK200 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel10 MCLK200 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL10_MCLK400()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel10 MCLK400 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel10 MCLK400 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL11_DCHREQ()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel11 DCHREQ 1");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL12_ACLK300()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 ACLK300 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 ACLK300 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL12_GYROCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 GYROCLK 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 GYROCLK 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL12_SHDRCLK()\n", seq_no++));
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 SHDRCLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 SHDRCLK 8");
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 SHDRCLK 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 SHDRCLK 16");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL12_FPT0CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 FPT0CLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 FPT0CLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL12_FPT1CLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 FPT1CLK 15");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 FPT1CLK 8");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL12_MECLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 MECLK 7");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 MECLK 4");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL12_NFBCHCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 NFBCHCLK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 NFBCHCLK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSEL12_RIBCLK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 RIBCLK 31");
	ct_dd_top_pctest_wrap("ddtop clock set_reg clksel12 RIBCLK 16");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL00ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL00ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL00ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL01ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL01ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL01ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL02ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL02ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL02ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL03ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL03ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL03ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL04ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL04ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL04ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL05ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL05ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL05ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL05AST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL05AST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL05AST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL06ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL06ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL06ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL07ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL07ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL07ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL08ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL08ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL08ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL10ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL10ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL10ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL10AST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL10AST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL10AST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_PL11ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL11ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 PL11ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_DPL00ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL00ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL00ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_DPL01ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL01ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL01ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_DPL02ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL02ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL02ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_DPL10ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL10ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL10ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_DPL11ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL11ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL11ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT1_DPL12ST()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL12ST 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt1 DPL12ST 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL00SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL00SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL00SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL01SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL01SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL01SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL02SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL02SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL02SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL03SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL03SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL03SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL04SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL04SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL04SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL05SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL05SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL05SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL05ASEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL05ASEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL05ASEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL06SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL06SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL06SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL07SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL07SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL07SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL08SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL08SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL08SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL10SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL10SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL10SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL10ASEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL10ASEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL10ASEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL11SEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL11SEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL11SEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL01SSEN()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL01SSEN 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL01SSEN 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT2_PL02SSEN()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL02SSEN 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt2 PL02SSEN 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT3_P00POSTDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt3 P00POSTDIV 255");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt3 P00POSTDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT3_P00PREDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt3 P00PREDIV 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt3 P00PREDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT3_P00PLLDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt3 P00PLLDIV 255");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt3 P00PLLDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT3_P00PLLDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt3 P00PLLDIV 255");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt3 P00PLLDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT4_P00FNUM()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt4 P00FNUM -1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt4 P00FNUM 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT5_P00FDEN()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt5 P00FDEN -1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt5 P00FDEN 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT6_P01RATE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P01RATE 164");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P01RATE 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT6_P01FREQ()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P01FREQ 2");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P01FREQ 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT6_P01MODE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P01MODE 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P01MODE 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT6_P02RATE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P02RATE 164");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P02RATE 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT6_P02FREQ()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P02FREQ 2");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P02FREQ 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT6_P02MODE()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P02MODE 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt6 P02MODE 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_P03POSTDIV0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03POSTDIV0 8");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03POSTDIV0 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_P03POSTDIV1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03POSTDIV1 8");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03POSTDIV1 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_P03PREDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03PREDIV 5");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03PREDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_P03PLLDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03PLLDIV 149");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03PLLDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_P03OC0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03OC0 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03OC0 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_P03OC1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03OC1 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 P03OC1 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_D0XOC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 D0XOC 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 D0XOC 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_D1XOC()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 D1XOC 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 D1XOC 0");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_DPLX8()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 DPLX8 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 DPLX8 0");

// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT7_DSEL()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 DSEL 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt7 DSEL 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT8_P04POSTDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt8 P04POSTDIV 254");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt8 P04POSTDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT8_P04PREDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt8 P04PREDIV 5");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt8 P04PREDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT8_P04PLLDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt8 P04PLLDIV 149");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt8 P04PLLDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT9_P10APLLDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt9 P10APLLDIV 2");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt9 P10APLLDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT9_P08PLLDIV()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt9 P08PLLDIV 74");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt9 P08PLLDIV 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_PLLCNT9_P05CHG()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt9 P05CHG 1");
	ct_dd_top_pctest_wrap("ddtop clock set_reg pllcnt9 P05CHG 0");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_DSPCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 DSPCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 DSPCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_DSPAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 DSPAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 DSPAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_SENCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 SENCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 SENCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_SENAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 SENAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 SENAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_SENAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 SENAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 SENAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_SENAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 SENAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 SENAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_GPIOAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 GPIOAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 GPIOAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_AU0CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU0CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU0CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_AU2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_AU3CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU3CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU3CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_AU4CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU4CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU4CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_AU5CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU5CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 AU5CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_NETAUCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 NETAUCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 NETAUCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP1_TEMPCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 TEMPCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP1 TEMPCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_RCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 RCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 RCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_UHS1CK0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 UHS1CK0 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 UHS1CK0 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_UHS1CK1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 UHS1CK1 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 UHS1CK1 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_UHS1CK2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 UHS1CK2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 UHS1CK2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_UHS2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 UHS2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 UHS2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_NFCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 NFCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 NFCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_EMMCCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 EMMCCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 EMMCCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_NETSECCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 NETSECCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 NETSECCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_NETRCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 NETRCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 NETRCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_EXSAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 EXSAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 EXSAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_SPICK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SPICK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SPICK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_SLIMB00CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SLIMB00CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SLIMB00CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_SLIMB01CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SLIMB01CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SLIMB01CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_SLIMB10CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SLIMB10CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SLIMB10CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_SLIMB11CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SLIMB11CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 SLIMB11CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP2_PCISUPPCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 PCISUPPCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP2 PCISUPPCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_IIPCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 IIPCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 IIPCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_IIPAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 IIPAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 IIPAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_IIPAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 IIPAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 IIPAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_IIPAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 IIPAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 IIPAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_LCDCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 LCDCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 LCDCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_HIFCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 HIFCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 HIFCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_MIFCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 MIFCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 MIFCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_DISPAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 DISPAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 DISPAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_DISPAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 DISPAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 DISPAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_DISPAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 DISPAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 DISPAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_JPGCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 JPGCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 JPGCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_JPGAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 JPGAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 JPGAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_JPGAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 JPGAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 JPGAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_JPGAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 JPGAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 JPGAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_PDM0CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 PDM0CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 PDM0CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP3_PDM1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 PDM1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP3 PDM1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_GPUCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 GPUCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 GPUCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_GPUAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 GPUAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 GPUAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_GPUCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 GPUAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 GPUAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_GPUCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 GPUAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 GPUAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_FPT0CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT0CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT0CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_FPT0AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT0AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT0AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_FPT0AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT0AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT0AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_FPT0AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT0AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT0AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_FPT1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_FPT1AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT1AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT1AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_FPT1AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT1AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT1AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_FPT1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 FPT1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_APCK0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 APCK0 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 APCK0 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_APCK1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 APCK1 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 APCK1 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_APCK2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 APCK2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 APCK2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP4_APCK3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 APCK3 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP4 APCK3 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAX0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX0 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX0 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAX1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX1 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX1 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAX2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAX3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX3 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX3 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAX5()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX5 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX5 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAX6()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX6 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAX6 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAP0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP0 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP0 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAP1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP1 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP1 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAP2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAP3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP3 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP3 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAP4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAP5()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP5 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP5 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP5_MICAP6()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP6 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP5 MICAP6 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_MICAH1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 MICAH1 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 MICAH1 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_MICAH2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 MICAH2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 MICAH2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_MICAH3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 MICAH3 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 MICAH3 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_IMGAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_IMGAH0()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAH0 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAH0 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_IMGAH1()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAH1 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAH1 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_IMGAH3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAH3 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAH3 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_IMGAP3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAP3 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 IMGAP3 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_REGAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 REGAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 REGAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_XCHAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 XCHAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 XCHAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_XCHAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 XCHAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 XCHAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_ELACK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 ELACK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 ELACK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_ELAAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 ELAAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 ELAAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP6_ELAAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 ELAAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP6 ELAAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_IPUFDCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 IPUFDCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 IPUFDCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_IPUVISCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 IPUVISCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 IPUVISCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_IPUAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 IPUAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 IPUAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_IPUAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 IPUAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 IPUAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_RAWCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 RAWCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 RAWCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_RAWAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 RAWAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 RAWAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_RAWAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 RAWAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 RAWAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_SHDRCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 SHDRCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 SHDRCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_SHDRAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 SHDRAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 SHDRAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_SHDRAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 SHDRAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 SHDRAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_SHDRAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 SHDRAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 SHDRAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_M0CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 M0CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 M0CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_MECK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 MECK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 MECK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_MEAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 MEAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 MEAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP7_MEAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 MEAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP7 MEAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_RIBCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 RIBCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 RIBCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_RIBAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 RIBAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 RIBAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_HEVDFCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 HEVDFCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 HEVDFCK 2");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_HEPXFCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 HEPXFCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 HEPXFCK 2");
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_HEVENCCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 HEVENCCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 HEVENCCK 2");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_HEIPPCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 HEIPPCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 HEIPPCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC0HEVCMX2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0HEVCMX2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0HEVCMX2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC0HEVCMX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0HEVCMX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0HEVCMX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC0RBRMX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0RBRMX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0RBRMX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC1HEVCMX2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC1HEVCMX2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC1HEVCMX2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC1HEVCMX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC1HEVCMX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC1HEVCMX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC1RBRMX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC1RBRMX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC1RBRMX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC0CMNAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0CMNAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0CMNAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC0MX1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0MX1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0MX1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC0MX2AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0MX2AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0MX2AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC0MX3AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0MX3AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0MX3AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP8_UMC0MX4AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0MX4AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP8 UMC0MX4AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC0MX5AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0MX5AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0MX5AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1CMNAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1CMNAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1CMNAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1MX1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1MX2AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX2AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX2AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1MX3AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX3AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX3AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1MX4AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX4AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX4AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1MX5AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX5AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX5AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC0MX0AX3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0MX0AX3 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0MX0AX3 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC0MX6AX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0MX6AX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0MX6AX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC0HEVCAX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0HEVCAX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0HEVCAX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1MX0AX3()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX0AX3 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX0AX3 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1MX6AX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX6AX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1MX6AX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1HEVCAX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1HEVCAX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1HEVCAX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC0AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC0AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP9_UMC1AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP9 UMC1AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_SRO1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_SRO1CK_2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1CK_2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1CK_2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_SRO1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_SRO1AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_SRO1AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 SRO1AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_B2B1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2B1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2B1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_B2B1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2B1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2B1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_B2B1AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2B1AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2B1AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_B2B1AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2B1AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2B1AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_LTMRBK1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 LTMRBK1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 LTMRBK1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_B2R1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2R1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2R1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_B2R1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2R1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2R1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_B2R1AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2R1AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2R1AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP10_B2R1AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2R1AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP10 B2R1AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_LTM1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 LTM1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 LTM1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_LTM1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 LTM1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 LTM1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_LTM1AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 LTM1AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 LTM1AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_LTM1AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 LTM1AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 LTM1AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_R2Y1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 R2Y1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 R2Y1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_R2Y1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 R2Y1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 R2Y1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_R2Y1AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 R2Y1AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 R2Y1AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_R2Y1AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 R2Y1AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 R2Y1AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_CNR1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 CNR1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 CNR1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_CNR1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 CNR1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 CNR1AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_CNR1AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 CNR1AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 CNR1AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_APAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 APAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 APAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_DBGAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 DBGAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 DBGAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP11_NFBCHCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 NFBCHCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP11 NFBCHCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_SRO2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_SRO2CK_2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2CK_2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2CK_2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_SRO2AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_SRO2AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_SRO2AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 SRO2AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_B2B2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2B2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2B2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_B2B2AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2B2AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2B2AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_B2B2AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2B2AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2B2AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_B2B2AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2B2AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2B2AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_LTMRBK2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 LTMRBK2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 LTMRBK2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_B2R2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2R2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2R2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_B2R2AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2R2AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2R2AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_B2R2AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2R2AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2R2AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP12_B2R2AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2R2AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP12 B2R2AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_LTM2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 LTM2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 LTM2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_LTM2AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 LTM2AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 LTM2AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_LTM2AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 LTM2AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 LTM2AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_LTM2AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 LTM2AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 LTM2AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_R2Y2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 R2Y2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 R2Y2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_R2Y2AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 R2Y2AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 R2Y2AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_R2Y2AH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 R2Y2AH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 R2Y2AH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_R2Y2AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 R2Y2AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 R2Y2AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_CNR2CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 CNR2CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 CNR2CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_CNR2AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 CNR2AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 CNR2AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_CNR2AP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 CNR2AP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 CNR2AP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_UMCVDFMX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCVDFMX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCVDFMX4 2");

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_UMCPXFMX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCPXFMX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCPXFMX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_UMCVDFMX2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCVDFMX2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCVDFMX2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_UMCPXFMX2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCPXFMX2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCPXFMX2 2");
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_UMCHEVENCAX4()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCHEVENCAX4 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCHEVENCAX4 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_UMCVDFMX2()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCVDFMX2 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 UMCVDFMX2 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_HEVENCAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 HEVENCAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 HEVENCAP 2");
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP13_BMH1CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 BMH1CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP13 BMH1CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_STATAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 STATAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 STATAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_STATAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 STATAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 STATAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_STATAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 STATAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 STATAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_PERIAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 PERIAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 PERIAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_PERIAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 PERIAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 PERIAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_SENMSKCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 SENMSKCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 SENMSKCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_GYROCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 GYROCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 GYROCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_EXSAH()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 EXSAH 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 EXSAH 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_EXSAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 EXSAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 EXSAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_PASCK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 PASCK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 PASCK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_BMH0CK()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 BMH0CK 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 BMH0CK 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_BMH0AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 BMH0AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 BMH0AX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_RDMAAX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 RDMAAX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 RDMAAX 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_RDMAAP()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 RDMAAP 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 RDMAAP 2");

	Ddim_Print(("*** [CT] 01-05-%03d : Dd_Top_Set_CLKSTOP14_BMH1AX()\n", seq_no++));
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 BMH1AX 3");
	ct_dd_top_pctest_wrap("ddtop clock set_reg CLKSTOP14 BMH1AX 2");

	Ddim_Print(("*** [CT] 01-06-001 - 005\n"));
	ct_dd_top_pctest_wrap("ddtop gpio test");
	ct_dd_top_pctest_wrap("ddtop gpio err");
}
