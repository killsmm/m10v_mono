/**
 * @file		nf.h
 * @brief		Definition Nand Flash Macro I/O register
 * @note		None
 * @attention	HPNFC IP6017 Revision: 0.21
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _NF_H
#define _NF_H

/*******************/
/* Register Region */
/*******************/
/* a register-region base address is at 0x1910_0000 */

/* structure of cmd_reg0 (0x0000) */
union io_nf_cmd_reg0 {
	unsigned long word; 
	struct {
		unsigned long						:24;
		unsigned long	trd_num				:3;
		unsigned long						:3;
		unsigned long	ct					:2;
	} bit_cmd_dma;		/* @CMD DMA Work Mode */
	struct {
		unsigned long	cmd_type			:16;
		unsigned long	vol_id				:4;
		unsigned long	intr				:1;
		unsigned long	dma_sel				:1;
		unsigned long						:2;
		unsigned long	trd_num				:3;
		unsigned long						:3;
		unsigned long	ct					:2;
	} bit_pio;		/* @PIO Work Mode */
	struct {
		unsigned long						:20;
		unsigned long	intr				:1;
		unsigned long						:3;
		unsigned long	trd_num				:3;
		unsigned long						:3;
		unsigned long	ct					:2;
	} bit_gen;		/* @Generic Work Mode */
};

/* structure of cmd_reg1 (0x0004) */
union io_nf_cmd_reg1 {
	unsigned long word; 
	struct {
		unsigned long	row_address			:24;
		unsigned long	bank				:3;
		unsigned long						:5;
	} bit_pio;		/* @PIO Work Mode */
};

/* structure of cmd_reg2 (0x0008) */
union io_nf_cmd_reg2 {
	unsigned long word;
	struct {
		unsigned long	descriptor_addr_l	:32;
	} bit_cmd_dma;		/* @CMD DMA Work Mode */
	struct {
		unsigned long	mem_addr_ptr_l		:32;
	} bit_pio;		/* @PIO Work Mode */
	struct {
		unsigned long	cmd_val_l			:32;
	} bit_gen;		/* @Generic Work Mode */
};

/* structure of cmd_reg3 (0x000c) */
union io_nf_cmd_reg3 {
	unsigned long word;
	struct {
		unsigned long	descriptor_addr_h	:32;
	} bit_cmd_dma;		/* @CMD DMA Work Mode */
	struct {
		unsigned long	mem_addr_ptr_h		:32;
	} bit_pio;		/* @PIO Work Mode */
	struct {
		unsigned long	cmd_val_h			:32;
	} bit_gen;		/* @Generic Work Mode */
};

/* structure of cmd_status_ptr (0x0010) */
union io_nf_cmd_status_ptr {
	unsigned long word;
	struct {
		unsigned long	thrd_status_sel		:3;
		unsigned long						:29;
	} bit;
};

/* structure of cmd_status (0x0014) */
union io_nf_cmd_status {
	unsigned long word;
    struct {
		unsigned long	descriptor_error	:1;
		unsigned long	ecc_err				:1;
		unsigned long	max_error			:8;
		unsigned long						:1;
		unsigned long	erased_page			:1;
		unsigned long	device_error		:1;
		unsigned long	dqs_error			:1;
		unsigned long	fail				:1;
		unsigned long	complete			:1;
		unsigned long	bus_error			:1;
		unsigned long						:7;
		unsigned long	error_index			:8;
	} bit_cmd_dma;		/* @CMD DMA Work Mode */
    struct {
		unsigned long	cmd_error			:1;
		unsigned long	ecc_err				:1;
		unsigned long	max_error			:8;
		unsigned long						:1;
		unsigned long	erased_page			:1;
		unsigned long	device_error		:1;
		unsigned long	dqs_error			:1;
		unsigned long	fail				:1;
		unsigned long	complete			:1;
		unsigned long	bus_error			:1;
		unsigned long						:7;
		unsigned long	error_index			:8;
	} bit_pio;		/* @PIO Work Mode */
    struct {
		unsigned long	cmd_error			:1;
		unsigned long	ecc_err				:1;
		unsigned long	max_error			:8;
		unsigned long						:1;
		unsigned long						:2;
		unsigned long	dqs_error			:1;
		unsigned long	fail				:1;
		unsigned long	complete			:1;
		unsigned long	bus_error			:1;
		unsigned long						:15;
	} bit_gen;		/* @Generic Work Mode */
};

/* structure of intr_status (0x0110) */
union io_nf_intr_status {
	unsigned long word;
	struct {
		unsigned long						:16;
		unsigned long	__cdma_idle			:1;
		unsigned long	__cdma_terr			:1;
		unsigned long	__ddma_terr			:1;
		unsigned long						:1;
		unsigned long	__cmd_ignore		:1;
		unsigned long	__sdma_trigg		:1;
		unsigned long	__sdma_error		:1;
		unsigned long						:9;
	} bit;
};

/* structure of intr_enable (0x0114) */
union io_nf_intr_enable {
	unsigned long word;
	struct {
		unsigned long						:16;
		unsigned long	cdma_idle_en		:1;
		unsigned long	cdma_terr_en		:1;
		unsigned long	ddma_terr_en		:1;
		unsigned long						:1;
		unsigned long	cmd_ignore_en		:1;
		unsigned long	sdma_trigg_en		:1;
		unsigned long	sdma_error_en		:1;
		unsigned long						:8;
		unsigned long	intr_en				:1;
	} bit;
};

/* structure of ctrl_status (0x0118) */
union io_nf_ctrl_status {
	unsigned long word;
	struct {
		unsigned long	sdma_busy			:1;
		unsigned long	mdma_busy			:1;
		unsigned long	cmd_eng_busy		:1;
		unsigned long	mc_busy				:1;
		unsigned long						:4;
		unsigned long	ctrl_busy			:1;
		unsigned long	init_comp			:1;
		unsigned long						:22;
	} bit;
};

/* structure of trd_status (0x0120) */
union io_nf_trd_status {
	unsigned long word;
	struct {
		unsigned long	trd_busy			:8;
		unsigned long						:24;
	} bit;
};

/* structure of trd_error_intr_status (0x0128) */
union io_nf_trd_error_intr_status {
	unsigned long word;
	struct {
		unsigned long	__trd0_error_stat	:1;
		unsigned long	__trd1_error_stat	:1;
		unsigned long	__trd2_error_stat	:1;
		unsigned long	__trd3_error_stat	:1;
		unsigned long	__trd4_error_stat	:1;
		unsigned long	__trd5_error_stat	:1;
		unsigned long	__trd6_error_stat	:1;
		unsigned long	__trd7_error_stat	:1;
		unsigned long						:24;
	} bit;
};

/* structure of trd_error_intr_en (0x0130) */
union io_nf_trd_error_intr_en {
	unsigned long word;
	struct {
		unsigned long	trd_error_intr_en	:8;
		unsigned long						:24;
	} bit;
};

/* structure of trd_comp_intr_status (0x0138) */
union io_nf_trd_comp_intr_status {
	unsigned long word;
	struct {
		unsigned long	__trd0_comp			:1;
		unsigned long	__trd1_comp			:1;
		unsigned long	__trd2_comp			:1;
		unsigned long	__trd3_comp			:1;
		unsigned long	__trd4_comp			:1;
		unsigned long	__trd5_comp			:1;
		unsigned long	__trd6_comp			:1;
		unsigned long	__trd7_comp			:1;
		unsigned long						:24;
	} bit;
};

/* structure of dma_target_error_l (0x0140) */
union io_nf_dma_target_error_l {
	unsigned long word;
	struct {
		unsigned long	target_err_l		:32;
	} bit;
};

/* structure of boot_status (0x0148) */
union io_nf_boot_status {
	unsigned long word;
	struct {
		unsigned long	bch_err_type		:2;
		unsigned long	axi_err				:1;
		unsigned long	phy_err				:1;
		unsigned long	cpy_id				:2;
		unsigned long	init_err			:1;
		unsigned long	tim_out_err			:1;
		unsigned long						:24;
	} bit;
};

/* structure of trd_timeout_intr_status (0x014c) */
union io_nf_trd_timeout_intr_status {
	unsigned long word;
	struct {
		unsigned long	__trd0_timeout_stat	:1;
		unsigned long	__trd1_timeout_stat	:1;
		unsigned long	__trd2_timeout_stat	:1;
		unsigned long	__trd3_timeout_stat	:1;
		unsigned long	__trd4_timeout_stat	:1;
		unsigned long	__trd5_timeout_stat	:1;
		unsigned long	__trd6_timeout_stat	:1;
		unsigned long	__trd7_timeout_stat	:1;
		unsigned long						:24;
	} bit;
};

/* structure of trd_timeout_intr_en (0x0154) */
union io_nf_trd_timeout_intr_en {
	unsigned long word;
	struct {
		unsigned long	trd_timeout_intr_en	:8;
		unsigned long						:24;
	} bit;
};

/* structure of transfer_cfg_0 (0x0400) */
union io_nf_transfer_cfg_0 {
	unsigned long word;
	struct {
		unsigned long	sector_cnt			:8;
		unsigned long						:8;
		unsigned long	offset				:16;
	} bit;
};

/* structure of transfer_cfg_1 (0x0404) */
union io_nf_transfer_cfg_1 {
	unsigned long word;
	struct {
		unsigned long	sector_size			:16;
		unsigned long	last_sector_size	:16;
	} bit;
};

/* structure of long_polling (0x0408) */
union io_nf_long_polling {
	unsigned long word;
	struct {
		unsigned long	long_polling		:16;
		unsigned long						:16;
	} bit;
};

/* structure of short_polling (0x040c) */
union io_nf_short_polling {
	unsigned long word;
	struct {
		unsigned long	short_polling		:16;
		unsigned long						:16;
	} bit;
};

/* structure of rdst_ctrl_0 (0x0410) */
union io_nf_rdst_ctrl_0 {
	unsigned long word;
	struct {
		unsigned long	rb_enable			:1;
		unsigned long						:15;
		unsigned long	ready_value			:8;
		unsigned long	ready_mask			:8;
	} bit;
};

/* structure of rdst_ctrl_1 (0x0414) */
union io_nf_rdst_ctrl_1 {
	unsigned long word;
	struct {
		unsigned long						:16;
		unsigned long	error_value			:8;
		unsigned long	error_mask			:8;
	} bit;
};

/* structure of lun_status_cmd (0x0418) */
union io_nf_lun_status_cmd {
	unsigned long word;
	struct {
		unsigned long	lun_stat_sel		:1;
		unsigned long						:31;
	} bit;
};

/* structure of lun_interleaved_cmd (0x041c) */
union io_nf_lun_interleaved_cmd {
	unsigned long word;
	struct {
		unsigned long						:6;
		unsigned long	program_after_read	:1;
		unsigned long						:25;
	} bit;
};

/* structure of lun_addr_offset (0x0420) */
union io_nf_lun_addr_offset {
	unsigned long word;
	struct {
		unsigned long	lun_addr_offset		:5;
		unsigned long						:27;
	} bit;
};

/* structure of nf_dev_layout (0x0424) */
union io_nf_nf_dev_layout {
	unsigned long word;
	struct {
		unsigned long	ppb					:16;
		unsigned long	lun_en				:1;
		unsigned long						:3;
		unsigned long	ln					:4;
		unsigned long						:8;
	} bit;
};

/* structure of nf_device_areas (0x0428) */
union io_nf_nf_device_areas {
	unsigned long word;
	struct {
		unsigned long	main_area_size		:16;
		unsigned long	spare_area_size		:16;
	} bit;
};

/* structure of ecc_config_0 (0x042c) */
union io_nf_ecc_config_0 {
	unsigned long word;
	struct {
		unsigned long	ecc_enable			:1;
		unsigned long	erase_det_en		:1;
		unsigned long						:2;
		unsigned long	scrambler_en		:1;
		unsigned long						:3;
		unsigned long	corr_str			:3;
		unsigned long						:21;
	} bit;
};

/* structure of ecc_config_1 (0x0430) */
union io_nf_ecc_config_1 {
	unsigned long word;
	struct {
		unsigned long	erase_det_lvl		:8;
		unsigned long						:24;
	} bit;
};

/* structure of device_ctrl (0x0434) */
union io_nf_device_ctrl {
	unsigned long word;
	struct {
		unsigned long						:1;
		unsigned long	ce_pin_reduct		:1;
		unsigned long						:1;
		unsigned long	cont_on_err			:1;
		unsigned long	time_out_en			:1;
		unsigned long						:27;
	} bit;
};

/* structure of multiplane_config (0x0438) */
union io_nf_multiplane_config {
	unsigned long word;
	struct {
		unsigned long	mpl_rd_en			:1;
		unsigned long	mpl_wr_en			:1;
		unsigned long						:6;
		unsigned long	mpl_pl_num			:2;
		unsigned long						:6;
		unsigned long	mpl_prg_seq			:2;
		unsigned long						:4;
		unsigned long	mpl_rd_seq			:2;
		unsigned long	mpl_erase_seq		:1;
		unsigned long	last_wr_cmd			:1;
		unsigned long						:6;
	} bit;
};

/* structure of cache_config (0x043c) */
union io_nf_cache_config {
	unsigned long word;
	struct {
		unsigned long	cache_rd_en			:1;
		unsigned long	cache_wr_en			:1;
		unsigned long						:30;
	} bit;
};

/* structure of dma_settings (0x0440) */
union io_nf_dma_settings {
	unsigned long word;
	struct {
		unsigned long	burst_sel			:8;
		unsigned long						:8;
		unsigned long	ote					:1;
		unsigned long	sdma_err_rsp		:1;
		unsigned long						:14;
	} bit;
};

/* structure of sdma_size (0x0444) */
union io_nf_sdma_size {
	unsigned long word;
	struct {
		unsigned long	sdma_size			:32;
	} bit;
};

/* structure of sdma_trd_num (0x0448) */
union io_nf_sdma_trd_num {
	unsigned long word;
	struct {
		unsigned long	dma_trd				:3;
		unsigned long						:29;
	} bit;
};

/* structure of time_out (0x044c) */
union io_nf_time_out {
	unsigned long word;
	struct {
		unsigned long	time_out_val		:32;
	} bit;
};

/* structure of sdma_addr0 (0x0450) */
union io_nf_sdma_addr0 {
	unsigned long word;
	struct {
		unsigned long	sdma_addr_l			:32;
	} bit;
};

/* structure of ctrl_version (0x0800) */
union io_nf_ctrl_version {
	unsigned long word;
	struct {
		unsigned long	ctrl_rev			:8;
		unsigned long	ctrl_fix			:8;
		unsigned long						:16;
	} bit;
};

/* structure of ctrl_features_reg (0x0804) */
union io_nf_ctrl_features_reg {
	unsigned long word;
	struct {
		unsigned long	n_threads			:4;
		unsigned long						:12;
		unsigned long	boot_available		:1;
		unsigned long	ecc_available		:1;
		unsigned long	dma_intf			:2;
		unsigned long	dma_addr_width		:1;
		unsigned long	dma_data_width		:1;
		unsigned long	sfr_intf			:2;
		unsigned long	n_banks				:2;
		unsigned long	async_supp			:1;
		unsigned long	nvddr				:1;
		unsigned long	nvddr2_3			:1;
		unsigned long	nf_16b_supp			:1;
		unsigned long						:2;
	} bit;
};

/* structure of manufacturer_id (0x0808) */
union io_nf_manufacturer_id {
	unsigned long word;
	struct {
		unsigned long	mid					:8;
		unsigned long						:8;
		unsigned long	did					:8;
		unsigned long						:8;
	} bit;
};

/* structure of device_params_0 (0x080c) */
union io_nf_device_params_0 {
	unsigned long word;
	struct {
		unsigned long	no_of_luns			:8;
		unsigned long	plane_addr_bits		:8;
		unsigned long						:14;
		unsigned long	device_type			:2;
	} bit;
};

/* structure of device_params_1 (0x0810) */
union io_nf_device_params_1 {
	unsigned long word;
	struct {
		unsigned long	readid_3			:8;
		unsigned long	readid_4			:8;
		unsigned long	readid_5			:8;
		unsigned long	readid_6			:8;
	} bit;
};

/* structure of device_features (0x0814) */
union io_nf_device_features {
	unsigned long word;
	struct {
		unsigned long	device_features		:16;
		unsigned long	optional_commands	:16;
	} bit;
};

/* structure of device_blocks_per_lun (0x0818) */
union io_nf_device_blocks_per_lun {
	unsigned long word;
	struct {
		unsigned long	no_of_blocks		:32;
	} bit;
};

/* structure of device_revision (0x081c) */
union io_nf_device_revision {
	unsigned long word;
	struct {
		unsigned long	revisions			:16;
		unsigned long						:16;
	} bit;
};
/* structure of onfi_timing_modes_0 (0x0820) */
union io_nf_onfi_timing_modes_0 {
	unsigned long word;
	struct {
		unsigned long	sdr_modes			:16;
		unsigned long	nv_ddr_modes		:8;
		unsigned long						:8;
	} bit;
};

/* structure of onfi_timing_modes_1 (0x0824) */
union io_nf_onfi_timing_modes_1 {
	unsigned long word;
	struct {
		unsigned long	nv_ddr2_modes		:16;
		unsigned long	nv_ddr3_modes		:16;
	} bit;
};

/* structure of onfi_iterlv_op_attr (0x0828) */
union io_nf_onfi_iterlv_op_attr {
	unsigned long word;
	struct {
		unsigned long	iterlv_op			:8;
		unsigned long						:24;
	} bit;
};

/* structure of onfi_sync_opt_0 (0x082c) */
union io_nf_onfi_sync_opt_0 {
	unsigned long word;
	struct {
		unsigned long	nvddr_supp_ft		:8;
		unsigned long						:8;
		unsigned long	onfi_tccs_min		:16;
	} bit;
};

/* structure of onfi_sync_opt_1 (0x0830) */
union io_nf_onfi_sync_opt_1 {
	unsigned long word;
	struct {
		unsigned long	adv_cmd_supp		:8;
		unsigned long						:8;
		unsigned long	nvddr2_3_features	:8;
		unsigned long	warmup_cycles		:8;
	} bit;
};

/* structure of bch_cfg_0 (0x0834) */
union io_nf_bch_cfg_0 {
	unsigned long word;
	struct {
		unsigned long	bch_corr_0			:8;
		unsigned long	bch_corr_1			:8;
		unsigned long	bch_corr_2			:8;
		unsigned long	bch_corr_3			:8;
	} bit;
};

/* structure of bch_cfg_1 (0x0838) */
union io_nf_bch_cfg_1 {
	unsigned long word;
	struct {
		unsigned long	bch_corr_4			:8;
		unsigned long	bch_corr_5			:8;
		unsigned long	bch_corr_6			:8;
		unsigned long	bch_corr_7			:8;
	} bit;
};

/* structure of bch_cfg_2 (0x083c) */
union io_nf_bch_cfg_2 {
	unsigned long word;
	struct {
		unsigned long	bch_sect_0			:16;
		unsigned long	bch_sect_1			:16;
	} bit;
};

/* structure of bch_cfg_3 (0x0840) */
union io_nf_bch_cfg_3 {
	unsigned long word;
	struct {
		unsigned long	bch_brlk_factor		:8;
		unsigned long	bch_chien_factor	:8;
		unsigned long	bch_metadata_size	:8;
		unsigned long						:8;
	} bit;
};

/* structure of wp_settings (0x1000) */
union io_nf_wp_settings {
	unsigned long word;
	struct {
		unsigned long	wp					:1;
		unsigned long						:31;
	} bit;
};

/* structure of rbn_settings (0x1004) */
union io_nf_rbn_settings {
	unsigned long word;
	struct {
		unsigned long	rbn					:2;
		unsigned long						:30;
	} bit;
};

/* structure of common_settings (0x1008) */
union io_nf_common_settings {
	unsigned long word;
	struct {
		unsigned long	opr_mode			:2;
		unsigned long						:6;
		unsigned long	device_16bit		:1;
		unsigned long						:7;
		unsigned long	rd_warmup			:4;
		unsigned long	wr_warmup			:4;
		unsigned long						:8;
	} bit;
};

/* structure of skip_bytes_conf (0x100c) */
union io_nf_skip_bytes_conf {
	unsigned long word;
	struct {
		unsigned long	skip_bytes			:8;
		unsigned long						:8;
		unsigned long	maker				:16;
	} bit;
};

/* structure of skip_bytes_offset (0x1010) */
union io_nf_skip_bytes_offset {
	unsigned long word;
	struct {
		unsigned long	skip_bytes_offset	:24;
		unsigned long						:8;
	} bit;
};

/* structure of toggle_timings_0 (0x1014) */
union io_nf_toggle_timings_0 {
	unsigned long word;
	struct {
		unsigned long	tpsth				:6;
		unsigned long						:2;
		unsigned long	tcdqss				:6;
		unsigned long						:2;
		unsigned long	tpre				:6;
		unsigned long						:2;
		unsigned long	tcr					:6;
		unsigned long						:2;
	} bit;
};

/* structure of toggle_timings_1 (0x1018) */
union io_nf_toggle_timings_1 {
	unsigned long word;
	struct {
		unsigned long	twpst				:6;
		unsigned long						:2;
		unsigned long	trpst				:6;
		unsigned long						:2;
		unsigned long	tcres				:6;
		unsigned long						:2;
		unsigned long	tcdqsh				:6;
		unsigned long						:2;
	} bit;
};

/* structure of async_toggle_timings (0x101c) */
union io_nf_async_toggle_timings {
	unsigned long word;
	struct {
		unsigned long	twp					:5;
		unsigned long						:3;
		unsigned long	twh					:5;
		unsigned long						:3;
		unsigned long	trp					:5;
		unsigned long						:3;
		unsigned long	trh					:5;
		unsigned long						:3;
	} bit;
};

/* structure of sync_timings (0x1020) */
union io_nf_sync_timings {
	unsigned long word;
	struct {
		unsigned long	tcad				:6;
		unsigned long						:2;
		unsigned long	twrck				:6;
		unsigned long						:2;
		unsigned long	tckwr				:6;
		unsigned long						:10;
	} bit;
};

/* structure of timings0 (0x1024) */
union io_nf_timings0 {
	unsigned long word;
	struct {
		unsigned long	trhw				:8;
		unsigned long	twhr				:8;
		unsigned long	tccs				:8;
		unsigned long	tadl				:8;
	} bit;
};

/* structure of timings1 (0x1028) */
union io_nf_timings1 {
	unsigned long word;
	struct {
		unsigned long	tvdly				:8;
		unsigned long	tcwaw				:8;
		unsigned long	twb					:8;
		unsigned long	trhz				:8;
	} bit;
};

/* structure of timings2 (0x102c) */
union io_nf_timings2 {
	unsigned long word;
	struct {
		unsigned long	cs_setup_time		:6;
		unsigned long						:2;
		unsigned long	cs_hold_time		:6;
		unsigned long						:2;
		unsigned long	tfeat				:10;
		unsigned long						:6;
	} bit;
};

/* structure of dll_phy_update_cnt (0x1030) */
union io_nf_dll_phy_update_cnt {
	unsigned long word;
	struct {
		unsigned long	resync_cnt			:32;
	} bit;
};

/* structure of dll_phy_ctrl (0x1034) */
union io_nf_dll_phy_ctrl {
	unsigned long word;
	struct {
		unsigned long	resync_idle_cnt		:8;
		unsigned long	resync_high_wait_cnt:4;
		unsigned long						:4;
		unsigned long	extended_rd_mode	:1;
		unsigned long	extended_wr_mode	:1;
		unsigned long						:6;
		unsigned long	dll_rst_n			:1;
		unsigned long	dfi_ctrlupd_req		:1;
		unsigned long						:6;
	} bit;
};

/* structure of phy_dq_timing_reg (0x2000) */
union io_nf_phy_dq_timing_reg {
	unsigned long word;
	struct {
		unsigned long	data_select_oe_end		:3;
		unsigned long							:1;
		unsigned long	data_select_oe_start	:3;
		unsigned long							:1;
		unsigned long	data_select_tsel_end	:4;
		unsigned long	data_select_tsel_start	:4;
		unsigned long							:16;
	} bit;
};

/* structure of phy_dqs_timing_reg (0x2004) */
union io_nf_phy_dqs_timing_reg {
	unsigned long word;
	struct {
		unsigned long	dqs_select_oe_end		:4;
		unsigned long	dqs_select_oe_start		:4;
		unsigned long	dqs_select_tsel_end		:4;
		unsigned long	dqs_select_tsel_start	:4;
		unsigned long	phony_dqs_sel			:1;
		unsigned long							:3;
		unsigned long	use_phony_dqs			:1;
		unsigned long							:11;
	} bit;
};

/* structure of phy_gate_lpbk_ctrl_reg (0x2008) */
union io_nf_phy_gate_lpbk_ctrl_reg {
	unsigned long word;
	struct {
		unsigned long	gate_cfg				:4;
		unsigned long	gate_cfg_close			:2;
		unsigned long							:2;
		unsigned long	lpbk_en					:1;
		unsigned long	lpbk_internal			:1;
		unsigned long	loopback_control		:2;
		unsigned long	lpbk_fail_muxsel		:1;
		unsigned long	lpbk_err_check_timing	:3;
		unsigned long							:3;
		unsigned long	rd_del_sel				:5;
		unsigned long							:8;
	} bit;
};

/* structure of phy_dll_master_ctrl_reg (0x200c) */
union io_nf_phy_dll_master_ctrl_reg {
	unsigned long word;
	struct {
		unsigned long	param_dll_start_point	:7;
		unsigned long							:9;
		unsigned long	param_dll_lock_num		:3;
		unsigned long							:1;
		unsigned long	param_phase_detect_sel	:3;
		unsigned long	param_dll_bypass_mode	:1;
		unsigned long	param_half_clock_mode	:1;
		unsigned long							:7;
	} bit;
};

/* structure of phy_dll_slave_ctrl_reg (0x2010) */
union io_nf_phy_dll_slave_ctrl_reg {
	unsigned long word;
	struct {
		unsigned long	read_dqs_delay			:7;
		unsigned long							:1;
		unsigned long	clk_wr_delay			:7;
		unsigned long							:17;
	} bit;
};

/* structure of phy_obs_reg_0 (0x2014) */
union io_nf_phy_obs_reg_0 {
	unsigned long word;
	struct {
		unsigned long	lpbk_status				:4;
		unsigned long							:4;
		unsigned long	lpbk_dq_data			:16;
		unsigned long							:8;
	} bit;
};

/* structure of phy_dll_obs_reg_0 (0x2018) */
union io_nf_phy_dll_obs_reg_0 {
	unsigned long word;
	struct {
		unsigned long	dll_lock				:1;
		unsigned long	dll_locked_mode			:2;
		unsigned long	dll_unlock_count		:5;
		unsigned long	dll_lock_value			:7;
		unsigned long							:1;
		unsigned long	dll_dec_debug			:8;
		unsigned long	lock_inc_debug			:8;
	} bit;
};

/* structure of phy_dll_obs_reg_1 (0x201c) */
union io_nf_phy_dll_obs_reg_1 {
	unsigned long word;
	struct {
		unsigned long	decoder_out_rd			:7;
		unsigned long							:9;
		unsigned long	decoder_out_wr			:7;
		unsigned long							:9;
	} bit;
};

/* structure of phy_ctrl_reg (0x2080) */
union io_nf_phy_ctrl_reg {
	unsigned long word;
	struct {
		unsigned long							:4;
		unsigned long	phony_dqs_timing		:5;
		unsigned long							:5;
		unsigned long	sdr_dqs_value			:1;
		unsigned long							:17;
	} bit;
};

/* structure of phy_tsel_reg (0x2084) */
union io_nf_phy_tsel_reg {
	unsigned long word;
	struct {
		unsigned long							:8;
		unsigned long	tsel_rd_value_dqs		:4;
		unsigned long	tsel_off_value_dqs		:4;
		unsigned long	tsel_rd_value_data		:4;
		unsigned long	tsel_off_value_data		:4;
		unsigned long							:8;
	} bit;
};

/* structure of phy_gpio_ctrl (0x2088) */
union io_nf_phy_gpio_ctrl {
	unsigned long word;
	struct {
		unsigned long	phy_gpio_ctrl_value		:32;
	} bit;
};

/* structure of phy_gpio_status (0x208c) */
union io_nf_phy_gpio_status {
	unsigned long word;
	struct {
		unsigned long	phy_gpio_status_value	:32;
	} bit;
};

/* Register Region is started 0x1910_0000 */
struct io_nf_reg { 
	/* Command and Status registers.  offset:0x0000 */
	union io_nf_cmd_reg0				cmd_reg0;						/* 1910_(0000 - 0003h) */
	union io_nf_cmd_reg1				cmd_reg1;						/* 1910_(0004 - 0007h) */
	union io_nf_cmd_reg2				cmd_reg2;						/* 1910_(0008 - 000Bh) */
	union io_nf_cmd_reg3				cmd_reg3;						/* 1910_(000C - 000Fh) */
	union io_nf_cmd_status_ptr			cmd_status_ptr;					/* 1910_(0010 - 0013h) */
	union io_nf_cmd_status				cmd_status;						/* 1910_(0014 - 0017h) */
	unsigned char						dummy_18_10F[0x110 - 0x18];		/* 1910_(0018 - 010Fh) */
	union io_nf_intr_status				intr_status;					/* 1910_(0110 - 0113h) */
	union io_nf_intr_enable				intr_enable;					/* 1910_(0114 - 0117h) */
	union io_nf_ctrl_status				ctrl_status;					/* 1910_(0118 - 011Bh) */
	unsigned char						dummy_11C_11F[0x120 - 0x11C];	/* 1910_(011C - 011Fh) */
	union io_nf_trd_status				trd_status;						/* 1910_(0120 - 0123h) */
	unsigned char						dummy_124_127[0x128 - 0x124];	/* 1910_(0124 - 0127h) */
	union io_nf_trd_error_intr_status	trd_error_intr_status;			/* 1910_(0128 - 012Bh) */
	unsigned char						dummy_12C_12F[0x130 - 0x12C];	/* 1910_(012C - 012Fh) */
	union io_nf_trd_error_intr_en		trd_error_intr_en;				/* 1910_(0130 - 0133h) */
	unsigned char						dummy_134_137[0x138 - 0x134];	/* 1910_(0134 - 0137h) */
	union io_nf_trd_comp_intr_status	trd_comp_intr_status;			/* 1910_(0138 - 013Bh) */
	unsigned char						dummy_13C_13F[0x140 - 0x13C];	/* 1910_(013C - 013Fh) */
	union io_nf_dma_target_error_l		dma_target_error_l;				/* 1910_(0140 - 0143h) */
	unsigned char						dummy_144_147[0x148 - 0x144];	/* 1910_(0144 - 0147h) */
	union io_nf_boot_status				BOOT_STATUS;					/* 1910_(0148 - 014Bh) */
	union io_nf_trd_timeout_intr_status	trd_timeout_intr_status;		/* 1910_(014C - 014Fh) */
	unsigned char						dummy_150_153[0x154 - 0x150];	/* 1910_(0150 - 0153h) */
	union io_nf_trd_timeout_intr_en		trd_timeout_intr_en;			/* 1910_(0154 - 0157h) */
	unsigned char						dummy_158_3FF[0x400 - 0x158];	/* 1910_(0158 - 03FFh) */
	
	/* Config Registers.   offset:0x0400 */
	union io_nf_transfer_cfg_0			transfer_cfg_0;					/* 1910_(0400 - 0403h) */
	union io_nf_transfer_cfg_1			transfer_cfg_1;					/* 1910_(0404 - 0407h) */
	union io_nf_long_polling			long_polling;					/* 1910_(0408 - 040Bh) */
	union io_nf_short_polling			short_polling;					/* 1910_(040C - 040Fh) */
	union io_nf_rdst_ctrl_0				rdst_ctrl_0;					/* 1910_(0410 - 0413h) */
	union io_nf_rdst_ctrl_1				rdst_ctrl_1;					/* 1910_(0414 - 0417h) */
	union io_nf_lun_status_cmd			lun_status_cmd;					/* 1910_(0418 - 041Bh) */
	union io_nf_lun_interleaved_cmd		lun_interleaved_cmd;			/* 1910_(041C - 041Fh) */
	union io_nf_lun_addr_offset			lun_addr_offset;				/* 1910_(0420 - 0423h) */
	union io_nf_nf_dev_layout			nf_dev_layout;					/* 1910_(0424 - 0427h) */
	union io_nf_nf_device_areas			nf_device_areas;				/* 1910_(0428 - 042Bh) */
	union io_nf_ecc_config_0			ecc_config_0;					/* 1910_(042C - 042Fh) */
	union io_nf_ecc_config_1			ecc_config_1;					/* 1910_(0430 - 0433h) */
	union io_nf_device_ctrl				device_ctrl;					/* 1910_(0434 - 0437h) */
	union io_nf_multiplane_config		multiplane_config;				/* 1910_(0438 - 043Bh) */
	union io_nf_cache_config			cache_config;					/* 1910_(043C - 043Fh) */
	union io_nf_dma_settings			dma_settings;					/* 1910_(0440 - 0443h) */
	union io_nf_sdma_size				sdma_size;						/* 1910_(0444 - 0447h) */
	union io_nf_sdma_trd_num			sdma_trd_num;					/* 1910_(0448 - 044Bh) */
	union io_nf_time_out				time_out;						/* 1910_(044C - 044Fh) */
	union io_nf_sdma_addr0				sdma_addr0;						/* 1910_(0450 - 0453h) */
	unsigned char						dummy_454_7FF[0x800 - 0x454];	/* 1910_(0454 - 07FFh) */
	
	/* Controller and Device Parameters.  offset:0x800 */
	union io_nf_ctrl_version			ctrl_version;					/* 1910_(0800 - 0803h) */
	union io_nf_ctrl_features_reg		ctrl_features_reg;				/* 1910_(0804 - 0807h) */
	union io_nf_manufacturer_id			manufacturer_id;				/* 1910_(0808 - 080Bh) */
	union io_nf_device_params_0			device_params_0;				/* 1910_(080C - 080Fh) */
	union io_nf_device_params_1			device_params_1;				/* 1910_(0810 - 0813h) */
	union io_nf_device_features			device_features;				/* 1910_(0814 - 0817h) */
	union io_nf_device_blocks_per_lun	device_blocks_per_lun;			/* 1910_(0818 - 081Bh) */
	union io_nf_device_revision			device_revision;				/* 1910_(081C - 081Fh) */
	union io_nf_onfi_timing_modes_0		onfi_timing_modes_0;			/* 1910_(0820 - 0823h) */
	union io_nf_onfi_timing_modes_1		onfi_timing_modes_1;			/* 1910_(0824 - 0827h) */
	union io_nf_onfi_iterlv_op_attr		onfi_iterlv_op_attr;			/* 1910_(0828 - 082Bh) */
	union io_nf_onfi_sync_opt_0			onfi_sync_opt_0;				/* 1910_(082C - 082Fh) */
	union io_nf_onfi_sync_opt_1			onfi_sync_opt_1;				/* 1910_(0830 - 0833h) */
	union io_nf_bch_cfg_0				bch_cfg_0;						/* 1910_(0834 - 0837h) */
	union io_nf_bch_cfg_1				bch_cfg_1;						/* 1910_(0838 - 083Bh) */
	union io_nf_bch_cfg_2				bch_cfg_2;						/* 1910_(083C - 083Fh) */
	union io_nf_bch_cfg_3				bch_cfg_3;						/* 1910_(0840 - 0843h) */
	unsigned char						dummy_844_FFF[0x1000 - 0x844];	/* 1910_(0844 - 0FFFh) */
	
	/* Minicontroller registers  offset:0x1000 */
	union io_nf_wp_settings				wp_settings;					/* 1910_(1000 - 1003h) */
	union io_nf_rbn_settings			rbn_settings;					/* 1910_(1004 - 1007h) */
	union io_nf_common_settings			common_settings;				/* 1910_(1008 - 100Bh) */
	union io_nf_skip_bytes_conf			skip_bytes_conf;				/* 1910_(100C - 100Fh) */
	union io_nf_skip_bytes_offset		skip_bytes_offset;				/* 1910_(1010 - 1013h) */
	union io_nf_toggle_timings_0		toggle_timings_0;				/* 1910_(1014 - 1017h) */
	union io_nf_toggle_timings_1		toggle_timings_1;				/* 1910_(1018 - 101Bh) */
	union io_nf_async_toggle_timings	async_toggle_timings;			/* 1910_(101C - 101Fh) */
	union io_nf_sync_timings			sync_timings;					/* 1910_(1020 - 1023h) */
	union io_nf_timings0				timings0;						/* 1910_(1024 - 1027h) */
	union io_nf_timings1				timings1;						/* 1910_(1028 - 102Bh) */
	union io_nf_timings2				timings2;						/* 1910_(102C - 102Fh) */
	union io_nf_dll_phy_update_cnt		dll_phy_update_cnt;				/* 1910_(1030 - 1033h) */
	union io_nf_dll_phy_ctrl			dll_phy_ctrl;					/* 1910_(1034 - 1037h) */
	unsigned char						dummy_1038_1FFF[0x2000-0x1038];	/* 1910_(1038 - 1FFFh) */
	
	/* DLL PHY registers. offset:0x2000 */
	union io_nf_phy_dq_timing_reg		phy_dq_timing_reg;				/* 1910_(2000 - 2003h) */
	union io_nf_phy_dqs_timing_reg		phy_dqs_timing_reg;				/* 1910_(2004 - 2007h) */
	union io_nf_phy_gate_lpbk_ctrl_reg	phy_gate_lpbk_ctrl_reg;			/* 1910_(2008 - 200Bh) */
	union io_nf_phy_dll_master_ctrl_reg	phy_dll_master_ctrl_reg;		/* 1910_(200C - 200Fh) */
	union io_nf_phy_dll_slave_ctrl_reg	phy_dll_slave_ctrl_reg;			/* 1910_(2010 - 2013h) */
	union io_nf_phy_obs_reg_0			phy_obs_reg_0;					/* 1910_(2013 - 2017h) */
	union io_nf_phy_dll_obs_reg_0		phy_dll_obs_reg_0;				/* 1910_(2018 - 201Bh) */
	union io_nf_phy_dll_obs_reg_1		phy_dll_obs_reg_1;				/* 1910_(201C - 201Fh) */
	unsigned char 						dummy_2020_207F[0x2080-0x2020];	/* 1910_(2020 - 207Fh) */
	
	/* Control Timing Block registers. offset:0x2080 */
	union io_nf_phy_ctrl_reg			phy_ctrl_reg;					/* 1910_(2080 - 2083h) */
	union io_nf_phy_tsel_reg			phy_tsel_reg;					/* 1910_(2084 - 2087h) */
	union io_nf_phy_gpio_ctrl			phy_gpio_ctrl;					/* 1910_(2088 - 208Bh) */
	union io_nf_phy_gpio_status			phy_gpio_status;				/* 1910_(208C - 208Fh) */
};

extern volatile struct io_nf_reg IO_NF_REG;

#endif//_NF_H
