#pragma once

#include "affine.h"
#include "coordsysaxis.h"
#include "coordsys.h"

#ifdef __cplusplus
extern "C" {
#endif

    // right-handed rule
    typedef enum {
        PoseAxis_X = CoordSysAxis_X,
        PoseAxis_Y = CoordSysAxis_Y,
        PoseAxis_Z = CoordSysAxis_Z,
        PoseAxis_DefaultFront = PoseAxis_Z,
        PoseAxis_DefaultUp    = PoseAxis_Y,
        PoseAxis_DefaultLeft  = PoseAxis_X
    } PoseAxis;

    PoseAxis PoseAxis_Front2Up(PoseAxis front);
    PoseAxis PoseAxis_Front2Left(PoseAxis front);
    PoseAxis PoseAxis_Up2Front(PoseAxis up);
    PoseAxis PoseAxis_Left2Front(PoseAxis left);

    typedef struct {
        PoseAxis    front;
        PoseAxis    up;
        PoseAxis    left;
        PoseAxis    pole;
        XXXX_FLOAT  objdistance;
        CoordSys    coordsys;
    } Pose;

    void Pose_Init(Pose *pose, PoseAxis front, PoseAxis pole, Affine *poseinworld, XXXX_FLOAT objscale);
    void Pose_Update(Pose *pose, Affine *poseinworld, XXXX_FLOAT objscale);

#ifdef __cplusplus
}
#endif
