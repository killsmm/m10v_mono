#pragma once

#include "vec.h"
#include "affine.h"

#ifdef __cplusplus
extern "C" {
#endif

    // geographic coordinate
    typedef struct {
        XXXX_FLOAT  r;
        XXXX_FLOAT  lat;        // north pole = pi/2, south pole = -pi/2
        XXXX_FLOAT  lon;        // west end = pi, east end = -pi
    } CoordGeo;

    // spherical coordinate
    typedef struct {
        XXXX_FLOAT  r;
        XXXX_FLOAT  polar;      // north pole = 0, south pole = pi
        XXXX_FLOAT  azimuth;    // west end = pi, east end = -pi
    } CoordSph;

    // right-handed rule
    typedef struct {
        Affine          *poseinworld;
        Affine          local2world;
        Affine          world2local;
        CoordSysAxis    axPole;
        CoordSysAxis    axPlaneX;
        CoordSysAxis    axPlaneY;
    } CoordSys;

    void CoordSys_Init(CoordSys *coordsys, Affine *poseinworld, XXXX_FLOAT objscale, CoordSysAxis axPole);
    void CoordSys_SetPose(CoordSys *coordsys, Affine *poseinworld, XXXX_FLOAT objscale);
    void CoordSys_XYZ2Geo(CoordSys *coordsys, Vec3 *xyz, CoordGeo *geo);
    void CoordSys_Geo2XYZ(CoordSys *coordsys, CoordGeo *geo, Vec3 *xyz);
    void CoordSys_XYZ2Sph(CoordSys *coordsys, Vec3 *xyz, CoordSph *sph);
    void CoordSys_Sph2XYZ(CoordSys *coordsys, CoordSph *sph, Vec3 *xyz);
    void CoordSys_Local2World(CoordSys *coordsys, Vec3 *local, Vec3 *world);
    void CoordSys_World2Local(CoordSys *coordsys, Vec3 *world, Vec3 *local);

    typedef struct {
        CoordSys    *src;
        CoordSys    *dst;
        //Affine      src2dst;
        Affine      dst2src;
    } CoordSysPair;

    void CoordSysPair_Init(CoordSysPair *coordsyspair, CoordSys *src, CoordSys *dst);
    void CoordSysPair_Compose(CoordSysPair *coordsyspair);
    void CoordSysPair_ChangeBasis_Inverse(CoordSysPair *coordsyspair, Vec3 *ondst, Vec3 *onsrc);
    //void CoordSysPair_ChangeBasis_Forward(CoordSysPair *coordsyspair, Vec3 *onsrc, Vec3 *ondst);

#ifdef __cplusplus
}
#endif
