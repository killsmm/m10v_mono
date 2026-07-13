/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx477_imx183.h"
#include "camera_config.h"
#include "os_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_MSHUTTER_P	(E_DD_TOP_GPIO_PG7)
#define D_MSHUTTER_N	(E_DD_TOP_GPIO_PG6)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

const T_CAMERA_SENSOR_BASIC		IMX477_IMX183_0_BASIC = // IMX477
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_MAX, //E_CAMERA_SENSOR_INCLK_24MHz,
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};

const T_CAMERA_SENSOR_BASIC		IMX477_IMX183_1_BASIC = // IMX183
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_72MHz,
	.access			= E_CAMERA_ACCESS_SPI,
	.sensor_if		= E_CAMERA_SENSOR_IF_LVDS,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define M_SHUTTER_INIT()\
	Dd_Top_Set_Gpio_Function(D_MSHUTTER_P, D_DD_TOP_GPIO_FUNC_GPIO);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_HIGH);\
	Dd_Top_Set_Gpio_Direction(D_MSHUTTER_P, D_DD_TOP_GPIO_DIR_OUT);\
	Dd_Top_Set_Gpio_Function(D_MSHUTTER_N, D_DD_TOP_GPIO_FUNC_GPIO);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_HIGH);\
	Dd_Top_Set_Gpio_Direction(D_MSHUTTER_N, D_DD_TOP_GPIO_DIR_OUT);\

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX477_IMX183_Clock_Gpio_Init(VOID)
{
	INT32 sensor_id;

	// Clock ★平気？ sensor 全て同じにすべき？
	for( sensor_id = 0; sensor_id < CAMERA.sensor_num; sensor_id++){
		if( CAMERA.sensor[sensor_id].basic->input_clock != E_CAMERA_SENSOR_INCLK_MAX){
			switch( CAMERA.sensor[sensor_id].basic->input_clock )
			{
				case E_CAMERA_SENSOR_INCLK_24MHz:
					Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// 40MHz / (val+1)
					Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
					Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
					Dd_Top_Set_PLLCNT3_P00PLLDIV(60);	// * val
					Dd_Top_Set_PLLCNT3_P00POSTDIV(49);	// / (val+1)
					Dd_Top_Start_Pll00();				// PLL00 Start
					break;
				case E_CAMERA_SENSOR_INCLK_72MHz:
					Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// 40MHz / (val+1)
					Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
					Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
					Dd_Top_Set_PLLCNT3_P00PLLDIV(90);	// * val
					Dd_Top_Set_PLLCNT3_P00POSTDIV(24);	// / (val+1)
					Dd_Top_Start_Pll00();				// PLL00 Start
					break;
				case E_CAMERA_SENSOR_INCLK_37_125MHz:
					// 40 / (3+1) * (29*10+7)/10 / (7 + 1) = 37.125
					Dd_Top_Set_PLLCNT3_P00PREDIV(3);	// 40MHz / (val+1)
					Dd_Top_Set_PLLCNT4_P00FNUM(7);		//
					Dd_Top_Set_PLLCNT5_P00FDEN(10); 	//
					Dd_Top_Set_PLLCNT3_P00PLLDIV(29);	// * val
					Dd_Top_Set_PLLCNT3_P00POSTDIV(7);	// / (val+1)
					Dd_Top_Start_Pll00();				// PLL00 Start
					break;
				default:
					return E_CAMERA_ERR_NG;
					//break;
			}
			break;
		}
	}
	
	// Sensor Initialization ★電源立上げシーケンスは平気？？？
	for( sensor_id = 0; sensor_id < CAMERA.sensor_num; sensor_id++){
		
		// SPI?
		if( CAMERA.sensor[sensor_id].basic->access == E_CAMERA_ACCESS_SPI){
			switch(CAMERA.sensor[sensor_id].spi_ch){
				case FJ_PERI_CH_0:
					// SP0SCK Pull Up
					Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PD2, 1 );
					Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PD2, 1 );
				break;
				case FJ_PERI_CH_1:
					// SP1SCK Pull Up
					Dd_Top_Set_PUDCNT_SP1SCKUDC( 1 );
					Dd_Top_Set_PUDCNT_SP1SCKPE( 1 );
				break;
				case FJ_PERI_CH_2:
					// SP2SCK Pull Up
					Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PB0, 1 );
					Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PB0, 1 );
				break;
				default:
					return E_CAMERA_ERR_NG;
			}
		}

		// Clock
		switch(CAMERA.sensor[sensor_id].port_inck){
			case E_DD_TOP_GPIO_PG2:
				FJ_Top_Set_Clock_Select_AP0(FJ_TOP_SNAPCLKO_CLKSEL_PLL0);
				Dd_Top_Set_CLKSTOP4_APCK0(0);
				break;
			case E_DD_TOP_GPIO_PG5:
				FJ_Top_Set_Clock_Select_AP1(FJ_TOP_SNAPCLKO_CLKSEL_PLL0);
				Dd_Top_Set_CLKSTOP4_APCK1(0);
				break;
			case E_DD_TOP_GPIO_PH0:
				FJ_Top_Set_Clock_Select_AP2(FJ_TOP_SNAPCLKO_CLKSEL_PLL0);
				Dd_Top_Set_CLKSTOP4_APCK2(0);
				break;
			case E_DD_TOP_GPIO_PH3:
				FJ_Top_Set_Clock_Select_AP3(FJ_TOP_SNAPCLKO_CLKSEL_PLL0);
				Dd_Top_Set_CLKSTOP4_APCK3(0);
				break;
			default:
				break;
		}

		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_inck, 1 );	// CLK from Sensor

		// Release Reset
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xclr, 0 );	// use as GPIO
		Dd_Top_Set_Gpio_Direction( CAMERA.sensor[sensor_id].port_xclr, 1 );	// output
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 );	// XCLR
		OS_User_Dly_Tsk(1);		// Wait Sensor Stability (Needs Over 100ns)
		
		if( CAMERA.sensor[sensor_id].is_spics_gpio ){
			// SP0CS0X is used as GPIO because it is pulled down
			Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_FUNC_GPIO);
			Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_STATUS_HIGH);
			Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_DIR_OUT);
		}

		// XVS XHS
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xhs, 1 );	// XHS from Sensor
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xvs, 1 );	// XVS from Sensor

		// init setting
		if( NULL != CAMERA.sensor[sensor_id].Func_Init ){
			if( CAMERA.sensor[sensor_id].Func_Init(sensor_id) == E_CAMERA_ERR_NG ){
				CAMERA_PRINT_ERROR(1);
				printf( "Sensor %d NOT connected\n", sensor_id );
			}
		}
	}

	M_SHUTTER_INIT();

	return E_CAMERA_ERR_OK;
}

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

