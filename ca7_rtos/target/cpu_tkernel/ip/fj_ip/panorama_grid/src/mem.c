#include "mem.h"
#include <stdio.h>
#include <stdlib.h>


//Mem gmem;
//
//void Mem_Init(Mem *mem, int size)
//{
//    mem->addr  = malloc(size);
//    mem->total = size;
//    mem->used  = 0;
//}
//
//void *Mem_Alloc(Mem *mem, int size)
//{
//    if (size > mem->total - mem->used) {
//        printf("Mem_Alloc : error : no memory\n");
//        return NULL;
//    }
//    void *addr = (unsigned char *)mem->addr + mem->used;
//    mem->used += size;
//    return addr;
//}
//
//void Mem_Free(Mem *mem)
//{
//    free(mem->addr);
//}
//
typedef struct {
    void *addr;
    int  size;
} MemAddr;

void *MemAddr_Init(MemAddr *mem, int count, int unitsize)
{
    mem->addr = malloc(count * unitsize);
    mem->size = unitsize;
    return mem->addr;
}

#define MEM_COUNT   (65535)

typedef struct {
    MemAddr mem[MEM_COUNT];
    int     i;
    int     sizetotal;
} Mem;

#if 1
static
Mem gmem = { .i = 0,
             .sizetotal = 0, };
static
Mem gmem_final = { .i = 0,
                   .sizetotal = 0, };
#else
Mem gmem = {0};
#endif

void *Mem_Alloc(int count, int unitsize)
{
    if (gmem.i > MEM_COUNT) {
        printf("Mem_Alloc : error : max mem count over\n");
        //exit(1);
    }
    void *addr = MemAddr_Init(&gmem.mem[gmem.i], count, unitsize);
    int size = count * unitsize;
    gmem.i++;
    gmem.sizetotal += size;
//  printf("%d, %d, %d, %d, %p\n", gmem.i, count, unitsize, gmem.sizetotal, addr);
    return addr;
}

void Mem_Free()
{
    for (int i = 0; i < gmem.i; i++) {
        free(gmem.mem[i].addr);
    }

    gmem.i = 0;
    gmem.sizetotal = 0;
}

void *Mem_AllocFinal(int count, int unitsize)
{
    if (gmem_final.i > MEM_COUNT) {
        printf("Mem_Alloc : error : max mem count over\n");
        //exit(1);
    }
    void *addr = MemAddr_Init(&gmem_final.mem[gmem_final.i], count, unitsize);
    gmem_final.i++;
//  printf("final %d, %d, %d, %d, %p\n", gmem_final.i, count, unitsize, gmem_final.sizetotal, addr);
    return addr;
}

void Mem_FreeFinal()
{
    for (int i = 0; i < gmem_final.i; i++) {
        free(gmem_final.mem[i].addr);
    }

    gmem_final.i = 0;
}
