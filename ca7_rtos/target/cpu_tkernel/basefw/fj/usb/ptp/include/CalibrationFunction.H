/**
 * @file		CalibrationFunction.H
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _CALIBRATIONFUNCTION_HEADER_
#define _CALIBRATIONFUNCTION_HEADER_

//#define CALIBRATION_COMMANMD tonyho
#ifdef CALIBRATION_COMMANMD
UCHAR Calibration_PTPStartSensorSubSystem();
UCHAR Calibration_PTPEndSensorSubSystem();
UCHAR Calibration_PTPInitSensorSubSystem();
UCHAR Calibration_PTPExitSensorSubSystem();
UCHAR Calibration_PTPGetFirmwareVersion();
UCHAR Calibration_PTPGetSerialNum();
UCHAR Calibration_PTPSaveSerialNum();
UCHAR Calibration_PTPTakePhoto();
UCHAR Calibration_PTPGetJpeg();
UCHAR Calibration_PTPMBIL_GetRaw();
UCHAR Calibration_PTPCheckBadPixel();
UCHAR Calibration_PTPSendBadPixel();
UCHAR Calibration_PTPCheckZoomOffset();
UCHAR Calibration_PTPCheckFocusOffset();
UCHAR Calibration_PTPCheckFocusRange();
UCHAR Calibration_PTPCheckShutterDelay();
UCHAR Calibration_PTPCheckIris();
UCHAR Calibration_PTPCheckStrobe();
UCHAR Calibration_PTPCheckZ3GOffset();
UCHAR Calibration_PTPGetZoomOffset();
UCHAR Calibration_PTPGetFocusOffset();
UCHAR Calibration_PTPGetFocusRange();
UCHAR Calibration_PTPGetShutterDelay();
UCHAR Calibration_PTPGetZ3GOffset();
//=======================================================
// Calibration Command Defintion
//=======================================================
#define Calibration_GetFirmwareVersion			(0x9010)
#define Calibration_GetSerialNum				(0x9011)
#define Calibration_SaveSerialNum				(0x9012)

#define Calibration_TakePhoto					(0x9020)
#define Calibration_GetJpeg						(0x9021)
#define Calibration_GetRaw						(0x9022)
#define Calibration_StartSensorSubSystem		(0x9023)
#define Calibration_EndSensorSubSystem			(0x9024)


#define Calibration_CheckBadPixel				(0x9030)
#define Calibration_SendBadPixel				(0x9031)

#define Calibration_CheckZoomOffset				(0x9040)
#define Calibration_CheckFocusOffset			(0x9041)
#define Calibration_CheckFocusRange				(0x9042)
#define Calibration_CheckShutterDelay			(0x9043)
#define Calibration_CheckIris					(0x9044)
#define Calibration_CheckStrobe					(0x9045)
#define Calibration_CheckZ3GOffset				(0x9046)

#define Calibration_GetZoomOffset				(0x9050)
#define Calibration_GetFocusOffset				(0x9051)
#define Calibration_GetFocusRange				(0x9052)
#define Calibration_GetShutterDelay				(0x9053)
#define Calibration_GetZ3GOffset				(0x9056)

#define Calibration_User_Define1				(0x9060)
#define Calibration_User_Define2				(0x9061)
#define Calibration_User_Define3				(0x9062)
#endif

#endif
