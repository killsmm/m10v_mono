/**
 * @file		tlv32aic3204.c
 * @brief		TLV320AIC3204 Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "dd_arm.h"
#include "tlv320aic3204.h"
#include "dd_top.h"
#include "os_user_custom.h"
#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#ifndef TRUE
#define TRUE	(1)
#endif
#ifndef FALSE
#define FALSE	(0)
#endif


/**** Register ****/

/* Page select register */
#define D_AIC3204_REG_PAGE_SELECT		(0x00)

/****************************/
/*	Page 0					*/
/****************************/

/* Perform software reset */
#define D_AIC3204_REG_RESET_SOFT		(0x01)

/* Clock Setting Register 1, Multiplexers */
#define D_AIC3204_REG_CLK1					(0x04)

#define D_AIC3204_REG_CLK1_PLLRANGE_LOW		(0 << 6)	/* PLL Range Select: Low Range */
#define D_AIC3204_REG_CLK1_PLLRANGE_HIGH	(1 << 6)	/* PLL Range Select: High Range */
#define D_AIC3204_REG_CLK1_PLLRANGE			(1 << 6)	/* PLL Range Select Mask */

#define D_AIC3204_REG_CLK1_PLLSRC_MCLK		(0 << 2)	/* PLL Input Clock Select: MCLK */
#define D_AIC3204_REG_CLK1_PLLSRC_BCLK		(1 << 2)	/* PLL Input Clock Select: BCLK */
#define D_AIC3204_REG_CLK1_PLLSRC_GPIO		(2 << 2)	/* PLL Input Clock Select: GPIO */
#define D_AIC3204_REG_CLK1_PLLSRC_DIN		(3 << 2)	/* PLL Input Clock Select: Data In */
#define D_AIC3204_REG_CLK1_PLLSRC			(3 << 2)	/* PLL Input Clock Select Mask */

#define D_AIC3204_REG_CLK1_CODECCLK_MCLK	(0)			/* CODEC Clock Source Select: MCLK */
#define D_AIC3204_REG_CLK1_CODECCLK_BCLK	(1)			/* CODEC Clock Source Select: BCLK */
#define D_AIC3204_REG_CLK1_CODECCLK_GPIO	(2)			/* CODEC Clock Source Select: GPIO */
#define D_AIC3204_REG_CLK1_CODECCLK_PLL		(3)			/* CODEC Clock Source Select: PLL */
#define D_AIC3204_REG_CLK1_CODECCLK			(3)			/* CODEC Clock Source Select Mask */


/* Clock Setting Register 2, PLL P & R Values */
#define D_AIC3204_REG_CLK2					(0x05)

#define D_AIC3204_REG_CLK2_PLL_OFF			(0 << 7)	/* PLL State: Powered Down */
#define D_AIC3204_REG_CLK2_PLL_ON			(1 << 7)	/* PLL State: Powered Up */
#define D_AIC3204_REG_CLK2_PLL				(1 << 7)	/* PLL State Mask */

#define D_AIC3204_REG_CLK2_PLL_SHIFT		(7)			/* PLL State bitfield location */
#define D_AIC3204_REG_CLK2_PLL_P_SHIFT		(4)			/* PLL P Divider value bitfield location */
#define D_AIC3204_REG_CLK2_PLL_P			(7 << 4)	/* PLL P Divider value bitfield mask */

#define D_AIC3204_REG_CLK2_PLL_R_SHIFT		(0)			/* PLL R Divider value bitfield location */

#define D_AIC3204_REG_CLK2_PLL_R			(0xf)		/* PLL R Divider value bitfield mask */


/* Clock Setting Register 3, PLL J Values */
#define D_AIC3204_REG_CLK3					(0x06)

#define D_AIC3204_REG_CLK3_PLL_J_SHIFT		(0)			/* PLL J Divider value bitfield location */
#define D_AIC3204_REG_CLK3_PLL_J			(0x3f)		/* PLL J Divider value bitfield mask */


/* Clock Setting Register 4, PLL D MSB */
#define D_AIC3204_REG_CLK4					(0x07)

#define D_AIC3204_REG_CLK4_PLL_D_SHIFT		(0)			/* PLL D most significant portion bitfield location */
#define D_AIC3204_REG_CLK4_PLL_D			(0x3f)		/* PLL D most significant portion bitfield mask */
#define D_AIC3204_REG_CLK4_PLL_D_VSHIFT		(8)			/* PLL D most significant portion value shift */


/* Clock Setting Register 5, PLL D LSB */
#define D_AIC3204_REG_CLK5					(0x8)

#define D_AIC3204_REG_CLK5_PLL_D_SHIFT		(0)			/* PLL D least significant portion bitfield location */
#define D_AIC3204_REG_CLK5_PLL_D			(0xff)		/* PLL D least significant portion bitfield mask */
#define D_AIC3204_REG_CLK5_PLL_D_VSHIFT		(0)			/* PLL D least significant portion value shift */


/* Clock Setting Register 6, NDAC Values */
#define D_AIC3204_REG_CLK6					(0x0B)

#define D_AIC3204_REG_CLK6_NDAC_STATE_OFF	(0 << 7)	/* NDAC State: Powered Down */
#define D_AIC3204_REG_CLK6_NDAC_STATE_ON	(1 << 7)	/* NDAC State: Powered Up */
#define D_AIC3204_REG_CLK6_NDAC_STATE		(1 << 7)	/* NDAC State Mask */
#define D_AIC3204_REG_CLK6_NDAC_STATE_SHIFT	(7)			/* NDAC State bitfield location */

#define D_AIC3204_REG_CLK6_NDAC_SHIFT		(0)			/* NDAC value bitfield location */
#define D_AIC3204_REG_CLK6_NDAC				(0x7f)				/* NDAC value bitfield mask */


/* Clock Setting Register 7, MDAC Values */
#define D_AIC3204_REG_CLK7					(0x0C)

#define D_AIC3204_REG_CLK7_MDAC_STATE_OFF	(0 << 7)	/* MDAC State: Powered Down */
#define D_AIC3204_REG_CLK7_MDAC_STATE_ON	(1 << 7)	/* MDAC State: Powered Up */
#define D_AIC3204_REG_CLK7_MDAC_STATE		(1 << 7)	/* MDAC State Mask */
#define D_AIC3204_REG_CLK7_MDAC_STATE_SHIFT	(7)			/* MDAC State bitfield location */

#define D_AIC3204_REG_CLK7_MDAC_SHIFT		(0)			/* MDAC value bitfield location */
#define D_AIC3204_REG_CLK7_MDAC				(0x7f)		/* MDAC value bitfield mask */


/* DAC OSR Setting Register 1, MSB */
#define D_AIC3204_REG_DOSR1					(0x0D)

#define D_AIC3204_REG_DOSR1_MSB_SHIFT		(0)			/* DOSR most significant portion bitfield location */
#define D_AIC3204_REG_DOSR1_MSB				(3)			/* DOSR most significant portion bitfield mask */

#define D_AIC3204_REG_DOSR1_MSB_VSHIFT		(8)			/* DOSR most significant portion value shift */


/* DAC OSR Setting Register 2, LSB */
#define D_AIC3204_REG_DOSR2					(0x0E)

#define D_AIC3204_REG_DOSR2_LSB_SHIFT		(0)			/* DOSR least significant portion bitfield location */
#define D_AIC3204_REG_DOSR2_LSB				(0xff)		/* DOSR least significant portion bitfield mask */

#define D_AIC3204_REG_DOSR2_LSB_VSHIFT		(0)			/* DOSR least significant portion value shift */


/* Clock Setting Register 8, NADC Values */
#define D_AIC3204_REG_CLK8					(0x12)

#define D_AIC3204_REG_CLK8_NADC_STATE_OFF	(0 << 7)	/* NADC State: Powered Down */
#define D_AIC3204_REG_CLK8_NADC_STATE_ON	(1 << 7)	/* NADC State: Powered Up */
#define D_AIC3204_REG_CLK8_NADC_STATE		(1 << 7)	/* NADC State Mask */

#define D_AIC3204_REG_CLK8_NADC_STATE_SHIFT	(7)			/* NADC State bitfield location */
#define D_AIC3204_REG_CLK8_NADC_SHIFT		(0)			/* NADC value bitfield location */
#define D_AIC3204_REG_CLK8_NADC				(0x7f)		/* NADC value bitfield mask */


/* Clock Setting Register 9, MADC Values */
#define D_AIC3204_REG_CLK9					(0x13)

#define D_AIC3204_REG_CLK9_MADC_STATE_OFF	(0 << 7)	/* MADC State: Powered Down */
#define D_AIC3204_REG_CLK9_MADC_STATE_ON	(1 << 7)	/* MADC State: Powered Up */
#define D_AIC3204_REG_CLK9_MADC_STATE		(1 << 7)	/* MADC State Mask */

#define D_AIC3204_REG_CLK9_MADC_STATE_SHIFT	(7)			/* MADC State bitfield location */
#define D_AIC3204_REG_CLK9_MADC_SHIFT		(0)			/* MADC value bitfield location */
#define D_AIC3204_REG_CLK9_MADC				(0x7f)		/* MADC value bitfield mask */


/* ADC OSR Setting Register */
#define D_AIC3204_REG_AOSR					(0x14)


/* 
 * Clock Register setting 10, Multiplexers
 *
 * XXX: Note that the datasheet goofs up here, calling this register 9, but we
 * already had a register 9 just before AOSR.  Therefore, we will call this
 * register 10, and will increment all following by one to correct TI's
 * mistake.
 */
#define D_AIC3204_REG_CLK10					(0x19)

#define D_AIC3204_REG_CLK10_CDIV_MCLK		(0)			/* CDIV Clock Source: MCLK */
#define D_AIC3204_REG_CLK10_CDIV_BCLK		(1)			/* CDIV Clock Source: BCLK */
#define D_AIC3204_REG_CLK10_CDIV_DIN		(2)			/* CDIV Clock Source: Data IN */
#define D_AIC3204_REG_CLK10_CDIV_PLL		(3)			/* CDIV Clock Source: PLL */
#define D_AIC3204_REG_CLK10_CDIV_DAC		(4)			/* CDIV Clock Source: DAC */
#define D_AIC3204_REG_CLK10_CDIV_DAC_MOD	(5)			/* CDIV Clock Source: DAC Modulation */
#define D_AIC3204_REG_CLK10_CDIV_ADC		(6)			/* CDIV Clock Source: ADC */
#define D_AIC3204_REG_CLK10_CDIV_ADC_MOD	(7)			/* CDIV Clock Source: ADC Modulation */
#define D_AIC3204_REG_CLK10_CDIV			(7)			/* CDIV Clock Source Mask */


/* Clock Register setting 11, CLKOUT M divider value */
#define D_AIC3204_REG_CLK11					(0x1A)

#define D_AIC3204_REG_CLK11_CLKOUT_OFF		(0 << 7)	/* CLKOUT Divider State: Off */
#define D_AIC3204_REG_CLK11_CLKOUT_ON		(1 << 7)	/* CLKOUT Divider State: On */
#define D_AIC3204_REG_CLK11_CLKOUT			(1 << 7)	/* CLKOUT Divider State Mask */

#define D_AIC3204_REG_CLK11_CLKOUTM_SHIFT	(0)			/* CLKOUT Divider M value bitfield location */
#define D_AIC3204_REG_CLK11_CLKOUTM			(0x7f)		/* CLKOUT Divider M value bitfield mask */


/* Audio Interface Setting Register 1 */
#define D_AIC3204_REG_AISR1					(0x1B)

#define D_AIC3204_REG_AISR1_INT_I2S			(0 << 6)	/* Audio Interface Select: I2S */
#define D_AIC3204_REG_AISR1_INT_DSP			(1 << 6)	/* Audio Interface Select: DSP */
#define D_AIC3204_REG_AISR1_INT_RJF			(2 << 6)	/* Audio Interface Select: Right-Justified Format */
#define D_AIC3204_REG_AISR1_INT_LJF			(3 << 6)	/* Audio Interface Select: Left-Justified Format */
#define D_AIC3204_REG_AISR1_INT				(3 << 6)	/* Audio Interface Select Mask */

#define D_AIC3204_REG_AISR1_WL_16BITS		(0 << 4)	/* Audio Data Word Length: 16 bits */
#define D_AIC3204_REG_AISR1_WL_20BITS		(1 << 4)	/* Audio Data Word Length: 20 bits */
#define D_AIC3204_REG_AISR1_WL_24BITS		(2 << 4)	/* Audio Data Word Length: 24 bits */
#define D_AIC3204_REG_AISR1_WL_32BITS		(3 << 4)	/* Audio Data Word Length: 32 bits */
#define D_AIC3204_REG_AISR1_WL				(3 << 4)	/* Audio Data Word Length Mask */

#define D_AIC3204_REG_AISR1_BCLK_IN			(0 << 3)	/* BCLK Direction Control: Input */
#define D_AIC3204_REG_AISR1_BCLK_OUT		(1 << 3)	/* BCLK Direction Control: Output */
#define D_AIC3204_REG_AISR1_BCLK			(1 << 3)	/* BCLK Direction Control: Mask */

#define D_AIC3204_REG_AISR1_WCLK_IN			(0 << 2)	/* WCLK Direction Control: Input */
#define D_AIC3204_REG_AISR1_WCLK_OUT		(1 << 2)	/* WCLK Direction Control: Output */
#define D_AIC3204_REG_AISR1_WCLK			(1 << 2)	/* WCLK Direction Control: Mask */

#define D_AIC3204_REG_AISR1_HIZ_NEVER		(0 << 0)	/* DOUT High Impedance Output Control: Never high impedance */
#define D_AIC3204_REG_AISR1_HIZ_IDLE		(1 << 0)	/* DOUT High Impedance Output Control: High impedance when idle */
#define D_AIC3204_REG_AISR1_HIZ				(1 << 0)	/* DOUT High Impedance Output Control Mask */

/* Audio Interface Register 2, Data offset setting */
#define D_AIC3204_REG_AISR2					(0x1C)


/* Audio Interface Register 3 */
#define D_AIC3204_REG_AISR3					(0x1D)

#define D_AIC3204_REG_AISR3_ADLO_OFF		(0 << 5)	/* Audio Data Loopback Control: Disabled */
#define D_AIC3204_REG_AISR3_ADLO_ON			(1 << 5)	/* Audio Data Loopback Control: Enabled */
#define D_AIC3204_REG_AISR3_ADLO			(1 << 5)	/* Audio Data Loopback Control Mask */

#define D_AIC3204_REG_AISR3_ADDALO_OFF		(0 << 4)	/* ADC->DAC Loopback Control: Disabled */
#define D_AIC3204_REG_AISR3_ADDALO_ON		(1 << 4)	/* ADC->DAC Loopback Control: Enabled */
#define D_AIC3204_REG_AISR3_ADDALO			(1 << 4)	/* ADC->DAC Loopback Control Mask */

#define D_AIC3204_REG_AISR3_BCLKPOL_NOR		(0 << 3)	/* Audio Bit Clock Polarity: Normal */
#define D_AIC3204_REG_AISR3_BCLKPOL_INV		(1 << 3)	/* Audio Bit Clock Polarity: Inverted */
#define D_AIC3204_REG_AISR3_BCLKPOL			(1 << 3)	/* Audio Bit Clock Polarity Mask */

#define D_AIC3204_REG_AISR3_ADICLK_ALWAYS	(0 << 2)	/* Audio Data Interface Clock Buffers: Always powered */
#define D_AIC3204_REG_AISR3_ADICLK_CODEC	(1 << 2)	/* Audio Data Interface Clock Buffers: Powered with CODEC only */
#define D_AIC3204_REG_AISR3_ADICLK			(1 << 2)	/* Audio Data Interface Clock Buffers' State */

#define D_AIC3204_REG_AISR3_BDIV_DAC		(0 << 0)	/* Audio Bit Clock Divider Source: DAC */
#define D_AIC3204_REG_AISR3_BDIV_DAC_MOD	(1 << 0)	/* Audio Bit Clock Divider Source: DAC Modulation */
#define D_AIC3204_REG_AISR3_BDIV_ADC		(2 << 0)	/* Audio Bit Clock Divider Source: ADC */
#define D_AIC3204_REG_AISR3_BDIV_ADC_MOD	(3 << 0)	/* Audio Bit Clock Divider Source: ADC Modulation */
#define D_AIC3204_REG_AISR3_BDIV			(3 << 0)	/* Audio Bit Clock Divider Source Mask */



/* Clock Setting Register 12, BCLK N Divider */
#define D_AIC3204_REG_CLK12					(0x1E)

#define D_AIC3204_REG_PG0_CLK12_BCLK_STATE_OFF	(0 << 7)	/* BCLK N State: Powered Down */
#define D_AIC3204_REG_CLK12_BCLK_STATE_ON		(1 << 7)	/* BCLK N State: Powered Up */
#define D_AIC3204_REG_CLK12_BCLK_STATE			(1 << 7)	/* BCLK N State Mask */

#define D_AIC3204_REG_CLK12_BCLK_STATE_SHIFT	(7)			/* BCLK N State bitfield location */
#define D_AIC3204_REG_CLK12_BCLK_SHIFT			(0)			/* BCLK N value bitfield location */
#define D_AIC3204_REG_CLK12_BCLK				(0x7f)		/* BCLK N value bitfield mask */


/* Audio Interface Setting Register 4, Secondary Audio Interface */
#define D_AIC3204_REG_AISR4					(0x1F)

#define D_AIC3204_REG_AISR4_SECBCLK_GPIO	(0 << 5)	/* Secondary Bit Clock: GPIO */
#define D_AIC3204_REG_AISR4_SECBCLK_SCLK	(1 << 5)	/* Secondary Bit Clock: SCLK */
#define D_AIC3204_REG_AISR4_SECBCLK_MISO	(2 << 5)	/* Secondary Bit Clock: MISO */
#define D_AIC3204_REG_AISR4_SECBCLK_DOUT	(3 << 5)	/* Secondary Bit Clock: DOUT */
#define D_AIC3204_REG_AISR4_SECBCLK			(3 << 5)	/* Secondary Bit Clock Mask */

#define D_AIC3204_REG_AISR4_SECWCLK_GPIO	(0 << 3)	/* Secondary Word Clock: GPIO */
#define D_AIC3204_REG_AISR4_SECWCLK_SCLK	(1 << 3)	/* Secondary Word Clock: SCLK */
#define D_AIC3204_REG_AISR4_SECWCLK_MISO	(2 << 3)	/* Secondary Word Clock: MISO */
#define D_AIC3204_REG_AISR4_SECWCLK_DOUT	(3 << 3)	/* Secondary Word Clock: DOUT */
#define D_AIC3204_REG_AISR4_SECWCLK			(3 << 3)	/* Secondary Word Clock Mask */
#define D_AIC3204_REG_AISR4_ADCWCLK_GPIO	(0 << 3)	/* ADC Word Clock: GPIO */
#define D_AIC3204_REG_AISR4_ADCWCLK_SCLK	(1 << 3)	/* ADC Word Clock: SCLK */
#define D_AIC3204_REG_AISR4_ADCWCLK_MISO	(2 << 3)	/* ADC Word Clock: MISO */
#define D_AIC3204_REG_AISR4_ADCWCLK			(3 << 3)	/* ADC Word Clock Mask */

#define D_AIC3204_REG_AISR4_SECDIN_GPIO		(0 << 0)	/* Secondary Data Input: GPIO */
#define D_AIC3204_REG_AISR4_SECDIN_SCLK		(1 << 0)	/* Secondary Data Input: SCLK */
#define D_AIC3204_REG_AISR4_SECDIN			(1 << 0)	/* Secondary Data Input Mask */


/* Audio Interface Setting Register 5 */
#define D_AIC3204_REG_AISR5					(0x20)

#define D_AIC3204_REG_AISR5_BCLKIN_PRI		(0 << 3)	/* Audio Interface Bit Clock: Primary (BCLK) */
#define D_AIC3204_REG_AISR5_BCLKIN_SEC		(1 << 3)	/* Audio Interface Bit Clock: Secondary */

#define D_AIC3204_REG_AISR5_WCLKIN			(1 << 2)	/* Audio Interface Bit Clock Mask */
#define D_AIC3204_REG_AISR5_WCLKIN_PRI		(0 << 2)	/* Audio Interface Word Clock: Primary (BCLK) */
#define D_AIC3204_REG_AISR5_WCLKIN_SEC		(1 << 2)	/* Audio Interface Word Clock: Secondary */
#define D_AIC3204_REG_AISR5_WCLKIN			(1 << 2)	/* Audio Interface Word Clock Mask */

#define ADC3204_PG0_AISR5_ADCWCLK_DAC		(0 << 1)	/* ADC Word Clock Control: ADC WCLK = DAC WCLK */
#define ADC3204_PG0_AISR5_ADCWCLK_SEC		(1 << 1)	/* ADC Word Clock Control: ADC WCLK = Secondary ADC WCLK */
#define ADC3204_PG0_AISR5_ADCWCLK			(1 << 1)	/* ADC Word Clock Control Mask */

#define ADC3204_PG0_AISR5_DIN_PRI			(0 << 0)	/* Audio Data In: Primary Data In */
#define ADC3204_PG0_AISR5_DIN_SEC			(1 << 0)	/* Audio Data In: Secondary Data In */
#define ADC3204_PG0_AISR5_DIN				(1 << 0)	/* Audio Data In Mask */


/* Audio Interface Setting Register 6 */
#define D_AIC3204_REG_AISR6					(0x21)

#define D_AIC3204_REG_AISR6_BCLKOUT_GEN		(0 << 7)	/* BCLK Output Control: Generated Primary Bit Clock */
#define D_AIC3204_REG_AISR6_BCLKOUT_SEC		(1 << 7)	/* BCLK Output Control: Secondary Bit Clock */
#define D_AIC3204_REG_AISR6_BCLKOUT			(1 << 7)	/* BCLK Output Control Mask */

#define D_AIC3204_REG_AISR6_SBCLKOUT_BCLK	(0 << 6)	/* Secondary BCLK Output Control: Primary Bit Clock Input */
#define D_AIC3204_REG_AISR6_SBCLKOUT_GEN	(1 << 6)	/* Secondary BCLK Output Control: Generated Primary Bit Clock */
#define D_AIC3204_REG_AISR6_SBCLKOUT		(1 << 6)	/* Secondary BCLK Output Control Mask */

#define D_AIC3204_REG_AISR6_WCLKOUT_DAC		(0 << 4)	/* WCLK Output Control: Generated DAC_FS */
#define D_AIC3204_REG_AISR6_WCLKOUT_ADC		(1 << 4)	/* WCLK Output Control: Generated ADC_FS */
#define D_AIC3204_REG_AISR6_WCLKOUT_SWCLK	(2 << 4)	/* WCLK Output Control: Secondary WCLK Input */
#define D_AIC3204_REG_AISR6_WCLKOUT			(3 << 4)	/* WCLK Output Control Mask */

#define D_AIC3204_REG_AISR6_SWCLKOUT_WCLK	(0 << 2)	/* Secondary WCLK Output Control: WCLK Input */
#define D_AIC3204_REG_AISR6_SWCLKOUT_DAC	(1 << 2)	/* Secondary WCLK Output Control: Generated DAC_FS */
#define D_AIC3204_REG_AISR6_SWCLKOUT_ADC	(2 << 2)	/* Secondary WCLK Output Control: Generated ADC_FS */
#define D_AIC3204_REG_AISR6_SWCLKOUT		(3 << 2)	/* Secondary WCLK Output Control Mask */

#define D_AIC3204_REG_AISR6_DOUT_INT		(0 << 1)	/* Primary Data Output Control: Serial Interface */
#define D_AIC3204_REG_AISR6_DOUT_SDIN		(1 << 1)	/* Primary Data Output Control: Secondary Data Input */
#define D_AIC3204_REG_AISR6_DOUT			(1 << 1)	/* Primary Data Output Control Mask */

#define D_AIC3204_REG_AISR6_SDOUT_DIN		(0 << 0)	/* Secondary Data Output Control: Primary Data In */
#define D_AIC3204_REG_AISR6_SDOUT_INT		(1 << 0)	/* Secondary Data Output Control: Serial Interface */
#define D_AIC3204_REG_AISR6_SDOUT			(1 << 0)	/* Secondary Data Output Control Mask */

/* Digital Interface Misc. Setting Register */
#define D_AIC3204_REG_DIMISC				(0x22)

#define D_AIC3204_REG_DIMISC_I2CGC_IGNORE	(0 << 5)	/* I2C General Call Address Configuration: Ignore */
#define D_AIC3204_REG_DIMISC_I2CGC_ACCEPT	(1 << 5)	/* I2C General Call Address Configuration: Accept */

/* ADC Flag Register */
#define D_AIC3204_REG_ADCF					(0x24)

#define D_AIC3204_REG_ADCF_LEFT_PGASET		(1 << 7)	/* Left ADC PGA Status: Gain is set */

#define D_AIC3204_REG_ADCF_LEFT_UP			(1 << 6)	/* Left ADC Power Status: Powered Up */

#define D_AIC3204_REG_ADCF_LEFT_AGCSAT		(1 << 5)	/* Left ADC AGC Status: Gain is saturated */

#define D_AIC3204_REG_ADCF_RIGHT_PGASET		(1 << 3)	/* Right ADC PGA Status: Gain is set */

#define D_AIC3204_REG_ADCF_RIGHT_UP			(1 << 2)	/* Right ADC Power Status: Powered Up */

#define D_AIC3204_REG_ADCF_RIGHT_AGCSAT		(1 << 1)	/* Right ADC AGC Status: Gain is saturated */


/* DAC Flag Register 1 */
#define D_AIC3204_REG_DACF1					(0x25)

#define D_AIC3204_REG_DACF1_LEFT_UP			(1 << 7)	/* Left DAC powered up */

#define D_AIC3204_REG_DACF1_LOL_UP			(1 << 6)	/* Left Line Output Driver powered up */

#define D_AIC3204_REG_DACF1_HPL_UP			(1 << 5)	/* Left Headphone Output Driver powered up */

#define D_AIC3204_REG_DACF1_RIGHT_UP		(1 << 3)	/* Right DAC powered up */

#define D_AIC3204_REG_DACF1_LOR_UP			(1 << 2)	/* Right Line Output Driver powered up */

#define D_AIC3204_REG_DACF1_HPR_UP			(1 << 1)	/* Right Headphone Output Driver powered up */


/* DAC Flag Register 2 */
#define D_AIC3204_REG_DACF2					(0x26)

#define D_AIC3204_REG_DACF2_LEFT_PGASET		(1 << 4)	/* Left DAC PGA Status: Gain is set */

#define D_AIC3204_REG_DACF2_RIGHT_PGASET	(1 << 0)	/* Right DAC PGA Status: Gain is set */


/* Sticky Flag Register 1 */
#define D_AIC3204_REG_STICK1				(0x2A)

#define D_AIC3204_REG_STICK1_LDAC_OVER		(1 << 7)	/* Left DAC Overflow */

#define D_AIC3204_REG_STICK1_RDAC_OVER		(1 << 6)	/* Right DAC Overflow */

#define D_AIC3204_REG_STICK1_LADC_OVER		(1 << 3)	/* Left ADC Overflow */

#define D_AIC3204_REG_STICK1_RADC_OVER		(1 << 2)	/* Right ADC Overflow */


/* Interrupt Flag Register 1 */
#define D_AIC3204_REG_INTF1					(0x2B)

#define D_AIC3204_REG_INTF1_LDAC_OVER		(1 << 7)	/* Left DAC Overflow */

#define D_AIC3204_REG_INTF1_RDAC_OVER		(1 << 6)	/* Right DAC Overflow */

#define D_AIC3204_REG_INTF1_LADC_OVER		(1 << 3)	/* Left ADC Overflow */

#define D_AIC3204_REG_INTF1_RADC_OVER		(1 << 2)	/* Right ADC Overflow */


/* Sticky Flag Register 2 */
#define D_AIC3204_REG_STICK2				(0x2C)

#define D_AIC3204_REG_STICK2_HPL_OVER		(1 << 7)	/* Left Headphone Driver Over Current */

#define D_AIC3204_REG_STICK2_HPR_OVER		(1 << 6)	/* Right Headphone Driver Over Current */

#define D_AIC3204_REG_STICK2_HS_BUTTON		(1 << 5)	/* Headset button pressed */

#define D_AIC3204_REG_STICK2_HS_PLUGGED		(1 << 4)	/* Headset plug inserted/removed */

#define D_AIC3204_REG_STICK2_LDRC_OVER		(1 << 3)	/* Left Channel DRC: Over threshold */

#define D_AIC3204_REG_STICK2_RDRC_OVER		(1 << 2)	/* Right Channel DRC: Over threshold */


/* Sticky Flag Register 3 */
#define D_AIC3204_REG_STICK3				(0x2D)

#define D_AIC3204_REG_STICK3_LAGC_OVER		(1 << 6)	/* Left AGC Noise Threshold Flag: Over threshold */

#define D_AIC3204_REG_STICK3_RAGC_OVER		(1 << 5)	/* Right AGC Noise Threshold Flag: Over threshold */

#define D_AIC3204_REG_STICK3_LADC_DC		(1 << 2)	/* Left ADC DC Measurement Available */

#define D_AIC3204_REG_STICK3_RADC_DC		(1 << 1)	/* Right ADC DC Measurement Available */


/* Interrupt Flag Register 2 */
#define D_AIC3204_REG_INTF2					(0x2E)

#define D_AIC3204_REG_INTF2_HPL_OVER		(1 << 7)	/* Left Headphone Driver Over Current */

#define D_AIC3204_REG_INTF2_HPR_OVER		(1 << 6)	/* Right Headphone Driver Over Current */

#define D_AIC3204_REG_INTF2_HS_BUTTON		(1 << 5)	/* Headset button pressed */

#define D_AIC3204_REG_INTF2_HS_PLUGGED		(1 << 4)	/* Headset plug inserted/removed */

#define D_AIC3204_REG_INTF2_LDRC_OVER		(1 << 3)	/* Left Channel DRC: Over threshold */

#define D_AIC3204_REG_INTF2_RDRC_OVER		(1 << 2)	/* Right Channel DRC: Over threshold */


/* Interrupt Flag Register 3 */
#define D_AIC3204_REG_INTF3					(0x2F)

#define D_AIC3204_REG_INTF3_LAGC_OVER		(1 << 6)	/* Left AGC Noise Threshold Flag: Over threshold */

#define D_AIC3204_REG_INTF3_RAGC_OVER		(1 << 5)	/* Right AGC Noise Threshold Flag: Over threshold */

#define D_AIC3204_REG_INTF3_LADC_DC			(1 << 2)	/* Left ADC DC Measurement Available */

#define D_AIC3204_REG_INTF3_RADC_DC			(1 << 1)	/* Right ADC DC Measurement Available */


/* INT1 Interrupt Control Register */
#define D_AIC3204_REG_INT1					(0x30)

#define D_AIC3204_REG_INT1_HS_PLUG			(1 << 7)	/* INT1 Generated on Headset insertion */

#define D_AIC3204_REG_INT1_HS_BUTTON		(1 << 6)	/* INT1 Generated on Headset Button press */

#define D_AIC3204_REG_INT1_DAC_DRC			(1 << 5)	/* INT1 Generated on DAC DRC Signal Threshold */

#define D_AIC3204_REG_INT1_ADC_NOISE		(1 << 4)	/* INT1 Generated on AGC Noise Interrupt */

#define D_AIC3204_REG_INT1_HP_OVERCURRENT	(1 << 3)	/* INT1 Generated on Over Current */

#define D_AIC3204_REG_INT1_OVERFLOW			(1 << 2)	/* INT1 Generated on overflow event */

#define D_AIC3204_REG_INT1_DC				(1 << 1)	/* INT1 Generated on DC measurement */

#define D_AIC3204_REG_INT1_CONT_PULSE		(1 << 0)	/* INT1 pulse control: Continuous pulse train */


/* INT2 Interrupt Control Register */
#define D_AIC3204_REG_INT2					(0x31)

#define D_AIC3204_REG_INT2_HS_PLUG			(1 << 7)	/* INT2 Generated on Headset insertion */

#define D_AIC3204_REG_INT2_HS_BUTTON		(1 << 6)	/* INT2 Generated on Headset Button press */

#define D_AIC3204_REG_INT2_DAC_DRC			(1 << 5)	/* INT2 Generated on DAC DRC Signal Threshold */

#define D_AIC3204_REG_INT2_ADC_NOISE		(1 << 4)	/* INT2 Generated on AGC Noise Interrupt */

#define D_AIC3204_REG_INT2_HP_OVERCURRENT	(1 << 3)	/* INT2 Generated on Over Current */

#define D_AIC3204_REG_INT2_OVERFLOW			(1 << 2)	/* INT2 Generated on overflow event */

#define D_AIC3204_REG_INT2_DC				(1 << 1)	/* INT2 Generated on DC measurement */

#define D_AIC3204_REG_INT2_CONT_PULSE		(1 << 0)	/* INT2 pulse control: Continuous pulse train */


/* GPIO/MFP5 Control Register */
#define D_AIC3204_REG_MFP5					(0x34)

#define D_AIC3204_REG_MFP5_FUNC_DISABLED	(0 << 2)	/* GPIO Control: Disabled */
#define D_AIC3204_REG_MFP5_FUNC_SAI_DM_CI	(1 << 2)	/* GPIO Control: Secondary audio interface/digital microphone/clock input */
#define D_AIC3204_REG_MFP5_FUNC_INPUT		(2 << 2)	/* GPIO Control: General Purpose Input */
#define D_AIC3204_REG_MFP5_FUNC_OUTPUT		(3 << 2)	/* GPIO Control: General Purpose Output */
#define D_AIC3204_REG_MFP5_FUNC_CLKOUT		(4 << 2)	/* GPIO Control: CLKOUT Output */
#define D_AIC3204_REG_MFP5_FUNC_INT1		(4 << 2)	/* GPIO Control: INT1 Output */
#define D_AIC3204_REG_MFP5_FUNC_INT2		(4 << 2)	/* GPIO Control: INT2 Output */
#define D_AIC3204_REG_MFP5_FUNC_ADC_WCLK	(4 << 2)	/* GPIO Control: ADC_WCLK */
#define D_AIC3204_REG_MFP5_FUNC_SEC_BCLK	(4 << 2)	/* GPIO Control: Secondary Bit Clock */
#define D_AIC3204_REG_MFP5_FUNC_SEC_WCLK	(4 << 2)	/* GPIO Control: Secondary Word Clock */
#define D_AIC3204_REG_MFP5_FUNC_DMIC_CLK	(4 << 2)	/* GPIO Control: Digital Microphone Clock */
#define D_AIC3204_REG_MFP5_FUNC				(15 << 2)	/* GPIO Control Mask */

#define D_AIC3204_REG_MFP5_IN				(1 << 1)	/* GPIO Input State */
#define D_AIC3204_REG_MFP5_OUT				(1 << 0)	/* GPIO Output State */


/* DOUT/MFP2 Function Control Register */
#define D_AIC3204_REG_MFP2					(0x35)

#define D_AIC3204_REG_MFP2_BK				(1 << 4)	/* DOUT Bus Keeper Enabled */

#define D_AIC3204_REG_MFP2_FUNC_DISABLED	(0 << 1)	/* DOUT MUX Control: Disabled */
#define D_AIC3204_REG_MFP2_FUNC_PRI_DOUT	(1 << 1)	/* DOUT MUX Control: Primary DOUT */
#define D_AIC3204_REG_MFP2_FUNC_OUTPUT		(2 << 1)	/* DOUT MUX Control: General Purpose Output */
#define D_AIC3204_REG_MFP2_FUNC_CLKOUT		(3 << 1)	/* DOUT MUX Control: CLKOUT Clock Output */
#define D_AIC3204_REG_MFP2_FUNC_INT1		(4 << 1)	/* DOUT MUX Control: INT1 Output */
#define D_AIC3204_REG_MFP2_FUNC_INT2		(5 << 1)	/* DOUT MUX Control: INT2 Output */
#define D_AIC3204_REG_MFP2_FUNC_SEC_BCLK	(6 << 1)	/* DOUT MUX Control: Secondary Bit Clock */
#define D_AIC3204_REG_MFP2_FUNC_SEC_WCLK	(7 << 1)	/* DOUT MUX Control: Secondary Word Clock */
#define D_AIC3204_REG_MFP2_FUNC				(7 << 1)	/* DOUT MUX Control Mask */

#define D_AIC3204_REG_MFP2_OUT				(1 << 0)	/* DOUT General Purpose Output State */


/* DIN/MFP1 Function Control Register */
#define D_AIC3204_REG_MFP1					(0x36)

#define D_AIC3204_REG_MFP1_FUNC_DISABLED	(0 << 1)	/* DIN Function Control: Disabled */

#define D_AIC3204_REG_MFP1_FUNC_DIN_DM_CI	(1 << 1)	/* DIN Function Control: Primary Data Input/Digital Microphone/Clock Input */

/* DIN Function Control: General Purpose Input */
#define D_AIC3204_REG_MFP1_FUNC_INPUT		(2 << 1)
/* DIN Function Control Mask */
#define D_AIC3204_REG_MFP1_FUNC				(3 << 1)
/* DIN Input State */
#define D_AIC3204_REG_MFP1_IN				(1 << 0)


/* MISO/MFP4 Function Control Register */
#define D_AIC3204_REG_MFP4					(0x37)

#define D_AIC3204_REG_MFP4_FUNC_DISABLED	(0 << 1)	/* MISO Function Control: Disabled */
#define D_AIC3204_REG_MFP4_FUNC_SPI_OUT		(1 << 1)	/* MISO Function Control: SPI Data Output (disabled in I2C mode) */
#define D_AIC3204_REG_MFP4_FUNC_OUTPUT		(2 << 1)	/* MISO Function Control: General Purpose Output */
#define D_AIC3204_REG_MFP4_FUNC_CLKOUT		(3 << 1)	/* MISO Function Control: CLKOUT Clock Output */
#define D_AIC3204_REG_MFP4_FUNC_INT1		(4 << 1)	/* MISO Function Control: INT1 Output */
#define D_AIC3204_REG_MFP4_FUNC_INT2		(5 << 1)	/* MISO Function Control: INT2 Output */
#define D_AIC3204_REG_MFP4_FUNC_ADC_WCLK	(6 << 1)	/* MISO Function Control: ADC Word Clock Output */
#define D_AIC3204_REG_MFP4_FUNC_DMIC_CLK	(7 << 1)	/* MISO Function Control: Digital Microphone Clock Output */
#define D_AIC3204_REG_MFP4_FUNC_SEC_DOUT	(8 << 1)	/* MISO Function Control: Secondary Data Output */
#define D_AIC3204_REG_MFP4_FUNC_SEC_BCLK	(9 << 1)	/* MISO Function Control: Secondary Bit Clock */
#define D_AIC3204_REG_MFP4_FUNC_SEC_WCLK	(10 << 1)	/* MISO Function Control: Secondary Word Clock */
#define D_AIC3204_REG_MFP4_FUNC				(15 << 1)	/* MISO Function Control Mask */

#define D_AIC3204_REG_MFP4_OUT				(1 << 0)	/* MISO Output State */


/* SCLK/MFP3 Function Control Register */
#define D_AIC3204_REG_MFP3					(0x38)

#define D_AIC3204_REG_MFP3_FUNC_DISABLED	(0 << 1)	/* SCLK Function Control: Disabled */
#define D_AIC3204_REG_MFP3_FUNC_SPI_SI_DM	(1 << 1)	/* SCLK Function Control: SPI Clock / Secondary Interface / Digital Mic Input */
#define D_AIC3204_REG_MFP3_FUNC_INPUT		(2 << 1)	/* SCLK Function Control: General Purpose Input */
#define D_AIC3204_REG_MFP3_FUNC				(3 << 1)	/* SCLK Function Control Mask */

#define D_AIC3204_REG_MFP3_IN				(1 << 0)	/* SCLK Input State */


/* DAC Signal Processing Block Control Register */
#define D_AIC3204_REG_DACSPB				(0x3C)

#define D_AIC3204_REG_DACSPB_MASK			(0x1f)		/* DAC Signal Processing Block Mask */


/* ADC Signal Processing Block Control Register */
#define D_AIC3204_REG_ADCSPB				(0x3D)

#define D_AIC3204_REG_ADCSPB_MASK			(0x1f)		/* ADC Signal Processing Block Mask */


/* DAC Channel Setup Register 1 */
#define D_AIC3204_REG_DACS1					(0x3F)

#define D_AIC3204_REG_DACS1_LDAC_UP_SHIFT	(7)			/* Left DAC Powered Up */

#define D_AIC3204_REG_DACS1_RDAC_UP_SHIFT	(6)			/* Right DAC Powered Up */

#define D_AIC3204_REG_DACS1_LDACD_DIS		(0 << 4)	/* Left DAC Data Path Control: Disabled */
#define D_AIC3204_REG_DACS1_LDACD_LEFT		(1 << 4)	/* Left DAC Data Path Control: Left Data */
#define D_AIC3204_REG_DACS1_LDACD_RIGHT		(2 << 4)	/* Left DAC Data Path Control: Right Data */
#define D_AIC3204_REG_DACS1_LDACD_MIX		(3 << 4)	/* Left DAC Data Path Control: Left + Right */
#define D_AIC3204_REG_DACS1_LDACD			(3 << 4)	/* Left DAC Data Path Control Mask */

#define D_AIC3204_REG_DACS1_RDACD_DIS		(0 << 2)	/* Right DAC Data Path Control: Disabled */
#define D_AIC3204_REG_DACS1_RDACD_RIGHT		(1 << 2)	/* Right DAC Data Path Control: Right Data */
#define D_AIC3204_REG_DACS1_RDACD_LEFT		(2 << 2)	/* Right DAC Data Path Control: Left Data */
#define D_AIC3204_REG_DACS1_RDACD_MIX		(3 << 2)	/* Right DAC Data Path Control: Left + Right */
#define D_AIC3204_REG_DACS1_RDACD			(3 << 2)	/* Right DAC Data Path Control Mask */

#define D_AIC3204_REG_DACS1_SOFT_1SEC		(0 << 0)	/* DAC Soft-Step: 1 step every clock */
#define D_AIC3204_REG_DACS1_SOFT_1SE2C		(1 << 0)	/* DAC Soft-Step: 1 step every 2 clocks */
#define D_AIC3204_REG_DACS1_SOFT_DIS		(2 << 0)	/* DAC Soft-Step: Disabled */
#define D_AIC3204_REG_DACS1_SOFT			(3 << 0)	/* DAC Soft-Step Mask */


/* DAC Channel Setup Register 2 */
#define D_AIC3204_REG_DACS2					(0x40)

#define D_AIC3204_REG_DACS2_RMOD_INV_SHIFT	(7)			/* Right Modulator Output Control: Output is inverted left output */

#define D_AIC3204_REG_DACS2_AMUTE_DIS		(0 << 4)	/* DAC Auto Mute: Disabled */
#define D_AIC3204_REG_DACS2_AMUTE_100		(1 << 4)	/* DAC Auto Mute: After 100 samples of DC */
#define D_AIC3204_REG_DACS2_AMUTE_200		(2 << 4)	/* DAC Auto Mute: After 200 samples of DC */
#define D_AIC3204_REG_DACS2_AMUTE_400		(3 << 4)	/* DAC Auto Mute: After 400 samples of DC */
#define D_AIC3204_REG_DACS2_AMUTE_800		(4 << 4)	/* DAC Auto Mute: After 800 samples of DC */
#define D_AIC3204_REG_DACS2_AMUTE_1600		(5 << 4)	/* DAC Auto Mute: After 1600 samples of DC */
#define D_AIC3204_REG_DACS2_AMUTE_3200		(6 << 4)	/* DAC Auto Mute: After 3200 samples of DC */
#define D_AIC3204_REG_DACS2_AMUTE_6400		(7 << 4)	/* DAC Auto Mute: After 6400 samples of DC */
#define D_AIC3204_REG_DACS2_AMUTE			(7 << 4)	/* DAC Auto Mute Mask */

#define D_AIC3204_REG_DACS2_LEFT_MUTE_SHIFT		(3)		/* Left DAC Muted */

#define D_AIC3204_REG_DACS2_RIGHT_MUTE_SHIFT	(2)		/* Right DAC Muted */

#define D_AIC3204_REG_DACS2_VOL_IND			(0 << 0)	/* DAC Master Volume Control: Independant */
#define D_AIC3204_REG_DACS2_VOL_LEFT		(1 << 0)	/* DAC Master Volume Control: Left controls right */
#define D_AIC3204_REG_DACS2_VOL_RIGHT		(2 << 0)	/* DAC Master Volume Control: Right controls left */
#define D_AIC3204_REG_DACS2_VOL				(3 << 0)	/* DAC Master Volume Control Mask */


/* Left DAC Digital Volume Control */
#define D_AIC3204_REG_LDACVOL				(0x41)


/* Right DAC Digital Volume Control */
#define D_AIC3204_REG_RDACVOL				(0x42)


/* Headset Detection Configuration Register */
#define D_AIC3204_REG_HSDET					(0x43)

#define D_AIC3204_REG_HSDET_ENABLE			(1 << 7)	/* Enable Headset Detection */

#define D_AIC3204_REG_HSDET_NONE			(0 << 5)	/* Headset Type: No Headset */
#define D_AIC3204_REG_HSDET_STEREO			(1 << 5)	/* Headset Type: Stereo Headset */
#define D_AIC3204_REG_HSDET_CELLSTEREO		(3 << 5)	/* Headset Type: Cellular Stereo Headset */

#define D_AIC3204_REG_HSDET_PLUGDT_16		(0 << 2)	/* Headset Detection Debounce Time: 16ms */
#define D_AIC3204_REG_HSDET_PLUGDT_32		(1 << 2)	/* Headset Detection Debounce Time: 32ms */
#define D_AIC3204_REG_HSDET_PLUGDT_64		(2 << 2)	/* Headset Detection Debounce Time: 64ms */
#define D_AIC3204_REG_HSDET_PLUGDT_128		(3 << 2)	/* Headset Detection Debounce Time: 128ms */
#define D_AIC3204_REG_HSDET_PLUGDT_256		(4 << 2)	/* Headset Detection Debounce Time: 256ms */
#define D_AIC3204_REG_HSDET_PLUGDT_512		(5 << 2)	/* Headset Detection Debounce Time: 512ms */
#define D_AIC3204_REG_HSDET_PLUGDT			(7 << 2)	/* Headset Detection Debounce Time Mask */

#define D_AIC3204_REG_HSDET_BTNDT_8			(0 << 0)	/* Headset Button Debounce Time: 8ms */
#define D_AIC3204_REG_HSDET_BTNDT_16		(1 << 0)	/* Headset Button Debounce Time: 16ms */
#define D_AIC3204_REG_HSDET_BTNDT_32		(2 << 0)	/* Headset Button Debounce Time: 32ms */
#define D_AIC3204_REG_HSDET_BTNDT			(3 << 0)	/* Headset Button Debounce Time Mask */


/* DRC Control Register 1 */
#define D_AIC3204_REG_DRC1					(0x44)

#define D_AIC3204_REG_DRC1_LEFT_EN			(1 << 6)	/* Left Channel DRC Enable */

#define D_AIC3204_REG_DRC1_RIGHT_EN			(1 << 5)	/* Right Channel DRC Enable */

#define D_AIC3204_REG_DRC1_THOLD_3DBFS		(0 << 2)	/* DRC Threshold: -3dBFS */
#define D_AIC3204_REG_DRC1_THOLD_6DBFS		(1 << 2)	/* DRC Threshold: -6dBFS */
#define D_AIC3204_REG_DRC1_THOLD_9DBFS		(2 << 2)	/* DRC Threshold: -9dBFS */
#define D_AIC3204_REG_DRC1_THOLD_12DBFS		(3 << 2)	/* DRC Threshold: -12dBFS */
#define D_AIC3204_REG_DRC1_THOLD_15DBFS		(4 << 2)	/* DRC Threshold: -15dBFS */
#define D_AIC3204_REG_DRC1_THOLD_18DBFS		(5 << 2)	/* DRC Threshold: -18dBFS */
#define D_AIC3204_REG_DRC1_THOLD_21DBFS		(6 << 2)	/* DRC Threshold: -21dBFS */
#define D_AIC3204_REG_DRC1_THOLD_24DBFS		(7 << 2)	/* DRC Threshold: -24dBFS */
#define D_AIC3204_REG_DRC1_THOLD			(7 << 2)	/* DRC Threshold Mask */

#define D_AIC3204_REG_DRC1_HYST_0DB			(0 << 0)	/* DRC Hysteresis: 0dB */
#define D_AIC3204_REG_DRC1_HYST_1DB			(1 << 0)	/* DRC Hysteresis: 1dB */
#define D_AIC3204_REG_DRC1_HYST_2DB			(2 << 0)	/* DRC Hysteresis: 2dB */
#define D_AIC3204_REG_DRC1_HYST_3DB			(3 << 0)	/* DRC Hysteresis: 3dB */
#define D_AIC3204_REG_DRC1_HYST				(3 << 0)	/* DRC Hysteresis Mask */


/* DRC Control Register 2 */
#define D_AIC3204_REG_DRC2					(0x45)

#define D_AIC3204_REG_DRC2_HOLD_DISABLED	(0 << 3)	/* DRC Hold: Disabled */
#define D_AIC3204_REG_DRC2_HOLD_32WC		(1 << 3)	/* DRC Hold: 32 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_64WC		(2 << 3)	/* DRC Hold: 64 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_128WC		(3 << 3)	/* DRC Hold: 128 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_256WC		(4 << 3)	/* DRC Hold: 256 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_512WC		(5 << 3)	/* DRC Hold: 512 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_1024WC		(6 << 3)	/* DRC Hold: 1024 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_2048WC		(7 << 3)	/* DRC Hold: 2048 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_4096WC		(8 << 3)	/* DRC Hold: 4096 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_8192WC		(9 << 3)	/* DRC Hold: 8192 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_16384WC		(10 << 3)	/* DRC Hold: 16384 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_32768WC		(11 << 3)	/* DRC Hold: 32768 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_65536WC		(12 << 3)	/* DRC Hold: 65536 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_98304WC		(13 << 3)	/* DRC Hold: 98304 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_131072WC	(14 << 3)	/* DRC Hold: 131072 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD_163840WC	(15 << 3)	/* DRC Hold: 163840 Word Clocks */
#define D_AIC3204_REG_DRC2_HOLD				(15 << 3)	/* DRC Hold Mask */


/* DRC Control Register 3 */
#define D_AIC3204_REG_DRC3					(0x46)

#define D_AIC3204_REG_DRC3_ATTACK_SHIFT		(4)			/* DRC Attack Rate Shift */
#define D_AIC3204_REG_DRC3_ATTACK			(15 << D_AIC3204_REG_DRC3_ATTACK_SHIFT)	/* DRC Attack Rate Mask */

#define D_AIC3204_REG_DRC3_DECAY_SHIFT		(0)			/* DRC Decay Rate Shift */
#define D_AIC3204_REG_DRC3_DECAY			(15 << D_AIC3204_REG_DRC3_DECAY_SHIFT)	/* DRC Decay Rate Mask */


/* Beep Generator Register 1 */
#define D_AIC3204_REG_BEEP1					(0x47)

#define D_AIC3204_REG_BEEP1_EN				(1 << 7)	/* Enable Beep Generator */
#define D_AIC3204_REG_BEEP1_VOL_SHIFT		(0)			/* Left Channel Beep Volume Shift */
#define D_AIC3204_REG_BEEP1_VOL				(0x3f)		/* Left Channel Beep Volume Mask */


/* Beep Generator Register 2 */
#define D_AIC3204_REG_BEEP2					(0x48)

#define D_AIC3204_REG_BEEP2_MVOL_IND		(0 << 6)	/* Beep Generator Master Volume: Independant Left/Right Channels */
#define D_AIC3204_REG_BEEP2_MVOL_LEFT		(1 << 6)	/* Beep Generator Master Volume: Left Channel controls Right */
#define D_AIC3204_REG_BEEP2_MVOL_RIGHT		(2 << 6)	/* Beep Generator Master Volume: Right Channel controls Left */
#define D_AIC3204_REG_BEEP2_MVOL			(3 << 6)	/* Beep Generator Master Volume Mask */


/* Beep Generator Register 3 */
#define D_AIC3204_REG_BEEP3					(0x49)

#define D_AIC3204_REG_BEEP3_LENGTH_VSHIFT	(16)		/* Sample Length Value Shift */
#define D_AIC3204_REG_BEEP3_LENGTH_SHIFT	(0)			/* Sample Length Shift */
#define D_AIC3204_REG_BEEP3_LENGTH			(0xff)		/* Sample Length Mask */


/* Beep Generator Register 4 */
#define D_AIC3204_REG_BEEP4					(0x4A)

#define D_AIC3204_REG_BEEP4_LENGTH_VSHIFT	(8)			/* Sample Length Value Shift */
#define D_AIC3204_REG_BEEP4_LENGTH_SHIFT	(0)			/* Sample Length Shift */
#define D_AIC3204_REG_BEEP4_LENGTH			(0xff)		/* Sample Length Mask */


/* Beep Generator Register 5 */
#define D_AIC3204_REG_BEEP5					(0x4B)

#define D_AIC3204_REG_BEEP5_LENGTH_VSHIFT	(0)			/* Sample Length Value Shift */
#define D_AIC3204_REG_BEEP5_LENGTH_SHIFT	(0)			/* Sample Length Shift */
#define D_AIC3204_REG_BEEP5_LENGTH			(0xff)		/* Sample Length Mask */


/* Beep Generator Register 6 */
#define D_AIC3204_REG_BEEP6					(0x4C)

#define D_AIC3204_REG_BEEP6_SINFREQ_VSHIFT	(8)			/* Relative Sine Frequency Value Shift */
#define D_AIC3204_REG_BEEP6_SINFREQ_SHIFT	(0)			/* Relative Sine Frequency Shift */
#define D_AIC3204_REG_BEEP6_SINFREQ			(0xff)		/* Relative Sine Frequency Mask */


/* Beep Generator Register 7 */
#define D_AIC3204_REG_BEEP7					(0x4D)

#define D_AIC3204_REG_BEEP7_SINFREQ_VSHIFT	(0)			/* Relative Sine Frequency Value Shift */
#define D_AIC3204_REG_BEEP7_SINFREQ_SHIFT	(0)			/* Relative Sine Frequency Shift */
#define D_AIC3204_REG_BEEP7_SINFREQ			(0xff)		/* Relative Sine Frequency Mask */


/* Beep Generator Register 8 */
#define D_AIC3204_REG_BEEP8					(0x4E)

#define D_AIC3204_REG_BEEP8_COSFREQ_VSHIFT	(8)			/* Relative Cosine Frequency Value Shift */
#define D_AIC3204_REG_BEEP8_COSFREQ_SHIFT	(0)			/* Relative Cosine Frequency Shift */
#define D_AIC3204_REG_BEEP8_COSFREQ			(0xff)		/* Relative Cosine Frequency Mask */



/* Beep Generator Register 9 */
#define D_AIC3204_REG_BEEP9					(0x4F)

#define D_AIC3204_REG_BEEP9_COSFREQ_VSHIFT	(0)			/* Relative Cosine Frequency Value Shift */
#define D_AIC3204_REG_BEEP9_COSFREQ_SHIFT	(0)			/* Relative Cosine Frequency Shift */
#define D_AIC3204_REG_BEEP9_COSFREQ			(0xff)		/* Relative Cosine Frequency Mask */


/* ADC Channel Setup Register */
#define D_AIC3204_REG_ADCS					(0x51)

#define D_AIC3204_REG_ADCS_LADC_UP_SHIFT	(7)			/* Left Channel ADC Powered Up */

#define D_AIC3204_REG_ADCS_RADC_UP_SHIFT	(6)			/* Right Channel ADC Powered Up */

#define D_AIC3204_REG_ADC5_DMICIN_GPIO		(0 << 4)	/* Digital Microphone Input: GPIO */
#define D_AIC3204_REG_ADC5_DMICIN_SCLK		(1 << 4)	/* Digital Microphone Input: SCLK */
#define D_AIC3204_REG_ADC5_DMICIN_DIN		(2 << 4)	/* Digital Microphone Input: DIN */
#define D_AIC3204_REG_ADC5_DMICIN			(3 << 4)	/* Digital Microphone Input Mask */

#define D_AIC3204_REG_ADC5_LADC_DMIC		(1 << 3)	/* Left ADC Source: Digital Microphone */

#define D_AIC3204_REG_ADC5_RADC_DMIC		(1 << 2)	/* Right ADC Source: Digital Microphone */

#define D_AIC3204_REG_ADC5_VOLSS_1SPWC		(0 << 0)	/* ADC Volume Soft Step: 1 step / word clock */
#define D_AIC3204_REG_ADC5_VOLSS_1SP2WC		(1 << 0)	/* ADC Volume Soft Step: 1 step / 2 word clocks */
#define D_AIC3204_REG_ADC5_VOLSS_DISABLED	(2 << 0)	/* ADC Volume Soft Step: Disabled */
#define D_AIC3204_REG_ADC5_VOLSS			(3 << 0)	/* ADC Volume Soft Step Mask */


/* ADC Fine Gain Adjust Register */
#define D_AIC3204_REG_ADCFINE				(0x52)

#define D_AIC3204_REG_ADCFINE_LEFT_MUTE		(1 << 7)	/* Left ADC Mute */

#define D_AIC3204_REG_ADCFINE_LEFT_0DB		(0 << 4)	/* Left ADC Gain: 0dB */
#define D_AIC3204_REG_ADCFINE_LEFT_0DB1		(1 << 4)	/* Left ADC Gain: -0.1dB */
#define D_AIC3204_REG_ADCFINE_LEFT_0DB2		(2 << 4)	/* Left ADC Gain: -0.2dB */
#define D_AIC3204_REG_ADCFINE_LEFT_0DB3		(2 << 4)	/* Left ADC Gain: -0.3dB */
#define D_AIC3204_REG_ADCFINE_LEFT_0DB4		(3 << 4)	/* Left ADC Gain: -0.4dB */
#define D_AIC3204_REG_ADCFINE_LEFT			(7 << 4)	/* Left ADC Gain Mask */

#define D_AIC3204_REG_ADCFINE_RIGHT_MUTE	(1 << 3)	/* Right ADC Mute */

#define D_AIC3204_REG_ADCFINE_RIGHT_0DB		(0 << 0)	/* Right ADC Gain: 0dB */
#define D_AIC3204_REG_ADCFINE_RIGHT_0DB1	(1 << 0)	/* Right ADC Gain: -0.1dB */
#define D_AIC3204_REG_ADCFINE_RIGHT_0DB2	(2 << 0)	/* Right ADC Gain: -0.2dB */
#define D_AIC3204_REG_ADCFINE_RIGHT_0DB3	(2 << 0)	/* Right ADC Gain: -0.3dB */
#define D_AIC3204_REG_ADCFINE_RIGHT_0DB4	(3 << 0)	/* Right ADC Gain: -0.4dB */
#define D_AIC3204_REG_ADCFINE_RIGHT			(7 << 0)	/* Right ADC Gain Mask */


/* Left ADC Channel Volume Register */
#define D_AIC3204_REG_LADCVOL				(0x53)

#define D_AIC3204_REG_LADCVOL_MASK			(0x7f)		/* Left ADC Channel Volume Mask */


/* Right ADC Channel Volume Register */
#define D_AIC3204_REG_RADCVOL				(0x54)

#define D_AIC3204_REG_RADCVOL_MASK			(0x7f)		/* Right ADC Channel Volume Mask */


/* ADC Phase Adjust Register */
#define D_AIC3204_REG_ADCPHASE				(0x55)


/* Left Channel AGC Control Register 1 */
#define D_AIC3204_REG_LAGC1					(0x56)

#define D_AIC3204_REG_LAGC1_ENABLE				(1 << 7)	/* Left Channel AGC Enabled */

#define D_AIC3204_REG_LAGC1_LEVEL_5DBFS5		(0 << 4)	/* Left Channel Target Level: -5.5dBFS */
#define D_AIC3204_REG_LAGC1_LEVEL_8DBFS			(1 << 4)	/* Left Channel Target Level: -8.0dBFS */
#define D_AIC3204_REG_LAGC1_LEVEL_10DBFS		(2 << 4)	/* Left Channel Target Level: -10dBFS */
#define D_AIC3204_REG_LAGC1_LEVEL_12DBFS		(3 << 4)	/* Left Channel Target Level: -12dBFS */
#define D_AIC3204_REG_LAGC1_LEVEL_14DBFS		(4 << 4)	/* Left Channel Target Level: -14dBFS */
#define D_AIC3204_REG_LAGC1_LEVEL_17DBFS		(5 << 4)	/* Left Channel Target Level: -17dBFS */
#define D_AIC3204_REG_LAGC1_LEVEL_20DBFS		(6 << 4)	/* Left Channel Target Level: -20dBFS */
#define D_AIC3204_REG_LAGC1_LEVEL_24DBFS		(7 << 4)	/* Left Channel Target Level: -24dBFS */
#define D_AIC3204_REG_LAGC1_LEVEL				(7 << 4)	/* Left Channel Target Level Mask */

#define D_AIC3204_REG_LAGC1_GAINHYST_DISABLED	(0 << 0)	/* Left Channel Gain Hysteresis: Disabled */
#define D_AIC3204_REG_LAGC1_GAINHYST_0DB5		(1 << 0)	/* Left Channel Gain Hysteresis: 0.5dB */
#define D_AIC3204_REG_LAGC1_GAINHYST_1DB		(2 << 0)	/* Left Channel Gain Hysteresis: 1dB */
#define D_AIC3204_REG_LAGC1_GAINHYST_1DB5		(3 << 0)	/* Left Channel Gain Hysteresis: 1.5dB */
#define D_AIC3204_REG_LAGC1_GAINHYST			(3 << 0)	/* Left Channel Gain Hysteresis Mask */


/* Left Channel AGC Control Register 2 */
#define D_AIC3204_REG_LAGC2					(0x57)

#define D_AIC3204_REG_LAGC2_HYST_1DB		(0 << 6)	/* Left Channel AGC Hysteresis: 1dB */
#define D_AIC3204_REG_LAGC2_HYST_2DB		(1 << 6)	/* Left Channel AGC Hysteresis: 2dB */
#define D_AIC3204_REG_LAGC2_HYST_4DB		(2 << 6)	/* Left Channel AGC Hysteresis: 4dB */
#define D_AIC3204_REG_LAGC2_HYST_DISABLED	(3 << 6)	/* Left Channel AGC Hysteresis: Disabled */
#define D_AIC3204_REG_LAGC2_HYST			(3 << 6)	/* Left Channel AGC Hysteresis Mask */

#define D_AIC3204_REG_LAGC2_NGATE			(0x1f)		/* Left Channel AGC Noise Gate Threshold Mask */

#define D_AIC3204_REG_LAGC2_NGATE_SHIFT		(1)			/* Left Channel AGC Noise Gate Threshold Shift */


/* Left Channel AGC Control Register 3 */
#define D_AIC3204_REG_LAGC3					(0x58)


/* Left Channel AGC Control Register 4 */
#define D_AIC3204_REG_LAGC4					(0x59)


/* Left Channel AGC Control Register 5 */
#define D_AIC3204_REG_LAGC5					(0x5A)


/* Left Channel AGC Control Register 6 */
#define D_AIC3204_REG_LAGC6					(0x5B)


/* Left Channel AGC Control Register 7 */
#define D_AIC3204_REG_LAGC7					(0x5C)


/* Left Channel AGC Control Register 8 */
#define D_AIC3204_REG_LAGC8					(0x5D)


/* Right Channel AGC Control Register 1 */
#define D_AIC3204_REG_RAGC1					(0x5E)


/* Right Channel AGC Control Register 2 */
#define D_AIC3204_REG_RAGC2					(0x5F)


/* Right Channel AGC Control Register 3 */
#define D_AIC3204_REG_RAGC3					(0x60)


/* Right Channel AGC Control Register 4 */
#define D_AIC3204_REG_RAGC4					(0x61)


/* Right Channel AGC Control Register 5 */
#define D_AIC3204_REG_RAGC5					(0x62)


/* Right Channel AGC Control Register 6 */
#define D_AIC3204_REG_RAGC6					(0x63)


/* Right Channel AGC Control Register 7 */
#define D_AIC3204_REG_RAGC7					(0x64)


/* Right Channel AGC Control Register 8 */
#define D_AIC3204_REG_RAGC8					(0x65)


/* DC Measurement Register 1 */
#define D_AIC3204_REG_DCM1					(0x66)


/* DC Measurement Register 2 */
#define D_AIC3204_REG_DCM2					(0x67)


/* Left Channel DC Measurement Output Register 1 */
#define D_AIC3204_REG_LDCMOUT1				(0x68)


/* Left Channel DC Measurement Output Register 2 */
#define D_AIC3204_REG_LDCMOUT2				(0x69)


/* Left Channel DC Measurement Output Register 3 */
#define D_AIC3204_REG_LDCMOUT3				(0x6A)


/* Right Channel DC Measurement Output Register 1 */
#define D_AIC3204_REG_RDCMOUT1				(0x6B)


/* Right Channel DC Measurement Output Register 2 */
#define D_AIC3204_REG_RDCMOUT2				(0x6C)


/* Right Channel DC Measurement Output Register 3 */
#define D_AIC3204_REG_RDCMOUT3				(0x6D)



/****************************/
/*	Page 1					*/
/****************************/

/* Power Configuration Register */
#define D_AIC3204_REG_POWER					(0x01)

#define D_AIC3204_REG_POWER_AVDD_DVDD_UNLINK	(1 << 3)	/* Disable AVDD -> DVDD Link */


/* LDO Control Register */
#define D_AIC3204_REG_LDO					(0x02)

#define D_AIC3204_REG_LDO_DVDD_1V72			(0 << 6)	/* DVDD LDO: 1.72V */
#define D_AIC3204_REG_LDO_DVDD_1V67			(1 << 6)	/* DVDD LDO: 1.67V */
#define D_AIC3204_REG_LDO_DVDD_1V77			(2 << 6)	/* DVDD LDO: 1.77V */
#define D_AIC3204_REG_LDO_DVDD				(3 << 6)	/* DVDD LDO Mask */

#define D_AIC3204_REG_LDO_AVDD_1V72			(0 << 4)	/* AVDD LDO: 1.72V */
#define D_AIC3204_REG_LDO_AVDD_1V67			(1 << 4)	/* AVDD LDO: 1.67V */
#define D_AIC3204_REG_LDO_AVDD_1V77			(2 << 4)	/* AVDD LDO: 1.77V */
#define D_AIC3204_REG_LDO_AVDD				(3 << 4)	/* AVDD LDO Mask */

#define D_AIC3204_REG_LDO_ANALOG_ENABLED	(0 << 3)	/* Analog Blocks Enabled */
#define D_AIC3204_REG_LDO_ANALOG_DISABLED	(1 << 3)	/* Analog Blocks Disabled */
#define D_AIC3204_REG_LDO_ANALOG			(1 << 3)	/* Analog Block Status */

#define D_AIC3204_REG_LDO_DVDD_OVERCURRENT	(1 << 2)	/* DVDD Over Current Detect */

#define D_AIC3204_REG_LDO_AVDD_OVERCURRENT	(1 << 1)	/* AVDD Over Current Detect */

#define D_AIC3204_REG_LDO_AVDD_UP			(1 << 0)	/* AVDD Powered Up */


/* Playback Configuration Register 1 */
#define D_AIC3204_REG_PLAYBACK1				(0x03)


/* Playback Configuration Register 2 */
#define D_AIC3204_REG_PLAYBACK2				(0x04)


/* Output Driver Power Control Register */
#define D_AIC3204_REG_OUTDRV				(0x09)

#define D_AIC3204_REG_OUTDRV_HPL_UP_SHIFT	(5)			/* Left Headphone Driver Up */
#define D_AIC3204_REG_OUTDRV_HPR_UP_SHIFT	(4)			/* Right Headphone Driver Up */
#define D_AIC3204_REG_OUTDRV_LOL_UP_SHIFT	(3)			/* Left Line Output Driver Up */
#define D_AIC3204_REG_OUTDRV_LOR_UP_SHIFT	(2)			/* Right Line Output Driver Up */
#define D_AIC3204_REG_OUTDRV_MAL_UP_SHIFT	(1)			/* Left Mixer Amplifier Up */
#define D_AIC3204_REG_OUTDRV_MAR_UP_SHIFT	(0)			/* Right Mixer Amplifier Up */


/* Common Mode Control Register */
#define D_AIC3204_REG_CMCTL					(0x0A)

#define D_AIC3204_REG_CMCTL_FCCM_0V9			(0 << 6)	/* Full Chip Common Mode = 0.9V */
#define D_AIC3204_REG_CMCTL_FCCM_0V75			(1 << 6)	/* Full Chip Common Mode = 0.75V */
#define D_AIC3204_REG_CMCTL_FCCM				(1 << 6)	/* Full Chip Common Mode Mask */

#define D_AIC3204_REG_CMCTL_HPCM_FCCM			(0 << 4)	/* Headphone Output Common Mode = Full Chip Common Mode */
#define D_AIC3204_REG_CMCTL_HPCM_1V25			(1 << 4)	/* Headphone Output Common Mode = 1.25V */
#define D_AIC3204_REG_CMCTL_HPCM_1V5			(2 << 4)	/* Headphone Output Common Mode = 1.5V */
#define D_AIC3204_REG_CMCTL_HPCM_1V65			(3 << 4)	/* Headphone Output Common Mode = 1.65V */
#define D_AIC3204_REG_CMCTL_HPCM				(3 << 4)	/* Headphone Output Common Mode Mask */

#define D_AIC3204_REG_CMCTL_LOCM_FCCM			(0 << 3)	/* Line Output Common Mode = Full Chip Common Mode */
#define D_AIC3204_REG_CMCTL_LOCM_1V65LDOIN		(1 << 3)	/* Line Output Common Mode = 1.65V from LDOin */
#define D_AIC3204_REG_CMCTL_LOCM				(1 << 3)	/* Line Output Common Mode Mask */

#define D_AIC3204_REG_CMCTL_HPCMSRC_AVDD		(0 << 1)	/* Headphone Output Common Mode Source = AVDD */
#define D_AIC3204_REG_CMCTL_HPCMSRC_LDOIN		(1 << 1)	/* Headphone Output Common Mode Source = LDOin */
#define D_AIC3204_REG_CMCTL_HPCMSRC				(1 << 1)	/* Headphone Output Common Mode Source Mask */

#define D_AIC3204_REG_CMCTL_LDORANGE_1V5_1V95	(0 << 0)	/* LDO Input Range = 1.5~1.95V */
#define D_AIC3204_REG_CMCTL_LDORANGE_1V8_3V6	(1 << 0)	/* LDO Input Range = 1.8~3.6V */
#define D_AIC3204_REG_CMCTL_LDORANGE			(1 << 0)	/* LDO Input Range Mask (effective when HPCMSRC = LDOIN) */


/* Over Current Protection Configuration Register */
#define D_AIC3204_REG_OCP					(0x0B)


/* Left Headphone Driver Routing Register */
#define D_AIC3204_REG_HPLROUTE				(0x0C)

#define D_AIC3204_REG_HPLROUTE_LDACP_SHIFT	(3)			/* Left Headphone Driver connects to LDAC+ */
#define D_AIC3204_REG_HPLROUTE_IN1L_SHIFT	(2)			/* Left Headphone Driver connects to IN1L */
#define D_AIC3204_REG_HPLROUTE_MAL_SHIFT	(1)			/* Left Headphone Driver connects to MAL */
#define D_AIC3204_REG_HPLROUTE_MAR_SHIFT	(0)			/* Left Headphone Driver connects to MAR */


/* Right Headphone Driver Routing Register */
#define D_AIC3204_REG_HPRROUTE				(0x0D)

#define D_AIC3204_REG_HPRROUTE_LDACN_SHIFT	(4)			/* Right Headphone Driver connects to LDAC- */
#define D_AIC3204_REG_HPRROUTE_RDACP_SHIFT	(3)			/* Right Headphone Driver connects to RDAC+ */
#define D_AIC3204_REG_HPRROUTE_IN1R_SHIFT	(2)			/* Right Headphone Driver connects to IN1L */
#define D_AIC3204_REG_HPRROUTE_MAR_SHIFT	(1)			/* Right Headphone Driver connects to MAL */
#define D_AIC3204_REG_HPRROUTE_HPL_SHIFT	(0)			/* Right Headphone Driver connects to HPL */


/* Left Line Output Routing Register */
#define D_AIC3204_REG_LOLROUTE				(0x0E)

#define D_AIC3204_REG_LOLROUTE_RDACN_SHIFT	(4)			/* Left Line Output Driver connects to RDAC- */
#define D_AIC3204_REG_LOLROUTE_LDACP_SHIFT	(3)			/* Left Line Output Driver connects to LDAC+ */
#define D_AIC3204_REG_LOLROUTE_MAL_SHIFT	(1)			/* Left Line Output Driver connects to MAL */
#define D_AIC3204_REG_LOLROUTE_LOR_SHIFT	(0)			/* Left Line Output Driver connects to LOR */



/* Right Line Output Routing Register */
#define D_AIC3204_REG_LORROUTE				(0x0F)

#define D_AIC3204_REG_LORROUTE_RDACP_SHIFT	(3)			/* Right Line Output Driver connects to RDAC+ */
#define D_AIC3204_REG_LORROUTE_MAR_SHIFT	(1)			/* Right Line Output Driver connects to MAR */


/* Left Headphone Gain Setting Register */
#define D_AIC3204_REG_HPLGAIN				(0x10)

#define D_AIC3204_REG_HPLGAIN_MUTE_SHIFT	(6)			/* Left Headphone Mute */
#define D_AIC3204_REG_HPLGAIN_MASK			(0x3f)		/* Left Headphone Gain Mask */


/* Right Headphone Gain Setting Register */
#define D_AIC3204_REG_HPRGAIN				(0x11)

#define D_AIC3204_REG_HPRGAIN_MUTE_SHIFT	(6)			/* Right Headphone Mute */
#define D_AIC3204_REG_HPRGAIN_MASK			(0x3f)		/* Right Headphone Gain Mask */


/* Left Line Output Gain Setting Register */
#define D_AIC3204_REG_LOLGAIN				(0x12)

#define D_AIC3204_REG_LOLGAIN_MUTE_SHIFT	(6)			/* Left Line Output Mute */
#define D_AIC3204_REG_LOLGAIN_MASK			(0x3f)		/* Left Line Output Gain Mask */


/* Right Line Output Gain Setting Register */
#define D_AIC3204_REG_LORGAIN				(0x13)

#define D_AIC3204_REG_LORGAIN_MUTE_SHIFT	(6)			/* Right Line Output Mute */
#define D_AIC3204_REG_LORGAIN_MASK			(0x3f)		/* Right Line Output Gain Mask */


/* Headphone Driver Startup Control Register */
#define D_AIC3204_REG_HPDRVSTART			(0x14)


/* IN1L to HPL Volume Control Register */
#define D_AIC3204_REG_IN1L2HPLVOL			(0x16)


/* IN1R to HPR Volume Control Register */
#define D_AIC3204_REG_IN1R2HPRVOL			(0x17)


/* Left Mixer Amplifier Gain Setting Register */
#define D_AIC3204_REG_MALGAIN				(0x18)

#define D_AIC3204_REG_MALGAIN_MASK			(0x3f)		/* Left Mixer Amplifier Gain Mask */


/* Right Mixer Amplifier Gain Setting Register */
#define D_AIC3204_REG_MARGAIN				(0x19)

#define D_AIC3204_REG_MARGAIN_MASK			(0x3f)		/* Right Mixer Amplifier Gain Mask */


/* MICBIAS Configuration Register */
#define D_AIC3204_REG_MICBIAS				(0x33)


/* Left MICPGA Positive Terminal Input Routing Configuration Register */
#define D_AIC3204_REG_LMICPGAPROUTE			(0x34)


/* Left MICPGA Negative Terminal Input Routing Configuration Register */
#define D_AIC3204_REG_LMICPGANROUTE			(0x36)


/* Right MICPGA Positive Terminal Input Routing Configuration Register */
#define D_AIC3204_REG_RMICPGAPROUTE			(0x37)


/* Right MICPGA Negative Terminal Input Routing Configuration Register */
#define D_AIC3204_REG_RMICPGANROUTE			(0x39)

/* Floating Input Configuration Register */
#define D_AIC3204_REG_FLOATIN				(0x3A)


/* Left MICPGA Volume Control Register */
#define D_AIC3204_REG_LMICPGAVOL			(0x3B)


/* Right MICPGA Volume Control Register */
#define D_AIC3204_REG_RMICPGAVOL			(0x3C)


/* ADC Power Tune Mode Register */
#define D_AIC3204_REG_ADCPTM				(0x3D)

#define D_AIC3204_REG_ADCPTM_R4				(0x00)		/* ADC Power Tune Mode: PTM_R4 */
#define D_AIC3204_REG_ADCPTM_R3				(0x64)		/* ADC Power Tune Mode: PTM_R3 */
#define D_AIC3204_REG_ADCPTM_R2				(0xb6)		/* ADC Power Tune Mode: PTM_R2 */
#define D_AIC3204_REG_ADCPTM_R1				(0xff)		/* ADC Power Tune Mode: PTM_R1 */


/* ADC Analog Volume Control Flag Register */
#define D_AIC3204_REG_ADCAVCF				(0x3E)



/* DAC Analog Gain Control Flag Register */
#define D_AIC3204_REG_DACAGCF				(0x3F)


/* Analogue Input Quick Charging Configuration Register */
#define D_AIC3204_REG_AIQC					(0x47)

#define D_AIC3204_REG_AIQC_PWRUP_DEFAULT	(0x00)		/* Power up time: Default Value (whatever that is) */
#define D_AIC3204_REG_AIQC_PWRUP_3M1S		(0x31)		/* Power up time: 3.1ms */
#define D_AIC3204_REG_AIQC_PWRUP_6M4S		(0x32)		/* Power up time: 6.4ms */
#define D_AIC3204_REG_AIQC_PWRUP_1M6S		(0x33)		/* Power up time: 1.6ms */
#define D_AIC3204_REG_AIQC_PWRUP			(0x3f)		/* Power up time mask */


/* Reference Power-Up Configuration Register */
#define D_AIC3204_REG_REFPU					(0x7B)

#define D_AIC3204_REG_REFPU_DELAY_SLOW		(0 << 0)	/* Reference Power-Up delay: Slow */
#define D_AIC3204_REG_REFPU_DELAY_40MS		(1 << 0)	/* Reference Power-Up delay: 40ms */
#define D_AIC3204_REG_REFPU_DELAY_80MS		(2 << 0)	/* Reference Power-Up delay: 80ms */
#define D_AIC3204_REG_REFPU_DELAY_120MS		(3 << 0)	/* Reference Power-Up delay: 120ms */

#define D_AIC3204_REG_REFPU_FORCE			(1 << 2)	/* Force power-up */


/* Set Register = Value */
#define D_AIC3204_REG_INITFLAG_SET		(0 << 0)
/* Set Register = Register & Value */
#define D_AIC3204_REG_INITFLAG_AND		(1 << 0)
/* Set Register = Register | Value */
#define D_AIC3204_REG_INITFLAG_OR		(2 << 0)
/* Set Register = Register ^ Value */
#define D_AIC3204_REG_INITFLAG_XOR		(3 << 0)
/* Fail on Register != Value */
#define D_AIC3204_REG_INITFLAG_FAILNEQ	(4 << 0)
/* Fail on Register == Value */
#define D_AIC3204_REG_INITFLAG_FAILEQ		(5 << 0)
/* Fail on (Register & Value) == 0 */
#define D_AIC3204_REG_INITFLAG_FAILNSET	(6 << 0)
/* Fail on (Register & Value) != 0 */
#define D_AIC3204_REG_INITFLAG_FAILSET	(7 << 0)
/* Fail on (Register | Value) == 0xff */
#define D_AIC3204_REG_INITFLAG_FAILCLR	(8 << 0)
/* Fail on (Register | Value) != 0xff */
#define D_AIC3204_REG_INITFLAG_FAILNCLR	(9 << 0)
/* Register operation mask */
#define D_AIC3204_REG_INITFLAG_OPMASK		(15 << 0)



/**** Status ****/

#define D_AIC3204_USED_BIT_REC	(0x01)
#define D_AIC3204_USED_BIT_PLAY	(0x02)


/**** GPIO ****/
// #define AIC3204_POWER_ON()			{ FJ_Gpio_Set_Status(FJ_GPIO_P70, FJ_GPIO_STATUS_HIGH); FJ_Gpio_Set_Status(FJ_GPIO_P71, FJ_GPIO_STATUS_HIGH); }
// #define AIC3204_POWER_OFF()			{ FJ_Gpio_Set_Status(FJ_GPIO_P70, FJ_GPIO_STATUS_LOW); FJ_Gpio_Set_Status(FJ_GPIO_P71, FJ_GPIO_STATUS_LOW);}

// #define AIC3204_POWER_AMP_ON()			{ FJ_Gpio_Set_Status(FJ_GPIO_P72, FJ_GPIO_STATUS_HIGH); }
// #define AIC3204_POWER_AMP_OFF()			{ FJ_Gpio_Set_Status(FJ_GPIO_P72, FJ_GPIO_STATUS_LOW); }

// #define AIC3204_SPI_ON()				{ FJ_Gpio_Set_Status(FJ_GPIO_P73, FJ_GPIO_STATUS_LOW); FJ_Gpio_Set_Status(FJ_GPIO_P74, FJ_GPIO_STATUS_LOW);	 }
// #define AIC3204_SPI_OFF()				{ FJ_Gpio_Set_Status(FJ_GPIO_P73, FJ_GPIO_STATUS_HIGH); FJ_Gpio_Set_Status(FJ_GPIO_P74, FJ_GPIO_STATUS_HIGH);	 }

#define AIC3204_POWER_ON()			
#define AIC3204_POWER_OFF()

#define AIC3204_POWER_AMP_ON()	
#define AIC3204_POWER_AMP_OFF()	

#define AIC3204_SPI_ON()		
#define AIC3204_SPI_OFF()		

#define AIC3204_OPEN()					{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.state = E_AIC3204_OPEN; Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define AIC3204_CLODE()					{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.state = E_AIC3204_CLOSE; Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define IS_AIC3204_OPEN()				( gAic3402Info.state == E_AIC3204_OPEN )


#define AIC3204_AUDIO_INIT_USED()		{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.used = 0x00; Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define AIC3204_AUDIO_REC_USED()		{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.used |= D_AIC3204_USED_BIT_REC; Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define AIC3204_AUDIO_REC_UNUSED()		{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.used &= ~(D_AIC3204_USED_BIT_REC); Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define IS_AIC3204_AUDIO_REC_USE()		((gAic3402Info.used & D_AIC3204_USED_BIT_REC) != 0x00)


#define AIC3204_AUDIO_PLAY_USED()		{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.used |= D_AIC3204_USED_BIT_PLAY; Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define AIC3204_AUDIO_PLAY_UNUSED()		{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.used &= ~(D_AIC3204_USED_BIT_PLAY); Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define IS_AIC3204_AUDIO_PLAY_USE()		((gAic3402Info.used & D_AIC3204_USED_BIT_PLAY) != 0x00)


#define AIC3204_REC_MODE_SET_STOP()		{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.rec_mode = E_AIC3204_MODE_STOP;  Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define AIC3204_REC_MODE_SET_START()	{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.rec_mode = E_AIC3204_MODE_RECORD; Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }

#define AIC3204_PLAY_MODE_SET_STOP()	{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.play_mode = E_AIC3204_MODE_STOP; Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }
#define AIC3204_PLAY_MODE_SET_START()	{ Dd_ARM_Critical_Section_Start(gAic3204_CrtSct); gAic3402Info.play_mode = E_AIC3204_MODE_PLAY; Dd_ARM_Critical_Section_End(gAic3204_CrtSct); }


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Audio Status */
typedef struct{
	E_AIC3204_STATE	state;			/* E_AIC3204_OPEN or E_AIC3204_CLOSE */
	unsigned char		used;			/* TRUE or FALSE */
	E_AIC3204_MODE		rec_mode;		/* E_AIC3204_MODE_STOP or E_AIC3204_MODE_RECORD or E_AIC3204_MODE_PLAY */
	E_AIC3204_MODE		play_mode;		/* E_AIC3204_MODE_STOP or E_AIC3204_MODE_RECORD or E_AIC3204_MODE_PLAY */
	E_AIC3204_VOLUME	volume;			/* E_AIC3204_VOLUME_MUTE or E_AIC3204_VOLUME_1~8 */
}T_AUDIO_CODE_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Audio Status */
static T_AUDIO_CODE_INFO gAic3402Info = {
	E_AIC3204_CLOSE,
	0x00,
	E_AIC3204_MODE_STOP,
	E_AIC3204_MODE_STOP,
	E_AIC3204_VOLUME_4,
};

/* Volume Table of AIC3204  								*/
static const unsigned char	vol_tbl[ E_AIC3204_VOLUME_STOPPER ] = {
	0x81,			/* vol 0  [-63.5dB] Min*/
	0xAC,			/* vol 1  [-52dB]*/
	0xC4,			/* vol 2  [-30dB]*/
	0xD6,			/* vol 3  [-21dB]*/
	0xE8,			/* vol 4  [-12dB]*/
	0xF4,			/* vol 5  [- 6dB]*/
	0x00,			/* vol 6  [  0dB]*/
	0x0C,			/* vol 7  [+ 6dB]*/
	0x18,			/* vol 8  [+12dB]*/
	0x24,			/* vol 9  [+15dB]*/
	0x30			/* vol 10 [+18dB] Max*/
};

static unsigned char gAic3205_PowerUp = FALSE;

// Critical Section
static unsigned long gAic3204_CrtSct __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Local Function  														*/
/*----------------------------------------------------------------------*/

static int aic3204_write(unsigned char addr, unsigned char data)
{
	FJ_SPI_CTRL spi_ctrl;
	FJ_SPI_CTRL_TRANSFER spi_trans_ctrl;
	unsigned char send_data[2];
	
	send_data[0] = (addr << 1) & 0xFE;
	send_data[1] = data;
	
	if (FJ_SPI_Open(FJ_PERI_CH_1) == FJ_ERR_OK){
		
		spi_ctrl.type = FJ_SPI_TYPE_MASTER;
		spi_ctrl.mode = FJ_SPI_MODE_1_CPOL0_CPHA1;
		spi_ctrl.bit_direction = FJ_SPI_BIT_DIR_MSB_FIRST;
		spi_ctrl.data_length = FJ_SPI_LENGTH_8;
		spi_ctrl.baudrate = 10000000;
		spi_ctrl.fifo_wmark_tx = 16;
		spi_ctrl.fifo_wmark_rx = 16;
		spi_ctrl.dma_en = 0;
		spi_ctrl.dma_recv_to = 0;
		spi_ctrl.pcallback = NULL;
		
		if (FJ_SPI_Ctrl(FJ_PERI_CH_1, &spi_ctrl) == FJ_ERR_OK){
			
			spi_trans_ctrl.ss_info.ssout[0] = 0;
			spi_trans_ctrl.ss_info.ssout[1] = 0;
			spi_trans_ctrl.ss_info.ssout[2] = 0;
			spi_trans_ctrl.ss_info.ssout[3] = 1;
			spi_trans_ctrl.ss_info.sspol[0] = 0;
			spi_trans_ctrl.ss_info.sspol[1] = 0;
			spi_trans_ctrl.ss_info.sspol[2] = 0;
			spi_trans_ctrl.ss_info.sspol[3] = 0;
			spi_trans_ctrl.ss_info.cont_trans = FJ_SPI_CONT_TRANS_INACT_FIFO_EMPTY;
			spi_trans_ctrl.direction	= FJ_SPI_DATA_DIR_SEND;
			spi_trans_ctrl.data_count	= 2;
			spi_trans_ctrl.data_ptr = send_data;
			
			FJ_SPI_Transfer(FJ_PERI_CH_1, &spi_trans_ctrl, FJ_USIO_DMA_CH_NOT_USE);
		}
		
		FJ_SPI_Close(FJ_PERI_CH_1);
	}
	
	return 0;
}


/*----------------------------------------------------------------------*/
/* API Function  														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		AIC3204 Powerup sequence
 * @param[in]	None
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note
 */
int AIC3204_Open( void )
{
	if( IS_AIC3204_OPEN() ){
		return D_AIC3204_ERROR;
	}
	
	AIC3204_OPEN();
	
	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 Close
 * @param[in]	None
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec End
 */
int AIC3204_Close( void )
{
	if( !IS_AIC3204_OPEN() ){
		return D_AIC3204_ERROR;
	}
	
	AIC3204_CLODE();
	
	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 Powerup
 * @param[in]	None
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec Audio Play Start
 */
int AIC3204_PowerUp( void )
{
	unsigned char aisr1 = 0;
	unsigned char clk6 = 0;
	unsigned char clk7 = 0;
	unsigned char clk8 = 0;
	unsigned char clk9 = 0;
	
	
	if (!IS_AIC3204_OPEN()){
		return D_AIC3204_ERROR;
	}
	
	if( gAic3205_PowerUp == TRUE ){
		return D_AIC3204_OK;
	}
	
	AIC3204_POWER_OFF();
	
	// Wait 1ms
	OS_User_Dly_Tsk(1);
	
	AIC3204_POWER_AMP_OFF();
	AIC3204_POWER_ON();
//	AIC3204_POWER_AMP_ON();
	gAic3205_PowerUp = TRUE;
	
	AIC3204_SPI_ON()
	
	/**** PAGE 0 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x00);	// [0x00] Select Page 0
	aic3204_write(D_AIC3204_REG_RESET_SOFT,		0x01);	// [0x01] Initialize the device through software reset
	
	aic3204_write(D_AIC3204_REG_CLK1,			D_AIC3204_REG_CLK1_CODECCLK_MCLK);	// [0x04] -[0x00]CODEC_CLKIN <- MCLK
	
	clk6 = D_AIC3204_REG_CLK6_NDAC_STATE_ON | 1;
	aic3204_write(D_AIC3204_REG_CLK6,			clk6);	// [0x0B] - [0x81]Power up the NDAC divider with value 1
	
	clk7 = D_AIC3204_REG_CLK7_MDAC_STATE_ON | 2;
	aic3204_write(D_AIC3204_REG_CLK7,			clk7);	// [0x0C] - [0x82]Power up the MDAC divider with value 2
	
	clk8 = D_AIC3204_REG_CLK8_NADC_STATE_ON | 1;
	aic3204_write(D_AIC3204_REG_CLK8,			clk8);	// [0x12] - [0x81]Power up the NADC divider with value 1
	
	clk9 = D_AIC3204_REG_CLK9_MADC_STATE_ON | 2;
	aic3204_write(D_AIC3204_REG_CLK9,			clk9);	// [0x13] - [0x82]Power up the MADC divider with value 2
	
	aic3204_write(D_AIC3204_REG_DOSR1,			0x00);	// [0x0D] Program the OSR of DAC to 128
	aic3204_write(D_AIC3204_REG_DOSR2,			0x80);	// [0x0E] Program the OSR of DAC to 128
	aic3204_write(D_AIC3204_REG_AOSR,			0x80);	// [0x14] Program the OSR of ADC to 128
	
	aisr1 =	D_AIC3204_REG_AISR1_BCLK_IN		|
			D_AIC3204_REG_AISR1_WCLK_IN		|
			D_AIC3204_REG_AISR1_INT_LJF		|
			D_AIC3204_REG_AISR1_WL_16BITS	;
	
	aic3204_write(D_AIC3204_REG_AISR1,			aisr1);	// [0x1B] - [0xC0]Left-Justified Format, 16bit, Slave
	
	aic3204_write(D_AIC3204_REG_AISR2,			0x00);	// [0x1C] Offset 0
	aic3204_write(D_AIC3204_REG_AISR3,			0x00);	// [0x1D] No Loopback
	
	
	/**** PAGE 1 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x01);	// [0x00] Select Page 1
	aic3204_write(D_AIC3204_REG_POWER,			D_AIC3204_REG_POWER_AVDD_DVDD_UNLINK);	// [0x01] - [0x08]Disabled weak connection of AVDD with DVDD
	aic3204_write(D_AIC3204_REG_LDO,			D_AIC3204_REG_LDO_AVDD_UP);				// [0x02] - [0x01]AVDD LDO Powered up
	aic3204_write(D_AIC3204_REG_CMCTL,			D_AIC3204_REG_CMCTL_LDORANGE_1V8_3V6);	// [0x0A] Set the input common mode to 0.9V. LDO_IN:3.3V
	
	AIC3204_AUDIO_INIT_USED();
	
	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 Powerup
 * @param[in]	None
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec Audio Play Start
 */
int AIC3204_PowerDown( void )
{
	if (!IS_AIC3204_OPEN()){
		return D_AIC3204_ERROR;
	}
	
	gAic3205_PowerUp = FALSE;
	
//	AIC3204_POWER_AMP_OFF();
	AIC3204_SPI_OFF();
	AIC3204_POWER_OFF();
	
	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 Audio Play Start
 * @param[in]	None
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec Audio Play Start
 */
int AIC3204_Play_Start( void )
{
	unsigned char lol_route = 0;
	unsigned char lor_route = 0;
	unsigned char outdrv = 0;
	unsigned char dacs1 = 0;

	if( !IS_AIC3204_OPEN() ){
		return D_AIC3204_ERROR;
	}
	
	if(IS_AIC3204_AUDIO_PLAY_USE()){
		return D_AIC3204_ERROR;
	}
	
	AIC3204_AUDIO_PLAY_USED();
	AIC3204_PLAY_MODE_SET_START();
	
	
	/**** PAGE 0 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x00);	// [0x00] Select Page 0
	aic3204_write(D_AIC3204_REG_DACSPB,			0x01);	// [0x3C] Set the DAC Mode to PRB_P1
	
	
	/**** PAGE 1 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x01);	// [0x00] Select Page 1
	aic3204_write(D_AIC3204_REG_REFPU,			D_AIC3204_REG_REFPU_DELAY_40MS);	// [0x7B]-[0x01]Set the REF charging time to 40ms
	
	lol_route = 1 << D_AIC3204_REG_LOLROUTE_LDACP_SHIFT;
	aic3204_write(D_AIC3204_REG_LOLROUTE,		lol_route);	// [0x0E]-[0x08]Route Left DAC to LOL
	
	lor_route = 1 << D_AIC3204_REG_LORROUTE_RDACP_SHIFT;
	aic3204_write(D_AIC3204_REG_LORROUTE,		lor_route);	// [0x0F]-[0x08]Route Right DAC to LOR
	
	aic3204_write(D_AIC3204_REG_PLAYBACK1,		0x00);	// [0x03] PTM_P3/4
	aic3204_write(D_AIC3204_REG_PLAYBACK2,		0x00);	// [0x04] PTM_P3/4
	aic3204_write(D_AIC3204_REG_LOLGAIN,		0x00);	// [0x12] Set the LOL gain to 0dB
	aic3204_write(D_AIC3204_REG_LORGAIN,		0x00);	// [0x13] Set the LOL gain to 0dB
	
	outdrv = (1 << D_AIC3204_REG_OUTDRV_LOL_UP_SHIFT)	|
			 (1 << D_AIC3204_REG_OUTDRV_LOR_UP_SHIFT)	;
	aic3204_write(D_AIC3204_REG_OUTDRV,			outdrv);	// [0x09]-[0x3C]Power up HPL/HPR/LOL/LOR drivers
	
	
	/**** PAGE 0 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x00);	// [0x00] Select Page 0
	
	dacs1 = (1 << D_AIC3204_REG_DACS1_LDAC_UP_SHIFT)	|
			(1 << D_AIC3204_REG_DACS1_RDAC_UP_SHIFT)	|
			D_AIC3204_REG_DACS1_LDACD_LEFT				|
			D_AIC3204_REG_DACS1_RDACD_RIGHT				|
			D_AIC3204_REG_DACS1_SOFT_1SEC				;
	aic3204_write(D_AIC3204_REG_DACS1,			dacs1);	// [0x3F]-[0xD4]Power up the Left and Right DAC
	
	aic3204_write(D_AIC3204_REG_DACS2,			0x00);	// [0x40] Unmute the DAC digital volume control
	aic3204_write(D_AIC3204_REG_LDACVOL,		vol_tbl[gAic3402Info.volume]);	// [0x41] Unmute the DAC digital volume control
	aic3204_write(D_AIC3204_REG_RDACVOL,		vol_tbl[gAic3402Info.volume]);	// [0x42] Unmute the DAC digital volume control
	
	//wait end of pop noise.(PowerUP;40ms PopNoise:10ms)
	OS_User_Dly_Tsk(52);
	
	AIC3204_POWER_AMP_ON();

	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 Audio Play Stop
 * @param[in]	None
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec Audio Play Stop
 */
int AIC3204_Play_Stop( void )
{
	unsigned char dacs1 = 0;
	unsigned char dacs2 = 0;
	
	if( !IS_AIC3204_OPEN() ){
		return D_AIC3204_ERROR;
	}
	
	if(!IS_AIC3204_AUDIO_PLAY_USE()){
		return D_AIC3204_ERROR;
	}
	
	AIC3204_POWER_AMP_OFF();
	
	/**** PAGE 0 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x00);	// [0x00] Select Page 0
	
	dacs2 = (1 << D_AIC3204_REG_DACS2_LEFT_MUTE_SHIFT) |
			(1 << D_AIC3204_REG_DACS2_RIGHT_MUTE_SHIFT);
	
	aic3204_write(D_AIC3204_REG_DACS2,		dacs2);	// [0x40]-[0x0C]Mute the DAC digital volume control
	
	dacs1 = (0 << D_AIC3204_REG_DACS1_LDAC_UP_SHIFT)	|
			(0 << D_AIC3204_REG_DACS1_RDAC_UP_SHIFT)	|
			D_AIC3204_REG_DACS1_LDACD_LEFT				|
			D_AIC3204_REG_DACS1_RDACD_RIGHT				|
			D_AIC3204_REG_DACS1_SOFT_DIS				;
	
	aic3204_write(D_AIC3204_REG_DACS1,		dacs1);	// [0x3F]-[0x16] Power down the Left and Right DAC
	
	AIC3204_PLAY_MODE_SET_STOP();
	AIC3204_AUDIO_PLAY_UNUSED();
	
	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 Audio Record Start
 * @param[in]	None
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec Audio Record Start
 */
int AIC3204_Record_Start( void )
{
	unsigned char adcs = 0;
	unsigned char agc1 = 0;
	unsigned char agc2 = 0;
	
	if( !IS_AIC3204_OPEN() ){
		return D_AIC3204_ERROR;
	}
	
	if(IS_AIC3204_AUDIO_REC_USE()){
		return D_AIC3204_ERROR;
	}
	
	AIC3204_AUDIO_REC_USED();
	AIC3204_REC_MODE_SET_START();
	
	
	/**** PAGE 0 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x00);	// [0x00] Select Page 0
	aic3204_write(D_AIC3204_REG_ADCSPB,			0x01);	// [0x3D] Set the ADC Mode to PRB_R1
	
	
	/**** PAGE 1 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x01);	// [0x00] Select Page 1
	aic3204_write(D_AIC3204_REG_ADCPTM,			D_AIC3204_REG_ADCPTM_R4);			// [0x3D]-[0x00] Select ADC PTM_R4
	aic3204_write(D_AIC3204_REG_AIQC,			D_AIC3204_REG_AIQC_PWRUP_3M1S);		// [0x47]-[0x31] Set MicPGA startup delay to 3.1ms
	aic3204_write(D_AIC3204_REG_REFPU,			D_AIC3204_REG_REFPU_DELAY_40MS);	// [0x7B]-[0x01] Set the REF charging time to 40ms
	
	aic3204_write(D_AIC3204_REG_MICBIAS,		0x78);	// [0x33] 
	
	aic3204_write(D_AIC3204_REG_LMICPGAPROUTE,	0x20);	// [0x34] Route IN2L to LEFT_P with 20K input impedance
	aic3204_write(D_AIC3204_REG_LMICPGANROUTE,	0x20);	// [0x36] Route Common Mode to LEFT_M with impedance of 20K
	aic3204_write(D_AIC3204_REG_RMICPGAPROUTE,	0x80);	// [0x37] Route IN1R to RIGHT_P with input impedance of 20K
	aic3204_write(D_AIC3204_REG_RMICPGANROUTE,	0x20);	// [0x39] Route Common Mode to RIGHT_M with impedance of 20K
	aic3204_write(D_AIC3204_REG_LMICPGAVOL,		0x30);	// [0x3B] 24dB
	aic3204_write(D_AIC3204_REG_RMICPGAVOL,		0x30);	// [0x3C] 24dB
	
	
	/**** PAGE 0 ****/
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x00);	// [0x00] Select Page 0
	
	adcs = (1 << D_AIC3204_REG_ADCS_LADC_UP_SHIFT)	|
		   (1 << D_AIC3204_REG_ADCS_RADC_UP_SHIFT)	;
	aic3204_write(D_AIC3204_REG_ADCS,		adcs);	// [0x51]-[0xC0]Power up Left and Right ADC Channels
	aic3204_write(D_AIC3204_REG_ADCFINE,	0x00);	// [0x52] Unmute Left and Right ADC Digital Volume Control.
	aic3204_write(D_AIC3204_REG_LADCVOL,	0x1E);	// [0x53] 15dB
	aic3204_write(D_AIC3204_REG_RADCVOL,	0x1E);	// [0x54] 15dB
	
	// ACG
	agc1 = D_AIC3204_REG_LAGC1_ENABLE | D_AIC3204_REG_LAGC1_LEVEL_5DBFS5;
	aic3204_write(D_AIC3204_REG_LAGC1,		agc1);	// [0x56]-[0x80] Left: AGC Enable, Terget gain = -5.5dB
	aic3204_write(D_AIC3204_REG_RAGC1,		agc1);	// [0x5E]-[0x80] Right: AGC Enable, Terget gain = -5.5dB
	
	agc2 = D_AIC3204_REG_LAGC2_HYST_2DB | (0x1E << D_AIC3204_REG_LAGC2_NGATE_SHIFT);
	aic3204_write(D_AIC3204_REG_LAGC2,		agc2);	// [0x57]-[0x7C] Left: Hysteresis=2dB, Noise threshold= -88dB
	aic3204_write(D_AIC3204_REG_RAGC2,		agc2);	// [0x5F]-[0x7C] Right: Hysteresis=2dB, Noise threshold= -88dB
	
	aic3204_write(D_AIC3204_REG_LAGC3,		0x5C);	// [0x58] Left: Maximum gain = 46dB
	aic3204_write(D_AIC3204_REG_RAGC3,		0x5C);	// [0x60] Right: Maximum gain = 46dB
	aic3204_write(D_AIC3204_REG_LAGC4,		0x08);	// [0x59] Left: Attack time = 20ms
	aic3204_write(D_AIC3204_REG_RAGC4,		0x08);	// [0x61] Right: Attack time = 20ms
	aic3204_write(D_AIC3204_REG_LAGC5,		0x32);	// [0x5A] Left: Decay time = 500ms
	aic3204_write(D_AIC3204_REG_RAGC6,		0x32);	// [0x62] Right: Decay time = 500ms
	aic3204_write(D_AIC3204_REG_LAGC6,		0x00);	// [0x5B] Left: Noise debounce time = 0ms
	aic3204_write(D_AIC3204_REG_RAGC6,		0x00);	// [0x63] Right: Noise debounce time = 0ms
	aic3204_write(D_AIC3204_REG_LAGC7,		0x06);	// [0x5C] Left: Signal debounce time = 2ms
	aic3204_write(D_AIC3204_REG_RAGC7,		0x06);	// [0x64] Right: Signal debounce time = 2ms
	
	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 Audio Record Stop
 * @param[in]	None
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec Audio Stop
 */
int AIC3204_Record_Stop( void )
{
	unsigned char adcfine = 0;
	
	if( !IS_AIC3204_OPEN() ){
		return D_AIC3204_ERROR;
	}
	
	if(!IS_AIC3204_AUDIO_REC_USE()){
		return D_AIC3204_ERROR;
	}
	
	aic3204_write(D_AIC3204_REG_PAGE_SELECT,	0x00);	// [0x00] Select Page 0
	
	adcfine = D_AIC3204_REG_ADCFINE_LEFT_MUTE	|
		   D_AIC3204_REG_ADCFINE_RIGHT_MUTE	;
	
	aic3204_write(D_AIC3204_REG_ADCFINE,		adcfine);	// [0x52]-[0x88]Mute Left and Right ADC Digital Volume Control.
	
	aic3204_write(D_AIC3204_REG_ADCS,			0x00);	// [0x51] Power down Left and Right ADC Channels
	
	AIC3204_REC_MODE_SET_STOP();
	AIC3204_AUDIO_REC_UNUSED();
	
	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 volume Change
 * @param[in]	E_AUDIO_VOLUME vol	; Audio volume
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec volume Change
 */
int AIC3204_Set_Volume( E_AIC3204_VOLUME vol )
{
	if (!IS_AIC3204_OPEN()){
		return D_AIC3204_ERROR;
	}
	
	if( vol >= E_AIC3204_VOLUME_STOPPER ){
		return D_AIC3204_ERROR;
	}
	
	if(gAic3402Info.volume == vol) {
		return D_AIC3204_OK;
	}
	
	gAic3402Info.volume = vol;
	aic3204_write(D_AIC3204_REG_LDACVOL, vol_tbl[gAic3402Info.volume]);	// [0x41] Unmute the DAC digital volume control
	aic3204_write(D_AIC3204_REG_RDACVOL, vol_tbl[gAic3402Info.volume]);	// [0x42] Unmute the DAC digital volume control
	
	return D_AIC3204_OK;
}

/**
 * @brief		AIC3204 volume Change
 * @param[in]	E_AUDIO_VOLUME vol	; Audio volume
 * @param[out]	None
 * @return		int					; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec volume Change
 */
int AIC3204_Set_Mute( E_AIC3204_MUTE mute )
{
	unsigned char dacs2 = 0;
	
	if (!IS_AIC3204_OPEN()){
		return D_AIC3204_ERROR;
	}
	
	if( mute >= E_AIC3204_MUTE_STOPPER ){
		return D_AIC3204_ERROR;
	}
	
	if(mute == E_AIC3204_MUTE_ON) {
		dacs2 = (1 << D_AIC3204_REG_DACS2_LEFT_MUTE_SHIFT) |
				(1 << D_AIC3204_REG_DACS2_RIGHT_MUTE_SHIFT);
		
		aic3204_write(D_AIC3204_REG_DACS2, dacs2);	// [0x40]-[0x0C]Mute the DAC digital volume control
	}
	else if(mute == E_AIC3204_MUTE_OFF) {
		aic3204_write(D_AIC3204_REG_DACS2, 0x00);	// [0x40] Unmute the DAC digital volume control
	}
	else {
		;
	}
	
	return D_AIC3204_OK;
}


/**
 * @brief		AIC3204 Used Flag
 * @param[in]	None
 * @param[out]	None
 * @return		unsigned char		; D_AIC3204_OK, D_AIC3204_ERROR
 * @note		AIC3204 Codec Used Flag
 */
unsigned char AIC3204_Is_Used( void )
{
	return (gAic3402Info.used);
}

//--------------------------------------------------------------------------------------------
