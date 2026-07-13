/**
 * @file		__f_hdmitx2p0_lp.h
 * @brief		Definition F_HDMITX2P0_LP Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* Interrupt Registers */

/* structure of PWRCTRL (2892_0000h) */
union  io_f_hdmitx_pwrctrl{
	unsigned short		hword;
	struct {
		unsigned short	ck_poweron					:1;
		unsigned short	tx_poweron2					:1;
		unsigned short	tx_poweron1					:1;
		unsigned short	tx_poweron0					:1;
		unsigned short	tx_pwron					:1;
		unsigned short	bias_data_on				:1;
		unsigned short	bias_clk_on					:1;
		unsigned short								:8;
		unsigned short	Override					:1;
	}bit;
};

/* structure of OPMODE_PLLCFG (2892_0006h) */
union  io_f_hdmitx_cpmode_pllcfg{
	unsigned short		hword;
	struct {
		unsigned short	n_cntrl						:2;
		unsigned short	ref_cntrl					:2;
		unsigned short	fbdiv1_cntrl				:2;
		unsigned short	fbdiv2_cntrl				:3;
		unsigned short	opmode						:2;
		unsigned short	tmdsmhl_cntrl				:2;
		unsigned short	pre_pdiv					:2;
		unsigned short								:1;
	}bit;
};

/* structure of CKSYMTXCTRL (2892_0009h) */
union  io_f_hdmitx_cksymtxctrl{
	unsigned short		hword;
	struct {
		unsigned short	Ck_symon					:1;
		unsigned short	Tx_trbon					:1;
		unsigned short	Tx_traon					:1;
		unsigned short	Tx_symon					:1;
		unsigned short	slopeboost					:2;
		unsigned short								:9;
		unsigned short	Override					:1;
	}bit;
};

/* structure of VLEVCTRL (2892_000Eh) */
union  io_f_hdmitx_vlevctrl{
	unsigned short		hword;
	struct {
		unsigned short	sup_ck_lvl					:5;
		unsigned short	sup_tx_lvl					:5;
		unsigned short								:5;
		unsigned short	Override					:1;
	}bit;
};

/* structure of PLLCURRCTRL (2892_0010h) */
union  io_f_hdmitx_pllcurrctrl{
	unsigned short		hword;
	struct {
		unsigned short	mpll_int_cntrl				:3;
		unsigned short	mpll_prop_cntrl				:3;
		unsigned short								:10;
	}bit;
};

/* structure of PLLCTRL (2892_0014h) */
union  io_f_hdmitx_pllctrl{
	unsigned short		hword;
	struct {
		unsigned short								:2;
		unsigned short	mpll_rst					:1;
		unsigned short	mpll_pwr_on					:1;
		unsigned short								:11;
		unsigned short	Override					:1;
	}bit;
};

/* structure of PLLGMPCTRL (2892_0015h) */
union  io_f_hdmitx_pllgmpctrl{
	unsigned short		hword;
	struct {
		unsigned short	Mpll_gmp_cntrl				:2;
		unsigned short								:14;
	}bit;
};

/* structure of TXTERM (2892_0019h) */
union  io_f_hdmitx_txterm{
	unsigned short		hword;
	struct {
		unsigned short	d_tx_term					:3;
		unsigned short								:13;
	}bit;
};

/*  structure of ih_fc_stat0 (2892_0100h)    */
union io_f_hdmitx_ih_fc_stat0{
	unsigned char		byte[4];
	struct {
		unsigned char	NULL_PKT					:1;
		unsigned char	ACR							:1;
		unsigned char	AUDS						:1;
		unsigned char	NVBI						:1;
		unsigned char	MAS							:1;
		unsigned char	HBR							:1;
		unsigned char	ACP							:1;
		unsigned char	AUDI						:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_fc_stat1 (2892_0101h)    */
union io_f_hdmitx_ih_fc_stat1{
	unsigned char		byte[4];
	struct {
		unsigned char	GCP							:1;
		unsigned char	AVI							:1;
		unsigned char	AMP							:1;
		unsigned char	SPD							:1;
		unsigned char	VSD							:1;
		unsigned char	ISCR2						:1;
		unsigned char	ISCR1						:1;
		unsigned char	GMD							:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_fc_stat2 (2892_0102h)    */
union io_f_hdmitx_ih_fc_stat2{
	unsigned char		byte[4];
	struct {
		unsigned char	HighPriority_overflow		:1;
		unsigned char	LowPriority_overflow		:1;
		unsigned char								:6;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_as_stat0 (2892_0103h)    */
union io_f_hdmitx_ih_as_stat0{
	unsigned char		byte[4];
	struct {
		unsigned char	Aud_fifo_overflow			:1;
		unsigned char	Aud_fifo_underflow			:1;
		unsigned char								:1;
		unsigned char	fifo_overrun				:1;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_phy_stat0 (2892_0104h) / ih_mute_phy_stat0 (2892_0184h)   */
union io_f_hdmitx_ih_phy_stat0{
	unsigned char		byte[4];
	struct {
		unsigned char	HDP							:1;
		unsigned char	TX_PHY_LOCK					:1;
		unsigned char	RX_SENSE_0					:1;
		unsigned char	RX_SENSE_1					:1;
		unsigned char	RX_SENSE_2					:1;
		unsigned char	RX_SENSE_3					:1;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_i2cm_stat0 (2892_0105h)    */
union io_f_hdmitx_ih_i2cm_stat0{
	unsigned char		byte[4];
	struct {
		unsigned char	I2Cmastererror				:1;
		unsigned char	I2Cmasterdone				:1;
		unsigned char	scdc_readreq				:1;
		unsigned char								:5;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_cec_stat0 (2892_0106h)    */
union io_f_hdmitx_ih_cec_stat0{
	unsigned char		byte[4];
	struct {
		unsigned char	DONE						:1;
		unsigned char	EOM							:1;
		unsigned char	NACK						:1;
		unsigned char	ARB_LOST					:1;
		unsigned char	ERROR_INITIATOR				:1;
		unsigned char	ERROR_FOLLOW				:1;
		unsigned char	WAKEUP						:1;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_vp_stat0 (2892_0107h)    */
union io_f_hdmitx_ih_vp_stat0{
	unsigned char		byte[4];
	struct {
		unsigned char	fifoemptybyp				:1;
		unsigned char	fifofullbyp					:1;
		unsigned char	fifoemptyremap				:1;
		unsigned char	fifofullremap				:1;
		unsigned char	fifoemptypp					:1;
		unsigned char	fifofullpp					:1;
		unsigned char	fifoemptyrepet				:1;
		unsigned char	fifofullrepet				:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_i2cmphy_stat0 (2892_0108h)    */
union io_f_hdmitx_ih_i2cmphy_stat0{
	unsigned char		byte[4];
	struct {
		unsigned char	I2Cmphyerror				:1;
		unsigned char	I2Cmphydone					:1;
		unsigned char								:6;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_decode (2892_0170h)    */
union io_f_hdmitx_ih_decode{
	unsigned char		byte[4];
	struct {
		unsigned char								:1;
		unsigned char	ih_cec_stat0				:1;
		unsigned char	ih_i2cm_stat0				:1;
		unsigned char	ih_phy						:1;
		unsigned char	ih_as_stat0					:1;
		unsigned char	ih_fc_stat2_vp				:1;
		unsigned char	ih_fc_stat1					:1;
		unsigned char	ih_fc_stat0					:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of ih_mute (2892_01FFh)    */
union io_f_hdmitx_ih_mute{
	unsigned char		byte[4];
	struct {
		unsigned char	mute_all_interrup			:1;
		unsigned char	mute_wakeup_interrupt		:1;
		unsigned char								:6;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* VideoSampler Registers */

/*  structure of tx_invid0 (2892_0200h)    */
union io_f_hdmitx_tx_invid0{
	unsigned char		byte[4];
	struct {
		unsigned char	video_mapping				:5;
		unsigned char								:2;
		unsigned char	internal_de_generator		:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of tx_instuffing (2892_0201h)    */
union io_f_hdmitx_tx_instuffing{
	unsigned char		byte[4];
	struct {
		unsigned char	gydata_stuffing				:1;
		unsigned char	rcrdata_stuffing			:1;
		unsigned char	bcbdata_stuffing			:1;
		unsigned char								:5;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* VideoPacketizer Registers */

/*  structure of vp_status (2892_0800h)    */
union io_f_hdmitx_vp_status{
	unsigned char		byte[4];
	struct {
		unsigned char	packing_phase				:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of vp_pr_cd (2892_0801h)    */
union io_f_hdmitx_vp_pr_cd{
	unsigned char		byte[4];
	struct {
		unsigned char	desired_pr_factor			:4;
		unsigned char	color_depth					:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of vp_stuff (2892_0802h)    */
union io_f_hdmitx_vp_stuff{
	unsigned char		byte[4];
	struct {
		unsigned char	pr_stuffing					:1;
		unsigned char	pp_stuffing					:1;
		unsigned char	ycc422_stuffing				:1;
		unsigned char	icx_goto_p0_st				:1;
		unsigned char	ifix_pp_to_last				:1;
		unsigned char	idefault_phase				:1;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of vp_remap (2892_0803h)    */
union io_f_hdmitx_vp_remap{
	unsigned char		byte[4];
	struct {
		unsigned char	ycc422_size					:2;
		unsigned char								:6;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of vp_conf (2892_0804h)    */
union io_f_hdmitx_vp_conf{
	unsigned char		byte[4];
	struct {
		unsigned char	output_selector				:2;
		unsigned char	bypass_select				:1;
		unsigned char	ycc422_en					:1;
		unsigned char	pr_en						:1;
		unsigned char	pp_en						:1;
		unsigned char	bypass_en					:1;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of vp_mask (2892_0807h)    */
union io_f_hdmitx_vp_mask{
	unsigned char		byte[4];
	struct {
		unsigned char	ointemptybyp				:1;
		unsigned char	ointfullbyp					:1;
		unsigned char	ointemptyremap				:1;
		unsigned char	ointfullremap				:1;
		unsigned char	ointemptypp					:1;
		unsigned char	ointfullpp					:1;
		unsigned char	ointemptyrepet				:1;
		unsigned char	ointfullrepet				:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* FrameComposer Registers */

/*  structure of fc_invidconf (2892_1000h)    */
union io_f_hdmitx_fc_invidconf{
	unsigned char		byte[4];
	struct {
		unsigned char	in_I_P						:1;
		unsigned char	r_v_blank_in_osc			:1;
		unsigned char								:1;
		unsigned char	DVI_modez					:1;
		unsigned char	de_in_polarity				:1;
		unsigned char	hsync_in_polarity			:1;
		unsigned char	vsync_in_polarity			:1;
		unsigned char	HDCP_keepout				:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_inhactiv0 (2892_1001h)    */
union io_f_hdmitx_fc_inhactiv0{
	unsigned char		byte[4];
	struct {
		unsigned char	H_in_activ					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_inhactiv1 (2892_1002h)    */
union io_f_hdmitx_fc_inhactiv1{
	unsigned char		byte[4];
	struct {
		unsigned char	H_in_activ					:4;
		unsigned char	H_in_activ_12				:1;
		unsigned char	H_in_activ_13				:1;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_inhblank0 (2892_1003h)    */
union io_f_hdmitx_fc_inhblank0{
	unsigned char		byte[4];
	struct {
		unsigned char	H_in_blank					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_inhblank1 (2892_1004h)    */
union io_f_hdmitx_fc_inhblank1{
	unsigned char		byte[4];
	struct {
		unsigned char	H_in_blank					:2;
		unsigned char	H_in_blank_12				:3;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_invactiv0 (2892_1005h)    */
union io_f_hdmitx_fc_invactiv0{
	unsigned char		byte[4];
	struct {
		unsigned char	V_in_activ					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_invactiv1 (2892_1006h)    */
union io_f_hdmitx_fc_invactiv1{
	unsigned char		byte[4];
	struct {
		unsigned char	V_in_activ					:3;
		unsigned char	V_in_activ_12_11			:2;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_hsyncindelay0 (2892_1008h)    */
union io_f_hdmitx_fc_hsyncindelay0{
	unsigned char		byte[4];
	struct {
		unsigned char	H_in_delay					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_hsyncindelay1 (2892_1009h)    */
union io_f_hdmitx_fc_hsyncindelay1{
	unsigned char		byte[4];
	struct {
		unsigned char	H_in_delay					:3;
		unsigned char	H_in_delay_12				:2;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_hsyncinwidth0 (2892_100Ah)    */
union io_f_hdmitx_fc_hsyncinwidth0{
	unsigned char		byte[4];
	struct {
		unsigned char	H_in_width					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_hsyncinwidth1 (2892_100Bh)    */
union io_f_hdmitx_fc_hsyncinwidth1{
	unsigned char		byte[4];
	struct {
		unsigned char	H_in_width					:1;
		unsigned char	H_in_width_9				:1;
		unsigned char								:6;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_vsyncinwidth (2892_100Dh)    */
union io_f_hdmitx_fc_vsyncinwidth{
	unsigned char		byte[4];
	struct {
		unsigned char	V_in_width					:6;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/*  structure of fc_infreq2 (2892_1010h)    */
union io_f_hdmitx_fc_infreq2{
	unsigned char		byte[4];
	struct {
		unsigned char	infreq						:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_ch1pream (2892_1015h)    */
union io_f_hdmitx_fc_ch1pream{
	unsigned char		byte[4];
	struct {
		unsigned char	ch1_preamble_filter			:6;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_ch2pream (2892_1016h)    */
union io_f_hdmitx_fc_ch2pream{
	unsigned char		byte[4];
	struct {
		unsigned char	ch2_preamble_filter			:6;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_aviconf3 (2892_1017h)    */
union io_f_hdmitx_fc_aviconf3{
	unsigned char		byte[4];
	struct {
		unsigned char	CN							:2;
		unsigned char	YQ							:2;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_gcp (2892_1018h)    */
union io_f_hdmitx_fc_gcp{
	unsigned char		byte[4];
	struct {
		unsigned char	clear_avmute				:1;
		unsigned char	set_avmute					:1;
		unsigned char	default_phase				:1;
		unsigned char								:5;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_aviconf0 (2892_1019h)    */
union io_f_hdmitx_fc_aviconf0{
	unsigned char		byte[4];
	struct {
		unsigned char	rgc_ycc_indication			:2;
		unsigned char	bar_information				:2;
		unsigned char	scan_information			:2;
		unsigned char	active_format_presen		:1;
		unsigned char	rgc_ycc_indication_2		:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_aviconf1 (2892_101Ah)    */
union io_f_hdmitx_fc_aviconf1{
	unsigned char		byte[4];
	struct {
		unsigned char	active_aspect_ratio			:4;
		unsigned char	picture_aspect_ratio		:2;
		unsigned char	Colorimetry					:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_aviconf2 (2892_101Bh)    */
union io_f_hdmitx_fc_aviconf2{
	unsigned char		byte[4];
	struct {
		unsigned char	non_uniform_picture_scaling	:2;
		unsigned char	quantization_range			:2;
		unsigned char	extended_colorimetry		:3;
		unsigned char	it_content					:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_avivid (2892_101Ch)    */
union io_f_hdmitx_fc_avivid{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_avivid					:7;
		unsigned char	fc_avivid_7					:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audiconf0 (2892_1025h)    */
union io_f_hdmitx_fc_audiconf0{
	unsigned char		byte[4];
	struct {
		unsigned char	CT							:4;
		unsigned char	CC							:3;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audiconf1 (2892_1026h)    */
union io_f_hdmitx_fc_audiconf1{
	unsigned char		byte[4];
	struct {
		unsigned char	SF							:3;
		unsigned char								:1;
		unsigned char	SS							:2;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audiconf3 (2892_1028h)    */
union io_f_hdmitx_fc_audiconf3{
	unsigned char		byte[4];
	struct {
		unsigned char	LSV							:4;
		unsigned char	DM_INH						:1;
		unsigned char	LFEPBL						:2;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_vsdsize (2892_102Ah)    */
union io_f_hdmitx_fc_vsdsize{
	unsigned char		byte[4];
	struct {
		unsigned char	VSDSIZE						:5;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_vsdpayload (2892_1032 - 2892_1049h)    */
union io_f_hdmitx_fc_vsdpayload{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_vsdpayload				:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_spdvendorname (2892_104A - 2892_1051h)    */
union io_f_hdmitx_fc_spdvendorname{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_spdvendorname			:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_spdproductname (2892_1052 - 2892_1061h)    */
union io_f_hdmitx_fc_spdproductname{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_spdproductname			:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_acp (2892_1082 - 2892_1091h)    */
union io_f_hdmitx_fc_acp{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_acp						:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_iscr1 (2892_1082 - 2892_1091h)    */
union io_f_hdmitx_fc_iscr1{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_iscr1					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_iscr2 (2892_10A3 - 2892_10B2h)    */
union io_f_hdmitx_fc_iscr2{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_iscr2					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audsconf (2892_1063h)    */
union io_f_hdmitx_fc_audsconf{
	unsigned char		byte[4];
	struct {
		unsigned char	aud_packet_layout			:1;
		unsigned char								:3;
		unsigned char	aud_packet_sampflt			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audsstat (2892_1064h)    */
union io_f_hdmitx_fc_audsstat{
	unsigned char		byte[4];
	struct {
		unsigned char	packet_sampprs				:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audsv (2892_1065h)    */
union io_f_hdmitx_fc_audsv{
	unsigned char		byte[4];
	struct {
		unsigned char	V0l							:1;
		unsigned char	V1l							:1;
		unsigned char	V2l							:1;
		unsigned char	V3l							:1;
		unsigned char	V0r							:1;
		unsigned char	V1r							:1;
		unsigned char	V2r							:1;
		unsigned char	V3r							:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audsu (2892_1066h)    */
union io_f_hdmitx_fc_audsu{
	unsigned char		byte[4];
	struct {
		unsigned char	U0l							:1;
		unsigned char	U1l							:1;
		unsigned char	U2l							:1;
		unsigned char	U3l							:1;
		unsigned char	U0r							:1;
		unsigned char	U1r							:1;
		unsigned char	U2r							:1;
		unsigned char	U3r							:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audschnl0 (2892_1067h)    */
union io_f_hdmitx_fc_audschnl0{
	unsigned char		byte[4];
	struct {
		unsigned char	oiec_copyright				:1;
		unsigned char								:3;
		unsigned char	oiec_cgmsa					:2;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audschnl2 (2892_1069h)    */
union io_f_hdmitx_fc_audschnl2{
	unsigned char		byte[4];
	struct {
		unsigned char	oiec_sourcenumber			:4;
		unsigned char	oiec_pcmaudiomode			:3;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audschnl3 (2892_106Ah)    */
union io_f_hdmitx_fc_audschnl3{
	unsigned char		byte[4];
	struct {
		unsigned char	oiec_channelnumcr0			:4;
		unsigned char	oiec_channelnumcr1			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audschnl4 (2892_106Bh)    */
union io_f_hdmitx_fc_audschnl4{
	unsigned char		byte[4];
	struct {
		unsigned char	oiec_channelnumcr2			:4;
		unsigned char	oiec_channelnumcr3			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audschnl5 (2892_106Ch)    */
union io_f_hdmitx_fc_audschnl5{
	unsigned char		byte[4];
	struct {
		unsigned char	oiec_channelnumcl0			:4;
		unsigned char	oiec_channelnumcl1			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audschnl6 (2892_106Dh)    */
union io_f_hdmitx_fc_audschnl6{
	unsigned char		byte[4];
	struct {
		unsigned char	oiec_channelnumcl2			:4;
		unsigned char	oiec_channelnumcl3			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audschnl7 (2892_106Eh)    */
union io_f_hdmitx_fc_audschnl7{
	unsigned char		byte[4];
	struct {
		unsigned char	oiec_sampfreq				:4;
		unsigned char	oiec_clkaccuracy			:2;
		unsigned char	oiec_sampfreq_ext			:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_audschnl8 (2892_106Fh)    */
union io_f_hdmitx_fc_audschnl8{
	unsigned char		byte[4];
	struct {
		unsigned char	oiec_wordlength				:4;
		unsigned char	oiec_origsampfreq			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_ctrlqhigh (2892_1073h)    */
union io_f_hdmitx_fc_ctrlqhigh{
	unsigned char		byte[4];
	struct {
		unsigned char	onhighattended				:5;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_ctrlqlow (2892_1074h)    */
union io_f_hdmitx_fc_ctrlqlow{
	unsigned char		byte[4];
	struct {
		unsigned char	onlowattended				:5;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_iscr1_0 (2892_1092h)    */
union io_f_hdmitx_fc_iscr1_0{
	unsigned char		byte[4];
	struct {
		unsigned char	isrc_cont					:1;
		unsigned char	isrc_valid					:1;
		unsigned char	isrc_status					:3;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_datauto0 (2892_10B3h)    */
union io_f_hdmitx_fc_datauto0{
	unsigned char		byte[4];
	struct {
		unsigned char	acp_auto					:1;
		unsigned char	iscr1_auto					:1;
		unsigned char	iscr2_auto					:1;
		unsigned char	vsd_auto					:1;
		unsigned char	spd_auto					:1;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_datauto1 (2892_10B4h)    */
union io_f_hdmitx_fc_datauto1{
	unsigned char		byte[4];
	struct {
		unsigned char	auto_frame_interpolation	:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_datauto2 (2892_10B5h)    */
union io_f_hdmitx_fc_datauto2{
	unsigned char		byte[4];
	struct {
		unsigned char	auto_line_spacing			:4;
		unsigned char	auto_frame_packets			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_datman (2892_10B6h)    */
union io_f_hdmitx_fc_datman{
	unsigned char		byte[4];
	struct {
		unsigned char	acp_tx						:1;
		unsigned char	iscr1_tx					:1;
		unsigned char	iscr2_tx					:1;
		unsigned char	vsd_tx						:1;
		unsigned char	spd_tx						:1;
		unsigned char	null_tx						:1;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_datauto3 (2892_10B7h)    */
union io_f_hdmitx_fc_datauto3{
	unsigned char		byte[4];
	struct {
		unsigned char	acr_auto					:1;
		unsigned char	audi_auto					:1;
		unsigned char	gcp_auto					:1;
		unsigned char	avi_auto					:1;
		unsigned char	amp_auto					:1;
		unsigned char	nvbi_auto					:1;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb0 (2892_10B8h)    */
union io_f_hdmitx_fc_rdrb0{
	unsigned char		byte[4];
	struct {
		unsigned char	ACRframeinterpolation		:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb1 (2892_10B9h)    */
union io_f_hdmitx_fc_rdrb1{
	unsigned char		byte[4];
	struct {
		unsigned char	ACRpacketlinespacing		:4;
		unsigned char	ACRpacketsinframe			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb2 (2892_10BAh)    */
union io_f_hdmitx_fc_rdrb2{
	unsigned char		byte[4];
	struct {
		unsigned char	AUDIframeinterpolation		:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb3 (2892_10BBh)    */
union io_f_hdmitx_fc_rdrb3{
	unsigned char		byte[4];
	struct {
		unsigned char	AUDIpacketlinespacing		:4;
		unsigned char	AUDIpacketsinframe			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb4 (2892_10BCh)    */
union io_f_hdmitx_fc_rdrb4{
	unsigned char		byte[4];
	struct {
		unsigned char	GCPframeinterpolation		:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb5 (2892_10BDh)    */
union io_f_hdmitx_fc_rdrb5{
	unsigned char		byte[4];
	struct {
		unsigned char	GCPpacketlinespacing		:4;
		unsigned char	GCPpacketsinframe			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb6 (2892_10BEh)    */
union io_f_hdmitx_fc_rdrb6{
	unsigned char		byte[4];
	struct {
		unsigned char	AVIframeinterpolation		:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb7 (2892_10BFh)    */
union io_f_hdmitx_fc_rdrb7{
	unsigned char		byte[4];
	struct {
		unsigned char	AVIpacketlinespacing		:4;
		unsigned char	AVIpacketsinframe			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb8 (2892_10C0h)    */
union io_f_hdmitx_fc_rdrb8{
	unsigned char		byte[4];
	struct {
		unsigned char	AMPframeinterpolation		:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb9 (2892_10C1h)    */
union io_f_hdmitx_fc_rdrb9{
	unsigned char		byte[4];
	struct {
		unsigned char	AMPpacketlinespacing		:4;
		unsigned char	AMPpacketsinframe			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb10 (2892_10C2h)    */
union io_f_hdmitx_fc_rdrb10{
	unsigned char		byte[4];
	struct {
		unsigned char	NVBIframeinterpolation		:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_rdrb11 (2892_10C3h)    */
union io_f_hdmitx_fc_rdrb11{
	unsigned char		byte[4];
	struct {
		unsigned char	NVBIpacketlinespacing		:4;
		unsigned char	NVBIpacketsinframe			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_prconf (2892_10E0h)    */
union io_f_hdmitx_fc_prconf{
	unsigned char		byte[4];
	struct {
		unsigned char	output_pr_factor			:4;
		unsigned char	incoming_pr_factor			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_scrambler_ctrl (2892_10E1h)    */
union io_f_hdmitx_fc_scrambler_ctrl{
	unsigned char		byte[4];
	struct {
		unsigned char	scrambler_on				:1;
		unsigned char								:3;
		unsigned char	scrambler_ucp_line			:1;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_multistream_ctrl (2892_10E2h)    */
union io_f_hdmitx_fc_multistream_ctrl{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_mas_packet_en			:1;
		unsigned char								:7;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_packet_tx_en (2892_10E3h)    */
union io_f_hdmitx_fc_packet_tx_en{
	unsigned char		byte[4];
	struct {
		unsigned char	acr_tx_en					:1;
		unsigned char	gcp_tx_en					:1;
		unsigned char	avi_tx_en					:1;
		unsigned char	audi_tx_en					:1;
		unsigned char	aut_tx_en					:1;
		unsigned char	amp_tx_en					:1;
		unsigned char	nvbi_tx_en					:1;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_actspc_hdlr_cfg (2892_10E8h)    */
union io_f_hdmitx_fc_actspc_hdlr_cfg{
	unsigned char		byte[4];
	struct {
		unsigned char	actspc_hdlr_en				:1;
		unsigned char	actspc_hdlr_tgl				:1;
		unsigned char								:6;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_invact_2d_0 (2892_10E9h)    */
union io_f_hdmitx_fc_invact_2d0{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_invact_2d_0				:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_invact_2d_1 (2892_10EAh)    */
union io_f_hdmitx_fc_invact_2d1{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_invact_2d_1				:4;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_gmd_stat (2892_1100h)    */
union io_f_hdmitx_fc_gmd_stat{
	unsigned char		byte[4];
	struct {
		unsigned char	igmdcurrent_gamut_seq_num	:4;
		unsigned char	igmdpacket_seq				:2;
		unsigned char	igmddnext_field				:1;
		unsigned char	igmdno_crnt_gbd				:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_gmd_en (2892_1101h)    */
union io_f_hdmitx_fc_gmd_en{
	unsigned char		byte[4];
	struct {
		unsigned char	gmdenabletx					:1;
		unsigned char								:7;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_gmd_up (2892_1102h)    */
union io_f_hdmitx_fc_gmd_up{
	unsigned char		byte[4];
	struct {
		unsigned char	gmdupdatepacket				:1;
		unsigned char								:7;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_gmd_conf (2892_1103h)    */
union io_f_hdmitx_fc_gmd_conf{
	unsigned char		byte[4];
	struct {
		unsigned char	gmdpacketlinespacing		:4;
		unsigned char	gmdpacketsinframe			:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_gmd_hb (2892_1104h)    */
union io_f_hdmitx_fc_gmd_hb{
	unsigned char		byte[4];
	struct {
		unsigned char	gmdaffected_gamut_seq_num	:4;
		unsigned char	gmdgbd_profile				:3;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_gmd_pb (2892_1105 - 2892_1120h)    */
union io_f_hdmitx_fc_gmd_pb{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_gmd_pb					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_amp_pb (2892_112A - 2892_1145h)    */
union io_f_hdmitx_fc_amp_pb{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_amp_pb					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of fc_nvbi_pb (2892_114A - 2892_1164h)    */
union io_f_hdmitx_fc_nvbi_pb{
	unsigned char		byte[4];
	struct {
		unsigned char	fc_nvbi_pb					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/* PHYConfiguration Registers */

/*  structure of phy_conf0 (2892_3000h)    */
union io_f_hdmitx_phy_conf0{
	unsigned char		byte[4];
	struct {
		unsigned char								:2;
		unsigned char	enhpdrxsense				:1;
		unsigned char	txpwron						:1;
		unsigned char	pddq						:1;
		unsigned char	svsret						:1;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of phy_stat0 (2892_3004h) / phy_int0 (2892_3005h) / phy_mask0 (2892_3006h) / phy_pol0 (2892_3007h) */
union io_f_hdmitx_phy_stat0{
	unsigned char		byte[4];
	struct {
		unsigned char	TX_PHY_LOCK					:1;
		unsigned char	HPD							:1;
		unsigned char								:2;
		unsigned char	RX_SENSE_0					:1;
		unsigned char	RX_SENSE_1					:1;
		unsigned char	RX_SENSE_2					:1;
		unsigned char	RX_SENSE_3					:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of phy_i2cm_slave (2892_3020h)    */
union io_f_hdmitx_phy_i2cm_slave{
	unsigned char		byte[4];
	struct {
		unsigned char	slaveaddr					:7;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of phy_i2cm_operation (2892_3026h)    */
union io_f_hdmitx_phy_i2cm_operation{
	unsigned char		byte[4];
	struct {
		unsigned char	rd							:1;
		unsigned char								:3;
		unsigned char	wr							:1;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of phy_i2cm_int (2892_3027h)    */
union io_f_hdmitx_phy_i2cm_int{
	unsigned char		byte[4];
	struct {
		unsigned char	done_status					:1;
		unsigned char	done_interrupt				:1;
		unsigned char	done_mask					:1;
		unsigned char	done_pol					:1;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of phy_i2cm_ctlint (2892_3028h)    */
union io_f_hdmitx_phy_i2cm_ctlint{
	unsigned char		byte[4];
	struct {
		unsigned char	arbitration_status			:1;
		unsigned char	arbitration_interrupt		:1;
		unsigned char	arbitration_mask			:1;
		unsigned char	arbitration_pol				:1;
		unsigned char	nack_status					:1;
		unsigned char	nack_interrupt				:1;
		unsigned char	nack_mask					:1;
		unsigned char	nack_pol					:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of phy_i2cm_div (2892_3029h)    */
union io_f_hdmitx_phy_i2cm_div{
	unsigned char		byte[4];
	struct {
		unsigned char								:3;
		unsigned char	fast_std_mode				:1;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of phy_i2cm_softrstz (2892_302Ah)    */
union io_f_hdmitx_phy_i2cm_softrstz{
	unsigned char		byte[4];
	struct {
		unsigned char	i2c_softrstz				:1;
		unsigned char								:7;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* AudioSample Registers */

/*  structure of aud_conf0 (2892_3100h)    */
union io_f_hdmitx_aud_conf0{
	unsigned char		byte[4];
	struct {
		unsigned char	I2S_in_en					:4;
		unsigned char								:1;
		unsigned char	i2s_select					:1;
		unsigned char								:1;
		unsigned char	sw_audio_fifo_rst			:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_conf1 (2892_3101h)    */
union io_f_hdmitx_aud_conf1{
	unsigned char		byte[4];
	struct {
		unsigned char	I2S_width					:5;
		unsigned char	I2S_mode					:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_int (2892_3102h)    */
union io_f_hdmitx_aud_int{
	unsigned char		byte[4];
	struct {
		unsigned char								:2;
		unsigned char	fifo_full_mask				:1;
		unsigned char	fifo_empty_mask				:1;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_conf2 (2892_3103h)    */
union io_f_hdmitx_aud_conf2{
	unsigned char		byte[4];
	struct {
		unsigned char	HBR							:1;
		unsigned char	NLPCM						:1;
		unsigned char								:6;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_int1 (2892_3104h)    */
union io_f_hdmitx_aud_int1{
	unsigned char		byte[4];
	struct {
		unsigned char								:4;
		unsigned char	fifo_overrun_mask			:1;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* AudioPacketizer Registers */

/*  structure of aud_n3 (2892_3202h)    */
union io_f_hdmitx_aud_n3{
	unsigned char		byte[4];
	struct {
		unsigned char	AudN						:4;
		unsigned char								:3;
		unsigned char	ncts_atomic_write			:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_cts3 (2892_3205h)    */
union io_f_hdmitx_aud_cts3{
	unsigned char		byte[4];
	struct {
		unsigned char	AudCTS						:4;
		unsigned char	CTS_manual					:1;
		unsigned char	N_shift						:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_inputclkfs (2892_3206h)    */
union io_f_hdmitx_aud_inputclkfs{
	unsigned char		byte[4];
	struct {
		unsigned char	ifsfactor					:3;
		unsigned char								:5;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/* AudioSampleSPDIF Registers */

/*  structure of aud_spdif0 (2892_3300h)    */
union io_f_hdmitx_aud_spdif0{
	unsigned char		byte[4];
	struct {
		unsigned char								:7;
		unsigned char	sw_audio_fifo_rst			:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_spdif1 (2892_3301h)    */
union io_f_hdmitx_aud_spdif1{
	unsigned char		byte[4];
	struct {
		unsigned char	spdif_width					:5;
		unsigned char								:1;
		unsigned char	spdif_hbr_mode				:1;
		unsigned char	setnlpcm					:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_spdifint (2892_3302h)    */
union io_f_hdmitx_aud_spdifint{
	unsigned char		byte[4];
	struct {
		unsigned char								:2;
		unsigned char	spdif_fifo_full_mask		:1;
		unsigned char	spdif_fifo_empty_mask		:1;
		unsigned char								:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of aud_spdifint1 (2892_3303h)    */
union io_f_hdmitx_aud_spdifint1{
	unsigned char		byte[4];
	struct {
		unsigned char								:4;
		unsigned char	fifo_overrun_mask			:1;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* MainController Registers */

/*  structure of mc_clkdis (2892_4001h)    */
union io_f_hdmitx_mc_clkdis{
	unsigned char		byte[4];
	struct {
		unsigned char	pixelclk_disable			:1;
		unsigned char	tmdsclk_disable				:1;
		unsigned char	prepclk_disable				:1;
		unsigned char	audclk_disable				:1;
		unsigned char	cscclk_disable				:1;
		unsigned char	cecclk_disable				:1;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of mc_swrstzreq (2892_4002h)    */
union io_f_hdmitx_mc_swrstzreq{
	unsigned char		byte[4];
	struct {
		unsigned char	pixelswrst_req				:1;
		unsigned char	tmdsswrst_req				:1;
		unsigned char	prepswrst_req				:1;
		unsigned char	ii2sswrst_req				:1;
		unsigned char	ispdifswrst_req				:1;
		unsigned char								:1;
		unsigned char	cecswrst_req				:1;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of mc_flowctrl (2892_4004h)    */
union io_f_hdmitx_mc_flowctrl{
	unsigned char		byte[4];
	struct {
		unsigned char	Feed_through_off			:1;
		unsigned char								:7;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of mc_phyrstz (2892_4005h)    */
union io_f_hdmitx_mc_phyrstz{
	unsigned char		byte[4];
	struct {
		unsigned char	phyrstz						:1;
		unsigned char								:7;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* ColorSpaceConverter Registers */

/*  structure of csc_cfg (2892_4100h)    */
union io_f_hdmitx_csc_cfg{
	unsigned char		byte[4];
	struct {
		unsigned char	decmode						:2;
		unsigned char								:2;
		unsigned char	intmode						:2;
		unsigned char								:1;
		unsigned char	csc_limit					:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of csc_scale (2892_4101h)    */
union io_f_hdmitx_csc_scale{
	unsigned char		byte[4];
	struct {
		unsigned char	cscscale					:2;
		unsigned char								:2;
		unsigned char	csc_color_depth				:4;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* CEC Registers */

/*  structure of cec_ctrl (2892_7D00h)    */
union io_f_hdmitx_cec_ctrl{
	unsigned char		byte[4];
	struct {
		unsigned char	Send						:1;
		unsigned char	frame_typ					:2;
		unsigned char	bc_nack						:1;
		unsigned char	Standby						:1;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_mask (2892_7D02h)    */
union io_f_hdmitx_cec_mask{
	unsigned char		byte[4];
	struct {
		unsigned char	done						:1;
		unsigned char	eom							:1;
		unsigned char	nack						:1;
		unsigned char	arb_lost					:1;
		unsigned char	error_initiator				:1;
		unsigned char	error_flow					:1;
		unsigned char	wakeup						:1;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_addr_l (2892_7D05h)    */
union io_f_hdmitx_cec_addr_l{
	unsigned char		byte[4];
	struct {
		unsigned char	cec_addr_l_0				:1;
		unsigned char	cec_addr_l_1				:1;
		unsigned char	cec_addr_l_2				:1;
		unsigned char	cec_addr_l_3				:1;
		unsigned char	cec_addr_l_4				:1;
		unsigned char	cec_addr_l_5				:1;
		unsigned char	cec_addr_l_6				:1;
		unsigned char	cec_addr_l_7				:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_addr_h (2892_7D06h)    */
union io_f_hdmitx_cec_addr_h{
	unsigned char		byte[4];
	struct {
		unsigned char	cec_addr_h_0				:1;
		unsigned char	cec_addr_h_1				:1;
		unsigned char	cec_addr_h_2				:1;
		unsigned char	cec_addr_h_3				:1;
		unsigned char	cec_addr_h_4				:1;
		unsigned char	cec_addr_h_5				:1;
		unsigned char	cec_addr_h_6				:1;
		unsigned char	cec_addr_h_7				:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_tx_cnt (2892_7D07h)    */
union io_f_hdmitx_cec_tx_cnt{
	unsigned char		byte[4];
	struct {
		unsigned char	cec_tx_cnt					:5;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_rx_cnt (2892_7D08h)    */
union io_f_hdmitx_cec_rx_cnt{
	unsigned char		byte[4];
	struct {
		unsigned char	cec_rx_cnt					:5;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_tx_data (2892_7D10 - 2892_7D1Fh)    */
union io_f_hdmitx_cec_tx_data{
	unsigned char		byte[4];
	struct {
		unsigned char	cec_tx_data					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_rx_data (2892_7D20 - 2892_7D2Fh)    */
union io_f_hdmitx_cec_rx_data{
	unsigned char		byte[4];
	struct {
		unsigned char	cec_rx_data					:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_lock (2892_7D30h)    */
union io_f_hdmitx_cec_lock{
	unsigned char		byte[4];
	struct {
		unsigned char	locked_buffer				:1;
		unsigned char								:7;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of cec_wakeupctrl (2892_7D31h)    */
union io_f_hdmitx_cec_wakeupctrl{
	unsigned char		byte[4];
	struct {
		unsigned char	opcode0x04en				:1;
		unsigned char	opcode0x0Den				:1;
		unsigned char	opcode0x41en				:1;
		unsigned char	opcode0x42en				:1;
		unsigned char	opcode0x44en				:1;
		unsigned char	opcode0x70en				:1;
		unsigned char	opcode0x82en				:1;
		unsigned char	opcode0x86en				:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};


/* EDDC Registers */

/*  structure of i2cm_slave (2892_7E00h)    */
union io_f_hdmitx_i2cm_slave{
	unsigned char		byte[4];
	struct {
		unsigned char	Slaveaddr					:7;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_operation (2892_7E04h)    */
union io_f_hdmitx_i2cm_operation{
	unsigned char		byte[4];
	struct {
		unsigned char	rd							:1;
		unsigned char	rd_ext						:1;
		unsigned char	rd8							:1;
		unsigned char	rd8_ext						:1;
		unsigned char	Wr							:1;
		unsigned char								:3;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_int (2892_7E05h)    */
union io_f_hdmitx_i2cm_int{
	unsigned char		byte[4];
	struct {
		unsigned char								:2;
		unsigned char	done_mask					:1;
		unsigned char								:3;
		unsigned char	read_req_mask				:1;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_ctlint (2892_7E06h)    */
union io_f_hdmitx_i2cm_ctlint{
	unsigned char		byte[4];
	struct {
		unsigned char								:2;
		unsigned char	arbitration_mask			:1;
		unsigned char								:3;
		unsigned char	nack_mask					:1;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_div (2892_7E07h)    */
union io_f_hdmitx_i2cm_div{
	unsigned char		byte[4];
	struct {
		unsigned char								:3;
		unsigned char	fast_std_mode				:1;
		unsigned char								:3;
		unsigned char	busclear					:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_segaddr (2892_7E08h)    */
union io_f_hdmitx_i2cm_segaddr{
	unsigned char		byte[4];
	struct {
		unsigned char	seg_addr					:7;
		unsigned char								:1;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_softrstz (2892_7E09h)    */
union io_f_hdmitx_i2cm_softrstz{
	unsigned char		byte[4];
	struct {
		unsigned char	i2c_softrstz				:1;
		unsigned char								:7;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_scdc_read_update (2892_7E14h)    */
union io_f_hdmitx_i2cm_scdc_read_update{
	unsigned char		byte[4];
	struct {
		unsigned char	read_update					:1;
		unsigned char								:3;
		unsigned char	read_request_en				:1;
		unsigned char	updtrd_vsyncpoll_en			:1;
		unsigned char								:2;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_read_buff (2892_7E20 - 2892_7E27h)    */
union io_f_hdmitx_i2cm_read_buff{
	unsigned char		byte[4];
	struct {
		unsigned char	i2cm_read_buff				:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of i2cm_scdc_update (2892_7E30 - 2892_7E31h)    */
union io_f_hdmitx_i2cm_scdc_update{
	unsigned char		byte[4];
	struct {
		unsigned char	i2cm_scdc_update			:8;
		unsigned char								:8;
		unsigned char								:8;
		unsigned char								:8;
	}bit;
};

/*  structure of csc_coef (2892_(4102 - 4119)h) */
struct io_f_hdmi_csc_coef{
	unsigned char	msb;		/* Color Space Converter Matrix Coefficient Register MSB */
	unsigned char		dmy_f_hdmitx1[3];
	unsigned char	lsb;		/* Color Space Converter Matrix Coefficient Register LSB */
	unsigned char		dmy_f_hdmitx2[3];
};

/* Define i/o mapping */
struct io_f_hdmitx2p0_lp{
	/* F_HDMITX2p0_LP */
	union  io_f_hdmitx_pwrctrl						PWRCTRL;						/* 2892_0000 - 0001h */
	
	unsigned char	dmy_f_hdmitx_0002_0017[0x0018 - 0x0002];						/* 2892_0002 - 0005h */
	
	union  io_f_hdmitx_cpmode_pllcfg				OPMODE_PLLCFG;					/* 2892_0006 - 0007h(2892_0018 - 2892_0019h) */
	
	unsigned char	dmy_f_hdmitx_001A_0023[0x0024 - 0x001A];						/* 2892_0008h */
	
	union  io_f_hdmitx_cksymtxctrl					CKSYMTXCTRL;					/* 2892_0009 - 000Ah(2892_0024 - 2892_0025h) */
	
	unsigned char	dmy_f_hdmitx_0026_0037[0x0038 - 0x0026];						/* 2892_000B - 000Dh */
	
	union  io_f_hdmitx_vlevctrl						VLEVCTRL;						/* 2892_000E - 000Fh(2892_0038 - 2892_0039h) */
	
	unsigned char	dmy_f_hdmitx_003A_003F[0x0040 - 0x003A];						/* 2892_000B - 000Dh */
	
	union  io_f_hdmitx_pllcurrctrl					PLLCURRCTRL;					/* 2892_0010 - 0011h(2892_0040 - 2892_0041h) */
	
	unsigned char	dmy_f_hdmitx_0042_004F[0x0050 - 0x0042];						/* 2892_0012 - 0013h */
	
	union  io_f_hdmitx_pllctrl						PLLCTRL;						/* 2892_0014 - 0015h(2892_0050 - 2892_0051h) */
	
	unsigned char	dmy_f_hdmitx_0052_0053[0x0054 - 0x0052];						/* 2892_000B - 000Dh */
	
	union  io_f_hdmitx_pllgmpctrl					PLLGMPCTRL;						/* 2892_0015 - 0016h(2892_0054 - 2892_0055h) */
	
	unsigned char	dmy_f_hdmitx_0056_0063[0x0064 - 0x0056];						/* 2892_0017 - 0018h */
	
	union  io_f_hdmitx_txterm						TXTERM;							/* 2892_0019 - 001Ah(2892_0064 - 2892_0065h) */
	
	unsigned char	dmy_f_hdmitx_0066_03FF[0x0400 - 0x0066];						/* 2892_001B - 00FFh(2892_006C - 2892_03FCh) */
	
	union  io_f_hdmitx_ih_fc_stat0					ih_fc_stat0;					/* 2892_0100h(2892_0400h) */
	union  io_f_hdmitx_ih_fc_stat1					ih_fc_stat1;					/* 2892_0101h(2892_0404h) */
	union  io_f_hdmitx_ih_fc_stat2					ih_fc_stat2;					/* 2892_0102h(2892_0408h) */
	union  io_f_hdmitx_ih_as_stat0					ih_as_stat0;					/* 2892_0103h(2892_040Ch) */
	union  io_f_hdmitx_ih_phy_stat0					ih_phy_stat0;					/* 2892_0104h(2892_0410h) */
	union  io_f_hdmitx_ih_i2cm_stat0				ih_i2cm_stat0;					/* 2892_0105h(2892_0414h) */
	union  io_f_hdmitx_ih_cec_stat0					ih_cec_stat0;					/* 2892_0106h(2892_0418h) */
	union  io_f_hdmitx_ih_vp_stat0					ih_vp_stat0;					/* 2892_0107h(2892_041Ch) */
	union  io_f_hdmitx_ih_i2cmphy_stat0				ih_i2cmphy_stat0;				/* 2892_0108h(2892_0420h) */
	
	unsigned char	dmy_f_hdmitx_0424_05BF[0x05C0 - 0x0424];						/* 2892_(0109 - 016Fh) */
	
	union  io_f_hdmitx_ih_decode					ih_decode;						/* 2892_0170h(2892_05C0h) */
	
	unsigned char	dmy_f_hdmitx_05C4_05FF[0x0600 - 0x05C4];						/* 2892_(0171 - 017Fh) */
	
	union  io_f_hdmitx_ih_fc_stat0					ih_mute_fc_stat0;				/* 2892_0180h(2892_0600h) */
	union  io_f_hdmitx_ih_fc_stat1					ih_mute_fc_stat1;				/* 2892_0181h(2892_0604h) */
	union  io_f_hdmitx_ih_fc_stat2					ih_mute_fc_stat2;				/* 2892_0182h(2892_0608h) */
	union  io_f_hdmitx_ih_as_stat0					ih_mute_as_stat0;				/* 2892_0183h(2892_060Ch) */
	union  io_f_hdmitx_ih_phy_stat0					ih_mute_phy_stat0;				/* 2892_0184h(2892_0610h) */
	union  io_f_hdmitx_ih_i2cm_stat0				ih_mute_i2cm_stat0;				/* 2892_0185h(2892_0614h) */
	union  io_f_hdmitx_ih_cec_stat0					ih_mute_cec_stat0;				/* 2892_0186h(2892_0618h) */
	union  io_f_hdmitx_ih_vp_stat0					ih_mute_vp_stat0;				/* 2892_0187h(2892_061Ch) */
	union  io_f_hdmitx_ih_i2cmphy_stat0				ih_mute_i2cmphy_stat0;			/* 2892_0188h(2892_0620h) */
	
	unsigned char	dmy_f_hdmitx_0624_07FB[0x07FC - 0x0624];						/* 2892_(0189 - 01FEh) */
	
	union  io_f_hdmitx_ih_mute						ih_mute;						/* 2892_01FFh(2892_07FCh) */
	union  io_f_hdmitx_tx_invid0					tx_invid0;						/* 2892_0200h(2892_0800h) */
	union  io_f_hdmitx_tx_instuffing				tx_instuffing;					/* 2892_0201h(2892_0804h) */
	unsigned char									tx_gydata0;						/* 2892_0202h(2892_0808h) */
	unsigned char	dmy_f_hdmitx2892_0808[3];
	unsigned char									tx_gydata1;						/* 2892_0203h(2892_080Ch) */
	unsigned char	dmy_f_hdmitx2892_080C[3];
	unsigned char									tx_rcrdata0;					/* 2892_0204h(2892_0810h) */
	unsigned char	dmy_f_hdmitx2892_0810[3];
	unsigned char									tx_rcrdata1;					/* 2892_0205h(2892_0814h) */
	unsigned char	dmy_f_hdmitx2892_0814[3];
	unsigned char									tx_bcbdata0;					/* 2892_0206h(2892_0818h) */
	unsigned char	dmy_f_hdmitx2892_0818[3];
	unsigned char									tx_bcbdata1;					/* 2892_0207h(2892_081Ch) */
	unsigned char	dmy_f_hdmitx2892_081C[3];
	
	unsigned char	dmy_f_hdmitx_0820_1FFF[0x2000 - 0x0820];						/* 2892_(0208 - 07FFh) */
	
	union  io_f_hdmitx_vp_status					vp_status;						/* 2892_0800h(2892_2000h) */
	union  io_f_hdmitx_vp_pr_cd						vp_pr_cd;						/* 2892_0801h(2892_2004h) */
	union  io_f_hdmitx_vp_stuff						vp_stuff;						/* 2892_0802h(2892_2008h) */
	union  io_f_hdmitx_vp_remap						vp_remap;						/* 2892_0803h(2892_200Ch) */
	union  io_f_hdmitx_vp_conf						vp_conf;						/* 2892_0804h(2892_2010h) */
	
	unsigned char	dmy_f_hdmitx_2014_201B[0x201C - 0x2014];						/* 2892_(0805 - 0806h) */
	
	union  io_f_hdmitx_vp_mask						vp_mask;						/* 2892_0807h(2892_201Ch) */
	
	unsigned char	dmy_f_hdmitx_2020_3FFF[0x4000 - 0x2020];						/* 2892_(0808 - 0FFFh) */
	
	union  io_f_hdmitx_fc_invidconf					fc_invidconf;					/* 2892_1000h(2892_4000h) */
	union  io_f_hdmitx_fc_inhactiv0					fc_inhactiv0;					/* 2892_1001h(2892_4004h) */
	union  io_f_hdmitx_fc_inhactiv1					fc_inhactiv1;					/* 2892_1002h(2892_4008h) */
	union  io_f_hdmitx_fc_inhblank0					fc_inhblank0;					/* 2892_1003h(2892_400Ch) */
	union  io_f_hdmitx_fc_inhblank1					fc_inhblank1;					/* 2892_1004h(2892_4010h) */
	union  io_f_hdmitx_fc_invactiv0					fc_invactiv0;					/* 2892_1005h(2892_4014h) */
	union  io_f_hdmitx_fc_invactiv1					fc_invactiv1;					/* 2892_1006h(2892_4018h) */
	unsigned char									fc_invblank;					/* 2892_1007h(2892_401Ch) */
	unsigned char	dmy_f_hdmitx2892_401C[3];
	union  io_f_hdmitx_fc_hsyncindelay0				fc_hsyncindelay0;				/* 2892_1008h(2892_4020h) */
	union  io_f_hdmitx_fc_hsyncindelay1				fc_hsyncindelay1;				/* 2892_1009h(2892_4024h) */
	union  io_f_hdmitx_fc_hsyncinwidth0				fc_hsyncinwidth0;				/* 2892_100Ah(2892_4028h) */
	union  io_f_hdmitx_fc_hsyncinwidth1				fc_hsyncinwidth1;				/* 2892_100Bh(2892_402Ch) */
	unsigned char									fc_vsyncindelay;				/* 2892_100Ch(2892_4030h) */
	unsigned char	dmy_f_hdmitx2892_4030[3];
	union  io_f_hdmitx_fc_vsyncinwidth				fc_vsyncinwidth;				/* 2892_100Dh(2892_4034h) */
	unsigned char									fc_infreq0;						/* 2892_100Eh(2892_4038h) */
	unsigned char	dmy_f_hdmitx2892_4038[3];
	unsigned char									fc_infreq1;						/* 2892_100Fh(2892_403Ch) */
	unsigned char	dmy_f_hdmitx2892_403C[3];
	union  io_f_hdmitx_fc_infreq2					fc_infreq2;						/* 2892_1010h(2892_4040h) */
	unsigned char									fc_ctrldur;						/* 2892_1011h(2892_4044h) */
	unsigned char	dmy_f_hdmitx2892_4044[3];
	unsigned char									fc_exctrldur;					/* 2892_1012h(2892_4048h) */
	unsigned char	dmy_f_hdmitx2892_4048[3];
	unsigned char									fc_exctrlspac;					/* 2892_1013h(2892_404Ch) */
	unsigned char	dmy_f_hdmitx2892_404C[3];
	unsigned char									fc_ch0pream;					/* 2892_1014h(2892_4050h) */
	unsigned char	dmy_f_hdmitx2892_4050[3];
	union  io_f_hdmitx_fc_ch1pream					fc_ch1pream;					/* 2892_1015h(2892_4054h) */
	union  io_f_hdmitx_fc_ch2pream					fc_ch2pream;					/* 2892_1016h(2892_4058h) */
	union  io_f_hdmitx_fc_aviconf3					fc_aviconf3;					/* 2892_1017h(2892_405Ch) */
	union  io_f_hdmitx_fc_gcp						fc_gcp;							/* 2892_1018h(2892_4060h) */
	union  io_f_hdmitx_fc_aviconf0					fc_aviconf0;					/* 2892_1019h(2892_4064h) */
	union  io_f_hdmitx_fc_aviconf1					fc_aviconf1;					/* 2892_101Ah(2892_4068h) */
	union  io_f_hdmitx_fc_aviconf2					fc_aviconf2;					/* 2892_101Bh(2892_406Ch) */
	union  io_f_hdmitx_fc_avivid					fc_avivid;						/* 2892_101Ch(2892_4070h) */
	unsigned char									fc_avietb0;						/* 2892_101Dh(2892_4074h) */
	unsigned char	dmy_f_hdmitx2892_4074[3];
	unsigned char									fc_avietb1;						/* 2892_101Eh(2892_4078h) */
	unsigned char	dmy_f_hdmitx2892_4078[3];
	unsigned char									fc_avisbb0;						/* 2892_101Fh(2892_407Ch) */
	unsigned char	dmy_f_hdmitx2892_407C[3];
	unsigned char									fc_avisbb1;						/* 2892_1020h(2892_4080h) */
	unsigned char	dmy_f_hdmitx2892_4080[3];
	unsigned char									fc_avielb0;						/* 2892_1021h(2892_4084h) */
	unsigned char	dmy_f_hdmitx2892_4084[3];
	unsigned char									fc_avielb1;						/* 2892_1022h(2892_4088h) */
	unsigned char	dmy_f_hdmitx2892_4088[3];
	unsigned char									fc_avisrb0;						/* 2892_1023h(2892_408Ch) */
	unsigned char	dmy_f_hdmitx2892_408C[3];
	unsigned char									fc_avisrb1;						/* 2892_1024h(2892_4090h) */
	unsigned char	dmy_f_hdmitx2892_4090[3];
	union  io_f_hdmitx_fc_audiconf0					fc_audiconf0;					/* 2892_1025h(2892_4094h) */
	union  io_f_hdmitx_fc_audiconf1					fc_audiconf1;					/* 2892_1026h(2892_4098h) */
	unsigned char									fc_audiconf2;					/* 2892_1027h(2892_409Ch) */
	unsigned char	dmy_f_hdmitx2892_409C[3];
	union  io_f_hdmitx_fc_audiconf3					fc_audiconf3;					/* 2892_1028h(2892_40A0h) */
	unsigned char									fc_vsdieeeid2;					/* 2892_1029h(2892_40A4h) */
	unsigned char	dmy_f_hdmitx2892_40A4[3];
	union  io_f_hdmitx_fc_vsdsize					fc_vsdsize;						/* 2892_102Ah(2892_40A8h) */
	
	unsigned char	dmy_f_hdmitx_40AC_40BF[0x40C0 - 0x40AC];						/* 2892_(102B - 102Fh) */
	
	unsigned char									fc_vsdieeeid1;					/* 2892_1030h(2892_40C0h) */
	unsigned char	dmy_f_hdmitx2892_40C0[3];
	unsigned char									fc_vsdieeeid0;					/* 2892_1031h(2892_40C4h) */
	unsigned char	dmy_f_hdmitx2892_40C4[3];
	union io_f_hdmitx_fc_vsdpayload					fc_vsdpayload[24];				/* 2892_1032 - 1049h(2892_40C8h - 2892_4124h) */
	union io_f_hdmitx_fc_spdvendorname				fc_spdvendorname[8];			/* 2892_104A - 1051h(2892_4128h - 2892_4144h) */
	union io_f_hdmitx_fc_spdproductname				fc_spdproductname[16];			/* 2892_1052 - 1061h(2892_4148h - 2892_4184h) */
	unsigned char									fc_spddeviceinf;				/* 2892_1062h(2892_4188h) */
	unsigned char	dmy_f_hdmitx2892_4188[3];
	union  io_f_hdmitx_fc_audsconf					fc_audsconf;					/* 2892_1063h(2892_418Ch) */
	union  io_f_hdmitx_fc_audsstat					fc_audsstat;					/* 2892_1064h(2892_4190h) */
	union  io_f_hdmitx_fc_audsv						fc_audsv;						/* 2892_1065h(2892_4194h) */
	union  io_f_hdmitx_fc_audsu						fc_audsu;						/* 2892_1066h(2892_4198h) */
	union  io_f_hdmitx_fc_audschnl0					fc_audschnl0;					/* 2892_1067h(2892_419Ch) */
	unsigned char									fc_audschnl1;					/* 2892_1068h(2892_41A0h) */
	unsigned char	dmy_f_hdmitx2892_41A0[3];
	union  io_f_hdmitx_fc_audschnl2					fc_audschnl2;					/* 2892_1069h(2892_41A4h) */
	union  io_f_hdmitx_fc_audschnl3					fc_audschnl3;					/* 2892_106Ah(2892_41A8h) */
	union  io_f_hdmitx_fc_audschnl4					fc_audschnl4;					/* 2892_106Bh(2892_41ACh) */
	union  io_f_hdmitx_fc_audschnl5					fc_audschnl5;					/* 2892_106Ch(2892_41B0h) */
	union  io_f_hdmitx_fc_audschnl6					fc_audschnl6;					/* 2892_106Dh(2892_41B4h) */
	union  io_f_hdmitx_fc_audschnl7					fc_audschnl7;					/* 2892_106Eh(2892_41B8h) */
	union  io_f_hdmitx_fc_audschnl8					fc_audschnl8;					/* 2892_106Fh(2892_41BCh) */
	
	unsigned char	dmy_f_hdmitx_41C0_41CB[0x41CC - 0x41C0];						/* 2892_(1070 - 1072h) */
	
	union  io_f_hdmitx_fc_ctrlqhigh					fc_ctrlqhigh;					/* 2892_1073h(2892_41CCh) */
	union  io_f_hdmitx_fc_ctrlqlow					fc_ctrlqlow;					/* 2892_1074h(2892_41D0h) */
	unsigned char									fc_acp0;						/* 2892_1075h(2892_41D4h) */
	unsigned char	dmy_f_hdmitx2892_41D4[3];
	
	unsigned char	dmy_f_hdmitx_41D8_4207[0x4208 - 0x41D8];						/* 2892_(1076 - 1081h) */
	
	union io_f_hdmitx_fc_acp						fc_acp[16];						/* 2892_1082 - 1091h(2892_4208h - 2892_4244h) */
	
	union  io_f_hdmitx_fc_iscr1_0					fc_iscr1_0;						/* 2892_1092h(2892_4248h) */
	
	union io_f_hdmitx_fc_iscr1						fc_iscr1[16];					/* 2892_1093 - 10A2h(2892_424Ch - 2892_4288h) */
	
	union io_f_hdmitx_fc_iscr2						fc_iscr2[16];					/* 2892_10A3 - 10B2h(2892_428Ch - 2892_42C8h) */
	
	union  io_f_hdmitx_fc_datauto0					fc_datauto0;					/* 2892_10B3h(2892_42CCh) */
	union  io_f_hdmitx_fc_datauto1					fc_datauto1;					/* 2892_10B4h(2892_42D0h) */
	union  io_f_hdmitx_fc_datauto2					fc_datauto2;					/* 2892_10B5h(2892_42D4h) */
	union  io_f_hdmitx_fc_datman					fc_datman;						/* 2892_10B6h(2892_42D8h) */
	union  io_f_hdmitx_fc_datauto3					fc_datauto3;					/* 2892_10B7h(2892_42DCh) */
	union  io_f_hdmitx_fc_rdrb0						fc_rdrb0;						/* 2892_10B8h(2892_42E0h) */
	union  io_f_hdmitx_fc_rdrb1						fc_rdrb1;						/* 2892_10B9h(2892_42E4h) */
	union  io_f_hdmitx_fc_rdrb2						fc_rdrb2;						/* 2892_10BAh(2892_42E8h) */
	union  io_f_hdmitx_fc_rdrb3						fc_rdrb3;						/* 2892_10BBh(2892_42ECh) */
	union  io_f_hdmitx_fc_rdrb4						fc_rdrb4;						/* 2892_10BCh(2892_42F0h) */
	union  io_f_hdmitx_fc_rdrb5						fc_rdrb5;						/* 2892_10BDh(2892_42F4h) */
	union  io_f_hdmitx_fc_rdrb6						fc_rdrb6;						/* 2892_10BEh(2892_42F8h) */
	union  io_f_hdmitx_fc_rdrb7						fc_rdrb7;						/* 2892_10BFh(2892_42FCh) */
	union  io_f_hdmitx_fc_rdrb8						fc_rdrb8;						/* 2892_10C0h(2892_4300h) */
	union  io_f_hdmitx_fc_rdrb9						fc_rdrb9;						/* 2892_10C1h(2892_4304h) */
	union  io_f_hdmitx_fc_rdrb10					fc_rdrb10;						/* 2892_10C2h(2892_4308h) */
	union  io_f_hdmitx_fc_rdrb11					fc_rdrb11;						/* 2892_10C3h(2892_430Ch) */
	
	unsigned char	dmy_f_hdmitx_4310_4347[0x4348 - 0x4310];						/* 2892_(10C4 - 10D1h) */
	
	union  io_f_hdmitx_ih_fc_stat0					fc_mask0;						/* 2892_10D2h(2892_4348h) */
	
	unsigned char	dmy_f_hdmitx_434C_4357[0x4358 - 0x434C];						/* 2892_(10D3 - 10D5h) */
	
	union  io_f_hdmitx_ih_fc_stat1					fc_mask1;						/* 2892_10D6h(2892_4358h) */
	
	unsigned char	dmy_f_hdmitx_435C_4367[0x4368 - 0x435C];						/* 2892_(10D7 - 10D9h) */
	
	union  io_f_hdmitx_ih_fc_stat2					fc_mask2;						/* 2892_10DAh(2892_4368h) */
	
	unsigned char	dmy_f_hdmitx_436C_437F[0x4380 - 0x436C];						/* 2892_(10DB - 10DFh) */
	
	union  io_f_hdmitx_fc_prconf					fc_prconf;						/* 2892_10E0h(2892_4380h) */
	union  io_f_hdmitx_fc_scrambler_ctrl			fc_scrambler_ctrl;				/* 2892_10E1h(2892_4384h) */
	union  io_f_hdmitx_fc_multistream_ctrl			fc_multistream_ctrl;			/* 2892_10E2h(2892_4388h) */
	union  io_f_hdmitx_fc_packet_tx_en				fc_packet_tx_en;				/* 2892_10E3h(2892_438Ch) */
	
	unsigned char	dmy_f_hdmitx_4390_439F[0x43A0 - 0x4390];						/* 2892_(10E4 - 10E7h) */
	
	union  io_f_hdmitx_fc_actspc_hdlr_cfg			fc_actspc_hdlr_cfg;				/* 2892_10E8h(2892_43A0h) */
	union  io_f_hdmitx_fc_invact_2d0				fc_invact_2d0;					/* 2892_10E9h(2892_43A4h) */
	union  io_f_hdmitx_fc_invact_2d1				fc_invact_2d1;					/* 2892_10EAh(2892_43A8h) */
	
	unsigned char	dmy_f_hdmitx_43AC_43FF[0x4400 - 0x43AC];						/* 2892_(10EB - 10FFh) */
	
	union  io_f_hdmitx_fc_gmd_stat					fc_gmd_stat;					/* 2892_1100h(2892_4400h) */
	union  io_f_hdmitx_fc_gmd_en					fc_gmd_en;						/* 2892_1101h(2892_4404h) */
	union  io_f_hdmitx_fc_gmd_up					fc_gmd_up;						/* 2892_1102h(2892_4408h) */
	union  io_f_hdmitx_fc_gmd_conf					fc_gmd_conf;					/* 2892_1103h(2892_440Ch) */
	union  io_f_hdmitx_fc_gmd_hb					fc_gmd_hb;						/* 2892_1104h(2892_4410h) */

	union  io_f_hdmitx_fc_gmd_pb					fc_gmd_pb[28];					/* 2892_1105 - 1120h(2892_4414h - 2892_4480h) */
	
	unsigned char	dmy_f_hdmitx_4484_449F[0x44A0 - 0x4484];						/* 2892_(1121 - 1127h) */
	
	unsigned char									fc_amp_hb1;						/* 2892_1128h(2892_44A0h) */
	unsigned char	dmy_f_hdmitx2892_44A0[3];
	unsigned char									fc_amp_hb2;						/* 2892_1129h(2892_44A4h) */
	unsigned char	dmy_f_hdmitx2892_44A4[3];
	
	union io_f_hdmitx_fc_amp_pb						fc_amp_pb[28];					/* 2892_112A - 1145h(2892_44A8h - 2892_4514h) */
	
	unsigned char	dmy_f_hdmitx_4518_451F[0x4520 - 0x4518];						/* 2892_(1146 - 1147h) */
	
	unsigned char									fc_nvbi_hb1;					/* 2892_1148h(2892_4520h) */
	unsigned char	dmy_f_hdmitx2892_4520[3];
	unsigned char									fc_nvbi_hb2;					/* 2892_1149h(2892_4524h) */
	unsigned char	dmy_f_hdmitx2892_4524[3];
	
	union io_f_hdmitx_fc_nvbi_pb					fc_nvbi_pb[27];					/* 2892_114A - 1164h(2892_4528h - 2892_4590h) */
	
	unsigned char	dmy_f_hdmitx_4594_BFFF[0xC000 - 0x4594];						/* 2892_(1165 - 2FFFh) */
	
	union  io_f_hdmitx_phy_conf0					phy_conf0;						/* 2892_3000h(2892_C000h) */
	
	unsigned char	dmy_f_hdmitx_C004_C00F[0xC010 - 0xC004];						/* 2892_(3001 - 3003h) */
	
	union  io_f_hdmitx_phy_stat0					phy_stat0;						/* 2892_3004h(2892_C010h) */
	union  io_f_hdmitx_phy_stat0					phy_int0;						/* 2892_3005h(2892_C014h) */
	union  io_f_hdmitx_phy_stat0					phy_mask0;						/* 2892_3006h(2892_C018h) */
	union  io_f_hdmitx_phy_stat0					phy_pol0;						/* 2892_3007h(2892_C01Ch) */
	
	unsigned char	dmy_f_hdmitx_C020_C07F[0xC080 - 0xC020];						/* 2892_(3008 - 301Fh) */
	
	union  io_f_hdmitx_phy_i2cm_slave				phy_i2cm_slave;					/* 2892_3020h(2892_C080h) */
	unsigned char									phy_i2cm_address;				/* 2892_3021h(2892_C084h) */
	unsigned char	dmy_f_hdmitx2892_C084[3];
	unsigned char									phy_i2cm_datao_1;				/* 2892_3022h(2892_C088h) */
	unsigned char	dmy_f_hdmitx2892_C088[3];
	unsigned char									phy_i2cm_datao_0;				/* 2892_3023h(2892_C08Ch) */
	unsigned char	dmy_f_hdmitx2892_C08C[3];
	unsigned char									phy_i2cm_datai_1;				/* 2892_3024h(2892_C090h) */
	unsigned char	dmy_f_hdmitx2892_C090[3];
	unsigned char									phy_i2cm_datai_0;				/* 2892_3025h(2892_C094h) */
	unsigned char	dmy_f_hdmitx2892_C094[3];
	union  io_f_hdmitx_phy_i2cm_operation			phy_i2cm_operation;				/* 2892_3026h(2892_C098h) */
	union  io_f_hdmitx_phy_i2cm_int					phy_i2cm_int;					/* 2892_3027h(2892_C09Ch) */
	union  io_f_hdmitx_phy_i2cm_ctlint				phy_i2cm_ctlint;				/* 2892_3028h(2892_C0A0h) */
	union  io_f_hdmitx_phy_i2cm_div					phy_i2cm_div;					/* 2892_3029h(2892_C0A4h) */
	union  io_f_hdmitx_phy_i2cm_softrstz			phy_i2cm_softrstz;				/* 2892_302Ah(2892_C0A8h) */
	unsigned char									phy_i2cm_ss_scl_hcnt_1_addr;	/* 2892_302Bh(2892_C0ACh) */
	unsigned char	dmy_f_hdmitx2892_C0AC[3];
	unsigned char									phy_i2cm_ss_scl_hcnt_0_addr;	/* 2892_302Ch(2892_C0B0h) */
	unsigned char	dmy_f_hdmitx2892_C0B0[3];
	unsigned char									phy_i2cm_ss_scl_lcnt_1_addr;	/* 2892_302Dh(2892_C0B4h) */
	unsigned char	dmy_f_hdmitx2892_C0B4[3];
	unsigned char									phy_i2cm_ss_scl_lcnt_0_addr;	/* 2892_302Eh(2892_C0B8h) */
	unsigned char	dmy_f_hdmitx2892_C0B8[3];
	unsigned char									phy_i2cm_fs_scl_hcnt_1_addr;	/* 2892_302Fh(2892_C0BCh) */
	unsigned char	dmy_f_hdmitx2892_C0BC[3];
	unsigned char									phy_i2cm_fs_scl_hcnt_0_addr;	/* 2892_3030h(2892_C0C0h) */
	unsigned char	dmy_f_hdmitx2892_C0C0[3];
	unsigned char									phy_i2cm_fs_scl_lcnt_1_addr;	/* 2892_3031h(2892_C0C4h) */
	unsigned char	dmy_f_hdmitx2892_C0C4[3];
	unsigned char									phy_i2cm_fs_scl_lcnt_0_addr;	/* 2892_3032h(2892_C0C8h) */
	unsigned char	dmy_f_hdmitx2892_C0C8[3];
	unsigned char									phy_i2cm_sda_hold;				/* 2892_3033h(2892_C0CCh) */
	unsigned char	dmy_f_hdmitx2892_C0CC[3];
	
	unsigned char	dmy_f_hdmitx_C0D0_C3FF[0xC400 - 0xC0D0];						/* 2892_(3034 - 30FFh) */
	
	union  io_f_hdmitx_aud_conf0					aud_conf0;						/* 2892_3100h(2892_C400h) */
	union  io_f_hdmitx_aud_conf1					aud_conf1;						/* 2892_3101h(2892_C404h) */
	union  io_f_hdmitx_aud_int						aud_int;						/* 2892_3102h(2892_C408h) */
	union  io_f_hdmitx_aud_conf2					aud_conf2;						/* 2892_3103h(2892_C40Ch) */
	union  io_f_hdmitx_aud_int1						aud_int1;						/* 2892_3104h(2892_C410h) */
	
	unsigned char	dmy_f_hdmitx_C414_C7FF[0xC800 - 0xC414];						/* 2892_(3105 - 31FFh) */
	
	unsigned char									aud_n1;							/* 2892_3200h(2892_C800h) */
	unsigned char	dmy_f_hdmitx2892_C800[3];
	unsigned char									aud_n2;							/* 2892_3201h(2892_C804h) */
	unsigned char	dmy_f_hdmitx2892_C804[3];
	union  io_f_hdmitx_aud_n3						aud_n3;							/* 2892_3202h(2892_C808h) */
	unsigned char									aud_cts1;						/* 2892_3203h(2892_C80Ch) */
	unsigned char	dmy_f_hdmitx2892_C80C[3];
	unsigned char									aud_cts2;						/* 2892_3204h(2892_C810h) */
	unsigned char	dmy_f_hdmitx2892_C810[3];
	union  io_f_hdmitx_aud_cts3						aud_cts3;						/* 2892_3205h(2892_C814h) */
	union  io_f_hdmitx_aud_inputclkfs				aud_inputclkfs;					/* 2892_3206h(2892_C818h) */
	
	unsigned char	dmy_f_hdmitx_C81C_CBFF[0xCC00 - 0xC81C];						/* 2892_(3207 - 32FFh) */
	
	union  io_f_hdmitx_aud_spdif0					aud_spdif0;						/* 2892_3300h(2892_CC00h) */
	union  io_f_hdmitx_aud_spdif1					aud_spdif1;						/* 2892_3301h(2892_CC04h) */
	union  io_f_hdmitx_aud_spdifint					aud_spdifint;					/* 2892_3302h(2892_CC08h) */
	union  io_f_hdmitx_aud_spdifint1				aud_spdifint1;					/* 2892_3303h(2892_CC0Ch) */
	
	unsigned char	dmy_f_hdmitx_2CC10_30003[0x30004 - 0x2CC10];					/* 2892_(3304 - 4000h) */
	
	union  io_f_hdmitx_mc_clkdis					mc_clkdis;						/* 2892_4001h(2893_0004h) */
	union  io_f_hdmitx_mc_swrstzreq					mc_swrstzreq;					/* 2892_4002h(2893_0008h) */
	
	unsigned char	dmy_f_hdmitx_000C_000F[0x0010 - 0x000C];						/* 2892_4003h(2893_000Ch) */
	
	union  io_f_hdmitx_mc_flowctrl					mc_flowctrl;					/* 2892_4004h(2893_0010h) */
	union  io_f_hdmitx_mc_phyrstz					mc_phyrstz;						/* 2892_4005h(2893_0014h) */
	
	unsigned char	dmy_f_hdmitx2893_0018h[4];										/* 2892_4006h(2893_0018h) */
	
	unsigned char	dmy_f_hdmitx_001C_03FF[0x0400 - 0x001C];						/* 2892_(4007h - 40FFh) */
	
	union  io_f_hdmitx_csc_cfg						csc_cfg;						/* 2892_4100h(2893_0400h) */
	union  io_f_hdmitx_csc_scale					csc_scale;						/* 2892_4101h(2893_0404h) */
	struct io_f_hdmi_csc_coef						csc_coef[3][4];					/* 2892_4102h - 4119h(2893_0408h - 2893_0464h) */
	unsigned char									csc_limit_up_msb;				/* 2892_411Ah(2893_0468h) */
	unsigned char	dmy_f_hdmitx2893_0468[3];
	unsigned char									csc_limit_up_lsb;				/* 2892_411Bh(2893_046Ch) */
	unsigned char	dmy_f_hdmitx2893_046C[3];
	unsigned char									csc_limit_dn_msb;				/* 2892_411Ch(2893_0470h) */
	unsigned char	dmy_f_hdmitx2893_0470[3];
	unsigned char									csc_limit_dn_lsb;				/* 2892_411Dh(2893_0474h) */
	unsigned char	dmy_f_hdmitx2893_0474[3];
	
	unsigned char	dmy_f_hdmitx_0478_F3FF[0xF400 - 0x0478];						/* 2892_(411Eh - 7CFFh) */
	
	union  io_f_hdmitx_cec_ctrl						cec_ctrl;						/* 2892_7D00h(2893_F400h) */
	
	unsigned char	dmy_f_hdmitx2893_F404h[4];										/* 2892_7D01h(2893_F404h) */
	
	union  io_f_hdmitx_cec_mask						cec_mask;						/* 2892_7D02h(2893_F408h) */
	
	unsigned char	dmy_f_hdmitx_F40C_F413[0xF414 - 0xF40C];						/* 2892_(7D03h - 7D04h) */
	
	union  io_f_hdmitx_cec_addr_l					cec_addr_l;						/* 2892_7D05h(2893_F414h) */
	union  io_f_hdmitx_cec_addr_h					cec_addr_h;						/* 2892_7D06h(2893_F418h) */
	union  io_f_hdmitx_cec_tx_cnt					cec_tx_cnt;						/* 2892_7D07h(2893_F41Ch) */
	union  io_f_hdmitx_cec_rx_cnt					cec_rx_cnt;						/* 2892_7D08h(2893_F420h) */
	
	unsigned char	dmy_f_hdmitx_F424_F43F[0xF440 - 0xF424];						/* 2892_(7D09h - 7D0Fh) */
	
	union io_f_hdmitx_cec_tx_data					cec_tx_data[16];				/* 2892_7D10h - 7D1Fh(2893_F440h - 2893_F47Ch) */
	union io_f_hdmitx_cec_rx_data					cec_rx_data[16];				/* 2892_7D20h - 7D2Fh(2893_F480h - 2893_F4BCh) */

	union  io_f_hdmitx_cec_lock						cec_lock;						/* 2892_7D30h(2893_F4C0h) */
	union  io_f_hdmitx_cec_wakeupctrl				cec_wakeupctrl;					/* 2892_7D31h(2893_F4C4h) */
	
	unsigned char	dmy_f_hdmitx_F4C8_F7FF[0xF800 - 0xF4C8];						/* 2892_(7D32h - 7DFFh) */
	
	union  io_f_hdmitx_i2cm_slave					i2cm_slave;						/* 2892_7E00h(2893_F800h) */
	unsigned char									i2cm_address;					/* 2892_7E01h(2893_F804h) */
	unsigned char	dmy_f_hdmitx2893_F804[3];
	unsigned char									i2cm_datao;						/* 2892_7E02h(2893_F808h) */
	unsigned char	dmy_f_hdmitx2893_F808[3];
	unsigned char									i2cm_datai;						/* 2892_7E03h(2893_F80Ch) */
	unsigned char	dmy_f_hdmitx2893_F80C[3];
	union  io_f_hdmitx_i2cm_operation				i2cm_operation;					/* 2892_7E04h(2893_F810h) */
	union  io_f_hdmitx_i2cm_int						i2cm_int;						/* 2892_7E05h(2893_F814h) */
	union  io_f_hdmitx_i2cm_ctlint					i2cm_ctlint;					/* 2892_7E06h(2893_F818h) */
	union  io_f_hdmitx_i2cm_div						i2cm_div;						/* 2892_7E07h(2893_F81Ch) */
	union  io_f_hdmitx_i2cm_segaddr					i2cm_segaddr;					/* 2892_7E08h(2893_F820h) */
	union  io_f_hdmitx_i2cm_softrstz				i2cm_softrstz;					/* 2892_7E09h(2893_F824h) */
	unsigned char									i2cm_segptr;					/* 2892_7E0Ah(2893_F828h) */
	unsigned char	dmy_f_hdmitx2893_F828[3];
	unsigned char									i2cm_ss_scl_hcnt_1_addr;		/* 2892_7E0Bh(2893_F82Ch) */
	unsigned char	dmy_f_hdmitx2893_F82C[3];
	unsigned char									i2cm_ss_scl_hcnt_0_addr;		/* 2892_7E0Ch(2893_F830h) */
	unsigned char	dmy_f_hdmitx2893_F830[3];
	unsigned char									i2cm_ss_scl_lcnt_1_addr;		/* 2892_7E0Dh(2893_F834h) */
	unsigned char	dmy_f_hdmitx2893_F834[3];
	unsigned char									i2cm_ss_scl_lcnt_0_addr;		/* 2892_7E0Eh(2893_F838h) */
	unsigned char	dmy_f_hdmitx2893_F838[3];
	unsigned char									i2cm_fs_scl_hcnt_1_addr;		/* 2892_7E0Fh(2893_F83Ch) */
	unsigned char	dmy_f_hdmitx2893_F83C[3];
	unsigned char									i2cm_fs_scl_hcnt_0_addr;		/* 2892_7E10h(2893_F840h) */
	unsigned char	dmy_f_hdmitx2893_F840[3];
	unsigned char									i2cm_fs_scl_lcnt_1_addr;		/* 2892_7E11h(2893_F844h) */
	unsigned char	dmy_f_hdmitx2893_F844[3];
	unsigned char									i2cm_fs_scl_lcnt_0_addr;		/* 2892_7E12h(2893_F848h) */
	unsigned char	dmy_f_hdmitx2893_F848[3];
	unsigned char									i2cm_sda_hold;					/* 2892_7E13h(2893_F84Ch) */
	unsigned char	dmy_f_hdmitx2893_F84C[3];
	union  io_f_hdmitx_i2cm_scdc_read_update		i2cm_scdc_read_update;			/* 2892_7E14h(2893_F850h) */
	
	unsigned char	dmy_f_hdmitx_F854_F87F[0xF880 - 0xF854];						/* 2892_(7E15h - 7E1Fh) */
	
	union io_f_hdmitx_i2cm_read_buff				i2cm_read_buff[8];				/* 2892_7E20h - 7E27h(2893_F880h - 2893_F89Ch) */
	
	
	unsigned char	dmy_f_hdmitx_F8A0_F8BF[0xF8C0 - 0xF8A0];						/* 2892_(7E28h - 7E2Fh) */
	
	union io_f_hdmitx_i2cm_scdc_update				i2cm_scdc_update[2];			/* 2892_7E30h - 7E31h(2893_F8C0h - 2893_F8C4h) */

    unsigned char	dmy_2893F8C8_2896FFFC[0x28970000-0x2893F8C8];					/* 2892_7E32 - 2893_FFFFh(2893_F8C8h - 2896_FFFCh) */
};

