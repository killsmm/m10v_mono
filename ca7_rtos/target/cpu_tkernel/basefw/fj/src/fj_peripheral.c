/**
 * @file		fj_peripheral.c
 * @brief		Peripheral Controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_peripheral.h"
#include "i2c.h"
#include "sio.h"
#include "../device_driver/include/spi.h"
#include "gyroif.h"
#include "pwm.h"
#include "timer32.h"
#include "uart.h"
#include "wdog.h"
#include "interrupt.h"
#include "device_config.h"
#include "udc.h"
#include "../device_driver/include/slimbus.h"

/******************************************************************************
 Program
******************************************************************************/

FJ_ERR_CODE FJ_PWM_Open( FJ_PERI_CH ch )
{
	return fj_pwm_open( ch );
}

FJ_ERR_CODE FJ_PWM_Open_Step( FJ_PERI_CH ch )
{
	return fj_pwm_open_step( ch );
}

FJ_ERR_CODE FJ_PWM_Open_Step_2ch( FJ_PERI_CH ch_1st,  FJ_PERI_CH ch_2nd)
{
	return fj_pwm_open_step_2ch(ch_1st, ch_2nd);
}

FJ_ERR_CODE FJ_PWM_Calculate_Clock_Sel(ULONG usec, UCHAR* const clk_sel, USHORT* const cycle)
{
	return fj_pwm_calculate_clock_sel(usec, clk_sel, cycle);
}

FJ_ERR_CODE FJ_PWM_Calculate_Array_Clock_Sel(ULONG usec_array[][2], UCHAR* const clk_sel, FJ_PWM_STEP_DATA step_data[], UCHAR step_data_num)
{
	return fj_pwm_calculate_array_clock_sel(usec_array, clk_sel, step_data, step_data_num);
}

FJ_ERR_CODE FJ_PWM_Set_Buffer_Data_Window(FJ_PERI_CH ch, ULONG usec_array[][2], FJ_PWM_STEP_DATA step_data[], UCHAR data_num, UCHAR pulse_end_flag)
{
	return fj_pwm_set_buffer_data_window(ch, usec_array, step_data, data_num, pulse_end_flag);
}

FJ_ERR_CODE FJ_PWM_Set_Buffer_Data_Window_by_Step_Data(FJ_PERI_CH ch, FJ_PWM_STEP_DATA step_data[], UCHAR data_num)
{
	return fj_pwm_set_buffer_data_window_by_step_data(ch, step_data, data_num);
}

FJ_ERR_CODE FJ_PWM_Set_By_Ratio( FJ_PWM_CTRL const* const ctrl, UINT32 cycle_hz, UINT32 duty_ratio )
{
	return fj_pwm_set_by_ratio( ctrl, cycle_hz, duty_ratio );
}

FJ_ERR_CODE FJ_PWM_Set_By_Time( FJ_PWM_CTRL const* const ctrl, ULONG time1, ULONG time2 )
{
	return fj_pwm_set_by_time( ctrl, time1, time2 );
}

FJ_ERR_CODE FJ_PWM_Set_2ch( FJ_PWM_CTRL_PARAM const* const ctrl_a, FJ_PWM_CTRL_PARAM const* const ctrl_b)
{
	return fj_pwm_set_2ch( ctrl_a, ctrl_b);
}

FJ_ERR_CODE FJ_PWM_Set_Step( FJ_PWM_STEP_CTRL const* const ctrl )
{
	return fj_pwm_set_step( ctrl );
}

FJ_ERR_CODE FJ_PWM_Start( FJ_PERI_CH ch )
{
	return fj_pwm_start( ch );
}

FJ_ERR_CODE FJ_PWM_2ch_Start( FJ_PERI_CH ch_a, FJ_PERI_CH ch_b )
{
	return fj_pwm_2ch_start( ch_a, ch_b );
}

FJ_ERR_CODE FJ_PWM_Stop( FJ_PERI_CH ch )
{
	return fj_pwm_stop( ch );
}

FJ_ERR_CODE FJ_PWM_Close( FJ_PERI_CH ch )
{
	return fj_pwm_close( ch );
}

#ifndef CO_DEBUG_ON_PC
FJ_ERR_CODE FJ_TMR32_Open( FJ_PERI_CH ch )
{
	return fj_tmr32_open( ch );
}

FJ_ERR_CODE FJ_TMR32_Set( FJ_PERI_CH ch, ULONG usec, VP_CALLBACK pcallback )
{
	return fj_tmr32_set( ch, usec, pcallback );
}

FJ_ERR_CODE FJ_TMR32_Set_Pwm( FJ_PERI_CH reload_ch, FJ_PERI_CH pwm_ch, UCHAR mode, VP_CALLBACK pcallback )
{
	return fj_tmr32_set_pwm( reload_ch, pwm_ch, mode, pcallback );
}

FJ_ERR_CODE FJ_TMR32_Start( FJ_PERI_CH ch, BOOL reload )
{
	return fj_tmr32_start( ch, reload );
}

FJ_ERR_CODE FJ_TMR32_Stop( FJ_PERI_CH ch )
{
	return fj_tmr32_stop( ch );
}

FJ_ERR_CODE FJ_TMR32_Close( FJ_PERI_CH ch )
{
	return fj_tmr32_close( ch );
}
#endif // CO_DEBUG_ON_PC

FJ_ERR_CODE FJ_TMR32_Ctrl( FJ_PERI_CH ch,  FJ_TMR32_CTRL const* const tmr32_ctrl, VP_CALLBACK pcallback )
{
	return fj_tmr32_ctrl( ch, tmr32_ctrl, pcallback );
}

FJ_ERR_CODE FJ_TMR32_Tout_Ctrl( FJ_PERI_CH ch, BOOL enable, UINT32 out_level )
{
	return fj_tmr32_tout_ctrl( ch, enable, out_level );
}

FJ_ERR_CODE FJ_I2C_Open( FJ_PERI_CH ch )
{
	return fj_i2c_open( ch );
}

FJ_ERR_CODE FJ_I2C_Close( FJ_PERI_CH ch )
{
	return fj_i2c_close( ch );
}

FJ_ERR_CODE FJ_I2C_Ctrl( FJ_PERI_CH ch, FJ_I2C_CTRL i2c_ctrl, I2C_CALLBACK pcallback )
{
	return fj_i2c_ctrl( ch, i2c_ctrl, pcallback );
}

FJ_ERR_CODE FJ_I2C_Transfer( FJ_PERI_CH ch, FJ_I2C_TRANSFER_DATA trans_ctrl[], UINT32 length )
{
	return fj_i2c_transfer( ch, trans_ctrl, length );
}

FJ_ERR_CODE FJ_I2C_Set_ApproxClock( FJ_PERI_CH ch, UINT32 req_clock, UINT32* act_clock, BOOL under_flag )
{
	return fj_i2c_set_approxclock( ch, req_clock, act_clock, under_flag);
}

FJ_ERR_CODE FJ_UART_Debug_Start( FJ_PERI_CH ch, FJ_USIO_DMA_CH dma_ch )
{
	return fj_uart_debug_start( ch, dma_ch );
}

FJ_ERR_CODE FJ_UART_Debug_Stop( FJ_PERI_CH ch )
{
	return fj_uart_debug_stop( ch );
}

FJ_ERR_CODE	FJ_UART_Debug_Get_Char( CHAR *Buf )
{
	return fj_uart_debug_get_char( Buf );
}

FJ_ERR_CODE FJ_UART_Debug_Save( const CHAR* file_name )
{
	return fj_uart_debug_save( file_name );
}

FJ_ERR_CODE FJ_UART_Debug_Configure( FJ_UART_DEBUG_CONFIG const* config )
{
	return fj_uart_debug_configure( config );
}

FJ_ERR_CODE FJ_UART_Open( FJ_PERI_CH ch )
{
	return fj_uart_open( ch );
}

FJ_ERR_CODE FJ_UART_Close( FJ_PERI_CH ch )
{
	return fj_uart_close( ch );
}

FJ_ERR_CODE FJ_UART_Ctrl( FJ_PERI_CH ch, FJ_UART_CTRL const* const uart_ctrl )
{
	return fj_uart_ctrl( ch, uart_ctrl );;
}

FJ_ERR_CODE FJ_UART_Start( FJ_PERI_CH ch )
{
	return fj_uart_start( ch );
}

FJ_ERR_CODE FJ_UART_Stop( FJ_PERI_CH ch )
{
	return fj_uart_stop( ch );
}

FJ_ERR_CODE FJ_UART_Send( FJ_PERI_CH ch, VOID const* const send_ptr, UINT32 data_count, FJ_USIO_DMA_CH dma_ch )
{
	return fj_uart_send( ch, send_ptr, data_count, dma_ch );
}

FJ_ERR_CODE FJ_UART_Receive( FJ_PERI_CH ch, VOID const* const recv_ptr, UINT32 data_count, FJ_USIO_DMA_CH dma_ch )
{
	return fj_uart_receive( ch, recv_ptr, data_count, dma_ch );
}

FJ_ERR_CODE FJ_UART_Send_Target_Slave_Addr( FJ_PERI_CH ch, UCHAR slave_addr )
{
	return fj_uart_send_target_slave_addr( ch, slave_addr );
}

FJ_ERR_CODE FJ_UART_Set_Slave_Addr( FJ_PERI_CH ch, UCHAR slave_addr )
{
	return fj_uart_set_slave_addr( ch, slave_addr );
}

FJ_ERR_CODE FJ_UART_Send_Sync( FJ_PERI_CH ch, VOID const* const send_ptr, UINT32 buffer_size, UINT32 *data_bytes_sent, UINT32 timeout )
{
	return fj_uart_send_sync( ch, send_ptr, buffer_size, data_bytes_sent, timeout );
}

FJ_ERR_CODE FJ_UART_Receive_Sync( FJ_PERI_CH ch, VOID const* const recv_ptr, UINT32 buffer_size, UINT32 *data_bytes_read, UINT32 timeout )
{
	return fj_uart_receive_sync( ch, recv_ptr, buffer_size, data_bytes_read, timeout );
}


FJ_ERR_CODE	FJ_Gpio_Set_Function( FJ_GPIO_PORT port, UCHAR function )
{
	return fj_gpio_set_function( port, function );
}

FJ_ERR_CODE	FJ_Gpio_Set_Direction( FJ_GPIO_PORT port, UCHAR direction )
{
	return fj_gpio_set_direction( port, direction );
}

FJ_ERR_CODE	FJ_Gpio_Set_Status( FJ_GPIO_PORT port, UCHAR status )
{
	return fj_gpio_set_status( port, status );
}
FJ_ERR_CODE	FJ_Gpio_Get_Status( FJ_GPIO_PORT port, UCHAR* status )
{
	return fj_gpio_get_status( port, status );
}
FJ_ERR_CODE	FJ_Gpio_Set_Pull_Up_Down_Enable( FJ_GPIO_PORT port, UCHAR enable )
{
	return fj_gpio_set_pull_up_down_enable( port, enable );
}
FJ_ERR_CODE	FJ_Gpio_Set_Pull_Up_Down_Ctrl( FJ_GPIO_PORT port, UCHAR control )
{
	return fj_gpio_set_pull_up_down_ctrl( port, control );
}

FJ_ERR_CODE	FJ_Ext_Interrupt_Set_Config( FJ_GPIO_PORT port, FJ_EXT_INT_CONFIG* int_config )
{
	return fj_ext_interrupt_set_config( port, int_config );
}

FJ_ERR_CODE	FJ_Ext_Interrupt_Set_Enable( FJ_GPIO_PORT port, UCHAR enable )
{
	return fj_ext_interrupt_set_enable( port, enable );
}

FJ_ERR_CODE FJ_Ext_Interrupt_Set_Level( FJ_GPIO_PORT port, UCHAR level )
{
	return fj_ext_interrupt_set_level( port, level );
}

FJ_ERR_CODE FJ_Ext_Interrupt_Get_Level( FJ_GPIO_PORT port, UCHAR* level )
{
	return fj_ext_interrupt_get_level( port, level );
}

FJ_ERR_CODE FJ_Ext_Interrupt_Clear_Req( FJ_GPIO_PORT port )
{
	return fj_ext_interrupt_clear_req( port );
}

FJ_ERR_CODE FJ_Wdog_Timer_Init( VOID )
{
	return fj_wdog_timer_init();
}

FJ_ERR_CODE FJ_Wdog_Timer_Open( VOID )
{
	return fj_wdog_timer_open();
}

FJ_ERR_CODE FJ_Wdog_Timer_Ctrl( FJ_WDOG_MODE wdog_mode, ULONG msec, VP_CALLBACK user_handler )
{
	return fj_wdog_timer_ctrl( wdog_mode, msec, user_handler );
}

FJ_ERR_CODE FJ_Wdog_Timer_Refresh( VOID )
{
	return fj_wdog_timer_refresh();
}

FJ_ERR_CODE FJ_Wdog_Timer_Get_Reset_Status( FJ_WDOG_RESET_STATUS* raw_wdog_int, FJ_WDOG_RESET_MASK_STATUS* mask_wdog_int )
{
	return fj_wdog_timer_get_reset_status( raw_wdog_int, mask_wdog_int );
}

FJ_ERR_CODE FJ_Wdog_Timer_Get_Load_Value( ULONG* load_value )
{
	return fj_wdog_timer_get_load_value(load_value);
}

FJ_ERR_CODE FJ_Wdog_Timer_Close( VOID )
{
	return fj_wdog_timer_close();
}

FJ_ERR_CODE FJ_SIO_Open( FJ_PERI_CH ch )
{
	return fj_sio_open( ch );
}

FJ_ERR_CODE FJ_SIO_Close( FJ_PERI_CH ch )
{
	return fj_sio_close( ch );
}

FJ_ERR_CODE FJ_SIO_Ctrl( FJ_PERI_CH ch, FJ_SIO_CTRL const* const sio_ctrl )
{
	return fj_sio_ctrl( ch , sio_ctrl );
}

FJ_ERR_CODE FJ_SIO_Stop( FJ_PERI_CH ch )
{
	return fj_sio_stop( ch );
}

FJ_ERR_CODE FJ_SIO_Transfer( FJ_PERI_CH ch, FJ_SIO_CTRL_TRANSFER const* const trans_ctrl, FJ_USIO_DMA_CH dma_ch )
{
	return fj_sio_transfer( ch, trans_ctrl, dma_ch );
}

FJ_ERR_CODE FJ_SIO_Full_Duplex( FJ_PERI_CH ch, FJ_SIO_CTRL_FULL_DUPLEX const* const sio_ctrl )
{
	return fj_sio_full_duplex( ch, sio_ctrl );
}

FJ_ERR_CODE FJ_SIO_Full_Duplex_DMA( FJ_PERI_CH ch, FJ_SIO_CTRL_FULL_DUPLEX const* const sio_ctrl, FJ_USIO_DMA_CH dma_ch_send, FJ_USIO_DMA_CH dma_ch_recv )
{
	return fj_sio_full_duplex_dma( ch, sio_ctrl, dma_ch_send, dma_ch_recv );
}

FJ_ERR_CODE FJ_SPI_Open( FJ_PERI_CH ch )
{
	return fj_spi_open( ch );
}

FJ_ERR_CODE FJ_SPI_Close( FJ_PERI_CH ch )
{
	return fj_spi_close( ch );
}

FJ_ERR_CODE FJ_SPI_Ctrl( FJ_PERI_CH ch, FJ_SPI_CTRL const* const spi_ctrl )
{
	return fj_spi_ctrl( ch , spi_ctrl );
}

FJ_ERR_CODE FJ_SPI_Stop( FJ_PERI_CH ch )
{
	return fj_spi_stop( ch );
}

FJ_ERR_CODE FJ_SPI_Transfer( FJ_PERI_CH ch, FJ_SPI_CTRL_TRANSFER const* const trans_ctrl, FJ_USIO_DMA_CH dma_ch )
{
	return fj_spi_transfer( ch, trans_ctrl, dma_ch );
}

FJ_ERR_CODE FJ_SPI_Full_Duplex( FJ_PERI_CH ch, FJ_SPI_CTRL_FULL_DUPLEX const* const spi_ctrl )
{
	return fj_spi_full_duplex( ch, spi_ctrl );
}

FJ_ERR_CODE FJ_SPI_Full_Duplex_DMA( FJ_PERI_CH ch, FJ_SPI_CTRL_FULL_DUPLEX const* const spi_ctrl, FJ_USIO_DMA_CH dma_ch_send, FJ_USIO_DMA_CH dma_ch_recv )
{
	return fj_spi_full_duplex_dma( ch, spi_ctrl, dma_ch_send, dma_ch_recv );
}

FJ_ERR_CODE FJ_GyroIF_Start_Read( FJ_GYROIF_READ_CTRL const* const gyro_ctrl )
{
	return fj_gyroif_start_read( gyro_ctrl );
}

FJ_ERR_CODE FJ_GyroIF_Stop_Read( VOID )
{
	return fj_gyroif_stop_read();
}

FJ_ERR_CODE FJ_GyroIF_Get_Read_Count( UINT32* const read_count )
{
	return fj_gyroif_get_read_count( read_count );
}

FJ_ERR_CODE FJ_UDC_Init( FJ_UDC_ZIN_CTRL zin_ctrl )
{
	return fj_udc_init( zin_ctrl );
}

FJ_ERR_CODE FJ_UDC_Open( FJ_PERI_CH ch )
{
	return fj_udc_open( ch );
}

FJ_ERR_CODE FJ_UDC_Close( FJ_PERI_CH ch )
{
	return fj_udc_close( ch );
}

FJ_ERR_CODE FJ_UDC_Start( FJ_PERI_CH ch )
{
	return fj_udc_start( ch );
}

FJ_ERR_CODE FJ_UDC_Stop( FJ_PERI_CH ch )
{
	return fj_udc_stop( ch );
}

FJ_ERR_CODE FJ_UDC_Set_Timer_Mode( FJ_PERI_CH ch, ULONG usec, UDC_CALLBACK pCallback )
{
	return fj_udc_set_timer_mode( ch, usec, pCallback );
}

FJ_ERR_CODE FJ_UDC_Set_Up_Down_Count_Mode( FJ_PERI_CH ch, FJ_UDC_COUNT_EDGE edge, FJ_UDC_CTRL const* const udc_ctrl )
{
	return fj_udc_set_up_down_count_mode(ch, edge, udc_ctrl);
}

FJ_ERR_CODE FJ_UDC_Set_Phase_Lag_Mode( FJ_PERI_CH ch, FJ_UDC_PHASE_COUNT count_mode, FJ_UDC_CTRL const* const udc_ctrl )
{
	return fj_udc_set_phase_lag_mode( ch, count_mode, udc_ctrl );
}

FJ_ERR_CODE FJ_UDC_Set_Up_Down_Counter( FJ_PERI_CH ch, USHORT up_down_counter )
{
	return fj_udc_set_up_down_counter( ch, up_down_counter );
}

USHORT FJ_UDC_Get_Up_Down_Counter( FJ_PERI_CH ch )
{
	return fj_udc_get_up_down_counter( ch );
}

FJ_ERR_CODE FJ_UDC_Clear_Up_Down_Counter( FJ_PERI_CH ch )
{
	return fj_udc_clear_up_down_counter( ch );
}

FJ_ERR_CODE FJ_UDC_Set_Comp_Counter( FJ_PERI_CH ch, USHORT comp_counter )
{
	return fj_udc_set_comp_counter( ch, comp_counter );
}

FJ_UDC_COUNT_DIR FJ_UDC_Get_Up_Down_Flag( FJ_PERI_CH ch )
{
	return fj_udc_get_up_down_flag( ch );
}

FJ_ERR_CODE FJ_UDC_Set_CallBack( FJ_PERI_CH ch, UDC_CALLBACK pCallBack )
{
	return fj_udc_set_callback( ch, pCallBack );
}

FJ_ERR_CODE FJ_Slimbus_Open( FJ_SLIMBUS_CH ch )
{
	return fj_slimbus_open( ch );
}

FJ_ERR_CODE FJ_Slimbus_Close( FJ_SLIMBUS_CH ch )
{
	return fj_slimbus_close( ch );
}

FJ_ERR_CODE FJ_Slimbus_Ctrl( FJ_SLIMBUS_CTRL const* const slimbus_ctrl )
{
	return fj_slimbus_ctrl( slimbus_ctrl );
}

FJ_ERR_CODE FJ_Slimbus_Get_Ctrl( FJ_SLIMBUS_CH ch, FJ_SLIMBUS_CTRL* const slimbus_ctrl )
{
	return fj_slimbus_get_ctrl( ch, slimbus_ctrl );
}

FJ_ERR_CODE FJ_Slimbus_Start( FJ_SLIMBUS_CH ch )
{
	return fj_slimbus_start( ch );
}

FJ_ERR_CODE FJ_Slimbus_Stop( FJ_SLIMBUS_CH ch )
{
	return fj_slimbus_stop( ch );
}

FJ_ERR_CODE FJ_Slimbus_Write_Msg( FJ_SLIMBUS_CH ch, UINT32* msg, UINT32 size )
{
	return fj_slimbus_write_msg( ch, msg, size );
}

FJ_ERR_CODE FJ_Slimbus_Read_Msg( FJ_SLIMBUS_CH ch, UINT32* msg, UINT32* size )
{
	return fj_slimbus_read_msg( ch,  msg,  size );
}

FJ_ERR_CODE FJ_Slimbus_Write_Data( FJ_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, SLIMBUS_CALLBACK pcallback )
{
	return fj_slimbus_write_data( ch, dma_ch, data, count, pcallback );
}

FJ_ERR_CODE FJ_Slimbus_Read_Data( FJ_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, SLIMBUS_CALLBACK pcallback )
{
	return fj_slimbus_read_data( ch, dma_ch, data, count, pcallback );
}

FJ_ERR_CODE FJ_Slimbus_Set_Read_Msg_Area_Addr( FJ_SLIMBUS_CH ch, UINT32 addr, UINT32 size )
{
	return fj_slimbus_set_read_msg_area_addr(ch, addr, size);
}

VOID FJ_Slimbus_Get_Read_Msg_Area_Addr( FJ_SLIMBUS_CH ch, UINT32* addr, UINT32* size )
{
	fj_slimbus_get_read_msg_area_addr(ch, addr, size);
}

FJ_ERR_CODE FJ_Slimbus_Set_Read_Data_Area_Addr( FJ_SLIMBUS_CH ch, UINT32 addr, UINT32 size )
{
	return fj_slimbus_set_read_data_area_addr(ch, addr, size);
}

VOID FJ_Slimbus_Get_Read_Data_Area_Addr( FJ_SLIMBUS_CH ch, UINT32* addr, UINT32* size )
{
	fj_slimbus_get_read_data_area_addr(ch, addr, size);
}
