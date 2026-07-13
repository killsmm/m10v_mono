#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        int     x;  // (x,y) = left top position
        int     y;
        int     w;
        int     h;
    } Roi;

    void Roi_Init(Roi *roi, int x, int y, int w, int h);
    void Roi_Init_AtImageCenter(Roi *roi, int roiw, int roih, int imagew, int imageh);
    void Roi_Copy(Roi *dst, Roi *src);
    void Roi_MoveTo(Roi *roi, int x, int y);
    void Roi_ExpandLeft(Roi *roi, int length);
    void Roi_ExpandRight(Roi *roi, int length);
    void Roi_ExpandUp(Roi *roi, int length);
    void Roi_ExpandDown(Roi *roi, int length);

#ifdef __cplusplus
}
#endif
