#ifndef __CAMERA_SENSOR_SHT30_H__
#define _CAMERA_SENSOR_SHT30_H__
#include "dd_top.h"

int SHT30_Init();
int SHT30_Read_Result(UINT32* temperature, UINT32* humidity);

#endif // _CAMERA_SENSOR_SHT30_H__