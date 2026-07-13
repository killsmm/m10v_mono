#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        int         w;
        int         h;
        int         total;
    } Size;

    void Size_Init(Size *size, int w, int h);

#ifdef __cplusplus
}
#endif
