/**
 * @file		ct_dd_udc.c
 * @brief		This is test code for dd_udc.c
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ddim_user_custom.h"
#include "peripheral.h"
#include "dd_udc.h"
#include "dd_top.h"
#include "dd_tmr32.h"
#include "stdlib.h"
#include "string.h"

#include "dd_gic.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CT_DD_UDC_TMR_OBS_CH	(2)	// Timer32 ch for UDCR observer
#define D_CT_DD_UDC_TMR_AIN_CH	(3)	// Timer32 ch for virtual AIN
#define D_CT_DD_UDC_TMR_BIN_CH	(4)	// Timer32 ch for virtual BIN

#define D_CT_DD_UDC_CH0_AIN		(E_DD_TOP_GPIO_PF2)
#define D_CT_DD_UDC_CH0_BIN		(E_DD_TOP_GPIO_PF0)
#define D_CT_DD_UDC_CH0_ZIN		(E_DD_TOP_GPIO_PE1)
#define D_CT_DD_UDC_CH1_AIN		(E_DD_TOP_GPIO_PF1)
#define D_CT_DD_UDC_CH1_BIN		(E_DD_TOP_GPIO_P87)
#define D_CT_DD_UDC_CH1_ZIN		(E_DD_TOP_GPIO_PE3)
#define D_CT_DD_UDC_CH2_AIN		(E_DD_TOP_GPIO_PE0)
#define D_CT_DD_UDC_CH2_BIN		(E_DD_TOP_GPIO_PE1)
#define D_CT_DD_UDC_CH3_AIN		(E_DD_TOP_GPIO_PE2)
#define D_CT_DD_UDC_CH3_BIN		(E_DD_TOP_GPIO_PE3)
#define D_CT_DD_UDC_CH4_AIN		(E_DD_TOP_GPIO_PE4)
#define D_CT_DD_UDC_CH4_BIN		(E_DD_TOP_GPIO_PE5)
#define D_CT_DD_UDC_CH5_AIN		(E_DD_TOP_GPIO_PM0)
#define D_CT_DD_UDC_CH5_BIN		(E_DD_TOP_GPIO_PP3)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Input port kind of UDC
typedef enum {
	E_CT_DD_UDC_TRG_KIND_AIN = 0,	// AIN
	E_CT_DD_UDC_TRG_KIND_BIN,		// BIN
	E_CT_DD_UDC_TRG_KIND_ZIN,		// ZIN
	E_CT_DD_UDC_TRG_KIND_END		// End of enum
} E_CT_DD_UDC_TRG_KIND;

// Kind of general-purpose port register
typedef enum {
	E_CT_DD_UDC_PORT_KIND_PDR = 0,	// PDR
	E_CT_DD_UDC_PORT_KIND_DDR,		// DDR
	E_CT_DD_UDC_PORT_KIND_EPCR,		// EPCR
	E_CT_DD_UDC_PORT_KIND_END		// End of enum
} E_CT_DD_UDC_PORT_KIND;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Control data of virtual AIN input.
static UCHAR	gCt_Dd_Udc_Ain_Ch;		// Target UDC channel number
static DDIM_USER_SYSTIM	gCt_Dd_Udc_Tim_A_Sta;
static DDIM_USER_SYSTIM	gCt_Dd_Udc_Tim_A_End;

// Control data of virtual BIN input.
static UCHAR	gCt_Dd_Udc_Bin_Ch;		// Target UDC channel number
static DDIM_USER_SYSTIM	gCt_Dd_Udc_Tim_B_Sta;
static DDIM_USER_SYSTIM	gCt_Dd_Udc_Tim_B_End;

// Control data of UDCR observer
static UCHAR	gCt_Dd_Udc_Obs_ch;		// Target UDC channel number

//Save system time for UDC utility function
static DDIM_USER_SYSTIM	gCt_Dd_Udc_Tim_Util_Sta;
static DDIM_USER_SYSTIM	gCt_Dd_Udc_Tim_Util_End;





#define D_CT_DD_UDC_PORT_AIN_CH0	(IO_CHIPTOP.PDR.bit.PS6)

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/*
Set value of general-purpose port control register.
*/
static INT32 ct_dd_udc_set_port(UCHAR ch, E_CT_DD_UDC_TRG_KIND trg, E_CT_DD_UDC_PORT_KIND port, UCHAR write_val)
{
	switch (ch) {
		case D_DD_UDC_CH0:
			// UDC ch0
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH0_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH0_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
//						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH0_AIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH0_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH0_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
//						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH0_BIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				case E_CT_DD_UDC_TRG_KIND_ZIN:
					// ZIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH0_ZIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH0_ZIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
//★						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH0_ZIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_set_port(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH1:
			// UDC ch1
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH1_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH1_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
/*★*/						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH1_AIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH1_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH1_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH1_BIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				case E_CT_DD_UDC_TRG_KIND_ZIN:
					// ZIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH1_ZIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH1_ZIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH1_ZIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				default: 
					// Parameter error.
					Ddim_Print(("ct_dd_udc_set_port(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH2:
			// UDC ch2
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH2_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH2_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
//						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH2_AIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH2_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH2_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
//★						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH2_BIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_set_port(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH3:
			// UDC ch3
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH3_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH3_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
//						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH3_AIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH3_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH3_BIN, write_val );
					}
					else {
						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH3_BIN, write_val );
					}
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_set_port(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH4:
			// UDC ch4
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH4_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH4_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
//						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH4_AIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH4_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH4_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH4_BIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_set_port(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH5:
			// UDC ch5
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						//IO_CHIPTOP.PDR.bit.PR2 = write_val;
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH5_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						//IO_CHIPTOP.DDR.bit.PR2 = write_val;
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH5_AIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
//						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH5_AIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					if (port == E_CT_DD_UDC_PORT_KIND_PDR) {
						//IO_CHIPTOP.PDR.bit.PR3 = write_val;
						Dd_Top_Set_Gpio_Status( D_CT_DD_UDC_CH5_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_DDR) {
						//IO_CHIPTOP.DDR.bit.PR3 = write_val;
						Dd_Top_Set_Gpio_Direction( D_CT_DD_UDC_CH5_BIN, write_val );
					}
					else if (port == E_CT_DD_UDC_PORT_KIND_EPCR) {
						//IO_CHIPTOP.EPCR.bit.PR3 = write_val;
						Dd_Top_Set_Gpio_Function( D_CT_DD_UDC_CH5_BIN, write_val );
					}
					else {
						;	// DO NOTHING
					}
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_set_port(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		default:
			// Parameter error.
			Ddim_Print(("ct_dd_udc_set_port(): Parameter error. ch=%d, trg=%d\n", ch, trg));
			return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	return D_DDIM_OK;
}

/*
Get value of PDR register value.
*/
static INT32 ct_dd_udc_get_pdr(UCHAR ch, E_CT_DD_UDC_TRG_KIND trg, UCHAR* val)
{
	switch (ch) {
		case D_DD_UDC_CH0:
			// UDC ch0
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH0_AIN, val );
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH0_BIN, val );
					break;

				case E_CT_DD_UDC_TRG_KIND_ZIN:
					// ZIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH0_ZIN, val );
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_get_pdr(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH1:
			// UDC ch1
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH1_AIN, val );
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH1_BIN, val );
					break;

				case E_CT_DD_UDC_TRG_KIND_ZIN:
					// ZIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH1_ZIN, val );
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_get_pdr(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH2:
			// UDC ch2
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH2_AIN, val );
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH1_BIN, val );
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_get_pdr(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH3:
			// UDC ch3
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH3_AIN, val );
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH3_BIN, val );
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_get_pdr(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH4:
			// UDC ch4
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH4_AIN, val );
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH4_BIN, val );
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_get_pdr(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		case D_DD_UDC_CH5:
			// UDC ch5
			switch (trg) {
				case E_CT_DD_UDC_TRG_KIND_AIN:
					// AIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH5_AIN, val );
					break;

				case E_CT_DD_UDC_TRG_KIND_BIN:
					// BIN
					Dd_Top_Get_Gpio_Status( D_CT_DD_UDC_CH5_BIN, val );
					break;

				default:
					// Parameter error.
					Ddim_Print(("ct_dd_udc_get_pdr(): Parameter error. ch=%d, trg=%d\n", ch, trg));
					return D_DD_UDC_INPUT_PARAM_ERROR;
			}
			break;

		default:
			// Parameter error.
			Ddim_Print(("ct_dd_udc_get_pdr(): Parameter error. ch=%d, trg=%d\n", ch, trg));
			return D_DD_UDC_INPUT_PARAM_ERROR;
	}
	return D_DDIM_OK;
}

/**
Callback function for interrupt of UDC.
*/
static VOID ct_dd_udc_callback(UCHAR ch, INT32 factor)
{
	USHORT udcr;

	// Get UDCR
	udcr = Dd_UDC_Get_UDCR_Counter(ch);

	switch (factor) {
		case D_DD_UDC_CMPF_OK:
			// Compare
			Ddim_Print(("[UDC#%d INT] D_DD_UDC_CMPF_OK. UDCR=%d\n", ch, udcr));
			break;

		case D_DD_UDC_OVER_FLOW:
			// Overflow
			Ddim_Print(("[UDC#%d INT] D_DD_UDC_OVER_FLOW. UDCR=%d\n", ch, udcr));
			break;

		case D_DD_UDC_UNDER_FLOW:
			// Underflow
			Ddim_Print(("[UDC#%d INT] D_DD_UDC_UNDER_FLOW. UDCR=%d\n", ch, udcr));
			break;

		case D_DD_UDC_COUNT_DIRECTION:
			// Count direction change
			Ddim_Print(("[UDC#%d INT] D_DD_UDC_COUNT_DIRECTION. UDCR=%d DIR=%d\n", ch, udcr, Dd_UDC_Get_Up_Down_Flg(ch)));
			break;

		default:
			// Unknown factor
			Ddim_Print(("[UDC#%d INT] Unknown factor. factor=%d UDCR=%d\n", ch, factor, udcr));
			break;
	}
}

/**
Callback sub function for interrupt of UDC.
*/
static VOID ct_dd_udc_callback_sub(UCHAR ch, INT32 factor)
{
	Ddim_Print(("[UDC#%d INT] ct_dd_udc_callback_sub() Start!!\n", ch));
	ct_dd_udc_callback(ch, factor);
	Ddim_Print(("[UDC#%d INT] ct_dd_udc_callback_sub() End!!\n", ch));
}

/**
Callback function for interrupt of UDC utility function
*/
static VOID ct_dd_udc_utility_callback( UCHAR ch )
{
	DDIM_USER_ER	ercd;

	ercd = DDIM_User_Get_Tim(&gCt_Dd_Udc_Tim_Util_End);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("UDC utility function: get end time NG. ercd=%d\n", ercd));
		return;
	}

	Ddim_Print(("Stop utility function\n"));
	Ddim_Print(("Measuring Time(msec) = %lu\n",(ULONG)(gCt_Dd_Udc_Tim_Util_End - gCt_Dd_Udc_Tim_Util_Sta)))

	return;
}

/**
Callback function for virtual AIN control.
*/
static VOID ct_dd_udc_ain_handler(VOID)
{
	DDIM_USER_ER	ercd;
	UCHAR 			pdr;

	ercd = DDIM_User_Get_Tim(&gCt_Dd_Udc_Tim_A_End);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("ct_dd_udc_ain_handler(): get_tim NG. ercd=%d\n", ercd));
		return;
	}

	ercd = (DDIM_USER_ER)ct_dd_udc_get_pdr(gCt_Dd_Udc_Ain_Ch, E_CT_DD_UDC_TRG_KIND_AIN, &pdr);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("ct_dd_udc_ain_handler(): ct_dd_udc_get_pdr NG. ercd=%d\n", ercd));
		return;
	}

	if (pdr == 0) {
		// AIN -> High
		pdr = 1;
	}
	else {
		// AIN -> Low
		pdr = 0;
	}
	ercd = (DDIM_USER_ER)ct_dd_udc_set_port(gCt_Dd_Udc_Ain_Ch, E_CT_DD_UDC_TRG_KIND_AIN, E_CT_DD_UDC_PORT_KIND_PDR, pdr);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("ct_dd_udc_ain_handler(): ct_dd_udc_set_port NG. ercd=%d\n", ercd));
		return;
	}

	Ddim_Print((
		"[AIN] (Ch=%d) (TIM=%lu) (UDCR=%d) (PDR=%d)\n",
		gCt_Dd_Udc_Ain_Ch,
		(ULONG)(gCt_Dd_Udc_Tim_A_End - gCt_Dd_Udc_Tim_A_Sta), 
		Dd_UDC_Get_UDCR_Counter(gCt_Dd_Udc_Ain_Ch), 
		pdr
	));
	gCt_Dd_Udc_Tim_A_Sta = gCt_Dd_Udc_Tim_A_End;
}

/**
Callback function for virtual BIN control.
*/
static VOID ct_dd_udc_bin_handler(VOID)
{
	DDIM_USER_ER	ercd;
	UCHAR 			pdr;

	ercd = DDIM_User_Get_Tim(&gCt_Dd_Udc_Tim_B_End);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("ct_dd_udc_bin_handler(): get_tim NG. ercd=%d\n", ercd));
		return;
	}

	ercd = (DDIM_USER_ER)ct_dd_udc_get_pdr(gCt_Dd_Udc_Bin_Ch, E_CT_DD_UDC_TRG_KIND_BIN, &pdr);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("ct_dd_udc_bin_handler(): ct_dd_udc_get_pdr NG. ercd=%d\n", ercd));
		return;
	}

	if (pdr == 0) {
		// BIN -> High
		pdr = 1;
	}
	else {
		// BIN -> Low
		pdr = 0;
	}
	ercd = (DDIM_USER_ER)ct_dd_udc_set_port(gCt_Dd_Udc_Bin_Ch, E_CT_DD_UDC_TRG_KIND_BIN, E_CT_DD_UDC_PORT_KIND_PDR, pdr);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("ct_dd_udc_bin_handler(): ct_dd_udc_set_port NG. ercd=%d\n", ercd));
		return;
	}

	Ddim_Print((
		"[BIN] (Ch=%d) (TIM=%lu) (UDCR=%d) (PDR=%d)\n",
		gCt_Dd_Udc_Bin_Ch,
		(ULONG)(gCt_Dd_Udc_Tim_B_End - gCt_Dd_Udc_Tim_B_Sta), 
		Dd_UDC_Get_UDCR_Counter(gCt_Dd_Udc_Bin_Ch), 
		pdr
	));
	gCt_Dd_Udc_Tim_B_Sta = gCt_Dd_Udc_Tim_B_End;
}

/**
Callback function for UDCR observer.
*/
static VOID ct_dd_udc_observe_udcr(VOID)
{
	Ddim_Print((
		"[Observer] (Ch=%d) (UDCR=%d)\n", 
		gCt_Dd_Udc_Obs_ch, 
		Dd_UDC_Get_UDCR_Counter(gCt_Dd_Udc_Obs_ch)
	));
}

/**
Start 32bit timer
*/
static INT32 ct_dd_udc_start_timer32(UCHAR tmr_ch, ULONG interval, VP_CALLBACK cb_func)
{
	INT32 ercd;

	// Timer32 GIC.
	Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH5_INT,1,D_DD_GIC_PRI30,2);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH6_INT,1,D_DD_GIC_PRI30,2);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH7_INT,1,D_DD_GIC_PRI30,2);
	
	// Timer32 open
	ercd = Dd_TMR32_Open((UINT32)tmr_ch,D_DDIM_USER_SEM_WAIT_FEVR);
	if(ercd != D_DDIM_OK) {
		Ddim_Print(("ct_dd_udc_start_timer32(): Dd_TMR32_Open(%d) NG. ercd=%d\n", tmr_ch, ercd));
		return ercd;
	}

	// Timer32 setting
	ercd = Dd_TMR32_SetTimer(tmr_ch, interval, cb_func);
	if(ercd != D_DDIM_OK) {
		Ddim_Print(("ct_dd_udc_start_timer32(): Dd_TMR32_SetTimer(%d) NG. ercd=%d\n", tmr_ch, ercd));
		return ercd;
	}

	// Start timer
	ercd = Dd_TMR32_StartReload(tmr_ch);
	if(ercd != D_DDIM_OK) {
		Ddim_Print(("ct_dd_udc_start_timer32(): Dd_TMR32_StartReload(%d) NG. ercd=%d\n", tmr_ch, ercd));
		return ercd;
	}
	return D_DDIM_OK;
}

/**
Stop 32bit timer
*/
static VOID ct_dd_udc_stop_timer32(UCHAR tmr_ch)
{
	Dd_TMR32_Stop(tmr_ch);
	Dd_TMR32_Close(tmr_ch);
}

/**
Start virtual AIN input timer.
*/
static INT32 ct_dd_udc_start_virtual_ain(UCHAR udc_ch, UCHAR tmr_ch, ULONG tmr_interval)
{
	DDIM_USER_ER	ercd;

	gCt_Dd_Udc_Ain_Ch = udc_ch; // UDC channel number

	// Start timer
	ercd = (DDIM_USER_ER)ct_dd_udc_start_timer32(
		tmr_ch, 
		tmr_interval, 
		ct_dd_udc_ain_handler
	);
	if (ercd != D_DDIM_USER_E_OK) {
		// Timer execution error.
		Ddim_Print(("ct_dd_udc_start_virtual_ain(): ct_dd_udc_start_timer32 NG. ercd=%d\n", ercd));
		return((INT32)ercd);
	}
	// Save start time
	ercd = DDIM_User_Get_Tim(&gCt_Dd_Udc_Tim_A_Sta);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("ct_dd_udc_start_virtual_ain(): get_tim NG. ercd=%d\n", ercd));
		return((INT32)ercd);
	}
	return D_DDIM_OK;
}

/**
Stop virtual AIN input timer.
*/
static VOID ct_dd_udc_stop_virtual_ain(UCHAR tmr_ch)
{
	ct_dd_udc_stop_timer32(tmr_ch);
}

/**
Start virtual BIN input timer.
*/
static INT32 ct_dd_udc_start_virtual_bin(UCHAR udc_ch, UCHAR tmr_ch, ULONG tmr_interval)
{
	DDIM_USER_ER	ercd;

	// Check UDCTRG setting
	if ((Dd_Top_Get_PERSEL2_UDCTRG() == 1) && ((udc_ch == 2) || (udc_ch == 3))) {
		Ddim_Print(("ct_dd_udc_start_virtual_bin(): UDCTRG setting error. BIN can't use in ch2~3.\n"));
		return D_DD_UDC_INPUT_PARAM_ERROR;
	}

	gCt_Dd_Udc_Bin_Ch = udc_ch; // UDC channel number

	// Start timer
	ercd = (DDIM_USER_ER)ct_dd_udc_start_timer32(
		tmr_ch, 
		tmr_interval, 
		ct_dd_udc_bin_handler
	);
	if (ercd !=D_DDIM_USER_E_OK) {
		// Timer execution error.
		Ddim_Print(("ct_dd_udc_start_virtual_bin(): ct_dd_udc_start_timer32 NG. ercd=%d\n", ercd));
		return((INT32)ercd);
	}
	// Save start time
	ercd = DDIM_User_Get_Tim(&gCt_Dd_Udc_Tim_B_Sta);
	if (ercd != D_DDIM_USER_E_OK) {
		Ddim_Print(("ct_dd_udc_start_virtual_bin(): get_tim NG. ercd=%d\n", ercd));
		return((INT32)ercd);
	}
	return D_DDIM_OK;
}

/**
Stop virtual BIN input timer.
*/
static VOID ct_dd_udc_stop_virtual_bin(UCHAR tmr_ch)
{
	ct_dd_udc_stop_timer32(tmr_ch);
}

/**
Print T_DD_UDC_CTRL_CMN data.
*/
static VOID ct_dd_udc_print_crl_common(UCHAR ch, T_DD_UDC_CTRL_CMN const* const udc_crl)
{
	Ddim_Print(("---- T_DD_UDC_CTRL_CMN(Ch%d) ----\n", ch));
	Ddim_Print(("UDCR     = 0x%x\n", udc_crl->udcr		));
	Ddim_Print(("RCR      = 0x%x\n", udc_crl->rcr		));
	Ddim_Print(("CCR.CMS  = 0x%x\n", udc_crl->count_mode	));
	Ddim_Print(("CCR.UCRE = 0x%x\n", udc_crl->comp_clear	));
	Ddim_Print(("CCR.RLDE = 0x%x\n", udc_crl->reload		));
	Ddim_Print(("CCR.CGSC = 0x%x\n", udc_crl->zin_mode	));
	Ddim_Print(("CCR.CGE  = 0x%x\n", udc_crl->zin_edge	));
	Ddim_Print(("CSR.CITE = 0x%x\n", udc_crl->cmp_int	));
	Ddim_Print(("CSR.UDIE = 0x%x\n", udc_crl->under_over	));
	Ddim_Print(("CCR.CFIE = 0x%x\n", udc_crl->cnt_dir_int));
	Ddim_Print(("udc_crl.pCallBack = %p\n", udc_crl->pCallBack));
	Ddim_Print(("---------------------------------\n"));
}

/**
Print RCLK status.
*/
static VOID ct_dd_udc_print_clk_info(VOID)
{
	Ddim_Print(("==== Clock info ====\n"));
	Ddim_Print(("RCLK=%d\n", Dd_Top_Get_CLKSEL1_RCLK()));
//	Ddim_Print(("RCK=%d\n", Dd_Top_Get_CLKSTOP_RCK()));
	Ddim_Print(("====================\n"));
}

/**
Print triger status of all channels.
*/
static VOID ct_dd_udc_print_trg_info(VOID)
{
	INT32 i;

	Ddim_Print(("==== Trigger info ====\n"));
	for (i = 0; i < 6; i++) {
		Ddim_Print(("IO_PERI.UDC[%d].CSR.bit.__CSTR=%d\n", i, IO_PERI.UDC[i].CSR.bit.__CSTR));
	}
	Ddim_Print(("======================\n"));
}

/**
Print register value of all channels.
*/
static VOID ct_dd_udc_print_reg_info(VOID)
{
	INT32 i;

	Ddim_Print(("==== UDC reg info ====\n"));
	for (i = 0; i < 6; i++) {
		Ddim_Print(("IO_PERI.UDC[%d].CCR.word=%lu\n", i, IO_PERI.UDC[i].CCR.word));
		Ddim_Print(("IO_PERI.UDC[%d].CSR.word=%lu\n", i, IO_PERI.UDC[i].CSR.word));
		Ddim_Print(("IO_PERI.UDC[%d].UDCR=%lu\n", i, IO_PERI.UDC[i].UDCR.word));
	}
	Ddim_Print(("======================\n"));
}

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for UDC test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 */
VOID Ct_Dd_Udc_Main( INT32 argc, CHAR** argv ){
	INT32				ercd;
	INT32				i;
	T_DD_UDC_CTRL_CMN	udc_crl;
	DD_UDC_CALLBACK		callback;
	UCHAR				ch;
	UCHAR				tmr_ain_ch;
	UCHAR				tmr_bin_ch;
	BOOL				is_timer_mode   = FALSE;
	BOOL				is_up_down_mode = FALSE;
	UCHAR				prescaler;
	UCHAR				edge;
	ULONG				usec;
	UCHAR				peri_clock;

	memset(&udc_crl, 0, sizeof(T_DD_UDC_CTRL_CMN));

	if (strcmp((const char *)argv[1], "init") == 0){
		// UDC initialize test 
		// [command] udc init
		Dd_UDC_Init();

		// General-purpose port setting for UDC test.
		for (i = 0; i < 6; i++) {
		// AIN
			ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_AIN, E_CT_DD_UDC_PORT_KIND_EPCR, 0);
			ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_AIN, E_CT_DD_UDC_PORT_KIND_DDR, 0);
			ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_AIN, E_CT_DD_UDC_PORT_KIND_PDR, 0);

			// BIN
			ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_BIN, E_CT_DD_UDC_PORT_KIND_EPCR, 0);
			ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_BIN, E_CT_DD_UDC_PORT_KIND_DDR, 0);
			ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_BIN, E_CT_DD_UDC_PORT_KIND_PDR, 0);

			// ZIN
			if ((i == D_DD_UDC_CH0) || (i == D_DD_UDC_CH1)) {
				ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_ZIN, E_CT_DD_UDC_PORT_KIND_EPCR, 0);
				ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_ZIN, E_CT_DD_UDC_PORT_KIND_DDR, 0);
				ct_dd_udc_set_port(i, E_CT_DD_UDC_TRG_KIND_ZIN, E_CT_DD_UDC_PORT_KIND_PDR, 0);
			}
		}

		ct_dd_udc_print_clk_info();
		ct_dd_udc_print_trg_info();
		ct_dd_udc_print_reg_info();

		// GIC set (ch0-5)
		Dd_GIC_Ctrl(E_DD_GIC_INTID_UPDOWN_COUNTER_CH0_INT,1,D_DD_GIC_PRI30,2);
		Dd_GIC_Ctrl(E_DD_GIC_INTID_UPDOWN_COUNTER_CH1_INT,1,D_DD_GIC_PRI30,2);
		Dd_GIC_Ctrl(E_DD_GIC_INTID_UPDOWN_COUNTER_CH2_INT,1,D_DD_GIC_PRI30,2);
		Dd_GIC_Ctrl(E_DD_GIC_INTID_UPDOWN_COUNTER_CH3_INT,1,D_DD_GIC_PRI30,2);
		Dd_GIC_Ctrl(E_DD_GIC_INTID_UPDOWN_COUNTER_CH4_INT,1,D_DD_GIC_PRI30,2);
		Dd_GIC_Ctrl(E_DD_GIC_INTID_UPDOWN_COUNTER_CH5_INT,1,D_DD_GIC_PRI30,2);

		Ddim_Print(("Dd_UDC_Init OK.\n"));
	}
	else if(strcmp((const char *)argv[1], "open") == 0) {
		// UDC open
		// [command] udc open P1
		//           P1: Channel number(0~3)
		if(argc >= 3) {
			ercd = Dd_UDC_Open((atoi((const char *)argv[2])),D_DDIM_USER_SEM_WAIT_POL);
			if(ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Open NG. ercd=%d\n", ercd));
			}
			else {
				Ddim_Print(("Dd_UDC_Open OK.\n"));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "close") == 0) {
		// UDC close
		// [command] udc close P1
		//           P1: Channel number(0~3)
		if(argc >= 3) {
			ercd = Dd_UDC_Close(atoi((const char *)argv[2]));
			if(ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Close NG. ercd=%d\n", ercd));
			}
			else {
				Ddim_Print(("Dd_UDC_Close OK.\n"));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "start") == 0) {
		// UDC start
		// [command] udc start P1 P2 P3 P4 P5 P6 P7 P8 P9 P11 (P12)
		//           P1 : Mode("timer", "up_down" "phase2" "phase4")					@CCR.CMS
		//           P2 : Channel number(0~3)
		//           P3 : UDCR															@UDCR
		//           P4 : RCR															@RCR
		//           P5 : Compare clear enable(0:Disable, 1:Enable)						@CCR.UCRE
		//           P6 : Reload enable(0:Disable, 1:Enable)							@CCR.RLDE
		//           P7 : Function of terminal ZIN.(0:Clear, 1:Gate)					@CCR.CGSC
		//           P8 : Detect edge of terminal ZIN.(0:Not, 1:Fall/Low, 2:Rise/High)	@CCR.CGE
		//           P9 : Compare interrupt enable(0:Disable, 1:Enable)					@CSR.CITE
		//           P10: Under/Over-flow interrupt enable(0:Disable, 1:Enable)			@CSR.UDIE
		//           P11: Count direction interrupt enable(0:Disable, 1:Enable)			@CSR.CFIE
		// (Timer)   P12: Built-in prescaler.(0:2 Clock cycle, 1:8 Clock cycle)			@CCR.CLKS
		// (Up/Down) P12: Count clock edge(0:Not, 1:Fall, 2:Rise, 3:Both)				@CCR.CES
		// (Phase)   P12: Not used.
		if(argc >= 13) {
			// Set common parameter
			ch                  = atoi((const char *)argv[3]);
			udc_crl.udcr        = atoi((const char *)argv[4]);
			udc_crl.rcr         = atoi((const char *)argv[5]);
			udc_crl.comp_clear  = atoi((const char *)argv[6]);
			udc_crl.reload      = atoi((const char *)argv[7]);
			udc_crl.zin_mode    = atoi((const char *)argv[8]);
			udc_crl.zin_edge    = atoi((const char *)argv[9]);
			udc_crl.cmp_int     = atoi((const char *)argv[10]);
			udc_crl.under_over  = atoi((const char *)argv[11]);
			udc_crl.cnt_dir_int = atoi((const char *)argv[12]);
			udc_crl.pCallBack   = ct_dd_udc_callback;

			if (strcmp((const char *)argv[2], "timer") == 0) {
				// Timer mode
				Ddim_Print(("Timer mode.\n"));
				if(argc < 14) {
					Ddim_Print(("Command parameter error.\n"));
					return;
				}
				udc_crl.count_mode = D_DD_UDC_CMS_TIMER;
				is_timer_mode      = TRUE;
			}
			else if (strcmp((const char *)argv[2], "up_down") == 0) {
				// Up/Down count mode
				Ddim_Print(("Up/Down count mode.\n"));
				if(argc < 14) {
					Ddim_Print(("Command parameter error.\n"));
					return;
				}
				udc_crl.count_mode = D_DD_UDC_CMS_UP_DOWN;
				is_up_down_mode    = TRUE;
			}
			else if (strcmp((const char *)argv[2], "phase2") == 0) {
				// Phase lag count mode 2
				Ddim_Print(("Phase lag count mode 2 mode.\n"));
				udc_crl.count_mode = D_DD_UDC_CMS_PHASE_LAG_2;

				// Preparation for test of phase lag count mode. (Initial state is Low.)
				ct_dd_udc_set_port(ch, E_CT_DD_UDC_TRG_KIND_AIN, E_CT_DD_UDC_PORT_KIND_PDR, 0);	// AIN -> Low
				ct_dd_udc_set_port(ch, E_CT_DD_UDC_TRG_KIND_BIN, E_CT_DD_UDC_PORT_KIND_PDR, 0);	// BIN -> Low
			}
			else if (strcmp((const char *)argv[2], "phase4") == 0) {
				// Phase lag count mode 4
				Ddim_Print(("Phase lag count mode 4 mode.\n"));
				udc_crl.count_mode = D_DD_UDC_CMS_PHASE_LAG_4;

				// Preparation for test of phase lag count mode. (Initial state is Low.)
				ct_dd_udc_set_port(ch, E_CT_DD_UDC_TRG_KIND_AIN, E_CT_DD_UDC_PORT_KIND_PDR, 0);	// AIN -> Low
				ct_dd_udc_set_port(ch, E_CT_DD_UDC_TRG_KIND_BIN, E_CT_DD_UDC_PORT_KIND_PDR, 0);	// BIN -> Low
			}
			else {
				// Command error
				Ddim_Print(("Command parameter error.\n"));
				return;
			}

			// Display settings berore Dd_UDC_Ctrl_Common
			Ddim_Print(("Setting before write\n"));
			ct_dd_udc_print_crl_common(ch, &udc_crl);

			// Set execution parameter
			ercd = Dd_UDC_Ctrl_Common(ch, &udc_crl);
			if(ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Ctrl_Common error. ercd=%d\n", ercd));
				return;
			}
			else {
				Ddim_Print(("Dd_UDC_Ctrl_Common OK.\n"));
			}

			// Display settings after Dd_UDC_Ctrl_Common
			Ddim_Print(("Setting after write\n"));
			ercd = Dd_UDC_Get_Ctrl_Common(ch, &udc_crl);
			if(ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Get_Ctrl_Common NG. ercd=%d\n", ercd));
				return;
			}
			ct_dd_udc_print_crl_common(ch, &udc_crl);

			if (is_timer_mode == TRUE) {
				ercd = Dd_UDC_Ctrl_Timer(ch, atoi((const char *)argv[13]));
				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Ctrl_Timer error. ercd=%d\n", ercd));
					return;
				}
				else {
					Ddim_Print(("Dd_UDC_Ctrl_Timer OK. prescaler=%d\n", atoi((const char *)argv[13])));
				}
			}
			else if(is_up_down_mode == TRUE) {
				ercd = Dd_UDC_Ctrl_Up_Down(ch, atoi((const char *)argv[13]));
				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Ctrl_Up_Down error. ercd=%d\n", ercd));
					return;
				}
				else {
					Ddim_Print(("Dd_UDC_Ctrl_Up_Down OK. edge=%d\n", atoi((const char *)argv[13])));
				}
			}
			else {
				// DO NOTHING
			}

			// UDC start
			ercd = Dd_UDC_Start(ch);
			if(ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Start(%d) NG. ercd=%d\n", ch, ercd));
			}
			else {
				ct_dd_udc_print_trg_info();
				Ddim_Print(("Dd_UDC_Start(%d) OK.\n", ch));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "stop") == 0) {
		// UDC stop
		// [command] udc stop P1
		//           P1 : Channel number(0~3)
		if(argc >= 3) {
			ch = atoi((const char *)argv[2]);
			ercd = Dd_UDC_Stop(ch);
			if(ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Stop(%d) NG. ercd=%d\n", ch, ercd));
			}
			else {
				ct_dd_udc_print_trg_info();
				Ddim_Print(("Dd_UDC_Stop(%d) OK.\n", ch));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "get_crl") == 0) {
		// Get control data
		// [command] udc get_crl P1 P2
		//           P1 : Kind of control data("common", "timer" "up_down")
		//           P2 : Channel number(0~3)
		if(argc >= 4) {
			ch = atoi((const char *)argv[3]);

			if (strcmp((const char *)argv[2], "common") == 0) {
				// Get common control data
				ercd = Dd_UDC_Get_Ctrl_Common(ch, &udc_crl);
				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Get_Ctrl_Common NG. ercd=%d\n", ercd));
					return;
				}
				Ddim_Print(("---- Dd_UDC_Get_Ctrl_Common(Ch%d) ----\n", ch));
				ct_dd_udc_print_crl_common(ch, &udc_crl);
				Ddim_Print(("Dd_UDC_Get_Ctrl_Common OK.\n"));
			}
			else if (strcmp((const char *)argv[2], "timer") == 0) {
				// Get timer control data
				ercd = Dd_UDC_Get_Ctrl_Timer(ch, &prescaler);
				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Get_Ctrl_Timer NG. ercd=%d\n", ercd));
					return;
				}
				Ddim_Print(("---- Dd_UDC_Get_Ctrl_Timer(Ch%d) ----\n", ch));
				Ddim_Print(("CCR.CLKS = 0x%x\n", prescaler));
				Ddim_Print(("Dd_UDC_Get_Ctrl_Timer OK.\n"));
			}
			else if (strcmp((const char *)argv[2], "up_down") == 0) {
				// Get up/donw control data
				ercd = Dd_UDC_Get_Ctrl_Up_Down(ch, &edge);
				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Get_Ctrl_Up_Down NG. ercd=%d\n", ercd));
					return;
				}
				Ddim_Print(("---- Dd_UDC_Get_Ctrl_Up_Down(Ch%d) ----\n", ch));
				Ddim_Print(("CCR.CES  = 0x%x\n", edge));
				Ddim_Print(("Dd_UDC_Get_Ctrl_Up_Down OK.\n"));
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "cb") == 0) {
		if (strcmp((const char *)argv[2], "set") == 0) {
			// Set callback function pointer
			// [command] udc cb set P1 P2
			//           P1 : Channel number(0~3)
			//           P2 : Function selection
			if(argc >= 5) {
				ch = atoi((const char *)argv[3]);

				// Select callback function
				switch (atoi((const char *)argv[4])) {
					case 0:
						// Main function
						callback = ct_dd_udc_callback;
						break;

					default:
						// Sub function
						callback = ct_dd_udc_callback_sub;
						break;
				}
				Dd_UDC_Set_CallBack(ch, callback);
				Ddim_Print(("---- Dd_UDC_Set_CallBack(Ch%d) ----\n", ch));
				Ddim_Print(("callback = %p\n", callback));
				Ddim_Print(("Dd_UDC_Set_CallBack OK.\n"));
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "rcr") == 0) {
		if (strcmp((const char *)argv[2], "set") == 0) {
			// Set RCR register value
			// [command] udc rcr set P1 P2
			//           P1 : Channel number(0~3)
			//           P2 : RCR
			if(argc >= 5) {
				ch = atoi((const char *)argv[3]);

				ercd = Dd_UDC_Set_RCR_Counter(ch, atoi((const char *)argv[4]));
				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Set_RCR_Counter NG. ercd=%d\n", ercd));
				}
				else {
					Ddim_Print(("Dd_UDC_Set_RCR_Counter OK.\n"));
				}
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "udcr") == 0) {
		if (strcmp((const char *)argv[2], "get") == 0) {
			// Get UDCR register value
			// [command] udc udcr get P1
			//           P1 : Channel number(0~3)
			if(argc >= 4) {
				ch = atoi((const char *)argv[3]);
				Ddim_Print(("---- Dd_UDC_Get_UDCR_Counter(Ch%d) ----\n", ch));
				Ddim_Print(("UDCR = 0x%x\n", Dd_UDC_Get_UDCR_Counter(ch)));
				Ddim_Print(("Dd_UDC_Get_UDCR_Counter OK.\n"));
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "set") == 0) {
			// Set UDCR register value
			// [command] udc udcr set P1 P2
			//           P1 : Channel number(0~3)
			//           P2 : UDCR
			if(argc >= 5) {
				ch = atoi((const char *)argv[3]);

				ercd = Dd_UDC_Set_UDCR_Counter(ch, atoi((const char *)argv[4]));
				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Set_UDCR_Counter NG. ercd=%d\n", ercd));
				}
				else {
					Ddim_Print(("Dd_UDC_Set_UDCR_Counter OK.\n"));
				}
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "clear") == 0) {
			// Clear UDCR register value
			// [command] udc udcr clear P1
			//           P1 : Channel number(0~3)
			if(argc >= 4) {
				ch = atoi((const char *)argv[3]);

				Ddim_Print(("---- Dd_UDC_Force_Clear_UDCR(Ch%d) ----\n", ch));
				Ddim_Print(("Before UDCR = 0x%x\n", Dd_UDC_Get_UDCR_Counter(ch)));
				ercd = Dd_UDC_Force_Clear_UDCR(ch);
				Ddim_Print(("After UDCR = 0x%x\n", Dd_UDC_Get_UDCR_Counter(ch)));
				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Force_Clear_UDCR NG. ercd=%d\n", ercd));
				}
				else {
					Ddim_Print(("Dd_UDC_Force_Clear_UDCR OK.\n"));
				}
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "mode") == 0) {
		if (strcmp((const char *)argv[2], "set") == 0) {
			// Set count mode
			// [command] udc mode set P1 P2
			//           P1 : Channel number(0~3)
			//           P2 : Count mode(0~3)
			if(argc >= 5) {
				ch = atoi((const char *)argv[3]);
				Ddim_Print(("Before set: IO_PERI.UDC[%d].CCR.bit.__CMS=%d\n", ch, IO_PERI.UDC[ch].CCR.bit.__CMS));
				ercd = Dd_UDC_Set_Count_Mode(ch, atoi((const char *)argv[4]));
				Ddim_Print(("After set : IO_PERI.UDC[%d].CCR.bit.__CMS=%d\n", ch, IO_PERI.UDC[ch].CCR.bit.__CMS));

				if(ercd != D_DDIM_OK) {
					Ddim_Print(("Dd_UDC_Set_Count_Mode NG.\n"));
				}
				else {
					Ddim_Print(("Dd_UDC_Set_Count_Mode OK.\n"));
				}
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "port") == 0) {
		// Virtual input control(AIN, BIN, ZIN)
		tmr_ain_ch = D_CT_DD_UDC_TMR_AIN_CH;
		tmr_bin_ch = D_CT_DD_UDC_TMR_BIN_CH;

		if (strcmp((const char *)argv[2], "ph_lag") == 0) {
			// Virtual input for phase lag count mode.
			if (strcmp((const char *)argv[3], "start") == 0) {
				/*
				  Start virtual AIN/BIN input for phase lag count mode.
							+----+    +----+    +----+   
							|    |    |    |    |    |   
					AIN ----+    +----+    +----+    +---
							  +----+    +----+    +----+ 
							  |    |    |    |    |    | 
					BIN ------+    +----+    +----+    +-

				  [command] udc port ph_lag start P1 P2 P3 P4
							P1 : Channel number of UDC(0~3)
							P2 : AIN interval(msec)
							P3 : BIN interval(msec)
							P4 : Gap of start timing.(msec)
				*/
				if (argc >= 8) {
					ch = atoi((const char *)argv[4]); // UDC channel number

					// Start virtual AIN input.
					ercd = ct_dd_udc_start_virtual_ain(
						ch,										// UDC channel number
						tmr_ain_ch,								// Timer channel number
						(atoi((const char *)argv[5]) * 1000)	// Interval(usec)
					);
					if (ercd != D_DDIM_OK) {
						Ddim_Print(("ct_dd_udc_start_virtual_ain NG. ercd=%d\n", ercd));
						return;
					}
					ct_dd_udc_set_port(ch, E_CT_DD_UDC_TRG_KIND_AIN, E_CT_DD_UDC_PORT_KIND_PDR, 1);	// AIN -> High
					Ddim_Print((
						"[AIN] (Ch=%d) (TIM=0) (UDCR=%d) (PDR=1)\n",
						gCt_Dd_Udc_Ain_Ch,
						Dd_UDC_Get_UDCR_Counter(gCt_Dd_Udc_Ain_Ch)));

					// Wait gap time
					DDIM_User_Dly_Tsk((DDIM_USER_RELTIM)atoi((const char *)argv[7]));

					// Start virtual BIN input.
					ercd = ct_dd_udc_start_virtual_bin(
						ch,										// UDC channel number
						tmr_bin_ch,								// Timer channel number
						(atoi((const char *)argv[6]) * 1000)	// Interval(usec)
					);
					if (ercd != D_DDIM_OK) {
						Ddim_Print(("ct_dd_udc_start_virtual_bin NG. ercd=%d\n", ercd));
						return;
					}
					ct_dd_udc_set_port(ch, E_CT_DD_UDC_TRG_KIND_BIN, E_CT_DD_UDC_PORT_KIND_PDR, 1);	// BIN -> High
					Ddim_Print((
						"[BIN] (Ch=%d) (TIM=0) (UDCR=%d) (PDR=1)\n",
						gCt_Dd_Udc_Bin_Ch,
						Dd_UDC_Get_UDCR_Counter(gCt_Dd_Udc_Bin_Ch)));
				}
				else {
					Ddim_Print(("Command parameter error.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "stop") == 0) {
				// Stop virtual AIN/BIN input for phase lag count mode.
				// [command] udc port ph_lag stop

				// Stop AIN/BIN
				ct_dd_udc_stop_virtual_ain(tmr_ain_ch);
				ct_dd_udc_stop_virtual_bin(tmr_bin_ch);
				Ddim_Print(("Stop virtual AIN/BIN OK.\n"));
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "ain") == 0) {
			// Virtual input AIN
			if (strcmp((const char *)argv[3], "start") == 0) {
				/*
				  Start virtual AIN input
							+----+    +----+    +----+   
							|    |    |    |    |    |   
					AIN ----+    +----+    +----+    +---

				  [command] udc port ain start P1 P2
							P1 : Channel number of UDC(0~3)
							P2 : AIN interval(msec)
				*/
				if (argc >= 6) {
					ercd = ct_dd_udc_start_virtual_ain(
						atoi((const char *)argv[4]),			// UDC channel number
						tmr_ain_ch,										// Timer channel number
						(atoi((const char *)argv[5]) * 1000)	// Interval(usec)
					);
					if (ercd != D_DDIM_OK) {
						Ddim_Print(("ct_dd_udc_start_virtual_ain NG. ercd=%d\n", ercd));
						return;
					}
					else {
						Ddim_Print(("Start virtual AIN OK.\n"));
					}
				}
				else {
					Ddim_Print(("Command parameter error.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "stop") == 0) {
				// Stop virtual AIN input.
				// [command] udc port ain stop
				ct_dd_udc_stop_virtual_ain(tmr_ain_ch);
				Ddim_Print(("Stop virtual AIN OK.\n"));
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "bin") == 0) {
			// Virtual input BIN
			if (strcmp((const char *)argv[3], "start") == 0) {
				/*
				  Start virtual BIN input
							+----+    +----+    +----+   
							|    |    |    |    |    |   
					BIN ----+    +----+    +----+    +---

				  [command] udc port bin start P1 P2
							P1 : Channel number of UDC(0~3)
							P2 : BIN interval(msec)
				*/
				if (argc >= 6) {
					ercd = ct_dd_udc_start_virtual_bin(
						atoi((const char *)argv[4]),			// UDC channel number
						tmr_bin_ch,										// Timer channel number
						(atoi((const char *)argv[5]) * 1000)	// Interval(usec)
					);
					if (ercd != D_DDIM_OK) {
						Ddim_Print(("ct_dd_udc_start_virtual_bin NG. ercd=%d\n", ercd));
						return;
					}
					else {
						Ddim_Print(("Start virtual BIN OK.\n"));
					}
				}
				else {
					Ddim_Print(("Command parameter error.\n"));
				}
			}
			else if (strcmp((const char *)argv[3], "stop") == 0) {
				// Stop virtual BIN input.
				// [command] udc port bin stop
				ct_dd_udc_stop_virtual_bin(tmr_bin_ch);
				Ddim_Print(("Stop virtual BIN OK.\n"));
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if (strcmp((const char *)argv[2], "zin") == 0) {
			// Change ZIN level
			// [command] udc port zin P1 P2
			//           P1 : Channel number(0~1)
			//           P2 : Level(h: high, l:low)
			if (argc >= 5) {
				ch = atoi((const char *)argv[3]); // UDC channel number

				// Check UDCTRG setting
				if ((Dd_Top_Get_PERSEL2_UDCTRG() == 0) || 
					((Dd_Top_Get_PERSEL2_UDCTRG() == 1) && (ch != 0) && (ch != 1))) {
					Ddim_Print(("UDCTRG setting error. ZIN can't use.\n"));
					return;
				}

				if (strcmp((const char *)argv[4], "h") == 0) {
					// ZIN -> High
					ct_dd_udc_set_port(ch, E_CT_DD_UDC_TRG_KIND_ZIN, E_CT_DD_UDC_PORT_KIND_PDR, 1);
					Ddim_Print(("ZIN -> High.\n"));
				}
				else if (strcmp((const char *)argv[4], "l") == 0) {
					// ZIN -> Low
					ct_dd_udc_set_port(ch, E_CT_DD_UDC_TRG_KIND_ZIN, E_CT_DD_UDC_PORT_KIND_PDR, 0);
					Ddim_Print(("ZIN -> Low.\n"));
				}
				else {
					Ddim_Print(("Command parameter error.\n"));
				}
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "observe") == 0) {
		// Setting of UDCR observer
		if (strcmp((const char *)argv[2], "start") == 0) {
			if(argc >= 5) {
				Ddim_Print(("[observe start]\n"));
				// Start cycle handler for observe UDCR register.
				// [command] udc observe start P1 P2
				//           P1 : Channel number(0~3)
				//           P2 : Cycle time(msec)
				gCt_Dd_Udc_Obs_ch = atoi((const char *)argv[3]); // UDC channel number

				// Start timer
				ercd = ct_dd_udc_start_timer32(
					D_CT_DD_UDC_TMR_OBS_CH, 
					(atoi((const char *)argv[4]) * 1000), 
					ct_dd_udc_observe_udcr
				);
				if (ercd != D_DDIM_OK) {
					// Timer execution error.
					Ddim_Print(("ct_dd_udc_start_timer32 NG. ercd=%d\n", ercd));
					return;
				}
				Ddim_Print(("Start Ch%d UDCR observer.\n", gCt_Dd_Udc_Obs_ch));
			}
			else {
				Ddim_Print(("Command parameter error.\n"));
				return;
			}
		}
		else if (strcmp((const char *)argv[2], "stop") == 0) {
			// Stop cycle handler.
			// [command] udc observe stop
			ct_dd_udc_stop_timer32(D_CT_DD_UDC_TMR_OBS_CH);
			Ddim_Print(("Stop UDCR observer.\n"));
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "udctrg") == 0) {
		if (strcmp((const char *)argv[2], "set") == 0) {
			// Set PERSEL2.UDCTRG
			// [command] udc udctrg set P1
			//           P1 : UpDownCounter Trigger select
			//                0: All channels can use phase lag, can't use zin. 
			//                1: ch0~1 can use phase lag and zin.
			//                   ch2~3 can't use phase lag, zin and up count.
			UINT32 udctrg;
			udctrg = atoi((const char *)argv[3]);

			switch (udctrg) {
				case 0: // FALL THROUGH 
				case 1:
					Dd_Top_Set_PERSEL2_UDCTRG(udctrg);
					break;

				default:
					Ddim_Print(("Command parameter error. UDCTRG must be 0~1\n"));
					return;
			}
		}
		else if (strcmp((const char *)argv[2], "get") == 0) {
			// Get PERSEL2.UDCTRG
			Ddim_Print(("IO_CHIPTOP.PERSEL2.bit.UDCTRG=%d\n", (UINT32)Dd_Top_Get_PERSEL2_UDCTRG()));
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "utility") == 0) {
		if (strcmp((const char *)argv[2], "start") == 0) {
			// Start utility function.
			// [command] udc utility start P1 P2
			//           P1 : Channel number(0~3)
			//           P2 : Count time(usec)

			//utility function
			Ddim_Print(("Start utility function.\n"));

			// Set UDC ch and count time
			ch = atoi((const char *)argv[3]);
			usec = atol((const char *)argv[4]);

			// Get peripheral clock
			peri_clock = Dd_Top_Get_CLKSEL1_RCLK();
			Ddim_Print(("Top:CLKSEL-A.RCLK = 0x%x\n",peri_clock));

			// Channel error check
			if (ch >= 6) {
				Ddim_Print(("Command parameter error.\n"));
				return;
			}

			// Set utility function parameter
			ercd = Dd_UDC_Write_Timer_Mode(ch, usec, ct_dd_udc_utility_callback);
			if (ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Write_Timer_Mode NG. ercd=%d\n",ercd));
				return;
			}

			// Check dividing rate
			ercd = Dd_UDC_Get_Ctrl_Timer(ch, &prescaler);
			if (ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Get_Ctrl_Timer NG. ercd=%d\n",ercd));
				return;
			}
			else {
				Ddim_Print(("CLKS = %d\n",prescaler));
			}

			ercd = DDIM_User_Get_Tim(&gCt_Dd_Udc_Tim_Util_Sta);
			if (ercd != D_DDIM_USER_E_OK) {
				Ddim_Print(("UDC utility function: get_tim NG. ercd=%d\n", ercd));
				return;
			}

			// Check utility function parameter
			ercd = Dd_UDC_Get_Ctrl_Common(ch,&udc_crl);
			if (ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Get_Ctrl_Common NG. ercd=%d\n",ercd));
				return;
			}
			ct_dd_udc_print_crl_common(ch,&udc_crl);

			// Start utility function
			ercd = Dd_UDC_Start(ch);
			if (ercd != D_DDIM_OK) {
				Ddim_Print(("Dd_UDC_Start NG. ercd=%d\n",ercd));
				return;
			}
		}
		else {
			Ddim_Print(("Command parameter error.\n"));
		}
	}
	else if(strcmp((const char *)argv[1], "err") == 0) {
		// Error processing test
		// [command] udc err
		if(strcmp((const char *)argv[2], "ctrl_common") == 0){
			// [Dd_UDC_Ctrl_Common]
			if(strcmp((const char *)argv[3], "p1") == 0){
				//   - Channel number is illegal.
				ercd = Dd_UDC_Ctrl_Common(6, &udc_crl);
				Ddim_Print(("[Error Test] Dd_UDC_Ctrl_Common(): Channel number is illegal. ercd=%d\n", ercd));
			}
			else if(strcmp((const char *)argv[3], "p2") == 0){
				//   - udc_crl parameter is NULL.
				ercd = Dd_UDC_Ctrl_Common(0, NULL);
				Ddim_Print(("[Error Test] Dd_UDC_Ctrl_Common(): udc_crl parameter is NULL. ercd=%d\n", ercd));
			}
			else{
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if(strcmp((const char *)argv[2], "get_ctrl_common") == 0){
			// [Dd_UDC_Get_Ctrl_Common]
			if(strcmp((const char *)argv[3], "p1") == 0){
				//   - Channel number is illegal.
				ercd = Dd_UDC_Get_Ctrl_Common(6, &udc_crl);
				Ddim_Print(("[Error Test] Dd_UDC_Get_Ctrl_Common(): Channel number is illegal. ercd=%d\n", ercd));
			}
			else if(strcmp((const char *)argv[3], "p2") == 0){
				//   - udc_crl parameter is NULL.
				ercd = Dd_UDC_Get_Ctrl_Common(0, NULL);
				Ddim_Print(("[Error Test] Dd_UDC_Get_Ctrl_Common(): udc_crl parameter is NULL. ercd=%d\n", ercd));
			}
			else{
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if(strcmp((const char *)argv[2], "ctrl_timer") == 0){
			// [Dd_UDC_Ctrl_Timer]
			if(strcmp((const char *)argv[3], "p1") == 0){
				//   - Channel number is illegal.
				ercd = Dd_UDC_Ctrl_Timer(6, D_DD_UDC_CLKS_2_CLOCK);
				Ddim_Print(("[Error Test] Dd_UDC_Ctrl_Timer(): Channel number is illegal. ercd=%d\n", ercd));
			}
			else if(strcmp((const char *)argv[3], "p2") == 0){
				//   - Prescaler is illegal.
				ercd = Dd_UDC_Ctrl_Timer(D_DD_UDC_CH0, 2);
				Ddim_Print(("[Error Test] Dd_UDC_Ctrl_Timer(): Prescaler is illegal. ercd=%d\n", ercd));
			}
			else{
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if(strcmp((const char *)argv[2], "get_ctrl_timer") == 0){
			// [Dd_UDC_Get_Ctrl_Timer]
			if(strcmp((const char *)argv[3], "p1") == 0){
				//   - Channel number is illegal.
				ercd = Dd_UDC_Get_Ctrl_Timer(6, &prescaler);
				Ddim_Print(("[Error Test] Dd_UDC_Get_Ctrl_Timer(): Channel number is illegal. ercd=%d\n", ercd));
			}
			else if(strcmp((const char *)argv[3], "p2") == 0){
				//   - prescaler parameter is NULL.
				ercd = Dd_UDC_Get_Ctrl_Timer(0, NULL);
				Ddim_Print(("[Error Test] Dd_UDC_Get_Ctrl_Timer(): prescaler parameter is NULL. ercd=%d\n", ercd));
			}
			else{
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if(strcmp((const char *)argv[2], "ctrl_up_down") == 0){
			// [Dd_UDC_Ctrl_Up_Down]
			if(strcmp((const char *)argv[3], "p1") == 0){
				//   - Channel number is illegal.
				ercd = Dd_UDC_Ctrl_Up_Down(6, D_DD_UDC_CES_RISE);
				Ddim_Print(("[Error Test] Dd_UDC_Ctrl_Up_Down(): Channel number is illegal. ercd=%d\n", ercd));
			}
			else if(strcmp((const char *)argv[3], "p2") == 0){
				//   - Edge is illegal.
				ercd = Dd_UDC_Ctrl_Up_Down(D_DD_UDC_CH0, 4);
				Ddim_Print(("[Error Test] Dd_UDC_Ctrl_Up_Down(): Edge is illegal. ercd=%d\n", ercd));
			}
			else{
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if(strcmp((const char *)argv[2], "get_ctrl_up_down") == 0){
			// [Dd_UDC_Get_Ctrl_Up_Down]
			if(strcmp((const char *)argv[3], "p1") == 0){
				//   - Channel number is illegal.
				ercd = Dd_UDC_Get_Ctrl_Up_Down(6, &edge);
				Ddim_Print(("[Error Test] Dd_UDC_Get_Ctrl_Up_Down(): Channel number is illegal. ercd=%d\n", ercd));
			}
			else if(strcmp((const char *)argv[3], "p2") == 0){
				//   - edge parameter is NULL.
				ercd = Dd_UDC_Get_Ctrl_Up_Down(0, NULL);
				Ddim_Print(("[Error Test] Dd_UDC_Get_Ctrl_Up_Down(): edge parameter is NULL. ercd=%d\n", ercd));
			}
			else{
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if(strcmp((const char *)argv[2], "start") == 0){
			// [Dd_UDC_Start]
			//   - Channel number is illegal.
			ercd = Dd_UDC_Start(6);
			Ddim_Print(("[Error Test] Dd_UDC_Start(): Channel number is illegal. ercd=%d\n", ercd));
		}
		else if(strcmp((const char *)argv[2], "stop") == 0){
			// [Dd_UDC_Stop]
			//   - Channel number is illegal.
			ercd = Dd_UDC_Stop(6);
			Ddim_Print(("[Error Test] Dd_UDC_Stop(): Channel number is illegal. ercd=%d\n", ercd));
		}
		else if(strcmp((const char *)argv[2], "callback") == 0){
			// [Dd_UDC_Set_CallBack]
			//   - Channel number is illegal.
			Dd_UDC_Set_CallBack(6, ct_dd_udc_callback);
		}
		else if(strcmp((const char *)argv[2], "set_rcr_counter") == 0){
			// [Dd_UDC_Set_RCR_Counter]
			//   - Channel number is illegal.
			ercd = Dd_UDC_Set_RCR_Counter(6, 10000);
			Ddim_Print(("[Error Test] Dd_UDC_Set_RCR_Counter(): Channel number is illegal. ercd=%d\n", ercd));
		}
		else if(strcmp((const char *)argv[2], "get_udcr_counter") == 0){
			// [Dd_UDC_Get_UDCR_Counter]
			//   - Channel number is illegal.
			ercd = Dd_UDC_Get_UDCR_Counter(6);
			Ddim_Print(("[Error Test] Dd_UDC_Get_UDCR_Counter(): Channel number is illegal. ercd=%d\n", ercd));
		}
		else if(strcmp((const char *)argv[2], "set_udcr_counter") == 0){
			// [Dd_UDC_Set_UDCR_Counter]
			//   - Channel number is illegal.
			ercd = Dd_UDC_Set_UDCR_Counter(6, 10000);
			Ddim_Print(("[Error Test] Dd_UDC_Set_UDCR_Counter(): Channel number is illegal. ercd=%d\n", ercd));
		}
		else if(strcmp((const char *)argv[2], "force_clear_udcr") == 0){
			// [Dd_UDC_Force_Clear_UDCR]
			//   - Channel number is illegal.
			ercd = Dd_UDC_Force_Clear_UDCR(6);
			Ddim_Print(("[Error Test] Dd_UDC_Force_Clear_UDCR(): Channel number is illegal. ercd=%d\n", ercd));
		}
		else if(strcmp((const char *)argv[2], "get_up_down_flg") == 0){
			// [Dd_UDC_Get_Up_Down_Flg]
			//   - Channel number is illegal.
			ercd = Dd_UDC_Get_Up_Down_Flg(6);
			Ddim_Print(("[Error Test] Dd_UDC_Get_Up_Down_Flg(): Channel number is illegal. ercd=%d\n", ercd));
		}
		else if(strcmp((const char *)argv[2], "set_count_mode") == 0){
			// [Dd_UDC_Set_Count_Mode]
			if(strcmp((const char *)argv[3], "p1") == 0){
				//   - Channel number is illegal.
				ercd = Dd_UDC_Set_Count_Mode(6, D_DD_UDC_CMS_TIMER);
				Ddim_Print(("[Error Test] Dd_UDC_Set_Count_Mode(): Channel number is illegal. ercd=%d\n", ercd));
			}
			else if(strcmp((const char *)argv[3], "p2") == 0){
				//   - Count mode is illegal.
				ercd = Dd_UDC_Set_Count_Mode(D_DD_UDC_CH0, 4);
				Ddim_Print(("[Error Test] Dd_UDC_Set_Count_Mode(): Count mode is illegal. ercd=%d\n", ercd));
			}
			else{
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else if(strcmp((const char *)argv[2], "write_timer_mode") == 0){
			// [Dd_UDC_Write_Timer_Mode]
			if(strcmp((const char *)argv[3], "p1") == 0){
				//   - Channel number is illegal.
				ercd = Dd_UDC_Write_Timer_Mode(6, 100, ct_dd_udc_utility_callback);
				Ddim_Print(("[Error Test] Dd_UDC_Write_Timer_Mode(): Channel number is illegal. ercd=%d\n", ercd));
			}
			else if(strcmp((const char *)argv[3], "p3") == 0){
				//   - timer_callback parameter is NULL.
				ercd = Dd_UDC_Write_Timer_Mode(D_DD_UDC_CH0, 100, NULL);
				Ddim_Print(("[Error Test] Dd_UDC_Write_Timer_Mode(): timer_callback parameter is NULL. ercd=%d\n", ercd));
			}
			else{
				Ddim_Print(("Command parameter error.\n"));
			}
		}
		else{
			Ddim_Print(("Command parameter error.\n"));
		}

	}
	else {
		Ddim_Print(("Command parameter error.\n"));
	}
}
