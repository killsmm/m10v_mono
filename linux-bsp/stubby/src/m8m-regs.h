#ifndef  ___REGS_H___
#define  ___REGS_H___

#define IOADR_BASE_SDI0	(0x20020000)
#define IOADR_BASE_SDI1	(0x20040000)
#define IOADR_BASE_SDI2	(0x20060000)
//全体制御
#define IOADR_SDITOPRST1(b)	((b) + 0x0000)
#define IOADR_SDITOPSCNF1(b)	((b) + 0x0100)	// 使用する SDRAM のバス幅を設定(2:x16,3:x32)
#define IOADR_SDITOPSCNF2(b)	((b) + 0x0104)	// CLK[1][0]の(1:有効/0:無効)を設定
#define IOADR_SDITOPSCNF3(b)	((b) + 0x0108)	// 
// clock
#define IOADR_SDICLKEN2(b)	((b) + 0x1004)	// 
#define IOADR_SDICLKEN3(b)	((b) + 0x1008)	// 
#define IOADR_SDICLKEN4(b)	((b) + 0x100C)	// 
#define IOADR_SDICLKEN5(b)	((b) + 0x1010)	// 
#define IOADR_SDICLKEN6(b)	((b) + 0x1014)	// 
#define IOADR_SDICLKEN9(b)	((b) + 0x1020)	// 
#define IOADR_SDICLKEN10(b)	((b) + 0x1024)	// 
#define IOADR_SDICLKEN11(b)	((b) + 0x1028)	// 
#define IOADR_SDICLKEN13(b)	((b) + 0x1030)	// 
#define IOADR_SDICLKEN14(b)	((b) + 0x1034)	// 
// Phy I/F
#define IOADR_SDIPHYRST1(b)	((b) + 0x2000)	// リセット制御
#define IOADR_SDIPHYCTL1(b)	((b) + 0x2100)	// 制御
#define IOADR_SDIPHYINTE1(b)	((b) + 0x2200)	// 割り込み要求許
#define IOADR_SDIPHYINTF1(b)	((b) + 0x2300)	// 割り込み要求フラグ
#define IOADR_SDIPHYAC1(b)	((b) + 0x2400)	// PHY AC設定
#define IOADR_SDIPHYAC2(b)	((b) + 0x2404)	// 
#define IOADR_SDIPHYAC3(b)	((b) + 0x2408)	// 
#define IOADR_SDIPHYAC4(b)	((b) + 0x240C)	// 
#define IOADR_SDIPHYAC5(b)	((b) + 0x2410)	// 
#define IOADR_SDIPHYAC6(b)	((b) + 0x2414)	// 
#define IOADR_SDIPHYCAL1(b)	((b) + 0x2500)	// PHY DCキャリブレーション設定

// SDRAM I/F
#define IOADR_SDIDDRRST1(b)	((b) + 0x3000)	// リセット制御
#define IOADR_SDIDDRCTL1(b)	((b) + 0x3100)	// 制御
#define IOADR_SDIDDRCTL2(b)	((b) + 0x3104)	// 
#define IOADR_SDIDDRCTL3(b)	((b) + 0x3108)	// 
#define IOADR_SDIDDRCTL4(b)	((b) + 0x310C)	// 
#define IOADR_SDIDDRCTL5(b)	((b) + 0x3110)	// 
#define IOADR_SDIDDRCTL6(b)	((b) + 0x3114)	// 
#define IOADR_SDIDDRCTL7(b)	((b) + 0x3118)	// 
#define IOADR_SDIDDRINTE1(b)	((b) + 0x3200)	// 割り込み要求許
#define IOADR_SDIDDRINTE2(b)	((b) + 0x3204)	// 
#define IOADR_SDIDDRINTE3(b)	((b) + 0x3208)	// 
#define IOADR_SDIDDRINTE4(b)	((b) + 0x320C)	// 
#define IOADR_SDIDDRINTE5(b)	((b) + 0x3210)	// 
#define IOADR_SDIDDRINTE6(b)	((b) + 0x3214)	// 
#define IOADR_SDIDDRINTE7(b)	((b) + 0x3218)	// 
#define IOADR_SDIDDRINTF1(b)	((b) + 0x3300)	// 割り込み要求フラグ
#define IOADR_SDIDDRINTF2(b)	((b) + 0x3304)	// 
#define IOADR_SDIDDRINTF3(b)	((b) + 0x3308)	// 
#define IOADR_SDIDDRINTF4(b)	((b) + 0x330C)	// 
#define IOADR_SDIDDRINTF5(b)	((b) + 0x3310)	// 
#define IOADR_SDIDDRINTF6(b)	((b) + 0x3314)	// 
#define IOADR_SDIDDRINTF7(b)	((b) + 0x3318)	// 
#define IOADR_SDIDDRMR0(b)	((b) + 0x3400)	// SDRAM MR設定
#define IOADR_SDIDDRMR1(b)	((b) + 0x3404)	// 
#define IOADR_SDIDDRMR2(b)	((b) + 0x3408)	// 
#define IOADR_SDIDDRMR3(b)	((b) + 0x340C)	// 
#define IOADR_SDIDDRCNF(b)	((b) + 0x3500)	// SDRAM構成設定
#define IOADR_SDIDDRAC1(b)	((b) + 0x3600)	// SDRAM AC設定
#define IOADR_SDIDDRAC2(b)	((b) + 0x3604)	// 
#define IOADR_SDIDDRAC3(b)	((b) + 0x3608)	// 
#define IOADR_SDIDDRAC4(b)	((b) + 0x360C)	// 
#define IOADR_SDIDDRAC5(b)	((b) + 0x3610)	// 
#define IOADR_SDIDDRAC6(b)	((b) + 0x3614)	// 
#define IOADR_SDIDDRAC7(b)	((b) + 0x3618)	// 
#define IOADR_SDIDDRAC8(b)	((b) + 0x361C)	// 
#define IOADR_SDIDDRAC9(b)	((b) + 0x3620)	// 
#define IOADR_SDIDDRACA(b)	((b) + 0x3624)	// 
#define IOADR_SDIDDRACB(b)	((b) + 0x3628)	// 
#define IOADR_SDIDDRACC(b)	((b) + 0x362C)	// 
#define IOADR_SDIDDRACD(b)	((b) + 0x3630)	// 
#define IOADR_SDIDDRPHY1(b)	((b) + 0x3700)	// PHY機能設定
#define IOADR_SDIDDRPHY2(b)	((b) + 0x3704)	// 
#define IOADR_SDIDDRPHY3(b)	((b) + 0x3708)	// 
#define IOADR_SDIDDRAR1(b)	((b) + 0x3800)	// SDRAM AR設定
#define IOADR_SDIDDRAR2(b)	((b) + 0x3804)	// 
#define IOADR_SDIDDRZQ1(b)	((b) + 0x3900)	// SDRAM ZQ設定
#define IOADR_SDIDDRZQ2(b)	((b) + 0x3904)	// 
#define IOADR_SDIDDRCMD1(b)	((b) + 0x3A00)	// SDRAM CMD設定
#define IOADR_SDIDDRCMD2(b)	((b) + 0x3A04)	// 

// アービトレーション制御部
#define IOADR_SDIARBRST(b)	((b) + 0x4000)	// リセット制御
#define IOADR_SDIARBCTL1(b)	((b) + 0x4100)	// 制御
#define IOADR_SDIARBCTL2(b)	((b) + 0x4104)	// 制御
#define IOADR_SDIARBINTE1(b)	((b) + 0x4200)	// 割り込み要求許
#define IOADR_SDIARBINTE2(b)	((b) + 0x4204)	// 割り込み要求許
#define IOADR_SDIARBINTF1(b)	((b) + 0x4300)	// 割り込み要求フラグ
#define IOADR_SDIARBINTF2(b)	((b) + 0x4304)	// 割り込み要求フラグ
#define IOADR_SDIARBARB1(b)	((b) + 0x4400)	// アービトレーション設定
#define IOADR_SDIARBARB2(b)	((b) + 0x4404)	// アービトレーション設定

// アクセスモニタ制御部
#define IOADR_SDIMONRST1(b)	((b) + 0x5000)	// リセット制御
#define IOADR_SDIMONCTL1(b)	((b) + 0x5100)	// 制御
#define IOADR_SDIMONCTL2(b)	((b) + 0x5104)	// 
#define IOADR_SDIMONCTL3(b)	((b) + 0x5108)	// 
#define IOADR_SDIMONINTE1(b)	((b) + 0x5200)	// 割り込み要求許
#define IOADR_SDIMONINTE2(b)	((b) + 0x5204)	// 
#define IOADR_SDIMONINTE3(b)	((b) + 0x5208)	// 
#define IOADR_SDIMONINTF1(b)	((b) + 0x5300)	// 割り込み要求フラグ
#define IOADR_SDIMONINTF2(b)	((b) + 0x5304)	// 
#define IOADR_SDIMONINTF3(b)	((b) + 0x5308)	// 
#define IOADR_SDIMONSET1(b)	((b) + 0x5400)	// アクセスモニタ設定
#define IOADR_SDIMONSET2(b)	((b) + 0x5404)	// 
#define IOADR_SDIMONCMON1(b)	((b) + 0x5500)	// 転送回数モニタ
#define IOADR_SDIMONCMON2(b)	((b) + 0x5504)	// 
#define IOADR_SDIMONCMON3(b)	((b) + 0x5508)	// 
#define IOADR_SDIMONCMON4(b)	((b) + 0x550C)	// 
#define IOADR_SDIMONCMON5(b)	((b) + 0x5510)	// 
#define IOADR_SDIMONCMON6(b)	((b) + 0x5514)	// 
#define IOADR_SDIMONCMON7(b)	((b) + 0x5518)	// 
#define IOADR_SDIMONCMON8(b)	((b) + 0x551C)	// 
#define IOADR_SDIMONCMON9(b)	((b) + 0x5520)	// 
#define IOADR_SDIMONCMONA(b)	((b) + 0x5524)	// 
#define IOADR_SDIMONSMON1(b)	((b) + 0x5600)	// 転送量モニタ	
#define IOADR_SDIMONSMON2(b)	((b) + 0x5604)	// 
#define IOADR_SDIMONSMON3(b)	((b) + 0x5608)	// 
#define IOADR_SDIMONSMON4(b)	((b) + 0x560C)	// 
#define IOADR_SDIMONSMON5(b)	((b) + 0x5610)	// 
#define IOADR_SDIMONSMON6(b)	((b) + 0x5614)	// 
#define IOADR_SDIMONSMON7(b)	((b) + 0x5618)	// 
#define IOADR_SDIMONSMON8(b)	((b) + 0x561C)	// 
#define IOADR_SDIMONSMON9(b)	((b) + 0x5620)	// 
#define IOADR_SDIMONSMONA(b)	((b) + 0x5624)	// 

// AXI I/F制御部[0]
#define IOADR_SDIAXI0RST1(b)	((b) + 0x8000)	// リセット制御
#define IOADR_SDIAXI0CTL1(b)	((b) + 0x8100)	// 制御
#define IOADR_SDIAXI0CTL2(b)	((b) + 0x8104)	// 
#define IOADR_SDIAXI0INTE1(b)	((b) + 0x8200)	// 割り込み要求許
#define IOADR_SDIAXI0INTE2(b)	((b) + 0x8204)	// 
#define IOADR_SDIAXI0INTF1(b)	((b) + 0x8300)	// 割り込み要求フラグ
#define IOADR_SDIAXI0INTF2(b)	((b) + 0x8304)	// 
#define IOADR_SDIAXI0ARB1(b)	((b) + 0x8400)	// アービトレーション設定

// AXI I/F制御部[1]
#define IOADR_SDIAXI1RST1(b)	((b) + 0x9000)	// リセット制御
#define IOADR_SDIAXI1CTL1(b)	((b) + 0x9100)	// 制御
#define IOADR_SDIAXI1CTL2(b)	((b) + 0x9104)	// 
#define IOADR_SDIAXI1INTE1(b)	((b) + 0x9200)	// 割り込み要求許
#define IOADR_SDIAXI1INTE2(b)	((b) + 0x9204)	// 
#define IOADR_SDIAXI1INTF1(b)	((b) + 0x9300)	// 割り込み要求フラグ
#define IOADR_SDIAXI1INTF2(b)	((b) + 0x9304)	// 
#define IOADR_SDIAXI1ARB1(b)	((b) + 0x9400)	// アービトレーション設定

// AXI I/F制御部[2]
#define IOADR_SDIAXI2RST1(b)	((b) + 0xA000)	// リセット制御
#define IOADR_SDIAXI2CTL1(b)	((b) + 0xA100)	// 制御
#define IOADR_SDIAXI2CTL2(b)	((b) + 0xA104)	// 
#define IOADR_SDIAXI2INTE1(b)	((b) + 0xA200)	// 割り込み要求許
#define IOADR_SDIAXI2INTE2(b)	((b) + 0xA204)	// 
#define IOADR_SDIAXI2INTF1(b)	((b) + 0xA300)	// 割り込み要求フラグ
#define IOADR_SDIAXI2INTF2(b)	((b) + 0xA304)	// 
#define IOADR_SDIAXI2ARB1(b)	((b) + 0xA400)	// アービトレーション設定


// Ribery I/F制御部[0]
#define IOADR_SDIRBR0RST1(b)	((b) + 0xC000)	// リセット制御
#define IOADR_SDIRBR0CTL1(b)	((b) + 0xC100)	// 制御
#define IOADR_SDIRBR0INTE1(b)	((b) + 0xC200)	// 割り込み要求許
#define IOADR_SDIRBR0INTF1(b)	((b) + 0xC300)	// 割り込み要求フラグ

// Ribery I/F制御部[1]
#define IOADR_SDIRBR1RST1(b)	((b) + 0xD000)	// リセット制御
#define IOADR_SDIRBR1CTL1(b)	((b) + 0xD100)	// 制御
#define IOADR_SDIRBR1INTE1(b)	((b) + 0xD200)	// 割り込み要求許
#define IOADR_SDIRBR1INTF1(b)	((b) + 0xD300)	// 割り込み要求フラグ

// DDRL-11
#define IOADR_DDRL_MRS0(b)	((b) + 0x10000)
#define IOADR_DDRL_MRS1(b)	((b) + 0x10004)
#define IOADR_DDRL_MRS2(b)	((b) + 0x10008)
#define IOADR_DDRL_MRS3(b)	((b) + 0x1000C)
#define IOADR_DDRL_MRSWT(b)	((b) + 0x10010)
#define IOADR_DDRL_ACTWT(b)	((b) + 0x10014)
#define IOADR_DDRL_PREWT(b)	((b) + 0x10018)
#define IOADR_DDRL_WRITEWT(b)	((b) + 0x1001C)
#define IOADR_DDRL_READWT(b)	((b) + 0x10020)
#define IOADR_DDRL_PHYMODE(b)	((b) + 0x10024)
#define   BIT_PHYMODE__ODT_OFF	(1<<12)

#define IOADR_DDRL_DLLMODE1(b)	((b) + 0x10028)

#define IOADR_DDRL_DDRIS1(b)	((b) + 0x10030)	// "DDR-PHY Setting Impedance Selection 1"
#define  DF_SOCDRV_60OHM (4)
#define  DF_SOCDRV_48OHM (5)
#define  DF_SOCDRV_40OHM (6)
#define  DF_SOCDRV_34OHM (7)
#define IOADR_DDRL_PZQCM(b)	((b) + 0x10034)
#define IOADR_DDRL_PZQCDM(b)	((b) + 0x10038)
#define IOADR_DDRL_DDRIOCM(b)	((b) + 0x1003C)
#define  DF_SOCODT_120OHM (1)
#define  DF_SOCODT_60OHM (2)
#define  DF_SOCODT_40OHM (3)
#define IOADR_DDRL_CTRLDSPC(b)	((b) + 0x10040)
#define IOADR_DDRL_DDRIS2(b)	((b) + 0x10044)	// "DDR-PHY Setting Impedance Selection 2"
#define IOADR_DDRL_DLLMODE3(b)	((b) + 0x10048)	// 本レジスタは初期値でしか使用しません。

#define IOADR_DDRL_TRMS(b)	((b) + 0x10050)	// Training mode mode select
#define  BIT_DDRL__WL_DLY_SEL	(1<<0)	// {0:traingin result, 1:register value}
#define  BIT_DDRL__GT_DLY_SEL	(1<<1)
#define  BIT_DDRL__RDS_DLY_SEL	(1<<2)
#define  BIT_DDRL__RDE_DLY_SEL	(1<<3)
#define  BIT_DDRL__WDE_DLY_SEL	(1<<4)
#define  DF_DDRL__RDE_MODE_SFT	(16)	// { 0:Byte, 1:Byte+Bit, 2:Bit(*_DLY_SEL=0のときのみ選択可) }
#define  DF_DDRL__WDE_MODE_SFT	(24)	// 上に同じ


// follows are manual set. set TRMS::(WL|GT|RDS|RDE|WDT)_DLY_SEL=0, training result are used.
#define IOADR_DDRL_PBLCSW0(b)	((b) + 0x10054)	// Per Byte leveling code setting (Without Training) Write Leveling/Write Data Eye Byte0
#define IOADR_DDRL_PBLCSW1(b)	((b) + 0x10058)
//#define IOADR_DDRL_PBLCSW2(b)	((b) + 0x1005C)
//#define IOADR_DDRL_PBLCSW3(b)	((b) + 0x10060)
#define IOADR_DDRL_PBLCSG0(b)	((b) + 0x10074)	// Per Byte leveling code setting (Without Training) Gate Training/Read Data Eye Byte0
#define IOADR_DDRL_PBLCSG1(b)	((b) + 0x10078)
#define IOADR_DDRL_PBLCSR0(b)	((b) + 0x10094)	// Per Byte leveling code setting (Without Training) Read Data Synchronizer Byte0
#define IOADR_DDRL_PBLCSR1(b)	((b) + 0x10098)

#define IOADR_DDRL_SDRAMM(b)	((b) + 0x100B4)
#define IOADR_DDRL_RANKSEL(b)	((b) + 0x100B8)
#define IOADR_DDRL_NAODTS(b)	((b) + 0x100BC)
#define IOADR_DDRL_WLOS(b)	((b) + 0x100C0)
#define IOADR_DDRL_WLRS1(b)	((b) + 0x100C4)
#define IOADR_DDRL_WLRS2(b)	((b) + 0x100C8)

// テスト範囲の指定
#define IOADR_DDRL_TRWLVL0(b)	((b) + 0x100E4)	// Write Leveling Byte0
#define IOADR_DDRL_TRWLVL1(b)	((b) + 0x100E8)	// Write Leveling Byte1
#define IOADR_DDRL_TRGL0(b)		((b) + 0x10104)	// Gate Training Byte0
#define IOADR_DDRL_TRGL1(b)		((b) + 0x10108)	
#define IOADR_DDRL_TRRDS(b)		((b) + 0x10124)	// Read Data Synchronizer
#define IOADR_DDRL_TRRDEY0(b)	((b) + 0x10128)	// Read Data Eye Byte0
#define IOADR_DDRL_TRRDEY1(b)	((b) + 0x1012C)	
#define IOADR_DDRL_TRWDEDQS0(b)	((b) + 0x10148)	// Write Data Eye Byte0
#define IOADR_DDRL_TRWDEDQS1(b)	((b) + 0x1014C)	
#define IOADR_DDRL_TRWDEPB0(b)	((b) + 0x103DC)	// Write Data Eye(PerBit) Byte0
#define IOADR_DDRL_TRWDEPB1(b)	((b) + 0x103E0)	
#define IOADR_DDRL_TRRDEPB0(b)	((b) + 0x10400)	// Read Data Eye(PerBit) Byte0
#define IOADR_DDRL_TRRDEPB1(b)	((b) + 0x10404)

#define IOADR_DDRL_TTDTPS(b)	((b) + 0x10820)	// Read/Write Data Eye用簡易パターン選択

// トレーニングエラー
#define IOADR_DDRL_TRAINERR(b)	((b) + 0x1016C)	// [RO] Trainig error status register
#define  BIT_TRAIN_ERR_CAT_ERR	(1<<8)
#define  BIT_TRAIN_ERR_RWT_ERR	(1<<7)
#define  BIT_TRAIN_ERR_WDE_ERR	(1<<6)
#define  BIT_TRAIN_ERR_RDE_ERR	(1<<5)
#define  BIT_TRAIN_ERR_RDS_ERR	(1<<4)
#define  BIT_TRAIN_ERR_GT_ERR	(1<<3)
#define  BIT_TRAIN_ERR_WL_ERR	(1<<2)
#define  BIT_TRAIN_ERR_TRMOD_ERR	(1<<1)
#define  BIT_TRAIN_ERR_REG_CHE_ERR	(1<<0)


struct stSDI_CONFIG {
	// 全体制御 - SDRAM構成設定
	u32 tops_cnf1 ;
	u32 tops_cnf2 ;
	u32 tops_cnf3 ;

	// PHY I/F制御部
	// AC
	u32 phy_ac1 ;
	u32 phy_ac2 ;
	u32 phy_ac3 ;
	u32 phy_ac4 ;
	u32 phy_ac5 ;
	u32 phy_ac6 ;
	// Cal.
	u32 phy_cal1 ;

	// SDRAM I/F制御部
	u32 ddr_mr0 ;
	u32 ddr_mr1 ;
	u32 ddr_mr2 ;
//	u32 ddr_mr3 ;
	// 
	u32 ddr_cnf1 ;
	// AC設定
	u32 ddr_ac1 ;
	u32 ddr_ac2 ;
	u32 ddr_ac3 ;
	u32 ddr_ac4 ;
	u32 ddr_ac5 ;
	u32 ddr_ac6 ;
	u32 ddr_ac7 ;
	u32 ddr_ac8 ;
	u32 ddr_ac9 ;
	u32 ddr_ac10 ;
	u32 ddr_ac11 ;
	u32 ddr_ac12 ;
	u32 ddr_ac13 ;
	// PHY機能設定
	u32 ddr_phy1 ;
	u32 ddr_phy2 ;
	u32 ddr_phy3 ;
	// AR設定
	u32 ddr_ar1 ;
	u32 ddr_ar2 ;
	// ZQ設定
	u32 ddr_zq1 ;
	u32 ddr_zq2 ;
	// CMD設定
	u32 ddr_cmd1 ;
	u32 ddr_cmd2 ;

	//ddrl
	u32 ddrl_mr0;
	u32 ddrl_mr1;
	u32 ddrl_mr2;
	u32 ddrl_mrswt;
	u32 ddrl_actwt;
	u32 ddrl_prewt;
	u32 ddrl_writewt;
	u32 ddrl_readwt;
	u32 ddrl_ddris1;
	u32 ddrl_pzqcm;
	u32 ddrl_ddriocm;
	u32 ddrl_ctrldspc;
	u32 ddrl_ddris2;
	u32 ddrl_trms;
	u32 ddrl_pblcsw0;
	u32 ddrl_pblcsw1;
	u32 ddrl_pblcsg0;
	u32 ddrl_pblcsg1;
	u32 ddrl_pblcsr0;
	u32 ddrl_pblcsr1;
	u32 ddrl_trwlvl0;
	u32 ddrl_trwlvl1;
	u32 ddrl_trgl0;
	u32 ddrl_trgl1;
	u32 ddrl_trrds;
	u32 ddrl_trrdey0;
	u32 ddrl_trrdey1;
	u32 ddrl_trwdedqs0;
	u32 ddrl_trwdedqs1;
	u32 ddrl_trwdepb0;
	u32 ddrl_trwdepb1;
	u32 ddrl_trrdepb0;
	u32 ddrl_trrdepb1;
};


#endif /* ___REGS_H___ */

