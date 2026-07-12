#define RU(v, d)	(((v)+(d)-1)/(d))
#define RD(v, d)	((v)/(d))

static const struct stSDI_CONFIG pSDICFIG[2] = {
	{ // DDR-ch.1: MT41K512M8RH-125:E x 2

		// 全体制御 - SDRAM構成設定
		.tops_cnf1 = 2,		/* 2:"x16接続", 3:"x32接続" (ch2はx8の２パラ) */
		.tops_cnf2 = 3,		/* DDRCLK[1][0] CLK端子 output-(1:enable,0:disable) */
		.tops_cnf3 =  3 << 0 |	/* CKE[1][0]    output-(1:enable,0:disable) */
			      3 << 8 |	/* DDRCSX[1][0] output-(1:enable,0:disable) */
			      3 << 16 ,	/* DDRODT[1][0] output-(1:enable,0:disable) */

		// PHY I/F制御部
		// AC - 設定値指定@"JDSSDI11A_Rev1.2"
		.phy_ac1 = 4,		/* AC1 fixed.value:  */
		.phy_ac2 = 17,		/* AC2 fixed.value:  */
		.phy_ac3 = 9859,	/* AC3 (CLK freq.依存/800M=8077,667M=9859) */
		.phy_ac4 = 15,		/* AC4 fixed.value:  */
		.phy_ac5 = 9,		/* AC5 fixed.value:  */
		.phy_ac6 = 19637,	/* AC6 (CLK freq.依存/800M=21819,667M=19637) */
		// Cal.
		.phy_cal1 = 15,		/* CAL1 (CLK freq.依存/800M以下:15)*/

		// SDRAM I/F制御部 ... CAUTION: MRxは 手動で設定してね! テーブル引いてもいいけどチェックが面倒
		.ddr_mr0 = (	0 << 0		// BL=8(val=0)
					|	5 << 4		// CL=9(val=CL-4)
					|	5 << 9		// WR=gretor than 15nSec. tCK=1.5nSec=> 10cyc
					|	0 << 12),	// PD=1, Precharge PD: as DLL on(Fast Exit mode),
		.ddr_mr1 = (	0x0000		// drive strength 2'bxx := bit[5],[1] {00:40ohm(RZQ/6), 01:34ohm(RZQ/7)}
					|	0x0004),		// RTT[3] [9][6][2] {000:ODT-off,_001:60ohm_,010:120ohm,011:40ohm}
		.ddr_mr2 = (	2 << 3),	// CWL=7, {3'b000:5, 3'b001:6, 3'b010:7, 3'b011:8}	// Speed Bins

		// Config.
		.ddr_cnf1 = (	0 << 0
					|	1 << 8		// Bank ADR 3bits{0:2bit, 1:3bit}
					|	4 << 16		// Row ADR 16bits, 12+n bits(n=0..4)
					|	2 << 24),	// Col ADR 10bits, 8+n bits(n=0..3)


		// AC設定
		.ddr_ac1 = (RU(500000000,DDRCFG1_tCK*2) << 0),	// RoundUp(500uSec / tCK / 2)
		.ddr_ac2 = ((RU(DDRCFG1_tXPR,DDRCFG1_tCK*2)-1) << 0),	// RoundUp( tXPR / tCK / 2 ) -1 
		.ddr_ac3 = ((RU(DDRCFG1_tDLLK,DDRCFG1_tCK*2)-1) << 0),		// RoundUp( tDLLK[cyc] / 2 ) -1
		.ddr_ac4 = ((RU(DDRCFG1_tZQinit,DDRCFG1_tCK*2)-1) << 0),		// RoundUp( tZQinit[cyc] / 2 ) -1
		.ddr_ac5 = (	(RU(DDRCFG1_tMRD,DDRCFG1_tCK*2)-1) << 0		// TMRD:  RoundUp( tMRD[cyc] / 2 ) -1
					|	(RU(DDRCFG1_tMOD,DDRCFG1_tCK*2)-1) << 8		// TMOD:  RoundUp( tMOD[cyc] / 2 ) -1
					|	(RU(DDRCFG1_tMRSPDEN,DDRCFG1_tCK*2)-1) << 16),		// TMTPD: RoundUp( tMRSPDEN[cyc] / 2 ) -1
		.ddr_ac6 = (	(RU(DDRCFG1_tRCD,DDRCFG1_tCK*2)-1) << 0		// TRCD:  RoundUp( tRCD[cyc] / 2 ) -1
					|	(RU(DDRCFG1_tRAS,DDRCFG1_tCK*2)-1) << 8		// TRAS:  RounUp( tRAS / tCK / 2 ) -1
					|	(RU(DDRCFG1_tRRD,DDRCFG1_tCK*2)-1) << 16		// TRRD:  RounUp( tRRD / tCK / 2 ) -1
					|	(RU(DDRCFG1_tFAW,DDRCFG1_tCK*2)-1) << 24),	// TFAW:  RounUp( tFAW / tCK / 2 ) -1
		.ddr_ac7 = (	(RU(DDRCFG1_tRP,DDRCFG1_tCK*2)-1) << 0		// TRP:   RoundUp( tRP[cyc] / 2 ) -1
					|	(RU(DDRCFG1_tPRPDEN,DDRCFG1_tCK*2)-1) << 16),		// TPTPD: RounUp( tPRPDEN[cyc] / 2 ) -1
		.ddr_ac8 = (	(RU(DDRCFG1_CWL_cyc*2+DDRCFG1_BL_cyc+DDRCFG1_tWTR_cyc*2,4)-1) << 0
						// TWTR:  RoundUp( (CWL+BL/2+tWTR サイクル数) / 2 ) -1
					|	(RU(DDRCFG1_CWL_cyc*2+DDRCFG1_BL_cyc+DDRCFG1_tWR_cyc*2,4)-1) << 8
						// TWTP:  RoundUp( (CWL+BL/2+tWR サイクル数) / 2 ) -1,
					|	(RU(DDRCFG1_CWL_cyc*2+DDRCFG1_BL_cyc+DDRCFG1_tWR_cyc*2,4)-1) << 16
						// TWTPD: RoundUp( (CWL+BL/2+tWR サイクル数) / 2 ) -1,
					|	(RU(DDRCFG1_BL_cyc+4,4)-1) << 24),	// TWTW:  RoundUp( (BL/2 + 2) /2 ) -1,
		.ddr_ac9 = (	(RU((DDRCFG1_CL_cyc-DDRCFG1_CWL_cyc+6)*2+DDRCFG1_BL_cyc,4)-1) << 0
						// TRTW:  RoundUp( (CL-CWL+BL/2+6) / 2) -1, 11-8+8/2+6, =>6
					|	 (RU(DDRCFG1_tRTP_cyc,2)-1) << 8		// TRTP:  RoundUp( (tRTP サイクル数) / 2 ) -1
					|	 (RU((DDRCFG1_CL_cyc+1)*2+DDRCFG1_BL_cyc,4)-1) << 16	// TRTPD: RoundUp( (CL+BL/2+1) / 2) -1
					|	 0 << 24),	// - : DDR3では未使用
		.ddr_ac10 = (	(RU(DDRCFG1_tRFC_cyc,2)-1) << 0	// TRFC:  RoundUp( tRFC / tCK / 2) -1
					|	(RU(DDRCFG1_tXPDLL_cyc,2) - RU(DDRCFG1_tXP_cyc,2) -1) << 16),
						// TATPD: RoundUp(tXPDLL サイクル数 / 2) - RU(tXP サイクル数 / 2) - 1
		.ddr_ac11 = 0x007F001F, /* 初回配送分の不良. reset直後の初期値で良い. */
		.ddr_ac12 = 0x00010002,	/* slowExitを使うので 2にしている. */
/*
		.ddr_ac11 = (	(RU(MAX(DDRCFG1_tZQCS_cyc,DDRCFG1_tZQoper_cyc),2)-1) << 0			// TZQCS:  RoundUp(tZQCS サイクル数 / 2) -1
					|	(RU(DDRCFG1_tZQoper_cyc,2)-1) << 16),	// TZQCL:  RoundUp(tZQoper サイクル数 / 2) -1
		.ddr_ac12 = (	(RU(DDRCFG1_tXP_cyc,2)-1) << 0		// TXP:  RoundUp(tXP サイクル数 / 2) - 1, 
					|	(RU(DDRCFG1_tCKE_cyc,2)-1) << 16),		// TCKE: RoundUp(tCKE サイクル数 / 2) - 1
*/
		.ddr_ac13 = (	(RU(DDRCFG1_tXSDLL_cyc,2)-1) << 0	// TXSR:   RoundUp(tXSDLL サイクル数 / 2) - 1
					|	(RU(DDRCFG1_tCKESR_cyc,2)-1) << 16),		// TCKESR: RoundUp(tCKESR サイクル数 / 2) - 1,

		// PHY機能設定
		.ddr_phy1 = (8 << 0),	// GETDLY: 13 + RD((tphy_rdlat + CL)/2) - RU(CWL/2), "9+(それ+11)/2"
								// [ToDo] tphy_rdlat の導出＠DLL設定ガイド
								// 11 + n(設定値), n=5..10
				.ddr_phy2 = (3 << 0),	// PZQMD[1]ZQ-cal.,[0]Auto-refresh {1:cal.する.2:calしない}
		.ddr_phy3 = (0 << 0	// LPMDEN: low power down mode{0:不許可,1:許可}
				|	9 << 16),	// DDR3:固定値

		// AR設定
		.ddr_ar1 = (RU(DDRCFG1_tREFI_cyc,2) << 0),	// REFCNT: RoundUp( tREFI[cyc] / 2 ), Tc<=85度なら 7.8uSec, 6240CKs.
								// この周期はaverage. 64mSec/8192回, RefCMDの開きは最悪70uSec以下.
		.ddr_ar2 = 0x00000000,
		// ZQ設定
		.ddr_zq1 = 0x02849880, // (RU(DDRCFG1_ZQ_calcyc,2)-1),	// ZQCCNT: RoundUp( ZQキャリブレーション間隔[cyc] / 2 ) -1, 
		.ddr_zq2 = (0 << 0),	// ZQCMD: {0:Short(periodic), 1:Long(initial)}
		// CMD設定
		.ddr_cmd1 = (7 << 0),	// BANKNO: bank開放数 {3'b111:8bank, 3'b011:4bank, 3'b001:2bank}
		.ddr_cmd2 = (0 << 0),	// PREMD: {0:Active powerdown mode, 1:Precharge powerdown mode}

		.ddrl_mr0 = (	0 << 0		// BL=8
					|	5 << 4		// CL=9(4'b{[6:4][2] | 1110=>11,1010=>9} 
					|	1 << 8		// DLL reset(1:Yes,0:No)
					|	5 << 9		// WR=gretor than 15nSec. tCK=1.5nSec=> 10cyc
					|	0 << 12),	// PD=1, Precharge PD: as DLL on(Fast Exit mode),
		.ddrl_mr1 = (	0x0000		// drive strength 2'bxx := bit[5],[1] {00:40ohm(RZQ/6), 01:34ohm(RZQ/7)}
					|	0x0004		// RTT[3] [9][6][2] {000:ODT-off,_001:60ohm_,010:120ohm,011:40ohm}
					|	0 << 11),	// TDQS (0:disable, 1:enable)
		.ddrl_mr2 = (	2 << 3),	// CWL=7, {3'b000:5, 3'b001:6, 3'b010:7, 3'b011:8}	// Speed Bins

		.ddrl_mrswt = RU(DDRCFG1_tMOD,DDRCFG1_tCK*2),	// TMOD:  RoundUp( tMOD[ns] / tCK / 2 )
		.ddrl_actwt = RU(DDRCFG1_tRCD,DDRCFG1_tCK*2),	// TRCD:  RoundUp( tRCD[ns] / tCK / 2 )
		.ddrl_prewt = RU(DDRCFG1_tRP,DDRCFG1_tCK*2),	// TRP:   RoundUp( tRP[ns] / tCK / 2 )
		.ddrl_writewt = RU(DDRCFG1_tWR,DDRCFG1_tCK*2)+1,	// TWR:   RoundUp( tWR[ns] / tCK / 2 )+1
		.ddrl_readwt = RU(DDRCFG1_tRTP,DDRCFG1_tCK*2),		// TRTP: Roundup(tRTP[ns] / tCK / 2)

		// 評価ボードのマクロから抽出
		.ddrl_ddris1 = (	DF_SOCDRV_40OHM << 0	// CDRV_CK[2:0]
						|	DF_SOCDRV_60OHM << 8	// CDRV_CA[2:0]
						|	DF_SOCDRV_40OHM << 12	// CDRV_DQS[2:0]
						|	DF_SOCDRV_40OHM << 16	// CDRV_DQ[2:0]
						|				0 << 20	// PODON {0:DDR3L, 1:DDR3}
						|				1 <<24),	// CDRVTM {0:ODTOFF,1:ODTON} of DDR-SDRAM
		.ddrl_pzqcm = (	0x00003013		// fixed. value..
						|	6 << 8),	// PZQCLK_SEL[3:0] {DDRCLK? 2:400, 4:533, 6:667, 7:800}
		.ddrl_ddriocm = (	DF_SOCODT_60OHM << 0	// RTE_DQ[2:0]
						|	DF_SOCODT_60OHM << 4),	// RTE_DQS[2:0]
		// 評価ボードのマクロから抽出
		.ddrl_ctrldspc = 0x25,		//ITXCTRL_SFT[8:0]
		// 評価ボードのマクロから抽出
		.ddrl_ddris2 = (	DF_SOCDRV_48OHM << 0	// CDRV_ODT[2:0]
						|	DF_SOCDRV_48OHM << 4	// CDRV_CS_N
						|	DF_SOCDRV_48OHM << 8	// CDRV_CKE[2:0]
						|	DF_SOCDRV_60OHM << 12),	// CDRV_RESET_N[2:0]
		.ddrl_trms = (	1 << DF_DDRL__RDE_MODE_SFT	//{ 0:Byte, 1:Byte+Bit, 2:Bit(*_DLY_SEL=0のときのみ選択可) }
					|	1 << DF_DDRL__WDE_MODE_SFT
					// *_DLY_SEL <=> {0:traingin result, 1:register value}
					//	| BIT_DDRL__WL_DLY_SEL
					//	| BIT_DDRL__GT_DLY_SEL
						| BIT_DDRL__RDS_DLY_SEL
					//	| BIT_DDRL__RDE_DLY_SEL
					//	| BIT_DDRL__WDE_DLY_SEL
					),
		.ddrl_pblcsw0 = 0x0005C03D,
		.ddrl_pblcsw1 = 0x00057038,
		.ddrl_pblcsg0 = 0x0001804B,
		.ddrl_pblcsg1 = 0x00018046,
		.ddrl_pblcsr0 = 0x00000300,
		.ddrl_pblcsr1 = 0x00000300,
		// DLL設定ガイドを見ないとわからない.
		.ddrl_trwlvl0 = 0x0007D00D,
		.ddrl_trwlvl1 = 0x00078008,
		.ddrl_trgl0 = 0x000000CB,
		.ddrl_trgl1 = 0x000000C6,
		.ddrl_trrds = 0x00000003,
		.ddrl_trrdey0 = 0x00010020,
		.ddrl_trrdey1 = 0x00010020,
		.ddrl_trwdedqs0 = 0x00010020,
		.ddrl_trwdedqs1 = 0x00010020,
		.ddrl_trwdepb0 = 0x0000000F,
		.ddrl_trwdepb1 = 0x0000000F,
		.ddrl_trrdepb0 = 0x0000000F,
		.ddrl_trrdepb1 = 0x0000000F,

	},
	{ // DDR-ch.2: MT41K256M16-125 x 1

		// 全体制御 - SDRAM構成設定
		.tops_cnf1 = 2,		/* 2:"x16接続", 3:"x32接続" (ch2はx8の２パラ) */
		.tops_cnf2 = 1,		/* DDRCLK[1][0] CLK端子 output-(1:enable,0:disable) */
		.tops_cnf3 =  1 << 0	/* CKE[1][0]    output-(1:enable,0:disable) */
					| 1 << 8	/* DDRCSX[1][0] output-(1:enable,0:disable) */
					| 1 << 16 ,	/* DDRODT[1][0] output-(1:enable,0:disable) */

		// PHY I/F制御部
		// AC - 設定値指定@"JDSSDI11A_Rev1.2"
		.phy_ac1 = 4,		/* AC1 fixed.value:  */
		.phy_ac2 = 17,		/* AC2 fixed.value:  */
		.phy_ac3 = 9859,	/* AC3 (CLK freq.依存/800M=8077,667M=9859) */
		.phy_ac4 = 15,		/* AC4 fixed.value:  */
		.phy_ac5 = 9,		/* AC5 fixed.value:  */
		.phy_ac6 = 19637,	/* AC6 (CLK freq.依存/800M=21819,667M=19637) */
		// Cal.
		.phy_cal1 = 15,		/* CAL1 (CLK freq.依存/800M以下:15)*/

		// SDRAM I/F制御部 ... CAUTION: MRxは 手動で設定してね! テーブル引いてもいいけどチェックが面倒
		.ddr_mr0 = (	0 << 0		// BL=8(val=0)
					|	5 << 4		// CL=9(val=CL-4)
					|	5 << 9		// WR=gretor than 15nSec. tCK=1.5nSec=> 10cyc
					|	0 << 12),	// PD=1, Precharge PD: as DLL on(Fast Exit mode),
		.ddr_mr1 = (	0x0000		// drive strength 2'bxx := bit[5],[1] {00:40ohm(RZQ/6), 01:34ohm(RZQ/7)}
					|	0x0004),		// RTT[3] [9][6][2] {000:ODT-off,_001:60ohm_,010:120ohm,011:40ohm}
		.ddr_mr2 = (	2 << 3),	// CWL=7, {3'b000:5, 3'b001:6, 3'b010:7, 3'b011:8}	// Speed Bins

		// Config.
		.ddr_cnf1 = (	0 << 0
					|	1 << 8		// Bank ADR 3bits{0:2bit, 1:3bit}
					|	3 << 16		// Row ADR 16bits, 12+n bits(n=0..4)
					|	2 << 24),	// Col ADR 10bits, 8+n bits(n=0..3)


		// AC設定
		.ddr_ac1 = (RU(500000000,DDRCFG2_tCK*2) << 0),	// RoundUp(500uSec / tCK / 2)
		.ddr_ac2 = ((RU(DDRCFG2_tXPR,DDRCFG2_tCK*2)-1) << 0),	// RoundUp( tXPR / tCK / 2 ) -1 
		.ddr_ac3 = ((RU(DDRCFG2_tDLLK,DDRCFG2_tCK*2)-1) << 0),		// RoundUp( tDLLK[cyc] / 2 ) -1
		.ddr_ac4 = ((RU(DDRCFG2_tZQinit,DDRCFG2_tCK*2)-1) << 0),		// RoundUp( tZQinit[cyc] / 2 ) -1
		.ddr_ac5 = (	(RU(DDRCFG2_tMRD,DDRCFG2_tCK*2)-1) << 0		// TMRD:  RoundUp( tMRD[cyc] / 2 ) -1
					|	(RU(DDRCFG2_tMOD,DDRCFG2_tCK*2)-1) << 8		// TMOD:  RoundUp( tMOD[cyc] / 2 ) -1
					|	(RU(DDRCFG2_tMRSPDEN,DDRCFG2_tCK*2)-1) << 16),		// TMTPD: RoundUp( tMRSPDEN[cyc] / 2 ) -1
		.ddr_ac6 = (	(RU(DDRCFG2_tRCD,DDRCFG2_tCK*2)-1) << 0		// TRCD:  RoundUp( tRCD[cyc] / 2 ) -1
					|	(RU(DDRCFG2_tRAS,DDRCFG2_tCK*2)-1) << 8		// TRAS:  RounUp( tRAS / tCK / 2 ) -1
					|	(RU(DDRCFG2_tRRD,DDRCFG2_tCK*2)-1) << 16		// TRRD:  RounUp( tRRD / tCK / 2 ) -1
					|	(RU(DDRCFG2_tFAW,DDRCFG2_tCK*2)-1) << 24),	// TFAW:  RounUp( tFAW / tCK / 2 ) -1
		.ddr_ac7 = (	(RU(DDRCFG2_tRP,DDRCFG2_tCK*2)-1) << 0		// TRP:   RoundUp( tRP[cyc] / 2 ) -1
					|	(RU(DDRCFG2_tPRPDEN,DDRCFG2_tCK*2)-1) << 16),		// TPTPD: RounUp( tPRPDEN[cyc] / 2 ) -1
		.ddr_ac8 = (	(RU(DDRCFG2_CWL_cyc*2+DDRCFG2_BL_cyc+DDRCFG2_tWTR_cyc*2,4)-1) << 0
						// TWTR:  RoundUp( (CWL+BL/2+tWTR サイクル数) / 2 ) -1
					|	(RU(DDRCFG2_CWL_cyc*2+DDRCFG2_BL_cyc+DDRCFG2_tWR_cyc*2,4)-1) << 8
						// TWTP:  RoundUp( (CWL+BL/2+tWR サイクル数) / 2 ) -1,
					|	(RU(DDRCFG2_CWL_cyc*2+DDRCFG2_BL_cyc+DDRCFG2_tWR_cyc*2,4)-1) << 16
						// TWTPD: RoundUp( (CWL+BL/2+tWR サイクル数) / 2 ) -1,
					|	(RU(DDRCFG2_BL_cyc+4,4)-1) << 24),	// TWTW:  RoundUp( (BL/2 + 2) /2 ) -1,
		.ddr_ac9 = (	(RU((DDRCFG2_CL_cyc-DDRCFG2_CWL_cyc+6)*2+DDRCFG2_BL_cyc,4)-1) << 0
						// TRTW:  RoundUp( (CL-CWL+BL/2+6) / 2) -1, 11-8+8/2+6, =>6
					|	 (RU(DDRCFG2_tRTP_cyc,2)-1) << 8		// TRTP:  RoundUp( (tRTP サイクル数) / 2 ) -1
					|	 (RU((DDRCFG2_CL_cyc+1)*2+DDRCFG2_BL_cyc,4)-1) << 16	// TRTPD: RoundUp( (CL+BL/2+1) / 2) -1
					|	 0 << 24),	// - : DDR3では未使用
		.ddr_ac10 = (	(RU(DDRCFG2_tRFC_cyc,2)-1) << 0	// TRFC:  RoundUp( tRFC / tCK / 2) -1
					|	(RU(DDRCFG2_tXPDLL_cyc,2) - RU(DDRCFG2_tXP_cyc,2) -1) << 16),
						// TATPD: RoundUp(tXPDLL サイクル数 / 2) - RU(tXP サイクル数 / 2) - 1
		.ddr_ac11 = 0x007F001F, /* 初回配送分の不良. reset直後の初期値で良い. */
		.ddr_ac12 = 0x00010002,	/* slowExitを使うので 2にしている. */
/*
		.ddr_ac11 = (	(RU(MAX(DDRCFG1_tZQCS_cyc,DDRCFG1_tZQoper_cyc),2)-1) << 0			// TZQCS:  RoundUp(tZQCS サイクル数 / 2) -1
					|	(RU(DDRCFG1_tZQoper_cyc,2)-1) << 16),	// TZQCL:  RoundUp(tZQoper サイクル数 / 2) -1
		.ddr_ac12 = (	(RU(DDRCFG1_tXP_cyc,2)-1) << 0		// TXP:  RoundUp(tXP サイクル数 / 2) - 1, 
					|	(RU(DDRCFG1_tCKE_cyc,2)-1) << 16),		// TCKE: RoundUp(tCKE サイクル数 / 2) - 1
*/
		.ddr_ac13 = (	(RU(DDRCFG2_tXSDLL_cyc,2)-1) << 0	// TXSR:   RoundUp(tXSDLL サイクル数 / 2) - 1
					|	(RU(DDRCFG2_tCKESR_cyc,2)-1) << 16),		// TCKESR: RoundUp(tCKESR サイクル数 / 2) - 1,

		// PHY機能設定
		.ddr_phy1 = (8 << 0),	// GETDLY: 13 + RD((tphy_rdlat + CL)/2) - RU(CWL/2), "9+(それ+11)/2"
								// [ToDo] tphy_rdlat の導出＠DLL設定ガイド
								// 11 + n(設定値), n=5..10
				.ddr_phy2 = (3 << 0),	// PZQMD[1]ZQ-cal.,[0]Auto-refresh {1:cal.する.2:calしない}
		.ddr_phy3 = (0 << 0	// LPMDEN: low power down mode{0:不許可,1:許可}
				|	9 << 16),	// DDR3:固定値

		// AR設定
		.ddr_ar1 = (RU(DDRCFG2_tREFI_cyc,2) << 0),	// REFCNT: RoundUp( tREFI[cyc] / 2 ), Tc<=85度なら 7.8uSec, 6240CKs.
								// この周期はaverage. 64mSec/8192回, RefCMDの開きは最悪70uSec以下.
		.ddr_ar2 = 0x00000000,
		// ZQ設定
		.ddr_zq1 = 0x02849880, // (RU(DDRCFG2_ZQ_calcyc,2)-1),	// ZQCCNT: RoundUp( ZQキャリブレーション間隔[cyc] / 2 ) -1, 
		.ddr_zq2 = (0 << 0),	// ZQCMD: {0:Short(periodic), 1:Long(initial)}
		// CMD設定
		.ddr_cmd1 = (7 << 0),	// BANKNO: bank開放数 {3'b111:8bank, 3'b011:4bank, 3'b001:2bank}
		.ddr_cmd2 = (0 << 0),	// PREMD: {0:Active powerdown mode, 1:Precharge powerdown mode}

		.ddrl_mr0 = (	0 << 0		// BL=8
					|	5 << 4		// CL=9(4'b{[6:4][2] | 1110=>11,1010=>9} 
					|	1 << 8		// DLL reset(1:Yes,0:No)
					|	5 << 9		// WR=gretor than 15nSec. tCK=1.5nSec=> 10cyc
					|	0 << 12),	// PD=1, Precharge PD: as DLL on(Fast Exit mode),
		.ddrl_mr1 = (	0x0000		// drive strength 2'bxx := bit[5],[1] {00:40ohm(RZQ/6), 01:34ohm(RZQ/7)}
					|	0x0004		// RTT[3] [9][6][2] {000:ODT-off,_001:60ohm_,010:120ohm,011:40ohm}
					|	0 << 11),	// TDQS (0:disable, 1:enable)
		.ddrl_mr2 = (	2 << 3),	// CWL=7, {3'b000:5, 3'b001:6, 3'b010:7, 3'b011:8}	// Speed Bins

		.ddrl_mrswt = RU(DDRCFG2_tMOD,DDRCFG2_tCK*2),	// TMOD:  RoundUp( tMOD[ns] / tCK / 2 )
		.ddrl_actwt = RU(DDRCFG2_tRCD,DDRCFG2_tCK*2),	// TRCD:  RoundUp( tRCD[ns] / tCK / 2 )
		.ddrl_prewt = RU(DDRCFG2_tRP,DDRCFG2_tCK*2),	// TRP:   RoundUp( tRP[ns] / tCK / 2 )
		.ddrl_writewt = RU(DDRCFG2_tWR,DDRCFG2_tCK*2)+1,	// TWR:   RoundUp( tWR[ns] / tCK / 2 )+1
		.ddrl_readwt = RU(DDRCFG2_tRTP,DDRCFG2_tCK*2),		// TRTP: Roundup(tRTP[ns] / tCK / 2)

		// 評価ボードのマクロから抽出
		.ddrl_ddris1 = (	DF_SOCDRV_40OHM << 0	// CDRV_CK[2:0]
						|	DF_SOCDRV_60OHM << 8	// CDRV_CA[2:0]
						|	DF_SOCDRV_40OHM << 12	// CDRV_DQS[2:0]
						|	DF_SOCDRV_40OHM << 16	// CDRV_DQ[2:0]
						|				0 << 20	// PODON {0:DDR3L, 1:DDR3}
						|				1 <<24),	// CDRVTM {0:ODTOFF,1:ODTON} of DDR-SDRAM
		.ddrl_pzqcm = (	0x00003013		// fixed. value..
						|	6 << 8),	// PZQCLK_SEL[3:0] {DDRCLK? 2:400, 4:533, 6:667, 7:800}
		.ddrl_ddriocm = (	DF_SOCODT_60OHM << 0	// RTE_DQ[2:0]
						|	DF_SOCODT_60OHM << 4),	// RTE_DQS[2:0]
		// 評価ボードのマクロから抽出
		.ddrl_ctrldspc = 0x22,		//ITXCTRL_SFT[8:0]
		// 評価ボードのマクロから抽出
		.ddrl_ddris2 = (	DF_SOCDRV_48OHM << 0	// CDRV_ODT[2:0]
						|	DF_SOCDRV_48OHM << 4	// CDRV_CS_N
						|	DF_SOCDRV_48OHM << 8	// CDRV_CKE[2:0]
						|	DF_SOCDRV_60OHM << 12),	// CDRV_RESET_N[2:0]
		.ddrl_trms = (	1 << DF_DDRL__RDE_MODE_SFT	//{ 0:Byte, 1:Byte+Bit, 2:Bit(*_DLY_SEL=0のときのみ選択可) }
					|	1 << DF_DDRL__WDE_MODE_SFT
					// *_DLY_SEL <=> {0:traingin result, 1:register value}
					//	| BIT_DDRL__WL_DLY_SEL
					//	| BIT_DDRL__GT_DLY_SEL
						| BIT_DDRL__RDS_DLY_SEL
					//	| BIT_DDRL__RDE_DLY_SEL
					//	| BIT_DDRL__WDE_DLY_SEL
					),
		.ddrl_pblcsw0 = 0x00054036,
		.ddrl_pblcsw1 = 0x00056038,
		.ddrl_pblcsg0 = 0x00019049,
		.ddrl_pblcsg1 = 0x0001904B,
		.ddrl_pblcsr0 = 0x00000300,
		.ddrl_pblcsr1 = 0x00000300,
		// DLL設定ガイドを見ないとわからない.
		.ddrl_trwlvl0 = 0x00076006,
		.ddrl_trwlvl1 = 0x00078008,
		.ddrl_trgl0 = 0x000000C9,
		.ddrl_trgl1 = 0x000000CB,
		.ddrl_trrds = 0x00000003,
		.ddrl_trrdey0 = 0x00010020,
		.ddrl_trrdey1 = 0x00010020,
		.ddrl_trwdedqs0 = 0x00010020,
		.ddrl_trwdedqs1 = 0x00010020,
		.ddrl_trwdepb0 = 0x0000000F,
		.ddrl_trwdepb1 = 0x0000000F,
		.ddrl_trrdepb0 = 0x0000000F,
		.ddrl_trrdepb1 = 0x0000000F,
	}
};

