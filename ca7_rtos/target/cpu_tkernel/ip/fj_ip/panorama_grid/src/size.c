#include "size.h"

void Size_Init(Size *size, int w, int h)
{
    size->w = w;
    size->h = h;
    size->total = w * h;
}
