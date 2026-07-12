#include "asm/utils.h"
#include "m8m-regs.h"

int m8m_init_ddr(u32 reg_base, const struct stSDI_CONFIG* pCFG)
{
	// writel(1, IOADR_SDITOPRST1(reg_base));	// software reset(POR default)
	// wait a moment?
	writel(0, IOADR_SDITOPRST1(reg_base));	// start module

	writel(pCFG->tops_cnf1, IOADR_SDITOPSCNF1(reg_base));	// ch1,ch2ともにx16接続. (ch2はx8の２パラ)
	writel(pCFG->tops_cnf2, IOADR_SDITOPSCNF2(reg_base));	// ch1は 1で良い(電気代が高いだけ)
	writel(pCFG->tops_cnf3, IOADR_SDITOPSCNF3(reg_base));	// ch1は 1で良い(電気代が高いだけ)
//  clock
	writel(1, IOADR_SDICLKEN2(reg_base));	// コアクロック生成開始
	writel(1, IOADR_SDICLKEN3(reg_base));	// PHYへクロック供給
	writel(1, IOADR_SDICLKEN4(reg_base));	// SDRAM I/F制御部へクロック供給
	writel(1, IOADR_SDICLKEN5(reg_base));	// アービトレーション部へクロック供給
	writel(0, IOADR_SDICLKEN6(reg_base));	// アクセスモニタ部へクロック供給停止

// Phy設定
// start: DDR-IF_DDRL-11_Application_Node_Rev1_00.pdf
	writel(pCFG->ddrl_mr0, IOADR_DDRL_MRS0(reg_base));
	writel(pCFG->ddrl_mr1, IOADR_DDRL_MRS1(reg_base));
	writel(pCFG->ddrl_mr2, IOADR_DDRL_MRS2(reg_base));
	writel(				0, IOADR_DDRL_MRS3(reg_base));
	writel(pCFG->ddrl_mrswt, IOADR_DDRL_MRSWT(reg_base));
	writel(pCFG->ddrl_actwt, IOADR_DDRL_ACTWT(reg_base));
	writel(pCFG->ddrl_prewt, IOADR_DDRL_PREWT(reg_base));
	writel(pCFG->ddrl_writewt, IOADR_DDRL_WRITEWT(reg_base));
	writel(pCFG->ddrl_readwt, IOADR_DDRL_READWT(reg_base));

	writel(0x0003020C, IOADR_DDRL_PHYMODE(reg_base));	// Change if ODT-OFF mode.
	writel(0x00000012, IOADR_DDRL_DLLMODE1(reg_base));

	writel(pCFG->ddrl_ddris1,	IOADR_DDRL_DDRIS1(reg_base));
	writel(pCFG->ddrl_pzqcm,	IOADR_DDRL_PZQCM(reg_base));
	writel(pCFG->ddrl_ddriocm,	IOADR_DDRL_DDRIOCM(reg_base));
	writel(pCFG->ddrl_ctrldspc,	IOADR_DDRL_CTRLDSPC(reg_base));
	writel(pCFG->ddrl_ddris2,	IOADR_DDRL_DDRIS2(reg_base));
	writel(	0x00300036, IOADR_DDRL_DLLMODE3(reg_base));

	writel(pCFG->ddrl_trms,	IOADR_DDRL_TRMS(reg_base));
	writel(pCFG->ddrl_pblcsw0, IOADR_DDRL_PBLCSW0(reg_base));
	writel(pCFG->ddrl_pblcsw1, IOADR_DDRL_PBLCSW1(reg_base));
	writel(pCFG->ddrl_pblcsg0, IOADR_DDRL_PBLCSG0(reg_base));
	writel(pCFG->ddrl_pblcsg1, IOADR_DDRL_PBLCSG1(reg_base));
	writel(pCFG->ddrl_pblcsr0, IOADR_DDRL_PBLCSR0(reg_base));
	writel(pCFG->ddrl_pblcsr1, IOADR_DDRL_PBLCSR1(reg_base));
// delay値(設定不要,削除)



//////////////////////
	writel(	1, IOADR_DDRL_SDRAMM(reg_base));
	writel(	1<<0	// ACT_RANK[3:0]
			|	0<<8	// ADR_MIR_EN[3:0]
			, IOADR_DDRL_RANKSEL(reg_base));
	writel(	1	/* (0:ODTOFF of DDR-SDRAM ,1:ODTON of DDR-SDRAM) */
			, IOADR_DDRL_NAODTS(reg_base));
	writel(	1	/* (0:ODTOFF of DDR-SDRAM ,1:ODTON of DDR-SDRAM) */
			, IOADR_DDRL_WLOS(reg_base));
	//[ToDo] PCB設計ガイドライン
	writel(1 /* Rtt_Nom - {0:disable,1:RZQ/4,2:RZQ/2,3:RZQ/6=40} */
			, IOADR_DDRL_WLRS1(reg_base));
	writel(0 /* 初期値(0)でしか使いません */
			, IOADR_DDRL_WLRS2(reg_base));
	//[ToDo] DLL設定ガイド
	writel(	1<<0	// TPAT_SEL_BYTE: {0:noise-worst pattern, 1:easy-pattern}
			|	0<<1	// TPAT_SEL_BIT: {0:noise-worst pattern, 1:easy-pattern}
			, IOADR_DDRL_TTDTPS(reg_base));


// end: DDR-IF_DDRL-11_Application_Node_Rev1_00.pdf
// テスト範囲の指定
	writel(pCFG->ddrl_trwlvl0, IOADR_DDRL_TRWLVL0(reg_base));	// Write Leveling Byte0
	writel(pCFG->ddrl_trwlvl1, IOADR_DDRL_TRWLVL1(reg_base));	// Write Leveling Byte1
	writel(pCFG->ddrl_trgl0, IOADR_DDRL_TRGL0(reg_base));	// Gate Training Byte0
	writel(pCFG->ddrl_trgl1, IOADR_DDRL_TRGL1(reg_base));	// Gate Training Byte1
	writel(pCFG->ddrl_trrds, IOADR_DDRL_TRRDS(reg_base));	// Read Data Synchronizer
	writel(pCFG->ddrl_trrdey0, IOADR_DDRL_TRRDEY0(reg_base));	// Read Data Eye Byte0
	writel(pCFG->ddrl_trrdey1, IOADR_DDRL_TRRDEY1(reg_base));	// Read Data Eye Byte1
	writel(pCFG->ddrl_trwdedqs0, IOADR_DDRL_TRWDEDQS0(reg_base));	// Write Data Eye(PerBit) Byte0
	writel(pCFG->ddrl_trwdedqs1, IOADR_DDRL_TRWDEDQS1(reg_base));	// Write Data Eye(PerBit) Byte1
	writel(pCFG->ddrl_trwdepb0, IOADR_DDRL_TRWDEPB0(reg_base));	// Write Data Eye(PerBit) Byte0
	writel(pCFG->ddrl_trwdepb1, IOADR_DDRL_TRWDEPB1(reg_base));	// Write Data Eye(PerBit) Byte1
	writel(pCFG->ddrl_trrdepb0, IOADR_DDRL_TRRDEPB0(reg_base));	// Read Data Eye(PerBit) Byte0
	writel(pCFG->ddrl_trrdepb1, IOADR_DDRL_TRRDEPB1(reg_base));	// Read Data Eye(PerBit) Byte1

// PHY I/F制御部起動
	writel(0, IOADR_SDIPHYRST1(reg_base));	// reset release
	writel(0, IOADR_SDIPHYINTE1(reg_base));	// interrupt disable
	writel(1, IOADR_SDIPHYINTF1(reg_base));	// boot-complete flag clear
	writel(pCFG->phy_ac1, IOADR_SDIPHYAC1(reg_base));
	writel(pCFG->phy_ac2, IOADR_SDIPHYAC2(reg_base));
	writel(pCFG->phy_ac3, IOADR_SDIPHYAC3(reg_base));
	writel(pCFG->phy_ac4, IOADR_SDIPHYAC4(reg_base));
	writel(pCFG->phy_ac5, IOADR_SDIPHYAC5(reg_base));
	writel(pCFG->phy_ac6, IOADR_SDIPHYAC6(reg_base));
	writel(pCFG->phy_cal1, IOADR_SDIPHYCAL1(reg_base));

	writel(1, IOADR_SDIPHYCTL1(reg_base));	// 起動要求
	while((readl(IOADR_SDIPHYINTF1(reg_base)) & 1) == 0)
		;	// 100uSec程度を要する。
	// [ToDo] timeout?


// SDRAM I/F制御部起動 "4.4.3.1. SDRAM I/F 制御部起動手順"
	writel(1, IOADR_SDIPHYINTF1(reg_base));	// boot-complete flag clear
	writel(0, IOADR_SDIDDRRST1(reg_base));	// リセット解除
	writel(0, IOADR_SDIDDRINTE7(reg_base));	// trainig finish interrupt disable
	// SDRAM MR設定
	writel(pCFG->ddr_mr0, IOADR_SDIDDRMR0(reg_base));
	writel(pCFG->ddr_mr1, IOADR_SDIDDRMR1(reg_base));
	writel(pCFG->ddr_mr2, IOADR_SDIDDRMR2(reg_base));
	writel(	0, IOADR_SDIDDRMR3(reg_base));

	// SDRAM構成設定  ... MT41K256M16-125
	writel(pCFG->ddr_cnf1, IOADR_SDIDDRCNF(reg_base));


	// SDRAM AC設定
	writel(pCFG->ddr_ac1, IOADR_SDIDDRAC1(reg_base));
	writel(pCFG->ddr_ac2, IOADR_SDIDDRAC2(reg_base));
	writel(pCFG->ddr_ac3, IOADR_SDIDDRAC3(reg_base));
	writel(pCFG->ddr_ac4, IOADR_SDIDDRAC4(reg_base));
	writel(pCFG->ddr_ac5, IOADR_SDIDDRAC5(reg_base));
	writel(pCFG->ddr_ac6, IOADR_SDIDDRAC6(reg_base));
	writel(pCFG->ddr_ac7, IOADR_SDIDDRAC7(reg_base));
	writel(pCFG->ddr_ac8, IOADR_SDIDDRAC8(reg_base));
	writel(pCFG->ddr_ac9, IOADR_SDIDDRAC9(reg_base));
	writel(pCFG->ddr_ac10, IOADR_SDIDDRACA(reg_base));	//SDIDDRAC-A
	writel(pCFG->ddr_ac11, IOADR_SDIDDRACB(reg_base));	//SDIDDRAC-B
	writel(pCFG->ddr_ac12, IOADR_SDIDDRACC(reg_base));	//SDIDDRAC-C
	writel(pCFG->ddr_ac13, IOADR_SDIDDRACD(reg_base));	//SDIDDRAC-D
	// PHY機能設定★クレクレ★
	writel(pCFG->ddr_phy1, IOADR_SDIDDRPHY1(reg_base));	//SDIDDRPHY-1
	writel(pCFG->ddr_phy2, IOADR_SDIDDRPHY2(reg_base));	//SDIDDRPHY-2
	writel(pCFG->ddr_phy3, IOADR_SDIDDRPHY3(reg_base));	//SDIDDRPHY-2

	// SDRAM AR設定
	writel(pCFG->ddr_ar1, IOADR_SDIDDRAR1(reg_base));	// SDIDDRAR1
		// SDIDDRAR2 は DDR3未使用

	// SDRAM ZQ設定  //[ToDo] "Micron AN. TN-41-02"を参照して導出する。環境依存。
	writel(pCFG->ddr_zq1, IOADR_SDIDDRZQ1(reg_base));
	writel(pCFG->ddr_zq2, IOADR_SDIDDRZQ2(reg_base));

	// SDRAM CMD設定
	writel(pCFG->ddr_cmd1, IOADR_SDIDDRCMD1(reg_base));	// BANKNO: bank開放数
	writel(pCFG->ddr_cmd2, IOADR_SDIDDRCMD2(reg_base));	// PREMD:


	writel(1, IOADR_SDIDDRINTF1(reg_base));	//要因フラグクリア
	writel(1, IOADR_SDIDDRCTL1(reg_base));	// 起動要求
	while((readl(IOADR_SDIDDRINTF1(reg_base)) & 1) == 0)
		;	// 100uSec程度を要する。
	// [ToDo] timeout?

	// Write Leveling - training
	writel(1, IOADR_SDIDDRINTF7(reg_base));	// 要因フラグクリア
	writel(0 << 16 | 1, IOADR_SDIDDRCTL7(reg_base));	
	while((readl(IOADR_SDIDDRINTF7(reg_base)) & 1) == 0)
		;
	
	if (readl(IOADR_DDRL_TRAINERR(reg_base)) != 0 ) {
		puts("[Error] DDR: Cal.Write Leveling error\r\n");
		return 1;
	}

	// Gate Training
	writel(1, IOADR_SDIDDRINTF7(reg_base));	// 要因フラグクリア
	writel(1 << 16 | 1, IOADR_SDIDDRCTL7(reg_base));	
	while((readl(IOADR_SDIDDRINTF7(reg_base)) & 1) == 0)
		;
	
	if (readl(IOADR_DDRL_TRAINERR(reg_base)) != 0 ) {
		puts("[Error] DDR: Cal. Write Leveling error\r\n");
		return 2;
	}

	// Read Data Synchronizer
	writel(1, IOADR_SDIDDRINTF7(reg_base));	// 要因フラグクリア
	writel(2 << 16 | 1, IOADR_SDIDDRCTL7(reg_base));	
	while((readl(IOADR_SDIDDRINTF7(reg_base)) & 1) == 0)
		;
	
	if (readl(IOADR_DDRL_TRAINERR(reg_base)) != 0 ) {
		puts("[Error] DDR: Cal. Read Data Synchronizer error\r\n");
		return 3;
	}

	// Read Data Eye Training
	writel(1, IOADR_SDIDDRINTF7(reg_base));	// 要因フラグクリア
	writel(3 << 16 | 1, IOADR_SDIDDRCTL7(reg_base));	
	while((readl(IOADR_SDIDDRINTF7(reg_base)) & 1) == 0)
		;
	
	if (readl(IOADR_DDRL_TRAINERR(reg_base)) != 0 ) {
		puts("[Error] DDR: Cal. Read Data Eye Training error\r\n");
		return 4;
	}

	// Write Data Eye Training
	writel(1, IOADR_SDIDDRINTF7(reg_base));	// 要因フラグクリア
	writel(4 << 16 | 1, IOADR_SDIDDRCTL7(reg_base));	
	while((readl(IOADR_SDIDDRINTF7(reg_base)) & 1) == 0)
		;
	
	if (readl(IOADR_DDRL_TRAINERR(reg_base)) != 0 ) {
		puts("[Error] DDR: Cal. Write Data Eye Training error\r\n");
		return 5;
	}


// アービトレーション制御部起動
	writel(0, IOADR_SDIARBRST(reg_base));	// release module reset
	writel(0, IOADR_SDIARBINTE1(reg_base));
	writel(0, IOADR_SDIARBCTL1(reg_base));	// {0:"AXI0 > others", 1:"AXI0:AXI1 > others"}
	writel(	 0 << 0	/* AXI0 */
			|	 0 << 4	/* AXI1 */
			|	 0 << 8	/* AXI2 */
			|	 0 << 16	/* PRRBR0 */
			|	 0 << 20	/* PRRBR1 */
			, IOADR_SDIARBCTL2(reg_base));

	writel(1, IOADR_SDIARBINTF1(reg_base));	// reset flag
	writel(1, IOADR_SDIARBCTL1(reg_base));	// 起動
	while((readl(IOADR_SDIARBINTF1(reg_base)) & 1) == 0)
		;
	writel(1, IOADR_SDIARBINTF1(reg_base));	// reset flag

	// AXI[0] I/F制御部
	writel(1, IOADR_SDICLKEN9(reg_base));	// AXI0 I/F部へクロック供給
	writel(0, IOADR_SDIAXI0RST1(reg_base));	// release module-reset
	writel(0, IOADR_SDIAXI0INTE1(reg_base));
	writel(	0 << 0	/* RPR */
			|	0 << 16	/* WPR */
			, IOADR_SDIAXI0ARB1(reg_base));
	writel(1, IOADR_SDIAXI0INTF1(reg_base));	// reset flag
	writel(1, IOADR_SDIAXI0CTL1(reg_base));	// 起動
	while((readl(IOADR_SDIAXI0INTF1(reg_base)) & 1) == 0)
		;
	writel(1, IOADR_SDIAXI0INTF1(reg_base));	// reset flag

	// AXI[1] I/F制御部
	writel(1, IOADR_SDICLKEN10(reg_base));	// AXI1 I/F部へクロック供給
	writel(0, IOADR_SDIAXI1RST1(reg_base));	// release module-reset
	writel(0, IOADR_SDIAXI1INTE1(reg_base));
	writel(	0 << 0	/* RPR */
			|	0 << 16	/* WPR */
			, IOADR_SDIAXI1ARB1(reg_base));
	writel(1, IOADR_SDIAXI1INTF1(reg_base));	// reset flag
	writel(1, IOADR_SDIAXI1CTL1(reg_base));	// 起動
	while((readl(IOADR_SDIAXI1INTF1(reg_base)) & 1) == 0)
		;
	writel(1, IOADR_SDIAXI1INTF1(reg_base));	// reset flag

	// AXI[2] I/F制御部
	writel(1, IOADR_SDICLKEN11(reg_base));	// AXI2 I/F部へクロック供給
	writel(0, IOADR_SDIAXI2RST1(reg_base));	// release module-reset
	writel(0, IOADR_SDIAXI2INTE1(reg_base));
	writel(	0 << 0	/* RPR */
			|	0 << 16	/* WPR */
			, IOADR_SDIAXI2ARB1(reg_base));
	writel(1, IOADR_SDIAXI2INTF1(reg_base));	// reset flag
	writel(1, IOADR_SDIAXI2CTL1(reg_base));	// 起動
	while((readl(IOADR_SDIAXI2INTF1(reg_base)) & 1) == 0)
		;
	writel(1, IOADR_SDIAXI2INTF1(reg_base));	// reset flag


	// Ribery I/F制御部[0]
	writel(1, IOADR_SDICLKEN13(reg_base));	// Ribery0 I/F部へクロック供給
	writel(0, IOADR_SDIRBR0RST1(reg_base));	// release module-reset
	writel(0, IOADR_SDIRBR0INTE1(reg_base));
		// No-configuration registers...
	writel(1, IOADR_SDIRBR0INTF1(reg_base));	// reset flag
	writel(1, IOADR_SDIRBR0CTL1(reg_base));	// 起動
	while((readl(IOADR_SDIRBR0INTF1(reg_base)) & 1) == 0)
		;
	writel(1, IOADR_SDIRBR0INTF1(reg_base));	// reset flag

	// Ribery I/F制御部[1]
	writel(1, IOADR_SDICLKEN14(reg_base));	// Ribery1 I/F部へクロック供給
	writel(0, IOADR_SDIRBR1RST1(reg_base));	// release module-reset
	writel(0, IOADR_SDIRBR1INTE1(reg_base));
		// No-configuration registers...
	writel(1, IOADR_SDIRBR1INTF1(reg_base));	// reset flag
	writel(1, IOADR_SDIRBR1CTL1(reg_base));	// 起動
	while((readl(IOADR_SDIRBR1INTF1(reg_base)) & 1) == 0)
		;
	writel(1, IOADR_SDIRBR1INTF1(reg_base));	// reset flag


#if 0	/* アクセスモニタは寝かせておく */
	// アクセスモニタ制御部
	writel(0, IOADR_SDIMONRST1(reg_base));	// release module-reset

	writel(1, IOADR_SDIMONINTF1(reg_base));	// reset flag
	writel(1, IOADR_SDIMONINTE1(reg_base));	// 起動
	while((readl(IOADR_SDIMONINTF1(reg_base)) & 1) == 0)
		;
#endif /* アクセスモニタは寝かせておく */

return 0;
}


