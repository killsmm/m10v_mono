#include "stitch.h"
#include "mem.h"
#include <stdio.h>

void StitchGridPair_Init(StitchGridPair *gridpair, int gridpaircount, int camerapaircount, int gridstepx, int gridstepy);
void StitchGridPair_Compute(StitchGridPair *gridpair, int gridpairid, int camerapair, BOOL cross);

void StitchGridPair_Init(StitchGridPair *gridpair, int gridpaircount, int camerapaircount, int gridstepx, int gridstepy)
{
    gridpair->gridpaircount = gridpaircount;
    gridpair->gridpair      = Mem_Alloc(gridpaircount, sizeof(GridPair));
    gridpair->camerapaircount = camerapaircount;
    gridpair->camerapair    = Mem_Alloc(camerapaircount, sizeof(CameraPair));
    gridpair->srcimage2grid = Mem_Alloc(camerapaircount, sizeof(Vec2));
    gridpair->gridstepx     = gridstepx;
    gridpair->gridstepy     = gridstepy;
}

void StitchGridPair_Compute(StitchGridPair *gridpair, int gridpairid, int camerapair, BOOL cross)
{
    if ((gridpair->gridstepx <= 1) && (gridpair->gridstepy <= 1)) {
        GridPair_Src_ComputeCoordAll(&gridpair->gridpair[gridpairid], &gridpair->camerapair[camerapair]);
        if (cross == TRUE) {
            GridPair_Src_ShiftCoord_CrossingBorderX(&gridpair->gridpair[gridpairid], gridpair->camerapair[camerapair].dst->image.w/2.0f, (XXXX_FLOAT)gridpair->camerapair[camerapair].dst->image.w, gridpair->gridstepx, gridpair->gridstepy);
        }
    } else {
        GridPair_Src_ComputeCoordAll_Step(&gridpair->gridpair[gridpairid], &gridpair->camerapair[camerapair], gridpair->gridstepx, gridpair->gridstepy);
        if (cross == TRUE) {
            GridPair_Src_ShiftCoord_CrossingBorderX(&gridpair->gridpair[gridpairid], gridpair->camerapair[camerapair].dst->image.w/2.0f, (XXXX_FLOAT)gridpair->camerapair[camerapair].dst->image.w, gridpair->gridstepx, gridpair->gridstepy);
        }
        GridPair_Src_ComputeCoordAll_StepInterpolate(&gridpair->gridpair[gridpairid], gridpair->gridstepx, gridpair->gridstepy);
    }
}

void StitchGrid_Init(StitchGrid *stitchgrid, GridPair *gridpair)
{
    stitchgrid->cellw = gridpair->cell.size.w;
    stitchgrid->cellh = gridpair->cell.size.h;
    stitchgrid->nodew = gridpair->src.node.size.w;
    stitchgrid->nodeh = gridpair->src.node.size.h;
    stitchgrid->nodes = (StitchGridVec2 *)gridpair->src.node.coord;
    stitchgrid->outputx = gridpair->dstx;
    stitchgrid->outputy = gridpair->dsty;

#if 1
    stitchgrid->xform = NULL;
    if ( stitchgrid->cellw == 64 ) {
        stitchgrid->cellw = 6;
    }
    if ( stitchgrid->cellh == 32 ) {
        stitchgrid->cellh = 5;
    }
#endif
}

void StitchGrids_Init(StitchGrids *stitchgrids, StitchGridPair *gridpair)
{
    stitchgrids->grid  = Mem_Alloc(gridpair->gridpaircount, sizeof(StitchGrid));
    stitchgrids->count = gridpair->gridpaircount;

    for (int i = 0; i < stitchgrids->count; i++) {
        StitchGrid_Init(&stitchgrids->grid[i], &gridpair->gridpair[i]);
    }
}

void StitchGrids_Init_All( StitchGrids *stitchgrids, StitchGridPair *gridpair_l, StitchGridPair *gridpair_r )
{
    stitchgrids->grid  = Mem_Alloc(3, sizeof(StitchGrid));
    stitchgrids->count = 3;

    StitchGrid_Init(&stitchgrids->grid[0], &gridpair_l->gridpair[0]);
    StitchGrid_Init(&stitchgrids->grid[1], &gridpair_l->gridpair[0]);
    StitchGrid_Init(&stitchgrids->grid[2], &gridpair_r->gridpair[0]);
}

void StitchGrids_InitFinal(StitchGrids *stitchgrids, StitchGridPair *gridpair)
{
    stitchgrids->grid  = Mem_AllocFinal(gridpair->gridpaircount, sizeof(StitchGrid));
    stitchgrids->count = gridpair->gridpaircount;

    for (int i = 0; i < stitchgrids->count; i++) {
        StitchGrid_Init(&stitchgrids->grid[i], &gridpair->gridpair[i]);
    }
}

void StitchGrids_Init2(StitchGrids *stitchgrids, GridPair *gridpair)
{
    stitchgrids->grid  = Mem_Alloc(1, sizeof(StitchGrid));
    stitchgrids->count = 1;
    StitchGrid_Init(stitchgrids->grid, gridpair);
}

void StitchGrids_Init3(StitchGrids *stitchgrids, StitchGrid *gridpair)
{
    stitchgrids->grid  = Mem_Alloc(1, sizeof(StitchGrid));
    stitchgrids->count = 1;

    stitchgrids->grid[0] = *gridpair;
}

void Stitch_Dump(Stitch *stitch)
{
    printf("[Stitch]\n");
    printf("dstobjdistance  = %f\n", stitch->dstobjdistance);
    printf("stitchw         = %d\n", stitch->stitchw);
    
    printf("[StitchCamera]\n");
    printf("[Camera]\n");
}

void Stitch_Init(Stitch *stitch, StitchParam *param)
{
    stitch->stitchw         = param->stitchw;
    stitch->dstobjdistance  = param->dstobjdistance;

    Camera_Init(&stitch->camera.lens[0]      , &param->camerasrc[0], param->dstobjdistance);
    Camera_Init(&stitch->camera.lens[1]      , &param->camerasrc[1], param->dstobjdistance);
    Camera_Init(&stitch->camera.erstitchfinal, &param->cameradst   , param->dstobjdistance);

    // camera for stitch in/out
    {
        ParamCamera paramcamera;

        int w = param->cameradst.image.w;
        int h = param->cameradst.image.h;
        XXXX_FLOAT centerx = param->cameradst.image.centerx;
        XXXX_FLOAT centery = param->cameradst.image.centery;
        XXXX_FLOAT scalepixw = param->cameradst.image.scalepixw;
        XXXX_FLOAT scaledegw = param->cameradst.image.scaledegw;

        ParamCamera_ProjType projtype = ParamCamera_ProjType_Equirectangular;

        PoseAxis pole = stitch->camera.lens[0].pose.front;                      // pole = lens0 front
        PoseAxis front = ProjType_Pole2Front(ProjType_Equirectangular, pole);
        Affine poseinworld;
        Affine_CopyMatVec(&poseinworld, &param->camerasrc[0].poseinworld.m, &param->cameradst.poseinworld.t);

        ParamCamera_Init(&paramcamera, w, h, centerx, centery, scalepixw, scaledegw, projtype, NULL, front, &poseinworld);

        Camera_Init(&stitch->camera.erstitchout  , &paramcamera, param->dstobjdistance);
    }

    {
        Roi roistitchin[2];
        int imageoutw = param->cameradst.image.w;
        int imageouth = param->cameradst.image.h;
        int roiw = imageoutw;       // without stitchmargin
        int roih = param->stitchw;  // without stitchmargin
        Roi_Init_AtImageCenter(&roistitchin[0], roiw, roih, imageoutw, imageouth);
        Roi_ExpandLeft (&roistitchin[0], param->stitchmargin);
        Roi_ExpandRight(&roistitchin[0], param->stitchmargin);
        Roi_Copy(&roistitchin[1], &roistitchin[0]);

        Roi_ExpandUp  (&roistitchin[0], param->stitchmargin);
        Roi_ExpandDown(&roistitchin[1], param->stitchmargin);

        int gridpaircount = 1;
        int camerapaircount = 1;
        int gridstepx = 1;
        int gridstepy = 1;
        for (int i = 0; i < 2; i++) {
            StitchGridPair_Init(&stitch->stitchin[i], gridpaircount, camerapaircount, gridstepx, gridstepy);

            Vec2 srcorg = {.x = 0.0f, .y = 0.0f};
            Vec2 dstorg = {.x = (XXXX_FLOAT)-roistitchin[i].x, .y = (XXXX_FLOAT)-roistitchin[i].y};
            CameraPair_Init(&stitch->stitchin[i].camerapair[0], &stitch->camera.lens[i], &stitch->camera.erstitchout, &srcorg, &dstorg);
            GridPair_Init(&stitch->stitchin[i].gridpair[0], param->cellw, param->cellh, roistitchin[i].w, roistitchin[i].h, 0, 0, TRUE);
        }
    }

    {
        int groupw = param->cellw * param->cellingroupw;
        int grouph = param->cellh * param->cellingrouph;
        int groupcountw = param->cameradst.image.w / groupw;
        int groupcounth = param->cameradst.image.h / grouph;
        int groupcount = groupcountw * groupcounth;

        int camerapaircount = 3;
        StitchGridPair_Init(&stitch->stitchfinal, groupcount, camerapaircount, param->gridstepx, param->gridstepy);

        Vec2 srcorg[GeoRegionLat_Count];
        Vec2 dstorg = {0};
        srcorg[GeoRegionLat_North ].x = 0;
        srcorg[GeoRegionLat_North ].y = 0;
        srcorg[GeoRegionLat_South ].x = (XXXX_FLOAT)stitch->camera.lens[0].image.w;
        srcorg[GeoRegionLat_South ].y = 0;
        srcorg[GeoRegionLat_Border].x = (XXXX_FLOAT)stitch->stitchin[0].camerapair[0].dstorg.x;
        srcorg[GeoRegionLat_Border].y = (XXXX_FLOAT)stitch->stitchin[0].camerapair[0].dstorg.y + stitch->camera.lens[0].image.h;
        CameraPair_Init(&stitch->stitchfinal.camerapair[GeoRegionLat_North ], &stitch->camera.lens[0]    , &stitch->camera.erstitchfinal, &srcorg[GeoRegionLat_North ], &dstorg);
        CameraPair_Init(&stitch->stitchfinal.camerapair[GeoRegionLat_South ], &stitch->camera.lens[1]    , &stitch->camera.erstitchfinal, &srcorg[GeoRegionLat_South ], &dstorg);
        CameraPair_Init(&stitch->stitchfinal.camerapair[GeoRegionLat_Border], &stitch->camera.erstitchout, &stitch->camera.erstitchfinal, &srcorg[GeoRegionLat_Border], &dstorg);

        for (int y = 0; y < groupcounth; y++) {
            for (int x = 0; x < groupcountw; x++) {
                int i = x + (y * groupcountw);
                GridPair_Init(&stitch->stitchfinal.gridpair[i], param->cellw, param->cellh, groupw, grouph, x * groupw, y *grouph, TRUE);
            }
        }

        GridPair_Init(&stitch->finalgrid, param->cellw, param->cellh, param->cameradst.image.w, param->cameradst.image.h, 0, 0, FALSE);

        int cellstepx   = stitch->stitchfinal.gridpair[0].cell.count.w;
        int cellstepy   = stitch->stitchfinal.gridpair[0].cell.count.h;
        int cellcountw  = stitch->finalgrid.cell.count.w;
        int cellcounth  = stitch->finalgrid.cell.count.h;
        XXXX_FLOAT pix2deg = param->cameradst.image.scaledegw / param->cameradst.image.scalepixw;
        XXXX_FLOAT stitchdeg = param->stitchw * pix2deg;
        XXXX_FLOAT stitchrad = DEG2RAD(stitchdeg);
        XXXX_FLOAT bordernorth =  stitchrad / 2;
        XXXX_FLOAT bordersouth = -stitchrad / 2;
        GeoRegionGrid_Init(&stitch->regiongrid, bordernorth, bordersouth, cellstepx, cellstepy, cellcountw, cellcounth);
    }
}

void Stitch_Compute_StitchIn(Stitch *stitch, StitchGrids *grids0, StitchGrids *grids1)
{
    int gridpairid = 0;
    int camerapair = 0;
    for (int i = 0; i < 2; i++) {
        StitchGridPair_Compute(&stitch->stitchin[i], gridpairid, camerapair, FALSE);
    }
//  StitchGrids_Init(grids0, &stitch->stitchin[0]);
//  StitchGrids_Init(grids1, &stitch->stitchin[1]);
    StitchGrids_Init_All(grids0, &stitch->stitchin[0], &stitch->stitchin[1]);
}

void Stitch_Compute_StitchFinal(Stitch *stitch, Affine *poseinworld, StitchGrids *grids)
{
    CameraPair_UpdateDstPose(&stitch->stitchfinal.camerapair[GeoRegionLat_North ], poseinworld, stitch->dstobjdistance);
    CameraPair_UpdateDstPose(&stitch->stitchfinal.camerapair[GeoRegionLat_South ], poseinworld, stitch->dstobjdistance);
    CameraPair_UpdateDstPose(&stitch->stitchfinal.camerapair[GeoRegionLat_Border], poseinworld, stitch->dstobjdistance);

    int snxcount = stitch->regiongrid.grid.dst.node.size.w;
    int snycount = stitch->regiongrid.grid.dst.node.size.h;
    int nxstep = stitch->regiongrid.grid.cell.size.w;
    int nystep = stitch->regiongrid.grid.cell.size.h;
    int nxmax = stitch->finalgrid.dst.node.lastid.x;
    int nymax = stitch->finalgrid.dst.node.lastid.y;
    int xstep = stitch->finalgrid.cell.size.w;
    int ystep = stitch->finalgrid.cell.size.h;

    Vec2 dst2;
    for (int sny = 0; sny < snycount; sny++) {
        int ny = sny * nystep;
        if (ny > nymax) {
            ny = nymax;
        }
        dst2.y = (XXXX_FLOAT)ny * ystep;
        for (int snx = 0; snx < snxcount; snx++) {
            int nx = snx * nxstep;
            if (nx > nxmax) {
                nx = nxmax;
            }
            dst2.x = (XXXX_FLOAT)nx * xstep;

            GeoRegionGrid_SetNodeFromDst2(&stitch->regiongrid, snx, sny, &dst2, &stitch->stitchfinal.camerapair[GeoRegionLat_Border]);
        }
    }

    int cellcountw = stitch->regiongrid.grid.cell.count.w;
    int cellcounth = stitch->regiongrid.grid.cell.count.h;
    for (int celly = 0; celly < cellcounth; celly++) {
        for (int cellx = 0; cellx < cellcountw; cellx++) {
            int cell = GridCell_GetRasterId(&stitch->regiongrid.grid.cell, cellx, celly);
            GeoRegion cellregion = GeoRegionGrid_GetCellRegion(&stitch->regiongrid, cellx, celly);
            BOOL cross = ((cellregion.lat == GeoRegionLat_Border) && (cellregion.lon == GeoRegionLon_FarWest2FarEast)) ? TRUE : FALSE;
            StitchGridPair_Compute(&stitch->stitchfinal, cell, cellregion.lat, cross);
        }
    }

    StitchGrids_InitFinal(grids, &stitch->stitchfinal);
}

void Stitch_Delete(Stitch *stitch)
{
    Mem_Free();
}

void Stitch_DeleteFinal(Stitch *stitch)
{
    Mem_FreeFinal();
}
