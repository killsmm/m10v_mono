/**
 * @file		im_ltm_common.c
 * @brief		Local Tone Mapping process(common api)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#include "im_ltm.h"

#include <string.h>
//#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
//#endif
#include "dd_arm.h"

#include "jdsltm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_IMAGE_CLOCK)
#define CO_ACT_LTM_CLOCK
#define CO_ACT_LTMRBK_CLOCK
#endif	// CO_ACT_CLOCK
#if defined(CO_ACT_HCLOCK) || defined(CO_ACT_IMAGE_HCLOCK)
#define CO_ACT_LTM_HCLOCK
#endif	// CO_ACT_HCLOCK
#if defined(CO_ACT_PCLOCK) || defined(CO_ACT_IMAGE_PCLOCK)
#define CO_ACT_LTM_PCLOCK
#endif	// CO_ACT_PCLOCK
#if defined(CO_ACT_ICLOCK) || defined(CO_ACT_IMAGE_ICLOCK)
#define CO_ACT_LTM_ACLOCK
#endif	// CO_ACT_ICLOCK

#define Im_LTM_Dsb()	Dd_ARM_Dsb_Pou()

#define D_IM_LTM1_RBKCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())
#define D_IM_LTM1_CLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP11())
#define D_IM_LTM1_HCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP11())
#define D_IM_LTM1_PCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP11())
#define D_IM_LTM1_ACLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP11())

#define D_IM_LTM2_RBKCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())
#define D_IM_LTM2_CLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP13())
#define D_IM_LTM2_HCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP13())
#define D_IM_LTM2_PCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP13())
#define D_IM_LTM2_ACLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP13())

#define D_IM_LTM1_RBKCLK_REG_BIT	D_DD_TOP_LTMRBK1CK_BIT
#define D_IM_LTM1_CLK_REG_BIT		D_DD_TOP_LTM1CK_BIT
#define D_IM_LTM1_HCLK_REG_BIT		D_DD_TOP_LTM1AH_BIT
#define D_IM_LTM1_PCLK_REG_BIT		D_DD_TOP_LTM1AP_BIT
#define D_IM_LTM1_ACLK_REG_BIT		D_DD_TOP_LTM1AX_BIT

#define D_IM_LTM2_RBKCLK_REG_BIT	D_DD_TOP_LTMRBK2CK_BIT
#define D_IM_LTM2_CLK_REG_BIT		D_DD_TOP_LTM2CK_BIT
#define D_IM_LTM2_HCLK_REG_BIT		D_DD_TOP_LTM2AH_BIT
#define D_IM_LTM2_PCLK_REG_BIT		D_DD_TOP_LTM2AP_BIT
#define D_IM_LTM2_ACLK_REG_BIT		D_DD_TOP_LTM2AX_BIT

// PIPE count
#define D_IM_LTM_PIPE_COUNT				(2)

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define im_ltm_check_target_pipe_no_1(a)		(((a) + 1) & (D_IM_LTM_PIPE1 + 1))
#define im_ltm_check_target_pipe_no_2(a)		(((a) + 1) & (D_IM_LTM_PIPE2 + 1))

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Clock control counter
#ifdef CO_ACT_LTM_CLOCK
static volatile UCHAR gIM_LTM_Clk_Ctrl_Cnt[D_IM_LTM_PIPE_COUNT] = {0,0};
static volatile UCHAR gIM_LTM_RBKClk_Ctrl_Cnt[D_IM_LTM_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_LTM_CLOCK
#ifdef CO_ACT_LTM_HCLOCK
static volatile UCHAR gIM_LTM_Hclk_Ctrl_Cnt[D_IM_LTM_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_LTM_HCLOCK
#ifdef CO_ACT_LTM_PCLOCK
static volatile UCHAR gIM_LTM_Pclk_Ctrl_Cnt[D_IM_LTM_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_LTM_PCLOCK
#ifdef CO_ACT_LTM_ACLOCK
static volatile UCHAR gIM_LTM_Aclk_Ctrl_Cnt[D_IM_LTM_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_LTM_ACLOCK


// clock off control is disabled(for register dump on debugger)
#ifdef CO_ACT_LTM_CLOCK
static volatile UINT32 gIM_LTM_Clk_Ctrl_Disable[D_IM_LTM_PIPE_COUNT] = {0,0};
static volatile UINT32 gIM_LTM_RBKClk_Ctrl_Disable[D_IM_LTM_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_LTM_CLOCK
#ifdef CO_ACT_LTM_HCLOCK
static volatile UINT32 gIM_LTM_Hclk_Ctrl_Disable[D_IM_LTM_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_LTM_HCLOCK
#ifdef CO_ACT_LTM_PCLOCK
static volatile UINT32 gIM_LTM_Pclk_Ctrl_Disable[D_IM_LTM_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_LTM_PCLOCK
#ifdef CO_ACT_LTM_ACLOCK
static volatile UINT32 gIM_LTM_Aclk_Ctrl_Disable[D_IM_LTM_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_LTM_ACLOCK

#if defined(CO_ACT_LTM_CLOCK) || defined(CO_ACT_LTM_ACLOCK) || defined(CO_ACT_LTM_HCLOCK) || defined(CO_ACT_LTM_PCLOCK)
static ULONG gOsUsr_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));
#endif	// defined(CO_ACT_LTM_CLOCK) || defined(CO_ACT_LTM_ACLOCK) || defined(CO_ACT_LTM_HCLOCK) || defined(CO_ACT_LTM_PCLOCK)

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/* LTM PCLK change to ON
 */
INT32 Im_LTM_On_Pclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_PCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_On_Pclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_Pclk_Ctrl_Cnt[0], D_IM_LTM1_PCLK_REG_ADDR, ~D_IM_LTM1_PCLK_REG_BIT );
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_Pclk_Ctrl_Cnt[1], D_IM_LTM2_PCLK_REG_ADDR, ~D_IM_LTM2_PCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_PCLOCK

	return D_DDIM_OK;
}

/* LTM PCLK change to OFF
 */
INT32 Im_LTM_Off_Pclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_PCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Off_Pclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_LTM_Pclk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_LTM_Pclk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Pclk_Ctrl_Cnt[0], D_IM_LTM1_PCLK_REG_ADDR, D_IM_LTM1_PCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Pclk_Ctrl_Cnt[0], D_IM_LTM1_PCLK_REG_ADDR, D_IM_LTM1_PCLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_LTM_Pclk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_LTM_Pclk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Pclk_Ctrl_Cnt[1], D_IM_LTM2_PCLK_REG_ADDR, D_IM_LTM2_PCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Pclk_Ctrl_Cnt[1], D_IM_LTM2_PCLK_REG_ADDR, D_IM_LTM2_PCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_PCLOCK

	return D_DDIM_OK;
}

/* LTM PCLK change to OFF force
 */
INT32 Im_LTM_Force_Off_Pclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_PCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Force_Off_Pclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		gIM_LTM_Pclk_Ctrl_Cnt[0] = 1;
		if( gIM_LTM_Pclk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Pclk_Ctrl_Cnt[0], D_IM_LTM1_PCLK_REG_ADDR, D_IM_LTM1_PCLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		gIM_LTM_Pclk_Ctrl_Cnt[1] = 1;
		if( gIM_LTM_Pclk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Pclk_Ctrl_Cnt[1], D_IM_LTM2_PCLK_REG_ADDR, D_IM_LTM2_PCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_PCLOCK

	return D_DDIM_OK;
}

/* LTM HCLK change to ON
 */
INT32 Im_LTM_On_Hclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_HCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_On_Hclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_Hclk_Ctrl_Cnt[0], D_IM_LTM1_HCLK_REG_ADDR, ~D_IM_LTM1_HCLK_REG_BIT );
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_Hclk_Ctrl_Cnt[1], D_IM_LTM2_HCLK_REG_ADDR, ~D_IM_LTM2_HCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_HCLOCK

	return D_DDIM_OK;
}

/* LTM HCLK change to OFF
 */
INT32 Im_LTM_Off_Hclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_HCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Off_Hclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_LTM_Hclk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_LTM_Hclk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Hclk_Ctrl_Cnt[0], D_IM_LTM1_HCLK_REG_ADDR, D_IM_LTM1_HCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Hclk_Ctrl_Cnt[0], D_IM_LTM1_HCLK_REG_ADDR, D_IM_LTM1_HCLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_LTM_Hclk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_LTM_Hclk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Hclk_Ctrl_Cnt[1], D_IM_LTM2_HCLK_REG_ADDR, D_IM_LTM2_HCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Hclk_Ctrl_Cnt[1], D_IM_LTM2_HCLK_REG_ADDR, D_IM_LTM2_HCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_HCLOCK

	return D_DDIM_OK;
}

/* LTM HCLK change to OFF force
 */
INT32 Im_LTM_Force_Off_Hclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_HCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Force_Off_Hclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		gIM_LTM_Hclk_Ctrl_Cnt[0] = 1;
		if( gIM_LTM_Hclk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Hclk_Ctrl_Cnt[0], D_IM_LTM1_HCLK_REG_ADDR, D_IM_LTM1_HCLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		gIM_LTM_Hclk_Ctrl_Cnt[1] = 1;
		if( gIM_LTM_Hclk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Hclk_Ctrl_Cnt[1], D_IM_LTM2_HCLK_REG_ADDR, D_IM_LTM2_HCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_HCLOCK

	return D_DDIM_OK;
}

/* LTM ACLK change to ON
 */
INT32 Im_LTM_On_Aclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_ACLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_On_Aclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_Aclk_Ctrl_Cnt[0], D_IM_LTM1_ACLK_REG_ADDR, ~D_IM_LTM1_ACLK_REG_BIT );
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_Aclk_Ctrl_Cnt[1], D_IM_LTM2_ACLK_REG_ADDR, ~D_IM_LTM2_ACLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_ACLOCK

	return D_DDIM_OK;
}

/* LTM ACLK change to OFF
 */
INT32 Im_LTM_Off_Aclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_ACLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Off_Aclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_LTM_Aclk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_LTM_Aclk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Aclk_Ctrl_Cnt[0], D_IM_LTM1_ACLK_REG_ADDR, D_IM_LTM1_ACLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Aclk_Ctrl_Cnt[0], D_IM_LTM1_ACLK_REG_ADDR, D_IM_LTM1_ACLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_LTM_Aclk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_LTM_Aclk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Aclk_Ctrl_Cnt[1], D_IM_LTM2_ACLK_REG_ADDR, D_IM_LTM2_ACLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Aclk_Ctrl_Cnt[1], D_IM_LTM2_ACLK_REG_ADDR, D_IM_LTM2_ACLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_ACLOCK

	return D_DDIM_OK;
}

/* LTM ACLK change to OFF force
 */
INT32 Im_LTM_Force_Off_Aclk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_ACLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Force_Off_Aclk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		gIM_LTM_Aclk_Ctrl_Cnt[0] = 1;
		if( gIM_LTM_Aclk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Aclk_Ctrl_Cnt[0], D_IM_LTM1_ACLK_REG_ADDR, D_IM_LTM1_ACLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		gIM_LTM_Aclk_Ctrl_Cnt[1] = 1;
		if( gIM_LTM_Aclk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Aclk_Ctrl_Cnt[1], D_IM_LTM2_ACLK_REG_ADDR, D_IM_LTM2_ACLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_ACLOCK

	return D_DDIM_OK;
}

/* LTMCLK change to ON
 */
INT32 Im_LTM_On_Clk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_On_Clk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_Clk_Ctrl_Cnt[0], D_IM_LTM1_CLK_REG_ADDR, ~D_IM_LTM1_CLK_REG_BIT );
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_Clk_Ctrl_Cnt[1], D_IM_LTM2_CLK_REG_ADDR, ~D_IM_LTM2_CLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_CLOCK

	return D_DDIM_OK;
}

/* LTMCLK change to OFF
 */
INT32 Im_LTM_Off_Clk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Off_Clk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_LTM_Clk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_LTM_Clk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Clk_Ctrl_Cnt[0], D_IM_LTM1_CLK_REG_ADDR, D_IM_LTM1_CLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Clk_Ctrl_Cnt[0], D_IM_LTM1_CLK_REG_ADDR, D_IM_LTM1_CLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_LTM_Clk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_LTM_Clk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Clk_Ctrl_Cnt[1], D_IM_LTM2_CLK_REG_ADDR, D_IM_LTM2_CLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Clk_Ctrl_Cnt[1], D_IM_LTM2_CLK_REG_ADDR, D_IM_LTM2_CLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_CLOCK

	return D_DDIM_OK;
}

/* LTMCLK change to OFF force
 */
INT32 Im_LTM_Force_Off_Clk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Force_Off_Clk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		gIM_LTM_Clk_Ctrl_Cnt[0] = 1;
		if( gIM_LTM_Clk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Clk_Ctrl_Cnt[0], D_IM_LTM1_CLK_REG_ADDR, D_IM_LTM1_CLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		gIM_LTM_Clk_Ctrl_Cnt[1] = 1;
		if( gIM_LTM_Clk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_Clk_Ctrl_Cnt[1], D_IM_LTM2_CLK_REG_ADDR, D_IM_LTM2_CLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_CLOCK

	return D_DDIM_OK;
}

/* LTMRBKCLK change to ON
 */
INT32 Im_LTM_On_RBKClk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_On_RBKClk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_RBKClk_Ctrl_Cnt[0], D_IM_LTM1_RBKCLK_REG_ADDR, ~D_IM_LTM1_RBKCLK_REG_BIT );
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_LTM_RBKClk_Ctrl_Cnt[1], D_IM_LTM2_RBKCLK_REG_ADDR, ~D_IM_LTM2_RBKCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_CLOCK

	return D_DDIM_OK;
}

/* LTMRBKCLK change to OFF
 */
INT32 Im_LTM_Off_RBKClk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Off_RBKClk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_LTM_RBKClk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_LTM_RBKClk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_RBKClk_Ctrl_Cnt[0], D_IM_LTM1_RBKCLK_REG_ADDR, D_IM_LTM1_RBKCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_RBKClk_Ctrl_Cnt[0], D_IM_LTM1_RBKCLK_REG_ADDR, D_IM_LTM1_RBKCLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_LTM_RBKClk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_LTM_RBKClk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_RBKClk_Ctrl_Cnt[1], D_IM_LTM2_RBKCLK_REG_ADDR, D_IM_LTM2_RBKCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_RBKClk_Ctrl_Cnt[1], D_IM_LTM2_RBKCLK_REG_ADDR, D_IM_LTM2_RBKCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_CLOCK

	return D_DDIM_OK;
}

/* LTMRBKCLK change to OFF force
 */
INT32 Im_LTM_Force_Off_RBKClk( UCHAR pipe_no )
{
#ifdef CO_ACT_LTM_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Force_Off_RBKClk error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_ltm_check_target_pipe_no_1( pipe_no ) ){
		gIM_LTM_RBKClk_Ctrl_Cnt[0] = 1;
		if( gIM_LTM_RBKClk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_RBKClk_Ctrl_Cnt[0], D_IM_LTM1_RBKCLK_REG_ADDR, D_IM_LTM1_RBKCLK_REG_BIT );
		}
	}
	if( im_ltm_check_target_pipe_no_2( pipe_no ) ){
		gIM_LTM_RBKClk_Ctrl_Cnt[1] = 1;
		if( gIM_LTM_RBKClk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_LTM_RBKClk_Ctrl_Cnt[1], D_IM_LTM2_RBKCLK_REG_ADDR, D_IM_LTM2_RBKCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_LTM_CLOCK

	return D_DDIM_OK;
}

/* LTM Initialize
 */
INT32 Im_LTM_Init_All_Block( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Init_All_Block error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_RBK_Init( pipe_no );
	Im_LTM_MAP_Init( pipe_no );
	Im_LTM_LTM_Init( pipe_no );

	return D_DDIM_OK;
}

/* LTM SW reset
 */
INT32 Im_LTM_SW_Reset_All_Block( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_SW_Reset_All_Block error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_RBK_SW_Reset( pipe_no );
	Im_LTM_MAP_SW_Reset( pipe_no );
	Im_LTM_LTM_SW_Reset( pipe_no );

	return D_DDIM_OK;
}

/* Stop LTM(All block) process
 */
INT32 Im_LTM_Stop_All_Block( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Stop_All_Block error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_MAP_Stop( pipe_no );
	Im_LTM_RBK_Stop( pipe_no );
	Im_LTM_LTM_Stop( pipe_no );

	return D_DDIM_OK;
}

/* LTM Interrupt Handler
 */
VOID Im_LTM_Int_Handler( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_Int_Handler error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_LTM
	Ddim_Print(( "Im_LTM_Int_Handler begin\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM
	Ddim_Print(( "Im_LTM_Int_Handler on pclk\n" ));
#endif
	Im_LTM_On_Pclk( pipe_no );

	if( pipe_no == D_IM_LTM_PIPE1 ){
		if( IO_LTM_P1.RBK.RINT1.word & 0xFFFFFFFF ){
			Im_LTM_RBK_Int_Handler( pipe_no );
		}
		if( IO_LTM_P1.MAP.MINT1.word & 0xFFFFFFFF ){
			Im_LTM_MAP_Int_Handler( pipe_no );
		}
		if( IO_LTM_P1.LTM.LINT1.word & 0xFFFFFFFF ){
			Im_LTM_LTM_Int_Handler( pipe_no );
		}
	}
	else{
		if( IO_LTM_P2.RBK.RINT1.word & 0xFFFFFFFF ){
			Im_LTM_RBK_Int_Handler( pipe_no );
		}
		if( IO_LTM_P2.MAP.MINT1.word & 0xFFFFFFFF ){
			Im_LTM_MAP_Int_Handler( pipe_no );
		}
		if( IO_LTM_P2.LTM.LINT1.word & 0xFFFFFFFF ){
			Im_LTM_LTM_Int_Handler( pipe_no );
		}
	}

#ifdef CO_DEBUG_PRINT_IM_LTM
	Ddim_Print(( "Im_LTM_Int_Handler off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );

#ifdef CO_DEBUG_PRINT_IM_LTM
	Ddim_Print(( "Im_LTM_Int_Handler end\n" ));
#endif
}

#ifdef IM_LTM_STATUS_PRINT
VOID Im_LTM_Print_ClockStatus( VOID )
{
#ifdef CO_ACT_LTM_CLOCK
	Ddim_Print(( "LTM1CK=%u\n", IO_CHIPTOP.CLKSTOP10.bit.LTM1CK ));
	Ddim_Print(( "\tgIM_LTM_Clk_Ctrl_Cnt[0]      = %u\n", gIM_LTM_Clk_Ctrl_Cnt[0] ));
	Ddim_Print(( "\tgIM_LTM_Clk_Ctrl_Disable[0]  = %u\n", gIM_LTM_Clk_Ctrl_Disable[0] ));
	Ddim_Print(( "LTM2CK=%u\n", IO_CHIPTOP.CLKSTOP12.bit.LTM2CK ));
	Ddim_Print(( "\tgIM_LTM_Clk_Ctrl_Cnt[1]      = %u\n", gIM_LTM_Clk_Ctrl_Cnt[1] ));
	Ddim_Print(( "\tgIM_LTM_Clk_Ctrl_Disable[1]  = %u\n", gIM_LTM_Clk_Ctrl_Disable[1] ));
#endif	// CO_ACT_LTM_CLOCK
#ifdef CO_ACT_LTM_HCLOCK
	Ddim_Print(( "LTM1HCLK=%u\n", IO_CHIPTOP.CLKSTOP10.bit.LTM1AH ));
	Ddim_Print(( "\tgIM_LTM_Hclk_Ctrl_Cnt[0]     = %u\n", gIM_LTM_Hclk_Ctrl_Cnt[0] ));
	Ddim_Print(( "\tgIM_LTM_Hclk_Ctrl_Disable[0] = %u\n", gIM_LTM_Hclk_Ctrl_Disable[0] ));
	Ddim_Print(( "LTM2HCLK=%u\n", IO_CHIPTOP.CLKSTOP12.bit.LTM2AH ));
	Ddim_Print(( "\tgIM_LTM_Hclk_Ctrl_Cnt[1]     = %u\n", gIM_LTM_Hclk_Ctrl_Cnt[1] ));
	Ddim_Print(( "\tgIM_LTM_Hclk_Ctrl_Disable[1] = %u\n", gIM_LTM_Hclk_Ctrl_Disable[1] ));
#endif	// CO_ACT_LTM_HCLOCK
#ifdef CO_ACT_LTM_PCLOCK
	Ddim_Print(( "LTM1PCLK=%u\n", IO_CHIPTOP.CLKSTOP10.bit.LTM1AP ));
	Ddim_Print(( "\tgIM_LTM_Pclk_Ctrl_Cnt[0]     = %u\n", gIM_LTM_Pclk_Ctrl_Cnt[0] ));
	Ddim_Print(( "\tgIM_LTM_Pclk_Ctrl_Disable[0] = %u\n", gIM_LTM_Pclk_Ctrl_Disable[0] ));
	Ddim_Print(( "LTM2PCLK=%u\n", IO_CHIPTOP.CLKSTOP12.bit.LTM2AP ));
	Ddim_Print(( "\tgIM_LTM_Pclk_Ctrl_Cnt[1]     = %u\n", gIM_LTM_Pclk_Ctrl_Cnt[1] ));
	Ddim_Print(( "\tgIM_LTM_Pclk_Ctrl_Disable[1] = %u\n", gIM_LTM_Pclk_Ctrl_Disable[1] ));
#endif	// CO_ACT_LTM_PCLOCK
#ifdef CO_ACT_LTM_ACLOCK
	Ddim_Print(( "LTMLTM1ACLK=%u\n", IO_CHIPTOP.CLKSTOP10.bit.LTM1AX ));
	Ddim_Print(( "\tgIM_LTM_Aclk_Ctrl_Cnt[0]     = %u\n", gIM_LTM_Aclk_Ctrl_Cnt[0] ));
	Ddim_Print(( "\tgIM_LTM_Aclk_Ctrl_Disable[0] = %u\n", gIM_LTM_Aclk_Ctrl_Disable[0] ));
	Ddim_Print(( "LTM2ACLK=%u\n", IO_CHIPTOP.CLKSTOP12.bit.LTM2AX ));
	Ddim_Print(( "\tgIM_LTM_Aclk_Ctrl_Cnt[1]     = %u\n", gIM_LTM_Aclk_Ctrl_Cnt[1] ));
	Ddim_Print(( "\tgIM_LTM_Aclk_Ctrl_Disable[1] = %u\n", gIM_LTM_Aclk_Ctrl_Disable[1] ));
#endif	// CO_ACT_LTM_ACLOCK
	Ddim_Print(( "\n" ));
}

#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE
