/**
 * @file		im_pro_interrupt.c
 * @brief		pre process interrupt api
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
#define	D_IM_PRO_INTFLG_CLEAR	(1)				// Clear the PRO interrupt flag

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Interrupt parameter table
typedef struct {
	ULONG				flg_bitmask;		// Bitmask of 
	ULONG				ena_bitmask;		// Bitmask of 
	ULONG				int_fact;			// Interrupt factor bitmask. 
} T_IM_PRO_INTFLG_TBL;

typedef struct {
	ULONG	slvs_common_intf;
	ULONG	slvs_intf[D_IM_PRO_SEN_SLVS_CH_NUM];
	ULONG	slvs_leintf[D_IM_PRO_SEN_SLVS_CH_NUM];
	ULONG	slvs_meintf0[D_IM_PRO_SEN_SLVS_CH_NUM];
	ULONG	slvs_meintf1[D_IM_PRO_SEN_SLVS_CH_NUM];
	ULONG	lvds_intf0;
	ULONG	lvds_intf1;
	ULONG	lvds_intf2;
	ULONG	cmipi_intf1;
	ULONG	cmipi_intf2;
	ULONG	dmipi_intf;
	ULONG	moni_intf;
} T_IM_PRO_SENCORE_INT_FACTOR;

typedef struct {
	ULONG	pwch_intf[D_IM_PRO_SRO_PWCH_CH_NUM];
	ULONG	prch_intf[D_IM_PRO_SRO_PRCH_CH_NUM];
	ULONG	sdc_intf[D_IM_PRO_SRO_SDC_CH_NUM];
	ULONG	moni_intf;
} T_IM_PRO_SRO_INT_FACTOR;

typedef struct {
	ULONG	pwch_intf[D_IM_PRO_B2B_PWCH_CH_NUM];
	ULONG	prch_intf[D_IM_PRO_B2B_PRCH_CH_NUM];
} T_IM_PRO_B2B_INT_FACTOR;

typedef struct {
	E_IM_PRO_UNIT_NUM			unit_no;
	E_IM_PRO_BLOCK_TYPE			block_type;
	volatile struct io_pwch*	reg_ptr;
} T_IM_PRO_PWCH_INT_INFO;

typedef struct {
	E_IM_PRO_UNIT_NUM			unit_no;
	E_IM_PRO_BLOCK_TYPE			block_type;
	volatile struct io_prch*	reg_ptr;
} T_IM_PRO_PRCH_INT_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Call back function
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SENTOP_VD_CallBack_Func														= {0};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SENTOP_HD_CallBack_Func														= {0};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SENTOP_Gyro_CallBack_Func													= {0};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SG_VD_CallBack_Func															= {0};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SG_HD_CallBack_Func															= {0};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SLVS_Common_CallBack_Func													= {0};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_LVDS_CallBack_Func[D_IM_PRO_SEN_LVDS_CH_NUM][E_IM_PRO_LVDS_OUTPUT_MODE_MAX]	= {{{0}}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_LDIV_CallBack_Func[D_IM_PRO_SEN_LDIV_CH_NUM]								= {{0}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_DMIPI_CallBack_Func[D_IM_PRO_SEN_DMIPI_CH_NUM]								= {{0}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_CMIPI_CallBack_Func[D_IM_PRO_SEN_CMIPI_CH_NUM][E_IM_PRO_CMIPI_INT_CATE_1+1]	= {{{0}}};

static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SRO_VD_CallBack_Func[D_IM_PRO_PIPE_NUM]										= {{0}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SRO_HD_CallBack_Func[D_IM_PRO_PIPE_NUM]										= {{0}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_B2B_VD_CallBack_Func[D_IM_PRO_PIPE_NUM]										= {{0}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_B2B_HD_CallBack_Func[D_IM_PRO_PIPE_NUM]										= {{0}};

static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_AEAWB_CallBack_Func[D_IM_PRO_STAT_AEAWB_CH_NUM]								= {{0}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_AF_CallBack_Func[D_IM_PRO_STAT_AF_CH_NUM]									= {{0}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_HIST_CallBack_Func[D_IM_PRO_STAT_HIST_CH_NUM]								= {{0}};

static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_PAS_VD_CallBack_Func														= {0};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_PAS_HD_CallBack_Func														= {0};

static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SLVS_CallBack_Func[D_IM_PRO_SEN_SLVS_CH_NUM][E_IM_PRO_SLVS_INT_TYPE_MAC_ERR2 + 1]			= {{{0}}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_OBD_CallBack_Func[D_IM_PRO_SENTOP_BLOB_NUM][D_IM_PRO_SEN_OBD_CH_NUM]						= {{{0}}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_OBT_CallBack_Func[D_IM_PRO_SENTOP_BLOB_NUM][D_IM_PRO_SEN_OBT_CH_NUM]						= {{{0}}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_SDC_CallBack_Func[D_IM_PRO_PIPE_NUM][D_IM_PRO_SRO_SDC_CH_NUM]								= {{{0}}};

static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_MONI_CallBack_Func[D_IM_PRO_PIPE_NUM][E_IM_PRO_BLOCK_TYPE_MAX][D_IM_PRO_SEN_MONI_CH_NUM]	= {{{{0}}}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_PWch_CallBack_Func[D_IM_PRO_PIPE_NUM][E_IM_PRO_BLOCK_TYPE_MAX][E_IM_PRO_PWCH_MAX]			= {{{{0}}}};
static	volatile	T_IM_PRO_CALLBACK_INFO	gIM_PRO_PRch_CallBack_Func[D_IM_PRO_PIPE_NUM][E_IM_PRO_BLOCK_TYPE_MAX][E_IM_PRO_PRCH_MAX]			= {{{{0}}}};


static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_vd[8] = {
	{ D_IM_PRO_INT_VDF00,	D_IM_PRO_INT_VDE00,	D_IM_PRO_INT_VDF00 },
	{ D_IM_PRO_INT_VDF01,	D_IM_PRO_INT_VDE01,	D_IM_PRO_INT_VDF01 },
	{ D_IM_PRO_INT_VDF10,	D_IM_PRO_INT_VDE10,	D_IM_PRO_INT_VDF10 },
	{ D_IM_PRO_INT_VDF11,	D_IM_PRO_INT_VDE11,	D_IM_PRO_INT_VDF11 },
	{ D_IM_PRO_INT_VDF20,	D_IM_PRO_INT_VDE20,	D_IM_PRO_INT_VDF20 },
	{ D_IM_PRO_INT_VDF21,	D_IM_PRO_INT_VDE21,	D_IM_PRO_INT_VDF21 },
	{ D_IM_PRO_INT_VDF30,	D_IM_PRO_INT_VDE30,	D_IM_PRO_INT_VDF30 },
	{ D_IM_PRO_INT_VDF31,	D_IM_PRO_INT_VDE31,	D_IM_PRO_INT_VDF31 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_hd[8] = {
	{ D_IM_PRO_INT_HDF00,	D_IM_PRO_INT_HDE00,	D_IM_PRO_INT_HDF00 },
	{ D_IM_PRO_INT_HDF01,	D_IM_PRO_INT_HDE01,	D_IM_PRO_INT_HDF01 },
	{ D_IM_PRO_INT_HDF10,	D_IM_PRO_INT_HDE10,	D_IM_PRO_INT_HDF10 },
	{ D_IM_PRO_INT_HDF11,	D_IM_PRO_INT_HDE11,	D_IM_PRO_INT_HDF11 },
	{ D_IM_PRO_INT_HDF20,	D_IM_PRO_INT_HDE20,	D_IM_PRO_INT_HDF20 },
	{ D_IM_PRO_INT_HDF21,	D_IM_PRO_INT_HDE21,	D_IM_PRO_INT_HDF21 },
	{ D_IM_PRO_INT_HDF30,	D_IM_PRO_INT_HDE30,	D_IM_PRO_INT_HDF30 },
	{ D_IM_PRO_INT_HDF31,	D_IM_PRO_INT_HDE31,	D_IM_PRO_INT_HDF31 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_sg_vd[4] = {
	{ D_IM_PRO_INT_SGVDF0,	D_IM_PRO_INT_SGVDE0,	D_IM_PRO_INT_SGVDF0 },
	{ D_IM_PRO_INT_SGVDF1,	D_IM_PRO_INT_SGVDE1,	D_IM_PRO_INT_SGVDF1 },
	{ D_IM_PRO_INT_SGVDF2,	D_IM_PRO_INT_SGVDE2,	D_IM_PRO_INT_SGVDF2 },
	{ D_IM_PRO_INT_SGVDF3,	D_IM_PRO_INT_SGVDE3,	D_IM_PRO_INT_SGVDF3 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_sg_hd[4] = {
	{ D_IM_PRO_INT_SGHDF0,	D_IM_PRO_INT_SGHDE0,	D_IM_PRO_INT_SGHDF0 },
	{ D_IM_PRO_INT_SGHDF1,	D_IM_PRO_INT_SGHDE1,	D_IM_PRO_INT_SGHDF1 },
	{ D_IM_PRO_INT_SGHDF2,	D_IM_PRO_INT_SGHDE2,	D_IM_PRO_INT_SGHDF2 },
	{ D_IM_PRO_INT_SGHDF3,	D_IM_PRO_INT_SGHDE3,	D_IM_PRO_INT_SGHDF3 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_gyro[8] = {
	{ D_IM_PRO_INT_GYRO_SOFF0,	D_IM_PRO_INT_GYRO_SOFE0,	D_IM_PRO_INT_GYRO_SOFF0 },
	{ D_IM_PRO_INT_GYRO_EOFF0,	D_IM_PRO_INT_GYRO_EOFE0,	D_IM_PRO_INT_GYRO_EOFF0 },
	{ D_IM_PRO_INT_GYRO_SOFF1,	D_IM_PRO_INT_GYRO_SOFE1,	D_IM_PRO_INT_GYRO_SOFF1 },
	{ D_IM_PRO_INT_GYRO_EOFF1,	D_IM_PRO_INT_GYRO_EOFE1,	D_IM_PRO_INT_GYRO_EOFF1 },
	{ D_IM_PRO_INT_GYRO_SOFF2,	D_IM_PRO_INT_GYRO_SOFE2,	D_IM_PRO_INT_GYRO_SOFF2 },
	{ D_IM_PRO_INT_GYRO_EOFF2,	D_IM_PRO_INT_GYRO_EOFE2,	D_IM_PRO_INT_GYRO_EOFF2 },
	{ D_IM_PRO_INT_GYRO_SOFF3,	D_IM_PRO_INT_GYRO_SOFE3,	D_IM_PRO_INT_GYRO_SOFF3 },
	{ D_IM_PRO_INT_GYRO_EOFF3,	D_IM_PRO_INT_GYRO_EOFE3,	D_IM_PRO_INT_GYRO_EOFF3 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_slvs_common[24] = {
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE7_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE7_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE7_BIT  },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE6_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE6_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE6_BIT  },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE5_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE5_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE5_BIT  },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE4_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE4_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE4_BIT  },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE3_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE3_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE3_BIT  },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE2_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE2_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE2_BIT  },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE1_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE1_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE1_BIT  },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE0_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE0_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE0_BIT  },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE7_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE7_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE7_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE6_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE6_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE6_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE5_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE5_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE5_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE4_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE4_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE4_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE3_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE3_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE3_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE2_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE2_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE2_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE1_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE1_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE1_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE0_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE0_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE0_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE7_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE7_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE7_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE6_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE6_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE6_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE5_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE5_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE5_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE4_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE4_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE4_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE3_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE3_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE3_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE2_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE2_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE2_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE1_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE1_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE1_BIT },
	{ D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE0_BIT,	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE0_BIT,	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE0_BIT },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_slvs_int[6] = {
	{ D_IM_PRO_SLVS_INTST_FSI_BIT,	D_IM_PRO_SLVS_INTEN_FSI_BIT,	D_IM_PRO_SLVS_INTST_FSI_BIT  },
	{ D_IM_PRO_SLVS_INTST_FSO_BIT,	D_IM_PRO_SLVS_INTEN_FSO_BIT,	D_IM_PRO_SLVS_INTST_FSO_BIT  },
	{ D_IM_PRO_SLVS_INTST_FEI_BIT,	D_IM_PRO_SLVS_INTEN_FEI_BIT,	D_IM_PRO_SLVS_INTST_FEI_BIT  },
	{ D_IM_PRO_SLVS_INTST_FEO_BIT,	D_IM_PRO_SLVS_INTEN_FEO_BIT,	D_IM_PRO_SLVS_INTST_FEO_BIT  },
	{ D_IM_PRO_SLVS_INTST_RDY_BIT,	D_IM_PRO_SLVS_INTEN_RDY_BIT,	D_IM_PRO_SLVS_INTST_RDY_BIT  },
	{ D_IM_PRO_SLVS_INTST_STBY_BIT,	D_IM_PRO_SLVS_INTEN_STBY_BIT,	D_IM_PRO_SLVS_INTST_STBY_BIT },
};                                                                  

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_slvs_leint[10] = {
	{ D_IM_PRO_SLVS_LEINTST_LBOVF_BIT,	D_IM_PRO_SLVS_LEINTEN_LBOVF_BIT,	D_IM_PRO_SLVS_LEINTST_LBOVF_BIT		},
	{ D_IM_PRO_SLVS_LEINTST_LNE_BIT,	D_IM_PRO_SLVS_LEINTEN_LNE_BIT,		D_IM_PRO_SLVS_LEINTST_LNE_BIT		},
	{ D_IM_PRO_SLVS_LEINTST_ECCE_BIT,	D_IM_PRO_SLVS_LEINTEN_ECCE_BIT,		D_IM_PRO_SLVS_LEINTST_ECCE_BIT		},
	{ D_IM_PRO_SLVS_LEINTST_ECC2C_BIT,	D_IM_PRO_SLVS_LEINTEN_ECC2C_BIT,	D_IM_PRO_SLVS_LEINTST_ECC2C_BIT		},
	{ D_IM_PRO_SLVS_LEINTST_ECC1C_BIT,	D_IM_PRO_SLVS_LEINTEN_ECC1C_BIT,	D_IM_PRO_SLVS_LEINTST_ECC1C_BIT		},
	{ D_IM_PRO_SLVS_LEINTST_HCRCE_BIT,	D_IM_PRO_SLVS_LEINTEN_HCRCE_BIT,	D_IM_PRO_SLVS_LEINTST_HCRCE_BIT		},
	{ D_IM_PRO_SLVS_LEINTST_HCRC2C_BIT,	D_IM_PRO_SLVS_LEINTEN_HCRC2C_BIT,	D_IM_PRO_SLVS_LEINTST_HCRC2C_BIT	},
	{ D_IM_PRO_SLVS_LEINTST_HCRC1C_BIT,	D_IM_PRO_SLVS_LEINTEN_HCRC1C_BIT,	D_IM_PRO_SLVS_LEINTST_HCRC1C_BIT	},
	{ D_IM_PRO_SLVS_LEINTST_PCRCE_BIT,	D_IM_PRO_SLVS_LEINTEN_PCRCE_BIT,	D_IM_PRO_SLVS_LEINTST_PCRCE_BIT		},
	{ D_IM_PRO_SLVS_LEINTST_LLE_BIT,	D_IM_PRO_SLVS_LEINTEN_LLE_BIT,		D_IM_PRO_SLVS_LEINTST_LLE_BIT		},
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_slvs_meint0[4] = {
	{ D_IM_PRO_SLVS_MEINTST0_DSD_BIT,	D_IM_PRO_SLVS_MEINTEN0_DSD_BIT,	D_IM_PRO_SLVS_MEINTST0_DSD_BIT },
	{ D_IM_PRO_SLVS_MEINTST0_DCL_BIT,	D_IM_PRO_SLVS_MEINTEN0_DCL_BIT,	D_IM_PRO_SLVS_MEINTST0_DCL_BIT },
	{ D_IM_PRO_SLVS_MEINTST0_SCL_BIT,	D_IM_PRO_SLVS_MEINTEN0_SCL_BIT,	D_IM_PRO_SLVS_MEINTST0_SCL_BIT },
	{ D_IM_PRO_SLVS_MEINTST0_ECL_BIT,	D_IM_PRO_SLVS_MEINTEN0_ECL_BIT,	D_IM_PRO_SLVS_MEINTST0_ECL_BIT },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_slvs_meint1[16] = {
	{ D_IM_PRO_SLVS_MEINTST1_FUNF_LANE7_BIT,	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE7_BIT,	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE7_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FUNF_LANE6_BIT,	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE6_BIT,	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE6_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FUNF_LANE5_BIT,	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE5_BIT,	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE5_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FUNF_LANE4_BIT,	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE4_BIT,	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE4_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FUNF_LANE3_BIT,	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE3_BIT,	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE3_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FUNF_LANE2_BIT,	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE2_BIT,	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE2_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FUNF_LANE1_BIT,	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE1_BIT,	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE1_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FUNF_LANE0_BIT,	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE0_BIT,	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE0_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FOVF_LANE7_BIT,	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE7_BIT,	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE7_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FOVF_LANE6_BIT,	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE6_BIT,	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE6_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FOVF_LANE5_BIT,	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE5_BIT,	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE5_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FOVF_LANE4_BIT,	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE4_BIT,	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE4_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FOVF_LANE3_BIT,	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE3_BIT,	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE3_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FOVF_LANE2_BIT,	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE2_BIT,	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE2_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FOVF_LANE1_BIT,	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE1_BIT,	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE1_BIT },
	{ D_IM_PRO_SLVS_MEINTST1_FOVF_LANE0_BIT,	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE0_BIT,	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE0_BIT },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_lvds_normal_int[4] = {
	{ D_IM_PRO_LVDSINTFLG_SOFF0,	D_IM_PRO_LVDSINTENB_SOFE0,	D_IM_PRO_LVDSINTFLG_SOFF0 },
	{ D_IM_PRO_LVDSINTFLG_EOFF0,	D_IM_PRO_LVDSINTENB_EOFE0,	D_IM_PRO_LVDSINTFLG_EOFF0 },
	{ D_IM_PRO_LVDSINTFLG_SOLF0,	D_IM_PRO_LVDSINTENB_SOLE0,	D_IM_PRO_LVDSINTFLG_SOLF0 },
	{ D_IM_PRO_LVDSINTFLG_EOLF0,	D_IM_PRO_LVDSINTENB_EOLE0,	D_IM_PRO_LVDSINTFLG_EOLF0 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_lvds_dol1_int[4] = {
	{ D_IM_PRO_LVDSINTFLG_SOFF1,	D_IM_PRO_LVDSINTENB_SOFE1,	D_IM_PRO_LVDSINTFLG_SOFF1 },
	{ D_IM_PRO_LVDSINTFLG_EOFF1,	D_IM_PRO_LVDSINTENB_EOFE1,	D_IM_PRO_LVDSINTFLG_EOFF1 },
	{ D_IM_PRO_LVDSINTFLG_SOLF1,	D_IM_PRO_LVDSINTENB_SOLE1,	D_IM_PRO_LVDSINTFLG_SOLF1 },
	{ D_IM_PRO_LVDSINTFLG_EOLF1,	D_IM_PRO_LVDSINTENB_EOLE1,	D_IM_PRO_LVDSINTFLG_EOLF1 },
};


static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_lvds_dol2_int[4] = {
	{ D_IM_PRO_LVDSINTFLG_SOFF2,	D_IM_PRO_LVDSINTENB_SOFE2,	D_IM_PRO_LVDSINTFLG_SOFF2 },
	{ D_IM_PRO_LVDSINTFLG_EOFF2,	D_IM_PRO_LVDSINTENB_EOFE2,	D_IM_PRO_LVDSINTFLG_EOFF2 },
	{ D_IM_PRO_LVDSINTFLG_SOLF2,	D_IM_PRO_LVDSINTENB_SOLE2,	D_IM_PRO_LVDSINTFLG_SOLF2 },
	{ D_IM_PRO_LVDSINTFLG_EOLF2,	D_IM_PRO_LVDSINTENB_EOLE2,	D_IM_PRO_LVDSINTFLG_EOLF2 },
};


static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_cmipi_int1[27] = {
	{ D_IM_PRO_MPICINTF1_CECF2,		D_IM_PRO_MPICINTE1_CEC2,		D_IM_PRO_MPICINTF1_CECF2	},
	{ D_IM_PRO_MPICINTF1_CECF1,		D_IM_PRO_MPICINTE1_CEC1,		D_IM_PRO_MPICINTF1_CECF1	},
	{ D_IM_PRO_MPICINTF1_CECF0,		D_IM_PRO_MPICINTE1_CEC0,		D_IM_PRO_MPICINTF1_CECF0	},
	{ D_IM_PRO_MPICINTF1_CEEF2,		D_IM_PRO_MPICINTE1_CEE2,		D_IM_PRO_MPICINTF1_CEEF2	},
	{ D_IM_PRO_MPICINTF1_CEEF1,		D_IM_PRO_MPICINTE1_CEE1,		D_IM_PRO_MPICINTF1_CEEF1	},
	{ D_IM_PRO_MPICINTF1_CEEF0,		D_IM_PRO_MPICINTE1_CEE0,		D_IM_PRO_MPICINTF1_CEEF0	},
	{ D_IM_PRO_MPICINTF1_CESSHF2,	D_IM_PRO_MPICINTE1_CESSH2,		D_IM_PRO_MPICINTF1_CESSHF2	},
	{ D_IM_PRO_MPICINTF1_CESSHF1,	D_IM_PRO_MPICINTE1_CESSH1,		D_IM_PRO_MPICINTF1_CESSHF1	},
	{ D_IM_PRO_MPICINTF1_CESSHF0,	D_IM_PRO_MPICINTE1_CESSH0,		D_IM_PRO_MPICINTF1_CESSHF0	},
	{ D_IM_PRO_MPICINTF1_CESHF2,	D_IM_PRO_MPICINTE1_CESH2,		D_IM_PRO_MPICINTF1_CESHF2	},
	{ D_IM_PRO_MPICINTF1_CESHF1,	D_IM_PRO_MPICINTE1_CESH1,		D_IM_PRO_MPICINTF1_CESHF1	},
	{ D_IM_PRO_MPICINTF1_CESHF0,	D_IM_PRO_MPICINTE1_CESH0,		D_IM_PRO_MPICINTF1_CESHF0	},
	{ D_IM_PRO_MPICINTF1_FEO1F,		D_IM_PRO_MPICINTE1_FEO1,		D_IM_PRO_MPICINTF1_FEO1F	},
	{ D_IM_PRO_MPICINTF1_FEO0F,		D_IM_PRO_MPICINTE1_FEO0,		D_IM_PRO_MPICINTF1_FEO0F	},
	{ D_IM_PRO_MPICINTF1_FEI1F,		D_IM_PRO_MPICINTE1_FEI1,		D_IM_PRO_MPICINTF1_FEI1F	},
	{ D_IM_PRO_MPICINTF1_FEI0F,		D_IM_PRO_MPICINTE1_FEI0,		D_IM_PRO_MPICINTF1_FEI0F	},
	{ D_IM_PRO_MPICINTF1_FSO1F,		D_IM_PRO_MPICINTE1_FSO1,		D_IM_PRO_MPICINTF1_FSO1F	},
	{ D_IM_PRO_MPICINTF1_FSO0F,		D_IM_PRO_MPICINTE1_FSO0,		D_IM_PRO_MPICINTF1_FSO0F	},
	{ D_IM_PRO_MPICINTF1_FSI1F,		D_IM_PRO_MPICINTE1_FSI1,		D_IM_PRO_MPICINTF1_FSI1F	},
	{ D_IM_PRO_MPICINTF1_FSI0F,		D_IM_PRO_MPICINTE1_FSI0,		D_IM_PRO_MPICINTF1_FSI0F	},
	{ D_IM_PRO_MPICINTF1_LZEF2,		D_IM_PRO_MPICINTE1_LZE2,		D_IM_PRO_MPICINTF1_LZEF2	},
	{ D_IM_PRO_MPICINTF1_LZEF1,		D_IM_PRO_MPICINTE1_LZE1,		D_IM_PRO_MPICINTF1_LZEF1	},
	{ D_IM_PRO_MPICINTF1_LZEF0,		D_IM_PRO_MPICINTE1_LZE0,		D_IM_PRO_MPICINTF1_LZEF0	},
	{ D_IM_PRO_MPICINTF1_EFSF,		D_IM_PRO_MPICINTE1_EFS,			D_IM_PRO_MPICINTF1_EFSF		},
	{ D_IM_PRO_MPICINTF1_CRCDTEF,	D_IM_PRO_MPICINTE1_CRCDTE,		D_IM_PRO_MPICINTF1_CRCDTEF	},
	{ D_IM_PRO_MPICINTF1_PHCRC2EF,	D_IM_PRO_MPICINTE1_PHCRC2E,		D_IM_PRO_MPICINTF1_PHCRC2EF	},
	{ D_IM_PRO_MPICINTF1_PHCRC1EF,	D_IM_PRO_MPICINTE1_PHCRC1E,		D_IM_PRO_MPICINTF1_PHCRC1EF	},
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_cmipi_int2[4] = {
	{ D_IM_PRO_MPICINTF2_CICHSF2,	D_IM_PRO_MPICINTE2_CICHSE2,		D_IM_PRO_MPICINTF2_CICHSF2 },
	{ D_IM_PRO_MPICINTF2_CICHSF1,	D_IM_PRO_MPICINTE2_CICHSE1,		D_IM_PRO_MPICINTF2_CICHSF1 },
	{ D_IM_PRO_MPICINTF2_CICHSF0,	D_IM_PRO_MPICINTE2_CICHSE0,		D_IM_PRO_MPICINTF2_CICHSF0 },
	{ D_IM_PRO_MPICINTF2_PHCRCAEF,	D_IM_PRO_MPICINTE2_PHCRCAEE,	D_IM_PRO_MPICINTF2_PHCRCAEF },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_dmipi_int[32] = {
	{ D_IM_PRO_MPIINTF_DEC3,	D_IM_PRO_MPIINTE_DEC3,		D_IM_PRO_MPIINTF_DEC3	 },
	{ D_IM_PRO_MPIINTF_DEC2,	D_IM_PRO_MPIINTE_DEC2,		D_IM_PRO_MPIINTF_DEC2	 },
	{ D_IM_PRO_MPIINTF_DEC1,	D_IM_PRO_MPIINTE_DEC1,		D_IM_PRO_MPIINTF_DEC1	 },
	{ D_IM_PRO_MPIINTF_DEC0,	D_IM_PRO_MPIINTE_DEC0,		D_IM_PRO_MPIINTF_DEC0	 },
	{ D_IM_PRO_MPIINTF_DEE3,	D_IM_PRO_MPIINTE_DEE3,		D_IM_PRO_MPIINTF_DEE3	 },
	{ D_IM_PRO_MPIINTF_DEE2,	D_IM_PRO_MPIINTE_DEE2,		D_IM_PRO_MPIINTF_DEE2	 },
	{ D_IM_PRO_MPIINTF_DEE1,	D_IM_PRO_MPIINTE_DEE1,		D_IM_PRO_MPIINTF_DEE1	 },
	{ D_IM_PRO_MPIINTF_DEE0,	D_IM_PRO_MPIINTE_DEE0,		D_IM_PRO_MPIINTF_DEE0	 },
	{ D_IM_PRO_MPIINTF_DESSH3,	D_IM_PRO_MPIINTE_DESSH3,	D_IM_PRO_MPIINTF_DESSH3	 },
	{ D_IM_PRO_MPIINTF_DESSH2,	D_IM_PRO_MPIINTE_DESSH2,	D_IM_PRO_MPIINTF_DESSH2	 },
	{ D_IM_PRO_MPIINTF_DESSH1,	D_IM_PRO_MPIINTE_DESSH1,	D_IM_PRO_MPIINTF_DESSH1	 },
	{ D_IM_PRO_MPIINTF_DESSH0,	D_IM_PRO_MPIINTE_DESSH0,	D_IM_PRO_MPIINTF_DESSH0	 },
	{ D_IM_PRO_MPIINTF_DESH3,	D_IM_PRO_MPIINTE_DESHE3,	D_IM_PRO_MPIINTF_DESH3	 },
	{ D_IM_PRO_MPIINTF_DESH2,	D_IM_PRO_MPIINTE_DESHE2,	D_IM_PRO_MPIINTF_DESH2	 },
	{ D_IM_PRO_MPIINTF_DESH1,	D_IM_PRO_MPIINTE_DESHE1,	D_IM_PRO_MPIINTF_DESH1	 },
	{ D_IM_PRO_MPIINTF_DESH0,	D_IM_PRO_MPIINTE_DESHE0,	D_IM_PRO_MPIINTF_DESH0	 },
	{ D_IM_PRO_MPIINTF_FEO1,	D_IM_PRO_MPIINTE_FEO1,		D_IM_PRO_MPIINTF_FEO1	 },
	{ D_IM_PRO_MPIINTF_FEO0,	D_IM_PRO_MPIINTE_FEO0,		D_IM_PRO_MPIINTF_FEO0	 },
	{ D_IM_PRO_MPIINTF_FEI1,	D_IM_PRO_MPIINTE_FEI1,		D_IM_PRO_MPIINTF_FEI1	 },
	{ D_IM_PRO_MPIINTF_FEI0,	D_IM_PRO_MPIINTE_FEI0,		D_IM_PRO_MPIINTF_FEI0	 },
	{ D_IM_PRO_MPIINTF_FSO1,	D_IM_PRO_MPIINTE_FSO1,		D_IM_PRO_MPIINTF_FSO1	 },
	{ D_IM_PRO_MPIINTF_FSO0,	D_IM_PRO_MPIINTE_FSO0,		D_IM_PRO_MPIINTF_FSO0	 },
	{ D_IM_PRO_MPIINTF_FSI1,	D_IM_PRO_MPIINTE_FSI1,		D_IM_PRO_MPIINTF_FSI1	 },
	{ D_IM_PRO_MPIINTF_FSI0,	D_IM_PRO_MPIINTE_FSI0,		D_IM_PRO_MPIINTF_FSI0	 },
	{ D_IM_PRO_MPIINTF_LZE3,	D_IM_PRO_MPIINTE_LZE3,		D_IM_PRO_MPIINTF_LZE3	 },
	{ D_IM_PRO_MPIINTF_LZE2,	D_IM_PRO_MPIINTE_LZE2,		D_IM_PRO_MPIINTF_LZE2	 },
	{ D_IM_PRO_MPIINTF_LZE1,	D_IM_PRO_MPIINTE_LZE1,		D_IM_PRO_MPIINTF_LZE1	 },
	{ D_IM_PRO_MPIINTF_LZE0,	D_IM_PRO_MPIINTE_LZE0,		D_IM_PRO_MPIINTF_LZE0	 },
	{ D_IM_PRO_MPIINTF_EFS,		D_IM_PRO_MPIINTE_EFS,		D_IM_PRO_MPIINTF_EFS	 },
	{ D_IM_PRO_MPIINTF_CRCE,	D_IM_PRO_MPIINTE_CRCE,		D_IM_PRO_MPIINTF_CRCE	 },
	{ D_IM_PRO_MPIINTF_ECCW,	D_IM_PRO_MPIINTE_ECCW,		D_IM_PRO_MPIINTF_ECCW	 },
	{ D_IM_PRO_MPIINTF_ECCE,	D_IM_PRO_MPIINTE_ECCE,		D_IM_PRO_MPIINTF_ECCE	 },
};                                                          

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_ldiv_int[1] = {
	{ D_IM_PRO_LDIVINTFLG_LDIVEF,	D_IM_PRO_LDIVINTENB_LDIVEE,	D_IM_PRO_LDIVINTFLG_LDIVEF },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_obd_int[1] = {
	{ D_IM_PRO_OBDINTFLG_OBDF,	D_IM_PRO_OBDINTENB_OBDE,		D_IM_PRO_OBDINTFLG_OBDF },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sen_obt_int[1] = {
	{ D_IM_PRO_OBTINTFLG_OBTF,	D_IM_PRO_OBTINTENB_OBTE,		D_IM_PRO_OBTINTFLG_OBTF },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sro_vd[2] = {
	{ D_IM_PRO_SROTOP_INT_VDF0,	D_IM_PRO_SROTOP_INT_VDE0,		D_IM_PRO_SROTOP_INT_VDF0 },
	{ D_IM_PRO_SROTOP_INT_VDF1,	D_IM_PRO_SROTOP_INT_VDE1,		D_IM_PRO_SROTOP_INT_VDF1 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sro_hd[2] = {
	{ D_IM_PRO_SROTOP_INT_HDF0,	D_IM_PRO_SROTOP_INT_HDE0,		D_IM_PRO_SROTOP_INT_HDF0 },
	{ D_IM_PRO_SROTOP_INT_HDF1,	D_IM_PRO_SROTOP_INT_HDE1,		D_IM_PRO_SROTOP_INT_HDF1 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_sro_sdc_int[1] = {
	{ D_IM_PRO_SDCINTFLG_SDCF,	D_IM_PRO_SDCINTENB_SDCE,		D_IM_PRO_SDCINTFLG_SDCF },
};


static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_b2b_vd[2] = {
	{ D_IM_PRO_B2BTOP_INT_VDF0,	D_IM_PRO_B2BTOP_INT_VDE0,		D_IM_PRO_B2BTOP_INT_VDF0 },
	{ D_IM_PRO_B2BTOP_INT_VDF1,	D_IM_PRO_B2BTOP_INT_VDE1,		D_IM_PRO_B2BTOP_INT_VDF1 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_b2b_hd[2] = {
	{ D_IM_PRO_B2BTOP_INT_HDF0,	D_IM_PRO_B2BTOP_INT_HDE0,		D_IM_PRO_B2BTOP_INT_HDF0 },
	{ D_IM_PRO_B2BTOP_INT_HDF1,	D_IM_PRO_B2BTOP_INT_HDE1,		D_IM_PRO_B2BTOP_INT_HDF1 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_stat_aeawb_int[1] = {
	{ D_IM_PRO_LINEINTFLG_LINEF,	D_IM_PRO_LINEINTENB_LINEE,	D_IM_PRO_LINEINTFLG_LINEF },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_stat_af_int[1] = {
	{ D_IM_PRO_AFINTFLG_AFF,		D_IM_PRO_AFINTENB_AFE,		D_IM_PRO_AFINTFLG_AFF },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_stat_hist_int[1] = {
	{ D_IM_PRO_HISTINTFLG_HISTF,	D_IM_PRO_HISTINTENB_HISTE,	D_IM_PRO_HISTINTFLG_HISTF },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_pas_vd[2] = {
	{ D_IM_PRO_PASTOP_INT_VDF0,	D_IM_PRO_PASTOP_INT_VDE0,		D_IM_PRO_PASTOP_INT_VDF0 },
	{ D_IM_PRO_PASTOP_INT_VDF1,	D_IM_PRO_PASTOP_INT_VDE1,		D_IM_PRO_PASTOP_INT_VDF1 },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_pas_hd[2] = {
	{ D_IM_PRO_PASTOP_INT_HDF0,	D_IM_PRO_PASTOP_INT_HDE0,		D_IM_PRO_PASTOP_INT_HDF0 },
	{ D_IM_PRO_PASTOP_INT_HDF1,	D_IM_PRO_PASTOP_INT_HDE1,		D_IM_PRO_PASTOP_INT_HDF1 },
};


static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_pwch_int[3] = {
	{ D_IM_PRO_PWCHINTFLG_PWF,	D_IM_PRO_PWCHINTENB_PWE,		D_IM_PRO_PWCHINTFLG_PWF  },
	{ D_IM_PRO_PWCHINTFLG_PWEF,	D_IM_PRO_PWCHINTENB_PWEE,		D_IM_PRO_PWCHINTFLG_PWEF },
	{ D_IM_PRO_PWCHINTFLG_PWXF,	D_IM_PRO_PWCHINTENB_PWXE,		D_IM_PRO_PWCHINTFLG_PWXF },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_prch_int[3] = {
	{ D_IM_PRO_PRCHINTENB_PRE,	D_IM_PRO_PRCHINTFLG_PRF,		D_IM_PRO_PRCHINTENB_PRE  },
	{ D_IM_PRO_PRCHINTENB_PREE,	D_IM_PRO_PRCHINTFLG_PREF,		D_IM_PRO_PRCHINTENB_PREE },
	{ D_IM_PRO_PRCHINTENB_PRXE,	D_IM_PRO_PRCHINTFLG_PRXF,		D_IM_PRO_PRCHINTENB_PRXE },
};

static const T_IM_PRO_INTFLG_TBL gim_pro_inttbl_moni_int[5] = {
	{ D_IM_PRO_MONIINTFLG_MONIHDF,		D_IM_PRO_MONIINTENB_MONIHDE,		D_IM_PRO_MONIINTFLG_MONIHDF		 },
	{ D_IM_PRO_MONIINTFLG_MONIEPFF,		D_IM_PRO_MONIINTENB_MONIEPFE,		D_IM_PRO_MONIINTFLG_MONIEPFF	 },
	{ D_IM_PRO_MONIINTFLG_MONIEPLF,		D_IM_PRO_MONIINTENB_MONIEPLE,		D_IM_PRO_MONIINTFLG_MONIEPLF	 },
	{ D_IM_PRO_MONIINTFLG_MONIBLLMINF,	D_IM_PRO_MONIINTENB_MONIBLLMINE,	D_IM_PRO_MONIINTFLG_MONIBLLMINF	 },
	{ D_IM_PRO_MONIINTFLG_MONIBLLMAXF,	D_IM_PRO_MONIINTENB_MONIBLLMAXE,	D_IM_PRO_MONIINTFLG_MONIBLLMAXF	 },
};


/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static VOID im_pro_clear_sensor_if_int_factor( UCHAR sencore_ch, T_IM_PRO_SENCORE_INT_FACTOR* int_factor );
static VOID im_pro_callback_sensor_if_int( UCHAR sencore_ch, T_IM_PRO_SENCORE_INT_FACTOR* int_factor );

static VOID im_pro_clear_sro_int_factor( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_SRO_INT_FACTOR* int_factor );
static VOID im_pro_callback_sro_int( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_SRO_INT_FACTOR* int_factor );

static VOID im_pro_sro_vd_proc( E_IM_PRO_UNIT_NUM unit_no );
static VOID im_pro_sro_hd_proc( E_IM_PRO_UNIT_NUM unit_no );

static VOID im_pro_clear_b2b_int_factor( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_B2B_INT_FACTOR* int_factor );
static VOID im_pro_callback_b2b_int( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_B2B_INT_FACTOR* int_factor );

static VOID im_pro_b2b_vd_proc( E_IM_PRO_UNIT_NUM unit_no );
static VOID im_pro_b2b_hd_proc( E_IM_PRO_UNIT_NUM unit_no );

static VOID im_pro_pwch_clear_int_factor( T_IM_PRO_PWCH_INT_INFO* pwch_info, ULONG* int_factor, UCHAR ch_cnt );
static VOID im_pro_pwch_callback_int_factor( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, ULONG* int_factor );
static VOID im_pro_prch_clear_int_factor( T_IM_PRO_PRCH_INT_INFO* pwch_info, ULONG* int_factor, UCHAR ch_cnt );
static VOID im_pro_prch_callback_int_factor( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, ULONG* int_factor );

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

static VOID im_pro_clear_sensor_if_int_factor_print( UCHAR sencore_ch, UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	if( sencore_ch == 0 ) {
		UCHAR slvs_ch;
		for( slvs_ch = 0; slvs_ch < D_IM_PRO_SEN_SLVS_CH_NUM; slvs_ch++ ) {
			Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) SLVS  PEINTST (0x%08x) PEINTEN (0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.SLVSEC.COMMON.PEINTST.word, IO_PRO.SEN.SLVSEC.COMMON.PEINTEN.word, status ));
			Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) SLVS  INTST   (0x%08x) INTEN   (0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.SLVSEC.STREAM[slvs_ch].INTST.word, IO_PRO.SEN.SLVSEC.STREAM[slvs_ch].INTEN.word, status ));
			Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) SLVS  LEINTST (0x%08x) LEINTEN (0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.SLVSEC.STREAM[slvs_ch].LEINTST.word, IO_PRO.SEN.SLVSEC.STREAM[slvs_ch].LEINTEN.word, status ));
			Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) SLVS  MEINTST0(0x%08x) MEINTEN0(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.SLVSEC.STREAM[slvs_ch].MEINTST0.word, IO_PRO.SEN.SLVSEC.STREAM[slvs_ch].MEINTEN0.word, status ));
			Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) SLVS  MEINTST1(0x%08x) MEINTEN1(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.SLVSEC.STREAM[slvs_ch].MEINTST1.word, IO_PRO.SEN.SLVSEC.STREAM[slvs_ch].MEINTEN1.word, status ));
		}
	}

	Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) LVDS  LVDSINTFLG0(0x%08x) LVDSINTENB0(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG0.word, IO_PRO.SEN.LVDS[sencore_ch].LVDSINTENB0.word, status ));
	Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) LVDS  LVDSINTFLG1(0x%08x) LVDSINTENB1(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG1.word, IO_PRO.SEN.LVDS[sencore_ch].LVDSINTENB1.word, status ));
	Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) LVDS  LVDSINTFLG2(0x%08x) LVDSINTENB2(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG2.word, IO_PRO.SEN.LVDS[sencore_ch].LVDSINTENB2.word, status ));

	Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) CMIPI MPICINTF1(0x%08x) MPICINTE1(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.MIPIC[sencore_ch].MPICINTF1.word, IO_PRO.SEN.MIPIC[sencore_ch].MPICINTE1.word, status ));
	Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) CMIPI MPICINTF2(0x%08x) MPICINTE2(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.MIPIC[sencore_ch].MPICINTF2.word, IO_PRO.SEN.MIPIC[sencore_ch].MPICINTE2.word, status ));

	Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) DMIPI MPIINTF(0x%08x) MPIINTE(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.MIPID[sencore_ch].MPIINTF.word, IO_PRO.SEN.MIPID[sencore_ch].MPIINTE.word, status ));

	Ddim_Print(( "im_pro_clear_sensor_if_int_factor(%u) MONI  MONIINTFLG(0x%08x) MONIINTENB(0x%08x) (status:%u)\n", sencore_ch, IO_PRO.SEN.MONI[sencore_ch].MONIINTFLG.word, IO_PRO.SEN.MONI[sencore_ch].MONIINTENB.word, status ));
#endif
}
static VOID im_pro_clear_sensor_if_int_factor( UCHAR sencore_ch, T_IM_PRO_SENCORE_INT_FACTOR* int_factor )
{
	UCHAR	ch_loopcnt;

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	Im_Pro_Dsb();
	im_pro_clear_sensor_if_int_factor_print( sencore_ch, 0 );

	// SLVS-EC
	if( sencore_ch == 0 ) {
		// Get & Clear interrupt flag
		int_factor->slvs_common_intf	= IO_PRO.SEN.SLVSEC.COMMON.PEINTST.word & IO_PRO.SEN.SLVSEC.COMMON.PEINTEN.word;
		IO_PRO.SEN.SLVSEC.COMMON.PEINTST.word	= int_factor->slvs_common_intf;
		for( ch_loopcnt = 0; ch_loopcnt < D_IM_PRO_SEN_SLVS_CH_NUM; ch_loopcnt++ ) {
			int_factor->slvs_intf[ch_loopcnt]		= IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].INTST.word & IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].INTEN.word;
			IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].INTST.word	= int_factor->slvs_intf[ch_loopcnt];

			int_factor->slvs_leintf[ch_loopcnt]		= IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].LEINTST.word & IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].LEINTEN.word;
			IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].LEINTST.word	= int_factor->slvs_leintf[ch_loopcnt];

			int_factor->slvs_meintf0[ch_loopcnt]	= IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].MEINTST0.word & IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].MEINTEN0.word;
			IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].MEINTST0.word = int_factor->slvs_meintf0[ch_loopcnt];

			int_factor->slvs_meintf1[ch_loopcnt]	= IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].MEINTST1.word & IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].MEINTEN1.word;
			IO_PRO.SEN.SLVSEC.STREAM[ch_loopcnt].MEINTST1.word = int_factor->slvs_meintf1[ch_loopcnt];
		}
	}

	// LVDS
	// Get interrupt flag
	int_factor->lvds_intf0	= IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG0.word & IO_PRO.SEN.LVDS[sencore_ch].LVDSINTENB0.word;
	int_factor->lvds_intf1	= IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG1.word & IO_PRO.SEN.LVDS[sencore_ch].LVDSINTENB1.word;
	int_factor->lvds_intf2	= IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG2.word & IO_PRO.SEN.LVDS[sencore_ch].LVDSINTENB2.word;

	// Clear interrupt flag
	IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG0.word	= int_factor->lvds_intf0;
	IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG1.word	= int_factor->lvds_intf1;
	IO_PRO.SEN.LVDS[sencore_ch].LVDSINTFLG2.word	= int_factor->lvds_intf2;

	//************* C-MIPI *************//
	// Get interrupt flag
	int_factor->cmipi_intf1	= IO_PRO.SEN.MIPIC[sencore_ch].MPICINTF1.word & IO_PRO.SEN.MIPIC[sencore_ch].MPICINTE1.word;
	int_factor->cmipi_intf2	= IO_PRO.SEN.MIPIC[sencore_ch].MPICINTF2.word & IO_PRO.SEN.MIPIC[sencore_ch].MPICINTE2.word;

	// Clear interrupt flag
	IO_PRO.SEN.MIPIC[sencore_ch].MPICINTF1.word	= int_factor->cmipi_intf1;
	IO_PRO.SEN.MIPIC[sencore_ch].MPICINTF2.word	= int_factor->cmipi_intf2;

	//************* D-MIPI *************//
	int_factor->dmipi_intf	= IO_PRO.SEN.MIPID[sencore_ch].MPIINTF.word & IO_PRO.SEN.MIPID[sencore_ch].MPIINTE.word;

	// Clear interrupt flag
	IO_PRO.SEN.MIPID[sencore_ch].MPIINTF.word	= int_factor->dmipi_intf;

	//************* MONI *************//
	int_factor->moni_intf	= IO_PRO.SEN.MONI[sencore_ch].MONIINTFLG.word & IO_PRO.SEN.MONI[sencore_ch].MONIINTENB.word;
	if( int_factor->moni_intf != 0 ) {
		im_pro_comm_update_moni_result( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, sencore_ch );
	}

	// Clear interrupt flag
	IO_PRO.SEN.MONI[sencore_ch].MONIINTFLG.word	= int_factor->moni_intf;

	Im_Pro_Dsb();
	im_pro_clear_sensor_if_int_factor_print( sencore_ch, 1 );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
}

static VOID im_pro_callback_sensor_if_int( UCHAR sencore_ch, T_IM_PRO_SENCORE_INT_FACTOR* int_factor )
{
	UINT32	loopcnt;
	UINT32	ch_loopcnt;
	ULONG	callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};
	DDIM_USER_FLGPTN	flgptn = 0;

	if( sencore_ch == 0 ) {
		/***************/
		/* SLVS Common */
		/***************/
		callback_result = 0;
		for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_slvs_common) / sizeof(gim_pro_inttbl_sen_slvs_common[0])); loopcnt++ ) {
			if( (int_factor->slvs_common_intf & gim_pro_inttbl_sen_slvs_common[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_sen_slvs_common[loopcnt].int_fact;
			}
		}
		// Callback
		if (( gIM_PRO_SLVS_Common_CallBack_Func.p_callback != NULL ) && ( callback_result != 0 )){
			cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
			cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
			cb_id.channel		= sencore_ch,
			gIM_PRO_SLVS_Common_CallBack_Func.p_callback( &cb_id, callback_result, gIM_PRO_SLVS_Common_CallBack_Func.user_param );
		}

		// Each stream interrupt
		for( ch_loopcnt = 0; ch_loopcnt < D_IM_PRO_SEN_SLVS_CH_NUM; ch_loopcnt++ ) {
			/*************/
			/* SLVS INT  */
			/*************/
			callback_result = 0;
			for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_slvs_int) / sizeof(gim_pro_inttbl_sen_slvs_int[0])); loopcnt++ ) {
				if( (int_factor->slvs_intf[ch_loopcnt] & gim_pro_inttbl_sen_slvs_int[loopcnt].flg_bitmask) != 0 ) {
					callback_result	|= gim_pro_inttbl_sen_slvs_int[loopcnt].int_fact;

					if ((int_factor->slvs_intf[ch_loopcnt] & D_IM_PRO_SLVS_INTST_FSI_BIT) != 0){
						flgptn |= (ch_loopcnt == 0) ? D_IM_PRO_SLVS_STREAM_A_INTST_FSI_WAITEND_FLG : D_IM_PRO_SLVS_STREAM_B_INTST_FSI_WAITEND_FLG;
					}
					if ((int_factor->slvs_intf[ch_loopcnt] & D_IM_PRO_SLVS_INTST_FSO_BIT) != 0){
						flgptn |= (ch_loopcnt == 0) ? D_IM_PRO_SLVS_STREAM_A_INTST_FSO_WAITEND_FLG : D_IM_PRO_SLVS_STREAM_B_INTST_FSO_WAITEND_FLG;
					}
					if ((int_factor->slvs_intf[ch_loopcnt] & D_IM_PRO_SLVS_INTST_FEI_BIT) != 0){
						flgptn |= (ch_loopcnt == 0) ? D_IM_PRO_SLVS_STREAM_A_INTST_FEI_WAITEND_FLG : D_IM_PRO_SLVS_STREAM_B_INTST_FEI_WAITEND_FLG;
					}
					if ((int_factor->slvs_intf[ch_loopcnt] & D_IM_PRO_SLVS_INTST_FEO_BIT) != 0){
						flgptn |= (ch_loopcnt == 0) ? D_IM_PRO_SLVS_STREAM_A_INTST_FEO_WAITEND_FLG : D_IM_PRO_SLVS_STREAM_B_INTST_FEO_WAITEND_FLG;
					}
					if ((int_factor->slvs_intf[ch_loopcnt] & D_IM_PRO_SLVS_INTST_RDY_BIT) != 0){
						flgptn |= (ch_loopcnt == 0) ? D_IM_PRO_SLVS_STREAM_A_INTST_RDY_WAITEND_FLG : D_IM_PRO_SLVS_STREAM_B_INTST_RDY_WAITEND_FLG;
					}
					if ((int_factor->slvs_intf[ch_loopcnt] & D_IM_PRO_SLVS_INTST_STBY_BIT) != 0){
						flgptn |= (ch_loopcnt == 0) ? D_IM_PRO_SLVS_STREAM_A_INTST_STBY_WAITEND_FLG : D_IM_PRO_SLVS_STREAM_B_INTST_STBY_WAITEND_FLG;
					}
				}
			}
			if( flgptn != 0 ) {
				DDIM_User_Set_Flg( FID_IM_SEN, flgptn );
			}

			// Callback
			if (( gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_INTN_O].p_callback != NULL ) && ( callback_result != 0 )){
				cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
				cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
				cb_id.channel		= sencore_ch,
				gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_INTN_O].p_callback( &cb_id, callback_result, gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_INTN_O].user_param );
			}

			/***************/
			/* SLVS LEINT  */
			/***************/
			callback_result = 0;
			for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_slvs_leint) / sizeof(gim_pro_inttbl_sen_slvs_leint[0])); loopcnt++ ) {
				if( (int_factor->slvs_leintf[ch_loopcnt] & gim_pro_inttbl_sen_slvs_leint[loopcnt].flg_bitmask) != 0 ) {
					callback_result	|= gim_pro_inttbl_sen_slvs_leint[loopcnt].int_fact;
				}
			}
			// Callback
			if (( gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_LINK_ERR].p_callback != NULL ) && ( callback_result != 0 )){
				cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
				cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
				cb_id.channel		= ch_loopcnt,
				gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_LINK_ERR].p_callback( &cb_id, callback_result, gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_LINK_ERR].user_param);
			}

			/***************/
			/* SLVS MEINT0 */
			/***************/
			callback_result = 0;
			for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_slvs_meint0) / sizeof(gim_pro_inttbl_sen_slvs_meint0[0])); loopcnt++ ) {
				if( (int_factor->slvs_meintf0[ch_loopcnt] & gim_pro_inttbl_sen_slvs_meint0[loopcnt].flg_bitmask) != 0 ) {
					callback_result	|= gim_pro_inttbl_sen_slvs_meint0[loopcnt].int_fact;
				}
			}
			// Callback
			if (( gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_MAC_ERR1].p_callback != NULL ) && ( callback_result != 0 )){
				cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
				cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
				cb_id.channel		= ch_loopcnt,
				gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_MAC_ERR1].p_callback( &cb_id, callback_result, gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_MAC_ERR1].user_param );
			}

			/***************/
			/* SLVS MEINT1 */
			/***************/
			callback_result = 0;
			for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_slvs_meint1) / sizeof(gim_pro_inttbl_sen_slvs_meint1[0])); loopcnt++ ) {
				if( (int_factor->slvs_meintf1[ch_loopcnt] & gim_pro_inttbl_sen_slvs_meint1[loopcnt].flg_bitmask) != 0 ) {
					callback_result	|= gim_pro_inttbl_sen_slvs_meint1[loopcnt].int_fact;
				}
			}
			// Callback
			if (( gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_MAC_ERR2].p_callback != NULL ) && ( callback_result != 0 )){
				cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
				cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
				cb_id.channel		= ch_loopcnt,
				gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_MAC_ERR2].p_callback( &cb_id, callback_result, gIM_PRO_SLVS_CallBack_Func[ch_loopcnt][E_IM_PRO_SLVS_INT_TYPE_MAC_ERR2].user_param );
			}
		}
	}

	/********************/
	/* LVDS Normal INT  */
	/********************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_lvds_normal_int) / sizeof(gim_pro_inttbl_sen_lvds_normal_int[0])); loopcnt++ ) {
		if( (int_factor->lvds_intf0 & gim_pro_inttbl_sen_lvds_normal_int[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sen_lvds_normal_int[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_LVDS_CallBack_Func[sencore_ch][0].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		cb_id.channel		= sencore_ch,
		gIM_PRO_LVDS_CallBack_Func[sencore_ch][0].p_callback( &cb_id, callback_result, gIM_PRO_LVDS_CallBack_Func[sencore_ch][0].user_param );
	}

	/********************/
	/* LVDS DOL1 INT    */
	/********************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_lvds_dol1_int) / sizeof(gim_pro_inttbl_sen_lvds_dol1_int[0])); loopcnt++ ) {
		if( (int_factor->lvds_intf1 & gim_pro_inttbl_sen_lvds_dol1_int[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sen_lvds_dol1_int[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_LVDS_CallBack_Func[sencore_ch][1].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		cb_id.channel		= sencore_ch,
		gIM_PRO_LVDS_CallBack_Func[sencore_ch][1].p_callback( &cb_id, callback_result, gIM_PRO_LVDS_CallBack_Func[sencore_ch][1].user_param );
	}

	/********************/
	/* LVDS DOL2 INT    */
	/********************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_lvds_dol2_int) / sizeof(gim_pro_inttbl_sen_lvds_dol2_int[0])); loopcnt++ ) {
		if( (int_factor->lvds_intf2 & gim_pro_inttbl_sen_lvds_dol2_int[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sen_lvds_dol2_int[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_LVDS_CallBack_Func[sencore_ch][2].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		cb_id.channel		= sencore_ch,
		gIM_PRO_LVDS_CallBack_Func[sencore_ch][2].p_callback( &cb_id, callback_result, gIM_PRO_LVDS_CallBack_Func[sencore_ch][2].user_param );
	}

	/***************/
	/* D-MIPI      */
	/***************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_dmipi_int) / sizeof(gim_pro_inttbl_sen_dmipi_int[0])); loopcnt++ ) {
		if( (int_factor->dmipi_intf & gim_pro_inttbl_sen_dmipi_int[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sen_dmipi_int[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_DMIPI_CallBack_Func[sencore_ch].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		cb_id.channel		= sencore_ch,
		gIM_PRO_DMIPI_CallBack_Func[sencore_ch].p_callback( &cb_id, callback_result, gIM_PRO_DMIPI_CallBack_Func[sencore_ch].user_param );
	}

	/********************/
	/* C-MIPI category0 */
	/********************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_cmipi_int1) / sizeof(gim_pro_inttbl_sen_cmipi_int1[0])); loopcnt++ ) {
		if( (int_factor->cmipi_intf1 & gim_pro_inttbl_sen_cmipi_int1[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sen_cmipi_int1[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_CMIPI_CallBack_Func[sencore_ch][E_IM_PRO_CMIPI_INT_CATE_0].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		cb_id.channel		= sencore_ch,
		gIM_PRO_CMIPI_CallBack_Func[sencore_ch][E_IM_PRO_CMIPI_INT_CATE_0].p_callback( &cb_id, callback_result, gIM_PRO_CMIPI_CallBack_Func[sencore_ch][E_IM_PRO_CMIPI_INT_CATE_0].user_param );
	}

	/********************/
	/* C-MIPI category1 */
	/********************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_cmipi_int2) / sizeof(gim_pro_inttbl_sen_cmipi_int2[0])); loopcnt++ ) {
		if( (int_factor->cmipi_intf2 & gim_pro_inttbl_sen_cmipi_int2[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sen_cmipi_int2[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_CMIPI_CallBack_Func[sencore_ch][E_IM_PRO_CMIPI_INT_CATE_1].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		cb_id.channel		= sencore_ch,
		gIM_PRO_CMIPI_CallBack_Func[sencore_ch][E_IM_PRO_CMIPI_INT_CATE_1].p_callback( &cb_id, callback_result, gIM_PRO_CMIPI_CallBack_Func[sencore_ch][E_IM_PRO_CMIPI_INT_CATE_1].user_param );
	}

	/***************/
	/* MONI        */
	/***************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_moni_int) / sizeof(gim_pro_inttbl_moni_int[0])); loopcnt++ ) {
		if( (int_factor->moni_intf & gim_pro_inttbl_moni_int[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_moni_int[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_MONI_CallBack_Func[0][E_IM_PRO_BLOCK_TYPE_SEN][sencore_ch].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		cb_id.channel		= sencore_ch,
		gIM_PRO_MONI_CallBack_Func[0][E_IM_PRO_BLOCK_TYPE_SEN][sencore_ch].p_callback( &cb_id, callback_result, gIM_PRO_MONI_CallBack_Func[0][E_IM_PRO_BLOCK_TYPE_SEN][sencore_ch].user_param );
	}
	
}

static VOID im_pro_clear_sro_int_factor_print( E_IM_PRO_UNIT_NUM unit_no, UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_clear_sro_int_factor_print(%u) MONI MONIINTFLG(0x%08x) MONIINTENB(0x%08x) (status:%u)\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.MONI0.MONIINTFLG.word, IO_PRO.IMG_PIPE[unit_no].SRO.MONI0.MONIINTENB.word, status ));

	for(UCHAR i = 0; i < D_IM_PRO_SRO_SDC_CH_NUM; i++) {
		Ddim_Print(( "im_pro_clear_sro_int_factor_print(%u) SDC%u SDCINTFLG(0x%08x) SDCINTENB(0x%08x) (status:%u)\n", unit_no, i, IO_PRO.IMG_PIPE[unit_no].SRO.SDC[i].SDCINTFLG.word, IO_PRO.IMG_PIPE[unit_no].SRO.SDC[i].SDCINTENB.word, status ));
	}

	for(UCHAR i = 0; i < D_IM_PRO_SRO_PWCH_CH_NUM; i++) {
		Ddim_Print(( "im_pro_clear_sro_int_factor_print(%u) PWCH%u PWCHINTFLG(0x%08x) PWCHINTENB(0x%08x) (status:%u)\n", unit_no, i, IO_PRO.IMG_PIPE[unit_no].SRO.PWCH[i].PWCHINTFLG.word, IO_PRO.IMG_PIPE[unit_no].SRO.PWCH[i].PWCHINTENB.word, status ));
	}

	for(UCHAR i = 0; i < D_IM_PRO_SRO_PRCH_CH_NUM; i++) {
		Ddim_Print(( "im_pro_clear_sro_int_factor_print(%u) PRCH%u PRCHINTFLG(0x%08x) PRCHINTENB(0x%08x) (status:%u)\n", unit_no, i, IO_PRO.IMG_PIPE[unit_no].SRO.PRCH[i].PRCHINTFLG.word, IO_PRO.IMG_PIPE[unit_no].SRO.PRCH[i].PRCHINTENB.word, status ));
	}
#endif
}

static VOID im_pro_clear_sro_int_factor( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_SRO_INT_FACTOR* int_factor )
{
	T_IM_PRO_PWCH_INT_INFO pwch_info = {0};
	T_IM_PRO_PRCH_INT_INFO prch_info = {0};

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );
	Im_Pro_Dsb();
	im_pro_clear_sro_int_factor_print( unit_no, 0 );

	//************* MONI *************//
	// Get interrupt flag
	int_factor->moni_intf	= IO_PRO.IMG_PIPE[unit_no].SRO.MONI0.MONIINTFLG.word & IO_PRO.IMG_PIPE[unit_no].SRO.MONI0.MONIINTENB.word;
	// Clear interrupt flag
	IO_PRO.IMG_PIPE[unit_no].SRO.MONI0.MONIINTFLG.word = int_factor->moni_intf;

	if( int_factor->moni_intf != 0 ) {
		im_pro_comm_update_moni_result( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_MONI_CH_0 );
	}

	//************* SDC *************//
	// Get interrupt flag
	int_factor->sdc_intf[0]	= IO_PRO.IMG_PIPE[unit_no].SRO.SDC[0].SDCINTFLG.word & IO_PRO.IMG_PIPE[unit_no].SRO.SDC[0].SDCINTENB.word;
	int_factor->sdc_intf[1]	= IO_PRO.IMG_PIPE[unit_no].SRO.SDC[1].SDCINTFLG.word & IO_PRO.IMG_PIPE[unit_no].SRO.SDC[1].SDCINTENB.word;
	// Clear interrupt flag
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[0].SDCINTFLG.word = int_factor->sdc_intf[0];
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[1].SDCINTFLG.word = int_factor->sdc_intf[1];

	//************* PWCH *************//
	pwch_info.unit_no		= unit_no;
	pwch_info.block_type	= E_IM_PRO_BLOCK_TYPE_B2B;
	pwch_info.reg_ptr		= IO_PRO.IMG_PIPE[unit_no].SRO.PWCH;
	im_pro_pwch_clear_int_factor( &pwch_info, int_factor->pwch_intf, D_IM_PRO_SRO_PWCH_CH_NUM );

	//************* PRCH *************//
	prch_info.unit_no		= unit_no;
	prch_info.block_type	= E_IM_PRO_BLOCK_TYPE_B2B;
	prch_info.reg_ptr		= IO_PRO.IMG_PIPE[unit_no].SRO.PRCH;
	im_pro_prch_clear_int_factor( &prch_info, int_factor->prch_intf, D_IM_PRO_SRO_PRCH_CH_NUM );

	Im_Pro_Dsb();
	im_pro_clear_sro_int_factor_print( unit_no, 1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );
}

static VOID im_pro_callback_sro_int( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_SRO_INT_FACTOR* int_factor )
{
	UINT32	loopcnt;
	UINT32	ch_loopcnt;
	ULONG	callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

	/***************/
	/* MONI        */
	/***************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_moni_int) / sizeof(gim_pro_inttbl_moni_int[0])); loopcnt++ ) {
		if( (int_factor->moni_intf & gim_pro_inttbl_moni_int[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_moni_int[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_MONI_CallBack_Func[unit_no][E_IM_PRO_BLOCK_TYPE_SRO][E_IM_PRO_MONI_CH_0].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= unit_no,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SRO,
		cb_id.channel		= 0,
		gIM_PRO_MONI_CallBack_Func[unit_no][E_IM_PRO_BLOCK_TYPE_SRO][E_IM_PRO_MONI_CH_0].p_callback( &cb_id, callback_result, gIM_PRO_MONI_CallBack_Func[unit_no][E_IM_PRO_BLOCK_TYPE_SRO][E_IM_PRO_MONI_CH_0].user_param );
	}

	/***************/
	/* SDC         */
	/***************/
	for( ch_loopcnt = 0; ch_loopcnt < D_IM_PRO_SRO_SDC_CH_NUM; ch_loopcnt++ ) {
		callback_result = 0;
		for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sro_sdc_int) / sizeof(gim_pro_inttbl_sro_sdc_int[0])); loopcnt++ ) {
			if( (int_factor->moni_intf & gim_pro_inttbl_sro_sdc_int[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_sro_sdc_int[loopcnt].int_fact;
			}
		}
		// Callback
		if (( gIM_PRO_SDC_CallBack_Func[unit_no][ch_loopcnt].p_callback != NULL ) && ( callback_result != 0 )){
			cb_id.unit_no		= unit_no,
			cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SRO,
			cb_id.channel		= ch_loopcnt,
			gIM_PRO_SDC_CallBack_Func[unit_no][ch_loopcnt].p_callback( &cb_id, callback_result, gIM_PRO_SDC_CallBack_Func[unit_no][ch_loopcnt].user_param );
		}
	}

	/***************/
	/* PWCH        */
	/***************/
	for( ch_loopcnt = 0; ch_loopcnt < D_IM_PRO_SRO_PWCH_CH_NUM; ch_loopcnt++ ) {
		im_pro_pwch_callback_int_factor( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, (E_IM_PRO_PWCH)ch_loopcnt, int_factor->pwch_intf );
	}

	/***************/
	/* PRCH        */
	/***************/
	for( ch_loopcnt = 0; ch_loopcnt < D_IM_PRO_SRO_PRCH_CH_NUM; ch_loopcnt++ ) {
		im_pro_prch_callback_int_factor( unit_no, E_IM_PRO_BLOCK_TYPE_SRO, (E_IM_PRO_PRCH)ch_loopcnt, int_factor->prch_intf );
	}
}

static VOID im_pro_sro_vd_proc_print( E_IM_PRO_UNIT_NUM unit_no, UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_sro_vd_proc_print(%u) VD VHDINTFLG(0x%08x) VHDINTENB(0x%08x) (status:%u)\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTFLG.word, IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTENB.word, status ));
#endif
}

static VOID im_pro_sro_vd_proc( E_IM_PRO_UNIT_NUM unit_no )
{
	UINT32			loopcnt;
	ULONG			vd_intf = 0;
	ULONG callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );
	Im_Pro_Dsb();
	im_pro_sro_vd_proc_print( unit_no, 0 );

	// Get interrupt flag
	vd_intf	= IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTFLG.word & IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTENB.word & D_IM_PRO_SROTOP_INT_VD_ALL;

	// Clear interrupt flag
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTFLG.word	= vd_intf;

	Im_Pro_Dsb();
	im_pro_sro_vd_proc_print( unit_no, 1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );

	/***********/
	/* VD Edge */
	/***********/
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sro_vd) / sizeof(gim_pro_inttbl_sro_vd[0])); loopcnt++ ) {
		if( (vd_intf & gim_pro_inttbl_sro_vd[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sro_vd[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_SRO_VD_CallBack_Func[unit_no].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= unit_no,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SRO,
		cb_id.channel		= 0,
		gIM_PRO_SRO_VD_CallBack_Func[unit_no].p_callback( &cb_id, callback_result, gIM_PRO_SRO_VD_CallBack_Func[unit_no].user_param );
	}
}

static VOID im_pro_sro_hd_proc_print( E_IM_PRO_UNIT_NUM unit_no, UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_sro_hd_proc_print(%u) HD VHDINTFLG(0x%08x) VHDINTENB(0x%08x) (status:%u)\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTFLG.word, IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTENB.word, status ));
#endif
}

static VOID im_pro_sro_hd_proc( E_IM_PRO_UNIT_NUM unit_no )
{
	UINT32			loopcnt;
	ULONG			vd_intf;
	ULONG callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );
	Im_Pro_Dsb();
	im_pro_sro_hd_proc_print( unit_no, 0 );

	// Get interrupt flag
	vd_intf	= IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTFLG.word & IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTENB.word & D_IM_PRO_SROTOP_INT_HD_ALL;

	// Clear interrupt flag
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTFLG.word	= vd_intf;

	Im_Pro_Dsb();
	im_pro_sro_hd_proc_print( unit_no, 1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );

	/***********/
	/* HD Edge */
	/***********/
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sro_hd) / sizeof(gim_pro_inttbl_sro_hd[0])); loopcnt++ ) {
		if( (vd_intf & gim_pro_inttbl_sro_hd[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sro_hd[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_SRO_HD_CallBack_Func[unit_no].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= unit_no,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SRO,
		cb_id.channel		= 0,
		gIM_PRO_SRO_HD_CallBack_Func[unit_no].p_callback( &cb_id, callback_result, gIM_PRO_SRO_HD_CallBack_Func[unit_no].user_param );
	}
}

static VOID im_pro_clear_b2b_int_factor_print( E_IM_PRO_UNIT_NUM unit_no, UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	for(UCHAR i = 0; i < D_IM_PRO_B2B_PWCH_CH_NUM; i++) {
		Ddim_Print(( "im_pro_clear_b2b_int_factor_print(%u) PWCH%u PWCHINTFLG(0x%08x) PWCHINTENB(0x%08x) (status:%u)\n", unit_no, i, IO_PRO.IMG_PIPE[unit_no].B2B.PWCH[i].PWCHINTFLG.word, IO_PRO.IMG_PIPE[unit_no].B2B.PWCH[i].PWCHINTENB.word, status ));
	}

	for(UCHAR i = 0; i < D_IM_PRO_B2B_PRCH_CH_NUM; i++) {
		Ddim_Print(( "im_pro_clear_b2b_int_factor_print(%u) PRCH%u PRCHINTFLG(0x%08x) PRCHINTENB(0x%08x) (status:%u)\n", unit_no, i, IO_PRO.IMG_PIPE[unit_no].B2B.PRCH[i].PRCHINTFLG.word, IO_PRO.IMG_PIPE[unit_no].B2B.PRCH[i].PRCHINTENB.word, status ));
	}
#endif
}

static VOID im_pro_clear_b2b_int_factor( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_B2B_INT_FACTOR* int_factor )
{
	T_IM_PRO_PWCH_INT_INFO pwch_info = {0};
	T_IM_PRO_PRCH_INT_INFO prch_info = {0};

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );
	Im_Pro_Dsb();
	im_pro_clear_b2b_int_factor_print( unit_no, 0 );

	//************* PWCH *************//
	pwch_info.unit_no		= unit_no;
	pwch_info.block_type	= E_IM_PRO_BLOCK_TYPE_B2B;
	pwch_info.reg_ptr		= IO_PRO.IMG_PIPE[unit_no].B2B.PWCH;
	im_pro_pwch_clear_int_factor( &pwch_info, int_factor->pwch_intf, D_IM_PRO_B2B_PWCH_CH_NUM );

	//************* PRCH *************//
	prch_info.unit_no		= unit_no;
	prch_info.block_type	= E_IM_PRO_BLOCK_TYPE_B2B;
	prch_info.reg_ptr		= IO_PRO.IMG_PIPE[unit_no].B2B.PRCH;
	im_pro_prch_clear_int_factor( &prch_info, int_factor->prch_intf, D_IM_PRO_B2B_PRCH_CH_NUM );

	Im_Pro_Dsb();
	im_pro_clear_b2b_int_factor_print( unit_no, 1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );
}

static VOID im_pro_callback_b2b_int( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_B2B_INT_FACTOR* int_factor )
{
	UINT32	ch_loopcnt;

	/***************/
	/* PWCH        */
	/***************/
	for( ch_loopcnt = 0; ch_loopcnt < D_IM_PRO_B2B_PWCH_CH_NUM; ch_loopcnt++ ) {
		im_pro_pwch_callback_int_factor( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, (E_IM_PRO_PWCH)ch_loopcnt, int_factor->pwch_intf );
	}

	/***************/
	/* PRCH        */
	/***************/
	for( ch_loopcnt = 0; ch_loopcnt < D_IM_PRO_B2B_PRCH_CH_NUM; ch_loopcnt++ ) {
		im_pro_prch_callback_int_factor( unit_no, E_IM_PRO_BLOCK_TYPE_B2B, (E_IM_PRO_PRCH)ch_loopcnt, int_factor->prch_intf );
	}
}

static VOID im_pro_b2b_vd_proc_print( E_IM_PRO_UNIT_NUM unit_no, UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_b2b_vd_proc_print(%u) VD VHDINTFLG(0x%08x) VHDINTENB(0x%08x) (status:%u)\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTFLG.word, IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTENB.word, status ));
#endif
}

static VOID im_pro_b2b_vd_proc( E_IM_PRO_UNIT_NUM unit_no )
{
	UINT32			loopcnt;
	ULONG			vd_intf;
	ULONG callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );
	Im_Pro_Dsb();
	im_pro_b2b_vd_proc_print( unit_no, 0 );

	// Get interrupt flag
	vd_intf	= IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTFLG.word & IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTENB.word & D_IM_PRO_B2BTOP_INT_VD_ALL;

	// Clear interrupt flag
	IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTFLG.word	= vd_intf;

	Im_Pro_Dsb();
	im_pro_b2b_vd_proc_print( unit_no, 1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );

	/***********/
	/* VD Edge */
	/***********/
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_b2b_vd) / sizeof(gim_pro_inttbl_b2b_vd[0])); loopcnt++ ) {
		if( (vd_intf & gim_pro_inttbl_b2b_vd[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_b2b_vd[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_B2B_VD_CallBack_Func[unit_no].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= unit_no,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_B2B,
		cb_id.channel		= 0,
		gIM_PRO_B2B_VD_CallBack_Func[unit_no].p_callback( &cb_id, callback_result, gIM_PRO_B2B_VD_CallBack_Func[unit_no].user_param );
	}
}

static VOID im_pro_b2b_hd_proc_print( E_IM_PRO_UNIT_NUM unit_no, UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_b2b_hd_proc_print(%u) VD VHDINTFLG(0x%08x) VHDINTENB(0x%08x) (status:%u)\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTFLG.word, IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTENB.word, status ));
#endif
}

static VOID im_pro_b2b_hd_proc( E_IM_PRO_UNIT_NUM unit_no )
{
	UINT32			loopcnt;
	ULONG			vd_intf;
	ULONG callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_b2b_hd_proc %u begin\n", unit_no ));
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );
	Im_Pro_Dsb();
	im_pro_b2b_hd_proc_print( unit_no, 0 );

	// Get interrupt flag
	vd_intf	= IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTFLG.word & IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTENB.word & D_IM_PRO_B2BTOP_INT_HD_ALL;

	// Clear interrupt flag
	IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTFLG.word	= vd_intf;

	Im_Pro_Dsb();
	im_pro_b2b_hd_proc_print( unit_no, 1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );

	/***********/
	/* VD Edge */
	/***********/
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_b2b_hd) / sizeof(gim_pro_inttbl_b2b_hd[0])); loopcnt++ ) {
		if( (vd_intf & gim_pro_inttbl_b2b_hd[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_b2b_hd[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_B2B_HD_CallBack_Func[unit_no].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= unit_no,
		cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_B2B,
		cb_id.channel		= 0,
		gIM_PRO_B2B_HD_CallBack_Func[unit_no].p_callback( &cb_id, callback_result, gIM_PRO_B2B_HD_CallBack_Func[unit_no].user_param );
	}
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_b2b_hd_proc %u end\n", unit_no ));
#endif
}

static VOID im_pro_pwch_clear_int_factor( T_IM_PRO_PWCH_INT_INFO* pwch_info, ULONG* int_factor, UCHAR ch_cnt )
{
	UINT32	ch_loopcnt;

	for( ch_loopcnt = 0; ch_loopcnt < ch_cnt; ch_loopcnt++ ) {
		// Get interrupt flag
		int_factor[ch_loopcnt]	= pwch_info->reg_ptr[ch_loopcnt].PWCHINTFLG.word & pwch_info->reg_ptr[ch_loopcnt].PWCHINTENB.word;
		// Clear interrupt flag
		pwch_info->reg_ptr[ch_loopcnt].PWCHINTFLG.word = int_factor[ch_loopcnt];

		if( ( int_factor[ch_loopcnt] & D_IM_PRO_PWCHINTFLG_PWF ) != 0 ) {
			switch( pwch_info->block_type ) {
				case E_IM_PRO_BLOCK_TYPE_SRO:
					// for AFPB buffer update
					if( ( ch_loopcnt == E_IM_PRO_PWCH_2 ) ||
						( ch_loopcnt == E_IM_PRO_PWCH_3 ) ||
						( ch_loopcnt == E_IM_PRO_PWCH_4 ) ||
						( ch_loopcnt == E_IM_PRO_PWCH_5 ) ) {
						im_pro_comm_update_afpb_pwch_index(pwch_info->block_type, ( ch_loopcnt - E_IM_PRO_PWCH_2 ));
					}
					break;

				case E_IM_PRO_BLOCK_TYPE_STAT:
					// for AEAWB / FLCK buffer update
					im_pro_comm_update_stat_pwch_index(ch_loopcnt);
					break;

				default:
					break;
			}
		}

		if( (int_factor[ch_loopcnt] & D_IM_PRO_PWCHINTFLG_PWXF) != 0 ) {
			im_pro_comm_set_pwch_axi_response( pwch_info->unit_no, pwch_info->block_type, ch_loopcnt, pwch_info->reg_ptr[ch_loopcnt].PWCHBRESP.bit.PWCHBRESP );
		}
	}
}

static VOID im_pro_pwch_callback_int_factor( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, ULONG* int_factor )
{
	UINT32	loopcnt;
	ULONG	callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

	/*******************/
	/* PWCH Callback   */
	/*******************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_pwch_int) / sizeof(gim_pro_inttbl_pwch_int[0])); loopcnt++ ) {
		if( (int_factor[ch] & gim_pro_inttbl_pwch_int[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_pwch_int[loopcnt].int_fact;
		}
	}
	if (( gIM_PRO_PWch_CallBack_Func[unit_no][block_type][ch].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= unit_no,
		cb_id.block_type	= block_type,
		cb_id.channel		= ch,
		gIM_PRO_PWch_CallBack_Func[unit_no][block_type][ch].p_callback( &cb_id, callback_result, gIM_PRO_PWch_CallBack_Func[unit_no][block_type][ch].user_param );
	}
	Im_Pro_Dsb();
}

static VOID im_pro_prch_clear_int_factor( T_IM_PRO_PRCH_INT_INFO* prch_info, ULONG* int_factor, UCHAR ch_cnt )
{
	UINT32	ch_loopcnt;

	for( ch_loopcnt = 0; ch_loopcnt < ch_cnt; ch_loopcnt++ ) {
		// Get interrupt flag
		int_factor[ch_loopcnt]	= prch_info->reg_ptr[ch_loopcnt].PRCHINTFLG.word & prch_info->reg_ptr[ch_loopcnt].PRCHINTENB.word;
		// Clear interrupt flag
		prch_info->reg_ptr[ch_loopcnt].PRCHINTFLG.word = int_factor[ch_loopcnt];

		if( (int_factor[ch_loopcnt] & D_IM_PRO_PRCHINTFLG_PRXF) != 0 ) {
			im_pro_comm_set_prch_axi_response( prch_info->unit_no, prch_info->block_type, ch_loopcnt, prch_info->reg_ptr[ch_loopcnt].PRCHBRESP.bit.PRCHBRESP );
		}
	}
}

static VOID im_pro_prch_callback_int_factor( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, ULONG* int_factor )
{
	UINT32	loopcnt;
	ULONG	callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

	/*******************/
	/* PRCH Callback   */
	/*******************/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_prch_int) / sizeof(gim_pro_inttbl_prch_int[0])); loopcnt++ ) {
		if( (int_factor[ch] & gim_pro_inttbl_prch_int[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_prch_int[loopcnt].int_fact;
		}
	}
	if (( gIM_PRO_PRch_CallBack_Func[unit_no][block_type][ch].p_callback != NULL ) && ( callback_result != 0 )){
		cb_id.unit_no		= unit_no,
		cb_id.block_type	= block_type,
		cb_id.channel		= ch,
		gIM_PRO_PRch_CallBack_Func[unit_no][block_type][ch].p_callback( &cb_id, callback_result, gIM_PRO_PRch_CallBack_Func[unit_no][block_type][ch].user_param );
	}
	Im_Pro_Dsb();
}

/**
SENTOP Interrupt setting
@param[in]	int_ctrl : SENTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_SENTOP_Interrupt_Ctrl( T_IM_PRO_INT_SENTOP_CTRL* int_ctrl )
{
	struct io_vhdintenb	vhdintenb;
	struct io_vhdintflg	vhdintflg;

#ifdef CO_PARAM_CHECK
	if (int_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Interrupt_Ctrl error. int_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_ctrl->vd_enable.permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Interrupt_Ctrl error. 'permission_flg' value over!! (%d)\n", int_ctrl->vd_enable.permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	vhdintenb.VHDINTENB1.word	= IO_PRO.SEN.SENTOP.VHDINTENB.VHDINTENB1.word;
	vhdintenb.VHDINTENB2.word	= IO_PRO.SEN.SENTOP.VHDINTENB.VHDINTENB2.word;

	vhdintflg.VHDINTFLG1.word	= 0;
	vhdintflg.VHDINTFLG2.word	= 0;

	// VHDINTENB VD Interrupt
	if (0 != (int_ctrl->vd_enable.interrupt_bit & D_IM_PRO_INT_VDE00)){
		vhdintflg.VHDINTFLG1.bit.__VDF00	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB1.bit.VDE00		= int_ctrl->vd_enable.permission_flg;
	}
	if (0 != (int_ctrl->vd_enable.interrupt_bit & D_IM_PRO_INT_VDE01)){
		vhdintflg.VHDINTFLG1.bit.__VDF01	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB1.bit.VDE01		= int_ctrl->vd_enable.permission_flg;
	}
	if (0 != (int_ctrl->vd_enable.interrupt_bit & D_IM_PRO_INT_VDE10)){
		vhdintflg.VHDINTFLG1.bit.__VDF10	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB1.bit.VDE10		= int_ctrl->vd_enable.permission_flg;
	}
	if (0 != (int_ctrl->vd_enable.interrupt_bit & D_IM_PRO_INT_VDE11)){
		vhdintflg.VHDINTFLG1.bit.__VDF11	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB1.bit.VDE10		= int_ctrl->vd_enable.permission_flg;
	}
	if (0 != (int_ctrl->vd_enable.interrupt_bit & D_IM_PRO_INT_VDE20)){
		vhdintflg.VHDINTFLG1.bit.__VDF20	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB1.bit.VDE20		= int_ctrl->vd_enable.permission_flg;
	}
	if (0 != (int_ctrl->vd_enable.interrupt_bit & D_IM_PRO_INT_VDE21)){
		vhdintflg.VHDINTFLG1.bit.__VDF21	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB1.bit.VDE21		= int_ctrl->vd_enable.permission_flg;
	}
	if (0 != (int_ctrl->vd_enable.interrupt_bit & D_IM_PRO_INT_VDE30)){
		vhdintflg.VHDINTFLG1.bit.__VDF30	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB1.bit.VDE30		= int_ctrl->vd_enable.permission_flg;
	}
	if (0 != (int_ctrl->vd_enable.interrupt_bit & D_IM_PRO_INT_VDE31)){
		vhdintflg.VHDINTFLG1.bit.__VDF31	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB1.bit.VDE31		= int_ctrl->vd_enable.permission_flg;
	}

	// VHDINTENB HD Interrupt
	if (0 != (int_ctrl->hd_enable.interrupt_bit & D_IM_PRO_INT_HDE00)){
		vhdintflg.VHDINTFLG2.bit.__HDF00	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB2.bit.HDE00		= int_ctrl->hd_enable.permission_flg;
	}
	if (0 != (int_ctrl->hd_enable.interrupt_bit & D_IM_PRO_INT_HDE01)){
		vhdintflg.VHDINTFLG2.bit.__HDF01	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB2.bit.HDE01		= int_ctrl->hd_enable.permission_flg;
	}
	if (0 != (int_ctrl->hd_enable.interrupt_bit & D_IM_PRO_INT_HDE10)){
		vhdintflg.VHDINTFLG2.bit.__HDF10	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB2.bit.HDE10		= int_ctrl->hd_enable.permission_flg;
	}
	if (0 != (int_ctrl->hd_enable.interrupt_bit & D_IM_PRO_INT_HDE11)){
		vhdintflg.VHDINTFLG2.bit.__HDF11	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB2.bit.HDE10		= int_ctrl->hd_enable.permission_flg;
	}
	if (0 != (int_ctrl->hd_enable.interrupt_bit & D_IM_PRO_INT_HDE20)){
		vhdintflg.VHDINTFLG2.bit.__HDF20	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB2.bit.HDE20		= int_ctrl->hd_enable.permission_flg;
	}
	if (0 != (int_ctrl->hd_enable.interrupt_bit & D_IM_PRO_INT_HDE21)){
		vhdintflg.VHDINTFLG2.bit.__HDF21	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB2.bit.HDE21		= int_ctrl->hd_enable.permission_flg;
	}
	if (0 != (int_ctrl->hd_enable.interrupt_bit & D_IM_PRO_INT_HDE30)){
		vhdintflg.VHDINTFLG2.bit.__HDF30	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB2.bit.HDE30		= int_ctrl->hd_enable.permission_flg;
	}
	if (0 != (int_ctrl->hd_enable.interrupt_bit & D_IM_PRO_INT_HDE31)){
		vhdintflg.VHDINTFLG2.bit.__HDF31	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.VHDINTENB2.bit.HDE31		= int_ctrl->hd_enable.permission_flg;
	}

	IO_PRO.SEN.SENTOP.INTCTL.bit.INTMD	= int_ctrl->int_mode;
	IO_PRO.SEN.SENTOP.VHDINTFLG.VHDINTFLG1.word	= vhdintflg.VHDINTFLG1.word;
	IO_PRO.SEN.SENTOP.VHDINTENB.VHDINTENB1.word	= vhdintenb.VHDINTENB1.word;

	IO_PRO.SEN.SENTOP.VHDINTFLG.VHDINTFLG2.word	= vhdintflg.VHDINTFLG2.word;
	IO_PRO.SEN.SENTOP.VHDINTENB.VHDINTENB2.word	= vhdintenb.VHDINTENB2.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
SENTOP Interrupt setting
@param[in]	sg_int_ctrl : SENTOP SG interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_SENTOP_SG_Interrupt_Ctrl( T_IM_PRO_INT_SG_CTRL* sg_int_ctrl )
{
	union io_sgvhdintenb	sgvhdintenb;
	union io_sgvhdintflg	sgvhdintflg;

#ifdef CO_PARAM_CHECK
	if (sg_int_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_SG_Interrupt_Ctrl error. sg_int_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (sg_int_ctrl->sg_vd_enable.permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_SENTOP_SG_Interrupt_Ctrl error. 'SG VD permission_flg' value over!! (%d)\n", sg_int_ctrl->sg_vd_enable.permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (sg_int_ctrl->sg_hd_enable.permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_SENTOP_SG_Interrupt_Ctrl error. 'SG HD permission_flg' value over!! (%d)\n", sg_int_ctrl->sg_hd_enable.permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	sgvhdintenb.word	= IO_PRO.SEN.SENTOP.SGVHDINTENB.word;
	sgvhdintflg.word	= 0;

	// SGVHDINTENB VD Interrupt
	if (0 != (sg_int_ctrl->sg_vd_enable.interrupt_bit & D_IM_PRO_INT_SGVDE0)){
		sgvhdintflg.bit.__VDF_SG0	= D_IM_PRO_INTFLG_CLEAR;
		sgvhdintenb.bit.VDE_SG0		= sg_int_ctrl->sg_vd_enable.permission_flg;
	}
	if (0 != (sg_int_ctrl->sg_vd_enable.interrupt_bit & D_IM_PRO_INT_SGVDE1)){
		sgvhdintflg.bit.__VDF_SG1	= D_IM_PRO_INTFLG_CLEAR;
		sgvhdintenb.bit.VDE_SG1		= sg_int_ctrl->sg_vd_enable.permission_flg;
	}
	if (0 != (sg_int_ctrl->sg_vd_enable.interrupt_bit & D_IM_PRO_INT_SGVDE2)){
		sgvhdintflg.bit.__VDF_SG2	= D_IM_PRO_INTFLG_CLEAR;
		sgvhdintenb.bit.VDE_SG2		= sg_int_ctrl->sg_vd_enable.permission_flg;
	}
	if (0 != (sg_int_ctrl->sg_vd_enable.interrupt_bit & D_IM_PRO_INT_SGVDE3)){
		sgvhdintflg.bit.__VDF_SG3	= D_IM_PRO_INTFLG_CLEAR;
		sgvhdintenb.bit.VDE_SG3		= sg_int_ctrl->sg_vd_enable.permission_flg;
	}

	// SGVHDINTENB HD Interrupt
	if (0 != (sg_int_ctrl->sg_hd_enable.interrupt_bit & D_IM_PRO_INT_SGHDE0)){
		sgvhdintflg.bit.__HDF_SG0	= D_IM_PRO_INTFLG_CLEAR;
		sgvhdintenb.bit.HDE_SG0		= sg_int_ctrl->sg_hd_enable.permission_flg;
	}
	if (0 != (sg_int_ctrl->sg_hd_enable.interrupt_bit & D_IM_PRO_INT_SGHDE1)){
		sgvhdintflg.bit.__HDF_SG1	= D_IM_PRO_INTFLG_CLEAR;
		sgvhdintenb.bit.HDE_SG1		= sg_int_ctrl->sg_hd_enable.permission_flg;
	}
	if (0 != (sg_int_ctrl->sg_hd_enable.interrupt_bit & D_IM_PRO_INT_SGHDE2)){
		sgvhdintflg.bit.__HDF_SG2	= D_IM_PRO_INTFLG_CLEAR;
		sgvhdintenb.bit.HDE_SG2		= sg_int_ctrl->sg_hd_enable.permission_flg;
	}
	if (0 != (sg_int_ctrl->sg_hd_enable.interrupt_bit & D_IM_PRO_INT_SGHDE3)){
		sgvhdintflg.bit.__HDF_SG3	= D_IM_PRO_INTFLG_CLEAR;
		sgvhdintenb.bit.HDE_SG3		= sg_int_ctrl->sg_hd_enable.permission_flg;
	}

	IO_PRO.SEN.SENTOP.VHDSGCTL.VHDSGCTL1.bit.SGVDEG0	= sg_int_ctrl->sg_sync.vdeg[0];
	IO_PRO.SEN.SENTOP.VHDSGCTL.VHDSGCTL1.bit.SGVDEG1	= sg_int_ctrl->sg_sync.vdeg[1];
	IO_PRO.SEN.SENTOP.VHDSGCTL.VHDSGCTL2.bit.SGVDEG2	= sg_int_ctrl->sg_sync.vdeg[2];
	IO_PRO.SEN.SENTOP.VHDSGCTL.VHDSGCTL2.bit.SGVDEG3	= sg_int_ctrl->sg_sync.vdeg[3];
	IO_PRO.SEN.SENTOP.VHDSGCTL.VHDSGCTL1.bit.SGHDEG0	= sg_int_ctrl->sg_sync.hdeg[0];
	IO_PRO.SEN.SENTOP.VHDSGCTL.VHDSGCTL1.bit.SGHDEG1	= sg_int_ctrl->sg_sync.hdeg[1];
	IO_PRO.SEN.SENTOP.VHDSGCTL.VHDSGCTL2.bit.SGHDEG2	= sg_int_ctrl->sg_sync.hdeg[2];
	IO_PRO.SEN.SENTOP.VHDSGCTL.VHDSGCTL2.bit.SGHDEG3	= sg_int_ctrl->sg_sync.hdeg[3];

	IO_PRO.SEN.SENTOP.SGVHDINTFLG.word	= sgvhdintflg.word;
	IO_PRO.SEN.SENTOP.SGVHDINTENB.word	= sgvhdintenb.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
GYRO Interrupt setting
@param[in]	gyro_int_ctrl : GYRO interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_SENTOP_Gyro_Interrupt_Ctrl( T_IM_PRO_INT_GYRO_CTRL* gyro_int_ctrl )
{
	union io_gintenb	gintenb;
	union io_gintflg	gintflg;

#ifdef CO_PARAM_CHECK
	if (gyro_int_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Gyro_Interrupt_Ctrl error. gyro_int_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (gyro_int_ctrl->gyro_sof_enable.permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Gyro_Interrupt_Ctrl error. 'SOF permission_flg' value over!! (%d)\n", gyro_int_ctrl->gyro_sof_enable.permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (gyro_int_ctrl->gyro_eof_enable.permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Gyro_Interrupt_Ctrl error. 'EOF permission_flg' value over!! (%d)\n", gyro_int_ctrl->gyro_sof_enable.permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	gintenb.word	= IO_PRO.SEN.SENTOP.GINTENB.word;
	gintflg.word	= 0;

	// GINTENB SOF Interrupt
	if (0 != (gyro_int_ctrl->gyro_sof_enable.interrupt_bit & D_IM_PRO_INT_GYRO_SOFE0)){
		gintflg.bit.__SOFF0	= D_IM_PRO_INTFLG_CLEAR;
		gintenb.bit.SOFE0	= gyro_int_ctrl->gyro_sof_enable.permission_flg;
	}
	if (0 != (gyro_int_ctrl->gyro_sof_enable.interrupt_bit & D_IM_PRO_INT_GYRO_SOFE1)){
		gintflg.bit.__SOFF1	= D_IM_PRO_INTFLG_CLEAR;
		gintenb.bit.SOFE1	= gyro_int_ctrl->gyro_sof_enable.permission_flg;
	}
	if (0 != (gyro_int_ctrl->gyro_sof_enable.interrupt_bit & D_IM_PRO_INT_GYRO_SOFE2)){
		gintflg.bit.__SOFF2	= D_IM_PRO_INTFLG_CLEAR;
		gintenb.bit.SOFE2	= gyro_int_ctrl->gyro_sof_enable.permission_flg;
	}
	if (0 != (gyro_int_ctrl->gyro_sof_enable.interrupt_bit & D_IM_PRO_INT_GYRO_SOFE3)){
		gintflg.bit.__SOFF3	= D_IM_PRO_INTFLG_CLEAR;
		gintenb.bit.SOFE3	= gyro_int_ctrl->gyro_sof_enable.permission_flg;
	}

	// GINTENB EOF Interrupt
	if (0 != (gyro_int_ctrl->gyro_eof_enable.interrupt_bit & D_IM_PRO_INT_GYRO_EOFE0)){
		gintflg.bit.__EOFF0	= D_IM_PRO_INTFLG_CLEAR;
		gintenb.bit.EOFE0	= gyro_int_ctrl->gyro_eof_enable.permission_flg;
	}
	if (0 != (gyro_int_ctrl->gyro_eof_enable.interrupt_bit & D_IM_PRO_INT_GYRO_EOFE1)){
		gintflg.bit.__EOFF1	= D_IM_PRO_INTFLG_CLEAR;
		gintenb.bit.EOFE1	= gyro_int_ctrl->gyro_eof_enable.permission_flg;
	}
	if (0 != (gyro_int_ctrl->gyro_eof_enable.interrupt_bit & D_IM_PRO_INT_GYRO_EOFE2)){
		gintflg.bit.__EOFF2	= D_IM_PRO_INTFLG_CLEAR;
		gintenb.bit.EOFE2	= gyro_int_ctrl->gyro_eof_enable.permission_flg;
	}
	if (0 != (gyro_int_ctrl->gyro_eof_enable.interrupt_bit & D_IM_PRO_INT_GYRO_EOFE3)){
		gintflg.bit.__EOFF3	= D_IM_PRO_INTFLG_CLEAR;
		gintenb.bit.EOFE3	= gyro_int_ctrl->gyro_eof_enable.permission_flg;
	}

	IO_PRO.SEN.SENTOP.GINTFLG.word	= gintflg.word;
	IO_PRO.SEN.SENTOP.GINTENB.word	= gintenb.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
SENTOP macro interrupt delay setting
@param[in]	block_num : SENCORE block number(0:SENCORE0, 1:SENCORE1, 2:SENCORE2, 3:SENCORE3)
@param[in]	ch : interrupt ch(0-1)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_Set_VHD_Delay( UCHAR block_num, UCHAR ch, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (block_num >= D_IM_PRO_SEN_SENCORE_NUM){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_VHD_Delay. block_num value error!! (block_num:%d ch:%d)\n", block_num, ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (ch >= D_IM_PRO_SENTOP_VHD_CH_NUM){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_VHD_Delay. ch value error!! (block_num:%d ch:%d)\n", block_num, ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_VHD_Delay error. delay_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->hd_delay > D_IM_PRO_HMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_VHD_Delay hd_delay value error. delay_ctrl->hd_delay=%u\n",delay_ctrl->hd_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_h_delay > D_IM_PRO_VMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_VHD_Delay vd_h_delay value error. delay_ctrl->vd_h_delay=%u\n",delay_ctrl->vd_h_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_v_delay > D_IM_PRO_VMIVADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_VHD_Delay vd_v_delay value error. delay_ctrl->vd_v_delay=%u\n",delay_ctrl->vd_v_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	switch( block_num ) {
		case 0:
			if( ch == 0 ) {
				IO_PRO.SEN.SENTOP.HMIHADR.HMIHADR1.bit.HMIHADR00	= delay_ctrl->hd_delay;
				IO_PRO.SEN.SENTOP.VMIHADR.VMIHADR1.bit.VMIHADR00	= delay_ctrl->vd_h_delay;
				IO_PRO.SEN.SENTOP.VMIVADR.VMIVADR1.bit.VMIVADR00	= delay_ctrl->vd_v_delay;
				IO_PRO.SEN.SENTOP.VHSET0.VHSET0_1.bit.VHSET00		= D_IM_PRO_FUNC_EN;
			}
			else {
				IO_PRO.SEN.SENTOP.HMIHADR.HMIHADR1.bit.HMIHADR01	= delay_ctrl->hd_delay;
				IO_PRO.SEN.SENTOP.VMIHADR.VMIHADR1.bit.VMIHADR01	= delay_ctrl->vd_h_delay;
				IO_PRO.SEN.SENTOP.VMIVADR.VMIVADR1.bit.VMIVADR01	= delay_ctrl->vd_v_delay;
				IO_PRO.SEN.SENTOP.VHSET0.VHSET0_2.bit.VHSET01		= D_IM_PRO_FUNC_EN;
			}
			break;

		case 1:
			if( ch == 0 ) {
				IO_PRO.SEN.SENTOP.HMIHADR.HMIHADR2.bit.HMIHADR10	= delay_ctrl->hd_delay;
				IO_PRO.SEN.SENTOP.VMIHADR.VMIHADR2.bit.VMIHADR10	= delay_ctrl->vd_h_delay;
				IO_PRO.SEN.SENTOP.VMIVADR.VMIVADR2.bit.VMIVADR10	= delay_ctrl->vd_v_delay;
				IO_PRO.SEN.SENTOP.VHSET1.VHSET1_1.bit.VHSET10		= D_IM_PRO_FUNC_EN;
			}
			else {
				IO_PRO.SEN.SENTOP.HMIHADR.HMIHADR2.bit.HMIHADR11	= delay_ctrl->hd_delay;
				IO_PRO.SEN.SENTOP.VMIHADR.VMIHADR2.bit.VMIHADR11	= delay_ctrl->vd_h_delay;
				IO_PRO.SEN.SENTOP.VMIVADR.VMIVADR2.bit.VMIVADR11	= delay_ctrl->vd_v_delay;
				IO_PRO.SEN.SENTOP.VHSET1.VHSET1_2.bit.VHSET11		= D_IM_PRO_FUNC_EN;
			}
			break;

		case 2:
			if( ch == 0 ) {
				IO_PRO.SEN.SENTOP.HMIHADR.HMIHADR3.bit.HMIHADR20	= delay_ctrl->hd_delay;
				IO_PRO.SEN.SENTOP.VMIHADR.VMIHADR3.bit.VMIHADR20	= delay_ctrl->vd_h_delay;
				IO_PRO.SEN.SENTOP.VMIVADR.VMIVADR3.bit.VMIVADR20	= delay_ctrl->vd_v_delay;
				IO_PRO.SEN.SENTOP.VHSET2.VHSET2_1.bit.VHSET20		= D_IM_PRO_FUNC_EN;
			}
			else {
				IO_PRO.SEN.SENTOP.HMIHADR.HMIHADR3.bit.HMIHADR21	= delay_ctrl->hd_delay;
				IO_PRO.SEN.SENTOP.VMIHADR.VMIHADR3.bit.VMIHADR21	= delay_ctrl->vd_h_delay;
				IO_PRO.SEN.SENTOP.VMIVADR.VMIVADR3.bit.VMIVADR21	= delay_ctrl->vd_v_delay;
				IO_PRO.SEN.SENTOP.VHSET2.VHSET2_2.bit.VHSET21		= D_IM_PRO_FUNC_EN;
			}
			break;

		//case 3:
		default:
			if( ch == 0 ) {
				IO_PRO.SEN.SENTOP.HMIHADR.HMIHADR4.bit.HMIHADR30	= delay_ctrl->hd_delay;
				IO_PRO.SEN.SENTOP.VMIHADR.VMIHADR4.bit.VMIHADR30	= delay_ctrl->vd_h_delay;
				IO_PRO.SEN.SENTOP.VMIVADR.VMIVADR4.bit.VMIVADR30	= delay_ctrl->vd_v_delay;
				IO_PRO.SEN.SENTOP.VHSET3.VHSET3_1.bit.VHSET30		= D_IM_PRO_FUNC_EN;
			}
			else {
				IO_PRO.SEN.SENTOP.HMIHADR.HMIHADR4.bit.HMIHADR31	= delay_ctrl->hd_delay;
				IO_PRO.SEN.SENTOP.VMIHADR.VMIHADR4.bit.VMIHADR31	= delay_ctrl->vd_h_delay;
				IO_PRO.SEN.SENTOP.VMIVADR.VMIVADR4.bit.VMIVADR31	= delay_ctrl->vd_v_delay;
				IO_PRO.SEN.SENTOP.VHSET3.VHSET3_2.bit.VHSET31		= D_IM_PRO_FUNC_EN;
			}
			break;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
SENTOP macro GYRO interrupt delay setting
@param[in]	block_num : SENCORE block number(0:SENCORE0, 1:SENCORE1, 2:SENCORE2, 3:SENCORE3)
@param[in]	ch : channel(0:HD/VD0, 1:HD/VD1, 2:HD/VD2, 3:HD/VD3)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_Set_Gyro_Int_Delay( UCHAR block_num, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (block_num >= D_IM_PRO_SEN_SENCORE_NUM){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_Gyro_Int_Delay. block_num value error!! (block_num:%d)\n", block_num));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_Gyro_Int_Delay error. delay_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->hd_delay > D_IM_PRO_HMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_Gyro_Int_Delay hd_delay value error. delay_ctrl->hd_delay=%u\n",delay_ctrl->hd_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_h_delay > D_IM_PRO_VMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_Gyro_Int_Delay vd_h_delay value error. delay_ctrl->vd_h_delay=%u\n",delay_ctrl->vd_h_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_v_delay > D_IM_PRO_VMIVADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Set_Gyro_Int_Delay vd_v_delay value error. delay_ctrl->vd_v_delay=%u\n",delay_ctrl->vd_v_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	switch( block_num ) {
		case 0:
			IO_PRO.SEN.SENTOP.GHADR.GHADR1.bit.GHADR0	= delay_ctrl->hd_delay;
			IO_PRO.SEN.SENTOP.GVADR.GVADR1.bit.GVADR0	= delay_ctrl->vd_v_delay;
			IO_PRO.SEN.SENTOP.GSET0.bit.GSET0			= D_IM_PRO_FUNC_EN;
			break;

		case 1:
			IO_PRO.SEN.SENTOP.GHADR.GHADR1.bit.GHADR1	= delay_ctrl->hd_delay;
			IO_PRO.SEN.SENTOP.GVADR.GVADR1.bit.GVADR1	= delay_ctrl->vd_v_delay;
			IO_PRO.SEN.SENTOP.GSET1.bit.GSET1			= D_IM_PRO_FUNC_EN;
			break;

		case 2:
			IO_PRO.SEN.SENTOP.GHADR.GHADR2.bit.GHADR2	= delay_ctrl->hd_delay;
			IO_PRO.SEN.SENTOP.GVADR.GVADR2.bit.GVADR2	= delay_ctrl->vd_v_delay;
			IO_PRO.SEN.SENTOP.GSET2.bit.GSET2			= D_IM_PRO_FUNC_EN;
			break;

		//case 3:
		default:
			IO_PRO.SEN.SENTOP.GHADR.GHADR2.bit.GHADR3	= delay_ctrl->hd_delay;
			IO_PRO.SEN.SENTOP.GVADR.GVADR2.bit.GVADR3	= delay_ctrl->vd_v_delay;
			IO_PRO.SEN.SENTOP.GSET3.bit.GSET3			= D_IM_PRO_FUNC_EN;
			break;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
SEN block VD Int handler setting.
@param[in]	inthandler : Interrupt handler address pointer 
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SEN_Set_VD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_SENTOP_VD_CallBack_Func.p_callback = callback_cfg->inthandler;
	gIM_PRO_SENTOP_VD_CallBack_Func.user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
SEN block HD Int handler setting.
@param[in]	inthandler : Interrupt handler address pointer 
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SEN_Set_HD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_SENTOP_HD_CallBack_Func.p_callback = callback_cfg->inthandler;
	gIM_PRO_SENTOP_HD_CallBack_Func.user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
SEN block SG VD Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SEN_Set_SG_VD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_SG_VD_CallBack_Func.p_callback = callback_cfg->inthandler;
	gIM_PRO_SG_VD_CallBack_Func.user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
SEN block SG HD Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SEN_Set_SG_HD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_SG_HD_CallBack_Func.p_callback = callback_cfg->inthandler;
	gIM_PRO_SG_HD_CallBack_Func.user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
SEN block Gyro Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SEN_Set_Gyro_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_SENTOP_Gyro_CallBack_Func.p_callback = callback_cfg->inthandler;
	gIM_PRO_SENTOP_Gyro_CallBack_Func.user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}


static VOID im_pro_sen_int_handler_print( UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	for(UCHAR i = 0; i < D_IM_PRO_SEN_LDIV_CH_NUM; i++) {
		Ddim_Print(( "im_pro_sen_int_handler_print() LDIV LDIVINTFLG(0x%08x) LDIVINTENB(0x%08x) (status:%u)\n", IO_PRO.SEN.LDIV[i].LDIVINTFLG.word, IO_PRO.SEN.LDIV[i].LDIVINTENB.word, status ));
	}

	for( UCHAR loopcnt = 0; loopcnt < D_IM_PRO_SENTOP_BLOB_NUM; loopcnt++ ) {
		for( UCHAR loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_SEN_OBD_CH_NUM; loopcnt_ch++ ) {
			Ddim_Print(( "im_pro_sen_int_handler_print() OBD[%u][%u] OBDINTFLG(0x%08x) OBDINTENB(0x%08x) (status:%u)\n", loopcnt, loopcnt_ch, IO_PRO.SEN.OBD[loopcnt][loopcnt_ch].OBDINTFLG.word, IO_PRO.SEN.OBD[loopcnt][loopcnt_ch].OBDINTENB.word, status ));
		}

		for( UCHAR loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_SEN_OBT_CH_NUM; loopcnt_ch++ ) {
			Ddim_Print(( "im_pro_sen_int_handler_print() OBT[%u][%u] OBTINTFLG(0x%08x) OBTINTENB(0x%08x) (status:%u)\n", loopcnt, loopcnt_ch, IO_PRO.SEN.OBT[loopcnt][loopcnt_ch].OBTINTFLG.word, IO_PRO.SEN.OBT[loopcnt][loopcnt_ch].OBTINTENB.word, status ));
		}
	}

	for(UCHAR i = 0; i < D_IM_PRO_SRO_PWCH_CH_NUM; i++) {
		Ddim_Print(( "im_pro_sen_int_handler_print() PWCH%u PWCHINTFLG(0x%08x) PWCHINTENB(0x%08x) (status:%u)\n", i, IO_PRO.SEN.PWCH[i].PWCHINTFLG.word, IO_PRO.SEN.PWCH[i].PWCHINTENB.word, status ));
	}
#endif
}

/**
SEN block Int handler except VD, HD, and Sensor I/F interrupts.
*/
VOID Im_PRO_SEN_Int_Handler( VOID )
{
	UINT32					loopcnt = 0;
	UINT32					loopcnt_ch = 0;
	UINT32					loopcnt_block = 0;
	ULONG					ldiv_intf[D_IM_PRO_SEN_LDIV_NUM] = {0};
	ULONG					obd_intf[D_IM_PRO_SENTOP_BLOB_NUM][D_IM_PRO_SEN_OBD_CH_NUM] = {{0}};
	ULONG					obt_intf[D_IM_PRO_SENTOP_BLOB_NUM][D_IM_PRO_SEN_OBT_CH_NUM] = {{0}};
	ULONG					pwch_intf[E_IM_PRO_PWCH_MAX] = {0};
	T_IM_PRO_PWCH_INT_INFO	pwch_info = {0};
	ULONG callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Int_Handler begin\n" ));
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	Im_Pro_Dsb();
	im_pro_sen_int_handler_print( 0 );

	/***********/
	/* PWCH    */
	/***********/
	// Get & clear interrupt flag
	pwch_info.unit_no		= E_IM_PRO_UNIT_NUM_1;
	pwch_info.block_type	= E_IM_PRO_BLOCK_TYPE_SEN;
	pwch_info.reg_ptr		= IO_PRO.SEN.PWCH;
	im_pro_pwch_clear_int_factor( &pwch_info, pwch_intf, E_IM_PRO_PWCH_MAX );

	/***********/
	/* LDIV    */
	/***********/
	// Get & clear interrupt flag
	for( loopcnt = 0; loopcnt < D_IM_PRO_SEN_LDIV_NUM; loopcnt++ ) {
		ldiv_intf[loopcnt]	= IO_PRO.SEN.LDIV[loopcnt].LDIVINTFLG.word & IO_PRO.SEN.LDIV[loopcnt].LDIVINTENB.word;
		IO_PRO.SEN.LDIV[loopcnt].LDIVINTFLG.word = ldiv_intf[loopcnt];
	}

	/***********/
	/* OBD/OBT */
	/***********/
	// Get & clear interrupt flag
	for( loopcnt = 0; loopcnt < D_IM_PRO_SENTOP_BLOB_NUM; loopcnt++ ) {
		for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_SEN_OBD_CH_NUM; loopcnt_ch++ ) {
			obd_intf[loopcnt][loopcnt_ch]	= IO_PRO.SEN.OBD[loopcnt][loopcnt_ch].OBDINTFLG.word & IO_PRO.SEN.OBD[loopcnt][loopcnt_ch].OBDINTENB.word;
			IO_PRO.SEN.OBD[loopcnt][loopcnt_ch].OBDINTFLG.word = obd_intf[loopcnt][loopcnt_ch];

			if( ( obd_intf[loopcnt][loopcnt_ch] & D_IM_PRO_OBDINTFLG_OBDF ) != 0 ) {
				im_pro_comm_update_obd_result( loopcnt, loopcnt_ch );
			}
		}

		for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_SEN_OBT_CH_NUM; loopcnt_ch++ ) {
			obt_intf[loopcnt][loopcnt_ch]	= IO_PRO.SEN.OBT[loopcnt][loopcnt_ch].OBTINTFLG.word & IO_PRO.SEN.OBT[loopcnt][loopcnt_ch].OBTINTENB.word;
			IO_PRO.SEN.OBT[loopcnt][loopcnt_ch].OBTINTFLG.word = obt_intf[loopcnt][loopcnt_ch];

			if( ( obt_intf[loopcnt][loopcnt_ch] & D_IM_PRO_OBTINTFLG_OBTF ) != 0 ) {
				im_pro_comm_update_obt_result( loopcnt, loopcnt_ch );
			}
		}
	}

	Im_Pro_Dsb();
	im_pro_sen_int_handler_print( 1 );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Int_Handler Int factor PWCH[0]=0x%lx PWCH[1]=0x%lx PWCH[2]=0x%lx PWCH[3]=0x%lx PWCH[4]=0x%lx PWCH[5]=0x%lx PWCH[6]=0x%lx PWCH[7]=0x%lx\n", pwch_intf[0], pwch_intf[1], pwch_intf[2], pwch_intf[3], pwch_intf[4], pwch_intf[5], pwch_intf[6], pwch_intf[7] ));
	Ddim_Print(( "Im_PRO_SEN_Int_Handler Int factor LDIV[0]=0x%lx LDIV[1]=0x%lx LDIV[2]=0x%lx LDIV[3]=0x%lx \n", ldiv_intf[0], ldiv_intf[1], ldiv_intf[2], ldiv_intf[3] ));
	Ddim_Print(( "Im_PRO_SEN_Int_Handler Int factor OBD[0][0]=0x%lx OBD[0][1]=0x%lx OBD[1][0]=0x%lx OBD[1][1]=0x%lx OBD[2][0]=0x%lx OBD[2][1]=0x%lx OBD[3][0]=0x%lx OBD[3][1]=0x%lx\n", obd_intf[0][0], obd_intf[0][1], obd_intf[1][0], obd_intf[1][1], obd_intf[2][0], obd_intf[2][1], obd_intf[3][0], obd_intf[3][1] ));
	Ddim_Print(( "Im_PRO_SEN_Int_Handler Int factor OBT[0][0]=0x%lx OBT[0][1]=0x%lx OBT[1][0]=0x%lx OBT[1][1]=0x%lx OBT[2][0]=0x%lx OBT[2][1]=0x%lx OBT[3][0]=0x%lx OBT[3][1]=0x%lx\n", obt_intf[0][0], obt_intf[0][1], obt_intf[1][0], obt_intf[1][1], obt_intf[2][0], obt_intf[2][1], obt_intf[3][0], obt_intf[3][1] ));
#endif
	/***********/
	/* PWCH    */
	/***********/
	for( loopcnt_ch = 0; loopcnt_ch < E_IM_PRO_PWCH_MAX; loopcnt_ch++ ) {
		im_pro_pwch_callback_int_factor( 0, E_IM_PRO_BLOCK_TYPE_SEN, (E_IM_PRO_PWCH)loopcnt_ch, pwch_intf );
	}
	Im_Pro_Dsb();

	/***********/
	/* LDIV    */
	/***********/
	for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_SEN_LDIV_NUM; loopcnt_ch++ ) {
		callback_result = 0;
		for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_ldiv_int) / sizeof(gim_pro_inttbl_sen_ldiv_int[0])); loopcnt++ ) {
			if( (ldiv_intf[loopcnt_ch] & gim_pro_inttbl_sen_ldiv_int[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_sen_ldiv_int[loopcnt].int_fact;
			}
		}
		// Callback
		if (( gIM_PRO_LDIV_CallBack_Func[loopcnt_ch].p_callback != NULL ) && ( callback_result != 0 )){
			cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1;
			cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN;
			cb_id.channel		= loopcnt_ch;
			gIM_PRO_LDIV_CallBack_Func[loopcnt_ch].p_callback( &cb_id, callback_result, gIM_PRO_LDIV_CallBack_Func[loopcnt_ch].user_param );
		}
	}
	Im_Pro_Dsb();

	/***********/
	/* OBD     */
	/***********/
	for( loopcnt_block = 0; loopcnt_block < D_IM_PRO_SENTOP_BLOB_NUM; loopcnt_block++ ) {
		for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_SEN_OBD_CH_NUM; loopcnt_ch++ ) {
			callback_result = 0;
			for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_obd_int) / sizeof(gim_pro_inttbl_sen_obd_int[0])); loopcnt++ ) {
				if( (obd_intf[loopcnt_block][loopcnt_ch] & gim_pro_inttbl_sen_obd_int[loopcnt].flg_bitmask) != 0 ) {
					callback_result	|= gim_pro_inttbl_sen_obd_int[loopcnt].int_fact;
				}
			}
			// Callback
			if (( gIM_PRO_OBD_CallBack_Func[loopcnt_block][loopcnt_ch].p_callback != NULL ) && ( callback_result != 0 )){
				cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1;
				cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN;
				im_pro_comm_get_obd_ch(loopcnt_block, loopcnt_ch, (E_IM_PRO_OBD_CH*)&cb_id.channel);
				gIM_PRO_OBD_CallBack_Func[loopcnt_block][loopcnt_ch].p_callback( &cb_id, callback_result, gIM_PRO_OBD_CallBack_Func[loopcnt_block][loopcnt_ch].user_param );
			}
		}
	}
	Im_Pro_Dsb();

	/***********/
	/* OBT     */
	/***********/
	for( loopcnt_block = 0; loopcnt_block < D_IM_PRO_SENTOP_BLOB_NUM; loopcnt_block++ ) {
		for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_SEN_OBT_CH_NUM; loopcnt_ch++ ) {
			callback_result = 0;
			for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_obt_int) / sizeof(gim_pro_inttbl_sen_obt_int[0])); loopcnt++ ) {
				if( (obt_intf[loopcnt_block][loopcnt_ch] & gim_pro_inttbl_sen_obt_int[loopcnt].flg_bitmask) != 0 ) {
					callback_result	|= gim_pro_inttbl_sen_obt_int[loopcnt].int_fact;
				}
			}
			// Callback
			if (( gIM_PRO_OBT_CallBack_Func[loopcnt_block][loopcnt_ch].p_callback != NULL ) && ( callback_result != 0 )){
				cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1;
				cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_SEN;
				im_pro_comm_get_obt_ch(loopcnt_block, loopcnt_ch, (E_IM_PRO_OBT_CH*)&cb_id.channel);
				gIM_PRO_OBT_CallBack_Func[loopcnt_block][loopcnt_ch].p_callback( &cb_id, callback_result, gIM_PRO_OBT_CallBack_Func[loopcnt_block][loopcnt_ch].user_param );
			}
		}
	}
	Im_Pro_Dsb();
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Int_Handler end\n" ));
#endif
}

static VOID im_pro_sen_vd_int_handler_print( UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_sen_vd_int_handler_print() VHDINTFLG1(0x%08x) VHDINTENB1(0x%08x) (status:%u)\n", IO_PRO.SEN.SENTOP.VHDINTFLG.VHDINTFLG1.word, IO_PRO.SEN.SENTOP.VHDINTENB.VHDINTENB1.word, status ));
	Ddim_Print(( "im_pro_sen_vd_int_handler_print() SGVHDINTFLG(0x%08x) SGVHDINTENB(0x%08x) (status:%u)\n", IO_PRO.SEN.SENTOP.SGVHDINTFLG.word, IO_PRO.SEN.SENTOP.SGVHDINTENB.word, status ));
	Ddim_Print(( "im_pro_sen_vd_int_handler_print() GINTFLG(0x%08x) GINTENB(0x%08x) (status:%u)\n", IO_PRO.SEN.SENTOP.GINTFLG.word, IO_PRO.SEN.SENTOP.GINTENB.word, status ));
#endif
}

/**
SEN block VD Int handler.
*/
VOID Im_PRO_SEN_VD_Int_Handler( VOID )
{
	UINT32			loopcnt;
	ULONG			vd_intf;
	ULONG			sg_vd_intf;
	ULONG			gyro_intf;
	ULONG callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= E_IM_PRO_UNIT_NUM_1,
		.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		.channel	= 0,
	};
	UINT32			corecnt;
	UCHAR			coresize;

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_VD_Int_Handler begin\n" ));
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	Im_Pro_Dsb();
	im_pro_sen_vd_int_handler_print( 0 );

	// Get interrupt flag
	vd_intf		= IO_PRO.SEN.SENTOP.VHDINTFLG.VHDINTFLG1.word & IO_PRO.SEN.SENTOP.VHDINTENB.VHDINTENB1.word;
	sg_vd_intf	= IO_PRO.SEN.SENTOP.SGVHDINTFLG.word & IO_PRO.SEN.SENTOP.SGVHDINTENB.word & D_IM_PRO_INT_SGVD_ALL;
	gyro_intf	= IO_PRO.SEN.SENTOP.GINTFLG.word & IO_PRO.SEN.SENTOP.GINTENB.word;

	// Clear interrupt flag
	IO_PRO.SEN.SENTOP.VHDINTFLG.VHDINTFLG1.word	= vd_intf;
	IO_PRO.SEN.SENTOP.SGVHDINTFLG.word			= sg_vd_intf;
	IO_PRO.SEN.SENTOP.GINTFLG.word				= gyro_intf;

	Im_Pro_Dsb();
	im_pro_sen_vd_int_handler_print( 1 );

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	/***********/
	/* VD Edge */
	/***********/
	coresize = ( sizeof(gim_pro_inttbl_sen_vd) / sizeof(gim_pro_inttbl_sen_vd[0]) / D_IM_PRO_SEN_SENCORE_NUM );
	for( corecnt = 0; corecnt < ( D_IM_PRO_SEN_SENCORE_NUM * coresize ); corecnt += coresize ){
		callback_result = 0;
		for( loopcnt = corecnt; loopcnt < ( corecnt + coresize ); loopcnt++ ) {
			if( (vd_intf & gim_pro_inttbl_sen_vd[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_sen_vd[loopcnt].int_fact;
			}
		}
		cb_id.channel = corecnt / coresize;
		// Callback
		if (( gIM_PRO_SENTOP_VD_CallBack_Func.p_callback != NULL ) && ( callback_result != 0 )){
			gIM_PRO_SENTOP_VD_CallBack_Func.p_callback( &cb_id, callback_result, gIM_PRO_SENTOP_VD_CallBack_Func.user_param );
		}
	}

	/**************/
	/* SG VD Edge */
	/**************/
	coresize = ( sizeof(gim_pro_inttbl_sen_sg_vd) / sizeof(gim_pro_inttbl_sen_sg_vd[0]) / D_IM_PRO_SEN_SENCORE_NUM );
	for( corecnt = 0; corecnt < ( D_IM_PRO_SEN_SENCORE_NUM * coresize ); corecnt += coresize ){
		callback_result = 0;
		for( loopcnt = corecnt; loopcnt < ( corecnt + coresize ); loopcnt++ ) {
			if( (sg_vd_intf & gim_pro_inttbl_sen_sg_vd[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_sen_sg_vd[loopcnt].int_fact;
			}
		}
		cb_id.channel = corecnt / coresize;
		// Callback
		if (( gIM_PRO_SG_VD_CallBack_Func.p_callback != NULL ) && ( callback_result != 0 )){
			gIM_PRO_SG_VD_CallBack_Func.p_callback( &cb_id, callback_result, gIM_PRO_SG_VD_CallBack_Func.user_param );
		}
	}

	/**************/
	/* Gyro Edge  */
	/**************/
	coresize = ( sizeof(gim_pro_inttbl_sen_gyro) / sizeof(gim_pro_inttbl_sen_gyro[0]) / D_IM_PRO_SEN_SENCORE_NUM );
	for( corecnt = 0; corecnt < ( D_IM_PRO_SEN_SENCORE_NUM * coresize ); corecnt += coresize ){
		callback_result = 0;
		for( loopcnt = corecnt; loopcnt < ( corecnt + coresize ); loopcnt++ ) {
			if( (gyro_intf & gim_pro_inttbl_sen_gyro[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_sen_gyro[loopcnt].int_fact;
			}
		}
		cb_id.channel = corecnt / coresize;
		// Callback
		if (( gIM_PRO_SENTOP_Gyro_CallBack_Func.p_callback != NULL ) && ( callback_result != 0 )){
			gIM_PRO_SENTOP_Gyro_CallBack_Func.p_callback( &cb_id, callback_result, gIM_PRO_SENTOP_Gyro_CallBack_Func.user_param );
		}
	}
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_VD_Int_Handler end\n" ));
#endif
}

static VOID im_pro_sen_hd_int_handler_print( UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_sen_hd_int_handler_print() VHDINTFLG2 (0x%08x) VHDINTENB2 (0x%08x) (status:%u)\n", IO_PRO.SEN.SENTOP.VHDINTFLG.VHDINTFLG2.word, IO_PRO.SEN.SENTOP.VHDINTENB.VHDINTENB2.word, status ));
	Ddim_Print(( "im_pro_sen_hd_int_handler_print() SGVHDINTFLG(0x%08x) SGVHDINTENB(0x%08x) (status:%u)\n", IO_PRO.SEN.SENTOP.SGVHDINTFLG.word, IO_PRO.SEN.SENTOP.SGVHDINTENB.word, status ));
#endif
}

/**
SEN block HD Int handler.
*/
VOID Im_PRO_SEN_HD_Int_Handler( VOID )
{
	UINT32			loopcnt;
	ULONG			hd_intf;
	ULONG			sg_hd_intf;
	ULONG			callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= E_IM_PRO_UNIT_NUM_1,
		.block_type	= E_IM_PRO_BLOCK_TYPE_SEN,
		.channel	= 0,
	};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_HD_Int_Handler begin\n" ));
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	Im_Pro_Dsb();
	im_pro_sen_hd_int_handler_print( 0 );

	// Get interrupt flag
	hd_intf		= IO_PRO.SEN.SENTOP.VHDINTFLG.VHDINTFLG2.word & IO_PRO.SEN.SENTOP.VHDINTENB.VHDINTENB2.word;
	sg_hd_intf	= IO_PRO.SEN.SENTOP.SGVHDINTFLG.word & IO_PRO.SEN.SENTOP.SGVHDINTENB.word & D_IM_PRO_INT_SGHD_ALL;

	// Clear interrupt flag
	IO_PRO.SEN.SENTOP.VHDINTFLG.VHDINTFLG1.word	= hd_intf;
	IO_PRO.SEN.SENTOP.SGVHDINTFLG.word			= sg_hd_intf;

	Im_Pro_Dsb();
	im_pro_sen_hd_int_handler_print( 1 );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	/***********/
	/* HD Edge */
	/***********/
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_hd) / sizeof(gim_pro_inttbl_sen_hd[0])); loopcnt++ ) {
		if( (hd_intf & gim_pro_inttbl_sen_hd[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sen_hd[loopcnt].int_fact;

		}
	}
	// Callback
	if (( gIM_PRO_SENTOP_HD_CallBack_Func.p_callback != NULL ) && ( callback_result != 0 )){
		gIM_PRO_SENTOP_HD_CallBack_Func.p_callback( &cb_id, callback_result, gIM_PRO_SENTOP_HD_CallBack_Func.user_param );
	}

	/***********/
	/* SG HD Edge */
	/***********/
	callback_result = 0;
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_sen_sg_hd) / sizeof(gim_pro_inttbl_sen_sg_hd[0])); loopcnt++ ) {
		if( (sg_hd_intf & gim_pro_inttbl_sen_sg_hd[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_sen_sg_hd[loopcnt].int_fact;

		}
	}
	// Callback
	if (( gIM_PRO_SG_HD_CallBack_Func.p_callback != NULL ) && ( callback_result != 0 )){
		gIM_PRO_SG_HD_CallBack_Func.p_callback( &cb_id, callback_result, gIM_PRO_SG_HD_CallBack_Func.user_param );
	}
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_HD_Int_Handler end\n" ));
#endif
}

/**
SEN block Sensor I/F 0 Int handler.
*/
VOID Im_PRO_SEN_Sensor_If_Int_Handler_0( VOID )
{
	T_IM_PRO_SENCORE_INT_FACTOR	sencore_int_factor = {0};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Sensor_If_Int_Handler_0 begin\n" ));
#endif
	im_pro_clear_sensor_if_int_factor( 0, &sencore_int_factor );
	im_pro_callback_sensor_if_int( 0, &sencore_int_factor );

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Sensor_If_Int_Handler_0 end\n" ));
#endif
}

/**
SEN block Sensor I/F 1 Int handler.
*/
VOID Im_PRO_SEN_Sensor_If_Int_Handler_1( VOID )
{
	T_IM_PRO_SENCORE_INT_FACTOR	sencore_int_factor = {0};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Sensor_If_Int_Handler_1 begin\n" ));
#endif
	im_pro_clear_sensor_if_int_factor( 1, &sencore_int_factor );
	im_pro_callback_sensor_if_int( 1, &sencore_int_factor );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Sensor_If_Int_Handler_1 end\n" ));
#endif
}

/**
SEN block Sensor I/F 2 Int handler.
*/
VOID Im_PRO_SEN_Sensor_If_Int_Handler_2( VOID )
{
	T_IM_PRO_SENCORE_INT_FACTOR	sencore_int_factor = {0};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Sensor_If_Int_Handler_2 begin\n" ));
#endif
	im_pro_clear_sensor_if_int_factor( 2, &sencore_int_factor );
	im_pro_callback_sensor_if_int( 2, &sencore_int_factor );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Sensor_If_Int_Handler_2 end\n" ));
#endif
}

/**
SEN block Sensor I/F 3 Int handler.
*/
VOID Im_PRO_SEN_Sensor_If_Int_Handler_3( VOID )
{
	T_IM_PRO_SENCORE_INT_FACTOR	sencore_int_factor = {0};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Sensor_If_Int_Handler_3 begin\n" ));
#endif
	im_pro_clear_sensor_if_int_factor( 3, &sencore_int_factor );
	im_pro_callback_sensor_if_int( 3, &sencore_int_factor );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SEN_Sensor_If_Int_Handler_3 end\n" ));
#endif
}

/**
Set SLVS-EC Common Interrupt flg
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SLVS_Set_Common_Interrupt( T_IM_PRO_INT_CFG* int_cfg )
{
	union io_slvsec_peintst	slvsec_peintst;
	union io_slvsec_peinten	slvsec_peinten;
#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Set_Common_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_SLVS_Set_Common_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	slvsec_peintst.word	= 0;
	slvsec_peinten.word	= IO_PRO.SEN.SLVSEC.COMMON.PEINTEN.word;

	// SLVS-EC IPC errot detect interrupt
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE0_BIT)){
		slvsec_peintst.bit.__IPC0	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IPC0		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE1_BIT)){
		slvsec_peintst.bit.__IPC1	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IPC1		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE2_BIT)){
		slvsec_peintst.bit.__IPC2	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IPC2		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE3_BIT)){
		slvsec_peintst.bit.__IPC3	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IPC3		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE4_BIT)){
		slvsec_peintst.bit.__IPC4	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IPC4		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE5_BIT)){
		slvsec_peintst.bit.__IPC5	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IPC5		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE6_BIT)){
		slvsec_peintst.bit.__IPC6	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IPC6		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE7_BIT)){
		slvsec_peintst.bit.__IPC7	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IPC7		= int_cfg->permission_flg;
	}

	// SLVS-EC RDE errot detect interrupt
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE0_BIT)){
		slvsec_peintst.bit.__RDE0	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.RDE0		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE1_BIT)){
		slvsec_peintst.bit.__RDE1	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.RDE1		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE2_BIT)){
		slvsec_peintst.bit.__RDE2	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.RDE2		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE3_BIT)){
		slvsec_peintst.bit.__RDE3	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.RDE3		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE4_BIT)){
		slvsec_peintst.bit.__RDE4	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.RDE4		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE5_BIT)){
		slvsec_peintst.bit.__RDE5	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.RDE5		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE6_BIT)){
		slvsec_peintst.bit.__RDE6	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.RDE6		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE7_BIT)){
		slvsec_peintst.bit.__RDE7	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.RDE7		= int_cfg->permission_flg;
	}

	// SLVS-EC IC errot detect interrupt
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE0_BIT)){
		slvsec_peintst.bit.__IC0	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IC0		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE1_BIT)){
		slvsec_peintst.bit.__IC1	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IC1		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE2_BIT)){
		slvsec_peintst.bit.__IC2	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IC2		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE3_BIT)){
		slvsec_peintst.bit.__IC3	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IC3		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE4_BIT)){
		slvsec_peintst.bit.__IC4	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IC4		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE5_BIT)){
		slvsec_peintst.bit.__IC5	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IC5		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE6_BIT)){
		slvsec_peintst.bit.__IC6	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IC6		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE7_BIT)){
		slvsec_peintst.bit.__IC7	= D_IM_PRO_INTFLG_CLEAR;
		slvsec_peinten.bit.IC7		= int_cfg->permission_flg;
	}

	IO_PRO.SEN.SLVSEC.COMMON.PEINTST.word		= slvsec_peintst.word;
	IO_PRO.SEN.SLVSEC.COMMON.PEINTEN.word		= slvsec_peinten.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
SLVS-EC Common Int handler setting.
@param[in]	inthandler : Interrupt handler address pointer 
*/
INT32 Im_PRO_SLVS_Set_Common_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_SLVS_Common_CallBack_Func.p_callback = callback_cfg->inthandler;
	gIM_PRO_SLVS_Common_CallBack_Func.user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
Set SLVS-EC Interrupt flg
@param[in]	stream_type : stream type of SLVS
@param[in]	int_type : interrupt type of SLVS
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SLVS_Set_Interrupt( E_IM_PRO_SLVS_STREAM_TYPE stream_type, E_IM_PRO_SLVS_INT_TYPE int_type, T_IM_PRO_INT_CFG* int_cfg )
{
	union io_slvsec_intst		slvsec_intst;
	union io_slvsec_inten		slvsec_inten;
	union io_slvsec_leintst		slvsec_leintst;
	union io_slvsec_leinten		slvsec_leinten;
	union io_slvsec_meintst0	slvsec_meintst0;
	union io_slvsec_meinten0	slvsec_meinten0;
	union io_slvsec_meintst1	slvsec_meintst1;
	union io_slvsec_meinten1	slvsec_meinten1;

#ifdef CO_PARAM_CHECK
	if (stream_type >= E_IM_PRO_SLVS_STREAM_TYPE_BOTH){
		Ddim_Assertion(("I:Im_PRO_SLVS_Set_Interrupt. stream_type value error!! (%d)\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_SLVS_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	switch( int_type ) {
		case E_IM_PRO_SLVS_INT_TYPE_INTN_O:
			slvsec_intst.word	= 0;
			slvsec_inten.word	= IO_PRO.SEN.SLVSEC.STREAM[stream_type].INTEN.word;

			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_INTST_STBY_BIT)){
				slvsec_intst.bit.STBY	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_inten.bit.STBY	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_INTST_RDY_BIT)){
				slvsec_intst.bit.RDY	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_inten.bit.RDY	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_INTST_FEO_BIT)){
				slvsec_intst.bit.FEO	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_inten.bit.FEO	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_INTST_FEI_BIT)){
				slvsec_intst.bit.FEI	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_inten.bit.FEI	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_INTST_FSO_BIT)){
				slvsec_intst.bit.FSO	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_inten.bit.FSO	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_INTST_FSI_BIT)){
				slvsec_intst.bit.FSI	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_inten.bit.FSI	= int_cfg->permission_flg;
			}

			IO_PRO.SEN.SLVSEC.STREAM[stream_type].INTST.word	= slvsec_intst.word;
			IO_PRO.SEN.SLVSEC.STREAM[stream_type].INTEN.word	= slvsec_inten.word;
			Im_Pro_Dsb();
			break;

		case E_IM_PRO_SLVS_INT_TYPE_LINK_ERR:
			slvsec_leintst.word	= 0;
			slvsec_leinten.word	= IO_PRO.SEN.SLVSEC.STREAM[stream_type].LEINTEN.word;

			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_LLE_BIT)){
				slvsec_leintst.bit.LLE		= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.LLE		= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_PCRCE_BIT)){
				slvsec_leintst.bit.PCRCE	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.PCRCE	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_HCRC1C_BIT)){
				slvsec_leintst.bit.HCRC1C	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.HCRC1C	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_HCRC2C_BIT)){
				slvsec_leintst.bit.HCRC2C	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.HCRC2C	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_HCRCE_BIT)){
				slvsec_leintst.bit.HCRCE	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.HCRCE	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_ECC1C_BIT)){
				slvsec_leintst.bit.ECC1C	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.ECC1C	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_ECC2C_BIT)){
				slvsec_leintst.bit.ECC2C	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.ECC2C	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_ECCE_BIT)){
				slvsec_leintst.bit.ECCE		= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.ECCE		= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_LNE_BIT)){
				slvsec_leintst.bit.LNE		= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.LNE		= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_LEINTEN_LBOVF_BIT)){
				slvsec_leintst.bit.LBOVF	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_leinten.bit.LBOVF	= int_cfg->permission_flg;
			}

			IO_PRO.SEN.SLVSEC.STREAM[stream_type].LEINTST.word	= slvsec_leintst.word;
			IO_PRO.SEN.SLVSEC.STREAM[stream_type].LEINTEN.word	= slvsec_leinten.word;
			Im_Pro_Dsb();
			break;

		case E_IM_PRO_SLVS_INT_TYPE_MAC_ERR1:
			slvsec_meintst0.word	= 0;
			slvsec_meinten0.word	= IO_PRO.SEN.SLVSEC.STREAM[stream_type].MEINTEN0.word;

			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN0_ECL_BIT)){
				slvsec_meintst0.bit.ECL		= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten0.bit.ECL		= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN0_SCL_BIT)){
				slvsec_meintst0.bit.SCL		= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten0.bit.SCL		= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN0_DCL_BIT)){
				slvsec_meintst0.bit.DCL		= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten0.bit.DCL		= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN0_DSD_BIT)){
				slvsec_meintst0.bit.DSD		= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten0.bit.DSD		= int_cfg->permission_flg;
			}

			IO_PRO.SEN.SLVSEC.STREAM[stream_type].MEINTST0.word	= slvsec_meintst0.word;
			IO_PRO.SEN.SLVSEC.STREAM[stream_type].MEINTEN0.word	= slvsec_meinten0.word;
			Im_Pro_Dsb();
			break;

		//case E_IM_PRO_SLVS_INT_TYPE_MAC_ERR2:
		default:
			slvsec_meintst1.word	= 0;
			slvsec_meinten1.word	= IO_PRO.SEN.SLVSEC.STREAM[stream_type].MEINTEN1.word;

			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE0_BIT)){
				slvsec_meintst1.bit.FOVF0	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FOVF0	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE1_BIT)){
				slvsec_meintst1.bit.FOVF1	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FOVF1	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE2_BIT)){
				slvsec_meintst1.bit.FOVF2	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FOVF2	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE3_BIT)){
				slvsec_meintst1.bit.FOVF3	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FOVF3	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE4_BIT)){
				slvsec_meintst1.bit.FOVF4	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FOVF4	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE5_BIT)){
				slvsec_meintst1.bit.FOVF5	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FOVF5	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE6_BIT)){
				slvsec_meintst1.bit.FOVF6	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FOVF6	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE7_BIT)){
				slvsec_meintst1.bit.FOVF7	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FOVF7	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE0_BIT)){
				slvsec_meintst1.bit.FUNF0	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FUNF0	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE1_BIT)){
				slvsec_meintst1.bit.FUNF1	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FUNF1	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE2_BIT)){
				slvsec_meintst1.bit.FUNF2	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FUNF2	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE3_BIT)){
				slvsec_meintst1.bit.FUNF3	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FUNF3	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE4_BIT)){
				slvsec_meintst1.bit.FUNF4	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FUNF4	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE5_BIT)){
				slvsec_meintst1.bit.FUNF5	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FUNF5	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE6_BIT)){
				slvsec_meintst1.bit.FUNF6	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FUNF6	= int_cfg->permission_flg;
			}
			if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE7_BIT)){
				slvsec_meintst1.bit.FUNF7	= D_IM_PRO_INTFLG_CLEAR;
				slvsec_meinten1.bit.FUNF7	= int_cfg->permission_flg;
			}

			IO_PRO.SEN.SLVSEC.STREAM[stream_type].MEINTST1.word	= slvsec_meintst1.word;
			IO_PRO.SEN.SLVSEC.STREAM[stream_type].MEINTEN1.word	= slvsec_meinten1.word;
			Im_Pro_Dsb();
			break;
	}

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
SLVS-EC Int handler setting.
@param[in]	stream_type : stream type of SLVS
@param[in]	inthandler : Interrupt handler address pointer 
*/
INT32 Im_PRO_SLVS_Set_Int_Handler( E_IM_PRO_SLVS_STREAM_TYPE stream_type, E_IM_PRO_SLVS_INT_TYPE int_type, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
#ifdef CO_PARAM_CHECK
	if (stream_type > E_IM_PRO_SLVS_STREAM_TYPE_B){
		Ddim_Assertion(("I:Im_PRO_SLVS_Set_Int_Handler error. 'stream_type' value over!! (%d)\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	gIM_PRO_SLVS_CallBack_Func[stream_type][int_type].p_callback = callback_cfg->inthandler;
	gIM_PRO_SLVS_CallBack_Func[stream_type][int_type].user_param = callback_cfg->user_param;

	return D_DDIM_OK;
}

/**
LVDS Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_LVDS_Set_Interrupt( E_IM_PRO_LVDS_CH ch, T_IM_PRO_INT_LVDS_CTRL* int_cfg )
{
	union io_lvds_intflg0 lvds_intflg0;
	union io_lvds_intenb0 lvds_intenb0;
	union io_lvds_intflg1 lvds_intflg1;
	union io_lvds_intenb1 lvds_intenb1;
	union io_lvds_intflg2 lvds_intflg2;
	union io_lvds_intenb2 lvds_intenb2;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_LVDS_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	lvds_intflg0.word	= 0;
	lvds_intenb0.word	= IO_PRO.SEN.LVDS[ch].LVDSINTENB0.word;

	if (0 != (int_cfg->lvds_int_enable[0].interrupt_bit & D_IM_PRO_LVDSINTENB_SOFE0)){
		lvds_intflg0.bit.__SOFF0	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb0.bit.SOFE0		= int_cfg->lvds_int_enable[0].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[0].interrupt_bit & D_IM_PRO_LVDSINTENB_EOFE0)){
		lvds_intflg0.bit.__EOFF0	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb0.bit.EOFE0		= int_cfg->lvds_int_enable[0].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[0].interrupt_bit & D_IM_PRO_LVDSINTENB_SOLE0)){
		lvds_intflg0.bit.__SOLF0	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb0.bit.SOLE0		= int_cfg->lvds_int_enable[0].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[0].interrupt_bit & D_IM_PRO_LVDSINTENB_EOLE0)){
		lvds_intflg0.bit.__EOLF0	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb0.bit.EOLE0		= int_cfg->lvds_int_enable[0].permission_flg;
	}

	lvds_intflg1.word	= 0;
	lvds_intenb1.word	= IO_PRO.SEN.LVDS[ch].LVDSINTENB1.word;

	if (0 != (int_cfg->lvds_int_enable[1].interrupt_bit & D_IM_PRO_LVDSINTENB_SOFE1)){
		lvds_intflg1.bit.__SOFF1	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb1.bit.SOFE1		= int_cfg->lvds_int_enable[1].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[1].interrupt_bit & D_IM_PRO_LVDSINTENB_EOFE1)){
		lvds_intflg1.bit.__EOFF1	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb1.bit.EOFE1		= int_cfg->lvds_int_enable[1].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[1].interrupt_bit & D_IM_PRO_LVDSINTENB_SOLE1)){
		lvds_intflg1.bit.__SOLF1	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb1.bit.SOLE1		= int_cfg->lvds_int_enable[1].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[1].interrupt_bit & D_IM_PRO_LVDSINTENB_EOLE1)){
		lvds_intflg1.bit.__EOLF1	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb1.bit.EOLE1		= int_cfg->lvds_int_enable[1].permission_flg;
	}

	lvds_intflg2.word	= 0;
	lvds_intenb2.word	= IO_PRO.SEN.LVDS[ch].LVDSINTENB2.word;

	if (0 != (int_cfg->lvds_int_enable[2].interrupt_bit & D_IM_PRO_LVDSINTENB_SOFE2)){
		lvds_intflg2.bit.__SOFF2	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb2.bit.SOFE2		= int_cfg->lvds_int_enable[2].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[2].interrupt_bit & D_IM_PRO_LVDSINTENB_EOFE2)){
		lvds_intflg2.bit.__EOFF2	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb2.bit.EOFE2		= int_cfg->lvds_int_enable[2].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[2].interrupt_bit & D_IM_PRO_LVDSINTENB_SOLE2)){
		lvds_intflg2.bit.__SOLF2	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb2.bit.SOLE2		= int_cfg->lvds_int_enable[2].permission_flg;
	}
	if (0 != (int_cfg->lvds_int_enable[2].interrupt_bit & D_IM_PRO_LVDSINTENB_EOLE2)){
		lvds_intflg2.bit.__EOLF2	= D_IM_PRO_INTFLG_CLEAR;
		lvds_intenb2.bit.EOLE2		= int_cfg->lvds_int_enable[2].permission_flg;
	}

	IO_PRO.SEN.LVDS[ch].LVDSINTFLG0.word	= lvds_intflg0.word;
	IO_PRO.SEN.LVDS[ch].LVDSINTENB0.word	= lvds_intenb0.word;
	Im_Pro_Dsb();

	IO_PRO.SEN.LVDS[ch].LVDSINTFLG1.word	= lvds_intflg1.word;
	IO_PRO.SEN.LVDS[ch].LVDSINTENB1.word	= lvds_intenb1.word;
	Im_Pro_Dsb();

	IO_PRO.SEN.LVDS[ch].LVDSINTFLG2.word	= lvds_intflg2.word;
	IO_PRO.SEN.LVDS[ch].LVDSINTENB2.word	= lvds_intenb2.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of LVDS is registered
@param[in]	ch : Channel No.
@param[in]	inthandler : LVDS Interrupt callback function address
*/
INT32 Im_PRO_LVDS_Set_Int_Handler( E_IM_PRO_LVDS_CH ch, E_IM_PRO_LVDS_OUTPUT_MODE out_mode, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_LVDS_CallBack_Func[ch][out_mode].p_callback = callback_cfg->inthandler;
	gIM_PRO_LVDS_CallBack_Func[ch][out_mode].user_param = callback_cfg->user_param;

	return D_DDIM_OK;
}


/**
DMIPI Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DMIPI_Set_Interrupt( E_IM_PRO_DMIPI_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	union io_mpiinte mpiinte;
	union io_mpiintf mpiintf;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_DMIPI_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_DMIPI_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	mpiintf.word	= 0;
	mpiinte.word	= IO_PRO.SEN.MIPID[ch].MPIINTE.word;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DEC3)){
		mpiintf.bit.__DECF3	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DECE3	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DEC2)){
		mpiintf.bit.__DECF2	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DECE2	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DEC1)){
		mpiintf.bit.__DECF1	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DECE1	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DEC0)){
		mpiintf.bit.__DECF0	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DECE0	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DEE3)){
		mpiintf.bit.__DEEF3	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DEEE3	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DEE2)){
		mpiintf.bit.__DEEF2	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DEEE2	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DEE1)){
		mpiintf.bit.__DEEF1	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DEEE1	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DEE0)){
		mpiintf.bit.__DEEF0	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DEEE0	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DESSH3)){
		mpiintf.bit.__DESSHF3	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DESSHE3		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DESSH2)){
		mpiintf.bit.__DESSHF2	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DESSHE2		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DESSH1)){
		mpiintf.bit.__DESSHF1	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DESSHE1		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DESSH0)){
		mpiintf.bit.__DESSHF0	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DESSHE0		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DESHE3)){
		mpiintf.bit.__DESHF3	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DESHE3		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DESHE2)){
		mpiintf.bit.__DESHF2	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DESHE2		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DESHE1)){
		mpiintf.bit.__DESHF1	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DESHE1		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_DESHE0)){
		mpiintf.bit.__DESHF0	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.DESHE0		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_FEO1)){
		mpiintf.bit.__FEO1F	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.FEO1E	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_FEO0)){
		mpiintf.bit.__FEO0F	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.FEO0E	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_FEI1)){
		mpiintf.bit.__FEI1F	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.FEI1E	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_FEI0)){
		mpiintf.bit.__FEI0F	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.FEI0E	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_FSO1)){
		mpiintf.bit.__FSO1F	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.FSO1E	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_FSO0)){
		mpiintf.bit.__FSO0F	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.FSO0E	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_FSI1)){
		mpiintf.bit.__FSI1F	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.FSI1E	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_FSI0)){
		mpiintf.bit.__FSI0F	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.FSI0E	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_LZE3)){
		mpiintf.bit.__LZEF3	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.LZEE3	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_LZE2)){
		mpiintf.bit.__LZEF2	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.LZEE2	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_LZE1)){
		mpiintf.bit.__LZEF1	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.LZEE1	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_LZE0)){
		mpiintf.bit.__LZEF0	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.LZEE0	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_EFS)){
		mpiintf.bit.__EFSF	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.EFSE	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_CRCE)){
		mpiintf.bit.__CRCEF	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.CRCEE	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_ECCW)){
		mpiintf.bit.__ECCWF	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.ECCWE	= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MPIINTE_ECCE)){
		mpiintf.bit.__ECCEF	= D_IM_PRO_INTFLG_CLEAR;
		mpiinte.bit.ECCEE	= int_cfg->permission_flg;
	}

	IO_PRO.SEN.MIPID[ch].MPIINTF.word	= mpiintf.word;
	IO_PRO.SEN.MIPID[ch].MPIINTE.word	= mpiinte.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of DMIPI is registered
@param[in]	ch : Channel No.
@param[in]	inthandler : DMIPI Interrupt callback function address
*/
INT32 Im_PRO_DMIPI_Set_Int_Handler( E_IM_PRO_DMIPI_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_DMIPI_CallBack_Func[ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_DMIPI_CallBack_Func[ch].user_param = callback_cfg->user_param;

	return D_DDIM_OK;
}

/**
CMIPI Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_CMIPI_Set_Interrupt( E_IM_PRO_CMIPI_CH ch, T_IM_PRO_INT_CMIPI_CTRL* cmipi_int_ctrl )
{
	union io_mpicinte1 mpicinte1;
	union io_mpicintf1 mpicintf1;
	union io_mpicinte2 mpicinte2;
	union io_mpicintf2 mpicintf2;

#ifdef CO_PARAM_CHECK
	if (cmipi_int_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_CMIPI_Set_Interrupt error. cmipi_int_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (cmipi_int_ctrl->cmipi_int1_enable.permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_CMIPI_Set_Interrupt error. cmipi_int1_enable 'permission_flg' value over!! (%d)\n", cmipi_int_ctrl->cmipi_int1_enable.permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (cmipi_int_ctrl->cmipi_int2_enable.permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_CMIPI_Set_Interrupt error. cmipi_int2_enable 'permission_flg' value over!! (%d)\n", cmipi_int_ctrl->cmipi_int2_enable.permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	// MPICINTE1 interrupt enabled setting
	mpicintf1.word	= 0;
	mpicinte1.word	= IO_PRO.SEN.MIPIC[ch].MPICINTE1.word;

	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CEC2)){
		mpicintf1.bit.__CECF2	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CECE2		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CEC1)){
		mpicintf1.bit.__CECF1	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CECE1		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CEC0)){
		mpicintf1.bit.__CECF0	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CECE0		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CEE2)){
		mpicintf1.bit.__CEEF2	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CEEE2		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CEE1)){
		mpicintf1.bit.__CEEF1	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CEEE1		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CEE0)){
		mpicintf1.bit.__CEEF0	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CEEE0		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CESSH2)){
		mpicintf1.bit.__CESSHF2	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CESSHE2	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CESSH1)){
		mpicintf1.bit.__CESSHF1	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CESSHE1	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CESSH0)){
		mpicintf1.bit.__CESSHF0	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CESSHE0	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CESH2)){
		mpicintf1.bit.__CESHF2	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CESHE2	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CESH1)){
		mpicintf1.bit.__CESHF1	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CESHE1	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CESH0)){
		mpicintf1.bit.__CESHF0	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CESHE0	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_FEO1)){
		mpicintf1.bit.__FEO1F		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.FEO1E		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_FEO0)){
		mpicintf1.bit.__FEO0F		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.FEO0E		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_FEI1)){
		mpicintf1.bit.__FEI1F		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.FEI1E		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_FEI0)){
		mpicintf1.bit.__FEI0F		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.FEI0E		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_FSO1)){
		mpicintf1.bit.__FSO1F		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.FSO1E		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_FSO0)){
		mpicintf1.bit.__FSO0F		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.FSO0E		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_FSI1)){
		mpicintf1.bit.__FSI1F		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.FSI1E		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_FSI0)){
		mpicintf1.bit.__FSI0F		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.FSI0E		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_LZE2)){
		mpicintf1.bit.__LZEF2		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.LZEE2		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_LZE1)){
		mpicintf1.bit.__LZEF1		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.LZEE1		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_LZE0)){
		mpicintf1.bit.__LZEF0		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.LZEE0		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_EFS)){
		mpicintf1.bit.__EFSF		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.EFSE		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_CRCDTE)){
		mpicintf1.bit.__CRCDTEF	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.CRCDTEE	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_PHCRC2E)){
		mpicintf1.bit.__PHCRC2EF	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.PHCRC2EE	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int1_enable.interrupt_bit & D_IM_PRO_MPICINTE1_PHCRC1E)){
		mpicintf1.bit.__PHCRC1EF	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte1.bit.PHCRC1EE	= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}

	// MPICINTE2 interrupt enabled setting
	mpicintf2.word	= 0;
	mpicinte2.word	= IO_PRO.SEN.MIPIC[ch].MPICINTE2.word;

	if (0 != (cmipi_int_ctrl->cmipi_int2_enable.interrupt_bit & D_IM_PRO_MPICINTE2_CICHSE2)){
		mpicintf2.bit.__CICHSF2		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte2.bit.CICHSE2		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int2_enable.interrupt_bit & D_IM_PRO_MPICINTE2_CICHSE1)){
		mpicintf2.bit.__CICHSF1		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte2.bit.CICHSE1		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int2_enable.interrupt_bit & D_IM_PRO_MPICINTE2_CICHSE0)){
		mpicintf2.bit.__CICHSF0		= D_IM_PRO_INTFLG_CLEAR;
		mpicinte2.bit.CICHSE0		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}
	if (0 != (cmipi_int_ctrl->cmipi_int2_enable.interrupt_bit & D_IM_PRO_MPICINTE2_PHCRCAEE)){
		mpicintf2.bit.__PHCRCAEF	= D_IM_PRO_INTFLG_CLEAR;
		mpicinte2.bit.PHCRCAEE		= cmipi_int_ctrl->cmipi_int1_enable.permission_flg;
	}

	IO_PRO.SEN.MIPIC[ch].MPICINTF1.word	= mpicintf1.word;
	IO_PRO.SEN.MIPIC[ch].MPICINTE1.word	= mpicinte1.word;
	Im_Pro_Dsb();

	IO_PRO.SEN.MIPIC[ch].MPICINTF2.word	= mpicintf2.word;
	IO_PRO.SEN.MIPIC[ch].MPICINTE2.word	= mpicinte2.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of CMIPI is registered
@param[in]	ch : Channel No.
@param[in]	inthandler : CMIPI Interrupt callback function address
*/
INT32 Im_PRO_CMIPI_Set_Int_Handler( E_IM_PRO_CMIPI_CH ch, E_IM_PRO_CMIPI_INT_CATE category, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_CMIPI_CallBack_Func[ch][category].p_callback = callback_cfg->inthandler;
	gIM_PRO_CMIPI_CallBack_Func[ch][category].user_param = callback_cfg->user_param;

	return D_DDIM_OK;
}

/**
MONI Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_MONI_Set_Interrupt( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	const T_IM_PRO_COMMON_MONI_INFO* moni_info = 0;
	union io_moniintflg moniintflg;
	union io_moniintenb moniintenb;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_MONI_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_MONI_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_comm_get_moni_reg_info( unit_no, block_type, ch, &moni_info );
#ifdef CO_PARAM_CHECK
	if (moni_info->reg_ptr == NULL){
		Ddim_Assertion(("I:Im_PRO_MONI_Set_Interrupt error. moni_info->reg_ptr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, im_pro_convert_clk_type( block_type ) );
	moniintflg.word	= 0;
	moniintenb.word	= moni_info->reg_ptr->MONIINTENB.word;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MONIINTENB_MONIHDE)){
		moniintflg.bit.__MONIHDF		= D_IM_PRO_INTFLG_CLEAR;
		moniintenb.bit.MONIHDE			= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MONIINTENB_MONIHDE)){
		moniintflg.bit.__MONIEPFF		= D_IM_PRO_INTFLG_CLEAR;
		moniintenb.bit.MONIEPFE			= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MONIINTENB_MONIHDE)){
		moniintflg.bit.__MONIEPLF		= D_IM_PRO_INTFLG_CLEAR;
		moniintenb.bit.MONIEPLE			= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MONIINTENB_MONIHDE)){
		moniintflg.bit.__MONIBLLMINF	= D_IM_PRO_INTFLG_CLEAR;
		moniintenb.bit.MONIBLLMINE		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_MONIINTENB_MONIHDE)){
		moniintflg.bit.__MONIBLLMAXF	= D_IM_PRO_INTFLG_CLEAR;
		moniintenb.bit.MONIBLLMAXE		= int_cfg->permission_flg;
	}

	moni_info->reg_ptr->MONIINTFLG.word	= moniintflg.word;
	moni_info->reg_ptr->MONIINTENB.word	= moniintenb.word;
	Im_Pro_Dsb();
	im_pro_off_pclk( unit_no, im_pro_convert_clk_type( block_type ) );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of MONI is registered
@param[in]	ch : Channel No.
@param[in]	inthandler : MONI Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_MONI_Set_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_MONI_CallBack_Func[unit_no][block_type][ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_MONI_CallBack_Func[unit_no][block_type][ch].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
LDIV Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_LDIV_Set_Interrupt( E_IM_PRO_LDIV_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	union  io_ldivintenb ldivintenb;
	union  io_ldivintflg ldivintflg;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	ldivintflg.word	= 0;
	ldivintenb.word	= IO_PRO.SEN.LDIV[ch].LDIVINTENB.word;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_LDIVINTENB_LDIVEE)){
		ldivintflg.bit.__LDIVEF		= D_IM_PRO_INTFLG_CLEAR;
		ldivintenb.bit.LDIVEE		= int_cfg->permission_flg;
	}
	IO_PRO.SEN.LDIV[ch].LDIVINTFLG.word	= ldivintflg.word;
	IO_PRO.SEN.LDIV[ch].LDIVINTENB.word	= ldivintenb.word;
	Im_Pro_Dsb();
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}


/**
CallBackFunction which Call to the interruption timing of LDIV is registered
@param[in]	ch : Channel No.
@param[in]	inthandler : LDIV Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_LDIV_Set_Int_Handler( E_IM_PRO_LDIV_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_LDIV_CallBack_Func[ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_LDIV_CallBack_Func[ch].user_param = callback_cfg->user_param;

	return D_DDIM_OK;
}

/**
OBT Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBT_Set_Interrupt( E_IM_PRO_OBT_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;
	union io_obtintenb obtintenb;
	union io_obtintflg obtintflg;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_OBT_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_OBT_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	im_pro_comm_get_obt_block_ch( ch, &block_num, &ch_num );

	obtintflg.word	= 0;
	obtintenb.word	= IO_PRO.SEN.OBT[block_num][ch].OBTINTENB.word;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_OBTINTENB_OBTE)){
		obtintflg.bit.__OBTF	= D_IM_PRO_INTFLG_CLEAR;
		obtintenb.bit.OBTE		= int_cfg->permission_flg;
	}

	IO_PRO.SEN.OBT[block_num][ch].OBTINTFLG.word	= obtintflg.word;
	IO_PRO.SEN.OBT[block_num][ch].OBTINTENB.word	= obtintenb.word;
	Im_Pro_Dsb();
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of OBT is registered
@param[in]	ch : Channel No.
@param[in]	inthandler : OBT Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBT_Set_Int_Handler( E_IM_PRO_OBT_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obt_block_ch( ch, &block_num, &ch_num );

	gIM_PRO_OBT_CallBack_Func[block_num][ch_num].p_callback = callback_cfg->inthandler;
	gIM_PRO_OBT_CallBack_Func[block_num][ch_num].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
OBD Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBD_Set_Interrupt( E_IM_PRO_OBD_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;
	union io_obdintenb obdintenb;
	union io_obdintflg obdintflg;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_OBD_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_OBD_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );
	im_pro_comm_get_obd_block_ch( ch, &block_num, &ch_num );

	obdintflg.word	= 0;
	obdintenb.word	= IO_PRO.SEN.OBD[block_num][ch].OBDINTENB.word;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_OBDINTENB_OBDE)){
		obdintflg.bit.__OBDF	= D_IM_PRO_INTFLG_CLEAR;
		obdintenb.bit.OBDE		= int_cfg->permission_flg;
	}

	IO_PRO.SEN.OBD[block_num][ch].OBDINTFLG.word	= obdintflg.word;
	IO_PRO.SEN.OBD[block_num][ch].OBDINTENB.word	= obdintenb.word;
	Im_Pro_Dsb();
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of OB is registered
@param[in]	ch : Channel No.
@param[in]	inthandler : OBD Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBD_Set_Int_Handler( E_IM_PRO_OBD_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obd_block_ch( ch, &block_num, &ch_num );

	gIM_PRO_OBD_CallBack_Func[block_num][ch_num].p_callback = callback_cfg->inthandler;
	gIM_PRO_OBD_CallBack_Func[block_num][ch_num].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
SROTOP Interrupt setting
@param[in]	unit_no : Unit number.
@param[in]	int_ctrl : SROTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_SROTOP_Interrupt_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_INT_SROTOP_CTRL* int_ctrl )
{
	union io_srovhdintenb	vhdintenb;
	union io_srovhdintflg	vhdintflg;

#ifdef CO_PARAM_CHECK
	if (int_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Interrupt_Ctrl error. int_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (( int_ctrl->vd_enable[0].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->vd_enable[1].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->hd_enable[0].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->hd_enable[1].permission_flg > D_IM_PRO_FUNC_EN )
		) {
		Ddim_Assertion(("I:Im_PRO_SROTOP_Interrupt_Ctrl error. 'permission_flg' value over!! (VD0:%d VD1:%d HD0:%d HD1:%d)\n"	, int_ctrl->vd_enable[0].permission_flg
																																, int_ctrl->vd_enable[1].permission_flg
																																, int_ctrl->hd_enable[0].permission_flg
																																, int_ctrl->hd_enable[1].permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );
	vhdintenb.word	= IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTENB.word;
	vhdintflg.word	= 0;

	// VHDINTENB VD Interrupt
	if (0 != (int_ctrl->vd_enable[0].interrupt_bit & D_IM_PRO_SROTOP_INT_VDE0)){
		vhdintflg.bit.__VDF0	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.VDE0		= int_ctrl->vd_enable[0].permission_flg;
	}
	if (0 != (int_ctrl->vd_enable[1].interrupt_bit & D_IM_PRO_SROTOP_INT_VDE1)){
		vhdintflg.bit.__VDF1	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.VDE1		= int_ctrl->vd_enable[1].permission_flg;
	}
	// VHDINTENB HD Interrupt
	if (0 != (int_ctrl->hd_enable[0].interrupt_bit & D_IM_PRO_SROTOP_INT_HDE0)){
		vhdintflg.bit.__HDF0	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.HDE0		= int_ctrl->hd_enable[0].permission_flg;
	}
	if (0 != (int_ctrl->hd_enable[1].interrupt_bit & D_IM_PRO_SROTOP_INT_HDE1)){
		vhdintflg.bit.__HDF1	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.HDE1		= int_ctrl->hd_enable[1].permission_flg;
	}

	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.INTCTL.bit.INTMD	= int_ctrl->int_mode;
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTFLG.word		= vhdintflg.word;
	IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHDINTENB.word		= vhdintenb.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );

	return D_DDIM_OK;
}

/**
SROTOP macro interrupt delay setting
@param[in]	unit_no : Unit number.
@param[in]	ch : channel(0:HD/VD0, 1:HD/VD1)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SROTOP_Set_VHD_Delay( E_IM_PRO_UNIT_NUM unit_no, UCHAR ch, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_PRO_SROTOP_VHD_CH_NUM){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Set_VHD_Delay. ch value error!! (unit_no:%d ch:%d)\n", unit_no, ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Set_VHD_Delay error. delay_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->hd_delay > D_IM_PRO_HMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Set_VHD_Delay hd_delay value error. delay_ctrl->hd_delay=%u\n",delay_ctrl->hd_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_h_delay > D_IM_PRO_VMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Set_VHD_Delay vd_h_delay value error. delay_ctrl->vd_h_delay=%u\n",delay_ctrl->vd_h_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_v_delay > D_IM_PRO_VMIVADR_MAX){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Set_VHD_Delay vd_v_delay value error. delay_ctrl->vd_v_delay=%u\n",delay_ctrl->vd_v_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );
	if( ch == 0 ) {
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.HMIHADR.bit.HMIHADR0	= delay_ctrl->hd_delay;
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VMIHADR.bit.VMIHADR0	= delay_ctrl->vd_h_delay;
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VMIVADR.bit.VMIVADR0	= delay_ctrl->vd_v_delay;
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHSET.bit.VHSET0		= D_IM_PRO_FUNC_EN;
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.HMIHADR.bit.HMIHADR1	= delay_ctrl->hd_delay;
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VMIHADR.bit.VMIHADR1	= delay_ctrl->vd_h_delay;
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VMIVADR.bit.VMIVADR1	= delay_ctrl->vd_v_delay;
		IO_PRO.IMG_PIPE[unit_no].SRO.SROTOP.VHSET.bit.VHSET1		= D_IM_PRO_FUNC_EN;
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );

	return D_DDIM_OK;
}

/**
SRO pipe VD Int handler setting.
@param[in]	unit_no : Unit number.
@param[in]	inthandler : Interrupt handler address pointer 
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_SROTOP_Pipe_Set_VD_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
#ifdef CO_PARAM_CHECK
	if (unit_no >= E_IM_PRO_BOTH_UNIT){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Pipe_Set_VD_Int_Handler. unit no value error!! (unit_no:%d)\n", unit_no));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	gIM_PRO_SRO_VD_CallBack_Func[unit_no].p_callback = callback_cfg->inthandler;
	gIM_PRO_SRO_VD_CallBack_Func[unit_no].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}


/**
SRO pipe HD Int handler setting.
@param[in]	unit_no : Unit number.
@param[in]	inthandler : Interrupt handler address pointer 
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_SROTOP_Pipe_Set_HD_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
#ifdef CO_PARAM_CHECK
	if (unit_no >= E_IM_PRO_BOTH_UNIT){
		Ddim_Assertion(("I:Im_PRO_SROTOP_Pipe_Set_HD_Int_Handler. unit no value error!! (unit_no:%d)\n", unit_no));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	gIM_PRO_SRO_HD_CallBack_Func[unit_no].p_callback = callback_cfg->inthandler;
	gIM_PRO_SRO_HD_CallBack_Func[unit_no].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
SRO Macro Pipe1 Int handler.
*/
VOID Im_PRO_SROTOP_Pipe1_Int_Handler( VOID )
{
	T_IM_PRO_SRO_INT_FACTOR int_factor;

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe1_Int_Handler begin\n" ));
#endif
	im_pro_clear_sro_int_factor( E_IM_PRO_UNIT_NUM_1, &int_factor );
	im_pro_callback_sro_int( E_IM_PRO_UNIT_NUM_1, &int_factor );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe1_Int_Handler end\n" ));
#endif
}

/**
SRO Macro Pipe1 VD Int handler.
*/
VOID Im_PRO_SROTOP_Pipe1_VD_Int_Handler( VOID )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe1_VD_Int_Handler begin\n" ));
#endif
	im_pro_sro_vd_proc( E_IM_PRO_UNIT_NUM_1 );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe1_VD_Int_Handler end\n" ));
#endif
}

/**
SRO Macro Pipe1 HD Int handler.
*/
VOID Im_PRO_SROTOP_Pipe1_HD_Int_Handler( VOID )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe1_HD_Int_Handler begin\n" ));
#endif
	im_pro_sro_hd_proc( E_IM_PRO_UNIT_NUM_1 );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe1_HD_Int_Handler end\n" ));
#endif
}

/**
SRO Macro Pipe2 Int handler.
*/
VOID Im_PRO_SROTOP_Pipe2_Int_Handler( VOID )
{
	T_IM_PRO_SRO_INT_FACTOR int_factor;

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe2_Int_Handler begin\n" ));
#endif
	im_pro_clear_sro_int_factor( E_IM_PRO_UNIT_NUM_2, &int_factor );
	im_pro_callback_sro_int( E_IM_PRO_UNIT_NUM_2, &int_factor );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe2_Int_Handler end\n" ));
#endif
}

/**
SRO Macro Pipe2 VD Int handler.
*/
VOID Im_PRO_SROTOP_Pipe2_VD_Int_Handler( VOID )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe2_VD_Int_Handler begin\n" ));
#endif
	im_pro_sro_vd_proc( E_IM_PRO_UNIT_NUM_2 );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe2_VD_Int_Handler end\n" ));
#endif
}

/**
SRO Macro Pipe2 HD Int handler.
*/
VOID Im_PRO_SROTOP_Pipe2_HD_Int_Handler( VOID )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe2_HD_Int_Handler begin\n" ));
#endif
	im_pro_sro_hd_proc( E_IM_PRO_UNIT_NUM_2 );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_SROTOP_Pipe2_HD_Int_Handler end\n" ));
#endif
}

/**
Set SDC Interrupt flg
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel<br>
	 			 value range:[0 - 1]<br>
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK				:setting ok
@retval		D_IM_PRO_PARAM_ERROR	:setting ng
*/
INT32 Im_PRO_SDC_Set_Interrupt( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	union io_sdcintenb sdcintenb;
	union io_sdcintflg sdcintflg;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_SDC_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_SDC_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );
	sdcintflg.word	= 0;
	sdcintenb.word	= IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCINTENB.word;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_SDCINTENB_SDCE)){
		sdcintflg.bit.__SDCF	= D_IM_PRO_INTFLG_CLEAR;
		sdcintenb.bit.SDCE		= int_cfg->permission_flg;
	}

	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCINTFLG.word	= sdcintflg.word;
	IO_PRO.IMG_PIPE[unit_no].SRO.SDC[ch].SDCINTENB.word	= sdcintenb.word;
	Im_Pro_Dsb();
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_SRO );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of SDC is registered
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel<br>
	 			 value range:[0 - 1]<br>
@param[in]	inthandler : SDC Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NCH_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SDC_Set_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_SDC_CallBack_Func[unit_no][ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_SDC_CallBack_Func[unit_no][ch].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
B2BTOP Interrupt setting
@param[in]	unit_no : Unit number.
@param[in]	int_ctrl : B2BTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_B2BTOP_Interrupt_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_INT_B2BTOP_CTRL* int_ctrl )
{
	union io_b2b_vhdintenb	vhdintenb;
	union io_b2b_vhdintflg	vhdintflg;

#ifdef CO_PARAM_CHECK
	if (int_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Interrupt_Ctrl error. int_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (( int_ctrl->vd_enable[0].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->vd_enable[1].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->hd_enable[0].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->hd_enable[1].permission_flg > D_IM_PRO_FUNC_EN )
		) {
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Interrupt_Ctrl error. 'permission_flg' value over!! (VD0:%d VD1:%d HD0:%d HD1:%d)\n"	, int_ctrl->vd_enable[0].permission_flg
																																, int_ctrl->vd_enable[1].permission_flg
																																, int_ctrl->hd_enable[0].permission_flg
																																, int_ctrl->hd_enable[1].permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );
	vhdintenb.word	= IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTENB.word;
	vhdintflg.word	= 0;

	// VHDINTENB VD Interrupt
	if (0 != (int_ctrl->vd_enable[0].interrupt_bit & D_IM_PRO_B2BTOP_INT_VDE0)){
		vhdintflg.bit.__VDF0	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.VDE0		= int_ctrl->vd_enable[0].permission_flg;
	}
	if (0 != (int_ctrl->vd_enable[1].interrupt_bit & D_IM_PRO_B2BTOP_INT_VDE1)){
		vhdintflg.bit.__VDF1	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.VDE1		= int_ctrl->vd_enable[1].permission_flg;
	}
	// VHDINTENB HD Interrupt
	if (0 != (int_ctrl->hd_enable[0].interrupt_bit & D_IM_PRO_B2BTOP_INT_HDE0)){
		vhdintflg.bit.__HDF0	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.HDE0		= int_ctrl->hd_enable[0].permission_flg;
	}
	if (0 != (int_ctrl->hd_enable[1].interrupt_bit & D_IM_PRO_B2BTOP_INT_HDE1)){
		vhdintflg.bit.__HDF1	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.HDE1		= int_ctrl->hd_enable[1].permission_flg;
	}

	IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.INTCTL.bit.INTMD	= int_ctrl->int_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTFLG.word		= vhdintflg.word;
	IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHDINTENB.word		= vhdintenb.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );

	return D_DDIM_OK;
}

/**
B2BTOP macro interrupt delay setting
@param[in]	unit_no : Unit number.
@param[in]	ch : channel(0:HD/VD0, 1:HD/VD1)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_B2BTOP_Set_VHD_Delay( E_IM_PRO_UNIT_NUM unit_no, UCHAR ch, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_PRO_B2BTOP_VHD_CH_NUM){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Set_VHD_Delay. ch value error!! (unit_no:%d ch:%d)\n", unit_no, ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Set_VHD_Delay error. delay_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->hd_delay > D_IM_PRO_HMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Set_VHD_Delay hd_delay value error. delay_ctrl->hd_delay=%u\n",delay_ctrl->hd_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_h_delay > D_IM_PRO_VMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Set_VHD_Delay vd_h_delay value error. delay_ctrl->vd_h_delay=%u\n",delay_ctrl->vd_h_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_v_delay > D_IM_PRO_VMIVADR_MAX){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Set_VHD_Delay vd_v_delay value error. delay_ctrl->vd_v_delay=%u\n",delay_ctrl->vd_v_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );
	if( ch == 0 ) {
		IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.HMIHADR.bit.HMIHADR0	= delay_ctrl->hd_delay;
		IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VMIHADR.bit.VMIHADR0	= delay_ctrl->vd_h_delay;
		IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VMIVADR.bit.VMIVADR0	= delay_ctrl->vd_v_delay;
		IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHSET.bit.VHSET0		= D_IM_PRO_FUNC_EN;
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.HMIHADR.bit.HMIHADR1	= delay_ctrl->hd_delay;
		IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VMIHADR.bit.VMIHADR1	= delay_ctrl->vd_h_delay;
		IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VMIVADR.bit.VMIVADR1	= delay_ctrl->vd_v_delay;
		IO_PRO.IMG_PIPE[unit_no].B2B.B2BTOP.VHSET.bit.VHSET1		= D_IM_PRO_FUNC_EN;
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );

	return D_DDIM_OK;
}

/**
B2B pipe VD Int handler setting.
@param[in]	unit_no : Unit number.
@param[in]	inthandler : Interrupt handler address pointer 
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_B2BTOP_Pipe_Set_VD_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
#ifdef CO_PARAM_CHECK
	if (unit_no >= E_IM_PRO_BOTH_UNIT){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Pipe_Set_VD_Int_Handler. unit no value error!! (unit_no:%d)\n", unit_no));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	gIM_PRO_B2B_VD_CallBack_Func[unit_no].p_callback = callback_cfg->inthandler;
	gIM_PRO_B2B_VD_CallBack_Func[unit_no].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
B2B pipe HD Int handler setting.
@param[in]	unit_no : Unit number.
@param[in]	inthandler : Interrupt handler address pointer 
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_B2BTOP_Pipe_Set_HD_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
#ifdef CO_PARAM_CHECK
	if (unit_no >= E_IM_PRO_BOTH_UNIT){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Pipe_Set_HD_Int_Handler. unit no value error!! (unit_no:%d)\n", unit_no));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	gIM_PRO_B2B_HD_CallBack_Func[unit_no].p_callback = callback_cfg->inthandler;
	gIM_PRO_B2B_HD_CallBack_Func[unit_no].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
B2B Macro Pipe1 Int handler.
*/
VOID Im_PRO_B2BTOP_Pipe1_Int_Handler( VOID )
{
	T_IM_PRO_B2B_INT_FACTOR int_factor;

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe1_Int_Handler begin\n" ));
#endif
	im_pro_clear_b2b_int_factor( E_IM_PRO_UNIT_NUM_1, &int_factor );
	im_pro_callback_b2b_int( E_IM_PRO_UNIT_NUM_1, &int_factor );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe1_Int_Handler end\n" ));
#endif
}

/**
B2B Macro Pipe1 VD Int handler.
*/
VOID Im_PRO_B2BTOP_Pipe1_VD_Int_Handler( VOID )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe1_VD_Int_Handler begin\n" ));
#endif
	im_pro_b2b_vd_proc( E_IM_PRO_UNIT_NUM_1 );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe1_VD_Int_Handler end\n" ));
#endif
}

/**
B2B Macro Pipe1 HD Int handler.
*/
VOID Im_PRO_B2BTOP_Pipe1_HD_Int_Handler( VOID )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe1_HD_Int_Handler begin\n" ));
#endif
	im_pro_b2b_hd_proc( E_IM_PRO_UNIT_NUM_1 );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe1_HD_Int_Handler end\n" ));
#endif
}

/**
B2B Macro Pipe2 Int handler.
*/
VOID Im_PRO_B2BTOP_Pipe2_Int_Handler( VOID )
{
	T_IM_PRO_B2B_INT_FACTOR int_factor;

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe2_Int_Handler begin\n" ));
#endif
	im_pro_clear_b2b_int_factor( E_IM_PRO_UNIT_NUM_2, &int_factor );
	im_pro_callback_b2b_int( E_IM_PRO_UNIT_NUM_2, &int_factor );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe2_Int_Handler end\n" ));
#endif
}

/**
B2B Macro Pipe2 VD Int handler.
*/
VOID Im_PRO_B2BTOP_Pipe2_VD_Int_Handler( VOID )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe2_VD_Int_Handler begin\n" ));
#endif
	im_pro_b2b_vd_proc( E_IM_PRO_UNIT_NUM_2 );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe2_VD_Int_Handler end\n" ));
#endif
}

/**
B2B Macro Pipe2 HD Int handler.
*/
VOID Im_PRO_B2BTOP_Pipe2_HD_Int_Handler( VOID )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe2_HD_Int_Handler begin\n" ));
#endif
	im_pro_b2b_hd_proc( E_IM_PRO_UNIT_NUM_2 );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_B2BTOP_Pipe2_HD_Int_Handler end\n" ));
#endif
}


/**
STATTOP Interrupt setting
@param[in]	int_ctrl : STATTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_STATTOP_Interrupt_Ctrl( T_IM_PRO_INT_STATTOP_CTRL* int_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (int_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_STATTOP_Interrupt_Ctrl error. int_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );
	IO_PRO.STAT.STATTOP.INTCTL.bit.INTMD	= int_ctrl->int_mode;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );

	return D_DDIM_OK;
}

static VOID im_pro_stat_int_handler_print( UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	for(UCHAR i = 0; i < D_IM_PRO_STAT_PWCH_CH_NUM; i++) {
		Ddim_Print(( "im_pro_stat_int_handler_print() PWCH%u PWCHINTFLG(0x%08x) PWCHINTENB(0x%08x) (status:%u)\n", i, IO_PRO.SEN.PWCH[i].PWCHINTFLG.word, IO_PRO.SEN.PWCH[i].PWCHINTENB.word, status ));
	}
	for(UCHAR i = 0; i < D_IM_PRO_STAT_AEAWB_CH_NUM; i++) {
		Ddim_Print(( "im_pro_stat_int_handler_print() AEAWB%u LINEINTFLG(0x%08x) LINEINTENB(0x%08x) (status:%u)\n", i, IO_PRO.STAT.AEAWB[i].LINEINTFLG.word, IO_PRO.STAT.AEAWB[i].LINEINTENB.word, status ));
	}
	for(UCHAR i = 0; i < D_IM_PRO_STAT_AF_CH_NUM; i++) {
		Ddim_Print(( "im_pro_stat_int_handler_print() AF%u AFINTFLG(0x%08x) AFINTENB(0x%08x) (status:%u)\n", i, IO_PRO.STAT.AF[i].AFGBAL.AFINTFLG.word, IO_PRO.STAT.AF[i].AFGBAL.AFINTENB.word, status ));
	}
	for(UCHAR i = 0; i < D_IM_PRO_STAT_HIST_CH_NUM; i++) {
		Ddim_Print(( "im_pro_stat_int_handler_print() HIST%u HISTINTFLG(0x%08x) HISTINTENB(0x%08x) (status:%u)\n", i, IO_PRO.STAT.HIST[i].HISTINTFLG.word, IO_PRO.STAT.HIST[i].HISTINTENB.word, status ));
	}
#endif
}

/**
STAT Macro Int handler.
*/
VOID Im_PRO_STAT_Int_Handler( VOID )
{
	UINT32	loopcnt = 0;
	UINT32	loopcnt_ch = 0;
	ULONG	aeawb_intf[D_IM_PRO_STAT_AEAWB_CH_NUM] = {0};
	ULONG	af_intf[D_IM_PRO_STAT_AF_CH_NUM] = {0};
	ULONG	hist_intf[D_IM_PRO_STAT_HIST_CH_NUM] = {0};
	ULONG	callback_result = 0;
	ULONG	pwch_intf[D_IM_PRO_STAT_PWCH_CH_NUM] = {0};
	T_IM_PRO_PWCH_INT_INFO	pwch_info = {0};
	T_CALLBACK_ID	cb_id = {
		.unit_no	= 0,
		.block_type	= 0,
		.channel	= 0,
	};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_STAT_Int_Handler begin\n" ));
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );
	Im_Pro_Dsb();
	im_pro_stat_int_handler_print( 0 );

	/***********/
	/* PWCH    */
	/***********/
	// Get & clear interrupt flag
	pwch_info.unit_no		= E_IM_PRO_UNIT_NUM_1;
	pwch_info.block_type	= E_IM_PRO_BLOCK_TYPE_STAT;
	pwch_info.reg_ptr		= IO_PRO.STAT.PWCH;
	im_pro_pwch_clear_int_factor( &pwch_info, pwch_intf, D_IM_PRO_STAT_PWCH_CH_NUM );

	/***********/
	/* AEAWB   */
	/***********/
	// Get & clear interrupt flag
	for( loopcnt = 0; loopcnt < D_IM_PRO_STAT_AEAWB_CH_NUM; loopcnt++ ) {
		aeawb_intf[loopcnt]	= IO_PRO.STAT.AEAWB[loopcnt].LINEINTFLG.word & IO_PRO.STAT.AEAWB[loopcnt].LINEINTENB.word;
		IO_PRO.STAT.AEAWB[loopcnt].LINEINTFLG.word = aeawb_intf[loopcnt];
	}

	/***********/
	/* AF      */
	/***********/
	// Get & clear interrupt flag
	for( loopcnt = 0; loopcnt < D_IM_PRO_STAT_AF_CH_NUM; loopcnt++ ) {
		af_intf[loopcnt]	= IO_PRO.STAT.AF[loopcnt].AFGBAL.AFINTFLG.word & IO_PRO.STAT.AF[loopcnt].AFGBAL.AFINTENB.word;
		IO_PRO.STAT.AF[loopcnt].AFGBAL.AFINTFLG.word = af_intf[loopcnt];

		if( af_intf[loopcnt] & D_IM_PRO_AFINTFLG_AFF ) {
			im_pro_comm_update_af_result( loopcnt );
		}
	}

	/***********/
	/* HIST    */
	/***********/
	// Get & clear interrupt flag
	for( loopcnt = 0; loopcnt < D_IM_PRO_STAT_HIST_CH_NUM; loopcnt++ ) {
		hist_intf[loopcnt]	= IO_PRO.STAT.HIST[loopcnt].HISTINTFLG.word & IO_PRO.STAT.HIST[loopcnt].HISTINTENB.word;
		IO_PRO.STAT.HIST[loopcnt].HISTINTFLG.word = hist_intf[loopcnt];
		if( hist_intf[loopcnt] & D_IM_PRO_HISTINTFLG_HISTF ) {
			im_pro_comm_update_hist_result( loopcnt );
		}
	}

	Im_Pro_Dsb();
	im_pro_stat_int_handler_print( 1 );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );


	/***********/
	/* PWCH    */
	/***********/
	for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_STAT_PWCH_CH_NUM; loopcnt_ch++ ) {
		im_pro_pwch_callback_int_factor( 0, E_IM_PRO_BLOCK_TYPE_STAT, (E_IM_PRO_PWCH)loopcnt_ch, pwch_intf );
	}

	/***********/
	/* AEAWB   */
	/***********/
	for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_STAT_AEAWB_CH_NUM; loopcnt_ch++ ) {
		callback_result = 0;
		for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_stat_aeawb_int) / sizeof(gim_pro_inttbl_stat_aeawb_int[0])); loopcnt++ ) {
			if( (aeawb_intf[loopcnt_ch] & gim_pro_inttbl_stat_aeawb_int[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_stat_aeawb_int[loopcnt].int_fact;
			}
		}
		// Callback
		if (( gIM_PRO_AEAWB_CallBack_Func[loopcnt_ch].p_callback != NULL ) && ( callback_result != 0 )){
			cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
			cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_STAT,
			cb_id.channel		= loopcnt_ch,
			gIM_PRO_AEAWB_CallBack_Func[loopcnt_ch].p_callback( &cb_id, callback_result, gIM_PRO_AEAWB_CallBack_Func[loopcnt_ch].user_param );
		}
	}
	Im_Pro_Dsb();

	/***********/
	/* AF      */
	/***********/
	for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_STAT_AF_CH_NUM; loopcnt_ch++ ) {
		callback_result = 0;
		for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_stat_af_int) / sizeof(gim_pro_inttbl_stat_af_int[0])); loopcnt++ ) {
			if( (af_intf[loopcnt_ch] & gim_pro_inttbl_stat_af_int[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_stat_af_int[loopcnt].int_fact;
			}
		}
		// Callback
		if (( gIM_PRO_AF_CallBack_Func[loopcnt_ch].p_callback != NULL ) && ( callback_result != 0 )){
			cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
			cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_STAT,
			cb_id.channel		= loopcnt_ch,
			gIM_PRO_AF_CallBack_Func[loopcnt_ch].p_callback( &cb_id, callback_result, gIM_PRO_AF_CallBack_Func[loopcnt_ch].user_param );
		}
	}
	Im_Pro_Dsb();

	/***********/
	/* HIST    */
	/***********/
	for( loopcnt_ch = 0; loopcnt_ch < D_IM_PRO_STAT_AF_CH_NUM; loopcnt_ch++ ) {
		callback_result = 0;
		for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_stat_hist_int) / sizeof(gim_pro_inttbl_stat_hist_int[0])); loopcnt++ ) {
			if( (hist_intf[loopcnt_ch] & gim_pro_inttbl_stat_hist_int[loopcnt].flg_bitmask) != 0 ) {
				callback_result	|= gim_pro_inttbl_stat_hist_int[loopcnt].int_fact;
			}
		}
		// Callback
		if (( gIM_PRO_HIST_CallBack_Func[loopcnt_ch].p_callback != NULL ) && ( callback_result != 0 )){
			cb_id.unit_no		= E_IM_PRO_UNIT_NUM_1,
			cb_id.block_type	= E_IM_PRO_BLOCK_TYPE_STAT,
			cb_id.channel		= loopcnt_ch,
			gIM_PRO_HIST_CallBack_Func[loopcnt_ch].p_callback( &cb_id, callback_result, gIM_PRO_HIST_CallBack_Func[loopcnt_ch].user_param );
		}
	}
	Im_Pro_Dsb();
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_STAT_Int_Handler end\n" ));
#endif
}

/**
AEAWB Macro interrupt permission set
@param[in]	ch : channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_Interrupt( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	union io_lineintenb lineintenb;
	union io_lineintflg lineintflg;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_OBD_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_OBD_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );
	lineintflg.word	= 0;
	lineintenb.word	= IO_PRO.STAT.AEAWB[ch].LINEINTENB.word;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_LINEINTENB_LINEE)){
		lineintflg.bit.__LINEF	= D_IM_PRO_INTFLG_CLEAR;
		lineintenb.bit.LINEE	= int_cfg->permission_flg;
	}

	IO_PRO.STAT.AEAWB[ch].LINEINTFLG.word	= lineintflg.word;
	IO_PRO.STAT.AEAWB[ch].LINEINTENB.word	= lineintenb.word;
	Im_Pro_Dsb();
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of AEAWB is registered
@param[in]	ch : channel
@param[in]	inthandler : AEAWB Interrupt callback function address
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_AEAWB_Set_Int_Handler( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_AEAWB_CallBack_Func[ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_AEAWB_CallBack_Func[ch].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
AF Macro interrupt permission set
@param[in]	ch : channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AF_Set_Interrupt( E_IM_PRO_AF_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	union io_afintenb afintenb;
	union io_afintflg afintflg;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_AF_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_AF_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );
	afintenb.word	= IO_PRO.STAT.AF[ch].AFGBAL.AFINTENB.word;
	afintflg.word	= 0;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_AFINTENB_AFE)){
		afintflg.bit.__AFF	= D_IM_PRO_INTFLG_CLEAR;
		afintenb.bit.AFE	= int_cfg->permission_flg;
	}

	IO_PRO.STAT.AF[ch].AFGBAL.AFINTFLG.word	= afintflg.word;
	IO_PRO.STAT.AF[ch].AFGBAL.AFINTENB.word	= afintenb.word;
	Im_Pro_Dsb();
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of AF is registered
@param[in]	ch : channel
@param[in]	inthandler : AF Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AF_Set_Int_Handler( E_IM_PRO_AF_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_AF_CallBack_Func[ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_AF_CallBack_Func[ch].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
HIST Macro interrupt permission set
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HIST_Set_Interrupt( E_IM_PRO_HIST_CH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	union io_histintenb histintenb;
	union io_histintflg histintflg;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_HIST_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_HIST_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );
	histintenb.word	= IO_PRO.STAT.HIST[ch].HISTINTENB.word;
	histintflg.word	= 0;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_HISTINTENB_HISTE)){
		histintflg.bit.__HISTF	= D_IM_PRO_INTFLG_CLEAR;
		histintenb.bit.HISTE	= int_cfg->permission_flg;
	}

	IO_PRO.STAT.HIST[ch].HISTINTFLG.word	= histintflg.word;
	IO_PRO.STAT.HIST[ch].HISTINTENB.word	= histintenb.word;
	Im_Pro_Dsb();
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of HIST is registered
@param[in]	ch : channel
@param[in]	inthandler : HIST Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HIST_Set_Int_Handler( E_IM_PRO_HIST_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_HIST_CallBack_Func[ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_HIST_CallBack_Func[ch].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
PASTOP Interrupt setting
@param[in]	int_ctrl : PASTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
INT32 Im_PRO_PASTOP_Interrupt_Ctrl( T_IM_PRO_INT_PASTOP_CTRL* int_ctrl )
{
	union io_pas_vhdintenb	vhdintenb;
	union io_pas_vhdintflg	vhdintflg;

#ifdef CO_PARAM_CHECK
	if (int_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_PASTOP_Interrupt_Ctrl error. int_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (( int_ctrl->vd_enable[0].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->vd_enable[1].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->hd_enable[0].permission_flg > D_IM_PRO_FUNC_EN ) ||
		( int_ctrl->hd_enable[1].permission_flg > D_IM_PRO_FUNC_EN )
		) {
		Ddim_Assertion(("I:Im_PRO_PASTOP_Interrupt_Ctrl error. 'permission_flg' value over!! (VD0:%d VD1:%d HD0:%d HD1:%d)\n"	, int_ctrl->vd_enable[0].permission_flg
																																, int_ctrl->vd_enable[1].permission_flg
																																, int_ctrl->hd_enable[0].permission_flg
																																, int_ctrl->hd_enable[1].permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );
	vhdintenb.word	= IO_PRO.PAS.PASTOP.VHDINTENB.word;
	vhdintflg.word	= 0;

	// VHDINTENB VD Interrupt
	if (0 != (int_ctrl->vd_enable[0].interrupt_bit & D_IM_PRO_PASTOP_INT_VDE0)){
		vhdintflg.bit.__VDF0	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.VDE0		= int_ctrl->vd_enable[0].permission_flg;
	}
	if (0 != (int_ctrl->vd_enable[1].interrupt_bit & D_IM_PRO_PASTOP_INT_VDE1)){
		vhdintflg.bit.__VDF1	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.VDE1		= int_ctrl->vd_enable[1].permission_flg;
	}
	// VHDINTENB HD Interrupt
	if (0 != (int_ctrl->hd_enable[0].interrupt_bit & D_IM_PRO_PASTOP_INT_HDE0)){
		vhdintflg.bit.__HDF0	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.HDE0		= int_ctrl->hd_enable[0].permission_flg;
	}
	if (0 != (int_ctrl->hd_enable[1].interrupt_bit & D_IM_PRO_PASTOP_INT_HDE1)){
		vhdintflg.bit.__HDF1	= D_IM_PRO_INTFLG_CLEAR;
		vhdintenb.bit.HDE1		= int_ctrl->hd_enable[1].permission_flg;
	}

	IO_PRO.PAS.PASTOP.INTCTL.bit.INTMD	= int_ctrl->int_mode;
	IO_PRO.PAS.PASTOP.VHDINTFLG.word	= vhdintflg.word;
	IO_PRO.PAS.PASTOP.VHDINTENB.word	= vhdintenb.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );

	return D_DDIM_OK;
}

/**
PASTOP macro interrupt delay setting
@param[in]	ch : channel(0:HD/VD0, 1:HD/VD1)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PASTOP_Set_VHD_Delay( UCHAR ch, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_PRO_PASTOP_VHD_CH_NUM){
		Ddim_Assertion(("I:Im_PRO_PASTOP_Set_VHD_Delay. ch value error!! (ch:%d)\n", ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_PASTOP_Set_VHD_Delay error. delay_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->hd_delay > D_IM_PRO_HMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_PASTOP_Set_VHD_Delay hd_delay value error. delay_ctrl->hd_delay=%u\n",delay_ctrl->hd_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_h_delay > D_IM_PRO_VMIHADR_MAX){
		Ddim_Assertion(("I:Im_PRO_PASTOP_Set_VHD_Delay vd_h_delay value error. delay_ctrl->vd_h_delay=%u\n",delay_ctrl->vd_h_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (delay_ctrl->vd_v_delay > D_IM_PRO_VMIVADR_MAX){
		Ddim_Assertion(("I:Im_PRO_PASTOP_Set_VHD_Delay vd_v_delay value error. delay_ctrl->vd_v_delay=%u\n",delay_ctrl->vd_v_delay));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );
	if( ch == 0 ) {
		IO_PRO.PAS.PASTOP.HMIHADR.bit.HMIHADR0	= delay_ctrl->hd_delay;
		IO_PRO.PAS.PASTOP.VMIHADR.bit.VMIHADR0	= delay_ctrl->vd_h_delay;
		IO_PRO.PAS.PASTOP.VMIVADR.bit.VMIVADR0	= delay_ctrl->vd_v_delay;
		IO_PRO.PAS.PASTOP.VHSET.bit.VHSET0		= D_IM_PRO_FUNC_EN;
	}
	else {
		IO_PRO.PAS.PASTOP.HMIHADR.bit.HMIHADR1	= delay_ctrl->hd_delay;
		IO_PRO.PAS.PASTOP.VMIHADR.bit.VMIHADR1	= delay_ctrl->vd_h_delay;
		IO_PRO.PAS.PASTOP.VMIVADR.bit.VMIVADR1	= delay_ctrl->vd_v_delay;
		IO_PRO.PAS.PASTOP.VHSET.bit.VHSET1		= D_IM_PRO_FUNC_EN;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );

	return D_DDIM_OK;
}

/**
PAS block VD Int handler setting.
@param[in]	inthandler : Interrupt handler address pointer 
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PASTOP_Set_VD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_PAS_VD_CallBack_Func.p_callback = callback_cfg->inthandler;
	gIM_PRO_PAS_VD_CallBack_Func.user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
PAS block HD Int handler setting.
@param[in]	inthandler : Interrupt handler address pointer 
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PASTOP_Set_HD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_PAS_HD_CallBack_Func.p_callback = callback_cfg->inthandler;
	gIM_PRO_PAS_HD_CallBack_Func.user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

static VOID im_pro_pastop_int_handler_print( UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_pastop_int_handler_print() PWCH PWCHINTFLG(0x%08x) PWCHINTENB(0x%08x) (status:%u)\n", IO_PRO.PAS.PWCH0.PWCHINTFLG.word, IO_PRO.PAS.PWCH0.PWCHINTENB.word, status ));
	Ddim_Print(( "im_pro_pastop_int_handler_print() PWCH PWCHINTFLG(0x%08x) PWCHINTENB(0x%08x) (status:%u)\n", IO_PRO.PAS.PRCH0.PRCHINTFLG.word, IO_PRO.PAS.PRCH0.PRCHINTENB.word, status ));
#endif
}

/**
PAS Macro Int handler.
*/
VOID Im_PRO_PASTOP_Int_Handler( VOID )
{
	ULONG			pwch_intf = 0;
	ULONG			prch_intf = 0;
	T_IM_PRO_PWCH_INT_INFO pwch_info = {0};
	T_IM_PRO_PRCH_INT_INFO prch_info = {0};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_PASTOP_Int_Handler begin\n" ));
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );
	Im_Pro_Dsb();
	im_pro_pastop_int_handler_print( 0 );

	/***********/
	/* PWCH    */
	/***********/
	// Get & clear interrupt flag
	pwch_info.unit_no		= E_IM_PRO_UNIT_NUM_1;
	pwch_info.block_type	= E_IM_PRO_BLOCK_TYPE_PAS;
	pwch_info.reg_ptr		= &IO_PRO.PAS.PWCH0;
	im_pro_pwch_clear_int_factor( &pwch_info, &pwch_intf, 1 );

	/***********/
	/* PRCH    */
	/***********/
	// Get & clear interrupt flag
	prch_info.unit_no		= E_IM_PRO_UNIT_NUM_1;
	prch_info.block_type	= E_IM_PRO_BLOCK_TYPE_PAS;
	prch_info.reg_ptr		= &IO_PRO.PAS.PRCH0;
	im_pro_prch_clear_int_factor( &prch_info, &prch_intf, 1 );

	Im_Pro_Dsb();
	im_pro_pastop_int_handler_print( 1 );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );

	/*******************/
	/* PWCH Callback   */
	/*******************/
	im_pro_pwch_callback_int_factor( 0, E_IM_PRO_BLOCK_TYPE_PAS, E_IM_PRO_PWCH_0, &pwch_intf );

	/*******************/
	/* PRCH Callback   */
	/*******************/
	im_pro_prch_callback_int_factor( 0, E_IM_PRO_BLOCK_TYPE_PAS, E_IM_PRO_PRCH_0, &prch_intf );
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_PASTOP_Int_Handler end\n" ));
#endif
}

static VOID im_pro_pastop_vd_int_handler_print( UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_sen_vd_int_handler_print() VHDINTFLG(0x%08x) VHDINTENB(0x%08x) (status:%u)\n", IO_PRO.PAS.PASTOP.VHDINTFLG.word, IO_PRO.PAS.PASTOP.VHDINTENB.word, status ));
#endif
}

/**
PAS Macro VD Int handler.
*/
VOID Im_PRO_PASTOP_VD_Int_Handler( VOID )
{
	UINT32			loopcnt;
	ULONG			vd_intf;
	ULONG callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= E_IM_PRO_UNIT_NUM_1,
		.block_type	= E_IM_PRO_BLOCK_TYPE_PAS,
		.channel	= 0,
	};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_PASTOP_VD_Int_Handler begin\n" ));
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );
	Im_Pro_Dsb();
	im_pro_pastop_vd_int_handler_print( 0 );

	// Get interrupt flag
	vd_intf	= IO_PRO.PAS.PASTOP.VHDINTFLG.word & IO_PRO.PAS.PASTOP.VHDINTENB.word & D_IM_PRO_PASTOP_INT_VD_ALL;

	// Clear interrupt flag
	IO_PRO.PAS.PASTOP.VHDINTFLG.word	= vd_intf;

	Im_Pro_Dsb();
	im_pro_pastop_vd_int_handler_print( 1 );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );

	/***********/
	/* VD Edge */
	/***********/
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_pas_vd) / sizeof(gim_pro_inttbl_pas_vd[0])); loopcnt++ ) {
		if( (vd_intf & gim_pro_inttbl_pas_vd[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_pas_vd[loopcnt].int_fact;
		}
	}
	// Callback
	if (( gIM_PRO_PAS_VD_CallBack_Func.p_callback != NULL ) && ( callback_result != 0 )){
		gIM_PRO_PAS_VD_CallBack_Func.p_callback( &cb_id, callback_result, gIM_PRO_PAS_VD_CallBack_Func.user_param );
	}
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_PASTOP_VD_Int_Handler end\n" ));
#endif
}

static VOID im_pro_pastop_hd_int_handler_print( UCHAR status )
{
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "im_pro_pastop_hd_int_handler_print() VHDINTFLG(0x%08x) VHDINTENB(0x%08x) (status:%u)\n", IO_PRO.PAS.PASTOP.VHDINTFLG.word, IO_PRO.PAS.PASTOP.VHDINTENB.word, status ));
#endif
}

/**
PAS Macro HD Int handler.
*/
VOID Im_PRO_PASTOP_HD_Int_Handler( VOID )
{
	UINT32			loopcnt;
	ULONG			hd_intf;
	ULONG			callback_result = 0;
	T_CALLBACK_ID	cb_id = {
		.unit_no	= E_IM_PRO_UNIT_NUM_1,
		.block_type	= E_IM_PRO_BLOCK_TYPE_PAS,
		.channel	= 0,
	};

#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_PASTOP_HD_Int_Handler begin\n" ));
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );
	Im_Pro_Dsb();
	im_pro_pastop_hd_int_handler_print( 0 );

	// Get interrupt flag
	hd_intf		= IO_PRO.PAS.PASTOP.VHDINTFLG.word & IO_PRO.PAS.PASTOP.VHDINTENB.word & D_IM_PRO_PASTOP_INT_HD_ALL;

	// Clear interrupt flag
	IO_PRO.PAS.PASTOP.VHDINTFLG.word	= hd_intf;

	Im_Pro_Dsb();
	im_pro_pastop_hd_int_handler_print( 1 );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );

	/***********/
	/* HD Edge */
	/***********/
	for( loopcnt = 0; loopcnt < (sizeof(gim_pro_inttbl_pas_hd) / sizeof(gim_pro_inttbl_pas_hd[0])); loopcnt++ ) {
		if( (hd_intf & gim_pro_inttbl_pas_hd[loopcnt].flg_bitmask) != 0 ) {
			callback_result	|= gim_pro_inttbl_pas_hd[loopcnt].int_fact;

		}
	}
	// Callback
	if (( gIM_PRO_PAS_HD_CallBack_Func.p_callback != NULL ) && ( callback_result != 0 )){
		gIM_PRO_PAS_HD_CallBack_Func.p_callback( &cb_id, callback_result, gIM_PRO_PAS_HD_CallBack_Func.user_param );
	}
#ifdef CO_DEBUG_PRINT_IM_PRO
	Ddim_Print(( "Im_PRO_PASTOP_HD_Int_Handler end\n" ));
#endif
}

/**
PWch Macro interrupt permission set
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch	 : PWch channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PWch_Set_Interrupt( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	union io_pwchintenb pwchintenb;
	union io_pwchintflg pwchintflg;
	const T_IM_PRO_COMMON_PWCH_INFO* pwch_reg_info = 0;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_PWch_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_PWch_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_comm_get_pwch_reg_info( unit_no, block_type, ch, &pwch_reg_info );
#ifdef CO_PARAM_CHECK
	if (pwch_reg_info->reg_ptr == NULL){
		Ddim_Assertion(("I:Im_PRO_PWch_Set_Interrupt error. pwch_reg_info->reg_ptr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, im_pro_convert_clk_type( block_type ) );

	pwchintenb.word	= pwch_reg_info->reg_ptr->PWCHINTENB.word;
	pwchintflg.word	= 0;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_PWCHINTENB_PWE)){
		pwchintflg.bit.__PWF	= D_IM_PRO_INTFLG_CLEAR;
		pwchintenb.bit.PWE		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_PWCHINTENB_PWEE)){
		pwchintflg.bit.__PWEF	= D_IM_PRO_INTFLG_CLEAR;
		pwchintenb.bit.PWEE		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_PWCHINTENB_PWXE)){
		pwchintflg.bit.__PWXF	= D_IM_PRO_INTFLG_CLEAR;
		pwchintenb.bit.PWXE		= int_cfg->permission_flg;
	}

	pwch_reg_info->reg_ptr->PWCHINTFLG.word	= pwchintflg.word;
	pwch_reg_info->reg_ptr->PWCHINTENB.word	= pwchintenb.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( unit_no, im_pro_convert_clk_type( block_type ) );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of PWch is registered
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@param[in]	inthandler	: PWch Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PWch_Set_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_PWch_CallBack_Func[unit_no][block_type][ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_PWch_CallBack_Func[unit_no][block_type][ch].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

/**
The permission setting of the PRch interrupt control is done.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PRch_Set_Interrupt( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, T_IM_PRO_INT_CFG* int_cfg )
{
	union io_prchintenb prchintenb;
	union io_prchintflg prchintflg;
	const T_IM_PRO_COMMON_PRCH_INFO* prch_reg_info = 0;

#ifdef CO_PARAM_CHECK
	if (int_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_PWch_Set_Interrupt error. int_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (int_cfg->permission_flg > D_IM_PRO_FUNC_EN){
		Ddim_Assertion(("I:Im_PRO_PWch_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", int_cfg->permission_flg));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_comm_get_prch_reg_info( unit_no, block_type, ch, &prch_reg_info );
#ifdef CO_PARAM_CHECK
	if (prch_reg_info->reg_ptr == NULL){
		Ddim_Assertion(("I:Im_PRO_PRch_Set_Interrupt error. prch_reg_info->reg_ptr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, im_pro_convert_clk_type( block_type ) );

	prchintenb.word	= prch_reg_info->reg_ptr->PRCHINTENB.word;
	prchintflg.word	= 0;

	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_PRCHINTENB_PRE)){
		prchintflg.bit.__PRF	= D_IM_PRO_INTFLG_CLEAR;
		prchintenb.bit.PRE		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_PRCHINTENB_PREE)){
		prchintflg.bit.__PREF	= D_IM_PRO_INTFLG_CLEAR;
		prchintenb.bit.PREE		= int_cfg->permission_flg;
	}
	if (0 != (int_cfg->interrupt_bit & D_IM_PRO_PRCHINTENB_PRXE)){
		prchintflg.bit.__PRXF	= D_IM_PRO_INTFLG_CLEAR;
		prchintenb.bit.PRXE		= int_cfg->permission_flg;
	}

	prch_reg_info->reg_ptr->PRCHINTFLG.word	= prchintflg.word;
	prch_reg_info->reg_ptr->PRCHINTENB.word	= prchintenb.word;
	Im_Pro_Dsb();

	im_pro_off_pclk( unit_no, im_pro_convert_clk_type( block_type ) );

	return D_DDIM_OK;
}

/**
CallBackFunction which Call to the interruption timing of PRch is registered
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	inthandler : PRch Interrupt callback function address
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PRch_Set_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg )
{
	gIM_PRO_PRch_CallBack_Func[unit_no][block_type][ch].p_callback = callback_cfg->inthandler;
	gIM_PRO_PRch_CallBack_Func[unit_no][block_type][ch].user_param = callback_cfg->user_param;
	return D_DDIM_OK;
}

