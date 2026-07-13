/**
 * @file		im_mxic.c
 * @brief		MXIC driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "jdsmxic.h"
#include "im_mxic.h"
#include "dd_top.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define Im_MXIC_Dsb()	Dd_ARM_Dsb_Pou()

#ifdef CO_ACT_HCLOCK
#define CO_ACT_MXIC_HCLOCK
#endif //CO_ACT_HCLOCK

#ifdef CO_ACT_PCLOCK
#define CO_ACT_MXIC_PCLOCK
#endif //CO_ACT_PCLOCK

// Mask bit to identify kind of master.
#define D_IM_MXIC_MASTER_TYPE_CHECK_MASK	(0x80)
// Write channel identification value.
#define D_IM_MXIC_MASTER_TYPE_WRITE			(D_IM_MXIC_W_RESERVE)
// Read channel identification value.
#define D_IM_MXIC_MASTER_TYPE_READ			(D_IM_MXIC_R_RESERVE)
// Mask bit to erase bit flag of read channel.
#define D_IM_MXIC_READ_MASTER_FLG_MASK		(0x7F)

// TMIF interrupt flag bit
#define D_IM_MXIC_TMIF_BIT_TCF				(0x00000001)	// TMIF.TCF bit
#define D_IM_MXIC_TMIF_BIT_TMF				(0x00000002)	// TMIF.TMF bit
#define D_IM_MXIC_TMIF_BIT_TDEWF			(0x00000004)	// TMIF.TDEWF bit
#define D_IM_MXIC_TMIF_BIT_TDERF			(0x00000008)	// TMIF.TDERF bit
#define D_IM_MXIC_TMIF_BIT_TSAWF			(0x00000010)	// TMIF.TSAWF bit

// TCKENP.TCKENP_1/2/3/4 bit flag
#define D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0	(0x01)			// Slave-x port-0 arbiter block
#define D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1	(0x02)			// Slave-x port-1 arbiter block
#define D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2	(0x04)			// Slave-x port-2 arbiter block
#define D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3	(0x08)			// Slave-x port-3 arbiter block

// TGCKEN.TGCKSEN bit flag
#define D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_0	(0x01)			// Group-0 arbiter block
#define D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_1	(0x02)			// Group-1 arbiter block
#define D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_2	(0x04)			// Group-2 arbiter block
#define D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_3	(0x08)			// Group-3 arbiter block
#define D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_4	(0x10)			// Group-4 arbiter block
#define D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_5	(0x20)			// Group-5 arbiter block
#define D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_6	(0x40)			// Group-6 arbiter block
#define D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_7	(0x80)			// Group-7 arbiter block

// TLVL*.LML* bit flag
#define D_IM_MXIC_TLVL_LML_SLOT0			(0x00000001)	// Slot0 mask bit
#define D_IM_MXIC_TLVL_LML_SLOT1			(0x00000002)	// Slot1 mask bit
#define D_IM_MXIC_TLVL_LML_SLOT2			(0x00000004)	// Slot2 mask bit
#define D_IM_MXIC_TLVL_LML_SLOT3			(0x00000008)	// Slot3 mask bit

// TRMC*.TRMFT* bit flag
#define D_IM_MXIC_TRMC_TRMFT_A_FACTOR		(0x00000001)	// Mask factor A group
#define D_IM_MXIC_TRMC_TRMFT_A_TARGET		(0x00000002)	// Mask target A group
#define D_IM_MXIC_TRMC_TRMFT_B_FACTOR		(0x00000004)	// Mask factor B group
#define D_IM_MXIC_TRMC_TRMFT_B_TARGET		(0x00000008)	// Mask target B group
#define D_IM_MXIC_TRMC_TRMFT_SELECT			(0x00000010)	// Mask state select

// TRMC*.TISULIM* bit value
#define D_IM_MXIC_TRMC_TISULIM_MIN_VAL		(0)				// Minimum value of transaction
#define D_IM_MXIC_TRMC_TISULIM_MAX_VAL		(63)			// Mzximum value of transaction

// Definition for acceptance capability
#define D_IM_MXIC_ACC_CAPA_MIN_VAL			(1)				// Minimum value of acceptance capability
#define D_IM_MXIC_ACC_CAPA_MAX_VAL			(8)				// Maximum value of acceptance capability

// Number of arbiters in MXIC of the full spec composition.
#define D_IM_MXIC_FULL_SPEC_ARBITER_NUM		(4)
// Number of slots for each arbiter in MXIC of the full spec composition.
#define D_IM_MXIC_FULL_SPEC_SLOT_NUM		(4)
// Number of masters for each slot in MXIC of the full spec composition.
#define D_IM_MXIC_FULL_SPEC_MASTER_NUM		(16)
// Number of masters for each port in MXIC of the full spec composition.
#define D_IM_MXIC_FULL_SPEC_PORT_NUM		(4)

// Separated position of the master output port configuration register.
#define D_IM_MXIC_OUT_PORT_SEP_NUM	(64)


// Definition of the monitored range checking.
#define D_IM_MXIC_CHECK_MASTER_W_MIN		(0x001)
#define D_IM_MXIC_CHECK_MASTER_W_MAX		(0x043)
#define D_IM_MXIC_CHECK_MASTER_R_MIN		(0x081)
#define D_IM_MXIC_CHECK_MASTER_R_MAX		(0x0C3)
#define D_IM_MXIC_CHECK_SLAVE_W_MIN			(0x104)
#define D_IM_MXIC_CHECK_SLAVE_W_MAX			(0x10E)
#define D_IM_MXIC_CHECK_SLAVE_R_MIN			(0x184)
#define D_IM_MXIC_CHECK_SLAVE_R_MAX			(0x18E)

// Master LO/HI bit mask
#define D_IM_MXIC_MASTER_LO_MASK			(0x00000000FFFFFFFF)		// Master LO bit mask
#define D_IM_MXIC_MASTER_HI_MASK			(0xFFFFFFFF00000000)		// Master HI bit mask

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Operation type (Set/Get)
typedef enum {
	E_IM_MXIC_OPERATION_TYPE_SET,			// Set operation
	E_IM_MXIC_OPERATION_TYPE_GET			// Get operation
} E_IM_MXIC_OPERATION_TYPE;

// Master mask operation type (Factor/Target)
typedef enum {
	E_IM_MXIC_MASTER_MASK_OPERATION_FACTOR,	// Factor
	E_IM_MXIC_MASTER_MASK_OPERATION_TARGET	// Target
} E_IM_MXIC_MASTER_MASK_OPERATION;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Master mask factor and target configration
typedef struct {
	E_IM_MXIC_OPERATION_TYPE		kind_set_get;		// Operation type (Set/Get)
	E_IM_MXIC_MASTER_MASK_OPERATION	kind_factor_target;	// Master mask operation type (Factor/Target)
	UCHAR							master;				// Master number.
	E_IM_MXIC_MASK_GROUP			group;				// Mask group.
	UCHAR							enable;				// Setting of whether to make a master into a mask factor(target).
} T_IM_MXIC_MASTER_MASK_FACTOR_TARGET;

// Spin_lock.
typedef struct {
	ULONG							lock;
	ULONG							dummy[ ( 64 / 4 ) - 1 ];
} T_IM_MXIC_SPIN_LOCK_VAR;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_MXIC_HCLOCK
static volatile UCHAR gIM_MXIC1_HCLK_Counter	= 0;
static volatile UCHAR gIM_MXIC2_HCLK_Counter	= 0;
static volatile UCHAR gIM_MXIC3_HCLK_Counter	= 0;
#endif

#ifdef CO_ACT_MXIC_PCLOCK
static volatile UCHAR gIM_MXIC0_PCLK_Counter	= 0;
static volatile UCHAR gIM_MXIC1_PCLK_Counter	= 0;
static volatile UCHAR gIM_MXIC2_PCLK_Counter	= 0;
static volatile UCHAR gIM_MXIC3_PCLK_Counter	= 0;
static volatile UCHAR gIM_MXIC4_PCLK_Counter	= 0;
static volatile UCHAR gIM_MXIC5_PCLK_Counter	= 0;
static volatile UCHAR gIM_MXIC6_PCLK_Counter	= 0;
#endif

// Running state of transfer/access monitor with limit.
static volatile UCHAR gIM_MXIC_Trans_Mon_Limit_End_State = D_IM_MXIC_MONITOR_OPERATION_END;

// Callback function pointer for transfer/access monitor end by limit.
static volatile VP_CALLBACK gIM_MXIC_Acs_Trans_Mon_Limit_End_CallBack = NULL;
// Callback function pointer for decode error.
static volatile IM_MXIC_DEC_ERR_CALLBACK gIM_MXIC_Dec_Err_Callback[E_IM_MXIC_UNIT_MAX] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
// Callback function pointer for detection of memory access.
static volatile VP_CALLBACK gIM_MXIC_Memory_Access_Detect_CallBack[E_IM_MXIC_UNIT_MAX] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// TAEN setting user counter.
static volatile INT32 gIM_MXIC_Taen_Counter[E_IM_MXIC_UNIT_MAX] = { 0, 0, 0, 0, 0, 0, 0 };	// 0 ~ (Maximum of INT32)

// Spin_lock.
static volatile T_IM_MXIC_SPIN_LOCK_VAR gIM_MXIC_Spin_Lock[E_IM_MXIC_UNIT_MAX] __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Shift operation for creating setting TPORTMW, TPORTMR
#define im_mxic_shift_tportmx(x, n)	((UINT64)(x & 0x01) << (n & 0x3F))

// Output port of the master setting acquisition from TPORTMW, TPORTMR
#define im_mxic_master_out_port_tportmx(x, n)	(((x & ((UINT64)0x01 << (n & 0x3F))) != 0) ? E_IM_MXIC_PORT_GR_1 : E_IM_MXIC_PORT_GR_0 )


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* MXIC PCLK change to ON.
 */
VOID im_mxic_on_pclk( E_IM_MXIC_UNIT unit )
{
#ifdef CO_ACT_MXIC_PCLOCK
	switch ( unit ) {
		case E_IM_MXIC_UNIT_0:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC0_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP0_BIT );
			break;
		case E_IM_MXIC_UNIT_1:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC1_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP1_BIT );
			break;
		case E_IM_MXIC_UNIT_2:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC2_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP2_BIT );
			break;
		case E_IM_MXIC_UNIT_3:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC3_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP3_BIT );
			break;
		case E_IM_MXIC_UNIT_4:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC4_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP4_BIT );
			break;
		case E_IM_MXIC_UNIT_5:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC5_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP5_BIT );
			break;
		case E_IM_MXIC_UNIT_6:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC6_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP6_BIT );
			break;
		default:
			break;
	}
#endif	// CO_ACT_MXIC_PCLOCK
}

/* MXIC PCLK change to OFF.
 */
VOID im_mxic_off_pclk( E_IM_MXIC_UNIT unit )
{
#ifdef CO_ACT_MXIC_PCLOCK
	switch ( unit ) {
		case E_IM_MXIC_UNIT_0:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC0_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), D_DD_TOP_MICAP0_BIT );
			break;
		case E_IM_MXIC_UNIT_1:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC1_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), D_DD_TOP_MICAP1_BIT );
			break;
		case E_IM_MXIC_UNIT_2:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC2_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), D_DD_TOP_MICAP2_BIT );
			break;
		case E_IM_MXIC_UNIT_3:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC3_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), D_DD_TOP_MICAP3_BIT );
			break;
		case E_IM_MXIC_UNIT_4:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC4_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), D_DD_TOP_MICAP4_BIT );
			break;
		case E_IM_MXIC_UNIT_5:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC5_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), D_DD_TOP_MICAP5_BIT );
			break;
		case E_IM_MXIC_UNIT_6:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC6_PCLK_Counter, &Dd_Top_Get_CLKSTOP5(), D_DD_TOP_MICAP6_BIT );
			break;
		default:
			break;
	}
#endif	// CO_ACT_MXIC_PCLOCK
}

/* MXIC HCLK change to ON.
 */
VOID im_mxic_on_hclk( E_IM_MXIC_UNIT unit )
{
#ifdef CO_ACT_MXIC_HCLOCK
	switch ( unit ) {
		case E_IM_MXIC_UNIT_1:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC1_HCLK_Counter, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_MICAH1_BIT );
			break;
		case E_IM_MXIC_UNIT_2:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC2_HCLK_Counter, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_MICAH2_BIT );
			break;
		case E_IM_MXIC_UNIT_3:
			Dd_Top_Start_Clock( (UCHAR*)&gIM_MXIC3_HCLK_Counter, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_MICAH3_BIT );
			break;
		default:
			break;
	}
#endif	// CO_ACT_MXIC_HCLOCK
}

/* MXIC HCLK change to OFF.
 */
VOID im_mxic_off_hclk( E_IM_MXIC_UNIT unit )
{
#ifdef CO_ACT_MXIC_HCLOCK
	switch ( unit ) {
		case E_IM_MXIC_UNIT_1:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC1_HCLK_Counter, &Dd_Top_Get_CLKSTOP6(), D_DD_TOP_MICAH1_BIT );
			break;
		case E_IM_MXIC_UNIT_2:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC2_HCLK_Counter, &Dd_Top_Get_CLKSTOP6(), D_DD_TOP_MICAH2_BIT );
			break;
		case E_IM_MXIC_UNIT_3:
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_MXIC3_HCLK_Counter, &Dd_Top_Get_CLKSTOP6(), D_DD_TOP_MICAH3_BIT );
			break;
		default:
			break;
	}
#endif	// CO_ACT_MXIC_HCLOCK
}

#ifdef CO_PARAM_CHECK
/**
Check unit parameter.
@param[in]		unit								MXIC unit number.<br>
@retval			D_DDIM_OK							Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail	Parameter error.
*/
static INT32 im_mxic_check_unit( E_IM_MXIC_UNIT unit )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( unit >= E_IM_MXIC_UNIT_MAX ) {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_unit(): Parameter is illegal. unitt=%d\n", unit));
	}

	return result;
}

/**
Check slot parameter.
@param[in]		wr_arbiter				Write or Read Arbiter.<br>
@param[in]		arbiter					Arbiter.<br>
@param[in]		port					Port.<br>
@param[in]		slot					Slot.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_slot( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT slot )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( ( wr_arbiter >= E_IM_MXIC_WR_ARBITER_MAX ) || ( arbiter >= E_IM_MXIC_SPEC_ARBITER_MAX ) || ( port >= E_IM_MXIC_PORT_MAX ) || ( slot >= E_IM_MXIC_SLOT_MAX ) ) {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_slot(): Parameter is illegal. wr_arbiter=%d, arbiter=%d, port=%d, slot=%d\n", wr_arbiter, arbiter, port, slot));
	}
	return result;
}

/**
Check port specifies parameter.
@param[in]		wr_arbiter				Write or Read Arbiter.<br>
@param[in]		arbiter					Arbiter.<br>
@param[in]		port					Port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_port( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( ( wr_arbiter >= E_IM_MXIC_WR_ARBITER_MAX ) || ( arbiter >= E_IM_MXIC_SPEC_ARBITER_MAX ) || ( port >= E_IM_MXIC_PORT_MAX ) ) {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_port(): Parameter is illegal. wr_arbiter=%d, arbiter=%d, port=%d\n", wr_arbiter, arbiter, port));
	}

	return result;
}

/**
Check level specifies parameter.
@param[in]		wr_arbiter				Write or Read Arbiter.<br>
@param[in]		arbiter					Arbiter.<br>
@param[in]		level					Level.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_level( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_LEVEL level )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( ( wr_arbiter >= E_IM_MXIC_WR_ARBITER_MAX ) || ( arbiter >= E_IM_MXIC_SPEC_ARBITER_MAX ) || ( level >= E_IM_MXIC_LEVEL_MAX ) ) {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_level(): Parameter is illegal. wr_arbiter=%d, arbiter=%d, level=%d\n", wr_arbiter, arbiter, level));
	}

	return result;
}

/**
Check arbiter specifies parameter.
@param[in]		wr_arbiter				Write or Read Arbiter.<br>
@param[in]		arbiter					Arbiter.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_arbiter( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( ( wr_arbiter >= E_IM_MXIC_WR_ARBITER_MAX ) || ( arbiter >= E_IM_MXIC_SPEC_ARBITER_MAX ) ) {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_arbiter(): Parameter is illegal. wr_arbiter=%d, arbiter=%d\n", wr_arbiter, arbiter));
	}

	return result;
}

/**
Check port specifies parameter. (gropu)
@param[in]		wr_arbiter				Write or Read Arbiter.<br>
@param[in]		arbiter					Arbiter.<br>
@param[in]		port					Port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_port_group( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER_GR arbiter, E_IM_MXIC_PORT_GR port )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( ( wr_arbiter >= E_IM_MXIC_WR_ARBITER_MAX ) || ( arbiter >= E_IM_MXIC_SPEC_ARBITER_GR_MAX ) || ( port >= E_IM_MXIC_PORT_GR_MAX ) ) {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_port_group(): Parameter is illegal. wr_arbiter=%d, arbiter=%d, port=%d\n", wr_arbiter, arbiter, port));
	}

	return result;
}

/**
Check level control block specifies parameter.
@param[in]		wr_arbiter				Write or Read Arbiter.<br>
@param[in]		arbiter					Arbiter.<br>
@param[in]		port					Port.<br>
@param[in]		level_number			Level control block.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_level_ctrl_block( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT_LEVEL_CTRL level_number )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( ( wr_arbiter >= E_IM_MXIC_WR_ARBITER_MAX ) || ( arbiter >= E_IM_MXIC_SPEC_ARBITER_MAX ) || ( port >= E_IM_MXIC_PORT_MAX ) || ( level_number >= E_IM_MXIC_SLOT_LEVEL_CTRL_MAX ) ) {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_level_ctrl_block(): Parameter is illegal. wr_arbiter=%d, arbiter=%d, port=%d, level_number=%d\n", wr_arbiter, arbiter, port, level_number));
	}

	return result;
}

/**
Check master number for monitor function.
@param[in]	master					Channel number.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_master_number_for_monitor( UINT32 master )
{
	// Check whether the specified number is a port number or "no use" number.
	if ( master == D_IM_MXIC_TARGET_NOTHING ) {
		return D_DDIM_OK;
	}
	if ( ( master >= D_IM_MXIC_CHECK_MASTER_W_MIN ) && ( master <= D_IM_MXIC_CHECK_MASTER_W_MAX ) ) {
		return D_DDIM_OK;
	}
	if ( ( master >= D_IM_MXIC_CHECK_MASTER_R_MIN ) && ( master <= D_IM_MXIC_CHECK_MASTER_R_MAX ) ) {
		return D_DDIM_OK;
	}
	if ( ( master >= D_IM_MXIC_CHECK_SLAVE_W_MIN ) && ( master <= D_IM_MXIC_CHECK_SLAVE_W_MAX ) ) {
		return D_DDIM_OK;
	}
	if ( ( master >= D_IM_MXIC_CHECK_SLAVE_R_MIN ) && ( master <= D_IM_MXIC_CHECK_SLAVE_R_MAX ) ) {
		return D_DDIM_OK;
	}

	return D_IM_MXIC_INPUT_PARAM_ERROR;
}

/**
Check master number and type.
@param[in]		master_type				Write master: D_IM_MXIC_MASTER_TYPE_WRITE
										Read master : D_IM_MXIC_MASTER_TYPE_READ 
@param[in]		master					Master number.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_rw_master_number( UCHAR master_type, UCHAR master )
{
	INT32 result;

	// Check master type
	if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == master_type ) {
		// Check-OK
		result = D_DDIM_OK;
	}
	else {
		// Master type is illegal.
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_rw_master_number(): master_type is illegal.\n"));
	}

	return result;
}

/**
Check value of acceptance capability.
@param[in]		capability				Acceptance capability.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_acceptance_capability( UCHAR capability )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( ( capability < D_IM_MXIC_ACC_CAPA_MIN_VAL ) || ( capability > D_IM_MXIC_ACC_CAPA_MAX_VAL ) ) {
		result = D_IM_MXIC_INPUT_PARAM_ERROR;
		Ddim_Print(("E:im_mxic_check_acceptance_capability(): capability is illegal. capability=%d\n", capability));
	}

	return result;
}

/**
Check parameter of Im_MXIC_Set_Master_W_Arbiter function.
@param[in]		arbiter					Target write arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_set_master_w_arbiter_param( E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT const* const assign )
{
	INT32 port_pos;
	INT32 port_pos2;
	INT32 slot_pos;
	INT32 slot_pos2;

	if ( arbiter >= E_IM_MXIC_W_ARBITER_MAX ) {
		// arbiter parameter value is illegal.
		Ddim_Print(("E:im_mxic_check_set_master_w_arbiter_param(): arbiter is illegal.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}

	if ( assign == NULL ) {
		// assign parameter is NULL.
		Ddim_Print(("E:im_mxic_check_set_master_w_arbiter_param(): assign is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}

	for ( port_pos = E_IM_MXIC_PORT_0; port_pos < E_IM_MXIC_PORT_MAX; port_pos++ ) {

		for ( slot_pos = 0; slot_pos < D_IM_MXIC_SLOT_SIZE_8; slot_pos++ ) {

			if ( (im_mxic_check_rw_master_number( D_IM_MXIC_MASTER_TYPE_WRITE, assign->port[port_pos].slot0[slot_pos] ) != D_DDIM_OK ) ||
				 (im_mxic_check_rw_master_number( D_IM_MXIC_MASTER_TYPE_WRITE, assign->port[port_pos].slot1[slot_pos] ) != D_DDIM_OK ) ||
				 (im_mxic_check_rw_master_number( D_IM_MXIC_MASTER_TYPE_WRITE, assign->port[port_pos].slot2[slot_pos] ) != D_DDIM_OK ) ||
				 (im_mxic_check_rw_master_number( D_IM_MXIC_MASTER_TYPE_WRITE, assign->port[port_pos].slot3[slot_pos] ) != D_DDIM_OK ) ) {
				// Master number is illegal.
				Ddim_Print(("E:im_mxic_check_set_master_w_arbiter_param(): Write master number is illegal. arbiter=%u port=%u slot=%u\n", arbiter, port_pos, slot_pos));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}

			if ( assign->port[port_pos].slot0[slot_pos] != D_IM_MXIC_W_RESERVE ) {

				for ( port_pos2 = E_IM_MXIC_PORT_0; port_pos2 < E_IM_MXIC_PORT_MAX; port_pos2++ ) {

					for ( slot_pos2 = 0; slot_pos2 < D_IM_MXIC_SLOT_SIZE_8; slot_pos2++ ) {

						if ( ( ( port_pos != port_pos2 ) && ( assign->port[port_pos].slot0[slot_pos] == assign->port[port_pos2].slot0[slot_pos2] ) ) ||
							 ( assign->port[port_pos].slot0[slot_pos] == assign->port[port_pos2].slot1[slot_pos2] ) ||
							 ( assign->port[port_pos].slot0[slot_pos] == assign->port[port_pos2].slot2[slot_pos2] ) ||
							 ( assign->port[port_pos].slot0[slot_pos] == assign->port[port_pos2].slot3[slot_pos2] ) ) {
							// Master number is illegal.
							Ddim_Print(("E:im_mxic_check_set_master_w_arbiter_param(): Write master number is overlap. port[%d].slot0[%d]=0x%02x\n",
										port_pos, slot_pos, assign->port[port_pos].slot0[slot_pos]));
							return D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
				}
			}

			if ( assign->port[port_pos].slot1[slot_pos] != D_IM_MXIC_W_RESERVE ) {

				for ( port_pos2 = E_IM_MXIC_PORT_0; port_pos2 < E_IM_MXIC_PORT_MAX; port_pos2++ ) {

					for ( slot_pos2 = 0; slot_pos2 < D_IM_MXIC_SLOT_SIZE_8; slot_pos2++ ) {

						if ( ( ( port_pos != port_pos2 ) && ( assign->port[port_pos].slot1[slot_pos] == assign->port[port_pos2].slot1[slot_pos2] ) ) ||
							 ( assign->port[port_pos].slot1[slot_pos] == assign->port[port_pos].slot2[slot_pos2] ) ||
							 ( assign->port[port_pos].slot1[slot_pos] == assign->port[port_pos].slot3[slot_pos2] ) ) {
							// Master number is illegal.
							Ddim_Print(("E:im_mxic_check_set_master_w_arbiter_param(): Write master number is overlap. port[%d].slot1[%d]=0x%02x\n",
										port_pos, slot_pos,  assign->port[port_pos].slot1[slot_pos]));
							return D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
				}
			}

			if ( assign->port[port_pos].slot2[slot_pos] != D_IM_MXIC_W_RESERVE ) {

				for ( port_pos2 = E_IM_MXIC_PORT_0; port_pos2 < E_IM_MXIC_PORT_MAX; port_pos2++ ) {

					for ( slot_pos2 = 0; slot_pos2 < D_IM_MXIC_SLOT_SIZE_8; slot_pos2++ ) {

						if ( ( ( port_pos != port_pos2 ) && ( assign->port[port_pos].slot2[slot_pos] == assign->port[port_pos2].slot2[slot_pos2] ) ) ||
							 ( assign->port[port_pos].slot2[slot_pos] == assign->port[port_pos].slot3[slot_pos2] ) ) {
							// Master number is illegal.
							Ddim_Print(("E:im_mxic_check_set_master_w_arbiter_param(): Write master number is overlap. port[%d].slot2[%d]=0x%02x\n",
										port_pos, slot_pos,  assign->port[port_pos].slot2[slot_pos]));
							return D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
				}
			}
		}
	}

	return D_DDIM_OK;
}

/**
Check parameter of Im_MXIC_Get_Master_W_Arbiter function.
@param[in]		arbiter					Target write arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_get_master_w_arbiter_param(E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT* const assign)
{
	if ( arbiter >= E_IM_MXIC_W_ARBITER_MAX ) {
		// arbiter parameter value is illegal.
		Ddim_Print(("E:im_mxic_check_get_master_w_arbiter_param(): arbiter is illegal.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( assign == NULL ) {
		// assign parameter is NULL.
		Ddim_Print(("E:im_mxic_check_get_master_w_arbiter_param(): assign is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}

/**
Check parameter of Im_MXIC_Set_Master_R_Arbiter function.
@param[in]		arbiter					Target read arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_set_master_r_arbiter_param( E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT const* const assign )
{
	INT32 port_pos;
	INT32 port_pos2;
	INT32 slot_pos;
	INT32 slot_pos2;

	if ( arbiter >= E_IM_MXIC_R_ARBITER_MAX ) {
		// arbiter parameter value is illegal.
		Ddim_Print(("E:im_mxic_check_set_master_r_arbiter_param(): arbiter is illegal.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}

	if ( assign == NULL ) {
		// assign parameter is NULL.
		Ddim_Print(("E:im_mxic_check_set_master_r_arbiter_param(): assign is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}

	for ( port_pos = E_IM_MXIC_PORT_0; port_pos < E_IM_MXIC_PORT_MAX; port_pos++ ) {

		for ( slot_pos = 0; slot_pos < D_IM_MXIC_SLOT_SIZE_8; slot_pos++ ) {

			if ( ( im_mxic_check_rw_master_number( D_IM_MXIC_MASTER_TYPE_READ, assign->port[port_pos].slot0[slot_pos] ) != D_DDIM_OK ) ||
				 ( im_mxic_check_rw_master_number( D_IM_MXIC_MASTER_TYPE_READ, assign->port[port_pos].slot1[slot_pos] ) != D_DDIM_OK ) ||
				 ( im_mxic_check_rw_master_number( D_IM_MXIC_MASTER_TYPE_READ, assign->port[port_pos].slot2[slot_pos] ) != D_DDIM_OK ) ||
				 ( im_mxic_check_rw_master_number( D_IM_MXIC_MASTER_TYPE_READ, assign->port[port_pos].slot3[slot_pos] ) != D_DDIM_OK ) ) {
				// Master number is illegal.
				Ddim_Print(("E:im_mxic_check_set_master_r_arbiter_param(): Read master number is illegal. arbiter=%u port=%u slot=%u\n", arbiter, port_pos, slot_pos));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}

			if ( assign->port[port_pos].slot0[slot_pos] != D_IM_MXIC_R_RESERVE ) {

				for ( port_pos2 = E_IM_MXIC_PORT_0; port_pos2 < E_IM_MXIC_PORT_MAX; port_pos2++ ) {

					for ( slot_pos2 = 0; slot_pos2 < D_IM_MXIC_SLOT_SIZE_8; slot_pos2++ ) {

						if ( ( ( port_pos != port_pos2 ) && ( assign->port[port_pos].slot0[slot_pos] == assign->port[port_pos2].slot0[slot_pos2] ) ) ||
							 ( assign->port[port_pos].slot0[slot_pos] == assign->port[port_pos].slot1[slot_pos2] ) ||
							 ( assign->port[port_pos].slot0[slot_pos] == assign->port[port_pos].slot2[slot_pos2] ) ||
							 ( assign->port[port_pos].slot0[slot_pos] == assign->port[port_pos].slot3[slot_pos2] ) ) {
							// Master number is illegal.
							Ddim_Print(("E:im_mxic_check_set_master_r_arbiter_param(): Read master number is overlap. port[%d].slot0[%d] = 0x%02x\n",
										port_pos, slot_pos, assign->port[port_pos].slot0[slot_pos]));
							return D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
				}
			}

			if ( assign->port[port_pos].slot1[slot_pos] != D_IM_MXIC_R_RESERVE ) {

				for ( port_pos2 = E_IM_MXIC_PORT_0; port_pos2 < E_IM_MXIC_PORT_MAX; port_pos2++ ) {

					for ( slot_pos2 = 0; slot_pos2 < D_IM_MXIC_SLOT_SIZE_8; slot_pos2++ ) {

						if ( ( ( port_pos != port_pos2 ) && ( assign->port[port_pos].slot1[slot_pos] == assign->port[port_pos2].slot1[slot_pos2] ) ) ||
							 ( assign->port[port_pos].slot1[slot_pos] == assign->port[port_pos].slot2[slot_pos2] ) ||
							 ( assign->port[port_pos].slot1[slot_pos] == assign->port[port_pos].slot3[slot_pos2] ) ) {
							// Master number is illegal.
							Ddim_Print(("E:im_mxic_check_set_master_r_arbiter_param(): Read master number is overlap. port[%d].slot1[%d] = 0x%02x\n",
										port_pos, slot_pos, assign->port[port_pos].slot1[slot_pos]));
							return D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
				}
			}

			if ( assign->port[port_pos].slot2[slot_pos] != D_IM_MXIC_R_RESERVE ) {

				for ( port_pos2 = E_IM_MXIC_PORT_0; port_pos2 < E_IM_MXIC_PORT_MAX; port_pos2++ ) {

					for ( slot_pos2 = 0; slot_pos2 < D_IM_MXIC_SLOT_SIZE_8; slot_pos2++ ) {

						if ( ( ( port_pos != port_pos2 ) && ( assign->port[port_pos].slot2[slot_pos] == assign->port[port_pos2].slot2[slot_pos2] ) ) ||
							 ( assign->port[port_pos].slot2[slot_pos] == assign->port[port_pos].slot3[slot_pos2] ) ) {
							// Master number is illegal.
							Ddim_Print(("E:im_mxic_check_set_master_r_arbiter_param(): Read master number is overlap. port[%d].slot2[%d] = 0x%02x\n",
										port_pos, slot_pos, assign->port[port_pos].slot2[slot_pos]));
							return D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
				}
			}
		}
	}

	return D_DDIM_OK;
}

/**
Check parameter of Im_MXIC_Get_Master_R_Arbiter function.
@param[in]		arbiter					Target read arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_get_master_r_arbiter_param( E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT* const assign )
{
	if ( arbiter >= E_IM_MXIC_R_ARBITER_MAX ) {
		// arbiter parameter value is illegal.
		Ddim_Print(("E:im_mxic_check_get_master_r_arbiter_param(): arbiter is illegal.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( assign == NULL ) {
		// assign parameter is NULL.
		Ddim_Print(("E:im_mxic_check_get_master_r_arbiter_param(): assign is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}

/**
Check parameter of function that operates master mask factor and target.<br>
@param[in]		operation				Operation type.(Set/Get)<br>
@param[in]		master					Master number.<br>
@param[in]		group					Mask group.<br>
@param[in]		enable					Setting of whether to make a master into a mask factor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_check_master_mask_factor_target_param( E_IM_MXIC_OPERATION_TYPE operation, UCHAR master, 
															E_IM_MXIC_MASK_GROUP group, UCHAR* const enable )
{
	if ( group >= E_IM_MXIC_MASK_GROUP_MAX ) {
		// Group is illegal.
		Ddim_Print(("E:im_mxic_check_master_mask_factor_target_param(): group is illegal. group=%d\n", group));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( enable == NULL ) {
		// enable is NULL.
		Ddim_Print(("E:im_mxic_check_master_mask_factor_target_param(): enable is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( operation == E_IM_MXIC_OPERATION_TYPE_SET ) {
		if ( ( *enable != D_IM_MXIC_ON ) && ( *enable != D_IM_MXIC_OFF ) ) {
			// enable parameter value is illegal.
			Ddim_Print(("E:im_mxic_check_master_mask_factor_target_param(): enable is illegal. enable=%d\n", *enable));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}

	return D_DDIM_OK;
}
#endif // CO_PARAM_CHECK

/**
Get unit address.
@param[in]		unit							MXIC unit number.<br>
@param[out]		io_mxic							MXIC address.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
*/
static INT32 im_mxic_get_unit_address( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic** io_mxic )
{
	INT32 result;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_unit( unit ) != D_DDIM_OK ) {
		// Parameter value is illegal.
		Ddim_Assertion(("E:im_mxic_get_unit_address(): im_mxic_check_unit error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( io_mxic == NULL ) {
		// Parameter is NULL.
		Ddim_Assertion(("E:im_mxic_get_unit_address(): io_mxic is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = D_DDIM_OK;

	switch ( unit ) {
		case E_IM_MXIC_UNIT_0:
			*io_mxic = &IO_MXIC0;
			break;
		case E_IM_MXIC_UNIT_1:
			*io_mxic = &IO_MXIC1;
			break;
		case E_IM_MXIC_UNIT_2:
			*io_mxic = &IO_MXIC2;
			break;
		case E_IM_MXIC_UNIT_3:
			*io_mxic = &IO_MXIC3;
			break;
		case E_IM_MXIC_UNIT_4:
			*io_mxic = &IO_MXIC4;
			break;
		case E_IM_MXIC_UNIT_5:
			*io_mxic = &IO_MXIC5;
			break;
		case E_IM_MXIC_UNIT_6:
			*io_mxic = &IO_MXIC6;
			break;
		default:
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
			break;
	}

	return result;
}

/**
Get unit table address.
@param[in]		unit						MXIC unit number.<br>
@param[out]		io_mxic_tbl					MXIC table address.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
*/
static INT32 im_mxic_get_unit_tbl_address( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic_tbl** io_mxic_tbl )
{
	INT32 result;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_unit( unit ) != D_DDIM_OK ) {
		// Parameter value is illegal.
		Ddim_Assertion(("E:im_mxic_get_unit_tbl_address(): im_mxic_check_unit error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( io_mxic_tbl == NULL ) {
		// Parameter is NULL.
		Ddim_Assertion(("E:im_mxic_get_unit_tbl_address(): io_mxic_tbl is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = D_DDIM_OK;

	switch ( unit ) {
		case E_IM_MXIC_UNIT_1:
			*io_mxic_tbl = &IO_MXIC1_TBL;
			break;
		case E_IM_MXIC_UNIT_2:
			*io_mxic_tbl = &IO_MXIC2_TBL;
			break;
		case E_IM_MXIC_UNIT_3:
			*io_mxic_tbl = &IO_MXIC3_TBL;
			break;
		default:
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
			break;
	}

	return result;
}

/**
Set TAEN register for power saving.
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		io_mxic					MXIC address.<br>
@param[in]		enable					TRUE : Enable  TSL* access.<br>
										FALSE: Disable TSL* access.<br>
*/
static VOID im_mxic_set_taen( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic* io_mxic, BOOL enable )
{
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Dd_ARM_Critical_Section_Start( gIM_MXIC_Spin_Lock[ unit ].lock );
#endif

	im_mxic_on_pclk( unit );

	if ( enable == TRUE ) {
		// Enable TSL* access.
		gIM_MXIC_Taen_Counter[unit]++;

		if ( gIM_MXIC_Taen_Counter[unit] == 1 ) {
			io_mxic->TAEN.bit.TAEN = 1;
			Im_MXIC_Dsb();
			Dd_ARM_Wait_ns( 1000 );
		}
	}
	else {
		// Disable TSL* access.
		if ( 0 < gIM_MXIC_Taen_Counter[unit] ) {
			gIM_MXIC_Taen_Counter[unit]--;
		}
		if ( gIM_MXIC_Taen_Counter[unit] == 0 ) {
			io_mxic->TAEN.bit.TAEN = 0;
		}
	}

	im_mxic_off_pclk( unit );

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
	Dd_ARM_Critical_Section_End( gIM_MXIC_Spin_Lock[ unit ].lock );
#endif
}

/**
Waits until the command becomes executable. 
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		io_mxic					MXIC address.<br>
*/
static VOID im_mxic_wait_command_enable( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic* io_mxic )
{
	im_mxic_on_pclk( unit );

	Im_MXIC_Dsb();

	while ( io_mxic->TCFMD.bit.TCFLG != 1 ) {
#ifdef CO_DEBUG_ON_PC
		io_mxic->TCFMD.bit.TCFLG = 1;
#endif
		;	// DO NOTHING.
	}

	im_mxic_off_pclk( unit );
}

/**
Configuration start and wait complete.
@param[in]		io_mxic					MXIC address.<br>
*/
static VOID im_mxic_process_configuration( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic* io_mxic )
{
	static const union io_mxic_tmif clear_tcf = {
		.bit.__TCF = 1,
	};

	im_mxic_on_pclk( unit );

	// Clear interrupt flag.
	io_mxic->TMIF.word = clear_tcf.word;

	// Configuration start.
	io_mxic->TCFTRG.bit.TCFTRG = 1;

	Im_MXIC_Dsb();

	// Wait configuration complete.
	while ( io_mxic->TMIF.bit.__TCF == 0 ) {
		;
	}

	im_mxic_wait_command_enable( unit, io_mxic );	// Wait TCFLG=1.

	im_mxic_off_pclk( unit );
}

/**
Get TLVL*.LML* bit flag.
@param[in]		slot			Slot.(Set/Get)<br>
@param[out]		bit_flag		TLVL*.LML* bit flag for specified slot.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_get_slot_mask_bit_flag( E_IM_MXIC_SLOT slot, INT32* const bit_flag )
{
	INT32 result;

	result = D_DDIM_OK;

	switch ( slot ) {
		case E_IM_MXIC_SLOT_0:
			*bit_flag = D_IM_MXIC_TLVL_LML_SLOT0;
			break;
		case E_IM_MXIC_SLOT_1:
			*bit_flag = D_IM_MXIC_TLVL_LML_SLOT1;
			break;
		case E_IM_MXIC_SLOT_2:
			*bit_flag = D_IM_MXIC_TLVL_LML_SLOT2;
			break;
		case E_IM_MXIC_SLOT_3:
			*bit_flag = D_IM_MXIC_TLVL_LML_SLOT3;
			break;
		default:
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
			Ddim_Print(("E:im_mxic_get_slot_mask_bit_flag(): slot is illegal. slot=%d\n", slot));
			break;
	}

	return result;
}

/**
This function operates the factor of a master mask, and the target.
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in/out]	config					Master mask factor and target configration.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_rw_master_mask_factor_or_target( E_IM_MXIC_UNIT unit, T_IM_MXIC_MASTER_MASK_FACTOR_TARGET* const config )
{
	INT32						result;
	INT32						operate_bit;
	INT32						compare_bit;
	UCHAR						read_master;
	volatile struct io_jdsmxic*	io_mxic;

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {
		// Decide operation target bit.
		if ( config->kind_factor_target == E_IM_MXIC_MASTER_MASK_OPERATION_FACTOR ) {
			// Mask factor operation.
			if ( config->group == E_IM_MXIC_MASK_GROUP_A ) {
				// Group-A.
				operate_bit = D_IM_MXIC_TRMC_TRMFT_A_FACTOR;
				compare_bit = D_IM_MXIC_TRMC_TRMFT_A_TARGET;
			}
			else {
				// Group-B.
				operate_bit = D_IM_MXIC_TRMC_TRMFT_B_FACTOR;
				compare_bit = D_IM_MXIC_TRMC_TRMFT_B_TARGET;
			}
		}
		else {
			// Mask target operation.
			if (config->group == E_IM_MXIC_MASK_GROUP_A) {
				// Group-A.
				operate_bit = D_IM_MXIC_TRMC_TRMFT_A_TARGET;
				compare_bit = D_IM_MXIC_TRMC_TRMFT_A_FACTOR;
			}
			else {
				// Group-B.
				operate_bit = D_IM_MXIC_TRMC_TRMFT_B_TARGET;
				compare_bit = D_IM_MXIC_TRMC_TRMFT_B_FACTOR;
			}
		}

		if ( ( config->master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {
			// Write master.
			if ( config->master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {

					im_mxic_on_pclk( unit );

					if ( config->kind_set_get == E_IM_MXIC_OPERATION_TYPE_SET ) {
						// Set operation.
						if ( ( config->enable == D_IM_MXIC_ON ) && ( ( io_mxic->TRMCW[(config->master - 1)].bit.TRMFTW & compare_bit ) == 0 ) ) {
							// Set mask factor(target) ON. **The same channel can't be set to the factor and the target.
							io_mxic->TRMCW[(config->master - 1)].bit.TRMFTW |= operate_bit;				// W1-63.
						}
						else if ( config->enable == D_IM_MXIC_OFF ) {
							// Set mask factor(target) OFF.
							io_mxic->TRMCW[(config->master - 1)].bit.TRMFTW &= (~operate_bit);			// W1-63.
						}
						else {
							// Error.
							Ddim_Print(("E:im_mxic_rw_master_mask_factor_or_target(): Write master mask setting is illegal.\n"));
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
					else {
						// Get operation.
						if ( ( io_mxic->TRMCW[(config->master - 1)].bit.TRMFTW & operate_bit ) != 0 ) {	// W1-63.
							config->enable = D_IM_MXIC_ON;
						}
						else {
							config->enable = D_IM_MXIC_OFF;
						}
					}

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("im_mxic_rw_master_mask_factor_or_target(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					config->master = config->master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					if ( config->kind_set_get == E_IM_MXIC_OPERATION_TYPE_SET ) {
						// Set operation.
						if ( ( config->enable == D_IM_MXIC_ON ) && ( ( io_mxic->TRMCW_GR[config->master].bit.TRMFTW & compare_bit ) == 0 ) ) {
							// Set mask factor(target) ON. **The same channel can't be set to the factor and the target.
							io_mxic->TRMCW_GR[config->master].bit.TRMFTW |= operate_bit;				// W64-127.
						}
						else if ( config->enable == D_IM_MXIC_OFF ) {
							// Set mask factor(target) OFF.
							io_mxic->TRMCW_GR[config->master].bit.TRMFTW &= (~operate_bit);				// W64-127.
						}
						else {
							// Error.
							Ddim_Print(("E:im_mxic_rw_master_mask_factor_or_target(): Write master mask setting is illegal.\n"));
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
					else {
						// Get operation.
						if ( ( io_mxic->TRMCW_GR[config->master].bit.TRMFTW & operate_bit ) != 0 ) {	// W64-127.
							config->enable = D_IM_MXIC_ON;
						}
						else {
							config->enable = D_IM_MXIC_OFF;
						}
					}

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("im_mxic_rw_master_mask_factor_or_target(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if ( ( config->master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {
			// Read master.
			read_master = ( config->master & D_IM_MXIC_READ_MASTER_FLG_MASK );

			if ( read_master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {

					im_mxic_on_pclk( unit );

					if ( config->kind_set_get == E_IM_MXIC_OPERATION_TYPE_SET ) {
						// Set operation.
						if ( ( config->enable == D_IM_MXIC_ON ) && ( ( io_mxic->TRMCR[(read_master - 1)].bit.TRMFTR & compare_bit ) == 0 ) ) {
							// Set mask factor(target) ON. **The same channel can't be set to the factor and the target.
							io_mxic->TRMCR[(read_master - 1)].bit.TRMFTR |= operate_bit;				// R1-63.
						}
						else if ( config->enable == D_IM_MXIC_OFF ) {
							// Set mask factor(target) OFF.
							io_mxic->TRMCR[(read_master - 1)].bit.TRMFTR &= (~operate_bit);				// R1-63.
						}
						else {
							// Error.
							Ddim_Print(("E:im_mxic_rw_master_mask_factor_or_target(): Read master mask setting is illegal.\n"));
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
					else {
						// Get operation.
						if ( ( io_mxic->TRMCR[(read_master - 1)].bit.TRMFTR & operate_bit ) != 0 ) {	// R1-63.
							config->enable = D_IM_MXIC_ON;
						}
						else {
							config->enable = D_IM_MXIC_OFF;
						}
					}

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("im_mxic_rw_master_mask_factor_or_target(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					read_master = read_master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					if ( config->kind_set_get == E_IM_MXIC_OPERATION_TYPE_SET ) {
						// Set operation.
						if ( ( config->enable == D_IM_MXIC_ON ) && ( ( io_mxic->TRMCR_GR[read_master].bit.TRMFTR & compare_bit ) == 0 ) ) {
							// Set mask factor(target) ON. **The same channel can't be set to the factor and the target.
							io_mxic->TRMCR_GR[read_master].bit.TRMFTR |= operate_bit;				// R64-127.
						}
						else if ( config->enable == D_IM_MXIC_OFF ) {
							// Set mask factor(target) OFF.
							io_mxic->TRMCR_GR[read_master].bit.TRMFTR &= (~operate_bit);			// R64-127.
						}
						else {
							// Error.
							Ddim_Print(("E:im_mxic_rw_master_mask_factor_or_target(): Read master mask setting is illegal.\n"));
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
						}
					}
					else {
						// Get operation.
						if ( ( io_mxic->TRMCR_GR[read_master].bit.TRMFTR & operate_bit ) != 0 ) {	// R64-127.
							config->enable = D_IM_MXIC_ON;
						}
						else {
							config->enable = D_IM_MXIC_OFF;
						}
						
					}

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("im_mxic_rw_master_mask_factor_or_target(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:im_mxic_rw_master_mask_factor_or_target(): Master number is illegal. master=%d\n", config->master));
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:im_mxic_rw_master_mask_factor_or_target(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This local function set the W master to the W arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		io_mxic					MXIC address.<br>
@param[in]		io_mxic_tbl				MXIC table address.<br>
@param[in]		arbiter					Target write arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting changed by this function is reflected by the configration start.
@remarks		D_IM_MXIC_W_RESERVE is set to the part where the channel is not set up without fail.
*/
static INT32 im_mxic_set_master_w_arbiter_reg( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic* io_mxic, volatile struct io_jdsmxic_tbl* io_mxic_tbl, E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT const* const assign )
{
	INT32 i, j;
	INT32 result;

	result = D_DDIM_OK;

	if ( ( unit == E_IM_MXIC_UNIT_1 ) || ( unit == E_IM_MXIC_UNIT_2 ) || ( unit == E_IM_MXIC_UNIT_3 ) ) {

		if ( unit == E_IM_MXIC_UNIT_3 ) {
			DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.
		}

		im_mxic_on_hclk( unit );

		for ( i = E_IM_MXIC_PORT_0; i < E_IM_MXIC_PORT_MAX; i++ ) {

			for ( j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++ ) {

				switch ( arbiter ) {
					case E_IM_MXIC_W_ARBITER_W1:
						io_mxic_tbl->TSLW_1[i].byte[E_IM_MXIC_SLOT_0][j] = assign->port[i].slot0[j];
						io_mxic_tbl->TSLW_1[i].byte[E_IM_MXIC_SLOT_1][j] = assign->port[i].slot1[j];
						io_mxic_tbl->TSLW_1[i].byte[E_IM_MXIC_SLOT_2][j] = assign->port[i].slot2[j];
						io_mxic_tbl->TSLW_1[i].byte[E_IM_MXIC_SLOT_3][j] = assign->port[i].slot3[j];
						break;
					case E_IM_MXIC_W_ARBITER_W2:
						io_mxic_tbl->TSLW_2[i].byte[E_IM_MXIC_SLOT_0][j] = assign->port[i].slot0[j];
						io_mxic_tbl->TSLW_2[i].byte[E_IM_MXIC_SLOT_1][j] = assign->port[i].slot1[j];
						io_mxic_tbl->TSLW_2[i].byte[E_IM_MXIC_SLOT_2][j] = assign->port[i].slot2[j];
						io_mxic_tbl->TSLW_2[i].byte[E_IM_MXIC_SLOT_3][j] = assign->port[i].slot3[j];
						break;
					case E_IM_MXIC_W_ARBITER_W3:
						io_mxic_tbl->TSLW_3[i].byte[E_IM_MXIC_SLOT_0][j] = assign->port[i].slot0[j];
						io_mxic_tbl->TSLW_3[i].byte[E_IM_MXIC_SLOT_1][j] = assign->port[i].slot1[j];
						io_mxic_tbl->TSLW_3[i].byte[E_IM_MXIC_SLOT_2][j] = assign->port[i].slot2[j];
						io_mxic_tbl->TSLW_3[i].byte[E_IM_MXIC_SLOT_3][j] = assign->port[i].slot3[j];
						break;
					case E_IM_MXIC_W_ARBITER_W4:
						io_mxic_tbl->TSLW_4[i].byte[E_IM_MXIC_SLOT_0][j] = assign->port[i].slot0[j];
						io_mxic_tbl->TSLW_4[i].byte[E_IM_MXIC_SLOT_1][j] = assign->port[i].slot1[j];
						io_mxic_tbl->TSLW_4[i].byte[E_IM_MXIC_SLOT_2][j] = assign->port[i].slot2[j];
						io_mxic_tbl->TSLW_4[i].byte[E_IM_MXIC_SLOT_3][j] = assign->port[i].slot3[j];
						break;
					default:
						// arbiter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:im_mxic_set_master_w_arbiter_reg(): arbiter is illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
		}

		im_mxic_off_hclk( unit );

		if ( unit == E_IM_MXIC_UNIT_3 ) {
			DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.
		}
	}
	else {
		// Unit number is invalid.
//		Ddim_Print(("im_mxic_set_master_w_arbiter_reg(): unit number is invalid. unit=%d\n", unit));
	}

	return result;
}

/**
This local function get the R master from the R arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		io_mxic					MXIC address.<br>
@param[in]		io_mxic_tbl				MXIC table address.<br>
@param[in]		arbiter					Target read arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value.<br>
@param[out]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_set_master_r_arbiter_reg( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic* io_mxic, volatile struct io_jdsmxic_tbl* io_mxic_tbl, E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT const* const assign )
{
	INT32 i, j;
	INT32 result;

	result = D_DDIM_OK;

	if ( ( unit == E_IM_MXIC_UNIT_1 ) || ( unit == E_IM_MXIC_UNIT_2 ) || ( unit == E_IM_MXIC_UNIT_3 ) ) {

		if ( unit == E_IM_MXIC_UNIT_3 ) {
			DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.
		}

		im_mxic_on_hclk( unit );

		for ( i = E_IM_MXIC_PORT_0; i < E_IM_MXIC_PORT_MAX; i++ ) {

			for ( j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++ ) {

				switch ( arbiter ) {
					case E_IM_MXIC_R_ARBITER_R1:
						io_mxic_tbl->TSLR_1[i].byte[E_IM_MXIC_SLOT_0][j] = (assign->port[i].slot0[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_1[i].byte[E_IM_MXIC_SLOT_1][j] = (assign->port[i].slot1[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_1[i].byte[E_IM_MXIC_SLOT_2][j] = (assign->port[i].slot2[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_1[i].byte[E_IM_MXIC_SLOT_3][j] = (assign->port[i].slot3[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						break;
					case E_IM_MXIC_R_ARBITER_R2:
						io_mxic_tbl->TSLR_2[i].byte[E_IM_MXIC_SLOT_0][j] = (assign->port[i].slot0[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_2[i].byte[E_IM_MXIC_SLOT_1][j] = (assign->port[i].slot1[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_2[i].byte[E_IM_MXIC_SLOT_2][j] = (assign->port[i].slot2[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_2[i].byte[E_IM_MXIC_SLOT_3][j] = (assign->port[i].slot3[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						break;
					case E_IM_MXIC_R_ARBITER_R3:
						io_mxic_tbl->TSLR_3[i].byte[E_IM_MXIC_SLOT_0][j] = (assign->port[i].slot0[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_3[i].byte[E_IM_MXIC_SLOT_1][j] = (assign->port[i].slot1[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_3[i].byte[E_IM_MXIC_SLOT_2][j] = (assign->port[i].slot2[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_3[i].byte[E_IM_MXIC_SLOT_3][j] = (assign->port[i].slot3[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						break;
					case E_IM_MXIC_R_ARBITER_R4:
						io_mxic_tbl->TSLR_4[i].byte[E_IM_MXIC_SLOT_0][j] = (assign->port[i].slot0[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_4[i].byte[E_IM_MXIC_SLOT_1][j] = (assign->port[i].slot1[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_4[i].byte[E_IM_MXIC_SLOT_2][j] = (assign->port[i].slot2[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						io_mxic_tbl->TSLR_4[i].byte[E_IM_MXIC_SLOT_3][j] = (assign->port[i].slot3[j] & D_IM_MXIC_READ_MASTER_FLG_MASK);
						break;
					default:
						// arbiter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:im_mxic_set_master_r_arbiter_reg(): arbiter is illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
		}

		im_mxic_off_hclk( unit );

		Im_MXIC_Dsb();

		if ( unit == E_IM_MXIC_UNIT_3 ) {
			DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.
		}
	}
	else {
		// Unit number is invalid.
//		Ddim_Print(("im_mxic_set_master_r_arbiter_reg(): unit number is invalid. unit=%d\n", unit));
	}

	return result;
}

/**
Set slave area setting of specified slave number.<br>
However, reflection of change is not carried out.
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		io_mxic					MXIC address.<br>
@param[in]		slave_number			Slave area number.<br>
										Please refer to @ref E_IM_MXIC_SLAVE_NUMBER for a set value. <br>
@param[in]		slave_area				Slave area setting.<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
static INT32 im_mxic_set_slave_area_reg( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic* io_mxic, E_IM_MXIC_SLAVE_NUMBER slave_number, T_IM_MXIC_SLAVE_AREA const* const slave_area )
{
	UCHAR reg_read_sw;
#ifdef CO_PARAM_CHECK
	if ( slave_number >= E_IM_MXIC_SLAVE_NUMBER_MAX ) {
		// slave_number parameter value is illegal.
		Ddim_Assertion(("E:im_mxic_set_slave_area_reg(): slave_number is illegal. slave_number=%d\n", slave_number));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( slave_area == NULL ) {
		// slave_area parameter is NULL.
		Ddim_Assertion(("E:im_mxic_set_slave_area_reg(): slave_area is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_on_pclk( unit );

	reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

	if( reg_read_sw == D_IM_MXIC_ON ) {
		io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
	}

	io_mxic->TREGION[slave_number].bit.TSIZE = slave_area->size;
	io_mxic->TREGION[slave_number].bit.TSTAD = slave_area->address;

	if( reg_read_sw == D_IM_MXIC_ON ) {
		io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
	}

	im_mxic_off_pclk( unit );

	return D_DDIM_OK;
}

/**
Get slave area setting of specified slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		io_mxic					MXIC address.<br>
@param[in]		slave_number			Slave area number.<br>
										Please refer to @ref E_IM_MXIC_SLAVE_NUMBER for a set value. <br>
@param[in]		slave_area				Slave area setting.<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
static INT32 im_mxic_get_slave_area_reg( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic* io_mxic, E_IM_MXIC_SLAVE_NUMBER slave_number, T_IM_MXIC_SLAVE_AREA* const slave_area )
{
#ifdef CO_PARAM_CHECK
	if ( slave_number >= E_IM_MXIC_SLAVE_NUMBER_MAX ) {
		// slave_number parameter value is illegal.
		Ddim_Assertion(("E:im_mxic_get_slave_area_reg(): slave_number is illegal. slave_number=%d\n", slave_number));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( slave_area == NULL ) {
		// slave_area parameter is NULL.
		Ddim_Assertion(("E:im_mxic_get_slave_area_reg(): slave_area is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_on_pclk( unit );

	slave_area->size	= io_mxic->TREGION[slave_number].bit.TSIZE;
	slave_area->address	= io_mxic->TREGION[slave_number].bit.TSTAD;

	im_mxic_off_pclk( unit );

	return D_DDIM_OK;
}

/**
Get slot status of specified slot.
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		io_mxic					MXIC address.<br>
@param[in]		wr_arbiter				Write or Read Arbiter.<br>
@param[in]		arbiter					Target arbiter.<br>
@param[in]		port					Target port.<br>
@param[in]		slot					Target slot.<br>
@param[out]		slot_status				Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
static INT32 im_mxic_get_slot_status_array( E_IM_MXIC_UNIT unit, volatile struct io_jdsmxic* io_mxic, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT slot, UCHAR* const slot_status )
{
	INT32 result;

	result = D_DDIM_OK;

	if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {
		// W arbiter.
		if ( unit != E_IM_MXIC_UNIT_4 ) {
			switch ( arbiter ) {
				case E_IM_MXIC_SPEC_ARBITER_1:
					slot_status[0] = io_mxic->TSLSW_1[port][slot].bit.SLOTSW_0;
					slot_status[1] = io_mxic->TSLSW_1[port][slot].bit.SLOTSW_1;
					slot_status[2] = io_mxic->TSLSW_1[port][slot].bit.SLOTSW_2;
					slot_status[3] = io_mxic->TSLSW_1[port][slot].bit.SLOTSW_3;
					slot_status[4] = io_mxic->TSLSW_1[port][slot].bit.SLOTSW_4;
					slot_status[5] = io_mxic->TSLSW_1[port][slot].bit.SLOTSW_5;
					slot_status[6] = io_mxic->TSLSW_1[port][slot].bit.SLOTSW_6;
					slot_status[7] = io_mxic->TSLSW_1[port][slot].bit.SLOTSW_7;
					break;
				case E_IM_MXIC_SPEC_ARBITER_2:
					slot_status[0] = io_mxic->TSLSW_2[port][slot].bit.SLOTSW_0;
					slot_status[1] = io_mxic->TSLSW_2[port][slot].bit.SLOTSW_1;
					slot_status[2] = io_mxic->TSLSW_2[port][slot].bit.SLOTSW_2;
					slot_status[3] = io_mxic->TSLSW_2[port][slot].bit.SLOTSW_3;
					slot_status[4] = io_mxic->TSLSW_2[port][slot].bit.SLOTSW_4;
					slot_status[5] = io_mxic->TSLSW_2[port][slot].bit.SLOTSW_5;
					slot_status[6] = io_mxic->TSLSW_2[port][slot].bit.SLOTSW_6;
					slot_status[7] = io_mxic->TSLSW_2[port][slot].bit.SLOTSW_7;
					break;
				case E_IM_MXIC_SPEC_ARBITER_3:
					slot_status[0] = io_mxic->TSLSW_3[port][slot].bit.SLOTSW_0;
					slot_status[1] = io_mxic->TSLSW_3[port][slot].bit.SLOTSW_1;
					slot_status[2] = io_mxic->TSLSW_3[port][slot].bit.SLOTSW_2;
					slot_status[3] = io_mxic->TSLSW_3[port][slot].bit.SLOTSW_3;
					slot_status[4] = io_mxic->TSLSW_3[port][slot].bit.SLOTSW_4;
					slot_status[5] = io_mxic->TSLSW_3[port][slot].bit.SLOTSW_5;
					slot_status[6] = io_mxic->TSLSW_3[port][slot].bit.SLOTSW_6;
					slot_status[7] = io_mxic->TSLSW_3[port][slot].bit.SLOTSW_7;
					break;
				case E_IM_MXIC_SPEC_ARBITER_4:
					slot_status[0] = io_mxic->TSLSW_4[port][slot].bit.SLOTSW_0;
					slot_status[1] = io_mxic->TSLSW_4[port][slot].bit.SLOTSW_1;
					slot_status[2] = io_mxic->TSLSW_4[port][slot].bit.SLOTSW_2;
					slot_status[3] = io_mxic->TSLSW_4[port][slot].bit.SLOTSW_3;
					slot_status[4] = io_mxic->TSLSW_4[port][slot].bit.SLOTSW_4;
					slot_status[5] = io_mxic->TSLSW_4[port][slot].bit.SLOTSW_5;
					slot_status[6] = io_mxic->TSLSW_4[port][slot].bit.SLOTSW_6;
					slot_status[7] = io_mxic->TSLSW_4[port][slot].bit.SLOTSW_7;
					break;
				default:
					// arbiter is illegal.
					result = D_IM_MXIC_INPUT_PARAM_ERROR;
					Ddim_Print(("E:im_mxic_get_slot_status_array(): arbiter is illegal. arbiter=%d\n", arbiter));
					break;
			}
		}
		else {
			slot_status[0] = 0;
			slot_status[1] = 0;
			slot_status[2] = 0;
			slot_status[3] = 0;
			slot_status[4] = 0;
			slot_status[5] = 0;
			slot_status[6] = 0;
			slot_status[7] = 0;

			// Unit number is invalid.
//			Ddim_Print(("im_mxic_get_slot_status_array(): unit number is invalid. unit=%d\n", unit));
		}
	}
	else {
		// R arbiter.
		if ( unit != E_IM_MXIC_UNIT_0 ) {
			switch ( arbiter ) {
				case E_IM_MXIC_SPEC_ARBITER_1:
					slot_status[0] = io_mxic->TSLSR_1[port][slot].bit.SLOTSR_0;
					slot_status[1] = io_mxic->TSLSR_1[port][slot].bit.SLOTSR_1;
					slot_status[2] = io_mxic->TSLSR_1[port][slot].bit.SLOTSR_2;
					slot_status[3] = io_mxic->TSLSR_1[port][slot].bit.SLOTSR_3;
					slot_status[4] = io_mxic->TSLSR_1[port][slot].bit.SLOTSR_4;
					slot_status[5] = io_mxic->TSLSR_1[port][slot].bit.SLOTSR_5;
					slot_status[6] = io_mxic->TSLSR_1[port][slot].bit.SLOTSR_6;
					slot_status[7] = io_mxic->TSLSR_1[port][slot].bit.SLOTSR_7;
					break;
				case E_IM_MXIC_SPEC_ARBITER_2:
					slot_status[0] = io_mxic->TSLSR_2[port][slot].bit.SLOTSR_0;
					slot_status[1] = io_mxic->TSLSR_2[port][slot].bit.SLOTSR_1;
					slot_status[2] = io_mxic->TSLSR_2[port][slot].bit.SLOTSR_2;
					slot_status[3] = io_mxic->TSLSR_2[port][slot].bit.SLOTSR_3;
					slot_status[4] = io_mxic->TSLSR_2[port][slot].bit.SLOTSR_4;
					slot_status[5] = io_mxic->TSLSR_2[port][slot].bit.SLOTSR_5;
					slot_status[6] = io_mxic->TSLSR_2[port][slot].bit.SLOTSR_6;
					slot_status[7] = io_mxic->TSLSR_2[port][slot].bit.SLOTSR_7;
					break;
				case E_IM_MXIC_SPEC_ARBITER_3:
					slot_status[0] = io_mxic->TSLSR_3[port][slot].bit.SLOTSR_0;
					slot_status[1] = io_mxic->TSLSR_3[port][slot].bit.SLOTSR_1;
					slot_status[2] = io_mxic->TSLSR_3[port][slot].bit.SLOTSR_2;
					slot_status[3] = io_mxic->TSLSR_3[port][slot].bit.SLOTSR_3;
					slot_status[4] = io_mxic->TSLSR_3[port][slot].bit.SLOTSR_4;
					slot_status[5] = io_mxic->TSLSR_3[port][slot].bit.SLOTSR_5;
					slot_status[6] = io_mxic->TSLSR_3[port][slot].bit.SLOTSR_6;
					slot_status[7] = io_mxic->TSLSR_3[port][slot].bit.SLOTSR_7;
					break;
				case E_IM_MXIC_SPEC_ARBITER_4:
					slot_status[0] = io_mxic->TSLSR_4[port][slot].bit.SLOTSR_0;
					slot_status[1] = io_mxic->TSLSR_4[port][slot].bit.SLOTSR_1;
					slot_status[2] = io_mxic->TSLSR_4[port][slot].bit.SLOTSR_2;
					slot_status[3] = io_mxic->TSLSR_4[port][slot].bit.SLOTSR_3;
					slot_status[4] = io_mxic->TSLSR_4[port][slot].bit.SLOTSR_4;
					slot_status[5] = io_mxic->TSLSR_4[port][slot].bit.SLOTSR_5;
					slot_status[6] = io_mxic->TSLSR_4[port][slot].bit.SLOTSR_6;
					slot_status[7] = io_mxic->TSLSR_4[port][slot].bit.SLOTSR_7;
					break;
				default:
					// arbiter is illegal.
					result = D_IM_MXIC_INPUT_PARAM_ERROR;
					Ddim_Print(("E:im_mxic_get_slot_status_array(): arbiter is illegal. arbiter=%d\n", arbiter));
					break;
			}
		}
		else {
			slot_status[0] = 0;
			slot_status[1] = 0;
			slot_status[2] = 0;
			slot_status[3] = 0;
			slot_status[4] = 0;
			slot_status[5] = 0;
			slot_status[6] = 0;
			slot_status[7] = 0;

			// Unit number is invalid.
//			Ddim_Print(("im_mxic_get_slot_status_array(): unit number is invalid. unit=%d\n", unit));
		}
	}

	return result;
}

/**
Init arbiter assign setting.<br>
@param[in]		unit						Target unit number.<br>
											Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_assign					Arbiter assign.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
*/
static INT32 im_mxic_init_arbiter_assign( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ARBITER_ASSIGN* all_assign )
{
	INT32	i, j, k;
	INT32	result;

	result = D_DDIM_OK;

	// (*Note) Recommendation setup is performed here!.
	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {

			for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {

				all_assign->w_arbiter[ i ].port[ j ].slot0[ k ] = D_IM_MXIC_W_RESERVE;
				all_assign->w_arbiter[ i ].port[ j ].slot1[ k ] = D_IM_MXIC_W_RESERVE;
				all_assign->w_arbiter[ i ].port[ j ].slot2[ k ] = D_IM_MXIC_W_RESERVE;
				all_assign->w_arbiter[ i ].port[ j ].slot3[ k ] = D_IM_MXIC_W_RESERVE;
			}
		}
	}

	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {

			for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {

				all_assign->r_arbiter[ i ].port[ j ].slot0[ k ] = D_IM_MXIC_R_RESERVE;
				all_assign->r_arbiter[ i ].port[ j ].slot1[ k ] = D_IM_MXIC_R_RESERVE;
				all_assign->r_arbiter[ i ].port[ j ].slot2[ k ] = D_IM_MXIC_R_RESERVE;
				all_assign->r_arbiter[ i ].port[ j ].slot3[ k ] = D_IM_MXIC_R_RESERVE;
			}
		}
	}

	switch ( unit ) {
		case E_IM_MXIC_UNIT_0:
			// Non-implementation.
			break;
		case E_IM_MXIC_UNIT_1:
			// TSLW_1, PORT0, SLOT0/1/2.
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_1_W_SRO;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_1_W_B2B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_1_W_LTMDT;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_1_W_CNR_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 4 ] = D_IM_MXIC_1_W_CNR_B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_1_W_SRO;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_1_W_B2B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_1_W_LTMDT;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_1_W_CNR_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 4 ] = D_IM_MXIC_1_W_CNR_B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_1_W_SRO;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_1_W_B2B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_1_W_LTMDT;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_1_W_CNR_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 4 ] = D_IM_MXIC_1_W_CNR_B;

			// TSLW_1, PORT1, SLOT0/1/2.
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 0 ] = D_IM_MXIC_1_W_R2Y_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_1_W_R2Y_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 2 ] = D_IM_MXIC_1_W_R2Y_C;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 3 ] = D_IM_MXIC_1_W_R2Y_D;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 0 ] = D_IM_MXIC_1_W_R2Y_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_1_W_R2Y_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 2 ] = D_IM_MXIC_1_W_R2Y_C;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 3 ] = D_IM_MXIC_1_W_R2Y_D;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 0 ] = D_IM_MXIC_1_W_R2Y_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_1_W_R2Y_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 2 ] = D_IM_MXIC_1_W_R2Y_C;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 3 ] = D_IM_MXIC_1_W_R2Y_D;

			// TSLW_2, PORT0, SLOT0/1/2.
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_1_W_SRO;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_1_W_B2B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_1_W_LTMDT;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_1_W_CNR_A;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_1_W_SRO;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_1_W_B2B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_1_W_LTMDT;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_1_W_CNR_A;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_1_W_SRO;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_1_W_B2B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_1_W_LTMDT;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_1_W_CNR_A;

			// TSLW_2, PORT1, SLOT0/1/2.
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 0 ] = D_IM_MXIC_1_W_R2Y_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_1_W_R2Y_B;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 2 ] = D_IM_MXIC_1_W_R2Y_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 3 ] = D_IM_MXIC_1_W_R2Y_D;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 0 ] = D_IM_MXIC_1_W_R2Y_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_1_W_R2Y_B;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 2 ] = D_IM_MXIC_1_W_R2Y_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 3 ] = D_IM_MXIC_1_W_R2Y_D;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 0 ] = D_IM_MXIC_1_W_R2Y_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_1_W_R2Y_B;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 2 ] = D_IM_MXIC_1_W_R2Y_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 3 ] = D_IM_MXIC_1_W_R2Y_D;

			// TSLR_1, Port0, SLOT0/1/2.
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_1_R_SRO;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_1_R_B2B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_1_R_LTMDT;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_1_R_R2Y_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 4 ] = D_IM_MXIC_1_R_R2Y_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 5 ] = D_IM_MXIC_1_R_R2Y_C;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_1_R_SRO;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_1_R_B2B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_1_R_LTMDT;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_1_R_R2Y_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 4 ] = D_IM_MXIC_1_R_R2Y_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 5 ] = D_IM_MXIC_1_R_R2Y_C;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_1_R_SRO;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_1_R_B2B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_1_R_LTMDT;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_1_R_R2Y_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 4 ] = D_IM_MXIC_1_R_R2Y_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 5 ] = D_IM_MXIC_1_R_R2Y_C;

			// TSLR_1, Port1, SLOT0/1/2.
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_1_R_CNR_B;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_1_R_CNR_B;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_1_R_CNR_B;

			// TSLR_2, Port0, SLOT0/1/2.
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_1_R_SRO;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_1_R_B2B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_1_R_LTMDT;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_1_R_R2Y_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 4 ] = D_IM_MXIC_1_R_R2Y_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 5 ] = D_IM_MXIC_1_R_R2Y_C;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_1_R_SRO;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_1_R_B2B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_1_R_LTMDT;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_1_R_R2Y_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 4 ] = D_IM_MXIC_1_R_R2Y_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 5 ] = D_IM_MXIC_1_R_R2Y_C;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_1_R_SRO;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_1_R_B2B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_1_R_LTMDT;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_1_R_R2Y_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 4 ] = D_IM_MXIC_1_R_R2Y_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 5 ] = D_IM_MXIC_1_R_R2Y_C;

			// TSLR_2, Port1, SLOT0/1/2.
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_1_R_CNR_B;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_1_R_CNR_B;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_1_R_CNR_B;
			break;
		case E_IM_MXIC_UNIT_2:
			// TSLW_1, PORT0, SLOT0/1/2.
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_2_W_SRO;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_2_W_B2B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_2_W_LTMDT;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_2_W_CNR_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 4 ] = D_IM_MXIC_2_W_CNR_B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_2_W_SRO;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_2_W_B2B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_2_W_LTMDT;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_2_W_CNR_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 4 ] = D_IM_MXIC_2_W_CNR_B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_2_W_SRO;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_2_W_B2B;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_2_W_LTMDT;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_2_W_CNR_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 4 ] = D_IM_MXIC_2_W_CNR_B;

			// TSLW_1, PORT1, SLOT0/1/2.
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 0 ] = D_IM_MXIC_2_W_R2Y_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_2_W_R2Y_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 2 ] = D_IM_MXIC_2_W_R2Y_C;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 3 ] = D_IM_MXIC_2_W_R2Y_D;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 0 ] = D_IM_MXIC_2_W_R2Y_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_2_W_R2Y_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 2 ] = D_IM_MXIC_2_W_R2Y_C;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 3 ] = D_IM_MXIC_2_W_R2Y_D;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 0 ] = D_IM_MXIC_2_W_R2Y_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_2_W_R2Y_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 2 ] = D_IM_MXIC_2_W_R2Y_C;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 3 ] = D_IM_MXIC_2_W_R2Y_D;

			// TSLW_2, PORT0, SLOT0/1/2.
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_2_W_SRO;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_2_W_B2B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_2_W_LTMDT;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_2_W_CNR_B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_2_W_SRO;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_2_W_B2B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_2_W_LTMDT;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_2_W_CNR_B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_2_W_SRO;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_2_W_B2B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_2_W_LTMDT;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_2_W_CNR_B;

			// TSLW_2, PORT1, SLOT0/1/2.
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 0 ] = D_IM_MXIC_2_W_R2Y_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_2_W_R2Y_B;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 2 ] = D_IM_MXIC_2_W_R2Y_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 3 ] = D_IM_MXIC_2_W_R2Y_D;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 0 ] = D_IM_MXIC_2_W_R2Y_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_2_W_R2Y_B;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 2 ] = D_IM_MXIC_2_W_R2Y_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 3 ] = D_IM_MXIC_2_W_R2Y_D;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 0 ] = D_IM_MXIC_2_W_R2Y_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_2_W_R2Y_B;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 2 ] = D_IM_MXIC_2_W_R2Y_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 3 ] = D_IM_MXIC_2_W_R2Y_D;

			// TSLR_1, PORT0, SLOT0/1/2/.
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_2_R_SRO;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_2_R_B2B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_2_R_LTMDT;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_2_R_R2Y_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 4 ] = D_IM_MXIC_2_R_R2Y_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 5 ] = D_IM_MXIC_2_R_R2Y_C;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_2_R_SRO;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_2_R_B2B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_2_R_LTMDT;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_2_R_R2Y_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 4 ] = D_IM_MXIC_2_R_R2Y_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 5 ] = D_IM_MXIC_2_R_R2Y_C;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_2_R_SRO;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_2_R_B2B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_2_R_LTMDT;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_2_R_R2Y_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 4 ] = D_IM_MXIC_2_R_R2Y_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 5 ] = D_IM_MXIC_2_R_R2Y_C;

			// TSLR_1, PORT1, SLOT0/1/2.
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_2_R_CNR_B;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_2_R_CNR_B;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_2_R_CNR_B;

			// TSLR_2, PORT0, SLOT0/1/2.
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_2_R_SRO;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_2_R_B2B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_2_R_LTMDT;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_2_R_R2Y_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 4 ] = D_IM_MXIC_2_R_R2Y_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 5 ] = D_IM_MXIC_2_R_R2Y_C;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_2_R_SRO;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_2_R_B2B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_2_R_LTMDT;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_2_R_R2Y_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 4 ] = D_IM_MXIC_2_R_R2Y_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 5 ] = D_IM_MXIC_2_R_R2Y_C;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_2_R_SRO;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_2_R_B2B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_2_R_LTMDT;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_2_R_R2Y_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 4 ] = D_IM_MXIC_2_R_R2Y_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 5 ] = D_IM_MXIC_2_R_R2Y_C;

			// TSLR_2, PORT1, SLOT0/1/2.
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_2_R_CNR_B;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_2_R_CNR_B;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_2_R_CNR_B;
			break;
		case E_IM_MXIC_UNIT_3:
			// TSLW_1, PORT0, SLOT0/1/2.
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_3_W_IPU_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_3_W_IPU_C;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_3_W_IPU_D;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_3_W_IPU_E;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_3_W_IPU_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_3_W_IPU_C;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_3_W_IPU_D;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_3_W_IPU_E;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_3_W_IPU_A;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_3_W_IPU_C;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_3_W_IPU_D;
			all_assign->w_arbiter[ 0 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_3_W_IPU_E;

			// TSLW_1, PORT1, SLOT0/1/2.
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 0 ] = D_IM_MXIC_3_W_SHDR_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_3_W_SHDR_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot0[ 2 ] = D_IM_MXIC_3_W_SHDR_C;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 0 ] = D_IM_MXIC_3_W_SHDR_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_3_W_SHDR_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot1[ 2 ] = D_IM_MXIC_3_W_SHDR_C;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 0 ] = D_IM_MXIC_3_W_SHDR_A;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_3_W_SHDR_B;
			all_assign->w_arbiter[ 0 ].port[ 1 ].slot2[ 2 ] = D_IM_MXIC_3_W_SHDR_C;

			// TSLW_1, PORT2, SLOT0/1/2.
			all_assign->w_arbiter[ 0 ].port[ 2 ].slot0[ 0 ] = D_IM_MXIC_3_W_ME;
			all_assign->w_arbiter[ 0 ].port[ 2 ].slot0[ 1 ] = D_IM_MXIC_3_W_GPU;
			all_assign->w_arbiter[ 0 ].port[ 2 ].slot1[ 0 ] = D_IM_MXIC_3_W_ME;
			all_assign->w_arbiter[ 0 ].port[ 2 ].slot1[ 1 ] = D_IM_MXIC_3_W_GPU;
			all_assign->w_arbiter[ 0 ].port[ 2 ].slot2[ 0 ] = D_IM_MXIC_3_W_ME;
			all_assign->w_arbiter[ 0 ].port[ 2 ].slot2[ 1 ] = D_IM_MXIC_3_W_GPU;

			// TSLW_2, PORT0, SLOT0/1/2.
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_3_W_SHDR_A;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_3_W_SHDR_B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_3_W_SHDR_C;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_3_W_SHDR_A;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_3_W_SHDR_B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_3_W_SHDR_C;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_3_W_SHDR_A;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_3_W_SHDR_B;
			all_assign->w_arbiter[ 1 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_3_W_SHDR_C;

			// TSLW_2, PORT1, SLOT0/1/2.
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 0 ] = D_IM_MXIC_3_W_IPU_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_3_W_IPU_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 2 ] = D_IM_MXIC_3_W_IPU_D;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot0[ 3 ] = D_IM_MXIC_3_W_IPU_E;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 0 ] = D_IM_MXIC_3_W_IPU_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_3_W_IPU_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 2 ] = D_IM_MXIC_3_W_IPU_D;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot1[ 3 ] = D_IM_MXIC_3_W_IPU_E;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 0 ] = D_IM_MXIC_3_W_IPU_A;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_3_W_IPU_C;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 2 ] = D_IM_MXIC_3_W_IPU_D;
			all_assign->w_arbiter[ 1 ].port[ 1 ].slot2[ 3 ] = D_IM_MXIC_3_W_IPU_E;

			// TSLR_1, PORT0, SLOT0/1/2.
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_3_R_IPU_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_3_R_IPU_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_3_R_IPU_C;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_3_R_IPU_D;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_3_R_IPU_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_3_R_IPU_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_3_R_IPU_C;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_3_R_IPU_D;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_3_R_IPU_A;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_3_R_IPU_B;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_3_R_IPU_C;
			all_assign->r_arbiter[ 0 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_3_R_IPU_D;

			// TSLR_1, PORT1, SLOT0/1/2.
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot0[ 0 ] = D_IM_MXIC_3_R_SHDR_A;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_3_R_SHDR_B;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot0[ 2 ] = D_IM_MXIC_3_R_SHDR_C;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot0[ 3 ] = D_IM_MXIC_3_R_SHDR_D;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot1[ 0 ] = D_IM_MXIC_3_R_SHDR_A;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_3_R_SHDR_B;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot1[ 2 ] = D_IM_MXIC_3_R_SHDR_C;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot1[ 3 ] = D_IM_MXIC_3_R_SHDR_D;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot2[ 0 ] = D_IM_MXIC_3_R_SHDR_A;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_3_R_SHDR_B;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot2[ 2 ] = D_IM_MXIC_3_R_SHDR_C;
			all_assign->r_arbiter[ 0 ].port[ 1 ].slot2[ 3 ] = D_IM_MXIC_3_R_SHDR_D;

			// TSLR_1, PORT2, SLOT0/1/2.
			all_assign->r_arbiter[ 0 ].port[ 2 ].slot0[ 0 ] = D_IM_MXIC_3_R_ME;
			all_assign->r_arbiter[ 0 ].port[ 2 ].slot0[ 1 ] = D_IM_MXIC_3_R_GPU;
			all_assign->r_arbiter[ 0 ].port[ 2 ].slot1[ 0 ] = D_IM_MXIC_3_R_ME;
			all_assign->r_arbiter[ 0 ].port[ 2 ].slot1[ 1 ] = D_IM_MXIC_3_R_GPU;
			all_assign->r_arbiter[ 0 ].port[ 2 ].slot2[ 0 ] = D_IM_MXIC_3_R_ME;
			all_assign->r_arbiter[ 0 ].port[ 2 ].slot2[ 1 ] = D_IM_MXIC_3_R_GPU;

			// TSLR_2, PORT0, SLOT0/1/2.
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 0 ] = D_IM_MXIC_3_R_SHDR_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 1 ] = D_IM_MXIC_3_R_SHDR_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 2 ] = D_IM_MXIC_3_R_SHDR_C;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot0[ 3 ] = D_IM_MXIC_3_R_SHDR_D;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 0 ] = D_IM_MXIC_3_R_SHDR_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 1 ] = D_IM_MXIC_3_R_SHDR_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 2 ] = D_IM_MXIC_3_R_SHDR_C;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot1[ 3 ] = D_IM_MXIC_3_R_SHDR_D;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 0 ] = D_IM_MXIC_3_R_SHDR_A;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 1 ] = D_IM_MXIC_3_R_SHDR_B;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 2 ] = D_IM_MXIC_3_R_SHDR_C;
			all_assign->r_arbiter[ 1 ].port[ 0 ].slot2[ 3 ] = D_IM_MXIC_3_R_SHDR_D;

			// TSLR_2, PORT1, SLOT0/1/2.
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot0[ 0 ] = D_IM_MXIC_3_R_IPU_A;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot0[ 1 ] = D_IM_MXIC_3_R_IPU_B;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot0[ 2 ] = D_IM_MXIC_3_R_IPU_C;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot0[ 3 ] = D_IM_MXIC_3_R_IPU_D;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot1[ 0 ] = D_IM_MXIC_3_R_IPU_A;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot1[ 1 ] = D_IM_MXIC_3_R_IPU_B;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot1[ 2 ] = D_IM_MXIC_3_R_IPU_C;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot1[ 3 ] = D_IM_MXIC_3_R_IPU_D;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot2[ 0 ] = D_IM_MXIC_3_R_IPU_A;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot2[ 1 ] = D_IM_MXIC_3_R_IPU_B;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot2[ 2 ] = D_IM_MXIC_3_R_IPU_C;
			all_assign->r_arbiter[ 1 ].port[ 1 ].slot2[ 3 ] = D_IM_MXIC_3_R_IPU_D;
			break;
		case E_IM_MXIC_UNIT_4:
		case E_IM_MXIC_UNIT_5:
		case E_IM_MXIC_UNIT_6:
			// Non-implementation.
			break;
		default :
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
			Ddim_Print(("E:im_mxic_init_arbiter_assign(): unit number(%d) is illegal.\n", unit));
			break;
	}

	return result;
}

/**
Init arbiter assign setting.<br>
@param[in]		unit						Target unit number.<br>
											Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_levelport				All LevelPort information.<br>
											Please refer to @ref T_IM_MXIC_ALL_LEVELPORT.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
*/
static INT32 im_mxic_init_all_levelport( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_LEVELPORT* all_levelport )
{
	INT32	i, j;
	INT32	result;

	result = D_DDIM_OK;

	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

			all_levelport->w_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)0;
			all_levelport->w_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)0;
		}
	}

	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

			all_levelport->r_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)0;
			all_levelport->r_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)0;
		}
	}

	switch ( unit ) {
		case E_IM_MXIC_UNIT_0:
			break;
		case E_IM_MXIC_UNIT_1:
		case E_IM_MXIC_UNIT_2:
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[  9 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[ 10 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[ 11 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[ 12 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[ 13 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[ 14 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R1 ].port[ 13 ]	= E_IM_MXIC_PORT_1;
			break;
		case E_IM_MXIC_UNIT_3:
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[  4 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[  5 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[  6 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[  8 ]	= E_IM_MXIC_PORT_2;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W1 ].port[ 16 ]	= E_IM_MXIC_PORT_2;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R1 ].port[  4 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R1 ].port[  5 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R1 ].port[  6 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R1 ].port[  7 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R1 ].port[  8 ]	= E_IM_MXIC_PORT_2;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R1 ].port[ 16 ]	= E_IM_MXIC_PORT_2;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W2 ].port[  9 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W2 ].port[ 11 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W2 ].port[ 12 ]	= E_IM_MXIC_PORT_1;
			all_levelport->w_arbiter[ E_IM_MXIC_W_ARBITER_W2 ].port[ 13 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R2 ].port[  9 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R2 ].port[ 10 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R2 ].port[ 11 ]	= E_IM_MXIC_PORT_1;
			all_levelport->r_arbiter[ E_IM_MXIC_R_ARBITER_R2 ].port[ 12 ]	= E_IM_MXIC_PORT_1;
			break;
		case E_IM_MXIC_UNIT_4:
		case E_IM_MXIC_UNIT_5:
		case E_IM_MXIC_UNIT_6:
			break;
		default :
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
			Ddim_Print(("E:im_mxic_init_all_levelport(): unit number(%d) is illegal.\n", unit));
			break;
	}

	return result;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

/**
This function does software reset.<br>
@param[in]		unit							Target unit number.<br>
												Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
*/
INT32 Im_MXIC_Reset( E_IM_MXIC_UNIT unit )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		// Reset release.
		io_mxic->TMIRST.bit.SR = 0;

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Reset(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set initial value to register.<br>
@param[in]		unit							Target unit number.<br>
												Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
*/
INT32 Im_MXIC_Init( E_IM_MXIC_UNIT unit )
{
	INT32										i, j, k;
	INT32										result;
	T_IM_MXIC_ALL_ARBITER_ASSIGN				all_assign;
	T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL			slot;
	T_IM_MXIC_ALL_LEVELPORT 					all_levelport;
	volatile struct io_jdsmxic*					io_mxic;
	volatile struct io_jdsmxic_tbl*				io_mxic_tbl;

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		// Because TCE/TCF is not used, the interruption is invalidated. 
		io_mxic->TMIE.bit.TCE = 0;
		io_mxic->TMIF.word &= D_IM_MXIC_TMIF_BIT_TCF;

		im_mxic_off_pclk( unit );

		im_mxic_wait_command_enable( unit, io_mxic );	// Wait TCFLG=1.

		im_mxic_set_taen( unit, io_mxic, TRUE );		// TAEN-ON.

		// Set Slot priority level.
		// Write Slave.
		for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

			for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {
				// (Slave=1 or 2 or 3) and (port=0).
				if ( ( i == E_IM_MXIC_SPEC_ARBITER_1 || i == E_IM_MXIC_SPEC_ARBITER_2 || i == E_IM_MXIC_SPEC_ARBITER_3 ) &&
					 ( j == E_IM_MXIC_PORT_0 )) {
					slot.w_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
					slot.w_level[i].level0[j].upper_priority_level	= 5;
					slot.w_level[i].level0[j].lower_priority_level	= 0;
					slot.w_level[i].level1[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.w_level[i].level1[j].upper_priority_level	= 0;
					slot.w_level[i].level1[j].lower_priority_level	= 0;
					slot.w_level[i].level2[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.w_level[i].level2[j].upper_priority_level	= 0;
					slot.w_level[i].level2[j].lower_priority_level	= 0;
				}

				// (Slave=1 or 2 or 3) and (port=1).
				if ( ( i == E_IM_MXIC_SPEC_ARBITER_1 || i == E_IM_MXIC_SPEC_ARBITER_2 || i == E_IM_MXIC_SPEC_ARBITER_3 ) &&
					 ( j == E_IM_MXIC_PORT_1 )) {
					slot.w_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
					slot.w_level[i].level0[j].upper_priority_level	= 1;
					slot.w_level[i].level0[j].lower_priority_level	= 7;
					slot.w_level[i].level1[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.w_level[i].level1[j].upper_priority_level	= 4;
					slot.w_level[i].level1[j].lower_priority_level	= 3;
					slot.w_level[i].level2[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.w_level[i].level2[j].upper_priority_level	= 3;
					slot.w_level[i].level2[j].lower_priority_level	= 0;
				}

				// (Slave=1 or 2 or 3) and (port=2).
				if ( ( i == E_IM_MXIC_SPEC_ARBITER_1 || i == E_IM_MXIC_SPEC_ARBITER_2 || i == E_IM_MXIC_SPEC_ARBITER_3 ) &&
					 ( j == E_IM_MXIC_PORT_2 )) {
					slot.w_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
					slot.w_level[i].level0[j].upper_priority_level	= 3;
					slot.w_level[i].level0[j].lower_priority_level	= 8;
					slot.w_level[i].level1[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.w_level[i].level1[j].upper_priority_level	= 5;
					slot.w_level[i].level1[j].lower_priority_level	= 3;
					slot.w_level[i].level2[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.w_level[i].level2[j].upper_priority_level	= 3;
					slot.w_level[i].level2[j].lower_priority_level	= 0;
				}

				// (Slave=4).
				if ( i == E_IM_MXIC_SPEC_ARBITER_4 ) {

					if ( j == E_IM_MXIC_PORT_0 ) {
						slot.w_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
						slot.w_level[i].level0[j].upper_priority_level	= 7;
						slot.w_level[i].level0[j].lower_priority_level	= 17;
					}
					else {
						slot.w_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
						slot.w_level[i].level0[j].upper_priority_level	= 0;
						slot.w_level[i].level0[j].lower_priority_level	= 0;
					}

					slot.w_level[i].level1[j].priority_level_type		= D_IM_MXIC_SLOT_PRIORITY_UPPER;

					if ( j == E_IM_MXIC_PORT_0 ) {
						slot.w_level[i].level1[j].upper_priority_level	= 7;
						slot.w_level[i].level1[j].lower_priority_level	= 10;
					}
					else {
						slot.w_level[i].level1[j].upper_priority_level	= 0;
						slot.w_level[i].level1[j].lower_priority_level	= 0;
					}

					slot.w_level[i].level2[j].priority_level_type		= D_IM_MXIC_SLOT_PRIORITY_UPPER;

					if ( j == E_IM_MXIC_PORT_0 ) {
						slot.w_level[i].level2[j].upper_priority_level	= 4;
						slot.w_level[i].level2[j].lower_priority_level	= 6;
					}
					else {
						slot.w_level[i].level2[j].upper_priority_level	= 0;
						slot.w_level[i].level2[j].lower_priority_level	= 0;
					}
				}
			}
		}

		// Read Slave.
		for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

			for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {
				// (Slave=1 or 2 or 3) and (port=0).
				if ( ( i == E_IM_MXIC_SPEC_ARBITER_1 || i == E_IM_MXIC_SPEC_ARBITER_2 || i == E_IM_MXIC_SPEC_ARBITER_3 ) &&
					 ( j == E_IM_MXIC_PORT_0 ) ) {
					slot.r_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
					slot.r_level[i].level0[j].upper_priority_level	= 5;
					slot.r_level[i].level0[j].lower_priority_level	= 0;
					slot.r_level[i].level1[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.r_level[i].level1[j].upper_priority_level	= 0;
					slot.r_level[i].level1[j].lower_priority_level	= 0;
					slot.r_level[i].level2[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.r_level[i].level2[j].upper_priority_level	= 0;
					slot.r_level[i].level2[j].lower_priority_level	= 0;
				}

				// (Slave=1 or 2 or 3) and (port=1).
				if ( ( i == E_IM_MXIC_SPEC_ARBITER_1 || i == E_IM_MXIC_SPEC_ARBITER_2 || i == E_IM_MXIC_SPEC_ARBITER_3 ) &&
					 ( j == E_IM_MXIC_PORT_1 ) ) {
					slot.r_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
					slot.r_level[i].level0[j].upper_priority_level	= 2;
					slot.r_level[i].level0[j].lower_priority_level	= 7;
					slot.r_level[i].level1[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.r_level[i].level1[j].upper_priority_level	= 4;
					slot.r_level[i].level1[j].lower_priority_level	= 3;
					slot.r_level[i].level2[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.r_level[i].level2[j].upper_priority_level	= 3;
					slot.r_level[i].level2[j].lower_priority_level	= 0;
				}

				// (Slave=1 or 2 or 3) and (port=2).
				if ( ( i == E_IM_MXIC_SPEC_ARBITER_1 || i == E_IM_MXIC_SPEC_ARBITER_2 || i == E_IM_MXIC_SPEC_ARBITER_3 ) &&
					 ( j == E_IM_MXIC_PORT_2 ) ) {
					slot.r_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
					slot.r_level[i].level0[j].upper_priority_level	= 3;
					slot.r_level[i].level0[j].lower_priority_level	= 8;
					slot.r_level[i].level1[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.r_level[i].level1[j].upper_priority_level	= 5;
					slot.r_level[i].level1[j].lower_priority_level	= 3;
					slot.r_level[i].level2[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
					slot.r_level[i].level2[j].upper_priority_level	= 3;
					slot.r_level[i].level2[j].lower_priority_level	= 0;
				}

				// (Slave=4).
				if ( i == E_IM_MXIC_SPEC_ARBITER_4 ) {

					if ( j == E_IM_MXIC_PORT_0 ) {
						slot.r_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER_ONLY;
						slot.r_level[i].level0[j].upper_priority_level	= 6;
						slot.r_level[i].level0[j].lower_priority_level	= 19;
					}
					else {
						slot.r_level[i].level0[j].priority_level_type	= D_IM_MXIC_SLOT_PRIORITY_UPPER;
						slot.r_level[i].level0[j].upper_priority_level	= 0;
						slot.r_level[i].level0[j].lower_priority_level	= 0;
					}

					slot.r_level[i].level1[j].priority_level_type		= D_IM_MXIC_SLOT_PRIORITY_UPPER;

					if ( j == E_IM_MXIC_PORT_0 ) {
						slot.r_level[i].level1[j].upper_priority_level	= 7;
						slot.r_level[i].level1[j].lower_priority_level	= 12;
					}
					else {
						slot.r_level[i].level1[j].upper_priority_level	= 0;
						slot.r_level[i].level1[j].lower_priority_level	= 0;
					}

					slot.r_level[i].level2[j].priority_level_type		= D_IM_MXIC_SLOT_PRIORITY_UPPER;

					if ( j == E_IM_MXIC_PORT_0 ) {
						slot.r_level[i].level2[j].upper_priority_level	= 6;
						slot.r_level[i].level2[j].lower_priority_level	= 6;
					}
					else {
						slot.r_level[i].level2[j].upper_priority_level	= 0;
						slot.r_level[i].level2[j].lower_priority_level	= 0;
					}
				}
			}
		}

		(VOID)im_mxic_init_arbiter_assign( unit, &all_assign );

		im_mxic_on_pclk( unit );

		// Set configuration target arbiter.(All target arbiter initialize).
		// Configuration Start is executed by Im_MXIC_Set_Master_All_Arbiter() API proccess.
		if ( unit != E_IM_MXIC_UNIT_4 ) {
			io_mxic->TCFMD.bit.TENW_1	= 1;
			io_mxic->TCFMD.bit.TENW_2	= 1;
			io_mxic->TCFMD.bit.TENW_3	= 1;
			io_mxic->TCFMD.bit.TENW_4	= 1;
		}

		if ( unit != E_IM_MXIC_UNIT_0 ) {
			io_mxic->TCFMD.bit.TENR_1	= 1;
			io_mxic->TCFMD.bit.TENR_2	= 1;
			io_mxic->TCFMD.bit.TENR_3	= 1;
			io_mxic->TCFMD.bit.TENR_4	= 1;
		}

		if ( unit == E_IM_MXIC_UNIT_3 ) {
			io_mxic->TGCFMD.bit.TEGW_0	= 1;
			io_mxic->TGCFMD.bit.TEGW_1	= 1;
			io_mxic->TGCFMD.bit.TEGW_2	= 1;
			io_mxic->TGCFMD.bit.TEGW_3	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGW_4	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGW_5	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGW_6	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGW_7	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGR_0	= 1;
			io_mxic->TGCFMD.bit.TEGR_1	= 1;
			io_mxic->TGCFMD.bit.TEGR_2	= 1;
			io_mxic->TGCFMD.bit.TEGR_3	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGR_4	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGR_5	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGR_6	= 0;	// No assign(No target).
			io_mxic->TGCFMD.bit.TEGR_7	= 0;	// No assign(No target).
		}

		im_mxic_off_pclk( unit );

		if ( ( unit == E_IM_MXIC_UNIT_1 ) || ( unit == E_IM_MXIC_UNIT_2 ) || ( unit == E_IM_MXIC_UNIT_3 ) ) {

			(VOID)im_mxic_get_unit_tbl_address( unit, &io_mxic_tbl );

			if ( unit == E_IM_MXIC_UNIT_3 ) {
				DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.
			}

			// Initialize all TSL* register.
			im_mxic_on_hclk( unit );

			for ( i = 0; i < D_IM_MXIC_FULL_SPEC_PORT_NUM; i++ ) {

				for ( j = 0; j < D_IM_MXIC_FULL_SPEC_SLOT_NUM; j++ ) {

					for ( k = 0; k < D_IM_MXIC_FULL_SPEC_MASTER_NUM; k++ ) {

						io_mxic_tbl->TSLW_1[i].byte[j][k] = 0;	// Reserve code.
						io_mxic_tbl->TSLW_2[i].byte[j][k] = 0;	// Reserve code.
						io_mxic_tbl->TSLW_3[i].byte[j][k] = 0;	// Reserve code.
						io_mxic_tbl->TSLW_4[i].byte[j][k] = 0;	// Reserve code.
						io_mxic_tbl->TSLR_1[i].byte[j][k] = 0;	// Reserve code.
						io_mxic_tbl->TSLR_2[i].byte[j][k] = 0;	// Reserve code.
						io_mxic_tbl->TSLR_3[i].byte[j][k] = 0;	// Reserve code.
						io_mxic_tbl->TSLR_4[i].byte[j][k] = 0;	// Reserve code.
					}
				}
			}

			im_mxic_off_hclk( unit );

			if ( unit == E_IM_MXIC_UNIT_3 ) {
				DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.
			}
		}

		im_mxic_set_taen( unit, io_mxic, FALSE );	// TAEN-OFF.

		// Set all channel allocation.
		result = Im_MXIC_Set_Master_All_Arbiter( unit, &all_assign );

		if ( result == D_DDIM_OK ) {

			result = Im_MXIC_Set_Slot_Priority_All_Arbiter( unit, &slot );

			if ( result != D_DDIM_OK ) {
				Ddim_Print(("E:Im_MXIC_Init(): Im_MXIC_Set_Slot_Priority_All_Arbiter error. result=%d\n", result));
			}

			(VOID)im_mxic_init_all_levelport( unit, &all_levelport );

			result = Im_MXIC_Set_LevelPort_All( unit, &all_levelport );

			if ( result != D_DDIM_OK ) {
				Ddim_Print(("E:Im_MXIC_Init(): Im_MXIC_Set_LevelPort_All error. result=%d\n", result));
			}
		}
		else {
			Ddim_Print(("E:Im_MXIC_Init(): Im_MXIC_Set_Master_All_Arbiter error. result=%d\n", result));
		}

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Init(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function select arbiters of executed "configration start" and start configuration.<br>
@param [in]		unit							Target unit number.<br>
												Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param [in]		target_arbiter					Target arbiter of configration start.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
@remarks		Duplication setup of the master number to another port of the same slave including a read channel / write channel is prohibition.<br>
				In this case, this function returns the error.<br>
				And, the allocation setting of the master is returned to the setting operating now.
*/
INT32 Im_MXIC_Start_Config( E_IM_MXIC_UNIT unit, T_IM_MXIC_CONFIG_ARBITER const* const target_arbiter )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( target_arbiter == NULL ) {
		// target_arbiter parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Start_Config(): target_arbiter is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_wait_command_enable( unit, io_mxic );		// Wait TCFLG=1.

		im_mxic_on_pclk( unit );

		// Set configuration target arbiter.
		if ( unit != E_IM_MXIC_UNIT_4 ) {
			io_mxic->TCFMD.bit.TENW_1 = target_arbiter->sl_w1_config_on_flg;
			io_mxic->TCFMD.bit.TENW_2 = target_arbiter->sl_w2_config_on_flg;
			io_mxic->TCFMD.bit.TENW_3 = target_arbiter->sl_w3_config_on_flg;
			io_mxic->TCFMD.bit.TENW_4 = target_arbiter->sl_w4_config_on_flg;
		}
		if ( unit != E_IM_MXIC_UNIT_0 ) {
			io_mxic->TCFMD.bit.TENR_1 = target_arbiter->sl_r1_config_on_flg;
			io_mxic->TCFMD.bit.TENR_2 = target_arbiter->sl_r2_config_on_flg;
			io_mxic->TCFMD.bit.TENR_3 = target_arbiter->sl_r3_config_on_flg;
			io_mxic->TCFMD.bit.TENR_4 = target_arbiter->sl_r4_config_on_flg;
		}
		if ( unit == E_IM_MXIC_UNIT_3 ) {
			io_mxic->TGCFMD.bit.TEGW_0 = target_arbiter->gr_w0_config_on_flg;
			io_mxic->TGCFMD.bit.TEGW_1 = target_arbiter->gr_w1_config_on_flg;
			io_mxic->TGCFMD.bit.TEGW_2 = target_arbiter->gr_w2_config_on_flg;
			io_mxic->TGCFMD.bit.TEGW_3 = target_arbiter->gr_w3_config_on_flg;
			io_mxic->TGCFMD.bit.TEGW_4 = target_arbiter->gr_w4_config_on_flg;
			io_mxic->TGCFMD.bit.TEGW_5 = target_arbiter->gr_w5_config_on_flg;
			io_mxic->TGCFMD.bit.TEGW_6 = target_arbiter->gr_w6_config_on_flg;
			io_mxic->TGCFMD.bit.TEGW_7 = target_arbiter->gr_w7_config_on_flg;
			io_mxic->TGCFMD.bit.TEGR_0 = target_arbiter->gr_r0_config_on_flg;
			io_mxic->TGCFMD.bit.TEGR_1 = target_arbiter->gr_r1_config_on_flg;
			io_mxic->TGCFMD.bit.TEGR_2 = target_arbiter->gr_r2_config_on_flg;
			io_mxic->TGCFMD.bit.TEGR_3 = target_arbiter->gr_r3_config_on_flg;
			io_mxic->TGCFMD.bit.TEGR_4 = target_arbiter->gr_r4_config_on_flg;
			io_mxic->TGCFMD.bit.TEGR_5 = target_arbiter->gr_r5_config_on_flg;
			io_mxic->TGCFMD.bit.TEGR_6 = target_arbiter->gr_r6_config_on_flg;
			io_mxic->TGCFMD.bit.TEGR_7 = target_arbiter->gr_r7_config_on_flg;
		}
		// Configuration start.
		im_mxic_process_configuration( unit, io_mxic );

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Start_Config(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}


/**
This function set clock supply to a functional block. 
@param [in]		unit							Target unit number.<br>
												Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param [in]		clock							Clock enable setting.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Clock_Enable( E_IM_MXIC_UNIT unit, T_IM_MXIC_CLOCK const* const clock )
{
	UCHAR						tckenp_1;	// Write value of IO_MXIC.TCKENP.bit.TCKENP_1.
	UCHAR						tckenp_2;	// Write value of IO_MXIC.TCKENP.bit.TCKENP_2.
	UCHAR						tckenp_3;	// Write value of IO_MXIC.TCKENP.bit.TCKENP_3.
	UCHAR						tckenp_4;	// Write value of IO_MXIC.TCKENP.bit.TCKENP_4.
	UCHAR						tgcksen;	// Write value of IO_MXIC.TGCKEN.bit.TGCKSEN.
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( clock == NULL ) {
		// clock parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Clock_Enable(): clock is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {
		// Initialize write value of TCKENP, TGCKSEN.
		tckenp_1	= 0;
		tckenp_2	= 0;
		tckenp_3	= 0;
		tckenp_4	= 0;
		tgcksen		= 0;

		im_mxic_on_pclk( unit );

		// Decode error block.
		if ( clock->clk_en_dec_err == D_IM_MXIC_ON ) {
			io_mxic->TCKEN.bit.TCKSEN = 1;
		}
		else {
			io_mxic->TCKEN.bit.TCKSEN = 0;
		}
		// Monitor block.
		if (clock->clk_en_monitor == D_IM_MXIC_ON ) {
			io_mxic->TCKEN.bit.TCKMEN = 1;
		}
		else{
			io_mxic->TCKEN.bit.TCKMEN = 0;
		}

		// Slave-1 port-0 arbiter block.
		if ( clock->clk_en_slave_1_0 == D_IM_MXIC_ON ) {
			tckenp_1 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0;
		}
		// Slave-1 port-1 arbiter block.
		if ( clock->clk_en_slave_1_1 == D_IM_MXIC_ON ) {
			tckenp_1 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1;
		}
		// Slave-1 port-2 arbiter block.
		if ( clock->clk_en_slave_1_2 == D_IM_MXIC_ON ) {
			tckenp_1 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2;
		}
		// Slave-1 port-3 arbiter block.
		if ( clock->clk_en_slave_1_3 == D_IM_MXIC_ON ) {
			tckenp_1 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3;
		}
		io_mxic->TCKENP.bit.TCKENP_1 = tckenp_1;

		// Slave-2 port-0 arbiter block.
		if ( clock->clk_en_slave_2_0 == D_IM_MXIC_ON ) {
			tckenp_2 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0;
		}
		// Slave-2 port-1 arbiter block.
		if ( clock->clk_en_slave_2_1 == D_IM_MXIC_ON ) {
			tckenp_2 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1;
		}
		// Slave-2 port-2 arbiter block.
		if ( clock->clk_en_slave_2_2 == D_IM_MXIC_ON ) {
			tckenp_2 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2;
		}
		// Slave-2 port-3 arbiter block.
		if ( clock->clk_en_slave_2_3 == D_IM_MXIC_ON ) {
			tckenp_2 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3;
		}
		io_mxic->TCKENP.bit.TCKENP_2 = tckenp_2;

		// Slave-3 port-0 arbiter block.
		if ( clock->clk_en_slave_3_0 == D_IM_MXIC_ON ) {
			tckenp_3 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0;
		}
		// Slave-3 port-1 arbiter block.
		if ( clock->clk_en_slave_3_1 == D_IM_MXIC_ON ) {
			tckenp_3 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1;
		}
		// Slave-3 port-2 arbiter block.
		if ( clock->clk_en_slave_3_2 == D_IM_MXIC_ON ) {
			tckenp_3 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2;
		}
		// Slave-3 port-3 arbiter block.
		if ( clock->clk_en_slave_3_3 == D_IM_MXIC_ON ) {
			tckenp_3 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3;
		}
		io_mxic->TCKENP.bit.TCKENP_3 = tckenp_3;

		// Slave-4 port-0 arbiter block.
		if ( clock->clk_en_slave_4_0 == D_IM_MXIC_ON ) {
			tckenp_4 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0;
		}
		// Slave-4 port-1 arbiter block.
		if ( clock->clk_en_slave_4_1 == D_IM_MXIC_ON ) {
			tckenp_4 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1;
		}
		// Slave-4 port-2 arbiter block.
		if ( clock->clk_en_slave_4_2 == D_IM_MXIC_ON ) {
			tckenp_4 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2;
		}
		// Slave-4 port-3 arbiter block.
		if ( clock->clk_en_slave_4_3 == D_IM_MXIC_ON ) {
			tckenp_4 |= D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3;
		}
		io_mxic->TCKENP.bit.TCKENP_4 = tckenp_4;

		if ( unit == E_IM_MXIC_UNIT_3 ) {
			// Group-0 arbiter block
			if ( clock->clk_en_group_0 == D_IM_MXIC_ON ) {
				tgcksen |= D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_0;
			}
			// Group-1 arbiter block
			if ( clock->clk_en_group_1 == D_IM_MXIC_ON ) {
				tgcksen |= D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_1;
			}
			// Group-2 arbiter block
			if ( clock->clk_en_group_2 == D_IM_MXIC_ON ) {
				tgcksen |= D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_2;
			}
			// Group-3 arbiter block
			if ( clock->clk_en_group_3 == D_IM_MXIC_ON ) {
				tgcksen |= D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_3;
			}
			// Group-4 arbiter block
			if ( clock->clk_en_group_4 == D_IM_MXIC_ON ) {
				tgcksen |= D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_4;
			}
			// Group-5 arbiter block
			if ( clock->clk_en_group_5 == D_IM_MXIC_ON ) {
				tgcksen |= D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_5;
			}
			// Group-6 arbiter block
			if ( clock->clk_en_group_6 == D_IM_MXIC_ON ) {
				tgcksen |= D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_6;
			}
			// Group-7 arbiter block
			if ( clock->clk_en_group_7 == D_IM_MXIC_ON ) {
				tgcksen |= D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_7;
			}
			io_mxic->TGCKEN.bit.TGCKSEN = tgcksen;
		}

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Clock_Enable(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get clock supply to a functional block. 
@param [in]		unit							Target unit number.<br>
												Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param [out]	clock							Clock enable setting.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR		Fail - Parameter error.
*/

INT32 Im_MXIC_Get_Clock_Enable( E_IM_MXIC_UNIT unit, T_IM_MXIC_CLOCK* const clock )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if (clock == NULL ) {
		// clock parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Clock_Enable(): clock is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		// Decode error block.
		clock->clk_en_dec_err = io_mxic->TCKEN.bit.TCKSEN;

		// Monitor block.
		clock->clk_en_monitor = io_mxic->TCKEN.bit.TCKMEN;

		// Slave-1 port-0 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_1 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0 ) != 0 ) {
			clock->clk_en_slave_1_0 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_1_0 = D_IM_MXIC_OFF;
		}
		// Slave-1 port-1 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_1 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1 ) != 0 ) {
			clock->clk_en_slave_1_1 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_1_1 = D_IM_MXIC_OFF;
		}
		// Slave-1 port-2 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_1 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2 ) != 0 ) {
			clock->clk_en_slave_1_2 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_1_2 = D_IM_MXIC_OFF;
		}
		// Slave-1 port-3 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_1 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3 ) != 0 ) {
			clock->clk_en_slave_1_3 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_1_3 = D_IM_MXIC_OFF;
		}
		// Slave-2 port-0 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_2 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0 ) != 0 ) {
			clock->clk_en_slave_2_0 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_2_0 = D_IM_MXIC_OFF;
		}
		// Slave-2 port-1 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_2 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1 ) != 0 ) {
			clock->clk_en_slave_2_1 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_2_1 = D_IM_MXIC_OFF;
		}
		// Slave-2 port-2 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_2 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2 ) != 0 ) {
			clock->clk_en_slave_2_2 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_2_2 = D_IM_MXIC_OFF;
		}
		// Slave-2 port-3 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_2 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3 ) != 0 ) {
			clock->clk_en_slave_2_3 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_2_3 = D_IM_MXIC_OFF;
		}
		// Slave-3 port-0 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_3 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0 ) != 0 ) {
			clock->clk_en_slave_3_0 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_3_0 = D_IM_MXIC_OFF;
		}
		// Slave-3 port-1 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_3 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1 ) != 0 ) {
			clock->clk_en_slave_3_1 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_3_1 = D_IM_MXIC_OFF;
		}
		// Slave-3 port-2 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_3 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2 ) != 0 ) {
			clock->clk_en_slave_3_2 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_3_2 = D_IM_MXIC_OFF;
		}
		// Slave-3 port-3 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_3 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3 ) != 0 ) {
			clock->clk_en_slave_3_3 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_3_3 = D_IM_MXIC_OFF;
		}
		// Slave-4 port-0 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_4 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_0 ) != 0 ) {
			clock->clk_en_slave_4_0 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_4_0 = D_IM_MXIC_OFF;
		}
		// Slave-4 port-1 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_4 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_1 ) != 0 ) {
			clock->clk_en_slave_4_1 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_4_1 = D_IM_MXIC_OFF;
		}
		// Slave-4 port-2 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_4 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_2 ) != 0 ) {
			clock->clk_en_slave_4_2 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_4_2 = D_IM_MXIC_OFF;
		}
		// Slave-4 port-3 arbiter block.
		if ( ( io_mxic->TCKENP.bit.TCKENP_4 & D_IM_MXIC_TCKENP_TCKENP_SLAVE_x_3 ) != 0 ) {
			clock->clk_en_slave_4_3 = D_IM_MXIC_ON;
		}
		else {
			clock->clk_en_slave_4_3 = D_IM_MXIC_OFF;
		}

		if ( unit == E_IM_MXIC_UNIT_3 ) {
			// Group-0 arbiter block.
			if ( ( io_mxic->TGCKEN.bit.TGCKSEN & D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_0 ) != 0 ) {
				clock->clk_en_group_0 = D_IM_MXIC_ON;
			}
			else {
				clock->clk_en_group_0 = D_IM_MXIC_OFF;
			}
			// Group-1 arbiter block.
			if ( ( io_mxic->TGCKEN.bit.TGCKSEN & D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_1 ) != 0 ) {
				clock->clk_en_group_1 = D_IM_MXIC_ON;
			}
			else {
				clock->clk_en_group_1 = D_IM_MXIC_OFF;
			}
			// Group-2 arbiter block.
			if ( ( io_mxic->TGCKEN.bit.TGCKSEN & D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_2 ) != 0 ) {
				clock->clk_en_group_2 = D_IM_MXIC_ON;
			}
			else {
				clock->clk_en_group_2 = D_IM_MXIC_OFF;
			}
			// Group-3 arbiter block.
			if ( ( io_mxic->TGCKEN.bit.TGCKSEN & D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_3 ) != 0 ) {
				clock->clk_en_group_3 = D_IM_MXIC_ON;
			}
			else {
				clock->clk_en_group_3 = D_IM_MXIC_OFF;
			}
			// Group-4 arbiter block.
			if ( ( io_mxic->TGCKEN.bit.TGCKSEN & D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_4 ) != 0 ) {
				clock->clk_en_group_4 = D_IM_MXIC_ON;
			}
			else {
				clock->clk_en_group_4 = D_IM_MXIC_OFF;
			}
			// Group-5 arbiter block.
			if ( ( io_mxic->TGCKEN.bit.TGCKSEN & D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_5 ) != 0 ) {
				clock->clk_en_group_5 = D_IM_MXIC_ON;
			}
			else {
				clock->clk_en_group_5 = D_IM_MXIC_OFF;
			}
			// Group-6 arbiter block.
			if ( ( io_mxic->TGCKEN.bit.TGCKSEN & D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_6 ) != 0 ) {
				clock->clk_en_group_6 = D_IM_MXIC_ON;
			}
			else {
				clock->clk_en_group_6 = D_IM_MXIC_OFF;
			}
			// Group-7 arbiter block.
			if ( ( io_mxic->TGCKEN.bit.TGCKSEN & D_IM_MXIC_TGCKEN_TGCKSEN_GROUP_7 ) != 0 ) {
				clock->clk_en_group_7 = D_IM_MXIC_ON;
			}
			else {
				clock->clk_en_group_7 = D_IM_MXIC_OFF;
			}
		}
		else {
			clock->clk_en_group_0 = D_IM_MXIC_OFF;
			clock->clk_en_group_1 = D_IM_MXIC_OFF;
			clock->clk_en_group_2 = D_IM_MXIC_OFF;
			clock->clk_en_group_3 = D_IM_MXIC_OFF;
			clock->clk_en_group_4 = D_IM_MXIC_OFF;
			clock->clk_en_group_5 = D_IM_MXIC_OFF;
			clock->clk_en_group_6 = D_IM_MXIC_OFF;
			clock->clk_en_group_7 = D_IM_MXIC_OFF;
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Clock_Enable(): im_mxic_get_unit_address error.\n"));
	}
	return result;
}

/**
This function set acceptance capability of the specified port.<br> 
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value. <br>
@param[in]		capability				Acceptance capability. (1~8)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Acceptance_Capability( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, UCHAR capability )
{
	UCHAR						reg_read_sw;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_port( wr_arbiter, arbiter, port ) != D_DDIM_OK ) {
		// arbiter or port parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Acceptance_Capability(): im_mxic_check_port error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( im_mxic_check_acceptance_capability( capability ) != D_DDIM_OK ) {
		// capability parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Acceptance_Capability(): im_mxic_check_acceptance_capability error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {
		// If operation type is "set", wait TCFLG=1.
		im_mxic_wait_command_enable( unit, io_mxic );

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( unit != E_IM_MXIC_UNIT_4 ) {
				// Write arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						// arbiter1.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								io_mxic->TACPTCW.bit.TACPTCW_1_0 = capability;
								break;
							case E_IM_MXIC_PORT_1:
								io_mxic->TACPTCW.bit.TACPTCW_1_1 = capability;
								break;
							case E_IM_MXIC_PORT_2:
								io_mxic->TACPTCW.bit.TACPTCW_1_2 = capability;
								break;
							case E_IM_MXIC_PORT_3:
								io_mxic->TACPTCW.bit.TACPTCW_1_3 = capability;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						// arbiter2.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								io_mxic->TACPTCW.bit.TACPTCW_2_0 = capability;
								break;
							case E_IM_MXIC_PORT_1:
								io_mxic->TACPTCW.bit.TACPTCW_2_1 = capability;
								break;
							case E_IM_MXIC_PORT_2:
								io_mxic->TACPTCW.bit.TACPTCW_2_2 = capability;
								break;
							case E_IM_MXIC_PORT_3:
								io_mxic->TACPTCW.bit.TACPTCW_2_3 = capability;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						// arbiter3.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								io_mxic->TACPTCW.bit.TACPTCW_3_0 = capability;
								break;
							case E_IM_MXIC_PORT_1:
								io_mxic->TACPTCW.bit.TACPTCW_3_1 = capability;
								break;
							case E_IM_MXIC_PORT_2:
								io_mxic->TACPTCW.bit.TACPTCW_3_2 = capability;
								break;
							case E_IM_MXIC_PORT_3:
								io_mxic->TACPTCW.bit.TACPTCW_3_3 = capability;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						// arbiter4.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								io_mxic->TACPTCW.bit.TACPTCW_4_0 = capability;
								break;
							case E_IM_MXIC_PORT_1:
								io_mxic->TACPTCW.bit.TACPTCW_4_1 = capability;
								break;
							case E_IM_MXIC_PORT_2:
								io_mxic->TACPTCW.bit.TACPTCW_4_2 = capability;
								break;
							case E_IM_MXIC_PORT_3:
								io_mxic->TACPTCW.bit.TACPTCW_4_3 = capability;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): arbiter illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Set_Acceptance_Capability(): unit number is invalid. unit=%d\n", unit));
			}
		}
		else {
			if ( unit != E_IM_MXIC_UNIT_0 ) {
				// Read arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						// arbiter1.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								io_mxic->TACPTCR.bit.TACPTCR_1_0 = capability;
								break;
							case E_IM_MXIC_PORT_1:
								io_mxic->TACPTCR.bit.TACPTCR_1_1 = capability;
								break;
							case E_IM_MXIC_PORT_2:
								io_mxic->TACPTCR.bit.TACPTCR_1_2 = capability;
								break;
							case E_IM_MXIC_PORT_3:
								io_mxic->TACPTCR.bit.TACPTCR_1_3 = capability;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						// arbiter2.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								io_mxic->TACPTCR.bit.TACPTCR_2_0 = capability;
								break;
							case E_IM_MXIC_PORT_1:
								io_mxic->TACPTCR.bit.TACPTCR_2_1 = capability;
								break;
							case E_IM_MXIC_PORT_2:
								io_mxic->TACPTCR.bit.TACPTCR_2_2 = capability;
								break;
							case E_IM_MXIC_PORT_3:
								io_mxic->TACPTCR.bit.TACPTCR_2_3 = capability;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						// arbiter3
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								io_mxic->TACPTCR.bit.TACPTCR_3_0 = capability;
								break;
							case E_IM_MXIC_PORT_1:
								io_mxic->TACPTCR.bit.TACPTCR_3_1 = capability;
								break;
							case E_IM_MXIC_PORT_2:
								io_mxic->TACPTCR.bit.TACPTCR_3_2 = capability;
								break;
							case E_IM_MXIC_PORT_3:
								io_mxic->TACPTCR.bit.TACPTCR_3_3 = capability;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						// arbiter4
						switch (port) {
							case E_IM_MXIC_PORT_0:
								io_mxic->TACPTCR.bit.TACPTCR_4_0 = capability;
								break;
							case E_IM_MXIC_PORT_1:
								io_mxic->TACPTCR.bit.TACPTCR_4_1 = capability;
								break;
							case E_IM_MXIC_PORT_2:
								io_mxic->TACPTCR.bit.TACPTCR_4_2 = capability;
								break;
							case E_IM_MXIC_PORT_3:
								io_mxic->TACPTCR.bit.TACPTCR_4_3 = capability;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): arbiter illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Set_Acceptance_Capability(): unit number is invalid. unit=%d\n", unit));
			}
		}

		// Configuration start.
		im_mxic_process_configuration( unit, io_mxic );

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get acceptance capability of the specified port.<br> 
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value. <br>
@param[out]		capability				Acceptance capability. (1~8)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Acceptance_Capability( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, UCHAR* capability )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_port( wr_arbiter, arbiter, port) != D_DDIM_OK ) {
		// arbiter or port parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_Acceptance_Capability(): im_mxic_check_port error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( capability == NULL ) {
		// capability parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Acceptance_Capability(): capability is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_wait_command_enable( unit, io_mxic );		// Wait TCFLG=1.

		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {
			// Write arbiter.
			im_mxic_on_pclk( unit );

			if ( unit != E_IM_MXIC_UNIT_4 ) {

				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						// arbiter1.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_1_0;
								break;
							case E_IM_MXIC_PORT_1:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_1_1;
								break;
							case E_IM_MXIC_PORT_2:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_1_2;
								break;
							case E_IM_MXIC_PORT_3:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_1_3;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						// arbiter2.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_2_0;
								break;
							case E_IM_MXIC_PORT_1:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_2_1;
								break;
							case E_IM_MXIC_PORT_2:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_2_2;
								break;
							case E_IM_MXIC_PORT_3:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_2_3;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						// arbiter3.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_3_0;
								break;
							case E_IM_MXIC_PORT_1:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_3_1;
								break;
							case E_IM_MXIC_PORT_2:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_3_2;
								break;
							case E_IM_MXIC_PORT_3:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_3_3;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						// arbiter4.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_4_0;
								break;
							case E_IM_MXIC_PORT_1:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_4_1;
								break;
							case E_IM_MXIC_PORT_2:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_4_2;
								break;
							case E_IM_MXIC_PORT_3:
								*capability = io_mxic->TACPTCW.bit.TACPTCW_4_3;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): arbiter illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				*capability = 0;

				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Get_Acceptance_Capability(): unit number is invalid. unit=%d\n", unit));
			}

			im_mxic_off_pclk( unit );
		}
		else {
			// Read arbiter.
			im_mxic_on_pclk( unit );

			if ( unit != E_IM_MXIC_UNIT_0 ) {

				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						// arbiter1.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_1_0;
								break;
							case E_IM_MXIC_PORT_1:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_1_1;
								break;
							case E_IM_MXIC_PORT_2:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_1_2;
								break;
							case E_IM_MXIC_PORT_3:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_1_3;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						// arbiter2.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_2_0;
								break;
							case E_IM_MXIC_PORT_1:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_2_1;
								break;
							case E_IM_MXIC_PORT_2:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_2_2;
								break;
							case E_IM_MXIC_PORT_3:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_2_3;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						// arbiter3.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_3_0;
								break;
							case E_IM_MXIC_PORT_1:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_3_1;
								break;
							case E_IM_MXIC_PORT_2:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_3_2;
								break;
							case E_IM_MXIC_PORT_3:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_3_3;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						// arbiter4.
						switch ( port ) {
							case E_IM_MXIC_PORT_0:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_4_0;
								break;
							case E_IM_MXIC_PORT_1:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_4_1;
								break;
							case E_IM_MXIC_PORT_2:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_4_2;
								break;
							case E_IM_MXIC_PORT_3:
								*capability = io_mxic->TACPTCR.bit.TACPTCR_4_3;
								break;
							default:
								// Port parameter is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): port illegal. port=%d\n", port));
								break;
						}
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): arbiter illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				*capability = 0;

				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Get_Acceptance_Capability(): unit number is invalid. unit=%d\n", unit));
			}

			im_mxic_off_pclk( unit );
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set acceptance capability (gropu) of the specified port.<br> 
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER_GR for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT_GR for a set value. <br>
@param[in]		capability				Acceptance capability. (1~8)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Acceptance_Capability_Group( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER_GR arbiter, E_IM_MXIC_PORT_GR port, UCHAR capability )
{
	UCHAR	reg_read_sw;
	INT32	result;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_port_group( wr_arbiter, arbiter, port ) != D_DDIM_OK ) {
		// arbiter or port parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Acceptance_Capability_Group(): im_mxic_check_port error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( im_mxic_check_acceptance_capability( capability ) != D_DDIM_OK ) {
		// capability parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Acceptance_Capability_Group(): im_mxic_check_acceptance_capability error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = D_DDIM_OK;

	im_mxic_wait_command_enable( E_IM_MXIC_UNIT_3, (struct io_jdsmxic *)&IO_MXIC3 );	// Wait TCFLG=1.

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	reg_read_sw = IO_MXIC3.TRVSW.bit.TRVSW;

	if ( reg_read_sw == D_IM_MXIC_ON ) {
		IO_MXIC3.TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
	}

	if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {
		// Write arbiter.
		switch ( arbiter ) {
			case E_IM_MXIC_SPEC_ARBITER_GR_0:
				// arbiter0.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_0_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_0_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_1:
				// arbiter1.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_1_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_1_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_2:
				// arbiter2.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_2_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_2_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_3:
				// arbiter3.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_3_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_3_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_4:
				// arbiter4.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_4_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_4_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_5:
				// arbiter5.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_5_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_5_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_6:
				// arbiter6.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_6_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_6_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_7:
				// arbiter7.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_7_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCW.bit.TGACPTCW_7_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			default:
				// Port parameter is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): arbiter illegal. arbiter=%d\n", arbiter));
				break;
		}
	}
	else {
		// Read arbiter.
		switch ( arbiter ) {
			case E_IM_MXIC_SPEC_ARBITER_GR_0:
				// arbiter0.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_0_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_0_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_1:
				// arbiter1.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_1_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_1_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_2:
				// arbiter2.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_2_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_2_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_3:
				// arbiter3.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_3_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_3_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_4:
				// arbiter4.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_4_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_4_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_5:
				// arbiter5.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_5_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_5_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_6:
				// arbiter6.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_6_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_6_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_7:
				// arbiter7.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_7_0 = capability;
						break;
					case E_IM_MXIC_PORT_GR_1:
						IO_MXIC3.TGACPTCR.bit.TGACPTCR_7_1 = capability;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			default:
				// Port parameter is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_Group(): arbiter illegal. arbiter=%d\n", arbiter));
				break;
		}
	}

	// Configuration start.
	im_mxic_process_configuration( E_IM_MXIC_UNIT_3, &IO_MXIC3 );

	if ( reg_read_sw == D_IM_MXIC_ON ) {
		IO_MXIC3.TRVSW.bit.TRVSW = D_IM_MXIC_ON;
	}

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	Im_MXIC_Dsb();

	return result;
}

/**
This function get acceptance capability (gropu) of the specified port.<br> 
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER_GR for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT_GR for a set value. <br>
@param[out]		capability				Acceptance capability. (1~8)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Acceptance_Capability_Group( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER_GR arbiter, E_IM_MXIC_PORT_GR port, UCHAR* capability )
{
	INT32	result;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_port_group( wr_arbiter, arbiter, port ) != D_DDIM_OK ) {
		// arbiter or port parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_Acceptance_Capability_Group(): im_mxic_check_port error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( capability == NULL ) {
		// capability parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Acceptance_Capability_Group(): capability is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = D_DDIM_OK;

	im_mxic_wait_command_enable( E_IM_MXIC_UNIT_3, (struct io_jdsmxic *)&IO_MXIC3 );	// Wait TCFLG=1.

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {
		// Write arbiter.
		switch ( arbiter ) {
			case E_IM_MXIC_SPEC_ARBITER_GR_0:
				// arbiter0.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_0_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_0_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_1:
				// arbiter1.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_1_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_1_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_2:
				// arbiter2.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_2_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_2_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_3:
				// arbiter3.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_3_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_3_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_4:
				// arbiter4.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_4_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_4_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_5:
				// arbiter5.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_5_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_5_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_6:
				// arbiter6.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_6_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_6_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_7:
				// arbiter7.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_7_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCW.bit.TGACPTCW_7_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			default:
				// Port parameter is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): arbiter illegal. arbiter=%d\n", arbiter));
				break;
		}
	}
	else {
		// Read arbiter.
		switch ( arbiter ) {
			case E_IM_MXIC_SPEC_ARBITER_GR_0:
				// arbiter0.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_0_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_0_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_1:
				// arbiter1.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_1_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_1_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_2:
				// arbiter2.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_2_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_2_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_3:
				// arbiter3.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_3_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_3_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_4:
				// arbiter4.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_4_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_4_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_5:
				// arbiter5.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_5_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_5_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_6:
				// arbiter6.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_6_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_6_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			case E_IM_MXIC_SPEC_ARBITER_GR_7:
				// arbiter7.
				switch ( port ) {
					case E_IM_MXIC_PORT_GR_0:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_7_0;
						break;
					case E_IM_MXIC_PORT_GR_1:
						*capability = IO_MXIC3.TGACPTCR.bit.TGACPTCR_7_1;
						break;
					default:
						// Port parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): port illegal. port=%d\n", port));
						break;
				}
				break;
			default:
				// Port parameter is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_Group(): arbiter illegal. arbiter=%d\n", arbiter));
				break;
		}
	}

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	return result;
}

/**
This function set acceptance capability of all ports.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		all_capability			Acceptance capability of all ports.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Acceptance_Capability_All_Port( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY const* const all_capability )
{
	INT32	result;
	INT32	i, j;

#ifdef CO_PARAM_CHECK
	if ( all_capability == NULL ) {
		// all_capability parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Acceptance_Capability_All_Port(): all_capability is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = D_DDIM_OK;

	// Setting of W ports.
	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {
		// Set acceptance capability of all W ports.
		for (j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++) {
			result = Im_MXIC_Set_Acceptance_Capability( unit, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, all_capability->w_capability[i][j] );
			if (result != D_DDIM_OK) {
				// Parameter error.
				Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_All_Port(): Im_MXIC_Set_Acceptance_Capability error. (Write)\n"));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}

	// Setting of R ports.
	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {
		// Set acceptance capability of all R ports.
		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {
			result = Im_MXIC_Set_Acceptance_Capability( unit, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, all_capability->r_capability[i][j] );
			if (result != D_DDIM_OK) {
				// Parameter error.
				Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_All_Port(): Im_MXIC_Set_Acceptance_Capability error. (Read)\n"));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}

	// Setting of Group-W ports.
	for ( i = E_IM_MXIC_W_ARBITER_GR_W0; i < E_IM_MXIC_W_ARBITER_GR_MAX; i++ ) {
		// Set acceptance capability of all W ports.
		for ( j = E_IM_MXIC_PORT_GR_0; j < E_IM_MXIC_PORT_GR_MAX; j++ ) {
			result = Im_MXIC_Set_Acceptance_Capability_Group(E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER_GR)i, (E_IM_MXIC_PORT_GR)j, all_capability->w_capability_gr[i][j] );
			if ( result != D_DDIM_OK ) {
				// Parameter error.
				Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_All_Port(): Im_MXIC_Set_Acceptance_Capability_Group error. (Write)\n"));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}

	// Setting of Group-R ports.
	for ( i = E_IM_MXIC_R_ARBITER_GR_R0; i < E_IM_MXIC_R_ARBITER_GR_MAX; i++ ) {
		// Set acceptance capability of all R ports.
		for (j = E_IM_MXIC_PORT_GR_0; j < E_IM_MXIC_PORT_GR_MAX; j++) {
			result = Im_MXIC_Set_Acceptance_Capability_Group( E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER_GR)i, (E_IM_MXIC_PORT_GR)j, all_capability->r_capability_gr[i][j] );
			if (result != D_DDIM_OK) {
				// Parameter error.
				Ddim_Print(("E:Im_MXIC_Set_Acceptance_Capability_All_Port(): Im_MXIC_Set_Acceptance_Capability_Group error. (Read)\n"));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}

	Im_MXIC_Dsb();

	return D_DDIM_OK;
}

/**
This function get acceptance capability of all ports.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_capability			Acceptance capability of all ports.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Acceptance_Capability_All_Port( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ACCEPTANCE_CAPABILITY* const all_capability )
{
	INT32	result;
	INT32	i, j;

#ifdef CO_PARAM_CHECK
	if ( all_capability == NULL ) {
		// all_capability parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Acceptance_Capability_All_Port(): all_capability is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = D_DDIM_OK;

	// Setting of W ports.
	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {
		// Get acceptance capability of all W ports.
		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {
			result = Im_MXIC_Get_Acceptance_Capability( unit, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, &all_capability->w_capability[i][j] );
			if ( result != D_DDIM_OK ) {
				// Parameter error.
				Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_All_Port(): Im_MXIC_Get_Acceptance_Capability error. (Write)\n"));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}

	// Setting of R ports.
	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {
		// Get acceptance capability of all R ports.
		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {
			result = Im_MXIC_Get_Acceptance_Capability( unit, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, &all_capability->r_capability[i][j] );
			if ( result != D_DDIM_OK ) {
				// Parameter error.
				Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_All_Port(): Im_MXIC_Get_Acceptance_Capability error. (Read)\n"));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}

	// Setting of Group-W ports.
	for ( i = E_IM_MXIC_W_ARBITER_GR_W0; i < E_IM_MXIC_W_ARBITER_GR_MAX; i++ ) {
		// Get acceptance capability of all W ports.
		for ( j = E_IM_MXIC_PORT_GR_0; j < E_IM_MXIC_PORT_GR_MAX; j++ ) {
			result = Im_MXIC_Get_Acceptance_Capability_Group( E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER_GR)i, (E_IM_MXIC_PORT_GR)j, &all_capability->w_capability_gr[i][j] );
			if ( result != D_DDIM_OK ) {
				// Parameter error.
				Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_All_Port(): Im_MXIC_Get_Acceptance_Capability_Group error. (Write)\n"));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}

	// Setting of Group-R ports.
	for ( i = E_IM_MXIC_R_ARBITER_GR_R0; i < E_IM_MXIC_R_ARBITER_GR_MAX; i++ ) {
		// Get acceptance capability of all R ports.
		for ( j = E_IM_MXIC_PORT_GR_0; j < E_IM_MXIC_PORT_GR_MAX; j++ ) {
			result = Im_MXIC_Get_Acceptance_Capability_Group( E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER_GR)i, (E_IM_MXIC_PORT_GR)j, &all_capability->r_capability_gr[i][j] );
			if ( result != D_DDIM_OK ) {
				// Parameter error.
				Ddim_Print(("E:Im_MXIC_Get_Acceptance_Capability_All_Port(): Im_MXIC_Get_Acceptance_Capability_Group error. (Read)\n"));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}

	return D_DDIM_OK;
}

/**
This function set slave area setting of specified slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		slave_number			Slave area number.<br>
										Please refer to @ref E_IM_MXIC_SLAVE_NUMBER for a set value. <br>
@param[in]		slave_area				Slave area setting.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Slave_Area( E_IM_MXIC_UNIT unit, E_IM_MXIC_SLAVE_NUMBER slave_number, T_IM_MXIC_SLAVE_AREA const* const slave_area )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		// Set slave area.
		result = im_mxic_set_slave_area_reg( unit, io_mxic, slave_number, slave_area );

		if ( result == D_DDIM_OK ) {
			// Change of a setup is reflected.
			im_mxic_on_pclk( unit );

			io_mxic->TSMTRG.bit.TSMTRG = 1;

			im_mxic_off_pclk( unit );
		}
		else {
			// Input parameter error.
			Ddim_Print(("E:Im_MXIC_Set_Slave_Area(): im_mxic_set_slave_area_reg error.\n"));
		}

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Slave_Area(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get slave area setting of specified slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		slave_number			Slave area number.<br>
										Please refer to @ref E_IM_MXIC_SLAVE_NUMBER for a set value. <br>
@param[out]		slave_area				Slave area setting.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Slave_Area( E_IM_MXIC_UNIT unit, E_IM_MXIC_SLAVE_NUMBER slave_number, T_IM_MXIC_SLAVE_AREA* const slave_area )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {
		// Get slave area setting.
		result = im_mxic_get_slave_area_reg( unit, io_mxic, slave_number, slave_area );

		if ( result != D_DDIM_OK ) {
			// Input parameter error
			Ddim_Print(("E:Im_MXIC_Get_Slave_Area(): im_mxic_get_slave_area_reg error.\n"));
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Slave_Area(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set slave area setting of all slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		all_slave_area			Slave area setting of all slave number.<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
INT32 Im_MXIC_Set_Slave_Area_All( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLAVE_AREA const* const all_slave_area )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;
	INT32						i;

#ifdef CO_PARAM_CHECK
	if ( all_slave_area == NULL ) {
		// all_slave_area parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Slave_Area_All(): all_slave_area is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {
		// Set all slave area.
		for ( i = E_IM_MXIC_SLAVE_NUMBER_1; i < E_IM_MXIC_SLAVE_NUMBER_MAX; i++ ) {

			result = im_mxic_set_slave_area_reg( unit, io_mxic, (E_IM_MXIC_SLAVE_NUMBER)i, &all_slave_area->slave_area[i] );

			if ( result != D_DDIM_OK ) {
				// Input parameter error.
				Ddim_Print(("E:Im_MXIC_Set_Slave_Area_All(): im_mxic_set_slave_area_reg error.\n"));
				return result;
			}
		}
		// Change of a setup is reflected.
		im_mxic_on_pclk( unit );

		io_mxic->TSMTRG.bit.TSMTRG = 1;

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Slave_Area_All(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get slave area setting of all slave number.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_slave_area			Slave area setting of all slave number.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Slave_Area_All( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLAVE_AREA* const all_slave_area )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;
	INT32						i;

#ifdef CO_PARAM_CHECK
	if ( all_slave_area == NULL ) {
		// all_slave_area parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slave_Area_All(): all_slave_area is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {
		// Get all slave area.
		for ( i = E_IM_MXIC_SLAVE_NUMBER_1; i < E_IM_MXIC_SLAVE_NUMBER_MAX; i++ ) {
			result = im_mxic_get_slave_area_reg( unit, io_mxic, (E_IM_MXIC_SLAVE_NUMBER)i, &all_slave_area->slave_area[i] );
			if ( result != D_DDIM_OK ) {
				// Input parameter error.
				Ddim_Print(("E:Im_MXIC_Get_Slave_Area_All(): im_mxic_get_slave_area_reg error.\n"));
				return result;
			}
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Slave_Area_All(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}


/**
This function set interruption setting of salave area decode error.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		dec_err_int				Setting of decoding error interruption.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please acquire error status with an Im_MXIC_Get_Decode_Error function 
				at the time of decode error interruption generating. 
*/
INT32 Im_MXIC_Set_Decode_Error_Int( E_IM_MXIC_UNIT unit, T_IM_MXIC_DEC_ERR_INT const* const dec_err_int )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( dec_err_int == NULL ) {
		// dec_err_int parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Decode_Error_Int(): dec_err_int is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {
		// Set interrupt enable
		im_mxic_on_pclk( unit );

		io_mxic->TMIE.bit.TDEWE = dec_err_int->w_err_int_en;
		io_mxic->TMIE.bit.TDERE = dec_err_int->r_err_int_en;

		im_mxic_off_pclk( unit );

		// Set callback function
		gIM_MXIC_Dec_Err_Callback[unit] = dec_err_int->pCallBack;

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Decode_Error_Int(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get interruption setting of salave area decode error.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		dec_err_int				Setting of decoding error interruption.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Decode_Error_Int( E_IM_MXIC_UNIT unit, T_IM_MXIC_DEC_ERR_INT* const dec_err_int )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if (dec_err_int == NULL) {
		// dec_err_int parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Decode_Error_Int(): dec_err_int is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {
		// Get interrupt enable
		im_mxic_on_pclk( unit );

		dec_err_int->w_err_int_en = io_mxic->TMIE.bit.TDEWE;
		dec_err_int->r_err_int_en = io_mxic->TMIE.bit.TDERE;

		im_mxic_off_pclk( unit );

		// Get callback function
		dec_err_int->pCallBack = gIM_MXIC_Dec_Err_Callback[unit];
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Decode_Error_Int(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get decode error status.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		status					Decode error status.<br>
@remarks		Decode error will be notifed by the callback function that set up with the Im_MXIC_Set_Decode_Error_Int function.<br>
				Please acquire error status with Im_MXIC_Get_Decode_Error function at this time.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Decode_Error( E_IM_MXIC_UNIT unit, T_IM_MXIC_DEC_ERR* const status )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( status == NULL ) {
		// status parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Decode_Error(): status is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		if ( unit != E_IM_MXIC_UNIT_4 ) {
			// Write channel.
			status->w_req_addr		= io_mxic->TDESTW.bit.TDEADW;
			status->w_req_master	= io_mxic->TDESTW.bit.TDEMNW;
		}
		else {
			status->w_req_addr		= 0;
			status->w_req_master	= 0;

			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Get_Decode_Error(): unit number is invalid. unit=%d\n", unit));
		}

		if ( unit != E_IM_MXIC_UNIT_0 ) {
			// Read channel.
			status->r_req_addr		= io_mxic->TDESTR.bit.TDEADR;
			status->r_req_master	= io_mxic->TDESTR.bit.TDEMNR;
		}
		else {
			status->r_req_addr		= 0;
			status->r_req_master	= 0;

			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Get_Decode_Error(): unit number is invalid. unit=%d\n", unit));
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Decode_Error(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the W master to the W arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		arbiter					Target write arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting changed by this function is reflected by the configration start.
@remarks		D_IM_MXIC_W_RESERVE is set to the part where the channel is not set up without fail.
*/
INT32 Im_MXIC_Set_Master_W_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT const* const assign )
{
	INT32							result;
	volatile struct io_jdsmxic*		io_mxic;
	volatile struct io_jdsmxic_tbl*	io_mxic_tbl;

#ifdef CO_PARAM_CHECK
	if (im_mxic_check_set_master_w_arbiter_param( arbiter, assign ) != D_DDIM_OK ) {
		// Input parameter error.
		Ddim_Assertion(("E:Im_MXIC_Set_Master_W_Arbiter(): im_mxic_check_set_master_w_arbiter_param error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		result = im_mxic_get_unit_tbl_address( unit, &io_mxic_tbl );

		if ( result == D_DDIM_OK ) {

			im_mxic_wait_command_enable( unit, io_mxic );	// Wait TCFLG=1.

			im_mxic_set_taen( unit, io_mxic, TRUE );		// TAEN-ON.

			result = im_mxic_set_master_w_arbiter_reg( unit, io_mxic, io_mxic_tbl, arbiter, assign );

			if ( result == D_DDIM_OK ) {

				im_mxic_process_configuration( unit, io_mxic );

			}

			im_mxic_set_taen( unit, io_mxic, FALSE );	// TAEN-OFF.
		}
		else {
			// Input parameter error.
//			Ddim_Print(("E:Im_MXIC_Set_Master_W_Arbiter(): im_mxic_get_unit_tbl_address error.\n"));
			result = D_DDIM_OK;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Master_W_Arbiter(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the W master from the W arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		arbiter					Target write arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value.<br>
@param[out]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_W_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT* const assign )
{
	INT32							i, j;
	INT32							result;
	volatile struct io_jdsmxic*		io_mxic;
	volatile struct io_jdsmxic_tbl*	io_mxic_tbl;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_get_master_w_arbiter_param( arbiter, assign ) != D_DDIM_OK ) {
		// Input parameter error.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_W_Arbiter(): im_mxic_check_get_master_w_arbiter_param error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		result = im_mxic_get_unit_tbl_address( unit, &io_mxic_tbl );

		if ( result == D_DDIM_OK ) {

			im_mxic_wait_command_enable( unit, io_mxic );	// Wait TCFLG=1.

			im_mxic_set_taen( unit, io_mxic, TRUE );		// TAEN-ON

			if ( unit == E_IM_MXIC_UNIT_3 ) {
				DDIM_User_AhbReg_SpinLock();					// JPEG issue work around.
			}

			im_mxic_on_hclk( unit );

			for ( i = E_IM_MXIC_PORT_0; i < E_IM_MXIC_PORT_MAX; i++ ) {

				for ( j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++ ) {

					switch ( arbiter ) {
						case E_IM_MXIC_W_ARBITER_W1:
							assign->port[i].slot0[j] = io_mxic_tbl->TSLW_1[i].byte[E_IM_MXIC_SLOT_0][j];
							assign->port[i].slot1[j] = io_mxic_tbl->TSLW_1[i].byte[E_IM_MXIC_SLOT_1][j];
							assign->port[i].slot2[j] = io_mxic_tbl->TSLW_1[i].byte[E_IM_MXIC_SLOT_2][j];
							assign->port[i].slot3[j] = io_mxic_tbl->TSLW_1[i].byte[E_IM_MXIC_SLOT_3][j];
							break;
						case E_IM_MXIC_W_ARBITER_W2:
							assign->port[i].slot0[j] = io_mxic_tbl->TSLW_2[i].byte[E_IM_MXIC_SLOT_0][j];
							assign->port[i].slot1[j] = io_mxic_tbl->TSLW_2[i].byte[E_IM_MXIC_SLOT_1][j];
							assign->port[i].slot2[j] = io_mxic_tbl->TSLW_2[i].byte[E_IM_MXIC_SLOT_2][j];
							assign->port[i].slot3[j] = io_mxic_tbl->TSLW_2[i].byte[E_IM_MXIC_SLOT_3][j];
							break;
						case E_IM_MXIC_W_ARBITER_W3:
							assign->port[i].slot0[j] = io_mxic_tbl->TSLW_3[i].byte[E_IM_MXIC_SLOT_0][j];
							assign->port[i].slot1[j] = io_mxic_tbl->TSLW_3[i].byte[E_IM_MXIC_SLOT_1][j];
							assign->port[i].slot2[j] = io_mxic_tbl->TSLW_3[i].byte[E_IM_MXIC_SLOT_2][j];
							assign->port[i].slot3[j] = io_mxic_tbl->TSLW_3[i].byte[E_IM_MXIC_SLOT_3][j];
							break;
						case E_IM_MXIC_W_ARBITER_W4:
							assign->port[i].slot0[j] = io_mxic_tbl->TSLW_4[i].byte[E_IM_MXIC_SLOT_0][j];
							assign->port[i].slot1[j] = io_mxic_tbl->TSLW_4[i].byte[E_IM_MXIC_SLOT_1][j];
							assign->port[i].slot2[j] = io_mxic_tbl->TSLW_4[i].byte[E_IM_MXIC_SLOT_2][j];
							assign->port[i].slot3[j] = io_mxic_tbl->TSLW_4[i].byte[E_IM_MXIC_SLOT_3][j];
							break;
						default:
							// arbiter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Get_Master_W_Arbiter(): arbiter is illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
			}

			im_mxic_off_hclk( unit );

			if ( unit == E_IM_MXIC_UNIT_3 ) {
				DDIM_User_AhbReg_SpinUnLock();				// JPEG issue work around.
			}

			im_mxic_set_taen( unit, io_mxic, FALSE );	// TAEN-OFF.
		}
		else {
			// Input parameter error.
//			Ddim_Print(("E:Im_MXIC_Get_Master_W_Arbiter(): im_mxic_get_unit_tbl_address error.\n"));
			result = D_DDIM_OK;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Master_W_Arbiter(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}


/**
This function set the R master to the R arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		arbiter					Target read arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value.<br>
@param[in]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting changed by this function is reflected by the configration start.
@remarks		D_IM_MXIC_R_RESERVE is set to the part where the channel is not set up without fail.
*/
INT32 Im_MXIC_Set_Master_R_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT const* const assign )
{
	INT32							result;
	volatile struct io_jdsmxic*		io_mxic;
	volatile struct io_jdsmxic_tbl*	io_mxic_tbl;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_set_master_r_arbiter_param( arbiter, assign ) != D_DDIM_OK ) {
		// Input parameter error
		Ddim_Assertion(("E:Im_MXIC_Set_Master_R_Arbiter(): im_mxic_check_set_master_r_arbiter_param error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		result = im_mxic_get_unit_tbl_address( unit, &io_mxic_tbl );

		if ( result == D_DDIM_OK ) {

			im_mxic_wait_command_enable( unit, io_mxic );	// Wait TCFLG=1.

			im_mxic_set_taen( unit, io_mxic, TRUE );		// TAEN-ON.

			result = im_mxic_set_master_r_arbiter_reg( unit, io_mxic, io_mxic_tbl, arbiter, assign );

			if ( result == D_DDIM_OK ) {

				// Configuration start.
				im_mxic_process_configuration( unit, io_mxic );
			}

			im_mxic_set_taen( unit, io_mxic, FALSE );	// TAEN-OFF.
		}
		else {
			// Input parameter error.
//			Ddim_Print(("E:Im_MXIC_Set_Master_R_Arbiter(): im_mxic_get_unit_tbl_address error.\n"));
			result = D_DDIM_OK;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Master_R_Arbiter(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the R master from the R arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		arbiter					Target read arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value.<br>
@param[out]		assign					Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_R_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT* const assign )
{
	INT32							i, j;
	INT32							result;
	volatile struct io_jdsmxic*		io_mxic;
	volatile struct io_jdsmxic_tbl*	io_mxic_tbl;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_get_master_r_arbiter_param( arbiter, assign ) != D_DDIM_OK ) {
		// Input parameter error.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_R_Arbiter(): im_mxic_check_get_master_r_arbiter_param error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		result = im_mxic_get_unit_tbl_address( unit, &io_mxic_tbl );

		if ( result == D_DDIM_OK ) {

			im_mxic_wait_command_enable( unit, io_mxic );	// Wait TCFLG=1.

			im_mxic_set_taen( unit, io_mxic, TRUE );		// TAEN-ON.

			if ( unit == E_IM_MXIC_UNIT_3 ) {
				DDIM_User_AhbReg_SpinLock();					// JPEG issue work around.
			}

			im_mxic_on_hclk( unit );

			for ( i = E_IM_MXIC_PORT_0; i < E_IM_MXIC_PORT_MAX; i++ ) {

				for ( j = 0; j < D_IM_MXIC_SLOT_SIZE_8; j++ ) {

					switch ( arbiter ) {
						case E_IM_MXIC_R_ARBITER_R1:
							assign->port[i].slot0[j] = io_mxic_tbl->TSLR_1[i].byte[E_IM_MXIC_SLOT_0][j];
							assign->port[i].slot1[j] = io_mxic_tbl->TSLR_1[i].byte[E_IM_MXIC_SLOT_1][j];
							assign->port[i].slot2[j] = io_mxic_tbl->TSLR_1[i].byte[E_IM_MXIC_SLOT_2][j];
							assign->port[i].slot3[j] = io_mxic_tbl->TSLR_1[i].byte[E_IM_MXIC_SLOT_3][j];
							break;
						case E_IM_MXIC_R_ARBITER_R2:
							assign->port[i].slot0[j] = io_mxic_tbl->TSLR_2[i].byte[E_IM_MXIC_SLOT_0][j];
							assign->port[i].slot1[j] = io_mxic_tbl->TSLR_2[i].byte[E_IM_MXIC_SLOT_1][j];
							assign->port[i].slot2[j] = io_mxic_tbl->TSLR_2[i].byte[E_IM_MXIC_SLOT_2][j];
							assign->port[i].slot3[j] = io_mxic_tbl->TSLR_2[i].byte[E_IM_MXIC_SLOT_3][j];
							break;
						case E_IM_MXIC_R_ARBITER_R3:
							assign->port[i].slot0[j] = io_mxic_tbl->TSLR_3[i].byte[E_IM_MXIC_SLOT_0][j];
							assign->port[i].slot1[j] = io_mxic_tbl->TSLR_3[i].byte[E_IM_MXIC_SLOT_1][j];
							assign->port[i].slot2[j] = io_mxic_tbl->TSLR_3[i].byte[E_IM_MXIC_SLOT_2][j];
							assign->port[i].slot3[j] = io_mxic_tbl->TSLR_3[i].byte[E_IM_MXIC_SLOT_3][j];
							break;
						case E_IM_MXIC_R_ARBITER_R4:
							assign->port[i].slot0[j] = io_mxic_tbl->TSLR_4[i].byte[E_IM_MXIC_SLOT_0][j];
							assign->port[i].slot1[j] = io_mxic_tbl->TSLR_4[i].byte[E_IM_MXIC_SLOT_1][j];
							assign->port[i].slot2[j] = io_mxic_tbl->TSLR_4[i].byte[E_IM_MXIC_SLOT_2][j];
							assign->port[i].slot3[j] = io_mxic_tbl->TSLR_4[i].byte[E_IM_MXIC_SLOT_3][j];
							break;
						default:
							// arbiter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Get_Master_R_Arbiter(): arbiter is illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
			}

			im_mxic_off_hclk( unit );

			if ( unit == E_IM_MXIC_UNIT_3 ) {
				DDIM_User_AhbReg_SpinUnLock();			// JPEG issue work around.
			}

			im_mxic_set_taen( unit, io_mxic, FALSE );	// TAEN-OFF.
		}
		else {
			// Input parameter error.
//			Ddim_Print(("E:Im_MXIC_Get_Master_R_Arbiter(): im_mxic_get_unit_tbl_address error.\n"));
			result = D_DDIM_OK;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Master_R_Arbiter(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the master to the all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		all_assign		 		Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting changed by this function is reflected by the configration start.
*/
INT32 Im_MXIC_Set_Master_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ARBITER_ASSIGN const* const all_assign )
{
	INT32							i;
	INT32							result;
	volatile struct io_jdsmxic*		io_mxic;
	volatile struct io_jdsmxic_tbl*	io_mxic_tbl;

#ifdef CO_PARAM_CHECK
	if ( all_assign == NULL ) {
		// all_assign parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Master_All_Arbiter(): all_assign is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		result = im_mxic_get_unit_tbl_address( unit, &io_mxic_tbl );

		if ( result == D_DDIM_OK ) {

			im_mxic_wait_command_enable( unit, io_mxic );	// Wait TCFLG=1.

			im_mxic_set_taen( unit, io_mxic, TRUE );		// TAEN-ON.

			for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

				result = im_mxic_set_master_w_arbiter_reg( unit, io_mxic, io_mxic_tbl, (E_IM_MXIC_W_ARBITER)i, (&all_assign->w_arbiter[i]) );

				if ( result != D_DDIM_OK ) {
					// Error occurred by the master allocation to W1 arbiter. 
					Ddim_Print(("E:Im_MXIC_Set_Master_All_Arbiter(): W%d error.\n", i+1));
					return result;
				}
			}

			for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

				result = im_mxic_set_master_r_arbiter_reg( unit, io_mxic, io_mxic_tbl, (E_IM_MXIC_R_ARBITER)i, (&all_assign->r_arbiter[i]) );

				if ( result != D_DDIM_OK ) {
					// Error occurred by the master allocation to R1 arbiter. 
					Ddim_Print(("E:Im_MXIC_Set_Master_All_Arbiter(): R%d error.\n", i+1));
					return result;
				}
			}

			im_mxic_process_configuration( unit, io_mxic );

			im_mxic_set_taen( unit, io_mxic, FALSE );	// TAEN-OFF.
		}
		else {
			// Input parameter error.
//			Ddim_Print(("E:Im_MXIC_Set_Master_All_Arbiter(): im_mxic_get_unit_tbl_address error.\n"));
			result = D_DDIM_OK;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Master_All_Arbiter(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the master from the all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_assign		 		Setting of master assign.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_ARBITER_ASSIGN* const all_assign )
{
	INT32 i;
	INT32 result;

#ifdef CO_PARAM_CHECK
	if ( all_assign == NULL ) {
		// all_assign parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_All_Arbiter(): all_assign is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		result = Im_MXIC_Get_Master_W_Arbiter( unit, (E_IM_MXIC_W_ARBITER)i, (&all_assign->w_arbiter[i]) );

		if ( result != D_DDIM_OK ) {
			// Error occurred by acquiring the master allocation setting of W1 arbiter. 
			Ddim_Print(("E:Im_MXIC_Get_Master_All_Arbiter(): W%d error.\n", i+1));
			return result;
		}
	}

	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		result = Im_MXIC_Get_Master_R_Arbiter( unit, (E_IM_MXIC_R_ARBITER)i, (&all_assign->r_arbiter[i]) );

		if ( result != D_DDIM_OK ) {
			// Error occurred by acquiring the master allocation setting of R1 arbiter. 
			Ddim_Print(("E:Im_MXIC_Get_Master_All_Arbiter(): R%d error.\n", i+1));
			return result;
		}
	}

	return D_DDIM_OK;
}

/**
This function set the slot priority level control.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@param[in]		level_number			Target level.<br>
										Please refer to @ref E_IM_MXIC_SLOT_LEVEL_CTRL for a set value.<br>
@param[in]		priority				Priority level control settings.
										Please refer to @ref T_IM_MXIC_SLOT_PRIORITY_LEVEL for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting ranges are from 1 to 64.
*/
INT32 Im_MXIC_Set_Slot_Priority( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT_LEVEL_CTRL level_number, T_IM_MXIC_SLOT_PRIORITY_LEVEL const* const priority )
{
	UCHAR						reg_read_sw;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_level_ctrl_block( wr_arbiter, arbiter, port, level_number ) != D_DDIM_OK ) {
		// Input parameter error.
		Ddim_Assertion(("E:Im_MXIC_Set_Slot_Priority(): im_mxic_check_level_ctrl_block error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( priority == NULL ) {
		// priority parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Slot_Priority(): priority is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( unit != E_IM_MXIC_UNIT_4 ) {
				// Write arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						// arbiter1.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								io_mxic->TLVLW1[port].bit.LPCCL0W	= priority->upper_priority_level;
								io_mxic->TLVLW1[port].bit.LPCSL0W	= priority->lower_priority_level;
								io_mxic->TLVLW1[port].bit.LPL0W		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								io_mxic->TLVLW1[port].bit.LPCCL1W	= priority->upper_priority_level;
								io_mxic->TLVLW1[port].bit.LPCSL1W	= priority->lower_priority_level;
								io_mxic->TLVLW1[port].bit.LPL1W		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								io_mxic->TLVLW1[port].bit.LPCCL2W	= priority->upper_priority_level;
								io_mxic->TLVLW1[port].bit.LPCSL2W	= priority->lower_priority_level;
								io_mxic->TLVLW1[port].bit.LPL2W		= priority->priority_level_type;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						// arbiter2.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								io_mxic->TLVLW2[port].bit.LPCCL0W	= priority->upper_priority_level;
								io_mxic->TLVLW2[port].bit.LPCSL0W	= priority->lower_priority_level;
								io_mxic->TLVLW2[port].bit.LPL0W		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								io_mxic->TLVLW2[port].bit.LPCCL1W	= priority->upper_priority_level;
								io_mxic->TLVLW2[port].bit.LPCSL1W	= priority->lower_priority_level;
								io_mxic->TLVLW2[port].bit.LPL1W		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								io_mxic->TLVLW2[port].bit.LPCCL2W	= priority->upper_priority_level;
								io_mxic->TLVLW2[port].bit.LPCSL2W	= priority->lower_priority_level;
								io_mxic->TLVLW2[port].bit.LPL2W		= priority->priority_level_type;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						// arbiter3.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								io_mxic->TLVLW3[port].bit.LPCCL0W	= priority->upper_priority_level;
								io_mxic->TLVLW3[port].bit.LPCSL0W	= priority->lower_priority_level;
								io_mxic->TLVLW3[port].bit.LPL0W		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								io_mxic->TLVLW3[port].bit.LPCCL1W	= priority->upper_priority_level;
								io_mxic->TLVLW3[port].bit.LPCSL1W	= priority->lower_priority_level;
								io_mxic->TLVLW3[port].bit.LPL1W		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								io_mxic->TLVLW3[port].bit.LPCCL2W	= priority->upper_priority_level;
								io_mxic->TLVLW3[port].bit.LPCSL2W	= priority->lower_priority_level;
								io_mxic->TLVLW3[port].bit.LPL2W		= priority->priority_level_type;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						// arbiter4.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								io_mxic->TLVLW4[port].bit.LPCCL0W	= priority->upper_priority_level;
								io_mxic->TLVLW4[port].bit.LPCSL0W	= priority->lower_priority_level;
								io_mxic->TLVLW4[port].bit.LPL0W		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								io_mxic->TLVLW4[port].bit.LPCCL1W	= priority->upper_priority_level;
								io_mxic->TLVLW4[port].bit.LPCSL1W	= priority->lower_priority_level;
								io_mxic->TLVLW4[port].bit.LPL1W		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								io_mxic->TLVLW4[port].bit.LPCCL2W	= priority->upper_priority_level;
								io_mxic->TLVLW4[port].bit.LPCSL2W	= priority->lower_priority_level;
								io_mxic->TLVLW4[port].bit.LPL2W		= priority->priority_level_type;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					default:
						// arbiter parameter value is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): arbiter is illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Set_Slot_Priority(): unit number is invalid. unit=%d\n", unit));
			}
		}
		else {
			if ( unit != E_IM_MXIC_UNIT_0 ) {
				// Read arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						// arbiter1.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								io_mxic->TLVLR1[port].bit.LPCCL0R	= priority->upper_priority_level;
								io_mxic->TLVLR1[port].bit.LPCSL0R	= priority->lower_priority_level;
								io_mxic->TLVLR1[port].bit.LPL0R		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								io_mxic->TLVLR1[port].bit.LPCCL1R	= priority->upper_priority_level;
								io_mxic->TLVLR1[port].bit.LPCSL1R	= priority->lower_priority_level;
								io_mxic->TLVLR1[port].bit.LPL1R		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								io_mxic->TLVLR1[port].bit.LPCCL2R	= priority->upper_priority_level;
								io_mxic->TLVLR1[port].bit.LPCSL2R	= priority->lower_priority_level;
								io_mxic->TLVLR1[port].bit.LPL2R		= priority->priority_level_type;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						// arbiter2.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								io_mxic->TLVLR2[port].bit.LPCCL0R	= priority->upper_priority_level;
								io_mxic->TLVLR2[port].bit.LPCSL0R	= priority->lower_priority_level;
								io_mxic->TLVLR2[port].bit.LPL0R		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								io_mxic->TLVLR2[port].bit.LPCCL1R	= priority->upper_priority_level;
								io_mxic->TLVLR2[port].bit.LPCSL1R	= priority->lower_priority_level;
								io_mxic->TLVLR2[port].bit.LPL1R		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								io_mxic->TLVLR2[port].bit.LPCCL2R	= priority->upper_priority_level;
								io_mxic->TLVLR2[port].bit.LPCSL2R	= priority->lower_priority_level;
								io_mxic->TLVLR2[port].bit.LPL2R		= priority->priority_level_type;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						// arbiter3.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								io_mxic->TLVLR3[port].bit.LPCCL0R	= priority->upper_priority_level;
								io_mxic->TLVLR3[port].bit.LPCSL0R	= priority->lower_priority_level;
								io_mxic->TLVLR3[port].bit.LPL0R		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								io_mxic->TLVLR3[port].bit.LPCCL1R	= priority->upper_priority_level;
								io_mxic->TLVLR3[port].bit.LPCSL1R	= priority->lower_priority_level;
								io_mxic->TLVLR3[port].bit.LPL1R		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								io_mxic->TLVLR3[port].bit.LPCCL2R	= priority->upper_priority_level;
								io_mxic->TLVLR3[port].bit.LPCSL2R	= priority->lower_priority_level;
								io_mxic->TLVLR3[port].bit.LPL2R		= priority->priority_level_type;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						// arbiter4.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								io_mxic->TLVLR4[port].bit.LPCCL0R	= priority->upper_priority_level;
								io_mxic->TLVLR4[port].bit.LPCSL0R	= priority->lower_priority_level;
								io_mxic->TLVLR4[port].bit.LPL0R		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								io_mxic->TLVLR4[port].bit.LPCCL1R	= priority->upper_priority_level;
								io_mxic->TLVLR4[port].bit.LPCSL1R	= priority->lower_priority_level;
								io_mxic->TLVLR4[port].bit.LPL1R		= priority->priority_level_type;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								io_mxic->TLVLR4[port].bit.LPCCL2R	= priority->upper_priority_level;
								io_mxic->TLVLR4[port].bit.LPCSL2R	= priority->lower_priority_level;
								io_mxic->TLVLR4[port].bit.LPL2R		= priority->priority_level_type;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					default:
						// arbiter parameter value is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): arbiter is illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Set_Slot_Priority(): unit number is invalid. unit=%d\n", unit));
			}
		}

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Slot_Priority(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the slot priority level control.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@param[in]		level_number			Target level.<br>
										Please refer to @ref E_IM_MXIC_SLOT_LEVEL_CTRL for a set value.<br>
@param[in]		priority				Priority level control settings.
										Please refer to @ref T_IM_MXIC_SLOT_PRIORITY_LEVEL for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Slot_Priority( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT_LEVEL_CTRL level_number, T_IM_MXIC_SLOT_PRIORITY_LEVEL* const priority )
{
	UCHAR						reg_read_sw;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if (im_mxic_check_level_ctrl_block( wr_arbiter, arbiter, port, level_number ) != D_DDIM_OK) {
		// Input parameter error.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Priority(): im_mxic_check_level_ctrl_block error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( priority == NULL ) {
		// priority parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Priority(): priority is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( unit != E_IM_MXIC_UNIT_4 ) {
				// Write arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						// arbiter1.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								priority->upper_priority_level = io_mxic->TLVLW1[port].bit.LPCCL0W;
								priority->lower_priority_level = io_mxic->TLVLW1[port].bit.LPCSL0W;
								priority->priority_level_type  = io_mxic->TLVLW1[port].bit.LPL0W;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								priority->upper_priority_level = io_mxic->TLVLW1[port].bit.LPCCL1W;
								priority->lower_priority_level = io_mxic->TLVLW1[port].bit.LPCSL1W;
								priority->priority_level_type  = io_mxic->TLVLW1[port].bit.LPL1W;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								priority->upper_priority_level = io_mxic->TLVLW1[port].bit.LPCCL2W;
								priority->lower_priority_level = io_mxic->TLVLW1[port].bit.LPCSL2W;
								priority->priority_level_type  = io_mxic->TLVLW1[port].bit.LPL2W;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						// arbiter2.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								priority->upper_priority_level = io_mxic->TLVLW2[port].bit.LPCCL0W;
								priority->lower_priority_level = io_mxic->TLVLW2[port].bit.LPCSL0W;
								priority->priority_level_type  = io_mxic->TLVLW2[port].bit.LPL0W;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								priority->upper_priority_level = io_mxic->TLVLW2[port].bit.LPCCL1W;
								priority->lower_priority_level = io_mxic->TLVLW2[port].bit.LPCSL1W;
								priority->priority_level_type  = io_mxic->TLVLW2[port].bit.LPL1W;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								priority->upper_priority_level = io_mxic->TLVLW2[port].bit.LPCCL2W;
								priority->lower_priority_level = io_mxic->TLVLW2[port].bit.LPCSL2W;
								priority->priority_level_type  = io_mxic->TLVLW2[port].bit.LPL2W;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						// arbiter3.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								priority->upper_priority_level = io_mxic->TLVLW3[port].bit.LPCCL0W;
								priority->lower_priority_level = io_mxic->TLVLW3[port].bit.LPCSL0W;
								priority->priority_level_type  = io_mxic->TLVLW3[port].bit.LPL0W;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								priority->upper_priority_level = io_mxic->TLVLW3[port].bit.LPCCL1W;
								priority->lower_priority_level = io_mxic->TLVLW3[port].bit.LPCSL1W;
								priority->priority_level_type  = io_mxic->TLVLW3[port].bit.LPL1W;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								priority->upper_priority_level = io_mxic->TLVLW3[port].bit.LPCCL2W;
								priority->lower_priority_level = io_mxic->TLVLW3[port].bit.LPCSL2W;
								priority->priority_level_type  = io_mxic->TLVLW3[port].bit.LPL2W;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						// arbiter4.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								priority->upper_priority_level = io_mxic->TLVLW4[port].bit.LPCCL0W;
								priority->lower_priority_level = io_mxic->TLVLW4[port].bit.LPCSL0W;
								priority->priority_level_type  = io_mxic->TLVLW4[port].bit.LPL0W;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								priority->upper_priority_level = io_mxic->TLVLW4[port].bit.LPCCL1W;
								priority->lower_priority_level = io_mxic->TLVLW4[port].bit.LPCSL1W;
								priority->priority_level_type  = io_mxic->TLVLW4[port].bit.LPL1W;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								priority->upper_priority_level = io_mxic->TLVLW4[port].bit.LPCCL2W;
								priority->lower_priority_level = io_mxic->TLVLW4[port].bit.LPCSL2W;
								priority->priority_level_type  = io_mxic->TLVLW4[port].bit.LPL2W;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					default:
						// arbiter parameter value is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): arbiter is illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				priority->upper_priority_level	= 0;
				priority->lower_priority_level	= 0;
				priority->priority_level_type	= 0;

				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Get_Slot_Priority(): unit number is invalid. unit=%d\n", unit));
			}
		}
		else {
			if ( unit != E_IM_MXIC_UNIT_0 ) {
				// Read arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						// arbiter1.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								priority->upper_priority_level = io_mxic->TLVLR1[port].bit.LPCCL0R;
								priority->lower_priority_level = io_mxic->TLVLR1[port].bit.LPCSL0R;
								priority->priority_level_type  = io_mxic->TLVLR1[port].bit.LPL0R;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								priority->upper_priority_level = io_mxic->TLVLR1[port].bit.LPCCL1R;
								priority->lower_priority_level = io_mxic->TLVLR1[port].bit.LPCSL1R;
								priority->priority_level_type  = io_mxic->TLVLR1[port].bit.LPL1R;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								priority->upper_priority_level = io_mxic->TLVLR1[port].bit.LPCCL2R;
								priority->lower_priority_level = io_mxic->TLVLR1[port].bit.LPCSL2R;
								priority->priority_level_type  = io_mxic->TLVLR1[port].bit.LPL2R;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						// arbiter2
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								priority->upper_priority_level = io_mxic->TLVLR2[port].bit.LPCCL0R;
								priority->lower_priority_level = io_mxic->TLVLR2[port].bit.LPCSL0R;
								priority->priority_level_type  = io_mxic->TLVLR2[port].bit.LPL0R;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								priority->upper_priority_level = io_mxic->TLVLR2[port].bit.LPCCL1R;
								priority->lower_priority_level = io_mxic->TLVLR2[port].bit.LPCSL1R;
								priority->priority_level_type  = io_mxic->TLVLR2[port].bit.LPL1R;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								priority->upper_priority_level = io_mxic->TLVLR2[port].bit.LPCCL2R;
								priority->lower_priority_level = io_mxic->TLVLR2[port].bit.LPCSL2R;
								priority->priority_level_type  = io_mxic->TLVLR2[port].bit.LPL2R;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						// arbiter3.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								priority->upper_priority_level = io_mxic->TLVLR3[port].bit.LPCCL0R;
								priority->lower_priority_level = io_mxic->TLVLR3[port].bit.LPCSL0R;
								priority->priority_level_type  = io_mxic->TLVLR3[port].bit.LPL0R;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								priority->upper_priority_level = io_mxic->TLVLR3[port].bit.LPCCL1R;
								priority->lower_priority_level = io_mxic->TLVLR3[port].bit.LPCSL1R;
								priority->priority_level_type  = io_mxic->TLVLR3[port].bit.LPL1R;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								priority->upper_priority_level = io_mxic->TLVLR3[port].bit.LPCCL2R;
								priority->lower_priority_level = io_mxic->TLVLR3[port].bit.LPCSL2R;
								priority->priority_level_type  = io_mxic->TLVLR3[port].bit.LPL2R;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						// arbiter4.
						switch ( level_number ) {
							case E_IM_MXIC_SLOT_LEVEL_CTRL_0:
								// Level0.
								priority->upper_priority_level = io_mxic->TLVLR4[port].bit.LPCCL0R;
								priority->lower_priority_level = io_mxic->TLVLR4[port].bit.LPCSL0R;
								priority->priority_level_type  = io_mxic->TLVLR4[port].bit.LPL0R;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_1:
								// Level1.
								priority->upper_priority_level = io_mxic->TLVLR4[port].bit.LPCCL1R;
								priority->lower_priority_level = io_mxic->TLVLR4[port].bit.LPCSL1R;
								priority->priority_level_type  = io_mxic->TLVLR4[port].bit.LPL1R;
								break;
							case E_IM_MXIC_SLOT_LEVEL_CTRL_2:
								// Level2.
								priority->upper_priority_level = io_mxic->TLVLR4[port].bit.LPCCL2R;
								priority->lower_priority_level = io_mxic->TLVLR4[port].bit.LPCSL2R;
								priority->priority_level_type  = io_mxic->TLVLR4[port].bit.LPL2R;
								break;
							default:
								// level_number parameter value is illegal.
								result = D_IM_MXIC_INPUT_PARAM_ERROR;
								Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): level_number is illegal. level_number=%d\n", level_number));
								break;
						}
						break;
					default:
						// arbiter parameter value is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): arbiter is illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				priority->upper_priority_level	= 0;
				priority->lower_priority_level	= 0;
				priority->priority_level_type	= 0;

				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Get_Slot_Priority(): unit number is invalid. unit=%d\n", unit));
			}
		}

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Slot_Priority(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the slot priority level control to all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		all_priority			Slot priority level control setting of all arbiter.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting  ranges are from 1 to 64.
*/
INT32 Im_MXIC_Set_Slot_Priority_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL const* const all_priority )
{
	INT32 result;
	INT32 i, j, k;
	
#ifdef CO_PARAM_CHECK
	if ( all_priority == NULL ) {
		// all_priority parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Slot_Priority_All_Arbiter(): all_priority is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = D_DDIM_OK;

	// Set priority level control to all Write arbiter.
	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {

			for ( k = E_IM_MXIC_SLOT_LEVEL_CTRL_0; k < E_IM_MXIC_SLOT_LEVEL_CTRL_MAX; k++ ) {
				// Set priority level.
				switch ( k ) {
					case 0:
						result = Im_MXIC_Set_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->w_level[i].level0[j] );
						break;
					case 1:
						result = Im_MXIC_Set_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->w_level[i].level1[j] );
						break;
					case 2:
						result = Im_MXIC_Set_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->w_level[i].level2[j] );
						break;
				}

				if ( result != D_DDIM_OK ) {
					// Priority level setting error.
					Ddim_Print(("E:Im_MXIC_Set_Slot_Priority_All_Arbiter(): Im_MXIC_Set_Slot_Priority error.\n"));
					Im_MXIC_Dsb();
					return result;
				}
			}
		}
	}

	// Set priority level control to all Read arbiter.
	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {

			for ( k = E_IM_MXIC_SLOT_LEVEL_CTRL_0; k < E_IM_MXIC_SLOT_LEVEL_CTRL_MAX; k++ ) {
				// Set priority level.
				switch ( k ) {
					case 0:
						result = Im_MXIC_Set_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->r_level[i].level0[j] );
						break;
					case 1:
						result = Im_MXIC_Set_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->r_level[i].level1[j] );
						break;
					case 2:
						result = Im_MXIC_Set_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->r_level[i].level2[j] );
						break;
				}

				if ( result != D_DDIM_OK ) {
					// Priority level setting error.
					Ddim_Print(("E:Im_MXIC_Set_Slot_Priority_All_Arbiter(): Im_MXIC_Set_Slot_Priority error.\n"));
					Im_MXIC_Dsb();
					return result;
				}
			}
		}
	}

	Im_MXIC_Dsb();

	return result;
}

/**
This function get the slot priority level control from all arbiter.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_priority			Slot priority level control setting of all arbiter.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Slot_Priority_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLOT_PRIORITY_LEVEL* const all_priority )
{
	INT32 result;
	INT32 i, j, k;

#ifdef CO_PARAM_CHECK
	if ( all_priority == NULL ) {
		// all_priority parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Priority_All_Arbiter(): all_priority is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = D_DDIM_OK;

	// Get priority level control to all Write arbiter.
	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {

			for ( k = E_IM_MXIC_SLOT_LEVEL_CTRL_0; k < E_IM_MXIC_SLOT_LEVEL_CTRL_MAX; k++ ) {
				// Get priority level.
				switch ( k ) {
					case 0:
						result = Im_MXIC_Get_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->w_level[i].level0[j] );
						break;
					case 1:
						result = Im_MXIC_Get_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->w_level[i].level1[j] );
						break;
					case 2:
						result = Im_MXIC_Get_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->w_level[i].level2[j] );
						break;
				}

				if ( result != D_DDIM_OK ) {
					// Priority level setting error.
					Ddim_Print(("E:Im_MXIC_Get_Slot_Priority_All_Arbiter(): Im_MXIC_Get_Slot_Priority error.\n"));
					Im_MXIC_Dsb();
					return result;
				}
			}
		}
	}

	// Get priority level control to all Read arbiter.
	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {

			for ( k = E_IM_MXIC_SLOT_LEVEL_CTRL_0; k < E_IM_MXIC_SLOT_LEVEL_CTRL_MAX; k++ ) {
				// Get priority level.
				switch ( k ) {
					case 0:
						result = Im_MXIC_Get_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->r_level[i].level0[j] );
						break;
					case 1:
						result = Im_MXIC_Get_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->r_level[i].level1[j] );
						break;
					case 2:
						result = Im_MXIC_Get_Slot_Priority( unit, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, (E_IM_MXIC_SLOT_LEVEL_CTRL)k, &all_priority->r_level[i].level2[j] );
						break;
				}

				if ( result != D_DDIM_OK ) {
					// Priority level setting error.
					Ddim_Print(("E:Im_MXIC_Get_Slot_Priority_All_Arbiter(): Im_MXIC_Get_Slot_Priority error.\n"));
					Im_MXIC_Dsb();
					return result;
				}
			}
		}
	}

	return result;
}

/**
This function set the slot mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@param[in]		slot					Target slot number.<br>
										Please refer to @ref E_IM_MXIC_SLOT for a set value. <br>
@param[in]		mask_enable				Slot mask enable.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Slot_Mask( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT slot, UCHAR mask_enable )
{
	INT32						result;
	INT32						bit_flag;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_slot( wr_arbiter, arbiter, port, slot ) != D_DDIM_OK ) {
		// Parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Slot_Mask(): im_mxic_check_slot error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( ( mask_enable != D_IM_MXIC_ON ) && ( mask_enable != D_IM_MXIC_OFF ) ) {
		// mask_enable parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Slot_Mask(): mask_enable is illegal. mask_enable=%d\n", mask_enable));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		// Get bit flag for register access.
		result = im_mxic_get_slot_mask_bit_flag( slot, &bit_flag );

		if ( result != D_DDIM_OK ) {
			// Parameter error.
			Ddim_Print(("E:Im_MXIC_Set_Slot_Mask(): im_mxic_get_slot_mask_bit_flag error.\n"));
			return result;
		}

		im_mxic_on_pclk( unit );

		// Set slot mask.
		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( unit != E_IM_MXIC_UNIT_4 ) {
				// Write arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						if ( mask_enable == D_IM_MXIC_ON ) {
							io_mxic->TLVLW1[port].bit.LMLW |= bit_flag;
						}
						else {
							io_mxic->TLVLW1[port].bit.LMLW &= ~bit_flag;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						if ( mask_enable == D_IM_MXIC_ON ) {
							io_mxic->TLVLW2[port].bit.LMLW |= bit_flag;
						}
						else {
							io_mxic->TLVLW2[port].bit.LMLW &= ~bit_flag;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						if ( mask_enable == D_IM_MXIC_ON ) {
							io_mxic->TLVLW3[port].bit.LMLW |= bit_flag;
						}
						 else {
							io_mxic->TLVLW3[port].bit.LMLW &= ~bit_flag;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						if ( mask_enable == D_IM_MXIC_ON ) {
							io_mxic->TLVLW4[port].bit.LMLW |= bit_flag;
						}
						else {
							io_mxic->TLVLW4[port].bit.LMLW &= ~bit_flag;
						}
						break;
					default:
						// arbiter parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Slot_Mask(): arbiter illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Set_Slot_Mask(): unit number is invalid. unit=%d\n", unit));
			}
		}
		else {
			if ( unit != E_IM_MXIC_UNIT_0 ) {
				// Read arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						if ( mask_enable == D_IM_MXIC_ON ) {
							io_mxic->TLVLR1[port].bit.LMLR |= bit_flag;
						}
						else {
							io_mxic->TLVLR1[port].bit.LMLR &= ~bit_flag;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						if ( mask_enable == D_IM_MXIC_ON ) {
							io_mxic->TLVLR2[port].bit.LMLR |= bit_flag;
						}
						else {
							io_mxic->TLVLR2[port].bit.LMLR &= ~bit_flag;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						if ( mask_enable == D_IM_MXIC_ON ) {
							io_mxic->TLVLR3[port].bit.LMLR |= bit_flag;
						}
						else {
							io_mxic->TLVLR3[port].bit.LMLR &= ~bit_flag;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						if ( mask_enable == D_IM_MXIC_ON ) {
							io_mxic->TLVLR4[port].bit.LMLR |= bit_flag;
						}
						else {
							io_mxic->TLVLR4[port].bit.LMLR &= ~bit_flag;
						}
						break;
					default:
						// arbiter parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Slot_Mask(): arbiter illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Set_Slot_Mask(): unit number is invalid. unit=%d\n", unit));
			}
		}

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Slot_Mask(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the slot mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@param[in]		slot					Target slot number.<br>
										Please refer to @ref E_IM_MXIC_SLOT for a set value. <br>
@param[out]		mask_enable				Slot mask enable.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
INT32 Im_MXIC_Get_Slot_Mask( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, E_IM_MXIC_SLOT slot, UCHAR* const mask_enable )
{
	INT32						result;
	INT32						bit_flag;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_slot( wr_arbiter, arbiter, port, slot ) != D_DDIM_OK ) {
		// Parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Mask(): im_mxic_check_slot error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( mask_enable == NULL ) {
		// mask_enable parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Mask(): mask_enable is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		// Get bit flag for register access.
		result = im_mxic_get_slot_mask_bit_flag( slot, &bit_flag );

		if ( result != D_DDIM_OK ) {
			// Parameter error.
			Ddim_Print(("E:Im_MXIC_Set_Slot_Mask(): im_mxic_get_slot_mask_bit_flag error.\n"));
			return result;
		}

		im_mxic_on_pclk( unit );

		// Get slot mask.
		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( unit != E_IM_MXIC_UNIT_4 ) {
				// Write arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						if ( ( io_mxic->TLVLW1[port].bit.LMLW & bit_flag ) != 0 ) {
							*mask_enable = D_IM_MXIC_ON;
						}
						else {
							*mask_enable = D_IM_MXIC_OFF;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						if ( ( io_mxic->TLVLW2[port].bit.LMLW & bit_flag ) != 0 ) {
							*mask_enable = D_IM_MXIC_ON;
						}
						else {
							*mask_enable = D_IM_MXIC_OFF;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						if ( ( io_mxic->TLVLW3[port].bit.LMLW & bit_flag ) != 0 ) {
							*mask_enable = D_IM_MXIC_ON;
						}
						else {
							*mask_enable = D_IM_MXIC_OFF;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						if ( ( io_mxic->TLVLW4[port].bit.LMLW & bit_flag ) != 0 ) {
							*mask_enable = D_IM_MXIC_ON;
						}
						else {
							*mask_enable = D_IM_MXIC_OFF;
						}
						break;
					default:
						// arbiter parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Slot_Mask(): arbiter illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				*mask_enable = D_IM_MXIC_OFF;

				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Set_Slot_Mask(): unit number is invalid. unit=%d\n", unit));
			}
		}
		else {
			if ( unit != E_IM_MXIC_UNIT_0 ) {
				// Read arbiter.
				switch ( arbiter ) {
					case E_IM_MXIC_SPEC_ARBITER_1:
						if ( ( io_mxic->TLVLR1[port].bit.LMLR & bit_flag ) != 0 ) {
							*mask_enable = D_IM_MXIC_ON;
						}
						else {
							*mask_enable = D_IM_MXIC_OFF;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_2:
						if ( ( io_mxic->TLVLR2[port].bit.LMLR & bit_flag ) !=  0) {
							*mask_enable = D_IM_MXIC_ON;
						}
						else {
							*mask_enable = D_IM_MXIC_OFF;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_3:
						if ( ( io_mxic->TLVLR3[port].bit.LMLR & bit_flag ) != 0 ) {
							*mask_enable = D_IM_MXIC_ON;
						}
						else {
							*mask_enable = D_IM_MXIC_OFF;
						}
						break;
					case E_IM_MXIC_SPEC_ARBITER_4:
						if ( ( io_mxic->TLVLR4[port].bit.LMLR & bit_flag ) != 0 ) {
							*mask_enable = D_IM_MXIC_ON;
						}
						else {
							*mask_enable = D_IM_MXIC_OFF;
						}
						break;
					default:
						// arbiter parameter is illegal.
						result = D_IM_MXIC_INPUT_PARAM_ERROR;
						Ddim_Print(("E:Im_MXIC_Set_Slot_Mask(): arbiter illegal. arbiter=%d\n", arbiter));
						break;
				}
			}
			else {
				*mask_enable = D_IM_MXIC_OFF;

				// Unit number is invalid.
//				Ddim_Print(("Im_MXIC_Set_Slot_Mask(): unit number is invalid. unit=%d\n", unit));
			}
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Slot_Mask(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the movement position after the master process.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		priority				Master priority level.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Master_Priority( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR priority )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( priority > D_IM_MXIC_MASTER_MOVE_15 ) {
		// priority parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Master_Priority(): priority is illegal. priority=%d\n", priority));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {
					// Set master priority level of write master.
					im_mxic_on_pclk( unit );

					io_mxic->TRMCW[(master - 1)].bit.TRPW	= priority;		// W1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Priority(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {
					// Set master priority level of write master.
					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					io_mxic->TRMCW_GR[master].bit.TRPW		= priority;		// W64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Priority(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if ( (master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {

			master &= D_IM_MXIC_READ_MASTER_FLG_MASK;

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {
					// Set master priority level of read master.
					im_mxic_on_pclk( unit );

					io_mxic->TRMCR[(master - 1)].bit.TRPR	= priority;		// R1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Priority(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {
					// Set master priority level of read master.
					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					io_mxic->TRMCR_GR[master].bit.TRPR		= priority;		// R64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Priority(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:Im_MXIC_Set_Master_Priority(): master is illegal. master=%d\n", master));
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
		}

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Master_Priority(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the movement position after the master process.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[out]		priority				Master priority level.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_Priority( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR* const priority )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( priority == NULL ) {
		// priority parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_Priority(): priority is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {
					// Get master priority level of write master.
					im_mxic_on_pclk( unit );

					*priority = io_mxic->TRMCW[(master - 1)].bit.TRPW;	// W1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					*priority = 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Priority(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {
					// Get master priority level of write master.
					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					*priority = io_mxic->TRMCW_GR[master].bit.TRPW;		// W64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					*priority = 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Priority(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {

			master &= D_IM_MXIC_READ_MASTER_FLG_MASK;

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {
					// Get master priority level of read master.
					im_mxic_on_pclk( unit );

					*priority = io_mxic->TRMCR[(master - 1)].bit.TRPR;	// R1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					*priority = 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Priority(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {
					// Get master priority level of read master.
					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					*priority = io_mxic->TRMCR_GR[master].bit.TRPR;		// R64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					*priority = 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Priority(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:Im_MXIC_Get_Master_Priority(): master is illegal. master=%d\n", master));
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Master_Priority(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the master transfer mask and mask period.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		mask					Setting of mask.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The setting mask ranges are from 4 to 4096.
*/
INT32 Im_MXIC_Set_Master_Mask( E_IM_MXIC_UNIT unit, UCHAR master, T_IM_MXIC_MASTER_MASK const* const mask )
{
	UCHAR						master_type_read;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( mask == NULL ) {
		// mask parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Master_Mask(): mask is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		// Set mask period.
		if ( mask->mask_type == D_IM_MXIC_MASTER_MASK_PERIOD_ON ) {

			if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {

				if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

					if ( unit != E_IM_MXIC_UNIT_4 ) {
						// Set mask period of write master.
						im_mxic_on_pclk( unit );

						io_mxic->TRMCW[(master - 1)].bit.TRMCVPW = mask->mask_period;	// W1-63.

						im_mxic_off_pclk( unit );
					}
					else {
						// Unit number is invalid.
//						Ddim_Print(("Im_MXIC_Set_Master_Mask(): unit number is invalid. unit=%d\n", unit));
					}
				}
				else {
					if ( unit == E_IM_MXIC_UNIT_3 ) {
						// Set mask period of write master.
						master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

						im_mxic_on_pclk( unit );

						io_mxic->TRMCW_GR[master].bit.TRMCVPW = mask->mask_period;		// W64-127.

						im_mxic_off_pclk( unit );
					}
					else {
						// Unit number is invalid.
//						Ddim_Print(("Im_MXIC_Set_Master_Mask(): unit number is invalid. unit=%d\n", unit));
					}
				}
			}
			else if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {

				master_type_read = master;
				master_type_read &= D_IM_MXIC_READ_MASTER_FLG_MASK;

				if ( master_type_read < D_IM_MXIC_MASTER_NUMBER_MAX ) {

					if ( unit != E_IM_MXIC_UNIT_0 ) {
						// Set mask period of read master.
						im_mxic_on_pclk( unit );

						io_mxic->TRMCR[(master_type_read - 1)].bit.TRMCVPR = mask->mask_period;		// R1-63.

						im_mxic_off_pclk( unit );
					}
					else {
						// Unit number is invalid.
//						Ddim_Print(("Im_MXIC_Set_Master_Mask(): unit number is invalid. unit=%d\n", unit));
					}
				}
				else {
					if ( unit == E_IM_MXIC_UNIT_3 ) {
						// Set mask period of write master.
						master_type_read = master_type_read - D_IM_MXIC_MASTER_NUMBER_MAX;

						im_mxic_on_pclk( unit );

						io_mxic->TRMCR_GR[master_type_read].bit.TRMCVPR = mask->mask_period;		// R64-127.

						im_mxic_off_pclk( unit );
					}
					else {
						// Unit number is invalid.
//						Ddim_Print(("Im_MXIC_Set_Master_Mask(): unit number is invalid. unit=%d\n", unit));
					}
				}
			}
			else {
				// Master number is illegal.
				Ddim_Print(("E:Im_MXIC_Set_Master_Mask(): master is illegal. master=%d\n", master));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}

		// Set mask type.
		if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {
					// Set mask type of write master.
					im_mxic_on_pclk( unit );

					io_mxic->TRMCW[(master - 1)].bit.TRMW = mask->mask_type;	// W1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Mask(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {
					// Set mask type of write master.
					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					io_mxic->TRMCW_GR[master].bit.TRMW = mask->mask_type;		// W64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Mask(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {

			master_type_read = master;
			master_type_read &= D_IM_MXIC_READ_MASTER_FLG_MASK;

			if ( master_type_read < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {
					// Set mask type of read master.
					im_mxic_on_pclk( unit );

					io_mxic->TRMCR[(master_type_read - 1)].bit.TRMR = mask->mask_type;	// R1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Mask(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {
					// Set mask type of read master.
					master_type_read = master_type_read - D_IM_MXIC_MASTER_NUMBER_MAX;
					im_mxic_on_pclk( unit );

					io_mxic->TRMCR_GR[master_type_read].bit.TRMR = mask->mask_type;		// R64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Mask(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:Im_MXIC_Set_Master_Mask(): master is illegal. master=%d\n", master));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the master transfer mask and mask period.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[out]		mask					Setting of mask.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_Mask( E_IM_MXIC_UNIT unit, UCHAR master, T_IM_MXIC_MASTER_MASK* const mask )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( mask == NULL ) {
		// mask parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_Mask(): mask is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {
					// Get mask setting of write master.
					im_mxic_on_pclk( unit );

					mask->mask_type		= io_mxic->TRMCW[(master - 1)].bit.TRMW;	// W1-63.
					mask->mask_period	= io_mxic->TRMCW[(master - 1)].bit.TRMCVPW;	// W1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					mask->mask_type		= 0;
					mask->mask_period	= 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Mask(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {
					// Get mask setting of write master.
					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					mask->mask_type		= io_mxic->TRMCW_GR[master].bit.TRMW;		// W64-127.
					mask->mask_period	= io_mxic->TRMCW_GR[master].bit.TRMCVPW;	// W64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					mask->mask_type		= 0;
					mask->mask_period	= 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Mask(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {

			master &= D_IM_MXIC_READ_MASTER_FLG_MASK;

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {
					// Get mask setting of read master.
					im_mxic_on_pclk( unit );

					mask->mask_type   = io_mxic->TRMCR[(master - 1)].bit.TRMR;		// R1-63.
					mask->mask_period = io_mxic->TRMCR[(master - 1)].bit.TRMCVPR;	// R1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					mask->mask_type		= 0;
					mask->mask_period	= 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Mask(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {
					// Get mask setting of read master.
					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					mask->mask_type   = io_mxic->TRMCR_GR[master].bit.TRMR;			// R64-127.
					mask->mask_period = io_mxic->TRMCR_GR[master].bit.TRMCVPR;		// R64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					mask->mask_type		= 0;
					mask->mask_period	= 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Mask(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:Im_MXIC_Get_Master_Mask(): master is illegal. master=%d\n", master));
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Master_Mask(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set factor of transfer demand mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		group					Mask group.<br>
										Please refer to @ref E_IM_MXIC_MASK_GROUP for a set value.<br>
@param[in]		enable					Setting of whether to make a master into a mask factor.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
INT32 Im_MXIC_Set_Master_Mask_Factor( E_IM_MXIC_UNIT unit, UCHAR master, E_IM_MXIC_MASK_GROUP group, UCHAR enable )
{
	INT32								result;
	T_IM_MXIC_MASTER_MASK_FACTOR_TARGET	config;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_master_mask_factor_target_param( E_IM_MXIC_OPERATION_TYPE_SET, master, group, &enable ) != D_DDIM_OK ) {
		// Parameter error.
		Ddim_Assertion(("E:Im_MXIC_Set_Master_Mask_Factor(): im_mxic_check_master_mask_factor_target_param error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	config.kind_set_get			= E_IM_MXIC_OPERATION_TYPE_SET;
	config.kind_factor_target	= E_IM_MXIC_MASTER_MASK_OPERATION_FACTOR;
	config.master				= master;
	config.group				= group;
	config.enable				= enable;

	// Set master mask factor.
	result = im_mxic_rw_master_mask_factor_or_target( unit, &config );

	if ( result != D_DDIM_OK ) {
		// Error.
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor(): im_mxic_rw_master_mask_factor_or_target error.\n"));
	}

	Im_MXIC_Dsb();

	return result;
}

/**
This function get factor of transfer demand mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		group					Mask group.<br>
										Please refer to @ref E_IM_MXIC_MASK_GROUP for a set value.<br>
@param[out]		enable					Setting of whether to make a master into a mask factor.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_Mask_Factor( E_IM_MXIC_UNIT unit, UCHAR master, E_IM_MXIC_MASK_GROUP group, UCHAR* const enable )
{
	INT32								result;
	T_IM_MXIC_MASTER_MASK_FACTOR_TARGET	config;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_master_mask_factor_target_param( E_IM_MXIC_OPERATION_TYPE_GET, master, group, enable ) != D_DDIM_OK ) {
		// Parameter error.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_Mask_Factor(): im_mxic_check_master_mask_factor_target_param error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	config.kind_set_get			= E_IM_MXIC_OPERATION_TYPE_GET;
	config.kind_factor_target	= E_IM_MXIC_MASTER_MASK_OPERATION_FACTOR;
	config.master				= master;
	config.group				= group;
	config.enable				= D_IM_MXIC_OFF;

	// Get master mask factor.
	result = im_mxic_rw_master_mask_factor_or_target( unit, &config );

	if ( result == D_DDIM_OK ) {
		// Get return value.
		*enable = config.enable;
	}
	else {
		// Error.
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Factor(): im_mxic_rw_master_mask_factor_or_target error.\n"));
	}

	return result;
}

/**
This function set target of transfer demand mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		group					Mask group.<br>
										Please refer to @ref E_IM_MXIC_MASK_GROUP for a set value.<br>
@param[in]		enable					Setting of whether to make a master into a mask target.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Master_Mask_Target( E_IM_MXIC_UNIT unit, UCHAR master, E_IM_MXIC_MASK_GROUP group, UCHAR enable )
{
	INT32								result;
	T_IM_MXIC_MASTER_MASK_FACTOR_TARGET	config;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_master_mask_factor_target_param( E_IM_MXIC_OPERATION_TYPE_SET, master, group, &enable ) != D_DDIM_OK ) {
		// Parameter error.
		Ddim_Assertion(("E:Im_MXIC_Set_Master_Mask_Target(): im_mxic_check_master_mask_factor_target_param error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	config.kind_set_get			= E_IM_MXIC_OPERATION_TYPE_SET;
	config.kind_factor_target	= E_IM_MXIC_MASTER_MASK_OPERATION_TARGET;
	config.master				= master;
	config.group				= group;
	config.enable				= enable;

	// Set master mask target.
	result = im_mxic_rw_master_mask_factor_or_target( unit, &config );

	if ( result != D_DDIM_OK ) {
		// Error.
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Target(): im_mxic_rw_master_mask_factor_or_target error.\n"));
	}

	Im_MXIC_Dsb();

	return result;
}

/**
This function get target of transfer demand mask.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		group					Mask group.<br>
										Please refer to @ref E_IM_MXIC_MASK_GROUP for a set value.<br>
@param[out]		enable					Setting of whether to make a master into a mask target.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_Mask_Target( E_IM_MXIC_UNIT unit, UCHAR master, E_IM_MXIC_MASK_GROUP group, UCHAR* const enable )
{
	INT32								result;
	T_IM_MXIC_MASTER_MASK_FACTOR_TARGET	config;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_master_mask_factor_target_param( E_IM_MXIC_OPERATION_TYPE_GET, master, group, enable ) != D_DDIM_OK ) {
		// Parameter error.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_Mask_Target(): im_mxic_check_master_mask_factor_target_param error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	config.kind_set_get			= E_IM_MXIC_OPERATION_TYPE_GET;
	config.kind_factor_target	= E_IM_MXIC_MASTER_MASK_OPERATION_TARGET;
	config.master				= master;
	config.group				= group;
	config.enable				= D_IM_MXIC_OFF;

	// Get master mask target.
	
	result = im_mxic_rw_master_mask_factor_or_target( unit, &config );

	if ( result == D_DDIM_OK ) {
		// Get return value.
		*enable = config.enable;
	}
	else {
		// Error.
		Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Target(): im_mxic_rw_master_mask_factor_or_target error.\n"));
	}

	return result;
}

/**
This function set the mask factor master state.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		select					Master state select.(0:master address ch demand period ,1:master address ch demand to data transfer complete)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Master_Mask_Select( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR select )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( ( select != D_IM_MXIC_ON ) && ( select != D_IM_MXIC_OFF ) ) {
		// select parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Master_Mask_Select(): select is illegal. select=%d\n", select));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( (master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK) == D_IM_MXIC_MASTER_TYPE_WRITE ) {
			// Set master mask state select of write master.
			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {

					im_mxic_on_pclk( unit );

					if ( select == D_IM_MXIC_ON ) {
						io_mxic->TRMCW[(master - 1)].bit.TRMFTW |= D_IM_MXIC_TRMC_TRMFT_SELECT;		// W1-63.
					} else {
						io_mxic->TRMCW[(master - 1)].bit.TRMFTW &= ~D_IM_MXIC_TRMC_TRMFT_SELECT;	// W1-63.
					}

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Mask_Select(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					if ( select == D_IM_MXIC_ON ) {
						io_mxic->TRMCW_GR[master].bit.TRMFTW |= D_IM_MXIC_TRMC_TRMFT_SELECT;		// W64-127.
					} else {
						io_mxic->TRMCW_GR[master].bit.TRMFTW &= ~D_IM_MXIC_TRMC_TRMFT_SELECT;		// W64-127.
					}

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Mask_Select(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if ( (master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {
			// Set master mask state select of read master.
			master &= D_IM_MXIC_READ_MASTER_FLG_MASK;

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {

					im_mxic_on_pclk( unit );

					if ( select == D_IM_MXIC_ON ) {
						io_mxic->TRMCR[(master - 1)].bit.TRMFTR |= D_IM_MXIC_TRMC_TRMFT_SELECT;		// R1-63.
					} else {
						io_mxic->TRMCR[(master - 1)].bit.TRMFTR &= ~D_IM_MXIC_TRMC_TRMFT_SELECT;	// R1-63.
					}

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Mask_Select(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					if ( select == D_IM_MXIC_ON ) {
						io_mxic->TRMCR_GR[master].bit.TRMFTR |= D_IM_MXIC_TRMC_TRMFT_SELECT;		// R64-127.
					} else {
						io_mxic->TRMCR_GR[master].bit.TRMFTR &= ~D_IM_MXIC_TRMC_TRMFT_SELECT;		// R64-127.
					}

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Mask_Select(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Select(): master is illegal. master=%d\n", master));
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Master_Mask_Select(): im_mxic_get_unit_address error.\n"));
	}

	Im_MXIC_Dsb();

	return result;
}

/**
This function get the mask factor master state.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		select					Master state select.(0:master address ch demand period ,1:master address ch demand to data transfer complete)<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_Mask_Select( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR* const select )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( select == NULL ) {
		// priority parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_Mask_Select(): select is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {
			// Set master mask state select of write master.
			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {

					im_mxic_on_pclk( unit );

					if ( ( io_mxic->TRMCW[(master - 1)].bit.TRMFTW & D_IM_MXIC_TRMC_TRMFT_SELECT ) != 0 ) {		// W1-63.
						*select = D_IM_MXIC_ON;
					}
					else {
						*select = D_IM_MXIC_OFF;
					}

					im_mxic_off_pclk( unit );
				}
				else {
					*select = D_IM_MXIC_OFF;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Mask_Select(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					if ( ( io_mxic->TRMCW_GR[master].bit.TRMFTW & D_IM_MXIC_TRMC_TRMFT_SELECT ) != 0 ) {		// W64-127.
						*select = D_IM_MXIC_ON;
					}
					else {
						*select = D_IM_MXIC_OFF;
					}

					im_mxic_off_pclk( unit );
				}
				else {
					*select = D_IM_MXIC_OFF;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Mask_Select(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {
			// Set master mask state select of read master.
			master &= D_IM_MXIC_READ_MASTER_FLG_MASK;

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {

					im_mxic_on_pclk( unit );

					if ( ( io_mxic->TRMCR[(master - 1)].bit.TRMFTR & D_IM_MXIC_TRMC_TRMFT_SELECT ) != 0 ) {		// R1-63.
						*select = D_IM_MXIC_ON;
					}
					else {
						*select = D_IM_MXIC_OFF;
					}

					im_mxic_off_pclk( unit );
				}
				else {
					*select = D_IM_MXIC_OFF;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Mask_Select(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					if ( ( io_mxic->TRMCR_GR[master].bit.TRMFTR & D_IM_MXIC_TRMC_TRMFT_SELECT ) != 0 ) {		// R64-127.
						*select = D_IM_MXIC_ON;
					}
					else {
						*select = D_IM_MXIC_OFF;
					}

					im_mxic_off_pclk( unit );
				}
				else {
					*select = D_IM_MXIC_OFF;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Mask_Select(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Select(): master is illegal. master=%d\n", master));
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Master_Mask_Select(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the max number of master transaction.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		select					Master transaction.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_Master_Transaction( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR transaction )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( transaction > D_IM_MXIC_TRMC_TISULIM_MAX_VAL ) {
		// select parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Master_Transaction(): transaction is illegal. transaction=%d\n", transaction));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {
			// Set master mask state select of write master.
			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {

					im_mxic_on_pclk( unit );

					io_mxic->TRMCW[(master - 1)].bit.TISULIMW = transaction;		// W1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Transaction(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					io_mxic->TRMCW_GR[master].bit.TISULIMW = transaction;			// W1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Transaction(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {
			// Set master mask state select of read master.
			master &= D_IM_MXIC_READ_MASTER_FLG_MASK;

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {

					im_mxic_on_pclk( unit );

					io_mxic->TRMCR[(master - 1)].bit.TISULIMR = transaction;		// R1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Transaction(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					io_mxic->TRMCR_GR[master].bit.TISULIMR = transaction;			// R64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_Master_Transaction(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:Im_MXIC_Set_Master_Transaction(): master is illegal. master=%d\n", master));
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
		}

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_Master_Transaction(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the max number of master transaction.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		master					Master number.<br>
@param[in]		transaction				Master transaction.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Master_Transaction( E_IM_MXIC_UNIT unit, UCHAR master, UCHAR* const transaction )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( transaction == NULL ) {
		// priority parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_Transaction(): select is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_WRITE ) {
			// Set master mask state select of write master.
			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {

					im_mxic_on_pclk( unit );

					*transaction = io_mxic->TRMCW[(master - 1)].bit.TISULIMW;		// W1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					*transaction = 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Transaction(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					*transaction = io_mxic->TRMCW_GR[master].bit.TISULIMW;			// W64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					*transaction = 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Transaction(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else if ( ( master & D_IM_MXIC_MASTER_TYPE_CHECK_MASK ) == D_IM_MXIC_MASTER_TYPE_READ ) {
			// Set master mask state select of read master.
			master &= D_IM_MXIC_READ_MASTER_FLG_MASK;

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {

					im_mxic_on_pclk( unit );

					*transaction = io_mxic->TRMCR[(master - 1)].bit.TISULIMR;		// R1-63.

					im_mxic_off_pclk( unit );
				}
				else {
					*transaction = 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Transaction(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				if ( unit == E_IM_MXIC_UNIT_3 ) {

					master = master - D_IM_MXIC_MASTER_NUMBER_MAX;

					im_mxic_on_pclk( unit );

					*transaction = io_mxic->TRMCR_GR[master].bit.TISULIMR;			// R64-127.

					im_mxic_off_pclk( unit );
				}
				else {
					*transaction = 0;

					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_Master_Transaction(): unit number is invalid. unit=%d\n", unit));
				}
			}
		}
		else {
			// Master number is illegal.
			Ddim_Print(("E:Im_MXIC_Get_Master_Transaction(): master is illegal. master=%d\n", master));
			result = D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Master_Transaction(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the LevelPort(Port).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		master					Master number.<br>
@param[in]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_LevelPort_Port( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, UCHAR master, E_IM_MXIC_PORT port )
{
	UCHAR						reg_read_sw;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_port( wr_arbiter, arbiter, port ) != D_DDIM_OK ) {
		// arbiter or port parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_LevelPort_Port(): im_mxic_check_port error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {
					// Write arbiter.
					switch ( arbiter ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							io_mxic->TMARLP_W_1[(master - 1)].bit.MAS_PORT_W = port;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							io_mxic->TMARLP_W_2[(master - 1)].bit.MAS_PORT_W = port;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							io_mxic->TMARLP_W_3[(master - 1)].bit.MAS_PORT_W = port;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							io_mxic->TMARLP_W_4[(master - 1)].bit.MAS_PORT_W = port;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Set_LevelPort_Port(): arbiter illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_LevelPort_Port(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				// master number is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Set_LevelPort_Port(): master number illegal. arbiter=%d\n", arbiter));
			}
		}
		else {
			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {
					// Read arbiter.
					switch ( arbiter ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							io_mxic->TMARLP_R_1[(master - 1)].bit.MAS_PORT_R = port;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							io_mxic->TMARLP_R_2[(master - 1)].bit.MAS_PORT_R = port;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							io_mxic->TMARLP_R_3[(master - 1)].bit.MAS_PORT_R = port;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							io_mxic->TMARLP_R_4[(master - 1)].bit.MAS_PORT_R = port;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Set_LevelPort_Port(): arbiter illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_LevelPort_Port(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				// master number is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Set_LevelPort_Port(): master number illegal. arbiter=%d\n", arbiter));
			}
		}

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_LevelPort_Port(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the LevelPort(Port).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		master					Master number.<br>
@param[out]		port					Target Port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_LevelPort_Port( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, UCHAR master, E_IM_MXIC_PORT* const port )
{
	UCHAR						reg_read_sw;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_arbiter( wr_arbiter, arbiter ) != D_DDIM_OK ) {
		// arbiter parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_LevelPort_Port(): im_mxic_check_arbiter error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( port == NULL ) {
		// port parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_LevelPort_Port(): port is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {
					// Write arbiter.
					switch ( arbiter ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							*port = (E_IM_MXIC_PORT)io_mxic->TMARLP_W_1[(master - 1)].bit.MAS_PORT_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							*port = (E_IM_MXIC_PORT)io_mxic->TMARLP_W_2[(master - 1)].bit.MAS_PORT_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							*port = (E_IM_MXIC_PORT)io_mxic->TMARLP_W_3[(master - 1)].bit.MAS_PORT_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							*port = (E_IM_MXIC_PORT)io_mxic->TMARLP_W_4[(master - 1)].bit.MAS_PORT_W;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Get_LevelPort_Port(): arbiter illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_LevelPort_Port(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				// master number is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Get_LevelPort_Port(): master number illegal. arbiter=%d\n", arbiter));
			}
		}
		else {
			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {
					// Read arbiter.
					switch ( arbiter ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							*port = (E_IM_MXIC_PORT)io_mxic->TMARLP_R_1[(master - 1)].bit.MAS_PORT_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							*port = (E_IM_MXIC_PORT)io_mxic->TMARLP_R_2[(master - 1)].bit.MAS_PORT_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							*port = (E_IM_MXIC_PORT)io_mxic->TMARLP_R_3[(master - 1)].bit.MAS_PORT_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							*port = (E_IM_MXIC_PORT)io_mxic->TMARLP_R_4[(master - 1)].bit.MAS_PORT_R;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Get_LevelPort_Port(): arbiter illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_LevelPort_Port(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				// master number is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Get_LevelPort_Port(): master number illegal. arbiter=%d\n", arbiter));
			}
		}

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_LevelPort_Port(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the LevelPort(Level).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		master					Master number.<br>
@param[in]		level					Target Level.<br>
										Please refer to @ref E_IM_MXIC_LEVEL for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_LevelPort_Level( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, UCHAR master, E_IM_MXIC_LEVEL level )
{
	UCHAR						reg_read_sw;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_level( wr_arbiter, arbiter, level ) != D_DDIM_OK ) {
		// arbiter or port parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_LevelPort_Level(): im_mxic_check_level error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {
					// Write arbiter.
					switch ( arbiter ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							io_mxic->TMARLP_W_1[(master - 1)].bit.MAS_LEVEL_W = level;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							io_mxic->TMARLP_W_2[(master - 1)].bit.MAS_LEVEL_W = level;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							io_mxic->TMARLP_W_3[(master - 1)].bit.MAS_LEVEL_W = level;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							io_mxic->TMARLP_W_4[(master - 1)].bit.MAS_LEVEL_W = level;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Set_LevelPort_Level(): arbiter illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_LevelPort_Level(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				// master number is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Set_LevelPort_Level(): master number illegal. arbiter=%d\n", arbiter));
			}
		}
		else {
			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {
					// Read arbiter.
					switch ( arbiter ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							io_mxic->TMARLP_R_1[(master - 1)].bit.MAS_LEVEL_R = level;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							io_mxic->TMARLP_R_2[(master - 1)].bit.MAS_LEVEL_R = level;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							io_mxic->TMARLP_R_3[(master - 1)].bit.MAS_LEVEL_R = level;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							io_mxic->TMARLP_R_4[(master - 1)].bit.MAS_LEVEL_R = level;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Set_LevelPort_Level(): arbiter illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Set_LevelPort_Level(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				// master number is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Set_LevelPort_Level(): master number illegal. arbiter=%d\n", arbiter));
			}
		}

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_LevelPort_Level(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the LevelPort(Level).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value.<br>
@param[in]		master					Master number.<br>
@param[out]		level					Target Level.<br>
										Please refer to @ref E_IM_MXIC_LEVEL for a set value.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_LevelPort_Level( E_IM_MXIC_UNIT unit, E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, UCHAR master, E_IM_MXIC_LEVEL* const level )
{
	UCHAR						reg_read_sw;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_arbiter( wr_arbiter, arbiter ) != D_DDIM_OK ) {
		// arbiter parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_LevelPort_Level(): im_mxic_check_arbiter error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( level == NULL ) {
		// level parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_LevelPort_Level(): level is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_4 ) {
					// Write arbiter.
					switch ( arbiter ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							*level = (E_IM_MXIC_LEVEL)io_mxic->TMARLP_W_1[(master - 1)].bit.MAS_LEVEL_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							*level = (E_IM_MXIC_LEVEL)io_mxic->TMARLP_W_2[(master - 1)].bit.MAS_LEVEL_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							*level = (E_IM_MXIC_LEVEL)io_mxic->TMARLP_W_3[(master - 1)].bit.MAS_LEVEL_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							*level = (E_IM_MXIC_LEVEL)io_mxic->TMARLP_W_4[(master - 1)].bit.MAS_LEVEL_W;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Get_LevelPort_Level(): arbiter illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_LevelPort_Level(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				// master number is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Get_LevelPort_Level(): master number illegal. arbiter=%d\n", arbiter));
			}
		}
		else {
			if ( master < D_IM_MXIC_MASTER_NUMBER_MAX ) {

				if ( unit != E_IM_MXIC_UNIT_0 ) {
					// Read arbiter.
					switch ( arbiter ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							*level = (E_IM_MXIC_LEVEL)io_mxic->TMARLP_R_1[(master - 1)].bit.MAS_LEVEL_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							*level = (E_IM_MXIC_LEVEL)io_mxic->TMARLP_R_2[(master - 1)].bit.MAS_LEVEL_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							*level = (E_IM_MXIC_LEVEL)io_mxic->TMARLP_R_3[(master - 1)].bit.MAS_LEVEL_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							*level = (E_IM_MXIC_LEVEL)io_mxic->TMARLP_R_4[(master - 1)].bit.MAS_LEVEL_R;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Get_LevelPort_Level(): arbiter illegal. arbiter=%d\n", arbiter));
							break;
					}
				}
				else {
					// Unit number is invalid.
//					Ddim_Print(("Im_MXIC_Get_LevelPort_Level(): unit number is invalid. unit=%d\n", unit));
				}
			}
			else {
				// master number is illegal.
				result = D_IM_MXIC_INPUT_PARAM_ERROR;
				Ddim_Print(("E:Im_MXIC_Get_LevelPort_Level(): master number illegal. arbiter=%d\n", arbiter));
			}
		}

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_LevelPort_Level(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the LevelPort(All).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		all_levelport			All LevelPort information.<br>
										Please refer to @ref T_IM_MXIC_ALL_LEVELPORT.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Set_LevelPort_All( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_LEVELPORT* all_levelport )
{
	UCHAR						reg_read_sw;
	INT32						result;
	INT32						i, j;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( all_levelport == NULL ) {
		// all_levelport parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_LevelPort_All(): all_levelport is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( unit != E_IM_MXIC_UNIT_4 ) {

			// Write arbiter.
			for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

				for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

					switch ( i ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							io_mxic->TMARLP_W_1[ j ].bit.MAS_PORT_W		= all_levelport->w_arbiter[ i ].port[ j ];
							io_mxic->TMARLP_W_1[ j ].bit.MAS_LEVEL_W	= all_levelport->w_arbiter[ i ].level[ j ];
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							io_mxic->TMARLP_W_2[ j ].bit.MAS_PORT_W		= all_levelport->w_arbiter[ i ].port[ j ];
							io_mxic->TMARLP_W_2[ j ].bit.MAS_LEVEL_W	= all_levelport->w_arbiter[ i ].level[ j ];
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							io_mxic->TMARLP_W_3[ j ].bit.MAS_PORT_W		= all_levelport->w_arbiter[ i ].port[ j ];
							io_mxic->TMARLP_W_3[ j ].bit.MAS_LEVEL_W	= all_levelport->w_arbiter[ i ].level[ j ];
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							io_mxic->TMARLP_W_4[ j ].bit.MAS_PORT_W		= all_levelport->w_arbiter[ i ].port[ j ];
							io_mxic->TMARLP_W_4[ j ].bit.MAS_LEVEL_W	= all_levelport->w_arbiter[ i ].level[ j ];
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Set_LevelPort_All(): write arbiter illegal. arbiter=%d\n", i));
							break;
					}
				}
			}
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Set_LevelPort_All(): unit number is invalid. unit=%d\n", unit));
		}

		if ( unit != E_IM_MXIC_UNIT_0 ) {

			// Read arbiter.
			for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

				for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

					switch ( i ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							io_mxic->TMARLP_R_1[ j ].bit.MAS_PORT_R		= all_levelport->r_arbiter[ i ].port[ j ];
							io_mxic->TMARLP_R_1[ j ].bit.MAS_LEVEL_R	= all_levelport->r_arbiter[ i ].level[ j ];
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							io_mxic->TMARLP_R_2[ j ].bit.MAS_PORT_R		= all_levelport->r_arbiter[ i ].port[ j ];
							io_mxic->TMARLP_R_2[ j ].bit.MAS_LEVEL_R	= all_levelport->r_arbiter[ i ].level[ j ];
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							io_mxic->TMARLP_R_3[ j ].bit.MAS_PORT_R		= all_levelport->r_arbiter[ i ].port[ j ];
							io_mxic->TMARLP_R_3[ j ].bit.MAS_LEVEL_R	= all_levelport->r_arbiter[ i ].level[ j ];
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							io_mxic->TMARLP_R_4[ j ].bit.MAS_PORT_R		= all_levelport->r_arbiter[ i ].port[ j ];
							io_mxic->TMARLP_R_4[ j ].bit.MAS_LEVEL_R	= all_levelport->r_arbiter[ i ].level[ j ];
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Set_LevelPort_All(): read arbiter illegal. arbiter=%d\n", i));
							break;
					}
				}
			}
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Set_LevelPort_All(): unit number is invalid. unit=%d\n", unit));
		}

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Set_LevelPort_All(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function get the LevelPort(All).<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_levelport			All LevelPort information.<br>
										Please refer to @ref T_IM_MXIC_ALL_LEVELPORT.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_LevelPort_All( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_LEVELPORT* all_levelport )
{
	UCHAR						reg_read_sw;
	INT32						result;
	INT32						i, j;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( all_levelport == NULL ) {
		// all_levelport parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_LevelPort_All(): all_levelport is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		reg_read_sw = io_mxic->TRVSW.bit.TRVSW;

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
		}

		if ( unit != E_IM_MXIC_UNIT_4 ) {

			// Write arbiter.
			for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

				for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

					switch ( i ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							all_levelport->w_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)io_mxic->TMARLP_W_1[ j ].bit.MAS_PORT_W;
							all_levelport->w_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)io_mxic->TMARLP_W_1[ j ].bit.MAS_LEVEL_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							all_levelport->w_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)io_mxic->TMARLP_W_2[ j ].bit.MAS_PORT_W;
							all_levelport->w_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)io_mxic->TMARLP_W_2[ j ].bit.MAS_LEVEL_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							all_levelport->w_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)io_mxic->TMARLP_W_3[ j ].bit.MAS_PORT_W;
							all_levelport->w_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)io_mxic->TMARLP_W_3[ j ].bit.MAS_LEVEL_W;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							all_levelport->w_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)io_mxic->TMARLP_W_4[ j ].bit.MAS_PORT_W;
							all_levelport->w_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)io_mxic->TMARLP_W_4[ j ].bit.MAS_LEVEL_W;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Get_LevelPort_All(): write arbiter illegal. arbiter=%d\n", i));
							break;
					}
				}
			}
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Get_LevelPort_All(): unit number is invalid. unit=%d\n", unit));
		}

		if ( unit != E_IM_MXIC_UNIT_0 ) {

			// Read arbiter.
			for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

				for ( j = 0; j < D_IM_MXIC_MASTER_NUMBER_MAX-1; j++ ) {

					switch ( i ) {
						case E_IM_MXIC_SPEC_ARBITER_1:
							all_levelport->r_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)io_mxic->TMARLP_R_1[ j ].bit.MAS_PORT_R;
							all_levelport->r_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)io_mxic->TMARLP_R_1[ j ].bit.MAS_LEVEL_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_2:
							all_levelport->r_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)io_mxic->TMARLP_R_2[ j ].bit.MAS_PORT_R;
							all_levelport->r_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)io_mxic->TMARLP_R_2[ j ].bit.MAS_LEVEL_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_3:
							all_levelport->r_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)io_mxic->TMARLP_R_3[ j ].bit.MAS_PORT_R;
							all_levelport->r_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)io_mxic->TMARLP_R_3[ j ].bit.MAS_LEVEL_R;
							break;
						case E_IM_MXIC_SPEC_ARBITER_4:
							all_levelport->r_arbiter[ i ].port[ j ]		= (E_IM_MXIC_PORT)io_mxic->TMARLP_R_4[ j ].bit.MAS_PORT_R;
							all_levelport->r_arbiter[ i ].level[ j ]	= (E_IM_MXIC_LEVEL)io_mxic->TMARLP_R_4[ j ].bit.MAS_LEVEL_R;
							break;
						default:
							// arbiter parameter is illegal.
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
							Ddim_Print(("E:Im_MXIC_Get_LevelPort_All(): read arbiter illegal. arbiter=%d\n", i));
							break;
					}
				}
			}
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Get_LevelPort_All(): unit number is invalid. unit=%d\n", unit));
		}

		if ( reg_read_sw == D_IM_MXIC_ON ) {
			io_mxic->TRVSW.bit.TRVSW = D_IM_MXIC_ON;
		}

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_LevelPort_All(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
"Service history monitor" and "Access count or amount of transfer monitor" 
are started with this function.<br>
*/
VOID Im_MXIC_Start_Monitor( VOID )
{
	// Monitor processing ended flag is cleared.
	gIM_MXIC_Trans_Mon_Limit_End_State = D_IM_MXIC_MONITOR_OPERATION_PROCESS;

	// Start monitor.
	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	IO_MXIC3.TMTRG.bit.TMTRG = 1;

	if ( IO_MXIC3.TMMD.bit.TMON == D_IM_MXIC_MONITOR_START_TRG ) {
		// When start conditions are TONTRG=1 writing. TONTRG=1 is performed here.
		IO_MXIC3.TMONTRG.bit.TONTRG = 1;
	}

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	Im_MXIC_Dsb();

	return;
}

/**
"Service history monitor" and "Access count or amount of transfer monitor" 
are stoped with this function.<br>
*/
VOID Im_MXIC_Stop_Monitor( VOID )
{
	// Stop monitor.
	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	IO_MXIC3.TMTRG.bit.TMTRG = 0;

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	Im_MXIC_Dsb();

	// Monitor processing ended flag is cleared.
	gIM_MXIC_Trans_Mon_Limit_End_State = D_IM_MXIC_MONITOR_OPERATION_PROCESS;
}

/**
Set whether to stop service history monitor operation in a THSTOP terminal input.<br>
@param[in]		enable					Setting of whether to stop service history monitor operation in a THSTOP terminal input.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
INT32 Im_MXIC_Set_History_Monitor_Stop_Enable( UCHAR enable )
{
#ifdef CO_PARAM_CHECK
	if ( ( enable != D_IM_MXIC_ON) && (enable != D_IM_MXIC_OFF ) ) {
		// enable parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_History_Monitor_Stop_Enable(): enable is illegal. enable=%d\n", enable));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	IO_MXIC3.TMMD.bit.THSEN = enable;

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	Im_MXIC_Dsb();

	return D_DDIM_OK;
}

/**
Get whether to stop service history monitor operation in a THSTOP terminal input.<br>
@param[out]		enable					Setting of whether to stop service history monitor operation in a THSTOP terminal input.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
*/
INT32 Im_MXIC_Get_History_Monitor_Stop_Enable( UCHAR* const enable )
{
#ifdef CO_PARAM_CHECK
	if ( enable == NULL ) {
		// enable parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_History_Monitor_Stop_Enable(): enable is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	*enable = IO_MXIC3.TMMD.bit.THSEN;

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	return D_DDIM_OK;
}

/**
The service history monitor result of the specified port is acquired. <br>
param[in]		wr_arbiter				Target write or read Arbiter.<br>
										Please refer to @ref E_IM_MXIC_WR_ARBITER for a set value.<br>
@param[in]		arbiter					Target arbiter.<br>
										Please refer to @ref E_IM_MXIC_SPEC_ARBITER for a set value. <br>
@param[in]		port					Target port.<br>
										Please refer to @ref E_IM_MXIC_PORT for a set value. <br>
@param[out]		history					Result of service history monitor.<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the port for a monitor.</ul>
*/
INT32 Im_MXIC_Get_History_Monitor( E_IM_MXIC_WR_ARBITER wr_arbiter, E_IM_MXIC_SPEC_ARBITER arbiter, E_IM_MXIC_PORT port, T_IM_MXIC_HISTORY_MONITOR* const history )
{
#ifdef CO_PARAM_CHECK
	if ( im_mxic_check_port( wr_arbiter, arbiter, port ) != D_DDIM_OK ) {
		// Parameter error.
		Ddim_Assertion(("E:Im_MXIC_Get_History_Monitor(): im_mxic_check_port error.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}

	if ( history == NULL ) {
		// history parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_History_Monitor(): history is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	if ( wr_arbiter == E_IM_MXIC_WR_ARBITER_W ) {

		im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

		// Get service history of W port.
		history->history[0]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_0;
		history->history[1]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_1;
		history->history[2]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_2;
		history->history[3]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_3;
		history->history[4]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_4;
		history->history[5]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_5;
		history->history[6]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_6;
		history->history[7]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_7;
		history->history[8]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_8;
		history->history[9]		= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_9;
		history->history[10]	= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_10;
		history->history[11]	= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_11;
		history->history[12]	= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_12;
		history->history[13]	= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_13;
		history->history[14]	= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_14;
		history->history[15]	= IO_MXIC3.THSTW[arbiter][port].bit.THSTW_15;

		im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );
	}
	else {
		im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

		// Get service history of R port.
		history->history[0]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_0;
		history->history[1]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_1;
		history->history[2]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_2;
		history->history[3]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_3;
		history->history[4]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_4;
		history->history[5]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_5;
		history->history[6]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_6;
		history->history[7]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_7;
		history->history[8]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_8;
		history->history[9]		= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_9;
		history->history[10]	= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_10;
		history->history[11]	= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_11;
		history->history[12]	= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_12;
		history->history[13]	= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_13;
		history->history[14]	= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_14;
		history->history[15]	= IO_MXIC3.THSTR[arbiter][port].bit.THSTR_15;

		im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );
	}

	return D_DDIM_OK;
}

/**
The service history monitor result of the all ports is acquired. <br>
@param[out]		all_history				Result of service history monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the port for a monitor.</ul>
*/
INT32 Im_MXIC_Get_History_Monitor_All_Port( T_IM_MXIC_ALL_HISTORY_MONITOR* const all_history )
{
	INT32 result;
	INT32 i, j;
	
#ifdef CO_PARAM_CHECK
	if ( all_history == NULL ) {
		// all_history parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_History_Monitor_All_Port(): all_history is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	// Get service history monitor result of W ports.
	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {

			result = Im_MXIC_Get_History_Monitor( E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, &all_history->w_arbiter[i][j] );

			if ( result != D_DDIM_OK ) {
				// Error
				Ddim_Print(("E:Im_MXIC_Get_History_Monitor_All_Port(): Im_MXIC_Get_History_Monitor error.(W)\n"));
				return result;
			}
		}
	}

	// Get service history monitor result of R ports.
	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		for ( j = E_IM_MXIC_PORT_0; j < E_IM_MXIC_PORT_MAX; j++ ) {

			result = Im_MXIC_Get_History_Monitor( E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)i, (E_IM_MXIC_PORT)j, &all_history->r_arbiter[i][j] );

			if ( result != D_DDIM_OK ) {
				// Error
				Ddim_Print(("E:Im_MXIC_Get_History_Monitor_All_Port(): Im_MXIC_Get_History_Monitor error.(R)\n"));
				return result;
			}
		}
	}

	return D_DDIM_OK;
}

/**
The slot status monitor result of the specified write channel arbiter is acquired. <br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		arbiter					Target write channel arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER for a set value. <br>
@param[out]		status					Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
INT32 Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_W_ARBITER arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_PORT* const status )
{
	INT32						result;
	INT32						i, j, k;
	E_IM_MXIC_SLOT				slot;
	UCHAR						slot_status[D_IM_MXIC_SLOT_SIZE_8];
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( arbiter >= E_IM_MXIC_W_ARBITER_MAX ) {
		// arbiter parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter(): arbiter is illegal.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( status == NULL ) {
		// status parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter(): status is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_wait_command_enable( unit, io_mxic );		// Wait TCFLG=1.

		im_mxic_on_pclk( unit );

		for ( i = 0; i < E_IM_MXIC_PORT_MAX; i++ ) {

			for ( j = 0; j < E_IM_MXIC_SLOT_MAX; j++ ) {
				// Convert value type. (INT32 to E_IM_MXIC_SLOT)
				slot = (E_IM_MXIC_SLOT)j;

				// Get status of specified slot.
				result = im_mxic_get_slot_status_array( unit, io_mxic, E_IM_MXIC_WR_ARBITER_W, (E_IM_MXIC_SPEC_ARBITER)arbiter, (E_IM_MXIC_PORT)i, slot, slot_status );

				if ( result == D_DDIM_OK ) {
					switch ( slot ) {
						case E_IM_MXIC_SLOT_0:
							// Get status of slot-0.
							for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {
								status->port[i].slot0[k] = slot_status[k];
							}
							break;
						case E_IM_MXIC_SLOT_1:
							// Get status of slot-1.
							for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {
								status->port[i].slot1[k] = slot_status[k];
							}
							break;
						case E_IM_MXIC_SLOT_2:
							// Get status of slot-2.
							for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {
								status->port[i].slot2[k] = slot_status[k];
							}
							break;
						case E_IM_MXIC_SLOT_3:
							// Get status of slot-3.
							for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {
								status->port[i].slot3[k] = slot_status[k];
							}
							break;
						default:
							// slot is illegal.
							Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter(): slot is illegal. slot=%d\n", slot));
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
					}
				}
			}
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
The slot status monitor result of the specified read channel arbiter is acquired. <br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		arbiter					Target read channel arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER for a set value. <br>
@param[out]		status					Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
INT32 Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter( E_IM_MXIC_UNIT unit, E_IM_MXIC_R_ARBITER arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_PORT* const status )
{
	INT32						result;
	INT32						i, j, k;
	E_IM_MXIC_SLOT				slot;
	UCHAR						slot_status[D_IM_MXIC_SLOT_SIZE_8];
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( arbiter >= E_IM_MXIC_R_ARBITER_MAX ) {
		// arbiter parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter(): arbiter is illegal.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( status == NULL ) {
		// status parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter(): status is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_wait_command_enable( unit, io_mxic );		// Wait TCFLG=1.

		im_mxic_on_pclk( unit );

		for ( i = 0; i < E_IM_MXIC_PORT_MAX; i++ ) {

			for ( j = 0; j < E_IM_MXIC_SLOT_MAX; j++ ) {
				// Convert value type. (INT32 to E_IM_MXIC_SLOT)
				slot = (E_IM_MXIC_SLOT)j;

				// Get status of specified slot.
				result = im_mxic_get_slot_status_array( unit, io_mxic, E_IM_MXIC_WR_ARBITER_R, (E_IM_MXIC_SPEC_ARBITER)arbiter, (E_IM_MXIC_PORT)i, slot, slot_status );

				if ( result == D_DDIM_OK ) {
					switch ( slot ) {
						case E_IM_MXIC_SLOT_0:
							// Get status of slot-0.
							for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {
								status->port[i].slot0[k] = slot_status[k];
							}
							break;
						case E_IM_MXIC_SLOT_1:
							// Get status of slot-1.
							for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {
								status->port[i].slot1[k] = slot_status[k];
							}
							break;
						case E_IM_MXIC_SLOT_2:
							// Get status of slot-2.
							for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {
								status->port[i].slot2[k] = slot_status[k];
							}
							break;
						case E_IM_MXIC_SLOT_3:
							// Get status of slot-3.
							for ( k = 0; k < D_IM_MXIC_SLOT_SIZE_8; k++ ) {
								status->port[i].slot3[k] = slot_status[k];
							}
							break;
						default:
							// slot is illegal.
							Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter(): slot is illegal. slot=%d\n", slot));
							result = D_IM_MXIC_INPUT_PARAM_ERROR;
					}
				}
			}
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
The slot status monitor result of the specified write channel arbiter is acquired. (group)<br>
@param[in]		arbiter					Target write channel arbiter.<br>
										Please refer to @ref E_IM_MXIC_W_ARBITER_GR for a set value. <br>
@param[out]		status					Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
INT32	Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group( E_IM_MXIC_W_ARBITER_GR arbiter, T_IM_MXIC_W_ARBITER_ASSIGN_GR* const status )
{
#ifdef CO_PARAM_CHECK
	if ( arbiter >= E_IM_MXIC_W_ARBITER_GR_MAX ) {
		// arbiter parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group(): arbiter is illegal.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( status == NULL ) {
		// status parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group(): status is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_wait_command_enable( E_IM_MXIC_UNIT_3, (struct io_jdsmxic *)&IO_MXIC3 );	// Wait TCFLG=1.

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	status->slot[0] = IO_MXIC3.TGSLSW[arbiter].bit.GSLOTSW_0;
	status->slot[1] = IO_MXIC3.TGSLSW[arbiter].bit.GSLOTSW_1;
	status->slot[2] = IO_MXIC3.TGSLSW[arbiter].bit.GSLOTSW_2;
	status->slot[3] = IO_MXIC3.TGSLSW[arbiter].bit.GSLOTSW_3;
	status->slot[4] = IO_MXIC3.TGSLSW[arbiter].bit.GSLOTSW_4;
	status->slot[5] = IO_MXIC3.TGSLSW[arbiter].bit.GSLOTSW_5;
	status->slot[6] = IO_MXIC3.TGSLSW[arbiter].bit.GSLOTSW_6;
	status->slot[7] = IO_MXIC3.TGSLSW[arbiter].bit.GSLOTSW_7;

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	return D_DDIM_OK;
}

/**
The slot status monitor result of the specified read channel arbiter is acquired. (group)<br>
@param[in]		arbiter					Target read channel arbiter.<br>
										Please refer to @ref E_IM_MXIC_R_ARBITER_GR for a set value. <br>
@param[out]		status					Result of slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
INT32	Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group( E_IM_MXIC_R_ARBITER_GR arbiter, T_IM_MXIC_R_ARBITER_ASSIGN_GR* const status )
{
#ifdef CO_PARAM_CHECK
	if (arbiter >= E_IM_MXIC_R_ARBITER_GR_MAX ) {
		// arbiter parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group(): arbiter is illegal.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( status == NULL ) {
		// status parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group(): status is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_wait_command_enable( E_IM_MXIC_UNIT_3, (struct io_jdsmxic *)&IO_MXIC3 );	// Wait TCFLG=1.

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	status->slot[0] = IO_MXIC3.TGSLSR[arbiter].bit.GSLOTSR_0;
	status->slot[1] = IO_MXIC3.TGSLSR[arbiter].bit.GSLOTSR_1;
	status->slot[2] = IO_MXIC3.TGSLSR[arbiter].bit.GSLOTSR_2;
	status->slot[3] = IO_MXIC3.TGSLSR[arbiter].bit.GSLOTSR_3;
	status->slot[4] = IO_MXIC3.TGSLSR[arbiter].bit.GSLOTSR_4;
	status->slot[5] = IO_MXIC3.TGSLSR[arbiter].bit.GSLOTSR_5;
	status->slot[6] = IO_MXIC3.TGSLSR[arbiter].bit.GSLOTSR_6;
	status->slot[7] = IO_MXIC3.TGSLSR[arbiter].bit.GSLOTSR_7;

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	return D_DDIM_OK;
}

/**
The slot status monitor result of all arbiter is acquired. <br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		all_status				Result of all slot status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Please use this function on condition of following either.<br>
				<ul><li>Under a monitor operation stop.
					<li>The state which the transmission demand has not generated in the master currently assigned to the arbiter for a monitor.</ul>
*/
INT32 Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter( E_IM_MXIC_UNIT unit, T_IM_MXIC_ALL_SLOT_STATUS_MONITOR* const all_status )
{
	INT32 result;
	INT32 i;

#ifdef CO_PARAM_CHECK
	if ( all_status == NULL ) {
		// all_status parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter(): all_status is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	// Get slot status of all W arbiter.
	for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

		result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter( unit, (E_IM_MXIC_W_ARBITER)i, &(all_status->w_arbiter[i]) );

		if ( result != D_DDIM_OK ) {
			// Error.
			Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter(): Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter error.\n"));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}

	// Get slot status of all R arbiter.
	for ( i = E_IM_MXIC_R_ARBITER_R1; i < E_IM_MXIC_R_ARBITER_MAX; i++ ) {

		result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter( unit, (E_IM_MXIC_R_ARBITER)i, &(all_status->r_arbiter[i]) );

		if ( result != D_DDIM_OK ) {
			// Error.
			Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter(): Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter error.\n"));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}

	// Get slot status of all group-W arbiter.
	for ( i = E_IM_MXIC_W_ARBITER_GR_W0; i < E_IM_MXIC_W_ARBITER_GR_MAX; i++ ) {

		result = Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group( (E_IM_MXIC_W_ARBITER_GR)i, &(all_status->w_arbiter_gr[i]) );

		if ( result != D_DDIM_OK ) {
			// Error.
			Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter(): Im_MXIC_Get_Slot_Status_Monitor_W_Arbiter_Group error.\n"));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}

	// Get slot status of all group-R arbiter.
	for ( i = E_IM_MXIC_R_ARBITER_GR_R0; i < E_IM_MXIC_R_ARBITER_GR_MAX; i++ ) {

		result = Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group( (E_IM_MXIC_R_ARBITER_GR)i, &(all_status->r_arbiter_gr[i]) );

		if ( result != D_DDIM_OK ) {
			// Error.
			Ddim_Print(("E:Im_MXIC_Get_Slot_Status_Monitor_All_Arbiter(): Im_MXIC_Get_Slot_Status_Monitor_R_Arbiter_Group error.\n"));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}

	return D_DDIM_OK;
}

/**
The master status monitor result is acquired.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		status					Result of all master status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The master number corresponds to the index of "status.w_master" and "status.r_master" array.<br>
				An example is shown below.<br>
				JDSPRO I/F-0 Write master status = status.w_master[D_IM_MXIC_W_PRO_IF_0]<br>
				JDSDISP I/F-0 Read master status = status.r_master[D_IM_MXIC_R_DISP_IF_0]<br>
*/
INT32 Im_MXIC_Get_Master_Status_Monitor( E_IM_MXIC_UNIT unit, T_IM_MXIC_MASTER_STATUS_MONITOR* const status )
{
	INT32						master_counter;
	INT32						reg_counter;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( status == NULL ) {
		// status parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_Status_Monitor(): status is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		// Clear reserve area.
		status->w_master[0] = 0;
		status->r_master[0] = 0;

		// Initialize counter.
		master_counter = 1;
		reg_counter    = 0;

		im_mxic_on_pclk( unit );

		// Get W master status.
		if ( unit != E_IM_MXIC_UNIT_4 ) {

			for (;;) {
				status->w_master[master_counter] = io_mxic->TMSTW[reg_counter].bit.TMSTW_1;
				master_counter++;
				if ( master_counter >= D_IM_MXIC_MAX_W_MASTER_NUM ) {
					// End of enable master number.
					break;
				}
	
				status->w_master[master_counter] = io_mxic->TMSTW[reg_counter].bit.TMSTW_2;
				master_counter++;
				if ( master_counter >= D_IM_MXIC_MAX_W_MASTER_NUM ) {
					// End of enable master number.
					break;
				}
	
				status->w_master[master_counter] = io_mxic->TMSTW[reg_counter].bit.TMSTW_3;
				master_counter++;
				if ( master_counter >= D_IM_MXIC_MAX_W_MASTER_NUM ) {
					// End of enable master number.
					break;
				}
	
				status->w_master[master_counter] = io_mxic->TMSTW[reg_counter].bit.TMSTW_4;
				master_counter++;
				if ( master_counter >= D_IM_MXIC_MAX_W_MASTER_NUM ) {
					// End of enable master number.
					break;
				}
				reg_counter++;
			}
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Get_Master_Status_Monitor(): unit number is invalid. unit=%d\n", unit));
		}

		// Initialize counter.
		master_counter = 1;
		reg_counter    = 0;

		// Get R master status.
		if ( unit != E_IM_MXIC_UNIT_0 ) {

			for (;;) {
				status->r_master[master_counter] = io_mxic->TMSTR[reg_counter].bit.TMSTR_1;
				master_counter++;
				if ( master_counter >= D_IM_MXIC_MAX_R_MASTER_NUM ) {
					// End of enable master number.
					break;
				}
	
				status->r_master[master_counter] = io_mxic->TMSTR[reg_counter].bit.TMSTR_2;
				master_counter++;
				if ( master_counter >= D_IM_MXIC_MAX_R_MASTER_NUM ) {
					// End of enable master number.
					break;
				}
	
				status->r_master[master_counter] = io_mxic->TMSTR[reg_counter].bit.TMSTR_3;
				master_counter++;
				if ( master_counter >= D_IM_MXIC_MAX_R_MASTER_NUM ) {
					// End of enable master number.
					break;
				}
	
				status->r_master[master_counter] = io_mxic->TMSTR[reg_counter].bit.TMSTR_4;
				master_counter++;
				if ( master_counter >= D_IM_MXIC_MAX_R_MASTER_NUM ) {
					// End of enable master number.
					break;
				}
				reg_counter++;
			}
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Get_Master_Status_Monitor(): unit number is invalid. unit=%d\n", unit));
		}

		im_mxic_off_pclk( unit );
	}
	else {
		// Input parameter error.
		Ddim_Print(("E:Im_MXIC_Get_Master_Status_Monitor(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
The master status monitor result is acquired.(group)<br>
@param[out]		status					Result of all master status monitor.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The master number corresponds to the index of "status.w_master" and "status.r_master" array.<br>
				An example is shown below.<br>
				JDSPRO I/F-0 Write master status = status.w_master[D_IM_MXIC_W_PRO_IF_0]<br>
				JDSDISP I/F-0 Read master status = status.r_master[D_IM_MXIC_R_DISP_IF_0]<br>
*/
INT32 Im_MXIC_Get_Master_Status_Monitor_Group( T_IM_MXIC_MASTER_STATUS_MONITOR* const status )
{
	INT32 master_counter;
	INT32 reg_counter;

#ifdef CO_PARAM_CHECK
	if ( status == NULL ) {
		// status parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Master_Status_Monitor_Group(): status is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	// Clear reserve area.
	status->w_master[0] = 0;
	status->r_master[0] = 0;

	// Initialize counter.
	master_counter = 1;
	reg_counter    = 0;

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	// Get W master status.
	for (;;) {
		status->w_master[master_counter] = IO_MXIC3.TMSTW_GR[reg_counter].bit.TMSTW_1;
		master_counter++;
		if ( master_counter >= D_IM_MXIC_MAX_W_MASTER_NUM ) {
			// End of enable master number.
			break;
		}

		status->w_master[master_counter] = IO_MXIC3.TMSTW_GR[reg_counter].bit.TMSTW_2;
		master_counter++;
		if ( master_counter >= D_IM_MXIC_MAX_W_MASTER_NUM ) {
			// End of enable master number.
			break;
		}

		status->w_master[master_counter] = IO_MXIC3.TMSTW_GR[reg_counter].bit.TMSTW_3;
		master_counter++;
		if ( master_counter >= D_IM_MXIC_MAX_W_MASTER_NUM ) {
			// End of enable master number.
			break;
		}

		status->w_master[master_counter] = IO_MXIC3.TMSTW_GR[reg_counter].bit.TMSTW_4;
		master_counter++;
		if ( master_counter >= D_IM_MXIC_MAX_W_MASTER_NUM ) {
			// End of enable master number.
			break;
		}
		reg_counter++;
	}

	// Initialize counter.
	master_counter = 1;
	reg_counter    = 0;

	// Get R master status.
	for (;;) {
		status->r_master[master_counter] = IO_MXIC3.TMSTR_GR[reg_counter].bit.TMSTR_1;
		master_counter++;
		if ( master_counter >= D_IM_MXIC_MAX_R_MASTER_NUM ) {
			// End of enable master number.
			break;
		}

		status->r_master[master_counter] = IO_MXIC3.TMSTR_GR[reg_counter].bit.TMSTR_2;
		master_counter++;
		if ( master_counter >= D_IM_MXIC_MAX_R_MASTER_NUM ) {
			// End of enable master number.
			break;
		}

		status->r_master[master_counter] = IO_MXIC3.TMSTR_GR[reg_counter].bit.TMSTR_3;
		master_counter++;
		if ( master_counter >= D_IM_MXIC_MAX_R_MASTER_NUM ) {
			// End of enable master number.
			break;
		}

		status->r_master[master_counter] = IO_MXIC3.TMSTR_GR[reg_counter].bit.TMSTR_4;
		master_counter++;
		if ( master_counter >= D_IM_MXIC_MAX_R_MASTER_NUM ) {
			// End of enable master number.
			break;
		}
		reg_counter++;
	}

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	return D_DDIM_OK;
}

/**
This function set the parameter of access count or transfer amount monitor.<br>
@param[in]		param					Setting of monitor operation.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Change of a parameter is reflected at the time of the monitor start by @ref Im_MXIC_Start_Monitor function. 
*/
INT32 Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter( T_IM_MXIC_MONITOR_PARAMETER const* const param )
{
#ifdef CO_PARAM_CHECK
	INT32 i;

	if ( param == NULL ) {
		// param parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(): param is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( param->start_condition != D_IM_MXIC_MONITOR_START_OFF ) {
		for ( i = 0; i < D_IM_MXIC_MAX_MONITOR_TARGET_NUM; i++ ) {
			if ( im_mxic_check_master_number_for_monitor( param->target[i].monitor_target ) != D_DDIM_OK ) {
				// Monitor target master number is illegal.
				Ddim_Assertion(("E:Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(): monitor_target is illegal. (%02X)\n", param->target[i].monitor_target));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
		if ( param->end_condition == D_IM_MXIC_MONITOR_END_LIMIT ) {
			if ( im_mxic_check_master_number_for_monitor( param->limit_detection_target ) != D_DDIM_OK ) {
				// Limit judgment target master number is illegal.
				Ddim_Assertion(("E:Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(): limit_detection_target is illegal. (%02X)\n", param->limit_detection_target));
				return D_IM_MXIC_INPUT_PARAM_ERROR;
			}
		}
	}
#endif // CO_PARAM_CHECK

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	// Set monitor mode register. (TMMD)
	IO_MXIC3.TMMD.bit.TMON		= param->start_condition;						// Monitor start conditions.
	IO_MXIC3.TMMD.bit.TMOFF		= param->end_condition;							// Monitor end conditions.

	IO_MXIC3.TMMD.bit.TMUP		= param->update_enable;							// Update enabling of a monitor.
	IO_MXIC3.TMMD.bit.TMUPC		= param->update_trigger_interval;				// Update trigger generation interval.

	IO_MXIC3.TMMD.bit.TMCLR		= param->update_clear_en;						// Selection monitor clear by update trigger.
	IO_MXIC3.TMMD.bit.TMCLRC	= param->start_clear_en;						// Selection monitor clear by asserting of a monitor start terminal.

	IO_MXIC3.TMMD.bit.TMRSEL	= param->cnt_val;								// Selection switch of reading value.

	// Detailed setup is performed when using a monitor.
	if ( param->start_condition != D_IM_MXIC_MONITOR_START_OFF ) {				// TMON="00b"
		// Set monitor target and kind(Access or Transfer).
		IO_MXIC3.TMSEL.bit.TMAS_0	= param->target[0].monitor_sel;
		IO_MXIC3.TMSEL.bit.TMSEL_0	= param->target[0].monitor_target;
		IO_MXIC3.TMSEL.bit.TMAS_1	= param->target[1].monitor_sel;
		IO_MXIC3.TMSEL.bit.TMSEL_1	= param->target[1].monitor_target;
		IO_MXIC3.TMSEL.bit.TMAS_2	= param->target[2].monitor_sel;
		IO_MXIC3.TMSEL.bit.TMSEL_2	= param->target[2].monitor_target;
		IO_MXIC3.TMSEL.bit.TMAS_3	= param->target[3].monitor_sel;
		IO_MXIC3.TMSEL.bit.TMSEL_3	= param->target[3].monitor_target;

		if ( param->end_condition == D_IM_MXIC_MONITOR_END_LIMIT ) {			// TMOFF="11b"
			// Set limit judgment conditions.
			IO_MXIC3.TMLIMSEL.bit.TMALMS	= param->limit_access_trans;		// Selection limit judgment. (Access or Transfer)
			IO_MXIC3.TMLIMSEL.bit.TLIMSEL	= param->limit_detection_target;	// Selection limit judgment target.
			IO_MXIC3.TMLIMIT.bit.TMLIMT		= param->limit;						// Limit value.
		}
	}

	// The TMF interrupt is enable.
	IO_MXIC3.TMIE.bit.TME = 1;

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	Im_MXIC_Dsb();

	// Set callback for TMF interrupt.
	gIM_MXIC_Acs_Trans_Mon_Limit_End_CallBack = param->pCallBack;

	return D_DDIM_OK;
}

/**
This function get the parameter of access count or transfer amount monitor.<br>
@param[out]		param					Setting of monitor operation.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter( T_IM_MXIC_MONITOR_PARAMETER* const param )
{
#ifdef CO_PARAM_CHECK
	if ( param == NULL ) {
		// param parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter(): param is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	// Get monitor mode register. (TMMD)
	param->start_condition			= IO_MXIC3.TMMD.bit.TMON;			// Monitor start conditions.
	param->end_condition			= IO_MXIC3.TMMD.bit.TMOFF;			// Monitor end conditions.
	param->update_enable			= IO_MXIC3.TMMD.bit.TMUP;			// Update enabling of a monitor.
	param->update_trigger_interval	= IO_MXIC3.TMMD.bit.TMUPC;			// Update trigger generation interval.
	param->update_clear_en			= IO_MXIC3.TMMD.bit.TMCLR;			// Selection monitor clear by update trigger.
	param->start_clear_en			= IO_MXIC3.TMMD.bit.TMCLRC;			// Selection monitor clear by asserting of a monitor start terminal.
	param->cnt_val					= IO_MXIC3.TMMD.bit.TMRSEL;			// Selection switch of reading value.

	// Get monitor target and kind(Access or Transfer).
	param->target[0].monitor_sel    = IO_MXIC3.TMSEL.bit.TMAS_0;
	param->target[0].monitor_target = IO_MXIC3.TMSEL.bit.TMSEL_0;
	param->target[1].monitor_sel    = IO_MXIC3.TMSEL.bit.TMAS_1;
	param->target[1].monitor_target = IO_MXIC3.TMSEL.bit.TMSEL_1;
	param->target[2].monitor_sel    = IO_MXIC3.TMSEL.bit.TMAS_2;
	param->target[2].monitor_target = IO_MXIC3.TMSEL.bit.TMSEL_2;
	param->target[3].monitor_sel    = IO_MXIC3.TMSEL.bit.TMAS_3;
	param->target[3].monitor_target = IO_MXIC3.TMSEL.bit.TMSEL_3;

	// Get limit judgment conditions.
	param->limit_access_trans		= IO_MXIC3.TMLIMSEL.bit.TMALMS;		// Selection limit judgment. (Access or Transfer)
	param->limit_detection_target	= IO_MXIC3.TMLIMSEL.bit.TLIMSEL;	// Selection limit judgment target.
	param->limit					= IO_MXIC3.TMLIMIT.bit.TMLIMT;		// Limit value.

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	// Set callback for TMF interrupt.
	param->pCallBack = gIM_MXIC_Acs_Trans_Mon_Limit_End_CallBack;

	return D_DDIM_OK;
}

/**
This function get access count or transfer amount monitor result of specified entry.<br>
@param[in]		entry					Monitor number. (0~3)
@param[out]		result					Monitor result.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Access_Or_Trans_Monitor( UCHAR entry, UINT32* const result )
{
#ifdef CO_PARAM_CHECK
	if ( entry > 3 ) {
		// entry parameter value is illegal.
		Ddim_Assertion(("E:Im_MXIC_Get_Access_Or_Trans_Monitor(): entry is illegal. entry=%d\n", entry));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
	if ( result == NULL ) {
		// result parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Access_Or_Trans_Monitor(): result is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	if ( IO_MXIC3.TMMD.bit.TMRSEL == D_IM_MXIC_VALUE_WHEN_UPDATE_TRG ) {
		// Generate update trigger.
		IO_MXIC3.TMUPTRG.bit.TUPTRG = 1;
	}

	// Get monitor result.
	*result = IO_MXIC3.TMCNT[entry].bit.TMCNT;

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	return D_DDIM_OK;
}

/**
This function get access count or transfer amount monitor result of all entry.<br>
@param[out]		all_result				Monitor result of all entry.
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry( T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR* const all_result )
{
	INT32 i;

#ifdef CO_PARAM_CHECK
	if ( all_result == NULL ) {
		// all_result parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry(): all_result is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	if ( IO_MXIC3.TMMD.bit.TMRSEL == D_IM_MXIC_VALUE_WHEN_UPDATE_TRG ) {
		// Generate update trigger.
		IO_MXIC3.TMUPTRG.bit.TUPTRG = 1;
	}

	// Get all monitor result.
	for ( i = 0; i < D_IM_MXIC_MAX_MONITOR_TARGET_NUM; i++ ) {
		all_result->count[i] = IO_MXIC3.TMCNT[i].bit.TMCNT;
	}

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	return D_DDIM_OK;
}

/**
The state of access count or transfer amount monitor is acquired.<br>
Please use this function in only the following the monitor end condition.<br>
- Monitor is ended by the amount limit judging of transmission. (D_IM_MXIC_MONITOR_END_LIMIT)<br><br>
@param[out]		mon_state				Moniter limit end state.<br>
										Please refer to the following for moniter processing state.<br>
										<ul><li>@ref D_IM_MXIC_MONITOR_OPERATION_PROCESS
											<li>@ref D_IM_MXIC_MONITOR_OPERATION_END</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State( UCHAR* mon_state )
{
#ifdef CO_PARAM_CHECK
	if ( mon_state == NULL ) {
		// mon_state parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Access_Or_Trans_Monitor_Limit_End_State(): mon_state is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	*mon_state = gIM_MXIC_Trans_Mon_Limit_End_State;

	return D_DDIM_OK;
}

/**
Detection of Memory Access Area" are started with this function.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		param					Setting of detection parameters of slave each memory access.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Start_Memory_Access_Detect( E_IM_MXIC_UNIT unit, T_IM_MXIC_MEMORY_ACCESS_SLAVE  const* const param )
{
	INT32						i;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( param == NULL ) {
		// param parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Start_Memory_Access_Detect(): param is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( unit != E_IM_MXIC_UNIT_4 ) {

			im_mxic_on_pclk( unit );

			// Start detect memory access.
			for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {
				io_mxic->TSASETW[i].bit.TSATRGW = param->slave[i].start_trigger;
			}

			im_mxic_off_pclk( unit );

			Im_MXIC_Dsb();
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Start_Memory_Access_Detect(): unit number is invalid. unit=%d\n", unit));
		}
	}
	else {
		// Error.
		Ddim_Print(("E:Im_MXIC_Start_Memory_Access_Detect(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
Detection of Memory Access Area" are stoped with this function.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_MXIC_Stop_Memory_Access_Detect( E_IM_MXIC_UNIT unit )
{
	INT32						i;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( unit != E_IM_MXIC_UNIT_4 ) {

			// Stop monitor.
			im_mxic_on_pclk( unit );

			for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {
				io_mxic->TSASETW[i].bit.TSATRGW = 0;
			}

			im_mxic_off_pclk( unit );

			Im_MXIC_Dsb();
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Stop_Memory_Access_Detect(): unit number is invalid. unit=%d\n", unit));
		}
	}
	else {
		// Error.
		Ddim_Print(("E:Im_MXIC_Stop_Memory_Access_Detect(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set the parameter of Detection parameters of slave each memory access.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		param					Setting of detection parameters of slave each memory access.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Change of a parameter is reflected at the time of the detection start by @ref Im_MXIC_Start_Memory_Access_Detect function. 
*/
INT32 Im_MXIC_Set_Memory_Access_Detect( E_IM_MXIC_UNIT unit, T_IM_MXIC_MEMORY_ACCESS_SLAVE const* const param )
{
	INT32						i;
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( param == NULL ) {
		// param parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Memory_Access_Detect(): param is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( unit != E_IM_MXIC_UNIT_4 ) {

			im_mxic_on_pclk( unit );

			for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {
				memset((VOID*)&io_mxic->TSASETW[i], 0, sizeof(io_mxic->TSASETW[i]));
			}

			// Set detection of memory access register. (TSASETW)
			for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {

				if ( param->slave[i].start_trigger == D_IM_MXIC_ON ) {
					io_mxic->TSASETW[i].bit.TSAMASW47_1_LO		= (ULONG)((param->slave[i].master47_1   & D_IM_MXIC_MASTER_LO_MASK));
					io_mxic->TSASETW[i].bit.TSAMASW47_1_HI		= (ULONG)((param->slave[i].master47_1   & D_IM_MXIC_MASTER_HI_MASK) >> 32);
					io_mxic->TSASETW[i].bit.TSAMASW127_64_LO	= (ULONG)((param->slave[i].master127_64 & D_IM_MXIC_MASTER_LO_MASK));
					io_mxic->TSASETW[i].bit.TSAMASW127_64_HI	= (ULONG)((param->slave[i].master127_64 & D_IM_MXIC_MASTER_HI_MASK) >> 32);
					io_mxic->TSASETW[i].bit.TSASADW				= param->slave[i].start_address;
					io_mxic->TSASETW[i].bit.TSAEADW				= param->slave[i].end_address;
					io_mxic->TSASETW[i].bit.TSARW				= param->slave[i].mode;
				}
			}

			// The TMF interrupt is enable.
			io_mxic->TMIE.bit.TSAWE = 1;

			im_mxic_off_pclk( unit );

			Im_MXIC_Dsb();

			// Set callback for TMF interrupt.
			gIM_MXIC_Memory_Access_Detect_CallBack[unit] = param->pCallBack;
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Set_Memory_Access_Detect(): unit number is invalid. unit=%d\n", unit));
		}
	}
	else {
		// Error.
		Ddim_Print(("E:Im_MXIC_Set_Memory_Access_Detect(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}


/**
This function get the parameter of Detection parameters of slave each memory access.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		param					Setting of detection parameters of slave each memory access.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Change of a parameter is reflected at the time of the detection start by @ref Im_MXIC_Start_Memory_Access_Detect function. 
*/
INT32 Im_MXIC_Get_Memory_Access_Detect( E_IM_MXIC_UNIT unit, T_IM_MXIC_MEMORY_ACCESS_SLAVE* const param )
{
	INT32						i;
	INT32						result;
	UINT64						w_47_1		= 0;
	UINT64						w_127_64	= 0;
	volatile struct io_jdsmxic*	io_mxic;

#ifdef CO_PARAM_CHECK
	if ( param == NULL ) {
		// param parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Memory_Access_Detect(): param is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		if ( unit != E_IM_MXIC_UNIT_4 ) {

			im_mxic_on_pclk( unit );

			// Set detection of memory access register. (TSASETW)
			for ( i = E_IM_MXIC_W_ARBITER_W1; i < E_IM_MXIC_W_ARBITER_MAX; i++ ) {
				param->slave[i].start_trigger	= io_mxic->TSASETW[i].bit.TSATRGW;
				w_47_1							= io_mxic->TSASETW[i].bit.TSAMASW47_1_HI;
				w_47_1							= (w_47_1 << 32) | io_mxic->TSASETW[i].bit.TSAMASW47_1_LO;
				param->slave[i].master47_1		= w_47_1;
				w_127_64						= io_mxic->TSASETW[i].bit.TSAMASW127_64_HI;
				w_127_64						= (w_127_64 << 32) | io_mxic->TSASETW[i].bit.TSAMASW127_64_LO;
				param->slave[i].master127_64	= w_127_64;
				param->slave[i].start_address	= io_mxic->TSASETW[i].bit.TSASADW;
				param->slave[i].end_address		= io_mxic->TSASETW[i].bit.TSAEADW;
				param->slave[i].mode			= io_mxic->TSASETW[i].bit.TSARW;
				param->slave[i].detect_master	= io_mxic->TSASETW[i].bit.TSAMNSTW;
				param->slave[i].detect_address	= io_mxic->TSASETW[i].bit.TSAADSTW;
			}

			im_mxic_off_pclk( unit );

			// Set callback for TMF interrupt.
			param->pCallBack = gIM_MXIC_Memory_Access_Detect_CallBack[unit];
		}
		else {
			// Unit number is invalid.
//			Ddim_Print(("Im_MXIC_Get_Memory_Access_Detect(): unit number is invalid. unit=%d\n", unit));
		}
	}
	else {
		// Error.
		Ddim_Print(("E:Im_MXIC_Get_Memory_Access_Detect(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
Set switch of register read value.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[in]		sw						Switching of the register to be read.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		When set to ON, the read value of the register the following switches to the value of the internal circuit.<br>
					<ul><li>TREGION
						<li>TLVLW, TLVLR
						<li>TPORTMW, TPORTMR
						<li>TACPTCW, TACPTCR
						<li>TGACPTCW, TGACPTCR
						<li>TSLSW, TSLSR
						<li>TGSLSW, TGSLSR</ul>
*/
INT32	Im_MXIC_Set_Reg_Read_Switch( E_IM_MXIC_UNIT unit, UCHAR sw )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		io_mxic->TRVSW.bit.TRVSW = sw;

		im_mxic_off_pclk( unit );

		Im_MXIC_Dsb();
	}
	else {
		// Error.
		Ddim_Print(("E:Im_MXIC_Set_Reg_Read_Switch(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
Get switch of register read value.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
@param[out]		sw						Switching of the register to be read.<br>
										<ul><li>@ref D_IM_MXIC_ON
											<li>@ref D_IM_MXIC_OFF</ul>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32	Im_MXIC_Get_Reg_Read_Switch( E_IM_MXIC_UNIT unit, UCHAR *const sw )
{
	INT32						result;
	volatile struct io_jdsmxic*	io_mxic;

	result = im_mxic_get_unit_address( unit, &io_mxic );

	if ( result == D_DDIM_OK ) {

		im_mxic_on_pclk( unit );

		*sw = io_mxic->TRVSW.bit.TRVSW;

		im_mxic_off_pclk( unit );
	}
	else {
		// Error.
		Ddim_Print(("E:Im_MXIC_Get_Reg_Read_Switch(): im_mxic_get_unit_address error.\n"));
	}

	return result;
}

/**
This function set output port.<br>
@param[in]		target					Setting target master.<br>
										<ul><li>@ref D_IM_MXIC_3_W_ELA
											<li>@ref D_IM_MXIC_3_W_XCH
											<li>@ref D_IM_MXIC_3_W_FPT_0
											<li>@ref D_IM_MXIC_3_W_FPT_1
											<li>@ref D_IM_MXIC_3_R_ELA
											<li>@ref D_IM_MXIC_3_R_XCH
											<li>@ref D_IM_MXIC_3_R_FPT_0
											<li>@ref D_IM_MXIC_3_R_FPT_1
@param[in]		out_port				Setting of output port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32	Im_MXIC_Set_Output_Port( UCHAR target, E_IM_MXIC_PORT_GR output_port )
{
	UCHAR				reg_read_sw;
	UCHAR				write_master;
	UCHAR				read_master;
	UINT32				port_setting_work_lo;
	UINT32				port_setting_work_hi;
	UINT64				port_set_mask = 1;

#ifdef CO_PARAM_CHECK
	if ( output_port >= E_IM_MXIC_PORT_GR_MAX ) {
		// port parameter is illegal.
		Ddim_Assertion(("E:Im_MXIC_Set_Output_Port(): output_port=%d.\n", output_port));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_wait_command_enable( E_IM_MXIC_UNIT_3, (struct io_jdsmxic *)&IO_MXIC3 );	// Wait TCFLG=1.

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	reg_read_sw = IO_MXIC3.TRVSW.bit.TRVSW;

	if ( reg_read_sw == D_IM_MXIC_ON ) {
		IO_MXIC3.TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
	}

	if ( target <= D_IM_MXIC_READ_MASTER_FLG_MASK ) {
		// Write master.
		if ( target >= D_IM_MXIC_OUT_PORT_SEP_NUM ) {

			port_setting_work_lo	= IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_LO;
			port_setting_work_hi	= IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_HI;
			write_master			= target - D_IM_MXIC_OUT_PORT_SEP_NUM;

			if ( write_master >= 32 ) {

				port_set_mask <<= ( write_master - 32 );

				if ( output_port == E_IM_MXIC_PORT_GR_0 ) {
					port_setting_work_hi &= ~port_set_mask;
				}
				else {
					port_setting_work_hi |= port_set_mask;
				}
			}
			else {

				port_set_mask <<= write_master;

				if ( output_port == E_IM_MXIC_PORT_GR_0 ) {
					port_setting_work_lo &= ~port_set_mask;
				}
				else {
					port_setting_work_lo |= port_set_mask;
				}
			}

			IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_LO = port_setting_work_lo;
			IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_HI = port_setting_work_hi;
		}
		else {
			// target parameter is illegal.
			Ddim_Assertion(("E:Im_MXIC_Set_Output_Port(): target=%d.\n", target));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}
	else {
		// Read master.
		read_master = (target & D_IM_MXIC_READ_MASTER_FLG_MASK);

		if ( read_master >= D_IM_MXIC_OUT_PORT_SEP_NUM ) {

			port_setting_work_lo	= IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_LO;
			port_setting_work_hi	= IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_HI;
			read_master				-= D_IM_MXIC_OUT_PORT_SEP_NUM;

			if ( read_master >= 32 ) {

				port_set_mask <<= ( read_master - 32 );

				if ( output_port == E_IM_MXIC_PORT_GR_0 ) {
					port_setting_work_hi &= ~port_set_mask;
				}
				else {
					port_setting_work_hi |= port_set_mask;
				}
			}
			else {

				port_set_mask <<= read_master;

				if ( output_port == E_IM_MXIC_PORT_GR_0 ) {
					port_setting_work_lo &= ~port_set_mask;
				}
				else {
					port_setting_work_lo |= port_set_mask;
				}
			}

			IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_LO = port_setting_work_lo;
			IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_HI = port_setting_work_hi;
		}
		else {
			// target parameter is illegal.
			Ddim_Assertion(("E:Im_MXIC_Set_Output_Port(): target=%d.\n", target));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}

	// Configuration start.
	im_mxic_process_configuration( E_IM_MXIC_UNIT_3, &IO_MXIC3 );

	if ( reg_read_sw == D_IM_MXIC_ON ) {
		IO_MXIC3.TRVSW.bit.TRVSW = D_IM_MXIC_ON;
	}

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	Im_MXIC_Dsb();

	return D_DDIM_OK;
}

/**
This function get output port.<br>
@param[in]		target					Setting target master.<br>
										<ul><li>@ref D_IM_MXIC_3_W_ELA
											<li>@ref D_IM_MXIC_3_W_XCH
											<li>@ref D_IM_MXIC_3_W_FPT_0
											<li>@ref D_IM_MXIC_3_W_FPT_1
											<li>@ref D_IM_MXIC_3_R_ELA
											<li>@ref D_IM_MXIC_3_R_XCH
											<li>@ref D_IM_MXIC_3_R_FPT_0
											<li>@ref D_IM_MXIC_3_R_FPT_1
											<li>@ref D_IM_MXIC_R_FTP_IF</ul>
@param[out]		out_port				Setting of output port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32	Im_MXIC_Get_Output_Port( UCHAR target, E_IM_MXIC_PORT_GR* const output_port )
{
	UCHAR	write_master;
	UCHAR	read_master;
	UINT32	port_setting_work;
	UINT64	port_mask = 1;

#ifdef CO_PARAM_CHECK
	if ( output_port == NULL ) {
		// mon_state parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Output_Port(): output_port is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_wait_command_enable( E_IM_MXIC_UNIT_3, (struct io_jdsmxic *)&IO_MXIC3 );	// Wait TCFLG=1.

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	if ( target <= D_IM_MXIC_READ_MASTER_FLG_MASK ) {
		// Write master.
		if ( target >= D_IM_MXIC_OUT_PORT_SEP_NUM ) {

			write_master = target - D_IM_MXIC_OUT_PORT_SEP_NUM;

			if ( write_master >= 32 ) {
				port_mask <<= ( write_master - 32 );
				port_setting_work = IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_HI;
			}
			else {
				port_mask <<= write_master;
				port_setting_work = IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_LO;
			}
		} 
		else {
			// target parameter is NULL.
			Ddim_Assertion(("E:Im_MXIC_Get_Output_Port(): target=%d.\n\n", target));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}
	else {
		// Read master.
		read_master = ( target & D_IM_MXIC_READ_MASTER_FLG_MASK );

		if ( read_master >= D_IM_MXIC_OUT_PORT_SEP_NUM ) {

			read_master -= D_IM_MXIC_OUT_PORT_SEP_NUM;

			if ( read_master >= 32 ) {
				port_mask <<= ( read_master - 32 );
				port_setting_work = IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_HI;
			} 
			else {
				port_mask <<= read_master;
				port_setting_work = IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_LO;
			}
		}
		else {
			// target parameter is NULL.
			Ddim_Assertion(("E:Im_MXIC_Get_Output_Port(): target=%d.\n\n", target));
			return D_IM_MXIC_INPUT_PARAM_ERROR;
		}
	}

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	port_setting_work &= port_mask;

	if ( port_setting_work == 0 ) {
		*output_port = E_IM_MXIC_PORT_GR_0;
	}
	else {
		*output_port = E_IM_MXIC_PORT_GR_1;
	}

	return D_DDIM_OK;
}

/**
This function set output port.<br>
@param[in]		out_port					Setting of output port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32	Im_MXIC_Set_Output_Port_All( T_IM_MXIC_OUTPUT_PORT const* const out_port )
{
	UCHAR	reg_read_sw;
	UINT64	port_setting_w_127_64 = 0;
	UINT64	port_setting_r_127_64 = 0;

#ifdef CO_PARAM_CHECK
	if ( out_port == NULL ) {
		// mon_state parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Set_Output_Port(): out_port is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	// TPORTMW_127_64.
	port_setting_w_127_64 |= im_mxic_shift_tportmx( out_port->w_out_port.ela,   D_IM_MXIC_3_W_ELA   );		/* pgr0821 */
	port_setting_w_127_64 |= im_mxic_shift_tportmx( out_port->w_out_port.xch,   D_IM_MXIC_3_W_XCH   );		/* pgr0821 */
	port_setting_w_127_64 |= im_mxic_shift_tportmx( out_port->w_out_port.fpt_0, D_IM_MXIC_3_W_FPT_0 );		/* pgr0821 */
	port_setting_w_127_64 |= im_mxic_shift_tportmx( out_port->w_out_port.fpt_1, D_IM_MXIC_3_W_FPT_1 );		/* pgr0821 */

	// TPORTMR_127_64.
	port_setting_r_127_64 |= im_mxic_shift_tportmx( out_port->r_out_port.ela,   D_IM_MXIC_3_R_ELA   );		/* pgr0821 */
	port_setting_r_127_64 |= im_mxic_shift_tportmx( out_port->r_out_port.xch,   D_IM_MXIC_3_R_XCH   );		/* pgr0821 */
	port_setting_r_127_64 |= im_mxic_shift_tportmx( out_port->r_out_port.fpt_0, D_IM_MXIC_3_R_FPT_0 );		/* pgr0821 */
	port_setting_r_127_64 |= im_mxic_shift_tportmx( out_port->r_out_port.fpt_1, D_IM_MXIC_3_R_FPT_1 );		/* pgr0821 */

	im_mxic_wait_command_enable( E_IM_MXIC_UNIT_3, (struct io_jdsmxic *)&IO_MXIC3 );	// Wait TCFLG=1.

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	reg_read_sw = IO_MXIC3.TRVSW.bit.TRVSW;

	if ( reg_read_sw == D_IM_MXIC_ON ) {
		IO_MXIC3.TRVSW.bit.TRVSW = D_IM_MXIC_OFF;
	}

	IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_LO = (   port_setting_w_127_64 & D_IM_MXIC_MASTER_LO_MASK );
	IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_HI = ( ( port_setting_w_127_64 & D_IM_MXIC_MASTER_HI_MASK ) >> 32 );
	IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_LO = (   port_setting_r_127_64 & D_IM_MXIC_MASTER_LO_MASK );
	IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_HI = ( ( port_setting_r_127_64 & D_IM_MXIC_MASTER_HI_MASK ) >> 32 );

	// Configuration start.
	im_mxic_process_configuration( E_IM_MXIC_UNIT_3, &IO_MXIC3 );

	if ( reg_read_sw == D_IM_MXIC_ON ) {
		IO_MXIC3.TRVSW.bit.TRVSW = D_IM_MXIC_ON;
	}

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	Im_MXIC_Dsb();

	return D_DDIM_OK;
}

/**
This function get output port.<br>
@param[out]		out_port					Setting of output port.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_MXIC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32	Im_MXIC_Get_Output_Port_All( T_IM_MXIC_OUTPUT_PORT *const out_port )
{
	UINT64	port_setting_w_127_64 = 0;
	UINT64	port_setting_r_127_64 = 0;

#ifdef CO_PARAM_CHECK
	if ( out_port == NULL ) {
		// mon_state parameter is NULL.
		Ddim_Assertion(("E:Im_MXIC_Get_Output_Port(): out_port is NULL.\n"));
		return D_IM_MXIC_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_mxic_wait_command_enable( E_IM_MXIC_UNIT_3, (struct io_jdsmxic *)&IO_MXIC3 );	// Wait TCFLG=1.

	im_mxic_on_pclk( E_IM_MXIC_UNIT_3 );

	port_setting_w_127_64 = IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_HI;
	port_setting_w_127_64 = (port_setting_w_127_64 << 32) | IO_MXIC3.TPORTMW.bit.TPORTMW_127_64_LO;
	port_setting_r_127_64 = IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_HI;
	port_setting_r_127_64 = (port_setting_r_127_64 << 32) | IO_MXIC3.TPORTMR.bit.TPORTMR_127_64_LO;

	im_mxic_off_pclk( E_IM_MXIC_UNIT_3 );

	// TPORTMW_127_64.
	out_port->w_out_port.ela	= im_mxic_master_out_port_tportmx( port_setting_w_127_64, D_IM_MXIC_3_W_ELA   );
	out_port->w_out_port.xch	= im_mxic_master_out_port_tportmx( port_setting_w_127_64, D_IM_MXIC_3_W_XCH   );
	out_port->w_out_port.fpt_0	= im_mxic_master_out_port_tportmx( port_setting_w_127_64, D_IM_MXIC_3_W_FPT_0 );
	out_port->w_out_port.fpt_1	= im_mxic_master_out_port_tportmx( port_setting_w_127_64, D_IM_MXIC_3_W_FPT_1 );

	// TPORTMR_127_64.
	out_port->r_out_port.ela	= im_mxic_master_out_port_tportmx( port_setting_r_127_64, D_IM_MXIC_3_R_ELA   );
	out_port->r_out_port.xch	= im_mxic_master_out_port_tportmx( port_setting_r_127_64, D_IM_MXIC_3_R_XCH   );
	out_port->r_out_port.fpt_0	= im_mxic_master_out_port_tportmx( port_setting_r_127_64, D_IM_MXIC_3_R_FPT_0 );
	out_port->r_out_port.fpt_1	= im_mxic_master_out_port_tportmx( port_setting_r_127_64, D_IM_MXIC_3_R_FPT_1 );

	return D_DDIM_OK;
}

/**
Interruption handler of MXIC.<br>
@param[in]		unit					Target unit number.<br>
										Please refer to @ref E_IM_MXIC_UNIT for a set value. <br>
*/
VOID Im_MXIC_Int_Handler( E_IM_MXIC_UNIT unit )
{
	INT32						dec_err_ch;
	volatile struct io_jdsmxic*	io_mxic = NULL;

	(VOID)im_mxic_get_unit_address( unit, &io_mxic );

	dec_err_ch  = 0;

	im_mxic_on_pclk( unit );

	Im_MXIC_Dsb();

	// Check configuration permission flag.
	if ( ( io_mxic->TMIE.bit.TCE == 1 ) && ( io_mxic->TMIF.bit.__TCF == 1 ) ) {
		// Clear TCF interrupt flag.
		io_mxic->TMIF.word &= D_IM_MXIC_TMIF_BIT_TCF;
	}

	// Check amount limit flag of transmission.
	if ( ( io_mxic->TMIE.bit.TME == 1 ) && ( io_mxic->TMIF.bit.__TMF == 1 ) ) {
		// Access count or transfer data monitor processing ended by limt judgment.
		gIM_MXIC_Trans_Mon_Limit_End_State = D_IM_MXIC_MONITOR_OPERATION_END;

		// Callback.
		if ( gIM_MXIC_Acs_Trans_Mon_Limit_End_CallBack != NULL ) {
			((VOID (*)()) gIM_MXIC_Acs_Trans_Mon_Limit_End_CallBack)();
		}
		// Clear TMF interrupt flag.
		io_mxic->TMIF.word &= D_IM_MXIC_TMIF_BIT_TMF;
	}

	// Check decode error flag.
	if ( ( io_mxic->TMIE.bit.TDEWE == 1 ) && ( io_mxic->TMIF.bit.__TDEWF == 1 ) ) {
		// Write channel decode error.
		io_mxic->TMIF.word &= D_IM_MXIC_TMIF_BIT_TDEWF;	// Workaround of do not clear interrupt flag when set TDECLR = 1.
		dec_err_ch |= D_IM_MXIC_DEC_ERR_CH_W;
	}
	if ( ( io_mxic->TMIE.bit.TDERE == 1 ) && ( io_mxic->TMIF.bit.__TDERF == 1 ) ) {
		// Read channel decode error.
		io_mxic->TMIF.word &= D_IM_MXIC_TMIF_BIT_TDERF;	// Workaround of do not clear interrupt flag when set TDECLR = 1.
		dec_err_ch |= D_IM_MXIC_DEC_ERR_CH_R;
	}
	if ( dec_err_ch != 0 ) {
		// Callback.
		if ( gIM_MXIC_Dec_Err_Callback[unit] != NULL ) {
			((VOID (*)()) gIM_MXIC_Dec_Err_Callback[unit])( dec_err_ch );
		}
		// Clear interrupt flag and decode error information.
		// **Note** TDEWF/TDERF interrupt flag is initialized by this processing. ... TDEWF/TDERF is not initialized. why?
		io_mxic->TDECLR.bit.TDECLR = 1;
	}

	// Check detection of memory access of transmission.
	if ( ( io_mxic->TMIE.bit.TSAWE == 1 ) && ( io_mxic->TMIF.bit.__TSAWF == 1 ) ) {
		// Callback.
		if ( gIM_MXIC_Memory_Access_Detect_CallBack[unit] != NULL ) {
			((VOID (*)()) gIM_MXIC_Memory_Access_Detect_CallBack[unit])();
		}
		// Clear TSAWF interrupt flag.
		io_mxic->TMIF.word &= D_IM_MXIC_TMIF_BIT_TSAWF;
	}

	im_mxic_off_pclk( unit );

	Im_MXIC_Dsb();

	return;
}
