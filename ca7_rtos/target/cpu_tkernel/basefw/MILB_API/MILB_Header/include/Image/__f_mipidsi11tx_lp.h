/**
 * @file		__f_mipidsi11tx_lp.h
 * @brief		Definition F_MIPIDSI11TX_LP Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*  structure of PWR_UP    (2800_C004h)    */
union io_pwr_up{
    unsigned long       word;
    struct {
        unsigned long   shutdownz   :1;
        unsigned long               :31;
    }bit;
};

/*  structure of CLKMGR_CFG    (2800_C008h)    */
union io_clkmgr_cfg{
    unsigned long       word;
    struct {
        unsigned long   tx_esc_clk_division :8;
        unsigned long   to_clk_division     :8;
        unsigned long                       :16;
    }bit;
};

/*  structure of DPI_VCID    (2800_C00Ch)    */
union io_dpi_vcid{
    unsigned long       word;
    struct {
        unsigned long   dpi_vcid    :2;
        unsigned long               :30;
    }bit;
};

/*  structure of DPI_COLOR_CODING    (2800_C010h)    */
union io_dpi_color_coding{
    unsigned long       word;
    struct {
        unsigned long   dpi_color_coding    :4;
        unsigned long                       :4;
        unsigned long   loosely18_en        :1;
        unsigned long                       :23;
    }bit;
};

/*  structure of DPI_CFG_POL (2800_C014h)    */
union io_dpi_cfg_pol{
    unsigned long       word;
    struct {
        unsigned long   dataen_active_low   :1;
        unsigned long   vsync_active_low    :1;
        unsigned long   hsync_active_low    :1;
        unsigned long   shutd_active_low    :1;
        unsigned long   colorm_acvite_low   :1;
        unsigned long                       :27;
    }bit;
};

/*  structure of DPI_LP_CMD_TIM (2800_C018h)    */
union io_dpi_lp_cmd_tim{
    unsigned long       word;
    struct {
        unsigned long   invact_lpcmd_time   :8;
        unsigned long                       :8;
        unsigned long   outvact_lpcmd_time  :8;
        unsigned long                       :8;
    }bit;
};

/*  structure of PCKHDL_CFG    (2800_C02Ch)    */
union io_pckhdl_cfg{
    unsigned long       word;
    struct {
        unsigned long   eotp_tx_en  :1;
        unsigned long               :31;
    }bit;
};

/*  structure of MODE_CFG    (2800_C034h)    */
union io_mode_cfg{
    unsigned long       word;
    struct {
        unsigned long   cmd_video_mode  :1;
        unsigned long                   :31;
    }bit;
};

/*  structure of VID_MODE_CFG  (2800_C038h)    */
union io_vid_mode_cfg{
    unsigned long       word;
    struct {
        unsigned long   vid_mode_type   :2;
        unsigned long                   :6;
        unsigned long   lp_vsa_en       :1;
        unsigned long   lp_vbp_en       :1;
        unsigned long   lp_vfp_en       :1;
        unsigned long   lp_vact_en      :1;
        unsigned long   lp_hbp_en       :1;
        unsigned long   lp_hfp_en       :1;
        unsigned long                   :1;
        unsigned long   lp_cmd_en       :1;
        unsigned long   vpg_en          :1;
        unsigned long                   :3;
        unsigned long   vpg_mode        :1;
        unsigned long                   :3;
        unsigned long   vpg_orientation :1;
        unsigned long                   :7;
    }bit;
};

/*  structure of VID_PKT_SIZE   (2800_C03Ch)    */
union io_vid_pkt_size{
    unsigned long       word;
    struct {
        unsigned long   vid_pkt_size    :14;
        unsigned long                   :18;
    }bit;
};

/*  structure of VID_NUM_CHUNKS    (2800_C040h)    */
union io_vid_num_chunks{
    unsigned long       word;
    struct {
        unsigned long   vid_num_chunks  :13;
        unsigned long                   :19;
    }bit;
};

/*  structure of VID_NULL_SIZE    (2800_C044h)    */
union io_vid_null_size{
    unsigned long       word;
    struct {
        unsigned long   vid_null_size   :13;
        unsigned long                   :19;
    }bit;
};

/*  structure of VID_HSA_TIME   (2800_C048h)    */
union io_vid_hsa_time{
    unsigned long       word;
    struct {
        unsigned long   vid_hsa_time    :12;
        unsigned long                   :20;
    }bit;
};

/*  structure of VID_HBP_TIME   (2800_C04Ch)    */
union io_vid_hbp_time{
    unsigned long       word;
    struct {
        unsigned long   vid_hbp_time    :12;
        unsigned long                   :20;
    }bit;
};

/*  structure of VID_HLINE_TIME  (2800_C050h)    */
union io_vid_hline_time{
    unsigned long       word;
    struct {
        unsigned long   vid_hline_time  :15;
        unsigned long                   :17;
    }bit;
};

/*  structure of VID_VSA_LINES  (2800_C054h)    */
union io_vid_vsa_lines{
    unsigned long       word;
    struct {
        unsigned long   vsa_lines   :10;
        unsigned long               :22;
    }bit;
};

/*  structure of VID_VBP_LINES  (2800_C058h)    */
union io_vid_vbp_lines{
    unsigned long       word;
    struct {
        unsigned long   vbp_lines   :10;
        unsigned long               :22;
    }bit;
};

/*  structure of VID_VFP_LINES  (2800_C05Ch)    */
union io_vid_vfp_lines{
    unsigned long       word;
    struct {
        unsigned long   vfp_lines   :10;
        unsigned long               :22;
    }bit;
};

/*  structure of VID_VACTIVE_LINES  (2800_C060h)    */
union io_vid_vactive_lines{
    unsigned long       word;
    struct {
        unsigned long   v_active_lines  :14;
        unsigned long                   :18;
    }bit;
};

/*  structure of CMD_MODE_CFG  (2800_C068h)    */
union io_cmd_mode_cfg{
    unsigned long       word;
    struct {
        unsigned long                   :8;
        unsigned long   gen_sw_0p_tx    :1;
        unsigned long   gen_sw_1p_tx    :1;
        unsigned long   gen_sw_2p_tx    :1;
        unsigned long                   :3;
        unsigned long   gen_lw_tx       :1;
        unsigned long                   :1;
        unsigned long   dcs_sw_0p_tx    :1;
        unsigned long   dcs_sw_1p_tx    :1;
        unsigned long                   :1;
        unsigned long   dcs_lw_tx       :1;
        unsigned long                   :12;
    }bit;
};

/*  structure of GEN_HDR  (2800_C06Ch)    */
union io_gen_hdr{
    unsigned long       word;
    struct {
        unsigned long   gen_dt          :6;
        unsigned long   gen_vc          :2;
        unsigned long   gen_wc_lsbyte   :8;
        unsigned long   gen_wc_msbyte   :8;
        unsigned long                   :8;
    }bit;
};

/*  structure of GEN_PLD_DATA  (2800_C070h)    */
union io_gen_pld_data{
    unsigned long       word;
    struct {
        unsigned long   gen_pld_b1  :8;
        unsigned long   gen_pld_b2  :8;
        unsigned long   gen_pld_b3  :8;
        unsigned long   gen_pld_b4  :8;
    }bit;
};

/*  structure of CMD_PKT_STATUS  (2800_C074h)    */
union io_cmd_pkt_status{
    unsigned long       word;
    struct {
        unsigned long   gen_cmd_empty   :1;
        unsigned long   gen_cmd_full    :1;
        unsigned long   gen_pld_w_empty :1;
        unsigned long   gen_pld_w_full  :1;
        unsigned long                   :28;
    }bit;
};

/*  structure of TO_CNT_CFG  (2800_C078h)    */
union io_to_cnt_cfg{
    unsigned long       word;
    struct {
        unsigned long               :16;
        unsigned long   hstx_to_cnt :16;
    }bit;
};

/*  structure of HS_WR_TO_CNT  (2800_C084h)    */
union io_hs_wr_to_cnt{
    unsigned long       word;
    struct {
        unsigned long   hs_wr_to_cnt    :16;
        unsigned long                   :16;
    }bit;
};

/*  structure of LP_WR_TO_CNT  (2800_C088h)    */
union io_lp_wr_to_cnt{
    unsigned long       word;
    struct {
        unsigned long   lp_wr_to_cnt    :16;
        unsigned long                   :16;
    }bit;
};

/*  structure of SDF_3D  (2800_C090h)    */
union io_sdf_3d{
    unsigned long       word;
    struct {
        unsigned long   mode_3d         :2;
        unsigned long   format_3d       :2;
        unsigned long   second_vsync    :1;
        unsigned long   right_first     :1;
        unsigned long                   :10;
        unsigned long   send_3d_cfg     :1;
        unsigned long                   :15;
    }bit;
};

/*  structure of LPCLK_CTRL  (2800_C094h)    */
union io_lpclk_ctrl{
    unsigned long       word;
    struct {
        unsigned long   phy_txrequestclkhs  :1;
        unsigned long   auto_clklane_ctrl   :1;
        unsigned long                       :30;
    }bit;
};

/*  structure of PHY_TMR_LPCLK_CFG  (2800_C098h)    */
union io_phy_tmr_lpclk_cfg{
    unsigned long       word;
    struct {
        unsigned long   phy_clkhs2lp_time   :10;
        unsigned long                       :6;
        unsigned long   phy_clklp2hs_time   :10;
        unsigned long                       :6;
    }bit;
};

/*  structure of PHY_TMR_CFG  (2800_C09Ch)    */
union io_phy_tmr_cfg{
    unsigned long       word;
    struct {
        unsigned long                   :16;
        unsigned long   phy_lp2hs_time  :8;
        unsigned long   phy_hs2lp_time  :8;
    }bit;
};

/*  structure of PHY_RSTZ  (2800_C0A0h)    */
union io_phy_rstz{
    unsigned long       word;
    struct {
        unsigned long   phy_shutdownz   :1;
        unsigned long                   :31;
    }bit;
};

/*  structure of PHY_IF_CFG  (2800_C0A4h)    */
union io_phy_if_cfg{
    unsigned long       word;
    struct {
        unsigned long   n_lanes     :2;
        unsigned long               :30;
    }bit;
};

/*  structure of PHY_ULPS_CTRL  (2800_C0A8h)    */
union io_phy_ulps_ctrl{
    unsigned long       word;
    struct {
        unsigned long   phy_txrequlpsclk    :1;
        unsigned long   phy_txexitulpsclk   :1;
        unsigned long   phy_txrequlpslan    :1;
        unsigned long   phy_txexitulpslan   :1;
        unsigned long                       :28;
    }bit;
};

/*  structure of PHY_TX_TRIGGERS  (2800_C0ACh)    */
union io_phy_tx_triggers{
    unsigned long       word;
    struct {
        unsigned long   phy_tx_triggers     :4;
        unsigned long                       :28;
    }bit;
};

/*  structure of PHY_STATUS  (2800_C0B0h)    */
union io_phy_status{
    unsigned long       word;
    struct {
        unsigned long                           :1;
        unsigned long   phy_direction           :1;
        unsigned long   phy_stopstateclklane    :1;
        unsigned long   phy_ulpsactivenotclk    :1;
        unsigned long   phy_stopstate0lane      :1;
        unsigned long   phy_ulpsactivenot0lane  :1;
        unsigned long                           :1;
        unsigned long   phy_stopstate1lane      :1;
        unsigned long   phy_ulpsactivenot1lane  :1;
        unsigned long   phy_stopstate2lane      :1;
        unsigned long   phy_ulpsactivenot2lane  :1;
        unsigned long   phy_stopstate3lane      :1;
        unsigned long   phy_ulpsactivenot3lane  :1;
        unsigned long                           :19;
    }bit;
};

/*  structure of INT_ST1  (2800_C0C0h)    */
union io_int_st1{
    unsigned long       word;
    struct {
        unsigned long   to_hs_tx            :1;
        unsigned long                       :6;
        unsigned long   dpi_pld_wr_err      :1;
        unsigned long   gen_cmd_wr_err      :1;
        unsigned long   gen_pld_wr_err      :1;
        unsigned long   gen_pld_send_err    :1;
        unsigned long                       :21;
    }bit;
};

/*  structure of INT_MSK1  (2800_C0C8h)    */
union io_int_msk1{
    unsigned long       word;
    struct {
        unsigned long   to_hs_tx            :1;
        unsigned long                       :6;
        unsigned long   dpi_pld_wr_err      :1;
        unsigned long   gen_cmd_wr_err      :1;
        unsigned long   gen_pld_wr_err      :1;
        unsigned long   gen_pld_send_err    :1;
        unsigned long                       :21;
    }bit;
};

/*  structure of VID_SHADOW_CTRL  (2800_C100h)    */
union io_vid_shadow_ctrl{
    unsigned long       word;
    struct {
        unsigned long   vid_shadow_en       :1;
        unsigned long                       :7;
        unsigned long   vid_shadow_req      :1;
        unsigned long                       :7;
        unsigned long   vid_shadow_pin_req  :1;
        unsigned long                       :15;
    }bit;
};


/*  structure of VID_MODE_CFG_ACT  (2800_C038h)    */
union io_vid_mode_cfg_act{
    unsigned long       word;
    struct {
        unsigned long   vid_mode_type   :2;
        unsigned long                   :6;
        unsigned long   lp_vsa_en       :1;
        unsigned long   lp_vbp_en       :1;
        unsigned long   lp_vfp_en       :1;
        unsigned long   lp_vact_en      :1;
        unsigned long   lp_hbp_en       :1;
        unsigned long   lp_hfp_en       :1;
        unsigned long                   :1;
        unsigned long   lp_cmd_en       :1;
        unsigned long                   :16;
    }bit;
};

/*  structure of PHY_SETUP_CL (2800_C400h) / PHY_SETUP_DL (2800_C404h)    */
union io_phy_setup{
    unsigned long       word;
    struct {
        unsigned long   SAP_TLPX    :8;
        unsigned long   SAP_HS0     :8;
        unsigned long   SAP_TRAIL   :8;
        unsigned long   SAP_PRE     :8;
    }bit;
};

/*  structure of PHY_TCLKPOST  (2800_C408h)    */
union io_phy_tclkost{
    unsigned long       word;
    struct {
        unsigned long   TCLK_POST   :6;
        unsigned long               :26;
    }bit;
};


/* Define i/o mapping */
struct io_f_mipidsi11tx_lp{
    /* F_MIPIDSI11TX_LP */
    unsigned long					VERSION;				/* 2800_(C000 - C003h) */
    union io_pwr_up					PWR_UP;					/* 2800_(C004 - C007h) */
    union io_clkmgr_cfg				CLKMGR_CFG;				/* 2800_(C008 - C00Bh) */
    union io_dpi_vcid				DPI_VCID;				/* 2800_(C00C - C00Fh) */
    union io_dpi_color_coding		DPI_COLOR_CODING;		/* 2800_(C010 - C013h) */
    union io_dpi_cfg_pol			DPI_CFG_POL;			/* 2800_(C014 - C017h) */
    union io_dpi_lp_cmd_tim			DPI_LP_CMD_TIM;			/* 2800_(C018 - C01Bh) */

    unsigned char dmy_C01C_C02B[0xC02C-0xC01C]; 			/* 2800_(C01C - C02Bh) */

    union io_pckhdl_cfg				PCKHDL_CFG;				/* 2800_(C02C - C02Fh) */

    unsigned char dmy_C030_C033[0xC034-0xC030]; 			/* 2800_(C030 - C033h) */

    union io_mode_cfg				MODE_CFG;				/* 2800_(C034 - C037h) */
    union io_vid_mode_cfg			VID_MODE_CFG;			/* 2800_(C038 - C03Bh) */
    union io_vid_pkt_size			VID_PKT_SIZE;			/* 2800_(C03C - C03Fh) */
    union io_vid_num_chunks			VID_NUM_CHUNKS;			/* 2800_(C040 - C043h) */
    union io_vid_null_size			VID_NULL_SIZE;			/* 2800_(C044 - C047h) */
    union io_vid_hsa_time			VID_HSA_TIME;			/* 2800_(C048 - C04Bh) */
    union io_vid_hbp_time			VID_HBP_TIME;			/* 2800_(C04C - C04Fh) */
    union io_vid_hline_time			VID_HLINE_TIME;			/* 2800_(C050 - C053h) */
    union io_vid_vsa_lines			VID_VSA_LINES;			/* 2800_(C054 - C057h) */
    union io_vid_vbp_lines			VID_VBP_LINES;			/* 2800_(C058 - C05Bh) */
    union io_vid_vfp_lines			VID_VFP_LINES;			/* 2800_(C05C - C05Fh) */
    union io_vid_vactive_lines		VID_VACTIVE_LINES;		/* 2800_(C060 - C063h) */

    unsigned char dmy_C064_C067[0xC068-0xC064]; 			/* 2800_(C064 - C067h) */

    union io_cmd_mode_cfg			CMD_MODE_CFG;			/* 2800_(C068 - C06Bh) */
    union io_gen_hdr				GEN_HDR;				/* 2800_(C06C - C06Fh) */
    union io_gen_pld_data			GEN_PLD_DATA;			/* 2800_(C070 - C073h) */
    union io_cmd_pkt_status			CMD_PKT_STATUS;			/* 2800_(C074 - C077h) */
    union io_to_cnt_cfg				TO_CNT_CFG;				/* 2800_(C078 - C07Bh) */

    unsigned char dmy_C07C_C083[0xC084-0xC07C]; 			/* 2800_(C07C - C083h) */

    union io_hs_wr_to_cnt			HS_WR_TO_CNT;			/* 2800_(C084 - C087h) */
    union io_lp_wr_to_cnt			LP_WR_TO_CNT;			/* 2800_(C088 - C08Bh) */

    unsigned char dmy_C08C_C08F[0xC090-0xC08C]; 			/* 2800_(C08C - C08Fh) */

    union io_sdf_3d					SDF_3D;					/* 2800_(C090 - C093h) */
    union io_lpclk_ctrl				LPCLK_CTRL;				/* 2800_(C094 - C097h) */
    union io_phy_tmr_lpclk_cfg		PHY_TMR_LPCLK_CFG;		/* 2800_(C098 - C09Bh) */
    union io_phy_tmr_cfg			PHY_TMR_CFG;			/* 2800_(C09C - C09Fh) */
    union io_phy_rstz				PHY_RSTZ;				/* 2800_(C0A0 - C0A3h) */
    union io_phy_if_cfg				PHY_IF_CFG;				/* 2800_(C0A4 - C0A7h) */
    union io_phy_ulps_ctrl			PHY_ULPS_CTRL;			/* 2800_(C0A8 - C0ABh) */
    union io_phy_tx_triggers		PHY_TX_TRIGGERS;		/* 2800_(C0AC - C0AFh) */
    union io_phy_status				PHY_STATUS;				/* 2800_(C0B0 - C0B3h) */

    unsigned char dmy_C0B4_C0BF[0xC0C0-0xC0B4]; 			/* 2800_(C0B4 - C0BFh) */

    union io_int_st1				INT_ST1;				/* 2800_(C0C0 - 0C3Ch) */

    unsigned char dmy_C0C4_C0C7[0xC0C8-0xC0C4]; 			/* 2800_(C0C4 - C0C7h) */

    union io_int_msk1				INT_MSK1;				/* 2800_(C0C8 - C0CBh) */

    unsigned char dmy_C0CC_C0FF[0xC100-0xC0CC]; 			/* 2800_(C0CC - C0FFh) */

    union io_vid_shadow_ctrl		VID_SHADOW_CTRL;		/* 2800_(C100 - C103h) */

    unsigned char dmy_C104_C10B[0xC10C-0xC104]; 			/* 2800_(C104 - C10Bh) */

    union io_dpi_vcid				DPI_VCID_ACT;			/* 2800_(C10C - C10Fh) */
    union io_dpi_color_coding		DPI_COLOR_CODING_ACT;	/* 2800_(C110 - C113h) */

    unsigned char dmy_C114_C117[0xC118-0xC114]; 			/* 2800_(C114 - C117h) */

    union io_dpi_lp_cmd_tim			DPI_LP_CMD_TIM_ACT;		/* 2800_(C118 - C11Bh) */

    unsigned char dmy_C11C_C137[0xC138-0xC11C]; 			/* 2800_(C11C - C137h) */

    union io_vid_mode_cfg_act		VID_MODE_CFG_ACT;		/* 2800_(C138 - C13Bh) */
    union io_vid_pkt_size			VID_PKT_SIZE_ACT;		/* 2800_(C13C - C13Fh) */
    union io_vid_num_chunks			VID_NUM_CHUNKS_ACT;		/* 2800_(C140 - C143h) */
    union io_vid_null_size			VID_NULL_SIZE_ACT;		/* 2800_(C144 - C147h) */
    union io_vid_hsa_time			VID_HSA_TIME_ACT;		/* 2800_(C148 - C14Bh) */
    union io_vid_hbp_time			VID_HBP_TIME_ACT;		/* 2800_(C14C - C14Fh) */
    union io_vid_hline_time			VID_HLINE_TIME_ACT;		/* 2800_(C150 - C153h) */
    union io_vid_vsa_lines			VID_VSA_LINES_ACT;		/* 2800_(C154 - C157h) */
    union io_vid_vbp_lines			VID_VBP_LINES_ACT;		/* 2800_(C158 - C15Bh) */
    union io_vid_vfp_lines			VID_VFP_LINES_ACT;		/* 2800_(C15C - C15Fh) */
    union io_vid_vactive_lines		VID_VACTIVE_LINES_ACT;	/* 2800_(C160 - C163h) */

    unsigned char dmy_C164_C18F[0xC190-0xC164]; 			/* 2800_(C164 - C18Fh) */

    union io_sdf_3d					SDF_3D_ACT;				/* 2800_(C190 - C193h) */

    unsigned char dmy_C194_C3FF[0xC400-0xC194]; 			/* 2800_(C194 - C3FFh) */

    union io_phy_setup				PHY_SETUP_CL;			/* 2800_(C400 - C403h) */
    union io_phy_setup				PHY_SETUP_DL;			/* 2800_(C404 - C407h) */
    union io_phy_tclkost			PHY_TCLKPOST;			/* 2800_(C408 - C40Bh) */

    unsigned char dmy_C40C_CFFF[0xD000-0xC40C];				/* 2800_(C40C - CFFFCh) */
};

