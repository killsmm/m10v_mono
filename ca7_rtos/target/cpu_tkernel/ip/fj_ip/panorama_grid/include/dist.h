#pragma once

#include "common.h"
#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        XXXX_FLOAT  *input;
        XXXX_FLOAT  *output;
        int         length;
    } Dist;

    void Dist_Init(Dist *dist, XXXX_FLOAT *tableinput, XXXX_FLOAT *tableoutput, int tablelength, XXXX_FLOAT in2rad, XXXX_FLOAT out2pix);
    XXXX_FLOAT Dist_ProjForward(Dist *dist, XXXX_FLOAT in);

#ifdef __cplusplus
}
#endif
