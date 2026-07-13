#include "coordsys.h"
#include "coordsysaxis.h"
#include "common.h"
#include "grid.h"
#include "camera.h"
#include <math.h>

void CoordSys_Init(CoordSys *coordsys, Affine *poseinworld, XXXX_FLOAT objscale, CoordSysAxis axPole)
{
    coordsys->poseinworld = &coordsys->local2world;
    CoordSys_SetPose(coordsys, poseinworld, objscale);
    coordsys->axPole   = axPole;
    coordsys->axPlaneX = CoordSysAxis_Pole2PlaneX(axPole);
    coordsys->axPlaneY = CoordSysAxis_Pole2PlaneY(axPole);
}

void CoordSys_SetPose(CoordSys *coordsys, Affine *poseinworld, XXXX_FLOAT objscale)
{
#if 0
    Affine_Copy(&coordsys->local2world, poseinworld);
#else
    Affine_Copy(&coordsys->local2world, poseinworld);
    Vec3_MulScalarToSrc(&coordsys->local2world.t, 1/objscale);
#endif
    Affine_GetInv(&coordsys->local2world, &coordsys->world2local);
}

XXXX_FLOAT CoordSys_GetVecElem_Pole(CoordSys *coordsys, Vec3 *xyz)
{
    return xyz->v[coordsys->axPole];
}

XXXX_FLOAT CoordSys_GetVecElem_PlaneX(CoordSys *coordsys, Vec3 *xyz)
{
    return xyz->v[coordsys->axPlaneX];
}

XXXX_FLOAT CoordSys_GetVecElem_PlaneY(CoordSys *coordsys, Vec3 *xyz)
{
    return xyz->v[coordsys->axPlaneY];
}

XXXX_FLOAT CoordGeo_GetLatitude_PoleZ(XXXX_FLOAT z, XXXX_FLOAT r)
{
    return ASIN(z / r); // [-pi/2, pi/2]
}

XXXX_FLOAT CoordGeo_GetLongitude_PoleZ(XXXX_FLOAT x, XXXX_FLOAT y, XXXX_FLOAT r)
{
    return ATAN2(y / r, x / r); // [-pi, pi]
}

void CoordSys_XYZ2Geo(CoordSys *coordsys, Vec3 *xyz, CoordGeo *geo)
{
    XXXX_FLOAT pole   = xyz->v[coordsys->axPole  ];
    XXXX_FLOAT planex = xyz->v[coordsys->axPlaneX];
    XXXX_FLOAT planey = xyz->v[coordsys->axPlaneY];
    geo->r   = Vec3_GetNorm(xyz);
    geo->lat = CoordGeo_GetLatitude_PoleZ(pole, geo->r);
    geo->lon = CoordGeo_GetLongitude_PoleZ(planex, planey, geo->r);
}

void CoordSys_Geo2XYZ(CoordSys *coordsys, CoordGeo *geo, Vec3 *xyz)
{
    xyz->v[coordsys->axPole  ] = geo->r * SIN(geo->lat);
    xyz->v[coordsys->axPlaneX] = geo->r * COS(geo->lat) * COS(geo->lon);
    xyz->v[coordsys->axPlaneY] = geo->r * COS(geo->lat) * SIN(geo->lon);
}

XXXX_FLOAT CoordSph_GetPolar_PoleZ(XXXX_FLOAT z, XXXX_FLOAT r)
{
    return DEG2RAD(90) - CoordGeo_GetLatitude_PoleZ(z, r);    // [0, pi]
}

XXXX_FLOAT CoordSph_GetAzimuth_PoleZ(XXXX_FLOAT x, XXXX_FLOAT y, XXXX_FLOAT r)
{
    return CoordGeo_GetLongitude_PoleZ(x, y, r); // [-pi, pi]
}

void CoordSys_XYZ2Sph(CoordSys *coordsys, Vec3 *xyz, CoordSph *sph)
{
    XXXX_FLOAT pole   = xyz->v[coordsys->axPole  ];
    XXXX_FLOAT planex = xyz->v[coordsys->axPlaneX];
    XXXX_FLOAT planey = xyz->v[coordsys->axPlaneY];
    sph->r       = Vec3_GetNorm(xyz);
    sph->polar   = CoordSph_GetPolar_PoleZ(pole, sph->r);
    sph->azimuth = CoordSph_GetAzimuth_PoleZ(planex, planey, sph->r);
}

void CoordSys_Sph2XYZ(CoordSys *coordsys, CoordSph *sph, Vec3 *xyz)
{
    xyz->v[coordsys->axPole  ] = sph->r * COS(sph->polar);
    xyz->v[coordsys->axPlaneX] = sph->r * SIN(sph->polar) * COS(sph->azimuth);
    xyz->v[coordsys->axPlaneY] = sph->r * SIN(sph->polar) * SIN(sph->azimuth);
}

void CoordSys_Local2World(CoordSys *coordsys, Vec3 *local, Vec3 *world)
{
    Affine_Transform(&coordsys->local2world, local, world);
}

void CoordSys_World2Local(CoordSys *coordsys, Vec3 *world, Vec3 *local)
{
    Affine_Transform(&coordsys->world2local, world, local);
}

void CoordSysPair_Init(CoordSysPair *coordsyspair, CoordSys *src, CoordSys *dst)
{
    coordsyspair->src = src;
    coordsyspair->dst = dst;
    CoordSysPair_Compose(coordsyspair);
}

void CoordSysPair_Compose(CoordSysPair *coordsyspair)
{
    CoordSys *src = coordsyspair->src;
    CoordSys *dst = coordsyspair->dst;
    //Affine_Compose(&src->local2world, &dst->world2local, &coordsyspair->src2dst);
    Affine_Compose(&dst->local2world, &src->world2local, &coordsyspair->dst2src);
}

//void CoordSysPair_ChangeBasis_Forward(CoordSysPair *coordsyspair, Vec3 *onsrc, Vec3 *ondst)
//{
//    Affine_Transform(&coordsyspair->src2dst, onsrc, ondst);
//}

void CoordSysPair_ChangeBasis_Inverse(CoordSysPair *coordsyspair, Vec3 *ondst, Vec3 *onsrc)
{
    Affine_Transform(&coordsyspair->dst2src, ondst, onsrc);
}
