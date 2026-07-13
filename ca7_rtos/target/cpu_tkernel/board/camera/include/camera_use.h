#ifndef _CAMERA_USE_H
#define _CAMERA_USE_H
#include "camera_config.h"
#define USE_SENSOR (SENSOR_IMX571)
#define USE_GYRO (GYRO_ICM20948)
#define EIS_X (-GYRO_X)
#define EIS_Y (GYRO_Y)
#define EIS_Z (-GYRO_Z)
#define USE_FOCUS (FOCUS_NONE)
#endif
