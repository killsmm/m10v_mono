#pragma once

#include "size.h"
#include "gridcell.h"
#include "vec.h"
#include "camera.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        int     x;
        int     y;
    } Id;

    typedef struct {
        Vec2    *coord;
        Size    size;
        Size    sizemain;
        Id      lastid;
    } GridNode;

    void GridNode_Init(GridNode *node, int nodew, int nodeh, Size *cellsizemain, BOOL alloc);

    Vec2 *GridNode_GetCoordPtr(GridNode *node, int x, int y);

    int  GridNode_GetRasterId(GridNode *node, int x, int y);
    int  GridNode_GetRasterId_(int nodew, int x, int y);
    void GridNode_Print(GridNode *node);

#ifdef __cplusplus
}
#endif
