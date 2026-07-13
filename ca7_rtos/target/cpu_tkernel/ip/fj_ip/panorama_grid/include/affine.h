#pragma once

#include "mat.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        Mat     m;
        Vec3    t;
    } Affine;

    void Affine_Init(Affine *a, XXXX_FLOAT m[VEC_DIM][VEC_DIM], XXXX_FLOAT tx, XXXX_FLOAT ty, XXXX_FLOAT tz);
    void Affine_CopyMat(Affine *a, Mat *m);
    void Affine_CopyVec(Affine *a, Vec3 *t);
    void Affine_CopyMatVec(Affine *a, Mat *m, Vec3 *t);
    void Affine_CopyMatVec(Affine *a, Mat *m, Vec3 *t);
    void Affine_Transform(Affine *a, Vec3 *v, Vec3 *vout);                     // vout = a(v)
    void Affine_GetInv(Affine *a, Affine *inv);
    void Affine_Compose(Affine *first, Affine *second, Affine *composed);    // composed(x) = second(first(x))
    void Affine_Copy(Affine *dst, Affine *src);
    void Affine_MulScalarToVec(Affine *a, XXXX_FLOAT scalar, Affine *out);

    void Affine_Init_Identity(Affine *a);
    void Affine_Init_RotX(Affine *a, XXXX_FLOAT rad, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2);
    void Affine_Init_RotY(Affine *a, XXXX_FLOAT rad, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2);
    void Affine_Init_RotZ(Affine *a, XXXX_FLOAT rad, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2);
    void Affine_Init_Rot(Affine *a, XXXX_FLOAT rad, CoordSysAxis ax, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2);
    void Affine_Init_RotEuler(Affine *a, XXXX_FLOAT rad0, XXXX_FLOAT rad1, XXXX_FLOAT rad2, CoordSysAxis ax0, CoordSysAxis ax1, CoordSysAxis ax2, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2);

#ifdef __cplusplus
}
#endif
