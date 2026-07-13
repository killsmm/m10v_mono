#pragma once

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

    void accel2pose(float ax, float ay, float az, float ax0, float ay0, float az0, float posemat[][3]);

#ifdef __cplusplus
}
#endif
