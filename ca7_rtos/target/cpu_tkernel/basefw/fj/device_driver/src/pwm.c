/**
 * @file	  : pwm.c
 * @brief	  : PWM Timer wrapper
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fj_peripheral.h"
#include "pwm.h"
#include "dd_top.h"
#include "dd_pwm.h"
#include "debug.h"
#include "os_user_custom.h"
#include "fj_host.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* PWM stepping motor DMA channel */
#define D_BF_PWM_DMA_CH_TOP		(4)		// Registered ch4-7 on the channel allocation list.

#define PWM_DATA_NUM_MAX		(30)
#define PWM_STEP_DATA_NUM_MIN	(10)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* PWM Management Structure */
typedef struct {
	UCHAR mode;							// PWM operation mode
	UCHAR callback_status;				// Callback status
	UCHAR step_enable;					// PWM Step enable
	PWM_CALLBACK callback_1st;			// 1st Callback Function
	PWM_CALLBACK callback_2nd;			// 2nd Callback Function
} T_BF_PWM_MNG;

/* PWM Channel Management Structure */
typedef struct {
	UCHAR status;						// Channel status
} T_BF_PWM_CH_MNG;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* PWM Management Structure */
static volatile T_BF_PWM_MNG gBF_PWM_Mng[D_DD_PWM_CH_NUM_MAX] = {{0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}, 
                                                                 {0, 0, 0, NULL, NULL}}; // = {0};

/* PWM Channel Management Structure */
static volatile T_BF_PWM_CH_MNG gBF_PWM_Ch_Mng[D_DD_PWM_CH_NUM_MAX] = {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}}; // = {0};


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Callback control function
static VOID pwm_timer_callback( INT32 ch, INT32 status )
{
	if ((gBF_PWM_Mng[ch].callback_1st != NULL) && (gBF_PWM_Mng[ch].callback_2nd != NULL)) {
		if (gBF_PWM_Mng[ch].callback_status == 1) {			// 1st Callback
			if (gBF_PWM_Mng[ch].callback_1st) {
				(*gBF_PWM_Mng[ch].callback_1st)(status);	// Call Back function for 1st
			}
			gBF_PWM_Mng[ch].callback_status++;
		}
		else if (gBF_PWM_Mng[ch].callback_status == 2) {	// 2nd Callback
			if (gBF_PWM_Mng[ch].callback_2nd) {
				(*gBF_PWM_Mng[ch].callback_2nd)(status);	// Call Back function for 2nd
			}

			if (gBF_PWM_Mng[ch].mode == FJ_PWM_MODE_CONTINUOUS) {	// continuous mode
				gBF_PWM_Mng[ch].callback_status = 1;		// continue
			}
			else {													// One-Shot mode
				gBF_PWM_Mng[ch].callback_status = 1;		// end
			}
		}
	}
	else {
		if (gBF_PWM_Mng[ch].callback_status == 1) {			// 1st Callback
			if (gBF_PWM_Mng[ch].callback_1st) {
				(*gBF_PWM_Mng[ch].callback_1st)(status);	// Call Back function for 1st
			}
			else if (gBF_PWM_Mng[ch].callback_2nd) {
				(*gBF_PWM_Mng[ch].callback_2nd)(status);	// Call Back function for 2nd
			}
			else {
				// DO NOTHING
			}
		}
		if (gBF_PWM_Mng[ch].mode == FJ_PWM_MODE_ONE_SHOT) {	// One-Shot mode
			gBF_PWM_Mng[ch].callback_status = 1;			// end
		}
	}
}

// Callback control function for step
static VOID pwm_step_callback( INT32 ch, INT32 status )
{
	if (gBF_PWM_Mng[ch].callback_1st) {
		(*gBF_PWM_Mng[ch].callback_1st)(status);		// Call Back function
	}
}

// PWM channel management function
static VOID pwm_set_ch_status( UCHAR ch , UCHAR status )
{
	// Register channel status
	gBF_PWM_Ch_Mng[ch].status = status;
}

// PWM control set common function
static FJ_ERR_CODE pwm_set_common( FJ_PWM_CTRL const* const ctrl,
								   T_DD_PWM_CTRL_CMN *const ctrl_cmn,
								   T_DD_PWM_CTRL_TIMER_ABCD *const ctrl_abcd )
{
	INT32 ret;

	// Set Control Parameter
	ctrl_cmn->drv_mode		= ctrl->mode;										/* continuous or one-shot	*/
	ctrl_cmn->in_trigger	= (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ctrl->ch;		/* trigger input factor		*/
	ctrl_cmn->polarity		= ctrl->polarity;									/* output polarity			*/
	ctrl_cmn->out_mask		= 0;												/* output level mask		*/
	ctrl_cmn->restart_trig	= ctrl->restart_enable;								/* restart enable			*/
	ctrl_cmn->edge			= 1;												/* edge enable select		*/
	ctrl_cmn->tim_e_enable	= 0;												/* Timer E disable			*/
	ctrl_cmn->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;							/* channel assignment		*/
	gBF_PWM_Mng[ctrl->ch].callback_1st	= ctrl->pcallback1;						/* 1st callback function	*/
	gBF_PWM_Mng[ctrl->ch].callback_2nd	= ctrl->pcallback2;						/* 2nd callback function	*/
	gBF_PWM_Mng[ctrl->ch].mode			= (UCHAR)ctrl->mode;					/* PWM or One-Shot			*/

	ctrl_cmn->int_enable	= 1;												/* interrupt enable			*/
	ctrl_cmn->pCallBack		= (VP_CALLBACK_PWM)pwm_timer_callback;				/* callback  function		*/

	if ((ctrl->pcallback1 != NULL) && (ctrl->pcallback2 != NULL)) {
		ctrl_cmn->trigger		= D_DD_PWM_PCNHL_IRS_CYC_DTY;					/* interrupt factor (both)	*/
	}
	else if (ctrl->pcallback1 != NULL) {
		ctrl_cmn->trigger	= D_DD_PWM_PCNHL_IRS_DUTY;							/* interrupt factor (duty)	*/
	}
	else if (ctrl->pcallback2 != NULL) {
		ctrl_cmn->trigger	= D_DD_PWM_PCNHL_IRS_CYCLE;							/* interrupt factor (cycle)	*/
	}
	else {
		ctrl_cmn->int_enable	= 0;											/* interrupt enable			*/
		ctrl_cmn->trigger		= 0;											/* interrupt factor			*/
		ctrl_cmn->pCallBack		= NULL;											/* callback function		*/
	}

	// Set PWM Control(Common Setting)
	ret = Dd_PWM_Ctrl_Common(ctrl->ch, ctrl_cmn);
	if(ret != D_DDIM_OK) {	// PWM Control NG?
		BF_Debug_Print_Warning(("pwm_set_common() - Error!! : Dd_PWM_Ctrl_Common() Error ret=0x%x\n", ret));
		Dd_PWM_Close(ctrl->ch);

		return FJ_ERR_NG;
	}

	// Set PWM Control(ABCD Timer Setting)
	ret = Dd_PWM_Ctrl_Timer_ABCD(ctrl->ch, ctrl_abcd);
	if(ret != D_DDIM_OK) {	// PWM Control NG?
		BF_Debug_Print_Warning(("pwm_set_common() - Error!! : Dd_PWM_Ctrl_Timer_ABCD() Error ret=0x%x\n", ret));
		Dd_PWM_Close(ctrl->ch);

		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}


// PWM control set common function
static FJ_ERR_CODE pwm_set_common2( FJ_PWM_CTRL_PARAM const* const ctrl,
								   T_DD_PWM_CTRL_CMN *const ctrl_cmn,
								   T_DD_PWM_CTRL_TIMER_ABCD_2ch *const ctrl_abcd_2ch )
{
	INT32 ret;
	UCHAR prev_en=0;


	// Set Control Parameter
	ctrl_cmn->drv_mode		= ctrl->mode;										/* continuous or one-shot	*/
	ctrl_cmn->in_trigger	= D_DD_PWM_GCN_TSEL_TIM0;							/* trigger input factor		*/
//	ctrl_cmn->in_trigger	= D_DD_PWM_GCN_TSEL_EM3;							/* trigger input factor		*/
	ctrl_cmn->polarity		= ctrl->polarity;									/* output polarity			*/
	ctrl_cmn->out_mask		= 0;												/* output level mask		*/
	ctrl_cmn->restart_trig	= 1;												/* restart enable			*/
	ctrl_cmn->edge			= 1;												/* edge enable select		*/
	ctrl_cmn->tim_e_enable	= 0;												/* Timer E disable			*/
	ctrl_cmn->ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;							/* channel assignment		*/
	gBF_PWM_Mng[ctrl->ch].callback_1st	= ctrl->pcallback1;						/* 1st callback function	*/
	gBF_PWM_Mng[ctrl->ch].callback_2nd	= ctrl->pcallback2;						/* 2nd callback function	*/
	gBF_PWM_Mng[ctrl->ch].mode			= (UCHAR)ctrl->mode;					/* PWM or One-Shot			*/

	ctrl_cmn->int_enable	= 1;												/* interrupt enable			*/
	ctrl_cmn->pCallBack		= (VP_CALLBACK_PWM)pwm_timer_callback;				/* callback  function		*/

	if ((ctrl->pcallback1 != NULL) && (ctrl->pcallback2 != NULL)) {
		ctrl_cmn->trigger		= D_DD_PWM_PCNHL_IRS_CYC_DTY;					/* interrupt factor (both)	*/
	}
	else if (ctrl->pcallback1 != NULL) {
		ctrl_cmn->trigger	= D_DD_PWM_PCNHL_IRS_DUTY;							/* interrupt factor (duty)	*/
	}
	else if (ctrl->pcallback2 != NULL) {
		ctrl_cmn->trigger	= D_DD_PWM_PCNHL_IRS_CYCLE;							/* interrupt factor (cycle)	*/
	}
	else {
		ctrl_cmn->int_enable	= 0;											/* interrupt enable			*/
		ctrl_cmn->trigger		= 0;											/* interrupt factor			*/
		ctrl_cmn->pCallBack		= NULL;											/* callback function		*/
	}


	if(ctrl->total_pulse_num > 1){
		prev_en = 1;
	}
	else{
		prev_en = 0;
	}

	// Set PWM Control(Common Setting)
	ret = Dd_PWM_Ctrl_Common2(ctrl->ch, ctrl_cmn, prev_en);
	if(ret != D_DDIM_OK) {	// PWM Control NG?
		BF_Debug_Print_Warning(("pwm_set_common2() - Error!! : Dd_PWM_Ctrl_Common() Error ret=0x%x\n", ret));
		Dd_PWM_Close(ctrl->ch);

		return FJ_ERR_NG;
	}

	// Set PWM Control(ABCD Timer Setting)
	ret = Dd_PWM_Ctrl_Timer_ABCD_2ch(ctrl->ch, ctrl_abcd_2ch);
	if(ret != D_DDIM_OK) {	// PWM Control NG?
		BF_Debug_Print_Warning(("pwm_set_common2() - Error!! : Dd_PWM_Ctrl_Timer_ABCD() Error ret=0x%x\n", ret));
		Dd_PWM_Close(ctrl->ch);

		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

void pwm_cal_by_hz(ULONG hz, UCHAR* const clk, USHORT* const cycle)
{
	// clock	CKS	2^	div_wk
	// 1		0	0	1
	// 4		1	2	2
	// 16		2	4	3
	// 64		3	6	4
	UINT32 find_flag;
	ULONG peri_clock;
	ULONG total_count;
	ULONG total_count_wk;
	ULONG div_tmp;
	UCHAR div, div_wk;
	ULONG count_base;

	peri_clock = Dd_Top_Get_RCLK();

	total_count = 0;
	div_wk = 0;
	find_flag = 0;
	for (div_wk = 4; div_wk > 0; div_wk--) {
		div_tmp = 1 << (UCHAR)((div_wk - 1) * 2);			 // clock
		count_base = peri_clock / div_tmp;
		
		if((count_base % hz) == 0){
			total_count = count_base / hz;
			div = div_wk - 1;
			find_flag = 1;
			break;
		}else{
			total_count_wk = count_base / hz;
			if((total_count_wk > total_count) && (total_count_wk <= 0xFFFF)){
				total_count = total_count_wk;
				div = div_wk - 1;
				find_flag = 1;
			}
		}
	}

	if(find_flag == 0){
		total_count = peri_clock / hz;
		div = 0;
	}

	*clk = div;
	*cycle	= (USHORT)total_count;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		Open PWM timer
 *	@param		i:ch					Channel number
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_open( FJ_PERI_CH ch )
{
	INT32 ret;

	if( ch >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_open() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Open PWM
	ret = Dd_PWM_Open(ch, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ) {	// PWM Open NG?
		BF_Debug_Print_Warning(("fj_pwm_open() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}

	pwm_set_ch_status( ch, 1 );

	gBF_PWM_Mng[ch].step_enable = 0;

	return FJ_ERR_OK;
}

/**
 *	@brief		Open PWM timer step
 *	@param		i:ch					Channel number
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_open_step( FJ_PERI_CH ch )
{
	INT32 ret;
	UCHAR ch_step;
	UCHAR ch_last;

	if( ( ch >= D_DD_PWM_CH_NUM_MAX ) || ( ch % D_DD_PWM_CH_NUM_IN_BLOCK ) ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_open_step() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Open PWM
	ret = Dd_PWM_Open_Timer_E(ch, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ) {	// PWM Open NG?
		BF_Debug_Print_Warning(("fj_pwm_open_step() - Error! : ch = %d, ret = %d\n", ch, ret));
		return FJ_ERR_NG;
	}

	ch_last = ch + D_DD_PWM_CH_NUM_IN_BLOCK;

	for( ch_step = ch; ch_step < ch_last; ch_step++ ){
		pwm_set_ch_status( ch_step, 1 );
	}

	gBF_PWM_Mng[ch].step_enable = 1;

	return FJ_ERR_OK;
}


/**
 *	@brief		Open PWM timer step 2ch
 *	@param		i:ch_1st				Channel number
 *	@param		i:ch_2nd				Channel number
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_open_step_2ch( FJ_PERI_CH ch_1st, FJ_PERI_CH ch_2nd )
{
	INT32 ret;
	UCHAR ch_step;
	UCHAR ch_last;

	if( ( ch_1st >= D_DD_PWM_CH_NUM_MAX ) || ( ch_1st % D_DD_PWM_CH_NUM_IN_BLOCK ) ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_open_step() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( ( ch_2nd >= D_DD_PWM_CH_NUM_MAX ) || ( ch_2nd % D_DD_PWM_CH_NUM_IN_BLOCK ) ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_open_step() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Open PWM
	ret = Dd_PWM_Open_Timer_E(ch_1st, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ) {	// PWM Open NG?
		BF_Debug_Print_Warning(("fj_pwm_open_step() - Error! : ch = %d, ret = %d\n", ch_1st, ret));
		return FJ_ERR_NG;
	}
	// Open PWM
	ret = Dd_PWM_Open_Timer_E(ch_2nd, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ) {	// PWM Open NG?
		BF_Debug_Print_Warning(("fj_pwm_open_step() - Error! : ch = %d, ret = %d\n", ch_2nd, ret));
		return FJ_ERR_NG;
	}

	ch_last = ch_1st + D_DD_PWM_CH_NUM_IN_BLOCK;
	for( ch_step = ch_1st; ch_step < ch_last; ch_step++ ){
		pwm_set_ch_status( ch_step, 1 );
	}
	gBF_PWM_Mng[ch_1st].step_enable = 1;


	ch_last = ch_2nd + D_DD_PWM_CH_NUM_IN_BLOCK;
	for( ch_step = ch_2nd; ch_step < ch_last; ch_step++ ){
		pwm_set_ch_status( ch_step, 1 );
	}
	gBF_PWM_Mng[ch_2nd].step_enable = 1;


	return FJ_ERR_OK;
}




/**
 *	@brief		Calculate conunter clock
 *	@param		i:usec					pulse[usec]
 *	@param		i/o:clk_sel				clk_sel
 *	@param		i/o:cycle				cycle
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_calculate_clock_sel(ULONG usec, UCHAR* const clk_sel, USHORT* const cycle)
{
	return Dd_PWM_Calculate(usec, clk_sel, cycle);
}


/**
 *	@brief		Calculate conunter clock
 *	@param		i:usec_array			target pulse period[usec]
 *	@param		i/o:clk_sel				clk_sel
 *	@param		i/o:step_data			resister value of PWM
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_calculate_array_clock_sel(ULONG usec_array[][2], UCHAR* const clk_sel, FJ_PWM_STEP_DATA step_data[], UCHAR step_data_num)
{

	UCHAR	cnt = 0;
	UCHAR	array_num = 0;

	USHORT	cycle = 0;
	FJ_ERR_CODE ret = FJ_ERR_OK;
	ULONG	tmp_min_usec = 0;
	ULONG	tmp_max_usec = 0;

	cnt = 0;
	array_num = 0;
	tmp_max_usec = usec_array[0][0]; // maxmum refer by cycle
	tmp_min_usec = usec_array[0][1]; // minimum refer by duty
	do{
		// cycle_usec < duty_usec
		if(usec_array[cnt][0] < usec_array[cnt][1]){
			return FJ_ERR_NG_INPUT_PARAM;
		}

		// cycle_usec > 0 and duty_usec > 0
		if((usec_array[cnt][0] > 0) && (usec_array[cnt][1] > 0)){
			array_num++;

			// search a cycle
//			printf("usec_array[%d][0] = %ld \n", cnt, usec_array[cnt][0]);
			if(tmp_max_usec < usec_array[cnt][0]){
				tmp_max_usec = usec_array[cnt][0];
			}

			// search a duty
//			printf("usec_array[%d][1] = %ld \n", cnt, usec_array[cnt][1]);
			if(tmp_min_usec > usec_array[cnt][1]){
				tmp_min_usec = usec_array[cnt][1];
			}
		}else{
			return FJ_ERR_NG_INPUT_PARAM;
		}
		
		cnt++;
	}while(cnt < step_data_num);

	if(tmp_max_usec <= 4000){
		ret = Dd_PWM_Calculate(tmp_max_usec, clk_sel, &cycle);
		if( ret != D_DDIM_OK ) {	// PWM Open NG?
			BF_Debug_Print_Warning(("Dd_PWM_Calculate() - Error! : tmp_min_usec = %ld, *clk_sel = %d ret = %d \n", tmp_min_usec, *clk_sel, ret));
			return FJ_ERR_NG;
		}
	}else{
		pwm_cal_by_hz((1000000 / tmp_max_usec), clk_sel, &cycle);
	}
	BF_Debug_Print_Warning(("*clk_sel = %d\n", *clk_sel));
	
	ret = fj_pwm_calculate_cycle(usec_array, *clk_sel, &step_data[0], array_num);
	if( ret != D_DDIM_OK ) {	// PWM Open NG?
		BF_Debug_Print_Warning(("fj_pwm_calculate_cycle() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}

	return ret;
}



/**
 *	@brief		Set Buffer Data window
 *	@param		i:ch					pwm channel
 *	@param		i:usec_array			target pulse period[usec]
 *	@param		i:step_data				resister value of PWM
 *	@param		i:data_num				the number of data
 *	@param		i:pulse_end_flag		0:continue to output pwm, 1: the end of pwm pulse
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_set_buffer_data_window(FJ_PERI_CH ch, ULONG usec_array[][2], FJ_PWM_STEP_DATA step_data[], UCHAR data_num, UCHAR pulse_end_flag)
{

	static UCHAR	cnt = 0;
	UCHAR			clk_sel_current = 0;

	FJ_ERR_CODE ret = FJ_ERR_OK;
	UINT32			dma_ch;

	if(data_num < PWM_STEP_DATA_NUM_MIN){
		BF_Debug_Print_Warning(("fj_pwm_set_buffer_data_window() - step_data_num = %d < %d Error!\n", data_num, PWM_STEP_DATA_NUM_MIN ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

// check duty usec value
	cnt = 0;
	do{
		// cycle_usec < duty_usec
		if(usec_array[cnt][0] < usec_array[cnt][1]){
			return FJ_ERR_NG_INPUT_PARAM;
		}
	cnt++;
	}while(cnt < data_num);


	ret = Dd_PWM_Get_Clock_Sel_TimerE(ch, &clk_sel_current);
	if( ret != D_DDIM_OK ) {	// 
		BF_Debug_Print_Warning(("Dd_PWM_Get_Clock_Sel_TimerE() - Error! : ch = %d, clk_sel_current = %d ret = %d \n", ch, clk_sel_current, ret));
		return FJ_ERR_NG;
	}

	ret = fj_pwm_calculate_cycle(usec_array, clk_sel_current, &step_data[0], data_num);
	if( ret != D_DDIM_OK ) {	// 
		BF_Debug_Print_Warning(("fj_pwm_calculate_cycle() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}

	for(cnt = 0; cnt < data_num; cnt++){
		step_data[cnt].pulse_link.bit.pulse_num = 1;
		step_data[cnt].pulse_link.bit.link = 1;
	}
	// overwrite
	if(pulse_end_flag==1){
		printf("pulse_end_flag = %d \n", pulse_end_flag);
		step_data[data_num-1].pulse_link.bit.pulse_num = 1;
		step_data[data_num-1].pulse_link.bit.link = 0;
	}


	dma_ch = (ch >> 2) + D_BF_PWM_DMA_CH_TOP;
	ret = Dd_PWM_Write_Step_Only_Buf_Data_Window(ch, dma_ch, (T_DD_PWM_TIMER_E_STEPS_DATA*)&step_data[0], data_num);
	if( ret != D_DDIM_OK ) {	// 
		BF_Debug_Print_Warning(("Dd_PWM_Write_Step_Only_Buf_Data_Window() - Error! : ch = %d dma_ch = %d array_num = %d ret = 0x%x \n", ch, dma_ch, data_num, ret));
		return FJ_ERR_NG;
	}

	return ret;
}


/**
 *	@brief		Set Buffer Data Window by step_data
 *	@param		i:ch					pwm channel
 *	@param		i:step_data				resister value of PWM
 *	@param		i:ch					the number of data
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_set_buffer_data_window_by_step_data(FJ_PERI_CH ch, FJ_PWM_STEP_DATA step_data[], UCHAR data_num)
{

	FJ_ERR_CODE ret = FJ_ERR_OK;
	UINT32		dma_ch;

	dma_ch = (ch >> 2) + D_BF_PWM_DMA_CH_TOP;
	ret = Dd_PWM_Write_Step_Only_Buf_Data_Window(ch, dma_ch, (T_DD_PWM_TIMER_E_STEPS_DATA*)&step_data[0], data_num);
	if( ret != D_DDIM_OK ) {	// 
		BF_Debug_Print_Warning(("fj_pwm_set_buffer_data_window_by_step_data() - Error! : ch = %d dma_ch = %d data_num = %d ret = 0x%x \n", ch, dma_ch, data_num, ret));
		return FJ_ERR_NG;
	}

	return ret;
}


/**
 *	@brief		Calculate cycle value
 *	@param		i:usec					target pulse period[usec]
 *	@param		i/o:clk_sel				clk_sel
 *	@param		i/o:step_data			resister value of PWM
 *	@param		i:cycle_num				cycle_num : 1ï½?
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_calculate_cycle(ULONG usec[][2], UCHAR const clk_sel, FJ_PWM_STEP_DATA step_data[], UCHAR cycle_num)
{
	INT32 ret = FJ_ERR_OK;
	UCHAR cnt = 0;
	UCHAR cnt2 = 0;
	USHORT cycle_;


	if( cycle_num > D_DD_PWM_TIMERE_DATA_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_calculate_cycle() - cycle_num Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	for(cnt=0; cnt < cycle_num; cnt++){	
		for(cnt2=0; cnt2 < 2; cnt2++){	
			ret |= Dd_PWM_Calculate_Cycle(usec[cnt][cnt2], clk_sel, &cycle_);
			switch(cnt2){
				case 0:
					step_data[cnt].cyc_num = (cycle_ | 0x3);	// bit0&bit1 will be fixed as 1
					break;
				case 1:
					if(usec[cnt][0] == usec[cnt][1]){
						step_data[cnt].duty_num = (cycle_ | 0x3);
					}
					else{
						step_data[cnt].duty_num = cycle_;
					}
					break;
				default:
					BF_Debug_Print_Warning(("fj_pwm_calculate_cycle() - Error! : %d %d \n", cnt, cnt2));
					break;
			}
		}
	}

	return ret;
}


/**
 *	@brief		Set PWM control by cycle and duty ratio.
 *	@param		i:ctrl					PWM control data for ratio
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_set_by_ratio( FJ_PWM_CTRL const* const ctrl, UINT32 cycle_hz, UINT32 duty_ratio )
{
	INT32 ret;
	T_DD_PWM_CTRL_CMN ctrl_cmn;
	T_DD_PWM_CTRL_TIMER_ABCD ctrl_abcd;
	UCHAR clk;

	if( ctrl->ch >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_set_by_ratio() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if (gBF_PWM_Ch_Mng[ctrl->ch].status == 0){	// Channel not open?
		BF_Debug_Print_Warning(("fj_pwm_set_by_ratio() -  Error!! : Channel not open\n"));
		return FJ_ERR_NG;
	}

	if(cycle_hz <= 1000000){
		ctrl_abcd.cyc_time = 1000 * 1000 / cycle_hz;								/* wave cycle(usec)	*/
		ret = Dd_PWM_Calculate( ctrl_abcd.cyc_time, &clk, &ctrl_abcd.cyc_num);		/* number of cycle	*/
		if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){	// PWM Calculate NG?
			BF_Debug_Print_Warning(("fj_pwm_set_by_ratio() -  Error!! : Dd_PWM_Calculate() Error ret=0x%x\n", ret));
			return FJ_ERR_NG;
		}
	}else{
		ctrl_abcd.cyc_time = 20 * 1000 * 1000 / cycle_hz;							/* wave cycle(0.2usec)	*/
		// search sutiable cyc_num
		pwm_cal_by_hz( cycle_hz, &clk, &ctrl_abcd.cyc_num);
	}

	ctrl_cmn.clk_sel = clk;														/* clock select		*/
	ctrl_abcd.duty_num = ctrl_abcd.cyc_num * duty_ratio / 100;

	// Set common data
	ret = pwm_set_common( ctrl, &ctrl_cmn, &ctrl_abcd );

	return (FJ_ERR_CODE)ret;
}

/**
 *	@brief		Set PWM control by time. (usec order)
 *	@param		i:ctrl					PWM control data for time
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_set_by_time( FJ_PWM_CTRL const* const ctrl, ULONG time1, ULONG time2  )
{
	INT32 ret;
	T_DD_PWM_CTRL_CMN ctrl_cmn;
	T_DD_PWM_CTRL_TIMER_ABCD ctrl_abcd;
	UCHAR clk;

	if( ctrl->ch >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_set_by_time() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if (gBF_PWM_Ch_Mng[ctrl->ch].status == 0){	// Channel not open?
		BF_Debug_Print_Warning(("fj_pwm_set_by_time() -  Error!! : Channel not open\n"));
		return FJ_ERR_NG;
	}

	ctrl_abcd.cyc_time = time1 + time2;											/* wave cycle(usec)	*/
	ret = Dd_PWM_Calculate( ctrl_abcd.cyc_time, &clk, &ctrl_abcd.cyc_num);		/* number of cycle	*/
	if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){	// PWM Calculate NG?
		BF_Debug_Print_Warning(("fj_pwm_set_by_time() -  Error!! : Dd_PWM_Calculate() Error ret=0x%x\n", ret));
		return FJ_ERR_NG;
	}

	ctrl_cmn.clk_sel = clk;														/* clock select		*/
	ctrl_abcd.duty_num = ctrl_abcd.cyc_num * time2 / ctrl_abcd.cyc_time;		/* number of duty	*/

	// Set common data
	ret = pwm_set_common( ctrl, &ctrl_cmn, &ctrl_abcd );

	return (FJ_ERR_CODE)ret;
}


/**
 *	@brief		Set PWM control by time. (usec order)
 *	@param		i:ctrl_a				PWM control data for time
 *	@param		i:ctrl_b				PWM control data for time
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_set_2ch( FJ_PWM_CTRL_PARAM const* const ctrl_a, FJ_PWM_CTRL_PARAM const* const ctrl_b )
{
	INT32 ret;
	T_DD_PWM_CTRL_CMN ctrl_cmn;
	T_DD_PWM_CTRL_TIMER_ABCD_2ch ctrl_abcd_2ch;
	UCHAR clk;
	UCHAR cnt=0;
	UCHAR loop_cnt=0;
	
// ch_a
	if( ctrl_a->ch >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_set_2ch() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if (gBF_PWM_Ch_Mng[ctrl_a->ch].status == 0){	// Channel not open?
		BF_Debug_Print_Warning(("fj_pwm_set_2ch() -  Error!! : Channel not open\n"));
		return FJ_ERR_NG;
	}

	if(ctrl_a->total_pulse_num == 1){
		loop_cnt = 1;
	}
	else if(ctrl_a->total_pulse_num > 1){
		loop_cnt = 2;
	}
	else{
		BF_Debug_Print_Warning(("fj_pwm_set_2ch() -  Error!! : Channel not open\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	memset(&ctrl_cmn, 0, sizeof(T_DD_PWM_CTRL_CMN));
	memset(&ctrl_abcd_2ch, 0, sizeof(T_DD_PWM_CTRL_TIMER_ABCD_2ch));

		

	#if 0
	for(cnt=0; cnt<loop_cnt; cnt++){
		ctrl_abcd_2ch.cyc_time[cnt] = (ctrl_a->step_data[cnt].cycle_usec);			/* wave cycle(usec)	*/
		ret = Dd_PWM_Calculate( ctrl_abcd_2ch.cyc_time[cnt], &clk, &ctrl_abcd_2ch.cyc_num[cnt]);			/* number of cycle	*/
		if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){	// PWM Calculate NG?
			BF_Debug_Print_Warning(("fj_pwm_set_2ch() -  Error!! : Dd_PWM_Calculate() Error ret=0x%x\n", ret));
			return FJ_ERR_NG;
		}

		ctrl_cmn.clk_sel = clk;														/* clock select		*/
		ctrl_abcd_2ch.duty_num[cnt] = ctrl_abcd_2ch.cyc_num[cnt] * ctrl_a->step_data[cnt].duty_usec / ctrl_abcd_2ch.cyc_time[cnt];		/* number of duty	*/

		printf("cyc_time[%d] = %d \n", cnt, ctrl_abcd_2ch.cyc_time[cnt]);
		printf("clk_sel = %d \n", ctrl_cmn.clk_sel);
		printf("duty_num[%d] = %d \n\n", cnt, ctrl_abcd_2ch.duty_num[cnt]);
	}
	#else
	clk = ctrl_a->clk;
	for(cnt=0; cnt<loop_cnt; cnt++){
		ctrl_abcd_2ch.cyc_time[cnt] = (ctrl_a->step_data[cnt].cycle_usec);			/* wave cycle(usec)	*/
		ret = Dd_PWM_Calculate_Cycle( ctrl_abcd_2ch.cyc_time[cnt], clk, &ctrl_abcd_2ch.cyc_num[cnt]);			/* number of cycle	*/
		if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){	// PWM Calculate NG?
			BF_Debug_Print_Warning(("fj_pwm_set_2ch() -  Error!! : Dd_PWM_Calculate() Error ret=0x%x\n", ret));
			return FJ_ERR_NG;
		}

		ctrl_cmn.clk_sel = clk;														/* clock select		*/
		ctrl_abcd_2ch.duty_num[cnt] = ctrl_abcd_2ch.cyc_num[cnt] * ctrl_a->step_data[cnt].duty_usec / ctrl_abcd_2ch.cyc_time[cnt];		/* number of duty	*/

		#if 0
		printf("cyc_time[%d] = %d \n", cnt, ctrl_abcd_2ch.cyc_time[cnt]);
		printf("clk_sel = %d \n", ctrl_cmn.clk_sel);
		printf("duty_num[%d] = %d \n\n", cnt, ctrl_abcd_2ch.duty_num[cnt]);
		#endif
	}
	#endif

	// Set common data
	ret = pwm_set_common2( ctrl_a, &ctrl_cmn, &ctrl_abcd_2ch );


// ch_b
	if( ctrl_b->ch >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_set_2ch() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if (gBF_PWM_Ch_Mng[ctrl_b->ch].status == 0){	// Channel not open?
		BF_Debug_Print_Warning(("fj_pwm_set_2ch() -  Error!! : Channel not open\n"));
		return FJ_ERR_NG;
	}

	if(ctrl_b->total_pulse_num == 1){
		loop_cnt = 1;
	}
	else if(ctrl_b->total_pulse_num > 1){
		loop_cnt = 2;
	}
	else{
		BF_Debug_Print_Warning(("fj_pwm_set_2ch() -  Error!! : Channel not open\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}


	memset(&ctrl_cmn, 0, sizeof(T_DD_PWM_CTRL_CMN));
	memset(&ctrl_abcd_2ch, 0, sizeof(T_DD_PWM_CTRL_TIMER_ABCD_2ch));

	#if 0
	for(cnt=0; cnt<loop_cnt; cnt++){
		ctrl_abcd_2ch.cyc_time[cnt] = (ctrl_b->step_data[cnt].cycle_usec);			/* wave cycle(usec)	*/
		ret = Dd_PWM_Calculate( ctrl_abcd_2ch.cyc_time[cnt], &clk, &ctrl_abcd_2ch.cyc_num[cnt]);			/* number of cycle	*/
		if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){	// PWM Calculate NG?
			BF_Debug_Print_Warning(("fj_pwm_set_2ch() -  Error!! : Dd_PWM_Calculate() Error ret=0x%x\n", ret));
			return FJ_ERR_NG;
		}

		ctrl_cmn.clk_sel = clk;														/* clock select		*/
		ctrl_abcd_2ch.duty_num[cnt] = ctrl_abcd_2ch.cyc_num[cnt] * ctrl_b->step_data[cnt].duty_usec / ctrl_abcd_2ch.cyc_time[cnt];		/* number of duty	*/

		printf("cyc_time[%d] = %d \n", cnt, ctrl_abcd_2ch.cyc_time[cnt]);
		printf("clk_sel = %d \n", ctrl_cmn.clk_sel);
		printf("duty_num[%d] = %d \n\n", cnt, ctrl_abcd_2ch.duty_num[cnt]);
	}
	#else
	clk = ctrl_b->clk;
	for(cnt=0; cnt<loop_cnt; cnt++){
		ctrl_abcd_2ch.cyc_time[cnt] = (ctrl_b->step_data[cnt].cycle_usec);			/* wave cycle(usec)	*/
		ret = Dd_PWM_Calculate_Cycle( ctrl_abcd_2ch.cyc_time[cnt], clk, &ctrl_abcd_2ch.cyc_num[cnt]);			/* number of cycle	*/
		if ((ret != D_DDIM_OK) && (ret != D_DD_PWM_OVERFLOW_OK)){	// PWM Calculate NG?
			BF_Debug_Print_Warning(("fj_pwm_set_2ch() -  Error!! : Dd_PWM_Calculate() Error ret=0x%x\n", ret));
			return FJ_ERR_NG;
		}

		ctrl_cmn.clk_sel = clk;														/* clock select		*/
		ctrl_abcd_2ch.duty_num[cnt] = ctrl_abcd_2ch.cyc_num[cnt] * ctrl_b->step_data[cnt].duty_usec / ctrl_abcd_2ch.cyc_time[cnt];		/* number of duty	*/

		#if 0
		printf("cyc_time[%d] = %d \n", cnt, ctrl_abcd_2ch.cyc_time[cnt]);
		printf("clk_sel = %d \n", ctrl_cmn.clk_sel);
		printf("duty_num[%d] = %d \n\n", cnt, ctrl_abcd_2ch.duty_num[cnt]);
		#endif
	}
	#endif


	// Set common data
	ret = pwm_set_common2( ctrl_b, &ctrl_cmn, &ctrl_abcd_2ch );



	return (FJ_ERR_CODE)ret;
}




/**
 *	@brief		Set PWM control by time. (usec order)
 *	@param		i:ctrl					PWM control data for time
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_set_step( FJ_PWM_STEP_CTRL const* const ctrl  )
{
	INT32 ret;
	UCHAR index;
	T_DD_PWM_STEP pwm_step;
	UINT32 dma_ch;

	if( ( ctrl->ch >= D_DD_PWM_CH_NUM_MAX ) || ( ctrl->ch % D_DD_PWM_CH_NUM_IN_BLOCK ) ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_set_step() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if (gBF_PWM_Ch_Mng[ctrl->ch].status == 0){	// Channel not open?
		BF_Debug_Print_Warning(("fj_pwm_set_step() -  Error!! : Channel not open\n"));
		return FJ_ERR_NG;
	}

	if(ctrl->step_data_num < PWM_STEP_DATA_NUM_MIN){
		BF_Debug_Print_Warning(("fj_pwm_set_step() - step_data_num = %d < %d Error!\n", ctrl->step_data_num, PWM_STEP_DATA_NUM_MIN ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set PWM Control Data
	pwm_step.clk_sel		= ctrl->clk;							// clock select
	pwm_step.polarity		= ctrl->polarity;						// output polarity
	pwm_step.restart_trig	= ctrl->restart_enable;					// restart enable
	pwm_step.ch_assign		= ctrl->ch_assign;						// channel assignment

	// Set hold and force
	for( index = 0; index < D_DD_PWM_CH_NUM_IN_BLOCK; index++ ) {
		pwm_step.hold[index]	= ctrl->hold[index];				// hold setting at stop
		pwm_step.force[index]	= ctrl->force_enable[index];		// force value
	}

	// Set data
	pwm_step.step_data_num		= ctrl->step_data_num;									// number of step data
	pwm_step.undr_run_data_num	= ctrl->undr_run_data_num;								// number of under run data
	pwm_step.step_data			= (T_DD_PWM_TIMER_E_STEPS_DATA*)ctrl->step_data;		// step data(cycle,duty,pulse,link)
	pwm_step.under_run_data		= (T_DD_PWM_TIMER_E_STEPS_DATA*)ctrl->under_run_data;	// under run data(cycle,duty,pulse,link)

	// Set callback
	pwm_step.pCallBack					= (VP_CALLBACK_PWM)pwm_step_callback;	// local
	gBF_PWM_Mng[ctrl->ch].callback_1st	= ctrl->pcallback;						// user

	// Set PWM Step
	dma_ch = (ctrl->ch >> 2) + D_BF_PWM_DMA_CH_TOP;
	ret = Dd_PWM_Write_Step( ctrl->ch, dma_ch, &pwm_step );
	if( ret != D_DDIM_OK ) {	// PWM Step Set NG?
		BF_Debug_Print_Warning(("fj_pwm_set_step() - Error!! : Dd_PWM_Write_Step() Error ret=0x%x\n", ret));
		Dd_PWM_Close_Timer_E(ctrl->ch);
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Start PWM timer
 *	@param		i:ch					Channel number
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_start( FJ_PERI_CH ch )
{
	INT32 ret;

	if( ch >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_start() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	gBF_PWM_Mng[ch].callback_status = 1;

	// Start PWM
	ret = Dd_PWM_Start(ch);
	if (ret != D_DDIM_OK) {	// PWM Start NG?
		BF_Debug_Print_Warning(("fj_pwm_start() - Error! : ret = %d\n", ret));

		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Start PWM timer
 *	@param		i:ch_a					Channel number
 *	@param		i:ch_b					Channel number
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_2ch_start( FJ_PERI_CH ch_a, FJ_PERI_CH ch_b )
{
	INT32 ret;

	if( ch_a >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_start() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( ch_b >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_start() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( ch_a == ch_b ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_start() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	gBF_PWM_Mng[ch_a].callback_status = 1;
	gBF_PWM_Mng[ch_b].callback_status = 1;

	// Start PWM
	ret = Dd_PWM_2ch_Start(ch_a, ch_b);
	if (ret != D_DDIM_OK) {	// PWM Start NG?
		BF_Debug_Print_Warning(("fj_pwm_start() - Error! : ret = %d\n", ret));

		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Start PWM timer
 *	@param		i:ch_a					Channel number
 *	@param		i:ch_b					Channel number
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_2ch_restart( FJ_PERI_CH ch_a, FJ_PERI_CH ch_b )
{
	INT32 ret;

	if( ch_a >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_start() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( ch_b >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_start() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( ch_a == ch_b ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_start() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	gBF_PWM_Mng[ch_a].callback_status = 1;
	gBF_PWM_Mng[ch_b].callback_status = 1;

	// Start PWM
	ret = Dd_PWM_2ch_Start(ch_a, ch_b);
	if (ret != D_DDIM_OK) {	// PWM Start NG?
		BF_Debug_Print_Warning(("fj_pwm_start() - Error! : ret = %d\n", ret));

		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 *	@brief		Stop PWM timer
 *	@param		i:ch					Channel number
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_stop( FJ_PERI_CH ch )
{
	INT32 ret;
	
	if( ch >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_stop() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	ret = Dd_PWM_Stop(ch);
	if (ret != D_DDIM_OK) {	// PWM Stop NG?
		BF_Debug_Print_Warning(("fj_pwm_stop() - Error! : ret = %d\n", ret));

		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Close PWM timer
 *	@param		i:ch					Channel number
  *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pwm_close( FJ_PERI_CH ch )
{
	UCHAR ch_step;
	UCHAR ch_last;
	INT32 ret;
	
	if( ch >= D_DD_PWM_CH_NUM_MAX ){	/* pgr0839 */
		BF_Debug_Print_Warning(("fj_pwm_close() - Channel Number Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	gBF_PWM_Mng[ch].callback_status = 0;

	if( gBF_PWM_Mng[ch].step_enable == 1){
		ch_last = ch + D_DD_PWM_CH_NUM_IN_BLOCK;
		for( ch_step = ch; ch_step < ch_last; ch_step++ ){
			pwm_set_ch_status( ch_step, 0 );
		}
		ret = Dd_PWM_Close_Timer_E(ch);
		gBF_PWM_Mng[ch].step_enable = 0;
		if (ret != D_DDIM_OK) {	// PWM Close NG?
			BF_Debug_Print_Warning(("fj_pwm_close() - Error! : ret = %d\n", ret));
			return FJ_ERR_NG;
		}
	}
	else{
		pwm_set_ch_status( ch, 0 );
		ret = Dd_PWM_Close(ch);
		if (ret != D_DDIM_OK) {	// PWM Close NG?
			BF_Debug_Print_Warning(("fj_pwm_close() - Error! : ret = %d\n", ret));
			return FJ_ERR_NG;
		}
	}
	return FJ_ERR_OK;
}

