#pragma once

#include "common.h"
#include "affine.h"
#include "roi.h"
#include "pose.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        XXXX_FLOAT      *input;
        XXXX_FLOAT      inputScalerToRad;
        XXXX_FLOAT      *output;
        XXXX_FLOAT      outputScalerToPix;
        int             tablelength;
    } ParamDist;

    void ParamDist_Init(ParamDist *param, XXXX_FLOAT *input, XXXX_FLOAT inputScalerToRad, XXXX_FLOAT *output, XXXX_FLOAT outputScalerToPix, int tablelength);

    typedef enum {
        ParamCamera_ProjType_Lens               = 0,
        ParamCamera_ProjType_Equirectangular    = 1,
        ParamCamera_ProjType_Cylindrical        = 2,
        ParamCamera_ProjType_Rectilinear        = 3
    } ParamCamera_ProjType;

    typedef struct {
        int             w;
        int             h;
        XXXX_FLOAT      centerx;
        XXXX_FLOAT      centery;
        XXXX_FLOAT      scalepixw;
        XXXX_FLOAT      scaledegw;
    } ParamImage;

    void ParamImage_Init(ParamImage *param, int w, int h, XXXX_FLOAT centerx, XXXX_FLOAT centery, XXXX_FLOAT scalepixw, XXXX_FLOAT scaledegw);

    typedef struct {
        ParamCamera_ProjType    projtype;
        PoseAxis                front;
        Affine                  poseinworld;       // camera pose in world coordinate system
        ParamDist               dist;
        ParamImage              image;
    } ParamCamera;

    void ParamCamera_Init(ParamCamera *param, int w, int h, XXXX_FLOAT centerxshift, XXXX_FLOAT centeryshift, XXXX_FLOAT scalepixw, XXXX_FLOAT scaledegw, ParamCamera_ProjType projtype, ParamDist *paramdist, PoseAxis front, Affine *poseinworld);

#ifdef __cplusplus
}
#endif
