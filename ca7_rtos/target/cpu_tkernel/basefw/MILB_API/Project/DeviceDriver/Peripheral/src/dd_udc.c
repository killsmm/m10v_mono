/**
 * @file		dd_udc.c
 * @brief		UpDown Counter driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ddim_user_custom.h"

#include "peripheral.h"
#include "dd_udc.h"
#include "dd_top.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// UDC task mode
//#define D_DD_UDC_EXC_OFF		(0)				// State of unlock
//#define D_DD_UDC_EXC_ON			(1)				// State of lock

// UDC channel count
#define D_DD_UDC_CH_COUNT		(6)

// CCRH/L: 16bit mode permission setting
#define D_DD_UDC_M16E_8BIT		(0x00)			//  8bit operational mode
#define D_DD_UDC_M16E_16BIT		(0x01)			// 16bit operational mode

// Interrupt flag bit
#define D_DD_UDC_CSR_BIT_CMPF	(0x00000010)	// CSR.CMPF
#define D_DD_UDC_CSR_BIT_OVFF	(0x00000008)	// CSR.OVFF
#define D_DD_UDC_CSR_BIT_UDFF	(0x00000004)	// CSR.UDFF
#define D_DD_UDC_CCR_BIT_CDCF	(0x00004000)	// CCR.CDCF

// Peripheral block function select UDCTRG
#define D_DD_UDC_UDCTRG_OFF		(0x00)			// UDCTRG bit OFF
#define D_DD_UDC_UDCTRG_ON		(0x01)			// UDCTRG bit ON

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// UDC clock selection
#define D_DD_UDC_CLK_SEL_2		(2)				// Ratio of dividing peripheral clock 2
#define D_DD_UDC_CLK_SEL_8		(8)				// Ratio of dividing peripheral clock 8
#define D_DD_UDC_CLK_SEL_MAX	(2)				// Maximum value of Ratio of dividing peripheral clock

// MAX value of RCR/UDCR
#define D_DD_UDC_COUNTR_MAX		(0xFFFF)		// MAX value of RCR/UDCR

//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Set data maintenance table
/* 
	rcr      : RCR register value (0 ~ 65535)
	pCallBack: Callback function pointer
*/
typedef struct {
	USHORT			rcr;
	DD_UDC_CALLBACK	pCallBack;
} T_DD_UDC_MNG_SAVE;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Set timer configuration data table
/*
	reload_count   : Number of reloading for maximum timer counter
	remain_count   : Remaining Timer Counter
	pTimer_Callback: Callback function pointer for timer function
*/
typedef struct {
	ULONG					reload_count;
	USHORT					remain_count;
	DD_UDC_TIMER_CALLBACK	pTimer_CallBack;
} T_DD_UDC_TIMER_MNG;

//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/*
	UDC status
		D_DD_UDC_EXC_OFF	: UDC is not locked. 
		D_DD_UDC_EXC_ON		: UDC is being locked.
*/
//static UCHAR gDD_UDC_State[D_DD_UDC_CH_COUNT] = {D_DD_UDC_EXC_OFF, D_DD_UDC_EXC_OFF, D_DD_UDC_EXC_OFF, D_DD_UDC_EXC_OFF};
//static UCHAR gDD_UDC_State[D_DD_UDC_CH_COUNT] = {D_DD_UDC_EXC_OFF, D_DD_UDC_EXC_OFF, D_DD_UDC_EXC_OFF, D_DD_UDC_EXC_OFF, D_DD_UDC_EXC_OFF, D_DD_UDC_EXC_OFF};

// Save parameter
static volatile T_DD_UDC_MNG_SAVE gDD_UDC_Mng[D_DD_UDC_CH_COUNT];

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Save Ratio of dividing peripheral clock
static volatile UCHAR gDD_UDC_CLK_SEL[D_DD_UDC_CLK_SEL_MAX] = { D_DD_UDC_CLK_SEL_2, D_DD_UDC_CLK_SEL_8 };

// Save Timer configuration data
static volatile T_DD_UDC_TIMER_MNG	gDD_UDC_Timer_Mng[D_DD_UDC_CH_COUNT];

//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Macro													 			*/
/*----------------------------------------------------------------------*/
// Set to the lock status.
//#define dd_udc_lock(ch)		{ gDD_UDC_State[(ch)] = D_DD_UDC_EXC_ON;  }
// Set to the unlock status.
//#define dd_udc_unlock(ch)	{ gDD_UDC_State[(ch)] = D_DD_UDC_EXC_OFF; }

//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
The CCR register value that can be safely "read modify write" is returned. 
*/
static union io_udc_ccr dd_udc_get_safe_write_ccr(UCHAR ch)
{
	union io_udc_ccr ccr;

	ccr.word = IO_PERI.UDC[ch].CCR.word;
	ccr.bit.__CDCF = 1;		// Not change CDCF

	return ccr;
}

/**
The CSR register value that can be safely "read modify write" is returned. 
*/
static union io_udc_csr dd_udc_get_safe_write_csr(UCHAR ch)
{
	union io_udc_csr csr;

	csr.word = IO_PERI.UDC[ch].CSR.word;
	csr.bit.__CMPF = 1;		// Not change CMPF
	csr.bit.__OVFF = 1;		// Not change OVFF
	csr.bit.__UDFF = 1;		// Not change UDFF

	return csr;
}

/**
RCR is written in the register, 
and the reproduction is maintained. (for RCR improper reading)
@param [in] ch	Channel number.<br>
@param [in] rcr	RCR value.<br>
*/
static VOID dd_udc_set_rcr(UCHAR ch, USHORT rcr)
{
	IO_PERI.UDC[ch].RCR.bit.D = (ULONG)rcr;
	gDD_UDC_Mng[ch].rcr = rcr;
}

/**
Get RCR value. 
@param [in] ch	Channel number.<br>
@retval			RCR value
*/
static USHORT dd_udc_get_rcr(UCHAR ch)
{
	return gDD_UDC_Mng[ch].rcr;
}

#ifdef CO_PARAM_CHECK
/**
Check channel number.
@param [in] ch	Channel number.<br>
*/
static INT32 dd_udc_chk_ch_number(UCHAR ch)
{
	// channel number range check
	if( ch >= D_DD_UDC_CH_COUNT ){
		// Error of channel number
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	return D_DDIM_OK;
}

/**
Check count mode.
@param [in] count_mode	Count mode.<br>
*/
static INT32 dd_udc_chk_count_mode(UCHAR count_mode)
{
	INT32 result;

	switch (count_mode) {
		case D_DD_UDC_CMS_TIMER:
		case D_DD_UDC_CMS_UP_DOWN:
		case D_DD_UDC_CMS_PHASE_LAG_2:
		case D_DD_UDC_CMS_PHASE_LAG_4:
			result = D_DDIM_OK;
			break;

		default:
			result = D_DD_UDC_INPUT_PARAM_ERROR;
			break;
	}
	return result;
}
#endif	// CO_PARAM_CHECK

//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
calculate parameter for timer function.
@param [in] ch		Channel number.<br>
@param [in] usec	setting time.<br>
*/
static VOID dd_udc_timer_calculate(UCHAR ch, ULONG usec, UCHAR* const clock_sel, ULLONG* const timer_count)
{
	ULLONG total_count;
	ULONG peri_clk;

	//Get peripheral clock
	peri_clk = Dd_Top_Get_RCLK();

	//Get total counter (Before clock dividing)
	total_count = (((ULLONG)peri_clk * (ULLONG)usec) / 1000000);	// usec order

	// Caluculate counter value and dividing rate
	if( ( total_count % gDD_UDC_CLK_SEL[1] ) == 0 ){		// Divide by 8
		// Get total counter with dividing
		*timer_count = total_count / gDD_UDC_CLK_SEL[1];
		*clock_sel = D_DD_UDC_CLKS_8_CLOCK;
	}
	else if( ( total_count % gDD_UDC_CLK_SEL[0] ) == 0 ){	// Divide by 2
		// Get total counter with dividing
		*timer_count = total_count / gDD_UDC_CLK_SEL[0];
		*clock_sel = D_DD_UDC_CLKS_2_CLOCK;
	}
	else{
		/* Case of Can not be divided evenly : Dividing rate 2 that the error margin is
											   fewer is adopted and the fraction is rounded down */ 
		*timer_count = total_count / gDD_UDC_CLK_SEL[0];
		*clock_sel = D_DD_UDC_CLKS_2_CLOCK;
	}

	return;
}

/**
Number of reloading was Decremented and callback at end of timer count.
@param [in] ch		Channel number.<br>
@param [in] result	Interrupt factor.<br>
*/
static VOID dd_udc_utility_callback( UCHAR ch, INT32 result )
{
	if(result == D_DD_UDC_UNDER_FLOW){
		if( gDD_UDC_Timer_Mng[ch].reload_count == 0 ){	// Case of timer count is completed
			Dd_UDC_Stop( ch );
			(*gDD_UDC_Timer_Mng[ch].pTimer_CallBack)(ch);
		}
		else{
			if( gDD_UDC_Timer_Mng[ch].reload_count >= 1 ){
				gDD_UDC_Timer_Mng[ch].reload_count--;
			}

			if( gDD_UDC_Timer_Mng[ch].reload_count == 0 ){
				Dd_UDC_Stop(ch);
				Dd_UDC_Set_UDCR_Counter(ch, gDD_UDC_Timer_Mng[ch].remain_count);
				Dd_UDC_Start(ch);
			}
			else{
				// The reload function as before reboot 0xFFFF
			}
		}
	}

	return;
}
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/**
 * @brief  The UDC control register (all channels) is initialized.
 * @param  VOID
 * @return VOID
 */
VOID Dd_UDC_Init( VOID )
{
	UCHAR ch;

	for( ch = 0; ch < D_DD_UDC_CH_COUNT; ch++ ){
		IO_PERI.UDC[ch].CCR.word = 0x00008008;	// clear of up/down counter register
												// [b15]	M16E=1	:16bit mode(Fixed value)
												// [b14]	CDCF=0	:clear(initilal value)
												// [b13]	CFIE=0	:disable(initilal value)
												// [b12]	CLKS=0	:built-in prescaler(initilal value)
												// [b11-10]	CMS =0	:timer mode(initilal value)
												// [b 9- 8]	CES =0	:detect disable(initilal value)
												// [b 7] 			:reserve
												// [b 6]	CTUT=0	:disable(invalid)
												// [b 5]	UCRE=0	:disable(initial value)
												// [b 4]	RLDE=0	:disable(initial value)
												// [b 3]	UDCC=1	:clear of UDCR register
												// [b 2]	CGSC=0	:gate(initial value)
												// [b1-0]	CGE =0	:disable(initial value)

		IO_PERI.UDC[ch].CSR.word = 0x00000000;	// clear of counter status register
												// [b7]		CSTR=0	:disable(initilal value)
												// [b6]		CITE=0	:disable(initilal value)
												// [b5]		UDIE=0	:disable(initilal value)
												// [b4]		CMPF=0	:clear(initilal value)
												// [b3]		OVFF=0	:clear(initilal value)
												// [b2]		UDFF=0	:clear(initilal value)
												// [b1-0]	UDF		:read only

		dd_udc_set_rcr(ch, 0x0000);	// clear of reload/compare data

		// Initialize for chX management info
		gDD_UDC_Mng[ch].pCallBack = NULL;		// callback function pointer
	}
	return;
}

/**
 * @brief  The UDC channel is exclusively controlled.
 * @param  UCHAR	ch
 * @param  INT32	tmout
 * @return INT32	D_DDIM_OK/D_DD_UDC_INPUT_PARAM_ERROR/D_DD_UDC_EXT_LOCK_NG/D_DD_UDC_SEM_TIMEOUT
 */
INT32 Dd_UDC_Open(UCHAR ch,INT32 tmout)
{
//	INT32	status_reg;
	INT32	ercd;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Open parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}

	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR) {
		Ddim_Assertion(("E:Dd_UDC_Open parameter error. tmout = %x\n",tmout));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	
//	status_reg = Dd_ARM_DI();
	// Lock check

	if (D_DDIM_USER_SEM_WAIT_POL == tmout){
		ercd = DDIM_User_Pol_Sem( SID_DD_UDC(ch) );				// pol_sem()
	}
	else {
		ercd = DDIM_User_Twai_Sem( SID_DD_UDC(ch), (DDIM_USER_TMO)tmout );	// twai_sem()
	}

	if (D_DDIM_USER_E_OK != ercd){
		if (D_DDIM_USER_E_TMOUT == ercd){
//			Dd_ARM_EI(status_reg);
			return D_DD_UDC_SEM_TIMEOUT;
		}
//		Dd_ARM_EI(status_reg);
		return D_DD_UDC_EXT_LOCK_NG;
	}

//	Dd_ARM_EI(status_reg);
	return D_DDIM_OK;
}

/**
 * @brief  The exclusive control of control Channel it is released.
 * @param  UCHAR cntl_ch
 * @return INT32	D_DDIM_OK/D_DD_UDC_INPUT_PARAM_ERROR/D_DD_UDC_SEM_NG
 */
INT32 Dd_UDC_Close(UCHAR ch)
{
	INT32	ercd;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Close parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// Initialize for chX management info
	gDD_UDC_Mng[ch].pCallBack = NULL;		// callback function pointer

	ercd = DDIM_User_Sig_Sem( SID_DD_UDC(ch) );				// sig_sem()
	if (D_DDIM_USER_E_OK != ercd) {
		return D_DD_UDC_SEM_NG;
	}

	return D_DDIM_OK;
}

/**
 * @brief  Set common setting to each mode of UDC.
 * @param  UCHAR ch, T_DD_UDC_CTRL_CMN *udc_crl
 * @return INT32	D_DDIM_OK/D_DD_UDC_INPUT_PARAM_ERROR/D_DD_UDC_ERROR_BUSY
 */
INT32 Dd_UDC_Ctrl_Common(UCHAR ch, T_DD_UDC_CTRL_CMN const* const udc_crl)
{
	INT32 result;
	union io_udc_ccr ccr;
	union io_udc_csr csr;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Ctrl_Common parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	// Parameter check
	if (udc_crl == NULL) {
		// A set value makes an error for NULL.
		Ddim_Assertion(("E:Dd_UDC_Ctrl_Common parameter error. udc_crl is NULL.\n"));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if((D_DD_UDC_CMS_PHASE_LAG_2 == udc_crl->count_mode) || (D_DD_UDC_CMS_PHASE_LAG_4 == udc_crl->count_mode)) {
		// M7M update
		//if((ch == 4) || (5 == ch)) {
		//	return D_DD_UDC_EXT_LOCK_NG;		// Not Support Mode
		//}
		if(Dd_Top_Get_PERSEL2_UDCTRG() == D_DD_UDC_UDCTRG_ON){
			if(ch >= 2) {
				return D_DD_UDC_EXT_LOCK_NG;		// Not Support Mode
			}
		}
	}

	gDD_UDC_Mng[ch].pCallBack    = udc_crl->pCallBack;	// callback function pointer

	ccr = dd_udc_get_safe_write_ccr(ch);

	ccr.bit.__CGE  = udc_crl->zin_edge;
	ccr.bit.__CGSC = udc_crl->zin_mode;
	ccr.bit.__RLDE = udc_crl->reload;
	ccr.bit.__UCRE = udc_crl->comp_clear;
	ccr.bit.__CMS  = udc_crl->count_mode;
	ccr.bit.__CFIE = udc_crl->cnt_dir_int;
	ccr.bit.__M16E = D_DD_UDC_M16E_16BIT;				//Reserve bit. However, it is necessary to be "1".
	IO_PERI.UDC[ch].CCR.word = ccr.word;

	csr = dd_udc_get_safe_write_csr(ch);

	csr.bit.__CITE = udc_crl->cmp_int;
	csr.bit.__UDIE = udc_crl->under_over;
	IO_PERI.UDC[ch].CSR.word = csr.word;

	result = Dd_UDC_Set_RCR_Counter(ch, udc_crl->rcr);
	if (result != D_DDIM_OK) {
		// RCR setting error.
		Ddim_Print(("E:Dd_UDC_Ctrl_Common RCR setting error. ch=%d, result=%d\n", ch, result));
		return result;
	}
	result = Dd_UDC_Set_UDCR_Counter(ch, udc_crl->udcr);
	if (result != D_DDIM_OK) {
		// UDCR setting error.
		Ddim_Print(("E:Dd_UDC_Ctrl_Common UDCR setting error. ch=%d, result=%d\n", ch, result));
		return result;
	}
	return D_DDIM_OK;
}

/**
Get common setting to each mode of UDC.<br>
@param[in]	ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
@param[out]	udc_crl	UDC Start table pointer. See @ref T_DD_UDC_CTRL_CMN
*/
INT32 Dd_UDC_Get_Ctrl_Common(UCHAR ch, T_DD_UDC_CTRL_CMN* const udc_crl)
{
#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Get_Ctrl_Common parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	if (udc_crl == NULL) {
		// Output parameter is NULL.
		Ddim_Assertion(("E:Dd_UDC_Get_Ctrl_Common parameter error. udc_crl is NULL.\n"));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	udc_crl->udcr        = Dd_UDC_Get_UDCR_Counter(ch);
	udc_crl->rcr         = dd_udc_get_rcr(ch);

	udc_crl->count_mode  = IO_PERI.UDC[ch].CCR.bit.__CMS;
	udc_crl->comp_clear  = IO_PERI.UDC[ch].CCR.bit.__UCRE;
	udc_crl->reload      = IO_PERI.UDC[ch].CCR.bit.__RLDE;
	udc_crl->zin_mode    = IO_PERI.UDC[ch].CCR.bit.__CGSC;
	udc_crl->zin_edge    = IO_PERI.UDC[ch].CCR.bit.__CGE;
	udc_crl->cmp_int     = IO_PERI.UDC[ch].CSR.bit.__CITE;
	udc_crl->under_over  = IO_PERI.UDC[ch].CSR.bit.__UDIE;
	udc_crl->cnt_dir_int = IO_PERI.UDC[ch].CCR.bit.__CFIE;
	udc_crl->pCallBack   = gDD_UDC_Mng[ch].pCallBack;

	return D_DDIM_OK;
}

/**
Set parameter only for the timer mode.
@param [in] ch			Channel number.<br>
@param [in]	prescaler	Built-in prescaler selection.<br>
*/
INT32 Dd_UDC_Ctrl_Timer(UCHAR ch, UCHAR prescaler)
{
	union io_udc_ccr ccr;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Ctrl_Timer parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	if ((prescaler != D_DD_UDC_CLKS_2_CLOCK) && (prescaler != D_DD_UDC_CLKS_8_CLOCK)) {
		// Error of prescaler selection
		Ddim_Assertion(("E:Dd_UDC_Ctrl_Timer parameter error. prescaler=%d\n", prescaler));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	ccr = dd_udc_get_safe_write_ccr(ch);
	ccr.bit.__CLKS = prescaler;
	IO_PERI.UDC[ch].CCR.word = ccr.word;

	return D_DDIM_OK;
}

/**
Get parameter only for the timer mode.
@param[in]	ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
@param[out]	prescaler	Built-in prescaler selection.<br>
						- @ref D_DD_UDC_CLKS_2_CLOCK
						- @ref D_DD_UDC_CLKS_8_CLOCK
*/
INT32 Dd_UDC_Get_Ctrl_Timer(UCHAR ch, UCHAR* const prescaler)
{
#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Get_Ctrl_Timer parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	if (prescaler == NULL) {
		// Output parameter is NULL.
		Ddim_Assertion(("E:Dd_UDC_Get_Ctrl_Timer parameter error. prescaler is NULL.\n"));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*prescaler = IO_PERI.UDC[ch].CCR.bit.__CLKS;

	return D_DDIM_OK;
}

/**
Set parameter only for the up/down count mode.
@param [in] ch		Channel number.<br>
@param [in]	edge	Count clock edge.<br>
*/
INT32 Dd_UDC_Ctrl_Up_Down(UCHAR ch, UCHAR edge)
{
	union io_udc_ccr ccr;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Ctrl_Up_Down parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	switch (edge) {
		case D_DD_UDC_CES_NOT:
		case D_DD_UDC_CES_RISE:
		case D_DD_UDC_CES_FALL:
		case D_DD_UDC_CES_BOTH:
			// DO NOTHING
			break;
		default:
			// Error of edge selection
			Ddim_Assertion(("E:Dd_UDC_Ctrl_Up_Down parameter error. edge=%d\n", edge));
			return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	ccr = dd_udc_get_safe_write_ccr(ch);
	ccr.bit.__CES = edge;
	IO_PERI.UDC[ch].CCR.word = ccr.word;

	return D_DDIM_OK;
}

/**
Get parameter only for the up/down count mode.
@param[in]	ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
@param[out]	edge	Count clock edge.<br>
					Please specify either the following.<br>
					- @ref D_DD_UDC_CES_NOT
					- @ref D_DD_UDC_CES_RISE
					- @ref D_DD_UDC_CES_FALL
					- @ref D_DD_UDC_CES_BOTH
*/
INT32 Dd_UDC_Get_Ctrl_Up_Down(UCHAR ch, UCHAR* const edge)
{
#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Get_Ctrl_Up_Down parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	if (edge == NULL) {
		// Output parameter is NULL.
		Ddim_Assertion(("E:Dd_UDC_Ctrl_Common parameter error. edge is NULL.\n"));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*edge = IO_PERI.UDC[ch].CCR.bit.__CES;

	return D_DDIM_OK;
}

/**
The UDC of a control channel is started.<br>
@param [in] ch	Channel number.<br>
*/
INT32 Dd_UDC_Start(UCHAR ch)
{
	union io_udc_ccr ccr;
	union io_udc_csr csr;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Start parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	ccr = dd_udc_get_safe_write_ccr(ch);
	ccr.bit.__CDCF = 0;	// Clear CDCF
	IO_PERI.UDC[ch].CCR.word = ccr.word;

	csr = dd_udc_get_safe_write_csr(ch);
	csr.bit.__CMPF = 0;	// Clear CMPF
	csr.bit.__OVFF = 0;	// Clear OVFF
	csr.bit.__UDFF = 0;	// Clear UDFF
	IO_PERI.UDC[ch].CSR.word = csr.word;

	csr = dd_udc_get_safe_write_csr(ch);
	csr.bit.__CSTR = 1;	// Start
	IO_PERI.UDC[ch].CSR.word = csr.word;
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
The UDC of a control channel is stopped.<br>
@param [in] ch	Channel number.<br>
*/
INT32 Dd_UDC_Stop(UCHAR ch)
{
	union io_udc_csr csr;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Stop parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// UDC stop
	csr = dd_udc_get_safe_write_csr(ch);
	csr.bit.__CSTR = 0;
	IO_PERI.UDC[ch].CSR.word = csr.word;
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
The interruption flag is cleared, and the CallBack function for UpDown Counter called.
@param [in] ch	Channel number.<br>
*/
VOID Dd_UDC_Int_Handler(UCHAR ch)
{
	INT32	result;
	ULONG	reg;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Int_Handler parameter error. ch=%d\n", ch));
		return;
	}
#endif	// CO_PARAM_CHECK

	if( (IO_PERI.UDC[ch].CSR.bit.__CITE == 1) && (IO_PERI.UDC[ch].CSR.bit.__CMPF == 1) ){
		// The comparison agreement was detected.
		if( gDD_UDC_Mng[ch].pCallBack != NULL ){
			// Call callback function
			result = D_DD_UDC_CMPF_OK;
			(*gDD_UDC_Mng[ch].pCallBack)(ch, result);
		}
		// Clear interrupt flag
		reg = IO_PERI.UDC[ch].CSR.word;
		reg &= ~D_DD_UDC_CSR_BIT_CMPF;							// Clear CMPF
		reg |= (D_DD_UDC_CSR_BIT_OVFF | D_DD_UDC_CSR_BIT_UDFF);	// Not change OVFF, UDFF
		IO_PERI.UDC[ch].CSR.word = reg;
		Dd_ARM_Dsb_Pou();
	}

	if( (IO_PERI.UDC[ch].CSR.bit.__UDIE == 1) && (IO_PERI.UDC[ch].CSR.bit.__OVFF == 1) ){
		// The overflow was detected.
		if( gDD_UDC_Mng[ch].pCallBack != NULL ){
			// Call callback function
			result = D_DD_UDC_OVER_FLOW;
			(*gDD_UDC_Mng[ch].pCallBack)(ch, result);
		}
		// Clear interrupt flag
		reg = IO_PERI.UDC[ch].CSR.word;
		reg &= ~D_DD_UDC_CSR_BIT_OVFF;							// Clear OVFF
		reg |= (D_DD_UDC_CSR_BIT_CMPF | D_DD_UDC_CSR_BIT_UDFF);	// Not change CMPF, UDFF
		IO_PERI.UDC[ch].CSR.word = reg;
		Dd_ARM_Dsb_Pou();
	}

	if( (IO_PERI.UDC[ch].CSR.bit.__UDIE == 1) && (IO_PERI.UDC[ch].CSR.bit.__UDFF == 1) ){
		// The underflow was detected.
		if( gDD_UDC_Mng[ch].pCallBack != NULL ){
			// Call callback function
			result = D_DD_UDC_UNDER_FLOW;
			(*gDD_UDC_Mng[ch].pCallBack)(ch, result);
		}
		// Clear interrupt flag
		reg = IO_PERI.UDC[ch].CSR.word;
		reg &= ~D_DD_UDC_CSR_BIT_UDFF;							// Clear UDFF
		reg |= (D_DD_UDC_CSR_BIT_CMPF | D_DD_UDC_CSR_BIT_OVFF);	// Not change CMPF, OVFF
		IO_PERI.UDC[ch].CSR.word = reg;
		Dd_ARM_Dsb_Pou();
	}

	if( (IO_PERI.UDC[ch].CCR.bit.__CFIE == 1) && (IO_PERI.UDC[ch].CCR.bit.__CDCF == 1) ){
		// Count direction change was detected.
		if( gDD_UDC_Mng[ch].pCallBack != NULL ){
			// Call callback function
			result = D_DD_UDC_COUNT_DIRECTION;
			(*gDD_UDC_Mng[ch].pCallBack)(ch, result);
		}
		// Clear interrupt flag
		IO_PERI.UDC[ch].CCR.word &= ~D_DD_UDC_CCR_BIT_CDCF;		// Clear CDCF
		Dd_ARM_Dsb_Pou();
	}
	return;
}

/**
Set count mode.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
@param [in]	count_mode	Count mode.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CMS_TIMER 
				- @ref D_DD_UDC_CMS_UP_DOWN
				- @ref D_DD_UDC_CMS_PHASE_LAG_2
				- @ref D_DD_UDC_CMS_PHASE_LAG_4
*/
INT32 Dd_UDC_Set_Count_Mode( UCHAR ch, UCHAR count_mode )
{
	union io_udc_ccr ccr;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Set_Count_Mode parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	// Check count mode.
	if( dd_udc_chk_count_mode(count_mode) != D_DDIM_OK ){
		// Error of count mode
		Ddim_Assertion(("E:Dd_UDC_Set_Count_Mode parameter error. count_mode=%d\n", count_mode));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	ccr = dd_udc_get_safe_write_ccr(ch);
	ccr.bit.__CMS  = count_mode;
	IO_PERI.UDC[ch].CCR.word = ccr.word;

	return D_DDIM_OK;
}

/**
Set the callback function when interruption is generate to pCallBack.
@param [in] ch	Channel number.<br>
@param [in]		pCallBack	Call Back function pointer
*/
VOID Dd_UDC_Set_CallBack( UCHAR ch, DD_UDC_CALLBACK pCallBack )
{
#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Set_CallBack parameter error. ch=%d\n", ch));
		return;
	}
#endif	// CO_PARAM_CHECK

	gDD_UDC_Mng[ch].pCallBack = pCallBack;
	return;
}

/**
The input counter is set to the RCR register.
@param [in] ch		Channel number.<br>
@param [in] rcr		Set reload / compare counter
*/
INT32 Dd_UDC_Set_RCR_Counter(UCHAR ch, USHORT rcr)
{
#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Set_RCR_Counter parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	dd_udc_set_rcr(ch, rcr);

	return D_DDIM_OK;
}

/**
The Up/Down counter value is returned.
@param [in] ch	Channel number.<br>
@retval UDCR counter value.<br>
*/
USHORT Dd_UDC_Get_UDCR_Counter(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Get_UDCR_Counter parameter error. ch=%d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK
	return (USHORT)(IO_PERI.UDC[ch].UDCR.bit.D);
}

/**
The input counter is set to UDCR register.
@param [in] ch		Channel number.<br>
@param [in] udcr	Set counter value.
*/
INT32 Dd_UDC_Set_UDCR_Counter(UCHAR ch, USHORT udcr)
{
	union io_udc_csr	csr;
	union io_udc_ccr 	ccr;
	UINT32				to_cnt = 0;
	INT32				result;
	DDIM_USER_ER		ercd;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Set_UDCR_Counter parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	result   = D_DDIM_OK;
	csr.word = IO_PERI.UDC[ch].CSR.word;

	// Wait for CSTR state clear. (about 100msec)
	while( csr.bit.__CSTR == 1 ){	/* pgr0872 */
		ercd = DDIM_User_Dly_Tsk(1);
		if( ercd != D_DDIM_USER_E_OK ){
			// DDIM_User_Dly_Tsk error
			Ddim_Print(("E:Dd_UDC_Set_UDCR_Counter(CSTR): DDIM_User_Dly_Tsk error. ercd=%d\n", ercd));
		}
		to_cnt++;
		if( to_cnt > 100 ){
			// Error when not stopping even if 100ms is waited
			Ddim_Print(("E:Dd_UDC_Set_UDCR_Counter: Macro busy error.\n"));
			return D_DD_UDC_ERROR_BUSY;
		}
		csr.word = IO_PERI.UDC[ch].CSR.word;
	}

	// UDCR is written by way of RCR.
	IO_PERI.UDC[ch].RCR.bit.D = (ULONG)udcr;					// Temporarily write

	ccr = dd_udc_get_safe_write_ccr(ch);
	ccr.bit.__CTUT = 1;							// Set RCR --> UDCR
	IO_PERI.UDC[ch].CCR.word = ccr.word;

	// Wait for RCR is forwarded to UDCR. (about 10msec)
	to_cnt = 0;
	while (IO_PERI.UDC[ch].UDCR.bit.D != (ULONG)udcr) {
		ercd = DDIM_User_Dly_Tsk(1);
		if( ercd != D_DDIM_USER_E_OK ){
			// DDIM_User_Dly_Tsk error
			Ddim_Print(("E:Dd_UDC_Set_UDCR_Counter(UDCR):DDIM_User_Dly_Tsk error. ercd=%d\n", ercd));
		}
		to_cnt++;
		if( to_cnt > 10 ){
			// Error when UDCR doesn't become equivalent with write value.
			result = D_DD_UDC_ERROR_BUSY;
			Ddim_Print(("E:Dd_UDC_Set_UDCR_Counter: UDCR write error.\n"));
			break;
		}
	}

	IO_PERI.UDC[ch].RCR.bit.D = (ULONG)dd_udc_get_rcr(ch);	// Roll back RCR

	return result;
}

/**
The RDCC bit of the CCR register of the specified channel is cleared.
@param [in] ch	Channel number.<br>
*/
INT32 Dd_UDC_Force_Clear_UDCR(UCHAR ch)
{
	union io_udc_ccr ccr;

#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Force_Clear_UDCR parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	ccr = dd_udc_get_safe_write_ccr(ch);
	ccr.bit.__UDCC = 0;					// clear UDCR
	IO_PERI.UDC[ch].CCR.word = ccr.word;

	return D_DDIM_OK;
}

/**
Get kind of the count immediately before. 
@param [in] ch	Channel number.<br>
*/
UCHAR Dd_UDC_Get_Up_Down_Flg(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	// channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Get_Up_Down_Flg parameter error. ch=%d\n", ch));
		return 0;
	}
#endif	// CO_PARAM_CHECK

	return IO_PERI.UDC[ch].CSR.bit.__UDF;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
Configuration for timer utility function.
@param [in] ch					Channel number.<br>
@param [in] usec				Setting time.(usec order)<br>
@param [in] timer_callback		Callback function for timer function.<br>
*/
INT32 Dd_UDC_Write_Timer_Mode( UCHAR ch, ULONG usec, DD_UDC_TIMER_CALLBACK timer_callback )
{
	INT32 result;
	ULLONG timer_count;
	UCHAR clock_sel;
	T_DD_UDC_CTRL_CMN udc_crl;

#ifdef CO_PARAM_CHECK
	// Channel number range check
	if( dd_udc_chk_ch_number(ch) != D_DDIM_OK ){
		// Error of channel number
		Ddim_Assertion(("E:Dd_UDC_Write_Timer_Mode parameter error. ch=%d\n", ch));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}

	// Callback function check
	if( timer_callback == NULL ){
		// Error of callback function
		Ddim_Assertion(("E:Dd_UDC_Write_Timer_Mode parameter error. timer_callback is NULL"));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// Calculate timer counter
	dd_udc_timer_calculate(ch, usec, &clock_sel, &timer_count);

	// Save soft_counter to loop timer operation
	gDD_UDC_Timer_Mng[ch].reload_count = ( timer_count / D_DD_UDC_COUNTR_MAX );
	gDD_UDC_Timer_Mng[ch].remain_count = ( timer_count % D_DD_UDC_COUNTR_MAX );

	// Set reload counter
	if( gDD_UDC_Timer_Mng[ch].reload_count >= 1 ){
		udc_crl.rcr  = D_DD_UDC_COUNTR_MAX;		// Set reload counter as MAX value
		udc_crl.udcr = D_DD_UDC_COUNTR_MAX;
	}
	else{
		udc_crl.rcr  = gDD_UDC_Timer_Mng[ch].remain_count;	// Set reload counter
		udc_crl.udcr = gDD_UDC_Timer_Mng[ch].remain_count;
	}

	udc_crl.count_mode = D_DD_UDC_CMS_TIMER;		//Set timer mode
	udc_crl.comp_clear = D_DD_UDC_UCRE_OFF;			//Compare clear disable
	udc_crl.reload = D_DD_UDC_RLDE_ON;				//Reload enable
	udc_crl.zin_mode = D_DD_UDC_CGSC_CLEAR;			//Set counter clear function for ZIN terminal function
	udc_crl.zin_edge = D_DD_UDC_CGE_NOT;			//Disable edge detection for ZIN terminal function
	udc_crl.cmp_int = D_DD_UDC_CITE_OFF;			//Compare interrupt disable
	udc_crl.under_over = D_DD_UDC_UDIE_ON;			//Under/Over flow interrupt enable
	udc_crl.cnt_dir_int = D_DD_UDC_CFIE_OFF;		//Count direction interrupt disable
	udc_crl.pCallBack = dd_udc_utility_callback;	//Set callback function for check timer reload;

	result = Dd_UDC_Ctrl_Common(ch, &udc_crl);
	if(result != D_DDIM_OK){
		// Parameter setting error.
		Ddim_Print(("E:Dd_UDC_Write_Timer_Mode ctrl_common parameter setting error. ch=%d, result=%d\n", ch, result));
		return result;
	}

	result = Dd_UDC_Ctrl_Timer(ch, clock_sel);
	if( result != D_DDIM_OK ){
		// CLKS setting error.
		Ddim_Print(("E:Dd_UDC_Write_Timer_Mode prescaler setting error. ch=%d, result=%d\n", ch, result));
		return result;
	}

	// Set callback function for inform the completion of the timer count
	gDD_UDC_Timer_Mng[ch].pTimer_CallBack = timer_callback;

	return D_DDIM_OK;
}
//---------------------- colabo  section -------------------------------
#endif

