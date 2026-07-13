#pragma once

#include "common.h"

#define VEC_DIM     (3)

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        XXXX_FLOAT  v[VEC_DIM];
    } Vec3;

    void Vec3_Init(Vec3 *in, XXXX_FLOAT x, XXXX_FLOAT y, XXXX_FLOAT z);
    void Vec3_MulScalar(Vec3 *in, XXXX_FLOAT scalar, Vec3 *out);    // out = in * scalar
    void Vec3_MulScalarToSrc(Vec3 *v, XXXX_FLOAT scalar);           // in = in * scalar
    void Vec3_Add(Vec3 *in0, Vec3 *in1, Vec3 *out);                 // out = in0 + in1
    void Vec3_Copy(Vec3 *dst, Vec3 *src);
    XXXX_FLOAT Vec3_GetNorm(Vec3 *v);
    void Vec3_AddWeighted(Vec3 *in0, Vec3 *in1, XXXX_FLOAT w0, XXXX_FLOAT w1, Vec3 *out);
    void Vec3_Average(Vec3 *in0, Vec3 *in1, Vec3 *out);
    void Vec3_Init_Zero(Vec3 *in);

    typedef struct {
        XXXX_FLOAT  x;
        XXXX_FLOAT  y;
    } Vec2;

    void Vec2_Init(Vec2 *v, XXXX_FLOAT x, XXXX_FLOAT y);
    void Vec2_AddWeighted(Vec2 *in0, Vec2 *in1, XXXX_FLOAT w0, XXXX_FLOAT w1, Vec2 *out);

#ifdef __cplusplus
}
#endif
