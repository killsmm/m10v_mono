#include "georegion.h"
#include "mem.h"

GeoRegionLat GeoRegionLat_Get(XXXX_FLOAT lat, XXXX_FLOAT bordernorth, XXXX_FLOAT bordersouth)
{
    if (lat > bordernorth) {
        return GeoRegionLat_North;
    } else if (lat < bordersouth) {
        return GeoRegionLat_South;
    } else {
        return GeoRegionLat_Border;
    }
}

GeoRegionLon GeoRegionLon_Get(XXXX_FLOAT lon)
{
    if (lon < DEG2RAD(-90)) {
        return GeoRegionLon_FarWest;
    } else if (DEG2RAD(90) < lon) {
        return GeoRegionLon_FarEast;
    } else {
        return GeoRegionLon_Center;
    }
}

void GeoRegion_Init(GeoRegion *region, XXXX_FLOAT lat, XXXX_FLOAT lon, XXXX_FLOAT bordernorth, XXXX_FLOAT bordersouth)
{
    region->lat = GeoRegionLat_Get(lat, bordernorth, bordersouth);
    region->lon = GeoRegionLon_Get(lon);
}

void GeoRegionGrid_Init(GeoRegionGrid *regiongrid, XXXX_FLOAT bordernorth, XXXX_FLOAT bordersouth, int cellstepx, int cellstepy, int cellcountw, int cellcounth)
{
    GridPair_Init(&regiongrid->grid, cellstepx, cellstepy, cellcountw, cellcounth, 0, 0, FALSE);
    regiongrid->regioncount = regiongrid->grid.dst.node.size.total;
    regiongrid->region = Mem_Alloc(regiongrid->regioncount, sizeof(GeoRegion));
    regiongrid->bordernorth = bordernorth;
    regiongrid->bordersouth = bordersouth;
}

GeoRegion *GeoRegionGrid_GetNodePtr(GeoRegionGrid *regiongrid, int nodex, int nodey)
{
    int i = GridNode_GetRasterId(&regiongrid->grid.dst.node, nodex, nodey);
    return &regiongrid->region[i];
}

void GeoRegionGrid_SetNodeFromDst2(GeoRegionGrid *regiongrid, int nodex, int nodey, Vec2 *dst2, CameraPair *camerapair)
{
    Vec3 src3;
    CameraPair_ProjInverse_GetSrc3(camerapair, dst2, &src3);

    CoordGeo in3geo;
    CoordSys_XYZ2Geo(&camerapair->src->pose.coordsys, &src3, &in3geo);

    GeoRegion *region = GeoRegionGrid_GetNodePtr(regiongrid, nodex, nodey);
    GeoRegion_Init(region, in3geo.lat, in3geo.lon, regiongrid->bordernorth, regiongrid->bordersouth);
}

GeoRegion GeoRegionGrid_GetCellRegion(GeoRegionGrid *regiongrid, int cellx, int celly)
{
    int nodex = cellx;
    int nodey = celly;
    GeoRegion *nodeRegion[4];
    nodeRegion[0] = GeoRegionGrid_GetNodePtr(regiongrid, nodex,   nodey  );
    nodeRegion[1] = GeoRegionGrid_GetNodePtr(regiongrid, nodex+1, nodey  );
    nodeRegion[2] = GeoRegionGrid_GetNodePtr(regiongrid, nodex,   nodey+1);
    nodeRegion[3] = GeoRegionGrid_GetNodePtr(regiongrid, nodex+1, nodey+1);

    int nRegionLon[GeoRegionLon_Count] = {0};
    int nRegionLat[GeoRegionLat_Count] = {0};

    for (int i = 0; i < 4; i++) {
        nRegionLat[nodeRegion[i]->lat]++;
        nRegionLon[nodeRegion[i]->lon]++;
    }

    GeoRegion cellregion;
    if (nRegionLat[GeoRegionLat_North] == 4) {
        cellregion.lat = GeoRegionLat_North;
    } else if (nRegionLat[GeoRegionLat_South] == 4) {
        cellregion.lat = GeoRegionLat_South;
    } else {
        cellregion.lat = GeoRegionLat_Border;
    }

    cellregion.lon = GeoRegionLon_Center;
    if ((nRegionLon[GeoRegionLon_FarWest] + nRegionLon[GeoRegionLon_FarEast] == 4)) {
        if ((nRegionLon[GeoRegionLon_FarWest] != 4) && (nRegionLon[GeoRegionLon_FarEast] != 4)) {
            cellregion.lon = GeoRegionLon_FarWest2FarEast;
        }
    }
    return cellregion;
}
