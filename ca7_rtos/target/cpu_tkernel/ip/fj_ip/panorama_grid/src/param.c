#include "param.h"

void ParamDist_Init(ParamDist *param, XXXX_FLOAT *input, XXXX_FLOAT inputScalerToRad, XXXX_FLOAT *output, XXXX_FLOAT outputScalerToPix, int tablelength)
{
    param->input                = input;
    param->inputScalerToRad     = inputScalerToRad;
    param->output               = output;
    param->outputScalerToPix    = outputScalerToPix;
    param->tablelength = tablelength;
}

void ParamImage_Init(ParamImage *param, int w, int h, XXXX_FLOAT centerx, XXXX_FLOAT centery, XXXX_FLOAT scalepixw, XXXX_FLOAT scaledegw)
{
    param->w            = w;
    param->h            = h;
    param->centerx      = centerx;
    param->centery      = centery;
    param->scalepixw    = scalepixw;
    param->scaledegw    = scaledegw;
}

void ParamCamera_Init(ParamCamera *param, int w, int h, XXXX_FLOAT centerx, XXXX_FLOAT centery, XXXX_FLOAT scalepixw, XXXX_FLOAT scaledegw, ParamCamera_ProjType projtype, ParamDist *paramdist, PoseAxis front, Affine *poseinworld)
{
    param->front = front;
    Affine_Copy(&param->poseinworld, poseinworld);
    param->projtype = projtype;
    if (paramdist != NULL) {
        ParamDist_Init(&param->dist, paramdist->input, paramdist->inputScalerToRad, paramdist->output, paramdist->outputScalerToPix, paramdist->tablelength);
    }
    ParamImage_Init(&param->image, w, h, centerx, centery, scalepixw, scaledegw);
}
