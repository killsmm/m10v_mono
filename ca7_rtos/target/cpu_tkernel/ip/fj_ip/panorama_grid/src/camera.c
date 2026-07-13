#include "camera.h"
#include "common.h"
#include "coordsys.h"
#include "mem.h"
#include <math.h>
#include <stdio.h>

PoseAxis ProjType_Front2Pole(ProjType projtype, PoseAxis front)
{
    ProjType pole = PoseAxis_X;
    switch (projtype) {
    case ProjType_Lens:
    case ProjType_Rectilinear:
        pole = front;
        break;
    case ProjType_Equirectangular:
    case ProjType_Cylindrical:
        pole = PoseAxis_Front2Up(front);
        break;
    default:
        printf("[ERROR] %s : unknown projtype (%d)\n", __func__, projtype); /* pgr0822 */
        break;
    }
    return pole;
}

PoseAxis ProjType_Pole2Front(ProjType projtype, PoseAxis pole)
{
    ProjType front = PoseAxis_X;
    switch (projtype) {
    case ProjType_Lens:
    case ProjType_Rectilinear:
        front = pole;
        break;
    case ProjType_Equirectangular:
    case ProjType_Cylindrical:
        front = PoseAxis_Up2Front(pole);
        break;
    default:
        printf("[ERROR] %s : unknown projtype (%d)\n", __func__, projtype); /* pgr0822 */
        break;
    }
    return front;
}

void Proj_Init(Proj *proj, ProjType projtype, ParamDist *distparam, Pose *pose)
{
    proj->type      = projtype;
    proj->coordsys  = &pose->coordsys;

    switch (projtype) {
    case ProjType_Lens:
        proj->forward = Proj_Forward_Lens;
        proj->inverse = Proj_Inverse_Lens;
        break;
    case ProjType_Equirectangular:
        proj->forward = Proj_Forward_Equirectangular;
        proj->inverse = Proj_Inverse_Equirectangular;
        break;
    case ProjType_Cylindrical:
        proj->forward = Proj_Forward_Cylindrical;
        proj->inverse = Proj_Inverse_Cylindrical;
        break;
    case ProjType_Rectilinear:
        proj->forward = Proj_Forward_Rectilinear;
        proj->inverse = Proj_Inverse_Rectilinear;
        break;
    default:
        printf("[ERROR] %s : unknown projtype (%d)\n", __func__, projtype); /* pgr0822 */
        break;
    }

    if (distparam != NULL) {
        Dist_Init(&proj->dist, distparam->input, distparam->output, distparam->tablelength, distparam->inputScalerToRad, distparam->outputScalerToPix);
    }
}

void Proj_Inverse_Lens(Proj *proj, Vec2 *src, Vec3 *dst)
{
}

void Proj_Inverse_Equirectangular(Proj *proj, Vec2 *src, Vec3 *dst)
{
    CoordGeo geo;
    geo.r = 1.0;
    geo.lon = src->x;
    geo.lat = src->y;
    CoordSys_Geo2XYZ(proj->coordsys, &geo, dst);
}

void Proj_Inverse_Cylindrical(Proj *proj, Vec2 *src, Vec3 *dst)
{
    XXXX_FLOAT lon = src->x;
    dst->v[0] = COS(lon);
    dst->v[1] = SIN(lon);
    dst->v[2] = src->y;
}

void Proj_Inverse_Rectilinear(Proj *proj, Vec2 *src, Vec3 *dst)
{
    dst->v[0] = 1;
    dst->v[1] = src->x;
    dst->v[2] = src->y;
}

void Proj_Forward_Lens(Proj *proj, Vec3 *src, Vec2 *dst)
{
    CoordSph sph;
    CoordSys_XYZ2Sph(proj->coordsys, src, &sph);
    XXXX_FLOAT r = Dist_ProjForward(&proj->dist, sph.polar);
    dst->x = r * COS(sph.azimuth);
    dst->y = r * SIN(sph.azimuth);
}

void Proj_Forward_Equirectangular(Proj *proj, Vec3 *src, Vec2 *dst)
{
    CoordGeo geo;
    CoordSys_XYZ2Geo(proj->coordsys, src, &geo);
    dst->x = geo.lon;
    dst->y = geo.lat;
}

void Proj_Forward_Cylindrical(Proj *proj, Vec3 *src, Vec2 *dst)
{
}

void Proj_Forward_Rectilinear(Proj *proj, Vec3 *src, Vec2 *dst)
{
}

void Camera_Init(Camera *camera, ParamCamera *param, XXXX_FLOAT objscale)
{
    CoordSysAxis pole = ProjType_Front2Pole(param->projtype, param->front);
    Pose_Init(&camera->pose, param->front, pole, &param->poseinworld, objscale);
    Proj_Init(&camera->proj, param->projtype, &param->dist, &camera->pose);
    Image_Init(&camera->image, param->image.w, param->image.h, param->image.centerx, param->image.centery, param->image.scalepixw, param->image.scaledegw);
}

void Camera_UpdatePose(Camera *camera, Affine *poseinworld, XXXX_FLOAT objscale)
{
    Pose_Update(&camera->pose, poseinworld, objscale);
}

void Camera_ProjInverse(Camera *camera, Vec2 *src2, Vec3 *dst3)
{
    camera->proj.inverse(&camera->proj, src2, dst3);
}

void Camera_ProjForward(Camera *camera, Vec3 *src3, Vec2 *dst2)
{
    camera->proj.forward(&camera->proj, src3, dst2);
}

void CameraPair_Init(CameraPair *camerapair, Camera *src, Camera *dst, Vec2 *srcorg, Vec2 *dstorg)
{
    camerapair->src = src;
    camerapair->dst = dst;
    camerapair->srcorg = *srcorg;
    camerapair->dstorg = *dstorg;
    CoordSysPair_Init(&camerapair->coordsyspair, &src->pose.coordsys, &dst->pose.coordsys);
}

void CameraPair_UpdateDstPose(CameraPair *camerapair, Affine *poseinworld, XXXX_FLOAT objdistance)
{
    Camera_UpdatePose(camerapair->dst, poseinworld, objdistance);
    CoordSysPair_Compose(&camerapair->coordsyspair);
}

void CameraPair_ProjInverse(CameraPair *camerapair, Vec2 *ondst, Vec2 *onsrc)
{
    Vec2 ondstn, onsrcn;
    Vec3 dst3, src3;
    Image_Coord_Normalize(&camerapair->dst->image, ondst, &camerapair->dstorg, &ondstn);
    Camera_ProjInverse(camerapair->dst, &ondstn, &dst3);
#if 0
    Vec3 world;
    CoordSys_Local2World(&camerapair->dst->pose.coordsys, &dst3, &world);
    CoordSys_World2Local(&camerapair->src->pose.coordsys, &world, &src3);
#else
    CoordSysPair_ChangeBasis_Inverse(&camerapair->coordsyspair, &dst3, &src3);
#endif
    Camera_ProjForward(camerapair->src, &src3, &onsrcn);
    Image_Coord_Unnormalize(&camerapair->src->image, &onsrcn, &camerapair->srcorg, onsrc);
}

void CameraPair_ProjInverse_GetSrc3(CameraPair *camerapair, Vec2 *ondst, Vec3 *onsrc3)
{
    Vec2 ondstn;
    Vec3 dst3;
    Image_Coord_Normalize(&camerapair->dst->image, ondst, &camerapair->dstorg, &ondstn);
    Camera_ProjInverse(camerapair->dst, &ondstn, &dst3);
    CoordSysPair_ChangeBasis_Inverse(&camerapair->coordsyspair, &dst3, onsrc3);
}
