#pragma once

#include "common.h"
#include "param.h"
#include "roi.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        int         w;
        int         h;
        XXXX_FLOAT  centerx;
        XXXX_FLOAT  centery;
        XXXX_FLOAT  f;
        //Roi         roi;
    } Image;

    void Image_Init(Image *image, int w, int h, XXXX_FLOAT centerx, XXXX_FLOAT centery, XXXX_FLOAT scalepixw, XXXX_FLOAT scaledegw);
    //void Image_SetRoi(Image *image, int x, int y, int w, int h);
    //void Image_SetRoi_FromStr(Image *image, Roi *roi);
    //void Image_MoveRoiTo(Image *image, int x, int y);
    //void Image_SetRoi_Default(Image *image);

    XXXX_FLOAT Image_Coord_NormalizeX(Image *image, XXXX_FLOAT x, XXXX_FLOAT org);
    XXXX_FLOAT Image_Coord_NormalizeY(Image *image, XXXX_FLOAT y, XXXX_FLOAT org);
    void Image_Coord_Normalize(Image *image, Vec2 *v, Vec2 *org, Vec2 *normalized);

    XXXX_FLOAT Image_Coord_UnnormalizeX(Image *image, XXXX_FLOAT x, XXXX_FLOAT org);
    XXXX_FLOAT Image_Coord_UnnormalizeY(Image *image, XXXX_FLOAT y, XXXX_FLOAT org);
    void Image_Coord_Unnormalize(Image *image, Vec2 *v, Vec2 *org, Vec2 *denormalized);

    XXXX_FLOAT Image_GetDefaultCenter(XXXX_FLOAT length);
    XXXX_FLOAT Image_GetCenterShift(XXXX_FLOAT length, XXXX_FLOAT center);

#ifdef __cplusplus
}
#endif
