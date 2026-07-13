#pragma once

#include "common.h"
#include "camera.h"
#include "vec.h"
#include "gridnode.h"
#include "gridcell.h"
#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        GridNode    node;
    } Grid;

    typedef struct {
        Grid        src;
        Grid        dst;
        GridCell    cell;
        int         dstx;
        int         dsty;
    } GridPair;

    void GridPair_Init(GridPair *gridpair, int cellw, int cellh, int dstw, int dsth, int dstx, int dsty, BOOL alloc);

    int  GridPair_Dst_GetCoord_IntX(GridPair *gridpair, int x);
    int  GridPair_Dst_GetCoord_IntY(GridPair *gridpair, int y);
    void GridPair_Dst_GetCoord_Int(GridPair *gridpair, int x, int y, int *coordx, int *coordy);
    void GridPair_Dst_GetCoord_Vec2(GridPair *gridpair, int x, int y, Vec2 *coord);

    void GridPair_Dst_ComputeCoordAt(GridPair *gridpair, int x, int y);
    void GridPair_Dst_ComputeCoordAll(GridPair *gridpair);

    void GridPair_Src_ComputeCoordAt(GridPair *gridpair, CameraPair *camerapair, int x, int y);
    void GridPair_Src_ComputeCoordAt_Raster(GridPair *gridpair, CameraPair *camerapair, int i);
    void GridPair_Src_ComputeCoordAll(GridPair *gridpair, CameraPair *camerapair);
    void GridPair_Src_ComputeCoordAll_Step(GridPair *gridpair, CameraPair *camerapair, int stepx, int stepy);
    void GridPair_Src_ComputeCoordAll_StepInterpolate(GridPair *gridpair, int stepx, int stepy);

    void GridPair_Src_ShiftCoord_CrossingBorderX(GridPair *gridpair, XXXX_FLOAT minx, XXXX_FLOAT shiftx, int gridstepx, int gridstepy);
    void GridPair_Src_ShiftCoordAll(GridPair *gridpair, XXXX_FLOAT shiftx, XXXX_FLOAT shifty);

#ifdef __cplusplus
}
#endif
