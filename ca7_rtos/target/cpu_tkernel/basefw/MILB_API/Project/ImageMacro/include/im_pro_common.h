/**
 * @file		im_pro_common.h
 * @brief		pre process common definition header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IM_PRO_COMMON_H_
#define _IM_PRO_COMMON_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_DEBUG_PRINT_IM_PRO
#define CO_PRO_DEBUG_PRINT_FUNC

#ifdef CO_PRO_DEBUG_PRINT_FUNC
#define Im_PRO_Print_Pcdbg	Ddim_Print
#else
#define Im_PRO_Print_Pcdbg(dmy)
#endif


#define	D_IM_PRO_SR_RELEASE			(0)				// SW reset release
#define	D_IM_PRO_SR_RESET			(1)				// SW reset state
#define	D_IM_PRO_CLOCK_ON			(0)				// To supply the clock to the PRO
#define	D_IM_PRO_CLOCK_OFF			(1)				// Does not supply the clock to the PRO

#define D_IM_PRO_TRG_FRAME_STOP		(0)				// Trigger Frame STOP
#define D_IM_PRO_TRG_START			(1)				// Trigger START
#define D_IM_PRO_TRG_FORCE_STOP		(2)				// Trigger Force STOP

#define D_IM_PRO_TRG_STATUS_STOPPED	(2)				// Macro is stopped.
#define D_IM_PRO_TRG_STATUS_RUNNING	(3)				// Macro is reunning.

#define	D_IM_PRO_FUNC_EN			(1)				// PRO function Enable
#define	D_IM_PRO_FUNC_DIS			(0)				// PRO function Disable

#define	D_IM_PRO_COMMON_PG_PRE_STARTED_STATUS_MAX	(8)					/* Pre-Started Macro Status managing MAX count for PG */
#define	D_IM_PRO_COMMON_P2M_PRE_STARTED_STATUS_MAX	(2)					/* Pre-Started Macro Status managing MAX count for P2M */
#define	D_IM_PRO_COMMON_M2P_PRE_STARTED_STATUS_MAX	(2)					/* Pre-Started Macro Status managing MAX count for M2P */

#define	D_IM_PRO_PIPE_NUM			(2)	// Image pipe number

#define D_IM_PRO_B2B_FSHD_CTBL_CNT	(E_IM_PRO_FSHD_LUT_SEL_RAM1 + 1)
#define D_IM_PRO_PAS_SPT_TBL_CNT	(E_IM_PRO_SPT_TBL_SEL_SPTTBL1 + 1)
#define D_IM_PRO_SRO_SDC_TBL_CNT	(E_IM_PRO_SDC_GAIN_TABLE_SEL_1 + 1)

#define D_IM_PRO_PGAIN_CNT			(E_IM_PRO_PGAIN2_SELECT + 1)

#define	D_IM_PRO_OB_BUFFER_MIN		(0)				// OB Data Table Index Min
#define	D_IM_PRO_OB_BUFFER_MAX		(4)				// OB Data Table Index Max

#define D_IM_PRO_AF_BUFFER_MAX		(7)

#define D_IM_PRO_HIST_BUFFER_MAX	(4)

// PRO Channel/Macro Flag for MCLK/ICLK control
#define	D_IM_SEN_STATUS_NONE		(0x00000000UL)			/* All stopped		*/
#define	D_IM_SEN_STATUS_LVDS0		(0x00000000UL)&(1<<0)	/* LVDS0			*/
#define	D_IM_SEN_STATUS_LVDS1		(0x00000000UL)&(1<<1)	/* LVDS1			*/
#define	D_IM_SEN_STATUS_LVDS2		(0x00000000UL)&(1<<2)	/* LVDS2			*/
#define	D_IM_SEN_STATUS_LVDS3		(0x00000000UL)&(1<<3)	/* LVDS3			*/
#define	D_IM_SEN_STATUS_DMIPI0		(0x00000000UL)&(1<<4)	/* MIPI D-PHY0		*/
#define	D_IM_SEN_STATUS_DMIPI1		(0x00000000UL)&(1<<5)	/* MIPI D-PHY1		*/
#define	D_IM_SEN_STATUS_DMIPI2		(0x00000000UL)&(1<<6)	/* MIPI D-PHY2		*/
#define	D_IM_SEN_STATUS_DMIPI3		(0x00000000UL)&(1<<7)	/* MIPI D-PHY3		*/
#define	D_IM_SEN_STATUS_CMIPI0		(0x00000000UL)&(1<<8)	/* MIPI C-PHY0		*/
#define	D_IM_SEN_STATUS_CMIPI1		(0x00000000UL)&(1<<9)	/* MIPI C-PHY1		*/
#define	D_IM_SEN_STATUS_CMIPI2		(0x00000000UL)&(1<<10)	/* MIPI C-PHY2		*/
#define	D_IM_SEN_STATUS_CMIPI3		(0x00000000UL)&(1<<11)	/* MIPI C-PHY3		*/
#define	D_IM_SEN_STATUS_SLVSEC0		(0x00000000UL)&(1<<12)	/* SLVS-EC stream 0	*/
#define	D_IM_SEN_STATUS_SLVSEC1		(0x00000000UL)&(1<<13)	/* SLVS-EC stream 1	*/
#define	D_IM_SEN_STATUS_SG0			(0x00000000UL)&(1<<14)	/* SG0				*/
#define	D_IM_SEN_STATUS_SG1			(0x00000000UL)&(1<<15)	/* SG1				*/
#define	D_IM_SEN_STATUS_SG2			(0x00000000UL)&(1<<16)	/* SG2				*/
#define	D_IM_SEN_STATUS_SG3			(0x00000000UL)&(1<<17)	/* SG3				*/
#define	D_IM_SEN_STATUS_LDIV0		(0x00000000UL)&(1<<18)	/* LDIV0			*/
#define	D_IM_SEN_STATUS_LDIV1		(0x00000000UL)&(1<<19)	/* LDIV1			*/
#define	D_IM_SEN_STATUS_LDIV2		(0x00000000UL)&(1<<20)	/* LDIV2			*/
#define	D_IM_SEN_STATUS_LDIV3		(0x00000000UL)&(1<<21)	/* LDIV3			*/
#define	D_IM_SEN_STATUS_MONI0		(0x00000000UL)&(1<<22)	/* MONI0			*/
#define	D_IM_SEN_STATUS_MONI1		(0x00000000UL)&(1<<23)	/* MONI1			*/
#define	D_IM_SEN_STATUS_MONI2		(0x00000000UL)&(1<<24)	/* MONI2			*/
#define	D_IM_SEN_STATUS_MONI3		(0x00000000UL)&(1<<25)	/* MONI3			*/
#define	D_IM_SEN_STATUS_OBT00		(0x10000000UL)&(1<<0)	/* OBT00			*/
#define	D_IM_SEN_STATUS_OBT01		(0x10000000UL)&(1<<1)	/* OBT01			*/
#define	D_IM_SEN_STATUS_OBT10		(0x10000000UL)&(1<<2)	/* OBT10			*/
#define	D_IM_SEN_STATUS_OBT11		(0x10000000UL)&(1<<3)	/* OBT11			*/
#define	D_IM_SEN_STATUS_OBT20		(0x10000000UL)&(1<<4)	/* OBT20			*/
#define	D_IM_SEN_STATUS_OBT21		(0x10000000UL)&(1<<5)	/* OBT21			*/
#define	D_IM_SEN_STATUS_OBT30		(0x10000000UL)&(1<<6)	/* OBT30			*/
#define	D_IM_SEN_STATUS_OBT31		(0x10000000UL)&(1<<7)	/* OBT31			*/
#define	D_IM_SEN_STATUS_OBD00		(0x10000000UL)&(1<<8)	/* OBD00			*/
#define	D_IM_SEN_STATUS_OBD01		(0x10000000UL)&(1<<9)	/* OBD01			*/
#define	D_IM_SEN_STATUS_OBD10		(0x10000000UL)&(1<<10)	/* OBD10			*/
#define	D_IM_SEN_STATUS_OBD11		(0x10000000UL)&(1<<11)	/* OBD11			*/
#define	D_IM_SEN_STATUS_OBD20		(0x10000000UL)&(1<<12)	/* OBD20			*/
#define	D_IM_SEN_STATUS_OBD21		(0x10000000UL)&(1<<13)	/* OBD21			*/
#define	D_IM_SEN_STATUS_OBD30		(0x10000000UL)&(1<<14)	/* OBD30			*/
#define	D_IM_SEN_STATUS_OBD31		(0x10000000UL)&(1<<15)	/* OBD31			*/

#define	D_IM_PRO_COMMON_STATUS_NONE				(0x00000000UL)			/* All stopped		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_P2M0			(0x00000000UL)&(1<<0)	/* SEN P2M0			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_P2M1			(0x00000000UL)&(1<<1)	/* SEN P2M1			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_P2M2			(0x00000000UL)&(1<<2)	/* SEN P2M2			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_P2M3			(0x00000000UL)&(1<<3)	/* SEN P2M3			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_P2M4			(0x00000000UL)&(1<<4)	/* SEN P2M4			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_P2M5			(0x00000000UL)&(1<<5)	/* SEN P2M5			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_P2M6			(0x00000000UL)&(1<<6)	/* SEN P2M6			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_P2M7			(0x00000000UL)&(1<<7)	/* SEN P2M7			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_PWCH0		(0x00000000UL)&(1<<8)	/* SEN PWCH0		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_PWCH1		(0x00000000UL)&(1<<9)	/* SEN PWCH1		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_PWCH2		(0x00000000UL)&(1<<10)	/* SEN PWCH2		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_PWCH3		(0x00000000UL)&(1<<11)	/* SEN PWCH3		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_PWCH4		(0x00000000UL)&(1<<12)	/* SEN PWCH4		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_PWCH5		(0x00000000UL)&(1<<13)	/* SEN PWCH5		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_PWCH6		(0x00000000UL)&(1<<14)	/* SEN PWCH6		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_PWCH7		(0x00000000UL)&(1<<15)	/* SEN PWCH7		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_LNR0			(0x00000000UL)&(1<<16)	/* SEN LNR0			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_LNR1			(0x00000000UL)&(1<<17)	/* SEN LNR1			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_LNR2			(0x00000000UL)&(1<<18)	/* SEN LNR2			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_LNR3			(0x00000000UL)&(1<<19)	/* SEN LNR3			*/
#define	D_IM_PRO_COMMON_STATUS_SEN_MONI0		(0x00000000UL)&(1<<20)	/* SEN MONI0		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_MONI1		(0x00000000UL)&(1<<21)	/* SEN MONI1		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_MONI2		(0x00000000UL)&(1<<22)	/* SEN MONI2		*/
#define	D_IM_PRO_COMMON_STATUS_SEN_MONI3		(0x00000000UL)&(1<<23)	/* SEN MONI3		*/

#define	D_IM_PRO_COMMON_STATUS_SRO1_P2M0		(0x10000000UL)&(1<<0)	/* SRO1 P2M0		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_P2M1		(0x10000000UL)&(1<<1)	/* SRO1 P2M1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_P2M2		(0x10000000UL)&(1<<2)	/* SRO1 P2M2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_P2M3		(0x10000000UL)&(1<<3)	/* SRO1 P2M3		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_P2M4		(0x10000000UL)&(1<<4)	/* SRO1 P2M4		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_P2M5		(0x10000000UL)&(1<<5)	/* SRO1 P2M5		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_P2M0		(0x10000000UL)&(1<<6)	/* SRO2 P2M0		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_P2M1		(0x10000000UL)&(1<<7)	/* SRO2 P2M1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_P2M2		(0x10000000UL)&(1<<8)	/* SRO2 P2M2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_P2M3		(0x10000000UL)&(1<<9)	/* SRO2 P2M3		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_P2M4		(0x10000000UL)&(1<<10)	/* SRO2 P2M4		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_P2M5		(0x10000000UL)&(1<<11)	/* SRO2 P2M5		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PWCH0		(0x10000000UL)&(1<<12)	/* SRO1 PWCH0		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PWCH1		(0x10000000UL)&(1<<13)	/* SRO1 PWCH1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PWCH2		(0x10000000UL)&(1<<14)	/* SRO1 PWCH2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PWCH3		(0x10000000UL)&(1<<15)	/* SRO1 PWCH3		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PWCH4		(0x10000000UL)&(1<<16)	/* SRO1 PWCH4		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PWCH5		(0x10000000UL)&(1<<17)	/* SRO1 PWCH5		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PWCH0		(0x10000000UL)&(1<<18)	/* SRO2 PWCH0		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PWCH1		(0x10000000UL)&(1<<19)	/* SRO2 PWCH1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PWCH2		(0x10000000UL)&(1<<20)	/* SRO2 PWCH2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PWCH3		(0x10000000UL)&(1<<21)	/* SRO2 PWCH3		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PWCH4		(0x10000000UL)&(1<<22)	/* SRO2 PWCH4		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PWCH5		(0x10000000UL)&(1<<23)	/* SRO2 PWCH5		*/

#define	D_IM_PRO_COMMON_STATUS_SRO1_M2P0		(0x20000000UL)&(1<<0)	/* SRO1 M2P0		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_M2P1		(0x20000000UL)&(1<<1)	/* SRO1 M2P1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_M2P2		(0x20000000UL)&(1<<2)	/* SRO1 M2P2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_M2P3		(0x20000000UL)&(1<<3)	/* SRO1 M2P3		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_M2P6		(0x20000000UL)&(1<<4)	/* SRO1 M2P6		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_M2P7		(0x20000000UL)&(1<<5)	/* SRO1 M2P7		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_M2P8		(0x20000000UL)&(1<<6)	/* SRO1 M2P8		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_M2P9		(0x20000000UL)&(1<<7)	/* SRO1 M2P9		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_M2P0		(0x20000000UL)&(1<<8)	/* SRO2 M2P0		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_M2P1		(0x20000000UL)&(1<<9)	/* SRO2 M2P1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_M2P2		(0x20000000UL)&(1<<10)	/* SRO2 M2P2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_M2P3		(0x20000000UL)&(1<<11)	/* SRO2 M2P3		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_M2P6		(0x20000000UL)&(1<<12)	/* SRO2 M2P6		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_M2P7		(0x20000000UL)&(1<<13)	/* SRO2 M2P7		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_M2P8		(0x20000000UL)&(1<<14)	/* SRO2 M2P8		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_M2P9		(0x20000000UL)&(1<<15)	/* SRO2 M2P9		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH0		(0x20000000UL)&(1<<16)	/* SRO1 PRCH0		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH1		(0x20000000UL)&(1<<17)	/* SRO1 PRCH1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH2		(0x20000000UL)&(1<<18)	/* SRO1 PRCH2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH3		(0x20000000UL)&(1<<19)	/* SRO1 PRCH3		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH4		(0x20000000UL)&(1<<20)	/* SRO1 PRCH4		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH5		(0x20000000UL)&(1<<21)	/* SRO1 PRCH5		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH6		(0x20000000UL)&(1<<22)	/* SRO1 PRCH6		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH7		(0x20000000UL)&(1<<23)	/* SRO1 PRCH7		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH8		(0x20000000UL)&(1<<24)	/* SRO1 PRCH8		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PRCH9		(0x20000000UL)&(1<<25)	/* SRO1 PRCH9		*/

#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH0		(0x30000000UL)&(1<<0)	/* SRO2 PRCH0		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH1		(0x30000000UL)&(1<<1)	/* SRO2 PRCH1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH2		(0x30000000UL)&(1<<2)	/* SRO2 PRCH2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH3		(0x30000000UL)&(1<<3)	/* SRO2 PRCH3		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH4		(0x30000000UL)&(1<<4)	/* SRO2 PRCH4		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH5		(0x30000000UL)&(1<<5)	/* SRO2 PRCH5		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH6		(0x30000000UL)&(1<<6)	/* SRO2 PRCH6		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH7		(0x30000000UL)&(1<<7)	/* SRO2 PRCH7		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH8		(0x30000000UL)&(1<<8)	/* SRO2 PRCH8		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PRCH9		(0x30000000UL)&(1<<9)	/* SRO2 PRCH9		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_MONI		(0x30000000UL)&(1<<10)	/* SRO1 MONI		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_MONI		(0x30000000UL)&(1<<11)	/* SRO2 MONI		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PG			(0x30000000UL)&(1<<12)	/* SRO1 PG			*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PG			(0x30000000UL)&(1<<13)	/* SRO2 PG			*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PGAIN		(0x30000000UL)&(1<<14)	/* SRO1 PGAIN		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PGAIN		(0x30000000UL)&(1<<15)	/* SRO2 PGAIN		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PZSFTB1		(0x30000000UL)&(1<<16)	/* SRO1 PZSFTB1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PZSFTB2		(0x30000000UL)&(1<<17)	/* SRO1 PZSFTB2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PZSFTC1		(0x30000000UL)&(1<<18)	/* SRO1 PZSFTC1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO1_PZSFTC2		(0x30000000UL)&(1<<19)	/* SRO1 PZSFTC2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PZSFTB1		(0x30000000UL)&(1<<20)	/* SRO2 PZSFTB1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PZSFTB2		(0x30000000UL)&(1<<21)	/* SRO2 PZSFTB2		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PZSFTC1		(0x30000000UL)&(1<<22)	/* SRO2 PZSFTC1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_PZSFTC2		(0x30000000UL)&(1<<23)	/* SRO2 PZSFTC2		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_P2M0		(0x30000000UL)&(1<<24)	/* B2B1 P2M0		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_P2M1		(0x30000000UL)&(1<<25)	/* B2B1 P2M1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_P2M0		(0x30000000UL)&(1<<26)	/* B2B2 P2M0		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_P2M1		(0x30000000UL)&(1<<27)	/* B2B2 P2M1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PWCH0		(0x40000000UL)&(1<<0)	/* B2B1 PWCH0		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PWCH1		(0x40000000UL)&(1<<1)	/* B2B1 PWCH1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PWCH0		(0x40000000UL)&(1<<2)	/* B2B2 PWCH0		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PWCH1		(0x40000000UL)&(1<<3)	/* B2B2 PWCH1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_M2P0		(0x40000000UL)&(1<<4)	/* B2B1 M2P0		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_M2P1		(0x40000000UL)&(1<<5)	/* B2B1 M2P1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_M2P2		(0x40000000UL)&(1<<6)	/* B2B1 M2P2		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_M2P3		(0x40000000UL)&(1<<7)	/* B2B1 M2P3		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_M2P0		(0x40000000UL)&(1<<8)	/* B2B2 M2P0		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_M2P1		(0x40000000UL)&(1<<9)	/* B2B2 M2P1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_M2P2		(0x40000000UL)&(1<<10)	/* B2B2 M2P2		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_M2P3		(0x40000000UL)&(1<<11)	/* B2B2 M2P3		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PRCH0		(0x40000000UL)&(1<<12)	/* B2B1 PRCH0		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PRCH1		(0x40000000UL)&(1<<13)	/* B2B1 PRCH1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PRCH2		(0x40000000UL)&(1<<14)	/* B2B1 PRCH2		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PRCH3		(0x40000000UL)&(1<<15)	/* B2B1 PRCH3		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PRCH4		(0x40000000UL)&(1<<16)	/* B2B1 PRCH4		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PRCH0		(0x40000000UL)&(1<<17)	/* B2B2 PRCH0		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PRCH1		(0x40000000UL)&(1<<18)	/* B2B2 PRCH1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PRCH2		(0x40000000UL)&(1<<19)	/* B2B2 PRCH2		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PRCH3		(0x40000000UL)&(1<<20)	/* B2B2 PRCH3		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PRCH4		(0x40000000UL)&(1<<21)	/* B2B2 PRCH4		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PG			(0x40000000UL)&(1<<22)	/* B2B1 PG			*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PG			(0x40000000UL)&(1<<23)	/* B2B2 PG			*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PGAIN1		(0x40000000UL)&(1<<24)	/* B2B1 PGAIN1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PGAIN2		(0x40000000UL)&(1<<25)	/* B2B1 PGAIN2		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PGAIN1		(0x40000000UL)&(1<<26)	/* B2B2 PGAIN1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PGAIN2		(0x40000000UL)&(1<<27)	/* B2B2 PGAIN2		*/

#define	D_IM_PRO_COMMON_STATUS_B2B1_PZSFTA1		(0x50000000UL)&(1<<0)	/* B2B1 PZSFTA1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PZSFTA2		(0x50000000UL)&(1<<1)	/* B2B1 PZSFTA2		*/
#define	D_IM_PRO_COMMON_STATUS_B2B1_PZSFTA2A	(0x50000000UL)&(1<<2)	/* B2B1 PZSFTA2A	*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PZSFTA1		(0x50000000UL)&(1<<3)	/* B2B2 PZSFTA1		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PZSFTA2		(0x50000000UL)&(1<<4)	/* B2B2 PZSFTA2		*/
#define	D_IM_PRO_COMMON_STATUS_B2B2_PZSFTA2A	(0x50000000UL)&(1<<5)	/* B2B2 PZSFTA2A	*/
#define	D_IM_PRO_COMMON_STATUS_STAT_PWCH0		(0x50000000UL)&(1<<6)	/* STAT PWCH0		*/
#define	D_IM_PRO_COMMON_STATUS_STAT_PWCH1		(0x50000000UL)&(1<<7)	/* STAT PWCH1		*/
#define	D_IM_PRO_COMMON_STATUS_STAT_PWCH2		(0x50000000UL)&(1<<8)	/* STAT PWCH2		*/
#define	D_IM_PRO_COMMON_STATUS_STAT_PWCH3		(0x50000000UL)&(1<<9)	/* STAT PWCH3		*/
#define	D_IM_PRO_COMMON_STATUS_STAT_PWCH4		(0x50000000UL)&(1<<10)	/* STAT PWCH4		*/
#define	D_IM_PRO_COMMON_STATUS_STAT_PWCH5		(0x50000000UL)&(1<<11)	/* STAT PWCH5		*/
#define	D_IM_PRO_COMMON_STATUS_PAS_P2M0			(0x50000000UL)&(1<<12)	/* PAS P2M0			*/
#define	D_IM_PRO_COMMON_STATUS_PAS_PWCH0		(0x50000000UL)&(1<<13)	/* PAS PWCH0		*/
#define	D_IM_PRO_COMMON_STATUS_PAS_M2P0			(0x50000000UL)&(1<<14)	/* PAS M2P0			*/
#define	D_IM_PRO_COMMON_STATUS_PAS_PRCH0		(0x50000000UL)&(1<<15)	/* PAS PRCH0		*/
#define	D_IM_PRO_COMMON_STATUS_PAS_LNR			(0x50000000UL)&(1<<16)	/* PAS LNR			*/
#define	D_IM_PRO_COMMON_STATUS_PAS_PG			(0x50000000UL)&(1<<17)	/* PAS PG			*/
#define	D_IM_PRO_COMMON_STATUS_PAS_PGAIN		(0x50000000UL)&(1<<18)	/* PAS PGAIN		*/
// --- REMOVE_BUZ BEGIN
#define	D_IM_PRO_COMMON_STATUS_SRO1_MONI1		(0x50000000UL)&(1<<19)	/* SRO1 MONI1		*/
#define	D_IM_PRO_COMMON_STATUS_SRO2_MONI1		(0x50000000UL)&(1<<20)	/* SRO2 MONI1		*/
// --- REMOVE_BUZ END

#define	D_IM_PRO_FSHD_PRE_STARTED_STATUS_MAX	(8)						/* Pre-Started Macro Status managing MAX count for FSHD */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define Im_Pro_Dsb()	Dd_ARM_Dsb_Pou()

// The structure type matching check.
#ifdef IM_PRO_REG_TYPE_CHECK
#define im_pro_check_reg_type( dst, src )	(dst) = (src);
#else
#define im_pro_check_reg_type( dst, src )
#endif

// Signed register access macros.
#define im_pro_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_pro_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}

#define im_pro_set_reg_signed_a( reg, type, member, val )		\
		{ \
			type work = { .word[0] = 0xFFFFFFFFUL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_pro_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}

#define im_pro_get_reg_signed( val, reg, type, member )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_pro_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

#define im_pro_get_reg_signed_a( val, reg, type, member )		\
		{ \
			type work = { .word[0] = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_pro_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

#define im_pro_wait_usec( usec )				Dd_ARM_Wait_ns( usec * 1000 )


#define im_pro_common_wait_by_clk_cycle( wait_cycle_cnt, wait_unit_clk_hz, cpu_clk_hz )		\
		{ \
			ULONG nop_cnt = 0; \
			nop_cnt = (wait_cycle_cnt * ( 1 / (double)wait_unit_clk_hz ) ) / ( 1 / (double)cpu_clk_hz ); \
			for (UINT32 nop_loop_cnt = 0; nop_loop_cnt < nop_cnt; nop_loop_cnt++) { }; \
		}

#define im_pro_spin_lock_start(unit_no, block_type, val )	Dd_ARM_Critical_Section_Start( val[unit_no][block_type].lock )
#define im_pro_spin_lock_end(unit_no, block_type, val)		Dd_ARM_Critical_Section_End( val[unit_no][block_type].lock )


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_IM_PRO_AF_RESULT_TYPE_EVAL_CALC = 0,
	E_IM_PRO_AF_RESULT_TYPE_EVAL_BRIGHT,
	E_IM_PRO_AF_RESULT_TYPE_EVAL_DARK,
	E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL0,
	E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL1,
	E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL2,
	E_IM_PRO_AF_RESULT_TYPE_VERTICAL,
	E_IM_PRO_AF_RESULT_TYPE_EVALUATED_VALUE_MAX,
	E_IM_PRO_AF_RESULT_TYPE_EVALUATED_VALUE_MIN,
	E_IM_PRO_AF_RESULT_TYPE_EVALUATION_VALUE_MAX,
	E_IM_PRO_AF_RESULT_TYPE_EVALUATION_VALUE_MIN,
	E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_0_EACH_LINE_MAX,
	E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_1_EACH_LINE_MAX,
	E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_2_EACH_LINE_MAX,
} E_IM_PRO_AF_RESULT_TYPE;

typedef enum {
	E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT,
	E_IM_PRO_AF_RESULT_DATA_TYPE_LAST,
} E_IM_PRO_AF_RESULT_DATA_TYPE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Table Index Structure
typedef struct {
	UCHAR	max;	// index max value
	UCHAR	cur;	// current index value
} T_IM_PRO_INDEX;

// Bayer Address Mng Table
typedef struct {
	ULONG			top_addr[D_IM_PRO_PWCH_BUFFER_MAX];		// Bayer Address
	ULONG			buff_last_addr;						// Latest Bayer Address
	T_IM_PRO_INDEX	buff_index;								// Bayer Address Table Index
} T_IM_PRO_ADDR_MNG;

typedef struct {
	volatile struct io_p2m*		reg_ptr;
	volatile union io_p2mkntbl*	kntbl_ptr;
	UINT32						status;
	UINT32						pre_started_status[D_IM_PRO_COMMON_P2M_PRE_STARTED_STATUS_MAX];
} T_IM_PRO_COMMON_P2M_INFO;

typedef struct {
	volatile struct io_pwch*	reg_ptr;
	UINT32						status;
} T_IM_PRO_COMMON_PWCH_INFO;

typedef struct {
	volatile struct io_m2p*			reg_ptr;
	volatile union io_m2pdkntbl*	dkntbl_ptr;
	UINT32							status;
	UINT32							pre_started_status[D_IM_PRO_COMMON_M2P_PRE_STARTED_STATUS_MAX];
} T_IM_PRO_COMMON_M2P_INFO;

typedef struct {
	volatile struct io_prch*	reg_ptr;
	UINT32						status;
} T_IM_PRO_COMMON_PRCH_INFO;

typedef struct {
	volatile struct io_lnr*		reg_ptr;
	UINT32						status;
} T_IM_PRO_COMMON_LNR_INFO;

typedef struct {
	volatile struct io_pgain*	reg_ptr;
	UINT32						status;
} T_IM_PRO_COMMON_PGAIN_INFO;

typedef struct {
	volatile struct io_pzsft*	reg_ptr;
	UINT32						status;
} T_IM_PRO_COMMON_PZSFT_INFO;

typedef struct {
	volatile struct io_pg*		reg_ptr;
	UINT32						status;
	UINT32						pre_started_status[D_IM_PRO_COMMON_PG_PRE_STARTED_STATUS_MAX];
} T_IM_PRO_COMMON_PG_INFO;

typedef struct {
	volatile struct io_moni*	reg_ptr;
	UINT32						status;
} T_IM_PRO_COMMON_MONI_INFO;

typedef struct {
	volatile struct io_fshdl*		reg_ptr;
	UINT32							status;
} T_IM_PRO_SRO_FSHDL_INFO;

typedef struct {
	volatile union io_sro_sdcgd*	tbl_reg_ptr;
	UINT32							status;
} T_IM_PRO_SRO_SDC_INFO;


typedef struct {
	volatile VOID*	reg_addr;
	UINT32			reg_bit;
	volatile UCHAR*	reg_ctrl_cnt;
} T_IM_PRO_CLK_REG_INFO;

typedef struct {
	UCHAR	block_num;
	UCHAR	ch_num;
} T_IM_PRO_CH_COMBI;

typedef struct {
	T_IM_PRO_INDEX			addr_index;
	volatile T_IM_PRO_RGB2*	data_addr;
	T_IM_PRO_RGB2			data_table[ D_IM_PRO_OB_BUFFER_MAX ];
} T_IM_PRO_OBT_BUF_INFO;

typedef struct {
	T_IM_PRO_INDEX			addr_index;
	volatile T_IM_PRO_RGB4*	data_addr;
	volatile T_IM_PRO_RGB4*	cnt_addr;
	T_IM_PRO_RGB4			data_table[ D_IM_PRO_OB_BUFFER_MAX ];
	T_IM_PRO_RGB4			cnt_table[ D_IM_PRO_OB_BUFFER_MAX ];
} T_IM_PRO_OBD_BUF_INFO;

typedef struct {
	T_IM_PRO_INDEX	addr_index;
	ULONG			addr_table[ D_IM_PRO_PWCH_BUFFER_MAX ];
	ULONG 			current_addr;
	ULONG			last_addr;
} T_IM_PRO_AFPB_BUF_INFO;

typedef struct {
	T_IM_PRO_INDEX	addr_index;
	ULONG			addr_table[ D_IM_PRO_PWCH_BUFFER_MAX ];
	ULONG 			current_addr;
	ULONG			last_addr;
} T_IM_PRO_STAT_PWCH_BUF_INFO;

typedef struct {
	T_IM_PRO_INDEX	data_index;
	UCHAR			current_data_index;
	UCHAR			last_data_index;
	ULONG			DataS_Tbl    [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataGE_Tbl   [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataL_Tbl    [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataH0_Tbl   [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataH1_Tbl   [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataH2_Tbl   [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataV_Tbl    [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataMAX_Tbl  [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataMIN_Tbl  [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataMAXE_Tbl [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataMINE_Tbl [ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataMAXH0_Tbl[ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataMAXH1_Tbl[ D_IM_PRO_AF_BUFFER_MAX ];
	ULONG			DataMAXH2_Tbl[ D_IM_PRO_AF_BUFFER_MAX ];
} T_IM_PRO_STAT_AF_BUF_INFO;

typedef struct {
	T_IM_PRO_INDEX	data_index;
	UCHAR			current_data_index;
	UCHAR			last_data_index;
	UCHAR			most_frequent_value_r[ D_IM_PRO_HIST_BUFFER_MAX ];
	UCHAR			most_frequent_value_g[ D_IM_PRO_HIST_BUFFER_MAX ];
	UCHAR			most_frequent_value_b[ D_IM_PRO_HIST_BUFFER_MAX ];
	UCHAR			most_frequent_value_y[ D_IM_PRO_HIST_BUFFER_MAX ];
	ULONG			sum_r[ D_IM_PRO_HIST_BUFFER_MAX ];
	ULONG			sum_g[ D_IM_PRO_HIST_BUFFER_MAX ];
	ULONG			sum_b[ D_IM_PRO_HIST_BUFFER_MAX ];
	ULONG			sum_y[ D_IM_PRO_HIST_BUFFER_MAX ];
	ULONG			data_r[ D_IM_PRO_HIST_BUFFER_MAX ][256];
	ULONG			data_g[ D_IM_PRO_HIST_BUFFER_MAX ][256];
	ULONG			data_b[ D_IM_PRO_HIST_BUFFER_MAX ][256];
	ULONG			data_y[ D_IM_PRO_HIST_BUFFER_MAX ][256];
} T_IM_PRO_STAT_HIST_BUF_INFO;

typedef struct {
	volatile struct io_b2btop*	reg_ptr;
} T_IM_PRO_B2BTOP_INFO;

typedef struct {
	volatile union io_b2b_fshdctbl*	tbl_reg_ptr;
	UINT32							status;
	UINT32							pre_started_status[D_IM_PRO_FSHD_PRE_STARTED_STATUS_MAX];
} T_IM_PRO_FSHD_INFO;

typedef struct {
	T_CALLBACK_LONG				p_callback;
	ULONG						user_param;
} T_IM_PRO_CALLBACK_INFO;


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
	extern "C" {
#endif
extern VOID im_pro_memcpy(VOID *dst, VOID *src, UINT32 cp_size);

extern VOID im_pro_sen_set_start_status(UINT32 status, UINT32 ch);
extern VOID im_pro_sen_set_stop_status(UINT32 status, UINT32 ch);
extern BOOL im_pro_sen_get_start_status(UINT32 status, UINT32 ch);
extern BOOL im_pro_sen_get_stop_all(VOID);

extern BOOL im_pro_comm_get_start_status( UINT32 status, UINT32 ch );

extern VOID im_pro_on_drive_clk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type );
extern VOID im_pro_on_pclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type );
extern VOID im_pro_on_hclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type );
extern VOID im_pro_on_xclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type );

extern VOID im_pro_off_drive_clk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type );
extern VOID im_pro_off_pclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type );
extern VOID im_pro_off_hclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type );
extern VOID im_pro_off_xclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type );
extern E_IM_PRO_CLK_BLOCK_TYPE im_pro_convert_clk_type( E_IM_PRO_BLOCK_TYPE block_type );

extern VOID im_pro_get_current_clk_hz( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type, UINT32* cpu_clk_hz, UINT32* macro_clk_hz );

extern	VOID im_pro_comm_get_pwch_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, const T_IM_PRO_COMMON_PWCH_INFO** pwch_info );
extern	VOID im_pro_comm_get_prch_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, const T_IM_PRO_COMMON_PRCH_INFO** prch_info );
extern	VOID im_pro_comm_get_p2m_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_P2M_CH ch, const T_IM_PRO_COMMON_P2M_INFO** p2m_info );
extern	VOID im_pro_comm_get_m2p_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_M2P_CH ch, const T_IM_PRO_COMMON_M2P_INFO** m2p_info );
extern	VOID im_pro_comm_get_lnr_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_LNR_CH ch, const T_IM_PRO_COMMON_LNR_INFO** lnr_info );
extern	VOID im_pro_comm_get_moni_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch, const T_IM_PRO_COMMON_MONI_INFO** moni_info );
extern	VOID im_pro_comm_get_pg_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, const T_IM_PRO_COMMON_PG_INFO** pg_info );
extern	VOID im_pro_comm_get_pgain_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PGAIN_SELECT ch, const T_IM_PRO_COMMON_PGAIN_INFO** pgain_info );
extern	VOID im_pro_comm_get_pzsft_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_PZSFT_CH ch, const T_IM_PRO_COMMON_PZSFT_INFO** pzsft_info );
extern	VOID im_pro_comm_get_fshdl_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, const T_IM_PRO_SRO_FSHDL_INFO** fshdl_info );
extern	VOID im_pro_comm_get_sdc_reg_info( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, const T_IM_PRO_SRO_SDC_INFO** sdc_info );
extern	VOID im_pro_comm_get_fshd_reg_info( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_FSHD_INFO** fshdl_info );

extern	VOID im_pro_comm_set_pwch_axi_response( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, UCHAR axi_resp );
extern	VOID im_pro_comm_set_prch_axi_response( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, UCHAR axi_resp );

extern	VOID im_pro_comm_update_obd_result( UINT32 blob_num, UINT32 ch );
extern	VOID im_pro_comm_update_obt_result( UINT32 blob_num, UINT32 ch );
extern	VOID im_pro_comm_update_stat_pwch_index( UINT32 ch );
extern	VOID im_pro_comm_update_afpb_pwch_index( E_IM_PRO_UNIT_NUM unit_no, UINT32 ch );
extern	VOID im_pro_comm_update_af_result( E_IM_PRO_AF_CH ch );
extern	VOID im_pro_comm_update_hist_result( E_IM_PRO_HIST_CH ch );
extern	VOID im_pro_comm_update_moni_result( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch );

extern	VOID im_pro_comm_get_obd_data( UINT32 blob_num, UINT32 ch, T_IM_PRO_RGB4* data );
extern	VOID im_pro_comm_get_obd_cnt( UINT32 blob_num, UINT32 ch, T_IM_PRO_RGB4* cnt );
extern	VOID im_pro_comm_get_obt_data( UINT32 blob_num, UINT32 ch, T_IM_PRO_RGB2* data );
extern	ULONG im_pro_comm_get_af_data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no, E_IM_PRO_AF_RESULT_TYPE type, E_IM_PRO_AF_RESULT_DATA_TYPE data_type );
extern	VOID im_pro_comm_get_hist_result( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_DATA* hist_data );
extern	VOID im_pro_comm_get_hist_ram( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_RAM_DATA* hist_ram_data );
extern	ULONG im_pro_comm_get_stat_cur_data_addr( UINT32 ch );
extern	ULONG im_pro_comm_get_stat_last_data_addr( UINT32 ch );
extern	VOID im_pro_comm_get_obd_block_ch( E_IM_PRO_OBD_CH ch, UCHAR* block_num, UCHAR* ch_num );
extern	VOID im_pro_comm_get_obt_block_ch( E_IM_PRO_OBT_CH ch, UCHAR* block_num, UCHAR* ch_num );
extern	VOID im_pro_comm_get_obd_ch( UCHAR block_num, UCHAR ch_num, E_IM_PRO_OBD_CH* ch );
extern	VOID im_pro_comm_get_obt_ch( UCHAR block_num, UCHAR ch_num, E_IM_PRO_OBT_CH* ch );

extern BOOL im_pro_check_val_range(ULONG max, ULONG min, ULONG val, char* func_name );

extern VOID im_pro_management_info_print( VOID );

#ifdef __cplusplus
	}
#endif

#endif	// _IM_PRO_COMMON_H_


