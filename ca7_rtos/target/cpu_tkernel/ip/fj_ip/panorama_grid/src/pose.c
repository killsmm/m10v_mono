#include "pose.h"

PoseAxis PoseAxis_Front2Up(PoseAxis front)
{
    return CoordSysAxis_CycleReverse(front);    // x -> z, y -> x, z -> y
}

PoseAxis PoseAxis_Front2Left(PoseAxis front)
{
    return CoordSysAxis_CycleForward(front);    // x -> y, y -> z, z -> x
}

PoseAxis PoseAxis_Up2Front(PoseAxis up)
{
    return CoordSysAxis_CycleForward(up);
}

PoseAxis PoseAxis_Left2Front(PoseAxis left)
{
    return CoordSysAxis_CycleReverse(left);
}

void Pose_Init(Pose *pose, PoseAxis front, PoseAxis pole, Affine *poseinworld, XXXX_FLOAT objscale)
{
    pose->front = front;
    pose->up    = PoseAxis_Front2Up(front);
    pose->left  = PoseAxis_Front2Left(front);
    pose->pole  = pole;

    CoordSys_Init(&pose->coordsys, poseinworld, objscale, pole);
}

void Pose_Update(Pose *pose, Affine *poseinworld, XXXX_FLOAT objscale)
{
    CoordSys_SetPose(&pose->coordsys, poseinworld, objscale);
}
