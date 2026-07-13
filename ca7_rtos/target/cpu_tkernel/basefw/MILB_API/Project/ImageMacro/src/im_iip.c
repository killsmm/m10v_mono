/**
 * @file		im_iip.c
 * @brief		JDSIIP4B Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "im_iip.h"

#include "driver_common.h"

//#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
//#endif
#include "dd_arm.h"

#include "ddim_user_custom.h"

#include "jdsiip.h"

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#include "palladium_test.h"
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

#include <string.h>

//#define CO_IIP_DEBUG
//#define CO_DEBUG_PRINT_IM_IIP
//#define CO_IIP_DEBUG_PRINT_FUNC
//#define CO_IIP_REG_TYPE_CHECK
//#define CO_IIP_DEBUG_CLK_DISABLE

#ifdef CO_DEBUG_ON_PC
#undef CO_IIP_DEBUG
#undef CO_DEBUG_PRINT_IM_IIP
#undef CO_IIP_DEBUG_PRINT_FUNC
#undef CO_IIP_REG_TYPE_CHECK
#undef CO_IIP_DEBUG_CLK_DISABLE
#endif


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Low power consumption option
#ifndef CO_IIP_DEBUG_CLK_DISABLE
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_IMAGE_CLOCK)
#	define CO_ACT_IIPCLK
#endif
#if defined(CO_ACT_ICLOCK) || defined(CO_ACT_IMAGE_ICLOCK)
#	define CO_ACT_IIP_ICLK
#endif
#if defined(CO_ACT_HCLOCK) || defined(CO_ACT_IMAGE_HCLOCK)
#	define CO_ACT_IIP_HCLK
#endif
#if defined(CO_ACT_PCLOCK) || defined(CO_ACT_IMAGE_PCLOCK)
#	define CO_ACT_IIP_PCLK
#endif
#endif

// Debug option
#ifdef CO_IIP_DEBUG
#	ifndef CO_PARAM_CHECK
#		define CO_PARAM_CHECK
#	endif
#	ifndef CO_IIP_DEBUG_CLK_DISABLE
#		ifndef CO_ACT_IIPCLK
#			define CO_ACT_IIPCLK
#		endif
#		ifndef CO_ACT_IIP_HCLK
#			define CO_ACT_IIP_HCLK
#		endif
#		ifndef CO_ACT_IIP_PCLK
#			define CO_ACT_IIP_PCLK
#		endif
#	endif
#endif

//#ifdef CO_DEBUG_ON_PC
//#define CO_IIP_DEBUG_PRINT_FUNC
//#endif
#ifdef CO_IIP_DEBUG_PRINT_FUNC
#define Im_IIP_Print_Pcdbg	Ddim_Print
#else
#define Im_IIP_Print_Pcdbg(dmy)
#endif

#define Im_IIP_Dmb()	Dd_ARM_Dmb_Pou()
#define Im_IIP_Dsb()	Dd_ARM_Dsb_Pou()

#define D_IM_IIP_TRIG_STOP_CHK_RETRY_COUNT	(1000)

#define	D_IM_IIP_TRUE		(1)
#define	D_IM_IIP_FALSE		(0)

// Maximum value
#define	D_IM_IIP_MAX_PIXID			(12)
#define D_IM_IIP_MAX_LUTRAM_TYPE	(6)			// A~F
#define D_IM_IIP_MAX_LUTRAM_RESOL	(1024)		// 0~1023

// IZACTL1.bit.TRG
#define	D_IM_IIP_STOP				(0)
#define	D_IM_IIP_START				(1)
#define	D_IM_IIP_STOPPED			(2)
#define	D_IM_IIP_RUNNING			(3)

// UNITINFTBL0.bit.SWTRG
#define	D_IM_IIP_SWTRG_START		(1)
#define	D_IM_IIP_SWTRG_STOPPED		(2)
#define	D_IM_IIP_SWTRG_RUNNING		(3)

// INTIZ0, INTIZ1, INTIZ2 Register bits
#define	D_IM_IIP_IZAFLG_BIT			(1<<0)
#define	D_IM_IIP_AXEFLG_BIT			(1<<2)
#define	D_IM_IIP_SL0FF_BIT			(1<<0)
#define	D_IM_IIP_SL1FF_BIT			(1<<1)
#define	D_IM_IIP_SL2FF_BIT			(1<<2)
#define	D_IM_IIP_SL3FF_BIT			(1<<3)
#define	D_IM_IIP_SL4FF_BIT			(1<<4)
#define	D_IM_IIP_SL5FF_BIT			(1<<5)
#define	D_IM_IIP_SL6FF_BIT			(1<<6)
#define	D_IM_IIP_SL7FF_BIT			(1<<7)
#define	D_IM_IIP_SL8FF_BIT			(1<<8)
#define	D_IM_IIP_SL0MF_BIT			(1<<16)
#define	D_IM_IIP_SL1MF_BIT			(1<<17)
#define	D_IM_IIP_SL2MF_BIT			(1<<18)
#define	D_IM_IIP_SL3MF_BIT			(1<<19)
#define	D_IM_IIP_SL4MF_BIT			(1<<20)
#define	D_IM_IIP_SL5MF_BIT			(1<<21)
#define	D_IM_IIP_SL6MF_BIT			(1<<22)
#define	D_IM_IIP_SL7MF_BIT			(1<<23)
#define	D_IM_IIP_SL8MF_BIT			(1<<24)
#define	D_IM_IIP_REAF0F_BIT			(1<<0)
#define	D_IM_IIP_REAF1F_BIT			(1<<1)
#define	D_IM_IIP_REAF2F_BIT			(1<<2)
#define	D_IM_IIP_REAF3F_BIT			(1<<3)
#define	D_IM_IIP_REFR0F_BIT			(1<<4)
#define	D_IM_IIP_REFR1F_BIT			(1<<5)
#define	D_IM_IIP_REFLTF_BIT			(1<<6)

// Event flag
#define	D_IM_IIP_FLG_USED_BIT_PATTERN		(0x7FFFFFF)

// INTEN0, INTEN1, INTEN2 Register bits
#define	D_IM_IIP_IZAEN_BIT			D_IM_IIP_IZAFLG_BIT
#define	D_IM_IIP_AXEEN_BIT			D_IM_IIP_AXEFLG_BIT
#define	D_IM_IIP_SL0FEN_BIT			D_IM_IIP_SL0FF_BIT
#define	D_IM_IIP_SL1FEN_BIT			D_IM_IIP_SL1FF_BIT
#define	D_IM_IIP_SL2FEN_BIT			D_IM_IIP_SL2FF_BIT
#define	D_IM_IIP_SL3FEN_BIT			D_IM_IIP_SL3FF_BIT
#define	D_IM_IIP_SL4FEN_BIT			D_IM_IIP_SL4FF_BIT
#define	D_IM_IIP_SL5FEN_BIT			D_IM_IIP_SL5FF_BIT
#define	D_IM_IIP_SL6FEN_BIT			D_IM_IIP_SL6FF_BIT
#define	D_IM_IIP_SL7FEN_BIT			D_IM_IIP_SL7FF_BIT
#define	D_IM_IIP_SL8FEN_BIT			D_IM_IIP_SL8FF_BIT
#define	D_IM_IIP_SL0MEN_BIT			D_IM_IIP_SL0MF_BIT
#define	D_IM_IIP_SL1MEN_BIT			D_IM_IIP_SL1MF_BIT
#define	D_IM_IIP_SL2MEN_BIT			D_IM_IIP_SL2MF_BIT
#define	D_IM_IIP_SL3MEN_BIT			D_IM_IIP_SL3MF_BIT
#define	D_IM_IIP_SL4MEN_BIT			D_IM_IIP_SL4MF_BIT
#define	D_IM_IIP_SL5MEN_BIT			D_IM_IIP_SL5MF_BIT
#define	D_IM_IIP_SL6MEN_BIT			D_IM_IIP_SL6MF_BIT
#define	D_IM_IIP_SL7MEN_BIT			D_IM_IIP_SL7MF_BIT
#define	D_IM_IIP_SL8MEN_BIT			D_IM_IIP_SL8MF_BIT
#define	D_IM_IIP_REAF0E_BIT			D_IM_IIP_REAF0F_BIT
#define	D_IM_IIP_REAF1E_BIT			D_IM_IIP_REAF1F_BIT
#define	D_IM_IIP_REAF2E_BIT			D_IM_IIP_REAF2F_BIT
#define	D_IM_IIP_REAF3E_BIT			D_IM_IIP_REAF3F_BIT
#define	D_IM_IIP_REFR0E_BIT			D_IM_IIP_REFR0F_BIT
#define	D_IM_IIP_REFR1E_BIT			D_IM_IIP_REFR1F_BIT
#define	D_IM_IIP_REFLTE_BIT			D_IM_IIP_REFLTF_BIT

// HISTGRAM
#define D_IM_IIP_NUM_HISTGRAM		(256)
#define D_IM_IIP_HISTGRAM_BYTES		(D_IM_IIP_NUM_HISTGRAM *4)

// Unit Parameter [debug function]
#define	D_IM_IIP_UNITPARAM_SIZE_LD		(0x040)
#define	D_IM_IIP_UNITPARAM_SIZE_CFL		(0x020)
#define	D_IM_IIP_UNITPARAM_SIZE_GPC		(0x01C)
#define	D_IM_IIP_UNITPARAM_SIZE_CSC		(0x060)
#define	D_IM_IIP_UNITPARAM_SIZE_AFN		(0x070)
#define	D_IM_IIP_UNITPARAM_SIZE_FRECT	(0x070)
#define	D_IM_IIP_UNITPARAM_SIZE_MON		(0x060)
#define	D_IM_IIP_UNITPARAM_SIZE_BLEND	(0x0DC)
#define	D_IM_IIP_UNITPARAM_SIZE_SL		(0x3A0)
#define	D_IM_IIP_UNITPARAM_SIZE_LUT		(0x0E0)
#define	D_IM_IIP_UNITPARAM_SIZE_FLT		(0x240)
#define	D_IM_IIP_UNITPARAM_SIZE_MFT		(0x020)
#define	D_IM_IIP_UNITPARAM_SIZE_ERROR	(0x000)

#define	D_IM_IIP_UNITPARAM_BASEADDR		(0x10000)

#define D_IM_IIP_SRAM_WAIT_USEC			(1)

#define D_IM_IIP_NUM_CPU_CORES			(4)
#define D_IM_IIP_RECURSIVE_COUNT_MAX	(0xFFFFFFFF)

// The structure type matching check for im_iip_get_reg_signed_a().
#ifdef CO_IIP_REG_TYPE_CHECK
#define im_iip_check_reg_type( dst, src )	(dst) = (src);
#else
#define im_iip_check_reg_type( dst, src )
#endif

// Get signed Bit field value to signed variable.
#define im_xxx_get_reg_signed( val, reg, type, member )		{ \
			type work = { .word = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_iip_check_reg_type( (reg), work ); /* The structure type matching check. */ \
		}

#define im_iip_get_reg_signed_a( val, reg, type, member )		{ \
			type work = { .word[0] = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_iip_check_reg_type( (reg), work ); /* The structure type matching check. */ \
		}

// Set signed Bit field value from signed variable.
#define im_iip_set_reg_signed_a( reg, type, member, val )		\
		{ \
			type work = { .word[0] = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_iip_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

// Interrupt parameter table
typedef struct {
	ULONG				flg_bitmask;		// Bitmask of INTIZ0/INTIZ1/INTIZ2.
	ULONG				ena_bitmask;		// Bitmask of INTEN0/INTEN1/INTEN2.
	ULONG				int_fact;			// Interrupt factor bitmask. (parameter "interrupt_bitmask" of Im_IIP_Set_Interrupt() and Im_IIP_Set_IntHandler())
	ULLONG				unitid_bitmask;		// PLDUNIT bitmask. (D_IM_IIP_PARAM_PLDUNIT_xxx)
	T_IM_IIP_CALLBACK	callback_func;		// Software trigger's Callback function.
} T_IM_IIP_INTFLG_TBL;

// Software triggerd unit management structure
typedef struct {
	ULLONG				unit_bitmask;						// Using unit bitmask. (parameter "unitid_bitmask" of Im_IIP_Open_SWTRG())
	ULONG				pixfmttbl_bitmask;					// Using PIXFMTTBL bitmask. (parameter "pixid_bitmask" of Im_IIP_Open_SWTRG())
	ULONG				res_bitmask;						// Using resource bitmask. (parameter "open_res_bitmask" of Im_IIP_Open_SWTRG())
	UINT32				cache_users[D_IM_IIP_CACHE_MAXNUM];	// Using users count of Image Cache.
} T_IM_IIP_SWTRG_MNG;

// parameter structure of im_iip_calc_timeout()
typedef struct {
	DDIM_USER_TMO		last_wait_time;		// "wait_time" for next OS serivce call (ex. DDIM_User_Twai_Flg()).
	DDIM_USER_TMO		wait_time;			// "wait_time" parameter of MILB_API functions.
	DDIM_USER_SYSTIM	start_time;			// System time when start MILB_API functions.
	DDIM_USER_SYSTIM	cur_time;			// Now System time.
} T_IM_IIP_TIMEOUT;

// 64bit bit mask separated to 32bit x2.
typedef union {
	ULLONG	bit64;
	struct {
		ULONG	lower;
		ULONG	upper;
	}bit32;
	struct {
		DDIM_USER_FLGPTN	lower;
		DDIM_USER_FLGPTN	upper;
	}flgptn;
} U_IM_IIP_64BIT_MASK;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_IIPCLK
static volatile UCHAR gIM_IIP_IIPCLK_Counter  = 0;
#endif
#ifdef CO_ACT_IIP_ICLK
static volatile UCHAR gIM_IIP_ICLK_Counter    = 0;
#endif
#ifdef CO_ACT_IIP_PCLK
static volatile UCHAR gIM_IIP_PCLK_Counter    = 0;
#endif
#ifdef CO_ACT_IIP_HCLK
static volatile UCHAR gIM_IIP_HCLK_Counter    = 0;
#endif

// clock off control is disabled(for register dump on debugger)
#ifdef CO_ACT_IIPCLK
static volatile UINT32 gIM_IIP_Iipclk_Ctrl_Disable = 0;
#endif // CO_ACT_IIPCLK
#ifdef CO_ACT_IIP_ICLK
static volatile UINT32 gIM_IIP_Iclk_Ctrl_Disable = 0;
#endif // CO_ACT_IIP_ICLK
#ifdef CO_ACT_IIP_PCLK
static volatile UINT32 gIM_IIP_Pclk_Ctrl_Disable = 0;
#endif // CO_ACT_IIP_PCLK
#ifdef CO_ACT_IIP_HCLK
static volatile UINT32 gIM_IIP_Hclk_Ctrl_Disable = 0;
#endif // CO_ACT_IIP_HCLK

// ACCESSEN control counter
static volatile UINT32 gIM_IIP_Accen0_Izarac_Counter = 0;

/** Software triggerd unit management */
static volatile T_IM_IIP_SWTRG_MNG gIM_IIP_SwtrgMng = {
	.unit_bitmask = 0ULL,
	.pixfmttbl_bitmask = 0,
	.res_bitmask = 0,
	.cache_users = {
		0, 0,
	},
};

/** Interrupt Callback function pointer for hardware trigger mode */
static volatile T_IM_IIP_CALLBACK gIM_IIP_Callback = NULL;

/** Interrupt parameter table of INTIZ0/INTEN0 */
static volatile T_IM_IIP_INTFLG_TBL gIM_IIP_Inttbl_0[] = {
	{ D_IM_IIP_IZAFLG_BIT, D_IM_IIP_IZAEN_BIT, D_IM_IIP_INT_FACTOR_IZAEND, 0, NULL },
	{ D_IM_IIP_AXEFLG_BIT, D_IM_IIP_AXEEN_BIT, D_IM_IIP_INT_FACTOR_AXIERR, 0, NULL },
};

/** Interrupt parameter table of INTIZ1/INTEN1 */
static volatile T_IM_IIP_INTFLG_TBL gIM_IIP_Inttbl_1[] = {
	{ D_IM_IIP_SL0FF_BIT, D_IM_IIP_SL0FEN_BIT, D_IM_IIP_INT_FACTOR_SL0END, D_IM_IIP_PARAM_PLDUNIT_SL0, NULL },
	{ D_IM_IIP_SL1FF_BIT, D_IM_IIP_SL1FEN_BIT, D_IM_IIP_INT_FACTOR_SL1END, D_IM_IIP_PARAM_PLDUNIT_SL1, NULL },
	{ D_IM_IIP_SL2FF_BIT, D_IM_IIP_SL2FEN_BIT, D_IM_IIP_INT_FACTOR_SL2END, D_IM_IIP_PARAM_PLDUNIT_SL2, NULL },
	{ D_IM_IIP_SL3FF_BIT, D_IM_IIP_SL3FEN_BIT, D_IM_IIP_INT_FACTOR_SL3END, D_IM_IIP_PARAM_PLDUNIT_SL3, NULL },
	{ D_IM_IIP_SL4FF_BIT, D_IM_IIP_SL4FEN_BIT, D_IM_IIP_INT_FACTOR_SL4END, D_IM_IIP_PARAM_PLDUNIT_SL4, NULL },
	{ D_IM_IIP_SL5FF_BIT, D_IM_IIP_SL5FEN_BIT, D_IM_IIP_INT_FACTOR_SL5END, D_IM_IIP_PARAM_PLDUNIT_SL5, NULL },
	{ D_IM_IIP_SL6FF_BIT, D_IM_IIP_SL6FEN_BIT, D_IM_IIP_INT_FACTOR_SL6END, D_IM_IIP_PARAM_PLDUNIT_SL6, NULL },
	{ D_IM_IIP_SL7FF_BIT, D_IM_IIP_SL7FEN_BIT, D_IM_IIP_INT_FACTOR_SL7END, D_IM_IIP_PARAM_PLDUNIT_SL7, NULL },
	{ D_IM_IIP_SL8FF_BIT, D_IM_IIP_SL8FEN_BIT, D_IM_IIP_INT_FACTOR_SL8END, D_IM_IIP_PARAM_PLDUNIT_SL8, NULL },
	{ D_IM_IIP_SL0MF_BIT, D_IM_IIP_SL0MEN_BIT, D_IM_IIP_INT_FACTOR_SL0_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL0, NULL },
	{ D_IM_IIP_SL1MF_BIT, D_IM_IIP_SL1MEN_BIT, D_IM_IIP_INT_FACTOR_SL1_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL1, NULL },
	{ D_IM_IIP_SL2MF_BIT, D_IM_IIP_SL2MEN_BIT, D_IM_IIP_INT_FACTOR_SL2_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL2, NULL },
	{ D_IM_IIP_SL3MF_BIT, D_IM_IIP_SL3MEN_BIT, D_IM_IIP_INT_FACTOR_SL3_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL3, NULL },
	{ D_IM_IIP_SL4MF_BIT, D_IM_IIP_SL4MEN_BIT, D_IM_IIP_INT_FACTOR_SL4_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL4, NULL },
	{ D_IM_IIP_SL5MF_BIT, D_IM_IIP_SL5MEN_BIT, D_IM_IIP_INT_FACTOR_SL5_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL5, NULL },
	{ D_IM_IIP_SL6MF_BIT, D_IM_IIP_SL6MEN_BIT, D_IM_IIP_INT_FACTOR_SL6_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL6, NULL },
	{ D_IM_IIP_SL7MF_BIT, D_IM_IIP_SL7MEN_BIT, D_IM_IIP_INT_FACTOR_SL7_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL7, NULL },
	{ D_IM_IIP_SL8MF_BIT, D_IM_IIP_SL8MEN_BIT, D_IM_IIP_INT_FACTOR_SL8_LINEEND, D_IM_IIP_PARAM_PLDUNIT_SL8, NULL },
};

#if 0	// INTIZ2/INTEN2 delete
/** Interrupt parameter table of INTIZ2/INTEN2 */
static volatile T_IM_IIP_INTFLG_TBL gIM_IIP_Inttbl_2[] = {
	{ D_IM_IIP_REAF0F_BIT, D_IM_IIP_REAF0E_BIT, D_IM_IIP_INT_FACTOR_RINGFULL_AFN0, D_IM_IIP_PARAM_PLDUNIT_AFN0, NULL },
	{ D_IM_IIP_REAF1F_BIT, D_IM_IIP_REAF1E_BIT, D_IM_IIP_INT_FACTOR_RINGFULL_AFN1, D_IM_IIP_PARAM_PLDUNIT_AFN1, NULL },
	{ D_IM_IIP_REAF2F_BIT, D_IM_IIP_REAF2E_BIT, D_IM_IIP_INT_FACTOR_RINGFULL_AFN2, D_IM_IIP_PARAM_PLDUNIT_AFN2, NULL },
	{ D_IM_IIP_REAF3F_BIT, D_IM_IIP_REAF3E_BIT, D_IM_IIP_INT_FACTOR_RINGFULL_AFN3, D_IM_IIP_PARAM_PLDUNIT_AFN3, NULL },
	{ D_IM_IIP_REFR0F_BIT, D_IM_IIP_REFR0E_BIT, D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0, D_IM_IIP_PARAM_PLDUNIT_FRECT0, NULL },
	{ D_IM_IIP_REFR1F_BIT, D_IM_IIP_REFR1E_BIT, D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1, D_IM_IIP_PARAM_PLDUNIT_FRECT1, NULL },
	{ D_IM_IIP_REFLTF_BIT, D_IM_IIP_REFLTE_BIT, D_IM_IIP_INT_FACTOR_RINGFULL_FLT, D_IM_IIP_PARAM_PLDUNIT_FLT, NULL },
};
#endif	// INTIZ2/INTEN2 delete

typedef struct {
	E_IM_IIP_UNIT_ID	unit_id;
	const CHAR*			unit_name;
} T_IM_IIP_DUMP_UNIT_NAME;

// Unit name to Unit ID (for Im_IIP_Start_DumpUnitReg_ByName())
static const T_IM_IIP_DUMP_UNIT_NAME gIM_IIP_Dump_Unit_Name[] = {
	{ .unit_id = E_IM_IIP_UNIT_ID_LD0,		.unit_name = "ld0", },
	{ .unit_id = E_IM_IIP_UNIT_ID_LD1,		.unit_name = "ld1", },
	{ .unit_id = E_IM_IIP_UNIT_ID_LD2,		.unit_name = "ld2", },
	{ .unit_id = E_IM_IIP_UNIT_ID_LD3,		.unit_name = "ld3", },
	{ .unit_id = E_IM_IIP_UNIT_ID_FLT,		.unit_name = "flt", },
	{ .unit_id = E_IM_IIP_UNIT_ID_AFN0,		.unit_name = "afn0", },
	{ .unit_id = E_IM_IIP_UNIT_ID_AFN1,		.unit_name = "afn1", },
	{ .unit_id = E_IM_IIP_UNIT_ID_AFN2,		.unit_name = "afn2", },
	{ .unit_id = E_IM_IIP_UNIT_ID_FRECT0,	.unit_name = "frect0", },
	{ .unit_id = E_IM_IIP_UNIT_ID_MON,		.unit_name = "mon", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL0,		.unit_name = "sl0", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL1,		.unit_name = "sl1", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL2,		.unit_name = "sl2", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL3,		.unit_name = "sl3", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL4,		.unit_name = "sl4", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL5,		.unit_name = "sl5", },
	{ .unit_id = E_IM_IIP_UNIT_ID_CSC0,		.unit_name = "csc0", },
	{ .unit_id = E_IM_IIP_UNIT_ID_CSC1,		.unit_name = "csc1", },
	{ .unit_id = E_IM_IIP_UNIT_ID_LUT,		.unit_name = "lut", },
	{ .unit_id = E_IM_IIP_UNIT_ID_GPC,		.unit_name = "gpc", },
	{ .unit_id = E_IM_IIP_UNIT_ID_BLEND0,	.unit_name = "blend0", },
	{ .unit_id = E_IM_IIP_UNIT_ID_BLEND1,	.unit_name = "blend1", },
	{ .unit_id = E_IM_IIP_UNIT_ID_FRECT1,	.unit_name = "frect1", },
	{ .unit_id = E_IM_IIP_UNIT_ID_CFL0,		.unit_name = "cfl0", },
	{ .unit_id = E_IM_IIP_UNIT_ID_CSC2,		.unit_name = "csc2", },
	{ .unit_id = E_IM_IIP_UNIT_ID_CSC3,		.unit_name = "csc3", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL6,		.unit_name = "sl6", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL7,		.unit_name = "sl7", },
	{ .unit_id = E_IM_IIP_UNIT_ID_SL8,		.unit_name = "sl8", },
	{ .unit_id = E_IM_IIP_UNIT_ID_LD4,		.unit_name = "ld4", },
	{ .unit_id = E_IM_IIP_UNIT_ID_LD5,		.unit_name = "ld5", },
	{ .unit_id = E_IM_IIP_UNIT_ID_LD6,		.unit_name = "ld6", },
	{ .unit_id = E_IM_IIP_UNIT_ID_LD7,		.unit_name = "ld7", },
	{ .unit_id = E_IM_IIP_UNIT_ID_AFN3,		.unit_name = "afn3", },
	{ .unit_id = E_IM_IIP_UNIT_ID_MFT,		.unit_name = "mft", },
};

// Unit register memory space.
const volatile U_IM_IIP_PARAM_UNITDEBUG* const gIM_IIP_UnitDebugReg = (U_IM_IIP_PARAM_UNITDEBUG*)IO_IIP.UNITDEBUG;

static ULONG  gIM_IIP_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));
static volatile UINT32 gIM_IIP_Cretseq_Recursive_Count[ D_IM_IIP_NUM_CPU_CORES ] = { 0, 0, 0, 0 };


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// wait usec
#define im_iip_wait_usec( usec )				Dd_ARM_Wait_ns( (usec) * 1000 )


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#ifdef CO_PARAM_CHECK
/**
@brief		Check wait_time
@param[in]	wait_time			wait time
@retval		D_IM_IIP_TRUE		wait_time is valid.
@retval		D_IM_IIP_FALSE		wait_time is invalid.
*/
static INT32 im_iip_check_wait_time( const INT32 wait_time )
{
	if( wait_time >= D_DDIM_USER_SEM_WAIT_FEVR ) {
		return D_IM_IIP_TRUE;
	}
	else {
		return D_IM_IIP_FALSE;
	}
}
#endif	// CO_PARAM_CHECK

UINT32 im_iip_irq_disable( void )
{
	UINT32	recursive_count;
	UINT32	lock_id = 0;
	ULONG	cpu_id;

	lock_id = (UINT32)Dd_ARM_DI();
	cpu_id = Dd_ARM_Get_CPU_ID();

	if ( cpu_id >= D_IM_IIP_NUM_CPU_CORES ) {
		while ( 1 ) {	// Assersion
			;
		}
	}

	if ( gIM_IIP_Cretseq_Recursive_Count[cpu_id] >= D_IM_IIP_RECURSIVE_COUNT_MAX ) {
		while ( 1 ) {	// Assersion
			;
		}
	}

	recursive_count = gIM_IIP_Cretseq_Recursive_Count[ cpu_id ];
	gIM_IIP_Cretseq_Recursive_Count[ cpu_id ]++;

	if ( recursive_count == 0 ) {
		Dd_ARM_Spin_Lock( &gIM_IIP_Spin_Lock );
	}

	return lock_id;
}

void im_iip_irq_enable( UINT32 irq_mask )
{
	UINT32	recursive_count;
	ULONG	cpu_id;

	cpu_id = Dd_ARM_Get_CPU_ID();
	if ( cpu_id >= D_IM_IIP_NUM_CPU_CORES ) {
		while ( 1 ) {	// Assersion
			;
		}
	}

	if ( gIM_IIP_Cretseq_Recursive_Count[ cpu_id ] == 0 ) {
		while ( 1 ) {	// Assersion
			;
		}
	}

	recursive_count = gIM_IIP_Cretseq_Recursive_Count[ cpu_id ];
	gIM_IIP_Cretseq_Recursive_Count[ cpu_id ]--;

	if ( recursive_count == 1 ) {
		Dd_ARM_Spin_Unlock( &gIM_IIP_Spin_Lock );
	}
	Dd_ARM_EI( (UINT32)irq_mask );
}

/**
@brief		IZARAC, CAxRAC ON
*/
static VOID im_iip_On_izarac_caxrac( VOID )
{
	UINT32 irq_mask;

	irq_mask = im_iip_irq_disable();

	if( gIM_IIP_Accen0_Izarac_Counter < 0xFFFFFFFF ) {
		IO_IIP.ACESEN0.bit.IZARAC = 1;
		IO_IIP.ACESEN0.bit.CA1RAC = 1;
		IO_IIP.ACESEN0.bit.CA2RAC = 1;

		gIM_IIP_Accen0_Izarac_Counter++;
	}

	im_iip_irq_enable( irq_mask );
}

/**
@brief		IZARAC, CAxRAC OFF
*/
static VOID im_iip_Off_izarac_caxrac( VOID )
{
#ifdef CO_ACT_IIPCLK
	UINT32 irq_mask;

	irq_mask = im_iip_irq_disable();

	if( gIM_IIP_Accen0_Izarac_Counter > 0 ) {
		gIM_IIP_Accen0_Izarac_Counter--;

		if( gIM_IIP_Accen0_Izarac_Counter == 0 ) {
			IO_IIP.ACESEN0.bit.CA2RAC = 0;
			IO_IIP.ACESEN0.bit.CA1RAC = 0;
			IO_IIP.ACESEN0.bit.IZARAC = 0;
		}
	}

	im_iip_irq_enable( irq_mask );
#endif
}

/**
@brief		IIPCK ON
*/
static VOID im_iip_On_Iipclk( VOID )
{
#ifdef CO_ACT_IIPCLK
	if( gIM_IIP_IIPCLK_Counter < 255 ) {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_IIP_IIPCLK_Counter, &Dd_Top_Get_CLKSTOP3(), ~D_DD_TOP_IIPCK_BIT );
#ifdef CO_DEBUG_ON_PC
		Dd_Top_Get_CLKSTOP3() &= ~D_DD_TOP_IIPCK_BIT;
#endif
	}
#endif
}

/**
@brief		IIPCK OFF
*/
static VOID im_iip_Off_Iipclk( VOID )
{
#ifdef CO_ACT_IIPCLK
	UINT32 irq_mask;

	irq_mask = im_iip_irq_disable();

	if( gIM_IIP_IIPCLK_Counter == 1 ) {
		if( gIM_IIP_Iipclk_Ctrl_Disable == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_IIP_IIPCLK_Counter, &Dd_Top_Get_CLKSTOP3(), D_DD_TOP_IIPCK_BIT );
#ifdef CO_DEBUG_ON_PC
			Dd_Top_Get_CLKSTOP3() |= D_DD_TOP_IIPCK_BIT;
#endif
		}
	}
	else {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_IIP_IIPCLK_Counter, &Dd_Top_Get_CLKSTOP3(), D_DD_TOP_IIPCK_BIT );
#ifdef CO_DEBUG_ON_PC
		Dd_Top_Get_CLKSTOP3() |= D_DD_TOP_IIPCK_BIT;
#endif
	}

	im_iip_irq_enable( irq_mask );
#endif
}

/**
@brief		IIPAX ON
*/
static VOID im_iip_On_Iclk( VOID )
{
#ifdef CO_ACT_IIP_ICLK
	if( gIM_IIP_ICLK_Counter < 255 ) {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_IIP_ICLK_Counter, &Dd_Top_Get_CLKSTOP3(), ~D_DD_TOP_IIPAX_BIT );
#ifdef CO_DEBUG_ON_PC
		Dd_Top_Get_CLKSTOP3() &= ~D_DD_TOP_IIPAX_BIT;
#endif
	}
#endif
}

/**
@brief		IIPAX OFF
*/
static VOID im_iip_Off_Iclk( VOID )
{
#ifdef CO_ACT_IIP_ICLK
	UINT32 irq_mask;

	irq_mask = im_iip_irq_disable();

	if( gIM_IIP_ICLK_Counter == 1 ) {
		if( gIM_IIP_Iclk_Ctrl_Disable == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_IIP_ICLK_Counter, &Dd_Top_Get_CLKSTOP3(), D_DD_TOP_IIPAX_BIT );
#ifdef CO_DEBUG_ON_PC
			Dd_Top_Get_CLKSTOP3() |= D_DD_TOP_IIPAX_BIT;
#endif
		}
	}
	else {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_IIP_ICLK_Counter, &Dd_Top_Get_CLKSTOP3(), D_DD_TOP_IIPAX_BIT );
#ifdef CO_DEBUG_ON_PC
		Dd_Top_Get_CLKSTOP3() |= D_DD_TOP_IIPAX_BIT;
#endif
	}

	im_iip_irq_enable( irq_mask );
#endif
}

// Calculate wait time to last_wait_time
static INT32 im_iip_calc_timeout( T_IM_IIP_TIMEOUT* const tmout )
{
	switch( tmout->wait_time ) {
		case D_DDIM_USER_SEM_WAIT_FEVR:		// FALLTHROUGH
		case D_DDIM_USER_SEM_WAIT_POL:
			tmout->last_wait_time = tmout->wait_time;
			break;
		default:
			if( (tmout->wait_time) < (tmout->cur_time - tmout->start_time) ) {
				// Timeout
				return D_IM_IIP_TIMEOUT_ERR;
			}

			tmout->last_wait_time = tmout->wait_time - (DDIM_USER_TMO)(tmout->cur_time - tmout->start_time);
			break;
	}

	return D_IM_IIP_OK;
}

// twai_flg and pol_flg. (Task context:twai_flg, Interrupt context:pol_flg)
static INT32 im_iip_twai_pol_flg( const DDIM_USER_ID flagid, const DDIM_USER_FLGPTN waiptn, const DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* const p_flgptn, const DDIM_USER_TMO tmout )
{
	DDIM_USER_ER ercd;

	if( tmout == D_DDIM_USER_SEM_WAIT_POL ) {
		ercd = DDIM_User_Pol_Flg( flagid, waiptn, wfmode, p_flgptn );
	}
	else {
		ercd = DDIM_User_Twai_Flg( flagid, waiptn, wfmode, p_flgptn, tmout );
	}

	if( D_DDIM_USER_E_TMOUT == ercd ) {
		return D_IM_IIP_TIMEOUT_ERR;
	}
	else
	if( D_DDIM_USER_E_OK != ercd ) {
		Im_IIP_Print_Pcdbg(( "im_iip_twai_pol_flg() err%u ercd=%d id=%u wait=0x%lx\n", 1, ercd, flagid, waiptn ));
		return D_IM_IIP_SYSTEMCALL_ERR;
	}
	return D_IM_IIP_OK;
}

// twai_sem and pol_sem. (Task context:twai_sem, Interrupt context:pol_sem)
static INT32 im_iip_twai_pol_sem( const DDIM_USER_ID semid, const DDIM_USER_TMO tmout )
{
	DDIM_USER_ER ercd;

	if( tmout == D_DDIM_USER_SEM_WAIT_POL ) {
		ercd = DDIM_User_Pol_Sem( semid );
	}
	else {
		ercd = DDIM_User_Twai_Sem( semid, tmout );
	}
	if( D_DDIM_USER_E_TMOUT == ercd ) {
		return D_IM_IIP_TIMEOUT_ERR;
	}
	else
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_IM_IIP_SYSTEMCALL_ERR;
	}
	return D_IM_IIP_OK;
}

// Set ACESEN0 register
static VOID im_iip_set_acesen0_reg( volatile T_IM_IIP_SWTRG_MNG* const swtrg_mng )
{
	BOOL is_enabled = FALSE;

	Im_IIP_On_Pclk();

	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_LUT_A) != 0 ) {
		IO_IIP.ACESEN0.bit.LUARAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.LUARAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_LUT_B) != 0 ) {
		IO_IIP.ACESEN0.bit.LUBRAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.LUBRAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_LUT_C) != 0 ) {
		IO_IIP.ACESEN0.bit.LUCRAC = 1;
	}
	else {
		IO_IIP.ACESEN0.bit.LUCRAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_LUT_D) != 0 ) {
		IO_IIP.ACESEN0.bit.LUDRAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.LUDRAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_LUT_E) != 0 ) {
		IO_IIP.ACESEN0.bit.LUERAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.LUERAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_LUT_F) != 0 ) {
		IO_IIP.ACESEN0.bit.LUFRAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.LUFRAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_HIST_0) != 0 ) {
		IO_IIP.ACESEN0.bit.MO0RAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.MO0RAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_HIST_1) != 0 ) {
		IO_IIP.ACESEN0.bit.MO1RAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.MO1RAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_HIST_2) != 0 ) {
		IO_IIP.ACESEN0.bit.MO2RAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.MO2RAC = 0;
	}
	if( (swtrg_mng->res_bitmask & E_IM_IIP_OPEN_RES_HIST_3) != 0 ) {
		IO_IIP.ACESEN0.bit.MO3RAC = 1;
		is_enabled = TRUE;
	}
	else {
		IO_IIP.ACESEN0.bit.MO3RAC = 0;
	}

	Im_IIP_Off_Pclk();

	if( is_enabled != FALSE ) {
		im_iip_wait_usec( D_IM_IIP_SRAM_WAIT_USEC );
	}
}

// Resource hunt (Unit, PIXFMTTBL, BI-Cubic unit, LUTA~F, Histgram0~3)
static INT32 im_iip_hunt_unit_and_pix( const U_IM_IIP_64BIT_MASK unitid_bitmask, const UINT32 pixid_bitmask, const UINT32 other_bitmask, const UINT32 cache_bitmask )
{
	DDIM_USER_ER		ercd;
	DDIM_USER_FLGPTN	unit_huntedptn;
	DDIM_USER_FLGPTN	pix_huntedptn;
	DDIM_USER_FLGPTN	other_huntedptn;
	static const char	errmsg1[] = "Im_IIP_Open_SWTRG: warning: cache %u open count is full.\n";
	UINT32				irq_mask;
#if defined(CO_IIP_DEBUG_PRINT_FUNC)
	static const char	errmsg2[] = "im_iip_hunt_unit_and_pix() err%u ercd=%d\n";
#endif

	if( unitid_bitmask.flgptn.lower != 0 ) {
		ercd = DDIM_User_Pol_Flg( FID_IM_IIP_UNIT_LO, unitid_bitmask.flgptn.lower, D_DDIM_USER_TWF_ANDW, &unit_huntedptn );
		if( ercd != D_DDIM_USER_E_OK ) {
			Im_IIP_Print_Pcdbg(( errmsg2, 1, ercd ));
			return ercd;
		}
	}

	if( unitid_bitmask.flgptn.upper != 0 ) {
		ercd = DDIM_User_Pol_Flg( FID_IM_IIP_UNIT_HI, unitid_bitmask.flgptn.upper, D_DDIM_USER_TWF_ANDW, &unit_huntedptn );
		if( ercd != D_DDIM_USER_E_OK ) {
			Im_IIP_Print_Pcdbg(( errmsg2, 2, ercd ));
			return ercd;
		}
	}

	if( pixid_bitmask != 0 ) {
		ercd = DDIM_User_Pol_Flg( FID_IM_IIP_PIX, (DDIM_USER_FLGPTN)pixid_bitmask, D_DDIM_USER_TWF_ANDW, &pix_huntedptn );
		if( ercd != D_DDIM_USER_E_OK ) {
			Im_IIP_Print_Pcdbg(( errmsg2, 3, ercd ));
			return ercd;
		}
	}

	if( other_bitmask != 0 ) {
		ercd = DDIM_User_Pol_Flg( FID_IM_IIP_RES, (DDIM_USER_FLGPTN)other_bitmask, D_DDIM_USER_TWF_ANDW, &other_huntedptn );
		if( ercd != D_DDIM_USER_E_OK ) {
			Im_IIP_Print_Pcdbg(( errmsg2, 4, ercd ));
			return ercd;
		}
	}


	// Resources hunt successful.


	// Do resources hunt.
	ercd = DDIM_User_Clr_Flg( FID_IM_IIP_UNIT_LO, ~unitid_bitmask.flgptn.lower );
	if( ercd != D_DDIM_USER_E_OK ) {
		Im_IIP_Print_Pcdbg(( errmsg2, 5, ercd ));
		return ercd;
	}

	ercd = DDIM_User_Clr_Flg( FID_IM_IIP_UNIT_HI, ~unitid_bitmask.flgptn.upper );
	if( ercd != D_DDIM_USER_E_OK ) {
		Im_IIP_Print_Pcdbg(( errmsg2, 6, ercd ));
		return ercd;
	}

	ercd = DDIM_User_Clr_Flg( FID_IM_IIP_PIX, ~(DDIM_USER_FLGPTN)pixid_bitmask );
	if( ercd != D_DDIM_USER_E_OK ) {
		Im_IIP_Print_Pcdbg(( errmsg2, 7, ercd ));
		return ercd;
	}

	if( other_bitmask != 0 ) {
		ercd = DDIM_User_Clr_Flg( FID_IM_IIP_RES, ~(DDIM_USER_FLGPTN)other_bitmask );
		if( ercd != D_DDIM_USER_E_OK ) {
			Im_IIP_Print_Pcdbg(( errmsg2, 8, ercd ));
			return ercd;
		}
	}

	irq_mask = im_iip_irq_disable();

	gIM_IIP_SwtrgMng.unit_bitmask |= unitid_bitmask.bit64;
	gIM_IIP_SwtrgMng.pixfmttbl_bitmask |= pixid_bitmask;
	gIM_IIP_SwtrgMng.res_bitmask |= other_bitmask;
	if( (cache_bitmask & E_IM_IIP_OPEN_RES_CACHE0) != 0 ) {
		if( gIM_IIP_SwtrgMng.cache_users[0] != 0xFFFFFFFF ) {

			gIM_IIP_SwtrgMng.cache_users[0]++;
		}
		else {
			Ddim_Print(( errmsg1, 0 ));
		}
	}
	if( (cache_bitmask & E_IM_IIP_OPEN_RES_CACHE1) != 0 ) {
		if( gIM_IIP_SwtrgMng.cache_users[1] != 0xFFFFFFFF ) {
			gIM_IIP_SwtrgMng.cache_users[1]++;
		}
		else {
			Ddim_Print(( errmsg1, 1 ));
		}
	}
	if( (cache_bitmask & E_IM_IIP_OPEN_RES_CACHE2) != 0 ) {
		if( gIM_IIP_SwtrgMng.cache_users[2] != 0xFFFFFFFF ) {
			gIM_IIP_SwtrgMng.cache_users[2]++;
		}
		else {
			Ddim_Print(( errmsg1, 2 ));
		}
	}
	im_iip_set_acesen0_reg( &gIM_IIP_SwtrgMng );

	im_iip_irq_enable( irq_mask );

	return D_DDIM_USER_E_OK;
}

// Wait update PIXFMTTBL enable flag change to valid
static VOID im_iip_wait_pftflg( VOID )
{
	if( IO_IIP.IZACTL1.bit.TRG == D_IM_IIP_STOPPED ) {
		return;
	}

	while( IO_IIP.PFTREWTFLG.bit.PFTFLG == 0 ) {
		// do nothing
#ifdef CO_DEBUG_ON_PC
		IO_IIP.PFTREWTFLG.bit.PFTFLG = 1;
#endif //CO_DEBUG_ON_PC
	}

	IO_IIP.PFTREWTFLG.bit.PFTFLG = 1;
}

/*
   Clear Interrupt enable bit.
   Clear Interrupt cause flag bit.
   Clear Interrupt factor bit for Im_IIP_Wait_End().
*/
static INT32 im_iip_unset_interrupt( const U_IM_IIP_64BIT_MASK unitid_bitmask, const UCHAR cause )
{
	INT32 ercd;
	UINT32 loopcnt;
	ULONG int_fact = 0;

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_0) / sizeof(gIM_IIP_Inttbl_0[0])); loopcnt++ ) {
		if( (gIM_IIP_Inttbl_0[loopcnt].unitid_bitmask & unitid_bitmask.bit64) != 0 ) {
			int_fact |= gIM_IIP_Inttbl_0[loopcnt].int_fact;
		}
	}

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_1) / sizeof(gIM_IIP_Inttbl_1[0])); loopcnt++ ) {
		if( (gIM_IIP_Inttbl_1[loopcnt].unitid_bitmask & unitid_bitmask.bit64) != 0 ) {
			int_fact |= gIM_IIP_Inttbl_1[loopcnt].int_fact;
		}
	}

#if 0	// INTIZ2/INTEN2 delete
	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_2) / sizeof(gIM_IIP_Inttbl_2[0])); loopcnt++ ) {
		if( (gIM_IIP_Inttbl_2[loopcnt].unitid_bitmask & unitid_bitmask.bit64) != 0 ) {
			int_fact |= gIM_IIP_Inttbl_2[loopcnt].int_fact;
		}
	}
#endif	// INTIZ2/INTEN2 delete

	ercd = Im_IIP_Set_Interrupt( int_fact, cause );
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	if( cause == D_IM_IIP_ENABLE_OFF ) {
		ercd = Im_IIP_Set_IntHandler( int_fact, NULL );
		if( ercd != D_IM_IIP_OK ) {
			return ercd;
		}

		// Clear Interrupt factor
		ercd = DDIM_User_Clr_Flg( FID_IM_IIP, ~int_fact );
		if( ercd != D_DDIM_USER_E_OK ) {
			return D_IM_IIP_SYSTEMCALL_ERR;
		}
	}

	return ercd;
}

// Uewrite PIXFMTTBL when IIP macro is running (IZACTL1.TRG==3)
static VOID im_iip_update_pixfmttbl( const UINT32 pixid )
{
	if( IO_IIP.IZACTL1.bit.TRG == D_IM_IIP_STOPPED ) {
		return;
	}

	IO_IIP.PFTREWT.bit.PFTREWT = pixid;

	while( IO_IIP.PFTREWTFLG.bit.PFTFLG == 0 ) {
		// do nothing
#ifdef CO_DEBUG_ON_PC
		IO_IIP.PFTREWTFLG.bit.PFTFLG = 1;
#endif //CO_DEBUG_ON_PC
	}
}

/**
@brief		Set UNITINFTBL (except for SL)
@param[in]	cfg				pointer of configuration data
@param[in]	pUNITINFTBL		Beginning address of UNITINFTBL register
*/
static VOID im_iip_set_UNITINFTBL( const T_IM_IIP_UNIT_CFG* const cfg, volatile struct io_iip_uinftbl* const pUNITINFTBL )
{
	if( cfg == NULL ) {
		pUNITINFTBL->UNITINFTBL0.bit.HWEN = D_IM_IIP_HW_CTRL_SWTRG;
	}
	else {
		pUNITINFTBL->UNITINFTBL0.bit.HWEN = cfg->unit_ctrl;
		pUNITINFTBL->UNITINFTBL0.bit.CHAIN = cfg->chain_enable;
		pUNITINFTBL->UNITINFTBL2.bit.PADRS = cfg->unit_param_addr;
	}
}

/**
@brief		Set UNITINFTBL for SL
@param[in]	cfg				pointer of configuration data
@param[in]	pUNITINFTBL		Beginning address of UNITINFTBL register
*/
static VOID im_iip_set_UNITINFTBL_SL( const T_IM_IIP_UNIT_CFG* const sl_cfg, volatile struct io_iip_uinftbl* const pUNITINFTBL )
{
	union {
		ULLONG					load_unit_param_flag;
		union io_iip_uinftbl_1	uinftbl_1;
	} u_pldunit = {
		.load_unit_param_flag = 0ull,
		.uinftbl_1.word = { 0ul, 0ul },
	};

	if( sl_cfg == NULL ) {
		pUNITINFTBL->UNITINFTBL0.bit.HWEN = D_IM_IIP_HW_CTRL_SWTRG;
	}
	else {
		pUNITINFTBL->UNITINFTBL0.bit.HWEN = sl_cfg->unit_ctrl;
		pUNITINFTBL->UNITINFTBL0.bit.CHAIN = sl_cfg->chain_enable;
		u_pldunit.load_unit_param_flag = sl_cfg->load_unit_param_flag;
		pUNITINFTBL->UNITINFTBL1 = u_pldunit.uinftbl_1;		// Force 32bit access
		pUNITINFTBL->UNITINFTBL2.bit.PADRS = sl_cfg->unit_param_addr;
	}
}

/**
@brief		Update PADRS
@param[in]	unit_param_addr		Beginning address of Unit parameters
@param[in]	pUNITINFTBL			Beginning address of UNITINFTBL register
*/
static VOID im_iip_update_UNITINFTBL_PADRS( const UINT32 unit_param_addr, volatile union io_iip_uinftbl_2* const pUNITINFTBL2 )
{
	pUNITINFTBL2->bit.PADRS = unit_param_addr;
}

/**
@brief		Control SWTRG
@param[in]	pUNITINFTBL		Beginning address of UNITINFTBL register
@param[in]	swtrg			Setting value of SWTRG
*/
static INT32 im_iip_control_SWTRG( volatile union io_iip_uinftbl_0* const pUNITINFTBL0, const INT32 swtrg )
{
	if( pUNITINFTBL0->bit.HWEN == D_IM_IIP_HW_CTRL_SWTRG ) {
		if( swtrg == D_IM_IIP_SWTRG_ON ) {
			pUNITINFTBL0->bit.SWTRG = D_IM_IIP_SWTRG_START;
			Im_IIP_Print_Pcdbg(( "im_iip_control_SWTRG: 0x%lx = %u\n", (ULONG)pUNITINFTBL0, D_IM_IIP_SWTRG_START ));
		}
		return D_IM_IIP_OK;
	}
	else {
		return D_IM_IIP_ERR;
	}
}

/**
@brief		Copy
@param[out]	dst				Destination data
@param[in]	src				Source data
@param[in]	bytes			Copy size
*/
static VOID im_iip_get_regword( ULONG* dst, const volatile ULONG* src, const UINT32 bytes )
{
	UINT32	loopcnt;

	for ( loopcnt = 0; loopcnt < bytes; loopcnt += 4 ) {
		*(dst++) = *(src++);
	}
}

/**
@brief		Fill
@param[in]	src				Source data
@param[in]	start_pos		Start position
@param[in]	num				Fill size (num * 4 bytes)
@param[in]	fill_value		Fill value
*/
static VOID im_iip_fill_word( volatile ULONG* dst, const UINT32 start_pos, const UINT32 num, const ULONG fill_value )
{
	UINT32	loopcnt;

	dst += start_pos;
	for ( loopcnt = 0; loopcnt < num; loopcnt++ ) {
		*(dst++) = fill_value;
	}
}

/**
@brief		Copy
@param[out]	dst				Destination data
@param[in]	src				Source data
@param[in]	bytes			Copy size
*/
static VOID im_iip_copy_short_to_word( volatile ULONG* dst, const USHORT* src, const UINT32 bytes )
{
	UINT32	loopcnt;

	for( loopcnt = 0; loopcnt < bytes; loopcnt += 2 ) {
		*(dst++) = *(src++);
	}
}

/**
@brief		Get Unit Parameter Size
@param[out]	size			Unit Parameter Size
@param[in]	unit_id			Unit ID
*/
static VOID im_iip_get_unit_param_size( UINT32* const size, const E_IM_IIP_UNIT_ID unit_id )
{
	switch ( unit_id ) {
		case E_IM_IIP_UNIT_ID_LD0:
		case E_IM_IIP_UNIT_ID_LD1:
		case E_IM_IIP_UNIT_ID_LD2:
		case E_IM_IIP_UNIT_ID_LD3:
		case E_IM_IIP_UNIT_ID_LD4:
		case E_IM_IIP_UNIT_ID_LD5:
		case E_IM_IIP_UNIT_ID_LD6:
		case E_IM_IIP_UNIT_ID_LD7:
			*size = D_IM_IIP_UNITPARAM_SIZE_LD;
			break;
		case E_IM_IIP_UNIT_ID_FLT:
			*size = D_IM_IIP_UNITPARAM_SIZE_FLT;
			break;
		case E_IM_IIP_UNIT_ID_AFN0:
		case E_IM_IIP_UNIT_ID_AFN1:
		case E_IM_IIP_UNIT_ID_AFN2:
		case E_IM_IIP_UNIT_ID_AFN3:
			*size = D_IM_IIP_UNITPARAM_SIZE_AFN;
			break;
		case E_IM_IIP_UNIT_ID_FRECT0:
		case E_IM_IIP_UNIT_ID_FRECT1:
			*size = D_IM_IIP_UNITPARAM_SIZE_FRECT;
			break;
		case E_IM_IIP_UNIT_ID_MON:
			*size = D_IM_IIP_UNITPARAM_SIZE_MON;
			break;
		case E_IM_IIP_UNIT_ID_SL0:
		case E_IM_IIP_UNIT_ID_SL1:
		case E_IM_IIP_UNIT_ID_SL2:
		case E_IM_IIP_UNIT_ID_SL3:
		case E_IM_IIP_UNIT_ID_SL4:
		case E_IM_IIP_UNIT_ID_SL5:
		case E_IM_IIP_UNIT_ID_SL6:
		case E_IM_IIP_UNIT_ID_SL7:
		case E_IM_IIP_UNIT_ID_SL8:
			*size = D_IM_IIP_UNITPARAM_SIZE_SL;
			break;
		case E_IM_IIP_UNIT_ID_CSC0:
		case E_IM_IIP_UNIT_ID_CSC1:
		case E_IM_IIP_UNIT_ID_CSC2:
		case E_IM_IIP_UNIT_ID_CSC3:
			*size = D_IM_IIP_UNITPARAM_SIZE_CSC;
			break;
		case E_IM_IIP_UNIT_ID_LUT:
			*size = D_IM_IIP_UNITPARAM_SIZE_LUT;
			break;
		case E_IM_IIP_UNIT_ID_GPC:
			*size = D_IM_IIP_UNITPARAM_SIZE_GPC;
			break;
		case E_IM_IIP_UNIT_ID_BLEND0:
		case E_IM_IIP_UNIT_ID_BLEND1:
			*size = D_IM_IIP_UNITPARAM_SIZE_BLEND;
			break;
		case E_IM_IIP_UNIT_ID_CFL0:
			*size = D_IM_IIP_UNITPARAM_SIZE_CFL;
			break;
		case E_IM_IIP_UNIT_ID_MFT:
			*size = D_IM_IIP_UNITPARAM_SIZE_MFT;
			break;
		default :
			*size = D_IM_IIP_UNITPARAM_SIZE_ERROR;
	}
}

/**
@brief		Get UnitInfTbl register address
@param[in]	unit_id			Unit ID
@return		UnitInfTbl address
*/
static volatile struct io_iip_uinftbl* im_iip_get_unitinftbl_regaddr( const E_IM_IIP_UNIT_ID unit_id )
{
	switch ( unit_id ) {
		case E_IM_IIP_UNIT_ID_LD0:
			return &IO_IIP.UNITINFTBL_LD0;
		case E_IM_IIP_UNIT_ID_LD1:
			return &IO_IIP.UNITINFTBL_LD1;
		case E_IM_IIP_UNIT_ID_LD2:
			return &IO_IIP.UNITINFTBL_LD2;
		case E_IM_IIP_UNIT_ID_LD3:
			return &IO_IIP.UNITINFTBL_LD3;
		case E_IM_IIP_UNIT_ID_LD4:
			return &IO_IIP.UNITINFTBL_LD4;
		case E_IM_IIP_UNIT_ID_LD5:
			return &IO_IIP.UNITINFTBL_LD5;
		case E_IM_IIP_UNIT_ID_LD6:
			return &IO_IIP.UNITINFTBL_LD6;
		case E_IM_IIP_UNIT_ID_LD7:
			return &IO_IIP.UNITINFTBL_LD7;
		case E_IM_IIP_UNIT_ID_FLT:
			return &IO_IIP.UNITINFTBL_FLT;
		case E_IM_IIP_UNIT_ID_AFN0:
			return &IO_IIP.UNITINFTBL_AFN0;
		case E_IM_IIP_UNIT_ID_AFN1:
			return &IO_IIP.UNITINFTBL_AFN1;
		case E_IM_IIP_UNIT_ID_AFN2:
			return &IO_IIP.UNITINFTBL_AFN2;
		case E_IM_IIP_UNIT_ID_AFN3:
			return &IO_IIP.UNITINFTBL_AFN3;
		case E_IM_IIP_UNIT_ID_FRECT0:
			return &IO_IIP.UNITINFTBL_FRC0;
		case E_IM_IIP_UNIT_ID_FRECT1:
			return &IO_IIP.UNITINFTBL_FRC1;
		case E_IM_IIP_UNIT_ID_MON:
			return &IO_IIP.UNITINFTBL_MON;
		case E_IM_IIP_UNIT_ID_SL0:
			return &IO_IIP.UNITINFTBL_SL0;
		case E_IM_IIP_UNIT_ID_SL1:
			return &IO_IIP.UNITINFTBL_SL1;
		case E_IM_IIP_UNIT_ID_SL2:
			return &IO_IIP.UNITINFTBL_SL2;
		case E_IM_IIP_UNIT_ID_SL3:
			return &IO_IIP.UNITINFTBL_SL3;
		case E_IM_IIP_UNIT_ID_SL4:
			return &IO_IIP.UNITINFTBL_SL4;
		case E_IM_IIP_UNIT_ID_SL5:
			return &IO_IIP.UNITINFTBL_SL5;
		case E_IM_IIP_UNIT_ID_SL6:
			return &IO_IIP.UNITINFTBL_SL6;
		case E_IM_IIP_UNIT_ID_SL7:
			return &IO_IIP.UNITINFTBL_SL7;
		case E_IM_IIP_UNIT_ID_SL8:
			return &IO_IIP.UNITINFTBL_SL8;
		case E_IM_IIP_UNIT_ID_CSC0:
			return &IO_IIP.UNITINFTBL_CSC0;
		case E_IM_IIP_UNIT_ID_CSC1:
			return &IO_IIP.UNITINFTBL_CSC1;
		case E_IM_IIP_UNIT_ID_CSC2:
			return &IO_IIP.UNITINFTBL_CSC2;
		case E_IM_IIP_UNIT_ID_CSC3:
			return &IO_IIP.UNITINFTBL_CSC3;
		case E_IM_IIP_UNIT_ID_LUT:
			return &IO_IIP.UNITINFTBL_LUT;
		case E_IM_IIP_UNIT_ID_GPC:
			return &IO_IIP.UNITINFTBL_GPC;
		case E_IM_IIP_UNIT_ID_BLEND0:
			return &IO_IIP.UNITINFTBL_BLD0;
		case E_IM_IIP_UNIT_ID_BLEND1:
			return &IO_IIP.UNITINFTBL_BLD1;
		case E_IM_IIP_UNIT_ID_CFL0:
			return &IO_IIP.UNITINFTBL_CFL0;
		case E_IM_IIP_UNIT_ID_MFT:
			return &IO_IIP.UNITINFTBL_MFT;
		default:
			return NULL;
	}
}

// Clear UNITINFTBL.HWEN bit of all units.
static VOID im_iip_clear_hwen_all( VOID )
{
	E_IM_IIP_UNIT_ID unitid;
	volatile struct io_iip_uinftbl* p_uinftbl;

	Im_IIP_On_Pclk();

	for( unitid = E_IM_IIP_UNIT_ID_TOP; unitid < E_IM_IIP_UNIT_ID_MAX; unitid++ ) {
		p_uinftbl = im_iip_get_unitinftbl_regaddr( unitid );
		if( p_uinftbl == NULL ) {
			continue;
		}
		p_uinftbl->UNITINFTBL0.bit.HWEN = D_IM_IIP_HW_CTRL_SWTRG;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();
}

#ifdef CO_DEBUG_ON_PC
static VOID im_iip_pseudo_interrupt( UINT32 wait_factor )
{
	UINT32			loopcnt;
	ULONG			intiz0 = 0ul;
	ULONG			intiz1 = 0ul;
//	ULONG			intiz2 = 0ul;

	Dd_ARM_Critical_Section_Start( gIM_IIP_Spin_Lock );

	wait_factor &= ~D_IM_IIP_INT_FACTOR_AXIERR;
	IO_IIP.INTIZ0.word &= ~D_IM_IIP_AXEFLG_BIT;

#if 0	// INTIZ2/INTEN2 delete
	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_2) / sizeof(gIM_IIP_Inttbl_2[0])); loopcnt++ ) {
		if( (wait_factor & gIM_IIP_Inttbl_2[loopcnt].int_fact) != 0 ) {
			intiz2 |= gIM_IIP_Inttbl_2[loopcnt].flg_bitmask;
		}
	}
#endif	// INTIZ2/INTEN2 delete

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_1) / sizeof(gIM_IIP_Inttbl_1[0])); loopcnt++ ) {
		if( (wait_factor & gIM_IIP_Inttbl_1[loopcnt].int_fact) != 0 ) {
			intiz1 |= gIM_IIP_Inttbl_1[loopcnt].flg_bitmask;
		}
	}

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_0) / sizeof(gIM_IIP_Inttbl_0[0])); loopcnt++ ) {
		if( (wait_factor & gIM_IIP_Inttbl_0[loopcnt].int_fact) != 0 ) {
			intiz0 |= gIM_IIP_Inttbl_0[loopcnt].flg_bitmask;
		}
	}

	IO_IIP.INTIZ0.word |= intiz0;
	IO_IIP.INTIZ1.word |= intiz1;
#if 0	// INTIZ2/INTEN2 delete
	IO_IIP.INTIZ2.word |= intiz2;
#endif	// INTIZ2/INTEN2 delete

	Im_IIP_Int_Handler();

	Dd_ARM_Critical_Section_End( gIM_IIP_Spin_Lock );
}
#endif


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
@brief		HCLK ON
*/
VOID Im_IIP_On_Hclk( VOID )
{
#ifdef CO_ACT_IIP_HCLK
	if( gIM_IIP_HCLK_Counter < 255 ) {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_IIP_HCLK_Counter, &Dd_Top_Get_CLKSTOP3(), ~D_DD_TOP_IIPAH_BIT );
#ifdef CO_DEBUG_ON_PC
		Dd_Top_Get_CLKSTOP3() &= ~D_DD_TOP_IIPAH_BIT;
#endif
	}
#endif
}

/**
@brief		HCLK OFF
*/
VOID Im_IIP_Off_Hclk( VOID )
{
#ifdef CO_ACT_IIP_HCLK
	UINT32 irq_mask;

	irq_mask = im_iip_irq_disable();

	if( gIM_IIP_HCLK_Counter == 1 ) {
		if( gIM_IIP_Hclk_Ctrl_Disable == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_IIP_HCLK_Counter, &Dd_Top_Get_CLKSTOP3(), D_DD_TOP_IIPAH_BIT );
#ifdef CO_DEBUG_ON_PC
			Dd_Top_Get_CLKSTOP3() |= D_DD_TOP_IIPAH_BIT;
#endif
		}
	}
	else {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_IIP_HCLK_Counter, &Dd_Top_Get_CLKSTOP3(), D_DD_TOP_IIPAH_BIT );
#ifdef CO_DEBUG_ON_PC
		Dd_Top_Get_CLKSTOP3() |= D_DD_TOP_IIPAH_BIT;
#endif
	}

	im_iip_irq_enable( irq_mask );
#endif
}

/**
@brief		PCLK ON
*/
VOID Im_IIP_On_Pclk( VOID )
{
#ifdef CO_ACT_IIP_PCLK
	if( gIM_IIP_PCLK_Counter < 255 ) {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_IIP_PCLK_Counter, &Dd_Top_Get_CLKSTOP3(), ~D_DD_TOP_IIPAP_BIT );
#ifdef CO_DEBUG_ON_PC
		Dd_Top_Get_CLKSTOP3() &= ~D_DD_TOP_IIPAP_BIT;
#endif

		// wait
		IO_IIP.AHBSTS.word;	/* pgr0695 */
	}
#endif
}

/**
@brief		PCLK OFF
*/
VOID Im_IIP_Off_Pclk( VOID )
{
#ifdef CO_ACT_IIP_PCLK
	UINT32 irq_mask;

	irq_mask = im_iip_irq_disable();

	// wait
	IO_IIP.AHBSTS.word;	/* pgr0695 */

	if( gIM_IIP_PCLK_Counter == 1 ) {
		if( gIM_IIP_Pclk_Ctrl_Disable == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_IIP_PCLK_Counter, &Dd_Top_Get_CLKSTOP3(), D_DD_TOP_IIPAP_BIT );
#ifdef CO_DEBUG_ON_PC
			Dd_Top_Get_CLKSTOP3() |= D_DD_TOP_IIPAP_BIT;
#endif
		}
	}
	else {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_IIP_PCLK_Counter, &Dd_Top_Get_CLKSTOP3(), D_DD_TOP_IIPAP_BIT );
#ifdef CO_DEBUG_ON_PC
		Dd_Top_Get_CLKSTOP3() |= D_DD_TOP_IIPAP_BIT;
#endif
	}

	im_iip_irq_enable( irq_mask );
#endif
}

/**
*	Driver Initialization
*/
INT32 Im_IIP_Init( VOID )
{
	INT32	retval;

	im_iip_On_Iipclk();
	im_iip_On_Iclk();
	Im_IIP_On_Pclk();

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Im_IIP_Print_Pcdbg(( "PCLK=%u ICLK=%u CLK=%u\n", Dd_Top_Get_CLKSTOP_IIPAP(), Dd_Top_Get_CLKSTOP_IIPAX(), Dd_Top_Get_CLKSTOP_IIPCK() ));
#endif
	if( IO_IIP.IZACTL1.bit.TRG == D_IM_IIP_STOPPED ) {
		im_iip_On_izarac_caxrac();

		Im_IIP_Dsb();

		im_iip_wait_usec( D_IM_IIP_SRAM_WAIT_USEC );

		// Software-Reset ON
		IO_IIP.IZACTL0.word = 1;

		Im_IIP_Dsb();

		// IIPCLK        = MAX 252.0MHz		MIN 31.5MHz
		// APB clock     = MAX  66.7MHz		MIN 20.8MHz
		// -->   ( (1/IIPCLK) / (1/APB clock) ) * 2 + 1
		//     = ( (1/31.5)   / (1/66.7)      ) * 2 + 1
		//     = 5.23 => 6 APB cycle

		// wait 6 APB clock
		IO_IIP.IZACTL0.word;	/* pgr0695 */
		IO_IIP.IZACTL0.word;	/* pgr0695 */
		IO_IIP.IZACTL0.word;	/* pgr0695 */

		Im_IIP_Print_Pcdbg(( "IZACTL0 = 0x%lx IZACTL0 = 0x%x\n", IO_IIP.IZACTL0.word, IO_IIP.ACESEN0.bit.IZARAC ));

		Im_IIP_Dsb();

		// Software-Reset OFF
		IO_IIP.IZACTL0.word = 0;

		Im_IIP_Dsb();

		// wait 6 APB clock
		IO_IIP.IZACTL0.word;	/* pgr0695 */
		IO_IIP.IZACTL0.word;	/* pgr0695 */
		IO_IIP.IZACTL0.word;	/* pgr0695 */

		Im_IIP_Dsb();

		im_iip_Off_izarac_caxrac();

		Im_IIP_Print_Pcdbg(( "IZACTL0 = 0x%lx IZACTL0 = 0x%x\n", IO_IIP.IZACTL0.word, IO_IIP.ACESEN0.bit.IZARAC ));

		retval = D_IM_IIP_OK;
	}
	else {
		retval = D_IM_IIP_ERR;
	}

	Im_IIP_Off_Pclk();
	im_iip_Off_Iclk();
	im_iip_Off_Iipclk();
	Im_IIP_Dsb();

	if( retval != D_IM_IIP_OK ) {
		return retval;
	}

	retval = DDIM_User_Set_Flg( FID_IM_IIP_UNIT_LO, (DDIM_USER_FLGPTN)0xFFFFFFFF );
	if( retval != D_DDIM_USER_E_OK ) {
		return D_IM_IIP_ERR;
	}
	retval = DDIM_User_Set_Flg( FID_IM_IIP_UNIT_HI, (DDIM_USER_FLGPTN)0xFFFFFFFF );
	if( retval != D_DDIM_USER_E_OK ) {
		return D_IM_IIP_ERR;
	}
	retval = DDIM_User_Set_Flg( FID_IM_IIP_PIX, (DDIM_USER_FLGPTN)0xFFFFFFFF );
	if( retval != D_DDIM_USER_E_OK ) {
		return D_IM_IIP_ERR;
	}
	retval = DDIM_User_Set_Flg( FID_IM_IIP_RES, (DDIM_USER_FLGPTN)0xFFFFFFFF );
	if( retval != D_DDIM_USER_E_OK ) {
		return D_IM_IIP_ERR;
	}

	return D_IM_IIP_OK;
}

/**
 * Initialize IIP Unit. This API executes software-reset [UNITSR.UNITSR = 1b] and releases software-reset [UNITSR.UNITSR = 0b].
 */
INT32 Im_IIP_Init_Unit( const E_IM_IIP_UNIT_ID unit_id )
{
	volatile struct io_iip_uinftbl* p_uinftbl;
	INT32	retval;

	p_uinftbl = im_iip_get_unitinftbl_regaddr( unit_id );
	if( p_uinftbl == NULL ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}


	im_iip_On_Iipclk();
	im_iip_On_Iclk();
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	im_iip_On_izarac_caxrac();

	if( p_uinftbl->UNITINFTBL0.bit.SWTRG == D_IM_IIP_SWTRG_STOPPED ) {
		// Software-Reset ON
		p_uinftbl->UNITSR.word = 1;

		Im_IIP_Dsb();

		// IIPCLK        = MAX 252.0MHz		MIN 31.5MHz
		// APB clock     = MAX  66.7MHz		MIN 20.8MHz
		// -->   ( (1/IIPCLK) / (1/APB clock) ) * 2 + 1
		//     = ( (1/31.5)   / (1/66.7)      ) * 2 + 1
		//     = 5.23 => 6 APB cycle

		// wait 6 APB clock
		IO_IIP.IZACTL0.word;	/* pgr0695 */
		IO_IIP.IZACTL0.word;	/* pgr0695 */
		IO_IIP.IZACTL0.word;	/* pgr0695 */

		Im_IIP_Dsb();

		// Software-Reset OFF
		p_uinftbl->UNITSR.word = 0;

		Im_IIP_Dsb();

		// wait 6 APB clock
		IO_IIP.IZACTL0.word;	/* pgr0695 */
		IO_IIP.IZACTL0.word;	/* pgr0695 */
		IO_IIP.IZACTL0.word;	/* pgr0695 */

		Im_IIP_Dsb();

		retval = D_IM_IIP_OK;
	}
	else {
		retval = D_IM_IIP_ERR;
	}

	im_iip_Off_izarac_caxrac();

	Im_IIP_Off_Pclk();
	im_iip_Off_Iclk();
	im_iip_Off_Iipclk();
	Im_IIP_Dsb();

	return retval;
}

/**
*	Open Driver for hardware trigger.
*/
INT32 Im_IIP_Open( const INT32 wait_time )
{
#ifdef CO_PARAM_CHECK
	if( im_iip_check_wait_time(wait_time) == D_IM_IIP_FALSE ) {
		Ddim_Assertion(("I:Im_IIP_Open INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	return im_iip_twai_pol_sem( SID_IM_IIP, (DDIM_USER_TMO)wait_time );
}

/**
*	Close Driver for hardware trigger.
*/
INT32 Im_IIP_Close( VOID )
{
	DDIM_USER_ER	ercd;
	UINT32			irq_mask;

	gIM_IIP_Callback = NULL;

	irq_mask = im_iip_irq_disable();

	IO_IIP.INTEN0.bit.IZAEN = 0;

	im_iip_irq_enable( irq_mask );

	im_iip_clear_hwen_all();

	ercd = DDIM_User_Clr_Flg( FID_IM_IIP, ~D_IM_IIP_FLG_USED_BIT_PATTERN );
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_IM_IIP_SYSTEMCALL_ERR;
	}

	ercd = DDIM_User_Sig_Sem( SID_IM_IIP );
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_IM_IIP_SYSTEMCALL_ERR;
	}

	return D_IM_IIP_OK;
}

/**
*	Set PIXFMTTBL
*/
INT32 Im_IIP_Ctrl_PIXFMTTBL( const UINT32 pixid, const T_IM_IIP_PIXFMTTBL* const tbl )
{
	volatile struct io_iip_pixfmttbl*	pPIXFMTTBL;
	UINT32								irq_mask;

#ifdef CO_PARAM_CHECK
	if( pixid >= D_IM_IIP_MAX_PIXID ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_PIXFMTTBL INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	if( tbl == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_PIXFMTTBL INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif	// CO_PARAM_CHECK

	pPIXFMTTBL = &IO_IIP.PIXFMTTBL[pixid];

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

#ifdef CO_DEBUG_PRINT_IM_IIP
	Ddim_Print(( "Im_IIP_Ctrl_PIXFMTTBL pixid=%u\n", pixid ));
#endif

	irq_mask = im_iip_irq_disable();

	im_iip_wait_pftflg();

	pPIXFMTTBL->PIXFMTTBL_0.bit.PFMT = tbl->pix_format;
	pPIXFMTTBL->PIXFMTTBL_0.bit.PDEPTH = tbl->pix_depth;
	pPIXFMTTBL->PIXFMTTBL_0.bit.ALPHA = tbl->alpha;
	pPIXFMTTBL->PIXFMTTBL_0.bit.ADEPTH = tbl->alpha_depth;
	pPIXFMTTBL->PIXFMTTBL_0.bit.CHKYORDR = ((0x3 & tbl->chunky_color.component3) << 6) |
										   ((0x3 & tbl->chunky_color.component2) << 4) |
										   ((0x3 & tbl->chunky_color.component1) << 2) |
										   ((0x3 & tbl->chunky_color.component0) << 0);

	pPIXFMTTBL->PIXFMTTBL_1.bit.SGYG = tbl->sign_Y_G;
	pPIXFMTTBL->PIXFMTTBL_1.bit.SGB = tbl->sign_Cb_B;
	pPIXFMTTBL->PIXFMTTBL_1.bit.SGR = tbl->sign_Cr_R;
	pPIXFMTTBL->PIXFMTTBL_1.bit.SGA = tbl->sign_D3;
	pPIXFMTTBL->PIXFMTTBL_1.bit.ASUBSPL = tbl->alpha_subsampling;
	pPIXFMTTBL->PIXFMTTBL_1.bit.OARBHV = tbl->pix_outside_img;

	pPIXFMTTBL->PIXFMTTBL_8.bit.PICHSZ = tbl->width;
	pPIXFMTTBL->PIXFMTTBL_8.bit.PICVSZ = tbl->lines;

	pPIXFMTTBL->PIXFMTTBL_2.bit.OARVALYG = tbl->Y_G_val_outside_img;
	pPIXFMTTBL->PIXFMTTBL_2.bit.OARVALB = tbl->Cb_B_val_outside_img;
	pPIXFMTTBL->PIXFMTTBL_3.bit.OARVALR = tbl->Cr_R_val_outside_img;
	pPIXFMTTBL->PIXFMTTBL_3.bit.OARVALA = tbl->Alpha_val_outside_img;

	pPIXFMTTBL->PIXFMTTBL_0.bit.FTYPE = tbl->frame_type;

	pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFYG = tbl->masterIF_Y_G;
	pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFB = tbl->masterIF_Cb_B;
	pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFR = tbl->masterIF_Cr_R;
	pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFA = tbl->masterIF_Alpha;

	pPIXFMTTBL->PIXFMTTBL_4.bit.GHSZYG = tbl->line_bytes.Y_G;
	pPIXFMTTBL->PIXFMTTBL_5.bit.GHSZB = tbl->line_bytes.Cb_B;
	pPIXFMTTBL->PIXFMTTBL_6.bit.GHSZR = tbl->line_bytes.Cr_R;
	pPIXFMTTBL->PIXFMTTBL_7.bit.GHSZA = tbl->line_bytes.Alpha;

	pPIXFMTTBL->PIXFMTTBL_9.bit.ADRSYG = tbl->addr.Y_G;
	pPIXFMTTBL->PIXFMTTBL_10.bit.ADRSB = tbl->addr.Cb_B;
	pPIXFMTTBL->PIXFMTTBL_11.bit.ADRSR = tbl->addr.Cr_R;
	pPIXFMTTBL->PIXFMTTBL_12.bit.ADRSA = tbl->addr.Alpha;

	im_iip_update_pixfmttbl( pixid );

	im_iip_irq_enable( irq_mask );

#ifdef CO_DEBUG_PRINT_IM_IIP
	Ddim_Print(( "PFMT = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_0.bit.PFMT ));
	Ddim_Print(( "PDEPTH = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_0.bit.PDEPTH ));
	Ddim_Print(( "ALPHA = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_0.bit.ALPHA ));
	Ddim_Print(( "ADEPTH = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_0.bit.ADEPTH ));
	Ddim_Print(( "CHKYORDR = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_0.bit.CHKYORDR ));

	Ddim_Print(( "SGYG = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.SGYG ));
	Ddim_Print(( "SGB = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.SGB ));
	Ddim_Print(( "SGR = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.SGR ));
	Ddim_Print(( "SGA = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.SGA ));
	Ddim_Print(( "ASUBPL = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.ASUBSPL ));
	Ddim_Print(( "OARBHV = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.OARBHV ));

	Ddim_Print(( "PICHSZ = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_8.bit.PICHSZ ));
	Ddim_Print(( "PICVSZ = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_8.bit.PICVSZ ));

	Ddim_Print(( "OARVALYG = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_2.bit.OARVALYG ));
	Ddim_Print(( "OARVALB = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_2.bit.OARVALB ));
	Ddim_Print(( "OARVALR = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_3.bit.OARVALR ));
	Ddim_Print(( "OARVALA = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_3.bit.OARVALA ));

	Ddim_Print(( "MTIFYG = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFYG ));
	Ddim_Print(( "MTIFB = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFB ));
	Ddim_Print(( "MTIFR = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFR ));
	Ddim_Print(( "MTIFA = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_1.bit.MTIFA ));

	Ddim_Print(( "FTYPE = 0x%x\n", (UINT32)pPIXFMTTBL->PIXFMTTBL_0.bit.FTYPE ));

	Ddim_Print(( "GHSZYG = 0x%x\n", (INT32)pPIXFMTTBL->PIXFMTTBL_4.bit.GHSZYG ));
	Ddim_Print(( "GHSZB = 0x%x\n", (INT32)pPIXFMTTBL->PIXFMTTBL_5.bit.GHSZB ));
	Ddim_Print(( "GHSZR = 0x%x\n", (INT32)pPIXFMTTBL->PIXFMTTBL_6.bit.GHSZR ));
	Ddim_Print(( "GHSZA = 0x%x\n", (INT32)pPIXFMTTBL->PIXFMTTBL_7.bit.GHSZA ));

	Ddim_Print(( "ADRSYG = 0x%lx\n", pPIXFMTTBL->PIXFMTTBL_9.bit.ADRSYG ));
	Ddim_Print(( "ADRSB = 0x%lx\n", pPIXFMTTBL->PIXFMTTBL_10.bit.ADRSB ));
	Ddim_Print(( "ADRSR = 0x%lx\n", pPIXFMTTBL->PIXFMTTBL_11.bit.ADRSR ));
	Ddim_Print(( "ADRSA = 0x%lx\n", pPIXFMTTBL->PIXFMTTBL_12.bit.ADRSA ));
#endif //CO_DEBUG_PRINT_IM_IIP

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Set UNITINFTBL
*/
INT32 Im_IIP_Ctrl_UNITINFTBL( const T_IM_IIP_UNITINFTBL* const tbl )
{
#ifdef CO_PARAM_CHECK
	if( tbl == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_UNITINFTBL INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif	// CO_PARAM_CHECK

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	im_iip_set_UNITINFTBL( tbl->pLD[0], &IO_IIP.UNITINFTBL_LD0 );
	im_iip_set_UNITINFTBL( tbl->pLD[1], &IO_IIP.UNITINFTBL_LD1 );
	im_iip_set_UNITINFTBL( tbl->pLD[2], &IO_IIP.UNITINFTBL_LD2 );
	im_iip_set_UNITINFTBL( tbl->pLD[3], &IO_IIP.UNITINFTBL_LD3 );
	im_iip_set_UNITINFTBL( tbl->pLD[4], &IO_IIP.UNITINFTBL_LD4 );
	im_iip_set_UNITINFTBL( tbl->pLD[5], &IO_IIP.UNITINFTBL_LD5 );
	im_iip_set_UNITINFTBL( tbl->pLD[6], &IO_IIP.UNITINFTBL_LD6 );
	im_iip_set_UNITINFTBL( tbl->pLD[7], &IO_IIP.UNITINFTBL_LD7 );

	im_iip_set_UNITINFTBL( tbl->pFLT, &IO_IIP.UNITINFTBL_FLT );

	im_iip_set_UNITINFTBL( tbl->pAFN[0], &IO_IIP.UNITINFTBL_AFN0 );
	im_iip_set_UNITINFTBL( tbl->pAFN[1], &IO_IIP.UNITINFTBL_AFN1 );
	im_iip_set_UNITINFTBL( tbl->pAFN[2], &IO_IIP.UNITINFTBL_AFN2 );
	im_iip_set_UNITINFTBL( tbl->pAFN[3], &IO_IIP.UNITINFTBL_AFN3 );

	im_iip_set_UNITINFTBL( tbl->pFRECT[0], &IO_IIP.UNITINFTBL_FRC0 );
	im_iip_set_UNITINFTBL( tbl->pFRECT[1], &IO_IIP.UNITINFTBL_FRC1 );

	im_iip_set_UNITINFTBL( tbl->pMON, &IO_IIP.UNITINFTBL_MON );

	im_iip_set_UNITINFTBL_SL( tbl->pSL[0], &IO_IIP.UNITINFTBL_SL0 );
	im_iip_set_UNITINFTBL_SL( tbl->pSL[1], &IO_IIP.UNITINFTBL_SL1 );
	im_iip_set_UNITINFTBL_SL( tbl->pSL[2], &IO_IIP.UNITINFTBL_SL2 );
	im_iip_set_UNITINFTBL_SL( tbl->pSL[3], &IO_IIP.UNITINFTBL_SL3 );
	im_iip_set_UNITINFTBL_SL( tbl->pSL[4], &IO_IIP.UNITINFTBL_SL4 );
	im_iip_set_UNITINFTBL_SL( tbl->pSL[5], &IO_IIP.UNITINFTBL_SL5 );
	im_iip_set_UNITINFTBL_SL( tbl->pSL[6], &IO_IIP.UNITINFTBL_SL6 );
	im_iip_set_UNITINFTBL_SL( tbl->pSL[7], &IO_IIP.UNITINFTBL_SL7 );
	im_iip_set_UNITINFTBL_SL( tbl->pSL[8], &IO_IIP.UNITINFTBL_SL8 );

	im_iip_set_UNITINFTBL( tbl->pCSC[0], &IO_IIP.UNITINFTBL_CSC0 );
	im_iip_set_UNITINFTBL( tbl->pCSC[1], &IO_IIP.UNITINFTBL_CSC1 );
	im_iip_set_UNITINFTBL( tbl->pCSC[2], &IO_IIP.UNITINFTBL_CSC2 );
	im_iip_set_UNITINFTBL( tbl->pCSC[3], &IO_IIP.UNITINFTBL_CSC3 );

	im_iip_set_UNITINFTBL( tbl->pLUT, &IO_IIP.UNITINFTBL_LUT );

	im_iip_set_UNITINFTBL( tbl->pGPC, &IO_IIP.UNITINFTBL_GPC );

	im_iip_set_UNITINFTBL( tbl->pBLEND[0], &IO_IIP.UNITINFTBL_BLD0 );
	im_iip_set_UNITINFTBL( tbl->pBLEND[1], &IO_IIP.UNITINFTBL_BLD1 );

	im_iip_set_UNITINFTBL( tbl->pCFL, &IO_IIP.UNITINFTBL_CFL0 );

	im_iip_set_UNITINFTBL( tbl->pMFT, &IO_IIP.UNITINFTBL_MFT );

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Set LUTRAM
*/
INT32 Im_IIP_Ctrl_LUT( const T_IM_IIP_LUT* const lut )
{
	ULONG			lutramst = 0;
	ULONG			lutram_ofst = 0;
	INT32			retval = D_IM_IIP_OK;
	volatile ULONG*	dst;
	const USHORT*	src;

#ifdef CO_PARAM_CHECK
	if( lut == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_LUT INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	if( lut->buffer_bytes > (D_IM_IIP_MAX_LUTRAM_RESOL * sizeof(lut->buffer_addr[0])) ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_LUT INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	if( lut->lutram_type >= E_IM_IIP_LUTRAM_TYPE_MAX ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_LUT INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif	// CO_PARAM_CHECK

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	Im_IIP_On_Hclk();
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	while( 1 ) {
		switch( lut->lutram_type ) {
			case E_IM_IIP_LUTRAM_TYPE_A:
				lutramst = IO_IIP.AHBSTS.bit.LUTAST;
				lutram_ofst = 1024 * 0;
				break;
			case E_IM_IIP_LUTRAM_TYPE_B:
				lutramst = IO_IIP.AHBSTS.bit.LUTBST;
				lutram_ofst = 1024 * 2;
				break;
			case E_IM_IIP_LUTRAM_TYPE_C:
				lutramst = IO_IIP.AHBSTS.bit.LUTCST;
				lutram_ofst = 1024 * 4;
				break;
			case E_IM_IIP_LUTRAM_TYPE_D:
				lutramst = IO_IIP.AHBSTS.bit.LUTDST;
				lutram_ofst = 1024 * 1;
				break;
			case E_IM_IIP_LUTRAM_TYPE_E:
				lutramst = IO_IIP.AHBSTS.bit.LUTEST;
				lutram_ofst = 1024 * 3;
				break;
			case E_IM_IIP_LUTRAM_TYPE_F:
				lutramst = IO_IIP.AHBSTS.bit.LUTFST;
				lutram_ofst = 1024 * 5;
				break;
			default:
				retval = D_IM_IIP_INVALID_ARG_ERR;
		}
		if( lutramst != 0 ) {
			retval = D_IM_IIP_ERR;
		}

		if( retval != D_IM_IIP_OK ) {
			break;
		}


		dst = &IO_IIP_TBL.LUTRAM.LUTRAM.word[0][0] + lutram_ofst;
		src = lut->buffer_addr;

		im_iip_wait_usec( 1 );

		im_iip_copy_short_to_word( dst, src, lut->buffer_bytes );

		break;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Off_Hclk();
	Im_IIP_Dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return retval;
}

/**
   Open driver. This API gets IIP's UNIT and PIXFMTTBL semaphore using SWTRG system-call.

   @param[in]	unitid_bitmask : UNIT ID bitmask. (D_IM_IIP_PARAM_PLDUNIT_xxx).
   @param[in]	pixid_bitmask : PIXFMTTBL ID bitmask. @ref E_IM_IIP_PIXID.
   @param[in]	open_res_bitmask : Resource ID bitmask. @ref E_IM_IIP_OPEN_RES.
   @param[in]	wait_time : Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1,
							API waits permanently unless system-call gets IIP semaphore.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_TIMEOUT_ERR		: Wait timeout error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @remarks		This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
				This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
INT32 Im_IIP_Open_SWTRG( const ULLONG unitid_bitmask64, const UINT32 pixid_bitmask, const UINT32 open_res_bitmask, const INT32 wait_time )
{
	DDIM_USER_ER		ercd;
	DDIM_USER_ER		ercd_crit;
	T_IM_IIP_TIMEOUT	tmout;
	DDIM_USER_FLGPTN	unit_huntedptn;		// for dummy read.
	DDIM_USER_FLGPTN	pix_huntedptn;		// for dummy read.
	DDIM_USER_FLGPTN	other_huntedptn;	// for dummy read.
	U_IM_IIP_64BIT_MASK	unitid_bitmask = {
		.bit64 = unitid_bitmask64,
	};
	UINT32				cache_bitmask;
	UINT32				other_bitmask;


#ifdef CO_PARAM_CHECK
	if( im_iip_check_wait_time(wait_time) == D_IM_IIP_FALSE ) {
		Ddim_Assertion(("I:Im_IIP_Open INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif


	ercd = DDIM_User_Get_Tim( &tmout.start_time );
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_IM_IIP_SYSTEMCALL_ERR;
	}
	tmout.cur_time = tmout.start_time;
	tmout.wait_time = (DDIM_USER_TMO)wait_time;

	unitid_bitmask.bit64 = unitid_bitmask64;
	cache_bitmask = open_res_bitmask & (E_IM_IIP_OPEN_RES_CACHE0 | E_IM_IIP_OPEN_RES_CACHE1 | E_IM_IIP_OPEN_RES_CACHE2);
	other_bitmask = open_res_bitmask & ~(E_IM_IIP_OPEN_RES_CACHE0 | E_IM_IIP_OPEN_RES_CACHE1 | E_IM_IIP_OPEN_RES_CACHE2);

	while( 1 ) {
		// Unit hunt check upper 32bit
		if( unitid_bitmask.flgptn.lower != 0 ) {
			ercd = im_iip_calc_timeout( &tmout );
			if( ercd != D_IM_IIP_OK ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 1, ercd ));
				return ercd;
			}
			ercd = im_iip_twai_pol_flg( FID_IM_IIP_UNIT_LO, unitid_bitmask.flgptn.lower, D_DDIM_USER_TWF_ANDW, &unit_huntedptn, tmout.last_wait_time );
			if( D_IM_IIP_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x bitmask=0x%llx\n", 2, ercd, gIM_IIP_SwtrgMng.unit_bitmask ));
				return ercd;
			}
			ercd = DDIM_User_Set_Flg( FID_IM_IIP_UNIT_LO, unitid_bitmask.flgptn.lower );
			if( D_IM_IIP_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 3, ercd ));
				return ercd;
			}
		}



		// Unit hunt check upper 32bit
		if( unitid_bitmask.flgptn.upper != 0 ) {
			ercd = DDIM_User_Get_Tim( &tmout.cur_time );
			if( D_DDIM_USER_E_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 4, ercd ));
				return D_IM_IIP_SYSTEMCALL_ERR;
			}
			ercd = im_iip_calc_timeout( &tmout );
			if( ercd != D_IM_IIP_OK ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 1, ercd ));
				return ercd;
			}
			ercd = im_iip_twai_pol_flg( FID_IM_IIP_UNIT_HI, unitid_bitmask.flgptn.upper, D_DDIM_USER_TWF_ANDW, &unit_huntedptn, tmout.last_wait_time );
			if( D_IM_IIP_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x bitmask=0x%llx\n", 2, ercd, gIM_IIP_SwtrgMng.unit_bitmask ));
				return ercd;
			}
			ercd = DDIM_User_Set_Flg( FID_IM_IIP_UNIT_HI, unitid_bitmask.flgptn.upper );
			if( D_IM_IIP_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 3, ercd ));
				return ercd;
			}
		}



		// Pixid hunt check
		if( pixid_bitmask != 0 ) {
			ercd = DDIM_User_Get_Tim( &tmout.cur_time );
			if( D_DDIM_USER_E_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 4, ercd ));
				return D_IM_IIP_SYSTEMCALL_ERR;
			}
			ercd = im_iip_calc_timeout( &tmout );
			if( ercd != D_IM_IIP_OK ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 5, ercd ));
				return ercd;
			}
			ercd = im_iip_twai_pol_flg( FID_IM_IIP_PIX, (DDIM_USER_FLGPTN)pixid_bitmask, D_DDIM_USER_TWF_ANDW, &pix_huntedptn, tmout.last_wait_time );
			if( D_IM_IIP_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x bitmask=0x%lx\n", 6, ercd, gIM_IIP_SwtrgMng.pixfmttbl_bitmask ));
				return ercd;
			}
			ercd = DDIM_User_Set_Flg( FID_IM_IIP_PIX, (DDIM_USER_FLGPTN)pixid_bitmask );
			if( D_IM_IIP_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 7, ercd ));
				return ercd;
			}
		}



		// Other hunt check
		if( other_bitmask != 0 ) {
			ercd = DDIM_User_Get_Tim( &tmout.cur_time );
			if( D_DDIM_USER_E_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 8, ercd ));
				return D_IM_IIP_SYSTEMCALL_ERR;
			}
			ercd = im_iip_calc_timeout( &tmout );
			if( ercd != D_IM_IIP_OK ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 9, ercd ));
				return ercd;
			}
			ercd = im_iip_twai_pol_flg( FID_IM_IIP_RES, (DDIM_USER_FLGPTN)other_bitmask, D_DDIM_USER_TWF_ANDW, &other_huntedptn, tmout.last_wait_time );
			if( D_IM_IIP_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x bitmask=0x%lx\n", 11, ercd, gIM_IIP_SwtrgMng.res_bitmask ));
				return ercd;
			}
			ercd = DDIM_User_Set_Flg( FID_IM_IIP_RES, (DDIM_USER_FLGPTN)other_bitmask );
			if( D_IM_IIP_OK != ercd ) {
				Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 10, ercd ));
				return ercd;
			}
		}



		// Entry critical section
		ercd = DDIM_User_Get_Tim( &tmout.cur_time );
		if( D_DDIM_USER_E_OK != ercd ) {
			Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 12, ercd ));
			return D_IM_IIP_SYSTEMCALL_ERR;
		}
		ercd = im_iip_calc_timeout( &tmout );
		if( ercd != D_IM_IIP_OK ) {
			Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 13, ercd ));
			return ercd;
		}
		ercd = im_iip_twai_pol_sem( SID_IM_IIP_SW, tmout.last_wait_time );
		if( ercd != D_IM_IIP_OK ) {
			Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 14, ercd ));
			return ercd;
		}

		/////////////////////////
		// Begin critical section

		ercd_crit = im_iip_hunt_unit_and_pix( unitid_bitmask, pixid_bitmask, other_bitmask, cache_bitmask );

		// End critical section
		///////////////////////

		// Leave critical section
		ercd = DDIM_User_Sig_Sem( SID_IM_IIP_SW );
		if( D_DDIM_USER_E_OK != ercd ) {
			Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 15, ercd ));
			return D_IM_IIP_SYSTEMCALL_ERR;
		}



		if( ercd_crit == D_DDIM_USER_E_OK ) {
			// Resource hunt successfull. break loop.
			break;
		}
		else
		if( ercd_crit == D_DDIM_USER_E_TMOUT ) {
			// Resource already hunted other one.
			// Goto retry.
		}
		else {
			Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 16, ercd ));
			return D_IM_IIP_SYSTEMCALL_ERR;
		}



		// Goto retry

		if( wait_time == D_DDIM_USER_SEM_WAIT_POL ) {
			Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 17, ercd ));
			return D_IM_IIP_TIMEOUT_ERR;
		}

		ercd = DDIM_User_Get_Tim( &tmout.cur_time );
		if( D_DDIM_USER_E_OK != ercd ) {
			Im_IIP_Print_Pcdbg(( "Im_IIP_Open_SWTRG() err%u ercd=0x%x\n", 18, ercd ));
			return D_IM_IIP_SYSTEMCALL_ERR;
		}
	}


	return D_IM_IIP_OK;
}

/**
   Close driver. This API releases IIP's UNIT and PIXFMTTBL semaphore using SWTRG system-call.

   @param[in]	unitid_bitmask64 : UNIT ID bitmask. (D_IM_IIP_PARAM_PLDUNIT_xxx).
   @param[in]	pixid_bitmask : PIXFMTTBL ID bitmask. @ref E_IM_IIP_PIXID.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
   @retval		D_IM_IIP_SYSTEMCALL_ERR		: System-call error.
   @remarks		This API uses DDIM_User_Sig_Sem().
*/
INT32 Im_IIP_Close_SWTRG( const ULLONG unitid_bitmask64, const UINT32 pixid_bitmask, const UINT32 open_res_bitmask )
{
	DDIM_USER_ER		ercd;
	INT32				ercd_im = D_IM_IIP_OK;
	U_IM_IIP_64BIT_MASK	unitid_bitmask = {
		.bit64 = unitid_bitmask64,
	};
	UINT32				cache_bitmask;
	UINT32				other_bitmask;
	UINT32				irq_mask;
	static const char	errmsg1[] = "Im_IIP_Close_SWTRG: warning: cache %u not open.\n";

	unitid_bitmask.bit64 = unitid_bitmask64;
	cache_bitmask = open_res_bitmask & (E_IM_IIP_OPEN_RES_CACHE0 | E_IM_IIP_OPEN_RES_CACHE1 | E_IM_IIP_OPEN_RES_CACHE2);
	other_bitmask = open_res_bitmask & ~(E_IM_IIP_OPEN_RES_CACHE0 | E_IM_IIP_OPEN_RES_CACHE1 | E_IM_IIP_OPEN_RES_CACHE2);

	irq_mask = im_iip_irq_disable();

	gIM_IIP_SwtrgMng.unit_bitmask &= ~unitid_bitmask.bit64;
	gIM_IIP_SwtrgMng.pixfmttbl_bitmask &= ~pixid_bitmask;
	gIM_IIP_SwtrgMng.res_bitmask &= ~other_bitmask;
	if( (cache_bitmask & E_IM_IIP_OPEN_RES_CACHE0) != 0 ) {
		if( gIM_IIP_SwtrgMng.cache_users[0]!= 0  ) {
			gIM_IIP_SwtrgMng.cache_users[0]--;
		}
		else {
			Ddim_Print(( errmsg1, 0 ));
		}
	}
	if( (cache_bitmask & E_IM_IIP_OPEN_RES_CACHE1) != 0 ) {
		if( gIM_IIP_SwtrgMng.cache_users[1] != 0 ) {
			gIM_IIP_SwtrgMng.cache_users[1]--;
		}
		else {
			Ddim_Print(( errmsg1, 1 ));
		}
	}
	if( (cache_bitmask & E_IM_IIP_OPEN_RES_CACHE2) != 0 ) {
		if( gIM_IIP_SwtrgMng.cache_users[2] != 0 ) {
			gIM_IIP_SwtrgMng.cache_users[2]--;
		}
		else {
			Ddim_Print(( errmsg1, 2 ));
		}
	}
	im_iip_set_acesen0_reg( &gIM_IIP_SwtrgMng );

	if( gIM_IIP_SwtrgMng.unit_bitmask == 0 ) {
		ercd_im = Im_IIP_Stop( D_IM_IIP_ABORT );
	}

	im_iip_irq_enable( irq_mask );

	ercd = im_iip_unset_interrupt( unitid_bitmask, D_IM_IIP_ENABLE_OFF );
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	ercd = DDIM_User_Set_Flg( FID_IM_IIP_UNIT_LO, unitid_bitmask.flgptn.lower );
	if( D_DDIM_USER_E_OK != ercd ) {
		if( D_DDIM_USER_E_OK != ercd_im ) {
			return D_IM_IIP_SYSTEMCALL_ERR;
		}
	}

	ercd = DDIM_User_Set_Flg( FID_IM_IIP_UNIT_HI, unitid_bitmask.flgptn.upper );
	if( D_DDIM_USER_E_OK != ercd ) {
		if( D_DDIM_USER_E_OK != ercd_im ) {
			return D_IM_IIP_SYSTEMCALL_ERR;
		}
	}

	ercd = DDIM_User_Set_Flg( FID_IM_IIP_PIX, (DDIM_USER_FLGPTN)pixid_bitmask );
	if( D_DDIM_USER_E_OK != ercd ) {
		if( D_DDIM_USER_E_OK != ercd_im ) {
			return D_IM_IIP_SYSTEMCALL_ERR;
		}
	}

	ercd = DDIM_User_Set_Flg( FID_IM_IIP_RES, (DDIM_USER_FLGPTN)other_bitmask );
	if( D_DDIM_USER_E_OK != ercd ) {
		if( D_DDIM_USER_E_OK != ercd_im ) {
			return D_IM_IIP_SYSTEMCALL_ERR;
		}
	}

	return ercd_im;
}

/**
   Configure UNITINFTBL registers for SWTRG.

   @param[in]	unit_id			: UNIT ID. @ref E_IM_IIP_UNIT_ID.
   @param[in]	p_unitcfg		: Pointer of UNITINFTBL parameter. Should not be NULL.
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
INT32 Im_IIP_Ctrl_SWTRG_Unit( const E_IM_IIP_UNIT_ID unit_id, const T_IM_IIP_UNIT_CFG* const p_unitcfg )
{
	volatile struct io_iip_uinftbl* p_uinftbl;

#ifdef CO_PARAM_CHECK
	if( p_unitcfg == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_SWTRG_Unit INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif	// CO_PARAM_CHECK

	p_uinftbl = im_iip_get_unitinftbl_regaddr( unit_id );
	if( p_uinftbl == NULL ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}


	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	switch( unit_id ) {
		case E_IM_IIP_UNIT_ID_SL0:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL1:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL2:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL3:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL4:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL5:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL6:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL7:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL8:
			im_iip_set_UNITINFTBL_SL( p_unitcfg, p_uinftbl );
			break;
		default:
			im_iip_set_UNITINFTBL( p_unitcfg, p_uinftbl );
			break;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
   Set Interrupt enable bit.
   Clear Interrupt cause flag bit.

   @param[in]	interrupt_bitmask		: Interrupt bit mask.
												<ul>
													 <li>@ref D_IM_IIP_INT_FACTOR_IZAEND	[INTEN0.IZAEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_AXIERR	[INTEN0.AXEEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0END	[INTEN1.SL0FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1END	[INTEN1.SL1FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2END	[INTEN1.SL2FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3END	[INTEN1.SL3FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4END	[INTEN1.SL4FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5END	[INTEN1.SL5FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6END	[INTEN1.SL6FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL7END	[INTEN1.SL7FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL8END	[INTEN1.SL8FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0_LINEEND	[INTEN1.SL0MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1_LINEEND	[INTEN1.SL1MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2_LINEEND	[INTEN1.SL2MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3_LINEEND	[INTEN1.SL3MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4_LINEEND	[INTEN1.SL4MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5_LINEEND	[INTEN1.SL5MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6_LINEEND	[INTEN1.SL6MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL7_LINEEND	[INTEN1.SL7MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL8_LINEEND	[INTEN1.SL8MEN = 1]
#if 0	// INTIZ2/INTEN2 delete
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_AFN0	[INTEN1.REAF0F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_AFN1	[INTEN1.REAF1F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_AFN2	[INTEN1.REAF2F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_AFN3	[INTEN1.REAF3F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0	[INTEN1.REFR0F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1	[INTEN1.REFR1F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_FLT	[INTEN1.REFLTF = 1]
#endif	// INTIZ2/INTEN2 delete
												 </ul>
											   When interrupt_bit set to (D_IM_IIP_INT_FACTOR_IZAEND | D_IM_IIP_INT_FACTOR_SL0END) and paermission_flg set to 1,
											   Both INTEN0 and INTEN1 are set to 00000001h.
   @param[in]	permission_flg		: 1:Set interrupt to enable following interrupt_bit. 0:Set interrupt to disable following interrupt_bit.
   @retval		D_IM_IIP_OK					: Normal end.
*/
INT32 Im_IIP_Set_Interrupt( const ULONG interrupt_bitmask, const UCHAR permission_flg )
{
	ULONG			inten0;
	ULONG			inten1;
//	ULONG			inten2;
	ULONG			intiz0;
	ULONG			intiz1;
//	ULONG			intiz2;
	UINT32			irq_mask;

	inten0 = 0;
	intiz0 = 0;
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_IZAEND ) {
		inten0 |= D_IM_IIP_IZAEN_BIT;
		intiz0 |= D_IM_IIP_IZAFLG_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_AXIERR ) {
		inten0 |= D_IM_IIP_AXEEN_BIT;
		intiz0 |= D_IM_IIP_AXEFLG_BIT;
	}

	inten1 = 0;
	intiz1 = 0;
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL0END ) {
		inten1 |= D_IM_IIP_SL0FEN_BIT;
		intiz1 |= D_IM_IIP_SL0FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL1END ) {
		inten1 |= D_IM_IIP_SL1FEN_BIT;
		intiz1 |= D_IM_IIP_SL1FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL2END ) {
		inten1 |= D_IM_IIP_SL2FEN_BIT;
		intiz1 |= D_IM_IIP_SL2FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL3END ) {
		inten1 |= D_IM_IIP_SL3FEN_BIT;
		intiz1 |= D_IM_IIP_SL3FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL4END ) {
		inten1 |= D_IM_IIP_SL4FEN_BIT;
		intiz1 |= D_IM_IIP_SL4FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL5END ) {
		inten1 |= D_IM_IIP_SL5FEN_BIT;
		intiz1 |= D_IM_IIP_SL5FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL6END ) {
		inten1 |= D_IM_IIP_SL6FEN_BIT;
		intiz1 |= D_IM_IIP_SL6FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL7END ) {
		inten1 |= D_IM_IIP_SL7FEN_BIT;
		intiz1 |= D_IM_IIP_SL7FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL8END ) {
		inten1 |= D_IM_IIP_SL8FEN_BIT;
		intiz1 |= D_IM_IIP_SL8FF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL0_LINEEND ) {
		inten1 |= D_IM_IIP_SL0MEN_BIT;
		intiz1 |= D_IM_IIP_SL0MF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL1_LINEEND ) {
		inten1 |= D_IM_IIP_SL1MEN_BIT;
		intiz1 |= D_IM_IIP_SL1MF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL2_LINEEND ) {
		inten1 |= D_IM_IIP_SL2MEN_BIT;
		intiz1 |= D_IM_IIP_SL2MF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL3_LINEEND ) {
		inten1 |= D_IM_IIP_SL3MEN_BIT;
		intiz1 |= D_IM_IIP_SL3MF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL4_LINEEND ) {
		inten1 |= D_IM_IIP_SL4MEN_BIT;
		intiz1 |= D_IM_IIP_SL4MF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL5_LINEEND ) {
		inten1 |= D_IM_IIP_SL5MEN_BIT;
		intiz1 |= D_IM_IIP_SL5MF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL6_LINEEND ) {
		inten1 |= D_IM_IIP_SL6MEN_BIT;
		intiz1 |= D_IM_IIP_SL6MF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL7_LINEEND ) {
		inten1 |= D_IM_IIP_SL7MEN_BIT;
		intiz1 |= D_IM_IIP_SL7MF_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL8_LINEEND ) {
		inten1 |= D_IM_IIP_SL8MEN_BIT;
		intiz1 |= D_IM_IIP_SL8MF_BIT;
	}

#if 0	// INTIZ2/INTEN2 delete
	inten2 = 0;
	intiz2 = 0;
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_RINGFULL_AFN0 ) {
		inten2 |= D_IM_IIP_REAF0E_BIT;
		intiz2 |= D_IM_IIP_REAF0F_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_RINGFULL_AFN1 ) {
		inten2 |= D_IM_IIP_REAF1E_BIT;
		intiz2 |= D_IM_IIP_REAF1F_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_RINGFULL_AFN2 ) {
		inten2 |= D_IM_IIP_REAF2E_BIT;
		intiz2 |= D_IM_IIP_REAF2F_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_RINGFULL_AFN3 ) {
		inten2 |= D_IM_IIP_REAF3E_BIT;
		intiz2 |= D_IM_IIP_REAF3F_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0 ) {
		inten2 |= D_IM_IIP_REFR0E_BIT;
		intiz2 |= D_IM_IIP_REFR0F_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1 ) {
		inten2 |= D_IM_IIP_REFR1E_BIT;
		intiz2 |= D_IM_IIP_REFR1F_BIT;
	}
	if( interrupt_bitmask & D_IM_IIP_INT_FACTOR_RINGFULL_FLT ) {
		inten2 |= D_IM_IIP_REFLTE_BIT;
		intiz2 |= D_IM_IIP_REFLTF_BIT;
	}
#endif	// INTIZ2/INTEN2 delete

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	irq_mask = im_iip_irq_disable();

	if( permission_flg == D_IM_IIP_ENABLE_OFF ) {
		// disable interrupt
		IO_IIP.INTEN0.word &= ~inten0;
		IO_IIP.INTEN1.word &= ~inten1;
//		IO_IIP.INTEN2.word &= ~inten2;

		// clear interrupt
		IO_IIP.INTIZ0.word = intiz0;
		IO_IIP.INTIZ1.word = intiz1;
//		IO_IIP.INTIZ2.word = intiz2;
	}
	else {
		// clear interrupt
		IO_IIP.INTIZ0.word = intiz0;
		IO_IIP.INTIZ1.word = intiz1;
//		IO_IIP.INTIZ2.word = intiz2;

		// enable interrupt
		IO_IIP.INTEN0.word |= inten0;
		IO_IIP.INTEN1.word |= inten1;
//		IO_IIP.INTEN2.word |= inten2;
	}

	im_iip_irq_enable( irq_mask );

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();


	return D_IM_IIP_OK;
}

/**
   Set Interrupt enable bit.

   @param[in]	interrupt_bitmask	: Interrupt bit mask.
												<ul>
													 <li>@ref D_IM_IIP_INT_FACTOR_IZAEND	[INTEN0.IZAEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_AXIERR	[INTEN0.AXEEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0END	[INTEN1.SL0FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1END	[INTEN1.SL1FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2END	[INTEN1.SL2FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3END	[INTEN1.SL3FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4END	[INTEN1.SL4FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5END	[INTEN1.SL5FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6END	[INTEN1.SL6FEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2_LINEEND	[INTEN1.SL2MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3_LINEEND	[INTEN1.SL3MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4_LINEEND	[INTEN1.SL4MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5_LINEEND	[INTEN1.SL5MEN = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6_LINEEND	[INTEN1.SL6MEN = 1]
#if 0	// INTIZ2/INTEN2 delete
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_AFN0	[INTEN1.REAF0F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_AFN1	[INTEN1.REAF1F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_AFN2	[INTEN1.REAF2F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0	[INTEN1.REFR0F = 1]
													 <li>@ref D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1	[INTEN1.REFR1F = 1]
#endif	// INTIZ2/INTEN2 delete
												 </ul>
											   When interrupt_bit set to (D_IM_IIP_INT_FACTOR_IZAEND | D_IM_IIP_INT_FACTOR_SL0END) and paermission_flg set to 1,
											   Both INTEN0 and INTEN1 are set to 00000001h.
   @param[in]	int_handler		: The pointer of callback function. The callback function can be called in Im_IIP_Int_Handler.
												 If this parameter is set to NULL, Im_IIP_Int_Handler does not call callback function. <br><br>
												 Sample of callback function.<br>
												 @code
													VOID IIP_User_Callback( UINT32 interrupt_flag_mask )
													{
														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_IZAEND ) {
															// process according to IIPFLG interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_AXIERR ) {
															// process according to AXEFLG interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL0END ) {
															// process according to SL0FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL1END ) {
															// process according to SL1FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2END ) {
															// process according to SL2FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3END ) {
															// process according to SL3FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4END ) {
															// process according to SL4FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5END ) {
															// process according to SL5FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6END ) {
															// process according to SL6FF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL2_LINEEND ) {
															// process according to SL2MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL3_LINEEND ) {
															// process according to SL3MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL4_LINEEND ) {
															// process according to SL4MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL5_LINEEND ) {
															// process according to SL5MF interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_SL6_LINEEND ) {
															// process according to SL6MF interrupt
														}
#if 0	// INTIZ2/INTEN2 delete
														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_RINGFULL_AFN0 ) {
															// process according to REAF0F interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_RINGFULL_AFN1 ) {
															// process according to REAF1F interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_RINGFULL_AFN2 ) {
															// process according to REAF2F interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_RINGFULL_FRECT0 ) {
															// process according to REFR0F interrupt
														}

														if ( interrupt_flag_mask & D_IM_IIP_INT_FACTOR_RINGFULL_FRECT1 ) {
															// process according to REFR1F interrupt
														}
#endif	// INTIZ2/INTEN2 delete
													}
												 @endcode
   @retval		D_IM_IIP_OK					: Normal end.
   @retval		D_IM_IIP_INVALID_ARG_ERR	: Invalid argument error.
*/
INT32 Im_IIP_Set_IntHandler( const ULONG interrupt_bitmask, const T_IM_IIP_CALLBACK int_handler )
{
	UINT32 loopcnt;

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_0) / sizeof(gIM_IIP_Inttbl_0[0])); loopcnt++ ) {
		if( (gIM_IIP_Inttbl_0[loopcnt].int_fact & interrupt_bitmask) != 0 ) {
			gIM_IIP_Inttbl_0[loopcnt].callback_func = int_handler;
		}
	}

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_1) / sizeof(gIM_IIP_Inttbl_1[0])); loopcnt++ ) {
		if( (gIM_IIP_Inttbl_1[loopcnt].int_fact & interrupt_bitmask) != 0 ) {
			gIM_IIP_Inttbl_1[loopcnt].callback_func = int_handler;
		}
	}

#if 0	// INTIZ2/INTEN2 delete
	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_2) / sizeof(gIM_IIP_Inttbl_2[0])); loopcnt++ ) {
		if( (gIM_IIP_Inttbl_2[loopcnt].int_fact & interrupt_bitmask) != 0 ) {
			gIM_IIP_Inttbl_2[loopcnt].callback_func = int_handler;
		}
	}
#endif	// INTIZ2/INTEN2 delete

	return D_IM_IIP_OK;
}

/**
   Set interrupt line number which Store 0~8 transfers.

   @param[in]	interrupt_bitmask	: Target line interrupt bit mask.
												<ul>
													 <li>@ref D_IM_IIP_INT_FACTOR_SL0_LINEEND	[LINE_VAL_INT_SL0.LINE_VAL_INT_SL0[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL1_LINEEND	[LINE_VAL_INT_SL1.LINE_VAL_INT_SL1[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL2_LINEEND	[LINE_VAL_INT_SL2.LINE_VAL_INT_SL2[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL3_LINEEND	[LINE_VAL_INT_SL3.LINE_VAL_INT_SL3[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL4_LINEEND	[LINE_VAL_INT_SL4.LINE_VAL_INT_SL4[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL5_LINEEND	[LINE_VAL_INT_SL5.LINE_VAL_INT_SL5[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL6_LINEEND	[LINE_VAL_INT_SL6.LINE_VAL_INT_SL6[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL7_LINEEND	[LINE_VAL_INT_SL7.LINE_VAL_INT_SL7[13:0] = trans_lines]
													 <li>@ref D_IM_IIP_INT_FACTOR_SL8_LINEEND	[LINE_VAL_INT_SL8.LINE_VAL_INT_SL8[13:0] = trans_lines]
												 </ul>
   @param[in]	trans_lines			: The interrupt line number which Store 0~8 transfers.
												 Should be align on 2.
												 If this set to 0, disable interrupt which is associated with INTEN1.SLnMEN.
   @retval		D_IM_IIP_OK					: Normal end.
*/
INT32 Im_IIP_Set_SLIntLines( const ULONG interrupt_bitmask, const UINT32 trans_lines )
{
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL0_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL0.bit.LINE_VAL_INT_SL = trans_lines;
	}
	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL1_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL1.bit.LINE_VAL_INT_SL = trans_lines;
	}
	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL2_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL2.bit.LINE_VAL_INT_SL = trans_lines;
	}
	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL3_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL3.bit.LINE_VAL_INT_SL = trans_lines;
	}
	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL4_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL4.bit.LINE_VAL_INT_SL = trans_lines;
	}
	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL5_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL5.bit.LINE_VAL_INT_SL = trans_lines;
	}
	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL6_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL6.bit.LINE_VAL_INT_SL = trans_lines;
	}
	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL7_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL7.bit.LINE_VAL_INT_SL = trans_lines;
	}
	if( (interrupt_bitmask & D_IM_IIP_INT_FACTOR_SL8_LINEEND) != 0 ) {
		IO_IIP.LINE_VAL_INT_SL8.bit.LINE_VAL_INT_SL = trans_lines;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Set AXI
*/
INT32 Im_IIP_Ctrl_AXI( const E_IM_IIP_AXI_ID axi_id, const T_IM_IIP_CTRL_AXI* const cfg )
{
#ifdef CO_PARAM_CHECK
	if( axi_id >= E_IM_IIP_AXI_ID_MAX ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_AXI INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	if( cfg == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_AXI INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif	// CO_PARAM_CHECK

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	switch( axi_id ) {
		case E_IM_IIP_AXI_ID_CACHE0:
			IO_IIP.PDCCTL[0].bit.PDCPKG		= cfg->trans_req_ctrl;
			IO_IIP.PDCCTL[0].bit.PDCARCACHE	= cfg->cache_type;
			IO_IIP.PDCCTL[0].bit.PDCARPROT	= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_CACHE1:
			IO_IIP.PDCCTL[1].bit.PDCPKG		= cfg->trans_req_ctrl;
			IO_IIP.PDCCTL[1].bit.PDCARCACHE	= cfg->cache_type;
			IO_IIP.PDCCTL[1].bit.PDCARPROT	= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_CACHE2:
			IO_IIP.PDCCTL[2].bit.PDCPKG		= cfg->trans_req_ctrl;
			IO_IIP.PDCCTL[2].bit.PDCARCACHE	= cfg->cache_type;
			IO_IIP.PDCCTL[2].bit.PDCARPROT	= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_DP:
			IO_IIP.DPAXICTL.bit.DPARCACHE	= cfg->cache_type;
			IO_IIP.DPAXICTL.bit.DPARPROT	= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_AFN0:
			IO_IIP.AFNAXICTL.bit.AFN0PK			= cfg->trans_req_ctrl;
			IO_IIP.AFNAXICTL.bit.AFN0ARCACHE	= cfg->cache_type;
			IO_IIP.AFNAXICTL.bit.AFN0ARPROT		= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_AFN1:
			IO_IIP.AFNAXICTL.bit.AFN1PK			= cfg->trans_req_ctrl;
			IO_IIP.AFNAXICTL.bit.AFN1ARCACHE	= cfg->cache_type;
			IO_IIP.AFNAXICTL.bit.AFN1ARPROT		= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_AFN2:
			IO_IIP.AFNAXICTL.bit.AFN2PK			= cfg->trans_req_ctrl;
			IO_IIP.AFNAXICTL.bit.AFN2ARCACHE	= cfg->cache_type;
			IO_IIP.AFNAXICTL.bit.AFN2ARPROT		= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_AFN3:
			IO_IIP.AFNAXICTL.bit.AFN3PK			= cfg->trans_req_ctrl;
			IO_IIP.AFNAXICTL.bit.AFN3ARCACHE	= cfg->cache_type;
			IO_IIP.AFNAXICTL.bit.AFN3ARPROT		= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_FRECT0:
			IO_IIP.FRAXICTL.bit.FR0PKG			= cfg->trans_req_ctrl;
			IO_IIP.FRAXICTL.bit.FR0ARCACHE		= cfg->cache_type;
			IO_IIP.FRAXICTL.bit.FR0ARPROT		= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_FRECT1:
			IO_IIP.FRAXICTL.bit.FR1PKG			= cfg->trans_req_ctrl;
			IO_IIP.FRAXICTL.bit.FR1ARCACHE		= cfg->cache_type;
			IO_IIP.FRAXICTL.bit.FR1ARPROT		= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_GPC:
			IO_IIP.GPCAXICTL.bit.GPCARCACHE		= cfg->cache_type;
			IO_IIP.GPCAXICTL.bit.GPCARPROT		= cfg->protection_type;
			break;
		case E_IM_IIP_AXI_ID_SL0:
			IO_IIP.SLAXICTL.SLAXICTL1.bit.SL0PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL1.bit.SL0AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL1.bit.SL0AWCACHE	= cfg->cache_type;
			break;
		case E_IM_IIP_AXI_ID_SL1:
			IO_IIP.SLAXICTL.SLAXICTL1.bit.SL1PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL1.bit.SL1AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL1.bit.SL1AWCACHE	= cfg->cache_type;
			break;
		case E_IM_IIP_AXI_ID_SL2:
			IO_IIP.SLAXICTL.SLAXICTL2.bit.SL2PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL2.bit.SL2AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL2.bit.SL2AWCACHE	= cfg->cache_type;
			break;
		case E_IM_IIP_AXI_ID_SL3:
			IO_IIP.SLAXICTL.SLAXICTL2.bit.SL3PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL2.bit.SL3AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL2.bit.SL3AWCACHE	= cfg->cache_type;
			break;
		case E_IM_IIP_AXI_ID_SL4:
			IO_IIP.SLAXICTL.SLAXICTL3.bit.SL4PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL3.bit.SL4AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL3.bit.SL4AWCACHE	= cfg->cache_type;
			break;
		case E_IM_IIP_AXI_ID_SL5:
			IO_IIP.SLAXICTL.SLAXICTL3.bit.SL5PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL3.bit.SL5AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL3.bit.SL5AWCACHE	= cfg->cache_type;
			break;
		case E_IM_IIP_AXI_ID_SL6:
			IO_IIP.SLAXICTL.SLAXICTL4.bit.SL6PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL4.bit.SL6AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL4.bit.SL6AWCACHE	= cfg->cache_type;
			break;
		case E_IM_IIP_AXI_ID_SL7:
			IO_IIP.SLAXICTL.SLAXICTL4.bit.SL7PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL4.bit.SL7AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL4.bit.SL7AWCACHE	= cfg->cache_type;
			break;
		case E_IM_IIP_AXI_ID_SL8:
			IO_IIP.SLAXICTL.SLAXICTL5.bit.SL8PKG		= cfg->trans_req_ctrl;
			IO_IIP.SLAXICTL.SLAXICTL5.bit.SL8AWPROT		= cfg->protection_type;
			IO_IIP.SLAXICTL.SLAXICTL5.bit.SL8AWCACHE	= cfg->cache_type;
			break;
		default:
			break;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Set ALL AXI
*/
INT32 Im_IIP_Ctrl_AXI_ALL( const T_IM_IIP_CTRL_AXI_ALL* const cfg )
{
#ifdef CO_PARAM_CHECK
	if( cfg == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_AXI_ALL INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	if( cfg->imgcache0 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_CACHE0, cfg->imgcache0 );
	}

	if( cfg->imgcache1 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_CACHE1, cfg->imgcache1 );
	}

	if( cfg->imgcache2 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_CACHE2, cfg->imgcache2 );
	}

	if( cfg->dp != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_DP, cfg->dp );
	}

	if( cfg->afn0 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_AFN0, cfg->afn0 );
	}

	if( cfg->afn1 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_AFN1, cfg->afn1 );
	}

	if( cfg->afn2 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_AFN2, cfg->afn2 );
	}

	if( cfg->afn3 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_AFN3, cfg->afn3 );
	}

	if( cfg->frect0 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_FRECT0, cfg->frect0 );
	}

	if( cfg->frect1 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_FRECT1, cfg->frect1 );
	}

	if( cfg->gpc != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_GPC, cfg->gpc );
	}

	if( cfg->sl0 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL0, cfg->sl0 );
	}

	if( cfg->sl1 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL1, cfg->sl1 );
	}

	if( cfg->sl2 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL2, cfg->sl2 );
	}

	if( cfg->sl3 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL3, cfg->sl3 );
	}

	if( cfg->sl4 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL4, cfg->sl4 );
	}

	if( cfg->sl5 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL5, cfg->sl5 );
	}

	if( cfg->sl6 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL6, cfg->sl6 );
	}

	if( cfg->sl7 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL7, cfg->sl7 );
	}

	if( cfg->sl8 != NULL ) {
		Im_IIP_Ctrl_AXI( E_IM_IIP_AXI_ID_SL8, cfg->sl8 );
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Set flame mode, bank control, link control, cache control and interrupt control.
*/
INT32 Im_IIP_Ctrl_Base( const T_IM_IIP_CTRL_BASE* const cfg )
{
	UINT32	loopcnt;

#ifdef CO_PARAM_CHECK
	if( cfg == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Ctrl_Base INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

#if 0	// B2Y Link not support
	// link control
	IO_IIP.IZACTL1.bit.LKEN = cfg->b2y_link;
#endif	// B2Y Link not support

	// cache control
	for( loopcnt = 0; loopcnt < D_IM_IIP_CACHE_MAXNUM; loopcnt++ ) {
		IO_IIP.PDCCTL[loopcnt].bit.FLHALL = cfg->cache_flush_ctrl[loopcnt];
		IO_IIP.PDCCTL[loopcnt].bit.EXTBIT = cfg->ext_bit_ctrl_less_8bits[loopcnt];
	}

	// interrupt control
	gIM_IIP_Callback = cfg->int_handler;

	IO_IIP.LINE_VAL_INT_SL0.bit.LINE_VAL_INT_SL = cfg->sl0_interrupt_lines;
	IO_IIP.LINE_VAL_INT_SL1.bit.LINE_VAL_INT_SL = cfg->sl1_interrupt_lines;
	IO_IIP.LINE_VAL_INT_SL2.bit.LINE_VAL_INT_SL = cfg->sl2_interrupt_lines;
	IO_IIP.LINE_VAL_INT_SL3.bit.LINE_VAL_INT_SL = cfg->sl3_interrupt_lines;
	IO_IIP.LINE_VAL_INT_SL4.bit.LINE_VAL_INT_SL = cfg->sl4_interrupt_lines;
	IO_IIP.LINE_VAL_INT_SL5.bit.LINE_VAL_INT_SL = cfg->sl5_interrupt_lines;
	IO_IIP.LINE_VAL_INT_SL6.bit.LINE_VAL_INT_SL = cfg->sl6_interrupt_lines;
	IO_IIP.LINE_VAL_INT_SL7.bit.LINE_VAL_INT_SL = cfg->sl7_interrupt_lines;
	IO_IIP.LINE_VAL_INT_SL8.bit.LINE_VAL_INT_SL = cfg->sl8_interrupt_lines;

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return Im_IIP_Set_Interrupt( cfg->enable_interrupt_flag, D_IM_IIP_ENABLE_ON );
}

/**
*	Update PADRS
*/
INT32 Im_IIP_Update_PADRS( const E_IM_IIP_UNIT_ID unit_id, const ULONG unit_param_addr )
{
	volatile struct io_iip_uinftbl* p_uinftbl;

	p_uinftbl = im_iip_get_unitinftbl_regaddr( unit_id );
	if( p_uinftbl == NULL ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}


	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	im_iip_update_UNITINFTBL_PADRS( unit_param_addr, &p_uinftbl->UNITINFTBL2 );

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();


	return D_IM_IIP_OK;
}

/**
*	Start IIP
*/
INT32 Im_IIP_Start( VOID )
{
	UINT32 irq_mask;

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	irq_mask = im_iip_irq_disable();

	if( IO_IIP.IZACTL1.bit.TRG == D_IM_IIP_STOPPED ) {
		im_iip_On_Iipclk();
		im_iip_On_Iclk();
		Im_IIP_Dsb();

		im_iip_On_izarac_caxrac();

		Im_IIP_Dsb();
		im_iip_wait_usec( D_IM_IIP_SRAM_WAIT_USEC );

		IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_START;

#ifdef CO_DEBUG_ON_PC
		IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_RUNNING;
		IO_IIP.IZACTL2.bit.ABORT = D_IM_IIP_RUNNING;
		Ddim_Print(( "Im_IIP_Start: IZACTL1.bit.TRG = %u\n", IO_IIP.IZACTL1.bit.TRG ));
#endif //CO_DEBUG_ON_PC
	}
	else {
		IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_START;
#ifdef CO_DEBUG_ON_PC
		IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_RUNNING;
		IO_IIP.IZACTL2.bit.ABORT = D_IM_IIP_RUNNING;
		Ddim_Print(( "Im_IIP_Start: IZACTL1.bit.TRG = %u\n", IO_IIP.IZACTL1.bit.TRG ));
#endif //CO_DEBUG_ON_PC
	}

	im_iip_irq_enable( irq_mask );

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	IIP SWTRG
*/
INT32 Im_IIP_Start_SWTRG( const E_IM_IIP_UNIT_ID unit_id, const INT32 swtrg )
{
	return Im_IIP_LinkStart_SWTRG( unit_id, swtrg, D_IM_IIP_ENABLE_OFF );
}

INT32 Im_IIP_LinkStart_SWTRG( const E_IM_IIP_UNIT_ID unit_id, const INT32 swtrg, const UINT32 link_onoff )
{
	INT32							retval = D_IM_IIP_OK;
	volatile struct io_iip_uinftbl*	p_uinftbl;
	UINT32							loopcnt;
	UINT32							irq_mask;

#ifdef CO_PARAM_CHECK
	if( (swtrg != D_IM_IIP_SWTRG_ON) && (swtrg != D_IM_IIP_SWTRG_OFF) ) {
		Ddim_Assertion(("I:Im_IIP_SWTRG INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif


	Im_IIP_On_Pclk();
	Im_IIP_Dsb();


	// Start IIP macro when IZACTL1.TRG == stop
	irq_mask = im_iip_irq_disable();

	if( IO_IIP.IZACTL1.bit.TRG == D_IM_IIP_STOPPED ) {
		im_iip_On_Iipclk();
		im_iip_On_Iclk();
		Im_IIP_Dsb();

		for( loopcnt = 0; loopcnt < D_IM_IIP_CACHE_MAXNUM; loopcnt++ ) {
			IO_IIP.PDCCTL[loopcnt].bit.FLHALL = D_IM_IIP_FLUSH_IMGCACHE_START;
		}

		im_iip_On_izarac_caxrac();

		Im_IIP_Dsb();
		im_iip_wait_usec( D_IM_IIP_SRAM_WAIT_USEC );

		IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_START;

#ifdef CO_DEBUG_ON_PC
		IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_RUNNING;
		IO_IIP.IZACTL2.bit.ABORT = D_IM_IIP_RUNNING;
#endif //CO_DEBUG_ON_PC

		Im_IIP_Print_Pcdbg(( "Im_IIP_(Link)Start_SWTRG: IZACTL1.bit.TRG = %u\n", IO_IIP.IZACTL1.bit.TRG ));
	}
	else {
		if( link_onoff != D_IM_IIP_ENABLE_OFF ) {
			IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_START;
#ifdef CO_DEBUG_ON_PC
			IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_RUNNING;
#endif //CO_DEBUG_ON_PC

			Im_IIP_Print_Pcdbg(( "Im_IIP_LinkStart_SWTRG: IZACTL1.bit.TRG = %u\n", IO_IIP.IZACTL1.bit.TRG ));
		}
	}

	im_iip_irq_enable( irq_mask );

	// Start Unit
	p_uinftbl = im_iip_get_unitinftbl_regaddr( unit_id );
	if( p_uinftbl == NULL ) {
		retval = D_IM_IIP_INVALID_ARG_ERR;
	}
	else {
		retval = im_iip_control_SWTRG( &p_uinftbl->UNITINFTBL0, swtrg );
	}


	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return retval;
}

/**
*	Stop IIP
*/
INT32 Im_IIP_Stop( const UINT32 stop_type )
{
	INT32			retval = D_IM_IIP_OK;
	DDIM_USER_ER	ercd;
	UINT32			loopcnt;
	UINT32			irq_mask;

	im_iip_On_Iipclk();
	im_iip_On_Iclk();
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	irq_mask = im_iip_irq_disable();

	if( IO_IIP.IZACTL1.bit.TRG == D_IM_IIP_RUNNING ) {
		if( stop_type == D_IM_IIP_STOP ) {
			IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_STOP;

#ifdef CO_DEBUG_ON_PC
			IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_STOPPED;
			IO_IIP.IZACTL2.bit.ABORT = D_IM_IIP_STOPPED;
			Im_IIP_Print_Pcdbg(( "Im_IIP_Stop: IZACTL1.bit.TRG = %u\n", IO_IIP.IZACTL1.bit.TRG ));
#endif //CO_DEBUG_ON_PC

			if( IO_IIP.IZACTL1.bit.TRG == D_IM_IIP_STOPPED ) {
				// off clock on from Im_IIP_Start() or Im_IIP_Start_SWTRG().
				im_iip_Off_izarac_caxrac();
				im_iip_Off_Iclk();
				im_iip_Off_Iipclk();
			}
		}
		else if( stop_type == D_IM_IIP_ABORT ) {
			IO_IIP.IZACTL2.bit.ABORT = 0;

#ifdef CO_DEBUG_ON_PC
			IO_IIP.IZACTL1.bit.TRG = D_IM_IIP_STOPPED;
			IO_IIP.IZACTL2.bit.ABORT = D_IM_IIP_STOPPED;
			Im_IIP_Print_Pcdbg(( "Im_IIP_Stop: IZACTL1.bit.ABORT = %u\n", IO_IIP.IZACTL2.bit.ABORT ));
#endif //CO_DEBUG_ON_PC

			for( loopcnt = 0; loopcnt < D_IM_IIP_TRIG_STOP_CHK_RETRY_COUNT; loopcnt++ ) {
				if( IO_IIP.IZACTL1.bit.TRG == D_IM_IIP_STOPPED ) {
					break;
				}
			}
			if( loopcnt >= D_IM_IIP_TRIG_STOP_CHK_RETRY_COUNT ) {
				retval = D_IM_IIP_ABORT_FAILED_ERR;
				Ddim_Print(("I:Im_IIP_Stop abort Failed.\n"));
//				Ddim_Assertion(("I:Im_IIP_Stop abort Failed.\n"));
			}

			// off clock on from Im_IIP_Start() or Im_IIP_Start_SWTRG().
			im_iip_Off_izarac_caxrac();
			im_iip_Off_Iclk();
			im_iip_Off_Iipclk();
		}
		else {
			retval = D_IM_IIP_INVALID_ARG_ERR;
		}
	}

	if( (stop_type == D_IM_IIP_ABORT) && (retval == D_IM_IIP_OK) ) {
		// Clear all interrupt flag.
		// And **ENABLE** Interrupt in service call.
		ercd = DDIM_User_Clr_Flg( FID_IM_IIP, ~D_IM_IIP_FLG_USED_BIT_PATTERN );
		if( ercd != D_DDIM_USER_E_OK ) {
			retval = D_IM_IIP_SYSTEMCALL_ERR;
		}
		else {
		}
	}

	im_iip_irq_enable( irq_mask );

	Im_IIP_Off_Pclk();
	im_iip_Off_Iclk();
	im_iip_Off_Iipclk();
	Im_IIP_Dsb();

	return retval;
}

/**
*	Wait
*/
INT32 Im_IIP_Wait_End( UINT32* const p_wait_factor, const UINT32 wait_factor, const UINT32 wait_mode, const INT32 wait_time )
{
	DDIM_USER_ER		ercd;
	DDIM_USER_FLGPTN	flgptn;

#ifdef CO_PARAM_CHECK
	if( (p_wait_factor == NULL) ||
		 (im_iip_check_wait_time(wait_time) == D_IM_IIP_FALSE) ) {
		Ddim_Assertion(("I:Im_IIP_Wait_End INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

#ifdef CO_DEBUG_ON_PC
	im_iip_pseudo_interrupt( wait_factor );
#endif

	ercd = DDIM_User_Twai_Flg( FID_IM_IIP, wait_factor, wait_mode, &flgptn, wait_time );
	switch( ercd ) {
		case D_DDIM_USER_E_OK:
			break;
		case D_DDIM_USER_E_TMOUT:
			return D_IM_IIP_TIMEOUT_ERR;
		default:
			return D_IM_IIP_SYSTEMCALL_ERR;
	}

	ercd = DDIM_User_Clr_Flg( FID_IM_IIP, ~(flgptn & wait_factor) );
	if( ercd != D_DDIM_USER_E_OK ) {
		return D_IM_IIP_SYSTEMCALL_ERR;
	}

	*p_wait_factor = D_IM_IIP_FLG_USED_BIT_PATTERN & (flgptn & wait_factor);

	return D_IM_IIP_OK;
}

/**
*	Interrupt Handler
*/
INT32 Im_IIP_Int_Handler( VOID )
{
	ULONG			intiz0;
	ULONG			intiz1;
//	ULONG			intiz2;
	UINT32			int_factor = 0;
	DDIM_USER_ER	ercd;
	UINT32			loopcnt;

#ifdef CO_DEBUG_PRINT_IM_IIP
		Ddim_Print(( "Im_IIP_Int_Handler begin\n" ));
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	// Get interrupt
	intiz0 = IO_IIP.INTIZ0.word & IO_IIP.INTEN0.word;
	intiz1 = IO_IIP.INTIZ1.word & IO_IIP.INTEN1.word;
//	intiz2 = IO_IIP.INTIZ2.word & IO_IIP.INTEN2.word;
	IO_IIP.INTIZ0.word = intiz0;
	IO_IIP.INTIZ1.word = intiz1;
//	IO_IIP.INTIZ2.word = intiz2;
#ifdef CO_DEBUG_ON_PC
	IO_IIP.INTIZ0.word &= ~intiz0;
	IO_IIP.INTIZ1.word &= ~intiz1;
//	IO_IIP.INTIZ2.word &= ~intiz2;
#endif // CO_DEBUG_ON_PC

	// Process for each interrupt


#if 0	// INTIZ2/INTEN2 delete
	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_2) / sizeof(gIM_IIP_Inttbl_2[0])); loopcnt++ ) {
		if( (intiz2 & gIM_IIP_Inttbl_2[loopcnt].flg_bitmask) != 0 ) {
			int_factor |= gIM_IIP_Inttbl_2[loopcnt].int_fact;
			if( gIM_IIP_Inttbl_2[loopcnt].callback_func != NULL ) {
				gIM_IIP_Inttbl_2[loopcnt].callback_func( gIM_IIP_Inttbl_2[loopcnt].int_fact );
			}
		}
	}
#endif	// INTIZ2/INTEN2 delete

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_1) / sizeof(gIM_IIP_Inttbl_1[0])); loopcnt++ ) {
		if( (intiz1 & gIM_IIP_Inttbl_1[loopcnt].flg_bitmask) != 0 ) {
			int_factor |= gIM_IIP_Inttbl_1[loopcnt].int_fact;
			if( gIM_IIP_Inttbl_1[loopcnt].callback_func != NULL ) {
				gIM_IIP_Inttbl_1[loopcnt].callback_func( gIM_IIP_Inttbl_1[loopcnt].int_fact );
			}
		}
	}

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_0) / sizeof(gIM_IIP_Inttbl_0[0])); loopcnt++ ) {
		if( (intiz0 & gIM_IIP_Inttbl_0[loopcnt].flg_bitmask) != 0 ) {
			int_factor |= gIM_IIP_Inttbl_0[loopcnt].int_fact;
			if( gIM_IIP_Inttbl_0[loopcnt].callback_func != NULL ) {
				gIM_IIP_Inttbl_0[loopcnt].callback_func( gIM_IIP_Inttbl_0[loopcnt].int_fact );
			}
		}
	}


	Im_IIP_Dsb();

#ifdef CO_DEBUG_PRINT_IM_IIP
	Ddim_Print(( "Im_IIP_Int_Handler IZ0=0x%lx IZ1=0x%lx IZ2=0x%lx EN0=0x%lx EN1=0x%lx EN2=0x%lx factor=0x%x\n", intiz0, intiz1, intiz2, inten0, inten1, inten2, int_factor ));
#endif

	ercd = DDIM_User_Set_Flg( FID_IM_IIP, int_factor );
	if( ercd != D_DDIM_USER_E_OK ) {
		Im_IIP_Off_Pclk();
		Im_IIP_Dsb();
		return D_IM_IIP_SYSTEMCALL_ERR;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	if( gIM_IIP_Callback != NULL ) {
#ifdef CO_DEBUG_PRINT_IM_IIP
		Ddim_Print(( "Im_IIP_Int_Handler begin callback\n" ));
#endif
		gIM_IIP_Callback( int_factor );
#ifdef CO_DEBUG_PRINT_IM_IIP
		Ddim_Print(( "Im_IIP_Int_Handler end callback\n" ));
#endif
	}

#ifdef CO_DEBUG_PRINT_IM_IIP
		Ddim_Print(( "Im_IIP_Int_Handler end\n" ));
#endif

	return D_IM_IIP_OK;
}

/**
*	Clear HISTGRAM info
*/
INT32 Im_IIP_Clear_HISTGRAM( const UINT32 histgram_a_clr_flag, const UINT32 histgram_b_clr_flag, const UINT32 histgram_c_clr_flag, const UINT32 histgram_d_clr_flag )
{
	ULONG	hissts;
	INT32	retval;

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	Im_IIP_On_Hclk();
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	hissts = IO_IIP.AHBSTS.bit.HISSTS;
	if( hissts != 0 ) {
		retval = D_IM_IIP_ERR;
	}
	else {
		IO_IIP.MONINIT.bit.INIT = 1;

		im_iip_wait_usec( 1 );

		if( 0 != histgram_a_clr_flag ) {
			im_iip_fill_word( &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[0][0],
							  0,
							  D_IM_IIP_NUM_HISTGRAM,
							  0x0UL );
		}

		if( 0 != histgram_b_clr_flag ) {
			im_iip_fill_word( &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[1][0],
							  0,
							  D_IM_IIP_NUM_HISTGRAM,
							  0x0UL );
		}

		if( 0 != histgram_c_clr_flag ) {
			im_iip_fill_word( &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[2][0],
							  0,
							  D_IM_IIP_NUM_HISTGRAM,
							  0x0UL );
		}

		if( 0 != histgram_d_clr_flag ) {
			im_iip_fill_word( &IO_IIP_TBL.HISTGRAM.HISTGRAM.word[3][0],
							  0,
							  D_IM_IIP_NUM_HISTGRAM,
							  0x0UL );
		}

		retval = D_IM_IIP_OK;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Off_Hclk();
	Im_IIP_Dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return retval;
}

/**
*	Get HISTGRAM info
*/
INT32 Im_IIP_Get_HISTGRAM( ULONG* const buffer_histgram_a, ULONG* const buffer_histgram_b, ULONG* const buffer_histgram_c, ULONG* const buffer_histgram_d )
{
	ULONG	hissts;
	INT32	retval;

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	Im_IIP_On_Hclk();
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	hissts = IO_IIP.AHBSTS.bit.HISSTS;
	if( hissts != 0 ) {
		retval = D_IM_IIP_ERR;
	}
	else {
		im_iip_wait_usec( 1 );

		if( buffer_histgram_a != NULL ) {
			im_iip_get_regword( buffer_histgram_a,	// Read access only support Word-width. (Hardware specification.)
								IO_IIP_TBL.HISTGRAM.HISTGRAM.word[0],
								D_IM_IIP_HISTGRAM_BYTES );
		}

		if( buffer_histgram_b != NULL ) {
			im_iip_get_regword( buffer_histgram_b,	// Read access only support Word-width. (Hardware specification.)
								IO_IIP_TBL.HISTGRAM.HISTGRAM.word[1],
								D_IM_IIP_HISTGRAM_BYTES );
		}

		if( buffer_histgram_c != NULL ) {
			im_iip_get_regword( buffer_histgram_c,	// Read access only support Word-width. (Hardware specification.)
								IO_IIP_TBL.HISTGRAM.HISTGRAM.word[2],
								D_IM_IIP_HISTGRAM_BYTES );
		}

		if( buffer_histgram_d != NULL ) {
			im_iip_get_regword( buffer_histgram_d,	// Read access only support Word-width. (Hardware specification.)
								IO_IIP_TBL.HISTGRAM.HISTGRAM.word[3],
								D_IM_IIP_HISTGRAM_BYTES );
		}
		retval = D_IM_IIP_OK;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Off_Hclk();
	Im_IIP_Dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return retval;
}

/**
*	Get HISTMAX info
*/
INT32 Im_IIP_Get_HISTMAX( UCHAR* const histmax_a, UCHAR* const histmax_b, UCHAR* const histmax_c, UCHAR* const histmax_d )
{
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	if( histmax_a != NULL ) {
		*histmax_a = IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_0;
	}

	if( histmax_b != NULL ) {
		*histmax_b = IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_1;
	}

	if( histmax_c != NULL ) {
		*histmax_c = IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_2;
	}

	if( histmax_d != NULL ) {
		*histmax_d = IO_IIP.HISTMAX.HISTMAX1.bit.HISTMAX_3;
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Get MAXMON info
*/
INT32 Im_IIP_Get_MAXMON( T_IM_IIP_MAXMON* const mon )
{
	UINT32 loopcnt;

#ifdef CO_PARAM_CHECK
	if( mon == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Get_MAXMON INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	for( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		mon->max[loopcnt].value = IO_IIP.MAXMON[loopcnt].bit.MAXVAL;
		mon->max[loopcnt].pos_x = IO_IIP.MAXMON[loopcnt].bit.HPOINTMAX;
		mon->max[loopcnt].pos_y = IO_IIP.MAXMON[loopcnt].bit.VPOINTMAX;
		im_iip_get_reg_signed_a( mon->max[loopcnt].value, IO_IIP.MAXMON[loopcnt], union io_iip_maxmon, MAXVAL );
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Get MINMON info
*/
INT32 Im_IIP_Get_MINMON( T_IM_IIP_MINMON* const mon )
{
	UINT32 loopcnt;

#ifdef CO_PARAM_CHECK
	if( mon == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Get_MINMON INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	for( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		mon->min[loopcnt].pos_x = IO_IIP.MINMON[loopcnt].bit.HPOINTMIN;
		mon->min[loopcnt].pos_y = IO_IIP.MINMON[loopcnt].bit.VPOINTMIN;
		im_iip_get_reg_signed_a( mon->min[loopcnt].value, IO_IIP.MINMON[loopcnt], union io_iip_minmon, MINVAL );
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Get ADDMON info
*/
INT32 Im_IIP_Get_ADDMON( T_IM_IIP_ADDMON* const mon )
{
	UINT32 loopcnt;

#ifdef CO_PARAM_CHECK
	if( mon == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Get_ADDMON INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	for( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		mon->add[loopcnt].pos_x = IO_IIP.ADDMON[loopcnt].bit.HPOINTADD;
		mon->add[loopcnt].pos_y = IO_IIP.ADDMON[loopcnt].bit.VPOINTADD;
		mon->add[loopcnt].value = IO_IIP.ADDMON[loopcnt].bit.ADDVAL;
		im_iip_get_reg_signed_a( mon->add[loopcnt].value, IO_IIP.ADDMON[loopcnt], union io_iip_addmon, ADDVAL );
	}

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Get Image Cache Access info
*/
INT32 Im_IIP_Get_ImgCache_Access_Info( const E_IM_IIP_CACHE_CH cache_ch, T_IM_IIP_ACCESS_INFO* const info )
{
#ifdef CO_PARAM_CHECK
	if( info == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Get_ImgCache_Access_Info INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	if( cache_ch >= E_IM_IIP_CACHE_CH_MAX ) {
		Ddim_Assertion(("I:Im_IIP_Get_ImgCache_Access_Info INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	info->real_access_counter	= IO_IIP.PDCRAC[cache_ch].word;
	info->real_miss_counter		= IO_IIP.PDCRMC[cache_ch].word;

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Get AXI Status info
*/
INT32 Im_IIP_Get_AXI_Status( T_IM_IIP_AXI_STATUS* const sts )
{
#ifdef CO_PARAM_CHECK
	if( sts == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Get_AXI_Status INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	sts->read_channel_status  = IO_IIP.AXRSTS.word;
	sts->write_channel_status = IO_IIP.AXWSTS.word;

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Get SL UNIT status
*/
INT32 Im_IIP_Get_SL_Status( UINT32* const sl_status )
{
#ifdef CO_PARAM_CHECK
	if( sl_status == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Get_SL_Status INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	*sl_status = IO_IIP.IZACTL1.bit.SLALE;

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

/**
*	Get UNIT PARAMETER
*/
INT32 Im_IIP_Get_UNIT_PARAM( const E_IM_IIP_UNIT_ID unit_id, ULONG* const p_param_buffer, UINT32* const param_size )
{
	UINT32			size;
	INT32			result;
	volatile ULONG* regaddr;

#ifdef CO_PARAM_CHECK
	if( (p_param_buffer == NULL) ||
	     (param_size == NULL) ) {
		Ddim_Assertion(("I:Im_IIP_Get_UNIT_PARAM INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	im_iip_get_unit_param_size( &size, unit_id );
	if( size == D_IM_IIP_UNITPARAM_SIZE_ERROR ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}

	im_iip_On_Iipclk();
	im_iip_On_Iclk();
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	while( 1 ) {
		if( IO_IIP.IZACTL1.bit.TRG != D_IM_IIP_STOPPED ) {
			result = D_IM_IIP_ERR;
			break;
		}


		regaddr = (volatile ULONG*)(((ULONG)&IO_IIP) + D_IM_IIP_UNITPARAM_BASEADDR);

		IO_IIP.IZACTL1.bit.SELRDREG_UNIT = unit_id;

		IO_IIP.IZACTL1.bit.DBEN = 1;
		im_iip_get_regword( p_param_buffer, regaddr, size );
		IO_IIP.IZACTL1.bit.DBEN = 0;

		*param_size = size;

		result = D_IM_IIP_OK;
		break;
	}

	Im_IIP_Off_Pclk();
	im_iip_Off_Iclk();
	im_iip_Off_Iipclk();
	Im_IIP_Dsb();

	return result;
}

/**
   Set the access enable register (ACESEN0).

   @param[in]	res_bitmask				: Target resource bitmask.<br>
											<ul>
												<li>	E_IM_IIP_OPEN_RES_LUT_A		[ACESEN0.LUARAC]
												<li>	E_IM_IIP_OPEN_RES_LUT_B		[ACESEN0.LUARBC]
												<li>	E_IM_IIP_OPEN_RES_LUT_C		[ACESEN0.LUARCC]
												<li>	E_IM_IIP_OPEN_RES_LUT_D		[ACESEN0.LUARDC]
												<li>	E_IM_IIP_OPEN_RES_LUT_E		[ACESEN0.LUAREC]
												<li>	E_IM_IIP_OPEN_RES_LUT_F		[ACESEN0.LUARFC]
												<li>	E_IM_IIP_OPEN_RES_HIST_0	[ACESEN0.MO0RAC]
												<li>	E_IM_IIP_OPEN_RES_HIST_1	[ACESEN0.MO1RAC]
												<li>	E_IM_IIP_OPEN_RES_HIST_2	[ACESEN0.MO2RAC]
												<li>	E_IM_IIP_OPEN_RES_HIST_3	[ACESEN0.MO3RAC]
												<li>	E_IM_IIP_OPEN_RES_CACHE1	[ACESEN0.CA1RAC]
												<li>	E_IM_IIP_OPEN_RES_CACHE2	[ACESEN0.CA2RAC]
											</ul>

   @param[in]	permission_flg			: Access permission. 0:register set to 0.  1:register set to 1.
   @retval		D_IM_IIP_OK				: Normal end.
*/
INT32 Im_IIP_Set_AccessEnable( const UINT32 res_bitmask, const UCHAR permission_flg )
{
	UINT32 irq_mask;

	irq_mask = im_iip_irq_disable();

	Im_IIP_On_Pclk();

	if( (res_bitmask & E_IM_IIP_OPEN_RES_CACHE1) != 0 ) {
		IO_IIP.ACESEN0.bit.CA1RAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_CACHE2) != 0 ) {
		IO_IIP.ACESEN0.bit.CA2RAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_LUT_A) != 0 ) {
		IO_IIP.ACESEN0.bit.LUARAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_LUT_B) != 0 ) {
		IO_IIP.ACESEN0.bit.LUBRAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_LUT_C) != 0 ) {
		IO_IIP.ACESEN0.bit.LUCRAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_LUT_D) != 0 ) {
		IO_IIP.ACESEN0.bit.LUDRAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_LUT_E) != 0 ) {
		IO_IIP.ACESEN0.bit.LUERAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_LUT_F) != 0 ) {
		IO_IIP.ACESEN0.bit.LUFRAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_HIST_0) != 0 ) {
		IO_IIP.ACESEN0.bit.MO0RAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_HIST_1) != 0 ) {
		IO_IIP.ACESEN0.bit.MO1RAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_HIST_2) != 0 ) {
		IO_IIP.ACESEN0.bit.MO2RAC = permission_flg;
	}
	if( (res_bitmask & E_IM_IIP_OPEN_RES_HIST_3) != 0 ) {
		IO_IIP.ACESEN0.bit.MO3RAC = permission_flg;
	}

	Im_IIP_Off_Pclk();

	im_iip_irq_enable( irq_mask );

	if( permission_flg != 0 ) {
		Im_IIP_Dsb();
		im_iip_wait_usec( D_IM_IIP_SRAM_WAIT_USEC );
	}

	return D_IM_IIP_OK;
}

#if 0	// B2Y Link not support
/**
   Set Link enable register (LKEN).

   @param[in]	b2y_link			: <ul>
   										<li> @ref D_IM_IIP_ENABLE_ON : B2Y Link enable
   										<li> @ref D_IM_IIP_ENABLE_OFF : B2Y Linke disable
									  </ul>
   @retval		D_IM_IIP_OK				: Normal end.
*/
INT32 Im_IIP_Set_B2YLinkEnable( const UCHAR b2y_link )
{
	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	// link control
	IO_IIP.IZACTL1.bit.LKEN = b2y_link;

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}
#endif	// B2Y Link not support

INT32 Im_IIP_Get_Genline( T_IM_IIP_GENLINE* const genline )
{
#ifdef CO_PARAM_CHECK
	if( genline == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Get_Genline INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	genline->b2y_outlineno = IO_IIP.GENLINE.GENLINE1.bit.B2Y_OUTLINENO;
	genline->afn_outlineno[0] = IO_IIP.GENLINE.GENLINE1.bit.AFN0_OUTLINENO;
	genline->afn_outlineno[1] = IO_IIP.GENLINE.GENLINE1.bit.AFN1_OUTLINENO;
	genline->afn_outlineno[2] = IO_IIP.GENLINE.GENLINE1.bit.AFN2_OUTLINENO;
	genline->afn_outlineno[3] = IO_IIP.GENLINE.GENLINE2.bit.AFN3_OUTLINENO;
	genline->frect_outlineno[0] = IO_IIP.GENLINE.GENLINE2.bit.FR0_OUTLINENO;
	genline->frect_outlineno[1] = IO_IIP.GENLINE.GENLINE2.bit.FR1_OUTLINENO;
	genline->ld_outlineno[0] = IO_IIP.GENLINE.GENLINE2.bit.LD0_OUTLINENO;
	genline->ld_outlineno[1] = IO_IIP.GENLINE.GENLINE3.bit.LD1_OUTLINENO;
	genline->ld_outlineno[2] = IO_IIP.GENLINE.GENLINE3.bit.LD2_OUTLINENO;
	genline->ld_outlineno[3] = IO_IIP.GENLINE.GENLINE3.bit.LD3_OUTLINENO;
	genline->ld_outlineno[4] = IO_IIP.GENLINE.GENLINE3.bit.LD4_OUTLINENO;
	genline->ld_outlineno[5] = IO_IIP.GENLINE.GENLINE4.bit.LD5_OUTLINENO;
	genline->ld_outlineno[6] = IO_IIP.GENLINE.GENLINE4.bit.LD6_OUTLINENO;
	genline->flt_outlineno = IO_IIP.GENLINE.GENLINE4.bit.FLT_OUTLINENO;

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}

INT32 Im_IIP_Get_Debug_Monitor( T_IM_IIP_DEBUG_MONITOR* const monitor_val )
{
	union {
		ULLONG					data_out_port;
		union io_iip_dtout_mon	dtout_mon;
	} u_dtout_mon = {
		.data_out_port = 0ULL,
	};
	union {
		ULLONG					waiting_port;
		union io_iip_wait_mon	wait_mon;
	} u_wait_mon = {
		.waiting_port = 0ULL,
	};

#ifdef CO_PARAM_CHECK
	if( monitor_val == NULL ) {
		Ddim_Assertion(("I:Im_IIP_Get_Debug_Monitor INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	Im_IIP_On_Pclk();
	Im_IIP_Dsb();

	u_dtout_mon.dtout_mon = IO_IIP.DTOUT_MON;		// Force 32bit access
	monitor_val->data_out_port = u_dtout_mon.data_out_port;
	u_wait_mon.wait_mon = IO_IIP.WAIT_MON;			// Force 32bit access
	monitor_val->waiting_port  = u_wait_mon.waiting_port;
	monitor_val->imgcache_data_out[0] = IO_IIP.PDC_DTOUT_MON.PDC_DTOUT_MON1.bit.PDC_DTOUT_MON_0;
	monitor_val->imgcache_data_out[1] = IO_IIP.PDC_DTOUT_MON.PDC_DTOUT_MON1.bit.PDC_DTOUT_MON_1;
	monitor_val->imgcache_data_out[2] = IO_IIP.PDC_DTOUT_MON.PDC_DTOUT_MON2.bit.PDC_DTOUT_MON_2;
	monitor_val->imgcache_ready[0] = IO_IIP.PDC_RDY_MON.PDC_RDY_MON1.bit.PDC_RDY_MON_0;
	monitor_val->imgcache_ready[1] = IO_IIP.PDC_RDY_MON.PDC_RDY_MON1.bit.PDC_RDY_MON_1;
	monitor_val->imgcache_ready[2] = IO_IIP.PDC_RDY_MON.PDC_RDY_MON2.bit.PDC_RDY_MON_2;

	Im_IIP_Off_Pclk();
	Im_IIP_Dsb();

	return D_IM_IIP_OK;
}


INT32 Im_IIP_Start_DumpUnitReg( const E_IM_IIP_UNIT_ID unit_id )
{
	Dd_ARM_Critical_Section_Start( gIM_IIP_Spin_Lock );

	Im_IIP_On_Pclk();
	im_iip_On_Iipclk();

	// IIP Force stop for Dump enabled.
	IO_IIP.IZACTL2.bit.ABORT = 0;

	IO_IIP.IZACTL1.bit.SELRDREG_UNIT = unit_id;
	IO_IIP.IZACTL1.bit.DBEN = 1;

	Dd_ARM_Critical_Section_End( gIM_IIP_Spin_Lock );

	return D_IM_IIP_OK;
}


INT32 Im_IIP_Stop_DumpUnitReg( VOID )
{
	Dd_ARM_Critical_Section_Start( gIM_IIP_Spin_Lock );

	IO_IIP.IZACTL1.bit.DBEN = 0;

	Im_IIP_Off_Pclk();
	im_iip_Off_Iipclk();

	Dd_ARM_Critical_Section_End( gIM_IIP_Spin_Lock );

	return D_IM_IIP_OK;
}


INT32 Im_IIP_Start_DumpUnitReg_ByName( const CHAR* const unit_name )
{
	UINT32 loopcnt;

	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Dump_Unit_Name) / sizeof(gIM_IIP_Dump_Unit_Name[0])); loopcnt++ ) {
		if( strcmp( unit_name, gIM_IIP_Dump_Unit_Name[loopcnt].unit_name ) == 0 ) {
			return Im_IIP_Start_DumpUnitReg( gIM_IIP_Dump_Unit_Name[loopcnt].unit_id );
		}
	}

	return D_IM_IIP_INVALID_ARG_ERR;
}


#ifdef CO_IIP_DEBUG_PRINT_FUNC
VOID Im_IIP_Print_DebugStatus( VOID )
{
	UINT32 loopcnt;

	Ddim_Print(( "gIM_IIP_SwtrgMng.unit_bitmask = 0x%llx\n", gIM_IIP_SwtrgMng.unit_bitmask ));
	Ddim_Print(( "gIM_IIP_SwtrgMng.pixfmttbl_bitmask = 0x%lx\n", gIM_IIP_SwtrgMng.pixfmttbl_bitmask ));
	Ddim_Print(( "gIM_IIP_SwtrgMng.res_bitmask = 0x%lx\n", gIM_IIP_SwtrgMng.res_bitmask ));
	for( loopcnt = 0; loopcnt < D_IM_IIP_CACHE_MAXNUM; loopcnt++ ) {
		Ddim_Print(( "gIM_IIP_SwtrgMng.cache_users[%u] = 0x%x\n", loopcnt, gIM_IIP_SwtrgMng.cache_users[loopcnt] ));
	}
	Ddim_Print(( "gIM_IIP_Callback = 0x%lx\n", (ULONG)gIM_IIP_Callback ));
	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_0) / sizeof(gIM_IIP_Inttbl_0[0])); loopcnt++ ) {
		Ddim_Print(( "gIM_IIP_Inttbl_0[%u].callback_func = 0x%lx\n", loopcnt, (ULONG)gIM_IIP_Inttbl_0[loopcnt].callback_func ));
	}
	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_1) / sizeof(gIM_IIP_Inttbl_1[0])); loopcnt++ ) {
		Ddim_Print(( "gIM_IIP_Inttbl_1[%u].callback_func = 0x%lx\n", loopcnt, (ULONG)gIM_IIP_Inttbl_1[loopcnt].callback_func ));
	}
#if 0 // INTIZ2/INTEN2 delete
	for( loopcnt = 0; loopcnt < (sizeof(gIM_IIP_Inttbl_2) / sizeof(gIM_IIP_Inttbl_2[0])); loopcnt++ ) {
		Ddim_Print(( "gIM_IIP_Inttbl_2[%u].callback_func = 0x%lx\n", loopcnt, (ULONG)gIM_IIP_Inttbl_2[loopcnt].callback_func ));
	}
#endif // INTIZ2/INTEN2 delete
}

VOID Im_IIP_Print_ClockStatus( VOID )
{
	Ddim_Print(( "CLKSTOP: AP=%u\n", Dd_Top_Get_CLKSTOP_IIPAP() ));
	Ddim_Print(( "CLKSTOP: AH=%u\n", Dd_Top_Get_CLKSTOP_IIPAH() ));
	Ddim_Print(( "CLKSTOP: AX=%u\n", Dd_Top_Get_CLKSTOP_IIPAX() ));
	Ddim_Print(( "CLKSTOP: CK=%u\n", Dd_Top_Get_CLKSTOP_IIPCK() ));
#ifdef CO_ACT_IIP_PCLK
	Ddim_Print(( "CLKCNT: AP=%u\n", gIM_IIP_PCLK_Counter ));
#endif
#ifdef CO_ACT_IIP_HCLK
	Ddim_Print(( "CLKCNT: AH=%u\n", gIM_IIP_HCLK_Counter ));
#endif
#ifdef CO_ACT_IIP_ICLK
	Ddim_Print(( "CLKCNT: AX=%u\n", gIM_IIP_ICLK_Counter ));
#endif
#ifdef CO_ACT_IIPCLK
	Ddim_Print(( "CLKCNT: CK=%u\n", gIM_IIP_IIPCLK_Counter ));
#endif
}
#else
VOID Im_IIP_Print_DebugStatus( VOID )
{
}

VOID Im_IIP_Print_ClockStatus( VOID )
{
}
#endif









//-----------------------------------------------------------------------
#ifdef CO_IM_IIP_ENABLE_UTILITY

#include "im_iip_param.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define im_iip_roundup_8(val)			(((UINT32)(val) + 7) & 0xFFFFFFF8)
#define im_iip_get_fixed_point(val)		( (INT32)((DOUBLE)(val) * D_IM_IIP_PARAM_AFN_VAL_1_0) )
#define im_iip_util_conv_unitid_to_pldunit(unit_id)		(1ULL << (unit_id))

#define	D_IM_IIP_UTIL_ORIGIN					(0x00)
#define	D_IM_IIP_UTIL_TOPRIGHT					(0x01)
#define	D_IM_IIP_UTIL_BOTTOMLEFT				(0x02)
#define	D_IM_IIP_UTIL_BOTTOMRIGHT				(0x03)


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

// ALphaLane information.
typedef struct {
	E_IM_IIP_ADEPTH		alpha_depth;			// PIXFMTTBL.ADEPTH
	UINT32				alpha_subsampling;		// PIXFMTTBL.ASUBSPL
} T_IM_IIP_UTIL_ALPHA_INFO;

// PHSZ, PVSZ parameter for Loader and STS Unit.
typedef struct {
	UINT32	PHSZ0;
	UINT32	PHSZ1;
	UINT32	PVSZ0;
	UINT32	PVSZ1;
} T_IM_IIP_UTIL_PHVSZ;

// Parameter of Im_IIP_Open_SWTRG().
typedef struct {
	ULLONG						unitid_bitmask;
	UINT32						pixid_bitmask;
	UINT32						open_res_bitmask;
} T_IM_IIP_UTIL_PARAM_OPEN;

// Parameter of Im_IIP_Open_SWTRG(), Im_IIP_Set_Interrupt() and Im_IIP_Wait_End()>
typedef struct {
	UINT32						int_fact;	// Parameter "interrupt_bitmask" of Im_IIP_Set_Interrupt() and Parameter "wait_factor" of Im_IIP_Wait_End()
	INT32						wait_time;	// Parameter "wait_time" of Im_IIP_Open_SWTRG() and Parameter "wait_time" of Im_IIP_Wait_End()
} T_IM_IIP_UTIL_PARAM_WAIT;

// Internal parameter of IM_IIP_Utility_ResizeRotate()
typedef struct {
	T_IM_IIP_UTIL_PARAM_OPEN	open_param;
	T_IM_IIP_UTIL_PARAM_WAIT	wait_param;
	ULONG						param_buffer_addr;
	UCHAR						param_master_IF;
	T_IM_IIP_PIXFMTTBL			pixfmttbl[2];
	UINT32						pixid[2];
	T_IM_IIP_UNIT_CFG			unit_cfg_afn;
	T_IM_IIP_UNIT_CFG			unit_cfg_sl;
	E_IM_IIP_AXI_ID				axi_id_afn;
	E_IM_IIP_AXI_ID				axi_id_sl;
	T_IM_IIP_CTRL_AXI			axi_cfg_afn;
	T_IM_IIP_CTRL_AXI			axi_cfg_sl;

	T_IM_IIP_PARAM_STS*			p_param_sl;
	T_IM_IIP_PARAM_AFN*			p_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	p_afn_struct_param;
} T_IM_IIP_UTIL_PARAM_RESIZE_ROTATE;

// Internal parameter of IM_IIP_Utility_CSC()
typedef struct {
	T_IM_IIP_UTIL_PARAM_OPEN	open_param;
	T_IM_IIP_UTIL_PARAM_WAIT	wait_param;
	ULONG						param_buffer_addr;
	UCHAR						param_master_IF;
	T_IM_IIP_PIXFMTTBL			pixfmttbl[2];
	UINT32						pixid[2];
	T_IM_IIP_UNIT_CFG			unit_cfg_1d;
	T_IM_IIP_UNIT_CFG			unit_cfg_csc;
	T_IM_IIP_UNIT_CFG			unit_cfg_sl;
	E_IM_IIP_AXI_ID				axi_id_ld;
	E_IM_IIP_AXI_ID				axi_id_sl;
	T_IM_IIP_CTRL_AXI			axi_cfg_1d;
	T_IM_IIP_CTRL_AXI			axi_cfg_sl;

	T_IM_IIP_PARAM_1DL*			p_param_1d;
	T_IM_IIP_PARAM_CSC*			p_param_csc;
	T_IM_IIP_PARAM_STS*			p_param_sl;
} T_IM_IIP_UTIL_PARAM_CSC;

// Internal parameter of IM_IIP_Utility_LUT()
typedef struct {
	T_IM_IIP_UTIL_PARAM_OPEN	open_param;
	T_IM_IIP_UTIL_PARAM_WAIT	wait_param;
	ULONG						param_buffer_addr;
	UCHAR						param_master_IF;
	T_IM_IIP_PIXFMTTBL			pixfmttbl[2];
	UINT32						pixid[2];
	T_IM_IIP_UNIT_CFG			unit_cfg_1d;
	T_IM_IIP_UNIT_CFG			unit_cfg_lut;
	T_IM_IIP_UNIT_CFG			unit_cfg_sl;
	E_IM_IIP_AXI_ID				axi_id_ld;
	E_IM_IIP_AXI_ID				axi_id_sl;
	T_IM_IIP_CTRL_AXI			axi_cfg_1d;
	T_IM_IIP_CTRL_AXI			axi_cfg_sl;

	T_IM_IIP_PARAM_1DL*			p_param_1d;
	T_IM_IIP_PARAM_LUT*			p_param_lut;
	T_IM_IIP_PARAM_STS*			p_param_sl;
} T_IM_IIP_UTIL_PARAM_LUT;

// Internal parameter of IM_IIP_Utility_AlphaBlend()
typedef struct {
	T_IM_IIP_UTIL_PARAM_OPEN	open_param;
	T_IM_IIP_UTIL_PARAM_WAIT	wait_param;
	ULONG						param_buffer_addr;
	UCHAR						param_master_IF;
	T_IM_IIP_PIXFMTTBL			pixfmttbl[3];
	UINT32						pixid[3];
	T_IM_IIP_UNIT_CFG			unit_cfg_1d[2];
	T_IM_IIP_UNIT_CFG			unit_cfg_alp;
	T_IM_IIP_UNIT_CFG			unit_cfg_sl;
	E_IM_IIP_AXI_ID				axi_id_ld[2];
	E_IM_IIP_AXI_ID				axi_id_sl;
	T_IM_IIP_CTRL_AXI			axi_cfg_1d[2];
	T_IM_IIP_CTRL_AXI			axi_cfg_sl;

	T_IM_IIP_PARAM_1DL*			p_param_1d[2];
	T_IM_IIP_PARAM_BLEND*		p_param_alp;
	T_IM_IIP_PARAM_STS*			p_param_sl;
} T_IM_IIP_UTIL_PARAM_ALPHA;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

// CPU Data Cache cleaner wrapper
static VOID im_iip_util_clean_dcache( const ULONG addr, const ULONG bytes )
{
	DDIM_User_L1l2cache_Clean_Addr( addr, bytes );
	Im_IIP_Dmb();
}

/**
@brief		Check master I/F
@param[in]	master_IF			The value of master I/F parameter
@retval		D_IM_IIP_TRUE		master_IF is valid.
@retval		D_IM_IIP_FALSE		master_IF is invalid.
*/
#ifdef CO_PARAM_CHECK
static INT32 im_iip_util_check_master_IF( const INT32 master_IF )
{
	switch( master_IF ) {
		case D_IM_IIP_MASTER_IF0:	// FALLTHROUGH
		case D_IM_IIP_MASTER_IF1:	// FALLTHROUGH
#if D_IM_IIP_MASTER_IF1 != D_IM_IIP_MASTER_IF2
		case D_IM_IIP_MASTER_IF2:
#endif
			return D_IM_IIP_TRUE;
		default:
			return D_IM_IIP_FALSE;
	}
}
#endif

// Convert E_IM_IIP_PIXID(bitmask) to PIXID
static ULONG im_iip_util_conv_pixid( const E_IM_IIP_PIXID pixid )
{
	switch( pixid ) {
		case E_IM_IIP_PIXID_0:
			return 0UL;
		case E_IM_IIP_PIXID_1:
			return 1UL;
		case E_IM_IIP_PIXID_2:
			return 2UL;
		case E_IM_IIP_PIXID_3:
			return 3UL;
		case E_IM_IIP_PIXID_4:
			return 4UL;
		case E_IM_IIP_PIXID_5:
			return 5UL;
		case E_IM_IIP_PIXID_6:
			return 6UL;
		case E_IM_IIP_PIXID_7:
			return 7UL;
		case E_IM_IIP_PIXID_8:
			return 8UL;
		case E_IM_IIP_PIXID_9:
			return 9UL;
		case E_IM_IIP_PIXID_10:
			return 10UL;
		case E_IM_IIP_PIXID_11:
			return 11UL;
		default:
			return (ULONG)E_IM_IIP_PIXID_INVALID;
	}
}

// Convert UnitID to PortID
static ULONG im_iip_util_conv_portid( const E_IM_IIP_UNIT_ID unitid )
{
	switch( unitid ) {
		case E_IM_IIP_UNIT_ID_LD0:
			return E_IM_IIP_PARAM_PORTID_LD0;
		case E_IM_IIP_UNIT_ID_LD1:
			return E_IM_IIP_PARAM_PORTID_LD1;
		case E_IM_IIP_UNIT_ID_LD2:
			return E_IM_IIP_PARAM_PORTID_LD2;
		case E_IM_IIP_UNIT_ID_LD3:
			return E_IM_IIP_PARAM_PORTID_LD3;
		case E_IM_IIP_UNIT_ID_FLT:
			return E_IM_IIP_PARAM_PORTID_FLT_P0;	// Only port0
		case E_IM_IIP_UNIT_ID_AFN0:
			return E_IM_IIP_PARAM_PORTID_AFN0;
		case E_IM_IIP_UNIT_ID_AFN1:
			return E_IM_IIP_PARAM_PORTID_AFN1;
		case E_IM_IIP_UNIT_ID_AFN2:
			return E_IM_IIP_PARAM_PORTID_AFN2;
		case E_IM_IIP_UNIT_ID_FRECT0:
			return E_IM_IIP_PARAM_PORTID_FRECT0;
		case E_IM_IIP_UNIT_ID_MON:
			return E_IM_IIP_PARAM_PORTID_NONE;		// Port not found
		case E_IM_IIP_UNIT_ID_SL0:
			return E_IM_IIP_PARAM_PORTID_SL0;
		case E_IM_IIP_UNIT_ID_SL1:
			return E_IM_IIP_PARAM_PORTID_SL1;
		case E_IM_IIP_UNIT_ID_SL2:
			return E_IM_IIP_PARAM_PORTID_SL2;
		case E_IM_IIP_UNIT_ID_SL3:
			return E_IM_IIP_PARAM_PORTID_SL3;
		case E_IM_IIP_UNIT_ID_SL4:
			return E_IM_IIP_PARAM_PORTID_SL4;
		case E_IM_IIP_UNIT_ID_SL5:
			return E_IM_IIP_PARAM_PORTID_SL5;
		case E_IM_IIP_UNIT_ID_SL6:
			return E_IM_IIP_PARAM_PORTID_SL6;
		case E_IM_IIP_UNIT_ID_FRECT1:
			return E_IM_IIP_PARAM_PORTID_FRECT1;
		case E_IM_IIP_UNIT_ID_SL7:
			return E_IM_IIP_PARAM_PORTID_SL7;
		case E_IM_IIP_UNIT_ID_SL8:
			return E_IM_IIP_PARAM_PORTID_SL8;
		case E_IM_IIP_UNIT_ID_CSC0:
			return E_IM_IIP_PARAM_PORTID_CSC0;
		case E_IM_IIP_UNIT_ID_CSC1:
			return E_IM_IIP_PARAM_PORTID_CSC1;
		case E_IM_IIP_UNIT_ID_LUT:
			return E_IM_IIP_PARAM_PORTID_LUT_P0;	// Only port0
		case E_IM_IIP_UNIT_ID_GPC:
			return E_IM_IIP_PARAM_PORTID_GPC_P0;	// Only port0
		case E_IM_IIP_UNIT_ID_BLEND0:
			return E_IM_IIP_PARAM_PORTID_BLEND0_P0;	// Only port0
		case E_IM_IIP_UNIT_ID_CFL0:
			return E_IM_IIP_PARAM_PORTID_CFL0_P0;	// Only port0
		case E_IM_IIP_UNIT_ID_CSC2:
			return E_IM_IIP_PARAM_PORTID_CSC2;
		case E_IM_IIP_UNIT_ID_CSC3:
			return E_IM_IIP_PARAM_PORTID_CSC3;
		case E_IM_IIP_UNIT_ID_LD4:
			return E_IM_IIP_PARAM_PORTID_LD4;
		case E_IM_IIP_UNIT_ID_LD5:
			return E_IM_IIP_PARAM_PORTID_LD5;
		case E_IM_IIP_UNIT_ID_LD6:
			return E_IM_IIP_PARAM_PORTID_LD6;
		case E_IM_IIP_UNIT_ID_LD7:
			return E_IM_IIP_PARAM_PORTID_LD7;
		case E_IM_IIP_UNIT_ID_AFN3:
			return E_IM_IIP_PARAM_PORTID_AFN3;
		case E_IM_IIP_UNIT_ID_BLEND1:
			return E_IM_IIP_PARAM_PORTID_BLEND1_P0;	// Only port0
		case E_IM_IIP_UNIT_ID_MFT:
			return E_IM_IIP_PARAM_PORTID_MFT;
		default:
			return E_IM_IIP_PARAM_PORTID_NONE;	// Invalid parameter
	}
}

static ULONG im_iip_util_conv_portid_1( const E_IM_IIP_UNIT_ID unitid )
{
	switch( unitid ) {
		case E_IM_IIP_UNIT_ID_FLT:
			return E_IM_IIP_PARAM_PORTID_FLT_P1;
		case E_IM_IIP_UNIT_ID_LUT:
			return E_IM_IIP_PARAM_PORTID_LUT_P1;
		case E_IM_IIP_UNIT_ID_GPC:
			return E_IM_IIP_PARAM_PORTID_GPC_P1;
		case E_IM_IIP_UNIT_ID_BLEND0:
			return E_IM_IIP_PARAM_PORTID_BLEND0_P1;
		case E_IM_IIP_UNIT_ID_CFL0:
			return E_IM_IIP_PARAM_PORTID_CFL0_P1;
		case E_IM_IIP_UNIT_ID_BLEND1:
			return E_IM_IIP_PARAM_PORTID_BLEND1_P1;
		default:
			return E_IM_IIP_PARAM_PORTID_NONE;	// Other unit
	}
}

/**
@brief		Set UNITINFTBL parameter
@param[out]	unitinftbl_param	Pointer of UNITINFTBL parameter
@param[in]	id					UNIT ID
@param[in]	addr				Beginning address of unit parameter
*/
static VOID im_iip_set_unitinftbl_param( T_IM_IIP_UNIT_CFG* const unitinftbl_param, const E_IM_IIP_UNIT_ID id, const VOID* const addr, const ULLONG unitid_bitmask )
{
	switch( id ) {
		case E_IM_IIP_UNIT_ID_SL0:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL1:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL2:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL3:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL4:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL5:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL6:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL7:	// FALLTHROUGH
		case E_IM_IIP_UNIT_ID_SL8:
			unitinftbl_param->unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
			unitinftbl_param->chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
			unitinftbl_param->unit_param_addr = (ULONG)addr;
			unitinftbl_param->load_unit_param_flag = unitid_bitmask;
			break;
		default:
			unitinftbl_param->unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
			unitinftbl_param->unit_param_addr = (ULONG)addr;
			break;
	}
}

/**
@brief		Set PIXFMTTBL parameter
@param[in]	pixid				PIXID
@param[in]	gbl					Info. of global image
@param[in]	pix_depth			Depth of pixel
@param[in]	alpha				Pointer of alpha_depth and alpha_subsampling
*/
static VOID im_iip_set_pixfmttbl_param( T_IM_IIP_PIXFMTTBL* const pixfmttbl,
									    const T_IM_IIP_UTIL_IMG_GBL* const gbl,
									    const UINT32 pix_depth,
									    const T_IM_IIP_UTIL_ALPHA_INFO* const alpha )
{
	pixfmttbl->pix_depth = pix_depth;
	if( (gbl->alpha == D_IM_IIP_ALPHA_FALSE) || (alpha == NULL) ) {
		pixfmttbl->alpha = D_IM_IIP_ALPHA_FALSE;
		pixfmttbl->alpha_depth = E_IM_IIP_ADEPTH_8BITS;
		pixfmttbl->alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP;
	}
	else {
		pixfmttbl->alpha = D_IM_IIP_ALPHA_TRUE;
		pixfmttbl->alpha_depth = alpha->alpha_depth;
		pixfmttbl->alpha_subsampling = alpha->alpha_subsampling;

		pixfmttbl->line_bytes.Alpha = gbl->line_bytes.Alpha;
		pixfmttbl->addr.Alpha = gbl->addr.Alpha;
		pixfmttbl->masterIF_Alpha = gbl->masterIF_Alpha;
	}
	pixfmttbl->frame_type = gbl->frame_type;
	pixfmttbl->pix_format = gbl->pix_format;
	pixfmttbl->chunky_color.component0 = gbl->chunky_color.component0;
	pixfmttbl->chunky_color.component1 = gbl->chunky_color.component1;
	pixfmttbl->chunky_color.component2 = gbl->chunky_color.component2;
	pixfmttbl->chunky_color.component3 = gbl->chunky_color.component3;
	pixfmttbl->sign_Y_G = gbl->sign_Y_G;
	pixfmttbl->sign_Cb_B = gbl->sign_Cb_B;
	pixfmttbl->sign_Cr_R = gbl->sign_Cr_R;
	pixfmttbl->sign_D3 = gbl->sign_D3;
	pixfmttbl->masterIF_Y_G = gbl->masterIF_Y_G;
	pixfmttbl->masterIF_Cb_B = gbl->masterIF_Cb_B;
	pixfmttbl->masterIF_Cr_R = gbl->masterIF_Cr_R;
	pixfmttbl->pix_outside_img = D_IM_IIP_OUTSIDE_IMG_EDGE;
//	pixfmttbl->Y_G_val_outside_img;		// no use
//	pixfmttbl->Cb_B_val_outside_img;	// no use
//	pixfmttbl->Cr_R_val_outside_img;	// no use
//	pixfmttbl->Alpha_val_outside_img;	// no use
	pixfmttbl->width = gbl->width;
	pixfmttbl->lines = gbl->lines;
	pixfmttbl->line_bytes.Y_G = gbl->line_bytes.Y_G;
	pixfmttbl->line_bytes.Cb_B = gbl->line_bytes.Cb_B;
	pixfmttbl->line_bytes.Cr_R = gbl->line_bytes.Cr_R;
	pixfmttbl->addr.Y_G = gbl->addr.Y_G;
	pixfmttbl->addr.Cb_B = gbl->addr.Cb_B;
	pixfmttbl->addr.Cr_R = gbl->addr.Cr_R;
}

/**
@brief		Set arugument of Im_IIP_Ctrl_AXI
@param[in]	master_IF		    Master I/F for SDRAM Parameter
*/
static VOID im_iip_set_axi_param( const E_IM_IIP_UNIT_ID unitid, E_IM_IIP_AXI_ID* const axi_id, T_IM_IIP_CTRL_AXI* const axi_cfg, const UCHAR master_IF )
{
	switch( unitid ) {
		case E_IM_IIP_UNIT_ID_AFN0:
			*axi_id = E_IM_IIP_AXI_ID_AFN0;
			break;
		case E_IM_IIP_UNIT_ID_AFN1:
			*axi_id = E_IM_IIP_AXI_ID_AFN1;
			break;
		case E_IM_IIP_UNIT_ID_AFN2:
			*axi_id = E_IM_IIP_AXI_ID_AFN2;
			break;
		case E_IM_IIP_UNIT_ID_AFN3:
			*axi_id = E_IM_IIP_AXI_ID_AFN3;
			break;
		case E_IM_IIP_UNIT_ID_FRECT0:
			*axi_id = E_IM_IIP_AXI_ID_FRECT0;
			break;
		case E_IM_IIP_UNIT_ID_FRECT1:
			*axi_id = E_IM_IIP_AXI_ID_FRECT1;
			break;
		case E_IM_IIP_UNIT_ID_GPC:
			*axi_id = E_IM_IIP_AXI_ID_GPC;
			break;
		case E_IM_IIP_UNIT_ID_SL0:
			*axi_id = E_IM_IIP_AXI_ID_SL0;
			break;
		case E_IM_IIP_UNIT_ID_SL1:
			*axi_id = E_IM_IIP_AXI_ID_SL1;
			break;
		case E_IM_IIP_UNIT_ID_SL2:
			*axi_id = E_IM_IIP_AXI_ID_SL2;
			break;
		case E_IM_IIP_UNIT_ID_SL3:
			*axi_id = E_IM_IIP_AXI_ID_SL3;
			break;
		case E_IM_IIP_UNIT_ID_SL4:
			*axi_id = E_IM_IIP_AXI_ID_SL4;
			break;
		case E_IM_IIP_UNIT_ID_SL5:
			*axi_id = E_IM_IIP_AXI_ID_SL5;
			break;
		case E_IM_IIP_UNIT_ID_SL6:
			*axi_id = E_IM_IIP_AXI_ID_SL6;
			break;
		case E_IM_IIP_UNIT_ID_SL7:
			*axi_id = E_IM_IIP_AXI_ID_SL7;
			break;
		case E_IM_IIP_UNIT_ID_SL8:
			*axi_id = E_IM_IIP_AXI_ID_SL8;
			break;
		default:
			*axi_id = E_IM_IIP_AXI_ID_MAX;		// mean "None"
			return;	// Do nothing
	}

	axi_cfg->trans_req_ctrl = 0;	// register default
	axi_cfg->cache_type = 1;		// register default
	axi_cfg->protection_type = 0;	// register default
}

// Enable Interrupt
static UINT32 im_iip_util_set_int_fact( const ULLONG unitid_bitmask )
{
	UINT32 int_fact = D_IM_IIP_INT_FACTOR_AXIERR;

	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL0 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL0END;
	}
	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL1 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL1END;
	}
	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL2 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL2END;
	}
	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL3 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL3END;
	}
	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL4 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL4END;
	}
	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL5 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL5END;
	}
	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL6 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL6END;
	}
	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL7 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL7END;
	}
	if( unitid_bitmask & D_IM_IIP_PARAM_PLDUNIT_SL8 ) {
		int_fact = D_IM_IIP_INT_FACTOR_SL8END;
	}

	return int_fact;
}

// Wait Unit's finish and close Unit
static INT32 im_iip_util_wait_and_close( T_IM_IIP_UTIL_PARAM_OPEN* const open_param, T_IM_IIP_UTIL_PARAM_WAIT* const wait_param, const E_IM_IIP_UNIT_ID cubeic_free_unitid )
{
	UINT32	wait_factor;
	INT32	retval;
	INT32	ercd;

	retval = Im_IIP_Wait_End( &wait_factor,
							  wait_param->int_fact,
							  D_IM_IIP_OR_WAIT,
							  wait_param->wait_time );
	if( retval == D_IM_IIP_OK ) {
		if( wait_factor & D_IM_IIP_INT_FACTOR_AXIERR ) {
			retval = D_IM_IIP_ERR;
		}
	}


	ercd = Im_IIP_Close_SWTRG( open_param->unitid_bitmask, open_param->pixid_bitmask, open_param->open_res_bitmask );
	if( retval == D_IM_IIP_OK ) {
		return ercd;
	}

	return retval;
}

/**
@brief		Calc max/min value
@param[out]	max_val					Max value
@param[out]	min_val					Min value
@param[in]	pix_depth				Pixel depth
@param[in]	sign					Sign info.
*/
static VOID im_iip_get_max_min_value( UINT32* const max_val,
									  UINT32* const min_val,
									  const UINT32 pix_depth,
									  const UCHAR sign )
{
	INT32 p;

	switch ( pix_depth ) {
		case D_IM_IIP_PDEPTH_8BITS:
			p = 8;
			break;
		case D_IM_IIP_PDEPTH_12BITS:
			p = 12;
			break;
//		case D_IM_IIP_PDEPTH_16BITS:
		default:
			p = 14;
			break;
	}

	if( D_IM_IIP_SIGNED_DATA == sign ) {
		*min_val = (UINT32)(-(1 << (p - 1)));
		*max_val = (UINT32)((1 << (p - 1)) - 1);
	}
	else {
		*min_val = 0x0000;
		*max_val = (UINT32)((1 << p) - 1);
	}
}

/**
@brief		Set AFN struct parameter for mirror/reverse.
@param[in]	afn_struct_param	Pointer of AFN struct paramter
@param[in]	src					Source image info.
@param[in]	rotation			Angle of rotation
@param[in]	flip				flip information
*/
static void im_iip_set_afn_struct_flip( T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_struct_param,
										 const T_IM_IIP_UTIL_IMG* const src,
										 const E_IM_IIP_UTIL_ROTATE rotation,
										 const E_IM_IIP_UTIL_FLIP flip)
{
	U_IM_IIP_PARAM_AFN_STAXY	set_sta;
	INT32						wok;
	static const int sta_pos_tbl[E_IM_IIP_UTIL_ROTATE_MAX][E_IM_IIP_UTIL_FLIP_MAX] = {
		// off							mirror						reverse						mirror_reverse
		{ D_IM_IIP_UTIL_ORIGIN,			D_IM_IIP_UTIL_TOPRIGHT,		D_IM_IIP_UTIL_BOTTOMLEFT,	D_IM_IIP_UTIL_BOTTOMRIGHT	},	// 0
		{ D_IM_IIP_UTIL_BOTTOMLEFT,		D_IM_IIP_UTIL_ORIGIN,		D_IM_IIP_UTIL_BOTTOMRIGHT,	D_IM_IIP_UTIL_TOPRIGHT		},	// 90
		{ D_IM_IIP_UTIL_BOTTOMRIGHT,	D_IM_IIP_UTIL_BOTTOMLEFT,	D_IM_IIP_UTIL_TOPRIGHT,		D_IM_IIP_UTIL_ORIGIN		},	// 180
		{ D_IM_IIP_UTIL_TOPRIGHT,		D_IM_IIP_UTIL_BOTTOMRIGHT,	D_IM_IIP_UTIL_ORIGIN,		D_IM_IIP_UTIL_BOTTOMLEFT	},	// 270
	};

	switch (sta_pos_tbl[rotation][flip])
	{
	case D_IM_IIP_UTIL_TOPRIGHT:
		set_sta.bit.STAX = im_iip_get_fixed_point(src->rect.left + src->rect.width - 1.0);
		set_sta.bit.STAY = im_iip_get_fixed_point(src->rect.top);
		break;
	case D_IM_IIP_UTIL_BOTTOMLEFT:
		set_sta.bit.STAX = im_iip_get_fixed_point(src->rect.left);
		set_sta.bit.STAY = im_iip_get_fixed_point(src->rect.top + src->rect.lines - 1.0);
		break;
	case D_IM_IIP_UTIL_BOTTOMRIGHT:
		set_sta.bit.STAX = im_iip_get_fixed_point(src->rect.left + src->rect.width - 1.0);
		set_sta.bit.STAY = im_iip_get_fixed_point(src->rect.top  + src->rect.lines - 1.0);
		break;
	case D_IM_IIP_UTIL_ORIGIN:
		set_sta.bit.STAX = im_iip_get_fixed_point(src->rect.left);
		set_sta.bit.STAY = im_iip_get_fixed_point(src->rect.top);
		break;
	default:
		// Don't set paramter about Mirror/Reverse in cases of error.
		return;
	}

	if (flip == E_IM_IIP_UTIL_FLIP_REVERSE)
	{
		afn_struct_param->STAXY0.bit.STAX = set_sta.bit.STAX;
		afn_struct_param->STAXY0.bit.STAY = set_sta.bit.STAY;

		// make y-component negative to flip vertical
		// DYX = -DYX
		im_iip_get_reg_signed_a( wok, afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX, -wok );

		// DYY = -DYY
		im_iip_get_reg_signed_a( wok, afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYY );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYY, -wok );
	}
	else if (flip == E_IM_IIP_UTIL_FLIP_MIRROR)
	{
		afn_struct_param->STAXY0.bit.STAX = set_sta.bit.STAX;
		afn_struct_param->STAXY0.bit.STAY = set_sta.bit.STAY;

		// make x-component negative to flip horizontal
		// DXY = -DXY
		im_iip_get_reg_signed_a( wok, afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXY );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX, -wok );
		afn_struct_param->D_OUT_IN_0.bit.DXY = -(afn_struct_param->D_OUT_IN_0.bit.DXY);

		// DXX = -DXX
		im_iip_get_reg_signed_a( wok, afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXX );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXX, -wok );
	}
	else if (flip == E_IM_IIP_UTIL_FLIP_MIR_AND_REV)
	{
		afn_struct_param->STAXY0.bit.STAX = set_sta.bit.STAX;
		afn_struct_param->STAXY0.bit.STAY = set_sta.bit.STAY;

		// make x and y component negative to flip horizontal and vertical
		// DXY = -DXY
		im_iip_get_reg_signed_a( wok, afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXY );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX, -wok );
		afn_struct_param->D_OUT_IN_0.bit.DXY = -(afn_struct_param->D_OUT_IN_0.bit.DXY);

		// DXX = -DXX
		im_iip_get_reg_signed_a( wok, afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXX );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXX, -wok );

		// DYX = -DYX
		im_iip_get_reg_signed_a( wok, afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX, -wok );

		// DYY = -DYY
		im_iip_get_reg_signed_a( wok, afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYY );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYY, -wok );
	}
	else
	{
		afn_struct_param->STAXY0.bit.STAX = set_sta.bit.STAX;
		afn_struct_param->STAXY0.bit.STAY = set_sta.bit.STAY;
	}
}

/**
@brief		Set AFN struct parameter
@param[in]	afn_struct_param	Pointer of AFN struct paramter
@param[in]	src					Source image info.
@param[in]	dst					Destination image info.
@param[in]	rotation			Angle of rotation
*/
static VOID im_iip_set_afn_struct_param( T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_struct_param,
										 const T_IM_IIP_UTIL_IMG* const src,
										 const T_IM_IIP_UTIL_IMG_INT* const dst,
										 const E_IM_IIP_UTIL_ROTATE rotation,
										 const E_IM_IIP_UTIL_FLIP flip )
{
	INT32	h_pit_fixed_point;
	INT32	v_pit_fixed_point;

	memset( afn_struct_param, '\0', sizeof(*afn_struct_param) );

	if( rotation == E_IM_IIP_UTIL_ROTATE_000 ) {
		h_pit_fixed_point = (((INT32)src->rect.width - 1) * D_IM_IIP_PARAM_AFN_VAL_1_0) / (dst->rect.width - 1);
		v_pit_fixed_point = (((INT32)src->rect.lines - 1) * D_IM_IIP_PARAM_AFN_VAL_1_0) / (dst->rect.lines - 1);

		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXX, h_pit_fixed_point );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXY, 0x0 );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX, 0x0 );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYY, v_pit_fixed_point );
	}
	else if( rotation == E_IM_IIP_UTIL_ROTATE_090 ) {
		h_pit_fixed_point = (((INT32)src->rect.width - 1) * D_IM_IIP_PARAM_AFN_VAL_1_0) / (dst->rect.lines - 1);
		v_pit_fixed_point = (((INT32)src->rect.lines - 1) * D_IM_IIP_PARAM_AFN_VAL_1_0) / (dst->rect.width - 1);

		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXX, 0x0 );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXY, - v_pit_fixed_point );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX, h_pit_fixed_point );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYY, 0x0 );
	}
	else if( rotation == E_IM_IIP_UTIL_ROTATE_270 ) {
		h_pit_fixed_point = (((INT32)src->rect.width - 1) * D_IM_IIP_PARAM_AFN_VAL_1_0) / (dst->rect.lines - 1);
		v_pit_fixed_point = (((INT32)src->rect.lines - 1) * D_IM_IIP_PARAM_AFN_VAL_1_0) / (dst->rect.width - 1);

		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXX, 0x0 );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXY, v_pit_fixed_point );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX, - h_pit_fixed_point );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYY, 0x0 );
	}
	else {
		// E_IM_IIP_UTIL_ROTATE_180
		h_pit_fixed_point = (((INT32)src->rect.width - 1) * D_IM_IIP_PARAM_AFN_VAL_1_0) / (dst->rect.width - 1);
		v_pit_fixed_point = (((INT32)src->rect.lines - 1) * D_IM_IIP_PARAM_AFN_VAL_1_0) / (dst->rect.lines - 1);

		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXX, - h_pit_fixed_point );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DXY, 0x0 );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYX, 0x0 );
		im_iip_set_reg_signed_a( afn_struct_param->D_OUT_IN_0, U_IM_IIP_PARAM_AFN_D_OUT_IN, DYY, - v_pit_fixed_point );
	}

	afn_struct_param->SZ.bit.AFNDVSZ = dst->rect.lines;
	afn_struct_param->SZ.bit.AFNDHSZ = dst->rect.width;

	im_iip_set_afn_struct_flip( afn_struct_param, src, rotation, flip );
}

/**
@brief		Set AFN unit parameter
@param[out]	afn						Pointer of AFN unit paramter
@param[in]	afn_struct_param_addr	Beginning address of AFN struct paramter
@param[in]	resize_mode				resize mode (interpolation mode)
@param[in]	dst						Destination image info.
*/
static VOID im_iip_set_afn_unit_param( T_IM_IIP_PARAM_AFN* const afn,
									   const ULONG afn_struct_param_addr,
									   const T_IM_IIP_UTIL_RR* const cfg )
{
	const T_IM_IIP_UTIL_RECT_INT* dst_rect = &cfg->dst.rect;
	UINT32 max_val;
	UINT32 min_val;

	memset( afn, '\0', sizeof(*afn) );

	afn->header0.bit.BeginningAddress = 0x0000;
	afn->header0.bit.WordLength = 29;
	afn->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
	afn->AFNTOPCNF.bit.DATACONF = 0;
//	afn->AFNTOPCNF.bit.WAITCONF;	// set later
	afn->AFNLDCNF.bit.DTHD = E_IM_IIP_PARAM_DTHD_NORMAL;
	afn->AFNLDCNF.bit.CBC = E_IM_IIP_PARAM_CBC_DISABLE;
	afn->AFNCTL.bit.LPLVL = E_IM_IIP_PARAM_LPLVL_H_LEVEL;
	afn->AFNCTL.bit.HDBL = E_IM_IIP_PARAM_HDBL_SAME_AS_PICHSZ;
	afn->AFNCTL.bit.VDBL = E_IM_IIP_PARAM_VDBL_SAME_AS_PICVSZ;
	afn->AFNCTL.bit.GHSZ = E_IM_IIP_PARAM_GHSZ_32PIX;
	afn->AFNCTL.bit.GVSZ = ( dst_rect->lines >=   8 ) ? E_IM_IIP_PARAM_GVSZ_8PIX   :
		                   ( dst_rect->lines >=   4 ) ? E_IM_IIP_PARAM_GVSZ_4PIX   :
		                                                E_IM_IIP_PARAM_GVSZ_2PIX;
	afn->AFNCTL.bit.HSMD = E_IM_IIP_PARAM_HSMD_NORMAL;
	afn->PFCTL.bit.PFEXTMC = 4;
	afn->PFCTL.bit.PFEXTADC = 64;
	if( (E_IM_IIP_UTIL_ROTATE_090 == cfg->rotation) ||
		 (E_IM_IIP_UTIL_ROTATE_270 == cfg->rotation) ) {
		afn->PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_1;
		afn->PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_1;
	}
	else {
		afn->PFCTL.bit.PFSP1 = E_IM_IIP_PARAM_PFSP_1;
		afn->PFCTL.bit.PFSP2 = E_IM_IIP_PARAM_PFSP_16;
	}
	afn->PFCTL.bit.PFVSP = E_IM_IIP_PARAM_PFVSP_VERTICAL_0;
	afn->PFCTL.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN;
//	afn->PIXIDEF.bit.IPIXID;		// set later
	afn->PIXIDEF.bit.EXA = E_IM_IIP_PARAM_EXA_NO_ALPHA_TO_NEXT_UNIT;
	afn->PIXIDEF.bit.CSEL = cfg->afn_cache_select;
	afn->CALMETHOD.bit.OPCOL = 0;
	afn->CALMETHOD.bit.ITMD = cfg->resize_mode;
	afn->CALMETHOD.bit.ARA = E_IM_IIP_PARAM_ARA_ITMD;
	afn->CALMETHOD.bit.FILMD = 0;
	if( cfg->resize_mode == E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR ) {
		afn->CALMETHOD.bit.DANTI = 1;
		afn->CALMETHOD.bit.AANTI = 1;
	}
	else {
		afn->CALMETHOD.bit.DANTI = 0;
		afn->CALMETHOD.bit.AANTI = 0;
	}
	switch( cfg->afn_unitid ) {
		case E_IM_IIP_UNIT_ID_AFN0:
			afn->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN0;
			break;
		case E_IM_IIP_UNIT_ID_AFN1:
			afn->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN1;
			break;
		case E_IM_IIP_UNIT_ID_AFN2:
			afn->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN2;
			break;
		case E_IM_IIP_UNIT_ID_AFN3:
			afn->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN3;
			break;
		default:
			afn->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN0;	// Failsafe
			break;
	}
	afn->CALMETHOD.bit.SCUB = cfg->bi_cubic_unitid;
	afn->FILVAL0.bit.FILVALYG = 0x0;
	afn->FILVAL0.bit.FILVALB = 0x0;
	afn->FILVAL1.bit.FILVALR = 0x0;
	afn->FILVAL1.bit.FILVALA = 0x0;
	afn->OPMD.bit.OPMD0 = E_IM_IIP_PARAM_OPMD_ADD;
	afn->OPY.bit.OPBY = 0;
	afn->OPY.bit.OPAY = 0x10;	// 1.0
	afn->OPB.bit.OPBB = 0;
	afn->OPB.bit.OPAB = 0x10;	// 1.0
	afn->OPR.bit.OPBR = 0;
	afn->OPR.bit.OPAR = 0x10;	// 1.0
	afn->OPA.bit.OPBA = 0;
	afn->OPA.bit.OPAA = 0x10;	// 1.0
	im_iip_get_max_min_value( &max_val, &min_val, cfg->pix_depth, cfg->src.gbl.sign_Y_G );
	afn->CLIPLVLY.bit.CLIPLVLYH = max_val;
	afn->CLIPLVLY.bit.CLIPLVLYL = min_val;
	im_iip_get_max_min_value( &max_val, &min_val, cfg->pix_depth, cfg->src.gbl.sign_Cb_B );
	afn->CLIPLVLB.bit.CLIPLVLBH = max_val;
	afn->CLIPLVLB.bit.CLIPLVLBL = min_val;
	im_iip_get_max_min_value( &max_val, &min_val, cfg->pix_depth, cfg->src.gbl.sign_Cr_R );
	afn->CLIPLVLR.bit.CLIPLVLRH = max_val;
	afn->CLIPLVLR.bit.CLIPLVLRL = min_val;
	im_iip_get_max_min_value( &max_val, &min_val, cfg->pix_depth, cfg->src.gbl.sign_D3 );
	afn->CLIPLVLA.bit.CLIPLVLAH = max_val;
	afn->CLIPLVLA.bit.CLIPLVLAL = min_val;
	afn->AFNPCNT.bit.AFNPHCNT = 1;
	afn->AFNPCNT.bit.AFNPVCNT = 1;
	afn->AFNPA.word = afn_struct_param_addr;
	afn->OUTSIZE.bit.OUTHSZ = 0;
	afn->OUTSIZE.bit.OUTVSZ = 0;
	afn->OUTSTARTPOS.bit.STARTX = 0;
	afn->OUTSTARTPOS.bit.STARTY = 0;
	afn->UPLK_TARGET.bit.UPSU0 = E_IM_IIP_PARAM_UPSU_NONE;
	afn->UPLK_TARGET.bit.UPSU1 = E_IM_IIP_PARAM_UPSU_NONE;
	afn->UPLK_TARGET.bit.UPSU2 = 0;
	afn->DWLK_TARGET.bit.DWSU0 = E_IM_IIP_PARAM_DWSU_NONE;
	afn->DWLK_TARGET.bit.DWSU1 = E_IM_IIP_PARAM_DWSU_NONE;
	afn->DWLK_TARGET.bit.DWSU2 = 0;
	afn->LKNUM.bit.UPNUM = E_IM_IIP_PARAM_UPNUM_NONE;
	afn->LKNUM.bit.DWNUM = E_IM_IIP_PARAM_DWNUM_NONE;
	afn->RING.bit.URINGSIZE = 0;
	afn->RING.bit.UMARGINIYSZ = 0;
	afn->RING.bit.DRINGSIZE = 0;
}

static VOID im_iip_util_set_afn_raw_input_unit( T_IM_IIP_PARAM_AFN* const afn, T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_struct_param, T_IM_IIP_PARAM_STS* const sl )
{
	INT32 wok;

	im_iip_get_reg_signed_a( wok, afn_struct_param->STAXY0, U_IM_IIP_PARAM_AFN_STAXY, STAX );
	wok /= 2;
	im_iip_set_reg_signed_a( afn_struct_param->STAXY0, U_IM_IIP_PARAM_AFN_STAXY, STAX, wok );
	im_iip_get_reg_signed_a( wok, afn_struct_param->STAXY0, U_IM_IIP_PARAM_AFN_STAXY, STAY );
	wok /= 2;
	im_iip_set_reg_signed_a( afn_struct_param->STAXY0, U_IM_IIP_PARAM_AFN_STAXY, STAY, wok );

	afn->OUTSIZE.bit.OUTHSZ /= 2;
	afn->OUTSIZE.bit.OUTVSZ /= 2;
	afn->OUTSTARTPOS.bit.STARTX = 0 /2;
	afn->OUTSTARTPOS.bit.STARTY = 0 /2;
	afn->CALMETHOD.bit.FILMD = 1;
	afn->CALMETHOD.bit.DANTI = 0;
	afn->CALMETHOD.bit.AANTI = 0;

	sl->BASE.SL_PHSZ.bit.PHSZ0 *= 2;
	sl->BASE.SL_PHSZ.bit.PHSZ1 *= 2;
	sl->BASE.SL_PVSZ.bit.PVSZ0 *= 2;
	sl->BASE.SL_PVSZ.bit.PVSZ1 *= 2;
}

static VOID im_iip_util_set_afn_raw_input_pixfmttbl( T_IM_IIP_PIXFMTTBL* const in_pixfmttbl )
{
	in_pixfmttbl->line_bytes.Y_G *= 2;
	in_pixfmttbl->line_bytes.Cb_B *= 2;
	in_pixfmttbl->line_bytes.Cr_R *= 2;
	in_pixfmttbl->width *= 2;
	in_pixfmttbl->lines *= 2;
}

/**
@brief		Get PHSZ0, PHSZ1, PVSZ0 and PVSZ1
@param[out]	phvsz			Pointer of PHSZ0, PHSZ1, PVSZ0 and PVSZ1
@param[in]	dst				Info. of destination image
*/
static VOID im_iip_set_phvsz_1d_sl( T_IM_IIP_UTIL_PHVSZ* const phvsz,
								    const T_IM_IIP_UTIL_IMG_INT* const dst )
{
	UINT32						mod_sz;
	const T_IM_IIP_UTIL_RECT_INT* dst_rect = &dst->rect;

	phvsz->PVSZ1 = dst_rect->lines;
	phvsz->PHSZ1 = ( dst_rect->width >= 64 ) ? 64 : 32;

	mod_sz = dst_rect->left % 8;
	phvsz->PHSZ0 = (mod_sz == 0) ? phvsz->PHSZ1 : (8 - mod_sz);
	mod_sz = dst_rect->top % 8;
	phvsz->PVSZ0 = (mod_sz == 0) ? phvsz->PVSZ1 : (8 - mod_sz);
}

/**
@brief		Get the setting value of PF_PDIST
@param[in]	pix_depth		Pixel depth
@param[in]	frame_type		Frame type
@param[in]	pix_format		Pixel format
@param[in]	alpha			Alpha ON/OFF info.
*/
static E_IM_IIP_PARAM_PF_PDIST im_iip_get_PF_PDIST( const UINT32 pix_depth,
												    const E_IM_IIP_FTYPE frame_type,
												    const E_IM_IIP_PFMT pix_format,
												    const UINT32 alpha )
{
	if(
		(pix_format == E_IM_IIP_PFMT_RGB565) ||
		(pix_format == E_IM_IIP_PFMT_RGBA5551) ||
		(pix_format == E_IM_IIP_PFMT_RGBA4444)
	   ) {
		return E_IM_IIP_PARAM_PF_PDIST_32PIX;
	}


	if( D_IM_IIP_PDEPTH_8BITS == pix_depth ) {

		if( pix_format == E_IM_IIP_PFMT_CBCR ) {
			return E_IM_IIP_PARAM_PF_PDIST_32PIX;
		}

		if(
			((E_IM_IIP_PFMT_BAYER == pix_format) && (D_IM_IIP_ALPHA_TRUE == alpha)) ||
			((E_IM_IIP_PFMT_PLANE == pix_format) && (D_IM_IIP_ALPHA_TRUE == alpha))
			) {
			return E_IM_IIP_PARAM_PF_PDIST_32PIX;
		}

		if(
			(E_IM_IIP_PFMT_BAYER == pix_format) ||
			(E_IM_IIP_PFMT_PLANE == pix_format)
		   ) {
			return E_IM_IIP_PARAM_PF_PDIST_64PIX;
		}

		if( E_IM_IIP_PFMT_VIDEO == pix_format ) {
			return E_IM_IIP_PARAM_PF_PDIST_64PIX;
		}

		if( E_IM_IIP_FTYPE_CHUNKY == frame_type ) {
			if( E_IM_IIP_PFMT_YCC444 == pix_format ) {
				return E_IM_IIP_PARAM_PF_PDIST_16PIX;
			}
			else {
				return E_IM_IIP_PARAM_PF_PDIST_32PIX;
			}
		}
		else {
			return E_IM_IIP_PARAM_PF_PDIST_64PIX;
		}

	}
	else {

		if( pix_format == E_IM_IIP_PFMT_CBCR ) {
			return E_IM_IIP_PARAM_PF_PDIST_16PIX;
		}

		if(
			((E_IM_IIP_PFMT_BAYER == pix_format) && (D_IM_IIP_ALPHA_TRUE == alpha)) ||
			((E_IM_IIP_PFMT_PLANE == pix_format) && (D_IM_IIP_ALPHA_TRUE == alpha))
			) {
			return E_IM_IIP_PARAM_PF_PDIST_16PIX;
		}

		if(
			(E_IM_IIP_PFMT_BAYER == pix_format) ||
			(E_IM_IIP_PFMT_PLANE == pix_format)
		   ) {
			return E_IM_IIP_PARAM_PF_PDIST_32PIX;
		}

		if( E_IM_IIP_PFMT_VIDEO == pix_format ) {
			return E_IM_IIP_PARAM_PF_PDIST_32PIX;
		}

		if( E_IM_IIP_FTYPE_CHUNKY == frame_type ) {
			if( E_IM_IIP_PFMT_YCC444 == pix_format ) {
				return E_IM_IIP_PARAM_PF_PDIST_8PIX;
			}
			else {
				return E_IM_IIP_PARAM_PF_PDIST_16PIX;
			}
		}
		else {
			return E_IM_IIP_PARAM_PF_PDIST_32PIX;
		}

	}
}

/**
@brief		Set 1D unit parameter
*/
static VOID im_iip_set_1d_unit_param( T_IM_IIP_PARAM_1DL* const p1D,
									  const T_IM_IIP_UTIL_PHVSZ* const phvsz,
									  const T_IM_IIP_UTIL_IMG_INT* const src,
									  const UINT32 pix_depth,
									  const E_IM_IIP_PARAM_CSEL cache_select )
{
	const T_IM_IIP_UTIL_RECT_INT* src_rect = &src->rect;

	memset( p1D, '\0', sizeof(*p1D) );

	p1D->header0.bit.BeginningAddress = 0x4;
	p1D->header0.bit.WordLength = 16;
	p1D->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
//	p1D->LD_TOPCNF0.bit.WAITCONF;		// set later
	p1D->LD_TOPCNF1.bit.LINK_UID = 0x4; // dummy. initial value

//	p1D->PIXIDDEF.bit.IPIXID;	// set later
	p1D->LD_CTL.bit.LD_MODE = E_IM_IIP_PARAM_LD_MODE_NORMAL_LOAD;
	p1D->LD_CTL.bit.LD_OUT = 0;
	p1D->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_2PIX_CYC;
	p1D->LD_CTL.bit.SAVERD = 0;
	p1D->LD_CTL.bit.CSEL = cache_select;
	p1D->LD_PREFETCH.bit.PFOFF = E_IM_IIP_PARAM_PFOFF_RUN;
	p1D->LD_PREFETCH.bit.PF_PDIST = im_iip_get_PF_PDIST( pix_depth,
														 src->gbl.frame_type,
														 src->gbl.pix_format,
														 src->gbl.alpha );
	p1D->LD_PREFETCH.bit.PF_1ST = E_IM_IIP_PARAM_PF_1ST_ACCESS_BEFORE_PF;
	p1D->LD_PREFETCH.bit.PFB_NUM = 0;
	p1D->LD_PREFETCH.bit.PF_ADD = 0;

	p1D->LD_HSIZE.bit.HSTART = src_rect->left;
	p1D->LD_HSIZE.bit.HEND = src_rect->left + src_rect->width - 1;
	p1D->LD_VSIZE.bit.VSTART = src_rect->top;
	p1D->LD_VSIZE.bit.VEND = src_rect->top  + src_rect->lines - 1;

	p1D->LD_PHSZ.bit.PHSZ0 = phvsz->PHSZ0;
	p1D->LD_PHSZ.bit.PHSZ1 = phvsz->PHSZ1;
	p1D->LD_PVSZ.bit.PVSZ0 = phvsz->PVSZ0;
	p1D->LD_PVSZ.bit.PVSZ1 = phvsz->PVSZ1;
	p1D->UPLK_TARGET.bit.UPSU0 = E_IM_IIP_PARAM_UPSU_NONE;
	p1D->UPLK_TARGET.bit.UPSU1 = E_IM_IIP_PARAM_UPSU_NONE;
	p1D->UPLK_TARGET.bit.UPSU2 = 0;
	p1D->DWLK_TARGET.bit.DWSU0 = E_IM_IIP_PARAM_DWSU_NONE;
	p1D->DWLK_TARGET.bit.DWSU1 = E_IM_IIP_PARAM_DWSU_NONE;
	p1D->DWLK_TARGET.bit.DWSU2 = 0;
	p1D->LKNUM.bit.UPNUM = E_IM_IIP_PARAM_UPNUM_NONE;
	p1D->LKNUM.bit.DWNUM = E_IM_IIP_PARAM_DWNUM_NONE;
	p1D->RING.bit.URINGSIZE = 0;
	p1D->RING.bit.UMARGINIYSZ = 0;
	p1D->RING.bit.DRINGSIZE = 0;
}

/**
@brief		Set SL unit parameter
*/
static VOID im_iip_set_sl_unit_param( T_IM_IIP_PARAM_STS* const sl,
									  const T_IM_IIP_UTIL_PHVSZ* const phvsz,
									  const T_IM_IIP_UTIL_RECT_INT* const dst_rect,
									  const T_IM_IIP_UTIL_IMG_GBL* const dst_gbl,
									  const UINT32 pix_depth )
{
	UINT32 max_val, min_val;

	memset( sl, '\0', sizeof(*sl) );

	sl->header0.bit.BeginningAddress = 0x0000;
	sl->header0.bit.WordLength = 26;
	sl->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY;
	sl->BASE.SL_TOPCNF0.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;
//	sl->BASE.SL_TOPCNF0.bit.DATACONF;		// set later
//	sl->BASE.PIXIDDEF.bit.OPIXID;			// set later
	sl->BASE.PIXIDDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0;	// dummy
	sl->BASE.PIXIDDEF.bit.DIV1ST = E_IM_IIP_PARAM_DIV1ST_1ST;
	sl->BASE.PIXIDDEF.bit.LKDIV = 0;

	sl->BASE.PLDUNIT_SL.bit.PLDUNIT_SL_LO = 0;
	sl->BASE.PLDUNIT_SL.bit.PLDUNIT_SL_HI = 0;

	sl->BASE.SL_CTL.bit.SL_MODE = 0;
	sl->BASE.SL_CTL.bit.LOADMD = 0;
	sl->BASE.SL_CTL.bit.MASKEN = E_IM_IIP_PARAM_MASKEN_NO_MASK;

	sl->BASE.OUTCOL.bit.OUTCOL = 0;

	sl->BASE.SL_HSIZE.bit.HSTART = dst_rect->left;
	sl->BASE.SL_HSIZE.bit.HEND = dst_rect->left + dst_rect->width - 1;
	sl->BASE.SL_VSIZE.bit.VSTART = dst_rect->top;
	sl->BASE.SL_VSIZE.bit.VEND = dst_rect->top  + dst_rect->lines - 1;
	sl->BASE.SL_PHSZ.bit.PHSZ0 = phvsz->PHSZ0;
	sl->BASE.SL_PHSZ.bit.PHSZ1 = phvsz->PHSZ1;
	sl->BASE.SL_PVSZ.bit.PVSZ0 = phvsz->PVSZ0;
	sl->BASE.SL_PVSZ.bit.PVSZ1 = phvsz->PVSZ1;

	sl->BASE.SEL_OPARA.bit.SEL_OPARA = 0;

	sl->BASE.OFSET_0.bit.OFSET = 0;
	im_iip_get_max_min_value( &max_val, &min_val, pix_depth, dst_gbl->sign_Y_G );
	sl->BASE.CLPTH_U_0.bit.CLPTH_U = max_val;
	sl->BASE.CLPTH_L_0.bit.CLPTH_L = min_val;
	sl->BASE.CLPVAL_U_0.bit.CLPVAL_U = sl->BASE.CLPTH_U_0.bit.CLPTH_U;
	sl->BASE.CLPVAL_L_0.bit.CLPVAL_L = sl->BASE.CLPTH_L_0.bit.CLPTH_L;
	sl->BASE.SFT_0.bit.SFTVAL = 0;
	sl->BASE.SFT_0.bit.SFTM = 0;
	sl->BASE.W14MD_0.bit.W14LSFT = 0;
	sl->BASE.W14MD_0.bit.W14SAT = 0;

	sl->header1.bit.BeginningAddress = 0x0200;
	sl->header1.bit.WordLength = 2;
	sl->header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
	sl->FILL_EN.bit.FILL_EN0 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN1 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN2 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN3 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN4 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN5 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN6 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN7 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN8 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN9 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN10 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN11 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN12 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN13 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN14 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN15 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN16 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN17 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN18 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
	sl->FILL_EN.bit.FILL_EN19 = E_IM_IIP_PARAM_FILL_EN_DISABLE;
}

/**
@brief		Execute Utility function
*/
static INT32 im_iip_util_exec_rotate( T_IM_IIP_UTIL_PARAM_RESIZE_ROTATE* const rr_param, const T_IM_IIP_UTIL_RR* const cfg )
{
	INT32	retval;
	UINT32	loopcnt;

	for ( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		retval = Im_IIP_Ctrl_PIXFMTTBL( rr_param->pixid[loopcnt], &rr_param->pixfmttbl[loopcnt] );
		if( retval != D_IM_IIP_OK ) {
			return retval;
		}
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->afn_unitid, &rr_param->unit_cfg_afn );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->sl_unitid, &rr_param->unit_cfg_sl );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	if( rr_param->axi_id_afn != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( rr_param->axi_id_afn, &rr_param->axi_cfg_afn );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	if( rr_param->axi_id_sl != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( rr_param->axi_id_sl, &rr_param->axi_cfg_sl );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	rr_param->wait_param.int_fact = im_iip_util_set_int_fact( rr_param->open_param.unitid_bitmask );

	retval = Im_IIP_Set_Interrupt( rr_param->wait_param.int_fact, 1 );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}


	im_iip_util_clean_dcache( rr_param->param_buffer_addr -D_IM_IIP_UTIL_RR_BUF_BYTES, D_IM_IIP_UTIL_RR_BUF_BYTES );


	retval = Im_IIP_Start_SWTRG( cfg->sl_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Start_SWTRG( cfg->afn_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	return D_IM_IIP_OK;
}

/**
@brief		Set Parameters of ResizeRotate Utility
*/
static INT32 im_iip_util_set_param_resizerotate( T_IM_IIP_UTIL_PARAM_RESIZE_ROTATE* const rr_param, const T_IM_IIP_UTIL_RR* const cfg )
{
	T_IM_IIP_PARAM_AFN*			afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_struct_param;
	T_IM_IIP_PARAM_STS*			sl;
	T_IM_IIP_UTIL_PHVSZ			phvsz;
	UINT32						mod_sz;

	/// Set Pointer of SDRAM parameter

	afn_struct_param = (T_IM_IIP_PARAM_AFN_OPCOL_0*)rr_param->param_buffer_addr;
	rr_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );

	afn = (T_IM_IIP_PARAM_AFN*)rr_param->param_buffer_addr;
	rr_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_AFN) );

	sl = (T_IM_IIP_PARAM_STS*)rr_param->param_buffer_addr;
	rr_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) );


//	printf( "SIZE: %u, %u\n", im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ) + im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_AFN) ) + im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) ), D_IM_IIP_UTIL_RR_BUF_BYTES );

	/// Set SDRAM paramter

	im_iip_set_afn_struct_param( afn_struct_param, &cfg->src, &cfg->dst, cfg->rotation, cfg->flip );

	im_iip_set_afn_unit_param( afn, (UINT32)afn_struct_param, cfg );

	phvsz.PHSZ1 = ( E_IM_IIP_PARAM_GHSZ_64PIX == afn->AFNCTL.bit.GHSZ ) ? 64 : 32;
	switch( afn->AFNCTL.bit.GVSZ ) {
		case E_IM_IIP_PARAM_GVSZ_SAME_AS_VERTICAL_PIX:
			 phvsz.PVSZ1 = afn_struct_param->SZ.bit.AFNDVSZ;
			 break;
		case E_IM_IIP_PARAM_GVSZ_256PIX:
			phvsz.PVSZ1 = 256;
			break;
		case E_IM_IIP_PARAM_GVSZ_128PIX:
			phvsz.PVSZ1 = 128;
			break;
		case E_IM_IIP_PARAM_GVSZ_64PIX:
			phvsz.PVSZ1 = 64;
			break;
		case E_IM_IIP_PARAM_GVSZ_32PIX:
			phvsz.PVSZ1 = 32;
			break;
		case E_IM_IIP_PARAM_GVSZ_16PIX:
			phvsz.PVSZ1 = 16;
			break;
		case E_IM_IIP_PARAM_GVSZ_8PIX:
			phvsz.PVSZ1 = 8;
			break;
		case E_IM_IIP_PARAM_GVSZ_4PIX:
			phvsz.PVSZ1 = 4;
			break;
		case E_IM_IIP_PARAM_GVSZ_2PIX:
			phvsz.PVSZ1 = 2;
			break;
		default:
			return D_IM_IIP_INVALID_ARG_ERR;
	}
	if( (afn->OUTSIZE.bit.OUTHSZ == 0) && (afn->OUTSIZE.bit.OUTVSZ == 0) ) {
		phvsz.PHSZ0 = phvsz.PHSZ1;
		phvsz.PVSZ0 = phvsz.PVSZ1;
	}
	else {
		mod_sz = afn->OUTSTARTPOS.bit.STARTX % phvsz.PHSZ1;
		phvsz.PHSZ0 = (mod_sz == 0) ? phvsz.PHSZ1 : (phvsz.PHSZ1 - mod_sz);
		mod_sz = afn->OUTSTARTPOS.bit.STARTY % phvsz.PVSZ1;
		phvsz.PVSZ0 = (mod_sz == 0) ? phvsz.PVSZ1 : (phvsz.PVSZ1 - mod_sz);
	}

	im_iip_set_sl_unit_param( sl, &phvsz, &cfg->dst.rect, &cfg->dst.gbl, cfg->pix_depth );

	afn->PIXIDEF.bit.IPIXID = rr_param->pixid[0];
	afn->AFNTOPCNF.bit.WAITCONF = im_iip_util_conv_portid( cfg->sl_unitid );

	sl->BASE.PIXIDDEF.bit.OPIXID = rr_param->pixid[1];
	sl->BASE.SL_TOPCNF0.bit.DATACONF = im_iip_util_conv_portid( cfg->afn_unitid );


	if( cfg->src.gbl.pix_format == E_IM_IIP_PFMT_BAYER ) {
		im_iip_util_set_afn_raw_input_unit( afn, afn_struct_param, sl );
	}


	rr_param->p_param_afn = afn;
	rr_param->p_afn_struct_param = afn_struct_param;
	rr_param->p_param_sl = sl;

	im_iip_set_unitinftbl_param( &rr_param->unit_cfg_afn, cfg->afn_unitid, afn, 0ULL );
	im_iip_set_unitinftbl_param( &rr_param->unit_cfg_sl, cfg->sl_unitid, sl, rr_param->open_param.unitid_bitmask );

	return D_IM_IIP_OK;
}

/**
@brief		Set Parameters of Resize & Rotation Utility
*/
static INT32 im_iip_util_resizerotate_main( T_IM_IIP_UTIL_PARAM_RESIZE_ROTATE* const rr_param, const T_IM_IIP_UTIL_RR* const cfg )
{
	INT32	retval;

	rr_param->open_param.unitid_bitmask = im_iip_util_conv_unitid_to_pldunit(cfg->afn_unitid)
										| im_iip_util_conv_unitid_to_pldunit(cfg->sl_unitid);
	rr_param->open_param.pixid_bitmask = cfg->src_pixid | cfg->dst_pixid;
	rr_param->open_param.open_res_bitmask = E_IM_IIP_OPEN_RES_NONE;

	if( cfg->resize_mode == E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC ) {
		switch( cfg->bi_cubic_unitid ) {
			case E_IM_IIP_PARAM_SCUB_0:
				rr_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_BICUBIC0;
				break;
			case E_IM_IIP_PARAM_SCUB_1:
				rr_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_BICUBIC1;
				break;
			default:
				return D_IM_IIP_INVALID_ARG_ERR;
		}
	}
	switch( cfg->afn_cache_select ) {
		case E_IM_IIP_PARAM_CSEL_0:
			rr_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE0;
			break;
		case E_IM_IIP_PARAM_CSEL_1:
			rr_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE1;
			break;
		case E_IM_IIP_PARAM_CSEL_2:
			rr_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE2;
			break;
		default:
			return D_IM_IIP_INVALID_ARG_ERR;
	}

	rr_param->pixid[0] = im_iip_util_conv_pixid( cfg->src_pixid );
	if( rr_param->pixid[0] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	rr_param->pixid[1] = im_iip_util_conv_pixid( cfg->dst_pixid );
	if( rr_param->pixid[1] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}

	retval = Im_IIP_Open_SWTRG( rr_param->open_param.unitid_bitmask, rr_param->open_param.pixid_bitmask, rr_param->open_param.open_res_bitmask, rr_param->wait_param.wait_time );
	if( retval != D_IM_IIP_OK ) {
		return retval;
	}


	while( 1 ) {
		/// Set SDRAM parameter and UNITINFTBL parameter
		im_iip_util_set_param_resizerotate( rr_param, cfg );

		/// Set IIP driver parameter
		im_iip_set_pixfmttbl_param( &rr_param->pixfmttbl[0], &cfg->src.gbl, cfg->pix_depth, NULL );
		im_iip_set_pixfmttbl_param( &rr_param->pixfmttbl[1], &cfg->dst.gbl, cfg->pix_depth, NULL );

		if( cfg->src.gbl.pix_format == E_IM_IIP_PFMT_BAYER ) {
			im_iip_util_set_afn_raw_input_pixfmttbl( &rr_param->pixfmttbl[0] );
		}

		im_iip_set_axi_param( cfg->afn_unitid, &rr_param->axi_id_afn, &rr_param->axi_cfg_afn, rr_param->param_master_IF );

		im_iip_set_axi_param( cfg->sl_unitid, &rr_param->axi_id_sl, &rr_param->axi_cfg_sl, rr_param->param_master_IF );

		/// Execute IIP
		retval = im_iip_util_exec_rotate( rr_param, cfg );
		if( retval != D_IM_IIP_OK ) {
			break;
		}

		// Unit start successful
		return retval;
	}

	// Error route
	(VOID)Im_IIP_Close_SWTRG( rr_param->open_param.unitid_bitmask, rr_param->open_param.pixid_bitmask, rr_param->open_param.open_res_bitmask );

	return retval;
}

/**
@brief		Execute Utility function
*/
static INT32 im_iip_util_exec_csc( T_IM_IIP_UTIL_PARAM_CSC* const csc_param, const T_IM_IIP_UTIL_CSC* const cfg )
{
	INT32	retval;
	UINT32	loopcnt;

	for ( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		retval = Im_IIP_Ctrl_PIXFMTTBL( csc_param->pixid[loopcnt], &csc_param->pixfmttbl[loopcnt] );
		if( retval != D_IM_IIP_OK ) {
			return retval;
		}
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->ld_unitid, &csc_param->unit_cfg_1d );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->csc_unitid, &csc_param->unit_cfg_csc );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->sl_unitid, &csc_param->unit_cfg_sl );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	if( csc_param->axi_id_ld != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( csc_param->axi_id_ld, &csc_param->axi_cfg_1d );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	if( csc_param->axi_id_sl != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( csc_param->axi_id_sl, &csc_param->axi_cfg_sl );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	csc_param->wait_param.int_fact = im_iip_util_set_int_fact( csc_param->open_param.unitid_bitmask );

	retval = Im_IIP_Set_Interrupt( csc_param->wait_param.int_fact, 1 );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}


	im_iip_util_clean_dcache( csc_param->param_buffer_addr -D_IM_IIP_UTIL_CSC_BUF_BYTES, D_IM_IIP_UTIL_CSC_BUF_BYTES );


	retval = Im_IIP_Start_SWTRG( cfg->sl_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Start_SWTRG( cfg->csc_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Start_SWTRG( cfg->ld_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	return D_IM_IIP_OK;
}

/**
@brief		Set CSC unit parameter
*/
static VOID im_iip_set_csc_unit_param( T_IM_IIP_PARAM_CSC* const csc,
									   const T_IM_IIP_UTIL_CSC* const cfg )
{
	INT32	loopcnt;

	memset( csc, '\0', sizeof(*csc) );

	csc->header0.bit.BeginningAddress = 0x0000;
	csc->header0.bit.WordLength = 25;
	csc->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
//	csc->CSCTOPCNF.bit.DATACONF;	// set later
//	csc->CSCTOPCNF.bit.WAITCONF;	// set later

	for ( loopcnt = 0; loopcnt < 4; loopcnt++ ) {
		csc->CSCK[loopcnt].bit.CSCK_0 = cfg->csck[loopcnt][0];
		csc->CSCK[loopcnt].bit.CSCK_1 = cfg->csck[loopcnt][1];
		csc->CSCK[loopcnt].bit.CSCK_2 = cfg->csck[loopcnt][2];
		csc->CSCK[loopcnt].bit.CSCK_3 = cfg->csck[loopcnt][3];
	}

	csc->MDSEL.bit.MDSEL = cfg->csck_mode_sel;
	csc->ALPSEL.bit.ALPSEL = cfg->alpha_in_sel;
	csc->ALPSEL.bit.ALOSEL = cfg->alpha_out_sel;
	csc->ALPVAL.bit.ALPVAL = cfg->alpha_val;
	csc->MAXMIN.bit.MAXMIN0 = E_IM_IIP_PARAM_MAXMIN_THROUGH;
	csc->MAXMIN.bit.MAXMIN1 = E_IM_IIP_PARAM_MAXMIN_THROUGH;
	csc->MAXMIN.bit.MAXMIN2 = E_IM_IIP_PARAM_MAXMIN_THROUGH;
	csc->MAXMIN.bit.MAXMIN3 = E_IM_IIP_PARAM_MAXMIN_THROUGH;
	csc->OUTCLIP.bit.CLPMAX0 = 0x3FFF;
	csc->OUTCLIP.bit.CLPMIN0 = -0x4000;
	csc->OUTCLIP.bit.CLPMAX1 = 0x3FFF;
	csc->OUTCLIP.bit.CLPMIN1 = -0x4000;
	csc->OUTCLIP.bit.CLPMAX2 = 0x3FFF;
	csc->OUTCLIP.bit.CLPMIN2 = -0x4000;
	csc->OUTCLIP.bit.CLPMAX3 = 0xFF;
	csc->OUTCLIP.bit.CLPMIN3 = -0x100;
}

/**
@brief		Set Parameters of CSC Utility
*/
static VOID im_iip_util_set_param_csc( T_IM_IIP_UTIL_PARAM_CSC* const csc_param, const T_IM_IIP_UTIL_CSC* const cfg )
{
	T_IM_IIP_PARAM_1DL*			oned;
	T_IM_IIP_PARAM_CSC*			csc;
	T_IM_IIP_PARAM_STS*			sl;
	T_IM_IIP_UTIL_PHVSZ			phvsz;

	/// Set Pointer of SDRAM parameter

	oned = (T_IM_IIP_PARAM_1DL*)csc_param->param_buffer_addr;
	csc_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_1DL) );

	csc = (T_IM_IIP_PARAM_CSC*)csc_param->param_buffer_addr;
	csc_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_CSC) );

	sl = (T_IM_IIP_PARAM_STS*)csc_param->param_buffer_addr;
	csc_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) );


	/// Set SDRAM paramter
	im_iip_set_phvsz_1d_sl( &phvsz, &cfg->dst );

	im_iip_set_1d_unit_param( oned, &phvsz, &cfg->src, cfg->pix_depth, cfg->ld_cache_select );

	im_iip_set_csc_unit_param( csc, cfg );

	im_iip_set_sl_unit_param( sl, &phvsz, &cfg->dst.rect, &cfg->dst.gbl, cfg->pix_depth );

	//Set Shift bit
	sl->BASE.SFT_0.bit.SFTVAL = cfg->sl_sftval;

	oned->PIXIDDEF.bit.IPIXID = csc_param->pixid[0];
	oned->LD_TOPCNF0.bit.WAITCONF = im_iip_util_conv_portid( cfg->csc_unitid );

	csc->CSCTOPCNF.bit.DATACONF = im_iip_util_conv_portid( cfg->ld_unitid );
	csc->CSCTOPCNF.bit.WAITCONF = im_iip_util_conv_portid( cfg->sl_unitid );

	sl->BASE.PIXIDDEF.bit.OPIXID = csc_param->pixid[1];
	sl->BASE.SL_TOPCNF0.bit.DATACONF = im_iip_util_conv_portid( cfg->csc_unitid );

	csc_param->p_param_1d = oned;
	csc_param->p_param_csc = csc;
	csc_param->p_param_sl = sl;

	im_iip_set_unitinftbl_param( &csc_param->unit_cfg_1d, cfg->ld_unitid, oned, 0ULL );
	im_iip_set_unitinftbl_param( &csc_param->unit_cfg_csc, cfg->csc_unitid, csc, 0ULL );
	im_iip_set_unitinftbl_param( &csc_param->unit_cfg_sl, cfg->sl_unitid, sl, csc_param->open_param.unitid_bitmask );
}

/**
@brief		Set Parameters of CSC Utility
*/
static INT32 im_iip_util_csc_main( T_IM_IIP_UTIL_PARAM_CSC* const csc_param, const T_IM_IIP_UTIL_CSC* const cfg, const INT32 wait_time )
{
	INT32	retval;
	T_IM_IIP_UTIL_ALPHA_INFO	alpha_info;

	csc_param->open_param.unitid_bitmask = im_iip_util_conv_unitid_to_pldunit(cfg->ld_unitid)
										 | im_iip_util_conv_unitid_to_pldunit(cfg->csc_unitid)
										 | im_iip_util_conv_unitid_to_pldunit(cfg->sl_unitid);
	csc_param->open_param.pixid_bitmask = cfg->src_pixid | cfg->dst_pixid;
	csc_param->open_param.open_res_bitmask = E_IM_IIP_OPEN_RES_NONE;

	switch( cfg->ld_cache_select ) {
		case E_IM_IIP_PARAM_CSEL_0:
			csc_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE0;
			break;
		case E_IM_IIP_PARAM_CSEL_1:
			csc_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE1;
			break;
		case E_IM_IIP_PARAM_CSEL_2:
			csc_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE2;
			break;
		default:
			return D_IM_IIP_INVALID_ARG_ERR;
	}

	csc_param->pixid[0] = im_iip_util_conv_pixid( cfg->src_pixid );
	if( csc_param->pixid[0] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	csc_param->pixid[1] = im_iip_util_conv_pixid( cfg->dst_pixid );
	if( csc_param->pixid[1] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}

	retval = Im_IIP_Open_SWTRG( csc_param->open_param.unitid_bitmask, csc_param->open_param.pixid_bitmask, csc_param->open_param.open_res_bitmask, csc_param->wait_param.wait_time );
	if( retval != D_IM_IIP_OK ) {
		return retval;
	}


	while( 1 ) {
		/// Set SDRAM parameter and UNITINFTBL parameter
		im_iip_util_set_param_csc( csc_param, cfg );

		/// Set IIP driver parameter
		alpha_info.alpha_depth = cfg->alpha_depth;
		alpha_info.alpha_subsampling = cfg->alpha_subsampling;
		im_iip_set_pixfmttbl_param( &csc_param->pixfmttbl[0], &cfg->src.gbl, cfg->pix_depth, &alpha_info );
		im_iip_set_pixfmttbl_param( &csc_param->pixfmttbl[1], &cfg->dst.gbl, cfg->pix_depth, &alpha_info );

		im_iip_set_axi_param( cfg->ld_unitid, &csc_param->axi_id_ld, &csc_param->axi_cfg_1d, csc_param->param_master_IF );

		im_iip_set_axi_param( cfg->sl_unitid, &csc_param->axi_id_sl, &csc_param->axi_cfg_sl, csc_param->param_master_IF );

		/// Execute IIP
		retval = im_iip_util_exec_csc( csc_param, cfg );
		if( retval != D_IM_IIP_OK ) {
			break;
		}

		// Unit start successful
		return retval;
	}

	// Error route
	(VOID)Im_IIP_Close_SWTRG( csc_param->open_param.unitid_bitmask, csc_param->open_param.pixid_bitmask, csc_param->open_param.open_res_bitmask );

	return retval;
}

/**
@brief		Execute Utility function
*/
static INT32 im_iip_util_exec_lut( T_IM_IIP_UTIL_PARAM_LUT* const lut_param, const T_IM_IIP_UTIL_LUT* const cfg )
{
	INT32	retval;
	UINT32	loopcnt;

	for ( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		retval = Im_IIP_Ctrl_PIXFMTTBL( lut_param->pixid[loopcnt], &lut_param->pixfmttbl[loopcnt] );
		if( retval != D_IM_IIP_OK ) {
			return retval;
		}
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->ld_unitid, &lut_param->unit_cfg_1d );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->lut_unitid, &lut_param->unit_cfg_lut );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->sl_unitid, &lut_param->unit_cfg_sl );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	if( lut_param->axi_id_ld != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( lut_param->axi_id_ld, &lut_param->axi_cfg_1d );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	if( lut_param->axi_id_sl != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( lut_param->axi_id_sl, &lut_param->axi_cfg_sl );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	lut_param->wait_param.int_fact = im_iip_util_set_int_fact( lut_param->open_param.unitid_bitmask );

	retval = Im_IIP_Set_Interrupt( lut_param->wait_param.int_fact, 1 );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}


	im_iip_util_clean_dcache( lut_param->param_buffer_addr -D_IM_IIP_UTIL_LUT_BUF_BYTES, D_IM_IIP_UTIL_LUT_BUF_BYTES );

#if 0
	{	// Print register for debug.
		extern VOID CT_Im_IIP_Print_Unitcfg_Byid( const E_IM_IIP_UNIT_ID unitid );
		extern VOID CT_Im_IIP_Print_Pixfmttbl_Byid( const E_IM_IIP_PIXID pixid );

		CT_Im_IIP_Print_Unitcfg_Byid( cfg->ld_unitid );
		CT_Im_IIP_Print_Unitcfg_Byid( cfg->lut_unitid );
		CT_Im_IIP_Print_Unitcfg_Byid( cfg->sl_unitid );
		CT_Im_IIP_Print_Pixfmttbl_Byid( cfg->src_pixid );
		CT_Im_IIP_Print_Pixfmttbl_Byid( cfg->dst_pixid );
	}
#endif

	retval = Im_IIP_Start_SWTRG( cfg->sl_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Start_SWTRG( cfg->lut_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Start_SWTRG( cfg->ld_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	return D_IM_IIP_OK;
}

/**
@brief		Set LUTRAM
@param[in]	lutram	LUTRAM paramters
@param[in]	alpha	Alpha ON/OFF info.
*/
static INT32 im_iip_set_LUTRAM( const T_IM_IIP_LUTRAM* const lutram,
							    const UINT32 alpha )
{
	T_IM_IIP_LUT			lut;
	INT32					retval = D_IM_IIP_ERR;
	E_IM_IIP_LUTRAM_TYPE	loopcnt;

	for ( loopcnt = E_IM_IIP_LUTRAM_TYPE_A; loopcnt < E_IM_IIP_LUTRAM_TYPE_MAX; loopcnt++ ) {
		lut.buffer_bytes = lutram->buffer_bytes[loopcnt];
		lut.buffer_addr = lutram->buffer_addr[loopcnt];
		lut.lutram_type = loopcnt;
		retval = Im_IIP_Ctrl_LUT( &lut );
		if( retval != D_IM_IIP_OK ) {
			return retval;
		}
	}
	return retval;
}

/**
@brief		Set LUT unit parameter
@param[out]	lut				Pointer of LUT unit paramter
@param[in]	cfg				Pointer of LUT parameter
*/
static VOID im_iip_util_set_lut_unit_param( T_IM_IIP_PARAM_LUT* const lut,
									   const T_IM_IIP_UTIL_LUT* const cfg )
{
	INT32	loopcnt;

	memset( lut, '\0', sizeof(*lut) );

	lut->header0.bit.BeginningAddress = 0x0000;
	lut->header0.bit.WordLength = 57;
	lut->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
//	lut->LUTTOPCNF[0].bit.DATACONF;	// set later
//	lut->LUTTOPCNF[0].bit.WAITCONF;	// set later
	for ( loopcnt = 1; loopcnt < 4; loopcnt++ ) {
		lut->LUTTOPCNF[loopcnt].bit.DATACONF = E_IM_IIP_PARAM_PORTID_NONE;
		lut->LUTTOPCNF[loopcnt].bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;
	}
	lut->LUTUNITCTL.bit.PORTEN = 0x1;
	lut->LUTUNITCTL.bit.PORT3SELI = 0;
	lut->LUTUNITCTL.bit.PORT3SELO = 0;
	lut->LUTUNITCTL.bit.BRANCH = 0;
	lut->LUTUNITCTL.bit.PORT2SELI = 0;
	lut->LUTUNITCTL.bit.PORT2SELO = 0;
	lut->LUTUNITCTL.bit.OUTMD0 = 0;
	lut->LUTUNITCTL.bit.OUTMD1 = 0;
	lut->LUTUNITCTL.bit.OUTMD2 = 0;
	lut->LUTUNITCTL.bit.OUTMD3 = 0;

	// LUT A
	lut->LUTCTL_A.bit.LUTBIT = cfg->lut_pix_depth[0];
	lut->LUTCTL_A.bit.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT;
	lut->LUTCTL_A.bit.SGN = cfg->src.gbl.sign_Y_G;
	lut->LUTCTL_A.bit.SPLBIT = cfg->splbit[0];
	lut->LUTPRECAL_A.bit.SHIFT = 0;
	lut->LUTPRECAL_A.bit.OFSET = 0;
	lut->LUTPRECAL_A.bit.CLPMAX = 0x3FFF;
	lut->LUTPRECAL_A.bit.THMAX = 0x3FFF;
	lut->LUTPRECAL_A.bit.CLPMIN = -0x4000;
	lut->LUTPRECAL_A.bit.THMIN = -0x4000;
	lut->LUTPRECAL_A.bit.ABS = 0x0;
	lut->LUTPRECAL_A.bit.LUTADRS = 0x0000;

	// LUT B
	lut->LUTCTL_B.bit.LUTBIT = cfg->lut_pix_depth[1];
	lut->LUTCTL_B.bit.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT;
	lut->LUTCTL_B.bit.SGN = cfg->src.gbl.sign_Cb_B;
	lut->LUTCTL_B.bit.SPLBIT = cfg->splbit[1];
	lut->LUTPRECAL_B.bit.SHIFT = 0;
	lut->LUTPRECAL_B.bit.OFSET = 0;
	lut->LUTPRECAL_B.bit.CLPMAX = 0x3FFF;
	lut->LUTPRECAL_B.bit.THMAX = 0x3FFF;
	lut->LUTPRECAL_B.bit.CLPMIN = -0x4000;
	lut->LUTPRECAL_B.bit.THMIN = -0x4000;
	lut->LUTPRECAL_B.bit.ABS = 0x0;
	lut->LUTPRECAL_B.bit.LUTADRS = 0x2000;

	// LUT C
	lut->LUTCTL_C.bit.LUTBIT = cfg->lut_pix_depth[2];
	lut->LUTCTL_C.bit.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT;
	lut->LUTCTL_C.bit.SGN = cfg->src.gbl.sign_Cr_R;
	lut->LUTCTL_C.bit.SPLBIT = cfg->splbit[2];
	lut->LUTPRECAL_C.bit.SHIFT = 0;
	lut->LUTPRECAL_C.bit.OFSET = 0;
	lut->LUTPRECAL_C.bit.CLPMAX = 0x3FFF;
	lut->LUTPRECAL_C.bit.THMAX = 0x3FFF;
	lut->LUTPRECAL_C.bit.CLPMIN = -0x4000;
	lut->LUTPRECAL_C.bit.THMIN = -0x4000;
	lut->LUTPRECAL_C.bit.ABS = 0x0;
	lut->LUTPRECAL_C.bit.LUTADRS = 0x4000;

	// LUT D
	lut->LUTCTL_D.bit.LUTBIT = cfg->lut_pix_depth[3];
	lut->LUTCTL_D.bit.USE = E_IM_IIP_PARAM_LUTUSE_PRECALC;
	lut->LUTCTL_D.bit.SGN = cfg->src.gbl.sign_D3;
	lut->LUTCTL_D.bit.SPLBIT = cfg->splbit[3];
	lut->LUTPRECAL_D.bit.SHIFT = 0;
	lut->LUTPRECAL_D.bit.OFSET = 0;
	lut->LUTPRECAL_D.bit.CLPMAX = 0x3FFF;
	lut->LUTPRECAL_D.bit.THMAX = 0x3FFF;
	lut->LUTPRECAL_D.bit.CLPMIN = -0x4000;
	lut->LUTPRECAL_D.bit.THMIN = -0x4000;
	lut->LUTPRECAL_D.bit.ABS = 0x0;
	lut->LUTPRECAL_D.bit.LUTADRS = 0x1000;

	// LUT E
	lut->LUTCTL_E.bit.LUTBIT = cfg->lut_pix_depth[0];
	lut->LUTCTL_E.bit.USE = E_IM_IIP_PARAM_LUTUSE_PRECALC;
	lut->LUTCTL_E.bit.SGN = cfg->src.gbl.sign_Y_G;
	lut->LUTCTL_E.bit.SPLBIT = 0;
	lut->LUTPRECAL_E.bit.SHIFT = 0;
	lut->LUTPRECAL_E.bit.OFSET = 0;
	lut->LUTPRECAL_E.bit.CLPMAX = 0x3FFF;
	lut->LUTPRECAL_E.bit.THMAX = 0x3FFF;
	lut->LUTPRECAL_E.bit.CLPMIN = -0x4000;
	lut->LUTPRECAL_E.bit.THMIN = -0x4000;
	lut->LUTPRECAL_E.bit.ABS = 0x0;
	lut->LUTPRECAL_E.bit.LUTADRS = 0x3000;

	// LUT F
	lut->LUTCTL_F.bit.LUTBIT = cfg->lut_pix_depth[1];
	lut->LUTCTL_F.bit.USE = E_IM_IIP_PARAM_LUTUSE_PRECALC;
	lut->LUTCTL_F.bit.SGN = cfg->src.gbl.sign_Cb_B;
	lut->LUTCTL_F.bit.SPLBIT = 0;
	lut->LUTPRECAL_F.bit.SHIFT = 0;
	lut->LUTPRECAL_F.bit.OFSET = 0;
	lut->LUTPRECAL_F.bit.CLPMAX = 0x3FFF;
	lut->LUTPRECAL_F.bit.THMAX = 0x3FFF;
	lut->LUTPRECAL_F.bit.CLPMIN = -0x4000;
	lut->LUTPRECAL_F.bit.THMIN = -0x4000;
	lut->LUTPRECAL_F.bit.ABS = 0x0;
	lut->LUTPRECAL_F.bit.LUTADRS = 0x5000;

}

/**
@brief		Set Parameters of LUT Utility
*/
static VOID im_iip_util_set_param_lut( T_IM_IIP_UTIL_PARAM_LUT* const lut_param, const T_IM_IIP_UTIL_LUT* const cfg )
{
	T_IM_IIP_PARAM_1DL*			oned;
	T_IM_IIP_PARAM_LUT*			lut;
	T_IM_IIP_PARAM_STS*			sl;
	T_IM_IIP_UTIL_PHVSZ			phvsz;

	/// Set Pointer of SDRAM parameter

	oned = (T_IM_IIP_PARAM_1DL*)lut_param->param_buffer_addr;
	lut_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_1DL) );

	lut = (T_IM_IIP_PARAM_LUT*)lut_param->param_buffer_addr;
	lut_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_LUT) );

	sl = (T_IM_IIP_PARAM_STS*)lut_param->param_buffer_addr;
	lut_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) );


	/// Set SDRAM paramter
	im_iip_set_phvsz_1d_sl( &phvsz, &cfg->dst );

	im_iip_set_1d_unit_param( oned, &phvsz, &cfg->src, cfg->pix_depth, cfg->ld_cache_select );

	im_iip_util_set_lut_unit_param( lut, cfg );

	im_iip_set_sl_unit_param( sl, &phvsz, &cfg->dst.rect, &cfg->dst.gbl, cfg->pix_depth );

	oned->PIXIDDEF.bit.IPIXID = lut_param->pixid[0];
	oned->LD_TOPCNF0.bit.WAITCONF = im_iip_util_conv_portid( cfg->lut_unitid );

	lut->LUTTOPCNF[0].bit.DATACONF = im_iip_util_conv_portid( cfg->ld_unitid );
	lut->LUTTOPCNF[0].bit.WAITCONF = im_iip_util_conv_portid( cfg->sl_unitid );

	sl->BASE.PIXIDDEF.bit.OPIXID = lut_param->pixid[1];
	sl->BASE.SL_TOPCNF0.bit.DATACONF = im_iip_util_conv_portid( cfg->lut_unitid );

	lut_param->p_param_1d = oned;
	lut_param->p_param_lut = lut;
	lut_param->p_param_sl = sl;

	im_iip_set_unitinftbl_param( &lut_param->unit_cfg_1d, cfg->ld_unitid, oned, 0ULL );
	im_iip_set_unitinftbl_param( &lut_param->unit_cfg_lut, cfg->lut_unitid, lut, 0ULL );
	im_iip_set_unitinftbl_param( &lut_param->unit_cfg_sl, cfg->sl_unitid, sl, lut_param->open_param.unitid_bitmask );
}

/**
@brief		Set Parameters of LUT Utility
*/
static INT32 im_iip_util_lut_main( T_IM_IIP_UTIL_PARAM_LUT* const lut_param, const T_IM_IIP_UTIL_LUT* const cfg )
{
	INT32						retval;
	T_IM_IIP_UTIL_ALPHA_INFO	alpha_info;

	lut_param->open_param.unitid_bitmask = im_iip_util_conv_unitid_to_pldunit(cfg->ld_unitid)
										 | im_iip_util_conv_unitid_to_pldunit(cfg->lut_unitid)
										 | im_iip_util_conv_unitid_to_pldunit(cfg->sl_unitid);
	lut_param->open_param.pixid_bitmask = cfg->src_pixid | cfg->dst_pixid;
	lut_param->open_param.open_res_bitmask = E_IM_IIP_OPEN_RES_NONE | E_IM_IIP_OPEN_RES_LUT_A | E_IM_IIP_OPEN_RES_LUT_B | E_IM_IIP_OPEN_RES_LUT_C | E_IM_IIP_OPEN_RES_LUT_D | E_IM_IIP_OPEN_RES_LUT_E | E_IM_IIP_OPEN_RES_LUT_F;

	switch( cfg->ld_cache_select ) {
		case E_IM_IIP_PARAM_CSEL_0:
			lut_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE0;
			break;
		case E_IM_IIP_PARAM_CSEL_1:
			lut_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE1;
			break;
		case E_IM_IIP_PARAM_CSEL_2:
			lut_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE2;
			break;
		default:
			return D_IM_IIP_INVALID_ARG_ERR;
	}

	lut_param->pixid[0] = im_iip_util_conv_pixid( cfg->src_pixid );
	if( lut_param->pixid[0] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	lut_param->pixid[1] = im_iip_util_conv_pixid( cfg->dst_pixid );
	if( lut_param->pixid[1] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}

	retval = Im_IIP_Open_SWTRG( lut_param->open_param.unitid_bitmask, lut_param->open_param.pixid_bitmask, lut_param->open_param.open_res_bitmask, lut_param->wait_param.wait_time );
	if( retval != D_IM_IIP_OK ) {
		return retval;
	}


	while( 1 ) {
		/// Set SDRAM parameter and UNITINFTBL parameter
		im_iip_util_set_param_lut( lut_param, cfg );

		/// Set LUTRAM
		retval = im_iip_set_LUTRAM( &cfg->lutram, cfg->src.gbl.alpha );
		if( retval != D_IM_IIP_OK ) {
			break;
		}

		/// Set IIP driver parameter
		alpha_info.alpha_depth = cfg->alpha_depth;
		alpha_info.alpha_subsampling = cfg->alpha_subsampling;
		im_iip_set_pixfmttbl_param( &lut_param->pixfmttbl[0], &cfg->src.gbl, cfg->pix_depth, &alpha_info );
		im_iip_set_pixfmttbl_param( &lut_param->pixfmttbl[1], &cfg->dst.gbl, cfg->pix_depth, &alpha_info );

		im_iip_set_axi_param( cfg->ld_unitid, &lut_param->axi_id_ld, &lut_param->axi_cfg_1d, lut_param->param_master_IF );
		im_iip_set_axi_param( cfg->sl_unitid, &lut_param->axi_id_sl, &lut_param->axi_cfg_sl, lut_param->param_master_IF );

		/// Execute IIP
		retval = im_iip_util_exec_lut( lut_param, cfg );
		if( retval != D_IM_IIP_OK ) {
			break;
		}

		// Unit start successful
		return retval;
	}

	// Error route
	(VOID)Im_IIP_Close_SWTRG( lut_param->open_param.unitid_bitmask, lut_param->open_param.pixid_bitmask, lut_param->open_param.open_res_bitmask );

	return retval;
}

/**
@brief		Execute Utility function
*/
static INT32 im_iip_util_exec_alpha( T_IM_IIP_UTIL_PARAM_ALPHA* const alp_param, const T_IM_IIP_UTIL_ALPHABLEND* const cfg )
{
	INT32	retval;
	UINT32	loopcnt;

	for ( loopcnt = 0; loopcnt < 3; loopcnt++ ) {
		retval = Im_IIP_Ctrl_PIXFMTTBL( alp_param->pixid[loopcnt], &alp_param->pixfmttbl[loopcnt] );
		if( retval != D_IM_IIP_OK ) {
			return retval;
		}
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->ld_unitid[0], &alp_param->unit_cfg_1d[0] );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->ld_unitid[1], &alp_param->unit_cfg_1d[1] );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->blend_unitid, &alp_param->unit_cfg_alp );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Ctrl_SWTRG_Unit( cfg->sl_unitid, &alp_param->unit_cfg_sl );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	if( alp_param->axi_id_ld[0] != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( alp_param->axi_id_ld[0], &alp_param->axi_cfg_1d[0] );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	if( alp_param->axi_id_ld[1] != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( alp_param->axi_id_ld[1], &alp_param->axi_cfg_1d[1] );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	if( alp_param->axi_id_sl != E_IM_IIP_AXI_ID_MAX ) {
		retval = Im_IIP_Ctrl_AXI( alp_param->axi_id_sl, &alp_param->axi_cfg_sl );
		if( retval != D_IM_IIP_OK ) {
				return retval;
		}
	}

	alp_param->wait_param.int_fact = im_iip_util_set_int_fact( alp_param->open_param.unitid_bitmask );

	retval = Im_IIP_Set_Interrupt( alp_param->wait_param.int_fact, 1 );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}


	im_iip_util_clean_dcache( alp_param->param_buffer_addr -D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES, D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES );


	retval = Im_IIP_Start_SWTRG( cfg->sl_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Start_SWTRG( cfg->blend_unitid, D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Start_SWTRG( cfg->ld_unitid[1], D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	retval = Im_IIP_Start_SWTRG( cfg->ld_unitid[0], D_IM_IIP_SWTRG_ON );
	if( retval != D_IM_IIP_OK ) {
			return retval;
	}

	return D_IM_IIP_OK;
}

/**
@brief		Set Alpha unit parameter
@param[out]	alpha			Pointer of ALPHA unit paramter
@param[in]	cfg				Pointer of Alpha parameter
*/
static VOID im_iip_set_alpha_unit_param( T_IM_IIP_PARAM_BLEND* const alpha,
										 const T_IM_IIP_UTIL_ALPHABLEND* const cfg )
{
	memset( alpha, '\0', sizeof(*alpha) );

	alpha->header0.bit.BeginningAddress = 0x0000;
	alpha->header0.bit.WordLength = 4;
	alpha->header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY;
//	alpha->BLENDTOPCNF0.bit.WAITCONF_0;	// set later
//	alpha->BLENDTOPCNF0.bit.DATACONF_0;	// set later
//	alpha->BLENDTOPCNF1.bit.WAITCONF_1;	// set later
//	alpha->BLENDTOPCNF1.bit.DATACONF_1;	// set later
	alpha->BLENDTOPCNF2.bit.DATACONF_2 = E_IM_IIP_PARAM_PORTID_NONE;

	alpha->header1.bit.BeginningAddress = 0x0080;
	alpha->header1.bit.WordLength = 24;
	alpha->header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET;
	alpha->BLENDCTL.bit.BLD_MD = E_IM_IIP_PARAM_BLD_MD_ALPHA_BLEND;
	alpha->BLENDCTL.bit.CLP_MD = E_IM_IIP_PARAM_CLP_MD_NO_CLIP;
	if( cfg->dst.gbl.alpha == D_IM_IIP_ALPHA_TRUE ) {
		alpha->BLENDCTL.bit.ALPHA_OUT = cfg->alpha.out;
	}
	else {
		alpha->BLENDCTL.bit.ALPHA_OUT = E_IM_IIP_PARAM_ALPHA_OUT_NONE;
	}

	alpha->ALPHACTL0.bit.ALPHA_STEP = cfg->alpha.step;
	alpha->ALPHACTL0.bit.ALPHA_SEL = cfg->alpha.sel;
	alpha->ALPHACTL1.bit.ALPHA_VAL0 = cfg->alpha.ALPHA_VAL[0];
	alpha->ALPHACTL1.bit.ALPHA_VAL1 = cfg->alpha.ALPHA_VAL[1];
	alpha->ALPHACTL2.bit.ALPHA_VAL2 = cfg->alpha.ALPHA_VAL[2];
	alpha->ALPHACTL2.bit.ALPHA_VAL3 = cfg->alpha.ALPHA_VAL[3];

// alpha->WADCTl[0].bit.WADD_KA = 0;	// not used
// alpha->WADCTl[0].bit.WADD_KB = 0;	// not used
// alpha->WADCTl[1].bit.WADD_KA = 0;	// not used
// alpha->WADCTl[1].bit.WADD_KB = 0;	// not used
// alpha->WADCTl[2].bit.WADD_KA = 0;	// not used
// alpha->WADCTl[2].bit.WADD_KB = 0;	// not used
// alpha->WADCTl[3].bit.WADD_KA = 0;	// not used
// alpha->WADCTl[3].bit.WADD_KB = 0;	// not used
// alpha->DIVCTL0.bit.RSFT = 0;	// not used
// alpha->MSKCTL0.bit.MSKSEL = 0;	// not used
// alpha->MSKCTL0.bit.DINSEL = 0;	// not used
// alpha->MULCTL[0].bit.ADDVAL_A = 0;	// not used
// alpha->MULCTL[0].bit.ADDVAL_B = 0;	// not used
// alpha->MULCTL[0].bit.ADDVAL_C = 0;	// not used
// alpha->MULCTL[0].bit.SFTSEL = 0;	// not used
// alpha->MULCTL[1].bit.ADDVAL_A = 0;	// not used
// alpha->MULCTL[1].bit.ADDVAL_B = 0;	// not used
// alpha->MULCTL[1].bit.ADDVAL_C = 0;	// not used
// alpha->MULCTL[1].bit.SFTSEL = 0;	// not used
// alpha->MULCTL[2].bit.ADDVAL_A = 0;	// not used
// alpha->MULCTL[2].bit.ADDVAL_B = 0;	// not used
// alpha->MULCTL[2].bit.ADDVAL_C = 0;	// not used
// alpha->MULCTL[2].bit.SFTSEL = 0;	// not used
// alpha->MULCTL[3].bit.ADDVAL_A = 0;	// not used
// alpha->MULCTL[3].bit.ADDVAL_B = 0;	// not used
// alpha->MULCTL[3].bit.ADDVAL_C = 0;	// not used
// alpha->MULCTL[3].bit.SFTSEL = 0;	// not used
}

/**
@brief		Set Parameters of Alpha Blend Utility
*/
static VOID im_iip_util_set_param_alpha( T_IM_IIP_UTIL_PARAM_ALPHA* const alp_param, const T_IM_IIP_UTIL_ALPHABLEND* const cfg )
{
	T_IM_IIP_PARAM_1DL*			oned[2];
	T_IM_IIP_PARAM_BLEND*		alpha;
	T_IM_IIP_PARAM_STS*			sl;
	T_IM_IIP_UTIL_PHVSZ			phvsz;

	/// Set Pointer of SDRAM parameter

	oned[0] = (T_IM_IIP_PARAM_1DL*)alp_param->param_buffer_addr;
	alp_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_1DL) );

	oned[1] = (T_IM_IIP_PARAM_1DL*)alp_param->param_buffer_addr;
	alp_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_1DL) );

	alpha = (T_IM_IIP_PARAM_BLEND*)alp_param->param_buffer_addr;
	alp_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_BLEND) );

	sl = (T_IM_IIP_PARAM_STS*)alp_param->param_buffer_addr;
	alp_param->param_buffer_addr += im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) );



	/// Set SDRAM paramter
	im_iip_set_phvsz_1d_sl( &phvsz, &cfg->dst );

	im_iip_set_1d_unit_param( oned[0], &phvsz, &cfg->src[0], cfg->pix_depth, cfg->ld_cache_select[0] );
	im_iip_set_1d_unit_param( oned[1], &phvsz, &cfg->src[1], cfg->pix_depth, cfg->ld_cache_select[1] );

	im_iip_set_alpha_unit_param( alpha, cfg );

	im_iip_set_sl_unit_param( sl, &phvsz, &cfg->dst.rect, &cfg->dst.gbl, cfg->pix_depth );

	oned[0]->PIXIDDEF.bit.IPIXID = alp_param->pixid[0];
	oned[0]->LD_TOPCNF0.bit.WAITCONF = im_iip_util_conv_portid( cfg->blend_unitid );

	oned[1]->PIXIDDEF.bit.IPIXID = alp_param->pixid[1];
	oned[1]->LD_TOPCNF0.bit.WAITCONF = im_iip_util_conv_portid_1( cfg->blend_unitid );

	alpha->BLENDTOPCNF0.bit.DATACONF_0 = im_iip_util_conv_portid( cfg->ld_unitid[0] );
	alpha->BLENDTOPCNF1.bit.DATACONF_1 = im_iip_util_conv_portid( cfg->ld_unitid[1] );
	alpha->BLENDTOPCNF0.bit.WAITCONF_0 = im_iip_util_conv_portid( cfg->sl_unitid );

	sl->BASE.PIXIDDEF.bit.OPIXID = alp_param->pixid[2];
	sl->BASE.SL_TOPCNF0.bit.DATACONF = im_iip_util_conv_portid( cfg->blend_unitid );

	alp_param->p_param_1d[0] = oned[0];
	alp_param->p_param_1d[0] = oned[1];
	alp_param->p_param_alp = alpha;
	alp_param->p_param_sl = sl;

	/// Set UNITINFTBL parameter
	im_iip_set_unitinftbl_param( &alp_param->unit_cfg_1d[0], cfg->ld_unitid[0], oned[0], 0ULL );
	im_iip_set_unitinftbl_param( &alp_param->unit_cfg_1d[1], cfg->ld_unitid[1], oned[1], 0ULL );
	im_iip_set_unitinftbl_param( &alp_param->unit_cfg_alp, cfg->blend_unitid, alpha, 0ULL );
	im_iip_set_unitinftbl_param( &alp_param->unit_cfg_sl, cfg->sl_unitid, sl, alp_param->open_param.unitid_bitmask );
}

/**
@brief		Set Parameters of Alpha Blend Utility
*/
static INT32 im_iip_util_alpha_main( T_IM_IIP_UTIL_PARAM_ALPHA* const alp_param, const T_IM_IIP_UTIL_ALPHABLEND* const cfg )
{
	INT32						retval;
	T_IM_IIP_UTIL_ALPHA_INFO	alpha_info;

	alp_param->open_param.unitid_bitmask = im_iip_util_conv_unitid_to_pldunit(cfg->ld_unitid[0])
										 | im_iip_util_conv_unitid_to_pldunit(cfg->ld_unitid[1])
										 | im_iip_util_conv_unitid_to_pldunit(cfg->blend_unitid)
										 | im_iip_util_conv_unitid_to_pldunit(cfg->sl_unitid);
	alp_param->open_param.pixid_bitmask = cfg->src_pixid[0] | cfg->src_pixid[1] | cfg->dst_pixid;
	alp_param->open_param.open_res_bitmask = E_IM_IIP_OPEN_RES_NONE;

	switch( cfg->ld_cache_select[0] ) {
		case E_IM_IIP_PARAM_CSEL_0:
			alp_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE0;
			break;
		case E_IM_IIP_PARAM_CSEL_1:
			alp_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE1;
			break;
		case E_IM_IIP_PARAM_CSEL_2:
			alp_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE2;
			break;
		default:
			return D_IM_IIP_INVALID_ARG_ERR;
	}

	switch( cfg->ld_cache_select[1] ) {
		case E_IM_IIP_PARAM_CSEL_0:
			alp_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE0;
			break;
		case E_IM_IIP_PARAM_CSEL_1:
			alp_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE1;
			break;
		case E_IM_IIP_PARAM_CSEL_2:
			alp_param->open_param.open_res_bitmask |= E_IM_IIP_OPEN_RES_CACHE2;
			break;
		default:
			return D_IM_IIP_INVALID_ARG_ERR;
	}

	alp_param->pixid[0] = im_iip_util_conv_pixid( cfg->src_pixid[0] );
	if( alp_param->pixid[0] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	alp_param->pixid[1] = im_iip_util_conv_pixid( cfg->src_pixid[1] );
	if( alp_param->pixid[1] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	alp_param->pixid[2] = im_iip_util_conv_pixid( cfg->dst_pixid );
	if( alp_param->pixid[2] == E_IM_IIP_PIXID_INVALID ) {
		return D_IM_IIP_INVALID_ARG_ERR;
	}

	retval = Im_IIP_Open_SWTRG( alp_param->open_param.unitid_bitmask, alp_param->open_param.pixid_bitmask, alp_param->open_param.open_res_bitmask, alp_param->wait_param.wait_time );
	if( retval != D_IM_IIP_OK ) {
		return retval;
	}


	while( 1 ) {
		/// Set SDRAM parameter and UNITINFTBL parameter
		im_iip_util_set_param_alpha( alp_param, cfg );

		/// Set IIP driver parameter
		alpha_info.alpha_depth = cfg->alpha_depth;
		alpha_info.alpha_subsampling = cfg->alpha_subsampling;
		im_iip_set_pixfmttbl_param( &alp_param->pixfmttbl[0], &cfg->src[0].gbl, cfg->pix_depth, &alpha_info );
		im_iip_set_pixfmttbl_param( &alp_param->pixfmttbl[1], &cfg->src[1].gbl, cfg->pix_depth, &alpha_info );
		im_iip_set_pixfmttbl_param( &alp_param->pixfmttbl[2], &cfg->dst.gbl,    cfg->pix_depth, &alpha_info );

		im_iip_set_axi_param( cfg->ld_unitid[0], &alp_param->axi_id_ld[0], &alp_param->axi_cfg_1d[0], alp_param->param_master_IF );
		im_iip_set_axi_param( cfg->ld_unitid[1], &alp_param->axi_id_ld[1], &alp_param->axi_cfg_1d[1], alp_param->param_master_IF );
		im_iip_set_axi_param( cfg->sl_unitid, &alp_param->axi_id_sl, &alp_param->axi_cfg_sl, alp_param->param_master_IF );

		/// Execute IIP
		retval = im_iip_util_exec_alpha( alp_param, cfg );
		if( retval != D_IM_IIP_OK ) {
			break;
		}

		// Unit start successful
		return retval;
	}


	// Error route
	(VOID)Im_IIP_Close_SWTRG( alp_param->open_param.unitid_bitmask, alp_param->open_param.pixid_bitmask, alp_param->open_param.open_res_bitmask );

	return retval;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
*	Resize and Rotate
*/
INT32 IM_IIP_Utility_ResizeRotate( const T_IM_IIP_UTIL_RR* const cfg0,
								   const T_IM_IIP_UTIL_RR* const cfg1,
								   const UCHAR param_master_IF,
								   const ULONG param_buffer_addr,
								   const INT32 wait_time )
{
	INT32 retval;
	INT32 retval2;
	T_IM_IIP_UTIL_PARAM_RESIZE_ROTATE rr_param[2];

#ifdef CO_PARAM_CHECK
	if( (cfg0 == NULL) ||
		 (param_buffer_addr == 0) ||
		 (im_iip_util_check_master_IF(param_master_IF) == D_IM_IIP_FALSE) ||
		 (im_iip_check_wait_time(wait_time) == D_IM_IIP_FALSE) ||
		 ((cfg0->resize_mode == E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC) && (cfg0->rotation != E_IM_IIP_UTIL_ROTATE_000))
	   ) {
		Ddim_Assertion(("I:IM_IIP_Utility_ResizeRotate INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
	if( cfg1 != NULL ) {
		if( cfg1->resize_mode == E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC ) {
			Ddim_Assertion(("I:IM_IIP_Utility_ResizeRotate INVALID_ARG_ERR\n"));
			return D_IM_IIP_INVALID_ARG_ERR;
		}
	}
#endif

	memset( &rr_param[0], '\0', sizeof(rr_param[0]) );	/* pgr0584 */
	rr_param[0].param_buffer_addr = im_iip_roundup_8( param_buffer_addr );
	rr_param[0].param_master_IF = param_master_IF;
	rr_param[0].wait_param.wait_time = wait_time;
	retval = im_iip_util_resizerotate_main( &rr_param[0], cfg0 );
	if( retval != D_IM_IIP_OK ) {
		return retval;
	}

	if( cfg1 != NULL ) {
		memset( &rr_param[1], '\0', sizeof(rr_param[1]) );
		rr_param[1].param_buffer_addr = rr_param[0].param_buffer_addr;
		rr_param[1].param_master_IF = param_master_IF;
		rr_param[1].wait_param.wait_time = wait_time;
		retval = im_iip_util_resizerotate_main( &rr_param[1], cfg1 );
		if( retval != D_IM_IIP_OK ) {
			return retval;
		}
	}

	retval = im_iip_util_wait_and_close( &rr_param[0].open_param, &rr_param[0].wait_param, cfg0->afn_unitid );

	if( cfg1 != NULL ) {
		retval2 = im_iip_util_wait_and_close( &rr_param[1].open_param, &rr_param[1].wait_param, cfg1->afn_unitid );
		if( retval == D_IM_IIP_OK ) {
			retval = retval2;
		}
	}

	return retval;
}

/**
*	Color space conversion
*/
INT32 IM_IIP_Utility_CSC( const T_IM_IIP_UTIL_CSC* const cfg0,
						  const T_IM_IIP_UTIL_CSC* const cfg1,
						  const UCHAR param_master_IF,
						  const ULONG param_buffer_addr,
						  const INT32 wait_time )
{
	INT32 						retval;
	INT32 						retval2;
	T_IM_IIP_UTIL_PARAM_CSC		csc_param[2];

#ifdef CO_PARAM_CHECK
	if( (cfg0 == NULL) ||
		 (param_buffer_addr == 0) ||
		 (im_iip_util_check_master_IF(param_master_IF) == D_IM_IIP_FALSE) ||
		 (im_iip_check_wait_time(wait_time) == D_IM_IIP_FALSE)
	   ) {
		Ddim_Assertion(("I:IM_IIP_Utility_CSC INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	memset( &csc_param[0], '\0', sizeof(csc_param[0]) );	/* pgr0584 */
	csc_param[0].param_buffer_addr = im_iip_roundup_8( param_buffer_addr );
	csc_param[0].param_master_IF = param_master_IF;
	csc_param[0].wait_param.wait_time = wait_time;
	retval = im_iip_util_csc_main( &csc_param[0], cfg0, wait_time );
	if( retval != D_IM_IIP_OK ) {
		return retval;
	}

	if( cfg1 != NULL ) {
		memset( &csc_param[1], '\0', sizeof(csc_param[1]) );
		csc_param[1].param_buffer_addr = csc_param[0].param_buffer_addr;
		csc_param[1].param_master_IF = param_master_IF;
		csc_param[1].wait_param.wait_time = wait_time;
		retval = im_iip_util_csc_main( &csc_param[1], cfg1, wait_time );
		if( retval != D_IM_IIP_OK ) {
			return retval;
		}
	}

	retval = im_iip_util_wait_and_close( &csc_param[0].open_param, &csc_param[0].wait_param, E_IM_IIP_UNIT_ID_MAX );
								// cubeic_free_unitid value is dummy

	if( cfg1 != NULL ) {
		retval2 = im_iip_util_wait_and_close( &csc_param[1].open_param, &csc_param[1].wait_param, E_IM_IIP_UNIT_ID_MAX );
									// cubeic_free_unitid value is dummy
		if( retval == D_IM_IIP_OK ) {
			retval = retval2;
		}
	}


	return retval;
}

/**
*	Look up table
*/
INT32 IM_IIP_Utility_LUT( const T_IM_IIP_UTIL_LUT* const cfg,
						  const UCHAR param_master_IF,
						  const ULONG param_buffer_addr,
						  const INT32 wait_time )
{
	INT32 retval;
	T_IM_IIP_UTIL_PARAM_LUT lut_param;

#ifdef CO_PARAM_CHECK
	if( (cfg == NULL) ||
		 (param_buffer_addr == 0) ||
		 (im_iip_util_check_master_IF(param_master_IF) == D_IM_IIP_FALSE) ||
		 (im_iip_check_wait_time(wait_time) == D_IM_IIP_FALSE)
	   ) {
		Ddim_Assertion(("I:IM_IIP_Utility_LUT INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	memset( &lut_param, '\0', sizeof(lut_param) );
	lut_param.param_buffer_addr = im_iip_roundup_8( param_buffer_addr );
	lut_param.param_master_IF = param_master_IF;
	lut_param.wait_param.wait_time = wait_time;
	retval = im_iip_util_lut_main( &lut_param, cfg );
	if( retval != D_IM_IIP_OK ) {
		return retval;
	}

	retval = im_iip_util_wait_and_close( &lut_param.open_param, &lut_param.wait_param, E_IM_IIP_UNIT_ID_MAX );
								// cubeic_free_unitid value is dummy

// ### REMOVE_RELEASE BEGIN
#if 0
//#ifdef CO_PT_ENABLE
	{
		static BYTE unitdbg_buf[0x10000];
		extern VOID CT_Im_IIP_Print_Unitcfg_Byid( const E_IM_IIP_UNIT_ID unitid );
		extern VOID CT_Im_IIP_Print_Pixfmttbl_Detail( const E_IM_IIP_PIXID pixid );
		Palladium_Set_Out_Localstack( (ULONG)lut_param.p_param_1d, sizeof(*lut_param.p_param_1d) );
		Palladium_Set_Out_Localstack( (ULONG)lut_param.p_param_lut, sizeof(*lut_param.p_param_lut) );
		Palladium_Set_Out_Localstack( (ULONG)lut_param.p_param_sl, sizeof(*lut_param.p_param_sl) );

		CT_Im_IIP_Print_Unitcfg_Byid( cfg->ld_unitid );
		CT_Im_IIP_Print_Pixfmttbl_Detail( 1 );
		CT_Im_IIP_Print_Unitcfg_Byid( cfg->lut_unitid );
		CT_Im_IIP_Print_Unitcfg_Byid( cfg->sl_unitid );
		CT_Im_IIP_Print_Pixfmttbl_Detail( 2 );

		Im_IIP_Start_DumpUnitReg( cfg->ld_unitid );
		memcpy( unitdbg_buf, (void*)gIM_IIP_UnitDebugReg, 0x10000 );
		DDIM_User_L1l2cache_Clean_Addr( (UINT32)unitdbg_buf, 0x10000 );
		Palladium_Set_Out_Localstack( (ULONG)unitdbg_buf, 0x10000 );

		Im_IIP_Start_DumpUnitReg( cfg->lut_unitid );
		memcpy( unitdbg_buf, (void*)gIM_IIP_UnitDebugReg, 0x10000 );
		DDIM_User_L1l2cache_Clean_Addr( (UINT32)unitdbg_buf, 0x10000 );
		Palladium_Set_Out_Localstack( (ULONG)unitdbg_buf, 0x10000 );

		Im_IIP_Start_DumpUnitReg( cfg->sl_unitid );
		memcpy( unitdbg_buf, (void*)gIM_IIP_UnitDebugReg, 0x10000 );
		DDIM_User_L1l2cache_Clean_Addr( (UINT32)unitdbg_buf, 0x10000 );
		Palladium_Set_Out_Localstack( (ULONG)unitdbg_buf, 0x10000 );

		Im_IIP_Stop_DumpUnitReg();
	}
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	return retval;
}

/**
*	Alpha blend
*/
INT32 IM_IIP_Utility_AlphaBlend( const T_IM_IIP_UTIL_ALPHABLEND* const cfg,
								 const UCHAR param_master_IF,
								 const ULONG param_buffer_addr,
								 const INT32 wait_time )
{
	INT32 						retval;
	T_IM_IIP_UTIL_PARAM_ALPHA	alp_param;

#ifdef CO_PARAM_CHECK
	if( (cfg == NULL) ||
		 (param_buffer_addr == 0) ||
		 (im_iip_util_check_master_IF(param_master_IF) == D_IM_IIP_FALSE) ||
		 (im_iip_check_wait_time(wait_time) == D_IM_IIP_FALSE)
	   ) {
		Ddim_Assertion(("I:IM_IIP_Utility_AlphaBlend INVALID_ARG_ERR\n"));
		return D_IM_IIP_INVALID_ARG_ERR;
	}
#endif

	memset( &alp_param, '\0', sizeof(alp_param) );
	alp_param.param_buffer_addr = im_iip_roundup_8( param_buffer_addr );
	alp_param.param_master_IF = param_master_IF;
	alp_param.wait_param.wait_time = wait_time;
	retval = im_iip_util_alpha_main( &alp_param, cfg );
	if( retval != D_IM_IIP_OK ) {
		return retval;
	}

	retval = im_iip_util_wait_and_close( &alp_param.open_param, &alp_param.wait_param, E_IM_IIP_UNIT_ID_MAX );
								// cubeic_free_unitid value is dummy

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
	{
		static BYTE unitdbg_buf[0x10000];
		extern VOID CT_Im_IIP_Print_Unitcfg_Byid( const E_IM_IIP_UNIT_ID unitid );
		Palladium_Set_Out_Localstack( (ULONG)alp_param.p_param_1d[0], sizeof(*alp_param.p_param_1d[0]) );
		Palladium_Set_Out_Localstack( (ULONG)alp_param.p_param_1d[1], sizeof(*alp_param.p_param_1d[1]) );
		Palladium_Set_Out_Localstack( (ULONG)alp_param.p_param_alp, sizeof(*alp_param.p_param_alp) );
		Palladium_Set_Out_Localstack( (ULONG)alp_param.p_param_sl, sizeof(*alp_param.p_param_sl) );

		CT_Im_IIP_Print_Unitcfg_Byid( cfg->ld_unitid[0] );
		CT_Im_IIP_Print_Unitcfg_Byid( cfg->ld_unitid[1] );
		CT_Im_IIP_Print_Unitcfg_Byid( cfg->blend_unitid );
		CT_Im_IIP_Print_Unitcfg_Byid( cfg->sl_unitid );

		Im_IIP_Start_DumpUnitReg( cfg->ld_unitid[0] );
		memcpy( unitdbg_buf, (void*)gIM_IIP_UnitDebugReg, 0x10000 );
		DDIM_User_L1l2cache_Clean_Addr( (UINT32)unitdbg_buf, 0x10000 );
		Palladium_Set_Out_Localstack( (ULONG)unitdbg_buf, 0x10000 );

		Im_IIP_Start_DumpUnitReg( cfg->ld_unitid[1] );
		memcpy( unitdbg_buf, (void*)gIM_IIP_UnitDebugReg, 0x10000 );
		DDIM_User_L1l2cache_Clean_Addr( (UINT32)unitdbg_buf, 0x10000 );
		Palladium_Set_Out_Localstack( (ULONG)unitdbg_buf, 0x10000 );

		Im_IIP_Start_DumpUnitReg( cfg->blend_unitid );
		memcpy( unitdbg_buf, (void*)gIM_IIP_UnitDebugReg, 0x10000 );
		DDIM_User_L1l2cache_Clean_Addr( (UINT32)unitdbg_buf, 0x10000 );
		Palladium_Set_Out_Localstack( (ULONG)unitdbg_buf, 0x10000 );

		Im_IIP_Start_DumpUnitReg( cfg->sl_unitid );
		memcpy( unitdbg_buf, (void*)gIM_IIP_UnitDebugReg, 0x10000 );
		DDIM_User_L1l2cache_Clean_Addr( (UINT32)unitdbg_buf, 0x10000 );
		Palladium_Set_Out_Localstack( (ULONG)unitdbg_buf, 0x10000 );

		Im_IIP_Stop_DumpUnitReg();
	}
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	return retval;
}

#ifdef CO_IM_IIP_UTIL_DEBUG
VOID IM_IIP_Utility_PrintParamBufferBytes( VOID )
{
	DDIM_User_Printf( "IM_IIP_Utility_ResizeRotate() param_buffer_addr's area bytes: %u\n",
		  im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_AFN) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) )
		);
	DDIM_User_Printf( "IM_IIP_Utility_CSC() param_buffer_addr's area bytes: %u\n",
		im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_1DL) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_CSC) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) )
		);

	DDIM_User_Printf( "IM_IIP_Utility_LUT() param_buffer_addr's area bytes: %u\n",
		im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_1DL) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_LUT) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) )
		);

	DDIM_User_Printf( "IM_IIP_Utility_AlphaBlend() param_buffer_addr's area bytes: %u\n",
		im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_1DL) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_1DL) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_BLEND) )
		+ im_iip_roundup_8( sizeof(T_IM_IIP_PARAM_STS) )
		);
}
#endif //CO_IM_IIP_UTIL_DEBUG

#endif	// CO_IM_IIP_ENABLE_UTILITY

