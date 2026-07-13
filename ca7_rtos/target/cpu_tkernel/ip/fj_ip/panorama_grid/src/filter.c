#include "filter.h"
#include <stdio.h>

void Accel_Init(Accel *accel, float x, float y, float z)
{
    accel->x = x;
    accel->y = y;
    accel->z = z;
}

#if 0
void Accel_Floor(Accel *accel, float x, float y, float z)
{
    accel->x = floor( x * 100 ) / 100;
    accel->y = floor( y * 100 ) / 100;
    accel->z = floor( z * 100 ) / 100;
}
#endif

void RingBuffer_Init(RingBuffer *ring)
{
    ring->top       = ring->buffer;
    ring->bottom    = ring->top + (RING_BUFFER_LENGTH - 1);
    ring->rp        = ring->top;
    ring->wp        = ring->top;
    ring->length    = 0;
    ring->sum       = 0;
}

float *RingBuffer_Inc(RingBuffer *ring, float *p)
{
    return (p == ring->bottom) ? ring->top : p + 1;
}

void RingBuffer_Push(RingBuffer *ring, float value)
{
    *ring->wp = value;
    ring->wp = RingBuffer_Inc(ring, ring->wp);
    ring->length++;
    ring->sum += value;
}

float RingBuffer_Pop(RingBuffer *ring)
{
    float value = *ring->rp;
    ring->rp = RingBuffer_Inc(ring, ring->rp);
    ring->length--;
    ring->sum -= value;
    return value;
}

float RingBuffer_GetAverage(RingBuffer *ring)
{
    return ring->sum / ring->length;
}

void AccelFilter_Init(AccelFilter *filter)
{
    RingBuffer_Init(&filter->accelx);
    RingBuffer_Init(&filter->accely);
    RingBuffer_Init(&filter->accelz);
    filter->taps = 7;
}

void AccelFilter_GetFiltered(AccelFilter *filter, float ax, float ay, float az, Accel *filtered)
{
    RingBuffer_Push(&filter->accelx, ax);
    RingBuffer_Push(&filter->accely, ay);
    RingBuffer_Push(&filter->accelz, az);
    if (filter->accelx.length > filter->taps) {
        RingBuffer_Pop(&filter->accelx);
        RingBuffer_Pop(&filter->accely);
        RingBuffer_Pop(&filter->accelz);
    }
    filtered->x = RingBuffer_GetAverage(&filter->accelx);
    filtered->y = RingBuffer_GetAverage(&filter->accely);
    filtered->z = RingBuffer_GetAverage(&filter->accelz);
}

void TemporalFilter_Init(TemporalFilter *filter, float alpha, Accel *init)
{
    filter->prev = *init;
    filter->alpha = alpha;
}

void TemporalFilter_GetFiltered(TemporalFilter *filter, float ax, float ay, float az, Accel *filtered)
{
    filtered->x = filter->alpha * ax + (1 - filter->alpha) * filter->prev.x;
    filtered->y = filter->alpha * ay + (1 - filter->alpha) * filter->prev.y;
    filtered->z = filter->alpha * az + (1 - filter->alpha) * filter->prev.z;

    filter->prev = *filtered;
}

#if 0
void movingaverage()
{
    FILE *fp = fopen("accel.txt", "w");
    int size = sizeof(accels) / sizeof(accels[0]);
    AccelFilter filter;
    AccelFilter_Init(&filter);
    for (int i = 0; i < size; i++) {
        Accel filtered;
        float ax = accels[i][0];
        float ay = accels[i][1];
        float az = accels[i][2];
        AccelFilter_GetFiltered(&filter, ax, ay, az, &filtered);
        fprintf(fp, "%f\t%f\t%f\n", filtered.x, filtered.y, filtered.z);
    }
    fclose(fp);
}

void temporalfilter()
{
    float alpha = 0.1;
    FILE *fp = fopen("accel_temporal.txt", "w");
    int size = sizeof(accels) / sizeof(accels[0]);
    TemporalFilter filter;
    Accel init;
    init.x = accels[0][0];
    init.y = accels[0][1];
    init.z = accels[0][2];
    TemporalFilter_Init(&filter, alpha, &init);
    for (int i = 0; i < size; i++) {
        Accel filtered;
        float ax = accels[i][0];
        float ay = accels[i][1];
        float az = accels[i][2];
        TemporalFilter_GetFiltered(&filter, ax, ay, az, &filtered);
        fprintf(fp, "%f\t%f\t%f\n", filtered.x, filtered.y, filtered.z);
    }
    fclose(fp);
}
#endif
