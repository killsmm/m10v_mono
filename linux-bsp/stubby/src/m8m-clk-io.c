/**
 * @file boot.c
 * @author
 * @date 2014/Aug./28
 * @brief 2nd booter for cpu0 and cpu1. cpu1 is stay in sram with WFE-state.
 */
#include "asm/utils.h"
#include "m8m-regs.h"

#include "ddr-MT41K512M8RH-125Ex2.h"

extern int m8m_init_ddr(u32 reg_base, const struct stSDI_CONFIG* pCFG);

static const struct {
	u8 pudcr;
	u8 puder;
	u8 epcr;
	u8 pdr;
	u8 ddr;
} pinctrl[] = {
	{ 0x00,  0x00,  0x0b,  0x00,  0x8b },	// P3
	{ 0x00,  0x00,  0x3C,  0x04,  0x07 },	// P4
	{ 0x00,  0xF0,  0x0B,  0x00,  0x18 },	// P5
	{ 0x00,  0x40,  0x01,  0x0F,  0x8F },	// P6
	{ 0x00,  0x06,  0x08,  0x00,  0x08 },	// P7
	{ 0x00,  0x02,  0x28,  0x01,  0x29 },	// P8
	{ 0x00,  0x0E,  0x10,  0x01,  0x11 },	// P9
	{ 0x00,  0x20,  0xA4,  0x13,  0x97 },	// PA
	{ 0x01,  0xF5,  0xF8,  0x00,  0x0C },	// PC
	{ 0x00,  0x00,  0xFF,  0x00,  0xFF },	// PD
	{ 0x00,  0x00,  0xFF,  0x00,  0xFF },	// PE
	{ 0x00,  0x00,  0xFF,  0x00,  0xFF },	// PF
	{ 0x00,  0x00,  0x3F,  0x00,  0x3F },	// PG
	{ 0x00,  0x00,  0x12,  0x00,  0x12 },	// PP
	{ 0x00,  0xE3,  0x00,  0x1C,  0x1C },	// PR
	{ 0x00,  0x00,  0x7C,  0x20,  0xF4 },	// PS
	{ 0x00,  0x00,  0x07,  0x00,  0x07 },	// PT
	{ 0x00,  0xE0,  0x01,  0x10,  0x00 }	// PU
};

void init_io(void)
{
	u32 reg, i;

	for (i = 0; i < ARRAY_SIZE(pinctrl); i++) {
		reg = readb(pPUDCR + (i << 2)) ^ pinctrl[i].pudcr;
		if (reg) {
			writeb(readb(pPUDER + (i << 2)) & (~reg),
			       pPUDER + (i << 2));
			writeb(pinctrl[i].pudcr, pPUDCR + (i << 2));
		}
		writeb(pinctrl[i].puder, pPUDER + (i << 2));
		writeb(pinctrl[i].epcr, pEPCR + (i << 2));
		writeb(pinctrl[i].pdr, pPDR_S + (i << 2));
		writeb(~pinctrl[i].pdr, pPDR_C + (i << 2));
		writeb(pinctrl[i].ddr, pDDR + (i << 2));
	}

	// hack to let AK4642 out of reset while no gpio driver in kernel
	writeb(0x10, pPDR_S + (2 << 2));
}

void start_ptimer( uint32_t cnt )
{ 
	uint32_t val ;

	writel(cnt, PTIMER + 4);
	val = readl(PTIMER + 8);
	val &= ~(0xFF07); // Prescaler=0, Interrupt Disable, Single Shot, Counter Decrement Disable
	writel(val, PTIMER + 8);
	writel(val | 1, PTIMER + 8);
}

void check_ptimer( void )
{
	while (readl(PTIMER + 4))
		;
	writel(0, PTIMER + 8);
	writel(1, PTIMER + 0xc);
}

void init_clock(void)
{
	// PLL

	// CLKSEL
	writel(0
//		| 0x39 << 26		// APCLKO(72MHz(864/12)) ... CLKSTOPS_D::APCKSをセットして停止してから変更
		| 0x03 << 24		// NFCLK(756MHz,1512/2)
		| 0x0C << 20		// SD0CLK(594MHz)
//		| 0x04 << 16		// SD1U1CLK(2000MHz/10)
//		| 0x1C << 10		// SD1U2CLK(1512/58)
//		| 0x04 << 7		// SENCLK(30.86MHz,864/28)
		| 0x07 << 4
		, IOADR_CLKSEL_A);	// RCLK(54MHz,864/16)

	writel(0
		| 0x10 << 0 		// CPUCLK (XCLK / 1)
		| 0x11 << 8 		// L2CLK (XCLK / 2)
		| 0x13 << 16 		// PERICLK (XCLK / 4)
		, IOADR_CLKSEL_E);	// CLKSEL_G レジスタの DCHREQ ビットに 1を書いて反映

	writel(0
		| 0x11 << 0 		// EAXICLK (200MHz, XCLK / 2)
		| 0x11 << 8 		// AXICLK (200MHz, XCLK / 2)
		| 0x13 << 16		// AHBCLK (100MHz, XCLK / 4)
		| 0x17 << 24 		// APBCLK ( 50MHz, XCLK / 8)
		, IOADR_CLKSEL_F);	// CLKSEL_G レジスタの DCHREQ ビットに 1を書いて反映
	writel(1, IOADR_CLKSEL_G);
	while ((readl(IOADR_CLKSEL_G) & 1))
		;


// DDR-PLL (ch.1 and ch1.2)
	writel(readl(IOADR_PLLCNTL2) | 0x28, IOADR_PLLCNTL2 );	//DDRPLLn出力停止（OC=1)
	writel(readl(IOADR_PLLCNTL2) | 0x3C, IOADR_PLLCNTL2 );	//発振停止(ST=1)
	writel((readl(IOADR_PLLCNTL2) & 0xFF) | 0x60606000,
				IOADR_PLLCNTL2 ); // 1320MHz設定
	writel(readl(IOADR_PLLCNTL2) & 0xFFFFFFE8, IOADR_PLLCNTL2 );	// 発振開始(ST=0)
	start_ptimer(42000);						// PrivateTimer Counter(200us〜 wait)
	check_ptimer();
	writel(readl(IOADR_PLLCNTL2) & 0xFFFFFFC0, IOADR_PLLCNTL2 );	// 出力開始(OC=0)

	// Clock gate : Must stop target module.
	writel(0x0C8C4C0A, IOADR_CLKSTOPC_X(0) ) ;	// CLKSTOPc_A
	writel(0x00700C30, IOADR_CLKSTOPS_X(0) ) ;	// CLKSTOPs_A
	writel(0x05000393, IOADR_CLKSTOPC_X(1) ) ;	// CLKSTOPc_B
	writel(0x00A4B000, IOADR_CLKSTOPS_X(1) ) ;	// CLKSTOPs_B
	writel(0xB0D1E6FF, IOADR_CLKSTOPS_X(2) ) ;	// CLKSTOPs_C
	writel(0xBFBD0F0A, IOADR_CLKSTOPS_X(3) ) ;	// CLKSTOPs_D
	writel(0x33370470, IOADR_CLKSTOPC_X(4) ) ;	// CLKSTOPc_E is osctilation all
	writel(0x0000DDBA, IOADR_CLKSTOPS_X(5) ) ;	// CLKSTOPs_F
	writel(0x00000001, IOADR_CLKSTOPC_X(5) ) ;	// CLKSTOPc_F

	/* start the 86 pll (I2S IPs all held in reset) */
	writel(readl(IOADR_PLLCNTL) & ~(1 << 2), IOADR_PLLCNTL);

	start_ptimer(42000);	// PrivateTimer Counter(200us〜 wait)
	check_ptimer();

	/* let the 86 PLL clock output */
	writel(readl(IOADR_PLLCNTL) & ~(1 << 3), IOADR_PLLCNTL);

	/* start the audio pll (I2S IPs all held in reset) */
	writel(readl(IOADR_PLLCNTL) & ~(1 << 6), IOADR_PLLCNTL);

	start_ptimer(42000);	// PrivateTimer Counter(200us〜 wait)
	check_ptimer();

	/* let the Audio PLL clock output */
	writel(readl(IOADR_PLLCNTL) & ~(1 << 7), IOADR_PLLCNTL);

	/* AUCLKSEL0 from PLL */
	writel((2 << 30) | (2 << 28) | (2 << 26) | (2 << 24), IOADR_CLKSEL_H);

	/* APLCLK from PLL */
	writel(6 << 25, IOADR_CLKSEL_B);
}

void m8m_setup_soc(void)
{
	/* GPV Region Control Register */
	writel(0xff /*0x33*/, 0x29000000);
	/* Remap SRAM to 0x0 */
	writel(1, 0xf8000000);

	/* Mysterious master 14: allow reads */
//	writel(1, 0x2901f008);
//	writel(1, 0x2901f108);
//	writel(1, 0x2904e108);
//	writel(3, 0x290c6008);
//	writel(3, 0x290c6108);

	/* "Address filtering" */

#ifndef CONFIG_RTOS_OWNS_IPS
	/* sd 0 */
	writel(readl(0x2b11000c) &~(1 << 8), 0x2b11000c);
	writel(readl(0x2b110040) &~(1 << 0), 0x2b110040);
	writel((readl(0x18000020) & ~0xf) | 5, 0x18000020);

	init_clock();
	init_io();
	m8m_init_ddr(IOADR_BASE_SDI1, &pSDICFIG[0]);
	m8m_init_ddr(IOADR_BASE_SDI2, &pSDICFIG[1]);
#endif
}

