#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        CoordSysAxis_X = 0,
        CoordSysAxis_Y = 1,
        CoordSysAxis_Z = 2,
        CoordSysAxis_DefaultPole = CoordSysAxis_Z
    } CoordSysAxis;

    CoordSysAxis CoordSysAxis_CycleForward(CoordSysAxis axis);
    CoordSysAxis CoordSysAxis_CycleReverse(CoordSysAxis axis);
    CoordSysAxis CoordSysAxis_Pole2PlaneX(CoordSysAxis pole);
    CoordSysAxis CoordSysAxis_Pole2PlaneY(CoordSysAxis pole);

#ifdef __cplusplus
}
#endif
