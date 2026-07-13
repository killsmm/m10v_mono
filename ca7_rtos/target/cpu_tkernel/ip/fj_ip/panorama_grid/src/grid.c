#include "grid.h"
#include "image.h"
#include "affine.h"
#include "mem.h"

#include <math.h>
#include <stdio.h>

#include "common.h"

#if 0
static
int node2cell(int node)
{
    return node - 1;
}
#endif

static
int cell2node(int cell)
{
    return cell + 1;
}

#if 0
static
int count2idmax(int count)
{
    return count - 1;
}
#endif

void GridPair_Init(GridPair *gridpair, int cellw, int cellh, int dstw, int dsth, int dstx, int dsty, BOOL alloc)
{
    GridCell_Init(&gridpair->cell, dstw, dsth, cellw, cellh);
    int nodew = cell2node(gridpair->cell.count.w);
    int nodeh = cell2node(gridpair->cell.count.h);
    GridNode_Init(&gridpair->src.node, nodew, nodeh, &gridpair->cell.sizemain, alloc);
    GridNode_Init(&gridpair->dst.node, nodew, nodeh, &gridpair->cell.sizemain, alloc);
    gridpair->dstx = dstx;
    gridpair->dsty = dsty;
    if (alloc == TRUE) {
        GridPair_Dst_ComputeCoordAll(gridpair);
    }
}

int  GridPair_Dst_GetCoord_IntX(GridPair *gridpair, int x)
{
    return (x * gridpair->cell.size.w) + gridpair->dstx;
}

int  GridPair_Dst_GetCoord_IntY(GridPair *gridpair, int y)
{
    return (y * gridpair->cell.size.h) + gridpair->dsty;
}

void GridPair_Dst_GetCoord_Int(GridPair *gridpair, int x, int y, int *coordx, int *coordy)
{
    *coordx = GridPair_Dst_GetCoord_IntX(gridpair, x);
    *coordy = GridPair_Dst_GetCoord_IntY(gridpair, y);
}

void GridPair_Dst_GetCoord_Vec2(GridPair *gridpair, int x, int y, Vec2 *coord)
{
    int coordx, coordy;
    GridPair_Dst_GetCoord_Int(gridpair, x, y, &coordx, &coordy);
    coord->x = (XXXX_FLOAT)coordx;
    coord->y = (XXXX_FLOAT)coordy;
}

void GridPair_Dst_ComputeCoordAt(GridPair *gridpair, int x, int y)
{
    Grid *dst = &gridpair->dst;
    int i = GridNode_GetRasterId(&dst->node, x, y);
    GridPair_Dst_GetCoord_Vec2(gridpair, x, y, &dst->node.coord[i]);
}

void GridPair_Dst_ComputeCoordAll(GridPair *gridpair)
{
    Grid *dst = &gridpair->dst;
    for (int y = 0; y < dst->node.size.h; y++) {
        for (int x = 0; x < dst->node.size.w; x++) {
            GridPair_Dst_ComputeCoordAt(gridpair, x, y);
        }
    }
}

void GridPair_Src_ComputeCoordAt(GridPair *gridpair, CameraPair *camerapair, int x, int y)
{
    Grid *src = &gridpair->src;
    int i = GridNode_GetRasterId(&src->node, x, y);
    GridPair_Src_ComputeCoordAt_Raster(gridpair, camerapair, i);
}

void GridPair_Src_ComputeCoordAt_Raster(GridPair *gridpair, CameraPair *camerapair, int i)
{
    Vec2 *dst = &gridpair->dst.node.coord[i];
    Vec2 *src = &gridpair->src.node.coord[i];
    CameraPair_ProjInverse(camerapair, dst, src);
}

void GridPair_Src_ComputeCoordAll(GridPair *gridpair, CameraPair *camerapair)
{
    Grid *src = &gridpair->src;
    for (int i = 0; i < src->node.size.total; i++) {
        GridPair_Src_ComputeCoordAt_Raster(gridpair, camerapair, i);
    }
}

void GridPair_Src_ComputeCoordAll_Step(GridPair *gridpair, CameraPair *camerapair, int stepx, int stepy)
{
    Grid *src = &gridpair->src;
    int xmain = (src->node.size.w - 1) / stepx * stepx;
    int ymain = (src->node.size.h - 1) / stepy * stepy;

    for (int y = 0; y <= ymain; y += stepy) {
        for (int x = 0; x <= xmain; x += stepx) {
            GridPair_Src_ComputeCoordAt(gridpair, camerapair, x, y);
        }
    }

    if (GridCell_IsIncompleteX(&gridpair->cell)) {
        int xstart = xmain + 1;
        for (int y = 0; y < src->node.size.h; y++) {
            for (int x = xstart; x < src->node.size.w; x++) {
                GridPair_Src_ComputeCoordAt(gridpair, camerapair, x, y);
            }
        }
    }

    if (GridCell_IsIncompleteY(&gridpair->cell)) {
        int ystart = ymain + 1;
        for (int y = ystart; y < src->node.size.h; y++) {
            for (int x = 0; x <= xmain; x++) {
                GridPair_Src_ComputeCoordAt(gridpair, camerapair, x, y);
            }
        }
    }
}

void GridPair_Src_ComputeCoordAll_StepInterpolate(GridPair *gridpair, int stepx, int stepy)
{
    Grid *src= &gridpair->src;
    XXXX_FLOAT w0, w1;
    Vec2 *in0, *in1, *out;

    int xmain = (src->node.size.w - 1) / stepx * stepx;
    int ymain = (src->node.size.h - 1) / stepy * stepy;

    if (stepx > 1) {
        for (int y = 0; y <= ymain; y += stepy) {
            for (int x0 = 0; x0 <= xmain - stepx; x0 += stepx) {
                for (int x1 = 1; x1 <= stepx - 1; x1++) {
                    w1 = 1.0f / stepx * x1;
                    w0 = 1.0f - w1;
                    in0 = GridNode_GetCoordPtr(&src->node, x0, y);
                    in1 = GridNode_GetCoordPtr(&src->node, x0 + stepx, y);
                    out = GridNode_GetCoordPtr(&src->node, x0 + x1, y);
                    Vec2_AddWeighted(in0, in1, w0, w1, out);
                }
            }
        }
    }

    if (stepy > 1) {
        for (int x = 0; x <= xmain; x++) {
            for (int y0 = 0; y0 <= ymain - stepy; y0 += stepy) {
                for (int y1 = 1; y1 <= stepy - 1; y1++) {
                    w1 = 1.0f / stepy * y1;
                    w0 = 1.0f - w1;
                    in0 = GridNode_GetCoordPtr(&src->node, x, y0);
                    in1 = GridNode_GetCoordPtr(&src->node, x, y0 + stepy);
                    out = GridNode_GetCoordPtr(&src->node, x, y0 + y1);
                    Vec2_AddWeighted(in0, in1, w0, w1, out);
                }
            }
        }
    }
}

void GridPair_Src_ShiftCoord_CrossingBorderX(GridPair *gridpair, XXXX_FLOAT minx, XXXX_FLOAT shiftx, int gridstepx, int gridstepy)
{
    for (int ny = 0; ny < gridpair->src.node.size.h; ny += gridstepy) {
        for (int nx = 0; nx < gridpair->src.node.size.w; nx += gridstepx) {
            Vec2 *node = GridNode_GetCoordPtr(&gridpair->src.node, nx, ny);
            if (node->x <  minx) {
                node->x += shiftx;
            }
        }
    }
}

void GridPair_Src_ShiftCoordAll(GridPair *gridpair, XXXX_FLOAT shiftx, XXXX_FLOAT shifty)
{
    for (int i = 0; i < gridpair->src.node.size.total; i++) {
        gridpair->src.node.coord[i].x += shiftx;
        gridpair->src.node.coord[i].y += shifty;
    }
}
