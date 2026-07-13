/**
 * @file		motor_tc78h670ftg.c
 * @brief 		motor driver for TC78H670FTG
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2020 Socionext Inc.</I></B>
 */

#include "dd_top.h"
#include "fj_peripheral.h"
#include "os_user_custom.h"
/*--------------------------------------------------------------------------*/
/* Definition				 												*/
/*--------------------------------------------------------------------------*/

/*-------------------------*/
/* STBY				P11    */
/* MODE0/UP_DW		P67    */
/* MODE1/SET_EN		P26    */
/* MODE2/CLK		P81    */
/* MODE3/CW_CCW		P82    */
/* EN				P12    */
/*                         */
/* ERR				P12    */
/* SW_DET1			P32    */
/* SW_DET2			P33    */
/*                         */
/* CLK = P81 -> PWM ch1    */
/*-------------------------*/


/*--------------------------------------------------------------------------*/
/* Static Variables			 												*/
/*--------------------------------------------------------------------------*/
static UINT32 n_duty=0;

/*--------------------------------------------------------------------------*/
/* Static Function			 												*/
/*--------------------------------------------------------------------------*/
void motor_start_step(UCHAR dir_cw, UINT32 steps);
void motor_start(UCHAR mode3, UCHAR mode2, UCHAR mode1, UCHAR mode0, UCHAR dir_cw);
void motor_stop();
void motor_dir_cw();
void motor_dir_ccw();
void motor_res_lower();
void motor_res_higher();
static VOID motor_cycle_cb( UINT32 status );
static VOID motor_duty_cb( UINT32 status );
static VOID motor_eint_cb( FJ_GPIO_PORT port );


void motor_start_step(UCHAR dir_cw, UINT32 steps)
{
	motor_start(1, 0, 0, 0, dir_cw);	// full step resolution
	n_duty = 0;
	while (n_duty < (steps*4))
		OS_User_Dly_Tsk(1);
	motor_stop();
}


// CLK-IN mode
void motor_start(UCHAR mode3, UCHAR mode2, UCHAR mode1, UCHAR mode0, UCHAR dir_cw)
{
	FJ_PWM_CTRL pwm_ctrl;
	FJ_EXT_INT_CONFIG config;

	Dd_Top_Set_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_P12, D_DD_TOP_GPIO_DIR_OUT );	// EN
	Dd_Top_Set_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_P11, D_DD_TOP_GPIO_DIR_OUT );	// STBY
	Dd_Top_Set_Gpio_Direction( E_DD_TOP_GPIO_P67, D_DD_TOP_GPIO_DIR_OUT );			// Mode0/UP-DW
	Dd_Top_Set_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_P26, D_DD_TOP_GPIO_DIR_OUT );	// Mode1/SET-EN
	Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_P81, D_DD_TOP_GPIO_FUNC_GPIO );			// Mode2/CLK
	Dd_Top_Set_Gpio_Direction( E_DD_TOP_GPIO_P81, D_DD_TOP_GPIO_DIR_OUT );
	Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_P82, D_DD_TOP_GPIO_FUNC_GPIO );			// Mode3/CW-CCW
	Dd_Top_Set_Gpio_Direction( E_DD_TOP_GPIO_P82, D_DD_TOP_GPIO_DIR_OUT );
	
	// ERR (in) low active
	//Dd_Top_Set_Gpio_CM0_Function( E_DD_TOP_GPIO_CM0_P16, D_DD_TOP_GPIO_FUNC_HWMODE );
	Dd_Top_Set_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_P16, D_DD_TOP_GPIO_DIR_IN );
	// SW_DET1 (in)
	//Dd_Top_Set_Gpio_CM0_Function( E_DD_TOP_GPIO_CM0_P32, D_DD_TOP_GPIO_FUNC_HWMODE );
	Dd_Top_Set_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_P32, D_DD_TOP_GPIO_DIR_IN );
	// SW_DET2 (in)
	//Dd_Top_Set_Gpio_CM0_Function( E_DD_TOP_GPIO_CM0_P33, D_DD_TOP_GPIO_FUNC_HWMODE );
	Dd_Top_Set_Gpio_CM0_Direction( E_DD_TOP_GPIO_CM0_P33, D_DD_TOP_GPIO_DIR_IN );


	
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P12, D_DD_TOP_GPIO_STATUS_LOW );		// EN = 0
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P11, D_DD_TOP_GPIO_STATUS_LOW );		// STBY = 0
	
	Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_P67, mode0 );									// set Mode0, Mode1, Mode2, Mode3
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P26, mode1 );
	Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_P81, mode2 );
	Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_P82, mode3 );
	
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P11, D_DD_TOP_GPIO_STATUS_HIGH );		// STBY = 1, set CLK-IN mode
	
	Dd_ARM_Wait_ns( 100000 );	// Mode setting Data hold time > 100us
	
	// Mode2/CLK
	Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl( E_DD_TOP_GPIO_P81, D_DD_TOP_GPIO_PUD_DOWN );
	Dd_Top_Set_Gpio_Pull_Up_Down_Enable( E_DD_TOP_GPIO_P81, D_DD_TOP_GPIO_PUD_ON );
	Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_P81, D_DD_TOP_GPIO_FUNC_HWMODE );
	
	pwm_ctrl.ch 			= FJ_PERI_CH_1;	// WPPG1, P81
	pwm_ctrl.mode			= FJ_PWM_MODE_CONTINUOUS;
	pwm_ctrl.polarity		= FJ_PWM_POLARITY_NORMAL;
	pwm_ctrl.restart_enable = FJ_PWM_RESTART_ENABLE;
	pwm_ctrl.pcallback1 	= motor_duty_cb;
	pwm_ctrl.pcallback2 	= motor_cycle_cb;
	FJ_PWM_Open( pwm_ctrl.ch );
	FJ_PWM_Set_By_Ratio( &pwm_ctrl, 1000, 50 );	// cycle= 1000Hz
	FJ_PWM_Start( pwm_ctrl.ch );
	
	Dd_ARM_Wait_ns( 100000 );	// wait 100us
	
	Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_P82, dir_cw );								// CW-CCW
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P12, D_DD_TOP_GPIO_STATUS_HIGH );		// EN = 1

	// EINT P70
	//Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_P70, D_DD_TOP_GPIO_FUNC_HWMODE );
	Dd_Top_Set_Gpio_Direction( E_DD_TOP_GPIO_P70, D_DD_TOP_GPIO_DIR_IN );
	config.int_priority = 10;
	config.trigger_type = FJ_EXT_INT_TRIGGER_TYPE_RISEEDGE;
	config.cpu_target = FJ_EXT_INT_TARGET_CPU_0;
	config.int_callback = motor_eint_cb;
	FJ_Ext_Interrupt_Set_Config(FJ_GPIO_P70, &config);
	FJ_Ext_Interrupt_Set_Enable(FJ_GPIO_P70, 1);
}

void motor_stop()
{
	FJ_PERI_CH ch = FJ_PERI_CH_1;
	
	FJ_PWM_Stop(ch);
	FJ_PWM_Close(ch);
	
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P12, D_DD_TOP_GPIO_STATUS_LOW );		// EN = 0
}

void motor_dir_cw()
{
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P12, D_DD_TOP_GPIO_STATUS_LOW );		// EN = 0
	Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_P82, D_DD_TOP_GPIO_STATUS_HIGH );				// CW-CCW = 1
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P12, D_DD_TOP_GPIO_STATUS_HIGH );		// EN = 1
}

void motor_dir_ccw()
{
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P12, D_DD_TOP_GPIO_STATUS_LOW );		// EN = 0
	Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_P82, D_DD_TOP_GPIO_STATUS_LOW );				// CW-CCW = 0
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P12, D_DD_TOP_GPIO_STATUS_HIGH );		// EN = 1
}

void motor_res_lower()
{
	Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_P67, D_DD_TOP_GPIO_STATUS_HIGH );				// UP-DW = 1
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P26, D_DD_TOP_GPIO_STATUS_HIGH );		// SET-EN = 1
	// SET-EN = 0 in motor_duty_cb()
}

void motor_res_higher()
{
	Dd_Top_Set_Gpio_Status( E_DD_TOP_GPIO_P67, D_DD_TOP_GPIO_STATUS_LOW );				// UP-DW = 0
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P26, D_DD_TOP_GPIO_STATUS_HIGH );		// SET-EN = 1
	// SET-EN = 0 in motor_duty_cb()
}

static VOID motor_cycle_cb( UINT32 status )
{
}

static VOID motor_duty_cb( UINT32 status )
{
	n_duty++;
	Dd_Top_Set_Gpio_CM0_Status( E_DD_TOP_GPIO_CM0_P26, D_DD_TOP_GPIO_STATUS_LOW );		// SET-EN = 0
}

static VOID motor_eint_cb( FJ_GPIO_PORT port )
{
	printf("port %d trigger EINT, motor stop\n", port);
	motor_stop();
}

