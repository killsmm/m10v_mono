#include "dist.h"
#include "mem.h"

void Dist_Init(Dist *dist, XXXX_FLOAT *tableinput, XXXX_FLOAT *tableoutput, int tablelength, XXXX_FLOAT in2rad, XXXX_FLOAT out2pix)
{
    dist->length = tablelength;
    dist->input  = Mem_Alloc(tablelength, sizeof(XXXX_FLOAT));  // TODO : no initialize
    dist->output = Mem_Alloc(tablelength, sizeof(XXXX_FLOAT));

    for (int i = 0; i < dist->length; i++) {
        dist->input [i] = tableinput [i] * in2rad;
        dist->output[i] = tableoutput[i] * out2pix;
    }
}

XXXX_FLOAT interpolate(Dist *dist, XXXX_FLOAT input, int idx0, int idx1)
{
    XXXX_FLOAT input0  = dist->input [idx0];
    XXXX_FLOAT input1  = dist->input [idx1];
    XXXX_FLOAT output0 = dist->output[idx0];
    XXXX_FLOAT output1 = dist->output[idx1];
    return (output1 - output0) / (input1 - input0) * (input - input0) + output0;
}

XXXX_FLOAT Dist_ProjForward(Dist *dist, XXXX_FLOAT input)
{
    for (int i = 0; i < dist->length-1; i++) {
        if ((dist->input[i] <= input) && (input < dist->input[i+1])) {
            return interpolate(dist, input, i, i+1);
        }
    }
    return interpolate(dist, input, dist->length-2, dist->length-1);
}
