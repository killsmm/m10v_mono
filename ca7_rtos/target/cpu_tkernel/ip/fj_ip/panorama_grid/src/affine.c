#include "affine.h"

void Affine_Init(Affine *a, XXXX_FLOAT m[VEC_DIM][VEC_DIM], XXXX_FLOAT tx, XXXX_FLOAT ty, XXXX_FLOAT tz)
{
    Mat_Init(&a->m, m);
    Vec3_Init(&a->t, tx, ty, tz);
}

void Affine_CopyMat(Affine *a, Mat *m)
{
    Mat_Copy(&a->m, m);
}

void Affine_CopyVec(Affine *a, Vec3 *t)
{
    Vec3_Copy(&a->t, t);
}

void Affine_CopyMatVec(Affine *a, Mat *m, Vec3 *t)
{
    Affine_CopyMat(a, m);
    Affine_CopyVec(a, t);
}

void Affine_Transform(Affine *a, Vec3 *v, Vec3 *vout)
{
    Vec3 v0;
    Mat_MulVec(&a->m, v, &v0);   // v0  = a->m * v
    Vec3_Add(&v0, &a->t, vout);  // vout = a->m * v + a->t
}

void Affine_GetInv(Affine *a, Affine *inv)
{
    Vec3 v;
    Mat_GetInv3x3(&a->m, &inv->m);       // inv->m = (a->m)^-1
    Vec3_MulScalar(&a->t, -1, &v);       // v = -(a->t)
    Mat_MulVec(&inv->m, &v, &inv->t);    // inv->t = -(a->m)^-1 * (a->t)
}

void Affine_Compose(Affine *first, Affine *second, Affine *composed) // composed(x) = second(first(x))
{
    // first(x)  = m0*v + t0
    // second(x) = m1*v + t1
    // second(first(x)) = m1(m0*x + t0) + t1 = m1*m0*x + (m1*t0 + t1)

    Vec3 t;
    Mat_Mul(&second->m, &first->m, &composed->m);    // composed->m = second->m * first->m
    Mat_MulVec(&second->m, &first->t, &t);
    Vec3_Add(&t, &second->t, &composed->t);          // composed->t = second->m * first->t + second->t
}

void Affine_Copy(Affine *dst, Affine *src)
{
    Mat_Copy(&dst->m, &src->m);
    Vec3_Copy(&dst->t, &src->t);
}

void Affine_Init_Identity(Affine *a)
{
    Mat_Init_Identity(&a->m);
    Vec3_Init_Zero(&a->t);
}

void Affine_Init_RotX(Affine *a, XXXX_FLOAT rad, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2)
{
    Mat_Init_RotX(&a->m, rad);
    Vec3_Init(&a->t, t0, t1, t2);
}

void Affine_Init_RotY(Affine *a, XXXX_FLOAT rad, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2)
{
    Mat_Init_RotY(&a->m, rad);
    Vec3_Init(&a->t, t0, t1, t2);
}

void Affine_Init_RotZ(Affine *a, XXXX_FLOAT rad, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2)
{
    Mat_Init_RotZ(&a->m, rad);
    Vec3_Init(&a->t, t0, t1, t2);
}

void Affine_Init_Rot(Affine *a, XXXX_FLOAT rad, CoordSysAxis ax, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2)
{
    Mat_Init_Rot(&a->m, rad, ax);
    Vec3_Init(&a->t, t0, t1, t2);
}

void Affine_Init_RotEuler(Affine *a, XXXX_FLOAT rad0, XXXX_FLOAT rad1, XXXX_FLOAT rad2, CoordSysAxis ax0, CoordSysAxis ax1, CoordSysAxis ax2, XXXX_FLOAT t0, XXXX_FLOAT t1, XXXX_FLOAT t2)
{
    Mat_Init_RotEuler(&a->m, rad0, rad1, rad2, ax0, ax1, ax2);
    Vec3_Init(&a->t, t0, t1, t2);
}
