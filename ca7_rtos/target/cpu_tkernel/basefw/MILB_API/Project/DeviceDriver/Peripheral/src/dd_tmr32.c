/**
 * @file		dd_tmr32.c
 * @brief		peripheral 32 bit reload timer driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_tmr32.h"
#include "peripheral.h"
#include "dd_top.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#define D_DD_TMR32_1SEC_COUNT	(1687500)	//Dd_Top_Get_RCLK()[Hz] * 1[sec] / 32[div]

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
// Nothing Special

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
// Nothing Special

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
// Soft-Counter : 1 - 0xFFFFFFFF
static volatile ULONG		gDD_TMR32_Soft_Counter[D_DD_TMR32_CH_NUM_MAX]		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Initial value of Soft-Counter : 1 - 0xFFFFFFFF
static volatile ULONG		gDD_TMR32_Soft_Counter_Ini[D_DD_TMR32_CH_NUM_MAX]	= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Reload Flag : Reload = 1 / One-Shot = 0
static volatile UCHAR		gDD_TMR32_Set_Reload_Flg[D_DD_TMR32_CH_NUM_MAX]		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Pointer of Callback Function
static volatile VP_CALLBACK	gDD_TMR32_Callback[D_DD_TMR32_CH_NUM_MAX]			= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

// OUT Level
static volatile UINT32		gDD_TMR32_Out_Level = 0;


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Nothing Special
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
//static UCHAR dd_tmr32_cal_digit(ULONG value);
static INT32 dd_tmr32_check_overflow(ULONG param_1, ULONG param_2, ULONG margin );
static INT32 dd_tmr32_get_soft_counter(ULONG clk, ULONG time, ULONG dividing, ULONG* soft_counter);

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
 * @brief  The 32bit Reload Timer channel is exclusively controlled.
 * @param  UCHAR ch
 * @param  INT32 tmout
 * @return INT32 D_DDIM_OK/D_TIMER32_SEM_NG/D_DD_TMR32_INPUT_PARAM_ERR/D_DD_TMR32_SEM_TIMEOUT
 */
INT32 Dd_TMR32_Open(UINT32 ch, INT32 tmout)
{

	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_Timer32_Open: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR) {
		Ddim_Assertion(("Dd_Timer32_Open: input param error. tmout = %x\n", tmout));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif

	if( D_DDIM_USER_SEM_WAIT_POL == tmout ) {
		ercd = DDIM_User_Pol_Sem(SID_DD_TMR32(ch));							// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem(SID_DD_TMR32(ch), (DDIM_USER_TMO)tmout);	// twai_sem()
	}
	
	if( D_DDIM_USER_E_OK != ercd ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			return D_DD_TMR32_SEM_TIMEOUT;
		}
		return D_DD_TMR32_SEM_NG;
	}
	return D_DDIM_OK;

}


/**
 * @brief  The exclusive control of control Channel it is released.
 * @param  UINT32 ch
 * @return UINT32 D_DDIM_OK/D_DD_TMR32_SEM_NG/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Close(UINT32 ch)
{

	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if ( ch >= D_DD_TMR32_CH_NUM_MAX ) {
		Ddim_Assertion(("Dd_TMR32_Close: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif

	// Global variable is clear
	gDD_TMR32_Soft_Counter[ch]		= 0;
	gDD_TMR32_Soft_Counter_Ini[ch]	= 0;
	gDD_TMR32_Set_Reload_Flg[ch]	= 0;
	gDD_TMR32_Callback[ch]			= NULL;

	ercd = DDIM_User_Sig_Sem( SID_DD_TMR32(ch) );				// sig_sem()

	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_TMR32_SEM_NG;
	}
	return D_DDIM_OK;

}


/**
 * @brief  The operation condition of Reload Timer (Channel) is set.
 * @param  UCHAR ch, T_DD_TMR32_CTRL* tmr32_ctrl
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Ctrl(UINT32 ch , T_DD_TMR32_CTRL const* const tmr32_ctrl)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_Timer32_Ctrl: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
	if (tmr32_ctrl == NULL) {
		Ddim_Assertion(("Dd_Timer32_Ctrl: input param error. [*tmr32_ctrl] NULL\n"));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	// It sets it to the register
	IO_PERI.TMR[ch].TMCSR.word		= tmr32_ctrl->csr.word;
	IO_PERI.TMR[ch].TMCSR.word	   &= 0xFFFFFFF8;
	IO_PERI.TMR[ch].TMRLR0			= tmr32_ctrl->rlr1;
	IO_PERI.TMR[ch].TMRLR1			= tmr32_ctrl->rlr2;
	
	// The reload counter is set
	gDD_TMR32_Soft_Counter[ch]		= tmr32_ctrl->soft_counter;
	gDD_TMR32_Soft_Counter_Ini[ch]	= tmr32_ctrl->soft_counter;
	
	// Reload check
	if (tmr32_ctrl->csr.bit.reld == 1) {
		gDD_TMR32_Set_Reload_Flg[ch] = 1;	// Reload
	}
	else {
		gDD_TMR32_Set_Reload_Flg[ch] = 0;	// One shot
	}

	// The callback function is registered
	gDD_TMR32_Callback[ch] = tmr32_ctrl->pcallback;
	
	return D_DDIM_OK;
}

/**
 * @brief  Hard Counter, Soft Counter, and other parameters are calculated according
           to the designated time (usec).
           If it doesn't calculate, Input Parameter Error is returned.
 * @param  ULONG usec, T_DD_TMR32_CTRL* tmr32_ctrl, ULONG soft_counter
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Calculate(ULONG usec, T_DD_TMR32_CTRL* const tmr32_ctrl)
{
	ULONG  total_count;
	ULONG  dividing[3];
	ULLONG  count_base, count_base_wk;
	ULONG  peri_clk, peri_clk_wk;
	ULONG  soft_counter = 1;
	ULONG  soft_cnt;
	ULONG  dividing_wk_1, dividing_wk_2, dividing_wk_3;
	ULONG  peri_cnt_1 = 0;
	ULONG  peri_cnt_2 = 0;
	ULONG  peri_cnt_3 = 0;
	ULONG  margin_1, margin_2, margin_3;
//	ULONG  repeat_base;
//	ULONG  repeat;
	INT32  ret;
	INT8   div_wk;
	UCHAR  find_flag = 0;
	
	dividing[0]	 = 2;
	dividing[1]	 = 8;
	dividing[2]	 = 32;

#ifdef CO_PARAM_CHECK
	if (usec == 0) {
		Ddim_Assertion(("Dd_TMR32_Calculate: input param error. [usec] = 0\n"));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
	if (tmr32_ctrl == NULL) {
		Ddim_Assertion(("Dd_TMR32_Calculate: input param error. [*tmr32_ctrl] NULL\n"));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif

	peri_clk = Dd_Top_Get_RCLK();
	
	count_base = (ULLONG)peri_clk * usec;	// Number of total counts(Overflow is a disregarded number of counts)
	
	peri_clk_wk = peri_clk;
	count_base_wk = count_base;

	for (div_wk = 2; div_wk >= 0; div_wk--) {
		count_base = (ULLONG)(peri_clk / 1000000) * usec;
		if ((count_base / dividing[div_wk]) < 1) {
			continue;
		}
		if ((count_base % dividing[div_wk]) != 0) {
			continue;
		}
		for (;;) {
			if ((count_base_wk & (dividing[div_wk] - 1)) == 0) {
				soft_cnt = soft_counter;
				
				// Check overflow
				ret = dd_tmr32_get_soft_counter(peri_clk_wk, usec, dividing[div_wk], &soft_counter);
				if (ret == 0) {
					if (soft_cnt == soft_counter) {
						find_flag = 1;
						break;
					}
					peri_clk_wk = peri_clk / soft_counter;
					count_base_wk = (ULLONG)peri_clk_wk * usec;
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
		if (find_flag == 1) {
			if ((peri_clk % soft_counter) != 0) {
				Ddim_Print(("Dd_TMR32_Calculate:It cannot be divided.\n"));
			}
			break;
		}
	}
	while (1) {
		if (find_flag == 1) {
			for (dividing_wk_1 = dividing[div_wk]; dividing_wk_1 >= 1; dividing_wk_1 /= 2) {
				if ((peri_clk_wk % dividing_wk_1) == 0) {
					peri_cnt_1 = (peri_clk_wk / dividing_wk_1);
					break;
				}
			}
			dividing_wk_2 = dividing[div_wk] / dividing_wk_1;
			dividing_wk_3 = dividing_wk_2;
			
			for (; dividing_wk_2 >= 1; dividing_wk_2 /= 2) {
				if ((usec % dividing_wk_2) == 0) {
					peri_cnt_2 = (usec / dividing_wk_2);
					break;
				}
			}
			dividing_wk_3 = (dividing[div_wk] / dividing_wk_1) / dividing_wk_2;
			
			for (margin_1 = 1000000; margin_1 > 0; margin_1 /= 10) {
				if ((peri_cnt_1 % margin_1) == 0) {
					peri_cnt_1 = (peri_cnt_1 / margin_1);
					break;
				}
			}
			// Division in multiples of 10
			if ((1000000 / margin_1) > 1) {
				for (margin_2 = (1000000 / margin_1); margin_2 > 0; margin_2 /= 10) {
					if ((peri_cnt_2 % margin_2) == 0) {
						peri_cnt_2 = (peri_cnt_2 / margin_2);
						break;
					}
				}
				margin_3 = (1000000 / margin_1) / margin_2;
				peri_cnt_3 = margin_3;
			}
			else {
				margin_2 = 1;
				margin_3 = 1;
				peri_cnt_3 = margin_3;
			}
			// Check overflow
			ret = dd_tmr32_check_overflow(peri_cnt_1, peri_cnt_2, margin_3 );
			if (ret != 0) {
				// Division in multiples of 2
				switch (margin_3) {
					case 1000000:
						margin_1 = 64;
						margin_2 = 64;
						break;
					case 100000:
						margin_1 = 32;
						margin_2 = 32;
						break;
					case 10000:
						margin_1 = 16;
						margin_2 = 16;
						break;
					case 1000:
						margin_1 = 8;
						margin_2 = 8;
						break;
					case 100:
						margin_1 = 4;
						margin_2 = 4;
						break;
					case 10:
						margin_1 = 2;
						margin_2 = 2;
						break;
					default:
						find_flag = 0;
						Ddim_Print(("Dd_TMR32_Calculate:It was not possible to request it though it calculated.\n"));
						break;
				}
				for (; margin_1 > 0; margin_1 /= 2) {
					if ((peri_cnt_1 % margin_1) == 0) {
						peri_cnt_1 = (peri_cnt_1 / margin_1);
						break;
					}
				}
				if ((margin_3 / margin_1) > 1) {
					for (margin_2 = (margin_2 / margin_1); margin_2 > 0; margin_2 /= 2) {
						if ((peri_cnt_2 % margin_2) == 0) {
							peri_cnt_2 = (peri_cnt_2 / margin_2);
							break;
						}
					}
					margin_3 = (margin_3 / margin_1) / margin_2;
					peri_cnt_3 = margin_3;
				}
				else {
					margin_2 = 1;
					margin_3 = 1;
					peri_cnt_3 = margin_3;
				}
				ret = dd_tmr32_check_overflow(peri_cnt_1, peri_cnt_2, margin_3 );
				if (ret != 0) {
					find_flag = 0;
					Ddim_Print(("Dd_TMR32_Calculate:It was not possible to request it though it calculated.\n"));
					break;
				}
			}
			total_count = (((ULLONG)peri_cnt_1 * peri_cnt_2) / dividing_wk_3) / peri_cnt_3;

			// 32bit counter
			tmr32_ctrl->rlr1         = total_count - 1;
			tmr32_ctrl->rlr2         = total_count - 1;
			tmr32_ctrl->soft_counter = soft_counter;
			tmr32_ctrl->csr.word     = 0x00000018;		// Periodic mode, Interrupt Enable, Reload
			tmr32_ctrl->csr.bit.csl  = div_wk;
		}
		break;
	}
	if (find_flag == 1) {
		return D_DDIM_OK;
	}
	else {
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
}


/**
 * @brief  If it doesn't calculate, Input Parameter Error is returned.
 * @param  UINT32 pwm_ch, UCHAR mode, T_DD_TMR32_CTRL* tmr32_ctrl
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Calculate_Pwm_Sync(UINT32 pwm_ch, UCHAR mode, T_DD_TMR32_CTRL* const tmr32_ctrl)
{
	ULONG  total_count;
	ULONG  soft_counter = 0;
	INT8   div_wk;

#ifdef CO_PARAM_CHECK
	if ((pwm_ch != 4)&&(pwm_ch != 8)) {
		Ddim_Assertion(("Dd_TMR32_Calculate_Pwm_Sync: input param error. [ch] = %d\n", pwm_ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
	if (tmr32_ctrl == NULL) {
		Ddim_Assertion(("Dd_TMR32_Calculate: input param error. [*tmr32_ctrl] NULL\n"));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
	if ((mode != 0)&&(mode != 1)) {
		Ddim_Assertion(("Dd_TMR32_Calculate_Pwm_Sync: input param error. [mode] = %d\n", mode));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif

	// pwm			reload
	// 0 : φ		φ/2	
	// 1 : φ/4		φ/8
	// 2 : φ/16	φ/32
	// 3 : φ/64	
	// 60fps
	if(mode==0){
		switch(IO_PERI.PWM[pwm_ch>>2].PCNE.bit.__CKS){
			case 0:
				div_wk = 0;
				total_count = (IO_PERI.PWM[pwm_ch>>2].PCSRE1 + IO_PERI.PWM[pwm_ch>>2].PCSRE2 + 2)>>1;
				break;
			case 1:
				div_wk = 1;
				total_count = (IO_PERI.PWM[pwm_ch>>2].PCSRE1 + IO_PERI.PWM[pwm_ch>>2].PCSRE2 + 2)>>1;
				break;
			case 2:
				div_wk = 2;
				total_count = (IO_PERI.PWM[pwm_ch>>2].PCSRE1 + IO_PERI.PWM[pwm_ch>>2].PCSRE2 + 2)>>1;
				break;
			case 3:
				div_wk = 2;
				total_count = (IO_PERI.PWM[pwm_ch>>2].PCSRE1 + IO_PERI.PWM[pwm_ch>>2].PCSRE2 + 2)<<1;
				break;
			default:
				break;
		}
	}
	// 240fps
	else if(mode==1){
		switch(IO_PERI.PWM[pwm_ch>>2].PCNE.bit.__CKS){
			case 0:
				div_wk = 0;
				total_count = (IO_PERI.PWM[pwm_ch>>2].PCSRE1 + 1)>>2;
				break;
			case 1:
				div_wk = 1;
				total_count = (IO_PERI.PWM[pwm_ch>>2].PCSRE1 + 1)>>2;
				break;
			case 2:
				div_wk = 2;
				total_count = (IO_PERI.PWM[pwm_ch>>2].PCSRE1 + 1)>>2;
				break;
			case 3:
				div_wk = 2;
				total_count = (IO_PERI.PWM[pwm_ch>>2].PCSRE1 + 1)>>1;
				break;
			default:
				break;
		}
	}
	else{
		; //nothing
	}


	// 32bit counter
	tmr32_ctrl->rlr1		 = total_count - 1;
	tmr32_ctrl->rlr2		 = total_count - 1;
	tmr32_ctrl->soft_counter = soft_counter;
	tmr32_ctrl->csr.word	 = 0x00000018;		// Periodic mode, Interrupt Enable, Reload
	tmr32_ctrl->csr.bit.csl  = div_wk;


	return D_DDIM_OK;
}


/**
 * @brief  32bit Reload Timer(Channel) is started.
 * @param  UINT32 ch
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Start(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Start: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	IO_PERI.TMR[ch].TMCSR.word |= 0x0000000B;
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
 * @brief  32bit Reload Timer(Channel) wait for external trigger.
 * @param  UINT32 ch
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_External_Start(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_EXT_TRG_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_External_Start: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	IO_PERI.TMR[ch].TMCSR.word |= 0x0000000A;

	return D_DDIM_OK;
}

/**
 * @brief  32bit Reload Timer(Channel) is Stopped.
 * @param  UINT32 ch
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Stop(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Stop: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFB8;	// UF=0,CNTE=0,CNTE=0

	// If the interrupt request flag set timing and the clear
	// timing clash, the flag set has priority and the clearing
	// operation is disabled.
	IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFB8;
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}


/**
 * @brief  32bit Reload Timer(Channel) is paused.
 * @param  UINT32 ch
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Pause(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Pause: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	
	IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFBD;
	Dd_ARM_Dsb_Pou();
	return D_DDIM_OK;
}

/**
 * @brief  Get designated 32bit Reload Timer counter value.
 * @param  UINT32 ch, ULONG* h_count, ULONG* s_count
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Get_Counter(UINT32 ch, ULONG* const h_count, ULONG* const s_count)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Get_Counter: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
	if ((h_count == NULL) || (s_count == NULL)) {
		Ddim_Assertion(("Dd_TMR32_Get_Counter: input param error. [*h_count] or [*s_count] is NULL\n"));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	
	*h_count = IO_PERI.TMR[ch].TMR;
	*s_count = gDD_TMR32_Soft_Counter[ch];
	
	return D_DDIM_OK;
}

/**
 * @brief  All the register values of 32bit reload timer are acquired.
 * @param  UINT32 ch, T_DD_TMR32_CTRL* tmr32_ctrl
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Get_Ctrl(UINT32 ch, T_DD_TMR32_CTRL* const tmr32_ctrl)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Get_Ctrl: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
	if (tmr32_ctrl == NULL) {
		Ddim_Assertion(("Dd_TMR32_Get_Ctrl: input param error. [*tmr32_ctrl] NULL\n"));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	
	// Register value acquisition
	tmr32_ctrl->csr.word		= IO_PERI.TMR[ch].TMCSR.word;
	tmr32_ctrl->soft_counter	= gDD_TMR32_Soft_Counter[ch];
	tmr32_ctrl->pcallback		= gDD_TMR32_Callback[ch];
	
	return D_DDIM_OK;
}

/**
 * @brief  Only the soft count value that wants to repeat in the reload mode is set.
 * @param  UINT32 ch, ULONG soft_counter
 * @return VOID
 */
INT32 Dd_TMR32_Set_Softcounter(UINT32 ch, ULONG soft_counter)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Set_Softcounter: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	
	gDD_TMR32_Soft_Counter[ch] = soft_counter;
	return D_DDIM_OK;
}

/**
 * @brief  The soft count value is returned.
 * @param  UINT32 ch, ULONG soft_counter
 * @return ULONG soft counter
 */
ULONG Dd_TMR32_Get_Softcounter(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Set_Reload_Flg: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif
	
	return gDD_TMR32_Soft_Counter[ch];
}

/**
 * @brief  The reload_flg is set.
 * @param  UINT32 ch, UCHAR soft_counter
 * @return INT32 D_DDIM_OK / D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Set_Reload_Flg(UINT32 ch, UCHAR flg)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Set_Reload_Flg: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	
	gDD_TMR32_Set_Reload_Flg[ch] = flg;
	
	return D_DDIM_OK;
}

/**
 * @brief  The reload_flg value is returned.
 * @param  UINT32 ch, ULONG soft_counter
 * @return VOID
 */
INT32 Dd_TMR32_Get_Reload_Flg(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_Get_Reload_Flg: input param error. [ch] = %x\n", ch));
		return 0;
	}
#endif
	return gDD_TMR32_Set_Reload_Flg[ch];
}

/**
 * @brief  The interruption flag is cleared, and the CallBack function for 32bit Reload Timer is called.
 * @param  UINT32 ch
 * @return VOID
 */
VOID Dd_TMR32_Int_Handler(UINT32 ch)
{
	union	io_tmr_tmcsr	tmcsr;

	tmcsr.word = IO_PERI.TMR[ch].TMCSR.word;

//	printf("Dd_TMR32_Int_Handler = %d \n", ch);

	// The channel of the generated interruption is checked. 
	if ((tmcsr.word & 0x00000006) != 0x00000006) {
		return;
	}
	IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFBB;		// UF bit clear
	Dd_ARM_Dsb_Pou();
	
	// Soft-Counter check
	if (gDD_TMR32_Soft_Counter[ch] > 1) {
		gDD_TMR32_Soft_Counter[ch]--;
	}
	else {
		// Reload check
		if ((tmcsr.bit.__RELD == 1) && (gDD_TMR32_Set_Reload_Flg[ch] == 1)) {	/* pgr0872 */
			gDD_TMR32_Soft_Counter[ch] = gDD_TMR32_Soft_Counter_Ini[ch];
		}
		else {
			Dd_TMR32_Stop(ch);
		}
		
		// Callback check
		if (gDD_TMR32_Callback[ch]) {
			(*gDD_TMR32_Callback[ch])();	// Call Back function
		}
	}
}

#if 0
static UCHAR dd_tmr32_cal_digit(ULONG value)
{
	ULONG digit_value;
	INT8  digit_num;
	UCHAR digit = 0;
	
	for (digit_num = 31; digit_num >= 0; digit_num--) {
		digit_value = (1 << digit_num);
		if (value >= digit_value) {
			digit = (digit_num + 1);
			break;
		}
	}
	return digit;
}
#endif

static INT32 dd_tmr32_check_overflow(ULONG param_1, ULONG param_2, ULONG margin )
{
	INT32 ret;
	ULLONG total;

	total = param_1 * param_2 / margin;
	
	if (( total & 0xFFFFFFFF00000000 ) == 0) {
		ret = 0;
	}
	else {
		ret = 1;
	}
	return ret;
}

static INT32 dd_tmr32_get_soft_counter(ULONG clk, ULONG time, ULONG dividing, ULONG* soft_counter)
{
	ULONG cnt;
	LLONG clock_count;
	
	cnt = *soft_counter;
	
	clock_count = (LLONG)time * clk / dividing / 1000000;

	if( clock_count < 1 ){
		return -1;
	}
	else if( clock_count <= 0x100000000 ){
		return 0;
	}
	else{
		cnt = clock_count >> 32;
		if( clock_count & 0xFFFFFFFF ){
			cnt += 1;
		}

		for( ; ; ) {

			if ((clk % cnt) == 0)  {
				break;
			}
			else{
				if (clk <= cnt) {
					*soft_counter = 0;
					Ddim_Print(("dd_tmr32_get_soft_counter: check overflow error.\n"));
					return -1;
				}
				cnt++;
			}
		}

		*soft_counter = cnt;
	}
	return 0;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
 * @brief  The operation condition of Reload Timer (Channel) is set.
           Timer value unit:[ms]
 * @param  UINT32 ch, ULONG usec, ULONG soft_counter, VP_CALLBACK pcallback
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_SetTimer(UINT32 ch, ULONG usec, VP_CALLBACK pcallback)
{
	INT32 result;
	T_DD_TMR32_CTRL tmr32_ctrl;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_SetTimer: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	
	// A reload counter and the final, set value are cleared
	gDD_TMR32_Soft_Counter[ch]		= 0;
	gDD_TMR32_Soft_Counter_Ini[ch]	= 0;
	
	result = Dd_TMR32_Calculate(usec, &tmr32_ctrl);	// The register count value and the number of repetitions are requested.
	if (result != D_DDIM_OK) {
		Ddim_Print(("Dd_TMR32_SetTimer: Calculate error. ret=0x%x\n", result));
		return result;
	}
	tmr32_ctrl.pcallback = pcallback;

	if(ch == 3) {
		//OUTL resetting
		tmr32_ctrl.csr.bit.outl = gDD_TMR32_Out_Level;
	}
	result = Dd_TMR32_Ctrl(ch, &tmr32_ctrl);
	if (result != D_DDIM_OK) {
		Ddim_Print(("Dd_TMR32_SetTimer: ctrl error. ret=0x%x\n", result));
	}
	return result;
}

/**
 * @brief  The operation condition of Reload Timer (Channel) is set.
           Timer value unit:[ms]
 * @param  UINT32 reload_ch, UINT32 pwm_ch,  UCHAR mode, VP_CALLBACK pcallback
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_SetTimer_pwm(UINT32 reload_ch, UINT32 pwm_ch, UCHAR mode, VP_CALLBACK pcallback)
{
	INT32 result;
	T_DD_TMR32_CTRL tmr32_ctrl;
	
#ifdef CO_PARAM_CHECK
	if (reload_ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_SetTimer: input param error. [ch] = %x\n", reload_ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	
	// A reload counter and the final, set value are cleared
	gDD_TMR32_Soft_Counter[reload_ch]		= 0;
	gDD_TMR32_Soft_Counter_Ini[reload_ch]	= 0;
	
	result = Dd_TMR32_Calculate_Pwm_Sync(pwm_ch, mode, &tmr32_ctrl);	// The register count value and the number of repetitions are requested.
	if (result != D_DDIM_OK) {
		Ddim_Print(("Dd_TMR32_SetTimer: Dd_TMR32_Calculate_Pwm_Sync error. ret=0x%x\n", result));
		return result;
	}
	tmr32_ctrl.pcallback = pcallback;

	result = Dd_TMR32_Ctrl(reload_ch, &tmr32_ctrl);
	if (result != D_DDIM_OK) {
		Ddim_Print(("Dd_TMR32_SetTimer: ctrl error. ret=0x%x\n", result));
	}
	return result;
}


/**
 * @brief  32bit Reload Timer(Channel) is started for reload.
 * @param  UINT32 ch
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_StartReload(UINT32 ch)
{
	INT32 result = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_StartReload: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	Dd_TMR32_Set_Reload_Flg(ch, 1);	// Reload
	
	IO_PERI.TMR[ch].TMCSR.word |= 0x0000001B;
	Dd_ARM_Dsb_Pou();
	
	return result;
}

/**
 * @brief  Designated Channel of 32bit Reload Timer is started for oneshot.
 * @param  UINT32 ch
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_StartOneshot(UINT32 ch)
{
#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		Ddim_Assertion(("Dd_TMR32_StartOneshot: input param error. [ch] = %x\n", ch));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif
	Dd_TMR32_Set_Reload_Flg(ch, 0);		// One shot
	
	// Reload check
	if (Dd_TMR32_Get_Softcounter(ch) >= 2) {
		IO_PERI.TMR[ch].TMCSR.word |= 0x0000001B;
	}
	else {
		IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFEF;	// RELD bit clear
		IO_PERI.TMR[ch].TMCSR.word |= 0x0000000B;
	}
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief  The out_level(OUTL) is set.
 * @param  UINT32 out_level
 * @return INT32 D_DDIM_OK/D_DD_TMR32_INPUT_PARAM_ERR
 */
INT32 Dd_TMR32_Set_Outlevel(UINT32 out_level)
{
#ifdef CO_PARAM_CHECK
	if (out_level >= 2) {
		Ddim_Assertion(("Dd_TMR32_Set_Outlevel: input param error. [out_level] = %x\n", out_level));
		return D_DD_TMR32_INPUT_PARAM_ERR;
	}
#endif

	if (out_level == 0) {
		IO_PERI.TMR[3].TMCSR.word &= 0xFFFFFFDF;	// OUTL bit clear
	}
	else {
		IO_PERI.TMR[3].TMCSR.word |= 0x00000020;	// OUTL bit set
	}
	gDD_TMR32_Out_Level = out_level;

	return D_DDIM_OK;
}


//---------------------------- colabo section ----------------------------
// Nothing Special
#endif
