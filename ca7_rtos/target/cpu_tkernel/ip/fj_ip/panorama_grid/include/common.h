#pragma once

#include <math.h>

typedef int  BOOL;

typedef          char  sint8;
typedef unsigned char  uint8;
typedef          short sint16;
typedef unsigned short uint16;
typedef          int   sint32;
typedef unsigned int   uint32;

//#define ARM_TRIAL_INLINE

//#define ARM_TRIAL_DOUBLE
//#define ARM_TRIAL_TRIGONO_DISABLE
//#define ARM_TRIAL_TRIGONO_TABLE
//#define ARM_TRIAL_GRID_SPARSE_X1_Y2
#define ARM_TRIAL_GRID_SPARSE_X2_Y2
//#define TRIAL_ROTATE_STITCH_IMAGE

#ifdef ARM_TRIAL_INLINE
#define INLINE          extern inline
#define STATIC_INLINE   static inline
#else
#define INLINE          
#define STATIC_INLINE   
#endif

#ifdef ARM_TRIAL_DOUBLE
typedef double          XXXX_FLOAT;
#define SIN(rad)        sin(rad)
#define COS(rad)        cos(rad)
#define TAN(rad)        tan(rad)
#define ACOS(v)         acos(v)
#define ASIN(v)         asin(v)
#define ATAN2(y,x)      atan2(y,x)
#define SQRT(v)         sqrt(v)
#else
typedef float           XXXX_FLOAT;
#if defined(ARM_TRIAL_TRIGONO_DISABLE)
#define SIN(rad)        0
#define COS(rad)        0
#define TAN(rad)        0
#define ACOS(v)         0
#define ASIN(v)         0
#define ATAN2(y,x)      0
#define SQRT(v)         1
#elif defined(ARM_TRIAL_TRIGONO_TABLE)
#include "mathtbl.h"
extern MathSin  *mathSin;
extern MathCos  *mathCos;
extern MathAsin *mathAsin;
extern MathAcos *mathAcos;
extern MathAtan *mathAtan;
extern MathSqrt *mathSqrt;
#define SIN(rad)        (mathSinGet(mathSin, (rad)))
#define COS(rad)        (mathCosGet(mathCos, (rad)))
//#define TAN(rad)        (0)
#define ACOS(v)         (mathAcosGet(mathAcos, (v)))
#define ASIN(v)         (mathAsinGet(mathAsin, (v)))
#define ATAN2(y,x)      (mathAtanGet(mathAtan, mathSqrt, y, x))
#define SQRT(v)         (mathSqrtGet(mathSqrt, (v)))
#else
#define SIN(rad)        sinf(rad)
#define COS(rad)        cosf(rad)
#define TAN(rad)        tanf(rad)
#define ACOS(v)         acosf(v)
#define ASIN(v)         asinf(v)
#define ATAN2(y,x)      atan2f(y,x)
#define SQRT(v)         sqrtf(v)
#endif
#endif

#ifndef __cplusplus
#if !defined(NULL)
#define NULL  ((void *)0)
#endif
#endif

#define TRUE  (1)
#define FALSE (0)

#define PI              (3.14159265358979323846f)

#define DEG2RAD(deg)    ((deg) / 180.0f * PI)
#define RAD2DEG(rad)    ((rad) / PI * 180.0f)

#define ROUNDUP(n, m)   (int)(ceilf(((float)(n)/(m)*(m))))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
