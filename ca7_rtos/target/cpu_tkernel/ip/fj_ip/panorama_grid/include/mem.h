#pragma once

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEM_SIZE    (4*1024*1024)

    //typedef struct {
    //    void *addr;
    //    int  total;
    //    int  used;
    //} Mem;

    //void Mem_Init(Mem *mem, int size);
    //void *Mem_Alloc(Mem *mem, int size);
    //void Mem_Free(Mem *mem);

    //extern Mem gmem;

    void *Mem_Alloc(int count, int unitsize);
    void Mem_Free();
    void *Mem_AllocFinal(int count, int unitsize);
    void Mem_FreeFinal();

#ifdef __cplusplus
}
#endif
