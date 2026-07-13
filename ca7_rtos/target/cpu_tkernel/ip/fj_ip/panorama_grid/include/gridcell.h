#pragma once

#include "common.h"
#include "size.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        Size        size;
        Size        sizemain;
        int         restw;
        int         resth;
        Size        count;
    } GridCell;

    void gridCell_GetCount(int n, int step, int *count, int *rest, int *countmain);

    void GridCell_Init(GridCell *cell, int outw, int outh, int cellw, int cellh);

    BOOL GridCell_IsIncompleteX(GridCell *cell);
    BOOL GridCell_IsIncompleteY(GridCell *cell);

    int  GridCell_GetSizeTotalX(GridCell *cell);
    int  GridCell_GetSizeTotalY(GridCell *cell);

    int GridCell_GetRasterId(GridCell *cell, int x, int y);

#ifdef __cplusplus
}
#endif
