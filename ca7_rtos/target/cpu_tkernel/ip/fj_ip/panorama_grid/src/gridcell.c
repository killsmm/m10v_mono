#include "gridcell.h"

void gridCell_GetCount(int content, int step, int *count, int *rest, int *countmain)    // content = pixel (typically)
{
    int quo = content / step;
    int rem = content % step;

    *count          = quo;  // cell count
    *rest           = rem;  // content count in the last incomplete cell
    *countmain  = (rem == 0) ? quo : (quo + 1); // count of complete part only
}

void GridCell_Init(GridCell *cell, int outw, int outh, int cellw, int cellh)
{
    int countw, counth;
    int restw, resth;
    int mainw, mainh;
    gridCell_GetCount(outw, cellw, &countw, &restw, &mainw);
    gridCell_GetCount(outh, cellh, &counth, &resth, &mainh);

    Size_Init(&cell->size, cellw, cellh);
    Size_Init(&cell->sizemain, mainw, mainh);
    cell->restw = restw;
    cell->resth = resth;
    Size_Init(&cell->count, countw, counth);
}

BOOL GridCell_IsIncompleteX(GridCell *cell)
{
    return (cell->restw == 0) ? FALSE : TRUE;
}

BOOL GridCell_IsIncompleteY(GridCell *cell)
{
    return (cell->resth == 0) ? FALSE : TRUE;
}

int  GridCell_GetSizeTotalX(GridCell *cell)
{
    return cell->size.w * cell->count.w;
}

int  GridCell_GetSizeTotalY(GridCell *cell)
{
    return cell->size.h * cell->count.h;
}

int GridCell_GetRasterId(GridCell *cell, int x, int y)
{
    return x + (y * cell->count.w);
}
