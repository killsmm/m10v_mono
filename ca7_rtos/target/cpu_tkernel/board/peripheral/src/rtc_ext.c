/**
 * @file		rtc_ext.c
 * @brief		External Real Time Clock driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "rtc_ext.h"
#include "debug.h"
#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_RTC_EXT_OP_OR		(0)
#define D_RTC_EXT_OP_AND	(1)
#define D_RTC_EXT_OP_OW		(2) //over write

/*----------------------------------------------------------------------*/
/* Union																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static INT32 rtc_ext_i2c_read(E_RTC_EXT_ADDR addr, BYTE* receiveData);
static INT32 rtc_ext_i2c_send(E_RTC_EXT_ADDR addr, UCHAR data, CHAR op);
static UCHAR rtc_ext_conv_2_bcd_format(UCHAR data);
static UCHAR rtc_ext_conv_2_normal_format(UCHAR data);

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 @brief  I2C read process.
 @param  [I]E_RTC_EXT_ADDR addr
 @param  [O]BYTE* receiveData
 @retval INT32
 @note		None
 @attention	None
 */
static INT32 rtc_ext_i2c_read(E_RTC_EXT_ADDR addr, BYTE* receiveData)
{
	BYTE SendData;
	INT32 ret;
	FJ_I2C_CTRL slave_info;
	FJ_I2C_TRANSFER_DATA  memory_data;

	memset(&slave_info, 0, sizeof(slave_info));

	ret = FJ_I2C_Open( FJ_PERI_CH_2 );
	if( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Open ERROR): %d\n", __LINE__));
		return D_RTC_EXT_NG;
	}

	slave_info.slave_address = D_RTC_EXT_SLAVE_ADDRESS;			// Set slave address
	slave_info.address_format = FJ_I2C_ADDRESS_FORMAT_7_BIT;	// Set to 7 bit
	slave_info.speed = FJ_I2C_SPEED_FAST;
	slave_info.type = FJ_I2C_TYPE_MASTER;

	ret = FJ_I2C_Ctrl( FJ_PERI_CH_2, slave_info, NULL );
	if( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Ctrl ERROR): %d\n", __LINE__));
		ret = FJ_I2C_Close( FJ_PERI_CH_2 );
		if( ret != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Close ERROR): %d\n", __LINE__));
			return D_RTC_EXT_NG;
		}
		return D_RTC_EXT_NG;
	}
	
    SendData = (BYTE)((addr << 4) & 0xF0);
	memory_data.slave_register_address.data_length = 1;
	memory_data.slave_register_address.data = &SendData;
	memory_data.trn_data.data = receiveData;				// Read data buffer
	memory_data.trn_data.data_length = 1;					// Receive data size
	memory_data.dirction = 1;								// Read data from slave
	memory_data.retry_num = 3;								// Set retry num to 3
	memory_data.timeout_threshold = 500;					// Set timeout to 500ms
	
	ret = FJ_I2C_Transfer( FJ_PERI_CH_2, &memory_data, 1 );
	if( ret != FJ_ERR_OK )
	{
		BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Transfer ERROR): %d\n", __LINE__));
		ret = FJ_I2C_Close( FJ_PERI_CH_2 );
		if( ret != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Close ERROR): %d\n", __LINE__));
			return D_RTC_EXT_NG;
		}
		return D_RTC_EXT_NG;
	}

	ret = FJ_I2C_Close( FJ_PERI_CH_2 );
	if( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Close ERROR): %d\n", __LINE__));
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  I2C send process.
 @param  [I]E_RTC_EXT_ADDR addr
 @param  [I]UCHAR data
 @param  [I]CHAR op
 @retval INT32
 @note		None
 @attention	None
 */
static INT32 rtc_ext_i2c_send(E_RTC_EXT_ADDR addr, UCHAR data, CHAR op)
{
	INT32 ret;
	FJ_I2C_CTRL slave_info;
	FJ_I2C_TRANSFER_DATA  memory_data;
	BYTE address_data[1];
	BYTE send_data[1];
	BYTE receive_data[16] = {0};
	UCHAR rtc_ext_ctrl = 0;

	if ((op == D_RTC_EXT_OP_OR) || (op == D_RTC_EXT_OP_AND)) {
		if (rtc_ext_i2c_read(addr, &receive_data[0]) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}
		rtc_ext_ctrl = (UCHAR)receive_data[0];
	}
	if(op == D_RTC_EXT_OP_OW) {
		rtc_ext_ctrl = data;
	}					
	if(op == D_RTC_EXT_OP_OR) {
		rtc_ext_ctrl |= data;
	}
	if(op == D_RTC_EXT_OP_AND) {
		rtc_ext_ctrl &= data;
	}

	memset(&slave_info, 0, sizeof(slave_info));

	ret = FJ_I2C_Open( FJ_PERI_CH_2 );
	if( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Open ERROR): %d\n", __LINE__));
		return D_RTC_EXT_NG;
	}

	slave_info.slave_address = D_RTC_EXT_SLAVE_ADDRESS;			// Set slave address
	slave_info.address_format = FJ_I2C_ADDRESS_FORMAT_7_BIT;	// Set to 7 bit
	slave_info.speed = FJ_I2C_SPEED_FAST;
	slave_info.type = FJ_I2C_TYPE_MASTER;

	ret = FJ_I2C_Ctrl( FJ_PERI_CH_2, slave_info, NULL );
	if( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Ctrl ERROR): %d\n", __LINE__));
		ret = FJ_I2C_Close( FJ_PERI_CH_2 );
		if( ret != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Close ERROR): %d\n", __LINE__));
			return D_RTC_EXT_NG;
		}
		return D_RTC_EXT_NG;
	}

	address_data[0] = (BYTE)((addr << 4) & 0xF0);
	send_data[0] = (BYTE)rtc_ext_ctrl;
	memory_data.slave_register_address.data_length = 1;
	memory_data.slave_register_address.data = address_data;
	memory_data.trn_data.data = &send_data[0];				// Read data buffer
	memory_data.trn_data.data_length = sizeof(send_data);	// Receive data size
	memory_data.dirction = 0;								// Write data to slave
	memory_data.retry_num = 3;								// Set retry num to 3
	memory_data.timeout_threshold = 500;					// Set timeout to 500ms

	ret = FJ_I2C_Transfer( FJ_PERI_CH_2, &memory_data, 1 );
	if( ret != FJ_ERR_OK )
	{
		BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Transfer ERROR): %d\n", __LINE__));
		ret = FJ_I2C_Close( FJ_PERI_CH_2 );
		if( ret != FJ_ERR_OK ) {
			BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Close ERROR): %d\n", __LINE__));
			return D_RTC_EXT_NG;
		}
		return D_RTC_EXT_NG;
	}

	ret = FJ_I2C_Close( FJ_PERI_CH_2 );
	if( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("[RTC_EXT]FJ_I2C_Close ERROR): %d\n", __LINE__));
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Convert normal format to BCD format.
 @param  [I]UCHAR data
 @retval UCHAR
 @note		None
 @attention	None
 */
static UCHAR rtc_ext_conv_2_bcd_format(UCHAR data)
{
	UCHAR data_1;
	UCHAR data_10;

	data_1 = data % 10;
	data_10 = data / 10;

	return ((data_10 << 4) | data_1);
}

/**
 @brief  Convert BCD format to normal format.
 @param  [I]UCHAR data
 @retval UCHAR
 @note		None
 @attention	None
 */
static UCHAR rtc_ext_conv_2_normal_format(UCHAR data)
{
	UCHAR data_1;
	UCHAR data_10;

	data_1 = (data & 0x0F);
	data_10 = ((data & 0xF0) >> 4);

	return ((data_10 * 10) + data_1);
}

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 @brief  Initialize register.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		second register:			0000 0000B<br>
 			minute register:			0000 0000B<br>
 			hour register:				0001 0010B<br>
 			weekday register:			0000 0001B<br>
 			day register:				0000 0001B<br>
 			month register:				0000 0001B<br>
 			year register:				0000 0001B<br>
 			digital offset register:	0000 0000B<br>
 			alarm_w minute register:	0000 0000B<br>
 			alarm_w hour register:		0000 0000B<br>
 			alarm_w weekday register:	0000 0000B<br>
 			alarm_d minute register:	0000 0000B<br>
 			alarm_d hour register:		0000 0000B<br>
 			reserved register:			0000 0000B<br>
 			control_1 register:			0000 0000B<br>
 			control_2 register:			0011 0000B<br>
 @attention	None
 */
INT32 RTC_Ext_Init(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_SECONDS, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_MINUTES, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_HOURS, 0x12, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_WEEKDAYS, 0x01, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_DAYS, 0x01, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_MONTHS, 0x01, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_YEARS, 0x01, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_DIGITAL_OFFSET, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMW_MINUTE, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMW_HOUR, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMW_WEEKDAY, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMD_MINUTE, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMD_HOUR, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_RESERVED, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0x00, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0x30, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Set date and time information.
 @param  [I]T_RTC_EXT_DATA const* const rtc_data
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		Year range: 2001 to 2099<br>
			Month range: 1 to 12<br>
			Day range:<br>
				1,3,5,7,8,10,12 month: 1 to 31<br>
				4,6,9,11 month: 1 to 30<br>
				leap year 2 month: 1 to 29<br>
				normal year 2 month: 1 to 28<br>
			Hour range:<br>
				24 hours: 0 to 23<br>
				12 hours: 1 to 12(AM01 to AM12), 21 to 32(PM01 to PM12)<br>
			Minute range: 0 to 59<br>
			Second range: 0 to 59<br>
			Weekday range: 0 to 6(0:Sun 1:Mon 2:Tues 3:Wed 4:Thur 5:Fri 6:Sat)<br>
 @attention	Hours Time system is setted by RTC_Ext_Set_Hours_Mode function.
 */
INT32 RTC_Ext_Set_Time(T_RTC_EXT_DATA const* const rtc_data)
{
	UCHAR year;
	UCHAR month;
	UCHAR day;
	UCHAR weekday;
	UCHAR hour;
	UCHAR minute;
	UCHAR second;
	
#ifdef CO_PARAM_CHECK
	if(rtc_data == NULL){
		BF_Debug_Print_Error(("External RTC input param error. [*rtc_data] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	year	= rtc_ext_conv_2_bcd_format(rtc_data->YEAR - D_RTC_EXT_YEAR_BASE);
	month	= rtc_ext_conv_2_bcd_format(rtc_data->MONTH);
	day		= rtc_ext_conv_2_bcd_format(rtc_data->DAY);
	weekday	= rtc_data->WEEKDAY;
	hour	= rtc_ext_conv_2_bcd_format(rtc_data->HOUR);
	minute	= rtc_ext_conv_2_bcd_format(rtc_data->MINUTE);
	second	= rtc_ext_conv_2_bcd_format(rtc_data->SECOND);
	
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_YEARS, year, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_MONTHS, month, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_DAYS, day, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_WEEKDAYS, weekday, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_HOURS, hour, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_MINUTES, minute, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_SECONDS, second, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Get date and time information.
 @param  [O]T_RTC_EXT_DATA* const rtc_data
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		Year range: 2001 to 2099<br>
			Month range: 1 to 12<br>
			Day range:<br>
				1,3,5,7,8,10,12 month: 1 to 31<br>
				4,6,9,11 month: 1 to 30<br>
				leap year 2 month: 1 to 29<br>
				normal year 2 month: 1 to 28<br>
			Hour range:<br>
				24 hours: 0 to 23<br>
				12 hours: 1 to 12(AM01 to AM12), 21 to 32(PM01 to PM12)<br>
			Minute range: 0 to 59<br>
			Second range: 0 to 59<br>
			Weekday range: 0 to 6(0:Sun 1:Mon 2:Tues 3:Wed 4:Thur 5:Fri 6:Sat)<br>
 @attention	None
 */
INT32 RTC_Ext_Get_Time(T_RTC_EXT_DATA* const rtc_data)
{
	UCHAR month;
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_data == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_data] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_SECONDS, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_data->SECOND	= rtc_ext_conv_2_normal_format(receiveData[0]);

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_MINUTES, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_data->MINUTE	= rtc_ext_conv_2_normal_format(receiveData[0]);

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_HOURS, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_data->HOUR		= rtc_ext_conv_2_normal_format(receiveData[0]);

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_WEEKDAYS, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_data->WEEKDAY	= receiveData[0];

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_DAYS, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_data->DAY		= rtc_ext_conv_2_normal_format(receiveData[0]);

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_MONTHS, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	month				= receiveData[0];
	rtc_data->MONTH		= rtc_ext_conv_2_normal_format(month & 0x7F);

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_YEARS, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_data->YEAR		= rtc_ext_conv_2_normal_format(receiveData[0]) + D_RTC_EXT_YEAR_BASE + ((month & 0x80) >> 7) * 100;
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Set adjusted value of time precision.
 @param  [I]INT8 const* const rtc_offset
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		advance time range: 0 to 63<br>
			delay time range: -1 to -64<br>
 @attention	Register bit7 is test bit, must be setted 0.
 */
INT32 RTC_Ext_Set_Digital_Offset(INT8 const* const rtc_offset)
{
#ifdef CO_PARAM_CHECK
	if(rtc_offset == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_offset] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_DIGITAL_OFFSET, ((*rtc_offset) & 0x7F), D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Get adjusted value of time precision.
 @param  [O]signed char* const rtc_offset
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		advance time range: 0 to 63<br>
			delay time range: -1 to -64<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_Digital_Offset(signed char* const rtc_offset)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_offset == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_offset] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_DIGITAL_OFFSET, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_offset = receiveData[0];
	if ((*rtc_offset) & 0x40) { // rtc_offset < 0
		*rtc_offset |= 0x80;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Set alarm_w time information.
 @param  [I]T_RTC_EXT_ALARM_W const* const rtc_alarm
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		Hour range:<br>
				24 hours: 0 to 23<br>
				12 hours: 1 to 12(AM01 to AM12), 21 to 32(PM01 to PM12)<br>
			Minute range: 0 to 59<br>
			Weekday range: 0 to 127(bit7:0 bit6:Sat bit5:Fri bit4:Thur bit3:Wed bit2:Tues bit1:Mon bit0:Sun)<br>
 @attention	Hours Time system is setted by RTC_Ext_Set_Hours_Mode function.
 */
INT32 RTC_Ext_Set_Alarm_W(T_RTC_EXT_ALARM_W const* const rtc_alarm)
{
	UCHAR weekday;
	UCHAR hour;
	UCHAR minute;
	
#ifdef CO_PARAM_CHECK
	if(rtc_alarm == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_alarm] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	weekday	= rtc_alarm->WEEKDAY;
	hour	= rtc_ext_conv_2_bcd_format(rtc_alarm->HOUR);
	minute	= rtc_ext_conv_2_bcd_format(rtc_alarm->MINUTE);

	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMW_WEEKDAY, weekday, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMW_HOUR, hour, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMW_MINUTE, minute, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get alarm_w time information.
 @param  [O]T_RTC_EXT_ALARM_W* const rtc_alarm
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		Hour range:<br>
				24 hours: 0 to 23<br>
				12 hours: 1 to 12(AM01 to AM12), 21 to 32(PM01 to PM12)<br>
			Minute range: 0 to 59<br>
			Weekday range: 0 to 127(bit7:0 bit6:Sat bit5:Fri bit4:Thur bit3:Wed bit2:Tues bit1:Mon bit0:Sun)<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_Alarm_W(T_RTC_EXT_ALARM_W* const rtc_alarm)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_alarm == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_alarm] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_ALARMW_WEEKDAY, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_alarm->WEEKDAY	= receiveData[0];

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_ALARMW_HOUR, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_alarm->HOUR		= rtc_ext_conv_2_normal_format(receiveData[0]);

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_ALARMW_MINUTE, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_alarm->MINUTE	= rtc_ext_conv_2_normal_format(receiveData[0]);

	return D_RTC_EXT_OK;
}

/**
 @brief  Set alarm_d time information.
 @param  [I]T_RTC_EXT_ALARM_D const* const rtc_alarm
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		Hour range:<br>
				24 hours: 0 to 23<br>
				12 hours: 1 to 12(AM01 to AM12), 21 to 32(PM01 to PM12)<br>
			Minute range: 0 to 59<br>
 @attention	Hours Time system is setted by RTC_Ext_Set_Hours_Mode function.
 */
INT32 RTC_Ext_Set_Alarm_D(T_RTC_EXT_ALARM_D const* const rtc_alarm)
{
	UCHAR hour;
	UCHAR minute;
	
#ifdef CO_PARAM_CHECK
	if(rtc_alarm == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_alarm] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	hour	= rtc_ext_conv_2_bcd_format(rtc_alarm->HOUR);
	minute	= rtc_ext_conv_2_bcd_format(rtc_alarm->MINUTE);

	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMD_HOUR, hour, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_ALARMD_MINUTE, minute, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get alarm_d time information.
 @param  [O]T_RTC_EXT_ALARM_D* const rtc_alarm
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		Hour range:<br>
				24 hours: 0 to 23<br>
				12 hours: 1 to 12(AM01 to AM12), 21 to 32(PM01 to PM12)<br>
			Minute range: 0 to 59<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_Alarm_D(T_RTC_EXT_ALARM_D* const rtc_alarm)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_alarm == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_alarm] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_ALARMD_HOUR, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_alarm->HOUR		= rtc_ext_conv_2_normal_format(receiveData[0]);

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_ALARMD_MINUTE, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_alarm->MINUTE	= rtc_ext_conv_2_normal_format(receiveData[0]);

	return D_RTC_EXT_OK;
}

/**
 @brief  Set control_1 register.
 @param  [I]UCHAR const* const rtc_ctrl
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit7 bit6 bit5   bit4   bit3 bit2 bit1 bit0<br>
			WALE DALE /12,24 /CLEN2 TEST CT2  CT1  CT0<br>
 @attention	bit3 must be setted 0.
 */
INT32 RTC_Ext_Set_Ctrl1(UCHAR const* const rtc_ctrl)
{
#ifdef CO_PARAM_CHECK
	if(rtc_ctrl == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_ctrl] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, *rtc_ctrl, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get control_1 register.
 @param  [O]UCHAR* const rtc_ctrl
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit7 bit6 bit5   bit4   bit3 bit2 bit1 bit0<br>
			WALE DALE /12,24 /CLEN2 TEST CT2  CT1  CT0<br>
 @attention	None
 */
INT32 RTC_Ext_Get_Ctrl1(UCHAR* const rtc_ctrl)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_ctrl == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_ctrl] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL1, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_ctrl = receiveData[0];
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Set control_2 register.
 @param  [I]UCHAR const* const rtc_ctrl
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit7 bit6 bit5 bit4 bit3   bit2 bit1 bit0<br>
			VDSL VDET /XST PON  /CLEN1 CTFG WAFG DAFG<br>
 @attention	bit6:writing of 1 is invalid.<br>
 			bit5:writing of 0 is invalid.<br>
 			bit4:writing of 1 is invalid.<br>
 			bit2:writing of 1 is invalid.<br>
 			bit1:writing of 1 is invalid.<br>
 			bit0:writing of 1 is invalid.<br>
 */
INT32 RTC_Ext_Set_Ctrl2(UCHAR const* const rtc_ctrl)
{
#ifdef CO_PARAM_CHECK
	if(rtc_ctrl == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_ctrl] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, *rtc_ctrl, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get control_2 register.
 @param  [O]UCHAR* const rtc_ctrl
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit7 bit6 bit5 bit4 bit3   bit2 bit1 bit0<br>
			VDSL VDET /XST PON  /CLEN1 CTFG WAFG DAFG<br>
 @attention	None
 */
INT32 RTC_Ext_Get_Ctrl2(UCHAR* const rtc_ctrl)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_ctrl == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_ctrl] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_ctrl = receiveData[0];
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Clear constant periodic interruption output state of /INTA.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit2 of Control_2 register is setted 0
 @attention	None
 */
INT32 RTC_Ext_Clear_INTA_Status(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0xFB, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get constant periodic interruption output state of /INTA.
 @param  [O]E_RTC_EXT_INTA_OUTPUT_STATUS* const rtc_status
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		rtc_status:<br>
 				0:Output state is OFF; /INTA=OFF(Hi-z)<br>
				1:Output state is ON; /INTA="L"<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_INTA_Status(E_RTC_EXT_INTA_OUTPUT_STATUS* const rtc_status)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_status == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_status] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_status = (E_RTC_EXT_INTA_OUTPUT_STATUS)((receiveData[0] & 0x04) >> 2);
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Set output mode of /INTA.
 @param  [I]E_RTC_EXT_INTA_OUTPUT_MODE const* const rtc_mode
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		0: /INTA=Hi-z(=OFF)<br>
			1: /INTA="L"<br>
			2: 2Hz(Duty 50%)<br>
			3: 1Hz(Duty 50%)<br>
			4: per-second<br>
			5: per-minute<br>
			6: per-hour<br>
			7: per-month<br>
 @attention	None
 */
INT32 RTC_Ext_Set_INTA_Output_Mode(E_RTC_EXT_INTA_OUTPUT_MODE const* const rtc_mode)
{
	UCHAR rtc_ctrl;
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_mode == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_mode] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL1, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	rtc_ctrl = (receiveData[0] & 0xF8) | (*rtc_mode);
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, rtc_ctrl, D_RTC_EXT_OP_OW) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get output mode of /INTA.
 @param  [O]E_RTC_EXT_INTA_OUTPUT_MODE* const rtc_mode
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		0: /INTA=Hi-z(=OFF)<br>
			1: /INTA="L"<br>
			2: 2Hz(Duty 50%)<br>
			3: 1Hz(Duty 50%)<br>
			4: per-second<br>
			5: per-minute<br>
			6: per-hour<br>
			7: per-month<br>
 @attention	None
 */
INT32 RTC_Ext_Get_INTA_Output_Mode(E_RTC_EXT_INTA_OUTPUT_MODE* const rtc_mode)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_mode == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_mode] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL1, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_mode = (E_RTC_EXT_INTA_OUTPUT_MODE)(receiveData[0] & 0x07);
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Set hours time system.
 @param  [I]E_RTC_EXT_HOURS_MODE const* const rtc_hours_mode
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		0: 12 hours time system<br>
			1: 24 hours time system<br>
 @attention	None
 */
INT32 RTC_Ext_Set_Hours_Mode(E_RTC_EXT_HOURS_MODE const* const rtc_hours_mode)
{
#ifdef CO_PARAM_CHECK
	if(rtc_hours_mode == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_hours_mode] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (*rtc_hours_mode == E_RTC_EXT_HOURS_MODE_24) {
		if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0x20, D_RTC_EXT_OP_OR) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}
	}
	else {
		if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0xDF, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}		
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get hours time system.
 @param  [O]E_RTC_EXT_HOURS_MODE* const rtc_hours_mode
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		0: 12 hours time system<br>
			1: 24 hours time system<br>
 @attention	None
 */
INT32 RTC_Ext_Get_Hours_Mode(E_RTC_EXT_HOURS_MODE* const rtc_hours_mode)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_hours_mode == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_hours_mode] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL1, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_hours_mode = (E_RTC_EXT_HOURS_MODE)((receiveData[0] & 0x20) >> 5);

	return D_RTC_EXT_OK;
}

/**
 @brief  Start comparison of alarm_w time information.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit7 of Control_1 register is setted 1
 @attention	None
 */
INT32 RTC_Ext_Start_Alarm_W(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0x80, D_RTC_EXT_OP_OR) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Stop comparison of alarm_w time information.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit7 of Control_1 register is setted 0
 @attention	None
 */
INT32 RTC_Ext_Stop_Alarm_W(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0x7F, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Clear comparison status of alarm_w time.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit1 of Control_2 register is setted 0
 @attention	None
 */
INT32 RTC_Ext_Clear_Alarm_W_Status(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0xFD, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get comparison status of alarm_w time.
 @param  [O]E_RTC_EXT_ALARM_TIME_STATUS* const rtc_status
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		rtc_status:<br>
 				0:alarm_w setting time and current time is not same<br>
				1:alarm_w setting time and current time is same<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_Alarm_W_Status(E_RTC_EXT_ALARM_TIME_STATUS* const rtc_status)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_status == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_status] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_status = (E_RTC_EXT_ALARM_TIME_STATUS)((receiveData[0] & 0x02) >> 1);
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Start comparison of alarm_d time information.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit6 of Control_1 register is setted 1
 @attention	None
 */
INT32 RTC_Ext_Start_Alarm_D(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0x40, D_RTC_EXT_OP_OR) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Stop comparison of alarm_d time information.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit6 of Control_1 register is setted 0
 @attention	None
 */
INT32 RTC_Ext_Stop_Alarm_D(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0xBF, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}

	return D_RTC_EXT_OK;
}

/**
 @brief  Clear comparison status of alarm_d time.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit0 of Control_2 register is setted 0
 @attention	None
 */
INT32 RTC_Ext_Clear_Alarm_D_Status(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0xFE, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get comparison status of alarm_d time.
 @param  [O]E_RTC_EXT_ALARM_TIME_STATUS* const rtc_status
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		rtc_status:<br>
 				0:alarm_d setting time and current time is not same<br>
				1:alarm_d setting time and current time is same<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_Alarm_D_Status(E_RTC_EXT_ALARM_TIME_STATUS* const rtc_status)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_status == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_status] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_status = (E_RTC_EXT_ALARM_TIME_STATUS)(receiveData[0] & 0x01);
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Clear poweron reset status.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit4 of Control_2 register is setted 0
 @attention	None
 */
INT32 RTC_Ext_Clear_Poweron_Reset_Status(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0xEF, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get poweron reset status.
 @param  [O]E_RTC_EXT_POWERON_RESET_STATUS* const rtc_status
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		rtc_status:<br>
 				0:no detection of poweron reset<br>
				1:detection of poweron reset<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_Poweron_Reset_Status(E_RTC_EXT_POWERON_RESET_STATUS* const rtc_status)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_status == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_status] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_status = (E_RTC_EXT_POWERON_RESET_STATUS)((receiveData[0] & 0x10) >> 4);
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Clear crystal oscillation stop status.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit5 of Control_2 register is setted 1
 @attention	None
 */
INT32 RTC_Ext_Enable_Crystal_Stop_Check(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0x20, D_RTC_EXT_OP_OR) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get crystal oscillation stop status.
 @param  [O]E_RTC_EXT_CRYSTAL_STOP_STATUS* const rtc_status
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		rtc_status:<br>
 				0:no detection of crystal oscillation stop<br>
				1:detection of crystal oscillation stop<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_Crystal_Stop_Status(E_RTC_EXT_CRYSTAL_STOP_STATUS* const rtc_status)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_status == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_status] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_status = (E_RTC_EXT_CRYSTAL_STOP_STATUS)((receiveData[0] & 0x20) >> 5);
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Set low-voltage level value.
 @param  [I]E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE const* const rtc_value
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		0:low-voltage level value is 2.1V<br>
 			1:low-voltage level value is 1.3V<br>
 @attention	None
 */
INT32 RTC_Ext_Set_Low_Voltage_Level_Value(E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE const* const rtc_value)
{
#ifdef CO_PARAM_CHECK
	if(rtc_value == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_value] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (*rtc_value == E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE_1P3) {
		if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0x80, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}
	}
	else {
		if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0x7F, D_RTC_EXT_OP_OR) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get low-voltage level value.
 @param  [O]E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE* const rtc_value
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		0:low-voltage level value is 2.1V<br>
 			1:low-voltage level value is 1.3V<br>
 @attention	None
 */
INT32 RTC_Ext_Get_Low_Voltage_Level_Value(E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE* const rtc_value)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_value == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_value] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_value = (E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE)((receiveData[0] & 0x80) >> 7);
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Clear low-voltage status.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit6 of Control_2 register is setted 0
 @attention	None
 */
INT32 RTC_Ext_Clear_Low_Voltage_Level_Status(VOID)
{
	if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0xBF, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get low-voltage status.
 @param  [O]E_RTC_EXT_LOW_VOLTAGE_LEVEL_STATUS* const rtc_status
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		rtc_status:<br>
 				0:no detection of low-voltage<br>
				1:detection of low-voltage<br>
 @attention	None.
 */
INT32 RTC_Ext_Get_Low_Voltage_Level_Status(E_RTC_EXT_LOW_VOLTAGE_LEVEL_STATUS* const rtc_status)
{
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_status == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_status] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	*rtc_status = (E_RTC_EXT_LOW_VOLTAGE_LEVEL_STATUS)((receiveData[0] & 0x40) >> 6);
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Set FOUT output status.
 @param  [I]E_RTC_EXT_FOUT_STATUS const* const rtc_enable
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		0:FOUT output 32.768kHz<br>
			1:FOUT output OFF<br>
 @attention	None
 */
INT32 RTC_Ext_Set_FOUT_Enable(E_RTC_EXT_FOUT_STATUS const* const rtc_enable)
{
#ifdef CO_PARAM_CHECK
	if(rtc_enable == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_enable] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (*rtc_enable == E_RTC_EXT_FOUT_STATUS_ENABLE) {
		if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0xEF, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}
		if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0xF7, D_RTC_EXT_OP_AND) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}
	}
	else {
		if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL1, 0x10, D_RTC_EXT_OP_OR) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}
		if (rtc_ext_i2c_send(E_RTC_EXT_ADDR_CONTROL2, 0x08, D_RTC_EXT_OP_OR) != D_RTC_EXT_OK) {
			return D_RTC_EXT_NG;
		}
	}
	
	return D_RTC_EXT_OK;
}

/**
 @brief  Get FOUT output status.
 @param  [O]E_RTC_EXT_FOUT_STATUS* const rtc_enable
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		0:FOUT output 32.768kHz<br>
			1:FOUT output OFF<br>
 @attention	None
 */
INT32 RTC_Ext_Get_FOUT_Enable(E_RTC_EXT_FOUT_STATUS* const rtc_enable)
{
	UCHAR clen1;
	UCHAR clen2;
	UCHAR receiveData[16] = {0};
	
#ifdef CO_PARAM_CHECK
	if(rtc_enable == NULL){
		BF_Debug_Print_Error(("RTC input param error. [*rtc_enable] NULL\n"));
		return D_RTC_EXT_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK
	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL1, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	clen1 = ((receiveData[0] & 0x10) >> 4);

	if (rtc_ext_i2c_read(E_RTC_EXT_ADDR_CONTROL2, (BYTE*)&receiveData[0]) != D_RTC_EXT_OK) {
		return D_RTC_EXT_NG;
	}
	clen2 = ((receiveData[0] & 0x08) >> 3);

	if ((clen1 == 1) && (clen2 == 1)) {
		*rtc_enable = E_RTC_EXT_FOUT_STATUS_DISABLE;
	}
	else {
		*rtc_enable = E_RTC_EXT_FOUT_STATUS_ENABLE;
	}
	
	return D_RTC_EXT_OK;
}
