#pragma once

#include "grid.h"
#include "georegion.h"
#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STITCH_SRCCOUNT  (2)

    typedef struct {
        ParamCamera     camerasrc[STITCH_SRCCOUNT];
        ParamCamera     cameradst;
        int             dstw;
        int             dsth;
        int             dstx;
        int             dsty;

        XXXX_FLOAT      dstobjdistance;

        int             cellw;
        int             cellh;
        int             cellingroupw;
        int             cellingrouph;
        int             gridstepx;
        int             gridstepy;

        int             stitchw;
        int             stitchmargin;
    } StitchParam;

    typedef struct {
        Camera          lens[STITCH_SRCCOUNT];
        Camera          erstitchout;
        Camera          erstitchfinal;
    } StitchCamera;

    typedef struct {
        int             gridpaircount;
        GridPair        *gridpair;
        int             camerapaircount;
        CameraPair      *camerapair;
        Vec2            *srcimage2grid;
        int             gridstepx;
        int             gridstepy;
    } StitchGridPair;

    typedef struct {
        XXXX_FLOAT  x;
        XXXX_FLOAT  y;
    } StitchGridVec2;

    typedef struct {

#if 1
        int             nodew;
        int             nodeh;
        uint8           cellw;
        uint8           cellh;
        int             outputx;
        int             outputy;
        StitchGridVec2  *nodes;
        void            *xform;
#else
        int             cellw;
        int             cellh;
        int             nodew;
        int             nodeh;
        StitchGridVec2  *nodes;
        int             outputx;
        int             outputy;
#endif
    } StitchGrid;

    void StitchGrid_Init(StitchGrid *stitchgrid, GridPair *gridpair);

    typedef struct {
        StitchGrid      *grid;
        int             count;
    } StitchGrids;

    void StitchGrids_Init(StitchGrids *stitchgrids, StitchGridPair *gridpair);
    void StitchGrids_Init_All( StitchGrids *stitchgrids, StitchGridPair *gridpair_l, StitchGridPair *gridpair_r );
    void StitchGrids_InitFinal(StitchGrids *stitchgrids, StitchGridPair *gridpair);
    void StitchGrids_Init2(StitchGrids *stitchgrids, GridPair *gridpair);
    void StitchGrids_Init3(StitchGrids *stitchgrids, StitchGrid *gridpair);

    typedef struct {
        StitchCamera    camera;
        StitchGridPair  stitchin[STITCH_SRCCOUNT];
        StitchGridPair  stitchfinal;
        GeoRegionGrid   regiongrid;
        GridPair        finalgrid;
        XXXX_FLOAT      dstobjdistance;
        int             stitchw;
    } Stitch;

    void Stitch_Init(Stitch *stitch, StitchParam *param);
    void Stitch_Compute_StitchIn(Stitch *stitch, StitchGrids *grids0, StitchGrids *grids1);
    void Stitch_Compute_StitchFinal(Stitch *stitch, Affine *poseinworld, StitchGrids *grids);
    void Stitch_Delete(Stitch *stitch);
    void Stitch_DeleteFinal(Stitch *stitch);

#ifdef __cplusplus
}
#endif
