#include "image.h"
#include "camera.h"

XXXX_FLOAT Image_GetDefaultCenterX(Image *image)
{
    return image->w / 2.0f;
}

XXXX_FLOAT Image_GetDefaultCenterY(Image *image)
{
    return image->h / 2.0f;
}

void Image_Init(Image *image, int w, int h, XXXX_FLOAT centerx, XXXX_FLOAT centery, XXXX_FLOAT scalepixw, XXXX_FLOAT scaledegw)
{
    image->w = w;
    image->h = h;
    image->centerx = centerx;
    image->centery = centery;
    image->f = scalepixw / DEG2RAD(scaledegw);
}

XXXX_FLOAT Image_Coord_NormalizeX(Image *image, XXXX_FLOAT x, XXXX_FLOAT org)
{
    return -(x - org - image->centerx) / image->f;
}

XXXX_FLOAT Image_Coord_NormalizeY(Image *image, XXXX_FLOAT y, XXXX_FLOAT org)
{
    return -(y - org - image->centery) / image->f;
}

void Image_Coord_Normalize(Image *image, Vec2 *v, Vec2 *org, Vec2 *normalized)
{
    normalized->x = Image_Coord_NormalizeX(image, v->x, org->x);
    normalized->y = Image_Coord_NormalizeY(image, v->y, org->y);
}

XXXX_FLOAT Image_Coord_UnnormalizeX(Image *image, XXXX_FLOAT x, XXXX_FLOAT org)
{
    return (-x * image->f) + image->centerx + org;
}

XXXX_FLOAT Image_Coord_UnnormalizeY(Image *image, XXXX_FLOAT y, XXXX_FLOAT org)
{
    return (-y * image->f) + image->centery + org;
}

void Image_Coord_Unnormalize(Image *image, Vec2 *normalized, Vec2 *org, Vec2 *v)
{
    v->x = Image_Coord_UnnormalizeX(image, normalized->x, org->x);
    v->y = Image_Coord_UnnormalizeY(image, normalized->y, org->y);
}

XXXX_FLOAT Image_GetDefaultCenter(XXXX_FLOAT length)
{
    return length / 2.0f;
}

XXXX_FLOAT Image_GetCenterShift(XXXX_FLOAT length, XXXX_FLOAT center)
{
    return center - Image_GetDefaultCenter(length);
}
