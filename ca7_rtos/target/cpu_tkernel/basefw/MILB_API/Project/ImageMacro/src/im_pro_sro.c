/**
 * @file		im_pro_sro.c
 * @brief		SRO unit controler of PRO macro.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdspro.h"
#include "im_pro.h"
#include "im_pro_common.h"
#include "dd_top.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_IM_SRO1_STATUS_NONE		(0x00000000UL)				/* All stopped		*/
#define	D_IM_SRO1_STATUS_FF0		((0x00000000UL)&(1<< 0))	/* FF0				*/
#define	D_IM_SRO1_STATUS_FF1		((0x00000000UL)&(1<< 1))	/* FF1				*/
#define	D_IM_SRO1_STATUS_PGAIN0		((0x00000000UL)&(1<< 2))	/* PGAIN0			*/
#define	D_IM_SRO1_STATUS_FFSH0		((0x00000000UL)&(1<< 3))	/* FFSH0			*/
#define	D_IM_SRO1_STATUS_FFSH1		((0x00000000UL)&(1<< 4))	/* FFSH1			*/
#define	D_IM_SRO1_STATUS_SDC0		((0x00000000UL)&(1<< 5))	/* SDC0				*/
#define	D_IM_SRO1_STATUS_SDC1		((0x00000000UL)&(1<< 6))	/* SDC1				*/
#define	D_IM_SRO1_STATUS_DPC		((0x00000000UL)&(1<< 7))	/* DPC				*/
#define	D_IM_SRO1_STATUS_AFPB_A		((0x00000000UL)&(1<< 8))	/* AFPB A			*/
#define	D_IM_SRO1_STATUS_AFPB_G		((0x00000000UL)&(1<< 9))	/* AFPB G			*/
#define	D_IM_SRO1_STATUS_FSHDL0		((0x00000000UL)&(1<<10))	/* FSHDL0			*/
#define	D_IM_SRO1_STATUS_FSHDL1		((0x00000000UL)&(1<<11))	/* FSHDL1			*/
#define	D_IM_SRO1_STATUS_FFC		((0x00000000UL)&(1<<12))	/* FFC				*/
#define	D_IM_SRO1_STATUS_CAG		((0x00000000UL)&(1<<13))	/* CAG				*/
#define	D_IM_SRO1_STATUS_VRB		((0x00000000UL)&(1<<14))	/* VRB				*/
#define	D_IM_SRO1_STATUS_HRB		((0x00000000UL)&(1<<15))	/* HRB				*/

#define	D_IM_SRO2_STATUS_NONE		(0x00000000UL)				/* All stopped		*/
#define	D_IM_SRO2_STATUS_FF0		((0x10000000UL)&(1<< 0))	/* FF0				*/
#define	D_IM_SRO2_STATUS_FF1		((0x10000000UL)&(1<< 1))	/* FF1				*/
#define	D_IM_SRO2_STATUS_PGAIN0		((0x10000000UL)&(1<< 2))	/* PGAIN0			*/
#define	D_IM_SRO2_STATUS_FFSH0		((0x10000000UL)&(1<< 3))	/* FFSH0			*/
#define	D_IM_SRO2_STATUS_FFSH1		((0x10000000UL)&(1<< 4))	/* FFSH1			*/
#define	D_IM_SRO2_STATUS_SDC0		((0x10000000UL)&(1<< 5))	/* SDC0				*/
#define	D_IM_SRO2_STATUS_SDC1		((0x10000000UL)&(1<< 6))	/* SDC1				*/
#define	D_IM_SRO2_STATUS_DPC		((0x10000000UL)&(1<< 7))	/* DPC				*/
#define	D_IM_SRO2_STATUS_AFPB_A		((0x10000000UL)&(1<< 8))	/* AFPB A			*/
#define	D_IM_SRO2_STATUS_AFPB_G		((0x10000000UL)&(1<< 9))	/* AFPB G			*/
#define	D_IM_SRO2_STATUS_FSHDL0		((0x10000000UL)&(1<<10))	/* FSHDL0			*/
#define	D_IM_SRO2_STATUS_FSHDL1		((0x10000000UL)&(1<<11))	/* FSHDL1			*/
#define	D_IM_SRO2_STATUS_FFC		((0x10000000UL)&(1<<12))	/* FFC				*/
#define	D_IM_SRO2_STATUS_CAG		((0x10000000UL)&(1<<13))	/* CAG				*/
#define	D_IM_SRO2_STATUS_VRB		((0x10000000UL)&(1<<14))	/* VRB				*/
#define	D_IM_SRO2_STATUS_HRB		((0x10000000UL)&(1<<15))	/* HRB				*/

#define	D_IM_SRO_STATUS_FF0_BOTH	( D_IM_SRO1_STATUS_FF0		| D_IM_SRO2_STATUS_FF0		)	/* Image Pipe1&2 FF0	*/
#define	D_IM_SRO_STATUS_FF1_BOTH	( D_IM_SRO1_STATUS_FF1		| D_IM_SRO2_STATUS_FF1		)	/* Image Pipe1&2 FF0	*/
#define	D_IM_SRO_STATUS_PGAIN0_BOTH	( D_IM_SRO1_STATUS_PGAIN0	| D_IM_SRO2_STATUS_PGAIN0	)	/* Image Pipe1&2 PGAIN0	*/
#define	D_IM_SRO_STATUS_FFSH0_BOTH	( D_IM_SRO1_STATUS_FFSH0	| D_IM_SRO2_STATUS_FFSH0	)	/* Image Pipe1&2 FFSH0	*/
#define	D_IM_SRO_STATUS_FFSH1_BOTH	( D_IM_SRO1_STATUS_FFSH1	| D_IM_SRO2_STATUS_FFSH1	)	/* Image Pipe1&2 FFSH1	*/
#define	D_IM_SRO_STATUS_SDC0_BOTH	( D_IM_SRO1_STATUS_SDC0		| D_IM_SRO2_STATUS_SDC0		)	/* Image Pipe1&2 SDC0	*/
#define	D_IM_SRO_STATUS_SDC1_BOTH	( D_IM_SRO1_STATUS_SDC1		| D_IM_SRO2_STATUS_SDC1		)	/* Image Pipe1&2 SDC1	*/
#define	D_IM_SRO_STATUS_DPC_BOTH	( D_IM_SRO1_STATUS_DPC		| D_IM_SRO2_STATUS_DPC		)	/* Image Pipe1&2 DPC	*/
#define	D_IM_SRO_STATUS_AFPB_A_BOTH	( D_IM_SRO1_STATUS_AFPB_A	| D_IM_SRO2_STATUS_AFPB_A	)	/* Image Pipe1&2 AFPB_A	*/
#define	D_IM_SRO_STATUS_AFPB_G_BOTH	( D_IM_SRO1_STATUS_AFPB_G	| D_IM_SRO2_STATUS_AFPB_G	)	/* Image Pipe1&2 AFPB_G	*/
#define	D_IM_SRO_STATUS_FSHDL0_BOTH	( D_IM_SRO1_STATUS_FSHDL0	| D_IM_SRO2_STATUS_FSHDL0	)	/* Image Pipe1&2 FSHDL0	*/
#define	D_IM_SRO_STATUS_FSHDL1_BOTH	( D_IM_SRO1_STATUS_FSHDL1	| D_IM_SRO2_STATUS_FSHDL1	)	/* Image Pipe1&2 FSHDL1	*/
#define	D_IM_SRO_STATUS_FFC_BOTH	( D_IM_SRO1_STATUS_FFC		| D_IM_SRO2_STATUS_FFC		)	/* Image Pipe1&2 FFC	*/
#define	D_IM_SRO_STATUS_CAG_BOTH	( D_IM_SRO1_STATUS_CAG		| D_IM_SRO2_STATUS_CAG		)	/* Image Pipe1&2 CAG	*/
#define	D_IM_SRO_STATUS_VRB_BOTH	( D_IM_SRO1_STATUS_VRB		| D_IM_SRO2_STATUS_VRB		)	/* Image Pipe1&2 VRB	*/
#define	D_IM_SRO_STATUS_HRB_BOTH	( D_IM_SRO1_STATUS_HRB		| D_IM_SRO2_STATUS_HRB		)	/* Image Pipe1&2 HRB	*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// PRO Channel/Macro start/stop status
#define im_pro_sro_set_start_status(st, ch) 	(gIM_SRO_START_Status[((st) >> 28) & 0xf] |=  (((st) & 0x0fffffff) << (ch)))							/* SRO clock status ON	*/
#define im_pro_sro_set_stop_status(st, ch)		(gIM_SRO_START_Status[((st) >> 28) & 0xf] &= ~(((st) & 0x0fffffff) << (ch)))							/* SRO clock status OFF	*/
#define im_pro_sro_get_start_status(st, ch)		(((gIM_SRO_START_Status[((st) >> 28) & 0xf] & (((st) & 0x0fffffff) << (ch))) != 0) ? TRUE : FALSE)		/* started check	*/

#define im_pro_sro_get_stop_all()				(((gIM_SRO_START_Status[0] | gIM_SRO_START_Status[1] | gIM_SRO_START_Status[2]) != 0) ? FALSE : TRUE)	/* started check	*/

static const T_IM_PRO_RDMA_FFSH_ADDR gIM_PRO_FFSH_Addr[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_FFSH_CH_NUM] = {
	{
		{
			0x2840821C,0x28408230,0x28408234,0x28408238,
			0x2840823C,
		},
		{
			0x2840831C,0x28408330,0x28408334,0x28408338,
			0x2840833C,
		},
	},
	{
		{
			0x2850821C,0x28508230,0x28508234,0x28508238,
			0x2850823C,
		},
		{
			0x2850831C,0x28508330,0x28508334,0x28508338,
			0x2850833C,
		},
	},
	{
		{
			0x2860821C,0x28608230,0x28608234,0x28608238,
			0x2860823C,
		},
		{
			0x2860831C,0x28608330,0x28608334,0x28608338,
			0x2860833C,
		},
	},
};

static const T_IM_PRO_RDMA_SDC_ADDR gIM_PRO_SDC_Addr[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_SDC_CH_NUM] = {
	{
		{
			0x28408808,0x28408814,0x28408880,0x28408884,
			0x28408888,0x2840888C,0x28408890,0x28408894,
			0x28408898,0x2840889C,0x284088A0,0x284088A4,
			0x284088A8,0x284088AC,0x284088B0,0x284088B4,
			0x284088B8,0x284088BC,0x284088C0,0x284088C4,
			0x28408900,0x28408904,0x28408908,0x2840890C,
			0x28408910,0x28408914,0x28408918,0x2840891C,
			0x28408920,0x28408924,0x28408928,0x2840892C,
			0x28408930,0x28408934,0x28408938,0x2840893C,
			0x28408940,0x28408944,0x28408980,0x28408984,
			0x28408988,0x28408990,0x28408994,0x28408998,
			0x284089A0,0x284089A4,0x284089A8,0x284089AC,
			0x284089B0,0x284089C0,0x284089C4,0x284089C8,
			0x284089CC,0x284089D0,0x284089E0,0x284089E4,
			0x284089E8,0x284089EC,0x284089F0,0x28408A00,
			0x28408A04,0x28408A08,0x28408A0C,0x28408A10,
			0x28408A20,0x28408A24,0x28408A28,0x28408A2C,
			0x28408A30,0x28408A40,0x28408A44,0x28408A48,
			0x28408A4C,0x28408A50,0x28408A60,0x28408A64,
			0x28408A68,0x28408A6C,0x28408A70,0x28408A80,
			0x28408A84,0x28408A88,0x28408A8C,0x28408A90,
			0x28408AC0,0x28408AC4,0x28408AC8,0x28408ACC,
			0x28408AD0,0x28408AD4,0x28408AD8,0x28408ADC,
			0x28408AE0,0x28408AE4,0x28408AE8,0x28408AEC,
			0x28408AF0,0x28408AF4,0x28408AF8,0x28408AFC,
			0x28408B00,0x28408B04,0x28408B08,0x28408B0C,
			0x28408B10,0x28408B14,0x28408B18,0x28408B1C,
			0x28408B20,0x28408B24,0x28408B28,0x28408B2C,
			0x28408B30,0x28408B34,0x28408B38,0x28408B3C,
			0x28408B40,0x28408B44,0x28408B48,0x28408B4C,
			0x28408B50,0x28408B54,0x28408B58,0x28408B5C,
			0x28408B60,0x28408B64,0x28408B9C,0x28408BA0,
		},
		{
			0x2840C808,0x2840C814,0x2840C880,0x2840C884,
			0x2840C888,0x2840C88C,0x2840C890,0x2840C894,
			0x2840C898,0x2840C89C,0x2840C8A0,0x2840C8A4,
			0x2840C8A8,0x2840C8AC,0x2840C8B0,0x2840C8B4,
			0x2840C8B8,0x2840C8BC,0x2840C8C0,0x2840C8C4,
			0x2840C900,0x2840C904,0x2840C908,0x2840C90C,
			0x2840C910,0x2840C914,0x2840C918,0x2840C91C,
			0x2840C920,0x2840C924,0x2840C928,0x2840C92C,
			0x2840C930,0x2840C934,0x2840C938,0x2840C93C,
			0x2840C940,0x2840C944,0x2840C980,0x2840C984,
			0x2840C988,0x2840C990,0x2840C994,0x2840C998,
			0x2840C9A0,0x2840C9A4,0x2840C9A8,0x2840C9AC,
			0x2840C9B0,0x2840C9C0,0x2840C9C4,0x2840C9C8,
			0x2840C9CC,0x2840C9D0,0x2840C9E0,0x2840C9E4,
			0x2840C9E8,0x2840C9EC,0x2840C9F0,0x2840CA00,
			0x2840CA04,0x2840CA08,0x2840CA0C,0x2840CA10,
			0x2840CA20,0x2840CA24,0x2840CA28,0x2840CA2C,
			0x2840CA30,0x2840CA40,0x2840CA44,0x2840CA48,
			0x2840CA4C,0x2840CA50,0x2840CA60,0x2840CA64,
			0x2840CA68,0x2840CA6C,0x2840CA70,0x2840CA80,
			0x2840CA84,0x2840CA88,0x2840CA8C,0x2840CA90,
			0x2840CAC0,0x2840CAC4,0x2840CAC8,0x2840CACC,
			0x2840CAD0,0x2840CAD4,0x2840CAD8,0x2840CADC,
			0x2840CAE0,0x2840CAE4,0x2840CAE8,0x2840CAEC,
			0x2840CAF0,0x2840CAF4,0x2840CAF8,0x2840CAFC,
			0x2840CB00,0x2840CB04,0x2840CB08,0x2840CB0C,
			0x2840CB10,0x2840CB14,0x2840CB18,0x2840CB1C,
			0x2840CB20,0x2840CB24,0x2840CB28,0x2840CB2C,
			0x2840CB30,0x2840CB34,0x2840CB38,0x2840CB3C,
			0x2840CB40,0x2840CB44,0x2840CB48,0x2840CB4C,
			0x2840CB50,0x2840CB54,0x2840CB58,0x2840CB5C,
			0x2840CB60,0x2840CB64,0x2840CB9C,0x2840CBA0,
		},
	},
	{
		{
			0x28508808,0x28508814,0x28508880,0x28508884,
			0x28508888,0x2850888C,0x28508890,0x28508894,
			0x28508898,0x2850889C,0x285088A0,0x285088A4,
			0x285088A8,0x285088AC,0x285088B0,0x285088B4,
			0x285088B8,0x285088BC,0x285088C0,0x285088C4,
			0x28508900,0x28508904,0x28508908,0x2850890C,
			0x28508910,0x28508914,0x28508918,0x2850891C,
			0x28508920,0x28508924,0x28508928,0x2850892C,
			0x28508930,0x28508934,0x28508938,0x2850893C,
			0x28508940,0x28508944,0x28508980,0x28508984,
			0x28508988,0x28508990,0x28508994,0x28508998,
			0x285089A0,0x285089A4,0x285089A8,0x285089AC,
			0x285089B0,0x285089C0,0x285089C4,0x285089C8,
			0x285089CC,0x285089D0,0x285089E0,0x285089E4,
			0x285089E8,0x285089EC,0x285089F0,0x28508A00,
			0x28508A04,0x28508A08,0x28508A0C,0x28508A10,
			0x28508A20,0x28508A24,0x28508A28,0x28508A2C,
			0x28508A30,0x28508A40,0x28508A44,0x28508A48,
			0x28508A4C,0x28508A50,0x28508A60,0x28508A64,
			0x28508A68,0x28508A6C,0x28508A70,0x28508A80,
			0x28508A84,0x28508A88,0x28508A8C,0x28508A90,
			0x28508AC0,0x28508AC4,0x28508AC8,0x28508ACC,
			0x28508AD0,0x28508AD4,0x28508AD8,0x28508ADC,
			0x28508AE0,0x28508AE4,0x28508AE8,0x28508AEC,
			0x28508AF0,0x28508AF4,0x28508AF8,0x28508AFC,
			0x28508B00,0x28508B04,0x28508B08,0x28508B0C,
			0x28508B10,0x28508B14,0x28508B18,0x28508B1C,
			0x28508B20,0x28508B24,0x28508B28,0x28508B2C,
			0x28508B30,0x28508B34,0x28508B38,0x28508B3C,
			0x28508B40,0x28508B44,0x28508B48,0x28508B4C,
			0x28508B50,0x28508B54,0x28508B58,0x28508B5C,
			0x28508B60,0x28508B64,0x28508B9C,0x28508BA0,
		},
		{
			0x2850C808,0x2850C814,0x2850C880,0x2850C884,
			0x2850C888,0x2850C88C,0x2850C890,0x2850C894,
			0x2850C898,0x2850C89C,0x2850C8A0,0x2850C8A4,
			0x2850C8A8,0x2850C8AC,0x2850C8B0,0x2850C8B4,
			0x2850C8B8,0x2850C8BC,0x2850C8C0,0x2850C8C4,
			0x2850C900,0x2850C904,0x2850C908,0x2850C90C,
			0x2850C910,0x2850C914,0x2850C918,0x2850C91C,
			0x2850C920,0x2850C924,0x2850C928,0x2850C92C,
			0x2850C930,0x2850C934,0x2850C938,0x2850C93C,
			0x2850C940,0x2850C944,0x2850C980,0x2850C984,
			0x2850C988,0x2850C990,0x2850C994,0x2850C998,
			0x2850C9A0,0x2850C9A4,0x2850C9A8,0x2850C9AC,
			0x2850C9B0,0x2850C9C0,0x2850C9C4,0x2850C9C8,
			0x2850C9CC,0x2850C9D0,0x2850C9E0,0x2850C9E4,
			0x2850C9E8,0x2850C9EC,0x2850C9F0,0x2850CA00,
			0x2850CA04,0x2850CA08,0x2850CA0C,0x2850CA10,
			0x2850CA20,0x2850CA24,0x2850CA28,0x2850CA2C,
			0x2850CA30,0x2850CA40,0x2850CA44,0x2850CA48,
			0x2850CA4C,0x2850CA50,0x2850CA60,0x2850CA64,
			0x2850CA68,0x2850CA6C,0x2850CA70,0x2850CA80,
			0x2850CA84,0x2850CA88,0x2850CA8C,0x2850CA90,
			0x2850CAC0,0x2850CAC4,0x2850CAC8,0x2850CACC,
			0x2850CAD0,0x2850CAD4,0x2850CAD8,0x2850CADC,
			0x2850CAE0,0x2850CAE4,0x2850CAE8,0x2850CAEC,
			0x2850CAF0,0x2850CAF4,0x2850CAF8,0x2850CAFC,
			0x2850CB00,0x2850CB04,0x2850CB08,0x2850CB0C,
			0x2850CB10,0x2850CB14,0x2850CB18,0x2850CB1C,
			0x2850CB20,0x2850CB24,0x2850CB28,0x2850CB2C,
			0x2850CB30,0x2850CB34,0x2850CB38,0x2850CB3C,
			0x2850CB40,0x2850CB44,0x2850CB48,0x2850CB4C,
			0x2850CB50,0x2850CB54,0x2850CB58,0x2850CB5C,
			0x2850CB60,0x2850CB64,0x2850CB9C,0x2850CBA0,
		},
	},
	{
		{
			0x28608808,0x28608814,0x28608880,0x28608884,
			0x28608888,0x2860888C,0x28608890,0x28608894,
			0x28608898,0x2860889C,0x286088A0,0x286088A4,
			0x286088A8,0x286088AC,0x286088B0,0x286088B4,
			0x286088B8,0x286088BC,0x286088C0,0x286088C4,
			0x28608900,0x28608904,0x28608908,0x2860890C,
			0x28608910,0x28608914,0x28608918,0x2860891C,
			0x28608920,0x28608924,0x28608928,0x2860892C,
			0x28608930,0x28608934,0x28608938,0x2860893C,
			0x28608940,0x28608944,0x28608980,0x28608984,
			0x28608988,0x28608990,0x28608994,0x28608998,
			0x286089A0,0x286089A4,0x286089A8,0x286089AC,
			0x286089B0,0x286089C0,0x286089C4,0x286089C8,
			0x286089CC,0x286089D0,0x286089E0,0x286089E4,
			0x286089E8,0x286089EC,0x286089F0,0x28608A00,
			0x28608A04,0x28608A08,0x28608A0C,0x28608A10,
			0x28608A20,0x28608A24,0x28608A28,0x28608A2C,
			0x28608A30,0x28608A40,0x28608A44,0x28608A48,
			0x28608A4C,0x28608A50,0x28608A60,0x28608A64,
			0x28608A68,0x28608A6C,0x28608A70,0x28608A80,
			0x28608A84,0x28608A88,0x28608A8C,0x28608A90,
			0x28608AC0,0x28608AC4,0x28608AC8,0x28608ACC,
			0x28608AD0,0x28608AD4,0x28608AD8,0x28608ADC,
			0x28608AE0,0x28608AE4,0x28608AE8,0x28608AEC,
			0x28608AF0,0x28608AF4,0x28608AF8,0x28608AFC,
			0x28608B00,0x28608B04,0x28608B08,0x28608B0C,
			0x28608B10,0x28608B14,0x28608B18,0x28608B1C,
			0x28608B20,0x28608B24,0x28608B28,0x28608B2C,
			0x28608B30,0x28608B34,0x28608B38,0x28608B3C,
			0x28608B40,0x28608B44,0x28608B48,0x28608B4C,
			0x28608B50,0x28608B54,0x28608B58,0x28608B5C,
			0x28608B60,0x28608B64,0x28608B9C,0x28608BA0,
		},
		{
			0x2860C808,0x2860C814,0x2860C880,0x2860C884,
			0x2860C888,0x2860C88C,0x2860C890,0x2860C894,
			0x2860C898,0x2860C89C,0x2860C8A0,0x2860C8A4,
			0x2860C8A8,0x2860C8AC,0x2860C8B0,0x2860C8B4,
			0x2860C8B8,0x2860C8BC,0x2860C8C0,0x2860C8C4,
			0x2860C900,0x2860C904,0x2860C908,0x2860C90C,
			0x2860C910,0x2860C914,0x2860C918,0x2860C91C,
			0x2860C920,0x2860C924,0x2860C928,0x2860C92C,
			0x2860C930,0x2860C934,0x2860C938,0x2860C93C,
			0x2860C940,0x2860C944,0x2860C980,0x2860C984,
			0x2860C988,0x2860C990,0x2860C994,0x2860C998,
			0x2860C9A0,0x2860C9A4,0x2860C9A8,0x2860C9AC,
			0x2860C9B0,0x2860C9C0,0x2860C9C4,0x2860C9C8,
			0x2860C9CC,0x2860C9D0,0x2860C9E0,0x2860C9E4,
			0x2860C9E8,0x2860C9EC,0x2860C9F0,0x2860CA00,
			0x2860CA04,0x2860CA08,0x2860CA0C,0x2860CA10,
			0x2860CA20,0x2860CA24,0x2860CA28,0x2860CA2C,
			0x2860CA30,0x2860CA40,0x2860CA44,0x2860CA48,
			0x2860CA4C,0x2860CA50,0x2860CA60,0x2860CA64,
			0x2860CA68,0x2860CA6C,0x2860CA70,0x2860CA80,
			0x2860CA84,0x2860CA88,0x2860CA8C,0x2860CA90,
			0x2860CAC0,0x2860CAC4,0x2860CAC8,0x2860CACC,
			0x2860CAD0,0x2860CAD4,0x2860CAD8,0x2860CADC,
			0x2860CAE0,0x2860CAE4,0x2860CAE8,0x2860CAEC,
			0x2860CAF0,0x2860CAF4,0x2860CAF8,0x2860CAFC,
			0x2860CB00,0x2860CB04,0x2860CB08,0x2860CB0C,
			0x2860CB10,0x2860CB14,0x2860CB18,0x2860CB1C,
			0x2860CB20,0x2860CB24,0x2860CB28,0x2860CB2C,
			0x2860CB30,0x2860CB34,0x2860CB38,0x2860CB3C,
			0x2860CB40,0x2860CB44,0x2860CB48,0x2860CB4C,
			0x2860CB50,0x2860CB54,0x2860CB58,0x2860CB5C,
			0x2860CB60,0x2860CB64,0x2860CB9C,0x2860CBA0,
		},
	},
};

static const T_IM_PRO_RDMA_DPC_ADDR gIM_PRO_DPC_Addr[E_IM_PRO_UNIT_NUM_MAX] = {
	{
		0x28409118,0x2840911C,0x28409120,0x28409124,
		0x28409130,0x28409134,0x28409138,0x28409140,
		0x28409144,0x28409148,0x28409150,0x28409154,
		0x28409158,0x28409160,0x28409164,0x28409168,
		0x28409170,0x28409178,0x2840917C,0x28409180,
		0x28409184,0x28409188,0x2840918C,0x28409190,
		0x28409194,0x28409198,0x2840919C,0x284091A0,
		0x284091A4,0x284091A8,0x284091AC,0x284091B0,
		0x284091B4,0x284091B8,0x284091BC,0x284091C0,
		0x284091C4,
	},
	{
		0x28509118,0x2850911C,0x28509120,0x28509124,
		0x28509130,0x28509134,0x28509138,0x28509140,
		0x28509144,0x28509148,0x28509150,0x28509154,
		0x28509158,0x28509160,0x28509164,0x28509168,
		0x28509170,0x28509178,0x2850917C,0x28509180,
		0x28509184,0x28509188,0x2850918C,0x28509190,
		0x28509194,0x28509198,0x2850919C,0x285091A0,
		0x285091A4,0x285091A8,0x285091AC,0x285091B0,
		0x285091B4,0x285091B8,0x285091BC,0x285091C0,
		0x285091C4,
	},
	{
		0x28609118,0x2860911C,0x28609120,0x28609124,
		0x28609130,0x28609134,0x28609138,0x28609140,
		0x28609144,0x28609148,0x28609150,0x28609154,
		0x28609158,0x28609160,0x28609164,0x28609168,
		0x28609170,0x28609178,0x2860917C,0x28609180,
		0x28609184,0x28609188,0x2860918C,0x28609190,
		0x28609194,0x28609198,0x2860919C,0x286091A0,
		0x286091A4,0x286091A8,0x286091AC,0x286091B0,
		0x286091B4,0x286091B8,0x286091BC,0x286091C0,
		0x286091C4,
	},
};

static const T_IM_PRO_RDMA_FSHDL_ADDR gIM_PRO_FSHDL_Addr[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_FSHDL_CH_NUM] = {
	{
		{
			0x2840931C,0x28409320,0x28409324,0x28409328,
			0x28409330,0x28409334,0x28409338,0x2840933C,
			0x28409340,0x28409344,0x28409350,0x28409354,
			0x28409358,
		},
		{
			0x28409A1C,0x28409A20,0x28409A24,0x28409A28,
			0x28409A30,0x28409A34,0x28409A38,0x28409A3C,
			0x28409A40,0x28409A44,0x28409A50,0x28409A54,
			0x28409A58,
		},
	},
	{
		{
			0x2850931C,0x28509320,0x28509324,0x28509328,
			0x28509330,0x28509334,0x28509338,0x2850933C,
			0x28509340,0x28509344,0x28509350,0x28509354,
			0x28509358,
		},
		{
			0x28509A1C,0x28509A20,0x28509A24,0x28509A28,
			0x28509A30,0x28509A34,0x28509A38,0x28509A3C,
			0x28509A40,0x28509A44,0x28509A50,0x28509A54,
			0x28509A58,
		},
	},
	{
		{
			0x2860931C,0x28609320,0x28609324,0x28609328,
			0x28609330,0x28609334,0x28609338,0x2860933C,
			0x28609340,0x28609344,0x28609350,0x28609354,
			0x28609358,
		},
		{
			0x28609A1C,0x28609A20,0x28609A24,0x28609A28,
			0x28609A30,0x28609A34,0x28609A38,0x28609A3C,
			0x28609A40,0x28609A44,0x28609A50,0x28609A54,
			0x28609A58,
		},
	},
};

static const T_IM_PRO_RDMA_FFC_ADDR gIM_PRO_FFC_Addr[E_IM_PRO_UNIT_NUM_MAX] = {
	{
		0x2840971C,0x28409734,0x28409738,0x2840973C,
		0x28409740,
	},
	{
		0x2850971C,0x28509734,0x28509738,0x2850973C,
		0x28509740,
	},
	{
		0x2860971C,0x28609734,0x28609738,0x2860973C,
		0x28609740,
	},
};

static const T_IM_PRO_RDMA_CAG_ADDR gIM_PRO_CAG_Addr[E_IM_PRO_UNIT_NUM_MAX] = {
	{
		0x28409818,0x2840981C,0x28409820,0x28409824,
		0x28409828,0x2840982C,0x28409830,0x28409834,
		0x28409838,0x2840983C,0x28409840,0x28409844,
		0x28409848,0x2840984C,0x28409850,0x28409854,
		0x28409858,0x2840985C,0x28409860,0x28409864,
		0x28409868,0x2840986C,0x28409870,0x28409874,
		0x28409878,0x2840987C,0x28409880,0x28409884,
		0x28409888,0x2840988C,0x28409890,0x28409894,
		0x28409898,0x2840989C,0x284098A0,0x284098A4,
		0x284098A8,0x284098AC,0x284098B0,0x284098B4,
		0x284098B8,0x284098BC,0x284098C0,0x284098C4,
		0x284098C8,0x284098CC,0x284098D0,0x284098D4,
		0x284098D8
	},
	{
		0x28509818,0x2850981C,0x28509820,0x28509824,
		0x28509828,0x2850982C,0x28509830,0x28509834,
		0x28509838,0x2850983C,0x28509840,0x28509844,
		0x28509848,0x2850984C,0x28509850,0x28509854,
		0x28509858,0x2850985C,0x28509860,0x28509864,
		0x28509868,0x2850986C,0x28509870,0x28509874,
		0x28509878,0x2850987C,0x28509880,0x28509884,
		0x28509888,0x2850988C,0x28509890,0x28509894,
		0x28509898,0x2850989C,0x285098A0,0x285098A4,
		0x285098A8,0x285098AC,0x285098B0,0x285098B4,
		0x285098B8,0x285098BC,0x285098C0,0x285098C4,
		0x285098C8,0x285098CC,0x285098D0,0x285098D4,
		0x285098D8
	},
	{
		0x28609818,0x2860981C,0x28609820,0x28609824,
		0x28609828,0x2860982C,0x28609830,0x28609834,
		0x28609838,0x2860983C,0x28609840,0x28609844,
		0x28609848,0x2860984C,0x28609850,0x28609854,
		0x28609858,0x2860985C,0x28609860,0x28609864,
		0x28609868,0x2860986C,0x28609870,0x28609874,
		0x28609878,0x2860987C,0x28609880,0x28609884,
		0x28609888,0x2860988C,0x28609890,0x28609894,
		0x28609898,0x2860989C,0x286098A0,0x286098A4,
		0x286098A8,0x286098AC,0x286098B0,0x286098B4,
		0x286098B8,0x286098BC,0x286098C0,0x286098C4,
		0x286098C8,0x286098CC,0x286098D0,0x286098D4,
		0x286098D8
	},
};

static const T_IM_PRO_RDMA_SDC_GAIN_TBL_ADDR gIM_PRO_SDC_Gain_Tbl_Addr[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_SDC_CH_NUM][D_IM_PRO_SRO_SDC_TBL_CNT] = {
	{
		{
			{ 0x29440000 },
			{ 0x29440100 },
		},
		{
			{ 0x29440200},
			{ 0x29440300},
		},
	},
	{
		{
			{ 0x29540000 },
			{ 0x29540100 },
		},
		{
			{ 0x29540200},
			{ 0x29540300},
		},
	},
	{
		{
			{ 0x29640000 },
			{ 0x29640100 },
		},
		{
			{ 0x29640200},
			{ 0x29640300},
		},
	},
};

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static	volatile	ULONG	gIM_SRO_START_Status[3] = {0x00000000};

static const UINT32	gIM_PRO_AFPB_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_AFPB_CH_NUM] = {
	// E_IM_PRO_UNIT_NUM_1
	{
		D_IM_SRO1_STATUS_AFPB_A,		D_IM_SRO1_STATUS_AFPB_G,
	},
	// E_IM_PRO_UNIT_NUM_2
	{
		D_IM_SRO2_STATUS_AFPB_A,		D_IM_SRO2_STATUS_AFPB_G,
	},
	// E_IM_PRO_BOTH_UNIT
	{
		D_IM_SRO_STATUS_AFPB_A_BOTH,	D_IM_SRO_STATUS_AFPB_G_BOTH,
	},
};

static const T_IM_PRO_SRO_FSHDL_INFO	gIM_PRO_FSHDL_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_FSHDL_CH_NUM] = {
	// E_IM_PRO_UNIT_NUM_1
	{
		{ &IO_PRO.IMG_PIPE[0].SRO.FSHDL0,	D_IM_SRO1_STATUS_FSHDL0	},
		{ &IO_PRO.IMG_PIPE[0].SRO.FSHDL1,	D_IM_SRO1_STATUS_FSHDL1	},
	},
	// E_IM_PRO_UNIT_NUM_2
	{
		{ &IO_PRO.IMG_PIPE[1].SRO.FSHDL0,	D_IM_SRO2_STATUS_FSHDL0	},
		{ &IO_PRO.IMG_PIPE[1].SRO.FSHDL1,	D_IM_SRO2_STATUS_FSHDL1	},
	},
	// E_IM_PRO_BOTH_UNIT
	{
		{ &IO_PRO.IMG_PIPE[2].SRO.FSHDL0,	D_IM_SRO_STATUS_FSHDL0_BOTH	},
		{ &IO_PRO.IMG_PIPE[2].SRO.FSHDL1,	D_IM_SRO_STATUS_FSHDL1_BOTH	},
	},
};

static const UINT32	gIM_PRO_CAG_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	D_IM_SRO1_STATUS_CAG,	D_IM_SRO2_STATUS_CAG,	D_IM_SRO_STATUS_CAG_BOTH,
};

static const UINT32	gIM_PRO_DPC_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	D_IM_SRO1_STATUS_DPC,	D_IM_SRO2_STATUS_DPC,	D_IM_SRO_STATUS_DPC_BOTH,
};

static const UINT32	gIM_PRO_FFC_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	D_IM_SRO1_STATUS_FFC,	D_IM_SRO2_STATUS_FFC,	D_IM_SRO_STATUS_FFC_BOTH,
};

static const UINT32	gIM_PRO_HRB_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	D_IM_SRO1_STATUS_HRB,	D_IM_SRO2_STATUS_HRB,	D_IM_SRO_STATUS_HRB_BOTH,
};

static const UINT32	gIM_PRO_VRB_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	D_IM_SRO1_STATUS_VRB,	D_IM_SRO2_STATUS_VRB,	D_IM_SRO_STATUS_VRB_BOTH,
};

static const UINT32	gIM_PRO_FFSH_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_FFSH_CH_NUM] = {
	// E_IM_PRO_UNIT_NUM_1
	{
		D_IM_SRO1_STATUS_FFSH0,		D_IM_SRO1_STATUS_FFSH1,
	},
	// E_IM_PRO_UNIT_NUM_2
	{
		D_IM_SRO2_STATUS_FFSH0,		D_IM_SRO2_STATUS_FFSH1,
	},
	// E_IM_PRO_BOTH_UNIT
	{
		D_IM_SRO_STATUS_FFSH0_BOTH,	D_IM_SRO_STATUS_FFSH1_BOTH,
	},
};

static const UINT32	gIM_PRO_FF_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_FF_CH_NUM] = {
	// E_IM_PRO_UNIT_NUM_1
	{
		D_IM_SRO1_STATUS_FF0,		D_IM_SRO1_STATUS_FF1,
	},
	// E_IM_PRO_UNIT_NUM_2
	{
		D_IM_SRO2_STATUS_FF0,		D_IM_SRO2_STATUS_FF1,
	},
	// E_IM_PRO_BOTH_UNIT
	{
		D_IM_SRO_STATUS_FF0_BOTH,	D_IM_SRO_STATUS_FF1_BOTH,
	},
};

static const T_IM_PRO_SRO_SDC_INFO	gIM_PRO_SDC_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_SRO_SDC_CH_NUM] = {
	// E_IM_PRO_UNIT_NUM_1
	{
		{ &IO_PRO_TBL.SRO1_TBL.SDCGD[0][0],	D_IM_SRO1_STATUS_SDC0	},
		{ &IO_PRO_TBL.SRO1_TBL.SDCGD[1][0],	D_IM_SRO1_STATUS_SDC1	},
	},
	// E_IM_PRO_UNIT_NUM_2
	{
		{ &IO_PRO_TBL.SRO2_TBL.SDCGD[0][0],	D_IM_SRO2_STATUS_SDC0	},
		{ &IO_PRO_TBL.SRO2_TBL.SDCGD[1][0],	D_IM_SRO2_STATUS_SDC1	},
	},
	// E_IM_PRO_BOTH_UNIT
	{
		{ &IO_PRO_TBL.SRO3_TBL.SDCGD[0][0],	D_IM_SRO_STATUS_SDC0_BOTH	},
		{ &IO_PRO_TBL.SRO3_TBL.SDCGD[1][0],	D_IM_SRO_STATUS_SDC1_BOTH	},
	},
};


/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
VOID im_pro_comm_get_fshdl_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, const T_IM_PRO_SRO_FSHDL_INFO** fshdl_info )
{
	*fshdl_info = &gIM_PRO_FSHDL_Status_Tbl[unit_no][ch];
}

VOID im_pro_comm_get_sdc_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, const T_IM_PRO_SRO_SDC_INFO** sdc_info )
{
	*sdc_info = &gIM_PRO_SDC_Status_Tbl[unit_no][ch];
}


/********************************************************/
/*	SRO Top												*/
/********************************************************/
/**
SROTOP Macro initialize
@param[in]	unit_no : Unit number.
@remarks	Please Call at the time of system starting.
*/
VOID Im_PRO_SROTOP_Init( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	// Software release
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SR.bit.SR = D_IM_PRO_SR_RELEASE;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock
}

/**
SROTOP Macro software reset
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
INT32 Im_PRO_SROTOP_SW_Reset( E_IM_PRO_UNIT_NUM unit_no )
{
#ifdef CO_PARAM_CHECK
	if (im_pro_sro_get_stop_all() == FALSE){	//not ALL Stopped
		Ddim_Print(("I:Im_PRO_SROTOP_SW_Reset. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif	/* CO_ACT_PRO_CLOCK */

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock

	// Software reset
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SR.bit.SR = D_IM_PRO_SR_RESET;

	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
SROTOP Macro clock control
@param[in]	unit_no : Unit number.
@param[in]	clk_type	: SROTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SROTOP_Control_Clock( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SROTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip )
{
	UINT32 cpu_clk_hz;
	UINT32 macro_clk_hz;
	E_IM_PRO_CLK_BLOCK_TYPE clk_block_type;
#ifdef CO_PARAM_CHECK
	if (on_off > D_IM_PRO_CLOCK_OFF){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Control_Clock error. on_off value over!! (%d)\n", on_off));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else {
		// DO NOTHING
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	switch( clk_type ) {
		case E_IM_PRO_SROTOP_CLK_TYPE_SROCLK:
			IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.CLKSTP.bit.PSTP1 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			clk_block_type = E_IM_PRO_CLK_BLOCK_TYPE_SRO;
			break;

		//case E_IM_PRO_SROTOP_CLK_TYPE_SROCLK2:
		default:
			IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.CLKSTP.bit.PSTP2 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			clk_block_type = E_IM_PRO_CLK_BLOCK_TYPE_SRO2;
			break;
	}
	// waiting for 5 cycle
	if( wait_skip == 0 ) {
		im_pro_get_current_clk_hz( unit_no, clk_block_type, &cpu_clk_hz, &macro_clk_hz );
		im_pro_common_wait_by_clk_cycle( 5, macro_clk_hz, cpu_clk_hz );
#ifndef CO_DEBUG_ON_PC
//			__nop();
#endif
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
SROTOP macro control data setting
@param[in]	unit_no : Unit number.
@param[in]	ctrl : SROTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SROTOP_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_SROTOP_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SROTOPCTL1.bit.SROISW	= ctrl->input_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SROTOPCTL1.bit.SROMD	= ctrl->sro_mode_sel;
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SROTOPCTL1.bit.CAGSL	= ctrl->cag_target_sel;
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SROTOPCTL1.bit.BRPSW0	= ctrl->bay_out_path_sel;
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SROTOPCTL1.bit.BRPSW1	= ctrl->b2b_connect_path_sel;
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SROTOPCTL1.bit.BRPSW2	= ctrl->ltm_connect_path_sel;
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SROTOPCTL2.bit.ORG0		= ctrl->first_pixel[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.SROTOPCTL2.bit.ORG1		= ctrl->first_pixel[1];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
WEITGEN macro control data setting
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	weitgen_ctrl : WEITGEN macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SROTOP_WEITGEN_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_WEITGEN_CH ch, T_IM_PRO_SROTOP_WEITGEN_CTRL* weitgen_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (weitgen_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SROTOP_WEITGEN_Ctrl error. weitgen_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SROTOP_GENV_MIN, D_IM_PRO_SROTOP_GENV_MAX, weitgen_ctrl->v_wait_signal_pos, "Im_PRO_SROTOP_WEITGEN_Ctrl : v_wait_signal_pos" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SROTOP_GENH_MIN, D_IM_PRO_SROTOP_GENH_MAX, weitgen_ctrl->h_wait_signal_pos, "Im_PRO_SROTOP_WEITGEN_Ctrl : h_wait_signal_pos" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if( ch == E_IM_PRO_WEITGEN_CH0 ) {
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.WAITGENV.bit.GENV0	= weitgen_ctrl->v_wait_signal_pos;
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.WAITGENH.bit.GENH0	= weitgen_ctrl->h_wait_signal_pos;
	}
	else  if( ch == E_IM_PRO_WEITGEN_CH1 ){
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.WAITGENV.bit.GENV1	= weitgen_ctrl->v_wait_signal_pos;
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.WAITGENH.bit.GENH1	= weitgen_ctrl->h_wait_signal_pos;
	}
	else {
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
setup of enable to WEITGEN macro.
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	enabled : setup of enable to WEITGEN macro.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SROTOP_WEITGEN_Enable( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_WEITGEN_CH ch, E_IM_PRO_WEITGEN_ENABLE enabled )
{
#ifdef CO_PARAM_CHECK
	if ( (im_pro_sro_get_stop_all() == FALSE) || (im_pro_sen_get_stop_all() == FALSE)){	//not ALL Stopped
		Ddim_Print(("I:Im_PRO_SROTOP_WEITGEN_Enable. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if( ch == E_IM_PRO_WEITGEN_CH0 ) {
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.WAITGENEN.bit.GENEN0	= enabled;
	}
	else  if( ch == E_IM_PRO_WEITGEN_CH1 ){
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.WAITGENEN.bit.GENEN1	= enabled;
	}
	else {
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Start AFPB
@param[in]	unit_no : Unit number.
*/
VOID Im_PRO_AFPB_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_AFPB_CH ch )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.AFPB[ch].AFPBTRG.bit.AFPBTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_AFPB_Status_Tbl[unit_no][ch], 0);
}

/**
Stop AFPB
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
VOID Im_PRO_AFPB_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_AFPB_CH ch, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock

	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.AFPB[ch].AFPBTRG.bit.AFPBTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.AFPB[ch].AFPBTRG.bit.AFPBTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_AFPB_Status_Tbl[unit_no][ch], 0);
}

/**
Set AFPB control information
@param[in]	unit_no : Unit number.
@param[in]	afpb_ctrl	: AFPB control Information.
*/
INT32 Im_PRO_AFPB_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_AFPB_CH ch, T_IM_PRO_AFPB_CTRL* afpb_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (afpb_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_AFPB_Ctrl error. afpb_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AFPB_AFPBH_MIN, D_IM_PRO_AFPB_AFPBH_MAX, afpb_ctrl->afpb_area.pos_x, "Im_PRO_AFPB_Ctrl : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AFPB_AFPBV_MIN, D_IM_PRO_AFPB_AFPBV_MAX, afpb_ctrl->afpb_area.pos_y, "Im_PRO_AFPB_Ctrl : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AFPB_AFPBHW_MIN, D_IM_PRO_AFPB_AFPBHW_MAX, afpb_ctrl->afpb_area.width, "Im_PRO_AFPB_Ctrl : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AFPB_AFPBVW_MIN, D_IM_PRO_AFPB_AFPBVW_MAX, afpb_ctrl->afpb_area.lines, "Im_PRO_AFPB_Ctrl : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.AFPB[ch].AFPBH.bit.AFPBH	= afpb_ctrl->afpb_area.pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.AFPB[ch].AFPBV.bit.AFPBV	= afpb_ctrl->afpb_area.pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.AFPB[ch].AFPBHW.bit.AFPBHW	= afpb_ctrl->afpb_area.width;
	IO_PRO.IMG_PIPE[unit_no].SRO.AFPB[ch].AFPBVW.bit.AFPBVW	= afpb_ctrl->afpb_area.lines;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Flat Shading Start
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not running NG
@remarks	Flat Shading Compensation function is started(enable).
*/
INT32 Im_PRO_FSHDL_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHTRG.bit.FFSHTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].status, 0);

	return D_DDIM_OK;
}

/**
Flat Shading Stop
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not stopped NG
@remarks	Flat Shading Compensation function is stopped(disable).
*/
INT32 Im_PRO_FSHDL_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHTRG.bit.FFSHTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHTRG.bit.FFSHTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].status, 0);

	return D_DDIM_OK;
}

/**
Flat Shading Control
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	fshdl_ctrl : Shading Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHDL_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, T_IM_PRO_FSHDL_COMMON_CTRL* fshdl_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (fshdl_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHDL_Ctrl error. fshdl_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHULRR_MIN, D_IM_PRO_FSHDL_FFSHULRR_MAX, fshdl_ctrl->clip_r, "Im_PRO_FSHDL_Ctrl : clip_r" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHULGR_MIN, D_IM_PRO_FSHDL_FFSHULGR_MAX, fshdl_ctrl->clip_gr, "Im_PRO_FSHDL_Ctrl : clip_gr" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHULGB_MIN, D_IM_PRO_FSHDL_FFSHULGB_MAX, fshdl_ctrl->clip_gb, "Im_PRO_FSHDL_Ctrl : clip_gb" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHULBB_MIN, D_IM_PRO_FSHDL_FFSHULBB_MAX, fshdl_ctrl->clip_b, "Im_PRO_FSHDL_Ctrl : clip_b" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHCTL.bit.FFSHPTMD		= fshdl_ctrl->montage_mode;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHUL1.bit.FFSHULRR		= fshdl_ctrl->clip_r;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHUL1.bit.FFSHULGR		= fshdl_ctrl->clip_gr;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHUL2.bit.FFSHULGB		= fshdl_ctrl->clip_gb;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHUL2.bit.FFSHULBB		= fshdl_ctrl->clip_b;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
Flat Shading Blend ratio Control
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	fshdl_ctrl : Shading Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHDL_Set_Blend( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, T_IM_PRO_FSHDL_BLEND_CTRL* blend_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (blend_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHDL_Set_Blend error. blend_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHBC, union io_fshdlbc, FFSHBC0, blend_ctrl->shading_blend_0 );
	im_pro_set_reg_signed( gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHBC, union io_fshdlbc, FFSHBC1, blend_ctrl->shading_blend_1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of FSHDL.
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FSHDL_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHTRG.bit.FFSHTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_FSHDL_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHPAEN.bit.FFSHPAEN	= paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A Setup is Flat Shading Area
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	fshdl_area : Area for Shading compensation See @ref T_IM_PRO_AREA_INFO<br>
 												value range  :pos_x[0 - 4095]<br>
 												target registor  :@@FFSHH<br>
 												value range  :pos_y[2 - 8960] 2line boundary<br>
 												target registor  :@@FFSHV<br>
 												value range  :width[4 - 2496] 4pixel boundary<br>
 												target registor  :@@FFSHHW<br>
 												value range  :lines[4 - 8960] 4pixel boundary<br>
 												target registor  :@@FFSHVW
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHDL_Set_Area( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, T_IM_PRO_AREA_INFO* fshdl_area )
{
#ifdef CO_PARAM_CHECK
	if (fshdl_area == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHDL_Set_Area error. fshdl_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHH_MIN, D_IM_PRO_FSHDL_FFSHH_MAX, fshdl_area->pos_x, "Im_PRO_FSHDL_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHV_MIN, D_IM_PRO_FSHDL_FFSHV_MAX, fshdl_area->pos_y, "Im_PRO_FSHDL_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHHW_MIN, D_IM_PRO_FSHDL_FFSHHW_MAX, fshdl_area->width, "Im_PRO_FSHDL_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHVW_MIN, D_IM_PRO_FSHDL_FFSHVW_MAX, fshdl_area->lines, "Im_PRO_FSHDL_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHV.bit.FFSHV			= fshdl_area->pos_y;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHH.bit.FFSHH			= fshdl_area->pos_x;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHVW.bit.FFSHVW		= fshdl_area->lines;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHHW.bit.FFSHHW		= fshdl_area->width;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Flat Shading (Frame Correction) Control 
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	FSHDL_ctrl : Frame Correction Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHDL_Set_Frame( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, T_IM_PRO_FSHDL_FRAME_CTRL* fshdl_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (fshdl_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHDL_Set_Frame error. FSHDL_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHSUBPRH_MIN, D_IM_PRO_FSHDL_FFSHSUBPRH_MAX, fshdl_ctrl->frame_horizontal_size, "Im_PRO_FSHDL_Set_Frame : frame_horizontal_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHSUBPRV_MIN, D_IM_PRO_FSHDL_FFSHSUBPRV_MAX, fshdl_ctrl->frame_vertical_size, "Im_PRO_FSHDL_Set_Frame : frame_vertical_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHSUBSPH_MIN, D_IM_PRO_FSHDL_FFSHSUBSPH_MAX, fshdl_ctrl->horizontal_pixel, "Im_PRO_FSHDL_Set_Frame : horizontal_pixel" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHSUBSPV_MIN, D_IM_PRO_FSHDL_FFSHSUBSPV_MAX, fshdl_ctrl->vertical_pixel, "Im_PRO_FSHDL_Set_Frame : vertical_pixel" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHSUBSTH_MIN, D_IM_PRO_FSHDL_FFSHSUBSTH_MAX, fshdl_ctrl->start_pos_h, "Im_PRO_FSHDL_Set_Frame : start_pos_h" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHSUBSTV_MIN, D_IM_PRO_FSHDL_FFSHSUBSTV_MAX, fshdl_ctrl->start_pos_v, "Im_PRO_FSHDL_Set_Frame : start_pos_v" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHHWPH_MIN, D_IM_PRO_FSHDL_FFSHHWPH_MAX, fshdl_ctrl->montage_pos_h, "Im_PRO_FSHDL_Set_Frame : montage_pos_h" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHGFMT.bit.FFSHGFMT	= fshdl_ctrl->select_format;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBPR.bit.FFSHSUBPRH	= fshdl_ctrl->frame_horizontal_size;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBPR.bit.FFSHSUBPRV	= fshdl_ctrl->frame_vertical_size;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBSP.bit.FFSHSUBSPH	= fshdl_ctrl->horizontal_pixel;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBSP.bit.FFSHSUBSPV	= fshdl_ctrl->vertical_pixel;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBDR.bit.FFSHSUBDRM	= fshdl_ctrl->Inverse_base;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBDR.bit.FFSHSUBDRE	= fshdl_ctrl->Inverse_exponent;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBST.bit.FFSHSUBSTH	= fshdl_ctrl->start_pos_h;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBST.bit.FFSHSUBSTV	= fshdl_ctrl->start_pos_v;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHHWPH.bit.FFSHHWPH	= fshdl_ctrl->montage_pos_h;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHCCTL.bit.FFSHCEN		= fshdl_ctrl->sat_correction_enabled;
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHLP1.bit.FFSHLPRR		= fshdl_ctrl->sat_slope_gain[0];
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHLP1.bit.FFSHLPGR		= fshdl_ctrl->sat_slope_gain[1];
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHLP2.bit.FFSHLPGB		= fshdl_ctrl->sat_slope_gain[2];
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHLP2.bit.FFSHLPBB		= fshdl_ctrl->sat_slope_gain[3];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
set the request issue interval when reading the coefficients from PRCH/M2P macro.
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	req_interval_clk : RAM access control<br>
			value range :[4~1023]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FSHDL_Set_Coeff_Read_Line_Wait( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, USHORT req_interval_clk )
{
#ifdef CO_PARAM_CHECK
	if( im_pro_check_val_range( D_IM_PRO_FSHDL_FFSHWRIC_MIN, D_IM_PRO_FSHDL_FFSHWRIC_MAX, req_interval_clk, "Im_PRO_FSHDL_Set_Coeff_Read_Line_Wait : req_interval_clk" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( req_interval_clk >= ((gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHHW.bit.FFSHHW - (4*gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHSUBPR.bit.FFSHSUBPRH))-10)/3 ) {
		Ddim_Assertion(("I:Im_PRO_FSHDL_Set_Coeff_Read_Line_Wait. parameter error. Please satisfy following relationship. (req_interval_clk  < ((FFSHHW-(4*FFSHSUBPRH)) -10)/3) (%u)\n", req_interval_clk));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	gIM_PRO_FSHDL_Status_Tbl[unit_no][ch].reg_ptr->FFSHWRIC.bit.FFSHWRIC	= req_interval_clk;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of FSHDL control.
@param[in]		unit_no : Unit number.
@param[in]		ch : Channel No
@param[out]		addr					: Top address of the address array of FSHDL control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_FSHDL_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, const T_IM_PRO_RDMA_FSHDL_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_FSHDL_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_PRO_FSHDL_Addr[unit_no][ch]);

	return D_DDIM_OK;
}

/**
Start CAG
@param[in]	unit_no : Unit number.
*/
VOID Im_PRO_CAG_Start( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGTRG.bit.CAGTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_CAG_Status_Tbl[unit_no], 0);
}

/**
Stop CAG
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
VOID Im_PRO_CAG_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGTRG.bit.CAGTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGTRG.bit.CAGTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_CAG_Status_Tbl[unit_no], 0);
}


/**
Set CAG control information
@param[in]	unit_no : Unit number.
@param[in]	cag_ctrl	: CAG control Information.
*/
INT32 Im_PRO_CAG_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CAG_CTRL* cag_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (cag_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_CAG_Ctrl error. cag_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.RBR.bit.RBRV			= cag_ctrl->v_binning_ratio_reciprocal_num;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.RBR.bit.RBRH			= cag_ctrl->h_binning_ratio_reciprocal_num;
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOARH, union io_aboarh, ABOARH, cag_ctrl->opt_center_pos_h[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOAGH, union io_aboagh, ABOAGH, cag_ctrl->opt_center_pos_h[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOABH, union io_aboabh, ABOABH, cag_ctrl->opt_center_pos_h[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOARV, union io_aboarv, ABOARV, cag_ctrl->opt_center_pos_v[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOAGV, union io_aboagv, ABOAGV, cag_ctrl->opt_center_pos_v[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOABV, union io_aboabv, ABOABV, cag_ctrl->opt_center_pos_v[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABGAR, union io_abgar,	ABGAR,	 cag_ctrl->gain[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABGAG, union io_abgag,	ABGAG,	 cag_ctrl->gain[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABGAB, union io_abgab,	ABGAB,	 cag_ctrl->gain[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLRH, union io_ablrh,	ABLRHU,	 cag_ctrl->transfer_u_limit_h[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLGH, union io_ablgh,	ABLGHU,	 cag_ctrl->transfer_u_limit_h[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLBH, union io_ablbh,	ABLBHU,	 cag_ctrl->transfer_u_limit_h[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLRH, union io_ablrh,	ABLRHL,	 cag_ctrl->transfer_l_limit_h[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLGH, union io_ablgh,	ABLGHL,	 cag_ctrl->transfer_l_limit_h[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLBH, union io_ablbh,	ABLBHL,	 cag_ctrl->transfer_l_limit_h[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLRV, union io_ablrv,	ABLRVU,	 cag_ctrl->transfer_u_limit_v[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLGV, union io_ablgv,	ABLGVU,	 cag_ctrl->transfer_u_limit_v[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLBV, union io_ablbv,	ABLBVU,	 cag_ctrl->transfer_u_limit_v[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLRV, union io_ablrv,	ABLRVL,	 cag_ctrl->transfer_l_limit_v[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLGV, union io_ablgv,	ABLGVL,	 cag_ctrl->transfer_l_limit_v[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLBV, union io_ablbv,	ABLBVL,	 cag_ctrl->transfer_l_limit_v[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSRH, union io_abofsrh, ABOFSRH, cag_ctrl->transfer_offset_h[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSGH, union io_abofsgh, ABOFSGH, cag_ctrl->transfer_offset_h[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSBH, union io_abofsbh, ABOFSBH, cag_ctrl->transfer_offset_h[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSRV, union io_abofsrv, ABOFSRV, cag_ctrl->transfer_offset_v[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSGV, union io_abofsgv, ABOFSGV, cag_ctrl->transfer_offset_v[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSBV, union io_abofsbv, ABOFSBV, cag_ctrl->transfer_offset_v[2] );
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHR1.bit.ABNLTHR1	= cag_ctrl->corner_thresh_1[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHG1.bit.ABNLTHG1	= cag_ctrl->corner_thresh_1[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHB1.bit.ABNLTHB1	= cag_ctrl->corner_thresh_1[2];
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAR1, union io_abnlgar1, ABNLGAR1, cag_ctrl->corner_gain_1[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAG1, union io_abnlgag1, ABNLGAG1, cag_ctrl->corner_gain_1[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAB1, union io_abnlgab1, ABNLGAB1, cag_ctrl->corner_gain_1[2] );
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHR2.bit.ABNLTHR2	= cag_ctrl->corner_thresh_2[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHG2.bit.ABNLTHG2	= cag_ctrl->corner_thresh_2[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHB2.bit.ABNLTHB2	= cag_ctrl->corner_thresh_2[2];
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAR2, union io_abnlgar2, ABNLGAR2, cag_ctrl->corner_gain_2[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAG2, union io_abnlgag2, ABNLGAG2, cag_ctrl->corner_gain_2[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAB2, union io_abnlgab2, ABNLGAB2, cag_ctrl->corner_gain_2[2] );
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHR3.bit.ABNLTHR3	= cag_ctrl->corner_thresh_3[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHG3.bit.ABNLTHG3	= cag_ctrl->corner_thresh_3[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHB3.bit.ABNLTHB3	= cag_ctrl->corner_thresh_3[2];
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAR3, union io_abnlgar3, ABNLGAR3, cag_ctrl->corner_gain_3[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAG3, union io_abnlgag3, ABNLGAG3, cag_ctrl->corner_gain_3[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAB3, union io_abnlgab3, ABNLGAB3, cag_ctrl->corner_gain_3[2] );
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHR4.bit.ABNLTHR4	= cag_ctrl->corner_thresh_4[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHG4.bit.ABNLTHG4	= cag_ctrl->corner_thresh_4[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHB4.bit.ABNLTHB4	= cag_ctrl->corner_thresh_4[2];
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAR4, union io_abnlgar4, ABNLGAR4, cag_ctrl->corner_gain_4[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAG4, union io_abnlgag4, ABNLGAG4, cag_ctrl->corner_gain_4[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAB4, union io_abnlgab4, ABNLGAB4, cag_ctrl->corner_gain_4[2] );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of CAG.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg :  control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_CAG_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGTRG.bit.CAGTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGPAEN.bit.CPAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set CAG area
@param[in]	unit_no : Unit number.
@param[in]	cag_area	: CAG area.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_CAG_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CAG_AREA* cag_area )
{
#ifdef CO_PARAM_CHECK
	if (cag_area == NULL){
		Ddim_Assertion(("I:Im_PRO_CAG_Set_Area error. cag_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_CAG_CAGGH_MIN, D_IM_PRO_CAG_CAGGH_MAX, cag_area->g_pos_x, "Im_PRO_CAG_Set_Area : g_pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_CAG_CAGGV_MIN, D_IM_PRO_CAG_CAGGV_MAX, cag_area->g_pos_y, "Im_PRO_CAG_Set_Area : g_pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_CAG_CAGGHW_MIN, D_IM_PRO_CAG_CAGGHW_MAX, cag_area->g_width, "Im_PRO_CAG_Set_Area : g_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_CAG_CAGGVW_MIN, D_IM_PRO_CAG_CAGGVW_MAX, cag_area->g_lines, "Im_PRO_CAG_Set_Area : g_lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_CAG_CAGH_MIN, D_IM_PRO_CAG_CAGH_MAX, cag_area->pos_x, "Im_PRO_CAG_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_CAG_CAGV_MIN, D_IM_PRO_CAG_CAGV_MAX, cag_area->pos_y, "Im_PRO_CAG_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_CAG_CAGHW_MIN, D_IM_PRO_CAG_CAGHW_MAX, cag_area->width, "Im_PRO_CAG_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_CAG_CAGVW_MIN, D_IM_PRO_CAG_CAGVW_MAX, cag_area->lines, "Im_PRO_CAG_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGGWP.bit.CAGGH		= cag_area->g_pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGGWP.bit.CAGGV		= cag_area->g_pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGGWS.bit.CAGGHW		= cag_area->g_width;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGGWS.bit.CAGGVW		= cag_area->g_lines;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGWP.bit.CAGH			= cag_area->pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGWP.bit.CAGV			= cag_area->pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGWS.bit.CAGHW		= cag_area->width;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGWS.bit.CAGVW		= cag_area->lines;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set CAG opt_center_pos_h  control
@param[in]	unit_no : Unit number.
@param[in]	opt_center_pos_h	: CAG control Information.
*/
INT32 Im_PRO_CAG_Opt_Center_Pos_H_Ctrl( E_IM_PRO_UNIT_NUM unit_no, SHORT opt_center_pos_h [3] )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOARH, union io_aboarh, ABOARH, opt_center_pos_h[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOAGH, union io_aboagh, ABOAGH, opt_center_pos_h[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOABH, union io_aboabh, ABOABH, opt_center_pos_h[2] );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set CAG G level difference information
@param[in]	unit_no : Unit number.
@param[in]	cag_ctrl_glevel_diff	: CAG G level difference Information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_CAG_Ctrl_Glv_Diff( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CAG_CTRL_GLEVEL_DIFF* cag_ctrl_glevel_diff )
{
#ifdef CO_PARAM_CHECK
	if (cag_ctrl_glevel_diff == NULL){
		Ddim_Assertion(("I:Im_PRO_CAG_Ctrl_Glv_Diff error. cag_ctrl_glevel_diff=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGMD.bit.CLPSEL		= cag_ctrl_glevel_diff->clip_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGMD.bit.GCAMD		= cag_ctrl_glevel_diff->g_gap_cor_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGMD.bit.CAGPDD		= cag_ctrl_glevel_diff->g_gap_pad_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGMD.bit.CAMD			= cag_ctrl_glevel_diff->g_gap_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.BFGLMT.bit.BFGLMTH		= cag_ctrl_glevel_diff->gradient_u_limit;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.BFGLMT.bit.BFGLMTL		= cag_ctrl_glevel_diff->gradient_l_limit;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.BFGTH.bit.BFGTHA		= cag_ctrl_glevel_diff->gradient_threshold;
	IO_PRO.IMG_PIPE[unit_no].SRO.CAG.BFGTH.bit.BFGTHK		= cag_ctrl_glevel_diff->gradient_threshold_gain;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of CAG control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of CAG control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_CAG_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_CAG_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_CAG_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_PRO_CAG_Addr[unit_no]);

	return D_DDIM_OK;
}

/**
Compensation mode start
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_DPC_Start( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCTRG.bit.DPCTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_DPC_Status_Tbl[unit_no], 0);

	return D_DDIM_OK;
}

/**
Compensation mode stop
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_DPC_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCTRG.bit.DPCTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCTRG.bit.DPCTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_DPC_Status_Tbl[unit_no], 0);

	return D_DDIM_OK;
}

/**
DPC control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_ctrl : DPC control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_CTRL* dpc_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (dpc_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_DPC_Ctrl error. dpc_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCMD.bit.DPCMD			= dpc_ctrl->dpc_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCMD.bit.DPCRTZ			= dpc_ctrl->thre_conv_0;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of AEAWB.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCPAEN.bit.DPAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
DPC control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_area : DPC control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_AREA* dpc_area )
{
#ifdef CO_PARAM_CHECK
	if (dpc_area == NULL){
		Ddim_Assertion(("I:Im_PRO_DPC_Set_Area error. dpc_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCGH_MIN, D_IM_PRO_DPC_DPCGH_MAX, dpc_area->g_pos_x, "Im_PRO_DPC_Set_Area : g_pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCGV_MIN, D_IM_PRO_DPC_DPCGV_MAX, dpc_area->g_pos_y, "Im_PRO_DPC_Set_Area : g_pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCGHW_MIN, D_IM_PRO_DPC_DPCGHW_MAX, dpc_area->g_width, "Im_PRO_DPC_Set_Area : g_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCGVW_MIN, D_IM_PRO_DPC_DPCGVW_MAX, dpc_area->g_lines, "Im_PRO_DPC_Set_Area : g_lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCH_MIN, D_IM_PRO_DPC_DPCH_MAX, dpc_area->pos_x, "Im_PRO_DPC_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCV_MIN, D_IM_PRO_DPC_DPCV_MAX, dpc_area->pos_y, "Im_PRO_DPC_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCHW_MIN, D_IM_PRO_DPC_DPCHW_MAX, dpc_area->width, "Im_PRO_DPC_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCVW_MIN, D_IM_PRO_DPC_DPCVW_MAX, dpc_area->lines, "Im_PRO_DPC_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCGWP.bit.DPCGH			= dpc_area->g_pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCGWP.bit.DPCGV			= dpc_area->g_pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCGWS.bit.DPCGHW			= dpc_area->g_width;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCGWS.bit.DPCGVW			= dpc_area->g_lines;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCWP.bit.DPCH				= dpc_area->pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCWP.bit.DPCV				= dpc_area->pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCWS.bit.DPCHW			= dpc_area->width;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCWS.bit.DPCVW			= dpc_area->lines;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Alpha blend control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_ab_ctrl : Alpha blend Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Alpha_Blend_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_AB_CTRL* dpc_ab_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (dpc_ab_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_DPC_Alpha_Blend_Ctrl error. dpc_ab_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCBLW_MIN, D_IM_PRO_DPC_DPCBLW_MAX, dpc_ab_ctrl->alpha_blend_transition_width_p, "Im_PRO_DPC_Alpha_Blend_Ctrl : alpha_blend_transition_width_p" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_DPC_DPCBLW_MIN, D_IM_PRO_DPC_DPCBLW_MAX, dpc_ab_ctrl->alpha_blend_transition_width_m, "Im_PRO_DPC_Alpha_Blend_Ctrl : alpha_blend_transition_width_m" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCBLP.bit.DPCBLWP			= dpc_ab_ctrl->alpha_blend_transition_width_p;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCBLP.bit.DPCBLSP			= dpc_ab_ctrl->alpha_blend_start_pos_p;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCBLM.bit.DPCBLWM			= dpc_ab_ctrl->alpha_blend_transition_width_m;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCBLM.bit.DPCBLSM			= dpc_ab_ctrl->alpha_blend_start_pos_m;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Defect Dynamic Detection control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_ddd_ctrl : Defect Dynamic Detection Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Dynamic_Detect_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_DDD_CTRL* dpc_ddd_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (dpc_ddd_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_DPC_Dynamic_Detect_Ctrl error. dpc_ddd_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCTL.bit.DFGRGB			= dpc_ddd_ctrl->grgb_independent_setting;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCTL.bit.DFMDG			= dpc_ddd_ctrl->g_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCTL.bit.DFMDRB			= dpc_ddd_ctrl->rb_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCTL.bit.DFDCNT			= dpc_ddd_ctrl->upper_limit_Local_registration;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCTL.bit.DFRBUG			= dpc_ddd_ctrl->use_mdd;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCTL.bit.DFAGND			= dpc_ddd_ctrl->support_2concecutive_defect;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS1.bit.DFSRP			= dpc_ddd_ctrl->p_threshold_start_pos[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS2.bit.DFSGP			= dpc_ddd_ctrl->p_threshold_start_pos[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS3.bit.DFSBP			= dpc_ddd_ctrl->p_threshold_start_pos[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS1.bit.DFSRM			= dpc_ddd_ctrl->m_threshold_start_pos[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS2.bit.DFSGM			= dpc_ddd_ctrl->m_threshold_start_pos[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS3.bit.DFSBM			= dpc_ddd_ctrl->m_threshold_start_pos[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO1.bit.DFORP			= dpc_ddd_ctrl->p_threshold_start_offset[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO2.bit.DFOGP			= dpc_ddd_ctrl->p_threshold_start_offset[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO3.bit.DFOBP			= dpc_ddd_ctrl->p_threshold_start_offset[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO1.bit.DFORM			= dpc_ddd_ctrl->m_threshold_start_offset[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO2.bit.DFOGM			= dpc_ddd_ctrl->m_threshold_start_offset[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO3.bit.DFOBM			= dpc_ddd_ctrl->m_threshold_start_offset[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK1.bit.DFKRP			= dpc_ddd_ctrl->p_threshold_gain[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK2.bit.DFKGP			= dpc_ddd_ctrl->p_threshold_gain[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK3.bit.DFKBP			= dpc_ddd_ctrl->p_threshold_gain[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK1.bit.DFKRM			= dpc_ddd_ctrl->m_threshold_gain[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK2.bit.DFKGM			= dpc_ddd_ctrl->m_threshold_gain[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK3.bit.DFKBM			= dpc_ddd_ctrl->m_threshold_gain[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH1.bit.DFHRP			= dpc_ddd_ctrl->p_threshold_upper_limit[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH2.bit.DFHGP			= dpc_ddd_ctrl->p_threshold_upper_limit[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH3.bit.DFHBP			= dpc_ddd_ctrl->p_threshold_upper_limit[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH1.bit.DFHRM			= dpc_ddd_ctrl->m_threshold_upper_limit[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH2.bit.DFHGM			= dpc_ddd_ctrl->m_threshold_upper_limit[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH3.bit.DFHBM			= dpc_ddd_ctrl->m_threshold_upper_limit[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCSR.bit.DFCSRP			= dpc_ddd_ctrl->p_threshold_2consecutive;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCSR.bit.DFCSRM			= dpc_ddd_ctrl->m_threshold_2consecutive;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFLSTH.DFLSTH1.bit.DFLSTHR	= dpc_ddd_ctrl->threshold_2consecutive_bright_pix[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFLSTH.DFLSTH1.bit.DFLSTHG	= dpc_ddd_ctrl->threshold_2consecutive_bright_pix[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFLSTH.DFLSTH2.bit.DFLSTHB	= dpc_ddd_ctrl->threshold_2consecutive_bright_pix[2];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Calculation of mis-determination degree control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_mdd_ctrl : Calculation of mis-determination degree for correction Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Mis_Determination_Calc_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_MDD_CTRL* dpc_mdd_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (dpc_mdd_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_DPC_Mis_Determination_Calc_Ctrl error. dpc_mdd_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGEF.bit.DFAGEFP			= dpc_mdd_ctrl->p_mdd_effect;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGEF.bit.DFAGEFM			= dpc_mdd_ctrl->m_mdd_effect;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGEF2.bit.DFAGEFP2		= dpc_mdd_ctrl->p_mdd_effect2;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGEF2.bit.DFAGEFM2		= dpc_mdd_ctrl->m_mdd_effect2;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGTH.bit.DFAGTHP			= dpc_mdd_ctrl->p_threshold_offset;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGTH.bit.DFAGTHM			= dpc_mdd_ctrl->m_threshold_offset;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGTHK.bit.DFAGTHKP		= dpc_mdd_ctrl->p_threshold_gain;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGTHK.bit.DFAGTHKM		= dpc_mdd_ctrl->m_threshold_gain;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Pixel value magnitude defect correction control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_mdc_ctrl : Pixel value magnitude defect correction control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Magnitude_Defect_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_MDC_CTRL* dpc_mdc_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (dpc_mdc_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_DPC_Magnitude_Defect_Ctrl error. dpc_mdc_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMCTL.bit.DMGUG			= dpc_mdc_ctrl->use_ocd;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMWKS.bit.DMWKSP			= dpc_mdc_ctrl->p_manual_adj_val_single;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMWKS.bit.DMWKSM			= dpc_mdc_ctrl->m_manual_adj_val_single;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMWKD.bit.DMWKDP			= dpc_mdc_ctrl->p_manual_adj_val_2consecutive;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMWKD.bit.DMWKDM			= dpc_mdc_ctrl->m_manual_adj_val_2consecutive;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMAGK.bit.DMAGKP			= dpc_mdc_ctrl->p_correction_level_on_ocd;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMAGK.bit.DMAGKM			= dpc_mdc_ctrl->m_correction_level_on_ocd;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Edge storage defect correction control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_esdc_ctrl : Edge storage defect correction Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Edge_Storage_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_ESDC_CTRL* dpc_esdc_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (dpc_esdc_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_DPC_Edge_Storage_Ctrl error. dpc_esdc_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DECTL.bit.DEGRGB			= dpc_esdc_ctrl->esd_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DECTL.bit.DERBUG			= dpc_esdc_ctrl->use_rb_jdge;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DECTL.bit.DEGUG			= dpc_esdc_ctrl->use_ocd;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSS.bit.DENSS			= dpc_esdc_ctrl->threshold_start_pos;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSO.bit.DENSO			= dpc_esdc_ctrl->threshold_offset;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSK.bit.DENSK			= dpc_esdc_ctrl->threshold_gain;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSH.bit.DENSH			= dpc_esdc_ctrl->threshold_upper_limit;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSMGN.bit.DENSMGN		= dpc_esdc_ctrl->noise_tolerance;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENDCTH.bit.DENDCTH		= dpc_esdc_ctrl->threshold_calculation;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DEAGK.bit.DEAGKP			= dpc_esdc_ctrl->p_correction_level_on_ocd;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DEAGK.bit.DEAGKM			= dpc_esdc_ctrl->m_correction_level_on_ocd;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Calculation of over-correction degree for correction control setup 	
@param[in]	unit_no : Unit number.
@param[in]	dpc_ocd_ctrl : Calculation of over-correction degree for correction control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DPC_Over_Correction_Detect_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_OCD_CTRL* dpc_ocd_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (dpc_ocd_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_DPC_Over_Correction_Detect_Ctrl error. dpc_ocd_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DCAGEF.bit.DCAGEFP			= dpc_ocd_ctrl->p_ocd_effect;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DCAGEF.bit.DCAGEFM			= dpc_ocd_ctrl->m_ocd_effect;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DCAGTH.bit.DCAGTHP			= dpc_ocd_ctrl->p_threshold;
	IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DCAGTH.bit.DCAGTHM			= dpc_ocd_ctrl->m_threshold;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of DPC control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of SDC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_DPC_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_DPC_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_DPC_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_PRO_DPC_Addr[unit_no]);

	return D_DDIM_OK;
}

/**
Start FFC
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH/M2P not running NG
*/
INT32 Im_PRO_FFC_Start( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCTRG.bit.FCTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_FFC_Status_Tbl[unit_no], 0);

	return D_DDIM_OK;
}

/**
Stop FFC
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH/M2P not stopped NG
*/
INT32 Im_PRO_FFC_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCTRG.bit.FCTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCTRG.bit.FCTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_FFC_Status_Tbl[unit_no], 0);

	return D_DDIM_OK;
}

/**
Set FFC control information
@param[in]	unit_no : Unit number.
@param[in]	ffc_ctrl	: FFC control Information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFC_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FFC_CTRL* ffc_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ffc_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FFC_Ctrl error. ffc_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCSUBPRH_MIN, D_IM_PRO_FFC_FCSUBPRH_MAX, ffc_ctrl->coef_horizontal_size, "Im_PRO_FFC_Ctrl : coef_horizontal_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCSUBPRV_MIN, D_IM_PRO_FFC_FCSUBPRV_MAX, ffc_ctrl->coef_vertical_size, "Im_PRO_FFC_Ctrl : coef_vertical_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCSUBSPH_MIN, D_IM_PRO_FFC_FCSUBSPH_MAX, ffc_ctrl->horizontal_pixel, "Im_PRO_FFC_Ctrl : horizontal_pixel" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCSUBSPV_MIN, D_IM_PRO_FFC_FCSUBSPV_MAX, ffc_ctrl->vertical_pixel, "Im_PRO_FFC_Ctrl : vertical_pixel" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCSUBSTH_MIN, D_IM_PRO_FFC_FCSUBSTH_MAX, ffc_ctrl->coef_pos_x, "Im_PRO_FFC_Ctrl : coef_pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCSUBSTV_MIN, D_IM_PRO_FFC_FCSUBSTV_MAX, ffc_ctrl->coef_pos_y, "Im_PRO_FFC_Ctrl : coef_pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBPR.bit.FCSUBPRH	= ffc_ctrl->coef_horizontal_size;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBPR.bit.FCSUBPRV	= ffc_ctrl->coef_vertical_size;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBSP.bit.FCSUBSPH	= ffc_ctrl->horizontal_pixel;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBSP.bit.FCSUBSPV	= ffc_ctrl->vertical_pixel;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBDR.bit.FCSUBDRM	= ffc_ctrl->inverse_base;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBDR.bit.FCSUBDRE	= ffc_ctrl->inverse_exponent;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBST.bit.FCSUBSTH	= ffc_ctrl->coef_pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBST.bit.FCSUBSTV	= ffc_ctrl->coef_pos_y;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set FFC blend control information
@param[in]	unit_no : Unit number.
@param[in]	blend_ctrl	: FFC blend control Information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFC_Set_Blend( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FFC_BLEND_CTRL* blend_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (blend_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FFC_Set_Blend error. blend_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCBC, union io_ffc_fcbc, FCBC0, blend_ctrl->blend_ratio_0 );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCBC, union io_ffc_fcbc, FCBC1, blend_ctrl->blend_ratio_1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of FFC.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFC_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCTRG.bit.FCTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_FFC_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCPAEN.bit.FCPAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set FFC area
@param[in]	unit_no : Unit number.
@param[in]	ffc_area	: FFC area.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFC_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FFC_AREA* ffc_area )
{
#ifdef CO_PARAM_CHECK
	if (ffc_area == NULL){
		Ddim_Assertion(("I:Im_PRO_FFC_Set_Area error. ffc_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCH_MIN, D_IM_PRO_FFC_FCH_MAX, ffc_area->pos_x, "Im_PRO_FFC_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCV_MIN, D_IM_PRO_FFC_FCV_MAX, ffc_area->pos_y, "Im_PRO_FFC_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCHW_MIN, D_IM_PRO_FFC_FCHW_MAX, ffc_area->width, "Im_PRO_FFC_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFC_FCVW_MIN, D_IM_PRO_FFC_FCVW_MAX, ffc_area->lines, "Im_PRO_FFC_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCH.bit.FCH	= ffc_area->pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCV.bit.FCV	= ffc_area->pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCHW.bit.FCHW	= ffc_area->width;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCVW.bit.FCVW	= ffc_area->lines;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
set the request issue interval when reading the coefficients from PRCH/M2P macro.
@param[in]	unit_no : Unit number.
@param[in]	req_interval_clk : RAM access control<br>
			value range :[4~1023]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFC_Set_Coeff_Read_Line_Wait( E_IM_PRO_UNIT_NUM unit_no, USHORT req_interval_clk )
{
#ifdef CO_PARAM_CHECK
	if( im_pro_check_val_range( D_IM_PRO_FFC_FCWRIC_MIN, D_IM_PRO_FFC_FCWRIC_MAX, req_interval_clk, "Im_PRO_FFC_Set_Coeff_Read_Line_Wait : req_interval_clk" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( req_interval_clk >= ((IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCHW.bit.FCHW - (4*IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCSUBPR.bit.FCSUBPRH))-10)/3 ) {
		Ddim_Assertion(("I:Im_PRO_FFC_Set_Coeff_Read_Line_Wait. parameter error. Please satisfy following relationship. (req_interval_clk  < ((FCHW-(4*FCSUBPRH)) -10)/3) (%u)\n", req_interval_clk));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFC.FCWRIC.bit.FCWRIC	= req_interval_clk;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of FFC control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of FFC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_FFC_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_FFC_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_FFC_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_PRO_FFC_Addr[unit_no]);

	return D_DDIM_OK;
}

/**
Start HRB
@param[in]	unit_no : Unit number.
*/
VOID Im_PRO_HRB_Start( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBTRG.bit.HRBTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_HRB_Status_Tbl[unit_no], 0);
}

/**
Stop HRB
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
VOID Im_PRO_HRB_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBTRG.bit.HRBTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBTRG.bit.HRBTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_HRB_Status_Tbl[unit_no], 0);
}

/**
HRB control data setting
@param[in]	unit_no : Unit number.
@param[in]	hrb_ctrl : HRB control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HRB_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_HRB_CTRL* hrb_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (hrb_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_HRB_Ctrl error. hrb_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HRB_HRBH_MIN, D_IM_PRO_HRB_HRBH_MAX, hrb_ctrl->in_pos_x, "Im_PRO_HRB_Ctrl : in_pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HRB_HRBV_MIN, D_IM_PRO_HRB_HRBV_MAX, hrb_ctrl->in_pos_y, "Im_PRO_HRB_Ctrl : in_pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HRB_HRBHW_MIN, D_IM_PRO_HRB_HRBHW_MAX, hrb_ctrl->in_width, "Im_PRO_HRB_Ctrl : in_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HRB_HRBVW_MIN, D_IM_PRO_HRB_HRBVW_MAX, hrb_ctrl->in_lines, "Im_PRO_HRB_Ctrl : in_lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HRB_HRBOHW_MIN, D_IM_PRO_HRB_HRBOHW_MAX, hrb_ctrl->out_width, "Im_PRO_HRB_Ctrl : out_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HRB_HRBSCYC_MIN, D_IM_PRO_HRB_HRBSCYC_MAX, hrb_ctrl->cycle, "Im_PRO_HRB_Ctrl : cycle" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HRB_HRBOSM_MIN, D_IM_PRO_HRB_HRBOSM_MAX, hrb_ctrl->out_timing, "Im_PRO_HRB_Ctrl : out_timing" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBH.bit.HRBH				= hrb_ctrl->in_pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBV.bit.HRBV				= hrb_ctrl->in_pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBHW.bit.HRBHW			= hrb_ctrl->in_width;
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBVW.bit.HRBVW			= hrb_ctrl->in_lines;
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBOHW.bit.HRBOHW			= hrb_ctrl->out_width;
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBSCYC.bit.HRBSCYC		= hrb_ctrl->cycle;
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBOSM.bit.HRBOSM			= hrb_ctrl->out_timing;

	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK1.bit.HRBK_0_0	= hrb_ctrl->coefficient[ 0][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK1.bit.HRBK_0_1	= hrb_ctrl->coefficient[ 0][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK2.bit.HRBK_0_2	= hrb_ctrl->coefficient[ 0][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK2.bit.HRBK_0_3	= hrb_ctrl->coefficient[ 0][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK3.bit.HRBK_0_4	= hrb_ctrl->coefficient[ 0][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK3.bit.HRBK_1_0	= hrb_ctrl->coefficient[ 1][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK4.bit.HRBK_1_1	= hrb_ctrl->coefficient[ 1][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK4.bit.HRBK_1_2	= hrb_ctrl->coefficient[ 1][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK5.bit.HRBK_1_3	= hrb_ctrl->coefficient[ 1][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK5.bit.HRBK_1_4	= hrb_ctrl->coefficient[ 1][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK6.bit.HRBK_2_0	= hrb_ctrl->coefficient[ 2][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK6.bit.HRBK_2_1	= hrb_ctrl->coefficient[ 2][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK7.bit.HRBK_2_2	= hrb_ctrl->coefficient[ 2][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK7.bit.HRBK_2_3	= hrb_ctrl->coefficient[ 2][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK8.bit.HRBK_2_4	= hrb_ctrl->coefficient[ 2][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK8.bit.HRBK_3_0	= hrb_ctrl->coefficient[ 3][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK9.bit.HRBK_3_1	= hrb_ctrl->coefficient[ 3][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK9.bit.HRBK_3_2	= hrb_ctrl->coefficient[ 3][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK10.bit.HRBK_3_3	= hrb_ctrl->coefficient[ 3][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK10.bit.HRBK_3_4	= hrb_ctrl->coefficient[ 3][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK11.bit.HRBK_4_0	= hrb_ctrl->coefficient[ 4][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK11.bit.HRBK_4_1	= hrb_ctrl->coefficient[ 4][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK12.bit.HRBK_4_2	= hrb_ctrl->coefficient[ 4][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK12.bit.HRBK_4_3	= hrb_ctrl->coefficient[ 4][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK13.bit.HRBK_4_4	= hrb_ctrl->coefficient[ 4][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK13.bit.HRBK_5_0	= hrb_ctrl->coefficient[ 5][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK14.bit.HRBK_5_1	= hrb_ctrl->coefficient[ 5][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK14.bit.HRBK_5_2	= hrb_ctrl->coefficient[ 5][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK15.bit.HRBK_5_3	= hrb_ctrl->coefficient[ 5][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK15.bit.HRBK_5_4	= hrb_ctrl->coefficient[ 5][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK16.bit.HRBK_6_0	= hrb_ctrl->coefficient[ 6][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK16.bit.HRBK_6_1	= hrb_ctrl->coefficient[ 6][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK17.bit.HRBK_6_2	= hrb_ctrl->coefficient[ 6][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK17.bit.HRBK_6_3	= hrb_ctrl->coefficient[ 6][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK18.bit.HRBK_6_4	= hrb_ctrl->coefficient[ 6][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK18.bit.HRBK_7_0	= hrb_ctrl->coefficient[ 7][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK19.bit.HRBK_7_1	= hrb_ctrl->coefficient[ 7][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK19.bit.HRBK_7_2	= hrb_ctrl->coefficient[ 7][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK20.bit.HRBK_7_3	= hrb_ctrl->coefficient[ 7][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK20.bit.HRBK_7_4	= hrb_ctrl->coefficient[ 7][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK21.bit.HRBK_8_0	= hrb_ctrl->coefficient[ 8][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK21.bit.HRBK_8_1	= hrb_ctrl->coefficient[ 8][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK22.bit.HRBK_8_2	= hrb_ctrl->coefficient[ 8][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK22.bit.HRBK_8_3	= hrb_ctrl->coefficient[ 8][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK23.bit.HRBK_8_4	= hrb_ctrl->coefficient[ 8][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK23.bit.HRBK_9_0	= hrb_ctrl->coefficient[ 9][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK24.bit.HRBK_9_1	= hrb_ctrl->coefficient[ 9][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK24.bit.HRBK_9_2	= hrb_ctrl->coefficient[ 9][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK25.bit.HRBK_9_3	= hrb_ctrl->coefficient[ 9][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK25.bit.HRBK_9_4	= hrb_ctrl->coefficient[ 9][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK26.bit.HRBK_10_0	= hrb_ctrl->coefficient[10][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK26.bit.HRBK_10_1	= hrb_ctrl->coefficient[10][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK27.bit.HRBK_10_2	= hrb_ctrl->coefficient[10][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK27.bit.HRBK_10_3	= hrb_ctrl->coefficient[10][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK28.bit.HRBK_10_4	= hrb_ctrl->coefficient[10][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK28.bit.HRBK_11_0	= hrb_ctrl->coefficient[11][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK29.bit.HRBK_11_1	= hrb_ctrl->coefficient[11][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK29.bit.HRBK_11_2	= hrb_ctrl->coefficient[11][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK30.bit.HRBK_11_3	= hrb_ctrl->coefficient[11][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK30.bit.HRBK_11_4	= hrb_ctrl->coefficient[11][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK31.bit.HRBK_12_0	= hrb_ctrl->coefficient[12][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK31.bit.HRBK_12_1	= hrb_ctrl->coefficient[12][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK32.bit.HRBK_12_2	= hrb_ctrl->coefficient[12][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK32.bit.HRBK_12_3	= hrb_ctrl->coefficient[12][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK33.bit.HRBK_12_4	= hrb_ctrl->coefficient[12][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK33.bit.HRBK_13_0	= hrb_ctrl->coefficient[13][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK34.bit.HRBK_13_1	= hrb_ctrl->coefficient[13][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK34.bit.HRBK_13_2	= hrb_ctrl->coefficient[13][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK35.bit.HRBK_13_3	= hrb_ctrl->coefficient[13][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK35.bit.HRBK_13_4	= hrb_ctrl->coefficient[13][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK36.bit.HRBK_14_0	= hrb_ctrl->coefficient[14][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK36.bit.HRBK_14_1	= hrb_ctrl->coefficient[14][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK37.bit.HRBK_14_2	= hrb_ctrl->coefficient[14][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK37.bit.HRBK_14_3	= hrb_ctrl->coefficient[14][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK38.bit.HRBK_14_4	= hrb_ctrl->coefficient[14][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK38.bit.HRBK_15_0	= hrb_ctrl->coefficient[15][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK39.bit.HRBK_15_1	= hrb_ctrl->coefficient[15][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK39.bit.HRBK_15_2	= hrb_ctrl->coefficient[15][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK40.bit.HRBK_15_3	= hrb_ctrl->coefficient[15][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK40.bit.HRBK_15_4	= hrb_ctrl->coefficient[15][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK41.bit.HRBK_16_0	= hrb_ctrl->coefficient[16][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK41.bit.HRBK_16_1	= hrb_ctrl->coefficient[16][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK42.bit.HRBK_16_2	= hrb_ctrl->coefficient[16][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK42.bit.HRBK_16_3	= hrb_ctrl->coefficient[16][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK43.bit.HRBK_16_4	= hrb_ctrl->coefficient[16][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK43.bit.HRBK_17_0	= hrb_ctrl->coefficient[17][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK44.bit.HRBK_17_1	= hrb_ctrl->coefficient[17][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK44.bit.HRBK_17_2	= hrb_ctrl->coefficient[17][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK45.bit.HRBK_17_3	= hrb_ctrl->coefficient[17][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK45.bit.HRBK_17_4	= hrb_ctrl->coefficient[17][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK46.bit.HRBK_18_0	= hrb_ctrl->coefficient[18][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK46.bit.HRBK_18_1	= hrb_ctrl->coefficient[18][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK47.bit.HRBK_18_2	= hrb_ctrl->coefficient[18][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK47.bit.HRBK_18_3	= hrb_ctrl->coefficient[18][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK48.bit.HRBK_18_4	= hrb_ctrl->coefficient[18][4];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK48.bit.HRBK_19_0	= hrb_ctrl->coefficient[19][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK49.bit.HRBK_19_1	= hrb_ctrl->coefficient[19][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK49.bit.HRBK_19_2	= hrb_ctrl->coefficient[19][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK50.bit.HRBK_19_3	= hrb_ctrl->coefficient[19][3];
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBK.HRBK50.bit.HRBK_19_4	= hrb_ctrl->coefficient[19][4];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of HRB.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HRB_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBTRG.bit.HRBTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_HRB_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.HRB.HRBPAEN.bit.PAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Start VRB
@param[in]	unit_no : Unit number.
*/
VOID Im_PRO_VRB_Start( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBTRG.bit.VRBTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_VRB_Status_Tbl[unit_no], 0);
}

/**
Stop VRB
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
VOID Im_PRO_VRB_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBTRG.bit.VRBTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBTRG.bit.VRBTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_VRB_Status_Tbl[unit_no], 0);
}

/**
VRB control data setting
@param[in]	unit_no : Unit number.
@param[in]	vrb_ctrl : VRB control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_VRB_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_VRB_CTRL* vrb_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (vrb_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_VRB_Ctrl error. vrb_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_VRB_VRBH_MIN, D_IM_PRO_VRB_VRBH_MAX, vrb_ctrl->in_pos_x, "Im_PRO_VRB_Ctrl : in_pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_VRB_VRBV_MIN, D_IM_PRO_VRB_VRBV_MAX, vrb_ctrl->in_pos_y, "Im_PRO_VRB_Ctrl : in_pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_VRB_VRBHW_MIN, D_IM_PRO_VRB_VRBHW_MAX, vrb_ctrl->in_width, "Im_PRO_VRB_Ctrl : in_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_VRB_VRBVW_MIN, D_IM_PRO_VRB_VRBVW_MAX, vrb_ctrl->in_lines, "Im_PRO_VRB_Ctrl : in_lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_VRB_VRBSCYC_MIN, D_IM_PRO_VRB_VRBSCYC_MAX, vrb_ctrl->cycle, "Im_PRO_VRB_Ctrl : cycle" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBH.bit.VRBH				= vrb_ctrl->in_pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBV.bit.VRBV				= vrb_ctrl->in_pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBHW.bit.VRBHW			= vrb_ctrl->in_width;
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBVW.bit.VRBVW			= vrb_ctrl->in_lines;
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBSCYC.bit.VRBSCYC		= vrb_ctrl->cycle;

	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K1.bit.VRB0K_0_0		= vrb_ctrl->coefficient0[0][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K1.bit.VRB0K_0_1		= vrb_ctrl->coefficient0[0][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K2.bit.VRB0K_0_2		= vrb_ctrl->coefficient0[0][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K2.bit.VRB0K_1_0		= vrb_ctrl->coefficient0[1][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K3.bit.VRB0K_1_1		= vrb_ctrl->coefficient0[1][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K3.bit.VRB0K_1_2		= vrb_ctrl->coefficient0[1][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K4.bit.VRB0K_2_0		= vrb_ctrl->coefficient0[2][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K4.bit.VRB0K_2_1		= vrb_ctrl->coefficient0[2][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K5.bit.VRB0K_2_2		= vrb_ctrl->coefficient0[2][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K5.bit.VRB0K_3_0		= vrb_ctrl->coefficient0[3][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K6.bit.VRB0K_3_1		= vrb_ctrl->coefficient0[3][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K6.bit.VRB0K_3_2		= vrb_ctrl->coefficient0[3][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K7.bit.VRB0K_4_0		= vrb_ctrl->coefficient0[4][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K7.bit.VRB0K_4_1		= vrb_ctrl->coefficient0[4][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K8.bit.VRB0K_4_2		= vrb_ctrl->coefficient0[4][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K8.bit.VRB0K_5_0		= vrb_ctrl->coefficient0[5][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K9.bit.VRB0K_5_1		= vrb_ctrl->coefficient0[5][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K9.bit.VRB0K_5_2		= vrb_ctrl->coefficient0[5][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K10.bit.VRB0K_6_0	= vrb_ctrl->coefficient0[6][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K10.bit.VRB0K_6_1	= vrb_ctrl->coefficient0[6][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K11.bit.VRB0K_6_2	= vrb_ctrl->coefficient0[6][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K11.bit.VRB0K_7_0	= vrb_ctrl->coefficient0[7][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K12.bit.VRB0K_7_1	= vrb_ctrl->coefficient0[7][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K12.bit.VRB0K_7_2	= vrb_ctrl->coefficient0[7][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K13.bit.VRB0K_8_0	= vrb_ctrl->coefficient0[8][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K13.bit.VRB0K_8_1	= vrb_ctrl->coefficient0[8][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K14.bit.VRB0K_8_2	= vrb_ctrl->coefficient0[8][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K14.bit.VRB0K_9_0	= vrb_ctrl->coefficient0[9][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K15.bit.VRB0K_9_1	= vrb_ctrl->coefficient0[9][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB0K.VRB0K15.bit.VRB0K_9_2	= vrb_ctrl->coefficient0[9][2];

	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K1.bit.VRB1K_0_0		= vrb_ctrl->coefficient1[0][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K1.bit.VRB1K_0_1		= vrb_ctrl->coefficient1[0][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K2.bit.VRB1K_0_2		= vrb_ctrl->coefficient1[0][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K2.bit.VRB1K_1_0		= vrb_ctrl->coefficient1[1][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K3.bit.VRB1K_1_1		= vrb_ctrl->coefficient1[1][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K3.bit.VRB1K_1_2		= vrb_ctrl->coefficient1[1][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K4.bit.VRB1K_2_0		= vrb_ctrl->coefficient1[2][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K4.bit.VRB1K_2_1		= vrb_ctrl->coefficient1[2][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K5.bit.VRB1K_2_2		= vrb_ctrl->coefficient1[2][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K5.bit.VRB1K_3_0		= vrb_ctrl->coefficient1[3][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K6.bit.VRB1K_3_1		= vrb_ctrl->coefficient1[3][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K6.bit.VRB1K_3_2		= vrb_ctrl->coefficient1[3][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K7.bit.VRB1K_4_0		= vrb_ctrl->coefficient1[4][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K7.bit.VRB1K_4_1		= vrb_ctrl->coefficient1[4][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K8.bit.VRB1K_4_2		= vrb_ctrl->coefficient1[4][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K8.bit.VRB1K_5_0		= vrb_ctrl->coefficient1[5][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K9.bit.VRB1K_5_1		= vrb_ctrl->coefficient1[5][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K9.bit.VRB1K_5_2		= vrb_ctrl->coefficient1[5][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K10.bit.VRB1K_6_0	= vrb_ctrl->coefficient1[6][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K10.bit.VRB1K_6_1	= vrb_ctrl->coefficient1[6][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K11.bit.VRB1K_6_2	= vrb_ctrl->coefficient1[6][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K11.bit.VRB1K_7_0	= vrb_ctrl->coefficient1[7][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K12.bit.VRB1K_7_1	= vrb_ctrl->coefficient1[7][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K12.bit.VRB1K_7_2	= vrb_ctrl->coefficient1[7][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K13.bit.VRB1K_8_0	= vrb_ctrl->coefficient1[8][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K13.bit.VRB1K_8_1	= vrb_ctrl->coefficient1[8][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K14.bit.VRB1K_8_2	= vrb_ctrl->coefficient1[8][2];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K14.bit.VRB1K_9_0	= vrb_ctrl->coefficient1[9][0];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K15.bit.VRB1K_9_1	= vrb_ctrl->coefficient1[9][1];
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRB1K.VRB1K15.bit.VRB1K_9_2	= vrb_ctrl->coefficient1[9][2];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of VRB.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_VRB_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBTRG.bit.VRBTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_VRB_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.VRB.VRBPAEN.bit.PAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

// --- REMOVE_BUZ BEGIN
/** @addtogroup im_pro_sro_buztop BUZTOP
@{
*/
/**
Software reset of BUZTOP macro.
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_BUZTOP_SR( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.BUZ.SR.bit.SR = D_IM_PRO_SR_RESET;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Software reset release of BUZTOP.
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_BUZTOP_SR_Rlease( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.BUZ.SR.bit.SR = D_IM_PRO_SR_RELEASE;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
BUZTOP control data setting
@param[in]	unit_no : Unit number.
@param[in]	buz_ctrl : BUZTOP control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_BUZTOP_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_BUZTOP_CTRL* buz_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (buz_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_BUZTOP_Ctrl error. buz_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_BUZ_BUZH_MIN, D_IM_PRO_BUZ_BUZH_MAX, buz_ctrl->in_pos_x, "Im_PRO_BUZTOP_Ctrl : in_pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_BUZ_BUZV_MIN, D_IM_PRO_BUZ_BUZV_MAX, buz_ctrl->in_pos_y, "Im_PRO_BUZTOP_Ctrl : in_pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_BUZ_BUZHW_MIN, D_IM_PRO_BUZ_BUZHW_MAX, buz_ctrl->in_width, "Im_PRO_BUZTOP_Ctrl : in_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_BUZ_BUZVW_MIN, D_IM_PRO_BUZ_BUZVW_MAX, buz_ctrl->in_lines, "Im_PRO_BUZTOP_Ctrl : in_lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_BUZ_BUZOHW_MIN, D_IM_PRO_BUZ_BUZOHW_MAX, buz_ctrl->out_width, "Im_PRO_BUZTOP_Ctrl : out_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.BUZ.BUZH.bit.BUZH		= buz_ctrl->in_pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.BUZ.BUZV.bit.BUZV		= buz_ctrl->in_pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.BUZ.BUZHW.bit.BUZHW	= buz_ctrl->in_width;
	IO_PRO.IMG_PIPE[unit_no].SRO.BUZ.BUZVW.bit.BUZVW	= buz_ctrl->in_lines;
	IO_PRO.IMG_PIPE[unit_no].SRO.BUZ.BUZOHW.bit.BUZOHW	= buz_ctrl->out_width;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
A setup of enable access to the built-in RAM of BUZTOP.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
INT32 Im_PRO_BUZTOP_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.BRSZSL.bit.BRSZSL != 0 ) ) {
		Ddim_Assertion(("I:Im_PRO_BUZTOP_Set_PAEN. Buz is not selected error.\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.BUZ.PAEN.bit.PAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of Resizer select.
@param[in]	unit_no : Unit number.
@param[in]	sel     : Raw resizer select.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
INT32 Im_PRO_BUZTOP_Set_Resizer_Sel( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BUZTOP_SEL sel )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.BRSZSL.bit.BRSZSL = sel;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/*	@}*/	// im_pro_sro_buztop
// --- REMOVE_BUZ END

/**
FFSH start
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not running NG
*/
INT32 Im_PRO_FFSH_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHTRG.bit.FFSHTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_FFSH_Status_Tbl[unit_no][ch], 0);

	return D_DDIM_OK;
}

/**
FFSH stop
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not stopped NG
*/
INT32 Im_PRO_FFSH_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHTRG.bit.FFSHTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHTRG.bit.FFSHTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_FFSH_Status_Tbl[unit_no][ch], 0);

	return D_DDIM_OK;
}

/**
FFSH control setup 
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	ffsh_ctrl : FFSH control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFSH_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, T_IM_PRO_FFSH_CTRL* ffsh_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ffsh_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FFSH_Ctrl error. ffsh_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHSUBPRH_MIN, D_IM_PRO_FFSH_FFSHSUBPRH_MAX, ffsh_ctrl->h_size_correction_coeff, "Im_PRO_FFSH_Ctrl : h_size_correction_coeff" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHSUBPRV_MIN, D_IM_PRO_FFSH_FFSHSUBPRV_MAX, ffsh_ctrl->v_size_correction_coeff, "Im_PRO_FFSH_Ctrl : v_size_correction_coeff" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHSUBSPH_MIN, D_IM_PRO_FFSH_FFSHSUBSPH_MAX, ffsh_ctrl->h_pixel_unit_interpolation, "Im_PRO_FFSH_Ctrl : h_pixel_unit_interpolation" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHSUBSPV_MIN, D_IM_PRO_FFSH_FFSHSUBSPV_MAX, ffsh_ctrl->v_pixel_unit_interpolation, "Im_PRO_FFSH_Ctrl : v_pixel_unit_interpolation" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHSUBSTH_MIN, D_IM_PRO_FFSH_FFSHSUBSTH_MAX, ffsh_ctrl->h_start_pos_coeff, "Im_PRO_FFSH_Ctrl : h_start_pos_coeff" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHSUBSTV_MIN, D_IM_PRO_FFSH_FFSHSUBSTV_MAX, ffsh_ctrl->v_start_pos_coeff, "Im_PRO_FFSH_Ctrl : v_start_pos_coeff" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBPR.bit.FFSHSUBPRH	= ffsh_ctrl->h_size_correction_coeff;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBPR.bit.FFSHSUBPRV	= ffsh_ctrl->v_size_correction_coeff;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBSP.bit.FFSHSUBSPH	= ffsh_ctrl->h_pixel_unit_interpolation;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBSP.bit.FFSHSUBSPV	= ffsh_ctrl->v_pixel_unit_interpolation;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBDR.bit.FFSHSUBDRM	= ffsh_ctrl->rad_inverse_coeff_interpolation;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBDR.bit.FFSHSUBDRE	= ffsh_ctrl->exp_inverse_coeff_interpolation;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBST.bit.FFSHSUBSTH	= ffsh_ctrl->h_start_pos_coeff;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBST.bit.FFSHSUBSTV	= ffsh_ctrl->v_start_pos_coeff;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of FFSH blend ratio.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	blend_ctrl : FFSH blend control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFSH_Set_Blend( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, T_IM_PRO_FFSH_BLEND_CTRL* blend_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (blend_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FFSH_Set_Blend error. blend_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHBC, union io_ffshbc, FFSHBC0, blend_ctrl->frame_correction_blend_0 );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHBC, union io_ffshbc, FFSHBC1, blend_ctrl->frame_correction_blend_1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of FFSH effective area 
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	ffsh_area : FFSH area information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFSH_Set_Area( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, T_IM_PRO_FFSH_AREA* ffsh_area )
{
#ifdef CO_PARAM_CHECK
	if (ffsh_area == NULL){
		Ddim_Assertion(("I:Im_PRO_FFSH_Set_Area error. ffsh_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHH_MIN, D_IM_PRO_FFSH_FFSHH_MAX, ffsh_area->ffsh_area.pos_x, "Im_PRO_FFSH_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHV_MIN, D_IM_PRO_FFSH_FFSHV_MAX, ffsh_area->ffsh_area.pos_y, "Im_PRO_FFSH_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHHW_MIN, D_IM_PRO_FFSH_FFSHHW_MAX, ffsh_area->ffsh_area.width, "Im_PRO_FFSH_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHVW_MIN, D_IM_PRO_FFSH_FFSHVW_MAX, ffsh_area->ffsh_area.lines, "Im_PRO_FFSH_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHH.bit.FFSHH	= ffsh_area->ffsh_area.pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHV.bit.FFSHV	= ffsh_area->ffsh_area.pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHHW.bit.FFSHHW	= ffsh_area->ffsh_area.width;
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHVW.bit.FFSHVW	= ffsh_area->ffsh_area.lines;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of FFSH.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFSH_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, UCHAR paen_trg )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHPAEN.bit.FFSHPAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
set the request issue interval when reading the coefficients from PRCH/M2P macro.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	req_interval_clk : RAM access control<br>
			value range :[4~1023]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FFSH_Set_Coeff_Read_Line_Wait( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, USHORT req_interval_clk )
{
#ifdef CO_PARAM_CHECK
	if( im_pro_check_val_range( D_IM_PRO_FFSH_FFSHWRIC_MIN, D_IM_PRO_FFSH_FFSHWRIC_MAX, req_interval_clk, "Im_PRO_FFSH_Set_Coeff_Read_Line_Wait : req_interval_clk" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( req_interval_clk >= ((IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHHW.bit.FFSHHW - (4*IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHSUBPR.bit.FFSHSUBPRH))-10)/3 ) {
		Ddim_Assertion(("I:Im_PRO_FFSH_Set_Coeff_Read_Line_Wait. parameter error. Please satisfy following relationship. (req_interval_clk  < ((FFSHHW-(4*FFSHSUBPRH)) -10)/3) (%u)\n", req_interval_clk));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FFSH[ch].FFSHWRIC.bit.FFSHWRIC	= req_interval_clk;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of FFSH control.
@param[in]		unit_no : Unit number.
@param[in]		ch : Channel No
@param[out]		addr					: Top address of the address array of FFSH control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_FFSH_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, const T_IM_PRO_RDMA_FFSH_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_FFSH_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_PRO_FFSH_Addr[unit_no][ch]);

	return D_DDIM_OK;
}

/**
FF start.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FF_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFTRG.bit.FFTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_FF_Status_Tbl[unit_no][ch], 0);

	return D_DDIM_OK;
}

/**
FF stop.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FF_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFTRG.bit.FFTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFTRG.bit.FFTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_FF_Status_Tbl[unit_no][ch], 0);

	return D_DDIM_OK;
}

/**
Setup of FF control parameter.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	ff_ctrl : FF Controller
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FF_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch, T_IM_PRO_FF_CTRL* ff_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ff_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FF_Ctrl error. ff_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFDEFTS.bit.FFDEFTS	= ff_ctrl->ffdefts;
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFMNWGT, union io_ffmnwgt, FFMNWGT, ff_ctrl->mnwgt );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFSBWGT, union io_ffsbwgt, FFSBWGT, ff_ctrl->sbwgt );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Setup of input position .
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	ff_area	:The area for OB detection<br>
					value range :pos_x[0 - 12287]<br>
					target registor :@@FFH<br>
					value range :pos_y[0 - 8191]<br>
					target registor :@@FFV<br>
					value range :width[4 - 12288] 4pixel boundary<br>
					target registor :@@FFHW<br>
					value range :lines[4 - 8192] 4pixel boundary<br>
					target registor :@@FFVW<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FF_Set_Area( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch, T_IM_PRO_AREA_INFO* ff_area )
{
#ifdef CO_PARAM_CHECK
	if (ff_area == NULL){
		Ddim_Assertion(("I:Im_PRO_FF_Set_Area error. ff_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FF_FFH_MIN, D_IM_PRO_FF_FFH_MAX, ff_area->pos_x, "Im_PRO_FF_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FF_FFV_MIN, D_IM_PRO_FF_FFV_MAX, ff_area->pos_y, "Im_PRO_FF_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FF_FFHW_MIN, D_IM_PRO_FF_FFHW_MAX, ff_area->width, "Im_PRO_FF_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FF_FFVW_MIN, D_IM_PRO_FF_FFVW_MAX, ff_area->lines, "Im_PRO_FF_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFH.bit.FFH		= ff_area->pos_x;
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFV.bit.FFV		= ff_area->pos_y;
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFHW.bit.FFHW	= ff_area->width;
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFVW.bit.FFVW	= ff_area->lines;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
FIR coefficient setting.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	lpf_ctrl :LPF Control information.<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FF_Set_FIR_Filter( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch, T_IM_PRO_FF_LPF_CTRL* lpf_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (lpf_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FF_Set_FIR_Filter error. lpf_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( ( lpf_ctrl->b0 + 2 * ( lpf_ctrl->b1 + lpf_ctrl->b2 ) ) != 128 ) {
		Ddim_Assertion(("I:Im_PRO_FF_Set_FIR_Filter parameter error. Please satisfy following relationship. lpf_ctrl->b0 + 2*(lpf_ctrl->b1 + lpf_ctrl->b2) == 128  (%u %u %u)\n", lpf_ctrl->b0, lpf_ctrl->b1, lpf_ctrl->b2));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFLPB0.bit.FFLPB0	= lpf_ctrl->b0;
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFLPB1.bit.FFLPB1	= lpf_ctrl->b1;
	IO_PRO.IMG_PIPE[unit_no].SRO.FF[ch].FFLPB2.bit.FFLPB2	= lpf_ctrl->b2;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Compensation mode start
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not running NG
*/
INT32 Im_PRO_SDC_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCTRG.bit.SDCTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_start_status(gIM_PRO_SDC_Status_Tbl[unit_no][ch].status, 0);

	return D_DDIM_OK;
}

/**
Compensation mode stop
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not stopped NG
*/
INT32 Im_PRO_SDC_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCTRG.bit.SDCTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCTRG.bit.SDCTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	im_pro_sro_set_stop_status(gIM_PRO_SDC_Status_Tbl[unit_no][ch].status, 0);

	return D_DDIM_OK;
}

/**
SDC control setup 
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	sdc_ctrl : SDC control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SDC_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, T_IM_PRO_SDC_CTRL* sdc_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (sdc_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SDC_Ctrl error. sdc_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCCTL.bit.SDCMD		= sdc_ctrl->sdc_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCCTL.bit.SDCPTO		= sdc_ctrl->afpb_output_ptn;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCCTL.bit.SDCMONO		= sdc_ctrl->monochrome_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCCTL.bit.SDCPDAFEN	= sdc_ctrl->afpb_enabled;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Select SDC gain table.
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	tbl_sel : SDC gain table select.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SDC_Select_Gain_Table( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, E_IM_PRO_SDC_GAIN_TABLE_SEL tbl_sel )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCSGD.bit.SDCSGD	= tbl_sel;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
set SDC gain table.
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	sdc_afpb_ctrl : SDC AFPB control information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SDC_Ctrl_Afpb( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, T_IM_PRO_SDC_CTRL_AFPB* sdc_afpb_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (sdc_afpb_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SDC_Ctrl_Afpb error. sdc_afpb_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	for(UINT32 i = 0; i < 9; i++) {
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPHW_MIN, D_IM_PRO_SDC_SDCPHW_MAX, sdc_afpb_ctrl->h_pattern0_valid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern0_valid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPHW_MIN, D_IM_PRO_SDC_SDCPHW_MAX, sdc_afpb_ctrl->h_pattern1_valid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern1_valid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPxHW_MIN, D_IM_PRO_SDC_SDCPxHW_MAX, sdc_afpb_ctrl->h_pattern0_r1_valid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern0_r1_valid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPxHW_MIN, D_IM_PRO_SDC_SDCPxHW_MAX, sdc_afpb_ctrl->h_pattern1_r1_valid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern1_r1_valid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPxHW_MIN, D_IM_PRO_SDC_SDCPxHW_MAX, sdc_afpb_ctrl->h_pattern0_r2_valid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern0_r2_valid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPxHW_MIN, D_IM_PRO_SDC_SDCPxHW_MAX, sdc_afpb_ctrl->h_pattern1_r2_valid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern1_r2_valid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCNxHW_MIN, D_IM_PRO_SDC_SDCNxHW_MAX, sdc_afpb_ctrl->h_pattern0_r1_invalid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern0_r1_invalid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCNxHW_MIN, D_IM_PRO_SDC_SDCNxHW_MAX, sdc_afpb_ctrl->h_pattern1_r1_invalid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern1_r1_invalid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCNxHW_MIN, D_IM_PRO_SDC_SDCNxHW_MAX, sdc_afpb_ctrl->h_pattern0_r2_invalid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern0_r2_invalid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCNxHW_MIN, D_IM_PRO_SDC_SDCNxHW_MAX, sdc_afpb_ctrl->h_pattern1_r2_invalid_width[i], "Im_PRO_SDC_Ctrl_Afpb : h_pattern1_r2_invalid_width" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
	};
	for(UINT32 i = 0; i < 64; i++) {
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPC_MIN, D_IM_PRO_SDC_SDCPC_MAX, sdc_afpb_ctrl->h_column_pattern0[i], "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern0" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPC_MIN, D_IM_PRO_SDC_SDCPC_MAX, sdc_afpb_ctrl->h_column_pattern1[i], "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern1" ) == FALSE ) {
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
	};
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPCW_MIN, D_IM_PRO_SDC_SDCPCW_MAX, sdc_afpb_ctrl->h_column_pattern_valid_width0, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern_valid_width0" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPCW_MIN, D_IM_PRO_SDC_SDCPCW_MAX, sdc_afpb_ctrl->h_column_pattern_valid_width1, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern_valid_width1" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPxCW_MIN, D_IM_PRO_SDC_SDCPxCW_MAX, sdc_afpb_ctrl->h_column_pattern1_valid_width0, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern1_valid_width0" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPxCW_MIN, D_IM_PRO_SDC_SDCPxCW_MAX, sdc_afpb_ctrl->h_column_pattern1_valid_width1, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern1_valid_width1" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPxCW_MIN, D_IM_PRO_SDC_SDCPxCW_MAX, sdc_afpb_ctrl->h_column_pattern2_valid_width0, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern2_valid_width0" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCPxCW_MIN, D_IM_PRO_SDC_SDCPxCW_MAX, sdc_afpb_ctrl->h_column_pattern2_valid_width1, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern2_valid_width1" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCNxCW_MIN, D_IM_PRO_SDC_SDCPxCW_MAX, sdc_afpb_ctrl->h_column_pattern1_invalid_width0, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern1_invalid_width0" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCNxCW_MIN, D_IM_PRO_SDC_SDCNxCW_MAX, sdc_afpb_ctrl->h_column_pattern1_invalid_width1, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern1_invalid_width1" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCN2CW_MIN, D_IM_PRO_SDC_SDCN2CW_MAX, sdc_afpb_ctrl->h_column_pattern2_invalid_width0, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern2_invalid_width0" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCN2CW_MIN, D_IM_PRO_SDC_SDCN2CW_MAX, sdc_afpb_ctrl->h_column_pattern2_invalid_width1, "Im_PRO_SDC_Ctrl_Afpb : h_column_pattern2_invalid_width1" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_SDC_SDCNMAX_MIN, D_IM_PRO_SDC_SDCNMAX_MAX, sdc_afpb_ctrl->afpb_neighbor_pixel_correction_level, "Im_PRO_SDC_Ctrl_Afpb : afpb_neighbor_pixel_correction_level" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH01.bit.SDCPH0_0		= sdc_afpb_ctrl->h_pattern0[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH02.bit.SDCPH0_1		= sdc_afpb_ctrl->h_pattern0[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH03.bit.SDCPH0_2		= sdc_afpb_ctrl->h_pattern0[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH04.bit.SDCPH0_3		= sdc_afpb_ctrl->h_pattern0[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH05.bit.SDCPH0_4		= sdc_afpb_ctrl->h_pattern0[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH06.bit.SDCPH0_5		= sdc_afpb_ctrl->h_pattern0[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH07.bit.SDCPH0_6		= sdc_afpb_ctrl->h_pattern0[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH08.bit.SDCPH0_7		= sdc_afpb_ctrl->h_pattern0[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH0.SDCPH09.bit.SDCPH0_8		= sdc_afpb_ctrl->h_pattern0[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH11.bit.SDCPH1_0		= sdc_afpb_ctrl->h_pattern1[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH12.bit.SDCPH1_1		= sdc_afpb_ctrl->h_pattern1[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH13.bit.SDCPH1_2		= sdc_afpb_ctrl->h_pattern1[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH14.bit.SDCPH1_3		= sdc_afpb_ctrl->h_pattern1[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH15.bit.SDCPH1_4		= sdc_afpb_ctrl->h_pattern1[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH16.bit.SDCPH1_5		= sdc_afpb_ctrl->h_pattern1[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH17.bit.SDCPH1_6		= sdc_afpb_ctrl->h_pattern1[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH18.bit.SDCPH1_7		= sdc_afpb_ctrl->h_pattern1[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPH1.SDCPH19.bit.SDCPH1_8		= sdc_afpb_ctrl->h_pattern1[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW01.bit.SDCPHW0_0		= sdc_afpb_ctrl->h_pattern0_valid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW01.bit.SDCPHW0_1		= sdc_afpb_ctrl->h_pattern0_valid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW01.bit.SDCPHW0_2		= sdc_afpb_ctrl->h_pattern0_valid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW01.bit.SDCPHW0_3		= sdc_afpb_ctrl->h_pattern0_valid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW02.bit.SDCPHW0_4		= sdc_afpb_ctrl->h_pattern0_valid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW02.bit.SDCPHW0_5		= sdc_afpb_ctrl->h_pattern0_valid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW02.bit.SDCPHW0_6		= sdc_afpb_ctrl->h_pattern0_valid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW02.bit.SDCPHW0_7		= sdc_afpb_ctrl->h_pattern0_valid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW0.SDCPHW03.bit.SDCPHW0_8		= sdc_afpb_ctrl->h_pattern0_valid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW11.bit.SDCPHW1_0		= sdc_afpb_ctrl->h_pattern1_valid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW11.bit.SDCPHW1_1		= sdc_afpb_ctrl->h_pattern1_valid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW11.bit.SDCPHW1_2		= sdc_afpb_ctrl->h_pattern1_valid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW11.bit.SDCPHW1_3		= sdc_afpb_ctrl->h_pattern1_valid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW12.bit.SDCPHW1_4		= sdc_afpb_ctrl->h_pattern1_valid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW12.bit.SDCPHW1_5		= sdc_afpb_ctrl->h_pattern1_valid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW12.bit.SDCPHW1_6		= sdc_afpb_ctrl->h_pattern1_valid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW12.bit.SDCPHW1_7		= sdc_afpb_ctrl->h_pattern1_valid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPHW1.SDCPHW13.bit.SDCPHW1_8		= sdc_afpb_ctrl->h_pattern1_valid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW01.bit.SDCP1HW0_0	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW01.bit.SDCP1HW0_1	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW02.bit.SDCP1HW0_2	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW02.bit.SDCP1HW0_3	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW03.bit.SDCP1HW0_4	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW03.bit.SDCP1HW0_5	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW04.bit.SDCP1HW0_6	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW04.bit.SDCP1HW0_7	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW0.SDCP1HW05.bit.SDCP1HW0_8	= sdc_afpb_ctrl->h_pattern0_r1_valid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW11.bit.SDCP1HW1_0	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW11.bit.SDCP1HW1_1	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW12.bit.SDCP1HW1_2	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW12.bit.SDCP1HW1_3	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW13.bit.SDCP1HW1_4	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW13.bit.SDCP1HW1_5	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW14.bit.SDCP1HW1_6	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW14.bit.SDCP1HW1_7	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1HW1.SDCP1HW15.bit.SDCP1HW1_8	= sdc_afpb_ctrl->h_pattern1_r1_valid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW01.bit.SDCN1HW0_0	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW01.bit.SDCN1HW0_1	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW02.bit.SDCN1HW0_2	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW02.bit.SDCN1HW0_3	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW03.bit.SDCN1HW0_4	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW03.bit.SDCN1HW0_5	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW04.bit.SDCN1HW0_6	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW04.bit.SDCN1HW0_7	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW0.SDCN1HW05.bit.SDCN1HW0_8	= sdc_afpb_ctrl->h_pattern0_r1_invalid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW11.bit.SDCN1HW1_0	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW11.bit.SDCN1HW1_1	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW12.bit.SDCN1HW1_2	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW12.bit.SDCN1HW1_3	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW13.bit.SDCN1HW1_4	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW13.bit.SDCN1HW1_5	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW14.bit.SDCN1HW1_6	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW14.bit.SDCN1HW1_7	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1HW1.SDCN1HW15.bit.SDCN1HW1_8	= sdc_afpb_ctrl->h_pattern1_r1_invalid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW01.bit.SDCP2HW0_0	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW01.bit.SDCP2HW0_1	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW02.bit.SDCP2HW0_2	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW02.bit.SDCP2HW0_3	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW03.bit.SDCP2HW0_4	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW03.bit.SDCP2HW0_5	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW04.bit.SDCP2HW0_6	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW04.bit.SDCP2HW0_7	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW0.SDCP2HW05.bit.SDCP2HW0_8	= sdc_afpb_ctrl->h_pattern0_r2_valid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW11.bit.SDCP2HW1_0	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW11.bit.SDCP2HW1_1	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW12.bit.SDCP2HW1_2	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW12.bit.SDCP2HW1_3	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW13.bit.SDCP2HW1_4	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW13.bit.SDCP2HW1_5	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW14.bit.SDCP2HW1_6	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW14.bit.SDCP2HW1_7	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2HW1.SDCP2HW15.bit.SDCP2HW1_8	= sdc_afpb_ctrl->h_pattern1_r2_valid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW01.bit.SDCN2HW0_0	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW01.bit.SDCN2HW0_1	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW02.bit.SDCN2HW0_2	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW02.bit.SDCN2HW0_3	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW03.bit.SDCN2HW0_4	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW03.bit.SDCN2HW0_5	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW04.bit.SDCN2HW0_6	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW04.bit.SDCN2HW0_7	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW0.SDCN2HW05.bit.SDCN2HW0_8	= sdc_afpb_ctrl->h_pattern0_r2_invalid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW11.bit.SDCN2HW1_0	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW11.bit.SDCN2HW1_1	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW12.bit.SDCN2HW1_2	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW12.bit.SDCN2HW1_3	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW13.bit.SDCN2HW1_4	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW13.bit.SDCN2HW1_5	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW14.bit.SDCN2HW1_6	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW14.bit.SDCN2HW1_7	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2HW1.SDCN2HW15.bit.SDCN2HW1_8	= sdc_afpb_ctrl->h_pattern1_r2_invalid_width[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC01.bit.SDCPC0_0		= sdc_afpb_ctrl->h_column_pattern0[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC01.bit.SDCPC0_1		= sdc_afpb_ctrl->h_column_pattern0[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC01.bit.SDCPC0_2		= sdc_afpb_ctrl->h_column_pattern0[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC01.bit.SDCPC0_3		= sdc_afpb_ctrl->h_column_pattern0[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC02.bit.SDCPC0_4		= sdc_afpb_ctrl->h_column_pattern0[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC02.bit.SDCPC0_5		= sdc_afpb_ctrl->h_column_pattern0[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC02.bit.SDCPC0_6		= sdc_afpb_ctrl->h_column_pattern0[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC02.bit.SDCPC0_7		= sdc_afpb_ctrl->h_column_pattern0[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC03.bit.SDCPC0_8		= sdc_afpb_ctrl->h_column_pattern0[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC03.bit.SDCPC0_9		= sdc_afpb_ctrl->h_column_pattern0[9];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC03.bit.SDCPC0_10		= sdc_afpb_ctrl->h_column_pattern0[10];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC03.bit.SDCPC0_11		= sdc_afpb_ctrl->h_column_pattern0[11];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC04.bit.SDCPC0_12		= sdc_afpb_ctrl->h_column_pattern0[12];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC04.bit.SDCPC0_13		= sdc_afpb_ctrl->h_column_pattern0[13];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC04.bit.SDCPC0_14		= sdc_afpb_ctrl->h_column_pattern0[14];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC04.bit.SDCPC0_15		= sdc_afpb_ctrl->h_column_pattern0[15];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC05.bit.SDCPC0_16		= sdc_afpb_ctrl->h_column_pattern0[16];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC05.bit.SDCPC0_17		= sdc_afpb_ctrl->h_column_pattern0[17];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC05.bit.SDCPC0_18		= sdc_afpb_ctrl->h_column_pattern0[18];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC05.bit.SDCPC0_19		= sdc_afpb_ctrl->h_column_pattern0[19];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC06.bit.SDCPC0_20		= sdc_afpb_ctrl->h_column_pattern0[20];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC06.bit.SDCPC0_21		= sdc_afpb_ctrl->h_column_pattern0[21];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC06.bit.SDCPC0_22		= sdc_afpb_ctrl->h_column_pattern0[22];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC06.bit.SDCPC0_23		= sdc_afpb_ctrl->h_column_pattern0[23];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC07.bit.SDCPC0_24		= sdc_afpb_ctrl->h_column_pattern0[24];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC07.bit.SDCPC0_25		= sdc_afpb_ctrl->h_column_pattern0[25];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC07.bit.SDCPC0_26		= sdc_afpb_ctrl->h_column_pattern0[26];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC07.bit.SDCPC0_27		= sdc_afpb_ctrl->h_column_pattern0[27];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC08.bit.SDCPC0_28		= sdc_afpb_ctrl->h_column_pattern0[28];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC08.bit.SDCPC0_29		= sdc_afpb_ctrl->h_column_pattern0[29];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC08.bit.SDCPC0_30		= sdc_afpb_ctrl->h_column_pattern0[30];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC08.bit.SDCPC0_31		= sdc_afpb_ctrl->h_column_pattern0[31];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC09.bit.SDCPC0_32		= sdc_afpb_ctrl->h_column_pattern0[32];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC09.bit.SDCPC0_33		= sdc_afpb_ctrl->h_column_pattern0[33];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC09.bit.SDCPC0_34		= sdc_afpb_ctrl->h_column_pattern0[34];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC09.bit.SDCPC0_35		= sdc_afpb_ctrl->h_column_pattern0[35];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC010.bit.SDCPC0_36		= sdc_afpb_ctrl->h_column_pattern0[36];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC010.bit.SDCPC0_37		= sdc_afpb_ctrl->h_column_pattern0[37];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC010.bit.SDCPC0_38		= sdc_afpb_ctrl->h_column_pattern0[38];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC010.bit.SDCPC0_39		= sdc_afpb_ctrl->h_column_pattern0[39];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC011.bit.SDCPC0_40		= sdc_afpb_ctrl->h_column_pattern0[40];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC011.bit.SDCPC0_41		= sdc_afpb_ctrl->h_column_pattern0[41];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC011.bit.SDCPC0_42		= sdc_afpb_ctrl->h_column_pattern0[42];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC011.bit.SDCPC0_43		= sdc_afpb_ctrl->h_column_pattern0[43];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC012.bit.SDCPC0_44		= sdc_afpb_ctrl->h_column_pattern0[44];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC012.bit.SDCPC0_45		= sdc_afpb_ctrl->h_column_pattern0[45];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC012.bit.SDCPC0_46		= sdc_afpb_ctrl->h_column_pattern0[46];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC012.bit.SDCPC0_47		= sdc_afpb_ctrl->h_column_pattern0[47];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC013.bit.SDCPC0_48		= sdc_afpb_ctrl->h_column_pattern0[48];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC013.bit.SDCPC0_49		= sdc_afpb_ctrl->h_column_pattern0[49];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC013.bit.SDCPC0_50		= sdc_afpb_ctrl->h_column_pattern0[50];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC013.bit.SDCPC0_51		= sdc_afpb_ctrl->h_column_pattern0[51];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC014.bit.SDCPC0_52		= sdc_afpb_ctrl->h_column_pattern0[52];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC014.bit.SDCPC0_53		= sdc_afpb_ctrl->h_column_pattern0[53];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC014.bit.SDCPC0_54		= sdc_afpb_ctrl->h_column_pattern0[54];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC014.bit.SDCPC0_55		= sdc_afpb_ctrl->h_column_pattern0[55];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC015.bit.SDCPC0_56		= sdc_afpb_ctrl->h_column_pattern0[56];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC015.bit.SDCPC0_57		= sdc_afpb_ctrl->h_column_pattern0[57];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC015.bit.SDCPC0_58		= sdc_afpb_ctrl->h_column_pattern0[58];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC015.bit.SDCPC0_59		= sdc_afpb_ctrl->h_column_pattern0[59];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC016.bit.SDCPC0_60		= sdc_afpb_ctrl->h_column_pattern0[60];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC016.bit.SDCPC0_61		= sdc_afpb_ctrl->h_column_pattern0[61];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC016.bit.SDCPC0_62		= sdc_afpb_ctrl->h_column_pattern0[62];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC0.SDCPC016.bit.SDCPC0_63		= sdc_afpb_ctrl->h_column_pattern0[63];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC11.bit.SDCPC1_0		= sdc_afpb_ctrl->h_column_pattern1[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC11.bit.SDCPC1_1		= sdc_afpb_ctrl->h_column_pattern1[1];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC11.bit.SDCPC1_2		= sdc_afpb_ctrl->h_column_pattern1[2];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC11.bit.SDCPC1_3		= sdc_afpb_ctrl->h_column_pattern1[3];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC12.bit.SDCPC1_4		= sdc_afpb_ctrl->h_column_pattern1[4];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC12.bit.SDCPC1_5		= sdc_afpb_ctrl->h_column_pattern1[5];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC12.bit.SDCPC1_6		= sdc_afpb_ctrl->h_column_pattern1[6];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC12.bit.SDCPC1_7		= sdc_afpb_ctrl->h_column_pattern1[7];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC13.bit.SDCPC1_8		= sdc_afpb_ctrl->h_column_pattern1[8];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC13.bit.SDCPC1_9		= sdc_afpb_ctrl->h_column_pattern1[9];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC13.bit.SDCPC1_10		= sdc_afpb_ctrl->h_column_pattern1[10];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC13.bit.SDCPC1_11		= sdc_afpb_ctrl->h_column_pattern1[11];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC14.bit.SDCPC1_12		= sdc_afpb_ctrl->h_column_pattern1[12];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC14.bit.SDCPC1_13		= sdc_afpb_ctrl->h_column_pattern1[13];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC14.bit.SDCPC1_14		= sdc_afpb_ctrl->h_column_pattern1[14];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC14.bit.SDCPC1_15		= sdc_afpb_ctrl->h_column_pattern1[15];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC15.bit.SDCPC1_16		= sdc_afpb_ctrl->h_column_pattern1[16];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC15.bit.SDCPC1_17		= sdc_afpb_ctrl->h_column_pattern1[17];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC15.bit.SDCPC1_18		= sdc_afpb_ctrl->h_column_pattern1[18];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC15.bit.SDCPC1_19		= sdc_afpb_ctrl->h_column_pattern1[19];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC16.bit.SDCPC1_20		= sdc_afpb_ctrl->h_column_pattern1[20];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC16.bit.SDCPC1_21		= sdc_afpb_ctrl->h_column_pattern1[21];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC16.bit.SDCPC1_22		= sdc_afpb_ctrl->h_column_pattern1[22];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC16.bit.SDCPC1_23		= sdc_afpb_ctrl->h_column_pattern1[23];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC17.bit.SDCPC1_24		= sdc_afpb_ctrl->h_column_pattern1[24];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC17.bit.SDCPC1_25		= sdc_afpb_ctrl->h_column_pattern1[25];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC17.bit.SDCPC1_26		= sdc_afpb_ctrl->h_column_pattern1[26];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC17.bit.SDCPC1_27		= sdc_afpb_ctrl->h_column_pattern1[27];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC18.bit.SDCPC1_28		= sdc_afpb_ctrl->h_column_pattern1[28];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC18.bit.SDCPC1_29		= sdc_afpb_ctrl->h_column_pattern1[29];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC18.bit.SDCPC1_30		= sdc_afpb_ctrl->h_column_pattern1[30];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC18.bit.SDCPC1_31		= sdc_afpb_ctrl->h_column_pattern1[31];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC19.bit.SDCPC1_32		= sdc_afpb_ctrl->h_column_pattern1[32];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC19.bit.SDCPC1_33		= sdc_afpb_ctrl->h_column_pattern1[33];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC19.bit.SDCPC1_34		= sdc_afpb_ctrl->h_column_pattern1[34];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC19.bit.SDCPC1_35		= sdc_afpb_ctrl->h_column_pattern1[35];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC110.bit.SDCPC1_36		= sdc_afpb_ctrl->h_column_pattern1[36];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC110.bit.SDCPC1_37		= sdc_afpb_ctrl->h_column_pattern1[37];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC110.bit.SDCPC1_38		= sdc_afpb_ctrl->h_column_pattern1[38];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC110.bit.SDCPC1_39		= sdc_afpb_ctrl->h_column_pattern1[39];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC111.bit.SDCPC1_40		= sdc_afpb_ctrl->h_column_pattern1[40];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC111.bit.SDCPC1_41		= sdc_afpb_ctrl->h_column_pattern1[41];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC111.bit.SDCPC1_42		= sdc_afpb_ctrl->h_column_pattern1[42];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC111.bit.SDCPC1_43		= sdc_afpb_ctrl->h_column_pattern1[43];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC112.bit.SDCPC1_44		= sdc_afpb_ctrl->h_column_pattern1[44];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC112.bit.SDCPC1_45		= sdc_afpb_ctrl->h_column_pattern1[45];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC112.bit.SDCPC1_46		= sdc_afpb_ctrl->h_column_pattern1[46];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC112.bit.SDCPC1_47		= sdc_afpb_ctrl->h_column_pattern1[47];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC113.bit.SDCPC1_48		= sdc_afpb_ctrl->h_column_pattern1[48];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC113.bit.SDCPC1_49		= sdc_afpb_ctrl->h_column_pattern1[49];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC113.bit.SDCPC1_50		= sdc_afpb_ctrl->h_column_pattern1[50];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC113.bit.SDCPC1_51		= sdc_afpb_ctrl->h_column_pattern1[51];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC114.bit.SDCPC1_52		= sdc_afpb_ctrl->h_column_pattern1[52];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC114.bit.SDCPC1_53		= sdc_afpb_ctrl->h_column_pattern1[53];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC114.bit.SDCPC1_54		= sdc_afpb_ctrl->h_column_pattern1[54];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC114.bit.SDCPC1_55		= sdc_afpb_ctrl->h_column_pattern1[55];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC115.bit.SDCPC1_56		= sdc_afpb_ctrl->h_column_pattern1[56];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC115.bit.SDCPC1_57		= sdc_afpb_ctrl->h_column_pattern1[57];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC115.bit.SDCPC1_58		= sdc_afpb_ctrl->h_column_pattern1[58];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC115.bit.SDCPC1_59		= sdc_afpb_ctrl->h_column_pattern1[59];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC116.bit.SDCPC1_60		= sdc_afpb_ctrl->h_column_pattern1[60];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC116.bit.SDCPC1_61		= sdc_afpb_ctrl->h_column_pattern1[61];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC116.bit.SDCPC1_62		= sdc_afpb_ctrl->h_column_pattern1[62];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPC1.SDCPC116.bit.SDCPC1_63		= sdc_afpb_ctrl->h_column_pattern1[63];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPCW0.bit.SDCPCW0				= sdc_afpb_ctrl->h_column_pattern_valid_width0;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPCW1.bit.SDCPCW1				= sdc_afpb_ctrl->h_column_pattern_valid_width1;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1CW0.bit.SDCP1CW0				= sdc_afpb_ctrl->h_column_pattern1_valid_width0;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP1CW1.bit.SDCP1CW1				= sdc_afpb_ctrl->h_column_pattern1_valid_width1;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2CW0.bit.SDCP2CW0				= sdc_afpb_ctrl->h_column_pattern2_valid_width0;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCP2CW1.bit.SDCP2CW1				= sdc_afpb_ctrl->h_column_pattern2_valid_width1;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1CW0.bit.SDCN1CW0				= sdc_afpb_ctrl->h_column_pattern1_invalid_width0;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN1CW1.bit.SDCN1CW1				= sdc_afpb_ctrl->h_column_pattern1_invalid_width1;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2CW0.bit.SDCN2CW0				= sdc_afpb_ctrl->h_column_pattern2_invalid_width0;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCN2CW1.bit.SDCN2CW1				= sdc_afpb_ctrl->h_column_pattern2_invalid_width1;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCGMAXMIN.bit.SDCGMAX				= sdc_afpb_ctrl->afpb_gain_correction_u_threshold;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCGMAXMIN.bit.SDCGMIN				= sdc_afpb_ctrl->af_gain_correction_l_threshold;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCNMAX.bit.SDCNMAX				= sdc_afpb_ctrl->afpb_neighbor_pixel_correction_level;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
set SDC gain table.
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	tbl_sel : SDC gain table select.
@param[in]	gain_table : SDC gain table information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SDC_Set_Gain_Table( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, E_IM_PRO_SDC_GAIN_TABLE_SEL tbl_sel, T_IM_PRO_SDC_GAIN_TABLE* gain_table )
{
#ifdef CO_PARAM_CHECK
	if (gain_table == NULL){
		Ddim_Assertion(("I:Im_PRO_SDC_Set_Gain_Table error. gain_table=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( ( IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCTRG.bit.SDCTRG != D_IM_PRO_TRG_STATUS_STOPPED ) && ( IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCSGD.bit.SDCSGD == tbl_sel ) ) {
		Ddim_Assertion(("I:Im_PRO_SDC_Set_Gain_Table error. macro has not stopped, and RAM table that referrenced from SDC can not overwrite until macro stopping.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif

	im_pro_on_hclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	for( INT32 i = 0; i < D_IM_PRO_SDC_GAIN_TBL_NUM; i++ ) {
		gIM_PRO_SDC_Status_Tbl[unit_no][ch].tbl_reg_ptr[tbl_sel].bit[i].SDCGD	= gain_table->gain_table[i];
	}
	im_pro_off_hclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of SDC.
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SDC_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCTRG.bit.SDCTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCPAEN.bit.SDCPAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of SDC control.
@param[in]		unit_no : Unit number.
@param[in]		ch : Channel No
@param[out]		addr					: Top address of the address array of SDC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_SDC_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, const T_IM_PRO_RDMA_SDC_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_SDC_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &gIM_PRO_SDC_Addr[unit_no][ch];

	return D_DDIM_OK;
}


INT32 Im_PRO_Get_RdmaAddr_SDC_Gain_Tbl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, E_IM_PRO_SDC_GAIN_TABLE_SEL tbl_sel, const T_IM_PRO_RDMA_SDC_GAIN_TBL_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_SDC_Gain_Tbl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &gIM_PRO_SDC_Gain_Tbl_Addr[unit_no][ch][tbl_sel];

	return D_DDIM_OK;
}
