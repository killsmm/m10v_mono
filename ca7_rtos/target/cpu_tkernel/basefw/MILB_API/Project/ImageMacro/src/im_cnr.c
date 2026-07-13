/**
 * @file		im_cnr.c
 * @brief		JDSCNR1A Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "im_cnr.h"
#include "driver_common.h"
// #if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
// #endif
#include "dd_arm.h"
#include "ddim_user_custom.h"

#include "jdscnr.h"

// for memset
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// #define CO_CNR_DEBUG
// #define CO_CNR_REG_TYPE_CHECK

// Low power consumption option
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_IMAGE_CLOCK)
#	define CO_ACT_CNRCLOCK
#endif
#if defined(CO_ACT_ICLOCK) || defined(CO_ACT_IMAGE_ICLOCK)
#	define CO_ACT_CNR_ICLK
#endif
#if defined(CO_ACT_PCLOCK) || defined(CO_ACT_IMAGE_PCLOCK)
#	define CO_ACT_CNR_PCLK
#endif

#define D_IM_CNR_OFF_LINE				(0)				// Off Line Mode
#define D_IM_CNR_ON_THE_FLY				(1)				// On The Fly mode

#define D_IM_CNR_SYNC					(0)				// Sync Processing
#define D_IM_CNR_ASYNC					(1)				// Async Processing
#define D_IM_CNR_SRAM_WAIT_USEC			(1)

#define D_IM_CNR_OFL_0_WAIT_FLGPTN		(D_IM_CNR_FLG_0_OFL_SPR_END | D_IM_CNR_FLG_0_OFL_SPR_AXI_ERR)
#define D_IM_CNR_OFL_1_WAIT_FLGPTN		(D_IM_CNR_FLG_1_OFL_SPR_END | D_IM_CNR_FLG_1_OFL_SPR_AXI_ERR)
#define D_IM_CNR_OTF_0_WAIT_FLGPTN		(D_IM_CNR_FLG_0_OTF_SPR_END | D_IM_CNR_FLG_0_OTF_SPR_AXI_ERR | D_IM_CNR_FLG_0_OTF_SPR_DRT_ERR)
#define D_IM_CNR_OTF_1_WAIT_FLGPTN		(D_IM_CNR_FLG_1_OTF_SPR_END | D_IM_CNR_FLG_1_OTF_SPR_AXI_ERR | D_IM_CNR_FLG_1_OTF_SPR_DRT_ERR)


#define Im_CNR_Dsb()	Dd_ARM_Dsb_Pou()

#define	IO_CNR(ch)		((D_IM_CNR_CH_0 == ((ch))) ? &IO_CNR1 : (D_IM_CNR_CH_1 == ((ch))) ? &IO_CNR2 : &IO_CNR3)

#define	Im_CNR_OFL_Enable(ch)			IO_CNR(ch)->OFL_REG_RW.SPRTRG.bit.SPRTRG = 1;
#define	Im_CNR_OFL_Disable(ch)			IO_CNR(ch)->OFL_REG_RW.SPRTRG.bit.SPRTRG = 0;
#define	Im_CNR_OFL_Softreset(ch)		IO_CNR(ch)->OFL_REG_RW.SPRSR.bit.SR = 1;
#define	Im_CNR_OTF_Enable(ch)			IO_CNR(ch)->OTF_REG_RW.SPRTRG.bit.SPRTRG = 1;
#define	Im_CNR_OTF_Disable(ch)			IO_CNR(ch)->OTF_REG_RW.SPRTRG.bit.SPRTRG = 0;
#define	Im_CNR_OTF_Softreset(ch)		IO_CNR(ch)->OTF_REG_RW.SPRSR.bit.SR = 1;

#define im_cnr_wait_usec( usec )		Dd_ARM_Wait_ns( (usec * 1000) )

#ifdef CO_CNR_REG_TYPE_CHECK
#define im_cnr_check_reg_type( dst, src )	(dst) = (src);
#else
#define im_cnr_check_reg_type( dst, src )
#endif

#define im_cnr_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_cnr_check_reg_type( work, (reg) ); \
		}



#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Nothing Special */

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Output address management information
typedef struct {
	UCHAR	use_bank_num;							// Output bank number
	UCHAR	bank_area;								// Target bank number
	ULONG	output_addr[D_IM_CNR_ADDR_BANK_MAX];	// Output bank address information.
} T_IM_CNR_OUTPUT_MNG;

// Output latest management information
typedef struct {
	UCHAR	bank_area;								// Latest bank number
	ULONG	addr;									// Latest Output address.
} T_IM_CNR_LATEST_MNG;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define ADDR_BNK_LIMIT(pos, total) ((pos < total) ? pos : 0)

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

static volatile T_IM_CNR_CALLBACK	gIM_CNR_OFL_CallBack_Func[D_IM_CNR_CH_READ_NUM_MAX] = {NULL, NULL};
static volatile T_IM_CNR_CALLBACK	gIM_CNR_OTF_CallBack_Func[D_IM_CNR_CH_READ_NUM_MAX] = {NULL, NULL};
static volatile UINT32 gIM_CNR_OFL_UserParam[D_IM_CNR_CH_READ_NUM_MAX] = {0, 0};
static volatile UINT32 gIM_CNR_OTF_UserParam[D_IM_CNR_CH_READ_NUM_MAX] = {0, 0};
#ifdef CO_ACT_CNRCLOCK
static volatile UCHAR gIM_CNR_Clk_Ctrl_Cnt1 = 0;
static volatile UCHAR gIM_CNR_Clk_Ctrl_Cnt2 = 0;
#endif //CO_ACT_CNRCLOCK
#ifdef CO_ACT_CNR_ICLK
static volatile UCHAR gIM_CNR_Iclk_Ctrl_Cnt1 = 0;
static volatile UCHAR gIM_CNR_Iclk_Ctrl_Cnt2 = 0;
#endif //CO_ACT_CNR_ICLK
#ifdef CO_ACT_CNR_PCLK
static volatile UCHAR gIM_CNR_Pclk_Ctrl_Cnt1 = 0;
static volatile UCHAR gIM_CNR_Pclk_Ctrl_Cnt2 = 0;
#endif //CO_ACT_CNR_PCLK

// Output address parameter
static volatile T_IM_CNR_OUTPUT_MNG gIM_CNR_Output_Mng[D_IM_CNR_CH_READ_NUM_MAX];
// Output address parameter
static volatile T_IM_CNR_LATEST_MNG gIM_CNR_Latest_Mng[D_IM_CNR_CH_READ_NUM_MAX];

static UCHAR gim_cnr_mode[2] = { D_IM_CNR_MODE_OFL, D_IM_CNR_MODE_OFL };

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// SPR setting for CbCr address (OFL)
const static T_IM_CNR_RDMA_ADDR_CTRL_C gIM_CNR_OFL_Ctrl_C[D_IM_CNR_CH_NUM_MAX] = {
	// CSPREN		CGDKEN		MCEN		MCYTHH		MCYTHV
	// MCCTHH		MCCTHV		MCYDYM		MCYDYMDT	MCYSCL
	// MCCSCL		MCSSCL		LCEN		LCYTHH		LCYTHV
	// LCCTHH		LCCTHV		LCYDYM		LCYDYMDT	LCYSCL
	// LCCSCL		LCSSCL		BLEND		WHTMD

	// Pipeline-1
	{0x28423000,	0x28423008,	0x28423020,	0x28423024,	0x28423028,
	 0x2842302C,	0x28423030,	0x28423034,	0x28423038,	0x28423040,
	 0x28423060,	0x28423080,	0x284230C0,	0x284230C4,	0x284230C8,
	 0x284230CC,	0x284230D0,	0x284230D4,	0x284230D8,	0x284230E0,
	 0x28423100,	0x28423120,	0x28423160,	0x284231D0},
	// Pipeline-2
	{0x28523000,	0x28523008,	0x28523020,	0x28523024,	0x28523028,
	 0x2852302C,	0x28523030,	0x28523034,	0x28523038,	0x28523040,
	 0x28523060,	0x28523080,	0x285230C0,	0x285230C4,	0x285230C8,
	 0x285230CC,	0x285230D0,	0x285230D4,	0x285230D8,	0x285230E0,
	 0x28523100,	0x28523120,	0x28523160,	0x285231D0},
	// Pipeline-3
	{0x28623000,	0x28623008,	0x28623020,	0x28623024,	0x28623028,
	 0x2862302C,	0x28623030,	0x28623034,	0x28623038,	0x28623040,
	 0x28623060,	0x28623080,	0x286230C0,	0x286230C4,	0x286230C8,
	 0x286230CC,	0x286230D0,	0x286230D4,	0x286230D8,	0x286230E0,
	 0x28623100,	0x28623120,	0x28623160,	0x286231D0},
};

// SPR setting for CbCr address (OTF)
const static T_IM_CNR_RDMA_ADDR_CTRL_C gIM_CNR_OTF_Ctrl_C[D_IM_CNR_CH_NUM_MAX] = {
	// CSPREN		CGDKEN		MCEN		MCYTHH		MCYTHV
	// MCCTHH		MCCTHV		MCYDYM		MCYDYMDT	MCYSCL
	// MCCSCL		MCSSCL		LCEN		LCYTHH		LCYTHV
	// LCCTHH		LCCTHV		LCYDYM		LCYDYMDT	LCYSCL
	// LCCSCL		LCSSCL		BLEND		WHTMD

	// Pipeline-1
	{0x28421000,	0x28421008,	0x28421020,	0x28421024,	0x28421028,
	 0x2842102C,	0x28421030,	0x28421034,	0x28421038,	0x28421040,
	 0x28421060,	0x28421080,	0x284210C0,	0x284210C4,	0x284210C8,
	 0x284210CC,	0x284210D0,	0x284210D4,	0x284210D8,	0x284210E0,
	 0x28421100,	0x28421120,	0x28421160,	0x284211D0},
	// Pipeline-2
	{0x28521000,	0x28521008,	0x28521020,	0x28521024,	0x28521028,
	 0x2852102C,	0x28521030,	0x28521034,	0x28521038,	0x28521040,
	 0x28521060,	0x28521080,	0x285210C0,	0x285210C4,	0x285210C8,
	 0x285210CC,	0x285210D0,	0x285210D4,	0x285210D8,	0x285210E0,
	 0x28521100,	0x28521120,	0x28521160,	0x285211D0},
	// Pipeline-3
	{0x28621000,	0x28621008,	0x28621020,	0x28621024,	0x28621028,
	 0x2862102C,	0x28621030,	0x28621034,	0x28621038,	0x28621040,
	 0x28621060,	0x28621080,	0x286210C0,	0x286210C4,	0x286210C8,
	 0x286210CC,	0x286210D0,	0x286210D4,	0x286210D8,	0x286210E0,
	 0x28621100,	0x28621120,	0x28621160,	0x286211D0},
};

// SPR setting for Y address (OFL)
const static T_IM_CNR_RDMA_ADDR_OFL_CTRL_Y gIM_CNR_OFL_Ctrl_Y[D_IM_CNR_CH_NUM_MAX] = {
	// YSPREN			YSPRMODE		YSPRFXEY		YSPRFXEC		YSPRALPBD
	{0x28424000,		0x28424004,		0x28424020,		0x28424024,		0x28424028},	// Pipeline-1
	{0x28524000,		0x28524004,		0x28524020,		0x28524024,		0x28524028},	// Pipeline-2
	{0x28624000,		0x28624004,		0x28624020,		0x28624024,		0x28624028},	// Pipeline-3
};

// YSPR Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_OFL_YSPR_TBL gIM_CNR_OFL_YSPR_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// YSPRDYEYOF_1		YSPRDYEYOF_2	YSPRDYEYGA_1	YSPRDYEYGA_2	YSPRDYEYBD_1	YSPRDYEYBD_2
	{0x28424030,		0x28424034,		0x28424038,		0x2842403C,		0x28424040,		0x28424044},	// Pipeline-1
	{0x28524030,		0x28524034,		0x28524038,		0x2852403C,		0x28524040,		0x28524044},	// Pipeline-2
	{0x28624030,		0x28624034,		0x28624038,		0x2862403C,		0x28624040,		0x28624044},	// Pipeline-3
};

// CSPR Mid-Y Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL gIM_CNR_OFL_CSPR_MID_Y_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// MCYSCLOF_1		MCYSCLOF_2		MCYSCLGA_1		MCYSCLGA_2		MCYSCLBD_1,		MCYSCLBD_2
	{0x28423048,		0x2842304C,		0x28423050,		0x28423054,		0x28423058,		0x2842305C},	// Pipeline-1
	{0x28523048,		0x2852304C,		0x28523050,		0x28523054,		0x28523058,		0x2852305C},	// Pipeline-2
	{0x28623048,		0x2862304C,		0x28623050,		0x28623054,		0x28623058,		0x2862305C},	// Pipeline-3
};

// CSPR Mid-Y Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL gIM_CNR_OTF_CSPR_MID_Y_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// MCYSCLOF_1		MCYSCLOF_2		MCYSCLGA_1		MCYSCLGA_2		MCYSCLBD_1,		MCYSCLBD_2
	{0x28421048,		0x2842104C,		0x28421050,		0x28421054,		0x28421058,		0x2842105C},	// Pipeline-1
	{0x28521048,		0x2852104C,		0x28521050,		0x28521054,		0x28521058,		0x2852105C},	// Pipeline-2
	{0x28621048,		0x2862104C,		0x28621050,		0x28621054,		0x28621058,		0x2862105C},	// Pipeline-3
};

// CSPR Mid-CC-Y Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL gIM_CNR_OFL_CSPR_MID_CC_Y_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// MCCSCLOF_1		MCCSCLOF_2		MCCSCLGA_1		MCCSCLGA_2		MCCSCLBD_1,		MCCSCLBD_2
	{0x28423068,		0x2842306C,		0x28423070,		0x28423074,		0x28423078,		0x2842307C},	// Pipeline-1
	{0x28523068,		0x2852306C,		0x28523070,		0x28523074,		0x28523078,		0x2852307C},	// Pipeline-2
	{0x28623068,		0x2862306C,		0x28623070,		0x28623074,		0x28623078,		0x2862307C},	// Pipeline-3
};

// CSPR Mid-CC-Y Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL gIM_CNR_OTF_CSPR_MID_CC_Y_TBL[D_IM_CNR_CH_NUM_MAX] = {
	{0x28421068,		0x2842106C,		0x28421070,		0x28421074,		0x28421078,		0x2842107C},	// Pipeline-1
	{0x28521068,		0x2852106C,		0x28521070,		0x28521074,		0x28521078,		0x2852107C},	// Pipeline-2
	{0x28621068,		0x2862106C,		0x28621070,		0x28621074,		0x28621078,		0x2862107C},	// Pipeline-3
};

// CSPR Mid-CC-C Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL gIM_CNR_OFL_CSPR_MID_CC_C_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// MCSSCLOF_1		MCSSCLOF_2		MCSSCLGA_1		MCSSCLGA_2		MCSSCLBD_1,		MCSSCLBD_1
	{0x28423088,		0x2842308C,		0x28423090,		0x28423094,		0x28423098,		0x2842309C},	// Pipeline-1
	{0x28523088,		0x2852308C,		0x28523090,		0x28523094,		0x28523098,		0x2852309C},	// Pipeline-2
	{0x28623088,		0x2862308C,		0x28623090,		0x28623094,		0x28623098,		0x2862309C},	// Pipeline-3
};

// CSPR Mid-CC-C Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL gIM_CNR_OTF_CSPR_MID_CC_C_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// MCSSCLOF_1		MCSSCLOF_2		MCSSCLGA_1		MCSSCLGA_2		MCSSCLBD_1,		MCSSCLBD_1
	{0x28421088,		0x2842108C,		0x28421090,		0x28421094,		0x28421098,		0x2842109C},	// Pipeline-1
	{0x28521088,		0x2852108C,		0x28521090,		0x28521094,		0x28521098,		0x2852109C},	// Pipeline-2
	{0x28621088,		0x2862108C,		0x28621090,		0x28621094,		0x28621098,		0x2862109C},	// Pipeline-3
};

// CSPR Low-Y Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL gIM_CNR_OFL_CSPR_LOW_Y_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// LCYSCLOF_1		LCYSCLOF_2		LCYSCLGA_1		LCYSCLGA_2		LCYSCLBD_1,		LCYSCLBD_2
	{0x284230E8,		0x284230EC,		0x284230F0,		0x284230F4,		0x284230F8,		0x284230FC},	// Pipeline-1
	{0x285230E8,		0x285230EC,		0x285230F0,		0x285230F4,		0x285230F8,		0x285230FC},	// Pipeline-2
	{0x286230E8,		0x286230EC,		0x286230F0,		0x286230F4,		0x286230F8,		0x286230FC},	// Pipeline-3
};

// CSPR Low-Y Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL gIM_CNR_OTF_CSPR_LOW_Y_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// LCYSCLOF_1		LCYSCLOF_2		LCYSCLGA_1		LCYSCLGA_2		LCYSCLBD_1,		LCYSCLBD_2
	{0x284210E8,		0x284210EC,		0x284210F0,		0x284210F4,		0x284210F8,		0x284210FC},	// Pipeline-1
	{0x285210E8,		0x285210EC,		0x285210F0,		0x285210F4,		0x285210F8,		0x285210FC},	// Pipeline-2
	{0x286210E8,		0x286210EC,		0x286210F0,		0x286210F4,		0x286210F8,		0x286210FC},	// Pipeline-3
};

// CSPR Low-CC-Y Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL gIM_CNR_OFL_CSPR_LOW_CC_Y_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// LCCSCLOF_1		LCCSCLOF_2		LCCSCLGA_1		LCCSCLGA_2		LCCSCLBD_1,		LCCSCLBD_2
	{0x28423108,		0x2842310C,		0x28423110,		0x28423114,		0x28423118,		0x2842311C},	// Pipeline-1
	{0x28523108,		0x2852310C,		0x28523110,		0x28523114,		0x28523118,		0x2852311C},	// Pipeline-2
	{0x28623108,		0x2862310C,		0x28623110,		0x28623114,		0x28623118,		0x2862311C},	// Pipeline-3
};

// CSPR Low-CC-Y Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL gIM_CNR_OTF_CSPR_LOW_CC_Y_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// LCCSCLOF_1		LCCSCLOF_2		LCCSCLGA_1		LCCSCLGA_2		LCCSCLBD_1,		LCCSCLBD_2
	{0x28421108,		0x2842110C,		0x28421110,		0x28421114,		0x28421118,		0x2842111C},	// Pipeline-1
	{0x28521108,		0x2852110C,		0x28521110,		0x28521114,		0x28521118,		0x2852111C},	// Pipeline-2
	{0x28621108,		0x2862110C,		0x28621110,		0x28621114,		0x28621118,		0x2862111C},	// Pipeline-3
};

// CSPR Low-CC-C Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL gIM_CNR_OFL_CSPR_LOW_CC_C_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// LCSSCLOF_1		LCSSCLOF_2		LCSSCLGA_1		LCSSCLGA_2		LCSSCLBD_1,		LCSSCLBD_2
	{0x28423128,		0x2842312C,		0x28423130,		0x28423134,		0x28423138,		0x2842313C},	// Pipeline-1
	{0x28523128,		0x2852312C,		0x28523130,		0x28523134,		0x28523138,		0x2852313C},	// Pipeline-2
	{0x28623128,		0x2862312C,		0x28623130,		0x28623134,		0x28623138,		0x2862313C},	// Pipeline-3
};

// CSPR Low-CC-C Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL gIM_CNR_OTF_CSPR_LOW_CC_C_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// LCSSCLOF_1		LCSSCLOF_2		LCSSCLGA_1		LCSSCLGA_2		LCSSCLBD_1,		LCSSCLBD_2
	{0x28421128,		0x2842112C,		0x28421130,		0x28421134,		0x28421138,		0x2842113C},	// Pipeline-1
	{0x28521128,		0x2852112C,		0x28521130,		0x28521134,		0x28521138,		0x2852113C},	// Pipeline-2
	{0x28621128,		0x2862112C,		0x28621130,		0x28621134,		0x28621138,		0x2862113C},	// Pipeline-3
};

// CSPR EDGE Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL gIM_CNR_OFL_CSPR_EDGE_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// CESCLOF_1		CESCLOF_2		CESCLGA_1		CESCLGA_2		CESCLBD_1,		CESCLBD_2
	{0x28423168,		0x2842316C,		0x28423170,		0x28423174,		0x28423178,		0x2842317C},	// Pipeline-1
	{0x28523168,		0x2852316C,		0x28523170,		0x28523174,		0x28523178,		0x2852317C},	// Pipeline-2
	{0x28623168,		0x2862316C,		0x28623170,		0x28623174,		0x28623178,		0x2862317C},	// Pipeline-3
};

// CSPR EDGE Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL gIM_CNR_OTF_CSPR_EDGE_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// CESCLOF_1		CESCLOF_2		CESCLGA_1		CESCLGA_2		CESCLBD_1,		CESCLBD_2
	{0x28421168,		0x2842116C,		0x28421170,		0x28421174,		0x28421178,		0x2842117C},	// Pipeline-1
	{0x28521168,		0x2852116C,		0x28521170,		0x28521174,		0x28521178,		0x2852117C},	// Pipeline-2
	{0x28621168,		0x2862116C,		0x28621170,		0x28621174,		0x28621178,		0x2862117C},	// Pipeline-3
};

// CSPR Hue Specified Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL gIM_CNR_OFL_CSPR_HUE_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// CPHPT		CPHPTK		CDSCLOF_1	CDSCLOF_2	CDSCLOF_3,	CDSCLGA_1	CDSCLGA_2	CDSCLGA_3	CDSCLBD_1	CDSCLBD_2	CDSCLBD_3
	{0x28423190,	0x28423194,	0x284231A0,	0x284231A4,	0x284231A8,	0x284231B0,	0x284231B4,	0x284231B8,	0x284231C0,	0x284231C4,	0x284231C8},	// Pipeline-1
	{0x28523190,	0x28523194,	0x285231A0,	0x285231A4,	0x285231A8,	0x285231B0,	0x285231B4,	0x285231B8,	0x285231C0,	0x285231C4,	0x285231C8},	// Pipeline-2
	{0x28623190,	0x28623194,	0x286231A0,	0x286231A4,	0x286231A8,	0x286231B0,	0x286231B4,	0x286231B8,	0x286231C0,	0x286231C4,	0x286231C8},	// Pipeline-3
};

// CSPR Hue Specified Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL gIM_CNR_OTF_CSPR_HUE_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// CPHPT		CPHPTK		CDSCLOF_1	CDSCLOF_2	CDSCLOF_3,	CDSCLGA_1	CDSCLGA_2	CDSCLGA_3	CDSCLBD_1	CDSCLBD_2	CDSCLBD_3
	{0x28421190,	0x28421194,	0x284211A0,	0x284211A4,	0x284211A8,	0x284211B0,	0x284211B4,	0x284211B8,	0x284211C0,	0x284211C4,	0x284211C8},	// Pipeline-1
	{0x28521190,	0x28521194,	0x285211A0,	0x285211A4,	0x285211A8,	0x285211B0,	0x285211B4,	0x285211B8,	0x285211C0,	0x285211C4,	0x285211C8},	// Pipeline-2
	{0x28621190,	0x28621194,	0x286211A0,	0x286211A4,	0x286211A8,	0x286211B0,	0x286211B4,	0x286211B8,	0x286211C0,	0x286211C4,	0x286211C8},	// Pipeline-3
};

// CSPR SPRS_CC Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL gIM_CNR_OFL_SPRS_CC_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// SPRS,			SPRSCBOF_1		SPRSCBOF_2		SPRSCBGA_1		SPRSCBGA_2		SPRSCBGA_3,		SPRSCBBD_1		SPRSCBBD_2
	// SPRSCROF_1		SPRSCROF_2		SPRSCRGA_1		SPRSCRGA_2		SPRSCRGA_3,		SPRSCRBD_1		SPRSCRBD_2
	// Pipeline-1
	{0x28423200,		0x28423208,		0x2842320C,		0x28423210,		0x28423214,		0x28423218,		0x28423220,		0x28423224,
	 0x28423228,		0x2842322C,		0x28423230,		0x28423234,		0x28423238,		0x28423240,		0x28423244},
	// Pipeline-2
	{0x28523200,		0x28523208,		0x2852320C,		0x28523210,		0x28523214,		0x28523218,		0x28523220,		0x28523224,
	 0x28523228,		0x2852322C,		0x28523230,		0x28523234,		0x28523238,		0x28523240,		0x28523244},
	// Pipeline-3
	{0x28623200,		0x28623208,		0x2862320C,		0x28623210,		0x28623214,		0x28623218,		0x28623220,		0x28623224,
	 0x28623228,		0x2862322C,		0x28623230,		0x28623234,		0x28623238,		0x28623240,		0x28623244},
};

// CSPR SPRS_CC Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL gIM_CNR_OTF_SPRS_CC_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// SPRS,			SPRSCBOF_1		SPRSCBOF_2		SPRSCBGA_1		SPRSCBGA_2		SPRSCBGA_3,		SPRSCBBD_1		SPRSCBBD_2
	// SPRSCROF_1		SPRSCROF_2		SPRSCRGA_1		SPRSCRGA_2		SPRSCRGA_3,		SPRSCRBD_1		SPRSCRBD_2
	// Pipeline-1
	{0x28421200,		0x28421208,		0x2842120C,		0x28421210,		0x28421214,		0x28421218,		0x28421220,		0x28421224,
	 0x28421228,		0x2842122C,		0x28421230,		0x28421234,		0x28421238,		0x28421240,		0x28421244},
	// Pipeline-2
	{0x28521200,		0x28521208,		0x2852120C,		0x28521210,		0x28521214,		0x28521218,		0x28521220,		0x28521224,
	 0x28521228,		0x2852122C,		0x28521230,		0x28521234,		0x28521238,		0x28521240,		0x28521244},
	// Pipeline-3
	{0x28621200,		0x28621208,		0x2862120C,		0x28621210,		0x28621214,		0x28621218,		0x28621220,		0x28621224,
	 0x28621228,		0x2862122C,		0x28621230,		0x28621234,		0x28621238,		0x28621240,		0x28621244},
};

// CSPR SPRD_CC Table address (OFL) ---
const static T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL gIM_CNR_OFL_SPRD_CC_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// SPRD,			SPRDCBOF_1		SPRDCBOF_2		SPRDCBGA_1		SPRDCBGA_2		SPRDCBGA_3,		SPRDCBBD_1		SPRDCBBD_2
	// SPRDCROF_1		SPRDCROF_2		SPRDCRGA_1		SPRDCRGA_2		SPRDCRGA_3,		SPRDCRBD_1		SPRDCRBD_2
	// Pipeline-1
	{0x28423280,		0x28423288,		0x2842328C,		0x28423290,		0x28423294,		0x28423298,		0x284232A0,		0x284232A4,
	 0x284232A8,		0x284232AC,		0x284232B0,		0x284232B4,		0x284232B8,		0x284232C0,		0x284232C4},
	// Pipeline-2
	{0x28523280,		0x28523288,		0x2852328C,		0x28523290,		0x28523294,		0x28523298,		0x285232A0,		0x285232A4,
	 0x285232A8,		0x285232AC,		0x285232B0,		0x285232B4,		0x285232B8,		0x285232C0,		0x285232C4},
	// Pipeline-3
	{0x28623280,		0x28623288,		0x2862328C,		0x28623290,		0x28623294,		0x28623298,		0x286232A0,		0x286232A4,
	 0x286232A8,		0x286232AC,		0x286232B0,		0x286232B4,		0x286232B8,		0x286232C0,		0x286232C4},
};

// CSPR SPRD_CC Table address (OTF) ---
const static T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL gIM_CNR_OTF_SPRD_CC_TBL[D_IM_CNR_CH_NUM_MAX] = {
	// SPRD,			SPRDCBOF_1		SPRDCBOF_2		SPRDCBGA_1		SPRDCBGA_2		SPRDCBGA_3,		SPRDCBBD_1		SPRDCBBD_2
	// SPRDCROF_1		SPRDCROF_2		SPRDCRGA_1		SPRDCRGA_2		SPRDCRGA_3,		SPRDCRBD_1		SPRDCRBD_2
	// Pipeline-1
	{0x28421280,		0x28421288,		0x2842128C,		0x28421290,		0x28421294,		0x28421298,		0x284212A0,		0x284212A4,
	 0x284212A8,		0x284212AC,		0x284212B0,		0x284212B4,		0x284212B8,		0x284212C0,		0x284212C4},
	// Pipeline-2
	{0x28521280,		0x28521288,		0x2852128C,		0x28521290,		0x28521294,		0x28521298,		0x285212A0,		0x285212A4,
	 0x285212A8,		0x285212AC,		0x285212B0,		0x285212B4,		0x285212B8,		0x285212C0,		0x285212C4},
	// Pipeline-3
	{0x28621280,		0x28621288,		0x2862128C,		0x28621290,		0x28621294,		0x28621298,		0x286212A0,		0x286212A4,
	 0x286212A8,		0x286212AC,		0x286212B0,		0x286212B4,		0x286212B8,		0x286212C0,		0x286212C4},
};
#endif


/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

static VOID im_cnr_ofl_set_spde( const UCHAR ch, const UCHAR onoff );
static VOID im_cnr_otf_set_spde( const UCHAR ch, const UCHAR onoff );
static VOID im_cnr_ofl_clear_axif( UCHAR ch );
static VOID im_cnr_otf_clear_axif( UCHAR ch );
static VOID im_cnr_ofl_clear_spdf( UCHAR ch );
static VOID im_cnr_otf_clear_spdf( UCHAR ch );
static INT32 im_cnr_ofl_start_core( const UCHAR ch, const UCHAR sync, const INT32 wait_time );
static INT32 im_cnr_otf_start_core( const UCHAR ch, const UCHAR sync, const INT32 wait_time );
static VOID im_CNR_on_spr_clk( UCHAR ch );
static VOID im_CNR_off_spr_clk( UCHAR ch );


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Interrupt Enable/Disable control
static VOID im_cnr_ofl_set_spde( const UCHAR ch, const UCHAR onoff )
{
	union io_cnr_ofl_reg_rw_sprice spric;

	spric.word = 0;
	spric.bit.SPDE = onoff;
	spric.bit.AXIE = onoff;

	IO_CNR(ch)->OFL_REG_RW.SPRICE.word = spric.word;
#ifdef CO_DEBUG_ON_PC
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRICE.word = spric.word;
		IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRICE.word = spric.word;
	}
#endif	// CO_DEBUG_ON_PC
}

static VOID im_cnr_otf_set_spde( const UCHAR ch, const UCHAR onoff )
{
	union io_cnr_otf_reg_rw_sprice spric;

	spric.word = 0;
	spric.bit.SPDE = onoff;
	spric.bit.AXIE = onoff;
	spric.bit.DRTE = onoff;

	IO_CNR(ch)->OTF_REG_RW.SPRICE.word = spric.word;
#ifdef CO_DEBUG_ON_PC
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICE.word = spric.word;
		IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICE.word = spric.word;
	}
#endif	// CO_DEBUG_ON_PC
}

// AXIF Interrupt flag clear
static VOID im_cnr_ofl_clear_axif( UCHAR ch )
{
#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OFL_REG_RW.SPRICF.bit.__AXIF = 0;
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRICF.bit.__AXIF = 0;
		IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRICF.bit.__AXIF = 0;
	}
#else //!CO_DEBUG_ON_PC
	union io_cnr_ofl_reg_rw_spricf spricf;

	spricf.word = 0;
	spricf.bit.__AXIF = 1;

	IO_CNR(ch)->OFL_REG_RW.SPRICF.word = spricf.word;
#endif //!CO_DEBUG_ON_PC
}

// AXIF Interrupt flag clear
static VOID im_cnr_otf_clear_axif( UCHAR ch )
{
#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OTF_REG_RW.SPRICF.bit.__AXIF = 0;
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__AXIF = 0;
		IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__AXIF = 0;
	}
#else //!CO_DEBUG_ON_PC
	union io_cnr_otf_reg_rw_spricf spricf;

	spricf.word = 0;
	spricf.bit.__AXIF = 1;

	IO_CNR(ch)->OTF_REG_RW.SPRICF.word = spricf.word;
#endif //!CO_DEBUG_ON_PC
}

// DRTF Interrupt flag clear
static VOID im_cnr_otf_clear_drtf( UCHAR ch )
{
#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OTF_REG_RW.SPRICF.bit.__DRTF = 0;
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__DRTF = 0;
		IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__DRTF = 0;
	}
#else //!CO_DEBUG_ON_PC
	union io_cnr_otf_reg_rw_spricf spricf;

	spricf.word = 0;
	spricf.bit.__DRTF = 1;

	IO_CNR(ch)->OTF_REG_RW.SPRICF.word = spricf.word;
#endif //!CO_DEBUG_ON_PC
}

// SPDF Interrupt flag clear
static VOID im_cnr_ofl_clear_spdf( UCHAR ch )
{
#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OFL_REG_RW.SPRICF.bit.__SPDF = 0;
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRICF.bit.__SPDF = 0;
		IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRICF.bit.__SPDF = 0;
	}
#else //!CO_DEBUG_ON_PC
	union io_cnr_ofl_reg_rw_spricf spricf;

	spricf.word = 0;
	spricf.bit.__SPDF = 1;

	IO_CNR(ch)->OFL_REG_RW.SPRICF.word = spricf.word;
#endif //!CO_DEBUG_ON_PC
}

// SPDF Interrupt flag clear
static VOID im_cnr_otf_clear_spdf( UCHAR ch )
{
#ifdef CO_DEBUG_ON_PC
	IO_CNR(ch)->OTF_REG_RW.SPRICF.bit.__SPDF = 0;
	if( ch == D_IM_CNR_CH_2 ){
		IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__SPDF = 0;
		IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__SPDF = 0;
	}
#else //!CO_DEBUG_ON_PC
	union io_cnr_otf_reg_rw_spricf spricf;

	spricf.word = 0;
	spricf.bit.__SPDF = 1;

	IO_CNR(ch)->OTF_REG_RW.SPRICF.word = spricf.word;
#endif //!CO_DEBUG_ON_PC
}

// CNR(OFT) Start CORE
static INT32 im_cnr_ofl_start_core( const UCHAR ch, const UCHAR sync, const INT32 wait_time )
{
	INT32 ercd;
	UINT32 flg_ptn;

	// SUPPRE End Flag disable
	if (ch == D_IM_CNR_CH_0) {
		gim_cnr_mode[D_IM_CNR_CH_0] = D_IM_CNR_MODE_OFL;
		DDIM_User_Clr_Flg( FID_IM_CNR, ~D_IM_CNR_OFL_0_WAIT_FLGPTN );
	}
	else if (ch == D_IM_CNR_CH_1) {
		gim_cnr_mode[D_IM_CNR_CH_1] = D_IM_CNR_MODE_OFL;
		DDIM_User_Clr_Flg( FID_IM_CNR, ~D_IM_CNR_OFL_1_WAIT_FLGPTN );
	}
	else {
		gim_cnr_mode[D_IM_CNR_CH_0] = D_IM_CNR_MODE_OFL;
		gim_cnr_mode[D_IM_CNR_CH_1] = D_IM_CNR_MODE_OFL;
		DDIM_User_Clr_Flg( FID_IM_CNR, ~(D_IM_CNR_OFL_0_WAIT_FLGPTN|D_IM_CNR_OFL_1_WAIT_FLGPTN) );
	}

	im_CNR_on_spr_clk(ch);
	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	// Set Acess Enable
	IO_CNR(ch)->OFL_REG_RW.ACESEN.bit.SPRACS = 1;

	// wait 1us
	im_cnr_wait_usec(D_IM_CNR_SRAM_WAIT_USEC);

	Im_CNR_OFL_Enable(ch);

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

#ifdef CO_DEBUG_ON_PC
	if( ch == D_IM_CNR_CH_0 ){
		IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRICF.bit.__SPDF = 1;
	}
	else if( ch == D_IM_CNR_CH_1 ){
		IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRICF.bit.__SPDF = 1;
	}
	else{
		IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRICF.bit.__SPDF = 1;
		IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRICF.bit.__SPDF = 1;
	}
#endif //CO_DEBUG_ON_PC

	if( D_IM_CNR_ASYNC == sync ) {
		return D_DDIM_OK;
	}

#ifdef CO_DEBUG_ON_PC
	if( ch == D_IM_CNR_CH_2 ){
		if( (IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRICF.bit.__SPDF != 0)
		 || (IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRICF.bit.__AXIF != 0)
		 || (IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRICF.bit.__SPDF != 0)
		 || (IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRICF.bit.__AXIF != 0) ) {
			Im_CNR0_Int_Handler();
			Im_CNR1_Int_Handler();
		}
	}
	else{
		if( (IO_CNR(ch)->OFL_REG_RW.SPRICF.bit.__SPDF != 0)
		 || (IO_CNR(ch)->OFL_REG_RW.SPRICF.bit.__AXIF != 0) ) {
			if (ch == D_IM_CNR_CH_0) {
				Im_CNR0_Int_Handler();
			}
			else {
				Im_CNR1_Int_Handler();
			}
		}
	}
#endif //CO_DEBUG_ON_PC

	// Wait for Process end with Sync
	ercd = Im_CNR_OFL_Wait_End( ch, &flg_ptn, wait_time );
	if( ercd != D_DDIM_OK ) {
		return ercd;
	}
	if (ch == D_IM_CNR_CH_0) {
		if( (flg_ptn & D_IM_CNR_FLG_0_OFL_SPR_AXI_ERR) != 0 ) {
			return D_IM_CNR_AXI_ERR;
		}
	}
	else if (ch == D_IM_CNR_CH_1) {
		if( (flg_ptn & D_IM_CNR_FLG_1_OFL_SPR_AXI_ERR) != 0 ) {
			return D_IM_CNR_AXI_ERR;
		}
	}
	else {
		if( (flg_ptn & D_IM_CNR_FLG_0_OFL_SPR_AXI_ERR) != 0 ) {
			return D_IM_CNR_AXI_ERR;
		}
		if( (flg_ptn & D_IM_CNR_FLG_1_OFL_SPR_AXI_ERR) != 0 ) {
			return D_IM_CNR_AXI_ERR;
		}
	}

	return D_DDIM_OK;
}

// CNR(OTF) Start CORE
static INT32 im_cnr_otf_start_core( const UCHAR ch, const UCHAR sync, const INT32 wait_time )
{
	INT32 ercd;
	UINT32 flg_ptn;

	// SUPPRE End Flag disable
	if (ch == D_IM_CNR_CH_0) {
		gim_cnr_mode[D_IM_CNR_CH_0] = D_IM_CNR_MODE_OTF;
		DDIM_User_Clr_Flg( FID_IM_CNR, ~D_IM_CNR_OTF_0_WAIT_FLGPTN );
	}
	else if (ch == D_IM_CNR_CH_1) {
		gim_cnr_mode[D_IM_CNR_CH_1] = D_IM_CNR_MODE_OTF;
		DDIM_User_Clr_Flg( FID_IM_CNR, ~D_IM_CNR_OTF_1_WAIT_FLGPTN );
	}
	else {
		gim_cnr_mode[D_IM_CNR_CH_0] = D_IM_CNR_MODE_OTF;
		gim_cnr_mode[D_IM_CNR_CH_1] = D_IM_CNR_MODE_OTF;
		DDIM_User_Clr_Flg( FID_IM_CNR, ~(D_IM_CNR_OTF_0_WAIT_FLGPTN|D_IM_CNR_OTF_1_WAIT_FLGPTN) );
	}

	im_CNR_on_spr_clk(ch);
	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	// Set Acess Enable
	IO_CNR(ch)->OTF_REG_RW.ACESEN.bit.SPRACS = 1;

	// wait 1us
	im_cnr_wait_usec(D_IM_CNR_SRAM_WAIT_USEC);

	Im_CNR_OTF_Enable(ch);

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

#ifdef CO_DEBUG_ON_PC
	if( ch == D_IM_CNR_CH_0 ){
		IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
	}
	else if( ch == D_IM_CNR_CH_1 ){
		IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
	}
	else{
		IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
		IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__SPDF = 1;
	}
#endif //CO_DEBUG_ON_PC

	if( D_IM_CNR_ASYNC == sync ) {
		return D_DDIM_OK;
	}

#ifdef CO_DEBUG_ON_PC
	if( ch == D_IM_CNR_CH_2 ){
		if( (IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__SPDF != 0)
		 || (IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__AXIF != 0)
		 || (IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRICF.bit.__DRTF != 0)
		 || (IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__SPDF != 0)
		 || (IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__AXIF != 0)
		 || (IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRICF.bit.__DRTF != 0) ) {
			Im_CNR0_Int_Handler();
			Im_CNR1_Int_Handler();
		}
	}
	else{
		if( (IO_CNR(ch)->OTF_REG_RW.SPRICF.bit.__SPDF != 0)
		 || (IO_CNR(ch)->OTF_REG_RW.SPRICF.bit.__AXIF != 0)
		 || (IO_CNR(ch)->OTF_REG_RW.SPRICF.bit.__DRTF != 0) ) {
			if (ch == D_IM_CNR_CH_0) {
				Im_CNR0_Int_Handler();
			}
			else{
				Im_CNR1_Int_Handler();
			}
		}
	}
#endif //CO_DEBUG_ON_PC

	// Wait for Process end with Sync
	ercd = Im_CNR_OTF_Wait_End( ch, &flg_ptn, wait_time );
	if( ercd != D_DDIM_OK ) {
		return ercd;
	}
	if (ch == D_IM_CNR_CH_0) {
		if( (flg_ptn & D_IM_CNR_FLG_0_OTF_SPR_AXI_ERR) != 0 ) {
			return D_IM_CNR_AXI_ERR;
		}
		if( (flg_ptn & D_IM_CNR_FLG_0_OTF_SPR_DRT_ERR) != 0 ) {
			return D_IM_CNR_DRT_ERR;
		}
	}
	else if (ch == D_IM_CNR_CH_1) {
		if( (flg_ptn & D_IM_CNR_FLG_1_OTF_SPR_AXI_ERR) != 0 ) {
			return D_IM_CNR_AXI_ERR;
		}
		if( (flg_ptn & D_IM_CNR_FLG_1_OTF_SPR_DRT_ERR) != 0 ) {
			return D_IM_CNR_DRT_ERR;
		}
	}
	else {
		if( (flg_ptn & D_IM_CNR_FLG_0_OTF_SPR_AXI_ERR) != 0 ) {
			return D_IM_CNR_AXI_ERR;
		}
		if( (flg_ptn & D_IM_CNR_FLG_1_OTF_SPR_AXI_ERR) != 0 ) {
			return D_IM_CNR_AXI_ERR;
		}
		if( (flg_ptn & D_IM_CNR_FLG_0_OTF_SPR_DRT_ERR) != 0 ) {
			return D_IM_CNR_DRT_ERR;
		}
		if( (flg_ptn & D_IM_CNR_FLG_1_OTF_SPR_DRT_ERR) != 0 ) {
			return D_IM_CNR_DRT_ERR;
		}
	}

	return D_DDIM_OK;
}
// Start CNR ICLK, CNRCLK
static VOID im_CNR_on_spr_clk( const UCHAR ch )
{
#if defined(CO_ACT_CNR_ICLK) || defined(CO_ACT_CNRCLOCK)
	INT32 intkey;

	intkey = Dd_ARM_DI();
#endif // CO_ACT_CNR_ICLK || CO_ACT_CNRCLOCK

	if (ch == D_IM_CNR_CH_0) {
#ifdef CO_ACT_CNRCLOCK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), ~D_DD_TOP_CNR1CK_BIT );
#endif //CO_ACT_CNRCLOCK
#ifdef CO_ACT_CNR_ICLK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), ~D_DD_TOP_CNR1AX_BIT );
#endif //CO_ACT_CNR_ICLK
	}
	else if (ch == D_IM_CNR_CH_1) {
#ifdef CO_ACT_CNRCLOCK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_CNR2CK_BIT );
#endif //CO_ACT_CNRCLOCK
#ifdef CO_ACT_CNR_ICLK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_CNR2AX_BIT );
#endif //CO_ACT_CNR_ICLK
	}
	else {
#ifdef CO_ACT_CNRCLOCK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), ~D_DD_TOP_CNR1CK_BIT );
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_CNR2CK_BIT );
#endif //CO_ACT_CNRCLOCK
#ifdef CO_ACT_CNR_ICLK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), ~D_DD_TOP_CNR1AX_BIT );
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_CNR2AX_BIT );
#endif //CO_ACT_CNR_ICLK
	}

#if defined(CO_ACT_CNR_ICLK) || defined(CO_ACT_CNRCLOCK)
	Dd_ARM_EI( intkey );
#endif // CO_ACT_CNR_ICLK || CO_ACT_CNRCLOCK
}

// Stop CNR ICLK, CNRCLK
static VOID im_CNR_off_spr_clk( const UCHAR ch )
{
#if defined(CO_ACT_CNR_ICLK) || defined(CO_ACT_CNRCLOCK)
	INT32 intkey;

	intkey = Dd_ARM_DI();
#endif // CO_ACT_CNR_ICLK || CO_ACT_CNRCLOCK

	if (ch == D_IM_CNR_CH_0) {
#ifdef CO_ACT_CNR_ICLK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), D_DD_TOP_CNR1AX_BIT );
#endif //CO_ACT_CNR_ICLK

#ifdef CO_ACT_CNRCLOCK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), D_DD_TOP_CNR1CK_BIT );
#endif //CO_ACT_CNRCLOCK
	}
	else if (ch == D_IM_CNR_CH_1) {
#ifdef CO_ACT_CNR_ICLK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), D_DD_TOP_CNR2AX_BIT );
#endif //CO_ACT_CNR_ICLK

#ifdef CO_ACT_CNRCLOCK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), D_DD_TOP_CNR2CK_BIT );
#endif //CO_ACT_CNRCLOCK
	}
	else {
#ifdef CO_ACT_CNR_ICLK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), D_DD_TOP_CNR1AX_BIT );
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), D_DD_TOP_CNR2AX_BIT );
#endif //CO_ACT_CNR_ICLK

#ifdef CO_ACT_CNRCLOCK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), D_DD_TOP_CNR1CK_BIT );
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), D_DD_TOP_CNR2CK_BIT );
#endif //CO_ACT_CNRCLOCK
	}

#if defined(CO_ACT_CNR_ICLK) || defined(CO_ACT_CNRCLOCK)
	Dd_ARM_EI( intkey );
#endif // CO_ACT_CNR_ICLK || CO_ACT_CNRCLOCK
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/*
Convert T_IM_CNR_CTRL_C to T_IM_CNR_RDMA_DATA_CTRL_C
*/
static VOID im_cnr_convert_rdma_value_ctrl_c( const T_IM_CNR_CTRL_C* spr_ctrl_c, T_IM_CNR_RDMA_DATA_CTRL_C* rdma_ctrl_c )
{
	rdma_ctrl_c->CSPREN.bit.CSPREN	= spr_ctrl_c->enable;
	rdma_ctrl_c->CGDKEN.bit.CGDKEN	= spr_ctrl_c->gradation_keep_en;
	rdma_ctrl_c->MCEN.bit.MCEN		= spr_ctrl_c->mid_freq_nr.enable;

	rdma_ctrl_c->MCYTHH.bit.MCYTHH_0	= spr_ctrl_c->mid_freq_nr.h_y_threshold[0];
	rdma_ctrl_c->MCYTHH.bit.MCYTHH_1	= spr_ctrl_c->mid_freq_nr.h_y_threshold[1];
	rdma_ctrl_c->MCYTHH.bit.MCYTHH_2	= spr_ctrl_c->mid_freq_nr.h_y_threshold[2];
	rdma_ctrl_c->MCYTHH.bit.MCYTHH_3	= spr_ctrl_c->mid_freq_nr.h_y_threshold[3];
	rdma_ctrl_c->MCYTHV.bit.MCYTHV_0	= spr_ctrl_c->mid_freq_nr.v_y_threshold[0];
	rdma_ctrl_c->MCYTHV.bit.MCYTHV_1	= spr_ctrl_c->mid_freq_nr.v_y_threshold[1];
	rdma_ctrl_c->MCYTHV.bit.MCYTHV_2	= spr_ctrl_c->mid_freq_nr.v_y_threshold[2];
	rdma_ctrl_c->MCYTHV.bit.MCYTHV_3	= spr_ctrl_c->mid_freq_nr.v_y_threshold[3];
	rdma_ctrl_c->MCCTHH.bit.MCCTHH_0	= spr_ctrl_c->mid_freq_nr.h_c_threshold[0];
	rdma_ctrl_c->MCCTHH.bit.MCCTHH_1	= spr_ctrl_c->mid_freq_nr.h_c_threshold[1];
	rdma_ctrl_c->MCCTHH.bit.MCCTHH_2	= spr_ctrl_c->mid_freq_nr.h_c_threshold[2];
	rdma_ctrl_c->MCCTHH.bit.MCCTHH_3	= spr_ctrl_c->mid_freq_nr.h_c_threshold[3];
	rdma_ctrl_c->MCCTHV.bit.MCCTHV_0	= spr_ctrl_c->mid_freq_nr.v_c_threshold[0];
	rdma_ctrl_c->MCCTHV.bit.MCCTHV_1	= spr_ctrl_c->mid_freq_nr.v_c_threshold[1];
	rdma_ctrl_c->MCCTHV.bit.MCCTHV_2	= spr_ctrl_c->mid_freq_nr.v_c_threshold[2];
	rdma_ctrl_c->MCCTHV.bit.MCCTHV_3	= spr_ctrl_c->mid_freq_nr.v_c_threshold[3];

	rdma_ctrl_c->MCYDYMDT.bit.MCYDYMYCR	= spr_ctrl_c->mid_freq_nr.y_threshold_cor;
	rdma_ctrl_c->MCYDYMDT.bit.MCYDYMYG	= spr_ctrl_c->mid_freq_nr.y_threshold_gain;
	rdma_ctrl_c->MCYDYM.bit.MCYDYM		= spr_ctrl_c->mid_freq_nr.y_threshold_dym_en;
	rdma_ctrl_c->MCYSCL.bit.MCYSCL		= spr_ctrl_c->mid_freq_nr.y_threshold_scl_en;
	rdma_ctrl_c->MCSSCL.bit.MCSSCL		= spr_ctrl_c->mid_freq_nr.c_y_threshold_scl_en;
	rdma_ctrl_c->MCCSCL.bit.MCCSCL		= spr_ctrl_c->mid_freq_nr.c_threshold_scl_en;
	rdma_ctrl_c->LCEN.bit.LCEN			= spr_ctrl_c->low_freq_nr.enable;
	rdma_ctrl_c->LCYTHH.bit.LCYTHH_0	= spr_ctrl_c->low_freq_nr.h_y_threshold[0];
	rdma_ctrl_c->LCYTHH.bit.LCYTHH_1	= spr_ctrl_c->low_freq_nr.h_y_threshold[1];
	rdma_ctrl_c->LCYTHH.bit.LCYTHH_2	= spr_ctrl_c->low_freq_nr.h_y_threshold[2];
	rdma_ctrl_c->LCYTHH.bit.LCYTHH_3	= spr_ctrl_c->low_freq_nr.h_y_threshold[3];
	rdma_ctrl_c->LCYTHV.bit.LCYTHV_0	= spr_ctrl_c->low_freq_nr.v_y_threshold[0];
	rdma_ctrl_c->LCYTHV.bit.LCYTHV_1	= spr_ctrl_c->low_freq_nr.v_y_threshold[1];
	rdma_ctrl_c->LCYTHV.bit.LCYTHV_2	= spr_ctrl_c->low_freq_nr.v_y_threshold[2];
	rdma_ctrl_c->LCYTHV.bit.LCYTHV_3	= spr_ctrl_c->low_freq_nr.v_y_threshold[3];
	rdma_ctrl_c->LCCTHH.bit.LCCTHH_0	= spr_ctrl_c->low_freq_nr.h_c_threshold[0];
	rdma_ctrl_c->LCCTHH.bit.LCCTHH_1	= spr_ctrl_c->low_freq_nr.h_c_threshold[1];
	rdma_ctrl_c->LCCTHH.bit.LCCTHH_2	= spr_ctrl_c->low_freq_nr.h_c_threshold[2];
	rdma_ctrl_c->LCCTHH.bit.LCCTHH_3	= spr_ctrl_c->low_freq_nr.h_c_threshold[3];
	rdma_ctrl_c->LCCTHV.bit.LCCTHV_0	= spr_ctrl_c->low_freq_nr.v_c_threshold[0];
	rdma_ctrl_c->LCCTHV.bit.LCCTHV_1	= spr_ctrl_c->low_freq_nr.v_c_threshold[1];
	rdma_ctrl_c->LCCTHV.bit.LCCTHV_2	= spr_ctrl_c->low_freq_nr.v_c_threshold[2];
	rdma_ctrl_c->LCCTHV.bit.LCCTHV_3	= spr_ctrl_c->low_freq_nr.v_c_threshold[3];
	rdma_ctrl_c->LCYDYMDT.bit.LCYDYMYCR	= spr_ctrl_c->low_freq_nr.y_threshold_cor;
	rdma_ctrl_c->LCYDYMDT.bit.LCYDYMYG	= spr_ctrl_c->low_freq_nr.y_threshold_gain;
	rdma_ctrl_c->LCYDYM.bit.LCYDYM	= spr_ctrl_c->low_freq_nr.y_threshold_dym_en;
	rdma_ctrl_c->LCYSCL.bit.LCYSCL	= spr_ctrl_c->low_freq_nr.y_threshold_scl_en;
	rdma_ctrl_c->LCSSCL.bit.LCSSCL	= spr_ctrl_c->low_freq_nr.c_y_threshold_scl_en;
	rdma_ctrl_c->LCCSCL.bit.LCCSCL	= spr_ctrl_c->low_freq_nr.c_threshold_scl_en;

	rdma_ctrl_c->WHTMD.bit.WHTEN	= spr_ctrl_c->whitening_enable;
	rdma_ctrl_c->WHTMD.bit.WHTVAL	= spr_ctrl_c->whitening_value;
	rdma_ctrl_c->WHTMD.bit.WHTBASE	= spr_ctrl_c->whitening_base;

	rdma_ctrl_c->BLEND.bit.BLENDMD		= spr_ctrl_c->c_blend_md;
	rdma_ctrl_c->BLEND.bit.CSPRALPBD	= spr_ctrl_c->c_alpha_blend;
	rdma_ctrl_c->BLEND.bit.CSRGEN		= spr_ctrl_c->c_range;
	rdma_ctrl_c->BLEND.bit.CSRGMD		= spr_ctrl_c->c_range_mode;
}

/*
Convert T_IM_CNR_TABLE to T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL
*/
static VOID im_cnr_convert_rdma_cspr_mid_y_table( const T_IM_CNR_TABLE* spr_cspr_mid_y, T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL* rdma_cspr_mid_y_table )
{
	rdma_cspr_mid_y_table->MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_1	= spr_cspr_mid_y->bd1;
	rdma_cspr_mid_y_table->MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_2	= spr_cspr_mid_y->bd2;
	rdma_cspr_mid_y_table->MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_3	= spr_cspr_mid_y->bd3;
	rdma_cspr_mid_y_table->MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_4	= spr_cspr_mid_y->bd4;
	rdma_cspr_mid_y_table->MCYSCLBD.MCYSCLBD2.bit.MCYSCLBD_5	= spr_cspr_mid_y->bd5;
	rdma_cspr_mid_y_table->MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_0	= spr_cspr_mid_y->of0;
	rdma_cspr_mid_y_table->MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_1	= spr_cspr_mid_y->of1;
	rdma_cspr_mid_y_table->MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_2	= spr_cspr_mid_y->of2;
	rdma_cspr_mid_y_table->MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_3	= spr_cspr_mid_y->of3;
	rdma_cspr_mid_y_table->MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_4	= spr_cspr_mid_y->of4;
	rdma_cspr_mid_y_table->MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_5	= spr_cspr_mid_y->of5;
	// write signed data
	im_cnr_set_reg_signed( rdma_cspr_mid_y_table->MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_0, spr_cspr_mid_y->gain0 );
	im_cnr_set_reg_signed( rdma_cspr_mid_y_table->MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_1, spr_cspr_mid_y->gain1 );
	im_cnr_set_reg_signed( rdma_cspr_mid_y_table->MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_2, spr_cspr_mid_y->gain2 );
	im_cnr_set_reg_signed( rdma_cspr_mid_y_table->MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_3, spr_cspr_mid_y->gain3 );
	im_cnr_set_reg_signed( rdma_cspr_mid_y_table->MCYSCLGA.MCYSCLGA2, union io_cnr_cspr_reg_mcysclga_2, MCYSCLGA_4, spr_cspr_mid_y->gain4 );
	im_cnr_set_reg_signed( rdma_cspr_mid_y_table->MCYSCLGA.MCYSCLGA2, union io_cnr_cspr_reg_mcysclga_2, MCYSCLGA_5, spr_cspr_mid_y->gain5 );
}

/*
Convert T_IM_CNR_TABLE to T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL
*/
static VOID im_cnr_convert_rdma_cspr_mid_cc_y_table( const T_IM_CNR_TABLE* cspr_mid_cc_y, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL* rdma_mid_cc_y_table )
{
	rdma_mid_cc_y_table->MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_1	= cspr_mid_cc_y->bd1;
	rdma_mid_cc_y_table->MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_2	= cspr_mid_cc_y->bd2;
	rdma_mid_cc_y_table->MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_3	= cspr_mid_cc_y->bd3;
	rdma_mid_cc_y_table->MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_4	= cspr_mid_cc_y->bd4;
	rdma_mid_cc_y_table->MCCSCLBD.MCCSCLBD2.bit.MCCSCLBD_5	= cspr_mid_cc_y->bd5;
	rdma_mid_cc_y_table->MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_0	= cspr_mid_cc_y->of0;
	rdma_mid_cc_y_table->MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_1	= cspr_mid_cc_y->of1;
	rdma_mid_cc_y_table->MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_2	= cspr_mid_cc_y->of2;
	rdma_mid_cc_y_table->MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_3	= cspr_mid_cc_y->of3;
	rdma_mid_cc_y_table->MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_4	= cspr_mid_cc_y->of4;
	rdma_mid_cc_y_table->MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_5	= cspr_mid_cc_y->of5;

	// write signed data
	im_cnr_set_reg_signed( rdma_mid_cc_y_table->MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_0, cspr_mid_cc_y->gain0 );
	im_cnr_set_reg_signed( rdma_mid_cc_y_table->MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_1, cspr_mid_cc_y->gain1 );
	im_cnr_set_reg_signed( rdma_mid_cc_y_table->MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_2, cspr_mid_cc_y->gain2 );
	im_cnr_set_reg_signed( rdma_mid_cc_y_table->MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_3, cspr_mid_cc_y->gain3 );
	im_cnr_set_reg_signed( rdma_mid_cc_y_table->MCCSCLGA.MCCSCLGA2, union io_cnr_cspr_reg_mccsclga_2, MCCSCLGA_4, cspr_mid_cc_y->gain4 );
	im_cnr_set_reg_signed( rdma_mid_cc_y_table->MCCSCLGA.MCCSCLGA2, union io_cnr_cspr_reg_mccsclga_2, MCCSCLGA_5, cspr_mid_cc_y->gain5 );
}

/*
Convert T_IM_CNR_TABLE to T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL
*/
static VOID im_cnr_convert_rdma_cspr_mid_cc_c_table( const T_IM_CNR_TABLE* cspr_mid_cc_c, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL* rdma_mid_cc_c_table )
{
	rdma_mid_cc_c_table->MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_1	= cspr_mid_cc_c->bd1;
	rdma_mid_cc_c_table->MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_2	= cspr_mid_cc_c->bd2;
	rdma_mid_cc_c_table->MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_3	= cspr_mid_cc_c->bd3;
	rdma_mid_cc_c_table->MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_4	= cspr_mid_cc_c->bd4;
	rdma_mid_cc_c_table->MCSSCLBD.MCSSCLBD2.bit.MCSSCLBD_5	= cspr_mid_cc_c->bd5;
	rdma_mid_cc_c_table->MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_0	= cspr_mid_cc_c->of0;
	rdma_mid_cc_c_table->MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_1	= cspr_mid_cc_c->of1;
	rdma_mid_cc_c_table->MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_2	= cspr_mid_cc_c->of2;
	rdma_mid_cc_c_table->MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_3	= cspr_mid_cc_c->of3;
	rdma_mid_cc_c_table->MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_4	= cspr_mid_cc_c->of4;
	rdma_mid_cc_c_table->MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_5	= cspr_mid_cc_c->of5;
	// write signed data
	im_cnr_set_reg_signed( rdma_mid_cc_c_table->MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_0, cspr_mid_cc_c->gain0 );
	im_cnr_set_reg_signed( rdma_mid_cc_c_table->MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_1, cspr_mid_cc_c->gain1 );
	im_cnr_set_reg_signed( rdma_mid_cc_c_table->MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_2, cspr_mid_cc_c->gain2 );
	im_cnr_set_reg_signed( rdma_mid_cc_c_table->MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_3, cspr_mid_cc_c->gain3 );
	im_cnr_set_reg_signed( rdma_mid_cc_c_table->MCSSCLGA.MCSSCLGA2, union io_cnr_cspr_reg_mcssclga_2, MCSSCLGA_4, cspr_mid_cc_c->gain4 );
	im_cnr_set_reg_signed( rdma_mid_cc_c_table->MCSSCLGA.MCSSCLGA2, union io_cnr_cspr_reg_mcssclga_2, MCSSCLGA_5, cspr_mid_cc_c->gain5 );
}

/*
Convert T_IM_CNR_TABLE to T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL
*/
static VOID im_cnr_convert_rdma_cspr_low_y_table( const T_IM_CNR_TABLE* spr_cspr_low_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL* rdma_cspr_low_y_table )
{
	rdma_cspr_low_y_table->LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_1	= spr_cspr_low_y->bd1;
	rdma_cspr_low_y_table->LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_2	= spr_cspr_low_y->bd2;
	rdma_cspr_low_y_table->LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_3	= spr_cspr_low_y->bd3;
	rdma_cspr_low_y_table->LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_4	= spr_cspr_low_y->bd4;
	rdma_cspr_low_y_table->LCYSCLBD.LCYSCLBD2.bit.LCYSCLBD_5	= spr_cspr_low_y->bd5;
	rdma_cspr_low_y_table->LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_0	= spr_cspr_low_y->of0;
	rdma_cspr_low_y_table->LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_1	= spr_cspr_low_y->of1;
	rdma_cspr_low_y_table->LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_2	= spr_cspr_low_y->of2;
	rdma_cspr_low_y_table->LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_3	= spr_cspr_low_y->of3;
	rdma_cspr_low_y_table->LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_4	= spr_cspr_low_y->of4;
	rdma_cspr_low_y_table->LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_5	= spr_cspr_low_y->of5;
	// write signed data
	im_cnr_set_reg_signed( rdma_cspr_low_y_table->LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_0, spr_cspr_low_y->gain0 );
	im_cnr_set_reg_signed( rdma_cspr_low_y_table->LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_1, spr_cspr_low_y->gain1 );
	im_cnr_set_reg_signed( rdma_cspr_low_y_table->LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_2, spr_cspr_low_y->gain2 );
	im_cnr_set_reg_signed( rdma_cspr_low_y_table->LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_3, spr_cspr_low_y->gain3 );
	im_cnr_set_reg_signed( rdma_cspr_low_y_table->LCYSCLGA.LCYSCLGA2, union io_cnr_cspr_reg_lcysclga_2, LCYSCLGA_4, spr_cspr_low_y->gain4 );
	im_cnr_set_reg_signed( rdma_cspr_low_y_table->LCYSCLGA.LCYSCLGA2, union io_cnr_cspr_reg_lcysclga_2, LCYSCLGA_5, spr_cspr_low_y->gain5 );
}

/*
Convert T_IM_CNR_TABLE to T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL
*/
static VOID im_cnr_convert_rdma_cspr_low_cc_y_table( const T_IM_CNR_TABLE* cspr_low_cc_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL* rdma_low_cc_y_table )
{
	rdma_low_cc_y_table->LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_1	= cspr_low_cc_y->bd1;
	rdma_low_cc_y_table->LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_2	= cspr_low_cc_y->bd2;
	rdma_low_cc_y_table->LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_3	= cspr_low_cc_y->bd3;
	rdma_low_cc_y_table->LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_4	= cspr_low_cc_y->bd4;
	rdma_low_cc_y_table->LCCSCLBD.LCCSCLBD2.bit.LCCSCLBD_5	= cspr_low_cc_y->bd5;
	rdma_low_cc_y_table->LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_0	= cspr_low_cc_y->of0;
	rdma_low_cc_y_table->LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_1	= cspr_low_cc_y->of1;
	rdma_low_cc_y_table->LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_2	= cspr_low_cc_y->of2;
	rdma_low_cc_y_table->LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_3	= cspr_low_cc_y->of3;
	rdma_low_cc_y_table->LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_4	= cspr_low_cc_y->of4;
	rdma_low_cc_y_table->LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_5	= cspr_low_cc_y->of5;
	// write signed data
	im_cnr_set_reg_signed( rdma_low_cc_y_table->LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_0, cspr_low_cc_y->gain0 );
	im_cnr_set_reg_signed( rdma_low_cc_y_table->LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_1, cspr_low_cc_y->gain1 );
	im_cnr_set_reg_signed( rdma_low_cc_y_table->LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_2, cspr_low_cc_y->gain2 );
	im_cnr_set_reg_signed( rdma_low_cc_y_table->LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_3, cspr_low_cc_y->gain3 );
	im_cnr_set_reg_signed( rdma_low_cc_y_table->LCCSCLGA.LCCSCLGA2, union io_cnr_cspr_reg_lccsclga_2, LCCSCLGA_4, cspr_low_cc_y->gain4 );
	im_cnr_set_reg_signed( rdma_low_cc_y_table->LCCSCLGA.LCCSCLGA2, union io_cnr_cspr_reg_lccsclga_2, LCCSCLGA_5, cspr_low_cc_y->gain5 );
}

/*
Convert T_IM_CNR_TABLE to T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL
*/
static VOID im_cnr_convert_rdma_cspr_low_cc_c_table( const T_IM_CNR_TABLE* cspr_low_cc_c, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL* rdma_low_cc_c_table )
{
	rdma_low_cc_c_table->LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_1	= cspr_low_cc_c->bd1;
	rdma_low_cc_c_table->LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_2	= cspr_low_cc_c->bd2;
	rdma_low_cc_c_table->LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_3	= cspr_low_cc_c->bd3;
	rdma_low_cc_c_table->LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_4	= cspr_low_cc_c->bd4;
	rdma_low_cc_c_table->LCSSCLBD.LCSSCLBD2.bit.LCSSCLBD_5	= cspr_low_cc_c->bd5;
	rdma_low_cc_c_table->LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_0	= cspr_low_cc_c->of0;
	rdma_low_cc_c_table->LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_1	= cspr_low_cc_c->of1;
	rdma_low_cc_c_table->LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_2	= cspr_low_cc_c->of2;
	rdma_low_cc_c_table->LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_3	= cspr_low_cc_c->of3;
	rdma_low_cc_c_table->LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_4	= cspr_low_cc_c->of4;
	rdma_low_cc_c_table->LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_5	= cspr_low_cc_c->of5;
	// write signed data
	im_cnr_set_reg_signed( rdma_low_cc_c_table->LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_0, cspr_low_cc_c->gain0 );
	im_cnr_set_reg_signed( rdma_low_cc_c_table->LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_1, cspr_low_cc_c->gain1 );
	im_cnr_set_reg_signed( rdma_low_cc_c_table->LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_2, cspr_low_cc_c->gain2 );
	im_cnr_set_reg_signed( rdma_low_cc_c_table->LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_3, cspr_low_cc_c->gain3 );
	im_cnr_set_reg_signed( rdma_low_cc_c_table->LCSSCLGA.LCSSCLGA2, union io_cnr_cspr_reg_lcssclga_2, LCSSCLGA_4, cspr_low_cc_c->gain4 );
	im_cnr_set_reg_signed( rdma_low_cc_c_table->LCSSCLGA.LCSSCLGA2, union io_cnr_cspr_reg_lcssclga_2, LCSSCLGA_5, cspr_low_cc_c->gain5 );
}

/*
Convert T_IM_CNR_TABLE to T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL
*/
static VOID im_cnr_convert_rdma_cspr_edge_table( const T_IM_CNR_TABLE* cspr_edge, T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL* rdma_edge_table )
{
	rdma_edge_table->CESCLBD.CESCLBD1.bit.CESCLBD_1	= cspr_edge->bd1;
	rdma_edge_table->CESCLBD.CESCLBD1.bit.CESCLBD_2	= cspr_edge->bd2;
	rdma_edge_table->CESCLBD.CESCLBD1.bit.CESCLBD_3	= cspr_edge->bd3;
	rdma_edge_table->CESCLBD.CESCLBD1.bit.CESCLBD_4	= cspr_edge->bd4;
	rdma_edge_table->CESCLBD.CESCLBD2.bit.CESCLBD_5	= cspr_edge->bd5;
	rdma_edge_table->CESCLOF.CESCLOF1.bit.CESCLOF_0	= cspr_edge->of0;
	rdma_edge_table->CESCLOF.CESCLOF1.bit.CESCLOF_1	= cspr_edge->of1;
	rdma_edge_table->CESCLOF.CESCLOF1.bit.CESCLOF_2	= cspr_edge->of2;
	rdma_edge_table->CESCLOF.CESCLOF1.bit.CESCLOF_3	= cspr_edge->of3;
	rdma_edge_table->CESCLOF.CESCLOF2.bit.CESCLOF_4	= cspr_edge->of4;
	rdma_edge_table->CESCLOF.CESCLOF2.bit.CESCLOF_5	= cspr_edge->of5;
	// write signed data
	im_cnr_set_reg_signed( rdma_edge_table->CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_0, cspr_edge->gain0 );
	im_cnr_set_reg_signed( rdma_edge_table->CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_1, cspr_edge->gain1 );
	im_cnr_set_reg_signed( rdma_edge_table->CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_2, cspr_edge->gain2 );
	im_cnr_set_reg_signed( rdma_edge_table->CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_3, cspr_edge->gain3 );
	im_cnr_set_reg_signed( rdma_edge_table->CESCLGA.CESCLGA2, union io_cnr_cspr_reg_cesclga_2, CESCLGA_4, cspr_edge->gain4 );
	im_cnr_set_reg_signed( rdma_edge_table->CESCLGA.CESCLGA2, union io_cnr_cspr_reg_cesclga_2, CESCLGA_5, cspr_edge->gain5 );
}

/*
Convert T_IM_CNR_HUE_SPEC_TABLE to T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL
*/
static VOID im_cnr_convert_rdma_cspr_hue_table( const T_IM_CNR_HUE_SPEC_TABLE* cspr_hue, T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL* rdma_hue_table )
{
	// write signed data
	im_cnr_set_reg_signed( rdma_hue_table->CPHPT, union io_cnr_cspr_reg_cphpt, CDCB, cspr_hue->cd_cb );
	im_cnr_set_reg_signed( rdma_hue_table->CPHPT, union io_cnr_cspr_reg_cphpt, CDCR, cspr_hue->cd_cr );

	rdma_hue_table->CPHPTK.bit.KCB	= cspr_hue->k_cb;
	rdma_hue_table->CPHPTK.bit.KCR	= cspr_hue->k_cr;

	rdma_hue_table->CDSCLBD.CDSCLBD1.bit.CDSCLBD_1	= cspr_hue->bd1;
	rdma_hue_table->CDSCLBD.CDSCLBD1.bit.CDSCLBD_2	= cspr_hue->bd2;
	rdma_hue_table->CDSCLBD.CDSCLBD2.bit.CDSCLBD_3	= cspr_hue->bd3;
	rdma_hue_table->CDSCLBD.CDSCLBD2.bit.CDSCLBD_4	= cspr_hue->bd4;
	rdma_hue_table->CDSCLBD.CDSCLBD3.bit.CDSCLBD_5	= cspr_hue->bd5;
	rdma_hue_table->CDSCLOF.CDSCLOF1.bit.CDSCLOF_0	= cspr_hue->of0;
	rdma_hue_table->CDSCLOF.CDSCLOF1.bit.CDSCLOF_1	= cspr_hue->of1;
	rdma_hue_table->CDSCLOF.CDSCLOF2.bit.CDSCLOF_2	= cspr_hue->of2;
	rdma_hue_table->CDSCLOF.CDSCLOF2.bit.CDSCLOF_3	= cspr_hue->of3;
	rdma_hue_table->CDSCLOF.CDSCLOF3.bit.CDSCLOF_4	= cspr_hue->of4;
	rdma_hue_table->CDSCLOF.CDSCLOF3.bit.CDSCLOF_5	= cspr_hue->of5;
	// write signed data
	im_cnr_set_reg_signed( rdma_hue_table->CDSCLGA.CDSCLGA1, union io_cnr_cspr_reg_cdsclga_1, CDSCLGA_0, cspr_hue->gain0 );
	im_cnr_set_reg_signed( rdma_hue_table->CDSCLGA.CDSCLGA1, union io_cnr_cspr_reg_cdsclga_1, CDSCLGA_1, cspr_hue->gain1 );
	im_cnr_set_reg_signed( rdma_hue_table->CDSCLGA.CDSCLGA2, union io_cnr_cspr_reg_cdsclga_2, CDSCLGA_2, cspr_hue->gain2 );
	im_cnr_set_reg_signed( rdma_hue_table->CDSCLGA.CDSCLGA2, union io_cnr_cspr_reg_cdsclga_2, CDSCLGA_3, cspr_hue->gain3 );
	im_cnr_set_reg_signed( rdma_hue_table->CDSCLGA.CDSCLGA3, union io_cnr_cspr_reg_cdsclga_3, CDSCLGA_4, cspr_hue->gain4 );
	im_cnr_set_reg_signed( rdma_hue_table->CDSCLGA.CDSCLGA3, union io_cnr_cspr_reg_cdsclga_3, CDSCLGA_5, cspr_hue->gain5 );
}

/*
Convert T_IM_CNR_CTRL_CC_SRC_DST to T_IM_CNR_RDMA_DATA_SPRS_CC_TBL
*/
static VOID im_cnr_convert_rdma_cspr_sprs_cc( const T_IM_CNR_CTRL_CC_SRC_DST* sprs_ctrl, T_IM_CNR_RDMA_DATA_SPRS_CC_TBL* rdma_sprs_ctrl )
{
	rdma_sprs_ctrl->SPRS.bit.SPRS = sprs_ctrl->enable;
	// write signed data
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_1, sprs_ctrl->cb_table.bd1 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_2, sprs_ctrl->cb_table.bd2 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_3, sprs_ctrl->cb_table.bd3 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_4, sprs_ctrl->cb_table.bd4 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBBD.SPRSCBBD2, union io_cnr_cspr_reg_sprscbbd_2, SPRSCBBD_5, sprs_ctrl->cb_table.bd5 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_0, sprs_ctrl->cb_table.of0 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_1, sprs_ctrl->cb_table.of1 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_2, sprs_ctrl->cb_table.of2 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_3, sprs_ctrl->cb_table.of3 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBOF.SPRSCBOF2, union io_cnr_cspr_reg_sprscbof_2, SPRSCBOF_4, sprs_ctrl->cb_table.of4 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBOF.SPRSCBOF2, union io_cnr_cspr_reg_sprscbof_2, SPRSCBOF_5, sprs_ctrl->cb_table.of5 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBGA.SPRSCBGA1, union io_cnr_cspr_reg_sprscbga_1, SPRSCBGA_0, sprs_ctrl->cb_table.gain0 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBGA.SPRSCBGA1, union io_cnr_cspr_reg_sprscbga_1, SPRSCBGA_1, sprs_ctrl->cb_table.gain1 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBGA.SPRSCBGA2, union io_cnr_cspr_reg_sprscbga_2, SPRSCBGA_2, sprs_ctrl->cb_table.gain2 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBGA.SPRSCBGA2, union io_cnr_cspr_reg_sprscbga_2, SPRSCBGA_3, sprs_ctrl->cb_table.gain3 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBGA.SPRSCBGA3, union io_cnr_cspr_reg_sprscbga_3, SPRSCBGA_4, sprs_ctrl->cb_table.gain4 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCBGA.SPRSCBGA3, union io_cnr_cspr_reg_sprscbga_3, SPRSCBGA_5, sprs_ctrl->cb_table.gain5 );

	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_1, sprs_ctrl->cr_table.bd1 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_2, sprs_ctrl->cr_table.bd2 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_3, sprs_ctrl->cr_table.bd3 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_4, sprs_ctrl->cr_table.bd4 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRBD.SPRSCRBD2, union io_cnr_cspr_reg_sprscrbd_2, SPRSCRBD_5, sprs_ctrl->cr_table.bd5 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_0, sprs_ctrl->cr_table.of0 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_1, sprs_ctrl->cr_table.of1 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_2, sprs_ctrl->cr_table.of2 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_3, sprs_ctrl->cr_table.of3 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCROF.SPRSCROF2, union io_cnr_cspr_reg_sprscrof_2, SPRSCROF_4, sprs_ctrl->cr_table.of4 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCROF.SPRSCROF2, union io_cnr_cspr_reg_sprscrof_2, SPRSCROF_5, sprs_ctrl->cr_table.of5 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRGA.SPRSCRGA1, union io_cnr_cspr_reg_sprscrga_1, SPRSCRGA_0, sprs_ctrl->cr_table.gain0 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRGA.SPRSCRGA1, union io_cnr_cspr_reg_sprscrga_1, SPRSCRGA_1, sprs_ctrl->cr_table.gain1 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRGA.SPRSCRGA2, union io_cnr_cspr_reg_sprscrga_2, SPRSCRGA_2, sprs_ctrl->cr_table.gain2 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRGA.SPRSCRGA2, union io_cnr_cspr_reg_sprscrga_2, SPRSCRGA_3, sprs_ctrl->cr_table.gain3 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRGA.SPRSCRGA3, union io_cnr_cspr_reg_sprscrga_3, SPRSCRGA_4, sprs_ctrl->cr_table.gain4 );
	im_cnr_set_reg_signed( rdma_sprs_ctrl->SPRSCRGA.SPRSCRGA3, union io_cnr_cspr_reg_sprscrga_3, SPRSCRGA_5, sprs_ctrl->cr_table.gain5 );
}

/*
Convert T_IM_CNR_CTRL_CC_SRC_DST to T_IM_CNR_RDMA_DATA_SPRD_CC_TBL
*/
static VOID im_cnr_convert_rdma_cspr_sprd_cc( const T_IM_CNR_CTRL_CC_SRC_DST* sprd_ctrl, T_IM_CNR_RDMA_DATA_SPRD_CC_TBL* rdma_sprd_ctrl )
{
	rdma_sprd_ctrl->SPRD.bit.SPRD = sprd_ctrl->enable;
	// write signed data
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_1, sprd_ctrl->cb_table.bd1 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_2, sprd_ctrl->cb_table.bd2 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_3, sprd_ctrl->cb_table.bd3 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_4, sprd_ctrl->cb_table.bd4 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBBD.SPRDCBBD2, union io_cnr_cspr_reg_sprdcbbd_2, SPRDCBBD_5, sprd_ctrl->cb_table.bd5 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_0, sprd_ctrl->cb_table.of0 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_1, sprd_ctrl->cb_table.of1 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_2, sprd_ctrl->cb_table.of2 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_3, sprd_ctrl->cb_table.of3 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBOF.SPRDCBOF2, union io_cnr_cspr_reg_sprdcbof_2, SPRDCBOF_4, sprd_ctrl->cb_table.of4 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBOF.SPRDCBOF2, union io_cnr_cspr_reg_sprdcbof_2, SPRDCBOF_5, sprd_ctrl->cb_table.of5 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBGA.SPRDCBGA1, union io_cnr_cspr_reg_sprdcbga_1, SPRDCBGA_0, sprd_ctrl->cb_table.gain0 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBGA.SPRDCBGA1, union io_cnr_cspr_reg_sprdcbga_1, SPRDCBGA_1, sprd_ctrl->cb_table.gain1 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBGA.SPRDCBGA2, union io_cnr_cspr_reg_sprdcbga_2, SPRDCBGA_2, sprd_ctrl->cb_table.gain2 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBGA.SPRDCBGA2, union io_cnr_cspr_reg_sprdcbga_2, SPRDCBGA_3, sprd_ctrl->cb_table.gain3 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBGA.SPRDCBGA3, union io_cnr_cspr_reg_sprdcbga_3, SPRDCBGA_4, sprd_ctrl->cb_table.gain4 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCBGA.SPRDCBGA3, union io_cnr_cspr_reg_sprdcbga_3, SPRDCBGA_5, sprd_ctrl->cb_table.gain5 );

	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_1, sprd_ctrl->cr_table.bd1 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_2, sprd_ctrl->cr_table.bd2 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_3, sprd_ctrl->cr_table.bd3 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_4, sprd_ctrl->cr_table.bd4 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRBD.SPRDCRBD2, union io_cnr_cspr_reg_sprdcrbd_2, SPRDCRBD_5, sprd_ctrl->cr_table.bd5 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_0, sprd_ctrl->cr_table.of0 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_1, sprd_ctrl->cr_table.of1 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_2, sprd_ctrl->cr_table.of2 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_3, sprd_ctrl->cr_table.of3 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCROF.SPRDCROF2, union io_cnr_cspr_reg_sprdcrof_2, SPRDCROF_4, sprd_ctrl->cr_table.of4 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCROF.SPRDCROF2, union io_cnr_cspr_reg_sprdcrof_2, SPRDCROF_5, sprd_ctrl->cr_table.of5 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRGA.SPRDCRGA1, union io_cnr_cspr_reg_sprdcrga_1, SPRDCRGA_0, sprd_ctrl->cr_table.gain0 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRGA.SPRDCRGA1, union io_cnr_cspr_reg_sprdcrga_1, SPRDCRGA_1, sprd_ctrl->cr_table.gain1 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRGA.SPRDCRGA2, union io_cnr_cspr_reg_sprdcrga_2, SPRDCRGA_2, sprd_ctrl->cr_table.gain2 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRGA.SPRDCRGA2, union io_cnr_cspr_reg_sprdcrga_2, SPRDCRGA_3, sprd_ctrl->cr_table.gain3 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRGA.SPRDCRGA3, union io_cnr_cspr_reg_sprdcrga_3, SPRDCRGA_4, sprd_ctrl->cr_table.gain4 );
	im_cnr_set_reg_signed( rdma_sprd_ctrl->SPRDCRGA.SPRDCRGA3, union io_cnr_cspr_reg_sprdcrga_3, SPRDCRGA_5, sprd_ctrl->cr_table.gain5 );
}
#endif


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Start CNR PCLK
VOID Im_CNR_On_Pclk( UCHAR ch )
{
#ifdef CO_ACT_CNR_PCLK
	INT32 intkey;

	intkey = Dd_ARM_DI();

	if (ch == D_IM_CNR_CH_0) {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Pclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), ~D_DD_TOP_CNR1AP_BIT );
	}
	else if (ch == D_IM_CNR_CH_1) {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Pclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_CNR2AP_BIT );
	}
	else {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Pclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), ~D_DD_TOP_CNR1AP_BIT );
		Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Pclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_CNR2AP_BIT );
	}

	Dd_ARM_EI( intkey );
#endif //CO_ACT_CNR_PCLK
}

// Stop CNR PCLK
VOID Im_CNR_Off_Pclk( UCHAR ch )
{
#ifdef CO_ACT_CNR_PCLK
	INT32 intkey;

	intkey = Dd_ARM_DI();

	if (ch == D_IM_CNR_CH_0) {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Pclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), D_DD_TOP_CNR1AP_BIT );
	}
	else if (ch == D_IM_CNR_CH_1) {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Pclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), D_DD_TOP_CNR2AP_BIT );
	}
	else {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Pclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), D_DD_TOP_CNR1AP_BIT );
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_CNR_Pclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), D_DD_TOP_CNR2AP_BIT );
	}

	Dd_ARM_EI( intkey );
#endif //CO_ACT_CNR_PCLK
}

/*
The semaphore of CNR is acquired for Off Line
*/
INT32 Im_CNR_OFL_Open( UCHAR ch, const INT32 tmout )
{
	DDIM_USER_ID sid;
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Open() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ) {
		Ddim_Assertion(("I:Im_CNR_OFL_Open. input param error. tmout = %x\n", tmout));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if (ch == D_IM_CNR_CH_2) {
		for( UCHAR loop = D_IM_CNR_CH_0; loop <= D_IM_CNR_CH_1; loop++ ){
			sid = SID_IM_CNR_OFL(loop);
			if( D_DDIM_USER_SEM_WAIT_POL == tmout ) {
				ercd = DDIM_User_Pol_Sem( sid );				// pol_sem()
			}
			else{
				ercd = DDIM_User_Twai_Sem( sid, (DDIM_USER_TMO)tmout );	// twai_sem()
			}

			if( D_DDIM_USER_E_TMOUT == ercd ) {
				// A semaphore acquisition processing time out
				Ddim_Print(("I:Im_CNR_OFL_Open() Error : Semaphore Get Time Out\n"));
				return D_IM_CNR_SEM_TIMEOUT_ERR;
			}

			if( D_DDIM_USER_E_OK != ercd ) {
				// CNR processing error
				// CNR processing error
				Ddim_Print(("I:Im_CNR_OFL_Open() Error : Semaphore Get Error\n"));
				return D_IM_CNR_SEM_NG;
			}
		}
	}
	else{
		sid = SID_IM_CNR_OFL(ch);
		if( D_DDIM_USER_SEM_WAIT_POL == tmout ) {
			ercd = DDIM_User_Pol_Sem( sid );				// pol_sem()
		}
		else{
			ercd = DDIM_User_Twai_Sem( sid, (DDIM_USER_TMO)tmout );	// twai_sem()
		}

		if( D_DDIM_USER_E_TMOUT == ercd ) {
			// A semaphore acquisition processing time out
			Ddim_Print(("I:Im_CNR_OFL_Open() Error : Semaphore Get Time Out\n"));
			return D_IM_CNR_SEM_TIMEOUT_ERR;
		}

		if( D_DDIM_USER_E_OK != ercd ) {
			// CNR processing error
			// CNR processing error
			Ddim_Print(("I:Im_CNR_OFL_Open() Error : Semaphore Get Error\n"));
			return D_IM_CNR_SEM_NG;
		}
	}

	return D_DDIM_OK;
}

/*
The semaphore of CNR is acquired for On The Fly
*/
INT32 Im_CNR_OTF_Open( UCHAR ch, const INT32 tmout )
{
	DDIM_USER_ID sid;
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Open() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ) {
		Ddim_Assertion(("I:Im_CNR_OTF_Open. input param error. tmout = %x\n", tmout));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if (ch == D_IM_CNR_CH_2) {
		for( UCHAR loop = D_IM_CNR_CH_0; loop <= D_IM_CNR_CH_1; loop++ ){
			sid = SID_IM_CNR_OTF(loop);
			if( D_DDIM_USER_SEM_WAIT_POL == tmout ) {
				ercd = DDIM_User_Pol_Sem( sid );				// pol_sem()
			}
			else{
				ercd = DDIM_User_Twai_Sem( sid, (DDIM_USER_TMO)tmout );	// twai_sem()
			}

			if( D_DDIM_USER_E_TMOUT == ercd ) {
				// A semaphore acquisition processing time out
				Ddim_Print(("I:Im_CNR_OTF_Open() Error : Semaphore Get Time Out\n"));
				return D_IM_CNR_SEM_TIMEOUT_ERR;
			}

			if( D_DDIM_USER_E_OK != ercd ) {
				// CNR processing error
				// CNR processing error
				Ddim_Print(("I:Im_CNR_OTF_Open() Error : Semaphore Get Error\n"));
				return D_IM_CNR_SEM_NG;
			}
		}
	}
	else{
		sid = SID_IM_CNR_OTF(ch);
		if( D_DDIM_USER_SEM_WAIT_POL == tmout ) {
			ercd = DDIM_User_Pol_Sem( sid );				// pol_sem()
		}
		else{
			ercd = DDIM_User_Twai_Sem( sid, (DDIM_USER_TMO)tmout );	// twai_sem()
		}

		if( D_DDIM_USER_E_TMOUT == ercd ) {
			// A semaphore acquisition processing time out
			Ddim_Print(("I:Im_CNR_OTF_Open() Error : Semaphore Get Time Out\n"));
			return D_IM_CNR_SEM_TIMEOUT_ERR;
		}

		if( D_DDIM_USER_E_OK != ercd ) {
			// CNR processing error
			// CNR processing error
			Ddim_Print(("I:Im_CNR_OTF_Open() Error : Semaphore Get Error\n"));
			return D_IM_CNR_SEM_NG;
		}
	}

	return D_DDIM_OK;
}

/*
Set software reset and operating mode for Off Line
*/
INT32 Im_CNR_OFL_Init( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Init() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	im_CNR_on_spr_clk(ch);
	Im_CNR_On_Pclk(ch);

	if (ch == D_IM_CNR_CH_0) {
		IO_CNR(ch)->OFL_REG_RW.SPRSR.bit.SR = 1;	// Software reset assert

		Im_CNR_Dsb();

		// CNRCLK        = MAX 252.0MHz		MIN 31.5MHz
		// APB clock     = MAX  66.7MHz		MIN 20.8MHz
		// -->   ( (1/CNRCLK) / (1/APB clock) ) * 2 + 1
		//     = ( (1/31.5)   / (1/66.7)        ) * 2 + 1
		//     = 5.23 => 6 APB cycle

		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Im_CNR_Dsb();

		IO_CNR(ch)->OFL_REG_RW.SPRSR.bit.SR = 0;	// Software reset de-assert

		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Im_CNR_Dsb();
	}
	else if (ch == D_IM_CNR_CH_1) {
		IO_CNR(ch)->OFL_REG_RW.SPRSR.bit.SR = 1;	// Software reset assert

		Im_CNR_Dsb();

		// CNRCLK        = MAX 252.0MHz		MIN 31.5MHz
		// APB clock     = MAX  66.7MHz		MIN 20.8MHz
		// -->   ( (1/CNRCLK) / (1/APB clock) ) * 2 + 1
		//     = ( (1/31.5)   / (1/66.7)        ) * 2 + 1
		//     = 5.23 => 6 APB cycle

		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Im_CNR_Dsb();

		IO_CNR(ch)->OFL_REG_RW.SPRSR.bit.SR = 0;	// Software reset de-assert

		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Im_CNR_Dsb();
	}
	else {
		IO_CNR(ch)->OFL_REG_RW.SPRSR.bit.SR = 1;	// Software reset assert

		Im_CNR_Dsb();

		// CNRCLK        = MAX 252.0MHz		MIN 31.5MHz
		// APB clock     = MAX  66.7MHz		MIN 20.8MHz
		// -->   ( (1/CNRCLK) / (1/APB clock) ) * 2 + 1
		//     = ( (1/31.5)   / (1/66.7)        ) * 2 + 1
		//     = 5.23 => 6 APB cycle

		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Im_CNR_Dsb();

		IO_CNR(ch)->OFL_REG_RW.SPRSR.bit.SR = 0;	// Software reset de-assert

		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Im_CNR_Dsb();
	}

	im_cnr_ofl_clear_spdf(ch);	// Interrupt clear
	im_cnr_ofl_clear_axif(ch);	// Interrupt clear
	im_cnr_ofl_set_spde( ch, 1 );

	Im_CNR_Off_Pclk(ch);
	im_CNR_off_spr_clk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set software reset and operating mode for Off Line
*/
INT32 Im_CNR_OTF_Init( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Init() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	im_CNR_on_spr_clk(ch);
	Im_CNR_On_Pclk(ch);

	if (ch == D_IM_CNR_CH_0) {
		IO_CNR(ch)->OTF_REG_RW.SPRSR.bit.SR = 1;	// Software reset assert

		Im_CNR_Dsb();

		// CNRCLK        = MAX 252.0MHz		MIN 31.5MHz
		// APB clock     = MAX  66.7MHz		MIN 20.8MHz
		// -->   ( (1/CNRCLK) / (1/APB clock) ) * 2 + 1
		//     = ( (1/31.5)   / (1/66.7)        ) * 2 + 1
		//     = 5.23 => 6 APB cycle

		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Im_CNR_Dsb();

		IO_CNR(ch)->OTF_REG_RW.SPRSR.bit.SR = 0;	// Software reset de-assert

		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Im_CNR_Dsb();
	}
	else if (ch == D_IM_CNR_CH_1) {
		IO_CNR(ch)->OTF_REG_RW.SPRSR.bit.SR = 1;	// Software reset assert

		Im_CNR_Dsb();

		// CNRCLK        = MAX 252.0MHz		MIN 31.5MHz
		// APB clock     = MAX  66.7MHz		MIN 20.8MHz
		// -->   ( (1/CNRCLK) / (1/APB clock) ) * 2 + 1
		//     = ( (1/31.5)   / (1/66.7)        ) * 2 + 1
		//     = 5.23 => 6 APB cycle

		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Im_CNR_Dsb();

		IO_CNR(ch)->OTF_REG_RW.SPRSR.bit.SR = 0;	// Software reset de-assert

		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Im_CNR_Dsb();
	}
	else {
		IO_CNR(ch)->OTF_REG_RW.SPRSR.bit.SR = 1;	// Software reset assert

		Im_CNR_Dsb();

		// CNRCLK        = MAX 252.0MHz		MIN 31.5MHz
		// APB clock     = MAX  66.7MHz		MIN 20.8MHz
		// -->   ( (1/CNRCLK) / (1/APB clock) ) * 2 + 1
		//     = ( (1/31.5)   / (1/66.7)        ) * 2 + 1
		//     = 5.23 => 6 APB cycle

		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Im_CNR_Dsb();

		IO_CNR(ch)->OTF_REG_RW.SPRSR.bit.SR = 0;	// Software reset de-assert

		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP11();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Dd_Top_Get_CLKSTOP13();	/* pgr0695 */
		Im_CNR_Dsb();
	}

	im_cnr_otf_clear_spdf(ch);	// Interrupt clear
	im_cnr_otf_clear_axif(ch);	// Interrupt clear
	im_cnr_otf_clear_drtf(ch);	// Interrupt clear
	im_cnr_otf_set_spde( ch, 1 );

	Im_CNR_Off_Pclk(ch);
	im_CNR_off_spr_clk(ch);
	Im_CNR_Dsb();

	// Initialize of Address management information
	if( ch == D_IM_CNR_CH_2 ){
		memset( (VOID*)&gIM_CNR_Output_Mng[D_IM_CNR_CH_0], 0, sizeof(T_IM_CNR_OUTPUT_MNG) * 2 );
		memset( (VOID*)&gIM_CNR_Latest_Mng[D_IM_CNR_CH_0], 0, sizeof(T_IM_CNR_LATEST_MNG) * 2 );
	}
	else{
		memset( (VOID*)&gIM_CNR_Output_Mng[ch], 0, sizeof(T_IM_CNR_OUTPUT_MNG) );
		memset( (VOID*)&gIM_CNR_Latest_Mng[ch], 0, sizeof(T_IM_CNR_LATEST_MNG) );
	}

	return D_DDIM_OK;
}

/*
CNR(OFL) Control
*/
INT32 Im_CNR_OFL_Ctrl( UCHAR ch, const T_IM_CNR_OFL_CTRL* const spr_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Ctrl() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Ctrl() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif


	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	// Read Setting
	//      |---RYDEF---|       |---RCDEF---|       |---RCDEF---|
	//  RYTA|-HSIZE-|   |  RCBTA|-HSIZE-|   |  RCRTA|-HSIZE-|   |
	// -----+-------+---+  -----+-------+---+  -----+-------+---+
	//   |  |       | | |    |  |       | | |    |  |       | | |
	//   V  |       | d |    V  |       | d |    V  |       | d |
	//   S  |Y data | u |    S  |CB data| u |    S  |CR data| u |
	//   I  |valid  | m |    I  |valid  | m |    I  |valid  | m |
	//   Z  |area   | m |    Z  |area   | m |    Z  |area   | m |
	//   E  |       | y |    E  |       | y |    E  |       | y |
	//   |  |       | | |    |  |       | | |    |  |       | | |
	// -----+-------+---+  -----+-------+---+  -----+-------+---+

	// Write Setting
	//      |---WYDEF---|       |---WCDEF---|       |---WCDEF---|
	//  WYTA|-HSIZE-|   |  WCBTA|-HSIZE-|   |  WCRTA|-HSIZE-|   |
	// -----+-------+---+  -----+-------+---+  -----+-------+---+
	//   |  |       | | |    |  |       | | |    |  |       | | |
	//   V  |       | d |    V  |       | d |    V  |       | d |
	//   S  |Y data | u |    S  |CB data| u |    S  |CR data| u |
	//   I  |valid  | m |    I  |valid  | m |    I  |valid  | m |
	//   Z  |area   | m |    Z  |area   | m |    Z  |area   | m |
	//   E  |       | y |    E  |       | y |    E  |       | y |
	//   |  |       | | |    |  |       | | |    |  |       | | |
	// -----+-------+---+  -----+-------+---+  -----+-------+---+

	IO_CNR(ch)->OFL_REG_RW.SPRMOD.bit.YUV_MODE	= spr_ctrl->mode;	// SPR YCC mode		@YUV_MODE

	IO_CNR(ch)->OFL_REG_RW.HSIZE.bit.HSIZE	= spr_ctrl->width;		// SPR valid data width		@HSIZE
	IO_CNR(ch)->OFL_REG_RW.VSIZE.bit.VSIZE	= spr_ctrl->lines;		// SPR valid data lines		@VSIZE

	IO_CNR(ch)->OFL_REG_RW.RYDEF.bit.RYDEF	= spr_ctrl->r_y_width;	// Y width for read			@RYDEF
	IO_CNR(ch)->OFL_REG_RW.RYTA.bit.RYTA	= spr_ctrl->r_y_addr;	// Y address for read		@RYTA

	IO_CNR(ch)->OFL_REG_RW.RCDEF.bit.RCDEF	= spr_ctrl->r_c_width;	// CbCr width for read		@RCDEF
	IO_CNR(ch)->OFL_REG_RW.RCBTA.bit.RCBTA	= spr_ctrl->r_cb_addr;	// Cb address for read		@RCBTA
	IO_CNR(ch)->OFL_REG_RW.RCRTA.bit.RCRTA	= spr_ctrl->r_cr_addr;	// Cr address for read		@RCRTA

	IO_CNR(ch)->OFL_REG_RW.WYDEF.bit.WYDEF	= spr_ctrl->w_y_width;	// Y width for write		@WYDEF
	IO_CNR(ch)->OFL_REG_RW.WYTA.bit.WYTA	= spr_ctrl->w_y_addr;	// Y address for write		@WYTA

	IO_CNR(ch)->OFL_REG_RW.WCDEF.bit.WCDEF	= spr_ctrl->w_c_width;	// CbCr width for write		@WCDEF
	IO_CNR(ch)->OFL_REG_RW.WCBTA.bit.WCBTA	= spr_ctrl->w_cb_addr;	// Cb address for write		@WCBTA
	IO_CNR(ch)->OFL_REG_RW.WCRTA.bit.WCRTA	= spr_ctrl->w_cr_addr;	// Cr address for write		@WCRTA

	IO_CNR(ch)->OFL_REG_RW.TMPTA.bit.TMPTA	= spr_ctrl->work_addr;	// Address fir work area	@TMPTA

	IO_CNR(ch)->OFL_REG_RW.VDIV.bit.VDIVT	= spr_ctrl->divide_top;		// Virtucal process devide	@VDIVT
	IO_CNR(ch)->OFL_REG_RW.VDIV.bit.VDIVB	= spr_ctrl->divide_bot;		// Virtucal process devide	@VDIVB

	if( ch == D_IM_CNR_CH_2 ){
		gIM_CNR_OFL_CallBack_Func[D_IM_CNR_CH_0]	= spr_ctrl->pCallBack;
		gIM_CNR_OFL_CallBack_Func[D_IM_CNR_CH_1]	= spr_ctrl->pCallBack;
		gIM_CNR_OFL_UserParam[D_IM_CNR_CH_0]		= spr_ctrl->user_param;
		gIM_CNR_OFL_UserParam[D_IM_CNR_CH_1]		= spr_ctrl->user_param;
	}
	else{
		gIM_CNR_OFL_CallBack_Func[ch]	= spr_ctrl->pCallBack;
		gIM_CNR_OFL_UserParam[ch]		= spr_ctrl->user_param;
	}

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
CNR(OTF) Control
*/
INT32 Im_CNR_OTF_Ctrl( UCHAR ch, const T_IM_CNR_OTF_CTRL* const spr_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Ctrl() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Ctrl() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	if (spr_ctrl->width < 96 || spr_ctrl->width > 2360) {
		Ddim_Assertion(("Im_CNR_OTF_Ctrl() error. input width error : %d\n", spr_ctrl->width));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if (spr_ctrl->lines < 72 || spr_ctrl->lines > 4096) {
		Ddim_Assertion(("Im_CNR_OTF_Ctrl() error. input lines error : %d\n", spr_ctrl->lines));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if ( (spr_ctrl->w_offset_x != 0)&&(spr_ctrl->w_offset_x < 96 || spr_ctrl->w_offset_x > 2360)) {
		Ddim_Assertion(("Im_CNR_OTF_Ctrl() error. input w_offset_x error : %lu\n", spr_ctrl->w_offset_x));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if (spr_ctrl->video_format == 1){
		if (spr_ctrl->w_c_width % 128){
			Ddim_Assertion(("Im_CNR_OTF_Ctrl() error. input w_c_width error : %lu\n", spr_ctrl->w_c_width));
			return D_IM_CNR_INPUT_PARAM_ERROR;
		}
		if( spr_ctrl->mode != E_IM_CNR_OTF_YCC_MODE_YCC420_SIMPLE ){
			Ddim_Assertion(("Im_CNR_OTF_Ctrl() error. mode error : %d\n", spr_ctrl->mode));
			return D_IM_CNR_INPUT_PARAM_ERROR;
		}
	}


	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	// Write Setting
	//      |---WCDEF---|
	//  WCTA|-HSIZE-|   |
	// -----+-------+---+
	//   |  |       | | |
	//   V  |       | d |
	//   S  |C data | u |
	//   I  |valid  | m |
	//   Z  |area   | m |
	//   E  |       | y |
	//   |  |       | | |
	// -----+-------+---+

	IO_CNR(ch)->OTF_REG_RW.SPRMOD.bit.YUV_MODE		= spr_ctrl->mode;			// SPR YCC mode				@YUV_MODE
	IO_CNR(ch)->OTF_REG_RW.SPRMOD.bit.EXMODE		= spr_ctrl->extra_mode;		// SPR extra mode			@EXMODE
	IO_CNR(ch)->OTF_REG_RW.SPRMOD.bit.CNRVFM		= spr_ctrl->video_format;	// SPR video format			@CNRVFM

	IO_CNR(ch)->OTF_REG_RW.HSIZE.bit.HSIZE			= spr_ctrl->width;			// SPR valid data width		@HSIZE
	IO_CNR(ch)->OTF_REG_RW.VSIZE.bit.VSIZE			= spr_ctrl->lines;			// SPR valid data lines		@VSIZE

	IO_CNR(ch)->OTF_REG_RW.WCDEF.bit.WCDEF			= spr_ctrl->w_c_width;		// CbCr width for write		@WCDEF

	IO_CNR(ch)->OTF_REG_RW.CNRWOFSX.bit.CNRWOFSX	= spr_ctrl->w_offset_x;		// write offset X direction	@CNRWOFSX

	IO_CNR(ch)->OTF_REG_RW.HDIV.bit.HDIVL			= spr_ctrl->divide_left;	// Virtucal process devide	@HDIVL
	IO_CNR(ch)->OTF_REG_RW.HDIV.bit.HDIVR			= spr_ctrl->divide_right;	// Virtucal process devide	@VDIVR

	IO_CNR(ch)->OTF_REG_RW.SPRTRG.bit.CNRCSE		= spr_ctrl->continuous;		// Continuous processing	@CNRCSE

	if( ch == D_IM_CNR_CH_2 ){
		gIM_CNR_OTF_CallBack_Func[D_IM_CNR_CH_0]	= spr_ctrl->pCallBack;
		gIM_CNR_OTF_CallBack_Func[D_IM_CNR_CH_1]	= spr_ctrl->pCallBack;
		gIM_CNR_OTF_UserParam[D_IM_CNR_CH_0]		= spr_ctrl->user_param;
		gIM_CNR_OTF_UserParam[D_IM_CNR_CH_1]		= spr_ctrl->user_param;
	}
	else{
		gIM_CNR_OTF_CallBack_Func[ch]				= spr_ctrl->pCallBack;
		gIM_CNR_OTF_UserParam[ch]					= spr_ctrl->user_param;
	}

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
CNR(OTF) Set output address information
*/
INT32 Im_CNR_OTF_Set_OutAddr_Info( UCHAR ch, const T_IM_CNR_OTF_OUTADDR_INFO* const addr_info )
{
	INT32 out_addr_cnt;

#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_OutAddr_Info() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( addr_info == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_OutAddr_Info() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( (addr_info->use_bank_num == 0)||(addr_info->use_bank_num > D_IM_CNR_ADDR_BANK_MAX) ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_OutAddr_Info() use_bank_num parameter is error : %d\n", addr_info->use_bank_num));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	// Write Setting
	//      |---WCDEF---|
	//  WCTA|-HSIZE-|   |
	// -----+-------+---+
	//   |  |       | | |
	//   V  |       | d |
	//   S  |C data | u |
	//   I  |valid  | m |
	//   Z  |area   | m |
	//   E  |       | y |
	//   |  |       | | |
	// -----+-------+---+

	if( ch == D_IM_CNR_CH_2 ){
		// set output address information	@WCTA
		gIM_CNR_Latest_Mng[D_IM_CNR_CH_0].bank_area			= 0;	// reset
		gIM_CNR_Latest_Mng[D_IM_CNR_CH_1].bank_area			= 0;	// reset
		gIM_CNR_Latest_Mng[D_IM_CNR_CH_0].addr				= 0;	// reset
		gIM_CNR_Latest_Mng[D_IM_CNR_CH_1].addr				= 0;	// reset
		gIM_CNR_Output_Mng[D_IM_CNR_CH_0].bank_area			= 0;	// reset
		gIM_CNR_Output_Mng[D_IM_CNR_CH_1].bank_area			= 0;	// reset
		gIM_CNR_Output_Mng[D_IM_CNR_CH_0].use_bank_num		= addr_info->use_bank_num;
		gIM_CNR_Output_Mng[D_IM_CNR_CH_1].use_bank_num		= addr_info->use_bank_num;
		for( out_addr_cnt = 0; out_addr_cnt < addr_info->use_bank_num; out_addr_cnt++ ){
			gIM_CNR_Output_Mng[D_IM_CNR_CH_0].output_addr[out_addr_cnt]	= addr_info->w_c_addr[out_addr_cnt];
			gIM_CNR_Output_Mng[D_IM_CNR_CH_1].output_addr[out_addr_cnt]	= addr_info->w_c_addr[out_addr_cnt];
		}
		IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.WCTA.bit.WCTA		= addr_info->w_c_addr[0];
		IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.WCTA.bit.WCTA		= addr_info->w_c_addr[0];
	}
	else{
		// set output address information	@WCTA
		gIM_CNR_Latest_Mng[ch].bank_area		= 0;	// reset
		gIM_CNR_Latest_Mng[ch].addr				= 0;	// reset
		gIM_CNR_Output_Mng[ch].bank_area		= 0;	// reset
		gIM_CNR_Output_Mng[ch].use_bank_num		= addr_info->use_bank_num;
		for( out_addr_cnt = 0; out_addr_cnt < addr_info->use_bank_num; out_addr_cnt++ ){
			gIM_CNR_Output_Mng[ch].output_addr[out_addr_cnt]	= addr_info->w_c_addr[out_addr_cnt];
		}
		IO_CNR(ch)->OTF_REG_RW.WCTA.bit.WCTA	= addr_info->w_c_addr[0];
	}

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
CNR(OTF) increment output address index
*/
INT32 Im_CNR_OTF_Increment_OutAddr_Index( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch > D_IM_CNR_CH_1) {
		Ddim_Assertion(("Im_CNR_OTF_Increment_OutAddr_Index() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	// Update output address
	++gIM_CNR_Output_Mng[ch].bank_area;
	gIM_CNR_Output_Mng[ch].bank_area		= ADDR_BNK_LIMIT( gIM_CNR_Output_Mng[ch].bank_area, gIM_CNR_Output_Mng[ch].use_bank_num );
	IO_CNR(ch)->OTF_REG_RW.WCTA.bit.WCTA	= gIM_CNR_Output_Mng[ch].output_addr[gIM_CNR_Output_Mng[ch].bank_area];

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
CNR(OFL) Control set AXI bus I/F Control
*/
INT32 Im_CNR_OFL_Ctrl_Axi( UCHAR ch, const T_IM_CNR_OFL_AXI* const axi_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Ctrl_Axi() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( axi_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Ctrl_Axi() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.AWPROT		= axi_ctrl->write_protection_type;
	IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.ARPROT		= axi_ctrl->read_protection_type;
	IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.AWCACHE	= axi_ctrl->write_cache_type;
	IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.ARCACHE	= axi_ctrl->read_cache_type;

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
CNR(OTF) Control set AXI bus I/F Control
*/
INT32 Im_CNR_OTF_Ctrl_Axi( UCHAR ch, const T_IM_CNR_OTF_AXI* const axi_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Ctrl_Axi() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( axi_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Ctrl_Axi() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_REG_RW.SPRAXISET.bit.AWPROT		= axi_ctrl->write_protection_type;
	IO_CNR(ch)->OTF_REG_RW.SPRAXISET.bit.AWCACHE	= axi_ctrl->write_cache_type;

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Get AXI bus I/F Status
*/
INT32 Im_CNR_OFL_Get_AXI_Status( UCHAR ch, T_IM_CNR_OFL_AXI_STATUS* const sts )
{
#ifdef CO_PARAM_CHECK
	if (ch >= (D_IM_CNR_CH_NUM_MAX-1)) {
		Ddim_Assertion(("Im_CNR_OFL_Get_AXI_Status() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( sts == NULL ) {
		// sts setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Get_AXI_Status() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	sts->read_channel_response = IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.RRESP;
	sts->write_channel_response = IO_CNR(ch)->OFL_REG_RW.SPRAXISET.bit.WRESP;

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Get AXI bus I/F Status
*/
INT32 Im_CNR_OTF_Get_AXI_Status( UCHAR ch, T_IM_CNR_OTF_AXI_STATUS* const sts )
{
#ifdef CO_PARAM_CHECK
	if (ch >= (D_IM_CNR_CH_NUM_MAX-1)) {
		Ddim_Assertion(("Im_CNR_OTF_Get_AXI_Status() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( sts == NULL ) {
		// sts setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Get_AXI_Status() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	sts->write_channel_response = IO_CNR(ch)->OTF_REG_RW.SPRAXISET.bit.WRESP;

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR setting for CbCr
*/
INT32 Im_CNR_OFL_Ctrl_C( UCHAR ch, const T_IM_CNR_CTRL_C* const spr_ctrl_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Ctrl_C() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_ctrl_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Ctrl_C() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.CSPREN.bit.CSPREN = spr_ctrl_c->enable;
	IO_CNR(ch)->OFL_CSPR_REG.CGDKEN.bit.CGDKEN = spr_ctrl_c->gradation_keep_en;

	IO_CNR(ch)->OFL_CSPR_REG.MCEN.bit.MCEN = spr_ctrl_c->mid_freq_nr.enable;

	IO_CNR(ch)->OFL_CSPR_REG.MCYTHH.bit.MCYTHH_0 = spr_ctrl_c->mid_freq_nr.h_y_threshold[0];
	IO_CNR(ch)->OFL_CSPR_REG.MCYTHH.bit.MCYTHH_1 = spr_ctrl_c->mid_freq_nr.h_y_threshold[1];
	IO_CNR(ch)->OFL_CSPR_REG.MCYTHH.bit.MCYTHH_2 = spr_ctrl_c->mid_freq_nr.h_y_threshold[2];
	IO_CNR(ch)->OFL_CSPR_REG.MCYTHH.bit.MCYTHH_3 = spr_ctrl_c->mid_freq_nr.h_y_threshold[3];

	IO_CNR(ch)->OFL_CSPR_REG.MCYTHV.bit.MCYTHV_0 = spr_ctrl_c->mid_freq_nr.v_y_threshold[0];
	IO_CNR(ch)->OFL_CSPR_REG.MCYTHV.bit.MCYTHV_1 = spr_ctrl_c->mid_freq_nr.v_y_threshold[1];
	IO_CNR(ch)->OFL_CSPR_REG.MCYTHV.bit.MCYTHV_2 = spr_ctrl_c->mid_freq_nr.v_y_threshold[2];
	IO_CNR(ch)->OFL_CSPR_REG.MCYTHV.bit.MCYTHV_3 = spr_ctrl_c->mid_freq_nr.v_y_threshold[3];

	IO_CNR(ch)->OFL_CSPR_REG.MCCTHH.bit.MCCTHH_0 = spr_ctrl_c->mid_freq_nr.h_c_threshold[0];
	IO_CNR(ch)->OFL_CSPR_REG.MCCTHH.bit.MCCTHH_1 = spr_ctrl_c->mid_freq_nr.h_c_threshold[1];
	IO_CNR(ch)->OFL_CSPR_REG.MCCTHH.bit.MCCTHH_2 = spr_ctrl_c->mid_freq_nr.h_c_threshold[2];
	IO_CNR(ch)->OFL_CSPR_REG.MCCTHH.bit.MCCTHH_3 = spr_ctrl_c->mid_freq_nr.h_c_threshold[3];

	IO_CNR(ch)->OFL_CSPR_REG.MCCTHV.bit.MCCTHV_0 = spr_ctrl_c->mid_freq_nr.v_c_threshold[0];
	IO_CNR(ch)->OFL_CSPR_REG.MCCTHV.bit.MCCTHV_1 = spr_ctrl_c->mid_freq_nr.v_c_threshold[1];
	IO_CNR(ch)->OFL_CSPR_REG.MCCTHV.bit.MCCTHV_2 = spr_ctrl_c->mid_freq_nr.v_c_threshold[2];
	IO_CNR(ch)->OFL_CSPR_REG.MCCTHV.bit.MCCTHV_3 = spr_ctrl_c->mid_freq_nr.v_c_threshold[3];

	IO_CNR(ch)->OFL_CSPR_REG.MCYDYMDT.bit.MCYDYMYCR = spr_ctrl_c->mid_freq_nr.y_threshold_cor;
	IO_CNR(ch)->OFL_CSPR_REG.MCYDYMDT.bit.MCYDYMYG = spr_ctrl_c->mid_freq_nr.y_threshold_gain;

	IO_CNR(ch)->OFL_CSPR_REG.MCYDYM.bit.MCYDYM = spr_ctrl_c->mid_freq_nr.y_threshold_dym_en;

	IO_CNR(ch)->OFL_CSPR_REG.MCYSCL.bit.MCYSCL = spr_ctrl_c->mid_freq_nr.y_threshold_scl_en;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCL.bit.MCSSCL = spr_ctrl_c->mid_freq_nr.c_y_threshold_scl_en;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCL.bit.MCCSCL = spr_ctrl_c->mid_freq_nr.c_threshold_scl_en;

	IO_CNR(ch)->OFL_CSPR_REG.LCEN.bit.LCEN = spr_ctrl_c->low_freq_nr.enable;

	IO_CNR(ch)->OFL_CSPR_REG.LCYTHH.bit.LCYTHH_0 = spr_ctrl_c->low_freq_nr.h_y_threshold[0];
	IO_CNR(ch)->OFL_CSPR_REG.LCYTHH.bit.LCYTHH_1 = spr_ctrl_c->low_freq_nr.h_y_threshold[1];
	IO_CNR(ch)->OFL_CSPR_REG.LCYTHH.bit.LCYTHH_2 = spr_ctrl_c->low_freq_nr.h_y_threshold[2];
	IO_CNR(ch)->OFL_CSPR_REG.LCYTHH.bit.LCYTHH_3 = spr_ctrl_c->low_freq_nr.h_y_threshold[3];

	IO_CNR(ch)->OFL_CSPR_REG.LCYTHV.bit.LCYTHV_0 = spr_ctrl_c->low_freq_nr.v_y_threshold[0];
	IO_CNR(ch)->OFL_CSPR_REG.LCYTHV.bit.LCYTHV_1 = spr_ctrl_c->low_freq_nr.v_y_threshold[1];
	IO_CNR(ch)->OFL_CSPR_REG.LCYTHV.bit.LCYTHV_2 = spr_ctrl_c->low_freq_nr.v_y_threshold[2];
	IO_CNR(ch)->OFL_CSPR_REG.LCYTHV.bit.LCYTHV_3 = spr_ctrl_c->low_freq_nr.v_y_threshold[3];

	IO_CNR(ch)->OFL_CSPR_REG.LCCTHH.bit.LCCTHH_0 = spr_ctrl_c->low_freq_nr.h_c_threshold[0];
	IO_CNR(ch)->OFL_CSPR_REG.LCCTHH.bit.LCCTHH_1 = spr_ctrl_c->low_freq_nr.h_c_threshold[1];
	IO_CNR(ch)->OFL_CSPR_REG.LCCTHH.bit.LCCTHH_2 = spr_ctrl_c->low_freq_nr.h_c_threshold[2];
	IO_CNR(ch)->OFL_CSPR_REG.LCCTHH.bit.LCCTHH_3 = spr_ctrl_c->low_freq_nr.h_c_threshold[3];

	IO_CNR(ch)->OFL_CSPR_REG.LCCTHV.bit.LCCTHV_0 = spr_ctrl_c->low_freq_nr.v_c_threshold[0];
	IO_CNR(ch)->OFL_CSPR_REG.LCCTHV.bit.LCCTHV_1 = spr_ctrl_c->low_freq_nr.v_c_threshold[1];
	IO_CNR(ch)->OFL_CSPR_REG.LCCTHV.bit.LCCTHV_2 = spr_ctrl_c->low_freq_nr.v_c_threshold[2];
	IO_CNR(ch)->OFL_CSPR_REG.LCCTHV.bit.LCCTHV_3 = spr_ctrl_c->low_freq_nr.v_c_threshold[3];

	IO_CNR(ch)->OFL_CSPR_REG.LCYDYMDT.bit.LCYDYMYCR = spr_ctrl_c->low_freq_nr.y_threshold_cor;
	IO_CNR(ch)->OFL_CSPR_REG.LCYDYMDT.bit.LCYDYMYG = spr_ctrl_c->low_freq_nr.y_threshold_gain;

	IO_CNR(ch)->OFL_CSPR_REG.LCYDYM.bit.LCYDYM = spr_ctrl_c->low_freq_nr.y_threshold_dym_en;

	IO_CNR(ch)->OFL_CSPR_REG.LCYSCL.bit.LCYSCL = spr_ctrl_c->low_freq_nr.y_threshold_scl_en;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCL.bit.LCSSCL = spr_ctrl_c->low_freq_nr.c_y_threshold_scl_en;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCL.bit.LCCSCL = spr_ctrl_c->low_freq_nr.c_threshold_scl_en;

	IO_CNR(ch)->OFL_CSPR_REG.WHTMD.bit.WHTEN = spr_ctrl_c->whitening_enable;
	IO_CNR(ch)->OFL_CSPR_REG.WHTMD.bit.WHTVAL = spr_ctrl_c->whitening_value;
	IO_CNR(ch)->OFL_CSPR_REG.WHTMD.bit.WHTBASE = spr_ctrl_c->whitening_base;

	IO_CNR(ch)->OFL_CSPR_REG.BLEND.bit.BLENDMD = spr_ctrl_c->c_blend_md;
	IO_CNR(ch)->OFL_CSPR_REG.BLEND.bit.CSPRALPBD = spr_ctrl_c->c_alpha_blend;
	IO_CNR(ch)->OFL_CSPR_REG.BLEND.bit.CSRGEN = spr_ctrl_c->c_range;
	IO_CNR(ch)->OFL_CSPR_REG.BLEND.bit.CSRGMD = spr_ctrl_c->c_range_mode;

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR setting for CbCr
*/
INT32 Im_CNR_OTF_Ctrl_C( UCHAR ch, const T_IM_CNR_CTRL_C* const spr_ctrl_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Ctrl_C() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_ctrl_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Ctrl_C() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.CSPREN.bit.CSPREN = spr_ctrl_c->enable;
	IO_CNR(ch)->OTF_CSPR_REG.CGDKEN.bit.CGDKEN = spr_ctrl_c->gradation_keep_en;

	IO_CNR(ch)->OTF_CSPR_REG.MCEN.bit.MCEN = spr_ctrl_c->mid_freq_nr.enable;

	IO_CNR(ch)->OTF_CSPR_REG.MCYTHH.bit.MCYTHH_0 = spr_ctrl_c->mid_freq_nr.h_y_threshold[0];
	IO_CNR(ch)->OTF_CSPR_REG.MCYTHH.bit.MCYTHH_1 = spr_ctrl_c->mid_freq_nr.h_y_threshold[1];
	IO_CNR(ch)->OTF_CSPR_REG.MCYTHH.bit.MCYTHH_2 = spr_ctrl_c->mid_freq_nr.h_y_threshold[2];
	IO_CNR(ch)->OTF_CSPR_REG.MCYTHH.bit.MCYTHH_3 = spr_ctrl_c->mid_freq_nr.h_y_threshold[3];

	IO_CNR(ch)->OTF_CSPR_REG.MCYTHV.bit.MCYTHV_0 = spr_ctrl_c->mid_freq_nr.v_y_threshold[0];
	IO_CNR(ch)->OTF_CSPR_REG.MCYTHV.bit.MCYTHV_1 = spr_ctrl_c->mid_freq_nr.v_y_threshold[1];
	IO_CNR(ch)->OTF_CSPR_REG.MCYTHV.bit.MCYTHV_2 = spr_ctrl_c->mid_freq_nr.v_y_threshold[2];
	IO_CNR(ch)->OTF_CSPR_REG.MCYTHV.bit.MCYTHV_3 = spr_ctrl_c->mid_freq_nr.v_y_threshold[3];

	IO_CNR(ch)->OTF_CSPR_REG.MCCTHH.bit.MCCTHH_0 = spr_ctrl_c->mid_freq_nr.h_c_threshold[0];
	IO_CNR(ch)->OTF_CSPR_REG.MCCTHH.bit.MCCTHH_1 = spr_ctrl_c->mid_freq_nr.h_c_threshold[1];
	IO_CNR(ch)->OTF_CSPR_REG.MCCTHH.bit.MCCTHH_2 = spr_ctrl_c->mid_freq_nr.h_c_threshold[2];
	IO_CNR(ch)->OTF_CSPR_REG.MCCTHH.bit.MCCTHH_3 = spr_ctrl_c->mid_freq_nr.h_c_threshold[3];

	IO_CNR(ch)->OTF_CSPR_REG.MCCTHV.bit.MCCTHV_0 = spr_ctrl_c->mid_freq_nr.v_c_threshold[0];
	IO_CNR(ch)->OTF_CSPR_REG.MCCTHV.bit.MCCTHV_1 = spr_ctrl_c->mid_freq_nr.v_c_threshold[1];
	IO_CNR(ch)->OTF_CSPR_REG.MCCTHV.bit.MCCTHV_2 = spr_ctrl_c->mid_freq_nr.v_c_threshold[2];
	IO_CNR(ch)->OTF_CSPR_REG.MCCTHV.bit.MCCTHV_3 = spr_ctrl_c->mid_freq_nr.v_c_threshold[3];

	IO_CNR(ch)->OTF_CSPR_REG.MCYDYMDT.bit.MCYDYMYCR = spr_ctrl_c->mid_freq_nr.y_threshold_cor;
	IO_CNR(ch)->OTF_CSPR_REG.MCYDYMDT.bit.MCYDYMYG = spr_ctrl_c->mid_freq_nr.y_threshold_gain;

	IO_CNR(ch)->OTF_CSPR_REG.MCYDYM.bit.MCYDYM = spr_ctrl_c->mid_freq_nr.y_threshold_dym_en;

	IO_CNR(ch)->OTF_CSPR_REG.MCYSCL.bit.MCYSCL = spr_ctrl_c->mid_freq_nr.y_threshold_scl_en;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCL.bit.MCSSCL = spr_ctrl_c->mid_freq_nr.c_y_threshold_scl_en;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCL.bit.MCCSCL = spr_ctrl_c->mid_freq_nr.c_threshold_scl_en;

	IO_CNR(ch)->OTF_CSPR_REG.LCEN.bit.LCEN = spr_ctrl_c->low_freq_nr.enable;

	IO_CNR(ch)->OTF_CSPR_REG.LCYTHH.bit.LCYTHH_0 = spr_ctrl_c->low_freq_nr.h_y_threshold[0];
	IO_CNR(ch)->OTF_CSPR_REG.LCYTHH.bit.LCYTHH_1 = spr_ctrl_c->low_freq_nr.h_y_threshold[1];
	IO_CNR(ch)->OTF_CSPR_REG.LCYTHH.bit.LCYTHH_2 = spr_ctrl_c->low_freq_nr.h_y_threshold[2];
	IO_CNR(ch)->OTF_CSPR_REG.LCYTHH.bit.LCYTHH_3 = spr_ctrl_c->low_freq_nr.h_y_threshold[3];

	IO_CNR(ch)->OTF_CSPR_REG.LCYTHV.bit.LCYTHV_0 = spr_ctrl_c->low_freq_nr.v_y_threshold[0];
	IO_CNR(ch)->OTF_CSPR_REG.LCYTHV.bit.LCYTHV_1 = spr_ctrl_c->low_freq_nr.v_y_threshold[1];
	IO_CNR(ch)->OTF_CSPR_REG.LCYTHV.bit.LCYTHV_2 = spr_ctrl_c->low_freq_nr.v_y_threshold[2];
	IO_CNR(ch)->OTF_CSPR_REG.LCYTHV.bit.LCYTHV_3 = spr_ctrl_c->low_freq_nr.v_y_threshold[3];

	IO_CNR(ch)->OTF_CSPR_REG.LCCTHH.bit.LCCTHH_0 = spr_ctrl_c->low_freq_nr.h_c_threshold[0];
	IO_CNR(ch)->OTF_CSPR_REG.LCCTHH.bit.LCCTHH_1 = spr_ctrl_c->low_freq_nr.h_c_threshold[1];
	IO_CNR(ch)->OTF_CSPR_REG.LCCTHH.bit.LCCTHH_2 = spr_ctrl_c->low_freq_nr.h_c_threshold[2];
	IO_CNR(ch)->OTF_CSPR_REG.LCCTHH.bit.LCCTHH_3 = spr_ctrl_c->low_freq_nr.h_c_threshold[3];

	IO_CNR(ch)->OTF_CSPR_REG.LCCTHV.bit.LCCTHV_0 = spr_ctrl_c->low_freq_nr.v_c_threshold[0];
	IO_CNR(ch)->OTF_CSPR_REG.LCCTHV.bit.LCCTHV_1 = spr_ctrl_c->low_freq_nr.v_c_threshold[1];
	IO_CNR(ch)->OTF_CSPR_REG.LCCTHV.bit.LCCTHV_2 = spr_ctrl_c->low_freq_nr.v_c_threshold[2];
	IO_CNR(ch)->OTF_CSPR_REG.LCCTHV.bit.LCCTHV_3 = spr_ctrl_c->low_freq_nr.v_c_threshold[3];

	IO_CNR(ch)->OTF_CSPR_REG.LCYDYMDT.bit.LCYDYMYCR = spr_ctrl_c->low_freq_nr.y_threshold_cor;
	IO_CNR(ch)->OTF_CSPR_REG.LCYDYMDT.bit.LCYDYMYG = spr_ctrl_c->low_freq_nr.y_threshold_gain;

	IO_CNR(ch)->OTF_CSPR_REG.LCYDYM.bit.LCYDYM = spr_ctrl_c->low_freq_nr.y_threshold_dym_en;

	IO_CNR(ch)->OTF_CSPR_REG.LCYSCL.bit.LCYSCL = spr_ctrl_c->low_freq_nr.y_threshold_scl_en;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCL.bit.LCCSCL = spr_ctrl_c->low_freq_nr.c_threshold_scl_en;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCL.bit.LCCSCL = spr_ctrl_c->low_freq_nr.c_threshold_scl_en;

	IO_CNR(ch)->OTF_CSPR_REG.WHTMD.bit.WHTEN = spr_ctrl_c->whitening_enable;
	IO_CNR(ch)->OTF_CSPR_REG.WHTMD.bit.WHTVAL = spr_ctrl_c->whitening_value;
	IO_CNR(ch)->OTF_CSPR_REG.WHTMD.bit.WHTBASE = spr_ctrl_c->whitening_base;

	IO_CNR(ch)->OTF_CSPR_REG.BLEND.bit.BLENDMD = spr_ctrl_c->c_blend_md;
	IO_CNR(ch)->OTF_CSPR_REG.BLEND.bit.CSPRALPBD = spr_ctrl_c->c_alpha_blend;
	IO_CNR(ch)->OTF_CSPR_REG.BLEND.bit.CSRGEN = spr_ctrl_c->c_range;
	IO_CNR(ch)->OTF_CSPR_REG.BLEND.bit.CSRGMD = spr_ctrl_c->c_range_mode;

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR setting for Y for Off Line
*/
INT32 Im_CNR_OFL_Ctrl_Y( UCHAR ch, const T_IM_CNR_OFL_CTRL_Y* const spr_ctrl_y )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Ctrl_Y() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_ctrl_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Ctrl_Y() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_REG_YSPR.YSPREN.bit.YSPREN = spr_ctrl_y->enable;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRMODE.bit.YSPRMD = spr_ctrl_y->threshold_type;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRMODE.bit.YSPRFE = spr_ctrl_y->level;

	IO_CNR(ch)->OFL_REG_YSPR.YSPRFXEY.bit.YSPRFXEYV = spr_ctrl_y->v_y_threshold;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRFXEY.bit.YSPRFXEYH = spr_ctrl_y->h_y_threshold;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRFXEC.bit.YSPRFXECV = spr_ctrl_y->v_c_threshold;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRFXEC.bit.YSPRFXECH = spr_ctrl_y->h_c_threshold;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRALPBD.bit.YSPRALPBD = spr_ctrl_y->alpha_blend_ratio;

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR table for Y
*/
INT32 Im_CNR_OFL_Set_YSPR_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_yspr )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_YSPR_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_yspr == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_YSPR_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_1	= spr_yspr->bd1;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_2	= spr_yspr->bd2;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_3	= spr_yspr->bd3;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_4	= spr_yspr->bd4;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD2.bit.YSPRDYEYBD_5	= spr_yspr->bd5;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_0	= spr_yspr->of0;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_1	= spr_yspr->of1;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_2	= spr_yspr->of2;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_3	= spr_yspr->of3;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF2.bit.YSPRDYEYOF_4	= spr_yspr->of4;
	IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF2.bit.YSPRDYEYOF_5	= spr_yspr->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1, union io_cnr_ofl_reg_yspr_ysprdyeyga_1, YSPRDYEYGA_0, spr_yspr->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1, union io_cnr_ofl_reg_yspr_ysprdyeyga_1, YSPRDYEYGA_1, spr_yspr->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1, union io_cnr_ofl_reg_yspr_ysprdyeyga_1, YSPRDYEYGA_2, spr_yspr->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1, union io_cnr_ofl_reg_yspr_ysprdyeyga_1, YSPRDYEYGA_3, spr_yspr->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA2, union io_cnr_ofl_reg_yspr_ysprdyeyga_2, YSPRDYEYGA_4, spr_yspr->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA2, union io_cnr_ofl_reg_yspr_ysprdyeyga_2, YSPRDYEYGA_5, spr_yspr->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}


/*
Set SPR Y table for Middle frequency CbCr
*/
INT32 Im_CNR_OFL_Set_CSPR_Mid_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_mid_y )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_CSPR_Mid_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_mid_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_CSPR_Mid_Y_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_1	= spr_cspr_mid_y->bd1;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_2	= spr_cspr_mid_y->bd2;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_3	= spr_cspr_mid_y->bd3;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_4	= spr_cspr_mid_y->bd4;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD2.bit.MCYSCLBD_5	= spr_cspr_mid_y->bd5;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_0	= spr_cspr_mid_y->of0;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_1	= spr_cspr_mid_y->of1;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_2	= spr_cspr_mid_y->of2;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_3	= spr_cspr_mid_y->of3;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_4	= spr_cspr_mid_y->of4;
	IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_5	= spr_cspr_mid_y->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_0, spr_cspr_mid_y->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_1, spr_cspr_mid_y->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_2, spr_cspr_mid_y->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_3, spr_cspr_mid_y->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA2, union io_cnr_cspr_reg_mcysclga_2, MCYSCLGA_4, spr_cspr_mid_y->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA2, union io_cnr_cspr_reg_mcysclga_2, MCYSCLGA_5, spr_cspr_mid_y->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR Y table for Middle frequency CbCr
*/
INT32 Im_CNR_OTF_Set_CSPR_Mid_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_mid_y )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_CSPR_Mid_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_mid_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_CSPR_Mid_Y_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_1	= spr_cspr_mid_y->bd1;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_2	= spr_cspr_mid_y->bd2;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_3	= spr_cspr_mid_y->bd3;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.bit.MCYSCLBD_4	= spr_cspr_mid_y->bd4;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD2.bit.MCYSCLBD_5	= spr_cspr_mid_y->bd5;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_0	= spr_cspr_mid_y->of0;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_1	= spr_cspr_mid_y->of1;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_2	= spr_cspr_mid_y->of2;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.bit.MCYSCLOF_3	= spr_cspr_mid_y->of3;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_4	= spr_cspr_mid_y->of4;
	IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF2.bit.MCYSCLOF_5	= spr_cspr_mid_y->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_0, spr_cspr_mid_y->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_1, spr_cspr_mid_y->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_2, spr_cspr_mid_y->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1, union io_cnr_cspr_reg_mcysclga_1, MCYSCLGA_3, spr_cspr_mid_y->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA2, union io_cnr_cspr_reg_mcysclga_2, MCYSCLGA_4, spr_cspr_mid_y->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA2, union io_cnr_cspr_reg_mcysclga_2, MCYSCLGA_5, spr_cspr_mid_y->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR CbCr table for Middle frequency CbCr
*/
INT32 Im_CNR_OFL_Set_CSPR_Mid_CC_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_mid_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_CSPR_Mid_CC_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_mid_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_CSPR_Mid_CC_Y_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_1	= spr_cspr_mid_c->bd1;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_2	= spr_cspr_mid_c->bd2;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_3	= spr_cspr_mid_c->bd3;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_4	= spr_cspr_mid_c->bd4;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD2.bit.MCCSCLBD_5	= spr_cspr_mid_c->bd5;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_0	= spr_cspr_mid_c->of0;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_1	= spr_cspr_mid_c->of1;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_2	= spr_cspr_mid_c->of2;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_3	= spr_cspr_mid_c->of3;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_4	= spr_cspr_mid_c->of4;
	IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_5	= spr_cspr_mid_c->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_0, spr_cspr_mid_c->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_1, spr_cspr_mid_c->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_2, spr_cspr_mid_c->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_3, spr_cspr_mid_c->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA2, union io_cnr_cspr_reg_mccsclga_2, MCCSCLGA_4, spr_cspr_mid_c->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA2, union io_cnr_cspr_reg_mccsclga_2, MCCSCLGA_5, spr_cspr_mid_c->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR CbCr table for Middle frequency CbCr
*/
INT32 Im_CNR_OTF_Set_CSPR_Mid_CC_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_mid_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_CSPR_Mid_CC_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_mid_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_CSPR_Mid_CC_Y_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_1	= spr_cspr_mid_c->bd1;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_2	= spr_cspr_mid_c->bd2;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_3	= spr_cspr_mid_c->bd3;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.bit.MCCSCLBD_4	= spr_cspr_mid_c->bd4;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD2.bit.MCCSCLBD_5	= spr_cspr_mid_c->bd5;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_0	= spr_cspr_mid_c->of0;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_1	= spr_cspr_mid_c->of1;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_2	= spr_cspr_mid_c->of2;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.bit.MCCSCLOF_3	= spr_cspr_mid_c->of3;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_4	= spr_cspr_mid_c->of4;
	IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF2.bit.MCCSCLOF_5	= spr_cspr_mid_c->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_0, spr_cspr_mid_c->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_1, spr_cspr_mid_c->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_2, spr_cspr_mid_c->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1, union io_cnr_cspr_reg_mccsclga_1, MCCSCLGA_3, spr_cspr_mid_c->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA2, union io_cnr_cspr_reg_mccsclga_2, MCCSCLGA_4, spr_cspr_mid_c->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA2, union io_cnr_cspr_reg_mccsclga_2, MCCSCLGA_5, spr_cspr_mid_c->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR CbCr table for Middle frequency CbCr
*/
INT32 Im_CNR_OFL_Set_CSPR_Mid_CC_C_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_mid_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_CSPR_Mid_CC_C_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_mid_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_CSPR_Mid_CC_C_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_1	= spr_cspr_mid_c->bd1;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_2	= spr_cspr_mid_c->bd2;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_3	= spr_cspr_mid_c->bd3;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_4	= spr_cspr_mid_c->bd4;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD2.bit.MCSSCLBD_5	= spr_cspr_mid_c->bd5;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_0	= spr_cspr_mid_c->of0;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_1	= spr_cspr_mid_c->of1;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_2	= spr_cspr_mid_c->of2;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_3	= spr_cspr_mid_c->of3;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_4	= spr_cspr_mid_c->of4;
	IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_5	= spr_cspr_mid_c->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_0, spr_cspr_mid_c->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_1, spr_cspr_mid_c->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_2, spr_cspr_mid_c->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_3, spr_cspr_mid_c->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA2, union io_cnr_cspr_reg_mcssclga_2, MCSSCLGA_4, spr_cspr_mid_c->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA2, union io_cnr_cspr_reg_mcssclga_2, MCSSCLGA_5, spr_cspr_mid_c->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR CbCr table for Middle frequency CbCr
*/
INT32 Im_CNR_OTF_Set_CSPR_Mid_CC_C_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_mid_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_CSPR_Mid_CC_C_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_mid_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_CSPR_Mid_CC_C_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_1	= spr_cspr_mid_c->bd1;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_2	= spr_cspr_mid_c->bd2;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_3	= spr_cspr_mid_c->bd3;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.bit.MCSSCLBD_4	= spr_cspr_mid_c->bd4;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD2.bit.MCSSCLBD_5	= spr_cspr_mid_c->bd5;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_0	= spr_cspr_mid_c->of0;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_1	= spr_cspr_mid_c->of1;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_2	= spr_cspr_mid_c->of2;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.bit.MCSSCLOF_3	= spr_cspr_mid_c->of3;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_4	= spr_cspr_mid_c->of4;
	IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF2.bit.MCSSCLOF_5	= spr_cspr_mid_c->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_0, spr_cspr_mid_c->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_1, spr_cspr_mid_c->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_2, spr_cspr_mid_c->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1, union io_cnr_cspr_reg_mcssclga_1, MCSSCLGA_3, spr_cspr_mid_c->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA2, union io_cnr_cspr_reg_mcssclga_2, MCSSCLGA_4, spr_cspr_mid_c->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA2, union io_cnr_cspr_reg_mcssclga_2, MCSSCLGA_5, spr_cspr_mid_c->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR Y table for Low frequency CbCr
*/
INT32 Im_CNR_OFL_Set_CSPR_Low_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_low_y )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_CSPR_Low_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_low_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_CSPR_Low_Y_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_1	= spr_cspr_low_y->bd1;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_2	= spr_cspr_low_y->bd2;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_3	= spr_cspr_low_y->bd3;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_4	= spr_cspr_low_y->bd4;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD2.bit.LCYSCLBD_5	= spr_cspr_low_y->bd5;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_0	= spr_cspr_low_y->of0;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_1	= spr_cspr_low_y->of1;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_2	= spr_cspr_low_y->of2;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_3	= spr_cspr_low_y->of3;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_4	= spr_cspr_low_y->of4;
	IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_5	= spr_cspr_low_y->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_0, spr_cspr_low_y->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_1, spr_cspr_low_y->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_2, spr_cspr_low_y->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_3, spr_cspr_low_y->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA2, union io_cnr_cspr_reg_lcysclga_2, LCYSCLGA_4, spr_cspr_low_y->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA2, union io_cnr_cspr_reg_lcysclga_2, LCYSCLGA_5, spr_cspr_low_y->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR Y table for Low frequency CbCr
*/
INT32 Im_CNR_OTF_Set_CSPR_Low_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_low_y )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_CSPR_Low_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_low_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_CSPR_Low_Y_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_1	= spr_cspr_low_y->bd1;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_2	= spr_cspr_low_y->bd2;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_3	= spr_cspr_low_y->bd3;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.bit.LCYSCLBD_4	= spr_cspr_low_y->bd4;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD2.bit.LCYSCLBD_5	= spr_cspr_low_y->bd5;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_0	= spr_cspr_low_y->of0;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_1	= spr_cspr_low_y->of1;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_2	= spr_cspr_low_y->of2;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.bit.LCYSCLOF_3	= spr_cspr_low_y->of3;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_4	= spr_cspr_low_y->of4;
	IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF2.bit.LCYSCLOF_5	= spr_cspr_low_y->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_0, spr_cspr_low_y->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_1, spr_cspr_low_y->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_2, spr_cspr_low_y->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1, union io_cnr_cspr_reg_lcysclga_1, LCYSCLGA_3, spr_cspr_low_y->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA2, union io_cnr_cspr_reg_lcysclga_2, LCYSCLGA_4, spr_cspr_low_y->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA2, union io_cnr_cspr_reg_lcysclga_2, LCYSCLGA_5, spr_cspr_low_y->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR CbCr table for Low frequency CbCr
*/
INT32 Im_CNR_OFL_Set_CSPR_Low_CC_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_low_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_CSPR_Low_CC_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_low_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_CSPR_Low_CC_Y_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_1	= spr_cspr_low_c->bd1;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_2	= spr_cspr_low_c->bd2;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_3	= spr_cspr_low_c->bd3;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_4	= spr_cspr_low_c->bd4;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD2.bit.LCCSCLBD_5	= spr_cspr_low_c->bd5;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_0	= spr_cspr_low_c->of0;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_1	= spr_cspr_low_c->of1;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_2	= spr_cspr_low_c->of2;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_3	= spr_cspr_low_c->of3;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_4	= spr_cspr_low_c->of4;
	IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_5	= spr_cspr_low_c->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_0, spr_cspr_low_c->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_1, spr_cspr_low_c->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_2, spr_cspr_low_c->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_3, spr_cspr_low_c->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA2, union io_cnr_cspr_reg_lccsclga_2, LCCSCLGA_4, spr_cspr_low_c->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA2, union io_cnr_cspr_reg_lccsclga_2, LCCSCLGA_5, spr_cspr_low_c->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR CbCr table for Low frequency CbCr
*/
INT32 Im_CNR_OTF_Set_CSPR_Low_CC_Y_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_low_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_CSPR_Low_CC_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_low_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_CSPR_Low_CC_Y_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_1	= spr_cspr_low_c->bd1;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_2	= spr_cspr_low_c->bd2;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_3	= spr_cspr_low_c->bd3;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.bit.LCCSCLBD_4	= spr_cspr_low_c->bd4;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD2.bit.LCCSCLBD_5	= spr_cspr_low_c->bd5;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_0	= spr_cspr_low_c->of0;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_1	= spr_cspr_low_c->of1;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_2	= spr_cspr_low_c->of2;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.bit.LCCSCLOF_3	= spr_cspr_low_c->of3;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_4	= spr_cspr_low_c->of4;
	IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF2.bit.LCCSCLOF_5	= spr_cspr_low_c->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_0, spr_cspr_low_c->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_1, spr_cspr_low_c->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_2, spr_cspr_low_c->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1, union io_cnr_cspr_reg_lccsclga_1, LCCSCLGA_3, spr_cspr_low_c->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA2, union io_cnr_cspr_reg_lccsclga_2, LCCSCLGA_4, spr_cspr_low_c->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA2, union io_cnr_cspr_reg_lccsclga_2, LCCSCLGA_5, spr_cspr_low_c->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR CbCr table for Low frequency CbCr
*/
INT32 Im_CNR_OFL_Set_CSPR_Low_CC_C_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_low_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_CSPR_Low_CC_C_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_low_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_CSPR_Low_CC_C_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_1	= spr_cspr_low_c->bd1;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_2	= spr_cspr_low_c->bd2;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_3	= spr_cspr_low_c->bd3;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_4	= spr_cspr_low_c->bd4;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD2.bit.LCSSCLBD_5	= spr_cspr_low_c->bd5;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_0	= spr_cspr_low_c->of0;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_1	= spr_cspr_low_c->of1;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_2	= spr_cspr_low_c->of2;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_3	= spr_cspr_low_c->of3;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_4	= spr_cspr_low_c->of4;
	IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_5	= spr_cspr_low_c->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_0, spr_cspr_low_c->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_1, spr_cspr_low_c->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_2, spr_cspr_low_c->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_3, spr_cspr_low_c->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA2, union io_cnr_cspr_reg_lcssclga_2, LCSSCLGA_4, spr_cspr_low_c->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA2, union io_cnr_cspr_reg_lcssclga_2, LCSSCLGA_5, spr_cspr_low_c->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR CbCr table for Low frequency CbCr
*/
INT32 Im_CNR_OTF_Set_CSPR_Low_CC_C_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_low_c )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_CSPR_Low_CC_C_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_low_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_CSPR_Low_CC_C_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_1	= spr_cspr_low_c->bd1;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_2	= spr_cspr_low_c->bd2;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_3	= spr_cspr_low_c->bd3;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.bit.LCSSCLBD_4	= spr_cspr_low_c->bd4;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD2.bit.LCSSCLBD_5	= spr_cspr_low_c->bd5;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_0	= spr_cspr_low_c->of0;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_1	= spr_cspr_low_c->of1;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_2	= spr_cspr_low_c->of2;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.bit.LCSSCLOF_3	= spr_cspr_low_c->of3;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_4	= spr_cspr_low_c->of4;
	IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF2.bit.LCSSCLOF_5	= spr_cspr_low_c->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_0, spr_cspr_low_c->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_1, spr_cspr_low_c->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_2, spr_cspr_low_c->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1, union io_cnr_cspr_reg_lcssclga_1, LCSSCLGA_3, spr_cspr_low_c->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA2, union io_cnr_cspr_reg_lcssclga_2, LCSSCLGA_4, spr_cspr_low_c->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA2, union io_cnr_cspr_reg_lcssclga_2, LCSSCLGA_5, spr_cspr_low_c->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR Edge for CbcCr for Off Line
*/
INT32 Im_CNR_OFL_Set_CSPR_Edge_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_edge )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_CSPR_Edge_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_edge == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_CSPR_Edge_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_1	= spr_cspr_edge->bd1;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_2	= spr_cspr_edge->bd2;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_3	= spr_cspr_edge->bd3;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_4	= spr_cspr_edge->bd4;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD2.bit.CESCLBD_5	= spr_cspr_edge->bd5;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_0	= spr_cspr_edge->of0;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_1	= spr_cspr_edge->of1;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_2	= spr_cspr_edge->of2;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_3	= spr_cspr_edge->of3;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF2.bit.CESCLOF_4	= spr_cspr_edge->of4;
	IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF2.bit.CESCLOF_5	= spr_cspr_edge->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_0, spr_cspr_edge->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_1, spr_cspr_edge->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_2, spr_cspr_edge->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_3, spr_cspr_edge->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA2, union io_cnr_cspr_reg_cesclga_2, CESCLGA_4, spr_cspr_edge->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA2, union io_cnr_cspr_reg_cesclga_2, CESCLGA_5, spr_cspr_edge->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR Edge for CbcCr for Off Line
*/
INT32 Im_CNR_OTF_Set_CSPR_Edge_Table( UCHAR ch, const T_IM_CNR_TABLE* const spr_cspr_edge )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_CSPR_Edge_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_edge == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_CSPR_Edge_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_1	= spr_cspr_edge->bd1;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_2	= spr_cspr_edge->bd2;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_3	= spr_cspr_edge->bd3;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD1.bit.CESCLBD_4	= spr_cspr_edge->bd4;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD2.bit.CESCLBD_5	= spr_cspr_edge->bd5;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_0	= spr_cspr_edge->of0;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_1	= spr_cspr_edge->of1;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_2	= spr_cspr_edge->of2;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF1.bit.CESCLOF_3	= spr_cspr_edge->of3;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF2.bit.CESCLOF_4	= spr_cspr_edge->of4;
	IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF2.bit.CESCLOF_5	= spr_cspr_edge->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_0, spr_cspr_edge->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_1, spr_cspr_edge->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_2, spr_cspr_edge->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA1, union io_cnr_cspr_reg_cesclga_1, CESCLGA_3, spr_cspr_edge->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA2, union io_cnr_cspr_reg_cesclga_2, CESCLGA_4, spr_cspr_edge->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA2, union io_cnr_cspr_reg_cesclga_2, CESCLGA_5, spr_cspr_edge->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR Hue specified for CbcCr for Off Line
*/
INT32 Im_CNR_OFL_Set_CSPR_Hue_Specified_Table( UCHAR ch, const T_IM_CNR_HUE_SPEC_TABLE* const spr_cspr_hue )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Set_CSPR_Hue_Specified_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_hue == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Set_CSPR_Hue_Specified_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CPHPT, union io_cnr_cspr_reg_cphpt, CDCB, spr_cspr_hue->cd_cb );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CPHPT, union io_cnr_cspr_reg_cphpt, CDCR, spr_cspr_hue->cd_cr );

	IO_CNR(ch)->OFL_CSPR_REG.CPHPTK.bit.KCB	= spr_cspr_hue->k_cb;
	IO_CNR(ch)->OFL_CSPR_REG.CPHPTK.bit.KCR	= spr_cspr_hue->k_cr;

	IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD1.bit.CDSCLBD_1	= spr_cspr_hue->bd1;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD1.bit.CDSCLBD_2	= spr_cspr_hue->bd2;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD2.bit.CDSCLBD_3	= spr_cspr_hue->bd3;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD2.bit.CDSCLBD_4	= spr_cspr_hue->bd4;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD3.bit.CDSCLBD_5	= spr_cspr_hue->bd5;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF1.bit.CDSCLOF_0	= spr_cspr_hue->of0;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF1.bit.CDSCLOF_1	= spr_cspr_hue->of1;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF2.bit.CDSCLOF_2	= spr_cspr_hue->of2;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF2.bit.CDSCLOF_3	= spr_cspr_hue->of3;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF3.bit.CDSCLOF_4	= spr_cspr_hue->of4;
	IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF3.bit.CDSCLOF_5	= spr_cspr_hue->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA1, union io_cnr_cspr_reg_cdsclga_1, CDSCLGA_0, spr_cspr_hue->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA1, union io_cnr_cspr_reg_cdsclga_1, CDSCLGA_1, spr_cspr_hue->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA2, union io_cnr_cspr_reg_cdsclga_2, CDSCLGA_2, spr_cspr_hue->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA2, union io_cnr_cspr_reg_cdsclga_2, CDSCLGA_3, spr_cspr_hue->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA3, union io_cnr_cspr_reg_cdsclga_3, CDSCLGA_4, spr_cspr_hue->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA3, union io_cnr_cspr_reg_cdsclga_3, CDSCLGA_5, spr_cspr_hue->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set SPR Hue specified for CbcCr for Off Line
*/
INT32 Im_CNR_OTF_Set_CSPR_Hue_Specified_Table( UCHAR ch, const T_IM_CNR_HUE_SPEC_TABLE* const spr_cspr_hue )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Set_CSPR_Hue_Specified_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( spr_cspr_hue == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Set_CSPR_Hue_Specified_Table() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CPHPT, union io_cnr_cspr_reg_cphpt, CDCB, spr_cspr_hue->cd_cb );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CPHPT, union io_cnr_cspr_reg_cphpt, CDCR, spr_cspr_hue->cd_cr );

	IO_CNR(ch)->OTF_CSPR_REG.CPHPTK.bit.KCB	= spr_cspr_hue->k_cb;
	IO_CNR(ch)->OTF_CSPR_REG.CPHPTK.bit.KCR	= spr_cspr_hue->k_cr;

	IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD1.bit.CDSCLBD_1	= spr_cspr_hue->bd1;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD1.bit.CDSCLBD_2	= spr_cspr_hue->bd2;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD2.bit.CDSCLBD_3	= spr_cspr_hue->bd3;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD2.bit.CDSCLBD_4	= spr_cspr_hue->bd4;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD3.bit.CDSCLBD_5	= spr_cspr_hue->bd5;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF1.bit.CDSCLOF_0	= spr_cspr_hue->of0;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF1.bit.CDSCLOF_1	= spr_cspr_hue->of1;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF2.bit.CDSCLOF_2	= spr_cspr_hue->of2;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF2.bit.CDSCLOF_3	= spr_cspr_hue->of3;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF3.bit.CDSCLOF_4	= spr_cspr_hue->of4;
	IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF3.bit.CDSCLOF_5	= spr_cspr_hue->of5;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA1, union io_cnr_cspr_reg_cdsclga_1, CDSCLGA_0, spr_cspr_hue->gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA1, union io_cnr_cspr_reg_cdsclga_1, CDSCLGA_1, spr_cspr_hue->gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA2, union io_cnr_cspr_reg_cdsclga_2, CDSCLGA_2, spr_cspr_hue->gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA2, union io_cnr_cspr_reg_cdsclga_2, CDSCLGA_3, spr_cspr_hue->gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA3, union io_cnr_cspr_reg_cdsclga_3, CDSCLGA_4, spr_cspr_hue->gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA3, union io_cnr_cspr_reg_cdsclga_3, CDSCLGA_5, spr_cspr_hue->gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set CbCr setting of source
*/
INT32 Im_CNR_OFL_Ctrl_SPRS_CC( UCHAR ch, const T_IM_CNR_CTRL_CC_SRC_DST* const sprs_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Ctrl_SPRS_CC() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( sprs_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Ctrl_SPRS_CC() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.SPRS.bit.SPRS = sprs_ctrl->enable;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_1, sprs_ctrl->cb_table.bd1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_2, sprs_ctrl->cb_table.bd2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_3, sprs_ctrl->cb_table.bd3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_4, sprs_ctrl->cb_table.bd4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD2, union io_cnr_cspr_reg_sprscbbd_2, SPRSCBBD_5, sprs_ctrl->cb_table.bd5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_0, sprs_ctrl->cb_table.of0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_1, sprs_ctrl->cb_table.of1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_2, sprs_ctrl->cb_table.of2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_3, sprs_ctrl->cb_table.of3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF2, union io_cnr_cspr_reg_sprscbof_2, SPRSCBOF_4, sprs_ctrl->cb_table.of4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF2, union io_cnr_cspr_reg_sprscbof_2, SPRSCBOF_5, sprs_ctrl->cb_table.of5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA1, union io_cnr_cspr_reg_sprscbga_1, SPRSCBGA_0, sprs_ctrl->cb_table.gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA1, union io_cnr_cspr_reg_sprscbga_1, SPRSCBGA_1, sprs_ctrl->cb_table.gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA2, union io_cnr_cspr_reg_sprscbga_2, SPRSCBGA_2, sprs_ctrl->cb_table.gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA2, union io_cnr_cspr_reg_sprscbga_2, SPRSCBGA_3, sprs_ctrl->cb_table.gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA3, union io_cnr_cspr_reg_sprscbga_3, SPRSCBGA_4, sprs_ctrl->cb_table.gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA3, union io_cnr_cspr_reg_sprscbga_3, SPRSCBGA_5, sprs_ctrl->cb_table.gain5 );

	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_1, sprs_ctrl->cr_table.bd1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_2, sprs_ctrl->cr_table.bd2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_3, sprs_ctrl->cr_table.bd3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_4, sprs_ctrl->cr_table.bd4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD2, union io_cnr_cspr_reg_sprscrbd_2, SPRSCRBD_5, sprs_ctrl->cr_table.bd5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_0, sprs_ctrl->cr_table.of0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_1, sprs_ctrl->cr_table.of1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_2, sprs_ctrl->cr_table.of2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_3, sprs_ctrl->cr_table.of3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF2, union io_cnr_cspr_reg_sprscrof_2, SPRSCROF_4, sprs_ctrl->cr_table.of4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF2, union io_cnr_cspr_reg_sprscrof_2, SPRSCROF_5, sprs_ctrl->cr_table.of5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA1, union io_cnr_cspr_reg_sprscrga_1, SPRSCRGA_0, sprs_ctrl->cr_table.gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA1, union io_cnr_cspr_reg_sprscrga_1, SPRSCRGA_1, sprs_ctrl->cr_table.gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA2, union io_cnr_cspr_reg_sprscrga_2, SPRSCRGA_2, sprs_ctrl->cr_table.gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA2, union io_cnr_cspr_reg_sprscrga_2, SPRSCRGA_3, sprs_ctrl->cr_table.gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA3, union io_cnr_cspr_reg_sprscrga_3, SPRSCRGA_4, sprs_ctrl->cr_table.gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA3, union io_cnr_cspr_reg_sprscrga_3, SPRSCRGA_5, sprs_ctrl->cr_table.gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set CbCr setting of source
*/
INT32 Im_CNR_OTF_Ctrl_SPRS_CC( UCHAR ch, const T_IM_CNR_CTRL_CC_SRC_DST* const sprs_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Ctrl_SPRS_CC() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( sprs_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Ctrl_SPRS_CC() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.SPRS.bit.SPRS = sprs_ctrl->enable;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_1, sprs_ctrl->cb_table.bd1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_2, sprs_ctrl->cb_table.bd2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_3, sprs_ctrl->cb_table.bd3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1, union io_cnr_cspr_reg_sprscbbd_1, SPRSCBBD_4, sprs_ctrl->cb_table.bd4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD2, union io_cnr_cspr_reg_sprscbbd_2, SPRSCBBD_5, sprs_ctrl->cb_table.bd5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_0, sprs_ctrl->cb_table.of0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_1, sprs_ctrl->cb_table.of1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_2, sprs_ctrl->cb_table.of2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1, union io_cnr_cspr_reg_sprscbof_1, SPRSCBOF_3, sprs_ctrl->cb_table.of3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF2, union io_cnr_cspr_reg_sprscbof_2, SPRSCBOF_4, sprs_ctrl->cb_table.of4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF2, union io_cnr_cspr_reg_sprscbof_2, SPRSCBOF_5, sprs_ctrl->cb_table.of5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA1, union io_cnr_cspr_reg_sprscbga_1, SPRSCBGA_0, sprs_ctrl->cb_table.gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA1, union io_cnr_cspr_reg_sprscbga_1, SPRSCBGA_1, sprs_ctrl->cb_table.gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA2, union io_cnr_cspr_reg_sprscbga_2, SPRSCBGA_2, sprs_ctrl->cb_table.gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA2, union io_cnr_cspr_reg_sprscbga_2, SPRSCBGA_3, sprs_ctrl->cb_table.gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA3, union io_cnr_cspr_reg_sprscbga_3, SPRSCBGA_4, sprs_ctrl->cb_table.gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA3, union io_cnr_cspr_reg_sprscbga_3, SPRSCBGA_5, sprs_ctrl->cb_table.gain5 );

	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_1, sprs_ctrl->cr_table.bd1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_2, sprs_ctrl->cr_table.bd2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_3, sprs_ctrl->cr_table.bd3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1, union io_cnr_cspr_reg_sprscrbd_1, SPRSCRBD_4, sprs_ctrl->cr_table.bd4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD2, union io_cnr_cspr_reg_sprscrbd_2, SPRSCRBD_5, sprs_ctrl->cr_table.bd5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_0, sprs_ctrl->cr_table.of0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_1, sprs_ctrl->cr_table.of1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_2, sprs_ctrl->cr_table.of2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1, union io_cnr_cspr_reg_sprscrof_1, SPRSCROF_3, sprs_ctrl->cr_table.of3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF2, union io_cnr_cspr_reg_sprscrof_2, SPRSCROF_4, sprs_ctrl->cr_table.of4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF2, union io_cnr_cspr_reg_sprscrof_2, SPRSCROF_5, sprs_ctrl->cr_table.of5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA1, union io_cnr_cspr_reg_sprscrga_1, SPRSCRGA_0, sprs_ctrl->cr_table.gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA1, union io_cnr_cspr_reg_sprscrga_1, SPRSCRGA_1, sprs_ctrl->cr_table.gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA2, union io_cnr_cspr_reg_sprscrga_2, SPRSCRGA_2, sprs_ctrl->cr_table.gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA2, union io_cnr_cspr_reg_sprscrga_2, SPRSCRGA_3, sprs_ctrl->cr_table.gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA3, union io_cnr_cspr_reg_sprscrga_3, SPRSCRGA_4, sprs_ctrl->cr_table.gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA3, union io_cnr_cspr_reg_sprscrga_3, SPRSCRGA_5, sprs_ctrl->cr_table.gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}



/*
Set CbCr setting of destination
*/
INT32 Im_CNR_OFL_Ctrl_SPRD_CC( UCHAR ch, const T_IM_CNR_CTRL_CC_SRC_DST* const sprd_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Ctrl_SPRD_CC() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( sprd_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Ctrl_SPRD_CC() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OFL_CSPR_REG.SPRD.bit.SPRD = sprd_ctrl->enable;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_1, sprd_ctrl->cb_table.bd1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_2, sprd_ctrl->cb_table.bd2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_3, sprd_ctrl->cb_table.bd3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_4, sprd_ctrl->cb_table.bd4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD2, union io_cnr_cspr_reg_sprdcbbd_2, SPRDCBBD_5, sprd_ctrl->cb_table.bd5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_0, sprd_ctrl->cb_table.of0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_1, sprd_ctrl->cb_table.of1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_2, sprd_ctrl->cb_table.of2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_3, sprd_ctrl->cb_table.of3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF2, union io_cnr_cspr_reg_sprdcbof_2, SPRDCBOF_4, sprd_ctrl->cb_table.of4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF2, union io_cnr_cspr_reg_sprdcbof_2, SPRDCBOF_5, sprd_ctrl->cb_table.of5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA1, union io_cnr_cspr_reg_sprdcbga_1, SPRDCBGA_0, sprd_ctrl->cb_table.gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA1, union io_cnr_cspr_reg_sprdcbga_1, SPRDCBGA_1, sprd_ctrl->cb_table.gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA2, union io_cnr_cspr_reg_sprdcbga_2, SPRDCBGA_2, sprd_ctrl->cb_table.gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA2, union io_cnr_cspr_reg_sprdcbga_2, SPRDCBGA_3, sprd_ctrl->cb_table.gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA3, union io_cnr_cspr_reg_sprdcbga_3, SPRDCBGA_4, sprd_ctrl->cb_table.gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA3, union io_cnr_cspr_reg_sprdcbga_3, SPRDCBGA_5, sprd_ctrl->cb_table.gain5 );

	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_1, sprd_ctrl->cr_table.bd1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_2, sprd_ctrl->cr_table.bd2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_3, sprd_ctrl->cr_table.bd3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_4, sprd_ctrl->cr_table.bd4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD2, union io_cnr_cspr_reg_sprdcrbd_2, SPRDCRBD_5, sprd_ctrl->cr_table.bd5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_0, sprd_ctrl->cr_table.of0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_1, sprd_ctrl->cr_table.of1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_2, sprd_ctrl->cr_table.of2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_3, sprd_ctrl->cr_table.of3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF2, union io_cnr_cspr_reg_sprdcrof_2, SPRDCROF_4, sprd_ctrl->cr_table.of4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF2, union io_cnr_cspr_reg_sprdcrof_2, SPRDCROF_5, sprd_ctrl->cr_table.of5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA1, union io_cnr_cspr_reg_sprdcrga_1, SPRDCRGA_0, sprd_ctrl->cr_table.gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA1, union io_cnr_cspr_reg_sprdcrga_1, SPRDCRGA_1, sprd_ctrl->cr_table.gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA2, union io_cnr_cspr_reg_sprdcrga_2, SPRDCRGA_2, sprd_ctrl->cr_table.gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA2, union io_cnr_cspr_reg_sprdcrga_2, SPRDCRGA_3, sprd_ctrl->cr_table.gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA3, union io_cnr_cspr_reg_sprdcrga_3, SPRDCRGA_4, sprd_ctrl->cr_table.gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA3, union io_cnr_cspr_reg_sprdcrga_3, SPRDCRGA_5, sprd_ctrl->cr_table.gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
Set CbCr setting of destination
*/
INT32 Im_CNR_OTF_Ctrl_SPRD_CC( UCHAR ch, const T_IM_CNR_CTRL_CC_SRC_DST* const sprd_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Ctrl_SPRD_CC() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( sprd_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Ctrl_SPRD_CC() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	IO_CNR(ch)->OTF_CSPR_REG.SPRD.bit.SPRD = sprd_ctrl->enable;

	// write signed data
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_1, sprd_ctrl->cb_table.bd1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_2, sprd_ctrl->cb_table.bd2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_3, sprd_ctrl->cb_table.bd3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1, union io_cnr_cspr_reg_sprdcbbd_1, SPRDCBBD_4, sprd_ctrl->cb_table.bd4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD2, union io_cnr_cspr_reg_sprdcbbd_2, SPRDCBBD_5, sprd_ctrl->cb_table.bd5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_0, sprd_ctrl->cb_table.of0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_1, sprd_ctrl->cb_table.of1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_2, sprd_ctrl->cb_table.of2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1, union io_cnr_cspr_reg_sprdcbof_1, SPRDCBOF_3, sprd_ctrl->cb_table.of3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF2, union io_cnr_cspr_reg_sprdcbof_2, SPRDCBOF_4, sprd_ctrl->cb_table.of4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF2, union io_cnr_cspr_reg_sprdcbof_2, SPRDCBOF_5, sprd_ctrl->cb_table.of5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA1, union io_cnr_cspr_reg_sprdcbga_1, SPRDCBGA_0, sprd_ctrl->cb_table.gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA1, union io_cnr_cspr_reg_sprdcbga_1, SPRDCBGA_1, sprd_ctrl->cb_table.gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA2, union io_cnr_cspr_reg_sprdcbga_2, SPRDCBGA_2, sprd_ctrl->cb_table.gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA2, union io_cnr_cspr_reg_sprdcbga_2, SPRDCBGA_3, sprd_ctrl->cb_table.gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA3, union io_cnr_cspr_reg_sprdcbga_3, SPRDCBGA_4, sprd_ctrl->cb_table.gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA3, union io_cnr_cspr_reg_sprdcbga_3, SPRDCBGA_5, sprd_ctrl->cb_table.gain5 );

	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_1, sprd_ctrl->cr_table.bd1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_2, sprd_ctrl->cr_table.bd2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_3, sprd_ctrl->cr_table.bd3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1, union io_cnr_cspr_reg_sprdcrbd_1, SPRDCRBD_4, sprd_ctrl->cr_table.bd4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD2, union io_cnr_cspr_reg_sprdcrbd_2, SPRDCRBD_5, sprd_ctrl->cr_table.bd5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_0, sprd_ctrl->cr_table.of0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_1, sprd_ctrl->cr_table.of1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_2, sprd_ctrl->cr_table.of2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1, union io_cnr_cspr_reg_sprdcrof_1, SPRDCROF_3, sprd_ctrl->cr_table.of3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF2, union io_cnr_cspr_reg_sprdcrof_2, SPRDCROF_4, sprd_ctrl->cr_table.of4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF2, union io_cnr_cspr_reg_sprdcrof_2, SPRDCROF_5, sprd_ctrl->cr_table.of5 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA1, union io_cnr_cspr_reg_sprdcrga_1, SPRDCRGA_0, sprd_ctrl->cr_table.gain0 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA1, union io_cnr_cspr_reg_sprdcrga_1, SPRDCRGA_1, sprd_ctrl->cr_table.gain1 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA2, union io_cnr_cspr_reg_sprdcrga_2, SPRDCRGA_2, sprd_ctrl->cr_table.gain2 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA2, union io_cnr_cspr_reg_sprdcrga_2, SPRDCRGA_3, sprd_ctrl->cr_table.gain3 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA3, union io_cnr_cspr_reg_sprdcrga_3, SPRDCRGA_4, sprd_ctrl->cr_table.gain4 );
	im_cnr_set_reg_signed( IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA3, union io_cnr_cspr_reg_sprdcrga_3, SPRDCRGA_5, sprd_ctrl->cr_table.gain5 );

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();

	return D_DDIM_OK;
}

/*
CNR(OFL) Start Sync
*/
INT32 Im_CNR_OFL_Start_Sync( UCHAR ch, const INT32 wait_time )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Start_Sync() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	return im_cnr_ofl_start_core( ch, D_IM_CNR_SYNC, wait_time );
}

/*
CNR(OTF) Start Sync
*/
INT32 Im_CNR_OTF_Start_Sync( UCHAR ch, const INT32 wait_time )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Start_Sync() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	return im_cnr_otf_start_core( ch, D_IM_CNR_SYNC, wait_time );
}

/*
CNR(OFL) Start Async
*/
INT32 Im_CNR_OFL_Start_Async( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Start_Async() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	return im_cnr_ofl_start_core( ch, D_IM_CNR_ASYNC, 0 );	// wait_time is dummy.
}

/*
CNR(OTF) Start Async
*/
INT32 Im_CNR_OTF_Start_Async( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Start_Async() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	return im_cnr_otf_start_core( ch, D_IM_CNR_ASYNC, 0 );	// wait_time is dummy.
}

/*
CNR(OFL) Wait End
*/
INT32 Im_CNR_OFL_Wait_End( UCHAR ch, UINT32* const p_wait_factor, const INT32 wait_time )
{
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_FLGPTN waiptn;
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Wait_End() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( p_wait_factor == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OFL_Wait_End() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	if (ch == D_IM_CNR_CH_0) {
		waiptn = D_IM_CNR_OFL_0_WAIT_FLGPTN;
	}
	else if (ch == D_IM_CNR_CH_1) {
		waiptn = D_IM_CNR_OFL_1_WAIT_FLGPTN;
	}
	else {
		waiptn = (D_IM_CNR_OFL_0_WAIT_FLGPTN|D_IM_CNR_OFL_1_WAIT_FLGPTN);
	}
	ercd = DDIM_User_Twai_Flg( FID_IM_CNR, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, wait_time );
	if( ercd == D_DDIM_USER_E_TMOUT ) {
		// A semaphore acquisition processing time out
		Ddim_Print(("Im_CNR_OFL_Wait_End() time out. ercd = %d\n", ercd));
		return D_IM_CNR_SEM_TIMEOUT_ERR;
	}

	if( ercd != D_DDIM_USER_E_OK ) {
		// A semaphore acquisition processing error
		Ddim_Print(("Im_CNR_OFL_Wait_End() system error. ercd = %d\n", ercd));
		return D_IM_CNR_SEM_NG;
	}

	DDIM_User_Clr_Flg( FID_IM_CNR, ~( flgptn & waiptn ) );

	*p_wait_factor = (UINT32)( flgptn & waiptn );

	if( (flgptn & D_IM_CNR_AXI_ERR) != 0 ) {
		return D_IM_CNR_AXI_ERR;
	}

	return D_DDIM_OK;
}

/*
CNR(OTF) Wait End
*/
INT32 Im_CNR_OTF_Wait_End( UCHAR ch, UINT32* const p_wait_factor, const INT32 wait_time )
{
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_FLGPTN waiptn;
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Wait_End() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( p_wait_factor == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_OTF_Wait_End() Parameter is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif

	if (ch == D_IM_CNR_CH_0) {
		waiptn = D_IM_CNR_OTF_0_WAIT_FLGPTN;
	}
	else if (ch == D_IM_CNR_CH_1) {
		waiptn = D_IM_CNR_OTF_1_WAIT_FLGPTN;
	}
	else {
		waiptn = (D_IM_CNR_OTF_0_WAIT_FLGPTN|D_IM_CNR_OTF_1_WAIT_FLGPTN);
	}
	ercd = DDIM_User_Twai_Flg( FID_IM_CNR, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, wait_time );
	if( ercd == D_DDIM_USER_E_TMOUT ) {
		// A semaphore acquisition processing time out
		Ddim_Print(("Im_CNR_OTF_Wait_End() time out. ercd = %d\n", ercd));
		return D_IM_CNR_SEM_TIMEOUT_ERR;
	}

	if( ercd != D_DDIM_USER_E_OK ) {
		// A semaphore acquisition processing error
		Ddim_Print(("Im_CNR_OTF_Wait_End() system error. ercd = %d\n", ercd));
		return D_IM_CNR_SEM_NG;
	}

	DDIM_User_Clr_Flg( FID_IM_CNR, ~( flgptn & waiptn ) );

	*p_wait_factor = (UINT32)( flgptn & waiptn );

	if( (flgptn & D_IM_CNR_AXI_ERR) != 0 ) {
		return D_IM_CNR_AXI_ERR;
	}
	if( (flgptn & D_IM_CNR_DRT_ERR) != 0 ) {
		return D_IM_CNR_DRT_ERR;
	}

	return D_DDIM_OK;
}

/*
CNR(OFL) Stop
*/
INT32 Im_CNR_OFL_Stop( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Stop() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	if( IO_CNR(ch)->OFL_REG_RW.SPRTRG.bit.SPRTRG == 3 ) {
		Im_CNR_OFL_Disable(ch);
#ifdef CO_DEBUG_ON_PC
		if( ch == D_IM_CNR_CH_0 ){
			IO_CNR(ch)->OFL_REG_RW.SPRTRG.bit.SPRTRG = 2;
		}
		else if( ch == D_IM_CNR_CH_1 ){
			IO_CNR(ch)->OFL_REG_RW.SPRTRG.bit.SPRTRG = 2;
		}
		else{
			IO_CNR(D_IM_CNR_CH_0)->OFL_REG_RW.SPRTRG.bit.SPRTRG = 2;
			IO_CNR(D_IM_CNR_CH_1)->OFL_REG_RW.SPRTRG.bit.SPRTRG = 2;
		}
#endif //CO_DEBUG_ON_PC
	}

	// Set Acess Disable
	IO_CNR(ch)->OFL_REG_RW.ACESEN.bit.SPRACS = 0;

	im_CNR_off_spr_clk(ch);
	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();
//	Ddim_Print("I: [SPR] func disable.\n");

	return D_DDIM_OK;
}

/*
CNR(OTF) Stop
*/
INT32 Im_CNR_OTF_Stop( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Stop() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	if( IO_CNR(ch)->OTF_REG_RW.SPRTRG.bit.SPRTRG == 3 ) {
		Im_CNR_OTF_Disable(ch);
#ifdef CO_DEBUG_ON_PC
		if( ch == D_IM_CNR_CH_0 ){
			IO_CNR(ch)->OTF_REG_RW.SPRTRG.bit.SPRTRG = 2;
		}
		else if( ch == D_IM_CNR_CH_1 ){
			IO_CNR(ch)->OTF_REG_RW.SPRTRG.bit.SPRTRG = 2;
		}
		else{
			IO_CNR(D_IM_CNR_CH_0)->OTF_REG_RW.SPRTRG.bit.SPRTRG = 2;
			IO_CNR(D_IM_CNR_CH_1)->OTF_REG_RW.SPRTRG.bit.SPRTRG = 2;
		}
#endif //CO_DEBUG_ON_PC
	}

	// Set Acess Disable
	IO_CNR(ch)->OTF_REG_RW.ACESEN.bit.SPRACS = 0;

	im_CNR_off_spr_clk(ch);
	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();
//	Ddim_Print("I: [SPR] func disable.\n");

	return D_DDIM_OK;
}

/*
The semaphore of CNR(OFL) is returned
*/
INT32 Im_CNR_OFL_Close( UCHAR ch )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Close() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	im_CNR_off_spr_clk(ch);
	Im_CNR_Dsb();

	if (ch == D_IM_CNR_CH_2) {
		for( UCHAR loop = D_IM_CNR_CH_0; loop <= D_IM_CNR_CH_1; loop++ ){
			ercd = DDIM_User_Sig_Sem( SID_IM_CNR_OFL(loop) );				// sig_sem()

			if( D_DDIM_USER_E_OK != ercd ) {
				// SPR processing error
				Ddim_Print(("I:Im_CNR_OFL_Close() Error.\n"));
				return D_IM_CNR_SEM_NG;
			}
		}
	}
	else{
		ercd = DDIM_User_Sig_Sem( SID_IM_CNR_OFL(ch) );				// sig_sem()

		if( D_DDIM_USER_E_OK != ercd ) {
			// SPR processing error
			Ddim_Print(("I:Im_CNR_OFL_Close() Error.\n"));
			return D_IM_CNR_SEM_NG;
		}
	}
	return D_DDIM_OK;
}

/*
The semaphore of CNR(OTF) is returned
*/
INT32 Im_CNR_OTF_Close( UCHAR ch )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Close() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	im_CNR_off_spr_clk(ch);
	Im_CNR_Dsb();

	if (ch == D_IM_CNR_CH_2) {
		for( UCHAR loop = D_IM_CNR_CH_0; loop <= D_IM_CNR_CH_1; loop++ ){
			ercd = DDIM_User_Sig_Sem( SID_IM_CNR_OTF(loop) );				// sig_sem()

			if( D_DDIM_USER_E_OK != ercd ) {
				// SPR processing error
				Ddim_Print(("I:Im_CNR_OTF_Close() Error.\n"));
				return D_IM_CNR_SEM_NG;
			}
		}
	}
	else{
		ercd = DDIM_User_Sig_Sem( SID_IM_CNR_OTF(ch) );				// sig_sem()

		if( D_DDIM_USER_E_OK != ercd ) {
			// SPR processing error
			Ddim_Print(("I:Im_CNR_OTF_Close() Error.\n"));
			return D_IM_CNR_SEM_NG;
		}
	}
	return D_DDIM_OK;
}

/*
CNR(OFL) Reset
*/
INT32 Im_CNR_OFL_Reset( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OFL_Reset() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	Im_CNR_On_Pclk(ch);
	im_CNR_on_spr_clk(ch);
	Im_CNR_Dsb();

	Im_CNR_OFL_Softreset(ch);

	im_CNR_off_spr_clk(ch);
	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();
//	Ddim_Print("I: [SPR] func soft reset.\n");

	return D_DDIM_OK;
}

/*
CNR(OTF) Reset
*/
INT32 Im_CNR_OTF_Reset( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_OTF_Reset() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif
	Im_CNR_On_Pclk(ch);
	im_CNR_on_spr_clk(ch);
	Im_CNR_Dsb();

	Im_CNR_OTF_Softreset(ch);

	im_CNR_off_spr_clk(ch);
	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();
//	Ddim_Print("I: [SPR] func soft reset.\n");

	return D_DDIM_OK;
}

/* Get Latest Address information
 */
INT32 Im_CNR_OTF_Get_Latest_OutAddr( UCHAR ch, ULONG* const latest_addr )
{
#ifdef CO_PARAM_CHECK
	if( latest_addr == NULL ) {
		Ddim_Assertion(("Im_CNR_OTF_Get_Latest_OutAddr() error. latest_addr = NULL\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( ch >= D_IM_CNR_CH_READ_NUM_MAX ){
		Ddim_Assertion(( "Im_CNR_OTF_Get_Latest_OutAddr() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*latest_addr     = gIM_CNR_Latest_Mng[ch].addr;

	return D_DDIM_OK;
}

/*
CNR(OFL) Macro handler
*/
static VOID im_cnr_ofl_int_handler( UCHAR ch )
{
	DDIM_USER_FLGPTN flg;
	union io_cnr_ofl_reg_rw_sprice sprice;
	union io_cnr_ofl_reg_rw_spricf spricf;

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	sprice.word = IO_CNR(ch)->OFL_REG_RW.SPRICE.word;
	spricf.word = IO_CNR(ch)->OFL_REG_RW.SPRICF.word;

	if( (sprice.bit.AXIE != 0) && (spricf.bit.__AXIF != 0) ) {		/* pgr0872 */
		// clear interrupt end register
		im_cnr_ofl_clear_axif(ch);

		// Set End Flag
		if (ch == D_IM_CNR_CH_0) {
			flg = D_IM_CNR_FLG_0_OFL_SPR_AXI_ERR;

			DDIM_User_Set_Flg( FID_IM_CNR, D_IM_CNR_FLG_0_OFL_SPR_AXI_ERR );
		}
		else {
			flg = D_IM_CNR_FLG_1_OFL_SPR_AXI_ERR;

			DDIM_User_Set_Flg( FID_IM_CNR, D_IM_CNR_FLG_1_OFL_SPR_AXI_ERR );
		}

		// User Function Check
		if( gIM_CNR_OFL_CallBack_Func[ch] != NULL ) {
			gIM_CNR_OFL_CallBack_Func[ch]( ch, flg, gIM_CNR_OFL_UserParam[ch] );
		}
	}

	if( (sprice.bit.SPDE != 0) && (spricf.bit.__SPDF != 0) ) {		/* pgr0872 */
		// clear interrupt end register
		im_cnr_ofl_clear_spdf(ch);

		im_CNR_off_spr_clk(ch);

		if (ch == D_IM_CNR_CH_0) {
			flg = D_IM_CNR_FLG_0_OFL_SPR_END;
		}
		else {	// D_IM_CNR_CH_1
			flg = D_IM_CNR_FLG_1_OFL_SPR_END;
		}

		// Set End Flag
		(VOID)DDIM_User_Set_Flg( FID_IM_CNR, flg );

		// User Function Check
		if( gIM_CNR_OFL_CallBack_Func[ch] != NULL ) {
			gIM_CNR_OFL_CallBack_Func[ch]( ch, flg, gIM_CNR_OFL_UserParam[ch] );
		}
	}

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();
}

/*
CNR(OTF) Macro handler
*/
static VOID im_cnr_otf_int_handler( UCHAR ch )
{
	DDIM_USER_FLGPTN flg;
	union io_cnr_otf_reg_rw_sprice sprice;
	union io_cnr_otf_reg_rw_spricf spricf;

	Im_CNR_On_Pclk(ch);
	Im_CNR_Dsb();

	sprice.word = IO_CNR(ch)->OTF_REG_RW.SPRICE.word;
	spricf.word = IO_CNR(ch)->OTF_REG_RW.SPRICF.word;

	if( (sprice.bit.AXIE != 0) && (spricf.bit.__AXIF != 0) ) {		/* pgr0872 */
		// clear interrupt end register
		im_cnr_otf_clear_axif(ch);

		// Set End Flag
		if (ch == D_IM_CNR_CH_0) {
			flg = D_IM_CNR_FLG_0_OTF_SPR_AXI_ERR;

			DDIM_User_Set_Flg( FID_IM_CNR, D_IM_CNR_FLG_0_OTF_SPR_AXI_ERR );
		}
		else {
			flg = D_IM_CNR_FLG_1_OTF_SPR_AXI_ERR;

			DDIM_User_Set_Flg( FID_IM_CNR, D_IM_CNR_FLG_1_OTF_SPR_AXI_ERR );
		}

		// User Function Check
		if( gIM_CNR_OTF_CallBack_Func[ch] != NULL ) {
			gIM_CNR_OTF_CallBack_Func[ch]( ch, flg, gIM_CNR_OTF_UserParam[ch] );
		}
	}

	if( (sprice.bit.DRTE != 0) && (spricf.bit.__DRTF != 0) ) {		/* pgr0872 */
		// clear interrupt end register
		im_cnr_otf_clear_drtf(ch);

		// Set End Flag
		if (ch == D_IM_CNR_CH_0) {
			flg = D_IM_CNR_FLG_0_OTF_SPR_DRT_ERR;

			DDIM_User_Set_Flg( FID_IM_CNR, D_IM_CNR_FLG_0_OTF_SPR_DRT_ERR );
		}
		else {
			flg = D_IM_CNR_FLG_1_OTF_SPR_DRT_ERR;

			DDIM_User_Set_Flg( FID_IM_CNR, D_IM_CNR_FLG_1_OTF_SPR_DRT_ERR );
		}

		// User Function Check
		if( gIM_CNR_OTF_CallBack_Func[ch] != NULL ) {
			gIM_CNR_OTF_CallBack_Func[ch]( ch, flg, gIM_CNR_OTF_UserParam[ch] );
		}
	}

	if( (sprice.bit.SPDE != 0) && (spricf.bit.__SPDF != 0) ) {		/* pgr0872 */
		// clear interrupt end register
		im_cnr_otf_clear_spdf(ch);

//		im_CNR_off_spr_clk(ch);

		// Update latest address
		gIM_CNR_Latest_Mng[ch].addr	= gIM_CNR_Output_Mng[ch].output_addr[gIM_CNR_Latest_Mng[ch].bank_area];
		// Update output address
		++gIM_CNR_Latest_Mng[ch].bank_area;
		gIM_CNR_Latest_Mng[ch].bank_area	= ADDR_BNK_LIMIT( gIM_CNR_Latest_Mng[ch].bank_area, gIM_CNR_Output_Mng[ch].use_bank_num );

		if (ch == D_IM_CNR_CH_0) {
			flg = D_IM_CNR_FLG_0_OTF_SPR_END;
		}
		else{	// D_IM_CNR_CH_1
			flg = D_IM_CNR_FLG_1_OTF_SPR_END;
		}

		// Set End Flag
		(VOID)DDIM_User_Set_Flg( FID_IM_CNR, flg );

		// User Function Check
		if( gIM_CNR_OTF_CallBack_Func[ch] != NULL ) {
			gIM_CNR_OTF_CallBack_Func[ch]( ch, flg, gIM_CNR_OTF_UserParam[ch] );
		}
	}

	Im_CNR_Off_Pclk(ch);
	Im_CNR_Dsb();
}

VOID Im_CNR0_Int_Handler( VOID )
{
	if( gim_cnr_mode[D_IM_CNR_CH_0] == D_IM_CNR_MODE_OFL ){
		im_cnr_ofl_int_handler( D_IM_CNR_CH_0 );
	}
	else{
		im_cnr_otf_int_handler( D_IM_CNR_CH_0 );
	}
}

VOID Im_CNR1_Int_Handler( VOID )
{
	if( gim_cnr_mode[D_IM_CNR_CH_1] == D_IM_CNR_MODE_OFL ){
		im_cnr_ofl_int_handler( D_IM_CNR_CH_1 );
	}
	else{
		im_cnr_otf_int_handler( D_IM_CNR_CH_1 );
	}
}

#ifdef CO_CNR_DEBUG
VOID Im_CNR_Print_ClockStatus( VOID )
{
#ifdef CO_ACT_CNR_PCLK
	Ddim_Print(( "CLKSTOP: AP1=%u\n", Dd_Top_Get_CLKSTOP11_CNR1AP() ));
	Ddim_Print(( "CLKSTOP: AP2=%u\n", Dd_Top_Get_CLKSTOP13_CNR2AP() ));
#endif
#ifdef CO_ACT_CNR_ICLK
	Ddim_Print(( "CLKSTOP: AX1=%u\n", Dd_Top_Get_CLKSTOP11_CNR1AX() ));
	Ddim_Print(( "CLKSTOP: AX2=%u\n", Dd_Top_Get_CLKSTOP13_CNR2AX() ));
#endif
#ifdef CO_ACT_CNRCLK
	Ddim_Print(( "CLKSTOP: CK1=%u\n", Dd_Top_Get_CLKSTOP11_CNR1CK() ));
	Ddim_Print(( "CLKSTOP: CK2=%u\n", Dd_Top_Get_CLKSTOP13_CNR2CK() ));
#endif
#ifdef CO_ACT_CNR_PCLK
	Ddim_Print(( "CLKCNT: AP1=%u\n", gIM_CNR_Pclk_Ctrl_Cnt1 ));
	Ddim_Print(( "CLKCNT: AP2=%u\n", gIM_CNR_Pclk_Ctrl_Cnt2 ));
#endif
#ifdef CO_ACT_CNR_ICLK
	Ddim_Print(( "CLKCNT: AX1=%u\n", gIM_CNR_Iclk_Ctrl_Cnt1 ));
	Ddim_Print(( "CLKCNT: AX2=%u\n", gIM_CNR_Iclk_Ctrl_Cnt2 ));
#endif
#ifdef CO_ACT_CO_ACT_CNR_CLK
	Ddim_Print(( "CLKCNT: CK1=%u\n", gIM_CNR_Clk_Ctrl_Cnt1 ));
	Ddim_Print(( "CLKCNT: CK2=%u\n", gIM_CNR_Clk_Ctrl_Cnt2 ));
#endif
}
#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// --- Ctrl_C (OFL) ---
/*
CNR(OFL) Get SPR CTRL-C setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_Ctrl_C( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CTRL_C** const cnr_ctrl_c, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_Ctrl_C() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cnr_ctrl_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_Ctrl_C() Parameter(cnr_ctrl_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_Ctrl_C() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cnr_ctrl_c		= &gIM_CNR_OFL_Ctrl_C[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CTRL_C );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set SPR CTRL-C setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_Ctrl_C( const T_IM_CNR_CTRL_C* const spr_ctrl_c, T_IM_CNR_RDMA_DATA_CTRL_C* const rdma_ctrl_c )
{
#ifdef CO_PARAM_CHECK
	if( spr_ctrl_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_Ctrl_C() Parameter(spr_ctrl_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_ctrl_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_Ctrl_C() Parameter(rdma_ctrl_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_ctrl_c, 0, sizeof(T_IM_CNR_RDMA_DATA_CTRL_C) );
	im_cnr_convert_rdma_value_ctrl_c( spr_ctrl_c, rdma_ctrl_c );

	return D_DDIM_OK;
}


// --- Ctrl_C (OTF) ---
/*
CNR(OTF) Get SPR CTRL-C setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_Ctrl_C( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CTRL_C** const cnr_ctrl_c, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_Ctrl_C() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cnr_ctrl_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_Ctrl_C() Parameter(cnr_ctrl_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_Ctrl_C() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cnr_ctrl_c		= &gIM_CNR_OTF_Ctrl_C[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CTRL_C );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set SPR CTRL-C setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_Ctrl_C( const T_IM_CNR_CTRL_C* const spr_ctrl_c, T_IM_CNR_RDMA_DATA_CTRL_C* const rdma_ctrl_c )
{
#ifdef CO_PARAM_CHECK
	if( spr_ctrl_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_Ctrl_C() Parameter(spr_ctrl_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_ctrl_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_Ctrl_C() Parameter(rdma_ctrl_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_ctrl_c, 0, sizeof(T_IM_CNR_RDMA_DATA_CTRL_C) );
	im_cnr_convert_rdma_value_ctrl_c( spr_ctrl_c, rdma_ctrl_c );

	return D_DDIM_OK;
}


// --- Ctrl_Y (OFL) ---
/*
CNR(OFL) Get SPR CTRL-Y setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y( UCHAR ch, const T_IM_CNR_RDMA_ADDR_OFL_CTRL_Y** const cnr_ctrl_y, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cnr_ctrl_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y() Parameter(cnr_ctrl_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cnr_ctrl_y		= &gIM_CNR_OFL_Ctrl_Y[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_OFL_CTRL_Y );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set SPR CTRL-Y setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_Ctrl_Y( const T_IM_CNR_OFL_CTRL_Y* const spr_ctrl_y, T_IM_CNR_RDMA_DATA_OFL_CTRL_Y* const rdma_ctrl_y )
{
#ifdef CO_PARAM_CHECK
	if( spr_ctrl_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_Ctrl_Y() Parameter(spr_ctrl_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_ctrl_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_Ctrl_Y() Parameter(rdma_ctrl_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_ctrl_y, 0, sizeof(T_IM_CNR_RDMA_DATA_OFL_CTRL_Y) );

	rdma_ctrl_y->YSPREN.bit.YSPREN			= spr_ctrl_y->enable;
	rdma_ctrl_y->YSPRMODE.bit.YSPRMD		= spr_ctrl_y->threshold_type;
	rdma_ctrl_y->YSPRMODE.bit.YSPRFE		= spr_ctrl_y->level;

	rdma_ctrl_y->YSPRFXEY.bit.YSPRFXEYV		= spr_ctrl_y->v_y_threshold;
	rdma_ctrl_y->YSPRFXEY.bit.YSPRFXEYH		= spr_ctrl_y->h_y_threshold;
	rdma_ctrl_y->YSPRFXEC.bit.YSPRFXECV		= spr_ctrl_y->v_c_threshold;
	rdma_ctrl_y->YSPRFXEC.bit.YSPRFXECH		= spr_ctrl_y->h_c_threshold;
	rdma_ctrl_y->YSPRALPBD.bit.YSPRALPBD	= spr_ctrl_y->alpha_blend_ratio;

	return D_DDIM_OK;
}


// --- YSPR Table (OFL) ---
/*
CNR(OFL) Get YSPR Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_YSPR_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_OFL_YSPR_TBL** const cnr_yspr_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_YSPR_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cnr_yspr_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_YSPR_Table() Parameter(cnr_yspr_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_YSPR_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cnr_yspr_tbl	= &gIM_CNR_OFL_YSPR_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_OFL_YSPR_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set YSPR Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_YSPR_Table( const T_IM_CNR_TABLE* const yspr_table, T_IM_CNR_RDMA_DATA_OFL_YSPR_TBL* const rdma_yspr_table )
{
#ifdef CO_PARAM_CHECK
	if( yspr_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_YSPR_Table() Parameter(yspr_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_yspr_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_YSPR_Table() Parameter(rdma_yspr_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_yspr_table, 0, sizeof(T_IM_CNR_RDMA_DATA_OFL_YSPR_TBL) );
	rdma_yspr_table->YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_1	= yspr_table->bd1;
	rdma_yspr_table->YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_2	= yspr_table->bd2;
	rdma_yspr_table->YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_3	= yspr_table->bd3;
	rdma_yspr_table->YSPRDYEYBD.YSPRDYEYBD1.bit.YSPRDYEYBD_4	= yspr_table->bd4;
	rdma_yspr_table->YSPRDYEYBD.YSPRDYEYBD2.bit.YSPRDYEYBD_5	= yspr_table->bd5;
	rdma_yspr_table->YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_0	= yspr_table->of0;
	rdma_yspr_table->YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_1	= yspr_table->of1;
	rdma_yspr_table->YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_2	= yspr_table->of2;
	rdma_yspr_table->YSPRDYEYOF.YSPRDYEYOF1.bit.YSPRDYEYOF_3	= yspr_table->of3;
	rdma_yspr_table->YSPRDYEYOF.YSPRDYEYOF2.bit.YSPRDYEYOF_4	= yspr_table->of4;
	rdma_yspr_table->YSPRDYEYOF.YSPRDYEYOF2.bit.YSPRDYEYOF_5	= yspr_table->of5;
	// write signed data
	im_cnr_set_reg_signed( rdma_yspr_table->YSPRDYEYGA.YSPRDYEYGA1, union io_cnr_ofl_reg_yspr_ysprdyeyga_1, YSPRDYEYGA_0, yspr_table->gain0 );
	im_cnr_set_reg_signed( rdma_yspr_table->YSPRDYEYGA.YSPRDYEYGA1, union io_cnr_ofl_reg_yspr_ysprdyeyga_1, YSPRDYEYGA_1, yspr_table->gain1 );
	im_cnr_set_reg_signed( rdma_yspr_table->YSPRDYEYGA.YSPRDYEYGA1, union io_cnr_ofl_reg_yspr_ysprdyeyga_1, YSPRDYEYGA_2, yspr_table->gain2 );
	im_cnr_set_reg_signed( rdma_yspr_table->YSPRDYEYGA.YSPRDYEYGA1, union io_cnr_ofl_reg_yspr_ysprdyeyga_1, YSPRDYEYGA_3, yspr_table->gain3 );
	im_cnr_set_reg_signed( rdma_yspr_table->YSPRDYEYGA.YSPRDYEYGA2, union io_cnr_ofl_reg_yspr_ysprdyeyga_2, YSPRDYEYGA_4, yspr_table->gain4 );
	im_cnr_set_reg_signed( rdma_yspr_table->YSPRDYEYGA.YSPRDYEYGA2, union io_cnr_ofl_reg_yspr_ysprdyeyga_2, YSPRDYEYGA_5, yspr_table->gain5 );

	return D_DDIM_OK;
}


// --- CSPR Mid-Y Table (OFL) ---
/*
CNR(OFL) Get CSPR Mid_Y Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL** const cspr_mid_y_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_mid_y_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table() Parameter(cspr_mid_y_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_mid_y_tbl	= &gIM_CNR_OFL_CSPR_MID_Y_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR Mid_Y Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_y, T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL* const rdma_cspr_mid_y_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_mid_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table() Parameter(spr_cspr_mid_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_mid_y_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table() Parameter(rdma_cspr_mid_y_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_mid_y_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL) );
	im_cnr_convert_rdma_cspr_mid_y_table( spr_cspr_mid_y, rdma_cspr_mid_y_table );

	return D_DDIM_OK;
}


// --- CSPR Mid-Y Table (OTF) ---
/*
CNR(OTF) Get CSPR Mid_Y Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL** const cspr_mid_y_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_mid_y_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table() Parameter(cspr_mid_y_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_mid_y_tbl	= &gIM_CNR_OTF_CSPR_MID_Y_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR Mid_Y Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_y, T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL* const rdma_cspr_mid_y_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_mid_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table() Parameter(spr_cspr_mid_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_mid_y_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table() Parameter(rdma_cspr_mid_y_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_mid_y_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL) );
	im_cnr_convert_rdma_cspr_mid_y_table( spr_cspr_mid_y, rdma_cspr_mid_y_table );

	return D_DDIM_OK;
}


// --- CSPR Mid-CC-Y Table (OFL) ---
/*
CNR(OFL) Get CSPR Mid_CC_Y Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL** const cspr_mid_cc_y_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_mid_cc_y_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table() Parameter(cspr_mid_cc_y_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_mid_cc_y_tbl	= &gIM_CNR_OFL_CSPR_MID_CC_Y_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR Mid_CC_Y Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_cc_y, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL* const rdma_cspr_mid_cc_y_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_mid_cc_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_Y_Table() Parameter(spr_cspr_mid_cc_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_mid_cc_y_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_Y_Table() Parameter(rdma_cspr_mid_cc_y_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_mid_cc_y_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL) );
	im_cnr_convert_rdma_cspr_mid_cc_y_table( spr_cspr_mid_cc_y, rdma_cspr_mid_cc_y_table );

	return D_DDIM_OK;
}


// --- CSPR Mid-CC-Y Table (OTF) ---
/*
CNR(OTF) Get CSPR Mid_CC_Y Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL** const cspr_mid_cc_y_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_mid_cc_y_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table() Parameter(cspr_mid_cc_y_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_mid_cc_y_tbl	= &gIM_CNR_OTF_CSPR_MID_CC_Y_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR Mid_CC_Y Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_cc_y, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL* const rdma_cspr_mid_cc_y_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_mid_cc_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_Y_Table() Parameter(spr_cspr_mid_cc_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_mid_cc_y_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_Y_Table() Parameter(rdma_cspr_mid_cc_y_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_mid_cc_y_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL) );
	im_cnr_convert_rdma_cspr_mid_cc_y_table( spr_cspr_mid_cc_y, rdma_cspr_mid_cc_y_table );

	return D_DDIM_OK;
}


// --- CSPR Mid-CC-C Table (OFL) ---
/*
CNR(OFL) Get CSPR Mid_CC_C Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL** const cspr_mid_cc_c_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_mid_cc_c_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table() Parameter(cspr_mid_cc_c_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_mid_cc_c_tbl	= &gIM_CNR_OFL_CSPR_MID_CC_C_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR Mid_CC_C Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_cc_c, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL* const rdma_cspr_mid_cc_c_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_mid_cc_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table() Parameter(spr_cspr_mid_cc_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_mid_cc_c_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table() Parameter(rdma_cspr_mid_cc_c_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_mid_cc_c_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL) );
	im_cnr_convert_rdma_cspr_mid_cc_c_table( spr_cspr_mid_cc_c, rdma_cspr_mid_cc_c_table );

	return D_DDIM_OK;
}


// --- CSPR Mid-CC-C Table (OTF) ---
/*
CNR(OTF) Get CSPR Mid_CC_C Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL** const cspr_mid_cc_c_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_mid_cc_c_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table() Parameter(cspr_mid_cc_c_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_mid_cc_c_tbl	= &gIM_CNR_OTF_CSPR_MID_CC_C_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR Mid_CC_C Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table( const T_IM_CNR_TABLE* const spr_cspr_mid_cc_c, T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL* const rdma_cspr_mid_cc_c_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_mid_cc_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table() Parameter(spr_cspr_mid_cc_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_mid_cc_c_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table() Parameter(rdma_cspr_mid_cc_c_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_mid_cc_c_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL) );
	im_cnr_convert_rdma_cspr_mid_cc_c_table( spr_cspr_mid_cc_c, rdma_cspr_mid_cc_c_table );

	return D_DDIM_OK;
}


// --- CSPR Low-Y Table (OFL) ---
/*
CNR(OFL) Get CSPR Low_Y Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL** const cspr_low_y_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_low_y_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table() Parameter(cspr_low_y_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_low_y_tbl	= &gIM_CNR_OFL_CSPR_LOW_Y_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR Low_Y Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_low_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL* const rdma_cspr_low_y_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_low_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table() Parameter(spr_cspr_low_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_low_y_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table() Parameter(rdma_cspr_low_y_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_low_y_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL) );
	im_cnr_convert_rdma_cspr_low_y_table( spr_cspr_low_y, rdma_cspr_low_y_table );

	return D_DDIM_OK;
}


// --- CSPR Low-Y Table (OTF) ---
/*
CNR(OTF) Get CSPR Low_Y Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL** const cspr_low_y_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_low_y_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table() Parameter(cspr_low_y_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_low_y_tbl	= &gIM_CNR_OTF_CSPR_LOW_Y_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR Low_Y Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_low_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL* const rdma_cspr_low_y_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_low_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table() Parameter(spr_cspr_low_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_low_y_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table() Parameter(rdma_cspr_low_y_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_low_y_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL) );
	im_cnr_convert_rdma_cspr_low_y_table( spr_cspr_low_y, rdma_cspr_low_y_table );

	return D_DDIM_OK;
}


// --- CSPR Low-CC-Y Table (OFL) ---
/*
CNR(OFL) Get CSPR Low_CC_Y Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL** const cspr_low_cc_y_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_low_cc_y_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table() Parameter(cspr_low_y_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_low_cc_y_tbl	= &gIM_CNR_OFL_CSPR_LOW_CC_Y_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR Low_CC_Y Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_low_cc_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL* const rdma_cspr_low_cc_y_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_low_cc_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table() Parameter(spr_cspr_low_cc_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_low_cc_y_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table() Parameter(rdma_cspr_low_cc_y_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_low_cc_y_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL) );
	im_cnr_convert_rdma_cspr_low_cc_y_table( spr_cspr_low_cc_y, rdma_cspr_low_cc_y_table );

	return D_DDIM_OK;
}


// --- CSPR Low-CC-Y Table (OTF) ---
/*
CNR(OTF) Get CSPR Low_CC_Y Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL** const cspr_low_cc_y_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_low_cc_y_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table() Parameter(cspr_low_cc_y_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_low_cc_y_tbl	= &gIM_CNR_OTF_CSPR_LOW_CC_Y_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR Low_CC_Y Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table( const T_IM_CNR_TABLE* const spr_cspr_low_cc_y, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL* const rdma_cspr_low_cc_y_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_low_cc_y == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table() Parameter(spr_cspr_low_cc_y) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_low_cc_y_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table() Parameter(rdma_cspr_low_cc_y_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_low_cc_y_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL) );
	im_cnr_convert_rdma_cspr_low_cc_y_table( spr_cspr_low_cc_y, rdma_cspr_low_cc_y_table );

	return D_DDIM_OK;
}


// --- CSPR Low-CC-C Table (OFL) ---
/*
CNR(OFL) Get CSPR Low_CC_C Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL** const cspr_low_cc_c_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_low_cc_c_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table() Parameter(cspr_low_cc_c_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_low_cc_c_tbl	= &gIM_CNR_OFL_CSPR_LOW_CC_C_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR Low_CC_C Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table( const T_IM_CNR_TABLE* const spr_cspr_low_cc_c, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL* const rdma_cspr_low_cc_c_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_low_cc_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table() Parameter(spr_cspr_low_cc_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_low_cc_c_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table() Parameter(rdma_cspr_low_cc_c_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_low_cc_c_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL) );
	im_cnr_convert_rdma_cspr_low_cc_c_table( spr_cspr_low_cc_c, rdma_cspr_low_cc_c_table );

	return D_DDIM_OK;
}


// --- CSPR Low-CC-C Table (OTF) ---
/*
CNR(OTF) Get CSPR Low_CC_C Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL** const cspr_low_cc_c_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_low_cc_c_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table() Parameter(cspr_low_cc_c_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_low_cc_c_tbl	= &gIM_CNR_OTF_CSPR_LOW_CC_C_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR Low_CC_C Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table( const T_IM_CNR_TABLE* const spr_cspr_low_cc_c, T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL* const rdma_cspr_low_cc_c_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_low_cc_c == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table() Parameter(spr_cspr_low_cc_c) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_low_cc_c_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table() Parameter(rdma_cspr_low_cc_c_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_low_cc_c_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL) );
	im_cnr_convert_rdma_cspr_low_cc_c_table( spr_cspr_low_cc_c, rdma_cspr_low_cc_c_table );

	return D_DDIM_OK;
}


// --- CSPR EDGE Table (OFL) ---
/*
CNR(OFL) Get CSPR Edge Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL** const cspr_edge_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_edge_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table() Parameter(cspr_edge_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_edge_tbl	= &gIM_CNR_OFL_CSPR_EDGE_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR Edge Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table( const T_IM_CNR_TABLE* const spr_cspr_edge, T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL* const rdma_cspr_edge_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_edge == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table() Parameter(spr_cspr_edge) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_edge_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table() Parameter(rdma_cspr_edge_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_edge_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL) );
	im_cnr_convert_rdma_cspr_edge_table( spr_cspr_edge, rdma_cspr_edge_table );

	return D_DDIM_OK;
}


// --- CSPR EDGE Table (OTF) ---
/*
CNR(OTF) Get CSPR Edge Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL** const cspr_edge_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_edge_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table() Parameter(cspr_edge_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_edge_tbl	= &gIM_CNR_OTF_CSPR_EDGE_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR Edge Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table( const T_IM_CNR_TABLE* const spr_cspr_edge, T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL* const rdma_cspr_edge_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_edge == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table() Parameter(spr_cspr_edge) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_edge_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table() Parameter(rdma_cspr_edge_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_edge_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL) );
	im_cnr_convert_rdma_cspr_edge_table( spr_cspr_edge, rdma_cspr_edge_table );

	return D_DDIM_OK;
}


// --- CSPR Hue Specified Table (OFL) ---
/*
CNR(OFL) Get CSPR Hue Specified Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL** const cspr_hue_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_hue_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table() Parameter(cspr_hue_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_hue_tbl	= &gIM_CNR_OFL_CSPR_HUE_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR Hue Specified Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table( const T_IM_CNR_HUE_SPEC_TABLE* const spr_cspr_hue, T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL* const rdma_cspr_hue_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_hue == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table() Parameter(spr_cspr_hue) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_hue_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table() Parameter(rdma_cspr_hue_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_hue_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL) );
	im_cnr_convert_rdma_cspr_hue_table( spr_cspr_hue, rdma_cspr_hue_table );

	return D_DDIM_OK;
}


// --- CSPR Hue Specified Table (OTF) ---
/*
CNR(OTF) Get CSPR Hue Specified Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL** const cspr_hue_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_hue_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table() Parameter(cspr_hue_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_hue_tbl	= &gIM_CNR_OTF_CSPR_HUE_TBL[ch];
	*size			= sizeof( T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR Hue Specified Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table( const T_IM_CNR_HUE_SPEC_TABLE* const spr_cspr_hue, T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL* const rdma_cspr_hue_table )
{
#ifdef CO_PARAM_CHECK
	if( spr_cspr_hue == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table() Parameter(spr_cspr_hue) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_hue_table == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table() Parameter(rdma_cspr_hue_table) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_hue_table, 0, sizeof(T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL) );
	im_cnr_convert_rdma_cspr_hue_table( spr_cspr_hue, rdma_cspr_hue_table );

	return D_DDIM_OK;
}


// --- CSPR SPRS_CC Table (OFL) ---
/*
CNR(OFL) Get CSPR SPRS_CC Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL** const cspr_sprs_cc_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_sprs_cc_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table() Parameter(cspr_sprs_cc_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_sprs_cc_tbl	= &gIM_CNR_OFL_SPRS_CC_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR SPRS_CC Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table( const T_IM_CNR_CTRL_CC_SRC_DST* const cspr_sprs_cc, T_IM_CNR_RDMA_DATA_SPRS_CC_TBL* const rdma_cspr_sprs_cc )
{
#ifdef CO_PARAM_CHECK
	if( cspr_sprs_cc == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table() Parameter(cspr_sprs_cc) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_sprs_cc == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table() Parameter(rdma_cspr_sprs_cc) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_sprs_cc, 0, sizeof(T_IM_CNR_RDMA_DATA_SPRS_CC_TBL) );
	im_cnr_convert_rdma_cspr_sprs_cc( cspr_sprs_cc, rdma_cspr_sprs_cc );

	return D_DDIM_OK;
}


// --- CSPR SPRS_CC Table (OTF) ---
/*
CNR(OTF) Get CSPR SPRS_CC Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL** const cspr_sprs_cc_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_sprs_cc_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table() Parameter(cspr_sprs_cc_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_sprs_cc_tbl	= &gIM_CNR_OTF_SPRS_CC_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR SPRS_CC Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table( const T_IM_CNR_CTRL_CC_SRC_DST* const cspr_sprs_cc, T_IM_CNR_RDMA_DATA_SPRS_CC_TBL* const rdma_cspr_sprs_cc )
{
#ifdef CO_PARAM_CHECK
	if( cspr_sprs_cc == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table() Parameter(cspr_sprs_cc) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_sprs_cc == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table() Parameter(rdma_cspr_sprs_cc) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_sprs_cc, 0, sizeof(T_IM_CNR_RDMA_DATA_SPRS_CC_TBL) );
	im_cnr_convert_rdma_cspr_sprs_cc( cspr_sprs_cc, rdma_cspr_sprs_cc );

	return D_DDIM_OK;
}


// --- CSPR SPRD_CC Table (OFL) ---
/*
CNR(OFL) Get CSPR SPRD_CC Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL** const cspr_sprd_cc_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_sprd_cc_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table() Parameter(cspr_sprd_cc_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_sprd_cc_tbl	= &gIM_CNR_OFL_SPRD_CC_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL );

	return D_DDIM_OK;
}

/*
CNR(OFL) Set CSPR SPRD_CC Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table( const T_IM_CNR_CTRL_CC_SRC_DST* const cspr_sprd_cc, T_IM_CNR_RDMA_DATA_SPRD_CC_TBL* const rdma_cspr_sprd_cc )
{
#ifdef CO_PARAM_CHECK
	if( cspr_sprd_cc == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table() Parameter(cspr_sprd_cc) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_sprd_cc == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table() Parameter(rdma_cspr_sprd_cc) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_sprd_cc, 0, sizeof(T_IM_CNR_RDMA_DATA_SPRD_CC_TBL) );
	im_cnr_convert_rdma_cspr_sprd_cc( cspr_sprd_cc, rdma_cspr_sprd_cc );

	return D_DDIM_OK;
}


// --- CSPR SPRD_CC Table (OTF) ---
/*
CNR(OTF) Get CSPR SPRD_CC Table setting address information
*/
INT32 Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table( UCHAR ch, const T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL** const cspr_sprd_cc_tbl, ULONG* const size )
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_IM_CNR_CH_NUM_MAX) {
		Ddim_Assertion(("Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table() error. input channel error : %d\n", ch));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( cspr_sprd_cc_tbl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table() Parameter(cspr_sprd_cc_tbl) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( size == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table() Parameter(size) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*cspr_sprd_cc_tbl	= &gIM_CNR_OTF_SPRD_CC_TBL[ch];
	*size				= sizeof( T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL );

	return D_DDIM_OK;
}

/*
CNR(OTF) Set CSPR SPRD_CC Table setting parameter information
*/
INT32 Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table( const T_IM_CNR_CTRL_CC_SRC_DST* const cspr_sprd_cc, T_IM_CNR_RDMA_DATA_SPRD_CC_TBL* const rdma_cspr_sprd_cc )
{
#ifdef CO_PARAM_CHECK
	if( cspr_sprd_cc == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table() Parameter(cspr_sprd_cc) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
	if( rdma_cspr_sprd_cc == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table() Parameter(rdma_cspr_sprd_cc) is NULL.\n"));
		return D_IM_CNR_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	memset( rdma_cspr_sprd_cc, 0, sizeof(T_IM_CNR_RDMA_DATA_SPRD_CC_TBL) );
	im_cnr_convert_rdma_cspr_sprd_cc( cspr_sprd_cc, rdma_cspr_sprd_cc );

	return D_DDIM_OK;
}
#endif


