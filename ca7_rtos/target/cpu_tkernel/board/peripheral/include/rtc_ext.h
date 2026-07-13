/**
 * @file		rtc_ext.h
 * @brief		External Real Time Clock driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup rtc_ext
@{
*/

#ifndef _RTC_EXT_H_
#define _RTC_EXT_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_RTC_EXT_SLAVE_ADDRESS		(0x32)	/**< External RTC Slave Address. */
#define D_RTC_EXT_I2C_CHANNEL_NO   	(2)		/**< I2C channel. */
#define D_RTC_EXT_YEAR_BASE			(2000)	/**< External RTC Year offset. */
#define D_RTC_EXT_OK				(0)		/**< Processing OK. */
#define D_RTC_EXT_NG				(1)		/**< Processing NG. */
#define D_RTC_EXT_INPUT_PARAM_ERROR	(2)		/**< Input parameter error. */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** External RTC Registers address */
typedef enum {
	E_RTC_EXT_ADDR_SECONDS			= 0,							/**< Address of Seconds */
	E_RTC_EXT_ADDR_MINUTES			= 1,							/**< Address of Minutes */
	E_RTC_EXT_ADDR_HOURS			= 2,							/**< Address of Hours */
	E_RTC_EXT_ADDR_WEEKDAYS			= 3,							/**< Address of Weekdays */
	E_RTC_EXT_ADDR_DAYS				= 4,							/**< Address of Days */
	E_RTC_EXT_ADDR_MONTHS			= 5,							/**< Address of Months */
	E_RTC_EXT_ADDR_YEARS			= 6,							/**< Address of Years */
	E_RTC_EXT_ADDR_DIGITAL_OFFSET	= 7,							/**< Address of Digital Offset */
	E_RTC_EXT_ADDR_ALARMW_MINUTE	= 8,							/**< Address of Alarm_W Minute */
	E_RTC_EXT_ADDR_ALARMW_HOUR		= 9,							/**< Address of Alarm_W Hour */
	E_RTC_EXT_ADDR_ALARMW_WEEKDAY	= 10,							/**< Address of Alarm_W Weekday */
	E_RTC_EXT_ADDR_ALARMD_MINUTE	= 11,							/**< Address of Alarm_D Minute */
	E_RTC_EXT_ADDR_ALARMD_HOUR		= 12,							/**< Address of Alarm_D Hour */
	E_RTC_EXT_ADDR_RESERVED			= 13,							/**< Address of Reserved */
	E_RTC_EXT_ADDR_CONTROL1			= 14,							/**< Address of Control 1 */
	E_RTC_EXT_ADDR_CONTROL2			= 15							/**< Address of Control 2 */
} E_RTC_EXT_ADDR;

/** Constant periodic interruption output state of /INTA */
typedef enum {
	E_RTC_EXT_INTA_OUTPUT_STATUS_OFF = 0,							/**< /INTA output status is OFF	*/
	E_RTC_EXT_INTA_OUTPUT_STATUS_ON									/**< /INTA output status is ON	*/
} E_RTC_EXT_INTA_OUTPUT_STATUS;

/** /INTA Output Mode */
typedef enum {
	E_RTC_EXT_INTA_OUTPUT_MODE_INTA_OFF = 0,						/**< /INTA=Hi-z(=OFF)	*/
	E_RTC_EXT_INTA_OUTPUT_MODE_INTA_L,								/**< /INTA="L"	*/
	E_RTC_EXT_INTA_OUTPUT_MODE_2HZ,									/**< 2Hz(Duty 50%)	*/
	E_RTC_EXT_INTA_OUTPUT_MODE_1HZ,									/**< 1Hz(Duty 50%)	*/
	E_RTC_EXT_INTA_OUTPUT_MODE_SECOND,								/**< per-second	*/
	E_RTC_EXT_INTA_OUTPUT_MODE_MINUTE,								/**< per-minute	*/
	E_RTC_EXT_INTA_OUTPUT_MODE_HOUR,								/**< per-hour	*/
	E_RTC_EXT_INTA_OUTPUT_MODE_MONTH								/**< per-month	*/
} E_RTC_EXT_INTA_OUTPUT_MODE;

/** Hours Time Mode */
typedef enum {
	E_RTC_EXT_HOURS_MODE_12 = 0,									/**< 12 hours time system	*/
	E_RTC_EXT_HOURS_MODE_24											/**< 24 hours time system	*/
} E_RTC_EXT_HOURS_MODE;

/** Comparison status of time */
typedef enum {
	E_RTC_EXT_ALARM_AND_CURRENT_TIME_NOT_SAME = 0,					/**< setting time and current time is not same	*/
	E_RTC_EXT_ALARM_AND_CURRENT_TIME_SAME							/**< setting time and current time is same	*/
} E_RTC_EXT_ALARM_TIME_STATUS;

/** Poweron Reset Status */
typedef enum {
	E_RTC_EXT_POWERON_RESET_NO = 0,									/**< no detection of poweron reset	*/
	E_RTC_EXT_POWERON_RESET_YES										/**< detection of poweron reset	*/
} E_RTC_EXT_POWERON_RESET_STATUS;

/** Crystal Oscillation Stop Status */
typedef enum {
	E_RTC_EXT_CRYSTAL_STOP_YES = 0,									/**< detection of crystal oscillation stop	*/
	E_RTC_EXT_CRYSTAL_STOP_NO										/**< no detection of crystal oscillation stop	*/
} E_RTC_EXT_CRYSTAL_STOP_STATUS;

/** Low-Voltage Level Value */
typedef enum {
	E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE_2P1 = 0,						/**< low-voltage level value = 2.1V	*/
	E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE_1P3							/**< low-voltage level value = 1.3V	*/
} E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE;

/** Low-Voltage Status */
typedef enum {
	E_RTC_EXT_LOW_VOLTAGE_NO = 0,									/**< no detection of low-voltage	*/
	E_RTC_EXT_LOW_VOLTAGE_YES										/**< detection of low-voltage	*/
} E_RTC_EXT_LOW_VOLTAGE_LEVEL_STATUS;

/** FOUT Output */
typedef enum {
	E_RTC_EXT_FOUT_STATUS_ENABLE = 0,								/**< FOUT output enable	*/
	E_RTC_EXT_FOUT_STATUS_DISABLE									/**< FOUT output disable	*/
} E_RTC_EXT_FOUT_STATUS;

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/**	Date & Time information. */
typedef struct {
	USHORT	YEAR;			/**< Year ( 2001 to 2099 ) */
	UCHAR	MONTH;			/**< Month ( 1 to 12) */
	UCHAR	DAY;			/**< Day ( (1to28) or (1to29) or (1to30) or (1to31) ) */
	UCHAR	HOUR;			/**< Hour ( 0 to 23 ) */
	UCHAR	MINUTE;			/**< Minite ( 0 to 59 ) */
	UCHAR	SECOND;			/**< Second ( 0 to 59 ) */
	UCHAR	WEEKDAY;		/**< Day of the week ( 0 to 6 ) */
} T_RTC_EXT_DATA;

/** Alarm time information. */
typedef struct {
	UCHAR		HOUR;		/**< Hour ( 0 to 23 ) */
	UCHAR		MINUTE;		/**< Minute ( 0 to 59 ) */
	UCHAR		WEEKDAY;	/**< Weekday ( 0 to 127 ) */
} T_RTC_EXT_ALARM_W;

/** Alarm time information. */
typedef struct {
	UCHAR		HOUR;		/**< Hour ( 0 to 23 ) */
	UCHAR		MINUTE;		/**< Minute ( 0 to 59 ) */
} T_RTC_EXT_ALARM_D;

/*----------------------------------------------------------------------*/
/* Grobal Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

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
extern INT32 RTC_Ext_Init(VOID);

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
extern INT32 RTC_Ext_Set_Time(T_RTC_EXT_DATA const* const rtc_data);

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
extern INT32 RTC_Ext_Get_Time(T_RTC_EXT_DATA* const rtc_data);

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
extern INT32 RTC_Ext_Set_Digital_Offset(INT8 const* const rtc_offset);

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
extern INT32 RTC_Ext_Get_Digital_Offset(signed char* const rtc_offset);

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
extern INT32 RTC_Ext_Set_Alarm_W(T_RTC_EXT_ALARM_W const* const rtc_alarm);

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
extern INT32 RTC_Ext_Get_Alarm_W(T_RTC_EXT_ALARM_W* const rtc_alarm);

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
extern INT32 RTC_Ext_Set_Alarm_D(T_RTC_EXT_ALARM_D const* const rtc_alarm);

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
extern INT32 RTC_Ext_Get_Alarm_D(T_RTC_EXT_ALARM_D* const rtc_alarm);

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
extern INT32 RTC_Ext_Set_Ctrl1(UCHAR const* const rtc_ctrl);

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
extern INT32 RTC_Ext_Get_Ctrl1(UCHAR* const rtc_ctrl);

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
extern INT32 RTC_Ext_Set_Ctrl2(UCHAR const* const rtc_ctrl);

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
extern INT32 RTC_Ext_Get_Ctrl2(UCHAR* const rtc_ctrl);

/**
 @brief  Clear constant periodic interruption output state of /INTA.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit2 of Control_2 register is setted 0
 @attention	None
 */
extern INT32 RTC_Ext_Clear_INTA_Status(VOID);

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
extern INT32 RTC_Ext_Get_INTA_Status(E_RTC_EXT_INTA_OUTPUT_STATUS* const rtc_status);

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
extern INT32 RTC_Ext_Set_INTA_Output_Mode(E_RTC_EXT_INTA_OUTPUT_MODE const* const rtc_mode);

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
extern INT32 RTC_Ext_Get_INTA_Output_Mode(E_RTC_EXT_INTA_OUTPUT_MODE* const rtc_mode);

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
extern INT32 RTC_Ext_Set_Hours_Mode(E_RTC_EXT_HOURS_MODE const* const rtc_hours_mode);

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
extern INT32 RTC_Ext_Get_Hours_Mode(E_RTC_EXT_HOURS_MODE* const rtc_hours_mode);

/**
 @brief  Start comparison of alarm_w time information.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit7 of Control_1 register is setted 1
 @attention	None
 */
extern INT32 RTC_Ext_Start_Alarm_W(VOID);

/**
 @brief  Stop comparison of alarm_w time information.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit7 of Control_1 register is setted 0
 @attention	None
 */
extern INT32 RTC_Ext_Stop_Alarm_W(VOID);

/**
 @brief  Clear comparison status of alarm_w time.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit1 of Control_2 register is setted 0
 @attention	None
 */
extern INT32 RTC_Ext_Clear_Alarm_W_Status(VOID);

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
extern INT32 RTC_Ext_Get_Alarm_W_Status(E_RTC_EXT_ALARM_TIME_STATUS* const rtc_status);

/**
 @brief  Start comparison of alarm_d time information.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit6 of Control_1 register is setted 1
 @attention	None
 */
extern INT32 RTC_Ext_Start_Alarm_D(VOID);

/**
 @brief  Stop comparison of alarm_d time information.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit6 of Control_1 register is setted 0
 @attention	None
 */
extern INT32 RTC_Ext_Stop_Alarm_D(VOID);

/**
 @brief  Clear comparison status of alarm_d time.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit0 of Control_2 register is setted 0
 @attention	None
 */
extern INT32 RTC_Ext_Clear_Alarm_D_Status(VOID);

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
extern INT32 RTC_Ext_Get_Alarm_D_Status(E_RTC_EXT_ALARM_TIME_STATUS* const rtc_status);

/**
 @brief  Clear poweron reset status.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit4 of Control_2 register is setted 0
 @attention	None
 */
extern INT32 RTC_Ext_Clear_Poweron_Reset_Status(VOID);

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
extern INT32 RTC_Ext_Get_Poweron_Reset_Status(E_RTC_EXT_POWERON_RESET_STATUS* const rtc_status);

/**
 @brief  Clear crystal oscillation stop status.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit5 of Control_2 register is setted 1
 @attention	None
 */
extern INT32 RTC_Ext_Enable_Crystal_Stop_Check(VOID);

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
extern INT32 RTC_Ext_Get_Crystal_Stop_Status(E_RTC_EXT_CRYSTAL_STOP_STATUS* const rtc_status);

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
extern INT32 RTC_Ext_Set_Low_Voltage_Level_Value(E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE const* const rtc_value);

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
extern INT32 RTC_Ext_Get_Low_Voltage_Level_Value(E_RTC_EXT_LOW_VOLTAGE_LEVEL_VALUE* const rtc_value);

/**
 @brief  Clear low-voltage status.
 @param  None
 @retval D_RTC_EXT_OK					: OK
 @retval D_RTC_EXT_NG					: NG
 @retval D_RTC_EXT_INPUT_PARAM_ERROR	: Input parameter error
 @note		bit6 of Control_2 register is setted 0
 @attention	None
 */
extern INT32 RTC_Ext_Clear_Low_Voltage_Level_Status(VOID);

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
extern INT32 RTC_Ext_Get_Low_Voltage_Level_Status(E_RTC_EXT_LOW_VOLTAGE_LEVEL_STATUS* const rtc_status);

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
extern INT32 RTC_Ext_Set_FOUT_Enable(E_RTC_EXT_FOUT_STATUS const* const rtc_enable);

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
extern INT32 RTC_Ext_Get_FOUT_Enable(E_RTC_EXT_FOUT_STATUS* const rtc_enable);

/*@}*/
/*@}*/

#ifdef __cplusplus
}
#endif

#endif

/**
@weakgroup rtc_ext
@{
	
	<hr>
	@section RTC_Sample Sample of RTC
	@code

	// Initialization
	RTC_Ext_Init();

	// Set time for 12:34:56 of Tuesday, February 10, 2015
	T_RTC_EXT_DATA rtc_data;
	rtc_data.YEAR		= 2015;
	rtc_data.MONTH		= 2;
	rtc_data.DAY		= 10;
	rtc_data.HOUR		= 12;
	rtc_data.MINUTE		= 34;
	rtc_data.SECOND		= 56;
	rtc_data.WEEKDAY	= 2;
	ret = RTC_Ext_Set_Time( &rtc_data );

	// Set alarm_w for 2:30 per Monday.
	T_RTC_EXT_ALARM_W rtc_alarm;
	rtc_alarm.HOUR      = 2;
	rtc_alarm.MINUTE    = 30;
	rtc_alarm.WEEKDAY   = 2;
	RTC_Ext_Set_Alarm_W( &rtc_alarm );
	RTC_Ext_Start_Alarm_W();

	@endcode
@}*/

