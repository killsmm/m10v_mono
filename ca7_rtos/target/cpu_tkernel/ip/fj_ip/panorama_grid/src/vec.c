#include "vec.h"
#include <string.h>
#include <math.h>

void Vec3_Init(Vec3 *in, XXXX_FLOAT x, XXXX_FLOAT y, XXXX_FLOAT z)
{
    in->v[0] = x;
    in->v[1] = y;
    in->v[2] = z;
}

void Vec3_MulScalar(Vec3 *in, XXXX_FLOAT scalar, Vec3 *out)
{
    for (int i = 0; i < VEC_DIM; i++) {
        out->v[i] = in->v[i] * scalar;  // out = in * scalar
    }
}

void Vec3_MulScalarToSrc(Vec3 *v, XXXX_FLOAT scalar)
{
    for (int i = 0; i < VEC_DIM; i++) {
        v->v[i] *= scalar;
    }
}

void Vec3_Add(Vec3 *in0, Vec3 *in1, Vec3 *out)
{
    for (int i = 0; i < VEC_DIM; i++) {
        out->v[i] = in0->v[i] + in1->v[i];  // out = in0 + in1
    }
}

void Vec3_AddWeighted(Vec3 *in0, Vec3 *in1, XXXX_FLOAT w0, XXXX_FLOAT w1, Vec3 *out)
{
    for (int i = 0; i < VEC_DIM; i++) {
        out->v[i] = w0 * in0->v[i] + w1 * in1->v[i];
    }
}

void Vec3_Average(Vec3 *in0, Vec3 *in1, Vec3 *out)
{
    Vec3_AddWeighted(in0, in1, 0.5, 0.5, out);
}

void Vec3_Copy(Vec3 *dst, Vec3 *src)
{
    size_t size = VEC_DIM * sizeof(src->v[0]);
    memcpy(dst->v, src->v, size);
}

XXXX_FLOAT Vec3_GetNorm(Vec3 *v)
{
#if 1
    return SQRT(v->v[0] * v->v[0] +
                v->v[1] * v->v[1] +
                v->v[2] * v->v[2]);
#else
    XXXX_FLOAT sq = 0;
    for (int i = 0; i < VEC_DIM; i++) {
        sq += v->v[i] * v->v[i];
    }
    return SQRT(sq);
#endif
}

void Vec3_Init_Zero(Vec3 *in)
{
    for (int i = 0; i < VEC_DIM; i++) {
        in->v[i] = 0.0f;
    }
}

void Vec2_Init(Vec2 *v, XXXX_FLOAT x, XXXX_FLOAT y)
{
    v->x = x;
    v->y = y;
}

void Vec2_AddWeighted(Vec2 *in0, Vec2 *in1, XXXX_FLOAT w0, XXXX_FLOAT w1, Vec2 *out)
{
    out->x = in0->x * w0 + in1->x * w1;
    out->y = in0->y * w0 + in1->y * w1;
}

void Vec2_Average(Vec2 *in0, Vec2 *in1, Vec2 *out)
{
    Vec2_AddWeighted(in0, in1, 0.5, 0.5, out);
}
