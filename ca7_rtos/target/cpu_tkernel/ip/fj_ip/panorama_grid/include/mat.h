#pragma once

#include "vec.h"
#include "coordsysaxis.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        XXXX_FLOAT  e[VEC_DIM][VEC_DIM];
    } Mat;

    void Mat_Init(Mat *m, XXXX_FLOAT e[VEC_DIM][VEC_DIM]);
    void Mat_Mul(Mat *left, Mat *right, Mat *out);       // out = left * right
    void Mat_MulVec(Mat *m, Vec3 *v, Vec3 *vout);        // vout = m * v
    void Mat_GetInv3x3(Mat *m, Mat *inv);                // inv = m^-1
    XXXX_FLOAT Mat_GetDet3x3(Mat *m);
    void Mat_Copy(Mat *dst, Mat *src);
    void Mat_Print(Mat *m);
    void Mat_Init_Identity(Mat *m);
    void Mat_Init_RotX(Mat *m, XXXX_FLOAT rad);
    void Mat_Init_RotY(Mat *m, XXXX_FLOAT rad);
    void Mat_Init_RotZ(Mat *m, XXXX_FLOAT rad);
    void Mat_Init_Rot(Mat *m, XXXX_FLOAT rad, CoordSysAxis axrot);
    void Mat_Init_RotEuler(Mat *m, XXXX_FLOAT rad0, XXXX_FLOAT rad1, XXXX_FLOAT rad2, CoordSysAxis ax0, CoordSysAxis ax1, CoordSysAxis ax2);

#ifdef __cplusplus
}
#endif
