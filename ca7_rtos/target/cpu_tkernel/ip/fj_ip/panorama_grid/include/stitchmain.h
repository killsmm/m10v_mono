#pragma once

#include "stitch.h"

#ifdef __cplusplus
extern "C" {
#endif

    #define STITCH_IIP_LESS_OP

    #define STITCH_MARGIN  288
#if 1
    #define STITCH_WIDTH    64 //  32 pixels = fov  6 degs for 3840x1920 output
#else
    #define STITCH_WIDTH   320 // 320 pixels = fov 30 degs for 3840x1920 output
#endif

    void Stitch_Main(int stitch_width, Stitch *stitch, StitchGrids *stitchin0, StitchGrids *stitchin1);
    void Stitch_Final(float ax, float ay, float az, float ax0, float ay0, float az0, Stitch *stitch, StitchGrids *stitchfinal);

#ifdef __cplusplus
}
#endif
