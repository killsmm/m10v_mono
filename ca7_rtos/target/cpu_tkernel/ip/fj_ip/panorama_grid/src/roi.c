#include "roi.h"

void Roi_Init(Roi *roi, int x, int y, int w, int h)
{
    Roi_MoveTo(roi, x, y);
    roi->w = w;
    roi->h = h;
}

void Roi_Init_AtImageCenter(Roi *roi, int roiw, int roih, int imagew, int imageh)
{
    int x = imagew / 2 - roiw / 2;
    int y = imageh / 2 - roih / 2;
    Roi_Init(roi, x, y, roiw, roih);
}

void Roi_Copy(Roi *dst, Roi *src)
{
    Roi_Init(dst, src->x, src->y, src->w, src->h);
}

void Roi_MoveTo(Roi *roi, int x, int y)
{
    roi->x = x;
    roi->y = y;
}

void Roi_ExpandLeft(Roi *roi, int length)
{
    roi->x -= length;
    roi->w += length;
}

void Roi_ExpandRight(Roi *roi, int length)
{
    roi->w += length;
}

void Roi_ExpandUp(Roi *roi, int length)
{
    roi->y -= length;
    roi->h += length;
}

void Roi_ExpandDown(Roi *roi, int length)
{
    roi->h += length;
}
