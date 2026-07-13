#pragma once

#include "affine.h"
#include "dist.h"
#include "coordsys.h"
#include "param.h"
#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        ProjType_Lens = 0,
        ProjType_Equirectangular = 1,
        ProjType_Cylindrical = 2,
        ProjType_Rectilinear = 3,
    } ProjType;

    PoseAxis ProjType_Front2Pole(ProjType projtype, PoseAxis front);
    PoseAxis ProjType_Pole2Front(ProjType projtype, PoseAxis pole);

    typedef struct Proj Proj;
    typedef void(*ProjForward)(Proj *proj, Vec3 *src3, Vec2 *dst2);
    typedef void(*ProjInverse)(Proj *proj, Vec2 *src2, Vec3 *dst3);

    struct Proj {
        ProjType        type;
        Dist            dist;
        ProjForward     forward;
        ProjInverse     inverse;
        CoordSys        *coordsys;
    };

    void Proj_Init(Proj *proj, ProjType type, ParamDist *distparam, Pose *pose);

    void Proj_Inverse_Lens(Proj *proj, Vec2 *src2, Vec3 *dst3);
    void Proj_Inverse_Equirectangular(Proj *proj, Vec2 *src2, Vec3 *dst3);
    void Proj_Inverse_Cylindrical(Proj *proj, Vec2 *src2, Vec3 *dst3);
    void Proj_Inverse_Rectilinear(Proj *proj, Vec2 *src2, Vec3 *dst3);

    void Proj_Forward_Lens(Proj *proj, Vec3 *src3, Vec2 *dst2);
    void Proj_Forward_Equirectangular(Proj *proj, Vec3 *src3, Vec2 *dst2);
    void Proj_Forward_Cylindrical(Proj *proj, Vec3 *src3, Vec2 *dst2);
    void Proj_Forward_Rectilinear(Proj *proj, Vec3 *src3, Vec2 *dst2);

    typedef struct {
        Pose        pose;
        Proj        proj;
        Image       image;
        XXXX_FLOAT  objdistance;
    } Camera;

    void Camera_Init(Camera *camera, ParamCamera *param, XXXX_FLOAT objscale);
    void Camera_UpdatePose(Camera *camera, Affine *poseinworld, XXXX_FLOAT objscale);
    void Camera_ProjInverse(Camera *camera, Vec2 *src2, Vec3 *dst3);
    void Camera_ProjForward(Camera *camera, Vec3 *src3, Vec2 *dst2);

    typedef struct {
        Camera          *src;
        Camera          *dst;
        Vec2            srcorg;
        Vec2            dstorg;
        CoordSysPair    coordsyspair;
    } CameraPair;

    void CameraPair_Init(CameraPair *camerapair, Camera *src, Camera *dst, Vec2 *srcorg, Vec2 *dstorg);
    void CameraPair_UpdateDstPose(CameraPair *camerapair, Affine *poseinworld, XXXX_FLOAT objdistance);
    void CameraPair_ProjInverse(CameraPair *camerapair, Vec2 *ondst, Vec2 *onsrc);
    void CameraPair_ProjInverse_GetSrc3(CameraPair *camerapair, Vec2 *ondst, Vec3 *onsrc3);

#ifdef __cplusplus
}
#endif
