#include "accel2pose.h"

float norm(float x, float y, float z)
{
    return sqrt(x*x + y*y + z*z);
}

void accel2pose(float ax, float ay, float az, float ax0, float ay0, float az0, float posemat[][3])
{
    float a = norm(ax, ay, az);
    float a0 = norm(ax0, ay0, az0);
    float crossx = ay0 * az - az0 * ay;
    float crossy = az0 * ax - ax0 * az;
    float crossz = ax0 * ay - ay0 * ax;
    float cross = norm(crossx, crossy, crossz);
    float dot = ax * ax0 + ay * ay0 + az * az0;
    float s = cross / a / a0;
    float c = dot / a / a0;
#if 0
    float rad = atan2f(s, c);
    float axx = crossx / cross;
    float axy = crossy / cross;
    float axz = crossz / cross;
#else
    float rad;
    float axx;
    float axy;
    float axz;
    if (cross == 0.0f) {
        rad = 0.0f;
        axx = ax0;
        axy = ay0;
        axz = az0;
    } else {
        rad = atan2f(s, c);
        axx = crossx / cross;
        axy = crossy / cross;
        axz = crossz / cross;
    }
#endif
    posemat[0][0] = axx*axx*(1-cosf(rad))+    cosf(rad);    posemat[0][1] = axy*axx*(1-cosf(rad))-axz*sinf(rad);    posemat[0][2] = axz*axz*(1-cosf(rad))+axy*sinf(rad);
    posemat[1][0] = axx*axy*(1-cosf(rad))+axz*sinf(rad);    posemat[1][1] = axy*axy*(1-cosf(rad))+    cosf(rad);    posemat[1][2] = axz*axz*(1-cosf(rad))-axx*sinf(rad);
    posemat[2][0] = axx*axz*(1-cosf(rad))-axy*sinf(rad);    posemat[2][1] = axy*axz*(1-cosf(rad))+axx*sinf(rad);    posemat[2][2] = axz*axz*(1-cosf(rad))+    cosf(rad);
}

// usage
//    Affine poseaccel;
//    accel2pose(ax,ay,az,ax0,ay0,az0,poseaccel.m.e);
//    poseaccel.t.v[0] = 0.0f;
//    poseaccel.t.v[1] = 0.0f;
//    poseaccel.t.v[2] = 0.0f;
//    Stitch_Compute_StitchFinal(&stitch, &poseaccel, &stitchfinal);

// ax,ay,az = accel (1.0 = 1g)
// ax0, ay0, az0 = initial accel
