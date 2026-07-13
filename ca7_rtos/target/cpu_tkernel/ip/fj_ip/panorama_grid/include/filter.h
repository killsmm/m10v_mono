#pragma once

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

    #define RING_BUFFER_LENGTH  (16)

    typedef struct {
        float   x;
        float   y;
        float   z;
    } Accel;

    typedef struct {
        float   buffer[RING_BUFFER_LENGTH];
        float   *top;
        float   *bottom;
        float   *rp;
        float   *wp;
        int     length;
        float   sum;
    } RingBuffer;

    typedef struct {
        RingBuffer  accelx;
        RingBuffer  accely;
        RingBuffer  accelz;
        int         taps;
        int         length;
    } AccelFilter;

    typedef struct {
        Accel   prev;
        float   alpha;
    } TemporalFilter;

    void Accel_Init(Accel *accel, float x, float y, float z);
#if 0
    void Accel_Floor(Accel *accel, float x, float y, float z);
#endif
    void RingBuffer_Init(RingBuffer *ring);
    float *RingBuffer_Inc(RingBuffer *ring, float *p);
    void RingBuffer_Push(RingBuffer *ring, float value);
    float RingBuffer_Pop(RingBuffer *ring);
    float RingBuffer_GetAverage(RingBuffer *ring);
    void AccelFilter_Init(AccelFilter *filter);
    void AccelFilter_GetFiltered(AccelFilter *filter, float ax, float ay, float az, Accel *filtered);
    void TemporalFilter_Init(TemporalFilter *filter, float alpha, Accel *init);
    void TemporalFilter_GetFiltered(TemporalFilter *filter, float ax, float ay, float az, Accel *filtered);
#if 0
    void movingaverage(void);
    void temporalfilter(void);
#endif
#ifdef __cplusplus
}
#endif
