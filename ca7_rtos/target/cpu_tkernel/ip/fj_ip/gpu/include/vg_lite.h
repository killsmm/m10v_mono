/****************************************************************************
*
*    The MIT License (MIT)
*
*    Copyright (c) 2014 - 2016 Vivante Corporation
*
*    Permission is hereby granted, free of charge, to any person obtaining a
*    copy of this software and associated documentation files (the "Software"),
*    to deal in the Software without restriction, including without limitation
*    the rights to use, copy, modify, merge, publish, distribute, sublicense,
*    and/or sell copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    The GPL License (GPL)
*
*    Copyright (C) 2014 - 2016 Vivante Corporation
*
*    This program is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License
*    as published by the Free Software Foundation; either version 2
*    of the License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    recipient may use this file under the terms of either the MIT license or
*    GPL License. If you wish to use only one license not the other, you can
*    indicate your decision by deleting one of the above license notices in your
*    version of this file.
*
*****************************************************************************/


/*!
 @header VGLite API
 The API programs can use to use the VGLite accelerator.

 @copyright Vivante Corporation
 */

#ifndef _vg_lite_h_
#define _vg_lite_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "vg_lite_error.h"

    /* Op code for path: "VLC" means "VGLite Constants" */
#define VLC_OP_END          0x00
#define VLC_OP_CLOSE        0x01
#define VLC_OP_MOVE         0x02
#define VLC_OP_MOVE_REL     0x03
#define VLC_OP_LINE         0x04
#define VLC_OP_LINE_REL     0x05
#define VLC_OP_QUAD         0x06
#define VLC_OP_QUAD_REL     0x07
#define VLC_OP_CUBIC        0x08
#define VLC_OP_CUBIC_REL    0x09

    /* Macros for path manipulating: See path definitions. "VLM" means "VGLite Macros" */
#define VLM_PATH_ENABLE_UPLOAD(path)    (path).uploaded.reserved |= 1
#define VLM_PATH_DISABLE_UPLOAD(path)   (path).uploaded.reserved &= (~1)
#define VLM_PATH_GET_UPLOAD_BIT(path)   ((path).uploaded.reserved & 1)


/*!
 @abstract The floating point type used by the VGLite API.
 */
typedef float vg_lite_float_t;

/*!
 @abstract A 32-bit color value used by the VGLite API.

 @discussion
 The color value specifies the color used in various functions. The color is formed using 8-bit RGBA channels. The red channel
 is in the lower 8-bit of the color value, followed by the green and blue channels. The alpha channel is in the upper 8-bit of
 the color value.

 For L8 target formats, the RGB color is converted to L8 by using the default ITU-R BT.709 conversion rules.
 */
typedef uint32_t vg_lite_color_t;


/*!
 @abstract Quality enumeration for a given path.

 @discussion
 Each path should specify a quality hint for the hardware. The path generation tool will generate the quality hint based on the
 complexity of the path.
 */
typedef enum vg_lite_quality {
    VG_LITE_HIGH,   /*! High quality 16x anti-aliasing path. */
    VG_LITE_MEDIUM, /*! Medium quality 4x anti-aliasing path. */
    VG_LITE_LOW,    /*! Low quality pat without any anti-aliasing. */
} vg_lite_quality_t;

/*!
 @abstract Format of path coordinates.

 @discussion
 Each path can have a separate coordinate system. The path generation tool will find the most optimal coordinate system for any
 given path based on its dimensions and input coordinates.
 */
typedef enum vg_lite_format {
    VG_LITE_S8,     /*! Signed 8-bit coordinates. */
    VG_LITE_S16,    /*! Signed 16-bit coordinates. */
    VG_LITE_S32,    /*! Signed 32-bit coordinates. */
    VG_LITE_FP32,   /*! 32-bit floating point coordinates. */
} vg_lite_format_t;

/*!
 @abstract Format of a buffer.

 @discussion
 The pixel type for a <code>vg_lite_buffer_t</code> structure.
 */
typedef enum vg_lite_buffer_format {
    VG_LITE_RGBA8888,   /*! 32-bit RGBA format with 8 bits per color channel. Red is in bits 7:0, green in bits 15:8, blue in
                         bits 23:16, and the alpha channel is in bits 31:24. */
    VG_LITE_BGRA8888,   /*! 32-bit RGBA format with 8 bits per color channel. Red is in bits 23:16, green in bits 15:8, blue in
                         bits 7:0, and the alpha channel is in bits 31:24. */
    VG_LITE_RGBX8888,   /*! 32-bit RGBX format with 8 bits per color channel. Red is in bits 7:0, green in bits 15:8, blue in
                         bits 23:16, and the x channel is in bits 31:24. */
    VG_LITE_BGRX8888,   /*! 32-bit RGBX format with 8 bits per color channel. Red is in bits 23:16, green in bits 15:8, blue in
                         bits 7:0, and the x channel is in bits 31:24. */
    VG_LITE_RGB565,     /*! 16-bit RGB format with 5 and 6 bits per color channel. Red is in bits 4:0, green in bits 10:5, and
                         the blue color channel is in bits 15:11. */
    VG_LITE_BGR565,     /*! 16-bit RGB format with 5 and 6 bits per color channel. Red is in bits 15:11, green in bits 10:5,
                         and the blue color channel is in bits 4:0. */
    VG_LITE_RGBA4444,   /*! 16-bit RGBA format with 4 bits per color channel. Red is in bits 3:0, green in bits 7:4, blue in
                         bits 11:8 and the alpha channel is in bits 15:12. */
    VG_LITE_BGRA4444,   /*! 16-bit RGBA format with 4 bits per color channel. Red is in bits 11:8, green in bits 7:4, blue in
                         bits 3:0 and the alpha channel is in bits 15:12. */
    VG_LITE_A8,         /*! 8-bit alpha format. There are no RGB values. */
    VG_LITE_L8,         /*! 8-bit luminance value. There is no alpha value. */
    VG_LITE_YUYV,       /*! Packed YUV format, 32-bit for 2 pixels. Y0 is in bits 7:0 and V is in bits 31:23. */

    VG_LITE_YUY2,  /*! New formats. */
    VG_LITE_NV12,
    VG_LITE_ANV12,
    VG_LITE_AYUY2,

    VG_LITE_YV12,
    VG_LITE_YV24,
    VG_LITE_YV16,
    VG_LITE_NV16,

    VG_LITE_YUY2_TILED,  /*! Tiled YUV formats. */
    VG_LITE_NV12_TILED,
    VG_LITE_ANV12_TILED,
    VG_LITE_AYUY2_TILED,
} vg_lite_buffer_format_t;

typedef enum vg_lite_swizzle {
    VG_LITE_SWIZZLE_UV,
    VG_LITE_SWIZZLE_VU,
} vg_lite_swizzle_t;

typedef enum vg_lite_yuv2rgb {
    VG_LITE_YUV601,
    VG_LITE_YUV709,
} vg_lite_yuv2rgb_t;

typedef enum vg_lite_buffer_layout {
    VG_LITE_LINEAR,
    VG_LITE_TILED,
} vg_lite_buffer_layout_t;

typedef enum vg_lite_buffer_image_mode {
    VG_LITE_NORMAL_IMAGE_MODE,
    VG_LITE_NONE_IMAGE_MODE,
    VG_LITE_MULTIPLY_IMAGE_MODE
} vg_lite_buffer_image_mode_t;

typedef struct vg_lite_yuvinfo {
    vg_lite_swizzle_t    swizzle;        // UV swizzle.
    uint32_t    yuv2rgb;        // 601 or 709 conversion standard.
    uint32_t    uv_planar;      // UV(U) planar addres.
    uint32_t    v_planar;       // V planar address
    uint32_t    alpha_planar;   // Alpha planar address.
    uint32_t    uv_stride;      // UV(U) stride.
    uint32_t    v_stride;       // V stride.
    uint32_t    alpha_stride;   // Alpha stride.
    uint32_t    uv_height;      // UV(U) height.
    uint32_t    v_height;       // V height.
    void *    uv_memory;        // The logical pointer to the UV(U) planar memory.
    void *    v_memory;         // The logical pointer to the V planar memory.
    void *    uv_handle;        // The memory handle of the UV(U) planar.
    void *    v_handle;         // The memory handle of the V planar.
} vg_lite_yuvinfo_t;

/*!
 @abstract Blending modes.

 @discussion
 Some of the VGLite API functions calls support blending. S and D represent source and destination color channels and Sa and Da
 represent the source and destination alpha channels.
 */
typedef enum vg_lite_blend {
    VG_LITE_BLEND_NONE,     /*! S, i.e. no blending. */
    VG_LITE_BLEND_SRC_OVER, /*! S + (1 - Sa) * D */
    VG_LITE_BLEND_DST_OVER, /*! (1 - Da) * S + D */
    VG_LITE_BLEND_SRC_IN,   /*! Da * S */
    VG_LITE_BLEND_DST_IN,   /*! Sa * D */
    VG_LITE_BLEND_SCREEN,   /*! S + D - S * D */
    VG_LITE_BLEND_MULTIPLY, /*! S * (1 - Da) + D * (1 - Sa) + S * D */
    VG_LITE_BLEND_ADDITIVE, /*! S + D */
    VG_LITE_BLEND_SUBTRACT, /*! D * (1 - S) */
} vg_lite_blend_t;

/*!
 @abstract Fill rules.

 @discussion
 For drawing any path, the hardware supports both non-zero and odd-even fill rules.

 To determine whether any point is contained inside an object, imagine drawing a line from that point out to infinity in any
 direction such that the line does not cross any vertex of the path. For each edge that is crossed by the line, add 1 to the
 counter if the edge crosses from left to right, as seen by an observer walking across the line towards infinity, and subtract 1
 if the edge crosses from right to left. In this way, each region of the plane will receive an integer value.

 The non-zero fill rule says that a point is inside the shape if the resulting sum is not equal to zero. The even/odd rule says
 that a point is inside the shape if the resulting sum is odd, regardless of sign.
 */
typedef enum vg_lite_fill {
    VG_LITE_FILL_NON_ZERO,  /*! Non-zero fill rule. A pixel is drawn if it crosses at least one path pixel. */
    VG_LITE_FILL_EVEN_ODD,  /*! Even-odd fill rule. A pixel is drawn it it crosses an odd number of path pixels. */
} vg_lite_fill_t;

/* Chip features. */
typedef enum vg_lite_feature
{
    gcFEATURE_BIT_VG_AYUV_INPUT_OUTPUT = 0,
    gcFEATURE_BIT_VG_DOUBLE_IMAGE,
    gcFEATURE_BIT_VG_RECTANGLE_STRIPE_MODE,
    gcFEATURE_BIT_VG_MMU,
    gcFEATURE_BIT_VG_IM_FILTER,
    gcFEATURE_BIT_VG_IM_YUV_PACKET,
    gcFEATURE_BIT_VG_IM_YUV_PLANAR,
    gcFEATURE_BIT_VG_PE_YUV_PACKET,
    gcFEATURE_BIT_VG_COLOR_PRECISION_8_BIT,

    /* Insert features above this comment only. */
    gcFEATURE_COUNT                 /* Not a feature. */
}
vg_lite_feature_t;

/* Filter modes. */
typedef enum vg_lite_filter
{
    VG_LITE_FILTER_POINT = 0,   /* Only the nearest image pixel is fetched. */
    VG_LITE_FILTER_LINEAR = 0x10000,      /* Used for linear paint. */
    VG_LITE_FILTER_BI_LINEAR = 0x20000,   /* Use a 2x2 box around the image pixel and perform an interpolation. */
} vg_lite_filter_t;


/*!
 @abstract A 3x3 matrix.

 @discussion
 For those functions that need a matrix, this is the structure that defines it. The contents are a simple 3x3 matrix
 consisting of floating pointer numbers.
 */
typedef struct vg_lite_matrix {
    vg_lite_float_t m[3][3];    /*! The 3x3 matrix itself, in [row][column] order. */
} vg_lite_matrix_t;

/*!
 @abstract A wrapper structure for any image or render target.

 @discussion
 Each piece of memory, whether it is an image used as a source or a buffer used as a target, requires a structure to define it.
 This structure contains all the information the VGLite API requires to access the buffer's memory by the hardware.
 */
typedef struct vg_lite_buffer {
    int32_t width;                  /*! Width of the buffer in pixels. */
    int32_t height;                 /*! Height of the buffer in pixels. */
    int32_t stride;                 /*! The number of bytes to move from one line in the buffer to the next line. */
    int32_t tiled;                  /*! Indicating the buffer memory layout is linear or tiled. */
    vg_lite_buffer_format_t format; /*! The pixel format of the buffer. */
    void * handle;                  /*! The memory handle of the buffer's memory as allocated by the VGLite kernel. */
    void * memory;                  /*! The logical pointer to the buffer's memory for the CPU. */
    uint32_t address;               /*! The address to the buffer's memory for the hardware. */
    vg_lite_yuvinfo_t       yuv;    /*! The yuv format details. */
    uint32_t image_mode;             /*! The blit image mode. */
} vg_lite_buffer_t;

/* This structure simply records the memory allocation info by kernel. */
typedef struct vg_lite_hw_memory {
    void    * handle;               /* gpu memory object handle. */
    void    * memory;               /* logical memory address. */
    uint32_t  address;              /* GPU memory address. */
    uint32_t  bytes;                /* Size of memory. */
    uint32_t  reserved;             /* Reserved field for any specular use.
                                     Currently bit0 is used for path upload:
                                     1 to enable auto path data uploading;
                                     0 to disable path data uploading (always embedded into command buffer).
                                     */
} vg_lite_hw_memory_t;

/*!
 @abstract A path used by the drawing command.

 @discussion
 Each path needs a few parameters. This structure defines those parameters, so the VGLite driver knows the detail of a path.
 */
typedef struct vg_lite_path {
    vg_lite_float_t bounding_box[4];    /*! Bounding box specified as left, top, right, and bottom. */
    vg_lite_quality_t quality;          /*! Quality hint for the path. */
    vg_lite_format_t format;            /*! Coordinate format. */
    vg_lite_hw_memory_t uploaded;        /*! Pointer to a <code>vg_lite_buffer_t</code> structure if the path data has been
                                         upload into GPU addressable memory. */
    int32_t path_length;                /*! Number of bytes in the path data. */
    void * path;                        /*! Pointer to the physical description of the path. */
    int32_t path_changed;               /* Indicate whether path data is synced with command buffer (uploaded) or not. */
} vg_lite_path_t;

    /* Macros for path manipulating. */
#define VLM_PATH_ENABLE_UPLOAD(path)    (path).uploaded.reserved |= 1
#define VLM_PATH_DISABLE_UPLOAD(path)   (path).uploaded.reserved &= (~1)
#define VLM_PATH_GET_UPLOAD_BIT(path)   ((path).uploaded.reserved & 1)

/*!
 @abstract A rectangle.

 @discussion
 A rectangle defines a rectangular definition of the screen.
 */
typedef struct vg_lite_rectangle {
    int32_t x;      /*! Left coordinate of the rectangle. */
    int32_t y;      /*! Top coordinate of the rectangle. */
    int32_t width;  /*! Width of the rectangle. */
    int32_t height; /*! Height of the rectangle. */
} vg_lite_rectangle_t;

typedef struct vg_lite_tsbuffer_info {
    uint32_t tessellation_buffer_gpu[3];
    uint8_t *tessellation_buffer_logic[3];
    uint32_t tessellation_buffer_size[3];
    uint32_t tessellation_stride;
    uint32_t tessellation_width_height;
    uint32_t tessellation_shift;
    uint32_t tessellation_origin[2];
} vg_lite_tsbuffer_info_t;


/*!
 @abstract Allocate a buffer from hardware accessible memory.

 @discussion
 In order for the hardware to access some memory, like a source image or a target buffer, it needs to be allocated first. The
 supplied <code>vg_lite_buffer_t</code> structure needs to be initialized with the size (width and height) and format of the
 requested buffer. If the stride is set to zero, this function will fill it in.

 This function will call the kernel to actually allocate the memory and the memory handle and logical and hardware addresses
 will be filled in by the kernel.

 @param buffer
 Pointer to the buffer that holds the size and format of the buffer being allocated.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_allocate(vg_lite_buffer_t * buffer);

/*!
 @abstract Free a buffer that was previously allocated by {@link vg_lite_allocate}.

 @discussion
 Free any memory resources allocated by a previous call to {@link vg_lite_allocate}.

 @param buffer
 Pointer to a buffer structure that was filled in by {@link vg_lite_allocate}.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_free(vg_lite_buffer_t * buffer);

/*!
 @abstract Map a buffer into hardware accessible address space.

 @discussion
 If you want the use a frame buffer directly as an target buffer, you need to wrap a <code>vg_lite_buffer_t</code> structure
 around it and call the kernel to map the supplied logical or physical address into hardware accessible memory.

 For example, if you know the logical address of the frame buffer, set the memory field of the vg_lite_buffer_t structure
 with that address and call this function. If you know the physical address, set the memory field to <code>NULL</code> and
 program the address field with the physical address.

 @param buffer
 Pointer to the buffer that holds the size and format of the buffer being allocated. Either the memory or address field
 needs to be set to a non-zero value to map either a logical or physical address into hardware accessible memory.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_map(vg_lite_buffer_t * buffer);

/*!
 @abstract Unmap a buffer that was previously mapped by {@link vg_lite_map}.

 @discussion
 Free any memory resources allocated by a previous call to {@link vg_lite_map}.

 @param buffer
 Pointer to a buffer structure that was filled in by {@link vg_lite_map}.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_unmap(vg_lite_buffer_t * buffer);

/*!
 @abstract Fill a (partial) buffer with a specified color.

 @discussion
 Either an entire buffer or a partial rectangle of a buffer will be filled with a specific color.

 This function will wait until the hardware is complete, i.e. it is synchronous.

 @param target
 Pointer to a <code>vg_lite_buffer_t</code> structure that describes the buffer to be filled.

 @param rectangle
 Pointer to a rectangle that specifies the area to be filled. If <code>rectangle</code> is <code>NULL</code>, the entire target
 buffer will be filled with the specified color.

 @param color
 The color value to use for filling the buffer. If the buffer is in L8 format, the RGBA color will be converted into a
 luminance value.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_clear(vg_lite_buffer_t * target,
                              vg_lite_rectangle_t * rectangle,
                              vg_lite_color_t color);


/*!
 @abstract Copy a source image to the the destination window with a specified matrix that can include translation, rotation,
 scaling, and perspective correction.

 @discussion
 A source image is copied to the target using the specified matrix. If the specified matrix is <code>NULL</code>, an identity
 matrix is assumed, meaning the source will be copied directly on the target at 0,0 location.

 An optional blend mode can be specified that defines the blending of the source onto the target.

 Also, an optional mix color can be specified. The mix color will be multiplied by the source color. If you don't need a mix
 color, set the <code>color</code> parameter to 0.

 Note that on hardware that doesn't support border scissoring (GC355) the blend mode will be forced to
 <code>VG_LITE_BLEND_SRC_OVER</code> if rotation or perspective is involved.

 @param target
 Pointer to a <code>vg_lite_buffer_t</code> structure that describes the target of the blit.

 @param source
 Pointer to a <code>vg_lite_buffer_t</code> structure that describes the source of the blit.

 @param matrix
 Pointer to a 3x3 matrix that defines the transformation matrix of source pixels into the target. If matrix is
 <code>NULL</code>, an identity matrix is assumed.

 @param blend
 The blending mode to be applied to each image pixel. If no blending is required, set this value to
 <code>VG_LITE_BLEND_NONE</code> (0).

 @param color
 If non-zero, this color value will be used as a mix color. The mix color gets multiplied with each source pixel before
 blending happens.

 @param filter
 The filter mode to be applied. If no filter mode is required, set this value to
 <code>VG_LITE_FILTER_BI_LINEAR</code> (0x20000).

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_blit(vg_lite_buffer_t * target,
                             vg_lite_buffer_t * source,
                             vg_lite_matrix_t * matrix,
                             vg_lite_blend_t blend,
                             vg_lite_color_t color,
                             vg_lite_filter_t filter);

/* Blit with 2 image samplers. source0 works as image, Source1 works as background. */
vg_lite_error_t vg_lite_blit2(vg_lite_buffer_t * target,
                             vg_lite_buffer_t * source0,
                             vg_lite_buffer_t * source1,
                             vg_lite_matrix_t * matrix0,
                             vg_lite_matrix_t * matrix1,
                             vg_lite_blend_t blend,
                             vg_lite_filter_t filter);

/*!
 @abstract Initialize a vglite context.

 @discussion
 The {@link vg_lite_draw} function requires a draw context to be initialized. There is only one draw context per process, so
 this function has be called once in your application if any draw command will be used. If this would be the first context that
 accesses the hardware, the hardware will be turned on and initialized.

 The difference between a blit and draw context is that the draw context has a larger command buffer and allocates a
 tessellation buffer for the hardware. The size of the tessellation buffer can be specified, and that size will be aligned to
 the minimum required alignment of the hardware by the kernel. If you make the tessellation buffer smaller, less memory will
 be allocated, but a path might be sent down to the hardware multiple times because the hardware will walk the target with the
 provided tessellation window size, so performance might go down. It is good practice to set the tessellation buffer size to the
 most common path size. For example, if all you do is render up to 24-pt fonts, you can set the tessellation buffer to be
 24x24.

 @param tessellation_width
 The width of the tessellation window.

 @param tessellation_height
 The height of the tessellation window.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_init(int32_t tessellation_width,
                             int32_t tessellation_height);

/*!
 @abstract Destroy a vglite context.

 @discussion
 Destroy a draw context that was previously initialized by {@link vg_lite_draw_init}.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_close(void);

/*!
 @abstract Draw a path to a target buffer.

 @discussion
 The specified path will be transformed by the given matrix and drawn into the specified target buffer using the supplied color.
 Blending can be specified.

 @param target
 Pointer to a <code>vg_lite_buffer_t</code> structure that describes the target of the draw.

 @param path
 Pointer to a <code>vg_lite_path_t</code> structure that describes the path to draw.

 @param fill_rule
 Specified fill rule for the path.

 @param matrix
 Pointer to a 3x3 matrix that defines the transformation matrix of the path. If <code>matrix</code> is <code>NULL</code>, an
 identity matrix is assumed which is usually a bad idea since the path can be anything.

 @param blend
 The blending mode to be applied to each drawn pixel. If no blending is required, set this value to
 <code>VG_LITE_BLEND_NONE</code> (0).

 @param color
 The color applied to each pixel drawn by the path.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_draw(vg_lite_buffer_t * target,
                             vg_lite_path_t * path,
                             vg_lite_fill_t fill_rule,
                             vg_lite_matrix_t * matrix,
                             vg_lite_blend_t blend,
                             vg_lite_color_t color);


/*!
 @abstract Get the value of register from register's address.

 @discussion
 This address will be the AHB Byte address of the register whose value you want to dump.
 Refer to the Vivante AHB Register Specification document for register descriptions.
 The valid range for VGLite cores is usually 0x0 to 0x1FF and 0xA00 to 0xA7F.

 @param address
 Address of register which is needed to get its value.

 @param result
 The register's value.

 */
vg_lite_error_t vg_lite_getinfo(uint32_t address,
                                uint32_t * result);

/*
 Get the name of the VGLite Product.
 */
uint32_t vg_lite_get_product_info(char *name,
                                  uint32_t *chip_id,
                                  uint32_t *chip_rev);

/*!
 @abstract Queried whether the specified feature is available.

 @param feature
 Feature to be verified.

 */
uint32_t vg_lite_query_feature(vg_lite_feature_t feature);

/*!
 @abstract This api explicitly submits the command buffer to GPU and waits for it to complete.

 @param none.
 */
void vg_lite_finish();

/*!
 @abstract This api initializes a path object by given member values.

 @param ...
 */
void vg_lite_init_path(vg_lite_path_t * path,
                       vg_lite_format_t data_format,
                       vg_lite_quality_t quality,
                       uint32_t path_length,
                       void *   path_data,
                       vg_lite_float_t min_x, vg_lite_float_t min_y,
                       vg_lite_float_t max_x, vg_lite_float_t max_y);

/*!
 @abstract This api clears the path member values.
 */
void vg_lite_clear_path(vg_lite_path_t * path);

#if defined (VGLITE_TST_FIRMWARE)
/*!
 @abstract Free a buffer that was pointed to.

 @param logical_base
 Pointer to a buffer.

 @result
 Returns the status as defined by <code>vg_lite_error_t</code>.
 */
vg_lite_error_t vg_lite_free2(void * logical_base);

/*!
 @abstract Allocate a size from hardware accessible memory.

 @param size
 The size of buffer being allocated.

 @result
 Return the logical pointer to the buffer's memory.
 */
void * vg_lite_malloc(const int32_t size);
#endif

/*!
 @abstract Resolve a buffer into another buffer with different formats.
 
 @discussion
 This function is used to convert a buffer to a YUV formatted buffer. Currently
 this is only supported on GC355.
 
 @param target
 The target buffer to convert into.
 
 @param source
 The source buffer to convert from.
 
 @param rectangle
 The rectangle which specifies the x, y origin and width, height of the region
 to convert.
 
 @param src_uv
 The source UV swizzle if it's a YUV format.
 
 @param src_standard
 The source YUV standard.
 
 @param dst_uv
 The destination UV swizzle.
 
 @param dst_standard
 The destination YUV standard.
 
 @param dst_alpha
 Enable or disable the destination Alpha channel.
 */
vg_lite_error_t vg_lite_resolve(vg_lite_buffer_t * target,
                                vg_lite_buffer_t * source,
                                vg_lite_rectangle_t * rectangle,
                                vg_lite_swizzle_t src_uv,
                                vg_lite_yuv2rgb_t src_standard,
                                vg_lite_swizzle_t dst_uv,
                                vg_lite_yuv2rgb_t dst_standard,
                                uint32_t dst_alpha);

int vg_set_heap(unsigned int address, unsigned int size);
void vg_soft_reset(void);

#ifdef __cplusplus
}
#endif
#endif /* _vg_lite_h_ */
