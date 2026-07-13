#include "mat.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "common.h"


void Mat_Init(Mat *m, XXXX_FLOAT e[VEC_DIM][VEC_DIM])
{
    size_t size = VEC_DIM * VEC_DIM * sizeof(e[0][0]);
    memcpy(m->e, e, size);
}

void Mat_Mul(Mat *left, Mat *right, Mat *out)
{
    int r, c, i;
    for (r = 0; r < VEC_DIM; r++) {
        for (c = 0; c < VEC_DIM; c++) {
            out->e[r][c] = 0;
            for (i = 0; i < VEC_DIM; i++) {
                out->e[r][c] += left->e[r][i] * right->e[i][c];    // out = left * right
            }
        }
    }
}

void Mat_MulVec(Mat *m, Vec3 *v, Vec3 *vout)
{
    int r, i;

    for (r = 0; r < VEC_DIM; r++) {
        vout->v[r] = 0;
        for (i = 0; i < VEC_DIM; i++) {
            vout->v[r] += m->e[r][i] * v->v[i];     // vout = m * v
        }
    }
}

void Mat_GetInv3x3(Mat *m, Mat *inv)
{
    // tentative : inverse of a rotation matrix = transposed matrix
    int r, c;
    for (r = 0; r < VEC_DIM; r++) {
        for (c = 0; c < VEC_DIM; c++) {
            inv->e[r][c] = m->e[c][r];
        }
    }
}

XXXX_FLOAT Mat_GetDet3x3(Mat *m)
{
    return   m->e[0][0] * m->e[1][1] * m->e[2][2]
           + m->e[1][0] * m->e[2][1] * m->e[0][2]
           + m->e[2][0] * m->e[0][1] * m->e[1][2]
           - m->e[1][0] * m->e[0][1] * m->e[2][2]
           - m->e[2][0] * m->e[1][1] * m->e[0][2]
           - m->e[0][0] * m->e[2][1] * m->e[1][2];
}

void Mat_Copy(Mat *dst, Mat *src)
{
    Mat_Init(dst, src->e);
}

void Mat_Print(Mat *m)
{
    int r, c;
    for (r = 0; r < VEC_DIM; r++) {
        for (c = 0; c < VEC_DIM; c++) {
            printf("mat[%d][%d] = %f\n", r, c, m->e[r][c]);
        }
    }
    printf("\n");
}


void Mat_Init_Identity(Mat *m)
{
    m->e[0][0] = 1.0;      m->e[0][1] = 0.0;      m->e[0][2] = 0.0;
    m->e[1][0] = 0.0;      m->e[1][1] = 1.0;      m->e[1][2] = 0.0;
    m->e[2][0] = 0.0;      m->e[2][1] = 0.0;      m->e[2][2] = 1.0;
}

void Mat_Init_RotX(Mat *m, XXXX_FLOAT rad)
{
    //m->e[0][0] =  1.0;      m->e[0][1] =  0.0;      m->e[0][2] =  0.0;
    //m->e[1][0] =  0.0;      m->e[1][1] =  COS(rad); m->e[1][2] = -SIN(rad);
    //m->e[2][0] =  0.0;      m->e[2][1] =  SIN(rad); m->e[2][2] =  COS(rad);

    Mat_Init_Rot(m, rad, CoordSysAxis_X);
}

void Mat_Init_RotY(Mat *m, XXXX_FLOAT rad)
{
    //m->e[0][0] =  COS(rad); m->e[0][1] =  0.0;      m->e[0][2] =  SIN(rad);
    //m->e[1][0] =  0.0;      m->e[1][1] =  1.0;      m->e[1][2] =  0.0;
    //m->e[2][0] = -SIN(rad); m->e[2][1] =  0.0;      m->e[2][2] =  COS(rad);

    Mat_Init_Rot(m, rad, CoordSysAxis_Y);
}

void Mat_Init_RotZ(Mat *m, XXXX_FLOAT rad)
{
    //m->e[0][0] =  COS(rad); m->e[0][1] = -SIN(rad); m->e[0][2] =  0.0;
    //m->e[1][0] =  SIN(rad); m->e[1][1] =  COS(rad); m->e[1][2] =  0.0;
    //m->e[2][0] =  0.0;      m->e[2][1] =  0.0;      m->e[2][2] =  1.0;

    Mat_Init_Rot(m, rad, CoordSysAxis_Z);
}

void Mat_Init_Rot(Mat *m, XXXX_FLOAT rad, CoordSysAxis ax)
{
    int ax0 = CoordSysAxis_Pole2PlaneX(ax);
    int ax1 = CoordSysAxis_Pole2PlaneY(ax);
    int ax2 = ax;

    m->e[ax0][ax0] =  COS(rad); m->e[ax0][ax1] = -SIN(rad); m->e[ax0][ax2] =  0.0;
    m->e[ax1][ax0] =  SIN(rad); m->e[ax1][ax1] =  COS(rad); m->e[ax1][ax2] =  0.0;
    m->e[ax2][ax0] =  0.0;      m->e[ax2][ax1] =  0.0;      m->e[ax2][ax2] =  1.0;
}

void Mat_Init_RotEuler(Mat *m, XXXX_FLOAT rad0, XXXX_FLOAT rad1, XXXX_FLOAT rad2, CoordSysAxis ax0, CoordSysAxis ax1, CoordSysAxis ax2)
{
    Mat m0, m1, m2, m10;
    Mat_Init_Rot(&m0, rad0, ax0);
    Mat_Init_Rot(&m1, rad1, ax1);
    Mat_Init_Rot(&m2, rad2, ax2);
    Mat_Mul(&m1, &m0, &m10);
    Mat_Mul(&m2, &m10, m);
}
