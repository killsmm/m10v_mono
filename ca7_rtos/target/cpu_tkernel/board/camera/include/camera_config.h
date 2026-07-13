/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_CONFIG_H
#define _CAMERA_CONFIG_H

#include "ddim_typedef.h"
#include "camera_common.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define SENSOR_IMX477	0
#define SENSOR_IMX277	1
#define SENSOR_IMX383	2
#define SENSOR_IMX533	3
#define SENSOR_IMX183	4
#define SENSOR_IMX363	5
#define SENSOR_IMX290   6
#define SENSOR_OS08A10  7
#define SENSOR_AR0430	8
#define SENSOR_IMX377	9
#define SENSOR_OV12895	10
#define SENSOR_IMX577	11
#define SENSOR_IMX269	12
#define SENSOR_IMX571	13
#define SENSOR_IMX477_IMX183	0xFD
#define SENSOR_PSEUDO	0xFE
#define SENSOR_NONE		0xFF

// Gyro
#define GYRO_ICM20948	0
#define GYRO_MPU9250	1
#define GYRO_ICM20602	2
#define GYRO_NONE		0xFF

// FOCUS
#define FOCUS_ZC525		0
#define FOCUS_M43       1
#define FOCUS_F103T     2 
#define FOCUS_NONE		0xFF



// EIS
#define GYRO_X	(0+1)
#define GYRO_Y	(1+1)
#define GYRO_Z	(2+1)

// Timer
#define TIMER64_START_FROM_BOOT

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA CAMERA;
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern char* Camera_Get_SensorType(void);

#endif
