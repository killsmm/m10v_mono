#include "coordsysaxis.h"
#include <stdio.h>

CoordSysAxis CoordSysAxis_CycleForward(CoordSysAxis axis)
{
    CoordSysAxis next = CoordSysAxis_Y;
    switch (axis) {
    case CoordSysAxis_X:
        next = CoordSysAxis_Y;
        break;
    case CoordSysAxis_Y:
        next = CoordSysAxis_Z;
        break;
    case CoordSysAxis_Z:
        next = CoordSysAxis_X;
        break;
    default:
        printf("[ERROR] CoordSysAxis_CycleForward : unknown axis (%d)\n", axis);
    }
    return next;
}

CoordSysAxis CoordSysAxis_CycleReverse(CoordSysAxis axis)
{
    CoordSysAxis prev = CoordSysAxis_Z;
    switch (axis) {
    case CoordSysAxis_X:
        prev = CoordSysAxis_Z;
        break;
    case CoordSysAxis_Y:
        prev = CoordSysAxis_X;
        break;
    case CoordSysAxis_Z:
        prev = CoordSysAxis_Y;
        break;
    default:
        printf("[ERROR] CoordSysAxis_CycleReverse : unknown axis (%d)\n", axis);
    }
    return prev;
}

CoordSysAxis CoordSysAxis_Pole2PlaneX(CoordSysAxis pole)
{
    return CoordSysAxis_CycleForward(pole);
}

CoordSysAxis CoordSysAxis_Pole2PlaneY(CoordSysAxis pole)
{
    return CoordSysAxis_CycleReverse(pole);
}
