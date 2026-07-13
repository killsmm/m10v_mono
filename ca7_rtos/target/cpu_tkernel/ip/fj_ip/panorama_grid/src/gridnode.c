#include "gridnode.h"
#include "grid.h"
#include "mem.h"
#include <math.h>
#include <stdio.h>

int cell2node(int cell)
{
    return cell + 1;
}

void GridNode_Init(GridNode *node, int nodew, int nodeh, Size *cellsizemain, BOOL alloc)
{
    Size_Init(&node->size, nodew, nodeh);
    int mainw = cell2node(cellsizemain->w);
    int mainh = cell2node(cellsizemain->h);
    Size_Init(&node->sizemain, mainw, mainh);
    node->lastid.x = nodew - 1;
    node->lastid.y = nodeh - 1;
    if (alloc == TRUE) {
        node->coord = Mem_Alloc(node->size.total, sizeof(Vec2));
    } else {
        node->coord = NULL;
    }
}

int GridNode_GetRasterId(GridNode *node, int x, int y)
{
    return x + (y * node->size.w);
}

int GridNode_GetRasterId_(int nodew, int x, int y)
{
    return x + (y * nodew);
}

Vec2 *GridNode_GetCoordPtr(GridNode *node, int x, int y)
{
    int i = GridNode_GetRasterId_(node->size.w, x, y);
    return &node->coord[i];
}

void GridNode_Print(GridNode *node)
{
    printf("-- grid --\n");
    for (int y = 0; y < node->size.h; y++) {
        for (int x = 0; x < node->size.w; x++) {
            Vec2 *coord = GridNode_GetCoordPtr(node, x, y);
            printf("grid(%d,%d) : x = %f, y = %f\n", x, y, coord->x, coord->y);
        }
    }
    printf("\n");
}
