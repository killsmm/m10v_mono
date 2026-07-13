#pragma once

#include "common.h"
#include "coordsys.h"
#include "camera.h"
#include "grid.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        GeoRegionLat_North,
        GeoRegionLat_Border,
        GeoRegionLat_South,
        GeoRegionLat_Count
    } GeoRegionLat;

    GeoRegionLat GeoRegionLat_Get(XXXX_FLOAT lat, XXXX_FLOAT bordernorth, XXXX_FLOAT bordersouth);

    typedef enum {
        GeoRegionLon_FarWest,
        GeoRegionLon_Center,
        GeoRegionLon_FarEast,
        GeoRegionLon_FarWest2FarEast,
        GeoRegionLon_Count
    } GeoRegionLon;

    GeoRegionLon GeoRegionLon_Get(XXXX_FLOAT lon);

    typedef struct {
        GeoRegionLat    lat;
        GeoRegionLon    lon;
    } GeoRegion;

    void GeoRegion_Init(GeoRegion *region, XXXX_FLOAT lat, XXXX_FLOAT lon, XXXX_FLOAT bordernorth, XXXX_FLOAT bordersouth);

    typedef struct {
        GridPair    grid;
        GeoRegion   *region;
        int         regioncount;
        XXXX_FLOAT  bordernorth;
        XXXX_FLOAT  bordersouth;
    } GeoRegionGrid;

    void GeoRegionGrid_Init(GeoRegionGrid *regiongrid, XXXX_FLOAT bordernorth, XXXX_FLOAT bordersouth, int cellstepx, int cellstepy, int cellcountw, int cellcounth);
    GeoRegion *GeoRegionGrid_GetNodePtr(GeoRegionGrid *regiongrid, int nodex, int nodey);
    void GeoRegionGrid_SetNodeFromDst2(GeoRegionGrid *regiongrid, int nodex, int nodey, Vec2 *dst2, CameraPair *camerapair);
    GeoRegion GeoRegionGrid_GetCellRegion(GeoRegionGrid *regiongrid, int cellx, int celly);

#ifdef __cplusplus
}
#endif
